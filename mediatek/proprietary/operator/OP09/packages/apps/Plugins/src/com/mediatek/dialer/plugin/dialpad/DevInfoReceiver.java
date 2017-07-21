package com.mediatek.dialer.plugin.dialpad;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.os.Bundle;
import android.util.Log;

public class DevInfoReceiver extends BroadcastReceiver {
    private static final String TAG = "DevInfoReceiver";
    public  static final String EXTRA_KEY_IMEI = "extra_key_imei";
    public  static final String EXTRA_KEY_MEID = "extra_key_meid";

    private static final String PREF_DEVINFO_TAG = "pref_devinfo";
    public  static final String PREF_IMEI_ITEM = "pref_imei_item";
    public  static final String PREF_IMEI_MEID = "pref_imei_meid";
    public  static final String BROADCAST_ACTION = "intent_action_imei_meid";

    @Override
    public void onReceive(Context context, Intent intent) {
        if (!intent.getAction().equals(BROADCAST_ACTION)) {
            Log.d(TAG, "DevInfoReceiver, action = " + intent.getAction());
            return;
        }

        SharedPreferences sharedPrefs = context.getSharedPreferences(
                PREF_DEVINFO_TAG, Context.MODE_WORLD_WRITEABLE | Context.MODE_WORLD_READABLE);
        String meid = intent.getStringExtra(EXTRA_KEY_MEID);
        sharedPrefs.edit().putString(PREF_IMEI_MEID, meid).commit();

        Bundle bundle = intent.getExtras();
        if (bundle == null) {
            Log.d(TAG, "DevInfoReceiver, extra bundle is null");
            return;
        }

        String[] imei = bundle.getStringArray(EXTRA_KEY_IMEI);
        for (int i = 0; i < imei.length; i ++) {
            Log.d(TAG, "DevInfoReceiver, imei = " + imei[i]);
            sharedPrefs.edit().putString(PREF_IMEI_ITEM + (i + 1), imei[i]).commit();
        }
        Log.d(TAG, "DevInfoReceiver, meid = " + meid);
    }
}