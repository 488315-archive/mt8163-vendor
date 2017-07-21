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
#include "RpSimControllerBase.h"
#include "RpC2kSimController.h"
#include "RfxStatusDefs.h"
#include <utils/Log.h>
#include <cutils/properties.h>
#include <cutils/jstring.h>


/*****************************************************************************
 * Class RfxController
 *****************************************************************************/

RFX_IMPLEMENT_CLASS("RpC2kSimController", RpC2kSimController, RpSimControllerBase);

RpC2kSimController::RpC2kSimController() {
}

RpC2kSimController::~RpC2kSimController() {
}

void RpC2kSimController::onInit() {
    RpSimControllerBase::onInit(); // Required: invoke super class implementation

    const int request_id_list[] = {
        RIL_REQUEST_WRITE_PB_TO_RUIM  // FIXME Just for example
    };

    const int urc_id_list[] = {
        RIL_UNSOL_CDMA_CARD_TYPE,
        RIL_LOCAL_C2K_UNSOL_CDMA_CARD_READY,
        RIL_UNSOL_CDMA_IMSI_READY
    };

    registerToHandleReuquest(request_id_list, (sizeof(request_id_list)/sizeof(int)));
    registerToHandleUrc(urc_id_list, (sizeof(urc_id_list)/sizeof(int)));

    RLOGD("[RpC2kSimController] onInit (slot %d)", getSlotId());
}

#if 0
void RpSimController::onAvailable(RfxStatusKeyEnum key,
    RfxVariant old_value, RfxVariant value) {
}

void RpSimController::onPowerOn(RfxStatusKeyEnum key,
    RfxVariant old_value, RfxVariant value) {
}
#endif

bool RpC2kSimController::onHandleRequest(const sp<RfxMessage>& message) {
    RLOGD("[RpC2kSimController] handle req %s (slot %d)", requestToString(message->getId()),
            getSlotId());
    return true;
}

bool RpC2kSimController::onHandleUrc(const sp<RfxMessage>& message) {
    RLOGD("[RpC2kSimController] handle urc %s (slot %d)", urcToString(message->getId()),
            getSlotId());
    switch (message->getId()) {
        case RIL_UNSOL_CDMA_CARD_TYPE:
            handleCdmaCardTypeNotify(message);
            break;
        case RIL_LOCAL_C2K_UNSOL_CDMA_CARD_READY:
            getStatusManager()->setBoolValue(RFX_STATUS_KEY_CDMA_CARD_READY, true);
            break;
        case RIL_UNSOL_CDMA_IMSI_READY:
            getStatusManager()->setBoolValue(RFX_STATUS_KEY_CDMA_FILE_READY, true);
            break;
        default:
            return false;
    }
    return true;
}

bool RpC2kSimController::onHandleResponse(const sp<RfxMessage>& message) {
    RLOGD("[RpC2kSimController] handle %s response (slot %d)", requestToString(message->getId()),
            getSlotId());
    return true;
}

void RpC2kSimController::handleCdmaCardTypeNotify(const sp<RfxMessage>& message) {
    int cardtype;
    int count;
    Parcel *p = message->getParcel();
    count = p->readInt32();
    cardtype = p->readInt32();
    RLOGD("[RpC2kSimController] handleCdmaCardTypeNotify (slot %d) count: %d, cardtype: %d",
            getSlotId(), count, cardtype);
    if (count == 1) {
        getStatusManager()->setIntValue(RFX_STATUS_KEY_CDMA_CARD_TYPE, cardtype);
    } else {
        RLOGE("[RpC2kSimController] handleCdmaCardTypeNotify payload format is wrong! (slot %d)",
                getSlotId());
    }
}

void RpC2kSimController::setCt3gDualmodeValue(const sp<RfxMessage>& message) {
    int mode;
    int count;
    Parcel *p = message->getParcel();
    count = p->readInt32();
    mode = p->readInt32();
    RLOGD("[RpC2kSimController] setCt3gDualmodeValue (slot %d) count: %d, mode: %d",
            getSlotId(), count, mode);
    if (count == 1) {
        if (mode == 1) {
            getStatusManager()->setBoolValue(RFX_STATUS_KEY_CT3G_DUALMODE_CARD, true);
        } else {
            getStatusManager()->setBoolValue(RFX_STATUS_KEY_CT3G_DUALMODE_CARD, false);
        }
    } else {
        RLOGE("[RpC2kSimController] setCt3gDualmodeValue but payload format is wrong! (slot %d)",
                getSlotId());
    }
}

void RpC2kSimController::resetCdmaCardStatus() {
    RLOGD("[RpC2kSimController] handle resetCdmaCardStatus (slot %d)", getSlotId());
    getStatusManager()->setIntValue(RFX_STATUS_KEY_CDMA_CARD_TYPE, -1);
    getStatusManager()->setBoolValue(RFX_STATUS_KEY_CDMA_CARD_READY, false);
    getStatusManager()->setBoolValue(RFX_STATUS_KEY_CDMA_FILE_READY, false);
    getStatusManager()->setBoolValue(RFX_STATUS_KEY_CT3G_DUALMODE_CARD, false);
}

bool RpC2kSimController::onPreviewCheckRequestGetSimStatus() {
    RLOGD("[RpC2kSimController] handle onPreviewCheckRequestGetSimStatus (slot %d)", getSlotId());
    if (supportCardType(ICC_CSIM) || supportCardType(ICC_RUIM)) {
        if (getSlotId() == getNonSlotScopeStatusManager()->getIntValue(
                RFX_STATUS_KEY_ACTIVE_CDMALTE_MODE_SLOT)) {
            RLOGD("[RpC2kSimController] check result : %d",
                    !getStatusManager()->getBoolValue(RFX_STATUS_KEY_CDMA_CARD_READY, false));
            return (!getStatusManager()->getBoolValue(RFX_STATUS_KEY_CDMA_CARD_READY, false));
        } else {
            return false;
        }
    } else {
        return false;
    }
}

bool RpC2kSimController::onPreviewCheckRequestGetImsi(
        const sp<RfxMessage>& message) {
    RLOGD("[RpC2kSimController] handle onPreviewCheckRequestGetImsi (slot %d)", getSlotId());
    Parcel *req = message->getParcel();
    size_t dataPos = 0;
    int count = 0;

    dataPos = req->dataPosition();
    count = req->readInt32();
    if (count == 1) {
        // There is only one parameter "AID" in the request
        char *aid_ptr = strdupReadString(req);
        RLOGD("[RpC2kSimController] onPreviewCheckRequestGetImsi aid %s (slot %d)",
                aid_ptr, getSlotId());
        req->setDataPosition(dataPos);

        RILD_RadioTechnology_Group dest = choiceDestViaAid(aid_ptr);
        if (aid_ptr != NULL) {
            free(aid_ptr);
        }
        if (dest == RADIO_TECH_GROUP_C2K &&
                (!getStatusManager()->getBoolValue(RFX_STATUS_KEY_CDMA_FILE_READY, false))) {
            RLOGD("[RpC2kSimController] onPreviewCheckRequestGetImsi C2K file is ready (slot %d)",
                    getSlotId());
            return true;
        } else {
            RLOGE("[RpC2kSimController] onPreviewCheckRequestGetImsi out of scope");
            return false;
        }
    } else {
        RLOGE("[RpC2kSimController] onPreviewCheckRequestGetImsi format is wrong! (slot %d)",
                getSlotId());
        return false;
    }
}

void RpC2kSimController::handlePinPukReq(const sp<RfxMessage>& message) {
    RLOGD("[RpC2kSimController] handlePinPukReq (slot %d)", getSlotId());

    sendReq(message);
}

void RpC2kSimController::handlePinPukRsp(const sp<RfxMessage>& message) {
    RLOGD("[RpC2kSimController] handlePinPukRsp (slot %d)", getSlotId());

    sendRsp(message);
}

void RpC2kSimController::handleQuerySimFacilityReq(const sp<RfxMessage>& message) {
    RLOGD("[RpC2kSimController] handleQuerySimFacilityReq (slot %d)", getSlotId());

    sendReq(message);
}

void RpC2kSimController::handleQuerySimFacilityRsp(const sp<RfxMessage>& message) {
    RLOGD("[RpC2kSimController] handleQuerySimFacilityRsp (slot %d)", getSlotId());

    sendRsp(message);
}

void RpC2kSimController::handleSetSimFacilityReq(const sp<RfxMessage>& message) {
    RLOGD("[RpC2kSimController] handleSetSimFacilityReq (slot %d)", getSlotId());

    sendReq(message);
}

void RpC2kSimController::handleSetSimFacilityRsp(const sp<RfxMessage>& message) {
    RLOGD("[RpC2kSimController] handleSetSimFacilityRsp (slot %d)", getSlotId());

    sendRsp(message);
}


void RpC2kSimController::handleIccOpenChannelReq(const sp<RfxMessage>& message) {
    RLOGD("[RpC2kSimController] handleIccOpenChannelReq (slot %d)", getSlotId());

    sendReq(message);
}

void RpC2kSimController::handleIccOpenChannelRsp(const sp<RfxMessage>& message) {
    RLOGD("[RpC2kSimController] handleIccOpenChannelRsp (slot %d)", getSlotId());

    sendRsp(message);
}

void RpC2kSimController::handleIccCloseChannelReq(const sp<RfxMessage>& message) {
    RLOGD("[RpC2kSimController] handleIccCloseChannelReq (slot %d)", getSlotId());

    sendReq(message);
}

void RpC2kSimController::handleIccCloseChannelRsp(const sp<RfxMessage>& message) {
    RLOGD("[RpC2kSimController] handleIccCloseChannelRsp (slot %d)", getSlotId());

    sendRsp(message);
}

void RpC2kSimController::handleTransmitApduReq(const sp<RfxMessage>& message) {
    RLOGD("[RpC2kSimController] handleTransmitApduReq (slot %d)", getSlotId());

    sendReq(message);
}

void RpC2kSimController::handleTransmitApduRsp(const sp<RfxMessage>& message) {
    RLOGD("[RpC2kSimController] handleTransmitApduRsp (slot %d)", getSlotId());

    sendRsp(message);
}

void RpC2kSimController::handleGetAtrReq(const sp<RfxMessage>& message) {
    RLOGD("[RpC2kSimController] handleGetAtrReq (slot %d)", getSlotId());

    sendReq(message);
}

void RpC2kSimController::handleGetAtrRsp(const sp<RfxMessage>& message) {
    RLOGD("[RpC2kSimController] handleGetAtrRsp (slot %d)", getSlotId());

    sendRsp(message);
}

void RpC2kSimController::handleSetUiccSubscriptionReq(const sp<RfxMessage>& message) {
    RLOGD("[RpC2kSimController] handleSetUiccSubscriptionReq (slot %d)", getSlotId());

    sendReq(message);
}

void RpC2kSimController::handleSetUiccSubscriptionRsp(const sp<RfxMessage>& message) {
    RLOGD("[RpC2kSimController] handleSetUiccSubscriptionRsp (slot %d)", getSlotId());

    sendRsp(message);
}

void RpC2kSimController::handleSimIoReq(const sp<RfxMessage>& message) {
    RLOGD("[RpC2kSimController] handleSimIoReq (slot %d)", getSlotId());

    sendReq(message);
}

void RpC2kSimController::handleSimIoRsp(const sp<RfxMessage>& message) {
    RLOGD("[RpC2kSimController] handleSimIoRsp (slot %d)", getSlotId());

    sendRsp(message);
}

void RpC2kSimController::handleSimAuthenticationReq(const sp<RfxMessage>& message) {
    RLOGD("[RpC2kSimController] handleSimAuthenticationReq (slot %d)", getSlotId());

    sendReq(message);
}

void RpC2kSimController::handleSimAuthenticationRsp(const sp<RfxMessage>& message) {
    RLOGD("[RpC2kSimController] handleSimAuthenticationRsp (slot %d)", getSlotId());

    sendRsp(message);
}

void RpC2kSimController::handleSimTransmitApduBasicReq(
        const sp<RfxMessage>& message) {
    RLOGD("[RpC2kSimController] handleSimTransmitApduBasicReq (slot %d)", getSlotId());

    sendReq(message);
}

void RpC2kSimController::handleSimTransmitApduBasicRsp(
        const sp<RfxMessage>& message) {
    RLOGD("[RpC2kSimController] handleSimTransmitApduBasicRsp (slot %d)", getSlotId());

    sendRsp(message);
}

void RpC2kSimController::sendReq(const sp<RfxMessage>& message) {
    // Pass the request to RILD directly
    //size_t dataPos = 0;
    //Parcel* req = message->getParcel();
    sp<RfxMessage> request = RfxMessage::obtainRequest(RADIO_TECH_GROUP_C2K,
        message->getId(), message, true);

    // Clone Parcel
    //dataPos = req->dataPosition();
    //request->getParcel()->appendFrom(req, dataPos, (req->dataSize() - dataPos));
    //request->getParcel()->setDataPosition(0);

    // Send to GSM Rild
    requestToRild(request);
}

void RpC2kSimController::sendRsp(const sp<RfxMessage>& message) {
    Parcel *rsq = message->getParcel();
    size_t dataPos = 0;
    Parcel *p = NULL;

    sp<RfxMessage> response = RfxMessage::obtainResponse(message->getError(),
                message,true);

    // Clone parcel
    //dataPos = rsq->dataPosition();
    //p = response->getParcel();

    //p->appendFrom(rsq, dataPos, (rsq->dataSize() - dataPos));
    //p->setDataPosition(0);

    // Send to RILJ directly
    responseToRilj(response);
}


