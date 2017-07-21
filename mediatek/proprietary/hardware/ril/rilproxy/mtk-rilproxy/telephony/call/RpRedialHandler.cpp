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
#include "assert.h"

#include <cutils/properties.h>
#include <cutils/jstring.h>
#include "RfxMainThread.h"
#include "RpRedialHandler.h"
#include "nw/RpNwUtils.h"
#include "RpCallController.h"

/*****************************************************************************
 * Class RfxController
 *****************************************************************************/
RFX_IMPLEMENT_CLASS("RpRedialHandler", RpRedialHandler, RfxController);

RpRedialHandler::RpRedialHandler() {
    mRedialState = REDIAL_NONE;
    mIsUserHangUping = false;
    mInEmergencyMode = false;
    mMoCall = NULL;
    mDialSource = DIAL_FROM_CS;
    mPreRatMode = 0;
}

RpRedialHandler::~RpRedialHandler() {
    freeMoCallObject();
}

void RpRedialHandler::onInit() {
    RfxController::onInit();  // Required: invoke super class implementation
    RFX_LOG_D(RP_REDIAL_TAG, "onInit !!");

    const int request_id_list[] = {
        RIL_REQUEST_REDIAL,
        RIL_REQUEST_EMERGENCY_REDIAL,
        RIL_REQUEST_C2K_EMERGENCY_REDIAL,
        RIL_REQUEST_EMERGENCY_SESSION_BEGIN,
        RIL_REQUEST_EMERGENCY_SESSION_END,
        RIL_LOCAL_REQUEST_OEM_HOOK_ATCI_INTERNAL,
    };

    const int urc_id_list[] = {
    };

    /* 
    // register callbacks for IMS MO call notify
    getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_DIAL_SOURCE,
        RfxStatusChangeCallback(this, &RpRedialHandler::onDialSourceIms));
    */

    // Diable redial logic in non-Verizon project
    char optr[PROPERTY_VALUE_MAX] = {0};
    property_get("ro.operator.optr", optr, "");
    RFX_LOG_D(RP_REDIAL_TAG, "Current OP code %s", optr);

    // register request & URC id list
    // NOTE. one id can only be registered by one controller
    registerToHandleRequest(request_id_list, sizeof(request_id_list)/sizeof(int), MEDIUM);
    registerToHandleUrc(urc_id_list, sizeof(urc_id_list)/sizeof(int));

    RFX_LOG_D(RP_REDIAL_TAG, "request size:%d urc size:%d!",
        sizeof(request_id_list)/sizeof(int), sizeof(urc_id_list)/sizeof(int));
/*
    // register callbacks for prefered network type update complete
    getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_RAT_SWITCH_DONE,
        RfxStatusChangeCallback(this, &RpRedialHandler::onPreferNwSwitchComplete));
*/
    return;
}

void RpRedialHandler::onDialSourceIms(RfxStatusKeyEnum key,
    RfxVariant old_value, RfxVariant value) {

    setDialSource(DIAL_FROM_IMS);
    return;
}

void RpRedialHandler::onGlobalModeSwitchDone(int slotId) {
    // If Redial Handler is not in emergency mode, 
    // it's impossible to change to global mode.
    REDIAL_DBG_ASSERT(mInEmergencyMode == true);
    REDIAL_DBG_ASSERT(mRedialstate == REDIAL_CHANGE_GLOBAL_MODE);
    REDIAL_DBG_ASSERT(mPreRatMode != 0);

    RFX_LOG_D(RP_REDIAL_TAG, "Change to global mode compelte, redial to MD1");

    sp<RfxMessage> rilRequest = getRedialRequest(RADIO_TECH_GROUP_GSM, false);

    // rilReqeust may be NULL when user hangup during global mode change.
    if (rilRequest != NULL) {
        requestToRild(rilRequest);
        setRedialState(REDIAL_TO_MD1);
    }

    return;
}

void RpRedialHandler::onGlobalModeResumeDone(int slotId) {
    // If Redial Handler is not in emergency mode, 
    // it's impossible to change to global mode.
    REDIAL_DBG_ASSERT(mInEmergencyMode == true);
    REDIAL_DBG_ASSERT(mRedialstate == REDIAL_NONE);
    REDIAL_DBG_ASSERT(mPreRatMode == 0);

    RFX_LOG_D(RP_REDIAL_TAG, "Resume from global mode complete");

    setEmergencyMode(false, slotId);

    return;
}
/*
void RpRedialHandler::onPreferNwSwitchComplete(RfxStatusKeyEnum key,
    RfxVariant old_value, RfxVariant value) {

    if (mRedialState == REDIAL_CHANGE_GLOBAL_MODE) {
        // TODO: already resume global mode possible??
        REDIAL_ASSERT(mPreRatMode != 0);

        RFX_LOG_D(RP_REDIAL_TAG, "Change to global mode compelte, redial to MD1");

        sp<RfxMessage> rilRequest = getRedialRequest(RADIO_TECH_GROUP_GSM, false);
        requestToRild(rilRequest);

        setRedialState(REDIAL_TO_MD1);
    }
    else if (mRedialState == REDIAL_NONE) {
        // If Redial Handler is not in emergency mode, the RAT mode change
        // is not triggered by Redial Handler => ignore it.
        if (mInEmergencyMode == true) {
            REDIAL_ASSERT(mPreRatMode == 0);

            RFX_LOG_D(RP_REDIAL_TAG, "Resume from global mode complete");

            setEmergencyMode(false);
        }
    }
    else {
        // TODO: Unexpected, need error handling
        REDIAL_ASSERT(0);
    }

    return;
}
*/
bool RpRedialHandler::onHandleRequest(const sp<RfxMessage>& msg) {
    int msgId = msg->getId();

    RFX_LOG_D(RP_REDIAL_TAG, "handle request:%s (slot:%d)", requestToString(msgId),
            msg->getSlotId());

    switch (msgId) {
    default:
        RFX_LOG_E(RP_REDIAL_TAG, "unexpected request:%s (slot:%d)", requestToString(msgId),
                msg->getSlotId());
        REDIAL_ASSERT(0);
        break;
    }
    responseToRilj(msg);

    return true;
}

bool RpRedialHandler::onHandleResponse(const sp<RfxMessage>& msg) {
    int msgId = msg->getId();

    RFX_LOG_D(RP_REDIAL_TAG, "handle resp:%s (slot:%d)", requestToString(msgId),
            msg->getSlotId());

    switch (msgId) {
    case RIL_REQUEST_REDIAL:
    case RIL_REQUEST_EMERGENCY_REDIAL:
    case RIL_REQUEST_C2K_EMERGENCY_REDIAL:
    case RIL_REQUEST_EMERGENCY_SESSION_BEGIN:
    case RIL_REQUEST_EMERGENCY_SESSION_END:
        // Local RIL REQUEST send by RpRedialController => Not send to RILJ
        break;

    default:
        RFX_LOG_E(RP_REDIAL_TAG, "unexpected response:%s (slot:%d)", requestToString(msgId),
                msg->getSlotId());
        REDIAL_ASSERT(0);
        break;
    }
    // responseToRilj(msg);

    return true;
}

bool RpRedialHandler::onHandleAtciRequest(const sp<RfxMessage>& msg) {
    int     msgId   = msg->getId();
    int     slotId  = msg->getSlotId();
    bool    result  = false;
    Parcel  *p      = msg->getParcel();

    RFX_LOG_D(RP_REDIAL_TAG, "handle request:%s (slot:%d)", requestToString(msgId),
            msg->getSlotId());

    switch (msgId) {
    case RIL_LOCAL_REQUEST_OEM_HOOK_ATCI_INTERNAL:
        int         dataLength;
        const char  *data;

        dataLength = p->readInt32();
        data = p->readCString();

        RFX_LOG_D(RP_REDIAL_TAG, "Inject AT command %s (length:%d)", data, dataLength);

        if (strncmp(data, "ATD", 3) == 0) {
            result = handleAtciDialRequest(data, dataLength, slotId);
        }
        else if (strncmp(data, "AT+CDV=", 7) == 0) {
            result = handleAtciCdmaDialRequest(data, dataLength, slotId);
        }
        else if ((strncmp(data, "AT+CFUN=", 8) == 0) ||
                 (strncmp(data, "AT+EFUN=", 8) == 0)) {
            result = handleAtciSetRadioPower(data, dataLength, slotId);
        }
        break;

    default:
        RFX_LOG_E(RP_REDIAL_TAG, "unexpected ATCI request:%s (slot:%d)",
                requestToString(msgId), msg->getSlotId());
        REDIAL_ASSERT(0);
        break;
    }

    return result;
}

bool RpRedialHandler::onHandleAtciResponse(const sp<RfxMessage>& msg) {
    int msgId = msg->getId();

    switch (msgId) {
    case RIL_LOCAL_REQUEST_OEM_HOOK_ATCI_INTERNAL:
        break;

    default:
        RFX_LOG_E(RP_REDIAL_TAG, "unexpected ATCI response:%s (slot:%d)",
                requestToString(msgId), msg->getSlotId());
        REDIAL_ASSERT(0);
        break;
    }

    return false;
}

bool RpRedialHandler::onHandleUrc(const sp<RfxMessage>& msg) {
    int msgId = msg->getId();

    RFX_LOG_D(RP_REDIAL_TAG, "handle urc:%s (slot:%d)", requestToString(msgId),
            msg->getSlotId());

    switch (msgId) {
    default:
        RFX_LOG_E(RP_REDIAL_TAG, "unexpected URC:%s", requestToString(msgId));
        REDIAL_ASSERT(0);
        break;
    }

    return true;
}

bool RpRedialHandler::notifyRilRequest(const sp<RfxMessage>& msg) {
    int     msgId = msg->getId();

    RFX_LOG_D(RP_REDIAL_TAG, "handle req:%s (slot:%d)",
            requestToString(msgId), msg->getSlotId());

    if (checkOpCode(12) == false) {
        // In non-Verizon project, we still need to update dialSource state for
        // MO call domain selection. So tracking the MO call status to update
        // dialSource state is necessary.
        if ((msgId != RIL_REQUEST_DIAL) &&
            (msgId != RIL_REQUEST_EMERGENCY_DIAL)) {
            return true;
        }
    }

    switch (msgId) {
        // Notify Redial Handler
        case RIL_REQUEST_DIAL:
        case RIL_REQUEST_EMERGENCY_DIAL:
            handleDialRequest(msg);
            break;

        case RIL_REQUEST_HANGUP:
            Parcel *p;
            int hangUpCallId;

            p = msg->getParcel();
            hangUpCallId = p->readInt32();

            RFX_LOG_D(RP_REDIAL_TAG, "RIL_REQUEST_HANGUP(call ID:%d)", hangUpCallId);

            if ((mMoCall != NULL) &&
                (hangUpCallId == mMoCall->getCallId())) {
                // User cannot hang up call before call id assigned
                REDIAL_ASSERT(mMoCall->getcallId() != 0);

                setUserHangUping(true);
            }
            break;

        case RIL_REQUEST_HANGUP_ALL:
            if (mMoCall != NULL) {
                setUserHangUping(true);
            }
            break;

        default:
            RFX_LOG_E(RP_REDIAL_TAG, "unexpected request:%s (slot:%d)", requestToString(msgId),
                    msg->getSlotId());
            REDIAL_ASSERT(0);
            break;
    }

    RFX_LOG_D(RP_REDIAL_TAG, "notifyRilRequest complete");

    return true;
}

const sp<RfxMessage> RpRedialHandler::notifyRilResponse(const sp<RfxMessage>& msg) {
    int             msgId = msg->getId();
    sp<RfxMessage>  rilResponse = msg;

    // In non-Verizon project, there is no hVoLTE redial scenario. 
    // We don't need to modify the RIL response.
    if (checkOpCode(12) == false) {
        return msg;
    }

    RFX_LOG_D(RP_REDIAL_TAG, "handle response:%s (slot:%d)", requestToString(msgId),
            msg->getSlotId());

    switch (msgId) {
    case RIL_REQUEST_GET_CURRENT_CALLS:
        if (checkDropUrc(msg) == true) {
            RFX_LOG_D(RP_REDIAL_TAG,
                    "Let poll call failed because already redial to other modem");

            // Modified the original RIL response
            rilResponse = RfxMessage::obtainResponse(RIL_E_GENERIC_FAILURE, msg);
        }
        break;

    default:
        RFX_LOG_E(RP_REDIAL_TAG, "unexpected response:%s (slot:%d)", requestToString(msgId),
                msg->getSlotId());
        REDIAL_ASSERT(0);
        break;
    }

    return rilResponse;
}

bool RpRedialHandler::notifyRilUrc(const sp<RfxMessage>& msg) {
    int     msgId = msg->getId();
    bool    isSendToRilj = true;

    if (checkOpCode(12) == false) {
        // In non-Verizon project, RedialHandler still need to handl dialSource
        // to dispatch DIAL request. 
        // We still need to parse CALL_INFO_INDICATION here to update dialSource.
        if (msgId != RIL_UNSOL_CALL_INFO_INDICATION) {
            return true;
        }
    }

    RFX_LOG_D(RP_REDIAL_TAG, "handle URC:%s (slot:%d)", requestToString(msgId),
            msg->getSlotId());

    switch (msgId) {
    case RIL_UNSOL_CALL_INFO_INDICATION:
        isSendToRilj = handleCallInfoInd(msg);

        // CDMAPhone doesn't need CALL_INFO_INDICATION
        if (msg->getSource() == RADIO_TECH_GROUP_C2K) {
            isSendToRilj = false;
        }
        break;

    case RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED:
        isSendToRilj = handleCallStateChanged(msg);
        break;

    case RIL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE:
        isSendToRilj = handleEnterEcbm(msg);
        break;

    case RIL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE:
        isSendToRilj = handleExitEcbm(msg);
        break;

    default:
        RFX_LOG_E(RP_REDIAL_TAG, "unexpected URC:%s (slot:%d)", requestToString(msgId),
                msg->getSlotId());
        REDIAL_ASSERT(0);
        break;
    }

    RFX_LOG_D(RP_REDIAL_TAG, "notifyRilUrc complete(sendToRilj:%s)",
            boolToString(isSendToRilj));

    return isSendToRilj;
}

bool RpRedialHandler::handleDialRequest(const sp<RfxMessage>& msg) {
    char    *callNumber;
    bool    isEcc   = false;
    int     msgId   = msg->getId();
    int     slotId  = msg->getSlotId();
    Parcel  *p      = msg->getParcel();

    if (mMoCall != NULL) {
        RFX_LOG_E(RP_REDIAL_TAG,
                "MoCallContext exist(call ID:%d, isEcc:%s) and received new MO call",
                mMoCall->getCallId(), boolToString(mMoCall->isEccCall()));
        // TODO: error handling
        REDIAL_ASSERT(0);
    }

    if (msgId == RIL_REQUEST_EMERGENCY_DIAL) {
        if (mInEmergencyMode == false) {
            setEmergencyMode(true, slotId);
        }

        isEcc = true;
    }

    mMoCall = new MoCallContext(*p, isEcc, slotId);

    return true;
}

bool RpRedialHandler::handleAtciDialRequest(const char* data, int dataLength, int slotId) {
    int     cmdOffset   = 0;
    bool    isEcc       = false;
    char    *callNumber = NULL;
    char    simNo[PROPERTY_VALUE_MAX] = {0};

    property_get("persist.service.atci.sim", simNo, "0");
    if ((simNo[0] != '0') && (simNo[0] != '1')) {
        RFX_LOG_E(RP_REDIAL_TAG, "User inject MD1 dial command to wrong MD => ignore");
        return false;
    }

    if (mMoCall != NULL) {
        RFX_LOG_E(RP_REDIAL_TAG, "Simultaneous dial request, process late one");
        freeMoCallObject();
    }

    // ATDE: emergency call
    if (strncmp(data, "ATDE", 4) == 0) {
        if (mInEmergencyMode == false) {
            setEmergencyMode(true, slotId);
        }

        isEcc = true;
        cmdOffset = 4;
    }
    // ATD: normal call
    else {
        isEcc = false;
        cmdOffset = 3;
    }

    callNumber = new char[dataLength] {0};

    for (int i = 0; i < dataLength - cmdOffset; i++) {
        if (data[i + cmdOffset] == ';') {
            callNumber[i] = '\0';
            break;
        }
        callNumber[i] = data[i + cmdOffset];
    }

    mMoCall = new MoCallContext(callNumber, isEcc, slotId);
    delete(callNumber);

    return false;
}

bool RpRedialHandler::handleAtciCdmaDialRequest(const char* data, int dataLength, int slotId) {
    int     cmdOffset   = 7;    // 7 charaters for "AT+CDV="
    bool    isEcc       = false;
    char    *callNumber = NULL;
    char    simNo[PROPERTY_VALUE_MAX] = {0};

    property_get("persist.service.atci.sim", simNo, "0");
    if (simNo[0] != '9') {
        RFX_LOG_E(RP_REDIAL_TAG, "User inject MD3 dial command to wrong MD => ignore");
        return false;
    }

    if (mMoCall != NULL) {
        RFX_LOG_E(RP_REDIAL_TAG, "Simultaneous dial request, process late one");
        freeMoCallObject();
    }

    callNumber = new char[dataLength] {0};

    int i;
    // Parse Call number
    for (i = 0; i < dataLength - cmdOffset; i++) {
        if (data[i + cmdOffset] == ',') {
            callNumber[i++] = '\0';
            break;
        }
        callNumber[i] = data[i + cmdOffset];
    }
    // Parse emergency call indicator
    if ((i < dataLength - cmdOffset) && (data[i + cmdOffset] == '1')) {
        isEcc = true;
    }

    mMoCall = new MoCallContext(callNumber, isEcc, slotId);
    delete(callNumber);

    return false;
}

bool RpRedialHandler::handleAtciSetRadioPower(const char* data, int dataLength, int slotId) {
    int radioOn;

    if (checkOpCode(12) == false) {
        // We only need to set radio power in Verizon project.
        return false;
    }

    if ((strncmp(data, "AT+CFUN=0", 9) == 0) ||
        (strncmp(data, "AT+EFUN=0", 9) == 0)) {
        radioOn = 0;
    }
    else {
        radioOn = 1;
    }

    sp<RfxMessage> rilRequest = RfxMessage::obtainRequest(
            slotId,
            RADIO_TECH_GROUP_GSM,
            RIL_REQUEST_RADIO_POWER);

    Parcel *reqParcel = rilRequest->getParcel();

    reqParcel->writeInt32(1);
    reqParcel->writeInt32(radioOn);

    RFX_LOG_D(RP_REDIAL_TAG, "Set RADIO power(radio on:%d)", radioOn);
    RfxMainThread::enqueueMessageFront(rilRequest);

    return true;
}

bool RpRedialHandler::handleCallInfoInd(const sp<RfxMessage>& msg) {
    int     length = 0;
    int     callId, msgType, discCause = 0;
    bool    isSendToRilj = true;
    Parcel  *p = msg->getParcel();
    char*   callInfo[10] = {NULL};

    length = p->readInt32();
    RFX_LOG_D(RP_REDIAL_TAG, "ECPI parameter: %d", length);
    for (int i = 0; i < length; i++) {
        callInfo[i] = strdupReadString(*p);
    }

    RFX_LOG_D(RP_REDIAL_TAG, "ECPI: %s, %s, %s, %s, %s, %s, %s, %s, %s, %s",
            callInfo[0], callInfo[1], callInfo[2], callInfo[3], callInfo[4],
            callInfo[5], callInfo[6], callInfo[7], callInfo[8], callInfo[9]);

    if ((length < 2) || (callInfo[0] == NULL) || (callInfo[1] == NULL)) {
        // TODO: error handling
        RFX_LOG_E(RP_REDIAL_TAG, "ECPI parameter error");
        REDIAL_ASSERT(0);
        return isSendToRilj;
    }

    callId  = atoi(callInfo[0]);
    msgType = atoi(callInfo[1]);

    if (callInfo[9] != NULL) {
        discCause = atoi(callInfo[9]);
    }

    for (int i = 0; i < length; i++) {
        if (callInfo[i] != NULL) {
            free(callInfo[i]);
        }
    }

    RFX_LOG_D(RP_REDIAL_TAG, "CALL_INFO_IND(call ID:%d, msg type:%d, disconnect cause:%d)",
            callId, msgType, discCause);

    // No MO dailed call => no need to parse URC
    if (mMoCall == NULL) {
        RFX_LOG_D(RP_REDIAL_TAG, "No ongoing MO dialing call, send to RILJ directly ");
        return true;
    }

    // Received URC is not belonged to ongoing MO call => no need to parse URC
    if ((msgType != ECPI_MSG_TYPE_CALL_ID_ASSIGN) && (mMoCall->getCallId() != callId)) {
        if (checkDropUrc(msg) == false) {
            RFX_LOG_D(RP_REDIAL_TAG, "MT call, no need to parse CALL_INFO ");
            return true;
        }
    }

    switch (msgType) {
    // Call connect
    case ECPI_MSG_TYPE_CALL_CONNECT:
        isSendToRilj = processCallConnect(msg);
        break;

    // Call ID assign
    case ECPI_MSG_TYPE_CALL_ID_ASSIGN:
        // Received the call ID assign for the redialed call => drop it
        if (mMoCall->getCallId() != 0) {
            REDIAL_DBG_ASSERT(mMoCall->getCallId() == callId);

            RFX_LOG_D(RP_REDIAL_TAG, "Redialed call, no need to notify Call ID assign again");
            isSendToRilj = false;
        }
        else {
            mMoCall->setCallId(callId);
        }
        break;

    // Call disconnect
    case ECPI_MSG_TYPE_CALL_DISCONNECT:
        isSendToRilj = processCallDisconnect(msg, discCause);
        break;

    default:
        if (checkDropUrc(msg) == true) {
            isSendToRilj = false;
        }
        break;
    }

    RFX_LOG_D(RP_REDIAL_TAG, "handleCallInfoInd complete(callID:%d, msg type:%d, sendToRilj:%s)",
           callId, msgType, boolToString(isSendToRilj));

    return isSendToRilj;
}

bool RpRedialHandler::handleCallStateChanged(const sp<RfxMessage>& msg) {
    bool isSendToRilj = true;

    // Send to RILJ if current phone object need to handle CALL STATE CHANGED
    if (checkDropUrc(msg) == true) {
        isSendToRilj = false;
    }

    RFX_LOG_D(RP_REDIAL_TAG, "handleCallStateChanged complete(sendToRilj:%s)",
            boolToString(isSendToRilj));

    return isSendToRilj;
}

bool RpRedialHandler::handleEnterEcbm(const sp<RfxMessage>& msg) {
    bool    isSendToRilj = true;

    RFX_LOG_D(RP_REDIAL_TAG, "handleEnterEcbm complete(sendToRilj:%s)",
            boolToString(isSendToRilj));

    return isSendToRilj;
}

bool RpRedialHandler::handleExitEcbm(const sp<RfxMessage>& msg) {
    bool    isSendToRilj = true;

    if (checkDropUrc(msg) == true) {
        RFX_LOG_E(RP_REDIAL_TAG, "Detect drop URC needed when Exit ECBM");

        REDIAL_ASSERT(0);

        isSendToRilj = false;
    }

    if (mPreRatMode != 0) {
        // Resume RAT mode first, then leave emergency mode
        setRatMode(mPreRatMode, msg->getSlotId());
    }
    else {
        // Leave Emergency Callbaack mode => Also leave emergency mode
        setEmergencyMode(false, msg->getSlotId());
    }

    RFX_LOG_D(RP_REDIAL_TAG, "handleExitEcbm complete(sendToRilj:%s)",
            boolToString(isSendToRilj));

    return isSendToRilj;
}

bool RpRedialHandler::processCallConnect(const sp<RfxMessage>& msg) {
    bool    isSendToRilj = true;

    RFX_LOG_D(RP_REDIAL_TAG, "Call connect(RedialState:%s)",
            redialStateToString(mRedialState));

    if ((mDialSource == DIAL_FROM_IMS) &&
            (msg->getSource() == RADIO_TECH_GROUP_C2K)) {
        // Trigger fake SRVCC
        int srvccStart = 0;
        int srvccComplete = 1;
        Parcel p1, p2, p3;

        RFX_LOG_D(RP_REDIAL_TAG, "IMS call dial success on 1xRTT, trigger fake SRVCC");

        // Notify framework SRVCC start
        p1.writeInt32(srvccStart);

        sp<RfxMessage> urcSrvccStart = RfxMessage::obtainUrc(
                msg->getSlotId(), RIL_UNSOL_SRVCC_STATE_NOTIFY, RADIO_TECH_GROUP_C2K, &p1);

        responseToRilj(urcSrvccStart);

        sleep(1);
        // Notify framework SRVCC complete
        p2.writeInt32(srvccComplete);

        sp<RfxMessage> urcSrvccComplete = RfxMessage::obtainUrc(
                msg->getSlotId(), RIL_UNSOL_SRVCC_STATE_NOTIFY, RADIO_TECH_GROUP_C2K, &p2);

        responseToRilj(urcSrvccComplete);

        // Trigger CDMALTEPhone poll call
        sp<RfxMessage> urcCallStateChanged = RfxMessage::obtainUrc(
                msg->getSlotId(), RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED,
                RADIO_TECH_GROUP_C2K, &p3);

        responseToRilj(urcCallStateChanged);
    }

    setRedialState(REDIAL_NONE);

    RFX_LOG_D(RP_REDIAL_TAG, "processCallConnect complete(sendToRilj:%s)",
            boolToString(isSendToRilj));

    return isSendToRilj;
}

bool RpRedialHandler::processCallDisconnect(const sp<RfxMessage>& msg, int discCause) {
    bool            isSendToRilj = true;
    sp<RfxMessage>  rilRequest;

    RFX_LOG_D(RP_REDIAL_TAG, "Call disconnect(discCause:%d)", discCause);

    if (mIsUserHangUping == true) {
        RFX_LOG_D(RP_REDIAL_TAG, "User cancel dial, let this call failed directly");
        discCause = 0;
    }

    if ((discCause == CALL_DISC_CAUSE_REDIAL_MD1) && (mMoCall->isEccCall() == false)) {
        RFX_LOG_D(RP_REDIAL_TAG,
                "MD3 should not redial normal call to MD1, let this call failed directly");
        discCause = 0;
    }

    switch (discCause) {
        case CALL_DISC_CAUSE_REDIAL_MD1:
            rilRequest = getRedialRequest(RADIO_TECH_GROUP_GSM, false);

            if (rilRequest != NULL) {
                requestToRild(rilRequest);
                isSendToRilj = false;

                setRedialState(REDIAL_TO_MD1);
            }
            break;

        case CALL_DISC_CAUSE_REDIAL_MD3:
            rilRequest = getRedialRequest(RADIO_TECH_GROUP_C2K, true);

            if (rilRequest != NULL) {
                requestToRild(rilRequest);
                isSendToRilj = false;

                setRedialState(REDIAL_TO_MD3);
            }
            break;

        case CALL_DISC_CAUSE_REDIAL_MD3_WITHOUT_TRY_VOLTE:
            rilRequest = getRedialRequest(RADIO_TECH_GROUP_C2K, false);

            if (rilRequest != NULL) {
                requestToRild(rilRequest);
                isSendToRilj = false;

                setRedialState(REDIAL_TO_MD3);
            }
            break;

        case CALL_DISC_CAUSE_NEED_CHANGE_TO_GLOBAL_MODE:

            if (getStatusManager(msg->getSlotId())->
                    getIntValue(RFX_STATUS_KEY_PREFERRED_NW_TYPE) !=
                        REDIAL_NET_TYPE_GLOBAL_MODE) {

                setRatMode(REDIAL_NET_TYPE_GLOBAL_MODE, msg->getSlotId());
                setRedialState(REDIAL_CHANGE_GLOBAL_MODE);

                isSendToRilj = false;
            }
            // Already global mode => redial to MD1 directly
            else {
                rilRequest = getRedialRequest(RADIO_TECH_GROUP_GSM, false);

                if (rilRequest != NULL) {
                    requestToRild(rilRequest);
                    isSendToRilj = false;

                    setRedialState(REDIAL_TO_MD1);
                }
            }
            break;

        // Exactly CALL disconnect => Abort call redial
        default:
            if (mMoCall != NULL) {
                RFX_LOG_D(RP_REDIAL_TAG, "Call disconnect during dialing");

                if (mMoCall->isEccCall() == true) {
                    RFX_LOG_D(RP_REDIAL_TAG, "ECC call disconnect during dialing");

                    if (mPreRatMode != 0) {
                        RFX_LOG_D(RP_REDIAL_TAG, "Resume to %d from global mode", mPreRatMode);
                        // Resume RAT mode
                        setRatMode(mPreRatMode, msg->getSlotId());
                    }
                    else {
                        // ECC call disconnect before connect => emergency session end
                        setEmergencyMode(false, msg->getSlotId());
                    }
                }
            }

            setUserHangUping(false);
            setRedialState(REDIAL_NONE);
            break;
    }

    RFX_LOG_D(RP_REDIAL_TAG, "processCallDisconnect complete(sendToRilj:%s)",
            boolToString(isSendToRilj));

    return isSendToRilj;
}

bool RpRedialHandler::responseToRilj(const sp<RfxMessage>& msg) {
    RFX_LOG_D(RP_REDIAL_TAG, "Send RIL response/URC(%s) to framework",
            requestToString(msg->getId()));

    if ((msg->getSource() == RADIO_TECH_GROUP_C2K) &&
        (msg->getId() == RIL_UNSOL_CALL_INFO_INDICATION)) {
        RFX_LOG_D(RP_REDIAL_TAG, "Drop CALL_INFO_IND because CDMALTEPhone cannot handle this");
        return false;
    }

    RfxController::responseToRilj(msg);

    return true;
}

bool RpRedialHandler::checkDropUrc(const sp<RfxMessage>& msg) {
    bool result = false;
    int  deviceMode = getDeviceMode(msg->getSlotId());

    if (checkOpCode(12) == false) {
        // We only need to drop URC in Verizon hVoLTE project.
        return false;
    }

    switch (mRedialState) {
    case REDIAL_TO_MD1:
        // URC from MD3 => Drop
        if (msg->getSource() == RADIO_TECH_GROUP_C2K) {
            result = true;
        }
        else {
            // URC from MD1, Dial from CDMALTE phone => Drop
            if ((deviceMode  == NWS_MODE_CDMALTE) &&
                (mDialSource == DIAL_FROM_CS)) {
               result = true;
            }
        }
        break;

    case REDIAL_TO_MD3:
        // URC from MD1 => Drop
        if (msg->getSource() == RADIO_TECH_GROUP_GSM) {
            result = true;
        }
        else {
            // URC from MD3, Dial from GSM phone => Drop
            if (deviceMode == NWS_MODE_CSFB) {
                result = true;
            }
        }
        break;

    case REDIAL_CHANGE_GLOBAL_MODE:
        result = true;
        break;

    case REDIAL_NONE:
    default:
        result = false;
        break;
    }

    RFX_LOG_D(RP_REDIAL_TAG, "Drop URC:%s, RedialState:%s, URC from:%s, DeviceMode:%s",
            boolToString(result), redialStateToString(mRedialState),
            radioTechToString(msg->getSource()), deviceModeToString(deviceMode));

    return result;
}

void RpRedialHandler::setRedialState(RedialState state) {
    if (state == REDIAL_NONE) {
        setDialSource(DIAL_FROM_CS);
        freeMoCallObject();
    }

    if (mRedialState == state) {
        return;
    }

    RFX_LOG_D(RP_REDIAL_TAG, "Change RedialState from %s to %s",
            redialStateToString(mRedialState), redialStateToString(state));

    mRedialState = state;
    return;
}

void RpRedialHandler::setDialSource(DialSource source) {
    if (mDialSource == source) {
        return;
    }

    RFX_LOG_D(RP_REDIAL_TAG, "Change DialSource from %s to %s",
            dialSourceToString(mDialSource), dialSourceToString(source));

    mDialSource = source;

    return;
}

void RpRedialHandler::setEmergencyMode(bool emergencyMode, int slotId) {
    sp<RfxMessage> rilRequest;

    if ((mInEmergencyMode == emergencyMode) ||
        (checkOpCode(12) == false)) {
        // No need to update emergencyMode or send AT+EMCS to modem
        // in non-Verizon project.
        return;
    }

    RFX_LOG_D(RP_REDIAL_TAG, "Change mInEmergencyMode from %s to %s",
            boolToString(mInEmergencyMode), boolToString(emergencyMode));

    if (emergencyMode == true) {
        RFX_LOG_D(RP_REDIAL_TAG, "Notify MD1 Emergency session start");

        rilRequest = RfxMessage::obtainRequest(
                slotId,
                RADIO_TECH_GROUP_GSM,
                RIL_REQUEST_EMERGENCY_SESSION_BEGIN);
    }
    else {
        RFX_LOG_D(RP_REDIAL_TAG, "Notify MD1 Emergency session stop");

        rilRequest = RfxMessage::obtainRequest(
                slotId,
                RADIO_TECH_GROUP_GSM,
                RIL_REQUEST_EMERGENCY_SESSION_END);
    }
    requestToRild(rilRequest);

    mInEmergencyMode = emergencyMode;

    return;
}

void RpRedialHandler::setUserHangUping(bool userHangUping) {
    if (mIsUserHangUping == userHangUping) {
        return;
    }

    RFX_LOG_D(RP_REDIAL_TAG, "Change mIsUserHangUping from %s to %s",
            boolToString(mIsUserHangUping), boolToString(userHangUping));

    mIsUserHangUping = userHangUping;

    return;
}

void RpRedialHandler::setRatMode(int ratMode, int slotId) {
    sp<RfxAction> action;
    RpNwRatController* nwRatController =
            (RpNwRatController *)findController(slotId, RFX_OBJ_CLASS_INFO(RpNwRatController));

    // Change to Global mode
    if (ratMode == REDIAL_NET_TYPE_GLOBAL_MODE) {
        mPreRatMode = getStatusManager(slotId)->getIntValue(RFX_STATUS_KEY_PREFERRED_NW_TYPE);

        RFX_LOG_D(RP_REDIAL_TAG, "Change to global mode(RAT mode:%d -> %d)",
                mPreRatMode, REDIAL_NET_TYPE_GLOBAL_MODE);

        action = new RfxAction1<int>(this, &RpRedialHandler::onGlobalModeSwitchDone, slotId);
        nwRatController->setPreferredNetworkType(REDIAL_NET_TYPE_GLOBAL_MODE, action);
    }
    // Resume to original RAT mode
    else {
        RFX_LOG_D(RP_REDIAL_TAG, "Resume from global mode to original RAT mode");

        mPreRatMode = 0;

        action = new RfxAction1<int>(this, &RpRedialHandler::onGlobalModeResumeDone, slotId);
        nwRatController->setPreferredNetworkType(REDIAL_NET_TYPE_RESUME_RAT_MODE, action);
    }

    return;
}

sp<RfxMessage> RpRedialHandler::getRedialRequest(
        RILD_RadioTechnology_Group dialTarget, bool isVolteTried) {
    int requestId;

    if (mIsUserHangUping == true) {
        RFX_LOG_D(RP_REDIAL_TAG, "User is hanguping, not to perform redial");
        return NULL;
    }

    if (mMoCall == NULL) {
        RFX_LOG_D(RP_REDIAL_TAG, "mMoCall is NULL pointer, not to perform redial");
        return NULL;
    }

    if (mMoCall->isEccCall() == true) {
        if (dialTarget == RADIO_TECH_GROUP_GSM) {
            requestId = RIL_REQUEST_EMERGENCY_REDIAL;
            RpCallController::updatePhoneIdToMDIdProperty(mMoCall->getSlotId(), "MD1");
        }
        else {
            requestId = RIL_REQUEST_C2K_EMERGENCY_REDIAL;
            RpCallController::updatePhoneIdToMDIdProperty(mMoCall->getSlotId(), "MD3");
        }
    }
    else {
        if (dialTarget == RADIO_TECH_GROUP_GSM) {
            RFX_LOG_E(RP_REDIAL_TAG, "We don't support redial to MD1 now");
            return NULL;
        }
        else {
            requestId = RIL_REQUEST_REDIAL;
            RpCallController::updatePhoneIdToMDIdProperty(mMoCall->getSlotId(), "MD3");
        }
    }

    sp<RfxMessage> rilRequest =
        RfxMessage::obtainRequest(mMoCall->getSlotId(), dialTarget, requestId);

    Parcel *newReqParcel = rilRequest->getParcel();
    Parcel *oldReqParcel = mMoCall->getDialParcel();

    // Copy dialed reqeust parcel to new request parcel
    newReqParcel->write(oldReqParcel->data(), oldReqParcel->dataSize());

    // redial mode
    newReqParcel->writeInt32(isVolteTried);

    // call ID
    newReqParcel->writeInt32(mMoCall->getCallId());
/*
    RFX_LOG_D(RP_REDIAL_TAG, "old parcel data size:%d, new parcel data size:%d", 
            oldReqParcel->dataSize(), newReqParcel->dataSize());

    for (int i = 0; i < newReqParcel->dataSize(); i++) {
        RFX_LOG_D(RP_REDIAL_TAG, "%d, %d", i, *(newReqParcel->data() + i));
    }
*/
    RFX_LOG_D(RP_REDIAL_TAG,
            "get redial request %s from old parcel(call ID:%d, dial to:%s, isVolteTried:%d)",
            requestToString(requestId), mMoCall->getCallId(), 
            radioTechToString(dialTarget), isVolteTried);

    return rilRequest;
}

int RpRedialHandler::getDeviceMode(int slotId) {
    int mode = getStatusManager(slotId)->getIntValue(RFX_STATUS_KEY_NWS_MODE, NWS_MODE_CSFB);
    // RFX_LOG_D(RP_REDIAL_TAG, "Get device mode=%d", mode);

    return mode;
}

DialSource RpRedialHandler::getDialSource() {
    return mDialSource;
}

RedialState RpRedialHandler::getRedialState() {
    return mRedialState;
}

void RpRedialHandler::freeMoCallObject() {
    if (mMoCall != NULL) {
        RFX_LOG_D(RP_REDIAL_TAG, "free mMoCall object");

        free(mMoCall);
        mMoCall = NULL;
    }

    mMoCall = NULL;
    return;
}

char* RpRedialHandler::boolToString(bool value) {
    char *result = NULL;

    if (value == true) {
        result = "true";
    }
    else {
        result = "false";
    }

    return result;
}

char* RpRedialHandler::redialStateToString(RedialState state) {
    char *result = NULL;

    switch (state) {
    case REDIAL_NONE:
        result = "REDIAL_NONE";
        break;

    case REDIAL_TO_MD1:
        result = "REDIAL_TO_MD1";
        break;

    case REDIAL_TO_MD3:
        result = "REDIAL_TO_MD3";
        break;

    case REDIAL_CHANGE_GLOBAL_MODE:
        result = "REDIAL_CHANGE_GLOBAL_MODE";
        break;

    default:
        break;
    }

    return result;
}

char* RpRedialHandler::dialSourceToString(DialSource value) {
    char *result = NULL;

    switch (value) {
    case DIAL_FROM_CS:
        result = "DIAL_FROM_CS";
        break;

    case DIAL_FROM_IMS:
        result = "DIAL_FROM_IMS";
        break;

    default:
        break;
    }

    return result;
}

char* RpRedialHandler::deviceModeToString(int deviceMode) {
    char *result = NULL;

    switch (deviceMode) {
    case NWS_MODE_CDMALTE:
        result = "CDMALTE";
        break;

    case NWS_MODE_CSFB:
        result = "CSFB";
        break;

    default:
        break;
    }

    return result;
}

char* RpRedialHandler::radioTechToString(int radio) {
    char *result = NULL;

    switch (radio) {
    case RADIO_TECH_GROUP_GSM:
        result = "GSM";
        break;

    case RADIO_TECH_GROUP_C2K:
        result = "C2K";
        break;

    default:
        break;
    }

    return result;
}

char* RpRedialHandler::strdupReadString(Parcel &p) {
    size_t stringlen;
    const char16_t *s16;

    s16 = p.readString16Inplace(&stringlen);

    // RFX_LOG_D(RP_REDIAL_TAG, "%s, length:%d", s16, stringlen);
    return strndup16to8(s16, stringlen);
}

bool RpRedialHandler::checkOpCode(int opCode) {
    bool result = false;
    char optr[PROPERTY_VALUE_MAX] = {0};

    property_get("ro.operator.optr", optr, "");

    switch (opCode) {
    case 12:
        if (strcmp(optr, "OP12") == 0) {
            result = true;
        }

        // Always return true in IT phase
        // result = true;
        break;

    default:
        break;
    }
/*
    RFX_LOG_D(RP_REDIAL_TAG, "Is current OP code OP%d?? (current:%s, result:%s)",
            opCode, optr, boolToString(result));
*/
    return result;
}

MoCallContext::MoCallContext(char *callNumber, bool isEcc, int slotId) {
    RFX_LOG_D(RP_REDIAL_TAG, "MoCallContext Constructor(CallNumber:%s, isEcc:%s, slotId:%d)",
            callNumber, RpRedialHandler::boolToString(isEcc), slotId);

    mDialParcel.writeCString(callNumber);
    mDialParcel.writeInt32(0);
    mDialParcel.writeInt32(0);

    mIsEmergencyCall = isEcc;
    mCallId = 0;
    mSlotId = slotId;

    return;
}

MoCallContext::MoCallContext(Parcel& p, bool isEcc, int slotId) {
    RFX_LOG_D(RP_REDIAL_TAG, "MoCallContext Constructor(isEcc:%s, slotId:%d)",
            RpRedialHandler::boolToString(isEcc), slotId);
/*
    for (int i = 0; i < p.dataSize(); i++) {
        RFX_LOG_D(RP_REDIAL_TAG, "%d, %d", i, *(p.data() + i));
    }
*/
    mDialParcel.write(p.data() + REDIAL_RIL_PARCEL_HEADER, 
                      p.dataSize() - REDIAL_RIL_PARCEL_HEADER);

    mIsEmergencyCall = isEcc;
    mCallId = 0;
    mSlotId = slotId;

    return;
}

Parcel* MoCallContext::getDialParcel() {
    return &mDialParcel;
}

int MoCallContext::getCallId() {
    return mCallId;
}

int MoCallContext::getSlotId() {
    return mSlotId;
}

void MoCallContext::setCallId(int callId) {
    RFX_LOG_D(RP_REDIAL_TAG, "Set call id from %d to %d", mCallId, callId);

    mCallId = callId;
    return;
}

bool MoCallContext::isEccCall() {
    return mIsEmergencyCall;
}
