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
#include <cutils/properties.h>
#include "stdio.h"
#include "RfxLog.h"
#include "RpCardModeControllerFactory.h"
#include "RpCdmaLteDefs.h"
#include "RpOMCardModeController.h"
#include "RpOP09CardModeController.h"
#include "RpOP01CardModeController.h"

/*****************************************************************************
 * Class RpCardModeControllerFactory
 *****************************************************************************/

RpCardModeControllerFactory::RpCardModeControllerFactory() {
}

RpCardModeControllerFactory::~RpCardModeControllerFactory() {
}

RpBaseCardModeController* RpCardModeControllerFactory::getRpCardModeController() {
    char property_value[PROPERTY_VALUE_MAX] = { 0 };
    char forCtTestPropertyValue[PROPERTY_VALUE_MAX] = { 0 };
    property_get("ro.operator.optr", property_value, OPERATOR_OM);
    property_get("persist.sys.forcttestcard", forCtTestPropertyValue, "0");
    RFX_LOG_D(RP_CDMALTE_MODE_TAG, "[getRpCardModeController] >>> operator: %s; forcttestcard: %s",
        property_value, forCtTestPropertyValue);
    if (strncmp(forCtTestPropertyValue, "1", 1) == 0) {
        return RpOMCardModeController::getInstance();
    } else if (strncmp(property_value, OPERATOR_OP09, strlen(OPERATOR_OP09)) == 0) {
        memset(property_value, 0, sizeof(property_value));
        property_get("ro.operator.seg", property_value, "");
        RFX_LOG_D(RP_CDMALTE_MODE_TAG,
                "[getRpCardModeController] >>> operator_seg: %s", property_value);
        if (strncmp(property_value, SPEC_OP09_A, strlen(SPEC_OP09_A)) == 0) {
            return RpOP09CardModeController::getInstance();
        }
    } else if (strncmp(property_value, OPERATOR_OM, strlen(OPERATOR_OM)) == 0) {
        return RpOMCardModeController::getInstance();
    } else if (strncmp(property_value, OPERATOR_OP01, strlen(OPERATOR_OP01)) == 0) {
        return RpOP01CardModeController::getInstance();
    }

    RFX_LOG_D(RP_CDMALTE_MODE_TAG,
                "[getRpCardModeController] >>> Default is RpOMCardModeController");
    return RpOMCardModeController::getInstance();
}
