package com.mediatek.op.telephony;

import com.android.internal.telephony.PhoneConstants;
import android.telephony.Rlog;
import android.text.TextUtils;
import com.mediatek.common.PluginImpl;


/**
 * TelephonyExt OP16 plugin.
 *
 */
@PluginImpl(interfaceName="com.mediatek.common.telephony.ITelephonyExt")
public class TelephonyExtOP16 extends TelephonyExt {
    private static final String TAG = "TelephonyExtOP16";

    @Override
    public boolean ignoreDataRoaming(String apnType) {
        if (TextUtils.equals(apnType, PhoneConstants.APN_TYPE_IMS)) {
            Rlog.d(TAG, "ignoreDataRoaming, apnType = " + apnType);
            return true;
        }
        return false;
    }


}
