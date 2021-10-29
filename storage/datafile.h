#ifndef SABM_DATA_FILE_H_
#define SABM_DATA_FILE_H_

#include <iostream>
#include <stdio.h>
#include <string>
#include "buffer/frame.h"
#include "page.h"

namespace sabm
{
    class DSMgr
    {
    private:
        /* data */
        FILE *curFile;
        int numPages;
        Page headPage;
    public:
        DSMgr(/* args */);
        ~DSMgr();
        int OpenFile(std::string filename);
        int CloseFile();
        BufferFrame ReadPage(int page_id);
        int WritePage(int frame_id, BufferFrame frame);
        int Seek(int offset, int pos);
        FILE *GetFile();
        void IncNumPages();
        int GetNumPages();
        void SetUse(int index, int use_bit);
        int GetUse(int index);
    };
}
#endif