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
#include <featurePipe/vsdof/depthmap/DPENode.h>
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

class N3DBufferSizeConfig
{
public:
    N3DBufferSizeConfig();
    virtual ~N3DBufferSizeConfig();

public:
    // buffer size const
    MSize VSDOF_N3D_OUTPUT_IMG_SIZE;
    MSize VSDOF_N3D_OUTPUT_MASK_SIZE;
};

class N3DBufferPoolSet
{
public:
    N3DBufferPoolSet();
    virtual ~N3DBufferPoolSet();

    MBOOL init(const N3DBufferSizeConfig& config);
    MBOOL uninit();
public:
    android::sp<ImageBufferPool> mN3DImgBufPool;
    android::sp<ImageBufferPool> mN3DMaskBufPool;
};


N3DBufferSizeConfig::
N3DBufferSizeConfig()
{
    // setup constants
    StereoSizeProvider * pSizeProvder = StereoSizeProvider::getInstance();
    // img size is independent to scenarios
    VSDOF_N3D_OUTPUT_IMG_SIZE = pSizeProvder->getBufferSize(E_MV_Y, eSTEREO_SCENARIO_RECORD);
    // mask size is independent to scenarios
    VSDOF_N3D_OUTPUT_MASK_SIZE = pSizeProvder->getBufferSize(E_MASK_M_Y, eSTEREO_SCENARIO_RECORD);
}

N3DBufferSizeConfig::
~N3DBufferSizeConfig()
{}

N3DBufferPoolSet::
N3DBufferPoolSet()
{}

N3DBufferPoolSet::
~N3DBufferPoolSet()
{}

MBOOL
N3DBufferPoolSet::
init(const N3DBufferSizeConfig& config)
{
    // SW read/write, hw read
    MUINT32 usage = eBUFFER_USAGE_SW_READ_OFTEN | eBUFFER_USAGE_SW_WRITE_OFTEN | eBUFFER_USAGE_HW_CAMERA_READ;

    // INIT N3D output buffer pool - for MV/SV
    mN3DImgBufPool = ImageBufferPool::create("N3DImgBufPool", config.VSDOF_N3D_OUTPUT_IMG_SIZE.w,
                                        config.VSDOF_N3D_OUTPUT_IMG_SIZE.h, eImgFmt_YV12, usage);
    // INIT N3D output buffer pool - for MASK_M/MASK_Y
    mN3DMaskBufPool = ImageBufferPool::create("N3DMaskBufPool", config.VSDOF_N3D_OUTPUT_MASK_SIZE.w,
                                        config.VSDOF_N3D_OUTPUT_MASK_SIZE.h, eImgFmt_Y8, usage);
    // for MV/SV -> double size
    ALLOCATE_BUFFER_POOL(mN3DImgBufPool, 20 );
    // for MASK_M/MASK_Y -> double size
    ALLOCATE_BUFFER_POOL(mN3DMaskBufPool, 20);
    return MTRUE;
}

MBOOL
N3DBufferPoolSet::
uninit()
{
    ImageBufferPool::destroy(mN3DImgBufPool);
    ImageBufferPool::destroy(mN3DMaskBufPool);

    return MTRUE;
}

static N3DBufferSizeConfig sizeConfig;
static N3DBufferPoolSet bufPoolSet;

static MVOID setupEnvironment()
{
    bufPoolSet.init(sizeConfig);
}

#define USE_REAL
MVOID prepareDPEInputImgInfoMap(ImgInfoMapPtr& pToDPEImgInfo, int index)
{
    MY_LOGD("+");

#ifndef USE_REAL
    char MVY_FileName[256] = {'\0'};
    char SVY_FileName[256] = {'\0'};
    char MASK_M_FileName[256] = {'\0'};
    char MASK_S_FileName[256] = {'\0'};

    snprintf(MVY_FileName, 256, "/sdcard/vsdof/DPE/m_pDown_R_%5d.yv12", index);
    snprintf(SVY_FileName, 256, "/sdcard/vsdof/DPE/m_pDown_L_%5d.yv12", index);
    snprintf(MASK_M_FileName, 256, "/sdcard/vsdof/DPE/mask_dv_R_%5d.raw", index);
    snprintf(MASK_S_FileName, 256, "/sdcard/vsdof/DPE/mask_dv_L_%5d.raw", index);
#else
    char* MVY_FileName = "/sdcard/vsdof/DPE/BID_TO_DPE_OCC_MV_Y_272x144.yuv";
    char* SVY_FileName = "/sdcard/vsdof/DPE/BID_TO_DPE_OCC_SV_Y_272x144.yuv";
    char* MASK_M_FileName = "/sdcard/vsdof/DPE/BID_TO_DPE_MASK_M_272x144.yuv";
    char* MASK_S_FileName = "/sdcard/vsdof/DPE/BID_TO_DPE_MASK_S_272x144.yuv";
#endif


    SmartImageBuffer smImgBuf_MV_Y = bufPoolSet.mN3DImgBufPool->request();
    SmartImageBuffer smImgBuf_SV_Y = bufPoolSet.mN3DImgBufPool->request();
    SmartImageBuffer smImgBuf_MASK_M = bufPoolSet.mN3DMaskBufPool->request();
    SmartImageBuffer smImgBuf_MASK_S = bufPoolSet.mN3DMaskBufPool->request();

    LOAD_SMART_IMAGE_BUFFER(smImgBuf_MV_Y, "N3DImgBufPool", MVY_FileName, ImageBufferPool::USAGE_HW);
    LOAD_SMART_IMAGE_BUFFER(smImgBuf_SV_Y, "N3DImgBufPool", SVY_FileName, ImageBufferPool::USAGE_HW);

    LOAD_SMART_IMAGE_BUFFER(smImgBuf_MASK_M, "N3DMaskBufPool", MASK_M_FileName, ImageBufferPool::USAGE_HW);
    LOAD_SMART_IMAGE_BUFFER(smImgBuf_MASK_S, "N3DMaskBufPool", MASK_S_FileName, ImageBufferPool::USAGE_HW);

    pToDPEImgInfo->addImageBuffer(BID_TO_DPE_OCC_MV_Y, smImgBuf_MV_Y);
    pToDPEImgInfo->addImageBuffer(BID_TO_DPE_OCC_SV_Y, smImgBuf_SV_Y);
    pToDPEImgInfo->addImageBuffer(BID_TO_DPE_MASK_M, smImgBuf_MASK_M);
    pToDPEImgInfo->addImageBuffer(BID_TO_DPE_MASK_S, smImgBuf_MASK_S);
    MY_LOGD("-");
}

static MVOID setupDumpProperties(DPENode *pDPENode)
{
    const char* name = pDPENode->getName();

    property_set(name, "1");

    char prop_name[256];

    snprintf(prop_name, 256, "%s.%s", name, DepthMapDataHandler::ID2Name(DPE_TO_OCC_MVSV_DMP_CFM));
    property_set(prop_name, "1");
}

TEST(DepthMapPipe_DPENode, Standard)
{

    setupEnvironment();

    std::vector<ImgInfoMapPtr> vDPEInputImgInfoVec;
    std::vector<MUINT32> vReqIDVec;

    for(int index=0;index<1;++index)
    {
        sp<EffectRequest> pEffectReq = new EffectRequest(index);
        vReqIDVec.push_back(index);
        sp<EffectParameter> pEffectParam = new EffectParameter();
        ImgInfoMapPtr pToDPEImgInfo = new ImageBufInfoMap(pEffectReq);
        // ------------- test VR scenario (should be no differences in scenarios)----------------
        pEffectParam->set(DEPTHMAP_REQUEST_STATE_KEY, (int)STATE_NORMAL);
        pEffectReq->setRequestParameter(pEffectParam);
        // prepare DPE input ImgInfoMapPtr
        prepareDPEInputImgInfoMap(pToDPEImgInfo, index);
        vDPEInputImgInfoVec.push_back(pToDPEImgInfo);
    }


    DPENode* pDPENode = new DPENode("DPE_UT", NULL);
    CallbackUTNode *pCallbackUTNode = new CallbackUTNode("CBUTNode", ut_finishCB);
    pDPENode->connectData(DPE_TO_OCC_MVSV_DMP_CFM, DPE_TO_OCC_MVSV_DMP_CFM, pCallbackUTNode);

    vector<DepthMapDataID> vWaitDataID;
    vWaitDataID.push_back(DPE_TO_OCC_MVSV_DMP_CFM);
    pCallbackUTNode->setWaitingDataIDAndReqID(vWaitDataID, vReqIDVec);

    // dump buffers
    setupDumpProperties(pDPENode);

    // init/start nodes
    MY_LOGD("DPENode init!!!!!!!!!!");
    pDPENode->init();
    pCallbackUTNode->init();
    MY_LOGD("DPENode start!!!!!!!!!!");
    pDPENode->start();
    pCallbackUTNode->start();
    MY_LOGD("DPENode handleData!!!!!!!!!!!!!!!!!!!");

    for(size_t index = 0;index<vDPEInputImgInfoVec.size();index++)
    {
        // pass data to DPENode
        pDPENode->handleUTData(N3D_TO_DPE_MVSV_MASK, vDPEInputImgInfoVec[index]);
    }

    bool bRet = WaitingListener::waitRequest(vDPEInputImgInfoVec.size(), 5);
    EXPECT_TRUE(bRet);

    MY_LOGD("DPENode stop!!!!!!!!!!!!!!!!!!");
    vDPEInputImgInfoVec.clear();
    pCallbackUTNode->stop();
    pCallbackUTNode->uninit();
    pDPENode->stop();
    pDPENode->uninit();
    WaitingListener::resetCounter();
    delete pCallbackUTNode;
    delete pDPENode;
}

}
}
}





