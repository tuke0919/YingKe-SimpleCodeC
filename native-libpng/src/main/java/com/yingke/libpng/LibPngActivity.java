package com.yingke.libpng;

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


public class LibPngActivity extends BaseActivity implements AssetsTask.Callback{

    private ViewGroup mRootLayer;
    private Button mBtnLibpng;
    private TextView mTvPngPath;
    private SurfaceView mSurfaceView;

    public static void start(Context context){
        Intent intent = new Intent(context, LibPngActivity.class);
        context.startActivity(intent);
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_libpng);
        setupView();
    }

    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        AssetsTask task = new AssetsTask().context(this).files("input.png").callBack(this);
        task.execute();
    }

    private void setupView() {
        mRootLayer = (ViewGroup) findViewById(R.id.native_yub2rgb_root_layer);
        mBtnLibpng = (Button) findViewById(R.id.btn_libpng);
        mTvPngPath = (TextView) findViewById(R.id.tv_png_path);

        mBtnLibpng.setEnabled(false);

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
        String jpegPath = mTvPngPath.getText().toString().trim();
        LibPngUtil.loadPNGImage(jpegPath, mSurfaceView.getHolder().getSurface());
    }

    @Override
    public void onSuccess(String[] filePaths) {
        mBtnLibpng.setEnabled(true);
        mTvPngPath.setText(filePaths[0]);
    }

}
