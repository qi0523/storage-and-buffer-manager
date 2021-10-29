#ifndef SABM_BUFFER_H_
#define SABM_BUFFER_H_

#include <map>
#include "buffer/frame.h"
#include "buffer/bcb.h"

namespace sabm
{

    static const int bufferSize = 1024; // buffer size

    struct LRUNode
    {
        /* data */
        int page_id;
        struct LRUNode *pre;
        struct LRUNode *next;
        LRUNode();
    };

    class BufferManager
    {
    private:
        /* data */
        int curFreeFrame;               //current allign free frame
        int IONums;                     //IO numbers
        BufferFrame buffer[bufferSize]; //buffer arrays
        int frameToPage[bufferSize];    // index from frame_id to page_id
        BCB *hashTable[bufferSize];     //page_id index to frame_id
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

    BufferManager::BufferManager(/* args */)
    {
        this->curFreeFrame = 0;
        this->IONums = 0;
        this->head = new LRUNode();
        this->tail = new LRUNode();
        this->head->next = this->tail;
        this->tail->pre = this->head;
    }

    BufferManager::~BufferManager()
    {
        while (this->head != nullptr)
        {
            /* code */
            LRUNode *p = this->head;
            while (this->head != nullptr)
            {
                this->head = this->head->next;
                delete p;
                p = this->head;
            }
        }
        for (int i = 0; i < bufferSize; i++)
        {
            /* code */
            BCB *curHead = this->hashTable[i];
            if (curHead != nullptr)
            {
                BCB *p = curHead;
                while (curHead != nullptr)
                {
                    /* code */
                    curHead = curHead->next;
                    delete p;
                    p = curHead;
                }
            }
        }
    }

} // namespace sabm

#endif