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
#include "RpCallCenterController.h"

#define LOG_TAG "RpCallCenterController"

/*****************************************************************************
 * Class RpCallCenterController
 *****************************************************************************/
RFX_IMPLEMENT_CLASS("RpCallCenterController", RpCallCenterController, RfxController);

RpCallCenterController::RpCallCenterController() {

}

RpCallCenterController::~RpCallCenterController() {

}

void RpCallCenterController::onInit() {
    RfxController::onInit(); // Required: invoke super class implementation
    RLOGD("[RpCallCenterController] onInit E!");
}

void RpCallCenterController::handleSwitchAntennaRequest(SWITCH_ANTENNA_STATE state, SWITCH_ANTENNA_RAT_MODE rat) {
    RLOGD("[RpCallCenterController] handleSwitchAntennaRequest, state:%d, rat:%d", state, rat);
    int slotId = -1;
    for (int i = 0; i < RFX_SLOT_COUNT; i++) {
        if (getStatusManager(i)->getIntValue(RFX_STATUS_KEY_VOICE_TYPE, -1) == RADIO_TECH_GROUP_GSM) {
            slotId = i;
            break;
        }
    }
    RLOGD("Switch antenna, slotId:%d", slotId);
    if (slotId != -1) {
        sp<RfxMessage> rilRequest = RfxMessage::obtainRequest(slotId, RADIO_TECH_GROUP_GSM, RIL_REQUEST_SWITCH_ANTENNA);
        rilRequest->getParcel()->writeInt32(2);
        rilRequest->getParcel()->writeInt32(state);
        rilRequest->getParcel()->writeInt32(rat);
        requestToRild(rilRequest);
    }
}

void RpCallCenterController::handleSwitchHPF(bool enable) {
    RLOGD("[RpCallCenterController] handleSwitchHPF, enable:%d", enable);
    int slotId = -1;
    for (int i = 0; i < RFX_SLOT_COUNT; i++) {
        if (getStatusManager(i)->getIntValue(RFX_STATUS_KEY_VOICE_TYPE, -1) == RADIO_TECH_GROUP_C2K) {
            slotId = i;
            break;
        }
    }
    RLOGD("Switch HPF, slotId:%d", slotId);
    if (slotId != -1) {
        sp<RfxMessage> rilRequest = RfxMessage::obtainRequest(slotId, RADIO_TECH_GROUP_C2K, RIL_REQUEST_SWITCH_HPF);
        rilRequest->getParcel()->writeInt32(1);
        rilRequest->getParcel()->writeInt32(enable ? 1 : 0);
        requestToRild(rilRequest);
    }
}

bool RpCallCenterController::handleIncomingCallIndication(RILD_RadioTechnology_Group type, const sp<RfxMessage>& message) {
    RLOGD("[RpCallCenterController] handleIncomingCallIndication type:%d", type);
    int slotId = -1;
    int targetTech = -1;
    if (type == RADIO_TECH_GROUP_C2K) {
        targetTech = RADIO_TECH_GROUP_GSM;
    } else if (type == RADIO_TECH_GROUP_GSM) {
        targetTech = RADIO_TECH_GROUP_C2K;
    }
    if (targetTech != -1) {
        for (int i = 0; i < RFX_SLOT_COUNT; i++) {
            if (getStatusManager(i)->getIntValue(RFX_STATUS_KEY_VOICE_TYPE, -1) == targetTech) {
                slotId = i;
                break;
            }
        }
    }
    RLOGD("handleIncomingCallIndication, target slotId:%d, ", slotId);
    if (slotId != -1) {
        int callCount = getStatusManager(slotId)->getIntValue(RFX_STATUS_KEY_VOICE_CALL_COUNT, 0);
        RLOGD("handleIncomingCallIndication, callCount:%d, ", callCount);
        if (callCount > 0) {
            if (type == RADIO_TECH_GROUP_GSM) {
                int callId = -1;
                int seqNumber = -1;
                message->getParcel()->setDataPosition(3 * sizeof(int));
                String8 str = String8(message->getParcel()->readString16());
                callId = atoi(str.string());
                message->getParcel()->readString16();
                message->getParcel()->readString16();
                message->getParcel()->readString16();
                str = String8(message->getParcel()->readString16());
                seqNumber = atoi(str.string());
                RLOGD("handleIncomingCallIndication, callId:%d, seqNumber:%d", callId, seqNumber);
                sp<RfxMessage> rilRequest = RfxMessage::obtainRequest(message->getSlotId(),
                                                RADIO_TECH_GROUP_GSM, RIL_REQUEST_SET_CALL_INDICATION);
                rilRequest->getParcel()->writeInt32(3);
                rilRequest->getParcel()->writeInt32(1);
                rilRequest->getParcel()->writeInt32(callId);
                rilRequest->getParcel()->writeInt32(seqNumber);
                requestToRild(rilRequest);
                return false;
            } else if (type == RADIO_TECH_GROUP_C2K) {
                sp<RfxMessage> rilRequest = RfxMessage::obtainRequest(message->getSlotId(),
                                                RADIO_TECH_GROUP_C2K, RIL_REQUEST_HANGUP);
                rilRequest->getParcel()->writeInt32(1);
                rilRequest->getParcel()->writeInt32(-1);
                requestToRild(rilRequest);
                return false;
            }
        }
    }
    return true;
}
