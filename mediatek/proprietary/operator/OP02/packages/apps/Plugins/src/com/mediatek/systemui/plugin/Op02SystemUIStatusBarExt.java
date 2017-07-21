package com.mediatek.systemui.plugin;

import android.content.Context;
import android.os.Bundle;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.SystemProperties;
import android.util.Log;
import android.telephony.ServiceState;
import android.telephony.SignalStrength;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.ImageView;

import com.android.internal.telephony.ITelephony;

import com.mediatek.common.PluginImpl;
import com.mediatek.internal.telephony.ITelephonyEx;
import com.mediatek.op02.plugin.R;
import com.mediatek.systemui.ext.DefaultSystemUIStatusBarExt;

import java.util.ArrayList;
import java.util.List;

/**
 * M: Op02 implementation of Plug-in definition of Status bar.
 */
@PluginImpl(interfaceName="com.mediatek.systemui.ext.ISystemUIStatusBarExt")
public class Op02SystemUIStatusBarExt extends DefaultSystemUIStatusBarExt {

    private static final String TAG = "Op02SystemUIStatusBarExt";
    private static final boolean DEBUG = true; //!isUserLoad();

    private Context mContext;

    private final int mCount;
    private int mSlotId = SubscriptionManager.INVALID_SIM_SLOT_INDEX;
    private int mSubId = SubscriptionManager.INVALID_SUBSCRIPTION_ID;
    private int mNetworkTypeIconId = 0;
    private int mDataTypeIconId = 0;
    private int mSignalStrengthIconId = 0;

    private ViewGroup mRoot = null;
    private ImageView mDataTypeView;
    private ImageView mDataActivityView;
    private NoSimView mNoSimView;
    private ServiceState mServiceState;

    // Network type icons
    private final int NETWORK_TYPE_G = R.drawable.stat_sys_network_type_g;
    private final int NETWORK_TYPE_3G = R.drawable.stat_sys_network_type_3g;
    private final int NETWORK_TYPE_4G = R.drawable.stat_sys_network_type_4g;

    // Data type icons
    private final int DATA_TYPE_G = R.drawable.stat_sys_data_fully_connected_g;
    private final int DATA_TYPE_E = R.drawable.stat_sys_data_fully_connected_e;
    private final int DATA_TYPE_H = R.drawable.stat_sys_data_fully_connected_h;
    private final int DATA_TYPE_H_PLUS = R.drawable.stat_sys_data_fully_connected_h_plus;
    private final int DATA_TYPE_3G = R.drawable.stat_sys_data_fully_connected_3g;
    private final int DATA_TYPE_4G = R.drawable.stat_sys_data_fully_connected_4g;
    private final int DATA_TYPE_4G_PLUS = R.drawable.stat_sys_data_fully_connected_4ga;

    // Data activity icons
    private final int DATA_ACTIVITY_NOT_INOUT = R.drawable.stat_sys_signal_not_inout;
    private final int DATA_ACTIVITY_IN = R.drawable.stat_sys_signal_in;
    private final int DATA_ACTIVITY_OUT = R.drawable.stat_sys_signal_out;
    private final int DATA_ACTIVITY_INOUT = R.drawable.stat_sys_signal_inout;

    // Signal strength icons
    private final int SIGNAL_STRENGTH_NULL_ONE_SIM = R.drawable.stat_sys_signal_null_one_sim;
    private final int SIGNAL_STRENGTH_NULL_TWO_SIMS = R.drawable.stat_sys_signal_null_two_sims;
    private final int SIGNAL_STRENGTH_NULL_SIM1 = R.drawable.stat_sys_signal_null_sim1;
    private final int SIGNAL_STRENGTH_NULL_SIM2 = R.drawable.stat_sys_signal_null_sim2;

    // Data roaming indicator icon
    private final int DATA_ROAMING_INDICATOR = R.drawable.stat_sys_data_fully_connected_roam;

    private final int[][] SIGNAL_STRENGTH_SINGLE = {
        {
            R.drawable.stat_sys_signal_0_fully_sim1,
            R.drawable.stat_sys_signal_1_fully_sim1,
            R.drawable.stat_sys_signal_2_fully_sim1,
            R.drawable.stat_sys_signal_3_fully_sim1,
            R.drawable.stat_sys_signal_4_fully_sim1
        },
        {
            R.drawable.stat_sys_signal_0_fully_sim2,
            R.drawable.stat_sys_signal_1_fully_sim2,
            R.drawable.stat_sys_signal_2_fully_sim2,
            R.drawable.stat_sys_signal_3_fully_sim2,
            R.drawable.stat_sys_signal_4_fully_sim2
        }
    };

    /**
     * Constructs a new Op02SystemUIStatusBarExt instance with Context.
     * @param context The Context object
     */
    public Op02SystemUIStatusBarExt(Context context) {
        super(context);
        mContext = context;
        mCount = TelephonyManager.getDefault().getSimCount();
        if (DEBUG) {
            Log.d(TAG, "mCount=" + mCount);
        }
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

        mNetworkTypeIconId = getNetworkTypeIconId(networkType);
        if (DEBUG) {
            Log.d(TAG, "getNetworkTypeIcon(), subId=" + subId
                + ", networkType=" + networkType
                + ", iconId=" + mNetworkTypeIconId
                + ", serviceState=" + serviceState);
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
        int slotId = SubscriptionManager.getSlotId(subId);
        mSignalStrengthIconId = getSignalStrengthIconId(signalStrength, slotId);
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

        // Add the no sim view for another slot
        if (mCount == 2) {
            mNoSimView = new NoSimView(mContext);
            if (slotId == 0) {
                mNoSimView.mSlotId = 1;
                mNoSimView.mNoSimView.setImageDrawable(
                    mContext.getResources().getDrawable(
                        SIGNAL_STRENGTH_NULL_SIM2));
                root.addView(mNoSimView.mNoSimView);
            } else if (slotId == 1) {
                mNoSimView.mSlotId = 0;
                mNoSimView.mNoSimView.setImageDrawable(
                    mContext.getResources().getDrawable(
                        SIGNAL_STRENGTH_NULL_SIM1));
                root.addView(mNoSimView.mNoSimView, 0);
            }
        }

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

            if (isSimOffline(subId, mServiceState) || !isSimInsert(subId)) {
                networkTypeView.setVisibility(View.GONE);
                return;
            }

            int dataState = TelephonyManager.getDefault().getDataState(subId);
            if (networkTypeId != 0
                && dataState != TelephonyManager.DATA_CONNECTED) {
                // Disable the data view first
                if (mRoot != null) {
                    mDataTypeView.setVisibility(View.GONE);
                    mDataActivityView.setVisibility(View.GONE);
                }

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

            if (isSimOffline(subId, mServiceState) || !isSimInsert(subId)) {
                mDataTypeView.setVisibility(View.GONE);
                mDataActivityView.setVisibility(View.GONE);
                return;
            }

            int dataState = TelephonyManager.getDefault().getDataState(subId);

            if (dataTypeId != 0 && dataState == TelephonyManager.DATA_CONNECTED) {
                mDataTypeView.setImageDrawable(
                    mContext.getResources().getDrawable(dataTypeId));
                mDataTypeView.setVisibility(View.VISIBLE);

                int dataActivityId = getDataActivityIconId(dataIn, dataOut);
                mDataActivityView.setImageDrawable(
                    mContext.getResources().getDrawable(dataActivityId));
                mDataActivityView.setVisibility(View.VISIBLE);
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
            boolean isOffline = isSimOffline(subId, mServiceState);
            boolean isRoaming = isSimRoaming(subId, mServiceState);

            if (!isOffline && isRoaming) {
                mobileTypeView.setImageDrawable(
                    mContext.getResources().getDrawable(DATA_ROAMING_INDICATOR));
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
                int slotId = SubscriptionManager.getSlotId(subId);
                if (slotId == 0) {
                    signalStrengthId = SIGNAL_STRENGTH_NULL_SIM1;
                } else if (slotId == 1) {
                    signalStrengthId = SIGNAL_STRENGTH_NULL_SIM2;
                } else {
                    signalStrengthId = SIGNAL_STRENGTH_NULL_ONE_SIM;
                }
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

    @Override
    public void setCustomizedView(int subId) {
        if (mNoSimView != null) {
            // Show the no sim for other slot when needed
            if (SubscriptionManager.isValidSlotId(mNoSimView.mSlotId)) {
                boolean inserted = TelephonyManager.getDefault().hasIccCard(
                                        mNoSimView.mSlotId);

                if (!inserted) {
                    mNoSimView.mNoSimView.setVisibility(View.VISIBLE);

                    if (DEBUG) {
                        Log.d(TAG, "setCustomizedView(), subId=" + subId
                            + ", another SIM slotId=" + mNoSimView.mSlotId
                            + ", inserted=" + inserted);
                    }
                } else {
                    mNoSimView.mNoSimView.setVisibility(View.GONE);
                }
            } else {
                mNoSimView.mNoSimView.setVisibility(View.GONE);
            }
        }
    }

    @Override
    public void setCustomizedNoSimView(ImageView noSimView) {
        if (noSimView != null) {
            int iconId = 0;
            switch (mCount) {
                case 1:
                    iconId = SIGNAL_STRENGTH_NULL_ONE_SIM;
                    break;

                case 2:
                    iconId = SIGNAL_STRENGTH_NULL_TWO_SIMS;
                    break;

                default:
                    break;
            }

            if (iconId != 0) {
                noSimView.setImageDrawable(
                    mContext.getResources().getDrawable(iconId));
            }
        }
    }

    @Override
    public void setCustomizedAirplaneView(View noSimView, boolean airplaneMode) {
        if (DEBUG) {
            Log.d(TAG, "AirplaneView, airplaneMode=" + airplaneMode);
        }

        if ((noSimView != null) && airplaneMode) {
            noSimView.setVisibility(View.GONE);
        }
    }

    public class NoSimView {
        public int mSlotId;
        public ImageView mNoSimView;

        public NoSimView(Context context) {
            mSlotId = SubscriptionManager.INVALID_SIM_SLOT_INDEX;
            mNoSimView = new ImageView(mContext);
        }
    }

    public static final boolean isUserLoad() {
        return SystemProperties.get("ro.build.type").equals("user");
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
            isRoaming = serviceState.getRoaming();
        }

        if (DEBUG) {
            Log.d(TAG, "isSimRoaming: roaming=" + isRoaming
                + ", subId=" + subId
                + ", serviceState=" + serviceState);
        }

        return isRoaming;
    }

    private boolean isSimOffline(int subId, ServiceState serviceState) {
        boolean ret;

        boolean isEmergencyOnly =
            serviceState != null ? serviceState.isEmergencyOnly() : false;
        boolean isService = hasService(serviceState);
        int slotId = SubscriptionManager.getSlotId(subId);

        if (isEmergencyOnly || slotId < 0) {
            ret = true;
        } else {
            ret = !isService;
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
            case TelephonyManager.NETWORK_TYPE_GPRS:
            case TelephonyManager.NETWORK_TYPE_EDGE:
            case TelephonyManager.NETWORK_TYPE_CDMA:
            case TelephonyManager.NETWORK_TYPE_1xRTT:
                iconId = NETWORK_TYPE_G;
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
                iconId = NETWORK_TYPE_4G;
                break;

            default:
                break;
        }

        return iconId;
    }

    private int getDataTypeIconId(int dataType, int dataState) {
        int iconId = 0;

        if (dataState != TelephonyManager.DATA_CONNECTED) {
            return iconId;
        }

        switch (dataType) {
            case TelephonyManager.NETWORK_TYPE_UNKNOWN:
                break;

            case TelephonyManager.NETWORK_TYPE_GSM:
            case TelephonyManager.NETWORK_TYPE_GPRS:
            case TelephonyManager.NETWORK_TYPE_CDMA:
            case TelephonyManager.NETWORK_TYPE_1xRTT:
                iconId = DATA_TYPE_G;
                break;

            case TelephonyManager.NETWORK_TYPE_EDGE:
                iconId = DATA_TYPE_E;
                break;

            case TelephonyManager.NETWORK_TYPE_HSDPA:
            case TelephonyManager.NETWORK_TYPE_HSUPA:
            case TelephonyManager.NETWORK_TYPE_HSPA:
                iconId = DATA_TYPE_H;
                break;

            case TelephonyManager.NETWORK_TYPE_HSPAP:
                iconId = DATA_TYPE_H_PLUS;
                break;

            case TelephonyManager.NETWORK_TYPE_UMTS:
            case TelephonyManager.NETWORK_TYPE_EVDO_0:
            case TelephonyManager.NETWORK_TYPE_EVDO_A:
            case TelephonyManager.NETWORK_TYPE_EVDO_B:
            case TelephonyManager.NETWORK_TYPE_EHRPD:
                iconId = DATA_TYPE_3G;
                break;

            case TelephonyManager.NETWORK_TYPE_LTE:
                iconId = DATA_TYPE_4G;
                break;

            case TelephonyManager.NETWORK_TYPE_LTEA:
                iconId = DATA_TYPE_4G_PLUS;
                break;

            default:
                break;
        }

        return iconId;
    }

    private int getDataActivityIconId(boolean dataIn, boolean dataOut) {
        int iconId = DATA_ACTIVITY_NOT_INOUT;

        if (dataIn && dataOut) {
            iconId = DATA_ACTIVITY_INOUT;
        } else if (dataIn) {
            iconId = DATA_ACTIVITY_IN;
        } else if (dataOut) {
            iconId = DATA_ACTIVITY_OUT;
        }

        return iconId;
    }

    private int getSignalStrengthIconId(SignalStrength signalStrength, int slotId) {
        if (slotId < 0) {
            return SIGNAL_STRENGTH_NULL_ONE_SIM;
        }

        int level = (signalStrength != null ? signalStrength.getLevel() : 0);
        return SIGNAL_STRENGTH_SINGLE[slotId][level];
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

