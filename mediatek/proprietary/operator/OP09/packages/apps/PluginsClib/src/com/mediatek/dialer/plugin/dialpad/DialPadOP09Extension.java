package com.mediatek.dialer.plugin.dialpad;

import android.app.Fragment;
import android.content.Context;
import android.content.Intent;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.telephony.SubscriptionManager;
import android.util.Log;

import com.android.internal.telephony.ITelephony;
import com.android.internal.telephony.PhoneConstants;

import com.mediatek.common.PluginImpl;
import com.mediatek.dialer.ext.DefaultDialPadExtension;
import com.mediatek.dialer.ext.DialpadExtensionAction;

/**
 * dialpad extension plugin for op09.
*/
@PluginImpl(interfaceName = "com.mediatek.dialer.ext.IDialPadExtension")
public class DialPadOP09Extension extends DefaultDialPadExtension {
    private static final String TAG = "DialPadOP09Extension";
    private static final String PACKAGE_NAME = "com.mediatek.op09.plugin";

    private static final String PRL_VERSION_DISPLAY = "*#0000#";
    private static final String CDMAINFO = "android.intent.action.CdmaInfoSpecification";

    private static final int SLOT1 = 0;
    private static final int SLOT2 = 1;

    private Context mPluginContext;
    private Context mHostContext;

    public DialPadOP09Extension(Context cnx) {
        super();
        mPluginContext = cnx;
    }

    public void onCreate(Context context, Fragment fragment,
                        DialpadExtensionAction dialpadFragment) {
        mHostContext = context.getApplicationContext();
    }

    /**
     * handle special chars from user input on dial pad.
     *
     * @param context from host app.
     * @param input from user input in dial pad.
     * @return boolean, check if the input string is handled.
     */
    public boolean handleChars(Context context, String input) {
        if (input.equals(PRL_VERSION_DISPLAY)) {
            int subId = SubscriptionManager.INVALID_SUBSCRIPTION_ID;
            int length = SubscriptionManager.from(
                    mHostContext).getActiveSubscriptionIdList().length;
            try {
                ITelephony iTel = ITelephony.Stub.asInterface(
                        ServiceManager.getService(Context.TELEPHONY_SERVICE));
                log("handleChars getActiveSubscriptionIdList length:" + length);
                for (int i = 0; i < length; i++) {
                    int activeSubId = SubscriptionManager.from(
                    mHostContext).getActiveSubscriptionIdList()[i];
                    int phoneType = iTel.getActivePhoneTypeForSubscriber(activeSubId);
                    if (PhoneConstants.PHONE_TYPE_CDMA == phoneType) {
                        subId = activeSubId;
                        log("handleChars subId:" + subId);
                        break;
                    }
                }
            } catch (RemoteException ex) {
                ex.printStackTrace();
            }

            if (0 != length && SubscriptionManager.isValidSubscriptionId(subId)) {
                showPRLVersionSetting(context, subId);
                return true;
            } else {
                showPRLVersionSetting(context, SubscriptionManager.INVALID_SUBSCRIPTION_ID);
                return true;
            }
        }
        return false;
    }

    /**
     * show version by cdma phone provider info.
     *
     * @param context from host app.
     * @param slot indicator which slot is cdma phone.
     * @return void.
     */
    private void showPRLVersionSetting(Context context, int subId) {
        Intent intentCdma = new Intent(CDMAINFO);
        intentCdma.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        intentCdma.putExtra(CdmaInfoSpecification.KEY_SUB_ID, subId);
        context.startActivity(intentCdma);
    }

    /**
     * simple log info.
     *
     * @param msg need print out string.
     * @return void.
     */
    private static void log(String msg) {
        Log.d(TAG, msg);
    }
}
