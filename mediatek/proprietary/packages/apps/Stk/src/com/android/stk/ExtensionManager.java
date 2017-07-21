
package com.mediatek.stk.ext;

import android.app.Application;
import android.content.Context;
import android.util.Log;
import com.android.internal.telephony.cat.CatLog;

import com.mediatek.common.MPlugin;

public class ExtensionManager {

    private static final String TAG = "StkExtensionManager";

    private static ExtensionManager sInstance;
    private Context mContext;
    private IStkMainExt mStkMainExt;

    private ExtensionManager() {
    }

    public static ExtensionManager getInstance() {
        if (sInstance == null) {
            createInstanceSynchronized();
        }
        return sInstance;
    }

    private static synchronized void createInstanceSynchronized() {
        if (sInstance == null) {
            sInstance = new ExtensionManager();
        }
    }

    public void init(Application application) {
        mContext = application.getApplicationContext();
    }

    public IStkMainExt getStkMainExt() {
        synchronized (IStkMainExt.class) {
            if (mStkMainExt == null) {
                CatLog.d(TAG, "[getStkMainExt]mContext : " + mContext);
                mStkMainExt = (IStkMainExt) MPlugin.createInstance(IStkMainExt.class.getName(),
                        mContext);
                if (mStkMainExt == null) {
                    mStkMainExt = new DefaultStkMainExt();
                }
                CatLog.d(TAG, "[getStkMainExt]create ext instance: " + mStkMainExt);
            }
        }
        return mStkMainExt;
    }

}
