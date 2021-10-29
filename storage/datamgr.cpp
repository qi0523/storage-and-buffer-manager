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
        if (feof(curFile))
        {
            // true denotes file is empty.
            numPages = 0;
            SetValue(headPage, 0);
        }
        char c[size_int];
        fread(c, size_int, 1, curFile);
        numPages = GetValue(c);
        fread(headPage->field, pageSize, 1, curFile);
    }

    DSMgr::~DSMgr()
    {
    }

    int DSMgr::OpenFile(std::string filename)
    {
        curFile = fopen(filename.c_str(), "r+");
        if (curFile == nullptr)
        {
            return -1;
        }
        return 0;
    }

    void DSMgr::SetValue(Page *p, int n)
    {
        memset(p->field, 0, size_int);
        for (int i = 0; i < size_int; ++i)
        {
            p->field[i] = n % 256;
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
} // namespace sabm
