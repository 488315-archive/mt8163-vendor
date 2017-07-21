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
#include "RpIrStrategyOP094M.h"

#define JAP_MCC 440
#define KOR_MCC 450

#define RFX_LOG_TAG "[IRC][RpIrStrategyOP094M]"
/*****************************************************************************
 * Class RpIrStrategyOP094M
 *****************************************************************************/

RFX_IMPLEMENT_VIRTUAL_CLASS("RpIrStrategyOP094M", RpIrStrategyOP094M, RpIrStrategyOP095M);

RpIrStrategyOP094M::RpIrStrategyOP094M(RpIrController* ric,
                               IRpIrNwController* lwgController,
                               IRpIrNwController* cdmaController) :
        RpIrStrategyOP095M(ric, lwgController, cdmaController) {
}

RpIrStrategyOP094M::~RpIrStrategyOP094M() {
}

NwsMode RpIrStrategyOP094M::getNwsModeByPlmn(const String8& plmn) {
    if (!plmn.isEmpty()) {
        int mcc = atoi(string(plmn.string()).substr(0, 3).c_str());
        int home_mcc = getHomeMcc();
        logD(RFX_LOG_TAG, "getNwsModeByPlmn, home_mcc = %d, mcc=%d, is_sim_ready = %s",
                          home_mcc, mcc, Bool2Str(m_IrController->isSimReady()));
        if (home_mcc == 0) {
            // if bootup in CSFB mode, can't get C2K IMSI, so keep CSFB mode,
            // otherwise, it's in CDMALTE mode, should be able to get C2K IMSI
            logD(RFX_LOG_TAG, "getNwsModeByPlmn, plmn=[%s]  ret: [NWS_MODE_CSFB]", plmn.string());
            return NWS_MODE_CSFB;
        }
        logD(RFX_LOG_TAG, "getNwsModeByPlmn, mcc=%d", mcc);
        if (home_mcc == CHINA_TELECOM_MAINLAND_MCC &&
            (mcc == CHINA_TELECOM_MAINLAND_MCC || mcc == CHINA_TELECOM_MACCO_MCC ||
             mcc == JAP_MCC || mcc == KOR_MCC)) {
            logD(RFX_LOG_TAG, "getNwsModeByPlmn, plmn=[%s]  ret: [NWS_MODE_CDMALTE]", plmn.string());
            return NWS_MODE_CDMALTE;
        } else {
            return RpIrStrategyOP095M::getNwsModeByPlmn(plmn);
        }
    } else {
        logD(RFX_LOG_TAG, "getNwsModeByPlmn, plmn=[%s]  ret: [NWS_MODE_CSFB]", plmn.string());
        return NWS_MODE_CSFB;
    }
}

