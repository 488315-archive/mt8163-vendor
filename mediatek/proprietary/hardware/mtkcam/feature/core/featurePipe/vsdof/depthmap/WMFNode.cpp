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

#include "WMFNode.h"
#include "DepthMapPipe_Common.h"

#include <iopipe/PostProc/DpeUtility.h>
#include <vsdof/hal/stereo_tuning_provider.h>

#include <drv/imem_drv.h>

#define PIPE_CLASS_TAG "WMFNode"
#include <featurePipe/core/include/PipeLog.h>

using namespace NSCam::NSIoPipe::NSDpe;

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {

WMFNode::BufferPoolSet::
BufferPoolSet()
{}

WMFNode::BufferPoolSet::
~BufferPoolSet()
{}


MBOOL
WMFNode::BufferPoolSet::
init()
{
    MY_LOGD("+");
    // create the TBLI buffer pool
    StereoSizeProvider * pSizeProvder = StereoSizeProvider::getInstance();
    MUINT32 iWMFInputWidth = pSizeProvder->getBufferSize(E_MY_S, eSTEREO_SCENARIO_RECORD).size.w;

    MUINT32 iTbliStride = DPEQueryInDMAStride(DMA_WMFE_TBLI, WMFE_DPI_D_FMT, iWMFInputWidth);

    CREATE_IMGBUF_POOL( mpTbliImgBufPool, "TbliBufPool", MSize(iTbliStride, 1),
                        eImgFmt_Y8, ImageBufferPool::USAGE_HW, MTRUE);
    ALLOCATE_BUFFER_POOL(mpTbliImgBufPool, 1);


    MSize szDMWSize = pSizeProvder->getBufferSize(E_DMW, eSTEREO_SCENARIO_RECORD).size;
    CREATE_IMGBUF_POOL(mpInternalDMWImgBufPool, "InternalDMWBufPool", szDMWSize,
                        eImgFmt_Y8, ImageBufferPool::USAGE_HW, MTRUE);
    ALLOCATE_BUFFER_POOL(mpInternalDMWImgBufPool, VSDOF_WORKING_BUF_SET);
    MY_LOGD("-");
    return MTRUE;
}

MBOOL
WMFNode::BufferPoolSet::
uninit()
{
    ImageBufferPool::destroy(mpTbliImgBufPool);
    return MTRUE;
}


WMFNode::
WMFNode(const char* name, Graph_T *graph)
: DepthMapPipeNode(name, graph), mpDPEStream(NULL)
{
    this->addWaitQueue(&mOCCInfoQueue);
    this->addWaitQueue(&mWMFSecQueue);
}

WMFNode::
~WMFNode()
{
}

MVOID
WMFNode::
cleanUp()
{
    VSDOF_LOGD("+");
    if(mpDPEStream != NULL)
    {
        mpDPEStream->uninit();
        mpDPEStream = NULL;
    }
    // JOB queue clear
    mOCCInfoQueue.clear();
    mWMFSecQueue.clear();
    //
    mMYSLSImageBufMap.clear();
    mpTbliImgBuf = NULL;
    // buf pool uninit
    mBufPoolSet.uninit();
    VSDOF_LOGD("-");
}

MBOOL
WMFNode::
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
    MBOOL bRet = mpDPEStream->init();
    if(!bRet)
    {
        MY_LOGE("DPEStream init failed!!");
        return MFALSE;
    }

    // init the buffer pool set
    mBufPoolSet.init();
    // get Tbli tuning buffer
    mpTbliImgBuf = mBufPoolSet.mpTbliImgBufPool->request();
    // query tuning mgr to get Tbli
    StereoTuningProvider::getWMFTuningInfo(eWMFFilterSize, reinterpret_cast<void*>(mpTbliImgBuf->mImageBuffer->getBufVA(0)));
    mpTbliImgBuf->mImageBuffer->syncCache(eCACHECTRL_FLUSH);
    VSDOF_LOGD("-");
    return MTRUE;
}

MBOOL
WMFNode::
onUninit()
{
    VSDOF_LOGD("+");
    cleanUp();
    VSDOF_LOGD("-");
    return MTRUE;
}

MBOOL
WMFNode::
onThreadStart()
{
    return MTRUE;
}

MBOOL
WMFNode::
onThreadStop()
{
    return MTRUE;
}

MBOOL
WMFNode::
onData(DataID data, ImgInfoMapPtr& pSmImgInfo)
{
    MBOOL ret=MTRUE;
    VSDOF_PRFLOG("+, dataID=%d reqId=%d", data, pSmImgInfo->getRequestPtr()->getRequestNo());

    switch(data)
    {
        case OCC_TO_WMF_DMH_MY_S:
            mOCCInfoQueue.enque(pSmImgInfo);
            break;
        case P2AFM_TO_WMF_MY_SL:
        {
            android::Mutex::Autolock lock(mMYSLMutex);
            // add MY_SL to container
            SmartImageBuffer pMYSLImageBuf = pSmImgInfo->getImageBuffer(BID_P2AFM_FE1CO_INPUT);
            MUINT32 reqID = pSmImgInfo->getRequestPtr()->getRequestNo();
            mMYSLSImageBufMap.add(reqID, pMYSLImageBuf);
            break;
        }
        default:
            ret = MFALSE;
            break;
    }

    VSDOF_LOGD("-");
    return ret;
}

MBOOL
WMFNode::
onThreadLoop()
{
    ImgInfoMapPtr pJobImgInfo;
    EffectRequestPtr reqPtr;

    if( !waitAnyQueue() )
    {
        // mJobs.abort() called
        MY_LOGE("waitAllQueue failed");
        return MFALSE;
    }

    bool bExist = false;

    // check the second pass WMF queue first
    bExist = mWMFSecQueue.deque(pJobImgInfo);

    if(bExist)
    {
        return threadLoop_WMFSecPass(pJobImgInfo);
    }
    else
    {
        if( !mOCCInfoQueue.deque(pJobImgInfo) )
        {
            MY_LOGD("mOCCInfoQueue.deque() failed");
            return MFALSE;
        }
        return threadLoop_WMFFirstPass(pJobImgInfo);
    }

}

MVOID
WMFNode::
debugWMFParam(WMFEConfig &config)
{
    if(!mbDebugLog)
        return;

    #define LOG_BUFFER_INFO(buff_info)\
        MY_LOGD(#buff_info ".dmaport=%d", buff_info.dmaport);\
        MY_LOGD(#buff_info ".memID=%d", buff_info.memID);\
        MY_LOGD(#buff_info ".u4BufVA=%x", buff_info.u4BufVA);\
        MY_LOGD(#buff_info ".u4BufPA=%x", buff_info.u4BufPA);\
        MY_LOGD(#buff_info ".u4BufSize=%d", buff_info.u4BufSize);\
        MY_LOGD(#buff_info ".u4Stride=%d", buff_info.u4Stride);


    WMFECtrl ctrl = config.Wmfe_Ctrl_0;
    MY_LOGD("========== Wmfe_Ctrl_0 section==============");
    MY_LOGD("Wmfe_Enable=%d", ctrl.Wmfe_Enable);
    MY_LOGD("WmfeFilterSize=%d", ctrl.WmfeFilterSize);
    MY_LOGD("Wmfe_Width=%d", ctrl.Wmfe_Width);
    MY_LOGD("Wmfe_Height=%d", ctrl.Wmfe_Height);

    LOG_BUFFER_INFO(ctrl.Wmfe_Imgi);
    LOG_BUFFER_INFO(ctrl.Wmfe_Dpi);
    LOG_BUFFER_INFO(ctrl.Wmfe_Tbli);
    LOG_BUFFER_INFO(ctrl.Wmfe_Dpo);

    MY_LOGD("Tbli: ");
    short *addr = (short*) ctrl.Wmfe_Tbli.u4BufVA;
    int index = 0;
    while(index < 128)
    {
      MY_LOGD("%d \t%d \t%d \t%d \t%d \t%d \t%d \t%d \n",
                 addr[index], addr[index+1], addr[index+2], addr[index+3],
                 addr[index+4], addr[index+5], addr[index+6], addr[index+7]);
      index +=8;
    }

    ctrl = config.Wmfe_Ctrl_1;
    MY_LOGD("========== Wmfe_Ctrl_1 section==============");
    MY_LOGD("Wmfe_Enable=%d", ctrl.Wmfe_Enable);
    MY_LOGD("WmfeFilterSize=%d", ctrl.WmfeFilterSize);
    MY_LOGD("Wmfe_Width=%d", ctrl.Wmfe_Width);
    MY_LOGD("Wmfe_Height=%d", ctrl.Wmfe_Height);

    LOG_BUFFER_INFO(ctrl.Wmfe_Imgi);
    LOG_BUFFER_INFO(ctrl.Wmfe_Dpi);
    LOG_BUFFER_INFO(ctrl.Wmfe_Tbli);
    LOG_BUFFER_INFO(ctrl.Wmfe_Dpo);

    MY_LOGD("Tbli: ");
    addr = (short*) ctrl.Wmfe_Tbli.u4BufVA;
    index = 0;
    while(index < 128)
    {
      MY_LOGD("%d \t%d \t%d \t%d \t%d \t%d \t%d \t%d \n",
                 addr[index], addr[index+1], addr[index+2], addr[index+3],
                 addr[index+4], addr[index+5], addr[index+6], addr[index+7]);
      index +=8;
    }


}

MBOOL
WMFNode::
threadLoop_WMFFirstPass(ImgInfoMapPtr& rpOCCImgInfo)
{
    // mark on-going-request start
    this->incExtThreadDependency();

    EffectRequestPtr reqPtr = rpOCCImgInfo->getRequestPtr();

    VSDOF_PRFLOG("threadLoop start: WMF First-Pass, reqID=%d", reqPtr->getRequestNo());

    EnquedBufPool *pEnqueBufPool = new EnquedBufPool(rpOCCImgInfo, this);

    FrameInfoPtr framePtr_MY_S, framePtr_DMW;
    sp<IImageBuffer> pImgBuf_MYS, pImgBuf_DMW;
    // input buffer MY_S
    RETRIEVE_OFRAMEINFO_IMGBUF_ERROR_RETURN(reqPtr, framePtr_MY_S, BID_OUT_MY_S, pImgBuf_MYS);
    // input buffer DMH
    SmartImageBuffer smBuf_DMH = rpOCCImgInfo->getImageBuffer(BID_TO_WMF_DMH);
    // output buffer DMW
    RETRIEVE_OFRAMEINFO_IMGBUF_ERROR_RETURN(reqPtr, framePtr_DMW, BID_OUT_DMW, pImgBuf_DMW);

    // prepare the enque params
    WMFEParams enqueParams;
    WMFEConfig wmfConfig;

    prepareWMFEnqueConfig(pImgBuf_MYS, smBuf_DMH->mImageBuffer, pImgBuf_DMW, wmfConfig, pEnqueBufPool);
    enqueParams.mpfnCallback = onWMFEnqueDone_FirstPass;
    enqueParams.mWMFEConfigVec.push_back(wmfConfig);
    enqueParams.mpCookie = (void*) pEnqueBufPool;

    debugWMFParam(wmfConfig);

    VSDOF_PRFLOG("WMFE enque start - first pass, reqID=%d", reqPtr->getRequestNo());
    MBOOL bRet = mpDPEStream->WMFEenque(enqueParams);

    if(!bRet)
    {
        MY_LOGE("WMF enque failed!!");
        goto lbExit;
    }

    VSDOF_PRFLOG("threadLoop end: WMF First-Pass, reqID=%d", reqPtr->getRequestNo());

    return MTRUE;

lbExit:
    // mark on-going-request end
    this->decExtThreadDependency();
    return MFALSE;
}

MVOID
WMFNode::
onWMFEnqueDone_FirstPass(WMFEParams& rParams)
{
    EnquedBufPool* pEnquePool = reinterpret_cast<EnquedBufPool*>(rParams.mpCookie);
    WMFNode* pWMFNode = pEnquePool->mpNode;
    pWMFNode->handleWMFEnqueDone_FirstPass(rParams, pEnquePool);
}

MVOID
WMFNode::
handleWMFEnqueDone_FirstPass(WMFEParams& rParams, EnquedBufPool* pEnqueBufPool)
{
    ImgInfoMapPtr pOCCImgInfo = pEnqueBufPool->mpImgBufInfo;
    EffectRequestPtr pEffectReq = pOCCImgInfo->getRequestPtr();
    MUINT32 iReqNo =  pEffectReq->getRequestNo();

    VSDOF_PRFLOG("+, reqID=%d", iReqNo);
    // mark MY_S/DMW frameReady and return
    FrameInfoPtr framePtr_MY_S, framePtr_DMW;
    RETRIEVE_OFRAMEINFO(pEffectReq, framePtr_MY_S, BID_OUT_MY_S);
    RETRIEVE_OFRAMEINFO(pEffectReq, framePtr_DMW, BID_OUT_DMW);
    if(framePtr_MY_S.get())
        framePtr_MY_S->setFrameReady(true);
    if(framePtr_DMW.get())
        framePtr_DMW->setFrameReady(true);

    // dump data first
    handleDump(WMF_OUT_DMW_MY_S, framePtr_DMW, "WMF_OUT_DMW");
    handleDump(WMF_OUT_DMW_MY_S, framePtr_MY_S, "WMF_OUT_MY_S");
    // handle data next
    handleData(WMF_OUT_DMW_MY_S, framePtr_DMW);


    //
    SmartImageBuffer pSmImgInfo_MYSL;
    sp<ImageBufInfoMap> ToWMF_SecPass_ImgBufInfo;

    // get the depthmap frame info
    FrameInfoPtr framePtr_DEPTHMAP;
    RETRIEVE_OFRAMEINFO(pEffectReq, framePtr_DEPTHMAP, BID_OUT_DEPTHMAP);

    if (!framePtr_DEPTHMAP.get())
    {
        if(getRequestState(pEffectReq) == STATE_CAPTURE)
            MY_LOGW("Cannot find DEPTHMAP frameInfo when capture scenarios!");
        else
            VSDOF_LOGD("No need to generate DepthMap.");
        goto lbExit;
    }


    // perform second run for DepthMap
    {
        android::Mutex::Autolock lock(mMYSLMutex);
        size_t indexOfMYSL = mMYSLSImageBufMap.indexOfKey(pEffectReq->getRequestNo());
        if( indexOfMYSL >= 0)
        {
            pSmImgInfo_MYSL = mMYSLSImageBufMap[indexOfMYSL];
            mMYSLSImageBufMap.removeItemsAt(indexOfMYSL);
        }
        else
        {
            MY_LOGE("Not exist the MY_SL buffer to generate DepthMap, reqID=%d", iReqNo);
            goto lbExit;
        }
    }

    // prepare the second pass ImgBufInfo
    ToWMF_SecPass_ImgBufInfo = new ImageBufInfoMap(pEffectReq);
    ToWMF_SecPass_ImgBufInfo->addImageBuffer(BID_TO_WMF_MY_SL, pSmImgInfo_MYSL);

    // enque second pass WaitQueue
    mWMFSecQueue.enque(ToWMF_SecPass_ImgBufInfo);

lbExit:
    delete pEnqueBufPool;
    VSDOF_PRFLOG("-, reqID=%d", iReqNo);
    // mark on-going-request end
    this->decExtThreadDependency();
}

MBOOL
WMFNode::
threadLoop_WMFSecPass(ImgInfoMapPtr& rpWMF_ImgInfo)
{
    // mark on-going-request start
    this->incExtThreadDependency();

    EffectRequestPtr reqPtr = rpWMF_ImgInfo->getRequestPtr();
    VSDOF_PRFLOG("threadLoop start: WMF Sec-Pass, reqID=%d", reqPtr->getRequestNo());

    EnquedBufPool *pEnqueBufPool = new EnquedBufPool(rpWMF_ImgInfo, this);

    FrameInfoPtr framePtr_DepthMap, framePtr_DMW;
    sp<IImageBuffer> pImgBuf_DepthMap, pImgBuf_DMW;

    // input buffer MY_SL
    SmartImageBuffer smBuf_MYSL = rpWMF_ImgInfo->getImageBuffer(BID_TO_WMF_MY_SL);
    // input buffer DMW
    RETRIEVE_OFRAMEINFO_IMGBUF(reqPtr, framePtr_DMW, BID_OUT_DMW, pImgBuf_DMW);
    // output buffer DepthMap
    RETRIEVE_OFRAMEINFO_IMGBUF_WITH_WARNING(reqPtr, framePtr_DepthMap, BID_OUT_DEPTHMAP, pImgBuf_DepthMap);

    if(!framePtr_DepthMap.get())
    {
        MY_LOGE("Failed to run WMF second pass, does not have necessary FrameInfo.");
        return MFALSE;
    }

    // prepare the enque params
    WMFEParams enqueParams;
    WMFEConfig wmfConfig;

    prepareWMFEnqueConfig(smBuf_MYSL->mImageBuffer, pImgBuf_DMW, pImgBuf_DepthMap, wmfConfig, pEnqueBufPool);
    enqueParams.mpfnCallback = onWMFEnqueDone_SecPass;
    enqueParams.mWMFEConfigVec.push_back(wmfConfig);
    enqueParams.mpCookie = (void*) pEnqueBufPool;

    VSDOF_PRFLOG("WMFE enque start - second pass, reqID=%d", reqPtr->getRequestNo());
    MBOOL bRet = mpDPEStream->WMFEenque(enqueParams);

    if(!bRet)
    {
        MY_LOGE("WMF enque second pass failed!!");
        goto lbExit;
    }

    VSDOF_PRFLOG("threadLoop end: WMF Sec-Pass, reqID=%d", reqPtr->getRequestNo());
    return MTRUE;

lbExit:
    // mark on-going-request end
    this->decExtThreadDependency();
    return MFALSE;
}


MVOID
WMFNode::
onWMFEnqueDone_SecPass(WMFEParams& rParams)
{
    EnquedBufPool* pEnquePool = reinterpret_cast<EnquedBufPool*>(rParams.mpCookie);
    WMFNode* pWMFNode = pEnquePool->mpNode;
    pWMFNode->handleWMFEnqueDone_SecPass(rParams, pEnquePool);
}

MVOID
WMFNode::
handleWMFEnqueDone_SecPass(WMFEParams& rParams, EnquedBufPool* pEnqueBufPool)
{
    ImgInfoMapPtr ToWMFSecPass_ImgBufInfo = pEnqueBufPool->mpImgBufInfo;
    EffectRequestPtr pEffectReq = ToWMFSecPass_ImgBufInfo->getRequestPtr();

    VSDOF_PRFLOG("+, reqID=%d", pEffectReq->getRequestNo());

    // mark DepthMap output ready and output
    FrameInfoPtr framePtr_DepthMap;
    RETRIEVE_OFRAMEINFO(pEffectReq, framePtr_DepthMap, BID_OUT_DEPTHMAP);
    framePtr_DepthMap->setFrameReady(true);
    handleDataAndDump(WMF_OUT_DEPTHMAP,framePtr_DepthMap);

    delete pEnqueBufPool;

    VSDOF_PRFLOG("-, reqID=%d", pEffectReq->getRequestNo());
    // mark on-going-request end
    this->decExtThreadDependency();
}

MVOID
WMFNode::
prepareWMFEnqueConfig(sp<IImageBuffer> pImgBuf_SrcImg, sp<IImageBuffer> pImgBuf_SrcDepth, sp<IImageBuffer> pImgBuf_OutputDepth,  WMFEConfig &rWMFConfig, EnquedBufPool* pEnqueBufPool)
{
    // WMF TBLI tuning buffer
    setupDPEBufInfo(DMA_WMFE_TBLI, mpTbliImgBuf->mImageBuffer, rWMFConfig.Wmfe_Ctrl_0.Wmfe_Tbli);
    setupDPEBufInfo(DMA_WMFE_TBLI, mpTbliImgBuf->mImageBuffer, rWMFConfig.Wmfe_Ctrl_1.Wmfe_Tbli);


    // filter size
    rWMFConfig.Wmfe_Ctrl_0.WmfeFilterSize = eWMFFilterSize;
    rWMFConfig.Wmfe_Ctrl_1.WmfeFilterSize = eWMFFilterSize;
    // config WMFEConfig set 0
    rWMFConfig.Wmfe_Ctrl_0.Wmfe_Enable = true;
    rWMFConfig.Wmfe_Ctrl_1.Wmfe_Enable = true;
    // size are all the same, use MY_S size
    rWMFConfig.Wmfe_Ctrl_0.Wmfe_Width = pImgBuf_SrcImg->getImgSize().w;
    rWMFConfig.Wmfe_Ctrl_0.Wmfe_Height = pImgBuf_SrcImg->getImgSize().h;

    SmartImageBuffer smDMWInternal = mBufPoolSet.mpInternalDMWImgBufPool->request();
    pEnqueBufPool->addBuffData(BID_DMW_INTERNAL, smDMWInternal);
    rWMFConfig.Wmfe_Ctrl_1.Wmfe_Width = smDMWInternal->mImageBuffer->getImgSize().w;
    rWMFConfig.Wmfe_Ctrl_1.Wmfe_Height = smDMWInternal->mImageBuffer->getImgSize().h;
    // Y8 --> DPE_IMGI_Y_FMT
    rWMFConfig.Wmfe_Ctrl_1.WmfeImgiFmt = DPE_IMGI_Y_FMT;

    // IMGI FMT setting
    if(pImgBuf_SrcImg->getImgFormat() == eImgFmt_YV12 || pImgBuf_SrcImg->getImgFormat() == eImgFmt_Y8)
    {
        rWMFConfig.Wmfe_Ctrl_0.WmfeImgiFmt = DPE_IMGI_Y_FMT;
    }
    else if(pImgBuf_SrcImg->getImgFormat() == eImgFmt_YUY2)
    {
        rWMFConfig.Wmfe_Ctrl_0.WmfeImgiFmt = DPE_IMGI_YC_FMT;
    }
    else
    {
        MY_LOGE("WMF CTRL0:IMGI Format not-suporrted!! ImageFormat=%x", pImgBuf_SrcImg->getImgFormat());
        return;
    }

    rWMFConfig.Wmfe_Ctrl_0.WmfeDpiFmt = WMFE_DPI_D_FMT;
    rWMFConfig.Wmfe_Ctrl_1.WmfeDpiFmt = WMFE_DPI_D_FMT;
    // CTRL0: input buffer IMGI pImgBuf_SrcImg: MY_S or MY_SL
    setupDPEBufInfo(DMA_WMFE_IMGI, pImgBuf_SrcImg, rWMFConfig.Wmfe_Ctrl_0.Wmfe_Imgi);
    // CTRL0: input buffer DPI pImgBuf_SrcDepth:  DMH or DMW
    setupDPEBufInfo(DMA_WMFE_DPI, pImgBuf_SrcDepth, rWMFConfig.Wmfe_Ctrl_0.Wmfe_Dpi);
    // CTRL0: output buffer DPO: DMW_Internal
    setupDPEBufInfo(DMA_WMFE_DPO, smDMWInternal->mImageBuffer, rWMFConfig.Wmfe_Ctrl_0.Wmfe_Dpo);

    // CTRL1: input buffer IMGI: MY_S or MY_SL
    setupDPEBufInfo(DMA_WMFE_IMGI, pImgBuf_SrcImg, rWMFConfig.Wmfe_Ctrl_1.Wmfe_Imgi);
    // CTRL1: input buffer DPI: DMW_Internal
    setupDPEBufInfo(DMA_WMFE_DPI, smDMWInternal->mImageBuffer, rWMFConfig.Wmfe_Ctrl_1.Wmfe_Dpi);
    // CTRL1: output buffer DPO: DMW_Internal
    setupDPEBufInfo(DMA_WMFE_DPO, pImgBuf_OutputDepth, rWMFConfig.Wmfe_Ctrl_1.Wmfe_Dpo);

    // disable the set 2
    rWMFConfig.Wmfe_Ctrl_2.Wmfe_Enable = false;
}

MBOOL
WMFNode::
setupDPEBufInfo(DPEDMAPort dmaPort, android::sp<IImageBuffer> &pImgBuf, DPEBufInfo& rBufInfo)
{
    // plane 0 address
    rBufInfo.memID = pImgBuf->getFD(0);
    rBufInfo.dmaport = dmaPort;
    rBufInfo.u4BufVA = pImgBuf->getBufVA(0);
    rBufInfo.u4BufPA = pImgBuf->getBufPA(0);
    rBufInfo.u4BufSize = pImgBuf->getBufSizeInBytes(0);
    rBufInfo.u4Stride = pImgBuf->getBufStridesInBytes(0);

    return MTRUE;
}

}; //NSFeaturePipe
}; //NSCamFeature
}; //NSCam
