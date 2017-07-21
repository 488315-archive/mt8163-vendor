package com.mediatek.smartmotion.enabler;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import android.content.Context;
import android.content.SharedPreferences;


public class Preferences {
    public static final String PREFERENCES_FILE = "MediatekSmartMotion.Main";

    private static final String ENABLE_QUICK_ANSWER = "enableQuickAnswer";
    private static final String ENABLE_EASY_REJECT = "enableEasyReject";
    private static final String ENABLE_SMART_SILENT = "enableSmartSilent";
    private static final String ENABLE_IN_POCKET= "enableInPocket";
    private static final String ENABLE_PEDOMETER = "enablePedometer";
    private static final String ENABLE_USER_ACTIVITY = "enableUserActivity";
    private static final String ENABLE_SIGNIFICANT_SMD = "enableSignificantMotion";
    private static final String ENABLE_FREE_FALL = "enableFreeFall";
    private static final String ENABLE_SNAPSHOT = "enableSnapshot";
    private static final String ENABLE_PDR = "enablePdr";

    private static final String DEMO_MODE = "demo mode";
    private static final String CONFIGURED_GESTURE = "configuredGesture";
    private static final String DELIMITER = ",";

    private static Preferences sPreferences;
    private final SharedPreferences mSharedPreferences;

    private Preferences(Context context) {
        mSharedPreferences = context.getSharedPreferences(PREFERENCES_FILE, Context.MODE_PRIVATE);
    }

    public static synchronized Preferences getPreferences(Context context) {
        if (sPreferences == null) {
            sPreferences = new Preferences(context);
        }
        return sPreferences;
    }

    public static SharedPreferences getSharedPreferences(Context context) {
        return getPreferences(context).mSharedPreferences;
    }

    public void setQuickAnswer(boolean value) {
        mSharedPreferences.edit().putBoolean(ENABLE_QUICK_ANSWER, value).apply();
    }

    public boolean getQuickAnswer() {
        return mSharedPreferences.getBoolean(ENABLE_QUICK_ANSWER, false);
    }

    public void setEasyReject(boolean value) {
        mSharedPreferences.edit().putBoolean(ENABLE_EASY_REJECT, value).apply();
    }

    public boolean getEasyReject() {
        return mSharedPreferences.getBoolean(ENABLE_EASY_REJECT, false);
    }

    public void setInPocket(boolean value) {
        mSharedPreferences.edit().putBoolean(ENABLE_IN_POCKET, value).apply();
    }

    public boolean getInPocket() {
        return mSharedPreferences.getBoolean(ENABLE_IN_POCKET, false);
    }


    public void setSmartSilent(boolean value) {
        mSharedPreferences.edit().putBoolean(ENABLE_SMART_SILENT, value).apply();
    }

    public boolean getSmartSilent() {
        return mSharedPreferences.getBoolean(ENABLE_SMART_SILENT, false);
    }

    public void setPedometer(boolean value) {
        mSharedPreferences.edit().putBoolean(ENABLE_PEDOMETER, value).apply();
    }

    public boolean getPedometer() {
        return mSharedPreferences.getBoolean(ENABLE_PEDOMETER, false);
    }

    public void setUserActivity(boolean value) {
        mSharedPreferences.edit().putBoolean(ENABLE_USER_ACTIVITY, value).apply();
    }

    public boolean getUserActivity() {
        return mSharedPreferences.getBoolean(ENABLE_USER_ACTIVITY, false);
    }

    public void setSignificantMotion(boolean value) {
        mSharedPreferences.edit().putBoolean(ENABLE_SIGNIFICANT_SMD, value).apply();
    }

    public boolean getSignificantMotion() {
        return mSharedPreferences.getBoolean(ENABLE_SIGNIFICANT_SMD, false);
    }

    public void setFreeFall(boolean value) {
        mSharedPreferences.edit().putBoolean(ENABLE_FREE_FALL, value).apply();
    }

    public boolean getFreeFall() {
        return mSharedPreferences.getBoolean(ENABLE_FREE_FALL, false);
    }

    public void setSnapshot(boolean value) {
        mSharedPreferences.edit().putBoolean(ENABLE_SNAPSHOT, value).apply();
    }

    public boolean getSnapshot() {
        return mSharedPreferences.getBoolean(ENABLE_SNAPSHOT, false);
    }

    public void setPdr(boolean value) {
        mSharedPreferences.edit().putBoolean(ENABLE_PDR, value).apply();
    }

    public boolean getPdr() {
        return mSharedPreferences.getBoolean(ENABLE_PDR, false);
    }

    public void setDemoMode(boolean value) {
        mSharedPreferences.edit().putBoolean(DEMO_MODE, value).apply();
    }

    public boolean getDemoMode() {
        return mSharedPreferences.getBoolean(DEMO_MODE, true);
    }

    public void setConfiguredGesture(List<String> gestureList) {
        String gestures = null;
        StringBuilder sBuilder = new StringBuilder();
        for(String gesture : gestureList) {
            sBuilder.append(gesture).append(DELIMITER);
        }
        if (sBuilder.length() != 0) {
            sBuilder.deleteCharAt(sBuilder.length()-1);
        }
        gestures = sBuilder.toString();
        mSharedPreferences.edit().putString(CONFIGURED_GESTURE, gestures).apply();
    }

    public List<String> getConfiguredGesture() {
        String gestures = mSharedPreferences.getString(CONFIGURED_GESTURE, "");
        List<String> gestureList = new ArrayList<String>();
        if (!gestures.isEmpty()) {
            String[] gestureArray = gestures.split(DELIMITER);
            if (gestureArray != null || gestureArray.length != 0) {
                gestureList = new ArrayList<String>(Arrays.asList(gestureArray));
            }
        }
        return gestureList;
    }
}
