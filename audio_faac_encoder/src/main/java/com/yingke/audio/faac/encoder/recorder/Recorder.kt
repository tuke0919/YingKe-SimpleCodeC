package com.yingke.audio.mp3lame.encoder

/**
 * 功能：
 * </p>
 * <p>Copyright xxx.xxx.com 2019 All right reserved </p>
 *
 * @author tuke 时间 2020-06-18
 * @email
 * <p>
 * 最后修改人：无
 * <p>
 */
interface Recorder {

    /**
     * @param listener
     */
    fun setAudioListener(listener: RecorderListener)

    /**
     * @param path
     */
    fun startRecord(path: String)

    /**
     * 暂停 录制
     */
    fun pauseRecord()

    /**
     * 继续录制
     */
    fun resumeRecord()

    /**
     * 停止录制
     * @return 录制时长 ms
     */
    fun stopRecord(): Long

}