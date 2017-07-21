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
#include <featurePipe/vsdof/depthmap/FDNode.h>
#include <vsdof/hal/stereo_size_provider.h>
#include <featurePipe/vsdof/depthmap/DepthMapPipeUtils.h>
#include <featurePipe/vsdof/depthmap/DepthMapPipeNode.h>
#include <featurePipe/vsdof/depthmap/DepthMapPipe_Common.h>
#include <metadata/client/mtk_metadata_tag.h>

#include "CallbackUTNode.h"
#include "TestDepthMapPipe_Common.h"

using android::sp;
using namespace NSCam;
using namespace std;

namespace NSCam{
namespace NSCamFeature{
namespace NSFeaturePipe{

class P2ABufferSizeConfig_FD
{
public:
    P2ABufferSizeConfig_FD();
    virtual ~P2ABufferSizeConfig_FD() {};

public:
    MSize P2AFM_FD_IMG_SIZE;

};

class P2ABufferPoolSet_FD
{
public:
    P2ABufferPoolSet_FD();
    virtual ~P2ABufferPoolSet_FD();

    MBOOL init(const P2ABufferSizeConfig_FD& config);
    MBOOL uninit();
public:
    // buffer pools
    android::sp<ImageBufferPool> mpFDBufPool;
};


P2ABufferSizeConfig_FD::
P2ABufferSizeConfig_FD()
{
    Pass2SizeInfo pass2SizeInfo;
    StereoSizeProvider* pSizePrvder = StereoSizeProvider::getInstance();
    pSizePrvder->getPass2SizeInfo(PASS2A, eSTEREO_SCENARIO_RECORD, pass2SizeInfo);
    P2AFM_FD_IMG_SIZE = pass2SizeInfo.szIMG2O;
    MY_LOGD("P2AFM_FD_IMG_SIZE.w=%d P2AFM_FD_IMG_SIZE.h=%d", P2AFM_FD_IMG_SIZE.w, P2AFM_FD_IMG_SIZE.h);
}

P2ABufferPoolSet_FD::
P2ABufferPoolSet_FD()
{}

P2ABufferPoolSet_FD::
~P2ABufferPoolSet_FD()
{}


MBOOL
P2ABufferPoolSet_FD::
init(const P2ABufferSizeConfig_FD& config)
{
    MY_LOGD("+ : P2ABufferPoolSet_FD init ");

    // FD when Capture
    CREATE_IMGBUF_POOL(mpFDBufPool, "FDBufPool", config.P2AFM_FD_IMG_SIZE, eImgFmt_YUY2, ImageBufferPool::USAGE_HW, MFALSE);
    //CREATE_IMGBUF_POOL(mpFDBufPool, "FDBufPool", MSize(640, 480), eImgFmt_YUY2, ImageBufferPool::USAGE_HW, MFALSE);


    ALLOCATE_BUFFER_POOL(mpFDBufPool, VSDOF_WORKING_BUF_SET);
    MY_LOGD("- : P2ABufferPoolSet_FD init ");

    return MTRUE;
}

MBOOL
P2ABufferPoolSet_FD::
uninit()
{
    MY_LOGD("+");
    //ImageBufferPool::destroy(mpFDBufPool);
    MY_LOGD("-");

    return MTRUE;
}

#define DECLARE_MACRO()\
    sp<EffectFrameInfo> effectFrame;\
    MUINT32 reqIdx;

#define ADD_METABUF_FRAME(effectReq, frameMapName, frameId, rpMetaBuf) \
    rpMetaBuf = new IMetadata();\
    reqIdx = effectReq->getRequestNo();\
    effectFrame = new EffectFrameInfo(reqIdx, frameId); \
    {\
        sp<EffectParameter> effParam = new EffectParameter(); \
        effParam->setPtr(VSDOF_EFFECT_PARAMS_KEY, (void*)rpMetaBuf); \
        effectFrame->setFrameParameter(effParam); \
        effectReq->frameMapName.add(frameId, effectFrame); \
    }

#define ADD_METABUF_INPUT_FRAME(effectReq, frameId, rpMetaBuf)  \
    ADD_METABUF_FRAME(effectReq, vInputFrameInfo, frameId, rpMetaBuf)

#define ADD_METABUF_OUTPUT_FRAME(effectReq, frameId, rpMetaBuf)  \
    ADD_METABUF_FRAME(effectReq, vOutputFrameInfo, frameId, rpMetaBuf)

static P2ABufferSizeConfig_FD sizeConfig;
static P2ABufferPoolSet_FD bufferPoolSet;

static MVOID setupEnvironment()
{
    bufferPoolSet.init(sizeConfig);
}

MVOID setupDumpProperties(StereoFDNode *pFDNode)
{
    const char* name = pFDNode->getName();

    property_set(name, "1");

    char prop_name[256];

    snprintf(prop_name, 256, "%s.%s", name, DepthMapDataHandler::ID2Name(FD_OUT_EXTRADATA));
    property_set(prop_name, "1");
}

MVOID prepareFDInputImgInfoMap(sp<EffectRequest> pEffectReq, ImgInfoMapPtr& pToFDImgInfoFromP2A)
{
    MY_LOGD("+");

    DECLARE_MACRO();

    const char * sFDFilename = "/sdcard/vsdof/FD.raw";
    SmartImageBuffer smImgBuf_FD = bufferPoolSet.mpFDBufPool->request();
    LOAD_SMART_IMAGE_BUFFER(smImgBuf_FD, "FDImgBufPool", sFDFilename, ImageBufferPool::USAGE_HW);
    pToFDImgInfoFromP2A->addImageBuffer(BID_TO_FD_IMG, smImgBuf_FD);

    MY_LOGD("smImgBuf_FD====>%x", smImgBuf_FD.get());

    IMetadata* pMetadata;
    // InAppMeta
    ADD_METABUF_INPUT_FRAME(pEffectReq, BID_META_IN_APP, pMetadata);
    // InAppMeta - orientation
    trySetMetadata<MINT32>(pMetadata, MTK_JPEG_ORIENTATION, 0);

    // Extra data - blob format
    size_t bufSize = 1024;
    IImageBufferAllocator::ImgParam imgParam_ExtraData(bufSize, 0);
    IImageBuffer* buf = createEmptyImageBuffer(imgParam_ExtraData,  "ExtraData", ImageBufferPool::USAGE_SW);
    sp<EffectFrameInfo> frameInfo = new EffectFrameInfo(pEffectReq->getRequestNo(), BID_OUT_EXTRADATA);
    frameInfo->setFrameBuffer(buf);
    MY_LOGD("ExtraData imageBuffer=%x", buf);
    pEffectReq->vOutputFrameInfo.add(BID_OUT_EXTRADATA, frameInfo);

    MY_LOGD("BID_OUT_EXTRADATA===>%d", pEffectReq->vOutputFrameInfo.indexOfKey(BID_OUT_EXTRADATA));
    MY_LOGD("-");
}


TEST(DepthMapPipe_FDNode, Capture)
{
    std::vector<MUINT32> vReqIDVec;

    setupEnvironment();

    MUINT32 reqID = 0;
    sp<EffectRequest> pEffectReq = new EffectRequest(reqID);
    vReqIDVec.push_back(reqID);

    sp<EffectParameter> pEffectParam = new EffectParameter();
    // ------------- test Capture scenario ---------------
    pEffectParam->set(DEPTHMAP_REQUEST_STATE_KEY, (int)STATE_CAPTURE);
    pEffectReq->setRequestParameter(pEffectParam);
    sp<ImageBufInfoMap> TO_FD_ImgBufInfoFromP2A = new ImageBufInfoMap(pEffectReq);


    prepareFDInputImgInfoMap(pEffectReq, TO_FD_ImgBufInfoFromP2A);;

    StereoFDNode* pFDNode = new StereoFDNode("FDUTNode", NULL);
    CallbackUTNode *pCallbackUTNode = new CallbackUTNode("CBUTNode", ut_finishCB);
    pFDNode->connectData(FD_OUT_EXTRADATA, FD_OUT_EXTRADATA, pCallbackUTNode);
    // dump buffer
    setupDumpProperties(pFDNode);

    //wait IDs
    vector<DepthMapDataID> vWaitDataID;
    vWaitDataID.push_back(FD_OUT_EXTRADATA);
    pCallbackUTNode->setWaitingDataIDAndReqID(vWaitDataID, vReqIDVec);

    MY_LOGD("TO_FD_ImgBufInfoFromP2A count=%d", TO_FD_ImgBufInfoFromP2A->getStrongCount());

    // init/start nodes
    MY_LOGD("Node init!!!!!!!!!!");
    pFDNode->init();
    pCallbackUTNode->init();
    MY_LOGD("Node start!!!!!!!!!!");
    pFDNode->start();
    pCallbackUTNode->start();
    MY_LOGD("pFDNode handleData!!!!! !!!!!!!!!!!!!!");

     // pass data to FD
    pFDNode->handleUTData(P2AFM_TO_FD_IMG, TO_FD_ImgBufInfoFromP2A);
    MY_LOGD("TO_FD_ImgBufInfoFromP2A aaa count=%d", TO_FD_ImgBufInfoFromP2A->getStrongCount());
    ExtraDataInfoPtr pToFD_ExtraData = new ExtraDataInfo(pEffectReq, "{}");
    pFDNode->handleUTData(N3D_TO_FD_EXTDATA_MASK, pToFD_ExtraData);

    bool bRet = WaitingListener::waitRequest(1,5);
    EXPECT_TRUE(bRet);

    MY_LOGD("FDUT Ret=%d", bRet);

    MY_LOGD("TO_FD_ImgBufInfoFromP2A 2222 count=%d", TO_FD_ImgBufInfoFromP2A->getStrongCount());

    TO_FD_ImgBufInfoFromP2A = NULL;
    pEffectReq = NULL;
    pEffectParam = NULL;

    pCallbackUTNode->stop();
    pFDNode->stop();
    pCallbackUTNode->uninit();
    pFDNode->uninit();
    WaitingListener::resetCounter();
    MY_LOGD("delete pCallbackUTNode");
    delete pCallbackUTNode;
    MY_LOGD("delete pFDNode");
    delete pFDNode;
    MY_LOGD("bufferPoolSet.uninit");
    bufferPoolSet.uninit();

}








}
}
}