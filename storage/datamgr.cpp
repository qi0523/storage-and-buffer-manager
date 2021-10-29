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

} // namespace sabm
