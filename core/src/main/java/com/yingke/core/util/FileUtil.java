package com.yingke.core.util;

import android.text.TextUtils;

import com.yingke.core.core.CoreContext;

import java.io.File;
import java.io.IOException;

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
public class FileUtil {

    private static String sExternalFiles = "";
    private static String sExternalCache = "";

    private static String sInternalFiles = "";
    private static String sInternalCache = "";

    /**
     * /storage/emulated/0/Android/data/包名/files
     * @return
     */
    public static String getsExternalFilesPath(){
        if (!TextUtils.isEmpty(sExternalFiles)) {
            return sExternalFiles;
        }
        sExternalFiles = getsExternalFilesPath(null);
        return sExternalFiles;
    }

    /**
     * /storage/emulated/0/Android/data/包名/files/dir
     * @param dir
     * @return
     */
    public static String getsExternalFilesPath(String dir){
        File file = CoreContext.context().getExternalFilesDir(dir);

        if (file != null) {
            return file.getAbsolutePath();
        }
        return "";
    }

    /**
     * data/data/包名/files
     * @return
     */
    public static String getsInternalFilesPath(){
        if (!TextUtils.isEmpty(sInternalFiles)) {
            return sInternalFiles;
        }
        sInternalFiles = getsInternalFilesPath(null);
        return sInternalFiles;
    }

    /**
     * data/data/包名/files/dir
     * @param dir
     * @return
     */
    public static String getsInternalFilesPath(String dir){
        File file = CoreContext.context().getFilesDir();

        if (file != null) {
            return file.getAbsolutePath() + File.separator + dir;
        }

        return "";
    }

    public static File getsExternalFiles(){
        return CoreContext.context().getExternalFilesDir(null);
    }

    /**
     * 创建文件, 文件存在, 先删除
     *
     * @param path
     */
    public static void createFile(String path) {
        createFile(new File(path));
    }

    /**
     * 创建文件, 文件存在, 先删除
     *
     * @param file
     */
    public static void createFile(File file) {
        if (file == null) {
            return;
        }

        if (file.exists()) {
            file.delete();
        }

        File parent = file.getParentFile();
        if (!parent.exists()) {
            parent.mkdirs();
        }

        try {
            file.createNewFile();
        } catch (IOException e) {

        }
    }



}
