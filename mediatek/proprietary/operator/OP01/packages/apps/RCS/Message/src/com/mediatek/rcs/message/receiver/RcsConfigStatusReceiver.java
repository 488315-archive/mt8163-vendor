package com.mediatek.rcs.message.receiver;

import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.content.pm.PackageManager;
import android.text.TextUtils;
import android.util.Log;

import com.mediatek.rcs.common.RCSServiceManager;
import com.mediatek.rcs.message.utils.RcsMessageUtils;

/**
 * Describe class RcsConfigStatusReceiver here.
 *
 *
 * Created: Sun Mar 29 11:13:27 2015
 *
 * @author <a href="mailto:"></a>
 * @version 1.0
 */
public class RcsConfigStatusReceiver extends BroadcastReceiver {
    private static final String TAG = "Rcs/RcsConfigStatusReceiver";
    private static final String CONFIGURATION_STATUS =
        "com.orangelabs.rcs.CONFIGURATION_STATUS_TO_APP";
    private static final String CORE_CONFIGURATION_STATUS = "status";

    private static final String RCS_SERVICE_REGISTRERAD_STATUS
                                    = "com.mediatek.rcs.message.REGISTRATION_STATUS_CHANGE";
    private static final String SP_RCS_CONFIG = "rcs_config";
    private static final String KEY_SYNC_GROUP_LIST = "sync_group_list";

    /**
     * Creates a new <code>RcsConfigStatusReceiver</code> instance.
     *
     */
    public RcsConfigStatusReceiver() {
    }

    /**
     * Describe <code>onReceive</code> method here.
     *
     * @param context a <code>Context</code> value
     * @param intent an <code>Intent</code> value
     */
    public final void onReceive(final Context context, final Intent intent) {
        String action = intent.getAction();
        Log.d(TAG, "onReceive() action:" + action);
        if (TextUtils.isEmpty(action)) {
            return;
        }
        if (RCSServiceManager.RCS_SERVICE_STATE_ACTION.equals(action)) {
            boolean isConfigured = intent.getBooleanExtra(
                                    RCSServiceManager.RCS_SERVICE_STATE_CONFIGURED, false);
            boolean isActivated = intent.getBooleanExtra(
                                    RCSServiceManager.RCS_SERVICE_STATE_ACTIVATED, false);
//            boolean isRegistered = intent.getBooleanExtra(
//                                    RCSServiceManager.RCS_SERVICE_STATE_REGISTERED, false);
            Log.d(TAG, "onReceive() isConfigured:" + isConfigured +
                                        ", isActivated = " + isActivated);
            final ComponentName componentName =
                    new ComponentName(context, "com.mediatek.rcs.message.ui.ForwardActivity");
            context.getPackageManager()
            .setComponentEnabledSetting(componentName,
                            isConfigured && isActivated ?
                            PackageManager.COMPONENT_ENABLED_STATE_ENABLED :
                                PackageManager.COMPONENT_ENABLED_STATE_DISABLED,
                                PackageManager.DONT_KILL_APP);
        } else if (RCS_SERVICE_REGISTRERAD_STATUS.equals(action)) {
            boolean registered = intent.getBooleanExtra("status", false);
            Log.d(TAG, "registered = " + registered);
            if (registered) {
                int rcsSubId = RcsMessageUtils.getRcsSubId(context);
                int subId = getGroupListSyncedSubId(context);
                //TODO open the code below
                if (rcsSubId != subId) {
                    Intent groupListIntent =
                                        new Intent("com.mediatek.rcs.messageservice.SYNC_CHATS");
                    groupListIntent.setPackage("com.mediatek.rcs.messageservice");
                    context.startService(groupListIntent);
                    Log.d(TAG, "send sync group list groupListIntent");
                }
            }
        } else if (RCSServiceManager.RCS_SYNC_GROUP_CHATS_DONE.equals(action)) {
            boolean result = intent.getBooleanExtra("result", false);
            Log.d(TAG, "sync group done: result = " + result);
            if (result) {
                int rcsSubId = RcsMessageUtils.getRcsSubId(context);
                setGroupListSyncedSubId(context, rcsSubId);
            }
        }
    }

    private static void setGroupListSyncedSubId(Context context, int subId) {
        SharedPreferences sp = context.getSharedPreferences(SP_RCS_CONFIG,
                Context.MODE_WORLD_READABLE | Context.MODE_WORLD_WRITEABLE);
        if (sp != null) {
            Editor editor = sp.edit();
            editor.putInt(KEY_SYNC_GROUP_LIST, subId);
            editor.commit();
        }
        Log.d(TAG, "setGroupListSyncedSubId, subId = " + subId);
    }

    private static int getGroupListSyncedSubId(Context context) {
        int subId = -1;
        SharedPreferences sp = context.getSharedPreferences(SP_RCS_CONFIG,
                Context.MODE_WORLD_READABLE);
        if (sp != null) {
            subId = sp.getInt(KEY_SYNC_GROUP_LIST, -1);
        }
        Log.d(TAG, "getGroupListSyncedSubId: subId = " + subId);
        return subId;
    }
}
