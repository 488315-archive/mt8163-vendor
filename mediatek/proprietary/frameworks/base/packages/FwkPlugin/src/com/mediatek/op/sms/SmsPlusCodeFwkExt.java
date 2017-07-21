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

package com.mediatek.op.sms;

import android.text.TextUtils;
import android.util.Log;

import com.android.internal.telephony.SmsAddress;
import com.android.internal.telephony.cdma.sms.CdmaSmsAddress;

import com.mediatek.common.PluginImpl;
import com.mediatek.common.sms.ISmsPlusCodeFwkExt;

import com.mediatek.internal.telephony.cdma.pluscode.IPlusCodeUtils;
import com.mediatek.internal.telephony.cdma.pluscode.PlusCodeProcessor;

@PluginImpl(interfaceName = "com.mediatek.common.sms.ISmsPlusCodeFwkExt")
public class SmsPlusCodeFwkExt implements ISmsPlusCodeFwkExt {
    private static final String TAG = "ISmsPlusCodeFwkExt";

    private static IPlusCodeUtils sPlusCodeUtils = PlusCodeProcessor.getPlusCodeUtils();

    /**
     * Check and replace the Idd or Ndd in CDMA MT SMS address with
     * plus code.
     * @param obj The java object represents the CdmaSmsAddress.
     * @return The return value indicates that if the obj has been changed.
     */
    public boolean replaceIddNddWithPluscode(Object obj) {
        if ((sPlusCodeUtils == null) || !(obj instanceof CdmaSmsAddress)) {
            return false;
        }

        CdmaSmsAddress cdmaAddr = (CdmaSmsAddress) obj;
        String orignalStr = new String(cdmaAddr.origBytes);
        String number = handlePlusCodeInternal(cdmaAddr.ton, orignalStr);
        if (TextUtils.isEmpty(number)) {
            return false;
        }

        boolean changed = !number.equals(orignalStr);

        // Replace the address
        if (changed) {
            cdmaAddr.origBytes = number.getBytes();
            cdmaAddr.numberOfDigits = cdmaAddr.origBytes.length;
        }
        return changed;
    }

    /**
     * This method handles the plus code according to special rule.
     * Subclass can provide different rule for plus code handling by
     * override this method.
     *
     * @param ton The Address TON type.
     * @param number The number will be handled.
     * @return The string result after handling, the possible cases
     * after handled by sPlusCodeUtils:
     * a)the number is empty, return null tells caller that we can't handle this;
     * b)the number isn't empty, caller can check if this has been modified by
     * comparing with original string.
     */
    protected String handlePlusCodeInternal(int ton, String number) {
        String ret = sPlusCodeUtils.removeIddNddAddPlusCodeForSms(number);
        if (TextUtils.isEmpty(ret)) {
            return null;
        }

        if (ton == SmsAddress.TON_INTERNATIONAL && number.charAt(0) != '+') {
            Log.d(TAG, "handlePlusCodeInternal ton == SmsAddress.TON_INTERNATIONAL");
            ret = "+" + ret;
        }
        Log.d(TAG, "handlePlusCodeInternal, after handled, the address = " + ret);
        return ret;
    }
}
