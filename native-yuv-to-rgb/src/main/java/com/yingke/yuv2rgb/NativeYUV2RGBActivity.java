package com.yingke.yuv2rgb;

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


/**
 * 1,
 * 2, ffmpeg -i input.jpg -s 510x510 -pix_fmt nv12 nv12.yuv
 * 3, ffmpeg -i input.jpg -s 510x510 -pix_fmt nv21 nv21.yuv
 *
 */
public class NativeYUV2RGBActivity extends BaseActivity implements AssetsTask.Callback{

    private ViewGroup mRootLayer;
    private Button mBtnYuv420p, mBtnNV12, mBtnNV21;
    private TextView mTvYuv420p, mTvNV12, mTvNV21;
    private SurfaceView mSurfaceView;

    public static void start(Context context){
        Intent intent = new Intent(context, NativeYUV2RGBActivity.class);
        context.startActivity(intent);
    }


    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_native_yuv2rgb);
        setupView();
    }

    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        AssetsTask task = new AssetsTask().context(this).files("yuv420p.yuv", "nv12.yuv", "nv21.yuv").callBack(this);
        task.execute();
    }

    private void setupView() {
        mRootLayer = (ViewGroup) findViewById(R.id.native_yub2rgb_root_layer);
        mBtnYuv420p = (Button) findViewById(R.id.btn_yuv420p);
        mBtnNV12 = (Button) findViewById(R.id.btn_nv12);
        mBtnNV21 = (Button) findViewById(R.id.btn_nv21);

        mTvYuv420p = (TextView) findViewById(R.id.tv_yuv420p);
        mTvNV12 = (TextView) findViewById(R.id.tv_nv12);
        mTvNV21 = (TextView) findViewById(R.id.tv_nv21);

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

    public void onYUV420PToRGB24(View view) {
        String path = mTvYuv420p.getText().toString();
        NativeYUV2RGB.yuv2rgb(path, NativeYUV2RGB.Type.YUV420P_TO_RGB24, 510, 510, mSurfaceView.getHolder().getSurface());
    }

    public void onNV12ToRGB24(View view) {
        String path = mTvNV12.getText().toString();
        NativeYUV2RGB.yuv2rgb(path, NativeYUV2RGB.Type.NV12_TO_RGB24, 510, 510, mSurfaceView.getHolder().getSurface());
    }

    public void onNV21ToRGB24(View view) {
        String path = mTvNV21.getText().toString();
        NativeYUV2RGB.yuv2rgb(path, NativeYUV2RGB.Type.NV21_TO_RGB24, 510, 510, mSurfaceView.getHolder().getSurface());
    }


    @Override
    public void onSuccess(String[] filePaths) {
        mBtnYuv420p.setEnabled(true);
        mBtnNV12.setEnabled(true);
        mBtnNV21.setEnabled(true);

        mTvYuv420p.setText(filePaths[0]);
        mTvNV12.setText(filePaths[1]);
        mTvNV21.setText(filePaths[2]);

    }
}
