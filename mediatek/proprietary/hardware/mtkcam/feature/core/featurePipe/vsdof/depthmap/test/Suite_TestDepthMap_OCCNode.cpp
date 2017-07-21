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
#include <featurePipe/vsdof/depthmap/OCCNode.h>
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


class DPEBufferSizeConfig
{
public:
    DPEBufferSizeConfig();
    virtual ~DPEBufferSizeConfig();

public:
    MSize VSDOF_DMP_SIZE;
    MSize VSDOF_CFM_SIZE;
    MSize VSDOF_RESPO_SIZE;
    MSize VSDOF_N3D_LDC_SIZE;
    MSize VSDOF_N3D_OUTPUT_IMG_SIZE;
    // stride size
    MUINT32 VSDOF_ORG_IMG_WIDTH;

};

class DPEBufferPoolSet
{
public:
    DPEBufferPoolSet();
    virtual ~DPEBufferPoolSet();

    MBOOL init(const DPEBufferSizeConfig& config);
    MBOOL uninit();
public:
    android::sp<ImageBufferPool> mDMPBuffPool;
    android::sp<ImageBufferPool> mCFMBuffPool;
    android::sp<ImageBufferPool> mRespBuffPool;
    android::sp<ImageBufferPool> mLDCBufPool;
    android::sp<ImageBufferPool> mN3DImgBufPool;
};


DPEBufferSizeConfig::
DPEBufferSizeConfig()
{
     // setup constans
    StereoSizeProvider* pSizePrvder = StereoSizeProvider::getInstance();
    VSDOF_DMP_SIZE = pSizePrvder->getBufferSize(E_DMP_H, eSTEREO_SCENARIO_RECORD);
    VSDOF_CFM_SIZE = pSizePrvder->getBufferSize(E_CFM_H, eSTEREO_SCENARIO_RECORD);
    VSDOF_RESPO_SIZE = pSizePrvder->getBufferSize(E_RESPO, eSTEREO_SCENARIO_RECORD);
    VSDOF_N3D_OUTPUT_IMG_SIZE = pSizePrvder->getBufferSize(E_MV_Y, eSTEREO_SCENARIO_RECORD);

    // original image width is the size before N3D in Cap
    Pass2SizeInfo pass2SizeInfo;
    pSizePrvder->getPass2SizeInfo(PASS2A_P_2, eSTEREO_SCENARIO_CAPTURE, pass2SizeInfo);
    VSDOF_ORG_IMG_WIDTH = pass2SizeInfo.areaWDMA.size.w;
    VSDOF_N3D_LDC_SIZE = pSizePrvder->getBufferSize(E_LDC, eSTEREO_SCENARIO_RECORD);
}

DPEBufferSizeConfig::
~DPEBufferSizeConfig()
{}

DPEBufferPoolSet::
DPEBufferPoolSet()
{}

DPEBufferPoolSet::
~DPEBufferPoolSet()
{}

MBOOL
DPEBufferPoolSet::
init(const DPEBufferSizeConfig& config)
{
    // allocate with the stride size as the width
    mDMPBuffPool = ImageBufferPool::create("DMPBufPool", config.VSDOF_DMP_SIZE.w,
                                        config.VSDOF_DMP_SIZE.h, eImgFmt_Y16, ImageBufferPool::USAGE_HW);
    mCFMBuffPool = ImageBufferPool::create("CFMBufPool", config.VSDOF_CFM_SIZE.w,
                                        config.VSDOF_CFM_SIZE.h, eImgFmt_Y8, ImageBufferPool::USAGE_HW);
    mRespBuffPool = ImageBufferPool::create("RESPOBufPool", config.VSDOF_RESPO_SIZE.w,
                                        config.VSDOF_RESPO_SIZE.h, eImgFmt_Y8, ImageBufferPool::USAGE_HW);

    mLDCBufPool = ImageBufferPool::create("LDCBufPool", config.VSDOF_N3D_LDC_SIZE.w,
                                            config.VSDOF_N3D_LDC_SIZE.h, eImgFmt_Y8, ImageBufferPool::USAGE_SW);

    mN3DImgBufPool = ImageBufferPool::create("N3DImgBufPool", config.VSDOF_N3D_OUTPUT_IMG_SIZE.w,
                                        config.VSDOF_N3D_OUTPUT_IMG_SIZE.h, eImgFmt_YV12, ImageBufferPool::USAGE_HW);

    //DMP, CFM, RESPO have all Left/Right side ->  double size
    ALLOCATE_BUFFER_POOL(mDMPBuffPool, VSDOF_WORKING_BUF_SET*2);
    ALLOCATE_BUFFER_POOL(mCFMBuffPool, VSDOF_WORKING_BUF_SET*2);
    ALLOCATE_BUFFER_POOL(mRespBuffPool, VSDOF_WORKING_BUF_SET*2);
    ALLOCATE_BUFFER_POOL(mLDCBufPool, VSDOF_WORKING_BUF_SET*2);
    ALLOCATE_BUFFER_POOL(mN3DImgBufPool, VSDOF_WORKING_BUF_SET*2);

    // need hold the previous frame's DMP_LR buffer, extra allocate two
    ALLOCATE_BUFFER_POOL(mDMPBuffPool, 2);
    return MTRUE;
}

MBOOL
DPEBufferPoolSet::
uninit()
{
    ImageBufferPool::destroy(mDMPBuffPool);
    ImageBufferPool::destroy(mCFMBuffPool);
    ImageBufferPool::destroy(mRespBuffPool);
    return MTRUE;
}

static DPEBufferSizeConfig DPEBufSizeConfig;
static DPEBufferPoolSet DPEBufPoolSet;

static MVOID setupEnvironment()
{
    DPEBufPoolSet.init(DPEBufSizeConfig);
}

#define USE_REAL

MVOID prepareOCCInputImgInfoMap(ImgInfoMapPtr& pToOCCImgInfoFromN3D, ImgInfoMapPtr& pToOCCImgInfoFromDPE, sp<EffectRequest> pEffReq)
{
    MY_LOGD("+");

#ifndef USE_REAL
    const char* sMV_Y_FileName = "/sdcard/vsdof/OCC/m_pDown_R_00000.yv12";
    const char* sSV_Y_FileName = "/sdcard/vsdof/OCC/m_pDown_L_00000.yv12";
    //const char* sDMP_R_FileName = "/sdcard/vsdof/OCC/DV_HW_R_00000.raw";
    //const char* sDMP_L_FileName = "/sdcard/vsdof/OCC/DV_HW_L_00000.raw";
    const char* sLDC_FileName = "/sdcard/vsdof/OCC/XVEC_N3D.raw";

    const char* sDMP_R_FileName = "/sdcard/vsdof/OCC/BID_TO_OCC_DMP_R_272x144.yuv";
    const char* sDMP_L_FileName = "/sdcard/vsdof/OCC/BID_TO_OCC_DMP_L_272x144.yuv";

    /*
    const char* sMV_Y_FileName = "/sdcard/vsdof/OCC2/imageMain1_00000.yv12";
    const char* sSV_Y_FileName = "/sdcard/vsdof/OCC2/imageMain2_00000.yv12";
    const char* sDMP_R_FileName = "/sdcard/vsdof/OCC2/disparityRightToLEft_00000.raw";
    const char* sDMP_L_FileName = "/sdcard/vsdof/OCC2/disparityLeftToRight_00000.raw";
    const char* sLDC_FileName = "/sdcard/vsdof/OCC2/ldcMain1_00000.raw";
    */
#else
    const char* sMV_Y_FileName = "/sdcard/vsdof/OCC/BID_TO_DPE_OCC_MV_Y_272x144.yuv";
    const char* sSV_Y_FileName = "/sdcard/vsdof/OCC/BID_TO_DPE_OCC_SV_Y_272x144.yuv";
    const char* sDMP_R_FileName = "/sdcard/vsdof/OCC/BID_TO_OCC_DMP_R_272x144.yuv";
    const char* sDMP_L_FileName = "/sdcard/vsdof/OCC/BID_TO_OCC_DMP_L_272x144.yuv";
    const char* sLDC_FileName = "/sdcard/vsdof/OCC/BID_TO_OCC_LDC_272x144.yuv";

#endif

    //From N3D
    SmartImageBuffer sm_LDC = DPEBufPoolSet.mLDCBufPool->request();
    LOAD_SMART_IMAGE_BUFFER(sm_LDC, "LDCBufPool", sLDC_FileName, ImageBufferPool::USAGE_HW);
    pToOCCImgInfoFromN3D->addImageBuffer(BID_TO_OCC_LDC, sm_LDC);

    //From DPE
    SmartImageBuffer sm_MV_Y = DPEBufPoolSet.mN3DImgBufPool->request();
    LOAD_SMART_IMAGE_BUFFER(sm_MV_Y, "N3DImgBufPool", sMV_Y_FileName, ImageBufferPool::USAGE_HW);
    SmartImageBuffer sm_SV_Y = DPEBufPoolSet.mN3DImgBufPool->request();
    LOAD_SMART_IMAGE_BUFFER(sm_SV_Y, "N3DImgBufPool", sSV_Y_FileName, ImageBufferPool::USAGE_HW);
    SmartImageBuffer smImgBuf_DMP_L = DPEBufPoolSet.mDMPBuffPool->request();
    LOAD_SMART_IMAGE_BUFFER(smImgBuf_DMP_L, "DMPBufPool", sDMP_L_FileName, ImageBufferPool::USAGE_HW);
    SmartImageBuffer smImgBuf_DMP_R = DPEBufPoolSet.mDMPBuffPool->request();
    LOAD_SMART_IMAGE_BUFFER(smImgBuf_DMP_R, "DMPBufPool", sDMP_R_FileName, ImageBufferPool::USAGE_HW);

    pToOCCImgInfoFromDPE->addImageBuffer(BID_TO_DPE_OCC_MV_Y, sm_MV_Y);
    pToOCCImgInfoFromDPE->addImageBuffer(BID_TO_DPE_OCC_SV_Y, sm_SV_Y);
    pToOCCImgInfoFromDPE->addImageBuffer(BID_TO_OCC_DMP_L, smImgBuf_DMP_L);
    pToOCCImgInfoFromDPE->addImageBuffer(BID_TO_OCC_DMP_R, smImgBuf_DMP_R);

    //MY_S
    sp<EffectFrameInfo> frameInfo = new EffectFrameInfo(pEffReq->getRequestNo(), BID_OUT_MY_S);
    IImageBufferAllocator::ImgParam imgParam_MYS = getImgParam_MY_S();
    sp<IImageBuffer> buf = createEmptyImageBuffer(imgParam_MYS, "imgParam_MYS", eBUFFER_USAGE_HW_CAMERA_WRITE|eBUFFER_USAGE_SW_READ_OFTEN|eBUFFER_USAGE_SW_WRITE_OFTEN);
    frameInfo->setFrameBuffer(buf);
    pEffReq->vOutputFrameInfo.add(BID_OUT_MY_S, frameInfo);

    MY_LOGD("-");
}

MVOID setupDumpProperties(OCCNode *pOCCNode)
{
    const char* name = pOCCNode->getName();
    property_set(name, "1");
    char prop_name[256];

    snprintf(prop_name, 256, "%s.%s", name, DepthMapDataHandler::ID2Name(OCC_TO_WMF_DMH_MY_S));
    property_set(prop_name, "1");

}

TEST(DepthMapPipe_OCCNode, Standard)
{
    std::vector<MUINT32> vReqIDVec;
    setupEnvironment();

    MUINT reqID = 0;
    sp<EffectRequest> pEffectReq = new EffectRequest(reqID);
    vReqIDVec.push_back(reqID);

    sp<EffectParameter> pEffectParam = new EffectParameter();
    ImgInfoMapPtr pToOCCImgInfoFromN3D = new ImageBufInfoMap(pEffectReq);
    ImgInfoMapPtr pToOCCImgInfoFromDPE = new ImageBufInfoMap(pEffectReq);

    // ------------- test VR scenario (should be no differences in scenarios)----------------
    pEffectParam->set(DEPTHMAP_REQUEST_STATE_KEY, (int)STATE_NORMAL);
    pEffectReq->setRequestParameter(pEffectParam);

    // prepare DPE input ImgInfoMapPtr
    prepareOCCInputImgInfoMap(pToOCCImgInfoFromN3D, pToOCCImgInfoFromDPE, pEffectReq);
    OCCNode* pOCCNode = new OCCNode("OCCNODE_UT", NULL);
    CallbackUTNode *pCallbackUTNode = new CallbackUTNode("CBUTNode", ut_finishCB);
    pOCCNode->connectData(OCC_TO_WMF_DMH_MY_S, OCC_TO_WMF_DMH_MY_S, pCallbackUTNode);

    vector<DepthMapDataID> vWaitDataID;
    vWaitDataID.push_back(OCC_TO_WMF_DMH_MY_S);
    pCallbackUTNode->setWaitingDataIDAndReqID(vWaitDataID, vReqIDVec);

    setupDumpProperties(pOCCNode);

    // init/start nodes
    MY_LOGD("pOCCNode init!!!!!!!!!!");
    pOCCNode->init();
    pCallbackUTNode->init();
    MY_LOGD("pOCCNode start!!!!!!!!!!");
    pOCCNode->start();
    pCallbackUTNode->start();
    MY_LOGD("pOCCNode handleData!!!!! !!!!!!!!!!!!!!");

    // pass data to OCCNode
    pOCCNode->handleUTData(N3D_TO_OCC_LDC, pToOCCImgInfoFromN3D);
    pOCCNode->handleUTData(DPE_TO_OCC_MVSV_DMP_CFM, pToOCCImgInfoFromDPE);

    // dump buffers
    setupDumpProperties(pOCCNode);

    MY_LOGD("pOCCNode start to wait!!....");
    bool bRet = WaitingListener::waitRequest(1,5);
    EXPECT_TRUE(bRet);

    pOCCNode->stop();
    pOCCNode->uninit();
    pCallbackUTNode->stop();
    pCallbackUTNode->uninit();
    WaitingListener::resetCounter();
    delete pOCCNode;
    delete pCallbackUTNode;
}




}
}
}