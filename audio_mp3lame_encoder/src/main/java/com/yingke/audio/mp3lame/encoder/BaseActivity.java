package com.yingke.audio.mp3lame.encoder;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import java.util.List;

import pub.devrel.easypermissions.EasyPermissions;

/**
 * 功能：
 * </p>
 * <p>Copyright xxx.xxx.com 2020 All right reserved </p>
 *
 * @author tuke 时间 2020-06-21
 * @email <p>
 * 最后修改人：无
 * <p>
 */
public abstract class BaseActivity extends AppCompatActivity implements EasyPermissions.PermissionCallbacks {

    private static final String TAG = "BaseActivity";

    protected static final int CODE_REQUEST_AUDIO_RECORD = 0x101;
    protected static final int CODE_REQUEST_STORAGE = 0x103;

    private IPermissionResult permissionResult;

    /**
     * 音频权限
     * @param result
     */
    protected void requestAudioRecordPermission(IPermissionResult result){
        permissionResult = result;
        if (EasyPermissions.hasPermissions(this,
                Manifest.permission.RECORD_AUDIO,
                Manifest.permission.WRITE_EXTERNAL_STORAGE)) {

            if (result != null) {
                result.granted();
            }
        } else {
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.RECORD_AUDIO, Manifest.permission.WRITE_EXTERNAL_STORAGE},
                    CODE_REQUEST_AUDIO_RECORD);
        }
    }

    /**
     * 获取读写权限
     */
    public void requestSDCardPermission(IPermissionResult cb) {
        this.permissionResult = cb;

        if (EasyPermissions.hasPermissions(this, Manifest.permission.READ_EXTERNAL_STORAGE)
                && EasyPermissions.hasPermissions(this, Manifest.permission.WRITE_EXTERNAL_STORAGE)) {

            if (permissionResult != null) {
                permissionResult.granted();
            }
        } else {
            String[] perms = {Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE};
            ActivityCompat.requestPermissions(this, perms, CODE_REQUEST_STORAGE);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        EasyPermissions.onRequestPermissionsResult(requestCode, permissions, grantResults, this);
    }

    @Override
    public void onPermissionsGranted(int requestCode, List<String> perms) {
        switch (requestCode) {
            case CODE_REQUEST_AUDIO_RECORD:
                if (permissionResult != null) {
                    permissionResult.granted();
                }
                break;
                default:
                    if (permissionResult != null) {
                        permissionResult.granted();
                    }
        }
    }

    @Override
    public void onPermissionsDenied(int requestCode, List<String> perms) {
        if (permissionResult != null) {
            permissionResult.unGranted();
        }
    }


    public interface IPermissionResult {
        /**
         * 授权
         */
        void granted();

        /**
         * 为授权
         */
        void unGranted();
    }

}
