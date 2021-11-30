namespace buffer 
{
    struct BCB
    {
        /* data */
        int page_id;
        int frame_id;
        int latch;
        int count;
        int dirty;
        BCB *next;
        BCB():count(0), dirty(0), next(nullptr){}
    };
}