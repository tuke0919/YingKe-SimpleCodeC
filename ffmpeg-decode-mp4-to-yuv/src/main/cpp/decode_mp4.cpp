//
// Created by KE TU on 2020-07-04.
//

#include <stdio.h>
#include <time.h>
#include "decode_mp4.h"
#include "logger.h"

int Mp4ToYuvDecoder::initDecoder(const char *video_path) {

    // 1, 注册所有组件
    av_register_all();
    // 2, 创建AVFormatContext结构体
    pFormatContext = avformat_alloc_context();
    // 3, 打开一个输出文件
    if (avformat_open_input(&pFormatContext, video_path, NULL, NULL) != 0){
        LOGE("could not open input stream");
        return -1;
    }

    // 4, 获取媒体信息
    if (avformat_find_stream_info(pFormatContext, NULL) < 0) {
        LOGE("could not find stream information");
        return -1;
    }

    // 5, 获取视频轨 下标
    int videoIndex = -1;
    for (int i = 0; i < pFormatContext->nb_streams; ++i) {
        if (pFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
            videoIndex = i;
            break;
        }
    }
    if (videoIndex == -1){
        LOGE("could not find a video stream");
        return -1;
    }

    // 6. 查找解码器
    pCodec = avcodec_find_decoder(pFormatContext->streams[videoIndex]->codecpar->codec_id);
    if (pCodec == NULL) {
        LOGE("could not find Codec");
        return -1;
    }

    // 7. 配置编码器
    pCodecContext = avcodec_alloc_context3(pCodec);
    avcodec_parameters_to_context(pCodecContext, pFormatContext->streams[videoIndex]->codecpar);
    pCodecContext->thread_count = 1;

    // 8. 打开解码器
    if (avcodec_open2(pCodecContext, pCodec, NULL) < 0){
        LOGE("could not open codec");
        return -1;
    }

    // 9, frame packet 分配内存
    pFrame = av_frame_alloc();
    pFrameYUV = av_frame_alloc();
    int bufferSize = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,
                                              pCodecContext->width,
                                              pCodecContext->height,
                                              1);
    uint8_t *out_buffer = (unsigned char *) av_malloc(bufferSize);
    av_image_fill_arrays(pFrameYUV->data,
                         pFrameYUV->linesize,
                         out_buffer,
                         AV_PIX_FMT_YUV420P,
                         pCodecContext->width,
                         pCodecContext->height, 1);

    pAvPacket = (AVPacket *) av_malloc(sizeof(AVPacket));

    pSwsContext = sws_getContext(pCodecContext->width,
            pCodecContext->height,
            pCodecContext->pix_fmt,
            pCodecContext->width,
            pCodecContext->height,
            AV_PIX_FMT_YUV420P,
            SWS_BICUBIC, NULL, NULL, NULL);

    return 0;
}

/**
 * 解码器 解码 packet 到 frame
 * @param pCodecCtx
 * @param pAvPacket
 * @param pFrame
 * @return
 */
int Mp4ToYuvDecoder::DecodePacket(AVCodecContext *pCodecCtx, AVPacket *pAvPacket, AVFrame *pFrame) {
    // 向解码器 输出 packet数据
    int result = avcodec_send_packet(pCodecCtx, pAvPacket);
    if (result < 0) {
        LOGE("send packet for decoding failed");
        return -1;
    }
    while (!result){
        // 解码器解码 数据到 pFrame
        result = avcodec_receive_frame(pCodecCtx, pFrame);
        if (result == AVERROR(EAGAIN) || result == AVERROR_EOF) {
            return 0;
        } else if (result < 0) {
            LOGE("error during encoding %d", result);
            return -1;
        }

        // 转化成yuv帧
        sws_scale(pSwsContext,
                  (const uint8_t *const *) pFrame->data,
                  pFrame->linesize,
                  0,
                  pCodecCtx->height,
                  pFrameYUV->data,
                  pFrameYUV->linesize);

        int y_size = pCodecCtx->width * pCodecCtx->height;
        fwrite(pFrameYUV->data[0], 1, y_size, yuv_file);      //Y
        fwrite(pFrameYUV->data[1], 1, y_size / 4, yuv_file);  //U
        fwrite(pFrameYUV->data[2], 1, y_size / 4, yuv_file);  //V

        // 释放pFrame
        av_frame_unref(pFrame);
    }
    return 0;
}

/**
 * 解码 文件
 *
 * @param  yuv_path
 * @return
 */
int Mp4ToYuvDecoder::decodeFile(const char *yuv_path) {
    // 打开文件
    yuv_file = fopen(yuv_path, "wb+");
    if (yuv_file == NULL) {
        LOGE("could not open output file");
        return -1;
    }

    // 从 mp4文件 读出packet
    while (av_read_frame(pFormatContext, pAvPacket) >= 0){
        // 解码器 解码 packet
        DecodePacket(pCodecContext, pAvPacket, pFrame);
    }

    // flush 收尾
    DecodePacket(pCodecContext, NULL, pFrame);

    if (pSwsContext != NULL){
        sws_freeContext(pSwsContext);
        pSwsContext = NULL;
    }

    // 关闭文件
    fclose(yuv_file);

    // 关闭解码器 上下文
    if (pCodecContext != NULL) {
        avcodec_close(pCodecContext);
        avcodec_free_context(&pCodecContext);
        pCodecContext = NULL;
    }

    if (pFrame != NULL){
        av_free(pFrame);
        pFrame = NULL;
    }

    if (pFrameYUV != NULL) {
        av_free(pFrameYUV);
        pFrameYUV = NULL;
    }

    if (pFormatContext != NULL){
        avformat_close_input(&pFormatContext);
        avformat_free_context(pFormatContext);
        pFormatContext = NULL;
    }

    return 0;
}













