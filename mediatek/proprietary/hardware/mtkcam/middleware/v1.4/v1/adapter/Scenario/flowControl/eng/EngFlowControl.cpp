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
#include "EngFlowControl.h"
//
#include <camera_custom_nvram.h>
//
#include <LegacyPipeline/StreamId.h>
//
#define SUPPORT_3A (1)
//
#if SUPPORT_3A
#include <Hal3/IHal3A.h>
//using namespace NS3A;
using namespace NS3Av3;

//#include <Callback/ICallBack3A.h>
#endif
//



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

#if 1
#define FUNC_START     MY_LOGD("+")
#define FUNC_END       MY_LOGD("-")
#else
#define FUNC_START
#define FUNC_END
#endif
//
#define CHECK_OBJECT(x)  do{                                        \
    if (x == nullptr) { MY_LOGE("Null %s Object", #x); return MFALSE;} \
} while(0)


/*******************************************************************************
*
********************************************************************************/
MetaListener::
MetaListener(
char const* pcszName,
MINT32 const i4OpenId,
sp<IParamsManager> pParamsManager
)
: mName(pcszName),
mOpenId(i4OpenId),
mpParamsManager(pParamsManager)
{
}

/*******************************************************************************
*
********************************************************************************/
void MetaListener::
onResultReceived(
MUINT32         const requestNo,
StreamId_T      const streamId,
MBOOL           const errorResult,
IMetadata       const result
)
{
#if SUPPORT_3A
    {
        MINT32 iAECurrentEV;
        MUINT32 iOutLen;
        iAECurrentEV = 0;
        //
        AE_NVRAM_T AENVRAM;
        AENVRAM.rCCTConfig.i4BVOffset = 0;
        //
        IHal3A* pHal3a = IHal3A::createInstance(IHal3A::E_Camera_3, mOpenId, mName);
        if ( ! pHal3a )
        {
            MY_LOGE("pHal3a == NULL");
        }
        //
        // update preview real time params value (3A --> AP)
        FrameOutputParam_T RTParams;
        if ( ! pHal3a->send3ACtrl(NS3Av3::E3ACtrl_GetRTParamsInfo, reinterpret_cast<MINTPTR>(&RTParams), 0) )
        {
            MY_LOGE("getSupportedParams fail");
        }
        // BV value
        int rt_BV = RTParams.i4BrightValue_x10;
        //
        //Get AE index, shutter, isp_gain, sensor_gain
        int index, shutter, isp_gain, sensor_gain, iso;
        index = RTParams.u4AEIndex;
        shutter = RTParams.u4PreviewShutterSpeed_us;
        isp_gain = RTParams.u4PreviewISPGain_x1024;
        sensor_gain = RTParams.u4PreviewSensorGain_x1024;
        iso = RTParams.u4RealISOValue;
        //
        // EV Calibration
        pHal3a->send3ACtrl(NS3Av3::E3ACtrl_GetCurrentEV, reinterpret_cast<MINTPTR>(&iAECurrentEV), reinterpret_cast<MINTPTR>(&iOutLen));
        //
        pHal3a->send3ACtrl(NS3Av3::E3ACtrl_GetNVRAMParam, reinterpret_cast<MINTPTR>(&AENVRAM), reinterpret_cast<MINTPTR>(&iOutLen));
        //
        if (mpParamsManager != NULL)
        {
            mpParamsManager->updateBrightnessValue(rt_BV);// BV value
            mpParamsManager->updatePreviewAEIndexShutterGain(index, shutter, isp_gain, sensor_gain, iso);//preview AE params
            //
            mpParamsManager->set(MtkCameraParameters::KEY_ENG_EV_CALBRATION_OFFSET_VALUE, iAECurrentEV + AENVRAM.rCCTConfig.i4BVOffset);
            mpParamsManager->getStr(MtkCameraParameters::KEY_ENG_EV_CALBRATION_OFFSET_VALUE).string();
        }
        else
        {
            // Show some message for spParamsMgr is NULL
            MY_LOGE("[Engflowcontrol] - onResultReceived - spParamsMgr is NULL");
        }
        pHal3a->destroyInstance(LOG_TAG);
    }
#endif
    //
}

/*******************************************************************************
*
********************************************************************************/
String8 MetaListener::
getUserName()
{
    return String8(LOG_TAG);
}

/******************************************************************************
 *
 ******************************************************************************/
EngFlowControl::
EngFlowControl(
    char const*                 pcszName,
    MINT32 const                i4OpenId,
    sp<IParamsManager>          pParamsManager,
    sp<ImgBufProvidersManager>  pImgBufProvidersManager,
    sp<INotifyCallback>         pCamMsgCbInfo
)
    : mpParamsManager(pParamsManager)
    , mpImgBufProvidersMgr(pImgBufProvidersManager)
	, mpCamMsgCbInfo(pCamMsgCbInfo)
    , mOpenId(i4OpenId)
    , mName(const_cast<char*>(pcszName))
{
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
EngFlowControl::
getName()   const
{
    return mName;
}

/******************************************************************************
 *
 ******************************************************************************/
int32_t
EngFlowControl::
getOpenId() const
{
    return mOpenId;
}


/******************************************************************************
 *
 ******************************************************************************/
status_t
EngFlowControl::
startPreview()
{
    FUNC_START;
    mpRequestController = IRequestController::createInstance(
                                                mName,
                                                mOpenId,
                                                mpCamMsgCbInfo,
                                                mpParamsManagerV3
                                            );
    if ( mpParamsManager->getRecordingHint() )
    {
        mPipelineMode = PipelineMode_RECORDING;
        constructRecordingPipeline();
        mpRequestController->setRequestType(CAMERA3_TEMPLATE_VIDEO_RECORD);
    } else if ( PARAMSMANAGER_MAP_INST(eMapAppMode)->stringFor(mpParamsManager->getHalAppMode())
                ==  MtkCameraParameters::APP_MODE_NAME_MTK_ZSD)
    {
        mPipelineMode = PipelineMode_ZSD;
        constructZsdPreviewPipeline();
        mpRequestController->setRequestType(CAMERA3_TEMPLATE_ZERO_SHUTTER_LAG);
    }
    else
    {
        mPipelineMode = PipelineMode_PREVIEW;
        constructNormalPreviewPipeline();
        mpRequestController->setRequestType(MTK_CONTROL_CAPTURE_INTENT_PREVIEW);
    }

    if ( mpPipeline == 0 ) {
        MY_LOGE("Cannot get pipeline. start preview fail.");
        return BAD_VALUE;
    }
    //

    MERROR ret = mpRequestController->startPipeline(
                                    0,/*start*/
                                    1000, /*end*/
                                    mpPipeline,
                                    this
                                );
    //
    //EV calibration
    {
        sp<ResultProcessor> pResultProcessor = mpPipeline->getResultProcessor().promote();
        CHECK_OBJECT(pResultProcessor);
        //partial
        mpListener = new MetaListener(mName, mOpenId, mpParamsManager);
        //
        pResultProcessor->registerListener(eSTREAMID_META_APP_DYNAMIC_P1, mpListener);
        //
    }
    //
    FUNC_END;
    return ret;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
EngFlowControl::
stopPreview()
{
    FUNC_START;
    mpRequestController->stopPipeline();
    mpRequestController = NULL;

    if ( mpPipeline != 0 ) {
        mpPipeline->flush();
        mpPipeline->waitUntilDrained();
        mpPipeline = NULL;
    }
    FUNC_END;
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
EngFlowControl::
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
EngFlowControl::
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
     MERROR ret = mpRequestController->startPipeline(
                                     0,/*start*/
                                     1000, /*end*/
                                     mpPipeline,
                                     this
                                 );
     //
     FUNC_END;
     return ret;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
EngFlowControl::
startRecording()
{

#if 0
    if ( mPipelineMode != PipelineMode_T::RECORDING ){
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


    return mpRequestController->startPipeline(
                                    1001,/*start*/
                                    2000, /*end*/
                                    mpPipeline,
                                    this
                                );
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
EngFlowControl::
stopRecording()
{
/*
    if( mpRequestController != 0 )
        mpRequestController->stopPipeline();

    if ( mpPipeline != 0 ) {
        mpPipeline->flush();
        mpPipeline->waitUntilDrained();
        mpPipeline = NULL;
    }
*/
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
EngFlowControl::
autoFocus()
{
    return (mpRequestController != 0) ?
        mpRequestController->autoFocus() : OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
EngFlowControl::
cancelAutoFocus()
{
    return (mpRequestController != 0) ?
        mpRequestController->cancelAutoFocus() : OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
EngFlowControl::
precapture(int& flashRequired)
{
    return (mpRequestController != 0) ?
        mpRequestController->precapture(flashRequired) : OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
EngFlowControl::
setParameters()
{
    FUNC_START;

#if SUPPORT_3A

    IHal3A* pHal3a = IHal3A::createInstance(IHal3A::E_Camera_3, getOpenId(), getName());
    if ( ! pHal3a )
    {
        MY_LOGE("pHal3a == NULL");
        return INVALID_OPERATION;
    }
    //
    pHal3a->send3ACtrl(NS3Av3::E3ACtrl_SetOperMode,NSIspTuning::EOperMode_EM, 0);
    //
    pHal3a->send3ACtrl(NS3Av3::E3ACtrl_SetAECamMode, eAppMode_EngMode, 0);
    //
    //AF Manual focus position
    MINT32 i4MFPos = mpParamsManager->getInt(MtkCameraParameters::KEY_FOCUS_ENG_STEP);
    pHal3a->send3ACtrl(NS3Av3::E3ACtrl_SetAFMFPos, i4MFPos, NULL);
    MY_LOGD("i4MFPos(%d)", i4MFPos);
    //
    //AF Full Scan
    MINT32 i4FullScanStep = mpParamsManager->getInt(MtkCameraParameters::KEY_ENG_FOCUS_FULLSCAN_FRAME_INTERVAL);
    if(::strcmp(mpParamsManager->getStr(CameraParameters::KEY_FOCUS_MODE), "fullscan") == 0)
    {
        i4FullScanStep = ((i4FullScanStep & 0xFFFF) << 16) + ((1) & 0xFFFF);
        MY_LOGD("i4FullScanStep(%d)", i4FullScanStep);
        pHal3a->send3ACtrl(NS3Av3::E3ACtrl_SetAFFullScanStep, i4FullScanStep, NULL);
    }
    else
    {
        i4FullScanStep = 0;
        pHal3a->send3ACtrl(NS3Av3::E3ACtrl_SetAFFullScanStep, i4FullScanStep, NULL);
    }
    //
    // AWB enable
    MUINT32 u4AWBEnable = mpParamsManager->getInt(MtkCameraParameters::KEY_ENG_MTK_AWB_ENABLE) == 1 ?
                              MTRUE: MFALSE;
    pHal3a->send3ACtrl(NS3Av3::E3ACtrl_SetAwbBypCalibration, u4AWBEnable, 0 );
    //
    //shading
    int save_shading = mpParamsManager->getInt(MtkCameraParameters::KEY_ENG_SAVE_SHADING_TABLE);
    String8 ms3ACollectionFilePath;
    if (1 == save_shading)
    {
        String8 ms8RawFilePath(mpParamsManager->getStr(MtkCameraParameters::KEY_RAW_PATH)); // => /storage/sdcard1/DCIM/CameraEM/Preview01000108ISO0.raw
        ms8RawFilePath = ms8RawFilePath.getBasePath(); // => /storage/sdcard1/DCIM/CameraEM/Preview01000108ISO0

        MY_LOGD("shading_table_path = %s", ms8RawFilePath.string());
        pHal3a->send3ACtrl(NS3Av3::E3ACtrl_SetShadingSdblkCfg, MTRUE, reinterpret_cast<MINTPTR>(ms8RawFilePath.string()));
    }
    else
    {
        String8 ms8RawFilePath("");
        pHal3a->send3ACtrl(NS3Av3::E3ACtrl_SetShadingSdblkCfg, MFALSE, reinterpret_cast<MINTPTR>(ms8RawFilePath.string()));
    }
    // MTK Shading Enable
    MUINT32 u4ShadingEnable = mpParamsManager->getInt(MtkCameraParameters::KEY_ENG_MTK_SHADING_ENABLE) == 1 ?
                                             MTRUE : MFALSE;
    //shading table select
    int shading_table = mpParamsManager->getInt(MtkCameraParameters::KEY_ENG_SHADING_TABLE);
    //
    (u4ShadingEnable == 0) ?
        (pHal3a->send3ACtrl(NS3Av3::E3ACtrl_SetShadingEngMode, u4ShadingEnable, NULL)):
        (pHal3a->send3ACtrl(NS3Av3::E3ACtrl_SetShadingEngMode, (shading_table = shading_table + 1), NULL));
    MY_LOGD("save_shading=%d,u4ShadingEnable = %d,shading_table = %d", save_shading, u4ShadingEnable, shading_table);
    //
    // MTK shading 1 to 3 enable
    MUINT32 u4Shading123Enable = mpParamsManager->getInt(MtkCameraParameters::KEY_ENG_MTK_1to3_SHADING_ENABLE) == 1 ?
                                                   MFALSE : MTRUE;
    pHal3a->send3ACtrl(NS3Av3::E3ACtrl_SetShadingByp123, u4Shading123Enable, 0 );

    // Sensor OTP
    IHalSensorList* const pHalSensorList = IHalSensorList::get();
    IHalSensor* pSensorObj = NULL;
    if( pHalSensorList != NULL )
    {
        pSensorObj = pHalSensorList->createSensor(LOG_TAG, getOpenId());
    }
    else
    {
        MY_LOGE("pHalSensorList == NULL");
    }
    // Sensor AWB OTP
    MUINT32 u4SensorAWBOTPEn = mpParamsManager->getInt(MtkCameraParameters::KEY_ENG_SENSOR_AWB_ENABLE);
    pSensorObj->sendCommand(pHalSensorList->querySensorDevIdx(getOpenId()),
                                                SENSOR_CMD_SET_SENSOR_OTP_AWB_CMD,(MUINTPTR)&u4SensorAWBOTPEn,0,0);
    // Sensor Shading OTP
    MUINT32 u4SensorLSCOTPEn = mpParamsManager->getInt(MtkCameraParameters::KEY_ENG_SENSOR_SHADNING_ENABLE);
    pSensorObj->sendCommand(pHalSensorList->querySensorDevIdx(getOpenId()),
                                                SENSOR_CMD_SET_SENSOR_OTP_LSC_CMD,(MUINTPTR)&u4SensorLSCOTPEn, 0, 0);
    //
    if(pSensorObj)
    {
        pSensorObj->destroyInstance(LOG_TAG);
        pSensorObj = NULL;
    }
    //
    MY_LOGD("i4MFPos= %d,i4FullScanStep= %d,u4AWBEnable=%d,u4Shading123Enable=%d,u4SensorAWBOTPEn=%d,u4SensorLSCOTPEn=%d",
             i4MFPos, i4FullScanStep, u4AWBEnable, u4Shading123Enable, u4SensorAWBOTPEn, u4SensorLSCOTPEn );
    //
    pHal3a->destroyInstance(LOG_TAG);

#endif

    FUNC_END;

    return (mpRequestController != 0) ?
        mpRequestController->setParameters( this ) : OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
EngFlowControl::
sendCommand(
    int32_t cmd,
    int32_t arg1,
    int32_t arg2
)
{
    return (mpRequestController != 0) ?
        mpRequestController->sendCommand( cmd, arg1, arg2 ) : OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
EngFlowControl::
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
EngFlowControl::
updateParameters(
    IMetadata* setting
)
{
    status_t ret;
    if( mpParamsManagerV3 != 0 )
    {
       ret = mpParamsManagerV3->updateRequest(setting, mSensorParam.mode);
       ret = mpParamsManagerV3->updateRequestEng(setting);

       return ret;
    }
    else
    {
        MY_LOGE("init in thread failed");
        goto lbExit;
    }

lbExit:
    return UNKNOWN_ERROR;

}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
EngFlowControl::
updateRequestSetting(
    IMetadata* appSetting,
    IMetadata* halSetting
)
{
    // update app control

    // update hal control
    {
        IMetadata::IEntry entry(MTK_HAL_REQUEST_SENSOR_SIZE);
        entry.push_back(mSensorParam.size, Type2Type< MSize >());
        halSetting->update(entry.tag(), entry);
    }
    // update default HAL settings
    mpParamsManagerV3->updateRequestHal(halSetting);
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
EngFlowControl::
submitRequest(
    Vector< SettingSet > rvSettings,
    Vector< StreamId >   rvDstStreams,
    Vector< MINT32 >&    rvRequestNo
)
{

    return (mpRequestController != 0) ?
        mpRequestController->submitRequest( rvSettings, rvDstStreams, rvRequestNo ) : UNKNOWN_ERROR;
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
EngFlowControl::
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
