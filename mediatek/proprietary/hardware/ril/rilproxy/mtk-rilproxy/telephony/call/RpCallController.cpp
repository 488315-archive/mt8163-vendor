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
#include "RpCallController.h"

#define LOG_TAG "RpCallController"

/*****************************************************************************
 * Class RpCallController
 *****************************************************************************/
RFX_IMPLEMENT_CLASS("RpCallController", RpCallController, RfxController);

RpCallController::RpCallController() :
 mCsPhone(0), mCallCount(0) {
    char value[PROPERTY_VALUE_MAX] = { 0 };
    property_get("ro.mtk_switch_antenna", value, "");
    mSwitchAntennaSupported = !strcmp(value, "1");
}

RpCallController::~RpCallController() {
    if (mRedialHandler != NULL) {
        RFX_OBJ_CLOSE(mRedialHandler);
    }
}

void RpCallController::updatePhoneIdToMDIdProperty(int phoneId, const char mdId[PROPERTY_VALUE_MAX]) {
    RLOGD("[RpCallController] phone%d map to %s", phoneId + 1, mdId);
    switch (phoneId) {
        case SIM_ID_1:
            property_set("ril.phone1.mapped.md", mdId);
            break;
        case SIM_ID_2:
            property_set("ril.phone2.mapped.md", mdId);
            break;
    }
}

void RpCallController::onInit() {
    RfxController::onInit(); // Required: invoke super class implementation
    RLOGD("[RpCallController] onInit E!");

    const int request_id_list[] = {
        /* Common Request */
        RIL_REQUEST_GET_CURRENT_CALLS,
        RIL_REQUEST_DIAL,
        RIL_REQUEST_HANGUP,
        RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND,
        RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND,
        RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE,
        RIL_REQUEST_CONFERENCE,
        RIL_REQUEST_UDUB,
        RIL_REQUEST_LAST_CALL_FAIL_CAUSE,
        RIL_REQUEST_DTMF,
        RIL_REQUEST_ANSWER,
        RIL_REQUEST_DTMF_START,
        RIL_REQUEST_DTMF_STOP,
        RIL_REQUEST_SEPARATE_CONNECTION,
        RIL_REQUEST_SET_MUTE,
        /* MTK RIL Request */
        RIL_REQUEST_HANGUP_ALL,
        RIL_REQUEST_EMERGENCY_DIAL,
        RIL_REQUEST_SET_SPEECH_CODEC_INFO,
    };

    const int urc_id_list[] = {
        /* Common URC */
        RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED,
        RIL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE,
        RIL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE,
        /* MTK RIL URC */
        RIL_UNSOL_CALL_INFO_INDICATION,
        // GSM
        RIL_UNSOL_INCOMING_CALL_INDICATION,
    };

    // register request & URC id list
    // NOTE. one id can only be registered by one controller
    registerToHandleReuquest(request_id_list, sizeof(request_id_list)/sizeof(int));
    registerToHandleUrc(urc_id_list, sizeof(urc_id_list)/sizeof(int));
    RLOGD("[RpCallController] request size:%d urc size:%d!",
        sizeof(request_id_list)/sizeof(int), sizeof(urc_id_list)/sizeof(int));

    // register callbacks for CS phone
    getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_VOICE_TYPE,
        RfxStatusChangeCallback(this, &RpCallController::onCsPhone));

    // Create RpRedialHandler object
    RFX_OBJ_CREATE(mRedialHandler, RpRedialHandler, this);
}

void RpCallController::onDeinit() {
    RLOGD("[RpCallController] onDeinit E!");
    RfxController::onDeinit();
}

void RpCallController::onCsPhone(RfxStatusKeyEnum key,
        RfxVariant old_value, RfxVariant value) {
    mCsPhone = value.asInt();
    if (mCsPhone == RADIO_TECH_GROUP_C2K) {
        updatePhoneIdToMDIdProperty(getSlotId(), "MD3");
    } else if (mCsPhone == RADIO_TECH_GROUP_GSM) {
        updatePhoneIdToMDIdProperty(getSlotId(), "MD1");
    }
    RLOGD("[RpCallController] (slot %d) preCsPhone %d, mCsPhone %d",
            getSlotId(), old_value.asInt(), mCsPhone);
}

void RpCallController::onPreHandleRequest(const sp<RfxMessage>& message) {
    int msg_id = message->getId();
    if (msg_id == RIL_REQUEST_DIAL || msg_id == RIL_REQUEST_EMERGENCY_DIAL
            || msg_id == RIL_REQUEST_ANSWER) {
        // Switch Antenna
        if (mSwitchAntennaSupported) {
            RpCallCenterController *centerController =
                    (RpCallCenterController *)findController(RFX_SLOT_ID_UNKNOWN,
                        RFX_OBJ_CLASS_INFO(RpCallCenterController));
            if (centerController != NULL) {
                SWITCH_ANTENNA_RAT_MODE ratMode = RAT_MODE_NONE;
                if (mCsPhone == RADIO_TECH_GROUP_C2K) {
                    ratMode = RAT_MODE_C2K;
                } else if (mCsPhone == RADIO_TECH_GROUP_GSM) {
                    ratMode = RAT_MODE_GSM;
                }
                centerController->handleSwitchAntennaRequest(STATE_CONNECTED, ratMode);
            } else {
                RLOGE("Can't find RpCallCenterController!");
            }
        }
    }
}

bool RpCallController::onHandleRequest(const sp<RfxMessage>& message) {
    onPreHandleRequest(message);
    int msg_id = message->getId();
    RLOGD("[RpCallController] handle req %d %s (slot %d) CsPhone %d", msg_id,
            requestToString(msg_id), getSlotId(), mCsPhone);

    switch (msg_id) {
        // Notify Redial Handler
        case RIL_REQUEST_DIAL:
        case RIL_REQUEST_EMERGENCY_DIAL:
        case RIL_REQUEST_HANGUP:
        case RIL_REQUEST_HANGUP_ALL:
            if (mRedialHandler != NULL) {
                RLOGD("[RpCcController] forward req %s (slot %d) CsPhone %d to RedialHandler",
                        requestToString(msg_id), getSlotId(), mCsPhone);

                mRedialHandler->notifyRilRequest(message);
            }
            break;
        default:
            break;
    }

    if (RADIO_TECH_GROUP_GSM == mCsPhone) {
        requestToRild(message);
    } else {
        sp<RfxMessage> c2k_request = RfxMessage::obtainRequest(RADIO_TECH_GROUP_C2K,
                                                                msg_id, message, true);
        // fill in items in RfxMessage for c2k_request
        // such as new a parcel and attach it to c2k_request
        // NOTE. attached parcel will be free when the RfxMessage
        //       instance is destroyed, so don't attach one parcel
        //       object to more than one RfxMessage instance

        // send request to C2K RILD
        requestToRild(c2k_request);
    }
    return true;
}

bool RpCallController::onHandleResponse(const sp<RfxMessage>& message) {
    int msg_id = message->getId();
    sp<RfxMessage> rilResponse = message;

    RLOGD("[RpCallController] handle resp %d %s (slot %d)", msg_id, requestToString(msg_id),
            getSlotId());

    switch (msg_id) {
        case RIL_REQUEST_GET_CURRENT_CALLS:
            if (mRedialHandler != NULL) {
                RLOGD("[RpCallController] forward resp %s (slot %d) to RedialHandler",
                        requestToString(msg_id), getSlotId());

                // This reponse may be modified in RedialHandler
                rilResponse = mRedialHandler->notifyRilResponse(message);
            }

            onHandleGetCurrentCalls(rilResponse);
            break;
        default:
            responseToRilj(message);
            break;
    }
    return true;
}

bool RpCallController::onHandleUrc(const sp<RfxMessage>& message) {
    int msg_id = message->getId();
    RLOGD("[RpCallController] handle urc %d %s (slot %d)", msg_id, requestToString(msg_id),
            getSlotId());

    switch (msg_id) {
        case RIL_UNSOL_INCOMING_CALL_INDICATION:
            onHandleIncomingCallIndication(message);
            break;

        case RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED:
            // RpRedialHandler may block URC if needed
            if (mRedialHandler != NULL) {
                RLOGD("[RpCallController] forward resp %s (slot %d) to RedialHandler",
                        requestToString(msg_id), getSlotId());

                if (mRedialHandler->notifyRilUrc(message) == false) {
                    break;
                }
            }
            onHandleCallStateChanged(message);
            break;

        case RIL_UNSOL_CALL_INFO_INDICATION:
        case RIL_UNSOL_ENTER_EMERGENCY_CALLBACK_MODE:
        case RIL_UNSOL_EXIT_EMERGENCY_CALLBACK_MODE:
            // RpRedialHandler may block URC if needed
            if (mRedialHandler != NULL) {
                RLOGD("[RpCallController] forward resp %s (slot %d) to RedialHandler",
                        requestToString(msg_id), getSlotId());

                if (mRedialHandler->notifyRilUrc(message) == false) {
                    break;
                }
            }
            responseToRilj(message);
            break;
        default:
            responseToRilj(message);
    }
    return true;
}

void RpCallController::onHandleGetCurrentCalls(const sp<RfxMessage>& response) {
    bool reportToRilj = true;
    sp<RfxMessage> tmpMsg = RfxMessage::obtainResponse(response->getError(), response, true);
    tmpMsg->getParcel()->setDataPosition(3 * sizeof(int));
    int callCount = tmpMsg->getParcel()->readInt32();
    RLOGD("[RpCallController] onHandleGetCurrentCalls, callCount:%d", callCount);
    RpCallCenterController *centerController =
                (RpCallCenterController *)findController(RFX_SLOT_ID_UNKNOWN,
                    RFX_OBJ_CLASS_INFO(RpCallCenterController));
    if (mCallCount > 0 && callCount == 0) {
        if (centerController != NULL) {
            if (mCsPhone == RADIO_TECH_GROUP_GSM) {
                centerController->handleSwitchHPF(false);
            }
            if (mSwitchAntennaSupported) {
                SWITCH_ANTENNA_RAT_MODE ratMode = RAT_MODE_NONE;
                if (mCsPhone == RADIO_TECH_GROUP_C2K) {
                    ratMode = RAT_MODE_C2K;
                } else if (mCsPhone == RADIO_TECH_GROUP_GSM) {
                    ratMode = RAT_MODE_GSM;
                }
                centerController->handleSwitchAntennaRequest(STATE_DISCONNECTED, ratMode);
            }
        }
    } else if (mCallCount == 0 && callCount > 0) {
        if (centerController != NULL) {
            if (mCsPhone == RADIO_TECH_GROUP_GSM) {
                centerController->handleSwitchHPF(true);
            }
            if (mSwitchAntennaSupported && mCsPhone == RADIO_TECH_GROUP_C2K) {
                int i = 0;
                int value = -1;
                bool isMT = false;
                tmpMsg->getParcel()->setDataPosition(8 * sizeof(int));
                value = tmpMsg->getParcel()->readInt32();
                RLOGD("[RpCallController] onHandleGetCurrentCalls, value:%d", value);
                isMT = value != 0;
                if (isMT) {
                    reportToRilj = centerController->handleIncomingCallIndication(RADIO_TECH_GROUP_C2K, tmpMsg);
                    if (reportToRilj) {
                        sp<RfxMessage> tmpUrc = RfxMessage::obtainUrc(getSlotId(), RIL_UNSOL_RESPONSE_CALL_STATE_CHANGED);
                        responseToRilj(tmpUrc);
                        reportToRilj = false;
                    }
                }
            }
        }
    }
    mCallCount = callCount;
    getStatusManager(getSlotId())->setIntValue(RFX_STATUS_KEY_VOICE_CALL_COUNT, mCallCount);
    if (reportToRilj) {
        responseToRilj(response);
    }
}

void RpCallController::onHandleIncomingCallIndication(const sp<RfxMessage>& message) {
    bool reportToRilj = true;
    if (mSwitchAntennaSupported) {
        sp<RfxMessage> tmpMsg = RfxMessage::obtainUrc(getSlotId(), message->getId(), message, true);
        RpCallCenterController *centerController =
                    (RpCallCenterController *)findController(RFX_SLOT_ID_UNKNOWN,
                        RFX_OBJ_CLASS_INFO(RpCallCenterController));
        if (centerController != NULL) {
            reportToRilj = centerController->handleIncomingCallIndication(RADIO_TECH_GROUP_GSM, tmpMsg);
        } else {
            RLOGE("Can't find RpCallCenterController!");
        }
    }
    if (reportToRilj) {
        responseToRilj(message);
    }
}

void RpCallController::onHandleCallStateChanged(const sp<RfxMessage>& message) {
    if (mCallCount == 0 && mCsPhone == RADIO_TECH_GROUP_C2K && mSwitchAntennaSupported) {
        sp<RfxMessage> rilRequest = RfxMessage::obtainRequest(getSlotId(), RADIO_TECH_GROUP_C2K, RIL_REQUEST_GET_CURRENT_CALLS);
        requestToRild(rilRequest);
    } else {
        responseToRilj(message);
    }
}
