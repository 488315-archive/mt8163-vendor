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

package com.mediatek.systemupdate.sysoper;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;
import android.content.Context;
import android.os.PowerManager;
import com.android.internal.widget.LockPatternUtils;

/**
 * The service to reboot the phone to recovery mode.
 * @author mtk80357
 *
 */
public class RebootRecoveryService extends Service {


private static final String TAG = "SystemUpdate/RebootRecovery";
private static final String REBOOT_METHOD = "reboot_method";
private static final String NAND_PROJECT = "nand_project";
private static final String INTERNAL_FILE_PATH = "data/data/com.mediatek.systemupdate/" +
        "system_update/update.zip";
private static final int NORMAL_REBOOT = 1;
private static final int PMS_REBOOT = 2;

    @Override
    public IBinder onBind(Intent intent) {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();

    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
         Log.i("@M_" + TAG, "onStartCommand reboot");


        int counter = 0;
        int reboot_type = intent.getIntExtra(REBOOT_METHOD, 0);

        boolean nand_project = intent.getBooleanExtra(NAND_PROJECT, false);
        Log.i("@M_" + TAG, "onStartCommand. nand_project=" + nand_project);

        Log.i("@M_" + TAG, "onStartCommand reboot, reboot_type = " + reboot_type);

        if (reboot_type == NORMAL_REBOOT ||
            (nand_project == true && !LockPatternUtils.isDeviceEncryptionEnabled())) {
            Intent i = new Intent(Intent.ACTION_REBOOT);
            i.putExtra("nowait", 1);
            i.putExtra("interval", 1);
            i.putExtra("window", 0);
            sendBroadcast(i);
        } else {
               try {
            Log.i("@M_" + TAG, "onStartCommand reboot, path = "
            + INTERNAL_FILE_PATH);
                        PowerManager pm = (PowerManager) this
                              .getSystemService(Context.POWER_SERVICE);
                        pm.reboot("recovery");
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        }

        return START_STICKY;
    }
}
