package com.mediatek.sensorhub.settings;

import android.content.Context;
import android.content.SharedPreferences;
import android.hardware.Sensor;
import android.hardware.SensorManager;
import android.os.PowerManager;
import android.os.storage.StorageManager;
import android.preference.Preference;
import android.preference.PreferenceScreen;
import android.preference.Preference.OnPreferenceChangeListener;
import android.preference.SwitchPreference;
import android.text.format.DateFormat;
import android.util.Log;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class Utils {

    private static final String TAG = "SensorHubUtils";
    // for add preference to screen @ {
    public static final int TYPE_PREFERENCE = 0;
    public static final int TYPE_SWITCH = 1;
    // @ }
    // For restore status in SharedPreference @
    public static final String SHARED_PREF_SENSOR_HUB = "shared_sensorhub";
    // SMD status
    public static final String KEY_SMD_STATUS = "smd_status";
    public static final String KEY_SMD_NOTIFY_STATUS = "smd_notify_status";
    // Activity status
    public static final String KEY_ACTIVITY_STATUS = "activity_status";
    // Pedometer status
    public static final String KEY_PEDOMETER_STATUS = "pedometer_status";
    public static final String KEY_PRE_TOTAL_STEPS = "pre_total_steps";
    public static final String KEY_PRE_TOTAL_DISTANCE = "pre_total_distance";
    // In pocket status
    public static final String KEY_INPOCKET_STATUS = "inpocket_status";
    public static final String KEY_INPOCKET_NOTIFY_STATUS = "inpocket_notify_status";
    // ACCELEROMETER status
    public static final String KEY_ACCELEROMETER_STATUS = "accelerometer_status";
    // LIGHT status
    public static final String KEY_LIGHT_STATUS = "light_status";
    // PROXIMITY status
    public static final String KEY_PROXIMITY_STATUS = "proximity_status";
    // PDR status
    public static final String KEY_PDR_STATUS = "pdr_status";
    // Composite status
    public static final String KEY_COMPOSITE_STATUS = "composite_status";
    // Answer call status
    public static final String KEY_ANSWER_CALL_STATUS = "answer_call_status";
    // Log status
    public static final String LOG_STATUS = "log_status";
    // @}
    // for Sensor @{
    public static final int[] triggerSensorType = { Sensor.TYPE_WAKE_GESTURE,
            Sensor.TYPE_GLANCE_GESTURE, Sensor.TYPE_PICK_UP_GESTURE,
            Sensor.TYPE_SIGNIFICANT_MOTION, Sensor.TYPE_ANSWER_CALL };
    public static final int[] compositeType = { Sensor.TYPE_GRAVITY,
            Sensor.TYPE_LINEAR_ACCELERATION, Sensor.TYPE_ROTATION_VECTOR,
            Sensor.TYPE_MAGNETIC_FIELD_UNCALIBRATED, Sensor.TYPE_GAME_ROTATION_VECTOR,
            Sensor.TYPE_GYROSCOPE_UNCALIBRATED, Sensor.TYPE_GEOMAGNETIC_ROTATION_VECTOR,
            Sensor.TYPE_ACCELEROMETER_UNCALIBRATED };
    public static final int[] orginalGestureType = { Sensor.TYPE_WAKE_GESTURE,
            Sensor.TYPE_GLANCE_GESTURE, Sensor.TYPE_PICK_UP_GESTURE };
    private static SensorManager mSensorManager;
    private static List<Sensor> mSensorsList;
    // @}
    private static PowerManager.WakeLock sScreenOnWakeLock;
    // for log @{
    private static StorageManager mStorageManager;
    private static String mExternalStoragePath;
    // @}
    private static Utils sUtilsPlus;
    private static Context mContext;

    private Utils(Context context) {
        mContext = context;
        mSensorManager = (SensorManager) mContext.getSystemService(Context.SENSOR_SERVICE);
        mStorageManager = (StorageManager) mContext.getSystemService(Context.STORAGE_SERVICE);
        mSensorsList = mSensorManager.getSensorList(Sensor.TYPE_ALL);
        Log.d(TAG, "mSensorsList size : " + mSensorsList.size());
    }

    public static synchronized Utils getInstance(Context context) {
        if (sUtilsPlus == null) {
            Log.d(TAG, "new UtilsPlus");
            sUtilsPlus = new Utils(context);
        }
        return sUtilsPlus;
    }

    public static SensorManager getSensorManager() {
        return mSensorManager;
    }

    public static List<Sensor> getSensorsList() {
        return mSensorsList;
    }

    public static HashMap<Integer, Sensor> getSensorKeyMap() {
        HashMap<Integer, Sensor> sensorKeyMap = new HashMap<Integer, Sensor>();
        List<Sensor> sensorList = getSensorsList();
        if (sensorList != null && sensorList.size() != 0) {
            for (Sensor sensor : sensorList) {
                if (sensor != null) {
                    int sensorType = sensor.getType();
                    sensorKeyMap.put(sensorType, sensor);
                }
            }
        }
        return sensorKeyMap;
    }

    // For restore status in SharedPreference @{
    public static void setSensorStatus(String key, boolean status) {
        SharedPreferences.Editor editor = getSharedPreferences(mContext, SHARED_PREF_SENSOR_HUB)
                .edit();
        editor.putBoolean(key, status);
        editor.apply();
    }

    public static boolean getSensorStatus(String sensorType) {
        return getSharedPreferences(mContext, SHARED_PREF_SENSOR_HUB).getBoolean(sensorType, false);
    }

    public static void setSensorValues(String key, float value) {
        SharedPreferences.Editor editor = getSharedPreferences(mContext, SHARED_PREF_SENSOR_HUB)
                .edit();
        editor.putFloat(key, value);
        editor.apply();
    }

    public static float getSensorValues(String valueKey) {
        return getSharedPreferences(mContext, SHARED_PREF_SENSOR_HUB).getFloat(valueKey, 0.0f);
    }

    public static SharedPreferences getSharedPreferences(Context context, String name) {
        return context.getSharedPreferences(name, Context.MODE_PRIVATE);
    }

    public static void restoreStatusToDefult() {
        // restore SMD
        setSensorStatus(KEY_SMD_STATUS, false);
        setSensorStatus(KEY_SMD_NOTIFY_STATUS, false);
        // restore Activity
        setSensorStatus(KEY_ACTIVITY_STATUS, false);
        // restore Pedometer
        setSensorStatus(KEY_PEDOMETER_STATUS, false);
        setSensorValues(KEY_PRE_TOTAL_DISTANCE, 0.0f);
        setSensorValues(KEY_PRE_TOTAL_STEPS, 0.0f);
        // restore Inpocket
        setSensorStatus(KEY_INPOCKET_STATUS, false);
        setSensorStatus(KEY_INPOCKET_NOTIFY_STATUS, false);
        // restore gesture
        for (int type : Utils.orginalGestureType) {
            setSensorStatus(String.valueOf(type), false);
        }
        // restore hardware sensors
        setSensorStatus(KEY_ACCELEROMETER_STATUS, false);
        setSensorStatus(KEY_LIGHT_STATUS, false);
        setSensorStatus(KEY_PROXIMITY_STATUS, false);
        // restore pdr
        setSensorStatus(KEY_PDR_STATUS, false);
        // restore log
        setSensorStatus(LOG_STATUS, false);
        // restore composite sensors
        for (int type : Utils.compositeType) {
            setSensorStatus(String.valueOf(type), false);
        }
    }

    // @}
    // For record logs @{
    public static void initRecordFileName() {
        String sensorTestDirStr = mStorageManager.getPrimaryVolume().getPath() + "/MtkSensorTest/";
        File sensorTestDir = new File(sensorTestDirStr);
        boolean makeDir = false;
        if (!sensorTestDir.isDirectory()) {
            makeDir = sensorTestDir.mkdirs();
        }
        mExternalStoragePath = sensorTestDirStr + "sensor_test_" + getTimeString() + ".txt";
        Log.d(TAG, "mExternalStoragePath1: " + mExternalStoragePath + " makeDir: " + makeDir);
    }

    public static void recordToSdcard(byte[] data) {
        FileOutputStream fos = null;
        try {
            fos = new FileOutputStream(mExternalStoragePath, true);
            fos.write(data);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            Log.w(TAG, "FileNotFoundException " + e.getMessage());
            return;
        } catch (IOException e) {
            Log.w(TAG, "IOException " + e.getMessage());
            return;
        } finally {
            try {
                if (fos != null) {
                    fos.flush();
                    fos.close();
                }
            } catch (IOException e2) {
                Log.w(TAG, "IOException " + e2.getMessage());
                return;
            }
        }
    }

    public static String getTimeString() {
        return DateFormat.format("yyyy-MM-dd-kk-mm-ss", System.currentTimeMillis()).toString();
    }

    // @}
    // For screen wake lock @{
    public static void acquireScreenWakeLock(Context context) {
        Log.d(TAG, "Acquiring screen on and cpu wake lock");
        if (sScreenOnWakeLock != null) {
            return;
        }

        PowerManager pm = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
        sScreenOnWakeLock = pm
                .newWakeLock(PowerManager.FULL_WAKE_LOCK | PowerManager.ACQUIRE_CAUSES_WAKEUP
                        | PowerManager.ON_AFTER_RELEASE, "SchPwrOnOff");
        sScreenOnWakeLock.acquire();
    }

    public static void releaseScreenWakeLock() {
        if (sScreenOnWakeLock != null) {
            sScreenOnWakeLock.release();
            sScreenOnWakeLock = null;
        }
    }

    // @}

    // for create preference @{
    public static Preference createPreference(int type, String title, String key,
            PreferenceScreen screen, Context context) {
        Preference preference = null;
        switch (type) {
        case TYPE_PREFERENCE:
            preference = new Preference(context);
            break;
        case TYPE_SWITCH:
            preference = new SwitchPreference(context);
            preference.setOnPreferenceChangeListener((OnPreferenceChangeListener) context);
            break;
        default:
            break;
        }
        preference.setKey(key);
        preference.setTitle(title);
        screen.addPreference(preference);
        return preference;
    }

    // @}

    public static boolean isCompositeSesnor(int sensorType) {
        return containsSensor(compositeType, sensorType);
    }

    public static boolean isOrginalGesture(int sensorType) {
        return containsSensor(orginalGestureType, sensorType);
    }

    // Return whether the array is contains the sensor type
    public static boolean containsSensor(int[] sensorTypes, int type) {
        List<Integer> sensors = new ArrayList();
        for (int i = 0; i < sensorTypes.length; i++) {
            sensors.add(sensorTypes[i]);
        }
        return sensors.contains(type);
    }

    public static boolean getMultiSensorsStatus(int[] sensorTypes){
        boolean status = false;
        for (int type : sensorTypes) {
            if (Utils.getSensorStatus(String.valueOf(type))) {
                status = true;
                break;
            }
        }
        return status;
    }
}
