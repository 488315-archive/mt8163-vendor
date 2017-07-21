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

#include "DPENode.h"
#include "DepthMapPipe_Common.h"

#include <iopipe/PostProc/DpeUtility.h>
#include <vsdof/hal/stereo_tuning_provider.h>

#define PIPE_CLASS_TAG "DPENode"
#include <featurePipe/core/include/PipeLog.h>

using namespace NSCam::NSIoPipe::NSDpe;

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {

DPENode::BufferSizeConfig::
BufferSizeConfig()
{
     // setup constans
    StereoSizeProvider* pSizePrvder = StereoSizeProvider::getInstance();
    VSDOF_DMP_SIZE = pSizePrvder->getBufferSize(E_DMP_H, eSTEREO_SCENARIO_RECORD);
    VSDOF_CFM_SIZE = pSizePrvder->getBufferSize(E_CFM_H, eSTEREO_SCENARIO_RECORD);
    VSDOF_RESPO_SIZE = pSizePrvder->getBufferSize(E_RESPO, eSTEREO_SCENARIO_RECORD);

    // original image width is the size before N3D in Cap
    Pass2SizeInfo pass2SizeInfo;
    pSizePrvder->getPass2SizeInfo(PASS2A_P_2, eSTEREO_SCENARIO_CAPTURE, pass2SizeInfo);
    VSDOF_ORG_IMG_WIDTH = pass2SizeInfo.areaWDMA.size.w;
    VSDOF_ORG_IMG_HEIGHT = pass2SizeInfo.areaWDMA.size.h;

    VSDOF_DMP_STRIDE_SIZE = DPEQueryDveOutDMAStride(DMA_DVE_DVO, DVE_HORZ_DS_MODE_DIV8, VSDOF_ORG_IMG_WIDTH);

    if(VSDOF_DMP_STRIDE_SIZE != VSDOF_DMP_SIZE.w * 2)
        MY_LOGW("VSDOF_DMP_STRIDE_SIZE is not equal to VSDOF_DMP_SIZE.w * 2");


    VSDOF_CFM_STRIDE_SIZE = DPEQueryDveOutDMAStride(DMA_DVE_CONFO, DVE_HORZ_DS_MODE_DIV8, VSDOF_ORG_IMG_WIDTH);
    VSDOF_RESPO_STRIDE_SIZE = DPEQueryDveOutDMAStride(DMA_DVE_RESPO, DVE_HORZ_DS_MODE_DIV8, VSDOF_ORG_IMG_WIDTH);
}

DPENode::BufferSizeConfig::
~BufferSizeConfig()
{}

DPENode::BufferPoolSet::
BufferPoolSet()
{}

DPENode::BufferPoolSet::
~BufferPoolSet()
{}

MBOOL
DPENode::BufferPoolSet::
init(const BufferSizeConfig& config)
{
    MY_LOGD("+");
    // allocate with the stride size as the width
    CREATE_IMGBUF_POOL(mDMPBuffPool, "DMPBufPool", config.VSDOF_DMP_SIZE,
                                        eImgFmt_Y16, ImageBufferPool::USAGE_HW, MTRUE);
    CREATE_IMGBUF_POOL(mCFMBuffPool, "CFMBufPool", config.VSDOF_CFM_SIZE,
                                        eImgFmt_Y8, ImageBufferPool::USAGE_HW, MTRUE);
    CREATE_IMGBUF_POOL(mRespBuffPool, "RESPOBufPool", config.VSDOF_RESPO_SIZE,
                                        eImgFmt_Y8, ImageBufferPool::USAGE_HW, MTRUE);
    //DMP, CFM, RESPO have all Left/Right side ->  double size
    ALLOCATE_BUFFER_POOL(mDMPBuffPool, VSDOF_WORKING_BUF_SET*2 + 2);
    ALLOCATE_BUFFER_POOL(mCFMBuffPool, VSDOF_WORKING_BUF_SET*2);
    ALLOCATE_BUFFER_POOL(mRespBuffPool, VSDOF_WORKING_BUF_SET*2);

    // need hold the previous frame's DMP_LR buffer, extra allocate two
    ALLOCATE_BUFFER_POOL(mDMPBuffPool, 2);
    MY_LOGD("-");
    return MTRUE;
}

MBOOL
DPENode::BufferPoolSet::
uninit()
{
    ImageBufferPool::destroy(mDMPBuffPool);
    ImageBufferPool::destroy(mCFMBuffPool);
    ImageBufferPool::destroy(mRespBuffPool);
    return MTRUE;
}



DPENode::
DPENode(const char* name, Graph_T *graph)
:DepthMapPipeNode(name, graph), mpDPEStream(NULL), mbFirst(MTRUE)
{
    this->addWaitQueue(&mJobQueue);

}

DPENode::
~DPENode()
{
}

MVOID
DPENode::
cleanUp()
{
    VSDOF_LOGD("+");
    //
    if(mpDPEStream != NULL)
    {
        mpDPEStream->uninit();
        mpDPEStream = NULL;
    }
    mJobQueue.clear();
    mbFirst = MTRUE;
    mLastDMP_L = NULL;
    mLastDMP_R = NULL;
    //
    mBufPoolSet.uninit();
    VSDOF_LOGD("-");
}

MBOOL
DPENode::
onInit()
{
    VSDOF_LOGD("+");

    // init DPEStream
    mpDPEStream = NSCam::NSIoPipe::NSDpe::IDpeStream::createInstance("VSDOF_DPE");
    if(mpDPEStream == NULL)
    {
        MY_LOGE("DPE Stream create instance failed!");
        return MFALSE;
    }
    mpDPEStream->init();
    // init buffer pool set
    mBufPoolSet.init(mBufConfig);
    // init the last DMP
    mLastDMP_L = mBufPoolSet.mDMPBuffPool->request();
    mLastDMP_R = mBufPoolSet.mDMPBuffPool->request();

    VSDOF_LOGD("-");
    return MTRUE;
}

MBOOL
DPENode::
onUninit()
{
    VSDOF_LOGD("+");
    cleanUp();
    VSDOF_LOGD("-");
    return MTRUE;
}

MBOOL
DPENode::
onThreadStart()
{
    return MTRUE;
}

MBOOL
DPENode::
onThreadStop()
{
    return MTRUE;
}


MBOOL
DPENode::
onData(DataID data, ImgInfoMapPtr& N3DImgInfo)
{
    MBOOL ret = MTRUE;
    VSDOF_PRFLOG("+ : reqId=%d", N3DImgInfo->getRequestPtr()->getRequestNo());

    switch(data)
    {
        case N3D_TO_DPE_MVSV_MASK:
            mJobQueue.enque(N3DImgInfo);
            break;
        default:
            MY_LOGW("Unrecongnized DataID=%d", data);
            ret = MFALSE;
            break;
    }

    VSDOF_LOGD("-");
    return ret;
}

MBOOL
DPENode::
onThreadLoop()
{
    ImgInfoMapPtr pN3DImgInfo;
    EffectRequestPtr reqPtr;

    if( !waitAllQueue() )
    {
        // mJobs.abort() called
        MY_LOGE("waitAllQueue failed");
        return MFALSE;
    }

    if( !mJobQueue.deque(pN3DImgInfo) )
    {
        MY_LOGE("mJobQueue.deque() failed");
        return MFALSE;
    }
    // mark on-going-request start
    this->incExtThreadDependency();

    reqPtr = pN3DImgInfo->getRequestPtr();
    MUINT32 iReqIdx = reqPtr->getRequestNo();

    VSDOF_PRFLOG("threadLoop start, reqID=%d", iReqIdx);

    DVEParams enqueParams;
    DVEConfig enqueConfig;

    EnquedBufPool *pEnqueBufPool = new EnquedBufPool(pN3DImgInfo, this);

    // prepare the enque configs
    prepareDPEEnqueConfig(pN3DImgInfo, enqueConfig, pEnqueBufPool);

    debugDPEConfig(enqueConfig);

    enqueParams.mDVEConfigVec.push_back(enqueConfig);
    enqueParams.mpfnCallback = onDPEEnqueDone;
    enqueParams.mpCookie = (void*) pEnqueBufPool;

    VSDOF_PRFLOG("DVE Enque start, reqID=%d", iReqIdx);
    MBOOL bRet = mpDPEStream->DVEenque(enqueParams);

    if(!bRet)
    {
        MY_LOGE("DPE enque failed!!");
        goto lbExit;
    }

    VSDOF_PRFLOG("threadLoop end! reqID=%d", iReqIdx);

    return MTRUE;

lbExit:
    // mark on-going-request end
    this->decExtThreadDependency();
    return MFALSE;
}

MVOID
DPENode::
debugDPEConfig(DVEConfig& config)
{
    if(!mbDebugLog)
        return;

    MY_LOGD("DVEConfig.Dve_Skp_Pre_Dv=%d", config.Dve_Skp_Pre_Dv);
    MY_LOGD("DVEConfig.Dve_Mask_En=%d", config.Dve_Mask_En);
    MY_LOGD("DVEConfig.Dve_l_Bbox_En=%d", config.Dve_l_Bbox_En);
    MY_LOGD("DVEConfig.Dve_r_Bbox_En=%d", config.Dve_r_Bbox_En);
    MY_LOGD("DVEConfig.Dve_Horz_Ds_Mode=%d", config.Dve_Horz_Ds_Mode);
    MY_LOGD("DVEConfig.Dve_Vert_Ds_Mode=%d", config.Dve_Vert_Ds_Mode);
    MY_LOGD("DVEConfig.Dve_Imgi_l_Fmt=%d", config.Dve_Imgi_l_Fmt);
    MY_LOGD("DVEConfig.Dve_Imgi_r_Fmt=%d", config.Dve_Imgi_r_Fmt);
    MY_LOGD("DVEConfig.Dve_Org_l_Bbox (left, right, top, down)=(%d, %d, %d, %d)",
                config.Dve_Org_l_Bbox.DVE_ORG_BBOX_LEFT, config.Dve_Org_l_Bbox.DVE_ORG_BBOX_RIGHT,
                config.Dve_Org_l_Bbox.DVE_ORG_BBOX_TOP, config.Dve_Org_l_Bbox.DVE_ORG_BBOX_BOTTOM);

    MY_LOGD("DVEConfig.Dve_Org_r_Bbox (left, right, top, down)=(%d, %d, %d, %d)",
                config.Dve_Org_r_Bbox.DVE_ORG_BBOX_LEFT, config.Dve_Org_r_Bbox.DVE_ORG_BBOX_RIGHT,
                config.Dve_Org_r_Bbox.DVE_ORG_BBOX_TOP, config.Dve_Org_r_Bbox.DVE_ORG_BBOX_BOTTOM);


    MY_LOGD("DVEConfig.Dve_Org_Width=%d", config.Dve_Org_Width);
    MY_LOGD("DVEConfig.Dve_Org_Height=%d", config.Dve_Org_Height);

    MY_LOGD("DVEConfig.Dve_Org_Horz_Sr_0=%d", config.Dve_Org_Horz_Sr_0);
    MY_LOGD("DVEConfig.Dve_Org_Horz_Sr_1=%d", config.Dve_Org_Horz_Sr_1);
    MY_LOGD("DVEConfig.Dve_Org_Vert_Sr_0=%d", config.Dve_Org_Vert_Sr_0);
    MY_LOGD("DVEConfig.Dve_Org_Start_Vert_Sv=%d", config.Dve_Org_Start_Vert_Sv);
    MY_LOGD("DVEConfig.Dve_Org_Start_Horz_Sv=%d", config.Dve_Org_Start_Horz_Sv);
    MY_LOGD("DVEConfig.Dve_Cand_Num=%d", config.Dve_Cand_Num);

    #define LOG_CANDIDATE(cand) \
        MY_LOGD("DVEConfig." #cand ".DVE_CAND_SEL = %d", config.cand.DVE_CAND_SEL); \
        MY_LOGD("DVEConfig." #cand ".DVE_CAND_TYPE = %d", config.cand.DVE_CAND_TYPE);

    LOG_CANDIDATE(Dve_Cand_0);
    LOG_CANDIDATE(Dve_Cand_1);
    LOG_CANDIDATE(Dve_Cand_2);
    LOG_CANDIDATE(Dve_Cand_3);
    LOG_CANDIDATE(Dve_Cand_4);
    LOG_CANDIDATE(Dve_Cand_5);
    LOG_CANDIDATE(Dve_Cand_6);
    LOG_CANDIDATE(Dve_Cand_7);

    MY_LOGD("DVEConfig.Dve_Rand_Lut_0=%d", config.Dve_Rand_Lut_0);
    MY_LOGD("DVEConfig.Dve_Rand_Lut_1=%d", config.Dve_Rand_Lut_1);
    MY_LOGD("DVEConfig.Dve_Rand_Lut_2=%d", config.Dve_Rand_Lut_2);
    MY_LOGD("DVEConfig.Dve_Rand_Lut_3=%d", config.Dve_Rand_Lut_3);
    MY_LOGD("DVEConfig.DVE_VERT_GMV=%d", config.DVE_VERT_GMV);
    MY_LOGD("DVEConfig.DVE_HORZ_GMV=%d", config.DVE_HORZ_GMV);
    MY_LOGD("DVEConfig.Dve_Horz_Dv_Ini=%d", config.Dve_Horz_Dv_Ini);
    MY_LOGD("DVEConfig.Dve_Coft_Shift=%d", config.Dve_Coft_Shift);
    MY_LOGD("DVEConfig.Dve_Corner_Th=%d", config.Dve_Corner_Th);
    MY_LOGD("DVEConfig.Dve_Smth_Luma_Th_1=%d", config.Dve_Smth_Luma_Th_1);
    MY_LOGD("DVEConfig.Dve_Smth_Luma_Th_0=%d", config.Dve_Smth_Luma_Th_0);
    MY_LOGD("DVEConfig.Dve_Smth_Luma_Ada_Base=%d", config.Dve_Smth_Luma_Ada_Base);
    MY_LOGD("DVEConfig.Dve_Smth_Luma_Horz_Pnlty_Sel=%d", config.Dve_Smth_Luma_Horz_Pnlty_Sel);
    MY_LOGD("DVEConfig.Dve_Smth_Dv_Mode=%d", config.Dve_Smth_Dv_Mode);
    MY_LOGD("DVEConfig.Dve_Smth_Dv_Th_1=%d", config.Dve_Smth_Dv_Th_1);
    MY_LOGD("DVEConfig.Dve_Smth_Dv_Th_0=%d", config.Dve_Smth_Dv_Th_0);
    MY_LOGD("DVEConfig.Dve_Smth_Dv_Ada_Base=%d", config.Dve_Smth_Dv_Ada_Base);
    MY_LOGD("DVEConfig.Dve_Smth_Dv_Vert_Pnlty_Sel=%d", config.Dve_Smth_Dv_Vert_Pnlty_Sel);
    MY_LOGD("DVEConfig.Dve_Smth_Dv_Horz_Pnlty_Sel=%d", config.Dve_Smth_Dv_Horz_Pnlty_Sel);
    MY_LOGD("DVEConfig.Dve_Ord_Pnlty_Sel=%d", config.Dve_Ord_Pnlty_Sel);
    MY_LOGD("DVEConfig.Dve_Ord_Coring=%d", config.Dve_Ord_Coring);
    MY_LOGD("DVEConfig.Dve_Ord_Th=%d", config.Dve_Ord_Th);
    MY_LOGD("DVEConfig.Dve_Type_Penality_Ctrl.DVE_REFINE_PNLTY_SEL=%d",
                config.Dve_Type_Penality_Ctrl.DVE_REFINE_PNLTY_SEL);
    MY_LOGD("DVEConfig.Dve_Type_Penality_Ctrl.DVE_GMV_PNLTY_SEL=%d",
                config.Dve_Type_Penality_Ctrl.DVE_GMV_PNLTY_SEL);
    MY_LOGD("DVEConfig.Dve_Type_Penality_Ctrl.DVE_PREV_PNLTY_SEL=%d",
                config.Dve_Type_Penality_Ctrl.DVE_PREV_PNLTY_SEL);
    MY_LOGD("DVEConfig.Dve_Type_Penality_Ctrl.DVE_NBR_PNLTY_SEL=%d",
                config.Dve_Type_Penality_Ctrl.DVE_NBR_PNLTY_SEL);
    MY_LOGD("DVEConfig.Dve_Type_Penality_Ctrl.DVE_RAND_PNLTY_SEL=%d",
                config.Dve_Type_Penality_Ctrl.DVE_RAND_PNLTY_SEL);
    MY_LOGD("DVEConfig.Dve_Type_Penality_Ctrl.DVE_TMPR_PNLTY_SEL=%d",
                config.Dve_Type_Penality_Ctrl.DVE_TMPR_PNLTY_SEL);
    MY_LOGD("DVEConfig.Dve_Type_Penality_Ctrl.DVE_SPTL_PNLTY_SEL=%d",
                config.Dve_Type_Penality_Ctrl.DVE_SPTL_PNLTY_SEL);
    MY_LOGD("DVEConfig.Dve_Type_Penality_Ctrl.DVE_RAND_COST=%d",
                config.Dve_Type_Penality_Ctrl.DVE_RAND_COST);
    MY_LOGD("DVEConfig.Dve_Type_Penality_Ctrl.DVE_GMV_COST=%d",
                config.Dve_Type_Penality_Ctrl.DVE_GMV_COST);
    MY_LOGD("DVEConfig.Dve_Type_Penality_Ctrl.DVE_PREV_COST=%d",
                config.Dve_Type_Penality_Ctrl.DVE_PREV_COST);
    MY_LOGD("DVEConfig.Dve_Type_Penality_Ctrl.DVE_NBR_COST=%d",
                config.Dve_Type_Penality_Ctrl.DVE_NBR_COST);
    MY_LOGD("DVEConfig.Dve_Type_Penality_Ctrl.DVE_REFINE_COST=%d",
                config.Dve_Type_Penality_Ctrl.DVE_REFINE_COST);
    MY_LOGD("DVEConfig.Dve_Type_Penality_Ctrl.DVE_TMPR_COST=%d",
                config.Dve_Type_Penality_Ctrl.DVE_TMPR_COST);
    MY_LOGD("DVEConfig.Dve_Type_Penality_Ctrl.DVE_SPTL_COST=%d",
                config.Dve_Type_Penality_Ctrl.DVE_SPTL_COST);


    #define LOG_DPEBuf(bufInfo) \
        MY_LOGD("DVEConfig." #bufInfo ".dmaport=%d", config.bufInfo.dmaport);\
        MY_LOGD("DVEConfig." #bufInfo ".memID=%d", config.bufInfo.memID); \
        MY_LOGD("DVEConfig." #bufInfo ".u4BufVA=%x", config.bufInfo.u4BufVA); \
        MY_LOGD("DVEConfig." #bufInfo ".u4BufPA=%x", config.bufInfo.u4BufPA); \
        MY_LOGD("DVEConfig." #bufInfo ".u4BufSize=%d", config.bufInfo.u4BufSize); \
        MY_LOGD("DVEConfig." #bufInfo ".u4Stride=%d", config.bufInfo.u4Stride);

    LOG_DPEBuf(Dve_Imgi_l);
    LOG_DPEBuf(Dve_Imgi_r);
    LOG_DPEBuf(Dve_Dvi_l);
    LOG_DPEBuf(Dve_Dvi_r);
    LOG_DPEBuf(Dve_Maski_l);
    LOG_DPEBuf(Dve_Maski_r);
    LOG_DPEBuf(Dve_Dvo_l);
    LOG_DPEBuf(Dve_Dvo_r);
    LOG_DPEBuf(Dve_Confo_l);
    LOG_DPEBuf(Dve_Confo_r);
    LOG_DPEBuf(Dve_Respo_l);
    LOG_DPEBuf(Dve_Respo_r);

    MY_LOGD("DVEConfig.Dve_Vert_Sv=%d", config.Dve_Vert_Sv);
    MY_LOGD("DVEConfig.Dve_Horz_Sv=%d", config.Dve_Horz_Sv);
}

MVOID
DPENode::
onDPEEnqueDone(DVEParams& rParams)
{
    EnquedBufPool* pEnquePool = reinterpret_cast<EnquedBufPool*>(rParams.mpCookie);
    DPENode* pDPENode = pEnquePool->mpNode;
    pDPENode->handleDPEEnqueDone(rParams, pEnquePool);
}

MVOID
DPENode::
handleDPEEnqueDone(DVEParams& rParams, EnquedBufPool* pEnqueBufPool)
{
    ImgInfoMapPtr pN3DImgInfo = pEnqueBufPool->mpImgBufInfo;

    VSDOF_PRFLOG("+, reqID=%d", pN3DImgInfo->getRequestPtr()->getRequestNo());

    SmartImageBuffer smImgBuf_DMP_L, smImgBuf_DMP_R, smImgBuf_CFM_L, smImgBuf_CFM_R;
    SmartImageBuffer smImgBuf_RESPO_L, smImgBuf_RESPO_R;
    MBOOL bRet;

    #define GET_SMBUFFER_FROM_ENQPOOL_AND_INVALIDATE(pEnqueBufPool, BID, rSmImgBuf) \
        pEnqueBufPool->getBuffData(BID, rSmImgBuf); \
        rSmImgBuf->mImageBuffer->syncCache(eCACHECTRL_INVALID);

    // get the output buffers and invalid
    GET_SMBUFFER_FROM_ENQPOOL_AND_INVALIDATE(pEnqueBufPool, BID_TO_OCC_DMP_L, smImgBuf_DMP_L);
    GET_SMBUFFER_FROM_ENQPOOL_AND_INVALIDATE(pEnqueBufPool, BID_TO_OCC_DMP_R, smImgBuf_DMP_R);GET_SMBUFFER_FROM_ENQPOOL_AND_INVALIDATE(pEnqueBufPool, BID_TO_OCC_CFM_L, smImgBuf_CFM_L);
    GET_SMBUFFER_FROM_ENQPOOL_AND_INVALIDATE(pEnqueBufPool, BID_TO_OCC_CFM_R, smImgBuf_CFM_R);
    GET_SMBUFFER_FROM_ENQPOOL_AND_INVALIDATE(pEnqueBufPool, BID_TO_OCC_RESPO_L, smImgBuf_RESPO_L);
    GET_SMBUFFER_FROM_ENQPOOL_AND_INVALIDATE(pEnqueBufPool, BID_TO_OCC_RESPO_R, smImgBuf_RESPO_R);

    #undef GET_SMBUFFER_FROM_ENQPOOL_AND_INVALIDATE

    // update the last DMP_L/R
    mLastDMP_L = smImgBuf_DMP_L;
    mLastDMP_R = smImgBuf_DMP_R;

    // prepare input for OCC
    sp<ImageBufInfoMap> TO_OCC_ImgBufInfo = new ImageBufInfoMap(pN3DImgInfo->getRequestPtr());
    // DMP /CFM
    TO_OCC_ImgBufInfo->addImageBuffer(BID_TO_OCC_DMP_L, smImgBuf_DMP_L);
    TO_OCC_ImgBufInfo->addImageBuffer(BID_TO_OCC_DMP_R, smImgBuf_DMP_R);
    TO_OCC_ImgBufInfo->addImageBuffer(BID_TO_OCC_CFM_L, smImgBuf_CFM_L);
    TO_OCC_ImgBufInfo->addImageBuffer(BID_TO_OCC_CFM_R, smImgBuf_CFM_R);
    // forward N3D output: MV_Y/SV_Y to OCC
    TO_OCC_ImgBufInfo->addImageBuffer(BID_TO_DPE_OCC_MV_Y, pN3DImgInfo->getImageBuffer(BID_TO_DPE_OCC_MV_Y));
    TO_OCC_ImgBufInfo->addImageBuffer(BID_TO_DPE_OCC_SV_Y, pN3DImgInfo->getImageBuffer(BID_TO_DPE_OCC_SV_Y));
    // post to OCC node
    handleDataAndDump(DPE_TO_OCC_MVSV_DMP_CFM, TO_OCC_ImgBufInfo);

    delete pEnqueBufPool;
    VSDOF_PRFLOG("-, reqID=%d", pN3DImgInfo->getRequestPtr()->getRequestNo());

    // mark on-going-request end
    this->decExtThreadDependency();
}

MBOOL
DPENode::
setupDPEBufInfo(DPEDMAPort dmaPort, SmartImageBuffer &pSmImgBuf, DPEBufInfo& rBufInfo)
{
    android::sp<IImageBuffer> pImgBuf = pSmImgBuf->mImageBuffer;
    // plane 0 address
    rBufInfo.memID = pImgBuf->getFD(0);
    rBufInfo.dmaport = dmaPort;
    rBufInfo.u4BufVA = pImgBuf->getBufVA(0);
    rBufInfo.u4BufPA = pImgBuf->getBufPA(0);
    rBufInfo.u4BufSize = pImgBuf->getBufSizeInBytes(0);
    rBufInfo.u4Stride = pImgBuf->getBufStridesInBytes(0);
    return MTRUE;
}

MBOOL
DPENode::
setupDPEBufInfo(DPEDMAPort dmaPort, SmartImageBuffer &pSmImgBuf, DPEBufInfo& rBufInfo, EnquedBufPool *pEnqueBufPool, DepthMapBufferID bufID, MSize szActiveArea)
{
    setupDPEBufInfo(dmaPort, pSmImgBuf, rBufInfo);
    // set active area
    android::sp<IImageBuffer> pImgBuf = pSmImgBuf->mImageBuffer;
    pImgBuf->setExtParam(szActiveArea);

    if(pEnqueBufPool != NULL)
    {
        pEnqueBufPool->addBuffData(bufID, pSmImgBuf);
    }
    return MTRUE;
}

MBOOL
DPENode::
prepareDPEEnqueConfig(ImgInfoMapPtr pN3DImgInfo, DVEConfig& rDPEConfig, EnquedBufPool *pEnqueBufPool)
{
    VSDOF_LOGD("+");
    // read the tuning data from tuning provider
    StereoTuningProvider::getDPETuningInfo(&rDPEConfig);

    // insert the dynamic data
    SmartImageBuffer smImgBuf_MV_Y = pN3DImgInfo->getImageBuffer(BID_TO_DPE_OCC_MV_Y);
    SmartImageBuffer smImgBuf_SV_Y = pN3DImgInfo->getImageBuffer(BID_TO_DPE_OCC_SV_Y);
    SmartImageBuffer smImgBuf_MASK_M = pN3DImgInfo->getImageBuffer(BID_TO_DPE_MASK_M);
    SmartImageBuffer smImgBuf_MASK_S  = pN3DImgInfo->getImageBuffer(BID_TO_DPE_MASK_S);

    // imgi format: YV12 -> DPE_IMGI_Y_FMT
    rDPEConfig.Dve_Imgi_l_Fmt = DPE_IMGI_Y_FMT;
    rDPEConfig.Dve_Imgi_r_Fmt = DPE_IMGI_Y_FMT;

    rDPEConfig.Dve_Org_Width = mBufConfig.VSDOF_ORG_IMG_WIDTH;
    rDPEConfig.Dve_Org_Height = mBufConfig.VSDOF_ORG_IMG_HEIGHT;

    // mask flag: use the tuning provider firstly.
    rDPEConfig.Dve_Mask_En &= true;

    // flush input image buffers
    smImgBuf_SV_Y->mImageBuffer->syncCache(eCACHECTRL_FLUSH);
    smImgBuf_MV_Y->mImageBuffer->syncCache(eCACHECTRL_FLUSH);
    smImgBuf_MASK_M->mImageBuffer->syncCache(eCACHECTRL_FLUSH);
    smImgBuf_MASK_S->mImageBuffer->syncCache(eCACHECTRL_FLUSH);
    // setup input enque buffers
    setupDPEBufInfo(DMA_DVE_IMGI, smImgBuf_SV_Y, rDPEConfig.Dve_Imgi_l);
    setupDPEBufInfo(DMA_DVE_IMGI, smImgBuf_MV_Y, rDPEConfig.Dve_Imgi_r);
    setupDPEBufInfo(DMA_DVE_MASKI, smImgBuf_MASK_S, rDPEConfig.Dve_Maski_l);
    setupDPEBufInfo(DMA_DVE_MASKI, smImgBuf_MASK_M, rDPEConfig.Dve_Maski_r);

    // check the first enque or not
    if(mbFirst)
    {
        mbFirst = MFALSE;
        rDPEConfig.Dve_Skp_Pre_Dv = true;
    }
    else
    {
        rDPEConfig.Dve_Skp_Pre_Dv = false;
    }
    // flush and setup DVE buffer
    mLastDMP_L->mImageBuffer->syncCache(eCACHECTRL_FLUSH);
    mLastDMP_R->mImageBuffer->syncCache(eCACHECTRL_FLUSH);
    setupDPEBufInfo(DMA_DVE_DVI, mLastDMP_L, rDPEConfig.Dve_Dvi_l);
    setupDPEBufInfo(DMA_DVE_DVI, mLastDMP_R, rDPEConfig.Dve_Dvi_r);


    // output buffers
    SmartImageBuffer smImgBuf_DMP_L =  mBufPoolSet.mDMPBuffPool->request();
    SmartImageBuffer smImgBuf_DMP_R =  mBufPoolSet.mDMPBuffPool->request();
    SmartImageBuffer smImgBuf_CFM_L =  mBufPoolSet.mCFMBuffPool->request();
    SmartImageBuffer smImgBuf_CFM_R =  mBufPoolSet.mCFMBuffPool->request();
    SmartImageBuffer smImgBuf_RESPO_L =  mBufPoolSet.mRespBuffPool->request();
    SmartImageBuffer smImgBuf_RESPO_R =  mBufPoolSet.mRespBuffPool->request();

    // setup output buffers and store to pEnqueBufPool
    setupDPEBufInfo(DMA_DVE_DVO, smImgBuf_DMP_L, rDPEConfig.Dve_Dvo_l, pEnqueBufPool,
                        BID_TO_OCC_DMP_L, mBufConfig.VSDOF_DMP_SIZE);
    setupDPEBufInfo(DMA_DVE_DVO, smImgBuf_DMP_R, rDPEConfig.Dve_Dvo_r, pEnqueBufPool,
                        BID_TO_OCC_DMP_R, mBufConfig.VSDOF_DMP_SIZE);
    setupDPEBufInfo(DMA_DVE_CONFO, smImgBuf_CFM_L, rDPEConfig.Dve_Confo_l, pEnqueBufPool,
                        BID_TO_OCC_CFM_L, mBufConfig.VSDOF_CFM_SIZE);
    setupDPEBufInfo(DMA_DVE_CONFO, smImgBuf_CFM_R, rDPEConfig.Dve_Confo_r, pEnqueBufPool,
                        BID_TO_OCC_CFM_R, mBufConfig.VSDOF_CFM_SIZE);
    setupDPEBufInfo(DMA_DVE_RESPO, smImgBuf_RESPO_L, rDPEConfig.Dve_Respo_l, pEnqueBufPool,
                        BID_TO_OCC_RESPO_L, mBufConfig.VSDOF_RESPO_SIZE);
    setupDPEBufInfo(DMA_DVE_RESPO, smImgBuf_RESPO_R, rDPEConfig.Dve_Respo_r, pEnqueBufPool,
                        BID_TO_OCC_RESPO_R, mBufConfig.VSDOF_RESPO_SIZE);


    rDPEConfig.Dve_Mask_En=1;
    rDPEConfig.Dve_Org_Horz_Sr_0 = 255;
    rDPEConfig.Dve_Org_Horz_Sr_1 = 128;


    VSDOF_LOGD("-");
    return MTRUE;
}

const char*
DPENode::
onDumpBIDToName(DepthMapBufferID BID)
{
#define MAKE_NAME_CASE(name) \
    case name: return #name;
    switch(BID)
    {
        MAKE_NAME_CASE(BID_TO_OCC_DMP_L);
        MAKE_NAME_CASE(BID_TO_OCC_DMP_R);
        MAKE_NAME_CASE(BID_TO_OCC_CFM_L);
        MAKE_NAME_CASE(BID_TO_OCC_CFM_R);
        MAKE_NAME_CASE(BID_TO_OCC_RESPO_L);
        MAKE_NAME_CASE(BID_TO_OCC_RESPO_R);
        MAKE_NAME_CASE(BID_TO_DPE_OCC_MV_Y);
        MAKE_NAME_CASE(BID_TO_DPE_OCC_SV_Y);

    }
    MY_LOGW("unknown BID:%d", BID);

    return "unknown";
#undef MAKE_NAME_CASE
}

}; //NSFeaturePipe
}; //NSCamFeature
}; //NSCam
