package com.mediatek.settings.plugin;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Binder;
import android.os.IBinder;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.provider.Settings;
import android.telephony.SubscriptionManager;
import android.util.Log;

import com.android.internal.telephony.ITelephony;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.mediatek.internal.telephony.ITelephonyEx;
import com.mediatek.telephony.TelephonyManagerEx;
/**
* Switch rat mode.
*/
public class ChangeRatModeService extends Service {

    private static final String TAG = "ChangeRatModeService";
    private static final String MODE_STATUS = "mode_status";
    private static final String LTETDD_CDMA = "ltetdd_cdma";

    private final IBinder mSetRatMode = new SetRatMode();
    private TelephonyManagerEx mTelephonyManagerEx = null;

    @Override
    public void onCreate() {
        Log.i(TAG, "ChangeRatModeService onCreate");
        mTelephonyManagerEx = TelephonyManagerEx.getDefault();
    }
    @Override
    public void onDestroy() {
        Log.i(TAG, "ChangeRatModeService onDestroy");
    }

    @Override
    public int onStartCommand(Intent intent, int flag, int startId) {
        Log.i(TAG, "ChangeRatModeService start");
        try {
            saveData(true);
            switchSvlte(true);
        } catch (RemoteException e) {
            e.printStackTrace();
        } finally {
            stopSelf(startId);
        }
        return START_NOT_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent) {
        return mSetRatMode;
    }
    /**
    *Binder for client.
    */
    public class SetRatMode extends Binder {
        public ChangeRatModeService getService() {
            return ChangeRatModeService.this;
        }
    }
    /**
    * Save RAT mode.
    * @param isChecked open or close the 4G switch.
    * @throws RemoteException remote exception
    */
    public void saveData(boolean isChecked) throws RemoteException {
        int subId = SubscriptionManager.getSubIdUsingPhoneId(PhoneConstants.SIM_ID_1);
        if (!isChecked) {
            int lastMode = Settings.Global.getInt(this.getContentResolver(),
                                    Settings.Global.PREFERRED_NETWORK_MODE + subId,
                                    Phone.NT_MODE_LTE_CDMA_AND_EVDO);
            Log.i(TAG, "saveData isChecked = false lastMode = " + lastMode);
            this.getSharedPreferences(MODE_STATUS, Context.MODE_PRIVATE)
                .edit().putInt(LTETDD_CDMA, lastMode).commit();
            Settings.Global.putInt(this.getContentResolver(),
                        Settings.Global.PREFERRED_NETWORK_MODE + subId,
                        Phone.NT_MODE_CDMA);
        } else {
            int lteCdma = this.getSharedPreferences(MODE_STATUS, Context.MODE_PRIVATE)
                .getInt(LTETDD_CDMA, -1);
            Log.i(TAG, "saveData isChecked = true lteCdma = " + lteCdma);
            if (lteCdma != -1) {
                Settings.Global.putInt(this.getContentResolver(),
                        Settings.Global.PREFERRED_NETWORK_MODE + subId,
                        lteCdma);
            } else {
                Settings.Global.putInt(this.getContentResolver(),
                        Settings.Global.PREFERRED_NETWORK_MODE + subId,
                        Phone.NT_MODE_LTE_CDMA_AND_EVDO);
            }
        }
    }
    /**
    * Switch RAT mode.
    * @param subId sub id
    * @param networkType network type
    */
    private void setPreferredNetworkType(int subId, int networkType) {
        Thread changeNetworkType = new Thread(new ChangeNetworkType(subId,
                                                                   networkType,
                                                                   this));
        changeNetworkType.start();
    }
    /**
    * Switch RAT mode.
    * @param isChecked open or close the 4G switch.
    * @throws RemoteException remote exception
    */
    public void switchSvlte(boolean isChecked) throws RemoteException {
        int subId = SubscriptionManager.getSubIdUsingPhoneId(PhoneConstants.SIM_ID_1);
        ITelephony telephony = ITelephony.Stub.asInterface(ServiceManager
                .getService(Context.TELEPHONY_SERVICE));

        if (null != telephony) {
            if (!isChecked) {
                Log.i(TAG, "switchSvlte isChecked = false switchSvlteRatMode 3g");
                setPreferredNetworkType(subId, Phone.NT_MODE_CDMA);
                Log.i(TAG, "switchSvlte setPreferredNetworkType 1, value= " +
                        Phone.NT_MODE_CDMA);
            } else {
                int lteCdma = this.getSharedPreferences(MODE_STATUS,
                        Context.MODE_PRIVATE).getInt(LTETDD_CDMA, -1);

                Log.i(TAG, "switchSvlte isChecked = true last mode lte_cdma = " + lteCdma);
                if (lteCdma != -1) {
                    Log.i(TAG, "switchSvlte saveData isChecked = true put LTETDD_CDMA = -1");
                    this.getSharedPreferences(MODE_STATUS, Context.MODE_PRIVATE)
                        .edit().putInt(LTETDD_CDMA, -1).commit();

                    if (lteCdma == Phone.NT_MODE_LTE_TDD_ONLY) {
                        Log.i(TAG, "switchSvlte last mode SVLTE_RAT_MODE_4G_DATA_ONLY");
                        setPreferredNetworkType(subId, Phone.NT_MODE_LTE_CDMA_AND_EVDO);
                        Log.i(TAG, "switchSvlte 4G_DATA_ONLY, value= " +
                            Phone.NT_MODE_LTE_CDMA_AND_EVDO);
                    } else if (lteCdma == Phone.NT_MODE_LTE_CDMA_AND_EVDO) {
                        Log.i(TAG, "switchSvlte last mode SVLTE_RAT_MODE_4G");
                        setPreferredNetworkType(subId, Phone.NT_MODE_LTE_CDMA_AND_EVDO);
                        Log.i(TAG, "switchSvlte RAT_MODE_4G, value= " +
                                                Phone.NT_MODE_LTE_CDMA_AND_EVDO);
                    } else {
                        Log.i(TAG, "switchSvlte last mode error, do open 4G mode");
                        setPreferredNetworkType(subId, Phone.NT_MODE_LTE_CDMA_AND_EVDO);
                        Log.i(TAG, "switchSvlte mode error, value= " +
                                Phone.NT_MODE_LTE_CDMA_AND_EVDO);
                    }
                } else {
                    setPreferredNetworkType(subId, Phone.NT_MODE_LTE_CDMA_AND_EVDO);
                    Log.i(TAG, "switchSvlte setPreferredNetworkType 2, value= " +
                            Phone.NT_MODE_LTE_CDMA_AND_EVDO);
                }
            }
        }
    }

}
