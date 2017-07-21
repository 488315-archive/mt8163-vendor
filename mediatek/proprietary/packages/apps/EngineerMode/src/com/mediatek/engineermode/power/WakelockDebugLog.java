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

package com.mediatek.engineermode.power;

import android.app.Activity;
import android.os.Bundle;
import android.widget.CompoundButton;
import android.widget.RadioButton;
import android.widget.Toast;

import com.mediatek.engineermode.Elog;
import com.mediatek.engineermode.R;
import com.mediatek.engineermode.ShellExe;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Wakelock Debug Log Activity.
 */
public class WakelockDebugLog extends Activity implements CompoundButton.OnCheckedChangeListener {
    private static final String TAG = "EM/WakelockDebugLog";
    private static final int TOTAL_ITEMS = 1;
    private static final int TOTAL_OPTIONS = 2;
    private static final int ENABLE = 1;
    private static final int DISABLE = 0;
    private static final int[][] OPTION_IDS = {
            {R.id.wakelock_debug_log_enable, R.id.wakelock_debug_log_disable},
    };
    private static final String[] FILE_NAMES = {
        "/proc/power/wakelock/enable_raw_log",
    };
    private static final int[] POWER_VALUES = {
        ENABLE,
        DISABLE,
    };
    private List<Item> mItems;
    private Toast mToast;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Elog.d(TAG, "onCreate");
        setContentView(R.layout.wakelock_debug_log);
        mItems = new ArrayList<Item>();
        for (int i = 0; i < TOTAL_ITEMS; i++) {
            mItems.add(new Item(FILE_NAMES[i], OPTION_IDS[i], POWER_VALUES));
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        Elog.d(TAG, "onResume");
        for (Item i : mItems) {
            i.queryCurrent();
        }
    }

    @Override
    public void onCheckedChanged(CompoundButton button, boolean check) {
        Elog.d(TAG, "onCheckChanged " + button.getText().toString() + "," + check);
        if (check) {
            Item i = (Item) button.getTag();
            i.check(button);
        }
    }

    private String execCommand(String cmd) {
         int ret = -1;
         Elog.d(TAG, "[cmd]:" + cmd);
         //Toast.makeText(this, cmd, Toast.LENGTH_LONG).show();
         try {
             ret = ShellExe.execCommand(cmd);
         } catch (IOException e) {
             Elog.e(TAG, "IOException: " + e.getMessage());
         }
         if (ret == 0) {
             String outStr = ShellExe.getOutput();
             Elog.d(TAG, "[output]: " + outStr);
             return outStr;
         }
         return null;
     }

    private void showToast(String msg) {
        if (mToast != null) {
            mToast.cancel();
        }
        mToast = Toast.makeText(this, msg, Toast.LENGTH_SHORT);
        mToast.show();
    }

    /**
     * Helper class.
     */
    class Item {
        protected List<RadioButton> mRadioButtons;
        protected Map<CompoundButton, Integer> mValues;
        protected int mCurrentValue;
        protected String mFilePath;

        public Item(String filePath, int[] radioButtons, int[] values) {
            mFilePath = filePath;
            mRadioButtons = new ArrayList<RadioButton>();
            mValues = new HashMap<CompoundButton, Integer>();
            for (int i = 0; i < radioButtons.length; i++) {
                RadioButton r = (RadioButton) WakelockDebugLog.this.findViewById(radioButtons[i]);
                r.setTag(this);
                r.setOnCheckedChangeListener(WakelockDebugLog.this);
                mRadioButtons.add(r);
                mValues.put(r, values[i]);
            }
        }

        void check(CompoundButton button) {
            int value = mValues.get(button);
            if (setValue(value)) {
                showToast("Set done");
            } else {
                showToast("Set failed");
            }
            for (RadioButton r : mRadioButtons) {
                if (r != button) {
                    r.setChecked(false);
                }
            }
            mCurrentValue = value;
        }

        void queryCurrent() {
            mCurrentValue = getValue();
            Elog.d(TAG, "queryCurrent() mCurrentValue = " + mCurrentValue);
            if (mCurrentValue >= 0) {
                for (RadioButton r : mRadioButtons) {
                    r.setChecked(mCurrentValue == mValues.get(r));
                }
            } else {
                showToast("Get current value failed");
            }
        }

        int getValue() {
            String result = execCommand("cat " + mFilePath);
            if (result != null) {
                try {
                    return Integer.parseInt(result);
                } catch (NumberFormatException e) {
                    Elog.e(TAG, "NumberFormatException");
                    return -1;
                }
            }
            return -1;
        }

        boolean setValue(int value) {
            return execCommand("echo " + value + " > " + mFilePath) != null;
        }
    }
}
