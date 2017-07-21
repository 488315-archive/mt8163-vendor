package com.mediatek.common.multiwindow;

import android.app.PendingIntent;
import android.os.IBinder;
import android.os.WorkSource;
import android.content.ComponentName;


/**
 * System private API for talking with the alarm manager service.
 *
 * {@hide}
 */
interface IMWBlackList {
    boolean shouldChangeConfig(in String packageName);
    boolean shouldRestartWhenMiniMax(in String packageName);

    List<String> getWhiteList();
    boolean inWhiteList(String packageName);
    void addIntoWhiteList(in String packageName);
    void addMoreIntoWhiteList(in List<String> packageList);
    void removeFromWhiteList(in String packageName);
}


