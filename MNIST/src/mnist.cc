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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "mnist.h"

static void dump_tensor_attr(rknn_tensor_attr *attr)
{
    printf("  index=%d, name=%s, n_dims=%d, dims=[%d, %d, %d, %d], n_elems=%d, size=%d, fmt=%s, type=%s, qnt_type=%s, "
           "zp=%d, scale=%f\n",
           attr->index, attr->name, attr->n_dims, attr->dims[0], attr->dims[1], attr->dims[2], attr->dims[3],
           attr->n_elems, attr->size, get_format_string(attr->fmt), get_type_string(attr->type),
           get_qnt_type_string(attr->qnt_type), attr->zp, attr->scale);
}

int init_mnist_model(const char *model_path, rknn_app_context_t *app_ctx)
{
    int ret;
    int model_len = 0;
    char *model;
    rknn_context ctx = 0;
    ret = rknn_init(&ctx, (char*)model_path, 0, 0, NULL);
    if (ret < 0)
    {
        printf("rknn_init fail! ret=%d\n", ret);
        return -1;
    }
    // Get Model Input Output Number
    rknn_input_output_num io_num;
    ret = rknn_query(ctx, RKNN_QUERY_IN_OUT_NUM, &io_num, sizeof(io_num));
    if (ret != RKNN_SUCC)
    {
        printf("rknn_query fail! ret=%d\n", ret);
        return -1;
    }
    // printf("model input num: %d, output num: %d\n", io_num.n_input, io_num.n_output);

    // Get Model Input Info
    //printf("input tensors:\n");
    rknn_tensor_attr input_attrs[io_num.n_input];
    memset(input_attrs, 0, sizeof(input_attrs));
    for (int i = 0; i < io_num.n_input; i++)
    {
        input_attrs[i].index = i;
        ret = rknn_query(ctx, RKNN_QUERY_NATIVE_INPUT_ATTR, &(input_attrs[i]), sizeof(rknn_tensor_attr));
        if (ret != RKNN_SUCC)
        {
            printf("rknn_query fail! ret=%d\n", ret);
            return -1;
        }
        dump_tensor_attr(&(input_attrs[i]));
    }

    // Get Model Output Info
    //printf("output tensors:\n");
    rknn_tensor_attr output_attrs[io_num.n_output];
    memset(output_attrs, 0, sizeof(output_attrs));
    for (int i = 0; i < io_num.n_output; i++)
    {
        output_attrs[i].index = i;
        //When using the zero-copy API interface, query the native output tensor attribute
        ret = rknn_query(ctx, RKNN_QUERY_NATIVE_NHWC_OUTPUT_ATTR, &(output_attrs[i]), sizeof(rknn_tensor_attr));
        if (ret != RKNN_SUCC)
        {
            printf("rknn_query fail! ret=%d\n", ret);
            return -1;
        }
        dump_tensor_attr(&(output_attrs[i]));
    }

    // default input type is int8 (normalize and quantize need compute in outside)
    // if set uint8, will fuse normalize and quantize to npu
    input_attrs[0].type = RKNN_TENSOR_UINT8;
    // default fmt is NHWC,1106 npu only support NHWC in zero copy mode
    input_attrs[0].fmt = RKNN_TENSOR_NHWC;
    //printf("input_attrs[0].size_with_stride=%d\n", input_attrs[0].size_with_stride);
    app_ctx->input_mems[0] = rknn_create_mem(ctx, input_attrs[0].size_with_stride);

    // Set input tensor memory
    ret = rknn_set_io_mem(ctx, app_ctx->input_mems[0], &input_attrs[0]);
    if (ret < 0) {
        printf("input_mems rknn_set_io_mem fail! ret=%d\n", ret);
        return -1;
    }

    // Set output tensor memory
    for (uint32_t i = 0; i < io_num.n_output; ++i) {
        app_ctx->output_mems[i] = rknn_create_mem(ctx, output_attrs[i].size_with_stride);
        ret = rknn_set_io_mem(ctx, app_ctx->output_mems[i], &output_attrs[i]);
        if (ret < 0) {
            printf("output_mems rknn_set_io_mem fail! ret=%d\n", ret);
            return -1;
        }
    }

    // Set to context
    app_ctx->rknn_ctx = ctx;

    // TODO
    if (output_attrs[0].qnt_type == RKNN_TENSOR_QNT_AFFINE_ASYMMETRIC)
    {
        app_ctx->is_quant = true;
    }
    else
    {
        app_ctx->is_quant = false;
    }

    app_ctx->io_num = io_num;
    app_ctx->input_attrs = (rknn_tensor_attr *)malloc(io_num.n_input * sizeof(rknn_tensor_attr));
    memcpy(app_ctx->input_attrs, input_attrs, io_num.n_input * sizeof(rknn_tensor_attr));
    app_ctx->output_attrs = (rknn_tensor_attr *)malloc(io_num.n_output * sizeof(rknn_tensor_attr));
    memcpy(app_ctx->output_attrs, output_attrs, io_num.n_output * sizeof(rknn_tensor_attr));

    if (input_attrs[0].fmt == RKNN_TENSOR_NCHW) 
    {
        printf("model is NCHW input fmt\n");
        app_ctx->model_channel = input_attrs[0].dims[1];
        app_ctx->model_height  = input_attrs[0].dims[2];
        app_ctx->model_width   = input_attrs[0].dims[3];
    } else 
    {
        printf("model is NHWC input fmt\n");
        app_ctx->model_height  = input_attrs[0].dims[1];
        app_ctx->model_width   = input_attrs[0].dims[2];
        app_ctx->model_channel = input_attrs[0].dims[3];
    } 

    printf("model input height=%d, width=%d, channel=%d\n",
           app_ctx->model_height, app_ctx->model_width, app_ctx->model_channel);

    return 0;
}

int release_mnist_model(rknn_app_context_t *app_ctx)
{
    if (app_ctx->input_attrs != NULL)
    {
        free(app_ctx->input_attrs);
        app_ctx->input_attrs = NULL;
    }
    if (app_ctx->output_attrs != NULL)
    {
        free(app_ctx->output_attrs);
        app_ctx->output_attrs = NULL;
    }
    for (int i = 0; i < app_ctx->io_num.n_input; i++) {
        if (app_ctx->input_mems[i] != NULL) {
            rknn_destroy_mem(app_ctx->rknn_ctx, app_ctx->input_mems[i]);
        }
    }
    for (int i = 0; i < app_ctx->io_num.n_output; i++) {
        if (app_ctx->output_mems[i] != NULL) {
            rknn_destroy_mem(app_ctx->rknn_ctx, app_ctx->output_mems[i]);
        }
    }
    if (app_ctx->rknn_ctx != 0)
    {
        rknn_destroy(app_ctx->rknn_ctx);

        app_ctx->rknn_ctx = 0;
    }

    printf("Release success\n");
    return 0;
}

static float deqnt_affine_to_f32(int8_t qnt, int32_t zp, float scale) { return ((float)qnt - (float)zp) * scale; }

int inference_mnist_model(rknn_app_context_t* app_ctx, cv::Mat &frame, std::vector<detect_result>& results)
{
    int ret;
    int width = app_ctx->input_attrs[0].dims[2];
	int stride = app_ctx->input_attrs[0].w_stride;

	if (width == stride){
		memcpy(app_ctx->input_mems[0]->virt_addr, frame.data, width * app_ctx->input_attrs[0].dims[1] * app_ctx->input_attrs[0].dims[3]);
	}else{
		int height = app_ctx->input_attrs[0].dims[1];
		int channel = app_ctx->input_attrs[0].dims[3];
		// copy from src to dst with stride
		uint8_t *src_ptr = frame.data;
		uint8_t *dst_ptr = (uint8_t *)app_ctx->input_mems[0]->virt_addr;
		// width-channel elements
		int src_wc_elems = width * channel;
		int dst_wc_elems = stride * channel;
		for (int h = 0; h < height; ++h){
			memcpy(dst_ptr, src_ptr, src_wc_elems);
			src_ptr += src_wc_elems;
			dst_ptr += dst_wc_elems;
		}
	}
   
    ret = rknn_run(app_ctx->rknn_ctx, nullptr);
    if (ret < 0) {
        printf("rknn_run fail! ret=%d\n", ret);
        return -1;
    }

    #define DETECT_NUM_SIZE 10

    // Post Process
    uint8_t  *output= (uint8_t*)malloc(sizeof(uint8_t) * DETECT_NUM_SIZE); 
	float *out_fp32 = (float*)malloc(sizeof(float) * DETECT_NUM_SIZE); 
	output = (uint8_t *)app_ctx->output_mems[0]->virt_addr;

    int32_t zp =  app_ctx->output_attrs[0].zp;
    float scale = app_ctx->output_attrs[0].scale;

	//反量化为浮点数
    for(int i = 0; i < DETECT_NUM_SIZE; i ++)
        out_fp32[i] = deqnt_affine_to_f32(output[i],zp,scale);

    //归一化
    float sum = 0;
    for(int i = 0; i < DETECT_NUM_SIZE; i++)
        sum += out_fp32[i] * out_fp32[i];

	float norm = sqrt(sum);
    for(int i = 0; i < DETECT_NUM_SIZE; i++)
        out_fp32[i] /= norm; 

	//对概率进行排序
	float max_probability = -1.0;
	int detect_num = -1;
	for (int i = 0; i < DETECT_NUM_SIZE; ++i){
		if (out_fp32[i] > max_probability){
			max_probability = out_fp32[i];
			detect_num = i;
		}
	}
	//结果入列
	results.push_back({detect_num, max_probability});
    return 0;
}

std::vector<cv::Rect> find_contour(const cv::Mat &image, std::vector<cv::Mat>& sub_pics) {
	// 预处理图像
    cv::Mat gray, blurred, edged, org_clone;
	org_clone = image.clone();
	// 转成灰度图
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
	// 反相灰度图
	edged = ~gray;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    // 边缘膨胀
	cv::dilate(edged, edged, kernel);
	// 腐蚀
	cv::erode(edged, edged, kernel);
	// 二值化
	cv::threshold(edged, edged, 127, 0, cv::THRESH_TOZERO);
	// 用来存找到的轮廓
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(edged, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
	std::vector<cv::Rect> borders;
	for(auto contour: contours){
		cv::Rect bounding_box = cv::boundingRect(contour);
		//当前矩形框大于30才使用
		if (cv::contourArea(contour) > 30) {
			//扩大矩形框，以防数字被截断
			bounding_box.x = std::max(0, bounding_box.x - 10);
			bounding_box.y = std::max(0, bounding_box.y - 10);
			bounding_box.width = std::min(image.cols - bounding_box.x, bounding_box.width + 20);
			bounding_box.height = std::min(image.rows - bounding_box.y, bounding_box.height + 20);
			borders.push_back(bounding_box);
            cv::Mat postsub;
            cv::Mat sub = edged(bounding_box);
            // 扩大数字轮廓
            cv::threshold(sub, sub, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
            // // 再次二值化
            // cv::threshold(sub, sub, 127, 255, cv::THRESH_BINARY_INV);
            // // 将图片大小调整为28
	        cv::resize(sub, postsub, cv::Size(28, 28), 0, 0, cv::INTER_AREA);
            sub_pics.push_back(postsub);
		}
	}
	return borders;
}