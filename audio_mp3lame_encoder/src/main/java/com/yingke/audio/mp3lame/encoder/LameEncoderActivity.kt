package com.yingke.audio.mp3lame.encoder

import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.util.Log
import android.widget.Button
import android.widget.Toast
import com.yingke.audio.mp3lame.encoder.recorder.Mp3Recorder
import com.yingke.audio.mp3lame.encoder.view.RecorderView
import com.yingke.audio.mp3lame.encoder.view.SpectrumView
import com.yingke.core.util.FileUtil
import java.io.File
import java.io.IOException
import java.lang.Exception

/**
 * 功能：
 * </p>
 * <p>Copyright xxx.xxx.com 2020 All right reserved </p>
 *
 * @author tuke 时间 2020-06-21
 * @email
 * <p>
 * 最后修改人：无
 * <p>
 */
class LameEncoderActivity: BasePermissionActivity() {

    companion object{
        const val TAG = "LameEncoderActivity"

        fun start(context: Context) {
            var intent = Intent(context, LameEncoderActivity::class.java)
            context.startActivity(intent)
        }
    }

    private var filePath: String =  ""
    private var recorder: Recorder? = null

    private var startButton: Button? = null
    private var stopButton: Button? = null

    private var pauseButton: Button?= null
    private var resumeButton: Button? = null

    private var spectrumView: SpectrumView? = null
    private var micView: RecorderView? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_mp3_lame)
        initData()
        initView()
        initListener()
    }

    private fun initData(){
        filePath = FileUtil.getsExternalFilesPath() + "/test.mp3"
        recorder = Mp3Recorder()
        createFile()
    }

    private fun initView(){
        startButton = findViewById<Button>(R.id.start_button)
        stopButton = findViewById<Button>(R.id.stop_button)
        pauseButton = findViewById<Button>(R.id.btn_pause)
        resumeButton = findViewById<Button>(R.id.btn_resume)
        spectrumView = findViewById(R.id.spectrum_view)
        micView = findViewById(R.id.mic_view)
    }

    private fun initListener(){
        recorder?.setAudioListener(object : RecorderListener {
            override fun onGetVolume(volume: Int) {
                Log.d(TAG, "onGetVolume: -->$volume")
            }
        })

        startButton?.setOnClickListener {

            requestAudioRecordPermission(object : IPermissionResult{
                /**
                 * 授权
                 */
                override fun granted() {
                    recorder?.startRecord(filePath)
                    spectrumView?.start()
                }

                /**
                 * 为授权
                 */
                override fun unGranted() {
                    Toast.makeText(this@LameEncoderActivity, "未取得权限", Toast.LENGTH_SHORT).show()
                }

            })
        }

        stopButton?.setOnClickListener {
            try {
                recorder?.stopRecord()
            } catch (e: Exception){}

            spectrumView?.stop()
        }

        pauseButton?.setOnClickListener {
            recorder?.pauseRecord()
        }

        resumeButton?.setOnClickListener {
            requestAudioRecordPermission(object : IPermissionResult{
                /**
                 * 授权
                 */
                override fun granted() {
                    recorder?.resumeRecord()
                }
                /**
                 * 为授权
                 */
                override fun unGranted() {
                    Toast.makeText(this@LameEncoderActivity, "未取得权限", Toast.LENGTH_SHORT).show()
                }
            })
        }

        micView?.setRecorderViewListener(object : RecorderView.RecorderViewListener {
            override fun onStart() {
                requestAudioRecordPermission(object : IPermissionResult{
                    /**
                     * 授权
                     */
                    override fun granted() {
                        recorder?.startRecord(filePath)
                    }
                    /**
                     * 为授权
                     */
                    override fun unGranted() {
                        Toast.makeText(this@LameEncoderActivity, "未取得权限", Toast.LENGTH_SHORT).show()
                    }
                })
            }

            override fun onStop() {
                try {
                    recorder?.stopRecord()
                    val strFinish = String.format("录制完成，保存在：%s", filePath)
                    Toast.makeText(this@LameEncoderActivity, strFinish, Toast.LENGTH_SHORT).show()
                } catch (e: Exception){}

            }
        })
    }

    /**
     * 创建 mp3文件
     */
    private fun createFile() {
        val file = File(filePath)
        if (!file.exists()) {
            try {
                file.createNewFile()
            } catch (e: IOException) {
                e.printStackTrace()
            }
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        try {
            recorder?.stopRecord()
        } catch (e: Exception){}

    }
}