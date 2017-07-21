package com.mediatek.op.telephony;

import com.android.internal.telephony.PhoneConstants;
import android.text.TextUtils;
import android.telephony.Rlog;
import com.mediatek.common.PluginImpl;

@PluginImpl(interfaceName="com.mediatek.common.telephony.ITelephonyExt")
public class TelephonyExtOP03 extends TelephonyExt {
    private static final String TAG = "TelephonyExtOP03";

    public boolean isSetLanguageBySIM() {
        return true;
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
