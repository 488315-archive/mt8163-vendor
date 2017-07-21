package com.mediatek.crossmountsettings;

import android.app.Application;

/**
 * Application class for SystemUI.
 */
public class CrossMountApplication extends Application {
    private static int countActiveActivity = 0;
    @Override
    public void onCreate() {
        super.onCreate();
    }

    /**
     * if entrance a activity, add it.
     */
    public static void addActiveActivity() {
        countActiveActivity++;
    }

    /**
     * if exit a activity, decrease it.
     */
    public static void deCreaseActiveActivity() {
     countActiveActivity--;
    }

    /**
     * Get the count.
     * @return the active activity count
     */
    public int getActiveActivityCount() {
      return countActiveActivity;
    }
}
