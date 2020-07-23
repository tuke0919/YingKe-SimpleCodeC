package com.yingke.libyuv2rgb;

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
public class LibYUV2RGB {

    static {
        System.loadLibrary("yingke-libyuv2rgb");
    }

    /**
     * 转换类型
     */
    public interface Type {
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

}
