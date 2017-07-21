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
#include "RpPhbController.h"
#include "RfxLog.h"

#define RFX_LOG_TAG "RpPhbController"

/*****************************************************************************
 * Class RpPhbController
 *****************************************************************************/

RFX_IMPLEMENT_CLASS("RpPhbController", RpPhbController, RfxController);

RpPhbController::RpPhbController() :
        mCardType(-1) {
}

RpPhbController::~RpPhbController() {
}

void RpPhbController::onInit() {
    RfxController::onInit(); // Required: invoke super class implementation

    const int request_id_list[] = {
            RIL_REQUEST_QUERY_PHB_STORAGE_INFO,
    };
    const int normal_request_id_lit[] = {
            RIL_REQUEST_WRITE_PHB_ENTRY,
            RIL_REQUEST_READ_PHB_ENTRY,
    };
    registerToHandleReuquest(request_id_list, (sizeof(request_id_list)/sizeof(int)));
    registerRequestToCsRild(normal_request_id_lit,(sizeof(normal_request_id_lit)/sizeof(int)));

}

bool RpPhbController::onHandleRequest(const sp<RfxMessage>& message) {
    RFX_LOG_D(RFX_LOG_TAG, " handle req %d (slot %d)", message->getId(),
            getSlotId());
    RFX_LOG_D(RFX_LOG_TAG, " RIL_REQUEST_QUERY_PHB_STORAGE_INFO= %d "
            , RIL_REQUEST_QUERY_PHB_STORAGE_INFO);
    mCardType = getStatusManager()->getIntValue(RFX_STATUS_KEY_CARD_TYPE, -1);
    RFX_LOG_D(RFX_LOG_TAG, " mCardType= %d "
                , mCardType);
    int msg_id = message->getId();
    switch (msg_id) {
        case RIL_REQUEST_QUERY_PHB_STORAGE_INFO:
            {
                // let MD1 handler all of the cases if it 4G cdma card.
                if (mCardType == RFX_CARD_TYPE_RUIM) {
                    sp<RfxMessage> phb_query_request = RfxMessage::obtainRequest(
                        RADIO_TECH_GROUP_C2K, msg_id, message, true);
                    requestToRild(phb_query_request);
                } else {
                    sp<RfxMessage> phb_query_request = RfxMessage::obtainRequest(
                        RADIO_TECH_GROUP_GSM, msg_id, message, true);
                    requestToRild(phb_query_request);
                }
            }
            break;
        default:
            break;
    }
    return true;
}


