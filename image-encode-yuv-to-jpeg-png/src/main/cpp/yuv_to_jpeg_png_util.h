//
// Created by KE TU on 2020-07-24.
//

#ifndef YINGKE_SIMPLECODEC_YUV_TO_JPEG_PNG_UTIL_H
#define YINGKE_SIMPLECODEC_YUV_TO_JPEG_PNG_UTIL_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL yuv2jpeg(JNIEnv *, jclass , jstring, jstring, jint, jint);

JNIEXPORT void JNICALL yuv2png(JNIEnv *, jclass, jstring, jstring, jint, jint);

#ifdef __cplusplus
}
#endif

#endif //YINGKE_SIMPLECODEC_YUV_TO_JPEG_PNG_UTIL_H
