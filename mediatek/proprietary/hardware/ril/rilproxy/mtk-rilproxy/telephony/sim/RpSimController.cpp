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
#include "RpSimController.h"
#include "RfxStatusDefs.h"
#include <utils/Log.h>
#include <binder/Parcel.h>
#include <cutils/properties.h>
#include <cutils/jstring.h>

/*****************************************************************************
 * Class RfxController
 *****************************************************************************/

RFX_IMPLEMENT_CLASS("RpSimController", RpSimController, RpSimControllerBase);

RpSimController::RpSimController() :
    mCurrentGetSimStatusReq(0),
    mGsmGetSimStatusReq(0),
    mC2kGetSimStatusReq(0),
    mGsmSimStatusRespParcel(NULL),
    mC2kSimStatusRespParcel(NULL),
    mLastCardType(-1),
    mGsmSimCtrl(NULL),
    mC2kSimCtrl(NULL),
    mGetSimStatusErr(RIL_E_SUCCESS) {
}

RpSimController::~RpSimController() {
}

void RpSimController::onInit() {
    RpSimControllerBase::onInit(); // Required: invoke super class implementation

    RLOGD("[RpSimController] onInit %d (slot %d)", mCurrentGetSimStatusReq, getSlotId());

    // Create Gsm SIM Controller and C2K SIM Controller
    RFX_OBJ_CREATE(mGsmSimCtrl, RpGsmSimController, this);
    RFX_OBJ_CREATE(mC2kSimCtrl, RpC2kSimController, this);

    const int request_id_list[] = {
        RIL_REQUEST_GET_SIM_STATUS,
        RIL_REQUEST_GET_IMSI,
        RIL_REQUEST_ENTER_SIM_PIN,
        RIL_REQUEST_ENTER_SIM_PUK,
        RIL_REQUEST_ENTER_SIM_PIN2,
        RIL_REQUEST_ENTER_SIM_PUK2,
        RIL_REQUEST_CHANGE_SIM_PIN,
        RIL_REQUEST_CHANGE_SIM_PIN2,
        RIL_REQUEST_QUERY_FACILITY_LOCK,
        RIL_REQUEST_SET_FACILITY_LOCK,
        RIL_REQUEST_SIM_IO,
        RIL_REQUEST_SIM_AUTHENTICATION,
        RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC,
        RIL_REQUEST_SIM_OPEN_CHANNEL,
        RIL_REQUEST_SIM_OPEN_CHANNEL_WITH_SW,
        RIL_REQUEST_SIM_CLOSE_CHANNEL,
        RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL,
        RIL_REQUEST_SIM_GET_ATR,
        RIL_REQUEST_SET_UICC_SUBSCRIPTION
    };

    const int urc_id_list[] = {
        RIL_LOCAL_GSM_UNSOL_CARD_TYPE_NOTIFY,
        RIL_LOCAL_GSM_UNSOL_CT3G_DUALMODE_CARD,
    };

    registerToHandleReuquest(request_id_list, (sizeof(request_id_list)/sizeof(int)));
    registerToHandleUrc(urc_id_list, (sizeof(urc_id_list)/sizeof(int)));

    getStatusManager()->setIntValue(RFX_STATUS_KEY_CARD_TYPE, -1);
    getStatusManager()->setBoolValue(RFX_STATUS_KEY_MODEM_SIM_TASK_READY, false, true);

    // register callbacks to get RADIO_UNAVAILABLE in order to reset
    // RFX_STATUS_KEY_MODEM_SIM_TASK_READY.
    getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_RADIO_STATE,
        RfxStatusChangeCallback(this, &RpSimController::onRadioStateChanged));
#if 0
    const int urc_id_list[] = {
        RIL_UNSOL_ON_USSD,
        RIL_UNSOL_ON_USSD_REQUEST,
        RIL_UNSOL_DATA_CALL_LIST_CHANGED
    };

    // register request & URC id list
    // NOTE. one id can only be registered by one controller
    registerToHandleReuquest(request_id_list, 3);
    registerToHandleUrc(urc_id_list, 3);

    // register callbacks to get required information
    getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_AVAILABLE,
        RfxStatusChangeCallback(this, &RfxHelloController::onAvailable));

    getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_POWER_ON,
        RfxStatusChangeCallback(this, &RfxHelloController::onPowerOn));

    RfxSampleController *sample_controller;

    // create the object of RfxSampleController as the child controller
    // of this object (an instance of RfxHelloController)
    RFX_OBJ_CREATE(sample_controller, RfxSampleController, this);

    // connect the signal defined by another module
    sample_controller->m_something_changed_singal.connect(this,
                                    &RfxHelloController::onSampleControlerSomethingChanged);
#endif
}

#if 0
void RpSimController::onAvailable(RfxStatusKeyEnum key,
    RfxVariant old_value, RfxVariant value) {
}

void RpSimController::onPowerOn(RfxStatusKeyEnum key,
    RfxVariant old_value, RfxVariant value) {
}
#endif

bool RpSimController::onPreviewMessage(const sp<RfxMessage>& message) {
    if (message->getType() == URC) {
        return true;
    }
    if (message->getType() == REQUEST && message->getId() == RIL_REQUEST_GET_SIM_STATUS) {
        RLOGD("[RpSimController] onPreviewMessage, (%d, %d) (slot %d)",
                    message->getId(), mCurrentGetSimStatusReq, getSlotId());
        if ((mCurrentGetSimStatusReq != 0) ||
            mC2kSimCtrl->onPreviewCheckRequestGetSimStatus()) {
            RLOGD("[RpSimController] onPreviewMessage, put %s into pending list (slot %d)",
                    requestToString(message->getId()), getSlotId());
            return false;
        }
    } else if (message->getType() == REQUEST && message->getId() == RIL_REQUEST_GET_IMSI) {
        if (mC2kSimCtrl->onPreviewCheckRequestGetImsi(message)) {
            RLOGD("[RpSimController] onPreviewMessage, put %s into pending list (slot %d)",
                    requestToString(message->getId()), getSlotId());
            return false;
        }
    }

    return true;
}

bool RpSimController::onCheckIfResumeMessage(const sp<RfxMessage>& message) {
    if (message->getType() == REQUEST && message->getId() == RIL_REQUEST_GET_SIM_STATUS) {
        if ((mCurrentGetSimStatusReq == 0) &&
                !mC2kSimCtrl->onPreviewCheckRequestGetSimStatus()) {
            return true;
        }
    } else if (message->getType() == REQUEST && message->getId() == RIL_REQUEST_GET_IMSI) {
        if (!mC2kSimCtrl->onPreviewCheckRequestGetImsi(message)) {
            return true;
        }
    }
    RLOGD("[RpSimController] onCheckIfResumeMessage, got request %d (slot %d)", message->getId(),
            getSlotId());
    return false;
}

bool RpSimController::onHandleRequest(const sp<RfxMessage>& message) {
    RLOGD("[RpSimController] handle req %s (slot %d)", requestToString(message->getId()),
            getSlotId());

    switch (message->getId()) {
    case RIL_REQUEST_GET_SIM_STATUS: {
            handleGetSimStatusReq(message);
            break;
        }
    case RIL_REQUEST_GET_IMSI: {
            handleGetImsiReq(message);
            break;
        }
    case RIL_REQUEST_ENTER_SIM_PIN:
    case RIL_REQUEST_ENTER_SIM_PUK:
    case RIL_REQUEST_ENTER_SIM_PIN2:
    case RIL_REQUEST_ENTER_SIM_PUK2:
    case RIL_REQUEST_CHANGE_SIM_PIN:
    case RIL_REQUEST_CHANGE_SIM_PIN2:
        {
            handlePinPukReq(message);
            break;
        }
    case RIL_REQUEST_QUERY_FACILITY_LOCK: {
            handleQuerySimFacilityReq(message);
            break;
        }
    case RIL_REQUEST_SET_FACILITY_LOCK: {
            handleSetSimFacilityReq(message);
            break;
        }
    case RIL_REQUEST_SIM_OPEN_CHANNEL:
    case RIL_REQUEST_SIM_OPEN_CHANNEL_WITH_SW:
        {
            handleIccOpenChannelReq(message);
            break;
        }
    case RIL_REQUEST_SIM_CLOSE_CHANNEL: {
            handleIccCloseChannelReq(message);
            break;
        }
    case RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL: {
            handleTransmitApduReq(message);
            break;
        }
    case RIL_REQUEST_SIM_GET_ATR: {
            handleGetAtrReq(message);
            break;
        }
    case RIL_REQUEST_SET_UICC_SUBSCRIPTION: {
            handleSetUiccSubscriptionReq(message);
            break;
        }
    case RIL_REQUEST_SIM_IO: {
            handleSimIoReq(message);
            break;
        }
    case RIL_REQUEST_SIM_AUTHENTICATION: {
            handleSimAuthenticationReq(message);
            break;
        }
    case RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC: {
            handleSimTransmitApduBasicReq(message);
            break;
        }
    default:
        return false;
    }
    return true;
}

bool RpSimController::onHandleResponse(const sp<RfxMessage>& message) {
    RLOGD("[RpSimController] handle response %s (slot %d)", requestToString(message->getId()),
            getSlotId());

    switch (message->getId()) {
    case RIL_REQUEST_GET_SIM_STATUS: {
            handleGetSimStatusRsp(message);
            break;
        }
    case RIL_REQUEST_GET_IMSI: {
            handleGetImsiRsp(message);
            break;
        }
    case RIL_REQUEST_ENTER_SIM_PIN:
    case RIL_REQUEST_ENTER_SIM_PUK:
    case RIL_REQUEST_ENTER_SIM_PIN2:
    case RIL_REQUEST_ENTER_SIM_PUK2:
    case RIL_REQUEST_CHANGE_SIM_PIN:
    case RIL_REQUEST_CHANGE_SIM_PIN2:
        {
            handlePinPukRsp(message);
            break;
        }
    case RIL_REQUEST_QUERY_FACILITY_LOCK: {
            handleQuerySimFacilityRsp(message);
            break;
        }
    case RIL_REQUEST_SET_FACILITY_LOCK: {
            handleSetSimFacilityRsp(message);
            break;
        }
    case RIL_REQUEST_SIM_OPEN_CHANNEL:
    case RIL_REQUEST_SIM_OPEN_CHANNEL_WITH_SW:
        {
            handleIccOpenChannelRsp(message);
            break;
        }
    case RIL_REQUEST_SIM_CLOSE_CHANNEL: {
            handleIccCloseChannelRsp(message);
            break;
        }
    case RIL_REQUEST_SIM_TRANSMIT_APDU_CHANNEL: {
            handleTransmitApduRsp(message);
            break;
        }
    case RIL_REQUEST_SIM_GET_ATR: {
            handleGetAtrRsp(message);
            break;
        }
    case RIL_REQUEST_SET_UICC_SUBSCRIPTION: {
            handleSetUiccSubscriptionRsp(message);
            break;
        }
    case RIL_REQUEST_SIM_IO: {
            handleSimIoRsp(message);
            break;
        }
    case RIL_REQUEST_SIM_AUTHENTICATION: {
            handleSimAuthenticationRsp(message);
            break;
        }
    case RIL_REQUEST_SIM_TRANSMIT_APDU_BASIC: {
            handleSimTransmitApduBasicRsp(message);
            break;
        }
    default:
        return false;
    }
    return true;
}

bool RpSimController::onHandleUrc(const sp<RfxMessage>& message) {
    RLOGD("[RpSimController] handle urc %s (slot %d)", urcToString(message->getId()),
            getSlotId());

    switch (message->getId()) {
        case RIL_LOCAL_GSM_UNSOL_CARD_TYPE_NOTIFY:
            handleLocalCardTypeNotify(message);
            break;
        case RIL_LOCAL_GSM_UNSOL_CT3G_DUALMODE_CARD:
            mC2kSimCtrl->setCt3gDualmodeValue(message);
            break;
        default:
            return false;
    }
    #if 0
    if (message->id == RIL_UNSOL_ON_USSD) {
        // 1. decompress message.parcel to get URC data

        // 2. if any predefined status need to be shared to other modules
        //    set it to status manager, status manager will inform
        //    registed callbacks
        getStatusManager()->setIntValue(RFX_STATUS_KEY_CARD_TYPE, 2);

        RfxTimer::stop(m_timer_handle);

        // 3. if the URC need to be sent to RILJ, send it,
        //    be able to update parceled data if required
        responseToRilj(message);
    }
    #endif
    return true;
}


#if 0
void RpSimController::onTimer() {
    // do something
}
#endif


// We decode response parcel only when we got "ALL" responses which we are waiting.
RIL_CardStatus_v6* RpSimController::decodeGetSimStatusResp() {
    RIL_CardStatus_v6 *cardStatus = (RIL_CardStatus_v6*)calloc(1, sizeof(RIL_CardStatus_v6));
    int i = 0;
    size_t s16Len;
    const char16_t *s16 = NULL;
    int card_state[2] = {-1};
    int universal_pin_state[2] = {-1};
    int gsmIndex[2] = {RIL_CARD_MAX_APPS};
    int c2kIndex[2] = {RIL_CARD_MAX_APPS};
    int imsIndex[2] = {RIL_CARD_MAX_APPS};
    int numApplications[2] = {0};
    int card_status = RFX_SIM_STATE_NOT_READY;
    RIL_AppState appState = RIL_APPSTATE_UNKNOWN;
    RIL_AppStatus* pAppStatus = NULL;

    if (mGsmSimStatusRespParcel != NULL) {
        // Read data from GSM parcel
        card_state[0] = mGsmSimStatusRespParcel->readInt32();
        universal_pin_state[0] = mGsmSimStatusRespParcel->readInt32();
        gsmIndex[0] = mGsmSimStatusRespParcel->readInt32();
        c2kIndex[0] = mGsmSimStatusRespParcel->readInt32();
        imsIndex[0] = mGsmSimStatusRespParcel->readInt32();
        numApplications[0] = mGsmSimStatusRespParcel->readInt32();
        // limit to maximum allowed applications
        if (numApplications[0] > RIL_CARD_MAX_APPS) {
            numApplications[0] = RIL_CARD_MAX_APPS;
        }
        cardStatus->gsm_umts_subscription_app_index = gsmIndex[0];
        cardStatus->ims_subscription_app_index = ((imsIndex[0] != RIL_CARD_MAX_APPS)?
                (cardStatus->gsm_umts_subscription_app_index + 1) : imsIndex[0]);
        if (mC2kSimStatusRespParcel == NULL) {
            // There is only GSM parcel for GET_SIM_STATUS
            cardStatus->card_state = (RIL_CardState)card_state[0];
            cardStatus->universal_pin_state = (RIL_PinState)universal_pin_state[0];
            cardStatus->cdma_subscription_app_index = c2kIndex[0];
            cardStatus->num_applications = numApplications[0];
        }

        for (i = 0; i < numApplications[0]; i++) {
            pAppStatus = &cardStatus->applications[i];
            pAppStatus->app_type = (RIL_AppType)mGsmSimStatusRespParcel->readInt32();
            pAppStatus->app_state = (RIL_AppState)mGsmSimStatusRespParcel->readInt32();
            pAppStatus->perso_substate = (RIL_PersoSubstate)mGsmSimStatusRespParcel->readInt32();

            // Copy AID ptr
            s16 = NULL;
            s16Len = 0;
            s16 = mGsmSimStatusRespParcel->readString16Inplace(&s16Len);
            if (s16 != NULL) {
                size_t strLen = strnlen16to8(s16, s16Len);
                pAppStatus->aid_ptr = (char*)calloc(1, (strLen+1)*sizeof(char));
                strncpy16to8(pAppStatus->aid_ptr, s16, strLen);
            }

            // Copy app label ptr
            s16 = NULL;
            s16Len = 0;
            s16 = mGsmSimStatusRespParcel->readString16Inplace(&s16Len);
            if (s16 != NULL) {
                size_t strLen = strnlen16to8(s16, s16Len);
                pAppStatus->app_label_ptr = (char*)calloc(1, (strLen+1)*sizeof(char));
                strncpy16to8(pAppStatus->app_label_ptr, s16, strLen);
            }

            pAppStatus->pin1_replaced = mGsmSimStatusRespParcel->readInt32();
            pAppStatus->pin1 = (RIL_PinState)mGsmSimStatusRespParcel->readInt32();
            pAppStatus->pin2 = (RIL_PinState)mGsmSimStatusRespParcel->readInt32();
            RLOGD("[RpSimController] (slot %d) GSM Application %d, (%d, %d, %d, %s, %s, %d, %d, %d)",
            getSlotId(), i, pAppStatus->app_type, pAppStatus->app_state, pAppStatus->perso_substate,
            pAppStatus->aid_ptr, pAppStatus->app_label_ptr, pAppStatus->pin1_replaced, pAppStatus->pin1,
            pAppStatus->pin2);
        }

    }

    if (mC2kSimStatusRespParcel != NULL) {
        // Read data from C2K parcel
        card_state[1] = mC2kSimStatusRespParcel->readInt32();
        universal_pin_state[1] = mC2kSimStatusRespParcel->readInt32();
        gsmIndex[1] = mC2kSimStatusRespParcel->readInt32();
        c2kIndex[1] = mC2kSimStatusRespParcel->readInt32();
        imsIndex[1] = mC2kSimStatusRespParcel->readInt32();
        numApplications[1] = mC2kSimStatusRespParcel->readInt32();
        // limit to maximum allowed applications
        if (numApplications[1] > RIL_CARD_MAX_APPS) {
            numApplications[1] = RIL_CARD_MAX_APPS;
        }
        if (mGsmSimStatusRespParcel == NULL) {
            // There is only C2K parcel for GET_SIM_STATUS
            cardStatus->card_state = (RIL_CardState)card_state[1];
            cardStatus->universal_pin_state = (RIL_PinState)universal_pin_state[1];
            cardStatus->gsm_umts_subscription_app_index = RIL_CARD_MAX_APPS;
            cardStatus->ims_subscription_app_index = RIL_CARD_MAX_APPS;
            cardStatus->cdma_subscription_app_index = 0;
            cardStatus->num_applications = numApplications[1];
        } else {
            // There are both GSM parcel and C2K parcel.
            // GSM information, card_state and universal_pin_state are written in GSM parcel handling.
            cardStatus->card_state = (RIL_CardState)card_state[0];
            cardStatus->universal_pin_state = (RIL_PinState)universal_pin_state[0];
            cardStatus->cdma_subscription_app_index =
                    ((cardStatus->ims_subscription_app_index != RIL_CARD_MAX_APPS)?
                    (cardStatus->ims_subscription_app_index + 1) :
                    (cardStatus->gsm_umts_subscription_app_index + 1));
            if (cardStatus->cdma_subscription_app_index > RIL_CARD_MAX_APPS) {
                cardStatus->cdma_subscription_app_index = RIL_CARD_MAX_APPS;
            }
            if (numApplications[1] != 0) {
                cardStatus->num_applications = (cardStatus->cdma_subscription_app_index + 1);
            } else {
                cardStatus->num_applications = numApplications[0];
            }
        }

        if (numApplications[1] > 0 && numApplications[1] <= RIL_CARD_MAX_APPS) {
            pAppStatus = &cardStatus->applications[cardStatus->cdma_subscription_app_index];
            pAppStatus->app_type = (RIL_AppType)mC2kSimStatusRespParcel->readInt32();
            pAppStatus->app_state = (RIL_AppState)mC2kSimStatusRespParcel->readInt32();
            pAppStatus->perso_substate = (RIL_PersoSubstate)mC2kSimStatusRespParcel->readInt32();
            // Copy AID ptr
            s16 = NULL;
            s16Len = 0;
            s16 = mC2kSimStatusRespParcel->readString16Inplace(&s16Len);
            if (s16 != NULL) {
                size_t strLen = strnlen16to8(s16, s16Len);
                pAppStatus->aid_ptr = (char*)calloc(1, (strLen+1)*sizeof(char));
                strncpy16to8(pAppStatus->aid_ptr, s16, strLen);
            }

            // Copy app label ptr
            s16 = NULL;
            s16Len = 0;
            s16 = mC2kSimStatusRespParcel->readString16Inplace(&s16Len);
            if (s16 != NULL) {
                size_t strLen = strnlen16to8(s16, s16Len);
                pAppStatus->app_label_ptr = (char*)calloc(1, (strLen+1)*sizeof(char));
                strncpy16to8(pAppStatus->app_label_ptr, s16, strLen);
            }

            pAppStatus->pin1_replaced = mC2kSimStatusRespParcel->readInt32();
            pAppStatus->pin1 = (RIL_PinState)mC2kSimStatusRespParcel->readInt32();
            pAppStatus->pin2 = (RIL_PinState)mC2kSimStatusRespParcel->readInt32();

            RLOGD("[RpSimController] (slot %d) C2K Application, (%d, %d, %d, %s, %s, %d, %d, %d)",
                getSlotId(), pAppStatus->app_type, pAppStatus->app_state, pAppStatus->perso_substate,
                pAppStatus->aid_ptr, pAppStatus->app_label_ptr, pAppStatus->pin1_replaced,
                pAppStatus->pin1, pAppStatus->pin2);
        }
    }

    RLOGD("[RpSimController] (slot %d) Card status: (%d, %d, %d, %d, %d, %d)", getSlotId(),
            cardStatus->card_state, cardStatus->universal_pin_state,
            cardStatus->gsm_umts_subscription_app_index, cardStatus->ims_subscription_app_index,
            cardStatus->cdma_subscription_app_index, cardStatus->num_applications);

    // Set card state according to phone type
    if (cardStatus->card_state == RIL_CARDSTATE_ABSENT) {
        card_status = RFX_SIM_STATE_ABSENT;
        getStatusManager()->setIntValue(RFX_STATUS_KEY_CARD_TYPE, 0);
        // To clear IMSI due to card removed
        getStatusManager()->setString8Value(RFX_STATUS_KEY_GSM_IMSI, String8(""));
        getStatusManager()->setString8Value(RFX_STATUS_KEY_C2K_IMSI, String8(""));
        property_set(PROPERTY_UIM_SUBSCRIBER_ID[getSlotId()], "N/A");
        // For RUIM single mode card, iccid is not set in rild. If C capability is not in this
        // card for C+C mode, there is no chance to update iccid in rild. Here is to update
        // iccid to N/A in such situation.
        property_set(PROPERTY_ICCID_SIM[getSlotId()], "N/A");
        if (mC2kSimCtrl != NULL) {
            mC2kSimCtrl->resetCdmaCardStatus();
        }
    } else if (cardStatus->card_state == RIL_CARDSTATE_PRESENT) {
        // Check phone type and source
        int nws_mode = getStatusManager()->getIntValue(RFX_STATUS_KEY_NWS_MODE, -1);
        RLOGD("[RpSimController] (slot %d) nws_mode %d", getSlotId(), nws_mode);
        if (nws_mode != 0 && mGsmSimStatusRespParcel != NULL) {
            // GSM
            appState =
                    cardStatus->applications[cardStatus->gsm_umts_subscription_app_index].app_state;
        } else if (mC2kSimStatusRespParcel != NULL){
            // C2K
            appState =
                    cardStatus->applications[cardStatus->cdma_subscription_app_index].app_state;
        }

        switch (appState) {
            case RIL_APPSTATE_READY:
                card_status = RFX_SIM_STATE_READY;
                break;
            case RIL_APPSTATE_PIN:
            case RIL_APPSTATE_PUK:
            case RIL_APPSTATE_SUBSCRIPTION_PERSO:
                card_status = RFX_SIM_STATE_LOCKED;
                break;
            default:
                card_status = RFX_SIM_STATE_NOT_READY;
                break;
        }
    }
    getStatusManager()->setIntValue(RFX_STATUS_KEY_SIM_STATE, card_status);
    return cardStatus;
}

void RpSimController::handleGetSimStatusReq(const sp<RfxMessage>& message) {
    bool sent = false;
    char tmp[PROPERTY_VALUE_MAX] = {0};

    if (mCurrentGetSimStatusReq == 0) {
        // Check card type
        if (supportCardType(ICC_SIM) || supportCardType(ICC_USIM)) {
            // There is GSM application in the SIM card
            sp<RfxMessage> request = RfxMessage::obtainRequest(RADIO_TECH_GROUP_GSM,
                message->getId(), message);

            // send request to GSM RILD
            sent = requestToRild(request);

            if (sent) {
                mGsmGetSimStatusReq = message->getId();
                RLOGD("[RpSimController] send GSM RILD GET_SIM_STATUS (slot %d)", getSlotId());
            }
        }

        if ((supportCardType(ICC_CSIM) || supportCardType(ICC_RUIM)) && (getSlotId() ==
                getNonSlotScopeStatusManager()->getIntValue(
                RFX_STATUS_KEY_ACTIVE_CDMALTE_MODE_SLOT))) {
            // There is C2K application in the SIM card
            sp<RfxMessage> c2k_request = RfxMessage::obtainRequest(RADIO_TECH_GROUP_C2K,
                message->getId(), message);

            // send request to C2K RILD
            sent = requestToRild(c2k_request);

            if (sent) {
                mC2kGetSimStatusReq = message->getId();
                RLOGD("[RpSimController] send C2K RILD GET_SIM_STATUS (slot %d)", getSlotId());
            }
        }

        if (mGsmGetSimStatusReq == 0 && mC2kGetSimStatusReq == 0) {
            if (getStatusManager()->getIntValue(RFX_STATUS_KEY_CARD_TYPE) == 0) {
                sp<RfxMessage> request = RfxMessage::obtainRequest(RADIO_TECH_GROUP_GSM,
                    message->getId(), message);
                // send request to GSM RILD
                sent = requestToRild(request);
                if (sent) {
                    mGsmGetSimStatusReq = message->getId();
                    RLOGD("[RpSimController] send GET_SIM_STATUS, but no card type (slot %d)",
                            getSlotId());
                }
            } else {
                sp<RfxMessage> response = RfxMessage::obtainResponse(RIL_E_GENERIC_FAILURE,
                        message);
                responseToRilj(response);
                RLOGD("[RpSimController] Response RIL_E_GENERIC_FAILURE (slot %d)", getSlotId());
                return;
            }
        }

        if (mGsmGetSimStatusReq != 0 || mC2kGetSimStatusReq != 0) {
            // At least one request to GSM RILD or C2K RILD
            mCurrentGetSimStatusReq = message->getId();
            RLOGD("[RpSimController] mCurrentGetSimStatusReq %d (slot %d)", mCurrentGetSimStatusReq,
                    getSlotId());
        }
    } else {
        RLOGE("[RpSimController] onHandleRequest, why onPreviewMessage not work?! (slot %d)",
                getSlotId());
    }
}

void RpSimController::handleGetSimStatusRsp(const sp<RfxMessage>& message) {
    if (mCurrentGetSimStatusReq == message->getId()) {
        RIL_CardStatus_v6 *cardStatus = NULL;
        size_t dataPos = 0;
        Parcel *p = NULL;

        // Check the source of message
        if (message->getSource() == RADIO_TECH_GROUP_GSM && mGsmGetSimStatusReq != 0) {
            //cardStatus = decodeGetSimStatusRespTest(message->getParcel());
            // Clone the GSM GET_SIM_STATUS response parcel
            if (message->getError() == RIL_E_SUCCESS) {
                mGsmSimStatusRespParcel = new Parcel();
                Parcel *p = message->getParcel();
                dataPos = p->dataPosition();
                mGsmSimStatusRespParcel->appendFrom(p, dataPos, (p->dataSize() - dataPos));
                mGsmSimStatusRespParcel->setDataPosition(0);
            } else {
                mGetSimStatusErr = message->getError();
                RLOGE("[RpSimController] Response from GSM got error %d !! (slot %d)",
                        mGetSimStatusErr, getSlotId());
            }
            mGsmGetSimStatusReq = 0;
        } else if (message->getSource() == RADIO_TECH_GROUP_C2K && mC2kGetSimStatusReq != 0) {
            // Clone the C2K GET_SIM_STATUS response parcel
            if (message->getError() == RIL_E_SUCCESS) {
                mC2kSimStatusRespParcel = new Parcel();
                Parcel *p = message->getParcel();
                dataPos = p->dataPosition();
                mC2kSimStatusRespParcel->appendFrom(p, dataPos, (p->dataSize() - dataPos));
                mC2kSimStatusRespParcel->setDataPosition(0);
            } else {
                mGetSimStatusErr = message->getError();
                RLOGE("[RpSimController] Response from C2K got error %d !! (slot %d)",
                        mGetSimStatusErr, getSlotId());
            }
            mC2kGetSimStatusReq = 0;
        } else {
            RLOGE("[RpSimController] Should not receive the response!! (slot %d)", getSlotId());
            return;
        }

        if (mGsmGetSimStatusReq == 0 && mC2kGetSimStatusReq == 0) {
            // obtain response message. Wait for HP provide the new API
            sp<RfxMessage> response = RfxMessage::obtainResponse(mGetSimStatusErr,
                message);

            if (mGetSimStatusErr == RIL_E_SUCCESS) {
                // decode parcel
                RLOGD("[RpSimController] handleGetSimStatusResp, decode parcel(s) (slot %d)",
                        getSlotId());
                cardStatus = decodeGetSimStatusResp();

                // get parcel from the message and write data
                Parcel* p = response->getParcel();

                p->writeInt32(cardStatus->card_state);
                p->writeInt32(cardStatus->universal_pin_state);
                p->writeInt32(cardStatus->gsm_umts_subscription_app_index);
                p->writeInt32(cardStatus->cdma_subscription_app_index);
                p->writeInt32(cardStatus->ims_subscription_app_index);
                p->writeInt32(cardStatus->num_applications);

                RIL_AppStatus* pApp = NULL;
                for (int i = 0; i < cardStatus->num_applications; i++) {
                    pApp = &cardStatus->applications[i];

                    p->writeInt32(pApp->app_type);
                    p->writeInt32(pApp->app_state);
                    p->writeInt32(pApp->perso_substate);
                    writeStringToParcel(p, (const char*)(pApp->aid_ptr));
                    writeStringToParcel(p, (const char*)(pApp->app_label_ptr));
                    p->writeInt32(pApp->pin1_replaced);
                    p->writeInt32(pApp->pin1);
                    p->writeInt32(pApp->pin2);
                }
            } else {
                RLOGD("[RpSimController] handleGetSimStatusResp, return error code (slot %d)",
                        getSlotId());
            }
            responseToRilj(response);

            mCurrentGetSimStatusReq = 0;
            mGetSimStatusErr = RIL_E_SUCCESS;

            if (cardStatus != NULL) {
                int i = 0;
                for (i = 0; i < cardStatus->num_applications; i++) {
                    // release aid and app label
                    if (cardStatus->applications[i].aid_ptr != NULL) {
                        free(cardStatus->applications[i].aid_ptr);
                    }
                    if (cardStatus->applications[i].app_label_ptr != NULL) {
                        free(cardStatus->applications[i].app_label_ptr);
                    }
                }
                free(cardStatus);
            }
            if (mGsmSimStatusRespParcel != NULL) {
                delete(mGsmSimStatusRespParcel);
                mGsmSimStatusRespParcel = NULL;
            }
            if (mC2kSimStatusRespParcel != NULL) {
                delete(mC2kSimStatusRespParcel);
                mC2kSimStatusRespParcel = NULL;
            }
        }
    }
}

void RpSimController::handleGetImsiReq(const sp<RfxMessage>& message) {
    Parcel *req = message->getParcel();
    size_t dataPos = 0;
    int count = 0;

    dataPos = req->dataPosition();

    count = req->readInt32();

    if (count == 1) {
        // There is only one parameter "AID" in the request
        char *aid_ptr = strdupReadString(req);
        RLOGD("[RpSimController] handleGetImsiReq aid %s (slot %d)", aid_ptr, getSlotId());
        // Pass the request to RILD directly
        req->setDataPosition(dataPos);
        sp<RfxMessage> request = RfxMessage::obtainRequest(choiceDestViaAid(aid_ptr),
            message->getId(), message, true);
        // Reset the parcel position
        //request->getParcel()->appendFrom(req, dataPos, (req->dataSize() - dataPos));
        // Reset the cloned parcel position
        //request->getParcel()->setDataPosition(0);
        requestToRild(request);

        if (aid_ptr != NULL) {
            free (aid_ptr);
        }
    } else {
        RLOGE("[RpSimController] handleGetImsiReq but payload format is wrong! (slot %d)",
                getSlotId());
    }
}

void RpSimController::handleGetImsiRsp(const sp<RfxMessage>& message) {
    Parcel *req = message->getParcel();
    size_t s16Len = 0;
    size_t dataPos = req->dataPosition();
    Parcel *p = NULL;

    // Check phone type and source
    if (message->getError() == RIL_E_SUCCESS) {
        // Get IMSI then notify.
        String8 str8(req->readString16Inplace(&s16Len));
        RLOGD("[RpSimController] handleGetImsiRsp : %s", str8.string());
        if (message->getSource() == RADIO_TECH_GROUP_GSM) {
            getStatusManager()->setString8Value(RFX_STATUS_KEY_GSM_IMSI, str8);
        } else if (message->getSource() == RADIO_TECH_GROUP_C2K) {
            getStatusManager()->setString8Value(RFX_STATUS_KEY_C2K_IMSI, str8);
            property_set(PROPERTY_UIM_SUBSCRIBER_ID[getSlotId()], str8.string());
        }
        req->setDataPosition(dataPos);

    }
    sp<RfxMessage> response = RfxMessage::obtainResponse(message->getError(),
                message, true);

    // Reset the parcel position
    //p = response->getParcel();
    //dataPos = req->dataPosition();

    // Send to RILJ directly
    //p->appendFrom(req, dataPos, (req->dataSize() - dataPos));
    //p->setDataPosition(0);

    responseToRilj(response);
}

void RpSimController::handlePinPukReq(const sp<RfxMessage>& message) {
    RILD_RadioTechnology_Group dest = choiceDestViaCurrCardType();

    if (dest == RADIO_TECH_GROUP_GSM && mGsmSimCtrl != NULL) {
        mGsmSimCtrl->handlePinPukReq(message);
    } else if (dest == RADIO_TECH_GROUP_C2K && mC2kSimCtrl != NULL) {
        mC2kSimCtrl->handlePinPukReq(message);
    } else {
        RLOGE("[RpSimController] handlePinPukReq, can't dispatch the request!");
    }
}

void RpSimController::handlePinPukRsp(const sp<RfxMessage>& message) {
    RILD_RadioTechnology_Group src = message->getSource();

    if (src == RADIO_TECH_GROUP_GSM && mGsmSimCtrl != NULL) {
        mGsmSimCtrl->handlePinPukRsp(message);
    } else if (src == RADIO_TECH_GROUP_C2K && mC2kSimCtrl != NULL) {
        mC2kSimCtrl->handlePinPukRsp(message);
    } else {
        RLOGE("[RpSimController] handlePinPukRsp, can't dispatch the response!");
    }
}

void RpSimController::handleQuerySimFacilityReq(const sp<RfxMessage>& message) {
    Parcel* p = message->getParcel();
    size_t dataPos = p->dataPosition();
    size_t s16Len = 0;
    const char16_t *s16 = NULL;
    RILD_RadioTechnology_Group dest = RADIO_TECH_GROUP_GSM;
    char* aid = NULL;

    // count string
    p->readInt32();
    // facility
    p->readString16Inplace(&s16Len);
    // password
    s16Len = 0;
    p->readString16Inplace(&s16Len);
    // serviceClass
    s16Len = 0;
    p->readString16Inplace(&s16Len);

    // Get AID
    s16 = NULL;
    s16Len = 0;
    s16 = p->readString16Inplace(&s16Len);
    if (s16 != NULL) {
        size_t strLen = strnlen16to8(s16, s16Len);
        aid = (char*)calloc(1, (strLen+1)*sizeof(char));
        strncpy16to8(aid, s16, strLen);
        RLOGD("[RpGsmSimController] handleQuerySimFacilityReq aid %s (slot %d)", aid, getSlotId());
    }

    dest = choiceDestViaAid(aid);

    if (aid != NULL) {
        free(aid);
    }

    // Reset the payload position in the message before pass to GSM or C2K
    p->setDataPosition(dataPos);

    if (dest == RADIO_TECH_GROUP_GSM && mGsmSimCtrl != NULL) {
        mGsmSimCtrl->handleQuerySimFacilityReq(message);
    } else if (dest == RADIO_TECH_GROUP_C2K && mC2kSimCtrl != NULL) {
        mC2kSimCtrl->handleQuerySimFacilityReq(message);
    } else {
        RLOGE("[RpSimController] handleQuerySimFacilityReq, can't dispatch the request!");
    }
}

void RpSimController::handleQuerySimFacilityRsp(const sp<RfxMessage>& message) {
    RILD_RadioTechnology_Group src = message->getSource();

    if (src == RADIO_TECH_GROUP_GSM && mGsmSimCtrl != NULL) {
        mGsmSimCtrl->handleQuerySimFacilityRsp(message);
    } else if (src == RADIO_TECH_GROUP_C2K && mC2kSimCtrl != NULL) {
        mC2kSimCtrl->handleQuerySimFacilityRsp(message);
    } else {
        RLOGE("[RpSimController] handleQuerySimFacilityRsp, can't dispatch the response!");
    }
}

void RpSimController::handleSetSimFacilityReq(const sp<RfxMessage>& message) {
    Parcel* p = message->getParcel();
    size_t dataPos = p->dataPosition();
    size_t s16Len = 0;
    const char16_t *s16 = NULL;
    RILD_RadioTechnology_Group dest = RADIO_TECH_GROUP_GSM;
    char* aid = NULL;

    // count string
    p->readInt32();
    // facility
    p->readString16Inplace(&s16Len);
    // lockString
    s16Len = 0;
    p->readString16Inplace(&s16Len);
    // password
    s16Len = 0;
    p->readString16Inplace(&s16Len);
    // serviceClass
    s16Len = 0;
    p->readString16Inplace(&s16Len);

    // Get AID
    s16 = NULL;
    s16Len = 0;
    s16 = p->readString16Inplace(&s16Len);
    if (s16 != NULL) {
        size_t strLen = strnlen16to8(s16, s16Len);
        aid = (char*)calloc(1, (strLen+1)*sizeof(char));
        strncpy16to8(aid, s16, strLen);
        RLOGD("[RpGsmSimController] handleSetSimFacilityReq aid %s (slot %d)", aid, getSlotId());
    }

    dest = choiceDestViaAid(aid);

    if (aid != NULL) {
        free(aid);
    }

    // Reset the payload position in the message before pass to GSM or C2K
    p->setDataPosition(dataPos);

    if (dest == RADIO_TECH_GROUP_GSM && mGsmSimCtrl != NULL) {
        mGsmSimCtrl->handleSetSimFacilityReq(message);
    } else if (dest == RADIO_TECH_GROUP_C2K && mC2kSimCtrl != NULL) {
        mC2kSimCtrl->handleSetSimFacilityReq(message);
    } else {
        RLOGE("[RpSimController] handleSetSimFacilityReq, can't dispatch the request!");
    }
}

void RpSimController::handleSetSimFacilityRsp(const sp<RfxMessage>& message) {
    RILD_RadioTechnology_Group src = message->getSource();

    if (src == RADIO_TECH_GROUP_GSM && mGsmSimCtrl != NULL) {
        mGsmSimCtrl->handleSetSimFacilityRsp(message);
    } else if (src == RADIO_TECH_GROUP_C2K && mC2kSimCtrl != NULL) {
        mC2kSimCtrl->handleSetSimFacilityRsp(message);
    } else {
        RLOGE("[RpSimController] handleSetSimFacilityRsp, can't dispatch the response!");
    }
}

void RpSimController::handleIccOpenChannelReq(const sp<RfxMessage>& message) {
    RILD_RadioTechnology_Group dest = RADIO_TECH_GROUP_GSM;
    if (supportCardType(ICC_CSIM) || supportCardType(ICC_RUIM)) {
        if (getSlotId() == getNonSlotScopeStatusManager()->getIntValue(
                RFX_STATUS_KEY_ACTIVE_CDMALTE_MODE_SLOT)) {
            dest = RADIO_TECH_GROUP_C2K;
        }
    }
    if (dest == RADIO_TECH_GROUP_GSM && mGsmSimCtrl != NULL) {
        mGsmSimCtrl->handleIccOpenChannelReq(message);
    } else if (dest == RADIO_TECH_GROUP_C2K && mC2kSimCtrl != NULL) {
        mC2kSimCtrl->handleIccOpenChannelReq(message);
    } else {
        RLOGE("[RpSimController] handleIccOpenChannelReq, can't dispatch the request!");
    }
}

void RpSimController::handleIccOpenChannelRsp(const sp<RfxMessage>& message) {
    RILD_RadioTechnology_Group src = message->getSource();

    if (src == RADIO_TECH_GROUP_GSM && mGsmSimCtrl != NULL) {
        mGsmSimCtrl->handleIccOpenChannelRsp(message);
    } else if (src == RADIO_TECH_GROUP_C2K && mC2kSimCtrl != NULL) {
        mC2kSimCtrl->handleIccOpenChannelRsp(message);
    } else {
        RLOGE("[RpSimController] handleIccOpenChannelRsp, can't dispatch the response!");
    }
}

void RpSimController::handleIccCloseChannelReq(const sp<RfxMessage>& message) {
    RILD_RadioTechnology_Group dest = RADIO_TECH_GROUP_GSM;
    if (supportCardType(ICC_CSIM) || supportCardType(ICC_RUIM)) {
        if (getSlotId() == getNonSlotScopeStatusManager()->getIntValue(
                RFX_STATUS_KEY_ACTIVE_CDMALTE_MODE_SLOT)) {
            dest = RADIO_TECH_GROUP_C2K;
        }
    }
    if (dest == RADIO_TECH_GROUP_GSM && mGsmSimCtrl != NULL) {
        mGsmSimCtrl->handleIccCloseChannelReq(message);
    } else if (dest == RADIO_TECH_GROUP_C2K && mC2kSimCtrl != NULL) {
        mC2kSimCtrl->handleIccCloseChannelReq(message);
    } else {
        RLOGE("[RpSimController] handleIccCloseChannelReq, can't dispatch the request!");
    }
}

void RpSimController::handleIccCloseChannelRsp(const sp<RfxMessage>& message) {
    RILD_RadioTechnology_Group src = message->getSource();

    if (src == RADIO_TECH_GROUP_GSM && mGsmSimCtrl != NULL) {
        mGsmSimCtrl->handleIccCloseChannelRsp(message);
    } else if (src == RADIO_TECH_GROUP_C2K && mC2kSimCtrl != NULL) {
        mC2kSimCtrl->handleIccCloseChannelRsp(message);
    } else {
        RLOGE("[RpSimController] handleIccCloseChannelRsp, can't dispatch the response!");
    }
}

void RpSimController::handleTransmitApduReq(const sp<RfxMessage>& message) {
    RILD_RadioTechnology_Group dest = RADIO_TECH_GROUP_GSM;
    if (supportCardType(ICC_CSIM) || supportCardType(ICC_RUIM)) {
        if (getSlotId() == getNonSlotScopeStatusManager()->getIntValue(
                RFX_STATUS_KEY_ACTIVE_CDMALTE_MODE_SLOT)) {
            dest = RADIO_TECH_GROUP_C2K;
        }
    }
    if (dest == RADIO_TECH_GROUP_GSM && mGsmSimCtrl != NULL) {
        mGsmSimCtrl->handleTransmitApduReq(message);
    } else if (dest == RADIO_TECH_GROUP_C2K && mC2kSimCtrl != NULL) {
        mC2kSimCtrl->handleTransmitApduReq(message);
    } else {
        RLOGE("[RpSimController] handleTransmitApduReq, can't dispatch the request!");
    }
}

void RpSimController::handleTransmitApduRsp(const sp<RfxMessage>& message) {
    RILD_RadioTechnology_Group src = message->getSource();

    if (src == RADIO_TECH_GROUP_GSM && mGsmSimCtrl != NULL) {
        mGsmSimCtrl->handleTransmitApduRsp(message);
    } else if (src == RADIO_TECH_GROUP_C2K && mC2kSimCtrl != NULL) {
        mC2kSimCtrl->handleTransmitApduRsp(message);
    } else {
        RLOGE("[RpSimController] handleTransmitApduRsp, can't dispatch the response!");
    }
}

void RpSimController::handleGetAtrReq(const sp<RfxMessage>& message) {
    RILD_RadioTechnology_Group dest = RADIO_TECH_GROUP_GSM;
    if (supportCardType(ICC_CSIM) || supportCardType(ICC_RUIM)) {
        if (getSlotId() == getNonSlotScopeStatusManager()->getIntValue(
                RFX_STATUS_KEY_ACTIVE_CDMALTE_MODE_SLOT)) {
            dest = RADIO_TECH_GROUP_C2K;
        }
    }
    if (dest == RADIO_TECH_GROUP_GSM && mGsmSimCtrl != NULL) {
        mGsmSimCtrl->handleGetAtrReq(message);
    } else if (dest == RADIO_TECH_GROUP_C2K && mC2kSimCtrl != NULL) {
        mC2kSimCtrl->handleGetAtrReq(message);
    } else {
        RLOGE("[RpSimController] handleGetAtrReq, can't dispatch the request!");
    }
}

void RpSimController::handleGetAtrRsp(const sp<RfxMessage>& message) {
    RILD_RadioTechnology_Group src = message->getSource();

    if (src == RADIO_TECH_GROUP_GSM && mGsmSimCtrl != NULL) {
        mGsmSimCtrl->handleGetAtrRsp(message);
    } else if (src == RADIO_TECH_GROUP_C2K && mC2kSimCtrl != NULL) {
        mC2kSimCtrl->handleGetAtrRsp(message);
    } else {
        RLOGE("[RpSimController] handleGetAtrRsp, can't dispatch the response!");
    }
}

void RpSimController::handleSetUiccSubscriptionReq(const sp<RfxMessage>& message) {
    RILD_RadioTechnology_Group dest = RADIO_TECH_GROUP_GSM;
    if (supportCardType(ICC_CSIM) || supportCardType(ICC_RUIM)) {
        if (getSlotId() == getNonSlotScopeStatusManager()->getIntValue(
                RFX_STATUS_KEY_ACTIVE_CDMALTE_MODE_SLOT)) {
            dest = RADIO_TECH_GROUP_C2K;
        }
    }
    if (dest == RADIO_TECH_GROUP_GSM && mGsmSimCtrl != NULL) {
        mGsmSimCtrl->handleSetUiccSubscriptionReq(message);
    } else if (dest == RADIO_TECH_GROUP_C2K && mC2kSimCtrl != NULL) {
        mC2kSimCtrl->handleSetUiccSubscriptionReq(message);
    } else {
        RLOGE("[RpSimController] handleSetUiccSubscriptionReq, can't dispatch the request!");
    }
}

void RpSimController::handleSetUiccSubscriptionRsp(const sp<RfxMessage>& message) {
    RILD_RadioTechnology_Group src = message->getSource();

    if (src == RADIO_TECH_GROUP_GSM && mGsmSimCtrl != NULL) {
        mGsmSimCtrl->handleSetUiccSubscriptionRsp(message);
    } else if (src == RADIO_TECH_GROUP_C2K && mC2kSimCtrl != NULL) {
        mC2kSimCtrl->handleSetUiccSubscriptionRsp(message);
    } else {
        RLOGE("[RpSimController] handleSetUiccSubscriptionRsp, can't dispatch the response!");
    }
}

void RpSimController::handleSimIoReq(const sp<RfxMessage>& message) {
    RLOGD("[RpSimController] handleSimIoReq (slot %d)", getSlotId());

    Parcel* p = message->getParcel();
    size_t dataPos = p->dataPosition();
    size_t s16Len = 0;
    const char16_t *s16 = NULL;
    RILD_RadioTechnology_Group dest = RADIO_TECH_GROUP_GSM;
    char* aid = NULL;

    // command
    p->readInt32();
    // field
    p->readInt32();
    // path
    p->readString16Inplace(&s16Len);
    // p1
    p->readInt32();
    // p2
    p->readInt32();
    // p3
    p->readInt32();
    // data
    p->readString16Inplace(&s16Len);
    // PIN2
    p->readString16Inplace(&s16Len);

    // Get AID
    s16 = NULL;
    s16Len = 0;
    s16 = p->readString16Inplace(&s16Len);
    if (s16 != NULL) {
        size_t strLen = strnlen16to8(s16, s16Len);
        aid = (char*)calloc(1, (strLen+1)*sizeof(char));
        strncpy16to8(aid, s16, strLen);
        RLOGD("[RpSimController] handleSimIoReq aid %s (slot %d)", aid, getSlotId());
    }

    dest = choiceDestViaAid(aid);

    if (aid != NULL) {
        free(aid);
    }

    // Reset the payload position in the message before pass to GSM or C2K
    p->setDataPosition(dataPos);

    if (dest == RADIO_TECH_GROUP_GSM && mGsmSimCtrl != NULL) {
        mGsmSimCtrl->handleSimIoReq(message);
    } else if (dest == RADIO_TECH_GROUP_C2K && mC2kSimCtrl != NULL) {
        mC2kSimCtrl->handleSimIoReq(message);
    } else {
        RLOGE("[RpSimController] handleSimIoReq, can't dispatch the request!");
    }

}

void RpSimController::handleSimIoRsp(const sp<RfxMessage>& message) {
    RILD_RadioTechnology_Group src = message->getSource();

    if (src == RADIO_TECH_GROUP_GSM && mGsmSimCtrl != NULL) {
        mGsmSimCtrl->handleSimIoRsp(message);
    } else if (src == RADIO_TECH_GROUP_C2K && mC2kSimCtrl != NULL) {
        mC2kSimCtrl->handleSimIoRsp(message);
    } else {
        RLOGE("[RpSimController] handleSimIoRsp, can't dispatch the response!");
    }
}

void RpSimController::handleSimAuthenticationReq(const sp<RfxMessage>& message) {
    RLOGD("[RpSimController] handleSimAuthenticationReq (slot %d)", getSlotId());

    Parcel* p = message->getParcel();
    size_t s16Len = 0;
    const char16_t *s16 = NULL;
    RILD_RadioTechnology_Group dest = RADIO_TECH_GROUP_GSM;
    char* aid = NULL;

    // authContext
    p->readInt32();
    // data
    p->readString16Inplace(&s16Len);

    // Get AID
    s16 = NULL;
    s16Len = 0;
    s16 = p->readString16Inplace(&s16Len);
    if (s16 != NULL) {
        size_t strLen = strnlen16to8(s16, s16Len);
        aid = (char*)calloc(1, (strLen+1)*sizeof(char));
        strncpy16to8(aid, s16, strLen);
        RLOGD("[RpSimController] handleSimAuthenticationReq aid %s (slot %d)", aid, getSlotId());
    }

    dest = choiceDestViaAid(aid);

    if (aid != NULL) {
        free(aid);
    }

    // Reset the payload position in the message before pass to GSM or C2K
    p->setDataPosition(0);

    if (dest == RADIO_TECH_GROUP_GSM && mGsmSimCtrl != NULL) {
        mGsmSimCtrl->handleSimAuthenticationReq(message);
    } else if (dest == RADIO_TECH_GROUP_C2K && mC2kSimCtrl != NULL) {
        mC2kSimCtrl->handleSimAuthenticationReq(message);
    } else {
        RLOGE("[RpSimController] handleSimAuthenticationReq, can't dispatch the request!");
    }
}

void RpSimController::handleSimAuthenticationRsp(const sp<RfxMessage>& message) {
    RILD_RadioTechnology_Group src = message->getSource();

    if (src == RADIO_TECH_GROUP_GSM && mGsmSimCtrl != NULL) {
        mGsmSimCtrl->handleSimAuthenticationRsp(message);
    } else if (src == RADIO_TECH_GROUP_C2K && mC2kSimCtrl != NULL) {
        mC2kSimCtrl->handleSimAuthenticationRsp(message);
    } else {
        RLOGE("[RpSimController] handleSimAuthenticationRsp, can't dispatch the response!");
    }
}

void RpSimController::handleSimTransmitApduBasicReq(
        const sp<RfxMessage>& message) {
    RILD_RadioTechnology_Group dest = choiceDestViaCurrCardType();

    if (dest == RADIO_TECH_GROUP_GSM && mGsmSimCtrl != NULL) {
        mGsmSimCtrl->handleSimTransmitApduBasicReq(message);
    } else if (dest == RADIO_TECH_GROUP_C2K && mC2kSimCtrl != NULL) {
        mC2kSimCtrl->handleSimTransmitApduBasicReq(message);
    } else {
        RLOGE("[RpSimController] handleSimTransmitApduBasicReq, can't dispatch the request!");
    }
}

void RpSimController::handleSimTransmitApduBasicRsp(
        const sp<RfxMessage>& message) {
    RILD_RadioTechnology_Group src = message->getSource();

    if (src == RADIO_TECH_GROUP_GSM && mGsmSimCtrl != NULL) {
        mGsmSimCtrl->handleSimTransmitApduBasicRsp(message);
    } else if (src == RADIO_TECH_GROUP_C2K && mC2kSimCtrl != NULL) {
        mC2kSimCtrl->handleSimTransmitApduBasicRsp(message);
    } else {
        RLOGE("[RpSimController] handleSimAuthenticationRsp, can't dispatch the response!");
    }
}

void RpSimController::handleLocalCardTypeNotify(const sp<RfxMessage>& message) {
    int cardType = 0;
    char *fullUiccType = NULL;
    Parcel *payload = message->getParcel();

    do {
        fullUiccType = strdupReadString(payload);

        // If there is "SIM", it always must be put in the first!
        if (strncmp(fullUiccType, "SIM", 3) == 0) {
            cardType |= RFX_CARD_TYPE_SIM;
        } else if (strncmp(fullUiccType, "N/A", 3) == 0) {
            cardType = 0;
        } else if (strlen(fullUiccType) == 0) {
            cardType = -1;
        }

        if (strstr(fullUiccType, "USIM") != NULL)  {
            cardType |= RFX_CARD_TYPE_USIM;
        }

        if (strstr(fullUiccType, "CSIM") != NULL)  {
            cardType |= RFX_CARD_TYPE_CSIM;
        }

        if (strstr(fullUiccType, "RUIM") != NULL)  {
            cardType |= RFX_CARD_TYPE_RUIM;
        }

        RLOGD("[RpSimController] last card type %d, full card type (%s, %d) (slot %d)",
                mLastCardType, fullUiccType, cardType, getSlotId());

        mLastCardType = cardType;

        getStatusManager()->setIntValue(RFX_STATUS_KEY_CARD_TYPE, cardType);

        if (cardType >= 0) {
            // Modem SIM task is ready because +EUSIM is coming
            getStatusManager()->setBoolValue(RFX_STATUS_KEY_MODEM_SIM_TASK_READY, true, true);
        }

        if (fullUiccType != NULL) {
            free(fullUiccType);
        }
    } while(0);
}

void RpSimController::onRadioStateChanged(RfxStatusKeyEnum key, RfxVariant old_value,
        RfxVariant value) {
    int oldState = -1, newState = -1;

    oldState = old_value.asInt();
    newState = value.asInt();

    RLOGD("[RpSimController] onRadioStateChanged (%d, %d) (slot %d)", oldState, newState,
            getSlotId());

    if (newState == RADIO_STATE_UNAVAILABLE) {
        // Reset mGsmGetSimStatusReq, mC2kGetSimStatusReq and mCurrentGetSimStatusReq due to
        // the socket was disconnected and there is no response anymore.
        mGsmGetSimStatusReq = 0;
        mC2kGetSimStatusReq = 0;
        mCurrentGetSimStatusReq = 0;
        // Modem SIM task is not ready because radio is not available
        getStatusManager()->setBoolValue(RFX_STATUS_KEY_MODEM_SIM_TASK_READY, false, true);
        getStatusManager()->setIntValue(RFX_STATUS_KEY_SIM_STATE, RFX_SIM_STATE_NOT_READY);
        getStatusManager()->setString8Value(RFX_STATUS_KEY_GSM_IMSI, String8(""));
        getStatusManager()->setString8Value(RFX_STATUS_KEY_C2K_IMSI, String8(""));
        property_set(PROPERTY_UIM_SUBSCRIBER_ID[getSlotId()], "");
        property_set(PROPERTY_RIL_CDMA_CARD_TYPE[getSlotId()], "");
        if (mC2kSimCtrl != NULL) {
            mC2kSimCtrl->resetCdmaCardStatus();
        }
    }
}

RILD_RadioTechnology_Group RpSimController::choiceDestByCardType() {
    return choiceDestViaCurrCardType();
}

/**
 * Switch RUIM card to SIM or switch SIM to RUIM.
 *@param cardtype  switch card type. 0: SIM   1: RUIM
 */
void RpSimController::switchCardType(int cardtype) {
    RLOGD("[RpSimController] switchCardType cardtype: %d (slot %d)", cardtype, getSlotId());

    if (((cardtype == 0) && supportCardType(ICC_RUIM)) ||
            ((cardtype == 1) && supportCardType(ICC_SIM))) {
        // send request to gsm ril
        sp<RfxMessage> request = RfxMessage::obtainRequest(getSlotId(), RADIO_TECH_GROUP_GSM,
                RIL_REQUEST_SWITCH_CARD_TYPE);
        request->getParcel()->writeInt32(1);
        request->getParcel()->writeInt32(cardtype);
        requestToRild(request);
        // send request to c2k ril
        RLOGD("[RpSimController] switchCardType cdma socket slot: %d  (slot %d)",
                getNonSlotScopeStatusManager()->getIntValue(RFX_STATUS_KEY_CDMA_SOCKET_SLOT, 0), getSlotId());
        if (getSlotId() == getNonSlotScopeStatusManager()->getIntValue(RFX_STATUS_KEY_CDMA_SOCKET_SLOT, 0)) {
            sp<RfxMessage> c2k_request = RfxMessage::obtainRequest(getSlotId(), RADIO_TECH_GROUP_C2K,
                    RIL_REQUEST_SWITCH_CARD_TYPE);
            request->getParcel()->writeInt32(1);
            request->getParcel()->writeInt32(cardtype);
            requestToRild(request);
        }
    } else {
        RLOGE("[RpSimController] switchCardType, can't switch card type!");
    }
}

