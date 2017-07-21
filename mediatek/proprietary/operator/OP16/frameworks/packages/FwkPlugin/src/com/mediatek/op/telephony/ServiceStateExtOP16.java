package com.mediatek.op.telephony;

import android.content.Context;
import android.os.SystemProperties;
import android.telephony.ServiceState;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.android.ims.ImsException;
import com.android.ims.ImsManager;
import com.android.internal.telephony.uicc.SIMRecords;

import com.mediatek.common.PluginImpl;
import com.mediatek.ims.WfcReasonInfo;

@PluginImpl(interfaceName = "com.mediatek.common.telephony.IServiceStateExt")
public class ServiceStateExtOP16 extends DefaultServiceStateExt {
    Context mContext;
    final String TAG = "ServiceStateExtOP16";
    final String OPERATOR_PLMN = "EE WiFiCall";

    public ServiceStateExtOP16() {
    }

    public ServiceStateExtOP16(Context context) {
        mContext = context;
        Log.d(TAG, "ServiceStateExtOP16");
    }

    public String onUpdateSpnDisplayForIms(String plmn, ServiceState ss,
            int lac, int phoneId, Object simRecords1) {
        String eons = null;
        if (SystemProperties.get("ro.mtk_wfc_support").equals("1")
                && ss.getVoiceRegState() == ServiceState.STATE_POWER_OFF
                && simRecords1 != null) {
            SIMRecords simRecords = (SIMRecords)simRecords1;
            try {
                boolean imsRegStatus = ImsManager
                        .getInstance(mContext, phoneId).getImsRegInfo();
                Log.d(TAG, "Ims Registration Status = " + imsRegStatus);
                if (imsRegStatus) {
                    try {
                        eons = (simRecords != null) ? simRecords
                                .getEonsIfExist(ss.getOperatorNumeric(), lac,
                                        true) : null;
                    } catch (RuntimeException ex) {
                        Log.e(TAG, "Exception while getEonsIfExist. " + ex);
                    }

                    if (eons != null) {
                        plmn = eons;
                    } else {
                        String tempPlmn = simRecords.getSIMCPHSOns();
                        if (tempPlmn != null) {
                            plmn = simRecords.getSIMCPHSOns();
                        }
                    }
                }
            } catch (ImsException ex) {
                Log.e(TAG, "Fail to get Ims Status");
            }
        }

        TelephonyManager telephonyManager = (TelephonyManager) mContext
                .getSystemService(Context.TELEPHONY_SERVICE);
        boolean isWFCEnabled = telephonyManager.isWifiCallingEnabled();
        Log.d("ServiceStateExtOP16", "isWFCEnabled = " + isWFCEnabled);
        if (isWFCEnabled) {
            return OPERATOR_PLMN;
        } else {
            return plmn;
        }
    }
}