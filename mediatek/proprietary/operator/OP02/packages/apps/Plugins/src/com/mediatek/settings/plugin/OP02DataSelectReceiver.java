package com.mediatek.settings.plugin;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.telephony.TelephonyManager;
import android.util.Log;

import java.util.List;
/**
 * For CU Spec, show a dialog to set default data.
 */
public class OP02DataSelectReceiver extends BroadcastReceiver {

    private static final String TAG = "OP02DataSelectReceiver";
    private TelephonyManager mTelephonyManager = null;

    @Override
    public void onReceive(Context context, Intent intent) {
        String action = intent.getAction();
        Log.d(TAG, "receive broadcast: " + action);
        mTelephonyManager = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);

        if (isSimDialogNeeded(context)) {
            Intent startDataPicker = new Intent(context, OP02DataPickService.class);
            if (action.equals("com.mediatek.settings.plugin.ACTION_RESEND_SIM_STATE")) {
                intent.putExtra("SEND_BY_SELF", true);
            }
            context.startService(startDataPicker);
        }
    }

    private boolean isSimDialogNeeded(Context context) {
        final int numSlots = mTelephonyManager.getSimCount();

        Log.d(TAG, "isSimDialogNeeded numSlots = " + numSlots);
        if (numSlots < 2) {
            return false;
        }

        Log.d(TAG, "dialog show flag: " + OP02DataPickService.sIsShow);
        if (OP02DataPickService.sIsShow) {
            return false;
        }
        return true;
    }
}
