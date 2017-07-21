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

package com.mediatek.engineermode.desenseat;

import android.annotation.TargetApi;
import android.app.Activity;
import android.app.WallpaperManager;
import android.bluetooth.BluetoothAdapter;
import android.content.ActivityNotFoundException;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.graphics.Point;
import android.net.Uri;
import android.os.Build;

import com.mediatek.engineermode.ChipSupport;
import com.mediatek.engineermode.Elog;
import com.mediatek.engineermode.FeatureSupport;
import com.mediatek.engineermode.ModemCategory;
import com.mediatek.engineermode.bluetooth.BtTest;
import com.mediatek.engineermode.wifi.EMWifi;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import java.util.Timer;
import java.util.TimerTask;

/**
 * Test items.
 */
public abstract class TestItem {
    protected static final String TAG = "EM/DesenseAT";
    private static ArrayList<TestItem> sTestItems;
    protected Context mContext;
    private TestMode mTestMode;
    private TestResult mTestModeResult;
    protected static final int TEST_STATE_ON = 0;
    protected static final int TEST_STATE_OFF = 1;
    public static final String TEST_MP3_FILE = "DesenseAT.mp3";
    public static final String TEST_3GP_FILE = "DesenseAT.3gp";
    public static final String TEST_GIF_FILE = "DesenseAT.gif";
    public static List<String> sItemsToCheck;
    private int mItemIndex;
    private long mTimeToWait;
    private boolean mTimeOut = false;
    private Timer mTimer;
    private TimerTask mTimerTask;
    protected int getDbSpec() {
        return 30;
    }
    protected boolean isTestDone() {
        return mTestModeResult.state == TestResult.ResultState.STATE_DONE;
    }

    protected boolean isTestRefDone() {
        return mTestModeResult.state == TestResult.ResultState.STATE_REF_DONE;
    }
    protected boolean isTestNa() {
        return mTestModeResult.state == TestResult.ResultState.STATE_NA;
    }

    public void setItemIndex(int itemIndex) {
        mItemIndex = itemIndex;
    }


    static ArrayList<TestItem> getTestItems(Context context) {
        if (sTestItems == null) {
            sTestItems = new ArrayList<TestItem>();
            sTestItems.add(new Display3DTest());
            sTestItems.add(new CpuFullLoadingTest());
            sTestItems.add(new SleepToNormalTest());
            sTestItems.add(new LcmOnTest());
            sTestItems.add(new LcmUpdateTest());
            sTestItems.add(new BacklightTest());
            sTestItems.add(new WifiTxTest());
            sTestItems.add(new WifiRxTest());
            sTestItems.add(new BtRxInquiryScanTest());
            sTestItems.add(new BtTxTest());
            sTestItems.add(new LiveWallpaperTest());
            sTestItems.add(new Mp3PlayTest());
            sTestItems.add(new VideoPlayTest());
            sTestItems.add(new PhoneStorageReadTest());
            sTestItems.add(new PhoneStorageWriteTest());
            sTestItems.add(new ExternalSdCardReadTest());
            sTestItems.add(new ExternalSdCardWriteTest());
            sTestItems.add(new MemoryAccessTest());
            sTestItems.add(new FrontCameraPreviewTest());
            sTestItems.add(new BackCameraPreviewTest());
            sTestItems.add(new GsmBandVirtualTest());
            if (ModemCategory.getModemType() == ModemCategory.MODEM_TD) {
                sTestItems.add(new TddBandVirtualTest());
            } else if (ModemCategory.getModemType() == ModemCategory.MODEM_FDD) {
                sTestItems.add(new FddBandVirtualTest());
            }
            if (FeatureSupport.isSupported(FeatureSupport.FK_LTE_SUPPORT)) {
                sTestItems.add(new LteBandVirtualTest());
            }
            if (ModemCategory.isCdma()) {
                sTestItems.add(new CdmaBandVirtualTest());

            }

        }

        for (TestItem i : sTestItems) {
            i.setContext(context);
        }
        return sTestItems;
    }

    abstract protected void init();

    abstract protected void doTest();

    abstract protected boolean doApiTest();
/*
    float getSpec() {
        return 1;
    }
*/

    public boolean isCancelled() {
        return DesenseAtActivity.getCancelled();
//        return mContext.isCancelled();
    }

    void sleep(long time) {
        // TODO: not exactly
        if (!isCancelled()) {
            try {
                Thread.sleep(time);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }

    TestResult run() {
        // Pass 1
        Elog.d(TAG, "init()");
        init();
        Elog.d(TAG, "after init()");
        if (isCancelled()) {
            Elog.d(TAG, "return null");
            return null;
        }
        Elog.d(TAG, "startTestMode()");
        startTestMode(TEST_STATE_ON);
        Elog.d(TAG, "stopTestMode()");
        stopTestMode(TEST_STATE_ON);
        Elog.d(TAG, "after stopTestMode()");
        if (mTimeOut && !isTestRefDone()) {
            mTestModeResult.result = TestResult.CONNECTION_FAIL;
        }

        if (mTestModeResult.result == TestResult.CONNECTION_FAIL) {
            return mTestModeResult;
        }

        // Pass 2
        Elog.d(TAG, "Pass 2");
        if (isCancelled()) {
            Elog.d(TAG, "return null");
            return null;
        }
        Elog.d(TAG, "doTest()");
        doTest();
        if (mTimeOut && !isTestDone()) {
            mTestModeResult.result = TestResult.CONNECTION_FAIL;
        }

        Elog.d(TAG, "calculateResult()");
        return mTestModeResult;

    }
/*
    private TestResult calculateResult(float[] result1, float[] result2) {
        TestResult result = new TestResult();
        float[] data = new float[result1.length + result2.length];
        float min = Float.POSITIVE_INFINITY;
        float max = Float.NEGATIVE_INFINITY;
        float total1 = 0;
        float total2 = 0;
        int j = 0;
        for (int i = 0; i < result1.length; i += 2) {
            total1 += result1[i + 1];
            if (result1[i + 1] < min) {
                min = result1[i + 1];
            }
            if (result1[i + 1] > max) {
                max = result1[i + 1];
            }
            data[j++] = result1[i];
            data[j++] = result1[i + 1];
        }
        for (int i = 0; i < result2.length; i += 2) {
            total2 += result2[i + 1];
            if (result2[i + 1] < min) {
                min = result2[i + 1];
            }
            if (result2[i + 1] > max) {
                max = result2[i + 1];
            }
            data[j++] = result2[i];
            data[j++] = result2[i + 1];
        }
        float average = (total1 + total2) / (data.length / 2);
        float diff = Math.abs(total1 / (result1.length / 2) - total2 / (result2.length / 2));
        result.value = String.valueOf(diff);
        result.result = diff <= getSpec() ? TestResult.PASS : TestResult.FAIL;
        result.data = data;
        result.summary = "Average:" + average + " Max:" + max + " Min:" + min;
        result.x = "TOW";
        result.y = "CNR";
        return result;
    }
*/
    void setContext(Context context) {
        mContext = context;
    }

    void setTestMode(TestMode mode) {
        mTestMode = mode;
    }

    protected void startTestMode(int testState) {
        if (testState == TEST_STATE_ON) {
            mTestModeResult = null;
        }
        mTestMode.setParameters(mItemIndex, testState, 30);
        new Thread() {
            public void run() {
                mTestModeResult = mTestMode.run();
            }
        }.start();

        mTimeOut = false;
        if (mTestMode.toString().equals("Signal")) {
            mTimeToWait = 40000;
        } else {
            mTimeToWait = 30000;
        }
        mTimer = new Timer();
        mTimerTask = new TimerTask() {
            public void run() {
                mTimeOut = true;
            }
        };
        mTimer.schedule(mTimerTask, mTimeToWait);

    }


    protected void stopTestMode(int testState) {
//        mTestMode.stop();

        if (testState == 0) {
            while ((mTestModeResult == null)
                    || (!isTestRefDone() && !isTestNa())) {
                if (!mTimeOut) {
                    sleep(100);
                } else {
                    break;
                }

            }
        } else {
            while ((mTestModeResult == null)
                    || (!isTestDone() && !isTestNa())) {
                if (!mTimeOut) {
                    sleep(100);
                } else {
                    break;
                }
            }
        }
        mTimer.cancel();
        mTimerTask.cancel();
        mTimer.purge();
        mTimer = null;
        mTimerTask = null;

    }
}

/**
 * TestMode.
 */
class SleepToNormalTest extends TestItem {
    @Override
    public void init() {
        Util.enterSleepMode(mContext);
        sleep(5 * 1000);
    }

    @Override
    public void doTest() {
        Util.leaveSleepMode(mContext);
        startTestMode(TEST_STATE_OFF);
        stopTestMode(TEST_STATE_OFF);
    }

    @Override
    public String toString() {
        return "Suspend";
    }

    @Override
    protected boolean doApiTest() {
        // TODO Auto-generated method stub
        // TODO Auto-generated method stub

        if (!Util.enterSleepMode(mContext)) {
            return false;
        }
        sleep(5000);
        if (!Util.leaveSleepMode(mContext)) {
            return false;
        }
        return true;

    }
}

/**
 * TestMode.
 */
class LcmOnTest extends TestItem {
    protected int getDbSpec() {
        return 10;
    }
    @Override
    public void init() {
        Util.enterFlightMode(mContext);
        Util.lcmOff();
        sleep(5 * 1000);
    }

    @Override
    public void doTest() {
        Util.lcmOn();
        sleep(5 * 1000);
        startTestMode(TEST_STATE_OFF);
//        sleep(30 * 1000);
        stopTestMode(TEST_STATE_OFF);
    }

    @Override
    public String toString() {
        return "LCM on";
    }


    @Override
    protected boolean doApiTest() {
        // TODO Auto-generated method stub

        if (!Util.lcmOff()) {
            return false;
        }
        sleep(3000);
        if (!Util.lcmOn()) {
            return false;
        }
        return true;
    }
}

/**
 * TestMode.
 */
class LcmUpdateTest extends TestItem {
    @Override
    public void init() {
        Util.enterFlightMode(mContext);
        Util.lcmOff();
        sleep(5 * 1000);
    }

    @Override
    public void doTest() {
        Util.lcmOn();
        sleep(5 * 1000);
        startTestMode(TEST_STATE_OFF);
        // TODO: update random data
        stopTestMode(TEST_STATE_OFF);
    }

    @Override
    public String toString() {
        return "LCM update";
    }

    @Override
    protected boolean doApiTest() {
        // TODO Auto-generated method stub

        if (!Util.lcmOff()) {
            return false;
        }
        sleep(3000);
        if (!Util.lcmOn()) {
            return false;
        }
        return true;
    }
}

/**
 * TestMode.
 */
class BacklightTest extends TestItem {
    @Override
    public void init() {
        Util.enterFlightMode(mContext);
        Util.backlightOff();
        sleep(5 * 1000);
    }

    @Override
    public void doTest() {
        Util.backlightOn();
        startTestMode(TEST_STATE_OFF);
        stopTestMode(TEST_STATE_OFF);
    }

    @Override
    public String toString() {
        return "Backlight";
    }

    @Override
    protected boolean doApiTest() {
        // TODO Auto-generated method stub

        if (!Util.backlightOff()) {
            return false;
        }
        sleep(3000);
        if (!Util.backlightOn()) {
            return false;
        }
        return true;

    }
}

/**
 * TestMode.
 */
class WifiTxTest extends TestItem {
    // From WiFiTx6620.java
    private static final int ATPARAM_INDEX_COMMAND = 1;
    private static final int ATPARAM_INDEX_POWER = 2;
    private static final int ATPARAM_INDEX_RATE = 3;
    private static final int ATPARAM_INDEX_PREAMBLE = 4;
    private static final int ATPARAM_INDEX_ANTENNA = 5;
    private static final int ATPARAM_INDEX_PACKLENGTH = 6;
    private static final int ATPARAM_INDEX_PACKCOUNT = 7;
    private static final int ATPARAM_INDEX_PACKINTERVAL = 8;
    private static final int ATPARAM_INDEX_TEMP_COMPENSATION = 9;
    private static final int ATPARAM_INDEX_TXOP_LIMIT = 10;
    private static final int ATPARAM_INDEX_PACKCONTENT = 12;
    private static final int ATPARAM_INDEX_RETRY_LIMIT = 13;
    private static final int ATPARAM_INDEX_QOS_QUEUE = 14;
    private static final int ATPARAM_INDEX_BANDWIDTH = 15;
    private static final int ATPARAM_INDEX_GI = 16;
    private static final int TXOP_LIMIT_VALUE = 0x00020000;
    private static final int ANTENNA = 0;
    private static final long[] PACKCONTENT_BUFFER =
            { 0xff220004, 0x33440006, 0x55660008, 0x55550019, 0xaaaa001b, 0xbbbb001d };
    private static final int COMMAND_INDEX_STARTTX = 1;
    private static final int COMMAND_INDEX_STOPTEST = 0;

    @Override
    public void init() {
        Util.leaveFlightMode(mContext);
        Util.lcmOff();
        Util.backlightOff();
        Util.disableWifi(mContext);
        sleep(5 * 1000);
    }

    @Override
    public void doTest() {
        Util.enableWifi(mContext);
        startTestMode(TEST_STATE_OFF);
        // Rate index:
        //    Refer to com.mediatek.engineermode.wifi.WiFiTx6620.RateInfo.mPszRate
        // Channel frequency:
        //    Refer to com.mediatek.engineermode.wifi.ChannelInfo.sWifiChannelDatas
        doTx(19, 0, 2412000); // 19dBm, 1M, Channel 1
        doTx(16, 0, 2437000); // 19dBm, 1M, Channel 6
        doTx(16, 0, 2462000); // 19dBm, 1M, Channel 11
        doTx(16, 11, 5500000); // 16dBm, 54M, Channel 100
        stopTestMode(TEST_STATE_OFF);
        Util.backlightOn();
        Util.lcmOn();
    }

    private void doTx(int power, int rateIndex, int channel) {
        if (isCancelled()) {
            return;
        }
        // From WiFiTx6620.java
        power *= 2;
        EMWifi.setATParam(ATPARAM_INDEX_POWER, power);
        EMWifi.setATParam(ATPARAM_INDEX_PREAMBLE, 0);
        EMWifi.setATParam(ATPARAM_INDEX_ANTENNA, ANTENNA);
        EMWifi.setATParam(ATPARAM_INDEX_PACKLENGTH, 1024);
        EMWifi.setATParam(ATPARAM_INDEX_PACKCOUNT, 3000);
        EMWifi.setATParam(ATPARAM_INDEX_PACKINTERVAL, 20);
        EMWifi.setATParam(ATPARAM_INDEX_TEMP_COMPENSATION, 0);
        EMWifi.setATParam(ATPARAM_INDEX_TXOP_LIMIT, TXOP_LIMIT_VALUE);
        for (int i = 0; i < PACKCONTENT_BUFFER.length; i++) {
            EMWifi.setATParam(ATPARAM_INDEX_PACKCONTENT, PACKCONTENT_BUFFER[i]);
        }
        EMWifi.setATParam(ATPARAM_INDEX_RETRY_LIMIT, 1);
        EMWifi.setATParam(ATPARAM_INDEX_QOS_QUEUE, 2);
        EMWifi.setATParam(ATPARAM_INDEX_GI, 0);
        EMWifi.setATParam(ATPARAM_INDEX_BANDWIDTH, 0); // TODO: check it
        EMWifi.setATParam(ATPARAM_INDEX_RATE, rateIndex);
        EMWifi.setChannel(channel);
        int ret = EMWifi.setATParam(ATPARAM_INDEX_COMMAND, COMMAND_INDEX_STARTTX);
        Elog.d(TAG, "start tx ret " + ret);
        sleep(15 * 1000); // TODO: is it OK?
        ret = EMWifi.setATParam(ATPARAM_INDEX_COMMAND, COMMAND_INDEX_STOPTEST);
        Elog.d(TAG, "stop tx ret " + ret);
    }

    @Override
    public String toString() {
        return "WIFI TX";
    }

    @Override
    protected boolean doApiTest() {
        // TODO Auto-generated method stub
        return ChipSupport.isFeatureSupported(ChipSupport.MTK_WLAN_SUPPORT);
    }
}

/**
 * TestMode.
 */
class WifiRxTest extends TestItem {
    // From WiFiRx6620.java
    private static final int ATPARAM_INDEX_COMMAND = 1;
    private static final int ATPARAM_INDEX_TEMP_COMPENSATION = 9;
    private static final int ATPARAM_INDEX_BANDWIDTH = 15;

    @Override
    public void init() {
        Util.leaveFlightMode(mContext);
        Util.lcmOff();
        Util.backlightOff();
        Util.disableWifi(mContext);
        sleep(5 * 1000);
    }

    @Override
    public void doTest() {
        Util.enableWifi(mContext);
        startTestMode(TEST_STATE_OFF);
        doRx();
        stopTestMode(TEST_STATE_OFF);
        Util.backlightOn();
        Util.lcmOn();
    }

    private void doRx() {
        if (isCancelled()) {
            return;
        }
        // Channel 7, Bandwidth 20MHz
        EMWifi.setChannel(2442000);
        EMWifi.setATParam(ATPARAM_INDEX_TEMP_COMPENSATION, 0);
        EMWifi.setATParam(ATPARAM_INDEX_BANDWIDTH, 0);
        int ret = EMWifi.setATParam(ATPARAM_INDEX_COMMAND, 2);
        Elog.d(TAG, "start rx ret " + ret);
        sleep(30 * 1000);
        ret = EMWifi.setATParam(ATPARAM_INDEX_COMMAND, 0);
        Elog.d(TAG, "stop rx ret " + ret);
    }

    @Override
    public String toString() {
        return "WIFI RX";
    }

    @Override
    protected boolean doApiTest() {
        // TODO Auto-generated method stub
        return ChipSupport.isFeatureSupported(ChipSupport.MTK_WLAN_SUPPORT);

    }
}

/**
 * TestMode.
 */
class BtRxInquiryScanTest extends TestItem {
    @Override
    public void init() {
        Util.leaveFlightMode(mContext);
        Util.lcmOff();
        Util.backlightOff();
        Util.disableBt();
        sleep(5 * 1000);
    }

    @Override
    public void doTest() {
        Util.enableBt();
        startTestMode(TEST_STATE_OFF);
        BluetoothAdapter.getDefaultAdapter().startDiscovery();
        Elog.d(TAG, "startDiscovery");
        sleep(30 * 1000);
        BluetoothAdapter.getDefaultAdapter().cancelDiscovery();
        Elog.d(TAG, "cancelDiscovery");
        stopTestMode(TEST_STATE_OFF);
        Util.backlightOn();
        Util.lcmOn();
    }

    @Override
    public String toString() {
        return "BT RX inquiry scan";
    }

    @Override
    protected boolean doApiTest() {
        // TODO Auto-generated method stub
        return ChipSupport.isFeatureSupported(ChipSupport.MTK_BT_SUPPORT);
    }
}

/**
 * TestMode.
 */
class BtTxTest extends TestItem {
    @Override
    public void init() {
        Util.leaveFlightMode(mContext);
        Util.lcmOff();
        Util.backlightOff();
        Util.disableBtEm();
        sleep(5 * 1000);
    }

    @Override
    public void doTest() {
        Util.enableBtEm();
        startTestMode(TEST_STATE_OFF);

        // reference to com.mediatek.engineermode.bluetooth.TxOnlyTestActivity
        BtTest mBtTest = new BtTest();
        Elog.d(TAG, "start bt tx");
        Util.disableBt();
        mBtTest.setPatter(3); // pseudo random bit
        mBtTest.setChannels(1); // 79 channel
        mBtTest.setPocketType(4); // DH1
        mBtTest.setPocketTypeLen(27);
        mBtTest.doBtTest(0);
        mBtTest.pollingStart();
        sleep(30 * 1000);
        Elog.d(TAG, "stop bt tx");
        mBtTest.pollingStop();
        mBtTest.doBtTest(3);

        stopTestMode(TEST_STATE_OFF);
        Util.backlightOn();
        Util.lcmOn();
    }

    @Override
    public String toString() {
        return "BT TX";
    }

    @Override
    protected boolean doApiTest() {
        // TODO Auto-generated method stub
        return ChipSupport.isFeatureSupported(ChipSupport.MTK_BT_SUPPORT);
   }
}

/**
 * TestMode.
 */
@TargetApi(Build.VERSION_CODES.JELLY_BEAN)
class LiveWallpaperTest extends TestItem {
    public static final int PICK_LIVE_WALLPAPER = 1537;

    @Override
    protected int getDbSpec() {
        return 15;
    }
    @Override
    public void init() {
        Util.lcmOn();
        Util.enterFlightMode(mContext);
        sleep(5 * 1000);
    }

    @Override
    public void doTest() {
        ComponentName comp = new ComponentName("com.android.wallpaper.holospiral",
                "com.android.wallpaper.holospiral.HoloSpiralWallpaper");
        Intent preview = new Intent(WallpaperManager.ACTION_CHANGE_LIVE_WALLPAPER);
        preview.putExtra(WallpaperManager.EXTRA_LIVE_WALLPAPER_COMPONENT, comp);
        ((Activity) mContext).startActivityForResult(preview, PICK_LIVE_WALLPAPER);
        sleep(5 * 1000);
        startTestMode(TEST_STATE_OFF);
//        sleep(30 * 1000);
        stopTestMode(TEST_STATE_OFF);

        Util.pressBack();
    }

    @Override
    public String toString() {
        return "Live wallpaper";
    }

    @Override
    protected boolean doApiTest() {
        // TODO Auto-generated method stub
        ComponentName comp = new ComponentName("com.android.wallpaper.holospiral",
                "com.android.wallpaper.holospiral.HoloSpiralWallpaper");
        Intent preview = new Intent(WallpaperManager.ACTION_CHANGE_LIVE_WALLPAPER);
        preview.putExtra(WallpaperManager.EXTRA_LIVE_WALLPAPER_COMPONENT, comp);
        return (Util.isIntentAvailable(mContext, preview));

    }
}

/**
 * TestMode.
 */
class Mp3PlayTest extends TestItem {
    @Override
    public void init() {
        Util.enterFlightMode(mContext);
        Util.lcmOff();
        Util.backlightOff();
        sleep(5 * 1000);
    }

    @Override
    public void doTest() {
        Intent intent = new Intent();
        intent.setAction(android.content.Intent.ACTION_VIEW);
        String filePath = Util.getTestFilePath(mContext, TEST_MP3_FILE);
        Elog.d(TAG, "filePath:" + filePath);
        File file = new File(filePath);
        intent.setDataAndType(Uri.fromFile(file), "audio/*");
        mContext.startActivity(intent);
        startTestMode(TEST_STATE_OFF);
        stopTestMode(TEST_STATE_OFF);
        Util.pressBack();
        Util.backlightOn();
        Util.lcmOn();
    }

    @Override
    public String toString() {
        return "MP3 playing";
    }

    @Override
    protected boolean doApiTest() {
        // TODO Auto-generated method stub
        Intent intent = new Intent();
        intent.setAction(android.content.Intent.ACTION_VIEW);
        String filePath = Util.getTestFilePath(mContext, TEST_MP3_FILE);
        Elog.d(TAG, "filePath:" + filePath);
        return (Util.isFileExist(filePath) && (Util.isIntentAvailable(mContext, intent)));

    }

}

/**
 * TestMode.
 */
class VideoPlayTest extends TestItem {
    @Override
    public void init() {
        Util.enterFlightMode(mContext);
        sleep(5 * 1000);
    }

    @Override
    public void doTest() {
        Intent intent = new Intent();
        intent.setAction(android.content.Intent.ACTION_VIEW);
        String filePath = Util.getTestFilePath(mContext, TEST_3GP_FILE);
        Elog.d(TAG, "filePath:" + filePath);
        File file = new File(filePath);
        intent.setDataAndType(Uri.fromFile(file), "video/*");
        mContext.startActivity(intent);
        startTestMode(TEST_STATE_OFF);
        stopTestMode(TEST_STATE_OFF);
        Util.pressBack();
    }

    @Override
    public String toString() {
        return "Video playing";
    }

    @Override
    protected boolean doApiTest() {
        // TODO Auto-generated method stub
        Intent intent = new Intent();
        intent.setAction(android.content.Intent.ACTION_VIEW);
        String filePath = Util.getTestFilePath(mContext, TEST_3GP_FILE);
        Elog.d(TAG, "filePath:" + filePath);
        return (Util.isFileExist(filePath) && Util.isIntentAvailable(mContext, intent));

    }
}

/**
 * TestMode.
 */
class CpuFullLoadingTest extends TestItem {
    @Override
    public void init() {
        Util.enterFlightMode(mContext);
        Util.lcmOff();
        Util.backlightOff();
        sleep(5 * 1000);
    }
    private Point mRadioBtnPoint = new Point(421, 370);
    private Point mStartBtnPoint = new Point(364, 533);

    @Override
    public void doTest() {
        Intent intent = new Intent();
        intent.setClassName("com.lyy.calculatepai", "com.lyy.calculatepai.CalculatePaiActivity");
        try {
            mContext.startActivity(intent);
            sleep(1000);
            Util.clickPoint(mRadioBtnPoint, false);
            sleep(500);
            Util.clickPoint(mStartBtnPoint, false);
            sleep(500);
            startTestMode(TEST_STATE_OFF);

            stopTestMode(TEST_STATE_OFF);
            sleep(90000);
            Util.pressBack();
            Util.leaveFlightMode(mContext);
            Util.backlightOn();
            Util.lcmOn();
        } catch (ActivityNotFoundException e) {
            Elog.d(TAG, "ActivityNotFoundException:" + e.getMessage());
        }
    }

    @Override
    public String toString() {
        return "CPU full loading";
    }

    @Override
    protected boolean doApiTest() {
        // TODO Auto-generated method stub
        Intent intent = new Intent();
        intent.setClassName("com.lyy.calculatepai", "com.lyy.calculatepai.CalculatePaiActivity");
        return (Util.isIntentAvailable(mContext, intent));

    }
}

/**
 * TestMode.
 */
class PhoneStorageReadTest extends TestItem {
    private boolean mRunning = false;
    private SdTest mSdTest;

    @Override
    public void init() {
        mSdTest = new SdTest(mContext);
        mSdTest.doTest(SdTest.TYPE_PREPARE, SdTest.STORAGE_INTERNAL, this);
        Util.enterFlightMode(mContext);
        Util.lcmOff();
        Util.backlightOff();
        sleep(5 * 1000);
    }

    @Override
    public void doTest() {
        startTestMode(TEST_STATE_OFF);
        readFiles();
        stopTestMode(TEST_STATE_OFF);
        Util.leaveFlightMode(mContext);
        Util.backlightOn();
        Util.lcmOn();
    }

    private void readFiles() {
        long start = System.currentTimeMillis();
        while (System.currentTimeMillis() - start < 30 * 1000 && !isCancelled() && !isTestDone()) {
            mSdTest.doTest(SdTest.TYPE_READ, SdTest.STORAGE_INTERNAL, this);
        }
    }

    @Override
    public String toString() {
        return "Phone storage reading";
    }

    @Override
    protected boolean doApiTest() {
        // TODO Auto-generated method stub
        SdTest sdTest = new SdTest(mContext);
        if (sdTest.doApiTest(SdTest.STORAGE_INTERNAL)) {
            return true;
        } else {
            return false;
        }
    }
}

/**
 * TestMode.
 */
class PhoneStorageWriteTest extends TestItem {
    private boolean mRunning = false;
    private SdTest mSdTest;

    @Override
    public void init() {
        mSdTest = new SdTest(mContext);
        mSdTest.doTest(SdTest.TYPE_PREPARE, SdTest.STORAGE_INTERNAL, this);
        Util.enterFlightMode(mContext);
        Util.lcmOff();
        Util.backlightOff();
        sleep(5 * 1000);
    }

    @Override
    public void doTest() {
        startTestMode(TEST_STATE_OFF);
        writeFiles();
        stopTestMode(TEST_STATE_OFF);
        Util.backlightOn();
        Util.lcmOn();
    }

    private void writeFiles() {
        long start = System.currentTimeMillis();
        while (System.currentTimeMillis() - start < 30 * 1000 && !isCancelled() && !isTestDone()) {
            mSdTest.doTest(SdTest.TYPE_WRITE, SdTest.STORAGE_INTERNAL, this);
        }
    }

    @Override
    public String toString() {
        return "Phone storage writing";
    }

    @Override
    protected boolean doApiTest() {
        // TODO Auto-generated method stub
        SdTest sdTest = new SdTest(mContext);
        if (sdTest.doApiTest(SdTest.STORAGE_INTERNAL)) {
            return true;
        } else {
            return false;
        }
    }
}

/**
 * TestMode.
 */
class ExternalSdCardReadTest extends TestItem {
    private SdTest mSdTest;

    @Override
    public void init() {
        mSdTest = new SdTest(mContext);
        mSdTest.doTest(SdTest.TYPE_PREPARE, SdTest.STORAGE_SDCARD, this);
        Util.enterFlightMode(mContext);
        Util.lcmOff();
        Util.backlightOff();
        sleep(5 * 1000);
    }

    @Override
    public void doTest() {
        startTestMode(TEST_STATE_OFF);
        readFiles();
        stopTestMode(TEST_STATE_OFF);
        Util.backlightOn();
        Util.lcmOn();
    }

    private void readFiles() {
        long start = System.currentTimeMillis();
        while (System.currentTimeMillis() - start < 30 * 1000 && !isCancelled()) {
            mSdTest.doTest(SdTest.TYPE_READ, SdTest.STORAGE_SDCARD, this);
        }
    }

    @Override
    public String toString() {
        return "External SD card reading";
    }

    @Override
    protected boolean doApiTest() {
        // TODO Auto-generated method stub
        SdTest sdTest = new SdTest(mContext);
        if (sdTest.doApiTest(SdTest.STORAGE_SDCARD)) {
            return true;
        } else {
            return false;
        }
    }
}

/**
 * TestMode.
 */
class ExternalSdCardWriteTest extends TestItem {
    private SdTest mSdTest;

    @Override
    public void init() {
        mSdTest = new SdTest(mContext);
        mSdTest.doTest(SdTest.TYPE_PREPARE, SdTest.STORAGE_SDCARD, this);
        Util.enterFlightMode(mContext);
        Util.lcmOff();
        Util.backlightOff();
        sleep(5 * 1000);
    }

    @Override
    public void doTest() {
        startTestMode(TEST_STATE_OFF);
        writeFiles();
        stopTestMode(TEST_STATE_OFF);
        Util.backlightOn();
        Util.lcmOn();
    }

    private void writeFiles() {
        long start = System.currentTimeMillis();
        while (System.currentTimeMillis() - start < 30 * 1000 && !isCancelled()) {
            mSdTest.doTest(SdTest.TYPE_WRITE, SdTest.STORAGE_SDCARD, this);
        }
    }

    @Override
    public String toString() {
        return "External SD card writing";
    }

    @Override
    protected boolean doApiTest() {
        // TODO Auto-generated method stub
        SdTest sdTest = new SdTest(mContext);
        if (sdTest.doApiTest(SdTest.STORAGE_SDCARD)) {
            return true;
        } else {
            return false;
        }
    }
}

/**
 * TestMode.
 */
class MemoryAccessTest extends TestItem {
    @Override
    public void init() {
        Util.enterFlightMode(mContext);
        Util.lcmOff();
        Util.backlightOff();
        sleep(5 * 1000);
    }

    @Override
    public void doTest() {
        startTestMode(TEST_STATE_OFF);
        Random rand = new Random();
        int[] test = new int[100000];
        long start = System.currentTimeMillis();
        while (System.currentTimeMillis() - start < 30 * 1000 && !isCancelled() && !isTestDone()) {
            test[rand.nextInt(100000)] = rand.nextInt();
        }
        stopTestMode(TEST_STATE_OFF);
        Util.backlightOn();
        Util.lcmOn();
    }

    @Override
    public String toString() {
        return "Memory accessing";
    }

    @Override
    protected boolean doApiTest() {
        // TODO Auto-generated method stub
        return true;
    }
}

/**
 * TestMode.
 */
class FrontCameraPreviewTest extends TestItem {
    @Override
    public void init() {
        Util.enterFlightMode(mContext);
        sleep(5 * 1000);
    }

    @Override
    public void doTest() {
        Intent intent = new Intent(mContext, CameraTest.class);
        intent.putExtra("facing", 1);
        ((Activity) mContext).startActivity(intent);
        startTestMode(TEST_STATE_OFF);
//        sleep(30 * 1000);
        stopTestMode(TEST_STATE_OFF);
        Util.pressBack();
    }

    @Override
    public String toString() {
        return "Front camera preview";
    }

    @Override
    protected boolean doApiTest() {
        // TODO Auto-generated method stub
        Intent intent = new Intent(mContext, CameraTest.class);
        intent.putExtra("facing", 1);
        return (Util.isIntentAvailable(mContext, intent));
    }
}

/**
 * TestMode.
 */
class BackCameraPreviewTest extends TestItem {
    @Override
    public void init() {
        Util.enterFlightMode(mContext);
        sleep(5 * 1000);
    }

    @Override
    public void doTest() {
        Intent intent = new Intent(mContext, CameraTest.class);
        intent.putExtra("facing", 0);
        ((Activity) mContext).startActivity(intent);
        startTestMode(TEST_STATE_OFF);
//        sleep(30 * 1000);
        stopTestMode(TEST_STATE_OFF);
        Util.pressBack();
    }

    @Override
    public String toString() {
        return "Back camera preview";
    }

    @Override
    protected boolean doApiTest() {
        Intent intent = new Intent(mContext, CameraTest.class);
        intent.putExtra("facing", 0);
        return (Util.isIntentAvailable(mContext, intent));

    }

}

/**
 * TestMode.
 */
class Display3DTest extends TestItem {
    private Point mBtnRunPoint = new Point(295, 478);
    @Override
    public void init() {
        Util.enterFlightMode(mContext);
        Util.lcmOn();
        sleep(5 * 1000);
    }

    @Override
    public void doTest() {
        Intent intent = new Intent();
        intent.setClassName("se.nena.nenamark2", "se.nena.nenamark2.NenaMark2");
        try {
            mContext.startActivity(intent);
            sleep(1000);
            Util.clickPoint(mBtnRunPoint, true);
            sleep(500);
            startTestMode(TEST_STATE_OFF);
            stopTestMode(TEST_STATE_OFF);
            sleep(30000);
            Util.pressBack();
            sleep(500);
            Util.pressBack();
            Util.enterFlightMode(mContext);
        } catch (ActivityNotFoundException e) {
            Elog.d(TAG, "ActivityNotFoundException:" + e.getMessage());
        }
        Util.backlightOn();
        Util.lcmOn();
    }

    @Override
    public String toString() {
        return "3D display";
    }

    @Override
    protected boolean doApiTest() {
        // TODO Auto-generated method stub

        Intent intent = new Intent();
        intent.setClassName("se.nena.nenamark2", "se.nena.nenamark2.NenaMark2");
        return (Util.isIntentAvailable(mContext, intent));
    }
}


/**
 * Virtual class to indicate all items for this modem category.
 *
 */
class BandVirtualTest extends TestItem {

    @Override
    public String toString() {
        return null;
    }

    @Override
    protected boolean doApiTest() {
        // TODO Auto-generated method stub
        return true;
    }

    @Override
    protected void init() {
        // TODO Auto-generated method stub
    }

    @Override
    protected void doTest() {
        // TODO Auto-generated method stub

    }
}

/**
 * Virtual class to indicate all items for GSM band.
 *
 */
class GsmBandVirtualTest extends BandVirtualTest {
    public static final String ITEM_TAG = "GsmBandTest";

    @Override
    public String toString() {
        return ITEM_TAG;
    }
}

/**
 * Virtual class to indicate all items for FDD band.
 *
 */
class FddBandVirtualTest extends BandVirtualTest {
    public static final String ITEM_TAG = "FddBandTest";
    @Override
    public String toString() {
        return ITEM_TAG;
    }
}

/**
 * Virtual class to indicate all items for TDD band.
 *
 */
class TddBandVirtualTest extends BandVirtualTest {
    public static final String ITEM_TAG = "TddBandTest";
    @Override
    public String toString() {
        return ITEM_TAG;
    }
}

/**
 * Virtual class to indicate all items for LTE band.
 *
 */
class LteBandVirtualTest extends BandVirtualTest {
    public static final String ITEM_TAG = "LteBandTest";
    @Override
    public String toString() {
        return ITEM_TAG;
    }
}

/**
 * Virtual class to indicate all items for CDMA band.
 *
 */
class CdmaBandVirtualTest extends BandVirtualTest {
    public static final String ITEM_TAG = "CdmaBandTest";
    @Override
    public String toString() {
        return ITEM_TAG;
    }
}

/**
 * Class for modem related test.
 *
 */
class BandTest extends TestItem {
    protected int mChannel;
    protected int mPower;
    protected int mBandValue;
    protected String mBandName;
    protected static final String CHANNEL = "CH:";
    protected static final String POWER = "PWR:";
    public static final String ITEM_TAG_PRE = "(";
    public static final String ITEM_TAG_POST = ")";
    public static final String ITEM_TAG_NA = "NA";

    protected void startDesense() {

    }

    BandTest(String name, int channel, int power, Context context) {
        mBandName = name;
        mChannel = channel;
        mPower = power;
        setContext(context);
    }
    @Override
    public void init() {
        Util.enterFlightMode(mContext);
        Util.lcmOff();
        Util.backlightOff();
        sleep(5 * 1000);
    }

    @Override
    public void doTest() {
        Util.leaveFlightMode(mContext);
        startDesense();
        startTestMode(TEST_STATE_OFF);
        stopTestMode(TEST_STATE_OFF);
        Util.lcmOn();
        Util.backlightOn();

    }

    @Override
    public String toString() {
        return null;
    }

    @Override
    protected boolean doApiTest() {
        // TODO Auto-generated method stub
        return true;
    }
}

/**
 * Class for GSM band related test.
 *
 */
class GsmBandTest extends BandTest {

    private static final int AFC = 4100;
    private static final int TSC = 0;
    private static final int PATTERN = 0;
    public static final String ITEM_TAG_PRE = "Gsm(";
    protected void startDesense() {
        String command = "AT+ERFTX=2,1," + mChannel + "," + AFC + "," + mBandValue + ","
                + TSC + "," + mPower + "," + PATTERN;
        Util.sendAtCommand(command, "", -1);
    }

    GsmBandTest(String name, int channel, int power, Context context) {
        super(name, channel, power, context);
    }

    @Override
    public String toString() {
        return mBandName + ITEM_TAG_PRE + CHANNEL + ((mChannel != -1) ? mChannel : ITEM_TAG_NA)
                + POWER + ((mPower != -1) ? mPower : ITEM_TAG_NA) + ITEM_TAG_POST;
    }


    @Override
    protected boolean doApiTest() {
        // TODO Auto-generated method stub
        return true;
    }
}

/**
 * Class for FDD/TDD band related test.
 *
 */
class FddTddBandTest extends BandTest {

    public static final String ITEM_TAG = "FddTddBandTest";
    protected void startDesense() {
        String command = "AT+ERFTX=0,0," + mBandValue + "," + mChannel + "," + mPower;

        Util.sendAtCommand(command, "", -1);
    }

    FddTddBandTest(String name, int channel, int power, Context context) {
        super(name, channel, power, context);
    }

    @Override
    protected boolean doApiTest() {
        // TODO Auto-generated method stub
        return true;
    }
}

/**
 * Class for FDD band related test.
 *
 */
class FddBandTest extends FddTddBandTest {

    FddBandTest(String name, int channel, int power, Context context) {
        super(name, channel, power, context);
    }
    @Override
    public String toString() {
        return mBandName + ITEM_TAG_PRE + CHANNEL + ((mChannel != -1) ? mChannel : ITEM_TAG_NA)
                + POWER + ((mPower != -1) ? mPower : ITEM_TAG_NA) + ITEM_TAG_POST;
    }


}

/**
 * Class for TDD band related test.
 *
 */
class TddBandTest extends FddTddBandTest {

    TddBandTest(String name, int channel, int power, Context context) {
        super(name, channel, power, context);
    }

    @Override
    public String toString() {
        return mBandName + ITEM_TAG_PRE  + CHANNEL + ((mChannel != -1) ? mChannel : ITEM_TAG_NA)
                + POWER + ((mPower != -1) ? mPower : ITEM_TAG_NA) + ITEM_TAG_POST;
    }
}

/**
 * Class for LTE  band related test.
 *
 */
class LteBandTest extends BandTest {

    private static final int FDD_BAND_MIN = 1;
    private static final int DUPLEX_FDD = 1;
    private static final int BAND_WIDTH_INDEX  = 0;
    private static final int TDD_CONFIG_INDEX = 0;
    private static final int TDD_SP_SF_CONFIG_INDEX = 0;
    private static final String DEFAULT_VRB_START = "0";
    private static final String DEFAULT_VRB_LENGTH = "1";
    private static final int DEFAULT_MCS = 0;
    public static final String UL_FREQ = "UL Freq";
/*    private static final String[] FREQ_RANGE = {
        "19200,19800",
        "18500,19100",
        "17100,17850",
        "17100,17550",
        "8240,8490",
        "8300,8400",
        "25000,25700",
        "8800,9150",
        "17499,17849",
        "17100,17700",
        "14279,14479",
        "6990,7160",
        "7770,7870",
        "7880,7980",
        "0,0",
        "0,0",
        "7040,7160",
        "8150,8300",
        "8300,8450",
        "8320,8620",
        "14479,14629",
        "34100,34900",
        "20000,20200",
        "16265,16605",
        "18500,19150",
        "8140,8490",
        "8070,8240",
        "7030,7480",
        "0,0",
        "23050,23150",
        "4525,4575",
        "0,0",
        "19000,19200",
        "20100,20250",
        "18500,19100",
        "19300,19900",
        "19100,19300",
        "25700,26200",
        "18800,19200",
        "23000,24000",
        "24960,26900",
        "34000,36000",
        "36000,38000",
        "7030,8030",
    };

    private int getDefaultFreq() {
        int band = mBandValue+FDD_BAND_MIN;
        String[] range = FREQ_RANGE[band - 1].split(",");
        try {
            int min = Integer.parseInt(range[0]);
            int max = Integer.parseInt(range[1]);
            return (min + max) / 2;
        } catch (NumberFormatException e) {
            throw new RuntimeException("Check the array resource");
        }
    }*/
    protected void startDesense() {

        String command = "AT+ERFTX=6,0,1,"
                + mBandValue
                + FDD_BAND_MIN + ","
                + BAND_WIDTH_INDEX + ","
                + mChannel + ","
                + DUPLEX_FDD + ","
                + TDD_CONFIG_INDEX + ","
                + TDD_SP_SF_CONFIG_INDEX + ","
                + DEFAULT_VRB_START + ","
                + DEFAULT_VRB_LENGTH + ","
                + DEFAULT_MCS + ","
                + mPower;
        Util.sendAtCommand(command, "", -1);
    }

    LteBandTest(String name, int channel, int power, Context context) {
        super(name, channel, power, context);
    }

    @Override
    public String toString() {
        return mBandName + ITEM_TAG_PRE  + UL_FREQ + ((mChannel != -1) ? mChannel : ITEM_TAG_NA)
                + POWER + ((mPower != -1) ? mPower : ITEM_TAG_NA) + ITEM_TAG_POST;
    }

    @Override
    protected boolean doApiTest() {
        // TODO Auto-generated method stub
        return true;
    }

}

/**
 * Class for CDMA band related test.
 *
 */
class CdmaBandTest extends BandTest {
    private static final int MODULATION = 0;

    protected void startDesense() {
        String command = "AT+ECRFTX=1," + mChannel + "," + mBandValue + "," + mPower + ","
                + MODULATION;

        Util.sendAtCommand(command, "", -1);
    }

    CdmaBandTest(String name, int channel, int power, Context context) {
        super(name, channel, power, context);
    }

    @Override
    public String toString() {
        return mBandName + ITEM_TAG_PRE  + CHANNEL + ((mChannel != -1) ? mChannel : ITEM_TAG_NA)
                + POWER + ((mPower != -1) ? mPower : ITEM_TAG_NA) + ITEM_TAG_POST;
    }


    @Override
    protected boolean doApiTest() {
        // TODO Auto-generated method stub
        return true;
    }
}