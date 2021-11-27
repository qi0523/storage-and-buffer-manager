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
    };
}