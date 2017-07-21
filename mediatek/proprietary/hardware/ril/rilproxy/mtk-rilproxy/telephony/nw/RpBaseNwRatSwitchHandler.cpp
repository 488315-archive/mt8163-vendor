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

#include "RpBaseNwRatSwitchHandler.h"

/*****************************************************************************
 * Class RpBaseNwRatSwitchHandler
 *****************************************************************************/

RFX_IMPLEMENT_CLASS("RpBaseNwRatSwitchHandler", RpBaseNwRatSwitchHandler, RfxObject);

RpBaseNwRatSwitchHandler::RpBaseNwRatSwitchHandler() {
}

RpBaseNwRatSwitchHandler::RpBaseNwRatSwitchHandler(RpNwRatController* nwRatController) :
    mNwRatController(nwRatController) {
    mSlotId = mNwRatController->getSlotId();
}

RpBaseNwRatSwitchHandler::~RpBaseNwRatSwitchHandler() {
}

void RpBaseNwRatSwitchHandler::requestGetPreferredNetworkType(const RILD_RadioTechnology_Group radioTechType, const sp<RfxMessage>& msg) {
    int msg_id = RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE;
    sp<RfxMessage> message = RfxMessage::obtainRequest(radioTechType, msg_id, msg);
    mNwRatController->requestToRild(message);
}

void RpBaseNwRatSwitchHandler::requestSetPreferredNetworkType(const RILD_RadioTechnology_Group radioTechType, const int prefNwType, const sp<RfxMessage>& msg) {
    if (!needRadioOn(radioTechType, prefNwType)) {
        logD(LOG_TAG, "requestSetPreferredNetworkType(), needRadioOn()=false.");
        return;
    }

    CombinedPreferredNwType type = getCombinedPreferredNwType(prefNwType);

    int nwType = -1;
    if (radioTechType == RADIO_TECH_GROUP_C2K) {
        nwType = type.c2k_preferred_nw_type;
    } else if (radioTechType == RADIO_TECH_GROUP_GSM) {
        nwType = type.gsm_preferred_nw_type;
    } else {
        return;
    }

    nwType = filterPreferNwTypeForTdCard(nwType);
    if (nwType != -1) {
        int msg_id = RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE;
        sp<RfxMessage> message = NULL;
        if (msg != NULL) {
            message = RfxMessage::obtainRequest(radioTechType, msg_id, msg);
        } else {
            message = RfxMessage::obtainRequest(mSlotId, RADIO_TECH_GROUP_GSM,
                    msg_id);
        }
        message->getParcel()->writeInt32(1);
        message->getParcel()->writeInt32(nwType);
        logD(LOG_TAG, "requestSetPreferredNetworkType(), set nw type to %d.",
                nwType);
        mNwRatController->requestToRild(message);
    }
}

void RpBaseNwRatSwitchHandler::requestGetVoiceRadioTech(const sp<RfxMessage>& msg) {
    int msg_id = RIL_REQUEST_VOICE_RADIO_TECH;
    sp<RfxMessage> message = RfxMessage::obtainRequest(RADIO_TECH_GROUP_GSM, msg_id, msg);
    mNwRatController->requestToRild(message);
}

void RpBaseNwRatSwitchHandler::responseGetVoiceRadioTech(const sp<RfxMessage>& response) {
    int error = response->getError() ;
    if (error != RIL_E_SUCCESS) {
        // error
        logD(LOG_TAG, "responseGetVoiceRadioTech(), error is %d.", error);
    } else {
        // ok
        int source = response->getSource();
        int32_t stgCount;
        int32_t radioTech;
        response->getParcel()->readInt32(&stgCount);
        response->getParcel()->readInt32(&radioTech);
        if (source == RADIO_TECH_GROUP_GSM) {
            sp<RfxMessage> resToRilj = RfxMessage::obtainResponse(RIL_E_SUCCESS, response);
            resToRilj->getParcel()->writeInt32(1);
            resToRilj->getParcel()->writeInt32(radioTech);
            logD(LOG_TAG, "responseGetVoiceRadioTech(), send response %d.", radioTech);
            mNwRatController->responseToRilj(resToRilj);
        }
    }
}

void RpBaseNwRatSwitchHandler::updatePhone(const int prefNwType) {
    int tech = techFromPrefNwType(prefNwType);
    logD(LOG_TAG, "updatePhone(), techFromPrefNwType is %d.", tech);
    if (tech > RADIO_TECH_UNKNOWN) {
        sp<RfxMessage> urcToRilj = RfxMessage::obtainUrc(mSlotId,
                RIL_UNSOL_VOICE_RADIO_TECH_CHANGED);
        urcToRilj->getParcel()->writeInt32(1);
        urcToRilj->getParcel()->writeInt32(tech);
        mNwRatController->responseToRilj(urcToRilj);
    }
}

int RpBaseNwRatSwitchHandler::techFromPrefNwType(const int prefNwType) {
    switch (prefNwType) {
    case PREF_NET_TYPE_GSM_ONLY:
        return RADIO_TECH_GPRS;
    case PREF_NET_TYPE_GSM_WCDMA:
    case PREF_NET_TYPE_GSM_WCDMA_AUTO:
    case PREF_NET_TYPE_WCDMA:
        return RADIO_TECH_HSPA;

    case PREF_NET_TYPE_LTE_GSM_WCDMA:
    case PREF_NET_TYPE_LTE_ONLY:
    case PREF_NET_TYPE_LTE_TDD_ONLY:
    case PREF_NET_TYPE_LTE_WCDMA:
    case PREF_NET_TYPE_LTE_GSM:
        return RADIO_TECH_LTE;

    case PREF_NET_TYPE_CDMA_ONLY:
        return RADIO_TECH_1xRTT;

    case PREF_NET_TYPE_CDMA_EVDO_AUTO:
    case PREF_NET_TYPE_EVDO_ONLY:
    case PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO:
    case PREF_NET_TYPE_LTE_CDMA_EVDO:
    case PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA:
        return RADIO_TECH_EVDO_A;
    }
    return RADIO_TECH_UNKNOWN;
}
