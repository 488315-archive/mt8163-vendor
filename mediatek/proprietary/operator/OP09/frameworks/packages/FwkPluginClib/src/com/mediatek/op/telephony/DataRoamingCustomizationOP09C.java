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
 * MediaTek Inc. (C) 2015. All rights reserved.
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

import android.content.SharedPreferences;
import android.database.ContentObserver;
import android.os.Handler;
import android.os.Message;
import android.os.SystemProperties;
import android.provider.Settings;
import android.telephony.Rlog;
import android.telephony.ServiceState;
import android.telephony.TelephonyManager;

import com.android.internal.telephony.PhoneProxy;

/**
 * Customization from CT for data when roaming.
 * Popup reminder dialog when roaming first time.
 */
public class DataRoamingCustomizationOP09C extends DataRoamingCustomization {
    protected static final String TAG = "DataRoamingCustomizationOP09C";
    private static final int EVENT_DATA_ROAMING_SWITCH_CHANGED = 3;
    private static final String[]  PROPERTY_RIL_FULL_UICC_TYPE = {
        "gsm.ril.fulluicctype",
        "gsm.ril.fulluicctype.2",
        "gsm.ril.fulluicctype.3",
        "gsm.ril.fulluicctype.4",
    };
    private static final String CARD_TYPE_RUIM = "RUIM";
    private static final String CARD_TYPE_CSIM = "CSIM";

    private String mRoamingUri = Settings.Global.DATA_ROAMING;

    private ContentObserver mRoamingSwitchObserver = new ContentObserver(new Handler()) {
        @Override
        public void onChange(boolean selfChange) {
            logd("Data roaming switch changed.");
            removeMessages(EVENT_DATA_ROAMING_SWITCH_CHANGED);
            sendEmptyMessage(EVENT_DATA_ROAMING_SWITCH_CHANGED);
        }
    };

    /**
     * Construct DataRoamingCustomizationOP09C with context and phone.
     * @param phoneProxy PhoneProxy
     */
    public DataRoamingCustomizationOP09C(PhoneProxy phoneProxy) {
        super(phoneProxy);
    }

    @Override
    public void handleMessage(Message msg) {
        int msgId = msg.what;
        logd("handleMessage: " + msgIdToString(msgId) + "(id=" + msgId + ")");
        switch (msgId) {
        case EVENT_DATA_ROAMING_SWITCH_CHANGED:
            checkFirstRoaming();
            break;
        default:
            super.handleMessage(msg);
            break;
        }
    }

    protected void registerForDataSetting(int subId) {
        super.registerForDataSetting(subId);
        // For Dual SIM phones, need to change URI.
        if (TelephonyManager.getDefault().getSimCount() > 1) {
            mRoamingUri = Settings.Global.DATA_ROAMING + subId;
        }

        logd("onSubInfoReady(), mRoamingUri=" + mRoamingUri);

        // Register for data roaming enabled with subId
        if (mRoamingUri != null && !mRoamingUri.equals(Settings.Global.DATA_ROAMING)) {
            mResolver.unregisterContentObserver(mRoamingSwitchObserver);
        }
        mResolver.registerContentObserver(
                Settings.Global.getUriFor(mRoamingUri), false, mRoamingSwitchObserver);
    }

    private boolean isCdmaCard() {
        int phoneId = mPhoneProxy.getPhoneId();
        logd("isCdmaCard phoneId=" + phoneId);
        if (phoneId >= 0 && phoneId < TelephonyManager.getDefault().getPhoneCount()) {
            String cardType = SystemProperties.get(PROPERTY_RIL_FULL_UICC_TYPE[phoneId]);
            logd("isCdmaCard cardType=" + cardType);
            if (cardType.contains(CARD_TYPE_RUIM) || cardType.contains(CARD_TYPE_CSIM)) {
                return true;
            }
        }
        return false;
    }

    protected void checkFirstRoaming() {
        if (!isCdmaCard()) {
            logd("checkFirstRoaming, is not cdma card");
            return;
        }

        boolean userDataEnabled = Settings.Global.getInt(mResolver, mUri, 1) == 1;

        boolean dataRoamingEnabled = Settings.Global.getInt(mResolver, mRoamingUri, 1) == 1;
        boolean isRoaming = mLastRilRegState == ServiceState.REGISTRATION_STATE_ROAMING;
        SharedPreferences roamingPreferences = mContext.getSharedPreferences(PREFERENCE_NAME, 0);
        boolean firstRoaming = roamingPreferences.getBoolean(mFirstRoamingKey, true);

        logd("checkFirstRoaming, userDataEnabled=" + userDataEnabled
                + ", dataRoamingEnabled=" + dataRoamingEnabled
                + ",isRoaming=" + isRoaming + ",firstRoaming=" + firstRoaming);
        if (userDataEnabled && isRoaming && firstRoaming && dataRoamingEnabled) {
            popupDialog();
            setFirstRoamingFlag(false);
        }
    }

    protected String msgIdToString(int id) {
        switch (id) {
        case EVENT_DATA_ROAMING_SWITCH_CHANGED:
            return "EVENT_DATA_ROAMING_SWITCH_CHANGED";
        default:
            return super.msgIdToString(id);
        }
    }

    protected void logd(String s) {
        Rlog.d(TAG + "[" + mPhoneProxy.getPhoneId() + "]", s);
    }
}
