package com.yingke.decode.mp4;

/**
 * 功能：
 * </p>
 * <p>Copyright xxx.xxx.com 2020 All right reserved </p>
 *
 * @author tuke 时间 2020-07-04
 * @email <p>
 * 最后修改人：无
 * <p>
 */
public class Mp4Decoder {

    static {
        System.loadLibrary("yingke-mp4-to-yuv");
    }

    public native int decode(String mp4Path, String yuvPath);

}
