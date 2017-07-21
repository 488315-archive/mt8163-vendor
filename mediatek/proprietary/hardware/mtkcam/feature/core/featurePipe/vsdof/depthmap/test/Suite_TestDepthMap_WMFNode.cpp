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

#include <time.h>
#include <gtest/gtest.h>

#include <vector>
#include <IImageBuffer.h>
#include <mmsdk/EffectRequest.h>
#include <utils/include/ImageBufferHeap.h>
#include <featurePipe/vsdof/depthmap/WMFNode.h>
#include <vsdof/hal/stereo_size_provider.h>
#include <featurePipe/vsdof/depthmap/DepthMapPipeNode.h>
#include <featurePipe/vsdof/depthmap/DepthMapPipe_Common.h>

#include "CallbackUTNode.h"
#include "TestDepthMapPipe_Common.h"

using android::sp;
using namespace NSCam;
using namespace std;

namespace NSCam{
namespace NSCamFeature{
namespace NSFeaturePipe{

class OCCBufferPoolSet
{
    public:
        OCCBufferPoolSet();
        virtual ~OCCBufferPoolSet();

        MBOOL init(const MSize& szDMH);
        MBOOL uninit();
    public:
        android::sp<ImageBufferPool> mDMHImgBufPool;
        android::sp<ImageBufferPool> mMYSImgBufPool;
        android::sp<ImageBufferPool> mMYSLImgBufPool;
};


OCCBufferPoolSet::
OCCBufferPoolSet()
{}

OCCBufferPoolSet::
~OCCBufferPoolSet()
{}

MBOOL
OCCBufferPoolSet::
init(const MSize& szDMHSize)
{
    mDMHImgBufPool = ImageBufferPool::create("DMHBufPool", szDMHSize.w, szDMHSize.h,
                                                eImgFmt_Y8, eBUFFER_USAGE_SW_WRITE_OFTEN | eBUFFER_USAGE_SW_READ_OFTEN | eBUFFER_USAGE_HW_CAMERA_READ);

    Pass2SizeInfo pass2SizeInfo;
    StereoSizeProvider * pSizeProvder = StereoSizeProvider::getInstance();
    pSizeProvder->getPass2SizeInfo(PASS2A_2, eSTEREO_SCENARIO_CAPTURE, pass2SizeInfo);
    MSize P2AFM_FE1CO_INPUT_SIZE = pass2SizeInfo.szIMG2O;

    mMYSLImgBufPool = ImageBufferPool::create("MYSLBufPool", P2AFM_FE1CO_INPUT_SIZE.w,
                                            P2AFM_FE1CO_INPUT_SIZE.h, eImgFmt_YUY2, ImageBufferPool::USAGE_HW, MFALSE);

    MSize szMY_S = pSizeProvder->getBufferSize(E_MY_S, eSTEREO_SCENARIO_CAPTURE).size;
    mMYSImgBufPool = ImageBufferPool::create("MYSBufPool", szMY_S.w,
                                            szMY_S.h, eImgFmt_YV12, ImageBufferPool::USAGE_HW, MFALSE);

    ALLOCATE_BUFFER_POOL(mDMHImgBufPool, VSDOF_WORKING_BUF_SET);
    ALLOCATE_BUFFER_POOL(mMYSLImgBufPool, VSDOF_WORKING_BUF_SET);
    ALLOCATE_BUFFER_POOL(mMYSImgBufPool, VSDOF_WORKING_BUF_SET);

    return MTRUE;
}

MBOOL
OCCBufferPoolSet::
uninit()
{
    ImageBufferPool::destroy(mDMHImgBufPool);
    return MTRUE;
}


// setup constants
StereoSizeProvider * pSizeProvder = StereoSizeProvider::getInstance();
MSize VSDOF_OCC_OUTPUT_DMH_SIZE = pSizeProvder->getBufferSize(E_DMH, eSTEREO_SCENARIO_RECORD);
MSize VSDOF_OCC_OUTPUT_MYS_SIZE = pSizeProvder->getBufferSize(E_MY_S, eSTEREO_SCENARIO_RECORD);



static OCCBufferPoolSet OCCBufPoolSet;


static MVOID setupEnvironment()
{
    OCCBufPoolSet.init(VSDOF_OCC_OUTPUT_DMH_SIZE);
}

MVOID prepareWMFInputImgInfoMap_Cap(sp<EffectRequest> pEffReq, ImgInfoMapPtr& pToWMFImgInfoFromOCC, ImgInfoMapPtr& pToWMFImgInfoFromP2A)
{
    MY_LOGD("+");
    const char * sMYSFilename = "/sdcard/vsdof/MYS_00000.raw";
    const char * sDMHFilename = "/sdcard/vsdof/DMH_00000.raw";

    SmartImageBuffer smImgBuf_MY_SL = OCCBufPoolSet.mMYSLImgBufPool->request();
    // TODO: load the MY_SL
    // smImgBuf_MY_SL->mImageBuffer->loadFromFile();
    // because the BID_P2AFM_FE1CO_INPUT is also the BID_TO_WMF_MY_SL
    pToWMFImgInfoFromP2A->addImageBuffer(BID_P2AFM_FE1CO_INPUT, smImgBuf_MY_SL);

    // inset MY_S into EffectRequest's OutputFrameInfo
    sp<EffectFrameInfo> pFrameInfo = new EffectFrameInfo();
    SmartImageBuffer smImgBuf_MY_S = OCCBufPoolSet.mMYSImgBufPool->request();
    LOAD_SMART_IMAGE_BUFFER(smImgBuf_MY_S, "MYSBufPool", sMYSFilename, ImageBufferPool::USAGE_HW);
    pFrameInfo->setFrameBuffer(smImgBuf_MY_S->mImageBuffer);
    pEffReq->vOutputFrameInfo.add(BID_OUT_MY_S, pFrameInfo);

    // insert DMH to InfoMap
    SmartImageBuffer smImgBuf_DMH = OCCBufPoolSet.mDMHImgBufPool->request();
    LOAD_SMART_IMAGE_BUFFER(smImgBuf_DMH, "DMHBufPool", sDMHFilename, ImageBufferPool::USAGE_HW);
    pToWMFImgInfoFromOCC->addImageBuffer(BID_TO_WMF_DMH, smImgBuf_DMH);

    // prepare output frame: DMW
    pFrameInfo = new EffectFrameInfo(pEffReq->getRequestNo(), BID_OUT_DMW);
    IImageBufferAllocator::ImgParam imgParam_DMW = getImgParam_DMW();
    IImageBuffer* buf = createEmptyImageBuffer(imgParam_DMW, "DMWImg", ImageBufferPool::USAGE_HW);
    pFrameInfo->setFrameBuffer(buf);
    pEffReq->vOutputFrameInfo.add(BID_OUT_DMW, pFrameInfo);

    MY_LOGD("-");
}

#define USE_REAL

MVOID prepareWMFInputImgInfoMap_Rec(sp<EffectRequest> pEffReq, ImgInfoMapPtr& pToWMFImgInfoFromOCC)
{
    MY_LOGD("+");

    #ifndef USE_REAL
    const char * sMYSFilename = "/sdcard/vsdof/WMF/MYS_00000.raw";
    const char * sDMHFilename = "/sdcard/vsdof/WMF/DMH_00000.raw";
    #else
    const char * sMYSFilename = "/sdcard/vsdof/WMF/BID_OUT_MY_S_240x136.yuv";
    const char * sDMHFilename = "/sdcard/vsdof/WMF/BID_TO_WMF_DMH_240x136.yuv";
    #endif
    // inset MY_S into EffectRequest's OutputFrameInfo
    sp<EffectFrameInfo> pFrameInfo = new EffectFrameInfo(pEffReq->getRequestNo(), BID_OUT_MY_S);
    SmartImageBuffer smImgBuf_MY_S = OCCBufPoolSet.mMYSImgBufPool->request();
    LOAD_SMART_IMAGE_BUFFER(smImgBuf_MY_S, "MYSBufPool", sMYSFilename, ImageBufferPool::USAGE_HW);
    pFrameInfo->setFrameBuffer(smImgBuf_MY_S->mImageBuffer);
    pEffReq->vOutputFrameInfo.add(BID_OUT_MY_S, pFrameInfo);

    // insert DMH to InfoMap
    SmartImageBuffer smImgBuf_DMH = OCCBufPoolSet.mDMHImgBufPool->request();
    LOAD_SMART_IMAGE_BUFFER(smImgBuf_DMH, "DMHBufPool", sDMHFilename, ImageBufferPool::USAGE_HW);
    pToWMFImgInfoFromOCC->addImageBuffer(BID_TO_WMF_DMH, smImgBuf_DMH);

    // prepare output frame: DMW
    pFrameInfo = new EffectFrameInfo(pEffReq->getRequestNo(), BID_OUT_DMW);
    IImageBufferAllocator::ImgParam imgParam_DMW = getImgParam_DMW();
    IImageBuffer *buf = createEmptyImageBuffer(imgParam_DMW, "DMWImg", ImageBufferPool::USAGE_HW);
    pFrameInfo->setFrameBuffer(buf);
    pEffReq->vOutputFrameInfo.add(BID_OUT_DMW, pFrameInfo);

    MY_LOGD("-");
}


MVOID setupDumpProperties(WMFNode *pWMFNode)
{
    const char* name = pWMFNode->getName();

    property_set(name, "1");

    char prop_name[256];

    snprintf(prop_name, 256, "%s.%s", name, DepthMapDataHandler::ID2Name(WMF_OUT_DMW_MY_S));
    property_set(prop_name, "1");
    snprintf(prop_name, 256, "%s.%s", name, DepthMapDataHandler::ID2Name(WMF_OUT_DEPTHMAP));
    property_set(prop_name, "1");
}


TEST(DepthMapPipe_WMFNode, Record)
{
    std::vector<MUINT32> vReqIDVec;
    setupEnvironment();

    MUINT reqID = 0;
    sp<EffectRequest> pEffectReq = new EffectRequest(reqID);
    vReqIDVec.push_back(reqID);

    sp<EffectParameter> pEffectParam = new EffectParameter();
    ImgInfoMapPtr pToWMFImgInfo = new ImageBufInfoMap(pEffectReq);
    // ------------- test RECORD scenario ---------------
    pEffectParam->set(DEPTHMAP_REQUEST_STATE_KEY, (int)STATE_NORMAL);
    pEffectReq->setRequestParameter(pEffectParam);
    // prepare DPE input ImgInfoMapPtr
    prepareWMFInputImgInfoMap_Rec(pEffectReq, pToWMFImgInfo);

    WMFNode* pWMFNode = new WMFNode("WMFNODE_UT", NULL);
    CallbackUTNode *pCallbackUTNode = new CallbackUTNode("CBUTNode", ut_finishCB);
    pWMFNode->connectData(WMF_OUT_DMW_MY_S, WMF_OUT_DMW_MY_S, pCallbackUTNode);
    pWMFNode->connectData(WMF_OUT_DEPTHMAP, WMF_OUT_DEPTHMAP, pCallbackUTNode);

    // dump buffer
    setupDumpProperties(pWMFNode);

    vector<DepthMapDataID> vWaitDataID;
    vWaitDataID.push_back(WMF_OUT_DMW_MY_S);
    pCallbackUTNode->setWaitingDataIDAndReqID(vWaitDataID, vReqIDVec);
    // init/start nodes
    MY_LOGD("pWMFNode init!!!!!!!!!!");
    pWMFNode->init();
    pCallbackUTNode->init();
    MY_LOGD("pWMFNode start!!!!!!!!!!");
    pWMFNode->start();
    pCallbackUTNode->start();

    MY_LOGD("pWMFNode handleData!!!!! !!!!!!!!!!!!!!");

    // pass data to WMF
    pWMFNode->handleUTData(OCC_TO_WMF_DMH_MY_S, pToWMFImgInfo);

    MY_LOGD("pWMFNode start to wait!!....");
    bool bRet = WaitingListener::waitRequest(1,5);
    EXPECT_TRUE(bRet);

    pCallbackUTNode->stop();
    pCallbackUTNode->uninit();
    pWMFNode->stop();
    pWMFNode->uninit();
    WaitingListener::resetCounter();
    delete pCallbackUTNode;
    delete pWMFNode;
}

//TEST(DepthMapPipe_WMFNode, Capture)
MVOID capture()
{
    std::vector<MUINT32> vReqIDVec;
    setupEnvironment();

    MUINT reqID = 0;
    sp<EffectRequest> pEffectReq = new EffectRequest(reqID);
    vReqIDVec.push_back(reqID);

    sp<EffectParameter> pEffectParam = new EffectParameter();
    ImgInfoMapPtr pToWMFImgInfoFromOCC = new ImageBufInfoMap(pEffectReq);
    ImgInfoMapPtr pToWMFImgInfoFromP2A = new ImageBufInfoMap(pEffectReq);
    // ------------- test CAPTURE scenario ---------------
    pEffectParam->set(DEPTHMAP_REQUEST_STATE_KEY, (int)STATE_CAPTURE);
    pEffectReq->setRequestParameter(pEffectParam);
    // prepare DPE input ImgInfoMapPtr
    prepareWMFInputImgInfoMap_Cap(pEffectReq, pToWMFImgInfoFromOCC, pToWMFImgInfoFromP2A);

    WMFNode* pWMFNode = new WMFNode("WMFNODE_UT", NULL);
    CallbackUTNode *pCallbackUTNode = new CallbackUTNode("CBUTNode", ut_finishCB);
    pWMFNode->connectData(WMF_OUT_DMW_MY_S, WMF_OUT_DMW_MY_S, pCallbackUTNode);
    pWMFNode->connectData(WMF_OUT_DEPTHMAP, WMF_OUT_DEPTHMAP, pCallbackUTNode);

    // dump buffer
    setupDumpProperties(pWMFNode);

    vector<DepthMapDataID> vWaitDataID;
    vWaitDataID.push_back(WMF_OUT_DMW_MY_S);
    vWaitDataID.push_back(WMF_OUT_DEPTHMAP);
    pCallbackUTNode->setWaitingDataIDAndReqID(vWaitDataID, vReqIDVec);

    // init/start nodes
    MY_LOGD("Node init!!!!!!!!!!");
    pWMFNode->init();
    pCallbackUTNode->init();
    MY_LOGD("Node start!!!!!!!!!!");
    pWMFNode->start();
    pCallbackUTNode->start();

    MY_LOGD("pWMFNode handleData!!!!! !!!!!!!!!!!!!!");
    pWMFNode->handleUTData(P2AFM_TO_WMF_MY_SL, pToWMFImgInfoFromP2A);
    // pass data to WMF
    pWMFNode->handleUTData(OCC_TO_WMF_DMH_MY_S, pToWMFImgInfoFromOCC);

    bool bRet = WaitingListener::waitRequest(1,5);
    EXPECT_TRUE(bRet);


    pCallbackUTNode->stop();
    pCallbackUTNode->uninit();
    pWMFNode->stop();
    pWMFNode->uninit();
    WaitingListener::resetCounter();
    delete pCallbackUTNode;
    delete pWMFNode;
}



}
}
}

