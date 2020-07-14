//
// Created by KE TU on 2020-07-14.
//

#ifndef YINGKE_SIMPLECODEC_FFMPEG_MUXER_H
#define YINGKE_SIMPLECODEC_FFMPEG_MUXER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>

#ifdef __cplusplus
}
#endif


class Mp4Muxer{
private:

    int  frameIndex = 0;
    int videoIndex = -1;
    int audioIndex = -1;

    int audioStreamIndex = -1;
    int videoStreamIndex = -1;

    int64_t cur_pts_v = 0, cur_pts_a = 0;

    /**
     * 读出的packet
     */
    AVPacket avPacket;

    /**
     * h264文件 上下文
     */
    AVFormatContext *h264FormatCtx = NULL;
    /**
     * aac文件 上下文
     */
    AVFormatContext *aacFormatCtx  = NULL;

public:
    /**
     * 输出文件 封装上下文
     */
    AVFormatContext *outPutFormatCtx = NULL;

    /**
     * 开始 封装
     * @param h264_path
     * @param aac_path
     * @param output_path
     * @return
     */
    int startTransform(const char *h264_path, const char *aac_path, const char *output_path);

    /**
     * 创建 音视频 流
     * @param avFormatContext
     * @param inputIndex
     * @param mediaType
     * @return
     */
    int createNewStream(AVFormatContext *avFormatContext, int &inputIndex, AVMediaType mediaType);

    /**
     * 写 pts
     * @param avPacket
     * @param inputStream
     */
    void writePts(AVPacket *avPacket, AVStream *inputStream);

};

#endif //YINGKE_SIMPLECODEC_FFMPEG_MUXER_H
