package com.yingke.yuv2rgb;

import android.view.Surface;

/**
 * 功能：
 * </p>
 * <p>Copyright xxx.xxx.com 2020 All right reserved </p>
 *
 * @author tuke 时间 2020-07-19
 * @email <p>
 * 最后修改人：无
 * <p>
 */
public class NativeYUV2RGB {

    static {
        System.loadLibrary("yingke-yuv2rgb");
    }

    /**
     * 转换类型
     */
    public interface Type {
        int RGB24_TO_YUV20P = -1;
        int YUV420P_TO_RGB24 = 0;
        int NV12_TO_RGB24 = 1;
        int NV21_TO_RGB24 = 2;
    }

    /**
     *  yuv 转 rgb
     * @param imagePath
     * @param type
     * @param width
     * @param height
     * @param surface
     */
    public static native void yuv2rgb(String imagePath, int type, int width, int height, Surface surface);

    /**
     *  rgb 转 yuv 文件
     * @param rgb24Path
     * @param yuv420pPath
     * @param width
     * @param height
     */
    public static native void rgb2yuv(String rgb24Path, String yuv420pPath, int width, int height);

}
