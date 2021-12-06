#include <storage/storage.h>

namespace storage
{
    DSMgr::DSMgr(/* args */)
    {
        char *filename = "../data.dbf";
        OpenFile(filename);
        size_int = sizeof(int);
        headPage = new Page();
        char c[size_int];
        if (feof(GetFile())) // file is empty.
        {
            SetValue(c, 0);
            fwrite(c, size_int, 1, GetFile());
            SetValue(headPage->field, 0);
            fwrite(headPage->field, pageSize, 1, GetFile());
        }
        fread(c, size_int, 1, GetFile());
        numPages = GetValue(c);
        fread(headPage->field, pageSize, 1, GetFile());
        numEntries = (pageSize / size_int);
    }

    DSMgr::~DSMgr()
    {
        char c[size_int];
        SetValue(c, numPages);
        printf("numpages:   %d\n", numPages);
        fseek(GetFile(), 0, SEEK_SET);
        fwrite(c, size_int, 1, GetFile());
        FileWrite(headPage->field, 0);
        delete headPage;
        CloseFile();
    }

    int DSMgr::OpenFile(char *filename)
    {
        curFile = fopen(filename, "rb+");
        if (GetFile() == nullptr)
        {
            printf("fail to open the data.dbf file.\n");
            exit(0);
        }
        return 0;
    }

    int DSMgr::CloseFile()
    {
        if (!fclose(GetFile()))
        {
            printf("file is closed successfully.\n");
            return 0;
        }
        return -1;
    }

    void DSMgr::SetValue(char c[], int n)
    {
        memset(c, 0, size_int);
        for (int i = 0; i < size_int; ++i)
        {
            c[i] = n % 128;
            n >>= 7;
        }
        // printf("SetValue: %d, %d, %d, %d\n", c[0], c[1], c[2], c[3]);
    }

    int DSMgr::GetValue(char c[])
    {
        // printf("GetValue: %d, %d, %d, %d\n", c[0], c[1], c[2], c[3]);
        int res = 0;
        for (int i = size_int - 1; i >= 0; --i)
        {
            res <<= 7;
            res += c[i];
        }
        return res;
    }

    void DSMgr::IncNumPages()
    {
        ++numPages; // insert a page
    }

    int DSMgr::GetNumPages()
    {
        return numPages;
    }

    FILE *DSMgr::GetFile()
    {
        return curFile;
    }

    int DSMgr::Seek(int offset)
    {
        return fseek(GetFile(), offset + size_int, SEEK_SET); // 从文件头移动
    }

    int DSMgr::ScanPage(char *s, int page_id)
    {
        int n = GetValue(s); //获取entry数量
        for (int i = 1; i <= n; i++)
        {
            /* code */
            if (page_id == GetValue(s + i * size_int))
            {
                return i;
            }
        }
        return -1;
    }

    int DSMgr::FileRead(frame::BufferFrame *&frm, int pagePos)
    {
        if (frm == nullptr)
            frm = new frame::BufferFrame();
        Seek(pagePos * pageSize);
        return fread(frm->field, pageSize, 1, GetFile());
    }

    frame::BufferFrame *DSMgr::ReadPage(int page_id, frame::BufferFrame *frm)
    {
        //先从headpage查找
        int pagePos = ScanPage(headPage->field, page_id);
        if (pagePos > 0)
        {
            FileRead(frm, pagePos);
            return frm;
        }
        // headpage 未找到
        if (GetValue(headPage->field) < numEntries - 1)
            return nullptr; //文件没有page_id的页
        // 文件未完全扫描
        pagePos = numEntries;
        while (1)
        {
            if (FileRead(frm, pagePos) == 0)
                break; //读取一个空目录页
            int pos = ScanPage(frm->field, page_id);
            if (pos > 0)
            {
                FileRead(frm, pagePos + pos);
                return frm;
            }
            if (GetValue(frm->field) < numEntries - 1)
                break; //文件没有page_id的页
            pagePos += numEntries;
        }
        return nullptr;
    }

    int DSMgr::FileWrite(char *frm, int pagePos)
    {
        Seek(pageSize * pagePos);
        printf("FileWrite pagePos: %d\n", pagePos);
        return fwrite(frm, pageSize, 1, GetFile());
    }

    int DSMgr::WriteScan(int page_id, int &pos)
    {
        pos = ScanPage(headPage->field, page_id);
        if (pos != -1)
            return 0;
        if (GetValue(headPage->field) < numEntries - 1)
            return -1;
        frame::BufferFrame *curDirPage = new frame::BufferFrame();
        for (size_t i = 1;; i++)
        {
            // 页号为i * 1024
            if (FileRead(curDirPage, i * numEntries) == 0)
            {
                delete curDirPage;
                return -1;
            }
            pos = ScanPage(curDirPage->field, page_id);
            if (pos != -1)
            {
                pos += (i * numEntries);
                delete curDirPage;
                return 0;
            }
            if (GetValue(curDirPage->field) < numEntries - 1)
            {
                delete curDirPage;
                return -1;
            }
        }
    }

    int DSMgr::WritePage(int page_id, frame::BufferFrame *frame)
    {
        int pos;
        int status = WriteScan(page_id, pos);
        if (status == -1)
        {
            ++numPages;
            printf("wirte page nums: %d\n", numPages);
            int dirPages = numPages / numEntries;
            pos = numPages % numEntries;
            if(pos == 0){
                Page *p= new Page();    //目录页
                SetValue(p->field, 1);
                SetValue(p->field + size_int, page_id);
                FileWrite(p->field, dirPages * numPages);
                delete p;
                ++pos;
            }
            return FileWrite(frame->field, dirPages * numEntries + pos);
        }
        else
        {
            return FileWrite(frame->field, pos);
        }
    }
} // namespace sabm
