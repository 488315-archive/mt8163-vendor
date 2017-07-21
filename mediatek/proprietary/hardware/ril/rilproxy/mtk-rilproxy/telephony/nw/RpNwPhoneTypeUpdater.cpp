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
#include "RpNwPhoneTypeUpdater.h"
#include "RpNwRatController.h"
#include "RpNwDefs.h"
#include "power/RadioConstants.h"

/*****************************************************************************
 * Class RfxController
 *****************************************************************************/
RFX_IMPLEMENT_CLASS("RpNwPhoneTypeUpdater", RpNwPhoneTypeUpdater, RfxController);

RpNwPhoneTypeUpdater::RpNwPhoneTypeUpdater() {
    RFX_LOG_D(RP_NW_PTU_TAG, " Constructor RpNwPhoneTypeUpdater");
    mLastDataRadioTech = RADIO_TECH_UNKNOWN;
}

RpNwPhoneTypeUpdater::~RpNwPhoneTypeUpdater() {
}

void RpNwPhoneTypeUpdater::onInit() {
    RfxController::onInit(); // Required: invoke super class implementation
    getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_SERVICE_STATE,
            RfxStatusChangeCallback(this, &RpNwPhoneTypeUpdater::onServiceStateChanged));

    getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_NWS_MODE,
            RfxStatusChangeCallback(this, &RpNwPhoneTypeUpdater::onNwsModeChanged));
    getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_VOICE_TYPE,
            RfxStatusChangeCallback(this, &RpNwPhoneTypeUpdater::onVoiceTypeChanged));
    getStatusManager()->registerStatusChanged(RFX_STATUS_KEY_DATA_TYPE,
            RfxStatusChangeCallback(this, &RpNwPhoneTypeUpdater::onDataTypeChanged));

    int mode = getStatusManager()->getIntValue(RFX_STATUS_KEY_NWS_MODE, NWS_MODE_CSFB);
    if (mode == NWS_MODE_CDMALTE) {
        getStatusManager()->setIntValue(RFX_STATUS_KEY_VOICE_TYPE, RADIO_TECH_GROUP_C2K);
        getStatusManager()->setIntValue(RFX_STATUS_KEY_DATA_TYPE, RADIO_TECH_GROUP_C2K);
    } else {
        getStatusManager()->setIntValue(RFX_STATUS_KEY_VOICE_TYPE, RADIO_TECH_GROUP_GSM);
        getStatusManager()->setIntValue(RFX_STATUS_KEY_DATA_TYPE, RADIO_TECH_GROUP_GSM);
    }
    RFX_LOG_D(RP_NW_PTU_TAG, " onInit() mode:%d", mode);
}

void RpNwPhoneTypeUpdater::onNwsModeChanged(RfxStatusKeyEnum key,
        RfxVariant oldValue, RfxVariant newValue) {
    int oldType = oldValue.asInt();
    int newType = newValue.asInt();
    int csType = RADIO_TECH_GROUP_GSM;
    if (newType == NWS_MODE_CDMALTE) {
        csType = RADIO_TECH_GROUP_C2K;
    } else if (newType == NWS_MODE_CSFB) {
        getStatusManager()->setIntValue(RFX_STATUS_KEY_DATA_TYPE, RADIO_TECH_GROUP_GSM);
    }
    getStatusManager()->setIntValue(RFX_STATUS_KEY_VOICE_TYPE, csType);
    RFX_LOG_D(RP_NW_PTU_TAG, " SIM%d NwsMode changed:%d, csType:%d", m_slot_id, newType, csType);
}

void RpNwPhoneTypeUpdater::onDataTypeChanged(RfxStatusKeyEnum key,
        RfxVariant oldValue, RfxVariant newValue) {
    int oldType = oldValue.asInt();
    int newType = newValue.asInt();
    RFX_LOG_D(RP_NW_PTU_TAG, " SIM%d Data type changed:%d -> %d", m_slot_id, oldType, newType);
}

void RpNwPhoneTypeUpdater::onVoiceTypeChanged(RfxStatusKeyEnum key,
        RfxVariant oldValue, RfxVariant newValue) {
    int oldType = oldValue.asInt();
    int newType = newValue.asInt();
    RFX_LOG_D(RP_NW_PTU_TAG, " SIM%d Voice type changed:%d -> %d", m_slot_id, oldType, newType);
}

void RpNwPhoneTypeUpdater::onServiceStateChanged(RfxStatusKeyEnum key,
        RfxVariant oldValue, RfxVariant newValue) {
    RfxNwServiceState oldSS = oldValue.asServiceState();
    RfxNwServiceState newSS = newValue.asServiceState();

    if (getNonSlotScopeStatusManager()->getIntValue(RFX_STATUS_KEY_CDMA_SOCKET_SLOT, 0)
            == m_slot_id) {
        RFX_LOG_D(RP_NW_PTU_TAG, " SIM%d onServiceStateChanged: newSS: %s",
                m_slot_id, newSS.toString().string());
        if (oldSS.getRilVoiceRegState() != newSS.getRilVoiceRegState()
                || oldSS.getRilVoiceRadioTech() != newSS.getRilVoiceRadioTech()) {
            onVoiceServiceStateChanged(oldSS, newSS);
        }
        if (oldSS.getRilDataRegState()!= newSS.getRilDataRegState()
                || oldSS.getRilDataRadioTech()!= newSS.getRilDataRadioTech()) {
            onDataServiceStateChanged(oldSS, newSS);
        }
    } else {
        getStatusManager()->setIntValue(RFX_STATUS_KEY_VOICE_TYPE, RADIO_TECH_GROUP_GSM);
        getStatusManager()->setIntValue(RFX_STATUS_KEY_DATA_TYPE, RADIO_TECH_GROUP_GSM);
    }
}

void RpNwPhoneTypeUpdater::onVoiceServiceStateChanged(RfxNwServiceState oldSS,
        RfxNwServiceState newSS) {
    int regState = newSS.getRilVoiceRegState();
    int radioTech = newSS.getRilVoiceRadioTech();
    int csType = RADIO_TECH_GROUP_GSM;

    RFX_LOG_D(RP_NW_PTU_TAG, " SIM%d Voice service state changed: regState=%d radioTech=%d",
            m_slot_id, regState, radioTech);
    if (RfxNwServiceState::isInService(regState)) {
        if (RfxNwServiceState::isGsmGroup(radioTech)) {
            // set PS phone type to GSM
            csType = RADIO_TECH_GROUP_GSM;
        } else if (RfxNwServiceState::isCdmaGroup(radioTech)) {
            // set PS phone type to C2K
            csType = RADIO_TECH_GROUP_C2K;
        } else {
            RFX_LOG_D(RP_NW_PTU_TAG, " Invalid rat");
        }
    } else {
        // OOS -> decide by device mode
        if (getDeviceMode() == NWS_MODE_CSFB) {
            // set PS phone type to GSM
            csType = RADIO_TECH_GROUP_GSM;
        } else if (getDeviceMode() == NWS_MODE_CDMALTE) {
            // set PS phone type to C2K
            csType = RADIO_TECH_GROUP_C2K;
        } else {
            RFX_LOG_D(RP_NW_PTU_TAG, " Invalid device mode");
        }
    }
    getStatusManager()->setIntValue(RFX_STATUS_KEY_VOICE_TYPE, csType);
    RFX_LOG_D(RP_NW_PTU_TAG, " Set voice type to %s", radioTechToString(csType));
}

void RpNwPhoneTypeUpdater::onDataServiceStateChanged(RfxNwServiceState oldSS,
        RfxNwServiceState newSS) {
    int regState = newSS.getRilDataRegState();
    int radioTech = newSS.getRilDataRadioTech();
    int psType = RADIO_TECH_GROUP_GSM;

    RFX_LOG_D(RP_NW_PTU_TAG, " SIM%d Data service state changed: regState=%d radioTech=%d",
            m_slot_id, regState, radioTech);
    if (RfxNwServiceState::isInService(regState)) {
        mLastDataRadioTech = radioTech;
        if (RfxNwServiceState::isCdmaGroup(radioTech)) {
            // set PS phone type to C2K
            psType = RADIO_TECH_GROUP_C2K;
        }
    } else {
        if (getDeviceMode() == NWS_MODE_CDMALTE) {
            int radioCapability = getStatusManager()->getIntValue(
                    RFX_STATUS_KEY_RADIO_CAPABILITY, RIL_CAPABILITY_NONE);
            if (radioCapability == RIL_CAPABILITY_CDMA_ONLY) {
                psType = RADIO_TECH_GROUP_C2K;
            } else {
                if (RfxNwServiceState::isCdmaGroup(mLastDataRadioTech)) {
                    psType = RADIO_TECH_GROUP_C2K;
                }
            }
        }
    }
    getStatusManager()->setIntValue(RFX_STATUS_KEY_DATA_TYPE, psType);
    RFX_LOG_D(RP_NW_PTU_TAG, " Set data type to %s", radioTechToString(psType));
}

int RpNwPhoneTypeUpdater::getDeviceMode() {
    int mode = getStatusManager()->getIntValue(RFX_STATUS_KEY_NWS_MODE, NWS_MODE_CSFB);
    RFX_LOG_D(RP_NW_PTU_TAG, " [getDeviceMode] mode=%d", mode);
    return mode;
}

char *RpNwPhoneTypeUpdater::radioTechToString(int radioTech) {
    switch (radioTech) {
        case RADIO_TECH_GROUP_GSM:
            return "GSM";
        case RADIO_TECH_GROUP_C2K:
            return "C2K";
        default:
            return "INVALID RADIO TECH";
    }
}
