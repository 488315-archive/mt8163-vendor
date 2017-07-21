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
#include <stdio.h>
#include <stdlib.h>
#include <cutils/properties.h>
#include "RfxLog.h"
#include "RpIrCdmaHandler.h"
#include "RpIrController.h"
#include "RpIrLwgHandler.h"
#include "RpIrMccTable.h"
#include "RpIrStrategyOP094M.h"
#include "RpIrStrategyOP095M.h"
#include "RpIrStrategyOm.h"
#include "RpIrNwRatSwitchCallback.h"


#define MCC_CN_MAINLAND 460
#define MCC_CN_MACCO 455
#define MCC_JP 440
#define MCC_KR 450

#define RFX_LOG_TAG "[IRC]"
/*****************************************************************************
 * Class RpIrController
 *****************************************************************************/

int RpIrController::sNoServiceDelayTime = NO_SERVICE_DELAY_TIME;

RFX_IMPLEMENT_CLASS("RpIrController", RpIrController, RfxController);

RpIrController::RpIrController():
    mIsEnabled(false),
    mIsGmssEnable(true),
    mIsSwitchingTo3GMode(false),
    mNwsMode(NWS_MODE_UNKNOWN),
    mPtrSwitchStrategy(NULL),
    mPtrLwgHandler(NULL),
    mPtrCdmaHandler(NULL),
    mNwRatSwitchCallback(NULL),
    mMccInGmss(0) {
}

RpIrController::~RpIrController() {
}

void RpIrController::onInit()
{
    RfxController::onInit();

    char strProp[PROPERTY_VALUE_MAX] = {0};
    property_get("persist.sys.ct.ir.switcher", strProp, "1");
    int nProp = atoi(strProp);
    if (nProp != 1) {
        logD(RFX_LOG_TAG, "onInit CT IR switcher is 0, return.");
        return;
    }

    memset(strProp, 0, sizeof(char)*PROPERTY_VALUE_MAX);
    property_get("persist.radio.ct.ir.engmode", strProp, "0");
    nProp = atoi(strProp);
    if (nProp != ENGINEER_MODE_AUTO) {
        logD(RFX_LOG_TAG, "onInit CT IR engmode is %d, return.", nProp);
        return;
    }

    const int urc_id_list[] = {
        RIL_UNSOL_GMSS_RAT_CHANGED,
    };

    registerToHandleUrc(urc_id_list, 1);

    RFX_OBJ_CREATE_EX(mPtrLwgHandler, RpIrLwgHandler, this, (this));
    RFX_OBJ_CREATE_EX(mPtrCdmaHandler, RpIrCdmaHandler, this, (this));

    // for debug to adjuest no service delay time
    char strTemp[20] = {0};
    sprintf(strTemp, "%d", NO_SERVICE_DELAY_TIME);
    memset(strProp, 0, sizeof(char)*PROPERTY_VALUE_MAX);
    property_get("persist.sys.ct.ir.nsd", strProp, strTemp);
    sNoServiceDelayTime = atoi(strProp);

    // get ir mode
    memset(strProp, 0, sizeof(char)*PROPERTY_VALUE_MAX);
    property_get("persist.sys.ct.ir.mode", strProp, "0");
    int nMode = atoi(strProp);

    logD(RFX_LOG_TAG, "onInit ir mode=%d", nMode);

    if (nMode == 0) {
        memset(strProp, 0, sizeof(char)*PROPERTY_VALUE_MAX);
        property_get("ro.mtk_svlte_lcg_support", strProp, "0");
        nProp = atoi(strProp);
        if (nProp == 1) {
            logD(RFX_LOG_TAG, "onInit StrategyOP094M");
            RFX_OBJ_CREATE_EX(mPtrSwitchStrategy, RpIrStrategyOP094M, this,
                    (this, (IRpIrNwController*)mPtrLwgHandler,
                    (IRpIrNwController*)mPtrCdmaHandler));
        } else {
            logD(RFX_LOG_TAG, "onInit StrategyOP095M");
            RFX_OBJ_CREATE_EX(mPtrSwitchStrategy, RpIrStrategyOP095M, this,
                    (this, (IRpIrNwController*)mPtrLwgHandler,
                    (IRpIrNwController*)mPtrCdmaHandler));
        }
    } else if (nMode == 5) {
        logD(RFX_LOG_TAG, "onInit StrategyOP095M");
        RFX_OBJ_CREATE_EX(mPtrSwitchStrategy, RpIrStrategyOP095M, this,
                    (this, (IRpIrNwController*)mPtrLwgHandler,
                    (IRpIrNwController*)mPtrCdmaHandler));
    } else if (nMode == 4) {
        logD(RFX_LOG_TAG, "onInit StrategyOP094M");
        RFX_OBJ_CREATE_EX(mPtrSwitchStrategy, RpIrStrategyOP094M, this,
                    (this, (IRpIrNwController*)mPtrLwgHandler,
                    (IRpIrNwController*)mPtrCdmaHandler));
    } else {
        logD(RFX_LOG_TAG, "onInit no Strategy!!!");
        RFX_ASSERT(0);
    }

    mNwsMode = NWS_MODE_CDMALTE;

    RFX_OBJ_CREATE_EX(mNwRatSwitchCallback,
                      RpIrNwRatSwitchCallback,
                      this,
                      (this, mPtrCdmaHandler, mPtrLwgHandler, mPtrSwitchStrategy));

    RpNwRatController* nwRatController;
    nwRatController = (RpNwRatController*)findController(RFX_OBJ_CLASS_INFO(RpNwRatController));
    nwRatController->registerRatSwitchCallback(mNwRatSwitchCallback);
}

bool RpIrController::onHandleUrc(const sp<RfxMessage>& urc) {
    int urcId = urc->getId();
    sp<RfxMessage> urcToRilj = urc;
    logD(RFX_LOG_TAG, "onHandleUrc(), %d", urcId);
    if (urc->getError() == RIL_E_SUCCESS) {
        switch (urcId) {
            case RIL_UNSOL_GMSS_RAT_CHANGED:
                processGmssRatChanged(urc);
                break;
            default:
                break;
        }
    } else {
        // urc contains error
    }
    return true;
}

void RpIrController::setNwsMode(NwsMode nwsMode, const sp<RfxAction>& action)
{
    logD(RFX_LOG_TAG, "setNwsMode, nwsMode=%s, mIsEnabled =%s, mIsSwitchingTo3GMode=%s",
              Nws2Str(nwsMode), Bool2Str(mIsEnabled), Bool2Str(mIsSwitchingTo3GMode));
    if (!mIsEnabled && !mIsSwitchingTo3GMode) {
        logD(RFX_LOG_TAG, "setNwsMode, nwsMode=%s, in disabled mode", Nws2Str(nwsMode));
        mNwsMode = nwsMode;
        if (action != NULL) {
            action->act();
        }
    } else {
        bool ret = true; // wait RAT controller add return value to setNwsMode()
        RpNwRatController* nwRatController;
        nwRatController = (RpNwRatController *) findController(RFX_OBJ_CLASS_INFO(RpNwRatController));
        nwRatController->setNwsMode(nwsMode, action);
        if (ret) {
            logD(RFX_LOG_TAG, "setNwsMode, nwsMode=%s", Nws2Str(nwsMode));
            mNwsMode = nwsMode;
        } else {
            logD(RFX_LOG_TAG, "setNwsMode, nwsMode=%s, return false", Nws2Str(nwsMode));
        }
    }
}

NwsMode RpIrController::getNwsMode() {
    logD(RFX_LOG_TAG, "getNwsMode, mNwsMode=%s", Nws2Str(mNwsMode));
    return mNwsMode;
}

bool RpIrController::isSimReady() {
    int sim_state = getStatusManager()->getIntValue(RFX_STATUS_KEY_SIM_STATE);
    bool bSimReady = sim_state == RFX_SIM_STATE_READY ? true : false;
    logD(RFX_LOG_TAG, "isSimReady, bSimReady=%s, sim_state = %d", Bool2Str(bSimReady), sim_state);
    return bSimReady;
}

bool RpIrController::isCdmaDualModeSimCard() {
    if (getStatusManager()->getBoolValue(RFX_STATUS_KEY_CT3G_DUALMODE_CARD)) {
        logD(RFX_LOG_TAG, "isCdmaDualModeSimCard, is CT3G dual mode card");
        return true;
    } else {
        int nCardType = getStatusManager()->getIntValue(RFX_STATUS_KEY_CARD_TYPE);
        bool ret = RFX_FLAG_HAS_ALL(nCardType, (RFX_CARD_TYPE_USIM | RFX_CARD_TYPE_CSIM));
        logD(RFX_LOG_TAG, "isCdmaDualModeSimCard, nCardType=0x%x, ret = %s", nCardType, Bool2Str(ret));
        return ret;
    }
}

bool RpIrController::isCtSimCard() {
    bool ret = false;
    int cdma_card_type = getStatusManager()->getIntValue(RFX_STATUS_KEY_CDMA_CARD_TYPE);
    if (cdma_card_type == CT_4G_UICC_CARD ||
        cdma_card_type == CT_UIM_SIM_CARD ||
        cdma_card_type == CT_3G_UIM_CARD) {
        ret = true;
    }
    logD(RFX_LOG_TAG, "isCtSimCard, CdmaCardType=%d, ret = %s", cdma_card_type, Bool2Str(ret));
    return ret;
}


void RpIrController::setIrControllerEnableState(bool isEnabled) {
    logD(RFX_LOG_TAG, "setIrControllerEnableState, isEnabled =%s", Bool2Str(isEnabled));
    mIsEnabled = isEnabled;
    mPtrSwitchStrategy->setIfEnabled(isEnabled);
}

bool RpIrController::getIrControllerEnableState() {
    return mIsEnabled;
}

void RpIrController::setIrEnableState(bool isEnabled) {
    logD(RFX_LOG_TAG, "setIrEnableState, isEnabled =%s", Bool2Str(isEnabled));
    setIrControllerEnableState(isEnabled);
    mPtrLwgHandler->setIfEnabled(isEnabled);
    mPtrCdmaHandler->setIfEnabled(isEnabled);
}

void RpIrController::dispose() {
    mPtrLwgHandler->dispose();
    mPtrCdmaHandler->dispose();
}

void RpIrController::onNwsModeChange(NwsMode curMode) {
    logD(RFX_LOG_TAG, "onNwsModeChange, preMode=%s, curMode=%s",
               Nws2Str(mNwsMode), Nws2Str(curMode));
    if (curMode != mNwsMode) {
        mNwsMode = curMode;
        mPtrLwgHandler->startNewSearchRound();
        mPtrCdmaHandler->startNewSearchRound();
    }
}

/*****************************************************************************
 * Handle URC for GMSS RAT changed start
 *****************************************************************************/
#define RFX_LOG_TAG_GMSS "IRC_GMSS"

void RpIrController::setGmssEnableState(bool isEnabled) {
    logD(RFX_LOG_TAG_GMSS, "setGmssEnableState, isEnabled =%s", Bool2Str(isEnabled));
    mIsGmssEnable = isEnabled;
}

bool RpIrController::getGmssEnableState() {
    return mIsGmssEnable;
}

bool RpIrController::needWaitImsi() {
    if (isCtSimCard()) {
        // already know it's CT card, so don't need to
        // wait for IMSI ready to get mcc
        logD(RFX_LOG_TAG_GMSS, "needWaitImsi: %s, isCtSimCard", Bool2Str(false));
        return false;
    }
    bool bNeedWaitImsi = isCdmaDualModeSimCard();
    logD(RFX_LOG_TAG_GMSS, "needWaitImsi: %s", Bool2Str(bNeedWaitImsi));
    return bNeedWaitImsi;
}


void RpIrController::processGmssRatChanged(const sp<RfxMessage>& msg) {
    if (!mIsGmssEnable) {
        logD(RFX_LOG_TAG_GMSS, "processGmssRatChanged, Gmss not Enabled");
        return;
    }

    int32_t count = 0;
    int32_t rat = 0;
    int32_t mcc = 0;
    int32_t attached = 0;

    Parcel *p = msg->getParcel();
    count = p->readInt32();
    rat = p->readInt32();
    mcc = p->readInt32();
    attached = p->readInt32();
    logD(RFX_LOG_TAG_GMSS, "processGmssRatChanged, count=%d rat=%d mcc=%d attached=%d",
            count, rat, mcc, attached);

    if (needWaitImsi()) {
        if (getStatusManager()->getString8Value(RFX_STATUS_KEY_C2K_IMSI).isEmpty() &&
                    getSlotId() == getNonSlotScopeStatusManager()->getIntValue(RFX_STATUS_KEY_ACTIVE_CDMALTE_MODE_SLOT)) {
            logD(RFX_LOG_TAG_GMSS, "processGmssRatChanged, Wait for C2K IMSI ready MCC = %d", mcc);
            mMccInGmss = mcc;
            getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_C2K_IMSI,
                    RfxStatusChangeCallback(this, &RpIrController::onImsiChanged));
        } else {
            onGmssRatChanged(mcc);
        }
    } else {
        onGmssRatChanged(mcc);
    }
}

void RpIrController::onImsiChanged(RfxStatusKeyEnum key, RfxVariant old_value, RfxVariant value) {
    RFX_UNUSED(key);
    RFX_UNUSED(old_value);
    logD(RFX_LOG_TAG_GMSS, "RpIrController::onImsiChanged() imsi = %s", value.toString().string());
    RFX_ASSERT(!value.asString8().isEmpty());
    getStatusManager()->unRegisterStatusChanged(RFX_STATUS_KEY_C2K_IMSI,
            RfxStatusChangeCallback(this, &RpIrController::onImsiChanged));
    onGmssRatChanged(mMccInGmss);
}


void RpIrController::onGmssRatChanged(int mcc) {
    if (!mIsGmssEnable) {
        logD(RFX_LOG_TAG_GMSS, "onGmssRatChanged, Gmss not Enabled");
        return;
    }

    NwsMode targetMode = getNwsModeByMcc(mcc);
    RpNwRatController* nwRatController;
    nwRatController = (RpNwRatController *)findController(RFX_OBJ_CLASS_INFO(RpNwRatController));
    nwRatController->setNwsMode(targetMode, NULL);
    onNwsModeChange(targetMode);
}

NwsMode RpIrController::getNwsModeByMcc(int mcc) {
    NwsMode nwsMode = NWS_MODE_CSFB;

    int home_mcc = mPtrSwitchStrategy->getHomeMcc();
    logD(RFX_LOG_TAG_GMSS, "getNwsModeByMcc, home_mcc = %d, mcc=%d, is_sim_ready = %s",
                      home_mcc, mcc, Bool2Str(isSimReady()));
    if (home_mcc == 0) {
        // if bootup in CSFB mode, can't get C2K IMSI, so keep CSFB mode,
        // otherwise, it's in CDMALTE mode, should be able to get C2K IMSI
        logD(RFX_LOG_TAG_GMSS, "getNwsModeByPlmn, mcc=[%d]  ret: [NWS_MODE_CSFB]", mcc);
        return NWS_MODE_CSFB;
    }
    logD(RFX_LOG_TAG_GMSS, "getNwsModeByMcc, mcc=%d", mcc);

    int nProp = 0;
    char strProp[PROPERTY_VALUE_MAX] = { 0 };
    memset(strProp, 0, sizeof(char) * PROPERTY_VALUE_MAX);
    property_get("ro.mtk_svlte_lcg_support", strProp, "0");
    nProp = atoi(strProp);
    if (nProp == 1) {
        logD(RFX_LOG_TAG_GMSS, "getNwsModeByMcc, checking JPKR NWS mode");
        if (mcc == MCC_JP || mcc == MCC_KR) {
            nwsMode = NWS_MODE_CDMALTE;
        }
    }

    if (nwsMode != NWS_MODE_CDMALTE) {
        logD(RFX_LOG_TAG_GMSS, "getNwsModeByMcc, checking normal NWS mode");
        bool is_home = RpIrMccTable::isSameCountryCode(home_mcc, mcc);
        if (is_home) {
            logD(RFX_LOG_TAG_GMSS, "getNwsModeByMcc, mcc=[%d]  ret: [NWS_MODE_CDMALTE]", mcc);
            return NWS_MODE_CDMALTE;
        } else {
            logD(RFX_LOG_TAG_GMSS, "getNwsModeByMcc, mcc=[%d]  ret: [NWS_MODE_CSFB]", mcc);
            return NWS_MODE_CSFB;
        }
    }

    logD(RFX_LOG_TAG_GMSS, "getNwsModeByMcc, mcc=%d, nwsMode=%s", mcc, nwsModeToString(nwsMode));
    return nwsMode;
}

/*****************************************************************************
 * Handle URC for GMSS RAT changed end
 *****************************************************************************/

const char* RpIrController::boolToString(bool value) {
    return value ? "true" : "false";
}

const char* RpIrController::nwsModeToString(NwsMode mode) {
    switch (mode) {
    case NWS_MODE_UNKNOWN:
        return "NWS_MODE_UNKNOWN";
        break;
    case NWS_MODE_CDMALTE:
        return "NWS_MODE_CDMALTE";
        break;
    case NWS_MODE_CSFB:
        return "NWS_MODE_CSFB";
        break;
    default:
        break;
    }
    RFX_ASSERT(0);
    return "";
}

const char *RpIrController::serviceTypeToString(ServiceType serviceType) {
    switch (serviceType) {
        case OUT_OF_SERVICE:
            return "OUT_OF_SERVICE";
        case IN_SEARCHING:
            return "IN_SEARCHING";
        case IN_SERVICE:
            return "IN_SERVICE";
        default:
            RFX_ASSERT(0);
    }
    RFX_ASSERT(0);
    return "";
}


