//
// Created by KE TU on 2020-07-24.
//
#include <jni.h>
#ifndef YINGKE_SIMPLECODEC_LIBPNG_UTIL_H
#define YINGKE_SIMPLECODEC_LIBPNG_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL loadPNGImage(JNIEnv *, jclass , jstring, jobject);


#ifdef __cplusplus
}
#endif

#endif //YINGKE_SIMPLECODEC_LIBPNG_UTIL_H
