//
// Created by KE TU on 2020-07-14.
//
#include <jni.h>
#include <stdio.h>
#include <time.h>

#include "ffmpeg_player.h"
#include "logger.h"

// 模仿native_play.cpp 的但是不知道那里错，avcodec_open2 一直返回-22，暂时使用native_play.cpp吧

int NativePlayer::native_play(const char *video_path, ANativeWindow *aNativeWindow) {
    int ret = 0;

    // 1. 注册所有组件
    av_register_all();
    // 2，创建avFormatContext 上下文
    avFormatContext = avformat_alloc_context();
    // 3，打开输入文件
    if (avformat_open_input(&avFormatContext, video_path, NULL, NULL) != 0) {
        LOGE("Could not open input stream");
        goto end_line;
    }
    // 4，查找文件 流信息
    if (avformat_find_stream_info(avFormatContext, NULL) < 0) {
        LOGE("Could not find stream information");
        goto end_line;
    }
    // 5，查找视频轨道
    for (int index = 0; index < avFormatContext->nb_streams; index++) {
        if (avFormatContext->streams[index]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = index;
            break;
        }
    }
    if (videoIndex == -1) {
        LOGE("Could not find a video stream");
        goto end_line;
    }
    // 6，查找解码器
    avCodec = avcodec_find_decoder(avFormatContext->streams[videoIndex]->codecpar->codec_id);
    if (avCodec == NULL) {
        LOGE("could not find codec");
        goto end_line;
    }
    // 7，创建分配 解码器上下文
    avCodecContext = avcodec_alloc_context3(avCodec);
    // 8，拷贝解码器参数 到 解码器上下文
    avcodec_parameters_to_context(avCodecContext, avFormatContext->streams[videoIndex]->codecpar);


    // 9，打开解码器
    ret = avcodec_open2(avCodecContext, avCodec, NULL);

    LOGE("avcodec_open2 ret = %d", ret);
    LOGE("avcodec_open2 str = %s", av_err2str(ret));

    if (ret < 0){
        LOGE("Could not open codec");
        goto end_line;
    }
    // 10，视频宽高
    width = avCodecContext->width;
    height = avCodecContext->height;

    // 11，分配 frame 和packet 空间
    avFrame = av_frame_alloc();
    avPacket = (AVPacket *)av_malloc(sizeof(AVPacket));
    avFrameRGB = av_frame_alloc();

    // 12， 绑定输出buffer
    bufferSize = av_image_get_buffer_size(AV_PIX_FMT_RGBA, width, height, 1);
    out_buffer = (uint8_t *)av_malloc(bufferSize * sizeof(uint8_t));

    av_image_fill_arrays(avFrameRGB->data, avFrameRGB->linesize, out_buffer, AV_PIX_FMT_RGBA, width, height, 1);

    // 13, 图像格式转换上下文
    swsContext = sws_getContext(width,
            height,
            avCodecContext->pix_fmt,
            width,
            height,
            AV_PIX_FMT_RGBA, SWS_BICUBIC, NULL, NULL, NULL);


    // 14，修改 windowbuffer 大小 和 像素格式
    if (ANativeWindow_setBuffersGeometry(aNativeWindow, width, height, WINDOW_FORMAT_RGBA_8888) < 0){
        LOGE("Could not set buffers geometry");
        ANativeWindow_release(aNativeWindow);
        goto end_line;
    }

    // 15，循环读Packet
    while (av_read_frame(avFormatContext, avPacket) >= 0){
        if (avPacket->stream_index == videoIndex){
            // 16， 把packet 传进 解码器
            if (avcodec_send_packet(avCodecContext, avPacket) != 0) {
                return -1;
            }
            // 17， 获取 解码后的图像帧
            while (avcodec_receive_frame(avCodecContext, avFrame) == 0){
                // 18， 转化格式 把avFrame 转化到 avFrameRGB
                sws_scale(swsContext,
                        (const uint8_t *const *) avFrame->data,
                        avFrame->linesize,
                        0,
                        avCodecContext->height,
                        avFrameRGB->data,
                        avFrameRGB->linesize);
                // 19, Lock the window's next drawing surface for writing. 锁住 window下一个surface
                if (ANativeWindow_lock(aNativeWindow, &windowBuffer, NULL) < 0){
                    LOGE("cannot lock window");
                } else {
                    // 20，逐行复制  avFrameRGB -> windowBuffer out_buffer
                    uint8_t *bufferBits = (uint8_t *) windowBuffer.bits;
                    for (int h = 0; h < height; h++) {
                        memcpy(bufferBits + h * windowBuffer.stride * 4,
                               out_buffer + h * avFrameRGB->linesize[0],
                               avFrameRGB->linesize[0]);
                    }
                    // 21，解锁 window的surface 并post去显示
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
    avcodec_free_context(&avCodecContext);
    avformat_close_input(&avFormatContext);

    return 0;
}