//
// Created by KE TU on 2020-07-13.
//

#ifndef YINGKE_SIMPLECODEC_MP4DEMUXERUTIL_H
#define YINGKE_SIMPLECODEC_MP4DEMUXERUTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <jni.h>
#include <logger.h>
#include <android/log.h>

    typedef struct InputParams{
         char * mp4_path;
         char * h264_path;
         char * aac_path;
    } inputParams;

JNIEXPORT jint JNICALL startMp4Demuxer(JNIEnv *jniEnv, jclass clz, jstring mp4Path, jstring h264Path, jstring aacPath, jobject callback);
JNIEXPORT void JNICALL stopMp4Demuxer(JNIEnv *jniEnv, jclass clz);



#ifdef __cplusplus
};
#endif
#endif //YINGKE_SIMPLECODEC_MP4DEMUXERUTIL_H
