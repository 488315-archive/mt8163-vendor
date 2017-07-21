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
 * MediaTek Inc. (C) 2014. All rights reserved.
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
package com.android.camera.v2.uimanager.preference;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.provider.MediaStore;
import android.util.Size;

import com.android.camera.R;
import com.android.camera.v2.app.SettingAgent;
import com.android.camera.v2.util.CameraUtil;
import com.android.camera.v2.util.SettingKeys;
import com.mediatek.camera.debug.LogHelper;
import com.mediatek.camera.debug.LogHelper.Tag;

import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class PreferenceManager {
    private static final Tag TAG = new Tag(PreferenceManager.class.getSimpleName());
    private Context                            mContext = null;
    private SettingAgent                       mSettingAgent = null;
    private Map<Integer, PreferenceGroup> mPreferenceGroupMap =
            new HashMap<Integer, PreferenceGroup>();
    private int                                mCameraId = 0;
    private Activity                           mActivity;
    private String[]                           mSettingKeys = null;

    public PreferenceManager(Activity activity, SettingAgent settingAgent) {
        LogHelper.i(TAG, "[PreferenceManager], instructor");
        mActivity = activity;
        mContext = activity.getApplicationContext();
        mSettingAgent = settingAgent;

        Intent intent = mActivity.getIntent();
        String action = null;
        if (intent != null) {
            action = intent.getAction();
        }
        LogHelper.i(TAG, "[PreferenceManager], action:" + action);

        // copy SettingKeys.KEYS_FOR_SETTING to mSettingKeys.
        mSettingKeys = new String[SettingKeys.KEYS_FOR_SETTING.length];
        for (int i = 0; i < SettingKeys.KEYS_FOR_SETTING.length; i++) {
            mSettingKeys[i] = SettingKeys.KEYS_FOR_SETTING[i];
        }

        if (MediaStore.ACTION_IMAGE_CAPTURE.equals(action)
                || MediaStore.ACTION_VIDEO_CAPTURE.equals(action)
                || CameraUtil.ACTION_STEREO3D.equals(action)) {
            for (int i = 0; i < SettingKeys.KEYS_UN_SUPPORTED_BY_3TH.length; i++) {
                int settingId = SettingKeys.KEYS_UN_SUPPORTED_BY_3TH[i];
                mSettingKeys[settingId] = null;
            }
        }
    }

    public void initializePreferences(int preferencesRes, int cameraId) {
        LogHelper.i(TAG, "[initializePreferences], start, cameraId:" + cameraId);
        mCameraId = cameraId;
        PreferenceGroup group = mPreferenceGroupMap.get(cameraId);
        if (group == null) {
            //constructor listPreference from xml.
            PreferenceInflater inflater = new PreferenceInflater(mContext);
            group = (PreferenceGroup) inflater.inflate(preferencesRes);
            mPreferenceGroupMap.put(cameraId, group);
            for (int i = 0; i < mSettingKeys.length; i++) {
                String key = mSettingKeys[i];
                if (key == null) {
                    continue;
                }
                ListPreference preference = group.findPreference(key);
                if (preference == null) {
                    continue;
                }

                int type = SettingKeys.getSettingType(key);
                // only decided by by app layer.
                if (type == SettingKeys.DECIDE_BY_APP) {
                    continue;
                }
                // get the supported values of setting which is decide by native from native.
                List<String> supportedValues =
                        mSettingAgent.getSupportedValues(key, String.valueOf(mCameraId));
                String supportedValue = null;
                if (supportedValues != null) {
                    for (int k = 0; k < supportedValues.size(); k++) {
                        String value = supportedValues.get(k);
                        supportedValue = supportedValue + value + ",";
                    }
                }
                LogHelper.d(TAG, "key:" + key + ", supportedValue:" + supportedValue);
                //filter listPreference.
                if (SettingKeys.KEY_PICTURE_SIZE.equals(key)) {
                    buildPictureSizeEntries(preference, supportedValues);
                } else {
                    filterUnSupportedValues(preference, supportedValues);
                }
            }
            filterGroupListPrference(group, SettingKeys.KEY_IMAGE_PROPERTIES);
            filterGroupListPrference(group, SettingKeys.KEY_FACE_BEAUTY_PROPERTIES);

            for (int i = 0; i < mSettingKeys.length; i++) {
                String key = mSettingKeys[i];
                if (key == null) {
                    continue;
                }
                ListPreference preference = group.findPreference(key);
                if (preference != null) {
                    LogHelper.i(TAG, "key:" + key
                            + ", defaultValue:" + preference.getDefaultValue());
                }
            }
        }

        // camera id may be changed, so do camera id setting change every time.
        mSettingAgent.doSettingChange(SettingKeys.KEY_CAMERA_ID, String.valueOf(cameraId));

        // setting may have last setting changed value, when change to a new camera, firstly
        // make all the settings' value to default value.
        Map<String, String> defaultSettings = new LinkedHashMap<String, String>();
        for (int i = 0; i < mSettingKeys.length; i++) {
            String key = mSettingKeys[i];
            if (key == null) {
                continue;
            }
            ListPreference preference = group.findPreference(key);
            if (preference == null) {
                continue;
            }
            preference.setOverrideValue(null);
            String defaultValue = preference.getDefaultValue();
            if (defaultValue == null) {
                List<String> supportedValues = mSettingAgent
                        .getSupportedValues(key, String.valueOf(mCameraId));
                if (supportedValues != null) {
                    defaultValue = supportedValues.get(0);
                }
                preference.setDefaultValue(defaultValue);
            }
            defaultSettings.put(key, defaultValue);

            // synch listPreference value to the value in sharedpreferences.
            String sharePreferencesValue =
                    mSettingAgent.getSharedPreferencesValue(key, String.valueOf(mCameraId));
            if (sharePreferencesValue != null) {
                preference.setValue(sharePreferencesValue);
            }
        }
       // do not need to configurate camera id.
        defaultSettings.remove(SettingKeys.KEY_CAMERA_ID);
        mSettingAgent.configurateSetting(defaultSettings);

        LogHelper.i(TAG, "[initializePreferences], end");
    }

    public ListPreference getListPreference(String key) {
        if (key == null) {
            return null;
        }
        // if key do not in mSettingKeys, return null;
        boolean isContains = false;
        for (int i = 0; i < mSettingKeys.length; i++) {
            if (mSettingKeys[i] != null && mSettingKeys[i].equals(key)) {
                isContains = true;
                break;
            }
        }
        if (!isContains) {
            return null;
        }

        PreferenceGroup group = mPreferenceGroupMap.get(mCameraId);
        return group.findPreference(key);
    }

    public PreferenceGroup getPreferenceGroup() {
        PreferenceGroup group = mPreferenceGroupMap.get(mCameraId);
        return group;
    }

    public void updateSettingResult(Map<String, String> values,
            Map<String, String> overrideValues) {
        LogHelper.i(TAG, "[updateSettingResult]");
        if (values != null) {
            Set<String> set = values.keySet();
            Iterator<String> iterator = set.iterator();
            PreferenceGroup group = mPreferenceGroupMap.get(mCameraId);
            while (iterator.hasNext()) {
                String key = iterator.next();
                String value = values.get(key);
                String overrideValue = CameraUtil.buildEnabledList(overrideValues.get(key), value);
                ListPreference preference = group.findPreference(key);
                if (preference != null  && preference.isVisibled()) {
                    preference.setValue(value);
                    preference.setOverrideValue(overrideValue);
                }
            }
        }
    }

    public void restoreSetting() {
        Map<String, String> defaultSettings = new HashMap<String, String>();
        PreferenceGroup group = mPreferenceGroupMap.get(mCameraId);
        for (int i = 0; i < mSettingKeys.length; i++) {
            String key = mSettingKeys[i];
            if (key == null) {
                continue;
            }

            ListPreference pref = group.findPreference(key);
            if (pref != null) {
                String defaultValue = pref.getDefaultValue();
                pref.setValue(defaultValue);
                defaultSettings.put(key, defaultValue);
            }
        }
        // do not need to reset camera id.
        defaultSettings.remove(SettingKeys.KEY_CAMERA_ID);
        mSettingAgent.doSettingChange(defaultSettings);
        clearSharedPreferencesValue();
    }

    public void clearSharedPreferencesValue() {
        Set<Integer> set = mPreferenceGroupMap.keySet();
        Iterator<Integer> iterator = set.iterator();
        // play the keys which do not want to clear as null.
        String[] clearKeys = new String[mSettingKeys.length];
        for (int i = 0; i < mSettingKeys.length; i++) {
            clearKeys[i] = mSettingKeys[i];
        }
        for (int i = 0; i < SettingKeys.KEYS_UN_CLEAR.length; i++) {
            int settingId = SettingKeys.KEYS_UN_CLEAR[i];
            clearKeys[settingId] = null;
        }

        while (iterator.hasNext()) {
            String cameraId = iterator.next().toString();
            mSettingAgent.clearSharedPreferencesValue(clearKeys, cameraId);
        }
    }

    private void buildPictureSizeEntries(ListPreference pref, List<String> supportedSizes) {
        if (supportedSizes == null || supportedSizes.size() == 0) {
            return;
        }
        sortSizesInAscending(supportedSizes);
        DecimalFormat sMegaPixelFormat = new DecimalFormat("##0");
        List<String> entryValuesList = new ArrayList<String>();
        List<String> entriesList = new ArrayList<String>();

        for (int i = 0; i < supportedSizes.size(); i++) {
            Size size = CameraUtil.getSize(supportedSizes.get(i));
            if (size == null) {
                continue;
            }
            String megaPixels = sMegaPixelFormat.format(
                    (size.getWidth() * size.getHeight()) / 1e6);
            String entry = mContext.getResources()
                    .getString(R.string.setting_summary_megapixels, megaPixels);
            if (size.getWidth() * size.getHeight() == CameraUtil.VGA_SIZE) {
                entry = "VGA";
            } else if (size.getWidth() * size.getHeight() == CameraUtil.QVGA_SIZE) {
                entry = "QVGA";
            }
            // If the entries list already has this entry and its entry value has the
            // same ratio, update the entry values in entry values list.
            boolean isEntryHaved = false;
            int index = entriesList.indexOf(entry);
            if (index > 0) {
                Size entryValue = CameraUtil.getSize(entryValuesList.get(index));
                if (CameraUtil.toleranceRatio(size, entryValue)) {
                    isEntryHaved = true;
                }
            }
            if (isEntryHaved) {
                entryValuesList.set(index, supportedSizes.get(i));
            } else {
                entryValuesList.add(supportedSizes.get(i));
                entriesList.add(entry);
            }
        }
        CharSequence[] entryValues = new CharSequence[entryValuesList.size()];
        CharSequence[] entries = new CharSequence[entryValuesList.size()];
        pref.setOriginalEntryValues(entryValuesList.toArray(entryValues));
        pref.setOriginalEntries(entriesList.toArray(entries));
        pref.filterUnsupported(supportedSizes);
    }

    private void sortSizesInAscending(List<String> supportedPictureSizes) {
        String maxSizeInString = null;
        Size maxSize = null;
        int maxIndex = 0;
        // Loop (n - 1) times.
        for (int i = 0; i < supportedPictureSizes.size() - 1; i++) {
            maxSizeInString = supportedPictureSizes.get(0);
            maxSize = CameraUtil.getSize(maxSizeInString);
            if (maxSize == null) {
                continue;
            }
            maxIndex = 0;
            String tempSizeInString = null;
            Size tempSize = null;
            // Find the max size.
            for (int j = 0; j < supportedPictureSizes.size() - i; j++) {
                tempSizeInString = supportedPictureSizes.get(j);
                tempSize = CameraUtil.getSize(tempSizeInString);
                if (tempSize == null) {
                    continue;
                }
                if (tempSize.getWidth() * tempSize.getHeight()
                        > maxSize.getWidth() * maxSize.getHeight()) {
                    maxSizeInString = tempSizeInString;
                    maxSize = tempSize;
                    maxIndex = j;
                }
            }
            // Place the max size to the end position.
            supportedPictureSizes.set(maxIndex, tempSizeInString);
            supportedPictureSizes.set(supportedPictureSizes.size() - 1 - i, maxSizeInString);
        }
    }

    private void filterUnSupportedValues(ListPreference pref, List<String> supportedValues) {
        if (supportedValues != null) {
            pref.filterUnsupported(supportedValues);
        }

        if (supportedValues == null || supportedValues.size() <= 1) {
            pref.setVisibled(false);
            return;
        }

        if (pref.getEntries().length <= 1) {
            pref.setVisibled(false);
            return;
        }

        resetIfInvalid(pref, true);
    }

    private void filterGroupListPrference(PreferenceGroup group, String key) {
        ListPreference groupPref = group.findPreference(key);
        if (groupPref == null) {
            return;
        }
        CharSequence[] entryValues = groupPref.getOriginalEntries();
        if (entryValues == null) {
            groupPref.setVisibled(false);
            return;
        }

        List<ListPreference> mChildPrefernce = new ArrayList<ListPreference>();
        for (CharSequence value : entryValues) {
            ListPreference pref = group.findPreference(value.toString());
            if (pref != null && pref.isVisibled()) {
                mChildPrefernce.add(pref);
            }
        }
        if (mChildPrefernce.size() <= 0) {
            groupPref.setVisibled(false);
        } else {
            groupPref.setChildPreferences(mChildPrefernce.toArray(
                    new ListPreference[mChildPrefernce.size()]));
        }
    }

    private void resetIfInvalid(ListPreference pref, boolean first) {
        // Set the value to the first entry if it is invalid.
        String value = pref.getValue();
        if (pref.findIndexOfValue(value) == -1) {
            if (first) {
                pref.setValueIndex(0);
            } else if (pref.getEntryValues() != null && pref.getEntryValues().length > 0) {
                pref.setValueIndex(pref.getEntryValues().length - 1);
            }
        }
    }
}
