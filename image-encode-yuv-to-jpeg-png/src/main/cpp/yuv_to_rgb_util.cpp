//
// Created by KE TU on 2020-07-24.
//

#include "yuv_to_rgb_util.h"
#include <sys/types.h>

/**
 * YUV420P 转 RGB24
 *
 * @param yuv420p_data
 * @param rgb24_data
 * @param width
 * @param height
 */
void YuvToRgb::YUV420P_TO_RGB24(unsigned char *yuv420p_data, unsigned char *rgb24_data, int width,int height) {

    int index = 0;
    // y,u,v 分量在内存中 的 开始地址
    unsigned char *y_base = yuv420p_data;
    unsigned char *u_base = yuv420p_data + width * height;
    unsigned char *v_base = yuv420p_data + width * height * 5 / 4;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // YYYYYYYYUUVV
            // 根据 图像坐标 ，确定 内存中Y，U，V分量位置
            u_char  Y = y_base[x + y * width];
            u_char  U = u_base[y / 2 * width / 2 + (x / 2)];
            u_char  V = v_base[y / 2 * width / 2 + (x / 2)];

            // 转换公式
            rgb24_data[index++] = Y + 1.402 * (V - 128); //R
            rgb24_data[index++] = Y - 0.34413 * (U - 128) - 0.71414 * (V - 128); //G
            rgb24_data[index++] = Y + 1.772 * (U - 128); //B
        }
    }


}