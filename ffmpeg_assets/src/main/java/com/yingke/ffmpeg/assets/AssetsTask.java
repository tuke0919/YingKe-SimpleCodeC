package com.yingke.ffmpeg.assets;

import android.content.Context;
import android.os.AsyncTask;

import com.yingke.core.util.FileUtil;

import java.io.Closeable;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.lang.ref.WeakReference;

/**
 * 功能：
 * </p>
 * <p>Copyright xxx.xxx.com 2020 All right reserved </p>
 *
 * @author tuke 时间 2020-07-03
 * @email <p>
 * 最后修改人：无
 * <p>
 */
public class AssetsTask extends AsyncTask<Void, Void, String> {


    private WeakReference<Context> weakContext;
    private String[] fileNames;
    private Callback callback;

    public AssetsTask context(Context context) {
        weakContext = new WeakReference<>(context);
        return this;
    }

    public AssetsTask files(String... fileNameParams){
        fileNames = new String[fileNameParams.length];
        for (int i = 0; i <  fileNameParams.length; i++) {
            fileNames[i]  = fileNameParams[i];
        }
        return this;
    }

    public AssetsTask callBack(Callback callback){
        this.callback = callback;
        return this;
    }

    @Override
    protected String doInBackground(Void... voids) {
        InputStream inputStream = null;
        FileOutputStream fos = null;
        File targetFile = null;
        try {
            Context appContext = weakContext.get();
            if (appContext != null) {
                for (String fileName : fileNames) {
                    inputStream = appContext.getAssets().open(fileName);
                    File files = FileUtil.getsExternalFiles();
                    targetFile = new File(files, fileName);
                    FileUtil.createFile(targetFile);

                    fos = new FileOutputStream(targetFile);
                    int length;
                    byte[] buffer = new byte[8 * 1024];
                    while ((length = inputStream.read(buffer)) != -1) {
                        fos.write(buffer, 0, length);
                        fos.flush();
                    }
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            ioClose(fos);
            ioClose(inputStream);
        }
        return targetFile == null? "" : targetFile.getAbsolutePath();
    }

    @Override
    protected void onPostExecute(String s) {
        super.onPostExecute(s);
        if (callback != null) {
            callback.onSuccess(s);
        }
    }

    public interface Callback {
        void onSuccess(String filePath);
    }

    private void ioClose(Closeable closeable) {
        if (closeable != null) {
            try {
                closeable.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}
