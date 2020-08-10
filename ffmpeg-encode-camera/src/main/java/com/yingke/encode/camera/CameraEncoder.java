package com.yingke.encode.camera;

/**
 * 功能：
 * </p>
 * <p>Copyright xxx.xxx.com 2020 All right reserved </p>
 *
 * @author tuke 时间 2020-08-10
 * @email <p>
 * 最后修改人：无
 * <p>
 */
public class CameraEncoder {

    static {
        System.loadLibrary("yingke-camera");
    }

    /**
     * 接收 相机数据
     *
     * @param yuvData
     * @param width
     * @param height
     */
    public static native void onPreviewFrame(byte[] yuvData, int width, int height);

    /**
     * 编码 mp4 开始
     * @param mp4Path
     * @param width
     * @param height
     */
    public static native void encodeMp4Start(String mp4Path, int width, int height);

    /**
     * 编码mp4 结束
     */
    public static native void encodeMp4Stop();

    /**
     * 编码 成jpeg
     * @param jpegPath
     * @param width
     * @param height
     */
    public native void encodeJPEG(String jpegPath, int width, int height);

}
