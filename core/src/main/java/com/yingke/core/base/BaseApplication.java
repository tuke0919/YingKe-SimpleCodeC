package com.yingke.core.base;

import android.app.Application;

import com.yingke.core.core.CoreContext;

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
public class BaseApplication extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        CoreContext.get().init(this);
    }
}
