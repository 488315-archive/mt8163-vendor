package com.mediatek.regionalphone;

import java.io.File;

import android.app.Application;
import android.content.Intent;
import android.util.Log;

public class RPMApplication extends Application {

    @Override
    public void onCreate() {
        super.onCreate();
        Log.d("@M_" + Common.LOG_TAG, "RPMApplication onCreate");

        Common.PREFE_FILE = this.getFilesDir().getParent() + File.separator
                + "shared_prefs" + File.separator + "mcc_mnc.xml";

        Configuration.getInstance().init(this);
        if (RPMUtils.isStartRPMService()) {
            Intent rps_intent = new Intent(this, RegionalPhoneService.class);
            rps_intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            rps_intent.putExtra(Common.SIM_LOADED, false);
            startService(rps_intent);
        }
    }
}
