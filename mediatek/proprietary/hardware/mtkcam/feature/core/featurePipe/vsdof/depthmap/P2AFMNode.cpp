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

#include "P2AFMNode.h"
#include "DepthMapPipe_Common.h"

#include <IHalSensor.h>
#include <aaa_hal_common.h>
#include <libion_mtk/include/ion.h>
#include <metadata/mtk_platform_metadata_tag.h>
#include <iopipe/PostProc/IFeatureStream.h>
#include <vsdof/hal/stereo_tuning_provider.h>

#define PIPE_CLASS_TAG "P2A_FMNode"
#include <featurePipe/core/include/PipeLog.h>


// FM frame 6~9, template 0~3
#define GET_FM_STAGE(FRAME_ID) FRAME_ID-6

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {

P2AFMNode::BufferSizeConfig::
BufferSizeConfig()
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

    debug();

}


MVOID
P2AFMNode::BufferSizeConfig::debug() const
{
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
        MY_LOGD("Area: " #areaCons " : size=%dx%d padd=%dx%d startPt=(%d, %d)\n", areaCons.size.w, areaCons.size.h, areaCons.padding.w, areaCons.padding.h, areaCons.startPt.x, areaCons.startPt.x);

    DEBUG_AREA(P2AFM_MAIN2_FEAO_AREA);
    DEBUG_AREA(P2AFM_MAIN2_FEBO_AREA);
    DEBUG_AREA(P2AFM_MAIN2_FECO_AREA);

    MY_LOGD("StereoSettingProvider::fefmBlockSize(1)=%d StereoSettingProvider::fefmBlockSize(2)=%d",
                StereoSettingProvider::fefmBlockSize(1), StereoSettingProvider::fefmBlockSize(2));


    MY_LOGD("StereoSettingProvider::getStereoCameraFOVRatio=%f",StereoSettingProvider::getStereoCameraFOVRatio());
}

P2AFMNode::BufferSizeConfig::
~BufferSizeConfig()
{}

P2AFMNode::BufferPoolSet::
BufferPoolSet()
{}

P2AFMNode::BufferPoolSet::
~BufferPoolSet()
{}

MVOID
P2AFMNode::BufferPoolSet::
queryFEOBufferSize(MSize iBufSize, MUINT iBlockSize, MUINT32 &riFEOWidth, MUINT32 &riFEOHeight)
{
    riFEOWidth = iBufSize.w/iBlockSize*40;
    riFEOHeight = iBufSize.h/iBlockSize;
    MY_LOGD("queryFEOBufferSize: iBufSize=%dx%d  ouput=%dx%d", iBufSize.w, iBufSize.h, riFEOWidth, riFEOHeight);
}

MVOID
P2AFMNode::BufferPoolSet::
queryFMOBufferSize(MUINT32 iFEOWidth, MUINT32 iFEOHeight, MUINT32 &riFMOWidth, MUINT32 &riFMOHeight)
{
    riFMOWidth = (iFEOWidth/40) * 2;
    riFMOHeight = iFEOHeight;

    MY_LOGD("queryFMOBufferSize: iFEOWidth=%d iFEOHeight=%d  ouput=%dx%d", iFEOWidth, iFEOHeight, riFMOWidth, riFMOHeight);
}

MBOOL
P2AFMNode::BufferPoolSet::
init(const BufferSizeConfig& config)
{
    MY_LOGD("+ : BufferPoolSet init ");
    //FE FM Buffer pool
    MUINT32 fe_width=0, fe_height=0;
    MUINT32 fm_width=0, fm_height=0;

    MUINT32 iBlockSize;

    if(VSDOF_CONST_FE_EXEC_TIMES== 2)
    {
        // stage 2 - FEFM
        MUINT32 iBlockSize = StereoSettingProvider::fefmBlockSize(1);
        // calculate the buffers which is the input for FE HW, use Main1 version/because Main2 has SRC CROP
        MSize szFEBufSize = config.P2AFM_FE1BO_INPUT_SIZE;
        queryFEOBufferSize(szFEBufSize, iBlockSize, fe_width, fe_height);
        queryFMOBufferSize(fe_width, fe_height, fm_width, fm_height);
        // create image buffer pool
        CREATE_IMGBUF_POOL(mpFMOB_BufPool, "FMB_BufPool", MSize(fm_width, fm_height),
                            eImgFmt_STA_BYTE, ImageBufferPool::USAGE_HW, MTRUE);
        CREATE_IMGBUF_POOL(mpFEOB_BufPool, "FEB_BufPoll", MSize(fe_width, fe_height),
                            eImgFmt_STA_BYTE, ImageBufferPool::USAGE_HW, MTRUE);
        // stage 3 - FEFM
        iBlockSize = StereoSettingProvider::fefmBlockSize(2);
        // calculate the buffers which is the input for FE HW, use Main1 version/because Main2 has SRC CROP
        szFEBufSize = config.P2AFM_FE1CO_INPUT_SIZE;
        queryFEOBufferSize(szFEBufSize, iBlockSize, fe_width, fe_height);
        queryFMOBufferSize(fe_width, fe_height, fm_width, fm_height);
        CREATE_IMGBUF_POOL(mpFMOC_BufPool, "FMC_BufPool", MSize(fm_width, fm_height), eImgFmt_STA_BYTE, ImageBufferPool::USAGE_HW, MTRUE);
        CREATE_IMGBUF_POOL(mpFEOC_BufPool, "FEC_BufPool", MSize(fe_width, fe_height), eImgFmt_STA_BYTE, ImageBufferPool::USAGE_HW, MTRUE);
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
    CREATE_IMGBUF_POOL(mpRectInVRBufPool, "RectInVR_BufPool", config.P2AFM_RECT_IN_VR_SIZE, eImgFmt_YV12, ImageBufferPool::USAGE_HW, MTRUE);
    // Capture
    // Rect_in1 : normal buffer
    CREATE_IMGBUF_POOL(mpRectInCapBufPool_Main1, "RectInCap_BufPool", config.P2AFM_RECT_IN_CAP_SIZE, eImgFmt_YV12, ImageBufferPool::USAGE_HW, MTRUE);
    // Rect_in2 : graphic buffer
    mpRectInCapBufPool_Gra_Main2 = GraphicBufferPool::create("RectInGra_BufPool", config.P2AFM_RECT_IN_CAP_SIZE.w,
                                            config.P2AFM_RECT_IN_CAP_SIZE.h, eImgFmt_YV12, ImageBufferPool::USAGE_HW);
    // PV: Rect_in1 + Rect_in2
    CREATE_IMGBUF_POOL(mpRectInPVBufPool, "RectInPV_BufPool", config.P2AFM_RECT_IN_PV_SIZE, eImgFmt_YV12, ImageBufferPool::USAGE_HW, MTRUE);

    //FEBO_Main1 input
    CREATE_IMGBUF_POOL(mpFEBOInBufPool_Main1, "FE1BO_InputBufPool", config.P2AFM_FE1BO_INPUT_SIZE, eImgFmt_YV12, ImageBufferPool::USAGE_HW, MFALSE);

    //FEBO_Main2 input
    CREATE_IMGBUF_POOL(mpFEBOInBufPool_Main2, "FE2BO_InputBufPool", config.P2AFM_FE2BO_INPUT_SIZE, eImgFmt_YV12, ImageBufferPool::USAGE_HW, MFALSE);

    //FECO_Main1 input
    CREATE_IMGBUF_POOL(mpFECOInBufPool_Main1, "FE1CO_InputBufPool", config.P2AFM_FE1CO_INPUT_SIZE, eImgFmt_YUY2, ImageBufferPool::USAGE_HW, MFALSE);

    //FECO_Main2 input
    CREATE_IMGBUF_POOL(mpFECOInBufPool_Main2, "FE2CO_InputBufPool", config.P2AFM_FE2CO_INPUT_SIZE, eImgFmt_YUY2, ImageBufferPool::USAGE_HW, MFALSE);

    // CC_in
    CREATE_IMGBUF_POOL(mpCCInBufPool, "CCin_BufPool", config.P2AFM_CCIN_SIZE, eImgFmt_Y8, ImageBufferPool::USAGE_HW, MTRUE);

    // FD when Capture
    CREATE_IMGBUF_POOL(mpFDBufPool, "FDBufPool", config.P2AFM_FD_IMG_SIZE, eImgFmt_YUY2, ImageBufferPool::USAGE_HW, MTRUE);

    #ifdef GTEST
    // FEInput For UT
    CREATE_IMGBUF_POOL(mFEInput_Stage2_Main1, "mFEInput_Stage2_Main1", config.P2AFM_FE1BO_INPUT_SIZE, eImgFmt_YV12, ImageBufferPool::USAGE_HW, MFALSE);
    CREATE_IMGBUF_POOL(mFEInput_Stage2_Main2, "mFEInput_Stage2_Main2", config.P2AFM_MAIN2_FEBO_AREA.size, eImgFmt_YV12, ImageBufferPool::USAGE_HW, MFALSE);
    CREATE_IMGBUF_POOL(mFEInput_Stage3_Main1, "mFEInput_Stage3_Main1", config.P2AFM_FE1CO_INPUT_SIZE, eImgFmt_YV12, ImageBufferPool::USAGE_HW, MFALSE);
    CREATE_IMGBUF_POOL(mFEInput_Stage3_Main2, "mFEInput_Stage3_Main2", config.P2AFM_MAIN2_FECO_AREA.size, eImgFmt_YV12, ImageBufferPool::USAGE_HW, MFALSE);
    ALLOCATE_BUFFER_POOL(mFEInput_Stage2_Main1, VSDOF_WORKING_BUF_SET);
    ALLOCATE_BUFFER_POOL(mFEInput_Stage2_Main2, VSDOF_WORKING_BUF_SET);
    ALLOCATE_BUFFER_POOL(mFEInput_Stage3_Main1, VSDOF_WORKING_BUF_SET);
    ALLOCATE_BUFFER_POOL(mFEInput_Stage3_Main2, VSDOF_WORKING_BUF_SET);
    #endif


    //Main1+Main2 --> 2 sets
    ALLOCATE_BUFFER_POOL(mpRectInVRBufPool, 3*VSDOF_WORKING_BUF_SET);
    ALLOCATE_BUFFER_POOL(mpRectInPVBufPool, 3*VSDOF_WORKING_BUF_SET);
    ALLOCATE_BUFFER_POOL(mpFEBOInBufPool_Main1, 2);
    ALLOCATE_BUFFER_POOL(mpFEBOInBufPool_Main2, 2);
    ALLOCATE_BUFFER_POOL(mpFECOInBufPool_Main1, 2);
    ALLOCATE_BUFFER_POOL(mpFECOInBufPool_Main2, 2);


    // cc_in --> 2 sets
    ALLOCATE_BUFFER_POOL(mpCCInBufPool, 3*VSDOF_WORKING_BUF_SET);
    // Rect_in
    ALLOCATE_BUFFER_POOL(mpRectInCapBufPool_Main1, VSDOF_WORKING_BUF_SET);
    ALLOCATE_BUFFER_POOL(mpRectInCapBufPool_Gra_Main2, VSDOF_WORKING_BUF_SET);
    // FD
    ALLOCATE_BUFFER_POOL(mpFDBufPool, VSDOF_WORKING_BUF_SET + 1);
    // TuningBufferPool creation
    //CREATE_IMGBUF_POOL(mpTuningBufferPool, "VSDOF_TuningBuf_P2A", MSize(sizeof(dip_x_reg_t), 1), eImgFmt_BLOB, ImageBufferPool::USAGE_SW, MFALSE);
    mpTuningBufferPool = TuningBufferPool::create("VSDOF_TUNING_P2A", sizeof(dip_x_reg_t));
    ALLOCATE_BUFFER_POOL(mpTuningBufferPool, 3 * VSDOF_WORKING_BUF_SET + 4 );


    MY_LOGD("mpFEOC_BufPool=%x mpRectInVRBufPool=%x mpFDBufPool=%x mpTuningBufferPool=%x",
    mpFEOC_BufPool.get(), mpRectInVRBufPool.get(), mpFDBufPool.get(), mpTuningBufferPool.get());

    MY_LOGD("- : BufferPoolSet init ");
    config.debug();

    return MTRUE;
}

MBOOL
P2AFMNode::BufferPoolSet::
uninit()
{
    MY_LOGD("+");

    //clear data
    ImageBufferPool::destroy(mpFEOB_BufPool);
    ImageBufferPool::destroy(mpFEOC_BufPool);
    ImageBufferPool::destroy(mpFMOB_BufPool);
    ImageBufferPool::destroy(mpFMOC_BufPool);
    ImageBufferPool::destroy(mpRectInVRBufPool);
    ImageBufferPool::destroy(mpRectInPVBufPool);
    ImageBufferPool::destroy(mpRectInCapBufPool_Main1);
    GraphicBufferPool::destroy(mpRectInCapBufPool_Gra_Main2);

    ImageBufferPool::destroy(mpCCInBufPool);
    ImageBufferPool::destroy(mpFEBOInBufPool_Main1);
    ImageBufferPool::destroy(mpFEBOInBufPool_Main2);
    ImageBufferPool::destroy(mpFECOInBufPool_Main1);
    ImageBufferPool::destroy(mpFECOInBufPool_Main2);
    ImageBufferPool::destroy(mpFDBufPool);
    TuningBufferPool::destroy(mpTuningBufferPool);

    #ifdef GTEST
    ImageBufferPool::destroy(mFEInput_Stage2_Main1);
    ImageBufferPool::destroy(mFEInput_Stage2_Main2);
    ImageBufferPool::destroy(mFEInput_Stage3_Main1);
    ImageBufferPool::destroy(mFEInput_Stage3_Main2);
    #endif
    MY_LOGD("-");

    return MTRUE;
}


P2AFMNode::
P2AFMNode(const char* name, Graph_T* graph, MUINT32 iSensorIdx_Main1, MUINT32 iSensorIdx_Main2)
:DepthMapPipeNode(name, graph), miSensorIdx_Main1(iSensorIdx_Main1),
 miSensorIdx_Main2(iSensorIdx_Main2), mp3A(NULL), mpINormalStream(NULL)
{
    VSDOF_LOGD("Sensor index, Main1: %d, Main2: %d", miSensorIdx_Main1, miSensorIdx_Main2);
    this->addWaitQueue(&mRequestQue);
}

P2AFMNode::
~P2AFMNode()
{
}

MVOID
P2AFMNode::
cleanUp()
{
    MY_LOGD("+");
    if(mpINormalStream != NULL)
    {
        mpINormalStream->uninit("VSDOF_P2AFM");
        mpINormalStream->destroyInstance();
        mpINormalStream = NULL;
    }

    if(mp3A)
    {
        mp3A->destroyInstance("VSDOF_P2AFM");
        mp3A = NULL;
    }

    mFETuningBufferMap.clear();
    mFMTuningBufferMap.clear();

    mBufPoolSet.uninit();

    MY_LOGD("-");
}


MBOOL
P2AFMNode::
onInit()
{
    MY_LOGD("+");
    // Create NormalStream
    VSDOF_LOGD("NormalStream create instance: idx=%d", miSensorIdx_Main1);
    mpINormalStream = NSCam::NSIoPipe::NSPostProc::INormalStream::createInstance(miSensorIdx_Main1);
    if (mpINormalStream == NULL)
    {
        MY_LOGE("mpINormalStream create instance for P2AFM Node failed!");
        cleanUp();
        return MFALSE;
    }

    mpINormalStream->init("VSDOF_P2AFM");

    // BufferPoolSet init
    VSDOF_LOGD("BufferPoolSet init");
    mBufPoolSet.init(mBufConfig);

    // 3A: create instance
    // UT does not test 3A
    #ifndef GTEST
    mp3A = IHal3A::createInstance(IHal3A::E_Camera_3, miSensorIdx_Main1, "VSDOF_P2AFM");
    MY_LOGD("3A create instance: %x", mp3A);
    #endif

    // prepare templates
    prepareTemplateParams();

    // prepare burst trigger QParams
    MBOOL bRet = prepareBurstQParams();
    if(!bRet)
        MY_LOGE("prepareBurstQParams Error! Please check the error msg above!");

    MY_LOGD("-");
    return MTRUE;
}

MVOID
P2AFMNode::
prepareTemplateParams()
{
    VSDOF_LOGD("+");
    //SRZ: crop first, then resize.
    #define CONFIG_SRZINFO_TO_CROPAREA(SrzInfo, StereoArea) \
        SrzInfo->in_w =  StereoArea.size.w;\
        SrzInfo->in_h =  StereoArea.size.h;\
        SrzInfo->crop_w = StereoArea.size.w - StereoArea.padding.w;\
        SrzInfo->crop_h = StereoArea.size.h - StereoArea.padding.h;\
        SrzInfo->crop_x = StereoArea.startPt.x;\
        SrzInfo->crop_y = StereoArea.startPt.y;\
        SrzInfo->crop_floatX = 0;\
        SrzInfo->crop_floatY = 0;\
        SrzInfo->out_w = StereoArea.size.w - StereoArea.padding.w;\
        SrzInfo->out_h = StereoArea.size.h - StereoArea.padding.h;

    // FE SRZ template - frame 2+4 need FEO srz crop
    mSrzSizeTemplateMap.add(2,  new _SRZ_SIZE_INFO_());
    mSrzSizeTemplateMap.add(4,  new _SRZ_SIZE_INFO_());
    CONFIG_SRZINFO_TO_CROPAREA(mSrzSizeTemplateMap.valueFor(2), mBufConfig.P2AFM_MAIN2_FEBO_AREA);
    CONFIG_SRZINFO_TO_CROPAREA(mSrzSizeTemplateMap.valueFor(4), mBufConfig.P2AFM_MAIN2_FECO_AREA);

    // FM tuning template
    // frame 6/8 - forward +  frame 7/9 - backward
    for(int frameID=6;frameID<10;++frameID)
    {
        // prepare FM tuning buffer
        SmartTuningBuffer smFMTuningBuffer = mBufPoolSet.mpTuningBufferPool->request();
        setupEmptyTuningWithFM(smFMTuningBuffer, frameID);
        dip_x_reg_t * pIspReg = (dip_x_reg_t *)smFMTuningBuffer->mpVA;
        MY_LOGD("frameID=%d mFMTuningBufferMap add!!DIP_X_FM_SIZE =%x DIP_X_FM_TH_CON0=%x ", frameID,
                pIspReg->DIP_X_FM_SIZE.Raw, pIspReg->DIP_X_FM_TH_CON0.Raw);
        mFMTuningBufferMap.add(frameID, smFMTuningBuffer);
    }

    // prepare FE tuning buffer
    for (int iStage=1;iStage<=2;++iStage)
    {
        SmartTuningBuffer smFETuningBuffer = mBufPoolSet.mpTuningBufferPool->request();
        MUINT32 iBlockSize = StereoSettingProvider::fefmBlockSize(iStage);
        setupEmptyTuningWithFE(smFETuningBuffer, iBlockSize);
        mFETuningBufferMap.add(iStage, smFETuningBuffer);
    }

    VSDOF_LOGD("-");
}

MVOID
P2AFMNode::
setupEmptyTuningWithFE(SmartTuningBuffer& targetTuningBuf, MUINT iBlockSize)
{
    dip_x_reg_t* isp_reg = reinterpret_cast<dip_x_reg_t*>(targetTuningBuf->mpVA);
    isp_reg->DIP_X_CTL_YUV_EN.Raw = 0;
    isp_reg->DIP_X_CTL_YUV2_EN.Raw = 0;
    isp_reg->DIP_X_CTL_RGB_EN.Raw = 0;

    if(iBlockSize == 16)
    {
        isp_reg->DIP_X_FE_CTRL1.Bits.FE_MODE = 1;
        isp_reg->DIP_X_FE_CTRL1.Bits.FE_PARAM = 4;
        isp_reg->DIP_X_FE_CTRL1.Bits.FE_FLT_EN = 1;
        isp_reg->DIP_X_FE_CTRL1.Bits.FE_TH_G = 1;
        isp_reg->DIP_X_FE_CTRL1.Bits.FE_TH_C = 0;
        isp_reg->DIP_X_FE_CTRL1.Bits.FE_DSCR_SBIT = 3;


        isp_reg->DIP_X_FE_IDX_CTRL.Bits.FE_XIDX = 0;
        isp_reg->DIP_X_FE_IDX_CTRL.Bits.FE_YIDX = 0;

        isp_reg->DIP_X_FE_CROP_CTRL1.Bits.FE_START_X = 0;
        isp_reg->DIP_X_FE_CROP_CTRL1.Bits.FE_START_Y = 0;

        isp_reg->DIP_X_FE_CROP_CTRL2.Bits.FE_IN_WD = 0;
        isp_reg->DIP_X_FE_CROP_CTRL2.Bits.FE_IN_HT = 0;
    }
    else
    {
        isp_reg->DIP_X_FE_CTRL1.Bits.FE_MODE = 2;
        isp_reg->DIP_X_FE_CTRL1.Bits.FE_PARAM = 4;
        isp_reg->DIP_X_FE_CTRL1.Bits.FE_FLT_EN = 1;
        isp_reg->DIP_X_FE_CTRL1.Bits.FE_TH_G = 1;
        isp_reg->DIP_X_FE_CTRL1.Bits.FE_TH_C = 0;
        isp_reg->DIP_X_FE_CTRL1.Bits.FE_DSCR_SBIT = 3;

        isp_reg->DIP_X_FE_IDX_CTRL.Bits.FE_XIDX = 0;
        isp_reg->DIP_X_FE_IDX_CTRL.Bits.FE_YIDX = 0;

        isp_reg->DIP_X_FE_CROP_CTRL1.Bits.FE_START_X = 0;
        isp_reg->DIP_X_FE_CROP_CTRL1.Bits.FE_START_Y = 0;

        isp_reg->DIP_X_FE_CROP_CTRL2.Bits.FE_IN_WD = 0;
        isp_reg->DIP_X_FE_CROP_CTRL2.Bits.FE_IN_HT = 0;
    }
}

MVOID
P2AFMNode::
setupEmptyTuningWithFM(SmartTuningBuffer& targetTuningBuf, MUINT iFrameID)
{
    MY_LOGD("+");
    dip_x_reg_t* isp_reg = reinterpret_cast<dip_x_reg_t*>(targetTuningBuf->mpVA);
    isp_reg->DIP_X_CTL_YUV_EN.Raw = 0;
    isp_reg->DIP_X_CTL_YUV2_EN.Raw = 0x00000001;
    isp_reg->DIP_X_CTL_RGB_EN.Raw = 0;

    MSize szFEBufSize = (iFrameID<=7) ? mBufConfig.P2AFM_FE1BO_INPUT_SIZE : mBufConfig.P2AFM_FE1CO_INPUT_SIZE;;
    MUINT32 iStage = (iFrameID<=7) ? 1 : 2;

    ENUM_FM_DIRECTION eDir = (iFrameID % 2 == 0) ? E_FM_L_TO_R : E_FM_R_TO_L;
    // query tuning parameter
    StereoTuningProvider::getFMTuningInfo(eDir, isp_reg->DIP_X_FM_SIZE, isp_reg->DIP_X_FM_TH_CON0);
    MUINT32 iBlockSize =  StereoSettingProvider::fefmBlockSize(iStage);
    // set width/height
    isp_reg->DIP_X_FM_SIZE.Bits.FM_WIDTH = szFEBufSize.w/iBlockSize;
    isp_reg->DIP_X_FM_SIZE.Bits.FM_HEIGHT = szFEBufSize.h/iBlockSize;

    MY_LOGD("frameID =%d DIP_X_FM_SIZE =%x DIP_X_FM_TH_CON0=%x", iFrameID, isp_reg->DIP_X_FM_SIZE.Raw, isp_reg->DIP_X_FM_TH_CON0.Raw);

    MY_LOGD("-");
}


MBOOL
P2AFMNode::
onUninit()
{
    cleanUp();
    return MTRUE;
}

MBOOL
P2AFMNode::
onData(DataID data, EffectRequestPtr &request)
{
  MBOOL ret = MTRUE;
  VSDOF_PRFLOG("+ : reqID=%d", request->getRequestNo());

  switch(data)
  {
  case ROOT_ENQUE:
    mRequestQue.enque(request);
    break;
  default:
    MY_LOGW("Un-recognized data ID, id=%d reqID=%d", data, request->getRequestNo());
    ret = MFALSE;
    break;
  }

  VSDOF_LOGD("-");
  return ret;
}

const char*
P2AFMNode::
onDumpBIDToName(DepthMapBufferID BID)
{
#define MAKE_NAME_CASE(name) \
    case name: return #name;
    switch(BID)
    {
        MAKE_NAME_CASE(BID_TO_N3D_FE1BO);
        MAKE_NAME_CASE(BID_TO_N3D_FE2BO);
        MAKE_NAME_CASE(BID_TO_N3D_FE1CO);
        MAKE_NAME_CASE(BID_TO_N3D_FE2CO);
        MAKE_NAME_CASE(BID_TO_N3D_FMBO_LR);
        MAKE_NAME_CASE(BID_TO_N3D_FMBO_RL);
        MAKE_NAME_CASE(BID_TO_N3D_FMCO_LR);
        MAKE_NAME_CASE(BID_TO_N3D_FMCO_RL);
        MAKE_NAME_CASE(BID_TO_N3D_CC_IN1);
        MAKE_NAME_CASE(BID_TO_N3D_CC_IN2);
        MAKE_NAME_CASE(BID_TO_N3D_RECT_IN1_CAP);
        MAKE_NAME_CASE(BID_TO_N3D_RECT_IN2_CAP);
        MAKE_NAME_CASE(BID_TO_N3D_RECT_IN1);
        MAKE_NAME_CASE(BID_TO_N3D_RECT_IN2);
        MAKE_NAME_CASE(BID_P2AFM_FE1CO_INPUT);
        MAKE_NAME_CASE(BID_TO_FD_IMG);
        MAKE_NAME_CASE(BID_P2AFM_FE1BO_INPUT);

        #ifdef GTEST
        MAKE_NAME_CASE(BID_FE2_IN_MAIN1);
        MAKE_NAME_CASE(BID_FE2_IN_MAIN2);
        MAKE_NAME_CASE(BID_FE3_IN_MAIN1);
        MAKE_NAME_CASE(BID_FE3_IN_MAIN2);
        #endif

    }
    MY_LOGW("unknown BID:%d", BID);

    return "unknown";
#undef MAKE_NAME_CASE
}


#define SET_FULL_CROP_RS_INFO(cropInfo, inputBufSize, frameID) \
    cropInfo.mCropRect.p_fractional.x=0; \
    cropInfo.mCropRect.p_fractional.y=0; \
    cropInfo.mCropRect.p_integral.x=0; \
    cropInfo.mCropRect.p_integral.y=0; \
    cropInfo.mCropRect.s=inputBufSize; \
    cropInfo.mResizeDst=inputBufSize; \
    cropInfo.mFrameGroup = frameID;

#define SET_ZERO_CROP_RS_INFO(cropInfo, cropSize, inputBufSize, frameID) \
    cropInfo.mCropRect.p_fractional.x=0; \
    cropInfo.mCropRect.p_fractional.y=0; \
    cropInfo.mCropRect.p_integral.x=0; \
    cropInfo.mCropRect.p_integral.y=0; \
    cropInfo.mCropRect.s=cropSize; \
    cropInfo.mResizeDst=inputBufSize; \
    cropInfo.mFrameGroup = frameID;

#define SET_START_CROP_RS_INFO(cropInfo, startPoint, cropSize, inputBufSize, frameID) \
    cropInfo.mCropRect.p_fractional.x=0; \
    cropInfo.mCropRect.p_fractional.y=0; \
    cropInfo.mCropRect.p_integral.x=startPoint.x; \
    cropInfo.mCropRect.p_integral.y=startPoint.y; \
    cropInfo.mCropRect.s=cropSize; \
    cropInfo.mResizeDst=inputBufSize; \
    cropInfo.mFrameGroup = frameID;

MBOOL
P2AFMNode::
prepareBurstQParams()
{
    MY_LOGD("+");
    using namespace NSCam::NSIoPipe::NSPostProc;
    QParams burstParams;

    // module rotation
    ENUM_ROTATION eRot = StereoSettingProvider::getModuleRotation();
    MINT32 iModuleTrans = -1;
    switch(eRot)
    {
        case eRotate_0:
            iModuleTrans = 0;
            break;
        case eRotate_90:
            iModuleTrans = eTransform_ROT_90;
            break;
        case eRotate_180:
            iModuleTrans = eTransform_ROT_180;
            break;
        case eRotate_270:
            iModuleTrans = eTransform_ROT_270;
            break;
        default:
            MY_LOGE("Not support module rotation =%d", eRot);
            return MFALSE;
    }

    MBOOL bRet = prepareBurstQParams_Cap(iModuleTrans);
    bRet &= prepareBurstQParams_NORMAL(iModuleTrans);
    MY_LOGD("-");
    return bRet;
}


MBOOL
P2AFMNode::
prepareBurstQParams_NORMAL(MINT32 iModuleTrans)
{
    MY_LOGD("+");
    using namespace NSCam::NSIoPipe::NSPostProc;

    MBOOL bSuccess;
    //--> frame 0
    // dummy size will be filled with the correct value when runtime.
    MSize dummySize(0,0);
    MPoint zeroPos(0,0);
    {
        bSuccess =
            QParamTemplateGenerator(0, ENormalStreamTag_Normal).   // frame 0
                addInput(PORT_IMGI).
                addCrop(eCROP_WROT, zeroPos, dummySize, mBufConfig.P2AFM_FE2BO_INPUT_SIZE).     // WROT : FE2BO input
                addOutput(mapToPortID(BID_P2AFM_FE2BO_INPUT)).
                generate(mBurstParamTmplate_NORMAL, mTemplStats_NORMAL);
    }

    //--> frame 1
    {
        bSuccess &=
            QParamTemplateGenerator(1, ENormalStreamTag_Normal, &mTemplStats_NORMAL).   // frame 1
                addInput(PORT_IMGI).
                addCrop(eCROP_CRZ, zeroPos, dummySize, mBufConfig.P2AFM_FD_IMG_SIZE).  // IMG2O : FD
                addOutput(mapToPortID(BID_OUT_FD)).
                addCrop(eCROP_WDMA, zeroPos, dummySize, mBufConfig.P2AFM_MAIN_IMAGE_SIZE).  // WDMA : MV_F
                addOutput(mapToPortID(BID_OUT_MV_F)).
                addCrop(eCROP_WROT, zeroPos, dummySize, mBufConfig.P2AFM_FE1BO_INPUT_SIZE).  // WROT: FE1BO input
                addOutput(mapToPortID(BID_P2AFM_FE1BO_INPUT), iModuleTrans).                // do module rotation
                generate(mBurstParamTmplate_NORMAL, mTemplStats_NORMAL);
    }

    //--> frame 2
    {
        MVOID* srzInfo = reinterpret_cast<MVOID*>(mSrzSizeTemplateMap.valueFor(2));

        bSuccess &=
            QParamTemplateGenerator(2, ENormalStreamTag_Normal, &mTemplStats_NORMAL).   // frame 2
                addInput(PORT_IMGI).
                addCrop(eCROP_CRZ, zeroPos, mBufConfig.P2AFM_FE2BO_INPUT_SIZE, mBufConfig.P2AFM_FE2CO_INPUT_SIZE).  // IMG2O: FE2CO input
                addOutput(mapToPortID(BID_P2AFM_FE2CO_INPUT)).
                addCrop(eCROP_WDMA, zeroPos, mBufConfig.P2AFM_FE2BO_INPUT_SIZE, dummySize).  // WDMA : Rect_in2
                addOutput(mapToPortID(BID_TO_N3D_RECT_IN2)).
                #ifdef GTEST
                addOutput(PORT_IMG3O).
                #endif
                addOutput(PORT_FEO).                           // FEO
                addModuleInfo(EDipModule_SRZ1, srzInfo).       // FEO SRZ1 config
                generate(mBurstParamTmplate_NORMAL, mTemplStats_NORMAL);
    }

    //--> frame 3
    {
        bSuccess &=
        QParamTemplateGenerator(3, ENormalStreamTag_Normal, &mTemplStats_NORMAL).   // frame 3
            addInput(PORT_IMGI).
            addCrop(eCROP_CRZ, zeroPos, mBufConfig.P2AFM_FE1BO_INPUT_SIZE, mBufConfig.P2AFM_FE1CO_INPUT_SIZE).  // IMG2O: FE1CO input
            addOutput(mapToPortID(BID_P2AFM_FE1CO_INPUT)).
            addCrop(eCROP_WDMA, zeroPos, mBufConfig.P2AFM_FE1BO_INPUT_SIZE, dummySize).  // WDMA : Rect_in1
            addOutput(mapToPortID(BID_TO_N3D_RECT_IN1)).
            #ifdef GTEST
            addOutput(PORT_IMG3O).
            #endif
            addOutput(PORT_FEO).                           // FEO
            generate(mBurstParamTmplate_NORMAL, mTemplStats_NORMAL);
    }

    //--> frame 4
    {
        MVOID* srzInfo = reinterpret_cast<MVOID*>(mSrzSizeTemplateMap.valueFor(4));
        bSuccess &=
        QParamTemplateGenerator(4, ENormalStreamTag_Normal, &mTemplStats_NORMAL).   // frame 4
            addInput(PORT_IMGI).
            addCrop(eCROP_WDMA, zeroPos, mBufConfig.P2AFM_FE2CO_INPUT_SIZE, mBufConfig.P2AFM_CCIN_SIZE).  // WDMA: CC_in2
            addOutput(mapToPortID(BID_TO_N3D_CC_IN2)).
            #ifdef GTEST
            addOutput(PORT_IMG3O).
            #endif
            addOutput(PORT_FEO).                           // FEO
            addModuleInfo(EDipModule_SRZ1, srzInfo).       // FEO SRZ1 config
            generate(mBurstParamTmplate_NORMAL, mTemplStats_NORMAL);
    }


    //--> frame 5
    {
        bSuccess &=
        QParamTemplateGenerator(5, ENormalStreamTag_Normal, &mTemplStats_NORMAL).   // frame 5
            addInput(PORT_IMGI).
            addCrop(eCROP_WDMA, zeroPos, mBufConfig.P2AFM_FE1CO_INPUT_SIZE, mBufConfig.P2AFM_CCIN_SIZE).  // WDMA: CC_in1
            addOutput(mapToPortID(BID_TO_N3D_CC_IN1)).
            #ifdef GTEST
            addOutput(PORT_IMG3O).
            #endif
            addOutput(PORT_FEO).                           // FEO
            generate(mBurstParamTmplate_NORMAL, mTemplStats_NORMAL);
    }

    //--> frame 6 ~ 9
    for(int frameID=6;frameID<10;++frameID)
    {
        bSuccess &=
        QParamTemplateGenerator(frameID, ENormalStreamTag_FM, &mTemplStats_NORMAL).
            addInput(PORT_DEPI).
            addInput(PORT_DMGI).
            addOutput(PORT_MFBO).
            generate(mBurstParamTmplate_NORMAL, mTemplStats_NORMAL);
    }

    //MY_LOGD("debugQParams mBurstParamTmplate_NORMAL:");
    //debugQParams(mBurstParamTmplate_NORMAL);

    MY_LOGD("-");
    return bSuccess;

}


MBOOL
P2AFMNode::
prepareBurstQParams_Cap(MINT32 iModuleTrans)
{
    MY_LOGD("+");
    using namespace NSCam::NSIoPipe::NSPostProc;

    // difference with PV : extra LSC2 buffer, different MV_F size, different Rect_in size
    bool bSuccess;
    //--> frame 0
    // dummy size will be filled with the correct value when runtime.
    MSize dummySize(0,0);
    MPoint zeroPos(0,0);
    {
        bSuccess =
        QParamTemplateGenerator(0, ENormalStreamTag_Normal).   // frame 0
            addInput(PORT_IMGI).
            addCrop(eCROP_WROT, zeroPos, dummySize, mBufConfig.P2AFM_FE2BO_INPUT_SIZE).  // WROT : FE2BO input
            addOutput(mapToPortID(BID_P2AFM_FE2BO_INPUT)).
            generate(mBurstParamTmplate_Cap, mTemplStats_Cap);
    }

    //--> frame 1
    {

        bSuccess &=
        QParamTemplateGenerator(1, ENormalStreamTag_Normal, &mTemplStats_Cap).   // frame 1
            addInput(PORT_IMGI).
            #ifndef GTEST
            addInput(PORT_DEPI).    // Capture need extra LSC2 buffer input
            #endif
            addCrop(eCROP_CRZ, zeroPos, dummySize, mBufConfig.P2AFM_FD_IMG_SIZE).  // IMG2O : FD
            addOutput(mapToPortID(BID_TO_FD_IMG)).
            addCrop(eCROP_WDMA, zeroPos, dummySize, mBufConfig.P2AFM_MAIN_IMAGE_CAP_SIZE).  // WDMA : MV_F
            addOutput(mapToPortID(BID_OUT_MV_F)).
            addCrop(eCROP_WROT, zeroPos, dummySize, mBufConfig.P2AFM_FE1BO_INPUT_SIZE).  // WROT: FE1BO input
            addOutput(mapToPortID(BID_P2AFM_FE1BO_INPUT), iModuleTrans).                // do module rotation
            generate(mBurstParamTmplate_Cap, mTemplStats_Cap);
    }

    //--> frame 2
    {
        MVOID* srzInfo = reinterpret_cast<MVOID*>(mSrzSizeTemplateMap.valueFor(2));

        bSuccess &=
        QParamTemplateGenerator(2, ENormalStreamTag_Normal, &mTemplStats_Cap).   // frame 2
            addInput(PORT_IMGI).
            addCrop(eCROP_CRZ, zeroPos, mBufConfig.P2AFM_FE2BO_INPUT_SIZE, mBufConfig.P2AFM_FE2CO_INPUT_SIZE).  // IMG2O: FE2CO input
            addOutput(mapToPortID(BID_P2AFM_FE2CO_INPUT)).
            addCrop(eCROP_WDMA, zeroPos, mBufConfig.P2AFM_FE2BO_INPUT_SIZE, mBufConfig.P2AFM_RECT_IN_CAP_SIZE).  // WDMA : Rect_in2
            addOutput(mapToPortID(BID_TO_N3D_RECT_IN2)).
            addOutput(PORT_FEO).                           // FEO
            addModuleInfo(EDipModule_SRZ1, srzInfo).       // FEO SRZ1 config
            generate(mBurstParamTmplate_Cap, mTemplStats_Cap);
    }

    //--> frame 3
    {
        bSuccess &=
        QParamTemplateGenerator(3, ENormalStreamTag_Normal, &mTemplStats_Cap).   // frame 3
            addInput(PORT_IMGI).
            addCrop(eCROP_CRZ, zeroPos, mBufConfig.P2AFM_FE1BO_INPUT_SIZE, mBufConfig.P2AFM_FE1CO_INPUT_SIZE).  // IMG2O: FE1CO input
            addOutput(mapToPortID(BID_P2AFM_FE1CO_INPUT)).
            addCrop(eCROP_WDMA, zeroPos, mBufConfig.P2AFM_FE1BO_INPUT_SIZE, mBufConfig.P2AFM_RECT_IN_CAP_SIZE).  // WDMA : Rect_in1
            addOutput(mapToPortID(BID_TO_N3D_RECT_IN1)).
            addOutput(PORT_FEO).                           // FEO
            generate(mBurstParamTmplate_Cap, mTemplStats_Cap);
    }

    //--> frame 4
    {
        MVOID* srzInfo = reinterpret_cast<MVOID*>(mSrzSizeTemplateMap.valueFor(4));
        bSuccess &=
        QParamTemplateGenerator(4, ENormalStreamTag_Normal, &mTemplStats_Cap).   // frame 4
            addInput(PORT_IMGI).
            // CC_in change to WDMA
            //addCrop(eCROP_CRZ, zeroPos, mBufConfig.P2AFM_FE2CO_INPUT_SIZE, mBufConfig.P2AFM_CCIN_SIZE).  // IMG2O: CC_in2
            addCrop(eCROP_WDMA, zeroPos, mBufConfig.P2AFM_FE2CO_INPUT_SIZE, mBufConfig.P2AFM_CCIN_SIZE).  // IMG2O: CC_in2
            addOutput(mapToPortID(BID_TO_N3D_CC_IN2)).
            addOutput(PORT_FEO).                           // FEO
            addModuleInfo(EDipModule_SRZ1, srzInfo).       // FEO SRZ1 config
            generate(mBurstParamTmplate_Cap, mTemplStats_Cap);
    }


    //--> frame 5
    {
        bSuccess &=
        QParamTemplateGenerator(5, ENormalStreamTag_Normal, &mTemplStats_Cap).   // frame 5
            addInput(PORT_IMGI).
            //addCrop(eCROP_CRZ, zeroPos, mBufConfig.P2AFM_FE1CO_INPUT_SIZE, mBufConfig.P2AFM_CCIN_SIZE).  // IMG2O: CC_in1
            addCrop(eCROP_WDMA, zeroPos, mBufConfig.P2AFM_FE1CO_INPUT_SIZE, mBufConfig.P2AFM_CCIN_SIZE).  // IMG2O: CC_in1
            addOutput(mapToPortID(BID_TO_N3D_CC_IN1)).
            addOutput(PORT_FEO).                           // FEO
            generate(mBurstParamTmplate_Cap, mTemplStats_Cap);
    }

    //--> frame 6 ~ 9
    for(int frameID=6;frameID<10;++frameID)
    {
        bSuccess &=
        QParamTemplateGenerator(frameID, ENormalStreamTag_FM, &mTemplStats_Cap).
            addInput(PORT_DEPI).
            addInput(PORT_DMGI).
            addOutput(PORT_MFBO).
            generate(mBurstParamTmplate_Cap, mTemplStats_Cap);
    }

    MY_LOGD("-");

    //MY_LOGD("debugQParams mBurstParamTmplate_Cap:");
    //debugQParams(mBurstParamTmplate_Cap);

    return bSuccess;

}

MBOOL
P2AFMNode::
buildQParams(EffectRequestPtr& rEffReqPtr, QParams& rEnqueParam, EnquedBufPool* pEnquePoolData, DepthNodeOpState eState, const TemplateStatistics& tmplStats)
{
    VSDOF_PRFLOG("+, reqID=%d", rEffReqPtr->getRequestNo());
    // Get the input/output buffer inside the request
    FrameInfoPtr framePtr_inMain1FSRAW = rEffReqPtr->vInputFrameInfo.valueFor(BID_P2AFM_IN_FSRAW);
    FrameInfoPtr framePtr_inMain1RSRAW = rEffReqPtr->vInputFrameInfo.valueFor(BID_P2AFM_IN_RSRAW1);
    FrameInfoPtr framePtr_inMain2RSRAW = rEffReqPtr->vInputFrameInfo.valueFor(BID_P2AFM_IN_RSRAW2);
    FrameInfoPtr framePtr_FD, framePtr_MV_F, framePtr_JPSMain1;
    sp<IImageBuffer> frameBuf, frameBuf_RSRAW1, frameBuf_RSRAW2, frameBuf_FSRAW1, frameBuf_FD;
    SmartImageBuffer poolBuf, fe2boBuf_in, fe1boBuf_in,  fe2coBuf_in, fe1coBuf_in, fmoBuf;
    SmartImageBuffer rectIn1Buf, rectIn2Buf, ccIn1Buf, ccIn2Buf, fDBuf;
    SmartGraphicBuffer rectIn2Buf_gra;
    SmartImageBuffer feoBuf[6];
    SmartTuningBuffer tuningBuf;

    framePtr_inMain1RSRAW->getFrameBuffer(frameBuf_RSRAW1);
    framePtr_inMain2RSRAW->getFrameBuffer(frameBuf_RSRAW2);


    if(eState == STATE_CAPTURE)
    {
        framePtr_inMain1FSRAW->getFrameBuffer(frameBuf_FSRAW1);
    }

    #define ADD_REQUEST_BUFFER_TO_MVOUT(bufID, reqBuf, filler, frameID, portID) \
        pEnquePoolData->addBuffData(bufID, reqBuf); \
        filler.insertOutputBuf(frameID, portID, reqBuf->mImageBuffer.get());

    #define ADD_REQUEST_BUFFER_WITH_AREA_TO_MVOUT(bufID, reqBuf, validSize, filler, frameID, portID) \
        pEnquePoolData->addBuffData(bufID, reqBuf); \
        reqBuf->mImageBuffer->setExtParam(validSize); \
        filler.insertOutputBuf(frameID, portID, reqBuf->mImageBuffer.get());

    //VSDOF_PRFLOG("+, reqID=%d  before frame 0, startup !", rEffReqPtr->getRequestNo());
    QParamTemplateFiller qParamFiller(rEnqueParam, tmplStats);
    MPoint zeroPos(0,0);
    // Make sure the ordering inside the mvIn mvOut
    int mvInIndex = 0, mvOutIndex = 0;
    MUINT iFrameNum = 0;

    FrameInfoPtr framePtr_inAppMeta = rEffReqPtr->vInputFrameInfo.valueFor(BID_META_IN_APP);
    IMetadata* pMeta_InApp = getMetadataFromFrameInfoPtr(framePtr_inAppMeta);

    FrameInfoPtr framePtr_inHalMeta;
    //VSDOF_PRFLOG("+, reqID=%d  before frame 0 !", rEffReqPtr->getRequestNo());
    //--> frame 0
    {
        FrameInfoPtr framePtr_inHalMeta_Main2 = rEffReqPtr->vInputFrameInfo.valueFor(BID_META_IN_HAL_MAIN2);
         // Apply tuning data
        VSDOF_PRFLOG("+, reqID=%d  frame 0, request tuning buffer!", rEffReqPtr->getRequestNo());

        tuningBuf = mBufPoolSet.mpTuningBufferPool->request();

        VSDOF_PRFLOG("+, reqID=%d  frame 0, add tuning buffer!", rEffReqPtr->getRequestNo());
        pEnquePoolData->addTuningData(tuningBuf);


        TuningParam rTuningParam = applyISPTuning(tuningBuf, framePtr_inAppMeta, framePtr_inHalMeta_Main2);
        // insert tuning data
        qParamFiller.insertTuningBuf(iFrameNum, tuningBuf->mpVA);

        // input: Main2 RSRAW
        qParamFiller.insertInputBuf(iFrameNum, PORT_IMGI, framePtr_inMain2RSRAW);
        // WROT : FE2BO input
        qParamFiller.setCrop(iFrameNum, eCROP_WROT, zeroPos, frameBuf_RSRAW2->getImgSize(),
                                mBufConfig.P2AFM_FE2BO_INPUT_SIZE);
        // output: FE2BO input(WROT)
        fe2boBuf_in = mBufPoolSet.mpFEBOInBufPool_Main2->request();
        ADD_REQUEST_BUFFER_TO_MVOUT(BID_P2AFM_FE2BO_INPUT, fe2boBuf_in, qParamFiller, iFrameNum,
                                    mapToPortID(BID_P2AFM_FE2BO_INPUT));
    }

    //VSDOF_PRFLOG("+, reqID=%d  frame 0 end!", rEffReqPtr->getRequestNo());
    //--> frame 1
    {
        iFrameNum = 1;
        FrameInfoPtr framePtr_inHalMeta_Main1 = rEffReqPtr->vInputFrameInfo.valueFor(BID_META_IN_HAL);
        // Apply tuning data
        tuningBuf = mBufPoolSet.mpTuningBufferPool->request();
        pEnquePoolData->addTuningData(tuningBuf);
        TuningParam rTuningParam = applyISPTuning(tuningBuf, framePtr_inAppMeta, framePtr_inHalMeta_Main1);
        // insert tuning data
        qParamFiller.insertTuningBuf(iFrameNum, tuningBuf->mpVA);

        sp<IImageBuffer> frameBuf_MV_F;

        if(eState == STATE_NORMAL)
        {
            // input: Main1 RSRAW
            qParamFiller.insertInputBuf(iFrameNum, PORT_IMGI, framePtr_inMain1RSRAW);

            // output FD
            if(rEffReqPtr->vOutputFrameInfo.indexOfKey(BID_OUT_FD) >= 0)
            {
                 // output: FD
                RETRIEVE_OFRAMEINFO_IMGBUF_ERROR_RETURN(rEffReqPtr, framePtr_FD, BID_OUT_FD, frameBuf_FD);
                // insert FD output
                qParamFiller.insertOutputBuf(iFrameNum, PORT_IMG2O, frameBuf_FD.get());
                // set FD crop
                qParamFiller.setCrop(iFrameNum, eCROP_CRZ, zeroPos, frameBuf_RSRAW1->getImgSize(),
                                        frameBuf_FD->getImgSize());
            }
            else
            {   // no FD-> remove port in template.
                qParamFiller.delOutputPort(iFrameNum, PORT_IMG2O, eCROP_CRZ);
            }
            // output: MV_F
            RETRIEVE_OFRAMEINFO_IMGBUF_ERROR_RETURN(rEffReqPtr, framePtr_MV_F, BID_OUT_MV_F, frameBuf_MV_F);
            if (isEISOn(pMeta_InApp))
            {
                IMetadata* pMeta_InHal = getMetadataFromFrameInfoPtr(framePtr_inHalMeta_Main1);
                eis_region region;
                // set MV_F crop for EIS
                if(queryEisRegion(pMeta_InHal, region, rEffReqPtr))
                {
                    qParamFiller.setCrop(1, eCROP_WDMA, MPoint(region.x_int, region.y_int), region.s, mBufConfig.P2AFM_MAIN_IMAGE_SIZE);
                }
                else
                {
                    MY_LOGE("Query EIS Region Failed! reqID=%d.", rEffReqPtr->getRequestNo());
                    return MFALSE;
                }
            }
            else
            {
                // MV_F crop
                qParamFiller.setCrop(iFrameNum, eCROP_WDMA, zeroPos, frameBuf_RSRAW1->getImgSize(), frameBuf_MV_F->getImgSize());
            }
            qParamFiller.insertOutputBuf(iFrameNum, PORT_WDMA, frameBuf_MV_F.get());
            // ouput: FE1BO_input
            fe1boBuf_in = mBufPoolSet.mpFEBOInBufPool_Main1->request();
            ADD_REQUEST_BUFFER_TO_MVOUT(BID_P2AFM_FE1BO_INPUT, fe1boBuf_in, qParamFiller, iFrameNum,
                                        mapToPortID(BID_P2AFM_FE1BO_INPUT));
            // FE1BO input crop
            qParamFiller.setCrop(iFrameNum, eCROP_WROT, zeroPos, frameBuf_RSRAW1->getImgSize(), mBufConfig.P2AFM_FE1BO_INPUT_SIZE);
        }
        else
        {
            #ifndef GTEST
            // input: LSC2 buffer (for tuning)
            IImageBuffer* pLSC2Src = static_cast<IImageBuffer*>(rTuningParam.pLsc2Buf);
            qParamFiller.insertInputBuf(iFrameNum, PORT_DEPI, pLSC2Src);
            #endif
            // input: Main1 Fullsize RAW
            qParamFiller.insertInputBuf(iFrameNum, PORT_IMGI, framePtr_inMain1FSRAW);
            // make sure the output is 16:9, get crop size& point
            MSize cropSize;
            MPoint startPoint;
            calCropForScreen(framePtr_inMain1FSRAW, startPoint, cropSize);
            // output: FD - working buf
            fDBuf = mBufPoolSet.mpFDBufPool->request();
            ADD_REQUEST_BUFFER_TO_MVOUT(BID_TO_FD_IMG, fDBuf, qParamFiller, iFrameNum, mapToPortID(BID_TO_FD_IMG));
            // FD crop
            qParamFiller.setCrop(iFrameNum, eCROP_CRZ, startPoint, cropSize, mBufConfig.P2AFM_FD_IMG_SIZE);
            // output : MV_F_CAP
            RETRIEVE_OFRAMEINFO_IMGBUF_ERROR_RETURN(rEffReqPtr, framePtr_MV_F, BID_OUT_MV_F_CAP, frameBuf_MV_F);
            qParamFiller.insertOutputBuf(iFrameNum, PORT_WDMA, frameBuf_MV_F.get());
            // MV_F_CAP crop
            qParamFiller.setCrop(iFrameNum, eCROP_WDMA, startPoint, cropSize, frameBuf_MV_F->getImgSize());

            // ouput: FE1BO_input
            fe1boBuf_in = mBufPoolSet.mpFEBOInBufPool_Main1->request();
            ADD_REQUEST_BUFFER_TO_MVOUT(BID_P2AFM_FE1BO_INPUT, fe1boBuf_in, qParamFiller, iFrameNum,
                                        mapToPortID(BID_P2AFM_FE1BO_INPUT));
            // FE1BO input crop
            qParamFiller.setCrop(iFrameNum, eCROP_WROT, startPoint, cropSize, mBufConfig.P2AFM_FE1BO_INPUT_SIZE);
        }

    }

    //VSDOF_PRFLOG("+, reqID=%d  frame 1 end!", rEffReqPtr->getRequestNo());
    //--> frame 2
    {
        iFrameNum = 2;
        // input: fe2boBuf_in
        qParamFiller.insertInputBuf(iFrameNum, PORT_IMGI, fe2boBuf_in->mImageBuffer.get());
        // output: FE2CO input
        fe2coBuf_in = mBufPoolSet.mpFECOInBufPool_Main2->request();
        ADD_REQUEST_BUFFER_TO_MVOUT(BID_P2AFM_FE2CO_INPUT, fe2coBuf_in, qParamFiller, iFrameNum,
                                    mapToPortID(BID_P2AFM_FE2CO_INPUT));

         // output: Rect_in2
        if(eState == STATE_CAPTURE)
        {
            rectIn2Buf_gra = mBufPoolSet.mpRectInCapBufPool_Gra_Main2->request();
            ADD_REQUEST_BUFFER_WITH_AREA_TO_MVOUT(BID_TO_N3D_RECT_IN2_CAP, rectIn2Buf_gra,
                            mBufConfig.P2AFM_RECT_IN_CAP_IMG_SIZE, qParamFiller, iFrameNum, mapToPortID(BID_TO_N3D_RECT_IN2_CAP));
        }
        else
        {
            rectIn2Buf = isEISOn(pMeta_InApp) ? mBufPoolSet.mpRectInVRBufPool->request() : mBufPoolSet.mpRectInPVBufPool->request();
            // Rect_in2 CROP
            qParamFiller.setCrop(iFrameNum, eCROP_WDMA, zeroPos, mBufConfig.P2AFM_FE2BO_INPUT_SIZE, rectIn2Buf->mImageBuffer->getImgSize());
            // Rect_in2
            ADD_REQUEST_BUFFER_TO_MVOUT(BID_TO_N3D_RECT_IN2, rectIn2Buf, qParamFiller, iFrameNum, mapToPortID(BID_TO_N3D_RECT_IN2));
            #ifdef GTEST
            if(eState == STATE_NORMAL)
            {
                SmartImageBuffer smBuf_UT = mBufPoolSet.mFEInput_Stage2_Main2->request();
                ADD_REQUEST_BUFFER_TO_MVOUT(BID_FE2_IN_MAIN2, smBuf_UT, qParamFiller, iFrameNum, PORT_IMG3O);
            }
            #endif

        }

        // output: FE2BO
        feoBuf[iFrameNum] = mBufPoolSet.mpFEOB_BufPool->request();
        ADD_REQUEST_BUFFER_TO_MVOUT(BID_TO_N3D_FE2BO, feoBuf[iFrameNum], qParamFiller, iFrameNum, mapToPortID(BID_TO_N3D_FE2BO));

        // tuning data - apply Stage_1 cached FE tuning buffer
        qParamFiller.insertTuningBuf(iFrameNum, mFETuningBufferMap.valueFor(1)->mpVA);
    }

    //VSDOF_PRFLOG("+, reqID=%d  frame 2 end!", rEffReqPtr->getRequestNo());
    //--> frame 3
    {
        iFrameNum = 3;
        // input: fe1boBuf_in
        qParamFiller.insertInputBuf(iFrameNum, PORT_IMGI, fe1boBuf_in->mImageBuffer.get());
        // output: FE1CO input
        fe1coBuf_in = mBufPoolSet.mpFECOInBufPool_Main1->request();
        ADD_REQUEST_BUFFER_TO_MVOUT(BID_P2AFM_FE1CO_INPUT, fe1coBuf_in, qParamFiller, iFrameNum, mapToPortID(BID_P2AFM_FE1CO_INPUT));

        // output: Rect_in1
        if(eState == STATE_CAPTURE)
        {
            rectIn1Buf = mBufPoolSet.mpRectInCapBufPool_Main1->request();
            ADD_REQUEST_BUFFER_WITH_AREA_TO_MVOUT(BID_TO_N3D_RECT_IN1_CAP, rectIn1Buf,
                            mBufConfig.P2AFM_RECT_IN_CAP_IMG_SIZE, qParamFiller, iFrameNum, mapToPortID(BID_TO_N3D_RECT_IN1_CAP));
        }
        else
        {
            rectIn1Buf = isEISOn(pMeta_InApp)? mBufPoolSet.mpRectInVRBufPool->request():mBufPoolSet.mpRectInPVBufPool->request();
            // Rect_in2 CROP
            qParamFiller.setCrop(iFrameNum, eCROP_WDMA, zeroPos, mBufConfig.P2AFM_FE1BO_INPUT_SIZE, rectIn1Buf->mImageBuffer->getImgSize());
            ADD_REQUEST_BUFFER_TO_MVOUT(BID_TO_N3D_RECT_IN1, rectIn1Buf, qParamFiller, iFrameNum, mapToPortID(BID_TO_N3D_RECT_IN1));
        }
        // output: FE1BO
        feoBuf[iFrameNum] = mBufPoolSet.mpFEOB_BufPool->request();
        ADD_REQUEST_BUFFER_TO_MVOUT(BID_TO_N3D_FE1BO, feoBuf[iFrameNum], qParamFiller, iFrameNum, mapToPortID(BID_TO_N3D_FE1BO));

        #ifdef GTEST
        if(eState == STATE_NORMAL)
        {
            SmartImageBuffer smBuf_UT = mBufPoolSet.mFEInput_Stage2_Main1->request();
            ADD_REQUEST_BUFFER_TO_MVOUT(BID_FE2_IN_MAIN1, smBuf_UT, qParamFiller, iFrameNum, PORT_IMG3O);
        }
        #endif

        // tuning data - apply Stage_1 cached FE tuning buffer
        qParamFiller.insertTuningBuf(iFrameNum, mFETuningBufferMap.valueFor(1)->mpVA);
    }

    //VSDOF_PRFLOG("+, reqID=%d  frame 3 end!", rEffReqPtr->getRequestNo());
    //--> frame 4
    {
        iFrameNum = 4;
        // input: fe2coBuf_in
        qParamFiller.insertInputBuf(iFrameNum, PORT_IMGI, fe2coBuf_in->mImageBuffer.get());
        // output: CC_in
        ccIn2Buf = mBufPoolSet.mpCCInBufPool->request();
        ADD_REQUEST_BUFFER_TO_MVOUT(BID_TO_N3D_CC_IN2, ccIn2Buf, qParamFiller, iFrameNum,
                                    mapToPortID(BID_TO_N3D_CC_IN2));
        // output: FE2CO
        feoBuf[iFrameNum] = mBufPoolSet.mpFEOC_BufPool->request();
        ADD_REQUEST_BUFFER_TO_MVOUT(BID_TO_N3D_FE2CO, feoBuf[iFrameNum], qParamFiller, iFrameNum,
                                    mapToPortID(BID_TO_N3D_FE2CO));

        #ifdef GTEST
        if(eState == STATE_NORMAL)
        {
            SmartImageBuffer smBuf_UT = mBufPoolSet.mFEInput_Stage3_Main2->request();
            ADD_REQUEST_BUFFER_TO_MVOUT(BID_FE3_IN_MAIN2, smBuf_UT, qParamFiller, iFrameNum, PORT_IMG3O);
        }
        #endif
        // tuning data - apply Stage_2 cached FE tuning buffer
        qParamFiller.insertTuningBuf(iFrameNum, mFETuningBufferMap.valueFor(2)->mpVA);
    }
    //VSDOF_PRFLOG("+, reqID=%d  frame 4 end!", rEffReqPtr->getRequestNo());
    //--> frame 5
    {
        iFrameNum = 5;
        // input: fe1coBuf_in
        qParamFiller.insertInputBuf(iFrameNum, PORT_IMGI, fe1coBuf_in->mImageBuffer.get());
        // output: CC_in
        ccIn1Buf = mBufPoolSet.mpCCInBufPool->request();
        ADD_REQUEST_BUFFER_TO_MVOUT(BID_TO_N3D_CC_IN1, ccIn1Buf, qParamFiller, iFrameNum, mapToPortID(BID_TO_N3D_CC_IN1));
        // output: FE1CO
        feoBuf[iFrameNum] = mBufPoolSet.mpFEOC_BufPool->request();
        ADD_REQUEST_BUFFER_TO_MVOUT(BID_TO_N3D_FE1CO, feoBuf[iFrameNum], qParamFiller, iFrameNum, mapToPortID(BID_TO_N3D_FE1CO));

        #ifdef GTEST
        if(eState == STATE_NORMAL)
        {
            SmartImageBuffer smBuf_UT = mBufPoolSet.mFEInput_Stage3_Main1->request();
            ADD_REQUEST_BUFFER_TO_MVOUT(BID_FE3_IN_MAIN1, smBuf_UT, qParamFiller, iFrameNum, PORT_IMG3O);
        }
        #endif
        // tuning data - apply Stage_2 cached FE tuning buffer
        qParamFiller.insertTuningBuf(iFrameNum, mFETuningBufferMap.valueFor(2)->mpVA);
    }

    //VSDOF_PRFLOG("+, reqID=%d  frame 5 end!", rEffReqPtr->getRequestNo());
    //--> frame 6: FM
    iFrameNum = 6;
    qParamFiller.insertInputBuf(iFrameNum, PORT_DEPI, feoBuf[3]->mImageBuffer.get());
    qParamFiller.insertInputBuf(iFrameNum, PORT_DMGI, feoBuf[2]->mImageBuffer.get());
    fmoBuf = mBufPoolSet.mpFMOB_BufPool->request();
    ADD_REQUEST_BUFFER_TO_MVOUT(BID_TO_N3D_FMBO_LR, fmoBuf, qParamFiller, iFrameNum, mapToPortID(BID_TO_N3D_FMBO_LR));
    //VSDOF_PRFLOG("+, reqID=%d  frame 5 before request!", rEffReqPtr->getRequestNo());
    SmartTuningBuffer smFMTuningBuf = mFMTuningBufferMap.valueFor(iFrameNum);
    qParamFiller.insertTuningBuf(iFrameNum, smFMTuningBuf->mpVA);
    //VSDOF_PRFLOG("+, reqID=%d  frame 6 end!", rEffReqPtr->getRequestNo());

    //--> frame 7: FM
    iFrameNum = 7;
    qParamFiller.insertInputBuf(iFrameNum, PORT_DEPI, feoBuf[2]->mImageBuffer.get());
    qParamFiller.insertInputBuf(iFrameNum, PORT_DMGI, feoBuf[3]->mImageBuffer.get());
    fmoBuf = mBufPoolSet.mpFMOB_BufPool->request();
    ADD_REQUEST_BUFFER_TO_MVOUT(BID_TO_N3D_FMBO_RL, fmoBuf, qParamFiller, iFrameNum, mapToPortID(BID_TO_N3D_FMBO_RL));

    smFMTuningBuf = mFMTuningBufferMap.valueFor(iFrameNum);
    qParamFiller.insertTuningBuf(iFrameNum, smFMTuningBuf->mpVA);
    //VSDOF_PRFLOG("+, reqID=%d  frame 7 end!", rEffReqPtr->getRequestNo());

    //--> frame 8: FM
    iFrameNum = 8;
    qParamFiller.insertInputBuf(iFrameNum, PORT_DEPI, feoBuf[5]->mImageBuffer.get());
    qParamFiller.insertInputBuf(iFrameNum, PORT_DMGI, feoBuf[4]->mImageBuffer.get());
    fmoBuf = mBufPoolSet.mpFMOC_BufPool->request();
    ADD_REQUEST_BUFFER_TO_MVOUT(BID_TO_N3D_FMCO_LR, fmoBuf, qParamFiller, iFrameNum, mapToPortID(BID_TO_N3D_FMCO_LR));

    smFMTuningBuf = mFMTuningBufferMap.valueFor(iFrameNum);
    qParamFiller.insertTuningBuf(iFrameNum, smFMTuningBuf->mpVA);

    //VSDOF_PRFLOG("+, reqID=%d  frame 8 end!", rEffReqPtr->getRequestNo());
    //--> frame 9: FM
    iFrameNum = 9;
    qParamFiller.insertInputBuf(iFrameNum, PORT_DEPI, feoBuf[4]->mImageBuffer.get());
    qParamFiller.insertInputBuf(iFrameNum, PORT_DMGI, feoBuf[5]->mImageBuffer.get());
    fmoBuf = mBufPoolSet.mpFMOC_BufPool->request();
    ADD_REQUEST_BUFFER_TO_MVOUT(BID_TO_N3D_FMCO_RL, fmoBuf, qParamFiller, iFrameNum, mapToPortID(BID_TO_N3D_FMCO_RL));

    smFMTuningBuf = mFMTuningBufferMap.valueFor(iFrameNum);
    qParamFiller.insertTuningBuf(iFrameNum, smFMTuningBuf->mpVA);

    //VSDOF_PRFLOG("+, reqID=%d  frame 9 end!", rEffReqPtr->getRequestNo());
    VSDOF_PRFLOG("-, reqID=%d", rEffReqPtr->getRequestNo());

    MBOOL bRet = qParamFiller.validate();

    return bRet;
}

#undef ADD_REQUEST_BUFFER_WITH_AREA_TO_MVOUT
#undef ADD_REQUEST_BUFFER_TO_MVOUT
#undef APPLY_EMPTY_TUNING_WITH_FM
#undef APPLY_EMPTY_TUNING_WITH_FE

MBOOL
P2AFMNode::
calCropForScreen(FrameInfoPtr& pFSRAW_frameInfo, MPoint &rCropStartPt, MSize& rCropSize )
{
    sp<IImageBuffer> pImgBuf;
    pFSRAW_frameInfo->getFrameBuffer(pImgBuf);
    MSize srcSize = pImgBuf->getImgSize();

    // check 16:9
    int iHeight = srcSize.w * (9.0) / 16;
    if(abs(iHeight-srcSize.h) < 10)
    {
        rCropStartPt = MPoint(0, 0);
        rCropSize = srcSize;
    }
    else
    {
        rCropStartPt.x = 0;
        rCropStartPt.y = iHeight/2;
        rCropSize.w = srcSize.w;
        rCropSize.h = iHeight;
    }

    MY_LOGD("calCropForScreen rCropStartPt: (%d, %d), \
                    rCropSize: %dx%d ", rCropStartPt.x, rCropStartPt.y, rCropSize.w, rCropSize.h);

    return MTRUE;

}

TuningParam
P2AFMNode::
applyISPTuning(SmartTuningBuffer& targetTuningBuf, FrameInfoPtr& framePtr_inAppMeta, FrameInfoPtr& framePtr_inHalMeta)
{
    VSDOF_PRFLOG("+, reqID=%d", framePtr_inAppMeta->getRequestNo());

    TuningParam tuningParam = {NULL, NULL};
    tuningParam.pRegBuf = reinterpret_cast<void*>(targetTuningBuf->mpVA);

    MetaSet_T inMetaSet;
    IMetadata* pMeta_InApp  = getMetadataFromFrameInfoPtr(framePtr_inAppMeta);
    IMetadata* pMeta_InHal  = getMetadataFromFrameInfoPtr(framePtr_inHalMeta);

    inMetaSet.appMeta = *pMeta_InApp;
    inMetaSet.halMeta = *pMeta_InHal;

    // USE resize raw-->set PGN 0
    updateEntry<MUINT8>(&(inMetaSet.halMeta), MTK_3A_PGN_ENABLE, 0);

    // UT do not test setIsp
    #ifndef GTEST
    mp3A->setIsp(0, inMetaSet, &tuningParam, NULL);
    #else
    {   // UT: use default tuning
        SetDefaultTuning((dip_x_reg_t*)tuningParam.pRegBuf, (MUINT32*)tuningParam.pRegBuf, tuning_tag_G2G, 0);
        SetDefaultTuning((dip_x_reg_t*)tuningParam.pRegBuf, (MUINT32*)tuningParam.pRegBuf, tuning_tag_G2C, 0);
        SetDefaultTuning((dip_x_reg_t*)tuningParam.pRegBuf, (MUINT32*)tuningParam.pRegBuf, tuning_tag_GGM, 0);
        SetDefaultTuning((dip_x_reg_t*)tuningParam.pRegBuf, (MUINT32*)tuningParam.pRegBuf, tuning_tag_UDM, 0);
    }
    #endif


    VSDOF_PRFLOG("-, reqID=%d", framePtr_inAppMeta->getRequestNo());

    return tuningParam;
}



MBOOL
P2AFMNode::
onThreadLoop()
{
    EffectRequestPtr request;

    if( !waitAllQueue() )
    {
        // mJobs.abort() called
        MY_LOGE("waitAllQueue failed");
        return MFALSE;
    }

    if( !mRequestQue.deque(request) )
    {
        MY_LOGE("mRequestQue.deque() failed");
        return MFALSE;
    }

    VSDOF_PRFLOG("threadLoop start, reqID=%d", request->getRequestNo());
    // mark on-going-request start
    this->incExtThreadDependency();
    //prepare enque parameter
    QParams enqueParams;
    EnquedBufPool *enquedData = new EnquedBufPool(request, this);

    // get current state
    DepthNodeOpState eState = getRequestState(request);
    TemplateStatistics tmplStats;

    // copy the corresponding QParams template
    if(eState == STATE_CAPTURE)
    {
        enqueParams = mBurstParamTmplate_Cap;
        tmplStats = mTemplStats_Cap;
    }
    else
    {
        enqueParams = mBurstParamTmplate_NORMAL;
        tmplStats = mTemplStats_NORMAL;
    }

    // buildQParams
    MBOOL ret = buildQParams(request, enqueParams, enquedData, eState, tmplStats);
    //debugQParams(enqueParams);

    if(!ret)
    {
        MY_LOGE("Failed to build P2 enque parametes.");
        return MFALSE;
    }

    // callback
    enqueParams.mpfnCallback = onP2Callback;
    enqueParams.mpfnEnQFailCallback = onP2FailedCallback;
    enqueParams.mpCookie = (MVOID*) enquedData;

    //VSDOF_PRFLOG("mpINormalStream enque start! reqID=%d", request->getRequestNo());
    MY_LOGD("[VSDOF Profile]mpINormalStream enque start, reqID=%d", request->getRequestNo());
    ret = mpINormalStream->enque(enqueParams);

    if(!ret)
    {
        MY_LOGE("mpINormalStream enque failed! reqID=%d", request->getRequestNo());
        goto lbExit;
    }

    VSDOF_PRFLOG("threadLoop end! reqID=%d", request->getRequestNo());

    return MTRUE;

lbExit:
    // mark on-going-request end
    this->decExtThreadDependency();
    return MFALSE;

}

MVOID
P2AFMNode::
onP2Callback(QParams& rParams)
{
    EnquedBufPool* pEnqueData = (EnquedBufPool*) (rParams.mpCookie);
    P2AFMNode* pP2AFMNode = (P2AFMNode*) (pEnqueData->mpNode);
    pP2AFMNode->handleP2Done(rParams, pEnqueData);
}

MVOID
P2AFMNode::
onP2FailedCallback(QParams& rParams)
{
    MY_LOGE("P2A operations failed!!Check the following log:");
    EnquedBufPool* pEnqueData = (EnquedBufPool*) (rParams.mpCookie);
    P2AFMNode* pP2AFMNode = (P2AFMNode*) (pEnqueData->mpNode);
    pP2AFMNode->debugQParams(rParams);
}

MVOID
P2AFMNode::
handleP2Done(QParams& rParams, EnquedBufPool* pEnqueBufPool)
{

    EffectRequestPtr request = pEnqueBufPool->mRequest;
    //VSDOF_PRFLOG("+ :reqID=%d", request->getRequestNo());
    MY_LOGD("[VSDOF_Profile] + :reqID=%d", request->getRequestNo());
    // mark frame ready
    FrameInfoPtr framePtr_FD, framePtr_outMainImg;

    // FD
    if (request->vOutputFrameInfo.indexOfKey(BID_OUT_FD) >= 0)
    {
        framePtr_FD = request->vOutputFrameInfo.valueFor(BID_OUT_FD);
        framePtr_FD->setFrameReady(true);
        handleDataAndDump(P2AFM_OUT_FD, framePtr_FD);
    }

    // MainImage
    if (request->vOutputFrameInfo.indexOfKey(BID_OUT_MV_F) >= 0)
    {
        framePtr_outMainImg = request->vOutputFrameInfo.valueFor(BID_OUT_MV_F);
        framePtr_outMainImg->setFrameReady(true);
        handleDataAndDump(P2AFM_OUT_MV_F, framePtr_outMainImg);
    }

    // MainImage Capture
    if (request->vOutputFrameInfo.indexOfKey(BID_OUT_MV_F_CAP) >= 0)
    {
        framePtr_outMainImg = request->vOutputFrameInfo.valueFor(BID_OUT_MV_F_CAP);
        framePtr_outMainImg->setFrameReady(true);
        handleDataAndDump(P2AFM_OUT_MV_F_CAP, framePtr_outMainImg);
    }

    // set output meta ready
    if (request->vOutputFrameInfo.indexOfKey(BID_META_OUT_APP) >= 0)
    {
        FrameInfoPtr outMeta_frameInfo = request->vOutputFrameInfo.valueFor(BID_META_OUT_APP);
        outMeta_frameInfo->setFrameReady(true);
        outMeta_frameInfo = request->vOutputFrameInfo.valueFor(BID_META_OUT_HAL);
        outMeta_frameInfo->setFrameReady(true);
    }


    // prepare data for N3D input
    SmartImageBuffer smBuf;

    #define ADD_ENQUE_BUFF_TO_INFO_MAP(infoMap, enqueBufPool, BID)\
        smBuf = enqueBufPool->mEnquedSmartImgBufMap.valueFor(BID); \
        infoMap->addImageBuffer(BID, smBuf);

    // FEO/FMO
    sp<ImageBufInfoMap> TON3D_ImgBufInfo = new ImageBufInfoMap(request);

    ADD_ENQUE_BUFF_TO_INFO_MAP(TON3D_ImgBufInfo, pEnqueBufPool, BID_TO_N3D_FE1BO);
    ADD_ENQUE_BUFF_TO_INFO_MAP(TON3D_ImgBufInfo, pEnqueBufPool, BID_TO_N3D_FE2BO);
    ADD_ENQUE_BUFF_TO_INFO_MAP(TON3D_ImgBufInfo, pEnqueBufPool, BID_TO_N3D_FE1CO);
    ADD_ENQUE_BUFF_TO_INFO_MAP(TON3D_ImgBufInfo, pEnqueBufPool, BID_TO_N3D_FE2CO);


    ADD_ENQUE_BUFF_TO_INFO_MAP(TON3D_ImgBufInfo, pEnqueBufPool, BID_TO_N3D_FMBO_LR);
    ADD_ENQUE_BUFF_TO_INFO_MAP(TON3D_ImgBufInfo, pEnqueBufPool, BID_TO_N3D_FMBO_RL);

    ADD_ENQUE_BUFF_TO_INFO_MAP(TON3D_ImgBufInfo, pEnqueBufPool, BID_TO_N3D_FMCO_LR);
    ADD_ENQUE_BUFF_TO_INFO_MAP(TON3D_ImgBufInfo, pEnqueBufPool, BID_TO_N3D_FMCO_RL);


    // CC_in
    ADD_ENQUE_BUFF_TO_INFO_MAP(TON3D_ImgBufInfo, pEnqueBufPool, BID_TO_N3D_CC_IN1);
    ADD_ENQUE_BUFF_TO_INFO_MAP(TON3D_ImgBufInfo, pEnqueBufPool, BID_TO_N3D_CC_IN2);

    // Rect_in
    DepthNodeOpState state = getRequestState(request);

    if(state == STATE_CAPTURE)
    {
        ADD_ENQUE_BUFF_TO_INFO_MAP(TON3D_ImgBufInfo, pEnqueBufPool, BID_TO_N3D_RECT_IN1_CAP);
        // Rect_in2: graphic buffers
        SmartGraphicBuffer pGraBuf = pEnqueBufPool->mEnquedSmartGraImgBufMap.valueFor(BID_TO_N3D_RECT_IN2_CAP);
        TON3D_ImgBufInfo->addGraphicBuffer(BID_TO_N3D_RECT_IN2_CAP, pGraBuf);
    }
    else
    {
        ADD_ENQUE_BUFF_TO_INFO_MAP(TON3D_ImgBufInfo, pEnqueBufPool, BID_TO_N3D_RECT_IN1);
        ADD_ENQUE_BUFF_TO_INFO_MAP(TON3D_ImgBufInfo, pEnqueBufPool, BID_TO_N3D_RECT_IN2);
    }



    // pass to N3D
    handleDataAndDump(P2AFM_TO_N3D_FEFM_CCin, TON3D_ImgBufInfo);


    #ifdef GTEST
    if(state == STATE_NORMAL)
    {
        //dump buffers
        sp<ImageBufInfoMap> UTDump_ImgBufInfo = new ImageBufInfoMap(request);

        ADD_ENQUE_BUFF_TO_INFO_MAP(UTDump_ImgBufInfo, pEnqueBufPool, BID_FE2_IN_MAIN1);
        ADD_ENQUE_BUFF_TO_INFO_MAP(UTDump_ImgBufInfo, pEnqueBufPool, BID_FE2_IN_MAIN2);
        ADD_ENQUE_BUFF_TO_INFO_MAP(UTDump_ImgBufInfo, pEnqueBufPool, BID_FE3_IN_MAIN1);
        ADD_ENQUE_BUFF_TO_INFO_MAP(UTDump_ImgBufInfo, pEnqueBufPool, BID_FE3_IN_MAIN2);
        handleDump(UT_OUT_FE, UTDump_ImgBufInfo);
    }

    #endif


    // if capture
    if(state == STATE_CAPTURE)
    {
        // pass to WMF node
        sp<ImageBufInfoMap> TO_WMF_ImgBufInfo = new ImageBufInfoMap(request);
        // because the BID_P2AFM_FE1CO_INPUT is also the BID_TO_WMF_MY_SL
        ADD_ENQUE_BUFF_TO_INFO_MAP(TO_WMF_ImgBufInfo, pEnqueBufPool, BID_P2AFM_FE1CO_INPUT);
        handleDataAndDump(P2AFM_TO_WMF_MY_SL, TO_WMF_ImgBufInfo);

        // pass to FD node
        sp<ImageBufInfoMap> TO_FD_ImgBufInfo = new ImageBufInfoMap(request);
        ADD_ENQUE_BUFF_TO_INFO_MAP(TO_FD_ImgBufInfo, pEnqueBufPool, BID_TO_FD_IMG);
        handleDataAndDump(P2AFM_TO_FD_IMG, TO_FD_ImgBufInfo);
    }

    delete pEnqueBufPool;
    VSDOF_PRFLOG("- :reqID=%d", request->getRequestNo());

    // mark on-going-request end
    this->decExtThreadDependency();
}

/******************************************************************************
 *
 ******************************************************************************/
MBOOL
P2AFMNode::
onThreadStart()
{
    return MTRUE;
}

MBOOL
P2AFMNode::
onThreadStop()
{
    return MTRUE;
}

MVOID
P2AFMNode::
setSensorIdx(MUINT32 main1_idx, MUINT32 main2_idx)
{
    VSDOF_LOGD("setSensorIdx main1=%d main2=%d", main1_idx, main2_idx);
    miSensorIdx_Main1 = main1_idx;
    miSensorIdx_Main2 = main2_idx;
}


NSCam::NSIoPipe::PortID
P2AFMNode::
mapToPortID(const MUINT32 nodeDataType)
{
    switch(nodeDataType)
    {
        case BID_P2AFM_IN_RSRAW1:
        case BID_P2AFM_IN_RSRAW2:
        case BID_P2AFM_IN_FSRAW:
            return PORT_IMGI;
            break;

        case BID_OUT_FD:
        case BID_TO_FD_IMG:
        case BID_P2AFM_FE1CO_INPUT:
        case BID_P2AFM_FE2CO_INPUT:
            return PORT_IMG2O;
            break;




        case BID_TO_N3D_FE1AO:
        case BID_TO_N3D_FE2AO:
        case BID_TO_N3D_FE1BO:
        case BID_TO_N3D_FE2BO:
        case BID_TO_N3D_FE1CO:
        case BID_TO_N3D_FE2CO:
            return PORT_FEO;
            break;


        case BID_TO_N3D_RECT_IN1:
        case BID_TO_N3D_RECT_IN2:
        case BID_TO_N3D_RECT_IN2_CAP:
        case BID_TO_N3D_RECT_IN1_CAP:
        case BID_OUT_MV_F:
        case BID_OUT_MV_F_CAP:
            return PORT_WDMA;
            break;


        case BID_P2AFM_FE1BO_INPUT:
        case BID_P2AFM_FE2BO_INPUT:
            return PORT_WROT;

        case BID_TO_N3D_CC_IN1:
        case BID_TO_N3D_CC_IN2:
            //TODO: restore CC_in to port IMG2O
            //return PORT_IMG2O;
            return PORT_WDMA;

        case BID_TO_N3D_FMBO_LR:
        case BID_TO_N3D_FMBO_RL:
        case BID_TO_N3D_FMCO_LR:
        case BID_TO_N3D_FMCO_RL:
            return PORT_MFBO;

        default:
            MY_LOGE("mapToPortID: not exist nodeDataType=%d", nodeDataType);
            break;
    }

    return NSCam::NSIoPipe::PortID();
}

MVOID P2AFMNode::debugQParams(const QParams& rInputQParam)
{
    if(!mbDebugLog)
        return;

    MY_LOGD("debugQParams start");
    MY_LOGD("Size of mvStreamTag=%d, mvTuningData=%d mvIn=%d mvOut=%d, mvCropRsInfo=%d, mvModuleData=%d\n",
                rInputQParam.mvStreamTag.size(), rInputQParam.mvTuningData.size(), rInputQParam.mvIn.size(),
                rInputQParam.mvOut.size(), rInputQParam.mvCropRsInfo.size(), rInputQParam.mvModuleData.size());

    MY_LOGD("mvStreamTag section");
    for(int i=0;i<rInputQParam.mvStreamTag.size(); i++)
    {
        MY_LOGD("Index = %d mvStreamTag = %d", i, rInputQParam.mvStreamTag[i]);
    }

    MY_LOGD("mvTuningData section");
    for(int i=0;i<rInputQParam.mvTuningData.size(); i++)
    {
        dip_x_reg_t* data = (dip_x_reg_t*) rInputQParam.mvTuningData[i];
        MY_LOGD("========\nIndex = %d", i);

        MY_LOGD("DIP_X_FE_CTRL1.Raw = %x", data->DIP_X_FE_CTRL1.Raw);
        MY_LOGD("DIP_X_FE_IDX_CTRL.Raw = %x", data->DIP_X_FE_IDX_CTRL.Raw);
        MY_LOGD("DIP_X_FE_CROP_CTRL1.Raw = %x", data->DIP_X_FE_CROP_CTRL1.Raw);
        MY_LOGD("DIP_X_FE_CROP_CTRL2.Raw = %x", data->DIP_X_FE_CROP_CTRL2.Raw);
        MY_LOGD("DIP_X_FE_CTRL2.Raw = %x", data->DIP_X_FE_CTRL2.Raw);
        MY_LOGD("DIP_X_FM_SIZE.Raw = %x", data->DIP_X_FM_SIZE.Raw);
        MY_LOGD("DIP_X_FM_TH_CON0.Raw = %x", data->DIP_X_FM_TH_CON0.Raw);
    }

    MY_LOGD("mvIn section");
    for(int i=0;i<rInputQParam.mvIn.size(); i++)
    {
        Input data = rInputQParam.mvIn[i];
        MY_LOGD("========\nIndex = %d", i);

        MY_LOGD("mvIn.PortID.index = %d", data.mPortID.index);
        MY_LOGD("mvIn.PortID.type = %d", data.mPortID.type);
        MY_LOGD("mvIn.PortID.inout = %d", data.mPortID.inout);
        MY_LOGD("mvIn.PortID.group = %d", data.mPortID.group);

        MY_LOGD("mvIn.mBuffer=%x", data.mBuffer);
        if(data.mBuffer !=NULL)
        {
            MY_LOGD("mvIn.mBuffer->getImgSize = %dx%d", data.mBuffer->getImgSize().w,
                                                data.mBuffer->getImgSize().h);

            MY_LOGD("mvIn.mBuffer->getImgFormat = %x", data.mBuffer->getImgFormat());
            MY_LOGD("mvIn.mBuffer->getPlaneCount = %d", data.mBuffer->getPlaneCount());
            for(int j=0;j<data.mBuffer->getPlaneCount();j++)
            {
                MY_LOGD("mvIn.mBuffer->getFD(%d) = %d", j, data.mBuffer->getFD(j));
                MY_LOGD("mvIn.mBuffer->getBufVA(%d) = %X", j, data.mBuffer->getBufVA(j));
                MY_LOGD("mvIn.mBuffer->getBufStridesInBytes(%d) = %d", j, data.mBuffer->getBufStridesInBytes(j));
            }
        }
        else
        {
            MY_LOGD("mvIn.mBuffer is NULL!!");
        }


        MY_LOGD("mvIn.mTransform = %d", data.mTransform);

    }

    MY_LOGD("mvOut section");
    for(int i=0;i<rInputQParam.mvOut.size(); i++)
    {
        Output data = rInputQParam.mvOut[i];
        MY_LOGD("========\nIndex = %d", i);

        MY_LOGD("mvOut.PortID.index = %d", data.mPortID.index);
        MY_LOGD("mvOut.PortID.type = %d", data.mPortID.type);
        MY_LOGD("mvOut.PortID.inout = %d", data.mPortID.inout);
        MY_LOGD("mvOut.PortID.group = %d", data.mPortID.group);

        MY_LOGD("mvOut.mBuffer=%x", data.mBuffer);
        if(data.mBuffer != NULL)
        {
            MY_LOGD("mvOut.mBuffer->getImgSize = %dx%d", data.mBuffer->getImgSize().w,
                                                data.mBuffer->getImgSize().h);

            MY_LOGD("mvOut.mBuffer->getImgFormat = %x", data.mBuffer->getImgFormat());
            MY_LOGD("mvOut.mBuffer->getPlaneCount = %d", data.mBuffer->getPlaneCount());
            for(int j=0;j<data.mBuffer->getPlaneCount();j++)
            {
                MY_LOGD("mvOut.mBuffer->getFD(%d) = %d", j, data.mBuffer->getFD(j));
                MY_LOGD("mvOut.mBuffer->getBufVA(%d) = %X", j, data.mBuffer->getBufVA(j));
                MY_LOGD("mvOut.mBuffer->getBufStridesInBytes(%d) = %d", j, data.mBuffer->getBufStridesInBytes(j));
            }
        }
        else
        {
            MY_LOGD("mvOut.mBuffer is NULL!!");
        }


        MY_LOGD("mvOut.mTransform = %d", data.mTransform);
    }

    MY_LOGD("mvCropRsInfo section");
    for(int i=0;i<rInputQParam.mvCropRsInfo.size(); i++)
    {
        MCrpRsInfo data = rInputQParam.mvCropRsInfo[i];
        MY_LOGD("========\nIndex = %d", i);

        MY_LOGD("CropRsInfo.mGroupID=%d", data.mGroupID);
        MY_LOGD("CropRsInfo.mFrameGroup=%d", data.mFrameGroup);
        MY_LOGD("CropRsInfo.mResizeDst=%dx%d", data.mResizeDst.w, data.mResizeDst.h);
        MY_LOGD("CropRsInfo.mCropRect.p_fractional=(%d,%d) ", data.mCropRect.p_fractional.x, data.mCropRect.p_fractional.y);
        MY_LOGD("CropRsInfo.mCropRect.p_integral=(%d,%d) ", data.mCropRect.p_integral.x, data.mCropRect.p_integral.y);
        MY_LOGD("CropRsInfo.mCropRect.s=%dx%d ", data.mCropRect.s.w, data.mCropRect.s.h);
    }

    MY_LOGD("mvModuleData section");
    for(int i=0;i<rInputQParam.mvModuleData.size(); i++)
    {
        ModuleInfo data = rInputQParam.mvModuleData[i];
        MY_LOGD("========\nIndex = %d", i);

        MY_LOGD("ModuleData.moduleTag=%d", data.moduleTag);
        MY_LOGD("ModuleData.frameGroup=%d", data.frameGroup);

        _SRZ_SIZE_INFO_ *SrzInfo = (_SRZ_SIZE_INFO_ *) data.moduleStruct;
        MY_LOGD("SrzInfo->in_w=%d", SrzInfo->in_w);
        MY_LOGD("SrzInfo->in_h=%d", SrzInfo->in_h);
        MY_LOGD("SrzInfo->crop_w=%d", SrzInfo->crop_w);
        MY_LOGD("SrzInfo->crop_h=%d", SrzInfo->crop_h);
        MY_LOGD("SrzInfo->crop_x=%d", SrzInfo->crop_x);
        MY_LOGD("SrzInfo->crop_y=%d", SrzInfo->crop_y);
        MY_LOGD("SrzInfo->crop_floatX=%d", SrzInfo->crop_floatX);
        MY_LOGD("SrzInfo->crop_floatY=%d", SrzInfo->crop_floatY);
        MY_LOGD("SrzInfo->out_w=%d", SrzInfo->out_w);
        MY_LOGD("SrzInfo->out_h=%d", SrzInfo->out_h);
    }
}

}; //NSFeaturePipe
}; //NSCamFeature
}; //NSCam

