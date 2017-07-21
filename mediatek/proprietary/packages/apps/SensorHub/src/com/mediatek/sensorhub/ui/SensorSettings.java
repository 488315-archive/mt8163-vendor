package com.mediatek.sensorhub.ui;

import android.app.ActionBar;
import android.app.PendingIntent;
import android.content.Intent;
import android.hardware.Sensor;
import android.os.Bundle;
import android.preference.Preference;
import android.preference.PreferenceScreen;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.SwitchPreference;
import android.util.Log;
import android.view.Gravity;
import android.widget.CompoundButton;
import android.widget.Switch;

import com.mediatek.sensorhub.Action;
import com.mediatek.sensorhub.ActionDataResult;
import com.mediatek.sensorhub.Condition;
import com.mediatek.sensorhub.ContextInfo;
import com.mediatek.sensorhub.DataCell;
import com.mediatek.sensorhub.SensorHubManager;
import com.mediatek.sensorhub.settings.Utils;

public class SensorSettings extends BaseActivity implements OnPreferenceChangeListener,
        CompoundButton.OnCheckedChangeListener {

    private static final String TAG = "SensorSettings";
    private static final String KEY_GESTURE_WAKE_UP = "gesture_wake_up_pref";
    private static final String KEY_GOOGLE_GESTURE = "google_gestures_pref";
    private static final String KEY_COMPOSITE_PREF = "composite_pref";
    private SensorHubManager mSensorHubManager;
    private int mRequestId;
    private Switch mActionBarSwitch;
    private SwitchPreference mAccelePref;
    private SwitchPreference mLightPref;
    private SwitchPreference mProximityPref;
    private SwitchPreference mAnswerCall;
    private Preference mInPocketPref;
    private Preference mPedometerPref;
    private Preference mAcitvityPref;
    private Preference mSmdPref;
    private Preference mPdrPref;
    private Preference mGoogleGsturePref;
    private Preference mCompositePref;

    public SensorSettings() {
        super(Utils.KEY_ANSWER_CALL_STATUS);
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        addPreferencesFromResource(R.xml.sensorhub_settings_pref);
        addSensorsList();
        initActionButton();
    }

    @Override
    protected void onResume() {
        super.onResume();
        Utils.acquireScreenWakeLock(this);
        updatePreferenceStatus();
        // For Gesture wake up
        Intent intent = getIntent();
        if (ActionDataResult.hasResult(intent)) {
            ActionDataResult result = ActionDataResult.extractResult(intent);
            mRequestId = result.getRequestId();
        }
    }

    @Override
    protected void onPause() {
        Utils.releaseScreenWakeLock();
        super.onPause();
    }

    private void initActionButton() {
        mActionBarSwitch = new Switch(getLayoutInflater().getContext());
        final int padding = getResources().getDimensionPixelSize(R.dimen.action_bar_switch_padding);
        mActionBarSwitch.setPaddingRelative(0, 0, padding, 0);
        getActionBar().setDisplayOptions(
                ActionBar.DISPLAY_HOME_AS_UP | ActionBar.DISPLAY_SHOW_CUSTOM,
                ActionBar.DISPLAY_HOME_AS_UP | ActionBar.DISPLAY_SHOW_CUSTOM);
        getActionBar()
                .setCustomView(
                        mActionBarSwitch,
                        new ActionBar.LayoutParams(ActionBar.LayoutParams.WRAP_CONTENT,
                                ActionBar.LayoutParams.WRAP_CONTENT, Gravity.CENTER_VERTICAL
                                        | Gravity.END));
        getActionBar().setDisplayHomeAsUpEnabled(false);
        mActionBarSwitch.setChecked(Utils.getSensorStatus(Utils.LOG_STATUS));
        mActionBarSwitch.setOnCheckedChangeListener(this);
    }

    private void addSensorsList() {
        mSensorHubManager = (SensorHubManager) getSystemService(SensorHubManager.SENSORHUB_SERVICE);
        if (mSensorHubManager == null) {
            Log.d(TAG, "mSensorHubManager == null");
        }
        // Add GestureWake when support. hide
        if (mSensorHubManager != null &&
	        mSensorHubManager.isContextSupported(ContextInfo.Type.GESTURE)) {
	            Utils.createPreference(Utils.TYPE_SWITCH,
			            getString(R.string.gesture_wake_up),
			            KEY_GESTURE_WAKE_UP, getPreferenceScreen(), this);
        }

        // Add sensor list
        mAccelePref =
            (SwitchPreference) getPreference(Utils.TYPE_SWITCH, Sensor.TYPE_ACCELEROMETER);
        mLightPref = (SwitchPreference) getPreference(Utils.TYPE_SWITCH, Sensor.TYPE_LIGHT);
        mProximityPref = (SwitchPreference) getPreference(Utils.TYPE_SWITCH, Sensor.TYPE_PROXIMITY);
        mAnswerCall = (SwitchPreference) getPreference(Utils.TYPE_SWITCH, Sensor.TYPE_ANSWER_CALL);
        mInPocketPref = getPreference(Utils.TYPE_PREFERENCE, Sensor.TYPE_IN_POCKET);
        mPedometerPref = getPreference(Utils.TYPE_PREFERENCE, Sensor.TYPE_PEDOMETER);
        mAcitvityPref = getPreference(Utils.TYPE_PREFERENCE, Sensor.TYPE_ACTIVITY);
        mSmdPref = getPreference(Utils.TYPE_PREFERENCE, Sensor.TYPE_SIGNIFICANT_MOTION);
        mPdrPref = getPreference(Utils.TYPE_PREFERENCE, Sensor.TYPE_PDR);

        // Add Google Geatures entrance
        mGoogleGsturePref = Utils.createPreference(Utils.TYPE_PREFERENCE,
                getString(R.string.orginal_gesture_title), KEY_GOOGLE_GESTURE,
                getPreferenceScreen(), this);
        // Add composite entrance
        mCompositePref = Utils.createPreference(Utils.TYPE_PREFERENCE,
                getString(R.string.composite_sensor_title), KEY_COMPOSITE_PREF,
                getPreferenceScreen(), this);
    }

    private Preference getPreference(int prefType, int sensorType) {
        Sensor sensor = mSensorKeyMap.get(sensorType);
        if (sensor != null) {
            return Utils.createPreference(prefType, sensor.getName(), String.valueOf(sensorType),
                    getPreferenceScreen(), this);
        } else {
            return null;
        }
    }

    private void updatePreferenceStatus() {
        updatePreferenceStatus(mAccelePref, Utils.KEY_ACCELEROMETER_STATUS);
        updatePreferenceStatus(mLightPref, Utils.KEY_LIGHT_STATUS);
        updatePreferenceStatus(mProximityPref, Utils.KEY_PROXIMITY_STATUS);
        updatePreferenceStatus(mAnswerCall, Utils.KEY_ANSWER_CALL_STATUS);
        updatePreferenceStatus(mInPocketPref, Utils.KEY_INPOCKET_STATUS);
        updatePreferenceStatus(mAcitvityPref, Utils.KEY_ACTIVITY_STATUS);
        updatePreferenceStatus(mPedometerPref, Utils.KEY_PEDOMETER_STATUS);
        updatePreferenceStatus(mPdrPref, Utils.KEY_PDR_STATUS);
        updatePreferenceStatus(mSmdPref, Utils.KEY_SMD_STATUS);
        mGoogleGsturePref
                .setSummary(Utils.getMultiSensorsStatus(Utils.orginalGestureType)
                        ? R.string.running_summary : R.string.space_summary);
        mCompositePref
                .setSummary(Utils.getMultiSensorsStatus(Utils.compositeType)
                        ? R.string.running_summary : R.string.space_summary);
    }

    private void updatePreferenceStatus(Preference pref, String statusKey) {
        boolean status = Utils.getSensorStatus(statusKey);
        if (pref != null) {
            if (pref instanceof SwitchPreference) {
                SwitchPreference prefer = (SwitchPreference) pref;
                prefer.setChecked(status);
            } else if (pref instanceof Preference) {
                pref.setSummary(status ? R.string.running_summary : R.string.space_summary);
            }
        }
    }

    public boolean onPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
        Log.d(TAG, "onPreferenceTreeClick : " + preference.getTitle());
        Intent intent = new Intent();
        if (preference == mGoogleGsturePref) {
            intent.setClass(this, OrginalGesturesActivity.class);
        } else if (preference == mInPocketPref) {
            intent.setClass(this, InPocketSensorActivity.class);
        } else if (preference == mPedometerPref) {
            intent.setClass(this, PedometerSensorActivity.class);
        } else if (preference == mAcitvityPref) {
            intent.setClass(this, ActivitySensorActivity.class);
        } else if (preference == mSmdPref) {
            intent.setClass(this, SmdSensorActivity.class);
        } else if (preference == mCompositePref) {
            intent.setClass(this, CompositeSensorActivity.class);
        } else if (preference == mPdrPref) {
            intent.setClass(this, PdrSensorActivity.class);
        } else {
            intent = null;
        }
        if (intent != null) {
            startActivity(intent);
        }
        return super.onPreferenceTreeClick(preferenceScreen, preference);
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        Log.d(TAG, "onPreferenceChange : " + preference.getTitle());
        boolean bNewValue = (Boolean) newValue;
        if (preference instanceof SwitchPreference) {
            if (KEY_GESTURE_WAKE_UP.equals(preference.getKey())) {
                if (bNewValue) {
                    mSensorHubManager.enableGestureWakeup(true);
                    setupAction();
                } else {
                    cancelAction(mRequestId);
                    mSensorHubManager.enableGestureWakeup(false);
                }
            } else {
                if (preference == mAccelePref) {
                    Utils.setSensorStatus(Utils.KEY_ACCELEROMETER_STATUS, bNewValue);
                } else if (preference == mLightPref) {
                    Utils.setSensorStatus(Utils.KEY_LIGHT_STATUS, bNewValue);
                } else if (preference == mProximityPref) {
                    Utils.setSensorStatus(Utils.KEY_PROXIMITY_STATUS, bNewValue);
                } else if (preference == mAnswerCall) {
                    Utils.setSensorStatus(Utils.KEY_ANSWER_CALL_STATUS, bNewValue);
                }
                if (mBound) {
                    mSensorService.registerSensor(preference.getKey(), bNewValue);
                }
            }
        }
        return true;
    }

    @Override
    public void onCheckedChanged(CompoundButton arg0, boolean arg1) {
        // enable or disable log
        Utils.setSensorStatus(Utils.LOG_STATUS, (Boolean) arg1);
        if (mBound) {
            mSensorService.recordLogs((Boolean) arg1);
        }
    }

    private void setupAction() {
        Intent intent = new Intent(this, SensorSettings.class);
        PendingIntent callbackIntent = PendingIntent.getActivity(this, 0, intent,
                PendingIntent.FLAG_UPDATE_CURRENT);

        Action action = new Action(callbackIntent, true, false);
        Condition.Builder builder = new Condition.Builder();
        Condition condition = builder.createCondition(ContextInfo.Gesture.VALUE,
                Condition.OP_EQUALS, ContextInfo.Gesture.Type.GES_DOUBLE_TAP);
        mRequestId = mSensorHubManager.requestAction(condition, action);
        Log.d(TAG, "requestGesture<<< requestid=" + mRequestId);
    }

    private void cancelAction(int requestId) {
        if (mRequestId != 0) {
            boolean result = mSensorHubManager.cancelAction(requestId);
            Log.d(TAG, "Cancel request success with id " + requestId + " result is " + result);
        }
    }

    @Override
    public void onSensorChanged(float[] value) {
        mAnswerCall.setSummary(String.valueOf(value[0]));
    }
}
