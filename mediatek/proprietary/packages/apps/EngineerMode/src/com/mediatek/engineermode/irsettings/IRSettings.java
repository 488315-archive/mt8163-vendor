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
package com.mediatek.engineermode.irsettings;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.os.AsyncResult;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.SystemProperties;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Spinner;
import android.widget.Toast;
import android.telephony.TelephonyManager;
import android.telephony.SubscriptionManager;
import com.mediatek.engineermode.ChipSupport;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneProxy;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.PhoneConstants;

import com.mediatek.engineermode.R;

import java.util.ArrayList;
import java.util.List;
/**
 *
 * For setting network mode.
 * @author mtk80137
 *
 */
public class IRSettings extends Activity implements OnClickListener {
    private static final String TAG = "EM/IRSettings";
    private static final String MODE_NONE = "0";
    private static final String MODE_CDMA_ONLY = "1";
    private static final String MODE_GSM_ONLY = "2";
    private static final String MODE_FTA_ONLY = "3";
    private static final int DIALOG_NOTICE = 0;
    private static final String IR_MODE_PROPERTY = "persist.radio.ct.ir.engmode";
    private static final String COMMAND_QUERY = "";
    private static final String COMMAND_QUERY_RESPONSE = "";
    private static final String COMMAND_SET = "AT+EFAKECFG=";
    private static final int OFF = 0;
    private static final int ON = 1;
    private static final int MSG_SET = 0;
    private static final String PREF_FILE = "pref";
    private static final String KEY_CURRENT_VALUE = "current_value";
    private static final int INTEGRITE_SPECIAL_CHIP_ID = 66;
    private int mCurrentSelected = 0;
    private Spinner mIRModeSpinner = null;
    private Spinner mIntegrityOffSpinner = null;
    private Button  mBtDone = null;
    private Button  mBtIntegrityOffDone = null;
    private TextView mIntegrityCheckOff = null;
    private int mInitSelected = 0;
    private Phone mPhone = null;
    private Toast mToast = null;
    private SharedPreferences mPref;
    private int mCdmaSlotId;
    private int mSlotId;
    private int[] mSubId = {0};
    private int mPhoneType = 0;

    private OnItemSelectedListener mIRModeListener = new OnItemSelectedListener() {
        @Override
        public void onItemSelected(AdapterView parent, View v, int pos, long id) {
            Log.d("@M_" + TAG, "onItemSelected " + pos);
            mCurrentSelected = pos;
            if (mCurrentSelected != mInitSelected) {
                mBtDone.setEnabled(true);
            } else {
                mBtDone.setEnabled(false);
            }
        }

        @Override
        public void onNothingSelected(AdapterView parent) {
        }
    };
    @Override
    public void onClick(View view) {
        Log.d("@M_" + TAG, "view_id = " + view.getId());
        if (view.getId() == mBtDone.getId()) {
            if (mCurrentSelected == 1) {
                SystemProperties.set(IR_MODE_PROPERTY, MODE_CDMA_ONLY);
            } else if (mCurrentSelected == 2) {
                SystemProperties.set(IR_MODE_PROPERTY, MODE_GSM_ONLY);
            } else if (mCurrentSelected == 3) {
                SystemProperties.set(IR_MODE_PROPERTY, MODE_FTA_ONLY);
            } else {
                SystemProperties.set(IR_MODE_PROPERTY, MODE_NONE);
            }
            if (mInitSelected != mCurrentSelected) {
                showDialog(DIALOG_NOTICE);
            }
        }else if (view.getId() == mBtIntegrityOffDone.getId()) {
            if (mIntegrityOffSpinner.getSelectedItemPosition() == 0) {
                sendATCommand(new String[] {COMMAND_SET + OFF, ""}, MSG_SET);
            } else if (mIntegrityOffSpinner.getSelectedItemPosition() == 1) {
                sendATCommand(new String[] {COMMAND_SET + ON, ""}, MSG_SET);
            }
        }
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.ir_settings);
        mIRModeSpinner = (Spinner) findViewById(R.id.c2k_ir_mode);
        List<String> modeList = new ArrayList<String>();
        modeList.add("None");
        modeList.add("CDMA Only");
        if (SystemProperties.get("ro.mtk_svlte_lcg_support", "0").equals("1")) {  // 4M
            modeList.add("LTE/GSM Only");
        } else { // 5M
            modeList.add("LTE/WCDMA/GSM Only");
        }
        modeList.add("FTA LTE Only");
        ArrayAdapter<String> adp = new ArrayAdapter<String>(this,
                android.R.layout.simple_spinner_item, modeList);
        adp.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        mIRModeSpinner.setAdapter(adp);
        mIRModeSpinner.setOnItemSelectedListener(mIRModeListener);
        mBtDone = (Button) findViewById(R.id.ir_done_btn);
        mBtDone.setOnClickListener(this);
        mBtDone.setEnabled(false);
        String mode = SystemProperties.get(IR_MODE_PROPERTY, MODE_NONE);
        Log.i("@M_" + TAG, "ir mode : " + mode);
        if (mode.equals(MODE_CDMA_ONLY)) {
            mInitSelected = 1;
            mIRModeSpinner.setSelection(1);
        } else if (mode.equals(MODE_GSM_ONLY)) {
            mInitSelected = 2;
            mIRModeSpinner.setSelection(2);
        } else if (mode.equals(MODE_FTA_ONLY)) {
            mInitSelected = 3;
            mIRModeSpinner.setSelection(3);
        } else {
            mInitSelected = 0;
            mIRModeSpinner.setSelection(0);
        }
        mIntegrityOffSpinner = (Spinner) findViewById(R.id.c2k_ir_integrity_off);
        mBtIntegrityOffDone = (Button) findViewById(R.id.ir_integrity_off_done_btn);
        mBtIntegrityOffDone.setOnClickListener(this);
        mPref = getSharedPreferences(PREF_FILE, MODE_PRIVATE);
        mIntegrityOffSpinner.setSelection(mPref.getInt(KEY_CURRENT_VALUE, OFF));
        mIntegrityCheckOff = (TextView) findViewById(R.id.wcdma_integrity_off);
        if (!ChipSupport.isCurrentChipEquals(INTEGRITE_SPECIAL_CHIP_ID)) {
            mIntegrityOffSpinner.setVisibility(View.GONE);
            mBtIntegrityOffDone.setVisibility(View.GONE);
            mIntegrityCheckOff.setVisibility(View.GONE);
        }
        for (int i = 0; i < 2; i++)
        {
            mSubId = SubscriptionManager.getSubId(i);
            if (mSubId != null) {
                mPhoneType= TelephonyManager.getDefault().getCurrentPhoneType(mSubId[0]);
                if (PhoneConstants.PHONE_TYPE_CDMA == mPhoneType) {
                    mCdmaSlotId = i;
                    break;
                }
            }
        }

        Phone phone = PhoneFactory.getPhone(mCdmaSlotId);
        if (phone == null) {
            Log.i("@M_" + TAG, "proxy is null " );
        }
    }

@Override
    protected Dialog onCreateDialog(int id) {
        Dialog dialog = null;
        AlertDialog.Builder builder = null;
        switch (id) {
        case DIALOG_NOTICE:
            builder = new AlertDialog.Builder(this);
            builder.setTitle(R.string.c2k_ir_dialog);
            builder.setCancelable(false);
            builder.setMessage(getString(R.string.c2k_ir_notice));
            builder.setPositiveButton(R.string.OK,
                    new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            finish();
                        }
                    });
            dialog = builder.create();
            break;
        default:
            Log.d("@M_" + TAG, "error dialog ID");
            break;
        }
        return dialog;
    }

    private final Handler mResponseHander = new Handler() {
        public void handleMessage(final Message msg) {
            switch (msg.what) {
                case MSG_SET:
                    AsyncResult ar = (AsyncResult) msg.obj;
                    if (ar.exception == null) {
                        mPref.edit().putInt(KEY_CURRENT_VALUE,
                             mIntegrityOffSpinner.getSelectedItemPosition()).commit();
                        showToast("Set done.");
                    } else {
                        mPref.edit().putInt(KEY_CURRENT_VALUE,
                             mIntegrityOffSpinner.getSelectedItemPosition()).commit();
                        showToast("Set failed.");
                    }
                break;
            }
        }
    };

    private void sendATCommand(String[] atCommand, int msg) {
        Log.d("@M_" + TAG, "sendATCommand() " + atCommand + ", " + msg);
        Phone phone = PhoneFactory.getPhone(mCdmaSlotId);
        phone.invokeOemRilRequestStrings(atCommand, mResponseHander.obtainMessage(msg));
    }

    private void showToast(String msg) {
        if (mToast != null) {
            mToast.cancel();
        }
        mToast = Toast.makeText(this, msg, Toast.LENGTH_SHORT);
        mToast.show();
    }

}
