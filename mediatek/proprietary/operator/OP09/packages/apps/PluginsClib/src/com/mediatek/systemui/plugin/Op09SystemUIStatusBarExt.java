package com.mediatek.systemui.plugin;

import android.content.Context;
import android.os.Bundle;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.SystemProperties;
import android.telephony.ServiceState;
import android.telephony.SignalStrength;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.ImageView;

import com.android.internal.telephony.ITelephony;
import com.mediatek.common.PluginImpl;
import com.mediatek.internal.telephony.ITelephonyEx;
import com.mediatek.op09.plugin.R;
import com.mediatek.systemui.ext.DefaultSystemUIStatusBarExt;
import com.mediatek.telephony.TelephonyManagerEx;

/**
 * M: Op09 implementation of Plug-in definition of Status bar for C lib.
 */
@PluginImpl(interfaceName="com.mediatek.systemui.ext.ISystemUIStatusBarExt")
public class Op09SystemUIStatusBarExt extends DefaultSystemUIStatusBarExt {

    private static final String TAG = "Op09SystemUIStatusBarExt";
    private static final boolean DEBUG = true; //!isUserLoad();

    private Context mContext;

    private int mNetworkTypeIconId = 0;
    private int mDataTypeIconId = 0;
    private int mSignalStrengthIconId = 0;

    private ViewGroup mRoot = null;
    private ImageView mDataTypeView;
    private ImageView mDataActivityView;
    private ServiceState mServiceState;

    // Network type icons
    private final int NETWORK_TYPE_G  = R.drawable.stat_sys_network_type_g;
    private final int NETWORK_TYPE_2G = R.drawable.stat_sys_network_type_2g;
    private final int NETWORK_TYPE_1X = R.drawable.stat_sys_network_type_1x;
    private final int NETWORK_TYPE_3G = R.drawable.stat_sys_network_type_3g;
    private final int NETWORK_TYPE_4G = R.drawable.stat_sys_network_type_4g;
    // CA update
    private final int NETWORK_TYPE_4G_PLUS = R.drawable.stat_sys_network_type_4g_plus;

    // Data type icons
    private final int DATA_TYPE_2G = R.drawable.stat_sys_data_fully_connected_2g;
    private final int DATA_TYPE_3G = R.drawable.stat_sys_data_fully_connected_3g;
    private final int DATA_TYPE_4G = R.drawable.stat_sys_data_fully_connected_4g;
    private final int DATA_TYPE_4G_PLUS = R.drawable.stat_sys_data_fully_connected_4ga;

    // Data activity icons
    private final int DATA_ACTIVITY_NOT_INOUT = R.drawable.stat_sys_signal_not_inout;
    private final int DATA_ACTIVITY_IN = R.drawable.stat_sys_signal_in;
    private final int DATA_ACTIVITY_OUT = R.drawable.stat_sys_signal_out;
    private final int DATA_ACTIVITY_INOUT = R.drawable.stat_sys_signal_inout;

    // Data primary/roaming indicator icon
    private final int DATA_INDICATOR_PRIMARY =
                        R.drawable.stat_sys_data_primary_simcard;
    private final int DATA_INDICATOR_ROAMING =
                        R.drawable.stat_sys_data_fully_connected_roam;
    private final int DATA_INDICATOR_PRIMARY_ROAMING =
                        R.drawable.stat_sys_data_primary_simcard_roam;

    private final int SIGNAL_STRENGTH_NULL = R.drawable.stat_sys_signal_null;

    private final int[] SIGNAL_STRENGTH_SINGLE = {
        R.drawable.stat_sys_signal_0_fully,
        R.drawable.stat_sys_signal_1_fully,
        R.drawable.stat_sys_signal_2_fully,
        R.drawable.stat_sys_signal_3_fully,
        R.drawable.stat_sys_signal_4_fully
    };

    /**
     * Constructs a new Op09SystemUIStatusBarExt instance with Context.
     * @param context The Context object
     */
    public Op09SystemUIStatusBarExt(Context context) {
        super(context);
        mContext = context;
    }

    @Override
    public void getServiceStateForCustomizedView(int subId) {
        mServiceState = getServiceState(subId);
    }

    @Override
    public int getNetworkTypeIcon(int subId, int iconId, int networkType,
                    ServiceState serviceState) {
        int newType = getNetworkTypeFromServiceState(networkType, serviceState);
        if (newType != networkType) {
            Log.d(TAG, "getNetworkTypeIcon(), revise networkType,"
                + " subId=" + subId
                + ", oldType=" + networkType
                + ", newType=" + newType);
            networkType = newType;
        }

        if (!hasService(serviceState)) {
            mNetworkTypeIconId = 0;
        } else {
            mNetworkTypeIconId = getNetworkTypeIconId(networkType);
        }

        if (DEBUG) {
            Log.d(TAG, "getNetworkTypeIcon(), subId=" + subId
                + ", networkType=" + networkType
                + ", iconId=" + mNetworkTypeIconId);
        }

        return mNetworkTypeIconId;
    }

    @Override
    public int getDataTypeIcon(int subId, int iconId,
                    int dataType, int dataState, ServiceState serviceState) {
        int newState = TelephonyManager.DATA_DISCONNECTED;
        if (hasService(serviceState)) {
            newState = TelephonyManager.getDefault().getDataState(subId);
        }
        if (newState != dataState) {
            Log.d(TAG, "getDataTypeIcon(), revise dataState,"
                + " subId=" + subId
                + ", oldState=" + dataState
                + ", newState=" + newState);
            dataState = newState;
        }

        mDataTypeIconId = getDataTypeIconId(dataType, dataState);

        if (DEBUG) {
            Log.d(TAG, "getDataTypeIcon(), subId=" + subId
                + ", dataType=" + dataType
                + ", dataState=" + dataState
                + ", iconId=" + mDataTypeIconId);
        }

        return mDataTypeIconId;
    }

    @Override
    public int getCustomizeSignalStrengthIcon(int subId, int iconId,
                    SignalStrength signalStrength, int networkType,
                    ServiceState serviceState) {
        int newType = getNetworkTypeFromServiceState(networkType, serviceState);
        if (newType != networkType) {
            if (DEBUG) {
                Log.d(TAG, "getCustomizeSignalStrengthIcon(), revise networkType,"
                    + " subId=" + subId
                    + ", oldType=" + networkType
                    + ", newType=" + newType);
            }
            networkType = newType;
        }
        if (hasService(serviceState) && signalStrength != null) {
            mSignalStrengthIconId = getSignalStrengthIconId(
                signalStrength, networkType);
        } else {
            mSignalStrengthIconId = SIGNAL_STRENGTH_NULL;
        }

        if (DEBUG) {
            Log.d(TAG, "getCustomizeSignalStrengthIcon(), subId=" + subId
                + ", networkType=" + networkType
                + ", iconId=" + mSignalStrengthIconId
                + ", signalStrength=" + signalStrength);
        }

        return mSignalStrengthIconId;
    }

    @Override
    public void addCustomizedView(int subId,
                    Context context, ViewGroup root) {
        int slotId = SubscriptionManager.getSlotId(subId);
        FrameLayout dataGroup = (FrameLayout) LayoutInflater.from(mContext)
            .inflate(R.layout.mobile_data_group, null);

        mRoot = root;
        mDataTypeView = (ImageView) dataGroup.findViewById(R.id.data_type);
        mDataActivityView = (ImageView)
            dataGroup.findViewById(R.id.data_activity);

        // Add the data group view in the forefront of the view group
        root.addView(dataGroup, 0);

        if (DEBUG) {
            Log.d(TAG, "addCustomizedView(), subId=" + subId
                + ", slotId=" + slotId
                + ", root=" + root);
        }
    }

    @Override
    public void setCustomizedNetworkTypeView(int subId,
                    int networkTypeId, ImageView networkTypeView) {
        if (networkTypeView != null) {
            if (DEBUG) {
                Log.d(TAG, "NetworkTypeView, subId=" + subId + " networkTypeId=" + networkTypeId);
            }

            if (!isSimInsert(subId)) {
                networkTypeView.setVisibility(View.GONE);
                return;
            }

            if (networkTypeId != 0) {
                networkTypeView.setImageDrawable(
                    mContext.getResources().getDrawable(networkTypeId));
                networkTypeView.setVisibility(View.VISIBLE);
            } else {
                networkTypeView.setVisibility(View.GONE);
            }
        }
    }

    @Override
    public void setCustomizedDataTypeView(int subId,
                    int dataTypeId, boolean dataIn, boolean dataOut) {
        if (mRoot != null) {
            if (DEBUG) {
                Log.d(TAG, "DataTypeView, subId=" + subId + " dataTypeId=" + dataTypeId);
            }

            if (!isSimInsert(subId)) {
                mDataTypeView.setVisibility(View.GONE);
                mDataActivityView.setVisibility(View.GONE);
                return;
            }

            if (dataTypeId != 0) {
                mDataTypeView.setImageDrawable(
                    mContext.getResources().getDrawable(dataTypeId));
                mDataTypeView.setVisibility(View.VISIBLE);

                int dataActivityId = getDataActivityIconId(dataIn, dataOut);
                if (dataActivityId != 0) {
                    mDataActivityView.setImageDrawable(
                        mContext.getResources().getDrawable(dataActivityId));
                    mDataActivityView.setVisibility(View.VISIBLE);
                } else {
                    mDataActivityView.setVisibility(View.GONE);
                }
            } else {
                mDataTypeView.setVisibility(View.GONE);
                mDataActivityView.setVisibility(View.GONE);
            }
        }
    }

    @Override
    public void setCustomizedMobileTypeView(int subId,
                    ImageView mobileTypeView) {
        if (mobileTypeView != null) {
            if (DEBUG) {
                Log.d(TAG, "setCustomizedMobileTypeView, subId=" + subId);
            }
            if (!isSimInsert(subId)) {
                mobileTypeView.setVisibility(View.GONE);
                return;
            }

            int iconId = getMobileTypeIconId(subId);

            if (iconId != 0) {
                mobileTypeView.setImageDrawable(
                    mContext.getResources().getDrawable(iconId));
                mobileTypeView.setVisibility(View.VISIBLE);
            } else {
                mobileTypeView.setVisibility(View.GONE);
            }
        }
    }

    @Override
    public void setCustomizedSignalStrengthView(int subId,
                    int signalStrengthId, ImageView signalStrengthView) {
        if (signalStrengthView != null) {
            if (DEBUG) {
                Log.d(TAG, "SignalView, subId=" + subId + " signalId=" + signalStrengthId);
            }

            if (!isSimInsert(subId)) {
                signalStrengthView.setVisibility(View.GONE);
                return;
            }
            if (isSimOffline(subId, mServiceState)) {
                signalStrengthId = SIGNAL_STRENGTH_NULL;
            }
            if (signalStrengthId != 0) {
                signalStrengthView.setImageDrawable(
                    mContext.getResources().getDrawable(signalStrengthId));
                signalStrengthView.setVisibility(View.VISIBLE);
            } else {
                signalStrengthView.setVisibility(View.GONE);
            }
        }
    }

    public static final boolean isUserLoad() {
        return SystemProperties.get("ro.build.type").equals("user");
    }

    public static final boolean isCASupport() {
        return SystemProperties.get("gsm.lte.ca.support").equals("1");
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

    private boolean isSimRoaming(int subId, ServiceState serviceState) {
        boolean isRoaming = false;

        if (serviceState != null) {
            isRoaming =
                !TelephonyManagerEx.getDefault().isInHomeNetwork(subId);
        }

        if (DEBUG) {
            Log.d(TAG, "isSimRoaming: roaming=" + isRoaming
                + ", subId=" + subId
                + ", serviceState=" + serviceState);
        }

        return isRoaming;
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

    private boolean isSimOffline(int subId, ServiceState serviceState) {
        boolean ret;
        boolean isRadioOn = isSimRadioOn(subId);
        boolean isService = hasService(serviceState);
        int slotId = SubscriptionManager.getSlotId(subId);

        if (slotId < 0) {
            ret = true;
        } else {
            ret = (!isRadioOn || !isService);
        }

        if (DEBUG) {
            Log.d(TAG, "isSimOffline: offline=" + ret
                + ", subId=" + subId + ",slotId=" + slotId
                + ", isService=" + isService
                + ", serviceState=" + serviceState);
        }

        return ret;
    }

    private boolean isSimInsert(int subId) {
        int slotId = SubscriptionManager.getSlotId(subId);
        boolean inserted = false;
        if (slotId >= 0) {
            inserted = TelephonyManager.getDefault().hasIccCard(slotId);
        }
        Log.d(TAG, "isSimInsert: slotId=" + slotId + " inserted=" + inserted);

        return inserted;
    }

    private int getNetworkTypeIconId(int networkType) {
        int iconId = 0;

        switch (networkType) {
            case TelephonyManager.NETWORK_TYPE_UNKNOWN:
                break;

            case TelephonyManager.NETWORK_TYPE_GSM:
                iconId = NETWORK_TYPE_G;
                break;

            case TelephonyManager.NETWORK_TYPE_GPRS:
            case TelephonyManager.NETWORK_TYPE_EDGE:
                iconId = NETWORK_TYPE_2G;
                break;

            case TelephonyManager.NETWORK_TYPE_CDMA:
            case TelephonyManager.NETWORK_TYPE_1xRTT:
                iconId = NETWORK_TYPE_1X;
                break;

            case TelephonyManager.NETWORK_TYPE_HSDPA:
            case TelephonyManager.NETWORK_TYPE_HSUPA:
            case TelephonyManager.NETWORK_TYPE_HSPA:
            case TelephonyManager.NETWORK_TYPE_HSPAP:
            case TelephonyManager.NETWORK_TYPE_UMTS:
            case TelephonyManager.NETWORK_TYPE_EVDO_0:
            case TelephonyManager.NETWORK_TYPE_EVDO_A:
            case TelephonyManager.NETWORK_TYPE_EVDO_B:
            case TelephonyManager.NETWORK_TYPE_EHRPD:
                iconId = NETWORK_TYPE_3G;
                break;

            case TelephonyManager.NETWORK_TYPE_LTE:
                if (isCASupport()) {
                    iconId = NETWORK_TYPE_4G_PLUS;
                } else {
                    iconId = NETWORK_TYPE_4G;
                }
                break;

            case TelephonyManager.NETWORK_TYPE_LTEA:
                iconId = NETWORK_TYPE_4G_PLUS;
                break;

            default:
                break;
        }

        return iconId;
    }

    private int getDataTypeIconId(int dataType, int dataState) {
        int iconId = 0;

        if (dataState == TelephonyManager.DATA_CONNECTED) {
            switch (dataType) {
                case TelephonyManager.NETWORK_TYPE_UNKNOWN:
                    break;

                case TelephonyManager.NETWORK_TYPE_GSM:
                case TelephonyManager.NETWORK_TYPE_GPRS:
                case TelephonyManager.NETWORK_TYPE_EDGE:
                case TelephonyManager.NETWORK_TYPE_CDMA:
                case TelephonyManager.NETWORK_TYPE_1xRTT:
                    iconId = DATA_TYPE_2G;
                    break;

                case TelephonyManager.NETWORK_TYPE_HSDPA:
                case TelephonyManager.NETWORK_TYPE_HSUPA:
                case TelephonyManager.NETWORK_TYPE_HSPA:
                case TelephonyManager.NETWORK_TYPE_HSPAP:
                case TelephonyManager.NETWORK_TYPE_UMTS:
                case TelephonyManager.NETWORK_TYPE_EVDO_0:
                case TelephonyManager.NETWORK_TYPE_EVDO_A:
                case TelephonyManager.NETWORK_TYPE_EVDO_B:
                case TelephonyManager.NETWORK_TYPE_EHRPD:
                    iconId = DATA_TYPE_3G;
                    break;

                case TelephonyManager.NETWORK_TYPE_LTE:
                    if (isCASupport()) {
                        iconId = DATA_TYPE_4G_PLUS;
                    } else {
                        iconId = DATA_TYPE_4G;
                    }
                    break;

                case TelephonyManager.NETWORK_TYPE_LTEA:
                    iconId = DATA_TYPE_4G_PLUS;
                    break;

                default:
                    break;
            }
        }

        return iconId;
    }

    private int getDataActivityIconId(boolean dataIn, boolean dataOut) {
        int iconId = 0;

        if (dataIn && dataOut) {
            iconId = DATA_ACTIVITY_INOUT;
        } else if (dataIn) {
            iconId = DATA_ACTIVITY_IN;
        } else if (dataOut) {
            iconId = DATA_ACTIVITY_OUT;
        }

        return iconId;
    }

    private int getMobileTypeIconId(int subId) {
        int iconId = 0;
        int defaultDataSubId = SubscriptionManager.getDefaultDataSubId();
        ServiceState serviceState = getServiceState(subId);
        boolean isRoaming = isSimRoaming(subId, serviceState);

        if (defaultDataSubId != SubscriptionManager.INVALID_SUBSCRIPTION_ID
            && defaultDataSubId == subId) {
            if (isRoaming) {
                iconId = DATA_INDICATOR_PRIMARY_ROAMING;
            } else {
                iconId = DATA_INDICATOR_PRIMARY;
            }
        } else if (isRoaming) {
            iconId = DATA_INDICATOR_ROAMING;
        }

        return iconId;
    }

    private int getSignalStrengthIconId(
                    SignalStrength signalStrength, int networkType) {
        int level = 0;

        if (signalStrength != null) {
            switch (networkType) {
                case TelephonyManager.NETWORK_TYPE_UNKNOWN:
                case TelephonyManager.NETWORK_TYPE_GSM:
                case TelephonyManager.NETWORK_TYPE_GPRS:
                case TelephonyManager.NETWORK_TYPE_EDGE:
                    level = signalStrength.getLevel();
                    break;

                case TelephonyManager.NETWORK_TYPE_CDMA:
                case TelephonyManager.NETWORK_TYPE_1xRTT:
                    level = signalStrength.getCdmaLevel();
                    break;

                case TelephonyManager.NETWORK_TYPE_HSDPA:
                case TelephonyManager.NETWORK_TYPE_HSUPA:
                case TelephonyManager.NETWORK_TYPE_HSPA:
                case TelephonyManager.NETWORK_TYPE_HSPAP:
                case TelephonyManager.NETWORK_TYPE_UMTS:
                    level = signalStrength.getLevel();
                    break;

                case TelephonyManager.NETWORK_TYPE_EVDO_0:
                case TelephonyManager.NETWORK_TYPE_EVDO_A:
                case TelephonyManager.NETWORK_TYPE_EVDO_B:
                case TelephonyManager.NETWORK_TYPE_EHRPD:
                    level = signalStrength.getEvdoLevel();
                    break;

                case TelephonyManager.NETWORK_TYPE_LTE:
                case TelephonyManager.NETWORK_TYPE_LTEA:
                    level = signalStrength.getLteLevel();
                    break;

                default:
                    break;
            }
        }

        return SIGNAL_STRENGTH_SINGLE[level];
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

    private int getNetworkTypeFromServiceState(
                    int networkType, ServiceState serviceState) {
        int type = networkType;
        if (serviceState != null) {
            type = serviceState.getDataNetworkType()
                != TelephonyManager.NETWORK_TYPE_UNKNOWN
                ? serviceState.getDataNetworkType()
                : serviceState.getVoiceNetworkType();
        }
        return type;
    }

}
