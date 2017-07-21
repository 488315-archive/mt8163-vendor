package com.mediatek.smartmotion.utils;

import android.util.Log;

public class LogUtils {
    public static String LOG_TAG = "SmartMotion";

    public static void v(String tag, String msg) {
        Log.v(LOG_TAG, " class: " + tag + " msg: " + msg);
    }

    public static void d(String tag, String msg) {
        Log.d(LOG_TAG, " class: " + tag + " msg: " + msg);
    }

    public static void i(String tag, String msg) {
        Log.i(LOG_TAG, " class: " + tag + " msg: " + msg);
    }

    public static void w(String tag, String msg) {
        Log.w(LOG_TAG, " class: " + tag + " msg: " + msg);
    }

    public static void e(String tag, String msg) {
        Log.e(LOG_TAG, " class: " + tag + " msg: " + msg);
    }

    public static void e(String tag, String msg, Throwable ex) {
        Log.e(LOG_TAG, " class: " + tag + " msg: " + msg, ex);
    }
}
