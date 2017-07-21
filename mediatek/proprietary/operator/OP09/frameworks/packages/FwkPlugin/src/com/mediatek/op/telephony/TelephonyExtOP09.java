package com.mediatek.op.telephony;

import android.telephony.Rlog;

import com.android.internal.telephony.PhoneProxy;
import com.mediatek.common.PluginImpl;

/**
 * TelephonyExt OP09 plugin.
 *
 */
@PluginImpl(interfaceName="com.mediatek.common.telephony.ITelephonyExt")
public class TelephonyExtOP09 extends TelephonyExt {
    private static final String TAG = "TelephonyExtOP09";
    private static final int CT_SIM_SLOT = 0;
    private DataRoamingCustomization mDataRoamingCustomization;

    @Override
    public boolean ignoreDataRoaming(String apnType) {
        Rlog.d(TAG, "ignoreDataRoaming, return true");
        return true;
    }

    @Override
    public void startDataRoamingStrategy(Object phoneProxy) {
        if (((PhoneProxy) phoneProxy).getPhoneId() == CT_SIM_SLOT) {
            Rlog.d(TAG, "createDataRoamingCustom");
            mDataRoamingCustomization = new DataRoamingCustomizationOP09(
                  (PhoneProxy) phoneProxy);
        }
    }

    @Override
    public void stopDataRoamingStrategy() {
        if (mDataRoamingCustomization != null) {
            mDataRoamingCustomization.dispose();
        }
    }
}
