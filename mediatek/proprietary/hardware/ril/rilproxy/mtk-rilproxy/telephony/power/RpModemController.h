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

#ifndef __RP_MODEM_CONTROLLER_H__
#define __RP_MODEM_CONTROLLER_H__

/*****************************************************************************
 * Include
 *****************************************************************************/
#include "RfxController.h"
#include "RfxTimer.h"
#include "ModemMessage.h"
#include "RfxAction.h"
#include "RfxMainThread.h"

#define INITIAL_RETRY_INTERVAL_MSEC 200 // 200ms

/*****************************************************************************
 * Class RfxController
 *****************************************************************************/

class RpModemController : public RfxController {
    RFX_DECLARE_CLASS(RpModemController); // Required: declare this class

public:

    RpModemController();
    virtual ~RpModemController();
    static RpModemController* getInstance();

    void configModemStatus(int cdmaSlot);
    void sendCdmaModemStatus();
    sp<RfxMessage> cdmaModemStatusRequest;
    void powerOffModemBeforeTrm(int cdmaSlot, sp<RfxAction>& action);

protected:

    virtual void onInit();
    virtual void onDeinit();
    virtual bool onHandleRequest(const sp<RfxMessage>& message);
    virtual bool onHandleResponse(const sp<RfxMessage>& message);
    virtual bool onHandleUrc(const sp<RfxMessage>& message);
    virtual bool onPreviewMessage(const sp<RfxMessage>& message);
    virtual bool onCheckIfResumeMessage(const sp<RfxMessage>& message);

private:
    ModemMessage* modemMessages;
    int* modemPower;
    sp<RfxAction> action;
    int cdmaLteModeControllerToken;
    void onConfigModemTimer();
    bool is4GCdmaCard(int cardType);
    bool is3GCdmaCard(int cardType);
    bool isGsmCard(int cardType);
    bool isNonCard(int cardType);
    bool isCdmaIratSupport();
    void initModemMessages();
    static TimerHandle s_cmd_startd_th;//cdma mode started timer handle
    int cdmaSlotId;
    void requestModemPower(int power, const sp<RfxMessage>& message);
    bool canHandleRequest(const sp<RfxMessage>& message);
    bool handleModemPowerResponse(const sp<RfxMessage>& message);
    bool checkIfRequestComplete(ModemMessage* modemMessage);
    void onCdmaSocketSlotChange(RfxStatusKeyEnum key, RfxVariant old_value, RfxVariant value);
    void onSocketStateChange(int slotId, RfxStatusKeyEnum key, RfxVariant old_value, RfxVariant value);
};

#endif /* __RP_MODEM_CONTROLLER_H__ */

