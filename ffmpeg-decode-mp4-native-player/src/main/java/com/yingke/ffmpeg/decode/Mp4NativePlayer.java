package com.yingke.ffmpeg.decode;

/**
 * 功能：使用ffmpeg 解码播放mp4
 * </p>
 * <p>Copyright xxx.xxx.com 2020 All right reserved </p>
 *
 * @author tuke 时间 2020-07-14
 * @email <p>
 * 最后修改人：无
 * <p>
 */
public class Mp4NativePlayer {

    static {
        System.loadLibrary("yingke-mp4-native-player");
    }

    /**
     * 播放mp4
     * @param videoPath
     * @param surface
     * @return
     */
    public static native int startMp4Player(String videoPath, Object surface);
}
