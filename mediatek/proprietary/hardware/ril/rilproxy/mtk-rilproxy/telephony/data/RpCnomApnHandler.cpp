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

#include "RpBaseApnHandler.h"
#include "RpCnomApnHandler.h"
#include "RpDataUtils.h"

/*****************************************************************************
 * Class RpCnomApnHandler
 *****************************************************************************/

#define RFX_LOG_TAG "RP_DC_CAPN"

RFX_IMPLEMENT_CLASS("RpCnomApnHandler", RpCnomApnHandler, RpBaseApnHandler);

RpCnomApnHandler::RpCnomApnHandler() {
}

RpCnomApnHandler::~RpCnomApnHandler() {
}

void RpCnomApnHandler::onInit() {
    RpBaseApnHandler::onInit();  // Required: invoke super class implementation
    logD(RFX_LOG_TAG, "onInit");
    const int request_id_list[] = {
            RIL_REQUEST_DEACTIVATE_LINK_DOWN_PDN,
    };
    registerToHandleRequest(request_id_list,
            sizeof(request_id_list) / sizeof(int));
}

void RpCnomApnHandler::onDeinit() {
    logD(RFX_LOG_TAG, "onDeinit");
    RpBaseApnHandler::onDeinit();
}

void RpCnomApnHandler::onDataSetupSucceed() {
    logD(RFX_LOG_TAG, "onDataSetupSucceed");
    deactivateLinkDownPdn();
}

void RpCnomApnHandler::deactivateLinkDownPdn() {
    logD(RFX_LOG_TAG, "deactivateLinkDownPdn");
    sp<RfxMessage> msg = RfxMessage::obtainRequest(getSlotId(),
                RADIO_TECH_GROUP_GSM, RIL_REQUEST_DEACTIVATE_LINK_DOWN_PDN);
        requestToRild(msg);
}

bool RpCnomApnHandler::onHandleResponse(const sp<RfxMessage>& message) {
    logD(RFX_LOG_TAG, "handle %s response",
            RpDataUtils::requestToString(message->getId()));

    switch (message->getId()) {
    case RIL_REQUEST_DEACTIVATE_LINK_DOWN_PDN:
        handleDeactivateLinkDownPdnResponse(message);
        break;
    default:
        logD(RFX_LOG_TAG, "unknown response, ignore!");
        break;
    }
    return true;
}
void RpCnomApnHandler::handleDeactivateLinkDownPdnResponse(const sp<RfxMessage>& response) {
    RFX_UNUSED(response);
    logD(RFX_LOG_TAG, "handleDeactivateLinkDownPdnResponse");
}

sp<RfxMessage> RpCnomApnHandler::onSetInitialAttachApn(const sp<RfxMessage>& request) {
    Parcel* parcel = request->getParcel();
    request->resetParcelDataStartPos();
    String16 apn = parcel->readString16();  // apn
    logD(RFX_LOG_TAG, "onSetInitialAttachApn apn:%s", String8(apn).string());
    String16 ctnet(CTNET);
    // For ctnet ia apn, remove usename and password
    // For the purpose of sharing apn between LTE and CDMA for CT 4G card,
    // we added username and password for CTNET APN(CDMA required),
    // but for the APN with username and password willn't be saved in ia cache.
    // This will casue that once set initial attach apn, ps will be detach and attach.
    if (apn == ctnet) {
        String16 protocol = parcel->readString16();  // protocol
        int authType = parcel->readInt32();  // authType
        String16 username = parcel->readString16();  // username
        String16 password = parcel->readString16();  // password
        String16 operatorNumeric = parcel->readString16();  // operatorNumeric
        int canHandleIms = parcel->readInt32();  // canHandleIms
        String16 dualApnPlmnList = parcel->readString16();  // dualApnPlmnList
        logD(RFX_LOG_TAG, "onSetInitialAttachApn protocol:%s,authType:%d,username:%s,"
                "operatorNumeric:%s,canHandleIms:%d,dualApnPlmnList:%s", String8(protocol).string(),
                authType, String8(username).string(), String8(operatorNumeric).string(),
                canHandleIms, String8(dualApnPlmnList).string());
        sp<RfxMessage> newMsg = RfxMessage::obtainRequest(RADIO_TECH_GROUP_GSM, request->getId(),
                request, false);
        Parcel* newParcel = newMsg->getParcel();
        String16 emptyStr("");
        newParcel->writeString16(apn);
        newParcel->writeString16(protocol);
        newParcel->writeInt32(authType);
        newParcel->writeString16(emptyStr);
        newParcel->writeString16(emptyStr);
        newParcel->writeString16(operatorNumeric);
        newParcel->writeInt32(canHandleIms);
        newParcel->writeString16(dualApnPlmnList);
        return newMsg;
    }
    return request;
}


