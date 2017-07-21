/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2014. All rights reserved.
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

package com.mediatek.selfregister;

import android.annotation.SuppressLint;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.ConnectivityManager;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.NetworkInfo;
import android.net.NetworkRequest;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.telephony.CellLocation;
import android.telephony.PhoneStateListener;
import android.telephony.ServiceState;
import android.telephony.cdma.CdmaCellLocation;
import android.telephony.gsm.GsmCellLocation;
import android.util.Log;

import com.mediatek.selfregister.utils.AgentProxy;
import com.mediatek.selfregister.utils.PlatformManager;
import com.mediatek.selfregister.utils.Utils;

import org.json.JSONObject;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Service which process the main logic of registration.
 */
public class RegisterService extends Service {

    public static final String TAG = Const.TAG_PREFIX + "RegisterService";

    // M: Message for handler.
    private static final int MSG_HANDLE_REGISTER = 0;
    private static final int MSG_HANDLE_RESULT = 1;
    private static final int MSG_HANDLE_NETWORK = 2;
    private static final int MSG_NO_STATE_CHANGE = 3;

    private static final int TIMES_MAX_RETRY = 10;

    // Key for unique preference
    private static final String PRE_KEY_RETRY_TIMES = "pref_key_retry_times";

    // M: Key for both shared preference and map
    private static final String KEY_SI = "key_system_id";
    private static final String KEY_NI = "key_network_id";
    private static final String KEY_BASEID = "key_base_station_id";

    // M: Key only for mPhoneValues.
    private static final String KEY_ROAMING = "key_roaming";

    private int[] mSlotList;
    private String[] mIccIdList;
    private PhoneStateListener[] mPhoneStateListener;
    private List<Map<String, Object>> mPhoneValueList;

    private Boolean[] mIsRoaming;
    private Boolean[] mIsSlotForCT;
    private Boolean mIsNetworkAvailable = false;
    private Boolean mIsMobileLink = false;
    private Boolean mHasBootAlready = false;

    private Context mContext;
    private PlatformManager mPlatformManager;
    private ConnectivityManager mConnectivityManager;
    private ConnectivityManager.NetworkCallback mNetworkCallback = null;

    private int mSlotDatalink = Const.SLOT_ID_INVALID;
    private int mSlotMaster = Const.SLOT_ID_INVALID;
    private int mTimesWait = 0;

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(TAG, "onCreate()");

        mContext = this;

        mPlatformManager = new PlatformManager(mContext);
        PlatformManager.stayForeground(this);

        if (mPlatformManager.isSingleLoad()) {
            mSlotList = Const.SINGLE_SIM_SLOT;
        } else {
            mSlotList = Const.DUAL_SIM_SLOT_LIST;
        }

        mPhoneValueList = new ArrayList<Map<String, Object>>();
        mPhoneStateListener = new PhoneStateListener[mSlotList.length];
        mIccIdList = new String[mSlotList.length];
        mIsRoaming = new Boolean[mSlotList.length];
        mIsSlotForCT = new Boolean[mSlotList.length];

        for (int i = 0; i < mSlotList.length; ++i) {
            mIsRoaming[i] = false;
            mIccIdList[i] = Const.ICCID_DEFAULT_VALUE;
            mPhoneStateListener[i] = null;

            Map<String, Object> value = new HashMap<String, Object>();
            mPhoneValueList.add(value);
        }

        mConnectivityManager = (ConnectivityManager) getSystemService(Context.CONNECTIVITY_SERVICE);
        Log.i(TAG, "mConnectivityManager is " + mConnectivityManager);
        if (mConnectivityManager == null) {
            throw new Error("ConnectivityManager is null.");
        }
    }

    @Override
    public IBinder onBind(Intent arg0) {
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.i(TAG, "onStartCommand() " + intent);

        if (intent == null) {
            Log.e(TAG, "Intent is null!");
            stopSelf();
            return START_STICKY;
        }

        String action = intent.getAction();
        if (action.equalsIgnoreCase(Const.ACTION_BOOT_COMPLETED)) {
            doAfterBoot();

        } else if (action.equalsIgnoreCase(Const.ACTION_RETRY)) {
            doAfterRetry();

        } else if (action.equalsIgnoreCase(Const.ACTION_PRE_BOOT_COMPLETED)) {
            doAfterPreBoot();

        } else if (action.equalsIgnoreCase(Const.ACTION_SIM_STATE_CHANGED)) {
            if (mHasBootAlready) {

                // remove MSG_NO_STATE_CHANGE message
                if (mHandler.hasMessages(MSG_NO_STATE_CHANGE)) {
                    mHandler.removeMessages(MSG_NO_STATE_CHANGE);
                }

                if (!mPlatformManager.hasCardInDevice(mSlotList)) {
                    Log.i(TAG, "No SIM card, stop");
                    stopSelf();
                } else {
                    registerPhoneListeners();
                }
            } else {
                Log.i(TAG, "Receive sim state change update before boot, do nothing");
            }

        } else {
            Log.i(TAG, "Action is invalid!");
        }

        return START_STICKY;
    }

    public void onDestroy() {
        Log.i(TAG, "onDestroy()");

        PlatformManager.leaveForeground(this);
        clearMessageInHandleQueue();

        for (int i = 0; i < mPhoneStateListener.length; ++i) {
            if (mPhoneStateListener[i] != null) {
                mPlatformManager.unRegisterPhoneListener(mPhoneStateListener[i]);
                mPhoneStateListener[i] = null;
            }
        }
        unregisterNetworkCallback();
        super.onDestroy();
    }

    //------------------------------------------------------
    // Handler init and related func
    //------------------------------------------------------

    private void doAfterBoot() {
        Log.i(TAG, "doAfterBoot");
        mHasBootAlready = true;

        /// M: Register callback to monitor network.
        /// the network maybe unstable after boot up, wait a while.
        mHandler.sendEmptyMessageDelayed(MSG_HANDLE_NETWORK, Const.ONE_SECOND * 10);

        // If no sim (or no change of sim/network), quit after 4 mins
        Log.i(TAG, "Send MSG_NO_STATE_CHANGE after 4 mins");
        mHandler.sendEmptyMessageDelayed(MSG_NO_STATE_CHANGE, Const.ONE_SECOND * 240);
    }

    private void doAfterRetry() {
        Log.i(TAG, "doAfterRetry");

        if (!mPlatformManager.hasCardInDevice(mSlotList)) {
            Log.i(TAG, "No SIM card, stop");
            stopSelf();
        }

        registerPhoneListeners();
        mHandler.sendEmptyMessageDelayed(MSG_HANDLE_NETWORK, Const.ONE_SECOND * 10);

        // If sim state & network all not ready, won't try to register or set the alarm. So always
        // send a delay message here.
        sendDelayHandlerMessage();
    }


    /*
     * Whether this is preboot due to factory reset or system udpate, both clear register flag.
     */
    private void doAfterPreBoot() {
        AgentProxy.getInstance().setSelfRegisterFlag(false);
    }

    private void registerPhoneListeners() {
        Log.i(TAG, "registerPhoneListeners");

        for (int i = 0; i < mSlotList.length; ++i) {

            if (mPhoneStateListener[i] != null) {
                Log.i(TAG, "Slot " + i + " already registered.");
                continue;
            }

            int slotId = i;
            int[] subId = PlatformManager.getSubId(slotId);

            if (subId != null && subId[0] > 0) {
                Log.i(TAG, "subId[0] is " + subId[0] + ", register listener for slot " + slotId);

                mPhoneStateListener[i] = new CustomizedPhoneStateListener(i, subId[0]);
                mPlatformManager.registerPhoneListener(mPhoneStateListener[i]);
            } else {
                Log.i(TAG, "No need to register listener for slot " + slotId);
            }
        }
    }

    //------------------------------------------------------
    // Handler init and related func
    //------------------------------------------------------

    private void handleRegisterMsg() {
        if (!needRegister()) {
            Log.i(TAG, "No need to register, stop");
            stopSelf();
            return;
        }

        mSlotMaster = getMasterSlot();
        if (mIsNetworkAvailable && isNetworkTypeValid()
                && mPlatformManager.isImsiInfoReady(mSlotMaster)) {
            composeAndSend();

        } else {
            Log.i(TAG, "Network/IMSI info not ready");
            mTimesWait++;

            if (mTimesWait <= 3) {
                int second = mTimesWait * 10;
                Log.i(TAG, "Times " + mTimesWait + ", wait " + second + "s and check");
                sendDelayHandlerMessage(second);

            } else {
                clearMessageInHandleQueue();
                setRetryAlarm();
                stopSelf();
            }
        }
    }

    private void composeAndSend() {
        Log.i(TAG, "composeAndSend");
        RegisterMessage registerMessage = new RegisterMessage(this, mSlotMaster);
        final String message = registerMessage.prepareContent();
        new Thread(new Runnable() {

            @Override
            public void run() {
                JSONObject response = Utils.httpSend(message);

                Message msg = mHandler.obtainMessage(MSG_HANDLE_RESULT, response);
                mHandler.sendMessage(msg);
            }

        }).start();
    }

    private void handleResponseMsg(Message msg) {
        boolean result = Utils.checkRegisterResult((JSONObject) msg.obj);

        if (result) {
            Log.i(TAG, "analyseResponse(), resultCode:0 resultDesc:Success");

            AgentProxy.getInstance().setSavedIccId(mIccIdList);
            AgentProxy.getInstance().setSelfRegisterFlag(true);
            PlatformManager.createIndicator(mContext);
            stopSelf();

        } else {
            Log.e(TAG, "Register fail!");

            clearMessageInHandleQueue();
            setRetryAlarm();
            stopSelf();
        }
    }

    private void handleNetworkMsg() {
        registerNetworkCallback();
    }

    private void handleNoStateChange() {
        Log.v(TAG, "[handleNoStateChange] sim/network state never change, stopSelf.");
        stopSelf();
    }

    private boolean needRegister() {
        Log.v(TAG, "Enter needRegister()...");

        mIccIdList = mPlatformManager.getIccIDFromCard(mSlotList);
        for (int i = 0; i < mSlotList.length; ++i) {
            mIsSlotForCT[i] = isIccCardForCT(mSlotList[i]);
        }

        mPlatformManager.showSimInfo(this, mSlotList);

        if (!PlatformManager.supportCTForAllSlots()) {
            // 1. If it's single load and not CT card, no need
            if (mPlatformManager.isSingleLoad() && !mIsSlotForCT[0]) {
                Log.i(TAG, "No CT card, no need");
                return false;
            }
        }

        // 2. If anyone is roaming, no need to register
        for (int i = 0; i < mPhoneValueList.size(); ++i) {
            mIsRoaming[i] = (Boolean) mPhoneValueList.get(i).get(KEY_ROAMING);
            if (mIsRoaming[i] != null && mIsRoaming[i]) {
                Log.i(TAG, "Sim " + i + " is roaming, return false.");
                return false;
            }
        }

        // 3. If already registered and iccIds not change, no need
        if (AgentProxy.getInstance().isSelfRegistered()) {
            if (mPlatformManager.isIccIDIdentical(mIccIdList, mSlotList.length)) {
                Log.i(TAG, "IccId is identical, no need");
                return false;
            }
        }

        return true;
    }

    private void clearMessageInHandleQueue() {
        Log.i(TAG, "clearMessageInHandleQueue");
        if (mHandler.hasMessages(MSG_HANDLE_REGISTER)) {
            mHandler.removeMessages(MSG_HANDLE_REGISTER);
        }
        if (mHandler.hasMessages(MSG_HANDLE_NETWORK)) {
            mHandler.removeMessages(MSG_HANDLE_NETWORK);
        }
        if (mHandler.hasMessages(MSG_NO_STATE_CHANGE)) {
            mHandler.removeMessages(MSG_NO_STATE_CHANGE);
        }
    }

    /*
     * if MSG_HANDLE_REGISTER already in queue, do nothing; else, send it with 60s delay
     */
    private void sendDelayHandlerMessage() {
        sendDelayHandlerMessage(60);
    }

    private void sendDelayHandlerMessage(int second) {
        if (mHandler.hasMessages(MSG_HANDLE_REGISTER)) {
            Log.i(TAG, "Find MSG_HANDLE_REGISTER in queue, do nothing");
        } else {
            Log.i(TAG, "Send MSG_HANDLE_REGISTER " + second + "s later");
            mHandler.sendEmptyMessageDelayed(MSG_HANDLE_REGISTER, Const.ONE_SECOND * second);
        }
    }

    /*
     * Whether network is value: should be Wi-Fi on data link on a CT card
     */
    private Boolean isNetworkTypeValid() {
        Log.i(TAG, "[isNetworkTypeValid] check network info");
        if (mIsMobileLink) {
            Log.i(TAG, "[isNetworkTypeValid] Data link from slot " + mSlotDatalink);

            if (PlatformManager.supportCTForAllSlots()) {
                if (mSlotDatalink == Const.SLOT_ID_INVALID) {
                    Log.i(TAG, "[isNetworkTypeValid] slot id invalid");
                    return false;
                }

                int index = getIndexFromSlotId(mSlotDatalink);
                if (!mIsSlotForCT[index]) {
                    Log.i(TAG, "[isNetworkTypeValid] " + mSlotDatalink + " is not for CT");
                    return false;
                }

            } else {
                if (mSlotDatalink != Const.SLOT_ID_0) {
                    Log.i(TAG, "[isNetworkTypeValid] not from slot " + Const.SLOT_ID_0);
                    return false;
                }
            }
        } else {
            Log.i(TAG, "[isNetworkTypeValid] not data link");
        }

        Log.i(TAG, "[isNetworkTypeValid] network valid");
        return true;
    }

    //------------------------------------------------------
    // alarm, network
    //------------------------------------------------------

    private void setRetryAlarm() {
        SharedPreferences preference = PlatformManager.getUniquePreferences(mContext);

        int retryTimes = preference.getInt(PRE_KEY_RETRY_TIMES, 0);
        if (retryTimes < TIMES_MAX_RETRY) {
            retryTimes++;
            Log.i(TAG, "Set alarm, retry one hour. Times " + retryTimes);
            preference.edit().putInt(PRE_KEY_RETRY_TIMES, retryTimes).commit();

            PlatformManager.setExactAlarm(this, RegisterService.class, Const.ACTION_RETRY,
                    Const.ONE_HOUR);
        } else {
            Log.i(TAG, "Already retried " + retryTimes + " times, register failed.");
        }

    }

    private void registerNetworkCallback() {
        if (mNetworkCallback == null) {
            Log.i(TAG, "registerNetworkCallback first time.");

            mNetworkCallback = new CustomizedNetworkCallback();
            NetworkRequest networkRequest = new NetworkRequest.Builder()
                .addCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET).build();

            if (mConnectivityManager == null) {
                mConnectivityManager = (ConnectivityManager) getSystemService(
                        Context.CONNECTIVITY_SERVICE);
                Log.i(TAG, "mConnectivityManager is null, init again " + mConnectivityManager);
            }

            mConnectivityManager.registerNetworkCallback(networkRequest, mNetworkCallback);
        } else {
            Log.i(TAG, "[registerNetworkCallback] already registered, do nothing");
        }
    }

    private void unregisterNetworkCallback() {
        Log.i(TAG, "unregisterNetworkCallback");
        if (mNetworkCallback != null) {
            mConnectivityManager.unregisterNetworkCallback(mNetworkCallback);

            mNetworkCallback = null;
            mConnectivityManager = null;
        }
    }

    //------------------------------------------------------
    // gets
    //------------------------------------------------------

    public String getIccIdFromCard(int slotId) {
        int index = getIndexFromSlotId(slotId);
        return mIccIdList[index];
    }

    public PlatformManager getPlatformManager() {
        return mPlatformManager;
    }

    private int getSlotIdFromIndex(int index) {
        int slotId = index + Const.SLOT_ID_0;
        return slotId;
    }

    private int getIndexFromSlotId(int slotId) {
        int index = slotId - Const.SLOT_ID_0;
        return index;
    }

    private int getMasterSlot() {

        if (PlatformManager.supportCTForAllSlots()) {
            // If enable mobile data, use this slot
            int slotId = mSlotDatalink;
            if (slotId != Const.SLOT_ID_INVALID) {
                Log.i(TAG, "[getMasterSlot] has mobile link, return " + slotId);
                return slotId;
            }

            // If set 3/4G capability, use this slot if has sim
            slotId = PlatformManager.getSimSwitchSlot();
            if (slotId != Const.SLOT_ID_INVALID && mPlatformManager.hasIccCard(slotId)) {
                Log.i(TAG, "[getMasterSlot] 3/4G on slot " + slotId);
                return slotId;
            }

            // Otherwise, find first slot having a sim
            for (int i = 0; i < mSlotList.length; ++i) {
                slotId = getSlotIdFromIndex(i);
                if (mPlatformManager.hasIccCard(slotId)) {
                    break;
                }
            }
            Log.i(TAG, "[getMasterSlot] first slot having a sim is " + slotId);
            return slotId;

        } else {
            // For A test, use slot 0
            Log.i(TAG, "[getMasterSlot] first slot having a sim is " + Const.SLOT_ID_0);
            return Const.SLOT_ID_0;
        }
    }

    private int getLocationSlot() {

        if (PlatformManager.supportCTForAllSlots()) {
            return mSlotMaster;

        } else {
         // For A test, use first slot having a sim
            int slotId = Const.SLOT_ID_INVALID;
            for (int i = 0; i < mSlotList.length; ++i) {
                slotId = getSlotIdFromIndex(i);
                if (mPlatformManager.hasIccCard(slotId)) {
                    break;
                }
            }
            Log.i(TAG, "[getLocationSlot] first slot having a sim is " + slotId);
            return slotId;
        }
    }

    //------------------------------------------------------
    // Wrapper of PlatformManager
    //------------------------------------------------------

    // M: CT SIM in Slot0 may returns two IMSIs.
    //    Index 0 returns CDMA IMSI, index 1 returns LTE IMSI.
    public String[] getComplexImsi(int slotId) {
        String imsiArray[] = mPlatformManager.getComplexImsi(mContext, slotId, mSlotMaster);
        return imsiArray;
    }

    private boolean isIccCardForCT(int slotId) {
        Boolean value = mPlatformManager.isSlotForCT(slotId);
        Log.i(TAG, "isIccCardForCT(" + slotId + ") " + value);
        return value;
    }

    // M: Get card type of 1 for ICC type, 2 for UICC type.
    public int getSimType(int slotId) {
        int value = mPlatformManager.getSimType(slotId);
        Log.i(TAG, "Sim type is " + value);
        return value;
    }

    public String getMeid() {
        String meid = mPlatformManager.getDeviceMeid(this);
        Log.i(TAG, "meid is " + meid);
        return meid;
    }

    public String getImei(int slotId) {
        String imei = mPlatformManager.getImei(this, slotId);
        Log.i(TAG, "imei is " + imei);
        return imei;
    }

    //------------------------------------------------------
    // Location info
    //------------------------------------------------------

    public int getCdmaBaseId() {
        int value = getCdmaLocationInfo(KEY_BASEID);
        Log.i(TAG, "[getCdmaBaseId] Get base Id " + value);
        return value;
    }

    public int getCdmaNi() {
        int value = getCdmaLocationInfo(KEY_NI);
        Log.i(TAG, "[getCdmaNi] Get network Id " + value);
        return value;
    }

    public int getCdmaSi() {
        int value = getCdmaLocationInfo(KEY_SI);
        Log.i(TAG, "[getCdmaSi] Get system Id " + value);
        return value;
    }

    private int getCdmaLocationInfo(String key) {
        Log.i(TAG, "getCdmaLocationInfo key " + key);

        int slotLocation = getLocationSlot();
        int index = getIndexFromSlotId(slotLocation);

        // 1. check the map first
        if (mPhoneValueList.get(index).containsKey(key)) {
            Integer result = (Integer) mPhoneValueList.get(index).get(key);
            Log.i(TAG, "Get from map, value is " + result);

            if (result != null) {
                return result.intValue();
            }
        } else {
            Log.i(TAG, "Key " + key + " not in map " + index);
        }

        CellLocation location = mPlatformManager.getCellLocation();
        if (location != null && location instanceof CdmaCellLocation) {

            int value = Const.VALUE_INVALID_INT;;
            if (key.equalsIgnoreCase(KEY_BASEID)) {
                value = ((CdmaCellLocation) location).getBaseStationId();

            } else if (key.equalsIgnoreCase(KEY_NI)) {
                value = ((CdmaCellLocation) location).getNetworkId();

            } else if (key.equalsIgnoreCase(KEY_SI)) {
                value = ((CdmaCellLocation) location).getSystemId();
            }

            Log.i(TAG, "Get from framework, value is " + value);
            return value;

        } else if (location != null && location instanceof GsmCellLocation) {
            int value = Const.VALUE_INVALID_INT;

            if (key.equalsIgnoreCase(KEY_BASEID)) {
                value = ((GsmCellLocation) location).getCid();
            }
            Log.i(TAG, "Get from framework, value is " + value);
            return value;
        }

        // 3. Get value from shared preference
        SharedPreferences preference = PlatformManager.getSimPreference(mContext, index);
        int value = preference.getInt(key, Const.VALUE_INVALID_INT);
        Log.i(TAG, "Get from preference, value " + value);
        return value;
    }

    //------------------------------------------------------------------------------------------
    // Instance of handler & definition of customized NetworkCallback & PhoneStateListener
    //------------------------------------------------------------------------------------------

    @SuppressLint("HandlerLeak")
    private Handler mHandler = new Handler() {

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_HANDLE_REGISTER:
                    handleRegisterMsg();
                    break;

                case MSG_HANDLE_RESULT:
                    handleResponseMsg(msg);
                    break;

                case MSG_HANDLE_NETWORK:
                    handleNetworkMsg();
                    break;

                case MSG_NO_STATE_CHANGE:
                    handleNoStateChange();
                    break;
                default:
                    super.handleMessage(msg);
            }
        }
    };

    private class CustomizedNetworkCallback extends ConnectivityManager.NetworkCallback {

        @Override
        public void onAvailable(Network network) {
            super.onAvailable(network);
            Log.i(TAG, "[onAvailable] network " + network);

            if (mConnectivityManager == null) {
                Log.e(TAG, "[onAvailable] connectivityManager is null!");
                return;
            }

            NetworkInfo networkInfo = mConnectivityManager.getNetworkInfo(network);
            if (networkInfo == null) {
                Log.e(TAG, "[onAvailable] networkInfo is null!");
                return;
            }

            displayNetworkInfo(networkInfo);

            int subId = PlatformManager.getDefaultDataSubId();
            int slotId = PlatformManager.getSlotId(subId);

            Log.i(TAG, "[onAvailable] subId is " + subId + ", slotId is " + slotId);

            int networkType = networkInfo.getType();
            if (networkType == ConnectivityManager.TYPE_MOBILE) {
                mIsMobileLink = true;
                mSlotDatalink = slotId;

            } else {
                mIsMobileLink = false;
                mSlotDatalink = Const.SLOT_ID_INVALID;
            }

            mIsNetworkAvailable = true;

            // when network ready, sim may not finish the init process
            sendDelayHandlerMessage();
        }

        @Override
        public void onLost(Network network) {
            super.onLost(network);
            Log.i(TAG, "[onLost] network " + network);

            if (mConnectivityManager == null) {
                Log.e(TAG, "[onLost] onnectivityManager is null!");
                return;
            }

            NetworkInfo networkInfo = mConnectivityManager.getNetworkInfo(network);
            if (networkInfo == null) {
                Log.e(TAG, "[onLost] networkInfo is null!");
                return;

            } else {
                displayNetworkInfo(networkInfo);
                mIsNetworkAvailable = false;
                mIsMobileLink = false;
                mSlotDatalink = Const.SLOT_ID_INVALID;

            }
        }

        private void displayNetworkInfo(NetworkInfo networkInfo) {
            Log.i(TAG, "NetworkInfo type " + networkInfo.getType() + ", name is "
                    + networkInfo.getTypeName());
        }
    };

    private class CustomizedPhoneStateListener extends PhoneStateListener {

        private int mIndex;

        public CustomizedPhoneStateListener(int index, int subId) {
            super(subId);
            mIndex = index;
        }

        @Override
        public void onCellLocationChanged(CellLocation location) {
            super.onCellLocationChanged(location);

            int systemId = Const.VALUE_INVALID_INT;
            int networkId = Const.VALUE_INVALID_INT;
            int baseId = Const.VALUE_INVALID_INT;

            if (location instanceof CdmaCellLocation) {
                systemId = ((CdmaCellLocation) location).getSystemId();
                networkId = ((CdmaCellLocation) location).getNetworkId();
                baseId = ((CdmaCellLocation) location).getBaseStationId();

            } else if (location instanceof GsmCellLocation) {
                baseId = ((GsmCellLocation) location).getCid();
            }

            Log.i(TAG, "[onCell " + mIndex + "] SI: " + systemId + " NI: " + networkId
                    + " Base ID: " + baseId);

            mPhoneValueList.get(mIndex).put(KEY_SI, systemId);
            mPhoneValueList.get(mIndex).put(KEY_NI, networkId);
            mPhoneValueList.get(mIndex).put(KEY_BASEID, baseId);

            // M: Save the SI and NI in SharedPreference, which may be used in retry.
            SharedPreferences.Editor editor = PlatformManager.
                    getSimPreference(mContext, mIndex).edit();

            editor.putInt(KEY_SI, systemId);
            editor.putInt(KEY_NI, networkId);
            editor.putInt(KEY_BASEID, baseId);
            editor.commit();

            // when location ready, sim/network operator info may not be available
            sendDelayHandlerMessage();
        }

        @Override
        public void onServiceStateChanged(ServiceState serviceState) {
            super.onServiceStateChanged(serviceState);

            if (serviceState.getState() != ServiceState.STATE_IN_SERVICE) {
                Log.i(TAG, "[onService " + mIndex + "] not in service " + serviceState.getState());
                return;
            }

            boolean roaming = serviceState.getRoaming();
            Log.i(TAG, "[onService " + mIndex + "] roaming: " + roaming);

            if (mPhoneValueList.get(mIndex).containsKey(KEY_ROAMING)) {
                mPhoneValueList.get(mIndex).put(KEY_ROAMING, roaming);

            } else {
                // First time in service, try to register
                CellLocation location = mPlatformManager.getCellLocation();
                if (location != null) {
                    Log.i(TAG, "[onService " + mIndex + "] CellLocation is " + location.toString());
                } else {
                    Log.i(TAG, "[onService " + mIndex + "] CellLocation is " + null);
                }

                mPhoneValueList.get(mIndex).put(KEY_ROAMING, roaming);

                sendDelayHandlerMessage();
            }
        }
    }

}
