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

#include "P2ACAPNode.h"
#include "DepthMapPipe_Common.h"

// TODO: make sure the isp_reg.h header location
#include <drv/isp_reg.h>
#include <libion_mtk/include/ion.h>
#include <metadata/mtk_platform_metadata_tag.h>

#define PIPE_CLASS_TAG "P2ACAP Node"
#include <featurePipe/core/include/PipeLog.h>

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {

P2ACAPNode::
P2ACAPNode(const char* name, Graph_T* graph, MUINT32 iSensorIdx_Main1, MUINT32 iSensorIdx_Main2)
:DepthMapPipeNode(name, graph), miSensorIdx_Main1(iSensorIdx_Main1),
 miSensorIdx_Main2(iSensorIdx_Main2)
{
    VSDOF_LOGD("Sensor index, Main1: %d, Main2: %d", miSensorIdx_Main1, miSensorIdx_Main2);
    this->addWaitQueue(&mJobQueue);
}

P2ACAPNode::
~P2ACAPNode()
{}

MVOID
P2ACAPNode::
cleanUp()
{
    if(mpINormalStream != NULL)
    {
        mpINormalStream->uninit("VSDOF_P2ACAM");
        mpINormalStream->destroyInstance("VSDOF_P2ACAM");
        mpINormalStream = NULL;
    }

    if(mp3A)
    {
        mp3A->destroyInstance("VSDOF");
        mp3A = NULL;
    }
}

MBOOL
P2ACAPNode::
onInit()
{
    VSDOF_LOD("+");

    // Create NormalStream
    VSDOF_LOGD("NormalStream create instance: idx=%d", miSensorIdx_Main1);
    mpINormalStream = NSCam::NSIoPipe::NSPostProc::IFeatureStream::createInstance(miSensorIdx_Main1);
    if (mpINormalStream == NULL)
    {
        MY_LOGE("mpINormalStream create instance for P2ACAP Node failed!");
        cleanUp();
        return MFALSE;
    }
    mpINormalStream->init("VSDOF_P2ACAM")

    // create 3AHAL
    // 3A: create instance
    mp3A = IHal3A::createInstance(IHal3A::E_Camera_3, miSensorIdx_Main1, "VSDOF");
    if(mp3A == NULL)
    {
        MY_LOGE("IHal3A create instance for P2ACAP Node failed!");
        cleanUp();
        return MFALSE;
    }

    VSDOF_LOD("-");
    return MTRUE;
}

MBOOL
P2ACAPNode::
onUninit()
{
    cleanUp();
    return MTRUE;
}

MBOOL
P2ACAPNode::
onThreadStart()
{
    return MTRUE;
}

MBOOL
P2ACAPNode::
onThreadStop()
{
    return MTRUE;
}

MBOOL
P2ACAPNode::
onData(DataID data, ImgInfoMapPtr& ReqImgInfo)
{
    MBOOL ret = MTRUE;
    MY_LOGD("+, reqId=%d", ReqImgInfo->getRequestPtr()->getRequestNo());
    //TRACE_FUNC_ENTER();

    switch(data)
    {
        case P2AFM_TO_P2A_CAP_REQDATA:
            mJobQueue.enque(ReqImgInfo);
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
P2ACAPNode::
onThreadLoop()
{
    ImgInfoMapPtr ReqImgInfo;
    EffectRequestPtr reqPtr;

    if( !waitAllQueue() )
    {
        // mJobs.abort() called
        MY_LOGD("waitAllQueue failed");
        return MFALSE;
    }

    if( !mJobQueue.deque(ReqImgInfo) )
    {
        MY_LOGD("P2ACAPNode mJobQueue.deque() failed");
        return MFALSE;
    }

    reqPtr = ReqImgInfo->getRequestPtr();
    // mark on-going-request start
    this->incExtThreadDependency();
    // check necessary frame
    if(reqPtr->vInputFrameInfo.indexOfKey(BID_P2AFM_IN_FSRAW) < 0 ||
        reqPtr->vOutputFrameInfo.indexOfKey(BID_OUT_MV_F_CAP) < 0)
    {
        MY_LOGE("The necessary frame info does not exist!!! FSRAW=%d MV_F_CAP=%d",
            reqPtr->vInputFrameInfo.indexOfKey(BID_P2AFM_IN_FSRAW),
            reqPtr->vOutputFrameInfo.indexOfKey(BID_OUT_MV_F_CAP));
            goto lbExit;
    }


    FrameInfoPtr framePtr_FullRaw = reqPtr->vInputFrameInfo.valueFor(BID_P2AFM_IN_FSRAW);
    FrameInfoPtr framePtr_MainImg_CAP = reqPtr->vInputFrameInfo.valueFor(BID_OUT_MV_F_CAP);

    // P2 enque parameter
    QParams enqueParams;
    EnquedBufPool *pEnqueBufPool = new EnquedBufPool(reqPtr, this);

    sp<IImageBuffer> frameBuf;
    // input buffer
    Input src;
    src.mPortID = PORT_IMGI;
    src.mPortID.group = 0;
    framePtr_FullRaw->getFrameBuffer(frameBuf);
    src.mBuffer = frameBuf.get();
    enqueParams.mvIn.push_back(src);
    // output buffer

    MCrpRsInfo crop1, crop2, crop3;
    // CRZ
    crop1.mGroupID = 1;
    crop1.mCropRect.p_fractional.x=0;
    crop1.mCropRect.p_fractional.y=0;
    crop1.mCropRect.p_integral.x=0;
    crop1.mCropRect.p_integral.y=0;
    crop1.mCropRect.s.w=src.mBuffer->getImgSize().w;
    crop1.mCropRect.s.h=src.mBuffer->getImgSize().h;
    crop1.mResizeDst.w=src.mBuffer->getImgSize().w;
    crop1.mResizeDst.h=src.mBuffer->getImgSize().h;
    enqueParams.mvCropRsInfo.push_back(crop1);
    // WDMA - MainImage_Cap
    crop2.mGroupID = 2;
    crop2.mCropRect.p_fractional.x=0;
    crop2.mCropRect.p_fractional.y=0;
    crop2.mCropRect.p_integral.x=0;
    crop2.mCropRect.p_integral.y=0;
    crop2.mCropRect.s.w=src.mBuffer->getImgSize().w;
    crop2.mCropRect.s.h=src.mBuffer->getImgSize().h;
    crop2.mResizeDst.w=src.mBuffer->getImgSize().w;
    crop2.mResizeDst.h=src.mBuffer->getImgSize().h;
    enqueParams.mvCropRsInfo.push_back(crop2);
    Output out;
    out.mPortID = PORT_WDMA;
    out.mPortID.group = 0;
    framePtr_MainImg_CAP->getFrameBuffer(frameBuf)
    out.mBuffer = frameBuf.get();
    enqueParams.mvOut.push_back(out);
    // WROT
    crop3.mGroupID = 3;
    crop3.mCropRect.p_fractional.x=0;
    crop3.mCropRect.p_fractional.y=0;
    crop3.mCropRect.p_integral.x=0;
    crop3.mCropRect.p_integral.y=0;
    crop3.mCropRect.s.w=src.mBuffer->getImgSize().w;
    crop3.mCropRect.s.h=src.mBuffer->getImgSize().h;
    crop3.mResizeDst=P2AFM_MAIN_IMAGE_CAP_SIZE;
    enqueParams.mvCropRsInfo.push_back(crop3);

    //
    enqueParams.mpfnCallback = onP2Callback;
    enqueParams.mpCookie = (void*) pEnqueBufPool;

    VSDOF_LOGD("mpINormalStream->enque start! reqID=%d", reqPtr->getRequestNo());

    MBOOL ret; = mpINormalStream->enque(enqueParams);

    if(!ret)
    {
        MY_LOGE("mpINormalStream enque failed! reqID=%d", reqPtr->getRequestNo());
        goto lbExit;
    }

    VSDOF_LOGD("mpINormalStream->enque done! reqID=%d", reqPtr->getRequestNo());
    return MTRUE;

lbExit:
    // mark on-going-request end
    this->decExtThreadDependency();
    return MFALSE;
}

MVOID
P2ACAPNode::
onP2Callback(QParams& rParams)
{
    VSDOF_LOD("+");
    EnquedBufPool *pEnqueBufPool = (EnquedBufPool *) rParams.mpCookie;
    P2ACAPNode* pP2ACAPNode = pEnqueBufPool->mpNode;
    pP2ACAPNode->handleP2Done(rParams, pEnqueBufPool);
    VSDOF_LOD("-");
}

MVOID
P2ACAPNode::
handleP2Done(QParams& rParams, EnquedBufPool* pEnqueBufPool)
{
    VSDOF_LOD("+");
    EffectRequestPtr request = pEnqueBufPool->mRequest;
    TRACE_FUNC("reqID=%d", request->getRequestNo());

    // mark frame ready
    FrameInfoPtr framePtr_MainImg_CAP = request->vOutputFrameInfo.valueFor(BID_OUT_MV_F_CAP);
    framePtr_MainImg_CAP->setFrameReady(true);
    handleData(P2ACAP_OUT_MV_F_CAP, framePtr_MainImg_CAP);

    delete pEnqueBufPool;
    // mark on-going-request end
    this->decExtThreadDependency();
    VSDOF_LOD("-");

}

}; //NSFeaturePipe
}; //NSCamFeature
}; //NSCam

