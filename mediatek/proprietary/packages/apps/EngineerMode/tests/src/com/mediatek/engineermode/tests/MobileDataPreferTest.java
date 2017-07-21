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
import android.test.ActivityInstrumentationTestCase2;

import com.jayway.android.robotium.solo.Solo;
import com.mediatek.engineermode.MobileDataPreferred;

public class MobileDataPreferTest extends ActivityInstrumentationTestCase2<MobileDataPreferred> {
    private static final String TAG = "MobileDataPreferred";
    private static Instrumentation sInst = null;
    private static Solo sSolo = null;
    private static Activity sActivity = null;
    private static Context sContext = null;

    public MobileDataPreferTest() {
        super(MobileDataPreferred.class);
    }

    protected void setUp() throws Exception {
        super.setUp();
        sInst = getInstrumentation();
        sContext = sInst.getTargetContext();
        sActivity = getActivity();
        sSolo = new Solo(sInst, sActivity);

    }

    @BasicFuncAnnotation
    public void test01_Prerequisite() {
        assertNotNull(sInst);
        assertNotNull(sContext);
        assertNotNull(sActivity);
        assertNotNull(sSolo);
    }

    @UiAnnotation
    public void test02_SetTest() {
        Activity testActivity = sSolo.getCurrentActivity();
        sSolo.assertCurrentActivity("Not MobileDataPreferred Class", MobileDataPreferred.class);

        sSolo.sleep(EmOperate.TIME_LONG);
        if (sSolo.isCheckBoxChecked(0)) {
            sSolo.clickOnCheckBox(0);
            sSolo.sleep(EmOperate.TIME_LONG);
        }

        // check the CheckBox
        sSolo.clickOnCheckBox(0);
        sSolo.sleep(EmOperate.TIME_SUPER_LONG);

        // dialog cancel
        sSolo.clickOnButton(0);

        // check the CheckBox
        sSolo.clickOnCheckBox(0);
        sSolo.sleep(EmOperate.TIME_SUPER_LONG);


        // ok the CheckBox
        sSolo.clickOnButton(1);
        sSolo.sleep(EmOperate.TIME_SUPER_LONG);

        sSolo.sleep(EmOperate.TIME_SUPER_LONG);

        if (sSolo.isCheckBoxChecked(0)) {
            sSolo.clickOnCheckBox(0);
            sSolo.sleep(EmOperate.TIME_LONG);
        }

        sSolo.sleep(EmOperate.TIME_LONG);
        sSolo.goBack();
    }

    protected void tearDown() throws Exception {
        super.tearDown();
    }

}
