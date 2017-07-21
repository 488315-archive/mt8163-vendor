/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * public under relevant copyright laws. The information contained herein is
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

package com.mediatek.engineermode.desenseat;

import android.annotation.TargetApi;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.bluetooth.BluetoothAdapter;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.graphics.Point;
import android.location.Location;
import android.location.LocationListener;
import android.media.RingtoneManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.PowerManager;
import android.os.storage.StorageManager;
import android.os.storage.StorageVolume;
import android.telephony.TelephonyManager;
import android.text.Html;


import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.PhoneFactory;

import com.mediatek.engineermode.Elog;
import com.mediatek.engineermode.FeatureSupport;
import com.mediatek.engineermode.ModemCategory;
import com.mediatek.engineermode.R;
import com.mediatek.engineermode.ShellExe;
import com.mediatek.engineermode.bluetooth.BtTest;
import com.mediatek.engineermode.emsvr.AFMFunctionCallEx;
import com.mediatek.engineermode.emsvr.FunctionReturn;
import com.mediatek.engineermode.wifi.WiFiStateManager;
import com.mediatek.storage.StorageManagerEx;

import java.io.File;
import java.io.IOException;
import java.util.List;

/**
 * Common functions.
 */
public class Util {
    public static final String TAG = "EM/DesenseAT";

    private static final String SHELL_CMD = "/system/bin/sh";
    private static final String SHELL_CAT = "-c";
    private static final String CMD_LCD_ON = "echo 255 > /sys/class/leds/lcd-backlight/brightness";
    private static final String CMD_LCD_OFF = "echo 0 > /sys/class/leds/lcd-backlight/brightness";
    private static final String FAIL_STRING = "FFFFFFFF";
    static final String HISTORY_PATH = "desense_at_history_data";
    private static final int RETURN_FAIL = -1;
    public static final int RETURN_SUCCESS = 0;
    private static final int FB0_LCM_POWER_ON = 4;
    private static final int FB0_LCM_POWER_OFF = 5;
    private static final int ID_FINISH_NOTIFICATION = 10;
    static final CharSequence TESTING = Html.fromHtml("<font color='#FFFF00'>Testing</font>");
    static final CharSequence PASS = Html.fromHtml("<font color='#00FF00'>PASS</font>");
    static final CharSequence FAIL = Html.fromHtml("<font color='#FF0000'>FAIL</font>");
    static final CharSequence CONN_FAIL = Html.fromHtml("<font color='#FF0000'>NA</font>");
    private static int sWidth;
    private static int sHeight;


    private static final LocationListener sLocListener = new LocationListener() {
        @Override
        public void onLocationChanged(Location location) {
        }

        @Override
        public void onProviderDisabled(String provider) {
        }

        @Override
        public void onProviderEnabled(String provider) {
        }

        @Override
        public void onStatusChanged(String provider, int status, Bundle extras) {
        }
    };

    static void setResolution(int x, int y) {
        Elog.i(TAG, "setResolution: " + x + "  " + y);
        sWidth = x;
        sHeight = y;
    }

/*
    public static void sleep(long time) {
        Elog.d(TAG, "sleep " + time);
        // TODO: not exactly
        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
        }
    }
*/
    static void sendAtCommand(String str1, String str2, int what) {
        String[] cmd = new String[2];
        cmd[0] = str1;
        cmd[1] = str2;
        Elog.i(TAG, "send: " + cmd[0]);
        Phone phone;
        if (TelephonyManager.getDefault().getPhoneCount() > 1) {
            phone = PhoneFactory.getPhone(PhoneConstants.SIM_ID_1);
        } else {
            phone = PhoneFactory.getDefaultPhone();
        }

        if (FeatureSupport.isSupported(FeatureSupport.FK_MTK_C2K_SUPPORT)) {
            if (FeatureSupport.isSupported(FeatureSupport.FK_MTK_SVLTE_SUPPORT)) {
                phone = ModemCategory.getCdmaPhone();
            }
            if (FeatureSupport.isSupported(FeatureSupport.FK_EVDO_DT_SUPPORT)
                    && phone.getPhoneType() == PhoneConstants.PHONE_TYPE_CDMA) {
                phone = PhoneFactory.getPhone(PhoneConstants.SIM_ID_2);
            }
        }

        phone.invokeOemRilRequestStrings(cmd, null);
    }

    static boolean isIntentAvailable(Context context, Intent intent) {
        final PackageManager packageManager = context.getPackageManager();
        List<ResolveInfo> list = packageManager.queryIntentActivities(intent,
                PackageManager.GET_ACTIVITIES);
        return list.size() > 0;
    }

    static void enterFlightMode(Context context) {
        Elog.d(TAG, "enterFlightMode");
        Intent intent = new Intent(Intent.ACTION_AIRPLANE_MODE_CHANGED);
        intent.putExtra("state", true);
        context.sendBroadcast(intent);
    }

    static void leaveFlightMode(Context context) {
        Elog.d(TAG, "leaveFlightMode");
        Intent intent = new Intent(Intent.ACTION_AIRPLANE_MODE_CHANGED);
        intent.putExtra("state", false);
        context.sendBroadcast(intent);
    }

    static WakeLock sWakeLock;
    static boolean enterSleepMode(Context context) {
        Elog.d(TAG, "enterSleepMode");
        sWakeLock = new WakeLock();
        sWakeLock.acquire(context);
        try {
            return (ShellExe.execCommand("input keyevent 26", true) == RETURN_SUCCESS);
        } catch (IOException e) {
            return false;
        }
    }

    static boolean leaveSleepMode(Context context) {
        Elog.d(TAG, "leaveSleepMode");
        try {
            int result = ShellExe.execCommand("input keyevent 26", true);
            sWakeLock.release();
            return (result == RETURN_SUCCESS);
        } catch (IOException e) {
            return false;
        }

    }


    static void clickPoint(Point point, boolean landscape) {

        Point clickPoint = getClickPoint(point, landscape);
        String cmd = "input tap " + clickPoint.x + " " + clickPoint.y;
        try {
            ShellExe.execCommand(cmd, true);
        } catch (IOException e) {
            e.printStackTrace();
        }


    }

    /**
     * Wake lock manager class.
     *
     */
    static class WakeLock {
        private PowerManager.WakeLock mScreenWakeLock = null;
        private PowerManager.WakeLock mCpuWakeLock = null;

        /**
         * Acquire CPU wake lock.
         *
         * @param context
         *            Global information about an application environment
         */
        void acquireCpuWakeLock(Context context) {
            Elog.v(TAG, "Acquiring cpu wake lock");
            if (mCpuWakeLock != null) {
                return;
            }

            PowerManager pm = (PowerManager) context
                    .getSystemService(Context.POWER_SERVICE);

            mCpuWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK
                    | PowerManager.ACQUIRE_CAUSES_WAKEUP, TAG);
            // | PowerManager.ON_AFTER_RELEASE, TAG);
            mCpuWakeLock.acquire();
        }

        /**
         * Acquire wake lock.
         *
         * @param context
         *            Global information about an application environment
         */
        void acquire(Context context) {
            acquireCpuWakeLock(context);
        }

        /**
         * Release wake lock.
         */
        void release() {
            Elog.v(TAG, "Releasing wake lock");
            if (mCpuWakeLock != null) {
                mCpuWakeLock.release();
                mCpuWakeLock = null;
            }
            if (mScreenWakeLock != null) {
                mScreenWakeLock.release();
                mScreenWakeLock = null;
            }
        }
    }

    static void enableWifi(Context context) {
        Elog.d(TAG, "enableWifi");
        new WiFiStateManager(context).checkState(context);
    }

    static void disableWifi(Context context) {
        Elog.d(TAG, "disableWifi");
        new WiFiStateManager(context).disableWiFi();
    }

    static boolean disableBt() {
        Elog.d(TAG, "disableBt");
        return BluetoothAdapter.getDefaultAdapter().disable();
    }

    static boolean enableBt() {
        Elog.d(TAG, "enableBt");
        return BluetoothAdapter.getDefaultAdapter().enable();
    }

    static boolean disableBtEm() {
        Elog.d(TAG, "disableBtEm");
        return (new BtTest().unInit() == RETURN_SUCCESS);
    }

    static boolean enableBtEm() {
        Elog.d(TAG, "enableBtEm");
        return (new BtTest().init() == RETURN_SUCCESS);
    }

    static boolean lcmOff() {
        Elog.d(TAG, "lcmOff");
        return (controlFb0(FB0_LCM_POWER_OFF) == RETURN_SUCCESS);
//        EmDsenseLcmAssit.lcmSetTm(0);
    }

    static boolean lcmOn() {
        Elog.d(TAG, "lcmOn");
        return (controlFb0(FB0_LCM_POWER_ON) == RETURN_SUCCESS);
//        EmDsenseLcmAssit.lcmSetTm(1);
    }

    /**
     * TRANSMIT command to driver file.
     *
     * @return result
     */
    private static int controlFb0(int... param) {
        int valueRet = RETURN_FAIL;
        AFMFunctionCallEx fb0Ctrl = new AFMFunctionCallEx();
        // boolean result = fb0Ctrl
        // .startCallFunctionStringReturn(AFMFunctionCallEx.FUNCTION_EM_FB0_IOCTL);
        //
        // if (!result) {
        // return RETURN_FAIL;
        // }

        if (fb0Ctrl
                .startCallFunctionStringReturn(AFMFunctionCallEx.FUNCTION_EM_FB0_IOCTL)) {
            fb0Ctrl.writeParamNo(param.length);
            for (int i : param) {
                fb0Ctrl.writeParamInt(i);
            }

            FunctionReturn resultStr;
            do {
                resultStr = fb0Ctrl.getNextResult();
                // if (resultStr.returnString == "") {
                if (resultStr.mReturnString.isEmpty()) {
                    break;
                } else {
                    if (resultStr.mReturnString.equalsIgnoreCase(FAIL_STRING)) {
                        valueRet = RETURN_FAIL;
                        break;
                    }
                    try {
                        valueRet = Integer.valueOf(resultStr.mReturnString);
                    } catch (NumberFormatException e) {
                        Elog.d(TAG, resultStr.mReturnString);
                        valueRet = RETURN_FAIL;
                    }
                }

            } while (resultStr.mReturnCode == AFMFunctionCallEx.RESULT_CONTINUE);

            if (resultStr.mReturnCode == AFMFunctionCallEx.RESULT_IO_ERR) {
                // error
                valueRet = RETURN_FAIL;
            }
        }
        return valueRet;

    }
    static boolean backlightOff() {

        Elog.d(TAG, "backlightOff");
        String[] cmd = {SHELL_CMD, SHELL_CAT, CMD_LCD_OFF };
        try {
            return (ShellExe.execCommand(cmd) == RETURN_SUCCESS);
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }
    }

    static boolean backlightOn() {
        Elog.d(TAG, "backlightOn");
        String[] cmd = {SHELL_CMD, SHELL_CAT, CMD_LCD_ON };
        try {
            return (ShellExe.execCommand(cmd) == RETURN_SUCCESS);
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }
    }

    static void pressBack() {
        try {
            ShellExe.execCommand("input keyevent 4", true);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    static boolean isFileExist(String filepath) {
        if (filepath == null) {
            return false;
        }
        File file = new File(filepath);
        return (file != null && file.exists());
    }

    static String getTestFilePath(Context context, String filepath) {
        String path = StorageManagerEx.getDefaultPath() +  "/" + filepath;
        Elog.i(TAG, "path :" + path);
        return path;
    }

    private static String getStoragePath(Context context, int index) {
        //return Environment.getExternalStorageDirectory().getPath();
        StorageVolume sv = getMountedVolumeById(context, index);
        if (sv != null) {
             Elog.i(TAG, "sv.getPath() :" + sv.getPath());
            return sv.getPath();
        }
        return null;
    }

    @TargetApi(Build.VERSION_CODES.GINGERBREAD)
    private static StorageVolume getMountedVolumeById(Context context, int index) {
        StorageManager storageMgr = (StorageManager) context.
                getSystemService(Context.STORAGE_SERVICE);
        StorageVolume[] volumes = storageMgr.getVolumeList();
        int mountedIndx = 0;
        for (int i = 0; i < volumes.length; i++) {
            String path = volumes[i].getPath();
            String state = storageMgr.getVolumeState(path);
            if (state.equals(Environment.MEDIA_MOUNTED)) {
                if (mountedIndx == index) {
                    return volumes[i];
                }
                mountedIndx++;
            }
        }
        return null;
    }

    static Point getClickPoint(Point p720p, boolean landscape) {
        if (landscape) {
            Point point = new Point();
            point.x = sHeight * p720p.x / 1280;
            point.y = sWidth - sWidth * p720p.y / 720;
            return point;
        } else {
            Point point = new Point();
            point.x = sWidth * p720p.x / 720;
            point.y = sHeight * p720p.y / 1280;
            return point;
        }

    }

    static void notifyFinish(Context context) {

        NotificationManager nm = (NotificationManager)
                context.getSystemService(Context.NOTIFICATION_SERVICE);
/*        Notification.Builder builder = new Notification.Builder(context);
        builder.setSmallIcon(android.R.drawable.ic_dialog_info).
        setContentTitle(context.getResources().getString(R.string.desense_at)).
        setContentText(context.getResources().getString(R.string.desense_at_finish_notification));
        Intent intent = new Intent(context, clazz);
        PendingIntent pi = PendingIntent.getActivity(this, 0, intent, 0);
        builder.setContentIntent(pi);*/
//        Intent intent = new Intent(context, DesenseAtActivity.class);
//        PendingIntent pi = PendingIntent.getActivity(context, 0, intent, 0);
        Notification notification = new Notification.Builder(context)
                .setSmallIcon(android.R.drawable.ic_dialog_info)
                .setContentText(context.getResources().
                        getString(R.string.desense_at_finish_notification))
                .setContentTitle(context.getResources().getString(R.string.desense_at))
                .setAutoCancel(true)
                .setContentIntent(PendingIntent.getActivity(context, 0, new Intent(), 0))
                .build();
        notification.sound = RingtoneManager.getDefaultUri(RingtoneManager.TYPE_NOTIFICATION);
        notification.flags |= Notification.FLAG_INSISTENT;
        nm.notify(ID_FINISH_NOTIFICATION, notification);
    }

}
