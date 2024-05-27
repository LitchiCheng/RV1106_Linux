/*****************************************************************************
* | Author      :   Luckfox team
* | Function    :   
* | Info        :
*
*----------------
* | This version:   V1.0
* | Date        :   2024-04-07
* | Info        :   Basic version
*
******************************************************************************/

#ifndef __LUCKFOX_MPI_H
#define __LUCKFOX_MPI_H

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
#include <unistd.h>

#include "sample_comm.h"

RK_U64 TEST_COMM_GetNowUs();

int vi_dev_init();
int vi_chn_init(int channelId, int width, int height);
int vpss_init(int VpssChn, int width, int height);
int venc_init(int chnId, int width, int height, RK_CODEC_ID_E enType);

#endif