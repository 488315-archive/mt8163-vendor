package com.mediatek.settings.plugin;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.SystemProperties;
import android.telephony.SubscriptionManager;
import android.util.Log;

import com.android.internal.telephony.IccCardConstants;
import com.android.internal.telephony.IccCardConstants.CardType;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.TelephonyIntents;
import com.mediatek.op09.plugin.R;
import com.mediatek.telephony.TelephonyManagerEx;

/**
* Show sim alert dialog.
*/
public class SimDialogService extends Service {

    private static final String TAG = "SimDialogService";
    public static final String ACTION_START_SELF =
        "com.mediatek.intent.action.STARTSELF_SIM_DIALOG_SERVICE";

    public static final String EXTRA_NAME = "extra_name";
    public static final String TEXT = "text";

    public static final String ACTION_NAME = "com.mediatek.OP09.SIM_DIALOG_SERVICE";
    private static final String UIM_CHANGE_ALERT_ACTIVITY_NAME
        = "com.mediatek.OP09.UIM_CHANGE_ALERT";
    private boolean mSimtype = false;
    private boolean mUserpresent = false;
    private String mText;

    @Override
    public void onCreate() {
        super.onCreate();
    }

    @Override
    public void onStart(Intent intent, int startId) {
        super.onStart(intent, startId);
        if (intent != null) {
            Intent receiverIntent = (Intent) intent.getParcelableExtra(EXTRA_NAME);
            String action = receiverIntent.getAction();
            Log.d(TAG, "receiverIntent action = " + action);
            if (Intent.ACTION_USER_PRESENT.equals(action)) {
                mUserpresent = true;
                Log.d(TAG, "PRESENT mSimtype = " + mSimtype + "mUserpresent =" + mUserpresent);
                showAlert();
            } else if (TelephonyIntents.ACTION_SIM_STATE_CHANGED.equals(action)) {
                String cardState =
                    receiverIntent.getStringExtra(IccCardConstants.INTENT_KEY_ICC_STATE);
                int phoneId = receiverIntent.getIntExtra(PhoneConstants.PHONE_KEY, -1);
                Log.d(TAG, "card state is: " + cardState + ", PhoneId: " + phoneId);
                SubscriptionManager subManager = SubscriptionManager.from(this);
                int subId = subManager.getSubIdUsingPhoneId(phoneId);
                int slotId = subManager.getSlotId(subId);
                Log.d(TAG, "slot id: " + slotId);
                if (phoneId != 0) {
                    return;
                }
                TelephonyManagerEx telephonyManagerEx = TelephonyManagerEx.getDefault();
                CardType cardType = telephonyManagerEx.getCdmaCardType(PhoneConstants.SIM_ID_1);
                Log.i(TAG, "cardType = " + cardType.toString());
                Log.d(TAG, "CARDTYPE mSimtype = " + mSimtype + "mUserpresent =" + mUserpresent);
                if (IccCardConstants.CardType.CARD_NOT_INSERTED.equals(cardType)) {
                    if (!cardState.equals(IccCardConstants.INTENT_VALUE_ICC_ABSENT)) {
                        return;
                    }
                    String sim2Info = null;
                    sim2Info = SystemProperties.get("gsm.ril.fulluicctype.2");
                    Log.i(TAG, "sim2Info = +" + sim2Info + "+");
                    if ((sim2Info == null) || (sim2Info.equals(""))) {
                        Log.i(TAG, "not insert sim2");
                        mText = getString(R.string.no_sim_dialog_message);
                        mSimtype = true;
                    }

                } else if (!(IccCardConstants.CardType.PIN_LOCK_CARD).equals(cardType)
                        && !(IccCardConstants.CardType.CT_4G_UICC_CARD).equals(cardType)) {
                    if (!cardState.equals(IccCardConstants.INTENT_VALUE_ICC_READY) &&
                            !cardState.equals(IccCardConstants.INTENT_VALUE_ICC_ABSENT)) {
                        return;
                    }
                    mText = getString(R.string.lte_sim_dialog_message);
                    mSimtype = true;
                } else if ((IccCardConstants.CardType.CT_4G_UICC_CARD).equals(cardType)) {
                    mText = null;
                    mSimtype = false;
                }
                Log.d(TAG, "mText = " + mText);
                showAlert();
            } else if (action.equals(SimDialogService.ACTION_START_SELF)) {
                Log.d(TAG, "recv  ACTION_START_SELF");
                mSimtype = false;
            }
        }
    }


    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    private void showAlert() {
        Log.d(TAG, "showAlert mSimtype = " + mSimtype + "mUserpresent="
            + mUserpresent + " mText = " + mText);
        if (mText != null && mSimtype && mUserpresent) {
            Intent launchIntent = new Intent(UIM_CHANGE_ALERT_ACTIVITY_NAME);
            launchIntent.setPackage(getPackageName());
            launchIntent.putExtra(TEXT, mText);
            launchIntent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            startActivity(launchIntent);
        }
    }
}
