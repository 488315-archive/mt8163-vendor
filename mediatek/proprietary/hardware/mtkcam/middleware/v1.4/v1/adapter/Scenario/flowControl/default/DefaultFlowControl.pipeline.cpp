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

#define LOG_TAG "MtkCam/DefaultFlowControl"
//
#include "MyUtils.h"
//
#include <v3/utils/streaminfo/MetaStreamInfo.h>
#include <v3/utils/streaminfo/ImageStreamInfo.h>
#include <v3/utils/streambuf/StreamBuffers.h>

#include <LegacyPipeline/StreamId.h>
#include <LegacyPipeline/NodeId.h>
#include <v1/IParamsManager.h>
#include <LegacyPipeline/ILegacyPipeline.h>
#include <LegacyPipeline/LegacyPipelineBuilder.h>
#include <LegacyPipeline/buffer/StreamBufferProviderFactory.h>
#include "DefaultFlowControl.h"
#include <buffer/ClientBufferPool.h>
#include <LegacyPipeline/buffer/BufferPoolImp.h>

#warning "FIXME"
#include <LegacyPipeline/buffer/Selector.h>

using namespace NSCam;
using namespace NSCam::v1;
using namespace NSCam::v1::NSLegacyPipeline;
using namespace android;
using namespace NSCam::v3;
using namespace NSCamHW;

/******************************************************************************
 *
 ******************************************************************************/
#define MY_LOGV(fmt, arg...)        CAM_LOGV("[%d:%s] " fmt, getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_LOGD("[%d:%s] " fmt, getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_LOGI("[%d:%s] " fmt, getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_LOGW("[%d:%s] " fmt, getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_LOGE("[%d:%s] " fmt, getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGA(fmt, arg...)        CAM_LOGA("[%d:%s] " fmt, getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGF(fmt, arg...)        CAM_LOGF("[%d:%s] " fmt, getOpenId(), __FUNCTION__, ##arg)
//
#define MY_LOGV_IF(cond, ...)       do { if ( (cond) ) { MY_LOGV(__VA_ARGS__); } }while(0)
#define MY_LOGD_IF(cond, ...)       do { if ( (cond) ) { MY_LOGD(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) ) { MY_LOGI(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) ) { MY_LOGW(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) ) { MY_LOGE(__VA_ARGS__); } }while(0)
#define MY_LOGA_IF(cond, ...)       do { if ( (cond) ) { MY_LOGA(__VA_ARGS__); } }while(0)
#define MY_LOGF_IF(cond, ...)       do { if ( (cond) ) { MY_LOGF(__VA_ARGS__); } }while(0)
//
#define MY_LOGD1(...)               MY_LOGD_IF((mLogLevel>=1),__VA_ARGS__)
#define MY_LOGD2(...)               MY_LOGD_IF((mLogLevel>=2),__VA_ARGS__)
#define MY_LOGD3(...)               MY_LOGD_IF((mLogLevel>=3),__VA_ARGS__)
//
#define FUNC_START                  MY_LOGD1("+")
#define FUNC_END                    MY_LOGD1("-")
//
#define TEST_RAW_ALLOCATE (0)
#define SUPPORT_VSS (1)
//
/******************************************************************************
 *
 ******************************************************************************/
MERROR
DefaultFlowControl::
decideRrzoImage(
    HwInfoHelper& helper,
    MUINT32 const bitDepth,
    MSize         referenceSize,
    MUINT const   usage,
    MINT32 const  minBuffer,
    MINT32 const  maxBuffer,
    sp<IImageStreamInfo>& rpImageStreamInfo
)
{
    MSize autualSize;
    size_t stride;
    MINT format;
    if( ! helper.getRrzoFmt(bitDepth, format) ||
        ! helper.alignRrzoHwLimitation(referenceSize, mSensorParam.size, autualSize) ||
        ! helper.alignPass1HwLimitation(mSensorParam.pixelMode, format, false, autualSize, stride) )
    {
        MY_LOGE("wrong params about rrzo");
        return BAD_VALUE;
    }
    //
    MY_LOGD("rrzo bitDepth:%d format:%d referenceSize:%dx%d, actual size:%dx%d, stride:%d",
                bitDepth,
                format,
                referenceSize.w, referenceSize.h,
                autualSize.w, autualSize.h,
                stride
            );
    rpImageStreamInfo =
        createRawImageStreamInfo(
            "Hal:Image:Resiedraw",
            eSTREAMID_IMAGE_PIPE_RAW_RESIZER,
            eSTREAMTYPE_IMAGE_INOUT,
            maxBuffer, minBuffer,
            usage, format, autualSize, stride
            );

    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
DefaultFlowControl::
decideImgoImage(
    HwInfoHelper& helper,
    MUINT32 const bitDepth,
    MSize         referenceSize,
    MUINT  const  usage,
    MINT32 const  minBuffer,
    MINT32 const  maxBuffer,
    sp<IImageStreamInfo>& rpImageStreamInfo
)
{
    MSize autualSize = referenceSize;
    size_t stride;
    MINT format;
    if( ! helper.getImgoFmt(bitDepth, format) ||
        ! helper.alignPass1HwLimitation(mSensorParam.pixelMode, format, true, autualSize, stride) )
    {
        MY_LOGE("wrong params about imgo");
        return BAD_VALUE;
    }
    //
    MY_LOGD("imgo bitDepth:%d format:%d referenceSize:%dx%d, actual size:%dx%d, stride:%d",
                bitDepth,
                format,
                referenceSize.w, referenceSize.h,
                autualSize.w, autualSize.h,
                stride
            );
    rpImageStreamInfo =
        createRawImageStreamInfo(
            "Hal:Image:Fullraw",
            eSTREAMID_IMAGE_PIPE_RAW_OPAQUE,
            eSTREAMTYPE_IMAGE_INOUT,
            maxBuffer, minBuffer,
            usage, format, autualSize, stride
            );

    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
DefaultFlowControl::
setCamClient(
    char* const name,
    StreamId streamId,
    Vector<PipelineImageParam>& vImageParam,
    Vector<MUINT32> clientMode,
    MUINT usage,
    MBOOL useTransform
)
{
    MSize const& size = MSize(-1,-1);
    MINT const format = eImgFmt_YUY2;
    size_t const stride = 1280;
    sp<IImageStreamInfo> pImage_Yuv =
        createImageStreamInfo(
            name,
            streamId,
            eSTREAMTYPE_IMAGE_INOUT,
            5, 1,
            usage, format, size, 0
            );
    sp<ClientBufferPool> pClient = new ClientBufferPool(getOpenId(), useTransform);
    pClient->setCamClient(
                        name,
                        mpImgBufProvidersMgr,
                        clientMode[0]
                    );
    for ( size_t i = 1; i < clientMode.size(); ++i ) {
        pClient->setCamClient( clientMode[i] );
    }

    sp<StreamBufferProviderFactory> pFactory = StreamBufferProviderFactory::createInstance();
    pFactory->setImageStreamInfo(pImage_Yuv);
    pFactory->setUsersPool(pClient);

    vImageParam.push_back(
        PipelineImageParam{
            pImage_Yuv,
            pFactory->create(),
            0
        }
    );

    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
DefaultFlowControl::
constructNormalPreviewPipeline()
{
    FUNC_START;

    CAM_TRACE_NAME("DFC:constructNormalPreviewPipeline");

    mLPBConfigParams.mode = LegacyPipelineMode_T::PipelineMode_Preview;

    sp<LegacyPipelineBuilder> pBuilder =
        LegacyPipelineBuilder::createInstance(
                                    mOpenId,
                                    "NormalPreview",
                                    mLPBConfigParams);
    if ( pBuilder == 0 ) {
        MY_LOGE("Cannot create LegacyPipelineBuilder.");
        return BAD_VALUE;
    }

    HwInfoHelper helper(mOpenId);
    if( ! helper.updateInfos() ) {
        MY_LOGE("cannot properly update infos");
        return BAD_VALUE;
    }
    //
    mSensorParam.mode = SENSOR_SCENARIO_ID_NORMAL_PREVIEW;
    if( ! helper.getSensorSize( mSensorParam.mode, mSensorParam.size) ||
        ! helper.getSensorFps( (MUINT32)mSensorParam.mode, (MINT32&)mSensorParam.fps) ||
        ! helper.queryPixelMode( mSensorParam.mode, mSensorParam.fps, mSensorParam.pixelMode)
      ) {
        MY_LOGE("cannot get params about sensor");
        return BAD_VALUE;
    }
    // Sensor
    MY_LOGD("sensor mode:%d size:%dx%d, fps:%d pixel:%d",
        mSensorParam.mode,
        mSensorParam.size.w, mSensorParam.size.h,
        mSensorParam.fps,
        mSensorParam.pixelMode);
    pBuilder->setSrc(mSensorParam);
    //
    sp<IScenarioControl> pScenarioCtrl = enterScenarioControl(getScenario(), mSensorParam.size, mSensorParam.fps);
    if( pScenarioCtrl.get() == NULL )
    {
        MY_LOGE("get Scenario Control fail");
        return BAD_VALUE;
    }
    pBuilder->setScenarioControl(pScenarioCtrl);

    // Image
    {
        CAM_TRACE_NAME("DFC:SetImageDst");
        Vector<PipelineImageParam> vImageParam;
        // RAW
        {
            MUINT32 const bitDepth = 10;
            MSize previewsize;
            MUINT const usage = eBUFFER_USAGE_SW_READ_OFTEN | eBUFFER_USAGE_HW_CAMERA_READWRITE; //not necessary here
            mpParamsManager->getPreviewSize(&previewsize.w, &previewsize.h);
            sp<IImageStreamInfo> pImage_Raw;

            if ( OK != decideRrzoImage(
                            helper, bitDepth,
                            previewsize, usage,
                            2, 8,
                            pImage_Raw
                        ))
            {
                MY_LOGE("No rrzo image");
                return BAD_VALUE;
            }

            vImageParam.push_back(
                PipelineImageParam{
                    pImage_Raw,
                    NULL,
                    0
                }
            );
        }
        // YUV preview -> display client
        {
            Vector<MUINT32> clientMode;
            clientMode.push_back(IImgBufProvider::eID_DISPLAY);

            setCamClient(
                "Hal:Image:yuvDisp",
                eSTREAMID_IMAGE_PIPE_YUV_00,
                vImageParam,
                clientMode,
                GRALLOC_USAGE_HW_COMPOSER,
                MTRUE
            );
        }
        // YUV preview callback -> preview callback client
        {
            Vector<MUINT32> clientMode;
            clientMode.push_back(IImgBufProvider::eID_PRV_CB);
            clientMode.push_back(IImgBufProvider::eID_GENERIC);

            setCamClient(
                "Hal:Image:yuvPrvCB",
                eSTREAMID_IMAGE_PIPE_YUV_01,
                vImageParam,
                clientMode
            );
        }
        // YUV FD & OT
        {
            Vector<MUINT32> clientMode;
            clientMode.push_back(IImgBufProvider::eID_FD);
            clientMode.push_back(IImgBufProvider::eID_OT);

            setCamClient(
                "Hal:Image:yuvFD",
                eSTREAMID_IMAGE_YUV_FD,
                vImageParam,
                clientMode
            );
        }

        pBuilder->setDst(vImageParam);
    }

    mpPipeline = pBuilder->create();

    if ( mpPipeline == 0) {
        MY_LOGE("Fail to create Legacy Pipeline.");
        return BAD_VALUE;
    }
    //
    sp<ResultProcessor> pProcessor = mpPipeline->getResultProcessor().promote();
    sp<FrameInfo> pFrameInfo = new FrameInfo();
    mpResourceContainer->setLatestFrameInfo(pFrameInfo);
    pProcessor->registerListener(
                    eSTREAMID_META_APP_DYNAMIC_P1,
                    pFrameInfo
                    );
    //
    FUNC_END;

    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
DefaultFlowControl::
constructRecordingPipeline()
{
    FUNC_START;

    CAM_TRACE_NAME("DFC:constructRecordingPipeline");

    mSensorParam.mode = SENSOR_SCENARIO_ID_NORMAL_VIDEO;

    if(mpParamsManager->getInt(CameraParameters::KEY_PREVIEW_FRAME_RATE) > HighSpeedVideoFpsBase)
    {
        mLPBConfigParams.mode = LegacyPipelineMode_T::PipelineMode_HighSpeedVideo;
        mpRequestThreadLoopCnt = mpParamsManager->getInt(CameraParameters::KEY_PREVIEW_FRAME_RATE)/HighSpeedVideoFpsBase;
        selectHighSpeedSensorScen(
            mpParamsManager->getInt(CameraParameters::KEY_PREVIEW_FRAME_RATE),
            mSensorParam.mode);
    }
    else
    {
        mLPBConfigParams.mode = LegacyPipelineMode_T::PipelineMode_VideoRecord;
    }

    sp<LegacyPipelineBuilder> pBuilder =
        LegacyPipelineBuilder::createInstance(
                                    mOpenId,
                                    "NormalRecord",
                                    mLPBConfigParams);
    if ( pBuilder == 0 ) {
        MY_LOGE("Cannot create LegacyPipelineBuilder.");
        return BAD_VALUE;
    }

    HwInfoHelper helper(mOpenId);
    if( ! helper.updateInfos() ) {
        MY_LOGE("cannot properly update infos");
        return BAD_VALUE;
    }
    //
    if( ! helper.getSensorSize( mSensorParam.mode, mSensorParam.size) ||
        ! helper.getSensorFps( (MUINT32)mSensorParam.mode, (MINT32&)mSensorParam.fps) ||
        ! helper.queryPixelMode( mSensorParam.mode, mSensorParam.fps, mSensorParam.pixelMode)
      ) {
        MY_LOGE("cannot get params about sensor");
        return BAD_VALUE;
    }
    // Sensor
    MY_LOGD("sensor mode:%d size:%dx%d, fps:%d pixel:%d",
        mSensorParam.mode,
        mSensorParam.size.w, mSensorParam.size.h,
        mSensorParam.fps,
        mSensorParam.pixelMode);
    pBuilder->setSrc(mSensorParam);
    //
    sp<IScenarioControl> pScenarioCtrl = enterScenarioControl(getScenario(), mSensorParam.size, mSensorParam.fps);
    if( pScenarioCtrl.get() == NULL )
    {
        MY_LOGE("get Scenario Control fail");
        return BAD_VALUE;
    }
    pBuilder->setScenarioControl(pScenarioCtrl);

    // Image
    sp<PairMetadata> pPair;
    {
        CAM_TRACE_NAME("DFC:SetImageDst");
        Vector<PipelineImageParam> vImageParam;
        // RAW
        {
            MUINT32 const bitDepth = 10;
            MUINT const usage = eBUFFER_USAGE_SW_READ_OFTEN | eBUFFER_USAGE_HW_CAMERA_READWRITE; //not necessary here

            //calculate rrzo size
            MSize paramSize, recordsize;
            mpParamsManager->getVideoSize(&paramSize.w, &paramSize.h);
            if( 32*mSensorParam.size.h/mSensorParam.size.w >= 21 )
            {
                recordsize = MSize(1440,1080);
            }
            else
            {
                recordsize = MSize(1920,1080);
            }
            if( paramSize.w > recordsize.w )
            {
                recordsize.w = paramSize.w;
            }
            if( paramSize.h > recordsize.h )
            {
                recordsize.h = paramSize.h;
            }
            recordsize.w *= 1.2;//for EIS
            recordsize.h *= 1.2;//for EIS

            sp<IImageStreamInfo> pImage_Raw;

            if ( OK != decideRrzoImage(
                            helper, bitDepth,
                            recordsize, usage,
                            4, 8,
                            pImage_Raw
                        ))
            {
                MY_LOGE("No rrzo image");
                return BAD_VALUE;
            }

            mRecordRrzoSize = pImage_Raw->getImgSize();

            vImageParam.push_back(
                PipelineImageParam{
                    pImage_Raw,
                    NULL,
                    0
                }
            );

#if SUPPORT_VSS
            // for VSS
            if ( OK != decideImgoImage(
                            helper, bitDepth,
                            mSensorParam.size, usage,
                            4, 8,
                            pImage_Raw
                        ))
            {
                MY_LOGE("No imgo image");
                return BAD_VALUE;
            }
            pPair = PairMetadata::createInstance(pImage_Raw->getStreamName());

            sp<StreamBufferProviderFactory> pFactory = StreamBufferProviderFactory::createInstance();
            pFactory->setImageStreamInfo(pImage_Raw);
            pFactory->setPairRule(pPair, 2);
            sp<StreamBufferProvider> pProducer = pFactory->create();

            mpResourceContainer->setConsumer(pImage_Raw->getStreamId(),pProducer);

            vImageParam.push_back(
                PipelineImageParam{
                    pImage_Raw,
                    pProducer,
                    0
                }
            );
#endif
        }
        // YUV preview -> display client
        {
            Vector<MUINT32> clientMode;
            clientMode.push_back(IImgBufProvider::eID_DISPLAY);

            setCamClient(
                "Hal:Image:yuvDisp",
                eSTREAMID_IMAGE_PIPE_YUV_00,
                vImageParam,
                clientMode,
                GRALLOC_USAGE_HW_COMPOSER,
                MTRUE
            );
        }
        // YUV record
        {
            Vector<MUINT32> clientMode;
            clientMode.push_back(IImgBufProvider::eID_REC_CB);

            setCamClient(
                "Hal:Image:yuvRecord",
                eSTREAMID_IMAGE_PIPE_YUV_01,
                vImageParam,
                clientMode,
                GRALLOC_USAGE_HW_VIDEO_ENCODER
            );
        }

        pBuilder->setDst(vImageParam);
    }

    mpPipeline = pBuilder->create();

    if ( mpPipeline == 0) {
        MY_LOGE("Fail to create Legacy Pipeline.");
        return BAD_VALUE;
    }

#if SUPPORT_VSS
    sp<StreamBufferProvider> pTempConsumer = mpResourceContainer->queryConsumer( eSTREAMID_IMAGE_PIPE_RAW_OPAQUE );

    MY_LOGD("provider(%x)", pTempConsumer.get());

    if ( pTempConsumer != 0 ) {
        MY_LOGD("provider(%x)", pTempConsumer.get());
        sp<ResultProcessor> pProcessor = mpPipeline->getResultProcessor().promote();
        pProcessor->registerListener(
                        eSTREAMID_META_APP_DYNAMIC_P1,
                        pPair
                        );
        pProcessor->registerListener(
                        eSTREAMID_META_HAL_DYNAMIC_P1,
                        pPair
                        );
        //
        sp<FrameInfo> pFrameInfo = new FrameInfo();
        mpResourceContainer->setLatestFrameInfo(pFrameInfo);
        pProcessor->registerListener(
                        eSTREAMID_META_APP_DYNAMIC_P1,
                        pFrameInfo
                        );
        //
        sp<VssSelector> pSelector = new VssSelector();
        pTempConsumer->setSelector(pSelector);
    }
    else
    {
        MY_LOGE("provider is NULL");
    }

#endif
    FUNC_END;

    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
DefaultFlowControl::
constructZsdPreviewPipeline()
{
    FUNC_START;

    CAM_TRACE_NAME("DFC:constructZsdPreviewPipeline");

    mLPBConfigParams.mode = LegacyPipelineMode_T::PipelineMode_ZsdPreview;

    sp<LegacyPipelineBuilder> pBuilder =
        LegacyPipelineBuilder::createInstance(
                                    mOpenId,
                                    "ZSD",
                                    mLPBConfigParams);
    if ( pBuilder == 0 ) {
        MY_LOGE("Cannot create LegacyPipelineBuilder.");
        return BAD_VALUE;
    }

    HwInfoHelper helper(mOpenId);
    if( ! helper.updateInfos() ) {
        MY_LOGE("cannot properly update infos");
        return BAD_VALUE;
    }
    //
    mSensorParam.mode = SENSOR_SCENARIO_ID_NORMAL_CAPTURE;
    if( ! helper.getSensorSize( mSensorParam.mode, mSensorParam.size) ||
        ! helper.getSensorFps( (MUINT32)mSensorParam.mode, (MINT32&)mSensorParam.fps) ||
        ! helper.queryPixelMode( mSensorParam.mode, mSensorParam.fps, mSensorParam.pixelMode)
      ) {
        MY_LOGE("cannot get params about sensor");
        return BAD_VALUE;
    }
    // Sensor
    MY_LOGD("sensor mode:%d size:%dx%d, fps:%d pixel:%d",
        mSensorParam.mode,
        mSensorParam.size.w, mSensorParam.size.h,
        mSensorParam.fps,
        mSensorParam.pixelMode);
    pBuilder->setSrc(mSensorParam);
    //

    sp<IScenarioControl> pScenarioCtrl = enterScenarioControl(getScenario(), mSensorParam.size, mSensorParam.fps);
    if( pScenarioCtrl.get() == NULL )
    {
        MY_LOGE("get Scenario Control fail");
        return BAD_VALUE;
    }
    pBuilder->setScenarioControl(pScenarioCtrl);


    // Image
    sp<PairMetadata>  pPair;
    sp<BufferPoolImp> pFullRawPool;
    sp<IImageStreamInfo> pImage_Raw;
    {
        Vector<PipelineImageParam> vImageParam;
        // RAW
        {
            CAM_TRACE_NAME("allocate RAW");
            MUINT32 const bitDepth = 10;
            MSize previewsize;
            MUINT const usage = eBUFFER_USAGE_SW_READ_OFTEN | eBUFFER_USAGE_HW_CAMERA_READWRITE; //not necessary here
            mpParamsManager->getPreviewSize(&previewsize.w, &previewsize.h);

            if ( OK != decideRrzoImage(
                            helper, bitDepth,
                            previewsize, usage,
                            0, 8,
                            pImage_Raw
                        ))
            {
                MY_LOGE("No rrzo image");
                return BAD_VALUE;
            }

            vImageParam.push_back(
                PipelineImageParam{
                    pImage_Raw,
                    NULL,
                    0
                }
            );

#if 1
            if ( OK != decideImgoImage(
                            helper, bitDepth,
                            mSensorParam.size, usage,
                            0, 12,
                            pImage_Raw
                        ))
            {
                MY_LOGE("No imgo image");
                return BAD_VALUE;
            }
            pPair = PairMetadata::createInstance(pImage_Raw->getStreamName());
            pFullRawPool = new BufferPoolImp(pImage_Raw);

            sp<StreamBufferProviderFactory> pFactory = StreamBufferProviderFactory::createInstance();
            pFactory->setImageStreamInfo(pImage_Raw);
            pFactory->setPairRule(pPair, 2);
            pFactory->setUsersPool(pFullRawPool);
            sp<StreamBufferProvider> pProducer = pFactory->create();

            mpResourceContainer->setConsumer( pImage_Raw->getStreamId(), pProducer);

            vImageParam.push_back(
                PipelineImageParam{
                    pImage_Raw,
                    pProducer,
                    0
                }
            );
#endif
        }
        // YUV preview -> display client
        {
            Vector<MUINT32> clientMode;
            clientMode.push_back(IImgBufProvider::eID_DISPLAY);

            setCamClient(
                "Hal:Image:yuvDisp",
                eSTREAMID_IMAGE_PIPE_YUV_00,
                vImageParam,
                clientMode,
                GRALLOC_USAGE_HW_COMPOSER,
                MTRUE
            );
        }
        // YUV preview callback -> preview callback client
        {
            Vector<MUINT32> clientMode;
            clientMode.push_back(IImgBufProvider::eID_PRV_CB);
            clientMode.push_back(IImgBufProvider::eID_GENERIC);

            setCamClient(
                "Hal:Image:yuvPrvCB",
                eSTREAMID_IMAGE_PIPE_YUV_01,
                vImageParam,
                clientMode
            );
        }
        // YUV FD & OT
        {
            Vector<MUINT32> clientMode;
            clientMode.push_back(IImgBufProvider::eID_FD);
            clientMode.push_back(IImgBufProvider::eID_OT);

            setCamClient(
                "Hal:Image:yuvFD",
                eSTREAMID_IMAGE_YUV_FD,
                vImageParam,
                clientMode
            );
        }

        pBuilder->setDst(vImageParam);
    }

    mpPipeline = pBuilder->create();

    if ( mpPipeline == 0) {
        MY_LOGE("Fail to create Legacy Pipeline.");
        return BAD_VALUE;
    }
    //
    pFullRawPool->allocateBuffer(
            pImage_Raw->getStreamName(),
            pImage_Raw->getMaxBufNum(),
            pImage_Raw->getMinInitBufNum()
            );

#if 1
    sp<StreamBufferProvider> pTempConsumer =
        mpResourceContainer->queryConsumer( eSTREAMID_IMAGE_PIPE_RAW_OPAQUE );

    if ( pTempConsumer != 0 ) {
        CAM_TRACE_NAME("DFC:SetSelector");
        sp<ResultProcessor> pProcessor = mpPipeline->getResultProcessor().promote();
        pProcessor->registerListener(
                        eSTREAMID_META_APP_DYNAMIC_P1,
                        pPair
                        );
        pProcessor->registerListener(
                        eSTREAMID_META_HAL_DYNAMIC_P1,
                        pPair
                        );
        //
        sp<FrameInfo> pFrameInfo = new FrameInfo();
        mpResourceContainer->setLatestFrameInfo(pFrameInfo);
        pProcessor->registerListener(
                        eSTREAMID_META_APP_DYNAMIC_P1,
                        pFrameInfo
                        );
        //
        sp<ZsdSelector> pSelector = new ZsdSelector();
        pTempConsumer->setSelector(pSelector);
    }
#endif

    FUNC_END;

    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
DefaultFlowControl::
constructVSSRequest(
    Vector< StreamId >&   vDstStreams
)
{
    FUNC_START;

    CAM_TRACE_NAME("DFC:constructVSSRequest");

    // stream
    vDstStreams.push_back(eSTREAMID_IMAGE_PIPE_RAW_OPAQUE);
    vDstStreams.push_back(eSTREAMID_IMAGE_PIPE_RAW_RESIZER);
    vDstStreams.push_back(eSTREAMID_IMAGE_PIPE_YUV_00);
    vDstStreams.push_back(eSTREAMID_IMAGE_PIPE_YUV_01);

    FUNC_END;
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MINT32
DefaultFlowControl::
getScenario() const
{
    switch(mLPBConfigParams.mode)
    {
        case LegacyPipelineMode_T::PipelineMode_Preview:
            return IScenarioControl::Scenario_NormalPreivew;
        case LegacyPipelineMode_T::PipelineMode_ZsdPreview:
            return IScenarioControl::Scenario_ZsdPreview;
        case LegacyPipelineMode_T::PipelineMode_VideoRecord:
            return IScenarioControl::Scenario_VideoRecord;
        case LegacyPipelineMode_T::PipelineMode_HighSpeedVideo:
            return IScenarioControl::Scenario_HighSpeedVideo;
        default:
            MY_LOGW("no mapped scenario for mode %d", mLPBConfigParams.mode);
            break;
    }
    return IScenarioControl::Scenario_None;
}
