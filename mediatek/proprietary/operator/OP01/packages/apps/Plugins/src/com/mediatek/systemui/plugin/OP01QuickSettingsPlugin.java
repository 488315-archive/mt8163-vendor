package com.mediatek.systemui.plugin;

import android.content.Context;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.mediatek.common.PluginImpl;
import com.mediatek.op01.plugin.R;
import com.mediatek.systemui.ext.DefaultQuickSettingsPlugin;

/**
 * Customize carrier text.
 */
@PluginImpl(interfaceName = "com.mediatek.systemui.ext.IQuickSettingsPlugin")
public class OP01QuickSettingsPlugin extends DefaultQuickSettingsPlugin {
    public static final String TAG = "OP01QuickSettingsPlugin";

    /**
     * Constructs a new OP01QuickSettingsPlugin instance with Context.
     * @param context A Context object
     */
    public OP01QuickSettingsPlugin(Context context) {
        super(context);
    }

    @Override
    public boolean customizeDisplayDataUsage(boolean isDisplay) {
        Log.i(TAG, "customizeDisplayDataUsage, " + " return true");
        return true;
    }

    @Override
    public String customizeQuickSettingsTileOrder(String defaultString) {
        if (TelephonyManager.getDefault().getPhoneCount() <= 1) {
            return mContext.getString(R.string.quick_settings_tiles_Single_Sim_default);
        } else {
            return mContext.getString(R.string.quick_settings_tiles_Dual_Sim_default);
        }
    }
}
