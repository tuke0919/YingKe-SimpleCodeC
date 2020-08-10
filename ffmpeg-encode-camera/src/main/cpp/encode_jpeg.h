//
// Created by KE TU on 2020-08-10.
//

#ifndef YINGKE_SIMPLECODEC_ENCODE_JPEG_H
#define YINGKE_SIMPLECODEC_ENCODE_JPEG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

#ifdef __cplusplus
}
#endif

class JPEGEncoder {
protected:

    /**
    * 编码开关
    */
    bool transform = false;

    const char *jpegPath;
    int width;
    int height;

    int buffer_size;

    AVFormatContext *pFormatCtx = NULL;
    AVCodecContext *pCodecCtx = NULL;
    AVCodec *pCodec = NULL;
    AVStream *pStream = NULL;
    AVFrame *pFrame = NULL;
    AVPacket avPacket;

    SwsContext *sws_ctx = NULL;
    uint8_t *out_buffer = NULL;

public:

    /**
    * 编码 开关
    * @return
    */
    bool isTransform();

    /**
     * 构造函数
     * @param jpegPath
     * @param width
     * @param height
     */
    JPEGEncoder(const char *jpegPath, int width, int height);

    /**
     * 析构函数
     */
    ~JPEGEncoder();

    /**
     * 编码 成jpeg
     * @param nv21Buffer
     * @return
     */
    int encodeJPEG(unsigned char *nv21Buffer);

};


#endif //YINGKE_SIMPLECODEC_ENCODE_JPEG_H
