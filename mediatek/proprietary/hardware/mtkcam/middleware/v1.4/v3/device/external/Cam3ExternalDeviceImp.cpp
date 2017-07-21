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

#define LOG_TAG "MtkCam/Cam3Device"
//
#include <utils/StrongPointer.h>
//
#include "MyUtils.h"
#include "Cam3ExternalDeviceImp.h"
//
using namespace android;
using namespace NSCam::v3;
using namespace NSDefaultCam3Device;
//
class IExternalDevice;
struct CameraInfo;
/******************************************************************************
 *
 ******************************************************************************/
#define MY_LOGV(fmt, arg...)        CAM_LOGV("(%s:%d)[%s] " fmt, getDevName(), getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_LOGD("(%s:%d)[%s] " fmt, getDevName(), getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_LOGI("(%s:%d)[%s] " fmt, getDevName(), getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_LOGW("(%s:%d)[%s] " fmt, getDevName(), getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_LOGE("(%s:%d)[%s] " fmt, getDevName(), getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGA(fmt, arg...)        CAM_LOGA("(%s:%d)[%s] " fmt, getDevName(), getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGF(fmt, arg...)        CAM_LOGF("(%s:%d)[%s] " fmt, getDevName(), getOpenId(), __FUNCTION__, ##arg)
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
extern "C"
NSCam::Cam3Device*
createCam3Device_External(
    String8 const&               rDevName,
    int32_t const                i4OpenId,
    NSCam::IExternalDevice*      aExternal,
    NSCam::CameraInfo*           aInfo
)
{
    return new NSDefaultCam3Device::Cam3ExternalDeviceImp(rDevName, i4OpenId, aExternal, aInfo);
}


/******************************************************************************
 *
 ******************************************************************************/
Cam3ExternalDeviceImp::
~Cam3ExternalDeviceImp()
{
}


/******************************************************************************
 *
 ******************************************************************************/
Cam3ExternalDeviceImp::
Cam3ExternalDeviceImp(
    String8 const&                      rDevName,
    int32_t const                       i4OpenId,
    NSCam::IExternalDevice*             aExternal,
    NSCam::CameraInfo*                  aInfo
)
    : Cam3DeviceBase(rDevName, i4OpenId)
    //
    , mAppContext()
    , mAppContextLock()
    //
    , mRequestingLock()
    , mRequestingAllowed(0)
    //
    , pExternal(aExternal)
    , pInfo(aInfo)
{
}

/******************************************************************************
 *
 ******************************************************************************/
camera_metadata const*
Cam3ExternalDeviceImp::
i_construct_default_request_settings(
    int type
)
{
    MY_LOGD_IF(1, "type:%d", type);
#warning "[TODO] remove hard code template device 0"
    ITemplateRequest* obj = NSTemplateRequestManager::valueFor(0);
    if(obj == NULL) {
        obj = ITemplateRequest::getInstance(0);
        NSTemplateRequestManager::add(0, obj);
    }
    return obj->getData(type);
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
Cam3ExternalDeviceImp::
onInitializeLocked()
{
    NSCam::Utils::CamProfile profile(__FUNCTION__, getDevName());
    //
lbExit:
    profile.print("");
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
status_t
Cam3ExternalDeviceImp::
onUninitializeLocked()
{
    NSCam::Utils::CamProfile profile(__FUNCTION__, getDevName());

/*
 * 12. Alternatively, the framework may call camera3_device_t->common->close()
 *    to end the camera session. This may be called at any time when no other
 *    calls from the framework are active, although the call may block until all
 *    in-flight captures have completed (all results returned, all buffers
 *    filled). After the close call returns, no more calls to the
 *    camera3_callback_ops_t functions are allowed from the HAL. Once the
 *    close() call is underway, the framework may not call any other HAL device
 *    functions.
 *
 */
    {
        disableRequesting();
        flushAndWait(getSafeAppContext());
        profile.print("flush -");
    }
    //
    {
        Mutex::Autolock _l(mAppContextLock);
        //
        if  ( mAppContext.pBufferReceiveNode != 0 ) {
            mAppContext.pBufferReceiveNode->uninit();
            mAppContext.pBufferReceiveNode = NULL;
        }
        //
        if  ( mAppContext.pAppStreamManager != 0 ) {
            mAppContext.pAppStreamManager->destroy();
            mAppContext.pAppStreamManager = NULL;
            profile.print("AppStreamManager -");
        }
        //
        if ( pExternal != 0 ) {
            pExternal = NULL;
        }
    }
    //
    profile.print("");
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
status_t
Cam3ExternalDeviceImp::
onFlushLocked()
{
    MY_LOGD("");
    //
    disableRequesting();
    //
    MERROR err = flushAndWait(getSafeAppContext());
    //
    enableRequesting();
    //
    return err;
}


/******************************************************************************
 *
 ******************************************************************************/
Cam3ExternalDeviceImp::AppContext
Cam3ExternalDeviceImp::
getSafeAppContext() const
{
    Mutex::Autolock _l(mAppContextLock);
    AppContext context = mAppContext;
    return context;
}


/******************************************************************************
 *
 ******************************************************************************/
/**
 * flush() should only return when there are no more outstanding buffers or
 * requests left in the HAL. The framework may call configure_streams (as
 * the HAL state is now quiesced) or may issue new requests.
 *
 * Performance requirements:
 *
 * The HAL should return from this call in 100ms, and must return from this
 * call in 1000ms. And this call must not be blocked longer than pipeline
 * latency (see S7 for definition).
 */
status_t
Cam3ExternalDeviceImp::
flushAndWait(AppContext const& appContext)
{
    MERROR err = OK;
    MY_LOGD("+");
    //
    if  ( appContext.pBufferReceiveNode != 0 ) {
        err = appContext.pBufferReceiveNode->flush();
        MY_LOGW_IF(OK!=err, "BufferReceiveNode::flush err:%d(%s)", err, ::strerror(-err));
    }
    //
    if  ( appContext.pAppStreamManager != 0 ) {
        err = appContext.pAppStreamManager->waitUntilDrained(::ms2ns(1000));
        MY_LOGW_IF(OK!=err, "AppStreamManager::waitUntilDrained err:%d(%s)", err, ::strerror(-err));
    }
    //
    MY_LOGD("-");
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
Cam3ExternalDeviceImp::
enableRequesting()
{
    Mutex::Autolock _lRequesting(mRequestingLock);
    ::android_atomic_release_store(1, &mRequestingAllowed);
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
Cam3ExternalDeviceImp::
disableRequesting()
{
    Mutex::Autolock _lRequesting(mRequestingLock);
    ::android_atomic_release_store(0, &mRequestingAllowed);
}


/******************************************************************************
 *
 ******************************************************************************/
status_t
Cam3ExternalDeviceImp::
onConfigureStreamsLocked(
    camera3_stream_configuration_t* stream_list
)
{
    MERROR err = OK;
    MY_LOGD("+");
    {
        disableRequesting();
        flushAndWait(getSafeAppContext());
    }
    //
    Mutex::Autolock _l(mAppContextLock);
    //
    if  ( mAppContext.pAppStreamManager != 0 ) {
        mAppContext.pAppStreamManager->destroy();
        mAppContext.pAppStreamManager = NULL;
    }
    //
    mAppContext.pAppStreamManager = IAppStreamManager::create(
        getOpenId(),
        mpCallbackOps,
        NSMetadataProviderManager::valueFor(getOpenId())
    );
    if  ( mAppContext.pAppStreamManager == 0 ) {
        MY_LOGE("IAppStreamManager::create");
        return -ENODEV;
    }
    //
    if  ( mAppContext.pBufferReceiveNode != 0 ) {
        mAppContext.pBufferReceiveNode->uninit();
        mAppContext.pBufferReceiveNode = NULL;
    }
    //
    mAppContext.pBufferReceiveNode = BufferReceiveNode::createInstance();
    if  ( mAppContext.pBufferReceiveNode == 0 ) {
        MY_LOGE("BufferReceiveNode::createInstance");
        return -ENODEV;
    }
    NSCam::v3::BufferReceiveNode::InitParams params;
    params.pExternal    = pExternal;
    params.pAppCallback = this;
    mAppContext.pBufferReceiveNode->init(params);
    //
    err = mAppContext.pAppStreamManager->configureStreams(stream_list);
    if  ( OK != err ) {
        return err;
    }
    //
    IAppStreamManager::ConfigAppStreams appStreams;
    err = mAppContext.pAppStreamManager->queryConfiguredStreams(appStreams);
    if  ( OK != err ) {
        return err;
    }
    //
    {
        NSCam::v3::BufferReceiveNode::ConfigParams params;
        params.pInAppMeta           = appStreams.vMetaStreams.editValueAt(0);
        for ( size_t i = 0; i < appStreams.vImageStreams.size(); ++i )
            params.pOutImage.add(appStreams.vImageStreams.keyAt(i), appStreams.vImageStreams.valueAt(i));
        mAppContext.pBufferReceiveNode->configure(params);
    }
    //
    enableRequesting();
    MY_LOGD("-");
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
status_t
Cam3ExternalDeviceImp::
i_process_capture_request(
    camera3_capture_request_t* request
)
{
    // to debug
    {
        MUINT64 start = static_cast<MUINT64>(systemTime());
        MUINT64 duration = start - mProcessRequestStart;
        MY_LOGW_IF(
            duration > 40000000,
            "frameNo:%u start=%"PRId64" Too long duration between two requests:%"PRId64"= "
            "HAL duration:%"PRId64" + Framework duration:%"PRId64,
            request->frame_number,
            start,
            duration,
            mProcessRequestEnd - mProcessRequestStart,
            duration - (mProcessRequestEnd - mProcessRequestStart)
        );
        mProcessRequestStart = start;
    }
    MERROR err = OK;
    AppContext appContext = getSafeAppContext();
    BufferReceiveNode* pBufferReceiveNode  = appContext.pBufferReceiveNode.get();
    IAppStreamManager* pAppStreamManager   = appContext.pAppStreamManager.get();
    IAppStreamManager::Request appRequest;
    //
    {
        Mutex::Autolock _lRequesting(mRequestingLock);
        //
        if  ( 0 == ::android_atomic_acquire_load(&mRequestingAllowed) ) {
            MY_LOGW("frameNo:%d - submitting during flushing", request->frame_number);
            flushRequest(request);
            return OK;
        }
        //
        if  ( pAppStreamManager == 0 || pBufferReceiveNode == 0 ) {
            MY_LOGE("Bad IAppStreamManager:%p pBufferReceiveNode:%p", pAppStreamManager, pBufferReceiveNode);
            return -ENODEV;
        }
        //
        err = pAppStreamManager->createRequest(request, appRequest);
        if  ( OK != err ) {
            return err;
        }
        //
        err = pAppStreamManager->registerRequest(appRequest);
        if  ( OK != err ) {
            return err;
        }
    }
    //
    android::sp<NSCam::v3::BufferReceiveNode::IRequest> appIRequest = new NSCam::v3::BufferReceiveNode::IRequest;
    {
#define _CLONE_(src, dst) \
            do  { \
                dst.setCapacity(src.size()); \
                dst.clear(); \
                for (size_t i = 0; i < src.size(); i++) { \
                    dst.add(src.keyAt(i), src.valueAt(i)); \
                } \
            } while (0) \

        appIRequest->requestNo = appRequest.frameNo;
        appIRequest->pAppCallback = this;
        appIRequest->setting = const_cast<camera_metadata*>(request->settings);
        _CLONE_(appRequest.vOutputImageBuffers, appIRequest->vOImageBuffers);
        appIRequest->vIMetaBuffers = appRequest.vInputMetaBuffers[0];

#undef  _CLONE_
    }
    //
    //  Since this call may block, it should be performed out of locking.
    err = pBufferReceiveNode->submitRequest(appIRequest);
    if  ( OK != err ) {
        return err;
    }
    // to debug
    {
        mProcessRequestEnd = static_cast<MUINT64>(systemTime());
    }
    //
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
Cam3ExternalDeviceImp::
flushRequest(
    camera3_capture_request_t* request
)   const
{
    MY_LOGW("frameNo:%d", request->frame_number);
    //
    {
        camera3_notify_msg msg;
        msg.type = CAMERA3_MSG_ERROR;
        msg.message.error.frame_number = request->frame_number;
        msg.message.error.error_stream = NULL;
        msg.message.error.error_code   = CAMERA3_MSG_ERROR_REQUEST;
        mpCallbackOps->notify(mpCallbackOps, &msg);
    }
    //
    {
        //camera3_capture_request_t::input_buffer
        Vector<camera3_stream_buffer_t> input_buffers;
        if  ( request->input_buffer )
        {
            input_buffers.resize(1);
            camera3_stream_buffer_t& buffer = input_buffers.editItemAt(0);
            buffer = *request->input_buffer;
            buffer.release_fence = buffer.acquire_fence;
            buffer.acquire_fence = -1;;
        }
        //camera3_capture_request_t::output_buffers
        Vector<camera3_stream_buffer_t> output_buffers;
        output_buffers.resize(request->num_output_buffers);
        for (uint32_t i = 0; i < request->num_output_buffers; i++)
        {
            camera3_stream_buffer_t& buffer = output_buffers.editItemAt(i);
            buffer = request->output_buffers[i];
            buffer.status = CAMERA3_BUFFER_STATUS_ERROR;
            buffer.release_fence = buffer.acquire_fence;
            buffer.acquire_fence = -1;
        }
        //
        camera3_capture_result const result =
        {
            frame_number        : request->frame_number,
            result              : NULL,
            num_output_buffers  : static_cast<uint32_t>(output_buffers.size()),
            output_buffers      : output_buffers.array(),
            input_buffer        : input_buffers.size() ? input_buffers.array() : NULL,
            partial_result      : 0,
        };
        mpCallbackOps->process_capture_result(mpCallbackOps, &result);
    }
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
Cam3ExternalDeviceImp::
updateFrame(
    MUINT32 const requestNo,
    MINTPTR const userId,
    bool    const lastPart,
    android::Vector<android::sp<IMetaStreamBuffer> >
                  vOutMeta
)
{
    AppContext appContext = getSafeAppContext();
    //
    IAppStreamManager* pAppStreamManager = appContext.pAppStreamManager.get();
    if  ( pAppStreamManager == 0 ) {
        MY_LOGE("NULL AppStreamManager");
        return;
    }
    //
    pAppStreamManager->updateResult(requestNo, userId, vOutMeta, lastPart);
}