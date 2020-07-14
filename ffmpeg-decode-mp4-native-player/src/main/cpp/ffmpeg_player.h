//
// Created by KE TU on 2020-07-14.
//
#include <android/native_window.h>
#include <android/native_window_jni.h>

#ifndef YINGKE_SIMPLECODEC_FFMPEG_PLAYER_H
#define YINGKE_SIMPLECODEC_FFMPEG_PLAYER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libavutil/log.h"
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>


#ifdef __cplusplus
}
#endif

class NativePlayer{

private:
    int width = 0;
    int height = 0;

    int bufferSize = 0;
    int videoIndex = -1;

    AVFormatContext *avFormatContext = NULL;
    AVCodec *avCodec = NULL;
    AVCodecContext *avCodecContext = NULL;

    AVPacket *avPacket = NULL;
    AVFrame *avFrame = NULL;
    AVFrame *avFrameRGB = NULL;

    SwsContext *swsContext = NULL;
    uint8_t  *out_buffer = NULL;
    ANativeWindow_Buffer windowBuffer;

public:
    /**
     * native 播放
     * @param video_path
     * @param aNativeWindow
     * @return
     */
    int native_play(const char * video_path, ANativeWindow *aNativeWindow);
};



#endif //YINGKE_SIMPLECODEC_FFMPEG_PLAYER_H
