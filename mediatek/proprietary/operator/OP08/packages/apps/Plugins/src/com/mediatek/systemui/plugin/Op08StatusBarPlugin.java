package com.mediatek.systemui.plugin;

import android.content.Context;
import android.os.Bundle;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.telephony.ServiceState;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.android.internal.telephony.ITelephony;

import com.mediatek.common.PluginImpl;
import com.mediatek.internal.telephony.ITelephonyEx;
import com.mediatek.op08.plugin.R;
import com.mediatek.systemui.ext.DefaultSystemUIStatusBarExt;
/**
 * M: OP08 implementation of Plug-in definition of Status bar.
 */
@PluginImpl(interfaceName = "com.mediatek.systemui.ext.ISystemUIStatusBarExt")
public class Op08StatusBarPlugin extends DefaultSystemUIStatusBarExt {

    private static final String TAG = "Op08StatusBarPlugin";
    private Context mContext;
    private static final int OPTR_MCCMNC = 310260;
    private int mNetworkTypeIconId = 0;
    private int mDataTypeIconId = 0;
    // Network type icons
    private final int NETWORK_TYPE_G = R.drawable.stat_sys_network_type_g;
    private final int NETWORK_TYPE_E = R.drawable.stat_sys_network_type_e;
    private final int NETWORK_TYPE_2G = R.drawable.stat_sys_network_type_2g;
    private final int NETWORK_TYPE_3G = R.drawable.stat_sys_network_type_3g;
    private final int NETWORK_TYPE_4G = R.drawable.stat_sys_network_type_4g;

    // Data type icons
    private final int DATA_TYPE_G = R.drawable.stat_sys_data_fully_connected_g;
    private final int DATA_TYPE_E = R.drawable.stat_sys_data_fully_connected_e;
    private final int DATA_TYPE_2G = R.drawable.stat_sys_data_fully_connected_2g;
    private final int DATA_TYPE_3G = R.drawable.stat_sys_data_fully_connected_3g;
    private final int DATA_TYPE_4G = R.drawable.stat_sys_data_fully_connected_4g;
    private final int DATA_TYPE_4G_PLUS = R.drawable.stat_sys_data_fully_connected_4ga;

    /** Constructor.
     * @param context id
    */
    public Op08StatusBarPlugin(Context context) {
        super(context);
        mContext = context;
    }

    /** updateIconOnModulation.
     * @param dataNetType network type
     * @return return the new icon type
    */
    public int updateIconOnModulation(int dataNetType) {
        Log.d(TAG, "updateIconOnModulation R.drawable.stat_sys_network_type_4g ="
                 + R.drawable.stat_sys_network_type_4g);
        int mccMnc = 0;
        String networkOperator = TelephonyManager.getDefault().getNetworkOperator();
        if (networkOperator != null && !networkOperator.isEmpty()) {
            Log.d(TAG, networkOperator);
            mccMnc = Integer.parseInt(networkOperator);
        } else {
            Log.d(TAG, "NULL or empty Network Operator string");
            return dataNetType;
        }
        if (mccMnc == OPTR_MCCMNC) {
            return R.drawable.stat_sys_network_type_4g;
        } else {
            return dataNetType;
        }
    }

    @Override
    public int getNetworkTypeIcon(int subId, int iconId, int networkType,
                    ServiceState serviceState) {
        if (isSimOffline(subId, serviceState)) {
            mNetworkTypeIconId = 0;
        } else {
            mNetworkTypeIconId = getNetworkTypeIconId(networkType, iconId);
        }
        Log.d(TAG, "getNetworkTypeIcon(), subId=" + subId
                + ", networkType=" + networkType
                + ", iconId=" + mNetworkTypeIconId
                + ", serviceState=" + serviceState);

        return mNetworkTypeIconId;
    }

    @Override
    public int getDataTypeIcon(int subId, int iconId,
                    int dataType, int dataState, ServiceState serviceState) {
        mDataTypeIconId = getDataTypeIconId(dataType, dataState, iconId);
        Log.d(TAG, "getDataTypeIcon(), subId=" + subId
                + ", dataType=" + dataType
                + ", dataState=" + dataState
                + ", iconId=" + mDataTypeIconId);

        return mDataTypeIconId;
    }


    private int getNetworkTypeIconId(int networkType, int iconId) {
        int netIconId = 0;
        int mccMnc = 0;
        String networkOperator = TelephonyManager.getDefault().getNetworkOperator();
        if (networkOperator != null && !networkOperator.isEmpty()) {
            Log.d(TAG, networkOperator);
            mccMnc = Integer.parseInt(networkOperator);
        } else {
            Log.d(TAG, "NULL or empty Network Operator string");
            return iconId;
        }
        switch (networkType) {
            case TelephonyManager.NETWORK_TYPE_HSDPA:
            case TelephonyManager.NETWORK_TYPE_HSUPA:
            case TelephonyManager.NETWORK_TYPE_HSPA:
            case TelephonyManager.NETWORK_TYPE_HSPAP:
            case TelephonyManager.NETWORK_TYPE_UMTS:
            case TelephonyManager.NETWORK_TYPE_EVDO_0:
            case TelephonyManager.NETWORK_TYPE_EVDO_A:
            case TelephonyManager.NETWORK_TYPE_EVDO_B:
            case TelephonyManager.NETWORK_TYPE_EHRPD:
                 if (mccMnc == OPTR_MCCMNC) {
                     netIconId = NETWORK_TYPE_4G;
                 } else {
                     netIconId = NETWORK_TYPE_3G;
                 }
                 break;
            case TelephonyManager.NETWORK_TYPE_LTE:
                netIconId = NETWORK_TYPE_4G;
                break;
            default:
                netIconId = iconId;
                break;
        }

        return netIconId;
    }

    private int getDataTypeIconId(int dataType, int dataState, int oriIconId) {
        int iconId = 0;
        int mccMnc = 0;
        String networkOperator = TelephonyManager.getDefault().getNetworkOperator();
        if (networkOperator != null && !networkOperator.isEmpty()) {
            Log.d(TAG, networkOperator);
            mccMnc = Integer.parseInt(networkOperator);
        } else {
            Log.d(TAG, "NULL or empty Network Operator string");
            return oriIconId;
        }
        if (dataState != TelephonyManager.DATA_CONNECTED) {
            return iconId;
        }

        switch (dataType) {
            case TelephonyManager.NETWORK_TYPE_HSDPA:
            case TelephonyManager.NETWORK_TYPE_HSUPA:
            case TelephonyManager.NETWORK_TYPE_HSPA:
            case TelephonyManager.NETWORK_TYPE_HSPAP:
            case TelephonyManager.NETWORK_TYPE_UMTS:
            case TelephonyManager.NETWORK_TYPE_EVDO_0:
            case TelephonyManager.NETWORK_TYPE_EVDO_A:
            case TelephonyManager.NETWORK_TYPE_EVDO_B:
            case TelephonyManager.NETWORK_TYPE_EHRPD:
                 if (mccMnc == OPTR_MCCMNC) {
                     iconId = DATA_TYPE_4G;
                 } else {
                     iconId = DATA_TYPE_3G;
                 }
                break;
            case TelephonyManager.NETWORK_TYPE_LTE:
                iconId = DATA_TYPE_4G;
                break;
            default:
                iconId = oriIconId;
                break;
        }

        return iconId;
    }

    private boolean isSimOffline(int subId, ServiceState serviceState) {
        boolean ret;

        boolean isEmergencyOnly =
            serviceState != null ? serviceState.isEmergencyOnly() : false;
        boolean isRadioOn = isSimRadioOn(subId);

        if (isEmergencyOnly) {
            ret = true;
        } else {
            ret = !hasService(serviceState);
        }

            Log.d(TAG, "isSimOffline: offline=" + ret
                + ", subId=" + subId
                + ", serviceState=" + serviceState);

        return ret;
    }

    private ServiceState getServiceState(int subId) {
        ServiceState serviceState = null;
        Bundle bd = null;

        try {
            ITelephonyEx phoneEx = ITelephonyEx.Stub.asInterface(
                    ServiceManager.checkService("phoneEx"));

            if (phoneEx != null) {
                bd = phoneEx.getServiceState(subId);
                if (bd != null) {
                    serviceState = ServiceState.newFromBundle(bd);
                }
            }
        } catch (RemoteException e) {
            Log.e(TAG, "getServiceState error e:" + e.getMessage());
        }

        return serviceState;
    }

    private boolean isSimRadioOn(int subId) {
        ITelephony telephony = ITelephony.Stub.asInterface(ServiceManager
                .getService(Context.TELEPHONY_SERVICE));
        try {
            if (telephony != null) {
                return telephony.isRadioOnForSubscriber(
                            subId, mContext.getPackageName());
            }
        } catch (RemoteException e) {
            Log.e(TAG, "ITelephony.isRadioOnForSubscriber exception");
        }
        return false;
    }

    private boolean hasService(ServiceState serviceState) {
        if (serviceState != null) {
            // Consider the device to be in service if either voice or data
            // service is available. Some SIM cards are marketed as data-only
            // and do not support voice service, and on these SIM cards, we
            // want to show signal bars for data service as well as the "no
            // service" or "emergency calls only" text that indicates that voice
            // is not available.
            switch (serviceState.getVoiceRegState()) {
                case ServiceState.STATE_POWER_OFF:
                    return false;
                case ServiceState.STATE_OUT_OF_SERVICE:
                case ServiceState.STATE_EMERGENCY_ONLY:
                    return serviceState.getDataRegState() == ServiceState.STATE_IN_SERVICE;
                default:
                    return true;
            }
        } else {
            return false;
        }
    }

}
