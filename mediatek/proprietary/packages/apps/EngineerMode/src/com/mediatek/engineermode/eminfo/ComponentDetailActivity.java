/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

package com.mediatek.engineermode.eminfo;

import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.AsyncResult;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.telephony.TelephonyManager;

import android.view.View;
import android.view.View.OnClickListener;

import android.widget.Button;
import android.widget.FrameLayout;

import android.widget.TextView;
import android.widget.Toast;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.PhoneFactory;

import com.mediatek.engineermode.FeatureSupport;
import com.mediatek.engineermode.Elog;
import com.mediatek.engineermode.ModemCategory;
import com.mediatek.engineermode.R;
import com.mediatek.engineermode.eminfo.UrcParser.Parser;



import java.util.Timer;
import java.util.TimerTask;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class ComponentDetailActivity extends Activity implements OnClickListener {
    private static final String TAG = "EmInfo";
    private static final String MODEM_NOTIFICATION_SHAREPRE =
        "telephony_modem_notification_settings";
    private static final int MSG_NW_INFO = 1;
    private static final int MSG_NW_INFO_URC = 2;
    private static final int MSG_NW_INFO_OPEN = 3;
    private static final int MSG_NW_INFO_CLOSE = 4;
    private static final int MSG_UPDATE_UI = 5;
    private static final int MSG_UNIT_TEST = 6;
    private static final int MSG_UPDATE_UI_URC = 7;
    private static final int TOTAL_TIMER = 1000;
    private static final int FLAG_OR_DATA = 0xFFFFFFF7;
    private static final int FLAG_OFFSET_BIT = 0x08;
    private static final int FLAG_DATA_BIT = 8;
    private static final String SERVICE_NO_CHANGE = "04";
    private static final String RRC_NO_CHANGE = "06";
    private static final String NO_CHANGE = "FEFF";
    private static final int CDMA_INTERVAL = 2;
    private static final int UPDATE_INTERVAL = 50;

    private Button mPageUp;
    private Button mPageDown;
    private FrameLayout mInfoFrameLayout;
    private Toast mToast = null;
    private TextView mTitle;

    private int mItemCount = 0;
    private int mCurrentItemIndex = 0;
    private Component mCurrentItem;
    private int[] mItem;
    private int mSimType;
    private Phone mPhone = null;
    private Timer mTimer = new Timer();
    private int mFlag = 0;
    private Handler mUrcHandler;
    private final HandlerThread mHandlerThread = new HandlerThread("components_urc_handler");
    private Handler mUpdateUiHandler;

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
                Toast.makeText(ComponentDetailActivity.this,
                        "Return type error", Toast.LENGTH_SHORT).show();
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
                    if (data == null || data.length < 1 || data[0] == null) {
                        return;
                    }
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
                // fall through
            case MSG_NW_INFO_OPEN:
            case MSG_NW_INFO_CLOSE:
                ar = (AsyncResult) msg.obj;
                if (ar.exception != null) {
                    Toast.makeText(ComponentDetailActivity.this, getString(R.string.send_at_fail),
                            Toast.LENGTH_SHORT);
                }
                break;
            default:
                break;
            }
        }
    };

    private static final String[] ss = new String[] {"0", "1", "2", "3", "4", "5", "6", "7",
            "8", "9", "A", "B", "C", "D", "E", "F"};
    private final Handler mUiHandler = new Handler() {
        public void handleMessage(Message msg) {
            Elog.v(TAG, "handleMessage " + msg.what);
            if (msg.what == MSG_UNIT_TEST) {
                Random rand = new Random();
                StringBuilder info = new StringBuilder();
                for (int i = 0; i < 1000; i++) {
                    info.append(ss[rand.nextInt(16)]);
                }
                Elog.v(TAG, "info " + info.toString());
                for (int i = 0; i < 600; i++) {
                    Message m = mUrcHandler.obtainMessage(MSG_NW_INFO_URC);
                    AsyncResult.forMessage(m,
                         new String[] {String.valueOf(i), info.toString()}, null);
                    mUrcHandler.sendMessage(m);
                }
            }
            if (msg.what == MSG_UPDATE_UI) {
                showNetworkInfo();
            }
        }
    };

    ArrayList<Integer> mCheckedEmTypes;
    List<Component> mComponents;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.component_detail);
        mTitle = (TextView) findViewById(R.id.detail_title);
        mInfoFrameLayout = (FrameLayout) findViewById(R.id.detail_frame);
        mPageUp = (Button) findViewById(R.id.NetworkInfo_PageUp);
        mPageDown = (Button) findViewById(R.id.NetworkInfo_PageDown);
        mPageUp.setOnClickListener(this);
        mPageDown.setOnClickListener(this);

        Intent intent = getIntent();
        mSimType = intent.getIntExtra("mSimType", PhoneConstants.SIM_ID_1);
        ArrayList<String> checked = (ArrayList<String>) intent.getSerializableExtra("mChecked");
        List<Component> allComponents = Component.getComponents(this);
        mComponents = new ArrayList<Component>();
        if (checked == null) {
            Elog.d(TAG, "checked is null");
        } else {
            for (String s : checked) {
                for (Component c : allComponents) {
                    if (s.equals(c.getName())) {
                        mComponents.add(c);
                        break;
                    }
                }
            }
        }

        mItemCount = mComponents.size();
        mCheckedEmTypes = new ArrayList<Integer>();
        for (Component c : mComponents) {
            int[] types = c.getEmType();
            if (types == null) {
                continue;
            }
            for (int i = 0; i < types.length; i++) {
                int type = types[i];
                if (!mCheckedEmTypes.contains(type)) {
                    mCheckedEmTypes.add(type);
                }
            }
        }
        mUpdateUiHandler = new Handler() {
            public void handleMessage(Message msg) {
                switch (msg.what) {
                    case MSG_UPDATE_UI_URC:
                        int type = msg.arg1;
                        Parser decodedData = (Parser)msg.obj;
                        if (decodedData != null) {
                            for (Component c : mComponents) {
                                if (c.hasEmType(type)) {
                                    Elog.v(TAG, "Update data to " +c.getName() +
                                           ", type = " + type + ", data = " + decodedData);
                                    c.update(type, decodedData);
                                }
                            }
                        }
                        updateUI();
                        break;
                    default:
                        break;
                }
            }
        };

        registerNetwork();

        updateUI();

    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    public void onStop() {
        mTimer.cancel();
        super.onStop();
    }

    @Override
    public void onDestroy() {
        unregisterNetwork();
        super.onDestroy();
    }

    @Override
    public void onClick(View arg0) {
        if (mItemCount != 0) {
            if (arg0.getId() == mPageUp.getId()) {
                mCurrentItemIndex = (mCurrentItemIndex - 1 + mItemCount) % mItemCount;
                updateUI();
            } else if (arg0.getId() == mPageDown.getId()) {
                mCurrentItemIndex = (mCurrentItemIndex + 1) % mItemCount;
                updateUI();
            }
        }
    }

    public void updateUI() {
        if (mCurrentItem != null) {
            mCurrentItem.removeView();
        }
        if (mComponents == null || mComponents.size() < 1) {
            Elog.e(TAG, "updateUI mComponents is null");
            return;
        }
        mCurrentItem = mComponents.get(mCurrentItemIndex);
        if (mCurrentItem == null) {
            Elog.e(TAG, "updateUI mCurrentItem is null");
            return;
        }
        mTitle.setText(mCurrentItem.getName());
        View view = mCurrentItem.getView();
        if (view == null) {
            Elog.e(TAG, "updateUI view is null");
            return;
        }
        mInfoFrameLayout.removeAllViews();
        mInfoFrameLayout.addView(view);
        mTimer.cancel();
        mTimer = new Timer();
        mTimer.schedule(new TimerTask() {
            @Override
            public void run() {
                Elog.v(TAG, "Send MSG_UNIT_TEST");
            }
        }, TOTAL_TIMER, TOTAL_TIMER);
    }

    private void showNetworkInfo() {
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

        final SharedPreferences modemNotificationSh = getSharedPreferences(
            MODEM_NOTIFICATION_SHAREPRE, android.content.Context.MODE_PRIVATE);
        boolean enableModemNotification = modemNotificationSh.getBoolean(
            getString(R.string.md_notification_value), false);
        Elog.d(TAG, "unregisterNetwork enableModemNotification " + enableModemNotification);
        if (enableModemNotification) {
            Elog.d(TAG, "Since enable MD Notification, do not send AT CMD to unregister");
            return;
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
}
