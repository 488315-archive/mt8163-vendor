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

#ifndef __RP_SIM_CONTROLLER_H__
#define __RP_SIM_CONTROLLER_H__

/*****************************************************************************
 * Include
 *****************************************************************************/
#include "RpSimControllerBase.h"
#include "RpGsmSimController.h"
#include "RpC2kSimController.h"

/*****************************************************************************
 * Class RpSimController
 *****************************************************************************/
const char PROPERTY_UIM_SUBSCRIBER_ID[4][25] = {
    "ril.uim.subscriberid.1",
    "ril.uim.subscriberid.2",
    "ril.uim.subscriberid.3",
    "ril.uim.subscriberid.4",
};

const char PROPERTY_RIL_CDMA_CARD_TYPE[4][25] = {
    "ril.cdma.card.type.1",
    "ril.cdma.card.type.2",
    "ril.cdma.card.type.3",
    "ril.cdma.card.type.4",
};

const char PROPERTY_ICCID_SIM[4][25] = {
    "ril.iccid.sim1",
    "ril.iccid.sim2",
    "ril.iccid.sim3",
    "ril.iccid.sim4",
};

class RpSimController : public RpSimControllerBase {
    RFX_DECLARE_CLASS(RpSimController); // Required: declare this class

public:

    RpSimController();

    virtual ~RpSimController();

// Override
protected:

    virtual bool onHandleRequest(const sp<RfxMessage>& message);

    virtual bool onHandleUrc(const sp<RfxMessage>& message);

    virtual bool onHandleResponse(const sp<RfxMessage>& message);

    virtual void onInit();

    // For pending the sequential request GET_SIM_STATUS
    virtual bool onPreviewMessage(const sp<RfxMessage>& message);
    virtual bool onCheckIfResumeMessage(const sp<RfxMessage>& message);


private:
    // Request
    RIL_CardStatus_v6* decodeGetSimStatusResp();
    void handleGetSimStatusReq(const sp<RfxMessage>& message);
    void handleGetSimStatusRsp(const sp<RfxMessage>& message);

    void handleGetImsiReq(const sp<RfxMessage>& message);
    void handleGetImsiRsp(const sp<RfxMessage>& message);

    void handlePinPukReq(const sp<RfxMessage>& message);
    void handlePinPukRsp(const sp<RfxMessage>& message);

    void handleQuerySimFacilityReq(const sp<RfxMessage>& message);
    void handleQuerySimFacilityRsp(const sp<RfxMessage>& message);

    void handleSetSimFacilityReq(const sp<RfxMessage>& message);
    void handleSetSimFacilityRsp(const sp<RfxMessage>& message);

    void handleIccOpenChannelReq(const sp<RfxMessage>& message);
    void handleIccOpenChannelRsp(const sp<RfxMessage>& message);

    void handleIccCloseChannelReq(const sp<RfxMessage>& message);
    void handleIccCloseChannelRsp(const sp<RfxMessage>& message);

    void handleTransmitApduReq(const sp<RfxMessage>& message);
    void handleTransmitApduRsp(const sp<RfxMessage>& message);

    void handleGetAtrReq(const sp<RfxMessage>& message);
    void handleGetAtrRsp(const sp<RfxMessage>& message);

    void handleSetUiccSubscriptionReq(const sp<RfxMessage>& message);
    void handleSetUiccSubscriptionRsp(const sp<RfxMessage>& message);


    void handleSimIoReq(const sp<RfxMessage>& message);
    void handleSimIoRsp(const sp<RfxMessage>& message);

    void handleSimAuthenticationReq(const sp<RfxMessage>& message);
    void handleSimAuthenticationRsp(const sp<RfxMessage>& message);

    void handleSimTransmitApduBasicReq(const sp<RfxMessage>& message);
    void handleSimTransmitApduBasicRsp(const sp<RfxMessage>& message);

    // URC
    void handleLocalCardTypeNotify(const sp<RfxMessage>& message);

private:
    // callback
    void onRadioStateChanged(RfxStatusKeyEnum key, RfxVariant old_value,
            RfxVariant value);

public:
    RILD_RadioTechnology_Group choiceDestByCardType();
    void switchCardType(int cardtype);

private:
    int mCurrentGetSimStatusReq;
    int mGsmGetSimStatusReq;
    int mC2kGetSimStatusReq;
    Parcel *mGsmSimStatusRespParcel;
    Parcel *mC2kSimStatusRespParcel;
    RIL_Errno mGetSimStatusErr;

    int mLastCardType;

    RpGsmSimController *mGsmSimCtrl;
    RpC2kSimController *mC2kSimCtrl;
};

#endif /* __RP_SIM_CONTROLLER_H__ */

