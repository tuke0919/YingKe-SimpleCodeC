//
// Created by KE TU on 2020-07-24.
//
#include "logger.h"
#include <stdio.h>
#include <string.h>
#include "yuv_to_png_util.h"


// 转rgb 头文件
#include "yuv_to_rgb_util.h"

// png的头文件
#include "png.h"
#include "zlib.h"



/**
 * 外部 调用
 * 流程： yuv文件 -> yuv_data --(YuvToRgb工具)--> rgb_data --(libpng库)--> png
 *
 * @param yuvPath  yuv文件路径
 * @param pngPath  png文件路径
 * @param width    宽
 * @param height   高
 */
void YuvToPng::YUV420P_TO_PNG(const char *yuvPath, const char *pngPath, int width, int height){

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

    // 此时 rgb24_data 中已经有数据, 压缩编码 成png

    // rgb24 内存数据 编码压缩 成png图像
    RGB24_ENCODE_TO_PNG(pngPath, rgb24_data, width, height, 100);

    // 释放空间
    delete[] yuv_data;
    delete[] rgb24_data;

    fclose(file);

}



/**
   * rgb24 内存数据 编码压缩 成png图像
   *
   * @param pngPath  dst png文件名
   * @param rgb_data src rgb数据
   * @param width
   * @param height
   * @param quality 质量
   * @return
   */
int  YuvToPng::RGB24_ENCODE_TO_PNG(const char *pngPath, u_char *rgb_data, int width, int height, int quality){

    FILE *fp;
    // png 的两个结构
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;

    // 打开文件，准备写入
    fp = fopen(pngPath, "wb");
    if (fp == NULL){
        return -1;
    }

    // 创建 png结构指针 和png信息指针
    png_ptr  = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    info_ptr = png_create_info_struct(png_ptr);
    if (png_ptr == NULL || info_ptr == NULL) {
        fclose(fp);
        return -1;
    }

    // 关联 png指针 和 文件
    png_init_io(png_ptr, fp);

    // 设置 压缩参数
    png_set_IHDR(png_ptr, info_ptr,
            width,
            height,
            8,
            PNG_COLOR_TYPE_RGB,
            PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_BASE,
            PNG_FILTER_TYPE_BASE);

    // 写info
    png_write_info(png_ptr, info_ptr);
    // 设置pack
    png_set_packing(png_ptr);

    // 一行像素数据 空间
    uint8_t *raw_data = new uint8_t[width * 3];
    memset(raw_data, 255, width * 3);

    int nv_start = width * height;
    uint32_t rgb_index = 0, pos;
    int r, g, b, nv_index, y, ru, rv;

    // 从上到下，扫描每个像素
    for (uint32_t i = 0; i < height; ++i) {
        for (uint32_t j = 0; j < width; ++j) {

            raw_data[j * 3 + 0] = rgb_data[rgb_index++];
            raw_data[j * 3 + 1] = rgb_data[rgb_index++];
            raw_data[j * 3 + 2] = rgb_data[rgb_index++];
        }
        // 写入一行的像素数据
        png_write_row(png_ptr, raw_data);
    }

    delete raw_data;
    // 写结束
    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    // 关闭文件
    fclose(fp);

    return 0;
}











