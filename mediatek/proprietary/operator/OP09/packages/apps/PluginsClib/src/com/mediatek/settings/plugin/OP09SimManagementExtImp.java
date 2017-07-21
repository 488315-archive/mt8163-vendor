package com.mediatek.settings.plugin;

import android.content.Context;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.mediatek.common.PluginImpl;
import com.mediatek.settings.ext.DefaultSimManagementExt;

/**
 * For settings SIM management feature.
 */
@PluginImpl(interfaceName = "com.mediatek.settings.ext.ISimManagementExt")
public class OP09SimManagementExtImp extends DefaultSimManagementExt {
    private static final String TAG = "OP09ClibSimManagementExt";
    private Context mContext;

    /**
    * Constructor method.
    * @param context plugin context
    */
    public OP09SimManagementExtImp(Context context) {
        super();
        mContext = context;
    }

    /**
    * Change default data sim and enable data service on this sim.
    * @param subid the sub id of new default data sim
    */
    @Override
    public void setDataState(int subid) {
        TelephonyManager telephonyManager = TelephonyManager.from(mContext);
        SubscriptionManager subscriptionManager = SubscriptionManager.from(mContext);

        boolean enableBefore = telephonyManager.getDataEnabled();
        int resetSubId = subscriptionManager.getDefaultDataSubId();
        Log.d(TAG, "subid = " + subid + ", resetSubId = " + resetSubId
                + ", enableBefore = " + enableBefore);
        if (subscriptionManager.isValidSubscriptionId(subid) &&
            subid != resetSubId) {
            subscriptionManager.setDefaultDataSubId(subid);
            if (enableBefore) {
                telephonyManager.setDataEnabled(subid, true);
                telephonyManager.setDataEnabled(resetSubId, false);
            }
        }
    }

    /**
     * Called when SIM dialog is about to show for SIM info changed.
     * @return false if plug-in do not need SIM dialog
     */
    @Override
    public boolean isSimDialogNeeded() {
        Log.d(TAG, "op09 isSimDialogNeeded() return false.");
        return false;
    }

}