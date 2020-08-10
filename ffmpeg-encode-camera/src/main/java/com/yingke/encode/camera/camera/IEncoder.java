package com.yingke.encode.camera.camera;

import android.view.SurfaceView;

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
public interface IEncoder {

    /**
     * 设置 预览
     * @param surfaceView
     */
    void setPreviewView(SurfaceView surfaceView);

    /**
     * 编码mp4开始
     * @param outputPath
     */
    void encodeStart(String outputPath);

    /**
     * 编码mp4结束
     */
    void encodeStop();

    /**
     * 编码jpeg
     * @param jpegPath
     */
    void encodeJPEG(String jpegPath);

    /**
     * 销毁
     */
    void onDestroy();
}
