//
// Created by KE TU on 2020-08-10.
//

#include <libyuv/convert.h>
#include <libyuv.h>
#include "logger.h"
#include "encode_h264.h"

/**
 * 编码开关
 * @return
 */
bool H264Encoder::isTransform() {
    return this->transform;
}

/**
 * 初始化
 * @param mp4Path
 * @param width
 * @param height
 */
void H264Encoder::initEncoder(const char *h264Path, int width, int height) {
    LOGI("initEncoder");
    LOGI("h264Path = %s, width = %d, height = %d", h264Path, width, height);

    this->h264Path = h264Path;
    this->width  = width;
    this->height = height;
}

/**
 * 编码开始
 */
void H264Encoder::encodeStart() {
    LOGI("encodeStart");

    //1. 注册所有组件
    av_register_all();

    //2. 初始化输出码流的AVFormatContext
    avformat_alloc_output_context2(&avFormatContext, NULL, NULL, this->h264Path);

    //3. 打开待输出的视频文件
    if (avio_open(&avFormatContext->pb, this->h264Path, AVIO_FLAG_READ_WRITE)){
        LOGE("open output file failed");
        return;
    }

    //4. 编码器和编码器上下文
    avCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!avCodec){
        LOGE("could not find encoder");
        return;
    }

    avCodecContext = avcodec_alloc_context3(avCodec);
    // 编码器的ID,这里是H264编码器
    avCodecContext->codec_id = avCodec->id;
    // 编码器编码的数据类型
    avCodecContext->codec_type = AVMEDIA_TYPE_VIDEO;
    // 设置读取像素格式
    avCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;
    // 相机摄像头的 宽高 是反的
    avCodecContext->width = height;
    avCodecContext->height = width;

    // 帧率
    avCodecContext->framerate = (AVRational){fps, 1};
    // 时间基
    avCodecContext->time_base = (AVRational){1, fps};
    // 比特率468kbps
    avCodecContext->bit_rate = 1024000;
    // gop
    avCodecContext->gop_size = 10;
    // 无b帧
    avCodecContext->max_b_frames = 0;

    avCodecContext->level = 41;
    avCodecContext->refs = 1;

    // h264编码需要设置 才能正常使用
    if (avCodecContext->codec_id == AV_CODEC_ID_H264) {
        // 设置量化参数（难点，我们一般设置默认值）
        // 量化系数越小，视频越是清晰。一般情况下都是默认值，最小量化系数默认值是10，最大量化系数默认值是51
        avCodecContext->qmin = 5;
        avCodecContext->qmax = 41;
        avCodecContext->qcompress = 0.6;
    }

    // 5，打开h264编码器
    // 缺少优化步骤？
    // 编码延时问题
    // 编码选项->编码设置
    AVDictionary *opts = NULL;
    if (avCodecContext->codec_id == AV_CODEC_ID_H264) {
        // 设置预备参数
        // key: preset value: slow->慢 superfast->超快
        av_dict_set(&opts, "preset", "superfast", 0);
        // 设置调优
        // key: tune->调优 value: zerolatency->零延迟
        av_dict_set(&opts, "tune", "zerolatency", 0);
    }

    // 打开
    int result = avcodec_open2(avCodecContext, avCodec, &opts);
    if (result < 0) {
        LOGE("open encoder failed %d", result);
        return;
    }


    //6. 初始化视频码流
    avStream = avformat_new_stream(avFormatContext, avCodec);
    if (avStream == NULL) {
        LOGE("allocation output stream failed");
        return;
    }
    avStream->time_base.num = 1;
    avStream->time_base.den = fps;
    avStream->codecpar->codec_tag = 0;

    // 将AVCodecContext的成员复制到AVCodecPrmeters结构体
    if (avcodec_parameters_from_context(avStream->codecpar, avCodecContext) < 0) {
        LOGE("Failed av codec parameters_from_context");
        return;
    }

    //7.写文件头部
    result = avformat_write_header(avFormatContext, NULL);
    if (result < 0) {
        LOGE("Error occurred when opening output URL %d", result);
        return;
    }

    //8, 初始化帧
    avFrame = av_frame_alloc();
    avFrame->width = avCodecContext->width;
    avFrame->height = avCodecContext->height;
    avFrame->format = avCodecContext->pix_fmt;

    // 申请分配图像缓存
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
void H264Encoder::encodeBuffer(unsigned char *nv21Buffer){
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

    // 编码这一帧图像 编码H.264
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
int H264Encoder::encodeFrame(AVCodecContext *pCodecCtx, AVFrame *pAvFrame, AVPacket *pAvPacket){
    LOGI("encodeFrame");

    // 向编码器 发送原始 图像帧
    int ret = avcodec_send_frame(pCodecCtx, pAvFrame);
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

        //AVFrame PTS
        pAvFrame->pts = index;

        LOGI("Write packed %d, size=%d ", pAvPacket->pts, pAvPacket->size);
        // packet在stream的位置
        pAvPacket->stream_index = avStream->index;
        // 时间基
        AVRational time_base = avFormatContext->streams[0]->time_base;//{ 1, 1000 };
        pAvPacket->pts = index * (avStream->time_base.den) / ((avStream->time_base.num) * fps);
        pAvPacket->dts = pAvPacket->pts;
        pAvPacket->duration = (avStream->time_base.den) / ((avStream->time_base.num) * fps);

        LOGI("Send frame index:%d,pts:%lld,dts:%lld,duration:%lld,time_base:%d,%d",
             index,
             (long long) pAvPacket->pts,
             (long long) pAvPacket->dts,
             (long long) pAvPacket->duration,
             time_base.num,
             time_base.den);

        pAvPacket->pos = -1;
        // 向媒体文件 写packet
        ret = av_interleaved_write_frame(avFormatContext, pAvPacket);

        if (ret != 0) {
            LOGE("av_interleaved_write_frame failed");
        }
        av_packet_unref(pAvPacket);

        index ++;
    }

    LOGI("encodeFrame: success ");
    return 0;
}

/**
 * 编码开始
 */
void H264Encoder::encodeStop(){
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



