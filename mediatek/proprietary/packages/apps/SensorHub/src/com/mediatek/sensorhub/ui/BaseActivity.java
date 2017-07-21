package com.mediatek.sensorhub.ui;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.hardware.Sensor;
import android.os.IBinder;
import android.preference.PreferenceActivity;

import com.mediatek.sensorhub.service.SensorEventListenerService;
import com.mediatek.sensorhub.service.SensorEventListenerService.LocalBinder;
import com.mediatek.sensorhub.service.SensorEventListenerService.OnSensorChangedListener;
import com.mediatek.sensorhub.settings.Utils;

import java.util.HashMap;

public class BaseActivity extends PreferenceActivity implements OnSensorChangedListener {

    public SensorEventListenerService mSensorService;
    public boolean mBound = false;
    private String mListenerKey;
    public static final HashMap<Integer, Sensor> mSensorKeyMap = Utils.getSensorKeyMap();

    public BaseActivity(String key) {
        mListenerKey = key;
    }

    @Override
    protected void onStart() {
        super.onStart();
        // Bind to LocalService
        Intent intent = new Intent(this, SensorEventListenerService.class);
        bindService(intent, mConnection, Context.BIND_AUTO_CREATE);
    }

    @Override
    protected void onStop() {
        super.onStop();
        // Unbind from the service
        if (mBound) {
            unbindService(mConnection);
            mBound = false;
        }
    }

    /** Defines callbacks for service binding, passed to bindService() */
    private ServiceConnection mConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName className, IBinder service) {
            // We've bound to LocalService, cast the IBinder and get
            // LocalService instance
            LocalBinder binder = (LocalBinder) service;
            mSensorService = binder.getService(mListenerKey, BaseActivity.this);
            mBound = true;
        }

        @Override
        public void onServiceDisconnected(ComponentName arg0) {
            mSensorService.removeOnSensorChangedListener(mListenerKey);
            mBound = false;
        }
    };

    @Override
    public void onSensorChanged(float[] value) {
    }

    @Override
    public void onSensorChanged(int sensorType, float[] value) {
    }
}
