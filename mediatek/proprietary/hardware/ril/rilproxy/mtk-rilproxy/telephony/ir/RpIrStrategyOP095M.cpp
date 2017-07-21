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

#include "RfxAction.h"
#include "RfxLog.h"
#include "RpIrStrategyOP095M.h"
#include "RpIrOP09CdmaStatusListener.h"
#include "RpIrOP09LwgStatusListener.h"
#include "RpIrMccTable.h"
#include <cutils/properties.h>

using namespace std;

#define RFX_LOG_TAG "[IRC][RpIrStrategyOP095M]"
/*****************************************************************************
 * Class RpIrStrategyOP095M
 *****************************************************************************/

RFX_IMPLEMENT_VIRTUAL_CLASS("RpIrStrategyOP095M", RpIrStrategyOP095M, RpIrStrategy);

int RpIrStrategyOP095M::sWatchdogDelayTime = NO_SEERVICE_WATCHDOG_DELAY_TIME;
int RpIrStrategyOP095M::sSwitchModeOrResumeDelayTime = SWITCH_RESUME_DELAY_TIME;

RpIrStrategyOP095M::RpIrStrategyOP095M(RpIrController* ric, IRpIrNwController* lwgController,
                               IRpIrNwController* cdmaController) :
        RpIrStrategy(ric, lwgController, cdmaController),
        mCdmaNwsMode(NWS_MODE_UNKNOWN),
        mLwgServiceState(OUT_OF_SERVICE),
        mCdmaServiceState(OUT_OF_SERVICE),
        mNoServiceTimeStamp(0),
        mWatchdogStartTime(0),
        mContinousRetryCount(0),
        m_lwgListener(NULL),
        m_cdmaListener(NULL),
        m_timer_handle_watchDog(NULL),
        m_timer_handle_watchDog_started(false) {
}

void RpIrStrategyOP095M::onInit() {
    RpIrStrategy::onInit(); // Required: invoke super class implementation

    char strProp[PROPERTY_VALUE_MAX] = {0};
    char strTemp[20] = {0};

    sprintf(strTemp, "%d", NO_SEERVICE_WATCHDOG_DELAY_TIME);
    memset(strProp, 0, sizeof(char)*PROPERTY_VALUE_MAX);
    property_get("persist.sys.ct.ir.wd", strProp, strTemp);
    RpIrStrategyOP095M::sWatchdogDelayTime = atoi(strProp);

    sprintf(strTemp, "%d", SWITCH_RESUME_DELAY_TIME);
    memset(strProp, 0, sizeof(char)*PROPERTY_VALUE_MAX);
    property_get("persist.sys.ct.ir.rnsd", strProp, strTemp);
    RpIrStrategyOP095M::sSwitchModeOrResumeDelayTime= atoi(strProp);

    RpIrOP09LwgStatusListener *lwgListener;
    RFX_OBJ_CREATE_EX(lwgListener, RpIrOP09LwgStatusListener, this, (this, m_IrController));
    m_lwgListener = (IRpIrNwControllerListener *)lwgListener;

    RpIrOP09CdmaStatusListener *cdmaListener;
    RFX_OBJ_CREATE_EX(cdmaListener, RpIrOP09CdmaStatusListener, this, (this, m_IrController));
    m_cdmaListener = (IRpIrNwControllerListener *)cdmaListener;

    m_LwgController->registerListener(m_lwgListener);
    m_CdmaController->registerListener(m_cdmaListener);
}

RpIrStrategyOP095M::~RpIrStrategyOP095M() {
}

void RpIrStrategyOP095M::onLwgPlmnChanged(const String8 &plmn) {
    logD(RFX_LOG_TAG, "onLwgPlmnChanged plmn: %s", plmn.string());

    if (supportRoaming()) {
        NwsMode targetMode = getNwsModeByPlmn(plmn);

        logD(RFX_LOG_TAG, "onLwgPlmnChanged mCdmaNwsMode: %s targetMode: %s",
                          Nws2Str(mCdmaNwsMode), Nws2Str(targetMode));

        if (targetMode == m_IrController->getNwsMode()) {
            m_LwgController->resumeNetwork();
        }
        m_LwgController->setNwsMode(targetMode);
    } else {
        m_LwgController->resumeNetwork();
        m_LwgController->setNwsMode(NWS_MODE_CDMALTE);
    }
}

void RpIrStrategyOP095M::onCdmaPlmnChanged(const String8 &plmn) {
    logD(RFX_LOG_TAG, "onCdmaPlmnChanged plmn: %s", plmn.string());
    // record cdma latest roaming mode for LWG
    // to decide its roaming status
    mCdmaNwsMode = getNwsModeByPlmn(plmn);

    if (supportRoaming()) {
        // these are OP09 IR 5M stratrgy deails
        // cdma roaming mode has higher priority
        if (mCdmaNwsMode == m_IrController->getNwsMode()
            && mCdmaNwsMode != NWS_MODE_CSFB) {
            m_CdmaController->resumeNetwork();
        }
        m_CdmaController->setNwsMode(mCdmaNwsMode);
    }
    else if (getStatusManager()->getIntValue(RFX_STATUS_KEY_CDMA_CARD_TYPE) == CT_3G_UIM_CARD
             && mCdmaNwsMode == NWS_MODE_CSFB) {
        // OP09 3g uim card (not dual mode) + roaming = do not resume.
        logD(RFX_LOG_TAG, "onCdmaPlmnChanged OP09 3g uim card (not dual mode) + roaming, do not resume.");
        return;
    }
    else {
        m_CdmaController->resumeNetwork();
        m_CdmaController->setNwsMode(NWS_MODE_CDMALTE);
    }
}

void RpIrStrategyOP095M::onSetIfEnabled(bool enabled) {
    RFX_UNUSED(enabled);
    updateWatchdog();
}

int RpIrStrategyOP095M::getHomeMcc() {
    if (m_IrController->isCtSimCard()) {
        // Heji card mcc is 454, but as CT spec, we should
        // process it as CT card, so can't only check mcc
        return CHINA_TELECOM_MAINLAND_MCC;
    }
    String8 home_imsi = getStatusManager()->getString8Value(RFX_STATUS_KEY_C2K_IMSI);
    if (home_imsi == String8("")) {
        logD(RFX_LOG_TAG, "No C2K IMSI");
        return 0;
    }
    if (home_imsi.length() < 3) {
        logD(RFX_LOG_TAG, "Invalid C2K IMSI %s", home_imsi.string());
        RFX_ASSERT(0);
        return 0;
    }
    const char *imsi = home_imsi.string();
    char home_mcc_str[4];
    home_mcc_str[0] = imsi[0];
    home_mcc_str[1] = imsi[1];
    home_mcc_str[2] = imsi[2];
    home_mcc_str[3] = 0;
    int home_mcc = atoi(home_mcc_str);
    logD(RFX_LOG_TAG, "Home mcc = %d", home_mcc);
    return home_mcc;
}


NwsMode RpIrStrategyOP095M::getNwsModeByPlmn(const String8& plmn) {
    if (!plmn.isEmpty()) {
        // For 5m project
        int mcc = atoi(string(plmn.string()).substr(0, 3).c_str());
        int home_mcc = getHomeMcc();
        logD(RFX_LOG_TAG, "getNwsModeByPlmn, home_mcc = %d, mcc=%d, is_sim_ready = %s",
                          home_mcc, mcc, Bool2Str(m_IrController->isSimReady()));
        if (home_mcc == 0) {
            // if bootup in CSFB mode, can't get C2K IMSI, so keep CSFB mode,
            // otherwise, it's in CDMALTE mode, should be able to get C2K IMSI
            logD(RFX_LOG_TAG, "getNwsModeByPlmn, plmn=[%s]  ret: [NWS_MODE_CSFB]", plmn.string());
            return NWS_MODE_CSFB;
        }
        bool is_home = RpIrMccTable::isSameCountryCode(home_mcc, mcc);
        logD(RFX_LOG_TAG, "getNwsModeByPlmn, is_home = %s", Bool2Str(is_home));
        if ((mcc == CHINA_TELECOM_MAINLAND_MCC || mcc == CHINA_TELECOM_MACCO_MCC) &&
            home_mcc == CHINA_TELECOM_MAINLAND_MCC) {
            logD(RFX_LOG_TAG, "getNwsModeByPlmn, plmn=[%s]  ret: [NWS_MODE_CDMALTE]", plmn.string());
            return NWS_MODE_CDMALTE;
        } else if (is_home) {
            logD(RFX_LOG_TAG, "getNwsModeByPlmn, plmn=[%s]  ret: [NWS_MODE_CDMALTE]", plmn.string());
            return NWS_MODE_CDMALTE;
        } else {
            logD(RFX_LOG_TAG, "getNwsModeByPlmn, plmn=[%s]  ret: [NWS_MODE_CSFB]", plmn.string());
            return NWS_MODE_CSFB;
        }
    } else {
        logD(RFX_LOG_TAG, "getNwsModeByPlmn, plmn=[%s]  ret: [NWS_MODE_CSFB]", plmn.string());
        return NWS_MODE_CSFB;
    }
}

bool RpIrStrategyOP095M::isDualServiceNotInService() {
    bool ret = (mCdmaServiceState != IN_SERVICE) && (mLwgServiceState != IN_SERVICE);
    logD(RFX_LOG_TAG, "isDualServiceNotInService() : %s", Bool2Str(ret));
    return ret;
}

bool RpIrStrategyOP095M::isDualRadioOff() {
    bool ret = !m_IsLwgRadioOn && !m_IsCdmaRadioOn;
    logD(RFX_LOG_TAG, "isDualRadioOff() m_IsLwgRadioOn = %s, m_IsCdmaRadioOn = %s, ret = %s",
                       Bool2Str(m_IsLwgRadioOn), Bool2Str(m_IsCdmaRadioOn), Bool2Str(ret));
    return ret;
}

bool RpIrStrategyOP095M::supportRoaming() {
    bool bSupportRoaming = m_IrController->isCdmaDualModeSimCard();
    logD(RFX_LOG_TAG, "supportRoaming, bSupportRoaming=%s", Bool2Str(bSupportRoaming));
    return bSupportRoaming;
}

bool RpIrStrategyOP095M::switchForNoService(bool forceSwitch) {
    logD(RFX_LOG_TAG, "switchForNoService mLwgServiceState: %s, mCdmaServiceState: %s",
                      ServiceType2Str(mLwgServiceState), ServiceType2Str(mCdmaServiceState));
    bool reallySwitchForNoService = false;
    int state = getStatusManager()->getIntValue(RFX_STATUS_KEY_WORLD_MODE_STATE, 1);
    if (state == 0) {
        logD(RFX_LOG_TAG, "switchForNoService, world mode is switching, return.");
        return reallySwitchForNoService;
    }

    if (!isDualRadioOff() && supportRoaming() && m_IrController->isSimReady()) {

        nsecs_t curTime = systemTime();

        nsecs_t duration = curTime - mNoServiceTimeStamp;
        stopNoServiceWatchdog();

        // prevent no service switch happens too
        // frequently, use a time stamp
        if (mNoServiceTimeStamp == 0 ||
            duration > ms2ns(sWatchdogDelayTime) ||
            forceSwitch) {
            logD(RFX_LOG_TAG, "switchForNoService really switched");

            if (m_IrController->getNwsMode() == NWS_MODE_CDMALTE) {
                sp<RfxAction> action = new RfxAction0(this, &RpIrStrategyOP095M::onActionNwsModeChanged);
                m_IrController->setNwsMode(NWS_MODE_CSFB, action);
            } else {
                sp<RfxAction> action = new RfxAction0(this, &RpIrStrategyOP095M::onActionNwsModeChanged);
                m_IrController->setNwsMode(NWS_MODE_CDMALTE, action);
            }
            mNoServiceTimeStamp = curTime;

            mCdmaNwsMode = NWS_MODE_UNKNOWN; // clear cdma record
            reallySwitchForNoService = true;
        } else {
            logD(RFX_LOG_TAG, "switchForNoService delay switch, sWatchdogDelayTime=%dms, duration=%dms",
                           sWatchdogDelayTime, (int)ns2ms(duration));
            RfxTimer::stop(m_timer_handle_watchDog);
            m_timer_handle_watchDog = RfxTimer::start(RfxCallback0(this,&RpIrStrategyOP095M::triggerNoServiceWatchdog),
                                                      ms2ns(sWatchdogDelayTime) - duration);
            m_timer_handle_watchDog_started = true;
        }
    }
    logD(RFX_LOG_TAG, "switchForNoService reallySwitchForNoService: %s",
                      Bool2Str(reallySwitchForNoService));
    return reallySwitchForNoService;
}

void RpIrStrategyOP095M::updateWatchdog() {
    if (mCdmaServiceState == IN_SERVICE ||
        mLwgServiceState == IN_SERVICE ||
        isDualRadioOff()) {
        mContinousRetryCount = 0;
    }

    if (!isDualRadioOff() && isDualServiceNotInService() && getIfEnabled()) {
        startNoServiceWatchdog();
    } else {
        stopNoServiceWatchdog();
    }
}

void RpIrStrategyOP095M::onRadioStateChanged() {
    NwsMode nwsMode = m_IrController->getNwsMode();

    logD(RFX_LOG_TAG, "onRadioStateChanged m_IsLwgRadioOn: %s m_IsCdmaRadioOn: %s nwsMode: %s",
                   Bool2Str(m_IsLwgRadioOn), Bool2Str(m_IsCdmaRadioOn), Nws2Str(nwsMode));
    if (isDualRadioOff()) {
        mCdmaNwsMode = NWS_MODE_UNKNOWN; // clear cdma record
    }

    updateWatchdog();
}

void RpIrStrategyOP095M::onActionNwsModeChanged() {
    m_LwgController->startNewSearchRound();
    m_CdmaController->startNewSearchRound();
}

void RpIrStrategyOP095M::triggerNoServiceWatchdog() {
    logD(RFX_LOG_TAG, "triggerNoServiceWatchdog mLwgServiceState: %s mCdmaServiceState: %s",
                   ServiceType2Str(mLwgServiceState), ServiceType2Str(mCdmaServiceState));
    if (isDualServiceNotInService()) {
        switchForNoService(false);
    }
    updateWatchdog();
}

void RpIrStrategyOP095M::startNoServiceWatchdog() {
    logD(RFX_LOG_TAG, "startNoServiceWatchdog");
    if (!m_timer_handle_watchDog_started) {
        char strProp[PROPERTY_VALUE_MAX] = {0};
        char strTemp[20] = {0};

        sprintf(strTemp, "%d", NO_SEERVICE_WATCHDOG_DELAY_TIME);
        memset(strProp, 0, sizeof(char)*PROPERTY_VALUE_MAX);
        property_get("persist.sys.ct.ir.wd", strProp, strTemp);
        RpIrStrategyOP095M::sWatchdogDelayTime = atoi(strProp);

        sWatchdogDelayTime += (mContinousRetryCount * WATCHDOG_RETRY_DELAY_STEP);
        if (sWatchdogDelayTime > MAX_WATCHDOG_RETRY_DELAY) {
            sWatchdogDelayTime = MAX_WATCHDOG_RETRY_DELAY;
        }
        mWatchdogStartTime = systemTime();
        logD(RFX_LOG_TAG, "really start watchdog sWatchdogDelayTime = %dms, mContinousRetryCount = %d",
                        sWatchdogDelayTime, mContinousRetryCount);
        m_timer_handle_watchDog = RfxTimer::start(RfxCallback0(this, &RpIrStrategyOP095M::triggerNoServiceWatchdog),
                                                  ms2ns(sWatchdogDelayTime));
        m_timer_handle_watchDog_started = true;
        mContinousRetryCount++;
    }
}

void RpIrStrategyOP095M::stopNoServiceWatchdog() {
    logD(RFX_LOG_TAG, "stopNoServiceWatchdog");
    mWatchdogStartTime = 0;
    RfxTimer::stop(m_timer_handle_watchDog);
    m_timer_handle_watchDog = NULL;
    m_timer_handle_watchDog_started = false;
}

void RpIrStrategyOP095M::postponeNoServiceWatchdogIfNeeded() {
    logD(RFX_LOG_TAG, "postponeNoServiceWatchdogIfNeeded");
    if (m_timer_handle_watchDog_started) {
        RfxTimer::stop(m_timer_handle_watchDog);
        nsecs_t remainingTime = ms2ns(sWatchdogDelayTime) -
                             (systemTime() - mWatchdogStartTime);
        if (remainingTime < 0) {
            remainingTime = 0;
        }
        nsecs_t newDelay = remainingTime + ms2ns(sSwitchModeOrResumeDelayTime);
        logD(RFX_LOG_TAG, "remainingTime = %dms newDelay = %dms", (int)ns2ms(remainingTime), (int)ns2ms(newDelay));
        m_timer_handle_watchDog = RfxTimer::start(RfxCallback0(this, &RpIrStrategyOP095M::triggerNoServiceWatchdog), newDelay);
    }
}

void RpIrStrategyOP095M::restartNoSerivceWatchdogIfNeeded() {
    logD(RFX_LOG_TAG, "restartNoSerivceWatchdogIfNeeded");
    if (m_timer_handle_watchDog_started) {
        stopNoServiceWatchdog();
        if (mContinousRetryCount > 0) {
            mContinousRetryCount--; // restarting need revert count
        }
        startNoServiceWatchdog();
    }
}

/*****************************************************************************
 * IRpIrNwRatModeChangedListener implementation start
 *****************************************************************************/
void RpIrStrategyOP095M::onRatSwitchDone(int prePrefNwType, int curPrefNwType) {
    logD(RFX_LOG_TAG, "onRatSwitchDone, prePrefNwType=%d, curPrefNwType=%d", prePrefNwType, curPrefNwType);
    if (getIfEnabled() &&
        !isDualRadioOff() &&
        isDualServiceNotInService() &&
        prePrefNwType == PREF_NET_TYPE_LTE_ONLY &&
        curPrefNwType != PREF_NET_TYPE_LTE_ONLY &&
        m_IrController->getNwsMode() != NWS_MODE_CDMALTE) {
        // if turned off TDD DATA only mode, always search from Home mode
        logD(RFX_LOG_TAG, "force to switch to Home mode");
        switchForNoService(true);
        updateWatchdog();
    }
}
/*****************************************************************************
 * IRpIrNwRatModeChangedListener implementation end
 *****************************************************************************/
