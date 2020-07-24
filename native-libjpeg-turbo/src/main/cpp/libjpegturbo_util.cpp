//
// Created by KE TU on 2020-07-24.
//
#include <stdio.h>
#include <time.h>
#include "libjpegturbo_util.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>

#include <jpeglib.h>
#include <malloc.h>
#include <string.h>

#include "logger.h"
#define NUM_METHODS(x) ((int)(sizeof(x)/ sizeof(x[0])))

/**
 * 关联方法
 */
static JNINativeMethod methods[] = {
        {"loadJPEGImage", "(Ljava/lang/String;Landroid/view/Surface;)V", (void *)loadJPEGImage},
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
    jclass clz = env->FindClass("com/yingke/libjpeg/turbo/LibJpegTurboUtil");
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
 * 画 jpeg
 * @param jpeg_path
 * @param nwBuffer  window 的buffer
 * @return
 */
int drawJPEG(const char *jpeg_path, ANativeWindow_Buffer &nwBuffer) {

    // 1, 解压信息结构
    jpeg_decompress_struct jpegInfo;
    // 2，解压错误结构
    jpeg_error_mgr jpegError;

    // 3，输入文件
    FILE *input_file;
    // 4，读入一行像素的 起始地址
    JSAMPARRAY buffer;
    // 5，内存中的 一个行像素占的宽度
    int row_width;

    unsigned char *pixel;

    jpegInfo.err = jpeg_std_error(&jpegError);

    // 6，读入jpeg文件
    if ((input_file = fopen(jpeg_path, "rb")) == NULL){
        fprintf(stderr, "can't open %s\n", input_file);
        LOGE("open file error");
        return -1;
    }

    // 7，初始化对象信息
    jpeg_create_decompress(&jpegInfo);

    // 8, 指定源jpeg文件
    jpeg_stdio_src(&jpegInfo, input_file);

    // 9, 读取文件头信息,设置默认的解压参数
    jpeg_read_header(&jpegInfo, TRUE);

    // 10， 开始解压
    jpeg_start_decompress(&jpegInfo);

    // 一行像素的 占的字节数
    row_width = jpegInfo.output_width * jpegInfo.output_components;
    // 分配
    buffer = (*jpegInfo.mem->alloc_sarray)((j_common_ptr) &jpegInfo, JPOOL_IMAGE, row_width, 1);

    pixel = (unsigned char *) malloc(row_width);
    memset(pixel, 0, row_width);

    // window buffer 的指针
    uint32_t *line  = (uint32_t *)(nwBuffer.bits);
    for (int i = 0; i < jpegInfo.output_height; i++) {
        // 读取一行rgb数据 存在buffer
        jpeg_read_scanlines(&jpegInfo, buffer, 1);
        // 一行数据的起始地址
        pixel = *buffer;

        for (int j = 0; j < jpegInfo.output_width ; j++) {
            // 存储顺序为BGR,BGR,BGR......
            line[j] = ((uint32_t) pixel[3 * j + 2]) << 16
                      | ((uint32_t) pixel[3 * j + 1] << 8)
                      | ((uint32_t) (pixel[3 * j + 0]));
        }

        line = line + nwBuffer.stride;
    }

    // 释放
    free(pixel);
    // 完成解压
    jpeg_finish_decompress(&jpegInfo);
    // 销毁解压信息
    jpeg_destroy_decompress(&jpegInfo);
    // 关闭文件句柄
    fclose(input_file);

    return 0;
}


/**
 *
 * @param env
 * @param thiz
 * @param jpegPath
 * @param surface
 */
void loadJPEGImage(JNIEnv *env, jclass thiz, jstring jpegPath, jobject surface) {

    const char *jpeg_path = env->GetStringUTFChars(jpegPath, 0);

    // 1, 创建 surface
    ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
    if (NULL == window) {
        ThrowException(env, "java/lang/RuntimeException", "unable to get native window");
        return;
    }
    // 2，修改像素 格式 默认的是RGB_565，转化成8888，一个像素4个字节的
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

    // 重点：绘制JPEG图片
    drawJPEG(jpeg_path, buffer);

    // 5，解锁窗口的绘图表面
    if (ANativeWindow_unlockAndPost(window) < 0) {
        ThrowException(env, "java/lang/RuntimeException",
                       "unable to unlock and post to native window");
    }

    env->ReleaseStringUTFChars(jpegPath, jpeg_path);
    // 释放
    ANativeWindow_release(window);


}






