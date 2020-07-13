//
// Created by KE TU on 2020-07-13.
//

#include "logger.h"
#include "ffmpeg_demuxer.h"

// 封装
FFmpegDemuxer global_demuxer;

int initFFmpegEngine(char* mp4_path){

    if(!mp4_path) {
        LOGE("initFFmpegEngine failed, mp4_path can not be null");
        return -1;
    }

    // 注册所有组件
    av_register_all();
    // 注册所有编解码
    avcodec_register_all();

    // 获取Format上下文
    global_demuxer.inputFormatCtx = avformat_alloc_context();
    if (!global_demuxer.inputFormatCtx) {
        releaseDemuxerFFmpeg();
        LOGE("avformat_alloc_context failed.");
        return -1;
    }

    // 打开输入文件
    int ret = avformat_open_input(&global_demuxer.inputFormatCtx, mp4_path, NULL, NULL);
    if (ret < 0){
        releaseDemuxerFFmpeg();
        LOGE("avformat_open_input failed.");
        return -1;
    }
    // 寻找流信息
    ret = avformat_find_stream_info(global_demuxer.inputFormatCtx, NULL);
    if (ret < 0) {
        releaseDemuxerFFmpeg();
        LOGE("avformat_find_stream_info failed.");
        return -1;
    }
    // 获取输入 音视频轨道id
    for (int i = 0; i < global_demuxer.inputFormatCtx->nb_streams; ++i) {
        AVStream * avStream = global_demuxer.inputFormatCtx->streams[i];
        if (!avStream){
            continue;
        }

        AVMediaType  mediaType = avStream->codecpar->codec_type;
        if (global_demuxer.video_stream_id == -1 || global_demuxer.audio_stream_id == -1){
            if (mediaType == AVMEDIA_TYPE_VIDEO){
                global_demuxer.video_stream_id = i;
            }
            if (mediaType == AVMEDIA_TYPE_AUDIO){
                global_demuxer.audio_stream_id = i;
            }
        }
    }
    // 临时分配个内存
    global_demuxer.avPacket = av_packet_alloc();
    av_init_packet(global_demuxer.avPacket);


    /** (1) 创建h264_mp4toannexb 比特流过滤器结构体AVBitStreamFilter
      *  声明位于../libavcodec/avcodec.h
      *  typedef struct AVBitStreamFilter {
      *       // 过滤器名称
      *       const char *name;
      *       // 过滤器支持的编码器ID列表
      *       const enum AVCodecID *codec_ids;
      *       const AVClass *priv_class;
      *       ...
      *   }
      * */

    // 生成bsf实例
    const AVBitStreamFilter *avBitStreamFilter = av_bsf_get_by_name("h264_mp4toannexb");
    if (!avBitStreamFilter){
        releaseDemuxerFFmpeg();
        LOGE("get AVBitStreamFilter failed. err = %d", ret);
        return ret;
    }

    /** (2)创建给定过滤器上下文结构体AVBSFContext，该结构体存储了过滤器的状态
    *  声明在../libavcodec/avcodec.h
    *  typedef struct AVBSFContext {
    *       ...
    *       const struct AVBitStreamFilter *filter;
    *       // 输入输出流参数信息
    *       // 调用av_bsf_alloc()后被创建分配
    *       // 调用av_bsf_init()后被初始化
    *       AVCodecParameters *par_in;
    *       AVCodecParameters *par_out;
    *       // 输入输出packet的时间基
    *       // 在调用av_bsf_init()之前被设置
    *       AVRational time_base_in;
    *       AVRational time_base_out;
    *  }
    * */
    // 给bsf 分配一个上下文
    ret = av_bsf_alloc(avBitStreamFilter, &global_demuxer.avbsfContext);
    if (ret < 0) {
        releaseDemuxerFFmpeg();
        LOGE("av_bsf_alloc failed. err = %d", ret);
        return ret;
    }


    /** (3) 拷贝输入流相关参数到过滤器的AVBSFContext*/
    // 拷贝 编解码器参数给 filter
    ret = avcodec_parameters_copy(global_demuxer.avbsfContext->par_in, global_demuxer.inputFormatCtx->streams[global_demuxer.video_stream_id]->codecpar);
    if (ret < 0) {
        releaseDemuxerFFmpeg();
        LOGE("copy codec params to filter failed,err = %d", ret);
        return ret;
    }

    /**(4) 使过滤器进入准备状态。在所有参数被设置完毕后调用*/
    // 初始化，准备
    ret = av_bsf_init(global_demuxer.avbsfContext);
    if (ret < 0) {
        releaseDemuxerFFmpeg();
        LOGE("prepare the filter failed,err = %d", ret);
        return ret;
    }

    return ret;

}

/**
 * 读入 avpacket
 * @return 返回AVPacket数据大小
 */
int readDataFromAVPacket() {
    int ret = -1;
    if(global_demuxer.avPacket) {
        ret = av_read_frame(global_demuxer.inputFormatCtx, global_demuxer.avPacket);
        if(ret == 0) {
            return global_demuxer.avPacket->size;
        }
    }
    return ret;
}

/**
 *  处理视频流 的avPacket
 * @param out
 * @param size
 * @return
 */
int handleAvPacketData(uint8_t *out, int size){
    if(!global_demuxer.avPacket || !out) {
        return -1;
    }
    int stream_index = global_demuxer.avPacket->stream_index;
    if (stream_index == getVideoStreamIndex()){
        // 视频流的packet

        // Packet 送入 bsf
        /**(5) 将输入packet提交到过滤器处理*/
        int ret = av_bsf_send_packet(global_demuxer.avbsfContext, global_demuxer.avPacket);
        if(ret < 0) {
            av_packet_unref(global_demuxer.avPacket);
            av_init_packet(global_demuxer.avPacket);
            return ret;
        }

        for(;;){
            /**(6) 循环读取过滤器，直到返回0标明读取完毕*/
            // Retrieve a filtered packet.
            int flags = av_bsf_receive_packet(global_demuxer.avbsfContext, global_demuxer.avPacket);
            if (flags == EAGAIN){
                continue;
            } else{
                break;
            }
        }

        memcpy(out, global_demuxer.avPacket->data, size);
    } else if (stream_index == getAudioStreamIndex()) {
        // 音频 流的 packet
        memcpy(out, global_demuxer.avPacket->data, size);
    }

    /**(7) 释放过滤器资源*/
    av_packet_unref(global_demuxer.avPacket);
    av_init_packet(global_demuxer.avPacket);

    return stream_index;
}

/**
 * 释放资源
 */
void releaseDemuxerFFmpeg() {
    if (global_demuxer.inputFormatCtx){
        avformat_close_input(&global_demuxer.inputFormatCtx);
        avformat_free_context(global_demuxer.inputFormatCtx);
    }

    if (global_demuxer.avPacket){
        av_packet_free(&global_demuxer.avPacket);
        global_demuxer.avPacket = NULL;
    }

    if (global_demuxer.avbsfContext){
        av_bsf_free(&global_demuxer.avbsfContext);
    }

    LOGE("releaseDemuxerFFmpeg success");
}

/**
 * 视频流 index
 * @return
 */
int getVideoStreamIndex() {
    return global_demuxer.video_stream_id;
}

/**
 * 音频流 index
 * @return
 */
int getAudioStreamIndex() {
    return global_demuxer.audio_stream_id;
}

/**
 * 采样率 的位置
 * @return
 */
int getAudioSampleRateIndex() {
    int rates[] = {96000, 88200, 64000,48000, 44100,
                   32000, 24000, 22050, 16000, 12000,
                   11025, 8000, 7350, -1, -1, -1};
    int sampe_rate = global_demuxer.inputFormatCtx->streams[getAudioStreamIndex()]->codecpar->sample_rate;
    for (int index = 0; index < 16; index++) {
        if(sampe_rate == rates[index]) {
            return index;
        }
    }
    return -1;
}

/**
 * 音频 profile
 * @return
 */
int getAudioProfile() {
    return global_demuxer.inputFormatCtx->streams[getAudioStreamIndex()]->codecpar->profile;
}

/**
 * 音频 信道数
 * @return
 */
int getAudioChannels() {
    return global_demuxer.inputFormatCtx->streams[getAudioStreamIndex()]->codecpar->channels;
}





