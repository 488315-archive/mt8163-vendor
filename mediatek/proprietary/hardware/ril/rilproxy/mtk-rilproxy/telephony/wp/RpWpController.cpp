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
#include <utils/Log.h>
#include "RfxLog.h"
#include "RfxStatusDefs.h"
#include "RpWpController.h"

#define LOG_TAG "RpWpController"
/*****************************************************************************
 * Class RpWpController
 *****************************************************************************/

RFX_IMPLEMENT_CLASS("RpWpController", RpWpController, RfxController);

RpWpController::RpWpController() {
}

RpWpController::~RpWpController() {
}

void RpWpController::onInit() {
    // Required: invoke super class implementation
    RfxController::onInit();

    const int urc_id_list[] = {
        RIL_UNSOL_WORLD_MODE_CHANGED
    };

    registerToHandleUrc(urc_id_list, 1);
}

void RpWpController::onDeinit() {
    // Required: invoke super class implementation
    RfxController::onDeinit();
}

bool RpWpController::onHandleUrc(const sp<RfxMessage>& message) {
    logD(LOG_TAG, "handle urc %s (slot %d)",
            urcToString(message->getId()), getSlotId());

    switch (message->getId()) {
    case RIL_UNSOL_WORLD_MODE_CHANGED:
        processWorldModeChanged(message);
        responseToRilj(message);
        break;
    default:
        break;
    }
    return true;
}

char* RpWpController::urcToString(int reqId) {
    switch (reqId) {
    case RIL_UNSOL_WORLD_MODE_CHANGED:
        return "RIL_UNSOL_WORLD_MODE_CHANGED";
    default:
        logD(LOG_TAG, "<UNKNOWN_URC>");
        return "<UNKNOWN_URC>";
    }
}

void RpWpController::processWorldModeChanged(const sp<RfxMessage>& msg) {
    if (msg->getError() != RIL_E_SUCCESS) {
        logD(LOG_TAG, "processWorldModeChanged, msg is not SUCCESS");
        return;
    }

    int32_t count = 0;
    int32_t state = -1;

    Parcel *p = msg->getParcel();
    count = p->readInt32();
    state = p->readInt32();
    logD(LOG_TAG, "processWorldModeChanged, count=%d state=%d", count, state);

    getStatusManager()->setIntValue(RFX_STATUS_KEY_WORLD_MODE_STATE, state);
}
