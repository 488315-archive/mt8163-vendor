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
#include "nw/RpNwRatController.h"
#include "RpCatController.h"
#include <cutils/jstring.h>

#define RFX_LOG_TAG "RpCatController"

/*****************************************************************************
 * Class RfxController
 *****************************************************************************/

RFX_IMPLEMENT_CLASS("RpCatController", RpCatController, RfxController);

RpCatController::RpCatController():
mCardType(-1),
mcdmaCardType(-1),
mSetupMenuSource(INVALID_VALUE),
mSetupEventSource(INVALID_VALUE),
mOpenChannelSource(INVALID_VALUE){
}

RpCatController::~RpCatController() {
}

void RpCatController::onInit() {
    RfxController::onInit();  // Required: invoke super class implementation
    logD(RFX_LOG_TAG,"onInit()");

    mProCmdRec.cmdType = 0;
    mProCmdRec.source = 0;

    const int request_id_list[] = {
        RIL_REQUEST_STK_GET_PROFILE,
        RIL_REQUEST_STK_SET_PROFILE,
        RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND,
        RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE,
        RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM,
        RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING,
        RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS,
        /* MTK RIL Request */
        RIL_REQUEST_STK_EVDL_CALL_BY_AP,
    };

    const int urc_id_list[] = {
        RIL_UNSOL_STK_SESSION_END,
        RIL_UNSOL_STK_PROACTIVE_COMMAND,
        RIL_UNSOL_STK_EVENT_NOTIFY,
        RIL_UNSOL_STK_CALL_SETUP,
        RIL_UNSOL_STK_CC_ALPHA_NOTIFY,
        /* MTK RIL Unsolicited Messge */
        RIL_UNSOL_STK_EVDL_CALL,
        RIL_UNSOL_STK_SETUP_MENU_RESET,
        RIL_UNSOL_STK_CALL_CTRL,
        RIL_UNSOL_STK_BIP_PROACTIVE_COMMAND,
    };

    // register request & URC id list
    // NOTE. one id can only be registered by one controller
    registerToHandleReuquest(request_id_list, sizeof(request_id_list)/sizeof(const int));
    registerToHandleUrc(urc_id_list, sizeof(urc_id_list)/sizeof(const int));

    // Register callbacks for card type
    getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_CARD_TYPE,
        RfxStatusChangeCallback(this, &RpCatController::onCardTypeChanged));

    // Register callbacks for roaming mode change
    getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_RAT_SWITCH_DONE,
        RfxStatusChangeCallback(this, &RpCatController::onRatSwitchDone));
}

void RpCatController::onCardTypeChanged(RfxStatusKeyEnum key,
    RfxVariant oldValue, RfxVariant newValue) {
    RFX_UNUSED(key);

    logD(RFX_LOG_TAG,"onCardTypeChanged oldValue %d, newValue %d",
        oldValue.asInt(), newValue.asInt());
}

void RpCatController::onRatSwitchDone(RfxStatusKeyEnum key,
    RfxVariant oldValue, RfxVariant newValue) {
    RFX_UNUSED(key);

    logD(RFX_LOG_TAG,"onRatSwitchDone oldValue %d, newValue %d",
        oldValue.asInt(), newValue.asInt());
}

void RpCatController::onDeinit() {
    logD(RFX_LOG_TAG,"onDeinit()");

    // Unregister callbacks for card type
    getStatusManager()->unRegisterStatusChanged(RFX_STATUS_KEY_CARD_TYPE,
        RfxStatusChangeCallback(this, &RpCatController::onCardTypeChanged));

    // Unregister callbacks for roaming mode change
    getStatusManager()->unRegisterStatusChanged(RFX_STATUS_KEY_RAT_SWITCH_DONE,
        RfxStatusChangeCallback(this, &RpCatController::onRatSwitchDone));

    // Required: invoke super class implementation
    RfxController::onDeinit();
}

bool RpCatController::onHandleRequest(const sp<RfxMessage>& message) {
    int msgId = message->getId();
    int cmdType = 0;
    int envCmdType = 0;
    int eventType = 0;

    logD(RFX_LOG_TAG,"req %d %s", msgId, requestToString(msgId));

    switch (msgId) {
        case RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND:
            parseStkEnvCmdType(message, &envCmdType, &eventType);

            switch (envCmdType) {
                case MENU_SELECTION_TAG:
                    logD(RFX_LOG_TAG,"mSetupMenuSource:%d", mSetupMenuSource);
                    if (RADIO_TECH_GROUP_C2K == mSetupMenuSource) {
                        sp<RfxMessage> c2k_request = RfxMessage::obtainRequest(
                            RADIO_TECH_GROUP_C2K, msgId, message, true);
                        requestToRild(c2k_request);
                    } else {
                        requestToRild(message);
                    }
                    break;

                case EVENT_DOWNLOAD_TAG:
                    switch (eventType) {
                        case EVENT_DATA_AVAILABLE:
                        case EVENT_CHANNEL_STATUS:
                            logD(RFX_LOG_TAG,"eventType:0x%02X mOpenChannelSource:%d",
                                eventType, mOpenChannelSource);
                            if (RADIO_TECH_GROUP_C2K == mOpenChannelSource) {
                                sp<RfxMessage> c2k_request =
                                    RfxMessage::obtainRequest(RADIO_TECH_GROUP_C2K,
                                    msgId, message, true);
                                requestToRild(c2k_request);
                            } else if (RADIO_TECH_GROUP_GSM == mOpenChannelSource){
                                requestToRild(message);
                            } else {
                                logD(RFX_LOG_TAG,"Unexpected DATA_AVAILABLE"
                                "/CHANNEL_STATUS Event. Warning!!!");
                            }
                            break;

                        default:
                            logD(RFX_LOG_TAG,"eventType:0x%02X mSetupEventSource:%d",
                                eventType, mSetupEventSource);
                            if (RADIO_TECH_GROUP_C2K == mSetupEventSource) {
                                sp<RfxMessage> c2k_request =
                                    RfxMessage::obtainRequest(RADIO_TECH_GROUP_C2K,
                                    msgId, message, true);
                                requestToRild(c2k_request);
                            } else {
                                requestToRild(message);
                            }
                            break;
                    }
                    break;

                default:
                    logD(RFX_LOG_TAG,"Unknow Envelope Command Warning!!!");
                    requestToRild(message);
                    break;
            }
            break;

        case RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM:
            logD(RFX_LOG_TAG,"mProCmdRec: cmdType:0x%02X source:%d",
                mProCmdRec.cmdType, mProCmdRec.source);

            if (CMD_SETUP_CALL == mProCmdRec.cmdType ||
                CMD_OPEN_CHAN == mProCmdRec.cmdType) {
                if (RADIO_TECH_GROUP_C2K == mProCmdRec.source) {
                    sp<RfxMessage> c2k_request = RfxMessage::obtainRequest(
                        RADIO_TECH_GROUP_C2K, msgId, message, true);
                    requestToRild(c2k_request);
                } else {
                    requestToRild(message);
                }
            } else {
                logD(RFX_LOG_TAG,"Unexpected CALL_SETUP_REQUESTED. Warning!!!");
                requestToRild(message);
            }

            // reset reset mProCmdRec.cmdType
            mProCmdRec.cmdType = 0;
            break;

        case RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE:
            parseStkCmdType(message, &cmdType);

            logD(RFX_LOG_TAG,"mProCmdRec: cmdType:0x%02X source:%d",
                mProCmdRec.cmdType, mProCmdRec.source);

            if (0 != mProCmdRec.cmdType && mProCmdRec.cmdType == cmdType) {
                if (RADIO_TECH_GROUP_C2K == mProCmdRec.source) {
                    sp<RfxMessage> c2k_request = RfxMessage::obtainRequest(
                        RADIO_TECH_GROUP_C2K, msgId, message, true);
                    requestToRild(c2k_request);
                } else {
                    requestToRild(message);
                }

                // reset mProCmdRec.cmdType
                mProCmdRec.cmdType = 0;
            } else {
                logD(RFX_LOG_TAG,"Not Expected TR. Warning!!!");
                requestToRild(message);
            }
            break;

        case RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING:
            requestToRild(message);

            mCardType = getStatusManager()->getIntValue(RFX_STATUS_KEY_CARD_TYPE, -1);
            mcdmaCardType = getStatusManager()->getIntValue(
                RFX_STATUS_KEY_CDMA_CARD_TYPE, -1);
            logD(RFX_LOG_TAG,"mCardType:0x%02X mcdmaCardType:%d", mCardType, mcdmaCardType);

            // When c2k-rild receive STK_SERVICE_IS_RUNNING, it will
            // send AT+UTKPD to c2k modem
            if (mCardType & RFX_CARD_TYPE_CSIM || mCardType & RFX_CARD_TYPE_RUIM) {
                sp<RfxMessage> c2k_request = RfxMessage::obtainRequest(
                    RADIO_TECH_GROUP_C2K, msgId, message);
                requestToRild(c2k_request);
            }
            break;

/*
        case RIL_REQUEST_STK_GET_PROFILE:
        case RIL_REQUEST_STK_SET_PROFILE:
        case RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS:
        case RIL_REQUEST_STK_EVDL_CALL_BY_AP:
*/
        default:
            requestToRild(message);
            break;
    }

    return true;
}

bool RpCatController::onHandleUrc(const sp<RfxMessage>& message) {
    int msgId = message->getId();
    int sourceId = message->getSource();
    int cmdType = 0;

    logD(RFX_LOG_TAG,"urc %d %s source:%d", msgId,
        urcToString(msgId), sourceId);

    switch (msgId) {
        case RIL_UNSOL_STK_PROACTIVE_COMMAND:
        case RIL_UNSOL_STK_EVENT_NOTIFY:
            parseStkCmdType(message, &cmdType);
            switch (cmdType) {
                case CMD_SETUP_MENU:
                    mSetupMenuSource = sourceId;
                    mProCmdRec.cmdType = cmdType;
                    mProCmdRec.source = sourceId;
                    break;

                case CMD_SETUP_EVENT_LIST:
                    mSetupEventSource = sourceId;
                    mProCmdRec.cmdType = cmdType;
                    mProCmdRec.source = sourceId;
                    break;

                case CMD_SETUP_CALL:
                case CMD_LAUNCH_BROWSER:
                case CMD_PLAY_TONE:
                case CMD_DSPL_TXT:
                case CMD_GET_INKEY:
                case CMD_GET_INPUT:
                case CMD_SELECT_ITEM:
                case CMD_PROVIDE_LOCAL_INFO:
                case CMD_IDLE_MODEL_TXT:
                case CMD_LANGUAGE_NOTIFY:
                // If open channel come with UNSOL_STK_PROACTIVE_COMMAND, it
                // need user confirm. After user confirm, the STK apk will
                // send the result with STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM
                case CMD_OPEN_CHAN:
                case CMD_CLOSE_CHAN:
                case CMD_RECEIVE_DATA:
                case CMD_SEND_DATA:
                case CMD_GET_CHAN_STATUS:
                    mProCmdRec.cmdType = cmdType;
                    mProCmdRec.source = sourceId;
                    break;

                default:
                    break;
            }

            responseToRilj(message);
            break;

        case RIL_UNSOL_STK_BIP_PROACTIVE_COMMAND:
            parseStkCmdType(message, &cmdType);
            switch (cmdType) {
                // If open channel command with UNSOL_STK_BIP_PROACTIVE_COMMAND, it
                // may not have alpha ID(No need user confirm) or it has been
                // confirmed by user
                case CMD_OPEN_CHAN:
                    mOpenChannelSource = sourceId;
                    mProCmdRec.cmdType = cmdType;
                    mProCmdRec.source = sourceId;
                    break;

                case CMD_CLOSE_CHAN:
                    mOpenChannelSource = INVALID_VALUE;
                    mProCmdRec.cmdType = cmdType;
                    mProCmdRec.source = sourceId;
                    break;

                case CMD_RECEIVE_DATA:
                case CMD_SEND_DATA:
                case CMD_GET_CHAN_STATUS:
                    mProCmdRec.cmdType = cmdType;
                    mProCmdRec.source = sourceId;
                    break;

                default:
                    break;
            }

            responseToRilj(message);
            break;

/*
        case RIL_UNSOL_STK_SESSION_END:
            break;
        case RIL_UNSOL_STK_CALL_CTRL:
            break;
        case RIL_UNSOL_STK_CC_ALPHA_NOTIFY:
            break;
        case RIL_UNSOL_STK_EVDL_CALL:
            break;
        case RIL_UNSOL_STK_SETUP_MENU_RESET:
            break;
*/
        default:
            responseToRilj(message);
            break;
    }

    return true;
}

bool RpCatController::onHandleResponse(const sp<RfxMessage>& message) {
    int msgId = message->getId();
    int sourceId = message->getSource();

    logD(RFX_LOG_TAG,"response %d %s source:%d", msgId,
        requestToString(msgId), sourceId);

    switch (msgId) {
        case RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING:
            if (RADIO_TECH_GROUP_C2K == sourceId) {
                logD(RFX_LOG_TAG,"Ignore response %d %s", msgId, requestToString(msgId));
                break;
            }
        default:
            responseToRilj(message);
            break;
    }

    return true;
}

int RpCatController::getNwsMode() {
    RpNwRatController* mNwRatController =
        (RpNwRatController *)findController(getSlotId(),
        RFX_OBJ_CLASS_INFO(RpNwRatController));

    if (NULL != mNwRatController)
        return mNwRatController->getNwsMode();
    else
        return NWS_MODE_UNKNOWN;
}

char* RpCatController::strdupReadString(Parcel* p) {
    size_t stringlen;
    const char16_t *s16;

    s16 = p->readString16Inplace(&stringlen);

    return strndup16to8(s16, stringlen);
}

int RpCatController::getStkCommandType(char *cmd) {
    char temp[3] = {0};
    int cmdType = 0;

    strncpy(temp, cmd, 2);
    cmdType = strtoul(temp, NULL, 16);
    cmdType = 0xFF & cmdType;

    return cmdType;
}

void RpCatController::parseStkCmdType(const sp<RfxMessage>& message, int* cmdType) {
    int msgId = message->getId();
    int typePos = 0;
    char* cmd = NULL;

    // Get command from message
    cmd = strdupReadString(message->getParcel());
    if (cmd) {
        logD(RFX_LOG_TAG,"cmd:%s", cmd);

        // decide typePos value
        switch (msgId) {
            case RIL_UNSOL_STK_PROACTIVE_COMMAND:
            case RIL_UNSOL_STK_EVENT_NOTIFY:
            case RIL_UNSOL_STK_BIP_PROACTIVE_COMMAND:
                if (cmd[2] <= '7') {
                    typePos = 10;
                } else {
                    typePos = 12;
                }
                break;

            case RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE:
                typePos = 6;
                break;

            default:
                logD(RFX_LOG_TAG,"Not Support msgId:%d", msgId);
                break;
        }

        // check command type
        *cmdType = getStkCommandType(&cmd[typePos]);

        free(cmd);
    } else {
        logD(RFX_LOG_TAG,"cmd is NULL. Error!!!");
    }
}

void RpCatController::parseStkEnvCmdType(const sp<RfxMessage>& message,
                                         int* envCmdType,
                                         int* eventType) {
    int msgId = message->getId();
    int envCmdPos = 0;
    int eventPos = 0;
    char* cmd = NULL;

    // Get command from message
    cmd = strdupReadString(message->getParcel());
    if (cmd) {
        logD(RFX_LOG_TAG,"cmd:%s", cmd);

        // decide typePos value
        switch (msgId) {
            case RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND:
                envCmdPos = 0;
                eventPos = 4;
                break;

            default:
                logD(RFX_LOG_TAG,"Not Support msgId:%d", msgId);
                break;
        }

        // check command type
        *envCmdType = getStkCommandType(&cmd[envCmdPos]);
        *eventType = getStkCommandType(&cmd[eventPos]);

        free(cmd);
    } else {
        logD(RFX_LOG_TAG,"cmd is NULL. Error!!!");
    }
}

const char* RpCatController::requestToString(int reqId) {
    switch (reqId) {
        case RIL_REQUEST_STK_GET_PROFILE: return "STK_GET_PROFILE";
        case RIL_REQUEST_STK_SET_PROFILE: return "STK_SET_PROFILE";
        case RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND:
            return "STK_SEND_ENVELOPE_COMMAND";
        case RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE:
            return "STK_SEND_TERMINAL_RESPONSE";
        case RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM:
            return "STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM";
        case RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING:
            return "REPORT_STK_SERVICE_IS_RUNNING";
        case RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS:
            return "STK_SEND_ENVELOPE_WITH_STATUS";
        case RIL_REQUEST_STK_EVDL_CALL_BY_AP: return "STK_EVDL_CALL_BY_AP";
        default:
            return "UNKNOWN_REQUEST";
    }
}

const char* RpCatController::urcToString(int urcId) {
    switch (urcId) {
        case RIL_UNSOL_STK_SESSION_END: return "UNSOL_STK_SESSION_END";
        case RIL_UNSOL_STK_PROACTIVE_COMMAND: return "UNSOL_STK_PROACTIVE_COMMAND";
        case RIL_UNSOL_STK_EVENT_NOTIFY: return "UNSOL_STK_EVENT_NOTIFY";
        case RIL_UNSOL_STK_CALL_SETUP: return "UNSOL_STK_CALL_SETUP";
        case RIL_UNSOL_STK_EVDL_CALL: return "UNSOL_STK_EVDL_CALL";
        case RIL_UNSOL_STK_SETUP_MENU_RESET: return "UNSOL_STK_SETUP_MENU_RESET";
        case RIL_UNSOL_STK_CALL_CTRL: return "UNSOL_STK_CALL_CTRL";
        case RIL_UNSOL_STK_CC_ALPHA_NOTIFY: return "UNSOL_STK_CC_ALPHA_NOTIFY";
        case RIL_UNSOL_STK_BIP_PROACTIVE_COMMAND:
            return "UNSOL_STK_BIP_PROACTIVE_COMMAND";
        default:
            return "UNKNOWN_URC";
    }
}