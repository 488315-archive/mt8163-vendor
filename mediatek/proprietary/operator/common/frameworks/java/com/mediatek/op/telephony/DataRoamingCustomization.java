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

import android.app.AlertDialog;
import android.content.BroadcastReceiver;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.database.ContentObserver;
import android.os.AsyncResult;
import android.os.Handler;
import android.os.Message;
import android.os.SystemProperties;
import android.provider.Settings;
import android.telephony.Rlog;
import android.telephony.ServiceState;
import android.telephony.SubscriptionManager;
import android.telephony.TelephonyManager;
import android.text.TextUtils;
import android.view.WindowManager;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneProxy;
import com.android.internal.telephony.TelephonyIntents;
import com.android.internal.telephony.uicc.UiccController;

import com.mediatek.internal.R;

/**
 * Customization from CT for data when roaming.
 * Popup reminder dialog when roaming first time.
 */
public abstract class DataRoamingCustomization extends Handler {
    protected static final String TAG = "DataRoamingCustomization";

    private static final int EVENT_DATA_SETTING_CHANGED = 1;
    private static final int EVENT_SERVICE_STATE_CHANGED = 2;

    private static final String CHINA_MCC = "460";
    private static final int MCC_LENGTH = 3;

    protected static final String PREFERENCE_NAME = "roaming_customization";
    private static final String FIRST_ROAMING_KEY = "first_roaming";
    private static final String LAST_REG_STATE_KEY = "last_reg_state";
    private static final String LAST_OPERATOR_NUMERIC_KEY = "last_operator_numeric";

    protected Context mContext;
    protected ContentResolver mResolver;
    private Phone mPhone;
    protected PhoneProxy mPhoneProxy;

    protected String mUri = Settings.Global.MOBILE_DATA;
    protected String mFirstRoamingKey = FIRST_ROAMING_KEY;
    private String mLastRegStateKey = LAST_REG_STATE_KEY;
    private String mLastOperatorNumericKey = LAST_OPERATOR_NUMERIC_KEY;

    protected int mLastRilRegState =
            ServiceState.REGISTRATION_STATE_NOT_REGISTERED_AND_NOT_SEARCHING;
    private String mLastOpNumeric = "00000";

    private ContentObserver mObserver = new ContentObserver(new Handler()) {
        @Override
        public void onChange(boolean selfChange) {
            logd("Observer Onchange");
            removeMessages(EVENT_DATA_SETTING_CHANGED);
            sendEmptyMessage(EVENT_DATA_SETTING_CHANGED);
        }
    };

    /**
     * Construct DataRoamingCustomization with context and phone.
     * @param phoneProxy PhoneProxy
     */
    public DataRoamingCustomization(PhoneProxy phoneProxy) {
        mContext = phoneProxy.getContext();
        mPhone = phoneProxy.getActivePhone();
        mPhoneProxy = phoneProxy;
        mResolver = mContext.getContentResolver();

        mPhone.registerForServiceStateChanged(
                this, EVENT_SERVICE_STATE_CHANGED, null);

        IntentFilter filter = new IntentFilter();
        filter.addAction(TelephonyIntents.ACTION_SUBINFO_RECORD_UPDATED);
        filter.addAction(TelephonyIntents.ACTION_RADIO_TECHNOLOGY_CHANGED);
        mContext.registerReceiver(mIntentReceiver, filter);
        logd("DataRoamingCustomization constructor for OP09");
    }

    @Override
    public void handleMessage(Message msg) {
        int msgId = msg.what;
        logd("handleMessage: " + msgIdToString(msgId) + "(id=" + msgId + ")");
        switch (msgId) {
        case EVENT_DATA_SETTING_CHANGED:
            checkFirstRoaming();
            break;
        case EVENT_SERVICE_STATE_CHANGED:
            AsyncResult ar = (AsyncResult) msg.obj;
            ServiceState serviceState = (ServiceState) ar.result;
            logd("serviceState = " + serviceState.toString());
            final int dataRegState = serviceState.getDataRegState();
            logd("dataRegState = " + dataRegState);
            if (dataRegState == ServiceState.STATE_IN_SERVICE) {
                final int rilDataRegState = serviceState.getRilDataRegState();
                final String operatorNumeric = serviceState.getOperatorNumeric();
                logd("rilDataRegState = " + rilDataRegState + ",operatorNumeric = " +
                        operatorNumeric + ",mLastRilRegState = " + mLastRilRegState +
                        ",mLastOpNumeric = " + mLastOpNumeric);
                if (isMccInvalid(operatorNumeric)) {
                    return;
                }
                if (rilDataRegState != mLastRilRegState ||
                        (mLastOpNumeric != null && operatorNumeric != null &&
                        !mLastOpNumeric.equals(operatorNumeric))) {
                    saveLastRegInfo(rilDataRegState, operatorNumeric);
                    if (rilDataRegState == ServiceState.REGISTRATION_STATE_ROAMING) {
                        checkFirstRoaming();
                    } else if (rilDataRegState == ServiceState.REGISTRATION_STATE_HOME_NETWORK) {
                        setFirstRoamingFlag(true);
                    }
                }
            }
            break;
        default:
            break;
        }
    }

    private boolean isMccInvalid(String opNumeric) {
        if (TextUtils.isEmpty(opNumeric)) {
            logd("isMccInvalid, opNumeric is empty");
            return false;
        }
        String mcc = opNumeric.substring(0, MCC_LENGTH);
        logd("isMccInvalid, mcc=" + mcc);
        return TextUtils.isEmpty(mcc) || mcc.equals("000") || mcc.equals("N/A");
    }

    private BroadcastReceiver mIntentReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            logd("onReceive: action=" + action);
            if (action.equals(TelephonyIntents.ACTION_SUBINFO_RECORD_UPDATED)) {
                onSubInfoReady();
            } else if (action.equals(TelephonyIntents.ACTION_RADIO_TECHNOLOGY_CHANGED)) {
                onRadioTechnologyChanged();
            }
        }
    };

    protected void onSubInfoReady() {
        int subId = SubscriptionManager.getSubIdUsingPhoneId(mPhoneProxy.getPhoneId());
        logd("onSubInfoReady: subId:" + subId);

        if (SubscriptionManager.isValidSubscriptionId(subId)) {
            getRoamingInfo(subId);
            registerForDataSetting(subId);
            // Trigger a self change to check whether need to popup prompt
            // dialog, in case the sub info ready is later than network
            // registered.
            mObserver.onChange(true);
        }
    }

    protected void registerForDataSetting(int subId) {
        // For Dual SIM phones, need to change URI.
        if (TelephonyManager.getDefault().getSimCount() > 1) {
            mUri = Settings.Global.MOBILE_DATA + subId;
        }

        logd("onSubInfoReady(), mUri=" + mUri);

        // Register for mobile data enabled.
        if (mUri != null && !mUri.equals(Settings.Global.MOBILE_DATA)) {
            mResolver.unregisterContentObserver(mObserver);
        }
        mResolver.registerContentObserver(Settings.Global.getUriFor(mUri), false, mObserver);
    }

    private void getRoamingInfo(int subId) {
        // Get roaming info from preference
        mFirstRoamingKey = FIRST_ROAMING_KEY + subId;
        mLastRegStateKey = LAST_REG_STATE_KEY + subId;
        mLastOperatorNumericKey = LAST_OPERATOR_NUMERIC_KEY + subId;
        SharedPreferences roamingPreferences = mContext.getSharedPreferences(
                PREFERENCE_NAME, 0);
        mLastRilRegState = roamingPreferences.getInt(mLastRegStateKey,
                ServiceState.REGISTRATION_STATE_NOT_REGISTERED_AND_NOT_SEARCHING);
        mLastOpNumeric = roamingPreferences.getString(mLastOperatorNumericKey, "00000");
    }

    private void onRadioTechnologyChanged() {
        logd("onRadioTechnologyChanged");
        mPhone.unregisterForServiceStateChanged(this);
        mPhone = mPhoneProxy.getActivePhone();
        mPhone.registerForServiceStateChanged(this, EVENT_SERVICE_STATE_CHANGED, null);
    }

    protected abstract void checkFirstRoaming();

    protected void setFirstRoamingFlag(boolean first) {
        logd("setFirstRoamingFlag, first=" + first);
        SharedPreferences roamingPreferences = mContext.getSharedPreferences(PREFERENCE_NAME, 0);
        Editor roamingEditor = roamingPreferences.edit();
        roamingEditor.putBoolean(mFirstRoamingKey, first);
        roamingEditor.commit();
    }

    private void saveLastRegInfo(int regState, String operatorNumeric) {
        logd("saveLastRegInfo, regState=" + regState + ",operatorNumeric=" + operatorNumeric);
        mLastRilRegState = regState;
        mLastOpNumeric = operatorNumeric;
        SharedPreferences roamingPreferences = mContext.getSharedPreferences(PREFERENCE_NAME, 0);
        Editor roamingEditor = roamingPreferences.edit();
        roamingEditor.putInt(mLastRegStateKey, regState);
        roamingEditor.putString(mLastOperatorNumericKey, operatorNumeric);
        roamingEditor.commit();
    }

    /**
     * Unregister from all events it registered for.
     */
    public void dispose() {
        mResolver.unregisterContentObserver(mObserver);
        mPhone.unregisterForServiceStateChanged(this);
        mContext.unregisterReceiver(mIntentReceiver);
    }

    protected void popupDialog() {
        logd("popupDialog for data enabled on roaming network.");
        AlertDialog.Builder builder = new AlertDialog.Builder(mContext);
        builder.setMessage(R.string.roaming_message);
        builder.setPositiveButton(R.string.known, null);
        AlertDialog dialog = builder.create();
        dialog.getWindow().setType(
                WindowManager.LayoutParams.TYPE_SYSTEM_DIALOG);
        dialog.show();
    }

    protected String msgIdToString(int id) {
        switch (id) {
        case EVENT_DATA_SETTING_CHANGED:
            return "EVENT_DATA_SETTING_CHANGED";
        case EVENT_SERVICE_STATE_CHANGED:
            return "EVENT_SERVICE_STATE_CHANGED";
        default:
            return "unknown event";
        }
    }

    protected void logd(String s) {
        Rlog.d(TAG + "[" + mPhoneProxy.getPhoneId() + "]", s);
    }
}
