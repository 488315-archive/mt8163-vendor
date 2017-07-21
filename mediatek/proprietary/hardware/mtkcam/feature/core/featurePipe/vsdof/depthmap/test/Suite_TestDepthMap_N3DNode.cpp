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
#include <Hal3/IHal3A.h>
#include <IImageBuffer.h>
#include <mmsdk/EffectRequest.h>
#include <utils/include/ImageBufferHeap.h>
#include <featurePipe/vsdof/depthmap/N3DNode.h>
#include <vsdof/hal/stereo_size_provider.h>
#include <featurePipe/vsdof/depthmap/DepthMapPipeNode.h>
#include <featurePipe/vsdof/depthmap/DepthMapPipe_Common.h>
#include <featurePipe/vsdof/depthmap/DepthMapPipeUtils.h>

#include <metadata/mtk_platform_metadata_tag.h>
#include <metadata/client/mtk_metadata_tag.h>

#include "CallbackUTNode.h"
#include "TestDepthMapPipe_Common.h"


using android::sp;
using namespace NSCam;
using namespace std;

namespace NSCam{
namespace NSCamFeature{
namespace NSFeaturePipe{

class P2ABufferSizeConfig
{
public:
    P2ABufferSizeConfig();
    virtual ~P2ABufferSizeConfig();

public:
    MSize P2AFM_FD_IMG_SIZE;
    MSize P2AFM_FEAO_INPUT_SIZE;
    MSize P2AFM_FE1BO_INPUT_SIZE;
    MSize P2AFM_FE2BO_INPUT_SIZE;
    MSize P2AFM_FE1CO_INPUT_SIZE;
    MSize P2AFM_FE2CO_INPUT_SIZE;
    MSize P2AFM_RECT_IN_VR_SIZE;
    MSize P2AFM_RECT_IN_PV_SIZE;
    MSize P2AFM_RECT_IN_CAP_SIZE;
    MSize P2AFM_RECT_IN_CAP_IMG_SIZE;
    MSize P2AFM_MAIN_IMAGE_SIZE;
    MSize P2AFM_MAIN_IMAGE_CAP_SIZE;
    MSize P2AFM_CCIN_SIZE;

    StereoArea P2AFM_MAIN2_FEAO_AREA;
    StereoArea P2AFM_MAIN2_FEBO_AREA;
    StereoArea P2AFM_MAIN2_FECO_AREA;
};

class P2ABufferPoolSet
{
public:
    P2ABufferPoolSet();
    virtual ~P2ABufferPoolSet();

    MBOOL init(const P2ABufferSizeConfig& config);
    MBOOL uninit();
public:
    // buffer pools
    android::sp<ImageBufferPool> mpFEOB_BufPool;
    android::sp<ImageBufferPool> mpFEOC_BufPool;
    android::sp<ImageBufferPool> mpFMOB_BufPool;
    android::sp<ImageBufferPool> mpFMOC_BufPool;

    android::sp<ImageBufferPool> mpRectInVRBufPool;
    android::sp<ImageBufferPool> mpRectInPVBufPool;
    android::sp<ImageBufferPool> mpRectInCapBufPool_Main1;
    android::sp<GraphicBufferPool> mpRectInCapBufPool_Gra_Main2;
    android::sp<ImageBufferPool> mpCCInBufPool;

private:
    MVOID queryFEOBufferSize(MSize iBufSize, MUINT iBlockSize, MUINT32 &riFEOWidth, MUINT32 &riFEOHeight);
    MVOID queryFMOBufferSize(MUINT32 iFEOWidth, MUINT32 iFEOHeight, MUINT32 &riFMOWidth, MUINT32 &riFMOHeight);
};



P2ABufferSizeConfig::
P2ABufferSizeConfig()
{
    StereoSizeProvider* pSizePrvder = StereoSizeProvider::getInstance();

    // PV/VR - frame 0
    Pass2SizeInfo pass2SizeInfo;
    pSizePrvder->getPass2SizeInfo(PASS2A_P, eSTEREO_SCENARIO_RECORD, pass2SizeInfo);
    P2AFM_FE2BO_INPUT_SIZE = pass2SizeInfo.szWROT;
    P2AFM_MAIN2_FEAO_AREA = pass2SizeInfo.areaFEO;
    P2AFM_FEAO_INPUT_SIZE = pass2SizeInfo.areaFEO.size;

    // frame 1
    pSizePrvder->getPass2SizeInfo(PASS2A, eSTEREO_SCENARIO_RECORD, pass2SizeInfo);
    P2AFM_FD_IMG_SIZE = pass2SizeInfo.szIMG2O;
    P2AFM_MAIN_IMAGE_SIZE = pass2SizeInfo.areaWDMA.size;
    P2AFM_FE1BO_INPUT_SIZE = pass2SizeInfo.szWROT;

    // frame 2
    pSizePrvder->getPass2SizeInfo(PASS2A_P_2, eSTEREO_SCENARIO_RECORD, pass2SizeInfo);
    P2AFM_FE2CO_INPUT_SIZE = pass2SizeInfo.szIMG2O;
    P2AFM_RECT_IN_VR_SIZE = pass2SizeInfo.areaWDMA.size;
    P2AFM_MAIN2_FEBO_AREA = pass2SizeInfo.areaFEO;

    // frame 3
    pSizePrvder->getPass2SizeInfo(PASS2A_2, eSTEREO_SCENARIO_RECORD, pass2SizeInfo);
    P2AFM_FE1CO_INPUT_SIZE = pass2SizeInfo.szIMG2O;

    // frame 4
    pSizePrvder->getPass2SizeInfo(PASS2A_P_3, eSTEREO_SCENARIO_RECORD, pass2SizeInfo);
    P2AFM_MAIN2_FECO_AREA = pass2SizeInfo.areaFEO;
    // cc_in
    P2AFM_CCIN_SIZE = pass2SizeInfo.szIMG2O;


    // PV - frame 2
    pSizePrvder->getPass2SizeInfo(PASS2A_P_2, eSTEREO_SCENARIO_PREVIEW, pass2SizeInfo);
    P2AFM_RECT_IN_PV_SIZE = pass2SizeInfo.areaWDMA.size;

    // CAP
    // frame 1
    pSizePrvder->getPass2SizeInfo(PASS2A, eSTEREO_SCENARIO_CAPTURE, pass2SizeInfo);
    P2AFM_MAIN_IMAGE_CAP_SIZE = pass2SizeInfo.areaWDMA.size;

    // frame 2
    pSizePrvder->getPass2SizeInfo(PASS2A_P_2, eSTEREO_SCENARIO_CAPTURE, pass2SizeInfo);
    P2AFM_RECT_IN_CAP_SIZE = pass2SizeInfo.areaWDMA.size;
    P2AFM_RECT_IN_CAP_IMG_SIZE = pass2SizeInfo.areaWDMA.size - pass2SizeInfo.areaWDMA.padding;


    MY_LOGD("P2AFM debug size======>\n");
    #define DEBUG_MSIZE(sizeCons) \
        MY_LOGD("size: " #sizeCons " : %dx%d\n", sizeCons.w, sizeCons.h);

    DEBUG_MSIZE( P2AFM_FD_IMG_SIZE);
    DEBUG_MSIZE( P2AFM_FEAO_INPUT_SIZE);
    DEBUG_MSIZE( P2AFM_FE1BO_INPUT_SIZE);
    DEBUG_MSIZE( P2AFM_FE2BO_INPUT_SIZE);
    DEBUG_MSIZE( P2AFM_FE1CO_INPUT_SIZE);
    DEBUG_MSIZE( P2AFM_FE2CO_INPUT_SIZE);
    DEBUG_MSIZE( P2AFM_RECT_IN_VR_SIZE);
    DEBUG_MSIZE( P2AFM_RECT_IN_PV_SIZE);
    DEBUG_MSIZE( P2AFM_RECT_IN_CAP_SIZE);
    DEBUG_MSIZE( P2AFM_RECT_IN_CAP_IMG_SIZE);
    DEBUG_MSIZE( P2AFM_MAIN_IMAGE_SIZE);
    DEBUG_MSIZE( P2AFM_MAIN_IMAGE_CAP_SIZE);
    DEBUG_MSIZE( P2AFM_CCIN_SIZE);

    #define DEBUG_AREA(areaCons) \
        MY_LOGD("Area: " #areaCons " : size=%dx%d padd=%dx%d\n", areaCons.size.w, areaCons.size.h, areaCons.padding.w, areaCons.padding.h);

    DEBUG_AREA(P2AFM_MAIN2_FEAO_AREA);
    DEBUG_AREA(P2AFM_MAIN2_FEBO_AREA);
    DEBUG_AREA(P2AFM_MAIN2_FECO_AREA);
}

P2ABufferSizeConfig::
~P2ABufferSizeConfig()
{}

P2ABufferPoolSet::
P2ABufferPoolSet()
{}

P2ABufferPoolSet::
~P2ABufferPoolSet()
{}

MVOID
P2ABufferPoolSet::
queryFEOBufferSize(MSize iBufSize, MUINT iBlockSize, MUINT32 &riFEOWidth, MUINT32 &riFEOHeight)
{

    riFEOWidth = iBufSize.w/iBlockSize*40;
    riFEOHeight = iBufSize.h/iBlockSize;

    MY_LOGD("queryFEOBufferSize~~~iBufSize=%dx%d  ouput=%dx%d", iBufSize.w, iBufSize.h, riFEOWidth, riFEOHeight);
}

MVOID
P2ABufferPoolSet::
queryFMOBufferSize(MUINT32 iFEOWidth, MUINT32 iFEOHeight, MUINT32 &riFMOWidth, MUINT32 &riFMOHeight)
{
    riFMOWidth = (iFEOWidth/40) * 2;
    riFMOHeight = iFEOHeight;

    MY_LOGD("queryFMOBufferSize~~~iFEOWidth=%d iFEOHeight=%d  ouput=%dx%d", iFEOWidth, iFEOHeight, riFMOWidth, riFMOHeight);
}

MBOOL
P2ABufferPoolSet::
init(const P2ABufferSizeConfig& config)
{
    //FE FM Buffer pool
    MUINT32 fe_width=0, fe_height=0;
    MUINT32 fm_width=0, fm_height=0;

    MUINT32 iBlockSize;

    if(VSDOF_CONST_FE_EXEC_TIMES== 2)
    {
        // stage 2 - FEFM
        MUINT32 iBlockSize = StereoSettingProvider::fefmBlockSize(1);

        queryFEOBufferSize(config.P2AFM_FE1BO_INPUT_SIZE, iBlockSize, fe_width, fe_height);
        queryFMOBufferSize(fe_width, fe_height, fm_width, fm_height);
        CREATE_IMGBUF_POOL(mpFMOB_BufPool, "FMB_BufPool", MSize(fm_width, fm_height),
                            eImgFmt_STA_BYTE, ImageBufferPool::USAGE_HW, MFALSE);
        CREATE_IMGBUF_POOL(mpFEOB_BufPool, "FEB_BufPoll", MSize(fe_width, fe_height),
                            eImgFmt_STA_BYTE, ImageBufferPool::USAGE_HW, MFALSE);
        // stage 3 - FEFM
        iBlockSize = StereoSettingProvider::fefmBlockSize(2);

        queryFEOBufferSize(config.P2AFM_FE1CO_INPUT_SIZE, iBlockSize, fe_width, fe_height);
        queryFMOBufferSize(fe_width, fe_height, fm_width, fm_height);
        CREATE_IMGBUF_POOL(mpFMOC_BufPool, "FMC_BufPool", MSize(fm_width, fm_height), eImgFmt_STA_BYTE, ImageBufferPool::USAGE_HW, MFALSE);
        CREATE_IMGBUF_POOL(mpFEOC_BufPool, "FEC_BufPool", MSize(fe_width, fe_height), eImgFmt_STA_BYTE, ImageBufferPool::USAGE_HW, MFALSE);
    }
    else
    {
        MY_LOGE("VSDOF_CONST_FE_EXEC_TIMES const error! val = %d", VSDOF_CONST_FE_EXEC_TIMES);
        return MFALSE;
    }


    android::sp<ImageBufferPool> FE_POOLS[2] = {mpFEOC_BufPool, mpFEOB_BufPool};
    android::sp<ImageBufferPool> FM_POOLS[2] = {mpFMOC_BufPool, mpFMOB_BufPool};

    for (MUINT8 index=0; index<VSDOF_CONST_FE_EXEC_TIMES;++index)
    {
        android::sp<ImageBufferPool> fe_pool = FE_POOLS[index];
        android::sp<ImageBufferPool> fm_pool = FM_POOLS[index];
        // Main1+Main2 -->　2 * VSDOF_WORKING_BUF_SET
        ALLOCATE_BUFFER_POOL(fe_pool, 2*VSDOF_WORKING_BUF_SET);
        ALLOCATE_BUFFER_POOL(fm_pool, 2*VSDOF_WORKING_BUF_SET);
    }


    // VR: Rect_in1 + Rect_in2
    CREATE_IMGBUF_POOL(mpRectInVRBufPool, "RectInVR_BufPool", config.P2AFM_RECT_IN_VR_SIZE, eImgFmt_YV12, ImageBufferPool::USAGE_HW|eBUFFER_USAGE_SW_WRITE_OFTEN, MTRUE);
    // Capture
    // Rect_in1 : normal buffer
    CREATE_IMGBUF_POOL(mpRectInCapBufPool_Main1, "RectInCap_BufPool", config.P2AFM_RECT_IN_CAP_SIZE, eImgFmt_YV12, ImageBufferPool::USAGE_HW|eBUFFER_USAGE_SW_WRITE_OFTEN, MTRUE);
    // Rect_in2 : graphic buffer
    mpRectInCapBufPool_Gra_Main2 = GraphicBufferPool::create("RectInGra_BufPool", config.P2AFM_RECT_IN_CAP_SIZE.w,
                                            config.P2AFM_RECT_IN_CAP_SIZE.h, eImgFmt_YV12, ImageBufferPool::USAGE_HW|eBUFFER_USAGE_SW_WRITE_OFTEN);
    // PV: Rect_in1 + Rect_in2
    CREATE_IMGBUF_POOL(mpRectInPVBufPool, "RectInPV_BufPool", config.P2AFM_RECT_IN_PV_SIZE, eImgFmt_YV12, ImageBufferPool::USAGE_HW|eBUFFER_USAGE_SW_WRITE_OFTEN, MTRUE);
    // CC_in
    CREATE_IMGBUF_POOL(mpCCInBufPool, "CCin_BufPool", config.P2AFM_CCIN_SIZE, eImgFmt_Y8, ImageBufferPool::USAGE_HW|eBUFFER_USAGE_SW_WRITE_OFTEN, MFALSE);

    //Main1+Main2 --> 2 sets
    ALLOCATE_BUFFER_POOL(mpRectInVRBufPool, 2*VSDOF_WORKING_BUF_SET);
    ALLOCATE_BUFFER_POOL(mpRectInPVBufPool, 2*VSDOF_WORKING_BUF_SET);
    // cc_in --> 2 sets
    ALLOCATE_BUFFER_POOL(mpCCInBufPool, 2*VSDOF_WORKING_BUF_SET);
    // Rect_in
    ALLOCATE_BUFFER_POOL(mpRectInCapBufPool_Main1, VSDOF_WORKING_BUF_SET);
    ALLOCATE_BUFFER_POOL(mpRectInCapBufPool_Gra_Main2, VSDOF_WORKING_BUF_SET);
    return MTRUE;
}

MBOOL
P2ABufferPoolSet::
uninit()
{
    ImageBufferPool::destroy(mpFEOB_BufPool);
    ImageBufferPool::destroy(mpFEOC_BufPool);
    ImageBufferPool::destroy(mpFMOB_BufPool);
    ImageBufferPool::destroy(mpFMOC_BufPool);
    ImageBufferPool::destroy(mpRectInVRBufPool);
    ImageBufferPool::destroy(mpRectInPVBufPool);
    ImageBufferPool::destroy(mpRectInCapBufPool_Main1);
    GraphicBufferPool::destroy(mpRectInCapBufPool_Gra_Main2);
    ImageBufferPool::destroy(mpCCInBufPool);
    return MTRUE;
}


static P2ABufferSizeConfig P2ABufConfig;
static P2ABufferPoolSet P2ABufPoolSet;

static MVOID setupEnvironment()
{
    P2ABufPoolSet.init(P2ABufConfig);

    //======== Power on sensor ==========
    NSCam::IHalSensorList* pHalSensorList = NSCam::IHalSensorList::get();
    pHalSensorList->searchSensors();
    int sensorCount = pHalSensorList->queryNumberOfSensors();

    int main1Idx, main2Idx;
    StereoSettingProvider::getStereoSensorIndex(main1Idx, main2Idx);
    MUINT pIndex[2] = { (MUINT)main1Idx, (MUINT)main2Idx };
    MUINT const main1Index = pIndex[0];
    MUINT const main2Index = pIndex[1];
    if(!pHalSensorList)
    {
        MY_LOGE("pHalSensorList == NULL");
    }
    //
    #define USER_NAME "N3DNodeUT"
    IHalSensor *pHalSensor = pHalSensorList->createSensor(
                                            USER_NAME,
                                            2,
                                            pIndex);
    if(pHalSensor == NULL)
    {
       MY_LOGE("pHalSensor is NULL");
    }
    // In stereo mode, Main1 needs power on first.
    // Power on main1 and main2 successively one after another.
    if( !pHalSensor->powerOn(USER_NAME, 1, &main1Index) )
    {
        MY_LOGE("sensor power on failed: %d", main1Index);
    }
    if( !pHalSensor->powerOn(USER_NAME, 1, &main2Index) )
    {
        MY_LOGE("sensor power on failed: %d", main2Index);
    }

}



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



#define USE_REAL_DATA

MVOID prepareN3DInputImgInfoMap(ImgInfoMapPtr& pToN3DImgInfo, sp<EffectRequest> &pEffReq)
{
    MY_LOGD("+");

#ifndef USE_REAL_DATA
    const char* sFE1BO_FN = "/sdcard/vsdof/N3D/fe_right0_0.bin";
    const char* sFE2BO_FN = "/sdcard/vsdof/N3D/fe_left0_0.bin";
    const char* sFE1CO_FN = "/sdcard/vsdof/N3D/fe_right1_0.bin";
    const char* sFE2CO_FN = "/sdcard/vsdof/N3D/fe_left1_0.bin";
    const char* sFMBO_LR_FN = "/sdcard/vsdof/N3D/fm_ltor0_0.bin";
    const char* sFMBO_RL_FN = "/sdcard/vsdof/N3D/fm_rtol0_0.bin";
    const char* sFMCO_LR_FN = "/sdcard/vsdof/N3D/fm_ltor1_0.bin";
    const char* sFMCO_RL_FN = "/sdcard/vsdof/N3D/fm_rtol1_0.bin";

    const char* sCCin1_FN = "/sdcard/vsdof/N3D/pho_main_0.y";
    const char* sCCin2_FN = "/sdcard/vsdof/N3D/pho_auxi_0.y";
    const char* sRectIn1_FN = "/sdcard/vsdof/N3D/Warp_main_src_0.yuv";
    const char* sRectIn2_FN = "/sdcard/vsdof/N3D/Warp_auxi_src_0.yuv";
#else
    const char* sFE1BO_FN = "/sdcard/vsdof/N3D/BID_TO_N3D_FE1BO_2400x34.yuv";
    const char* sFE2BO_FN = "/sdcard/vsdof/N3D/BID_TO_N3D_FE2BO_2400x34.yuv";
    const char* sFE1CO_FN = "/sdcard/vsdof/N3D/BID_TO_N3D_FE1CO_2400x34.yuv";
    const char* sFE2CO_FN = "/sdcard/vsdof/N3D/BID_TO_N3D_FE2CO_2400x34.yuv";
    const char* sFMBO_LR_FN = "/sdcard/vsdof/N3D/BID_TO_N3D_FMBO_LR_120x34.yuv";
    const char* sFMBO_RL_FN = "/sdcard/vsdof/N3D/BID_TO_N3D_FMBO_RL_120x34.yuv";
    const char* sFMCO_LR_FN = "/sdcard/vsdof/N3D/BID_TO_N3D_FMCO_LR_120x34.yuv";
    const char* sFMCO_RL_FN = "/sdcard/vsdof/N3D/BID_TO_N3D_FMCO_RL_120x34.yuv";

    const char* sCCin1_FN = "/sdcard/vsdof/N3D/BID_TO_N3D_CC_IN1_128x72.yuv";
    const char* sCCin2_FN = "/sdcard/vsdof/N3D/BID_TO_N3D_CC_IN2_128x72.yuv";
    const char* sRectIn1_FN = "/sdcard/vsdof/N3D/BID_TO_N3D_RECT_IN1_240x136.yuv";
    const char* sRectIn2_FN = "/sdcard/vsdof/N3D/BID_TO_N3D_RECT_IN2_240x136.yuv";
#endif

    SmartImageBuffer sm_FE1BO = P2ABufPoolSet.mpFEOB_BufPool->request();
    LOAD_SMART_IMAGE_BUFFER(sm_FE1BO, "FEB_BufPoll", sFE1BO_FN, ImageBufferPool::USAGE_HW|eBUFFER_USAGE_SW_WRITE_OFTEN);
    SmartImageBuffer sm_FE2BO = P2ABufPoolSet.mpFEOB_BufPool->request();
    LOAD_SMART_IMAGE_BUFFER(sm_FE2BO, "FEB_BufPoll", sFE2BO_FN, ImageBufferPool::USAGE_HW|eBUFFER_USAGE_SW_WRITE_OFTEN);

    SmartImageBuffer sm_FE1CO = P2ABufPoolSet.mpFEOC_BufPool->request();
    LOAD_SMART_IMAGE_BUFFER(sm_FE1CO, "FEC_BufPoll", sFE1CO_FN, ImageBufferPool::USAGE_HW|eBUFFER_USAGE_SW_WRITE_OFTEN);
    SmartImageBuffer sm_FE2CO = P2ABufPoolSet.mpFEOC_BufPool->request();
    LOAD_SMART_IMAGE_BUFFER(sm_FE2CO, "FEC_BufPool", sFE2CO_FN, ImageBufferPool::USAGE_HW|eBUFFER_USAGE_SW_WRITE_OFTEN);

    SmartImageBuffer sm_FMBO_LR = P2ABufPoolSet.mpFMOB_BufPool->request();
    LOAD_SMART_IMAGE_BUFFER(sm_FMBO_LR, "FMB_BufPool", sFMBO_LR_FN, ImageBufferPool::USAGE_HW|eBUFFER_USAGE_SW_WRITE_OFTEN);
    SmartImageBuffer sm_FMBO_RL = P2ABufPoolSet.mpFMOB_BufPool->request();
    LOAD_SMART_IMAGE_BUFFER(sm_FMBO_RL, "FMB_BufPool", sFMBO_RL_FN, ImageBufferPool::USAGE_HW|eBUFFER_USAGE_SW_WRITE_OFTEN);

    SmartImageBuffer sm_FMCO_LR = P2ABufPoolSet.mpFMOC_BufPool->request();
    LOAD_SMART_IMAGE_BUFFER(sm_FMCO_LR, "FMC_BufPool", sFMCO_LR_FN, ImageBufferPool::USAGE_HW|eBUFFER_USAGE_SW_WRITE_OFTEN);
    SmartImageBuffer sm_FMCO_RL = P2ABufPoolSet.mpFMOC_BufPool->request();
    LOAD_SMART_IMAGE_BUFFER(sm_FMCO_RL, "FMC_BufPool", sFMCO_RL_FN, ImageBufferPool::USAGE_HW|eBUFFER_USAGE_SW_WRITE_OFTEN);


    SmartImageBuffer sm_CCin1 = P2ABufPoolSet.mpCCInBufPool->request();
    LOAD_SMART_IMAGE_BUFFER(sm_CCin1, "CCin_BufPool", sCCin1_FN, ImageBufferPool::USAGE_HW|eBUFFER_USAGE_SW_WRITE_OFTEN);

    SmartImageBuffer sm_CCin2 = P2ABufPoolSet.mpCCInBufPool->request();
    LOAD_SMART_IMAGE_BUFFER(sm_CCin2, "CCin_BufPool", sCCin2_FN, ImageBufferPool::USAGE_HW|eBUFFER_USAGE_SW_WRITE_OFTEN);

    SmartImageBuffer sm_RectIn1, sm_RectIn2;
    sm_RectIn1 = P2ABufPoolSet.mpRectInPVBufPool->request();
    LOAD_SMART_IMAGE_BUFFER(sm_RectIn1, "RectInPV_BufPool", sRectIn1_FN, ImageBufferPool::USAGE_HW|eBUFFER_USAGE_SW_WRITE_OFTEN);
    sm_RectIn2 = P2ABufPoolSet.mpRectInPVBufPool->request();
    LOAD_SMART_IMAGE_BUFFER(sm_RectIn2, "RectInPV_BufPool", sRectIn2_FN, ImageBufferPool::USAGE_HW|eBUFFER_USAGE_SW_WRITE_OFTEN);


    pToN3DImgInfo->addImageBuffer(BID_TO_N3D_FE1BO, sm_FE1BO);
    pToN3DImgInfo->addImageBuffer(BID_TO_N3D_FE2BO, sm_FE2BO);
    pToN3DImgInfo->addImageBuffer(BID_TO_N3D_FE1CO, sm_FE1CO);
    pToN3DImgInfo->addImageBuffer(BID_TO_N3D_FE2CO, sm_FE2CO);
    pToN3DImgInfo->addImageBuffer(BID_TO_N3D_FMBO_LR, sm_FMBO_LR);
    pToN3DImgInfo->addImageBuffer(BID_TO_N3D_FMBO_RL, sm_FMBO_RL);
    pToN3DImgInfo->addImageBuffer(BID_TO_N3D_FMCO_LR, sm_FMCO_LR);
    pToN3DImgInfo->addImageBuffer(BID_TO_N3D_FMCO_RL, sm_FMCO_RL);

    pToN3DImgInfo->addImageBuffer(BID_TO_N3D_CC_IN1, sm_CCin1);
    pToN3DImgInfo->addImageBuffer(BID_TO_N3D_CC_IN2, sm_CCin2);
    pToN3DImgInfo->addImageBuffer(BID_TO_N3D_RECT_IN1, sm_RectIn1);
    pToN3DImgInfo->addImageBuffer(BID_TO_N3D_RECT_IN2, sm_RectIn2);

    IMetadata* pMetadata;
    MUINT32 reqIdx;
    sp<EffectFrameInfo> effectFrame;
    // prepare the metadata frame info
    // InAppMeta
    ADD_METABUF_INPUT_FRAME(pEffReq, BID_META_IN_APP, pMetadata);
    // InAppMeta - EIS off
    trySetMetadata<MUINT8>(pMetadata, MTK_CONTROL_VIDEO_STABILIZATION_MODE, MTK_CONTROL_VIDEO_STABILIZATION_MODE_OFF);
    // InHalMeta
    ADD_METABUF_INPUT_FRAME(pEffReq, BID_META_IN_HAL, pMetadata);

    MY_LOGD("-");
}

MVOID setupDumpProperties(N3DNode *pN3DNode)
{
    const char* name = pN3DNode->getName();

    property_set(name, "1");

    char prop_name[256];

    snprintf(prop_name, 256, "%s.%s", name, DepthMapDataHandler::ID2Name(N3D_TO_DPE_MVSV_MASK));
    property_set(prop_name, "1");
    snprintf(prop_name, 256, "%s.%s", name, DepthMapDataHandler::ID2Name(N3D_TO_OCC_LDC));
    property_set(prop_name, "1");
    snprintf(prop_name, 256, "%s.%s", name, DepthMapDataHandler::ID2Name(N3D_TO_FD_EXTDATA_MASK));
    property_set(prop_name, "1");
}

TEST(DepthMapPipe_N3DNode, Standard)
{
    std::vector<MUINT32> vReqIDVec;
    setupEnvironment();

    MUINT32 reqID=0;
    sp<EffectRequest> pEffectReq = new EffectRequest(reqID);
    vReqIDVec.push_back(reqID);

    sp<EffectParameter> pEffectParam = new EffectParameter();
    ImgInfoMapPtr pToN3DImgInfo = new ImageBufInfoMap(pEffectReq);
    // ------------- test PREView scenario (should be no differences in scenarios)----------------
    DepthNodeOpState state = STATE_NORMAL;
    pEffectParam->set(DEPTHMAP_REQUEST_STATE_KEY, (int)state);
    pEffectReq->setRequestParameter(pEffectParam);

    // prepare input
    prepareN3DInputImgInfoMap(pToN3DImgInfo, pEffectReq);
    N3DNode* pN3DNode = new N3DNode("N3DNode_UT", NULL);
    CallbackUTNode *pCallbackUTNode = new CallbackUTNode("CBUTNode", ut_finishCB);
    pN3DNode->connectData(N3D_TO_DPE_MVSV_MASK, N3D_TO_DPE_MVSV_MASK, pCallbackUTNode);
    pN3DNode->connectData(N3D_TO_OCC_LDC, N3D_TO_OCC_LDC, pCallbackUTNode);
    pN3DNode->connectData(N3D_OUT_JPS, N3D_OUT_JPS, pCallbackUTNode);
    pN3DNode->connectData(N3D_TO_FD_EXTDATA_MASK, N3D_TO_FD_EXTDATA_MASK, pCallbackUTNode);

    // VR only waits for N3D_TO_DPE_MVSV_MASK/N3D_TO_OCC_LDC
    vector<DepthMapDataID> vWaitDataID;
    vWaitDataID.push_back(N3D_TO_DPE_MVSV_MASK);
    vWaitDataID.push_back(N3D_TO_OCC_LDC);
    pCallbackUTNode->setWaitingDataIDAndReqID(vWaitDataID, vReqIDVec);

    // dump buffers
    setupDumpProperties(pN3DNode);

    // init/start nodes
    MY_LOGD("pN3DNode init!!!!!!!!!!");
    pN3DNode->init();
    pCallbackUTNode->init();
    MY_LOGD("pN3DNode start!!!!!!!!!!");
    pN3DNode->start();
    pCallbackUTNode->start();
    MY_LOGD("pN3DNode handleData!!!!! !!!!!!!!!!!!!!");
    // pass data to N3DNode
    pN3DNode->handleUTData(P2AFM_TO_N3D_FEFM_CCin, pToN3DImgInfo);
    MY_LOGD("pN3DNode start to wait!!....");
    bool bRet = WaitingListener::waitRequest(1, 5);
    MY_LOGD("pN3DNode done!!....");
    EXPECT_TRUE(bRet);

    pCallbackUTNode->stop();
    pCallbackUTNode->uninit();
    pN3DNode->stop();
    pN3DNode->uninit();
    WaitingListener::resetCounter();
    delete pCallbackUTNode;
    delete pN3DNode;

}



}
}
}