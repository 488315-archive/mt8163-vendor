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

package com.mediatek.op.telephony.cdma;

import android.telephony.PhoneNumberUtils;
import android.telephony.Rlog;

import com.mediatek.common.telephony.cdma.ICdmaCallTrackerExt;
import com.mediatek.common.PluginImpl;
import com.mediatek.internal.telephony.cdma.pluscode.IPlusCodeUtils;
import com.mediatek.internal.telephony.cdma.pluscode.PlusCodeProcessor;

@PluginImpl(interfaceName="com.mediatek.common.telephony.cdma.ICdmaCallTrackerExt")
public class CdmaCallTrackerExt implements ICdmaCallTrackerExt {
    private static final String TAG = "ICdmaCallTrackerExt";
    private IPlusCodeUtils mPlusCodeUtils = PlusCodeProcessor.getPlusCodeUtils();

    public CdmaCallTrackerExt() {
    }

    public String processPlusCodeForDriverCall(String number, boolean isMt, int typeOfAddress) {
        if (isMt && typeOfAddress == PhoneNumberUtils.TOA_International) {
            Rlog.d(TAG, "processPlusCodeForDriverCall, before format number:" + number);
            if (number != null && number.length() > 0 && number.charAt(0) == '+') {
                number = number.substring(1, number.length());
            }
            number = mPlusCodeUtils.removeIddNddAddPlusCode(number);
            Rlog.d(TAG, "processPlusCodeForDriverCall, after format number:" + number);
        }
        number = PhoneNumberUtils.stringFromStringAndTOA(number, typeOfAddress);
        return number;
    }

    public String processPlusCodeForWaitingCall(String number, int numberType) {
        Rlog.d(TAG, "processPlusCodeForWaitingCall, before format number:" + number);
        String format = mPlusCodeUtils.removeIddNddAddPlusCode(number);
        Rlog.d(TAG, "processPlusCodeForWaitingCall, aefore format number:" + format);
        if (format != null) {
            number = format;
            if (numberType == 1 && format.length() > 0 && format.charAt(0) != '+') {
                number = "+" + format;
            }
        }
        return number;
    }

    public boolean needToConvert(String source, String target) {
        Rlog.d(TAG, "needToConvert, source:" + source + ", target:" + target);
        return source != null && target != null && !source.equals(target);
    }
}
