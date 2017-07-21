package com.mediatek.sensorhub.ui;

import android.hardware.Sensor;
import android.os.Bundle;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.SwitchPreference;

import com.mediatek.sensorhub.settings.Utils;

public class InPocketSensorActivity extends BaseActivity implements OnPreferenceChangeListener {

    public static final String KEY_ENABLE_NOTIFY = "enable_notify_inpocket";
    private static final String TAG = "InPocketSensorActivity";
    private SwitchPreference mInpocketPreference;
    private SwitchPreference mEnableNotifiSwitch;

    public InPocketSensorActivity() {
        super(Utils.KEY_INPOCKET_STATUS);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.xml.sensor_in_pocket_pref);
        initializeAllPreferences();
    }

    @Override
    protected void onResume() {
        super.onResume();
        updatePreferenceStatus();
    }

    private void initializeAllPreferences() {
        mInpocketPreference = (SwitchPreference) Utils.createPreference(Utils.TYPE_SWITCH,
                mSensorKeyMap.get(Sensor.TYPE_IN_POCKET).getName(), String
                        .valueOf(Sensor.TYPE_IN_POCKET), getPreferenceScreen(), this);
        mInpocketPreference.setOrder(-1);
        mEnableNotifiSwitch = (SwitchPreference) findPreference(KEY_ENABLE_NOTIFY);
        mEnableNotifiSwitch.setOnPreferenceChangeListener(this);
    }

    private void updatePreferenceStatus() {
        mInpocketPreference.setChecked(Utils.getSensorStatus(Utils.KEY_INPOCKET_STATUS));
        mEnableNotifiSwitch.setChecked(Utils.getSensorStatus(Utils.KEY_INPOCKET_NOTIFY_STATUS));
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        boolean bNewValue = (Boolean) newValue;
        if (preference == mInpocketPreference) {
            Utils.setSensorStatus(Utils.KEY_INPOCKET_STATUS, bNewValue);
            if (mBound) {
                mSensorService.registerSensor(preference.getKey(), bNewValue);
            }
        } else if (preference == mEnableNotifiSwitch) {
            Utils.setSensorStatus(Utils.KEY_INPOCKET_NOTIFY_STATUS, bNewValue);
        }
        return true;
    }

    @Override
    public void onSensorChanged(float[] value) {
        // Log.d(TAG, "onSensorChanged value is: " + value[0]);
        mInpocketPreference.setSummary(String.valueOf(value[0]));
    }
}
