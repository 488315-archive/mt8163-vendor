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
#include "RpNwRatController.h"
#include "RfxLog.h"
#include "RfxStatusDefs.h"
#include "RpNwController.h"
#include "RpGsmNwRatSwitchHandler.h"
#include "RpCdmaNwRatSwitchHandler.h"
#include "RpCdmaLteNwRatSwitchHandler.h"
#include "modecontroller/RpCdmaLteDefs.h"
#include "modecontroller/RpCdmaLteModeController.h"
#include "ril.h"

#define LOG_TAG "RpNwRatController"

/*****************************************************************************
 * Class RpNwRatController
 *****************************************************************************/

RFX_IMPLEMENT_CLASS("RpNwRatController", RpNwRatController, RfxController);

bool RpNwRatController::sIsInSwitching = false;

RpNwRatController::RpNwRatController() :
    mCurAppFamilyType(APP_FAM_UNKNOWN),
    mCurPreferedNetWorkType(-1),
    mCurNwsMode(NWS_MODE_CDMALTE),
    mNewAppFamilyType(APP_FAM_UNKNOWN),
    mNewPreferedNetWorkType(-1),
    mNewNwsMode(NWS_MODE_CDMALTE),
    mNwRatSwitchHandler(NULL){
}

RpNwRatController::~RpNwRatController() {
    RFX_OBJ_CLOSE(mNwRatSwitchHandler);
}

void RpNwRatController::setPreferredNetworkType(const int prefNwType,
        const sp<RfxAction>& action) {
    if (getEnginenerMode() == ENGINEER_MODE_AUTO) {
        if (prefNwType == -1) {
            logD(LOG_TAG, "setPreferredNetworkType() leaving restricted mode");
            mPendingRestrictedRatSwitchRecord.prefNwType = -1;
            doPendingRatSwitchRecord();
        } else {
            logD(LOG_TAG, "setPreferredNetworkType() entering restricted mode: %d", prefNwType);
            mPendingRestrictedRatSwitchRecord.prefNwType = prefNwType;
            mPendingRestrictedRatSwitchRecord.appFamType = APP_FAM_3GPP2;
            mPendingRestrictedRatSwitchRecord.ratSwitchCaller = RAT_SWITCH_RESTRICT;
            mPendingRestrictedRatSwitchRecord.nwsMode = mCurNwsMode;
            if (mPendingInitRatSwitchRecord.prefNwType == -1
                    && mPendingNormalRatSwitchRecord.prefNwType == -1) {
                // save current state to pending queue
                queueRatSwitchRecord(mCurAppFamilyType, mCurPreferedNetWorkType, mCurNwsMode,
                        RAT_SWITCH_INIT, action, NULL);
            }
            switchNwRat(APP_FAM_3GPP2, PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA, mCurNwsMode,
                    RAT_SWITCH_RESTRICT, action, NULL);
        }
    }
}

void RpNwRatController::setPreferredNetworkType(const AppFamilyType appFamType,
        const int prefNwType, int type, const sp<RfxAction>& action) {
    logD(LOG_TAG, "setPreferredNetworkType() from mode controller, appFamType is %d, prefNwType is %d, type is %d",
            appFamType, prefNwType, type);
    if (getChipTestMode() == 1) {
        logD(LOG_TAG, "ChipTest! setPreferredNetworkType not executed! action->act()");
        if (action != NULL) {
            action->act();
        }
        return;
    } else {
        if (type == CARD_STATE_NO_CHANGED) {
            if (action != NULL) {
                action->act();
            }
            return;
        }
        if (type == CARD_STATE_NOT_HOT_PLUG) {
            clearState();
        }
    }
    mNewAppFamilyType = appFamType;

    if (appFamType == APP_FAM_3GPP) {
        /* For gsm card, create the ratSwitchHandler and set the related state*/
        switchNwRat(appFamType, prefNwType, NWS_MODE_CSFB, RAT_SWITCH_INIT,
                    action, NULL);
    } else {
        if (getEnginenerMode() == ENGINEER_MODE_AUTO) {
            if (mPendingNormalRatSwitchRecord.prefNwType != -1) {
                switchNwRat(appFamType, prefNwType,
                        mPendingNormalRatSwitchRecord.nwsMode, RAT_SWITCH_INIT,
                        action, NULL);
            } else {
                switchNwRat(appFamType, prefNwType, mNewNwsMode,
                        RAT_SWITCH_INIT, action, NULL);
            }
        } else {
            doNwSwitchForEngMode(action);
        }
    }
}

void RpNwRatController::setPreferredNetworkType(const int prefNwType, const sp<RfxMessage>& message) {
    logD(LOG_TAG, "setPreferredNetworkType() from normal, prefNwType is %d,mNewPreferedNetWorkType is %d. ",
            prefNwType, mNewPreferedNetWorkType);
    if (getChipTestMode() == 1) {
        logD(LOG_TAG, "ChipTest! setPreferredNetworkType not executed!");
        return;
    }
    AppFamilyType appFamilyType = mNewAppFamilyType;
    if (prefNwType == -1) {
        int activeCdmaLteSlot = getNonSlotScopeStatusManager()->getIntValue(
                RFX_STATUS_KEY_ACTIVE_CDMALTE_MODE_SLOT, 0);
        logD(LOG_TAG, "setPreferredNetworkType(), activeCdmaLteSlot %d.",
                activeCdmaLteSlot);
        if (getSlotId() == activeCdmaLteSlot) {
            appFamilyType = APP_FAM_3GPP2;
        } else {
            appFamilyType = APP_FAM_3GPP;
        }
    }

    if (appFamilyType == APP_FAM_3GPP) {
        /* For gsm card, create the ratSwitchHandler and set the related state*/
        switchNwRat(appFamilyType, prefNwType, NWS_MODE_CSFB, RAT_SWITCH_NORMAL,
                    NULL, message);
    } else if (appFamilyType == APP_FAM_3GPP2) {
        RpCdmaLteModeController *modeController = (RpCdmaLteModeController *) findController(-1,
                    RFX_OBJ_CLASS_INFO(RpCdmaLteModeController));
        int capability = modeController->getDefaultNetworkType(getSlotId());
        int targetPrefNwType = filterPrefNwType(appFamilyType, prefNwType, capability);
        logD(LOG_TAG, "the prefer network type is filtered as %d, capability is %d .",
                      targetPrefNwType, capability);

        int engineerMode = getEnginenerMode();
        if ((targetPrefNwType != mNewPreferedNetWorkType && engineerMode == ENGINEER_MODE_AUTO)
                || (engineerMode == ENGINEER_MODE_CDMA &&
                        (targetPrefNwType == PREF_NET_TYPE_CDMA_ONLY
                        || targetPrefNwType == PREF_NET_TYPE_EVDO_ONLY
                        || targetPrefNwType == PREF_NET_TYPE_CDMA_EVDO_AUTO))) {
            NwsMode nwsMode = mNewNwsMode;

            if (mPendingNormalRatSwitchRecord.prefNwType != -1) {
                nwsMode = mPendingNormalRatSwitchRecord.nwsMode;
            }

            switchNwRat(appFamilyType, targetPrefNwType, nwsMode, RAT_SWITCH_NORMAL,
                    NULL, message);
        }
    }
}

int RpNwRatController::filterPrefNwType(const AppFamilyType appFamilyType, int prefNwType, int capability){
    int targetPrefNwType = prefNwType;
    switch (prefNwType) {
    case PREF_NET_TYPE_LTE_CDMA_EVDO:
    case PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA:
        if (appFamilyType == APP_FAM_3GPP2) {
            if (capability == PREF_NET_TYPE_CDMA_EVDO_AUTO) {
                targetPrefNwType = PREF_NET_TYPE_CDMA_EVDO_AUTO;
            }
            if (capability == PREF_NET_TYPE_LTE_CDMA_EVDO) {
                targetPrefNwType = PREF_NET_TYPE_LTE_CDMA_EVDO;
            }
        } else if (appFamilyType == APP_FAM_3GPP) {
            if (capability == PREF_NET_TYPE_LTE_GSM_WCDMA) {
                targetPrefNwType = PREF_NET_TYPE_LTE_GSM_WCDMA;
            }
            if (capability == PREF_NET_TYPE_GSM_WCDMA) {
                targetPrefNwType = PREF_NET_TYPE_GSM_WCDMA;
            }
            if (capability == PREF_NET_TYPE_GSM_ONLY) {
                targetPrefNwType = PREF_NET_TYPE_GSM_ONLY;
            }
        }
        break;
    case PREF_NET_TYPE_CDMA_EVDO_AUTO:
    case PREF_NET_TYPE_CDMA_ONLY:
    case PREF_NET_TYPE_EVDO_ONLY:
    case PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO:
    case PREF_NET_TYPE_GSM_WCDMA:
    case PREF_NET_TYPE_GSM_ONLY:
    case PREF_NET_TYPE_WCDMA:
    case PREF_NET_TYPE_GSM_WCDMA_AUTO:
    case PREF_NET_TYPE_LTE_GSM_WCDMA:
    case PREF_NET_TYPE_LTE_ONLY:
    case PREF_NET_TYPE_LTE_TDD_ONLY:
    case PREF_NET_TYPE_LTE_WCDMA:
    case PREF_NET_TYPE_LTE_GSM:
        break;
    }
    return targetPrefNwType;
}

void RpNwRatController::creatRatSwitchHandlerIfNeeded(const AppFamilyType appFamType){
    logD(LOG_TAG,"creatRatSwitchHandlerIfNeeded(), appFamType is %d. ",appFamType);
    if (appFamType == APP_FAM_3GPP) {
        // boot up work flow for gsm card.
        if (mNwRatSwitchHandler == NULL) {
            RFX_OBJ_CREATE_EX(mNwRatSwitchHandler, RpGsmNwRatSwitchHandler, this, (this));
            // for sim card hot plug in/out
        } else if (appFamType != mCurAppFamilyType) {
            clearInvalidPendingRecords();
            RFX_OBJ_CLOSE(mNwRatSwitchHandler);
            RFX_OBJ_CREATE_EX(mNwRatSwitchHandler, RpGsmNwRatSwitchHandler, this, (this));
        }
    } else if (appFamType == APP_FAM_3GPP2) {
        // boot up work flow for cdma card.
        if (mNwRatSwitchHandler == NULL) {
            RFX_OBJ_CREATE_EX(mNwRatSwitchHandler, RpCdmaLteNwRatSwitchHandler, this, (this));
            // for sim card hot plug in/out
        } else if (appFamType != mCurAppFamilyType) {
            clearInvalidPendingRecords();
            RFX_OBJ_CLOSE(mNwRatSwitchHandler);
            RFX_OBJ_CREATE_EX(mNwRatSwitchHandler, RpCdmaLteNwRatSwitchHandler, this, (this));
        }
    }
    logD(LOG_TAG,"creatRatSwitchHandlerIfNeeded(), mNwRatSwitchHandler is %p. ",mNwRatSwitchHandler);
}

AppFamilyType RpNwRatController::getAppFamilyType() {
    logD(LOG_TAG,"getAppFamilyType(), mCurAppFamilyType is %d. ",mCurAppFamilyType);
    return mCurAppFamilyType;
}

int RpNwRatController::getPreferredNetworkType() {
    logD(LOG_TAG,"getPreferredNetworkType(), mCurPreferedNetWorkType is %d. ",mCurPreferedNetWorkType);
    return mCurPreferedNetWorkType;
}

void RpNwRatController::setNwsMode(const NwsMode nwsMode, const sp<RfxAction>& action) {
    logD(LOG_TAG, "setNwsMode(), nwsMode is %d.", nwsMode);
    if (getChipTestMode() == 1) {
        logD(LOG_TAG, "ChipTest! setNwsMode not executed! action->act()");
        if (action != NULL) {
            action->act();
        }
        return;
    }
    if (getEnginenerMode() == ENGINEER_MODE_AUTO) {
        int prefNwType = mNewPreferedNetWorkType;
        AppFamilyType appFamilyType = mNewAppFamilyType;

        if (mPendingNormalRatSwitchRecord.prefNwType != -1) {
            prefNwType = mPendingNormalRatSwitchRecord.prefNwType;
        }

        if (appFamilyType == APP_FAM_3GPP2) {
            switchNwRat(appFamilyType, prefNwType, nwsMode, RAT_SWITCH_NWS,
                    action, NULL);
        }
    }
}

NwsMode RpNwRatController::getNwsMode() {
    logD(LOG_TAG,"getNwsMode(), mCurNwsMode is %d. ",mCurNwsMode);
    return mCurNwsMode;
}

void RpNwRatController::onInit() {
    // Required: invoke super class implementation
    RfxController::onInit();

    logD(LOG_TAG,"onInit");

    // define and register request & urc id list
    const int request_id_list[] = {
        RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE,
        RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE,
        RIL_REQUEST_CONFIG_EVDO_MODE,
        RIL_REQUEST_VOICE_RADIO_TECH,
        RIL_REQUEST_SET_SVLTE_RAT_MODE};
    const int urc_id_list[] = { };
    registerToHandleReuquest(request_id_list, sizeof(request_id_list)/sizeof(int));
    registerToHandleUrc(urc_id_list, 0);

    getStatusManager()->setIntValue(RFX_STATUS_KEY_NWS_MODE, mCurNwsMode);
    getStatusManager()->setIntValue(RFX_STATUS_KEY_PREFERRED_NW_TYPE, mCurPreferedNetWorkType);
}

void RpNwRatController::onDeinit() {
    RfxController::onDeinit();
}

void RpNwRatController::doNwSwitchForEngMode(const sp<RfxAction>& action) {
    logD(LOG_TAG, "Radio Avaliable, have get the app family type ");
    if (mNewAppFamilyType == APP_FAM_3GPP2) {
        switch (getEnginenerMode()) {
        case ENGINEER_MODE_CDMA:
            logD(LOG_TAG, "Radio Avaliable, CDMA only mode. ");
            switchNwRat(APP_FAM_3GPP2, PREF_NET_TYPE_CDMA_EVDO_AUTO,
                    mCurNwsMode, RAT_SWITCH_NORMAL, action, NULL);
            break;
        case ENGINEER_MODE_CSFB:
            logD(LOG_TAG, "Radio Avaliable, CSFB only mode. ");
            switchNwRat(APP_FAM_3GPP2, PREF_NET_TYPE_LTE_GSM_WCDMA,
                    NWS_MODE_CSFB, RAT_SWITCH_NORMAL, action, NULL);
            break;
        case ENGINEER_MODE_LTE:
            logD(LOG_TAG, "Radio Avaliable, LTE only mode. ");
            switchNwRat(APP_FAM_3GPP2, PREF_NET_TYPE_LTE_ONLY, mCurNwsMode,
                    RAT_SWITCH_NORMAL, action, NULL);
            break;
        default:
            logD(LOG_TAG, "Radio Avaliable, auto mode, do nothing. ");
            break;
        }
    }
}
bool RpNwRatController::onHandleRequest(const sp<RfxMessage>& message) {
    int msg_id = message->getId();
    logD(LOG_TAG,"handle req %s.", requestToString(msg_id));

    if (mNwRatSwitchHandler != NULL) {
        switch (msg_id) {
        case RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE:
            int32_t stgCount;
            int32_t nwType;
            message->getParcel()->readInt32(&stgCount);
            message->getParcel()->readInt32(&nwType);
            setPreferredNetworkType(nwType, message);
            return true;
        case RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE:
            mNwRatSwitchHandler->requestGetPreferredNetworkType(message);
            return true;
        case RIL_REQUEST_VOICE_RADIO_TECH:
            logD(LOG_TAG, "request voice radio tech, mCurNwsMode = %d.", mCurNwsMode);
            if (mCurNwsMode == NWS_MODE_CSFB) {
                mNwRatSwitchHandler->requestGetVoiceRadioTech(message);
            } else {
                sp<RfxMessage> resToRilj = RfxMessage::obtainResponse(
                        RIL_E_SUCCESS, message);
                resToRilj->getParcel()->writeInt32(1);
                resToRilj->getParcel()->writeInt32(RADIO_TECH_1xRTT);
                logD(LOG_TAG, "request voice radio tech, send response.voiceRadioTech = 6.");
                responseToRilj(resToRilj);
            }
            return true;
        }
    }
    return false;
}

bool RpNwRatController::onHandleResponse(const sp<RfxMessage>& message) {
    int msg_id = message->getId();
    logD(LOG_TAG,"handle %s response.", requestToString(msg_id));

    if (mNwRatSwitchHandler != NULL) {
        switch (msg_id) {
        case RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE:
            if (mNewAppFamilyType == APP_FAM_3GPP) {
                mNwRatSwitchHandler->responseSetPreferredNetworkType(message);
            }
            return true;
        case RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE:
            mNwRatSwitchHandler->responseGetPreferredNetworkType(message);
            return true;
        case RIL_REQUEST_CONFIG_EVDO_MODE:
            mNwRatSwitchHandler->responseSetEvdoMode(message);
            return true;
        case RIL_REQUEST_SET_SVLTE_RAT_MODE: {
            bool is3gCdmaSim = getStatusManager()->getBoolValue(
                    RFX_STATUS_KEY_CT3G_DUALMODE_CARD, false);
            logD(LOG_TAG, "3gCdmaSim handle RIL_REQUEST_SET_SVLTE_RAT_MODE");
            if (is3gCdmaSim) {
                sp<RfxMessage> msg = sp < RfxMessage > (NULL);
                ResponseStatus responseStatus = preprocessResponse(message, msg,
                        RfxWaitResponseTimedOutCallback(this,
                                &RpBaseNwRatSwitchHandler::responseSetRatMode),
                        s2ns(10));
                if (responseStatus == RESPONSE_STATUS_HAVE_MATCHED) {
                    mNwRatSwitchHandler->responseSetRatMode(message);
                }
            } else {
                mNwRatSwitchHandler->responseSetRatMode(message);
            }
            return true;
        }
        case RIL_REQUEST_VOICE_RADIO_TECH:
            mNwRatSwitchHandler->responseGetVoiceRadioTech(message);
            return true;
        }
    }
    return false;
}

bool RpNwRatController::onHandleUrc(const sp<RfxMessage>& message) {
    int msg_id = message->getId();
    logD(LOG_TAG,"handle urc %s.", urcToString(msg_id));

    switch (msg_id) {
    default:
        break;
    }
    return true;
}

char* RpNwRatController::requestToString(int reqId) {
    switch (reqId) {
    case RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE:
        return "RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE";
    case RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE:
        return "RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE";
    case RIL_REQUEST_CONFIG_EVDO_MODE:
        return "RIL_REQUEST_CONFIG_EVDO_MODE";
    case RIL_REQUEST_SET_SVLTE_RAT_MODE:
        return "RIL_REQUEST_SET_SVLTE_RAT_MODE";
    case RIL_REQUEST_VOICE_RADIO_TECH:
        return "RIL_REQUEST_VOICE_RADIO_TECH";
    default:
        logD(LOG_TAG,"<UNKNOWN_REQUEST>");
        break;
    }
    return "";
}

char* RpNwRatController::urcToString(int reqId) {
    switch (reqId) {
    default:
        logD(LOG_TAG,"<UNKNOWN_URC>");
        break;
    }
    return "";
}

void RpNwRatController::registerRatSwitchCallback(IRpNwRatSwitchCallback* callback) {
    mRpNwRatSwitchListener = callback;
}

void RpNwRatController::unRegisterRatSwitchCallback(IRpNwRatSwitchCallback* callback) {
    mRpNwRatSwitchListener = NULL;
}

void RpNwRatController::onRatSwitchStart(const int prefNwType, const NwsMode newNwsMode){
    if (mRpNwRatSwitchListener != NULL) {
        mRpNwRatSwitchListener->onRatSwitchStart(mCurPreferedNetWorkType,
                prefNwType, mCurNwsMode, newNwsMode);
    }
}
void RpNwRatController::onRatSwitchDone(const int prefNwType){
    if (mRpNwRatSwitchListener != NULL) {
        mRpNwRatSwitchListener->onRatSwitchDone(mCurPreferedNetWorkType,
                prefNwType);
    }
}
void RpNwRatController::onEctModeChangeDone(const int prefNwType){
    if (mRpNwRatSwitchListener != NULL) {
        mRpNwRatSwitchListener->onEctModeChangeDone(mCurPreferedNetWorkType,
                prefNwType);
    }
}
void RpNwRatController::switchNwRat(const AppFamilyType appFamType,
        int prefNwType, NwsMode nwsMode,
        const RatSwitchCaller ratSwitchCaller, const sp<RfxAction>& action, const sp<RfxMessage>& message) {
    logD(LOG_TAG,"switchNwRat(), appFamType is %d, prefNwType is %d, nwsMode is %d . ratSwitchCaller is %d, sIsInSwitching is %s. ",
            appFamType, prefNwType, nwsMode, ratSwitchCaller, sIsInSwitching ? "true" : "false");

    if (sIsInSwitching) {
        queueRatSwitchRecord(appFamType, prefNwType, nwsMode, ratSwitchCaller, action, message);
    } else {
        if (ratSwitchCaller != RAT_SWITCH_RESTRICT
                && mPendingRestrictedRatSwitchRecord.prefNwType != -1) {
            logD(LOG_TAG, "switchNwRat(), in restricted mode!");
            queueRatSwitchRecord(appFamType, prefNwType, nwsMode, ratSwitchCaller, action, message);
            return;
        }
        sIsInSwitching = true;

        /* create the rat switch handler if need. */
        creatRatSwitchHandlerIfNeeded(appFamType);
        mCurAppFamilyType = appFamType;

        if (appFamType == APP_FAM_3GPP2) {
            /* Calculate the target preferred network type. */
            int targetPrefNwType = calculateTargetPreferredNwType(appFamType, prefNwType, nwsMode);
            logD(LOG_TAG, "switchNwRat(), targetPrefNwType is %d.", targetPrefNwType);

            /* Handle the network preferred network switch. */
            if (isValidPreferredNwType(appFamType, targetPrefNwType, nwsMode)) {
                mNewPreferedNetWorkType = targetPrefNwType;
                mNewNwsMode = nwsMode;
                mNwRatSwitchHandler->doNwRatSwitch(targetPrefNwType, nwsMode,
                        ratSwitchCaller, action, message);
            } else {
                logD(LOG_TAG, "switchNwRat(), invalid prefNwType is %d.", prefNwType);
                if (ratSwitchCaller == RAT_SWITCH_NORMAL && message != NULL) {
                    mNwRatSwitchHandler->responseSetPreferredNetworkType(message);
                    updateState(mCurNwsMode, mCurPreferedNetWorkType);
                }
            }
        } else if (appFamType == APP_FAM_3GPP) {
            mNewNwsMode = NWS_MODE_CSFB;
            logD(LOG_TAG, "switchNwRat(), set radio capability as 1.");
            getStatusManager()->setIntValue(RFX_STATUS_KEY_SUGGESTED_RADIO_CAPABILITY, RIL_CAPABILITY_GSM_ONLY);

            if (ratSwitchCaller == RAT_SWITCH_INIT) {
                mNewPreferedNetWorkType = calculateTargetPreferredNwType(appFamType, prefNwType, mNewNwsMode);
                mNwRatSwitchHandler->requestSetPreferredNetworkType(mNewPreferedNetWorkType, message);
                mNwRatSwitchHandler->updatePhone(mNewPreferedNetWorkType);
                if (action != NULL) {
                    action->act();
                }
            } else {
                mNewPreferedNetWorkType = prefNwType;
                mNwRatSwitchHandler->requestSetPreferredNetworkType(mNewPreferedNetWorkType, message);
            }
            getStatusManager()->setIntValue(RFX_STATUS_KEY_NWS_MODE, mNewNwsMode);
            getStatusManager()->setIntValue(RFX_STATUS_KEY_PREFERRED_NW_TYPE, mNewPreferedNetWorkType);
            mCurPreferedNetWorkType = mNewPreferedNetWorkType;
            mCurNwsMode = mNewNwsMode;
            sIsInSwitching = false;
        }
    }
}

void RpNwRatController::queueRatSwitchRecord(const AppFamilyType appFamType,
        int prefNwType, NwsMode nwsMode, const RatSwitchCaller ratSwitchCaller,
        const sp<RfxAction>& action, const sp<RfxMessage>& message) {
    /* Pending if in switching. */
    logD(LOG_TAG, "queueRatSwitchRecord(), ratSwitchCaller:%d prefNwType:%d",
            ratSwitchCaller, prefNwType);
    if (ratSwitchCaller == RAT_SWITCH_RESTRICT) {
        mPendingRestrictedRatSwitchRecord.appFamType = appFamType;
        mPendingRestrictedRatSwitchRecord.prefNwType = prefNwType;
        mPendingRestrictedRatSwitchRecord.nwsMode = nwsMode;
        mPendingRestrictedRatSwitchRecord.ratSwitchCaller = ratSwitchCaller;
        mPendingRestrictedRatSwitchRecord.action = action;
        mPendingRestrictedRatSwitchRecord.message = message;
    } else if (ratSwitchCaller == RAT_SWITCH_INIT) {
        mPendingInitRatSwitchRecord.appFamType = appFamType;
        mPendingInitRatSwitchRecord.prefNwType = prefNwType;
        mPendingInitRatSwitchRecord.nwsMode = nwsMode;
        mPendingInitRatSwitchRecord.ratSwitchCaller = ratSwitchCaller;
        mPendingInitRatSwitchRecord.action = action;
        mPendingInitRatSwitchRecord.message = message;
    } else {
        if(mPendingNormalRatSwitchRecord.prefNwType != -1 && mPendingNormalRatSwitchRecord.message != NULL){
            logD(LOG_TAG, "switchNwRat(), request set prefer network type is pending, will be ignored, send response.");
            mNwRatSwitchHandler->responseSetPreferredNetworkType(mPendingNormalRatSwitchRecord.message);
        }
        mPendingNormalRatSwitchRecord.appFamType = appFamType;
        mPendingNormalRatSwitchRecord.prefNwType = prefNwType;
        mPendingNormalRatSwitchRecord.nwsMode = nwsMode;
        mPendingNormalRatSwitchRecord.ratSwitchCaller = ratSwitchCaller;
        mPendingNormalRatSwitchRecord.action = action;
        mPendingNormalRatSwitchRecord.message = message;
    }
}

bool RpNwRatController::isValidPreferredNwType(const AppFamilyType appFamType, int prefNwType, NwsMode nwsMode){
    bool isValidPreferredNwType = true;
    if (appFamType == APP_FAM_3GPP2) {
        if (nwsMode == NWS_MODE_CDMALTE) {
            switch (prefNwType) {
                case PREF_NET_TYPE_CDMA_EVDO_AUTO:
                case PREF_NET_TYPE_CDMA_ONLY:
                case PREF_NET_TYPE_EVDO_ONLY:
                case PREF_NET_TYPE_LTE_CDMA_EVDO:
                case PREF_NET_TYPE_LTE_ONLY:
                case PREF_NET_TYPE_LTE_TDD_ONLY:
                    isValidPreferredNwType = true;
                    break;

                case PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA:
                    if (isRestrictedModeSupport()) {
                        isValidPreferredNwType = true;
                        break;
                    }
                case PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO:
                case PREF_NET_TYPE_GSM_WCDMA:
                case PREF_NET_TYPE_GSM_ONLY:
                case PREF_NET_TYPE_WCDMA:
                case PREF_NET_TYPE_GSM_WCDMA_AUTO:
                case PREF_NET_TYPE_LTE_GSM_WCDMA:
                case PREF_NET_TYPE_LTE_WCDMA:
                case PREF_NET_TYPE_LTE_GSM:
                    isValidPreferredNwType = false;
                    break;
            }
        }

        if (nwsMode == NWS_MODE_CSFB) {
            switch (prefNwType) {
            case PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA:
                if (isRestrictedModeSupport()) {
                    isValidPreferredNwType = true;
                    break;
                }
            case PREF_NET_TYPE_CDMA_EVDO_AUTO:
            case PREF_NET_TYPE_CDMA_ONLY:
            case PREF_NET_TYPE_EVDO_ONLY:
            case PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO:
            case PREF_NET_TYPE_LTE_CDMA_EVDO:
                isValidPreferredNwType = false;
                break;

            case PREF_NET_TYPE_GSM_WCDMA:
            case PREF_NET_TYPE_GSM_ONLY:
            case PREF_NET_TYPE_WCDMA:
            case PREF_NET_TYPE_GSM_WCDMA_AUTO:
            case PREF_NET_TYPE_LTE_GSM_WCDMA:
            case PREF_NET_TYPE_LTE_ONLY:
            case PREF_NET_TYPE_LTE_TDD_ONLY:
            case PREF_NET_TYPE_LTE_WCDMA:
            case PREF_NET_TYPE_LTE_GSM:
                isValidPreferredNwType = true;
                break;
            }
        }
    }
    return isValidPreferredNwType;
}

void RpNwRatController::doPendingRatSwitchRecord() {
    // Handle the pending item
    if (mPendingRestrictedRatSwitchRecord.prefNwType != -1) {
        logD(LOG_TAG, "doPendingRestrictedRatSwitchRecord(), appFamType is %d, prefNwType is %d, nwsMode is %d . ratSwitchCaller is %d",
                mPendingRestrictedRatSwitchRecord.appFamType,
                mPendingRestrictedRatSwitchRecord.prefNwType,
                mPendingRestrictedRatSwitchRecord.nwsMode,
                mPendingRestrictedRatSwitchRecord.ratSwitchCaller);
        logD(LOG_TAG, "doPendingRestrictedRatSwitchRecord(), mCurPreferedNetWorkType is %d, mCurNwsMode is %d.",
                mCurPreferedNetWorkType,
                mCurNwsMode);
        if (mCurPreferedNetWorkType == mPendingRestrictedRatSwitchRecord.prefNwType) {
            logD(LOG_TAG, "doPendingRestrictedRatSwitchRecord(), in restricted mode: prefNwType=%d, nwsMode=%d",
                    mCurPreferedNetWorkType, mCurNwsMode);
        } else {
            switchNwRat(mPendingRestrictedRatSwitchRecord.appFamType,
                    mPendingRestrictedRatSwitchRecord.prefNwType,
                    mPendingRestrictedRatSwitchRecord.nwsMode,
                    mPendingRestrictedRatSwitchRecord.ratSwitchCaller,
                    mPendingRestrictedRatSwitchRecord.action,
                    mPendingRestrictedRatSwitchRecord.message);
        }
    } else if (mPendingInitRatSwitchRecord.prefNwType != -1) {
        logD(LOG_TAG, "doPendingInitRatSwitchRecord(), appFamType is %d, prefNwType is %d, nwsMode is %d . ratSwitchCaller is %d",
                mPendingInitRatSwitchRecord.appFamType,
                mPendingInitRatSwitchRecord.prefNwType,
                mPendingInitRatSwitchRecord.nwsMode,
                mPendingInitRatSwitchRecord.ratSwitchCaller);
        switchNwRat(mPendingInitRatSwitchRecord.appFamType,
                mPendingInitRatSwitchRecord.prefNwType,
                mPendingInitRatSwitchRecord.nwsMode,
                mPendingInitRatSwitchRecord.ratSwitchCaller,
                mPendingInitRatSwitchRecord.action,
                mPendingInitRatSwitchRecord.message);
        mPendingInitRatSwitchRecord.prefNwType = -1;
    } else if (mPendingNormalRatSwitchRecord.prefNwType != -1) {
        logD(LOG_TAG, "doPendingNormalRatSwitchRecord(), appFamType is %d, prefNwType is %d, nwsMode is %d . ratSwitchCaller is %d",
                mPendingNormalRatSwitchRecord.appFamType,
                mPendingNormalRatSwitchRecord.prefNwType,
                mPendingNormalRatSwitchRecord.nwsMode,
                mPendingNormalRatSwitchRecord.ratSwitchCaller);
        switchNwRat(mPendingNormalRatSwitchRecord.appFamType,
                mPendingNormalRatSwitchRecord.prefNwType,
                mPendingNormalRatSwitchRecord.nwsMode,
                mPendingNormalRatSwitchRecord.ratSwitchCaller,
                mPendingNormalRatSwitchRecord.action,
                mPendingNormalRatSwitchRecord.message);
        mPendingNormalRatSwitchRecord.prefNwType = -1;
    } else {
        RpNwRatController *another = (RpNwRatController *) findController(
                getSlotId() == 0 ? 1 : 0,
                RFX_OBJ_CLASS_INFO(RpNwRatController));
        if (another != NULL && another->hasPendingRecord()) {
            logD(LOG_TAG, "doPendingRatSwitchRecord, another SIM has pending record, current is %d", getSlotId());
            another->doPendingRatSwitchRecord();
        }

        logD(LOG_TAG,
                "doPendingRatSwitchRecord(), no pending record, another sim has no pending record also, finish");
    }
}

bool RpNwRatController::hasPendingRecord() {
    if (mPendingInitRatSwitchRecord.prefNwType != -1
            || mPendingNormalRatSwitchRecord.prefNwType != -1
            || mPendingRestrictedRatSwitchRecord.prefNwType != -1) {
        return true;
    }
    return false;
}

int RpNwRatController::calculateTargetPreferredNwType(const AppFamilyType appFamType, int prefNwType, NwsMode nwsMode) {
    int targetPrefNwType;
    if (nwsMode == NWS_MODE_CDMALTE) {
        if (appFamType == APP_FAM_3GPP) {
            targetPrefNwType = prefNwType;
        } else if (appFamType == APP_FAM_3GPP2) {
            switch (prefNwType) {
            case PREF_NET_TYPE_CDMA_EVDO_AUTO:
            case PREF_NET_TYPE_CDMA_ONLY:
            case PREF_NET_TYPE_EVDO_ONLY:
            case PREF_NET_TYPE_LTE_ONLY:
            case PREF_NET_TYPE_LTE_TDD_ONLY:
                targetPrefNwType = prefNwType;
                break;
            case PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO:
            case PREF_NET_TYPE_GSM_WCDMA:
            case PREF_NET_TYPE_GSM_ONLY:
            case PREF_NET_TYPE_WCDMA:
            case PREF_NET_TYPE_GSM_WCDMA_AUTO:
                targetPrefNwType = PREF_NET_TYPE_CDMA_EVDO_AUTO;
                break;
            case PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA:
                if (isRestrictedModeSupport()) {
                    targetPrefNwType = prefNwType;
                    break;
                }
            case PREF_NET_TYPE_LTE_CDMA_EVDO:
            case PREF_NET_TYPE_LTE_GSM_WCDMA:
            case PREF_NET_TYPE_LTE_WCDMA:
            case PREF_NET_TYPE_LTE_GSM:
                targetPrefNwType = PREF_NET_TYPE_LTE_CDMA_EVDO;
                break;
            }
        }
    }

    if (nwsMode == NWS_MODE_CSFB) {
        switch (prefNwType) {
        case PREF_NET_TYPE_CDMA_EVDO_AUTO:
        case PREF_NET_TYPE_CDMA_ONLY:
        case PREF_NET_TYPE_EVDO_ONLY:
        case PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO:
            targetPrefNwType = PREF_NET_TYPE_GSM_WCDMA;
            break;
        case PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA:
            if (isRestrictedModeSupport()) {
                targetPrefNwType = prefNwType;
                break;
            }
        case PREF_NET_TYPE_LTE_CDMA_EVDO:
            targetPrefNwType = PREF_NET_TYPE_LTE_GSM_WCDMA;
            break;
        case PREF_NET_TYPE_GSM_WCDMA:
        case PREF_NET_TYPE_GSM_ONLY:
        case PREF_NET_TYPE_WCDMA:
        case PREF_NET_TYPE_GSM_WCDMA_AUTO:
        case PREF_NET_TYPE_LTE_GSM_WCDMA:
        case PREF_NET_TYPE_LTE_ONLY:
        case PREF_NET_TYPE_LTE_TDD_ONLY:
        case PREF_NET_TYPE_LTE_WCDMA:
        case PREF_NET_TYPE_LTE_GSM:
            targetPrefNwType = prefNwType;
            break;
        }
    }
    return targetPrefNwType;
}

void RpNwRatController:: updateState(NwsMode nwsMode, int prefNwType){
    logD(LOG_TAG,"updateNwsMode(), nwsMode is %d, prefNwType is %d. ", nwsMode, prefNwType);
    mCurNwsMode = nwsMode;
    mCurPreferedNetWorkType = prefNwType;
    getStatusManager()->setIntValue(RFX_STATUS_KEY_NWS_MODE, mCurNwsMode);
    getStatusManager()->setIntValue(RFX_STATUS_KEY_PREFERRED_NW_TYPE, mCurPreferedNetWorkType);
    sIsInSwitching = false;
}

bool RpNwRatController::getSwitchState() {
    return sIsInSwitching;
}

int RpNwRatController::getEnginenerMode() {
    char property_value[PROPERTY_VALUE_MAX] = { 0 };
    property_get("persist.radio.ct.ir.engmode", property_value, "0");
    int engineerMode = atoi(property_value);
    logD(LOG_TAG,"getEnginenerMode(), engineerMode is %d. ",engineerMode);
    return engineerMode;
}

int RpNwRatController::getChipTestMode() {
    int mode = 0;
    char chipsetMode[PROPERTY_VALUE_MAX] = { 0 };
    property_get("persist.chiptest.enable", chipsetMode, "0");
    mode = atoi(chipsetMode);
    logD(LOG_TAG,"getChipTestMode():%d", mode);
    return mode;
}

void RpNwRatController::clearSuggetRadioCapability() {
    getStatusManager()->setIntValue(RFX_STATUS_KEY_SUGGESTED_RADIO_CAPABILITY,
            RIL_CAPABILITY_NONE);
    logD(LOG_TAG, "clearSuggetRadioCapability");
}

void RpNwRatController::clearInvalidPendingRecords() {
    if (mPendingInitRatSwitchRecord.prefNwType != -1) {
        logD(LOG_TAG, "has pending init rat switch record: appFamType is %d, prefNwType is %d, nwsMode is %d . ratSwitchCaller is %d",
                mPendingInitRatSwitchRecord.appFamType,
                mPendingInitRatSwitchRecord.prefNwType,
                mPendingInitRatSwitchRecord.nwsMode,
                mPendingInitRatSwitchRecord.ratSwitchCaller);
        if (mPendingInitRatSwitchRecord.action != NULL) {
            mPendingInitRatSwitchRecord.action->act();
        }
        if (mPendingInitRatSwitchRecord.message != NULL) {
            mNwRatSwitchHandler->responseSetPreferredNetworkType(
                    mPendingInitRatSwitchRecord.message);
        }
        mPendingInitRatSwitchRecord.prefNwType = -1;
    }
    if (mPendingNormalRatSwitchRecord.prefNwType != -1) {
        logD(LOG_TAG, "has pending normal rat switch record: appFamType is %d, prefNwType is %d, nwsMode is %d . ratSwitchCaller is %d",
                mPendingNormalRatSwitchRecord.appFamType,
                mPendingNormalRatSwitchRecord.prefNwType,
                mPendingNormalRatSwitchRecord.nwsMode,
                mPendingNormalRatSwitchRecord.ratSwitchCaller);
        if (mPendingNormalRatSwitchRecord.action != NULL) {
            mPendingNormalRatSwitchRecord.action->act();
        }
        if (mPendingNormalRatSwitchRecord.message != NULL) {
            mNwRatSwitchHandler->responseSetPreferredNetworkType(
                    mPendingNormalRatSwitchRecord.message);
        }
        mPendingNormalRatSwitchRecord.prefNwType = -1;
    }
    logD(LOG_TAG, "the app family type change, clearInvalidPendingRecords");
}

bool RpNwRatController::isRestrictedModeSupport() {
    bool mode = false;
    char prop_val[PROPERTY_VALUE_MAX] = { 0 };
    property_get("ro.operator.optr", prop_val, "");
    if (strcmp("OP12", prop_val) == 0) {
        mode = true;
    }
    return mode;
}

void RpNwRatController::clearState(){
    logD(LOG_TAG, "clearState");
    mCurAppFamilyType = APP_FAM_UNKNOWN ;
    mCurPreferedNetWorkType = -1;
    mCurNwsMode = NWS_MODE_CDMALTE;
    mNewAppFamilyType = APP_FAM_UNKNOWN;
    mNewPreferedNetWorkType = -1;
    mNewNwsMode = NWS_MODE_CDMALTE;
    mPendingInitRatSwitchRecord.prefNwType = -1;
    mPendingNormalRatSwitchRecord.prefNwType = -1;
}
