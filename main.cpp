#include <iostream>
#include "./buffer/buffer.h"

int main()
{
    sabm::BufferManager *bm = new sabm::BufferManager();
    std::cout << bm->isFull() << std::endl;
    return 0;
}