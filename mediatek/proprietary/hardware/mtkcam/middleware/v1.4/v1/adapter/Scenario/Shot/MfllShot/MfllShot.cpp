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

#define DEBIG_LOG_TAG "MfllShot"

#include "MfllShot.h"

#include <Modes.h>

#include <camshot/_callbacks.h>

#include <CamShotUtils.h>

#include <IHalSensor.h>

#include <metadata/mtk_platform_metadata_tag.h>

#include <hwutils/HwInfoHelper.h>
#include <hwutils/CamManager.h>

#include <LegacyPipeline/ILegacyPipeline.h>
#include <LegacyPipeline/StreamId.h>
#include <LegacyPipeline/buffer/StreamBufferProvider.h>
#include <LegacyPipeline/IResourceContainer.h>
#include <LegacyPipeline/buffer/StreamBufferProviderFactory.h>
#include <LegacyPipeline/LegacyPipelineBuilder.h>

#include <v3/hwnode/MfllNode.h>

#include <common/mfnr/MfllLog.h>

using namespace android;
using namespace NSCam::Utils;
using namespace NSCam::v1;
using namespace NSCam::v1::NSLegacyPipeline;
using namespace NSCamHW;
using namespace NSShot;
using namespace NSCamShot;
using namespace NS3Av3;


extern "C" sp<IShot> createInstance_MfllShot(
        char const * const pszShotName,
        uint32_t const u4ShotMode,
        int32_t const  i4OpenId)
{
    sp<IShot> shot;
    sp<MfllShot> myShot;

    // new implementator
    myShot = new MfllShot(pszShotName, u4ShotMode, i4OpenId);
    if (myShot == NULL)
    {
        mfllLogE("create MfllShot failed");
        goto lbExit;
    }

    // new interface
    shot = new IShot(myShot);
    if (shot == NULL)
    {
        mfllLogE("create IShot failed");
        goto lbExit;
    }

lbExit:
    // free all resources if instantiation failed
    if ((shot == 0) && (myShot != NULL))
    {
        myShot.clear();
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
        mfllLogE("metadata is NULL");
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
    mfllLogD("streamInfo(%#08x) name(%s) format(%#08x) size(%dx%d)",
            info->getStreamId(), info->getStreamName(),
            info->getImgFormat(), info->getImgSize().w, info->getImgSize().h);
};

// ---------------------------------------------------------------------------

MfllShot::MfllShot(
        const char *pszShotName,
        uint32_t const u4ShotMode,
        int32_t const i4OpenId)
    : ImpShot(pszShotName, u4ShotMode, i4OpenId),
      mDataMsgSet(0)
{
}

MfllShot::~MfllShot()
{
}

// metadata event handler
void MfllShot::onResultReceived(
        MUINT32    const requestNo,
        StreamId_T const streamId,
        MBOOL      const errorResult,
        IMetadata  const /*result*/)
{
    mfllAutoLogFunc();
    mfllAutoTraceFunc();

    mfllLogD("metadata request(%d) streamID(%#x)", requestNo, streamId);

    if (errorResult)
    {
        mfllLogE("the result metadata may be wrong...");
    }

    // TODO: need to keep result metadata for later use

    if (eSTREAMID_META_APP_DYNAMIC_MFLL == streamId)
    {
        mfllLogD("send shutter callback");
        mpShotCallback->onCB_Shutter(true, 0);
    }
}

// image event handler
MERROR MfllShot::onResultReceived(
        MUINT32    const requestNo,
        StreamId_T const streamId,
        MBOOL      const errorBuffer,
        android::sp<IImageBuffer>& pBuffer)
{
    mfllAutoTraceFunc();

    mfllLogD("image request(%d) streamID(%#x)", requestNo, streamId);

    if (pBuffer == NULL)
    {
        mfllLogE("the buffer is NULL");
        return BAD_VALUE;
    }

    if (errorBuffer)
    {
        mfllLogE("the content of buffer may not correct...");
    }

    pBuffer->lockBuf(LOG_TAG, eBUFFER_USAGE_SW_READ_MASK);

    uint8_t const* puBuf = (uint8_t const*)pBuffer->getBufVA(0);
    MUINT32 u4Size = pBuffer->getBitstreamSize();

    switch (streamId)
    {
    // TODO: enable them after verification
#if 0
        case eSTREAMID_IMAGE_PIPE_RAW_OPAQUE:
            mpShotCallback->onCB_RawImage(0, u4Size, puBuf);
            break;

        case eSTREAMID_IMAGE_PIPE_YUV_00:
            mpShotCallback->onCB_PostviewDisplay(0, u4Size, puBuf);
            break;
#endif

        case eSTREAMID_IMAGE_MFLL:
            mpShotCallback->onCB_RawImage(0, 0, NULL);
            mpShotCallback->onCB_CompressedImage_packed(0, u4Size, puBuf, 0, true);
            break;

        default:
            mfllLogW("not supported streamID(%#x)", streamId);
    }

    pBuffer->unlockBuf(LOG_TAG);

    if (eShotMode_ZsdMfllShot == mu4ShotMode)
    {
        if (eSTREAMID_IMAGE_PIPE_RAW_OPAQUE == streamId)
        {
            mpBufferProvider->returnBuffer(pBuffer->getImageBufferHeap());
        }
    }

    return OK;
}

String8 MfllShot::getUserName()
{
    return String8(MFLL_LOG_KEYWORD);
}

// interface of ISelector
status_t MfllShot::selectResult(
        MINT32                  rRequestNo,
        Vector<MetaItemSet>     rvResult,
        sp<IImageBufferHeap>    rpHeap,
        MBOOL                   /*errorResult*/)
{
    mfllAutoLogFunc();
    mfllAutoTraceFunc();

    status_t err = OK;

    Mutex::Autolock _l(mCaptureLock);

    for (size_t i = 0; i < mvPreviewRequestNo.size(); i++)
    {
        if (rRequestNo == mvPreviewRequestNo[i])
        {
            if (rvResult.size() != 2)
            {
                mfllLogE("result metadata from MfllSelector is incomplete");
                err = NOT_ENOUGH_DATA;
                goto lbExit;
            }

            IMetadata *appSetting = &mShotParam.mAppSetting;
            IMetadata *halSetting = NULL;
            for (size_t j = 0; j < rvResult.size(); j++)
            {
                if (rvResult[j].id == eSTREAMID_META_HAL_DYNAMIC_P1)
                {
                    halSetting = &rvResult.editItemAt(i).meta;
                    break;
                }
            }

            if (halSetting == NULL)
            {
                mfllLogE("cannot find hal setting from MfllSelector");
                err = INVALID_OPERATION;
                goto lbExit;
            }

            // get and register input image buffer for later usage
            {
                sp<IImageBuffer> buf = rpHeap->createImageBuffer();
                mpP1Pool->addBuffer(buf);
            }

            if (mpPipeline->submitSetting(
                        mvCaptureRequestNo[0], *appSetting, *halSetting) != OK)
            {
                mfllLogE("submit setting failed");
                err = INVALID_OPERATION;
                goto lbExit;
            }

            mvCaptureRequestNo.removeAt(0);
        }
    }

    // clear preview requests
    mvPreviewRequestNo.clear();

    // notify the main thread that all of input farmes are received
    // from the preview pipeline
    mCondCaptureDone.broadcast();

lbExit:
    if (rpHeap != NULL)
    {
        mpBufferProvider->returnBuffer(rpHeap);
    }

    return err;
}

bool MfllShot::sendCommand(
        uint32_t const cmd,
        MUINTPTR const arg1,
        uint32_t const arg2,
        uint32_t const /*arg3*/)
{
    mfllAutoLogFunc();
    mfllAutoTraceFunc();

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

bool MfllShot::onCmd_reset()
{
    return true;
}

bool MfllShot::onCmd_capture()
{
    if (eShotMode_ZsdMfllShot == mu4ShotMode)
        return ZsdCapture();

    return NormalCapture();
}

void MfllShot::onCmd_cancel()
{
}

bool MfllShot::NormalCapture()
{
    // to make sure releaseResource() can be called when this function returns
    class scopedVar
    {
        public:
            scopedVar(MfllShot* shot) : mShot(shot) {}
            ~scopedVar()
            {
                sp<MfllShot> shot = mShot.promote();
                if (shot.get())
                {
                    shot->releaseResource();
                }
            }
        private:
            wp<MfllShot> mShot;
    } _localVar(this);

    mfllAutoLogFunc();

    mfllAutoTraceFunc();

    bool ret = true;

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
            (ECamShot_DATA_MSG_JPEG | ECamShot_DATA_MSG_POSTVIEW | ECamShot_DATA_MSG_EIS) :
            (ECamShot_DATA_MSG_JPEG | ECamShot_DATA_MSG_EIS);
    }

    // 2. create streams
    if (createStreams() != OK)
    {
        mfllLogE("create streams failed");
        return false;
    }

    // 3. create pipeline
    if (createPipeline() != OK)
    {
        mfllLogE("create pipeline failed");
        return false;
    }

    // 4. prepare app and hal metadata
    Vector<SettingSet> vSettings;
    {
        Vector<CaptureParam_T> vCaptureParams;

        // get Mfll capture parameters
        MfllNode::getCaptureParamSet(getOpenId(), vCaptureParams);
        vSettings.resize(vCaptureParams.size());

        // set parameters into metadata
        for (size_t i = 0; i < vCaptureParams.size(); i++)
        {
            IMetadata::Memory capParams;
            capParams.resize(sizeof(CaptureParam_T));
            memcpy(capParams.editArray(), &vCaptureParams[i], sizeof(CaptureParam_T));

            vSettings.editItemAt(i).appSetting = mShotParam.mAppSetting;
            vSettings.editItemAt(i).halSetting = mShotParam.mHalSetting;

            // modify app control metadata
            IMetadata &appSetting(vSettings.editItemAt(i).appSetting);
            {
                updateEntry<MUINT8>(
                        &appSetting,
                        MTK_CONTROL_VIDEO_STABILIZATION_MODE,
                        MTK_CONTROL_VIDEO_STABILIZATION_MODE_ON
                        );
            }

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
            mfllLogE("submit setting failed");
            return false;
        }
    }

    // 6. soldier on until pipeline has finished its job
    if (mpPipeline->waitUntilDrained() != OK)
    {
        mfllLogE("drained failed");
        return false;
    }

    return ret;
}

bool MfllShot::ZsdCapture()
{
    // to make sure releaseResource() can be called when this function returns
    class scopedVar
    {
        public:
            scopedVar(MfllShot* shot) : mShot(shot) {}
            ~scopedVar()
            {
                sp<MfllShot> shot = mShot.promote();
                if (shot.get())
                {
                    shot->releaseResource();
                }
            }
        private:
            wp<MfllShot> mShot;
    } _localVar(this);

    mfllAutoLogFunc();

    mfllAutoTraceFunc();

    bool ret = true;

    // 1. retrieve feature flow control, stream buffer provider
    // 2. backup the original selector and set a new one
    sp<IFeatureFlowControl> previewFlowCtrl;
    {
        Mutex::Autolock _l(mCaptureLock);

        sp<IResourceContainer> resourceContainer =
            IResourceContainer::getInstance(getOpenId());

        previewFlowCtrl = resourceContainer->queryFeatureFlowControl();

        mpBufferProvider =
            resourceContainer->queryConsumer(eSTREAMID_IMAGE_PIPE_RAW_OPAQUE);
        if (mpBufferProvider == NULL)
        {
            mfllLogE("can't find StreamBufferProvider in ConsumerContainer");
            return false;
        }

        // backup the original selector and
        // set a new one into stream buffer provider
        mOriginalSelector = mpBufferProvider->querySelector();
        mpBufferProvider->setSelector(this);
    }

    // 3. set the expected callback message type
    mDataMsgSet = ECamShot_DATA_MSG_JPEG;

    // 4. create streams
    if (createStreams(mpBufferProvider) != OK)
    {
        mfllLogE("create streams failed");
        return false;
    }

    // 5. create pipeline
    if (createPipeline(mpBufferProvider) != OK)
    {
        mfllLogE("create pipeline failed");
        return false;
    }

    // 6. prepare app and hal metadata
    Vector<SettingSet> vSettings;
    Vector<StreamId> vDstStreams;
    {
        Vector<CaptureParam_T> vCaptureParams;

        // get Mfll capture parameters
        MfllNode::getCaptureParamSet(getOpenId(), vCaptureParams);
        vSettings.resize(vCaptureParams.size());

        mvCaptureRequestNo.resize(vCaptureParams.size());

        // set parameters into metadata
        for (size_t i = 0; i < vCaptureParams.size(); i++)
        {
            IMetadata::Memory capParams;
            capParams.resize(sizeof(NS3Av3::CaptureParam_T));
            memcpy(capParams.editArray(), &vCaptureParams[i], sizeof(CaptureParam_T));

            vSettings.editItemAt(i).appSetting = mShotParam.mAppSetting;
            vSettings.editItemAt(i).halSetting = mShotParam.mHalSetting;

            // modify app control metadata
            IMetadata &appSetting(vSettings.editItemAt(i).appSetting);
            {
                updateEntry<MUINT8>(
                        &appSetting,
                        MTK_CONTROL_VIDEO_STABILIZATION_MODE,
                        MTK_CONTROL_VIDEO_STABILIZATION_MODE_ON
                        );
            }

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

            mvCaptureRequestNo.editItemAt(i) = i;
        }
    }

    // 7. submit settings to pipeline
    if (previewFlowCtrl->submitRequest(
                vSettings, vDstStreams, mvPreviewRequestNo) != OK)
    {
        mfllLogE("submit settings failed");
        return false;
    }

    // 8. wait for preview pipeline
    mCondCaptureDone.wait(mCaptureLock);

    // 9. soldier on until pipeline has finished its job
    if (mpPipeline->waitUntilDrained() != OK)
    {
        mfllLogE("drained failed");
        return false;
    }

    return ret;
}

MUINT32 MfllShot::getRotation() const
{
    return mShotParam.mu4Transform;
}

MERROR MfllShot::createStreams(const sp<StreamBufferProvider>& provider)
{
    mfllAutoLogFunc();

    mfllAutoTraceFunc();

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

    if (provider == nullptr)
    {
        mfllAutoLog("normal mode");

        // normal mode
        HwInfoHelper helper(openId);
        if (!helper.updateInfos())
        {
            mfllLogE("update sensor static information failed");
            return BAD_VALUE;
        }

        if (!helper.getSensorSize(sensorMode, mSensorSetting.size) ||
            !helper.getSensorFps(sensorMode, mSensorSetting.fps) ||
            !helper.queryPixelMode(
                sensorMode, mSensorSetting.fps, mSensorSetting.pixelMode))
        {
            mfllLogE("cannot get params about sensor");
            return BAD_VALUE;
        }

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
                mfllLogE("wrong parameter for IMGO");
                return BAD_VALUE;
            }

            mpImageInfo_fullRaw = createRawImageStreamInfo(
                    "MfllShot:FullRaw",
                    eSTREAMID_IMAGE_PIPE_RAW_OPAQUE,
                    eSTREAMTYPE_IMAGE_INOUT,
                    6, 1, // MAX, MIN
                    usage, format, size, stride);

            if (mpImageInfo_fullRaw == NULL)
            {
                mfllLogE("create ImageStreamInfo FullRaw failed");
                return MFALSE;
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
                mfllLogE("wrong parameter for RRZO");
                return BAD_VALUE;
            }

            mpImageInfo_resizedRaw = createRawImageStreamInfo(
                    "MfllShot:ResizedRaw",
                    eSTREAMID_IMAGE_PIPE_RAW_RESIZER,
                    eSTREAMTYPE_IMAGE_INOUT,
                    6, 0, // MfllShot don't need resized raw, therefore init it without buffer allocation
                    usage, format, size, stride);

            if (mpImageInfo_resizedRaw == NULL)
            {
                mfllLogE("create ImageStreamInfo ResizedRaw failed");
                return BAD_VALUE;
            }

            dumpImageStreamInfo(mpImageInfo_resizedRaw);
        }
    }
    else
    {
        mfllAutoLog("ZSD mode");

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
                "MfllShot:Postview",
                eSTREAMID_IMAGE_PIPE_YUV_00,
                eSTREAMTYPE_IMAGE_INOUT,
                1, 1,
                usage, format, size, transform);

        if (mpImageInfo_yuvPostview == NULL)
        {
            mfllLogE("create ImageStreamInfo Postview failed");
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
                "MfllShot:YuvJpeg",
                eSTREAMID_IMAGE_PIPE_YUV_JPEG,
                eSTREAMTYPE_IMAGE_INOUT,
                1, 1,
                usage, format, size, transform);

        if (mpImageInfo_yuvJpeg == NULL)
        {
            mfllLogE("create ImageStreamInfo YuvJpeg failed");
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
                "MfllShot:YuvThumbnail",
                eSTREAMID_IMAGE_PIPE_YUV_THUMBNAIL,
                eSTREAMTYPE_IMAGE_INOUT,
                1, 1,
                usage, format, size, transform);

        if (mpImageInfo_yuvThumbnail == NULL)
        {
            mfllLogE("create ImageStreamInfo YuvThumbnail failed");
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
                "MfllShot:Jpeg",
                eSTREAMID_IMAGE_MFLL,
                eSTREAMTYPE_IMAGE_INOUT,
                1, 1,
                usage, format, size, transform);

        if (mpImageInfo_jpeg == NULL)
        {
            mfllLogE("create ImageStreamInfo Jpeg failed");
            return BAD_VALUE;
        }

        dumpImageStreamInfo(mpImageInfo_jpeg);
    }

    return OK;
}

MERROR MfllShot::createPipeline(const sp<StreamBufferProvider>& provider)
{
    mfllAutoLogFunc();
    mfllAutoTraceFunc();

    LegacyPipelineBuilder::ConfigParams LPBConfigParams;
    LPBConfigParams.mode = LegacyPipelineMode_T::PipelineMode_Mfll;
    LPBConfigParams.enableEIS = MTRUE;
    LPBConfigParams.enableLCS = MFALSE;

    sp<LegacyPipelineBuilder> builder =
        LegacyPipelineBuilder::createInstance(
                mSensorParam.u4OpenID,
                "MfllShot",
                LPBConfigParams);

    if (builder == NULL) {
        mfllLogE("create LegacyPipelineBuilder failed");
        return BAD_VALUE;
    }

    mpCallbackHandler = new BufferCallbackHandler(mSensorParam.u4OpenID);

    // register image listener
    mpCallbackHandler->setImageCallback(this);

    sp<StreamBufferProviderFactory> pFactory =
        StreamBufferProviderFactory::createInstance();

    Vector<PipelineImageParam> vImageParam;

    if (provider == nullptr)
    {
        mfllAutoLog("normal mode");

        // normal mode
        //
        {
            sp<IScenarioControl> scenarioCtrl =
                IScenarioControl::create(mSensorParam.u4OpenID);
            if (scenarioCtrl == NULL)
            {
                mfllLogE("get scenario control failed");
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
            pFactory->setImageStreamInfo(pStreamInfo);
            pFactory->setUsersPool(cbBufferPool);

            PipelineImageParam imgParam =
            {
                .pInfo     = pStreamInfo,
                .pProvider = pFactory->create(),
                .usage     = 0
            };
            vImageParam.push_back(imgParam);

            mfllLogD("add full raw");
        }

        // resized raw
        if (mpImageInfo_resizedRaw.get())
        {
            sp<IImageStreamInfo>& pStreamInfo(mpImageInfo_resizedRaw);

            PipelineImageParam imgParam =
            {
                .pInfo     = pStreamInfo,
                .pProvider = nullptr,
                .usage     = 0
            };
            vImageParam.push_back(imgParam);

            mfllLogD("add resized raw");
        }

        PipelineSensorParam sensorParam =
        {
            .mode      = mSensorParam.u4Scenario,
            .rawType   = mSensorParam.u4RawType,
            .size      = mSensorSetting.size,
            .fps       = (MUINT32)mSensorSetting.fps,
            .pixelMode = mSensorSetting.pixelMode,
//            .vhdrMode  = SENSOR_VHDR_MODE_NONE
        };

        if (OK != builder->setSrc(sensorParam))
        {
            mfllLogE("normal mode - setSrc failed");
            return UNKNOWN_ERROR;
        }
    }
    else
    {
        mfllAutoLog("ZSD mode");

        // ZSD mode
        //
        sp<IImageStreamInfo>& pStreamInfo(mpImageInfo_fullRaw);

        mpP1Pool = prepareCallbackBufferPool(mpCallbackHandler, pStreamInfo, false);
        pFactory->setImageStreamInfo(pStreamInfo);
        pFactory->setUsersPool(mpP1Pool);

        PipelineImageParam imageParam =
        {
            .pInfo     = pStreamInfo,
            .pProvider = pFactory->create(),
            .usage     = 0
        };

        if (OK != builder->setSrc(imageParam))
        {
            mfllLogE("ZSD mode - setSrc failed");
            return UNKNOWN_ERROR;
        }
    }

    // jpeg yuv
    if (mpImageInfo_yuvJpeg.get())
    {
        sp<IImageStreamInfo>& pStreamInfo(mpImageInfo_yuvJpeg);

        sp<CallbackBufferPool> cbBufferPool =
            prepareCallbackBufferPool(mpCallbackHandler, pStreamInfo, true);
        pFactory->setImageStreamInfo(pStreamInfo);
        pFactory->setUsersPool(cbBufferPool);

        PipelineImageParam imageParam =
        {
            .pInfo     = pStreamInfo,
            .pProvider = pFactory->create(),
            .usage     = 0
        };
        vImageParam.push_back(imageParam);

        mfllLogD("add jpeg");
    }

    // postview
    if (mpImageInfo_yuvPostview.get())
    {
        sp<IImageStreamInfo>& pStreamInfo(mpImageInfo_yuvPostview);

        sp<CallbackBufferPool> cbBufferPool =
            prepareCallbackBufferPool(mpCallbackHandler, pStreamInfo, true);
        pFactory->setImageStreamInfo(pStreamInfo);
        pFactory->setUsersPool(cbBufferPool);

        PipelineImageParam imageParam =
        {
            .pInfo     = pStreamInfo,
            .pProvider = pFactory->create(),
            .usage     = 0
        };
        vImageParam.push_back(imageParam);

        mfllLogD("add postview");
    }

    // thumbnail yuv
    if (mpImageInfo_yuvThumbnail.get())
    {
        sp<IImageStreamInfo>& pStreamInfo(mpImageInfo_yuvThumbnail);

        PipelineImageParam imageParam =
        {
            .pInfo     = pStreamInfo,
            .pProvider = nullptr,
            .usage     = 0
        };
        vImageParam.push_back(imageParam);

        mfllLogD("add thumbnail yuv");
    }

    // jpeg
    if (mpImageInfo_jpeg.get())
    {
        sp<IImageStreamInfo>& pStreamInfo(mpImageInfo_jpeg);

        sp<CallbackBufferPool> cbBufferPool =
            prepareCallbackBufferPool(mpCallbackHandler, pStreamInfo, true);
        pFactory->setImageStreamInfo(pStreamInfo);
        pFactory->setUsersPool(cbBufferPool);

        PipelineImageParam imageParam =
        {
            .pInfo     = pStreamInfo,
            .pProvider = pFactory->create(),
            .usage     = 0
        };
        vImageParam.push_back(imageParam);

        mfllLogD("add jpeg yuv");
    }

    mfllLogD("image param count(%zu)", vImageParam.size());

    if (OK != builder->setDst(vImageParam))
    {
        mfllLogE("setDst failed");
        return UNKNOWN_ERROR;
    }

    mpPipeline = builder->create();
    if (mpPipeline == NULL) {
        mfllLogE("pBuffer->create returns NULL");
        return BAD_VALUE;
    }

    {
        sp<ResultProcessor> pResultProcessor =
            mpPipeline->getResultProcessor().promote();
        if (pResultProcessor == NULL) {
            mfllLogE("getResultProcessor failed");
            return BAD_VALUE;
        }

        // register metadata listener
        pResultProcessor->registerListener(0, 0, true, this);
    }

    return OK;
}

MERROR MfllShot::releaseResource()
{
    mfllAutoLogFunc();
    mfllAutoTraceFunc();

    mpPipeline.clear();

    mpCallbackHandler.clear();

    if (eShotMode_ZsdMfllShot == mu4ShotMode)
    {
        if (mOriginalSelector.get())
            mpBufferProvider->setSelector(mOriginalSelector);

        mpP1Pool.clear();
    }

    return OK;
}

