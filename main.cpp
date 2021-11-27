#include <iostream>
#include <stdio.h>
#include <buffer/buffer.h>

int main()
{
    buffer::BufferManager bm;
    if(bm.isFull()){
        printf("111");
    }else{
        printf("222");
    }
    return 0;
}