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

#ifndef __RP_DATA_CONTROLLER_H__
#define __RP_DATA_CONTROLLER_H__

/*****************************************************************************
 * Include
 *****************************************************************************/
#include <utils/Vector.h>
#include "RfxController.h"
#include "RfxTimer.h"
#include "RpDataConnectionInfo.h"

/*****************************************************************************
 * Class RpDataController
 *****************************************************************************/



class RpDataController : public RfxController {
    RFX_DECLARE_CLASS(RpDataController);  // Required: declare this class

public:
    RpDataController();

    virtual ~RpDataController();

// Override
protected:
    void sendToGsm(const sp<RfxMessage>& message);
    bool setActivePsSlotIfNeeded(const sp<RfxMessage>& message);
    virtual void onInit();
    virtual void onDeinit();
    virtual bool onHandleRequest(const sp<RfxMessage>& message);
    virtual bool onHandleUrc(const sp<RfxMessage>& message);
    virtual bool onHandleResponse(const sp<RfxMessage>& message);

    virtual void handleSetupDataRequest(const sp<RfxMessage>& request);
    virtual void handleDeactivateDataRequest(const sp<RfxMessage>& request);
    virtual void handleSetDataAllowRequest(const sp<RfxMessage>& request);
    virtual void handleGetDataCallListRequest(const sp<RfxMessage>& request);
    virtual void handleSetInitialAttachApnRequest(const sp<RfxMessage>& request);
    virtual void handleGetLastFailCauseRequest(const sp<RfxMessage>& request);
    virtual void handleSetFdModeRequest(const sp<RfxMessage>& request);
    virtual void handleRequestDefault(const sp<RfxMessage>& request);

    virtual void handleSetupDataResponse(const sp<RfxMessage>& response);
    virtual void handleDeactivateDataResponse(const sp<RfxMessage>& response);
    virtual void handleSetDataAllowResponse(const sp<RfxMessage>& response);
    virtual void handleGetDataCallListResponse(const sp<RfxMessage>& response);
    virtual void handleSetInitialAttachApnResponse(const sp<RfxMessage>& response);
    virtual void handleGetLastFailCauseResponse(const sp<RfxMessage>& response);
    virtual void handleSetFdModeResponse(const sp<RfxMessage>& response);

    virtual void handleDataCallListChangeUrc(const sp<RfxMessage>& message);
    virtual void handleDataAllowedUrc(const sp<RfxMessage>& message);
    virtual void onRadioStateChanged(RfxStatusKeyEnum key, RfxVariant old_value, RfxVariant value);

private:
    // Private functions
    void updateDataConnectionStatus(int oldCount, int newCount);
    bool needsetActivePsSlot();

private:
    // Private members
    int mInterfaceId;
    String16 mApnName;
    Vector<RpDataConnectionInfo*>* mDataInfoList;
};

#endif /* __RP_DATA_CONTROLLER_H__ */

