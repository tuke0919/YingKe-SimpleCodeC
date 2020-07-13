package com.yingke.ffmpeg.demuxer;

/**
 * 功能：
 * </p>
 * <p>Copyright xxx.xxx.com 2020 All right reserved </p>
 *
 * @author tuke 时间 2020-07-13
 * @email <p>
 * 最后修改人：无
 * <p>
 */
public class Mp4DemuxerUtil {

    static {
        System.loadLibrary("yingke-mp4-to-h264-aac");
    }

    public interface Callback{

        int INIT_INPUT_FAILED  = -1;  // 初始化输入流失败
        int INIT_OUTPUT_FAILED = -2;  // 初始化输出文件失败
        int INIT_SUCCESS   = 0;       // 初始化成功
        int START_DEMUXER  = 1;       // 开始解复用
        int STOP_DEMUXER   = 2;       // 结束处理

        /**
         * @param code
         */
        void code(int code);
    }

    /**
     * @param mp4Path  mp4 路径
     * @param h264Path h264 路径
     * @param aacPath  aac 路径
     * @param callback 回调
     */
    public static native int startDemuxerMp4(String mp4Path, String h264Path, String aacPath, Callback callback);

    /**
     * 停止
     */
    public static native void stopDemuxerMp4();


}
