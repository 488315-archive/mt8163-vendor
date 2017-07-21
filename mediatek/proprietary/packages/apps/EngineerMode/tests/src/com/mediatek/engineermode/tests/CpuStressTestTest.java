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

package com.mediatek.engineermode.tests;

import android.app.Activity;
import android.app.Instrumentation;
import android.content.Context;
import android.test.SingleLaunchActivityTestCase;
import android.text.TextUtils;
import android.util.Log;
import android.view.View;
import android.widget.CheckBox;
import android.widget.ListView;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;

import com.jayway.android.robotium.solo.Solo;
import com.mediatek.engineermode.cpustress.CpuStressTest;
import com.mediatek.engineermode.cpustress.CpuStressTestService;
import com.mediatek.engineermode.R;

import java.io.File;
import java.util.ArrayList;

public class CpuStressTestTest extends
        SingleLaunchActivityTestCase<CpuStressTest> {

    private static final String TAG = "EMTest/CpustressTest";
    private static final String FILE_THERMAL_ETC = "/etc/.tp/.ht120.mtc";
    private static final int ITEM_TEST_COUNT = 4;
    private static final String WAIT = "Waiting";
    private static final String FILE_DHRY_0 =
        "/sys/bus/platform/drivers/slt_cpu0_dhry/slt_cpu0_dhry";
    private static Solo sSolo = null;
    private static Activity sActivity = null;
    private static Context sContext = null;
    private static Instrumentation sInst = null;
    private static ListView sListView = null;
    private static boolean sHaveThermalEtc = false;
    private static boolean sInit = false;
    private static boolean sFinished = false;
    private static int sTestRadioOptions = 0;
    private static boolean sHasCustomMode = false;
    private static boolean sSupportStressTest = false;

    public CpuStressTestTest() {
        super("com.mediatek.engineermode", CpuStressTest.class);
    }

    @Override
    protected void setUp() throws Exception {
        super.setUp();
        sInit = true;
        if (null == sInst) {
            sInst = getInstrumentation();
        }
        if (null == sContext) {
            sContext = sInst.getTargetContext();
        }
        if (null == sActivity) {
            sActivity = getActivity();
            if (sActivity.getClass() != CpuStressTest.class) {
                sActivity.finish();
                sActivity = launchActivity("com.mediatek.engineermode",
                        CpuStressTest.class, null);
            }
        }
        if (null == sSolo) {
            sSolo = new Solo(sInst, sActivity);
        }
        sHaveThermalEtc = new File(FILE_THERMAL_ETC).exists();
        sSupportStressTest = new File(FILE_DHRY_0).exists();
    }

    @Override
    protected void tearDown() throws Exception {
        if (sFinished) {
            sSolo.finishOpenedActivities();
        }
        super.tearDown();
    }

    @UiAnnotation
    public void test01_Precondition() {
        assertNotNull(sInst);
        assertNotNull(sContext);
        assertNotNull(sActivity);
        assertNotNull(sSolo);
        if (null == sListView) {
            sListView = (ListView) sActivity
                    .findViewById(R.id.listview_hqa_cpu_main);
        }
        assertNotNull(sListView);
        sHasCustomMode = sSolo.searchText("Custom mode");
    }

    @UiAnnotation
    public void test02_TestList() {
        int itemCount = sListView.getAdapter().getCount();
        //assertEquals(ITEM_TEST_COUNT, itemCount);
        sSolo.clickOnText(sListView.getAdapter().getItem(0).toString());
        sSolo.sleep(EmOperate.TIME_MID);
    }

    @UiAnnotation
    public void test03_TestRadio() {
        if (!sSupportStressTest) {
            return;
        }
        if (sHasCustomMode) {
            sSolo.clickOnRadioButton(1);
            sSolo.sleep(200);
            sSolo.clickOnRadioButton(2);
            sSolo.sleep(200);
            sSolo.clickOnRadioButton(0);
            return;
        }
        RadioGroup rg = (RadioGroup) sActivity
                .findViewById(R.id.hqa_cpu_main_radiogroup);
        int count = rg.getChildCount();
        int coreNum = CpuStressTestService.sCoreNumber;
        int targetRadioOptions = coreNum + 2; // count test mode & default;
        if (coreNum >= 8) {
            targetRadioOptions = 7;
        }
        sTestRadioOptions = 0;
        for (int i = 0; i < count; i++) {
            View v = rg.getChildAt(i);
            if (v.isShown()) {
                sTestRadioOptions++;
            }
        }
        assertEquals(targetRadioOptions, sTestRadioOptions);
        for (int i = sTestRadioOptions - 1; i >= 0; i--) {
            sSolo.clickOnRadioButton(i);
            sSolo.sleep(300);
        }
    }

    @UiAnnotation
    public void test04_TestThermal() {
        if (!sSupportStressTest) {
            return;
        }
        CheckBox cbThermal = (CheckBox) sActivity
                .findViewById(R.id.hqa_cpu_main_checkbox);
        assertEquals(sHaveThermalEtc, cbThermal.isEnabled());
        if (sHaveThermalEtc) {
            sSolo.clickOnCheckBox(0);
            sSolo.sleep(EmOperate.TIME_LONG);
            sSolo.clickOnCheckBox(0);
            sSolo.sleep(EmOperate.TIME_LONG);
            sSolo.clickOnCheckBox(0);
        }
    }

    @UiAnnotation
    public void test05_TestRuntime() {
        if (!sSupportStressTest) {
            return;
        }
        sSolo.clickOnRadioButton(1);
        sSolo.clickOnText(sListView.getAdapter().getItem(0).toString());
        sSolo.sleep(EmOperate.TIME_LONG);
        sSolo.clickOnButton("Start");
        sSolo.goBack();
        sSolo.sleep(EmOperate.TIME_LONG);  // wait radio button status changed
        RadioGroup rg = (RadioGroup) sActivity
                .findViewById(R.id.hqa_cpu_main_radiogroup);
        int count = rg.getChildCount();
        for (int i = 0; i < count; i++) {
            RadioButton rb = (RadioButton) rg.getChildAt(i);
            if (rb.isShown()) {
                assertFalse(rb.isEnabled());
            }
        }
        CheckBox cbThermal = (CheckBox) sActivity
                .findViewById(R.id.hqa_cpu_main_checkbox);
        assertFalse(cbThermal.isEnabled());
    }

    @UiAnnotation
    public void test06_TestStop() {
        if (!sSupportStressTest) {
            return;
        }
        sSolo.clickOnText(sListView.getAdapter().getItem(0).toString());
        sSolo.sleep(EmOperate.TIME_LONG);
        assertTrue(sSolo.searchText("Stop"));
        sSolo.clickOnButton("Stop");
        sSolo.sleep(EmOperate.TIME_LONG);
        //sSolo.goBack();  // hide virtual keyboard
        //sSolo.goBack(); // go back
        sSolo.goBackToActivity(CpuStressTest.class.getSimpleName());
        sSolo.sleep(EmOperate.TIME_LONG);
        RadioGroup rg = (RadioGroup) sActivity
                .findViewById(R.id.hqa_cpu_main_radiogroup);
        int count = rg.getChildCount();
        for (int i = 0; i < count; i++) {
            RadioButton rb = (RadioButton) rg.getChildAt(i);
            assertTrue(rb.isEnabled());
        }
        CheckBox cbThermal = (CheckBox) sActivity
                .findViewById(R.id.hqa_cpu_main_checkbox);
        assertTrue(cbThermal.isEnabled());
        sSolo.clickOnText(sListView.getAdapter().getItem(2).toString());
        sSolo.clickOnRadioButton(0);
    }

    private String getTextContent(int resId) {
        Activity activity = sSolo.getCurrentActivity();
        TextView tv = (TextView) activity.findViewById(resId);
        return tv.getText().toString();
    }

    @UiAnnotation
    public void test07_ApMcuSingle() {
        if (!sSupportStressTest) {
            return;
        }
        if (sHasCustomMode) {
            return;
        }
        sSolo.clickOnRadioButton(2);
        sSolo.clickOnText(sListView.getAdapter().getItem(0).toString());
        sSolo.sleep(EmOperate.TIME_LONG);
        for (int i = 0; i < 6; i++) {
            sSolo.clickOnCheckBox(i);
        }
        sSolo.clickOnButton(6);
        waitForTestLoopUpdate(0);
        assertFalse(TextUtils.isEmpty(getTextContent(R.id.apmcu_neon_result_0)));
        assertTrue(TextUtils.isEmpty(getTextContent(R.id.apmcu_neon_result_1)));
        assertFalse(TextUtils.isEmpty(getTextContent(R.id.apmcu_ca9_result_0)));
        assertTrue(TextUtils.isEmpty(getTextContent(R.id.apmcu_ca9_result_1)));
        assertFalse(TextUtils.isEmpty(getTextContent(R.id.apmcu_result)));
        sSolo.clickOnButton(6);
        while (sSolo.searchText(WAIT)) {
            sSolo.sleep(EmOperate.TIME_MID);
        }
        sSolo.goBack();
    }

    @UiAnnotation
    public void test08_SwCodecSingle() {
        if (!sSupportStressTest) {
            return;
        }
        if (sHasCustomMode) {
            return;
        }
        sSolo.clickOnText(sListView.getAdapter().getItem(1).toString());
        sSolo.sleep(EmOperate.TIME_LONG);
        sSolo.clickOnButton(0);
        sSolo.sleep(EmOperate.TIME_SUPER_LONG);
        assertFalse(TextUtils.isEmpty(getTextContent(R.id.swvideo_iteration_result)));
        assertTrue(TextUtils.isEmpty(getTextContent(R.id.swvideo_iteration_result_1)));
        assertFalse(TextUtils.isEmpty(getTextContent(R.id.swvideo_result)));
        sSolo.clickOnButton(0);
        sSolo.sleep(EmOperate.TIME_SUPER_LONG);
        sSolo.goBack();
    }

    private void waitForTestLoopUpdate(int editIndex) {
        String origText = sSolo.getEditText(editIndex).getText().toString();
        Log.d("@M_" + TAG, "Loop Count:" + origText);
        String currText = origText;
        while (origText.equals(currText)) {
            sSolo.sleep(EmOperate.TIME_SHORT);
            currText = sSolo.getEditText(editIndex).getText().toString();
        }
    }

    private void setCustomModeDefaultOn() {
        sSolo.clickOnRadioButton(2);
        sSolo.sleep(100);
        sSolo.clickOnButton("Ok");
    }

    private void customModeApmcuTest() {
//        setCustomModeDefaultOn();
//        sSolo.clickOnText(sListView.getAdapter().getItem(0).toString());
//        sSolo.sleep(EmOperate.TIME_LONG);
        ArrayList<CheckBox> checkBoxs = sSolo.getCurrentViews(CheckBox.class);
        assertTrue(checkBoxs != null);
        assertTrue(checkBoxs.size() > 0);
        for (int i = 0; i < checkBoxs.size(); i++) {
            CheckBox cb = checkBoxs.get(i);
            if (!cb.isShown()) {
                continue;
            }
            String text = cb.getText().toString();
            sSolo.clickOnView(cb);
        }
        // to check the unshown test item
        while (sSolo.scrollDown()) { };
        checkBoxs = sSolo.getCurrentViews(CheckBox.class);
        for (int i = 0; i < checkBoxs.size(); i++) {
            CheckBox cb = checkBoxs.get(i);
            if (!cb.isShown()) {
                continue;
            }
            if (!cb.isChecked()) {
                sSolo.clickOnView(cb);
            }
        }
        sSolo.clickOnButton("Start");
        waitForTestLoopUpdate(0);
        assertFalse(sSolo.searchText("FAIL"));
        assertTrue(sSolo.searchText("Stop"));
        sSolo.clickOnButton("Stop");
        while (sSolo.searchText(WAIT)) {
            sSolo.sleep(EmOperate.TIME_MID);
        }
        while (sSolo.scrollUp()) { };
        checkBoxs = sSolo.getCurrentViews(CheckBox.class);
        for (int i = 0; i < checkBoxs.size(); i++) {
            CheckBox cb = checkBoxs.get(i);
            if (!cb.isShown()) {
                continue;
            }
            String text = cb.getText().toString();
            sSolo.clickOnView(cb);
        }
        // to uncheck the unshown test item
        while (sSolo.scrollDown()) { };
        checkBoxs = sSolo.getCurrentViews(CheckBox.class);
        for (int i = 0; i < checkBoxs.size(); i++) {
            CheckBox cb = checkBoxs.get(i);
            if (!cb.isShown()) {
                continue;
            }
            if (cb.isChecked()) {
                sSolo.clickOnView(cb);
            }
        }
        assertFalse(TextUtils.isEmpty(getTextContent(R.id.apmcu_result)));
        sSolo.goBack();
        sSolo.waitForActivity(CpuStressTest.class.getSimpleName());
        sSolo.clickOnRadioButton(0);
    }

    private void customModeSwVideoTest() {
//        setCustomModeDefaultOn();
        sSolo.clickOnText(sListView.getAdapter().getItem(1).toString());
        sSolo.sleep(EmOperate.TIME_LONG);
        sSolo.clickOnButton("Start");
        waitForTestLoopUpdate(0);
        assertTrue(sSolo.searchText("Stop"));
        sSolo.clickOnButton("Stop");
        while (sSolo.searchText(WAIT)) {
            sSolo.sleep(EmOperate.TIME_MID);
        }
        assertFalse(TextUtils.isEmpty(getTextContent(R.id.swvideo_result)));
        sSolo.goBack();
        sSolo.waitForActivity(CpuStressTest.class.getSimpleName());
    }

    @UiAnnotation
    public void test09_ApMcuQuad() {
        if (!sSupportStressTest) {
            return;
        }
        int coreNum = CpuStressTestService.sCoreNumber;
        if (coreNum < 4) {
            return;
        }

        sSolo.clickOnRadioButton(5);
        sSolo.clickOnText(sListView.getAdapter().getItem(0).toString());
        sSolo.sleep(EmOperate.TIME_LONG);
        Log.d("CpuStressTestTest", "sHasCustomMode = " + sHasCustomMode);
        if (sHasCustomMode) {
            ArrayList<CheckBox> checkBoxs = sSolo.getCurrentViews(CheckBox.class);
            assertTrue(checkBoxs != null);
            assertTrue(checkBoxs.size() > 0);
            for (int i = 0; i < checkBoxs.size(); i++) {
                CheckBox cb = checkBoxs.get(i);
                if (!cb.isShown()) {
                    continue;
                }
                String text = cb.getText().toString();
                sSolo.clickOnView(cb);
            }
            // to check the unshown test item
            while (sSolo.scrollDown()) { };
            checkBoxs = sSolo.getCurrentViews(CheckBox.class);
            for (int i = 0; i < checkBoxs.size(); i++) {
                CheckBox cb = checkBoxs.get(i);
                if (!cb.isShown()) {
                    continue;
                }
                if (!cb.isChecked()) {
                    sSolo.clickOnView(cb);
                }
            }

        }
        sSolo.clickOnButton("Start");
        waitForTestLoopUpdate(0);
        int[] showResultIds = {R.id.apmcu_neon_result_0,
                R.id.apmcu_neon_result_1,
                R.id.apmcu_neon_result_2,
                R.id.apmcu_neon_result_3};
        int[] emptyResultIds = {R.id.apmcu_neon_result_4,
                R.id.apmcu_neon_result_5,
                R.id.apmcu_neon_result_6,
                R.id.apmcu_neon_result_7};
        for (int i = 0; i < showResultIds.length; i++) {
            assertFalse(TextUtils.isEmpty(getTextContent(showResultIds[i])));
        }
        for (int i = 0; i < emptyResultIds.length; i++) {
            assertTrue(TextUtils.isEmpty(getTextContent(emptyResultIds[i])));
        }
        assertFalse(TextUtils.isEmpty(getTextContent(R.id.apmcu_result)));
        assertTrue(sSolo.searchText("Stop"));
        sSolo.clickOnButton("Stop");
        while (sSolo.searchText(WAIT)) {
            sSolo.sleep(EmOperate.TIME_MID);
        }
        sSolo.goBack();
    }

/*    public void test10_SwCodecDual() {
        if (!sSupportStressTest) {
            return;
        }
        int coreNum = CpuStressTestService.sCoreNumber;
        if (coreNum < 2) {
            return;
        }
        if (sHasCustomMode) {
            customModeSwVideoTest();
            return;
        }
        sSolo.clickOnRadioButton(3);
        sSolo.sleep(EmOperate.TIME_MID);
        sSolo.clickOnText(sListView.getAdapter().getItem(1).toString());
        sSolo.sleep(EmOperate.TIME_LONG);
        sSolo.clickOnButton(0);
        sSolo.sleep(EmOperate.TIME_SUPER_LONG);
        int[] showResultIds = {R.id.swvideo_iteration_result,
                R.id.swvideo_iteration_result_1};
        for (int i = 0; i < showResultIds.length; i++) {
            assertFalse(TextUtils.isEmpty(getTextContent(showResultIds[i])));
        }
        int[] emptyResultIds = {R.id.swvideo_iteration_result_2,
                R.id.swvideo_iteration_result_3,
                R.id.swvideo_iteration_result_4,
                R.id.swvideo_iteration_result_5,
                R.id.swvideo_iteration_result_6,
                R.id.swvideo_iteration_result_7};
        for (int i = 0; i < emptyResultIds.length; i++) {
            assertTrue(TextUtils.isEmpty(getTextContent(emptyResultIds[i])));
        }
        assertFalse(TextUtils.isEmpty(getTextContent(R.id.swvideo_result)));
        sSolo.clickOnButton(0);
        while (sSolo.searchText(WAIT)) {
            sSolo.sleep(EmOperate.TIME_MID);
        }
        sSolo.goBack();
    }*/

    @UiAnnotation
    public void test12_Help() {
        sSolo.clickOnText(sListView.getAdapter().getItem(2).toString());
        sSolo.sleep(EmOperate.TIME_LONG);
        sSolo.goBack();
    }

    public void test13_TestRestore() {
        if (!sSupportStressTest) {
            return;
        }
        sFinished = true;
        sSolo.clickOnRadioButton(0);
        if (sHaveThermalEtc) {
            sSolo.clickOnCheckBox(0);
        }
    }
}
