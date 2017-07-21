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

#ifndef __RP_IR_CONTROLLER_H__
#define __RP_IR_CONTROLLER_H__

/*****************************************************************************
 * Include
 *****************************************************************************/
#include "RfxAction.h"
#include "RfxController.h"
#include "RfxTimer.h"
#include "ril.h"
#include "RpNwDefs.h"
#include "IRpNwRatSwitchCallback.h"
#include "cutils/properties.h"
#include <string>

using namespace std;

using ::android::Vector;

typedef enum TagServiceType {
    OUT_OF_SERVICE,
    IN_SEARCHING,
    IN_SERVICE,
} ServiceType;

#define NO_SERVICE_DELAY_TIME (15*1000)

#define Bool2Str(value) RpIrController::boolToString(value)
#define Nws2Str(value) RpIrController::nwsModeToString(value)
#define ServiceType2Str(value) RpIrController::serviceTypeToString(value)

/*****************************************************************************
 * Class RpIrController
 *****************************************************************************/
class RpIrLwgHandler;
class RpIrCdmaHandler;
class RpIrStrategy;
class RpIrNwRatSwitchCallback;
class RpIrController : public RfxController{
    RFX_DECLARE_CLASS(RpIrController);
public:

    static int sNoServiceDelayTime;
    RpIrStrategy* mPtrSwitchStrategy;

private:
    RpIrLwgHandler* mPtrLwgHandler;
    RpIrCdmaHandler* mPtrCdmaHandler;

    bool mIsEnabled;
    bool mIsGmssEnable;
    bool mIsSwitchingTo3GMode;

    NwsMode mNwsMode;
    int32_t mMccInGmss;

    RpIrNwRatSwitchCallback *mNwRatSwitchCallback;

public:
    RpIrController();
    virtual ~RpIrController();

    void setIrControllerEnableState(bool isEnabled);
    bool getIrControllerEnableState();
    void setIrEnableState(bool isEnabled);
    bool isCdmaDualModeSimCard();
    bool isCtSimCard();
    void setNwsMode(NwsMode nwsMode, const sp<RfxAction>& action);
    NwsMode getNwsMode();
    bool isSimReady();

    // Handle URC for GMSS RAT changed
    void setGmssEnableState(bool isEnabled);
    bool getGmssEnableState();
    void onNwsModeChange(NwsMode curMode);
    void setIsSwitchingTo3GMode(bool isSwitchingTo3GMode) {
        mIsSwitchingTo3GMode = isSwitchingTo3GMode;
    }
    // Override

    static const char* boolToString(bool value);
    static const char* nwsModeToString(NwsMode mode);
    static const char* serviceTypeToString(ServiceType serviceType);
protected:

    virtual bool onHandleUrc(const sp<RfxMessage>& message);

    virtual void onInit();

private:

    void dispose();

    // Handle URC for GMSS RAT changed
    void processGmssRatChanged(const sp<RfxMessage>& msg);
    void onGmssRatChanged(int mcc);
    void onImsiChanged(RfxStatusKeyEnum key, RfxVariant old_value, RfxVariant value);
    NwsMode getNwsModeByMcc(int mcc);
    bool needWaitImsi();
};

#endif /* __RP_IR_CONTROLLER_H__ */

