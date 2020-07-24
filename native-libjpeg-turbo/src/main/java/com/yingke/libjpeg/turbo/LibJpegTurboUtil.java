package com.yingke.libjpeg.turbo;

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
public class LibJpegTurboUtil {

    static {
        System.loadLibrary("yingke-libjpeg-turbo");
    }

    /**
     * libjpeg-turbo 解码 jpeg图像，并在surface上展示
     *
     * @param imagePath
     * @param surface
     */
    public static native void loadJPEGImage(String imagePath, Surface surface);

}
