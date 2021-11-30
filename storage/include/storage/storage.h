#include <stdio.h>
#include <string>
#include <string.h>
#include "frame.h"
#include "page.h"

namespace storage 
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
        frame::BufferFrame *ReadPage(int page_id);
        int FileRead(frame::BufferFrame *&frm, int pagePos);
        int FileWrite(char *frm, int pagePos);
        int WritePage(int page_id, frame::BufferFrame *frame);
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