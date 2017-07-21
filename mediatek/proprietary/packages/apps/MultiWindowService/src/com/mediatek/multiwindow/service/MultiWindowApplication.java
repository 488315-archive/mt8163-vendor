package com.mediatek.multiwindow.service;

import android.app.Application;
import android.content.Intent;
import android.content.Context;
import android.util.Log;

public class MultiWindowApplication extends Application {

    private static final String TAG = "MultiWindowApplication";
    public static Context sContext;

    public MultiWindowApplication() {
    }

    @Override
    public void onCreate() {
        super.onCreate();
        sContext = this;

        Log.d(TAG, "start MultiWindow Manager Service");
        Intent startMWSIntent = new Intent(this, MultiWindowService.class);
        startService(startMWSIntent);

        Log.d(TAG, "start BlackList Service");
        Intent startBlSntent = new Intent(this, BlackListService.class);
        startService(startBlSntent);
    }

    public static Context getContext() {
        return sContext;
    }
}

