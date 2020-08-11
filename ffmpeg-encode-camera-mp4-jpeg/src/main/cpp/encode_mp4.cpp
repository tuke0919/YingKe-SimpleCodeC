//
// Created by KE TU on 2020-08-10.
//

#include <libyuv/convert.h>
#include <libyuv.h>
#include "logger.h"
#include "encode_mp4.h"

/**
 * 编码开关
 * @return
 */
bool Mp4Encoder::isTransform() {
    return this->transform;
}

/**
 * 初始化
 * @param mp4Path
 * @param width
 * @param height
 */
void Mp4Encoder::initEncoder(const char *mp4Path, int width, int height) {
    LOGI("initEncoder");
    LOGI("mp4Path = %s, width = %d, height = %d", mp4Path, width, height);

    this->mp4Path = mp4Path;
    this->width  = width;
    this->height = height;
}

/**
 * 编码开始
 */
void Mp4Encoder::encodeStart() {
    LOGI("encodeStart");

    //1. 注册所有组件
    av_register_all();

    //2. 初始化输出码流的AVFormatContext
    avformat_alloc_output_context2(&avFormatContext, NULL, NULL, this->mp4Path);

    //3. 打开待输出的视频文件
    if (avio_open(&avFormatContext->pb, this->mp4Path, AVIO_FLAG_READ_WRITE)){
        LOGE("open output file failed");
        return;
    }

    //4. 初始化视频码流
    avStream = avformat_new_stream(avFormatContext, NULL);
    if (avStream == NULL) {
        LOGE("allocation output stream failed");
        return;
    }

    //5. 寻找编码器并打开编码器
    avCodec = avcodec_find_encoder(AV_CODEC_ID_MPEG4);
    if (!avCodec){
        LOGE("could not find encoder");
        return;
    }

    //6. 分配编码器并设置参数
    avCodecContext = avcodec_alloc_context3(avCodec);
    // 设置视频编码器ID
    avCodecContext->codec_id = avCodec->id;
    // 编码器类型
    avCodecContext->codec_type = AVMEDIA_TYPE_VIDEO;
    // 设置读取像素格式
    avCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;
    // 注意宽高
    avCodecContext->width = height;
    avCodecContext->height = width;
    // 帧率越大越流畅
    // fps = 25fps
    avCodecContext->time_base.num = 1;
    avCodecContext->time_base.den = 35;

    // 视频码率的计算方式：视频文件大小/视频时间 码率越大，视频越大
    // 比特率400kbps
    avCodecContext->bit_rate = 468000;

    // I帧约少，视频越小。但过分的少，会导致视频编码失败，所以要适量。
    // gop大小
    avCodecContext->gop_size = 6;

    // 设置B帧最大值 设置为0，表示不需要B帧
    avCodecContext->max_b_frames = 0;

    // 设置量化参数（难点，我们一般设置默认值）
    // 量化系数越小，视频越是清晰。一般情况下都是默认值，最小量化系数默认值是10，最大量化系数默认值是51
    avCodecContext->qmin = 5;
    avCodecContext->qmax = 41;



    //将AVCodecContext的成员复制到AVCodecParameters结构体
    avcodec_parameters_from_context(avStream->codecpar, avCodecContext);
    av_stream_set_r_frame_rate(avStream, {1, 35});

    //7. 打开编码器
    if (avcodec_open2(avCodecContext, avCodec, NULL) < 0){
        LOGE("open encoder fail!");
        return;
    }

    //输出格式信息
    av_dump_format(avFormatContext, 0, this->mp4Path, 1);

    //初始化帧
    avFrame = av_frame_alloc();
    avFrame->width = avCodecContext->width;
    avFrame->height = avCodecContext->height;
    avFrame->format = avCodecContext->pix_fmt;

    int image_buffer_size = av_image_get_buffer_size(avCodecContext->pix_fmt,
            avCodecContext->width,
            avCodecContext->height,
            1);
    pFrameBuffer = (uint8_t*)(av_malloc(image_buffer_size));
    av_image_fill_arrays(avFrame->data,
            avFrame->linesize,
            pFrameBuffer,
            avCodecContext->pix_fmt,
            avCodecContext->width,
            avCodecContext->height,
            1);

    AVDictionary *opt = 0;

    //H.264
    if (avCodecContext->codec_id == AV_CODEC_ID_H264) {
        av_dict_set_int(&opt, "video_track_timescale", 35, 0);
        av_dict_set(&opt, "preset", "slow", 0);
        av_dict_set(&opt, "tune", "zerolatency", 0);
    }

    //8. 写文件头
    avformat_write_header(avFormatContext, &opt);

    //创建已编码帧
    av_new_packet(&avPacket, image_buffer_size * 3);

    // 标记开始 转换
    this->transform = true;
    LOGI("开始编码: transform = true");
}

/**
 * 编码 nv21 buffer
 * @param nv21Buffer
 */
void Mp4Encoder::encodeBuffer(unsigned char *nv21Buffer){
    LOGI("encodeBuffer");

    uint8_t *i420_y = pFrameBuffer;
    uint8_t *i420_u = pFrameBuffer + width * height;
    uint8_t *i420_v = pFrameBuffer + width * height * 5 / 4;

    // NV21转I420(420P) 并旋转270
    // 前置摄像头，旋转270 libyuv::kRotate270,
    // 后置摄像头，旋转90
    libyuv::ConvertToI420(nv21Buffer, width * height,
            i420_y, height,
            i420_u, height / 2,
            i420_v, height/ 2,
            0, 0,
            width, height,
            width, height,
            libyuv::kRotate90,
            libyuv::FOURCC_NV21);

    // 赋值给 图像帧结构
    avFrame->data[0] = i420_y;
    avFrame->data[1] = i420_u;
    avFrame->data[2] = i420_v;
    //AVFrame PTS
    avFrame->pts = index++;

    // 编码这一帧图像
    encodeFrame(avCodecContext, avFrame, &avPacket);
    LOGI("encodeBuffer: success ");
}

/**
 * 编码 一帧图像
 * @param pCodecCtx
 * @param pAvFrame
 * @param pAvPacket
 * @return
 */
int Mp4Encoder::encodeFrame(AVCodecContext *pCodecCtx, AVFrame *pAvFrame, AVPacket *pAvPacket){
    LOGI("encodeFrame");

    // 向编码器 发送原始 图像帧
    int  ret = avcodec_send_frame(pCodecCtx, pAvFrame);
    if (ret < 0){
        LOGE("failed to send frame for encoding");
        return -1;
    }

    while (!ret) {
        // 从编码器 获取 编码后的图像帧
        ret = avcodec_receive_packet(pCodecCtx, pAvPacket);
        LOGI("ret = %d, ret str = %s ", ret, av_err2str(ret));

        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            LOGE("receive_packet str = %s", av_err2str(ret));
            return 0;
        } else if (ret < 0) {
            LOGE("error during encoding = %s", av_err2str(ret));
            return -1;
        }

        LOGI("Write packed %d, size=%d ", pAvPacket->pts, pAvPacket->size);
        // packet在stream的位置
        pAvPacket->stream_index = avStream->index;
        // 转换时间域
        av_packet_rescale_ts(pAvPacket, pCodecCtx->time_base, avStream->time_base);

        pAvPacket->pos = -1;
        // 向媒体文件 写packet
        av_interleaved_write_frame(avFormatContext, pAvPacket);
        av_packet_unref(pAvPacket);
    }

    LOGI("encodeFrame: success ");
    return 0;
}

/**
 * 编码开始
 */
void Mp4Encoder::encodeStop(){
    LOGI("encodeStop");

    //标记转换结束
    this->transform = false;

    int result = encodeFrame(avCodecContext, NULL, &avPacket);
    if (result >= 0) {
        // 封装文件尾
        av_write_trailer(avFormatContext);
        // 释放内存
        if (avCodecContext != NULL) {
            avcodec_close(avCodecContext);
            avcodec_free_context(&avCodecContext);
            avCodecContext = NULL;
        }

        if (avFrame != NULL) {
            av_free(avFrame);
            avFrame = NULL;
        }

        if (pFrameBuffer != NULL) {
            av_free(pFrameBuffer);
            pFrameBuffer = NULL;
        }
        if (avFormatContext != NULL) {
            avio_close(avFormatContext->pb);
            avformat_free_context(avFormatContext);
            avFormatContext = NULL;
        }
    }
}



