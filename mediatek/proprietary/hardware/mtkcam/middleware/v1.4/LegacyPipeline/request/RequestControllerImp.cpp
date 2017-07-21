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

#define LOG_TAG "MtkCam/RequestControllerImp"
//
#include "MyUtils.h"
//
#include <v3/utils/streaminfo/MetaStreamInfo.h>
#include <v3/utils/streaminfo/ImageStreamInfo.h>
#include <v3/utils/streambuf/StreamBuffers.h>

#include <v1/camutils/CamInfo.h>
#include <v1/IParamsManager.h>
#include <LegacyPipeline/request/IRequestController.h>
#include <LegacyPipeline/ILegacyPipeline.h>
#include <LegacyPipeline/request/RequestSettingBuilder.h>
#include "RequestControllerImp.h"
#include <LegacyPipeline/processor/StreamingProcessor.h>
#include <LegacyPipeline/LegacyPipelineBuilder.h>

using namespace NSCam;
using namespace NSCam::v1;
using namespace NSCam::v1::NSLegacyPipeline;
using namespace android;
using namespace NSCam::v3;

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
/******************************************************************************
 *
 ******************************************************************************/
sp< IRequestController >
IRequestController::
createInstance(
    char                       const *name,
    MINT32                     const aCameraId,
    sp<INotifyCallback>        const &rpCamMsgCbInfo,
    sp< IParamsManagerV3 >     const &rpParamsManagerV3
)
{
    return new RequestControllerImp(
                    name,
                    aCameraId,
                    rpCamMsgCbInfo,
                    rpParamsManagerV3
               );
}

/******************************************************************************
 *
 ******************************************************************************/
RequestControllerImp::
RequestControllerImp(
    char                       const *name,
    MINT32                     const aCameraId,
    sp<INotifyCallback>        const &rpCamMsgCbInfo,
    sp< IParamsManagerV3 >     const &rpParamsManagerV3
)
    : mName(name)
    , mCameraId(aCameraId)
    , mTemplateType(1)
    , mLogLevel(1)
    , mpCamMsgCbInfo(rpCamMsgCbInfo)
    , mpParamsManagerV3(rpParamsManagerV3)
{}

/******************************************************************************
 *
 ******************************************************************************/
char const*
RequestControllerImp::
getName() const
{
    return mName;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
RequestControllerImp::
startPipeline(
    MINT32               aStartRequestNumber,
    MINT32               aEndRequestNumber,
    wp<ILegacyPipeline>  apPipeline,
    wp<IRequestUpdater>  apRequestUpdater,
    MINT32               aPipelineMode,
    MINT32               aLoopCnt
)
{
    //FUNC_START;
    MY_LOGD("startPipeline(%d, %d) +", aStartRequestNumber, aEndRequestNumber);

    sp<ILegacyPipeline> pPipeline = apPipeline.promote();
    if ( pPipeline == 0 ) {
        MY_LOGE("invalid LegacyPipeline.");
        return BAD_VALUE;
    }
    //
    if ( mpRequestThread != 0 ) mpRequestThread = NULL;
    if ( mpStreamingProcessor != 0 ) mpStreamingProcessor = NULL;
    if ( mpRequestSettingBuilder != 0 ) mpRequestSettingBuilder = NULL;
    //
    if ( mpRequestSettingBuilder == NULL ) {
        MY_LOGD("Create RequestSettingBuilder.");
        mpRequestSettingBuilder = RequestSettingBuilder::createInstance(
                                            mCameraId,
                                            mpParamsManagerV3
                                        );
    }
    if ( mpCamMsgCbInfo != 0 ) {
        MY_LOGD("Create StreamingProcessor.");
        mpStreamingProcessor = StreamingProcessor::createInstance(
                                                getOpenId(),
                                                mpCamMsgCbInfo,
                                                mpRequestSettingBuilder,
                                                mpParamsManagerV3
                                            );
    } else {
        MY_LOGW("Streaming processor does not create.");
    }
    //
    sp<ResultProcessor> pProcessor = pPipeline->getResultProcessor().promote();
    if ( pProcessor != 0 ) {
        pProcessor->registerListener(
                    aStartRequestNumber,
                    aEndRequestNumber,
                    true,
                    mpStreamingProcessor
                );
    } else {
        MY_LOGE("Cannot get result processor.");
    }

    sp< IRequestUpdater > pRequestUpdater = apRequestUpdater.promote();
    if ( pRequestUpdater == NULL) {
        MY_LOGE("RequestUpdater promote fail.");
        return BAD_VALUE;
    }

    IMetadata setting;
    mpRequestSettingBuilder->createDefaultRequest(mTemplateType, setting);
    pRequestUpdater->updateParameters(&setting);
    mpRequestSettingBuilder->setStreamingRequest(setting);

    mpPipeline = apPipeline;
    //
    MY_LOGD("aPipelineMode(%d)",aPipelineMode);
    switch(aPipelineMode)
    {
        case LegacyPipelineMode_T::PipelineMode_HighSpeedVideo:
        {
            mpRequestThread = new RequestThreadHighSpeedVideo(getOpenId(), this, aLoopCnt);
            break;
        }
        default:
        {
            mpRequestThread = new RequestThread(getOpenId(), this);
            break;
        }
    }
    //
    mpRequestThread->start(
                        apPipeline,
                        apRequestUpdater,
                        aStartRequestNumber,
                        aEndRequestNumber
                    );
    //
    if( mpRequestThread->run(REQUESTCONTROLLER_NAME) != OK ) {
        MY_LOGE("Thread init fail.");
        return BAD_VALUE;
    }
    //
    FUNC_END;

    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
RequestControllerImp::
stopPipeline()
{
    FUNC_START;

    if(mpRequestThread != NULL)
    {
        mpRequestThread->requestExit();
        mpRequestThread = NULL;
    }
    mpRequestSettingBuilder = NULL;
    mpStreamingProcessor = NULL;
    //
    FUNC_END;
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
RequestControllerImp::
autoFocus()
{
    if ( mpStreamingProcessor != 0 ) {
        return mpStreamingProcessor->startAutoFocus();
    }

#warning "FIXME"
    MY_LOGE("Streaming processor does not set.");
    //return UNKNOWN_ERROR;
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
RequestControllerImp::
cancelAutoFocus()
{
    if ( mpStreamingProcessor != 0 ) {
        return mpStreamingProcessor->cancelAutoFocus();
    }
#warning "FIXME"
    MY_LOGE("Streaming processor does not set.");
    //return UNKNOWN_ERROR;
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
RequestControllerImp::
precapture(int& flashRequired)
{
    if ( mpStreamingProcessor != 0 ) {
        return mpStreamingProcessor->preCapture(flashRequired);
    }

    MY_LOGE("Streaming processor does not set.");
    return UNKNOWN_ERROR;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
RequestControllerImp::
setParameters(
    android::wp< IRequestUpdater > pUpdater
)
{
    FUNC_START;

    sp< IRequestUpdater > pRequestUpdater = pUpdater.promote();
    if ( pRequestUpdater == NULL) {
        MY_LOGE("RequestUpdater promote fail.");
        return BAD_VALUE;
    }

    if(mpRequestSettingBuilder != NULL)
    {
        IMetadata setting;
        mpRequestSettingBuilder->createDefaultRequest(mTemplateType, setting);
        pRequestUpdater->updateParameters(&setting);
        mpRequestSettingBuilder->setStreamingRequest(setting);
    }

    FUNC_END;

    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
RequestControllerImp::
sendCommand(
    int32_t cmd,
    int32_t arg1,
    int32_t arg2
)
{
    if ( mpStreamingProcessor == 0 )
    {
        MY_LOGW("Streaming processor does not exist.");
        return INVALID_OPERATION;
    }
    //
    switch (cmd)
    {
        case CAMERA_CMD_START_SMOOTH_ZOOM:
        {
            MY_LOGD("CAMERA_CMD_START_SMOOTH_ZOOM");
            return mpStreamingProcessor->startSmoothZoom(arg1);
        } break;
        case CAMERA_CMD_STOP_SMOOTH_ZOOM:
        {
            MY_LOGD("CAMERA_CMD_STOP_SMOOTH_ZOOM");
            return mpStreamingProcessor->stopSmoothZoom();
        } break;
        case CAMERA_CMD_ENABLE_FOCUS_MOVE_MSG:
        {
            MY_LOGD("CAMERA_CMD_ENABLE_FOCUS_MOVE_MSG");
            return OK;
        } break;
        default:
        {
            //MY_LOGW("unsupported cmd(0x%08X),arg1(0x%08X),arg2(0x%08X)",cmd,arg1,arg2);
            break;
        }
    };

    return INVALID_OPERATION;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
RequestControllerImp::
setRequestType( int type )
{
    mTemplateType = type;
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
RequestControllerImp::
onLastStrongRef( const void* /*id*/)
{
    FUNC_START;

    mpParamsManagerV3       = NULL;
    mpCamMsgCbInfo          = NULL;

    FUNC_END;
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
RequestControllerImp::
dump()
{
#warning "TODO"
}

/******************************************************************************
 *
 ******************************************************************************/
sp< RequestSettingBuilder >
RequestControllerImp::
getRequestSettingBuilder()
{
    return mpRequestSettingBuilder;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
RequestControllerImp::
submitRequest(
    Vector< SettingSet > rvSettings,
    Vector< StreamId >   rvDstStreams,
    Vector< MINT32 >&    rvRequestNo
)
{
    status_t ret = UNKNOWN_ERROR;
    if( mpRequestThread != NULL )
    {
        MY_LOGD("submitRequest when mpRequestThread exist");
        ret = mpRequestThread->submitRequest(
                                rvSettings,
                                rvDstStreams,
                                rvRequestNo
                            );
    }
    else
    {
        MY_LOGD("submitRequest when mpRequestThread doesn't work");
        sp<ILegacyPipeline> pPipeline = mpPipeline.promote();
        if( pPipeline == NULL )
        {
            MY_LOGE("Pipeline doesn't exist, can't submitRequest!");
            ret = UNKNOWN_ERROR;
        }
        else
        {
            if(rvSettings.size() == 0)
            {
                MY_LOGE("rvSettings.size == 0");
                ret = BAD_VALUE;
            }
            else
            {
                int n = rvSettings.size();
                for(int i=0; i<n; i++)
                {
                    MY_LOGD("submitRequest (%d)",i);
                    ret = pPipeline->submitRequest(
                                    i,
                                    rvSettings.editItemAt(i).appSetting,
                                    rvSettings.editItemAt(i).halSetting,
                                    rvDstStreams
                                    );
                    if(ret != OK)
                    {
                        MY_LOGE("submitRequest faile in request(%d)",i);
                        break;
                    }
                    rvRequestNo.push_back(i);
                }
            }
        }
    }
    return ret;
}
