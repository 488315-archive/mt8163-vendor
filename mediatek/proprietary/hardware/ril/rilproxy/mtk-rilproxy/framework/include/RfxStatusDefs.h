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
 * File name:  RfxStatusDef.h
 * Author: Jun Liu (MTK80064)
 * Description:
 *  Define the keys of shared status.
 */

#ifndef __RFX_STATUS_DEFS_H__
#define __RFX_STATUS_DEFS_H__

/*****************************************************************************
 * Enum
 *****************************************************************************/

enum RfxStatusKeyEnum {
    RFX_STATUS_KEY_START,
    /*Please add your keys below this line*/

/**
 * The card type of SIM card.
 * value type : int
 * RFX_CARD_TYPE_SIM  0x01
 * RFX_CARD_TYPE_USIM 0x02
 * RFX_CARD_TYPE_CSIM 0x04
 * RFX_CARD_TYPE_RUIM 0x08
 */
    RFX_STATUS_KEY_CARD_TYPE,
/**
 * The card type of CDMA card.
 * value type : int
 * #define UIM_CARD  1  // cdma only card but not CT card
 * #define UIM_SIM_CARD  3  // cdma dual mode card but not CT card
 * #define UNKOWN_CARD  4  //unkonw card
 * #define CT_3G_UIM_CARD  5  // CT sigle mode card
 * #define CT_UIM_SIM_CARD  6  // CT dual mode card
 * #define NEED_TO_INPUT_PIN  7  // pin locked card
 * #define CT_4G_UICC_CARD  8  // CT 4G dual mode card
 * #define NOT_CT_UICC_CARD  9  // 4G dual mode card but not CT card
 * #define LOCKED_CARD  18  // card is locked by modem
 * #define IVSR_LOST  19  // invalid sim recovery
 * #define CARD_NOT_INSERTED  255  // no card inserted
 */
    RFX_STATUS_KEY_CDMA_CARD_TYPE,
/**
 * CDMA card ready after C2K modem sends ciev 109.
 * value type : bool
 * false : cdma card is not ready. It is default value.
 * true : cdma card is ready.
 */
    RFX_STATUS_KEY_CDMA_CARD_READY,
/**
 * Ready to read CDMA card file after C2K modem sends ciev 107.
 * value type : bool
 * false : cdma card file is not ready to read. It is default value.
 * true : cdma card file is ready to read.
 */
    RFX_STATUS_KEY_CDMA_FILE_READY,
/**
 * Ct 3g dualmode card flag.
 * value type : bool
 * false : it is not a ct 3g dualmode card. It is default value.
 * true : it is a ct 3g dualmode card.
 */
    RFX_STATUS_KEY_CT3G_DUALMODE_CARD,
    RFX_STATUS_KEY_SERVICE_STATE,
    RFX_STATUS_KEY_VOICE_TYPE,
    RFX_STATUS_KEY_DATA_TYPE,
/**
 * This is used to check the modem SIM task is ready or not.
 * NOTE: It is not SIM_STATE_CHANGED READY!!
 * value type : bool
 * false: modem SIM task is not ready. It is also default value
 * true: modem SIM task is ready.
 */
    RFX_STATUS_KEY_MODEM_SIM_TASK_READY,
    RFX_STATUS_KEY_SUGGESTED_RADIO_CAPABILITY,
    RFX_STATUS_KEY_RADIO_CAPABILITY,
    RFX_STATUS_KEY_RADIO_STATE,
    RFX_STATUS_KEY_REQUEST_RADIO_POWER,
/**
 * Active cdmalte mode slot id.
 * NOTE: Belong to non slot controller, use getNonSlotScopeStatusManager().
 * value type : int
 * CSFB_ON_SLOT
 * CDMALTE_ON_SLOT_0
 * CDMALTE_ON_SLOT_1
 */
    RFX_STATUS_KEY_ACTIVE_CDMALTE_MODE_SLOT,
/**
 * Slot id which connected to c2k rild.
 * NOTE: Belong to non slot controller, use getNonSlotScopeStatusManager().
 * value type : int
 * SIM_ID_1
 * SIM_ID_2
 */
    RFX_STATUS_KEY_CDMA_SOCKET_SLOT,
/**
 * mode controller switch mode state.
 * NOTE: Belong to non slot controller, use getNonSlotScopeStatusManager().
 * value type : int
 * MODE_SWITCH_START
 * MODE_SWITCH_FINISH
 */
    RFX_STATUS_KEY_MODE_SWITCH,
    RFX_STATUS_KEY_RAT_SWITCH_DONE, //

/**
 * rat controller  preferred network type.
 * value type : int
 * PREF_NET_TYPE_GSM_WCDMA                   GSM/WCDMA (WCDMA preferred)
 * PREF_NET_TYPE_GSM_ONLY                    GSM only
 * PREF_NET_TYPE_WCDMA                       WCDMA
 * PREF_NET_TYPE_GSM_WCDMA_AUTO              GSM/WCDMA (auto mode, according to PRL)
 * PREF_NET_TYPE_CDMA_EVDO_AUTO              CDMA and EvDo (auto mode, according to PRL)
 * PREF_NET_TYPE_CDMA_ONLY                   CDMA only
 * PREF_NET_TYPE_EVDO_ONLY                   EvDo only
 * PREF_NET_TYPE_GSM_WCDMA_CDMA_EVDO_AUTO    GSM/WCDMA, CDMA, and EvDo (auto mode, according to PRL)
 * PREF_NET_TYPE_LTE_CDMA_EVDO               LTE, CDMA and EvDo
 * PREF_NET_TYPE_LTE_GSM_WCDMA               LTE, GSM/WCDMA
 * PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA     LTE, CDMA, EvDo, GSM/WCDMA
 * PREF_NET_TYPE_LTE_ONLY                    LTE only
 * PREF_NET_TYPE_LTE_WCDMA                   LTE/WCDMA
 * PREF_NET_TYPE_LTE_GSM                     LTE/GSM
 * PREF_NET_TYPE_LTE_TDD_ONLY                LTE only
 */
    RFX_STATUS_KEY_PREFERRED_NW_TYPE,

/**
 * rat controller  mode state.
 * value type : int
 *  NWS_MODE_UNKNOWN
 *  NWS_MODE_CDMALTE
 *  NWS_MODE_CSFB
 */
    RFX_STATUS_KEY_NWS_MODE,

/**
 * Indicate the voice call count
 */
    RFX_STATUS_KEY_VOICE_CALL_COUNT,
/**
 * The SIM state. It will be updated upon receiving the response of GET_SIM_STATUS.
 * And be set as NOT_READY when RADIO_UNAVAILABLE.
 * value type : int
 * RFX_SIM_STATE_NOT_READY  0
 * RFX_SIM_STATE_READY 1
 * RFX_SIM_STATE_LOCKED 2
 * RFX_SIM_STATE_ABSENT 3
 */
    RFX_STATUS_KEY_SIM_STATE,
/**
 * The IMSI of GSM application.
 * value type : String8
 * "" or IMSI
 */
    RFX_STATUS_KEY_GSM_IMSI,
/**
 * The IMSI of CDMA application.
 * value type : String8
 * "" or IMSI
 */
    RFX_STATUS_KEY_C2K_IMSI,
/**
 * Slot data connection status is changed.
 * status will be notified only if status is changed.
 * value type : int
 * DISCONNECTED: no active data connection exist.
 * CONNECTED: at least one data connection exist.
 */
    RFX_STATUS_KEY_DATA_CONNECTION,

    /**
     * Get socket connected state
     * value type: RfxSocketState (reference to RfxSocketState.h)
     */
    RFX_STATUS_KEY_SOCKET_STATE,

    /**
     * The world mode switching state
     * value type : int
     * Switching:   0
     * Switch done: 1
     * Switch done but modem failure: -1
     */
    RFX_STATUS_KEY_WORLD_MODE_STATE,

    /*Please add your keys above this line*/
    RFX_STATUS_KEY_END_OF_ENUM
};

#define RFX_CARD_TYPE_SIM  0x01
#define RFX_CARD_TYPE_USIM 0x02
#define RFX_CARD_TYPE_CSIM 0x04
#define RFX_CARD_TYPE_RUIM 0x08

#define RFX_SIM_STATE_NOT_READY 0
#define RFX_SIM_STATE_READY 1
#define RFX_SIM_STATE_LOCKED 2
#define RFX_SIM_STATE_ABSENT 3



/* CDMA card type */
#define UIM_CARD  1
#define UIM_SIM_CARD  3
#define UNKOWN_CARD  4
#define CT_3G_UIM_CARD  5
#define CT_UIM_SIM_CARD  6
#define NEED_TO_INPUT_PIN  7
#define CT_4G_UICC_CARD  8
#define NOT_CT_UICC_CARD  9
#define LOCKED_CARD  18
#define IVSR_LOST  19
#define CARD_NOT_INSERTED  255

/* SIM_ID*/
#define SIM_ID_1 (0)
#define SIM_ID_2 (1)
#define SIM_ID_3 (2)
#define SIM_ID_4 (3)

/* ACTIVE_CDMAMODE_SLOT_ID*/
#define CSFB_ON_SLOT             (-1)
#define CDMALTE_ON_SLOT_0        SIM_ID_1
#define CDMALTE_ON_SLOT_1        SIM_ID_2

/* MODE_SWITCH_STATE*/
#define MODE_SWITCH_START        (1)
#define MODE_SWITCH_FINISH       (0)

/* DATA CONNECTION STATE*/
#define DATA_STATE_DISCONNECTED        (0)
#define DATA_STATE_CONNECTED           (1)

/* For RAT SWITCH*/
typedef enum {
    /* Rat switch for mode controller */
    RAT_SWITCH_INIT = 0,
    /* RAT switch done for NWS */
    RAT_SWITCH_NWS = 1,
    /* RAT switch done for RIL Request and signal */
    RAT_SWITCH_NORMAL = 2,
    /* Rat switch for some restricted mode. ex: ECC redial */
    RAT_SWITCH_RESTRICT = 3
} RatSwitchCaller;

#define RFX_STATUS_DEFAULT_VALUE_ENTRY(key, value) {key, #key, value}

#define RFX_STATUS_DEFAULT_VALUE_TABLE_BEGIN                       \
        const RfxStatusManager::StatusDefaultValueEntry RfxStatusManager::s_default_value_table[] = {

#define RFX_STATUS_DEFAULT_VALUE_TABLE_END                         \
        RFX_STATUS_DEFAULT_VALUE_ENTRY(RFX_STATUS_KEY_END_OF_ENUM, RfxVariant())}

#define RFX_STATUS_DECLARE_DEFAULT_VALUE_TABLE                     \
        static const StatusDefaultValueEntry s_default_value_table[]

#define RFX_STATUS_IMPLEMENT_DEFAULT_VALUE_TABLE                                                    \
    RFX_STATUS_DEFAULT_VALUE_TABLE_BEGIN                                                            \
        /*Please add your default value below this line*/                                           \
        /*NOTE. below every line should be ended by "\" */                                          \
        RFX_STATUS_DEFAULT_VALUE_ENTRY(RFX_STATUS_KEY_CARD_TYPE, RfxVariant(-1)),                     \
        RFX_STATUS_DEFAULT_VALUE_ENTRY(RFX_STATUS_KEY_CDMA_CARD_TYPE, RfxVariant(-1)),               \
        RFX_STATUS_DEFAULT_VALUE_ENTRY(RFX_STATUS_KEY_CDMA_CARD_READY, RfxVariant(false)),               \
        RFX_STATUS_DEFAULT_VALUE_ENTRY(RFX_STATUS_KEY_CDMA_FILE_READY, RfxVariant(false)),               \
        RFX_STATUS_DEFAULT_VALUE_ENTRY(RFX_STATUS_KEY_CT3G_DUALMODE_CARD, RfxVariant(false)),               \
        RFX_STATUS_DEFAULT_VALUE_ENTRY(RFX_STATUS_KEY_SERVICE_STATE, RfxVariant()),                 \
        RFX_STATUS_DEFAULT_VALUE_ENTRY(RFX_STATUS_KEY_VOICE_TYPE, RfxVariant()),                    \
        RFX_STATUS_DEFAULT_VALUE_ENTRY(RFX_STATUS_KEY_DATA_TYPE, RfxVariant()),                     \
        RFX_STATUS_DEFAULT_VALUE_ENTRY(RFX_STATUS_KEY_MODEM_SIM_TASK_READY, RfxVariant(false)),          \
        RFX_STATUS_DEFAULT_VALUE_ENTRY(RFX_STATUS_KEY_SUGGESTED_RADIO_CAPABILITY, RfxVariant()),    \
        RFX_STATUS_DEFAULT_VALUE_ENTRY(RFX_STATUS_KEY_RADIO_CAPABILITY, RfxVariant(0)),              \
        RFX_STATUS_DEFAULT_VALUE_ENTRY(RFX_STATUS_KEY_RADIO_STATE, RfxVariant()),                   \
        RFX_STATUS_DEFAULT_VALUE_ENTRY(RFX_STATUS_KEY_REQUEST_RADIO_POWER, RfxVariant(false)),      \
        RFX_STATUS_DEFAULT_VALUE_ENTRY(RFX_STATUS_KEY_MODE_SWITCH, RfxVariant(0)), \
        RFX_STATUS_DEFAULT_VALUE_ENTRY(RFX_STATUS_KEY_CDMA_SOCKET_SLOT, RfxVariant(0)),              \
        RFX_STATUS_DEFAULT_VALUE_ENTRY(RFX_STATUS_KEY_ACTIVE_CDMALTE_MODE_SLOT, RfxVariant(0)),      \
        RFX_STATUS_DEFAULT_VALUE_ENTRY(RFX_STATUS_KEY_RAT_SWITCH_DONE, RfxVariant()),               \
        RFX_STATUS_DEFAULT_VALUE_ENTRY(RFX_STATUS_KEY_SIM_STATE, RfxVariant(0)),               \
        RFX_STATUS_DEFAULT_VALUE_ENTRY(RFX_STATUS_KEY_GSM_IMSI, RfxVariant(String8(""))),      \
        RFX_STATUS_DEFAULT_VALUE_ENTRY(RFX_STATUS_KEY_C2K_IMSI, RfxVariant(String8(""))),      \
        RFX_STATUS_DEFAULT_VALUE_ENTRY(RFX_STATUS_KEY_DATA_CONNECTION, RfxVariant(0)),      \
        RFX_STATUS_DEFAULT_VALUE_ENTRY(RFX_STATUS_KEY_SOCKET_STATE, RfxVariant(RfxSocketState())),  \
        RFX_STATUS_DEFAULT_VALUE_ENTRY(RFX_STATUS_KEY_NWS_MODE, RfxVariant(RfxVariant(0))),  \
        RFX_STATUS_DEFAULT_VALUE_ENTRY(RFX_STATUS_KEY_PREFERRED_NW_TYPE, RfxVariant(RfxVariant(-1))),  \
        RFX_STATUS_DEFAULT_VALUE_ENTRY(RFX_STATUS_KEY_WORLD_MODE_STATE, RfxVariant(1)), \
        /*Please add your default value above this line*/                                           \
    RFX_STATUS_DEFAULT_VALUE_TABLE_END

#endif /* __RFX_STATUS_DEFS_H__ */
