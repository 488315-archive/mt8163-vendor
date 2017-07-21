package com.mediatek.mms.ext;

import android.app.Activity;
import android.os.SystemProperties;
import android.util.Log;

import com.mediatek.telephony.TelephonyManagerEx;


public class DefaultOpSubSelectActivityExt implements IOpSubSelectActivityExt {
    public static final boolean MTK_C2K_SUPPORT = SystemProperties.get("ro.mtk_c2k_support")
            .equals("1");
    private static final String TAG = "Mms/DefaultOpSubSelectActivityExt";

    @Override
    public void onCreate(Activity hostActivity) {

    }

    public boolean onListItemClick(Activity hostActivity, final int subId) {
        return false;
    }

    @Override
    public String [] setSaveLocation() {
        return null;
    }

    @Override
    public boolean isSimSupported(int subId) {
        Log.d(TAG, "[isSimSupported]: subId=" + subId);
        if (MTK_C2K_SUPPORT && isUSimType(subId)) {
            Log.d(TAG, "[isSimSupported]: false");
            return false;
        }
        Log.d(TAG, "[isSimSupported]: true");
        return true;
    }

    private boolean isUSimType(int subId) {
        String phoneType = TelephonyManagerEx.getDefault().getIccCardType(subId);
        if (phoneType == null) {
            Log.d(TAG, "[isUIMType]: phoneType = null");
            return false;
        }
        Log.d(TAG, "[isUIMType]: phoneType = " + phoneType);
        return phoneType.equalsIgnoreCase("CSIM") || phoneType.equalsIgnoreCase("UIM")
            || phoneType.equalsIgnoreCase("RUIM");
    }
}
