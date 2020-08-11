package com.yingke.encode.camera;


import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.Toast;

import com.yingke.core.base.BasePermissionActivity;
import com.yingke.core.util.FileUtil;
import com.yingke.encode.camera.camera.CameraV1;

public class EncodeMp4JpegActivity extends BasePermissionActivity {

    private ViewGroup mRootLayer;
    private Button mBtnEncodeMP4;
    private Button mBtnEncodeJpeg;

    private CameraV1 mCameraV1;
    private SurfaceView mSurfaceView;

    private boolean mIsEncodingMp4 = false;
    private String mEncodedMp4;
    private String mEncodedJpeg;

    public static void start(Context context){
        Intent intent = new Intent(context, EncodeMp4JpegActivity.class);
        context.startActivity(intent);
    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_encode_mp4_jpeg);

        requestCameraPermission(new IPermissionResult() {
            @Override
            public void granted() {
                setupView();
            }

            @Override
            public void unGranted() {

            }
        });
    }

    private void setupView() {
        mRootLayer = (ViewGroup) findViewById(R.id.surface_view_layout);

        mBtnEncodeMP4 = (Button) findViewById(R.id.btn_encode_mp4_start);
        mBtnEncodeJpeg = (Button) findViewById(R.id.btn_encode_jpeg);

        mSurfaceView = new SurfaceView(this);
        mRootLayer.addView(mSurfaceView);
        mCameraV1 = new CameraV1();
        mCameraV1.setPreviewView(mSurfaceView);
    }

    /**
     * 编码的 mp4
     * @param view
     */
    public void onEncodeMp4(View view) {
        if (!mIsEncodingMp4) {
            mIsEncodingMp4 = true;
            mBtnEncodeMP4.setEnabled(true);
            mBtnEncodeMP4.setText("停止编码MP4");
            mEncodedMp4 = "";

            mEncodedMp4 = FileUtil.getsExternalFiles() + "/" + "camera_" +System.currentTimeMillis() + ".mp4";
            mCameraV1.encodeStart(mEncodedMp4);

            Toast.makeText(this, "开始编码Mp4", Toast.LENGTH_SHORT).show();

        } else {
            mIsEncodingMp4 = false;
            mBtnEncodeMP4.setEnabled(true);
            mBtnEncodeMP4.setText("Camera编码MP4");

            mCameraV1.encodeStop();
            Toast.makeText(this, "编码成功：" + mEncodedMp4 , Toast.LENGTH_SHORT).show();
        }
    }

    /**
     * 编码 Jpeg
     * @param view
     */
    public void onEncodeJPEG(View view) {
        if (mCameraV1 != null) {
            mEncodedJpeg = FileUtil.getsExternalFiles() + "/" + "camera_" +System.currentTimeMillis() + ".jpeg";
            mCameraV1.encodeJPEG(mEncodedJpeg);
            Toast.makeText(this, "Jpeg路径：" + mEncodedJpeg, Toast.LENGTH_SHORT).show();
        }
    }

}
