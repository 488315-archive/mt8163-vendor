package com.mediatek.op.telephony;
import android.util.Log;

import com.mediatek.common.PluginImpl;

/**
 * plugin impl.
 */
@PluginImpl (interfaceName = "com.mediatek.common.telephony.IServiceStateExt")

/**
 * Disable IVSR feature.
 */
public class ServiceStateExtOP18 extends DefaultServiceStateExt {
    private static final String TAG = "ServiceStateExtOP18";

    /**
     * Return if need disable IVSR.
     * @return if need disable IVSR
     */
    public boolean isNeedDisableIVSR() {
        Log.i(TAG, "Disable IVSR");
        return true;
    }
}
