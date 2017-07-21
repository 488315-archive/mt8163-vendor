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

#include <assert.h>
#include <cutils/properties.h>
#include <stdlib.h>
#include <string.h>
#include "at_tok.h"
#include "nw/RpNwRatController.h"
#include "power/RpModemController.h"
#include "RfxAction.h"
#include "RfxLog.h"
#include "RpCardModeControllerFactory.h"
#include "RpCardTypeReadyController.h"
#include "RpCdmaLteModeController.h"
#include "utils/String8.h"
#include "power/RadioConstants.h"
#include "RfxTimer.h"
#include "sim/RpSimController.h"
#include "RfxRootController.h"

using ::android::String8;

/*****************************************************************************
 * Class RpCdmaLteModeController
 *****************************************************************************/

RFX_IMPLEMENT_CLASS("RpCdmaLteModeController", RpCdmaLteModeController, RfxController);

int RpCdmaLteModeController::sCdmaSocketSlotId = getCdmaSocketSlotId();

RpCdmaLteModeController::RpCdmaLteModeController() :
    mInSwitching(false),
    m_pending_card_types(NULL),
    m_pending_card_states(NULL),
    mSwitchingCardType(false) {

    m_card_types = new int[RFX_SLOT_COUNT];
    m_card_states = new int[RFX_SLOT_COUNT];
    mSlotBeSwitchCardType = new int[RFX_SLOT_COUNT];
    m_card_modes = new int[RFX_SLOT_COUNT];
    m_old_card_modes = new int[RFX_SLOT_COUNT];

    mCdmaLteModeSlotId = getActiveCdmaLteModeSlotId();
    for (int slotId = SIM_ID_1; slotId < RFX_SLOT_COUNT; slotId++) {
        m_card_types[slotId] = CARD_TYPE_INVALID;
        m_card_states[slotId] = CARD_STATE_INVALID;
        mSlotBeSwitchCardType[slotId] = -1;
        if (slotId == mCdmaLteModeSlotId) {
            m_card_modes[mCdmaLteModeSlotId] = RADIO_TECH_MODE_CDMALTE;
            m_old_card_modes[mCdmaLteModeSlotId] = RADIO_TECH_MODE_CDMALTE;
        } else {
            m_card_modes[slotId] = RADIO_TECH_MODE_CSFB;
            m_old_card_modes[slotId] = RADIO_TECH_MODE_CSFB;
        }
    }

    mCardTypeReadyController = RFX_OBJ_GET_INSTANCE(RpCardTypeReadyController);
}

RpCdmaLteModeController::~RpCdmaLteModeController() {
    if (m_card_types != NULL) {
        delete[] m_card_types;
        m_card_types = NULL;
    }
    if (m_card_states != NULL) {
        delete[] m_card_states;
        m_card_states = NULL;
    }
    if (m_card_modes != NULL) {
        delete[] m_card_modes;
        m_card_modes = NULL;
    }
    if (m_old_card_modes != NULL) {
        delete[] m_old_card_modes;
        m_old_card_modes = NULL;
    }
    if (m_pending_card_types != NULL) {
        delete[] m_pending_card_types;
        m_pending_card_types = NULL;
    }
    if (m_pending_card_states != NULL) {
        delete[] m_pending_card_states;
        m_pending_card_states = NULL;
    }
    if (mSlotBeSwitchCardType != NULL) {
        delete[] mSlotBeSwitchCardType;
        mSlotBeSwitchCardType = NULL;
    }
    RFX_OBJ_CLOSE_INSTANCE(RpCardTypeReadyController);
}

void RpCdmaLteModeController::onInit() {
    RfxController::onInit();
}

void RpCdmaLteModeController::onDeinit() {
    mSwitchQueue.clear();

    RfxController::onDeinit();
}

void RpCdmaLteModeController::onC2kSocketConnected(int slotId, RfxStatusKeyEnum key,
        RfxVariant old_value, RfxVariant value) {
    RfxSocketState newSocketState = value.asSocketState();
    RfxSocketState oldSocketState = old_value.asSocketState();

    RFX_LOG_D(RP_CDMALTE_MODE_TAG, "[SMC][onC2kSocketConnected] Slot %d, SocketState : %s -> %s",
            slotId, oldSocketState.toString().string(), newSocketState.toString().string());
    if (newSocketState.getSocketState(newSocketState.SOCKET_C2K)
            && !oldSocketState.getSocketState(oldSocketState.SOCKET_C2K)) {
        getStatusManager(slotId)->unRegisterStatusChangedEx(RFX_STATUS_KEY_SOCKET_STATE,
                RfxStatusChangeCallbackEx(this, &RpCdmaLteModeController::onC2kSocketConnected));
        doSwitchRadioTechImpl();
    }
}

void RpCdmaLteModeController::onCardTypeReady(int* card_types, int* card_state, int slotNum,
    CardTypeReadyReason ctrr) {
    assert (slotNum != RFX_SLOT_COUNT);
    for (int i = 0; i < slotNum; i ++) {
        RFX_LOG_D(RP_CDMALTE_MODE_TAG,
                "[SMC][onC2kCardTypeReady] Slot:%d, card type:%d, card state:%d",
                i, card_types[i], card_state[i]);
    }

    char tempstr[PROPERTY_VALUE_MAX];
    memset(tempstr, 0, sizeof(tempstr));
    property_get("ril.cdma.switching", tempstr, "0");
    int simSwitching = atoi(tempstr);
    if (simSwitching == 1) {
        RFX_LOG_D(RP_CDMALTE_MODE_TAG, "[SMC][onC2kCardTypeReady] SimSwitch on going, just return");
        return;
    }
    if (ctrr == CARD_TYPE_READY_REASON_BOOTUP_OR_RIL_RESET) {
        RFX_LOG_D(RP_CDMALTE_MODE_TAG, "[SMC][onC2kCardTypeReady] set mInTriggeringTrm= false");
        mInTriggeringTrm = false;
    }
    if (mInTriggeringTrm) {
        RFX_LOG_D(RP_CDMALTE_MODE_TAG,
            "[SMC][onC2kCardTypeReady] Just return without switching mode, as mInTriggeringTrm = true");
        return;
    }
    if (mInSwitching) {
        RFX_LOG_D(RP_CDMALTE_MODE_TAG, "[SMC][onC2kCardTypeReady] Switching now, pended");
        if (m_pending_card_types == NULL) {
            m_pending_card_types = new int[slotNum];
        }
        if (m_pending_card_states == NULL) {
            m_pending_card_states = new int[slotNum];
        }
        for (int slotId = SIM_ID_1; slotId < slotNum; slotId++) {
            m_pending_card_types[slotId] = card_types[slotId];
            m_pending_card_states[slotId] = card_state[slotId];
        }
        mPendingCardTypeReadyReason = ctrr;
    } else {
        RFX_LOG_D(RP_CDMALTE_MODE_TAG, "[SMC][onC2kCardTypeReady] Start switch");
        for (int slotId = SIM_ID_1; slotId < slotNum; slotId++) {
            m_card_types[slotId] = card_types[slotId];
            m_card_states[slotId] = card_state[slotId];
            m_old_card_modes[slotId] = m_card_modes[slotId];
        }
        calculateCardMode();
        int old_cdma_socket_slot = sCdmaSocketSlotId;
        if (tryToTriggerTrm(slotNum, old_cdma_socket_slot, ctrr)) {
            return;
        }
        if (needSwtichCardType()) {
            RFX_LOG_D(RP_CDMALTE_MODE_TAG, "[SMC][onC2kCardTypeReady] switchcardType and return");
            return;
        }
        setupCdmaLteMode();
        setupSwitchQueue(old_cdma_socket_slot);
        doSwitchRadioTech();
    }
}

bool RpCdmaLteModeController::tryToTriggerTrm(int slotNum, int old_cdma_socket_slot, CardTypeReadyReason ctrr) {
    int new_cdma_socket_slot = calculateNewCdmaSlotId();
    RFX_LOG_D(RP_CDMALTE_MODE_TAG,
        "[SMC][tryToTriggerTrm] oldCdmaSocketSlot:%d, newCdmaSocketSlot:%d, reason:%d",
        old_cdma_socket_slot, new_cdma_socket_slot, ctrr);
    if (ctrr != CARD_TYPE_READY_REASON_BOOTUP_OR_RIL_RESET
        && new_cdma_socket_slot != CSFB_ON_SLOT
        && old_cdma_socket_slot != new_cdma_socket_slot) {
        if (mSwitchingCardType) {
            mSwitchingCardType = false;
            RFX_LOG_D(RP_CDMALTE_MODE_TAG,
                "[SMC][tryToTriggerTrm] just return without trigger trm , as mSwitchingCardType = true");
            return false;
        }
        triggerTrm();
        for (int slotId = SIM_ID_1; slotId < slotNum; slotId++) {
            m_card_modes[slotId] = m_old_card_modes[slotId];
        }
        return true;
    }
    return false;
}

void RpCdmaLteModeController::calculateCardMode() {
    RpCardModeControllerFactory::getRpCardModeController()->calculateCardMode(m_card_types, m_old_card_modes,
        m_card_modes, mSlotBeSwitchCardType, RFX_SLOT_COUNT);
}

void RpCdmaLteModeController::configModemStatus() {
    RFX_LOG_D(RP_CDMALTE_MODE_TAG, "[SMC][configModemStatus] cdmaSlot : %d", sCdmaSocketSlotId);
    RpModemController::getInstance() -> configModemStatus(sCdmaSocketSlotId);
}
int RpCdmaLteModeController::getCurrentNetworkType(int slotId) {
    char tempstr[PROPERTY_VALUE_MAX];
    int netWorkType = -1;

    memset(tempstr, 0, sizeof(tempstr));
    property_get("ro.mtk_lte_support", tempstr, "0");
    int isLteSupport = atoi(tempstr);

    memset(tempstr, 0, sizeof(tempstr));
    property_get("persist.radio.simswitch", tempstr, "1");
    int  capabilitySlotId  = atoi(tempstr) - 1;
    if (getCCardCount() == 0 && isCt3GDualMode(slotId)) {
        netWorkType = PREF_NET_TYPE_CDMA_EVDO_AUTO;
    } else if (m_card_modes[slotId] == RADIO_TECH_MODE_CDMALTE) {
        //check whether L+C support first
        memset(tempstr, 0, sizeof(tempstr));
        property_get("ro.operator.optr", tempstr, "");
        int isOP01Support = (strcmp(tempstr, "OP01") == 0) ? 1 : 0;

        memset(tempstr, 0, sizeof(tempstr));
        property_get("ro.mtk_c2k_support", tempstr, "0");
        int  isC2KSupport  = atoi(tempstr);

        if (isOP01Support && isC2KSupport) {
            netWorkType = PREF_NET_TYPE_CDMA_EVDO_AUTO;
        } else {
            if (isLteSupport && slotId == capabilitySlotId && m_card_types[slotId] != CARD_TYPE_RUIM) {
                netWorkType = PREF_NET_TYPE_LTE_CDMA_EVDO;
            } else {
                netWorkType = PREF_NET_TYPE_CDMA_EVDO_AUTO;
            }
        }
    } else if (m_card_modes[slotId] == RADIO_TECH_MODE_CSFB) {
        if (slotId == capabilitySlotId) {
            if (isLteSupport) {
                netWorkType = PREF_NET_TYPE_LTE_GSM_WCDMA;
            } else {
                netWorkType = PREF_NET_TYPE_GSM_WCDMA;
            }
        } else {
            netWorkType = PREF_NET_TYPE_GSM_ONLY;
        }
    } else {
        assert(0);
    }
   RFX_LOG_D(RP_CDMALTE_MODE_TAG, "[SMC][getCurrentNetworkType]isLteSupport: %d, capabilitySlotId: %d, netWorkType: %d",
           isLteSupport, capabilitySlotId, netWorkType);
   return netWorkType;
}

int RpCdmaLteModeController::getCCardCount() {
    int cCardNum = 0;
    char tempstr[PROPERTY_VALUE_MAX];
    for (int slotId = 0; slotId < RFX_SLOT_COUNT; slotId++) {
        memset(tempstr, 0, sizeof(tempstr));
        property_get(PROPERTY_RIL_FULL_UICC_TYPE[slotId], tempstr, "");
        if ((strstr(tempstr, "CSIM") != NULL) || (strstr(tempstr, "RUIM") != NULL)) {
            cCardNum++;
        }
    }
    return cCardNum;
}

int RpCdmaLteModeController::getFirstCCardSlot() {
    char tempstr[PROPERTY_VALUE_MAX];
    for (int slotId = 0; slotId < RFX_SLOT_COUNT; slotId++) {
        getUiccType(slotId, tempstr);
        if (isCdmaCard(tempstr)) {
            return slotId;
        }
    }
    return -1;
}

bool RpCdmaLteModeController::isCt3GDualMode(int slotId) {
    bool isCt3G = RFX_OBJ_GET_INSTANCE(RfxRootController)->getStatusManager(
            slotId)->getBoolValue(RFX_STATUS_KEY_CT3G_DUALMODE_CARD, false);
    RFX_LOG_D(RP_CDMALTE_MODE_TAG, "[SMC]isCt3GDualMode(%d):%d", slotId, isCt3G);
    return isCt3G;
}

bool RpCdmaLteModeController::isCdmaCard(char* uicc_type) {
    if ((strstr(uicc_type, "CSIM") != NULL) || (strstr(uicc_type, "RUIM") != NULL)) {
        return true;
    }
    return false;
}

void RpCdmaLteModeController::getUiccType(int slotId, char* uiccType) {
     memset(uiccType, 0, sizeof(uiccType));
     property_get(PROPERTY_RIL_FULL_UICC_TYPE[slotId], uiccType, "");
}

int RpCdmaLteModeController::getDefaultNetworkType(int slotId) {
    char tempstr[PROPERTY_VALUE_MAX];
    int netWorkType = -1;

    memset(tempstr, 0, sizeof(tempstr));
    property_get("ro.mtk_lte_support", tempstr, "0");
    int isLteSupport = atoi(tempstr);

    memset(tempstr, 0, sizeof(tempstr));
    property_get("persist.radio.simswitch", tempstr, "1");
    int  capabilitySlotId  = atoi(tempstr) - 1;

    //check whether L+C CMCC support first
    memset(tempstr, 0, sizeof(tempstr));
    property_get("ro.operator.optr", tempstr, "");
    int isOP01Support = (strcmp(tempstr, "OP01") == 0) ? 1 : 0;

    memset(tempstr, 0, sizeof(tempstr));
    property_get("ro.mtk_c2k_support", tempstr, "0");
    int  isC2KSupport  = atoi(tempstr);

    memset(tempstr, 0, sizeof(tempstr));
    property_get(PROPERTY_RIL_FULL_UICC_TYPE[slotId], tempstr, "");

    int cCardNum = getCCardCount();
    if (cCardNum == 0 && isCt3GDualMode(slotId)) {
        netWorkType = PREF_NET_TYPE_CDMA_EVDO_AUTO;
    } else if ((strstr(tempstr, "CSIM") != NULL)|| (strstr(tempstr, "RUIM") != NULL)) {
        if (slotId == capabilitySlotId) {
            if (isOP01Support && isC2KSupport) {
                netWorkType = PREF_NET_TYPE_CDMA_EVDO_AUTO;
            } else {
                if (isLteSupport && strncmp(tempstr, "RUIM", strlen(tempstr)) != 0) {
                    netWorkType = PREF_NET_TYPE_LTE_CDMA_EVDO;
                } else {
                    netWorkType = PREF_NET_TYPE_CDMA_EVDO_AUTO;
                }
            }
        } else if (cCardNum > 1) {
            char tempUiccTypestr[PROPERTY_VALUE_MAX];
            getUiccType(capabilitySlotId, tempUiccTypestr);
            if (isCdmaCard(tempUiccTypestr)) {
                netWorkType = PREF_NET_TYPE_GSM_ONLY;
            } else {
                int firstCSlotId = getFirstCCardSlot();
                if (firstCSlotId == slotId) {
                    netWorkType = PREF_NET_TYPE_CDMA_EVDO_AUTO;
                } else {
                    netWorkType = PREF_NET_TYPE_GSM_ONLY;
                }
            }
        } else if (cCardNum == 1) {
            netWorkType = PREF_NET_TYPE_CDMA_EVDO_AUTO;
        }
    } else if ((strncmp(tempstr, "SIM", 3) == 0) || (strstr(tempstr, "USIM") != NULL)) {
        if (slotId == capabilitySlotId) {
            if (isLteSupport) {
                netWorkType = PREF_NET_TYPE_LTE_GSM_WCDMA;
            } else {
                netWorkType = PREF_NET_TYPE_GSM_WCDMA;
            }
        } else {
            netWorkType = PREF_NET_TYPE_GSM_ONLY;
        }
    } else {
        RFX_LOG_E(RP_CDMALTE_MODE_TAG, "[getDefaultNetworkType] None slot!");
        netWorkType = PREF_NET_TYPE_GSM_ONLY;
    }

   RFX_LOG_D(RP_CDMALTE_MODE_TAG, "[getDefaultNetworkType] cardType: %s, netWorkType: %d",
           tempstr, netWorkType);
   return netWorkType;
}

void RpCdmaLteModeController::setupSwitchQueue(int old_cdma_socket_slot) {
    int firstSwitchSlot = SIM_ID_1;
    int secondSwitchSlot = SIM_ID_2;

    if (old_cdma_socket_slot != sCdmaSocketSlotId) {
        firstSwitchSlot = old_cdma_socket_slot;
        secondSwitchSlot = sCdmaSocketSlotId;
    }

    mSwitchQueue.clear();
    mSwitchQueue.add(firstSwitchSlot);
    mSwitchQueue.add(secondSwitchSlot);

    if (RFX_SLOT_COUNT > 2) {
        for (int i = 0; i < RFX_SLOT_COUNT; i++) {
            if (i != firstSwitchSlot && i != secondSwitchSlot) {
                mSwitchQueue.add(i);
            }
        }
    }
    assert(mSwitchQueue.size() > 0 && mSwitchQueue.size() <= RFX_SLOT_COUNT);
}

bool RpCdmaLteModeController::needSwtichCardType() {
    // For C+C. just switchCardType and return.
    bool hasSwtiched = false;
    mSwitchingCardType = false;
    if (mSlotBeSwitchCardType != NULL) {
        for (int index = 0; index < RFX_SLOT_COUNT; index++) {
            if (mSlotBeSwitchCardType[index] > -1) {
                RFX_LOG_D(RP_CDMALTE_MODE_TAG,
                    "[SMC][needSwtichCardType] Just return. AS slot: %d switchCardType to sim.",
                    index);
                RpSimController *simController =
                    (RpSimController*)findController(index,
                    RFX_OBJ_CLASS_INFO(RpSimController));
                simController->switchCardType(0);
                hasSwtiched = true;
                mSwitchingCardType = true;
            }
        }
        if (hasSwtiched) {
            for (int slotId = SIM_ID_1; slotId < RFX_SLOT_COUNT; slotId++) {
                mSlotBeSwitchCardType[slotId] = -1;
                m_card_modes[slotId] = m_old_card_modes[slotId];
            }
            mInSwitching = false;
            RFX_LOG_D(RP_CDMALTE_MODE_TAG,
                "[SMC][needSwtichCardType] hasSwitched: %d", hasSwtiched);
            return hasSwtiched;
        }
    }
    RFX_LOG_D(RP_CDMALTE_MODE_TAG,
        "[SMC][needSwtichCardType] hasSwitched: %d", hasSwtiched);
    return hasSwtiched;
}

void RpCdmaLteModeController::doSwitchRadioTech() {
    startSwitchMode();
    RfxSocketState socketState=
            getStatusManager(sCdmaSocketSlotId)->getSocketStateValue(RFX_STATUS_KEY_SOCKET_STATE);
    RFX_LOG_D(RP_CDMALTE_MODE_TAG,
            "[SMC][doSwitchRadioTech] cdmaSocketSlotId : %d,  socketState : %s",
            sCdmaSocketSlotId, socketState.toString().string());
    if (!socketState.getSocketState(socketState.SOCKET_C2K)) {
        getStatusManager(sCdmaSocketSlotId)->registerStatusChangedEx(RFX_STATUS_KEY_SOCKET_STATE,
                RfxStatusChangeCallbackEx(this, &RpCdmaLteModeController::onC2kSocketConnected));
        RFX_LOG_D(RP_CDMALTE_MODE_TAG,
                "[SMC][doSwitchRadioTech] slot %d wait onC2kSocketConnected", sCdmaSocketSlotId);
        return;
    }

    doSwitchRadioTechImpl();
}

void RpCdmaLteModeController::doSwitchRadioTechImpl() {
    int switchSlot = mSwitchQueue.itemAt(0);
    RFX_LOG_D(RP_CDMALTE_MODE_TAG,
            "[SMC][doSwitchRadioTechImpl] Start switch Slot: %d firstly, mSwitchQueue size : %d",
            switchSlot, mSwitchQueue.size());
    mSwitchQueue.removeAt(0);

    configModemStatus();
    switchRadioTechnology(switchSlot);
}
void RpCdmaLteModeController::onRatSwitchDone(int slotId) {
    RFX_LOG_D(RP_CDMALTE_MODE_TAG, "[SMC][onRatSwitchDone] Slot : %d switch done, mSwitchQueue size : %d",
            slotId, mSwitchQueue.size());
    if (mSwitchQueue.isEmpty()) {
        finishSwitchMode();
     } else if (mSwitchQueue.size() >= 1) {
         int switchSlot = mSwitchQueue.itemAt(0);
         RFX_LOG_D(RP_CDMALTE_MODE_TAG, "[SMC][onRatSwitchDone] Start switch Slot: %d", switchSlot);
         mSwitchQueue.removeAt(0);
         switchRadioTechnology(switchSlot);
    }
}

void RpCdmaLteModeController::switchRadioTechnology(int slotId) {
    sp<RfxAction> action = new RfxAction1<int>(this, &RpCdmaLteModeController::onRatSwitchDone, slotId);
    AppFamilyType app_family_type = getAppFamilyType(slotId);

    int networkType = getCurrentNetworkType(slotId);

    RFX_LOG_D(RP_CDMALTE_MODE_TAG,
            "[SMC][switchRadioTechnology] SlotId : %d, %s -> %s, app_family_type : %d, \
            networkType : %d, card_state: %d",
            slotId,
            m_old_card_modes[slotId] == 3 ? "CDMALTE" : "CSFB",
            m_card_modes[slotId] == 3 ? "CDMALTE" : "CSFB",
            app_family_type, networkType, m_card_states[slotId]);
    RpNwRatController* nwRatController =
            (RpNwRatController *)findController(slotId, RFX_OBJ_CLASS_INFO(RpNwRatController));

    if (app_family_type == APP_FAM_3GPP2 && networkType == PREF_NET_TYPE_GSM_ONLY) {
        app_family_type = APP_FAM_3GPP;
        RFX_LOG_D(RP_CDMALTE_MODE_TAG,
            "[SMC][switchRadioTechnology] change app_family_type to APP_FAM_3GPP");
    }
    nwRatController->setPreferredNetworkType(app_family_type, networkType,
            m_card_states[slotId], action);
}

/**
 * Get the SVLTE slot id.
 * @return SVLTE slot id.
 *         0 : svlte in slot1
 *         1 : svlte in slot2
 */
int RpCdmaLteModeController::getActiveCdmaLteModeSlotId() {
    int i = 0;
    char tempstr[PROPERTY_VALUE_MAX];
    char* tok = NULL;

    memset(tempstr, 0, sizeof(tempstr));
    // 3 means SVLTE mode, 2 is CSFB mode in this persist.
    property_get("persist.radio.svlte_slot", tempstr, "3,2");
    tok = strtok(tempstr, ",");
    while (tok != NULL) {
        if (3 == atoi(tok)) {
            RFX_LOG_D(RP_CDMALTE_MODE_TAG, "getActiveCdmaLteModeSlotId : %d", i);
            return i;
        }
        i++;
        tok = strtok(NULL, ",");
    }
    RFX_LOG_D(RP_CDMALTE_MODE_TAG, "getActiveCdmaLteModeSlotId : -1");
    return CSFB_ON_SLOT;
}

/**
 * Get slot id which connect to c2k rild socket.
 * @return slot id which connect to c2k rild socket
 *         0 : slot1 connect to c2k rild socket
 *         1 : slot2 connect to c2k rild socket
 */
int RpCdmaLteModeController::getCdmaSocketSlotId() {
    int ret = 0;
    char tempstr[PROPERTY_VALUE_MAX] = { 0 };

    memset(tempstr, 0, sizeof(tempstr));
    property_get("persist.radio.cdma_slot", tempstr, "1");

    ret = atoi(tempstr) - 1;
    RFX_LOG_D(RP_CDMALTE_MODE_TAG, "getCdmaSocketSlotId : %d", ret);
    return ret;
}

void RpCdmaLteModeController::setCdmaSocketSlotId(int slotId) {
    RFX_LOG_D(RP_CDMALTE_MODE_TAG, "[SMC]setCdmaSocketSlotId : %d -> %d", sCdmaSocketSlotId, slotId);
    if (slotId >= 0 && slotId < RFX_SLOT_COUNT && sCdmaSocketSlotId != slotId) {
        sCdmaSocketSlotId = slotId;
        char* slot_str = NULL;
        asprintf(&slot_str, "%d", slotId + 1);
        property_set("persist.radio.cdma_slot", slot_str);
        free(slot_str);
    }
}

int RpCdmaLteModeController::calculateNewCdmaSlotId() {
    int cdmaLteModeSlotId = CSFB_ON_SLOT;
    int inserCount = 0;
    int cardSlot = -1;
    /** M: If there is only one ct3Gdual Card, set it as new cdma slot.
     *  Reason: When CT 4G + CT3GDaul,  CT 4G has the 3/4g capability. If you plugout the CT 4G card,
     *  should set C capability to CT 3G Dual.
     */
    for (int i= 0; i< RFX_SLOT_COUNT; i++) {
        if (m_card_types[i] != CARD_TYPE_NONE) {
            inserCount++;
            cardSlot = i;
        }
    }

    if (inserCount == 1 && isCt3GDualMode(cardSlot)) {
        cdmaLteModeSlotId = cardSlot;
    } else {
        for (int i = 0; i < RFX_SLOT_COUNT; i++) {
            if (m_card_modes[i] == RADIO_TECH_MODE_CDMALTE) {
                cdmaLteModeSlotId = i;
                break;
            }
        }
    }
    RFX_LOG_D(RP_CDMALTE_MODE_TAG, "[SMC]calculateNewCdmaSlotId : %d", cdmaLteModeSlotId);
    return cdmaLteModeSlotId;
}

void RpCdmaLteModeController::setActiveCdmaLteModeSlotId() {
    int cdmaLteModeSlotId = CSFB_ON_SLOT;
    for (int i = 0;i < RFX_SLOT_COUNT; i++) {
        if (m_card_modes[i] == RADIO_TECH_MODE_CDMALTE) {
            cdmaLteModeSlotId = i;
            break;
        }
    }

    RFX_LOG_D(RP_CDMALTE_MODE_TAG, "[SMC]setActiveCdmaLteModeSlotId : %d -> %d",
            mCdmaLteModeSlotId, cdmaLteModeSlotId);
    if (mCdmaLteModeSlotId != cdmaLteModeSlotId) {
        mCdmaLteModeSlotId = cdmaLteModeSlotId;
        String8 cardmodestr;
        cardmodestr.clear();
        for (int i = 0;i < RFX_SLOT_COUNT - 1; i++) {
            cardmodestr.appendFormat("%d,", m_card_modes[i]);
        }
        cardmodestr.appendFormat("%d", m_card_modes[RFX_SLOT_COUNT - 1]);
        property_set("persist.radio.svlte_slot", cardmodestr.string());
        RFX_LOG_D(RP_CDMALTE_MODE_TAG, "setupCdmaLteMode : %s", cardmodestr.string());
    }
}

void RpCdmaLteModeController::setupCdmaLteMode() {
    setActiveCdmaLteModeSlotId();
    getNonSlotScopeStatusManager()->setIntValue(
            RFX_STATUS_KEY_ACTIVE_CDMALTE_MODE_SLOT, mCdmaLteModeSlotId);

    setCdmaSocketSlotId(mCdmaLteModeSlotId > -1 ? mCdmaLteModeSlotId : sCdmaSocketSlotId);
    getNonSlotScopeStatusManager()->setIntValue(RFX_STATUS_KEY_CDMA_SOCKET_SLOT, sCdmaSocketSlotId);
}

void RpCdmaLteModeController::startSwitchMode() {
    RFX_LOG_D(RP_CDMALTE_MODE_TAG, "[SMC]startSwitchMode");
    mInSwitching = true;
    getNonSlotScopeStatusManager()->setIntValue(RFX_STATUS_KEY_MODE_SWITCH, MODE_SWITCH_START);
}

void RpCdmaLteModeController::finishSwitchMode() {
    RFX_LOG_D(RP_CDMALTE_MODE_TAG, "[SMC]finishSwitchMode");
    mInSwitching = false;
    getNonSlotScopeStatusManager()->setIntValue(RFX_STATUS_KEY_MODE_SWITCH, MODE_SWITCH_FINISH);

    if (m_pending_card_types != NULL) {
        RFX_LOG_D(RP_CDMALTE_MODE_TAG, "[SMC]Start switch mode pended");

        char tempstr[PROPERTY_VALUE_MAX];
        memset(tempstr, 0, sizeof(tempstr));
        property_get("ril.cdma.switching", tempstr, "0");
        int simSwitching = atoi(tempstr);
        if (simSwitching == 1) {
            RFX_LOG_D(RP_CDMALTE_MODE_TAG, "[SMC] SimSwitch on going, just return");
            delete[] m_pending_card_types;
            m_pending_card_types = NULL;
            delete[] m_pending_card_states;
            m_pending_card_states = NULL;
            return;
        }

        for (int slotId = SIM_ID_1; slotId <RFX_SLOT_COUNT; slotId++) {
            m_card_types[slotId] = m_pending_card_types[slotId];
            m_old_card_modes[slotId] = m_card_modes[slotId];
            m_card_states[slotId] = m_pending_card_states[slotId];
        }
        delete[] m_pending_card_types;
        m_pending_card_types = NULL;
        delete[] m_pending_card_states;
        m_pending_card_states = NULL;

        calculateCardMode();
        int old_cdma_socket_slot = sCdmaSocketSlotId;
        if (tryToTriggerTrm(RFX_SLOT_COUNT, old_cdma_socket_slot, mPendingCardTypeReadyReason)) {
            return;
        }
        if(needSwtichCardType()) {
            RFX_LOG_D(RP_CDMALTE_MODE_TAG,
                "[SMC][onC2kCardTypeReady] Pended: switchcardType and return");
            return;
        }
        setupCdmaLteMode();
        setupSwitchQueue(old_cdma_socket_slot);
        doSwitchRadioTech();
    }
}

bool RpCdmaLteModeController::is4GCdmaCard(int cardType) {
    if (containsUsim(cardType) && containsCdma(cardType)) {
        return true;
    }
    return false;
}

bool RpCdmaLteModeController::containsCdma(int cardType) {
     if ((cardType & CARD_TYPE_RUIM) > 0 ||
         (cardType & CARD_TYPE_CSIM) > 0) {
         return true;
     }
     return false;
}

bool RpCdmaLteModeController::containsGsm(int cardType) {
     if ((cardType & CARD_TYPE_SIM) > 0 ||
         (cardType & CARD_TYPE_USIM) > 0) {
         return true;
     }
     return false;
}

bool RpCdmaLteModeController::containsUsim(int cardType) {
     if ((cardType & CARD_TYPE_USIM) > 0) {
         return true;
     }
     return false;
}

bool RpCdmaLteModeController::isUsimOnlyCard(int cardType) {
      return (containsUsim(cardType) && !containsCdma(cardType));
}

AppFamilyType RpCdmaLteModeController::getAppFamilyType(int slotId) {
    if (containsCdma(m_card_types[slotId]) || (getCCardCount() == 0 && isCt3GDualMode(slotId))) {
        return APP_FAM_3GPP2;
    } else {
        return APP_FAM_3GPP;
    }
}

void RpCdmaLteModeController::triggerTrm() {
    mInTriggeringTrm = true;
    RFX_LOG_D(RP_CDMALTE_MODE_TAG, "[SMC][triggerTrm] trigger trm");
    property_set("ril.cdma.report.case", "1");
    property_set("ril.mux.report.case", "2");
    property_set("ril.muxreport", "1");
}
