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

#ifndef __RP_DATA_UTILS_H__
#define __RP_DATA_UTILS_H__

/*****************************************************************************
 * Include
 *****************************************************************************/
#include <cutils/properties.h>
#include "power/RadioConstants.h"
#include "RfxStatusDefs.h"
#include "RfxController.h"
#include "RpDataConnectionInfo.h"

#define RP_DC_UTIL_LOG_TAG  "RP_DC_UTIL"
#define RP_DATA_PROPERTY_4G_SIM "persist.radio.simswitch"
#define OPERATOR_OP09 "OP09"
#define SEGDEFAULT "SEGDEFAULT"
#define CTNET "ctnet"
#define RP_DATA_PROPERTY_IA "ril.c2kirat.ia"

const char PROPERTY_ICCID_SIM[4][25] = {
    "ril.iccid.sim1",
    "ril.iccid.sim2",
    "ril.iccid.sim3",
    "ril.iccid.sim4",
};

/*****************************************************************************
 * Class RpDataUtils
 *****************************************************************************/

class RpDataUtils {
public:
    RpDataUtils();
    virtual ~RpDataUtils();

    static const char* requestToString(int reqId);
    static const char* urcToString(int reqId);
    static const char* radioGroupToString(int radioGroupId);

    static inline RILD_RadioTechnology_Group getPsType(RfxController* context);
    static inline bool isRadioUnAvailable(RfxController* context);
    static inline int getActiveCdmaSlot(RfxController* context);
    static inline RIL_RADIO_CAPABILITY getRadioCapability(RfxController* context);
    static inline bool isCdmaLteMode(RfxController* context);
    static inline bool isSvlteSupport();
    static inline bool isSrlteSupport();
    static inline bool isOP09ASupport();
    /*
     * major slot is the one that with 4G capability.
     * return index from 0.
     */
    static int getMajorSlot();
    static bool isNonOP09CdmaCard(const int cardType);
    static bool isOP09Cdma4GCard(const int cardType);
    static Parcel* createParcelFromMessage(const sp<RfxMessage>& message);
    static Vector<RpDataConnectionInfo*>* parseDataConnectionInfo(Parcel* parcel);
    static bool syncDataConnectionApn(Vector<RpDataConnectionInfo*>* old, Vector<RpDataConnectionInfo*>* current);
    static void freeDataList(Vector<RpDataConnectionInfo*>* list);
    // slotId index from 0.
    static void storeIaProperty(int slotId, String16 apn);
};

inline RILD_RadioTechnology_Group RpDataUtils::getPsType(RfxController* context) {
    return (RILD_RadioTechnology_Group)context->getStatusManager()->getIntValue(RFX_STATUS_KEY_DATA_TYPE,
                                                                                RADIO_TECH_GROUP_GSM);
}

inline bool RpDataUtils::isRadioUnAvailable(RfxController* context) {
    int state = context->getStatusManager()->getIntValue(RFX_STATUS_KEY_RADIO_STATE,
                                                        (int)RADIO_STATE_UNAVAILABLE);
    return state == (int)RADIO_STATE_UNAVAILABLE;
}

inline int RpDataUtils::getActiveCdmaSlot(RfxController* context) {
    int activeCdmaSlot = context->getNonSlotScopeStatusManager()->getIntValue(
            RFX_STATUS_KEY_ACTIVE_CDMALTE_MODE_SLOT, CSFB_ON_SLOT);
    RFX_LOG_D(RP_DC_UTIL_LOG_TAG , "getActiveCdmaSlot: activeCdmaSlot = %d.", activeCdmaSlot);
    return activeCdmaSlot;
}

inline RIL_RADIO_CAPABILITY RpDataUtils::getRadioCapability(RfxController* context) {
    return (RIL_RADIO_CAPABILITY)context->getStatusManager()->getIntValue(RFX_STATUS_KEY_RADIO_CAPABILITY,
                                                                            RIL_CAPABILITY_NONE);
}

inline bool RpDataUtils::isCdmaLteMode(RfxController* context) {
    // CDMALTE mode means it is a CDMA card and 4G capability is on the slot.
    if (context->getSlotId() == RpDataUtils::getActiveCdmaSlot(context) &&
        context->getSlotId() == RpDataUtils::getMajorSlot()) {
        return true;
    }
    return false;
}

inline bool RpDataUtils::isSvlteSupport() {
    int svlteSupport = 0;
    char property_value[PROPERTY_VALUE_MAX] = { 0 };
    property_get("ro.mtk_svlte_support", property_value, "0");
    svlteSupport = atoi(property_value);
    RFX_LOG_D(RP_DC_UTIL_LOG_TAG , "isSvlteSupport: %d", svlteSupport);
    return (svlteSupport == 1);
}

inline bool RpDataUtils::isSrlteSupport() {
    int srlteSupport = 0;
    char property_value[PROPERTY_VALUE_MAX] = { 0 };
    property_get("ro.mtk_srlte_support", property_value, "0");
    srlteSupport = atoi(property_value);
    RFX_LOG_D(RP_DC_UTIL_LOG_TAG , "isSrlteSupport: %d", srlteSupport);
    return (srlteSupport == 1);
}

inline bool RpDataUtils::isOP09ASupport() {
    char optr_value[PROPERTY_VALUE_MAX] = { 0 };
    char seg_value[PROPERTY_VALUE_MAX] = { 0 };
    property_get("ro.operator.optr", optr_value, "0");
    property_get("ro.operator.seg", seg_value, "0");
    if (strcmp(optr_value, OPERATOR_OP09) == 0 &&
        strcmp(seg_value, SEGDEFAULT) == 0) {
        RFX_LOG_D(RP_DC_UTIL_LOG_TAG , "project is OP09A.");
        return true;
    }
    return false;
}

#endif /* __RP_DATA_UTILS_H__ */
