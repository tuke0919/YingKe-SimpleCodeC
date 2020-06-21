package com.yingke.simple.codec

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.widget.TextView
import com.yingke.audio.mp3lame.encoder.LameEncoderActivity

class MainActivity : AppCompatActivity() {

    private var textView00: TextView? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        initView()
        initListener()
    }

    private fun initView(){
        textView00 = findViewById(R.id.textView00)
    }

    private fun initListener(){
        textView00?.setOnClickListener {
            LameEncoderActivity.start(this@MainActivity)
        }
    }
}
