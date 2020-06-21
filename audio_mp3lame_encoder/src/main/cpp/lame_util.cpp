//
// Created by KE TU on 2020-06-18.
//
#include <jni.h>
#include <stdio.h>
#include "libmp3lame/lame.h"
#include <android/log.h>
#include "com_yingke_audio_mp3lame_encoder_encoder_LameUtil.h"
#include "com_yingke_audio_mp3lame_encoder_encoder_LameUtil_Companion.h"

#define TAG "lame_util"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, TAG, __VA_ARGS__)


static  lame_global_flags *lame = NULL;

extern "C"
JNIEXPORT void JNICALL Java_com_yingke_audio_mp3lame_encoder_encoder_LameUtil_00024Companion_init
(JNIEnv *env,
                jobject jobject,
                jint inSampleRate,
                jint inChannel,
                jint outSampleRate,
                jint outBitrate,
                jint quality){
    LOGI("初始化lame库完成");

    if (lame != NULL){
        lame_close(lame);
        lame = NULL;
    }
    //  初始化编码器引擎，返回一个lame_global_flags结构体类型指针
    //  说明编码所需内存分配完成，否则，返回NULL
    lame = lame_init();
    // 设置输入数据流的采样率，默认为44100Hz
    lame_set_in_samplerate(lame, inSampleRate);
    // 设置输入数据流的通道数量，默认为2
    lame_set_num_channels(lame, inChannel);
    // 设置输出数据流的采样率，默认为0，单位KHz
    lame_set_out_samplerate(lame, outSampleRate);
    // 设置比特压缩率，默认为11
    lame_set_brate(lame, outBitrate);
    // 编码质量，推荐2、5、7
    lame_set_quality(lame, quality);
    // 配置参数
    lame_init_params(lame);

    LOGI("配置lame参数完成");


}


extern "C"
JNIEXPORT jint JNICALL Java_com_yingke_audio_mp3lame_encoder_encoder_LameUtil_00024Companion_encode
        (JNIEnv *env,
         jobject jobject,
         jshortArray buffer_l,
         jshortArray buffer_r,
         jint samples,
         jbyteArray mp3buf){

    jshort* j_buffer_l = env->GetShortArrayElements(buffer_l, NULL);
    jshort* j_buffer_r = env->GetShortArrayElements(buffer_r, NULL);
    const jsize mp3buf_size = env->GetArrayLength(mp3buf);
    jbyte* j_mp3buf = env->GetByteArrayElements(mp3buf, NULL);

    /**
     *这里的len / 2，是因为我们录音数据是char *类型的，一个char占一个字节。而这里要传的数据是short *类型的，一个short占2个字节
     *
     * lame_encode_buffer             //录音数据单声道16位整形用这个方法
     * lame_encode_buffer_interleaved //录音数据双声道交错用这个方法
     *lame_encode_buffer_float       //录音数据采样深度32位浮点型用这个方法
     */
    // encode to mp3 frame
    int result = lame_encode_buffer(lame,
            j_buffer_l,   // 左通道pcm数据
            j_buffer_r,   // 右通道pcm数据
            samples,      // 通道数据流采样 数
            reinterpret_cast<unsigned char *>(j_mp3buf), // mp3数据缓存起始地址
            mp3buf_size); // 缓存地址中有效mp3数据长度

    // 释放资源
    env->ReleaseShortArrayElements(buffer_l, j_buffer_l, 0);
    env->ReleaseShortArrayElements(buffer_r, j_buffer_r, 0);
    env->ReleaseByteArrayElements(mp3buf, j_mp3buf, 0);
    LOGI("编码pcm为mp3，数据长度=%d",result);
    return result;

}


extern "C" JNIEXPORT jint JNICALL Java_com_yingke_audio_mp3lame_encoder_encoder_LameUtil_00024Companion_flush
        (JNIEnv *env,
         jobject jobject,
         jbyteArray mp3buf){

    const jsize mp3buf_size = env->GetArrayLength(mp3buf);
    jbyte* j_mp3buf = env->GetByteArrayElements(mp3buf, NULL);

    // flush
    int result = lame_encode_flush(lame, reinterpret_cast<unsigned char *>(j_mp3buf), mp3buf_size);

    env->ReleaseByteArrayElements(mp3buf, j_mp3buf, 0);
    return result;

}

extern "C"
JNIEXPORT void JNICALL Java_com_yingke_audio_mp3lame_encoder_encoder_LameUtil_00024Companion_close
        (JNIEnv *env, jobject jobject){
    // 释放所占内存资源
    lame_close(lame);
    lame = NULL;
    LOGI("释放lame资源");
}


