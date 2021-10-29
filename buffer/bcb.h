#ifndef SABM_BCB_H
#define SABM_BCB_H

namespace sabm 
{
    struct BCB
    {
        /* data */
        BCB();
        int page_id;
        int frame_id;
        int latch;
        int count;
        int dirty;
        BCB *next;
    };
}

#endif