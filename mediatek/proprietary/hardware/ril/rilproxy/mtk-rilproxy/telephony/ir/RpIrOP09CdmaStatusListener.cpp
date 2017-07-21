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
#include "RpIrOP09CdmaStatusListener.h"
#include "RpIrInvalidMccConvert.h"
#include "RpIrStrategyOP095M.h"

#define RFX_LOG_TAG "[IRC][RpIrOP09CdmaStatusListener]"
/*****************************************************************************
 * Class RpIrOP09CdmaStatusListener
 *****************************************************************************/

RFX_IMPLEMENT_CLASS("RpIrOP09CdmaStatusListener", RpIrOP09CdmaStatusListener, RfxController);

RpIrOP09CdmaStatusListener::RpIrOP09CdmaStatusListener():
        m_RpIrStrategyOP095M(NULL),
        m_RpIrController(NULL) {
}

RpIrOP09CdmaStatusListener::RpIrOP09CdmaStatusListener(RpIrStrategyOP095M *rpIrStrategyOP095M, RpIrController *rpIrController) :
        m_RpIrStrategyOP095M(rpIrStrategyOP095M),
        m_RpIrController(rpIrController) {
}

void RpIrOP09CdmaStatusListener::onRadioStateChanged(bool isRadioOn) {
    if (m_RpIrStrategyOP095M->getIsCdmaRadioOn() != isRadioOn) {
        logD(RFX_LOG_TAG, "[CDMA]onRadioStateChanged : %s", Bool2Str(isRadioOn));
        m_RpIrStrategyOP095M->setIsCdmaRadioOn(isRadioOn);
        m_RpIrStrategyOP095M->onRadioStateChanged();
    }
}

void RpIrOP09CdmaStatusListener::onPlmnChanged(String8 &plmn) {
    logD(RFX_LOG_TAG, "[CDMA]onPlmnChanged : %s", plmn.string());
    NwsMode oldMode = m_RpIrController->getNwsMode();

    plmn = convertInvalidMccBySidNid(plmn);

    m_RpIrStrategyOP095M->onCdmaPlmnChanged(plmn);

    if (oldMode != m_RpIrController->getNwsMode()) {
        m_RpIrStrategyOP095M->restartNoSerivceWatchdogIfNeeded();
    } else {
        m_RpIrStrategyOP095M->postponeNoServiceWatchdogIfNeeded();
    }
}

bool RpIrOP09CdmaStatusListener::needWaitImsi() {
    if (m_RpIrController->isCtSimCard()) {
        // already know it's CT card, so don't need to
        // wait for IMSI ready to get mcc
        return false;
    }
    return m_RpIrStrategyOP095M->supportRoaming();
}


String8 RpIrOP09CdmaStatusListener::convertInvalidMccBySidNid(const String8 &plmn) {
    logD(RFX_LOG_TAG, "[CDMA] convertInvalidMccBySidNid, plmn=%s", plmn.string());
    String8 convertCdmaPlmn = plmn;
    if ((plmn == "2134" && plmn.size() == 7) || plmn == "0000") {
        // Re-get plmn for special operator which doesn't release plmn when
        // network searched.
        convertCdmaPlmn = RpIrInvalidMccConvert::checkMccBySidLtmOff(plmn);
        logD(RFX_LOG_TAG, "[CDMA] convertInvalidMccBySidNid, convertCdmaPlmn = %s", convertCdmaPlmn.string());
    }
    return convertCdmaPlmn;
}

void RpIrOP09CdmaStatusListener::onServiceStateChanged(ServiceType serviceType) {
    logD(RFX_LOG_TAG, "[CDMA]onServiceStateChanged(%s)", ServiceType2Str(serviceType));
    if (serviceType != m_RpIrStrategyOP095M->getCdmaServiceState()) {
        m_RpIrStrategyOP095M->setCdmaServiceState(serviceType);
        if (serviceType == OUT_OF_SERVICE) {
            if (m_RpIrStrategyOP095M->getLwgServiceState() == OUT_OF_SERVICE) {
                m_RpIrStrategyOP095M->switchForNoService(true);
            }
        }
        m_RpIrStrategyOP095M->updateWatchdog();
    }
}

void RpIrOP09CdmaStatusListener::onNwsModeSwitchDone() {
    m_RpIrStrategyOP095M->getLwgController()->startNewSearchRound();
    m_RpIrStrategyOP095M->getCdmaController()->startNewSearchRound();
}

