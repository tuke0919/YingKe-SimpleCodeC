package com.yingke.ffmpeg.demuxer;

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
public class Mp4DemuxerActivity extends BaseActivity implements AssetsTask.Callback {


    private Button button;
    private TextView inputTv;
    private TextView outputH264Tv;
    private TextView outputAacTv;
    private TextView statusTv;
    private String mp4Path;


    public static void start(Context context){
        Intent intent = new Intent(context, Mp4DemuxerActivity.class);
        context.startActivity(intent);
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main_mp4_demuxer);
        button = findViewById(R.id.button);
        inputTv = findViewById(R.id.input_path);
        statusTv = findViewById(R.id.status);
        outputH264Tv = findViewById(R.id.output_h264_path);
        outputAacTv = findViewById(R.id.output_aac_path);

        button.setEnabled(false);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (TextUtils.isEmpty(mp4Path)) {
                    Toast.makeText(Mp4DemuxerActivity.this, "mp4路径空", Toast.LENGTH_SHORT).show();
                    return;
                }

                button.setEnabled(false);

                final String h264Path = FileUtil.getsExternalFilesPath() + "/h264_for_mp4.264";
                final String aacPath = FileUtil.getsExternalFilesPath() + "/aac_for_mp4.aac";

                FileUtil.createFile(h264Path);
                FileUtil.createFile(aacPath);

                Mp4DemuxerUtil.startDemuxerMp4(mp4Path, h264Path, aacPath, new Mp4DemuxerUtil.Callback() {
                    @Override
                    public void code(final int code) {
                        Mp4DemuxerActivity.this.runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                switch (code){
                                    case -1:
                                        statusTv.setText("初始化输入流失败");
                                        break;
                                    case -2:
                                        statusTv.setText("初始化输出文件失败");
                                        break;
                                    case 0:
                                        statusTv.setText("初始化成功");
                                        break;
                                    case 1:
                                        statusTv.setText("开始解复用");
                                        break;
                                    case 2:
                                        statusTv.setText("解复用成功");
                                        outputH264Tv.setText("H264路径：" + h264Path);
                                        outputAacTv.setText("AAC路径：" + aacPath);
                                        Toast.makeText(Mp4DemuxerActivity.this, "解复用成功", Toast.LENGTH_SHORT).show();
                                        break;
                                }
                            }
                        });
                    }
                });
            }
        });
    }

    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        AssetsTask task = new AssetsTask().context(this).files("mp4_for_h264_aac.mp4").callBack(this);
        task.execute();
    }

    @Override
    public void onSuccess(String[] filePath) {
        button.setEnabled(true);
        mp4Path = filePath[0];
        inputTv.setText("MP4路径：" + mp4Path);
    }
}
