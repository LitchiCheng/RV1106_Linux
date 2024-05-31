#ifndef __LUCKFOX_MPI_H
#define __LUCKFOX_MPI_H

#include "sample_comm.h"

int vi_dev_init();
int vi_chn_init(int channelId, int width, int height);
int vpss_init(int VpssChn, int width, int height);
int venc_init(int chnId, int width, int height, RK_CODEC_ID_E enType);

#endif