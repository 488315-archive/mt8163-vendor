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

#ifndef __RP_CDMALTE_NW_RAT_SWITCH_HANDLER__
#define __RP_CDMALTE_NW_RAT_SWITCH_HANDLER__

/*****************************************************************************
 * Include
 *****************************************************************************/

#include "RpCdmaNwRatSwitchHandler.h"

#define LOG_TAG "RpCdmaLteNwRatSwitchHandler"

typedef enum {
    SVLTE_RAT_MODE_INVALID = -1,
    SVLTE_RAT_MODE_4G = 0,
    SVLTE_RAT_MODE_3G = 1,
    SVLTE_RAT_MODE_4G_DATA_ONLY = 2
} SVLTE_RAT_MODE;

typedef enum RadioTechMode {
    RADIO_TECH_MODE_UNKNOWN  = 1,
    RADIO_TECH_MODE_CSFB  = 2,
    RADIO_TECH_MODE_SVLTE = 3
} RADIO_TECH_MODE;

/*****************************************************************************
 * Class RpCdmaLteNwRatSwitchHandler
 *****************************************************************************/

/*
 * Class RpCdmaLteNwRatSwitchHandler,
 * implement the general behaviors of cdma nw rat switch for CDMA with LTE mode.
 */
class RpCdmaLteNwRatSwitchHandler: public RpCdmaNwRatSwitchHandler {
RFX_DECLARE_CLASS(RpCdmaLteNwRatSwitchHandler); // Required: declare this class

public:
    RpCdmaLteNwRatSwitchHandler();
    RpCdmaLteNwRatSwitchHandler(RpNwRatController* nwRatController);
    virtual ~RpCdmaLteNwRatSwitchHandler();

public:
    /* Handle request and response for RILJ. */
    void requestGetPreferredNetworkType(const sp<RfxMessage>& message);
    void responseGetPreferredNetworkType(const sp<RfxMessage>& response);
    void requestSetPreferredNetworkType(const int prefNwType);
    void responseSetPreferredNetworkType(const sp<RfxMessage>& response);
    void requestSetEvdoMode(const int prefNwType);
    void responseSetEvdoMode(const sp<RfxMessage>& response);
    void requestSetRatMode();
    void responseSetRatMode(const sp<RfxMessage>& response);
    /* Handle network RAT switch. */
    void doNwRatSwitch(const int prefNwType, const NwsMode nwsMode, const RatSwitchCaller& ratSwitchCaller, const sp<RfxAction>& action, const sp<RfxMessage>& message);
    /* Try to override this function in child */
    bool needCdmaRadioOn(const int prefNwType, const NwsMode nwsMode);

private:
    void resetRadioPowerOff();
    void onResetRadioPowerOffChanged(RfxStatusKeyEnum key, RfxVariant old_value, RfxVariant new_value);
    void onRadioPowerOn(RfxStatusKeyEnum key, RfxVariant old_value, RfxVariant new_value);
    int radioPowerOn();
    SVLTE_RAT_MODE setRatModeFromPrefType(const int prefNwType);
    RADIO_TECH_MODE setRadioTechModeFromPrefType();
    void onNwRatSwitchDone();
    bool canRadioOn();

private:
    void combinePreferredNwType(CombinedPreferredNwType combinedPreferredNwType);
    void sendFailResponseToSMC();

private:
    int mPrefNwType;
    NwsMode mNwsMode;
    RatSwitchCaller mRatSwitchCaller;
    int mSugggestedResetRadioCapablity;
    int mSugggestedOnRadioCapablity;
    sp<RfxAction> mAction;
    sp<RfxMessage> mMessage;
};

#endif /* __RP_CDMALTE_NW_RAT_SWITCH_HANDLER__ */
