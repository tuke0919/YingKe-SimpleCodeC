package com.yingke.simple.codec;

import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import androidx.annotation.Nullable;

import com.yingke.audio.mp3lame.encoder.LameEncoderActivity;
import com.yingke.core.base.BaseActivity;
import com.yingke.decode.mp4.Mp4DecoderActivity;

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
    }
}
