package com.mediatek.selfregister.utils;

import android.app.AlarmManager;
import android.app.Notification;
import android.app.PendingIntent;
import android.app.Service;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.os.Environment;
import android.os.StatFs;
import android.os.SystemProperties;
import android.preference.PreferenceManager;
import android.provider.Settings;
import android.telephony.CellLocation;
import android.telephony.PhoneStateListener;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.mediatek.custom.CustomProperties;
import com.mediatek.selfregister.Const;
import com.mediatek.selfregister.R;
import com.mediatek.telephony.TelephonyManagerEx;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

public class PlatformManager {

    private static final String TAG = Const.TAG_PREFIX + "PlatformManager";

    private static final String PREFERENCE_FILE_0 = "preference_file_0";
    private static final String PREFERENCE_FILE_1 = "preference_file_1";

    private static final String OPERATOR_CT_4G = "46011";
    private static final String OPERATOR_CT_MAC = "45502";
    private static final String OPERATOR_CT = "46003";

    private static final int LENGTH_MEID = 14;
    private static final int LENGTH_IMEI = 15;
    private static final int LENGTH_IMSI = 15;

    // M: Index of misc feature config switch in engineer mode.
    private static final int BIT_MISC_CONFIG = 2;
    private static final String KEY_MISC_CONFIG = Settings.Global.TELEPHONY_MISC_FEATURE_CONFIG;

    private static final String VALUE_DEFALUT_SOFTWARE_VERSION = "L1.P1";
    private static final String VALUE_DEFAULT_MANUFACTURER = "MTK";
    private static final String VALUE_DEFAULT_MEID = "A0000100001000";
    private static final String VALUE_DEFAULT_IMEI = "860001000010000";

    private static final String INDICATOR_FILE_NAME = "CTSelfRegisterIndicator";

    private TelephonyManager mTelephonyManager;
    private String mMeid = null;
    private String[] mImeiArray = null;

    public PlatformManager(Context context) {
        mTelephonyManager = (TelephonyManager) context.getSystemService(Context.TELEPHONY_SERVICE);
        if (mTelephonyManager == null) {
            throw new Error("telephony manager is null");
        }
    }

    /*
     * Whether feature is enabled
     */
    public static boolean isFeatureEnabled(ContentResolver contentResolver) {
        int config = Settings.Global.getInt(contentResolver, KEY_MISC_CONFIG, 0);
        return ((config & (1 << BIT_MISC_CONFIG)) != 0);
    }

    /*
     * if OP09_SPEC0212_SEGDEFAULT, CT card could only in slot0;
     * CT card could in two slots.
     */
    public static boolean supportCTForAllSlots() {
        String optr_spec_seg = SystemProperties.get("ro.operator.optr", "") + "_"
                + SystemProperties.get("ro.operator.spec", "") + "_"
                + SystemProperties.get("ro.operator.seg", "");

        Log.i(TAG, "[supportCTForAllSlots] optr_spec_seg is " + optr_spec_seg);

        if (optr_spec_seg.equals("OP09_SPEC0212_SEGDEFAULT")) {
            Log.i(TAG, "[supportCTForAllSlots] for A lab (not support), return false");
            return false;
        } else {
            Log.i(TAG, "[supportCTForAllSlots] not for A lab (support), return true");
            return true;
        }
    }

    /*
     * Get the slot which has 3/4G capability and map to (0, 1)
     */
    public static int getSimSwitchSlot() {
        // Get the slot which has 3/4G capability (1 or 2)
        int value = SystemProperties.getInt("persist.radio.simswitch", -1) - 1;
        Log.i(TAG, "3/4G is on slot " + value);
        if (value == Const.SLOT_ID_0 || value == Const.SLOT_ID_1) {
            return value;
        } else {
            return Const.SLOT_ID_INVALID;
        }
    }

    public boolean isSingleLoad() {
        return (mTelephonyManager.getSimCount() == 1);
    }

    public boolean hasCardInDevice(int[] slotIds) {
        for (int i = 0; i < slotIds.length; ++i) {
            if (mTelephonyManager.hasIccCard(slotIds[i])) {
                return true;
            }
        }
        return false;
    }

    public Boolean isSlotForCT(int slotId) {

        if (!mTelephonyManager.hasIccCard(slotId)) {
            Log.i(TAG, "[isSlotForCT] No sim card, return false");
            return false;
        }

        int[] subId = SubscriptionManager.getSubId(slotId);
        if (subId == null || subId[0] < 0) {
            Log.i(TAG, "[isSlotForCT] SudId not valid, return false");
            return false;
        }

        int phoneType = mTelephonyManager.getCurrentPhoneType(subId[0]);
        Log.i(TAG, "[isSlotForCT] Phone type is " + phoneType);

        if (TelephonyManager.PHONE_TYPE_CDMA == phoneType) {
            String networkOperator = mTelephonyManager.getNetworkOperatorForSubscription(subId[0]);
            Log.i(TAG, "[isSlotForCT] Network operator is " + networkOperator);

            if (OPERATOR_CT.equals(networkOperator)
                    || OPERATOR_CT_MAC.equals(networkOperator)
                    || OPERATOR_CT_4G.equals(networkOperator)) {

                String simOperator = mTelephonyManager.getSimOperator(subId[0]);
                Log.i(TAG, "[isSlotForCT] Sim operator is " + simOperator);

                Boolean condition = networkOperator.equals(simOperator);
                condition |= networkOperator.equals(OPERATOR_CT)
                        && simOperator.equals(OPERATOR_CT_4G);
                condition |= networkOperator.equals(OPERATOR_CT_4G)
                        && simOperator.equals(OPERATOR_CT);
                if (condition) {
                    Log.i(TAG, "[isSlotForCT] " + slotId + " true");
                    return true;
                }
            }
        }

        Log.i(TAG, "[isSlotForCT] " + slotId + " false");
        return false;
    }

    /*
     * Get complex IMSI (For master, may get CDMA/LTE IMSI; slave slot, only set LTE IMSI)
     */
    public String[] getComplexImsi(Context context, int slotId, int masterSlotId) {
        Log.i(TAG, "getComplexImsi " + slotId);

        String imsiArray[] = new String[2];
        int[] subId = getSubId(slotId);

        // For master slot, need both CDMA/LTE IMSI. If 4G CT, set both; 3G CT, set CDMA IMSI;
        // otherwise, set LTE IMSI.
        if (slotId == masterSlotId) {

            if (isUsimAndCsim(slotId)) {
                imsiArray[0] = getCdmaImsiForCT4G(slotId);
                imsiArray[1] = mTelephonyManager.getSubscriberId(subId[0]);
            } else {
                imsiArray = getImsiForNonCT4G(imsiArray, subId);
            }

        } else {
            // For slave slot, only need one imsi
            imsiArray[0] = Const.VALUE_EMPTY;
            imsiArray[1] = mTelephonyManager.getSubscriberId(subId[0]);
        }

        Log.i(TAG, "Slot " + slotId + " CDMA/LTE IMSI " + imsiArray[0] + "/" + imsiArray[1]);
        return imsiArray;
    }

    /*
     * Get IMSI info for Non CT-4G. If CT 3G, set LTE IMSI "", otherwise, set CDMA IMSI "".
     */
    private String[] getImsiForNonCT4G(String imsiArray[], int[] subId) {
        String subscriberId = mTelephonyManager.getSubscriberId(subId[0]);
        if (subscriberId != null &&
            (subscriberId.startsWith(OPERATOR_CT) ||
             subscriberId.startsWith(OPERATOR_CT_MAC))) {
            Log.i(TAG, "CT 3G: " + subscriberId);
            // CT 3G
            imsiArray[0] = subscriberId;
            imsiArray[1] = Const.VALUE_EMPTY;
        } else {
            // non-CT, set CDMA IMSI empty
            Log.i(TAG, "Not CT: " + subscriberId);
            imsiArray[0] = Const.VALUE_EMPTY;
            imsiArray[1] = subscriberId;
        }
        return imsiArray;
    }

    /*
     * If 4G card, check whether CDMA IMSI info is ready; else, return true.
     */
    public Boolean isImsiInfoReady(int slotId) {
        Boolean result = true;

        if (isUsimAndCsim(slotId)) {
            String cdmaImsi = getCdmaImsiForCT4G(slotId);
            Log.i(TAG, "Slot " + slotId + " cdma imsi is " + cdmaImsi);
            if (cdmaImsi == null || cdmaImsi.length() != LENGTH_IMSI) {
                result = false;
                Log.i(TAG, "CDMA IMSI not ready, return false");
            }
        } else {
            Log.i(TAG, "Not CT 4G sim, no need to check");
        }
        return result;
    }

    /*
     * Get CDMA IMSI info for CT 4G card
     */
    private String getCdmaImsiForCT4G(int slotId) {
        String result = Const.VALUE_EMPTY;
        int[] subId = getSubId(slotId);

        if (subId != null && subId[0] >= 0) {
            result = TelephonyManagerEx.getDefault().getUimSubscriberId(subId[0]);
        }
        return result;
    }

    /*
     * Whether type array is "CSIM" + "USIM"
     */
    private Boolean isUsimAndCsim(int slotId) {
        // Return value of getSupportCardType is a string array, could be
        // CSIM+USIM(+?): 4G CT, RUIM: 3G CT, CSIM: 3G CT
        // USIM: 4G CMCC/CU, SIM: 2/3G card
        String[] cardType = TelephonyManagerEx.getDefault().getSupportCardType(slotId);
        if (hasElement(cardType, "CSIM") && hasElement(cardType, "USIM")) {
            Log.i(TAG, "This is CT 4G card");
            return true;
        }
        return false;
    }

    /*
     * Whether type array includes "USIM"
     */
    private Boolean isUsim(int slotId) {
        // Return value of getSupportCardType is a string array, could be
        // CSIM+USIM: 4G CT, RUIM: 3G CT, CSIM: 3G CT
        // USIM: 4G CMCC/CU, SIM: 2/3G card
        String[] cardType = TelephonyManagerEx.getDefault().getSupportCardType(slotId);
        if (hasElement(cardType, "USIM")) {
            Log.i(TAG, "This is USIM card");
            return true;
        }
        return false;
    }

    private Boolean hasElement(String[] array, String pattern) {
        if (array != null && array.length > 0 && pattern != null) {
            for (int i = 0; i < array.length; ++i) {
                if (array[i].equals(pattern)) {
                    return true;
                }
            }
        }
        return false;
    }

    /*
     * If it's for IccCard, return 1; for UiccCard, return 2
     */
    public int getSimType(int slotId) {
        if (isUsim(slotId)) {
            Log.i(TAG, "It's Uicc, return 2");
            return 2;
        } else {
            Log.i(TAG, "It's icc, return 1");
            return 1;
        }
    }

    public boolean hasIccCard(int slotId) {
        return mTelephonyManager.hasIccCard(slotId);
    }

    public CellLocation getCellLocation() {
        return mTelephonyManager.getCellLocation();
    }

    public String getDeviceMeid(Context context) {
        String result = "";
        if (mMeid != null) {
            result = mMeid;

        } else {
            if (initImeiAndMeid(context) && mMeid != null) {
                result = mMeid;
            }
        }

        if (result.length() != LENGTH_MEID) {
            result = VALUE_DEFAULT_MEID;
        }
        return result;
    }

    public String getImei(Context context, int slotId) {
        String result = "";
        if (mImeiArray != null && mImeiArray.length > slotId) {
            result = mImeiArray[slotId];

        } else {
            if (initImeiAndMeid(context) && mImeiArray != null && mImeiArray.length > slotId) {
                result = mImeiArray[slotId];
            }
        }

        if (result.length() != LENGTH_IMEI) {
            result = VALUE_DEFAULT_IMEI;
        }
        return result;
    }

    private Boolean initImeiAndMeid(Context context) {
        Intent intent = context.registerReceiver(null, new IntentFilter(Const.ACTION_IMEI_MEID));
        Log.i(TAG, "[initImeiAndMeid] intent is " + intent);
        if (intent != null) {
            mMeid = intent.getStringExtra("extra_key_meid");
            mImeiArray = intent.getStringArrayExtra("extra_key_imei");

            Log.i(TAG, "[initImeiAndMeid] mMeid is " + mMeid);
            if (mImeiArray != null) {
                Log.i(TAG, "[initImeiAndMeid] IMEI length is " + mImeiArray.length);
                for (int i = 0; i < mImeiArray.length; ++i) {
                    Log.i(TAG, "[initImeiAndMeid] IMEI[" + i + "] is " + mImeiArray[i]);
                }
            } else {
                Log.i(TAG, "[initImeiAndMeid] IMEI array is " + null);
            }

            return true;
        } else {
            return false;
        }
    }

    public boolean isIccIDIdentical(String[] iccIdOnSim, int slotNumber) {
        String[] iccIDFromDevice = AgentProxy.getInstance().getSavedIccId(slotNumber);

        if (iccIdOnSim.length != iccIDFromDevice.length) {
            Log.d(TAG, "ICCID is not the same: array length is not equal.");
            return false;
        }

        for (int i = 0; i < slotNumber; i++) {
            Log.i(TAG, "iccId [" + i + "] on sim is " + iccIdOnSim[i]);
            Log.i(TAG, "iccId [" + i + "] saved is " + iccIDFromDevice[i]);
        }

        if (iccIdOnSim.length == 1) {
            return iccIdOnSim[0].equals(iccIDFromDevice[0]);
        }

        Boolean condition = iccIdOnSim[0].equals(iccIDFromDevice[0])
                && iccIdOnSim[1].equals(iccIDFromDevice[1]);
        condition |= iccIdOnSim[0].equals(iccIDFromDevice[1])
                && iccIdOnSim[1].equals(iccIDFromDevice[0]);

        return condition;
    }

    /**
     * Get ICCID from SIM card.
     * @return Array of ICCIDs of two SIM cards.
     */
    public String[] getIccIDFromCard(int[] slotId) {

        String[] iccIdArray = new String[slotId.length];
        for (int i = 0; i < slotId.length; i++) {
            iccIdArray[i] = Const.ICCID_DEFAULT_VALUE;
        }

        for (int i = 0; i < slotId.length; i++) {
            int[] subId = SubscriptionManager.getSubId(i);

            if (subId != null && subId[0] >= 0) {
                Log.i(TAG, "subId[0] is " + subId[0]);

                String value = mTelephonyManager.getSimSerialNumber(subId[0]);
                Log.i(TAG, "value is " + value);

                if (value != null && value.length() == Const.ICCID_DEFAULT_VALUE.length()) {
                    iccIdArray[i] = value;
                }

            }
            Log.i(TAG, "iccIdArray[" + i + "] is " + iccIdArray[i]);
        }

        return iccIdArray;
    }

    public void registerPhoneListener(PhoneStateListener listener) {
        mTelephonyManager.listen(listener, PhoneStateListener.LISTEN_NONE);
        mTelephonyManager.listen(listener,
                PhoneStateListener.LISTEN_SERVICE_STATE | PhoneStateListener.LISTEN_CELL_LOCATION);
    }

    public void unRegisterPhoneListener(PhoneStateListener listener) {
        Log.i(TAG, "unRegisterPhoneListener");
        mTelephonyManager.listen(listener, PhoneStateListener.LISTEN_NONE);
    }

    //------------------------------------------------------
    // Wrapper of SubscriptionManager
    //------------------------------------------------------

    public static int[] getSubId(int slotId) {
        return SubscriptionManager.getSubId(slotId);
    }

    public static int getDefaultDataSubId() {
        int subId = SubscriptionManager.getDefaultDataSubId();
        return subId;
    }

    public static int getSlotId(int subId) {
        int slotId = SubscriptionManager.getSlotId(subId);
        return slotId;
    }

    //------------------------------------------------------
    // Function of Alarm and device related
    //------------------------------------------------------

    public static void setExactAlarm(Context context, Class<?> cls, String action, int delay) {
        AlarmManager alarm = (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);
        Intent intent = new Intent(action, null, context, cls);
        PendingIntent pendingIntent = PendingIntent.getService(context, 0, intent,
                PendingIntent.FLAG_ONE_SHOT);

        long triggerAtMillis = System.currentTimeMillis() + delay;
        alarm.setExact(AlarmManager.RTC_WAKEUP, triggerAtMillis, pendingIntent);
    }

    public static String getManufactor() {
        String manufacturer = CustomProperties.getString(CustomProperties.MODULE_DM,
                CustomProperties.MANUFACTURER, VALUE_DEFAULT_MANUFACTURER);
        Log.i(TAG, "manufacturer is " + manufacturer);
        return manufacturer;
    }

    public static String getSoftwareVersion() {
        String version = CustomProperties.getString(CustomProperties.MODULE_DM,
                "SoftwareVersion", VALUE_DEFALUT_SOFTWARE_VERSION);
        Log.i(TAG, "software version is " + version);
        return version;
    }

    public static String getHardwareVersion() {
        long systemSize = getDirectorySize(Environment.getRootDirectory());
        long dataSize = getDirectorySize(Environment.getDataDirectory());

        Log.d(TAG, "systemSize:" + systemSize + "\n" + "dataSize:" + dataSize);

        long totalSize = analyseTotalStorage(systemSize + dataSize);

        return totalSize + "G";
    }

    /**
     * If the size locate in the section of 8-16, returns 16;
     * If the size locate in the section of 16-32, returns 32;
     * And so on.
     * @param size
     * @return Integer like "16", "32", "64", Unit: G.
     */
    private static int analyseTotalStorage(long size) {
        double total = ((double) size) / (1024 * 1024 * 1024);
        Log.d(TAG, "analyseTotalStorage(), total: " + total);

        int storageSize = 1;
        while (total > storageSize) {
            storageSize = storageSize << 1;
        }

        return storageSize;
    }

    @SuppressWarnings("deprecation")
    private static long getDirectorySize(File path) {
        StatFs stat = new StatFs(path.getPath());
        long blockSize = stat.getBlockSize();
        long totalBlocks = stat.getBlockCount();
        return blockSize * totalBlocks;
    }

    //------------------------------------------------------
    // Function Related to shared preferences
    //------------------------------------------------------
    public static SharedPreferences getUniquePreferences(Context context) {
        return PreferenceManager.getDefaultSharedPreferences(context);
    }

    public static SharedPreferences getSimPreference(Context context, int index) {
        if (index == 0) {
            return context.getSharedPreferences(PREFERENCE_FILE_0, Context.MODE_PRIVATE);
        } else {
            return context.getSharedPreferences(PREFERENCE_FILE_1, Context.MODE_PRIVATE);
        }
    }

    public static void clearPreferences(Context context) {
        getUniquePreferences(context).edit().clear().commit();
        getSimPreference(context, 0).edit().clear().commit();
        getSimPreference(context, 1).edit().clear().commit();
    }

    //------------------------------------------------------
    // Function to indicator file
    //------------------------------------------------------
    public static boolean hasIndicator(Context context) {
        File dataDir = context.getFilesDir();
        File file = new File(dataDir.getAbsolutePath() + File.separator + INDICATOR_FILE_NAME);
        return file.isFile();
    }

    public static boolean removeIndicator(Context context) {
        File dataDir = context.getFilesDir();
        File file = new File(dataDir.getAbsolutePath() + File.separator + INDICATOR_FILE_NAME);
        return file.delete();
    }

    public static void createIndicator(Context context) {
        FileOutputStream fos = null;

        try {
            fos = context.openFileOutput(INDICATOR_FILE_NAME, Context.MODE_PRIVATE);
            fos.write(Long.toString(System.currentTimeMillis()).getBytes());

        } catch (FileNotFoundException e) {
            Log.e(TAG, "Cannot create indicator: " + e.getLocalizedMessage());
            e.printStackTrace();

        } catch (IOException e) {
            Log.e(TAG, "IOException: " + e.getLocalizedMessage());
            e.printStackTrace();

        } finally {
            if (fos != null) {
                try {
                    fos.close();
                } catch (IOException e) {
                    Log.e(TAG, "Faled to close output stream: " + e.getLocalizedMessage());
                    e.printStackTrace();
                }
            }
        }
    }

    /*
     * Show sim's info
     */
    public void showSimInfo(Context context, int[] slotIds) {

        Log.i(TAG, "----- showSimInfo start -----");

        String meid = getDeviceMeid(context);
        Log.i(TAG, "Meid is " + meid);

        for (int i = 0; i < slotIds.length; ++i) {

            int slotId = slotIds[i];

            String deviceId = mTelephonyManager.getDeviceId(slotId);
            Log.i(TAG, "Device id for slot " + slotId + " is " + deviceId);

            if (mTelephonyManager.hasIccCard(slotId)) {
                Log.i(TAG, "****** slot " + slotId);

                int[] subId = SubscriptionManager.getSubId(i);

                if (subId != null && subId[0] >= 0) {
                    Log.i(TAG, "subId[0] is " + subId[0]);

                    String iccid = mTelephonyManager.getSimSerialNumber(subId[0]);
                    String imsi = mTelephonyManager.getSubscriberId(subId[0]);
                    Log.i(TAG, "imsi/iccid is " + imsi + "/" + iccid);
                } else {
                    Log.i(TAG, "Ivalid sub Id.");
                }
            } else {
                Log.i(TAG, "slot " + slotId + " is empty.");
            }
        }

        Log.i(TAG, "----- showSimInfo end -----");
    }

    //------------------------------------------------------
    // Service priority
    //------------------------------------------------------
    public static void stayForeground(Service service) {
        Log.i(TAG, "Start service to foreground");
        Notification notify = new Notification.Builder(service).setSmallIcon(R.drawable.icon)
              .build();
        notify.flags |= Notification.FLAG_HIDE_NOTIFICATION;
        service.startForeground(1, notify);
    }

    public static void leaveForeground(Service service) {
        Log.i(TAG, "Stop service to foreground");
        service.stopForeground(true);
    }
}
