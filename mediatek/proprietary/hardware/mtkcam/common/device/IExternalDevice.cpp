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
#define LOG_TAG "IExternalDevice"
#include <utils/Log.h>
#include <stdint.h>
#include <sys/types.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <camera/camera2/ICameraDeviceUser.h>
#include <gui/IGraphicBufferProducer.h>
#include <gui/Surface.h>
#include <camera/CameraMetadata.h>
#include <camera/camera2/CaptureRequest.h>
#include <device/IExternalDevice.h>

/******************************************************************************
 *
 ******************************************************************************/
#define MY_LOGV(fmt, arg...)        CAM_LOGV("(%d)(%s:%d)[%s] "fmt, ::gettid(), getDevName(), getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_LOGD("(%d)(%s:%d)[%s] "fmt, ::gettid(), getDevName(), getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_LOGI("(%d)(%s:%d)[%s] "fmt, ::gettid(), getDevName(), getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_LOGW("(%d)(%s:%d)[%s] "fmt, ::gettid(), getDevName(), getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_LOGE("(%d)(%s:%d)[%s] "fmt, ::gettid(), getDevName(), getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGA(fmt, arg...)        CAM_LOGA("(%d)(%s:%d)[%s] "fmt, ::gettid(), getDevName(), getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGF(fmt, arg...)        CAM_LOGF("(%d)(%s:%d)[%s] "fmt, ::gettid(), getDevName(), getOpenId(), __FUNCTION__, ##arg)
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
    CLOSE_DEVICE = IBinder::FIRST_CALL_TRANSACTION,
    OPEN_DEVICE,
    SET_CALLBACK,
    RELEASE_BUFFER,
    SUBMIT_REPEATINGREQUEST,
    SUBMIT_REQUEST,
    CONFIGURE_STREAM,
    TRANSACT
};

class BpExternalDevice : public BpInterface<IExternalDevice>
{
public:
    BpExternalDevice(const sp<IBinder>& impl)
        : BpInterface<IExternalDevice>(impl)
    {
    }

    status_t  closeDevice ()
    {
        ALOGI("closeDevice");
        android::Parcel data, reply;
        data.writeInterfaceToken(IExternalDevice::getInterfaceDescriptor());
        remote()->transact(CLOSE_DEVICE, data, &reply);
        reply.readExceptionCode();
        //
        return reply.readInt32();
    }

    status_t  openDevice ()
    {
        ALOGI("openDevice");
        android::Parcel data, reply;
        data.writeInterfaceToken(IExternalDevice::getInterfaceDescriptor());
        remote()->transact(OPEN_DEVICE, data, &reply);
        reply.readExceptionCode();
        //
        return reply.readInt32();
    }

    status_t  setCallback (android::wp<IResultCallback>& cb)
    {
        ALOGI("setCallback");
        android::Parcel data, reply;
        data.writeInterfaceToken(IExternalDevice::getInterfaceDescriptor());
        // arg0
        sp<IResultCallback> cbsp = cb.promote();
        data.writeInt32(1);
        data.writeStrongBinder(asBinder(cbsp));
        //
        remote()->transact(SET_CALLBACK, data, &reply);
        reply.readExceptionCode();
        //
        return reply.readInt32();
    }

    status_t  releaseBuffer (Buffer* buffer)
    {
        ALOGI("releaseBuffer");
        android::Parcel data, reply;
        data.writeInterfaceToken(IExternalDevice::getInterfaceDescriptor());
        // arg0
        data.writeInt32(1);
        buffer->writeToParcel(data);
        //
        remote()->transact(RELEASE_BUFFER, data, &reply);
        reply.readExceptionCode();
        //
        return reply.readInt32();
    }

    status_t  submitRepeatingRequest ( RepeatingRequest* request )
    {
        ALOGI("submitRepeatingRequest");
        android::Parcel data, reply;
        data.writeInterfaceToken(IExternalDevice::getInterfaceDescriptor());
        //
        request->writeToParcel(data);
        //
        remote()->transact(SUBMIT_REPEATINGREQUEST, data, &reply);
        reply.readExceptionCode();
        //
        return reply.readInt32();
    }

    status_t  submitRequest ( Request* request )
    {
        ALOGI("submitRequest");
        android::Parcel data, reply;
        data.writeInterfaceToken(IExternalDevice::getInterfaceDescriptor());
        //
        request->writeToParcel(data);
        //
        remote()->transact(SUBMIT_REQUEST, data, &reply);
        reply.readExceptionCode();
        //
        return reply.readInt32();
    }

    status_t  configureStreams ( ImageInfo* info )
    {
        return OK;
    }

    status_t  transact ( uint32_t code,
                         const Parcel& parcel_data, 
                         Parcel* /*parcel_reply*/ )
    {
        ALOGI("transact");
        android::Parcel data, reply;
        data.writeInterfaceToken(IExternalDevice::getInterfaceDescriptor());
        // arg0 code
        data.writeInt32(code);
        // arg0 parcel_data
        data.writeInt32(parcel_data.mData.size());// parcel_data size
        if (!parcel_data.mData.empty()) {
            for (size_t i = 0; i < parcel_data.mData.size(); ++i) {
                data.writeInt32(parcel_data.mData[i]);
            }
        }
        //
        remote()->transact(TRANSACT, data, &reply);
        // arg1 parcel_reply
        reply.readExceptionCode();
        //
        return reply.readInt32();
    }

private:

};

IMPLEMENT_META_INTERFACE(ExternalDevice,
                         "mediatek.ExternalDevice");

status_t BnExternalDevice::onTransact(
    uint32_t code, const android::Parcel& data, android::Parcel* reply, uint32_t flags)
{
    switch(code) {
        case CLOSE_DEVICE: {
            ALOGI("CLOSE_DEVICE");
            CHECK_INTERFACE(IExternalDevice, data, reply);
            reply->writeNoException();
            reply->writeInt32(closeDevice());
            return NO_ERROR;
        } break;
        case OPEN_DEVICE: {
            ALOGI("OPEN_DEVICE");
            CHECK_INTERFACE(IExternalDevice, data, reply);
            reply->writeNoException();
            reply->writeInt32(openDevice());
            return NO_ERROR;
        } break;
        case SET_CALLBACK: {
            ALOGI("SET_CALLBACK");
            CHECK_INTERFACE(IExternalDevice, data, reply);
            // arg0
            wp<IResultCallback> cb;
            if (data.readInt32() != 0) {
                cb = interface_cast< IResultCallback >(data.readStrongBinder());
                ALOGI("%s: SET_CALLBACK: cb = YES", __FUNCTION__);
            } else {
                ALOGW("%s: SET_CALLBACK: cb = unset", __FUNCTION__);
            }
            //
            reply->writeNoException();
            reply->writeInt32(setCallback(cb));
            return NO_ERROR;
        } break;
        case RELEASE_BUFFER: {
            ALOGI("RELEASE_BUFFER");
            CHECK_INTERFACE(IExternalDevice, data, reply);
            // arg0
            Buffer buffer;
            if (data.readInt32() != 0) {
                if (buffer.readFromParcel(&data) != OK) {
                    ALOGE("%s: RELEASE_BUFFER fail", __FUNCTION__);
                } else {
                    ALOGV("%s: RELEASE_BUFFER: buffer = %p", __FUNCTION__, &buffer);
                }
            } else {
                ALOGW("%s: RELEASE_BUFFER: buffer = unset", __FUNCTION__);
            }
            //
            reply->writeNoException();
            reply->writeInt32(releaseBuffer(&buffer));
            return NO_ERROR;
        } break;
        case SUBMIT_REPEATINGREQUEST: {
            ALOGI("SUBMIT_REPEATINGREQUEST");
            CHECK_INTERFACE(IExternalDevice, data, reply);
            //
            RepeatingRequest request;
            //
            if (request.readFromParcel(&data) != OK) {
                ALOGE("%s: SUBMIT_REPEATINGREQUEST fail", __FUNCTION__);
            } else {
                ALOGV("%s: SUBMIT_REPEATINGREQUEST: request = %p", __FUNCTION__, &request);
            }
            //
            reply->writeNoException();
            reply->writeInt32(submitRepeatingRequest(&request));
            return NO_ERROR;
        } break;
        case SUBMIT_REQUEST: {
            ALOGI("SUBMIT_REQUEST");
            CHECK_INTERFACE(IExternalDevice, data, reply);
            //
            Request request;
            //
            if (request.readFromParcel(&data) != OK) {
                ALOGE("%s: SUBMIT_REQUEST fail", __FUNCTION__);
            } else {
                ALOGV("%s: SUBMIT_REQUEST: request = %p", __FUNCTION__, &request);
            }
            //
            reply->writeNoException();
            reply->writeInt32(submitRequest(&request));
            return NO_ERROR;
        } break;
        case CONFIGURE_STREAM: {
            return NO_ERROR;
        } break;
        case TRANSACT: {
            ALOGI("TRANSACT");
            CHECK_INTERFACE(IExternalDevice, data, reply);
            // arg 0 code
            bool code = data.readInt32();
            // arg1 Parcel_data
            Parcel Parcel_data, reply_data;
            int Size = data.readInt32();
            if (Size == 0) {
                ALOGW("%s: TRANSACT: Parcel_data = unset", __FUNCTION__);
            }
            //
            int SettingSize = data.readInt32();
            for (int i = 0; i < SettingSize; ++i) {
                Parcel_data.mData.push_back(data.readInt32());
            }
            //
            reply->writeNoException();
            return NO_ERROR;
        } break;
        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}

/******************************************************************************
 *
 ******************************************************************************/

enum {
    RESULT_RECEIVE = IBinder::FIRST_CALL_TRANSACTION
};

class BpResultCallback : public BpInterface<IResultCallback>
{
public:
    BpResultCallback(const sp<IBinder>& impl)
        : BpInterface<IResultCallback>(impl)
    {
    }

    status_t  onResultReceived ( Result* result )
    {
        ALOGI("onResultReceived");
        android::Parcel data, reply;
        data.writeInterfaceToken(IResultCallback::getInterfaceDescriptor());
        //
        result->writeToParcel(data);
        //
        remote()->transact(RESULT_RECEIVE, data, &reply);
        reply.readExceptionCode();
        //
        return reply.readInt32();
    }

private:

};

IMPLEMENT_META_INTERFACE(ResultCallback,
                         "mediatek.ResultCallback");

status_t BnResultCallback::onTransact(
    uint32_t code, const android::Parcel& data, android::Parcel* reply, uint32_t flags)
{
    switch(code) {
        case RESULT_RECEIVE: {
            ALOGI("RESULT_RECEIVE");
            CHECK_INTERFACE(IResultCallback, data, reply);
            //
            Result result;
            if (result.readFromParcel(&data) != OK) {
                ALOGE("%s: RESULT_RECEIVE fail", __FUNCTION__);
            } else {
                ALOGV("%s: RESULT_RECEIVE: buffer = %p", __FUNCTION__, &result);
            }
            //
            reply->writeNoException();
            reply->writeInt32(onResultReceived(&result));
            return NO_ERROR;
        } break;
        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}
// ----------------------------------------------------------------------------

}; // namespace NSCam
