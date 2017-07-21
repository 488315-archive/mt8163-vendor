package com.orangelabs.rcs.core.ims.rcsua;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.SystemProperties;

import com.orangelabs.rcs.utils.logger.Logger;

/**
 * The Class RcsVolteUpdateReceiver.
 */
public class RcsVolteUpdateReceiver extends BroadcastReceiver {

    
    /**
     * The logger
     */
    private Logger logger = Logger.getLogger(this.getClass().getName());


    
    /**
     * On receive.
     *
     * @param context the context
     * @param intent the intent
     */
    public void onReceive(Context context, Intent intent) {
        final String action = intent.getAction();
        logger.debug("set notification");
        if (action.equals("mediatek.volte.notification")) {
            //enable the RCS service and create socket
            String data = intent.getStringExtra("status");

            if (data.equals("true")) {
                RcsUaAdapter.getInstance().enableRCSProxyAdapter();
            } else if (data.equals("false")) {
                RcsUaAdapter.getInstance().disableRCSUAAdapter(false);
            }
        } else if (action.equals("com.android.ims.IMS_SERVICE_UP")) {

            logger.debug("com.android.ims.IMS_SERVICE_UP");

        } else if (action.equals("com.android.ims.IMS_SERVICE_DOWN")) {

            RcsUaAdapter.getInstance().disableRCSUAAdapter(false);
            
        } else if (action.equals("mediatek.volte.test.request.registrationinfo")) {

            RcsUaAdapter.getInstance().sendTestRequest();
        }
    }
}
