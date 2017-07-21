/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
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

package com.mediatek.phone.plugin;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.SystemProperties;
import android.preference.ListPreference;
import android.preference.Preference;
import android.preference.PreferenceActivity;
import android.preference.PreferenceScreen;
import android.preference.SwitchPreference;
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;
import android.util.Log;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneFactory;

import com.mediatek.common.PluginImpl;
import com.mediatek.op07.plugin.R;
import com.mediatek.phone.ext.DefaultMobileNetworkSettingsExt;

@PluginImpl(interfaceName = "com.mediatek.phone.ext.IMobileNetworkSettingsExt")
public class OP07MobileNetworkSettingsExt extends DefaultMobileNetworkSettingsExt {
    private AlertDialog.Builder mDialogBuild;
    private static final String TAG = "OP07MobileNetworkSettingsExt";
    private static final String BUTTON_CARRIER_SEL = "button_carrier_sel_key";
    private static final String BUTTON_NETWORK_MODE_KEY = "gsm_umts_preferred_network_mode_key";
    private static final String BUTTON_PLMN_LIST = "button_plmn_key";
    private static final String BUTTON_NETWORK_MODE_EX_KEY = "button_network_mode_ex_key";
    private static final String BUTTON_PREFERED_NETWORK_MODE = "preferred_network_mode_key";
    private static final String BUTTON_ENABLED_NETWORKS_KEY = "enabled_networks_key";
    private static final String BUTTON_NETWORK_MODE_LTE_KEY = "button_network_mode_LTE_key";
    private static final String BUTTON_ROAMING_KEY = "button_roaming_key";
    private static final int ROAMING_DIALOG = 300;
    private static final String BUTTON_DISABLE_2G = "button_diable_2g";
    public static final String PROP_SETTINGS_DISABLE_2G = "persist.radio.disable.2g";
    private static Context mContext;
    private SwitchPreference mDisable2GPreference;
    private AlertDialog mDialog;

    public OP07MobileNetworkSettingsExt(Context context) {
        mContext = context;
    }

    private PhoneStateListener mPhoneStateListener = new PhoneStateListener() {
        @Override
        public void onCallStateChanged(int state, String incomingNumber) {
            Log.d(TAG, "Phone state:" + state);
            switch (state) {
            case TelephonyManager.CALL_STATE_IDLE:
                Log.d(TAG, "No call switch enable");
                mDisable2GPreference.setEnabled(true);
                mDisable2GPreference.setChecked(isSettingOn());
                break;
            case TelephonyManager.CALL_STATE_OFFHOOK:
            case TelephonyManager.CALL_STATE_RINGING:
                Log.d(TAG, "call Exist switch disable");
                mDisable2GPreference.setEnabled(false);
                mDisable2GPreference.setChecked(isSettingOn());
                if (mDialog != null) {
                    mDialog.dismiss();
                }
                break;
            default:
                mDisable2GPreference.setEnabled(false);
                mDisable2GPreference.setChecked(isSettingOn());
                Log.d(TAG, "default");
                break;
            }
        }
    };

    @Override
    public void initOtherMobileNetworkSettings(PreferenceActivity activity, int subID) {
        Log.d("@M_" + TAG, "Initialize preference activity" + activity);
        PreferenceScreen prefSet = activity.getPreferenceScreen();
        final SwitchPreference mButtonDataRoam = (SwitchPreference) prefSet
                .findPreference(BUTTON_ROAMING_KEY);
        String summary = mContext.getString(R.string.data_roaming_warning);
        if (mButtonDataRoam != null) {
            mButtonDataRoam.setSummaryOn(summary);
            mButtonDataRoam.setSummaryOff(summary);

        }

        Log.d("@M_" + TAG, "Op07 setRoamingSummary with summary=" + summary);

        customizeFeatureForOperator(prefSet);
        addDisable2GPreference(activity);
    }

    @Override
    public boolean onPreferenceTreeClick(PreferenceScreen prefSet, Preference preference) {
        Preference buttonDisable2G = (SwitchPreference) prefSet.findPreference(BUTTON_DISABLE_2G);
        if (preference == buttonDisable2G) {
            mDisable2GPreference = (SwitchPreference) buttonDisable2G;
            boolean checked = ((SwitchPreference) preference).isChecked();
            if (checked) {
                AlertDialog.Builder builder = new AlertDialog.Builder(preference.getContext());
                Log.d(TAG, "Amit alert");
                builder.setTitle(mContext.getString(R.string.disable_2g_Alert_title));
                builder.setMessage(mContext.getString(R.string.disable_2g_menu_on_alert));
                builder.setOnCancelListener(new DialogInterface.OnCancelListener() {
                    @Override
                    public void onCancel(DialogInterface dialog) {
                        Log.d(TAG, "onCancel");
                        mDisable2GPreference.setChecked(isSettingOn());
                    }
                });
                builder.setPositiveButton(mContext.getString(R.string.disable_2g_alert_ok),
                        new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                handleSwitchAction(true, "1");
                            }
                        });
                builder.setNegativeButton(mContext.getString(R.string.disable_2g_alert_cancel),
                        new DialogInterface.OnClickListener() {
                            @Override
                            public void onClick(DialogInterface dialog, int which) {
                                mDisable2GPreference.setChecked(false);
                            }
                        });
                mDialog = builder.show();
            } else {
                handleSwitchAction(false, "0");
            }
            return true;
        } else {
            return false;
        }
    }

    private void addDisable2GPreference(PreferenceActivity activity) {
        // for disable 2G menu
        // check proj prop supporting 4G or not
        String lteSupport = SystemProperties.get("ro.mtk_lte_support", "0");
        boolean isLteEnabled = lteSupport.equals("1");
        Log.d(TAG, "isLteEnabled:" + isLteEnabled);
        if (isLteEnabled) {
            PreferenceScreen prefSet = activity.getPreferenceScreen();
            if (prefSet.findPreference(BUTTON_DISABLE_2G) == null) {
                SwitchPreference buttonDisable2G = new SwitchPreference(activity);
                mDisable2GPreference = buttonDisable2G;
                buttonDisable2G.setKey(BUTTON_DISABLE_2G);
                buttonDisable2G.setTitle(mContext.getString(R.string.network_button_disable_2g));
                buttonDisable2G.setSummary(mContext
                        .getString(R.string.network_button_disable_2g_summary));
                buttonDisable2G.setEnabled(true);
                TelephonyManager telephonyManager = (TelephonyManager) mContext
                        .getSystemService(Context.TELEPHONY_SERVICE);
                if (telephonyManager.getCallState() != TelephonyManager.CALL_STATE_IDLE) {
                    Log.d(TAG, "Call Exist so disable the menu");
                    buttonDisable2G.setEnabled(false);
                }
                prefSet.addPreference(buttonDisable2G);
                buttonDisable2G.setChecked(isSettingOn());
                ((TelephonyManager) mContext.getSystemService(Context.TELEPHONY_SERVICE)).listen(
                        mPhoneStateListener, PhoneStateListener.LISTEN_CALL_STATE);
            }
        }
    }

    @Override
    public void unRegister() {
        Log.d(TAG, "unRegister");
        ((TelephonyManager) mContext.getSystemService(Context.TELEPHONY_SERVICE)).listen(
                mPhoneStateListener, PhoneStateListener.LISTEN_NONE);
    }

    private void sendAtToModem(String propValue) {
        Log.d(TAG, "sendAtToModem value:" + propValue);
        String s[] = new String[2];
        s[0] = "AT+EDRAT=" + propValue;
        s[1] = "";
        Phone phone = PhoneFactory.getDefaultPhone();
        phone.invokeOemRilRequestStrings(s, null);

    }

    private void handleSwitchAction(boolean switchValue, String propValue) {
        Log.d(TAG, "handleSwitchAction value:" + switchValue + "SysProp:" + propValue);
        SystemProperties.set(PROP_SETTINGS_DISABLE_2G, propValue);
        mDisable2GPreference.setChecked(switchValue);
        sendAtToModem(propValue);
    }

    private boolean isSettingOn() {
        String propValue = SystemProperties.get(PROP_SETTINGS_DISABLE_2G, "0");
        Log.d(TAG, "isSettingOn propValue:" + propValue);
        boolean result = propValue.equals("1") ? true : false;
        Log.d(TAG, "isSettingOn result:" + result);
        return result;
    }

    @Override
    public void customizeAlertDialog(Preference preference, AlertDialog.Builder builder) {

        Log.d("@M_" + TAG, "Op07 Customize alert dialog ");
        if (preference != null) {
            builder.setMessage(mContext.getString(R.string.data_roaming_warning));
        }

    }

    //@Override
    public void customizeFeatureForOperator(PreferenceScreen prefSet) {
        Log.d("@M_" + TAG, "Op07 Customize feature for operator" + prefSet);
        Preference buttonPreferredNetworkModeEx = prefSet
                .findPreference(BUTTON_NETWORK_MODE_EX_KEY);
        Preference buttonPreferredNetworkMode = (ListPreference) prefSet
                .findPreference(BUTTON_PREFERED_NETWORK_MODE);
        Preference buttonGsmUmtsPreferredNetworkMode = (ListPreference) prefSet
                .findPreference(BUTTON_NETWORK_MODE_KEY);
        Preference buttonLtePreferredNetworkMode = (ListPreference) prefSet
                .findPreference(BUTTON_NETWORK_MODE_LTE_KEY);
        Preference buttonEnabledNetworks = (ListPreference) prefSet
                .findPreference(BUTTON_ENABLED_NETWORKS_KEY);
        if (buttonPreferredNetworkModeEx != null) {
            prefSet.removePreference(buttonPreferredNetworkModeEx);
        }
        if (buttonPreferredNetworkMode != null) {
            prefSet.removePreference(buttonPreferredNetworkMode);
        }
        if (buttonGsmUmtsPreferredNetworkMode != null) {
            prefSet.removePreference(buttonGsmUmtsPreferredNetworkMode);
        }
        if (buttonLtePreferredNetworkMode != null) {
            prefSet.removePreference(buttonLtePreferredNetworkMode);
        }
        if (buttonEnabledNetworks != null) {
            prefSet.removePreference(buttonEnabledNetworks);
        }
    }

}
