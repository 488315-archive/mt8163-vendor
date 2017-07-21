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

#define LOG_TAG "MtkCam/StereoFlowControl"
//
#include "MyUtils.h"
//
#include <v3/utils/streaminfo/MetaStreamInfo.h>
#include <v3/utils/streaminfo/ImageStreamInfo.h>
#include <v3/utils/streambuf/StreamBuffers.h>

//
#include <LegacyPipeline/StreamId.h>
#include <LegacyPipeline/NodeId.h>
#include <v1/IParamsManager.h>
#include <LegacyPipeline/ILegacyPipeline.h>
#include <LegacyPipeline/stereo/StereoLegacyPipelineDef.h>
#include <LegacyPipeline/stereo/StereoLegacyPipelineBuilder.h>
#include <LegacyPipeline/buffer/StreamBufferProviderFactory.h>
//
#include <IHalSensor.h>
//
#include <hwsync_drv.h>
//
#include <IHal3A.h>
//
#include "StereoFlowControl.h"
//
#include <vsdof/hal/stereo_size_provider.h>

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

#define FUNC_START     MY_LOGD("+")
#define FUNC_END       MY_LOGD("-")
#define FUNC_NAME      MY_LOGD("")
//
#define CHECK_OBJECT(x, ret)  do{                                        \
    if (x == NULL) { MY_LOGE("Null %s Object", #x); return ret;} \
} while(0)

/******************************************************************************
 *
 ******************************************************************************/
sp<ImageStreamInfo>
createImageStreamInfo(
    char const*         streamName,
    StreamId_T          streamId,
    MUINT32             streamType,
    size_t              maxBufNum,
    size_t              minInitBufNum,
    MUINT               usageForAllocator,
    MINT                imgFormat,
    MSize const&        imgSize,
    MUINT32             transform
)
{
    IImageStreamInfo::BufPlanes_t bufPlanes;
#define addBufPlane(planes, height, stride)                                      \
        do{                                                                      \
            size_t _height = (size_t)(height);                                   \
            size_t _stride = (size_t)(stride);                                   \
            IImageStreamInfo::BufPlane bufPlane= { _height * _stride, _stride }; \
            planes.push_back(bufPlane);                                          \
        }while(0)
    switch( imgFormat ) {
        case eImgFmt_YV12:
            addBufPlane(bufPlanes , imgSize.h      , imgSize.w);
            addBufPlane(bufPlanes , imgSize.h >> 1 , imgSize.w >> 1);
            addBufPlane(bufPlanes , imgSize.h >> 1 , imgSize.w >> 1);
            break;
        case eImgFmt_NV21:
            addBufPlane(bufPlanes , imgSize.h      , imgSize.w);
            addBufPlane(bufPlanes , imgSize.h >> 1 , imgSize.w);
            break;
        case eImgFmt_YUY2:
            addBufPlane(bufPlanes , imgSize.h      , imgSize.w << 1);
            break;
        case eImgFmt_Y8:
            addBufPlane(bufPlanes , imgSize.h      , imgSize.w);
            break;
        default:
            MY_LOGE("format not support yet %p", imgFormat);
            break;
    }
#undef  addBufPlane

    sp<ImageStreamInfo>
        pStreamInfo = new ImageStreamInfo(
                streamName,
                streamId,
                streamType,
                maxBufNum, minInitBufNum,
                usageForAllocator, imgFormat, imgSize, bufPlanes, transform
                );

    if( pStreamInfo == NULL ) {
        MY_LOGE("create ImageStream failed, %s, %#"PRIxPTR,
                streamName, streamId);
    }

    return pStreamInfo;
}

/******************************************************************************
*
*******************************************************************************/
MINT
StereoFlowControl::
getImageStreamIndex(
    ImageStreamManager::image_stream_info_pool_setting* pTable,
    NSCam::v3::StreamId_T id
)
{
    MINT index = 0;
    while(pTable->streamId != 0)
    {
        if(pTable->streamId == id)
        {
            return index;
        }
        pTable++;
        index++;
    }
    return (-1);
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
StereoFlowControl::
updateRrzoInfo(
    MBOOL bMain1,
    ImageStreamManager::image_stream_info_pool_setting* pImgTable,
    MUINT32 mode
)
{
    MINT32 index = getImageStreamIndex(
                    pImgTable,
                    (bMain1?(eSTREAMID_IMAGE_PIPE_RAW_RESIZER):(eSTREAMID_IMAGE_PIPE_RAW_RESIZER_01)));
    if(index == -1)
    {
        MY_LOGE("Stream do not exist in table!! %d", bMain1);
        return BAD_VALUE;
    }
    size_t  stride;
    MSize   imageSize;
    MRect   imageCrop;
    MINT    format = pImgTable[index].imgFormat;

    bool bRet = MTRUE;

    bRet = StereoSizeProvider::getInstance()->getPass1Size(
                        (bMain1)?(StereoHAL::eSTEREO_SENSOR_MAIN1):(StereoHAL::eSTEREO_SENSOR_MAIN2),
                        (EImageFormat)format,
                        NSImageio::NSIspio::EPortIndex_RRZO,
                        (StereoHAL::ENUM_STEREO_SCENARIO)mode,
                        (MRect&)imageCrop,
                        (MSize&)imageSize,
                        (MUINT32&)stride);
    if(!bRet) {
        MY_LOGE("Get Pass1 Size Fail.");
        return BAD_VALUE;
    }

    pImgTable[index].imgSize.w = imageSize.w;
    pImgTable[index].imgSize.h = imageSize.h;
    pImgTable[index].stride = stride;

    MY_LOGD("get size from size provider: updateRrzoInfo isMain1:%d, (%d,%d,%dx%d) (%dx%d), stride:%d",
        bMain1,
        imageCrop.p.x,
        imageCrop.p.y,
        imageCrop.s.w,
        imageCrop.s.h,
        imageSize.w,
        imageSize.h,
        stride
    );

    if(bMain1){
        mSensorDomainCrop = imageCrop;
    }else{
        mSensorDomainCrop_Main2 = imageCrop;
    }

    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
StereoFlowControl::
updateImgoInfo(
    MBOOL bMain1,
    ImageStreamManager::image_stream_info_pool_setting* pImgTable,
    MUINT32 mode
)
{
    MINT32 index = getImageStreamIndex(
                    pImgTable,
                    (bMain1?(eSTREAMID_IMAGE_PIPE_RAW_OPAQUE):(eSTREAMID_IMAGE_PIPE_RAW_OPAQUE_01)));
    if(index == -1)
    {
        MY_LOGD("Stream do not exist in table!! %d", bMain1);
        return BAD_VALUE;
    }
    size_t stride = 0;
    MRect imageCrop;
    MSize imageSize;
    MINT format = pImgTable[index].imgFormat;
    bool bRet = MTRUE;

    bRet = StereoSizeProvider::getInstance()->getPass1Size(
                        (bMain1)?(StereoHAL::eSTEREO_SENSOR_MAIN1):(StereoHAL::eSTEREO_SENSOR_MAIN2),
                        (EImageFormat)format,
                        NSImageio::NSIspio::EPortIndex_IMGO,
                        (StereoHAL::ENUM_STEREO_SCENARIO)mode,
                        (MRect&)imageCrop,
                        (MSize&)imageSize,
                        (MUINT32&)stride);
    if(!bRet) {
        MY_LOGE("Get Pass1 Size Fail.");
        return BAD_VALUE;
    }

    pImgTable[index].imgSize.w = imageSize.w;
    pImgTable[index].imgSize.h = imageSize.h;
    pImgTable[index].stride = stride;

    MY_LOGD("get size from size provider: updateImgoInfo isMain1:%d, %dx%d, stride:%d",
        bMain1,
        imageSize.w,
        imageSize.h,
        stride
    );

    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MUINT32
StereoFlowControl::
prepareSensor(MUINT sensorId, StereoPipelineSensorParam& sensorParam)
{
    FUNC_START;

    IHalSensorList* const pHalSensorList = IHalSensorList::get();
    IHalSensor* pSensorHalObj = pHalSensorList->createSensor(LOG_TAG, sensorId);
    if( ! pSensorHalObj ) {
        MY_LOGE("create sensor failed");
        return -1;
    }

    MUINT32 sensorDev = pHalSensorList->querySensorDevIdx(sensorId);
    SensorStaticInfo sensorStaticInfo;
    memset(&sensorStaticInfo, 0, sizeof(SensorStaticInfo));
    pHalSensorList->querySensorStaticInfo(sensorDev, &sensorStaticInfo);
    //
    switch(mPipelineMode)
    {
    case PipelineMode_PREVIEW:
        sensorParam.mode = NSCam::SENSOR_SCENARIO_ID_NORMAL_PREVIEW;
        sensorParam.size = MSize(sensorStaticInfo.previewWidth, sensorStaticInfo.previewHeight);
        break;
    case PipelineMode_ZSD:
        //For main2, ZSD mode use PREVIEW scenario
        if(getOpenId_Main2() == sensorId)
        {
            sensorParam.mode = NSCam::SENSOR_SCENARIO_ID_NORMAL_PREVIEW;
            sensorParam.size = MSize(sensorStaticInfo.previewWidth, sensorStaticInfo.previewHeight);
        }
        else{
            sensorParam.mode = NSCam::SENSOR_SCENARIO_ID_NORMAL_CAPTURE;
            sensorParam.size = MSize(sensorStaticInfo.captureWidth, sensorStaticInfo.captureHeight);
        }
        break;
    case PipelineMode_RECORDING:
        sensorParam.mode = NSCam::SENSOR_SCENARIO_ID_NORMAL_VIDEO;
        sensorParam.size = MSize(sensorStaticInfo.videoWidth, sensorStaticInfo.videoHeight);
        break;
    default:
        MY_LOGE("unrecognized mPipelineMode=%d", mPipelineMode);
    }
    pSensorHalObj->sendCommand(
                        pHalSensorList->querySensorDevIdx(sensorId),
                        SENSOR_CMD_GET_DEFAULT_FRAME_RATE_BY_SCENARIO,
                        (MINTPTR)(&sensorParam.mode),
                        (MINTPTR)(&sensorParam.fps),//get fps here
                        0);
    //
    sensorParam.fps /= 10;
    sensorParam.pixelMode = 0;
    pSensorHalObj->sendCommand(
            pHalSensorList->querySensorDevIdx(sensorId),
            SENSOR_CMD_GET_SENSOR_PIXELMODE,
            (MUINTPTR)(&sensorParam.mode),
            (MUINTPTR)(&sensorParam.fps),
            (MUINTPTR)(&sensorParam.pixelMode));
    //
    MY_LOGD("sensor(%d), sensorDev(%d), mode %d, size %dx%d, fps %d, pixelmode %d",
            sensorId,
            sensorDev,
            sensorParam.mode,
            sensorParam.size.w, sensorParam.size.h,
            sensorParam.fps,
            sensorParam.pixelMode);
    FUNC_END;
    return sensorDev;
}

/******************************************************************************
 * create p1 pipeline for main1 sensor
 ******************************************************************************/
sp<ILegacyPipeline>
StereoFlowControl::
constructP1Pipeline_Main1(
    const MetaStreamManager::metadata_info_setting* metaTable,
    const MINT32* nodeConfigData,
    ImageStreamManager::image_stream_info_pool_setting* imageTable
)
{
    FUNC_NAME;
    sp<StereoLegacyPipelineBuilder> pBuilder = new StereoLegacyPipelineBuilder(
                                                        getOpenId(),
                                                        "StereoP1",
                                                        StereoLegacyPipelineMode_T::STPipelineMode_P1);
    CHECK_OBJECT(pBuilder, NULL);

    // **********************
    // 0. Sensor Information & HWSync setup
    // **********************
    {
        MUINT32 sensorDev = prepareSensor(getOpenId(), mSensorParam);
        pBuilder->setSrc(mSensorParam);

        MUINT32 dvfsLevel = (mbDVFSLevel) ? 3 : 0;
        MUINT32 aeInitShutter = 33000;

        {
            NS3Av3::IHal3A* mp3A = NS3Av3::IHal3A::createInstance(NS3Av3::IHal3A::E_Camera_3, getOpenId(),"stereo_construct_p1");

            if(mp3A == NULL){
                MY_LOGE("create Hal3A failed!");
                return NULL;
            }

            mp3A->setSensorMode(mSensorParam.mode);
            mp3A->send3ACtrl(NS3Av3::E3ACtrl_GetInitExposureTime, reinterpret_cast<MINTPTR>(&aeInitShutter), NULL);

            mp3A->destroyInstance("stereo_construct_p1");
        }

        MY_LOGD("setup hwsync driver + , openId(%d), sensorDev(%d), dvfsLevel(%d), aeInitShutter(%d)",
            getOpenId(),
            sensorDev,
            dvfsLevel,
            aeInitShutter
        );

        {
            HWSyncDrv* pHwSync = HWSyncDrv::createInstance();
            if(pHwSync == NULL){
                MY_LOGE("create HWSyncDrv failed!");
                return NULL;
            }
            MUINT32 ret = pHwSync->sendCommand(
                                HW_SYNC_CMD_ENUM::HW_SYNC_CMD_SET_PARA,
                                sensorDev,
                                dvfsLevel,      // mmdvfs level
                                aeInitShutter   // init shutter time
            );
            pHwSync->destroyInstance();
            MY_LOGD("setup hwsync driver - , ret=%d", ret);
        }
    }
    // **********************
    // 1. ImageStreamInfo Preparation
    // **********************
    {
        MUINT32 stereoMode = StereoHAL::eSTEREO_SCENARIO_PREVIEW;
        switch(mPipelineMode)
        {
        default:                        MY_LOGE("unknown pipeline mode(%d)!!", mPipelineMode);  break;
        case PipelineMode_PREVIEW:      stereoMode = StereoHAL::eSTEREO_SCENARIO_PREVIEW;       break;
        case PipelineMode_ZSD:          stereoMode = StereoHAL::eSTEREO_SCENARIO_CAPTURE;       break;
        case PipelineMode_RECORDING:    stereoMode = StereoHAL::eSTEREO_SCENARIO_RECORD;       break;
        }
        //
        if(OK == updateRrzoInfo(MTRUE, imageTable, stereoMode))
        {
            updateImgoInfo(MTRUE, imageTable, stereoMode);
        }else {
            MY_LOGE("RRZO Info error!!");
            return NULL;
        }
    }
    // **********************
    // 2. Build Context
    // **********************
    sp<MetaStreamManager> spMetaStrmManager = MetaStreamManager::create(metaTable);
    CHECK_OBJECT(spMetaStrmManager, NULL);
    //
    // Shane Test
    sp<IMetaStreamInfo> appMetaControlInfo = spMetaStrmManager->getStreamInfoByStreamId(eSTREAMID_META_APP_CONTROL);
    if(appMetaControlInfo->getStreamType() == eSTREAMTYPE_META_INOUT){
        MY_LOGD("eSTREAMID_META_APP_CONTROL is eSTREAMTYPE_META_INOUT");
    }
    if(appMetaControlInfo->getStreamType() == eSTREAMTYPE_META_IN){
        MY_LOGD("eSTREAMID_META_APP_CONTROL is eSTREAMTYPE_META_IN");
    }

    //
    sp<ImageStreamManager> spImgStrmManager = ImageStreamManager::create(imageTable);
    CHECK_OBJECT(spImgStrmManager, NULL);
    //
    const NodeConfigDataManager::NodeConfigMgrSetting cfgSetting =
                                        {
                                            getOpenId(),
                                            getOpenId_Main2(),
                                            nodeConfigData
                                        };
    sp<NodeConfigDataManager> spNodeConfigManager = NodeConfigDataManager::create("StereoP1", &cfgSetting);
    CHECK_OBJECT(spNodeConfigManager, NULL);
    //
    Vector<StreamId_T> vMetaStreamInfoSet;
    vMetaStreamInfoSet.add(eSTREAMID_META_APP_DYNAMIC_P1);
    vMetaStreamInfoSet.add(eSTREAMID_META_HAL_DYNAMIC_P1);
    //
    Vector< sp<PairMetadata> > vPairMetaSet;
    for(MUINT32 i=0; i<spImgStrmManager->getSize(); i++)
    {
        sp<IImageStreamInfo> spStrmInfo = spImgStrmManager->getStreamInfoByIndex(i);
        if(spImgStrmManager->getPoolTypeByIndex(i) == NSPipelineContext::eStreamType_IMG_HAL_PROVIDER)
        {
            // create pairMetadata
            sp<PairMetadata> pPair = PairMetadata::createInstance(spStrmInfo->getStreamName());
            vPairMetaSet.add(pPair);

            // create image buffer provider
            sp<StreamBufferProviderFactory> pFactory = StreamBufferProviderFactory::createInstance();
            pFactory->setImageStreamInfo(spStrmInfo);
            pFactory->setPairRule(pPair, vMetaStreamInfoSet.size());
            sp<StreamBufferProvider> pProducer = pFactory->create(getOpenId(), MTRUE);

            spImgStrmManager->updateBufProvider(
                                        spStrmInfo->getStreamId(),
                                        pProducer,
                                        vMetaStreamInfoSet);
        }
        MY_LOGD("idx(%d) StreamId(%d), Name(%s) BufferType(%d), BufProvider(0x%X)",
                    i,
                    spStrmInfo->getStreamId(),
                    spStrmInfo->getStreamName(),
                    spImgStrmManager->getPoolTypeByIndex(i),
                    spImgStrmManager->getBufProviderByIndex(i).get()
                    );
    }
    if(vPairMetaSet.size() != spImgStrmManager->getSize()){
        MY_LOGE("the number of PairMetadata and imageStreams does not match!");
        return NULL;
    }

    // **********************
    // 3. create LegacyPipeline
    // **********************
    pBuilder->setMetaStreamId(
                eSTREAMID_META_HAL_CONTROL,
                eSTREAMID_META_APP_CONTROL);

    sp<ILegacyPipeline> pPipeline = pBuilder->create(spMetaStrmManager, spImgStrmManager,spNodeConfigManager);
    if(pPipeline == NULL){
        MY_LOGE("pBuilder->create P1Pipeline_Main1 failed");
        return NULL;
    }

    // **********************
    // 4. bind listeners for PairMetadata
    // **********************
    sp<ResultProcessor> pProcessor = pPipeline->getResultProcessor().promote();

    for(MUINT32 metaIdx=0; metaIdx < vMetaStreamInfoSet.size(); metaIdx++){
        for(MUINT32 pairIdx=0; pairIdx < vPairMetaSet.size(); pairIdx++){
            MY_LOGD("register metaStream:%d to pair:%p",vMetaStreamInfoSet[metaIdx], vPairMetaSet[pairIdx].get());
            pProcessor->registerListener(
                    vMetaStreamInfoSet[metaIdx],
                    vPairMetaSet[pairIdx]
            );
        }
    }

    return pPipeline;
};
/******************************************************************************
 * create p1 pipeline for main2 sensor
 ******************************************************************************/
sp<ILegacyPipeline>
StereoFlowControl::
constructP1Pipeline_Main2(
    const MetaStreamManager::metadata_info_setting* metaTable,
    const MINT32* nodeConfigData,
    ImageStreamManager::image_stream_info_pool_setting* imageTable
)
{
    FUNC_NAME;
    sp<StereoLegacyPipelineBuilder> pBuilder = new StereoLegacyPipelineBuilder(
                                                        getOpenId_Main2(),
                                                        "StereoP1Main2",
                                                        StereoLegacyPipelineMode_T::STPipelineMode_P1Main2);
    CHECK_OBJECT(pBuilder, NULL);

    // **********************
    // 1. Sensor Information & HWSync setup
    // **********************
    {
        MUINT32 sensorDev = prepareSensor(getOpenId_Main2(), mSensorParam_main2);
        pBuilder->setSrc(mSensorParam_main2);

        MUINT32 dvfsLevel = (mbDVFSLevel) ? 3 : 0;
        MUINT32 aeInitShutter = 33000;

        {
            NS3Av3::IHal3A* mp3A = NS3Av3::IHal3A::createInstance(NS3Av3::IHal3A::E_Camera_3, getOpenId_Main2(),"stereo_construct_p1_main2");

            if(mp3A == NULL){
                MY_LOGE("create Hal3A failed!");
                return NULL;
            }

            mp3A->setSensorMode(mSensorParam_main2.mode);
            mp3A->send3ACtrl(NS3Av3::E3ACtrl_GetInitExposureTime, reinterpret_cast<MINTPTR>(&aeInitShutter), NULL);

            mp3A->destroyInstance("stereo_construct_p1_main2");
        }

        MY_LOGD("setup hwsync driver + , openId(%d), sensorDev(%d), dvfsLevel(%d), aeInitShutter(%d)",
            getOpenId_Main2(),
            sensorDev,
            dvfsLevel,
            aeInitShutter
        );

        {
            HWSyncDrv* pHwSync = HWSyncDrv::createInstance();
            if(pHwSync == NULL){
                MY_LOGE("create HWSyncDrv failed!");
                return NULL;
            }
            MUINT32 ret = pHwSync->sendCommand(
                                HW_SYNC_CMD_ENUM::HW_SYNC_CMD_SET_PARA,
                                sensorDev,
                                dvfsLevel,      // mmdvfs level
                                aeInitShutter   // init shutter time
            );
            pHwSync->destroyInstance();
            MY_LOGD("setup hwsync driver - , ret=%d", ret);
        }
    }
    // **********************
    // 2. ImageStreamInfo Preparation
    // **********************
    {
        MUINT32 stereoMode = StereoHAL::eSTEREO_SCENARIO_PREVIEW;
        switch(mPipelineMode)
        {
        default:                        MY_LOGE("unknown pipeline mode(%d)!!", mPipelineMode);  break;
        case PipelineMode_PREVIEW:      stereoMode = StereoHAL::eSTEREO_SCENARIO_PREVIEW;       break;
        case PipelineMode_ZSD:          stereoMode = StereoHAL::eSTEREO_SCENARIO_PREVIEW;       break;
        case PipelineMode_RECORDING:    stereoMode = StereoHAL::eSTEREO_SCENARIO_RECORD;        break;
        }
        //
        if(OK == updateRrzoInfo(MFALSE, imageTable, stereoMode))
        {
            // TODO: tmp solution
            //updateImgoInfo(MFALSE, imageTable, stereoMode);
        }else {
            MY_LOGE("RRZO Info error!!");
            return NULL;
        }
    }

    // **********************
    // 3. Build Context
    // **********************
    sp<MetaStreamManager> spMetaStrmManager = MetaStreamManager::create(metaTable);
    CHECK_OBJECT(spMetaStrmManager, NULL);
    //
    sp<ImageStreamManager> spImgStrmManager = ImageStreamManager::create(imageTable);
    CHECK_OBJECT(spImgStrmManager, NULL);
    //
    const NodeConfigDataManager::NodeConfigMgrSetting cfgSetting =
                                        {
                                            getOpenId(),
                                            getOpenId_Main2(),
                                            nodeConfigData
                                        };
    sp<NodeConfigDataManager> spNodeConfigManager = NodeConfigDataManager::create("StereoP1Main2", &cfgSetting);
    CHECK_OBJECT(spNodeConfigManager, NULL);
    //
    Vector<StreamId_T> vMetaStreamInfoSet;
    vMetaStreamInfoSet.add(eSTREAMID_META_APP_DYNAMIC_P1_MAIN2);
    vMetaStreamInfoSet.add(eSTREAMID_META_HAL_DYNAMIC_P1_MAIN2);
    //
    Vector< sp<PairMetadata> > vPairMetaSet;
    for(MUINT32 i=0; i<spImgStrmManager->getSize(); i++)
    {
        sp<IImageStreamInfo> spStrmInfo = spImgStrmManager->getStreamInfoByIndex(i);
        if(spImgStrmManager->getPoolTypeByIndex(i) == NSPipelineContext::eStreamType_IMG_HAL_PROVIDER)
        {
            // create pairMetadata
            sp<PairMetadata> pPair = PairMetadata::createInstance(spStrmInfo->getStreamName());
            vPairMetaSet.add(pPair);

            // create image buffer provider
            sp<StreamBufferProviderFactory> pFactory = StreamBufferProviderFactory::createInstance();
            pFactory->setImageStreamInfo(spStrmInfo);
            pFactory->setPairRule(pPair, vMetaStreamInfoSet.size());
            sp<StreamBufferProvider> pProducer = pFactory->create(getOpenId_Main2(), MTRUE);

            spImgStrmManager->updateBufProvider(
                                        spStrmInfo->getStreamId(),
                                        pProducer,
                                        vMetaStreamInfoSet);
        }
        MY_LOGD("idx(%d) StreamId(%d), Name(%s) BufferType(%d), BufProvider(0x%X)",
                    i,
                    spStrmInfo->getStreamId(),
                    spStrmInfo->getStreamName(),
                    spImgStrmManager->getPoolTypeByIndex(i),
                    spImgStrmManager->getBufProviderByIndex(i).get()
                    );
    }
    if(vPairMetaSet.size() != spImgStrmManager->getSize()){
        MY_LOGE("the number of PairMetadata and imageStreams does not match!");
        return NULL;
    }
    // **********************
    // 4. create LegacyPipeline
    // **********************
    pBuilder->setMetaStreamId(
                eSTREAMID_META_HAL_CONTROL_MAIN2,
                eSTREAMID_META_APP_CONTROL_MAIN2);

    sp<ILegacyPipeline> pPipeline = pBuilder->create(spMetaStrmManager, spImgStrmManager,spNodeConfigManager);
    if(pPipeline == NULL){
        MY_LOGE("pBuilder->create P1Pipeline_Main2 failed");
        return NULL;
    }

    // **********************
    // 4. bind listeners for PairMetadata
    // **********************
    sp<ResultProcessor> pProcessor = pPipeline->getResultProcessor().promote();

    for(MUINT32 metaIdx=0; metaIdx < vMetaStreamInfoSet.size(); metaIdx++){
        for(MUINT32 pairIdx=0; pairIdx < vPairMetaSet.size(); pairIdx++){
            MY_LOGD("register metaStream:%d to pair:%p",vMetaStreamInfoSet[metaIdx], vPairMetaSet[pairIdx].get());
            pProcessor->registerListener(
                    vMetaStreamInfoSet[metaIdx],
                    vPairMetaSet[pairIdx]
            );
        }
    }

    return pPipeline;
}
