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

#include "DepthMapPipeUtils.h"
#include "DepthMapPipe_Common.h"



// #include <mtkcam/featureio/eis_hal.h>
#include <metadata/mtk_platform_metadata_tag.h>
#include <metadata/client/mtk_metadata_tag.h>

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {
// todo: REMOVE THIS
int EIS_FACTOR = 120;

ImageBufInfoMap::
ImageBufInfoMap(sp<EffectRequest> ptr)
:mvImgBufData(), mpReqPtr(ptr)
{

}

ssize_t
ImageBufInfoMap::
addImageBuffer(const DepthMapBufferID& key, const SmartImageBuffer& value)
{
    return mvImgBufData.add(key, value);
}

ssize_t
ImageBufInfoMap::
addGraphicBuffer(const DepthMapBufferID& key, const SmartGraphicBuffer& value)
{
    return mvGraImgBufData.add(key, value);
}

ssize_t
ImageBufInfoMap::
delImageBuffer(const DepthMapBufferID& key)
{
    return mvImgBufData.removeItem(key);
}

ssize_t
ImageBufInfoMap::
delGraphicBuffer(const DepthMapBufferID& key)
{
    return mvGraImgBufData.removeItem(key);
}

const SmartImageBuffer&
ImageBufInfoMap::
getImageBuffer(const DepthMapBufferID& key)
{
    return mvImgBufData.valueFor(key);
}

const SmartGraphicBuffer&
ImageBufInfoMap::
getGraphicBuffer(const DepthMapBufferID& key)
{
    return mvGraImgBufData.valueFor(key);
}

DepthNodeOpState getRequestState(EffectRequestPtr &request)
{
    sp<EffectParameter> pReqParam = request->getRequestParameter();
    DepthNodeOpState eState = (DepthNodeOpState) pReqParam->getInt(DEPTHMAP_REQUEST_STATE_KEY);
    return eState;
}

//  ============================  EIS utility  ============================
MBOOL
queryEisRegion(
    IMetadata* const inHal,
    eis_region& region,
    EffectRequestPtr request
)
{
    IMetadata::IEntry entry = inHal->entryFor(MTK_EIS_REGION);

#if SUPPORT_EIS_MV
    // get EIS's motion vector
    if (entry.count() > 8)
    {
        MINT32 x_mv         = entry.itemAt(6, Type2Type<MINT32>());
        MINT32 y_mv         = entry.itemAt(7, Type2Type<MINT32>());
        region.is_from_zzr  = entry.itemAt(8, Type2Type<MINT32>());
        MBOOL x_mv_negative = x_mv >> 31;
        MBOOL y_mv_negative = y_mv >> 31;
        // convert to positive for getting parts of int and float if negative
        if (x_mv_negative) x_mv = ~x_mv + 1;
        if (y_mv_negative) y_mv = ~y_mv + 1;
        //
        region.x_mv_int   = (x_mv & (~0xFF)) >> 8;
        region.x_mv_float = (x_mv & (0xFF)) << 31;
        if(x_mv_negative){
            region.x_mv_int   = ~region.x_mv_int + 1;
            region.x_mv_float = ~region.x_mv_float + 1;
        }
        region.y_mv_int   = (y_mv& (~0xFF)) >> 8;
        region.y_mv_float = (y_mv& (0xFF)) << 31;
        if(y_mv_negative){
            region.y_mv_int   = ~region.y_mv_int + 1;
            region.y_mv_float = ~region.x_mv_float + 1;
        }
        //
        MY_LOGD(mbEnableLog, "EIS MV:%d, %d, %d",
                        region.s.w,
                        region.s.h,
                        region.is_from_zzr);

        // calculate x_int/y_int from mv
        FrameInfoPtr sourceFrameInfo;
        sp<IImageBuffer> pSrcImgBuf;
        DepthMapBufferID srcBID = isFromRRZ ? BID_P2AFM_IN_RSRAW1 : BID_P2AFM_IN_FSRAW ;
        // get frame buffer
        if(request->vInputFrameInfo.indexOfKey(srcBID) >= 0)
        {
            FrameInfoPtr frameInfo = request->vInputFrameInfo.valueFor(BID_P2AFM_IN_RSRAW1);
            frameInfo->getFrameBuffer(pSrcImgBuf);
        }
        else
        {
            MY_LOGE("Cannot find input frame info, BID=%d", srcBID);
            return MFALSE;
        }

        MSize sourceSize = pSrcImgBuf->getImgSize();
         // eisCenterStart is the left-up position of eis crop region when no eis offset, that is the eis crop region is located in the center.
        MPoint eisCenterStart;
        eisCenterStart.x = sourceSize.w / (EIS_FACTOR/100.0) * (EIS_FACTOR-100)/100 * 0.5;
        eisCenterStart.y = sourceSize.h / (EIS_FACTOR/100.0) * (EIS_FACTOR-100)/100 * 0.5;

        region.x_int = eisCenterStart.x + mv_x;
        region.y_int = eisCenterStart.y + mv_y;
        region.s = sourceSize / (EIS_FACTOR/100.0);

        return MTRUE;
    }
#else
    // get EIS's region
    if (entry.count() > 5)
    {
        region.x_int        = entry.itemAt(0, Type2Type<MINT32>());
        region.x_float      = entry.itemAt(1, Type2Type<MINT32>());
        region.y_int        = entry.itemAt(2, Type2Type<MINT32>());
        region.y_float      = entry.itemAt(3, Type2Type<MINT32>());
        region.s.w          = entry.itemAt(4, Type2Type<MINT32>());
        region.s.h          = entry.itemAt(5, Type2Type<MINT32>());
        //
        MY_LOGD("EIS Region: %d, %d, %d, %d, %dx%d",
                        region.x_int,
                        region.x_float,
                        region.y_int,
                        region.y_float,
                        region.s.w,
                        region.s.h);
       return MTRUE;
    }
#endif
    MY_LOGE("wrong eis region count %zu", entry.count());
    return MFALSE;
}

/**
 * Check EIS is on or off
 * @param[in]  P1 HalMeta
 *
 * @return
 *  -  [bool] on/off status
 */

MBOOL
isEISOn(
    IMetadata* const inApp
)
{
    MUINT8 eisMode = MTK_CONTROL_VIDEO_STABILIZATION_MODE_OFF;
    if( !tryGetMetadata<MUINT8>(inApp, MTK_CONTROL_VIDEO_STABILIZATION_MODE, eisMode) ) {
        MY_LOGD("no MTK_CONTROL_VIDEO_STABILIZATION_MODE");
    }

    return eisMode == MTK_CONTROL_VIDEO_STABILIZATION_MODE_ON;
}

/**
 * Get metadata from FrameInfo
 * @param[in]  pFrameInfo: FrameInfo pointer
 *
 * @return
 *  -  IMetadata pointer
 */
IMetadata* getMetadataFromFrameInfoPtr(sp<EffectFrameInfo> pFrameInfo)
{
    IMetadata* result;
    sp<EffectParameter> effParam = pFrameInfo->getFrameParameter();
    result = reinterpret_cast<IMetadata*>(effParam->getPtr(VSDOF_EFFECT_PARAMS_KEY));
    return result;
}


} //NSFeaturePipe
} //NSCamFeature
} //NSCam