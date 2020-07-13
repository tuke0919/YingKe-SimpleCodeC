//
// Created by KE TU on 2020-07-13.
//

#ifndef YINGKE_SIMPLECODEC_FFMPEG_DEMUXER_H
#define YINGKE_SIMPLECODEC_FFMPEG_DEMUXER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"

    struct FFmpegDemuxer{
        AVFormatContext *inputFormatCtx = NULL;
        AVPacket *avPacket = NULL;

        int video_stream_id = -1;
        int audio_stream_id = -1;

        AVBSFContext *avbsfContext = NULL;
    };

int initFFmpegEngine(char* mp4_path);
int readDataFromAVPacket();
int handleAvPacketData(uint8_t *out, int size);
void releaseDemuxerFFmpeg();

int getVideoStreamIndex();
int getAudioStreamIndex();

int getAudioSampleRateIndex();
int getAudioProfile();
int getAudioChannels();


#ifdef __cplusplus
};
#endif


#endif //YINGKE_SIMPLECODEC_FFMPEG_DEMUXER_H
