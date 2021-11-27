#include "buffer.h"

namespace sabm
{
    int BufferManager::FixPage(int page_id, int prot)
    {
        // if page is in the buffer
        int pos = Hash(page_id);
        BCB *bcb = GetBCB(page_id);
        if (bcb != nullptr)
            return bcb->frame_id;
        //page is not in the buffer
        int frame_id;
        if (isFull())
        {
            frame_id = SelectVictim();
        }
        frame_id = curFreeFrame;
        // load file page.............
        return frame_id;
    }

    bool BufferManager::isFull()
    {
        return curFreeFrame == bufferSize;
    }

    int BufferManager::Hash(int page_id)
    {
        return page_id % bufferSize;
    }

    BCB *BufferManager::GetBCB(int page_id)
    {
        int pos = Hash(page_id);
        BCB *bcb = hashTable[pos];
        while (bcb != nullptr)
        {
            if (bcb->page_id == page_id)
                return bcb;
            bcb = bcb->next;
        }
        return bcb;
    }

    int BufferManager::SelectVictim()
    {
        LRUNode *ptr = tail->pre;
        while (ptr != head)
        {
            int page_id = ptr->page_id;
            BCB *bcb = GetBCB(page_id);
            // victim condiction
            if (bcb->count == 0 && bcb->dirty == 0)
            {
                // unused and not dirty
                RemoveBCB(bcb, page_id);
                RemoveLRUEle(ptr);
                return bcb->frame_id;
            }
            ptr = ptr->pre;
        }
        ptr = tail->pre;
        while (ptr != head)
        {
            int page_id = ptr->page_id;
            BCB *bcb = GetBCB(page_id);
            // victim condiction
            if (bcb->count == 0 && bcb->dirty == 1)
            {
                // unused and dirty
                RemoveBCB(bcb, page_id);
                RemoveLRUEle(ptr);
                WriteDirtys();
                return bcb->frame_id;
            }
            ptr = ptr->pre;
        }
    }

    void BufferManager::RemoveBCB(BCB *bcb, int page_id)
    {
        int pos = Hash(page_id);
        if (hashTable[pos] == bcb)
        {
            hashTable[pos] = bcb->next;
            delete bcb;
            return;
        }
        BCB *ptr = hashTable[pos];
        while (ptr->next != nullptr && ptr->next != bcb)
        {
            ptr = ptr->next;
        }
        ptr->next = bcb->next;
        delete bcb;
        return;
    }

    void BufferManager::RemoveLRUEle(LRUNode *ptr)
    {
        ptr->pre->next = ptr->next;
        ptr->next->pre = ptr->pre;
        delete ptr;
    }

    void BufferManager::ReadPages()
    {
        //read
        ++IONums;
    }
    void BufferManager::WriteDirtys()
    {
        //write
        ++IONums;
    }
} // namespace sabm
