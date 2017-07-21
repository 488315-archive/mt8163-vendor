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
#include "RpCdmaLteDataController.h"
#include "RpDataControllerProxy.h"
#include "RfxLog.h"
#include "RfxStatusDefs.h"
#include "RpDataUtils.h"

#define RFX_LOG_TAG "RP_DCP"

/*****************************************************************************
 * Class RpDataControllerProxy
 *****************************************************************************/

RFX_IMPLEMENT_CLASS("RpDataControllerProxy", RpDataControllerProxy, RfxController);

RpDataControllerProxy::RpDataControllerProxy() : mActiveDataController(NULL), mIsCdmaLteMode(false) {
}

RpDataControllerProxy::~RpDataControllerProxy() {
}

void RpDataControllerProxy::onInit() {
    RfxController::onInit();  // Required: invoke super class implementation

    mIsCdmaLteMode = RpDataUtils::isCdmaLteMode(this);

    if (!mIsCdmaLteMode) {
        RFX_OBJ_CREATE(mActiveDataController, RpDataController, this);
        logD(RFX_LOG_TAG, "onInit to CSFB: ctrl = %p.", mActiveDataController);
    } else {
        RpCdmaLteDataController *p;
        RFX_OBJ_CREATE(p, RpCdmaLteDataController, this);
        mActiveDataController = (RpDataController *) p;
        logD(RFX_LOG_TAG, "onInit to CDMALTE: ctrl = %p.", mActiveDataController);
    }

    // Register callbacks to listen mode change event.
    getNonSlotScopeStatusManager()->registerStatusChanged(RFX_STATUS_KEY_MODE_SWITCH,
            RfxStatusChangeCallback(this, &RpDataControllerProxy::onModeChange));
}

void RpDataControllerProxy::onDeinit() {
    logD(RFX_LOG_TAG, "onDeinit");
    getNonSlotScopeStatusManager()->unRegisterStatusChanged(RFX_STATUS_KEY_MODE_SWITCH,
        RfxStatusChangeCallback(this, &RpDataControllerProxy::onModeChange));
    RfxController::onDeinit();
}

void RpDataControllerProxy::onModeChange(RfxStatusKeyEnum key,
    RfxVariant old_value, RfxVariant value) {
    RFX_UNUSED(key);
    int oldStatus = old_value.asInt();
    int newStatus = value.asInt();
    bool isCdmaLteMode = RpDataUtils::isCdmaLteMode(this);
    logD(RFX_LOG_TAG, "onModeChange: new = %d, mIsCdmaLteMode = %d, isCdmaLteMode = %d.",
        newStatus, mIsCdmaLteMode, isCdmaLteMode);
    // after mode switch finish, update controller.

    if (newStatus == MODE_SWITCH_FINISH && mIsCdmaLteMode != isCdmaLteMode) {
        mIsCdmaLteMode = isCdmaLteMode;
        updateDataController();
    }
}

void RpDataControllerProxy::updateDataController() {
    if (mActiveDataController != NULL) {
        // Fwk will call Object.onDeinit() when close controller.
        RFX_OBJ_CLOSE(mActiveDataController);
        mActiveDataController = NULL;
    }

    if (!mIsCdmaLteMode) {
        RFX_OBJ_CREATE(mActiveDataController, RpDataController, this);
        logD(RFX_LOG_TAG, "updateDataController to CSFB: ctrl = %p.", mActiveDataController);
    } else {
        RpCdmaLteDataController *p;
        RFX_OBJ_CREATE(p, RpCdmaLteDataController, this);
        mActiveDataController = (RpDataController *) p;
        logD(RFX_LOG_TAG, "updateDataController to CDMALTE: ctrl = %p.", mActiveDataController);
    }
}
