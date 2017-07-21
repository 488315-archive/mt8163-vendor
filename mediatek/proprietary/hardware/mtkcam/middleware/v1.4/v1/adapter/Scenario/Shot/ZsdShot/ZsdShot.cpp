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

#define LOG_TAG "MtkCam/Shot"
//
#include <Log.h>
#include <common.h>
//
#include <hwutils/CameraProfile.h>
using namespace CPTool;
//
#include <IHalSensor.h>
//
#include <camshot/ICamShot.h>
#include <camshot/ISingleShot.h>
#include <camshot/ISmartShot.h>
//
#include <Shot/IShot.h>
//
#include "ImpShot.h"
#include "ZsdShot.h"
//
#include <hwutils/CamManager.h>
using namespace NSCam::Utils;
//
#include <LegacyPipeline/StreamId.h>
#include <camshot/_params.h>
//
#include <metadata/client/mtk_metadata_tag.h>
//
#include <LegacyPipeline/ILegacyPipeline.h>
#include <hwutils/HwInfoHelper.h>
#include <metadata/mtk_platform_metadata_tag.h>
#include <LegacyPipeline/buffer/Selector.h>
#include <utils/Vector.h>


using namespace android;
using namespace NSShot;
using namespace NSCam::v1;
using namespace NSCam::v1::NSLegacyPipeline;
using namespace NSCamHW;

#define DNG_META_CB 1


/******************************************************************************
 *
 ******************************************************************************/
#define MY_LOGV(fmt, arg...)        CAM_LOGV("(%d)(%s)[%s] " fmt, ::gettid(), getShotName(), __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_LOGD("(%d)(%s)[%s] " fmt, ::gettid(), getShotName(), __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_LOGI("(%d)(%s)[%s] " fmt, ::gettid(), getShotName(), __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_LOGW("(%d)(%s)[%s] " fmt, ::gettid(), getShotName(), __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_LOGE("(%d)(%s)[%s] " fmt, ::gettid(), getShotName(), __FUNCTION__, ##arg)
#define MY_LOGA(fmt, arg...)        CAM_LOGA("(%d)(%s)[%s] " fmt, ::gettid(), getShotName(), __FUNCTION__, ##arg)
#define MY_LOGF(fmt, arg...)        CAM_LOGF("(%d)(%s)[%s] " fmt, ::gettid(), getShotName(), __FUNCTION__, ##arg)
//
#define MY_LOGV_IF(cond, ...)       do { if ( (cond) ) { MY_LOGV(__VA_ARGS__); } }while(0)
#define MY_LOGD_IF(cond, ...)       do { if ( (cond) ) { MY_LOGD(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) ) { MY_LOGI(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) ) { MY_LOGW(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) ) { MY_LOGE(__VA_ARGS__); } }while(0)
#define MY_LOGA_IF(cond, ...)       do { if ( (cond) ) { MY_LOGA(__VA_ARGS__); } }while(0)
#define MY_LOGF_IF(cond, ...)       do { if ( (cond) ) { MY_LOGF(__VA_ARGS__); } }while(0)


/******************************************************************************
 *
 ******************************************************************************/
extern "C"
sp<IShot>
createInstance_ZsdShot(
    char const*const    pszShotName,
    uint32_t const      u4ShotMode,
    int32_t const       i4OpenId
)
{
    sp<IShot>       pShot = NULL;
    sp<ZsdShot>  pImpShot = NULL;
    //
    //  (1.1) new Implementator.
    pImpShot = new ZsdShot(pszShotName, u4ShotMode, i4OpenId);
    if  ( pImpShot == 0 ) {
        CAM_LOGE("[%s] new ZsdShot", __FUNCTION__);
        goto lbExit;
    }
    //
    //  (1.2) initialize Implementator if needed.
    if  ( ! pImpShot->onCreate() ) {
        CAM_LOGE("[%s] onCreate()", __FUNCTION__);
        goto lbExit;
    }
    //
    //  (2)   new Interface.
    pShot = new IShot(pImpShot);
    if  ( pShot == 0 ) {
        CAM_LOGE("[%s] new IShot", __FUNCTION__);
        goto lbExit;
    }
    //
lbExit:
    //
    //  Free all resources if this function fails.
    if  ( pShot == 0 && pImpShot != 0 ) {
        pImpShot->onDestroy();
        pImpShot = NULL;
    }
    //
    return  pShot;
}

/******************************************************************************
 *
 ******************************************************************************/
template <typename T>
inline MBOOL
tryGetMetadata(
    IMetadata const* const pMetadata,
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
 *  This function is invoked when this object is firstly created.
 *  All resources can be allocated here.
 ******************************************************************************/
bool
ZsdShot::
onCreate()
{
    bool ret = true;
    return ret;
}


/******************************************************************************
 *  This function is invoked when this object is ready to destryoed in the
 *  destructor. All resources must be released before this returns.
 ******************************************************************************/
void
ZsdShot::
onDestroy()
{

}


/******************************************************************************
 *
 ******************************************************************************/
ZsdShot::
ZsdShot(
    char const*const pszShotName,
    uint32_t const u4ShotMode,
    int32_t const i4OpenId
)
    : ImpShot(pszShotName, u4ShotMode, i4OpenId)
{
}


/******************************************************************************
 *
 ******************************************************************************/
ZsdShot::
~ZsdShot()
{
}


/******************************************************************************
 *
 ******************************************************************************/
bool
ZsdShot::
sendCommand(
    uint32_t const  cmd,
    MUINTPTR const  arg1,
    uint32_t const  arg2,
    uint32_t const  arg3
)
{
    AutoCPTLog cptlog(Event_Shot_sendCmd, cmd, arg1);
    bool ret = true;
    //
    switch  (cmd)
    {
    //  This command is to reset this class. After captures and then reset,
    //  performing a new capture should work well, no matter whether previous
    //  captures failed or not.
    //
    //  Arguments:
    //          N/A
    case eCmd_reset:
        ret = onCmd_reset();
        break;

    //  This command is to perform capture.
    //
    //  Arguments:
    //          N/A
    case eCmd_capture:
        ret = onCmd_capture();
        break;

    //  This command is to perform cancel capture.
    //
    //  Arguments:
    //          N/A
    case eCmd_cancel:
        onCmd_cancel();
        break;
    //
    default:
        ret = ImpShot::sendCommand(cmd, arg1, arg2, arg3);
    }
    //
    return ret;
}


/******************************************************************************
 *
 ******************************************************************************/
bool
ZsdShot::
onCmd_reset()
{
    bool ret = true;
    return ret;
}


/******************************************************************************
 *
 ******************************************************************************/
bool
ZsdShot::
onCmd_capture()
{
    MY_LOGD("ZSD Shot capture");

    AutoCPTLog cptlog(Event_Shot_capture);
    MBOOL ret = MTRUE;
    MBOOL isMfbShot = MFALSE;
    NSCamShot::ICamShot *pSingleShot = NSCamShot::ISmartShot::createInstance(static_cast<EShotMode>(mu4ShotMode)
                                                                            , "ZsdShot"
                                                                            , getOpenId()
                                                                            , mShotParam.mu4MultiFrameBlending
                                                                            , &isMfbShot
                                                                            , getPipelineMode()
                                                                            );
    //
    pSingleShot->init();
    //
    pSingleShot->enableNotifyMsg( NSCamShot::ECamShot_NOTIFY_MSG_EOF );
    //
    EImageFormat ePostViewFmt =
        static_cast<EImageFormat>(mShotParam.miPostviewDisplayFormat);

    pSingleShot->enableDataMsg(
            NSCamShot::ECamShot_DATA_MSG_JPEG
            |((isDataMsgEnabled(NSShot::EIShot_DATA_MSG_RAW))?
            NSCamShot::ECamShot_DATA_MSG_DNG : NSCamShot::ECamShot_DATA_MSG_NONE)
#if DNG_META_CB
            |((isDataMsgEnabled(NSShot::EIShot_DATA_MSG_RAW))?
            NSCamShot::ECamShot_DATA_MSG_APPMETA : NSCamShot::ECamShot_DATA_MSG_NONE)
#endif
            );

    // update post nr settings
    updatePostNRSetting(MTK_NR_MODE_AUTO, isMfbShot, &mShotParam.mHalSetting);

    // shot param
    NSCamShot::ShotParam rShotParam(
            eImgFmt_YUY2,                    //yuv format
            mShotParam.mi4PictureWidth,      //picutre width
            mShotParam.mi4PictureHeight,     //picture height
            mShotParam.mu4Transform,         //picture transform
            ePostViewFmt,                    //postview format
            mShotParam.mi4PostviewWidth,     //postview width
            mShotParam.mi4PostviewHeight,    //postview height
            0,                               //postview transform
            mShotParam.mu4ZoomRatio,         //zoom
            mShotParam.mAppSetting,
            mShotParam.mHalSetting
            );

    // jpeg param
    NSCamShot::JpegParam rJpegParam(
            NSCamShot::ThumbnailParam(
                mJpegParam.mi4JpegThumbWidth,
                mJpegParam.mi4JpegThumbHeight,
                mJpegParam.mu4JpegThumbQuality,
                MTRUE),
            mJpegParam.mu4JpegQuality,         //Quality
            MFALSE                             //isSOI
            );

    // sensor param
    NSCamShot::SensorParam rSensorParam(
            getOpenId(),                            //sensor idx
            SENSOR_SCENARIO_ID_NORMAL_CAPTURE,      //Scenaio
            10,                                     //bit depth
            MFALSE,                                 //bypass delay
            MFALSE                                  //bypass scenario
            );
    //
    pSingleShot->setCallbacks(fgCamShotNotifyCb, fgCamShotDataCb, this);
    //
    sp<StreamBufferProvider> pTempConsumer = IResourceContainer::getInstance(getOpenId())->queryConsumer( eSTREAMID_IMAGE_PIPE_RAW_OPAQUE );
    MY_LOGD("Query Consumer OpenID(%d) StreamID(%d)", getOpenId(), eSTREAMID_IMAGE_PIPE_RAW_OPAQUE);
    if(pTempConsumer == NULL)
    {
        MY_LOGE("can't find StreamBufferProvider in ConsumerContainer");
        return MFALSE;
    }
    //
    ret = pSingleShot->setShotParam(rShotParam);
    //
    ret = pSingleShot->setJpegParam(rJpegParam);
    //
    bool bFlashRequired = CheckIsFlashRequired();
    //
    if(bFlashRequired)
    {
        MBOOL res = InitFlashFlow(rSensorParam, rShotParam);
        if(res == MFALSE)
        {
            MY_LOGE("InitFlashFlow Fail!");
            return MFALSE;
        }
    }
    //
    ret = pSingleShot->startOne(rSensorParam, pTempConsumer.get());
    //
    ret = pSingleShot->uninit();
    //
    pSingleShot->destroyInstance();
    //
    if(bFlashRequired)
    {
        MBOOL res = UninitFlashFlow();
        if(res == MFALSE)
        {
            MY_LOGE("UninitFlashFlow Fail!");
            return MFALSE;
        }
    }
    //
    return ret;
}


/******************************************************************************
 *
 ******************************************************************************/
void
ZsdShot::
onCmd_cancel()
{
    AutoCPTLog cptlog(Event_Shot_cancel);
}


/******************************************************************************
 *
 ******************************************************************************/
MBOOL
ZsdShot::
fgCamShotNotifyCb(MVOID* user, NSCamShot::CamShotNotifyInfo const msg)
{
    AutoCPTLog cptlog(Event_Shot_handleNotifyCb);
    ZsdShot *pZsdShot = reinterpret_cast <ZsdShot *>(user);
    if (NULL != pZsdShot)
    {
        if ( NSCamShot::ECamShot_NOTIFY_MSG_EOF == msg.msgType)
        {
            pZsdShot->mpShotCallback->onCB_Shutter(true,
                                                      0
                                                     );
        }
    }

    return MTRUE;
}


/******************************************************************************
 *
 ******************************************************************************/
MBOOL
ZsdShot::
fgCamShotDataCb(MVOID* user, NSCamShot::CamShotDataInfo const msg)
{
    ZsdShot *pZsdShot = reinterpret_cast<ZsdShot *>(user);
    if (NULL != pZsdShot)
    {
        if (NSCamShot::ECamShot_DATA_MSG_POSTVIEW == msg.msgType)
        {
            //pZsdShot->handlePostViewData( msg.puData, msg.u4Size);
        }
        else if (NSCamShot::ECamShot_DATA_MSG_JPEG == msg.msgType)
        {
            pZsdShot->handleJpegData(msg.pBuffer);
        }
#if DNG_META_CB
        else if (NSCamShot::ECamShot_DATA_MSG_DNG == msg.msgType)//raw data callback to adapter
        {
            msg.pBuffer->lockBuf(LOG_TAG, eBUFFER_USAGE_SW_READ_MASK);
            pZsdShot->handleRaw16CB(msg.pBuffer);
            msg.pBuffer->unlockBuf(LOG_TAG);
        }
        else if (NSCamShot::ECamShot_DATA_MSG_APPMETA == msg.msgType)//raw data callback to adapter
        {
            pZsdShot->handleDNGMetaCB(msg.ext1);//metadata structure
        }
#endif
    }

    return MTRUE;
}


/******************************************************************************
*
*******************************************************************************/
MBOOL
ZsdShot::
handlePostViewData(MUINT8* const puBuf, MUINT32 const u4Size)
{
#if 0
    AutoCPTLog cptlog(Event_Shot_handlePVData);
    MY_LOGD("+ (puBuf, size) = (%p, %d)", puBuf, u4Size);
    mpShotCallback->onCB_PostviewDisplay(0,
                                         u4Size,
                                         reinterpret_cast<uint8_t const*>(puBuf)
                                        );

    MY_LOGD("-");
#endif
    return  MTRUE;
}

/******************************************************************************
*
*******************************************************************************/
MBOOL
ZsdShot::
handleRaw16CB(IImageBuffer const *pImgBuffer)
{
    MY_LOGD("+ (pImgBuffer) = (%p)", pImgBuffer);
    mpShotCallback->onCB_Raw16Image(reinterpret_cast<IImageBuffer const*>(pImgBuffer));
    return  MTRUE;
}


/******************************************************************************
*
*******************************************************************************/
MBOOL
ZsdShot::
handleDNGMetaCB(MUINTPTR const dngMetadata)
{
    mpShotCallback->onCB_DNGMetaData(dngMetadata);
    return  MTRUE;
}

/******************************************************************************
*
*******************************************************************************/
MBOOL
ZsdShot::
handleJpegData(IImageBuffer* pJpeg)
{
    AutoCPTLog cptlog(Event_Shot_handleJpegData);
    //
    class scopedVar
    {
    public:
                    scopedVar(IImageBuffer* pBuf)
                    : mBuffer(pBuf) {
                        if( mBuffer )
                            mBuffer->lockBuf(LOG_TAG, eBUFFER_USAGE_SW_READ_MASK);
                    }
                    ~scopedVar() {
                        if( mBuffer )
                            mBuffer->unlockBuf(LOG_TAG);
                    }
    private:
        IImageBuffer* mBuffer;
    } _local(pJpeg);
    //
    uint8_t const* puJpegBuf = (uint8_t const*)pJpeg->getBufVA(0);
    MUINT32 u4JpegSize = pJpeg->getBitstreamSize();

    MY_LOGD("+ (puJpgBuf, jpgSize) = (%p, %d)",
            puJpegBuf, u4JpegSize);

    // dummy raw callback
    mpShotCallback->onCB_RawImage(0, 0, NULL);

    // Jpeg callback
    CPTLogStr(Event_Shot_handleJpegData, CPTFlagSeparator, "onCB_CompressedImage");
    mpShotCallback->onCB_CompressedImage_packed(0,
                                         u4JpegSize,
                                         puJpegBuf,
                                         0,                       //callback index
                                         true                     //final image
                                         );
    MY_LOGD("-");

    return MTRUE;
}

/******************************************************************************
*
*******************************************************************************/
MBOOL
ZsdShot::
CheckIsFlashRequired()
{
    MBOOL bFlashRequired = false;
    sp<IFrameInfo> pFrameInfo = IResourceContainer::getInstance(getOpenId())->queryLatestFrameInfo();
    if(pFrameInfo == NULL)
    {
        MY_LOGW("Can't query Latest FrameInfo!");
    }
    else
    {
        //The FlashRequired condition must be the same with PreCapture required condition in CamAdapter::takePicture()
        IMetadata meta;
        pFrameInfo->getFrameMetadata(meta);
        MUINT8 AeState = 0;
        tryGetMetadata< MUINT8 >(&meta, MTK_CONTROL_AE_STATE, AeState);
        MUINT8 AeMode = 0;
        tryGetMetadata< MUINT8 >(&meta, MTK_CONTROL_AE_MODE, AeMode);
        if( AeMode == MTK_CONTROL_AE_MODE_ON_ALWAYS_FLASH ||
            (AeMode == MTK_CONTROL_AE_MODE_ON_AUTO_FLASH && AeState == MTK_CONTROL_AE_STATE_FLASH_REQUIRED) )
            bFlashRequired = true;
        else
            bFlashRequired = false;
        MY_LOGD("query AE mode(%d) AE state(%d) bFlashRequired(%d)", AeMode, AeState, bFlashRequired);
    }
    return bFlashRequired;
}

/******************************************************************************
*
*******************************************************************************/
MBOOL
ZsdShot::
InitFlashFlow(NSCamShot::SensorParam const &rSensorParam, NSCamShot::ShotParam const &rShotParam)
{
    MY_LOGD("ZSD in FlashRequired mode");
    //
    sp<IFeatureFlowControl> pFeatureFlowControl = IResourceContainer::getInstance(getOpenId())->queryFeatureFlowControl();
    if(pFeatureFlowControl == NULL)
    {
        MY_LOGE("IFeatureFlowControl is NULL!");
        return MFALSE;
    }
    //
    HwInfoHelper helper(getOpenId());
    MSize sensorSize;
    if( ! helper.updateInfos() ) {
        MY_LOGE("cannot properly update infos!");
        return MFALSE;
    }
    //
    MUINT32 const sensorMode = rSensorParam.u4Scenario;
    if( ! helper.getSensorSize( sensorMode, sensorSize) ) {
        MY_LOGE("cannot get params about sensor!");
        return MFALSE;
    }
    MY_LOGD("sensorMode(%d), sensorSize(%d,%d)", sensorMode, sensorSize.w, sensorSize.h);
    //
    IMetadata appSetting = rShotParam.appSetting;
    IMetadata halSetting = rShotParam.halSetting;
    //
    {
        IMetadata::IEntry entry(MTK_HAL_REQUEST_SENSOR_SIZE);
        entry.push_back(sensorSize, Type2Type< MSize >());
        halSetting.update(entry.tag(), entry);
    }
    //
    Vector< SettingSet > vSettings;
    Vector< StreamId >   vDstStreams;
    Vector< MINT32 >     vRequestNo;

    int iCaptureRequestNo = 0;
    SettingSet setting;
    setting.appSetting = appSetting;
    setting.halSetting = halSetting;
    vSettings.push_back(setting);
    vDstStreams.push_back(eSTREAMID_IMAGE_PIPE_RAW_OPAQUE);

    if( OK != pFeatureFlowControl->submitRequest(
                vSettings,
                vDstStreams,
                vRequestNo
                )
      )
    {
        MY_LOGE("submitRequest failed");
        return MFALSE;
    }
    iCaptureRequestNo = vRequestNo[0];
    MY_LOGD("FeatureFlowControl submitSetting (%d)", iCaptureRequestNo);
    //
    sp<StreamBufferProvider> pTempConsumer = IResourceContainer::getInstance(getOpenId())->queryConsumer( eSTREAMID_IMAGE_PIPE_RAW_OPAQUE );
    if(pTempConsumer == NULL)
    {
        MY_LOGE("Consumer is NULL!");
        return MFALSE;
    }
    sp<ZsdFlashSelector> pSelector = new ZsdFlashSelector();
    pSelector->setWaitRequestNo(iCaptureRequestNo);
    status_t status = pTempConsumer->setSelector(pSelector);
    if(status != OK)
    {
        MY_LOGE("change to ZSD Flash selector Fail!");
        return MFALSE;
    }
    MY_LOGD("change to ZSD Flash selector");
    //
    return MTRUE;
}

/******************************************************************************
*
*******************************************************************************/
MBOOL
ZsdShot::
UninitFlashFlow()
{
    sp<StreamBufferProvider> pTempConsumer = IResourceContainer::getInstance(getOpenId())->queryConsumer( eSTREAMID_IMAGE_PIPE_RAW_OPAQUE );
    if(pTempConsumer == NULL)
    {
        MY_LOGE("Consumer is NULL!");
        return MFALSE;
    }
    sp<ZsdSelector> pSelector = new ZsdSelector();
    pTempConsumer->setSelector(pSelector);
    MY_LOGD("change to ZSD selector");
    return MTRUE;
}
