package com.mediatek.sensorhub.ui;

import android.content.Context;
import android.hardware.Sensor;
import android.os.Bundle;
import android.os.PowerManager;
import android.os.SystemClock;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.SwitchPreference;
import android.util.Log;

import com.mediatek.sensorhub.settings.Utils;

public class OrginalGesturesActivity extends BaseActivity implements OnPreferenceChangeListener {

    private static final String TAG = "OrginalGesturesActivity";

    public OrginalGesturesActivity() {
        super(TAG);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.xml.orginal_gestures_pref);
        initializeAllPreferences();
    }

    @Override
    protected void onResume() {
        super.onResume();
        updatePreferenceStatus();
    }

    private void initializeAllPreferences() {
        for (int type : Utils.orginalGestureType) {
            Sensor sensor = mSensorKeyMap.get(type);
            if (sensor != null) {
                Utils.createPreference(Utils.TYPE_SWITCH, sensor.getName(), String.valueOf(type),
                        getPreferenceScreen(), this);
            }
        }
    }

    private void updatePreferenceStatus() {
        for (int type : Utils.orginalGestureType) {
            String prefKey = String.valueOf(type);
            SwitchPreference preference = (SwitchPreference) findPreference(prefKey);
            if (preference != null) {
                preference.setChecked(Utils.getSensorStatus(prefKey));
            }
        }
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        boolean bNewValue = (Boolean) newValue;
        Utils.setSensorStatus(preference.getKey(), bNewValue);
        if (mBound) {
            mSensorService.registerSensor(preference.getKey(), bNewValue);
        }
        return true;
    }

    private void suspend() {
        Log.d(TAG, "suspend");
        PowerManager powerManager = (PowerManager) getSystemService(Context.POWER_SERVICE);
        try {
            powerManager.goToSleep(SystemClock.uptimeMillis());
            Log.d(TAG, "Was able to call PowerManager.goToSleep.");
        } catch (SecurityException e) {
            Log.d(TAG, "Was unable to call PowerManager.goToSleep.");
        }
    }
}
