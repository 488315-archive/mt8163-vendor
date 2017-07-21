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

#include "P2ANode.h"
#include <iopipe/CamIO/IHalCamIO.h>
#include <mtkcam/feature/include/common/eis/eis_hal.h>

#define PIPE_CLASS_TAG "P2ANode"
#define PIPE_TRACE TRACE_P2A_NODE
#include <featurePipe/core/include/PipeLog.h>

using NSCam::NSIoPipe::NSPostProc::ENormalStreamTag_Normal;
using NSCam::NSIoPipe::NSPostProc::ENormalStreamTag_FE;
using NSCam::NSIoPipe::NSPostProc::Input;
using NSCam::NSIoPipe::NSPostProc::Output;
using NSImageio::NSIspio::EPortIndex_LCEI;
using NSImageio::NSIspio::EPortIndex_IMG3O;
using NSImageio::NSIspio::EPortIndex_WDMAO;
using NSImageio::NSIspio::EPortIndex_WROTO;
using NSImageio::NSIspio::EPortIndex_VIPI;

// debug usage
// === dump imaage: start ===
#define USE_VIPI_INPUT_TEST
//#define USE_MY_CROPINFO
#define USE_RAY_CROPINFO
// === dump imaage: end

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {

P2ANode::P2ANode(const char *name)
    : StreamingFeatureNode(name)
    , mNormalStream(NULL)
    , mpEisHal(NULL)
    , mp3dnr(NULL)
{
    TRACE_FUNC_ENTER();
    this->addWaitQueue(&mRequests);

    // 3dnr debug level: 0 for no logging, non-Zero: logging
    m3dnrLogLevel = getPropertyValue("camera.3dnr.log.level", 0);
    m3dnrDebugLevel = getPropertyValue("camera.3dnr.debug.level", 0);

    TRACE_FUNC_EXIT();
}

P2ANode::~P2ANode()
{
    TRACE_FUNC_ENTER();
    TRACE_FUNC_EXIT();
}

MVOID P2ANode::setNormalStream(NSCam::NSIoPipe::NSPostProc::INormalStream *stream)
{
    TRACE_FUNC_ENTER();
    mNormalStream = stream;
    TRACE_FUNC_EXIT();
}

MVOID P2ANode::setDsImgPool(android::sp<ImageBufferPool> pool)
{
    TRACE_FUNC_ENTER();
    mDsImgPool = pool;
    TRACE_FUNC_EXIT();
}

MVOID P2ANode::setFullImgPool(android::sp<GraphicBufferPool> pool)
{
    TRACE_FUNC_ENTER();
    mFullImgPool = pool;
    TRACE_FUNC_EXIT();
}

MBOOL P2ANode::onInit()
{
    TRACE_FUNC_ENTER();

    StreamingFeatureNode::onInit();

    init3DNR();

    initVHDR();

    initP2();

    TRACE_FUNC_EXIT();
    return MTRUE;
}

MBOOL P2ANode::onUninit()
{
    TRACE_FUNC_ENTER();

    uninit3DNR();

    uninitVHDR();

    uninitP2();

    TRACE_FUNC_EXIT();
    return MTRUE;
}

MBOOL P2ANode::onThreadStart()
{
    TRACE_FUNC_ENTER();
    TRACE_FUNC_EXIT();
    return MTRUE;
}

MBOOL P2ANode::onThreadStop()
{
    TRACE_FUNC_ENTER();
    waitEnqueQParamsDone();
    TRACE_FUNC_EXIT();
    return MTRUE;
}

MBOOL P2ANode::onData(DataID id, const RequestPtr &data)
{
    TRACE_FUNC_ENTER();
    TRACE_FUNC("Frame %d: %s arrived", data->mRequestNo, ID2Name(id));
    MBOOL ret = MFALSE;

    switch( id )
    {
    case ID_ROOT_ENQUE:
        mRequests.enque(data);
        ret = MTRUE;
        break;
    default:
        ret = MFALSE;
        break;
    }

    TRACE_FUNC_EXIT();
    return ret;
}

MBOOL P2ANode::onThreadLoop()
{
    TRACE_FUNC("Waitloop");
    RequestPtr request;

    if( !waitAllQueue() )
    {
        return MFALSE;
    }
    if( !mRequests.deque(request) )
    {
        MY_LOGE("Request deque out of sync");
        return MFALSE;
    }
    else if( request == NULL )
    {
        MY_LOGE("Request out of sync");
        return MFALSE;
    }
    TRACE_FUNC_ENTER();

    request->mTimer.startP2A();
    MY_LOGD("Frame %d in P2A, feature=0x%04x", request->mRequestNo, request->mFeatureMask);
    processP2A(request);
    request->mTimer.stopP2A();

    TRACE_FUNC_EXIT();
    return MTRUE;
}

MBOOL P2ANode::processP2A(const RequestPtr &request)
{
    TRACE_FUNC_ENTER();
    P2AEnqueData data;
    QParams param;
    MRect postCropSize;
    MCrpRsInfo P2AOutCropInfo;

    data.mRequest = request;

    calcSizeInfo(request);

    prepareQParams(param, request);

    prepare3A(param, request);

    if( request->need3DNR() )
    {
        prepare3DNR(param, request);
    }
    if( request->needEIS() )
    {
        prepareEIS(param, request, P2AOutCropInfo);
    }

    prepareIO(param, request, data);
    prepareCropInfo(param, request, data);

 //   dump_imgiImageBuffer(param);
 //   dump_Qparam(param, "sss_");

    enqueFeatureStream(param, data);

    TRACE_FUNC_EXIT();
    return MTRUE;
}

MBOOL P2ANode::calcSizeInfo(const RequestPtr &request)
{
    TRACE_FUNC_ENTER();

    request->mFullImgSize = request->getInputSize();
    request->mFullImgCrop = MCropRect(MPoint(0, 0), request->mFullImgSize);
    request->mMDPCrop = request->getP2Crop();

    if( request->needEIS() )
    {
#if 0
        MUINT32 fullWidth,fullHeight;
        MUINT32 eisFeBlock  = 0;
        MUINT32 i;

        fullWidth = request->mvIn[0].mBuffer->getImgSize().w;
        fullHeight = request->mvIn[0].mBuffer->getImgSize().h;
        MY_LOGD("[EIS] Full W/H(%u,%u)",fullWidth,fullHeight);
        MRect srcRect(MPoint(0, 0), MSize(fullWidth, fullHeight));
        MRect eisCropRect;

        MUINT32 eisCrop2Idx = 0;
        for( i = 0; i < request->mvCropRsInfo.size(); ++i)
        {
            if( request->mvCropRsInfo[i].mGroupID == 2 ) //GroupID=2 means the crop info is set for Video recording
            {
                eisCrop2Idx = i;
            }
        }

        //Important!!!
        //TBD: It should use the crop info without "Digital Zoom"!!!
        eisCropRect = MRect(request->mvCropRsInfo[eisCrop2Idx].mCropRect.p_integral, request->mvCropRsInfo[eisCrop2Idx].mCropRect.s);

#if 0
        MY_LOGD("[EIS]Idx(%u),ROI(%u,%u,%u,%u),ROI_noDZ(%u,%u,%u,%u),Dst(0,0,%u,%u),Crop(%u,%u,%u,%u)",eisCrop2Idx,
                                                                                 request->mvCropRsInfo[eisCrop2Idx].mCropRect.p_integral.x,
                                                                                 request->mvCropRsInfo[eisCrop2Idx].mCropRect.p_integral.y,
                                                                                 request->mvCropRsInfo[eisCrop2Idx].mCropRect.s.w,
                                                                                 request->mvCropRsInfo[eisCrop2Idx].mCropRect.s.h,
                                                                                 request->mvCropRsInfo[eisCrop2Idx].mCropRectExceptDZ.p_integral.x,
                                                                                 request->mvCropRsInfo[eisCrop2Idx].mCropRectExceptDZ.p_integral.y,
                                                                                 request->mvCropRsInfo[eisCrop2Idx].mCropRectExceptDZ.s.w,
                                                                                 request->mvCropRsInfo[eisCrop2Idx].mCropRectExceptDZ.s.h,
                                                                                 mEISDispW,
                                                                                 mEISDispH,
                                                                                 eisCropRect.x,
                                                                                 eisCropRect.y,
                                                                                 eisCropRect.w,
                                                                                 eisCropRect.h);

#endif

        if( eisCropRect.s.w <= D1_WIDTH && eisCropRect.s.h <= D1_HEIGHT )
        {
            eisFeBlock = 8;
        }
        else if( eisCropRect.s.w <= EIS_FE_MAX_INPUT_W && eisCropRect.s.h <= EIS_FE_MAX_INPUT_H )
        {
            eisFeBlock = 16;
        }
        else
        {
            eisFeBlock = 32;
            MY_LOGW("FE should not use 32");
        }

        eisCropRect.s.w = ALIGN_SIZE(eisCropRect.s.w,eisFeBlock);
        eisCropRect.s.h = ALIGN_SIZE(eisCropRect.s.h,eisFeBlock);
        request->mFullImgSize = eisCropRect.s;
        request->mFullImgCrop = eisCropRect;

#endif
    }

    TRACE_FUNC_EXIT();
    return MTRUE;
}

MBOOL P2ANode::onQParamsCB(const QParams &params, const P2AEnqueData &data)
{
    // This function is not thread safe,
    // avoid accessing P2ANode class members
    TRACE_FUNC_ENTER();
    MBOOL ret = MTRUE;
    //mEnqueMutex.unlock();

    RequestPtr request = data.mRequest;
    if( request == NULL )
    {
        MY_LOGE("Missing request");
        ret = MFALSE;
    }
    else
    {
        request->mTimer.stopEnqueP2A();
        MY_LOGD("Frame %d enque done in %d ms, result = %d", request->mRequestNo, request->mTimer.getElapsedEnqueP2A(), params.mDequeSuccess);

        if( !params.mDequeSuccess )
        {
            MY_LOGW("Frame %d enque result failed", request->mRequestNo);
        }

        request->mP2A_QParams = params;
        request->updateResult(params.mDequeSuccess);
        if( request->isLastNodeP2A() )
        {
            handleData(ID_P2A_TO_HELPER, request);
        }
        else if( request->needGPU() )
        {
            handleData(ID_P2A_TO_GPU_FULLIMG, ImgBufferData(data.mFullImg, request));
        }
        else if( request->needVFB() )
        {
            handleData(ID_P2A_TO_P2B_FULLIMG, ImgBufferData(data.mFullImg, request));
        }
        else
        {
            MY_LOGE("Why are you here?");
        }

        if( request->needVFB() )
        {
            handleData(ID_P2A_TO_FD_DSIMG, ImgBufferData(data.mDsImg, request));
            handleData(ID_P2A_TO_VFB_DSIMG, ImgBufferData(data.mDsImg, request));
        }
        if( request->needEIS() )
        {
            handleData(ID_P2A_TO_EIS_P2DONE, request);
        }
        if( request->needDump() )
        {
            if( data.mFullImg != NULL )
            {
                data.mFullImg->getImageBuffer()->syncCache(eCACHECTRL_INVALID);
                dumpData(data.mRequest, data.mFullImg->getImageBufferPtr(), "full");
            }
            if( data.mDsImg != NULL )
            {
                data.mDsImg->getImageBuffer()->syncCache(eCACHECTRL_INVALID);
                dumpData(data.mRequest, data.mDsImg->getImageBufferPtr(), "ds");
            }
        }
        request->mTimer.stopP2A();
    }

    this->decExtThreadDependency();
    TRACE_FUNC_EXIT();
    return ret;
}

MBOOL P2ANode::initP2()
{
    TRACE_FUNC_ENTER();
    MBOOL ret = MFALSE;
    if( mNormalStream != NULL )
    {
        ret = MTRUE;
    }

    mpEisHal = EisHal::CreateInstance("FeaturePipe_P2A", mSensorIndex);
    if (mpEisHal == NULL)
    {
        MY_LOGE("FeaturePipe_P2A CreateInstance failed");
        ret = MFALSE;
    }

    if(EIS_RETURN_NO_ERROR != mpEisHal->Init())
    {
        MY_LOGE("mpEisHal init failed");
        return MFALSE;
    }

    TRACE_FUNC_EXIT();
    return ret;
}

MVOID P2ANode::uninitP2()
{
    TRACE_FUNC_ENTER();
    if (mpEisHal)
    {
        mpEisHal->Uninit();
        mpEisHal->DestroyInstance("FeaturePipe_P2A");
        mpEisHal = NULL;
    }
    mNormalStream = NULL;
    mPrevFullImg = NULL;

    TRACE_FUNC_EXIT();
}

MBOOL P2ANode::prepareQParams(QParams &params, const RequestPtr &request)
{
    TRACE_FUNC_ENTER();
    params = request->mQParams;
    prepareStreamTag(params, request);
    TRACE_FUNC_EXIT();
    return MFALSE;
}

MCrpRsInfo P2ANode::calViewAngleCrop(QParams &params, const RequestPtr &request, MRect postCropSize)
{
    TRACE_FUNC_ENTER();

    MUINT32 PostProcOutWidth = 0, PostProcOutHeight = 0;
    MCrpRsInfo cropInfo;
    MUINT32 eisCrop2Idx = 0,i;

    for( i = 0; i < request->mvCropRsInfo.size(); ++i )
    {
        if( request->mvCropRsInfo[i].mGroupID == 2 )
        {
            eisCrop2Idx = i;
        }
    }

    PostProcOutWidth  = postCropSize.s.w;
    PostProcOutHeight = postCropSize.s.h;

    cropInfo = request->mvCropRsInfo[eisCrop2Idx];

    cropInfo.mGroupID = 1;
    cropInfo.mCropRect.p_integral.x = postCropSize.p.x;
    cropInfo.mCropRect.p_integral.y = postCropSize.p.y;
    cropInfo.mCropRect.s.w = PostProcOutWidth;
    cropInfo.mCropRect.s.h = PostProcOutHeight;
    cropInfo.mResizeDst.w  = PostProcOutWidth;
    cropInfo.mResizeDst.h  = PostProcOutHeight;

    TRACE_FUNC_EXIT();
    return cropInfo;
}

MBOOL P2ANode::needDigitalZoomCrop(const RequestPtr &request)
{
    return !(request->needEIS());
}

MBOOL P2ANode::prepareCropInfo(QParams &params, const RequestPtr &request, P2AEnqueData &data)
{
    TRACE_FUNC_ENTER();

    if( !request->isLastNodeP2A() )
    {
        params.mvCropRsInfo.clear();
    }
    if( request->needFullImg() )
    {
        MCrpRsInfo crop;
        crop.mGroupID = 1;
        crop.mCropRect = request->mFullImgCrop;
        crop.mResizeDst = request->mFullImgSize;


        MY_LOGD("crop.mCropRect :%d   %d   %d   %d", crop.mCropRect.p_integral.x, crop.mCropRect.p_fractional.x,
                                                     crop.mCropRect.p_integral.y, crop.mCropRect.p_fractional.y);
        // P2A using StreamTag_Normal does not support crz for IMG3O
        params.mvCropRsInfo.push_back(crop);
#if DEV_FULLIMG_FROM_MDP
        if( !request->isLastNodeP2A() )
        {
            crop.mGroupID = 3;
            params.mvCropRsInfo.push_back(crop);
        }
#endif // DEV_FULLIMG_FROM_MDP
    }
    if( request->needDsImg() )
    {
        MCrpRsInfo crop;
        crop.mGroupID = 2;
        crop.mCropRect = MCropRect(MPoint(0, 0), request->mFullImgSize);
        crop.mResizeDst = data.mDsImg->getImageBuffer()->getImgSize();
        params.mvCropRsInfo.push_back(crop);
    }

    for( unsigned i = 0, n = params.mvCropRsInfo.size(); i < n; ++i )
    {
        //params.mvCropRsInfo.editItemAt(i).mFrameGroup = request->mRequestNo;
    }

    TRACE_FUNC_EXIT();
    return MTRUE;
}

MBOOL P2ANode::prepareStreamTag(QParams &params, const RequestPtr &request)
{
    TRACE_FUNC_ENTER();
    if( request->needEIS() )
    {
        params.mvStreamTag.clear();
        params.mvStreamTag.push_back(ENormalStreamTag_Normal);
        //params.mvStreamTag.push_back(ENormalStreamTag_FE);
    }
    else if( request->needVFB() || request->need3DNR() )
    {
        params.mvStreamTag.clear();
        params.mvStreamTag.push_back(ENormalStreamTag_Normal);
    }
    TRACE_FUNC_EXIT();
    return MTRUE;
}

MBOOL P2ANode::prepareIO(QParams &params, const RequestPtr &request, P2AEnqueData &data)
{
    TRACE_FUNC_ENTER();
    MBOOL ret = MTRUE;
    //printRequestIO(request);
    params.mvIn = request->mQParams.mvIn;
    params.mvOut.clear();

    if( request->isLastNodeP2A() )
    {
        params.mvOut = request->mQParams.mvOut;
    }
    if( request->needFullImg() )
    {
        prepareFullImg(params, request, data);
    }
    if( request->needDsImg() )
    {
        prepareDsImg(params, request, data);
    }
    if( request->need3DNR() && mPrevFullImg != NULL)
    {
        prepareVIPI(params, request, data);
    }
    if( request->needVHDR() )
    {
        prepareLCEI(params, request, data);
    }
    mPrevFullImg = request->need3DNR() ? data.mFullImg : NULL;

    TRACE_FUNC_EXIT();
    return ret;
}

MVOID P2ANode::prepareFullImg(QParams &params, const RequestPtr &request, P2AEnqueData &data)
{
    TRACE_FUNC_ENTER();
    data.mFullImg = mFullImgPool->requestIIBuffer();
    data.mFullImg->getImageBuffer()->setExtParam(request->mFullImgSize);
    Output output;
    output.mPortID = PortID(EPortType_Memory, EPortIndex_IMG3O, PORTID_OUT);
#if DEV_FULLIMG_FROM_MDP
    if( !request->isLastNodeP2A() )
    {
        output.mPortID = PortID(EPortType_Memory, EPortIndex_WROTO, PORTID_OUT);
    }
#endif // DEV_FULLIMG_FROM_MDP
    output.mBuffer = data.mFullImg->getImageBufferPtr();
    output.mBuffer->setTimestamp(request->mQParams.mvIn[0].mBuffer->getTimestamp());
    params.mvOut.push_back(output);
    TRACE_FUNC_EXIT();
}

MVOID P2ANode::prepareDsImg(QParams &params, const RequestPtr &request, P2AEnqueData &data)
{
    TRACE_FUNC_ENTER();
    MSize dsSize;
    dsSize = calcDsImgSize(request->mFullImgSize);
    MY_LOGD("DsImg size from %dx%d to %dx%d", MSize_ARG(request->mFullImgSize), MSize_ARG(dsSize));
    data.mDsImg = mDsImgPool->requestIIBuffer();
    data.mDsImg->getImageBuffer()->setExtParam(dsSize);
    Output output;
    output.mPortID = PortID(EPortType_Memory, EPortIndex_WDMAO, PORTID_OUT);
    output.mBuffer = data.mDsImg->getImageBufferPtr();
    params.mvOut.push_back(output);
    TRACE_FUNC_EXIT();
}

MVOID P2ANode::prepareVIPI(QParams &params, const RequestPtr &request, P2AEnqueData &data)
{
    TRACE_FUNC_ENTER();
    data.mPrevFullImg = mPrevFullImg;
    Input input;
    input.mPortID = PortID(EPortType_Memory, EPortIndex_VIPI, PORTID_IN);
    input.mBuffer = data.mPrevFullImg->getImageBufferPtr();
    params.mvIn.push_back(input);
    TRACE_FUNC_EXIT();
}

MVOID P2ANode::prepareLCEI(QParams &params, const RequestPtr &request, P2AEnqueData &data)
{
    TRACE_FUNC_ENTER();
    prepareVHDR(params, request);
    TRACE_FUNC_EXIT();
}

MVOID P2ANode::enqueFeatureStream(NSCam::NSIoPipe::NSPostProc::QParams &params, P2AEnqueData &data)
{
    TRACE_FUNC_ENTER();
    MBOOL ret;
    //mEnqueMutex.lock();
    MY_LOGD("Frame %d enque start", data.mRequest->mRequestNo);
    data.mRequest->mTimer.startEnqueP2A();
    ret = this->enqueQParams(mNormalStream, params, data);
    this->incExtThreadDependency();
    if( !ret )
    {
        MY_LOGE("Frame %d enque failed", data.mRequest->mRequestNo);
        data.mRequest->updateResult(MFALSE);
        params.mDequeSuccess = MFALSE;
        P2ANode::onQParamsCB(params, data);
    }
    TRACE_FUNC_EXIT();
}

MVOID P2ANode::printRequestIO(const RequestPtr &request)
{
  for( unsigned i = 0, n = request->mQParams.mvIn.size(); i < n; ++i )
  {
      unsigned index = request->mQParams.mvIn[i].mPortID.index;
      MSize size = request->mQParams.mvIn[i].mBuffer->getImgSize();
      MY_LOGD("Frame %d mvIn[%d] idx=%d size=(%d,%d)", request->mRequestNo, i, index, size.w, size.h);
  }
  for( unsigned i = 0, n = request->mQParams.mvOut.size(); i < n; ++i )
  {
      unsigned index = request->mQParams.mvOut[i].mPortID.index;
      MSize size = request->mQParams.mvOut[i].mBuffer->getImgSize();
      //MBOOL isGraphic = (getGraphicBufferAddr(request->mQParams.mvOut[i].mBuffer) != NULL);
      MBOOL isGraphic = MFALSE;
      MINT fmt = request->mQParams.mvOut[i].mBuffer->getImgFormat();
      MY_LOGD("Frame %d mvOut[%d] idx=%d size=(%d,%d) fmt=%d, isGraphic=%d", request->mRequestNo, i, index, size.w, size.h, fmt, isGraphic);
  }
  for( unsigned i = 0, n = request->mQParams.mvCropRsInfo.size(); i < n; ++i )
  {
      MCrpRsInfo crop = request->mQParams.mvCropRsInfo[i];
      MY_LOGD("Frame %d crop[%d] " MCrpRsInfo_STR, request->mRequestNo, i,
          MCrpRsInfo_ARG(crop));
  }
}

MBOOL P2ANode::init3A()
{
    TRACE_FUNC_ENTER();
    TRACE_FUNC_EXIT();
    return MTRUE;
}

MVOID P2ANode::uninit3A()
{
    TRACE_FUNC_ENTER();
    TRACE_FUNC_EXIT();
}

MBOOL P2ANode::prepare3A(NSCam::NSIoPipe::NSPostProc::QParams &params, const RequestPtr &request)
{
    TRACE_FUNC_ENTER();
    (void)params;
    (void)request;
    TRACE_FUNC_EXIT();
    return MTRUE;
}

MBOOL P2ANode::initEIS()
{
    TRACE_FUNC_ENTER();
    TRACE_FUNC_EXIT();
    return MTRUE;
}

MVOID P2ANode::uninitEIS()
{
}

MBOOL P2ANode::prepareEIS(NSCam::NSIoPipe::NSPostProc::QParams &params, const RequestPtr &request, const MCrpRsInfo outCropInfo)
{
    TRACE_FUNC_ENTER();
    MUINT32 PostProcOutWidth = 0, PostProcOutHeight = 0;
    MUINT32 i;
    EIS_HAL_CONFIG_DATA eisHalConfigData;


    PostProcOutWidth  = request->mFullImgSize.w;
    PostProcOutHeight = request->mFullImgSize.h;

    eisHalConfigData.gpuTargetW = PostProcOutWidth/(mpEisHal->GetEisPlusCropRatio()/100.0f);
    eisHalConfigData.gpuTargetH = PostProcOutHeight/(mpEisHal->GetEisPlusCropRatio()/100.0f);

#define ROUND_TO_2X(x) ((x) & (~0x1))

    eisHalConfigData.gpuTargetW = ROUND_TO_2X(eisHalConfigData.gpuTargetW);
    eisHalConfigData.gpuTargetH = ROUND_TO_2X(eisHalConfigData.gpuTargetH);

#undef ROUND_TO_2X

#if 1
    eisHalConfigData.crzOutW = request->mFullImgSize.w;
    eisHalConfigData.crzOutH = request->mFullImgSize.h;
    eisHalConfigData.srzOutW = request->mFullImgSize.w; //TODO : need to modified for stereo
    eisHalConfigData.srzOutH = request->mFullImgSize.h; //TODO : need to modified for stereo
    eisHalConfigData.feTargetW = request->mFullImgSize.w; //TODO : need to modified for stereo
    eisHalConfigData.feTargetH = request->mFullImgSize.h; //TODO : need to modified for stereo

    eisHalConfigData.cropX   = request->mFullImgCrop.p_integral.x;
    eisHalConfigData.cropY   = request->mFullImgCrop.p_integral.y;


    eisHalConfigData.imgiW = request->mvIn[0].mBuffer->getImgSize().w;
    eisHalConfigData.imgiH = request->mvIn[0].mBuffer->getImgSize().h;
    eisHalConfigData.configSce = EIS_SCE_EIS_PLUS;

    MY_LOGD("[ConfigEis] imgi(%u,%u),crz(%u,%u),srz(%u,%u),feT(%u,%u),gpuT(%u,%u),crop(%u,%u),sce(%d)",eisHalConfigData.imgiW,
                                                                                                       eisHalConfigData.imgiH,
                                                                                                       eisHalConfigData.crzOutW,
                                                                                                       eisHalConfigData.crzOutH,
                                                                                                       eisHalConfigData.srzOutW,
                                                                                                       eisHalConfigData.srzOutH,
                                                                                                       eisHalConfigData.feTargetW,
                                                                                                       eisHalConfigData.feTargetH,
                                                                                                       eisHalConfigData.gpuTargetW,
                                                                                                       eisHalConfigData.gpuTargetH,
                                                                                                       eisHalConfigData.cropX,
                                                                                                       eisHalConfigData.cropY,
                                                                                                       eisHalConfigData.configSce);

#endif
    if( EIS_RETURN_NO_ERROR != mpEisHal->ConfigEis(EIS_PASS_2,eisHalConfigData) )
    {
        MY_LOGE("P2A ConfigEis fail");
        return MFALSE;
    }


    handleData(ID_P2A_TO_EIS_CONFIG, EisConfigData(eisHalConfigData, request));
    TRACE_FUNC_EXIT();
    return MTRUE;
}

MBOOL P2ANode::init3DNR()
{
    TRACE_FUNC_ENTER();

    mp3dnr = NSCam::NSIoPipe::NSPostProc::hal3dnrBase::createInstance();
    if (mp3dnr)
    {
        mp3dnr->init();
        m3dnrInitedSuccess = MTRUE;
    }

    TRACE_FUNC_EXIT();
    return MTRUE;
}

MVOID P2ANode::uninit3DNR()
{
    TRACE_FUNC_ENTER();
    mPrevFullImg = NULL;

    {
        Mutex::Autolock _l(m3dnrLock);
        if(mp3dnr) {
            mp3dnr->destroyInstance();
            mp3dnr = NULL;
        }
    }
    TRACE_FUNC_EXIT();
}

MBOOL P2ANode::prepare3DNR(NSCam::NSIoPipe::NSPostProc::QParams &params, const RequestPtr &request)
{
    TRACE_FUNC_ENTER();

    // === prepare cropInfo ===
//    Mutex::Autolock _l(m3dnrLock);

    MBOOL result = MTRUE;
    // 1. prepare all info
    // MBOOL prepare3DNR(
    //     NSCam::NSIoPipe::NSPostProc::QParams &params,
    //      crop_infos const& cropInfo, // TODO: try
    //      MINT32 iso,  // TODO: try 200, 800,
    //      IImageBuffer const *pIMGIBuffer,
    //      IImageBuffer *pIMG3OBuffer // TODO
    // );
    // [markInfo]: TODO: Crop, ISO are needed from upper layer

    // === cropInfo ===
#ifdef USE_MY_CROPINFO // get it from VarMap
    MRect postCropSize;
    postCropSize.s.w = postCropSize.s.h = 0;

    postCropSize.s.w = request->getVar<MUINT32>("3dnr.dstsize_resizer.w", postCropSize.s.w);
    postCropSize.s.h = request->getVar<MUINT32>("3dnr.dstsize_resizer.h", postCropSize.s.h);
    MY_LOGD_IF(m3dnrLogLevel >= 2, "aaaa_cropInfo_test(mark): w=%d, h=%d", postCropSize.s.w, postCropSize.s.h);
#endif

#ifdef USE_RAY_CROPINFO
    MRect postCropSize;
//    postCropSize.s.w = postCropSize.s.h = 0;
//    postCropSize = calcPostCropSize(request);
    postCropSize.s  = request->mFullImgSize; // from CalcSizeInfo

    MY_LOGD_IF(m3dnrLogLevel >= 2, "aaaa_cropInfo_test(ray): w=%d, h=%d", postCropSize.s.w, postCropSize.s.h);
#endif

    // === prepare eis info ===
    MBOOL tmp_isEisOn = 0;
    MUINT32 tmp_gmvX = 0;
    MUINT32 tmp_gmvY = 0;
    MUINT32 tmp_x_int = 0;
    MUINT32 tmp_y_int = 0;

    eis_region tmpEisRegion;
    request->getVar<MBOOL>("3dnr.eis.isEisOn", tmp_isEisOn);
    if (tmp_isEisOn)
    {
        tmp_gmvX = request->getVar<MUINT32>("3dnr.eis.gmvX", tmp_gmvX);
        tmp_gmvY = request->getVar<MUINT32>("3dnr.eis.gmvY", tmp_gmvY);
        tmp_x_int = request->getVar<MUINT32>("3dnr.eis.x_int", tmp_x_int);
        tmp_y_int = request->getVar<MUINT32>("3dnr.eis.y_int", tmp_y_int);
    }
    else
    {
        tmp_gmvX = 0;
        tmp_gmvY = 0;
        tmp_x_int = 0;
        tmp_y_int = 0;
    }
    tmpEisRegion.gmvX = tmp_gmvX;
    tmpEisRegion.gmvY = tmp_gmvY;
    tmpEisRegion.x_int = tmp_x_int;
    tmpEisRegion.y_int = tmp_y_int;

    MY_LOGD_IF(m3dnrLogLevel >= 2, "aaaa_eis_test: isEisOn=%d, gmvX=%d, gmvY=%d, x_int=%d, y_int=%d",
    tmp_isEisOn, tmp_gmvX, tmp_gmvY, tmp_x_int, tmp_y_int);

    // prepare iso
    MINT32 iso = 200;
    iso = request->getVar<MUINT32>("3dnr.iso", iso);
    MY_LOGD_IF(m3dnrLogLevel >= 2, "aaaa_3dnr.iso: %d", iso);

    MBOOL res = do3dnrFlow(params, postCropSize, tmpEisRegion, iso);

    TRACE_FUNC_EXIT();
    return MTRUE;
}

} // namespace NSFeaturePipe
} // namespace NSCamFeature
} // namespace NSCam
