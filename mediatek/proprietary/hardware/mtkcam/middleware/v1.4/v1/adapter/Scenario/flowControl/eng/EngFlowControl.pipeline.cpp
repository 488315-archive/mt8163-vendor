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

#define LOG_TAG "MtkCam/EngFlowControl"
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
#include "EngFlowControl.h"
#include <buffer/ClientBufferPool.h>


using namespace NSCam;
using namespace NSCam::v1;
using namespace NSCam::v1::NSLegacyPipeline;
using namespace android;
using namespace NSCam::v3;
using namespace NSCamHW;

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

#if 1
#define FUNC_START     MY_LOGD("+")
#define FUNC_END       MY_LOGD("-")
#else
#define FUNC_START
#define FUNC_END
#endif

#define TEST_RAW_ALLOCATE (0)


/******************************************************************************
 *
 ******************************************************************************/
MERROR
EngFlowControl::
decideRrzoImage(
    HwInfoHelper& helper,
    MUINT32 const bitDepth,
    MSize referenceSize,
    MINT format,
    MUINT const usage,
    Vector<PipelineImageParam>& vImageParam

)
{
    MSize autualSize;
    size_t stride;
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
    sp<IImageStreamInfo> pImage_RrzoRaw =
        createRawImageStreamInfo(
            "Hal:Image:Resiedraw",
            eSTREAMID_IMAGE_PIPE_RAW_RESIZER,
            eSTREAMTYPE_IMAGE_INOUT,
            8, 4,
            usage, format, autualSize, stride
            );

#if TEST_RAW_ALLOCATE
    sp<StreamBufferProviderFactory> pFactory = StreamBufferProviderFactory::createInstance();
    //pFactory->setName(pImage_RrzoRaw->getStreamName());
    pFactory->setImageStreamInfo(pImage_RrzoRaw);

    vImageParam.push_back(
        PipelineImageParam{
            pImage_RrzoRaw,
            pFactory->create(),
            0
        }
    );
#else
    vImageParam.push_back(
        PipelineImageParam{
            pImage_RrzoRaw,
            NULL,
            0
        }
    );
#endif
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
EngFlowControl::
setCamClient(
    char* const name,
    StreamId streamId,
    Vector<PipelineImageParam>& vImageParam,
    Vector<MUINT32> clientMode
)
{
    MSize const& size = MSize(-1,-1);
    MINT const format = eImgFmt_YUY2;
    size_t const stride = 1280;
    MUINT const usage = 0;
    sp<IImageStreamInfo> pImage_Yuv =
        createImageStreamInfo(
            name,
            streamId,
            eSTREAMTYPE_IMAGE_INOUT,
            5, 1,
            usage, format, size, 0
            );
    sp<ClientBufferPool> pClient = new ClientBufferPool(getOpenId());
    pClient->setCamClient(
                        name,
                        mpImgBufProvidersMgr,
                        clientMode[0]
                    );
    for ( size_t i = 1; i < clientMode.size(); ++i ) {
        pClient->setCamClient( clientMode[i] );
    }

    sp<StreamBufferProviderFactory> pFactory = StreamBufferProviderFactory::createInstance();
    //pFactory->setName(pImage_Yuv->getStreamName());
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
EngFlowControl::
constructNormalPreviewPipeline()
{
    FUNC_START;
    LegacyPipelineBuilder::ConfigParams LPBConfigParams;
    LPBConfigParams.mode = LegacyPipelineMode_T::PipelineMode_Preview;
    LPBConfigParams.enableEIS = MFALSE;
    LPBConfigParams.enableLCS = MFALSE;

    sp<LegacyPipelineBuilder> pBuilder =
        LegacyPipelineBuilder::createInstance(
                                    mOpenId,
                                    "NormalPreview",
                                    LPBConfigParams);
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
    { // start of Engineer Mode
            String8 ms8SaveMode = mpParamsManager->getStr(MtkCameraParameters::KEY_RAW_SAVE_MODE);

            int camera_mode = mpParamsManager->getInt(MtkCameraParameters::KEY_CAMERA_MODE);
            if (camera_mode != 0)
            {
                ms8SaveMode = String8("4"); // force sensor scenario to "normal video" // should remove when em camera app modifed
                MY_LOGW("camera_mode != 0 and force sensor scenario to normal video");
            }

            const char *strSaveMode = ms8SaveMode.string();
            mSensorParam.mode = SENSOR_SCENARIO_ID_NORMAL_PREVIEW;

            switch (strSaveMode[0])
            {
                case '1': // 1: "Preview Mode",
                    // mSensorParam.mode = SENSOR_SCENARIO_ID_NORMAL_PREVIEW;
                    MY_LOGD("SENSOR_SCENARIO_ID_NORMAL_PREVIEW");
                    break;
                case '2': // 2: "Capture Mode",
                    // mSensorParam.mode = SENSOR_SCENARIO_ID_NORMAL_CAPTURE;
                    MY_LOGD("SENSOR_SCENARIO_ID_NORMAL_PREVIEW");
                    break;
                case '4': // 4: "Video Preview Mode"
                    mSensorParam.mode = SENSOR_SCENARIO_ID_NORMAL_VIDEO;
                    MY_LOGD("SENSOR_SCENARIO_ID_NORMAL_VIDEO");
                    break;
                case '5': // 5: "Slim Video 1"
                    mSensorParam.mode = SENSOR_SCENARIO_ID_SLIM_VIDEO1;
                    MY_LOGD("SENSOR_SCENARIO_ID_SLIM_VIDEO1");
                    break;
                case '6': // 6: "Slim Video 2"
                    mSensorParam.mode = SENSOR_SCENARIO_ID_SLIM_VIDEO2;
                    MY_LOGD("SENSOR_SCENARIO_ID_SLIM_VIDEO2");
                    break;
                case '0': // 0: do not save
                case '3': // 3: "JPEG Only"
                default:
                    mSensorParam.mode = SENSOR_SCENARIO_ID_NORMAL_PREVIEW;
                    MY_LOGD("SENSOR_SCENARIO_ID_NORMAL_PREVIEW");
                    break;
            }
        }// end of Engineer Mode setting
    //
    //mSensorParam.mode = SENSOR_SCENARIO_ID_NORMAL_PREVIEW;
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
    sp<IScenarioControl> pScenarioCtrl = enterScenarioControl(IScenarioControl::Scenario_NormalPreivew, mSensorParam.size, mSensorParam.fps);
    if( pScenarioCtrl.get() == NULL )
    {
        MY_LOGE("get Scenario Control fail");
        return BAD_VALUE;
    }
    pBuilder->setScenarioControl(pScenarioCtrl);

    // Image
    {
        Vector<PipelineImageParam> vImageParam;
        // RAW
        {
            MUINT32 const bitDepth = 10;
            MSize previewsize;
            MSize size;
            MINT format;
            size_t stride;
            MUINT const usage = eBUFFER_USAGE_SW_READ_OFTEN | eBUFFER_USAGE_HW_CAMERA_READWRITE; //not necessary here
            mpParamsManager->getPreviewSize(&previewsize.w, &previewsize.h);

            if ( OK != decideRrzoImage(
                            helper, bitDepth,
                            previewsize, format, usage,
                            vImageParam

                        ))
            {
                MY_LOGE("No rrzo image");
                return BAD_VALUE;
            }
        }
        // YUV preview -> display client
        {
            Vector<MUINT32> clientMode;
            clientMode.push_back(IImgBufProvider::eID_DISPLAY);

            setCamClient(
                "Hal:Image:yuvDisp",
                eSTREAMID_IMAGE_PIPE_YUV_00,
                vImageParam,
                clientMode
            );
        }
        // YUV preview callback -> preview callback client
        {
            Vector<MUINT32> clientMode;
            clientMode.push_back(IImgBufProvider::eID_PRV_CB);

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
EngFlowControl::
constructRecordingPipeline()
{
    FUNC_START;
    LegacyPipelineBuilder::ConfigParams LPBConfigParams;
    LPBConfigParams.mode = LegacyPipelineMode_T::PipelineMode_VideoRecord;
    LPBConfigParams.enableEIS = MFALSE;
    LPBConfigParams.enableLCS = MFALSE;

    sp<LegacyPipelineBuilder> pBuilder =
        LegacyPipelineBuilder::createInstance(
                                    mOpenId,
                                    "NormalRecord",
                                    LPBConfigParams);
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
    mSensorParam.mode = SENSOR_SCENARIO_ID_NORMAL_VIDEO;
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
    sp<IScenarioControl> pScenarioCtrl = enterScenarioControl(IScenarioControl::Scenario_VideoRecord, mSensorParam.size, mSensorParam.fps);
    if( pScenarioCtrl.get() == NULL )
    {
        MY_LOGE("get Scenario Control fail");
        return BAD_VALUE;
    }
    pBuilder->setScenarioControl(pScenarioCtrl);

    // Image
    {
        Vector<PipelineImageParam> vImageParam;
        // RAW
        {
            MUINT32 const bitDepth = 10;
            MSize recordsize;
            MSize size;
            MINT format;
            size_t stride;
            MUINT const usage = eBUFFER_USAGE_SW_READ_OFTEN | eBUFFER_USAGE_HW_CAMERA_READWRITE; //not necessary here
            mpParamsManager->getVideoSize(&recordsize.w, &recordsize.h);

            if ( OK != decideRrzoImage(
                            helper, bitDepth,
                            recordsize, format, usage,
                            vImageParam

                        ))
            {
                MY_LOGE("No rrzo image");
                return BAD_VALUE;
            }
        }
        // YUV preview -> display client
        {
            Vector<MUINT32> clientMode;
            clientMode.push_back(IImgBufProvider::eID_DISPLAY);

            setCamClient(
                "Hal:Image:yuvDisp",
                eSTREAMID_IMAGE_PIPE_YUV_00,
                vImageParam,
                clientMode
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

    FUNC_END;

    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MINT32
EngFlowControl::
getScenario() const
{
    if( mSensorParam.mode == SENSOR_SCENARIO_ID_NORMAL_VIDEO )
    {
        return IScenarioControl::Scenario_VideoRecord;
    }
    else
    {
        return IScenarioControl::Scenario_NormalPreivew;
    }
}
