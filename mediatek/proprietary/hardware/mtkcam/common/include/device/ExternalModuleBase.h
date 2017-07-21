/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#ifndef _MTK_PLATFORM_HARDWARE_MTKCAM_DEVICEMGR_EXTERNALMODULEBASE_H_
#define _MTK_PLATFORM_HARDWARE_MTKCAM_DEVICEMGR_EXTERNALMODULEBASE_H_
//
#include <utils/RefBase.h>
#include <utils/String8.h>
#include <utils/KeyedVector.h>
#include <utils/StrongPointer.h>
//
#include "IExternalModule.h"


/******************************************************************************
 *
 ******************************************************************************/
namespace NSCam {


/******************************************************************************
 *
 ******************************************************************************/
class ExternalModuleBase : public IExternalModule
{
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Implementations.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
protected:  ////                          Data Members.
    mutable android::Mutex                mRWLock;
    android::sp<IExternalDeviceListener>  mListener;

public:     ////                          Instantiation.
    virtual                               ~ExternalModuleBase();
                                          ExternalModuleBase();

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  IExternalModule Interfaces.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////

    virtual const sp< IExternalDeviceListener >& getListener();

};


/******************************************************************************
 *
 ******************************************************************************/
class ExternalDeviceListenerImp : 
    public BnExternalDeviceListener
{
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Implementations.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
                                          ExternalDeviceListenerImp(android::sp<CamDeviceManagerBase> mgr)
                                              : mDeviceManager(mgr) {};

                                          ~ExternalDeviceListenerImp() {};

protected:  ////                          Data Members.
    mutable android::Mutex                mRWLock;
    android::SortedVector<int32_t>        mAvailableID;
    android::sp<CamDeviceManagerBase>     mDeviceManager;

protected:  ////                          Operations.
    status_t                              onConnectedLocked (
                                                uintptr_t deviceID,
                                                sp< IExternalDevice > device,
                                                CameraInfo* info,
                                                int32_t& remoteID
                                          );

    status_t                              onDisconnectedLocked (
                                                uintptr_t  deviceID 
                                          );
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  IExternalDeviceListener Interfaces.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////
    virtual status_t                      onConnected (
                                                uintptr_t deviceID,
                                                sp< IExternalDevice > device,
                                                CameraInfo* info,
                                                int32_t& remoteID
                                          );

    virtual status_t                      onDisconnected (
                                                uintptr_t  deviceID
                                          );
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  CamDeviceManagerBase Interfaces.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////
    virtual int32_t                     enumDeviceLocked() { return 0;}
};

};  //namespace NSCam
#endif  //_MTK_PLATFORM_HARDWARE_MTKCAM_DEVICEMGR_EXTERNALMODULEBASE_H_

