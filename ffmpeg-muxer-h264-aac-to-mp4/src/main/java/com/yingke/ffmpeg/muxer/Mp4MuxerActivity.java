package com.yingke.ffmpeg.muxer;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.Nullable;

import com.yingke.core.base.BaseActivity;
import com.yingke.core.util.FileUtil;
import com.yingke.ffmpeg.assets.AssetsTask;

import java.util.concurrent.Callable;

import bolts.Continuation;
import bolts.Task;

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
public class Mp4MuxerActivity extends BaseActivity implements AssetsTask.Callback {

    private Button button;
    private TextView inputH264Tv;
    private TextView inputAacTv;

    private TextView outputTv;
    private String mp4Path;

    public static void start(Context context){
        Intent intent = new Intent(context, Mp4MuxerActivity.class);
        context.startActivity(intent);
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main_mp4_muxer);
        button = findViewById(R.id.button);

//        final String h264Path = FileUtil.getsExternalFilesPath() + "/" + "h264_for_mp4.h264";
//        final String aacPath = FileUtil.getsExternalFilesPath() + "/" + "aac_for_mp4.aac";

        inputH264Tv = findViewById(R.id.h264_path);
//        inputH264Tv.setText(h264Path);
        inputAacTv = findViewById(R.id.aac_path);
//        inputAacTv.setText(aacPath);

        // 输出文件
        final String output = FileUtil.getsExternalFilesPath() + "/" + "aac_h264_to_mp4.mp4";
        FileUtil.createFile(output);

        outputTv = findViewById(R.id.output_path);

        button.setEnabled(false);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                button.setEnabled(false);

                final String h264Path = inputH264Tv.getText().toString();
                final String aacPath = inputAacTv.getText().toString();


                Task.callInBackground(new Callable<Boolean>() {
                    @Override
                    public Boolean call() throws Exception {
                        return Mp4MuxerUtil.startMuxerMp4(h264Path, aacPath, output) == 0;
                    }
                }).onSuccess(new Continuation<Boolean, Object>() {
                    @Override
                    public Object then(Task<Boolean> task) throws Exception {
                        boolean success = task.getResult();
                        button.setEnabled(true);

                        outputTv.setText("Mp4路径：" + output);
                        Toast.makeText(Mp4MuxerActivity.this, success ? "合成成功" : "合成失败", Toast.LENGTH_SHORT).show();
                        return null;
                    }
                }, Task.UI_THREAD_EXECUTOR);
            }
        });
    }

    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        AssetsTask task = new AssetsTask().context(this).files("aac_for_mp4.aac", "h264_for_mp4.h264").callBack(this);
        task.execute();
    }

    @Override
    public void onSuccess(String[] filePath) {
        button.setEnabled(true);
        inputAacTv.setText(filePath[0]);
        inputH264Tv.setText(filePath[1]);
    }
}
