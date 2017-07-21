package com.mediatek.smartmotion.enabler;

import android.content.Context;
import android.widget.Switch;

import com.mediatek.sensorhub.ContextInfo;
import com.mediatek.sensorhub.SensorHubManager;
import com.mediatek.smartmotion.sensor.SensorHubClient;
import com.mediatek.smartmotion.utils.LogUtils;

public class FreeFallEnabler extends SmartMotionEnabler {

    private static FreeFallEnabler sFreeFallEnabler;

    private FreeFallEnabler(Context context) {
        super(context);
        mIsChecked = mPreferences.getFreeFall();
    }

    public synchronized static void registerSwitch(Context context, Switch switch_) {
        if (context == null || switch_ == null) {
            return;
        }

        if (sFreeFallEnabler == null) {
            sFreeFallEnabler = new FreeFallEnabler(context);
        }
        sFreeFallEnabler.addSwitch(switch_);
    }

    public synchronized static void unregisterSwitch(Switch switch_) {
        if (sFreeFallEnabler == null) {
            return;
        }
        sFreeFallEnabler.removeSwitch(switch_);
    }

    public synchronized static void unregisterAllSwitches() {
        if (sFreeFallEnabler == null) {
            return;
        }
        sFreeFallEnabler.removeAllSwitches();
    }

    @Override
    protected void setPreference() {
        mPreferences.setFreeFall(mIsChecked);
    }

    @Override
    protected void enableSensor() {
        mRequestId = sSensorHubClient.addRequest(ContextInfo.Type.FREE_FALL);
        LogUtils.d(TAG, "add request for FreeFall: " + mRequestId);
    }

}
