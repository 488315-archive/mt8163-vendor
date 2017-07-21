/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2015. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

package com.mediatek.wfo.impl;

import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;

import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

import java.util.Set;
import java.util.concurrent.CopyOnWriteArraySet;

import android.content.BroadcastReceiver;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.database.ContentObserver;
import android.net.ConnectivityManager;
import android.net.INetworkManagementEventObserver;
import android.net.LinkAddress;
import android.net.LinkProperties;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkInfo;
import android.net.NetworkRequest;
import android.net.Uri;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Handler;
import android.os.IBinder;
import android.os.INetworkManagementService;
import android.os.Message;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.provider.Settings;

import android.telephony.PhoneStateListener;
import android.telephony.ServiceState;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.mediatek.internal.telephony.RadioCapabilitySwitchUtil;

import com.android.ims.ImsManager;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.RILConstants;
import com.android.server.net.BaseNetworkObserver;

import com.mediatek.wfo.DisconnectCause;
import com.mediatek.wfo.IWifiOffloadService;
import com.mediatek.wfo.IWifiOffloadListener;
import com.mediatek.wfo.WifiOffloadManager;

public class WifiOffloadService extends IWifiOffloadService.Stub {
    static {
        System.loadLibrary("wfo_jni");
    }

    private native void nativeInit();
    private native int nativeGetRatType();
    private native DisconnectCause nativeGetDisconnectCause();
    private native void nativeSetWosProfile(boolean volteEnabled, boolean wfcEnabled,
            String fqdn, boolean wifiEnabled, int wfcMode, int dataRoaming_enabled);
    private native void nativeSetWifiStatus(boolean wifiConnected, String ifNmae, String ipv4, String ipv6, String mac);
    private native void nativeSetCallState(boolean inCall, int callType);
    private native void nativeSetServiceState(
            int mdIdx, int simIdx, boolean isDataRoaming, int ratType, int serviceState);

    static final String TAG = "WifiOffloadService";

    private Set<IWifiOffloadListener> mListeners = new CopyOnWriteArraySet<IWifiOffloadListener>();

    private Context mContext;
    private WifiManager mWifiManager;
    private ConnectivityManager mConnectivityManager;
    private INetworkManagementService mNetworkManager;
    private SubscriptionManager mSubscriptionManager;
    private TelephonyManager mTelephonyManager;

    // from user settings
    private boolean mIsVolteEnabled;
    private boolean mIsWfcEnabled;
    private int mWfcMode;
    private boolean mIsWifiEnabled;

    // for VoWiFi Provisioning utilising SMS
    private String mFqdn = "";

    // wifi state
    private boolean mIsWifiConnected = false;
    private String mWifiApMac = "";
    private String mWifiIpv4Address = "";
    private String mWifiIpv6Address = "";
    private String mIfName ="";

    // call type
    private static final int IMSCALL_NONE = 0;
    private static final int IMSCALL_IR92_VOICE_CALL = 1;
    private static final int IMSCALL_IR94_VIDEO_CALL = 2;

    // data roaming, service states.
    private int mSimCount;
    private int mDataRoamingEnabled = SubscriptionManager.DATA_ROAMING_DISABLE;
    private boolean[] mIsCurDataRoaming;
    private int[] mRadioTechnology;

    // subId -> PhoneStateListener
    private Map<Integer, PhoneStateListener> mPhoneServicesStateListeners
            = new ConcurrentHashMap<Integer, PhoneStateListener>();

     /**
     * @see ServiceState
     * STATE_IN_SERVICE, STATE_OUT_OF_SERVICE, STATE_EMERGENCY_ONLY, STATE_POWER_OFF
     */
    private int[] mDataRegState;

    private WifiManager.WifiLock mWifiLock;

    private SettingsObserver mSettingsObserver = new SettingsObserver(null);

    private BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.d(TAG, "onReceive action:" + intent.getAction());
            if (intent.getAction().equals(WifiManager.WIFI_STATE_CHANGED_ACTION)) {
                if (mWifiManager == null) {
                    Log.d(TAG, "Unexpected error, mWifiManager is null!");
                    return;
                }

                boolean isWifiEnabled = mWifiManager.isWifiEnabled();
                if (isWifiEnabled != mIsWifiEnabled) {
                    mIsWifiEnabled = isWifiEnabled;
                    notifyMalUserProfile();
                }
            } else if (intent.getAction().equals(TelephonyManager.ACTION_PHONE_STATE_CHANGED)) {
                int phoneType = intent.getIntExtra(PhoneConstants.PHONE_TYPE_KEY,
                        RILConstants.NO_PHONE);
                if (phoneType != RILConstants.IMS_PHONE) {
                    Log.d(TAG, "not IMS phone");
                    return;
                }
                boolean inCall = false;
                int callType = IMSCALL_NONE;
                String state = intent.getStringExtra(TelephonyManager.EXTRA_STATE);
                if (TelephonyManager.EXTRA_STATE_OFFHOOK.equals(state)
                        || TelephonyManager.EXTRA_STATE_RINGING.equals(state)) {
                    inCall = true;
                    // TODO: detect voice or video call if MAL needs it
                    callType = IMSCALL_IR92_VOICE_CALL;
                }
                notifyCallState(inCall, callType);
            }
        }
    };

    private INetworkManagementEventObserver mNetworkObserver = new BaseNetworkObserver() {
        @Override
        public void addressUpdated(String iface, LinkAddress address) {
            Log.d(TAG, "mNetworkObserver.addressUpdated: iface=" + iface
                    + ", address = " + address);
            if (iface != null && iface.startsWith("wlan")
                    && address != null && address.getAddress() instanceof Inet6Address
                    && address.isGlobalPreferred()) {
                // At this timing, just refresh IPV6 address without changing 'mIsWifiConnected'.
                // mIsWifiConnected depends on ConnectivityManager.NetworkCallback.onAvailable.
                Message msg = mHandler.obtainMessage(
                        EVENT_WIFI_NETWORK_STATE_CHANGE,
                        mIsWifiConnected?1:0,  // isConnected
                        0, null);
                mHandler.sendMessage(msg);
            }
        }
    };

    private class SettingsObserver extends ContentObserver {
        private final Uri VOLTE_ENABLED_URI = Settings.Global
                .getUriFor(android.provider.Settings.Global.ENHANCED_4G_MODE_ENABLED);
        private final Uri WFC_ENABLED_URI = Settings.Global
                .getUriFor(android.provider.Settings.Global.WFC_IMS_ENABLED);
        private final Uri WFC_MODE_URI = Settings.Global
                .getUriFor(android.provider.Settings.Global.WFC_IMS_MODE);

        public SettingsObserver(Handler handler) {
            super(handler);
        }

        private void register() {
            ContentResolver resolver = mContext.getContentResolver();
            resolver.registerContentObserver(VOLTE_ENABLED_URI, false, this);
            resolver.registerContentObserver(WFC_ENABLED_URI, false, this);
            resolver.registerContentObserver(WFC_MODE_URI, false, this);
        }

        private void unregister() {
            ContentResolver resolver = mContext.getContentResolver();
            resolver.unregisterContentObserver(this);
        }

        @Override
        public void onChange(boolean selfChange) {
            onChange(selfChange, null);
        }

        @Override
        public void onChange(boolean selfChange, Uri uri) {
            if (VOLTE_ENABLED_URI.equals(uri)) {
                mIsVolteEnabled = ImsManager.isEnhanced4gLteModeSettingEnabledByUser(mContext);
            }
            if (WFC_ENABLED_URI.equals(uri)) {
                mIsWfcEnabled = ImsManager.isWfcEnabledByUser(mContext);
            }
            if (WFC_MODE_URI.equals(uri)) {
                mWfcMode = ImsManager.getWfcMode(mContext);
            }
            notifyMalUserProfile();
        }
    }

    private SubscriptionManager.OnSubscriptionsChangedListener mSubscriptionsChangedlistener =
            new SubscriptionManager.OnSubscriptionsChangedListener() {

            @Override
            public void onSubscriptionsChanged() {
                mHandler.sendMessage(mHandler.obtainMessage(EVENT_SUBSCRIPTION_INFO_CHANGED));
           }
    };

    // Message codes. See mHandler below.
    private static final int EVENT_WIFI_NETWORK_STATE_CHANGE = 1;
    private static final int EVENT_SUBSCRIPTION_INFO_CHANGED = 2;
    private static final int EVENT_NOTIFY_SERVICE_STATE_CHANGE = 3;

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case EVENT_WIFI_NETWORK_STATE_CHANGE:
                    updateWifiConnectedInfo(msg.arg1);
                    break;
                case EVENT_SUBSCRIPTION_INFO_CHANGED:
                    updateServiceStateListeners();
                    updateDataRoamingSetting();
                    break;
                case EVENT_NOTIFY_SERVICE_STATE_CHANGE:
                    updateServiceState(msg.arg1, (ServiceState)msg.obj);
                    break;
                default:
                    break;
            }
        }
    };

    public WifiOffloadService(Context context) {
        nativeInit();
        mContext = context;
        mWifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);
        mConnectivityManager = ConnectivityManager.from(mContext);
        IBinder b = ServiceManager.getService(Context.NETWORKMANAGEMENT_SERVICE);
        mNetworkManager = INetworkManagementService.Stub.asInterface(b);

        mSubscriptionManager = SubscriptionManager.from(mContext);
        mSubscriptionManager.addOnSubscriptionsChangedListener(mSubscriptionsChangedlistener);

        mTelephonyManager = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);

        mIsVolteEnabled = ImsManager.isEnhanced4gLteModeSettingEnabledByUser(mContext);
        mIsWfcEnabled = ImsManager.isWfcEnabledByUser(mContext);
        mWfcMode = ImsManager.getWfcMode(mContext);
        mIsWifiEnabled = mWifiManager.isWifiEnabled();
        mDataRoamingEnabled = getDataRoamingSetting();

        if (mTelephonyManager != null) {
            mSimCount = mTelephonyManager.getSimCount();
            mIsCurDataRoaming = new boolean[mSimCount];
            mRadioTechnology = new int[mSimCount];
            mDataRegState = new int[mSimCount];
        }

        if (mWifiManager != null) {
            mWifiLock = mWifiManager.createWifiLock("WifiOffloadService-Wifi Lock");
            if (mWifiLock != null) {
                mWifiLock.setReferenceCounted(false);
            }
        }

        // force update user profile to MAL at first time.
        notifyMalUserProfile();

        mSettingsObserver.register();
        registerForBroadcast();
        setupCallbacksForWifiStatus();
        updateServiceStateListeners();
        Log.d(TAG, "Initialize finish");
    }

    private void updateServiceStateListeners() {
        if (mSubscriptionManager == null) {
            Log.d(TAG, "Unexpected error, mSubscriptionManager=null");
            return;
        }
        if (mTelephonyManager == null) {
            Log.d(TAG, "Unexpected error, mTelephonyManager=null");
            return;
        }

        HashSet<Integer> unUsedSubscriptions =
                new HashSet<Integer>(mPhoneServicesStateListeners.keySet());
        final List<SubscriptionInfo> slist = mSubscriptionManager.getActiveSubscriptionInfoList();

        if (slist != null) {
            for (SubscriptionInfo subInfoRecord : slist) {
                int subId = subInfoRecord.getSubscriptionId();
                if (mPhoneServicesStateListeners.get(subId) == null) {
                    // Create listeners for new subscriptions.
                    Log.d(TAG, "create ServicesStateListener for " + subId);
                    PhoneStateListener listener = new PhoneStateListener(subId) {
                            @Override
                            public void onServiceStateChanged(ServiceState serviceState) {
                                if (serviceState == null) {
                                    Log.d(TAG, "onServiceStateChanged-" + this.mSubId
                                            + ": serviceState is null");
                                    return;
                                }
                                mHandler.sendMessage(
                                mHandler.obtainMessage(EVENT_NOTIFY_SERVICE_STATE_CHANGE,
                                        this.mSubId, 0, serviceState));
                            }
                    };

                    mTelephonyManager.listen(
                            listener, PhoneStateListener.LISTEN_SERVICE_STATE);
                    mPhoneServicesStateListeners.put(subId, listener);
                } else {
                    // this is still a valid subscription.
                    Log.d(TAG, "ServicesStateListener-" + subId + " is used.");
                    unUsedSubscriptions.remove(subId);
                }
            }
        }

        for (Integer key: unUsedSubscriptions) {
            Log.d(TAG, "remove unused ServicesStateListener for " + key);
            mTelephonyManager.listen(mPhoneServicesStateListeners.get(key), 0);
            mPhoneServicesStateListeners.remove(key);
        }
    }

    @Override
    public void registerForHandoverEvent(IWifiOffloadListener listener) {
        mListeners.add(listener);
    }

    @Override
    public void unregisterForHandoverEvent(IWifiOffloadListener listener) {
        mListeners.remove(listener);
    }

    @Override
    public int getRatType() {
        int result = nativeGetRatType();
        Log.d(TAG, "rat type is " + result);
        return result;
    }

    @Override
    public DisconnectCause getDisconnectCause() {
        DisconnectCause result = nativeGetDisconnectCause();
        Log.d(TAG, "disconnect cause is " + result);
        return result;
    }

    @Override
    public void setEpdgFqdn(String fqdn, boolean wfcEnabled) {
        if (fqdn == null) {
            Log.d(TAG, "fqdn is null");
            return;
        }
        mFqdn = fqdn;
        mIsWfcEnabled = wfcEnabled;
        notifyMalUserProfile();
    }

    private void registerForBroadcast() {
        IntentFilter filter = new IntentFilter();
        filter.addAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
        filter.addAction(TelephonyManager.ACTION_PHONE_STATE_CHANGED);
        mContext.registerReceiver(mReceiver, filter);
    }

    private void updateWifiConnectedInfo(int isConnected) {
        Log.d(TAG, "updateWifiConnectedInfo isConnected=" + isConnected);

        boolean changed = false;

        if (isConnected == 0) {
            if (mIsWifiConnected) {
                mIsWifiConnected = false;
                mWifiApMac = "";
                mWifiIpv4Address = "";
                mWifiIpv6Address = "";
                mIfName ="";
                changed = true;
            }
        } else {
            String wifiApMac = "", ipv4Address = "", ipv6Address = "", ifName = "";

            if (!mIsWifiConnected) {
                mIsWifiConnected = true;
                changed = true;
            }

            // get MAC address of the current access point
            WifiInfo wifiInfo = mWifiManager.getConnectionInfo();
            if (wifiInfo != null) {
                wifiApMac = wifiInfo.getBSSID();
                if (!mWifiApMac.equals(wifiApMac)) {
                    mWifiApMac = (wifiApMac == null ? "" : wifiApMac);
                    changed = true;
                }
            }

            // get ip
            for (Network nw : mConnectivityManager.getAllNetworks()) {
                LinkProperties prop = mConnectivityManager.getLinkProperties(nw);
                // MAL only care about wlan
                if (prop == null || prop.getInterfaceName() == null
                        || !prop.getInterfaceName().startsWith("wlan")) {
                    continue;
                }
                for (InetAddress address : prop.getAddresses()) {
                    if (address instanceof Inet4Address) {
                        ipv4Address = address.getHostAddress();
                    } else if (address instanceof Inet6Address && !address.isLinkLocalAddress()) {
                        // Filters out link-local address. If cannot find non-link-local address,
                        // pass empty string to MAL.
                        ipv6Address = address.getHostAddress();
                    }
                }
                // get interface name
                ifName = prop.getInterfaceName();
            }
            if (!mWifiIpv4Address.equals(ipv4Address)) {
                mWifiIpv4Address = (ipv4Address == null ? "" : ipv4Address);
                changed = true;
            }
            if (!mWifiIpv6Address.equals(ipv6Address)) {
                mWifiIpv6Address = (ipv6Address == null ? "" : ipv6Address);
                changed = true;
            }
            if (!mIfName.equals(ifName)) {
                mIfName = (ifName == null ? "" : ifName);
                changed = true;
            }
        }

        if (changed) {
            notifyMalWifiState();
        }
    }

    // Currently it only care about MD1's data roaming setting.
    private int getDataRoamingSetting() {
        if (mSubscriptionManager == null) {
            Log.d(TAG, "Unexpected error, mSubscriptionManager=null");
            return SubscriptionManager.DATA_ROAMING_DISABLE;
        }

        int mainCapabilityPhoneId = RadioCapabilitySwitchUtil.getMainCapabilityPhoneId();
        SubscriptionInfo subInfo = mSubscriptionManager.getActiveSubscriptionInfoForSimSlotIndex(
                mainCapabilityPhoneId);
        Log.d(TAG, "getDataRoamingSetting: mainCapabilityPhoneId=" + mainCapabilityPhoneId
                + " , subInfo=" + subInfo);

        if (subInfo != null) {
            return subInfo.getDataRoaming();
        } else {
            Log.d(TAG, "Cannot get subscription information for slot:" + mainCapabilityPhoneId);
            return SubscriptionManager.DATA_ROAMING_DISABLE;
        }
    }

    private void updateDataRoamingSetting() {
        int isDataRoamingEnabled = getDataRoamingSetting();
        if (isDataRoamingEnabled != mDataRoamingEnabled) {
            mDataRoamingEnabled = isDataRoamingEnabled;
            notifyMalUserProfile();
        }
    }

    private void updateServiceState(int subId, ServiceState state) {
        boolean isDataRoaming = state.getDataRoaming();
        int radioTechnology = state.getRilDataRadioTechnology();
        int dataRegState = state.getDataRegState();

        if (radioTechnology == ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN) {
            dataRegState = ServiceState.STATE_OUT_OF_SERVICE;
        }

        int simId = SubscriptionManager.getPhoneId(subId);
        int mainCapabilityPhoneId = RadioCapabilitySwitchUtil.getMainCapabilityPhoneId();
        int mdIdx = mainCapabilityPhoneId==simId?0:1;

        if (simId < 0 || simId >= mSimCount) {
            Log.d(TAG, "updateServiceState: sim-" + simId + " sub-" + subId
                    + ": invalid simId");
            return;
        }

        Log.d(TAG, "updateServiceState: sim-" + simId + " sub-" + subId
                + ", isDataRoaming=" + isDataRoaming
                + ", radioTechnology=" + radioTechnology
                + ", dataRegState=" + dataRegState);

        if (mIsCurDataRoaming[simId] != isDataRoaming
                || mRadioTechnology[simId] != radioTechnology
                || mDataRegState[simId] != dataRegState) {
            mIsCurDataRoaming[simId] = isDataRoaming;
            mRadioTechnology[simId] = radioTechnology;
            mDataRegState[simId] = dataRegState;
            notifyMalServiceState(simId, subId);
        }
    }

    private void notifyMalUserProfile() {
        Log.d(TAG, "notifyMalUserProfile mIsVolteEnabled: " + mIsVolteEnabled + " mIsWfcEnabled: "
                + mIsWfcEnabled + " mFqdn: " + mFqdn + " mIsWifiEnabled: " + mIsWifiEnabled
                + " mWfcMode: " + mWfcMode + " mDataRoamingEnabled: " + mDataRoamingEnabled);
        nativeSetWosProfile(mIsVolteEnabled, mIsWfcEnabled, mFqdn,
                mIsWifiEnabled, mWfcMode, mDataRoamingEnabled);
    }

    private void notifyMalWifiState() {
        Log.d(TAG, "notifyMalWifiState mIsWifiConnected: " + mIsWifiConnected + " mIfaceName: "
                + mIfName + " mWifiIpv4Address: " + mWifiIpv4Address + " mWifiIpv6Address: "
                + mWifiIpv6Address + " mWifiApMac: " + mWifiApMac);
        nativeSetWifiStatus(mIsWifiConnected, mIfName, mWifiIpv4Address, mWifiIpv6Address,
                mWifiApMac);
    }

    private void notifyCallState(boolean inCall, int callType) {
        Log.d(TAG, "notifyCallState inCall: " + inCall + " callType: " + callType);
        nativeSetCallState(inCall, callType);
    }

    private void notifyMalServiceState(int simIdx, int mdIdx) {
        Log.d(TAG, "nativeSetServiceState simIdx: " + simIdx
                + "mdIdx: " + mdIdx
                + "mIsCurDataRoaming: " + mIsCurDataRoaming[simIdx]
                + ", mRadioTechnology: " + mRadioTechnology[simIdx]
                + ", mDataRegState: " + mDataRegState[simIdx]);
        nativeSetServiceState(mdIdx, simIdx, mIsCurDataRoaming[simIdx],
                mRadioTechnology[simIdx], mDataRegState[simIdx]);
    }

    /**
     * callback from MAL when IMS PDN handover.
     * @param stage handover start/end
     * @param ratType current rat type
     */
    private void onHandover(int stage, int ratType) {
        for (IWifiOffloadListener listener : mListeners) {
            if (listener != null) {
                try {
                    listener.onHandover(stage, ratType);
                } catch (RemoteException e) {
                    Log.e(TAG, "onHandover: RemoteException occurs!");
                }
            }
        }
    }

    /**
     * callback from MAL when rove out condition meets.
     * @param roveOut is rove out or not
     * @param rssi current WiFi rssi
     */
    private void onRoveOut(boolean roveOut, int rssi) {
        Log.d(TAG, "onRoveOut: roveOut is " + roveOut + " rssi " + rssi);
        for (IWifiOffloadListener listener : mListeners) {
            if (listener != null) {
                try {
                    listener.onRoveOut(roveOut, rssi);
                } catch (RemoteException e) {
                    Log.e(TAG, "onRoveOut: RemoteException occurs!");
                }
            }
        }
        // temporary solution for testing
        if (roveOut) {
            Log.d(TAG, "onRoveOut: sendBroadcast");
            mContext.sendBroadcast(new Intent(WifiOffloadManager.ACTION_ROVE_OUT_ALERT));
        }
    }

    /**
     * callback from MAL when IMS PDN is lost
     */
    private void onLost() {
        // TODO: broadcast
    }

    /**
     * callback from MAL when IMS PDN is unavailable
     */
    private void onUnavailable() {
        // TODO: broadcast
    }

    /**
     * callback from MAL when PDN over ePDG is active.
     * @param active is PDN over ePDG active or in-active.
     */
    private void onPdnStateChanged(boolean active) {
        Log.d(TAG, "onPdnStateChanged: active=" + active);
        if (mWifiLock == null) {
            Log.d(TAG, "Unexpected error, mWifiLock is null");
        }

        if (active) {
            mWifiLock.acquire();
        } else {
            mWifiLock.release();
        }
    }

    /**
     * setup callbacks from ConnectivityService when WiFi is changed.
     */
    private void setupCallbacksForWifiStatus() {
        if (mConnectivityManager == null) {
            Log.d(TAG, "Unexpected error, mConnectivityManager=null");
            return;
        }

        if (mNetworkManager == null) {
            Log.d(TAG, "Unexpected error, mNetworkManager=null");
            return;
        }


        NetworkRequest request = new NetworkRequest.Builder()
                .addTransportType(NetworkCapabilities.TRANSPORT_WIFI)
                .build();
        mConnectivityManager.registerNetworkCallback(request,
                new ConnectivityManager.NetworkCallback() {
                    /**
                     * @param network
                     */
                    @Override
                    public void onAvailable(Network network) {
                        Log.d(TAG, "NetworkCallback.onAvailable, network=" + network);

                        Message msg = mHandler.obtainMessage(
                                EVENT_WIFI_NETWORK_STATE_CHANGE,
                                1,  // isConnected
                                0, null);
                        mHandler.sendMessage(msg);
                    }

                    /**
                     * @param network
                     */
                    @Override
                    public void onLost(Network network) {
                        Log.d(TAG, "NetworkCallback.onLost, network=" + network);

                        Message msg = mHandler.obtainMessage(
                                EVENT_WIFI_NETWORK_STATE_CHANGE,
                                0,  // isConnected
                                0, null);
                        mHandler.sendMessage(msg);
                    }
                });

        try {
            mNetworkManager.registerObserver(mNetworkObserver);
        } catch (Exception e) {
            Log.d(TAG, "mNetworkManager.registerObserver failed: " + e);
        }
    }
}
