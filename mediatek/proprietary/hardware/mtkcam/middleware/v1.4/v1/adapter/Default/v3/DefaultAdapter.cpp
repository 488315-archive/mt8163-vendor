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
//
#include <inc/CamUtils.h>
#include <cutils/properties.h>  // For property_get().

using namespace android;
using namespace MtkCamUtils;
//
#include <camera/MtkCamera.h>
//
#include <inc/ImgBufProvidersManager.h>
//
#include <v1/IParamsManager.h>
#include <v1/ICamAdapter.h>
#include <inc/BaseCamAdapter.h>
//
#include "inc/v3/DefaultAdapter.h"
using namespace NSDefaultAdapter;
//
#include <IHalSensor.h>
using namespace NSCam;
//

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

#define MY_LOGD1(...)               MY_LOGD_IF((mLogLevel>=1),__VA_ARGS__)
#define MY_LOGD2(...)               MY_LOGD_IF((mLogLevel>=2),__VA_ARGS__)
#define MY_LOGD3(...)               MY_LOGD_IF((mLogLevel>=3),__VA_ARGS__)

//
#define FUNC_START                  MY_LOGD2("+")
#define FUNC_END                    MY_LOGD2("-")


/******************************************************************************
*
*******************************************************************************/
sp<ICamAdapter>
createDefaultAdapter(
    String8 const&      rAppMode,
    int32_t const       i4OpenId,
    sp<IParamsManager>  pParamsMgr
)
{
    CAM_LOGD("createDefaultAdapter");
    return new CamAdapter(
        rAppMode,
        i4OpenId,
        pParamsMgr
    );
}


/******************************************************************************
*
*******************************************************************************/
CamAdapter::
CamAdapter(
    String8 const&      rAppMode,
    int32_t const       i4OpenId,
    sp<IParamsManager>  pParamsMgr
)
    : BaseCamAdapter(rAppMode, i4OpenId, pParamsMgr)
    //
    , mpStateManager(NULL)
    //
    , mpCaptureCmdQueThread(0)
    , mpZipCallbackThread(0)
    //
    , mpShot(0)
    //
    , msAppMode(rAppMode)
    //
    , mbTakePicPrvNotStop(false)
    , mbFixFps(false)
    , mPreviewMaxFps(0)
    , mShotMode(0)
    //
    , mLastVdoWidth(0)
    , mLastVdoHeight(0)
    //
    , mEngParam()
    , mLogLevel(0)
    , mpFlowControl(NULL)
    , mbFlashOn(MFALSE)
    //
    , mEnRaw16CB(0)
    //
    , mbNeedResumPreview(MFALSE)
{
    mLogLevel = ::property_get_int32("debug.camera.log", 0);
    if ( mLogLevel == 0 )
    {
        mLogLevel = ::property_get_int32("debug.camera.adapter.loglevel", 2);
    }

    MY_LOGD1(
        "AppMode:%s, sizeof=%d, this=%p, mpStateManager=%p",
        msAppMode.string(),
        sizeof(CamAdapter),
        this,&mpStateManager
    );


    mpParamsManagerV3 = IParamsManagerV3::createInstance(rAppMode, i4OpenId, pParamsMgr);

}


/******************************************************************************
*
*******************************************************************************/
CamAdapter::
~CamAdapter()
{
    MY_LOGD1("tid(%d), OpenId(%d)", ::gettid(), getOpenId());
}


/******************************************************************************
*
*******************************************************************************/
#pragma push_macro("match")
#undef match
#define match(mgr, key, value) \
  (!strcmp(mgr->getStr(MtkCameraParameters::key), MtkCameraParameters::value))

bool useFeatureFlowControl(const String8 &appMode, const sp<IParamsManager> &mgr)
{
    bool result = false;
    int debugVal = ::property_get_int32("debug.force.p2feature", 0);
    if( debugVal == 1 || debugVal == 2 )
    {
        ::property_set("debug.force.p2feature", "0");
    }
    int camMode = mgr->getInt(MtkCameraParameters::KEY_CAMERA_MODE);

    result = (camMode == MtkCameraParameters::CAMERA_MODE_MTK_PRV) ||
             (camMode == MtkCameraParameters::CAMERA_MODE_MTK_VDO) ||
             (appMode == MtkCameraParameters::APP_MODE_NAME_MTK_PHOTO) ||
             (debugVal == 1) || (debugVal == 9) ||
             (0 && match(mgr, KEY_3DNR_MODE, TRUE)) ||
             (0 && match(mgr, KEY_FACE_BEAUTY, TRUE)) ||
             (0 && (mgr->getVHdr() != SENSOR_VHDR_MODE_NONE)) ||
             0;

    if( (debugVal == 2) || (debugVal == 8) )
    {
        result = false;
    }

    CAM_LOGD("FlowControl Selection: appMode = %s, camMode=%d, prop = %d, result = %d",
            appMode.string(), camMode, debugVal, result);
    return result;
}
#pragma pop_macro("match")
/******************************************************************************
*
*******************************************************************************/
bool
CamAdapter::
init()
{
    FUNC_START;
    bool ret = false;
    status_t status = OK;
    //
    mpStateManager = IStateManager::createInstance();
    if ( mpStateManager != NULL )
    {
        if(!(mpStateManager->init()))
        {
            MY_LOGE("mpStateManager->init fail");
            goto lbExit;
        }
    }
    //
#warning "FIXME"
    if(msAppMode == MtkCameraParameters::APP_MODE_NAME_MTK_ENG)
    {
        mpFlowControl = IFlowControl::createInstance(
                        "EngCamAdapter",
                        getOpenId(),
                        IFlowControl::ControlType_T::CONTROL_ENG,
                        mspParamsMgr,
                        mpImgBufProvidersMgr,
                        this
                    );
    }
#if (MTKCAM_BASIC_PACKAGE != 1)
    else if( useFeatureFlowControl(msAppMode, mspParamsMgr) )
    {
        mpFlowControl = IFlowControl::createInstance(
                            "FeatureCamAdapter",
                            getOpenId(),
                            IFlowControl::ControlType_T::CONTROL_FEATURE,
                            mspParamsMgr,
                            mpImgBufProvidersMgr,
                            this
                        );
    }
#endif // (MTKCAM_BASIC_PACKAGE != 1)
    else
    {
        mpFlowControl = IFlowControl::createInstance(
                            "DefaultCamAdapter",
                            getOpenId(),
                            IFlowControl::ControlType_T::CONTROL_DEFAULT,
                            mspParamsMgr,
                            mpImgBufProvidersMgr,
                            this
                        );
    }
    if ( mpFlowControl == 0 ) goto lbExit;

    mpCaptureCmdQueThread = ICaptureCmdQueThread::createInstance(this);
    if  ( mpCaptureCmdQueThread == 0 || OK != (status = mpCaptureCmdQueThread->run() ) )
    {
        MY_LOGE(
            "Fail to run CaptureCmdQueThread - mpCaptureCmdQueThread.get(%p), status[%s(%d)]",
            mpCaptureCmdQueThread.get(), ::strerror(-status), -status
        );
        goto lbExit;
    }
    //
    {
        MY_LOGD("construct ITemplateRequest of id %d", getOpenId());
        ITemplateRequest* obj = NSTemplateRequestManager::valueFor(getOpenId());
        if(obj == NULL) {
            obj = ITemplateRequest::getInstance(getOpenId());
            NSTemplateRequestManager::add(getOpenId(), obj);
        }
    }
    //
    ret = true;
lbExit:
    if(!ret)
    {
        MY_LOGE("init() fail; now call uninit()");
        uninit();
    }
    FUNC_END;
    return  ret;
}


/******************************************************************************
*
*******************************************************************************/
bool
CamAdapter::
uninit()
{
    FUNC_START;
    bool ret = true;
    //  Close Command Queue Thread of Capture.
    sp<ICaptureCmdQueThread> pCaptureCmdQueThread = mpCaptureCmdQueThread;
    mpCaptureCmdQueThread = 0;
    if  ( pCaptureCmdQueThread != 0 ) {
        pCaptureCmdQueThread->requestExit();
        pCaptureCmdQueThread->join();
        pCaptureCmdQueThread = 0;
    }
    //
    if( !waitForShotDone() )
    {
        MY_LOGE("wait for shot done failed");
    }
    //
    if( previewEnabled())
    {
        MY_LOGD("Force to stop preview start (%d)",mbTakePicPrvNotStop);
        stopPreview();
        MY_LOGD1("Force to stop preview done");
    }
    //
    if(mpStateManager != NULL)
    {
        mpStateManager->uninit();
        mpStateManager->destroyInstance();
        mpStateManager = NULL;
    }
    //
    mpFlowControl = NULL;
    //
    BaseCamAdapter::uninit();
    //
    FUNC_END;
    return  ret;
}


/******************************************************************************
*
*******************************************************************************/
status_t
CamAdapter::
sendCommand(int32_t cmd, int32_t arg1, int32_t arg2)
{
    status_t ret = OK;
    CAM_TRACE_NAME("Adapter:sendCommand");
    //
    MY_LOGD("cmd(0x%08X),arg1(0x%08X),arg2(0x%08X)", cmd, arg1, arg2);
    //
    switch( cmd )
    {
        case CAMERA_CMD_CANCEL_CSHOT:
        {
            return cancelPicture();
        }
        case CAMERA_CMD_SET_CSHOT_SPEED:
        {
            return setCShotSpeed(arg1);
        }
        case CAMERA_CMD_ENABLE_RAW16_CALLBACK:
        {
            CAM_LOGD("[sendCommand] CAMERA_CMD_ENABLE_RAW16_CALLBACK (%d)\n", arg1);
            mEnRaw16CB = arg1;
            return OK;
        }
        default:
        {
            break;
        }
    }

    return mpFlowControl->sendCommand(cmd, arg1, arg2);

}

/******************************************************************************
*
*******************************************************************************/
void
CamAdapter::
doNotifyCallback(
    int32_t _msgType,
    int32_t _ext1,
    int32_t _ext2
)
{
    mpCamMsgCbInfo->mNotifyCb(
        _msgType,
        _ext1,
        _ext2,
        mpCamMsgCbInfo->mCbCookie
    );
}
/******************************************************************************
*
*******************************************************************************/

void
CamAdapter::
doDataCallback(
    int32_t  _msgType,
    void*    _data,
    uint32_t  _size
)
{
    switch(_msgType)
    {
        case MTK_CAMERA_MSG_EXT_DATA_AF:
        {
            int32_t* pSrc = reinterpret_cast<int32_t*>(_data);
            for(int i=0;i<_size;i++)
            {
                MY_LOGD2("data(%d)=%d",i,pSrc[i]);
            }
            camera_memory* mem = mpCamMsgCbInfo->mRequestMemory(-1, sizeof(int32_t)*(_size+1), 1, NULL);
            int32_t* pDst = reinterpret_cast<int32_t*>(mem->data);
            pDst[0] = MTK_CAMERA_MSG_EXT_DATA_AF;
            for (uint32_t i = 0; i < _size; i++)
            {
                pDst[i+1] = pSrc[i];
            }
            for(int i=0;i<_size+1;i++)
            {
                MY_LOGD2("pDst(%d)=%d",i,pDst[i]);
            }
            mpCamMsgCbInfo->mDataCb(MTK_CAMERA_MSG_EXT_DATA, mem, 0, NULL, mpCamMsgCbInfo->mCbCookie);
            mem->release(mem);
            break;
        }
        default:
        {
            MY_LOGW("Not support this data callback");
            break;
        }
    }
}

/******************************************************************************
*
*******************************************************************************/
bool
CamAdapter::
msgTypeEnabled( int32_t msgType )
{
    return msgType == (msgType & ::android_atomic_release_load(&mpCamMsgCbInfo->mMsgEnabled));
}

/******************************************************************************
*
*******************************************************************************/
bool
CamAdapter::
isThirdPartyZSD() const
{
    if(msAppMode == MtkCameraParameters::APP_MODE_NAME_MTK_ZSD &&   //ZSD
       mspParamsMgr->getInt(MtkCameraParameters::KEY_CAMERA_MODE) == MtkCameraParameters::CAMERA_MODE_NORMAL    //3rd-party
       )
    {
        return true;
    }
    else
    {
        return false;
    }
}
