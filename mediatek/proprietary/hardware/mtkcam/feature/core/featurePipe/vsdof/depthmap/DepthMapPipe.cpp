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
#include "DepthMapPipe.h"

#include <utils/String8.h>
#include <utils/Vector.h>

#define PIPE_CLASS_TAG "DepthMapPipe"
#include <featurePipe/core/include/PipeLog.h>

/******************************************************************************
 *
 ******************************************************************************/

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {

IDepthMapPipe*
IDepthMapPipe::createInstance(MINT32 iSensorIdx_Main1, MINT32 iSensorIdx_Main2, MUINT32 iFeatureMask)
{
    return new DepthMapPipe(iSensorIdx_Main1, iSensorIdx_Main2, iFeatureMask);
}

MBOOL
IDepthMapPipe::destroyInstance()
{
    delete this;
    return MTRUE;
}

DepthMapPipe::DepthMapPipe(MINT32 iSensorIdx_Main1, MINT32 iSensorIdx_Main2, MUINT32 iFeatureMask)
  : CamPipe<DepthMapPipeNode>("DepthMapPipe"),
  mP2AFMNode("P2AFM", &mCamGraph, iSensorIdx_Main1, iSensorIdx_Main2),
  mN3DNode("N3DNode", &mCamGraph), mDPENode("DPENode", &mCamGraph),
  mOCCNode("OCCNode", &mCamGraph), mWMFNode("WMFNode", &mCamGraph),
  mFDNode("SFDNode", &mCamGraph), mvRequestPtrMap()
{
    mFeatureOnMask = iFeatureMask;
    //P2AFM to N3D + P2AFM_output
    this->connectData(P2AFM_TO_N3D_FEFM_CCin, P2AFM_TO_N3D_FEFM_CCin, mP2AFMNode, mN3DNode);
    this->connectData(P2AFM_OUT_MV_F, P2AFM_OUT_MV_F, mP2AFMNode, this);
    this->connectData(P2AFM_OUT_FD, P2AFM_OUT_FD, mP2AFMNode, this);
    this->connectData(P2AFM_OUT_MV_F_CAP, P2AFM_OUT_MV_F_CAP, mP2AFMNode, this);
    // Meta frame output (DepthMap Node currently has no meta output.)
    this->connectData(DEPTHMAP_META_OUT,DEPTHMAP_META_OUT, mP2AFMNode, this);
    // P2AFM to FD
    this->connectData(P2AFM_TO_FD_IMG, P2AFM_TO_FD_IMG, mP2AFMNode, mFDNode);
    // DPE to OCC
    this->connectData(DPE_TO_OCC_MVSV_DMP_CFM, DPE_TO_OCC_MVSV_DMP_CFM, mDPENode, mOCCNode);
    // P2AFM to WMF
    this->connectData(P2AFM_TO_WMF_MY_SL, P2AFM_TO_WMF_MY_SL, mP2AFMNode, mWMFNode);
    // WMF output
    this->connectData(WMF_OUT_DMW_MY_S, WMF_OUT_DMW_MY_S, mWMFNode, this);
    this->connectData(WMF_OUT_DEPTHMAP, WMF_OUT_DEPTHMAP, mWMFNode, this);
    //N3D to DPE/OCC
    this->connectData(N3D_TO_DPE_MVSV_MASK, N3D_TO_DPE_MVSV_MASK, mN3DNode, mDPENode);
    this->connectData(N3D_TO_OCC_LDC, N3D_TO_OCC_LDC, mN3DNode, mOCCNode);
    this->connectData(N3D_OUT_JPS, N3D_OUT_JPS, mN3DNode, this);
    //N3D to FD
    this->connectData(N3D_TO_FD_EXTDATA_MASK, mN3DNode, mFDNode);
    //OCC to WMF + OCC_output
    this->connectData(OCC_TO_WMF_DMH_MY_S, OCC_TO_WMF_DMH_MY_S, mOCCNode, mWMFNode);
    this->setRootNode(&mP2AFMNode);
    // logging status
    mbDebugLog = StereoSettingProvider::isLogEnabled(PERPERTY_DEPTHMAP_NODE_LOG);
    mbProfileLog = StereoSettingProvider::isProfileLogEnabled();
}

DepthMapPipe::~DepthMapPipe()
{
    // must call dispose to free CamGraph
    this->dispose();
}

MBOOL DepthMapPipe::onInit()
{
    return MTRUE;
}

MVOID DepthMapPipe::onUninit()
{
}

MVOID
DepthMapPipe::
flush()
{
    VSDOF_PRFLOG("+");

    // lauch the default flush operations
    CamPipe::flush();

    // relase all the effectRequest
    android::Mutex::Autolock lock(mReqMapLock);
    for(size_t index=0;index<mvRequestPtrMap.size();++index)
    {
        MUINT32 iFlushReqID = mvRequestPtrMap.keyAt(index);
        VSDOF_LOGD("flush ReqID = %d", iFlushReqID);

        EffectRequestPtr pEffectReq = mvRequestPtrMap.valueAt(index);
        // callback to pipeline node with FLUSH KEY
        pEffectReq->mpOnRequestProcessed(pEffectReq->mpTag, String8( DEPTHMAP_FLUSH_KEY ), pEffectReq);
    }
    // clear all request map
    mvRequestPtrMap.clear();

    VSDOF_PRFLOG("-");
}

MBOOL
DepthMapPipe::
enque(EffectRequestPtr& request)
{
    VSDOF_PRFLOG("+");

    MUINT32 reqID = request->getRequestNo();
    // autolock for request map
    {
        VSDOF_PRFLOG("request map add reqId=%d ", reqID);
        android::Mutex::Autolock lock(mReqMapLock);
        mvRequestPtrMap.add(reqID, request);
    }
    // call parent class:FeaturePipe enque function
    MBOOL ret = CamPipe::enque(ROOT_ENQUE, request);

    VSDOF_PRFLOG("-");
    return ret;
}


MBOOL DepthMapPipe::onData(DataID id, EffectRequestPtr &data)
{
  return MTRUE;
}

MBOOL
DepthMapPipe::
onData(DataID id, FrameInfoPtr &data)
{
    VSDOF_PRFLOG("+ : reqID=%d DataID=%d", data->getRequestNo(), id);

    MUINT32 reqID;
    switch(id)
    {
        case P2AFM_OUT_MV_F:
        case P2AFM_OUT_MV_F_CAP:
        case P2AFM_OUT_FD:
        case WMF_OUT_DMW_MY_S:
        case WMF_OUT_DEPTHMAP:
        case DEPTHMAP_META_OUT:
        case N3D_OUT_JPS:
            reqID = data->getRequestNo();
            break;
        default:
            MY_LOGW("FeaturePipe_DepthMap: onGraphCB non-avaiilable id=%d", id);
            return MFALSE;
    }

    sp<EffectRequest> request;

    // autolock for request map
    {
        android::Mutex::Autolock lock(mReqMapLock);
        if(mvRequestPtrMap.indexOfKey(reqID)<0)
        {
            MY_LOGW("FeaturePipe_DepthMap: onGraphCB non-existed req_id=%d, might already return.", reqID);
            return MFALSE;
        }
        request = mvRequestPtrMap.valueFor(reqID);
    }

    size_t ouputBufSize = request->vOutputFrameInfo.size();
    MBOOL bReady=MTRUE;
    // make sure all output frame are ready
    for(size_t index=0;index<ouputBufSize;index++)
    {
        if(!request->vOutputFrameInfo[index]->isFrameBufferReady())
        {
            VSDOF_LOGD("req_id=%d Data not ready!! index=%d key=%d", reqID, index, request->vOutputFrameInfo.keyAt(index));
            bReady = MFALSE;
            break;
        }
    }
    // if ready callback to client
    if(bReady)
    {
        VSDOF_PRFLOG("Request data ready! req_id=%d", request->getRequestNo());
        request->mpOnRequestProcessed(request->mpTag, String8( DEPTHMAP_COMPLETE_KEY ), request);

        android::Mutex::Autolock lock(mReqMapLock);
        mvRequestPtrMap.removeItem(reqID);
    }

    VSDOF_LOGD("-");

    return MTRUE;
}


MBOOL DepthMapPipe::onData(DataID id, SmartImageBuffer &data)
{
  return MTRUE;
}

}; // NSFeaturePipe
}; // NSCamFeature
}; // NSCam
