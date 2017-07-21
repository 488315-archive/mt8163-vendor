package com.mediatek.multiwindow.service;

import com.mediatek.multiwindow.service.MultiWindowApplication;

import android.os.IBinder;
import android.app.Service;
import android.os.ServiceManager;
import android.util.Log;
import android.content.Intent;

public class MultiWindowService extends Service {

    private static final String TAG = "MultiWindowService";

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.d(TAG, "onCreate()");

        try {
            ServiceManager.addService("multiwindow", new MultiWindowManager(this));
        } catch (Exception e) {
            Log.e(TAG, "Fail to register service, do nothing", e);
            return;
        }

        Log.d(TAG, "Service is opened successfully.");
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.d(TAG, "onDestroy(), check why ?");

    }
}