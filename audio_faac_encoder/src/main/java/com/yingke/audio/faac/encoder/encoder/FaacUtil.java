package com.yingke.audio.faac.encoder.encoder;

/**
 * 功能：
 * </p>
 * <p>Copyright xxx.xxx.com 2020 All right reserved </p>
 *
 * @author tuke 时间 2020-06-30
 * @email <p>
 * 最后修改人：无
 * <p>
 */
public class FaacUtil {

    static {
        System.loadLibrary("yingke-faac");
    }

    public native static long[] faacOpen(int sampleRate, int channels);

    public native static int faacEncode(short[] pcmBuffer, int samples, byte[] aacBuffer);

    public native static void faacClose();


}
