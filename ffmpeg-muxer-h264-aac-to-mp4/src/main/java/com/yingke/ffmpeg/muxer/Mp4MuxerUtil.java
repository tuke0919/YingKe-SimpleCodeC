package com.yingke.ffmpeg.muxer;

/**
 * 功能：
 * </p>
 * <p>Copyright xxx.xxx.com 2020 All right reserved </p>
 *
 * @author tuke 时间 2020-07-14
 * @email <p>
 * 最后修改人：无
 * <p>
 */
public class Mp4MuxerUtil {

    static {
        System.loadLibrary("yingke-h264-aac-to-mp4");
    }

    /**
     * 封装成mp4
     * @param h264Path
     * @param aacPath
     * @param outputPath
     * @return
     */
    public static native int startMuxerMp4(String h264Path, String aacPath, String outputPath);
}
