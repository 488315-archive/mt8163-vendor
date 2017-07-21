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
#include <cutils/properties.h>
#include <cutils/jstring.h>
#include "RfxStatusDefs.h"
#include "RpNwController.h"
#include "RpNwRatController.h"
#include "RpIrController.h"
#include "RpWpController.h"

#define LOG_TAG "RpNwController"
/*****************************************************************************
 * Class RpNwController
 *****************************************************************************/

RFX_IMPLEMENT_CLASS("RpNwController", RpNwController, RfxController);

RpNwController::RpNwController() {
}

RpNwController::~RpNwController() {
}

void RpNwController::onInit() {
    // Required: invoke super class implementation
    RfxController::onInit();

    const int urc_id_list[] = { RIL_UNSOL_ENG_MODE_NETWORK_INFO };
    registerToHandleUrc(urc_id_list, 1);
    logD(LOG_TAG, " register URC: %s", urcToString(RIL_UNSOL_ENG_MODE_NETWORK_INFO));

    // create the object of RpNwRatController as the child controller
    // of this object (an instance of RpNwController)
    RpNwRatController* nw_rat_controller = NULL;
    RFX_OBJ_CREATE(nw_rat_controller, RpNwRatController, this);

    RpWpController *wp_controller;
    RFX_OBJ_CREATE(wp_controller, RpWpController, this);

    RpIrController *ir_controller;
    RFX_OBJ_CREATE(ir_controller, RpIrController, this);
}

void RpNwController::onDeinit() {
    // Required: invoke super class implementation
    RfxController::onDeinit();
}

bool RpNwController::onHandleRequest(const sp<RfxMessage>& message) {
    logD(LOG_TAG, " handle req %s (slot %d)",
            requestToString(message->getId()), getSlotId());

    switch (message->getId()) {
    default:
        break;
    }
    return true;
}

bool RpNwController::onHandleUrc(const sp<RfxMessage>& message) {
    logD(LOG_TAG, " handle urc %s (slot %d)",
            urcToString(message->getId()), getSlotId());

    switch (message->getId()) {
    case RIL_UNSOL_ENG_MODE_NETWORK_INFO:
        convertFromEiToNi(message);
        break;
    default:
        break;
    }
    return true;
}

bool RpNwController::onHandleResponse(const sp<RfxMessage>& message) {
    logD(LOG_TAG, " handle %s response (slot %d)",
            requestToString(message->getId()), getSlotId());

    switch (message->getId()) {
    default:
        break;
    }
    return true;
}

char* RpNwController::requestToString(int reqId) {
    switch (reqId) {
    default:
        logD(LOG_TAG, " <UNKNOWN_REQUEST>");
        break;
    }
    return "";
}

char* RpNwController::urcToString(int reqId) {
    switch (reqId) {
    case RIL_UNSOL_ENG_MODE_NETWORK_INFO:
        return "RIL_UNSOL_ENG_MODE_NETWORK_INFO";
    default:
        logD(LOG_TAG, " <UNKNOWN_URC>");
        break;
    }
    return "";
}

void RpNwController::convertFromEiToNi(const sp<RfxMessage>& message) {
    int source = message->getSource();
    Parcel *p = message->getParcel();

    logD(LOG_TAG, " convertFromEiToNi: source = %d", source);
    if (source == RADIO_TECH_GROUP_C2K) {
        int32_t size;
        p->readInt32(&size);
        if (size <= 0) {
            RLOGD("[RpNwController] convertFromEiToNi: invalid message");
            return;
        }

        logD(LOG_TAG, " convertFromEiToNi: get urc message size = %d",
                size);
        char *pStrings[size];
        for(int i = 0; i< size; i++){
            pStrings[i] = strdupReadString(p);
            logD(LOG_TAG, " convertFromEiToNi: get data from urc, data = %s ",
                    pStrings[i]);
        }

        sp<RfxMessage> urcToRilj = RfxMessage::obtainUrc(getSlotId(),
                RIL_UNSOL_NETWORK_INFO);
        urcToRilj->getParcel()->writeInt32(2);
        writeStringToParcel(urcToRilj->getParcel(), Eng_Nw_Info_Type);
        writeStringToParcel(urcToRilj->getParcel(), pStrings[size- 1]);
        responseToRilj(urcToRilj);
    }
}

char *RpNwController::strdupReadString(Parcel *p) {
    size_t stringlen;
    const char16_t *s16;

    s16 = p->readString16Inplace(&stringlen);

    return strndup16to8(s16, stringlen);
}

void RpNwController::writeStringToParcel(Parcel *p, const char *s) {
    char16_t *s16;
    size_t s16_len;
    s16 = strdup8to16(s, &s16_len);
    p->writeString16(s16, s16_len);
    free(s16);
}

