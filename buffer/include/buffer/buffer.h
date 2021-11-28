#include <map>
#include "bcb.h"
#include "lru.h"
#include <frame/frame.h>

namespace buffer
{

    class BufferManager
    {
    private:
        /* data */
        int curFreeFrame;               //current allign free frame
        int IONums;                     //IO numbers
        static const int bufferSize = 1024; // buffer size
        static frame::BufferFrame buffer[bufferSize]; //buffer arrays
        int frameToPage[bufferSize];    // index from frame_id to page_id
        BCB **hashTable;     //page_id index to frame_id
        LRUNode *head;
        LRUNode *tail;
        std::map<int, LRUNode *> address;

    public:
        BufferManager(/* args */);
        ~BufferManager();
        //buffer manager functions
        int FixPage(int page_id, int prot);
        void FixNewPage();
        int UnfixPage(int page_id);
        int NumFreeFrames();

        //Internal functions
        bool isFull();
        int SelectVictim();
        int Hash(int page_id);
        BCB *GetBCB(int page_id);
        void RemoveBCB(BCB *ptr, int page_id);
        void RemoveLRUEle(LRUNode *ptr);
        void SetDirty(int frame_id);
        void UnsetDirty(int frame_id);
        void ReadPages();
        void WriteDirtys();
        void PrintFrame(int frame_id);
    };

} // namespace sabm