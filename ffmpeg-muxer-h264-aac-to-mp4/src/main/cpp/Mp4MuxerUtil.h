//
// Created by KE TU on 2020-07-14.
//
#include <jni.h>
#ifndef YINGKE_SIMPLECODEC_MP4MUXERUTIL_H
#define YINGKE_SIMPLECODEC_MP4MUXERUTIL_H

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL startMuxerMp4(JNIEnv *env, jclass thiz, jstring h264Path, jstring aacPath, jstring outputPath);

#ifdef __cplusplus
}
#endif

#endif //YINGKE_SIMPLECODEC_MP4MUXERUTIL_H
