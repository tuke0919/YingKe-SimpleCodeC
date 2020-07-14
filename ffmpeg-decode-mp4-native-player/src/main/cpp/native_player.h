//
// Created by KE TU on 2020-07-14.
//

#include "jni.h"
#ifndef YINGKE_SIMPLECODEC_NATIVE_PLAYER_H
#define YINGKE_SIMPLECODEC_NATIVE_PLAYER_H

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jint JNICALL startNativePlayer(JNIEnv *env, jclass clz, jstring videoPath, jobject surface);

#ifdef __cplusplus
}
#endif

#endif //YINGKE_SIMPLECODEC_NATIVE_PLAYER_H
