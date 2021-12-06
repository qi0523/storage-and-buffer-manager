#include <buffer/buffer.h>

int main()
{
    FILE *f = fopen("../data-5w-50w-zipf.txt", "r");
    if (f == NULL)
    {
        printf("data-5w-50w-zipf.txt is not opened.\n");
        exit(0);
    }
    buffer::BufferManager *bm  = new buffer::BufferManager();
    int rw = -1, page_id = -1;
    fscanf(f, "%d,%d", &rw, &page_id);
    int c;
    while ((c = fgetc(f)) != EOF)
    {
        bm->FixPage(page_id, rw);
        fscanf(f, "%d,%d", &rw, &page_id);
    }
    bm->PrintInfo();
    delete bm;
    return 0;
}