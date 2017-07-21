package com.mediatek.engineermode.mbim;



import android.app.Service;
import android.content.Intent;

import android.os.AsyncTask;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.SystemProperties;

import android.widget.Toast;

import com.android.internal.telephony.Phone;
import com.mediatek.engineermode.Elog;
import com.mediatek.engineermode.R;


/**
 * Service to disable mbim in background.
 *
 */
public class UsbStateHandleService extends Service {

    private static final String TAG = "EM/UsbStateHandleService";
    private static final int TIME_TO_SLEEP = 2000;
    private static final String ATTACH_SYSTEM_PROPERTY = "net.mbim.state";
    private static final String VALUE_DETACH = "detach";
    private static final int MSG_PDN_SWITCH_FAIL = 21;
    private static final int MSG_SERVICE_STOP_FAIL = 22;
    private Phone mPhone = null;

    /**
     * Background operation for detach.
     *
     */
    private class DetachTask extends AsyncTask<Void, Void, Boolean> {

        @Override
        protected void onPostExecute(Boolean result) {
            // TODO Auto-generated method stub
            super.onPostExecute(result);
            stopSelf();
        }

        @Override
        protected Boolean doInBackground(Void... params) {
            // TODO Auto-generated method stub
            Elog.i(TAG, "doInBackground");
            MBIMConfig.disablePDN(mPhone);

            try {
                for (int k = 0; k < 10; k++) {
                    Thread.sleep(TIME_TO_SLEEP);
                    if (VALUE_DETACH.equals(SystemProperties.get(ATTACH_SYSTEM_PROPERTY))) {
                        break;
                    }
                }
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            Elog.i(TAG, "ATTACH_SYSTEM_PROPERTY = " + SystemProperties.get(ATTACH_SYSTEM_PROPERTY));
            if (!VALUE_DETACH.equals(SystemProperties.get(ATTACH_SYSTEM_PROPERTY))) {
                mHandler.sendEmptyMessage(MSG_PDN_SWITCH_FAIL);
            }

            //Stop MBIMD service
            if (MBIMJni.stopService() == -1) {
                mHandler.sendEmptyMessage(MSG_SERVICE_STOP_FAIL);
            }
            //Switch USB mode
            MBIMConfig.switchUsb(UsbStateHandleService.this, false);
            return true;
        }
    }

    @Override
    public void onCreate() {
        // TODO Auto-generated method stub
        super.onCreate();
        mPhone = MBIMConfig.initPhone();
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        // TODO Auto-generated method stub
        MBIMConfig.setEnablePref(this, false);
        new DetachTask().execute();
        return super.onStartCommand(intent, flags, startId);
    }

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case MSG_PDN_SWITCH_FAIL:
                Elog.i(TAG, "Receive msg MSG_PDN_SWITCH_FAIL");

                Toast.makeText(UsbStateHandleService.this,
                        R.string.pdn_switch_fail_msg, Toast.LENGTH_LONG).show();
                break;
            case MSG_SERVICE_STOP_FAIL:
                Elog.i(TAG, "Receive msg MSG_SERVICE_STOP_FAIL");
                Toast.makeText(UsbStateHandleService.this,
                        R.string.service_stop_fail_msg, Toast.LENGTH_LONG).show();
                break;
            default:
                break;
            }
        }
    };



}
