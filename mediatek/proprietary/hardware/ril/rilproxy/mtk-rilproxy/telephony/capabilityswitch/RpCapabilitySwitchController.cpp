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

#include <cutils/properties.h>
#include <ril.h>
#include <Errors.h>
#include "RpCapabilitySwitchController.h"
#include "RfxLog.h"
#include "RfxStatusManager.h"

#define RFX_LOG_TAG "RpCapabilitySwitchController"

/*****************************************************************************
 * Class RfxController
 *****************************************************************************/

RFX_IMPLEMENT_CLASS("RpCapabilitySwitchController", RpCapabilitySwitchController, RfxController);

RpCapabilitySwitchController::RpCapabilitySwitchController() : kModeSwitching(1) {
}

RpCapabilitySwitchController::~RpCapabilitySwitchController() {
}

void RpCapabilitySwitchController::onInit() {
    RfxController::onInit(); // Required: invoke super class implementation
    RFX_LOG_D(RFX_LOG_TAG, "RpCapabilitySwitchController, onInit");
    const int request_id_list[] = {
        RIL_REQUEST_SET_RADIO_CAPABILITY
    };

    // register request & URC id list
    // NOTE. one id can only be registered by one controller
    registerToHandleRequest(0, request_id_list, (sizeof(request_id_list)/sizeof(int)));
    registerToHandleRequest(1, request_id_list, (sizeof(request_id_list)/sizeof(int)));
    //registerToHandleUrc(urc_id_list, (sizeof(urc_id_list)/sizeof(int)));
}

bool RpCapabilitySwitchController::onPreviewMessage(const sp<RfxMessage>& message) {
    int modeSwitching = getNonSlotScopeStatusManager()->getIntValue(
        RFX_STATUS_KEY_MODE_SWITCH, 0);
    RFX_LOG_D(RFX_LOG_TAG, "RpCapabilitySwitchController,onPreviewMessage modeSwitching = %d",
        modeSwitching);
    if (modeSwitching == kModeSwitching){
        if (checkPhaseIfFinish(message)){
            RFX_LOG_D(RFX_LOG_TAG, "onPreviewMessage, modeswitching in finish phase ,by pass");
            return true;
        }
        return false;
    }
    return true;
}

bool RpCapabilitySwitchController::onCheckIfResumeMessage(const sp<RfxMessage>& message) {
    int modeSwitching = getNonSlotScopeStatusManager()->getIntValue(
        RFX_STATUS_KEY_MODE_SWITCH, 0);
    RFX_LOG_D(RFX_LOG_TAG, "RpCapabilitySwitchController,onCheckIfResumeMessage modeSwitching = %d",
        modeSwitching);
    if (modeSwitching == kModeSwitching){
        if (checkPhaseIfFinish(message)){
            RFX_LOG_D(RFX_LOG_TAG, "onCheckIfResumeMessage, modeswitching in finish phase ,by pass");
            return true;
        }
        return false;
    }
    return true;
}


bool RpCapabilitySwitchController::onHandleRequest(const sp<RfxMessage>& message) {
    RFX_LOG_D(RFX_LOG_TAG, "onHandleRequest, handle: %s", requestToString(message->getId()));
    int msg_id = message->getId();
    switch (msg_id) {
    case RIL_REQUEST_SET_RADIO_CAPABILITY: {
            sp<RfxMessage> c2k_request =
                    RfxMessage::obtainRequest(RADIO_TECH_GROUP_C2K, msg_id, message,true);
            char tmp[PROPERTY_VALUE_MAX] = {0};
            property_get(PROPERTY_NET_CDMA_MDMSTAT, tmp, "not");
            int c2kSlot = getStatusManager(RFX_SLOT_ID_UNKNOWN)->getIntValue(
                    RFX_STATUS_KEY_CDMA_SOCKET_SLOT, 0);
            RFX_LOG_D(RFX_LOG_TAG, "onHandleRequest, C2K slot=%d, %s", c2kSlot, tmp);
            if(checkPhase(message)){
               RFX_LOG_D(RFX_LOG_TAG, "onHandleRequest, checkPhase started true");
               if((strncmp(tmp, "ready",5) == 0) &&
                       (message->getSlotId() == c2kSlot)){
                  RFX_LOG_D(RFX_LOG_TAG, "onHandleRequest, handle C2K request");
                  requestToRild(c2k_request);
               } else {
                  RFX_LOG_D(RFX_LOG_TAG, "onHandleRequest, handle GSM request,slotId= %d",
                        message->getSlotId());
                  requestToRild(message);
               }
            } else {
               RFX_LOG_D(RFX_LOG_TAG, "onHandleRequest, other Phase always to GSM");
               requestToRild(message);
            }
        }
        break;
    default:
        break;
    }
    return true;
}

bool RpCapabilitySwitchController::checkPhase(const sp<RfxMessage>& message) {
    int status;
    int request;
    int token;
    RIL_RadioCapability rc;
    int t;
    Parcel* p = message->getParcel();

    memset (&rc, 0, sizeof(RIL_RadioCapability));

    status = p->readInt32(&t);
    rc.version = (int)t;
    //if (status != NO_ERROR) {
    //    goto invalid;
    //}

    status = p->readInt32(&t);
    rc.session= (int)t;
    //if (status != NO_ERROR) {
    //    goto invalid;
    //}

    status = p->readInt32(&t);
    rc.phase= (int)t;
    //if (status != NO_ERROR) {
    //    goto invalid;
    //}

    status = p->readInt32(&t);
    rc.rat = (int)t;
    //if (status != NO_ERROR) {
    //    goto invalid;
    //}

    //status = readStringFromParcelInplace(p, rc.logicalModemUuid, sizeof(rc.logicalModemUuid));
    //if (status != NO_ERROR) {
    //    goto invalid;
    //}

    //status = p->readInt32(&t);
    //rc.status = (int)t;

    //if (status != NO_ERROR) {
    //    goto invalid;
    //}

   if (rc.phase == RC_PHASE_APPLY){
        RFX_LOG_D(RFX_LOG_TAG, "check phase, RC_PHASE_APPLY");
        return true;
   } else {
        if (rc.phase == RC_PHASE_START){
            //Set property for modeController to check if simSwitching,
            //It will be reset in ril init after TRM or RC_FINISH
            RFX_LOG_D(RFX_LOG_TAG, "check phase, START and set switching flag");
            property_set("ril.cdma.switching", "1");
        }
        if (rc.phase == RC_PHASE_FINISH){
            //Reset switching flag when FINISH,because sim switch may fail and not do TRM
            RFX_LOG_D(RFX_LOG_TAG, "check phase, FINISH and reset switching flag");
            property_set("ril.cdma.switching", "0");
        }
        RFX_LOG_D(RFX_LOG_TAG, "check phase = %d",rc.phase);
        return false;
   }
}

bool RpCapabilitySwitchController::checkPhaseIfFinish(const sp<RfxMessage>& message) {
    int status;
    int request;
    int token;
    RIL_RadioCapability rc;
    int t;
    Parcel* p = message->getParcel();
    memset (&rc, 0, sizeof(RIL_RadioCapability));

    status = p->readInt32(&t);
    rc.version = (int)t;

    status = p->readInt32(&t);
    rc.session= (int)t;

    status = p->readInt32(&t);
    rc.phase= (int)t;

    status = p->readInt32(&t);
    rc.rat = (int)t;

    message->resetParcelDataStartPos();

    if ((rc.phase == RC_PHASE_FINISH) || (rc.phase == RC_PHASE_CONFIGURED)){
        RFX_LOG_D(RFX_LOG_TAG, "checkPhaseIfFinish true");
        return true;
    } else {
        return false;
    }
}


char* RpCapabilitySwitchController::requestToString(int reqId) {
    switch (reqId) {
        case RIL_REQUEST_SET_RADIO_CAPABILITY:
            return "SET_RADIO_CAPABILITY";
        default:
            RFX_LOG_E(RFX_LOG_TAG, "requestToString, reqId: %d", reqId);
            return "UNKNOWN_REQUEST";
    }
}

char* RpCapabilitySwitchController::urcToString(int urcId) {
    switch (urcId) {
        default:
            RFX_LOG_E(RFX_LOG_TAG, "requestToString, urcId: %d", urcId);
            return "UNKNOWN_URC";
    }
}

