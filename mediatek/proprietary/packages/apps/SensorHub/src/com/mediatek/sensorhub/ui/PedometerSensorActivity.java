package com.mediatek.sensorhub.ui;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.hardware.Sensor;
import android.os.Bundle;
import android.preference.Preference;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.SwitchPreference;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;

import com.mediatek.sensorhub.settings.Utils;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;

public class PedometerSensorActivity extends BaseActivity implements OnClickListener,
        OnPreferenceChangeListener {

    private static final String TAG = "PedometerSensorActivity";
    private static final String FILE_PATH = "/sys/class/misc/m_step_c_misc/step_length";
    public static final String KEY_TOTAL_STEPS_PREFER = "total_steps";
    public static final String KEY_TOTAL_DISTANCE_PREFER = "total_distance";
    private SwitchPreference mPedometerPref;
    private Preference mTotalStepsPref;
    private Preference mTotalDistancePref;
    private EditText mLength;
    private Button mSetButton;
    private Button mClearButton;
    private float mPreToatalSteps;
    private float mPreToatalDistance;

    public PedometerSensorActivity() {
        super(Utils.KEY_PEDOMETER_STATUS);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.xml.sensor_pedometer_pref);
        initializeAllPreferences();
    }

    @Override
    protected void onResume() {
        super.onResume();
        updatePreferenceStatus();
    }

    private void initializeAllPreferences() {
        mPedometerPref = (SwitchPreference) Utils.createPreference(Utils.TYPE_SWITCH, mSensorKeyMap
                .get(Sensor.TYPE_PEDOMETER).getName(), String.valueOf(Sensor.TYPE_PEDOMETER),
                getPreferenceScreen(), this);
        mPedometerPref.setOrder(-1);
        mTotalStepsPref = findPreference(KEY_TOTAL_STEPS_PREFER);
        mTotalDistancePref = findPreference(KEY_TOTAL_DISTANCE_PREFER);
        PedometerLayoutPreference pedometerLayout =
            (PedometerLayoutPreference) findPreference("pedometer_set_view");
        mLength = (EditText) pedometerLayout.findViewById(R.id.step_length_value);
        // mLength.requestFocus();
        mSetButton = (Button) pedometerLayout.findViewById(R.id.set_step);
        mSetButton.setOnClickListener(this);
        mClearButton = (Button) pedometerLayout.findViewById(R.id.clear_step);
        mClearButton.setOnClickListener(this);
    }

    private void updatePreferenceStatus() {
        mPedometerPref.setChecked(Utils.getSensorStatus(Utils.KEY_PEDOMETER_STATUS));
        mPreToatalDistance = Utils.getSensorValues(Utils.KEY_PRE_TOTAL_DISTANCE);
        mPreToatalSteps = Utils.getSensorValues(Utils.KEY_PRE_TOTAL_STEPS);
        mClearButton.setEnabled(false);
    }

    @Override
    public void onClick(View v) {
        Button buttonViewButton = (Button) v;
        Log.d(TAG, "onclick: " + buttonViewButton.getText());
        if (v == mClearButton) {
            clear();
        } else if (v == mSetButton) {
            Log.d(TAG, "onclick mLength.getText() " + mLength.getText());
            if (TextUtils.isEmpty(mLength.getText().toString())) {
                showDialog(0);
            } else {
                setStepLength(Integer.valueOf(mLength.getText().toString()));
            }
        }
    }

    private void clear() {
        mTotalDistancePref.setSummary(R.string.init_number_str);
        mTotalStepsPref.setSummary(R.string.init_number_str);
        try {
            mPreToatalDistance = Float.valueOf(findPreference("0").getSummary().toString());
            mPreToatalSteps = Float.valueOf(findPreference("2").getSummary().toString());
        } catch (NullPointerException e) {
            mPreToatalDistance = 0;
            mPreToatalSteps = 0;
        }
        Utils.setSensorValues(Utils.KEY_PRE_TOTAL_DISTANCE, mPreToatalDistance);
        Utils.setSensorValues(Utils.KEY_PRE_TOTAL_STEPS, mPreToatalSteps);
    }

    @Override
    protected Dialog onCreateDialog(int id) {
        return new AlertDialog.Builder(this).setMessage(R.string.set_step_message)
                .setPositiveButton(android.R.string.yes, null).setNegativeButton(
                        android.R.string.no, new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                mLength.requestFocus();
                            }
                        }).create();
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        boolean bNewValue = (Boolean) newValue;
        if (preference == mPedometerPref) {
            Utils.setSensorStatus(Utils.KEY_PEDOMETER_STATUS, bNewValue);
            if (mBound) {
                mSensorService.registerSensor(preference.getKey(), bNewValue);
            }
            if (!bNewValue) {
                clear();
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
        mTotalDistancePref.setSummary(String.valueOf(value[0] - mPreToatalDistance));
        mTotalStepsPref.setSummary(String.valueOf(value[2] - mPreToatalSteps));
        mClearButton.setEnabled(true);
    }

    private void setStepLength(int stepLength) {
        try {
            FileOutputStream out = new FileOutputStream(new File(FILE_PATH));
            PrintStream ps = new PrintStream(out);
            ps.print(stepLength);
            out.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            Log.d(TAG, "setStepLength, can't find file " + FILE_PATH);
        } catch (IOException e) {
            e.printStackTrace();
            Log.d(TAG, "setStepLength, IOException happens. ");
        }
    }
}
