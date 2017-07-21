/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

package com.mediatek.engineermode.worldmode;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.CountDownTimer;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;

import com.android.internal.telephony.TelephonyIntents;
import com.mediatek.internal.telephony.worldphone.WorldMode;
import com.mediatek.internal.telephony.worldphone.WorldPhoneUtil;

import android.telephony.ServiceState;
import com.android.internal.telephony.TelephonyIntents;
import com.mediatek.engineermode.FeatureSupport;
import com.mediatek.engineermode.Elog;
import com.mediatek.engineermode.R;

/**
 * Set world mode.
 */
public class WorldModeActivity extends Activity implements View.OnClickListener {
    private final static String TAG = "EM/ModemSwitch";
    private static final int WORLD_MODE_AUTO = 10;
    private static final int WORLD_MODE_LWCG = 11;
    private static final int C2K_WORLD_MODE_AUTO = 12;
    private static final int WORLD_MODE_LTG = 8;
    private static final int WORLD_MODE_LWG = 9;
    private static final int WORLD_MODE_LTTG = 13;
    private static final int WORLD_MODE_LFWG = 14;
    private static final int WORLD_MODE_LTCTG = 17;
    private static final String ACTION_WORLD_MODE_CHANGED =
            TelephonyIntents.ACTION_WORLD_MODE_CHANGED;
    private static final String ACTION_SERVICE_STATE_CHANGED =
            TelephonyIntents.ACTION_SERVICE_STATE_CHANGED;

    //[ALPS02294161]
    private static final String ACTION_SET_RADIO_CAPABILITY_DONE =
           TelephonyIntents.ACTION_SET_RADIO_CAPABILITY_DONE;

    private static final int REBOOT_RF = 0;
    private static final int DIALOG_WAIT = 1;
    private final int WAIT_TIME = 10;
    private ProgressDialog mProgressDialog;
    private static int mMode = 0;
    private static int mOldMode = 0;
    private static int mSetWorldMode = 0;
    private String[] mWorldMode = {"unknown", "lwg", "lttg"};
    private String[] mWorldModeFor6M = {"unknown", "LtCTG", "LWCG"};

    private RadioButton mRadioLttg;
    private RadioButton mRadioLtctg;
    private RadioButton mRadioLwg;
    private RadioButton mRadioLwcg;
    private RadioButton mRadioAuto;
    private RadioGroup mRadioGroup;
    private TextView mText;
    private EditText mValue;
    private Button mButtonSet;
    private Button mButtonSetValue;
    private Toast mToast;

    CountDownTimer timer;
    private final BroadcastReceiver mBroadcastReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Elog.d(TAG, "[Receiver]+");
            //[ALPS02294161]-start
            String action = intent.getAction();
            if (ACTION_WORLD_MODE_CHANGED.equals(action)) {
                int worldModeState = intent.getIntExtra(
                    TelephonyIntents.EXTRA_WORLD_MODE_CHANGE_STATE, -1);
                if (worldModeState == WorldMode.MD_WM_CHANGED_END) {
                    updateUi();
                }
            } else if (ACTION_SET_RADIO_CAPABILITY_DONE.equals(action)){
                updateUi();
            }
            //[ALPS02294161]-end
            Elog.d(TAG, "[Receiver]-");
        }
    };

    private final BroadcastReceiver mBroadcastReceiverRebootRF = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            Elog.d(TAG, "[Receiver]+");
            ServiceState ss = ServiceState.newFromBundle(intent.getExtras());
            Elog.d(TAG, "ss.getState(): " + ss.getState());
            if (ss.getState() != ServiceState.STATE_POWER_OFF) {
                if (mSetWorldMode == 1) {
                    updateUi();
                }
                if (timer != null) {
                    timer.cancel();
                    timer = null;
                }
                if(mProgressDialog != null){
                    mProgressDialog.dismiss();
                    mSetWorldMode = 0;
                }

            }
            Elog.d(TAG, "[Receiver]-");
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.world_mode);
        mRadioLtctg = (RadioButton) findViewById(R.id.world_mode_ltctg);
        mRadioLttg = (RadioButton) findViewById(R.id.world_mode_lttg);
        mRadioLwg = (RadioButton) findViewById(R.id.world_mode_lwg);
        mRadioLwcg = (RadioButton) findViewById(R.id.world_mode_lwcg);
        mRadioAuto = (RadioButton) findViewById(R.id.world_mode_auto_switch);
        mRadioGroup = (RadioGroup) findViewById(R.id.world_mode_radio_group);
        mButtonSet = (Button) findViewById(R.id.world_mode_set);
        mText = (TextView) findViewById(R.id.world_mode_current_value);
        mValue = (EditText) findViewById(R.id.world_mode_value);
        mButtonSetValue = (Button) findViewById(R.id.world_mode_set_value);

        if (is6m()) {
            mRadioLwg.setVisibility(View.GONE);
            mRadioLttg.setVisibility(View.GONE);
        } else {
            mRadioLtctg.setVisibility(View.GONE);
            mRadioLwcg.setVisibility(View.GONE);
        }

        IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(ACTION_WORLD_MODE_CHANGED);

        //[ALPS02294161]
        intentFilter.addAction(ACTION_SET_RADIO_CAPABILITY_DONE);

        IntentFilter intentFilterServiceStateChanged = new IntentFilter();
        intentFilterServiceStateChanged.addAction(ACTION_SERVICE_STATE_CHANGED);
        registerReceiver(mBroadcastReceiver, intentFilter);
        registerReceiver(mBroadcastReceiverRebootRF, intentFilterServiceStateChanged);
        updateUi();
    }

    @Override
    protected void onDestroy() {
        Elog.d(TAG, "onDestroy()");
        super.onDestroy();
        unregisterReceiver(mBroadcastReceiver);
        unregisterReceiver(mBroadcastReceiverRebootRF);
    }

    private void updateUi() {
        int mode = getWorldMode();
        int currentMode = WorldPhoneUtil.get3GDivisionDuplexMode();
        Elog.d(TAG, "Get world mode: " + mode);
        String modeString = "";
        if ((mode == WORLD_MODE_AUTO) || (mode == C2K_WORLD_MODE_AUTO)) {
            modeString = getString(R.string.world_mode_auto);
            if (is6m()) {
                modeString = modeString + "(" + mWorldModeFor6M[currentMode] + ")";
            } else {
                modeString = modeString + "(" + mWorldMode[currentMode] + ")";
            }
            mRadioAuto.setChecked(true);
        } else if (mode == WORLD_MODE_LTG) {
            modeString = getString(R.string.world_mode_ltg);
            mRadioGroup.check(-1);
        } else if (mode == WORLD_MODE_LWG) {
            modeString = getString(R.string.world_mode_lwg);
            mRadioLwg.setChecked(true);
        } else if (mode == WORLD_MODE_LTTG) {
            modeString = getString(R.string.world_mode_lttg);
            mRadioLttg.setChecked(true);
        } else if (mode == WORLD_MODE_LTCTG) {
            modeString = getString(R.string.world_mode_ltctg);
            mRadioLtctg.setChecked(true);
        } else if (mode == WORLD_MODE_LWCG) {
            modeString = getString(R.string.world_mode_lwcg);
            mRadioLwcg.setChecked(true);
        } else if (mode == WORLD_MODE_LFWG) {
            modeString = getString(R.string.world_mode_lfwg);
            mRadioGroup.check(-1);
        } else {
            modeString = mWorldMode[0];
            mRadioGroup.check(-1);
            showToast("Query world mode failed: " + mode);
        }
        mText.setText(getString(R.string.world_mode_current_value) + modeString);
        mValue.setText(String.valueOf(mode));
        mOldMode = mode;
    }

    @Override
    public void onClick(View v) {
        if (v == mButtonSetValue) {
            int mode = 0;
            try {
                mMode = Integer.parseInt(mValue.getText().toString());
            } catch (NumberFormatException e) {
                Elog.w(TAG, "Invalid format: " + mValue.getText());
                showToast("Invalid value");
            }
        } else if (v == mButtonSet) {
            if (mRadioLwg.isChecked()) {
                mMode = WORLD_MODE_LWG;
            } else if (mRadioLttg.isChecked()) {
                mMode = WORLD_MODE_LTTG;
            } else if (mRadioAuto.isChecked()) {
                if (is6m()) {
                    mMode = C2K_WORLD_MODE_AUTO;
                } else {
                    mMode = WORLD_MODE_AUTO;
                }
            } else if (mRadioLtctg.isChecked()) {
                mMode = WORLD_MODE_LTCTG;
            } else if (mRadioLwcg.isChecked()) {
                mMode = WORLD_MODE_LWCG;
            }else {
                return;
            }
        }
        if (mOldMode != mMode) {
            showDialog(REBOOT_RF);
        } else {
            Elog.i(TAG, "The mode is not changed!");
        }
    }

    @Override
    protected Dialog onCreateDialog(int id) {
        switch (id) {
            case REBOOT_RF:
                return new AlertDialog.Builder(this).setTitle("Hint")
                    .setMessage("This will cost several seconds,\n" +
                                  "UI will hang during reboot RF!")
                    .setPositiveButton("Confirm", new OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            if (which == DialogInterface.BUTTON_POSITIVE) {
                                setWorldMode(mMode);
                                mSetWorldMode = 1;
                                showDialog(DIALOG_WAIT);
                            }
                            dialog.dismiss();
                        }
                    })
                    .setNegativeButton("Cancel", null)
                    .create();

            case DIALOG_WAIT:
                mProgressDialog = new ProgressDialog(this);
                mProgressDialog.setTitle("Waiting");
                mProgressDialog.setMessage("Waiting rf reboot");
                mProgressDialog.setCancelable(false);
                mProgressDialog.setIndeterminate(true);
                timer = new CountDownTimer(WAIT_TIME * 1000, 1000) {
                    @Override
                    public void onTick(long millisUntilFinishied) {
                        Elog.d(TAG, "WAIT_TIME = " + WAIT_TIME);
                        Elog.d(TAG, "millisUntilFinishied = " + millisUntilFinishied);
                    }

                    @Override
                    public void onFinish() {
                        Elog.d(TAG, "millisUntilFinishied finish");
                        mProgressDialog.dismiss();
                        timer.cancel();
                        timer = null;
                    }
                };
                timer.start();
                Elog.d(TAG, "After timer.start(");
                return mProgressDialog;
            }
        return super.onCreateDialog(id);
    }

    private void setWorldMode(int mode) {
        Elog.d(TAG, "Set world mode: " + mode);
        WorldMode.setWorldMode(mode);
        updateUi();
    }

    private int getWorldMode() {
        return WorldMode.getWorldMode();
    }

    private void showToast(String msg) {
        if (mToast != null) {
            mToast.cancel();
        }
        mToast = Toast.makeText(this, msg, Toast.LENGTH_SHORT);
        mToast.show();
    }

    private boolean is6m() {
        boolean result = false;
        if (FeatureSupport.isSupported(FeatureSupport.FK_C2K_SUPPORT)) {
            if ((FeatureSupport.isSupported(FeatureSupport.FK_SVLTE_SUPPORT))
                    || (FeatureSupport.isSupported(FeatureSupport.FK_SRLTE_SUPPORT))) {
                result = true;
            }
        }
        return result;
    }
}
