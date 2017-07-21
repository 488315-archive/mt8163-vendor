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

// #define LOG_NDEBUG 0
#define LOG_TAG "IExternalModule"
#include <utils/Log.h>
#include <stdint.h>
#include <sys/types.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <device/IExternalModule.h>
#include <device/IExternalDevice.h>

/******************************************************************************
 *
 ******************************************************************************/
#define MY_LOGV(fmt, arg...)        CAM_LOGV("(%d)(%s:%d)[%s] " fmt, ::gettid(), getDevName(), getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_LOGD("(%d)(%s:%d)[%s] " fmt, ::gettid(), getDevName(), getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_LOGI("(%d)(%s:%d)[%s] " fmt, ::gettid(), getDevName(), getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_LOGW("(%d)(%s:%d)[%s] " fmt, ::gettid(), getDevName(), getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_LOGE("(%d)(%s:%d)[%s] " fmt, ::gettid(), getDevName(), getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGA(fmt, arg...)        CAM_LOGA("(%d)(%s:%d)[%s] " fmt, ::gettid(), getDevName(), getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGF(fmt, arg...)        CAM_LOGF("(%d)(%s:%d)[%s] " fmt, ::gettid(), getDevName(), getOpenId(), __FUNCTION__, ##arg)
//
#define MY_LOGV_IF(cond, ...)       do { if ( (cond) ) { MY_LOGV(__VA_ARGS__); } }while(0)
#define MY_LOGD_IF(cond, ...)       do { if ( (cond) ) { MY_LOGD(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) ) { MY_LOGI(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) ) { MY_LOGW(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) ) { MY_LOGE(__VA_ARGS__); } }while(0)
#define MY_LOGA_IF(cond, ...)       do { if ( (cond) ) { MY_LOGA(__VA_ARGS__); } }while(0)
#define MY_LOGF_IF(cond, ...)       do { if ( (cond) ) { MY_LOGF(__VA_ARGS__); } }while(0)

using namespace android;
/******************************************************************************
 *
 ******************************************************************************/

namespace NSCam {

typedef android::Parcel::WritableBlob WritableBlob;
typedef android::Parcel::ReadableBlob ReadableBlob;

enum {
    ON_CONNECTED = IBinder::FIRST_CALL_TRANSACTION,
    ON_DISCONNECTED
};

class BpExternalDeviceListener : public BpInterface<IExternalDeviceListener>
{
public:
    BpExternalDeviceListener(const sp<IBinder>& impl)
        : BpInterface<IExternalDeviceListener>(impl)
    {
    }

    status_t  onConnected (uintptr_t deviceID ,
                           android::sp< IExternalDevice > device ,
                           CameraInfo* info,
                           int32_t& remoteID
                           )
    {
        ALOGI("onConnected");
        android::Parcel data, reply;
        data.writeInterfaceToken(IExternalDeviceListener::getInterfaceDescriptor());
        //
        data.writeInt64(deviceID);
        data.writeStrongBinder(asBinder(device));
        info->writeToParcel(data);
        //
        remote()->transact(ON_CONNECTED, data, &reply);
        reply.readExceptionCode();
        remoteID = reply.readInt32();
        //
        return OK;
    }

    status_t  onDisconnected (uintptr_t  deviceID)
    {
        ALOGI("onDisconnected");
        android::Parcel data, reply;
        data.writeInterfaceToken(IExternalDeviceListener::getInterfaceDescriptor());
        //
        data.writeInt64(deviceID);
        //
        remote()->transact(ON_DISCONNECTED, data, &reply);
        reply.readExceptionCode();
        return reply.readInt32();
    }

private:

};

IMPLEMENT_META_INTERFACE(ExternalDeviceListener,
                         "mediatek.ExternalDeviceListener");

status_t BnExternalDeviceListener::onTransact(
    uint32_t code, const android::Parcel& data, android::Parcel* reply, uint32_t flags)
{
    switch(code) {
        case ON_CONNECTED: {
            ALOGI("ON_CONNECTED");
            CHECK_INTERFACE(IExternalDeviceListener, data, reply);
            //
            uintptr_t  deviceID;
            data.readInt64((int64_t*)&deviceID);
            //
            android::sp< IExternalDevice > device
                = interface_cast< IExternalDevice >(data.readStrongBinder());
            //
            CameraInfo info;
            info.readFromParcel(&data);
            //
            int32_t  remoteID;
            reply->writeNoException();
            onConnected(deviceID,
                        device,
                        &info,
                        remoteID);
            ALOGI("bp onConnected remoteID:%d deviceID:%d", remoteID, deviceID);
            reply->writeInt32(remoteID);
            return NO_ERROR;
        } break;
        case ON_DISCONNECTED: {
            ALOGI("ON_DISCONNECTED");
            CHECK_INTERFACE(IExternalDeviceListener, data, reply);
            //
            uintptr_t  deviceID;
            data.readInt64((int64_t*)&deviceID);
            //
            reply->writeNoException();
            reply->writeInt32(onDisconnected(deviceID));
            return NO_ERROR;
        } break;
        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}

}; // namespace NSCam
