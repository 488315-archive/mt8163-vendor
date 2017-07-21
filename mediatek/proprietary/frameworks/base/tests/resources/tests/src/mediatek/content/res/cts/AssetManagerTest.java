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

/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package mediatek.content.res.cts;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;

import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.content.res.Resources;
import android.content.res.XmlResourceParser;
import android.test.AndroidTestCase;
import android.util.TypedValue;

import com.mediatek.cts.resource.stub.R;
import com.android.internal.util.XmlUtils;


public class AssetManagerTest extends AndroidTestCase {
    private AssetManager mAssets;

    @Override
    protected void setUp() throws Exception {
        super.setUp();
        mAssets = mContext.getAssets();
    }

    public void testAssetOperations() throws IOException, XmlPullParserException {
        final Resources res = getContext().getResources();
        final TypedValue value = new TypedValue();
        res.getValue(R.raw.text, value, true);
        final String fileName = "text.txt";
        InputStream inputStream = mAssets.open(fileName);
        assertNotNull(inputStream);
        final String expect = "OneTwoThreeFourFiveSixSevenEightNineTen";
        assertContextEquals(expect, inputStream);
        inputStream = mAssets.open(fileName, AssetManager.ACCESS_BUFFER);
        assertNotNull(inputStream);
        assertContextEquals(expect, inputStream);

        AssetFileDescriptor assetFileDes = mAssets.openFd(fileName);
        assertNotNull(assetFileDes);
        assertContextEquals(expect, assetFileDes.createInputStream());
        assetFileDes = mAssets.openNonAssetFd(value.string.toString());
        assertNotNull(assetFileDes);
        assertContextEquals(expect, assetFileDes.createInputStream());
        assetFileDes = mAssets.openNonAssetFd(value.assetCookie, value.string.toString());
        assertNotNull(assetFileDes);
        assertContextEquals(expect, assetFileDes.createInputStream());

        XmlResourceParser parser = mAssets.openXmlResourceParser("AndroidManifest.xml");
        assertNotNull(parser);
        XmlUtils.beginDocument(parser, "manifest");
        parser = mAssets.openXmlResourceParser(0, "AndroidManifest.xml");
        assertNotNull(parser);
        beginDocument(parser, "manifest");

        String[] files = mAssets.list("");
        boolean result = false;
        for (int i = 0; i < files.length; i++) {
            if (files[i].equals(fileName)) {
                result = true;
                break;
            }
        }
        assertTrue(result);

        try {
            mAssets.open("notExistFile.txt", AssetManager.ACCESS_BUFFER);
            fail("test open(String, int) failed");
        } catch (IOException e) {
            // expected
        }

        try {
            mAssets.openFd("notExistFile.txt");
            fail("test openFd(String) failed");
        } catch (IOException e) {
            // expected
        }

        try {
            mAssets.openNonAssetFd(0, "notExistFile.txt");
            fail("test openNonAssetFd(int, String) failed");
        } catch (IOException e) {
            // expected
        }

        try {
            mAssets.openXmlResourceParser(0, "notExistFile.txt");
            fail("test openXmlResourceParser(int, String) failed");
        } catch (IOException e) {
            // expected
        }

        assertNotNull(mAssets.getLocales());

    }

    private void assertContextEquals(final String expect, final InputStream inputStream)
            throws IOException {
        final BufferedReader bf = new BufferedReader(new InputStreamReader(inputStream));
        final String result = bf.readLine();
        inputStream.close();
        assertNotNull(result);
        assertEquals(expect, result);
    }

    private void beginDocument(final XmlPullParser parser, final  String firstElementName)
            throws XmlPullParserException, IOException {
        int type;
        while ((type = parser.next()) != XmlPullParser.START_TAG) {
        }
        if (type != XmlPullParser.START_TAG) {
            fail("No start tag found");
        }
        assertEquals(firstElementName, parser.getName());
    }

}
