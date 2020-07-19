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
        {"yuv2rgb", "(Ljava/lang/String;IIILandroid/view/Surface;)V", (void *) yuv2rgb},
        {"rgb2yuv", "(Ljava/lang/String;Ljava/lang/String;II)V", (void *) rgb2yuv}
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
    TYPE_RGB24_TO_YUV20P = -1,
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
    // y,u,v 分量在内存中 的 开始地址
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
    // y,u,v 分量在内存中 的 开始地址
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
    // y,u,v 分量在内存中 的 开始地址
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
 *  yuv 转 rgb
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

    // 重点：yuv_path -> yuv_data -> rgb_data -> buffer
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




unsigned char ClipValue(unsigned char x, unsigned char min_val, unsigned char max_val) {
    if (x > max_val) {
        return max_val;
    } else if (x < min_val) {
        return min_val;
    } else {
        return x;
    }
}


/**
 * RGB24 转 YUV420P
 *
 * @param rgb24
 * @param width
 * @param height
 * @param yuv420p
 */
void RGB24_TO_YUV420P(unsigned char *rgb24, unsigned char *yuv420p, int width, int height) {

    unsigned char *ptrY, *ptrU, *ptrV;
    memset(yuv420p, 0, width * height * 3 / 2);
    // Y 开始地址
    ptrY = yuv420p;
    // U 开始地址
    ptrU = yuv420p + width * height;
    // V 开始地址
    ptrV = yuv420p + (width * height * 5 / 4);
    unsigned char y, u, v, r, g, b;
    int index = 0;
    // 遍历 rbg图像，每个pix 是3个字节
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            // 每个pix 内存位置
            index = width * j * 3 + i * 3;
            // r，g，b 分量位置
            r = rgb24[index];
            g = rgb24[index + 1];
            b = rgb24[index + 2];
            // 每个pix 转化 成 y，u，v
            y = (unsigned char) ((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
            u = (unsigned char) ((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
            v = (unsigned char) ((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;

            // 采样 每个4个y，共用一组u，v分量
            // 全部y
            *(ptrY++) = ClipValue(y, 0, 255);
            if (j % 2 == 0 && i % 2 == 0) {
                // 偶行列 采样u
                *(ptrU++) = ClipValue(u, 0, 255);
            } else if (i % 2 == 0) {
                // 偶行 采样v
                *(ptrV++) = ClipValue(v, 0, 255);
            }

        }
    }
}


void rgb24_to_yuv420p(const char *rgbPath, const char *yuvPath, int width, int height) {
    FILE *fp_rgb = fopen(rgbPath, "rb+");
    FILE *fp_yuv = fopen(yuvPath, "wb+");

    unsigned char *rgb24_data = (unsigned char *) malloc(width * height * 3);
    unsigned char *yuv420_data = (unsigned char *) malloc(width * height * 3 / 2);

    // 读入rgb24数据
    fread(rgb24_data, 1, width * height * 3, fp_rgb);
    // 转换
    RGB24_TO_YUV420P(rgb24_data, yuv420_data, width, height );
    // 写入yuv420p
    fwrite(yuv420_data, 1, width * height * 3 / 2, fp_yuv);

    free(rgb24_data);
    free(yuv420_data);
    fclose(fp_rgb);
    fclose(fp_yuv);
}



/**
 *  yuv 转 rgb
 *
 * @param env
 * @param thiz
 * @param yuvPath
 * @param type
 * @param width
 * @param height
 * @param surface
 */
void rgb2yuv(JNIEnv *env,
             jclass thiz,
             jstring rgb24Path,
             jstring yuv420pPath,
             jint width,
             jint height
             ) {

    const char *rgb24_path = env->GetStringUTFChars(rgb24Path, NULL);
    const char *yuv420p_path = env->GetStringUTFChars(yuv420pPath, NULL);


    rgb24_to_yuv420p(rgb24_path, yuv420p_path, width, height);


    env->ReleaseStringUTFChars(rgb24Path, rgb24_path);
    env->ReleaseStringUTFChars(yuv420pPath, yuv420p_path);

}

