#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <time.h>
#include <unistd.h>
#include <vector>

#include "rtsp_demo.h"
#include "luckfox_mpi.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <rknn_api.h>

#define WIDTH 640
#define HEIGHT 480
#define TEXT_SIZE   20

int main(int argc, char *argv[]){

	char *option = argv[1];
	printf("%s \r\n", option);
	bool once = false;

	if(strcmp("once", option) == 0){
		once = true;
	}else if(strcmp("while", option) == 0){
		once = false;
	}


    RK_S32 s32Ret = 0;

	int width = WIDTH;
	int height = HEIGHT;

    // CamID：       摄像头 ID
    // hdr_mode：    HDR 模式
    // multi_sensor：多路摄像头
    // iq_dir：      摄像头 ISP 算法配置文件 iqfile 所在目录
	RK_BOOL multi_sensor = RK_FALSE;
	const char *iq_dir = "/etc/iqfiles";
	rk_aiq_working_mode_t hdr_mode = RK_AIQ_WORKING_MODE_NORMAL;
	SAMPLE_COMM_ISP_Init(0, hdr_mode, multi_sensor, iq_dir);
    //运行 ISP 算法
	SAMPLE_COMM_ISP_Run(0);

	// rkmpi init
	if (RK_MPI_SYS_Init() != RK_SUCCESS)
	{
		RK_LOGE("rk mpi sys init fail!");
		return -1;
	}

    // vi init
	vi_dev_init();
	vi_chn_init(0, width, height);

	// vpss init
	vpss_init(0, width, height);

	//绑定 VI 通道到 VPSS 上
	MPP_CHN_S stSrcChn, stvpssChn;
	stSrcChn.enModId = RK_ID_VI;
	stSrcChn.s32DevId = 0;
	stSrcChn.s32ChnId = 0;

	stvpssChn.enModId = RK_ID_VPSS;
	stvpssChn.s32DevId = 0;
	stvpssChn.s32ChnId = 0;

	s32Ret = RK_MPI_SYS_Bind(&stSrcChn, &stvpssChn);
	if (s32Ret != RK_SUCCESS){
		RK_LOGE("bind 0 ch venc failed");
		return -1;
	}

	// venc init
	RK_CODEC_ID_E enCodecType = RK_VIDEO_ID_AVC;
	venc_init(0, width, height, enCodecType);

	VIDEO_FRAME_INFO_S stVpssFrame;

    char test_text[TEXT_SIZE];
	memset(test_text, 0, TEXT_SIZE);
    int count = 0;

    while (1)
	{
		// 获取 VPSS 数据帧和对应内存缓冲块虚拟地址
		s32Ret = RK_MPI_VPSS_GetChnFrame(0, 0, &stVpssFrame, -1);
		if (s32Ret == RK_SUCCESS)
		{
			void *data = RK_MPI_MB_Handle2VirAddr(stVpssFrame.stVFrame.pMbBlk);
			// 读取 VPSS 数据帧后,cv图像处理
			cv::Mat frame(height, width, CV_8UC3, data);
			sprintf(test_text, "test-count: %d", count);
			cv::putText(frame, test_text,
						cv::Point(40, 40),
						cv::FONT_HERSHEY_SIMPLEX, 1,
						cv::Scalar(0, 255, 0), 2);
            bool ret = cv::imwrite("pre.jpg", frame);
            count++;
			memcpy(data, frame.data, width * height * 3);
		}

		// release frame
		s32Ret = RK_MPI_VPSS_ReleaseChnFrame(0, 0, &stVpssFrame);
		if (s32Ret != RK_SUCCESS)
		{
			RK_LOGE("RK_MPI_VI_ReleaseChnFrame fail %x", s32Ret);
		}

		if(once)
			break;
        
	}

	RK_MPI_SYS_UnBind(&stSrcChn, &stvpssChn);

	RK_MPI_VI_DisableChn(0, 0);
	RK_MPI_VI_DisableDev(0);

	RK_MPI_VPSS_StopGrp(0);
	RK_MPI_VPSS_DestroyGrp(0);

	SAMPLE_COMM_ISP_Stop(0);

	RK_MPI_SYS_Exit();

    return 0;
}