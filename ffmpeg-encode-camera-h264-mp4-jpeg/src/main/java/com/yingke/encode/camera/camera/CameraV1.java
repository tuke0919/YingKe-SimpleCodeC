package com.yingke.encode.camera.camera;

import android.app.Activity;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.yingke.encode.camera.CameraEncoder;

import java.io.IOException;
import java.util.concurrent.Callable;

import bolts.Task;

/**
 * 功能：
 * </p>
 * <p>Copyright xxx.xxx.com 2020 All right reserved </p>
 *
 * @author tuke 时间 2020-08-10
 * @email <p>
 * 最后修改人：无
 * <p>
 */
public class CameraV1 implements IEncoder, SurfaceHolder.Callback, Camera.PreviewCallback {

    private static final String TAG = "Mp4Encoder";

    /**
     * 相机实例
     */
    private Camera mCamera;

    /**
     * 默认使用前置摄像头
     */
    private int mCameraId;

    /**
     * 相机预览数据输出
     *
     * @param data
     * @param camera
     */
    Camera.Size mPreviewSize;

    /**
     * surface view
     */
    private SurfaceView mSurfaceView;


    /**
     * 设置 预览
     *
     * @param surfaceView
     */
    @Override
    public void setPreviewView(SurfaceView surfaceView) {
        this.mSurfaceView = surfaceView;
//        this.mCameraId = Camera.CameraInfo.CAMERA_FACING_FRONT;
        this.mCameraId = Camera.CameraInfo.CAMERA_FACING_BACK;
        surfaceView.getHolder().addCallback(this);
    }

    /**
     * @param holder The SurfaceHolder whose surface is being created.
     */
    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.d(TAG, "surfaceCreated");
        mCamera = openCamera();
    }

    /**
     * @param holder The SurfaceHolder whose surface has changed.
     * @param format The new PixelFormat of the surface.
     * @param width  The new width of the surface.
     * @param height The new height of the surface.
     */
    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        Log.d(TAG, "surfaceChanged");
        try {
            // 设置角度
            setCameraDisplayOrientation(mCameraId, mCamera);
            Camera.Parameters parameters = mCamera.getParameters();
            parameters.setPreviewFormat(ImageFormat.NV21);
            mCamera.setParameters(parameters);
            // 预览显示
            mCamera.setPreviewDisplay(holder);
            // 预览回调
            mCamera.setPreviewCallback(this);
            // 开始预览
            mCamera.startPreview();
        } catch (IOException e) {
            e.printStackTrace();
        }

        Task.callInBackground(new Callable<Object>() {
            @Override
            public Object call() throws Exception {
                byte[] buffer = new byte[1024];
                CameraEncoder.onPreviewFrame(buffer, 240, 480);
                return null;
            }
        });
    }

    /**
     * @param holder The SurfaceHolder whose surface is being destroyed.
     */
    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.d(TAG, "surfaceDestroyed");

        if (mCamera != null) {
            mCamera.setPreviewCallback(null);
            mCamera.stopPreview();
            mCamera.release();
            mCamera = null;
        }

    }

    /**
     * Called as preview frames are displayed.  This callback is invoked
     * on the event thread {@link #open(int)} was called from.
     *
     * <p>If using the {@link ImageFormat#YV12} format,
     * refer to the equations in {@link Camera.Parameters#setPreviewFormat}
     * for the arrangement of the pixel data in the preview callback
     * buffers.
     *
     * @param data   the contents of the preview frame in the format defined
     *               by {@link ImageFormat}, which can be queried
     *               with {@link Camera.Parameters#getPreviewFormat()}.
     *               If {@link Camera.Parameters#setPreviewFormat(int)}
     *               is never called, the default will be the YCbCr_420_SP
     *               (NV21) format.
     * @param camera the Camera service object.
     */
    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        Log.d(TAG, "onPreviewFrame");

        this.mPreviewSize = camera.getParameters().getPreviewSize();

        Log.d(TAG, "mPreviewSize: width = " + this.mPreviewSize.width + " height = " + this.mPreviewSize.height);

        // 相机 原始图像数据回调 nv21格式
        CameraEncoder.onPreviewFrame(data, mPreviewSize.width, mPreviewSize.height);
    }


    /**
     * 编码h264开始
     *
     * @param outputPath
     */
    @Override
    public void encodeH264Start(String outputPath) {
        Log.d(TAG, "encodeH264Start: outputPath = " + outputPath);
        if (mPreviewSize != null) {
            CameraEncoder.encodeH264Start(outputPath, mPreviewSize.width, mPreviewSize.height);
        }
    }

    /**
     * 编码mp4结束
     */
    @Override
    public void encodeH264Stop() {
        Log.d(TAG, "encodeH264Stop");
        CameraEncoder.encodeH264Stop();
    }

    /**
     * 编码mp4开始
     *
     * @param outputPath
     */
    @Override
    public void encodeMp4Start(String outputPath) {
        Log.d(TAG, "encodeMp4Start: outputPath = " + outputPath);
        if (mPreviewSize != null) {
            CameraEncoder.encodeMp4Start(outputPath, mPreviewSize.width, mPreviewSize.height);
        }
    }

    /**
     * 编码mp4结束
     */
    @Override
    public void encodeMp4Stop() {
        Log.d(TAG, "encodeMp4Stop");
        CameraEncoder.encodeMp4Stop();
    }

    /**
     * 编码jpeg
     *
     * @param jpegPath
     */
    @Override
    public void encodeJPEG(String jpegPath) {
        Log.d(TAG, "encodeJPEG: jpegPath = " + jpegPath);
        if (mPreviewSize != null) {
            CameraEncoder.encodeJPEG(jpegPath, mPreviewSize.width, mPreviewSize.height);
        }
    }

    /**
     * 销毁
     */
    @Override
    public void onDestroy() {
        Log.d(TAG, "onDestroy");
        if (mCamera != null) {
            mCamera.release();
            mCamera = null;
        }
    }

    /**
     * 打开相机
     * @return
     */
    private Camera openCamera() {
        Log.d(TAG, "openCamera");
        Camera camera;
        try {
            camera = Camera.open(mCameraId);
        } catch (Exception e) {
            camera = null;
        }
        return camera;
    }

    /**
     * 前置摄像头要转换270度
     * 后置摄像头转换90度
     *
     * @param cameraId
     * @param camera
     */
    private void setCameraDisplayOrientation(int cameraId, Camera camera) {
        Log.d(TAG, "setCameraDisplayOrientation");

        Activity targetActivity = (Activity) mSurfaceView.getContext();
        Camera.CameraInfo info = new Camera.CameraInfo();
        Camera.getCameraInfo(cameraId, info);
        int rotation = targetActivity.getWindowManager().getDefaultDisplay().getRotation();
        int degrees = 0;
        switch (rotation) {
            case Surface.ROTATION_0:
                degrees = 0;
                break;
            case Surface.ROTATION_90:
                degrees = 90;
                break;
            case Surface.ROTATION_180:
                degrees = 180;
                break;
            case Surface.ROTATION_270:
                degrees = 270;
                break;
        }
        int displayDegree;
        if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
            displayDegree = (info.orientation + degrees) % 360;
            displayDegree = (360 - displayDegree) % 360;
        } else {
            displayDegree = (info.orientation - degrees + 360) % 360;
        }
        camera.setDisplayOrientation(displayDegree);
    }





}
