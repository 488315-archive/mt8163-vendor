package com.mediatek.commonpresence.plugin;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;

import android.util.Log;

import com.mediatek.common.PluginImpl;

//import com.mediatek.phone.ext.DefaultInCallUICommonPresenceExt;

/**
 * Plugin implementation for Phone Common Presence.
 */
@PluginImpl(interfaceName = "com.mediatek.phone.ext.IInCallUICommonPresenceExt")
public class OP08InCallUICommonPresenceExt /*
                                            * extends
                                            * DefaultInCallUICommonPresenceExt
                                            */{
    private static final String TAG = "OP08InCallUICommonPresenceExt";
    private Context mContext;
    private PluginApiManager mInstance = null;

    /**
     * Constructor.
     *
     * @param context context
     */
    public OP08InCallUICommonPresenceExt(Context context) {
        Log.d(TAG, "[OP08InCallUICommonPresenceExt] constuctor entry "
                + context);
        mContext = context;
        PluginApiManager.initialize(context);
        mInstance = PluginApiManager.getInstance();
    }

    /**
     * Checks if contact is video call capable
     *
     * @param contactId
     *            Contact Id.
     * @return true if contact is video call capable.
     */
    // @Override
    public boolean isVideoCallCapable(String number) {
        Log.d(TAG, "isVideoCallCapable entry");
        boolean videoCallCapability = false;
        if (mInstance != null) {

            videoCallCapability = mInstance.isVideoCallCapable(number);
            return videoCallCapability;
        }
        return videoCallCapability;
    }
}
