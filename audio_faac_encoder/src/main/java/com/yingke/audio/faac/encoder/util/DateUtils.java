package com.yingke.audio.faac.encoder.util;

import android.annotation.SuppressLint;

/**
 * @author ：chezi008 on 2018/4/17 22:24
 * @description ：
 * @email ：chezi008@163.com
 */
public class DateUtils {
    @SuppressLint({"DefaultLocale"})
    public static String toTime(int var0) {
        var0 /= 1000;
        int var1 = var0 / 60;
        boolean var2 = false;
        if (var1 >= 60) {
            int var4 = var1 / 60;
            var1 %= 60;
        }
        int var3 = var0 % 60;
        if (var1>0){
            return String.format("%d′%d\"", new Object[]{Integer.valueOf(var1), Integer.valueOf(var3)});
        }
        return String.format("%d\"", Integer.valueOf(var3));
    }
}
