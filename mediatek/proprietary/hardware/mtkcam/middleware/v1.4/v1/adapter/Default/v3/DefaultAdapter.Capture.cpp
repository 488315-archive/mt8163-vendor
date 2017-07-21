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

#define LOG_TAG "MtkCam/DefaultAdapter"
//

#include "MyUtils.h"
#include <camera/MtkCamera.h>
//
#include <inc/CamUtils.h>
using namespace android;
using namespace MtkCamUtils;
//
#include <inc/ImgBufProvidersManager.h>
//
#include <LegacyPipeline/StreamId.h>
#include <LegacyPipeline/IResourceContainer.h>
#include <LegacyPipeline/buffer/StreamBufferProviderFactory.h>
#include <v1/IParamsManager.h>
#include <v1/IParamsManagerV3.h>
#include <v1/ICamAdapter.h>
#include <inc/BaseCamAdapter.h>
#include <buffer/ClientBufferPool.h>
//
#include "inc/v3/DefaultAdapter.h"
using namespace NSDefaultAdapter;
//
#include <hardware/camera3.h>
//
#include <LegacyPipeline/IResourceContainer.h>
#include <metadata/mtk_platform_metadata_tag.h>

#include <IHalSensor.h>

/******************************************************************************
*
*******************************************************************************/
#define MY_LOGV(fmt, arg...)        CAM_LOGV("(%d)(%d)(%s)[%s] " fmt, ::gettid(), getOpenId(), getName(), __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_LOGD("(%d)(%d)(%s)[%s] " fmt, ::gettid(), getOpenId(), getName(), __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_LOGI("(%d)(%d)(%s)[%s] " fmt, ::gettid(), getOpenId(), getName(), __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_LOGW("(%d)(%d)(%s)[%s] " fmt, ::gettid(), getOpenId(), getName(), __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_LOGE("(%d)(%d)(%s)[%s] " fmt, ::gettid(), getOpenId(), getName(), __FUNCTION__, ##arg)
#define MY_LOGA(fmt, arg...)        CAM_LOGA("(%d)(%d)(%s)[%s] " fmt, ::gettid(), getOpenId(), getName(), __FUNCTION__, ##arg)
#define MY_LOGF(fmt, arg...)        CAM_LOGF("(%d)(%d)(%s)[%s] " fmt, ::gettid(), getOpenId(), getName(), __FUNCTION__, ##arg)
//
#define MY_LOGV_IF(cond, ...)       do { if ( (cond) ) { MY_LOGV(__VA_ARGS__); } }while(0)
#define MY_LOGD_IF(cond, ...)       do { if ( (cond) ) { MY_LOGD(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) ) { MY_LOGI(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) ) { MY_LOGW(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) ) { MY_LOGE(__VA_ARGS__); } }while(0)
#define MY_LOGA_IF(cond, ...)       do { if ( (cond) ) { MY_LOGA(__VA_ARGS__); } }while(0)
#define MY_LOGF_IF(cond, ...)       do { if ( (cond) ) { MY_LOGF(__VA_ARGS__); } }while(0)
#define FUNC_START                  MY_LOGD("+")
#define FUNC_END                    MY_LOGD("-")


/******************************************************************************
*   Function Prototype.
*******************************************************************************/
//
bool
createShotInstance(
    sp<IShot>&          rpShot,
    uint32_t const      u4ShotMode,
    int32_t const       i4OpenId,
    sp<IParamsManager>  pParamsMgr
);
//

/******************************************************************************
*
*******************************************************************************/
void
CamAdapter::
updateShotMode()
{
    String8 s8ShotMode = getParamsManager()->getShotModeStr();
    uint32_t u4ShotMode = getParamsManager()->getShotMode();
    {
        if (msAppMode == MtkCameraParameters::APP_MODE_NAME_MTK_ZSD)
        {
            if( u4ShotMode == eShotMode_ContinuousShot )
            {
                s8ShotMode = "ZSD CShot";
                u4ShotMode = eShotMode_ContinuousShot;
            }
            else
            {
                s8ShotMode = "ZSD";
                u4ShotMode = NSCam::eShotMode_ZsdShot;
            }
        }
    }
    if( mpStateManager->isState(IState::eState_Recording))
    {
        s8ShotMode = "VSS";
        u4ShotMode = eShotMode_VideoSnapShot;
    }
    //
    mShotMode = u4ShotMode;
    MY_LOGI("<shot mode> %#x(%s)", mShotMode, s8ShotMode.string());
    return;
}

/******************************************************************************
*   Before CamAdapter::takePicture() -> IState::onCapture()
*
*   To determine if using Multi Frame Noise Reduction based on the current ISO/EXP,
*   if yes, mShotMode will be updated to
*     1. eShotMode_NormalShot  -->  eShotMode_MfllShot
*     2. eShotMode_ZsdShot     -->  eShotMode_ZsdMfllShot
*******************************************************************************/
void
CamAdapter::
updateShotModeForMultiFrameBlending()
{
#if MTKCAM_HAVE_MFB_SUPPORT
    int mfbMode = 0; // Normal
    int doMfb = 0; // No need

    // Determine if it's necessary to do Multi-frame Noise Reduction, because
    // MFNR has no scene mode for UI which means, MFNR should be auto truned
    // on while capturing with NormalShot or ZsdShot.
    // TODO: Integrate with FaceBeautyShot
    if (mShotMode == eShotMode_NormalShot ||
        mShotMode == eShotMode_ZsdShot)
    {
        // 0: No, 1: MFNR, 2: AIS
        mfbMode = getParamsManager()->getMultFrameBlending();
        if (getParamsManager()->getMultFrameBlending() <= 0)
            return;

        // TODO: check if necessary to do MFNR by checking the current
        //       ISO and exposure
        doMfb = 1; // FIXME: force to 1 for AIS/night mode now

        if (doMfb == 0)
            return;

        switch (mShotMode)
        {
            case eShotMode_NormalShot:
                mShotMode = eShotMode_MfllShot;
                MY_LOGD("<shot mode> update shot mode to MfllShot");
                break;
            case eShotMode_ZsdShot:
                mShotMode = eShotMode_ZsdMfllShot;
                MY_LOGD("<shot mode> update shot mode to ZsdMfllShot");
                break;
            default:
                MY_LOGE("Not support eShotMode(%d)", mShotMode);
                break;
        }
    }
    return;
#endif
}

/******************************************************************************
*
*******************************************************************************/
bool
CamAdapter::
updateShotInstance()
{
    MY_LOGI("<shot mode> %#x", mShotMode);
    return  createShotInstance(mpShot,
                                (msAppMode == MtkCameraParameters::APP_MODE_NAME_MTK_ENG)?(NSCam::eShotMode_EngShot):mShotMode,
                                getOpenId(), getParamsManager());
}


/******************************************************************************
*
*******************************************************************************/
status_t
CamAdapter::
prepareToCapture()
{
    FUNC_START;
    status_t status = NO_ERROR;
    sp<ZipImageCallbackThread> pCallbackThread;
    //
    pCallbackThread = new ZipImageCallbackThread(
                            mpStateManager,
                            mpCamMsgCbInfo);
    //
    if  ( !pCallbackThread.get() || OK != (status = pCallbackThread->run() ) )
    {
        MY_LOGE(
            "Fail to run CallbackThread - CallbackThread.get(%p), status[%s(%d)]",
            pCallbackThread.get(), ::strerror(-status), -status
        );
        goto lbExit;
    }
    mpZipCallbackThread = pCallbackThread;
    //
lbExit:
     FUNC_END;
    return status;
}


/******************************************************************************
*
*******************************************************************************/
bool
CamAdapter::
waitForShotDone()
{
    FUNC_START;
    sp<ZipImageCallbackThread> pCallbackThread = mpZipCallbackThread.promote();
    //  wait for capture callback is done
    if  ( pCallbackThread.get() )
    {
        //post a fake data to make sure thread can exit
        ZipImageCallbackThread::callback_data callbackData;
        callbackData.type = ZipImageCallbackThread::callback_type_image;
        callbackData.pImage = NULL;
        callbackData.u4CallbackIndex = 0;
        callbackData.isFinalImage = true;
        pCallbackThread->addCallbackData(&callbackData);
        MY_LOGD("wait for capture callback done +");
        pCallbackThread->join();
        MY_LOGD("wait for capture callback done -");
        pCallbackThread = 0;
        mpZipCallbackThread = 0;
    }
    FUNC_END;
    return true;
}


/******************************************************************************
*
*******************************************************************************/
bool
CamAdapter::
isTakingPicture() const
{
    FUNC_START;
    bool ret =  mpStateManager->isState(IState::eState_PreCapture) ||
                mpStateManager->isState(IState::eState_NormalCapture) ||
                mpStateManager->isState(IState::eState_ZSLCapture) ||
                mpStateManager->isState(IState::eState_VideoSnapshot);
    if  ( ret )
    {
        MY_LOGD("isTakingPicture(1):%s", mpStateManager->getCurrentState()->getName());
    }
    //
    FUNC_END;
    return  ret;
}

/******************************************************************************
*
*******************************************************************************/
bool
CamAdapter::
isSupportVideoSnapshot()
{
    FUNC_START;
    if( ::strcmp(mspParamsMgr->getStr(MtkCameraParameters::KEY_VIDEO_FRAME_FORMAT), MtkCameraParameters::PIXEL_FORMAT_BITSTREAM) == 0 ||
            mspParamsMgr->getInt(CameraParameters::KEY_PREVIEW_FRAME_RATE) > 30)
    {
        return false;
    }
    else
    {
        return true;
    }
    FUNC_END;
}

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
        //MY_LOGE("pMetadata == NULL");
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
* Normal Capture: Preview -> PreCapture->Idle
* ZSD Capture: Preview -> Capture
* Recording capture: Record -> Capture
*******************************************************************************/
status_t
CamAdapter::
takePicture()
{
    FUNC_START;
    CAM_TRACE_NAME("Adapter:takePicture");
    status_t status = OK;
    //
    if (isTakingPicture())
    {
        MY_LOGW("already taking picture...");
        return status;
    }
    //
    if( OK != (status = prepareToCapture()) )
    {
        MY_LOGE("failed to prepareToCapture");
        goto lbExit;
    }
    //
    updateShotMode();
    //
    {
        //get flash info
        mbFlashOn = MFALSE;
        sp<IFrameInfo> pFrameInfo = IResourceContainer::getInstance(getOpenId())->queryLatestFrameInfo();
        if(pFrameInfo==NULL)
        {
            MY_LOGW("failed to queryLatestFrameInfo");
        }
        else
        {
            IMetadata meta;
            pFrameInfo->getFrameMetadata(meta);
            MUINT8 AeState = 0;
            tryGetMetadata< MUINT8 >(&meta, MTK_CONTROL_AE_STATE, AeState);
            MUINT8 AeMode = 0;
            tryGetMetadata< MUINT8 >(&meta, MTK_CONTROL_AE_MODE, AeMode);
            if( AeMode == MTK_CONTROL_AE_MODE_ON_ALWAYS_FLASH ||
                (AeMode == MTK_CONTROL_AE_MODE_ON_AUTO_FLASH && AeState == MTK_CONTROL_AE_STATE_FLASH_REQUIRED) )
            {
                mbFlashOn = MTRUE;
            }
            MY_LOGD("AE Mode(%d) AE State(%d), mbFlashOn(%d)", AeMode, AeState, mbFlashOn);
        }
    }
    //
    // State:Recording->VideoSnapShot
    if(mpStateManager->isState(IState::eState_Recording))
    {
        if (isSupportVideoSnapshot())
        {
            status = mpStateManager->getCurrentState()->onCapture(this);
            if (OK != status)
            {
                goto lbExit;
            }
        }
        else
        {
            MY_LOGE("Not support VSS");
            status = INVALID_OPERATION;
            goto lbExit;
        }
    }
    else
    {
        //  ZSD Capture, direct capture Preview -> TakePicture
        if (msAppMode == MtkCameraParameters::APP_MODE_NAME_MTK_ZSD)
        {
            if( mbFlashOn )
            {
                MY_LOGD("Need to PreCapture in ZSD");
                status = mpStateManager->getCurrentState()->onPreCapture(this);
                if (OK != status)
                {
                    MY_LOGE("onPreCapture Fail!");
                    goto lbExit;
                }
                mpStateManager->transitState(IState::eState_Preview);
            }
            else
            {
                // MFNR is only applied when no flash
                updateShotModeForMultiFrameBlending();
            }
            //
            status = mpStateManager->getCurrentState()->onCapture(this);
            if(OK != status)
            {
                goto lbExit;
            }
        }
        else // Normal Capture, Preview -> preCapture- > Idle
        {
            if( (!mbFlashOn) && (mShotMode == eShotMode_ContinuousShot) )
            {
                MY_LOGD("Skip PreCapture in normal");
                mpStateManager->transitState(IState::eState_PreCapture);
            }
            else
            {
                status = mpStateManager->getCurrentState()->onPreCapture(this);
                if (OK != status)
                {
                    goto lbExit;
                }
            }
            //
            status = mpStateManager->getCurrentState()->onStopPreview(this);
            if (OK != status)
            {
                goto lbExit;
            }
            // MFNR is only applied when no flash
            if (!mbFlashOn)
                updateShotModeForMultiFrameBlending();
            //
            status = mpStateManager->getCurrentState()->onCapture(this);
            if (OK != status)
            {
                goto lbExit;
            }
        }
    }
    //
lbExit:
    FUNC_END;
    return status;
}


/******************************************************************************
*
*******************************************************************************/
status_t
CamAdapter::
cancelPicture()
{
    FUNC_START;
    CAM_TRACE_NAME("Adapter:cancelPicture");
    if( mpStateManager->isState(IState::eState_NormalCapture) ||
        mpStateManager->isState(IState::eState_ZSLCapture) ||
        mpStateManager->isState(IState::eState_VideoSnapshot))
    {
        mpStateManager->getCurrentState()->onCancelCapture(this);
    }
    FUNC_END;
    return OK;
}


/******************************************************************************
*
*******************************************************************************/
status_t
CamAdapter::
setCShotSpeed(int32_t i4CShotSpeed)
{
    FUNC_START;
    if(i4CShotSpeed <= 0 )
    {
        MY_LOGE("cannot set continuous shot speed as %d fps)", i4CShotSpeed);
        return BAD_VALUE;
    }

    sp<IShot> pShot = mpShot;
    if( pShot != 0 )
    {
        pShot->sendCommand(eCmd_setCShotSpeed, i4CShotSpeed, 0);
    }
    FUNC_END;
    return OK;
}


/******************************************************************************
*   CamAdapter::takePicture() -> IState::onCapture() ->
*   (Idle)IStateHandler::onHandleNormalCapture() -> CamAdapter::onHandleNormalCapture()
*******************************************************************************/
status_t
CamAdapter::
onHandleNormalCapture()
{
    FUNC_START;
    CAM_TRACE_NAME("Adapter:onHandleNormalCapture");
    status_t status = DEAD_OBJECT;
    sp<ZipImageCallbackThread> pCallbackThread = mpZipCallbackThread.promote();
    sp<ICaptureCmdQueThread> pCaptureCmdQueThread = mpCaptureCmdQueThread;
    //
    if( !pCallbackThread.get() )
    {
        MY_LOGE("no callback thread");
        goto lbExit;
    }
    //
    pCallbackThread->setShotMode(mShotMode, IState::eState_Idle, msAppMode);
    pCallbackThread = 0;
    //
    if  ( pCaptureCmdQueThread != 0 ) {
        status = pCaptureCmdQueThread->onCapture();
    }
    //
lbExit:
    FUNC_END;
    return  status;
}

/******************************************************************************
*
*******************************************************************************/
int
CamAdapter::
getFlashQuickCalibrationResult()
{
    MY_LOGD("+");
    //
    #if '1'==MTKCAM_HAVE_3A_HAL
    //
    IHal3A* pHal3a = IHal3A::createInstance(IHal3A::E_Camera_1, getOpenId(), getName());

    if ( ! pHal3a )
    {
        MY_LOGE("pHal3a == NULL");
        return 1;
    }
    int result = pHal3a->getFlashQuickCalibrationResult();

    pHal3a->destroyInstance(getName());
    #endif
    //
    MY_LOGD("-");
    return OK;
    //return result;

}


/******************************************************************************
*
*******************************************************************************/
status_t
CamAdapter::
onHandleCaptureDone()
{
    CAM_TRACE_NAME("Adapter:onHandleCaptureDone");
    //
    FUNC_START;
    if (mpStateManager->isState(IState::eState_NormalCapture))
    {
        return onHandleNormalCaptureDone();
    }
    else if (mpStateManager->isState(IState::eState_ZSLCapture))
    {
        return onHandleZSLCaptureDone();
    }
    else if (mpStateManager->isState(IState::eState_VideoSnapshot))
    {
        return onHandleVideoSnapshotDone();
    }

    FUNC_END;
    return  OK;
}


/******************************************************************************
*
*******************************************************************************/
status_t
CamAdapter::
onHandleNormalCaptureDone()
{
    CAM_TRACE_NAME("Adapter:onHandleNormalCaptureDone");
    //
    FUNC_START;
    mpStateManager->transitState(IState::eState_Idle);
    FUNC_END;
    return OK ;
}


/******************************************************************************
*   CamAdapter::cancelPicture() -> IState::onCancelCapture() ->
*   IStateHandler::onHandleCancelCapture() -> CamAdapter::onHandleCancelCapture()
*******************************************************************************/
status_t
CamAdapter::
onHandleCancelCapture()
{
    CAM_TRACE_NAME("Adapter:onHandleCancelCapture");
    FUNC_START;
    sp<IShot> pShot = mpShot;
    if  ( pShot != 0 )
    {
        pShot->sendCommand(eCmd_cancel);
    }
    //
    if (mpStateManager->isState(IState::eState_ZSLCapture))
    {
        onHandleCancelZSLCapture();
    }
    else if (mpStateManager->isState(IState::eState_NormalCapture))
    {
        onHandleCancelNormalCapture();
    }
    else if (mpStateManager->isState(IState::eState_VideoSnapshot))
    {
        onHandleCancelVideoSnapshot();
    }
    FUNC_END;
    return  OK;
}


/******************************************************************************
*   CamAdapter::takePicture() -> IState::onCapture() ->
*   (Preview)IStateHandler::onHandleZSLCapture() -> CamAdapter::onHandleZSLCapture()
*******************************************************************************/
status_t
CamAdapter::
onHandleZSLCapture()
{
    FUNC_START;
    CAM_TRACE_NAME("Adapter:onHandleZSLCapture");
    status_t status = DEAD_OBJECT;
    sp<ZipImageCallbackThread> pCallbackThread = mpZipCallbackThread.promote();
    sp<ICaptureCmdQueThread> pCaptureCmdQueThread = mpCaptureCmdQueThread;
    //
    if( !pCallbackThread.get() )
    {
        MY_LOGE("no callback thread");
        goto lbExit;
    }
    //
    pCallbackThread->setShotMode(mShotMode, IState::eState_Preview, msAppMode);
    pCallbackThread = 0;
    //
    if  ( pCaptureCmdQueThread != 0 ) {
        status = pCaptureCmdQueThread->onCapture();
    }
    else
    {
        MY_LOGE("pCaptureCmdQueThread == NULL");
    }
    //
lbExit:
    FUNC_END;
    return  status;

}


/******************************************************************************
*
*******************************************************************************/
status_t
CamAdapter::
onHandleZSLCaptureDone()
{
    //
    FUNC_START;
    if (mpStateManager->isState(IState::eState_ZSLCapture))
    {
        if( mpFlowControl.get() != NULL )
        {
            mpFlowControl->changeToPreviewStatus();
        }
        else
        {
            MY_LOGE("get Flow Control fail");
        }
        mpStateManager->transitState(IState::eState_Preview);
    }
    else
    {
        MY_LOGW("Not in IState::ZSLCapture !!");
    }
    FUNC_END;
    return  OK;
}


/******************************************************************************
*   CamAdapter::cancelPicture() -> IState::onCancelCapture() ->
*   IStateHandler::onHandleCancelCapture() -> CamAdapter::onHandleCancelCapture()
*******************************************************************************/
status_t
CamAdapter::
onHandleCancelPreviewCapture()
{
    return  onHandleCancelCapture();
}


/******************************************************************************
*
*
*******************************************************************************/
status_t
CamAdapter::
onHandleVideoSnapshot()
{
    FUNC_START;
    CAM_TRACE_NAME("Adapter:onHandleVideoSnapshot");
    status_t status = DEAD_OBJECT;
    sp<ZipImageCallbackThread> pCallbackThread = mpZipCallbackThread.promote();
    sp<ICaptureCmdQueThread> pCaptureCmdQueThread = mpCaptureCmdQueThread;
    //
    if( !pCallbackThread.get() )
    {
        MY_LOGE("no callback thread");
        goto lbExit;
    }
    //
    pCallbackThread->setShotMode(mShotMode, IState::eState_Recording, msAppMode);
    pCallbackThread = 0;
    //
    mpFlowControl->takePicture();

    if  ( pCaptureCmdQueThread != 0 ) {
        status = pCaptureCmdQueThread->onCapture();
    }
    //
lbExit:
    FUNC_END;
    return  status;

}


/******************************************************************************
*
*
*******************************************************************************/
status_t
CamAdapter::
onHandleVideoSnapshotDone()
{
    FUNC_START;
    if(mpStateManager->isState(IState::eState_VideoSnapshot))
    {
        mpStateManager->transitState(IState::eState_Recording);
    }
    else
    {
        MY_LOGW("Not in IState::eState_VideoSnapshot !!");
    }
    FUNC_END;
    return OK ;
}


/******************************************************************************
*
*
*******************************************************************************/
status_t
CamAdapter::
onHandleCancelVideoSnapshot()
{
    FUNC_START;
    FUNC_END;
    return  OK;
}

/******************************************************************************
*
*
*******************************************************************************/
status_t
CamAdapter::
onHandleCancelNormalCapture()
{
    FUNC_START;
    FUNC_END;
    return  OK;
}
/******************************************************************************
*
*
*******************************************************************************/
status_t
CamAdapter::
onHandleCancelZSLCapture()
{
    FUNC_START;
    FUNC_END;
    return  OK;
}

/******************************************************************************
 *
 ******************************************************************************/
template <typename T>
inline MVOID
updateEntry(
    IMetadata* pMetadata,
    MUINT32 const tag,
    T const& val
)
{
    if( pMetadata == NULL ) {
        //MY_LOGE("pMetadata == NULL");
        return;
    }

    IMetadata::IEntry entry(tag);
    entry.push_back(val, Type2Type<T>());
    pMetadata->update(tag, entry);
}

/******************************************************************************
*
*******************************************************************************/
bool
CamAdapter::
onCaptureThreadLoop()
{
    FUNC_START;
    bool ret = false;
    MUINT32 transform = 0;
    //
    //  [1] transit to "Capture" state.
    camera3_request_template_t templateID = CAMERA3_TEMPLATE_STILL_CAPTURE;
    if(mpStateManager->isState(IState::eState_Idle))
    {
        mpStateManager->transitState(IState::eState_NormalCapture);
        templateID = CAMERA3_TEMPLATE_STILL_CAPTURE;
    }
    else
    if(mpStateManager->isState(IState::eState_Preview))
    {
        mpStateManager->transitState(IState::eState_ZSLCapture);
        templateID = CAMERA3_TEMPLATE_STILL_CAPTURE;
    }
    else
    if(mpStateManager->isState(IState::eState_Recording))
    {
        mpStateManager->transitState(IState::eState_VideoSnapshot);
        templateID = CAMERA3_TEMPLATE_VIDEO_SNAPSHOT;
    }
    //
    if( mpStateManager->isState(IState::eState_VideoSnapshot))
    {
        MY_LOGD("In VSS");
    }
    //else
    {
        //
        //  [2.1] update mpShot instance.
        ret = updateShotInstance();
        sp<IShot> pShot = mpShot;
        //
        //  [2.2] return if no shot instance.
        if  ( ! ret || pShot == 0 )
        {
            #warning "[TODO] perform a dummy compressed-image callback or CAMERA_MSG_ERROR to inform app of end of capture?"
            MY_LOGE("updateShotInstance(%d), pShot.get(%p)", ret, pShot.get());
            goto lbExit;
        }
        else
        {
            //  [3.1] prepare parameters
            sp<IParamsManager> pParamsMgr = getParamsManager();
            //
            int iPictureWidth = 0, iPictureHeight = 0;
            if(mpStateManager->isState(IState::eState_VideoSnapshot))
            {
                pParamsMgr->getVideoSize(&iPictureWidth, &iPictureHeight);
            }
            else
            {
                pParamsMgr->getPictureSize(&iPictureWidth, &iPictureHeight);
            }
            //
            int iPreviewWidth = 0, iPreviewHeight = 0;
            pParamsMgr->getPreviewSize(&iPreviewWidth, &iPreviewHeight);
            String8 s8DisplayFormat = mpImgBufProvidersMgr->queryFormat(IImgBufProvider::eID_DISPLAY);
            if  ( String8::empty() == s8DisplayFormat ) {
                MY_LOGW("Display Format is empty");
            }
            //
            if(mName == MtkCameraParameters::APP_MODE_NAME_MTK_ZSD ||       //ZSD preview
                    mpStateManager->isState(IState::eState_VideoSnapshot) ) //VSS shot
            {
            }
            // convert rotation to transform
            MINT32 rotation = pParamsMgr->getInt(CameraParameters::KEY_ROTATION);
            //
            switch(rotation)
            {
                case 0:
                    transform = 0;
                    break;
                case 90:
                    transform = eTransform_ROT_90;
                    break;
                case 180:
                    transform = eTransform_ROT_180;
                    break;
                case 270:
                    transform = eTransform_ROT_270;
                    break;
                default:
                    break;
            }

            //  [3.2] prepare parameters: ShotParam
            ShotParam shotParam;
            shotParam.miPictureFormat           = NSCam::Utils::Format::queryImageFormat(pParamsMgr->getStr(CameraParameters::KEY_PICTURE_FORMAT));
            if  ( 0 != iPictureWidth && 0 != iPictureHeight )
            {
                shotParam.mi4PictureWidth       = iPictureWidth;
                shotParam.mi4PictureHeight      = iPictureHeight;
            }
            else
            {   //  When 3rd-party apk sets "picture-size=0x0", replace it with "preview-size".
                shotParam.mi4PictureWidth       = iPreviewWidth;
                shotParam.mi4PictureHeight      = iPreviewHeight;
            }
            MY_LOGD("shotPicWxH(%dx%d)", shotParam.mi4PictureWidth, shotParam.mi4PictureHeight);
            shotParam.miPostviewDisplayFormat   = NSCam::Utils::Format::queryImageFormat(s8DisplayFormat.string());
            shotParam.miPostviewClientFormat    = NSCam::Utils::Format::queryImageFormat(pParamsMgr->getStr(CameraParameters::KEY_PREVIEW_FORMAT));
            shotParam.mi4PostviewWidth          = iPreviewWidth;
            shotParam.mi4PostviewHeight         = iPreviewHeight;
            shotParam.ms8ShotFileName           = pParamsMgr->getStr(MtkCameraParameters::KEY_CAPTURE_PATH);
            shotParam.mu4ZoomRatio              = pParamsMgr->getZoomRatio();
            shotParam.mu4ShotCount              = pParamsMgr->getInt(MtkCameraParameters::KEY_BURST_SHOT_NUM);
            shotParam.mu4Transform              = transform;
            shotParam.mu4MultiFrameBlending     = pParamsMgr->getMultFrameBlending();
            //
            {
                sp<IParamsManagerV3> pParamsMgrV3 =
                    IParamsManagerV3::createInstance(String8::format(getName()), getOpenId(), pParamsMgr);
                if( pParamsMgrV3 != 0 ) {
                    ITemplateRequest* obj = NSTemplateRequestManager::valueFor(getOpenId());
                    if(obj == NULL) {
                        MY_LOGE("cannot get template request!");
                    }
                    else {
                        //template id is defined in camera3.h
                        shotParam.mAppSetting = obj->getMtkData(templateID);
                        if( mShotMode == eShotMode_VideoSnapShot )
                        {
                            pParamsMgrV3->updateRequest(&shotParam.mAppSetting, SENSOR_SCENARIO_ID_NORMAL_VIDEO);
                        }
                        else
                        {
                            pParamsMgrV3->updateRequest(&shotParam.mAppSetting, SENSOR_SCENARIO_ID_NORMAL_CAPTURE);
                        }
                        MRect reqCropRegion, reqSensorCropRegion;
                        pParamsMgrV3->getCropRegion(reqCropRegion, reqSensorCropRegion);
                        //
                        IMetadata::IEntry entry1(MTK_P1NODE_SENSOR_CROP_REGION);
                        entry1.push_back(reqSensorCropRegion, Type2Type<MRect>());
                        shotParam.mHalSetting.update(MTK_P1NODE_SENSOR_CROP_REGION, entry1);

                        // update default HAL settings
                        pParamsMgrV3->updateRequestHal(&shotParam.mHalSetting);

                        {
                            IMetadata::IEntry entry(MTK_HAL_REQUEST_REQUIRE_EXIF);
                            entry.push_back(true, Type2Type< MUINT8 >());
                            shotParam.mHalSetting.update(entry.tag(), entry);
                            MY_LOGD("set HalSetting REQUIRE_EXIF (true)");
                        }
                        if( mEnRaw16CB == 1 )
                        {
                            IMetadata::IEntry entry(MTK_STATISTICS_LENS_SHADING_MAP_MODE);
                            entry.push_back(MTK_STATISTICS_LENS_SHADING_MAP_MODE_ON, Type2Type< MUINT8 >());
                            shotParam.mAppSetting.update(entry.tag(), entry);
                            MY_LOGD("DNG set MTK_STATISTICS_LENS_SHADING_MAP_MODE (ON)");
                        }
                    }
                }
                else {
                    MY_LOGW("cannot create paramsmgr v3");
                }
                // T.B.D.
                // shotParam.mHalSetting
            }

            //for continuous shot
            PreviewMetadata previewMetadata;
            sp<IResourceContainer> spContainer = NULL;
            MINT32 oldFlashSetting = 0;
            {
                if( (mShotMode == eShotMode_ContinuousShot) && mpStateManager->isState(IState::eState_NormalCapture) )
                {
                    MY_LOGD("normal CShot, get metadata and streambufferprovider");
                    //get preview metadata
                    sp<IParamsManagerV3> pParamsMgrV3 =
                        IParamsManagerV3::createInstance(String8::format(getName()), getOpenId(), pParamsMgr);
                    if( pParamsMgrV3 != 0 )
                    {
                        ITemplateRequest* obj = NSTemplateRequestManager::valueFor(getOpenId());
                        if(obj == NULL)
                        {
                            MY_LOGE("cannot get template request!");
                        }
                        else
                        {
                            //template id is defined in camera3.h
                            previewMetadata.mPreviewAppSetting = obj->getMtkData(CAMERA3_TEMPLATE_ZERO_SHUTTER_LAG);
                            pParamsMgrV3->updateRequest(&previewMetadata.mPreviewAppSetting, SENSOR_SCENARIO_ID_NORMAL_CAPTURE);
                            // update default HAL settings
                            pParamsMgrV3->updateRequestHal(&previewMetadata.mPreviewHalSetting);
                            //
                            IMetadata::IEntry entry(MTK_HAL_REQUEST_REQUIRE_EXIF);
                            entry.push_back(true, Type2Type< MUINT8 >());
                            previewMetadata.mPreviewHalSetting.update(entry.tag(), entry);
                        }
                    }
                    else
                    {
                        MY_LOGE("cannot get pParamsMgrV3");
                    }
                    //
                    if( mbFlashOn )
                    {
                        updateEntry<MUINT8>(&previewMetadata.mPreviewAppSetting , MTK_FLASH_MODE , MTK_FLASH_MODE_TORCH);
                        updateEntry<MUINT8>(&previewMetadata.mPreviewAppSetting , MTK_CONTROL_AE_MODE , MTK_CONTROL_AE_MODE_ON);
                    }

                    //create StreamBufferProvider and store in consumer container
                    sp<ClientBufferPool> pClient = new ClientBufferPool(getOpenId(), MTRUE);
                    if( pClient.get() == NULL )
                    {
                        MY_LOGE("pClient.get() == NULL");
                        goto lbExit;
                    }
                    pClient->setCamClient(
                                    "MShot:Preview:Image:yuvDisp",
                                    mpImgBufProvidersMgr,
                                    IImgBufProvider::eID_DISPLAY
                                    );
                    //
                    sp<StreamBufferProviderFactory> pFactory = StreamBufferProviderFactory::createInstance();
                    if( pFactory.get() == NULL )
                    {
                        MY_LOGE("pFactory.get() == NULL");
                        goto lbExit;
                    }
                    pFactory->setUsersPool(pClient);
                    //
                    sp<StreamBufferProvider> spProvider = pFactory->create();
                    if( spProvider.get() == NULL )
                    {
                        MY_LOGE("spProvider.get() == NULL");
                        goto lbExit;
                    }
                    //
                    spContainer = IResourceContainer::getInstance(getOpenId());
                    if( spContainer.get() == NULL )
                    {
                        MY_LOGE("spContainer.get() == NULL");
                        goto lbExit;
                    }
                    spContainer->setConsumer( eSTREAMID_IMAGE_PIPE_YUV_00, spProvider);
                }
                if( (mShotMode == eShotMode_ContinuousShot) && mpStateManager->isState(IState::eState_ZSLCapture) && mbFlashOn )
                {
                    MY_LOGD("ZSD CShot with flash, set parameter to update flash torch mode");
                    oldFlashSetting = mspParamsMgr->getInt(CameraParameters::KEY_FLASH_MODE);
                    mspParamsMgr->set(CameraParameters::KEY_FLASH_MODE, CameraParameters::FLASH_MODE_TORCH);
                    if( mpFlowControl != NULL)
                    {
                        mpFlowControl->setParameters();
                    }
                    else
                    {
                        MY_LOGW("mpFlowControl == NULL, torch mode not set");
                    }
                }
            }
            //
            //  [3.3] prepare parameters: JpegParam
            JpegParam jpegParam;
            jpegParam.mu4JpegQuality            = pParamsMgr->getInt(CameraParameters::KEY_JPEG_QUALITY);
            jpegParam.mu4JpegThumbQuality       = pParamsMgr->getInt(CameraParameters::KEY_JPEG_THUMBNAIL_QUALITY);
            jpegParam.mi4JpegThumbWidth         = pParamsMgr->getInt(CameraParameters::KEY_JPEG_THUMBNAIL_WIDTH);
            jpegParam.mi4JpegThumbHeight        = pParamsMgr->getInt(CameraParameters::KEY_JPEG_THUMBNAIL_HEIGHT);
            jpegParam.ms8GpsLatitude            = pParamsMgr->getStr(CameraParameters::KEY_GPS_LATITUDE);
            jpegParam.ms8GpsLongitude           = pParamsMgr->getStr(CameraParameters::KEY_GPS_LONGITUDE);
            jpegParam.ms8GpsAltitude            = pParamsMgr->getStr(CameraParameters::KEY_GPS_ALTITUDE);
            jpegParam.ms8GpsTimestamp           = pParamsMgr->getStr(CameraParameters::KEY_GPS_TIMESTAMP);
            jpegParam.ms8GpsMethod              = pParamsMgr->getStr(CameraParameters::KEY_GPS_PROCESSING_METHOD);
            //
            //check if raw16 CB is enable
            if( mEnRaw16CB == 1 )//CAMERA_CMD_ENABLE_RAW16_CALLBACK
            {
                MY_LOGD("adapter.capture: mEnRaw16CB == 1 to enableDataMsg(NSShot::EIShot_DATA_MSG_RAW)");
                pShot->enableDataMsg(NSShot::EIShot_DATA_MSG_RAW);
            }
            //
            // Default ZSD behavior for 3rd-party APP
            if(isThirdPartyZSD())
            {
                mpFlowControl->suspendPreview();
                mbNeedResumPreview = MTRUE;
                MY_LOGD("mpFlowControl->suspendPreview() success");
            }
            //
            //  [4.1] perform Shot operations.
            ret =
                    pShot->sendCommand(eCmd_reset)
                &&  pShot->setCallback(this)
                &&  pShot->sendCommand(eCmd_setShotParam, (MUINTPTR)&shotParam, sizeof(ShotParam))
                &&  pShot->sendCommand(eCmd_setJpegParam, (MUINTPTR)&jpegParam, sizeof(JpegParam))
                &&  ( ( (mShotMode == eShotMode_ContinuousShot) && mpStateManager->isState(IState::eState_NormalCapture))  ?
                    pShot->sendCommand(eCmd_setPreviewMetadata, (MUINTPTR)&previewMetadata, sizeof(PreviewMetadata)) : MTRUE )
                &&  pShot->sendCommand(eCmd_capture)
                    ;
            if  ( ! ret )
            {
                MY_LOGE("fail to perform shot operations");
            }
            //
            {
                spContainer = NULL;
                if( (mShotMode == eShotMode_ContinuousShot) && mpStateManager->isState(IState::eState_ZSLCapture) && mbFlashOn )
                {
                    MY_LOGD("ZSD CShot with flash, set parameter to cancel flash torch mode");
                    mspParamsMgr->set(CameraParameters::KEY_FLASH_MODE, oldFlashSetting);
                    if( mpFlowControl != NULL)
                    {
                        mpFlowControl->setParameters();
                    }
                    else
                    {
                        MY_LOGW("mpFlowControl == NULL, torch mode not set");
                    }
                }
            }
        }
        //
        //
        lbExit:
        //
        //  [5.1] uninit shot instance.
        MY_LOGD("free shot instance: (mpShot/pShot)=(%p/%p)", mpShot.get(), pShot.get());
        mpShot = NULL;
        pShot  = NULL;
        //
        //  [5.11] update focus steps.
        //
        {
            sp<IParamsManager> pParamsMgr = getParamsManager();
            pParamsMgr->updateBestFocusStep();
        }
        //
        //  [5.2] notify capture done.
    }
    //
    mpStateManager->getCurrentState()->onCaptureDone(this);
    //
    //
    FUNC_END;
    return  true;
}

