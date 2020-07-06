//
// Created by KE TU on 2020-07-04.
//

#ifndef YINGKE_SIMPLECODEC_DECODE_MP4_H
#define YINGKE_SIMPLECODEC_DECODE_MP4_H

#include "video_decode.h"

#ifdef __cplusplus
extern "C"{
#endif
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/log.h"
#include <libavutil/imgutils.h>

#ifdef __cplusplus
};
#endif

class Mp4ToYuvDecoder: public VideoDecoder {

private:

    AVFormatContext *pFormatContext = nullptr;
    AVCodecContext *pCodecContext = nullptr;

    AVCodec *pCodec = nullptr;
    AVFrame *pFrame, *pFrameYUV = nullptr;
    AVPacket *pAvPacket = nullptr;
    SwsContext *pSwsContext = nullptr;

    FILE *yuv_file = nullptr;

    int DecodePacket(AVCodecContext *pCodecCtx, AVPacket *pAvPacket, AVFrame *pFrame);

public:

    int initDecoder(const char* video_path);
    int decodeFile(const char* yuv_path);


};

#endif //YINGKE_SIMPLECODEC_DECODE_MP4_H
