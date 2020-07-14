//
// Created by KE TU on 2020-07-14.
//

#include "Mp4MuxerUtil.h"
#include "ffmpeg_muxer.h"

#include "logger.h"
#define NUM_METHODS(x) ((int)(sizeof(x)/ sizeof(x[0])))

/**
 * 关联方法
 */
static JNINativeMethod methods[] = {
        {"startMuxerMp4", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)I", (void *)startMuxerMp4},
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
    jclass clz = env->FindClass("com/yingke/ffmpeg/muxer/Mp4MuxerUtil");
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
 * @param thiz
 * @param h264Path
 * @param aacPath
 * @param outputPath
 * @return
 */
jint startMuxerMp4(JNIEnv *env, jclass thiz, jstring h264Path, jstring aacPath, jstring outputPath){

    const char *h264_path = env->GetStringUTFChars(h264Path, NULL);
    const char *aac_path = env->GetStringUTFChars(aacPath, NULL);
    const char *mp4_path = env->GetStringUTFChars(outputPath, NULL);

    // 开始封装
    Mp4Muxer *mp4Muxer = new Mp4Muxer();
    int result = mp4Muxer->startTransform(h264_path, aac_path, mp4_path);

    delete mp4Muxer;

    env->ReleaseStringUTFChars(h264Path, h264_path);
    env->ReleaseStringUTFChars(aacPath, aac_path);
    env->ReleaseStringUTFChars(outputPath, mp4_path);
    return result;
}





