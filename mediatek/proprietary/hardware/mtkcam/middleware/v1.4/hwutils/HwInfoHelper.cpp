/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
#define LOG_TAG "MtkCam/HwInfoHelper"
//
#include <Log.h>
#if defined(__func__)
#undef __func__
#endif
#define __func__ __FUNCTION__

#define MY_LOGV(fmt, arg...)        CAM_LOGV("[%s] " fmt, __func__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_LOGD("[%s] " fmt, __func__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_LOGI("[%s] " fmt, __func__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_LOGW("[%s] " fmt, __func__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_LOGE("[%s] " fmt, __func__, ##arg)
#define MY_LOGA(fmt, arg...)        CAM_LOGA("[%s] " fmt, __func__, ##arg)
#define MY_LOGF(fmt, arg...)        CAM_LOGF("[%s] " fmt, __func__, ##arg)
#define FUNC_START                  MY_LOGD("+")
#define FUNC_END                    MY_LOGD("-")
//

#include <imageio/ispio_utility.h>
#include <hwutils/HwInfoHelper.h>

using namespace NSCamHW;
using namespace NSCam;

/*******************************************************************************
*
********************************************************************************/
////////////////////////////////////////////////////////////////////////////////
//

/*******************************************************************************
*
********************************************************************************/
HwInfoHelper::
HwInfoHelper(MINT32 const openId)
    : mOpenId(openId)
{
    memset(&mSensorStaticInfo, 0, sizeof(SensorStaticInfo));
}


/*******************************************************************************
*
********************************************************************************/
MBOOL
HwInfoHelper::
updateInfos()
{
    IHalSensorList* pSensorList = IHalSensorList::get();
    if( ! pSensorList ) {
        MY_LOGE("cannot get sensorlist");
        return MFALSE;
    }
    MUINT32 sensorDev = pSensorList->querySensorDevIdx(mOpenId);
    pSensorList->querySensorStaticInfo(sensorDev, &mSensorStaticInfo);
    return MTRUE;
}


/*******************************************************************************
*
********************************************************************************/
MBOOL
HwInfoHelper::
getSensorSize(MUINT32 const sensorMode, MSize& size) const
{
    switch(sensorMode)
    {
#define scenario_case(scenario, KEY, size_var)          \
        case scenario:                                  \
            size_var.w = mSensorStaticInfo.KEY##Width;  \
            size_var.h = mSensorStaticInfo.KEY##Height; \
            break;
        scenario_case( SENSOR_SCENARIO_ID_NORMAL_PREVIEW, preview, size );
        scenario_case( SENSOR_SCENARIO_ID_NORMAL_CAPTURE, capture, size );
        scenario_case( SENSOR_SCENARIO_ID_NORMAL_VIDEO, video, size );
        scenario_case( SENSOR_SCENARIO_ID_SLIM_VIDEO1, video1, size );
        scenario_case( SENSOR_SCENARIO_ID_SLIM_VIDEO2, video2, size );
        scenario_case( SENSOR_SCENARIO_ID_CUSTOM1, SensorCustom1, size);
        scenario_case( SENSOR_SCENARIO_ID_CUSTOM2, SensorCustom2, size);
        scenario_case( SENSOR_SCENARIO_ID_CUSTOM3, SensorCustom3, size);
        scenario_case( SENSOR_SCENARIO_ID_CUSTOM4, SensorCustom4, size);
        scenario_case( SENSOR_SCENARIO_ID_CUSTOM5, SensorCustom5, size);
#undef scenario_case
        default:
            MY_LOGE("not support sensor scenario(0x%x)", sensorMode);
            return MFALSE;
    }
    return MTRUE;
}


/*******************************************************************************
*
********************************************************************************/
MBOOL
HwInfoHelper::
getSensorFps(MUINT32 const sensorMode, MINT32& fps) const
{
    switch(sensorMode)
    {
#define scenario_case(scenario, KEY, fps_var)              \
        case scenario:                                     \
            fps_var = mSensorStaticInfo.KEY##FrameRate/10; \
            break;
        scenario_case( SENSOR_SCENARIO_ID_NORMAL_PREVIEW, preview, fps );
        scenario_case( SENSOR_SCENARIO_ID_NORMAL_CAPTURE, capture, fps );
        scenario_case( SENSOR_SCENARIO_ID_NORMAL_VIDEO, video, fps );
        scenario_case( SENSOR_SCENARIO_ID_SLIM_VIDEO1, video1, fps );
        scenario_case( SENSOR_SCENARIO_ID_SLIM_VIDEO2, video2, fps );
        scenario_case( SENSOR_SCENARIO_ID_CUSTOM1, custom1, fps);
        scenario_case( SENSOR_SCENARIO_ID_CUSTOM2, custom2, fps);
        scenario_case( SENSOR_SCENARIO_ID_CUSTOM3, custom3, fps);
        scenario_case( SENSOR_SCENARIO_ID_CUSTOM4, custom4, fps);
        scenario_case( SENSOR_SCENARIO_ID_CUSTOM5, custom5, fps);

#undef scenario_case
        default:
            MY_LOGE("not support sensor scenario(0x%x)", sensorMode);
            return MFALSE;
    }
    return MTRUE;
}


/*******************************************************************************
*
********************************************************************************/
MBOOL
HwInfoHelper::
getImgoFmt(MUINT32 const bitDepth, MINT& fmt) const
{
#define case_Format( condition, mappedfmt, fmt_var) \
        case condition:                             \
            fmt_var = mappedfmt;                    \
        break;

    if( isYuv() )
    {
        switch( mSensorStaticInfo.sensorFormatOrder )
        {
            case_Format( SENSOR_FORMAT_ORDER_UYVY, eImgFmt_UYVY, fmt);
            case_Format( SENSOR_FORMAT_ORDER_VYUY, eImgFmt_VYUY, fmt);
            case_Format( SENSOR_FORMAT_ORDER_YUYV, eImgFmt_YUY2, fmt);
            case_Format( SENSOR_FORMAT_ORDER_YVYU, eImgFmt_YVYU, fmt);
            default:
            MY_LOGE("formatOrder not supported: 0x%x", mSensorStaticInfo.sensorFormatOrder);
            return MFALSE;
        }
    }
    else if( isRaw() )
    {
        switch( bitDepth )
        {
            case_Format(  8, eImgFmt_BAYER8 , fmt);
            case_Format( 10, eImgFmt_BAYER10, fmt);
            case_Format( 12, eImgFmt_BAYER12, fmt);
            case_Format( 14, eImgFmt_BAYER14, fmt);
            default:
            MY_LOGE("bitdepth not supported: %d", bitDepth);
            return MFALSE;
        }
    }
    else
    {
        MY_LOGE("sensorType not supported yet(0x%x)", mSensorStaticInfo.sensorType);
        return MFALSE;
    }
#undef case_Format
    return MTRUE;
}


/*******************************************************************************
*
********************************************************************************/
MBOOL
HwInfoHelper::
getRrzoFmt(MUINT32 const bitDepth, MINT& fmt) const
{
    if( isYuv() )
    {
        return MFALSE;
    }
    else if( isRaw() )
    {
        switch( bitDepth )
        {
#define case_Format( condition, mappedfmt, fmt_var) \
        case condition:                             \
            fmt_var = mappedfmt;                    \
        break;
            case_Format(  8, eImgFmt_FG_BAYER8 , fmt);
            case_Format( 10, eImgFmt_FG_BAYER10, fmt);
            case_Format( 12, eImgFmt_FG_BAYER12, fmt);
            case_Format( 14, eImgFmt_FG_BAYER14, fmt);
#undef case_Format
            default:
            MY_LOGE("bitdepth not supported: %d", bitDepth);
            return MFALSE;
        }
    }
    else
    {
        MY_LOGE("sensorType not supported yet(0x%x)", mSensorStaticInfo.sensorType);
        return MFALSE;
    }
    return MTRUE;

}


/*******************************************************************************
*
********************************************************************************/
MBOOL
HwInfoHelper::
queryPixelMode(
    MUINT32 const sensorMode,
    MINT32 const fps,
    MUINT32& pixelMode
) const
{
    IHalSensor* pSensorHalObj = NULL;
    IHalSensorList* const pHalSensorList = IHalSensorList::get();
    //
    if( !pHalSensorList ) { MY_LOGE("pHalSensorList == NULL"); return MFALSE; }

    pSensorHalObj = pHalSensorList->createSensor(LOG_TAG, mOpenId);
    if( pSensorHalObj == NULL ) { MY_LOGE("pSensorHalObj is NULL"); return MFALSE; }

    pSensorHalObj->sendCommand(
            pHalSensorList->querySensorDevIdx(mOpenId),
            SENSOR_CMD_GET_SENSOR_PIXELMODE,
            (MUINTPTR)(&sensorMode),
            (MUINTPTR)(&fps),
            (MUINTPTR)(&pixelMode));

    pSensorHalObj->destroyInstance(LOG_TAG);

    if( pixelMode != 0 && pixelMode != 1 ) {
        MY_LOGE("Un-supported pixel mode %d", pixelMode);
        return MFALSE;
    }

    return MTRUE;
}


/*******************************************************************************
*
********************************************************************************/
MBOOL
HwInfoHelper::
alignPass1HwLimitation(
    MUINT32 const pixelMode,
    MINT const imgFormat,
    MBOOL isImgo,
    MSize& size,
    size_t& stride
) const
{
    NSImageio::NSIspio::ISP_QUERY_RST queryRst;
    NSImageio::NSIspio::ISP_QuerySize(
            isImgo ?
            NSImageio::NSIspio::EPortIndex_IMGO:
            NSImageio::NSIspio::EPortIndex_RRZO,
            NSImageio::NSIspio::ISP_QUERY_X_PIX|
            NSImageio::NSIspio::ISP_QUERY_STRIDE_PIX|
            NSImageio::NSIspio::ISP_QUERY_STRIDE_BYTE,
            (EImageFormat)imgFormat,
            size.w,
            queryRst,
            pixelMode == 0 ?
            NSImageio::NSIspio::ISP_QUERY_1_PIX_MODE :
            NSImageio::NSIspio::ISP_QUERY_2_PIX_MODE
            );
    size.w = queryRst.x_pix;
    size.h = ((size.h + 1) & (~1));
    stride = queryRst.stride_byte;
    MY_LOGD("rrzo size %dx%d, stride %d", size.w, size.h, stride);
    return MTRUE;
}


/*******************************************************************************
*
********************************************************************************/
MBOOL
HwInfoHelper::
alignRrzoHwLimitation(
    MSize const targetSize,
    MSize const sensorSize,
    MSize& result
) const
{
    result = targetSize;
#define RRZO_SCALE_RATIO_10X    (4)
    if( result.w < sensorSize.w * RRZO_SCALE_RATIO_10X / 10 ||
        result.h < sensorSize.h * RRZO_SCALE_RATIO_10X / 10 )
    {
        MY_LOGD("lower: refine %dx%d", result.w, result.h);
        result =
            MSize(sensorSize.w * RRZO_SCALE_RATIO_10X / 10, sensorSize.h * RRZO_SCALE_RATIO_10X / 10);
    }
    //
    if( result.w > sensorSize.w || result.h > sensorSize.h ) {
        MY_LOGD("upper: refine %dx%d", result.w, result.h);
        result = sensorSize;
    }
    //
    return MTRUE;
#undef RRZO_SCALE_RATIO_10X
}


