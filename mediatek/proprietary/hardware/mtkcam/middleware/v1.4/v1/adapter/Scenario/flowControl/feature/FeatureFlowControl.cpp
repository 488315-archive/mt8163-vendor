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

#define LOG_TAG "MtkCam/FeatureFlowControl"
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
#include "FeatureFlowControl.h"

#include <camera/MtkCamera.h>

#ifdef FEATURE_MODIFY
#include "isp_tuning.h" // For EIspProfie_T
#define VHDR_PROFILE_READY 1
#include <eis/eis_ext.h>
using namespace NSIspTuning;
#endif // FEATURE_MODIFY

using namespace NSCam;
using namespace NSCam::v1;
using namespace NSCam::v1::NSLegacyPipeline;
using namespace android;
using namespace NSCam::v3;
using namespace NSCamHW;
using namespace NSCam::Utils;

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

#ifdef FEATURE_MODIFY
EIspProfile_T
getVHDR3AProfile(bool isRecording, MUINT32 vhdrMode)
{
    if(vhdrMode == SENSOR_VHDR_MODE_NONE)
        return EIspProfile_Preview;

    if(isRecording)
    {
        switch(vhdrMode)
        {
            case SENSOR_VHDR_MODE_ZVHDR:
                return EIspProfile_zHDR_Video;
            case SENSOR_VHDR_MODE_MVHDR:
                return EIspProfile_MHDR_Video;
            case SENSOR_VHDR_MODE_IVHDR:
                return EIspProfile_IHDR_Video;
        }
    }
    else
    { // Preview
        switch(vhdrMode)
        {
            case SENSOR_VHDR_MODE_ZVHDR:
                return EIspProfile_zHDR_Preview;
            case SENSOR_VHDR_MODE_MVHDR:
                return EIspProfile_MHDR_Preview;
            case SENSOR_VHDR_MODE_IVHDR:
                return EIspProfile_IHDR_Preview;
        }
    }

    return EIspProfile_Preview;
}
#endif // FEATURE_MODIFY


/******************************************************************************
 *
 ******************************************************************************/
FeatureFlowControl::
FeatureFlowControl(
    char const*                 pcszName,
    MINT32 const                i4OpenId,
    sp<IParamsManager>          pParamsManager,
    sp<ImgBufProvidersManager>  pImgBufProvidersManager,
    sp<INotifyCallback>         pCamMsgCbInfo
)
    : mName(const_cast<char*>(pcszName))
    , mOpenId(i4OpenId)
    , mLogLevel(1)
    , mpRequestThreadLoopCnt(1)
    , mEnDng(0)
    , mpParamsManager(pParamsManager)
    , mpImgBufProvidersMgr(pImgBufProvidersManager)
    , mpCamMsgCbInfo(pCamMsgCbInfo)
    , mEisMode(EIS_MODE_OFF)
    , mRecordRrzoSize(MSize(0,0))
{
    mLPBConfigParams.mode = LegacyPipelineMode_T::PipelineMode_Feature_Preview;
    mLPBConfigParams.enableEIS = MFALSE;
    mLPBConfigParams.enableLCS = MFALSE;
    mpParamsManagerV3 = IParamsManagerV3::createInstance(
                                    String8::format(pcszName),
                                    i4OpenId,
                                    pParamsManager
                                );
    //
    MY_LOGD("ResourceContainer::getInstance(%d)",mOpenId);
    mpResourceContainer = IResourceContainer::getInstance(mOpenId);
}

/******************************************************************************
 *
 ******************************************************************************/
char const*
FeatureFlowControl::
getName()   const
{
    return mName;
}

/******************************************************************************
 *
 ******************************************************************************/
int32_t
FeatureFlowControl::
getOpenId() const
{
    return mOpenId;
}


/******************************************************************************
 *
 ******************************************************************************/
status_t
FeatureFlowControl::
startPreview()
{
    FUNC_START;

#ifdef FEATURE_MODIFY
    mEisMode = EIS_MODE_OFF;
#endif // FEATURE_MODIFY

    //
    mLPBConfigParams.enableEIS = CamManager::getInstance()->getFirstPreviewId() == getOpenId() ? MTRUE : MFALSE;
    //
    mpRequestController = IRequestController::createInstance(
                                                mName,
                                                mOpenId,
                                                mpCamMsgCbInfo,
                                                mpParamsManagerV3
                                            );
    //
    mpResourceContainer->setFeatureFlowControl(this);
    //
    if ( mpParamsManager->getRecordingHint() )
    {
        constructRecordingPipeline();
        mpRequestController->setRequestType(CAMERA3_TEMPLATE_VIDEO_RECORD);
    } else if ( PARAMSMANAGER_MAP_INST(eMapAppMode)->stringFor(mpParamsManager->getHalAppMode())
                ==  MtkCameraParameters::APP_MODE_NAME_MTK_ZSD)
    {
        constructZsdPreviewPipeline();
        mpRequestController->setRequestType(CAMERA3_TEMPLATE_ZERO_SHUTTER_LAG);
    }
    else
    {
        constructNormalPreviewPipeline();
        mpRequestController->setRequestType(MTK_CONTROL_CAPTURE_INTENT_PREVIEW);
    }
    //
    mpParamsManagerV3->setPreviewMaxFps(
                            (mpParamsManager->getInt(CameraParameters::KEY_PREVIEW_FRAME_RATE) <= mSensorParam.fps) ?
                            mpParamsManager->getInt(CameraParameters::KEY_PREVIEW_FRAME_RATE) : mSensorParam.fps);
    //
    if ( mpPipeline == 0 ) {
        MY_LOGE("Cannot get pipeline. start preview fail.");
        return BAD_VALUE;
    }
    //
#warning "fixme start & end request number"
    MERROR ret = mpRequestController->startPipeline(
                                    0,/*start*/
                                    1000, /*end*/
                                    mpPipeline,
                                    this,
                                    mLPBConfigParams.mode,
                                    mpRequestThreadLoopCnt);
    //
    FUNC_END;
    return ret;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
FeatureFlowControl::
stopPreview()
{
    FUNC_START;
    if( mpRequestController != 0 )
    {
        mpRequestController->stopPipeline();
        mpRequestController = NULL;
    }

    if ( mpPipeline != 0 ) {
        mpPipeline->flush();
        mpPipeline->waitUntilDrained();
        mpPipeline = NULL;
    }

    mRecordRrzoSize = MSize(0,0);
    FUNC_END;
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
FeatureFlowControl::
suspendPreview()
{
    FUNC_START;
    if( mpRequestController == NULL )
    {
        MY_LOGE("can't suspend preview with a null RequestController");
        return UNKNOWN_ERROR;
    }
    //
    status_t ret = mpRequestController->stopPipeline();
    //
    FUNC_END;
    return ret;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
FeatureFlowControl::
resumePreview()
{
    FUNC_START;
     if( mpRequestController == NULL )
     {
         MY_LOGE("can't resume preview with a null RequestController");
         return UNKNOWN_ERROR;
     }
     //
     if ( mpPipeline == 0 ) {
         MY_LOGE("Cannot get pipeline. resume preview fail.");
         return BAD_VALUE;
     }
     //
#warning "fixme start & end request number"
     status_t ret = mpRequestController->startPipeline(
                                             1001,/*start*/
                                             2000, /*end*/
                                             mpPipeline,
                                             this,
                                             mLPBConfigParams.mode,
                                             mpRequestThreadLoopCnt);
     //
     FUNC_END;
     return ret;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
FeatureFlowControl::
startRecording()
{
#if 1
    if( ( mLPBConfigParams.mode != LegacyPipelineMode_T::PipelineMode_VideoRecord &&
          mLPBConfigParams.mode != LegacyPipelineMode_T::PipelineMode_HighSpeedVideo &&
          mLPBConfigParams.mode != LegacyPipelineMode_T::PipelineMode_Feature_VideoRecord &&
          mLPBConfigParams.mode != LegacyPipelineMode_T::PipelineMode_Feature_HighSpeedVideo ) ||
        needReconstructRecordingPipe() )
    {
        mpRequestController->stopPipeline();

        if ( mpPipeline != 0 ) {
            mpPipeline->flush();
            mpPipeline->waitUntilDrained();
            mpPipeline = NULL;
        }
        //
        constructRecordingPipeline();
    }
#endif

    if ( mpPipeline == 0 ) {
        MY_LOGE("Cannot get pipeline. start preview fail.");
        return BAD_VALUE;
    }
    //
    mpRequestController->setRequestType(MTK_CONTROL_CAPTURE_INTENT_VIDEO_RECORD);

#ifdef FEATURE_MODIFY
    if (mpParamsManager->getVideoStabilization())
    {
        MINT32 videoWidth = 0,videoHeight = 0;

        if( (mLPBConfigParams.mode == LegacyPipelineMode_T::PipelineMode_VideoRecord) ||
            (mLPBConfigParams.mode == LegacyPipelineMode_T::PipelineMode_Feature_VideoRecord) )
        {
            mpParamsManager->getVideoSize(&videoWidth,&videoHeight);

            if ( ((videoWidth*videoHeight) <= (VR_1080P_W*VR_1080P_H)) &&
                 (mpParamsManager->getVHdr() == SENSOR_VHDR_MODE_NONE))
            {
                //Enable GIS
                EIS_MODE_ENABLE_EIS_22(mEisMode);
            }else
            {
                EIS_MODE_ENABLE_EIS_12(mEisMode);
            }
            MY_LOGD("start recording WxH (%dx%d), eisMode %d",videoWidth,videoHeight,mEisMode);
        }else if( (mLPBConfigParams.mode == LegacyPipelineMode_T::PipelineMode_HighSpeedVideo) ||
                  (mLPBConfigParams.mode == LegacyPipelineMode_T::PipelineMode_Feature_HighSpeedVideo) )
        {
            //DoNothing
            mEisMode = EIS_MODE_OFF;
        }
    }
#endif // FEATURE_MODIFY



    //
#warning "fixme start & end request number"
    return mpRequestController->startPipeline(
                                    1001,/*start*/
                                    2000, /*end*/
                                    mpPipeline,
                                    this,
                                    mLPBConfigParams.mode,
                                    mpRequestThreadLoopCnt);
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
FeatureFlowControl::
stopRecording()
{
    /*if( mpRequestController != 0 )
        mpRequestController->stopPipeline();

    if ( mpPipeline != 0 ) {
        mpPipeline->flush();
        mpPipeline->waitUntilDrained();
        mpPipeline = NULL;
    }*/
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
FeatureFlowControl::
autoFocus()
{
    return (mpRequestController != 0) ?
        mpRequestController->autoFocus() : OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
FeatureFlowControl::
cancelAutoFocus()
{
    return (mpRequestController != 0) ?
        mpRequestController->cancelAutoFocus() : OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
FeatureFlowControl::
precapture(int& flashRequired)
{
    return (mpRequestController != 0) ?
        mpRequestController->precapture(flashRequired) : OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
FeatureFlowControl::
takePicture()
{
#if 1
    FUNC_START;

    if( mLPBConfigParams.mode == LegacyPipelineMode_T::PipelineMode_VideoRecord ||
        mLPBConfigParams.mode == LegacyPipelineMode_T::PipelineMode_Feature_VideoRecord )
    {
        Vector< SettingSet > vSettings;
        Vector< StreamId >   vDstStreams;
        Vector< MINT32 >     vRequestNo;

        constructVSSRequest(vDstStreams);
        submitRequest( vSettings, vDstStreams, vRequestNo );

        FUNC_END;
        return OK;
    }
    else
    {
        MY_LOGW("Not support LegacyPipelineMode %d",mLPBConfigParams.mode);
    }

#endif
    //
    FUNC_END;
    return UNKNOWN_ERROR;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
FeatureFlowControl::
setParameters()
{
    return (mpRequestController != 0) ?
        mpRequestController->setParameters( this ) : OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
FeatureFlowControl::
sendCommand(
    int32_t cmd,
    int32_t arg1,
    int32_t arg2
)
{
    if( cmd == CAMERA_CMD_ENABLE_RAW16_CALLBACK)
    {
        CAM_LOGD("[defaultflowcontrol:sendCommand] CAMERA_CMD_ENABLE_RAW16_CALLBACK (%d)\n", arg1);
        mEnDng = arg1;
    }
    return (mpRequestController != 0) ?
        mpRequestController->sendCommand( cmd, arg1, arg2 ) : OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
FeatureFlowControl::
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
FeatureFlowControl::
updateParameters(
    IMetadata* setting
)
{
    if (mpParamsManagerV3 != 0)
    {
        if( mLPBConfigParams.mode == LegacyPipelineMode_T::PipelineMode_VideoRecord ||
            mLPBConfigParams.mode == LegacyPipelineMode_T::PipelineMode_HighSpeedVideo ||
            mLPBConfigParams.mode == LegacyPipelineMode_T::PipelineMode_Feature_VideoRecord ||
            mLPBConfigParams.mode == LegacyPipelineMode_T::PipelineMode_Feature_HighSpeedVideo )
        {
            mpParamsManagerV3->updateRequestRecord(setting);
        }else
        {
            mpParamsManagerV3->updateRequestPreview(setting);
        }
    }

    return ( mpParamsManagerV3 != 0 )
                ? mpParamsManagerV3->updateRequest(setting, mSensorParam.mode)
                : UNKNOWN_ERROR;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
FeatureFlowControl::
updateRequestSetting(
    IMetadata* appSetting,
    IMetadata* halSetting
)
{
    // update app control
    if(mEnDng)
    {
        IMetadata::IEntry entry(MTK_STATISTICS_LENS_SHADING_MAP_MODE);
        entry.push_back(MTK_STATISTICS_LENS_SHADING_MAP_MODE_ON, Type2Type< MUINT8 >());
        appSetting->update(entry.tag(), entry);
        MY_LOGD("DNG set MTK_STATISTICS_LENS_SHADING_MAP_MODE (ON)");
    }

#ifdef FEATURE_MODIFY
    {
        MINT32 mode3DNR = MTK_NR_FEATURE_3DNR_MODE_OFF;
        if( ::strcmp(mpParamsManager->getStr(MtkCameraParameters::KEY_3DNR_MODE), "on") == 0 )
        {
            if( mLPBConfigParams.mode != LegacyPipelineMode_T::PipelineMode_Capture &&
                mLPBConfigParams.mode != LegacyPipelineMode_T::PipelineMode_Feature_Capture )
            {
                mode3DNR = MTK_NR_FEATURE_3DNR_MODE_ON;

            }
        }
        IMetadata::IEntry entry(MTK_NR_FEATURE_3DNR_MODE);
        entry.push_back(mode3DNR, Type2Type< MINT32 >());
        appSetting->update(entry.tag(), entry);
    }
#endif // FEATURE_MODIFY

    // update hal control
    {
        IMetadata::IEntry entry(MTK_HAL_REQUEST_SENSOR_SIZE);
        entry.push_back(mSensorParam.size, Type2Type< MSize >());
        halSetting->update(entry.tag(), entry);
    }
    // update default HAL settings
    mpParamsManagerV3->updateRequestHal(halSetting);

    //
    if( mLPBConfigParams.mode == LegacyPipelineMode_T::PipelineMode_ZsdPreview ||
        mLPBConfigParams.mode == LegacyPipelineMode_T::PipelineMode_VideoRecord ||
        mLPBConfigParams.mode == LegacyPipelineMode_T::PipelineMode_Feature_ZsdPreview ||
        mLPBConfigParams.mode == LegacyPipelineMode_T::PipelineMode_Feature_VideoRecord)
    {
        IMetadata::IEntry entry(MTK_HAL_REQUEST_REQUIRE_EXIF);
        entry.push_back(true, Type2Type< MUINT8 >());
        halSetting->update(entry.tag(), entry);
    }

#ifdef FEATURE_MODIFY
    //
#if VHDR_PROFILE_READY
    EIspProfile_T profile;
    profile = getVHDR3AProfile(mpParamsManager->getRecordingHint(), mpParamsManager->getVHdr());

    if( profile != EIspProfile_Preview) // only set profile when not normal situation
    {
        IMetadata::IEntry entry(MTK_3A_ISP_PROFILE);
        entry.push_back(profile, Type2Type< MUINT8 >());
        halSetting->update(entry.tag(), entry);
    }
#endif // VHDR_PROFILE_READY
    //

    if (mpParamsManager->getVideoStabilization())
    {
        MINT32 eisMode = 0;
        MINT32 videoWidth = 0,videoHeight = 0;
        IMetadata::IEntry entry(MTK_EIS_MODE);

        //WHY CHECK PipelineMode_Preview???
        if( (mLPBConfigParams.mode == LegacyPipelineMode_T::PipelineMode_Preview) ||
            (mLPBConfigParams.mode == LegacyPipelineMode_T::PipelineMode_Feature_Preview) )
        {
            if (mpParamsManager->getVHdr() == SENSOR_VHDR_MODE_NONE)
            {
                EIS_MODE_ENABLE_CALIBRATION(eisMode);
            }
            entry.push_back(eisMode, Type2Type< MINT32 >());

        }else if( (mLPBConfigParams.mode == LegacyPipelineMode_T::PipelineMode_VideoRecord) ||
                  (mLPBConfigParams.mode == LegacyPipelineMode_T::PipelineMode_Feature_VideoRecord) )
        {
            entry.push_back(mEisMode, Type2Type< MINT32 >());
        }else if( (mLPBConfigParams.mode == LegacyPipelineMode_T::PipelineMode_HighSpeedVideo) ||
                  (mLPBConfigParams.mode == LegacyPipelineMode_T::PipelineMode_Feature_HighSpeedVideo) )
        {
            entry.push_back(mEisMode, Type2Type< MINT32 >());
        }
        halSetting->update(entry.tag(), entry);
    }
#endif // FEATURE_MODIFY


    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
FeatureFlowControl::
submitRequest(
    Vector< SettingSet > rvSettings,
    Vector< StreamId >   rvDstStreams,
    Vector< MINT32 >&    rvRequestNo
)
{
    status_t ret = UNKNOWN_ERROR;
    if( mpRequestController == NULL)
    {
        MY_LOGE("mpRequestController is NULL");
        return UNKNOWN_ERROR;
    }
    //
    ret = mpRequestController->submitRequest( rvSettings, rvDstStreams, rvRequestNo );
    if( ret != OK)
    {
        MY_LOGE("submitRequest Fail!");
        return UNKNOWN_ERROR;
    }
    //
    return ret;
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
FeatureFlowControl::
onLastStrongRef( const void* /*id*/)
{
    FUNC_START;
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
FeatureFlowControl::
selectHighSpeedSensorScen(
    MUINT   fps,
    MUINT&  sensorScen)
{
    SensorSlimVideoInfo sensorSlimVideoselect;
    SensorSlimVideoInfo sensorSlimVideo[2];
    //
    HwInfoHelper helper(mOpenId);
    if( ! helper.updateInfos() )
    {
        MY_LOGE("cannot properly update infos");
        return BAD_VALUE;
    }
    //
    sensorSlimVideo[0].scenario = SENSOR_SCENARIO_ID_SLIM_VIDEO1;
    sensorSlimVideo[1].scenario = SENSOR_SCENARIO_ID_SLIM_VIDEO2;
    for(MUINT i = 0; i<2; i++)
    {
        helper.getSensorFps(sensorSlimVideo[i].scenario, (MINT32&)sensorSlimVideo[i].fps);
        MY_LOGD("Slim video(%d) FPS(%d)",
                i,
                sensorSlimVideo[i].fps);
    }
    //
    sensorSlimVideoselect.scenario = 0;
    sensorSlimVideoselect.fps = 0;
    for(MUINT i = 0; i<2; i++)
    {
        if(mpParamsManager->getInt(CameraParameters::KEY_PREVIEW_FRAME_RATE) <= sensorSlimVideo[i].fps)
        {
            if(sensorSlimVideoselect.fps > 0)
            {
                if(sensorSlimVideoselect.fps > sensorSlimVideo[i].fps)
                {
                    sensorSlimVideoselect.scenario = sensorSlimVideo[i].scenario;
                    sensorSlimVideoselect.fps = sensorSlimVideo[i].fps;
                }
            }
            else
            {
                sensorSlimVideoselect.scenario = sensorSlimVideo[i].scenario;
                sensorSlimVideoselect.fps = sensorSlimVideo[i].fps;
            }
        }
    }
    //
    if(sensorSlimVideoselect.fps > 0)
    {
        MY_LOGD("Use sensor scenario(%d) FPS(%d)",
                sensorSlimVideoselect.scenario,
                sensorSlimVideoselect.fps);
        sensorScen = sensorSlimVideoselect.scenario;
    }
    else
    {
        MY_LOGE("No sensor scenario FPS >= %d",
                mpParamsManager->getInt(CameraParameters::KEY_PREVIEW_FRAME_RATE));
        sensorScen = SENSOR_SCENARIO_ID_NORMAL_VIDEO;
        return UNKNOWN_ERROR;
    }
    //
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MBOOL
FeatureFlowControl::
needReconstructRecordingPipe()
{
    MBOOL ret = MFALSE;
    MSize paramSize;
    mpParamsManager->getVideoSize(&paramSize.w, &paramSize.h);

    if( (paramSize.w > mRecordRrzoSize.w) || (paramSize.h > mRecordRrzoSize.h) )
    {
        ret = MTRUE;
    }

    //algo requirement
    if( mRecordRrzoSize.w > 2304 || mRecordRrzoSize.h > 1306 )
    {
        if( paramSize.w <= 1920 && paramSize.h <= 1088 )
        {
            ret = MTRUE;
        }
    }

    MY_LOGD("param(%dx%d), rrzo(%dx%d), ret(%d)",
             paramSize.w, paramSize.h,
             mRecordRrzoSize.w, mRecordRrzoSize.h, ret);
    return ret;
}
