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

#include "RpGsmNwRatSwitchHandler.h"

#define LOG_TAG "RpGsmNwRatSwitchHandler"

/*****************************************************************************
 * Class RpGsmNwRatSwitchHandler
 *****************************************************************************/

RFX_IMPLEMENT_CLASS("RpGsmNwRatSwitchHandler", RpGsmNwRatSwitchHandler, RpBaseNwRatSwitchHandler);

RpGsmNwRatSwitchHandler::RpGsmNwRatSwitchHandler() {
}

RpGsmNwRatSwitchHandler::RpGsmNwRatSwitchHandler(RpNwRatController* nwRatController) :
    RpBaseNwRatSwitchHandler(nwRatController) {
}

RpGsmNwRatSwitchHandler::~RpGsmNwRatSwitchHandler() {
}

void RpGsmNwRatSwitchHandler::requestGetPreferredNetworkType(const sp<RfxMessage>& message) {
    RpBaseNwRatSwitchHandler::requestGetPreferredNetworkType(RADIO_TECH_GROUP_GSM, message);
}

void RpGsmNwRatSwitchHandler::responseGetPreferredNetworkType(const sp<RfxMessage>& response) {
    int error = response->getError() ;
    if (error != RIL_E_SUCCESS) {
        // error
        logD(LOG_TAG, "responseGetPreferredNetworkType(), error is %d.", error);
    } else {
        // ok
        int source = response->getSource();
        int32_t stgCount;
        int32_t nwType;
        response->getParcel()->readInt32(&stgCount);
        response->getParcel()->readInt32(&nwType);
        if (source == RADIO_TECH_GROUP_GSM) {
            sp<RfxMessage> resToRilj = RfxMessage::obtainResponse(RIL_E_SUCCESS, response);
            resToRilj->getParcel()->writeInt32(1);
            resToRilj->getParcel()->writeInt32(nwType);
            logD(LOG_TAG, "responseGetPreferredNetworkType(), send response %d.", nwType);
            mNwRatController->responseToRilj(resToRilj);
        }
    }
}

void RpGsmNwRatSwitchHandler::requestSetPreferredNetworkType(const int prefNwType, const sp<RfxMessage>& message) {
    RpBaseNwRatSwitchHandler::requestSetPreferredNetworkType(RADIO_TECH_GROUP_GSM, prefNwType, message);
}

void RpGsmNwRatSwitchHandler::responseSetPreferredNetworkType(const sp<RfxMessage>& response) {
    int error = response->getError() ;
    if (error != RIL_E_SUCCESS) {
        // error
        logD(LOG_TAG, "responseSetPreferredNetworkType(), error is %d. sSwitch:%d", error,
                 RpNwRatController::getSwitchState());
    } else {
        // ok
        int source = response->getSource();
        if (source == RADIO_TECH_GROUP_GSM) {
            sp<RfxMessage> resToRilj = RfxMessage::obtainResponse(RIL_E_SUCCESS, response);
            logD(LOG_TAG, "responseSetPreferredNetworkType(), send response, sSwitch:%d",
                 RpNwRatController::getSwitchState());
            mNwRatController->responseToRilj(resToRilj);
        }
    }

    //Any result, we should do pending record.
    if (!RpNwRatController::getSwitchState()) {
        mNwRatController->doPendingRatSwitchRecord();
    }
}

int RpGsmNwRatSwitchHandler::filterPreferNwTypeForTdCard(const int prefNwType) {
    int targetPerfNwType = prefNwType;
    String8 str = mNwRatController->getStatusManager()->getString8Value(RFX_STATUS_KEY_GSM_IMSI, String8(""));
    logD(LOG_TAG, "configPreferNwType(),  GSM IMSI = %s", str.string());
    bool isCmccCard = false;

    if (!str.isEmpty()) {
        if (str.find("46000", 0) == 0 || str.find("46002", 0) == 0
                || str.find("46007", 0) == 0 || str.find("46008", 0) == 0) {
            isCmccCard = true;
        }
    }
    if (isCmccCard) {
        int projectType = getSvlteProjectType();
        logD(LOG_TAG, "configPreferNwType(),  project type = %d. ", projectType);

        if (projectType == SVLTE_PROJ_SC_3M
                || projectType == SVLTE_PROJ_DC_3M) {
            targetPerfNwType = PREF_NET_TYPE_GSM_ONLY;
        }

        if (projectType == SVLTE_PROJ_SC_5M
                || projectType == SVLTE_PROJ_DC_5M) {
            targetPerfNwType = prefNwType;
            switch (prefNwType) {
            case PREF_NET_TYPE_GSM_WCDMA_AUTO:
            case PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO:
            case PREF_NET_TYPE_GSM_WCDMA:
                targetPerfNwType = PREF_NET_TYPE_GSM_ONLY;
                break;
            case PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA:
            case PREF_NET_TYPE_LTE_GSM_WCDMA:
                targetPerfNwType = PREF_NET_TYPE_LTE_GSM;
                break;
            case PREF_NET_TYPE_LTE_WCDMA:
                targetPerfNwType = PREF_NET_TYPE_LTE_ONLY;
                break;
            case PREF_NET_TYPE_WCDMA:
                targetPerfNwType = -1;
                break;
            }
        }
    }
    logD(LOG_TAG, "configPreferNwType(), targetPerfNwType is %d . ", targetPerfNwType);
    return targetPerfNwType;
}
