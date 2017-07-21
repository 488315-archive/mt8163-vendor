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
#include "RpCdmaOemController.h"
#include "RfxTimer.h"
#include <cutils/properties.h>
#include <string>
#include <stdlib.h>
#include <utils/String16.h>
#include <stdio.h>
#include "RfxLog.h"

using ::android::String16;
using ::android::String8;
/***************************************************************************** 
 * Class RpCdmaOemController
 *****************************************************************************/
#define RP_CDMA_OEM_TAG "RpCdmaOemController"
/*DESTRILD*/
#define DEST_C2K_RILD "DESTRILD:C2K"
#define DEST_GSM_RILD "DESTRILD:GSM"
#define GSM_TRM_MODE 0
#define CDMA_TRM_MODE 1

RFX_IMPLEMENT_CLASS("RpCdmaOemController", RpCdmaOemController, RfxController);
RFX_OBJ_IMPLEMENT_SINGLETON_CLASS(RpCdmaOemController);

RpCdmaOemController::RpCdmaOemController() {
}

RpCdmaOemController::~RpCdmaOemController() {
}

void RpCdmaOemController::onInit() {
    RfxController::onInit(); // Required: invoke super class implementation
    RFX_LOG_D(RP_CDMA_OEM_TAG, "[RpCdmaOemController] onInit()");
    const int request_id_list[] = {
        RIL_REQUEST_BASEBAND_VERSION,
        RIL_REQUEST_OEM_HOOK_STRINGS,
        RIL_REQUEST_SET_TRM,
        RIL_REQUEST_DEVICE_IDENTITY,
        RIL_REQUEST_SCREEN_STATE
    };
    const int urc_id_list[] = {

    };

    registerToHandleReuquest(request_id_list, (sizeof(request_id_list)/sizeof(int)));
    registerToHandleUrc(urc_id_list, 0);
}

bool RpCdmaOemController::onHandleRequest(const sp<RfxMessage>& message) {
    int msg_id = message->getId();
    RFX_LOG_D(RP_CDMA_OEM_TAG, "[RpCdmaOemController] xonHandleRequest req %d", msg_id);

    switch (msg_id) {
        case RIL_REQUEST_BASEBAND_VERSION: {
            requestBaseBand(message);
        }
        break;
        case RIL_REQUEST_OEM_HOOK_STRINGS:
            handleOemHookStringsReq(message);
            break;
        case RIL_REQUEST_SET_TRM:
            requestSetTrm(message);
            break;
        case RIL_REQUEST_DEVICE_IDENTITY:
            requestDeviceIdentity(message);
            break;
        case RIL_REQUEST_SCREEN_STATE:
            handleScreenStateReq(message);
            break;
        default:
        break;
    }
    return true;
}

bool RpCdmaOemController::onHandleUrc(const sp<RfxMessage>& message) {
    return true;
}

bool RpCdmaOemController::onHandleResponse(const sp<RfxMessage>& message) {
    int msg_id = message->getId();
    RFX_LOG_D(RP_CDMA_OEM_TAG, "[RpCdmaOemController] onHandleResponse req %d", msg_id);
    switch (msg_id) {
        case RIL_REQUEST_BASEBAND_VERSION: {
            responseBaseBand(message);
        }
        break;
        case RIL_REQUEST_OEM_HOOK_STRINGS: {
            responseToRilj(message);
        }
        break;
        case RIL_REQUEST_SET_TRM: {
            responseSetTrm(message);
        }
        break;
        case RIL_REQUEST_DEVICE_IDENTITY: {
            responseDeviceIdentity(message);
        }
        break;
        case RIL_REQUEST_SCREEN_STATE: {
           handleScreenStateRsp(message);
        }
        break;
        default:
        break;
    }
    return true;
}

void RpCdmaOemController::requestBaseBand(const sp<RfxMessage>& message) {
    sp<RfxMessage> cdma_baseband_request =
            RfxMessage::obtainRequest(RADIO_TECH_GROUP_C2K,
                    message->getId(), message);
    requestToRild(message);
    requestToRild(cdma_baseband_request);
}

void RpCdmaOemController::requestDeviceIdentity(const sp<RfxMessage>& message) {
    int slotId = message->getSlotId();
    int cdma_socket_id = getNonSlotScopeStatusManager()->getIntValue(RFX_STATUS_KEY_CDMA_SOCKET_SLOT, 0);
    RFX_LOG_D(RP_CDMA_OEM_TAG, "[requestDeviceIdentity]"
            " slotId = %d, cdma_socket_id = %d", slotId, cdma_socket_id);
    if (slotId != cdma_socket_id) {
        RFX_LOG_D(RP_CDMA_OEM_TAG, "[requestDeviceIdentity]"
                " slotId =! cdma_socket_id");
        sp<RfxMessage> un_cdma_socket_request =
                RfxMessage::obtainResponse(RIL_E_GENERIC_FAILURE, message, false);
        responseToRilj(un_cdma_socket_request);

    } else {
        requestToRild(message);
    }
}

void RpCdmaOemController::responseBaseBand(const sp<RfxMessage>& message) {
    if (message->getSource() == RADIO_TECH_GROUP_C2K) {
        if (NULL != message->getParcel()) {
            String16 str = message->getParcel()->readString16();
            RFX_LOG_D(RP_CDMA_OEM_TAG, "[RpCdmaOemController] onHandleResponse RADIO_TECH_GROUP_C2K"
                " RIL_REQUEST_BASEBAND_VERSION"
                " message->getParcel()->readString16() =  %s",
            String8(str).string());
            property_set("cdma.version.baseband", String8(str).string());
        }
    } else if (message->getSource() == RADIO_TECH_GROUP_GSM) {
        RFX_LOG_D(RP_CDMA_OEM_TAG, "[RpCdmaOemController]"
            " onHandleResponse RADIO_TECH_GROUP_GSM"
            " RIL_REQUEST_BASEBAND_VERSION");
        responseToRilj(message);
    }
}

void RpCdmaOemController::responseSetTrm(const sp<RfxMessage>& message) {
    RFX_LOG_D(RP_CDMA_OEM_TAG, "[RpCdmaOemController]"
        " responseSetTrm ");
    responseToRilj(message);
}

void RpCdmaOemController::responseDeviceIdentity(const sp<RfxMessage>& message) {
    RFX_LOG_D(RP_CDMA_OEM_TAG, "[responseDeviceIdentity]");
    responseToRilj(message);
}

void RpCdmaOemController::handleOemHookStringsReq(const sp<RfxMessage>& message) {
    Parcel *p = message->getParcel();
    int32_t countStrings = 0;
    String16 s16;
    p->readInt32 (&countStrings);
    RFX_LOG_D(RP_CDMA_OEM_TAG, "[handleOemHookStringsReq] countStrings: %d", countStrings);

    if (countStrings <= 1) {
        requestToRild(message);
    } else {
        int pos = p->dataPosition();
        for (int i = 0 ; i < countStrings; i++) {
            s16 = p->readString16();
        }
        const char* pString = String8(s16).string();
        p->setDataPosition(pos);
        RFX_LOG_D(RP_CDMA_OEM_TAG, "[handleOemHookStringsReq]string: %s, dataPosition: %d", pString, pos);
        if (pString != NULL && strncmp(pString, DEST_C2K_RILD, strlen(DEST_C2K_RILD)) == 0) {
            sp<RfxMessage> c2k_hook_strings_request = RfxMessage::obtainRequest(
                    RADIO_TECH_GROUP_C2K,
                    message->getId(),
                    message);
            c2k_hook_strings_request->getParcel()->writeInt32(countStrings - 1);
            for (int i = 0 ; i < countStrings - 1; i++) {
                s16 = p->readString16();
                c2k_hook_strings_request->getParcel()->writeString16(s16);
                RFX_LOG_D(RP_CDMA_OEM_TAG, "[handleOemHookStringsReq]To c2k, string: %s", String8(s16).string());
            }
            requestToRild(c2k_hook_strings_request);
        } else if (pString != NULL && strncmp(pString, DEST_GSM_RILD, strlen(DEST_GSM_RILD)) == 0) {
            sp<RfxMessage> gsm_hook_strings_request = RfxMessage::obtainRequest(
                    RADIO_TECH_GROUP_GSM,
                    message->getId(),
                    message);
            gsm_hook_strings_request->getParcel()->writeInt32(countStrings - 1);
            for (int i = 0 ; i < countStrings - 1; i++) {
                s16 = p->readString16();
                gsm_hook_strings_request->getParcel()->writeString16(s16);
                RFX_LOG_D(RP_CDMA_OEM_TAG, "[handleOemHookStringsReq]to gsm, string: %s", String8(s16).string());
            }
            requestToRild(gsm_hook_strings_request);
        } else {
            RFX_LOG_D(RP_CDMA_OEM_TAG, "[handleOemHookStringsReq]to gsm, default");
            requestToRild(message);
        }
    }
}

void RpCdmaOemController::requestSetTrm(const sp<RfxMessage>& message) {
    Parcel *p = message->getParcel();
    int pos = p->dataPosition();
    int32_t type = p->readInt32();
    int32_t mode = p->readInt32();
    p->setDataPosition(pos);

    RFX_LOG_D(RP_CDMA_OEM_TAG, "[RpCdmaOemController] requestSetTrm1"
        "type = %d mode = %d", type, mode);

    if (judgeTrmMode(mode) == GSM_TRM_MODE) {
        RFX_LOG_D(RP_CDMA_OEM_TAG, "[RpCdmaOemController] requestSetTrm"
            "to GSMa");
        sp<RfxMessage> gsm_trm_request =
                RfxMessage::obtainRequest(RADIO_TECH_GROUP_GSM, RIL_REQUEST_SET_TRM, message, true);
        requestToRild(gsm_trm_request);
    } else if (judgeTrmMode(mode) == CDMA_TRM_MODE) {
        int slotId = message->getSlotId();
        int cdma_socket_id = getNonSlotScopeStatusManager()->getIntValue(RFX_STATUS_KEY_CDMA_SOCKET_SLOT, 0);
        RFX_LOG_D(RP_CDMA_OEM_TAG, "[requestSetTrm]"
                " slotId = %d, cdma_socket_id = %d", slotId, cdma_socket_id);
        if (slotId != cdma_socket_id) {
            RFX_LOG_D(RP_CDMA_OEM_TAG, "[requestSetTrm]"
                    " slotId =! cdma_socket_id");
            sp<RfxMessage> un_cdma_socket_request =
                    RfxMessage::obtainResponse(RIL_E_GENERIC_FAILURE, message, false);
            responseToRilj(un_cdma_socket_request);

        } else {
            RFX_LOG_D(RP_CDMA_OEM_TAG, "[RpCdmaOemController] requestSetTrm"
                "to CDMAa");
            sp<RfxMessage> cdma_trm_request =
                    RfxMessage::obtainRequest(RADIO_TECH_GROUP_C2K, RIL_REQUEST_SET_TRM, message, true);
            requestToRild(cdma_trm_request);
        }
    } else {
        RFX_LOG_D(RP_CDMA_OEM_TAG, "[RpCdmaOemController] requestSetTrm mode not support");
    }
}

/**
 * 0 means is GSM trm mode.
 * 1 means is CDMA trm mode.
 * 2 means other.
 */
int RpCdmaOemController::judgeTrmMode(int mode) {
    switch (mode) {
        case 1:
        case 2:
        case 3:
        case 5:
        case 6:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 101:
            return 0;
            break;
        case 31:
        case 32:
        case 103:
            return 1;
            break;
        default:
            return 2;
            break;
    }
}

void RpCdmaOemController::handleScreenStateReq(const sp<RfxMessage>& message) {
    RFX_LOG_D(RP_CDMA_OEM_TAG, "[handleScreenStateReq]slot: %d", getSlotId());
    requestToRild(message);
    if (getSlotId() == getNonSlotScopeStatusManager()->getIntValue(
            RFX_STATUS_KEY_CDMA_SOCKET_SLOT, 0)) {
        sp<RfxMessage> cdma_screenstate_request =
                RfxMessage::obtainRequest(RADIO_TECH_GROUP_C2K, message->getId(), message, true);
        requestToRild(cdma_screenstate_request);
    }
}

void RpCdmaOemController::handleScreenStateRsp(const sp<RfxMessage>& message) {
    RFX_LOG_D(RP_CDMA_OEM_TAG, "[handleScreenStateRsp]slot: %d", getSlotId());
    if (getSlotId() == getNonSlotScopeStatusManager()->getIntValue(
            RFX_STATUS_KEY_CDMA_SOCKET_SLOT, 0)) {
        sp<RfxMessage> msgInQueue = sp<RfxMessage>(NULL);
        ResponseStatus responseStatus = preprocessResponse(message, msgInQueue,
                            RfxWaitResponseTimedOutCallback(this,
                                        &RpCdmaOemController::responseSetScreenState), s2ns(10));
        if (responseStatus == RESPONSE_STATUS_HAVE_MATCHED) {
            RFX_LOG_D(RP_CDMA_OEM_TAG, "[handleScreenStateRsp]Response matched");
            if (message->getError() != RIL_E_SUCCESS) {
                responseToRilj(message);
            } else {
                responseToRilj(msgInQueue);
            }
        }
    } else {
        responseToRilj(message);
    }
}

void RpCdmaOemController::responseSetScreenState(const sp<RfxMessage>& message){
    responseToRilj(message);
}
