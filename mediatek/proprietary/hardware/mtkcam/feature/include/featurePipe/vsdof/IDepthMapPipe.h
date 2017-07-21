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

#ifndef _MTK_CAMERA_FEATURE_PIPE_INTERFACE_DEPTH_MAP_PIPE_H_
#define _MTK_CAMERA_FEATURE_PIPE_INTERFACE_DEPTH_MAP_PIPE_H_

#include <utils/RefBase.h>
#include <common/include/common.h>
#include <mmsdk/EffectRequest.h>

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {

#define VSDOF_EFFECT_PARAMS_KEY "Metadata"
#define DEPTHMAP_COMPLETE_KEY "onComplete"
#define DEPTHMAP_FLUSH_KEY "onFlush"
#define DEPTHMAP_REQUEST_STATE_KEY "ReqStatus"

typedef enum DepthMapNode_BufferDataTypes{
    BID_DEPTHMAPPIPE_INVALID = 0,
    BID_REQUEST_STATE,

    BID_P2AFM_IN_FSRAW,
    BID_P2AFM_IN_RSRAW1,
    BID_P2AFM_IN_RSRAW2,

    BID_P2AFM_FE1BO_INPUT,
    BID_P2AFM_FE2BO_INPUT,
    BID_P2AFM_FE1CO_INPUT,
    BID_P2AFM_FE2CO_INPUT,

    BID_TO_FD_IMG,

    BID_TO_N3D_FE1AO,  //10
    BID_TO_N3D_FE2AO,
    BID_TO_N3D_FE1BO,
    BID_TO_N3D_FE2BO,
    BID_TO_N3D_FE1CO,
    BID_TO_N3D_FE2CO,

    BID_TO_N3D_RECT_IN1,
    BID_TO_N3D_RECT_IN2,
    BID_TO_N3D_RECT_IN1_CAP,
    BID_TO_N3D_RECT_IN2_CAP,

    BID_TO_N3D_CC_IN1,  //20
    BID_TO_N3D_CC_IN2,

    BID_TO_N3D_FMAO_LR,
    BID_TO_N3D_FMAO_RL,
    BID_TO_N3D_FMBO_LR,
    BID_TO_N3D_FMBO_RL,
    BID_TO_N3D_FMCO_LR,
    BID_TO_N3D_FMCO_RL,

    BID_TO_DPE_OCC_MV_Y,
    BID_TO_DPE_MASK_M,
    BID_TO_DPE_OCC_SV_Y,  //30
    BID_TO_DPE_MASK_S,


    BID_TO_OCC_LDC,
    BID_TO_OCC_CFM_L,
    BID_TO_OCC_CFM_R,
    BID_TO_OCC_DMP_L,
    BID_TO_OCC_DMP_R,
    BID_TO_OCC_RESPO_L,
    BID_TO_OCC_RESPO_R,

    BID_TO_WMF_DMH,
    BID_TO_WMF_MY_SL, //40

    BID_OUT_MV_F,
    BID_OUT_MV_F_CAP,
    BID_OUT_DEPTHMAP,
    BID_OUT_DMW,
    BID_OUT_MY_S,
    BID_OUT_JPS_MAIN1,
    BID_OUT_JPS_MAIN2,
    BID_OUT_FD,
    BID_OUT_EXTRADATA,

    BID_DMW_INTERNAL,
    // metadata
    BID_META_IN_APP,
    BID_META_IN_HAL,
    BID_META_IN_HAL_MAIN2,
    BID_META_OUT_APP,
    BID_META_OUT_HAL,

    BID_BOKEH_REC_YUV,
    BID_BOKEH_PV_YUV,
#ifdef GTEST
    // UT output
    BID_FE2_IN_MAIN1,
    BID_FE2_IN_MAIN2,
    BID_FE3_IN_MAIN1,
    BID_FE3_IN_MAIN2
#endif

} DepthMapBufferID;

typedef enum eDepthNodeOpState {
    STATE_NORMAL = 1,
    STATE_CAPTURE = 2
} DepthNodeOpState;

/******************************************************************************
 * The Interface of DepthMapNode's FeaturePipe
 ******************************************************************************/
class IDepthMapPipe
{
public:
    static IDepthMapPipe* createInstance(MINT32 iSensorIdx_Main1, MINT32 iSensorIdx_Main2, MUINT32 iFeatureMask=0);
    MBOOL destroyInstance();
    // destructor
    virtual ~IDepthMapPipe() {};
public:
    virtual MBOOL init() = 0;
    virtual MBOOL uninit() = 0;
    virtual MBOOL enque(android::sp<EffectRequest>& request) = 0;
    virtual MVOID flush() = 0;
    virtual MVOID setFlushOnStop(MBOOL flushOnStop) = 0;
};

}; // NSFeaturePipe
}; // NSCamFeature
}; // NSCam

#endif