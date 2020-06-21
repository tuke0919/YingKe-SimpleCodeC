package com.yingke.audio.mp3lame.encoder.recorder

import android.media.AudioFormat
import android.media.AudioFormat.*
import android.media.AudioRecord
import android.media.MediaRecorder
import android.os.Handler
import android.os.HandlerThread
import androidx.core.content.ContextCompat
import com.yingke.audio.mp3lame.encoder.Recorder
import com.yingke.audio.mp3lame.encoder.RecorderListener
import com.yingke.audio.mp3lame.encoder.encoder.LameUtil
import java.io.FileOutputStream
import java.io.IOException
import java.util.concurrent.Executors
import java.util.concurrent.Future
import java.util.jar.Manifest
import kotlin.math.sqrt

/**
 * 功能：
 * </p>
 * <p>Copyright xxx.xxx.com 2020 All right reserved </p>
 *
 * @author tuke 时间 2020-06-19
 * @email
 * <p>
 * 最后修改人：无
 * <p>
 */
class Mp3Recorder : Recorder{

    companion object{
        const val TAG = "Mp3Recorder"

        //=======================IAudioRecorder Default Settings=======================
        const val DEFAULT_AUDIO_SOURCE = MediaRecorder.AudioSource.MIC

        /**
         * 以下三项为默认配置参数。Google Android文档明确表明只有以下3个参数是可以在所有设备上保证支持的。
         * 模拟器仅支持从麦克风输入8kHz采样率
         */
        const val DEFAULT_SAMPLING_RATE = 44100
        const val DEFAULT_CHANNEL_CONFIG = AudioFormat.CHANNEL_IN_MONO
        const val DEFAULT_AUDIO_FORMAT = AudioFormat.ENCODING_PCM_16BIT

        //======================Lame Default Settings=====================
        const val DEFAULT_LAME_MP3_QUALITY = 7

        /**
         * 与DEFAULT_CHANNEL_CONFIG相关，因为是mono单声，所以是1
         */
        const val DEFAULT_LAME_IN_CHANNEL = 1

        /**
         * Encoded bit rate. MP3 file will be encoded with bit rate 32kbps
         */
        const val DEFAULT_LAME_MP3_BIT_RATE = 32

        /**
         * 自定义 每160帧(采样)作为一个周期，通知一下需要进行编码
         */
        const val FRAME_COUNT = 160
    }

    private var mAudioRecord: AudioRecord? = null

    private var mChildHandlerThread: HandlerThread? = null
    private var mChiHandler: Handler? = null

    private val esRecord = Executors.newSingleThreadExecutor()
    private var ftRecord: Future<*>? = null
    private var mIsRecording: Boolean = false;


    // record
    private var mRecordBufferSize: Int = 0
    private var mPCMBuffer: ShortArray? = null

    // encode
    private var mMp3Buffer: ByteArray? = null
    private var mFileOutputStream: FileOutputStream? = null

    private var listener: RecorderListener? = null
    private var startMillisecond: Long = 0

    init {
        initChildHandler()
    }


    private fun initChildHandler() {
        if (mChildHandlerThread == null) {
            mChildHandlerThread = HandlerThread("converMp3Thread")
            mChildHandlerThread?.start()

            mChiHandler = Handler(mChildHandlerThread?.looper)
        }
    }

    /**
     * @param listener
     */
    override fun setAudioListener(listener: RecorderListener) {
        this.listener = listener
    }

    /**
     * 开始录制
     * @param path
     */
    override fun startRecord(path: String) {

        try {
            mFileOutputStream = FileOutputStream(path)
        } catch (e: Exception) {
            e.printStackTrace()
        }
        if (mIsRecording) return

        initAudioRecord()
        initMp3Lame()

        startMillisecond = System.currentTimeMillis()
        mAudioRecord?.startRecording()

        mIsRecording = true
        if (ftRecord == null ) { // || ftRecord.isDone
            ftRecord = esRecord.submit(recordAudioRunnable)
        }

    }

    /**
     * 停止录制
     * @return 录制时长 ms
     */
    override fun stopRecord(): Long {

        if (startMillisecond == 0L) {
            return -1
        }
        val duration = System.currentTimeMillis() - startMillisecond
        startMillisecond = 0

        try {
            mIsRecording = false
            mAudioRecord?.stop()
            mAudioRecord?.release()
            mAudioRecord = null

            ftRecord?.cancel(true)
            ftRecord = null
        } catch (e: Exception) {
            e.printStackTrace()
        }

        return duration
    }

    /**
     * 暂停 录制
     */
    override fun pauseRecord() {
        try {
            mAudioRecord?.stop()
        } catch (e: Exception) {
            e.printStackTrace()
        }

    }

    /**
     * 继续录制
     */
    override fun resumeRecord() {
        startMillisecond = System.currentTimeMillis()
        mAudioRecord?.startRecording()
    }

    /**
     * 线程 记录音频
     */
    private var recordAudioRunnable = Runnable {

        while (mIsRecording) {
            mPCMBuffer?.let { pcmBuffer ->

                var readSize = mAudioRecord?.read(pcmBuffer, 0, mRecordBufferSize) ?: 0
                if (readSize > 0) {
                    processData(pcmBuffer, readSize)
                    calculateRealVolume(pcmBuffer, readSize)
                }
            }
        }
        // release and finalize audioRecord
        mAudioRecord?.release()
        mAudioRecord = null
        flushAndRelease()
    }

    /**
     * 初始化 录制
     */
    private fun initAudioRecord(){
        // 读到的字节数
        mRecordBufferSize = AudioRecord.getMinBufferSize(DEFAULT_SAMPLING_RATE, DEFAULT_CHANNEL_CONFIG, DEFAULT_AUDIO_FORMAT)
        // 2 字节
        var bytesPerSample = getBytesPerSample(DEFAULT_AUDIO_FORMAT)

        /* Get number of samples. Calculate the buffer size
         * (round up to the factor of given frame size)
         * 使能被整除，方便下面的周期性通知, buffer 里的采样数
         *
         * 在我们自己使用lame对pcm数据进行编码时，需要周期性的通知，所以需要将bufferSize像上取整到满足周期的大小
         * */
        var sampleCountInBuffer = mRecordBufferSize / bytesPerSample
        if (sampleCountInBuffer % FRAME_COUNT != 0) {
            sampleCountInBuffer += (FRAME_COUNT - sampleCountInBuffer % FRAME_COUNT)
            mRecordBufferSize = sampleCountInBuffer * bytesPerSample
        }

        mAudioRecord = AudioRecord(DEFAULT_AUDIO_SOURCE,
            DEFAULT_SAMPLING_RATE,
            DEFAULT_CHANNEL_CONFIG,
            DEFAULT_AUDIO_FORMAT,
            mRecordBufferSize)

        mPCMBuffer = ShortArray(mRecordBufferSize)
        // 给AudioRecord设置刷新监听，待录音帧数每次达到FRAME_COUNT，就通知转换线程转换一次数据
        mAudioRecord?.setRecordPositionUpdateListener(object : AudioRecord.OnRecordPositionUpdateListener {

            override fun onMarkerReached(recorder: AudioRecord?) {

            }


            override fun onPeriodicNotification(recorder: AudioRecord?) {

            }

        }, mChiHandler)
        // 设置 通知 位置周期
        mAudioRecord?.positionNotificationPeriod = FRAME_COUNT

        // mp3 buffer
        var mp3BufferSize = (7200 + mRecordBufferSize.toDouble() * 2.0 * 1.25).toInt()
        mMp3Buffer = ByteArray(mp3BufferSize)

    }

    /**
     * 初始化 mp3 lame
     */
    private fun initMp3Lame(){

        /*
         * Initialize lame buffer
         * mp3 sampling rate is the same as the recorded pcm sampling rate
         * The bit rate is 32kbps
         *
         */
        LameUtil.init(DEFAULT_SAMPLING_RATE,
            DEFAULT_LAME_IN_CHANNEL,
            DEFAULT_SAMPLING_RATE,
            DEFAULT_LAME_MP3_BIT_RATE,
            DEFAULT_LAME_MP3_QUALITY)

    }

    /**
     * lame 编码 pcm数据
     * @param pcmData
     * @param readSize
     *
     * @return 编码后的大小
     */
    private fun processData(pcmData: ShortArray, readSize: Int): Int{
        var encodeSize = LameUtil.encode(pcmData, pcmData, readSize, mMp3Buffer!!)
        if (encodeSize > 0) {
            try {
                mFileOutputStream?.write(mMp3Buffer,0, encodeSize)
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }

        return readSize
    }

    /**
     * 此计算方法来自samsung开发范例
     *
     * @param buffer   buffer
     * @param readSize readSize
     */
    private fun calculateRealVolume(buffer: ShortArray, readSize: Int) {
        var sum = 0.0
        for (i in 0 until readSize) {
            // 这里没有做运算的优化，为了更加清晰的展示代码
            sum += (buffer[i] * buffer[i]).toDouble()
        }
        if (readSize > 0) {
            val amplitude = sum / readSize
            val volume = sqrt(amplitude).toInt()
            listener?.onGetVolume(volume)
        }
    }

    /**
     * Flush all data left in lame buffer to file
     */
    private fun flushAndRelease(){
        var flushResult = LameUtil.flush(mMp3Buffer!!)
        if (flushResult > 0) {
            try {
                mFileOutputStream?.write(mMp3Buffer, 0, flushResult)
            } catch (e: IOException) {
                e.printStackTrace()
            } finally {
                if (mFileOutputStream != null) {
                    try {
                        mFileOutputStream?.close()
                    } catch (e: IOException) {
                        e.printStackTrace()
                    }
                }
                LameUtil.close()
            }
        }
    }



    private fun getBytesPerSample(audioFormat: Int): Int {
        return when (audioFormat) {
            ENCODING_PCM_8BIT -> 1
            ENCODING_PCM_16BIT, ENCODING_IEC61937, ENCODING_DEFAULT -> 2
            ENCODING_PCM_FLOAT -> 4
            ENCODING_INVALID -> throw IllegalArgumentException("Bad audio format $audioFormat")
            else -> throw IllegalArgumentException("Bad audio format $audioFormat")
        }
    }
}