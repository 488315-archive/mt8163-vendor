package com.mediatek.systemui.plugin;

import java.util.HashMap;

import android.content.Context;
import android.os.Bundle;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.SystemProperties;
import android.provider.Settings;
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

import com.android.internal.telephony.cdma.EriInfo;
import com.android.internal.telephony.ITelephony;
import com.android.internal.telephony.Phone;

import com.mediatek.common.PluginImpl;
import com.mediatek.internal.telephony.ITelephonyEx;
import com.mediatek.op09.plugin.R;
import com.mediatek.systemui.ext.DefaultSystemUIStatusBarExt;
import com.mediatek.telephony.TelephonyManagerEx;

/**
 * M: Op09 implementation of Plug-in definition of Status bar.
 */
@PluginImpl(interfaceName="com.mediatek.systemui.ext.ISystemUIStatusBarExt")
public class Op09SystemUIStatusBarExt extends DefaultSystemUIStatusBarExt {

    private static final String TAG = "Op09SystemUIStatusBarExt";
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
    private boolean mIsRoaming;

    private static HashMap<Integer, Boolean> sIsRoamingMap =
            new HashMap<Integer, Boolean>();

    // Network type icons
    private final int NETWORK_TYPE_2G = R.drawable.stat_sys_network_type_2g;
    private final int NETWORK_TYPE_3G = R.drawable.stat_sys_network_type_3g;
    private final int NETWORK_TYPE_4G = R.drawable.stat_sys_network_type_4g;
    private final int NETWORK_TYPE_3G_2G = R.drawable.stat_sys_network_type_3g_2g;
    private final int NETWORK_TYPE_4G_2G = R.drawable.stat_sys_network_type_4g_2g;
    private final int NETWORK_TYPE_ONLY_4G = R.drawable.stat_sys_network_type_4g_data_only;
    // CA update
    private final int NETWORK_TYPE_4G_PLUS = R.drawable.stat_sys_network_type_4g_plus;
    private final int NETWORK_TYPE_4G_PLUS_2G = R.drawable.stat_sys_network_type_4gplus_2g;
    private final int NETWORK_TYPE_ONLY_4G_PLUS =
            R.drawable.stat_sys_network_type_4g_plus_data_only;

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

    // Signal strength icons
    private final int SIGNAL_STRENGTH_NULL_ONE_SIM = R.drawable.stat_sys_signal_null;
    private final int SIGNAL_STRENGTH_NULL_TWO_SIMS = R.drawable.stat_sys_signal_null_two_sims;
    private final int SIGNAL_STRENGTH_OFFLINE = R.drawable.stat_sys_signal_radio_off;

    // Data roaming indicator icon
    private final int DATA_ROAMING_INDICATOR = R.drawable.stat_sys_data_fully_connected_roam;

    private final int[] SIGNAL_STRENGTH_SINGLE = {
        R.drawable.stat_sys_signal_1_fully, //R.drawable.stat_sys_signal_0_fully,
        R.drawable.stat_sys_signal_1_fully,
        R.drawable.stat_sys_signal_2_fully,
        R.drawable.stat_sys_signal_3_fully,
        R.drawable.stat_sys_signal_4_fully
    };

    private final int[][] SIGNAL_STRENGTH_TOWER = {
        {
            R.drawable.stat_sys_signal_1_1_fully, //R.drawable.stat_sys_signal_0_0_fully,
            R.drawable.stat_sys_signal_1_1_fully, //R.drawable.stat_sys_signal_0_1_fully,
            R.drawable.stat_sys_signal_1_2_fully, //R.drawable.stat_sys_signal_0_2_fully,
            R.drawable.stat_sys_signal_1_3_fully, //R.drawable.stat_sys_signal_0_3_fully,
            R.drawable.stat_sys_signal_1_4_fully  //R.drawable.stat_sys_signal_0_4_fully
        },
        {
            R.drawable.stat_sys_signal_1_1_fully, //R.drawable.stat_sys_signal_1_0_fully,
            R.drawable.stat_sys_signal_1_1_fully,
            R.drawable.stat_sys_signal_1_2_fully,
            R.drawable.stat_sys_signal_1_3_fully,
            R.drawable.stat_sys_signal_1_4_fully
        },
        {
            R.drawable.stat_sys_signal_2_1_fully, //R.drawable.stat_sys_signal_2_0_fully,
            R.drawable.stat_sys_signal_2_1_fully,
            R.drawable.stat_sys_signal_2_2_fully,
            R.drawable.stat_sys_signal_2_3_fully,
            R.drawable.stat_sys_signal_2_4_fully
        },
        {
            R.drawable.stat_sys_signal_3_1_fully, //R.drawable.stat_sys_signal_3_0_fully,
            R.drawable.stat_sys_signal_3_1_fully,
            R.drawable.stat_sys_signal_3_2_fully,
            R.drawable.stat_sys_signal_3_3_fully,
            R.drawable.stat_sys_signal_3_4_fully
        },
        {
            R.drawable.stat_sys_signal_4_1_fully, //R.drawable.stat_sys_signal_4_0_fully,
            R.drawable.stat_sys_signal_4_1_fully,
            R.drawable.stat_sys_signal_4_2_fully,
            R.drawable.stat_sys_signal_4_3_fully,
            R.drawable.stat_sys_signal_4_4_fully
        }
    };

    /**
     * Constructs a new Op09SystemUIStatusBarExt instance with Context.
     * @param context The Context object
     */
    public Op09SystemUIStatusBarExt(Context context) {
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

        if (isSimOffline(subId, networkType, serviceState) ||
                !hasService(serviceState)) {
            mNetworkTypeIconId = 0;
        } else if (isShow4GDataOnly(subId, networkType, serviceState)) {
            if (isCASupport()) {
                mNetworkTypeIconId = NETWORK_TYPE_ONLY_4G_PLUS;
            } else {
                mNetworkTypeIconId = NETWORK_TYPE_ONLY_4G;
            }
        } else {
            mNetworkTypeIconId =
                getNetworkTypeIconId(networkType, mIsRoaming);
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
        boolean showTower = false;
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

        if (isSimOffline(subId, networkType, serviceState) ||
                !hasService(serviceState)) {
            mSignalStrengthIconId = SIGNAL_STRENGTH_OFFLINE;
        } else {
            boolean is4GDataOnly =
                isShow4GDataOnly(subId, networkType, serviceState);
            mIsRoaming  = isRoaming(serviceState, signalStrength);
            updateRoamingStatusMap(subId, mIsRoaming);

            if (!is4GDataOnly
                && !mIsRoaming
                && (networkType == TelephonyManager.NETWORK_TYPE_EVDO_0
                    || networkType == TelephonyManager.NETWORK_TYPE_EVDO_A
                    || networkType == TelephonyManager.NETWORK_TYPE_EVDO_B
                    || networkType == TelephonyManager.NETWORK_TYPE_EHRPD
                    || networkType == TelephonyManager.NETWORK_TYPE_LTE)) {
                showTower = true;
            }

            mSignalStrengthIconId = getSignalStrengthIconId(
                signalStrength, showTower, networkType);
        }

        if (DEBUG) {
            Log.d(TAG, "getCustomizeSignalStrengthIcon(), subId=" + subId
                + ", networkType=" + networkType
                + ", showTower=" + showTower
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
            mNoSimView.mNoSimView.setImageDrawable(
                mContext.getResources().getDrawable(
                    SIGNAL_STRENGTH_NULL_ONE_SIM));
            if (slotId == 0) {
                mNoSimView.mSlotId = 1;
                root.addView(mNoSimView.mNoSimView);
            } else if (slotId == 1) {
                mNoSimView.mSlotId = 0;
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

            if (isSimOfflineBySub(subId, mServiceState) || !isSimInsert(subId) ||
                    !hasService(mServiceState)) {
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

            if (isSimOfflineBySub(subId, mServiceState) || !isSimInsert(subId) ||
                    !hasService(mServiceState)) {
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

            boolean isOffline = isSimOffline(subId,
                TelephonyManager.getDefault().getNetworkType(subId),
                mServiceState);
            boolean isRoaming = getRoamingStatusByMap(subId);

            if (!isOffline && isRoaming && hasService(mServiceState)) {
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
            if (isSimOfflineBySub(subId, mServiceState) || !hasService(mServiceState)) {
                signalStrengthId = SIGNAL_STRENGTH_OFFLINE;
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

    private boolean is4GDataOnlyMode(int subId) {
        int mode = Settings.Global.getInt(
                mContext.getContentResolver(),
                Settings.Global.LTE_ON_CDMA_RAT_MODE,
                Phone.NT_MODE_LTE_CDMA_AND_EVDO);

        if (DEBUG) {
            Log.d(TAG, "is4GDataOnlyMode(), mode=" + mode
                + "4gDataOnly=" + (mode == Phone.NT_MODE_LTE_TDD_ONLY));
        }

        return mode == Phone.NT_MODE_LTE_TDD_ONLY;
    }

    private boolean isShow4GDataOnly(int subId, int networkType,
                        ServiceState serviceState) {
        boolean dataOnly = false;

        if ((networkType == TelephonyManager.NETWORK_TYPE_LTE
             || networkType == TelephonyManager.NETWORK_TYPE_LTEA)
            && serviceState != null) {
            if (serviceState.getVoiceRegState() != ServiceState.STATE_IN_SERVICE) {
                if (serviceState.getDataRegState() == ServiceState.STATE_IN_SERVICE
                    || is4GDataOnlyMode(subId)) {
                    dataOnly = true;
                }
            }
        }

        if (DEBUG) {
            Log.d(TAG, "isShow4GDataOnly: dataOnly=" + dataOnly
                + ", subId=" + subId
                + ", networkType=" + networkType
                + ", serviceState=" + serviceState);
        }

        return dataOnly;
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

    private boolean isSimRoaming(int subId, ServiceState serviceState) {
        boolean isRoaming = false;

        if (serviceState != null) {
            isRoaming = !TelephonyManagerEx.getDefault().isInHomeNetwork(subId);
        }

        if (DEBUG) {
            Log.d(TAG, "isSimRoaming: roaming=" + isRoaming
                + ", subId=" + subId
                + ", serviceState=" + serviceState);
        }

        return isRoaming;
    }

    private boolean isRoaming(ServiceState serviceState, SignalStrength signalStrength) {
        if ((signalStrength != null) && !signalStrength.isGsm()) {
            final int iconMode = serviceState.getCdmaEriIconMode();
            return serviceState.getCdmaEriIconIndex() != EriInfo.ROAMING_INDICATOR_OFF
                    && (iconMode == EriInfo.ROAMING_ICON_MODE_NORMAL
                        || iconMode == EriInfo.ROAMING_ICON_MODE_FLASH);
        } else {
            return serviceState != null && serviceState.getRoaming();
        }
    }

    private static void updateRoamingStatusMap(int subId, boolean isRoaming) {
        if (!sIsRoamingMap.containsKey(subId) || isRoaming != sIsRoamingMap.get(subId)) {
            sIsRoamingMap.put(subId, isRoaming);
        }
    }

    private static boolean getRoamingStatusByMap(int subId) {
        if (sIsRoamingMap.containsKey(subId)) {
            return sIsRoamingMap.get(subId);
        } else {
            return false;
        }
    }

    private boolean isSimOffline(int subId, int networkType,
                        ServiceState serviceState) {
        boolean ret;

        boolean is4GDataOnly =
            isShow4GDataOnly(subId, networkType, serviceState);
        boolean isEmergencyOnly =
            serviceState != null ? serviceState.isEmergencyOnly() : false;
        boolean isRadioOn = isSimRadioOn(subId);

        if (isEmergencyOnly) {
            if (is4GDataOnly) {
                ret = false;
            } else {
                ret = true;
            }
        } else {
            ret = !(isRadioOn || (serviceState != null ?
                serviceState.getDataRegState() != ServiceState.STATE_POWER_OFF
                : false));
        }

        if (DEBUG) {
            Log.d(TAG, "isSimOffline: offline=" + ret
                + ", subId=" + subId
                + ", networkType=" + networkType
                + ", serviceState=" + serviceState);
        }

        return ret;
    }

    private boolean isSimOfflineBySub(int subId, ServiceState serviceState) {
        int networkType = TelephonyManager.getDefault().getNetworkType(subId);
        return isSimOffline(subId, networkType, serviceState);
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

    private int getNetworkTypeIconId(int networkType, boolean isRoaming) {
        int iconId = 0;

        switch (networkType) {
            case TelephonyManager.NETWORK_TYPE_UNKNOWN:
                break;

            case TelephonyManager.NETWORK_TYPE_GSM:
            case TelephonyManager.NETWORK_TYPE_GPRS:
            case TelephonyManager.NETWORK_TYPE_EDGE:
            case TelephonyManager.NETWORK_TYPE_CDMA:
            case TelephonyManager.NETWORK_TYPE_1xRTT:
                iconId = NETWORK_TYPE_2G;
                break;

            case TelephonyManager.NETWORK_TYPE_HSDPA:
            case TelephonyManager.NETWORK_TYPE_HSUPA:
            case TelephonyManager.NETWORK_TYPE_HSPA:
            case TelephonyManager.NETWORK_TYPE_HSPAP:
            case TelephonyManager.NETWORK_TYPE_UMTS:
                iconId = NETWORK_TYPE_3G;
                break;

            case TelephonyManager.NETWORK_TYPE_EVDO_0:
            case TelephonyManager.NETWORK_TYPE_EVDO_A:
            case TelephonyManager.NETWORK_TYPE_EVDO_B:
            case TelephonyManager.NETWORK_TYPE_EHRPD:
                if (isRoaming) {
                    iconId = NETWORK_TYPE_3G;
                } else {
                    iconId = NETWORK_TYPE_3G_2G;
                }
                break;

            case TelephonyManager.NETWORK_TYPE_LTE:
                if (isRoaming) {
                    if (isCASupport()) {
                        iconId = NETWORK_TYPE_4G_PLUS;
                    } else {
                        iconId = NETWORK_TYPE_4G;
                    }
                } else {
                    if (isCASupport()) {
                        iconId = NETWORK_TYPE_4G_PLUS_2G;
                    } else {
                        iconId = NETWORK_TYPE_4G_2G;
                    }
                }
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

    private int getSignalStrengthIconId(SignalStrength signalStrength,
                    boolean showTower, int networkType) {
        int level1 = 0;
        int level2 = 0;

        if (signalStrength != null) {
            switch (networkType) {
                case TelephonyManager.NETWORK_TYPE_UNKNOWN:
                case TelephonyManager.NETWORK_TYPE_GSM:
                case TelephonyManager.NETWORK_TYPE_GPRS:
                case TelephonyManager.NETWORK_TYPE_EDGE:
                    level1 = signalStrength.getLevel();
                    break;

                case TelephonyManager.NETWORK_TYPE_CDMA:
                case TelephonyManager.NETWORK_TYPE_1xRTT:
                    level1 = signalStrength.getCdmaLevel();
                    break;

                case TelephonyManager.NETWORK_TYPE_HSDPA:
                case TelephonyManager.NETWORK_TYPE_HSUPA:
                case TelephonyManager.NETWORK_TYPE_HSPA:
                case TelephonyManager.NETWORK_TYPE_HSPAP:
                case TelephonyManager.NETWORK_TYPE_UMTS:
                    level1 = signalStrength.getLevel();
                    break;

                case TelephonyManager.NETWORK_TYPE_EVDO_0:
                case TelephonyManager.NETWORK_TYPE_EVDO_A:
                case TelephonyManager.NETWORK_TYPE_EVDO_B:
                case TelephonyManager.NETWORK_TYPE_EHRPD:
                    level1 = signalStrength.getEvdoLevel();
                    level2 = signalStrength.getCdmaLevel();
                    break;

                case TelephonyManager.NETWORK_TYPE_LTE:
                    level1 = signalStrength.getLteLevel();
                    level2 = signalStrength.getCdmaLevel();
                    break;

                default:
                    break;
            }
        }

        if (showTower) {
            return SIGNAL_STRENGTH_TOWER[level1][level2];
        } else {
            return SIGNAL_STRENGTH_SINGLE[level1];
        }
    }

    private int getNetworkTypeFromServiceState(
                    int networkType, ServiceState serviceState) {
        int type = networkType;
        if (serviceState != null) {
            type = serviceState.getDataRegState() == ServiceState.STATE_IN_SERVICE &&
                    serviceState.getDataNetworkType() != TelephonyManager.NETWORK_TYPE_UNKNOWN
                ? serviceState.getDataNetworkType()
                : serviceState.getVoiceNetworkType();
        }
        return type;
    }

}
