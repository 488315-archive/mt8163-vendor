package com.mediatek.sensorhub.service;

import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.hardware.TriggerEventListener;
import android.hardware.TriggerEvent;
import android.media.AudioManager;
import android.media.SoundPool;
import android.os.Binder;
import android.os.IBinder;
import android.text.TextUtils;
import android.util.Log;

import com.mediatek.sensorhub.ui.R;
import com.mediatek.sensorhub.settings.Utils;

import java.util.Arrays;
import java.util.HashMap;

public class SensorEventListenerService extends Service implements SensorEventListener {
    private static final String TAG = "SensorEventListenerService";
    private SensorManager mSensorManager;
    private TriggerListener mTriggerListener = new TriggerListener();
    private HashMap<Integer, Sensor> mSensorKeyMap = new HashMap<Integer, Sensor>();
    // record log
    private String mRecordStr;

    // Play notify @{
    private SoundPool mSounds;
    private int mSoundIdF;
    private int mSoundIdT;
    private int mSoundStreamId;
    private AudioManager mAudioManager;
    private int mUiSoundsStreamType;
    // @}

    private final IBinder mBinder = new LocalBinder();

    public SensorEventListenerService() {

    }

    public interface OnSensorChangedListener {
        public void onSensorChanged(float[] value);

        public void onSensorChanged(int sensorType, float[] value);
    }

    private HashMap<String, OnSensorChangedListener> onSensorChangedListenerMap =
        new HashMap<String, OnSensorChangedListener>();

    public void putOnSensorChangedListener(String key, OnSensorChangedListener listener) {
        onSensorChangedListenerMap.put(key, listener);
    }

    public void removeOnSensorChangedListener(String key) {
        onSensorChangedListenerMap.remove(key);
    }

    /**
     * Class used for the client Binder. Because we know this service always
     * runs in the same process as its clients, we don't need to deal with IPC.
     */
    public class LocalBinder extends Binder {
        public SensorEventListenerService getService(String key, OnSensorChangedListener listener) {
            putOnSensorChangedListener(key, listener);
            // Return this instance of SensorEventListenerService so clients can
            // call public methods
            return SensorEventListenerService.this;
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.d(TAG, "onReceive: " + intent.getAction());
            stopSelf();
        }
    };

    @Override
    public void onCreate() {
        super.onCreate();
        Log.d(TAG, "onCreate");
        mSensorManager = Utils.getSensorManager();
        mSensorKeyMap = Utils.getSensorKeyMap();

        mAudioManager = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
        mSounds = new SoundPool(1, AudioManager.STREAM_NOTIFICATION, 0);
        mSoundIdF = mSounds.load(this, R.raw.in_pocket, 0);
        mSoundIdT = mSounds.load(this, R.raw.non_in_pocket, 0);

        IntentFilter filter = new IntentFilter();
        filter.addAction(Intent.ACTION_SHUTDOWN);
        registerReceiver(mReceiver, filter);
    }

    @Override
    public void onStart(Intent intent, int startId) {
        super.onStart(intent, startId);
    }

    @Override
    public void onDestroy() {
        Log.d(TAG, "onDestroy");
        unRegisterAllSensors();
        Utils.restoreStatusToDefult();
        recordLogs(false);
        unregisterReceiver(mReceiver);
        super.onDestroy();
    }

    // Register or unRegister sensor
    public void registerSensor(String prefKey, boolean isRegister) {
        Log.d(TAG, "registerSensor " + prefKey + " status " + isRegister);
        // Preference key is the sensor type
        int sensorType = Integer.valueOf(prefKey);
        Sensor sensor = mSensorKeyMap.get(sensorType);
        if (Utils.containsSensor(Utils.triggerSensorType, sensorType)) {
            if (isRegister) {
                mSensorManager.requestTriggerSensor(mTriggerListener, sensor);
            } else {
                mSensorManager.cancelTriggerSensor(mTriggerListener, sensor);
            }
        } else {
            if (isRegister) {
                mSensorManager.registerListener((SensorEventListener) this, sensor,
                        SensorManager.SENSOR_DELAY_NORMAL);
            } else {
                mSensorManager.unregisterListener(this);
            }
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {

    }

    @Override
    public void onSensorChanged(SensorEvent event) {
        // For record log
        String pRecordStr = event.sensor.getName() + "@" + Utils.getTimeString()
                + Arrays.toString(event.values);
        mRecordStr = mRecordStr + "\n" + pRecordStr;
        int sensorType = event.sensor.getType();
        OnSensorChangedListener listener = null;
        String sensorTypeKeyStr = null;
        switch (sensorType) {
        case Sensor.TYPE_PEDOMETER:
            sensorTypeKeyStr = Utils.KEY_PEDOMETER_STATUS;
            break;
        case Sensor.TYPE_ACTIVITY:
            sensorTypeKeyStr = Utils.KEY_ACTIVITY_STATUS;
            break;
        case Sensor.TYPE_IN_POCKET:
            sensorTypeKeyStr = Utils.KEY_INPOCKET_STATUS;
            notifyUser(Utils.KEY_INPOCKET_NOTIFY_STATUS, event.values[0]);
            break;
        default:
            break;
        }
        if (sensorTypeKeyStr != null) {
            listener = onSensorChangedListenerMap.get(sensorTypeKeyStr);
            if (listener != null) {
                listener.onSensorChanged(event.values);
            }
        }
        // Composite sensors
        if (Utils.isCompositeSesnor(sensorType)) {
            listener = onSensorChangedListenerMap.get(Utils.KEY_COMPOSITE_STATUS);
            if (listener != null) {
                listener.onSensorChanged(sensorType, event.values);
            }
        }
    }

    class TriggerListener extends TriggerEventListener {
        public void onTrigger(TriggerEvent event) {
            String pRecordStr = event.sensor.getName() + "@" + Utils.getTimeString()
                    + Arrays.toString(event.values);
            mRecordStr = mRecordStr + "\n" + pRecordStr;
            int sensorType = event.sensor.getType();
            OnSensorChangedListener listener = null;
            String sensorTypeKeyStr = null;
            switch (sensorType) {
            case Sensor.TYPE_SIGNIFICANT_MOTION:
                sensorTypeKeyStr = Utils.KEY_SMD_STATUS;
                notifyUser(Utils.KEY_SMD_NOTIFY_STATUS, event.values[0]);
                break;
            case Sensor.TYPE_ANSWER_CALL:
                sensorTypeKeyStr = Utils.KEY_ANSWER_CALL_STATUS;
                if (event.values[0] == 1) {
                    playSound(mSoundIdF);
                }
                break;
            default:
                break;
            }
            if (sensorTypeKeyStr != null) {
                listener = onSensorChangedListenerMap.get(sensorTypeKeyStr);
                if (listener != null) {
                    listener.onSensorChanged(event.values);
                }
            }
            if (Utils.isOrginalGesture(sensorType)) {
                if (event.values[0] == 1) {
                    playSound(mSoundIdF);
                }
            }
            if (Sensor.TYPE_SIGNIFICANT_MOTION == sensorType) {
                try {
                    Thread.sleep(1000); // SMD sleep 1s
                } catch (InterruptedException e) {
                    Log.d(TAG, "interrupt");
                }
            }
            mSensorManager.requestTriggerSensor(mTriggerListener, event.sensor);
        }
    }

    // notify user when sensor event change
    private void notifyUser(String key, float value) {
        if (Utils.getSensorStatus(key)) {
            if (value == 1) {
                playSound(mSoundIdF);
            }
        }
    }

    private void playSound(int soundId) {
        mSounds.stop(mSoundStreamId);
        if (mAudioManager != null) {
            mUiSoundsStreamType = mAudioManager.getUiSoundsStreamType();
        }
        // If the stream is muted, don't play the sound
        if (mAudioManager.isStreamMute(mUiSoundsStreamType))
            return;
        mSoundStreamId = mSounds.play(soundId, 1, 1, 1/* priortiy */, 0/* loop */, 1.0f/* rate */);
    }

    private void unRegisterAllSensors() {
        mSensorManager.unregisterListener((SensorEventListener) this);
    }

    public void recordLogs(boolean isRecord) {
        if (isRecord) {
            Log.d(TAG, "sensor algorithm test start");
            Utils.initRecordFileName();
            mRecordStr = "";
        } else {
            Log.d(TAG, "sensor algorithm test end");
            if (!TextUtils.isEmpty(mRecordStr)) {
                Utils.recordToSdcard(mRecordStr.getBytes());
            }
        }
        return;
    }
}
