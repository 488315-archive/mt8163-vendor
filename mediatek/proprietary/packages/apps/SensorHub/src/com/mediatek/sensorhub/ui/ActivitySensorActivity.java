package com.mediatek.sensorhub.ui;

import android.hardware.Sensor;
import android.os.Bundle;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.SwitchPreference;

import com.mediatek.sensorhub.settings.Utils;

public class ActivitySensorActivity extends BaseActivity implements OnPreferenceChangeListener {

    public ActivitySensorActivity() {
        super(Utils.KEY_ACTIVITY_STATUS);
    }

    private static final String TAG = "ActivitySensorActivity";
    private SwitchPreference mActivityPref;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.xml.sensor_activity_pref);
        initializeAllPreferences();
    }

    @Override
    protected void onResume() {
        super.onResume();
        updatePreferenceStatus();
    }


    private void initializeAllPreferences() {
        mActivityPref = (SwitchPreference) Utils.createPreference(Utils.TYPE_SWITCH, mSensorKeyMap
                .get(Sensor.TYPE_ACTIVITY).getName(), String.valueOf(Sensor.TYPE_ACTIVITY),
                getPreferenceScreen(), this);
        mActivityPref.setOrder(-1);
    }

    private void updatePreferenceStatus() {
        mActivityPref.setChecked(Utils.getSensorStatus(Utils.KEY_ACTIVITY_STATUS));
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        boolean bNewValue = (Boolean) newValue;
        if (preference == mActivityPref) {
            Utils.setSensorStatus(Utils.KEY_ACTIVITY_STATUS, bNewValue);
            if (mBound) {
                mSensorService.registerSensor(preference.getKey(), bNewValue);
            }
        }
        return true;
    }

    @Override
    public void onSensorChanged(float[] value) {
        // Log.d(TAG, "onSensorChanged value is: " + value[0]);
        for (int i = 0; i < value.length; i++) {
            Preference preference = findPreference(String.valueOf(i));
            if (preference != null) {
                preference.setSummary(String.valueOf(value[i]));
            }
        }
    }
}
