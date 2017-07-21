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
#include "../MyUtils.h"
//
#include <v3/utils/streaminfo/MetaStreamInfo.h>
#include <v3/utils/streaminfo/ImageStreamInfo.h>
#include <v3/utils/streambuf/StreamBuffers.h>

#include <mtk_platform_metadata_tag.h>
#include <metadata/client/mtk_metadata_tag.h>

#include <v1/camutils/CamInfo.h>
#include <v1/IParamsManager.h>
#include <LegacyPipeline/request/IRequestController.h>
#include <LegacyPipeline/ILegacyPipeline.h>

#include <feature/include/common/vsdof/hal/stereo_setting_provider.h>

#include <camera/MtkCamera.h>

#include "StereoFlowControl.h"

#include <vsdof/hal/stereo_size_provider.h>
using namespace NSCam;
using namespace NSCam::v1;
using namespace NSCam::v1::NSLegacyPipeline;
using namespace android;
using namespace NSCam::v3;

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

// temp set this level to be 0
#define MY_LOGD1(...)               MY_LOGD_IF(0<=mLogLevel, __VA_ARGS__)

#define FUNC_START     MY_LOGD("+")
#define FUNC_END       MY_LOGD("-")
/******************************************************************************
 *
 ******************************************************************************/
template <typename T>
inline MBOOL
tryGetMetadata(
    IMetadata* pMetadata,
    MUINT32 const tag,
    T & rVal
)
{
    if( pMetadata == NULL ) {
        MY_LOGW("pMetadata == NULL");
        return MFALSE;
    }

    IMetadata::IEntry entry = pMetadata->entryFor(tag);
    if( !entry.isEmpty() ) {
        rVal = entry.itemAt(0, Type2Type<T>());
        return MTRUE;
    }
    return MFALSE;
}
/******************************************************************************
 *
 ******************************************************************************/
StereoFlowControl::
StereoFlowControl(
    char const*                 pcszName,
    MINT32 const                i4OpenId,
    sp<IParamsManager>          pParamsManager,
    sp<ImgBufProvidersManager>  pImgBufProvidersManager,
    sp<INotifyCallback>         pCamMsgCbInfo
)
    : mpParamsManager(pParamsManager)
    , mpImgBufProvidersMgr(pImgBufProvidersManager)
    , mName(const_cast<char*>(pcszName))
    , mpCamMsgCbInfo(pCamMsgCbInfo)
    , mpStereoRequestUpdater(NULL)
    , mpStereoRequestUpdater_Main2(NULL)
    , mpStereoSelector_RESIZER(NULL)
    , mpStereoSelector_RESIZER_MAIN2(NULL)
    , mpStereoSelector_OPAQUE(NULL)
    , mpStereoSelector_OPAQUE_MAIN2(NULL)
    , mpStereoBufferSynchronizer(NULL)
    , mpStereoBufferPool_RESIZER(NULL)
    , mpStereoBufferPool_RESIZER_MAIN2(NULL)
    , mpStereoBufferPool_OPAQUE(NULL)
    , mpStereoBufferPool_OPAQUE_MAIN2(NULL)
    , mLogLevel(0)
    , mPipelineMode(PipelineMode_PREVIEW) // use preview as default
    , mbEnableP2Prv(MTRUE)
    , mBWCtype(BWCPT_CAMERA_ZSD)
    , mOpenId_P2Prv(28285) // magic number for pipelineP2Prv
    , mActiveArrayCrop(MPoint(0,0),MSize(0,0))
    , mActiveArrayCrop_Main2(MPoint(0,0),MSize(0,0))
    , mSensorDomainCrop(MPoint(0,0),MSize(0,0))
    , mSensorDomainCrop_Main2(MPoint(0,0),MSize(0,0))
{
    mpParamsManagerV3 = IParamsManagerV3::createInstance(
                                    String8::format(pcszName),
                                    i4OpenId,
                                    pParamsManager
                                );
    int32_t main1Id = -1, main2Id = -1;
    if(StereoSettingProvider::getStereoSensorIndex(main1Id, main2Id)){
        mOpenId = main1Id;
        mOpenId_main2 = main2Id;
    }else{
        MY_LOGE("Cannot get sensor ids from StereoSettingProvider!");
        return;
    }

    if(getOpenId() != i4OpenId){
        MY_LOGE("mOpenId(%d) != i4OpenId(%d), should not have happened!", getOpenId(), i4OpenId);
        return;
    }

    mpResourceContainier = IResourceContainer::getInstance(getOpenId());
    mpResourceContainierMain2 = IResourceContainer::getInstance(getOpenId_Main2());
    mpResourceContainierP2Prv = IResourceContainer::getInstance(getOpenId_P2Prv()); // Use this magic number to create consumer container for P2 Prv

    char cProperty[PROPERTY_VALUE_MAX];
    ::property_get("debug.camera.log", cProperty, "0");
    mLogLevel = ::atoi(cProperty);
    if ( 0 == mLogLevel ) {
        ::property_get("debug.camera.log.stereoflow", cProperty, "0");
        mLogLevel = ::atoi(cProperty);
    }

    MY_LOGD("StereoFlowControl => mOpenId(%d), mOpenId_main2(%d), mOpenId_P2Prv(%d), mLogLevel(%d)",
        getOpenId(),
        getOpenId_Main2(),
        getOpenId_P2Prv(),
        mLogLevel
    );
}

/******************************************************************************
 *
 ******************************************************************************/
char const*
StereoFlowControl::
getName()   const
{
    return mName;
}

/******************************************************************************
 *
 ******************************************************************************/
int32_t
StereoFlowControl::
getOpenId() const
{
    return mOpenId;
}

/******************************************************************************
 *
 ******************************************************************************/
int32_t
StereoFlowControl::
getOpenId_Main2() const
{
    return mOpenId_main2;
}

/******************************************************************************
 *
 ******************************************************************************/
int32_t
StereoFlowControl::
getOpenId_P2Prv() const
{
    return mOpenId_P2Prv;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
StereoFlowControl::
startPreview()
{
    // todo:
    // * getRecordHint

    mPipelineMode = PipelineMode_ZSD;
    MY_LOGD("mPipelineMode=%d", mPipelineMode);

    // BWC
    {
        MY_LOGD("BWC enter scenario:%d +", mBWCtype);
        MY_LOGD("enable stereo mode flag for mmdvfs");
        // enable stereo mode flag
        mmdvfs_set(
            mBWCtype,
            MMDVFS_CAMERA_MODE_STEREO, 1,
            MMDVFS_PARAMETER_EOF
        );

        // create BWC and enter scenario
        BWC BwcIns;
        BwcIns.Profile_Change(mBWCtype,true);

        // Query mmdvfs level
        mmdvfs_step_enum step;
        mbDVFSLevel = MFALSE;
        step = mmdvfs_query(
                            mBWCtype,
                            MMDVFS_CAMERA_MODE_STEREO, mbDVFSLevel,
                            MMDVFS_PARAMETER_EOF
                            );
        if(step == mmdvfs_step_enum::MMDVFS_STEP_LOW){
            mbDVFSLevel = MFALSE;
        }else
        if(step == mmdvfs_step_enum::MMDVFS_STEP_HIGH ||
           step == mmdvfs_step_enum::MMDVFS_STEP_HIGH2HIGH
        ){
            mbDVFSLevel = MTRUE;
        }else{
            MY_LOGD("unknown dvfs step value:%d", step);
        }

        MY_LOGD("Current DVFS level is %d", mbDVFSLevel);
        MY_LOGD("BWC enter scenario:%d -", mBWCtype);
    }

    // get pass1 active array crop
    {
        if(!StereoSizeProvider::getInstance()->getPass1ActiveArrayCrop(StereoHAL::eSTEREO_SENSOR_MAIN1, mActiveArrayCrop)){
            MY_LOGE("can't get active arrayCrop from StereoSizeProvider for eSTEREO_SENSOR_MAIN1");
            return BAD_VALUE;
        }
        if(!StereoSizeProvider::getInstance()->getPass1ActiveArrayCrop(StereoHAL::eSTEREO_SENSOR_MAIN2, mActiveArrayCrop_Main2)){
            MY_LOGE("can't get active arrayCrop from StereoSizeProvider for eSTEREO_SENSOR_MAIN2");
            return BAD_VALUE;
        }

        MY_LOGD("StereoSizeProvider => active array crop main1(%d,%d,%dx%d), main2(%d,%d,%dx%d)",
            mActiveArrayCrop.p.x,
            mActiveArrayCrop.p.y,
            mActiveArrayCrop.s.w,
            mActiveArrayCrop.s.h,
            mActiveArrayCrop_Main2.p.x,
            mActiveArrayCrop_Main2.p.y,
            mActiveArrayCrop_Main2.s.w,
            mActiveArrayCrop_Main2.s.h
        );
    }



    // create request controllers
    MY_LOGD("create request controllers");
    mpRequestController_P1 = IRequestController::createInstance(
                                                "mpRequestController_P1",
                                                getOpenId(),
                                                mpCamMsgCbInfo,
                                                mpParamsManagerV3
                                            );
    mpRequestController_P1_Main2 = IRequestController::createInstance(
                                            "mpRequestController_P1_Main2",
                                            getOpenId_Main2(),
                                            mpCamMsgCbInfo,
                                            mpParamsManagerV3
                                        );
    mpRequestController = IRequestController::createInstance(
                                            "mpRequestController",
                                            getOpenId(),
                                            mpCamMsgCbInfo,
                                            mpParamsManagerV3
                                        );
    MY_LOGD("mpRequestController_P1=%p", mpRequestController_P1.get());
    MY_LOGD("mpRequestController_P1_Main2=%p", mpRequestController_P1_Main2.get());
    MY_LOGD("mpRequestController=%p", mpRequestController.get());

    // create stereoSynchronizer
    MY_LOGD("create stereoSynchronizer");
    mpStereoBufferSynchronizer = StereoBufferSynchronizer::createInstance("Synchronizer");
#warning "fixme enable synchronizer debug mode"
    MY_LOGW("create stereoSynchronizer with debug mode DEBUG_MODE_SKIP_TIMESTAMP_CHECK");
    mpStereoBufferSynchronizer->setDebugMode(StereoBufferSynchronizer::DEBUG_MODE::DEBUG_MODE_SKIP_TIMESTAMP_CHECK);

    // create StereoSelectors
    MY_LOGD("create StereoSelectors");
    mpStereoSelector_RESIZER      = new StereoSelector("RESIZER");
    mpStereoSelector_RESIZER_MAIN2   = new StereoSelector("RESIZER_01");
    mpStereoSelector_OPAQUE       = new StereoSelector("OPAQUE");

    // bind stream/selector pair to synchronizer
    MY_LOGD("bind stream/selector pair to synchronizer");
    mpStereoBufferSynchronizer->addStream(eSTREAMID_IMAGE_PIPE_RAW_RESIZER,     mpStereoSelector_RESIZER);
    mpStereoBufferSynchronizer->addStream(eSTREAMID_IMAGE_PIPE_RAW_RESIZER_01,  mpStereoSelector_RESIZER_MAIN2);
    mpStereoBufferSynchronizer->addStream(eSTREAMID_IMAGE_PIPE_RAW_OPAQUE,      mpStereoSelector_OPAQUE);

    // start synchronizer
    MY_LOGD("start synchronizer");
    mpStereoBufferSynchronizer->start();

    // create pipelines
    MY_LOGD("create pipelines");
    MINT32  const nodeConfigData_P1[] = {eNODEID_P1Node, -1, -1};
    MINT32  const nodeConfigData_P1_Main2[] = {eNODEID_P1Node_main2, -1, -1};
    MINT32  const nodeConfigData_P2[] = {eNODEID_DepthMapNode, eNODEID_BokehNode, -1, -1};

    mpPipeline_P1 = constructP1Pipeline_Main1(
                        StereoPipelineMgrData::p1::zsd::gStereoMetaTbl_ZsdP1,
                        nodeConfigData_P1,
                        StereoPipelineMgrData::p1::zsd::gStereoImgStreamTbl_ZsdP1
                );
    mpPipeline_P1_Main2 = constructP1Pipeline_Main2(
                        StereoPipelineMgrData::p1::zsd::gStereoMetaTbl_ZsdP1Main2,
                        nodeConfigData_P1_Main2,
                        StereoPipelineMgrData::p1::zsd::gStereoImgStreamTbl_ZsdP1Main2
                );
    mpPipeline = constructP2Pipeline_Prv(
                        StereoPipelineMgrData::p2::prv::gStereoMetaTbl_P2Prv,
                        nodeConfigData_P2,
                        StereoPipelineMgrData::p2::prv::gStereoImgStreamTbl_P2Prv
                 );

    if ( mpPipeline_P1 == 0 || mpPipeline_P1_Main2 == 0 || mpPipeline == 0) {
        MY_LOGE("Cannot get pipeline. start preview fail.");
        MY_LOGE_IF(mpPipeline_P1 == 0, "mpPipeline_P1 == 0");
        MY_LOGE_IF(mpPipeline_P1_Main2 == 0, "mpPipeline_P1_Main2 == 0");
        MY_LOGE_IF(mpPipeline == 0, "mpPipeline == 0");
        return BAD_VALUE;
    }

    // query consumers
    MY_LOGD("query consumers");
    sp<StreamBufferProvider> provider_resizer       = mpResourceContainier->queryConsumer(eSTREAMID_IMAGE_PIPE_RAW_RESIZER);
    sp<StreamBufferProvider> provider_resizer_01    = mpResourceContainierMain2->queryConsumer(eSTREAMID_IMAGE_PIPE_RAW_RESIZER_01);
    sp<StreamBufferProvider> provider_opaque        = mpResourceContainier->queryConsumer(eSTREAMID_IMAGE_PIPE_RAW_OPAQUE);

    if( provider_resizer == NULL || provider_resizer_01 == NULL || provider_opaque == NULL)
    {
        MY_LOGE("Cannot get consumer. start preview fail.");
        MY_LOGE_IF(provider_resizer == NULL, "provider_resizer is NULL");
        MY_LOGE_IF(provider_resizer_01 == NULL, "provider_resizer_01 is NULL");
        MY_LOGE_IF(provider_opaque == NULL, "provider_opaque is NULL");
        return BAD_VALUE;
    }

    // bind selectors to consumers
    MY_LOGD("binding selectors to consumers");
    provider_resizer->setSelector(mpStereoSelector_RESIZER);
    provider_resizer_01->setSelector(mpStereoSelector_RESIZER_MAIN2);
    provider_opaque->setSelector(mpStereoSelector_OPAQUE);

    // create stereoRequestUpdater
    MY_LOGD("create stereoRequestUpdater");
    mpStereoRequestUpdater = new StereoRequestUpdater(this, mpParamsManager, mpStereoBufferSynchronizer);
    mpStereoRequestUpdater_Main2 = new StereoMain2RequestUpdater(this, mpParamsManager, mpStereoBufferSynchronizer);

    mpStereoRequestUpdater->addSelector(mpStereoSelector_RESIZER,       StereoRequestUpdater::StreamType::RESIZED);
    mpStereoRequestUpdater->addSelector(mpStereoSelector_RESIZER_MAIN2, StereoRequestUpdater::StreamType::RESIZED_MAIN2);
    mpStereoRequestUpdater->addSelector(mpStereoSelector_OPAQUE,        StereoRequestUpdater::StreamType::FULL);

    mpStereoRequestUpdater->addPool(mpStereoBufferPool_RESIZER,         StereoRequestUpdater::StreamType::RESIZED);
    mpStereoRequestUpdater->addPool(mpStereoBufferPool_RESIZER_MAIN2,   StereoRequestUpdater::StreamType::RESIZED_MAIN2);
    mpStereoRequestUpdater->addPool(mpStereoBufferPool_OPAQUE,          StereoRequestUpdater::StreamType::FULL);

    mpStereoRequestUpdater->setSensorParams(mSensorParam, mSensorParam_main2);
    mpStereoRequestUpdater_Main2->setSensorParams(mSensorParam, mSensorParam_main2);

#warning "fixme start & end request number"

    MY_LOGD("start pipelines");
    MERROR startPipelineResult = OK;
    {
        MERROR ret = mpRequestController_P1->startPipeline(
                                    0,/*start*/
                                    1000, /*end*/
                                    mpPipeline_P1,
                                    this
                                );
        if(ret != OK){
            MY_LOGE("startPipeline mpRequestController_P1 failed!");
            startPipelineResult = UNKNOWN_ERROR;
        }
    }
    {
        MERROR ret = mpRequestController_P1_Main2->startPipeline(
                                    0,/*start*/
                                    1000, /*end*/
                                    mpPipeline_P1_Main2,
                                    mpStereoRequestUpdater_Main2
                                );
        if(ret != OK){
            MY_LOGE("startPipeline mpRequestController_P1_Main2 failed!");
            startPipelineResult = UNKNOWN_ERROR;
        }
    }
    {
        MERROR ret = mpRequestController->startPipeline(
                                    0,/*start*/
                                    1000, /*end*/
                                    mpPipeline,
                                    mpStereoRequestUpdater
                                );
        if(ret != OK){
            MY_LOGE("startPipeline mpRequestController failed!");
            startPipelineResult = UNKNOWN_ERROR;
        }
    }

    return startPipelineResult;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
StereoFlowControl::
stopPreview()
{
    // CAUTION:
    // The following steps must be processed in this order
    // DO NOT modify it if you are not clearly understanding what you are doing

    // stop P2's pipeline
    mpRequestController->stopPipeline();
    mpRequestController = NULL;

    // stop P1's pipeline
    mpRequestController_P1->stopPipeline();
    mpRequestController_P1 = NULL;

    // stop P1_main2's pipeline
    mpRequestController_P1_Main2->stopPipeline();
    mpRequestController_P1_Main2 = NULL;

    {
        if ( mpPipeline != 0 ){
            mpPipeline->flush();
            mpPipeline->waitUntilDrained();
        }
        if ( mpPipeline_P1 != 0 ){
            mpPipeline_P1->flush();
            mpPipeline_P1->waitUntilDrained();
        }
        if ( mpPipeline_P1_Main2 != 0 ){
            mpPipeline_P1_Main2->flush();
            mpPipeline_P1_Main2->waitUntilDrained();
        }

        mpStereoBufferSynchronizer->flush();

        if ( mpPipeline != 0 ){
            mpPipeline = NULL;
            // no one will use these buffer pool anymore
            mpStereoBufferPool_RESIZER = NULL;
            mpStereoBufferPool_RESIZER_MAIN2 = NULL;
            mpStereoBufferPool_OPAQUE = NULL;
            mpStereoBufferPool_OPAQUE_MAIN2 = NULL;

            mpStereoRequestUpdater = NULL;
        }
        if ( mpPipeline_P1 != 0 ){
            mpPipeline_P1 = NULL;

            mpStereoSelector_RESIZER = NULL;
            mpStereoSelector_OPAQUE = NULL;
        }
        if ( mpPipeline_P1_Main2 != 0 ){
            mpPipeline_P1_Main2 = NULL;

            mpStereoSelector_RESIZER_MAIN2 = NULL;
            mpStereoSelector_OPAQUE_MAIN2 = NULL;

            mpStereoRequestUpdater_Main2 = NULL;
        }
    }


    {
        // disable stereo mode flag
        MY_LOGD("disable stereo mode flag for mmdvfs");
        mmdvfs_set(
            mBWCtype,
            MMDVFS_CAMERA_MODE_STEREO, 0,
            MMDVFS_PARAMETER_EOF
        );

        BWC BwcIns;
        BwcIns.Profile_Change(mBWCtype,false);
    }

    return OK;

}
/******************************************************************************
 *
 ******************************************************************************/
status_t
StereoFlowControl::
suspendPreview()
{
    //TODO
    return UNKNOWN_ERROR;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
StereoFlowControl::
resumePreview()
{
    //TODO
    return UNKNOWN_ERROR;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
StereoFlowControl::
startRecording()
{
    if ( mpRequestController == NULL || mpRequestController_P1 == NULL || mpRequestController_P1_Main2 == NULL) {
        MY_LOGE("Request controller is null. start recording fail.");
        MY_LOGE_IF(mpRequestController == NULL, "mpRequestController == NULL");
        MY_LOGE_IF(mpRequestController_P1 == NULL, "mpRequestController_P1 == NULL");
        MY_LOGE_IF(mpRequestController_P1_Main2 == NULL, "mpRequestController_P1_Main2 == NULL");
        return BAD_VALUE;
    }

    mPipelineMode = PipelineMode_PREVIEW; //PipelineMode_RECORDING;
    MY_LOGD("mPipelineMode=%d", mPipelineMode);

    // **********************
    // TODO: DVFS control??
    // **********************
    // add DVFS control here
    //

    mpRequestController->stopPipeline();
    mpRequestController_P1->stopPipeline();
    mpRequestController_P1_Main2->stopPipeline();
    //
    if ( mpPipeline != 0 ) {
        mpPipeline->flush();
        mpPipeline->waitUntilDrained();
        mpPipeline = NULL;
    }
    if ( mpPipeline_P1 != 0 ) {
        mpPipeline_P1->flush();
        mpPipeline_P1->waitUntilDrained();
        mpPipeline_P1 = NULL;
    }
    if ( mpPipeline_P1_Main2 != 0 ) {
        mpPipeline_P1_Main2->flush();
        mpPipeline_P1_Main2->waitUntilDrained();
        mpPipeline_P1_Main2 = NULL;
    }

    // delete and re-create stereoSynchronizer
    if(mpStereoBufferSynchronizer != NULL){
        mpStereoBufferSynchronizer->flush();
        mpStereoBufferSynchronizer = NULL;
    }
    MY_LOGD("create stereoSynchronizer");
    mpStereoBufferSynchronizer = StereoBufferSynchronizer::createInstance("SynchronizerRecord");

#warning "fixme enable synchronizer debug mode"
    MY_LOGW("create stereoSynchronizer with debug mode DEBUG_MODE_SKIP_TIMESTAMP_CHECK");
    mpStereoBufferSynchronizer->setDebugMode(StereoBufferSynchronizer::DEBUG_MODE::DEBUG_MODE_SKIP_TIMESTAMP_CHECK);

    // create StereoSelectors
    MY_LOGD("create StereoSelectors");
    mpStereoSelector_RESIZER      = new StereoSelector("RESIZER_VR");
    mpStereoSelector_RESIZER_MAIN2   = new StereoSelector("RESIZER_01_VR");

    // bind stream/selector pair to synchronizer
    MY_LOGD("bind stream/selector pair to synchronizer");
    mpStereoBufferSynchronizer->addStream(eSTREAMID_IMAGE_PIPE_RAW_RESIZER,     mpStereoSelector_RESIZER);
    mpStereoBufferSynchronizer->addStream(eSTREAMID_IMAGE_PIPE_RAW_RESIZER_01,  mpStereoSelector_RESIZER_MAIN2);

    // create pipelines
    MY_LOGD("create pipelines");
    MINT32  const nodeConfigData_P1[] = {eNODEID_P1Node, -1, -1};
    MINT32  const nodeConfigData_P1_Main2[] = {eNODEID_P1Node_main2, -1, -1};
    MINT32  const nodeConfigData_P2[] = {eNODEID_DepthMapNode, eNODEID_BokehNode, -1, -1};

    mpPipeline_P1 = constructP1Pipeline_Main1(
                        StereoPipelineMgrData::p1::VdoRec::gStereoMetaTbl_VdoP1,
                        nodeConfigData_P1,
                        StereoPipelineMgrData::p1::VdoRec::gStereoImgStreamTbl_VdoP1
                );
    mpPipeline_P1_Main2 = constructP1Pipeline_Main2(
                        StereoPipelineMgrData::p1::VdoRec::gStereoMetaTbl_VdoP1Main2,
                        nodeConfigData_P1_Main2,
                        StereoPipelineMgrData::p1::VdoRec::gStereoImgStreamTbl_VdoP1Main2
                );
    mpPipeline = constructP2Pipeline_Prv(
                        StereoPipelineMgrData::p2::VdoRec::gStereoMetaTbl_P2VdoRec,
                        nodeConfigData_P2,
                        StereoPipelineMgrData::p2::VdoRec::gStereoImgStreamTbl_P2VdoRec
                 );

    if ( mpPipeline_P1 == 0 || mpPipeline_P1_Main2 == 0 || mpPipeline == 0) {
        MY_LOGE("Cannot get pipeline. start recording fail.");
        MY_LOGE_IF(mpPipeline_P1 == 0, "mpPipeline_P1 == 0");
        MY_LOGE_IF(mpPipeline_P1_Main2 == 0, "mpPipeline_P1_Main2 == 0");
        MY_LOGE_IF(mpPipeline == 0, "mpPipeline == 0");
        return BAD_VALUE;
    }

    // query consumers
    MY_LOGD("query consumers");
    sp<StreamBufferProvider> provider_resizer       = mpResourceContainier->queryConsumer(eSTREAMID_IMAGE_PIPE_RAW_RESIZER);
    sp<StreamBufferProvider> provider_resizer_01    = mpResourceContainierMain2->queryConsumer(eSTREAMID_IMAGE_PIPE_RAW_RESIZER_01);

    if( provider_resizer == NULL || provider_resizer_01 == NULL)
    {
        MY_LOGE("Cannot get consumer. start recording fail.");
        MY_LOGE_IF(provider_resizer == NULL, "provider_resizer is NULL");
        MY_LOGE_IF(provider_resizer_01 == NULL, "provider_resizer_01 is NULL");
        return BAD_VALUE;
    }

    // bind selectors to consumers
    MY_LOGD("binding selectors to consumers");
    provider_resizer->setSelector(mpStereoSelector_RESIZER);
    provider_resizer_01->setSelector(mpStereoSelector_RESIZER_MAIN2);

    // create stereoRequestUpdater
    MY_LOGD("create stereoRequestUpdater");
    mpStereoRequestUpdater = new StereoRequestUpdater(this, mpParamsManager, mpStereoBufferSynchronizer);
    mpStereoRequestUpdater_Main2 = new StereoMain2RequestUpdater(this, mpParamsManager, mpStereoBufferSynchronizer);

    mpStereoRequestUpdater->addSelector(mpStereoSelector_RESIZER,       StereoRequestUpdater::StreamType::RESIZED);
    mpStereoRequestUpdater->addSelector(mpStereoSelector_RESIZER_MAIN2, StereoRequestUpdater::StreamType::RESIZED_MAIN2);

    mpStereoRequestUpdater->addPool(mpStereoBufferPool_RESIZER,         StereoRequestUpdater::StreamType::RESIZED);
    mpStereoRequestUpdater->addPool(mpStereoBufferPool_RESIZER_MAIN2,   StereoRequestUpdater::StreamType::RESIZED_MAIN2);

    mpStereoRequestUpdater->setSensorParams(mSensorParam, mSensorParam_main2);
    mpStereoRequestUpdater_Main2->setSensorParams(mSensorParam, mSensorParam_main2);

    //
    //constructRecordingPipeline();

    if ( mpPipeline == NULL || mpPipeline_P1 == NULL || mpPipeline_P1_Main2 == NULL) {
        MY_LOGE("Cannot get pipeline. start recording fail.");
        MY_LOGE_IF(mpPipeline_P1 == NULL, "mpPipeline_P1 == NULL");
        MY_LOGE_IF(mpPipeline_P1_Main2 == NULL, "mpPipeline_P1_Main2 == NULL");
        MY_LOGE_IF(mpPipeline == NULL, "mpPipeline == NULL");
        return BAD_VALUE;
    }
    // TODO: ??
    //mpRequestController->setRequestType(MTK_CONTROL_CAPTURE_INTENT_VIDEO_RECORD);
    //

#warning "fixme start & end request number"
    MY_LOGD("start pipelines");
    MERROR startPipelineResult = OK;
    {
        MERROR ret = mpRequestController_P1->startPipeline(
                                    0,/*start*/
                                    1000, /*end*/
                                    mpPipeline_P1,
                                    this
                                );
        if(ret != OK){
            MY_LOGE("startPipeline mpRequestController_P1 failed!");
            startPipelineResult = UNKNOWN_ERROR;
        }
    }
    {
        MERROR ret = mpRequestController_P1_Main2->startPipeline(
                                    0,/*start*/
                                    1000, /*end*/
                                    mpPipeline_P1_Main2,
                                    mpStereoRequestUpdater_Main2
                                );
        if(ret != OK){
            MY_LOGE("startPipeline mpRequestController_P1_Main2 failed!");
            startPipelineResult = UNKNOWN_ERROR;
        }
    }
    {
        MERROR ret = mpRequestController->startPipeline(
                                    0,/*start*/
                                    1000, /*end*/
                                    mpPipeline,
                                    mpStereoRequestUpdater
                                );
        if(ret != OK){
            MY_LOGE("startPipeline mpRequestController failed!");
            startPipelineResult = UNKNOWN_ERROR;
        }
    }

    return startPipelineResult;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
StereoFlowControl::
stopRecording()
{
    //TODO
    return UNKNOWN_ERROR;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
StereoFlowControl::
autoFocus()
{
    if(mpRequestController != 0){
        if(mpRequestController->autoFocus() != OK){
            MY_LOGE("mpRequestController->autoFocus failed!");
        }
    }

    return (mpRequestController_P1 != 0) ?
        mpRequestController_P1->cancelAutoFocus() : OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
StereoFlowControl::
cancelAutoFocus()
{
    if(mpRequestController != 0){
        if(mpRequestController->cancelAutoFocus() != OK){
            MY_LOGE("mpRequestController->cancelAutoFocus failed!");
        }
    }

    return (mpRequestController_P1 != 0) ?
        mpRequestController_P1->cancelAutoFocus() : OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
StereoFlowControl::
precapture(int& flashRequired)
{
    return (mpRequestController_P1 != 0) ?
        mpRequestController_P1->precapture(flashRequired) : OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
StereoFlowControl::
takePicture()
{
    FUNC_START;
    // TODO
    FUNC_END;
    return UNKNOWN_ERROR;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
StereoFlowControl::
setParameters()
{
    MERROR ret = OK;
    if(mpRequestController_P1 != 0){
        ret = mpRequestController_P1->setParameters( this );
    }
    if(mpRequestController_P1_Main2 != 0){
        ret = ret && mpRequestController_P1_Main2->setParameters( this );
    }
    if(mpRequestController != 0){
        ret = ret && mpRequestController->setParameters( mpStereoRequestUpdater );
    }

    return ret;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
StereoFlowControl::
sendCommand(
    int32_t cmd,
    int32_t arg1,
    int32_t arg2
)
{
    MERROR ret = OK;


    // the commands are defined here:
    // \system\core\include\system\camera.h:95
    // the cmd starts from 0 which means we can use 0 as customized command
    MY_LOGD("sendCommand cmd:%d arg1:%d arg2:%d)", cmd, arg1, arg2);

    if(cmd == 0 && arg1 == 0 && arg1 == 0){
        Mutex::Autolock _l(mP2PrvLock);
        MY_LOGD1("stop P2Prv");
        mbEnableP2Prv = MFALSE;
    }
    else
    if(cmd == 0 && arg1 == 1 && arg1 == 1){
        Mutex::Autolock _l(mP2PrvLock);
        mbEnableP2Prv = MTRUE;
        MY_LOGD1("start P2Prv");
        mCondP2PrvLock.signal();
    }else{
        if(mpRequestController_P1 != 0){
            ret = mpRequestController_P1->sendCommand( cmd, arg1, arg2 );
        }
        if(mpRequestController_P1_Main2 != 0){
            ret = ret & mpRequestController_P1_Main2->sendCommand( cmd, arg1, arg2 );
        }
        if(mpRequestController != 0){
            ret = ret & mpRequestController->sendCommand( cmd, arg1, arg2 );
        }
    }

    return ret;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
StereoFlowControl::
dump(
    int fd,
    Vector<String8>const& args
)
{
#warning "TODO"
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
StereoFlowControl::
updateParameters(
    IMetadata* setting
)
{
#warning "[FIXME] check main or main2"
    return ( mpParamsManagerV3 != 0 )
                ? mpParamsManagerV3->updateRequest(setting, mSensorParam.mode)
                : UNKNOWN_ERROR;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
StereoFlowControl::
updateRequestSetting(
    IMetadata* appSetting,
    IMetadata* halSetting
)
{
    // update app control
    {
        IMetadata::IEntry entry(MTK_SCALER_CROP_REGION);
        entry.push_back(this->getActiveArrayCrop(), Type2Type<MRect>());
        appSetting->update(MTK_SCALER_CROP_REGION, entry);

        // add 3DNR flag
        MY_LOGD1("main1 set 3DNR +");
        MINT32 mode3DNR = MTK_NR_FEATURE_3DNR_MODE_OFF;
        if(::strcmp(mpParamsManager->getStr
                    (MtkCameraParameters::KEY_3DNR_MODE),
                    "on") == 0 )
        {
            MY_LOGD("3DNR on");
            mode3DNR = MTK_NR_FEATURE_3DNR_MODE_ON;
        }
        IMetadata::IEntry entry2(MTK_NR_FEATURE_3DNR_MODE);
        entry2.push_back(mode3DNR, Type2Type< MINT32 >());
        appSetting->update(entry2.tag(), entry2);
        MY_LOGD1("main1 set 3DNR -");
    }

    // update hal control
    {
        IMetadata::IEntry entry(MTK_HAL_REQUEST_SENSOR_SIZE);
        entry.push_back(mSensorParam.size, Type2Type< MSize >());
        halSetting->update(entry.tag(), entry);

        IMetadata::IEntry entry2(MTK_P1NODE_SENSOR_CROP_REGION);
        entry2.push_back(this->getSensorDomainCrop(), Type2Type<MRect>());
        halSetting->update(MTK_P1NODE_SENSOR_CROP_REGION, entry2);
    }

    MY_LOGD1("P1 main1 udpate request sensorSize:(%dx%d)  sensorCrop:(%d,%d,%d,%d)",
        mSensorParam.size.w,
        mSensorParam.size.h,
        this->getSensorDomainCrop().p.x,
        this->getSensorDomainCrop().p.y,
        this->getSensorDomainCrop().s.w,
        this->getSensorDomainCrop().s.h
    );

    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
StereoFlowControl::
submitRequest(
    Vector< SettingSet > rvSettings,
    Vector< StreamId >   rvDstStreams,
    Vector< MINT32 >&    rvRequestNo
)
{
    MY_LOGE("No implementation!");
    return UNKNOWN_ERROR;
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
StereoFlowControl::
onLastStrongRef( const void* /*id*/)
{
    FUNC_START;
    mpRequestController_P1 = NULL;
    if ( mpPipeline_P1 != 0 ) {
        mpPipeline_P1->flush();
        mpPipeline_P1->waitUntilDrained();
        mpPipeline_P1 = NULL;
    }
    mpRequestController_P1_Main2 = NULL;
    if ( mpPipeline_P1_Main2 != 0 ) {
        mpPipeline_P1_Main2->flush();
        mpPipeline_P1_Main2->waitUntilDrained();
        mpPipeline_P1_Main2 = NULL;
    }
    mpRequestController = NULL;
    if ( mpPipeline != 0 ) {
        mpPipeline->flush();
        mpPipeline->waitUntilDrained();
        mpPipeline = NULL;
    }
    FUNC_END;
}
/******************************************************************************
 *
 ******************************************************************************/
MERROR
StereoFlowControl::
checkNotifyCallback(
    IMetadata* meta
)
{
    MY_LOGD1("+");

    // check stereo warning
    {
        MUINT8 tempResult;
        if(tryGetMetadata<MUINT8>(const_cast<IMetadata*>(meta), MTK_STEREO_FEATURE_WARNING, tempResult)){
            MY_LOGD("do cb MTK_STEREO_FEATURE_WARNING: %d", tempResult);

            // to adapter
            mpCamMsgCbInfo->doNotifyCallback(
                MTK_CAMERA_MSG_EXT_NOTIFY,
                MTK_CAMERA_MSG_EXT_NOTIFY_STEREO_WARNING,
                tempResult
            );
        }
    }

    MY_LOGD1("-");
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MBOOL
StereoFlowControl::
waitP2PrvReady()
{
    MY_LOGD1("+");

    // MY_LOGW("temp block waitP2PrvReady");
    // mbEnableP2Prv = MFALSE;

    Mutex::Autolock _l(mP2PrvLock);
    while(!mbEnableP2Prv){
        MY_LOGD1("wait for mbEnableP2Prv");
        mCondP2PrvLock.wait(mP2PrvLock);
    }

    MY_LOGD1("-");
    return MTRUE;
}
/******************************************************************************
 *
 ******************************************************************************/
status_t
StereoFlowControl::
changeToPreviewStatus()
{
    // FlowControlBase::changeToPreviewStatus()
    return OK;
}

/******************************************************************************
 * StereoRequestUpdater
 ******************************************************************************/
StereoFlowControl::StereoRequestUpdater::StereoRequestUpdater(
    sp<StereoFlowControl>  pStereoFlowControl,
    sp<IParamsManager> pParamsManager,
    sp<StereoBufferSynchronizer> pStereoBufferSynchronizer
)
: mSelector_resized(NULL)
, mSelector_full(NULL)
, mSelector_resized_main2(NULL)
, mSelector_full_main2(NULL)
, mPool_resized(NULL)
, mPool_full(NULL)
, mPool_resized_main2(NULL)
, mPool_full_main2(NULL)
, mLogLevel(0)
, mpStereoFlowControl(pStereoFlowControl)
, mpParamsManager(pParamsManager)
, mpStereoBufferSynchronizer(pStereoBufferSynchronizer)
, mReqNo(0)
{
    FUNC_START;

    MY_LOGD("StereoRequestUpdater ctor");

    char cProperty[PROPERTY_VALUE_MAX];
    ::property_get("debug.camera.log", cProperty, "0");
    mLogLevel = ::atoi(cProperty);
    if ( 0 == mLogLevel ) {
        ::property_get("debug.camera.log.stereoflow", cProperty, "0");
        mLogLevel = ::atoi(cProperty);
    }

    FUNC_END;
}
/******************************************************************************
 *
 ******************************************************************************/
MVOID
StereoFlowControl::StereoRequestUpdater::
onLastStrongRef( const void* /*id*/)
{
    FUNC_START;

    mSelector_resized = NULL;
    mSelector_full = NULL;
    mSelector_resized_main2 = NULL;
    mSelector_full_main2 = NULL;

    mPool_resized = NULL;
    mPool_full = NULL;
    mPool_resized_main2 = NULL;
    mPool_full_main2 = NULL;

    FUNC_END;
}
/******************************************************************************
 *
 ******************************************************************************/
MERROR
StereoFlowControl::StereoRequestUpdater::
setSensorParams(StereoPipelineSensorParam main1, StereoPipelineSensorParam main2){
    FUNC_START;

    mSensorParam        = main1;
    mSensorParam_main2  = main2;

    MY_LOGD("mSensorParam.size=(%dx%d)  mSensorParam_main2.size=(%dx%d)",
        mSensorParam.size.w,
        mSensorParam.size.h,
        mSensorParam_main2.size.w,
        mSensorParam_main2.size.h
    );

    if( mSensorParam.size.w == 0 ||
        mSensorParam.size.h == 0 ||
        mSensorParam_main2.size.w == 0 ||
        mSensorParam_main2.size.h == 0
    ){
        MY_LOGE("some of the mSensorParam is 0, should not have happened");
        return UNKNOWN_ERROR;
    }

    FUNC_END;
    return OK;
}
/******************************************************************************
 *
 ******************************************************************************/
MERROR
StereoFlowControl::StereoRequestUpdater::
addSelector(sp<StereoSelector> pSelector, MINT32 streamType)
{
    FUNC_START;

    switch(streamType){
        case StreamType::RESIZED:{
            mSelector_resized = pSelector;
            break;
        }
        case StreamType::FULL:{
            mSelector_full = pSelector;
            break;
        }
        case StreamType::RESIZED_MAIN2:{
            mSelector_resized_main2 = pSelector;
            break;
        }
        case StreamType::FULL_MAIN2:{
            mSelector_full_main2 = pSelector;
            break;
        }
        default:{
            MY_LOGE("adding selector with unknown stream type!");
            return UNKNOWN_ERROR;
        }
    }

    MY_LOGD("mSelector_resized:%p",         mSelector_resized.get());
    MY_LOGD("mSelector_full:%p",            mSelector_full.get());
    MY_LOGD("mSelector_resized_main2:%p",   mSelector_resized_main2.get());
    MY_LOGD("mSelector_full_main2:%p",      mSelector_full_main2.get());

    FUNC_END;
    return OK;
}
/******************************************************************************
 *
 ******************************************************************************/
MERROR
StereoFlowControl::StereoRequestUpdater::
addPool(sp<StereoBufferPool> pPool, MINT32 streamType)
{
    FUNC_START;

    switch(streamType){
        case StreamType::RESIZED:{
            mPool_resized = pPool;
            break;
        }
        case StreamType::FULL:{
            mPool_full = pPool;
            break;
        }
        case StreamType::RESIZED_MAIN2:{
            mPool_resized_main2 = pPool;
            break;
        }
        case StreamType::FULL_MAIN2:{
            mPool_full_main2 = pPool;
            break;
        }
        default:{
            MY_LOGE("adding selector with unknown stream type!");
            return UNKNOWN_ERROR;
        }
    }

    MY_LOGD("mPool_resized:%p",         mPool_resized.get());
    MY_LOGD("mPool_full:%p",            mPool_full.get());
    MY_LOGD("mPool_resized_main2:%p",   mPool_resized_main2.get());
    MY_LOGD("mPool_full_main2:%p",      mPool_full_main2.get());

    FUNC_END;
    return OK;
}
/******************************************************************************
 *
 ******************************************************************************/
MERROR
StereoFlowControl::StereoRequestUpdater::
updateRequestSetting(IMetadata* appSetting, IMetadata* halSetting)
{
    FUNC_START;

    mpStereoFlowControl->waitP2PrvReady();

    if(mSelector_resized == NULL || mSelector_resized_main2 == NULL){
        MY_LOGE("updateRequestSetting before required providers are set, sould not have happened!");
    }

    // step 0
    // lock synchronizer
    mpStereoBufferSynchronizer->lock();

    android::sp<IImageBufferHeap> spHeap_resized        = NULL;
    android::sp<IImageBufferHeap> spHeap_full           = NULL;
    android::sp<IImageBufferHeap> spHeap_resized_main2  = NULL;
    Vector<ISelector::MetaItemSet> tempMetadata;
    IMetadata appDynamic;

    MY_LOGD1("Creating P2 request req(%d) + ", mReqNo);

    // step 1
    // getResult from main1_resized
    // appMeta and halMeta also updated by this Selector
    if(mSelector_resized != NULL)
    {
        MY_LOGD1("getResult from main1_resized  +");
        tempMetadata.clear();

        MINT32 tempReqNo = -1;
        if(mSelector_resized->getResult(tempReqNo, tempMetadata, spHeap_resized) != OK){
            MY_LOGE("mSelector_resized->getResult failed!");
        }

        int index_appDynamic = -1;
        int index_halDynamic = -1;
        for ( int i = 0; i < tempMetadata.size(); ++i ){
            switch(tempMetadata[i].id){
                case eSTREAMID_META_APP_DYNAMIC_P1:
                    index_appDynamic = i;
                break;
                case eSTREAMID_META_HAL_DYNAMIC_P1:
                    index_halDynamic = i;
                break;
                default:
                    MY_LOGE("unexpected meta stream from selector:%d", tempMetadata[i].id);
                break;
            }
        }
        if(index_appDynamic != -1 || index_halDynamic != -1){
            appDynamic = tempMetadata[index_appDynamic].meta;
            *halSetting = tempMetadata[index_halDynamic].meta;
        }else{
            MY_LOGE("some meta stream from P1_main1 is not correctly set! (%d ,%d )",
                index_appDynamic,
                index_halDynamic
            );
            return UNKNOWN_ERROR;
        }

        MY_LOGD1("getResult from main1_resized  -");
    }

    // step 2
    // getResult from main1_full
    if(mSelector_full != NULL)
    {
        MY_LOGD1("getResult from main1_full  +");
        tempMetadata.clear();

        MINT32 tempReqNo = -1;
        if(mSelector_full->getResult(tempReqNo, tempMetadata, spHeap_full) != OK){
            MY_LOGE("mSelector_full->getResult failed!");
        }
        MY_LOGD1("getResult from main1_full  -");
    }

    // step 3
    // getResult from main2_resized
    if(mSelector_resized_main2 != NULL)
    {
        MY_LOGD1("getResult from main2_resized  +");
        tempMetadata.clear();

        MINT32 tempReqNo = -1;
        if(mSelector_resized_main2->getResult(tempReqNo, tempMetadata, spHeap_resized_main2) != OK){
            MY_LOGE("mSelector_resized_main2->getResult failed!");
        }

        int index_appDynamic = -1;
        int index_halDynamic = -1;
        for ( int i = 0; i < tempMetadata.size(); ++i ){
            switch(tempMetadata[i].id){
                case eSTREAMID_META_APP_DYNAMIC_P1_MAIN2:
                    index_appDynamic = i;
                break;
                case eSTREAMID_META_HAL_DYNAMIC_P1_MAIN2:
                    index_halDynamic = i;
                break;
                default:
                    MY_LOGE("unexpected meta stream from selector:%d", tempMetadata[i].id);
                break;
            }
        }
        if(index_appDynamic != -1 || index_halDynamic != -1){
            MY_LOGD1("use main2 hal dynamic");
            IMetadata theMeta = tempMetadata[index_halDynamic].meta;

            MY_LOGD1("push main2 sensor size into hal_meta");
            IMetadata::IEntry entry(MTK_HAL_REQUEST_SENSOR_SIZE);
            entry.push_back(mSensorParam_main2.size, Type2Type< MSize >());
            theMeta.update(entry.tag(), entry);

            MY_LOGD1("push main2 hal dynamic into hal_meta");
            IMetadata::IEntry entry_meta(MTK_P1NODE_MAIN2_HAL_META);
            entry_meta.push_back(theMeta, Type2Type< IMetadata >());
            halSetting->update(entry_meta.tag(), entry_meta);
        }else{
            MY_LOGE("some meta stream from P1_main2 is not correctly set! (%d ,%d )",
                index_appDynamic,
                index_halDynamic
            );
            return UNKNOWN_ERROR;
        }
        MY_LOGD1("getResult from main2_resized  -");
    }

    // step 4
    // unlock synchronizer
    mpStereoBufferSynchronizer->unlock();

    // step 5
    // get DOF level and push into appSetting
    // temp hard code dof level here
    // will get this value from app
    {
        int DoFLevel_UI = mpParamsManager->getInt(MtkCameraParameters::KEY_STEREO_DOF_LEVEL);
        int DoFLevel_HAL = DoFLevel_UI*2;
        int DoFMin_HAL = 0;
        int DoFMax_HAL = 30;

        if(DoFLevel_UI<DoFMin_HAL){
            MY_LOGW("DoFLevel_HAL not in illegal range, reset to %d", DoFMin_HAL);
            DoFLevel_HAL = DoFMin_HAL;
        }

        if(DoFLevel_UI>DoFMax_HAL){
            MY_LOGW("DoFLevel_HAL not in illegal range, reset to %d", DoFMax_HAL);
            DoFLevel_HAL = DoFMax_HAL;
        }

        IMetadata::IEntry entry(MTK_STEREO_FEATURE_DOF_LEVEL);
        entry.push_back(DoFLevel_HAL, Type2Type< MINT32 >());
        appSetting->update(entry.tag(), entry);
        MY_LOGD1("Set DofLevel UI/HAL (%d/%d)", DoFLevel_UI, DoFLevel_HAL);
    }
    // add 3DNR flag
    {
        MY_LOGD1("set 3DNR +");
        MINT32 mode3DNR = MTK_NR_FEATURE_3DNR_MODE_OFF;
        if(::strcmp(mpStereoFlowControl->mpParamsManager->getStr
                    (MtkCameraParameters::KEY_3DNR_MODE),
                    "on") == 0 )
        {
            MY_LOGD("3DNR on");
            mode3DNR = MTK_NR_FEATURE_3DNR_MODE_ON;
        }
        IMetadata::IEntry entry(MTK_NR_FEATURE_3DNR_MODE);
        entry.push_back(mode3DNR, Type2Type< MINT32 >());
        appSetting->update(entry.tag(), entry);
        MY_LOGD1("set 3DNR -");
    }

    // step 6
    // enque halBuffer to P2Pipeline's buffer pool
    {
        MY_LOGD1("setBuffer to pool +");
        mPool_resized->setBuffer("spHeap_resized", spHeap_resized);
        mPool_resized_main2->setBuffer("spHeap_resized_main2", spHeap_resized_main2);
        if(mPool_full != NULL) {
            mPool_full->setBuffer("spHeap_full", spHeap_full);
        }
        MY_LOGD1("setBuffer to pool -");
    }

    // step 7
    // update scaler crop region
    {
        IMetadata::IEntry entry(MTK_SCALER_CROP_REGION);
        entry.push_back(mpStereoFlowControl->getActiveArrayCrop(), Type2Type<MRect>());
        appSetting->update(MTK_SCALER_CROP_REGION, entry);
    }

    // step 8
    // check if we need to do some callback(ex: distance warning/ too close warning)
    if(mpStereoFlowControl != NULL){
        mpStereoFlowControl->checkNotifyCallback(&appDynamic);
    }

#if 1
    // Debug Log
    {
        MSize tempSize;
        if( ! tryGetMetadata<MSize>(const_cast<IMetadata*>(halSetting), MTK_HAL_REQUEST_SENSOR_SIZE, tempSize) ){
            MY_LOGE("cannot get MTK_HAL_REQUEST_SENSOR_SIZE after updating request");
        }else{
            MY_LOGD1("MTK_HAL_REQUEST_SENSOR_SIZE:(%dx%d)", tempSize.w, tempSize.h);
        }
        IMetadata tempMetadata;
        if( ! tryGetMetadata<IMetadata>(const_cast<IMetadata*>(halSetting), MTK_P1NODE_MAIN2_HAL_META, tempMetadata) ){
            MY_LOGE("cannot get MTK_P1NODE_MAIN2_HAL_META after updating request");
        }else{
            MY_LOGD1("MTK_P1NODE_MAIN2_HAL_META");
        }
        MINT32 tempLevel;
        if( ! tryGetMetadata<MINT32>(const_cast<IMetadata*>(appSetting), MTK_STEREO_FEATURE_DOF_LEVEL, tempLevel) ){
            MY_LOGE("cannot get MTK_STEREO_FEATURE_DOF_LEVEL after updating request");
        }else{
            MY_LOGD1("MTK_STEREO_FEATURE_DOF_LEVEL:%d", tempLevel);
        }
    }
#endif

    // send timestamp for those who needs one.
    {
        sp<IResourceContainer> pResourceContainierP2Prv = IResourceContainer::getInstance(mpStereoFlowControl->getOpenId_P2Prv());
        sp<StreamBufferProvider> provider_resizer       = pResourceContainierP2Prv->queryConsumer(eSTREAMID_IMAGE_PIPE_RAW_RESIZER);
        sp<StreamBufferProvider> provider_resizer_01    = pResourceContainierP2Prv->queryConsumer(eSTREAMID_IMAGE_PIPE_RAW_RESIZER_01);
        sp<StreamBufferProvider> provider_opaque        = pResourceContainierP2Prv->queryConsumer(eSTREAMID_IMAGE_PIPE_RAW_OPAQUE);
        sp<StreamBufferProvider> provider_preview_yuv   = pResourceContainierP2Prv->queryConsumer(eSTREAMID_IMAGE_PIPE_BOKEHNODE_PREVIEWYUV);
        sp<StreamBufferProvider> provider_record_yuv    = pResourceContainierP2Prv->queryConsumer(eSTREAMID_IMAGE_PIPE_BOKEHNODE_RECORDYUV);
        if( provider_resizer == NULL || provider_resizer_01 == NULL || provider_preview_yuv == NULL)
        {
            MY_LOGE("Cannot get provider. create P2 request faield.");
            MY_LOGE_IF(provider_resizer == NULL, "provider_resizer is NULL");
            MY_LOGE_IF(provider_resizer_01 == NULL, "provider_resizer_01 is NULL");
            MY_LOGE_IF(provider_preview_yuv == NULL, "provider_preview_yuv is NULL");
            return BAD_VALUE;
        }else{
            MINT64 dummyTimestamp = mReqNo;
            MY_LOGD1("manually do doTimestampCallback mReqNo(%d),errorResult(%d),dummyTimestamp(%lld)", mReqNo, MFALSE, dummyTimestamp);
            provider_resizer->doTimestampCallback(      mReqNo, MFALSE, dummyTimestamp);
            provider_resizer_01->doTimestampCallback(   mReqNo, MFALSE, dummyTimestamp);
            if(provider_opaque != NULL) {
                provider_opaque->doTimestampCallback(       mReqNo, MFALSE, dummyTimestamp);
            }
            provider_preview_yuv->doTimestampCallback(  mReqNo, MFALSE, dummyTimestamp);
            if(provider_record_yuv != NULL) {
                provider_record_yuv->doTimestampCallback(       mReqNo, MFALSE, dummyTimestamp);
            }
        }
    }

    MY_LOGD1("Creating P2 request req(%d) - ", mReqNo);
    mReqNo++;
#warning "fix me! hard code request range"
    if(mReqNo == 1001){
        mReqNo = 0;
    };

    FUNC_END;
    return OK;
}
/******************************************************************************
 *
 ******************************************************************************/
MERROR
StereoFlowControl::StereoRequestUpdater::
updateParameters(IMetadata* setting)
{
    MY_LOGE("No implementation!");
    return OK;
}
/******************************************************************************
 * StereoMain2RequestUpdater
 ******************************************************************************/
StereoFlowControl::StereoMain2RequestUpdater::StereoMain2RequestUpdater(
    sp<StereoFlowControl>  pStereoFlowControl,
    sp<IParamsManager> pParamsManager,
    sp<StereoBufferSynchronizer> pStereoBufferSynchronizer
)
: StereoRequestUpdater(pStereoFlowControl, pParamsManager, pStereoBufferSynchronizer)
{
    MY_LOGD("StereoMain2RequestUpdater ctor");
}
/******************************************************************************
 *
 ******************************************************************************/
MERROR
StereoFlowControl::StereoMain2RequestUpdater::
updateRequestSetting(IMetadata* appSetting, IMetadata* halSetting){
    // update app control
    {
        IMetadata::IEntry entry(MTK_SCALER_CROP_REGION);
        entry.push_back(mpStereoFlowControl->getActiveArrayCrop_Main2(), Type2Type<MRect>());
        appSetting->update(MTK_SCALER_CROP_REGION, entry);
    }

    // update hal control
    {
        IMetadata::IEntry entry(MTK_HAL_REQUEST_SENSOR_SIZE);
        entry.push_back(mSensorParam_main2.size, Type2Type< MSize >());
        halSetting->update(entry.tag(), entry);

        IMetadata::IEntry entry2(MTK_P1NODE_SENSOR_CROP_REGION);
        entry2.push_back(mpStereoFlowControl->getSensorDomainCrop_Main2(), Type2Type<MRect>());
        halSetting->update(MTK_P1NODE_SENSOR_CROP_REGION, entry2);
    }

    MY_LOGD1("P1 main2 udpate request sensorSize:(%dx%d)  sensorCrop:(%d,%d,%d,%d)",
        mSensorParam_main2.size.w,
        mSensorParam_main2.size.h,
        mpStereoFlowControl->getSensorDomainCrop_Main2().p.x,
        mpStereoFlowControl->getSensorDomainCrop_Main2().p.y,
        mpStereoFlowControl->getSensorDomainCrop_Main2().s.w,
        mpStereoFlowControl->getSensorDomainCrop_Main2().s.h
    );

    return OK;
}
