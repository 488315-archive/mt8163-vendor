package com.mediatek.engineermode.mbim;

import com.mediatek.engineermode.Elog;
import com.mediatek.engineermode.boot.EmBootStartService;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.hardware.usb.UsbManager;
import android.net.wifi.WifiManager;

public class UsbStateReceiver extends BroadcastReceiver {
    private static final String TAG = "EM/UsbStateReceiver";
    @Override
    public void onReceive(Context context, Intent intent) {
        // TODO Auto-generated method stub
        if (UsbManager.ACTION_USB_STATE.equals(intent.getAction())
                && intent.getBooleanExtra("USB_HW_DISCONNECTED", false)) {
            Elog.d(TAG, "receive broadcast: USB disconnected");
            if (MBIMConfig.getEnablePref(context)) {
                Elog.d(TAG, "startService");
                context.startService(new Intent(context, UsbStateHandleService.class));
            }
        }
    }

}
