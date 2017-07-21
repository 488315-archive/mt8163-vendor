/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2015. All rights reserved.
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

/*****************************************************************************
 * Include
 *****************************************************************************/
#include "RpSmsCtrl.h"
#include "RfxNwServiceState.h"

/*****************************************************************************
 * Class RpSmsCtrl
 *****************************************************************************/
RFX_IMPLEMENT_CLASS("RpSmsCtrl", RpSmsCtrl, RpSmsCtrlBase);

RpSmsCtrl::RpSmsCtrl() :
    mGsmSmsCtrl(NULL),
    mCdmaSmsCtrl(NULL) {
    setName(String8("RpSmsCtrl"));
}

void RpSmsCtrl::onInit() {
    RpSmsCtrlBase::onInit(); // Required: invoke super class implementation

    log(String8("onInit"));

    // Create Gsm/Cdma sms controller
    RFX_OBJ_CREATE(mGsmSmsCtrl, RpGsmSmsCtrl, this);
    RFX_OBJ_CREATE(mCdmaSmsCtrl, RpCdmaSmsCtrl, this);

    // Start to listen the hybrid requests
    const int request_id_list[] = {
        RIL_REQUEST_GET_SMSC_ADDRESS,
        RIL_REQUEST_SET_SMSC_ADDRESS,
        RIL_REQUEST_REPORT_SMS_MEMORY_STATUS,
        RIL_REQUEST_GET_SMS_SIM_MEM_STATUS
    };

    registerToHandleReuquest(request_id_list, (sizeof(request_id_list)/sizeof(int)));

    // Initialize the default value
    const RfxVariant defaultValue = RfxVariant(RADIO_TECH_GROUP_GSM);
    RfxVariant initValue;
    initValue = getStatusManager()->getValue(RFX_STATUS_KEY_VOICE_TYPE, defaultValue);
    mVoiceType = initValue.asInt();

    // Register the voice capability status manager
    getStatusManager()->registerStatusChanged(
            RFX_STATUS_KEY_VOICE_TYPE,
            RfxStatusChangeCallback(this, &RpSmsCtrl::onVoiceTypeChanged));
}

bool RpSmsCtrl::onHandleRequest(const sp<RfxMessage>& message) {
    //String8 reqName = String8("Handle request") + String8(requestToString(message->getId()));
    //log(reqName);

    // check if any sub-module need to handle
    if (RIL_REQUEST_HANDLE_REASON_HANDLED == mGsmSmsCtrl->isHandled(message)) {
        // Not handle due to Gsm is handled
        log(String8("RpGsmSmsCtrl is handled"));
        return true;
    }

    if (RIL_REQUEST_HANDLE_REASON_HANDLED == mCdmaSmsCtrl->isHandled(message)) {
        // Not handle due to Gsm is handled
        log(String8("RpCdmaSmsCtrl is handled"));
        return true;
    }

    switch (message->getId()) {
        default:
            // Pass the request to related CS domain RILD
            requestToRild(RfxMessage::obtainRequest(
                    (RILD_RadioTechnology_Group) mVoiceType, message->getId(), message, true));
            break;
    }
    return true;
}

void RpSmsCtrl::onVoiceTypeChanged(RfxStatusKeyEnum key, RfxVariant oldValue, RfxVariant newValue) {
    RFX_UNUSED(key);

    int oldVoiceType = oldValue.asInt();
    int newVoiceType = newValue.asInt();

    mVoiceType = newVoiceType;
}
