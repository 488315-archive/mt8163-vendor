package com.mediatek.settings.plugin;

import android.content.Context;
import android.content.Intent;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.util.Log;

import com.android.internal.telephony.ITelephony;

/**
* Since setPreferredNetworkType is a block API, call it in work thread.
*/
public class ChangeNetworkType implements Runnable {

    private int mSubId = -1;
    private int mNetworkType = -1;
    private ITelephony mTelephony = null;
    private Context mContext = null;
    private static final String TAG = "ChangeNetworkType";
    private static final String INTENT_ACTION_FINISH_SWITCH_SVLTE_RAT_MODE =
            "com.mediatek.intent.action.FINISH_SWITCH_SVLTE_RAT_MODE";
    /**
    * Construct method.
    * @param subId sub id
    * @param networkType network type
    * @param context app context or plugin context
    */
    public ChangeNetworkType(int subId, int networkType, Context context) {
        mSubId = subId;
        mNetworkType = networkType;
        mTelephony = ITelephony.Stub.asInterface(ServiceManager
                .getService(Context.TELEPHONY_SERVICE));
        mContext = context;
    }

    @Override
    public void run() {
        boolean result = false;
        try {
            result = mTelephony.setPreferredNetworkType(mSubId, mNetworkType);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
        Log.d(TAG, "setPreferredNetworkType return: " + result);
        if (result) {
            Intent intent = new Intent(INTENT_ACTION_FINISH_SWITCH_SVLTE_RAT_MODE);
            mContext.sendBroadcast(intent);
        }
    }
}