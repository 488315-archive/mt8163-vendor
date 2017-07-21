package com.mediatek.smartmotion.enabler;

import android.content.Context;
import android.widget.Switch;

import com.mediatek.sensorhub.ContextInfo;
import com.mediatek.smartmotion.utils.LogUtils;

public class PdrEnabler  extends SmartMotionEnabler {
    private static PdrEnabler PdrEnabler;

    private PdrEnabler(Context context) {
        super(context);
        mIsChecked = mPreferences.getPdr();
    }

    public synchronized static void registerSwitch(Context context, Switch switch_) {
        if (context == null || switch_ == null) {
            return;
        }

        if (PdrEnabler == null) {
            PdrEnabler = new PdrEnabler(context);
        }
        PdrEnabler.addSwitch(switch_);
        LogUtils.d(TAG, "registerSwitch");
    }

    public synchronized static void unregisterSwitch(Switch switch_) {
        if (PdrEnabler == null) {
            return;
        }
        PdrEnabler.removeSwitch(switch_);
    }

    public synchronized static void unregisterAllSwitches() {
        if (PdrEnabler == null) {
            return;
        }
        PdrEnabler.removeAllSwitches();
    }

    @Override
    protected void setPreference() {
        mPreferences.setPdr(mIsChecked);
    }

    @Override
    protected void enableSensor() {
        mRequestId = sSensorHubClient.addRequest(ContextInfo.Type.PDR);
        LogUtils.d(TAG, "add request for PDR: " + mRequestId);
    }
}
