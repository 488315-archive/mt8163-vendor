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

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.OutputStream;
import java.io.IOException;
import java.io.OutputStream;
import com.android.internal.widget.LockPatternUtils;

/**
 * The service to write upgrade package path.
 * @author mtk80357
 *
 */
public class WriteCommandService extends Service {
    private static final String COMMAND_PATH = "/cache/recovery";
    private static final String COMMAND_FILE = "/cache/recovery/command";
    private static final String COMMAND_PART1 = "--update_package=";
    private static final String COMMAND_PART2 = "COMMANDPART2";
    private static final String NAND_PROJECT = "nand_project";
    private static final String UNCRYPT_FILE_NAME = "/cache/recovery/uncrypt_file";

    private static final String TAG = "SystemUpdate/WriteCmd";

    private String package_path = "";
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {

        String part2 = intent.getStringExtra(COMMAND_PART2);
        package_path = part2;
        boolean nand_project = intent.getBooleanExtra(NAND_PROJECT, false);
        Log.i("@M_" + TAG, "onStartCommand. nand_project=" + nand_project);

        Log.i("@M_" + TAG, "onStartCommand. path=" + part2);
        addCommandFile(COMMAND_PART1 + part2, nand_project);
        return START_STICKY;
    }

    private void addCommandFile(String strCmdLine, boolean nand_project) {
        Log.i(TAG, "addCommandFile: cmd = " + strCmdLine);

        if (strCmdLine == null) {
            return;
        }

        OutputStream commandfile = null;
        try {
            File recovery = new File(COMMAND_PATH);
            if (!recovery.exists()) {
                boolean cr = recovery.mkdirs();
                Log.i(TAG, "addCommandFile:create recovery folder result =" + cr);
            }

            if (LockPatternUtils.isDeviceEncryptionEnabled() && package_path.startsWith("/data/")) {
                Log.i(TAG, "addCommandFile:uncrypt file path=" + package_path);
                File UNCRYPT_FILE = new File(UNCRYPT_FILE_NAME);

                if (UNCRYPT_FILE.exists()) {
                    UNCRYPT_FILE.delete();
                    Log.i(TAG,
                            "addCommandFile:delete existed command file");
                }
                UNCRYPT_FILE.createNewFile();

                FileWriter uncryptFile = new FileWriter(UNCRYPT_FILE);
                try {
                    uncryptFile.write(package_path + "\n");
                } finally {
                    uncryptFile.close();
                }
                package_path = "@/cache/recovery/block.map";
                final String filenameArg = "--update_package=" + package_path;
                strCmdLine = filenameArg;
            }

            File file = new File(COMMAND_FILE);
            if (file.exists()) {
                file.delete();
                Log.i(TAG,
                        "addCommandFile:delete existed command file");
            }
            file.createNewFile();
            commandfile = new BufferedOutputStream(new FileOutputStream(file));
            commandfile.write(strCmdLine.getBytes());

        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            if (commandfile != null) {
                try {
                    commandfile.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
            stopSelf();
        }
    }
}
