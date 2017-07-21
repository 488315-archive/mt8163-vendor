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

package com.mediatek.op.telephony;

import android.app.ActivityManagerNative;
import android.content.Intent;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;
import android.os.SystemProperties;
import android.os.UserHandle;

import android.telephony.Rlog;
import android.telephony.TelephonyManager;

import com.android.internal.telephony.CommandsInterface;
import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneBase;
import com.android.internal.telephony.PhoneFactory;
import com.android.internal.telephony.PhoneProxy;
import com.mediatek.common.PluginImpl;
import com.mediatek.common.telephony.IDeviceInformationExt;

@PluginImpl(interfaceName = "com.mediatek.common.telephony.IDeviceInformationExt")
public class DefaultDeviceInformationExt implements IDeviceInformationExt {
    private static final String TAG = "DefaultDeviceInformationExt";
    private static final int PROJECT_SIM_NUM = TelephonyManager.getDefault().getSimCount();
    private static final int EVENT_RADIO_AVAILABLE = 1;
    private static final int EVENT_GET_IMEI_DONE = 9;
    private static final int EVENT_GET_DEVICE_IDENTITY_DONE = 21;
    private static final String INTENT_ACTION_IMEI_MEID = "intent_action_imei_meid";
    private static final String EXTRA_KEY_IMEI = "extra_key_imei";
    private static final String EXTRA_KEY_MEID = "extra_key_meid";
    private static final String SUPPORT_YES = "1";
    private static final String SUPPORT_NO = "0";
    // SVLTE support system property
    private static final String MTK_SVLTE_SUPPORT = "ro.mtk_svlte_support";
    // SRLTE support system property
    private static final String MTK_SRLTE_SUPPORT = "ro.mtk_srlte_support";

    private static Phone[] sPhones = null;
    private static CommandsInterface[] sCi = new CommandsInterface[PROJECT_SIM_NUM];
    private String[] mImei = new String[PROJECT_SIM_NUM];
    private String mMeid = "";
    private DeviceInformationHandle[] mDeviceInformationHandle;

    public DefaultDeviceInformationExt() {
        Rlog.d(TAG, "DefaultDeviceInformationExt Constructor");
        init();
    }

    private void init() {
        if (enableDeviceInformation()) {
            sPhones = PhoneFactory.getPhones();
            mDeviceInformationHandle = new DeviceInformationHandle[PROJECT_SIM_NUM];
            Rlog.d(TAG, "DefaultDeviceInformationExt init() length = " + sPhones.length);
            for (int i = 0; i < sPhones.length; i++) {
                sCi[i] = ((PhoneBase) ((PhoneProxy) sPhones[i]).getActivePhone()).mCi;
                mDeviceInformationHandle[i] = new DeviceInformationHandle(i);
                sCi[i].registerForAvailable(mDeviceInformationHandle[i],
                        EVENT_RADIO_AVAILABLE, null);
            }
        } else {
            Rlog.d(TAG, "DefaultDeviceInformationExt don't init()");
        }
    }

    @Override
    public boolean enableDeviceInformation() {
        Rlog.d(TAG, "DefaultDeviceInformationExt enableDeviceInformation "
            + (isSrlteSupport() || isSvlteSupport()));
        return isSrlteSupport() || isSvlteSupport();
    }

    class DeviceInformationHandle extends Handler {
        CommandsInterface mCi;
        int mSlotId = -1;
        public DeviceInformationHandle(int i) {
            mCi = sCi[i];
            mSlotId = i;
        }
        @Override
        public void handleMessage(Message msg) {
            Rlog.d(TAG, "DefaultDeviceInformationExt handleMessage msg = " + msg.what);
            AsyncResult ar;
            Message onComplete;
            switch (msg.what) {
            case EVENT_RADIO_AVAILABLE:
                Rlog.d(TAG, "DefaultDeviceInformationExt EVENT_RADIO_AVAILABLE");
                mCi.getIMEI(obtainMessage(EVENT_GET_IMEI_DONE));
                mCi.getDeviceIdentity(obtainMessage(EVENT_GET_DEVICE_IDENTITY_DONE));
                break;
            case EVENT_GET_IMEI_DONE:
                Rlog.d(TAG, "DefaultDeviceInformationExt handleMessage msg = EVENT_GET_IMEI_DONE");
                ar = (AsyncResult) msg.obj;
                if (ar.exception != null) {
                    Rlog.d(TAG, "Null IMEI!!");
                    break;
                }
                mImei[mSlotId] = (String) ar.result;
                Rlog.d(TAG, "IMEI: " + mImei[mSlotId]);
                sendBroadcast();
                break;
            case EVENT_GET_DEVICE_IDENTITY_DONE:
                Rlog.d(TAG, "DefaultDeviceInformationExt " +
                    "handleMessage msg = EVENT_GET_DEVICE_IDENTITY_DONE");
                ar = (AsyncResult) msg.obj;
                if (ar.exception != null) {
                    Rlog.d(TAG, "Null mMeid!! ar.exception = " + ar.exception.toString());
                    break;
                }
                String[] respId = (String[]) ar.result;
                mMeid =  respId[3];
                if (mMeid != null) {
                    mMeid = mMeid.toUpperCase();
                }
                Rlog.d(TAG, "mMeid: " + mMeid);
                sendBroadcast();
                break;
            default:
                break;
            }
        }

        private void sendBroadcast() {
            boolean needSend = true;
            for (int i = 0; i < mImei.length; i++) {
                if (mImei[i] == null || mImei[i].length() <= 0) {
                    needSend = false;
                }
            }
            if (mMeid.length() <= 0) {
                needSend = false;
            }
            Rlog.d(TAG, "needSend: " + needSend);
            if (needSend) {
                Intent intent = new Intent(INTENT_ACTION_IMEI_MEID);
                intent.putExtra(EXTRA_KEY_MEID, mMeid);
                intent.putExtra(EXTRA_KEY_IMEI, mImei);
                ActivityManagerNative.broadcastStickyIntent(intent, null, UserHandle.USER_ALL);
            }
        }
    }

    private boolean isSrlteSupport() {
        return SUPPORT_YES.equals(SystemProperties.get(MTK_SRLTE_SUPPORT, SUPPORT_NO));
    }

    private boolean isSvlteSupport() {
        return SUPPORT_YES.equals(SystemProperties.get(MTK_SVLTE_SUPPORT, SUPPORT_NO));
    }
}
