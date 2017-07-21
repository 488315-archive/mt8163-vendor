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
#include <cutils/jstring.h>
#include "RpModemController.h"
#include "RfxDefs.h"
#include "RfxLog.h"
#include "RfxStatusDefs.h"
#include "RfxStatusManager.h"
#include "ril.h"
#include "RadioConstants.h"
#include <utils/Log.h>
#include "RfxSocketState.h"
#include "nw/RpNwRatController.h"
#define LOG_TAG "RpModemController"

#define ARRAY_LENGTH(array) (sizeof(array)/sizeof(array[0]))

/*****************************************************************************
 * Class RpModemController
 *****************************************************************************/

const char PROPERTY_NET_CDMA_MDMSTAT[25] = "net.cdma.mdmstat";
const char PROPERTY_CAPABILITY_SWITCH[25] = "persist.radio.simswitch";
const char MTK_C2KWP_P2_SUPPORT[25] = "ro.mtk.c2k.slot2.support";
const char MTK_IRAT_SUPPORT[25] = "ro.c2k.irat.support";

static const int request[] = {
    RIL_REQUEST_MODEM_POWERON,
    RIL_REQUEST_MODEM_POWEROFF,
    RIL_REQUEST_CONFIG_MODEM_STATUS
};

static RpModemController* controllerInstance = NULL;

static const int MODEM_POWER_OFF = 0;
static const int MODEM_POWER_ON = 1;

RFX_IMPLEMENT_CLASS("RpModemController", RpModemController, RfxController);

TimerHandle RpModemController::s_cmd_startd_th = 0;// a invalid timer handle

void RpModemController::onInit() {
    if (controllerInstance == NULL) {
        //super.init
        RLOGD("First Init()");
        RfxController::onInit();
        cdmaSlotId = getStatusManager(RFX_SLOT_ID_UNKNOWN)->getIntValue(
                    RFX_STATUS_KEY_CDMA_SOCKET_SLOT, 0);
        controllerInstance = this;
        RLOGD("array size %d", ARRAY_LENGTH(request));
        for (int i = 0; i < SIM_COUNT; i++) {
            registerToHandleRequest(i, request, ARRAY_LENGTH(request));
        }
        initModemMessages();
        getNonSlotScopeStatusManager()->registerStatusChanged(RFX_STATUS_KEY_CDMA_SOCKET_SLOT,
                RfxStatusChangeCallback(this, &RpModemController::onCdmaSocketSlotChange));
        action = NULL;
        cdmaLteModeControllerToken = -1;
        for (int i = 0; i< RFX_SLOT_COUNT; i++) {
            getStatusManager(i)->registerStatusChangedEx(RFX_STATUS_KEY_SOCKET_STATE,
                    RfxStatusChangeCallbackEx(this, &RpModemController::onSocketStateChange));
        }
    } else {
        RLOGD("Already Init, don't init again");
    }
}

void RpModemController::onDeinit() {
    for (int i = 0; i< RFX_SLOT_COUNT; i++) {
        getStatusManager(i)->unRegisterStatusChangedEx(RFX_STATUS_KEY_SOCKET_STATE,
                RfxStatusChangeCallbackEx(this, &RpModemController::onSocketStateChange));
    }
    RfxController::onDeinit();
}


void RpModemController::initModemMessages() {
    // TODO: only for DSDS, should handle with DSDA if necessary
    int modemCount = 1;
    modemMessages = new ModemMessage[SIM_COUNT];
    modemPower = new int[modemCount];

    for (int i = 0; i < SIM_COUNT; i++) {
        modemMessages[i] = ModemMessage();
        //TODO: only for DSDS part now, should handle with DSDA if necessary
        modemMessages[i].modemId = 0;
        modemMessages[i].connectWithCdmaModem = (cdmaSlotId == i) ? true : false;
        modemMessages[i].slotId = i;
        modemMessages[i].action = NULL;
        modemMessages[i].fromCdmaLteModeController = false;
    }
    for (int i = 0; i < SIM_COUNT; i++) {
        RLOGD("modemMessage %d is %s", i, modemMessages[i].connectWithCdmaModem ? "true" : "false");
    }

    for (int i = 0; i < modemCount; i++) {
        modemPower[i] = MODEM_POWER_ON;
    }
}

RpModemController::RpModemController() {
}


RpModemController::~RpModemController() {
    delete(modemMessages);
    delete(modemPower);
}


RpModemController* RpModemController::getInstance() {
    if (controllerInstance != NULL) {
        return controllerInstance;
    } else {
        // TODO: should initialize
        return controllerInstance;
    }
}

bool RpModemController::onPreviewMessage(const sp<RfxMessage>& message) {
    if (message -> getType() == REQUEST) {
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

bool RpModemController::onCheckIfResumeMessage(const sp<RfxMessage>& message) {
    if (message -> getType() == REQUEST) {
        if (canHandleRequest(message)) {
            return true;
        } else {
            return false;
        }
    } else {
        return true;
    }
}

bool RpModemController::canHandleRequest(const sp<RfxMessage>& message) {
    int slotId = message -> getSlotId();
    if(modemMessages[slotId].originalMessage == NULL) {
        bool allSocketConnect = true;
        for (int i=0; i<SIM_COUNT; i++) {
            RfxSocketState socketState = getStatusManager(i)->getSocketStateValue(
                    RFX_STATUS_KEY_SOCKET_STATE);
            allSocketConnect = allSocketConnect && socketState.getSocketState(
                        socketState.SOCKET_GSM);
            if (socketState.getIsCdmaSlot()) {
                 allSocketConnect = allSocketConnect && socketState.getSocketState(
                        socketState.SOCKET_C2K);
            }
        }
        RLOGD("RpModemController: canHandleRequest %d", allSocketConnect);
        return allSocketConnect;
        //return true;
    } else {
        RLOGD("RpModemController: canHandleRequest %d", false);
        return false;
    }
}

bool RpModemController::onHandleRequest(const sp<RfxMessage>& message) {

    int msg_id = message -> getId();
    RLOGD("RpModemController: handle request id %d", msg_id);

    switch (msg_id) {
    case RIL_REQUEST_MODEM_POWERON:
        requestModemPower(MODEM_POWER_ON, message);
        break;
    case RIL_REQUEST_MODEM_POWEROFF:
        requestModemPower(MODEM_POWER_OFF, message);
        break;
    default:
        break;
    }
    return true;
}

void RpModemController::requestModemPower(int power, const sp<RfxMessage>& message) {
    int msg_id = message -> getId();
    int pToken = message -> getPToken();
    int slotId = message -> getSlotId();
    RpNwRatController* nwRatController = (RpNwRatController *) findController(
            slotId, RFX_OBJ_CLASS_INFO(RpNwRatController));
    if (power == MODEM_POWER_ON) {
        nwRatController->clearSuggetRadioCapability();
        RLOGD("Modem Power on, clear last radio capability.");
    }
    RLOGD("Modem Power %s is apply on %d with serial %d", (power == 1) ? "on" : "off,", slotId,
            pToken);

    int modemId = modemMessages[slotId].modemId;
    if (power == modemPower[modemId]) {
        RLOGD("Modem Power is already applied, just return OK");
        sp<RfxMessage> response =  RfxMessage:: obtainResponse(RIL_E_SUCCESS, message);
        responseToRilj(response);
        return;
    } else {
        // Modem message will only do once at a time
        ModemMessage* modemMessage = &modemMessages[slotId];
        modemMessage -> serial = pToken;
        modemMessage -> originalMessage = message;
        if (message->getToken() == cdmaLteModeControllerToken) {
            RLOGD("MD off by CdmaLteModeController ");
            modemMessage->action = action;
            modemMessage->fromCdmaLteModeController = true;
        }

        //create gsm Modem Request
        RLOGD("Create Gsm Modem Request with serial %d", pToken);
        sp<RfxMessage> gsmModemRequest = RfxMessage::obtainRequest(RADIO_TECH_GROUP_GSM, msg_id,
                message);
        modemMessage -> gsmRequest = gsmModemRequest;
        requestToRild(gsmModemRequest);
        RLOGD("Connect with Cdma %s", (modemMessage -> connectWithCdmaModem) ? "true" : "false");
        //if (modemMessage -> connectWithCdmaModem) {
            //create cdma Modem Request
            RLOGD("Create Cdma Modem Request with serial %d", pToken);
            sp<RfxMessage> cdmaModemRequest = RfxMessage::obtainRequest(RADIO_TECH_GROUP_C2K,
                    msg_id, message);
            modemMessage -> cdmaRequest = cdmaModemRequest;
            requestToRild(cdmaModemRequest);
        //}
        modemPower[modemId] = power;
    }
}

bool RpModemController::onHandleUrc(const sp<RfxMessage>& message) {
    int msg_id = message -> getId();
    RLOGD("RpModemController: handle urc id %d", msg_id);
    return true;
}

bool RpModemController::onHandleResponse(const sp<RfxMessage>& message) {

    int msg_id = message -> getId();
    RLOGD("RpModemController: handle response id %d", msg_id);

    switch (msg_id) {
    case RIL_REQUEST_MODEM_POWERON:
        handleModemPowerResponse(message);
        break;
    case RIL_REQUEST_MODEM_POWEROFF:
        handleModemPowerResponse(message);
        break;
    default:
        break;
    }
    return true;
}

bool RpModemController::handleModemPowerResponse(const sp<RfxMessage>& message) {
    int slotId = message -> getSlotId();
    RIL_Errno rilErrno = message -> getError();
    ModemMessage* modemMessage = NULL;
    // Find modem message using ptoken if is from cdma
    for (int i = 0; i < SIM_COUNT; i++) {
        if (modemMessages[i].cdmaRequest == NULL) {
            RLOGD("modem message %d: cdma Request is null", i);
        } else {
            RLOGD("modem message %d: cdma Request ptoken is %d", i, modemMessages[i].cdmaRequest->getPToken());
        }
    }
    if (message->getSource() == RADIO_TECH_GROUP_C2K) {
        for (int i = 0; i < SIM_COUNT; i++) {
            if (modemMessages[i].cdmaRequest != NULL
                    && modemMessages[i].cdmaRequest->getPToken() == message->getPToken()) {
                modemMessage = &modemMessages[i];
                RLOGD("Find Modem message %d using message pToken %d", i, message->getPToken());
                break;
            }
        }
    } else {
        modemMessage = &modemMessages[slotId];
        RLOGD("Using Modem message slot Id %d", slotId);
    }

    // Couldn't find the right modem message here, so we stop here
    if (modemMessage == NULL) {
        LOG_ALWAYS_FATAL("Can't find modem message here");
        return false;
    }

    if (message->getSource() == RADIO_TECH_GROUP_C2K) {
        RLOGD("Handle Response From Cdma");
        if (rilErrno != RIL_E_SUCCESS) {
            RLOGD("cdma modem poower request fail, try again");
            requestToRild(modemMessage -> cdmaRequest);
            return true;
        } else {
            RLOGD("cdma modem power request sucess");
            modemMessage -> cdmaResponse = message;
        }
    } else {
        RLOGD("Handle Response From Gsm");
        if (rilErrno != RIL_E_SUCCESS) {
            RLOGD("gsm modem poower request fail, try again");
            requestToRild(modemMessage -> gsmRequest);
            return true;
        } else {
            RLOGD("gsm modem power request sucess");
            modemMessage -> gsmResponse = message;
        }
    }
    bool ret = checkIfRequestComplete(modemMessage);
    if (ret) {
        if (modemMessage->fromCdmaLteModeController) {
            RLOGD("Request Complete, excute action (request from fromCdmaLteModeController)");
            modemMessage->action->act();
            action = NULL;
            cdmaLteModeControllerToken = -1;
        } else {
            sp<RfxMessage> response =  RfxMessage:: obtainResponse(RIL_E_SUCCESS,
                    modemMessage -> originalMessage);
            RLOGD("Request Complete, send response");
            responseToRilj(response);
        }
        modemMessage -> resetModemMessage();
    }
    return true;
}

bool RpModemController::checkIfRequestComplete(ModemMessage* modemMessage){
    bool ret = false;
    bool sendToCdma = modemMessage -> connectWithCdmaModem;
    RLOGD("modem message sendToCdma: %s", sendToCdma ? "true" : "false");
    if (sendToCdma == true) {
        if ((modemMessage -> gsmResponse) != NULL && (modemMessage -> cdmaResponse) != NULL ) {
            RLOGD("both side is received, modemMessage %d is complete", modemMessage -> serial);
            ret = true;
        }
    } else {
        if ((modemMessage -> gsmResponse) != NULL ) {
            RLOGD("modemMessage %d is complete", modemMessage -> serial);
            ret = true;
        }
    }
    return ret;
}

// set cdma_slot when configModemStatus
void RpModemController::configModemStatus(int cdmaSlot) {
    char md3State[PROPERTY_VALUE_MAX] = {0};
    property_get(PROPERTY_NET_CDMA_MDMSTAT, md3State, "not ready");
    RLOGD("configModemStatus, net.cdma.mdmstat: %s", md3State);

    if ((strncmp(md3State, "ready", 5) != 0)) {
        cdmaSlotId = cdmaSlot;
        s_cmd_startd_th = RfxTimer::start(
                RfxCallback0(this, &RpModemController::onConfigModemTimer),
                ms2ns(INITIAL_RETRY_INTERVAL_MSEC));
        return;
    } else {
        if (cdmaSlotId != 0) {
            RfxTimer::stop(s_cmd_startd_th);
            s_cmd_startd_th = 0;
            cdmaSlotId = 0;
        }
    }

    char temp[PROPERTY_VALUE_MAX] = {0};
    int capability = 1;
    property_get(PROPERTY_CAPABILITY_SWITCH, temp, "1");
    RLOGD("configModemStatus, persist.radio.simswitch: %s", temp);
    capability = atoi(temp);

    int cardType1 = 0;
    int cardType2 = 0;
    for (int i = 0; i < RFX_SLOT_COUNT; i++) {
        if (i == 0) {
            cardType1 = getStatusManager(i)->getIntValue(RFX_STATUS_KEY_CARD_TYPE, 0);
        } else if (i == 1) {
            cardType2 = getStatusManager(i)->getIntValue(RFX_STATUS_KEY_CARD_TYPE, 0);
        } else {
            RLOGD("configModemStatus, not support slot: %d", i);
            break;
        }
    }
    RLOGD( "configModemStatus, slot number: %d, cardType1: %d, cardType2: %d",
            RFX_SLOT_COUNT, cardType1, cardType2);

    int modemStatus = 2;
    int remoteSimProtocol = 1;

    //test mode not implement here

    while (1) {
        property_get(MTK_C2KWP_P2_SUPPORT, temp, "");
        if (strncmp(temp, "1", 1) == 0) {
            RLOGD( "configModemStatus: Solution2");
            // case1: slot1 CDMA
            if ((is4GCdmaCard(cardType1) || is3GCdmaCard(cardType1))
                && (is4GCdmaCard(cardType2) || is3GCdmaCard(cardType2))) {
                // 2 CDMA SIM, C2K modem always follow 3/4G slot
                remoteSimProtocol = 1;
                if (capability == 1) {
                    if (is4GCdmaCard(cardType1)) {
                        modemStatus = 2;
                    } else {
                        modemStatus = 1;
                    }
                } else {
                    if (is4GCdmaCard(cardType2)) {
                        modemStatus = 2;
                    } else {
                        modemStatus = 1;
                    }
                }
                if (!isCdmaIratSupport()) {
                    modemStatus = 1;
                }
                break;
            }

            // case2: slot1 CDMA && slot2 non-CDMA
            if (is4GCdmaCard(cardType1) &&
                (isGsmCard(cardType2) ||
                isNonCard(cardType2))) {
                // slot1 CT 4G, slot2 any
                if (!isCdmaIratSupport()) {
                    modemStatus = 1;
                } else {
                    modemStatus = 2;
                }

                if (capability == 1) {
                    remoteSimProtocol = 1;
                } else {
                    remoteSimProtocol = 2;
                }
                break;
            } else if (is3GCdmaCard(cardType1) &&
                (isGsmCard(cardType2) ||
                isNonCard(cardType2))) {
                // slot1 CT 3G, slot2 any
                modemStatus = 1;
                if (capability == 1) {
                    remoteSimProtocol = 1;
                } else {
                    remoteSimProtocol = 2;
                }
                break;
            }

            // case2: slot2 CDMA && slot1 non-CDMA
            if (isGsmCard(cardType1) || isNonCard(cardType1)) {
                if (is4GCdmaCard(cardType2)) {
                    // slot1 GSM or N/A, slot2 CT 4G
                    if (!isCdmaIratSupport()) {
                        modemStatus = 1;
                    } else {
                        modemStatus = 2;
                    }

                    if (capability == 1) {
                        remoteSimProtocol = 2;
                    } else {
                        remoteSimProtocol = 1;
                    }
                    break;
                } else if (is3GCdmaCard(cardType2)) {
                    // slot1 GSM or N/A, slot2 CT 3G
                    modemStatus = 1;
                    if (capability == 1) {
                        remoteSimProtocol = 2;
                    } else {
                        remoteSimProtocol = 1;
                    }
                    break;
                }
            }

            // case3: non-CDMA
            if (isGsmCard(cardType1) && isGsmCard(cardType2)) {
                // slot1 GSM, slot2 GSM
                modemStatus = 0;
                remoteSimProtocol = 1;
                break;
            } else if (isGsmCard(cardType1) && isNonCard(cardType2)) {
                // slot1 GSM, slot2 N/A
                modemStatus = 0;
                remoteSimProtocol = 1;
                break;
            } else if (isNonCard(cardType1) && isGsmCard(cardType2)) {
                // slot1 N/A, slot2 GSM
                modemStatus = 0;
                remoteSimProtocol = 1;
                break;
            }
        } else {
            RLOGD( "configModemStatus: Solution1/OP09");
            bool md3AccessProtocol2 = (capability != 1);//ES2g = 2
            if (isNonCard(cardType1)) {
                // no card
                RLOGD( "configModemStatus: no card");
                modemStatus = 1;
                remoteSimProtocol = md3AccessProtocol2 ? 2 : 1;
            } else if (isGsmCard(cardType1)) {
                // GSM only
                RLOGD( "configModemStatus: GSM only");
                modemStatus = 0;
                remoteSimProtocol = 1;
            } else if (is3GCdmaCard(cardType1)) {
                //CT 3G
                RLOGD( "configModemStatus: CT 3G");
                modemStatus = 1;
                remoteSimProtocol = md3AccessProtocol2 ? 2 : 1;
            } else if (is4GCdmaCard(cardType1)) {
                // CT 4G
                RLOGD( "configModemStatus: CT 4G");
                if (!isCdmaIratSupport()) {
                    modemStatus = 1;
                } else {
                    modemStatus = 2;
                }
                remoteSimProtocol = md3AccessProtocol2 ? 2 : 1;
            } else {
                //other case, may not happen!
                RLOGD( "configModemStatus: other case, may not happen!");
                break;
            }
        }
        break;
    }

    RLOGD( "configModemStatus: (modemStatus, remoteSimProtocol, cdma_slot)"
        ":(%d, %d, %d)", modemStatus, remoteSimProtocol, cdmaSlot);

    for (int i = 0; i < RFX_SLOT_COUNT; i++) {
        sp<RfxMessage> request = RfxMessage::obtainRequest(i,
                                             RADIO_TECH_GROUP_GSM,
                                             RIL_REQUEST_CONFIG_MODEM_STATUS);
        Parcel* p = request->getParcel();
        p->writeInt32(2);
        p->writeInt32(modemStatus);
        p->writeInt32(remoteSimProtocol);

        requestToRild(request);
        RLOGD( "configModemStatus: for slot[%d] gsm rild", i);
    }

    sp<RfxMessage> request = RfxMessage::obtainRequest(cdmaSlot,
                                         RADIO_TECH_GROUP_C2K,
                                         RIL_REQUEST_CONFIG_MODEM_STATUS);
    Parcel* p = request->getParcel();
    p->writeInt32(2);
    p->writeInt32(modemStatus);
    p->writeInt32(remoteSimProtocol);
    this -> cdmaModemStatusRequest = request;
    this -> sendCdmaModemStatus();

    RLOGD( "configModemStatus: for slot[%d] cdma rild", cdmaSlot);
}


void RpModemController::sendCdmaModemStatus() {
    bool sendSuccess = this -> requestToRild(cdmaModemStatusRequest);
    if (sendSuccess == false) {
        RLOGD("retry emdstatus");
        RfxTimer::start(
                RfxCallback0(this, & RpModemController::sendCdmaModemStatus),
                ms2ns(2000));
        return;
    } else {
        RLOGD("success");
        return;
    }

}

void RpModemController::onConfigModemTimer() {
    configModemStatus(cdmaSlotId);
}

bool RpModemController::is4GCdmaCard(int cardType) {
    if ((cardType & RFX_CARD_TYPE_USIM) > 0 &&
        ((cardType & RFX_CARD_TYPE_CSIM) > 0 ||
        (cardType & RFX_CARD_TYPE_RUIM) > 0)) {
        return true;
    }
    return false;
}

bool RpModemController::is3GCdmaCard(int cardType) {
    if ((cardType & RFX_CARD_TYPE_SIM) == 0 &&
        (cardType & RFX_CARD_TYPE_USIM) == 0 &&
        ((cardType & RFX_CARD_TYPE_CSIM) > 0 ||
        (cardType & RFX_CARD_TYPE_RUIM) > 0)) {
        return true;
    }
    return false;
}

bool RpModemController::isGsmCard(int cardType) {
    if ((cardType & RFX_CARD_TYPE_CSIM) == 0 &&
        (cardType & RFX_CARD_TYPE_RUIM) == 0 &&
        ((cardType & RFX_CARD_TYPE_SIM) > 0 ||
        (cardType & RFX_CARD_TYPE_USIM) > 0)) {
        return true;
    }
    return false;
}

bool RpModemController::isNonCard(int cardType) {
    if (cardType == 0) {
        return true;
    }
    return false;
}

bool RpModemController::isCdmaIratSupport() {
    char temp[PROPERTY_VALUE_MAX] = {0};
    property_get(MTK_IRAT_SUPPORT, temp, "");
    if (strncmp(temp, "1", 1) == 0) {
        return true;
    } else {
        return false;
    }
}

void RpModemController::onCdmaSocketSlotChange(RfxStatusKeyEnum key,
    RfxVariant old_value, RfxVariant value) {
    RFX_UNUSED(key);
    RFX_UNUSED(old_value);
    int c2kSlot = value.asInt();

    if (cdmaSlotId != c2kSlot) {
        for (int i = 0; i < SIM_COUNT; i++) {
            if (i == c2kSlot) {
                RLOGD("onCdmaSocketSlotChange: update cdma slot %d", c2kSlot);
                modemMessages[i].connectWithCdmaModem = true;
            } else {
                modemMessages[i].connectWithCdmaModem = false;
            }
        }
        cdmaSlotId = c2kSlot;
    }
}

void RpModemController::powerOffModemBeforeTrm(int cdmaSlot, sp <RfxAction>& cdmaAction) {
    // create MD off message
    RLOGD("modemOffBeforeTRM cdma slot = %d", cdmaSlotId);
    sp<RfxMessage> message = RfxMessage::obtainRequest(cdmaSlot, RADIO_TECH_GROUP_GSM,
            RIL_REQUEST_MODEM_POWEROFF);
    cdmaLteModeControllerToken = message->getToken();
    action = cdmaAction;
    RfxMainThread::enqueueMessage(message);
}

void RpModemController::onSocketStateChange(int slotId, RfxStatusKeyEnum key, RfxVariant old_value, RfxVariant value) {
    RFX_UNUSED(key);
    RfxSocketState old_state = old_value.asSocketState();
    RfxSocketState new_state = value.asSocketState();

    bool old_gsm = old_state.getSocketState(RfxSocketState::SOCKET_GSM);
    bool new_gsm = new_state.getSocketState(RfxSocketState::SOCKET_GSM);
    bool old_c2k = old_state.getSocketState(RfxSocketState::SOCKET_C2K);
    bool new_c2k = new_state.getSocketState(RfxSocketState::SOCKET_C2K);
    RLOGD("onSocketStateChange old_gsm=%d, new_gsm=%d, old_c2k=%d, new_c2k=%d", old_gsm, new_gsm, old_c2k, new_c2k);

    bool reset_msg = false;
    if ((old_gsm != new_gsm) && (new_gsm == SOCKET_DISCONNECTED)) {
        reset_msg = true;
    } else if ((old_c2k != new_c2k) && (new_c2k == SOCKET_DISCONNECTED)) {
        reset_msg = true;
    }

    RLOGD("onSocketStateChange: reset_msg=%d", reset_msg);
    if (reset_msg) {
        for (int i = 0; i < SIM_COUNT; i++) {
            modemMessages[i].resetModemMessage();
        }
    }
}

