//
// Created by KE TU on 2020-07-24.
//

#ifndef YINGKE_SIMPLECODEC_YUV_TO_JPEG_UTIL_H
#define YINGKE_SIMPLECODEC_YUV_TO_JPEG_UTIL_H

#include <sys/types.h>

namespace YuvToJpeg{

    /**
     * rgb24 内存数据 编码压缩 成jpeg图像
     *
     * @param jpegPath dst jpeg文件名
     * @param rgb_data src rgb数据
     * @param width
     * @param height
     * @param quality 质量
     * @return
     */
    int  RGB24_ENCODE_TO_JPEG(const char *jpegPath, u_char *rgb_data, int width, int height, int quality);

    /**
     * 外部 调用
     * @param yuvPath  yuv文件路径
     * @param jpegPath jpeg文件路径
     * @param width    宽
     * @param height   高
     */
    void YUV420P_TO_JPEG(const char *yuvPath, const char *jpegPath, int width, int height);


}


#endif //YINGKE_SIMPLECODEC_YUV_TO_JPEG_UTIL_H
