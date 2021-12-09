#include <storage/storage.h>

namespace storage
{
    DSMgr::DSMgr(/* args */)
    {
        char *filename = "../data.dbf";
        OpenFile(filename);
        size_int = sizeof(int);
        numEntries = (pageSize / size_int);
        headPage = new HeadPage();
        char ch = fgetc(GetFile());
        if (ch == EOF) // file is empty.
        {
            numPages = 0;
            SetValue(headPage->p.field, 0);
        }
        else
        {
            fseek(GetFile(), 0, SEEK_SET);
            char c[size_int];
            fread(c, size_int, 1, GetFile());
            numPages = GetValue(c);
            fread(headPage->p.field, pageSize, 1, GetFile());
            int dirNums = numPages / (numEntries - 1);
            if (numPages % (numEntries - 1) != 0)
                ++dirNums;
            HeadPage *q = headPage;
            for (size_t i = 1; i < dirNums; i++)
            {
                /* code */
                HeadPage *ptr = new HeadPage();
                Seek(i * numEntries * pageSize);
                fread(ptr->p.field, pageSize, 1, GetFile());
                q->next = ptr;
                q = q->next;
            }
        }
    }

    DSMgr::~DSMgr()
    {
        char c[size_int];
        SetValue(c, numPages);
        fseek(GetFile(), 0, SEEK_SET);
        fwrite(c, size_int, 1, GetFile());
        int i = 0;
        HeadPage *p = headPage;
        while (headPage != nullptr)
        {
            /* code */
            Seek((i++) * numEntries * pageSize);
            fwrite(headPage->p.field, pageSize, 1, GetFile());
            headPage = headPage->next;
            delete p;
            p = headPage;
        }
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
    }

    int DSMgr::GetValue(char c[])
    {
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

    int DSMgr::Scan(int page_id, int &pos)
    {
        pos = 0;
        HeadPage *ptr = headPage;
        while (ptr != nullptr)
        {
            int n = GetValue(ptr->p.field);
            for (int i = 1; i <= n; i++)
            {
                if (page_id == GetValue(ptr->p.field + i * size_int))
                {
                    pos += i;
                    return 0;
                }
            }
            if (n < numEntries - 1)
            {
                SetValue(ptr->p.field, ++n);
                SetValue(ptr->p.field + n * size_int, page_id);
                pos += n;
                return -1;
            }
            pos += numEntries;
            if(ptr->next == nullptr){
                ptr->next = new HeadPage();
                SetValue(ptr->next->p.field, 1);
                SetValue(ptr->next->p.field + size_int, page_id);
                ++pos;
                return -1;
            }
            ptr = ptr->next;
        }
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
        int pos;
        int status = Scan(page_id, pos);
        if (status == 0)
        {
            FileRead(frm, pos);
            return frm;
        }else{
            SetValue(frm->field, pos);
            IncNumPages();
            return nullptr;
        }
    }

    int DSMgr::FileWrite(char *frm, int pagePos)
    {
        Seek(pageSize * pagePos);
        printf("FileWrite pagePos: %d\n", pagePos);
        fwrite(frm, pageSize, 1, GetFile());
        return fflush(GetFile());
    }

    int DSMgr::WritePage(int page_id, frame::BufferFrame *frame)
    {
        int pos;
        int status = Scan(page_id, pos);
        if (status != 0)
        {
            IncNumPages();
        }
        return FileWrite(frame->field, pos);
    }
} // namespace sabm
