
package com.mediatek.stk.ext;

import com.android.internal.telephony.cat.AppInterface;
import com.android.internal.telephony.cat.TextMessage;

public interface IStkMainExt {

    /**
     * For op09,no need show entrance for CDMA card.
     * @param slotId: the current used slot id
     * @return default return true.
     */
    public boolean isShowEntrance(int slotId);

    /**
     * For CT A/C lib requirement, when do BIP test if there is no message return
     * from SIM card,no need to show toast.
     * @param cmdType: the current commmand type,e.g.:OPEN CHANNEL,CLOSE CHANNEL
     * @param msg: the text message of the current command
     * @return default return true.
     */
    public boolean isShowBipToast(AppInterface.CommandType cmdType, TextMessage msg);

}
