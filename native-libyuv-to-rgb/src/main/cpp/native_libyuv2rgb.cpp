

#include <jni.h>
#include <stdio.h>
#include <time.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <malloc.h>
#include <string.h>
#include "native_libyuv2rgb.h"
#include "logger.h"

// yuv库
#include <libyuv.h>

/**
 * 动态注册
 */
JNINativeMethod methods[] = {
        {"yuv2rgb", "(Ljava/lang/String;IIILandroid/view/Surface;)V", (void *) yuv2rgb}
};

/**
 * 动态注册
 * @param env
 * @return
 */
jint registerNativeMethod(JNIEnv *env) {
    jclass cl = env->FindClass("com/yingke/libyuv2rgb/LibYUV2RGB");
    if ((env->RegisterNatives(cl, methods, sizeof(methods) / sizeof(methods[0]))) < 0) {
        return -1;
    }
    return 0;
}

/**
 * 加载默认回调
 * @param vm
 * @param reserved
 * @return
 */
jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    //注册方法
    if (registerNativeMethod(env) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_6;
}

void ThrowException(JNIEnv *env, const char *exception, const char *message) {
    jclass clazz = env->FindClass(exception);
    if (NULL != clazz) {
        env->ThrowNew(clazz, message);
    }
}

enum Type {
    TYPE_YUV420P_TO_RGB24 = 0,
    TYPE_NV12_TO_RGB24 = 1,
    TYPE_NV21_TO_RGB24 = 2
};

/**
 * I420 是 yuv420p
 * YYYYYYYYUUVV
 * @param yuvData src yuv数据地址
 * @param rgb24   dst rgb空间地址
 * @param width
 * @param height
 */
void I420_TO_RGB24(unsigned char *yuvData, unsigned char *rgb24, int width, int height) {

    // y,u,v 分量在内存中 的 开始地址
    unsigned char *ybase = yuvData;
    unsigned char *ubase = &yuvData[width * height];
    unsigned char *vbase = &yuvData[width * height * 5 / 4];
    // YUV420P转RGB24
    libyuv::I420ToRGB24(
            ybase, width,
            ubase, width / 2,
            vbase, width / 2,
            rgb24,
            width * 3,
            width, height);

}

/**
 * NV12属于YUV420SP格式
 * NV12转RGB24
 *
 * @param yuvData
 * @param rgb24
 * @param width
 * @param height
 */
void NV12_TO_RGB24(unsigned char *yuvData, unsigned char *rgb24, int width, int height) {

    // y,u,v 分量在内存中 的 开始地址
    unsigned char *ybase = yuvData;
    unsigned char *uvbase = &yuvData[width * height];
    // NV12转RGB24
    libyuv::NV12ToRGB24(ybase, width,
            uvbase, width,
            rgb24,
            width * 3,
            width, height);

}

/**
 * NV21属于YUV420SP格式
 * NV21转RGB24
 * YYYYYYYYVUVU
 *
 * @param yuvData
 * @param rgb24
 * @param width
 * @param height
 */
void NV21_TO_RGB24(unsigned char *yuvData, unsigned char *rgb24, int width, int height) {

    // y,u,v 分量在内存中 的 开始地址
    unsigned char *ybase = yuvData;
    unsigned char *vubase = &yuvData[width * height];
    // NV21转RGB24
    libyuv::NV21ToRGB24(
            ybase, width,
            vubase, width,
            rgb24,
            width * 3,
            width, height);

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
    FILE *yuv_file = fopen(yuv_path, "rb");
    int yuv_data_length = width * height * 3 / 2;
    unsigned char *yuv_data = new unsigned char[yuv_data_length];
    // 读yuv文件 到 yuv_data
    fread(yuv_data, 1, yuv_data_length, yuv_file);

    // 分配 rgb_data空间
    unsigned char *rgb24 = new unsigned char[width * height * 3];

    // YUV转RGB24
    switch (type) {
        case TYPE_YUV420P_TO_RGB24:
            // YUV420P转RGB24
            I420_TO_RGB24(yuv_data, rgb24, width, height);
            break;
        case TYPE_NV12_TO_RGB24:
            // YUV420SP转RGB24
            NV12_TO_RGB24(yuv_data, rgb24, width, height);
            break;
        case TYPE_NV21_TO_RGB24:
            // YUV420SP转RGB24
            NV21_TO_RGB24(yuv_data, rgb24, width, height);
            break;
    }

    // rgb24数据 拷贝到 window 的buffer中
    uint32_t *line = (uint32_t *) buffer.bits;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;
            line[x] = rgb24[index * 3] << 16
                      | rgb24[index * 3 + 1] << 8
                      | rgb24[index * 3 + 2];
        }
        line = line + buffer.stride;
    }

    //释放内存
    delete[] yuv_data;
    delete[] rgb24;

    //关闭文件句柄
    fclose(yuv_file);
}

/**
 * yuv 转 rgb
 *
 * @param env
 * @param obj
 * @param yuvPath
 * @param type
 * @param width
 * @param height
 * @param surface
 */
void yuv2rgb(JNIEnv *env,
        jclass clazz,
        jstring yuvPath,
        jint type,
        jint width,
        jint height,
        jobject surface) {

    const char *yuv_path = env->GetStringUTFChars(yuvPath, 0);

    // 1, 创建 surface
    ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
    if (NULL == window) {
        ThrowException(env, "java/lang/RuntimeException", "unable to get native window");
        return;
    }
    // 2，修改像素 格式 默认的是RGB_565
    int32_t result = ANativeWindow_setBuffersGeometry(window, 0, 0, WINDOW_FORMAT_RGBA_8888);
    if (result < 0) {
        ThrowException(env, "java/lang/RuntimeException", "unable to set buffers geometry");
        //释放窗口
        ANativeWindow_release(window);
        window = NULL;
        return;
    }
    // 3, 聚焦window
    ANativeWindow_acquire(window);

    // 图像数据 填充在 buffer中
    ANativeWindow_Buffer buffer;
    // 4，锁定窗口 绘制表面，准备绘制
    if (ANativeWindow_lock(window, &buffer, NULL) < 0) {
        ThrowException(env, "java/lang/RuntimeException", "unable to lock native window");
        // 释放窗口
        ANativeWindow_release(window);
        window = NULL;
        return;
    }

    // 绘制YUV420P 重点：yuv_path -> yuv_data -> rgb_data -> buffer
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

