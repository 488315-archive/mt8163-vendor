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
import android.os.SystemProperties;
import android.telephony.TelephonyManager;
import android.widget.CompoundButton;
import android.widget.RadioButton;
import android.widget.Toast;

import com.android.internal.telephony.Phone;
import com.android.internal.telephony.PhoneConstants;
import com.android.internal.telephony.PhoneFactory;
import com.mediatek.aal.AalUtils;
import com.mediatek.engineermode.Elog;
import com.mediatek.engineermode.R;
import com.mediatek.engineermode.ShellExe;



import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Power and Performance Customization Activity.
 */
public class PowerPerformance extends Activity implements CompoundButton.OnCheckedChangeListener {
    private static final String TAG = "EM/PowerPerf";
    private static final int TOTAL_ITEMS = 8;
    private static final int TOTAL_OPTIONS = 4;
    private static final int POWER_VALUE_LOW_POWER = 1;
    private static final int POWER_VALUE_BANLANCE = 2;
    private static final int POWER_VALUE_PERFORMANCE = 3;
    private static final String POWER_VALUE_STR_LOW_POWER = "low_power";
    private static final String POWER_VALUE_STR_PERFORMANCE = "performance";
    private static final int[][] OPTION_IDS = {
        {R.id.power_perf_1_1, R.id.power_perf_1_3},
        {R.id.power_perf_2_1, R.id.power_perf_2_3},
        {R.id.power_perf_3_1, R.id.power_perf_3_3},
        {R.id.power_perf_4_1, R.id.power_perf_4_3},
        {R.id.power_perf_5_1, R.id.power_perf_5_3},
        {R.id.power_perf_6_1, R.id.power_perf_6_2, R.id.power_perf_6_3},
        {R.id.power_perf_7_1, R.id.power_perf_7_2, R.id.power_perf_7_3},
    };
    private static final int[][] POWER_VALUES = {
        {POWER_VALUE_LOW_POWER, POWER_VALUE_PERFORMANCE},
        {POWER_VALUE_LOW_POWER, POWER_VALUE_PERFORMANCE},
        {POWER_VALUE_LOW_POWER, POWER_VALUE_PERFORMANCE},
        {POWER_VALUE_LOW_POWER, POWER_VALUE_PERFORMANCE},
        {POWER_VALUE_LOW_POWER, POWER_VALUE_PERFORMANCE},
        {POWER_VALUE_LOW_POWER, POWER_VALUE_BANLANCE, POWER_VALUE_PERFORMANCE},
        {POWER_VALUE_LOW_POWER, POWER_VALUE_BANLANCE, POWER_VALUE_PERFORMANCE},
    };
    private static final String[] FILE_NAMES = {
        "/proc/ppm/mode",
        "/proc/hps/power_mode",
        "/proc/cpufreq/cpufreq_power_mode",
        "/sys/kernel/ccci/mdsys1/md_rxd",
        "/d/ged/hal/event_notify",
        "d/dispsys",
    };
    private List<Item> mItems;
    private Toast mToast;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Elog.d(TAG, "onCreate");
        setContentView(R.layout.power_performance);
        mItems = new ArrayList<Item>();

        mItems.add(new PPM(FILE_NAMES[0], OPTION_IDS[0], POWER_VALUES[0]));
        mItems.add(new Item(FILE_NAMES[1], OPTION_IDS[1], POWER_VALUES[1]));
        mItems.add(new CpuDvfs(FILE_NAMES[2], OPTION_IDS[2], POWER_VALUES[2]));
        mItems.add(new MdRxd(FILE_NAMES[3], OPTION_IDS[3], POWER_VALUES[3]));
        mItems.add(new GpuDvfs(FILE_NAMES[4], OPTION_IDS[4], POWER_VALUES[4]));
        mItems.add(new DynamicDsiFreq(FILE_NAMES[5], OPTION_IDS[5], POWER_VALUES[5]));
        mItems.add(new AmsAal(null, OPTION_IDS[6], POWER_VALUES[6]));
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

         String outStr = ShellExe.getOutput();
         Elog.d(TAG, "[output]: " + outStr);
         if (ret == 0) {
//             String outStr = ShellExe.getOutput();
//             Elog.d(TAG, "[output]: " + outStr);
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
                RadioButton r = (RadioButton) PowerPerformance.this.findViewById(radioButtons[i]);
                r.setTag(this);
                r.setOnCheckedChangeListener(PowerPerformance.this);
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
            }
        }

        protected int getValue() {
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

        protected boolean setValue(int value) {
            return execCommand("echo " + value + " > " + mFilePath) != null;
        }
    }


    /**
     * DynamicDsiFreq item.
     *
     */
    class DynamicDsiFreq extends Item {
        private static final String CMD_PRE = "low_power_mode:";

        private static final String READ_PATH = "/d/disp/lowpowermode";
        public DynamicDsiFreq(String filePath, int[] radioButtons, int[] values) {
            super(filePath, radioButtons, values);
        }

        @Override
        protected int getValue() {
            String result = execCommand("cat " + READ_PATH);
            if (result != null) {
                result = result.trim();
                int index = result.indexOf("(");
                Elog.e(TAG, "index:" + index);
                result = result.substring(index + 1, index + 2);
                Elog.e(TAG, "result:" + result);
                try {
                    return Integer.parseInt(result);
                } catch (NumberFormatException e) {
                    Elog.e(TAG, "NumberFormatException");
                    return -1;
                }
            }
            return -1;
        }

        @Override
        protected boolean setValue(int value) {
           return execCommand("echo " + CMD_PRE + value + " > " + mFilePath) != null;

        }
    }

    /**
     * PPM item.
     *
     */
    class PPM extends Item {
        public PPM(String filePath, int[] radioButtons, int[] values) {
            super(filePath, radioButtons, values);
        }

        @Override
        protected int getValue() {
            String result = execCommand("cat " + mFilePath);
            if (result != null) {
                result = result.trim();
                result = result.toLowerCase();
                if (result.contains(POWER_VALUE_STR_LOW_POWER)) {
                    return POWER_VALUE_LOW_POWER;
                } else if (result.contains(POWER_VALUE_STR_PERFORMANCE)) {
                    return POWER_VALUE_PERFORMANCE;
                }
            }
            return -1;
        }

        @Override
        protected boolean setValue(int value) {

            switch (value) {
                case POWER_VALUE_LOW_POWER: {
                    return execCommand("echo " +
                POWER_VALUE_STR_LOW_POWER + " > " + mFilePath) != null;
                }
                case POWER_VALUE_PERFORMANCE: {
                    return execCommand("echo " +
                POWER_VALUE_STR_PERFORMANCE + " > " + mFilePath) != null;
                }
                default:
                    return false;
            }

        }
    }

    /**
     * CpuDvfs item.
     *
     */
    class CpuDvfs extends Item {

        private static final String CPU_LOW_POWER = "Low_power_Mode";
        private static final String CPU_PERFORMANCE = "Performance_Mode";
        public CpuDvfs(String filePath, int[] radioButtons, int[] values) {
            super(filePath, radioButtons, values);
        }

        @Override
        protected int getValue() {
            String result = execCommand("cat " + mFilePath);
            if (result != null) {
                result = result.trim();
                if (result != null) {
                    result = result.trim();
                    if (CPU_LOW_POWER.equalsIgnoreCase(result)) {
                        return POWER_VALUE_LOW_POWER;
                    } else if (CPU_PERFORMANCE.equalsIgnoreCase(result)) {
                        return POWER_VALUE_PERFORMANCE;
                    }
                }
            }
            return -1;
        }

    }

    /**
     * MdRxd item.
     *
     */
    class MdRxd extends Item {
        private static final String LOW_POWER_CMD = "AT+ERXPATH=10,0";
        private static final String PERFORMANCE_POWER_CMD = "AT+ERXPATH=12,0";
        private Phone mPhone = null;

        public MdRxd(String filePath, int[] radioButtons, int[] values) {
            super(filePath, radioButtons, values);
            if (TelephonyManager.getDefault().getPhoneCount() > 1) {
                mPhone = PhoneFactory.getPhone(PhoneConstants.SIM_ID_1);
            } else {
                mPhone = PhoneFactory.getDefaultPhone();
            }
        }

        @Override
        protected int getValue() {
            String result = execCommand("cat " + mFilePath);
            if (result != null) {
                result = result.trim();
                int index = result.indexOf("=");
                Elog.e(TAG, "index:" + index);
                result = result.substring(index + 1, index + 2);
                Elog.e(TAG, "result:" + result);
                try {
                    return Integer.parseInt(result);
                } catch (NumberFormatException e) {
                    Elog.e(TAG, "NumberFormatException");
                    return -1;
                }
            }
            return -1;
        }

        @Override
        protected boolean setValue(int value) {

            if (execCommand("echo " + value + " > " + mFilePath) == null) {
                return false;
            }

            if (value == POWER_VALUE_LOW_POWER) {
                sendATCommand(LOW_POWER_CMD);
            } else if (value == POWER_VALUE_PERFORMANCE) {
                sendATCommand(PERFORMANCE_POWER_CMD);
            }

            return true;
        }

        private void sendATCommand(String str) {
            Elog.d(TAG, "sendATCommand: " + str);
            String[] cmd = new String[2];
            cmd[0] = str;
            cmd[1] = "";
            mPhone.invokeOemRilRequestStrings(cmd, null);
        }

    }

    /**
     * GpuDvfs item.
     *
     */
    class GpuDvfs extends Item {
        private static final String GPU_LOW_POWER = "low-power-mode 1";
        private static final String GPU_PERFORMANCE = "low-power-mode 0";
        private static final String SYS_PROP_GPU = "persist.mtk.sf.fps.upper_bound";

        public GpuDvfs(String filePath, int[] radioButtons, int[] values) {
            super(filePath, radioButtons, values);
        }

        @Override
        protected int getValue() {
            String result = SystemProperties.get(SYS_PROP_GPU, null);
            Elog.e(TAG, "SystemProperties:" + result);

            if (result != null) {
                result = result.trim();
                if (GPU_LOW_POWER.equalsIgnoreCase(result)) {
                    return POWER_VALUE_LOW_POWER;
                } else if (GPU_PERFORMANCE.equalsIgnoreCase(result)) {
                    return POWER_VALUE_PERFORMANCE;
                }
            }
            return -1;
        }

        @Override
        protected boolean setValue(int value) {

            switch (value) {
                case POWER_VALUE_LOW_POWER: {
                    Elog.d(TAG, "echo " + GPU_LOW_POWER + " > " + mFilePath);
                    return execCommand("echo " + GPU_LOW_POWER + " > " + mFilePath) != null;
                }
                case POWER_VALUE_PERFORMANCE: {
                    Elog.d(TAG, "echo " + GPU_PERFORMANCE + " > " + mFilePath);
                    return execCommand("echo " + GPU_PERFORMANCE + " > " + mFilePath) != null;
                }
                default:
                    return false;
            }

        }
    }

    /**
     * AmsAal item.
     *
     */
    class AmsAal extends Item {
        private static final String GPU_LOW_POWER = "30";
        private static final String GPU_LOW_PERFORMANCE = "60";
        private static final String SYS_PROP_GPU = "persist.radio.fd.counter";

        public AmsAal(String filePath, int[] radioButtons, int[] values) {
            super(filePath, radioButtons, values);
        }

        @Override
        protected int getValue() {
            return -1;
        }

        @Override
        protected boolean setValue(int value) {
            AalUtils.getInstance().setEnabled(PowerPerformance.this, true);
            switch (value) {
                case POWER_VALUE_LOW_POWER: {
                    AalUtils.getInstance().setAalMode(
                            PowerPerformance.this, AalUtils.AAL_MODE_LOWPOWER);
                    break;
                }
                case POWER_VALUE_BANLANCE: {
                    AalUtils.getInstance().setAalMode(
                            PowerPerformance.this, AalUtils.AAL_MODE_BALANCE);
                    break;
                }
                case POWER_VALUE_PERFORMANCE: {
                    AalUtils.getInstance().setAalMode(
                            PowerPerformance.this, AalUtils.AAL_MODE_PERFORMANCE);
                    break;
                }
                default:
                    return false;
            }
            return true;

        }
    }
}
