//
// Created by KE TU on 2020-07-24.
//

#ifndef YINGKE_SIMPLECODEC_YUV_TO_PNG_UTIL_H
#define YINGKE_SIMPLECODEC_YUV_TO_PNG_UTIL_H
#include <sys/types.h>

namespace YuvToPng{
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
    int  RGB24_ENCODE_TO_PNG(const char *pngPath, u_char *rgb_data, int width, int height, int quality);

    /**
     * 外部 调用
     * @param yuvPath  yuv文件路径
     * @param pngPath  png文件路径
     * @param width    宽
     * @param height   高
     */
    void YUV420P_TO_PNG(const char *yuvPath, const char *pngPath, int width, int height);
}


#endif //YINGKE_SIMPLECODEC_YUV_TO_PNG_UTIL_H
