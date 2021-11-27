namespace buffer
{
    struct LRUNode
    {
        /* data */
        int page_id;
        struct LRUNode *pre;
        struct LRUNode *next;
    };
}