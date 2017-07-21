package com.mediatek.systemui.plugin;

import android.app.Service;
import android.content.Intent;
import android.content.Context;
import android.os.IBinder;
import android.telephony.PhoneStateListener;
import android.telephony.PreciseDataConnectionState;
import android.telephony.SubscriptionInfo;
import android.telephony.SubscriptionManager;
import android.telephony.SubscriptionManager.OnSubscriptionsChangedListener;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.android.internal.telephony.PhoneConstants;

import java.util.HashSet;
import java.util.HashMap;
import java.util.List;

/**
 * M: Op01 PhoneState Listener Controller.
 */
public class PhoneStateListenerController {
    private static final String TAG = "OP01-PhoneStateListenerController";

    private TelephonyManager mTelephonyManager;
    private SubscriptionManager mSubscriptionManager;
    private OnSubscriptionsChangedListener mSubscriptionsChangedListener;
    private Context mContext;

    private HashMap<Integer, StatusBarExtStateListener> mStatusBarExtStateListeners = 
            new HashMap<Integer, StatusBarExtStateListener>();

    private static PhoneStateListenerController mPhoneStateController;

    public static PhoneStateListenerController getInstance(Context context) {
        if (mPhoneStateController != null) {
            return mPhoneStateController;
        } else {
            mPhoneStateController = new PhoneStateListenerController(context);
            return mPhoneStateController;
        }
    }

    public PhoneStateListenerController(Context context) {
        Log.v(TAG, "OP01 PhoneStateListenerService onCreate");
        mContext = context;
        mTelephonyManager = TelephonyManager.from(mContext);
        mSubscriptionManager = SubscriptionManager.from(mContext);

        mSubscriptionsChangedListener = new OnSubscriptionsChangedListener() {
            @Override
            public void onSubscriptionsChanged() {
                updateStateListener();
            }
        };
        registerStateListener();
    }

    private void registerStateListener() {
        Log.d(TAG, "registerStateListener");
        List<SubscriptionInfo> infos = mSubscriptionManager.getActiveSubscriptionInfoList();
        if (infos == null) {
            return;
        }

        for (SubscriptionInfo info : infos) {
            int subId = info.getSubscriptionId();
            if (!mStatusBarExtStateListeners.containsKey(subId)) {
                StatusBarExtStateListener listener = new StatusBarExtStateListener(subId);
                mStatusBarExtStateListeners.put(subId, listener);
            }
        }

        // Listen for phone state changed
        for (StatusBarExtStateListener listener : mStatusBarExtStateListeners.values()) {
            mTelephonyManager.listen(listener,
                    PhoneStateListener.LISTEN_PRECISE_DATA_CONNECTION_STATE);
        }

        // Listen for sim changed.
        mSubscriptionManager.addOnSubscriptionsChangedListener(mSubscriptionsChangedListener);
    }

    public synchronized boolean isShowDataActyIcon(int subId) {
        boolean isShow = true;
        if (mStatusBarExtStateListeners != null) {
            StatusBarExtStateListener listener = mStatusBarExtStateListeners.get(subId);
            if (listener != null) {
                isShow = listener.isShowDataActivityIcon();
            }
        }
        Log.v(TAG, "isShowDataActyIcon(),subId: " + subId + ", return isShow: " + isShow);
        return isShow;
    }

    private void updateStateListener() {
        Log.d(TAG, "updateStateListener");
        List<SubscriptionInfo> subInfos = mSubscriptionManager.getActiveSubscriptionInfoList();
        if (subInfos == null) {
            return;
        }

        // We should remove all the item in mRichCallStateListener, and then add
        // the listener again.
        for (Integer id : mStatusBarExtStateListeners.keySet()) {
            int subId = id.intValue();
            StatusBarExtStateListener listener = mStatusBarExtStateListeners.get(id);
            mTelephonyManager.listen(listener, PhoneStateListener.LISTEN_NONE);
        }
        mStatusBarExtStateListeners.clear();

        for (SubscriptionInfo info : subInfos) {
            int subId = info.getSubscriptionId();
            if (!mStatusBarExtStateListeners.containsKey(subId)) {
                StatusBarExtStateListener listener = new StatusBarExtStateListener(subId);
                mStatusBarExtStateListeners.put(subId, listener);
                mTelephonyManager.listen(listener,
                        PhoneStateListener.LISTEN_PRECISE_DATA_CONNECTION_STATE);
            }
        }
    }

    private class StatusBarExtStateListener extends PhoneStateListener {
        private HashSet<String> mPreciseDataConnectedState = new HashSet<String>();

        public StatusBarExtStateListener(int subId) {
            super(subId);
        }

        @Override
        public void onPreciseDataConnectionStateChanged(PreciseDataConnectionState state) {
            Log.d(TAG, "onPreciseDataConnectionStateChanged: state = " + state.toString());
            String apnType = state.getDataConnectionAPNType();
            int dataState = state.getDataConnectionState();
            synchronized (mPreciseDataConnectedState) {
                if (dataState == TelephonyManager.DATA_CONNECTED) {
                    if (!mPreciseDataConnectedState.contains(apnType)) {
                        mPreciseDataConnectedState.add(apnType);
                        Log.d(TAG, "onPreciseDataConnectionStateChanged: put apnType: " + apnType
                                + ", dataState: " + dataState + " into mPreciseDataConnectedState");
                    }
                } else {
                    if (mPreciseDataConnectedState.contains(apnType)) {
                        mPreciseDataConnectedState.remove(apnType);
                        Log.d(TAG, "onPreciseDataConnectionStateChanged: remove apnType: "
                                + apnType + ", dataState: " + dataState
                                + " from mPreciseDataConnectedState");
                    }
                }
            }
        }

        /**
         * M: add for op01 Check if show data activity icon.
         * 
         * @return if show data activity icon.
         */
        public boolean isShowDataActivityIcon() {
            synchronized (mPreciseDataConnectedState) {
                if (mPreciseDataConnectedState.size() == 1) {
                    if (mPreciseDataConnectedState.contains(PhoneConstants.APN_TYPE_IMS)
                            || mPreciseDataConnectedState
                                    .contains(PhoneConstants.APN_TYPE_EMERGENCY)) {
                        Log.d(TAG, "isShowDataActivityIcon(), return false");
                        return false;
                    }
                } else if (mPreciseDataConnectedState.size() == 2) {
                    if (mPreciseDataConnectedState.contains(PhoneConstants.APN_TYPE_IMS)
                            && mPreciseDataConnectedState
                                    .contains(PhoneConstants.APN_TYPE_EMERGENCY)) {
                        Log.d(TAG, "isShowDataActivityIcon(), return false");
                        return false;
                    }
                }
            }
            Log.d(TAG, "isShowDataActivityIcon(), return true, "
                    + "mPreciseDataConnectedState.size(): " + mPreciseDataConnectedState.size());
            return true;
        }
    }

}
