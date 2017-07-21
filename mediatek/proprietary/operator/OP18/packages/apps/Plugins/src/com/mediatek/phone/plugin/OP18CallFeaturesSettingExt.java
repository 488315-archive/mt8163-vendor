package com.mediatek.phone.plugin;

import android.content.Context;
import android.util.Log;
import com.mediatek.common.PluginImpl;

import com.mediatek.phone.ext.DefaultCallFeaturesSettingExt;

@PluginImpl(interfaceName="com.mediatek.phone.ext.ICallFeaturesSettingExt")
public class OP18CallFeaturesSettingExt extends DefaultCallFeaturesSettingExt {
    private static final String TAG = "OP18CallFeaturesSettingExt";
    
    @Override
    public boolean needShowOpenMobileDataDialog(Context context, int subId) {
        Log.d(TAG, "needShowOpenMobileDataDialog false");
        return false;
    }
}
