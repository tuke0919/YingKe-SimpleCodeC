//
// Created by KE TU on 2020-06-30.
//
#include <android/log.h>
#include <faac.h>
#include "com_yingke_audio_faac_encoder_encoder_FaacUtil.h"

#define TAG "faac_util"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, TAG, __VA_ARGS__)


static faacEncHandle handle = NULL;
static unsigned long inputSamples = 0;
static unsigned long maxOutputBytes = 0;

extern "C"
JNIEXPORT jlongArray JNICALL
Java_com_yingke_audio_faac_encoder_encoder_FaacUtil_faacOpen(
        JNIEnv *env,
        jclass thiz,
        jint sample_rate,
        jint channels) {

    LOGE("faacOpen:");

    if (handle != NULL) {
        faacEncClose(handle);
        handle = NULL;
    }

    // 打开faac编码器引擎
    unsigned long sampleRate = static_cast<unsigned long>(sample_rate);
    unsigned long numChannels = static_cast<unsigned long>(channels);
    handle = faacEncOpen(sampleRate, numChannels, &inputSamples, &maxOutputBytes);

    LOGD("faacOpen: inputSamples = %lu", inputSamples);
    LOGD("faacOpen: maxOutputBytes = %lu", maxOutputBytes);

    if(handle == NULL){
        LOGE("faacOpen: [ERROR] Failed to call faacEncOpen()\n");
        return NULL;
    }

    // 获取当前编码器信息 设置编码配置信息
    faacEncConfigurationPtr encConfigurationPtr = faacEncGetCurrentConfiguration(handle);
    /*
		PCM Sample Input Format
		0	FAAC_INPUT_NULL			invalid, signifies a misconfigured config
		1	FAAC_INPUT_16BIT		native endian 16bit
		2	FAAC_INPUT_24BIT		native endian 24bit in 24 bits		(not implemented)
		3	FAAC_INPUT_32BIT		native endian 24bit in 32 bits		(DEFAULT)
		4	FAAC_INPUT_FLOAT		32bit floating point
    */
    encConfigurationPtr->inputFormat = FAAC_INPUT_16BIT;
    // 0 = Raw; 1 = ADTS
    encConfigurationPtr->outputFormat = 1;
    // AAC object types
    //#define MAIN 1
    //#define LOW  2
    //#define SSR  3
    //#define LTP  4
    encConfigurationPtr->aacObjectType = LOW;
    encConfigurationPtr->allowMidside = 0;
    encConfigurationPtr->useLfe = 0;
    encConfigurationPtr->bitRate = 48000;
    encConfigurationPtr->bandWidth = 32000;

    // 其他的参数不知道怎么配置，毕竟对音频不熟
    // 不过当前的设置可以实现转换，不过声音好像有一丢丢怪异
    // 这一块的配置信息很重要，错了会导致转码失败，然后你以为代码其他地方错了

    // 重置编码器配置信息
    faacEncSetConfiguration(handle, encConfigurationPtr);
    LOGD("faacOpen: faac Configuration done!");

    // 返回数组
    jlong params[2];
    params[0] = inputSamples;
    params[1] = maxOutputBytes;
    jlongArray array = env->NewLongArray(2);
    env->SetLongArrayRegion(array, 0, 2, params);

    return array;

}

extern "C"
JNIEXPORT jint JNICALL
Java_com_yingke_audio_faac_encoder_encoder_FaacUtil_faacEncode(
        JNIEnv *env,
        jclass thiz,
        jshortArray pcm_buffer,
        jint samples,
        jbyteArray aac_buffer) {

    LOGE("faacEncode:");
    if (handle == NULL) {
        LOGE("faacEncode: error handle is null!");
        return -1;
    }
    // 输入pcm的采样个数
    inputSamples = static_cast<unsigned long>(samples);

    jint* pcm_buf_ptr = reinterpret_cast<jint *>(env->GetShortArrayElements(pcm_buffer, NULL));
    unsigned char*  aac_buf_ptr = reinterpret_cast<unsigned char *>(env->GetByteArrayElements(aac_buffer, NULL));

    // 实际编码 返回字节数
    jint encodeSize = faacEncEncode(handle, pcm_buf_ptr, inputSamples, aac_buf_ptr, maxOutputBytes);
    return  encodeSize;

}

extern "C"
JNIEXPORT void JNICALL
Java_com_yingke_audio_faac_encoder_encoder_FaacUtil_faacClose(JNIEnv *env, jclass thiz) {
    faacEncClose(handle);
    inputSamples = 0;
    maxOutputBytes = 0;
    handle = NULL;
}



