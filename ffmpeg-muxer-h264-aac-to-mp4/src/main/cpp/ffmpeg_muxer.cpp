//
// Created by KE TU on 2020-07-14.
//

#include "ffmpeg_muxer.h"
#include "logger.h"


int Mp4Muxer::startTransform(const char *h264_path, const char *aac_path, const char *output_path) {

    int  ret = 0;
    //1.注册所有组件
    av_register_all();

    // 2，打开输出文件 上下文
    avformat_alloc_output_context2(&outPutFormatCtx, NULL, NULL, output_path);

    if (NULL == outPutFormatCtx) {
        LOGE("Could't create output context");
        goto end;
    }

    // 3, 打开视频文件 上下文
    ret = avformat_open_input(&h264FormatCtx, h264_path, 0, 0);
    if (ret < 0) {
        LOGE("Could't open h264 input file");
        goto end;
    }
    // 3.1 查找视频文件 流信息
    ret = avformat_find_stream_info(h264FormatCtx, NULL);
    if (ret < 0) {
        LOGE("Could't find h264 input stream information");
        goto end;
    }

    // 4, 打开音频文件 上下文
    ret = avformat_open_input(&aacFormatCtx, aac_path, 0, 0);
    if (ret < 0) {
        LOGE("Could't open aac input file");
        goto end;
    }

    // 4.1 查找音频文件 流信息
    ret = avformat_find_stream_info(aacFormatCtx, NULL);
    if (ret < 0) {
        LOGE("Could't find aac input stream information");
        goto end;
    }

    // 5，新建视频流，音频流
    videoStreamIndex = createNewStream(h264FormatCtx, videoIndex, AVMEDIA_TYPE_VIDEO);
    audioStreamIndex = createNewStream(aacFormatCtx, audioIndex, AVMEDIA_TYPE_AUDIO);

    // 6, 打开输出文件
    if (!(outPutFormatCtx->oformat->flags & AVFMT_NOFILE)) {

        if (avio_open(&outPutFormatCtx->pb, output_path, AVIO_FLAG_WRITE) < 0){
            LOGE("Could't open output %s", output_path);
            goto end;
        }
    }

    // 7, 写文件头
    ret = avformat_write_header(outPutFormatCtx, NULL);
    if (ret < 0) {
        LOGE("Could't write header");
        goto end;
    }


    while (true) {
        int streamIndex = 0;
        AVStream *inStream, *outStream;

        // 8. 从输入流 读出 packet

        // 比较时间戳
        int ts_ret = av_compare_ts(cur_pts_v,
                h264FormatCtx->streams[videoIndex]->time_base,
                cur_pts_a,
                aacFormatCtx->streams[audioIndex]->time_base);

        if (ts_ret <= 0) {
            // 读视频packet
            streamIndex = videoStreamIndex;
            while (av_read_frame(h264FormatCtx, &avPacket) >= 0) {
                // 视频输入流
                inStream = h264FormatCtx->streams[avPacket.stream_index];
                // 输出文件里的 视频输出流
                outStream = outPutFormatCtx->streams[streamIndex];

                if (avPacket.stream_index == videoIndex) {
                    // 如果 没有pts，需要写入，Raw H.264
                    writePts(&avPacket, inStream);
                    cur_pts_v = avPacket.pts;
                    break;
                }
            }
        } else {
            // 读音频 packet
            streamIndex = audioStreamIndex;
            while (av_read_frame(aacFormatCtx, &avPacket) >= 0) {
                // 输入音频流
                inStream = aacFormatCtx->streams[avPacket.stream_index];
                // 输出文件里的，音频输出流
                outStream = outPutFormatCtx->streams[streamIndex];
                if (avPacket.stream_index == audioIndex) {
                    // 如果 没有pts，需要写入
                    writePts(&avPacket, inStream);
                    cur_pts_a = avPacket.pts;
                    break;
                }
            }
        }

        // 9.转换PTS/DTS
        avPacket.pts = av_rescale_q_rnd(avPacket.pts,
                inStream->time_base,
                outStream->time_base,
                (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

        avPacket.dts = av_rescale_q_rnd(avPacket.dts,
                inStream->time_base,
                outStream->time_base,
                (AVRounding) (AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

        avPacket.duration = av_rescale_q(avPacket.duration,
                inStream->time_base,
                outStream->time_base);

        avPacket.pos = -1;
        avPacket.stream_index = streamIndex;

        // 10， 向输出文件 写数据
//        int write_ret = av_interleaved_write_frame(outPutFormatCtx, &avPacket);
        if (av_interleaved_write_frame(outPutFormatCtx, &avPacket) < 0) {
            LOGE("Could't write frame");
            break;
        }

        av_packet_unref(&avPacket);
    }

    // 11，写文件尾部
    av_write_trailer(outPutFormatCtx);

    end:
    avformat_close_input(&h264FormatCtx);
    avformat_close_input(&aacFormatCtx);

    // 关闭操作
    if (outPutFormatCtx && !(outPutFormatCtx->oformat->flags & AVFMT_NOFILE)) {
        avio_close(outPutFormatCtx->pb);
    }

    avformat_free_context(outPutFormatCtx);

    return 0;
}

/**
 * 新建输出流 返回输出流index
 *
 * @param avFormatContext
 * @param inputIndex
 * @param mediaType
 * @return
 */
int Mp4Muxer::createNewStream(AVFormatContext *avFormatContext, int &inputIndex, AVMediaType mediaType) {

    int outputStreamIndex = -1;
    for (int i = 0; i < avFormatContext->nb_streams; ++i) {
        // 文件输入流
        AVStream *inputAvStream = avFormatContext->streams[i];
        if (inputAvStream->codecpar->codec_type == mediaType) {
            // 分配一个编解码器上下文，默认值
            AVCodecContext * avCodecContext = avcodec_alloc_context3(NULL);

            // 拷贝输入流编码器参数，给avCodecContext
            avcodec_parameters_to_context(avCodecContext, inputAvStream->codecpar);

            // 新建输出流 用输入流的编码器 初始化输出流
            AVStream *out_stream = avformat_new_stream(outPutFormatCtx, avCodecContext->codec);
            inputIndex = i;

            if (NULL == out_stream) {
                LOGE("Could't allocating output stream");
                return outputStreamIndex;
            }
            // 输出流 的index
            outputStreamIndex = out_stream->index;

            // 拷贝 avCodecContext参数 到 输出流编解码器
            int ret = avcodec_parameters_from_context(out_stream->codecpar, avCodecContext);
            if (ret < 0) {
                LOGE("Could't to copy context from input to output stream codec context");
                return outputStreamIndex;
            }

            avCodecContext->codec_tag = 0;
            if (outPutFormatCtx->oformat->flags & AVFMT_GLOBALHEADER) {
                avCodecContext->flags |= CODEC_FLAG_GLOBAL_HEADER;
            }
            break;
        }
    }

    return outputStreamIndex;
}

/**
 * packet 写 pts
 * @param avPacket
 * @param inputStream
 */
void Mp4Muxer::writePts(AVPacket *avPacket, AVStream *inputStream) {
    if (avPacket->pts == AV_NOPTS_VALUE) {
        AVRational time_base = inputStream->time_base;

        // 计算两帧的时间
        int64_t calc_duration = (double) AV_TIME_BASE / av_q2d(inputStream->r_frame_rate);

        avPacket->pts = (double) (frameIndex * calc_duration) / (double) (av_q2d(time_base) * AV_TIME_BASE);
        avPacket->dts = avPacket->pts;
        avPacket->duration = (double) calc_duration /(double) (av_q2d(time_base) * AV_TIME_BASE);

        frameIndex++;
    }
}
