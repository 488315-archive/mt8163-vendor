package com.mediatek.commonpresence.plugin;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;

import android.util.Log;

import com.mediatek.common.PluginImpl;
import com.mediatek.dialer.ext.DefaultCallLogCommonPresenceExtension;

/**
 * Plugin implementation for Call Log Common Presence.
 */
@PluginImpl(interfaceName = "com.mediatek.dialer.ext.ICallLogCommonPresenceExtension")
public class OP08CallLogCommonPresenceExt extends
        DefaultCallLogCommonPresenceExtension {
    private static final String TAG = "OP08CallLogCommonPresenceExt";
    private Context mContext;
    private PluginApiManager mInstance = null;

    /**
     * Constructor.
     *
     * @param context context
     */
    public OP08CallLogCommonPresenceExt(Context context) {
        Log.d(TAG, "[OP08CallLogCommonPresenceExt] constuctor entry " + context);
        mContext = context;
        PluginApiManager.initialize(context);
        mInstance = PluginApiManager.getInstance();
    }

    /**
     * Checks if contact is video call capable
     *
     * @param number
     *            number to get video capability.
     * @param isAnonymous
     *            Is number saved in contact list.
     * @return true if contact is video call capable.
     */
    @Override
    public boolean isVideoCallCapable(String number, boolean isAnonymous) {
        Log.d(TAG, "isVideoCallCapable entry number :" + number
                + ",isAnonymous:" + isAnonymous);
        boolean videoCallCapability = false;
        if (mInstance != null) {
            if (isAnonymous) {
                mInstance.requestContactPresence(number);
            } else {
                videoCallCapability = mInstance.isVideoCallCapable(number);
            }
            return videoCallCapability;
        }
        return videoCallCapability;
    }
}
