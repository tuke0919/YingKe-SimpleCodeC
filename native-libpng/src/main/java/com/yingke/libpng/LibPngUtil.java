package com.yingke.libpng;

import android.view.Surface;

/**
 * 功能：
 * </p>
 * <p>Copyright xxx.xxx.com 2020 All right reserved </p>
 *
 * @author tuke 时间 2020-07-24
 * @email <p>
 * 最后修改人：无
 * <p>
 */
public class LibPngUtil {

    static {
        System.loadLibrary("yingke-libpng");
    }


    /**
     * libpng 解码 png图像文件 并显示
     * @param imagePath
     * @param surface
     */
    public static native void loadPNGImage(String imagePath, Surface surface);
}
