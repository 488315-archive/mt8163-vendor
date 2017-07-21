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
#include "RpRadioController.h"
#include "RpCdmaRadioController.h"
#include "RadioConstants.h"
#include "Parcel.h"
#include "RfxMainThread.h"
#include "RfxSocketState.h"
#include <utils/Log.h>
#include "utils/RefBase.h"
#include "utils/Errors.h"
#include <cutils/properties.h>
#include <string.h>
/*****************************************************************************
 * Class RfxController
 *****************************************************************************/

#define LOG_TAG "RpRadioController"

#define ARRAY_LENGTH(array) (sizeof(array)/sizeof(array[0]))

extern "C" void setRadioState(RIL_RadioState newState, RIL_SOCKET_ID rid);

RFX_IMPLEMENT_CLASS("RpRadioController", RpRadioController, RfxController);

RpRadioController::RpRadioController() {
}

RpRadioController::~RpRadioController() {
}

static const int request[] = {
    RIL_REQUEST_RADIO_POWER
};

static const int urc[] = {
    RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED
};

static const int INVALID_VALUE = -1;

void RpRadioController::onInit() {
    RLOGD("RpRadioController init()");
	RfxController::onInit();
    slotId = getSlotId();
    RLOGD("RpRadioController init(): %d", slotId);

    //WARNING: TEST for CDMA
    //if (slotId == 0) {
    //    getStatusManager() ->
    //        setIntValue(RFX_STATUS_KEY_SUGGESTED_RADIO_CAPABILITY, RIL_CAPABILITY_CDMA_ON_LTE);
    //}

    // set to GSM only if no one set this value
    int capability = getStatusManager()->getIntValue(RFX_STATUS_KEY_SUGGESTED_RADIO_CAPABILITY, INVALID_VALUE);
    if (capability == INVALID_VALUE) {
        getStatusManager()->setIntValue(RFX_STATUS_KEY_SUGGESTED_RADIO_CAPABILITY, RIL_CAPABILITY_GSM_ONLY);
    }

    getStatusManager()->setIntValue(RFX_STATUS_KEY_RADIO_STATE, RADIO_STATE_UNAVAILABLE);
    registerToHandleReuquest(request, ARRAY_LENGTH(request));
    registerToHandleUrc(urc, ARRAY_LENGTH(urc));
    registerForStatusChange();
    MessageQueue = new RadioMessage();
    RLOGD("RpRadioController init():Done");
}

void RpRadioController::registerForStatusChange() {
    RLOGD("RpRadioController: registerForStatusChange");
    getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_SUGGESTED_RADIO_CAPABILITY,
        RfxStatusChangeCallback(this, &RpRadioController::onSuggestedCapabilityChanged));
    getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_RADIO_STATE,
        RfxStatusChangeCallback(this, &RpRadioController::onRadioStateChanged));
    getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_RADIO_CAPABILITY,
        RfxStatusChangeCallback(this, &RpRadioController::onCapabilityChanged));
}

void RpRadioController::onRadioStateChanged(RfxStatusKeyEnum key,
    RfxVariant old_value, RfxVariant value) {
    //sync with rild's state
    int newValue = value.asInt();
    RLOGD("radio State Changed, using google native API for urc");
    setRadioState((RIL_RadioState)newValue, (RIL_SOCKET_ID)slotId);
}

void RpRadioController::onSuggestedCapabilityChanged(RfxStatusKeyEnum key,
    RfxVariant old_value, RfxVariant value) {
    int oldValue = old_value.asInt();
    int newValue = value.asInt();
    bool requestPower = getStatusManager()->getBoolValue(RFX_STATUS_KEY_REQUEST_RADIO_POWER, false);
    RLOGD("suggestCapability changed from %d to %d, current requestPower:%d", oldValue, newValue, requestPower);
    if ((newValue == RIL_CAPABILITY_NONE) || (newValue != RIL_CAPABILITY_NONE && requestPower)) {
        sp<RfxMessage> radioRequest =
            RfxMessage::obtainRequest(slotId, RADIO_TECH_GROUP_GSM, RIL_REQUEST_RADIO_POWER);
        radioRequest->getParcel()->writeInt32(1);
        radioRequest->getParcel()->writeInt32(1);
        RfxMainThread::enqueueMessageFront(radioRequest);
    } else {
        RLOGD("Should not change to CAPABILITY_NONE onCapabilityChanged");
    }
    return;
}

void RpRadioController::onCapabilityChanged(RfxStatusKeyEnum key,
    RfxVariant old_value, RfxVariant value) {
    int oldValue = old_value.asInt();
    int newValue = value.asInt();
    RLOGD("capability changed from %d to %d", oldValue, newValue);

    // check if not available
    int radioState = getStatusManager()->getIntValue(RFX_STATUS_KEY_RADIO_STATE, RADIO_STATE_UNAVAILABLE);
    if (radioState == RADIO_STATE_UNAVAILABLE) {
        RLOGD("radio state is unavailable, not update");
        return;
    }
    if (newValue == RIL_CAPABILITY_NONE) {
        RLOGD("unsol: RADIO_STATE_OFF");
        getStatusManager()->setIntValue(RFX_STATUS_KEY_RADIO_STATE, RADIO_STATE_OFF);
    } else {
        RLOGD("unsol: RADIO_STATE_ON");
        getStatusManager()->setIntValue(RFX_STATUS_KEY_RADIO_STATE, RADIO_STATE_ON);
    }
    return;
}

bool RpRadioController::onPreviewMessage(const sp<RfxMessage>& message) {
    if (message->getType() == REQUEST) {
        if (canHandleRequest(message)) {
            return true;
        } else {
            return false;
        }
    } else {
        RLOGD("Not Request, don't do check");
        return true;
    }
}

bool RpRadioController::onCheckIfResumeMessage(const sp<RfxMessage>& message) {
    if (message->getType() == REQUEST) {
        if (canHandleRequest(message)) {
            return true;
        } else {
            return false;
        }
    } else {
        return true;
    }
}

bool RpRadioController::canHandleRequest(const sp<RfxMessage>& message) {
    // check radio state
    int radioState = getStatusManager()->getIntValue(RFX_STATUS_KEY_RADIO_STATE, RADIO_STATE_UNAVAILABLE);
    if (radioState == RADIO_STATE_UNAVAILABLE) {
        RLOGD("Radio state not available, don't process RADIO_POWER");
        return false;
    }

    // check GSM socket
    RfxSocketState socketState = getStatusManager()->getSocketStateValue(RFX_STATUS_KEY_SOCKET_STATE);
    if (!socketState.getSocketState(socketState.SOCKET_GSM)) {
        RLOGD("GSM Socket not connected");
        return false;
    }

    // check CDMA socket & EMDSTATUS if needed
    if (socketState.getIsCdmaSlot()) {
        RLOGD("canHandleRequest, is cdma slot.");
        if (!socketState.getSocketState(socketState.SOCKET_C2K)) {
            RLOGD("CDMA Socket not connected");
            return false;
        }

        char modemStatusIsSend[PROPERTY_VALUE_MAX] = { 0 };
        property_get(emdstatusProperty, modemStatusIsSend, "0");
        if (atoi(modemStatusIsSend) == 0) {
            RLOGD("AT+EMDSTATUS not send");
            return false;
        }
    }
    return true;
}

bool RpRadioController::onHandleRequest(const sp<RfxMessage>& message) {
    int msg_id = message->getId();
    int pToken = message->getPToken();
    RLOGD("RpRadioController: handle request id %d", msg_id);
    switch (msg_id) {
    case RIL_REQUEST_RADIO_POWER:
        requestRadioPower(message);
        break;
    default:
        break;
    }
    return true;
}

void RpRadioController::requestRadioPower(const sp<RfxMessage>& message) {

    // handle with Parcel
    Parcel* parcel = message->getParcel();
    int32_t status;
    int32_t request;
    int32_t token;
    int32_t argc;
    int32_t power;

    parcel->setDataPosition(0);
    status = parcel->readInt32(&request);
    status = parcel->readInt32(&token);
    status = parcel->readInt32(&argc);
    status = parcel->readInt32(&power);
    parcel->setDataPosition(0);

    RLOGD("requestRadioPower, slotId:%d, onOff: %d", slotId, power);
    int suggestedCapability;
    int currentCapability = getStatusManager()->getIntValue(RFX_STATUS_KEY_RADIO_CAPABILITY, RIL_CAPABILITY_NONE);
    // adjust capability if power should be off
    if (power) {
        suggestedCapability = getStatusManager()->
            getIntValue(RFX_STATUS_KEY_SUGGESTED_RADIO_CAPABILITY, RIL_CAPABILITY_NONE);
        getStatusManager()->setBoolValue(RFX_STATUS_KEY_REQUEST_RADIO_POWER, true);
    } else {
        suggestedCapability = RIL_CAPABILITY_NONE;
        getStatusManager()->setBoolValue(RFX_STATUS_KEY_REQUEST_RADIO_POWER, false);
    }
    RLOGD("slotId:%d, currentCapability:%d, suggestedCapability:%d", slotId, currentCapability, suggestedCapability);
    RpCdmaRadioController::getInstance()->registerForCdmaCapability(slotId, CAPABILITY_HAS_CDMA(suggestedCapability));

    // remember as radio Message
    RadioMessage* radioMessage = new RadioMessage(message);
    int msg_id = message->getId();

    // CDMA
    RfxSocketState socketState = getStatusManager()->getSocketStateValue(RFX_STATUS_KEY_SOCKET_STATE);
    if (socketState.getIsCdmaSlot()) {
        bool cdmaPower = (power != 0) ? (RpCdmaRadioController::getInstance()->adjustCdmaRadioPower()) : false;
        RLOGD("CDMA power:%d, slotId:%d", cdmaPower, slotId);
        sp<RfxMessage> cdmaRadioRequest = RfxMessage::obtainRequest(RADIO_TECH_GROUP_C2K, msg_id, message);
        cdmaRadioRequest->getParcel()->writeInt32(1);
        cdmaRadioRequest->getParcel()->writeInt32(cdmaPower ? 1 : 0);
        requestToRild(cdmaRadioRequest);
        radioMessage->isSendToCdma = true;
        RLOGD("radioMessage->isSendToCdma: %s", (radioMessage->isSendToCdma) ? "true" : "false");
    }

    // GSM
    bool gsmPower = CAPABILITY_HAS_GSM(suggestedCapability);
    RLOGD("GSM power:%d, slotId:%d", gsmPower, slotId);
    sp<RfxMessage> gsmRadioRequest = RfxMessage::obtainRequest(RADIO_TECH_GROUP_GSM, msg_id, message);
    gsmRadioRequest->getParcel()->writeInt32(1);
    gsmRadioRequest->getParcel()->writeInt32(gsmPower ? 1 : 0);
    requestToRild(gsmRadioRequest);

    enqueueToMessageQueue(radioMessage);
}

void RpRadioController::enqueueToMessageQueue(RadioMessage* radioMessage) {
    RadioMessage* message = this->MessageQueue;
    RLOGD("enqueue radio message: %d", radioMessage->serial);
    for (;;) {
        if (message->nextMessage != NULL) {
            message = message->nextMessage;
        } else {
            break;
        }
    }
    message->nextMessage = radioMessage;
}


bool RpRadioController::onHandleUrc(const sp<RfxMessage>& message) {

    int msg_id = message->getId();
    RLOGD("RpRadioController: handle urc id %d", msg_id);
    switch (msg_id) {
    case RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED:
        handleRadioStateChanged(message);
        break;
    default:
        break;
    }
    return true;

}

void RpRadioController::handleRadioStateChanged(const sp<RfxMessage>& message) {
    RILD_RadioTechnology_Group source = message->getSource();
    int oldCapability = getStatusManager()->getIntValue(RFX_STATUS_KEY_RADIO_CAPABILITY, RIL_CAPABILITY_NONE);
    int msg_id = message->getId();

    // handle with Parcel
    Parcel* parcel = message -> getParcel();
    int32_t status;
    int32_t request;
    int32_t token;
    int32_t radioState;

    parcel->setDataPosition(0);
    status = parcel->readInt32(&request);
    status = parcel->readInt32(&token);
    status = parcel->readInt32(&radioState);
    parcel->setDataPosition(0);

    RLOGD("urc radioState: %d, oldCapability: %d, source: %d", radioState, oldCapability, source);

    if (source == RADIO_TECH_GROUP_C2K) {
        RpCdmaRadioController::getInstance()->handleRadioStateChanged(radioState);
    } else {
        if (radioState == RADIO_STATE_ON && !CAPABILITY_HAS_GSM(oldCapability)) {
            getStatusManager()->setIntValue(RFX_STATUS_KEY_RADIO_CAPABILITY, oldCapability + RIL_CAPABILITY_GSM_ONLY);
        } else if ((radioState == RADIO_STATE_OFF || radioState == RADIO_STATE_UNAVAILABLE) &&
                CAPABILITY_HAS_GSM(oldCapability)) {
            getStatusManager()->setIntValue(RFX_STATUS_KEY_RADIO_CAPABILITY,
                    oldCapability - RIL_CAPABILITY_GSM_ONLY);
        }
    }

    if (radioState == RADIO_STATE_UNAVAILABLE){
        getStatusManager()->setIntValue(RFX_STATUS_KEY_RADIO_STATE, RADIO_STATE_UNAVAILABLE);
        RLOGD("urc radioState: RADIO_STATE_UNAVAILABLE, just return after set state");
        return;
    }

    // handle state changed if is not unavaialable
    int newCapability = getStatusManager()->getIntValue(RFX_STATUS_KEY_RADIO_CAPABILITY, RIL_CAPABILITY_NONE);
    RLOGD("newCapability: %d", newCapability);
    int oldRadioState = getStatusManager()->getIntValue(RFX_STATUS_KEY_RADIO_STATE, RADIO_STATE_UNAVAILABLE);
    int newRadioState;

    if (newCapability == RIL_CAPABILITY_NONE) {
        RLOGD("unsol: RADIO_STATE_OFF");
        newRadioState = RADIO_STATE_OFF;
        getStatusManager()->setIntValue(RFX_STATUS_KEY_RADIO_STATE, RADIO_STATE_OFF);
    } else {
        RLOGD("unsol: RADIO_STATE_ON");
        newRadioState = RADIO_STATE_ON;
        getStatusManager()->setIntValue(RFX_STATUS_KEY_RADIO_STATE, RADIO_STATE_ON);
    }

    RLOGD("old radio state:%d, new radio state:%d", oldRadioState, newRadioState);

}

bool RpRadioController::onHandleResponse(const sp<RfxMessage>& message) {

    int msg_id = message->getId();
    RLOGD("RpRadioController: handle response id %d", msg_id);
    switch (msg_id) {
    case RIL_REQUEST_RADIO_POWER:
        handleRadioPowerResponse(message);
        break;
    default:
        break;
    }
    return true;

}

bool RpRadioController::handleRadioPowerResponse(const sp<RfxMessage>& message) {
    int serial = message->getPToken();
    // From local serial:minus, Not from local: positive
    RadioMessage* radioMessage = NULL;
    radioMessage = findRadioMessageUsingSerial(serial);

    // More check here
    if (radioMessage == NULL) {
        LOG_ALWAYS_FATAL("radioMessage should not be null here");
    }

    if (message->getSource() == RADIO_TECH_GROUP_C2K) {
        RLOGD("Handle Response From Cdma");
        radioMessage->cdmaResponse = message;
    } else {
        RLOGD("Handle Response From Gsm");
        radioMessage->gsmResponse = message;
    }
    bool ret = checkIfRequestComplete(radioMessage);
    if (ret) {
        // only return to RILJ, if is not local request
        int serial = radioMessage -> serial;
        if (serial >= 0) {
            sendResponse(radioMessage);
        }
        dequeueFromMessageQueue(radioMessage);
    }
    return true;
}

void RpRadioController::dequeueFromMessageQueue(RadioMessage* radioMessage) {
    RadioMessage* message = this->MessageQueue;
    RadioMessage* prevMessage = this->MessageQueue;
    for(;;) {
        if (message == radioMessage) {
            prevMessage->nextMessage = radioMessage->nextMessage;
            RLOGD("dequeue radio message: %d", radioMessage->serial);
            delete(radioMessage);
            break;
        } else if (message == NULL) {
            RLOGD("dequeue radio message: can't find serial %d", radioMessage->serial);
            LOG_ALWAYS_FATAL("dequeueFromMessageQueue: cant find message serial %d",
            radioMessage->serial);
            break;
        } else {
            prevMessage = message;
            message = message->nextMessage;
        }
    }
}

bool RpRadioController::checkIfRequestComplete(RadioMessage* radioMessage){
    bool ret = false;
    bool sendToCdma = radioMessage->isSendToCdma;
    RLOGD("radio message sendToCdma: %s", sendToCdma ? "true" : "false");
    if ((radioMessage->isSendToCdma) == true) {
        if ((radioMessage -> gsmResponse) != NULL && (radioMessage -> cdmaResponse) != NULL ) {
            RLOGD("both side is received, radioMessage %d is complete", radioMessage->serial);
            ret = true;
        }
    } else {
        if ((radioMessage -> gsmResponse) != NULL ) {
            RLOGD("radioMessage %d is complete", radioMessage->serial);
            ret = true;
        }
    }
    return ret;
}

void RpRadioController::sendResponse(RadioMessage* radioMessage) {
    RLOGD("sendResponse to RILJ");
    RIL_Errno rilErrno;
    if (radioMessage->isSendToCdma) {
        RIL_Errno gsmErrno = radioMessage->gsmResponse->getError();
        RIL_Errno cdmaErrno = radioMessage->cdmaResponse->getError();
        rilErrno = combineErrno(gsmErrno, cdmaErrno);
    } else {
        rilErrno = radioMessage->gsmResponse->getError();
    }
    sp<RfxMessage> response =  RfxMessage:: obtainResponse(rilErrno, radioMessage->gsmResponse);
    responseToRilj(response);
}

RIL_Errno RpRadioController::combineErrno(RIL_Errno gsmErrno, RIL_Errno cdmaErrno) {
    RLOGD("Combine Errno(): gsmErrno: %d, cdmaErrno: %d", gsmErrno, cdmaErrno);
    if (gsmErrno == RIL_E_SUCCESS && cdmaErrno == RIL_E_SUCCESS) {
        return RIL_E_SUCCESS;
    } else if (gsmErrno == RIL_E_RADIO_NOT_AVAILABLE || cdmaErrno == RIL_E_RADIO_NOT_AVAILABLE) {
        return RIL_E_RADIO_NOT_AVAILABLE;
    } else {
        return RIL_E_GENERIC_FAILURE;
    }
}

RadioMessage* RpRadioController::findRadioMessageUsingSerial(int serial) {
    RLOGD("We want to find message with serial: %d", serial);
    // TODO: message maybe return null if not exists
    RadioMessage* message = MessageQueue;
    for (;;) {
        if (message->serial == serial && message != MessageQueue) {
            RLOGD("find %d radio message in the queue", message->serial);
            break;
        } else if (message == NULL){
            LOG_ALWAYS_FATAL("Can't find message %d in the queue", serial);
            break;
        } else {
            message = message->nextMessage;
        }
    }
    return message;
}

RadioMessage* RpRadioController::findRadioMessageUsingToken(int token, int source) {
    RLOGD("We want to find message with token: %d, source: %d", token, source);
    // TODO: message maybe return null if not exists
    RadioMessage* message = MessageQueue;
    for (;;) {
        if (source == RADIO_TECH_GROUP_GSM && token == message->gsmToken) {
            RLOGD("find %d gsm token in the queue", token);
            break;
        } else if (source == RADIO_TECH_GROUP_C2K && token == message->cdmaToken) {
            RLOGD("find %d cdma token in the queue", token);
            break;
        } else {
            message = message->nextMessage;
        }
    }
    return message;
}

