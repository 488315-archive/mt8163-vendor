/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
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

/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
#define LOG_TAG "MtkCam/SShot"
//
#include <Log.h>
#if defined(__func__)
#undef __func__
#endif
#define __func__ __FUNCTION__

#define MY_LOGV(fmt, arg...)        CAM_LOGV("[%s] " fmt, __func__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_LOGD("[%s] " fmt, __func__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_LOGI("[%s] " fmt, __func__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_LOGW("[%s] " fmt, __func__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_LOGE("[%s] " fmt, __func__, ##arg)
#define MY_LOGA(fmt, arg...)        CAM_LOGA("[%s] " fmt, __func__, ##arg)
#define MY_LOGF(fmt, arg...)        CAM_LOGF("[%s] " fmt, __func__, ##arg)
#define FUNC_START                  MY_LOGD("+")
#define FUNC_END                    MY_LOGD("-")
//
//
#include <common.h>
using namespace android;
//
#include <camshot/_callbacks.h>
#include <camshot/_params.h>
//
#include <metadata/IMetadata.h>
using namespace NSCam;

#include <LegacyPipeline/ILegacyPipeline.h>
#include <LegacyPipeline/StreamId.h>
#include <LegacyPipeline/buffer/StreamBufferProviderFactory.h>
#include <LegacyPipeline/LegacyPipelineBuilder.h>
using namespace NSCam::v1::NSLegacyPipeline;

#include "../inc/CamShotImp.h"
#include "../inc/SingleShot.h"
#include "../inc/CamShotUtils.h"
//
#include <IHalSensor.h>
#include <imageio/ispio_utility.h>
//
#include <mtk_platform_metadata_tag.h>

#include <metadata/IMetadata.h>
#include <metadata/IMetadataProvider.h>
#include <metadata/IMetadataConverter.h>

#include <hwutils/HwInfoHelper.h>
using namespace NSCamHW;
//
#include <camshot/BufferCallbackHandler.h>
using namespace NSCam::v1;

#include <cutils/properties.h>
#define DUMP_KEY  "debug.singleshot.dump"
#define DUMP_PATH "/sdcard/singleshot"

#define CHECK_OBJECT(x)  do{                                        \
    if (x == nullptr) { MY_LOGE("Null %s Object", #x); return MFALSE;} \
} while(0)

/*******************************************************************************
*
********************************************************************************/
namespace NSCamShot {

/*******************************************************************************
*
********************************************************************************/
class ImageCallback
    : public IImageCallback
{
public:
                                        ImageCallback(
                                            CallbackListener* pListener,
                                            MUINT32 const data
                                            )
                                            : mpListener(pListener)
                                            , mData(data)
                                        {}
public:   ////    interface of IImageCallback
    /**
     *
     * Received result buffer.
     *
     * @param[in] RequestNo : request number.
     *
     * @param[in] pBuffer : IImageBuffer.
     *
     */
    virtual MERROR                      onResultReceived(
                                            MUINT32 const              RequestNo,
                                            StreamId_T const           streamId,
                                            MBOOL   const           errorBuffer,
                                            android::sp<IImageBuffer>& pBuffer
                                        ) {
                                            if( mpListener )
                                                mpListener->onDataReceived(
                                                    RequestNo, streamId, pBuffer
                                                    );
                                            return OK;
                                        }
protected:
    CallbackListener*                   mpListener;
    MUINT32 const                       mData;
};


/*******************************************************************************
*
********************************************************************************/
class MetadataListener
    : public ResultProcessor::IListener
{
public:
                                        MetadataListener(
                                            CallbackListener* pListener
                                            )
                                            : mpListener(pListener)
                                        {}
public:   ////    interface of IListener
    virtual void                        onResultReceived(
                                            MUINT32         const requestNo,
                                            StreamId_T      const streamId,
                                            MBOOL           const errorResult,
                                            IMetadata       const result
                                        ) {
                                            if( mpListener )
                                                mpListener->onMetaReceived(
                                                    requestNo, streamId, errorResult, result
                                                    );
                                        }

    virtual void                       onFrameEnd(
                                            MUINT32         const requestNo
                                        ) {};

    virtual String8                     getUserName() { return String8(LOG_TAG); }

protected:
    CallbackListener*                   mpListener;
};


/*******************************************************************************
*
********************************************************************************/
class ResourceHolder
    : public virtual android::RefBase
{
public:
    MVOID                       add( sp<RefBase> pResource ) {
                                    mvResources.push_back(pResource);
                                }
    MVOID                       clear() {
                                    mvResources.clear();
                                }
protected:
    Vector< sp<RefBase> >       mvResources;
};


/*******************************************************************************
*
********************************************************************************/
SingleShot::
SingleShot(
    EShotMode const eShotMode,
    char const*const szCamShotName,
    EPipelineMode const ePipelineMode
)
    : CamShotImp(eShotMode, szCamShotName, ePipelineMode)
    , mpResource( new ResourceHolder() )
    , mbCBShutter(MTRUE)
{
    char value[PROPERTY_VALUE_MAX];
    ::property_get(DUMP_KEY, value, "0");
    mDumpFlag = atoi(value);
    if( mDumpFlag ) {
        MY_LOGD("enable dump flag 0x%x", mDumpFlag);
        mkdir(DUMP_PATH, S_IRWXU | S_IRWXG | S_IRWXO);
    }
}


/*******************************************************************************
*
********************************************************************************/
SingleShot::
~SingleShot()
{
    if( mpPipeline.get() )
        mpPipeline->flush();
}


/*******************************************************************************
*
********************************************************************************/
MBOOL
SingleShot::
init()
{
    FUNC_START;
    MBOOL ret = MTRUE;
    FUNC_END;
    //
    return ret;
}


/*******************************************************************************
*
********************************************************************************/
MBOOL
SingleShot::
uninit()
{
    MBOOL ret = MTRUE;
    FUNC_START;

    FUNC_END;
    return ret;
}


/*******************************************************************************
*
********************************************************************************/
MBOOL
SingleShot::
start(SensorParam const & rSensorParam)
{
    CAM_TRACE_CALL();
    FUNC_START;
    mSensorParam = rSensorParam;
    //
    dumpSensorParam(mSensorParam);

    FUNC_END;
    //
    return MTRUE;
}


/*******************************************************************************
*
********************************************************************************/
MBOOL
SingleShot::
startOne(SensorParam const & rSensorParam)
{
    CAM_TRACE_CALL();
    FUNC_START;
    mSensorParam = rSensorParam;
    dumpSensorParam(mSensorParam);
    //
    if ( ! isDataMsgEnabled(ECamShot_DATA_MSG_ALL) && ! isNotifyMsgEnabled(ECamShot_DATA_MSG_ALL) )
    {
        MY_LOGE("no data/msg enabled");
        return MFALSE;
    }
    //
    if ( ! createStreams() ) {
        MY_LOGE("createStreams failed");
        return MFALSE;
    }
    //
    if ( ! createPipeline() ) {
        MY_LOGE("createPipeline failed");
        return MFALSE;
    }
    //
    IMetadata appSetting = mShotParam.appSetting;
    IMetadata halSetting = mShotParam.halSetting;
    //
    {
        IMetadata::IEntry entry(MTK_HAL_REQUEST_SENSOR_SIZE);
        entry.push_back(mSensorSize, Type2Type< MSize >());
        halSetting.update(entry.tag(), entry);
    }
    //
    if( OK != mpPipeline->submitSetting(
                0,
                appSetting,
                halSetting
                )
      )
    {
        MY_LOGE("submitRequest failed");
        return MFALSE;
    }
    //
    mpPipeline->waitUntilDrained();
    //
    mpPipeline = nullptr;
    mpResource.clear();
    //
    FUNC_END;
    //
    return MTRUE;
}

/*******************************************************************************
*
********************************************************************************/
MBOOL
SingleShot::
startOne(SensorParam const & rSensorParam, StreamBufferProvider* pProvider)
{
    CAM_TRACE_CALL();
    FUNC_START;
    mSensorParam = rSensorParam;
    dumpSensorParam(mSensorParam);
    //
    sp< ISelector > pSelector = pProvider->querySelector();
    if(pSelector == NULL)
    {
        MY_LOGE("can't find Selector in Consumer");
        return MFALSE;
    }
    //
    android::sp<IImageBuffer> pBuf = NULL;
    status_t status = OK;
    status = getSelectorData(pSelector.get(), mShotParam, pBuf);
    if(status != OK)
    {
        MY_LOGE("GetSelectorData Fail!");
        return MFALSE;
    }
    MY_LOGD("GetSelectorData IImageBuffer(%p) Heap(%p)",pBuf.get(),pBuf->getImageBufferHeap());
    //
    if(pBuf == NULL)
    {
        MY_LOGE("Get selector buffer is NULL!");
        return MFALSE;
    }
    //
    if(isDataMsgEnabled(ECamShot_DATA_MSG_DNG))
        handleDataCallback(ECamShot_DATA_MSG_DNG, 0, 0, pBuf.get());
    //
    if ( ! isDataMsgEnabled(ECamShot_DATA_MSG_ALL) && ! isNotifyMsgEnabled(ECamShot_DATA_MSG_ALL) )
    {
        MY_LOGE("no data/msg enabled");
        return MFALSE;
    }
    //
    if ( ! createStreams(pProvider) ) {
        MY_LOGE("createStreams failed");
        return MFALSE;
    }
    //
    if ( ! createPipeline(pBuf) ) {
        MY_LOGE("createPipeline failed");
        return MFALSE;
    }
    //
    IMetadata appSetting = mShotParam.appSetting;
    IMetadata halSetting = mShotParam.halSetting;
    //
    {
        IMetadata::IEntry entry(MTK_HAL_REQUEST_SENSOR_SIZE);
        entry.push_back(mSensorSize, Type2Type< MSize >());
        halSetting.update(entry.tag(), entry);
    }
    //
    ILegacyPipeline::ResultSet resultSet;
    resultSet.add(eSTREAMID_META_APP_DYNAMIC_P1, mSelectorAppMetadata);
    //
    if( OK != mpPipeline->submitSetting(
                0,
                appSetting,
                halSetting,
                &resultSet
                )
      )
    {
        MY_LOGE("submitRequest failed");
        return MFALSE;
    }
    //
    mpPipeline->waitUntilDrained();
    //
    mpPipeline = nullptr;
    mpResource.clear();
    //
    MY_LOGD("Ready to returnBuffer(%p)",pBuf->getImageBufferHeap());
    pSelector->returnBuffer(pBuf->getImageBufferHeap());
    //
    FUNC_END;
    //
    return MTRUE;
}


/*******************************************************************************
*
********************************************************************************/
MBOOL
SingleShot::
startAsync(SensorParam const & rSensorParam)
{
    FUNC_START;
    //
    MY_LOGE("not supported yet");
    //
    FUNC_END;
    //
    return MFALSE;
}

/*******************************************************************************
*
********************************************************************************/
MBOOL
SingleShot::
stop()
{
    CAM_TRACE_CALL();
    FUNC_START;

    if( mpPipeline.get() )
        mpPipeline->flush();

    FUNC_END;
    //
    return MTRUE;
}



/*******************************************************************************
*
********************************************************************************/
MBOOL
SingleShot::
setShotParam(ShotParam const & rParam)
{
    FUNC_START;
    mShotParam = rParam;
    //
    dumpShotParam(mShotParam);

    FUNC_END;
    //
    return MTRUE;
}


/*******************************************************************************
*
********************************************************************************/
MBOOL
SingleShot::
setJpegParam(JpegParam const & rParam)
{
    FUNC_START;
    mJpegParam = rParam;
    //
    dumpJpegParam(mJpegParam);

    FUNC_END;
    //
    return MTRUE;
}


/*******************************************************************************
*
********************************************************************************/
MBOOL
SingleShot::
registerImageBuffer(ECamShotImgBufType const eBufType, IImageBuffer const *pImgBuffer)
{
    MY_LOGD("register bufType(%d) pImgBuffer(%p)", eBufType, pImgBuffer);
    if(pImgBuffer==NULL)
    {
        MY_LOGE("register NULL buffer!");
        return MFALSE;
    }
    IImageBuffer* pBuf = const_cast<IImageBuffer*>(pImgBuffer);
    if(mvRegBuf.indexOfKey(eBufType) >= 0)
    {

        mvRegBuf.replaceValueFor(eBufType, pBuf);
    }
    else
        mvRegBuf.add(eBufType, pBuf);
    //
    return MTRUE;
}


/*******************************************************************************
*
********************************************************************************/
MBOOL
SingleShot::
sendCommand(MINT32 cmd, MINT32 arg1, MINT32 arg2, MINT32 arg3, MVOID* arg4)
{
    //FUNC_START;
    switch( cmd )
    {
#if 0
        case ECamShot_CMD_SET_CAPTURE_STYLE:
            muCapStyle = arg1;
            MY_LOGD("capture style 0x%x", muCapStyle);
            break;
        case ECamShot_CMD_SET_NRTYPE:
            muNRType = arg1;
            MY_LOGD("NR type 0x%x", muNRType);
            break;
        case ECamShot_CMD_SET_VHDR_PROFILE:
            mbVHdr = MTRUE; //enable vhdr mode
            mVhdrProfile = (EIspProfile_T)arg1;
            MY_LOGD("set vhdr profile 0x%x", arg1);
            break;
        case ECamShot_CMD_SET_RWB_PROC_TYPE:
            mbEnGPU = arg1; //enable GPU process
            MY_LOGD("GPU process 0x%x", mbEnGPU);
            break;
#endif
        default:
            MY_LOGE("not supported cmd 0x%x", cmd);
            return MFALSE;
    }
    //FUNC_END;
    //
    return MTRUE;
}

/*******************************************************************************
*
********************************************************************************/
MBOOL
SingleShot::
getResultMetadata(IMetadata &rAppResultMetadata, IMetadata &rHalResultMetadata)
{
    CAM_TRACE_CALL();
    Mutex::Autolock _l(mLock);
    if (!mAppDone || !mHalDone)
    {
      MY_LOGD("Wait Result of Metadata (APP: %d,HAL: %d)", mAppDone, mHalDone);
      mCond.wait(mLock);
    }

    rAppResultMetadata = mAppResultMetadata;
    rHalResultMetadata = mHalResultMetadata;
    return MTRUE;
}

/*******************************************************************************
*
********************************************************************************/
MUINT32
SingleShot::
getRotation() const
{
    return mShotParam.u4PictureTransform;
}


/*******************************************************************************
*
********************************************************************************/
MERROR
SingleShot::
createStreams(StreamBufferProvider* pProvider)
{
    CAM_TRACE_CALL();
    MUINT32 const openId     = mSensorParam.u4OpenID;
    MUINT32 const sensorMode = mSensorParam.u4Scenario;
    MUINT32 const bitDepth   = mSensorParam.u4Bitdepth;
    //
    MSize const previewsize   = MSize(mShotParam.u4PostViewWidth, mShotParam.u4PostViewHeight);
    MINT const previewfmt     = mShotParam.ePostViewFmt;
    MINT const yuvfmt         = mShotParam.ePictureFmt;
    MSize const jpegsize      = (getRotation() & eTransform_ROT_90) ?
        MSize(mShotParam.u4PictureHeight, mShotParam.u4PictureWidth):
        MSize(mShotParam.u4PictureWidth, mShotParam.u4PictureHeight);
    MSize const thumbnailsize = MSize(mJpegParam.u4ThumbWidth, mJpegParam.u4ThumbHeight);
    //
    if( pProvider == nullptr )
    {
        HwInfoHelper helper(openId);
        if( ! helper.updateInfos() ) {
            MY_LOGE("cannot properly update infos");
            return MFALSE;
        }
        //
        if( ! helper.getSensorSize( sensorMode, mSensorSize) ||
                ! helper.getSensorFps( sensorMode, mSensorFps) ||
                ! helper.queryPixelMode( sensorMode, mSensorFps, mPixelMode)
          ) {
            MY_LOGE("cannot get params about sensor");
            return MFALSE;
        }
        //
        // Fullsize Raw
        {
            MSize size = mSensorSize;
            MINT format;
            size_t stride;
            MUINT const usage = 0; //not necessary here
            if( ! helper.getImgoFmt(bitDepth, format) ||
                    ! helper.alignPass1HwLimitation(mPixelMode, format, true, size, stride) )
            {
                MY_LOGE("wrong params about imgo");
                return MFALSE;
            }
            //
            sp<IImageStreamInfo>
                pStreamInfo = createRawImageStreamInfo(
                        "SingleShot:Fullraw",
                        eSTREAMID_IMAGE_PIPE_RAW_OPAQUE,
                        eSTREAMTYPE_IMAGE_INOUT,
                        4, 4,
                        usage, format, size, stride
                        );
            if( pStreamInfo == nullptr ) {
                return MFALSE;
            }
            mpInfo_FullRaw = pStreamInfo;
        }
        //
        // Resize Raw
        if( helper.isRaw() && !! previewsize )
        {
            MSize size;
            MINT format;
            size_t stride;
            MUINT const usage = 0; //not necessary here
            if( ! helper.getRrzoFmt(bitDepth, format) ||
                    ! helper.alignRrzoHwLimitation(previewsize, mSensorSize, size) ||
                    ! helper.alignPass1HwLimitation(mPixelMode, format, false, size, stride) )
            {
                MY_LOGE("wrong params about imgo");
                return MFALSE;
            }
            //
            sp<IImageStreamInfo>
                pStreamInfo = createRawImageStreamInfo(
                        "SingleShot:Resizedraw",
                        eSTREAMID_IMAGE_PIPE_RAW_RESIZER,
                        eSTREAMTYPE_IMAGE_INOUT,
                        4, 4,
                        usage, format, size, stride
                        );
            if( pStreamInfo == nullptr ) {
                return MFALSE;
            }
            mpInfo_ResizedRaw = pStreamInfo;
        }
    }
    else
    {
        // zsd flow: query IImageStreamInfo of raw stream from provider
        mpInfo_FullRaw = pProvider->queryImageStreamInfo();
        mpInfo_ResizedRaw = nullptr;
        //
        HwInfoHelper helper(openId);
        if( ! helper.updateInfos() ) {
            MY_LOGE("cannot properly update infos");
            return MFALSE;
        }
        if( ! helper.getSensorSize( sensorMode, mSensorSize) ||
                ! helper.getSensorFps( sensorMode, mSensorFps) ||
                ! helper.queryPixelMode( sensorMode, mSensorFps, mPixelMode)
          ) {
            MY_LOGE("cannot get params about sensor");
            return MFALSE;
        }
        //
        MY_LOGD("zsd raw stream %#" PRIxPTR, "(%s) size(%dx%d), fmt 0x%x",
                mpInfo_FullRaw->getStreamId(),
                mpInfo_FullRaw->getStreamName(),
                mpInfo_FullRaw->getImgSize().w,
                mpInfo_FullRaw->getImgSize().h,
                mpInfo_FullRaw->getImgFormat()
               );
        //
    }
    //
    if( isDataMsgEnabled(ECamShot_DATA_MSG_POSTVIEW) )
    {
        MSize size        = previewsize;
        MINT format       = previewfmt;
        MUINT const usage = 0; //not necessary here
        MUINT32 transform = 0;
        sp<IImageStreamInfo>
            pStreamInfo = createImageStreamInfo(
                    "SingleShot:Postview",
                    eSTREAMID_IMAGE_PIPE_YUV_00,
                    eSTREAMTYPE_IMAGE_INOUT,
                    1, 1,
                    usage, format, size, transform
                    );
        if( pStreamInfo == nullptr ) {
            return BAD_VALUE;
        }
        //
        mpInfo_YuvPostview = pStreamInfo;
    }
    //
    // Yuv
    if( isDataMsgEnabled(ECamShot_DATA_MSG_YUV|ECamShot_DATA_MSG_JPEG) )
    {
        MSize size        = jpegsize;
        MINT format       = yuvfmt;
        MUINT const usage = 0; //not necessary here
        MUINT32 transform = getRotation();
        sp<IImageStreamInfo>
            pStreamInfo = createImageStreamInfo(
                    "SingleShot:Yuv",
                    eSTREAMID_IMAGE_PIPE_YUV_JPEG,
                    eSTREAMTYPE_IMAGE_INOUT,
                    1, 1,
                    usage, format, size, transform
                    );
        if( pStreamInfo == nullptr ) {
            return BAD_VALUE;
        }
        //
        mpInfo_Yuv = pStreamInfo;
    }
    //
    // Thumbnail Yuv
    if( isDataMsgEnabled(ECamShot_DATA_MSG_JPEG) && thumbnailsize != MSize(0,0))
    {
        MSize size        = thumbnailsize;
        MINT format       = yuvfmt;
        MUINT const usage = 0; //not necessary here
        MUINT32 transform = 0;
        sp<IImageStreamInfo>
            pStreamInfo = createImageStreamInfo(
                    "SingleShot:ThumbnailYuv",
                    eSTREAMID_IMAGE_PIPE_YUV_THUMBNAIL,
                    eSTREAMTYPE_IMAGE_INOUT,
                    1, 1,
                    usage, format, size, transform
                    );
        if( pStreamInfo == nullptr ) {
            return BAD_VALUE;
        }
        //
        mpInfo_YuvThumbnail = pStreamInfo;
    }
    //
    // Jpeg
    if( isDataMsgEnabled(ECamShot_DATA_MSG_JPEG) )
    {
        MSize size        = jpegsize;
        MINT format       = eImgFmt_BLOB;
        MUINT const usage = 0; //not necessary here
        MUINT32 transform = 0;
        sp<IImageStreamInfo>
            pStreamInfo = createImageStreamInfo(
                    "SingleShot:Jpeg",
                    eSTREAMID_IMAGE_JPEG,
                    eSTREAMTYPE_IMAGE_INOUT,
                    1, 1,
                    usage, format, size, transform
                    );
        if( pStreamInfo == nullptr ) {
            return BAD_VALUE;
        }
        //
        mpInfo_Jpeg = pStreamInfo;
    }
    //
    return MTRUE;
}


/*******************************************************************************
*
********************************************************************************/
MERROR
SingleShot::
createPipeline(android::sp<IImageBuffer> pBuf)

{
    CAM_TRACE_CALL();
    FUNC_START;
    LegacyPipelineBuilder::ConfigParams LPBConfigParams;
    LPBConfigParams.mode = getLegacyPipelineMode();
    LPBConfigParams.enableEIS = MFALSE;
    LPBConfigParams.enableLCS = MFALSE;

    sp<LegacyPipelineBuilder> pBuilder = LegacyPipelineBuilder::createInstance(
                                            mSensorParam.u4OpenID,
                                            "SingleShot",
                                            LPBConfigParams);

    CHECK_OBJECT(pBuilder);

    sp<ImageCallback> pCallback = new ImageCallback(this, 0);
    mpResource->add(pCallback);

    sp<BufferCallbackHandler> pCallbackHandler = new BufferCallbackHandler(mSensorParam.u4OpenID);
    pCallbackHandler->setImageCallback(pCallback);
    mpResource->add(pCallbackHandler);

    sp<StreamBufferProviderFactory> pFactory =
                StreamBufferProviderFactory::createInstance();
    //
    Vector<PipelineImageParam> vImageParam;
    //
    if( pBuf == nullptr )
    {
        MY_LOGD("createPipeline for Normal");
        // normal flow
        PipelineSensorParam sensorParam = {
            .mode      = mSensorParam.u4Scenario,
            .rawType   = mSensorParam.u4RawType,
            .size      = mSensorSize,
            .fps       = (MUINT32)mSensorFps,
            .pixelMode = mPixelMode
        };
        //
        if( OK != pBuilder->setSrc(sensorParam) ) {
            MY_LOGE("setSrc failed");
            return MFALSE;
        }
        //
        sp<IScenarioControl> pScenarioCtrl = IScenarioControl::create(mSensorParam.u4OpenID);
        if( pScenarioCtrl.get() == NULL )
        {
            MY_LOGE("get Scenario Control fail");
            return MFALSE;
        }
        IScenarioControl::ControlParam param;
        param.scenario = IScenarioControl::Scenario_Capture;
        param.sensorSize = mSensorSize;
        param.sensorFps  = mSensorFps;
        pScenarioCtrl->enterScenario(param);
        pBuilder->setScenarioControl(pScenarioCtrl);
        //
        // fullraw
        if( mpInfo_FullRaw.get() )
        {
            sp<IImageStreamInfo> pStreamInfo = mpInfo_FullRaw;
            //
            //pCallbackHandler->setImageStreamInfo(ECamShot_DATA_MSG_RAW, pStreamInfo,
            //    (mvRegBuf.indexOfKey(ECamShot_BUF_TYPE_RAW)>=0 ? mvRegBuf.editValueFor(ECamShot_BUF_TYPE_RAW) : NULL ));
            //
            sp<CallbackBufferPool> pPool = new CallbackBufferPool(pStreamInfo);
            if(mvRegBuf.indexOfKey(ECamShot_BUF_TYPE_RAW)>=0)
            {
                pPool->addBuffer(mvRegBuf.editValueFor(ECamShot_BUF_TYPE_RAW));
            }
            else
            {
                pPool->allocateBuffer(
                                  pStreamInfo->getStreamName(),
                                  pStreamInfo->getMaxBufNum(),
                                  pStreamInfo->getMinInitBufNum()
                                  );
            }
            pCallbackHandler->setBufferPool(pPool);
            //
            pFactory->setImageStreamInfo(pStreamInfo);
            pFactory->setUsersPool(
                        pCallbackHandler->queryBufferPool(pStreamInfo->getStreamId())
                    );
            //
            PipelineImageParam imgParam = {
                .pInfo     = pStreamInfo,
                .pProvider = pFactory->create(),
                .usage     = 0
            };
            vImageParam.push_back(imgParam);
        }
        // resized raw
        if( mpInfo_ResizedRaw.get() )
        {
            sp<IImageStreamInfo> pStreamInfo = mpInfo_ResizedRaw;
            //
            PipelineImageParam imgParam = {
                .pInfo     = pStreamInfo,
                .pProvider = nullptr,
                .usage     = 0
            };
            vImageParam.push_back(imgParam);
        }
    }
    else
    {
        MY_LOGD("createPipeline for ZSD");
        // zsd flow
        sp<IImageStreamInfo> pStreamInfo = mpInfo_FullRaw;
        //
        //pCallbackHandler->setImageStreamInfo(ECamShot_DATA_MSG_RAW, pStreamInfo,pBuf);
        sp<CallbackBufferPool> pPool = new CallbackBufferPool(pStreamInfo);
        pPool->addBuffer(pBuf);
        pCallbackHandler->setBufferPool(pPool);
        //
        pFactory->setImageStreamInfo(pStreamInfo);
        pFactory->setUsersPool(
                    pCallbackHandler->queryBufferPool(pStreamInfo->getStreamId())
                );
        PipelineImageParam imgParam = {
            .pInfo     = pStreamInfo,
            .pProvider = pFactory->create(),
            .usage     = 0
        };
        //
        if( OK != pBuilder->setSrc(imgParam) ) {
            MY_LOGE("setSrc failed");
            return MFALSE;
        //
        }
    }
    //
    {
        if( mpInfo_Yuv.get() )
        {
            sp<IImageStreamInfo> pStreamInfo = mpInfo_Yuv;
            //
            //pCallbackHandler->setImageStreamInfo(ECamShot_DATA_MSG_YUV, pStreamInfo,
            //    (mvRegBuf.indexOfKey(ECamShot_BUF_TYPE_YUV)>=0 ? mvRegBuf.editValueFor(ECamShot_BUF_TYPE_YUV) : NULL ));
            sp<CallbackBufferPool> pPool = new CallbackBufferPool(pStreamInfo);
            if(mvRegBuf.indexOfKey(ECamShot_BUF_TYPE_YUV)>=0)
            {
                pPool->addBuffer(mvRegBuf.editValueFor(ECamShot_BUF_TYPE_YUV));
            }
            else
            {
                pPool->allocateBuffer(
                                  pStreamInfo->getStreamName(),
                                  pStreamInfo->getMaxBufNum(),
                                  pStreamInfo->getMinInitBufNum()
                                  );
            }
            pCallbackHandler->setBufferPool(pPool);
            //
            pFactory->setImageStreamInfo(pStreamInfo);
            pFactory->setUsersPool(
                        pCallbackHandler->queryBufferPool(pStreamInfo->getStreamId())
                    );
            //
            PipelineImageParam imgParam = {
                .pInfo     = pStreamInfo,
                .pProvider = pFactory->create(),
                .usage     = 0
            };
            vImageParam.push_back(imgParam);
        }
        //
        if( mpInfo_YuvPostview.get() )
        {
            sp<IImageStreamInfo> pStreamInfo = mpInfo_YuvPostview;
            //
            //pCallbackHandler->setImageStreamInfo(ECamShot_DATA_MSG_POSTVIEW, pStreamInfo,
            //    (mvRegBuf.indexOfKey(ECamShot_BUF_TYPE_POSTVIEW)>=0 ? mvRegBuf.editValueFor(ECamShot_BUF_TYPE_POSTVIEW) : NULL ));
            sp<CallbackBufferPool> pPool = new CallbackBufferPool(pStreamInfo);
            if(mvRegBuf.indexOfKey(ECamShot_BUF_TYPE_POSTVIEW)>=0)
            {
                pPool->addBuffer(mvRegBuf.editValueFor(ECamShot_BUF_TYPE_POSTVIEW));
            }
            else
            {
                pPool->allocateBuffer(
                                  pStreamInfo->getStreamName(),
                                  pStreamInfo->getMaxBufNum(),
                                  pStreamInfo->getMinInitBufNum()
                                  );
            }
            pCallbackHandler->setBufferPool(pPool);
            //
            pFactory->setImageStreamInfo(pStreamInfo);
            pFactory->setUsersPool(
                        pCallbackHandler->queryBufferPool(pStreamInfo->getStreamId())
                    );
            //
            PipelineImageParam imgParam = {
                .pInfo     = pStreamInfo,
                .pProvider = pFactory->create(),
                .usage     = 0
            };
            vImageParam.push_back(imgParam);
        }
        //
        if( mpInfo_YuvThumbnail.get() )
        {
            sp<IImageStreamInfo> pStreamInfo = mpInfo_YuvThumbnail;
            //
            PipelineImageParam imgParam = {
                .pInfo     = pStreamInfo,
                .pProvider = nullptr,
                .usage     = 0
            };
            vImageParam.push_back(imgParam);
        }
        //
        if( mpInfo_Jpeg.get() )
        {
            sp<IImageStreamInfo> pStreamInfo = mpInfo_Jpeg;
            //
            //pCallbackHandler->setImageStreamInfo(ECamShot_DATA_MSG_JPEG, pStreamInfo,
            //    (mvRegBuf.indexOfKey(ECamShot_BUF_TYPE_JPEG)>=0 ? mvRegBuf.editValueFor(ECamShot_BUF_TYPE_JPEG) : NULL ));
            sp<CallbackBufferPool> pPool = new CallbackBufferPool(pStreamInfo);
            if(mvRegBuf.indexOfKey(ECamShot_BUF_TYPE_JPEG)>=0)
            {
                pPool->addBuffer(mvRegBuf.editValueFor(ECamShot_BUF_TYPE_JPEG));
            }
            else
            {
                pPool->allocateBuffer(
                                  pStreamInfo->getStreamName(),
                                  pStreamInfo->getMaxBufNum(),
                                  pStreamInfo->getMinInitBufNum()
                                  );
            }
            pCallbackHandler->setBufferPool(pPool);
            //
            pFactory->setImageStreamInfo(pStreamInfo);
            pFactory->setUsersPool(
                        pCallbackHandler->queryBufferPool(pStreamInfo->getStreamId())
                    );
            //
            PipelineImageParam imgParam = {
                .pInfo     = pStreamInfo,
                .pProvider = pFactory->create(),
                .usage     = 0
            };
            vImageParam.push_back(imgParam);
        }
        //
        if( OK != pBuilder->setDst(vImageParam) ) {
            MY_LOGE("setDst failed");
            return MFALSE;
        }
    }
    //
    mpPipeline = pBuilder->create();
    CHECK_OBJECT(mpPipeline);
    //
    {
        sp<ResultProcessor> pResultProcessor = mpPipeline->getResultProcessor().promote();
        CHECK_OBJECT(pResultProcessor);

        //partial
        sp<MetadataListener> pListenerPartial = new MetadataListener(this);
        mpResource->add(pListenerPartial);
        //
        pResultProcessor->registerListener( 0, 0, true, pListenerPartial);
        //
        //full
        sp<MetadataListener> pListener = new MetadataListener(this);
        mpResource->add(pListener);
        //
        pResultProcessor->registerListener( 0, 0, false, pListener);
        //
        if( pBuf != nullptr )
        {
            sp<TimestampProcessor> pTimestampProcessor = mpPipeline->getTimestampProcessor().promote();
            pTimestampProcessor->onResultReceived(
                                    0,
                                    eSTREAMID_META_APP_DYNAMIC_P1,
                                    MFALSE,
                                    mSelectorAppMetadata);
        }
        mAppDone = MFALSE;
        mHalDone = MFALSE;

        mAppResultMetadata.clear();
        mHalResultMetadata.clear();
    }
    //
    FUNC_END;
    return MTRUE;
}


/*******************************************************************************
*
********************************************************************************/
MVOID
SingleShot::
onMetaReceived(
    MUINT32         const requestNo,
    StreamId_T      const streamId,
    MBOOL           const errorResult,
    IMetadata       const result
)
{
    CAM_TRACE_FMT_BEGIN("onMetaReceived No%d,StreamID  %#" PRIxPTR, requestNo,streamId);
    MY_LOGD("requestNo %d, stream %#" PRIxPTR, requestNo, streamId);

    Mutex::Autolock _l(mLock);

    if (streamId == eSTREAMID_META_APP_FULL)
    {
      mAppResultMetadata = result;
      mAppDone = MTRUE;
#if 0
      sp<IMetadataConverter> pMetadataConverter;
      pMetadataConverter = IMetadataConverter::createInstance(IDefaultMetadataTagSet::singleton()->getTagSet());
      pMetadataConverter->dump(mAppResultMetadata, 0);
#endif
    }
    else if (streamId == eSTREAMID_META_HAL_FULL)
    {
      mHalResultMetadata = result;
      mHalDone = MTRUE;
    }

    if ((streamId == eSTREAMID_META_APP_FULL || streamId == eSTREAMID_META_HAL_FULL)
      && mAppDone && mHalDone)
    {
      mCond.signal();
    }



    {
      Mutex::Autolock _l(mShutterLock);
      if (mbCBShutter)
      {
        mbCBShutter = MFALSE;
        handleNotifyCallback(ECamShot_NOTIFY_MSG_EOF, 0, 0);
      }
    }
    //
    mAppResultMetadata+=mSelectorAppMetadata;
    //
    if(streamId == eSTREAMID_META_APP_FULL)
        handleDataCallback(ECamShot_DATA_MSG_APPMETA, (MUINTPTR)&mAppResultMetadata, 0, 0);
    else if(streamId == eSTREAMID_META_HAL_FULL)
        handleDataCallback(ECamShot_DATA_MSG_HALMETA, (MUINTPTR)&mHalResultMetadata, 0, 0);
    //
    CAM_TRACE_FMT_END();
}


/*******************************************************************************
*
********************************************************************************/
MVOID
SingleShot::
onDataReceived(
    MUINT32 const               requestNo,
    StreamId_T const            streamId,
    android::sp<IImageBuffer>&  pBuffer
)
{
    CAM_TRACE_FMT_BEGIN("onDataReceived No%d,streamId%d",requestNo,streamId);
    MY_LOGD("requestNo %d, streamId 0x%x, buffer %p", requestNo, streamId, pBuffer.get());
    //
    ECamShotDataMsg data = ECamShot_DATA_MSG_NONE;
    switch (streamId)
    {
        case eSTREAMID_IMAGE_PIPE_RAW_OPAQUE:
            data = ECamShot_DATA_MSG_RAW;
            break;
        case eSTREAMID_IMAGE_PIPE_YUV_JPEG:
            data = ECamShot_DATA_MSG_YUV;
            break;
        case eSTREAMID_IMAGE_PIPE_YUV_00:
            data = ECamShot_DATA_MSG_POSTVIEW;
            break;
        case eSTREAMID_IMAGE_JPEG:
            data = ECamShot_DATA_MSG_JPEG;
            break;
        default:
            data = ECamShot_DATA_MSG_NONE;
            break;
    }
    //
    switch( data )
    {
        case ECamShot_DATA_MSG_RAW:
        case ECamShot_DATA_MSG_YUV:
        case ECamShot_DATA_MSG_POSTVIEW:
        case ECamShot_DATA_MSG_JPEG:
            break;
        default:
            MY_LOGW("not supported yet: 0x%x", data);
            break;
    }

    if( pBuffer != 0 )
    {
        CAM_TRACE_NAME("handleDataCallback");
        // debug
        if( mDumpFlag & data )
        {
            String8 filename = String8::format("%s/singleshot_%dx%d",
                    DUMP_PATH, pBuffer->getImgSize().w, pBuffer->getImgSize().h);
            switch( data )
            {
                case ECamShot_DATA_MSG_RAW:
                    filename += String8::format("_%d.raw", pBuffer->getBufStridesInBytes(0));
                    break;
                case ECamShot_DATA_MSG_YUV:
                case ECamShot_DATA_MSG_POSTVIEW:
                    filename += String8(".yuv");
                    break;
                case ECamShot_DATA_MSG_JPEG:
                    filename += String8(".jpeg");
                    break;
                default:
                    break;
            }
            pBuffer->lockBuf(LOG_TAG, eBUFFER_USAGE_SW_READ_MASK);
            pBuffer->saveToFile(filename);
            pBuffer->unlockBuf(LOG_TAG);
            //
            MY_LOGD("dump buffer in %s", filename.string());
        }
        handleDataCallback(data, 0, 0, pBuffer.get());
    }
    CAM_TRACE_FMT_END();
}


/*******************************************************************************
*
********************************************************************************/

MINT
SingleShot::
getLegacyPipelineMode(void)
{
    int shotMode = getShotMode();
    EPipelineMode pipelineMode = getPipelineMode();
    int legacyPipeLineMode = LegacyPipelineMode_T::PipelineMode_Capture;
    switch(shotMode)
    {
        case eShotMode_VideoSnapShot:
            MY_LOGD("shotMode(%d)",shotMode);
            legacyPipeLineMode = (pipelineMode == ePipelineMode_Feature) ?
                LegacyPipelineMode_T::PipelineMode_Feature_Capture_VSS :
                LegacyPipelineMode_T::PipelineMode_Capture_VSS;
            break;

        default:
            legacyPipeLineMode = (pipelineMode == ePipelineMode_Feature) ?
                LegacyPipelineMode_T::PipelineMode_Feature_Capture :
                LegacyPipelineMode_T::PipelineMode_Capture;
            break;
    }
    return legacyPipeLineMode;
}

/*******************************************************************************
*
********************************************************************************/
status_t
SingleShot::
getSelectorData(
    ISelector*                  pSelector,
    ShotParam&                  rShotParam,
    android::sp<IImageBuffer>&  pBuffer)
{
    if(pSelector==NULL)
    {
        MY_LOGE("input Selector is NULL!");
        return UNKNOWN_ERROR;
    }

    status_t status = OK;
    MINT32 rRequestNo;
    Vector<ISelector::MetaItemSet> rvResultMeta;
    android::sp<IImageBufferHeap> rpHeap;
    status = pSelector->getResult(rRequestNo, rvResultMeta, rpHeap);
    if(status!=OK)
    {
        MY_LOGE("Selector getResult Fail!");
        return UNKNOWN_ERROR;
    }

    if(rvResultMeta.size()!=2)
    {
        MY_LOGE("ZsdSelect getResult rvResultMeta != 2");
        return UNKNOWN_ERROR;
    }

    MBOOL bAppSetting = MFALSE, bHalSetting = MFALSE;
    for(MUINT32 i=0; i<rvResultMeta.size(); i++)
    {
        if(rvResultMeta.editItemAt(i).id == eSTREAMID_META_APP_DYNAMIC_P1)
        {
            mSelectorAppMetadata = rvResultMeta.editItemAt(i).meta;
            bAppSetting = MTRUE;
        }
        else
        if(rvResultMeta.editItemAt(i).id == eSTREAMID_META_HAL_DYNAMIC_P1)
        {
            // overwrite some setting
            rShotParam.halSetting += rvResultMeta.editItemAt(i).meta;
            bHalSetting = MTRUE;
        }
    }
    //
    if( !bHalSetting ||
        !bAppSetting)
    {
        MY_LOGE("can't find App(%d)/Hal(%d) Setting from ZsdSelect getResult rvResultMeta",
                bAppSetting,
                bHalSetting);
        return UNKNOWN_ERROR;
    }
    //
    pBuffer = rpHeap->createImageBuffer();
    //
    if(pBuffer == NULL)
    {
        MY_LOGE("get buffer is NULL!");
        return UNKNOWN_ERROR;
    }
    return OK;
}

////////////////////////////////////////////////////////////////////////////////
};  //namespace NSCamShot
