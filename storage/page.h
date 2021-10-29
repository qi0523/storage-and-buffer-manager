#ifndef SABM_PAGE_H
#define SABM_PAGE_H_

#include <stdio.h>

namespace sabm
{

    static const int pageSize = 4096;

    /*
    前四个字节表示head_page中的page个数
    */

    struct Page
    {
    private:
        /* data */
        char field[pageSize];
    public:
        Page(/* args */);
        Page(FILE *f, int page_id);
        ~Page();
        int GetOffset();
    };

    Page::Page(/* args */)
    {
    }

    Page::Page(/* args */ FILE *f, int page_id)
    {
        fseek(f, page_id * pageSize, 0); //0表示从文件头，偏移量page_id * pageSize
        fread(field, pageSize, 1, f);
    }

    Page::~Page()
    {
    }



}

#endif