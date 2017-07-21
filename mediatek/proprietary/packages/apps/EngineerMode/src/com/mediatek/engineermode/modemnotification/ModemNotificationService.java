package com.mediatek.engineermode.modemnotification;

import android.app.Service;
import android.content.Intent;
import android.os.AsyncResult;
import android.os.Binder;
import android.os.IBinder;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.telephony.TelephonyManager;
import android.media.AudioManager;
import android.media.SoundPool;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.PhoneFactory;
import com.mediatek.engineermode.Elog;
import com.mediatek.engineermode.R;
import com.mediatek.engineermode.FeatureSupport;
import com.mediatek.engineermode.ModemCategory;
import com.mediatek.engineermode.eminfo.Content;
import com.mediatek.engineermode.eminfo.UrcParser;
import com.mediatek.engineermode.eminfo.UrcParser.Parser;

import java.util.ArrayList;
import java.util.List;
import java.util.HashMap;

/*
 * AT command tool should be able to run commands at background.
 */
public class ModemNotificationService extends Service {
    private static final String TAG = "EM/ModemNotificationService";
    private static final String caDetectedFile = "/system/res/sound/CADetected.ogg";
    private static final int TOTAL_NOTIFICATION_ITEM = 1;
    private static final int MSG_NW_INFO = 1;
    private static final int MSG_NW_INFO_URC = 2;
    private static final int MSG_NW_INFO_OPEN = 3;
    private static final int MSG_NW_INFO_CLOSE = 4;
    private static final int MSG_UPDATE_UI_URC = 7;
    private static final int FLAG_OR_DATA = 0xFFFFFFF7;
    private static final int FLAG_OFFSET_BIT = 0x08;
    private static final int FLAG_DATA_BIT = 8;
    private static final int UPDATE_INTERVAL = 50;
    private SoundPool soundPool = null;
    private HashMap<Integer,Integer> spMap;
    private boolean flag = false;

    //ERRC_EM_MOB_EVENT_IND
    //em_errc_mob_type_enum
    private static final int EM_ERRC_MOB_TYPE_CR = 0;
    private static final int EM_ERRC_MOB_TYPE_REDT = 1;
    private static final int EM_ERRC_MOB_TYPE_CCO = 2;
    private static final int EM_ERRC_MOB_TYPE_HO = 3;
    private static final int EM_ERRC_MOB_TYPE_REEST = 4;
    //em_errc_mob_dir_enum
    private static final int EM_ERRC_MOB_DIR_INTRA_LTE = 0;
    private static final int EM_ERRC_MOB_DIR_TO_LTE = 1;
    private static final int EM_ERRC_MOB_DIR_FROM_LTE = 2;
    //

    //ERRC_EM_RECONF_INFO
    //em_errc_reconf_info_type_enum
    private static final int ERRC_NORMAL_RECONF = 0;
    private static final int ERRC_INTRA_CEL_HO = 1;
    private static final int ERRC_INTER_CEL_HO = 2;
    private static final int ERRC_INTER_RAT_HO = 3;
    //

    //RAC_EM_INFO
    //rac_em_active_rat_info_enum
    private static final int RAC_EM_NO_SERVICE = 0;
    private static final int RAC_EM_LIMITED_SERVICE = 1;
    private static final int RAC_EM_GSM = 2;
    private static final int RAC_EM_UMTS_FDD = 3;
    private static final int RAC_EM_UMTS_TDD = 4;
    private static final int RAC_EM_LTE_FDD = 5;
    private static final int RAC_EM_LTE_TDD = 6;


    private List<String> mCommands = new ArrayList<String>();
    private boolean mSending = false;
    private boolean mFirstCaDetected = true;
    private Phone mPhone = null;
    private int mFlag = 0;
    private boolean mLastStatus = false;
    private boolean mCurStatus = false;
    private Handler mUrcHandler;
    private final HandlerThread mHandlerThread = new HandlerThread("md_notification_urc_handler");
    private Handler mUpdateUiHandler;
    private ArrayList<Integer> mCheckedEmTypes;



    private void urcHandle(Message msg) {
        if (msg.what == MSG_NW_INFO_URC) {
                long startTime = System.currentTimeMillis();

                AsyncResult ar = (AsyncResult) msg.obj;
                String[] data = (String[]) ar.result;
                if ((data.length <= 0) || (data[0] == null) || (data[1] == null)) {
                        return;
                    }
                Elog.v(TAG, "Receive URC: " + data[0] + ", " + data[1]);

                int type = -1;
                try {
                    type  = Integer.parseInt(data[0]);
                } catch (NumberFormatException e) {
                    Elog.e(TAG, "Return type error");
                    return;
                }
                Parser decodedData = UrcParser.parse(type, data[1]);
                Message msgToUi = mUpdateUiHandler.obtainMessage();
                msgToUi.what = MSG_UPDATE_UI_URC;
                msgToUi.arg1 = type;
                msgToUi.obj = decodedData;
                mUpdateUiHandler.sendMessageDelayed(msgToUi, UPDATE_INTERVAL);
            }
        }

    private Handler mATCmdHander = new Handler() {
        public void handleMessage(Message msg) {
                AsyncResult ar;
                switch (msg.what) {
                case MSG_NW_INFO:
                    ar = (AsyncResult) msg.obj;
                    if (ar.exception == null) {
                        String[] data = (String[]) ar.result;
                            if ((data.length > 0) && (data[0] != null)) {
                            Elog.v(TAG, "data[0] is : " + data[0]);
                            Elog.v(TAG, "flag is : " + data[0].substring(FLAG_DATA_BIT));
                            mFlag = Integer.valueOf(data[0].substring(FLAG_DATA_BIT));
                            mFlag = mFlag | FLAG_OFFSET_BIT;
                            Elog.v(TAG, "flag change is : " + mFlag);
                            for (Integer j : mCheckedEmTypes) {
                                String[] atCommand = new String[2];
                                atCommand[0] = "AT+EINFO=" + mFlag + "," + j + ",0";
                                atCommand[1] = "+EINFO";
                                sendATCommand(atCommand, MSG_NW_INFO_OPEN);
                            }
                        }
                    }
                    break;
                case MSG_NW_INFO_OPEN:
                case MSG_NW_INFO_CLOSE:
                    ar = (AsyncResult) msg.obj;
                    if (ar.exception != null) {
                        Elog.e(TAG, "AT cmd confirm error");
                    }
                   break;
                default:
                    break;
                }
            }
        };

    @Override
    public void onCreate() {
        // TODO Auto-generated method stub
        super.onCreate();

        soundPool = new SoundPool(TOTAL_NOTIFICATION_ITEM, AudioManager.STREAM_MUSIC, 0);
        spMap = new HashMap<Integer, Integer>();
        spMap.put(1, soundPool.load(caDetectedFile, 1));

        soundPool.setOnLoadCompleteListener(new SoundPool.OnLoadCompleteListener() {
            @Override
            public void onLoadComplete(SoundPool soundPool, int sampleId, int status) {
                 Elog.d(TAG, "onLoadComplete sampleId " + sampleId);
                 flag = true;
            }
        });

        mUpdateUiHandler = new Handler() {
            public void handleMessage(Message msg) {
                switch (msg.what) {
                    case MSG_UPDATE_UI_URC:
                        int type = msg.arg1;
                        Parser decodedData = (Parser)msg.obj;
                        if (decodedData != null) {
                            int id = checkNotification(type, decodedData);
                            if (id == 1){
                                 Elog.d(TAG, "Need to notify, please play the sound");
                                 playSounds(id);
                            }
                        }
                        break;
                    default:
                        break;
                }
            }
        };
        registerNetwork();

    }

    @Override
    public void onDestroy() {
        // TODO Auto-generated method stub
        Elog.v(TAG, "Enter onDestroy");
        soundPool.release();
        soundPool = null;
        unregisterNetwork();
        super.onDestroy();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Elog.d(TAG, "Enter onStartCommand");
        initNotification(intent);
        return START_NOT_STICKY;
    }


    public boolean isRunning() {
        return mSending;
    }

    private void initNotification(Intent intent) {

        int[] EM_TYPES = new int[] {Content.ERRC_EM_CONN_INFO};

        mCheckedEmTypes = new ArrayList<Integer>();
        for (int i = 0; i < EM_TYPES.length; i++) {
            int type = EM_TYPES[i];
            if (!mCheckedEmTypes.contains(type)) {
                mCheckedEmTypes.add(type);
            }
        }

    }

    private void registerNetwork() {
        Elog.d(TAG, "registerNetwork");
        if (TelephonyManager.getDefault().getPhoneCount() > 1) {
            mPhone = PhoneFactory.getPhone(PhoneConstants.SIM_ID_1);
        } else {
            mPhone = PhoneFactory.getDefaultPhone();
        }

        if (FeatureSupport.isSupported(FeatureSupport.FK_MTK_C2K_SUPPORT)) {
            if ((FeatureSupport.isSupported(FeatureSupport.FK_MTK_SVLTE_SUPPORT))
                    || (FeatureSupport.isSupported(FeatureSupport.FK_SRLTE_SUPPORT))) {
                mPhone = ModemCategory.getCdmaPhone();
            }
            if (FeatureSupport.isSupported(FeatureSupport.FK_EVDO_DT_SUPPORT)
                    && mPhone.getPhoneType() == PhoneConstants.PHONE_TYPE_CDMA) {
                mPhone = PhoneFactory.getPhone(PhoneConstants.SIM_ID_2);
            }
        }

        mHandlerThread.start();
        mUrcHandler = new Handler(mHandlerThread.getLooper()) {
            public void handleMessage(Message msg) {
                urcHandle(msg);
            }
        };
        if (mPhone != null) {
            mPhone.registerForNetworkInfo(mUrcHandler, MSG_NW_INFO_URC, null);
        }

        String[] atCommand = {"AT+EINFO?", "+EINFO"};
        sendATCommand(atCommand, MSG_NW_INFO);
    }

    private void unregisterNetwork() {
        Elog.d(TAG, "unregisterNetwork");
        if (mPhone != null) {
            mPhone.unregisterForNetworkInfo(mUrcHandler);
        }
        mFlag = mFlag & FLAG_OR_DATA;
        Elog.v(TAG, "The close flag is :" + mFlag);
        String[] atCloseCmd = new String[2];
        atCloseCmd[0] = "AT+EINFO=" + mFlag;
        atCloseCmd[1] = "";
        sendATCommand(atCloseCmd, MSG_NW_INFO_CLOSE);
    }

    void sendATCommand(String[] atCommand, Message msg) {
        Elog.d(TAG, "sendATCommand " + atCommand[0]);
        if (mPhone != null) {
            mPhone.invokeOemRilRequestStrings(atCommand, msg);
        }
    }

    private void sendATCommand(String[] atCommand, int msg) {
        Elog.d(TAG, "sendATCommand " + atCommand[0]);
        if (mPhone != null) {
            mPhone.invokeOemRilRequestStrings(atCommand, mATCmdHander.obtainMessage(msg));
        }
    }

    int checkNotification(int type, Parser data) {
        int soundId = -1;
        boolean conditionPass = false;
        switch (type) {
           case Content.ERRC_EM_CONN_INFO:
                List<Boolean> configured = data.getBooleanArray("is_scell_configured");
                for (int i = 0; i < configured.size(); i++) {
                    Elog.d(TAG, "checkNotification i " + i + ", is_scell_configured "
                        + configured.get(i));
                    if (configured.get(i)) {
                        conditionPass = true;
                        break;
                    }
                }
                if (mFirstCaDetected) {
                    mLastStatus = false;
                    mCurStatus = conditionPass;
                    mFirstCaDetected = false;
                } else {
                    mLastStatus = mCurStatus;
                    mCurStatus = conditionPass;
                }
                if ((mLastStatus != mCurStatus) && mCurStatus) {
                    soundId = 1;
                }
                break;
           default:
                break;
      }
     Elog.d(TAG, "checkNotification soundId " + soundId);
     return soundId;
   }

   void playSounds(int sound) {
       AudioManager am = (AudioManager)this.getSystemService(this.AUDIO_SERVICE);
       float audioMaxVolumn = am.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
       float audioCurrentVolumn = am.getStreamVolume(AudioManager.STREAM_MUSIC);
       float volumnRatio = audioCurrentVolumn/audioMaxVolumn;
       if (flag) {
           soundPool.play(spMap.get(sound), volumnRatio, volumnRatio, 1, 0, 1.0f);
       } else {
          Elog.e(TAG, "no playSounds");
       }

   }
}

