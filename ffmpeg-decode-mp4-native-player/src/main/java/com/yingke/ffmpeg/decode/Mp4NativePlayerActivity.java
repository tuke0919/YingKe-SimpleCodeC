package com.yingke.ffmpeg.decode;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
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
public class Mp4NativePlayerActivity extends BaseActivity implements AssetsTask.Callback {

    private Button button;
    private TextView outputTv;
    private String mp4Path;

    private SurfaceView surfaceView;

    public static void start(Context context){
        Intent intent = new Intent(context, Mp4NativePlayerActivity.class);
        context.startActivity(intent);
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main_mp4_player);
        button = findViewById(R.id.button);
        outputTv = findViewById(R.id.output_path);
        surfaceView = (SurfaceView) findViewById(R.id.surface_view);
        surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {

            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {

            }
        });

        button.setEnabled(false);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                final String mp4Path = outputTv.getText().toString();
                if (TextUtils.isEmpty(mp4Path)){
                    Toast.makeText(Mp4NativePlayerActivity.this, "mp4路径空", Toast.LENGTH_SHORT).show();
                    return;
                }
                Task.callInBackground(new Callable<Boolean>() {
                    @Override
                    public Boolean call() throws Exception {
                        Mp4NativePlayer.startMp4Player(mp4Path, surfaceView.getHolder().getSurface());
                        return true;
                    }
                });
            }
        });
    }

    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        AssetsTask task = new AssetsTask().context(this).files("mp4_for_native_player.mp4").callBack(this);
        task.execute();
    }

    @Override
    public void onSuccess(String[] filePath) {
        button.setEnabled(true);
        outputTv.setText(filePath[0]);
    }
}
