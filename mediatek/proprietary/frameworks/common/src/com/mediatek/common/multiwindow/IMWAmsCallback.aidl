package com.mediatek.common.multiwindow;

import android.app.PendingIntent;
import android.os.IBinder;
import android.os.WorkSource;
import android.content.ComponentName;

/**
 * System private API for talking with the activity manager service.
 *
 * {@hide}
 */
interface IMWAmsCallback {
    ///for AMS
    void restoreStack(in IBinder token,in boolean toMax);
    boolean moveActivityTaskToFront(in IBinder token);

    // Add for split-screen mode
    void switchMultiWindowMode();
}


