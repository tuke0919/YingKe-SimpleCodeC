package com.yingke.yuvtojpegpng;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.Nullable;

import com.yingke.core.base.BaseActivity;
import com.yingke.core.util.FileUtil;
import com.yingke.ffmpeg.assets.AssetsTask;


public class YuvJpegPngActivity extends BaseActivity implements AssetsTask.Callback{

    private TextView mTvYuvPath;

    private Button mBtnJpeg;
    private TextView mTvJpegPath;

    private Button mBtnPng;
    private TextView mTvPngPath;

    public static void start(Context context){
        Intent intent = new Intent(context, YuvJpegPngActivity.class);
        context.startActivity(intent);
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_yuv_jpeg_png);
        setupView();
    }

    @Override
    protected void onPostCreate(@Nullable Bundle savedInstanceState) {
        super.onPostCreate(savedInstanceState);
        AssetsTask task = new AssetsTask().context(this).files("yuv_jpeg_png.yuv").callBack(this);
        task.execute();
    }

    private void setupView() {
        mTvYuvPath = (TextView) findViewById(R.id.tv_yuv_path);

        mBtnJpeg = (Button) findViewById(R.id.btn_yuv_jpeg);
        mTvJpegPath = (TextView) findViewById(R.id.tv_jpeg_path);
        mBtnPng = (Button) findViewById(R.id.btn_yuv_png);
        mTvPngPath = (TextView) findViewById(R.id.tv_png_path);

        mBtnJpeg.setEnabled(false);
        mBtnPng.setEnabled(false);

    }


    @Override
    public void onSuccess(String[] filePaths) {
        mBtnJpeg.setEnabled(true);
        mBtnPng.setEnabled(true);
        mTvYuvPath.setText(filePaths[0]);
    }

    /**
     *  转换成 jpeg
     * @param view
     */
    public void onTransferJPEG(View view) {
        String yuvPath = mTvYuvPath.getText().toString().trim();
        String jpegPath = FileUtil.getsExternalFilesPath() + "/yuv_jpeg_png.jpeg";
        YuvToJpegPngUtil.yuv2jpeg(yuvPath, jpegPath, 510, 510);

        mTvJpegPath.setText(jpegPath);
        Toast.makeText(this, "Jpeg编码成功", Toast.LENGTH_SHORT).show();

    }

    /**
     * 转换成 png
     * @param view
     */
    public void onTransferPNG(View view) {
        String yuvPath = mTvYuvPath.getText().toString().trim();
        String pngPath = FileUtil.getsExternalFilesPath() + "/yuv_jpeg_png.png";
        YuvToJpegPngUtil.yuv2png(yuvPath, pngPath, 510, 510);

        mTvPngPath.setText(pngPath);
        Toast.makeText(this, "Png编码成功", Toast.LENGTH_SHORT).show();
    }

}
