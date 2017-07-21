/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
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

#include "RpDataController.h"
#include "RpCdmaLteDataController.h"
#include "RpIratController.h"
#include "RpDataUtils.h"

#include "RfxStatusDefs.h"

/*****************************************************************************
 * Class RfxController
 *****************************************************************************/

#define RFX_LOG_TAG "RP_DC_IRAT_CTRL"

RFX_IMPLEMENT_CLASS("RpIratController", RpIratController, RfxController);

RpIratController::RpIratController() : mIsDuringIrat(false), mDataController(NULL) {
}

RpIratController::~RpIratController() {
}

void RpIratController::onInit() {
    RfxController::onInit();  // Required: invoke super class implementation
    logD(RFX_LOG_TAG, "onInit");
    mDataController = static_cast<RpCdmaLteDataController *>(RfxObject::getParent());

    const int urc_id_list[] = {
            RIL_UNSOL_INTER_3GPP_IRAT_STATE_CHANGE,  // 1094
        };

    registerToHandleUrc(urc_id_list, sizeof(urc_id_list) / sizeof(int));
    getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_RADIO_STATE,
            RfxStatusChangeCallback(this, &RpIratController::onRadioStateChanged));
}

void RpIratController::onDeinit() {
    logD(RFX_LOG_TAG, "onDeinit");
    getStatusManager()->unRegisterStatusChanged(RFX_STATUS_KEY_RADIO_STATE,
                RfxStatusChangeCallback(this, &RpIratController::onRadioStateChanged));
    RfxController::onDeinit();
}

bool RpIratController::onHandleUrc(const sp<RfxMessage>& message) {
    logD(RFX_LOG_TAG, "Handle urc %s", RpDataUtils::urcToString(message->getId()));

    switch (message->getId()) {
        case RIL_UNSOL_INTER_3GPP_IRAT_STATE_CHANGE:
            handleIratStateChangedUrc(message);
            break;
    }
    return true;
}

bool RpIratController::onHandleResponse(const sp<RfxMessage>& message) {
    logD(RFX_LOG_TAG, "Handle response %s ", RpDataUtils::requestToString(message->getId()));

    switch (message->getId()) {
    case RIL_REQUEST_CONFIRM_INTER_3GPP_IRAT_CHANGE: {
        handleConfirmIratResponse(message);
            break;
        }
    default:
        break;
    }
    return true;
}

void RpIratController::handleConfirmIratResponse(const sp<RfxMessage>& message) {
    logD(RFX_LOG_TAG, "handleConfirmIratResponse");
    if (message->getError() != RIL_E_SUCCESS) {
        resumePsRequest();
    }
}

void RpIratController::handleIratStateChangedUrc(const sp<RfxMessage>& message) {
    Parcel *p = message->getParcel();
    int iratSource = p->readInt32();
    int iratTarget = p->readInt32();
    IratAction iratAction = (IratAction) p->readInt32();
    IratType iratType = (IratType) p->readInt32();
    if (iratAction == IRAT_ACTION_SOURCE_STARTED) {
        logD(RFX_LOG_TAG, "handleIratStateChangedUrc IRAT_ACTION_SOURCE_STARTED");
        mIsDuringIrat = true;
        suspendPsRequest();
        confirmIrat(message->getSource());
    } else if (iratAction == IRAT_ACTION_TARGET_FINISHED) {
        logD(RFX_LOG_TAG, "handleIratStateChangedUrc IRAT_ACTION_TARGET_FINISHED");
        resumePsRequest();
        mIsDuringIrat = false;
    }
}

void RpIratController::suspendPsRequest() {
    mDataController->suspendPsRequest();
}

void RpIratController::resumePsRequest() {
    mDataController->resumePsRequest();
}

void RpIratController::confirmIrat(RILD_RadioTechnology_Group sourceRat) {
    logD(RFX_LOG_TAG, "confirmIrat: sourceRat is %s",
            RpDataUtils::radioGroupToString(sourceRat));

    sp<RfxMessage> confirmMsg = RfxMessage::obtainRequest(getSlotId(),
            sourceRat, RIL_REQUEST_CONFIRM_INTER_3GPP_IRAT_CHANGE);
    Parcel *p = confirmMsg->getParcel();
    p->writeInt32(1);
    p->writeInt32(IRAT_CONFIRM_ACCEPTED);
    requestToRild(confirmMsg);
}

void RpIratController::onRadioStateChanged(RfxStatusKeyEnum key,
        RfxVariant oldValue, RfxVariant value) {
    RFX_UNUSED(key);
    RFX_UNUSED(oldValue);
    RIL_RadioState radioState = (RIL_RadioState) value.asInt();
    logD(RFX_LOG_TAG, "onRadioStateChanged: radioState=%d, mIsDuringIrat=%d",
            radioState, mIsDuringIrat);
    if (mIsDuringIrat && radioState == RADIO_STATE_UNAVAILABLE) {
        mDataController->resumePsRequest();
        mIsDuringIrat = false;
    }
}
