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
#include "RfxLog.h"
#include "RpIrStrategyOP095M.h"
#include "RpNwRatController.h"
#include "RpIrNwRatSwitchCallback.h"

#define RFX_LOG_TAG "[IRC][RpIrNwRatSwitchCallback]"

/*****************************************************************************
 * Class RpIrNwRatSwitchCallback
 *****************************************************************************/
RFX_IMPLEMENT_CLASS("RpIrNwRatSwitchCallback", RpIrNwRatSwitchCallback, RfxController);

RpIrNwRatSwitchCallback::RpIrNwRatSwitchCallback() :
        mRpIrController(NULL),
        mRpIrCdmaHandler(NULL),
        mRpIrLwgHandler(NULL),
        mRpIrStrategy(NULL),
        mRpNwRatController(NULL) {
}

RpIrNwRatSwitchCallback::RpIrNwRatSwitchCallback(
        RpIrController *rpIrController,
        RpIrCdmaHandler *rpIrCdmaHandler,
        RpIrLwgHandler *rpIrLwgHandler,
        RpIrStrategy *rpIrStrategy) :
        mRpIrController(rpIrController),
        mRpIrCdmaHandler(rpIrCdmaHandler),
        mRpIrLwgHandler(rpIrLwgHandler),
        mRpIrStrategy(rpIrStrategy),
        mRpNwRatController(NULL) {
}

RpIrNwRatSwitchCallback::~RpIrNwRatSwitchCallback()
{
}

void RpIrNwRatSwitchCallback::onInit() {
    RfxController::onInit();
    mRpNwRatController = (RpNwRatController *)findController(RFX_OBJ_CLASS_INFO(RpNwRatController));
    RFX_ASSERT(mRpNwRatController != NULL);
}


void RpIrNwRatSwitchCallback::onRatSwitchStart(const int curPrefNwType,
            const int newPrefNwType, const NwsMode curNwsMode,
            const NwsMode newNwsMode)
{
    logD(RFX_LOG_TAG, "onRatSwitchStart, curPrefNwType=%s, newPrefNwType=%s, curNwsMode=%s, newNwsMode=%s",
            prefNwType2Str(curPrefNwType), prefNwType2Str(newPrefNwType), Nws2Str(curNwsMode), Nws2Str(newNwsMode));
    if (!mRpIrController->isCdmaDualModeSimCard()) {
        logD(RFX_LOG_TAG, "onRatSwitchStart() not CT 4G or 3G dual mode return");
        mRpIrController->setIrEnableState(false);
        return;
    }
    const bool curPrefNwTypeIs3G = is3GPrefNwType(curPrefNwType);
    const bool newPrefNwTypeIs3G = is3GPrefNwType(newPrefNwType);

    if ((!curPrefNwTypeIs3G && newPrefNwTypeIs3G) || (curPrefNwTypeIs3G && !newPrefNwTypeIs3G)) {
        logD(RFX_LOG_TAG, "onRatSwitchStart, disable IR Controller and Gmss handle while switching");
        // disable IR controll while switching between AP-iRAT IR and MD-iRAT IR
        if (!mRpIrCdmaHandler->isCT3GCardType()) {
            mRpIrController->setIrControllerEnableState(false);
        }
        mRpIrController->setGmssEnableState(false);
    }
    if (newPrefNwTypeIs3G) {
        mRpIrLwgHandler->setIfEnabled(true);
        mRpIrCdmaHandler->setIfEnabled(true);
    } else {
        mRpIrLwgHandler->setIfEnabled(false);
        mRpIrCdmaHandler->setIfEnabled(false);
    }

    if (!curPrefNwTypeIs3G && newPrefNwTypeIs3G) {
        mRpIrController->setIsSwitchingTo3GMode(true);
    }

    mRpIrController->onNwsModeChange(newNwsMode);
}

void RpIrNwRatSwitchCallback::onRatSwitchDone(const int curPrefNwType,
            const int newPrefNwType)
{
    logD(RFX_LOG_TAG, "onRatSwitchDone, curPrefNwType=%s, newPrefNwType=%s",
            prefNwType2Str(curPrefNwType), prefNwType2Str(newPrefNwType));
    if (!mRpIrController->isCdmaDualModeSimCard()) {
        logD(RFX_LOG_TAG, "onRatSwitchDone()  not CT 4G or 3G dual mode return");
        return;
    }

    mRpIrController->setIsSwitchingTo3GMode(false);

    if (is3GPrefNwType(newPrefNwType)) {
        mRpIrController->setIrControllerEnableState(true);
        mRpIrLwgHandler->setIfEnabled(true);
        mRpIrCdmaHandler->setIfEnabled(true);
    } else {
        mRpIrController->setGmssEnableState(true);
        mRpIrLwgHandler->setIfEnabled(false);
        mRpIrCdmaHandler->setIfEnabled(false);
    }

    // For RpIrStrategyOP095M
    mRpIrStrategy->onRatSwitchDone(curPrefNwType, newPrefNwType);
}

void RpIrNwRatSwitchCallback::onEctModeChangeDone(const int curPrefNwType,
            const int newPrefNwType)
{
    logD(RFX_LOG_TAG, "onEctModeChangeDone, curPrefNwType=%s, newPrefNwType=%s",
            prefNwType2Str(curPrefNwType), prefNwType2Str(newPrefNwType));
    if (!mRpIrController->isCdmaDualModeSimCard()) {
        logD(RFX_LOG_TAG, "onEctModeChangeDone()  not CT 4G or 3G dual mode return");
        return;
    }

    if (is3GPrefNwType(newPrefNwType)) {
        mRpIrController->setIrControllerEnableState(true);
    }
}

bool RpIrNwRatSwitchCallback::is3GPrefNwType(int prefNwType) {
    switch (prefNwType) {
    case PREF_NET_TYPE_LTE_CDMA_EVDO:
    case PREF_NET_TYPE_LTE_GSM_WCDMA:
    case PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA:
    case PREF_NET_TYPE_LTE_ONLY:
    case PREF_NET_TYPE_LTE_WCDMA:
    case PREF_NET_TYPE_LTE_GSM:
    case PREF_NET_TYPE_LTE_TDD_ONLY:
        return false;
    default:
        return true;
    }
}

const char* RpIrNwRatSwitchCallback::prefNwType2Str(int prefNwType) {
    switch (prefNwType) {
    case PREF_NET_TYPE_GSM_WCDMA:
        return "PREF_NET_TYPE_GSM_WCDMA";
        break;
    case PREF_NET_TYPE_GSM_ONLY:
        return "PREF_NET_TYPE_GSM_ONLY";
        break;
    case PREF_NET_TYPE_WCDMA:
        return "PREF_NET_TYPE_WCDMA";
        break;
    case PREF_NET_TYPE_GSM_WCDMA_AUTO:
        return "PREF_NET_TYPE_GSM_WCDMA_AUTO";
        break;
    case PREF_NET_TYPE_CDMA_EVDO_AUTO:
        return "PREF_NET_TYPE_CDMA_EVDO_AUTO";
        break;
    case PREF_NET_TYPE_CDMA_ONLY:
        return "PREF_NET_TYPE_CDMA_ONLY";
        break;
    case PREF_NET_TYPE_EVDO_ONLY:
        return "PREF_NET_TYPE_EVDO_ONLY";
        break;
    case PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO:
        return "PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO";
        break;
    case PREF_NET_TYPE_LTE_CDMA_EVDO:
        return "PREF_NET_TYPE_LTE_CDMA_EVDO";
        break;
    case PREF_NET_TYPE_LTE_GSM_WCDMA:
        return "PREF_NET_TYPE_LTE_GSM_WCDMA";
        break;
    case PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA:
        return "PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA";
        break;
    case PREF_NET_TYPE_LTE_ONLY:
        return "PREF_NET_TYPE_LTE_ONLY";
        break;
    case PREF_NET_TYPE_LTE_WCDMA:
        return "PREF_NET_TYPE_LTE_WCDMA";
        break;
    case PREF_NET_TYPE_LTE_GSM:
        return "PREF_NET_TYPE_LTE_GSM";
        break;
    case PREF_NET_TYPE_LTE_TDD_ONLY:
        return "PREF_NET_TYPE_LTE_TDD_ONLY";
        break;
    default:
        break;
    }
    return "unknown";
}

