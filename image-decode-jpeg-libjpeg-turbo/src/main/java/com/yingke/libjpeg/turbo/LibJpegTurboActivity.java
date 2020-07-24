package com.yingke.libjpeg.turbo;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import androidx.annotation.Nullable;

import com.yingke.core.base.BaseActivity;
import com.yingke.ffmpeg.assets.AssetsTask;


public class LibJpegTurboActivity extends BaseActivity implements AssetsTask.Callback{

    private ViewGroup mRootLayer;
    private Button mBtnJpegTurbo;
    private TextView mTvJpegPath;
    private SurfaceView mSurfaceView;

    public static void start(Context context){
        Intent intent = new Intent(context, LibJpegTurboActivity.class);
        context.startActivity(intent);
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_libjpegturbo);
        setupView();
    }

    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        AssetsTask task = new AssetsTask().context(this).files("input.jpeg").callBack(this);
        task.execute();
    }

    private void setupView() {
        mRootLayer = (ViewGroup) findViewById(R.id.native_yub2rgb_root_layer);
        mBtnJpegTurbo = (Button) findViewById(R.id.btn_libjpeg);
        mTvJpegPath = (TextView) findViewById(R.id.tv_jpeg_path);

        mBtnJpegTurbo.setEnabled(false);

        mSurfaceView = new SurfaceView(this);
        mRootLayer.addView(mSurfaceView);
        mSurfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
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
    }

    public void decompile(View view) {
        String jpegPath = mTvJpegPath.getText().toString().trim();
        LibJpegTurboUtil.loadJPEGImage(jpegPath, mSurfaceView.getHolder().getSurface());
    }

    @Override
    public void onSuccess(String[] filePaths) {
        mBtnJpegTurbo.setEnabled(true);
        mTvJpegPath.setText(filePaths[0]);
    }

}
