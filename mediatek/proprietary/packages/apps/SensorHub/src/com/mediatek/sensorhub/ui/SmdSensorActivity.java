package com.mediatek.sensorhub.ui;

import android.hardware.Sensor;
import android.os.Bundle;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.SwitchPreference;

import com.mediatek.sensorhub.settings.Utils;

public class SmdSensorActivity extends BaseActivity implements OnPreferenceChangeListener {

    private static final String TAG = "SmdSensorActivity";
    public static final String KEY_ENABLE_NOTIFY = "enable_notify_smd";
    private SwitchPreference mSmdPreference;
    private SwitchPreference mEnableNotifiSwitch;

    public SmdSensorActivity() {
        super(Utils.KEY_SMD_STATUS);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.xml.sensor_smd_pref);
        initializeAllPreferences();
    }

    @Override
    protected void onResume() {
        super.onResume();
        updatePreferenceStatus();
    }

    private void initializeAllPreferences() {
        mSmdPreference = (SwitchPreference) Utils.createPreference(Utils.TYPE_SWITCH, mSensorKeyMap
                .get(Sensor.TYPE_SIGNIFICANT_MOTION).getName(), String
                .valueOf(Sensor.TYPE_SIGNIFICANT_MOTION), getPreferenceScreen(), this);
        mSmdPreference.setOrder(-1);
        mEnableNotifiSwitch = (SwitchPreference) findPreference(KEY_ENABLE_NOTIFY);
        mEnableNotifiSwitch.setOnPreferenceChangeListener(this);
    }

    private void updatePreferenceStatus() {
        mSmdPreference.setChecked(Utils.getSensorStatus(Utils.KEY_SMD_STATUS));
        mEnableNotifiSwitch.setChecked(Utils.getSensorStatus(Utils.KEY_SMD_NOTIFY_STATUS));
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        boolean bNewValue = (Boolean) newValue;
        if (preference == mSmdPreference) {
            Utils.setSensorStatus(Utils.KEY_SMD_STATUS, bNewValue);
            if (mBound) {
                mSensorService.registerSensor(preference.getKey(), bNewValue);
            }
        } else if (preference == mEnableNotifiSwitch) {
            Utils.setSensorStatus(Utils.KEY_SMD_NOTIFY_STATUS, bNewValue);
        }
        return true;
    }

    @Override
    public void onSensorChanged(float[] value) {
        mSmdPreference.setSummary(String.valueOf(value[0]));
    }
}
