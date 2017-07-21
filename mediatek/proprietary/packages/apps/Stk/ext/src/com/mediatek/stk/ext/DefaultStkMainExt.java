
package com.mediatek.stk.ext;

import com.android.internal.telephony.cat.AppInterface;
import com.android.internal.telephony.cat.TextMessage;

public class DefaultStkMainExt implements IStkMainExt {

    /**
     * for OP09
     * called when need judge if need show entrance,default return true
     * @param slotId
     */
    public boolean isShowEntrance(int slotId) {
        return true;
    }

    /**
     * For CT A/C lib requirement, when do BIP test if there is no message return
     * from SIM card,no need to show toast.
     */
    public boolean isShowBipToast(AppInterface.CommandType cmdType, TextMessage msg) {
        return true;
    }

}
