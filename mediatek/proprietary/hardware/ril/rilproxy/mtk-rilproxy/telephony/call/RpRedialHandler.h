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

#ifndef __RP_REDIAL_HANDLER_H__
#define __RP_REDIAL_HANDLER_H__

/*****************************************************************************
 * Include
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include <binder/Parcel.h>
#include <cutils/properties.h>

#include "RfxController.h"
#include "RfxStatusDefs.h"
#include "RfxTimer.h"
#include "nw/RpNwRatController.h"

/*****************************************************************************
 * MACRO
 *****************************************************************************/
#define ECPI_MSG_TYPE_CALL_CONNECT      6
#define ECPI_MSG_TYPE_CALL_ID_ASSIGN    130
#define ECPI_MSG_TYPE_CALL_DISCONNECT   133

#define REDIAL_RIL_PARCEL_HEADER        (2 * sizeof(int))
#define REDIAL_NET_TYPE_GLOBAL_MODE     PREF_NET_TYPE_LTE_CMDA_EVDO_GSM_WCDMA
#define REDIAL_NET_TYPE_RESUME_RAT_MODE -1

#define CALL_DISC_CAUSE_REDIAL_MD1                      107 
#define CALL_DISC_CAUSE_REDIAL_MD3                      1004
#define CALL_DISC_CAUSE_REDIAL_MD3_WITHOUT_TRY_VOLTE    1005
#define CALL_DISC_CAUSE_NEED_CHANGE_TO_GLOBAL_MODE      1006

#define RP_REDIAL_TAG "[RpRedialHandler]"

#define REDIAL_ASSERT(_c)       assert(_c)
#define REDIAL_DBG_ASSERT(_c)   assert(_c)


extern "C" const char * requestToString(int request);
/*****************************************************************************
 * ENUM
 *****************************************************************************/
enum DialSource {
    DIAL_FROM_CS,
    DIAL_FROM_IMS
};

enum RedialState {
    REDIAL_NONE     = 0,
    REDIAL_TO_MD1   = 1,
    REDIAL_TO_MD3   = 2,
    REDIAL_CHANGE_GLOBAL_MODE = 3,
};

/*****************************************************************************
 * Class MoCallContext
 *****************************************************************************/
class MoCallContext {
    public:
        MoCallContext(char *callNumber, bool isEcc, int slotId);
        MoCallContext(Parcel& p, bool isEcc, int slotId);

        Parcel* getDialParcel();
        int     getCallId();
        int     getSlotId();
        void    setCallId(int callId);
        bool    isEccCall();

    private:
        Parcel  mDialParcel;
        int     mCallId;
        int     mSlotId;
        bool    mIsEmergencyCall;
};

/*****************************************************************************
 * Class RpRedialHandler
 *****************************************************************************/
class RpRedialHandler : public RfxController {
    RFX_DECLARE_CLASS(RpRedialHandler);  // Required: declare this class

public:
    RpRedialHandler();

    virtual ~RpRedialHandler();

    bool notifyRilRequest(const sp<RfxMessage>& msg);
    const sp<RfxMessage>  notifyRilResponse(const sp<RfxMessage>& msg);
    bool notifyRilUrc(const sp<RfxMessage>& msg);
    DialSource getDialSource();
    RedialState getRedialState();

    // Utility function
    static char* boolToString(bool value);
    static char* redialStateToString(RedialState value);
    static char* dialSourceToString(DialSource value);
    static char* deviceModeToString(int deviceMode);
    static char* radioTechToString(int radio);
    static char* strdupReadString(Parcel &p);

// Override
protected:
    virtual bool onHandleRequest(const sp<RfxMessage>& msg);
    virtual bool onHandleResponse(const sp<RfxMessage>& msg);

    virtual bool onHandleAtciRequest(const sp<RfxMessage>& msg);
    virtual bool onHandleAtciResponse(const sp<RfxMessage>& msg);

    virtual bool onHandleUrc(const sp<RfxMessage>& msg);

    virtual void onInit();


private:
    void onDialSourceIms(RfxStatusKeyEnum key, RfxVariant old_value, RfxVariant value);
    void onGlobalModeSwitchDone(int slotId);
    void onGlobalModeResumeDone(int slotId);
    void onPreferNwSwitchComplete(RfxStatusKeyEnum key, RfxVariant old_value, RfxVariant value);
    bool handleDialRequest(const sp<RfxMessage>& msg);
    bool handleAtciDialRequest(const char* data, int dataLength, int slotId);
    bool handleAtciCdmaDialRequest(const char* data, int dataLength, int slotId);
    bool handleAtciSetRadioPower(const char* data, int dataLength, int slotId);
    bool handleCallInfoInd(const sp<RfxMessage>& msg);
    bool handleCallStateChanged(const sp<RfxMessage>& msg);
    bool handleEnterEcbm(const sp<RfxMessage>& msg);
    bool handleExitEcbm(const sp<RfxMessage>& msg);

    bool processCallConnect(const sp<RfxMessage>& msg);
    bool processCallDisconnect(const sp<RfxMessage>& msg, int discCause);

    // Overwrite
    bool responseToRilj(const sp<RfxMessage>& msg);

    bool checkDropUrc(const sp<RfxMessage>& msg);
    void setRedialState(RedialState state);
    void setDialSource(DialSource source);
    void setEmergencyMode(bool emergencyMode, int slotId);
    void setUserHangUping(bool userHangUping);
    void setRatMode(int ratMode, int slotId);
    sp<RfxMessage> getRedialRequest(RILD_RadioTechnology_Group dialTarget, bool isVolteTried);
    int  getDeviceMode(int slotId);
    void freeMoCallObject();

    bool checkOpCode(int opCode);

public:

private:
    int     mPreRatMode;
    bool    mInEmergencyMode;
    bool    mIsUserHangUping;

    DialSource      mDialSource;
    RedialState     mRedialState;
    MoCallContext   *mMoCall;
};

#endif /* __RP_REDIAL_HANDLER_H__ */

