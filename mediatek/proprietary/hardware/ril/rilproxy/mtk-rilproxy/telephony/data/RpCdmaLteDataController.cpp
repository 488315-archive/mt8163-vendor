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
#include "RpBaseApnHandler.h"
#include "RpCnomApnHandler.h"
#include "RpWwomApnHandler.h"
#include "RpDataUtils.h"

#include "RfxStatusDefs.h"
#include "modecontroller/RpCdmaLteModeController.h"

#define RFX_LOG_TAG "RP_DCLC"

/*****************************************************************************
 * Class RpCdmaLteDataController
 * The class is created if the slot is LC mode.
 * During class life time coordinate with two modems, gsm and c2k.
 *****************************************************************************/

RFX_IMPLEMENT_CLASS("RpCdmaLteDataController", RpCdmaLteDataController, RpDataController);

RpCdmaLteDataController::RpCdmaLteDataController() :
        mIratController(NULL), mApnHandler(NULL), mNeedSuspend(false) {
}

RpCdmaLteDataController::~RpCdmaLteDataController() {
}

void RpCdmaLteDataController::onInit() {
    RpDataController::onInit();  // Required: invoke super class implementation

    logD(RFX_LOG_TAG, "onInit");

    // create the object of RpIratController as the child controller
    // of this object (an instance of RpDataController)
    RFX_OBJ_CREATE(mIratController, RpIratController, this);
    getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_CDMA_CARD_TYPE,
        RfxStatusChangeCallback(this, &RpCdmaLteDataController::onCdmaCardTypeChanged));
    int cardType = getStatusManager()->getIntValue(RFX_STATUS_KEY_CDMA_CARD_TYPE);
    updateApnHandler(cardType);
}

void RpCdmaLteDataController::onDeinit() {
    logD(RFX_LOG_TAG, "onDeinit");
    getStatusManager()->unRegisterStatusChanged(RFX_STATUS_KEY_CDMA_CARD_TYPE,
            RfxStatusChangeCallback(this, &RpCdmaLteDataController::onCdmaCardTypeChanged));
    RpDataController::onDeinit();
}

void RpCdmaLteDataController::onCdmaCardTypeChanged(RfxStatusKeyEnum key,
        RfxVariant old_value, RfxVariant value) {
    RFX_UNUSED(key);

    if (value != old_value) {
        logD(RFX_LOG_TAG, "onCdmaCardTypeChanged old_value:%d, value:%d",
                old_value.asInt(), value.asInt());
        updateApnHandler(value.asInt());
    }
}

void RpCdmaLteDataController::updateApnHandler(int cardType) {
    logD(RFX_LOG_TAG, "updateApnHandler cardType:%d", cardType);
    if (cardType == CARD_NOT_INSERTED) {
        // TODO: for unset(-1) case return too.
        logD(RFX_LOG_TAG, "updateApnHandler card not inserted, do nothing.");
        return;
    }

    if (mApnHandler != NULL) {
        // Fwk will call object.deinit when close controller.
        RFX_OBJ_CLOSE(mApnHandler);
        mApnHandler = NULL;
    }
    if (RpDataUtils::isOP09Cdma4GCard(cardType)) {
        RpCnomApnHandler *p;
        RFX_OBJ_CREATE(p, RpCnomApnHandler, this);
        mApnHandler = (RpBaseApnHandler *)p;
    } else if (RpDataUtils::isNonOP09CdmaCard(cardType)) {
        RpWwomApnHandler *p;
        RFX_OBJ_CREATE(p, RpWwomApnHandler, this);
        mApnHandler = (RpBaseApnHandler *)p;
    } else {
        logD(RFX_LOG_TAG, "unexpected case, card type not processed.");
    }
}

bool RpCdmaLteDataController::onPreviewMessage(const sp<RfxMessage>& message) {
    if (needSuspendPsRequest() && message->getType() == REQUEST) {
        logD(RFX_LOG_TAG, "onPreviewMessage, put %s into pending list",
                RpDataUtils::requestToString(message->getId()));
        return false;
    } else {
        return true;
    }
}

bool RpCdmaLteDataController::onCheckIfResumeMessage(const sp<RfxMessage>& message) {
    logD(RFX_LOG_TAG, "onCheckIfResumeMessage, got request %d",
            message->getId());
    if (needResumePsRequest()) {
        logD(RFX_LOG_TAG, "resume a request %d", message->getId());
        return true;
    } else {
        logD(RFX_LOG_TAG, "keep pending a request %d", message->getId());
        return false;
    }
}

bool RpCdmaLteDataController::onCheckIfRemoveSuspendedMessage(const sp<RfxMessage>& message) {
    logD(RFX_LOG_TAG, "onCheckIfRemoveSuspendedMessage, got request %d",
            message->getId());
    /* when radio unavailable, status is reset, needn't process pending requests.
     * ril framework process all messages in one thread, so the process is atomic.
     */
    if (RpDataUtils::isRadioUnAvailable(this)) {
        logD(RFX_LOG_TAG, "onCheckIfRemoveSuspendedMessage, remove for radio unavailable!");
        return true;
    }
    return false;
}

bool RpCdmaLteDataController::needSuspendPsRequest() {
    return mNeedSuspend;
}

bool RpCdmaLteDataController::needResumePsRequest() {
    return !mNeedSuspend;
}

void RpCdmaLteDataController::suspendPsRequest() {
    mNeedSuspend = true;
}

void RpCdmaLteDataController::resumePsRequest() {
    mNeedSuspend = false;
}

void RpCdmaLteDataController::handleSetDataAllowRequest(
        const sp<RfxMessage>& request) {
    setActivePsSlotIfNeeded(request);
    // send request to two modems.
    sp<RfxMessage> gsmRequest = RfxMessage::obtainRequest(RADIO_TECH_GROUP_GSM,
                                                        request->getId(),
                                                        request,
                                                        true);
    requestToRild(gsmRequest);

    int cdmaSlotId = RpCdmaLteModeController::getCdmaSocketSlotId();
    logD(RFX_LOG_TAG, "AllowDataRequest cdmaSlotId = %d", cdmaSlotId);
    // Not send data allow command to CT slot when the slot have no CT capability.
    // This may happen when mode switching or mode have not switched when switch card.
    // If in this situation, it may get no response.
    if (getSlotId() == cdmaSlotId) {
        sp<RfxMessage> c2kRequest = RfxMessage::obtainRequest(RADIO_TECH_GROUP_C2K,
                                                        request->getId(),
                                                        request,
                                                        true);
        requestToRild(c2kRequest);
    }
}

void RpCdmaLteDataController::handleSetDataAllowResponse(const sp<RfxMessage>& response) {
    int cdmaSlotId = RpCdmaLteModeController::getCdmaSocketSlotId();
    logD(RFX_LOG_TAG, "handleSetDataAllowResponse cdmaSlotId = %d", cdmaSlotId);
    if (cdmaSlotId == getSlotId()) {
        sp<RfxMessage> outResponse;
        ResponseStatus status = preprocessResponse(response, outResponse);
        if (status == RESPONSE_STATUS_HAVE_MATCHED) {
            logD(RFX_LOG_TAG, "handleSetDataAllowResponse both gsm and c2k have response");
            if (response->getError() != RIL_E_SUCCESS) {
                responseToRilj(response);
            } else {
                responseToRilj(outResponse);
            }
        }
    } else {
        responseToRilj(response);
    }
}

void RpCdmaLteDataController::handleSetupDataResponse(const sp<RfxMessage>& response) {
    // call super class implement
    RpDataController::handleSetupDataResponse(response);

    if (response->getError() == RIL_E_SUCCESS
            && response->getSource() == RADIO_TECH_GROUP_GSM) {
        logD(RFX_LOG_TAG, "handleSetupDataResponse onDataSetupSucceed");
        if (mApnHandler != NULL) {
            mApnHandler->onDataSetupSucceed();
        }
    }
}

void RpCdmaLteDataController::handleSetInitialAttachApnRequest(const sp<RfxMessage>& request) {
    logD(RFX_LOG_TAG, "handleSetInitialAttachApnRequest");
    Parcel* parcel = request->getParcel();
    request->resetParcelDataStartPos();
    RpDataUtils::storeIaProperty(getSlotId(), parcel->readString16());
    if (mApnHandler != NULL) {
        sp<RfxMessage> newRequest = mApnHandler->onSetInitialAttachApn(request);
        sendToGsm(newRequest);
    } else {
        sendToGsm(request);
    }
}

