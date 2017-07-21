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

#include "OCCNode.h"
#include "DepthMapPipe_Common.h"

#define PIPE_CLASS_TAG "OCCNode"
#include <featurePipe/core/include/PipeLog.h>

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {

OCCNode::BufferPoolSet::
BufferPoolSet()
{}

OCCNode::BufferPoolSet::
~BufferPoolSet()
{}

MBOOL
OCCNode::BufferPoolSet::
init(const MSize& szDMHSize)
{
    mDMHImgBufPool = ImageBufferPool::create("DMHBufPool", szDMHSize.w, szDMHSize.h,
                                                eImgFmt_Y8, eBUFFER_USAGE_SW_WRITE_OFTEN | eBUFFER_USAGE_SW_READ_OFTEN | eBUFFER_USAGE_HW_CAMERA_READ);

    ALLOCATE_BUFFER_POOL(mDMHImgBufPool, VSDOF_WORKING_BUF_SET);
    return MTRUE;
}

MBOOL
OCCNode::BufferPoolSet::
uninit()
{
    ImageBufferPool::destroy(mDMHImgBufPool);
    return MTRUE;
}

OCCNode::
OCCNode(const char* name, Graph_T *graph)
: DepthMapPipeNode(name, graph), mpOCCHAL(NULL)
{
    this->addWaitQueue(&mJobQueue);
    this->addWaitQueue(&mLDCBufQueue);

    // setup constants
    StereoSizeProvider * pSizeProvder = StereoSizeProvider::getInstance();
    VSDOF_OCC_OUTPUT_DMH_SIZE = pSizeProvder->getBufferSize(E_DMH, eSTEREO_SCENARIO_RECORD);
}

OCCNode::
~OCCNode()
{
}

MVOID
OCCNode::
cleanUp()
{
    VSDOF_LOGD("+");
    if(mpOCCHAL!=NULL)
    {
        delete mpOCCHAL;
        mpOCCHAL = NULL;
    }
    mJobQueue.clear();
    mLDCBufQueue.clear();
    mLDCImageBufMap.clear();
    mBufPoolSet.uninit();
    VSDOF_LOGD("-");
}

MBOOL
OCCNode::
onInit()
{
    VSDOF_LOGD("+");
    // buffer pool set init
    mBufPoolSet.init(VSDOF_OCC_OUTPUT_DMH_SIZE);
    // OCC HAL initialization
    mpOCCHAL = OCC_HAL::createInstance();
    VSDOF_LOGD("-");

    return MTRUE;
}

MBOOL
OCCNode::
onUninit()
{
    cleanUp();
    return MTRUE;
}

MBOOL
OCCNode::
onThreadStart()
{
    return MTRUE;
}

MBOOL
OCCNode::
onThreadStop()
{
    return MTRUE;
}


MBOOL
OCCNode::
onData(DataID data, ImgInfoMapPtr& pImgInfo)
{
    MBOOL ret = MTRUE;
    VSDOF_PRFLOG("+, DataID=%d reqId=%d", data, pImgInfo->getRequestPtr()->getRequestNo());

    switch(data)
    {
        case N3D_TO_OCC_LDC:
        {
            mLDCBufQueue.enque(pImgInfo);
            break;
        }
        case DPE_TO_OCC_MVSV_DMP_CFM:
            mJobQueue.enque(pImgInfo);
            break;
        default:
            MY_LOGW("Unrecongnized DataID=%d", data);
            ret = MFALSE;
            break;
    }

    TRACE_FUNC_EXIT();
    return ret;
}

MBOOL
OCCNode::
onThreadLoop()
{
    ImgInfoMapPtr pDPEImgInfo;
    ImgInfoMapPtr pN3DImgInfo;
    EffectRequestPtr reqPtr;

    if( !waitAllQueue() )
    {
        // mJobs.abort() called
        MY_LOGE("waitAllQueue failed");
        return MFALSE;
    }

    if( !mJobQueue.deque(pDPEImgInfo) )
    {
        MY_LOGE("mJobQueue.deque() failed");
        return MFALSE;
    }

    if( !mLDCBufQueue.deque(pN3DImgInfo) )
    {
        MY_LOGE("mLDCBufQueue.deque() failed");
        return MFALSE;
    }

    if(pDPEImgInfo->getRequestPtr()->getRequestNo() != pN3DImgInfo->getRequestPtr()->getRequestNo())
    {
        MY_LOGE("The DPE ImgInfo is different fron N3D ImgInfo.");
        return MFALSE;
    }

    // mark on-going-request start
    this->incExtThreadDependency();

    reqPtr = pDPEImgInfo->getRequestPtr();
    MUINT32 iReqNo = reqPtr->getRequestNo();
    VSDOF_PRFLOG("OCC threadloop start, reqID=%d", iReqNo);

    OCC_HAL_PARAMS occInputParams;
    OCC_HAL_OUTPUT occOutputParams;
    ImgInfoMapPtr toWMFImgInfo = new ImageBufInfoMap(reqPtr);
    // prepare in/out params
    bool ret = prepareOCCParams(pDPEImgInfo, pN3DImgInfo, occInputParams, occOutputParams, toWMFImgInfo);

    if(!ret)
    {
        MY_LOGE("OCC ALGO stopped because of the enque parameter error.");
        return MFALSE;
    }

    VSDOF_PRFLOG("OCC ALGO start, reqID=%d", reqPtr->getRequestNo());

    mpOCCHAL->OCCHALRun(occInputParams, occOutputParams);

    VSDOF_PRFLOG("OCC ALGO end, reqID=%d", reqPtr->getRequestNo());

    // output to WMF node
    handleDataAndDump(OCC_TO_WMF_DMH_MY_S, toWMFImgInfo);
    // dump MY_S
    FrameInfoPtr frameInfo_MY_S = reqPtr->vOutputFrameInfo.valueFor(BID_OUT_MY_S);
    handleDump(OCC_TO_WMF_DMH_MY_S, frameInfo_MY_S, "BID_OUT_MY_S");

    // mark on-going-request end
    this->decExtThreadDependency();

    return MTRUE;
}

const char*
OCCNode::
onDumpBIDToName(DepthMapBufferID BID)
{
#define MAKE_NAME_CASE(name) \
    case name: return #name;
    switch(BID)
    {
        MAKE_NAME_CASE(BID_TO_WMF_DMH);
        default:
            MY_LOGW("unknown BID:%d", BID);
            return "unknown";
    }

#undef MAKE_NAME_CASE
}

MBOOL
OCCNode::
prepareOCCParams(ImgInfoMapPtr& pDPEImgInfo, ImgInfoMapPtr& pN3DImgInfo, OCC_HAL_PARAMS& enqueParams, OCC_HAL_OUTPUT& outputParams, ImgInfoMapPtr& toWMFImgInfo)
{
    SmartImageBuffer smMV_Y = pDPEImgInfo->getImageBuffer(BID_TO_DPE_OCC_MV_Y);
    SmartImageBuffer smSV_Y = pDPEImgInfo->getImageBuffer(BID_TO_DPE_OCC_SV_Y);
    SmartImageBuffer smDMP_L = pDPEImgInfo->getImageBuffer(BID_TO_OCC_DMP_L);
    SmartImageBuffer smDMP_R = pDPEImgInfo->getImageBuffer(BID_TO_OCC_DMP_R);

    enqueParams.imageMain1 = smMV_Y->mImageBuffer.get();
    enqueParams.imageMain2 = smSV_Y->mImageBuffer.get();
    enqueParams.disparityLeftToRight = (MUINT16*) smDMP_L->mImageBuffer->getBufVA(0);
    enqueParams.disparityRightToLEft = (MUINT16*) smDMP_R->mImageBuffer->getBufVA(0);
    enqueParams.confidenceMap = NULL;

    // push LDC info
    SmartImageBuffer smLDC = pN3DImgInfo->getImageBuffer(BID_TO_OCC_LDC);
    enqueParams.ldcMain1 = (MUINT8*) smLDC->mImageBuffer->getBufVA(0);

    // get output MY_S
    FrameInfoPtr frameInfo_MY_S;
    sp<IImageBuffer> pImgBuf_MY_S;
    RETRIEVE_OFRAMEINFO_IMGBUF(pDPEImgInfo->getRequestPtr(), frameInfo_MY_S, BID_OUT_MY_S, pImgBuf_MY_S);
    if(frameInfo_MY_S.get())
    {
        outputParams.downScaledImg = pImgBuf_MY_S.get();
    }
    else
    {
        MY_LOGE("Cannot find MY_S frameInfo to generate the input params for OCC.");
        return MFALSE;
    }
    // DMH
    SmartImageBuffer smDMH = mBufPoolSet.mDMHImgBufPool->request();
    toWMFImgInfo->addImageBuffer(BID_TO_WMF_DMH, smDMH);
    outputParams.depthMap = (MUINT8*) smDMH->mImageBuffer->getBufVA(0);

    debugOCCParams(smMV_Y->mImageBuffer, smSV_Y->mImageBuffer, smDMP_L->mImageBuffer, smDMP_R->mImageBuffer,  NULL, smLDC->mImageBuffer, pImgBuf_MY_S, smDMH->mImageBuffer);

    return MTRUE;
}

MVOID
OCCNode::
debugOCCParams(sp<IImageBuffer> smMV_Y, sp<IImageBuffer> smSV_Y, sp<IImageBuffer> smDMP_L, sp<IImageBuffer> smDMP_R, sp<IImageBuffer> confidenceMap, sp<IImageBuffer> ldcMap, sp<IImageBuffer> downScaledImg, sp<IImageBuffer> depthMap)
{
    if(!mbDebugLog)
        return;
    #define OUTPUT_IMG_BUFF(imageBuf)\
        if(imageBuf!=NULL)\
        {\
            MY_LOGD("=======================:" # imageBuf);\
            MY_LOGD("imageBuff size=%dx%d", imageBuf->getImgSize().w, imageBuf->getImgSize().h);\
            MY_LOGD("imageBuff plane count=%d", imageBuf->getPlaneCount());\
            MY_LOGD("imageBuff format=%x", imageBuf->getImgFormat());\
            MY_LOGD("imageBuff getImgBitsPerPixel=%d", imageBuf->getImgBitsPerPixel());\
            MY_LOGD("=======================");\
        }

    MY_LOGD("Input::");
    OUTPUT_IMG_BUFF(smMV_Y);
    OUTPUT_IMG_BUFF(smSV_Y);
    OUTPUT_IMG_BUFF(smDMP_L);
    OUTPUT_IMG_BUFF(smDMP_R);
    OUTPUT_IMG_BUFF(confidenceMap);

    MY_LOGD("Output::");
    OUTPUT_IMG_BUFF(downScaledImg);
    OUTPUT_IMG_BUFF(depthMap);

    #undef OUTPUT_IMG_BUFF
}


}; //NSFeaturePipe
}; //NSCamFeature
}; //NSCam
