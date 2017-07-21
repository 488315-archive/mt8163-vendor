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
#include <Hal3/aaa_hal_common.h>
#include <Hal3/IHal3A.h>
using namespace NS3Av3;
//
#include <IHalSensor.h>
//
#include <Shot/IShot.h>
//
#include <camshot/_params.h>
//
#include "inc/ImpShot.h"
//
#include <cutils/properties.h>
//
#include <metadata/mtk_platform_metadata_tag.h>
#include <camera_custom_capture_nr.h>
//
//#include <camera_custom_capture_rwb.h>
//
#include <hwutils/CamManager.h>
using namespace NSCam::Utils;
//
using namespace android;
using namespace NSShot;


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


////////////////////////////////////////////////////////////////////////////////
//  IShot
////////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 *
 ******************************************************************************/
template<typename T>
void
updateSingleEntry(IMetadata* pMeta, MUINT32 const tag, T const val)
{
    IMetadata::IEntry entry(tag);
    entry.push_back(val, Type2Type<T>());
    pMeta->update(tag, entry);
}

/******************************************************************************
 *
 ******************************************************************************/
IShot::
IShot(sp<ImpShot> pImpShot)
    : mpImpShot(pImpShot)
{
}


/******************************************************************************
 *
 ******************************************************************************/
IShot::
~IShot()
{
    MY_LOGD("");
    mpImpShot->onDestroy();
    mpImpShot = NULL;
}


/******************************************************************************
 *
 ******************************************************************************/
char const*
IShot::
getShotName() const
{
    return  mpImpShot->getShotName();
}


/******************************************************************************
 *
 ******************************************************************************/
uint32_t
IShot::
getShotMode() const
{
    return  mpImpShot->getShotMode();
}


/******************************************************************************
 *
 ******************************************************************************/
EPipelineMode
IShot::
getPipelineMode() const
{
    return  mpImpShot->getPipelineMode();
}


/******************************************************************************
 *
 ******************************************************************************/
int32_t
IShot::
getOpenId() const
{
    return  mpImpShot->getOpenId();
}


/******************************************************************************
 *
 ******************************************************************************/
bool
IShot::
setCallback(sp<IShotCallback> pShotCallback)
{
    return  mpImpShot->setCallback(pShotCallback);
}


/******************************************************************************
*
*******************************************************************************/
bool
IShot::
isDataMsgEnabled(MINT32 const i4MsgTypes)
{
    return  mpImpShot->isDataMsgEnabled(i4MsgTypes);
}

/******************************************************************************
*
*******************************************************************************/
void
IShot::
enableDataMsg(MINT32 const i4MsgTypes)
{
    mpImpShot->enableDataMsg(i4MsgTypes);
}

/******************************************************************************
*
*******************************************************************************/
void
IShot::
disableDataMsg(MINT32 const i4MsgTypes)
{
    mpImpShot->disableDataMsg(i4MsgTypes);
}


/******************************************************************************
 *
 ******************************************************************************/
bool
IShot::
sendCommand(
    ECommand const  cmd,
    MUINTPTR const  arg1 /*= 0*/,
    uint32_t const  arg2 /*= 0*/,
    uint32_t const  arg3
)
{
    return  mpImpShot->sendCommand(cmd, arg1, arg2, arg3);
}


////////////////////////////////////////////////////////////////////////////////
//  ImpShot
////////////////////////////////////////////////////////////////////////////////


/******************************************************************************
 *
 ******************************************************************************/
ImpShot::
ImpShot(
    char const*const pszShotName,
    uint32_t const u4ShotMode,
    int32_t const i4OpenId
)
    : ms8ShotName(String8(pszShotName))
    , mu4ShotMode(u4ShotMode)
    , mPipelineMode(ePipelineMode_Null)
    , mi4OpenId(i4OpenId)
    , mpShotCallback(NULL)
    , mShotParam()
    , mJpegParam()
    , mi4DataMsgSet(0)
{
}


/******************************************************************************
 *
 ******************************************************************************/
ImpShot::
~ImpShot()
{
    MY_LOGD("+");
    if  ( mpShotCallback != 0 ) {
        MY_LOGD("mpShotCallback.get(%p), mpShotCallback->getStrongCount(%d)", mpShotCallback.get(), mpShotCallback->getStrongCount());
    }
    MY_LOGD("-");
}


/******************************************************************************
 *
 ******************************************************************************/
bool
ImpShot::
setCallback(sp<IShotCallback>& rpShotCallback)
{
    MY_LOGD("+ rpShotCallback(%p), rpShotCallback->getStrongCount(%d)", rpShotCallback.get(), rpShotCallback->getStrongCount());
    mpShotCallback = rpShotCallback;
    return  (mpShotCallback != 0);
}


/******************************************************************************
 *
 ******************************************************************************/
bool
ImpShot::
setShotParam(void const* pParam, size_t const size)
{
    if  ( ! pParam )
    {
        MY_LOGE("Null pointer to ShotParam");
        return  false;
    }
    //
    if  ( size != sizeof(ShotParam) )
    {
        MY_LOGE("size[%d] != sizeof(ShotParam)[%d]; please fully build source codes", size, sizeof(ShotParam));
        return  false;
    }
    //
    mShotParam = *reinterpret_cast<ShotParam const*>(pParam);
    return  true;
}


/******************************************************************************
 *
 ******************************************************************************/
bool
ImpShot::
setJpegParam(void const* pParam, size_t const size)
{
    if  ( ! pParam )
    {
        MY_LOGE("Null pointer to JpegParam");
        return  false;
    }
    //
    if  ( size != sizeof(JpegParam) )
    {
        MY_LOGE("size[%d] != sizeof(JpegParam)[%d]; please fully build source codes", size, sizeof(JpegParam));
        return  false;
    }
    //
    mJpegParam = *reinterpret_cast<JpegParam const*>(pParam);
    return  true;
}


/******************************************************************************
 *
 ******************************************************************************/
bool
ImpShot::
sendCommand(
    uint32_t const  cmd,
    MUINTPTR const  arg1,
    uint32_t const  arg2,
    uint32_t const  arg3
)
{
    switch  (cmd)
    {
    //  This command is to set shot-related parameters.
    //
    //  Arguments:
    //      arg1
    //          [I] Pointer to ShotParam (i.e. ShotParam const*)
    //      arg2
    //          [I] sizeof(ShotParam)
    case eCmd_setShotParam:
        return  setShotParam(reinterpret_cast<void const*>(arg1), arg2);

    //  This command is to set jpeg-related parameters.
    //
    //  Arguments:
    //      arg1
    //          [I] Pointer to JpegParam (i.e. JpegParam const*)
    //      arg2
    //          [I] sizeof(JpegParam)
    case eCmd_setJpegParam:
        return  setJpegParam(reinterpret_cast<void const*>(arg1), arg2);

    //  This command is to set pipeline mode.
    //
    //  Arguments:
    //      arg2
    //          [I] pipeline mode
    case eCmd_setPipelineMode:
        mPipelineMode = static_cast<EPipelineMode>(arg2);
        return true;

    default:
        break;
    }
    MY_LOGW("Do nothing (cmd, arg1, arg2, arg3)=(%x, %d, %d, %d)", cmd, arg1, arg2, arg3);
    return  false;
}


/******************************************************************************
*
*******************************************************************************/
bool
ImpShot::
isDataMsgEnabled(MINT32 const i4MsgTypes)
{
    return (i4MsgTypes & mi4DataMsgSet);
}

/******************************************************************************
*
*******************************************************************************/
void
ImpShot::
enableDataMsg(MINT32 const i4MsgTypes)
{
    ::android_atomic_or(i4MsgTypes, &mi4DataMsgSet);
}

/******************************************************************************
*
*******************************************************************************/
void
ImpShot::
disableDataMsg(MINT32 const i4MsgTypes)
{
    ::android_atomic_and(~i4MsgTypes, &mi4DataMsgSet);
}

/******************************************************************************
*
*******************************************************************************/
bool
ImpShot::
updatePostNRSetting(MINT32 const nrtype, bool const isMfll, IMetadata* pMeta)
{
    NSCam::IHalSensorList* pHalSensorList = NSCam::IHalSensorList::get();
    if( pHalSensorList->queryType(getOpenId()) == NSCam::NSSensorType::eYUV ) {
        // yuv sensor not support multi-pass NR
        updateSingleEntry<MINT32>(pMeta, MTK_NR_MODE, MTK_NR_MODE_OFF);
        return true;
    }
    MUINT const sensorDev = pHalSensorList->querySensorDevIdx(getOpenId());
    //
    int hw_threshold = -1;
    int swnr_threshold = -1;
    // get threshold from custom folder
    get_capture_nr_th(
            sensorDev, getShotMode(), isMfll,
            &hw_threshold, &swnr_threshold);
    //
    MY_LOGD("shot mode(%d), nrtype(%d), threshold(%d/%d)",
            getShotMode(), nrtype, hw_threshold, swnr_threshold);
    //
    updateSingleEntry<MINT32>(pMeta, MTK_NR_MODE, nrtype);
    updateSingleEntry<MINT32>(pMeta, MTK_NR_MNR_THRESHOLD_ISO, hw_threshold);
    updateSingleEntry<MINT32>(pMeta, MTK_NR_SWNR_THRESHOLD_ISO, swnr_threshold);
    //
    return true;
}
