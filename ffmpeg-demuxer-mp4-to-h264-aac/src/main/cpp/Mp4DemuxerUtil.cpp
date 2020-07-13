//
// Created by KE TU on 2020-07-13.
//

#include "Mp4DemuxerUtil.h"
#include "ffmpeg_demuxer.h"
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <malloc.h>


#define NUM_METHODS(x) ((int)(sizeof(x)/ sizeof(x[0])))

// 虚拟机
JavaVM*  global_jvm;
// 全局回调
jobject  global_callback;
// 全局 退出标志
int global_exit_flag = 0;

// 声明方法
void *demuxer_thread(void *args);


/**
 * 关联方法
 */
static JNINativeMethod methods[] = {
        {"startDemuxerMp4", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Lcom/yingke/ffmpeg/demuxer/Mp4DemuxerUtil$Callback;)I", (void *)startMp4Demuxer},
        {"stopDemuxerMp4", "()V", (void *)stopMp4Demuxer}
};

/**
 * 动态注册
 */
extern "C" JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* jvm, void* reserved){
    global_jvm = jvm;

    // JVM中获取 JNIEnv 环境变量
    JNIEnv *env = NULL;
    if(JNI_OK != jvm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6)) {
        LOGE("Get JNIEnv failed");
        return JNI_ERR;
    }

    // 注册Native方法
    jclass clz = env->FindClass("com/yingke/ffmpeg/demuxer/Mp4DemuxerUtil");
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
 * 开始 解复用
 *
 * @param jniEnv
 * @param clz
 * @param mp4Path
 * @param h264Path
 * @param aacPath
 * @param callback
 * @return
 */
jint startMp4Demuxer(JNIEnv *jniEnv, jclass clz, jstring mp4Path, jstring h264Path, jstring aacPath, jobject callback){
    global_exit_flag = 0;
    if(!mp4Path) {
        LOGE("mp4Path can not be null");
        return -1;
    }

    if(callback) {
        global_callback = jniEnv->NewGlobalRef(callback);
    }

     char *c_mp4_path = const_cast<char *>(jniEnv->GetStringUTFChars(mp4Path, NULL));
     char *c_h264_path = const_cast<char *>(jniEnv->GetStringUTFChars(h264Path, NULL));
     char *c_aac_path = const_cast<char *>(jniEnv->GetStringUTFChars(aacPath, NULL));

    if(!c_mp4_path || !c_h264_path || !c_aac_path) {
        LOGE("url or h264 or c_aac path can not be null");
        return NULL;
    }
    // 构造参数
    inputParams *params = static_cast<inputParams *>(malloc(sizeof(inputParams)));
    params->mp4_path = c_mp4_path;
    params->h264_path = c_h264_path;
    params->aac_path = c_aac_path;

    // 创建线程
    pthread_t thread_id = 0;
    pthread_create(&thread_id, NULL, demuxer_thread, params);

    return 0;

}

/**
 * 解复用 线程 执行方法
 * @param args
 * @return
 */
void *demuxer_thread(void *args){
    // 主线程与子线程分离
    // 子线程结束后，资源自动回收
    pthread_detach(pthread_self());

    inputParams *params = static_cast<inputParams *>(args);
    if (!params){
        LOGE("pass params to demuxer thread failed");
        return NULL;
    }

    // 从Java VM中获取JNIEnv，将当前线程绑定到JavaVM，
    JNIEnv* env = NULL;
    jmethodID callback_Method_id = NULL;

    if (global_jvm && global_callback){
        if (global_jvm->GetEnv(reinterpret_cast<void **>(env), JNI_VERSION_1_6) > 0){
            LOGE("get JNIEnv from JVM failed.");
            return NULL;
        }

        if (JNI_OK != global_jvm->AttachCurrentThread(&env, NULL)){
            LOGE("attach thread failed");
            return NULL;
        }
        // 回调类 的class
        jclass callback_clz = env->GetObjectClass(global_callback);
        // 回调实例 的method id
        callback_Method_id = env->GetMethodID(callback_clz, "code", "(I)V");
    }

    LOGI("#### input url = %s", params->mp4_path);
    int ret = initFFmpegEngine(params->mp4_path);
    if (ret < 0) {
        if (params) {
            free(params->mp4_path);
            free(params->h264_path);
            free(params->aac_path);
            free(params);
        }

        if(callback_Method_id && global_jvm) {
            // 回调
            env->CallVoidMethod(global_callback, callback_Method_id, -1);
            env->DeleteGlobalRef(global_callback);
            // 解绑线程
            global_jvm->DetachCurrentThread();
        }
        return NULL;
    }

    // 打开文件
    LOGI("#### h264 save path = %s", params->h264_path);
    LOGI("#### aac save path = %s", params->aac_path);

    FILE * h264file = fopen(params->h264_path, "wb+");
    FILE * aacfile = fopen(params->aac_path, "wb+");

    if (h264file == NULL || aacfile == NULL){
        LOGE("open save file failed");
        if (params) {
            free(params->mp4_path);
            free(params->h264_path);
            free(params->aac_path);
            free(params);
        }
        releaseDemuxerFFmpeg();
        if(callback_Method_id && global_jvm) {
            // 回调
            env->CallVoidMethod(global_callback, callback_Method_id, -2);
            env->DeleteGlobalRef(global_callback);
            // 解绑线程
            global_jvm->DetachCurrentThread();
        }
        return NULL;
    }

    int packet_size = -1;
    int audio_profile = getAudioProfile();
    int rate_index = getAudioSampleRateIndex();
    int audio_channels = getAudioChannels();

    // 回调
    if(callback_Method_id) {
        env->CallVoidMethod(global_callback, callback_Method_id, 0);
    }
    bool is_reading = false;

    // 循环 读packet
    while ((packet_size = readDataFromAVPacket()) > 0){
        // 强制退出循环
        if (global_exit_flag){
            break;
        }

        if (!is_reading){
            is_reading = true;
            // 回调
            if(callback_Method_id) {
                env->CallVoidMethod(global_callback, callback_Method_id, 1);
            }
        }

        // 分配一个buffer，设置0
        uint8_t *out_buffer = (uint8_t *)malloc(packet_size * sizeof(uint8_t));
        memset(out_buffer, 0, packet_size * sizeof(uint8_t));

        /**
         * 处理读出的packet
         * 1,视频packet进过filter处理加 sps, pps
         * 2, 音频packet，暂不处理，后面加aac的adts头字节
         */
        int stream_index = handleAvPacketData(out_buffer, packet_size);
        if(stream_index < 0) {
            continue;
        }

        if (stream_index == getVideoStreamIndex()) {
            LOGI("--->write a h264 data，size = %d", packet_size);
            fwrite(out_buffer, packet_size, 1, h264file);
        } else if (stream_index == getAudioStreamIndex()){
            // 添加adts头部
            int adtslen = 7;
            uint8_t *ret = (uint8_t *)malloc(packet_size * sizeof(uint8_t) + adtslen * sizeof(char));
            memset(ret, 0, packet_size * sizeof(uint8_t) + adtslen * sizeof(char));
            char * adts = (char *)malloc(adtslen * sizeof(char));
            adts[0] = 0xFF;
            adts[1] = 0xF1;
            adts[2] = (((audio_profile - 1) << 6) + (rate_index << 2) + (audio_channels >> 2));
            adts[3] = (((audio_channels & 3) << 6) + (packet_size >> 11));
            adts[4] = ((packet_size & 0x7FF) >> 3);
            adts[5] = (((packet_size & 7) << 5) + 0x1F);
            adts[6] = 0xFC;

            memcpy(ret, adts, adtslen);
            memcpy(ret+adtslen, out_buffer, packet_size);
            fwrite(ret, packet_size + adtslen, 1, aacfile);
            free(adts);
            free(ret);

            LOGI("--->write a aac data, header = %d, size = %d", adtslen, packet_size);
        }
        free(out_buffer);
    }

    if (h264file){
        fclose(h264file);
    }

    if (aacfile){
        fclose(aacfile);
    }

    if(params) {
        free(params->mp4_path);
        free(params->h264_path);
        free(params->aac_path);
        free(params);
    }
    releaseDemuxerFFmpeg();
    if(global_callback && global_jvm) {
        env->CallVoidMethod(global_callback, callback_Method_id, 2);
        env->DeleteGlobalRef(global_callback);
        global_jvm->DetachCurrentThread();
    }
    LOGI("##### save success.");
    return NULL;
}



void stopMp4Demuxer(JNIEnv *jniEnv, jclass clz){
    global_exit_flag = 1;
}

