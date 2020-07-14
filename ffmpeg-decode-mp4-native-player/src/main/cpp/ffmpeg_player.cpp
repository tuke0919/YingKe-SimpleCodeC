//
// Created by KE TU on 2020-07-14.
//

#include "ffmpeg_player.h"
#include <stdio.h>
#include <time.h>

#include "logger.h"

int NativePlayer::native_play(const char *video_path, ANativeWindow *aNativeWindow) {

    av_register_all();

    avFormatContext = avformat_alloc_context();

    if (avformat_open_input(&avFormatContext, video_path, NULL, NULL) != 0) {
        LOGE("Could not open input stream");
        goto end_line;
    }

    if (avformat_find_stream_info(avFormatContext, NULL) < 0){
        LOGE("Could not find stream information");
        goto end_line;
    }

    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        if (avFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            videoIndex = i;
            break;
        }
    }

    if (videoIndex < 0) {
        LOGE("Could not find a video stream");
        goto end_line;
    }

    avCodec = avcodec_find_encoder(avFormatContext->streams[videoIndex]->codecpar->codec_id);
    if (avCodec == NULL) {
        LOGE("could not find codec");
        goto end_line;
    }

    avCodecContext = avcodec_alloc_context3(avCodec);
    avcodec_parameters_to_context(avCodecContext, avFormatContext->streams[videoIndex]->codecpar);

    if (avcodec_open2(avCodecContext, avCodec, NULL) < 0){
        LOGE("Could not open codec");
        goto end_line;
    }

    width = avCodecContext->width;
    height = avCodecContext->height;

    avFrame = av_frame_alloc();
    avPacket = (AVPacket *)av_malloc(sizeof(AVPacket));
    avFrameRGB = av_frame_alloc();

    bufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA, width, height, 1);

    out_buffer = (uint8_t *)av_malloc(bufferSize * sizeof(uint8_t));

    av_image_fill_arrays(avFrameRGB->data, avFrameRGB->linesize, width, height, AV_PIX_FMT_RGBA, SWS_BICUBIC, NULL, NULL, NUL)

    swsContext = sws_getContext(width,
            height,
            avCodecContext->pix_fmt,
            width,
            height,
            AV_PIX_FMT_RGBA, SWS_BICUBIC, NULL, NULL, NULL);


    if (ANativeWindow_setBuffersGeometry(aNativeWindow, width, height, WINDOW_FORMAT_RGBA_8888) < 0){
        LOGE("Could not set buffers geometry");
        ANativeWindow_release(aNativeWindow);
        goto end_line;
    }

    while (av_read_frame(avFormatContext, avPacket) >= 0){
        if (avPacket->stream_index == videoIndex){
            // 视频解码
            if (avcodec_send_packet(avCodecContext, avPacket) != 0) {
                return -1;
            }

            while (avcodec_receive_frame(avCodecContext, avFrame) == 0){
                // 转化格式
                sws_scale(swsContext,
                        (const uint8_t *const *) avFrame->data,
                        avFrame->linesize,
                        0,
                        avCodecContext->height,
                        avFrameRGB->data,
                        avFrameRGB->linesize);

                if (ANativeWindow_lock(aNativeWindow, &windowBuffer, NULL) < 0){
                    LOGE("cannot lock window");
                } else {
                    // 逐行复制  avFrameRGB -> windowBuffer
                    uint8_t *bufferBits = (uint8_t *) windowBuffer.bits;
                    for (int h = 0; h < height; h++) {
                        memcpy(bufferBits + h * windowBuffer.stride * 4,
                               out_buffer + h * avFrameRGB->linesize[0],
                               avFrameRGB->linesize[0]);
                    }
                    ANativeWindow_unlockAndPost(aNativeWindow);
                }
            }
        }
        av_packet_unref(avPacket);
    }

    // 释放内存
    sws_freeContext(swsContext);

    end_line:
    av_free(avPacket);
    av_free(avFrameRGB);
    avcodec_close(avCodecContext);
    avformat_close_input(&avFormatContext);

    return 0;
}