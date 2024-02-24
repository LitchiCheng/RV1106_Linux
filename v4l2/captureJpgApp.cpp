#include <iostream>

#include "v4l2CapPicTool.h"

int main(int argc, char const *argv[])
{
    v4l2CapPicTool vt("/dev/video0", 1080, 960, "jpg");
    vt.init();
    while (1)
    {
        usleep(100000);
        vt.capture();
        vt.save("./test.jpg");
    }
    return 0;
}
