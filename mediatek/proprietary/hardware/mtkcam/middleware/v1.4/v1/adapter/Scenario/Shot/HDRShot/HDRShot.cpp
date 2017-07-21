/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2015. All rights reserved.
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

#define DEBUG_LOG_TAG "HDRShot"

#include "HDRShot.h"

#include <Modes.h>

#include <camshot/_callbacks.h>

#include <CamShotUtils.h>

#include <IHalSensor.h>

#include <metadata/mtk_platform_metadata_tag.h>

#include <hwutils/HwInfoHelper.h>
#include <hwutils/CamManager.h>

#include <LegacyPipeline/ILegacyPipeline.h>
#include <LegacyPipeline/StreamId.h>
#include <LegacyPipeline/IResourceContainer.h>
#include <LegacyPipeline/LegacyPipelineBuilder.h>
#include <LegacyPipeline/buffer/StreamBufferProviderFactory.h>
#include <LegacyPipeline/mfc/buffer/Selector.h>

#include <v3/hwnode/HDRNode.h>

#include <sys/resource.h>

#include <utils/Thread.h>

#include <common/hdr/2.2/utils/Debug.h>
#include <common/hdr/Platform.h>

using namespace android;
using namespace NSCam::Utils;
using namespace NSCam::v1;
using namespace NSCam::v1::NSLegacyPipeline;
using namespace NSCamHW;
using namespace NSShot;
using namespace NSCamShot;
using namespace NS3Av3;

// ---------------------------------------------------------------------------

class HDRThread : public Thread
{
public:
    HDRThread(const char* name, const uint32_t shotMode, const sp<HDRShot>& shot);

    status_t queryResult() const { Mutex::Autolock _l(mLock); return mStatus; }

private:
    mutable Mutex mLock;

    char mThreadName[256];

    sp<HDRShot> mShot;

    const uint32_t mShotMode;

    // used to report the execution status
    status_t mStatus;

    // good place to do one-time initializations
    status_t readyToRun();

    bool threadLoop();
};

// ---------------------------------------------------------------------------

extern "C" sp<IShot> createInstance_HdrShot(
        char const * const pszShotName,
        uint32_t const u4ShotMode,
        int32_t const  i4OpenId)
{
    sp<IShot> shot;
    sp<HDRShot> hdrShot;

    // new implementator
    hdrShot = new HDRShot(pszShotName, u4ShotMode, i4OpenId);
    if (hdrShot == NULL)
    {
        HDR_LOGE("create HDRShot failed");
        goto lbExit;
    }

    // new interface
    shot = new IShot(hdrShot);
    if (shot == NULL)
    {
        HDR_LOGE("create IShot failed");
        goto lbExit;
    }

lbExit:
    // free all resources if instantiation failed
    if ((shot == 0) && (hdrShot != NULL))
    {
        hdrShot.clear();
    }

    return shot;
}

// ---------------------------------------------------------------------------

template <typename T>
static inline MVOID updateEntry(
        IMetadata* metadata,
        MUINT32 const tag,
        T const& val)
{
    if (NULL == metadata)
    {
        HDR_LOGE("metadata is NULL");
        return;
    }

    IMetadata::IEntry entry(tag);
    entry.push_back(val, Type2Type<T>());
    metadata->update(entry.tag(), entry);
}

static sp<CallbackBufferPool> prepareCallbackBufferPool(
        sp<BufferCallbackHandler>& callbackHandler,
        const sp<IImageStreamInfo>& streamInfo,
        bool allocateBuffer)
{
    sp<CallbackBufferPool> cbBufferPool = new CallbackBufferPool(streamInfo);

    if (allocateBuffer)
    {
        cbBufferPool->allocateBuffer(
                streamInfo->getStreamName(),
                streamInfo->getMaxBufNum(),
                streamInfo->getMinInitBufNum());
    }
    callbackHandler->setBufferPool(cbBufferPool);

    return cbBufferPool;
}

auto getRegisteredBuffer = [](
        KeyedVector< MINT32, sp<IImageBuffer> >& bufferContainer,
        ECamShotImgBufType type)
{
    return (bufferContainer.indexOfKey(type) >= 0 ?
            bufferContainer.editValueFor(type) : NULL);
};

auto dumpImageStreamInfo = [](const sp<IImageStreamInfo>& info)
{
    HDR_LOGD("streamInfo(%#08x) name(%s) format(%#08x) size(%dx%d)",
            info->getStreamId(), info->getStreamName(),
            info->getImgFormat(), info->getImgSize().w, info->getImgSize().h);
};

// ---------------------------------------------------------------------------

HDRThread::HDRThread(
        const char* name, const uint32_t shotMode, const sp<HDRShot>& shot)
    : mShot(shot)
    , mShotMode(shotMode)
    , mStatus(NO_ERROR)
{
    snprintf(mThreadName, sizeof(mThreadName), "%s", name);
}

status_t HDRThread::readyToRun()
{
    //  set thread policy & priority
    //  NOTE:
    //  Even if pthread_create() with SCHED_OTHER policy, a newly-created thread
    //  may inherit the non-SCHED_OTHER policy & priority of the thread creator.
    //  And thus, we must set the expected policy & priority after a thread creation.

    // retrieve the parameters of the calling process
    struct sched_param schedParam;
    sched_getparam(0, &schedParam);

    if (setpriority(PRIO_PROCESS, 0, ANDROID_PRIORITY_NORMAL))
    {
        HDR_LOGW("set priority failed(%s)", strerror(errno));
    }

    schedParam.sched_priority = 0;
    sched_setscheduler(0, SCHED_OTHER, &schedParam);

    return OK;
}

bool HDRThread::threadLoop()
{
    HDR_TRACE_CALL();

    Mutex::Autolock _l(mLock);

    if (!mShot.get())
    {
        HDR_LOGE("shot is not set");
        mStatus = NO_INIT;
        return false;
    }

    LifeSpanObserver<HDRShot> observer(mShot.get());

    if (eShotMode_ZsdHdrShot == mShotMode)
    {
        mStatus = mShot->zsdCapture() ? NO_ERROR : UNKNOWN_ERROR;
    }
    else
    {
        mStatus = mShot->normalCapture() ? NO_ERROR : UNKNOWN_ERROR;
    }

    // one-shot execution
    return false;
}

// ---------------------------------------------------------------------------

template<typename T>
LifeSpanObserver<T>::LifeSpanObserver(const wp<T>& object)
    :mObject(object)
{
    if (mObject.promote() != NULL)
    {
        Mutex::Autolock _l(T::sInstanceLock);
        T::sInstanceCount++;

        HDR_LOGD("instanceCount(%d) +", T::sInstanceCount);
    }
}

template<typename T>
LifeSpanObserver<T>::~LifeSpanObserver()
{
    if (mObject.promote() != NULL)
    {
        Mutex::Autolock _l(T::sInstanceLock);
        T::sInstanceCount--;

        HDR_LOGD("instanceCount(%d) -", T::sInstanceCount);
    }
}

// ---------------------------------------------------------------------------

MINT32 HDRShot::sSequence = 0;
Mutex HDRShot::sInstanceLock;
MINT32 HDRShot::sInstanceCount = 0;

Mutex HDRShot::sCaptureDoneLock;
sem_t HDRShot::sSemCaptureDone;

HDRShot::HDRShot(
        const char *pszShotName,
        uint32_t const u4ShotMode,
        int32_t const i4OpenId)
    : ImpShot(pszShotName, u4ShotMode, i4OpenId),
      mDataMsgSet(0)
{
    mSequence = sSequence++;
    sSequence = (sSequence >= 32) ? 0 : sSequence;

    // init semaphore
    {
        Mutex::Autolock _l(sInstanceLock);
        if (sInstanceCount == 0)
        {
            Mutex::Autolock _l(sCaptureDoneLock);
            sem_init(&sSemCaptureDone, 0, 1);
            HDR_LOGD("init semaphore");
        }
    }
}

HDRShot::~HDRShot()
{
    if (mpPipeline.get())
    {
        mpPipeline->flush();
    }

    // destroy semaphore
    {
        Mutex::Autolock _l(sInstanceLock);
        if (sInstanceCount == 0)
        {
            Mutex::Autolock _l(sCaptureDoneLock);
            sem_destroy(&sSemCaptureDone);
            HDR_LOGD("destroy semaphore");
        }
    }
}

// metadata event handler
void HDRShot::onResultReceived(
        MUINT32    const requestNo,
        StreamId_T const streamId,
        MBOOL      const errorResult,
        IMetadata  const /*result*/)
{
    HDR_TRACE_CALL();

    HDR_LOGD("metadata request(%d) streamID(%#x)", requestNo, streamId);

    if (errorResult)
    {
        HDR_LOGE("the result metadata may be wrong...");
    }

    // TODO: need to keep result metadata for later use
    switch (streamId)
    {
        case eSTREAMID_META_APP_DYNAMIC_HDR:
            HDR_LOGD("send shutter callback");
            mpShotCallback->onCB_Shutter(true, 0);
        break;

        // TODO: enable them after verification
#if 0
        case eSTREAMID_META_APP_FULL:
        break;

        case eSTREAMID_META_HAL_FULL:
        break;
#endif

        default:
            HDR_LOGW("unsupported streamID(%#x)", streamId);
    }
}

// image event handler
MERROR HDRShot::onResultReceived(
        MUINT32    const requestNo,
        StreamId_T const streamId,
        MBOOL      const errorBuffer,
        android::sp<IImageBuffer>& pBuffer)
{
    HDR_TRACE_CALL();

    HDR_LOGD("image request(%d) streamID(%#x)", requestNo, streamId);

    if (pBuffer == NULL)
    {
        HDR_LOGE("the buffer is NULL");
        return BAD_VALUE;
    }

    if (errorBuffer)
    {
        HDR_LOGE("the content of buffer may not correct...");
    }

    pBuffer->lockBuf(LOG_TAG, eBUFFER_USAGE_SW_READ_MASK);

    uint8_t const* puBuf = (uint8_t const*)pBuffer->getBufVA(0);
    MUINT32 u4Size = pBuffer->getBitstreamSize();

    switch (streamId)
    {
        case eSTREAMID_IMAGE_PIPE_RAW_OPAQUE:
            if (eShotMode_ZsdHdrShot == mu4ShotMode)
            {
                sem_post(&sSemCaptureDone);
            }
            break;
        // TODO: enable them after verification
#if 0

        case eSTREAMID_IMAGE_PIPE_YUV_00:
            mpShotCallback->onCB_PostviewDisplay(0, u4Size, puBuf);
            break;
#endif

        case eSTREAMID_IMAGE_HDR:
            // dummy raw callback
            mpShotCallback->onCB_RawImage(0, 0, NULL);
            // set callback index to 1 and the final image flag to true
            mpShotCallback->onCB_CompressedImage_packed(0, u4Size, puBuf, 0, true);
            break;

        default:
            HDR_LOGW("unsupported streamID(%#x)", streamId);
    }

    pBuffer->unlockBuf(LOG_TAG);

    if (eShotMode_ZsdHdrShot == mu4ShotMode)
    {
        if (eSTREAMID_IMAGE_PIPE_RAW_OPAQUE == streamId)
        {
            mpBufferProvider->returnBuffer(pBuffer->getImageBufferHeap());
        }
    }

    return OK;
}

String8 HDRShot::getUserName()
{
    HDR_TRACE_CALL();

    return String8(DEBUG_LOG_TAG);
}

bool HDRShot::sendCommand(
        uint32_t const cmd,
        MUINTPTR const arg1,
        uint32_t const arg2,
        uint32_t const /*arg3*/)
{
    HDR_TRACE_CALL();

    bool ret = true;

    switch (cmd)
    {
        // eCmd_reset is to reset this class. After captures and then reset,
        // performing a new capture should work well, no matter whether previous
        // captures failed or not.
        case eCmd_reset:
            ret = onCmd_reset();
            break;
        // eCmd_capture is to perform capture.
        case eCmd_capture:
            ret = onCmd_capture();
            break;
        // eCmd_cancel is to perform cancel capture.
        case eCmd_cancel:
            onCmd_cancel();
            break;
        default:
            ret = ImpShot::sendCommand(cmd, arg1, arg2);
    }

    return ret;
}

bool HDRShot::onCmd_reset()
{
    HDR_TRACE_CALL();

    return true;
}

bool HDRShot::onCmd_capture()
{
    FUNCTION_LOG_START;

    HDR_LOGD("[%d] %s", mSequence, __FUNCTION__);

    HDR_TRACE_CALL();

    // need to wait for previous HDR input frames are received
    // before start this round
    {
        HDR_TRACE_NAME("wait_capture_done");
        HDR_LOGD("waiting for capture done...");
        sem_wait(&sSemCaptureDone);
        HDR_LOGD("wait capture done");
    }

    if (eShotMode_HdrShot == mu4ShotMode)
    {
        LifeSpanObserver<HDRShot> observer(this);

        return normalCapture();
    }

    // otherwise, asynchronous capture and return as soon as possible
    String8 name;
    name.appendFormat("[%d] worker", mSequence);
    sp<HDRThread> workerThread =
        new HDRThread(name.string(), mu4ShotMode, this);
    workerThread->run(name.string());

    FUNCTION_LOG_END_MUM;

    return true;
}

void HDRShot::onCmd_cancel()
{
    HDR_TRACE_CALL();
}

bool HDRShot::normalCapture()
{
    FUNCTION_LOG_START;

    HDR_TRACE_CALL();

    bool ret = true;
    sp<Platform::IPerf> perfService = Platform::getInstance().getPerf();

    // 1. set sensor parameters and expected callback message type
    {
        CamManager* pCamMgr = CamManager::getInstance();
        const MUINT32 scenario =
            (pCamMgr->isMultiDevice() && (pCamMgr->getFrameRate(getOpenId()) == 0)) ?
            SENSOR_SCENARIO_ID_NORMAL_PREVIEW :
            SENSOR_SCENARIO_ID_NORMAL_CAPTURE;

        mSensorParam = NSCamShot::SensorParam(
                getOpenId(),  // sensor idx
                scenario,     // Scenaio
                10,           // bit depth
                MFALSE,       // bypass delay
                MFALSE);      // bypass scenario

        EImageFormat ePostViewFmt =
            static_cast<EImageFormat>(mShotParam.miPostviewDisplayFormat);

        mDataMsgSet =
            (ePostViewFmt != eImgFmt_UNKNOWN) ?
            (ECamShot_DATA_MSG_JPEG | ECamShot_DATA_MSG_POSTVIEW) :
            ECamShot_DATA_MSG_JPEG;
    }

    // 2. create streams
    if (createStreams() != OK)
    {
        HDR_LOGE("create streams failed");
        return false;
    }

    // 3. create pipeline
    if (createPipeline() != OK)
    {
        HDR_LOGE("create pipeline failed");
        return false;
    }

    // 4. prepare app and hal metadata
    Vector<SettingSet> vSettings;
    {
        Vector<CaptureParam_T> vCaptureParams;

        // get HDR capture parameters
        HDRNode::getCaptureParamSet(getOpenId(), vCaptureParams);
        vSettings.resize(vCaptureParams.size());

        // set parameters into metadata
        for (size_t i = 0; i < vCaptureParams.size(); i++)
        {
            IMetadata::Memory capParams;
            capParams.resize(sizeof(CaptureParam_T));
            memcpy(capParams.editArray(), &vCaptureParams[i], sizeof(CaptureParam_T));

            vSettings.editItemAt(i).appSetting = mShotParam.mAppSetting;
            vSettings.editItemAt(i).halSetting = mShotParam.mHalSetting;

            // modify hal control metadata
            IMetadata &halSetting(vSettings.editItemAt(i).halSetting);
            {
                updateEntry<IMetadata::Memory>(
                        &halSetting, MTK_3A_AE_CAP_PARAM, capParams);
                updateEntry<MSize>(
                        &halSetting, MTK_HAL_REQUEST_SENSOR_SIZE, mSensorSetting.size);
                updateEntry<MUINT8>(
                        &halSetting, MTK_HAL_REQUEST_REQUIRE_EXIF, true);
            }
        }
    }

    // 5. submit setting to pipeline
    for (size_t i = 0; i < vSettings.size(); i++)
    {
        if (mpPipeline->submitSetting(
                    i, vSettings.editItemAt(i).appSetting, vSettings.editItemAt(i).halSetting) != OK)
        {
            HDR_LOGE("submit setting failed");
            return false;
        }
    }

    // 6. soldier on until pipeline has finished its job
    if (mpPipeline->waitUntilDrained() != OK)
    {
        HDR_LOGE("drained failed");
        return false;
    }

    releaseResource();

    FUNCTION_LOG_END;

    return ret;
}

bool HDRShot::zsdCapture()
{
    FUNCTION_LOG_START;

    HDR_TRACE_CALL();

    bool ret = true;

    sp<Platform::IPerf> perfService = Platform::getInstance().getPerf();

    // 1. retrieve stream buffer provider
    mpBufferProvider =
        IResourceContainer::getInstance(
                getOpenId())->queryConsumer(eSTREAMID_IMAGE_PIPE_RAW_OPAQUE);
    if (mpBufferProvider == NULL)
    {
        HDR_LOGE("can't find StreamBufferProvider in ConsumerContainer");
        return false;
    }

    // 2. set the expected callback message type
    mDataMsgSet = ECamShot_DATA_MSG_JPEG;

    // 3. create streams
    if (createStreams(mpBufferProvider) != OK)
    {
        HDR_LOGE("create streams failed");
        return false;
    }

    // 4. create pipeline
    if (createPipeline(mpBufferProvider) != OK)
    {
        HDR_LOGE("create pipeline failed");
        return false;
    }

    // 5. prepare app and hal metadata
    Vector<SettingSet> vSettings;
    Vector<MINT32>     vCaptureRequest;
    {
        Vector<CaptureParam_T> vCaptureParams;
        Vector<CaptureParam_T> vOrigCaptureParams;

        // get HDR capture parameters
        HDRNode::getCaptureParamSet(
                getOpenId(), vCaptureParams, &vOrigCaptureParams);

        const size_t captureCount = vCaptureParams.size();
        const size_t delayFrameCount = vOrigCaptureParams.size();

        // NOTE: the frame delay time is only needed under ZSD mode
        //
        // even though capture can leverage per-frame control, however,
        // to avoid unstable exposure variation is seen on the display,
        // extra frame dealy time is needed for AE convergence
        vSettings.resize(captureCount + delayFrameCount);

        vCaptureRequest.resize(captureCount);

        // append the original capture parameters at the end of
        // HDR capture parameters
        vCaptureParams.appendVector(vOrigCaptureParams);

        // set parameters into metadata
        for (size_t i = 0; i < vSettings.size(); i++)
        {
            IMetadata::Memory capParams;
            capParams.resize(sizeof(NS3Av3::CaptureParam_T));
            memcpy(capParams.editArray(), &vCaptureParams[i], sizeof(CaptureParam_T));

            vSettings.editItemAt(i).appSetting = mShotParam.mAppSetting;
            vSettings.editItemAt(i).halSetting = mShotParam.mHalSetting;

            // modify hal control metadata
            IMetadata &halSetting(vSettings.editItemAt(i).halSetting);
            {
                updateEntry<IMetadata::Memory>(
                        &halSetting, MTK_3A_AE_CAP_PARAM, capParams);
                updateEntry<MSize>(
                        &halSetting, MTK_HAL_REQUEST_SENSOR_SIZE, mSensorSetting.size);
            }

            // use capture count as capture request number
            if (i < vCaptureRequest.size())
            {
                vCaptureRequest.editItemAt(i) = i;
            }
        }
    }

    // 6. submit settings to flow control
    sp<MfcSelector> selector = new MfcSelector();
    {
        // set HDR selector into stream buffer provider
        mOriginalSelector = mpBufferProvider->querySelector();
        status_t status = mpBufferProvider->setSelector(selector);
        if (status != OK)
        {
            HDR_LOGE("change to HDR selector failed");
            return false;
        }
        HDR_LOGD("change to HDR selector");

        // send capture requests to flow control
        sp<IFeatureFlowControl> previewFlowCtrl =
            IResourceContainer::getInstance(getOpenId())->queryFeatureFlowControl();
        if (previewFlowCtrl == NULL)
        {
            HDR_LOGE("previewFlowCtrl is NULL");
            return false;
        }

        // only outout full size
        Vector<StreamId>   vDstStreams;
        vDstStreams.push_back(eSTREAMID_IMAGE_PIPE_RAW_OPAQUE);

        Vector<MINT32> resultRequestNo;
        if (previewFlowCtrl->submitRequest(
                    vSettings, vDstStreams, resultRequestNo) != OK)
        {
            HDR_LOGE("submit settings failed");
            return false;
        }

        // set expected request numbers
        selector->setWaitRequestNo(resultRequestNo);
    }

    // 7. get capture results from selector,
    //    pass app result metadata to timestamp processor and
    //    submit setting to pipeline
    for (size_t i = 0; i < vCaptureRequest.size(); i++)
    {
        IMetadata appSetting;
        IMetadata halSetting;

        // blocking call to get captured result
        getSelectResult(selector, appSetting, halSetting);

        // pass app result metadata to timestamp processor
        {
            sp<TimestampProcessor> timestampProcessor =
                mpPipeline->getTimestampProcessor().promote();
            if (timestampProcessor.get())
            {
                timestampProcessor->onResultReceived(
                        0,
                        eSTREAMID_META_APP_DYNAMIC_P1,
                        MFALSE,
                        appSetting);
            }
            else
            {
                HDR_LOGE("promote timestamp processor failed");
            }
        }

        // submit request to pipeline
        if (mpPipeline->submitSetting(
                    vCaptureRequest[i], appSetting, halSetting) != OK)
        {
            HDR_LOGE("submit setting failed");
            return false;
        }
    }
    vCaptureRequest.clear();

    // 8. soldier on until pipeline has finished its job
    if (mpPipeline->waitUntilDrained() != OK)
    {
        HDR_LOGE("drained failed");
        return false;
    }

    releaseResource();

    perfService = NULL;

    FUNCTION_LOG_END;

    return ret;
}

MUINT32 HDRShot::getRotation() const
{
    return mShotParam.mu4Transform;
}

status_t HDRShot::createStreams(const sp<StreamBufferProvider>& provider)
{
    FUNCTION_LOG_START;

    HDR_TRACE_CALL();

    MUINT32 const openId     = mSensorParam.u4OpenID;
    MUINT32 const sensorMode = mSensorParam.u4Scenario;
    MUINT32 const bitDepth   = mSensorParam.u4Bitdepth;

    MSize const postviewsize =
        MSize(mShotParam.mi4PostviewWidth, mShotParam.mi4PostviewHeight);
    MINT const postviewfmt   = mShotParam.miPostviewDisplayFormat;
    // NOTE: jpeg & thumbnail yuv only support YUY2
    MINT const yuvfmt        = eImgFmt_YUY2;
    MSize const jpegsize     = (getRotation() & eTransform_ROT_90) ?
        MSize(mShotParam.mi4PictureHeight, mShotParam.mi4PictureWidth):
        MSize(mShotParam.mi4PictureWidth, mShotParam.mi4PictureHeight);
    MSize const thumbnailsize =
        MSize(mJpegParam.mi4JpegThumbWidth, mJpegParam.mi4JpegThumbHeight);


    HwInfoHelper helper(openId);
    if (!helper.updateInfos())
    {
        HDR_LOGE("update sensor static information failed");
        return BAD_VALUE;
    }

    if (!helper.getSensorSize(sensorMode, mSensorSetting.size) ||
        !helper.getSensorFps(sensorMode, mSensorSetting.fps) ||
        !helper.queryPixelMode(
            sensorMode, mSensorSetting.fps, mSensorSetting.pixelMode))
    {
        HDR_LOGE("cannot get params about sensor");
        return BAD_VALUE;
    }

    if (provider == NULL)
    {
        HDR_LOGD("normal mode");

        // full raw
        {
            MSize size = mSensorSetting.size;
            MINT format;
            size_t stride;
            MUINT const usage =
                eBUFFER_USAGE_SW_READ_OFTEN | eBUFFER_USAGE_HW_CAMERA_READWRITE;

            if (!helper.getImgoFmt(bitDepth, format) ||
                !helper.alignPass1HwLimitation(
                    mSensorSetting.pixelMode, format, true, size, stride))
            {
                HDR_LOGE("wrong parameter for IMGO");
                return BAD_VALUE;
            }

            mpImageInfo_fullRaw = createRawImageStreamInfo(
                    "HDRShot:FullRaw",
                    eSTREAMID_IMAGE_PIPE_RAW_OPAQUE,
                    eSTREAMTYPE_IMAGE_INOUT,
                    4, 1,
                    usage, format, size, stride);

            if (mpImageInfo_fullRaw == NULL)
            {
                HDR_LOGE("create ImageStreamInfo FullRaw failed");
                return NO_INIT;
            }

            dumpImageStreamInfo(mpImageInfo_fullRaw);
        }

        // resized raw
        if (helper.isRaw() && !!postviewsize)
        {
            MSize size;
            MINT format;
            size_t stride;
            MUINT const usage =
                eBUFFER_USAGE_SW_READ_OFTEN | eBUFFER_USAGE_HW_CAMERA_READWRITE;

            if(!helper.getRrzoFmt(bitDepth, format) ||
                    !helper.alignRrzoHwLimitation(
                        postviewsize, mSensorSetting.size, size) ||
                    !helper.alignPass1HwLimitation(
                        mSensorSetting.pixelMode, format, false, size, stride))
            {
                HDR_LOGE("wrong parameter for RRZO");
                return BAD_VALUE;
            }

            mpImageInfo_resizedRaw = createRawImageStreamInfo(
                    "HDRShot:ResizedRaw",
                    eSTREAMID_IMAGE_PIPE_RAW_RESIZER,
                    eSTREAMTYPE_IMAGE_INOUT,
                    4, 0,
                    usage, format, size, stride);

            if (mpImageInfo_resizedRaw == NULL)
            {
                HDR_LOGE("create ImageStreamInfo ResizedRaw failed");
                return BAD_VALUE;
            }

            dumpImageStreamInfo(mpImageInfo_resizedRaw);
        }
    }
    else
    {
        HDR_LOGD("ZSD mode");

        // ZSD mode
        mpImageInfo_fullRaw = provider->queryImageStreamInfo();
        mpImageInfo_resizedRaw = NULL;

        dumpImageStreamInfo(mpImageInfo_fullRaw);
    }

    // postview
    if (mDataMsgSet & ECamShot_DATA_MSG_POSTVIEW)
    {
        MSize size  = postviewsize;
        MINT format = postviewfmt;
        MUINT const usage = 0; //not necessary here
        MUINT32 transform = 0;

        mpImageInfo_yuvPostview = createImageStreamInfo(
                "HDRShot:Postview",
                eSTREAMID_IMAGE_PIPE_YUV_00,
                eSTREAMTYPE_IMAGE_INOUT,
                1, 1,
                usage, format, size, transform);

        if (mpImageInfo_yuvPostview == NULL)
        {
            HDR_LOGE("create ImageStreamInfo Postview failed");
            return BAD_VALUE;
        }

        dumpImageStreamInfo(mpImageInfo_yuvPostview);
    }

    // jpeg yuv
    if (mDataMsgSet & (ECamShot_DATA_MSG_YUV|ECamShot_DATA_MSG_JPEG))
    {
        MSize size  = jpegsize;
        MINT format = yuvfmt;
        MUINT const usage = 0; //not necessary here
        MUINT32 transform = getRotation();

        mpImageInfo_yuvJpeg = createImageStreamInfo(
                "HDRShot:YuvJpeg",
                eSTREAMID_IMAGE_PIPE_YUV_JPEG,
                eSTREAMTYPE_IMAGE_INOUT,
                1, 1,
                usage, format, size, transform);

        if (mpImageInfo_yuvJpeg == NULL)
        {
            HDR_LOGE("create ImageStreamInfo YuvJpeg failed");
            return BAD_VALUE;
        }

        dumpImageStreamInfo(mpImageInfo_yuvJpeg);
    }

    // thumbnail yuv
    if (mDataMsgSet & ECamShot_DATA_MSG_JPEG)
    {
        MSize size  = thumbnailsize;
        MINT format = yuvfmt;
        MUINT const usage = 0; //not necessary here
        MUINT32 transform = 0;

        mpImageInfo_yuvThumbnail = createImageStreamInfo(
                "HDRShot:YuvThumbnail",
                eSTREAMID_IMAGE_PIPE_YUV_THUMBNAIL,
                eSTREAMTYPE_IMAGE_INOUT,
                1, 1,
                usage, format, size, transform);

        if (mpImageInfo_yuvThumbnail == NULL)
        {
            HDR_LOGE("create ImageStreamInfo YuvThumbnail failed");
            return BAD_VALUE;
        }

        dumpImageStreamInfo(mpImageInfo_yuvThumbnail);
    }

    // jpeg
    if (mDataMsgSet & ECamShot_DATA_MSG_JPEG)
    {
        MSize size  = jpegsize;
        MINT format = eImgFmt_BLOB;
        MUINT const usage = 0; //not necessary here
        MUINT32 transform = 0;

        mpImageInfo_jpeg = createImageStreamInfo(
                "HDRShot:Jpeg",
                eSTREAMID_IMAGE_HDR,
                eSTREAMTYPE_IMAGE_INOUT,
                1, 1,
                usage, format, size, transform);

        if (mpImageInfo_jpeg == NULL)
        {
            HDR_LOGE("create ImageStreamInfo Jpeg failed");
            return BAD_VALUE;
        }

        dumpImageStreamInfo(mpImageInfo_jpeg);
    }

    FUNCTION_LOG_END_MUM;

    return OK;
}

status_t HDRShot::createPipeline(const sp<StreamBufferProvider>& provider)
{
    FUNCTION_LOG_START;

    HDR_TRACE_CALL();

    LegacyPipelineBuilder::ConfigParams LPBConfigParams;
    LPBConfigParams.mode = LegacyPipelineMode_T::PipelineMode_Hdr;
    LPBConfigParams.enableEIS = MFALSE;
    LPBConfigParams.enableLCS = MFALSE;

    sp<LegacyPipelineBuilder> builder =
        LegacyPipelineBuilder::createInstance(
                mSensorParam.u4OpenID,
                "HDRShot",
                LPBConfigParams);

    CHECK_OBJECT(builder, BAD_VALUE);

    mpCallbackHandler = new BufferCallbackHandler(mSensorParam.u4OpenID);

    // register image listener
    mpCallbackHandler->setImageCallback(this);

    sp<StreamBufferProviderFactory> bufferProvider =
        StreamBufferProviderFactory::createInstance();

    Vector<PipelineImageParam> vImageParam;

    if (provider == NULL)
    {
        HDR_LOGD("normal mode");

        // normal mode
        //
        {
            sp<IScenarioControl> scenarioCtrl =
                IScenarioControl::create(mSensorParam.u4OpenID);
            if (scenarioCtrl == NULL)
            {
                HDR_LOGE("get scenario control failed");
                return UNKNOWN_ERROR;
            }

            IScenarioControl::ControlParam param;
            param.scenario   = IScenarioControl::Scenario_Capture;
            param.sensorSize = mSensorSetting.size;
            param.sensorFps  = mSensorSetting.fps;

            scenarioCtrl->enterScenario(param);
            builder->setScenarioControl(scenarioCtrl);
        }

        // full raw
        if (mpImageInfo_fullRaw.get())
        {
            sp<IImageStreamInfo>& pStreamInfo(mpImageInfo_fullRaw);

            sp<CallbackBufferPool> cbBufferPool =
                prepareCallbackBufferPool(mpCallbackHandler, pStreamInfo, true);
            bufferProvider->setImageStreamInfo(pStreamInfo);
            bufferProvider->setUsersPool(cbBufferPool);

            PipelineImageParam imgParam =
            {
                .pInfo     = pStreamInfo,
                .pProvider = bufferProvider->create(),
                .usage     = 0
            };
            vImageParam.push_back(imgParam);

            HDR_LOGD("add full raw");
        }

        // resized raw
        if (mpImageInfo_resizedRaw.get())
        {
            sp<IImageStreamInfo>& pStreamInfo(mpImageInfo_resizedRaw);

            PipelineImageParam imgParam =
            {
                .pInfo     = pStreamInfo,
                .pProvider = NULL,
                .usage     = 0
            };
            vImageParam.push_back(imgParam);

            HDR_LOGD("add resized raw");
        }

        PipelineSensorParam sensorParam =
        {
            .mode      = mSensorParam.u4Scenario,
            .rawType   = mSensorParam.u4RawType,
            .size      = mSensorSetting.size,
            .fps       = (MUINT32)mSensorSetting.fps,
            .pixelMode = mSensorSetting.pixelMode,
            .vhdrMode  = SENSOR_VHDR_MODE_NONE
        };

        if (OK != builder->setSrc(sensorParam))
        {
            HDR_LOGE("normal mode - setSrc failed");
            return UNKNOWN_ERROR;
        }
    }
    else
    {
        HDR_LOGD("ZSD mode");

        // ZSD mode
        //
        sp<IImageStreamInfo>& pStreamInfo(mpImageInfo_fullRaw);

        mpP1Pool = prepareCallbackBufferPool(mpCallbackHandler, pStreamInfo, false);
        bufferProvider->setImageStreamInfo(pStreamInfo);
        bufferProvider->setUsersPool(mpP1Pool);

        PipelineImageParam imageParam =
        {
            .pInfo     = pStreamInfo,
            .pProvider = bufferProvider->create(),
            .usage     = 0
        };

        if (OK != builder->setSrc(imageParam))
        {
            HDR_LOGE("ZSD mode - setSrc failed");
            return UNKNOWN_ERROR;
        }
    }

    // jpeg yuv
    if (mpImageInfo_yuvJpeg.get())
    {
        sp<IImageStreamInfo>& pStreamInfo(mpImageInfo_yuvJpeg);

        sp<CallbackBufferPool> cbBufferPool =
            prepareCallbackBufferPool(mpCallbackHandler, pStreamInfo, true);
        bufferProvider->setImageStreamInfo(pStreamInfo);
        bufferProvider->setUsersPool(cbBufferPool);

        PipelineImageParam imageParam =
        {
            .pInfo     = pStreamInfo,
            .pProvider = bufferProvider->create(),
            .usage     = 0
        };
        vImageParam.push_back(imageParam);

        HDR_LOGD("add jpeg");
    }

    // postview
    if (mpImageInfo_yuvPostview.get())
    {
        sp<IImageStreamInfo>& pStreamInfo(mpImageInfo_yuvPostview);

        sp<CallbackBufferPool> cbBufferPool =
            prepareCallbackBufferPool(mpCallbackHandler, pStreamInfo, true);
        bufferProvider->setImageStreamInfo(pStreamInfo);
        bufferProvider->setUsersPool(cbBufferPool);

        PipelineImageParam imageParam =
        {
            .pInfo     = pStreamInfo,
            .pProvider = bufferProvider->create(),
            .usage     = 0
        };
        vImageParam.push_back(imageParam);

        HDR_LOGD("add postview");
    }

    // thumbnail yuv
    if (mpImageInfo_yuvThumbnail.get())
    {
        sp<IImageStreamInfo>& pStreamInfo(mpImageInfo_yuvThumbnail);

        PipelineImageParam imageParam =
        {
            .pInfo     = pStreamInfo,
            .pProvider = NULL,
            .usage     = 0
        };
        vImageParam.push_back(imageParam);

        HDR_LOGD("add thumbnail yuv");
    }

    // jpeg
    if (mpImageInfo_jpeg.get())
    {
        sp<IImageStreamInfo>& pStreamInfo(mpImageInfo_jpeg);

        sp<CallbackBufferPool> cbBufferPool =
            prepareCallbackBufferPool(mpCallbackHandler, pStreamInfo, true);
        bufferProvider->setImageStreamInfo(pStreamInfo);
        bufferProvider->setUsersPool(cbBufferPool);

        PipelineImageParam imageParam =
        {
            .pInfo     = pStreamInfo,
            .pProvider = bufferProvider->create(),
            .usage     = 0
        };
        vImageParam.push_back(imageParam);

        HDR_LOGD("add jpeg yuv");
    }

    HDR_LOGD("image param count(%zu)", vImageParam.size());

    if (OK != builder->setDst(vImageParam))
    {
        HDR_LOGE("setDst failed");
        return UNKNOWN_ERROR;
    }

    mpPipeline = builder->create();
    CHECK_OBJECT(mpPipeline, BAD_VALUE);

    {
        sp<ResultProcessor> resultProcessor =
            mpPipeline->getResultProcessor().promote();
        CHECK_OBJECT(resultProcessor, BAD_VALUE);

        // register partial metadata listener
        resultProcessor->registerListener(0, 0, true, this);
        // register full metadata listener
        resultProcessor->registerListener(0, 0, false, this);
    }

    FUNCTION_LOG_END_MUM;

    return OK;
}

status_t HDRShot::getSelectResult(
        const sp<ISelector>& selector,
        IMetadata& appSetting,
        IMetadata& halSetting)
{
    if (selector == NULL)
    {
        HDR_LOGE("selector is NULL");
        return NO_INIT;
    }

    MINT32 requestNo;
    Vector<ISelector::MetaItemSet> resultMeta;
    sp<IImageBufferHeap> heap;
    status_t err = selector->getResult(requestNo, resultMeta, heap);
    if (err)
    {
        HDR_LOGE("get select result failed(%s)", strerror(-err));
        return err;
    }

    HDR_LOGD("getSelectResult(%d)", requestNo);

    for (size_t i = 0; i < resultMeta.size(); i++)
    {
        if (resultMeta[i].id == eSTREAMID_META_APP_DYNAMIC_P1)
        {
            appSetting = resultMeta[i].meta;
            HDR_LOGD("P1 app dynamic found");
        }
        else if (resultMeta[i].id == eSTREAMID_META_HAL_DYNAMIC_P1)
        {
            halSetting = resultMeta[i].meta;
            HDR_LOGD("P1 hal dynamic found");
        }
    }

    if (appSetting.isEmpty() || halSetting.isEmpty())
    {
        HDR_LOGE("either app(%s) or hal(%s) result metadata is empty",
                appSetting.isEmpty() ? "y" : "n",
                halSetting.isEmpty() ? "y" : "n");
        return BAD_VALUE;
    }

    // get and register captured image buffer into callback buffer pool
    sp<IImageBuffer> buffer = heap->createImageBuffer();
    if (buffer == NULL)
    {
        HDR_LOGE("result buffer is NULL");
        return BAD_VALUE;
    }

    err = mpP1Pool->addBuffer(buffer);
    mpSelectedBuffer.push_back(buffer);

    return err;
}

status_t HDRShot::releaseResource()
{
    HDR_TRACE_CALL();

    HDR_LOGD("[%d] release resource", mSequence);

    if (eShotMode_ZsdHdrShot == mu4ShotMode)
    {
        HDR_LOGE_IF(mpP1Pool == NULL, "P1 pool is NULL");
        if (mpP1Pool.get())
        {
            mpP1Pool->flush();
            mpP1Pool.clear();
        }

        // return buffer to selector
        {
            sp<ISelector> selector = mpBufferProvider->querySelector();
            HDR_LOGE_IF(selector == NULL, "selector is NULL");
            if (selector.get())
            {
                for (size_t i = 0; i < mpSelectedBuffer.size(); i++)
                {
                    HDR_LOGD("return buffer(%zu)", i);
                    selector->returnBuffer(
                            mpSelectedBuffer[i]->getImageBufferHeap());
                }
            }
            mpSelectedBuffer.clear();
        }

        // restore selector
        HDR_LOGE_IF(mpBufferProvider == NULL, "StreamBufferProvider is NULL");
        if (mpBufferProvider.get())
        {
            status_t status = mpBufferProvider->setSelector(mOriginalSelector);
            if (status != OK)
            {
                HDR_LOGE("change to original selector failed");
                return UNKNOWN_ERROR;
            }
            HDR_LOGD("change to original selector");
        }
    }

    mpCallbackHandler.clear();

    return OK;
}
