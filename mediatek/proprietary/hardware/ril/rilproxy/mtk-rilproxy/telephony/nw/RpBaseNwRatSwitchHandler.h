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

#ifndef __RP_BASE_NW_RAT_SWITCH_HANDLER__
#define __RP_BASE_NW_RAT_SWITCH_HANDLER__

typedef enum SVLTE_PROJ_RYPE {
    SVLTE_PROJ_DC_3M = 3,
    SVLTE_PROJ_DC_4M = 4,
    SVLTE_PROJ_DC_5M = 5,
    SVLTE_PROJ_DC_6M = 6,
    SVLTE_PROJ_SC_3M = 103,
    SVLTE_PROJ_SC_4M = 104,
    SVLTE_PROJ_SC_5M = 105,
    SVLTE_PROJ_SC_6M = 106,
} SVLTE_PROJ_TYPE;
/*****************************************************************************
 * Include
 *****************************************************************************/
#include "RfxObject.h"
#include "RfxAction.h"
#include "RfxLog.h"
#include "ril.h"
#include "RfxStatusDefs.h"
#include "RfxRilAdapter.h"
#include "RpNwRatController.h"
#include "power/RadioConstants.h"
#include <cutils/properties.h>

#define LOG_TAG "RpBaseNwRatSwitchHandler"
#define LOG_BUF_SIZE 1024

/*****************************************************************************
 * Class RpBaseNwRatSwitchHandler
 *****************************************************************************/
 
/*
 * Class RpBaseNwRatSwitchHandler, 
 * prototype of base nw rat switch handler class, define the virtual functions which sub-class
 * need to override, implement the general behaviors of nw rat switch.
 */
class RpBaseNwRatSwitchHandler : public RfxObject {
    RFX_DECLARE_CLASS(RpBaseNwRatSwitchHandler); // Required: declare this class

public:
    RpBaseNwRatSwitchHandler();
    RpBaseNwRatSwitchHandler(RpNwRatController* nwRatController);
    virtual ~RpBaseNwRatSwitchHandler();

public:
    // Virtual functions.
    /* Handle request and response for RILJ. */
    virtual void requestGetPreferredNetworkType(const sp<RfxMessage>& message) {};
    virtual void responseGetPreferredNetworkType(const sp<RfxMessage>& response) {};
    virtual void requestSetPreferredNetworkType(const int prefNwType, const sp<RfxMessage>& message) {};
    virtual void responseSetPreferredNetworkType(const sp<RfxMessage>& response) {};
    virtual void requestSetEvdoMode() {};
    virtual void responseSetEvdoMode(const sp<RfxMessage>& response) {};
    virtual void requestSetRatMode(const int radioMode, const int preRatMode, const int newRatMode, const int preRoamingMode, const int newRoamingMode, bool is3gCdmaSim) {};
    virtual void responseSetRatMode(const sp<RfxMessage>& response) {};
    /* Handle network RAT switch. */
    virtual void doNwRatSwitch(const int prefNwType, const NwsMode nwsMode, const RatSwitchCaller& ratSwitchCaller, const sp<RfxAction>& action, const sp<RfxMessage>& message) {};
    virtual int filterPreferNwTypeForTdCard(const int prefNwType){ return 0;};
    void requestGetVoiceRadioTech(const sp<RfxMessage>& message);
    void responseGetVoiceRadioTech(const sp<RfxMessage>& response);
    void updatePhone(const int prefNwType);
    int techFromPrefNwType(const int prefNwType);

protected:
    void requestGetPreferredNetworkType(const RILD_RadioTechnology_Group radioTechType, const sp<RfxMessage>& msg);
    void requestSetPreferredNetworkType(const RILD_RadioTechnology_Group radioTechType, const int prefNwType, const sp<RfxMessage>& msg);

public:
    CombinedPreferredNwType getCombinedPreferredNwType(const int nwType) {
        CombinedPreferredNwType type;
        type.gsm_preferred_nw_type = -1;
        type.c2k_preferred_nw_type = -1;

        switch (nwType) {
        case PREF_NET_TYPE_GSM_WCDMA:
        case PREF_NET_TYPE_GSM_ONLY:
        case PREF_NET_TYPE_WCDMA:
        case PREF_NET_TYPE_GSM_WCDMA_AUTO:
        case PREF_NET_TYPE_LTE_GSM_WCDMA:
        case PREF_NET_TYPE_LTE_ONLY:
        case PREF_NET_TYPE_LTE_TDD_ONLY:
        case PREF_NET_TYPE_LTE_WCDMA:
        case PREF_NET_TYPE_LTE_GSM:
            type.gsm_preferred_nw_type = nwType;
            break;
        case PREF_NET_TYPE_CDMA_EVDO_AUTO:
        case PREF_NET_TYPE_CDMA_ONLY:
        case PREF_NET_TYPE_EVDO_ONLY:
            type.c2k_preferred_nw_type = -1;
            break;
        case PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO:
            type.gsm_preferred_nw_type = PREF_NET_TYPE_GSM_WCDMA_AUTO;
            type.c2k_preferred_nw_type = PREF_NET_TYPE_CDMA_EVDO_AUTO;
            break;
        case PREF_NET_TYPE_LTE_CDMA_EVDO:
            type.gsm_preferred_nw_type = PREF_NET_TYPE_LTE_ONLY;
            type.c2k_preferred_nw_type = PREF_NET_TYPE_CDMA_EVDO_AUTO;
            break;
        case PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA:
            type.gsm_preferred_nw_type = PREF_NET_TYPE_LTE_GSM_WCDMA;
            type.c2k_preferred_nw_type = PREF_NET_TYPE_CDMA_EVDO_AUTO;
            break;
        }

        return type;
    }

    bool needRadioOn(const RILD_RadioTechnology_Group radioTechType, const int prefNwType) {
        if (radioTechType == RADIO_TECH_GROUP_C2K) {
            return needCdmaRadioOn(prefNwType);
        } else if (radioTechType == RADIO_TECH_GROUP_GSM) {
            return needGsmRadioOn(prefNwType);
        } else {
            return false;
        }
    }

    bool needGsmRadioOn(const int prefNwType) {
        logD(LOG_TAG, "needGsmRadioOn(), prefNwType = %d", prefNwType);
        switch (prefNwType) {
        case PREF_NET_TYPE_GSM_WCDMA:
        case PREF_NET_TYPE_GSM_ONLY:
        case PREF_NET_TYPE_WCDMA:
        case PREF_NET_TYPE_GSM_WCDMA_AUTO:
        case PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO:
        case PREF_NET_TYPE_LTE_CDMA_EVDO:
        case PREF_NET_TYPE_LTE_GSM_WCDMA:
        case PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA:
        case PREF_NET_TYPE_LTE_ONLY:
        case PREF_NET_TYPE_LTE_TDD_ONLY:
        case PREF_NET_TYPE_LTE_WCDMA:
        case PREF_NET_TYPE_LTE_GSM:
            return true;
        case PREF_NET_TYPE_CDMA_EVDO_AUTO:
        case PREF_NET_TYPE_CDMA_ONLY:
        case PREF_NET_TYPE_EVDO_ONLY:
            return false;
        }
        return false;
    }

    virtual bool needCdmaRadioOn(const int prefNwType) {
        logD(LOG_TAG, "needCdmaRadioOn(), prefNwType = %d", prefNwType);
        switch (prefNwType) {
        case PREF_NET_TYPE_CDMA_EVDO_AUTO:
        case PREF_NET_TYPE_CDMA_ONLY:
        case PREF_NET_TYPE_EVDO_ONLY:
        case PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO:
        case PREF_NET_TYPE_LTE_CDMA_EVDO:
        case PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA:
            return true;
        case PREF_NET_TYPE_GSM_WCDMA:
        case PREF_NET_TYPE_GSM_ONLY:
        case PREF_NET_TYPE_WCDMA:
        case PREF_NET_TYPE_GSM_WCDMA_AUTO:
        case PREF_NET_TYPE_LTE_GSM_WCDMA:
        case PREF_NET_TYPE_LTE_ONLY:
        case PREF_NET_TYPE_LTE_TDD_ONLY:
        case PREF_NET_TYPE_LTE_WCDMA:
        case PREF_NET_TYPE_LTE_GSM:
            return false;
        }
        return false;
    }

    void logD(const char *tag, const char *fmt, ...) const {
        va_list ap;
        char buf[LOG_BUF_SIZE];
        String8 tagString = String8::format("%s[%d]", tag, mSlotId);
        va_start(ap, fmt);
        vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
        va_end(ap);

        __android_log_buf_write(LOG_ID_RADIO, ANDROID_LOG_DEBUG, tagString.string(), buf);
    }

    int getSvlteProjectType() {
        int type = -1;
        char property_value_mode[PROPERTY_VALUE_MAX] = { 0 };
        char property_value_nw_sel[PROPERTY_VALUE_MAX] = { 0 };

        property_get("ro.mtk.c2k.om.mode", property_value_mode, "cllwtg");
        property_get("ro.mtk_c2k_om_nw_sel_type", property_value_nw_sel, "0");
        if (strcmp(property_value_nw_sel, "1") == 0) {
            if (strcmp(property_value_mode, "cllwtg") == 0) {
                type = SVLTE_PROJ_SC_6M;
            } else if (strcmp(property_value_mode, "cllwg") == 0) {
                type = SVLTE_PROJ_SC_5M;
            } else if (strcmp(property_value_mode, "cllg") == 0) {
                type = SVLTE_PROJ_SC_4M;
            } else if (strcmp(property_value_mode, "cwg") == 0) {
                type = SVLTE_PROJ_SC_3M;
            }
        } else {
            if (strcmp(property_value_mode, "cllwtg") == 0) {
                type = SVLTE_PROJ_DC_6M;
            } else if (strcmp(property_value_mode, "cllwg") == 0) {
                type = SVLTE_PROJ_DC_5M;
            } else if (strcmp(property_value_mode, "cllg") == 0) {
                type = SVLTE_PROJ_DC_4M;
            } else if (strcmp(property_value_mode, "cwg") == 0) {
                type = SVLTE_PROJ_DC_3M;
            }
        }
        logD(LOG_TAG, " getSvlteWwopType(): %s %s %d", property_value_mode, property_value_nw_sel, type);
        return type;
    }

protected:
    // Variables.
    RpNwRatController* mNwRatController;
    int mSlotId;
};

#endif /* __RP_BASE_NW_RAT_SWITCH_HANDLER__ */

