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
 /*
  * rfx_default_dest_utils.h
  *
  *  Created on: 2015/09/03
  *  Author: MTK10602
  *
  */
#include "RfxDefaultDestUtils.h"

static RequestDestInfo s_rfx_request_dest[] = {
    #include "rfx_ril_commands.h"
};
static RequestDestInfo s_rfx_mtk_gsm_request_dest[] = {
    #include "rfx_mtk_commands.h"
};
#define LOG_TAG "RfxDefaultDestUtils"

RfxDefaultDestUtils::RfxDefaultDestUtils() {
}

RfxDefaultDestUtils::~RfxDefaultDestUtils() {
}

RILD_RadioTechnology_Group RfxDefaultDestUtils::getDefaultDest(int request) {
    // RFX_LOG_D(LOG_TAG, "[RFX] getDispatchInfo");
    RequestDestInfo *pInfo;
    if (request >= 1 && request <= (int32_t)NUM_ELEMS(s_rfx_request_dest)) {
        // RFX_LOG_D(LOG_TAG, "[RFX] use s_rfx_request_dest");
        pInfo = &(s_rfx_request_dest[request]);
        // RFX_LOG_D(LOG_TAG, "[RFX] pDisInfo->reqInfo->dest = %d, pDisInfo->reqInfo->request = %d",
        //        pInfo->dest, pInfo->request);
    } else if (request >= RIL_REQUEST_VENDOR_BASE && (request <= RIL_REQUEST_VENDOR_BASE +
                (int32_t)NUM_ELEMS(s_rfx_mtk_gsm_request_dest))) {
        // RFX_LOG_D(LOG_TAG, "[RFX] use s_rfx_mtk_gsm_request_dest");
        pInfo = &(s_rfx_mtk_gsm_request_dest[request-RIL_REQUEST_VENDOR_BASE]);
        // RFX_LOG_D(LOG_TAG, "[RFX] pDisInfo->reqInfo->dest = %d, pDisInfo->reqInfo->request = %d",
        //        pInfo->dest, pInfo->request);
    } else {
        RFX_LOG_D(LOG_TAG, "Should not be here");
        return RADIO_TECH_GROUP_GSM;
    }
    return pInfo->dest;
}
