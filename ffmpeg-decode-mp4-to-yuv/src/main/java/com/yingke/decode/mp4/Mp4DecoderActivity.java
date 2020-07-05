package com.yingke.decode.mp4;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.View;
import android.widget.Button;
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
public class Mp4DecoderActivity extends BaseActivity implements AssetsTask.Callback {

    private Mp4Decoder mp4Decoder;
    private Button button;
    private String mp4Path;

    public static void start(Context context){
        Intent intent = new Intent(context, Mp4DecoderActivity.class);
        context.startActivity(intent);
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main_mp4_decoder);
        button = findViewById(R.id.button);
        button.setEnabled(false);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (TextUtils.isEmpty(mp4Path)) {
                    Toast.makeText(Mp4DecoderActivity.this, "mp4路径空", Toast.LENGTH_SHORT).show();
                    return;
                }

                if (mp4Decoder == null) {
                    mp4Decoder = new Mp4Decoder();
                }
                Task.callInBackground(new Callable<Boolean>() {
                    @Override
                    public Boolean call() throws Exception {
                        String output = FileUtil.getsExternalFilesPath() + "/" + "output.yuv";
                        FileUtil.createFile(output);
                        return mp4Decoder.decode(mp4Path, output) == 0;
                    }
                }).onSuccess(new Continuation<Boolean, Object>() {
                    @Override
                    public Object then(Task<Boolean> task) throws Exception {
                        boolean success = task.getResult();
                        Toast.makeText(Mp4DecoderActivity.this, success ? "解码成功" : "解码失败", Toast.LENGTH_SHORT).show();
                        return null;
                    }
                }, Task.UI_THREAD_EXECUTOR);
            }
        });
    }

    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        AssetsTask task = new AssetsTask().context(this).files("input.mp4").callBack(this);
        task.execute();
    }

    @Override
    public void onSuccess(String filePath) {
        button.setEnabled(true);
        mp4Path = filePath;
    }
}
