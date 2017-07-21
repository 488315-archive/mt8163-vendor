package com.mediatek.smartmotion.enabler;

import com.mediatek.smartmotion.utils.LogUtils;

import android.content.Context;
import android.widget.Switch;

import com.mediatek.sensorhub.ContextInfo;

public class SnapshotEnabler extends SmartMotionEnabler {
    private static SnapshotEnabler sSnapShotEnabler;

    private SnapshotEnabler(Context context) {
        super(context);
        mIsChecked = mPreferences.getSnapshot();
    }

    public synchronized static void registerSwitch(Context context, Switch switch_) {
        if (context == null || switch_ == null) {
            return;
        }

        if (sSnapShotEnabler == null) {
            sSnapShotEnabler = new SnapshotEnabler(context);
        }
        sSnapShotEnabler.addSwitch(switch_);
    }

    public synchronized static void unregisterSwitch(Switch switch_) {
        if (sSnapShotEnabler == null) {
            return;
        }
        sSnapShotEnabler.removeSwitch(switch_);
    }

    public synchronized static void unregisterAllSwitches() {
        if (sSnapShotEnabler == null) {
            return;
        }
        sSnapShotEnabler.removeAllSwitches();
    }

    @Override
    protected void setPreference() {
        mPreferences.setSnapshot(mIsChecked);
    }

    @Override
    protected void enableSensor() {
        mRequestId = sSensorHubClient.addRequest(ContextInfo.Type.SNAPSHOT);
        LogUtils.d(TAG, "add request for SNAPSHOT: " + mRequestId);
    }
}
