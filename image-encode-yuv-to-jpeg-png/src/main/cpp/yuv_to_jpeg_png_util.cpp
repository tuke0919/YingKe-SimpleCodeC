//
// Created by KE TU on 2020-07-24.
//

#include "yuv_to_jpeg_png_util.h"
#include "logger.h"

#include "yuv_to_jpeg_util.h"
#include "yuv_to_png_util.h"

#define NUM_METHODS(x) ((int)(sizeof(x)/ sizeof(x[0])))


/**
 * 关联方法
 */
static JNINativeMethod methods[] = {
        {"yuv2jpeg", "(Ljava/lang/String;Ljava/lang/String;II)V", (void *) yuv2jpeg},
        {"yuv2png", "(Ljava/lang/String;Ljava/lang/String;II)V", (void *) yuv2png}
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
    jclass clz = env->FindClass("com/yingke/yuvtojpegpng/YuvToJpegPngUtil");
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

void ThrowException(JNIEnv *env, const char *exception, const char *message) {
    jclass clazz = env->FindClass(exception);
    if (NULL != clazz) {
        env->ThrowNew(clazz, message);
    }
}

/**
 *
 * @param env
 * @param thiz
 * @param yuvPath
 * @param jpegPath
 * @param width
 * @param height
 */
void yuv2jpeg(JNIEnv *env, jclass thiz, jstring yuvPath, jstring jpegPath, jint width, jint height) {

    const  char * yuv_path  = env->GetStringUTFChars(yuvPath, NULL);
    const char * jpeg_path = env->GetStringUTFChars(jpegPath, NULL);

    YuvToJpeg::YUV420P_TO_JPEG(yuv_path, jpeg_path, width, height);

    env->ReleaseStringUTFChars(yuvPath, yuv_path);
    env->ReleaseStringUTFChars(jpegPath, jpeg_path);
}


/**
 *
 * @param env
 * @param thiz
 * @param yuvPath
 * @param pngPath
 * @param width
 * @param height
 */
void yuv2png(JNIEnv *env, jclass thiz, jstring yuvPath, jstring pngPath, jint width, jint height) {

    const  char * yuv_path  = env->GetStringUTFChars(yuvPath, NULL);
    const char * png_path = env->GetStringUTFChars(pngPath, NULL);

    YuvToPng::YUV420P_TO_PNG(yuv_path, png_path, width, height);

    env->ReleaseStringUTFChars(yuvPath, yuv_path);
    env->ReleaseStringUTFChars(pngPath, png_path);



}






