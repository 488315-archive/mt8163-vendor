package com.mediatek.smartmotion.enabler;

import android.telephony.PhoneStateListener;

import com.mediatek.smartmotion.MainActivity;
import com.mediatek.smartmotion.utils.LogUtils;

import java.util.ArrayList;
import java.util.List;

public class IncomingCallListener extends PhoneStateListener {
    private static final String TAG = "IncomingCallListener";
    private static IncomingCallListener sIncomingCallListener;
    private List<Callback> mCallbacks = new ArrayList<Callback>();

    interface Callback {
        void onCallStateChanged(int state);
    }

    public synchronized static IncomingCallListener getInstance() {
        if (sIncomingCallListener == null) {
            sIncomingCallListener = new IncomingCallListener();
        }
        return sIncomingCallListener;
    }

    @Override
    public void onCallStateChanged(int state, String incomingNumber) {
        if (!MainActivity.sDemoMode) {
            return;
        }
        LogUtils.i(TAG, "IncomingCallListener onCallStateChanged, size:" + mCallbacks.size());
        for (Callback cb : mCallbacks) {
            cb.onCallStateChanged(state);
        }
    }

    public void registerListener(Callback listener) {
        mCallbacks.add(listener);
    }
}
