#ifndef SABM_DATA_FILE_H_
#define SABM_DATA_FILE_H_

#include <iostream>
#include <stdio.h>
#include <string.h>
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
        Page *headPage;
        int size_int;
        int numEntries;

    public:
        DSMgr(/* args */);
        ~DSMgr();
        int OpenFile(std::string filename);
        int CloseFile();
        int ScanPage(char *s, int page_id);
        BufferFrame *ReadPage(int page_id);
        void ReadPage(BufferFrame *&frm, int pagePos);
        void WritePage(char *frm, int pagePos);
        int WritePage(int page_id, BufferFrame *frame);
        int Seek(int offset);
        FILE *GetFile();
        void IncNumPages();
        int GetNumPages();
        void SetUse(int index, int use_bit);
        int GetUse(int index);
        //head pages ops
        void SetValue(char *c, int n);
        int GetValue(char *c);
    };
}
#endif