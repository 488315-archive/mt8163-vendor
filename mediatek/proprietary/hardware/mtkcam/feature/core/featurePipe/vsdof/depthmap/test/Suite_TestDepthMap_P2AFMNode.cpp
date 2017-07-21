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

#include <IImageBuffer.h>
#include <utils/Vector.h>
#include <drv/imem_drv.h>
#include <utils/String8.h>
#include <cutils/properties.h>
#include <mmsdk/EffectRequest.h>
#include <pic/imgi_2560x1440_bayer10.h>
#include <utils/include/ImageBufferHeap.h>
#include <featurePipe/vsdof/depthmap/P2AFMNode.h>
#include <featurePipe/vsdof/depthmap/DepthMapPipeUtils.h>
#include <featurePipe/vsdof/depthmap/DepthMapPipeNode.h>
#include <featurePipe/vsdof/depthmap/DepthMapPipe_Common.h>

#include <metadata/mtk_platform_metadata_tag.h>
#include <metadata/client/mtk_metadata_tag.h>

#include "CallbackUTNode.h"
#include "TestDepthMapPipe_Common.h"

using android::sp;
using android::String8;
using namespace NSCam;

namespace NSCam{
namespace NSCamFeature{
namespace NSFeaturePipe{

typedef IImageBufferAllocator::ImgParam ImgParam;
StereoSizeProvider* gpSizePrvder = StereoSizeProvider::getInstance();


IImageBuffer* createImgBufUsingSample()
{
    IMemDrv* mpImemDrv=NULL;
	mpImemDrv=IMemDrv::createInstance();
	mpImemDrv->init();
    IMEM_BUF_INFO imgiBuf;
	MINT32 bufBoundaryInBytes[3] = {0, 0, 0};
	IImageBuffer* srcBuffer;
	MUINT32 bufStridesInBytes[3] = {3200, 0, 0};
    imgiBuf.size=sizeof(g_imgi_array_2560x1440_b10);
	mpImemDrv->allocVirtBuf(&imgiBuf);
    memcpy( (MUINT8*)(imgiBuf.virtAddr), (MUINT8*)(g_imgi_array_2560x1440_b10), imgiBuf.size);
     //imem buffer 2 image heap
    PortBufInfo_v1 portBufInfo = PortBufInfo_v1( imgiBuf.memID,imgiBuf.virtAddr,0,imgiBuf.bufSecu, imgiBuf.bufCohe);
    IImageBufferAllocator::ImgParam imgParam = IImageBufferAllocator::ImgParam((eImgFmt_BAYER10),MSize(2560, 1440), bufStridesInBytes, bufBoundaryInBytes, 1);
    sp<ImageBufferHeap> pHeap = ImageBufferHeap::create( LOG_TAG, imgParam,portBufInfo,true);
	srcBuffer = pHeap->createImageBuffer();
        srcBuffer->incStrong(srcBuffer);
        srcBuffer->lockBuf(LOG_TAG,eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
    return srcBuffer;
}

MVOID loadResizRawsImgBuf(sp<IImageBuffer>& rPMain1ImgBuf, sp<IImageBuffer>& rPMain2ImgBuf)
{
    const char * sMain1Filename = "/sdcard/vsdof/PASS1_RESIZEDRAW_0_1920x1080_3600_7.raw";
    const char * sMain2Filename = "/sdcard/vsdof/PASS1_RESIZEDRAW_2_1920x1080_3600_7.raw";

    MUINT32 bufStridesInBytes[3] = {3600, 0, 0};
    MINT32 bufBoundaryInBytes[3] = {0, 0, 0};
    IImageBufferAllocator::ImgParam imgParamRRZO = IImageBufferAllocator::ImgParam((eImgFmt_FG_BAYER10), MSize(1920, 1080), bufStridesInBytes, bufBoundaryInBytes, 1);

    rPMain1ImgBuf = createImageBufferFromFile(imgParamRRZO, sMain1Filename, "RsRaw1",  eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN | eBUFFER_USAGE_SW_WRITE_RARELY);
    rPMain2ImgBuf = createImageBufferFromFile(imgParamRRZO, sMain2Filename, "RsRaw2",  eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN | eBUFFER_USAGE_SW_WRITE_RARELY);
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

void prepareRequest_VR(sp<EffectRequest>& effectReq)
{
    DECLARE_MACRO();
    MUINT32 reqID = effectReq->getRequestNo();

    sp<IImageBuffer> buf_Main1, buf_Main2, buf;
    // load resize raw
    loadResizRawsImgBuf(buf_Main1, buf_Main2);
    MY_LOGD("RRZ_RAW1=%x RRZ_RAW2=%x", buf_Main1.get(), buf_Main2.get());

    // prepare input frame info: RSRAW1
    sp<EffectFrameInfo> frameInfo = new EffectFrameInfo(reqID, BID_P2AFM_IN_RSRAW1);
    frameInfo->setFrameBuffer(buf_Main1);
    effectReq->vInputFrameInfo.add(BID_P2AFM_IN_RSRAW1, frameInfo);

    // prepare input frame info: RSRAW2
    frameInfo = new EffectFrameInfo(reqID, BID_P2AFM_IN_RSRAW2);
    frameInfo->setFrameBuffer(buf_Main2);
    effectReq->vInputFrameInfo.add(BID_P2AFM_IN_RSRAW2, frameInfo);

    IMetadata* pMetadata;
    // prepare the metadata frame info
    // InAppMeta
    ADD_METABUF_INPUT_FRAME(effectReq, BID_META_IN_APP, pMetadata);
    // InAppMeta - EIS on
    trySetMetadata<MUINT8>(pMetadata, MTK_CONTROL_VIDEO_STABILIZATION_MODE, MTK_CONTROL_VIDEO_STABILIZATION_MODE_ON);
    // InHalMeta
    ADD_METABUF_INPUT_FRAME(effectReq, BID_META_IN_HAL, pMetadata);

    // InHalMeta-EIS region
    IMetadata::IEntry entry(MTK_EIS_REGION);
    entry.push_back(0, Type2Type< MINT32 >());
    entry.push_back(0, Type2Type< MINT32 >());
    entry.push_back(0, Type2Type< MINT32 >());
    entry.push_back(0, Type2Type< MINT32 >());
    entry.push_back(1920, Type2Type< MINT32 >());
    entry.push_back(1080, Type2Type< MINT32 >());
    // the following is  MVtoCenterX, MVtoCenterY, IsFromRRZ
    entry.push_back(0, Type2Type< MINT32 >());
    entry.push_back(0, Type2Type< MINT32 >());
    entry.push_back(MTRUE, Type2Type< MBOOL >());
    entry.push_back(0, Type2Type< MINT32 >());
    entry.push_back(0, Type2Type< MINT32 >());
    entry.push_back(0, Type2Type< MINT32 >());
    entry.push_back(0, Type2Type< MINT32 >());
    entry.push_back(0, Type2Type< MINT32 >());
    entry.push_back(0, Type2Type< MINT32 >());
    entry.push_back(0, Type2Type< MINT32 >());
    pMetadata->update(MTK_EIS_REGION, entry);

    ADD_METABUF_INPUT_FRAME(effectReq, BID_META_IN_HAL_MAIN2, pMetadata);

    // prepare output frame: MV_F
    frameInfo = new EffectFrameInfo(reqID, BID_OUT_MV_F);
    ImgParam imgParam_MainImage = getImgParam_MV_F();
    buf = createEmptyImageBuffer(imgParam_MainImage, "MainImg", eBUFFER_USAGE_HW_CAMERA_WRITE|eBUFFER_USAGE_SW_READ_OFTEN);
    frameInfo->setFrameBuffer(buf);
    effectReq->vOutputFrameInfo.add(BID_OUT_MV_F, frameInfo);
    // VR no FD
    MY_LOGD("-------eeeeeeeee------>%d %d %d", buf->getFD(0),buf->getFD(1), buf->getFD(2));
}

void prepareRequest_PV(sp<EffectRequest>& effectReq)
{
    DECLARE_MACRO();
    sp<IImageBuffer> buf_Main1, buf_Main2, buf;
    // load resize raw
    loadResizRawsImgBuf(buf_Main1, buf_Main2);
    //buf_Main1 = createImgBufUsingSample();
    //buf_Main2 = createImgBufUsingSample();


    MUINT32 reqID = effectReq->getRequestNo();
    // prepare input frame info: RSRAW1
    sp<EffectFrameInfo> frameInfo = new EffectFrameInfo(reqID, BID_P2AFM_IN_RSRAW1);
    frameInfo->setFrameBuffer(buf_Main1);
    effectReq->vInputFrameInfo.add(BID_P2AFM_IN_RSRAW1, frameInfo);

    // prepare input frame info: RSRAW2
    frameInfo = new EffectFrameInfo(reqID, BID_P2AFM_IN_RSRAW2);
    frameInfo->setFrameBuffer(buf_Main2);
    effectReq->vInputFrameInfo.add(BID_P2AFM_IN_RSRAW2, frameInfo);

    IMetadata* pMetadata;
    // prepare the metadata frame info
    ADD_METABUF_INPUT_FRAME(effectReq, BID_META_IN_APP, pMetadata);
    ADD_METABUF_INPUT_FRAME(effectReq, BID_META_IN_HAL, pMetadata);
    ADD_METABUF_INPUT_FRAME(effectReq, BID_META_IN_HAL_MAIN2, pMetadata);
    ADD_METABUF_INPUT_FRAME(effectReq, BID_META_IN_APP, pMetadata);

    // prepare output frame: MV_F
    frameInfo = new EffectFrameInfo(reqID, BID_OUT_MV_F);
    ImgParam imgParam_MainImage = getImgParam_MV_F();
    //buf = createEmptyImageBuffer(imgParam_MainImage, "MainImg", eBUFFER_USAGE_HW_CAMERA_WRITE|eBUFFER_USAGE_SW_READ_OFTEN);
    buf = createEmptyImageBuffer(imgParam_MainImage, "MainImg", eBUFFER_USAGE_SW_READ_OFTEN | eBUFFER_USAGE_SW_WRITE_OFTEN | eBUFFER_USAGE_HW_CAMERA_READWRITE |  eBUFFER_USAGE_SW_WRITE_RARELY);

    frameInfo->setFrameBuffer(buf);
    effectReq->vOutputFrameInfo.add(BID_OUT_MV_F, frameInfo);
    // prepare output frame: FD
    frameInfo = new EffectFrameInfo(reqID, BID_OUT_FD);
    ImgParam imgParam_FD = getImgParam_FD();
    buf = createEmptyImageBuffer(imgParam_FD, "FD", eBUFFER_USAGE_SW_READ_OFTEN | eBUFFER_USAGE_SW_WRITE_OFTEN | eBUFFER_USAGE_HW_CAMERA_READWRITE |  eBUFFER_USAGE_SW_WRITE_RARELY);
    frameInfo->setFrameBuffer(buf);
    effectReq->vOutputFrameInfo.add(BID_OUT_FD, frameInfo);

}

void prepareRequest_CAP(sp<EffectRequest>& effectReq)
{
    DECLARE_MACRO();
    MUINT32 reqID = effectReq->getRequestNo();
    // prepare input frame info: RSRAW1
    sp<EffectFrameInfo> frameInfo = new EffectFrameInfo(reqID, BID_P2AFM_IN_RSRAW1);
    sp<IImageBuffer> buf = createImgBufUsingSample();
    frameInfo->setFrameBuffer(buf);
    effectReq->vInputFrameInfo.add(BID_P2AFM_IN_RSRAW1, frameInfo);

    // prepare input frame info: RSRAW2
    frameInfo = new EffectFrameInfo(reqID, BID_P2AFM_IN_RSRAW2);
    buf = createImgBufUsingSample();
    frameInfo->setFrameBuffer(buf);
    effectReq->vInputFrameInfo.add(BID_P2AFM_IN_RSRAW2, frameInfo);

    // prepare input frame info: FSRAW1
    frameInfo = new EffectFrameInfo(reqID, BID_P2AFM_IN_FSRAW);
    buf = createImgBufUsingSample();
    frameInfo->setFrameBuffer(buf);
    effectReq->vInputFrameInfo.add(BID_P2AFM_IN_FSRAW, frameInfo);


    // prepare the metadata frame info
    IMetadata* pMetadata;
    ADD_METABUF_INPUT_FRAME(effectReq, BID_META_IN_APP, pMetadata);
    ADD_METABUF_INPUT_FRAME(effectReq, BID_META_IN_HAL, pMetadata);
    ADD_METABUF_INPUT_FRAME(effectReq, BID_META_IN_HAL_MAIN2, pMetadata);
    ADD_METABUF_INPUT_FRAME(effectReq, BID_META_IN_APP, pMetadata);

    // prepare output frame: MV_F_CAP
    frameInfo = new EffectFrameInfo(reqID, BID_OUT_MV_F_CAP);
    ImgParam imgParam_MainImage_CAP = getImgParam_MV_F_CAP();
    buf = createEmptyImageBuffer(imgParam_MainImage_CAP, "MainImg_CAP", eBUFFER_USAGE_HW_CAMERA_WRITE|eBUFFER_USAGE_SW_READ_OFTEN);
    frameInfo->setFrameBuffer(buf);
    effectReq->vOutputFrameInfo.add(BID_OUT_MV_F_CAP, frameInfo);
    // Capture use interal FD feaure pipe node
}

MVOID setupDumpProperties(P2AFMNode *pP2AFMNode)
{
    const char* name = pP2AFMNode->getName();

    property_set(name, "1");

    char prop_name[256];

    snprintf(prop_name, 256, "%s.%s", name, DepthMapDataHandler::ID2Name(P2AFM_TO_N3D_FEFM_CCin));
    property_set(prop_name, "1");
    snprintf(prop_name, 256, "%s.%s", name, DepthMapDataHandler::ID2Name(P2AFM_TO_FD_IMG));
    property_set(prop_name, "1");
    snprintf(prop_name, 256, "%s.%s", name, DepthMapDataHandler::ID2Name(P2AFM_TO_WMF_MY_SL));
    property_set(prop_name, "1");
    snprintf(prop_name, 256, "%s.%s", name, DepthMapDataHandler::ID2Name(P2AFM_OUT_MV_F));
    property_set(prop_name, "1");
    snprintf(prop_name, 256, "%s.%s", name, DepthMapDataHandler::ID2Name(P2AFM_OUT_FD));
    property_set(prop_name, "1");
    snprintf(prop_name, 256, "%s.%s", name, DepthMapDataHandler::ID2Name(P2AFM_OUT_MV_F_CAP));
    property_set(prop_name, "1");
#ifdef GTEST
    snprintf(prop_name, 256, "%s.%s", name, DepthMapDataHandler::ID2Name(UT_OUT_FE));
    property_set(prop_name, "1");
#endif

}


TEST(DepthMapPipe_P2AFMNode, Standard_VR)
//MVOID aaa()
{
    std::vector<sp<EffectRequest>> vP2AFMInputEffectReqVec;
    std::vector<MUINT32> vReqIDVec;

    for(int reqID=0;reqID<10;reqID++)
    {
        vReqIDVec.push_back(reqID);
        sp<EffectRequest> pEffectReq = new EffectRequest(reqID);
        sp<EffectParameter> pEffectParam = new EffectParameter();
        // ------------- test VR scenario ----------------
        pEffectParam->set(DEPTHMAP_REQUEST_STATE_KEY, (int)STATE_NORMAL);
        pEffectReq->setRequestParameter(pEffectParam);
        // prepare EffectRequest
        prepareRequest_VR(pEffectReq);
        vP2AFMInputEffectReqVec.push_back(pEffectReq);
    }

    P2AFMNode *pP2AFMNode = new P2AFMNode("P2AFM_UT", NULL, UT_MAIN1_SENSOR_IDX, UT_MAIN2_SENSOR_IDX);
    CallbackUTNode *pCallbackUTNode = new CallbackUTNode("CBUTNode", ut_finishCB);

    // set VR connection
    //P2AFM to N3D + P2AFM_output
    pP2AFMNode->connectData(P2AFM_TO_N3D_FEFM_CCin, P2AFM_TO_N3D_FEFM_CCin, pCallbackUTNode);
    pP2AFMNode->connectData(P2AFM_OUT_MV_F, P2AFM_OUT_MV_F, pCallbackUTNode);
    #ifdef GTEST
    pP2AFMNode->connectData(UT_OUT_FE, UT_OUT_FE, pCallbackUTNode);
    #endif

    // dump buffers
    setupDumpProperties(pP2AFMNode);

    vector<DepthMapDataID> vWaitDataID;
    vWaitDataID.push_back(P2AFM_TO_N3D_FEFM_CCin);
    vWaitDataID.push_back(P2AFM_OUT_MV_F);
    pCallbackUTNode->setWaitingDataIDAndReqID(vWaitDataID, vReqIDVec);

    // init/start nodes
    MY_LOGD("P2AFMNode init!!!!!!!!!!");
    pP2AFMNode->init();
    pCallbackUTNode->init();
    MY_LOGD("P2AFMNode start!!!!!!!!!!");
    pP2AFMNode->start();
    pCallbackUTNode->start();

    MY_LOGD("P2AFMNode handleData!!!!! !!!!!!!!!!!!!!");

    for(size_t index = 0 ; index<vP2AFMInputEffectReqVec.size();index++)
    {
        pP2AFMNode->handleUTData(ROOT_ENQUE, vP2AFMInputEffectReqVec[index]);
    }

    MY_LOGD("pP2AFMNode start to wait!!....");
    bool bRet = WaitingListener::waitRequest(vP2AFMInputEffectReqVec.size(), 8);
    MY_LOGD("Wait done!!....");
    EXPECT_TRUE(bRet);

    MY_LOGD("pCallbackUTNode  stop!!....");
    pCallbackUTNode->stop();
    MY_LOGD("pCallbackUTNode  uninit!!....");
    pCallbackUTNode->uninit();
    MY_LOGD("pP2AFMNode  stop!!....");
    pP2AFMNode->stop();
    MY_LOGD("pP2AFMNode  uninit!!....");
    pP2AFMNode->uninit();
    MY_LOGD("pP2AFMNode  uninit!!....done!");
    WaitingListener::resetCounter();
    MY_LOGD("delete nodes....!");
    delete pCallbackUTNode;
    delete pP2AFMNode;
    MY_LOGD("delete nodes....done!");

}


TEST(DepthMapPipe_P2AFMNode, Standard_PV)
//void aaa()
{
    std::vector<MUINT32> vReqIDVec;

    P2AFMNode *pP2AFMNode = new P2AFMNode("P2AFM_PV", NULL, UT_MAIN1_SENSOR_IDX, UT_MAIN2_SENSOR_IDX);

    // ReqID = 0
    sp<EffectRequest> pEffectReq = new EffectRequest(0);
    vReqIDVec.push_back(0);

    sp<EffectParameter> pEffectParam = new EffectParameter();
    pEffectParam->set(DEPTHMAP_REQUEST_STATE_KEY, (int)STATE_NORMAL);
    pEffectReq->setRequestParameter(pEffectParam);

    CallbackUTNode *pCallbackUTNode = new CallbackUTNode("CBUTNode", ut_finishCB);

    // set PV connection
    //P2AFM to N3D + P2AFM_output
    pP2AFMNode->connectData(P2AFM_TO_N3D_FEFM_CCin, P2AFM_TO_N3D_FEFM_CCin, pCallbackUTNode);
    pP2AFMNode->connectData(P2AFM_OUT_MV_F, P2AFM_OUT_MV_F, pCallbackUTNode);
    // FD
    pP2AFMNode->connectData(P2AFM_OUT_FD, P2AFM_OUT_FD, pCallbackUTNode);
    #ifdef GTEST
    pP2AFMNode->connectData(UT_OUT_FE, UT_OUT_FE, pCallbackUTNode);
    #endif
    // PV no CAP MV_F
    // pP2AFMNode->connectData(P2AFM_OUT_MV_F_CAP, P2AFM_OUT_MV_F_CAP, pCallbackUTNode);
    // PV no CAP FD
    // pP2AFMNode->connectData(P2AFM_TO_FD_IMG, P2AFM_TO_FD_IMG, pCallbackUTNode);

    vector<DepthMapDataID> vWaitDataID;
    vWaitDataID.push_back(P2AFM_TO_N3D_FEFM_CCin);
    vWaitDataID.push_back(P2AFM_OUT_MV_F);
    vWaitDataID.push_back(P2AFM_OUT_FD);

    pCallbackUTNode->setWaitingDataIDAndReqID(vWaitDataID, vReqIDVec);
    // prepare EffectRequest
    prepareRequest_PV(pEffectReq);

    // dump buffers
    setupDumpProperties(pP2AFMNode);

     MY_LOGD("start P2AFM PV UT: REQ type=%d", getRequestState(pEffectReq));

    // init/start nodes
    MY_LOGD("P2AFMNode init!!!!!!!!!!");
    pP2AFMNode->init();
    pCallbackUTNode->init();
    MY_LOGD("P2AFMNode start!!!!!!!!!!");
    pP2AFMNode->start();
    pCallbackUTNode->start();


    MY_LOGD("P2AFMNode handleData!!!!! !!!!!!!!!!!!!!");
    pP2AFMNode->handleUTData(ROOT_ENQUE, pEffectReq);

     MY_LOGD("pP2AFMNode start to wait!!....");
    bool bRet = WaitingListener::waitRequest(1, 5);
    MY_LOGD("Wait done!!....");
    EXPECT_TRUE(bRet);

    MY_LOGD("pCallbackUTNode  stop!!....");
    pCallbackUTNode->stop();
    MY_LOGD("pCallbackUTNode  uninit!!....");
    pCallbackUTNode->uninit();
    MY_LOGD("pP2AFMNode  stop!!....");
    pP2AFMNode->stop();
    MY_LOGD("pP2AFMNode  uninit!!....");
    pP2AFMNode->uninit();
    MY_LOGD("pP2AFMNode  uninit!!....done!");
    WaitingListener::resetCounter();
    MY_LOGD("delete nodes....!");
    delete pCallbackUTNode;
    delete pP2AFMNode;
    MY_LOGD("delete nodes....done!");

}

//TEST(DepthMapPipe_P2AFMNode, StandardCAP)
MVOID ttt2()
{
    std::vector<MUINT32> vReqIDVec;

    P2AFMNode *pP2AFMNode = new P2AFMNode("P2AFM_CAP", NULL, UT_MAIN1_SENSOR_IDX, UT_MAIN2_SENSOR_IDX);

    MUINT32 reqID=0;
    sp<EffectRequest> pEffectReq = new EffectRequest(reqID);
    vReqIDVec.push_back(reqID);

    sp<EffectParameter> pEffectParam = new EffectParameter();

    pEffectParam->set(DEPTHMAP_REQUEST_STATE_KEY, (int)STATE_CAPTURE);
    pEffectReq->setRequestParameter(pEffectParam);

    CallbackUTNode *pCallbackUTNode = new CallbackUTNode("CBUTNode", ut_finishCB);

    // set Capture connection
    //P2AFM to N3D + P2AFM_output
    pP2AFMNode->connectData(P2AFM_TO_N3D_FEFM_CCin, P2AFM_TO_N3D_FEFM_CCin, pCallbackUTNode);
    // CAP MV_F
    pP2AFMNode->connectData(P2AFM_OUT_MV_F_CAP, P2AFM_OUT_MV_F_CAP, pCallbackUTNode);
    // CAP FD
    pP2AFMNode->connectData(P2AFM_TO_FD_IMG, P2AFM_TO_FD_IMG, pCallbackUTNode);
    pP2AFMNode->connectData(P2AFM_TO_WMF_MY_SL, P2AFM_TO_WMF_MY_SL, pCallbackUTNode);



    vector<DepthMapDataID> vWaitDataID;
    vWaitDataID.push_back(P2AFM_TO_N3D_FEFM_CCin);
    vWaitDataID.push_back(P2AFM_OUT_MV_F_CAP);
    vWaitDataID.push_back(P2AFM_TO_FD_IMG);
    vWaitDataID.push_back(P2AFM_TO_WMF_MY_SL);
    pCallbackUTNode->setWaitingDataIDAndReqID(vWaitDataID, vReqIDVec);

    // prepare EffectRequest
    prepareRequest_CAP(pEffectReq);

    MY_LOGD("start P2AFM CAP UT: REQ type=%d", getRequestState(pEffectReq));

    // init/start nodes
    MY_LOGD("Node init!!!!!!!!!!");
    pP2AFMNode->init();
    pCallbackUTNode->init();
    MY_LOGD("Node start!!!!!!!!!!");
    pP2AFMNode->start();
    pCallbackUTNode->start();

    MY_LOGD("P2AFMNode handleData!!!!! !!!!!!!!!!!!!!");

    pP2AFMNode->handleUTData(ROOT_ENQUE, pEffectReq);

    bool bRet = WaitingListener::waitRequest(1,5);
    EXPECT_TRUE(bRet);

    if(bRet)
    {   // dump MV_F_CAP
        char filename[1023];
        sp<EffectFrameInfo> frame;
        sp<IImageBuffer> imgBuf;
        frame = pEffectReq->vOutputFrameInfo.valueFor(BID_OUT_MV_F_CAP);
        frame->getFrameBuffer(imgBuf);
        sprintf(filename, "/sdcard/Pictures/VSDOF/MV_F_%dx%d(CAP).yuv",
                imgBuf->getImgSize().w, imgBuf->getImgSize().h);
        imgBuf->saveToFile(filename);
    }

    pCallbackUTNode->stop();
    pCallbackUTNode->uninit();
    pP2AFMNode->stop();
    pP2AFMNode->uninit();
    WaitingListener::resetCounter();
    delete pCallbackUTNode;
    delete pP2AFMNode;

}

}
}
}



