//
// Created by KE TU on 2020-07-24.
//

#ifndef YINGKE_SIMPLECODEC_YUV_TO_RGB_UTIL_H
#define YINGKE_SIMPLECODEC_YUV_TO_RGB_UTIL_H

namespace YuvToRgb {

    /**
     * YUV420P 转 RGB24
     *
     * @param yuv420p_data
     * @param rgb24_data
     * @param width
     * @param height
     */
    void YUV420P_TO_RGB24(unsigned char *yuv420p_data, unsigned char *rgb24_data, int width, int height);
}


#endif //YINGKE_SIMPLECODEC_YUV_TO_RGB_UTIL_H
