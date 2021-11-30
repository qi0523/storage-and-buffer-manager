#ifndef _FRAME_H
#define _FRAME_H

namespace frame{

    static const int frameSize = 4096;

    struct BufferFrame
    {
        char field[frameSize];
    };
}
#endif