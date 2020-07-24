//
// Created by KE TU on 2020-07-24.
//

#include "libpng_util.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <malloc.h>
#include <string.h>

#include <stdio.h>
#include <time.h>

// 两个头文件
#include "png.h"
#include "zlib.h"


#include "logger.h"
#define NUM_METHODS(x) ((int)(sizeof(x)/ sizeof(x[0])))

/**
 * 关联方法
 */
static JNINativeMethod methods[] = {
        {"loadPNGImage", "(Ljava/lang/String;Landroid/view/Surface;)V", (void *)loadPNGImage},
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
    jclass clz = env->FindClass("com/yingke/libpng/LibPngUtil");
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
 * 解码png
 *
 * @param name
 * @param nwBuffer
 */
void drawPNG(const char *png_path, ANativeWindow_Buffer &nwBuffer) {
    FILE *file = fopen(png_path, "rb");

    // 初始化结构 指针
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if (png_ptr == NULL) {
        fclose(file);
        return;
    }

    // 初始化 pnginfo 指针
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        fclose(file);
        png_destroy_write_struct(&png_ptr, NULL);
        return;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        fclose(file);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return;
    }

    // 重要，加载文件
    png_init_io(png_ptr, file);
    // 读文件信息，存放在info_ptr
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);

    // 文件的宽和高
    int m_width = png_get_image_width(png_ptr, info_ptr);
    int m_height = png_get_image_height(png_ptr, info_ptr);

    // 获取文件的 颜色类型
    int color_type = png_get_color_type(png_ptr, info_ptr);

    LOGI("width = %d, height = %d, color_type = %d", m_width, m_height, color_type);

    // row_pointers里边就是传说中的rgba数据了
    png_bytep *row_pointers = png_get_rows(png_ptr, info_ptr);

    switch (color_type) {
        case PNG_COLOR_TYPE_RGB_ALPHA:
            // RGB32
            // 拷贝！！注意，如果你读取的png没有A通道，就要3位3位的读。还有就是注意字节对其的问题，最简单的就是别用不能被4整除的宽度就行了。读过你实在想用，就要在这里加上相关的对齐处理。
            // 申请个内存玩玩，这里用的是c++语法，甭想再c上编过
//            int size = m_height * m_width * 4;
//            unsigned char* bgra = new unsigned char[size];
//            int pos = 0;
//            for(int i = 0; i < m_height; i++)
//            {
//                for(int j = 0; j < (4 * m_width); j += 4)
//                {
//                    bgra[pos++] = row_pointers[i][j + 2]; // blue
//                    bgra[pos++] = row_pointers[i][j + 1]; // green
//                    bgra[pos++] = row_pointers[i][j];   // red
//                    bgra[pos++] = row_pointers[i][j + 3]; // alpha
//                }
//            }

            break;
        case PNG_COLOR_TYPE_RGB:
            // RGB24 没有A通道，就要3位3位的读
            uint32_t *line = (uint32_t *) nwBuffer.bits;
            for (int row = 0; row < m_height; row++) {
                for (int column = 0; column < m_width; column++) {
                    // 存储顺序为BGR,BGR,BGR......
                    line[column] = ((uint32_t) row_pointers[row][3 * column + 2]) << 16
                                   | ((uint32_t) row_pointers[row][3 * column + 1]) << 8
                                   | (uint32_t) row_pointers[row][3 * column];
                }
                line = line + nwBuffer.stride;
            }
            break;
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, 0);
    fclose(file);

}

/**
 *
 * @param env
 * @param thiz
 * @param pngPath
 * @param surface
 */
void loadPNGImage(JNIEnv *env, jclass thiz, jstring pngPath, jobject surface) {

    const char *png_path = env->GetStringUTFChars(pngPath, 0);

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

    // 重点：解码png
    drawPNG(png_path, buffer);

    // 5，解锁窗口的绘图表面
    if (ANativeWindow_unlockAndPost(window) < 0) {
        ThrowException(env, "java/lang/RuntimeException","unable to unlock and post to native window");
    }

    env->ReleaseStringUTFChars(pngPath, png_path);
    // 释放
    ANativeWindow_release(window);




}











