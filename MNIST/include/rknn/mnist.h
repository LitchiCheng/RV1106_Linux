// Copyright (c) 2023 by Rockchip Electronics Co., Ltd. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#ifndef _MNIST_H_
#define _MNIST_H_

#include "rknn_api.h"
#include "luckfox_mpi.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#if defined(RV1106_1103) 
    typedef struct {
        char *dma_buf_virt_addr;
        int dma_buf_fd;
        int size;
    }rknn_dma_buf;
#endif


typedef struct {
    rknn_context rknn_ctx;
    rknn_input_output_num io_num;
    rknn_tensor_attr* input_attrs;
    rknn_tensor_attr* output_attrs;
    rknn_tensor_mem* net_mem;
#if defined(RV1106_1103) 
    rknn_tensor_mem* input_mems[1];
    rknn_tensor_mem* output_mems[3];
    rknn_dma_buf img_dma_buf;
#endif
    int model_channel;
    int model_width;
    int model_height;
    bool is_quant;
} rknn_app_context_t;

typedef struct {
	int num;
	float probability;
} detect_result;

int init_mnist_model(const char* model_path, rknn_app_context_t* app_ctx);

int release_mnist_model(rknn_app_context_t* app_ctx);

int inference_mnist_model(rknn_app_context_t* app_ctx, cv::Mat &frame, std::vector<detect_result>& results);

std::vector<cv::Rect> find_contour(const cv::Mat &image, std::vector<cv::Mat>& sub_pics);

#endif //_MNIST_H_