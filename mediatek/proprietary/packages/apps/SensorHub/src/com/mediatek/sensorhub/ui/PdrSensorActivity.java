package com.mediatek.sensorhub.ui;

import android.hardware.Sensor;
import android.os.Bundle;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.SwitchPreference;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;

import com.mediatek.sensorhub.settings.Utils;

import java.util.Arrays;

public class PdrSensorActivity extends BaseActivity implements OnPreferenceChangeListener,
        OnClickListener {

    private static final String TAG = "PdrSensorActivity";
    private PdrLayoutPreference mPdrSetView;
    private SwitchPreference mPdrPref;
    EditText mPdrHeight;
    EditText mPdrWeight;
    EditText mPdrGender;
    EditText mPdrStepLen;
    Button mSetButton;

    public PdrSensorActivity() {
        super(Utils.KEY_PDR_STATUS);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.xml.sensor_pdr_pref);
        initializeAllPreferences();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mPdrPref.setChecked(Utils.getSensorStatus(Utils.KEY_PDR_STATUS));
    }

    private void initializeAllPreferences() {
        mPdrPref = (SwitchPreference) Utils.createPreference(Utils.TYPE_SWITCH, mSensorKeyMap.get(
                Sensor.TYPE_PDR).getName(), String.valueOf(Sensor.TYPE_PDR), getPreferenceScreen(),
                this);
        mPdrPref.setOrder(-1);
        mPdrSetView = (PdrLayoutPreference) findPreference("pdr_set_view");
        mPdrHeight = (EditText) mPdrSetView.findViewById(R.id.pdr_height);
        mPdrWeight = (EditText) mPdrSetView.findViewById(R.id.pdr_weight);
        mPdrGender = (EditText) mPdrSetView.findViewById(R.id.pdr_gender);
        mPdrStepLen = (EditText) mPdrSetView.findViewById(R.id.pdr_step_len);
        mSetButton = (Button) mPdrSetView.findViewById(R.id.set);
        mSetButton.setOnClickListener(this);
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        boolean bNewValue = (Boolean) newValue;
        if (preference == mPdrPref) {
            Utils.setSensorStatus(Utils.KEY_PDR_STATUS, bNewValue);
            if (mBound) {
                mSensorService.registerSensor(preference.getKey(), bNewValue);
            }
        }
        return true;
    }

    @Override
    public void onClick(View v) {
        if (v == mSetButton) {

        }
    }

    @Override
    public void onSensorChanged(float[] value) {
        if (mPdrPref != null) {
            mPdrPref.setSummary(Arrays.toString(value));
        }
    }
}
