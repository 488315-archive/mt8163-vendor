package com.mediatek.op.telephony;

import com.android.internal.telephony.PhoneConstants;
import android.content.Context;
import android.content.Intent;
import android.telephony.Rlog;
import android.text.TextUtils;

import com.mediatek.common.PluginImpl;

/**
 * TelephonyExt OP08 plugin.
 *
 */
@PluginImpl(interfaceName="com.mediatek.common.telephony.ITelephonyExt")
public class TelephonySSExtOP08 extends TelephonyExt {
    private static final String TAG = "TelephonySSExt08";
    private static final String SETTINGS_CHANGED_OR_SS_COMPLETE
            = "com.mediatek.op.telephony.SETTINGS_CHANGED_OR_SS_COMPLETE";

    @Override
   /**
    * Sends to intent to start re registration
    * @param context Context
    */
    public void resetImsPdnOverSSComplete(Context context) {
        Intent intent = new Intent(SETTINGS_CHANGED_OR_SS_COMPLETE);
        context.sendBroadcast (intent);
        Rlog.d(TAG, "Intent Broadcast " + SETTINGS_CHANGED_OR_SS_COMPLETE);
    }

    @Override
    public boolean ignoreDataRoaming(String apnType) {
        if (TextUtils.equals(apnType, PhoneConstants.APN_TYPE_IMS)) {
            Rlog.d(TAG, "ignoreDataRoaming, apnType = " + apnType);
            return true;
        }
        return false;
    }

}

