//
// Created by KE TU on 2020-07-04.
//
#include <jni.h>
#include <stdio.h>
#include <time.h>

#include "decode_mp4.h"

extern "C"
JNIEXPORT jint JNICALL
decodeMp4(JNIEnv *env, jobject thiz, jstring mp4_path, jstring yuv_path){
    // 实例化 解码器
    VideoDecoder *mp4Decoder = new Mp4Decoder();

    const char *mp4Path = env->GetStringUTFChars(mp4_path, NULL);
    const char *yuvPath = env->GetStringUTFChars(yuv_path, NULL);

    // 初始化
    mp4Decoder->initDecoder(mp4Path);
    // 解码
    jint result = mp4Decoder->decodeFile(yuvPath);

    env->ReleaseStringUTFChars(mp4_path, mp4Path);
    env->ReleaseStringUTFChars(yuv_path, yuvPath);

    return result;
}

/**
 * 动态注册
 * 1, 添加 java方法和 jni方法对应
 * */
JNINativeMethod method[] = {
        {"decode", "(Ljava/lang/String;Ljava/lang/String)I", (void *)(decodeMp4)}
};

jint registerNativeMethod(JNIEnv *env){
    jclass cl = env->FindClass("com/yingke/decode/mp4/Mp4Decoder");
    // 3，调用RegisterNatives方法
    if ((env->RegisterNatives(cl, method, sizeof(method) / sizeof(method[0]))) < 0){
        return -1;
    }
    return 0;
}

/**
 * 2，java层 loadLibraries 的是调用
 *
 * @param vm
 * @param reserved
 * @return
 */
jint JNI_OnLoad(JavaVM *vm, void *reserved){
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    if (registerNativeMethod(env) != JNI_OK){
        return -1;
    }
    return JNI_VERSION_1_6;
}





