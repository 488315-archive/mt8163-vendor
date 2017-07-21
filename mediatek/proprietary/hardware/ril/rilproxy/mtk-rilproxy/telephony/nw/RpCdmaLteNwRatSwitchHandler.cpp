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
#include "RpCdmaLteNwRatSwitchHandler.h"
#include "ril.h"

/*****************************************************************************
 * Class RpCdmaLteNwRatSwitchHandler
 *****************************************************************************/

RFX_IMPLEMENT_CLASS("RpCdmaLteNwRatSwitchHandler", RpCdmaLteNwRatSwitchHandler,
        RpCdmaNwRatSwitchHandler);

RpCdmaLteNwRatSwitchHandler::RpCdmaLteNwRatSwitchHandler() {
}

RpCdmaLteNwRatSwitchHandler::RpCdmaLteNwRatSwitchHandler(RpNwRatController* nwRatController) :
        RpCdmaNwRatSwitchHandler(nwRatController) {
}

RpCdmaLteNwRatSwitchHandler::~RpCdmaLteNwRatSwitchHandler() {
}

void RpCdmaLteNwRatSwitchHandler::requestGetPreferredNetworkType(const sp<RfxMessage>& message) {
    RpBaseNwRatSwitchHandler::requestGetPreferredNetworkType(RADIO_TECH_GROUP_C2K, message);
}

void RpCdmaLteNwRatSwitchHandler::responseGetPreferredNetworkType(const sp<RfxMessage>& response) {
    int error = response->getError();
    if (error != RIL_E_SUCCESS) {
        // error
        logD(LOG_TAG, "responseGetPreferredNetworkType(), error is %d.", error);
    } else {
        // ok
        int source = response->getSource();
        int32_t stgCount;
        int32_t nwType;
        response->getParcel()->readInt32(&stgCount);
        response->getParcel()->readInt32(&nwType);
        // TODO combine
        if (source == RADIO_TECH_GROUP_C2K) {
            sp<RfxMessage> resToRilj = RfxMessage::obtainResponse(RIL_E_SUCCESS, response);
            resToRilj->getParcel()->writeInt32(1);
            resToRilj->getParcel()->writeInt32(nwType);
            mNwRatController->responseToRilj(resToRilj);
            logD(LOG_TAG, "responseGetPreferredNetworkType(), send response %d.", nwType);
        }
    }
}

void RpCdmaLteNwRatSwitchHandler::requestSetPreferredNetworkType(const int prefNwType) {
    char property_value[PROPERTY_VALUE_MAX] = { 0 };
    property_get("persist.radio.ct.ir.engmode", property_value, "0");
    int engineerMode = atoi(property_value);
    char tempstr[PROPERTY_VALUE_MAX] = { 0 };
    property_get("persist.radio.simswitch", tempstr, "1");
    int  capabilitySlotId  = atoi(tempstr) - 1;
    logD(LOG_TAG, "requestSetPreferredNetworkType(), engineerMode is %d, capabilitySlotId is %d. ",
            engineerMode, capabilitySlotId);
    if (engineerMode == ENGINEER_MODE_CDMA
            && (prefNwType == PREF_NET_TYPE_CDMA_ONLY
                    || prefNwType == PREF_NET_TYPE_EVDO_ONLY
                    || prefNwType == PREF_NET_TYPE_CDMA_EVDO_AUTO)
            && capabilitySlotId == mSlotId) {
        int msg_id = RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE;
        sp<RfxMessage> message = RfxMessage::obtainRequest(mSlotId,
                RADIO_TECH_GROUP_C2K, msg_id);
        message->getParcel()->writeInt32(1);
        message->getParcel()->writeInt32(prefNwType);
        logD(LOG_TAG, "requestSetPreferredNetworkType(), set nw type to %d.",
                prefNwType);
        mNwRatController->requestToRild(message);
    }
}

void RpCdmaLteNwRatSwitchHandler::responseSetPreferredNetworkType(const sp<RfxMessage>& response) {
    sp<RfxMessage> resToRilj = RfxMessage::obtainResponse(RIL_E_SUCCESS,
            response);
    mNwRatController->responseToRilj(resToRilj);
}

void RpCdmaLteNwRatSwitchHandler::doNwRatSwitch(const int prefNwType,
        const NwsMode nwsMode, const RatSwitchCaller& ratSwitchCaller, const sp<RfxAction>& action, const sp<RfxMessage>& message) {
    logD(LOG_TAG,
            "doNwRatSwitch(), prefNwType to %d. NwsMode is %d, ratSwitchCaller is %d ",
            prefNwType, nwsMode, ratSwitchCaller);
    mNwRatController->onRatSwitchStart(prefNwType, nwsMode);
    // 1, reset radio power off if will close finally
    // 2, config evdo mode if need
    // 3, switch network mode
    // 4, toggle phone object
    // 5, set radio power
    // 6, handle the radio state changed result

    mPrefNwType = prefNwType;
    mNwsMode = nwsMode;
    mRatSwitchCaller = ratSwitchCaller;
    mAction = action;
    mMessage = message;

    // 1, reset radio power off if will close finally
    resetRadioPowerOff();

}

void RpCdmaLteNwRatSwitchHandler::resetRadioPowerOff() {
    logD(LOG_TAG, "Step1: reset radio power off if will close finally");
    bool cdmaRadioOn = needCdmaRadioOn(mPrefNwType, mNwsMode);
    bool gsmRadioOn = needGsmRadioOn(mPrefNwType);
    int expectedRadioCapability = RIL_CAPABILITY_NONE;
    if (cdmaRadioOn && gsmRadioOn) {
        expectedRadioCapability = RIL_CAPABILITY_CDMA_ON_LTE;
    } else if (cdmaRadioOn && !gsmRadioOn) {
        expectedRadioCapability = RIL_CAPABILITY_CDMA_ONLY;
    } else if (!cdmaRadioOn && gsmRadioOn) {
        expectedRadioCapability = RIL_CAPABILITY_GSM_ONLY;
    } else {
        expectedRadioCapability = RIL_CAPABILITY_NONE;
    }
    int currentRadioCapability = mNwRatController->getStatusManager() -> getIntValue(RFX_STATUS_KEY_RADIO_CAPABILITY, RIL_CAPABILITY_NONE);

    logD(LOG_TAG, "resetRadioPowerOff(), expectedRadioCapability is %d, currentRadioCapability is %d. ",
            expectedRadioCapability, currentRadioCapability);
    int targetRadioCapability = RIL_CAPABILITY_NONE;
    bool needResetRadioOff = false;
    switch (currentRadioCapability) {
        case RIL_CAPABILITY_NONE:
            needResetRadioOff = false;
            break;
        case RIL_CAPABILITY_GSM_ONLY:
            if (expectedRadioCapability == RIL_CAPABILITY_GSM_ONLY
                    || expectedRadioCapability == RIL_CAPABILITY_CDMA_ON_LTE) {
                needResetRadioOff = false;
            } else {
                needResetRadioOff = true;
                targetRadioCapability = RIL_CAPABILITY_NONE;
            }
            break;
        case RIL_CAPABILITY_CDMA_ONLY:
            if (expectedRadioCapability == RIL_CAPABILITY_CDMA_ONLY
                    || expectedRadioCapability == RIL_CAPABILITY_CDMA_ON_LTE) {
                needResetRadioOff = false;
            } else {
                needResetRadioOff = true;
                targetRadioCapability = RIL_CAPABILITY_NONE;
            }
            break;
        case RIL_CAPABILITY_CDMA_ON_LTE:
            if (expectedRadioCapability == RIL_CAPABILITY_GSM_ONLY) {
                needResetRadioOff = true;
                targetRadioCapability = RIL_CAPABILITY_GSM_ONLY;
            } else if (expectedRadioCapability == RIL_CAPABILITY_CDMA_ONLY) {
                needResetRadioOff = true;
                targetRadioCapability = RIL_CAPABILITY_CDMA_ONLY;
            } else if (expectedRadioCapability == RIL_CAPABILITY_NONE) {
                needResetRadioOff = true;
                targetRadioCapability = RIL_CAPABILITY_NONE;
            } else {
                needResetRadioOff = false;
            }
            break;
        }
    logD(LOG_TAG, "resetRadioPowerOff(), targetRadioCapability is %d, needResetRadioOff is %s . ",
            targetRadioCapability, needResetRadioOff ? "true" : "false");
    if (needResetRadioOff) {
        mNwRatController->getStatusManager()->setIntValue(
                RFX_STATUS_KEY_SUGGESTED_RADIO_CAPABILITY,
                targetRadioCapability);
        mSugggestedResetRadioCapablity = targetRadioCapability;
        mNwRatController->getStatusManager()->registerStatusChanged(
                RFX_STATUS_KEY_RADIO_CAPABILITY,
                RfxStatusChangeCallback(this,
                        &RpCdmaLteNwRatSwitchHandler::onResetRadioPowerOffChanged));
    } else {
        logD(LOG_TAG, "no need reset radio power off");
        requestSetEvdoMode(mPrefNwType);
    }
}

void RpCdmaLteNwRatSwitchHandler::onResetRadioPowerOffChanged(
        RfxStatusKeyEnum key, RfxVariant old_value, RfxVariant new_value) {
    int newCapability = new_value.asInt();
    logD(LOG_TAG, "onResetRadioPowerOffChanged(), newCapability is %d", newCapability);
    if (newCapability == mSugggestedResetRadioCapablity) {
        mNwRatController->getStatusManager()->unRegisterStatusChanged(
                RFX_STATUS_KEY_RADIO_CAPABILITY,
                RfxStatusChangeCallback(this,
                        &RpCdmaLteNwRatSwitchHandler::onResetRadioPowerOffChanged));
        logD(LOG_TAG,
                "onResetRadioPowerOffChanged(), newCapability %d is the desired capablity %d . ",
                newCapability, mSugggestedResetRadioCapablity);
        requestSetEvdoMode(mPrefNwType);
    }
}
void RpCdmaLteNwRatSwitchHandler::requestSetEvdoMode(const int prefNwType ) {
    logD(LOG_TAG, "Step2: Config evdo mode if needed");
    EvdoMode evdoMode = EVDO_MODE_INVALID;
    switch(prefNwType){
    /* Disable EHRPD */
    case PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO:
    case PREF_NET_TYPE_EVDO_ONLY:
    case PREF_NET_TYPE_CDMA_ONLY:
    case PREF_NET_TYPE_CDMA_EVDO_AUTO:
        evdoMode = EVDO_MODE_NORMAL;
        break;

    /* Enable EHRPD */
    case PREF_NET_TYPE_LTE_CDMA_EVDO:
    case PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA:
        evdoMode = EVDO_MODE_EHRPD;
        break;

    default:
        break;
    }

    logD(LOG_TAG, "prepare set evdo mode is : %d ", evdoMode);
    if (evdoMode != EVDO_MODE_INVALID) {
        int msg_id = RIL_REQUEST_CONFIG_EVDO_MODE;
        sp<RfxMessage> message = RfxMessage::obtainRequest(mSlotId,
                RADIO_TECH_GROUP_C2K, msg_id);
        message->getParcel()->writeInt32(1);
        message->getParcel()->writeInt32(evdoMode);
        logD(LOG_TAG, "requestSetEvdoMode(), set evdo mode to %d.", evdoMode);
        mNwRatController->requestToRild(message);
    } else {
        // no need to config EHRPD
        logD(LOG_TAG, "no need to set evdo mode, prepare to set rat mode ");
        requestSetRatMode();
    }
}

void RpCdmaLteNwRatSwitchHandler::responseSetEvdoMode(const sp<RfxMessage>& response) {
    int error = response->getError() ;
    if (error != RIL_E_SUCCESS) {
        // error
        logD(LOG_TAG, "responseSetEvdoMode(), error is %d.", error);
        sendFailResponseToSMC();
    } else {
        // ok
        int source = response->getSource();
        if (source == RADIO_TECH_GROUP_C2K) {
            logD(LOG_TAG, "config evdo mode success, prepare to set rat mode ");
            requestSetRatMode();
        }
    }
}

void RpCdmaLteNwRatSwitchHandler::requestSetRatMode() {
    logD(LOG_TAG, "Step3: Set Rat mode");
    int radioMode = (int) setRadioTechModeFromPrefType();
    int preRatMode = (int) setRatModeFromPrefType(
            mNwRatController->getPreferredNetworkType());
    int newRatMode = (int) setRatModeFromPrefType(mPrefNwType);
    bool is3gCdmaSim = mNwRatController->getStatusManager()->getBoolValue(RFX_STATUS_KEY_CT3G_DUALMODE_CARD, false);
    logD(LOG_TAG,
            "requestSetRatMode(), radioMode is %d, preRatMode is %d, newRatMode is %d, preNwsMode is %d, newNwsMode is %d, is3gCdmaSim is %s",
            radioMode, preRatMode, newRatMode, (int)mNwRatController->getNwsMode(), (int)mNwsMode, is3gCdmaSim ? "true" : "false");
    if (radioMode != RADIO_TECH_MODE_UNKNOWN
            && newRatMode != SVLTE_RAT_MODE_INVALID) {
        int msg_id = RIL_REQUEST_SET_SVLTE_RAT_MODE;
        sp<RfxMessage> gsmMessage = RfxMessage::obtainRequest(mSlotId,
                RADIO_TECH_GROUP_GSM, msg_id);
        gsmMessage->getParcel()->writeInt32(6);
        gsmMessage->getParcel()->writeInt32(radioMode);
        gsmMessage->getParcel()->writeInt32(preRatMode);
        gsmMessage->getParcel()->writeInt32(newRatMode);
        gsmMessage->getParcel()->writeInt32(
                (int) mNwRatController->getNwsMode());
        gsmMessage->getParcel()->writeInt32((int) mNwsMode);
        gsmMessage->getParcel()->writeInt32(is3gCdmaSim ? 1 : 0);
        mNwRatController->requestToRild(gsmMessage);
        if (is3gCdmaSim) {
            sp<RfxMessage> cdmaMessage = RfxMessage::obtainRequest(
                    RADIO_TECH_GROUP_C2K, msg_id, gsmMessage);
            cdmaMessage->getParcel()->writeInt32(6);
            cdmaMessage->getParcel()->writeInt32(radioMode);
            cdmaMessage->getParcel()->writeInt32(preRatMode);
            cdmaMessage->getParcel()->writeInt32(newRatMode);
            cdmaMessage->getParcel()->writeInt32(
                    (int) mNwRatController->getNwsMode());
            cdmaMessage->getParcel()->writeInt32((int) mNwsMode);
            cdmaMessage->getParcel()->writeInt32(is3gCdmaSim ? 1 : 0);
            mNwRatController->requestToRild(cdmaMessage);
        }
    } else {
        logD(LOG_TAG, "requestSetRatMode , invalid prefNwType, update switch done");
        mNwRatController->updateState(mNwRatController->getNwsMode(),
                mNwRatController->getPreferredNetworkType());
    }
}

SVLTE_RAT_MODE RpCdmaLteNwRatSwitchHandler::setRatModeFromPrefType(const int prefNwType) {
    switch (prefNwType) {
    case PREF_NET_TYPE_LTE_GSM_WCDMA:
    case PREF_NET_TYPE_LTE_ONLY:
    case PREF_NET_TYPE_LTE_CDMA_EVDO:
    case PREF_NET_TYPE_LTE_WCDMA:
    case PREF_NET_TYPE_LTE_GSM:
    case PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA:
        return SVLTE_RAT_MODE_4G;

    case PREF_NET_TYPE_GSM_ONLY:
    case PREF_NET_TYPE_GSM_WCDMA:
    case PREF_NET_TYPE_GSM_WCDMA_AUTO:
    case PREF_NET_TYPE_WCDMA:
    case PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO:
    case PREF_NET_TYPE_CDMA_EVDO_AUTO:
    case PREF_NET_TYPE_EVDO_ONLY:
    case PREF_NET_TYPE_CDMA_ONLY:
        return SVLTE_RAT_MODE_3G;

    case PREF_NET_TYPE_LTE_TDD_ONLY:
        return SVLTE_RAT_MODE_4G_DATA_ONLY;

    default:
        return SVLTE_RAT_MODE_INVALID;
    }

}
RADIO_TECH_MODE RpCdmaLteNwRatSwitchHandler::setRadioTechModeFromPrefType() {
    switch (mPrefNwType) {
    case PREF_NET_TYPE_GSM_ONLY:
    case PREF_NET_TYPE_GSM_WCDMA:
    case PREF_NET_TYPE_GSM_WCDMA_AUTO:
    case PREF_NET_TYPE_WCDMA:
    case PREF_NET_TYPE_LTE_GSM_WCDMA:
    case PREF_NET_TYPE_LTE_ONLY:
    case PREF_NET_TYPE_LTE_TDD_ONLY:
    case PREF_NET_TYPE_LTE_WCDMA:
    case PREF_NET_TYPE_LTE_GSM:
        return RADIO_TECH_MODE_CSFB;

    case PREF_NET_TYPE_CDMA_ONLY:
    case PREF_NET_TYPE_CDMA_EVDO_AUTO:
    case PREF_NET_TYPE_EVDO_ONLY:
    case PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO:
    case PREF_NET_TYPE_LTE_CDMA_EVDO:
    case PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA:
        return RADIO_TECH_MODE_SVLTE;
    }
    return RADIO_TECH_MODE_UNKNOWN;
}
void RpCdmaLteNwRatSwitchHandler::responseSetRatMode(
        const sp<RfxMessage>& response) {
    mNwRatController->onEctModeChangeDone(mPrefNwType);
    int error = response->getError();
    if (error != RIL_E_SUCCESS) {
        // error
        logD(LOG_TAG, "responseSetSvlteMode(), error is %d.", error);
        sendFailResponseToSMC();
    } else {
        requestSetPreferredNetworkType(mPrefNwType);
        // 4, toggle phone object
        if (mNwRatController->mPendingRestrictedRatSwitchRecord.prefNwType == -1) {
            logD(LOG_TAG, "Step4: Toggle active phone");
            updatePhone(mPrefNwType);
        } else {
            logD(LOG_TAG, "Restricted mode, do not update phone!");
        }
        mNwRatController->onRatSwitchDone(mPrefNwType);
        // 5, set radio power
        logD(LOG_TAG, "Step5: Power on Radio according to the prefer nw type");
        mSugggestedOnRadioCapablity = radioPowerOn();
    }
}

int RpCdmaLteNwRatSwitchHandler::radioPowerOn() {
    int capability = RIL_CAPABILITY_NONE;
    bool cdmaRadioOn = needCdmaRadioOn(mPrefNwType, mNwsMode);
    bool gsmRadioOn = needGsmRadioOn(mPrefNwType);
    logD(LOG_TAG, "radioPowerOn(), needCdmaRadioOn is %s, needGsmRadioOn is %s",
            cdmaRadioOn ? "true" : "false", gsmRadioOn ? "true" : "false");
    if (!cdmaRadioOn && gsmRadioOn) {
        capability = RIL_CAPABILITY_GSM_ONLY;
    } else if (!gsmRadioOn && cdmaRadioOn) {
        capability = RIL_CAPABILITY_CDMA_ONLY;
    } else if (gsmRadioOn && cdmaRadioOn) {
        capability = RIL_CAPABILITY_CDMA_ON_LTE;
    } else {
        capability = RIL_CAPABILITY_NONE;
    }
    int currentCapability = mNwRatController->getStatusManager()->getIntValue(
            RFX_STATUS_KEY_RADIO_CAPABILITY, RIL_CAPABILITY_NONE);
    mNwRatController->getStatusManager()->setIntValue(
            RFX_STATUS_KEY_SUGGESTED_RADIO_CAPABILITY, capability);
    logD(LOG_TAG, "radioPowerOn(), desired radio capability is %d, currentCapability is %d.",
            capability, currentCapability);
    if (canRadioOn() && capability != currentCapability) {
        mNwRatController->getStatusManager()->registerStatusChanged(
                RFX_STATUS_KEY_RADIO_CAPABILITY,
                RfxStatusChangeCallback(this,
                        &RpCdmaLteNwRatSwitchHandler::onRadioPowerOn));
    } else {
        logD(LOG_TAG, "radioPowerOn(), no need monitor radio power on." );
        onNwRatSwitchDone();
    }

    return capability;
}

void RpCdmaLteNwRatSwitchHandler::onRadioPowerOn(RfxStatusKeyEnum key, RfxVariant old_value, RfxVariant new_value) {
    // 6, handle the radio state changed result
    RIL_RADIO_CAPABILITY newCapability = (RIL_RADIO_CAPABILITY)new_value.asInt();
    logD(LOG_TAG, "onRadioPowerOn(), newCapability is %d, mSugggestedOnRadioCapablity is %d",
            newCapability, mSugggestedOnRadioCapablity);
    if (newCapability == mSugggestedOnRadioCapablity) {

        mNwRatController->getStatusManager()->unRegisterStatusChanged(
                RFX_STATUS_KEY_RADIO_CAPABILITY,
                RfxStatusChangeCallback(this,
                        &RpCdmaLteNwRatSwitchHandler::onRadioPowerOn));
        onNwRatSwitchDone();
    }
}

void RpCdmaLteNwRatSwitchHandler::onNwRatSwitchDone(){
    mNwRatController->updateState(mNwsMode, mPrefNwType);
    mNwRatController->getStatusManager()->setIntValue(RFX_STATUS_KEY_RAT_SWITCH_DONE, mRatSwitchCaller);
    logD(LOG_TAG, "Step 6: rat switch done, mPrefNwType is %d, mNwsMode is %d, ",
            mPrefNwType, mNwsMode);

    if (mAction != NULL) {
        mAction->act();
        logD(LOG_TAG, "rat switch done, do the action. ");
    }

    if(mMessage != NULL){
        responseSetPreferredNetworkType(mMessage);
        logD(LOG_TAG, "rat switch done, response the set preferred nw type request. ");
    }

    mNwRatController->doPendingRatSwitchRecord();
}

bool RpCdmaLteNwRatSwitchHandler::needCdmaRadioOn(const int prefNwType, const NwsMode nwsMode) {
    logD(LOG_TAG, "needCdmaRadioOn, prefNwType=%d, mNwsMode = %d", prefNwType, mNwsMode);

    if (nwsMode == NWS_MODE_CDMALTE) {
        switch (prefNwType) {
            case PREF_NET_TYPE_CDMA_EVDO_AUTO:
            case PREF_NET_TYPE_CDMA_ONLY:
            case PREF_NET_TYPE_EVDO_ONLY:
            case PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO:
            case PREF_NET_TYPE_LTE_CDMA_EVDO:
            case PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA:
                return true;
            case PREF_NET_TYPE_GSM_WCDMA:
            case PREF_NET_TYPE_GSM_ONLY:
            case PREF_NET_TYPE_WCDMA:
            case PREF_NET_TYPE_GSM_WCDMA_AUTO:
            case PREF_NET_TYPE_LTE_GSM_WCDMA:
            case PREF_NET_TYPE_LTE_ONLY:
            case PREF_NET_TYPE_LTE_TDD_ONLY:
            case PREF_NET_TYPE_LTE_WCDMA:
            case PREF_NET_TYPE_LTE_GSM:
                return false;
        }
    } else if (nwsMode == NWS_MODE_CSFB) {
        char property_value[PROPERTY_VALUE_MAX] = { 0 };
        property_get("persist.radio.ct.ir.engmode", property_value, "0");
        int engineerMode = atoi(property_value);
        switch (prefNwType) {
            case PREF_NET_TYPE_CDMA_EVDO_AUTO:
            case PREF_NET_TYPE_CDMA_ONLY:
            case PREF_NET_TYPE_EVDO_ONLY:
            case PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO:
            case PREF_NET_TYPE_LTE_CDMA_EVDO:
            case PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA:
                return true;
            case PREF_NET_TYPE_LTE_WCDMA:
            case PREF_NET_TYPE_LTE_GSM_WCDMA:
            case PREF_NET_TYPE_LTE_GSM:
            case PREF_NET_TYPE_LTE_ONLY:
            if (engineerMode == ENGINEER_MODE_CSFB) {
                /* engineer set in CSFB mode c2k radio should off*/
                return false;
            } else {
                /* Roaming and 4G mode c2k radio should on*/
                return true;
            }
            case PREF_NET_TYPE_GSM_WCDMA:
            case PREF_NET_TYPE_GSM_ONLY:
            case PREF_NET_TYPE_WCDMA:
            case PREF_NET_TYPE_GSM_WCDMA_AUTO:
            case PREF_NET_TYPE_LTE_TDD_ONLY:
                return false;
        }
    }
    return false;
}
void RpCdmaLteNwRatSwitchHandler::sendFailResponseToSMC() {
     logD(LOG_TAG, "sendFailResponseToSMC");
     mNwRatController->updateState(mNwRatController->getNwsMode(),
                mNwRatController->getPreferredNetworkType());

    if (mAction != NULL) {
        mAction->act();
        logD(LOG_TAG, "sendFailResponseToSMC, do the action. ");
    }

    if (mMessage != NULL) {
        responseSetPreferredNetworkType(mMessage);
        logD(LOG_TAG, "sendFailResponseToSMC, response the set preferred nw type request. ");
    }

    mNwRatController->doPendingRatSwitchRecord();
}

bool RpCdmaLteNwRatSwitchHandler::canRadioOn(){
    char property_value[PROPERTY_VALUE_MAX] = { 0 };
    property_get("ril.cdma.emdstatus.send", property_value, "0");
    int sendEmdStatus = atoi(property_value);
    /* RADIO_STATE_ON and has send emdstatus*/
    bool canRadioOn = mNwRatController->getStatusManager()->getBoolValue(
            RFX_STATUS_KEY_REQUEST_RADIO_POWER, false) && (sendEmdStatus == 1);
    logD(LOG_TAG, "canRadioOn(), canRadioOn = %s. ", canRadioOn ? "true" : "false");
    return canRadioOn;
}
