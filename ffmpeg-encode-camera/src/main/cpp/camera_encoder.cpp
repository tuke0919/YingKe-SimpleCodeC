//
// Created by KE TU on 2020-08-10.
//

#include "camera_encoder.h"
#include "logger.h"
#include "encode_jpeg.h"
#include "encode_mp4.h"

#define NUM_METHODS(x)  ((int)(sizeof(x)/sizeof(x[0])))


static JNINativeMethod methods[] = {
        {"onPreviewFrame", "([BII)V",                 (void *)onPreviewFrame},
        {"encodeMp4Start", "(Ljava/lang/String;II)V", (void *)encodeMp4Start},
        {"encodeMp4Stop",  "()V",                     (void *)encodeMp4Stop},
        {"encodeJPEG",     "(Ljava/lang/String;II)V", (void *)encodeJPEG}
};

/**
 * 动态注册
 */
extern "C" JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* jvm, void* reserved){

    JNIEnv *env = NULL;
    // jvm 获取 jni环境变量
    if (jvm->GetEnv((void **)(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    // 注册方法
    jclass clazz = env->FindClass("com/yingke/encode/camera/CameraEncoder");
    if (env->RegisterNatives(clazz, methods,  NUM_METHODS(methods)) != JNI_OK) {
        return -1;
    }

    return JNI_VERSION_1_6;
}

// mp4 编码器
Mp4Encoder  *mp4Encoder = NULL;
// jpeg编码器
JPEGEncoder *jpegEncoder = NULL;

/**
 * 接收 相机数据
 *
 * @param jniEnv
 * @param thiz
 * @param yuvData
 * @param width
 * @param height
 */
void onPreviewFrame(JNIEnv * jniEnv, jclass thiz, jbyteArray yuvData, jint width, jint height){

    // 编码视频
    if (mp4Encoder != NULL && mp4Encoder->isTransform()) {
        jbyte *nv21Buffer = jniEnv->GetByteArrayElements(yuvData, 0);
        // 编码 nv21帧
        mp4Encoder->encodeBuffer((unsigned char *)(nv21Buffer));
        jniEnv->ReleaseByteArrayElements(yuvData, nv21Buffer, 0);
    }

    // 编码jpeg
    if (jpegEncoder != NULL) {
        if (jpegEncoder->isTransform()){
            jbyte *nv21Buffer = jniEnv->GetByteArrayElements(yuvData, 0);
            // 编码 nv21帧
            jpegEncoder->encodeJPEG((unsigned char *)(nv21Buffer));
            jniEnv->ReleaseByteArrayElements(yuvData, nv21Buffer, 0);
        }
        // 调用析构函数
        delete jpegEncoder;
        jpegEncoder = NULL;
    }
}

/**
 * 编码 mp4 开始
 *
 * @param jniEnv
 * @param thiz
 * @param mp4Path
 * @param width
 * @param height
 */
void encodeMp4Start(JNIEnv * jniEnv, jclass thiz, jstring mp4Path, jint width, jint height){
    const char *mp4_path = jniEnv->GetStringUTFChars(mp4Path, NULL);

    if (mp4Encoder == NULL) {
        mp4Encoder = new Mp4Encoder();
    }
    // 仅实力化，在Preview中 编码
    mp4Encoder->initEncoder(mp4_path, width, height);
    mp4Encoder->encodeStart();

    jniEnv->ReleaseStringUTFChars(mp4Path, mp4_path);


}

/**
 * 编码mp4 结束
 *
 * @param jniEnv
 * @param thiz
 */
void encodeMp4Stop(JNIEnv * jniEnv, jclass thiz){
    if (NULL != mp4Encoder) {
        mp4Encoder->encodeStop();
        mp4Encoder = NULL;
    }
}

/**
 * 编码 成jpeg
 *
 * @param jniEnv
 * @param thiz
 * @param jpegPath
 * @param width
 * @param height
 */
void encodeJPEG(JNIEnv * jniEnv, jclass thiz, jstring jpegPath, jint width, jint height){
     if (NULL == jpegEncoder){
         const char *jpeg_path = jniEnv->GetStringUTFChars(jpegPath, NULL);
         // 仅实例化，在preview编码
         jpegEncoder = new JPEGEncoder(jpeg_path, width, height);
         jniEnv->ReleaseStringUTFChars(jpegPath, jpeg_path);
     }
}






