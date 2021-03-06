#include <buffer/buffer.h>

namespace buffer
{
    BufferManager::BufferManager(/* args */)
    {
        this->curFreeFrame = 0;
        this->IONums = 0;
        this->hit = 0;
        this->dirtyNums = 0;
        head = new LRUNode;
        tail = new LRUNode;
        head->next = tail;
        tail->pre = head;
        hashTable = new BCB *[bufferSize];
        for (size_t i = 0; i < bufferSize; i++)
        {
            /* code */
            hashTable[i] = new BCB();
        }

        storgeMgr = new storage::DSMgr();
    }

    BufferManager::~BufferManager()
    {
        if (head != nullptr)
        {
            /* code */
            LRUNode *p = head;
            while (head->next != tail)
            {
                head = head->next;
                delete p;
                p = head;
            }
            delete head;
            delete tail;
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
                    if (p->dirty == 1) //如果是脏页，在系统退出前写回磁盘
                        WriteDirtys(p->page_id, p->frame_id);
                    delete p;
                    p = curHead;
                }
            }
        }
        delete hashTable;
        delete storgeMgr;
        PrintInfo();
    }

    int BufferManager::FixPage(int page_id, int prot)
    {
        // prot = 0：read，prot = 1：write
        //  page is in the buffer？
        BCB *bcb = GetBCB(page_id);
        if (bcb != nullptr)
        {
            //缓冲区存在
            hit++;
            printf("%d,%d in buffer\n", prot, page_id);
            if (prot == 0)
            {
                // read
                bcb->count++;
                bcb->count--;
            }
            else
            {
                bcb->count++;
                bcb->dirty = 1;
                bcb->count--;
            }
            LRU(bcb);
            return bcb->frame_id;
        }
        printf("%d,%d not in buffer\n", prot, page_id);
        // page is not in the buffer
        int frame_id;
        if (!isFull())
            frame_id = (curFreeFrame++);
        else
            frame_id = SelectVictim();
        printf("SelectVictim frame_id: %d\n", frame_id);
        frameToPage[frame_id] = page_id; //设置frame到page的映射
        //设置bcb
        SetBCB(page_id, frame_id, prot);
        // set lru node
        SetLRUEle(page_id);
        if (prot == 0)
        {
            // load file page.............
            printf("%d read\n", page_id);
            ReadPages(page_id, frame_id);
        }
        else
        {
            strcpy(buffer[frame_id].field, "page");
            IONums++;
            printf("%d write\n", page_id);
            storgeMgr->WritePage(page_id, &buffer[frame_id]);
        }
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

    int BufferManager::SelectVictim()
    {
        printf("SelectVictim\n");
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
                WriteDirtys(bcb->page_id, bcb->frame_id);
                RemoveBCB(bcb, page_id);
                RemoveLRUEle(ptr);
                return bcb->frame_id;
            }
            ptr = ptr->pre;
        }
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

    void BufferManager::SetBCB(int page_id, int frame_id, int prot)
    {
        int pos = Hash(page_id);
        BCB *bcb = new BCB();
        if (prot == 1)
            bcb->dirty = 1;
        bcb->page_id = page_id;
        bcb->frame_id = frame_id;
        bcb->next = hashTable[pos]->next;
        hashTable[pos]->next = bcb;
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
    }
    
    void BufferManager::LRU(BCB *b){
        LRUNode *node = address[b->page_id];
        node->pre->next = node->next;
        node->next->pre = node->pre;
        head->next->pre = node;
        node->next = head->next;
        node->pre = head;
        head->next = node;
    }

    void BufferManager::SetLRUEle(int page_id)
    {
        LRUNode *ele = new LRUNode();
        ele->page_id = page_id;
        address[page_id] = ele;
        head->next->pre = ele;
        ele->next = head->next;
        ele->pre = head;
        head->next = ele;
    }

    void BufferManager::RemoveLRUEle(LRUNode *ptr)
    {
        address.erase(ptr->page_id);
        ptr->pre->next = ptr->next;
        ptr->next->pre = ptr->pre;
        delete ptr;
    }

    void BufferManager::ReadPages(int page_id, int frame_id)
    {
        // read
        ++IONums;
        printf("%d ReadPages(int page_id, int frame_id)\n", page_id);
        frame::BufferFrame *frm = storgeMgr->ReadPage(page_id, &buffer[frame_id]);
        if (frm == nullptr)
        {
            printf("%d not in data file\n", page_id);
            FixNewPage(storgeMgr->GetValue(buffer[frame_id].field), frame_id);
            return;
        }
    }

    void BufferManager::FixNewPage(int page_id, int frame_id)
    {
        strcpy(buffer[frame_id].field, "fix new page");
        IONums++;
        storgeMgr->FileWrite(buffer[frame_id].field, page_id);
    }

    void BufferManager::WriteDirtys(int page_id, int frame_id)
    {
        // write
        ++IONums;
        ++dirtyNums;
        printf("WriteDirtys:      %d\n", page_id);
        storgeMgr->WritePage(page_id, &buffer[frame_id]);
    }

    void BufferManager::PrintInfo(){
        printf("IO times: %d\n", IONums);
        printf("hit times: %d\n", hit);
        printf("dirty write times: %d\n", dirtyNums);
    }
} // namespace sabm
