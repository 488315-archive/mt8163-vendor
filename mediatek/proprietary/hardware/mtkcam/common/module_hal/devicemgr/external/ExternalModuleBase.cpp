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

#define LOG_TAG "MtkCam/ExternalModule"
//
#include "../MyUtils.h"
#include <utils/RWLock.h>
#include <cutils/properties.h>
using namespace android;
using namespace NSCam;
//
#include <device/ExternalModuleBase.h>
using namespace NSCam::Utils;
//
/******************************************************************************
 *
 ******************************************************************************/
#define MY_LOGV(fmt, arg...)        CAM_LOGV("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_LOGD("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_LOGI("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_LOGW("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_LOGE("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGA(fmt, arg...)        CAM_LOGA("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGF(fmt, arg...)        CAM_LOGF("[%s] " fmt, __FUNCTION__, ##arg)
//
#define MY_LOGV_IF(cond, ...)       do { if ( (cond) ) { MY_LOGV(__VA_ARGS__); } }while(0)
#define MY_LOGD_IF(cond, ...)       do { if ( (cond) ) { MY_LOGD(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) ) { MY_LOGI(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) ) { MY_LOGW(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) ) { MY_LOGE(__VA_ARGS__); } }while(0)
#define MY_LOGA_IF(cond, ...)       do { if ( (cond) ) { MY_LOGA(__VA_ARGS__); } }while(0)
#define MY_LOGF_IF(cond, ...)       do { if ( (cond) ) { MY_LOGF(__VA_ARGS__); } }while(0)

/******************************************************************************
 *
 ******************************************************************************/
struct CameraInfo;

namespace NSCam {

ExternalModuleBase::
ExternalModuleBase()
    : mListener(NULL)
{}

/******************************************************************************
 *
 ******************************************************************************/
ExternalModuleBase::
~ExternalModuleBase()
{
    mListener = NULL;
}

/******************************************************************************
 *
 ******************************************************************************/
const sp< IExternalDeviceListener >&
ExternalModuleBase::
getListener()
{
    Mutex::Autolock _l(mRWLock);
    if(mListener == NULL) {
        MY_LOGI("IExternalDeviceListener has been created. %p", mListener.get());
    }
    //
    return mListener;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
ExternalDeviceListenerImp::
onConnected(
    uintptr_t deviceID,
    sp< IExternalDevice > device,
    CameraInfo* info,
    int32_t& remoteID
)
{
    Mutex::Autolock _l(mRWLock);
    return onConnectedLocked(deviceID, device, info, remoteID);
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
ExternalDeviceListenerImp::
onDisconnected (
    uintptr_t deviceID
)
{
    Mutex::Autolock _l(mRWLock);
    return onDisconnectedLocked(deviceID);
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
ExternalDeviceListenerImp::
onConnectedLocked(
    uintptr_t deviceID,
    sp< IExternalDevice > device,
    CameraInfo* info,
    int32_t& remoteID
)
{
    remoteID = -1;
    if (!mAvailableID.isEmpty()) {
        remoteID = mAvailableID[0];
        mAvailableID.removeAt(0);
    }
    //
    status_t err = mDeviceManager->addExternalDeviceLocked(deviceID, device, info, remoteID);
    return err;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
ExternalDeviceListenerImp::
onDisconnectedLocked(
    uintptr_t deviceID
)
{
    int32_t remoteID;
    //
    status_t err = mDeviceManager->removeExternalDeviceLocked(deviceID, remoteID);
    //
    if ( err == OK )
        mAvailableID.add(remoteID);
    //
    return err;
}
};  //namespace NSCam

/******************************************************************************
 *
 ******************************************************************************/
extern "C" {

    IExternalModule*  getExternalModule() {
        static IExternalModule* mModule = NULL;
        if(mModule == NULL) {
            mModule = new ExternalModuleBase();
            MY_LOGI("IExternalModule has been created. %p", mModule);
        }
        MY_LOGI("IExternalModule. %p", mModule);
        //
        return mModule;
    }
}// extern "C"
