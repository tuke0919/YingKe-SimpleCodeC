//
// Created by KE TU on 2020-07-14.
//

#include "native_player.h"
#include "logger.h"
#include "android/native_window.h"
#include "android/native_window_jni.h"
#include "ffmpeg_player.h"

#define NUM_METHODS(x) ((int)(sizeof(x)/ sizeof(x[0])))

/**
 * 关联方法
 */
static JNINativeMethod methods[] = {
        {"startMp4Player", "(Ljava/lang/String;Ljava/lang/Object;)I", (void *)startNativePlayer},
};

/**
 * 动态注册
 */
extern "C" JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* jvm, void* reserved){

    // JVM中获取 JNIEnv 环境变量
    JNIEnv *env = NULL;
    if(JNI_OK != jvm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6)) {
        LOGE("Get JNIEnv failed");
        return JNI_ERR;
    }

    // 注册Native方法
    jclass clz = env->FindClass("com/yingke/ffmpeg/decode/Mp4NativePlayer");
    int  ret = env->RegisterNatives(clz, methods, NUM_METHODS(methods));
    if (ret < 0){
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}

extern "C"
JNIEXPORT void JNICALL
JNI_OnUnload(JavaVM* jvm, void* reserved) {
    if(jvm) {
        jvm->DestroyJavaVM();
    }
}

/**
 *
 * @param env
 * @param clz
 * @param videoPath
 * @param surface
 * @return
 */
jint startNativePlayer(JNIEnv *env, jclass clz, jstring videoPath, jobject surface){

    const char *video_path = env->GetStringUTFChars(videoPath, NULL);
    // 创建nativeWindow
    ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
    if (window == 0) {
        LOGE("Could not get native window from surface");
        return -1;
    }
    // 封装类
    NativePlayer nativePlayer = NativePlayer();
    nativePlayer.native_play(video_path, window);

    env->ReleaseStringUTFChars(videoPath, video_path);
    return 0;
}
