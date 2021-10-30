#include "datafile.h"

namespace sabm
{

    DSMgr::DSMgr(/* args */)
    {
        std::string filename = "";
        int statusCode = OpenFile(filename);
        if (statusCode != 0)
        {
            std::cout << "fail to open the file" << std::endl;
            exit(0);
        }
        size_int = sizeof(int);
        headPage = new Page();
        if (feof(GetFile()))
        {
            // true denotes file is empty.
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

    void DSMgr::ReadPage(BufferFrame *&frm, int pagePos)
    {
        if (frm == nullptr)
            frm = new BufferFrame();
        Seek(pagePos * pageSize);
        fread(frm->field, pageSize, 1, GetFile());
    }

    BufferFrame *DSMgr::ReadPage(int page_id)
    {
        //先从headpage查找
        BufferFrame *frm = nullptr;
        int pagePos = ScanPage(headPage->field, page_id);
        if (pagePos > 0)
        {
            ReadPage(frm, pagePos);
            return frm;
        }
        //headpage 未找到
        if (GetValue(headPage->field) < numEntries - 1)
            return frm; //文件没有page_id的页
        // 文件未完全扫描
        int pagePos = numEntries;
        while (1)
        {
            ReadPage(frm, pagePos);
            int n = GetValue(frm->field);
            if (n == 0)
                break;
            int pos = ScanPage(frm->field, page_id);
            if (pos > 0)
            {
                ReadPage(frm, pagePos + pos);
                return frm;
            }
            if (n < numEntries - 1)
                break; //文件没有page_id的页
            pagePos += numEntries;
        }
        return nullptr;
    }

    void DSMgr::WritePage(char *frm, int pagePos)
    {
        Seek(pageSize * pagePos);
        fwrite(frm, pageSize, 1, GetFile());
    }

    int DSMgr::WritePage(int page_id, BufferFrame *frame)
    {
        ++numPages;
        int n = GetValue(headPage->field);
        if (n < numEntries - 1)
        {
            n++;
            SetValue(headPage->field, n);
            SetValue(headPage->field + n * size_int, page_id);
            WritePage(frame->field, n);
            WritePage(headPage->field, 0);
        }
    }

} // namespace sabm
