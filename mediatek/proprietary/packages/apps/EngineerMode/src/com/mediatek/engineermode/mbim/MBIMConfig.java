package com.mediatek.engineermode.mbim;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.hardware.usb.UsbManager;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.SystemProperties;
import android.telephony.TelephonyManager;
import android.view.View;
import android.widget.CheckBox;
import android.widget.Toast;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.PhoneFactory;
import com.mediatek.engineermode.Elog;
import com.mediatek.engineermode.FeatureSupport;
import com.mediatek.engineermode.ModemCategory;
import com.mediatek.engineermode.R;


/**
 * Class for MBIM control.
 *
 */
public class MBIMConfig extends Activity {

    private static final String TAG = "EM/MBIMConfig";
    private static final String ACTION_USB_MBIM_SETFUNCTION =
            "com.mbim.action.setfunction";
    private static final String PARAM_ENABLE_MBIM =
            "com.mbim.enable";
    public static final String USBMANAGER_ACTION_USB_STATE =
            "android.hardware.usb.action.USB_STATE";
    private static final String ATTACH_SYSTEM_PROPERTY = "net.mbim.state";
    private static final String VALUE_DETACH = "detach";
    private static final String ATC_DETACH = "AT+SCGACT=0";
    static final String MBIM_PREF = "mbim";
    static final String MBIM_KEY_ENABLE = "enable";
    private static final int DIALOG_SWITCHING_MBIM = 1;
    private static final int DIALOG_SERVICE_START_FAIL = 2;
    private static final int DIALOG_SWITCHING_NORMAL_ERROR = 3;
    private static final int DIALOG_SWITCHING_PDN = 4;
    private static final int DIALOG_SWITCHING_NORMAL = 5;
    private static final int DIALOG_SERVICE_STOP_FAIL = 6;
    private static final int DIALOG_USB_SWITCH_FAIL = 7;
    private static final int TIME_TO_SLEEP = 2000;
    private final static int SWITCH_TO_MBIM = 1;
    private final static int SWITCH_MBIM_DONE = 2;
    private final static int USB_SWITCH_FAIL = 3;
    private final static int SERVICE_START_FAIL = 4;
    private final static int SWITCH_PDN = 5;
    private final static int SWITCH_NORMAL_DONE = 6;
    private final static int PDN_SWITCH_FINISH = 7;
    private final static int SERVICE_STOP_FAIL = 8;

    private CheckBox mMbimEnableChk;
    private Phone mPhone = null;
    private boolean mUsbSwitchDone = false;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // TODO Auto-generated method stub
        super.onCreate(savedInstanceState);
        setContentView(R.layout.mbim);
        mPhone = initPhone();
        mMbimEnableChk = (CheckBox) findViewById(R.id.checkBoxMBIM);
        SharedPreferences pref = getSharedPreferences(MBIM_PREF, MODE_PRIVATE);
        mMbimEnableChk.setChecked(getEnablePref(this));
        Elog.i(TAG, "getEnablePref = " + getEnablePref(this));
        mMbimEnableChk.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                boolean newState = mMbimEnableChk.isChecked();
                if (newState) {
                    new AttachTask().execute();
                } else {
                    new DetachTask().execute();
                }
        } });

        IntentFilter filter = new IntentFilter();
        filter.addAction(UsbManager.ACTION_USB_STATE);
        registerReceiver(mReceiver, filter);

    }

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {

            if (UsbManager.ACTION_USB_STATE.equals(intent.getAction())) {
                if (!mUsbSwitchDone) {
                    boolean usbConfigured = intent.getBooleanExtra(
                            UsbManager.USB_CONFIGURED, false);
                    boolean usbConnected = intent.getBooleanExtra(UsbManager.USB_CONNECTED, false);
                    boolean mbimEnabled = intent.getBooleanExtra(
                            "mbim_dun", false);
                    Elog.i(TAG, "usbConnected = " + usbConnected + " usbConfigured = "
                            + usbConfigured + ", mbimEnabled = " + mbimEnabled);
                    if (usbConnected && usbConfigured && mbimEnabled) {
                        mUsbSwitchDone = true;
                    }
                }
                boolean usbDisconnected = intent.getBooleanExtra("USB_HW_DISCONNECTED", false);
                mMbimEnableChk.setEnabled(!usbDisconnected);

                if (mUsbSwitchDone && usbDisconnected) {
                    mMbimEnableChk.setChecked(false);
                }
             }

        }
    };
    @Override
    protected void onStart() {
        // TODO Auto-generated method stub
        super.onStart();
    }


    @Override
    protected void onDestroy() {
        // TODO Auto-generated method stub
        unregisterReceiver(mReceiver);
        super.onDestroy();
    }


    @Override
    protected Dialog onCreateDialog(int id) {
        switch (id) {
        case DIALOG_SWITCHING_MBIM:
            ProgressDialog dlgMbim = new ProgressDialog(this);
            dlgMbim.setMessage(getString(R.string.switch_to_mbim));
            dlgMbim.setCancelable(false);
            dlgMbim.setIndeterminate(true);
            return dlgMbim;
        case DIALOG_SERVICE_START_FAIL:
            return new AlertDialog.Builder(this)
                .setTitle(R.string.error_title)
                .setMessage(R.string.service_start_fail_msg)
                .setPositiveButton(android.R.string.ok, null).create();
        case DIALOG_SERVICE_STOP_FAIL: {
            return new AlertDialog.Builder(this)
                    .setTitle(R.string.error_title)
                       .setMessage(R.string.service_stop_fail_msg)
                       .setPositiveButton(android.R.string.ok, null).create();
        }
        case DIALOG_SWITCHING_PDN:
            ProgressDialog dlgPdn = new ProgressDialog(this);
            dlgPdn.setMessage(getString(R.string.pdn_switch));
            dlgPdn.setCancelable(false);
            dlgPdn.setIndeterminate(true);
            return dlgPdn;
        case DIALOG_SWITCHING_NORMAL:
            ProgressDialog dlgNormal = new ProgressDialog(this);
            dlgNormal.setMessage(getString(R.string.switch_to_normal));
            dlgNormal.setCancelable(false);
            dlgNormal.setIndeterminate(true);
            return dlgNormal;
        case DIALOG_USB_SWITCH_FAIL: {
            return new AlertDialog.Builder(this)
                .setTitle(R.string.error_title)
                .setMessage(R.string.usb_switch_fail_msg)
                .setPositiveButton(android.R.string.ok, null).create();
        }
        default:
            return null;
        }
    }

    static Phone initPhone() {
        Phone phone = null;

        if (TelephonyManager.getDefault().getPhoneCount() > 1) {
            phone = PhoneFactory.getPhone(PhoneConstants.SIM_ID_1);
        } else {
            phone = PhoneFactory.getDefaultPhone();
        }

        if (FeatureSupport.isSupported(FeatureSupport.FK_MTK_C2K_SUPPORT)) {
            if (FeatureSupport.isSupported(FeatureSupport.FK_MTK_SVLTE_SUPPORT)) {
                phone = ModemCategory.getCdmaPhone();
            }
            if (FeatureSupport.isSupported(FeatureSupport.FK_EVDO_DT_SUPPORT)
                    && phone.getPhoneType() == PhoneConstants.PHONE_TYPE_CDMA) {
                phone = PhoneFactory.getPhone(PhoneConstants.SIM_ID_2);
            }
        }

        return phone;
    }

    static void switchUsb(Context context, Boolean isEnable) {
        Elog.d(TAG, "switchUsb: " + isEnable);
        Intent intent = new Intent(ACTION_USB_MBIM_SETFUNCTION);
        intent.putExtra(PARAM_ENABLE_MBIM, isEnable ? 1 : 0);
        context.sendBroadcast(intent);
    }

    static void disablePDN(Phone phone) {
        Elog.d(TAG, "sendATCommand: " + ATC_DETACH);
        String[] cmd = new String[2];
        cmd[0] = ATC_DETACH;
        cmd[1] = "";
        phone.invokeOemRilRequestStrings(cmd, null);
    }

    /**
     * Background operation for attach.
     *
     */
    private class AttachTask extends AsyncTask<Void, Void, Boolean> {

        @Override
        protected Boolean doInBackground(Void... params) {
            // TODO Auto-generated method stub
            mHandler.sendEmptyMessage(SWITCH_TO_MBIM);
            //Start MBIMD service
            if (MBIMJni.startService() == -1) {
                mHandler.sendEmptyMessage(SERVICE_START_FAIL);
                return true;
            }
            //Switch USB mode
            mUsbSwitchDone = false;
            switchUsb(MBIMConfig.this, true);
            try {
                for (int k = 0; k < 5; k++) {
                    Thread.sleep(TIME_TO_SLEEP);
                    if (mUsbSwitchDone) {
                        mHandler.sendEmptyMessage(SWITCH_MBIM_DONE);
                        return true;
                    }
                }
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            MBIMJni.stopService();
            mHandler.sendEmptyMessage(USB_SWITCH_FAIL);
            return true;
        }
    }

    /**
     * Write preference for saving mbim enable status.
     * @param context Context for the operation
     * @param enable True for enable mbim, false for disable mbim
     */
    public static void setEnablePref(Context context, boolean enable) {
        SharedPreferences.Editor pref = context.getSharedPreferences(
                MBIM_PREF, MODE_MULTI_PROCESS).edit();
        pref.putBoolean(MBIM_KEY_ENABLE, enable);
        pref.commit();
        Elog.i(TAG, "setEnablePref = " + enable);
    }

    static boolean getEnablePref(Context context) {
        SharedPreferences pref = context.getSharedPreferences(MBIM_PREF, MODE_MULTI_PROCESS);
        Elog.i(TAG, "getEnablePref = " + pref.getBoolean(MBIM_KEY_ENABLE, false));
        return pref.getBoolean(MBIM_KEY_ENABLE, false);
    }

    /**
     * Background operation for detach.
     *
     */
    private class DetachTask extends AsyncTask<Void, Void, Boolean> {

        @Override
        protected Boolean doInBackground(Void... params) {
            // TODO Auto-generated method stub
            mHandler.sendEmptyMessage(SWITCH_PDN);
            disablePDN(mPhone);

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
            mHandler.sendEmptyMessage(PDN_SWITCH_FINISH);
            boolean stopServiceResult = true;
            //Stop MBIMD service
            if (MBIMJni.stopService() == -1) {
                stopServiceResult = false;
            }
            //Switch USB mode
            switchUsb(MBIMConfig.this, false);
            try {
                Thread.sleep(TIME_TO_SLEEP);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

            mHandler.sendEmptyMessage(stopServiceResult ? SWITCH_NORMAL_DONE : SERVICE_STOP_FAIL);
            return true;
        }
    }

    private Handler mHandler = new Handler() {
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case SWITCH_TO_MBIM: {
                showDialog(DIALOG_SWITCHING_MBIM);
                break;
            }
            case SWITCH_MBIM_DONE:
                removeDialog(DIALOG_SWITCHING_MBIM);
                setEnablePref(MBIMConfig.this, true);
                break;
            case SERVICE_START_FAIL: {
                mMbimEnableChk.setChecked(false);
                removeDialog(DIALOG_SWITCHING_MBIM);
                showDialog(DIALOG_SERVICE_START_FAIL);
                break;
            }
            case SWITCH_PDN: {
                showDialog(DIALOG_SWITCHING_PDN);
                break;
            }
            case PDN_SWITCH_FINISH : {
                removeDialog(DIALOG_SWITCHING_PDN);
                if (!VALUE_DETACH.equals(SystemProperties.get(ATTACH_SYSTEM_PROPERTY))) {

                    Toast.makeText(MBIMConfig.this, R.string.pdn_switch_fail_msg,
                            Toast.LENGTH_LONG).show();
                }
                showDialog(DIALOG_SWITCHING_NORMAL);
                break;
            }
            case SWITCH_NORMAL_DONE : {
                removeDialog(DIALOG_SWITCHING_NORMAL);
                setEnablePref(MBIMConfig.this, false);
                break;
            }
            case SERVICE_STOP_FAIL : {
                removeDialog(DIALOG_SWITCHING_NORMAL);
                showDialog(DIALOG_SERVICE_STOP_FAIL);
                break;
            }
            case USB_SWITCH_FAIL: {
                mMbimEnableChk.setChecked(false);
                removeDialog(DIALOG_SWITCHING_MBIM);
                showDialog(DIALOG_USB_SWITCH_FAIL);
                break;
            }
            default:
                break;
            }
        }
    };
}
