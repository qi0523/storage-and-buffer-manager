#ifndef SABM_FRAME_H
#define SABM_FRAME_H_

namespace sabm{

    static const int frameSize = 4096;

    struct BufferFrame
    {
        char field[frameSize];
    };
}

#endif