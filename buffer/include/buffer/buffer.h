#include <map>
#include "bcb.h"
#include "lru.h"
#include <../../storage/include/storage/storage.h>

namespace buffer
{
    class BufferManager
    {
    private:
        /* data */
        int curFreeFrame;               //current allign free frame
        int IONums;                     //IO numbers
        int hit;
        int dirtyNums;
        static const int bufferSize = 1024; // buffer size
        frame::BufferFrame buffer[bufferSize]; //buffer arrays
        int frameToPage[bufferSize];    // index from frame_id to page_id
        BCB **hashTable;     //page_id index to frame_id
        LRUNode *head;
        LRUNode *tail;
        std::map<int, LRUNode *> address;
        storage::DSMgr * storgeMgr;
    public:
        BufferManager(/* args */);
        ~BufferManager();
        //buffer manager functions
        int FixPage(int page_id, int prot);
        void FixNewPage(int page_id, int frame_id); //page_id:  pos
        int UnfixPage(int page_id);
        int NumFreeFrames();
        
        //Internal functions
        bool isFull();
        int SelectVictim();
        int Hash(int page_id);
        BCB *GetBCB(int page_id);
        void SetBCB(int page_id, int frame_id, int prot);
        void RemoveBCB(BCB *ptr, int page_id);
        void LRU(BCB *b);
        void SetLRUEle(int page_id);
        void RemoveLRUEle(LRUNode *ptr);
        void SetDirty(int frame_id);
        void UnsetDirty(int frame_id);
        void ReadPages(int page_id, int frame_id);
        void WriteDirtys(int page_id, int frame_id);
        void PrintInfo();
    };
} // namespace sabm