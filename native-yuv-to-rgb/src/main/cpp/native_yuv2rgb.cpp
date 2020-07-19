//
// Created by KE TU on 2020-07-19.
//

#include "logger.h"
#include "native_yuv2rgb.h"
#include "android/native_window_jni.h"
#include "android/native_window.h"
#include <malloc.h>
#include <string.h>

#define NUM_METHODS(x) ((int)(sizeof(x)/ sizeof(x[0])))

/**
 * 关联方法
 */
static JNINativeMethod methods[] = {
        {"yuv2rgb", "(Ljava/lang/String;IIILandroid/view/Surface;)V", (void *) yuv2rgb}
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
    jclass clz = env->FindClass("com/yingke/yuv2rgb/NativeYUV2RGB");
    int  ret = env->RegisterNatives(clz, methods, NUM_METHODS(methods));
    if (ret < 0){
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}

void ThrowException (JNIEnv *env, char *exception, char * msg ){
    jclass thiz = env->FindClass(exception);
    if (NULL != thiz) {
        env->ThrowNew(thiz, msg);
    }

}

enum Type{
    TYPE_YUV420P_TO_RGB24 = 0,
    TYPE_NV12_TO_RGB24 = 1,
    TYPE_NV21_TO_RGB24 = 2
};

/**
 * YUV420P转RGB24
 *
 * @param yuv_data   src yuv数据地址
 * @param rgb24_data dst rgb空间地址
 * @param width
 * @param height
 */
void YUV420P_TO_RGB24(unsigned char *yuv_data, unsigned char *rgb24_data, int width, int height) {
    int index = 0;
    unsigned char *y_base = yuv_data;
    unsigned char *u_base = yuv_data + width * height;
    unsigned char *v_base = yuv_data + width * height * 5 / 4;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // YYYYYYYYUUVV
            // 根据 图像坐标 ，确定 内存中Y，U，V分量位置
            u_char  Y = y_base[x + y * width];
            u_char  U = u_base[y / 2 * width / 2 + (x / 2)];
            u_char  V = v_base[y / 2 * width / 2 + (x / 2)];

            // 转换公式
            rgb24_data[index++] = Y + 1.402 * (V - 128); //R
            rgb24_data[index++] = Y - 0.34413 * (U - 128) - 0.71414 * (V - 128); //G
            rgb24_data[index++] = Y + 1.772 * (U - 128); //B
        }
    }

}

/**
 *
 * NV12属于YUV420SP格式
 * NV12转RGB24
 *
 * @param yuv_data
 * @param rgb24_data
 * @param width
 * @param height
 */
void NV12_TO_RGB24(unsigned char *yuv_data, unsigned char *rgb24_data, int width, int height) {
    int index = 0;
    unsigned char *y_base = yuv_data;
    unsigned char *u_base = yuv_data + width * height;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            //YYYYYYYYUVUV
            // 根据 图像坐标 ，确定 内存中Y，U，V分量位置
            u_char Y = y_base[x + y * width];
            u_char U = u_base[y / 2 * width + (x / 2) * 2];
            u_char V = u_base[y / 2 * width + (x / 2) * 2 + 1];

            // 转换公式
            rgb24_data[index++] = Y + 1.402 * (V - 128); //R
            rgb24_data[index++] = Y - 0.34413 * (U - 128) - 0.71414 * (V - 128); //G
            rgb24_data[index++] = Y + 1.772 * (U - 128); //B
        }
    }

}

/**
 * NV21属于YUV420SP格式
 * NV21转RGB24
 *
 * @param yuv_data
 * @param rgb24_data
 * @param width
 * @param height
 */
void NV21_TO_RGB24(unsigned char *yuv_data, unsigned char *rgb24_data, int width, int height) {
    int index = 0;
    unsigned char *y_base = yuv_data;
    unsigned char *u_base = yuv_data + width * height;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            //YYYYYYYYVUVU
            // 根据 图像坐标 ，确定 内存中Y，U，V分量位置
            u_char Y = y_base[x + y * width];
            u_char U = u_base[y / 2 * width + (x / 2) * 2 + 1];
            u_char V = u_base[y / 2 * width + (x / 2) * 2];

            // 转换公式
            rgb24_data[index++] = Y + 1.402 * (V - 128); //R
            rgb24_data[index++] = Y - 0.34413 * (U - 128) - 0.71414 * (V - 128); //G
            rgb24_data[index++] = Y + 1.772 * (U - 128); //B

        }
    }
}

/**
 *
 * @param yuv_path
 * @param type
 * @param width
 * @param height
 * @param buffer
 */
void drawYUV(const char *yuv_path, int type, int width, int height, ANativeWindow_Buffer buffer) {
     // 打开yuv文件
     FILE *yuv_file = fopen(yuv_path, "rb");
     // 分配yuv数据空间
     int yuv_data_length = width * height * 3 / 2;
     unsigned char *yuv_data = new unsigned char[yuv_data_length];
     // 读yuv文件 到 yuv_data
     fread(yuv_data, 1, yuv_data_length, yuv_file);

     // 分配 rgb_data空间
     int rgb_data_length = width * height * 3;
     unsigned char *rgb24_data = new unsigned char[rgb_data_length];

    // YUV转RGB24
    switch (type) {
        case TYPE_YUV420P_TO_RGB24:
            // YUV420P转RGB24;
            YUV420P_TO_RGB24(yuv_data, rgb24_data, width, height);

            break;
        case TYPE_NV12_TO_RGB24:
            // YUV420SP转RGB24
            NV12_TO_RGB24(yuv_data, rgb24_data, width, height);
            break;
        case TYPE_NV21_TO_RGB24:
            // YUV420SP转RGB24
            NV21_TO_RGB24(yuv_data, rgb24_data, width, height);

            break;
    }

    // rgb24数据 拷贝到 window 的buffer中
    uint32_t *line = (uint32_t *) buffer.bits;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;
            line[x] = rgb24_data[index * 3 + 2] << 16
                      | rgb24_data[index * 3 + 1] << 8
                      | rgb24_data[index * 3];
        }
        line = line + buffer.stride;
    }

    // 释放内存
    delete[] yuv_data;
    delete[] rgb24_data;

    // 关闭文件句柄
    fclose(yuv_file);

}

/**
 *
 * @param env
 * @param thiz
 * @param yuvPath
 * @param type
 * @param width
 * @param height
 * @param surface
 */
void yuv2rgb(JNIEnv *env,
        jclass thiz,
        jstring yuvPath,
        jint type,
        jint width,
        jint height,
        jobject surface) {

    const char *yuv_path = env->GetStringUTFChars(yuvPath, NULL);
    // 1, 创建 surface
    ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
    if (NULL == window) {
        ThrowException(env, "java/lang/RuntimeException", "unable to get native window");
        return;
    }

    // 2，修改像素 格式 默认的是RGB_565
    int32_t  result = ANativeWindow_setBuffersGeometry(window,  0, 0, WINDOW_FORMAT_RGBA_8888);
    if (result < 0) {
        ThrowException(env, "java/lang/RuntimeException", "unable to set buffers geometry");
        //释放窗口
        ANativeWindow_release(window);
        window = NULL;
        return;
    }
    // 3, 聚焦window
    ANativeWindow_acquire(window);


    // 4，锁定窗口 绘制表面，准备绘制
    ANativeWindow_Buffer buffer;
    // 图像数据 填充在 buffer中
    if (ANativeWindow_lock(window, &buffer, NULL) < 0) {
        ThrowException(env, "java/lang/RuntimeException", "unable to lock native window");
        // 释放窗口
        ANativeWindow_release(window);
        window = NULL;
        return;
    }

    // 重点：
    drawYUV(yuv_path, type, width, height, buffer);


    // 5，解锁窗口的绘图表面
    if (ANativeWindow_unlockAndPost(window) < 0) {
        ThrowException(env, "java/lang/RuntimeException",
                       "unable to unlock and post to native window");
    }

    env->ReleaseStringUTFChars(yuvPath, yuv_path);
    // 释放
    ANativeWindow_release(window);

}

