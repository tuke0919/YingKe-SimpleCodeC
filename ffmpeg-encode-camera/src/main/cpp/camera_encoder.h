//
// Created by KE TU on 2020-08-10.
//

#ifndef YINGKE_SIMPLECODEC_CAMERA_ENCODER_H
#define YINGKE_SIMPLECODEC_CAMERA_ENCODER_H

#include "jni.h"

#ifdef __cplusplus
extern "C" {
#endif

    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libavutil/avutil.h>
    #include <libswscale/swscale.h>

    JNIEXPORT void  JNICALL onPreviewFrame(JNIEnv * jniEnv, jclass thiz, jbyteArray yuvData, jint width, jint height);

    JNIEXPORT void  JNICALL encodeMp4Start(JNIEnv * jniEnv, jclass thiz, jstring mp4Path, jint width, jint height);

    JNIEXPORT void  JNICALL encodeMp4Stop(JNIEnv * jniEnv, jclass thiz);

    JNIEXPORT void  JNICALL encodeJPEG(JNIEnv * jniEnv, jclass thiz, jstring jpegPath, jint width, jint height);

#ifdef __cplusplus
}
#endif


#endif //YINGKE_SIMPLECODEC_CAMERA_ENCODER_H
