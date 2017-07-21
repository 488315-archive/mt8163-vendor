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

#include "RpDataController.h"
#include "RpDataUtils.h"

#include "RfxStatusDefs.h"

#define RFX_LOG_TAG "RP_DC"

/*****************************************************************************
 * Class RfxDataController
 * The class is created if the slot is single mode, LWG or C,
 * During class life time always communicate with one modem, gsm or c2k.
 *****************************************************************************/

RFX_IMPLEMENT_CLASS("RpDataController", RpDataController, RfxController);

RpDataController::RpDataController() :
        mInterfaceId(-1), mApnName(""), mDataInfoList(NULL) {
}

RpDataController::~RpDataController() {
}

void RpDataController::onInit() {
    RfxController::onInit();  // Required: invoke super class implementation

    logD(RFX_LOG_TAG, "onInit");
    mDataInfoList = new Vector<RpDataConnectionInfo*>();

    getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_RADIO_STATE,
        RfxStatusChangeCallback(this, &RpDataController::onRadioStateChanged));

    const int request_id_list[] = {
            RIL_REQUEST_SETUP_DATA_CALL,  // 27
            RIL_REQUEST_DEACTIVATE_DATA_CALL,  // 41
            RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE,  // 56
            RIL_REQUEST_DATA_CALL_LIST,  // 57
            RIL_REQUEST_SET_INITIAL_ATTACH_APN,  // 111
            RIL_REQUEST_ALLOW_DATA,  // 123
            RIL_REQUEST_SET_FD_MODE,  // Fast dormancy
            // VOLTE start
//            REQUEST_SETUP_DEDICATE_DATA_CALL,
//            REQUEST_DEACTIVATE_DEDICATE_DATA_CALL,
//            REQUEST_MODIFY_DATA_CALL,
//            REQUEST_ABORT_SETUP_DATA_CALL,
//            REQUEST_PCSCF_DISCOVERY_PCO,
//            REQUEST_LAST_DATA_CALL_FAIL_CAUSE,
//            REQUEST_CLEAR_DATA_BEARER,
            };
    const int urc_id_list[] = {
            RIL_UNSOL_DATA_CALL_LIST_CHANGED,  // 1010
            RIL_UNSOL_DATA_ALLOWED,  // 3046
            // VOLTE start
//            RIL_UNSOL_DEDICATE_BEARER_ACTIVATED,
//            RIL_UNSOL_DEDICATE_BEARER_MODIFIED,
//            RIL_UNSOL_DEDICATE_BEARER_DEACTIVATED,
        };

    // register request & URC id list
    // NOTE. one id can only be registered by one controller
    registerToHandleRequest(request_id_list,
            sizeof(request_id_list) / sizeof(int));
    registerToHandleUrc(urc_id_list, sizeof(urc_id_list) / sizeof(int));
}

void RpDataController::onDeinit() {
    logD(RFX_LOG_TAG, "onDeinit");
    getStatusManager()->unRegisterStatusChanged(RFX_STATUS_KEY_RADIO_STATE,
        RfxStatusChangeCallback(this, &RpDataController::onRadioStateChanged));
    RpDataUtils::freeDataList(mDataInfoList);
    mDataInfoList = NULL;
    RfxController::onDeinit();
}

bool RpDataController::onHandleRequest(const sp<RfxMessage>& message) {
    logD(RFX_LOG_TAG, "Handle request %s", RpDataUtils::requestToString(message->getId()));

    switch (message->getId()) {
    // common commands can send to md1(gsm)/md3(c2k) both.
    case RIL_REQUEST_SETUP_DATA_CALL:
        handleSetupDataRequest(message);
        break;
    case RIL_REQUEST_DEACTIVATE_DATA_CALL:
        handleDeactivateDataRequest(message);
        break;
    case RIL_REQUEST_DATA_CALL_LIST:
        handleGetDataCallListRequest(message);
        break;
    case RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE:
        handleGetLastFailCauseRequest(message);
        break;
    case RIL_REQUEST_SET_INITIAL_ATTACH_APN:
        handleSetInitialAttachApnRequest(message);
        break;
    case RIL_REQUEST_ALLOW_DATA:
        handleSetDataAllowRequest(message);
        break;
    // gsm only commands, send to c2k will return error code unsupport.
    case RIL_REQUEST_SET_FD_MODE:
        handleSetFdModeRequest(message);
        break;
    default:
        logD(RFX_LOG_TAG, "unknown request, ignore!");
        break;
    }
    return true;
}

bool RpDataController::onHandleResponse(const sp<RfxMessage>& message) {
    logD(RFX_LOG_TAG, "Handle response %s.", RpDataUtils::requestToString(message->getId()));

    switch (message->getId()) {
    case RIL_REQUEST_SETUP_DATA_CALL:
        handleSetupDataResponse(message);
        break;
    case RIL_REQUEST_DEACTIVATE_DATA_CALL:
        handleDeactivateDataResponse(message);
        break;
    case RIL_REQUEST_DATA_CALL_LIST:
        handleGetDataCallListResponse(message);
        break;
    case RIL_REQUEST_LAST_DATA_CALL_FAIL_CAUSE:
        handleGetLastFailCauseResponse(message);
        break;
    case RIL_REQUEST_SET_INITIAL_ATTACH_APN:
        handleSetInitialAttachApnResponse(message);
        break;
    case RIL_REQUEST_ALLOW_DATA:
        handleSetDataAllowResponse(message);
        break;
    case RIL_REQUEST_SET_FD_MODE:
        handleSetFdModeResponse(message);
        break;
    default:
        logD(RFX_LOG_TAG, "unknown response, ignore!");
        break;
    }
    return true;
}

bool RpDataController::onHandleUrc(const sp<RfxMessage>& message) {
    logD(RFX_LOG_TAG, "Handle URC %s", RpDataUtils::urcToString(message->getId()));

    switch (message->getId()) {
    case RIL_UNSOL_DATA_CALL_LIST_CHANGED: {
        handleDataCallListChangeUrc(message);
        break;
    }
    case RIL_UNSOL_DATA_ALLOWED: {
        handleDataAllowedUrc(message);
        break;
    }
    default:
        logD(RFX_LOG_TAG, "unknown urc, ignore!");
        break;
    }
    return true;
}

void RpDataController::handleSetupDataRequest(const sp<RfxMessage>& request) {
    // update data connection info
    // request message to create a connection, record apn & interfaceId.
    Parcel* parcel = request->getParcel();
    parcel->readInt32();  // conent length
    parcel->readString16();  // radioTechnology
    parcel->readString16();  // profile
    mApnName = parcel->readString16();  // apn
    parcel->readString16();  // user
    parcel->readString16();  // password
    parcel->readString16();  // authType
    parcel->readString16();  // protocol
    String16 idString = parcel->readString16();  // interfaceId
    mInterfaceId = atoi(String8(idString).string());
    logD(RFX_LOG_TAG, "record mApnName: %s mInterfaceId: %d", String8(mApnName).string(), mInterfaceId);
    handleRequestDefault(request);
}

void RpDataController::handleDeactivateDataRequest(const sp<RfxMessage>& request) {
    // update data connection info
    // this is a request message to release a connection, should always success.
    Parcel* parcel = request->getParcel();
    int oldCount = 0;
    int newCount = 0;
    oldCount = mDataInfoList->size();
    parcel->readInt32();  // conent length
    String16 idString = parcel->readString16();  // interfaceId string
    int id = atoi(String8(idString).string());
    logD(RFX_LOG_TAG, "deactive interfaceId:%d", id);
    for (int i = 0; i < oldCount; i++) {
        RpDataConnectionInfo* info = mDataInfoList->itemAt(i);
        if (info->interfaceId == id) {
            delete info;
            mDataInfoList->removeAt(i);
            logD(RFX_LOG_TAG, "remove interfaceId:%d from list.", id);
            break;
        }
    }
    newCount = mDataInfoList->size();
    updateDataConnectionStatus(oldCount, newCount);
    handleRequestDefault(request);
}

bool RpDataController::needsetActivePsSlot() {
    // Only SVLTE and OP09-A project need to send AT+EACTS to specify data SIM by modem's request.
    // TODO: Discuss with MD to align SVLTE design to be the same as SRLTE.
    if (RpDataUtils::isSvlteSupport() || RpDataUtils::isOP09ASupport()) {
        return true;
    }
    return false;
}

bool RpDataController::setActivePsSlotIfNeeded(const sp<RfxMessage>& request) {
    if (needsetActivePsSlot()) {
        // send AT+EACTS before AT+CGATT=1
        Parcel* parcel = request->getParcel();
        parcel->readInt32();  // pass length
        int allow = parcel->readInt32();  // get allow flag.
        logD(RFX_LOG_TAG, "setActivePsSlot: allow = %d.", allow);
        if (allow) {
            sp<RfxMessage> msg = RfxMessage::obtainRequest(getSlotId(),
                                                        RADIO_TECH_GROUP_GSM,
                                                        RIL_REQUEST_SET_ACTIVE_PS_SLOT);
            Parcel* parcel2 = msg->getParcel();
            parcel2->writeInt32(1);  // content length.
            parcel2->writeInt32(getSlotId() + 1);
            requestToRild(msg);
            return true;
        }
    }
    return false;
}

void RpDataController::handleSetDataAllowRequest(const sp<RfxMessage>& request) {
    setActivePsSlotIfNeeded(request);
    handleRequestDefault(request);
}

void RpDataController::handleGetDataCallListRequest(const sp<RfxMessage>& request) {
    handleRequestDefault(request);
}

void RpDataController::handleSetInitialAttachApnRequest(const sp<RfxMessage>& request) {
    Parcel* parcel = request->getParcel();
    RpDataUtils::storeIaProperty(getSlotId(), parcel->readString16());
    handleRequestDefault(request);
}

void RpDataController::handleGetLastFailCauseRequest(const sp<RfxMessage>& request) {
    handleRequestDefault(request);
}

void RpDataController::handleSetFdModeRequest(const sp<RfxMessage>& request) {
    sendToGsm(request);
}

void RpDataController::handleSetupDataResponse(const sp<RfxMessage>& response) {
    Parcel* parcel = response->getParcel();

    if (response->getError() != RIL_E_SUCCESS) {
        logD(RFX_LOG_TAG, "setupdata response fail!");
        responseToRilj(response);
        return;
    }

    // update data connection info
    // this is a response message to create a connection, when success update.
    int oldCount = mDataInfoList->size();
    int newCount = 0;

    Vector<RpDataConnectionInfo*>* newList = RpDataUtils::parseDataConnectionInfo(parcel);
    RpDataConnectionInfo* p = newList->itemAt(0);
    // interface Id of rild index begin is 0, setupdata id delivered begin as cid + 1.
    if (p->interfaceId == mInterfaceId - 1) {
        p->apn = mApnName;
        logD(RFX_LOG_TAG, "update cid %d apn to %s", p->interfaceId, String8(p->apn).string());
        p->dump();
    }
    mDataInfoList->add(p);
    newCount = mDataInfoList->size();
    updateDataConnectionStatus(oldCount, newCount);

    delete newList;

    responseToRilj(response);
}

void RpDataController::handleDeactivateDataResponse(const sp<RfxMessage>& response) {
    responseToRilj(response);
}

void RpDataController::handleSetDataAllowResponse(const sp<RfxMessage>& response) {
    responseToRilj(response);
}

void RpDataController::handleGetDataCallListResponse(const sp<RfxMessage>& response) {
    responseToRilj(response);
}

void RpDataController::handleSetInitialAttachApnResponse(const sp<RfxMessage>& response) {
    responseToRilj(response);
}

void RpDataController::handleGetLastFailCauseResponse(const sp<RfxMessage>& response) {
    responseToRilj(response);
}

void RpDataController::handleSetFdModeResponse(const sp<RfxMessage>& response) {
    responseToRilj(response);
}

void RpDataController::handleDataCallListChangeUrc(const sp<RfxMessage>& message) {
    Parcel* parcel = message->getParcel();

    // update data connection info
    // urc data connection list is changed, sync info totally.
    int oldCount = 0;
    int newCount = 0;
    Vector<RpDataConnectionInfo*>* newList = RpDataUtils::parseDataConnectionInfo(parcel);
    oldCount = mDataInfoList->size();
    RpDataUtils::syncDataConnectionApn(mDataInfoList, newList);
    RpDataUtils::freeDataList(mDataInfoList);
    mDataInfoList = newList;
    newCount = mDataInfoList->size();
    updateDataConnectionStatus(oldCount, newCount);

    responseToRilj(message);
}

void RpDataController::handleDataAllowedUrc(const sp<RfxMessage>& message) {
    responseToRilj(message);
}

void RpDataController::handleRequestDefault(const sp<RfxMessage>& request) {
    RILD_RadioTechnology_Group psType = RpDataUtils::getPsType(this);
    sp<RfxMessage> message = RfxMessage::obtainRequest(psType,
                                                        request->getId(),
                                                        request,
                                                        true);
    requestToRild(message);
    logD(RFX_LOG_TAG, "Send request %s to %s",
            RpDataUtils::requestToString(request->getId()),
            RpDataUtils::radioGroupToString(psType));
}

void RpDataController::updateDataConnectionStatus(int oldCount, int newCount) {
    logD(RFX_LOG_TAG, "updateDataConnectionStatus oldCount %d, newCount %d", oldCount, newCount);
    if (oldCount != newCount) {
        if (oldCount == 0) {
            getStatusManager()->setIntValue(RFX_STATUS_KEY_DATA_CONNECTION, DATA_STATE_CONNECTED);
        } else if (newCount == 0) {
            getStatusManager()->setIntValue(RFX_STATUS_KEY_DATA_CONNECTION, DATA_STATE_DISCONNECTED);
        }
    }
}

void RpDataController::sendToGsm(const sp<RfxMessage>& request) {
    logD(RFX_LOG_TAG, "request send to Gsm.");
    sp<RfxMessage> message = RfxMessage::obtainRequest(RADIO_TECH_GROUP_GSM,
                                                        request->getId(),
                                                        request,
                                                        true);
    requestToRild(message);
}

void RpDataController::onRadioStateChanged(RfxStatusKeyEnum key,
    RfxVariant old_value, RfxVariant value) {
    RFX_UNUSED(key);
    RFX_UNUSED(old_value);

    RIL_RadioState radioState = (RIL_RadioState) value.asInt();
    logD(RFX_LOG_TAG, "radioState %d", radioState);

    if (radioState == RADIO_STATE_UNAVAILABLE) {
        // do something on not available, reset status
        RpDataUtils::freeDataList(mDataInfoList);
        mDataInfoList = new Vector<RpDataConnectionInfo*>();
        getStatusManager()->setIntValue(RFX_STATUS_KEY_DATA_CONNECTION, DATA_STATE_DISCONNECTED);
    }
}

