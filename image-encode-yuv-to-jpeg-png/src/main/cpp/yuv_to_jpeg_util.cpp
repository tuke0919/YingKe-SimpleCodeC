//
// Created by KE TU on 2020-07-24.
//
#include <stdio.h>
#include "yuv_to_jpeg_util.h"
#include "logger.h"

// jpeg 头文件
#include "jpeglib.h"
// 转rgb 头文件
#include "yuv_to_rgb_util.h"


/**
 * 外部调用
 * 流程： yuv文件 -> yuv_data --(YuvToRgb工具)--> rgb_data --(libjpeg-turbo库)--> jpeg
 *
 * @param yuvPath  源地址 yuv文件地址
 * @param jpegPath 目的地址jpeg路径
 * @param width
 * @param height
 */
void YuvToJpeg::YUV420P_TO_JPEG(const char *yuvPath, const char *jpegPath, int width, int height) {
    FILE *file = fopen(yuvPath, "rb");
    // yuv_data的长度
    int yuv_data_length = width * height * 3 / 2;
    // 创建空间
    u_char  *yuv_data = new u_char[yuv_data_length];
    // 读yuv文件 到yuv_data 空间
    fread(yuv_data, 1, yuv_data_length, file);

    // rgb24 data的长度
    int rgb24_data_length = width * height * 3;
    // 创建 rgb24的空间
    u_char *rgb24_data = new u_char[rgb24_data_length];

    // yuv_data -> rgb24 转换
    YuvToRgb::YUV420P_TO_RGB24(yuv_data, rgb24_data, width, height);

    // 此时 rgb24_data 中已经有数据, 压缩编码 成jpeg

    // rgb24 内存数据 编码压缩 成jpeg图像
    RGB24_ENCODE_TO_JPEG(jpegPath, rgb24_data, width, height, 100);

    // 释放空间
    delete[] yuv_data;
    delete[] rgb24_data;

    fclose(file);
}

/**
 * rgb24 内存数据 编码压缩 成jpeg图像
 *
 * @param jpegPath
 * @param rgb_data
 * @param width
 * @param height
 * @param quality
 * @return
 */
int  YuvToJpeg::RGB24_ENCODE_TO_JPEG(const char *jpegPath, u_char *rgb_data, int width, int height, int quality) {

    jpeg_compress_struct jpegCompressStruct;
    jpeg_error_mgr jpegErrorMgr;

    jpegCompressStruct.err = jpeg_std_error(&jpegErrorMgr);

    // 初始化压缩 对象
    jpeg_create_compress(&jpegCompressStruct);

    // 打开文件 准备写入
    FILE *file = fopen(jpegPath, "wb");
    if (file == NULL){
        return -1;
    }
    // 关联 压缩对象 和 目的文件
    jpeg_stdio_dest(&jpegCompressStruct, file);


    // 设置 压缩默认参数
    jpegCompressStruct.client_data = &file;
    jpegCompressStruct.image_width = width;
    jpegCompressStruct.image_height = height;
    jpegCompressStruct.input_components = 3;
    jpegCompressStruct.in_color_space = JCS_RGB;
    jpeg_set_defaults(&jpegCompressStruct);

    // 设置 亮度和色度 质量
    jpegCompressStruct.q_scale_factor[0] = jpeg_quality_scaling(100);
    jpegCompressStruct.q_scale_factor[1] = jpeg_quality_scaling(100);

    // 设置图像采样率 默认 2x2
    jpegCompressStruct.comp_info[0].v_samp_factor = 2;
    jpegCompressStruct.comp_info[1].h_samp_factor = 2;

    // 设置 压缩质量
    jpeg_set_quality(&jpegCompressStruct, quality, TRUE);

    // 开始压缩
    jpeg_start_compress(&jpegCompressStruct, TRUE);

    // 图像一行像素采样 的指针
    JSAMPROW row_pointer[1];

    // 从上到下,设置每一个像素
    for (int i = 0; i < jpegCompressStruct.image_height; i++) {
        // 每一行 像素的开始 地址
        row_pointer[0] = rgb_data + i * jpegCompressStruct.image_width * 3;

        // 把每一行的 像素写入
        jpeg_write_scanlines(&jpegCompressStruct, row_pointer, 1);
    }

    jpeg_finish_compress(&jpegCompressStruct);
    fclose(file);
    jpeg_destroy_compress(&jpegCompressStruct);

    return 0;
}

