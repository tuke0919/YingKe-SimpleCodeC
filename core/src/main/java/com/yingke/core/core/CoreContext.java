package com.yingke.core.core;

import android.app.Application;

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
public class CoreContext {

    private final static class SingletonHolder {
        public final static  CoreContext INSTANCE = new CoreContext();
    }

    public static CoreContext get(){
        return SingletonHolder.INSTANCE;
    }

    public void init(Application application){
        sApplication = application;
    }

    private static Application sApplication;
    public static Application context() {
        return sApplication;
    }
}
