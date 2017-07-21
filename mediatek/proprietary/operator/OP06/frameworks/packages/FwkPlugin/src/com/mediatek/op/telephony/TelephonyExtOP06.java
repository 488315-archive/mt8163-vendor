package com.mediatek.op.telephony;

import com.android.internal.telephony.PhoneConstants;
import android.telephony.Rlog;
import android.text.TextUtils;
import com.mediatek.common.PluginImpl;


/**
 * TelephonyExt OP06 plugin.
 *
 */
@PluginImpl(interfaceName="com.mediatek.common.telephony.ITelephonyExt")
public class TelephonyExtOP06 extends TelephonyExt {
    private static final String TAG = "TelephonyExtOP06";

    @Override
    public boolean ignoreDataRoaming(String apnType) {
        if (TextUtils.equals(apnType, PhoneConstants.APN_TYPE_IMS)) {
            Rlog.d(TAG, "ignoreDataRoaming, apnType = " + apnType);
            return true;
        }
        return false;
    }


}
