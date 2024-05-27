#include "v4l2CapPicTool.h"

v4l2CapPicTool::v4l2CapPicTool(std::string dev, int width, 
            int height, std::string pic_format):_dev(dev),_pic_width(width),
                                            _pic_height(height){
    if ("jpg" == pic_format){
        _pic_format = V4L2_PIX_FMT_MJPEG;
    }else if("yuyv" == pic_format){
        _pic_format = V4L2_PIX_FMT_YUYV;
    }else if("uyvy" == pic_format){
		_pic_format = V4L2_PIX_FMT_UYVY;
	}else if("nv12" == pic_format){
		_pic_format = V4L2_PIX_FMT_NV12;
	}
}

v4l2CapPicTool::~v4l2CapPicTool(){
    if(_v4l2_fd >= 0){
        close(_v4l2_fd);
    }
}

int v4l2CapPicTool::init(){
    int ret;
    std::string _err_str;
	_v4l2_fd = open(_dev.c_str(), O_RDWR); 
	if (-1 == _v4l2_fd){
        _err_str = "open " + _dev + " failed";
		goto FAIL;
	}

	struct v4l2_format format;
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
	format.fmt.pix.width = _pic_width;              
	format.fmt.pix.height = _pic_height;            
	format.fmt.pix.pixelformat = _pic_format;           
	ret = ioctl(_v4l2_fd, VIDIOC_S_FMT, &format);
	if (-1 == ret){
        _err_str = "VIDIOC_S_FMT failed";
		goto FAIL;
	}

	ret = ioctl(_v4l2_fd, VIDIOC_G_FMT, &format);     // Get
	if (-1 == ret){
        _err_str = "VIDIOC_G_FMT failed";
		goto FAIL;
	}

	if (format.fmt.pix.pixelformat != _pic_format){
        _err_str = "set pixelformat failed";
		goto FAIL;
	}

	struct v4l2_requestbuffers reqbuf;
	reqbuf.count = 2;                       
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
	reqbuf.memory = V4L2_MEMORY_MMAP;
	ret = ioctl(_v4l2_fd, VIDIOC_REQBUFS, &reqbuf);
	if (-1 == ret){
        _err_str = "VIDIOC_REQBUFS failed";
		goto FAIL;
	}

	_buff.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	_buff.memory = V4L2_MEMORY_MMAP;
    _buff.index = 0;
    ret = ioctl(_v4l2_fd, VIDIOC_QUERYBUF, &_buff);
    if (-1 == ret){
        _err_str = "VIDIOC_QUERYBUF failed";
		goto FAIL;
    }
        
    _buffer_datas = (unsigned char*)mmap(NULL, _buff.length, PROT_READ, MAP_SHARED, _v4l2_fd, _buff.m.offset);
    if (MAP_FAILED == _buffer_datas){
        _err_str = "mmap failed";
        goto FAIL;
    }

    std::cout << "init success" << std::endl;
    return 0;
FAIL:
    close(_v4l2_fd);
    _v4l2_fd = -1;
    std::cout << _err_str << std::endl;
    return -1;
}

int v4l2CapPicTool::capture(){
	int ret;
    int on = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
    ret = ioctl(_v4l2_fd, VIDIOC_QBUF, &_buff);
	if (-1 == ret){
		_err_str = "VIDIOC_QBUF failed";
		goto FAIL;
	}
	
	ret = ioctl(_v4l2_fd, VIDIOC_STREAMON, &on);
	if (-1 == ret){
		_err_str = "VIDIOC_STREAMON failed";
		goto FAIL;
	}

	ret = ioctl(_v4l2_fd, VIDIOC_DQBUF, &_buff);
	if (-1 == ret){
        _err_str = "VIDIOC_DQUF failed";
		goto FAIL;
	}
    return 0;
FAIL:
    close(_v4l2_fd);
    _v4l2_fd = -1;
    std::cout << _err_str << std::endl;
    return -1;
}

int v4l2CapPicTool::save(std::string file_path){
    FILE *fl;
	fl = fopen(file_path.c_str(), "w");
	if (NULL == fl){
        _err_str = "open file_path:" + file_path + " failed.";
        std::cout << _err_str << std::endl;
        return -1;
	}
	fwrite(_buffer_datas, _buff.bytesused, 1, fl);
	fclose(fl);
    return 0;
}

int v4l2CapPicTool::picSize(){
	return _buff.bytesused;
}

unsigned char* v4l2CapPicTool::picBuffer(){
	return _buffer_datas;
}
