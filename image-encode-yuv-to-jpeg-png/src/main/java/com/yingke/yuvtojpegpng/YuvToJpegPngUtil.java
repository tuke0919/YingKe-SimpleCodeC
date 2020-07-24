package com.yingke.yuvtojpegpng;

/**
 * 功能：Yuv 原始图像 转换 成 压缩的 jpeg，png图像文件
 *
 * </p>
 * <p>Copyright xxx.xxx.com 2020 All right reserved </p>
 *
 * @author tuke 时间 2020-07-24
 * @email <p>
 * 最后修改人：无
 * <p>
 */
public class YuvToJpegPngUtil {

    static {
        System.loadLibrary("yingke-yuv-to-jpeg-png");
    }

    /**
     * yuv 原始图像 转换成 压缩的jpeg图像文件
     *
     * @param yuvPath
     * @param jpegPath
     * @param width
     * @param height
     */
    public static native void yuv2jpeg(String yuvPath, String jpegPath, int width, int height);

    /**
     * yuv 原始图像 转换成 压缩的png图像文件
     *
     * @param yuvPath
     * @param pngPath
     * @param width
     * @param height
     */
    public static native void yuv2png(String yuvPath, String pngPath, int width, int height);


}
