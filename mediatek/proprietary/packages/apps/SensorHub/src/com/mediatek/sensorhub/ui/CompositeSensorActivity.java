package com.mediatek.sensorhub.ui;

import android.hardware.Sensor;
import android.os.Bundle;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.SwitchPreference;

import com.mediatek.sensorhub.settings.Utils;

import java.util.Arrays;

public class CompositeSensorActivity extends BaseActivity implements OnPreferenceChangeListener {

    public CompositeSensorActivity() {
        super(Utils.KEY_COMPOSITE_STATUS);
    }

    private static final String TAG = "CompositeSensorActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.xml.orginal_gestures_pref);
        setTitle(R.string.composite_sensor_title);
        initializeAllPreferences();
    }

    @Override
    protected void onResume() {
        super.onResume();
        updatePreferenceStatus();
    }

    private void initializeAllPreferences() {
        for (int type : Utils.compositeType) {
            Sensor sensor = mSensorKeyMap.get(type);
            if (sensor != null) {
                Utils.createPreference(Utils.TYPE_SWITCH, sensor.getName(), String.valueOf(type),
                        getPreferenceScreen(), this);
            }
        }
    }

    private void updatePreferenceStatus() {
        for (int type : Utils.compositeType) {
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

    @Override
    public void onSensorChanged(int sensorType, float[] value) {
        Preference preference = findPreference(String.valueOf(sensorType));
        if (preference != null) {
            preference.setSummary(Arrays.toString(value));
        }
    }
}
