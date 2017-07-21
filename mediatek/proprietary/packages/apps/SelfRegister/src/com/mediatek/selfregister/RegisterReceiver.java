/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2014. All rights reserved.
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

package com.mediatek.selfregister;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.util.Log;

import com.mediatek.selfregister.utils.PlatformManager;

/**
 * A broadcast receiver handles the intent sent by system.
 */
public class RegisterReceiver extends BroadcastReceiver {
    private static final String TAG = Const.TAG_PREFIX + "RegisterReceiver";

    @Override
    public void onReceive(Context context, Intent intent) {

        Log.i(TAG, "onReceive " + intent);

        if(!isSwitchOpen(context)) {
            Log.i(TAG, "Feature is not enabled, do nothing");
            return;
        }

        String action = intent.getAction();
        Log.d(TAG, "Action: " + action);

        // M: Clear all preferences when boot
        if (action.equalsIgnoreCase(Const.ACTION_BOOT_COMPLETED)) {
            PlatformManager.clearPreferences(context);

        } else if (action.equalsIgnoreCase(Const.ACTION_SIM_STATE_CHANGED)) {

            if (intent.getStringExtra(Const.KEY_ICC_STATE).equals(Const.VALUE_ICC_LOADED)) {
                SharedPreferences preference = PlatformManager.getUniquePreferences(context);
                boolean notFirst = preference.getBoolean(Const.PRE_KEY_NOT_FIRST_SUBINFO, false);

                if (notFirst) {
                    Log.i(TAG, "Not the first sim state change broadcast.");
                    return;
                } else {
                    preference.edit().putBoolean(Const.PRE_KEY_NOT_FIRST_SUBINFO, true).commit();
                }
            } else {
                Log.i(TAG, "sim card not loaded, ignore");
                return;
            }
        }

        intent.setClass(context, RegisterService.class);
        context.startService(intent);
    }

    private Boolean isSwitchOpen(Context context) {
        return PlatformManager.isFeatureEnabled(context.getContentResolver());
    }
}
