/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly
 * prohibited.
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
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY
 * ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY
 * THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK
 * SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO
 * RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN
 * FORUM.
 * RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
 * LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation
 * ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

package com.mediatek.engineermode.audio;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.Context;
import android.media.AudioManager;
import android.media.AudioSystem;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.TextView;
import android.widget.Toast;

import com.mediatek.engineermode.FeatureSupport;
import com.mediatek.engineermode.R;
import com.mediatek.engineermode.ShellExe;

import java.io.IOException;

public class AudioDebugSession extends Activity implements OnClickListener, OnCheckedChangeListener {
    private static final String TAG = "EM/AudioDebugSession";
    private static final String CUST_XML_PARAM = "GET_CUST_XML_ENABLE";
    private static final String CUST_XML_SET_SUPPORT_PARAM = "SET_CUST_XML_ENABLE=1";
    private static final String CUST_XML_SET_UNSUPPORT_PARAM = "SET_CUST_XML_ENABLE=0";
    private static final String RESULT_SUPPORT = "GET_CUST_XML_ENABLE=1";
    private static final String RESULT_UNSUPPORT = "GET_CUST_XML_ENABLE=0";
    private static final int SET_MAGIASR_TEST_ENABLE = 0xA2;
    private static final int GET_MAGIASR_TEST_ENABLE = 0xA3;
    private static final int SET_AECREC_TEST_ENABLE = 0xA4;
    private static final int GET_AECREC_TEST_ENABLE = 0xA5;

    private static final int DIALOG_ID_REBOOT = 1;


    private Button mBtnDetect;
    private TextView mTvDetect;
    private CheckBox mCbMagi;
    private CheckBox mCbAecRec;
    private CheckBox mCbCustParam;
    private Toast mToast = null;
    private AudioManager mAudioMgr = null;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.audio_debugsession);
        mAudioMgr = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
        mBtnDetect = (Button) findViewById(R.id.Audio_Headset_Detect_Button);
        mBtnDetect.setOnClickListener(this);
        mTvDetect = (TextView) findViewById(R.id.Audio_Headset_Detect_Text);
        mCbMagi = (CheckBox) findViewById(R.id.Audio_MagiAsr);
        mCbAecRec = (CheckBox) findViewById(R.id.Audio_AecRec);
        mCbCustParam = (CheckBox) findViewById(R.id.Audio_Custom_Param);
        TextView tvCustParam =  (TextView) findViewById(R.id.Audio_Custom_Param_Title);
        View divider = findViewById(R.id.Audio_Custom_Param_Divider);
        if (Audio.AudioTuningVer.VER_2_2 != Audio.getAudioTuningVer()) {
            mCbCustParam.setVisibility(View.GONE);
            tvCustParam.setVisibility(View.GONE);
            divider.setVisibility(View.GONE);
        } else {
            if (FeatureSupport.isEngLoad()) {
                mCbCustParam.setChecked(true);
                mCbCustParam.setEnabled(false);
            } else {
                String check = mAudioMgr.getParameters(CUST_XML_PARAM);
                if ((check != null) && (RESULT_SUPPORT.equals(check))) {
                    mCbCustParam.setChecked(true);
                } else {
                    mCbCustParam.setChecked(false);
                }
                mCbCustParam.setOnCheckedChangeListener(this);
            }
        }
        int ret = AudioSystem.getAudioCommand(GET_MAGIASR_TEST_ENABLE);
        Log.d("@M_" + TAG, "getAudioCommand(0xA3) ret " + ret);
        if (ret == 0) {
            findViewById(R.id.Audio_MagiAsr_Group).setVisibility(View.GONE);
        } else {
            mCbMagi.setChecked(ret == 1);
            mCbMagi.setOnCheckedChangeListener(this);
        }

        ret = AudioSystem.getAudioCommand(GET_AECREC_TEST_ENABLE);
        Log.d("@M_" + TAG, "getAudioCommand(0xA5) ret " + ret);
        mCbAecRec.setChecked(ret == 1);
        mCbAecRec.setOnCheckedChangeListener(this);
    }

    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
        if (buttonView.equals(mCbMagi)) {
            int ret = AudioSystem.setAudioCommand(SET_MAGIASR_TEST_ENABLE, isChecked ? 1 : 0);
            Log.d("@M_" + TAG, "setAudioCommand(0xA2, " + (isChecked ? 1 : 0) + ") ret " + ret);
            if (ret == -1) {
                showToast("set audio parameter 0xA2 failed.");
            }
        } else if (buttonView.equals(mCbAecRec)) {
            int ret = AudioSystem.setAudioCommand(SET_AECREC_TEST_ENABLE, isChecked ? 1 : 0);
            Log.d("@M_" + TAG, "setAudioCommand(0xA4, " + (isChecked ? 1 : 0) + ") ret " + ret);
            if (ret == -1) {
                showToast("set audio parameter 0xA4 failed.");
            }
        } else if (buttonView.equals(mCbCustParam)) {
            mAudioMgr.setParameters(mCbCustParam.isChecked() ?
                    CUST_XML_SET_SUPPORT_PARAM : CUST_XML_SET_UNSUPPORT_PARAM);
            showDialog(DIALOG_ID_REBOOT);
        }
    }

    @Override
    public void onClick(View arg0) {
        if (arg0.getId() == mBtnDetect.getId()) {
            int ret = ShellExe.RESULT_FAIL;
            String cmd = "cat /sys/class/switch/h2w/state";
            try {
                ret = ShellExe.execCommand(cmd, true);
                if (ShellExe.RESULT_SUCCESS == ret) {
                    int mode = Integer.parseInt(ShellExe.getOutput());
                    Log.d("@M_" + TAG, "/sys/class/switch/h2w/state: " + ShellExe.getOutput());
                    if (mode == 1) {
                        mTvDetect.setText(getString(R.string.Audio_Headset_Jak_Headset));
                    } else if (mode == 2) {
                        mTvDetect.setText(getString(R.string.Audio_Headset_Jak_Headphone));
                    } else {
                        mTvDetect.setText(getString(R.string.Audio_Headset_None));
                    }
                } else {
                    showToast("Detection failed");
                }
            } catch (IOException e) {
                Log.d("@M_" + TAG, cmd.toString() + e.getMessage());
                showToast("Detection failed");
            } catch (NumberFormatException e) {
                showToast("Detection failed");
            }
        }
    }

    private void showToast(String msg) {
        if (mToast != null) {
            mToast.cancel();
        }
        mToast = Toast.makeText(this, msg, Toast.LENGTH_SHORT);
        mToast.show();
    }
    @Override
    public Dialog onCreateDialog(int dialogId) {
        switch (dialogId) {
        case DIALOG_ID_REBOOT:
            return new AlertDialog.Builder(this)
                    .setMessage(R.string.set_cust_xml_msg)
                    .setPositiveButton(android.R.string.ok, null).create();
        default:
            return null;
        }
    }
}

