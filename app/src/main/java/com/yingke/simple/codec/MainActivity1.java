package com.yingke.simple.codec;

import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import androidx.annotation.Nullable;

import com.yingke.audio.mp3lame.encoder.LameEncoderActivity;
import com.yingke.core.base.BaseActivity;
import com.yingke.decode.mp4.Mp4DecoderActivity;
import com.yingke.ffmpeg.decode.Mp4NativePlayerActivity;
import com.yingke.ffmpeg.demuxer.Mp4DemuxerActivity;
import com.yingke.ffmpeg.muxer.Mp4MuxerActivity;

/**
 * 功能：
 * </p>
 * <p>Copyright xxx.xxx.com 2020 All right reserved </p>
 *
 * @author tuke 时间 2020-07-05
 * @email <p>
 * 最后修改人：无
 * <p>
 */
public class MainActivity1 extends BaseActivity {

    private TextView textView00;
    private TextView textView01;
    private TextView textView02;
    private TextView textView03;
    private TextView textView04;
    private TextView textView05;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        initView();
        initListener();
    }

    private void initView(){
        textView00 = findViewById(R.id.textView00);
        textView01 = findViewById(R.id.textView01);
        textView02 = findViewById(R.id.textView02);
        textView03 = findViewById(R.id.textView03);
        textView04 = findViewById(R.id.textView04);
        textView05 = findViewById(R.id.textView05);
    }

    private void initListener(){
        textView00.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                LameEncoderActivity.Companion.start(MainActivity1.this);
            }
        });

        textView01.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                LameEncoderActivity.Companion.start(MainActivity1.this);
            }
        });

        textView02.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Mp4DecoderActivity.start(MainActivity1.this);
            }
        });

        textView03.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Mp4DemuxerActivity.start(MainActivity1.this);
            }
        });

        textView04.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Mp4MuxerActivity.start(MainActivity1.this);
            }
        });

        textView05.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Mp4NativePlayerActivity.start(MainActivity1.this);
            }
        });
    }
}
