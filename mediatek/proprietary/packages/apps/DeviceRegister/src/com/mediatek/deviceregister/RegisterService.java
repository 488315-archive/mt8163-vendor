package com.mediatek.deviceregister;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.PendingIntent;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.telephony.PhoneStateListener;
import android.telephony.ServiceState;
import android.util.Log;

import com.mediatek.deviceregister.utils.AgentProxy;
import com.mediatek.deviceregister.utils.PlatformManager;
import com.mediatek.deviceregister.utils.Utils;

public class RegisterService extends Service {

    private static final String TAG = Const.TAG_PREFIX + "RegisterService";

    private static final int MSG_HANDLE_IN_SERVICE = 0;
    private static final int MSG_HANDLE_STOP_SELF = 1;

    private static final int TIMES_RETRY_MAX = 3;
    private static final int UIM_NONE = -1;

    private static final int SLOT_ID_0 = 0;
    private static final int SLOT_ID_1 = 1;
    private static final int[] SINGLE_UIM_ID = {SLOT_ID_0};
    private static final int[] UIM_ID_LIST = {SLOT_ID_0, SLOT_ID_1};

    private String mMeid;
    private int mCurrentSlotId;
    private int mRetryTimes = 0;

    private int[] mSlotList;
    private String[] mImsiOnSim;

    private Boolean mHasSimReady = false;
    private PhoneStateListener[] mPhoneStateListener;

    private PlatformManager mPlatformManager;
    private SmsSendReceiver mSmsSendReceiver;

    @Override
    public IBinder onBind(Intent arg0) {
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.d(TAG, "onCreate");

        mPlatformManager = new PlatformManager(this);
        PlatformManager.stayForeground(this);

        if (mPlatformManager.isSingleLoad()) {
            mSlotList = RegisterService.SINGLE_UIM_ID;
        } else {
            mSlotList = RegisterService.UIM_ID_LIST;
        }
        mPhoneStateListener = new PhoneStateListener[mSlotList.length];

        for (int i = 0; i < mSlotList.length; ++i) {
            mPhoneStateListener[i] = null;
        }
    }

    @Override
    public void onDestroy() {
        Log.d(TAG, "onDestroy");

        PlatformManager.leaveForeground(this);

        clearMessageInHandleQueue();
        unRegisterSendReceiver();
        unRegisterPhoneListeners();

        super.onDestroy();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d(TAG, "onStartCommand " + intent);
        if (intent != null) {
            String action = intent.getAction();
            if (action != null) {
                if (action.equalsIgnoreCase(Const.ACTION_PRE_BOOT_COMPLETED)) {
                    doAfterPreBoot();

                } else if (action.equalsIgnoreCase(Const.ACTION_SMS_STATE_CHANGED)) {
                    doAfterSmsStateChange();

                } else if (action.equalsIgnoreCase(Const.ACTION_MESSAGE_SEND)) {
                    doAfterMessageSend();

                } else if (action.equalsIgnoreCase(Const.ACTION_CT_CONFIRMED_MESSAGE)) {
                    doAfterConfirmed(intent);
                }
            }
        } else {
            Log.i(TAG, "Intent is null, do nothing.");
        }
        return START_STICKY;
    }

    /*
     * Whether this is preboot due to factory reset or system udpate, both clear register flag.
     */
    private void doAfterPreBoot() {
        AgentProxy.getInstance().resetRegisterFlag();
    }

    private void doAfterSmsStateChange() {
        mHandler.sendEmptyMessageDelayed(MSG_HANDLE_STOP_SELF, Const.ONE_SECOND * 300);
        registerPhoneListeners();
    }

    private void doAfterSimInService() {
        if (mHandler.hasMessages(MSG_HANDLE_STOP_SELF)) {
            mHandler.removeMessages(MSG_HANDLE_STOP_SELF);
        }

        new Thread() {
            public void run() {
                if (needRegister()) {

                    registerSendReceiver();
                    sendRegisterMessage();

                } else {
                    Log.d(TAG, "Phone not need to register.");
                    RegisterService.this.stopSelf();
                }
            }
        } .start();
    }

    private void doAfterMessageSend() {
        AgentProxy.getInstance().resetRegisterFlag();
        AgentProxy.getInstance().setSavedImsi(mImsiOnSim);

        new Thread() {
            public void run() {
                writeEsnToUim();
            }
        } .start();
    }

    private void doAfterConfirmed(Intent intent) {
        Boolean result = mPlatformManager.checkRegisterResult(intent);
        AgentProxy.getInstance().setRegisterFlag(result);
        if (result) {
            PlatformManager.createIndicator(this);
            Log.i(TAG, "Register success!");
        } else {
            Log.i(TAG, "Register failed!");
        }
        stopSelf();
    }

    private void doAfterSendFailed() {
        if (mRetryTimes < TIMES_RETRY_MAX) {
            Log.i(TAG, "Have tried " + mRetryTimes + " times.");
            sendRegisterMessage();
            mRetryTimes++;
        } else {
            Log.i(TAG, "Send failed, reach limit " + mRetryTimes);
            stopSelf();
        }
    }

    private boolean needRegister() {
        Log.v(TAG, "needRegister");

        updateSimState();

        if (mCurrentSlotId == RegisterService.UIM_NONE) {
            Log.w(TAG, "There is no UIM card is connected CDMA net or it is roaming.");
            return false;
        }

        mMeid = mPlatformManager.getDeviceMeid(mCurrentSlotId);
        Log.d(TAG, "mMeid = " + mMeid);
        mImsiOnSim = mPlatformManager.getImsiFromSim(mSlotList);
        if (mImsiOnSim != null) {
            for (int i = 0; i < mImsiOnSim.length; ++i) {
                Log.d(TAG, "mImsiOnSim[" + i + "] = " + mImsiOnSim[i]);
            }
        } else {
            Log.d(TAG, "mImsiOnSim is null");
        }

        if (AgentProxy.getInstance().isRegistered()) {
            Log.d(TAG, "Have registerd, check imsi and meid.");
            Boolean condition = mPlatformManager.isImsiSame(mImsiOnSim, mSlotList) &&
                    mPlatformManager.hasSamePairEsn(mSlotList);

            if (condition) {
                Log.d(TAG, "imsi and meid same");
                return false;
            }
        }

        return true;
    }

    private void updateSimState() {
        mCurrentSlotId = RegisterService.UIM_NONE;

        // CT could in both slot 0 and 1
        if (PlatformManager.supportCTForAllSlots()) {
            for (int slotId : mSlotList) {
                if (mPlatformManager.isSimStateValid(slotId)) {
                    mCurrentSlotId = slotId;
                    break;
                }
            }
        } else {
            // CT could only in slot 0
            if (mPlatformManager.isSimStateValid(SLOT_ID_0)) {
                mCurrentSlotId = SLOT_ID_0;
            }
        }

        Log.d(TAG, "Current logon uim is " + mCurrentSlotId);
    }

    private void sendRegisterMessage() {
        byte[] registerMessage = new RegisterMessage(this).getRegisterMessage();

        Intent intent = new Intent(Const.ACTION_MESSAGE_SEND);
        PendingIntent pendingIntent = PendingIntent.getBroadcast(this, 0, intent,
                PendingIntent.FLAG_UPDATE_CURRENT);

        try {
            mPlatformManager.sendRegisterMessage(registerMessage, pendingIntent, mCurrentSlotId);
        } catch (UnsupportedOperationException e) {
            Log.i(TAG, "UnsupportedOperationException " + e);
            e.printStackTrace();
            doAfterSendFailed();
        }
    }

    private void writeEsnToUim() {
        String pEsn = Utils.getEsnFromMeid(mMeid);
        mPlatformManager.setUimEsn(mCurrentSlotId, pEsn);
    }

    private void registerSendReceiver() {
        if (mSmsSendReceiver == null) {
            mSmsSendReceiver = new SmsSendReceiver();
            registerReceiver(mSmsSendReceiver, new IntentFilter(Const.ACTION_MESSAGE_SEND));
        }
    }

    private void unRegisterSendReceiver() {
        if (mSmsSendReceiver != null) {
            unregisterReceiver(mSmsSendReceiver);
        }
    }

    public String getCurrentMeid() {
        return mMeid;
    }

    public String getCurrentImsi() {
        return mImsiOnSim[mCurrentSlotId];
    }

    private void registerPhoneListeners() {
        Log.i(TAG, "registerPhoneListeners");

        for (int i = 0; i < mSlotList.length; ++i) {

            if (mPhoneStateListener[i] != null) {
                Log.i(TAG, "Slot " + i + " already registered.");
                continue;
            }

            int slotId = i;
            int[] subId = PlatformManager.getSubId(slotId);

            if (subId != null && subId[0] > 0) {
                Log.i(TAG, "subId[0] is " + subId[0] + ", register listener for slot " + slotId);

                mPhoneStateListener[i] = new CustomizedPhoneStateListener(i, subId[0]);
                mPlatformManager.registerPhoneListener(mPhoneStateListener[i]);
            } else {
                Log.i(TAG, "No need to register listener for slot " + slotId);
            }
        }
    }

    private void unRegisterPhoneListeners() {
        Log.i(TAG, "unRegisterPhoneListeners");

        for (int i = 0; i < mPhoneStateListener.length; ++i) {
            if (mPhoneStateListener[i] != null) {
                mPlatformManager.unRegisterPhoneListener(mPhoneStateListener[i]);
                mPhoneStateListener[i] = null;
            }
        }
    }

    private void clearMessageInHandleQueue() {
        if (mHandler.hasMessages(MSG_HANDLE_IN_SERVICE)) {
            mHandler.removeMessages(MSG_HANDLE_IN_SERVICE);
        }
        if (mHandler.hasMessages(MSG_HANDLE_STOP_SELF)) {
            mHandler.removeMessages(MSG_HANDLE_STOP_SELF);
        }
    }

    private void sendDelayHandlerMessage() {
        if (!mHasSimReady) {
            Log.i(TAG, "First time find sim ready, send MSG_HANDLE_IN_SERVICE 60s later");
            mHandler.sendEmptyMessageDelayed(MSG_HANDLE_IN_SERVICE, Const.ONE_SECOND * 60);
        }
        mHasSimReady = true;
    }

    @SuppressLint("HandlerLeak")
    private Handler mHandler = new Handler() {

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_HANDLE_IN_SERVICE:
                    doAfterSimInService();
                    break;

                case MSG_HANDLE_STOP_SELF:
                    stopSelf();
                    break;

                default:
                    super.handleMessage(msg);
            }
        }
    };

    class SmsSendReceiver extends BroadcastReceiver {
        private static final String TAG = Const.TAG_PREFIX + "SmsSendReceiver";

        @Override
        public void onReceive(Context context, Intent intent) {
            Log.i(TAG, "onReceive " + intent);

            String action = intent.getAction();
            if (action.equalsIgnoreCase(Const.ACTION_MESSAGE_SEND)) {
                int resultCode = getResultCode();
                Log.i(TAG, "get result code:" + resultCode);

                if (resultCode == Activity.RESULT_OK) {
                    Log.i(TAG, "Send register message success.");

                    intent.setClass(context, RegisterService.class);
                    context.startService(intent);

                } else {
                    Log.i(TAG, "Send register message failed.");
                    doAfterSendFailed();
                }

            } else {
                Log.i(TAG, "action is not valid." + action);
            }
        }
    }

    private class CustomizedPhoneStateListener extends PhoneStateListener {

        private int mIndex;

        public CustomizedPhoneStateListener(int index, int subId) {
            super(subId);
            mIndex = index;
        }

        @Override
        public void onServiceStateChanged(ServiceState serviceState) {
            super.onServiceStateChanged(serviceState);

            if (serviceState.getState() != ServiceState.STATE_IN_SERVICE) {
                Log.i(TAG, "[onService " + mIndex + "] not in service " + serviceState.getState());

            } else {
                Log.i(TAG, "[onService " + mIndex + "] is in service");
                sendDelayHandlerMessage();
            }
        }
    }
}
