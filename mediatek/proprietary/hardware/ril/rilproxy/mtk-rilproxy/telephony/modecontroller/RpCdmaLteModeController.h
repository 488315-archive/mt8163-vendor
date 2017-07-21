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

#ifndef __RP_CDMALTE_MODE_CONTROLLER_H__
#define __RP_CDMALTE_MODE_CONTROLLER_H__

/***************************************************************************** 
 * Include
 *****************************************************************************/

#include "ril.h"
#include "nw/RpNwDefs.h"
#include "RfxController.h"
#include "RpCardTypeReadyController.h"
#include "RpCdmaLteDefs.h"
#include "utils/Vector.h"

using namespace android;
/***************************************************************************** 
 * Class RfxController
 *****************************************************************************/

class RpCdmaLteModeController : public RfxController {
    RFX_DECLARE_CLASS(RpCdmaLteModeController);

public:
    RpCdmaLteModeController();
    virtual void onInit();
    virtual void onDeinit();
    virtual ~RpCdmaLteModeController();
    void onCardTypeReady(int* card_types, int* card_state, int slotNum,
        CardTypeReadyReason ctrr);
    int getDefaultNetworkType(int slotId);
    static int getActiveCdmaLteModeSlotId();
    static int getCdmaSocketSlotId();
    static void setCdmaSocketSlotId(int slotId);

private:
    void onRatSwitchDone(int slotId);
    void setActiveCdmaLteModeSlotId();
    void setupCdmaLteMode();
    void initCardModes();
    void startSwitchMode();
    void finishSwitchMode();
    void calculateCardMode();
    void configModemStatus();
    int getCurrentNetworkType(int slotId);
    void setupSwitchQueue(int old_cdma_socket_slot);
    void doSwitchRadioTech();
    void doSwitchRadioTechImpl();
    void onC2kSocketConnected(int slotId, RfxStatusKeyEnum key, RfxVariant old_value, RfxVariant value);
    void switchRadioTechnology(int slotId);
    bool is4GCdmaCard(int cardType);
    bool containsCdma(int cardType);
    bool containsGsm(int cardType);
    bool containsUsim(int cardType);
    bool isUsimOnlyCard(int cardType);
    AppFamilyType getAppFamilyType(int slot);
    bool needSwtichCardType();
    bool tryToTriggerTrm(int slotNum, int old_cdma_socket_slot, CardTypeReadyReason ctrr);
    void triggerTrm();
    int calculateNewCdmaSlotId();
    void getUiccType(int slotId, char* uiccType);
    bool isCdmaCard(char* uicc_type);
    int getFirstCCardSlot();
    int getCCardCount();
    bool isCt3GDualMode(int slotId);

private:
    static int sCdmaSocketSlotId;
    int mCdmaLteModeSlotId;
    bool mInSwitching;
    int* m_pending_card_types;
    int* m_pending_card_states;
    int* mSlotBeSwitchCardType;
    int* m_card_types;
    int* m_card_states;
    int* m_card_modes;
    int* m_old_card_modes;
    bool mInTriggeringTrm;
    bool mSwitchingCardType;
    CardTypeReadyReason mPendingCardTypeReadyReason;
    Vector<int> mSwitchQueue;
    RpCardTypeReadyController* mCardTypeReadyController;
};

#endif /* __RP_CDMALTE_MODE_CONTROLLER_H__ */

