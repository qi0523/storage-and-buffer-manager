#include <storage/storage.h>

namespace storage
{

    DSMgr::DSMgr(/* args */)
    {
        std::string filename = "";
        if (OpenFile(filename) != 0)
        {
            printf("fail to open the file.\n");
            exit(0);
        }
        size_int = sizeof(int);
        headPage = new Page();
        if (feof(GetFile())) // true denotes file is empty.
        {
            numPages = 0;
            SetValue(headPage->field, 0);
            fwrite(headPage->field, pageSize, 1, GetFile());
        }
        char c[size_int];
        fread(c, size_int, 1, GetFile());
        numPages = GetValue(c);
        fread(headPage->field, pageSize, 1, GetFile());
        numEntries = (pageSize / size_int);
    }

    DSMgr::~DSMgr()
    {
        CloseFile();
    }

    int DSMgr::OpenFile(std::string filename)
    {
        curFile = fopen(filename.c_str(), "r+");
        if (GetFile() == nullptr)
        {
            return -1;
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

    void DSMgr::SetValue(char *c, int n)
    {
        memset(c, 0, size_int);
        for (int i = 0; i < size_int; ++i)
        {
            c[i] = n % 256;
            n >>= 8;
        }
    }

    int DSMgr::GetValue(char *c)
    {
        int res = 0;
        for (int i = size_int - 1; i >= 0; --i)
        {
            res <<= 8;
            res += c[i];
        }
        return res;
    }

    void DSMgr::IncNumPages()
    {
        ++numPages; //insert a page
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
            int cur_page_id = GetValue(s + i * size_int);
            if (page_id == cur_page_id)
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
        //headpage 未找到
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
        return fwrite(frm, pageSize, 1, GetFile());
    }

    int DSMgr::WritePage(int page_id, frame::BufferFrame *frame)
    {
        ++numPages;
        int n = GetValue(headPage->field);
        if (n < numEntries - 1)
        {
            n++;
            SetValue(headPage->field, n);
            SetValue(headPage->field + n * size_int, page_id);
            FileWrite(frame->field, n);
            FileWrite(headPage->field, 0);
        }
        //读取下一个目录页
        frame::BufferFrame *curDirPage = new frame::BufferFrame();
        for (size_t i = 1;; i++)
        {
            // 页号为i * 1024
            if (FileRead(curDirPage, i * numEntries) == 0)
            {
                SetValue(curDirPage->field, 1);
                SetValue(curDirPage->field + size_int, page_id);
                FileWrite(curDirPage->field, i * numEntries);
                return FileWrite(frame->field, i * numEntries + 1);
            }
            int n = GetValue(curDirPage->field);
            if (n < numEntries - 1) //找到不满页
            {
                n++;
                SetValue(curDirPage->field, n);
                SetValue(curDirPage->field + n * size_int, page_id);
                FileWrite(curDirPage->field, i * numEntries);
                return FileWrite(frame->field, i * numEntries + n);
            }
        }
    }

} // namespace sabm
