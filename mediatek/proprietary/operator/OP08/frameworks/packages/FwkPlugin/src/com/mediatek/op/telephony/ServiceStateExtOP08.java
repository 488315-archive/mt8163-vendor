package com.mediatek.op.telephony;

import android.content.Context;
import android.telephony.SignalStrength;
import android.util.Log;

import com.mediatek.common.PluginImpl;

/// M: The OP08 implementation of ServiceState.

@PluginImpl(interfaceName = "com.mediatek.common.telephony.IServiceStateExt")

public class ServiceStateExtOP08 extends DefaultServiceStateExt {

    /** Constructor.
     * @param
    */
    public ServiceStateExtOP08() {
    }

    /** Constructor.
     * @param context id
    */
    public ServiceStateExtOP08(Context context) {
    }

    /** mapLteSignalLevel.
     * @param mLteRsrp current rsrp
     * @param mLteRssnr lte rssnr
     * @param mLteSignalStrength signal strength
     * @return rsrpIconLevel
    */
    public int mapLteSignalLevel(int mLteRsrp, int mLteRssnr, int mLteSignalStrength) {
        int rsrpIconLevel;

        if (mLteRsrp < -140 || mLteRsrp > -44) {
            rsrpIconLevel = SignalStrength.SIGNAL_STRENGTH_NONE_OR_UNKNOWN;
            /*Here need one more for one more bar -100*/
        } else if (mLteRsrp >= -110) {
            rsrpIconLevel = SignalStrength.SIGNAL_STRENGTH_GREAT;
        } else if (mLteRsrp >= -115) {
            rsrpIconLevel = SignalStrength.SIGNAL_STRENGTH_GOOD;
        } else if (mLteRsrp >= -120) {
            rsrpIconLevel = SignalStrength.SIGNAL_STRENGTH_MODERATE;
        } else if (mLteRsrp >= -135) {
            rsrpIconLevel = SignalStrength.SIGNAL_STRENGTH_POOR;
        } else {
            rsrpIconLevel = SignalStrength.SIGNAL_STRENGTH_NONE_OR_UNKNOWN;
        }
        Log.i(TAG, "op08_mapLteSignalLevel=" + rsrpIconLevel);
        return rsrpIconLevel;
    }
}
