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
#include "RfxLog.h"
#include "RpIrController.h"
#include "RpIrLwgHandler.h"
#include "RpIrStrategy.h"
#include "power/RadioConstants.h" // move to RfxStatusDefs.h

#define RFX_LOG_TAG "[IRC][LwgHandler]"

/*****************************************************************************
 * Class RpIrLwgHandler
 *****************************************************************************/

RFX_IMPLEMENT_CLASS("RpIrLwgHandler", RpIrLwgHandler, RpIrBaseHandler);

RpIrLwgHandler::RpIrLwgHandler() :
        RpIrBaseHandler(),
        mModemResumeSessionId(-1),
        mbSuspendEnabled(false) {
}

RpIrLwgHandler::RpIrLwgHandler(RpIrController *ir_controller) :
        RpIrBaseHandler(ir_controller),
        mModemResumeSessionId(-1),
        mbSuspendEnabled(false) {
}

RpIrLwgHandler::~RpIrLwgHandler() {
}

void RpIrLwgHandler::onInit() {
    RpIrBaseHandler::onInit(); // Required: invoke super class implementation
    registerBaseListener();
    setState(STATE_INIT);
}

bool RpIrLwgHandler::onHandleUrc(const sp<RfxMessage>& message) {
    int id = message->getId();

    switch (id) {
    case RIL_UNSOL_RESPONSE_PLMN_CHANGED:
        handlePlmnChanged(message);
        return true;
        break;
    case RIL_UNSOL_RESPONSE_REGISTRATION_SUSPENDED:
        handleRegSuspended(message);
        return true;
        break;
    default:
        break;
    }
    return false;
}

void RpIrLwgHandler::setNwsMode(NwsMode nwsMode) {
    logD(RFX_LOG_TAG, "setNwsMode: %s", Nws2Str(nwsMode));
    if (nwsMode != mPtrIrController->getNwsMode()) {
        sp<RfxAction> actForNwsModeChanged = new RfxAction0(this, &RpIrLwgHandler::onNwsModeChanged);
        mPtrIrController->setNwsMode(nwsMode, actForNwsModeChanged);
    }
    else {
        mPtrIrController->setNwsMode(nwsMode, sp<RfxAction>(NULL));
    }
    setState(STATE_NETWORK_SELECTED);
}

void RpIrLwgHandler::resumeNetwork() {
    NwsMode currentNwsMode = mPtrIrController->getNwsMode();
    logD(RFX_LOG_TAG, "resumeNetwork: currentNwsMode=%s", Nws2Str(currentNwsMode));
    sp<RfxMessage> lwgResumeRegRequest =
            RfxMessage::obtainRequest(m_slot_id, RADIO_TECH_GROUP_GSM,
            RIL_REQUEST_RESUME_REGISTRATION);
    Parcel* p = lwgResumeRegRequest->getParcel();
    p->writeInt32(1);
    p->writeInt32(mModemResumeSessionId);

    // send request to GSM RILD
    requestToRild(lwgResumeRegRequest);
}

void RpIrLwgHandler::dispose() {
    unregisterBaseListener();
    unregisterSuspendListener();
    unregisterServiceStateChangeListener();
}


void RpIrLwgHandler::setIfEnabled(bool isEnabled) {
    enableSuspend(isEnabled);
}

void RpIrLwgHandler::onSocketStateChanged(RfxStatusKeyEnum key,
        RfxVariant old_value, RfxVariant value) {
    RFX_UNUSED(key);
    RfxSocketState old_state = old_value.asSocketState();
    RfxSocketState new_state = value.asSocketState();
    
    bool old_gsm = old_state.getSocketState(RfxSocketState::SOCKET_GSM);
    bool new_gsm = new_state.getSocketState(RfxSocketState::SOCKET_GSM);

    if (old_gsm == new_gsm) {
        return;
    }

    logD(RFX_LOG_TAG, "onSocketStateChanged, state = %s, old = %s, new = %s",
                      stateToString(getState()), Bool2Str(old_gsm), Bool2Str(new_gsm));

    if (new_gsm) {
        if (getState() == STATE_INIT) {
            removeNoServiceMessage();
            if (mPtrIrController->getIrControllerEnableState()) {
                enableSuspend(true);
                resetToInitialState();
            }
            const int urc_id_list[] = {RIL_UNSOL_RESPONSE_PLMN_CHANGED};
            registerToHandleUrc(urc_id_list, sizeof(urc_id_list)/sizeof(int));
            setState(STATE_GETTING_PLMN);
        }
    } else {
        unregisterServiceStateChangeListener();
        unregisterSuspendListener();
        setState(STATE_INIT);
        mPreVoiceState = -1;
        mPreDataState = -1;
        mIsFirstRoundSearch = true;
        if (mPtrListener) {
            mPtrListener->onRadioStateChanged(false);
        }
        setServiceType(OUT_OF_SERVICE);
        if (mNoServiceTimerHandle != NULL) {
            removeNoServiceMessage();
            if (mPtrListener) {
                mPtrListener->onServiceStateChanged(OUT_OF_SERVICE);
            }
        }
    }
}

void RpIrLwgHandler::onRadioCapabilityChanged(RfxStatusKeyEnum key,
        RfxVariant old_value, RfxVariant value) {
    RFX_UNUSED(key);
    RIL_RADIO_CAPABILITY old_cap, new_cap;
    old_cap = (RIL_RADIO_CAPABILITY)old_value.asInt();
    new_cap = (RIL_RADIO_CAPABILITY)value.asInt();

    logD(RFX_LOG_TAG, "onRadioCapabilityChanged, state = %s, old = %d, new = %d",
                      stateToString(getState()), old_cap, new_cap);

    if (!CAPABILITY_HAS_GSM(old_cap) && CAPABILITY_HAS_GSM(new_cap)) {
        removeNoServiceMessage();
        registerServiceStateChangeListener();
        if (mPtrListener) {
            mPtrListener->onRadioStateChanged(true);
        }
        mPreVoiceState = -1;
        mPreDataState = -1;
        mIsFirstRoundSearch = true;
    } else if (CAPABILITY_HAS_GSM(old_cap) && !CAPABILITY_HAS_GSM(new_cap)) {
        unregisterServiceStateChangeListener();
        setState(STATE_GETTING_PLMN);
        mPreVoiceState = -1;
        mPreDataState = -1;
        mIsFirstRoundSearch = true;
        if (mPtrListener) {
            mPtrListener->onRadioStateChanged(false);
        }
        setServiceType(OUT_OF_SERVICE);
        if (mNoServiceTimerHandle != NULL) {
            removeNoServiceMessage();
            if (mPtrListener) {
                mPtrListener->onServiceStateChanged(OUT_OF_SERVICE);
            }
        }
    }
}

void RpIrLwgHandler::onServiceStateChanged(RfxStatusKeyEnum key,
        RfxVariant old_value, RfxVariant value) {
    RFX_UNUSED(key);
    RFX_UNUSED(old_value);
    if (getState() == STATE_INIT) {
        logD(RFX_LOG_TAG, "onServiceStateChanged in INIT state");
        return;
    }

    RIL_RADIO_CAPABILITY radio_cap;
    radio_cap = (RIL_RADIO_CAPABILITY)getStatusManager()->getIntValue(RFX_STATUS_KEY_RADIO_CAPABILITY);

    if (!CAPABILITY_HAS_GSM(radio_cap)) {
        logD(RFX_LOG_TAG, "onServiceStateChanged but GSM radio off, ignore");
        return;
    }

    RfxNwServiceState serviceState = (RfxNwServiceState) value.asServiceState();
    int regVoiceState = convertVoiceRegState(serviceState.getRilVoiceRegState());
    int regDataState = serviceState.getRilDataRegState();
    logD(RFX_LOG_TAG, "onServiceStateChanged, VoiceState:%d, DataState:%d, mIsFirstRoundSearch:%s",
            regVoiceState, regDataState, Bool2Str(mIsFirstRoundSearch));
    if (mPreVoiceState == regVoiceState && mPreDataState == regDataState) {
        logD(RFX_LOG_TAG, "onServiceStateChanged, Voice state and Data state not change, do nothing.");
        return;
    }
    if (regVoiceState == RIL_REG_STATE_HOME
            || regVoiceState == RIL_REG_STATE_ROAMING
            || regDataState == RIL_REG_STATE_HOME
            || regDataState == RIL_REG_STATE_ROAMING) {
        setServiceType(IN_SERVICE);
        mIsFirstRoundSearch = true;
    } else if ((isPreStateBeforeNoService(mPreVoiceState) || isPreStateBeforeNoService(mPreDataState))
            && (isNoServiceState(regVoiceState) && isNoServiceState(regDataState))) {
        setServiceType(OUT_OF_SERVICE);
        mIsFirstRoundSearch = false;
    } else if (mIsFirstRoundSearch ||
            (mServiceType == OUT_OF_SERVICE && mNoServiceTimerHandle != NULL)) {
        if (mPtrIrController->mPtrSwitchStrategy->getIsLwgRadioOn()) {
            setServiceType(IN_SEARCHING);
        }
    }

    mPreVoiceState = regVoiceState;
    mPreDataState = regDataState;
}

void RpIrLwgHandler::registerBaseListener()
{
    logD(RFX_LOG_TAG, "registerBaseListener");
    getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_SOCKET_STATE,
        RfxStatusChangeCallback(this, &RpIrLwgHandler::onSocketStateChanged));
    getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_RADIO_CAPABILITY,
        RfxStatusChangeCallback(this, &RpIrLwgHandler::onRadioCapabilityChanged));
}

void RpIrLwgHandler::unregisterBaseListener()
{
    logD(RFX_LOG_TAG, "unregisterBaseListener");
    getStatusManager()->unRegisterStatusChanged(RFX_STATUS_KEY_SOCKET_STATE,
        RfxStatusChangeCallback(this, &RpIrLwgHandler::onSocketStateChanged));
    getStatusManager()->unRegisterStatusChanged(RFX_STATUS_KEY_RADIO_CAPABILITY,
        RfxStatusChangeCallback(this, &RpIrLwgHandler::onRadioCapabilityChanged));
}

void RpIrLwgHandler::unregisterSuspendListener()
{
    logD(RFX_LOG_TAG, "unregisterSuspendListener");
    const int urc_id_list[] = {
        RIL_UNSOL_RESPONSE_PLMN_CHANGED,
        RIL_UNSOL_RESPONSE_REGISTRATION_SUSPENDED
    };

    unregisterToHandleUrc(urc_id_list, sizeof(urc_id_list)/sizeof(int));
    mbSuspendEnabled = false;
}

void RpIrLwgHandler::enableSuspend(bool enable)
{
    logD(RFX_LOG_TAG, "enableSuspend: %s, mbSuspendEnabled: %s",
            Bool2Str(enable), Bool2Str(mbSuspendEnabled));
    const int urc_id_list[] = {RIL_UNSOL_RESPONSE_REGISTRATION_SUSPENDED};
    if (enable && !mbSuspendEnabled) {
        registerToHandleUrc(urc_id_list, sizeof(urc_id_list)/sizeof(int));
    } else if (!enable && mbSuspendEnabled){
        unregisterToHandleUrc(urc_id_list, sizeof(urc_id_list)/sizeof(int));
    }

    if (!mPtrIrController->isCdmaDualModeSimCard()) {
        logD(RFX_LOG_TAG, "enableSuspend: enable=%s, not ct dual mode sim, return",
                Bool2Str(enable));
        mbSuspendEnabled = enable;
        return;
    }

    if ((enable && !mbSuspendEnabled) || (!enable && mbSuspendEnabled)) {
        sp<RfxMessage> lwgRegSuspendEnabledRequest =
                RfxMessage::obtainRequest(m_slot_id, RADIO_TECH_GROUP_GSM,
                RIL_REQUEST_SET_REG_SUSPEND_ENABLED);
        Parcel* p = lwgRegSuspendEnabledRequest->getParcel();
        p->writeInt32(1);
        p->writeInt32(enable ? 1 : 0);

        // send request to GSM RILD
        requestToRild(lwgRegSuspendEnabledRequest);
    }
    mbSuspendEnabled = enable;
}

void RpIrLwgHandler::registerServiceStateChangeListener()
{
    logD(RFX_LOG_TAG, "registerServiceStateChangeListener");
    getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_SERVICE_STATE,
        RfxStatusChangeCallback(this, &RpIrLwgHandler::onServiceStateChanged));
}

void RpIrLwgHandler::unregisterServiceStateChangeListener()
{
    logD(RFX_LOG_TAG, "unregisterServiceStateChangeListener");
    getStatusManager()->unRegisterStatusChanged(RFX_STATUS_KEY_SERVICE_STATE,
        RfxStatusChangeCallback(this, &RpIrLwgHandler::onServiceStateChanged));
}

String8 RpIrLwgHandler::getSelectedPlmn() {
    return mVecPlmns.itemAt(0);
}

bool RpIrLwgHandler::isPreStateBeforeNoService(int nState)
{
    return (nState == RIL_REG_STATE_NOT_REG_SEARCHING
            || nState == RIL_REG_STATE_HOME
            || nState == RIL_REG_STATE_ROAMING);
}

bool RpIrLwgHandler::isNoServiceState(int nState)
{
    return (nState == RIL_REG_STATE_NOT_REG_NOT_SEARCHING
            || nState == RIL_REG_STATE_UNKNOWN);
}

void RpIrLwgHandler::handlePlmnChanged(const sp<RfxMessage>& message) {
    logD(RFX_LOG_TAG, "handlePlmnChanged, msg = %s", message->toString().string());
    if (getState() == STATE_INIT) {
        logD(RFX_LOG_TAG, "plmn changed in INIT state");
        return;
    }
    postponeNoServiceMessageIfNeeded(NO_SERVICE_DELAY_TIME);
    if (message->getError() == RIL_E_SUCCESS) {
        mVecPlmns.clear();
        Parcel *p = message->getParcel();
        int32_t plmn_count = p->readInt32();
        for (int32_t i = 0; i < plmn_count; i++) {
            String16 plmn = p->readString16();
            String8 plmn8(plmn);
            logD(RFX_LOG_TAG, "handlePlmnChanged() i = %d, plmn = %s", i, plmn8.string());
            mVecPlmns.add(plmn8);
        }
    }
    setState(STATE_SELECTING_NETWORK);
}

void RpIrLwgHandler::handleRegSuspended(const sp<RfxMessage>& message) {
    logD(RFX_LOG_TAG, "handleRegSuspended, msg = %s", message->toString().string());
    if (getState() == STATE_INIT) {
        logD(RFX_LOG_TAG, "reg suspended in INIT state");
        return;
    }
    postponeNoServiceMessageIfNeeded(NO_SERVICE_DELAY_TIME);
    setState(STATE_SELECTING_NETWORK);
    if (message->getError() == RIL_E_SUCCESS) {
        Parcel *p = message->getParcel();
        int32_t session_count = p->readInt32();
        mModemResumeSessionId = (int)p->readInt32();
        processPlmnChanged(getSelectedPlmn());
    }
}

