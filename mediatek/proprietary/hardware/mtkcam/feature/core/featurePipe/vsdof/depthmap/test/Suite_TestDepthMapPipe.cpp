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
#include <utils/include/ImageBufferHeap.h>
#include <mmsdk/EffectRequest.h>
#include <pic/imgi_2560x1440_bayer10.h>
#include <depthMap/DepthMapPipe_Common.h>

#include "TestDepthMapPipe_Common.h"

using android::sp;
using android::String8;
using namespace NSCam;

namespace NSCam{
namespace NSCamFeature{
namespace NSFeaturePipe{

const char *FULL_RAW_FILE_PATH = "/sdcard/P1_10_4176_3088_5220_0.raw";
const MSize FULL_RAW_SIZE = MSize(4176,3088);
MUINT32 FULL_RAW_STRIDES[3] = {5220, 0, 0};
MUINT32 FULL_RAW_BOUNDARY[3] = {0, 0, 0};

const IImageBufferAllocator::ImgParam imgParam_FullRaw(
    eImgFmt_BAYER10, FULL_RAW_SIZE, FULL_RAW_STRIDES, FULL_RAW_BOUNDARY, 1);

//
IImageBufferAllocator::ImgParam imgParam_MainImage = getImgParam_MV_F();
//
IImageBufferAllocator::ImgParam imgParam_MainImage_Cap = getImgParam_MV_F_CAP();
//
IImageBufferAllocator::ImgParam imgParam_FD = getImgParam_FD();
//
IImageBufferAllocator::ImgParam imgParam_DMW = getImgParam_DMW();
//
IImageBufferAllocator::ImgParam imgParam_MY_S = getImgParam_MY_S();
//
const MSize JPSMAIN_SIZE(2176,1152);
MUINT32 JSPMAIN_STRIDES[3] = {2176, 1088, 1088};

const IImageBufferAllocator::ImgParam imgParam_JPSMAIN(
    eImgFmt_YV12, JPSMAIN_SIZE, JSPMAIN_STRIDES, FULL_RAW_BOUNDARY, 3);

const MSize YUV01_SIZE(1920,1080);
MUINT32 YUV01_STRIDES[3] = {1920, 960, 960};

const IImageBufferAllocator::ImgParam imgParam_YUV01(
    eImgFmt_YV12, YUV01_SIZE, YUV01_STRIDES, FULL_RAW_BOUNDARY, 3);
//
const MSize YUV02_SIZE(1920,1080);
MUINT32 YUV02_STRIDES[3] = {1920, 960, 960};

const IImageBufferAllocator::ImgParam imgParam_YUV02(
    eImgFmt_YV12, YUV02_SIZE, YUV02_STRIDES, FULL_RAW_BOUNDARY, 3);
//
const MSize DEPMAP_SIZE(480,270);
MUINT32 DEPMAP_STRIDES[3] = {960, 0, 0};

const IImageBufferAllocator::ImgParam imgParam_DEPMAP(
    eImgFmt_YUY2, DEPMAP_SIZE, DEPMAP_STRIDES, FULL_RAW_BOUNDARY, 1);


IImageBuffer* createImgBufUsingExample()
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



void prepareRequest(sp<EffectRequest>& effectReq)
{
    MUINT32 reqID = effectReq->getRequestNo();
    // prepare input frame info
    sp<EffectFrameInfo> frameInfo = new EffectFrameInfo(reqID, BID_P2AFM_IN_RSRAW1);
    sp<IImageBuffer> buf = createImageBufferFromFile(imgParam_FullRaw, FULL_RAW_FILE_PATH, "ResizeRaw", eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
    //sp<IImageBuffer> buf = createImgBufUsingExample();
    MY_LOGD("DepthMapEffectHaleTest: ResizeRaw=%p frameInfo=%p", buf.get(), frameInfo.get());
    frameInfo->setFrameBuffer(buf);
    effectReq->vInputFrameInfo.add(BID_P2AFM_IN_RSRAW1, frameInfo);

    frameInfo = new EffectFrameInfo(reqID, BID_P2AFM_IN_RSRAW2);
    buf = createImageBufferFromFile(imgParam_FullRaw, FULL_RAW_FILE_PATH, "ResizeRaw2", eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN);
    //buf = createImgBufUsingExample();
    MY_LOGD("DepthMapEffectHaleTest: ResizeRaw222=%p frameInfo=%p", buf.get(), frameInfo.get());
    frameInfo->setFrameBuffer(buf);
    effectReq->vInputFrameInfo.add(BID_P2AFM_IN_RSRAW2, frameInfo);

    //prepare output frame info
    // Main Image
    frameInfo = new EffectFrameInfo(reqID, BID_OUT_MV_F);
    buf = createEmptyImageBuffer(imgParam_MainImage, "MainImg", eBUFFER_USAGE_HW_CAMERA_WRITE|eBUFFER_USAGE_SW_READ_OFTEN);
    frameInfo->setFrameBuffer(buf);
    effectReq->vOutputFrameInfo.add(BID_OUT_MV_F, frameInfo);
    // Main Image_Cap
    frameInfo = new EffectFrameInfo(reqID, BID_OUT_MV_F_CAP);
    buf = createEmptyImageBuffer(imgParam_MainImage_Cap, "MainImg_Cap", eBUFFER_USAGE_HW_CAMERA_WRITE|eBUFFER_USAGE_SW_READ_OFTEN);
    frameInfo->setFrameBuffer(buf);
    effectReq->vOutputFrameInfo.add(BID_OUT_MV_F_CAP, frameInfo);
    // DMW
    frameInfo = new EffectFrameInfo(reqID, BID_OUT_DMW);
    buf = createEmptyImageBuffer(imgParam_DMW, "DMW", eBUFFER_USAGE_HW_CAMERA_WRITE|eBUFFER_USAGE_SW_READ_OFTEN|eBUFFER_USAGE_SW_WRITE_OFTEN);
    frameInfo->setFrameBuffer(buf);
    effectReq->vOutputFrameInfo.add(BID_OUT_DMW, frameInfo);
    // MY_S
    frameInfo = new EffectFrameInfo(reqID, BID_OUT_MY_S);
    buf = createEmptyImageBuffer(imgParam_MY_S, "MY_S", eBUFFER_USAGE_HW_CAMERA_WRITE|eBUFFER_USAGE_SW_READ_OFTEN);
    frameInfo->setFrameBuffer(buf);
    effectReq->vOutputFrameInfo.add(BID_OUT_MY_S, frameInfo);
    // JPSMAIN1
    frameInfo = new EffectFrameInfo(reqID, BID_OUT_JPS_MAIN1);
    buf = createEmptyImageBuffer(imgParam_JPSMAIN, "JPSMAIN1", eBUFFER_USAGE_HW_CAMERA_WRITE|eBUFFER_USAGE_SW_READ_OFTEN);
    frameInfo->setFrameBuffer(buf);
    effectReq->vOutputFrameInfo.add(BID_OUT_JPS_MAIN1, frameInfo);
    // JPSMAIN2
    frameInfo = new EffectFrameInfo(reqID, BID_OUT_JPS_MAIN2);
    buf = createEmptyImageBuffer(imgParam_JPSMAIN, "JPSMAIN2", eBUFFER_USAGE_HW_CAMERA_WRITE|eBUFFER_USAGE_SW_READ_OFTEN);
    frameInfo->setFrameBuffer(buf);
    effectReq->vOutputFrameInfo.add(BID_OUT_JPS_MAIN2, frameInfo);
    // FD
    frameInfo = new EffectFrameInfo(reqID, BID_OUT_FD);
    buf = createEmptyImageBuffer(imgParam_FD, "FD", eBUFFER_USAGE_HW_CAMERA_WRITE|eBUFFER_USAGE_SW_READ_OFTEN);
    frameInfo->setFrameBuffer(buf);
    effectReq->vOutputFrameInfo.add(BID_OUT_FD, frameInfo);
    // YUV01
    /*frameInfo = new EffectFrameInfo(reqID, BID_YUV_OUT1);
    buf = createEmptyImageBuffer(imgParam_YUV01, "YUV01", eBUFFER_USAGE_HW_CAMERA_WRITE|eBUFFER_USAGE_SW_READ_OFTEN);
    frameInfo->setFrameBuffer(buf);
    effectReq->vOutputFrameInfo.add(BID_YUV_OUT1, frameInfo);*/
    // DEPTH MAP
    frameInfo = new EffectFrameInfo(reqID, BID_OUT_DEPTHMAP);
    buf = createEmptyImageBuffer(imgParam_DEPMAP, "DPEMAP", eBUFFER_USAGE_HW_CAMERA_WRITE|eBUFFER_USAGE_SW_READ_OFTEN);
    frameInfo->setFrameBuffer(buf);
    effectReq->vOutputFrameInfo.add(BID_OUT_DEPTHMAP, frameInfo);
}



TEST(DepthMapEffectHaleTest, Standard)
{
    DepthMapEffectHal depthEffectHal;

    int reqIdx = 0;

    MY_LOGD("DepthMapEffectHaleTest");
    MY_LOGD("depthEffectHal init!!");
    depthEffectHal.init();

    MY_LOGD("depthEffectHal set sensor index!");
    sp<EffectParameter> effParam = new EffectParameter();
    effParam->set(SENSOR_IDX_MAIN1, 0xFFFF);
    effParam->set(SENSOR_IDX_MAIN2, 0xFFFF);
    depthEffectHal.setParameters(effParam);

    MY_LOGD("depthEffectHal configure!!");
    depthEffectHal.configure();
    MY_LOGD("depthEffectHal start!!");
    depthEffectHal.start();

    sp<EffectRequest> effectReq = new EffectRequest(reqIdx, WaitingListener::CB);

    MY_LOGD("DepthMapEffectHaleTest: prepare request! pointer=%p", effectReq.get());

    prepareRequest(effectReq);


    MY_LOGD("DepthMapEffectHaleTest: insert request!");
    // enque EffectRequest
    depthEffectHal.updateEffectRequestImpl(effectReq);

    EXPECT_TRUE(WaitingListener::waitRequest(1,5));

    if(WaitingListener::vDoneReqVec.size() > 0)
    {
        sp<EffectRequest> resultReq = WaitingListener::vDoneReqVec[0];
        sp<EffectFrameInfo> frame = resultReq->vOutputFrameInfo.valueFor(BID_OUT_MV_F);

        sp<IImageBuffer> imgBuf;
        frame->getFrameBuffer(imgBuf);
        imgBuf->saveToFile("/sdcard/Pictures/VSDOF/out_1920x1080.yuv");
    }
    else
    {
        MY_LOGE("Cannot find done request");
    }

#ifdef SHOW_TEST_VERSION
  std::cout << "Finish FeaturePipe gtest: " TEST_VERSION_STR "\n";
#endif
}

}; // namespace NSFeaturePipe
}; // namespace NSCamFeature
}; // namespace NSCam

