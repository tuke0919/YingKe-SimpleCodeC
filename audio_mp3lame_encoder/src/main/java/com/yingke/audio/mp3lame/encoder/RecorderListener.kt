package com.yingke.audio.mp3lame.encoder

/**
 * 功能：
 * </p>
 * <p>Copyright corp.netease.com 2019 All right reserved </p>
 *
 * @author tuke 时间 2020-06-18
 * @email  tuke@corp.netease.com
 * <p>
 * 最后修改人：无
 * <p>
 */
interface RecorderListener {

    /**
     * 获取录制音量的大小
     *
     * @param volume
     */
    fun onGetVolume(volume: Int)

}