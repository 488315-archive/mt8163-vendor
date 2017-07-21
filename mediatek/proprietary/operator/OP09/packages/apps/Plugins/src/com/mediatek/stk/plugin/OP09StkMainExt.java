
package com.mediatek.stk.plugin;

import android.telephony.TelephonyManager;
import android.telephony.SubscriptionManager;
import android.util.Log;

import com.android.internal.telephony.cat.AppInterface;
import com.android.internal.telephony.cat.TextMessage;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;

import com.mediatek.common.PluginImpl;
import com.mediatek.stk.ext.DefaultStkMainExt;

@PluginImpl(interfaceName = "com.mediatek.stk.ext.IStkMainExt")
public class OP09StkMainExt extends DefaultStkMainExt {

    private static final String TAG = "OP09StkMainExt";
    /**
     * For op09,no need show entrance for CDMA card.
     * @param slotId: the current used slot id
     * @return false if meet condition,default return true.
     */
    public boolean isShowEntrance(int slotId) {

        int subId[] = SubscriptionManager.getSubId(slotId);
        Phone phone = getPhoneUsingSubId(subId[0]);
        String cardType = phone.getIccCard().getIccCardType();
        Log.d(TAG, "cardType == " + cardType);
        if (cardType.contains("RUIM") || cardType.contains("CSIM")) {
            return false;
        } else {
            return true;
        }
    }

    private Phone getPhoneUsingSubId(int subId) {
        Log.d(TAG, "getPhoneUsingSubId subId:" + subId);
        int phoneId = SubscriptionManager.getPhoneId(subId);
        if (phoneId < 0 || phoneId >= TelephonyManager.getDefault().getPhoneCount()) {
            return PhoneFactory.getPhone(0);
        }
        return PhoneFactory.getPhone(phoneId);
    }

    /**
     * For CT A/C lib requirement, when do BIP test if there is no message return
     * from SIM card,no need to show toast.
     * @param cmdType: the current commmand type,e.g.:OPEN CHANNEL,CLOSE CHANNEL
     * @param msg: the text message of the current command
     * @return false if meet condition,default return true.
     */
    public boolean isShowBipToast(AppInterface.CommandType cmdType, TextMessage msg) {
        Log.d(TAG, "isShowBipToast,cmdtype :" + cmdType + " msg.iconSelfExplanatory:" +
             msg.iconSelfExplanatory + " msg.text:" + msg.text);
        if (!msg.iconSelfExplanatory
                && msg.text == null
                && (cmdType == AppInterface.CommandType.OPEN_CHANNEL
                        || cmdType == AppInterface.CommandType.CLOSE_CHANNEL
                        || cmdType == AppInterface.CommandType.RECEIVE_DATA
                        || cmdType == AppInterface.CommandType.SEND_DATA
                        || cmdType == AppInterface.CommandType.GET_CHANNEL_STATUS)) {
            Log.d(TAG, "Will no show BIP related toast");
            return false;
        }
        return true;
    }
}
