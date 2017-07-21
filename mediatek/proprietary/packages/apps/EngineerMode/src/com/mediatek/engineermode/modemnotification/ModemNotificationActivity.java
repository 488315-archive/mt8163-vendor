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

package com.mediatek.engineermode.modemnotification;

import android.app.AlertDialog;
import android.app.Activity;
import android.app.ActivityManager;
import android.app.Service;
import android.os.AsyncResult;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.CompoundButton.OnCheckedChangeListener;
import android.widget.Toast;
import android.widget.Button;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.Intent;
import android.content.ServiceConnection;
import android.telephony.TelephonyManager;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.view.View;

import com.mediatek.engineermode.Elog;
import com.mediatek.engineermode.R;

import java.util.List;


/**
 * MD Notification
 */
public class ModemNotificationActivity extends Activity implements OnCheckedChangeListener {
    private static final String TAG = "EM/ModemNotification";
    private static final String MODEM_NOTIFICATION_SHAREPRE =
           "telephony_modem_notification_settings";
    private static final int TOTAL_NOTIFICATION_ITEM = 1;
    private CheckBox[] mCheckBox;
    private int mChecked = 0;
    private int mCheckedShare = 0;


    private static boolean isServiceRunning(Context context, String className) {
         boolean isRunning = false;
         ActivityManager am = (ActivityManager) context.getSystemService(Context.ACTIVITY_SERVICE);
         int maxCount = 100;
         List<ActivityManager.RunningServiceInfo> runningServices =
             am.getRunningServices(maxCount);
         while (runningServices.size() == maxCount) {
             maxCount += 50;
             runningServices = am.getRunningServices(maxCount);
         }

         for (int i = 0; i < runningServices.size(); i++) {
             ActivityManager.RunningServiceInfo info = runningServices.get(i);
             //Elog.d(TAG, "isServiceRunning service name " + info.service.getClassName());
             if (info.service.getClassName().equals(className)) {
                 isRunning = true;
                 break;
             }
         }

         Elog.d(TAG, "isServiceRunning isRunning " + isRunning);
         return isRunning;
     }


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.md_em_notification);

        final SharedPreferences modemNotificationSh = getSharedPreferences(
            MODEM_NOTIFICATION_SHAREPRE, android.content.Context.MODE_PRIVATE);
        mChecked = modemNotificationSh.getInt(getString(R.string.md_checkedshare_value), 0);
        Elog.d(TAG, "onCreate mChecked " + mChecked);

        mCheckBox = new CheckBox[TOTAL_NOTIFICATION_ITEM];

        for (int i = 0; i < TOTAL_NOTIFICATION_ITEM; i++) {
            mCheckBox[i] = null;
        }

        mCheckBox[0] = (CheckBox) findViewById(R.id.md_ca_detected);


        for (int i = 0; i < TOTAL_NOTIFICATION_ITEM; i++) {
            mCheckBox[i].setOnCheckedChangeListener(this);
            mCheckBox[i].setChecked(mChecked==1);
        }

    }

    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    public void onDestroy() {
        writeCheckedSharedPreference();
        super.onDestroy();
    }

    @Override
    public void onCheckedChanged(CompoundButton view, boolean isChecked) {
        for (int i = 0; i < TOTAL_NOTIFICATION_ITEM; i++) {
            if (view == mCheckBox[i]) {
                if (isChecked) {
                    mChecked = 1;
                    if (!isServiceRunning(this,
                        "com.mediatek.engineermode.modemnotification.ModemNotificationService")) {
                        startService(new Intent(this, ModemNotificationService.class));
                        writeSharedPreference(true);
                    }
                } else {
                    mChecked = 0;
                    if (isServiceRunning(this,
                        "com.mediatek.engineermode.modemnotification.ModemNotificationService")) {
                        stopService(new Intent(this, ModemNotificationService.class));
                        writeSharedPreference(false);
                    }
                }
            }
        }

    }


    private void writeCheckedSharedPreference() {
        final SharedPreferences modemNotificationSh = getSharedPreferences(
            MODEM_NOTIFICATION_SHAREPRE, android.content.Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = modemNotificationSh.edit();
        editor.putInt(getString(R.string.md_checkedshare_value), mChecked);
        editor.commit();
    }

    private void writeSharedPreference(boolean flag) {
        final SharedPreferences modemNotificationSh = getSharedPreferences(
            MODEM_NOTIFICATION_SHAREPRE, android.content.Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = modemNotificationSh.edit();
        editor.putBoolean(getString(R.string.md_notification_value), flag);
        editor.commit();
    }

}
