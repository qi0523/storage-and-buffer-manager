namespace storage
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

    struct HeadPage
    {
        /* data */
        Page p;
        struct HeadPage * next;
        HeadPage():next(nullptr){}
    };
}