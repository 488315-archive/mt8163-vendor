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
#include "RpMalController.h"
#include "Parcel.h"
#include "RfxMainThread.h"
#include "RfxSocketState.h"
#include <utils/Log.h>
#include "utils/RefBase.h"
#include "utils/Errors.h"
#include <cutils/properties.h>
#include <string.h>
/*****************************************************************************
 * Class RfxController
 *****************************************************************************/

#define LOG_TAG "RpMalController"

#define ARRAY_LENGTH(array) (sizeof(array)/sizeof(array[0]))

extern "C" void setRadioState(RIL_RadioState newState, RIL_SOCKET_ID rid);

static const int INVALID_SERIAL = -1;

RFX_IMPLEMENT_CLASS("RpMalController", RpMalController, RfxController);

RpMalController::RpMalController() {
}

RpMalController::~RpMalController() {
}

static const int request[] = {
    RIL_REQUEST_AT_COMMAND_WITH_PROXY,
    RIL_REQUEST_AT_COMMAND_WITH_PROXY_CDMA,
};

static const int urc[] = {
};

static const int INVALID_VALUE = -1;

void RpMalController::onInit() {
    RLOGD("RpMalController init()");
    RfxController::onInit();
    for (int i = 0; i < SIM_COUNT; i++) {
        registerToHandleRequest(i, request, ARRAY_LENGTH(request));
    }
    // For telling MAL which socket they should use

    char prop_val[PROPERTY_VALUE_MAX] = {0};
    property_get("persist.chiptest.enable", prop_val, "0");
    int chipMode = atoi(prop_val);

    if (chipMode == 1) {
        RLOGD("chipmode: %d", chipMode);
        RLOGD("MAL socket is set to ril-proxy-mal");
        property_set("ril.mal.socket","rilproxy-mal");
    }
    RLOGD("RpMalController init() done:");
}


bool RpMalController::onPreviewMessage(const sp<RfxMessage>& message) {
    if (message -> getType() == REQUEST) {
        if (canHandleRequest(message)) {
            return true;
        } else {
            return false;
        }
    } else {
        RLOGD("Not Request, don't do check");
        return true;
    }
}

bool RpMalController::onCheckIfResumeMessage(const sp<RfxMessage>& message) {
    if (message -> getType() == REQUEST) {
        if (canHandleRequest(message)) {
            return true;
        } else {
            return false;
        }
    } else {
        return true;
    }
}

bool RpMalController::canHandleRequest(const sp<RfxMessage>& message) {
    RfxSocketState socketState = getStatusManager(message -> getSlotId())
        -> getSocketStateValue(RFX_STATUS_KEY_SOCKET_STATE);
    if (message -> getDest() == RADIO_TECH_GROUP_GSM) {
        bool socketIsConnected = socketState.getSocketState(socketState.SOCKET_GSM);
        RLOGD("GSM socketIsConnected %s", socketIsConnected ? "true" : "false");
        return socketIsConnected ? true : false;
    } else {
        bool socketIsConnected = socketState.getSocketState(socketState.SOCKET_C2K);
        RLOGD("CDMA socketIsConnected %s", socketIsConnected ? "true" : "false");
        return socketIsConnected ? true : false;
    }
    return true;
}

bool RpMalController::onHandleRequest(const sp<RfxMessage>& message) {
    int msg_id = message -> getId();
    int pToken = message -> getPToken();
    RLOGD("RpMalController: handle request id %d", msg_id);
    switch (msg_id) {
    case RIL_REQUEST_AT_COMMAND_WITH_PROXY:
        requestToRild(message);
        break;
    case RIL_REQUEST_AT_COMMAND_WITH_PROXY_CDMA:
        requestToRild(message);
        break;
    default:
        break;
    }
    return true;
}



bool RpMalController::onHandleResponse(const sp<RfxMessage>& message) {

    int msg_id = message -> getId();
    RLOGD("RpMalController: handle response id %d", msg_id);
    switch (msg_id) {
    case RIL_REQUEST_AT_COMMAND_WITH_PROXY:
        responseToRilj(message);
        break;
    case RIL_REQUEST_AT_COMMAND_WITH_PROXY_CDMA:
        responseToRilj(message);
        break;
    default:
        break;
    }
    return true;
}

bool RpMalController::onHandleUrc(const sp<RfxMessage>& message) {
    return true;
}
