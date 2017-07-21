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

/************************************************************************
 * Include
 ************************************************************************/
#include <cutils/properties.h>
#include <mutex>
#include "RfxLog.h"
#include "RfxRootController.h"
#include "RpCdmaLteDefs.h"
#include "RpOMCardModeController.h"

/************************************************************************
 * Class RpOMCardModeController
 ************************************************************************/
std::mutex mtxx;
RpOMCardModeController* RpOMCardModeController::s_RpOMCardModeController;

RpOMCardModeController::RpOMCardModeController() {
}

RpOMCardModeController::~RpOMCardModeController() {
}

RpOMCardModeController* RpOMCardModeController::getInstance() {
    if (s_RpOMCardModeController == NULL) {
        mtxx.lock();
        if (s_RpOMCardModeController == NULL) {
            s_RpOMCardModeController = new RpOMCardModeController();
        }
        mtxx.unlock();
    }
    return s_RpOMCardModeController;
}

int RpOMCardModeController::getPreferRadioTech(int cardType, int slot,
    int* oldCardModes) {
    int prefer = RADIO_TECH_MODE_UNKNOWN;
    if ((cardType & CARD_TYPE_RUIM) > 0 ||
            (cardType & CARD_TYPE_CSIM) > 0) {
        prefer = RADIO_TECH_MODE_CDMALTE;
    } else if (cardType == CARD_TYPE_NONE) {
        prefer = oldCardModes[slot];
    } else {
        prefer = RADIO_TECH_MODE_CSFB;
    }
    RFX_LOG_D(RP_CDMALTE_MODE_TAG,
            "[SMC]getPreferRadioTech; cartType:%d, preferNetWorkType: %d", cardType, prefer);
    return prefer;
}

bool RpOMCardModeController::isCt3GDualMode(int slotId) {
    bool isCt3G = RFX_OBJ_GET_INSTANCE(RfxRootController)->getStatusManager(
            slotId)->getBoolValue(RFX_STATUS_KEY_CT3G_DUALMODE_CARD, false);
    RFX_LOG_D(RP_CDMALTE_MODE_TAG, "[SMC]isCt3GDualMode(%d):%d", slotId, isCt3G);
    return isCt3G;
}

void RpOMCardModeController::calculateCardMode(int* cardTypes, int* oldCardModes,
        int* newCardModes, int* needSwitchCardTypeSlots, int slotNum) {

    RFX_LOG_D(RP_CDMALTE_MODE_TAG,"[SMC][calculateCardMode] >>> OPERATOR_OM case");
    for (int i = 0; i < slotNum; i++) {
        needSwitchCardTypeSlots[i] = -1;
    }

    int* slotPreferRadioTec = new int[slotNum];
    /// M: how many cards been inserted.
    int insertCount = 0;
    /// M: How many cards' mode been set to cdmalte.
    int cdmaLteCount = 0;
    /// M: How man cards' mode been set to csfb.
    int csfbCount = 0;
    int mode = -1;
    int cardType = CARD_TYPE_NONE;
    for (int index = 0; index < slotNum; index ++) {
        cardType = cardTypes[index];
        if (cardType != CARD_TYPE_NONE) {
            insertCount++;
        }
        mode = getPreferRadioTech(cardType, index, oldCardModes);
        if (mode == RADIO_TECH_MODE_CDMALTE) {
            cdmaLteCount++;
        } else if (mode == RADIO_TECH_MODE_CSFB) {
            csfbCount++;
        }
       slotPreferRadioTec[index] = mode;
       newCardModes[index] = mode;
    }

    char property_value[PROPERTY_VALUE_MAX] = { 0 };
    /// M: Get capability Slot.
    property_get("persist.radio.simswitch", property_value, "1");
    int capabilitySlot = atoi(property_value) - 1;

    /**
     * M: This case for CT4G+CT3G Dual. If ct3g card has been changed to Gsm card
     * and this ct3g card has 3/4 G capability, should set other ct cards' card mode to csfb.
     */
    if (cdmaLteCount == 1 && insertCount > 1) {
        int cSlot = -1;
        for (int index = 0; index < slotNum; index ++) {
            mode = slotPreferRadioTec[index];
            if (mode == RADIO_TECH_MODE_CDMALTE) {
                cSlot = index;
                if (cSlot != capabilitySlot && isCt3GDualMode(capabilitySlot)) {
                    newCardModes[cSlot] = RADIO_TECH_MODE_CSFB;
                }
                break;
            }
        }
    } else if (cdmaLteCount > 1 && insertCount > 0) {
        /**
         * M: Should set the ct card's ( which has 3/4 G capability ) mode to cdmalte.
         * other cards' mode should be set to csfb.
         * And find ct cards which will been change to Gsm Card.
         */
        if (slotPreferRadioTec[capabilitySlot] == RADIO_TECH_MODE_CDMALTE &&
                cardTypes[capabilitySlot] != CARD_TYPE_NONE) {
            newCardModes[capabilitySlot] = RADIO_TECH_MODE_CDMALTE;
            for (int index =0; index < slotNum; index++) {
                if (index != capabilitySlot) {
                    newCardModes[index] = RADIO_TECH_MODE_CSFB;

                    mode = slotPreferRadioTec[index];
                    if (mode == RADIO_TECH_MODE_CDMALTE
                            && isCt3GDualMode(index)) {
                        needSwitchCardTypeSlots[index] = index;
                    }
                }
            }
        } else {
            /// M: Find the first c card and set it's card mode to cdmalte, set other c cards' mode to csfb.
            int firstCCard = -1;
            for (int index =0; index < slotNum; index++) {
                mode = slotPreferRadioTec[index];
                if (mode == RADIO_TECH_MODE_CDMALTE && firstCCard == -1 &&
                        cardTypes[index] != CARD_TYPE_NONE) {
                    newCardModes[index] = RADIO_TECH_MODE_CDMALTE;
                    firstCCard = index;
                } else {
                    newCardModes[index] = RADIO_TECH_MODE_CSFB;
                    if (mode == RADIO_TECH_MODE_CDMALTE
                            && isCt3GDualMode(index)) {
                        RFX_LOG_D(RP_CDMALTE_MODE_TAG,
                                "[SMC]calculateCardMode needSwitchCardTypeSlots: %d", index);
                        needSwitchCardTypeSlots[index] = index;
                    }
                }
            }
        }
    }
    for (int index = 0; index < slotNum; index++) {
        RFX_LOG_D(RP_CDMALTE_MODE_TAG,
                "[SMC]calculateCardMode slot: %d, mode: %s", index,
                (newCardModes[index] == RADIO_TECH_MODE_CDMALTE ?
                "RADIO_TECH_MODE_CDMALTE" :
                (newCardModes[index] == RADIO_TECH_MODE_CSFB ? "RADIO_TECH_MODE_CSFB" : "NULL")));
    }

    delete []slotPreferRadioTec;
}
