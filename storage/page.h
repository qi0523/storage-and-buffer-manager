#ifndef SABM_PAGE_H
#define SABM_PAGE_H_

namespace sabm
{

    static const int pageSize = 4096;

    /*
    前四个字节表示head_page中的page个数
    */

    struct Page
    {
        /* data */
        char field[pageSize];
    };
}

#endif