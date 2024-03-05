#ifndef __V4L2_CAP_PIC_TOOL_H__
#define __V4L2_CAP_PIC_TOOL_H__

#include <stdio.h>
#include <unistd.h>

//linux system need
#include <sys/types.h>                      
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
//for C++
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <iostream>
//linux v4l2 head file
#include <linux/videodev2.h>   

class v4l2CapPicTool
{
public:
    v4l2CapPicTool(std::string dev, int width, 
                int height, std::string pic_format);
    ~v4l2CapPicTool();
    int init();
    int capture();
    int save(std::string path);

    int picSize();
    unsigned char* picBuffer();

private:
    std::string _dev{"/dev/video0"};
    int _pic_width{1080};
    int _pic_height{960};
    uint32_t _pic_format{V4L2_PIX_FMT_JPEG};

    int _v4l2_fd{-1};
    unsigned char* _buffer_datas;
    struct v4l2_buffer _buff;

    std::string _err_str{""};


};

#endif  //__V4L2_CAP_PIC_TOOL_H__
