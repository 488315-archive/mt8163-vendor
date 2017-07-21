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

#include "VFBNode.h"
#include "GpuWarpBase.h"

#define PIPE_CLASS_TAG "VFBNode"
#define PIPE_TRACE TRACE_VFB_NODE
#include <featurePipe/core/include/PipeLog.h>

#define VFB_WARP_WIDTH        320
#define VFB_WARP_HEIGHT       320
#define VFB_WARP_FORMAT       eImgFmt_BAYER8

#define VFB_DSIMG_WIDTH       320
#define VFB_DSIMG_HEIGHT      320
#define VFB_DSIMG_FORMAT      eImgFmt_YUY2

#define VFB_ALPHA_CL_WIDTH    320
#define VFB_ALPHA_CL_HEIGHT   320
#define VFB_ALPHA_CL_FORMAT   eImgFmt_BAYER8

#define VFB_ALPHA_NR_WIDTH    320
#define VFB_ALPHA_NR_HEIGHT   320
#define VFB_ALPHA_NR_FORMAT   eImgFmt_BAYER8

#define VFB_PCA_WIDTH         160
#define VFB_PCA_HEIGHT        27
#define VFB_PCA_FORMAT        eImgFmt_BAYER8

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {

VFBNode::VFBNode(const char *name)
    : StreamingFeatureNode(name)
    , mRecordingHint(MFALSE)
{
    TRACE_FUNC_ENTER();
    this->addWaitQueue(&mP2ADatas);
    this->addWaitQueue(&mFaceDatas);
    this->addWaitQueue(&mExtWarpDatas);
    TRACE_FUNC_EXIT();
}

VFBNode::~VFBNode()
{
    TRACE_FUNC_ENTER();
    TRACE_FUNC_EXIT();
}

MBOOL VFBNode::onData(DataID id, const ImgBufferData &data)
{
    TRACE_FUNC_ENTER();
    TRACE_FUNC("Frame %d: %s arrived", data.mRequest->mRequestNo, ID2Name(id));
    MBOOL ret = MFALSE;
    if( id == ID_P2A_TO_VFB_DSIMG )
    {
        mP2ADatas.enque(data);
        // enque dummy warp in case of no EIS
        if( !data.mRequest->needEIS() )
        {
            mExtWarpDatas.enque(ImgBufferData(ImgBuffer(), data.mRequest));
        }
        ret = MTRUE;
    }
    else if( id == ID_EIS_TO_VFB_WARP )
    {
        mExtWarpDatas.enque(data);
        ret = MTRUE;
    }
    TRACE_FUNC_EXIT();
    return ret;
}

MBOOL VFBNode::onData(DataID id, const FaceData &data)
{
    TRACE_FUNC_ENTER();
    TRACE_FUNC("Frame %d: %s arrived", data.mRequest->mRequestNo, ID2Name(id));
    MBOOL ret = MFALSE;
    if( id == ID_FD_TO_VFB_FACE )
    {
        mFaceDatas.enque(data);
        ret = MTRUE;
    }
    TRACE_FUNC_EXIT();
    return ret;
}

MBOOL VFBNode::onInit()
{
    TRACE_FUNC_ENTER();
    StreamingFeatureNode::onInit();
    IsVFB = true;

    /* Create vFB HAL instance */
    mpVfbHal = halVFBBase::createInstance(IsVFB);
    if (mpVfbHal == NULL)
    {
        MY_LOGE("halVFBBase create instance failed!");
        goto _Exit;
    }

    /* Init vFB HAL */
    struct MTKPipeVfbEnvInfo vfbInfo;
    vfbInfo.Features = 0;
    ADD_FEATURE(vfbInfo.Features, MTK_VFB_BIT);
    vfbInfo.ThreadPolicy = SCHED_POLICY;
    vfbInfo.ThreadPriority = SCHED_PRIORITY;
    vfbInfo.SrcImgWidth = FULL_IMAGE_MAX_WIDTH;
    vfbInfo.SrcImgHeight = FULL_IMAGE_MAX_HEIGHT;
    vfbInfo.DsImgWidth = VFB_DS_IMAGE_MAX_WIDTH;
    vfbInfo.DsImgHeight = VFB_DS_IMAGE_MAX_HEIGHT;
    vfbInfo.FDWidth = VFBFDWidth;
    vfbInfo.FDHeight = VFBFDHeight;
    vfbInfo.SrcImgFormat = eImgFmt_YUY2;
    vfbInfo.DsImgFormat = eImgFmt_YV16;
    vfbInfo.RecordingHint = mRecordingHint;
    MY_LOGE("mHalVFBInit %s full image %dx%d in %d format, down-scaled image %dx%d in %d format", (vfbInfo.RecordingHint)? "recording": "previewing",
             vfbInfo.SrcImgWidth, vfbInfo.SrcImgHeight, vfbInfo.SrcImgFormat, vfbInfo.DsImgWidth, vfbInfo.DsImgHeight, vfbInfo.DsImgFormat);
    if (mpVfbHal->mHalVFBInit(vfbInfo) == false)
    {
        MY_LOGE("mHalVFBInit init failed!");
        goto _Exit;
    }

    /* Allocate working memory */
    MINT32 size;
    size = mpVfbHal->mHalVFBGetWorkingBufSize();
    if (size == 0)
    {
        MY_LOGE("mHalVFBGetWorkingBufSize failed!");
        goto _Exit;
    }
    mpWorkingBuffer = (MUINT8*) malloc(size);
    if (mpWorkingBuffer == NULL)
    {
        MY_LOGE("vFB working buffer allocation failed!");
        goto _Exit;
    }
    if (mpVfbHal->mHalVFBSetWorkingBuf((void*) mpWorkingBuffer, size) == false)
    {
        MY_LOGE("mHalVFBSetWorkingBuf failed!");
        goto _Exit;
    }

    mWarpBufferPool = ImageBufferPool::create("VFB/warp",
                        VFB_WARP_WIDTH * 4, VFB_WARP_HEIGHT * 2,
                        VFB_WARP_FORMAT, ImageBufferPool::USAGE_SW);
    mDsImgBufferPool = ImageBufferPool::create("VFB/DS",
                        VFB_DSIMG_WIDTH, VFB_DSIMG_HEIGHT,
                        VFB_DSIMG_FORMAT, ImageBufferPool::USAGE_HW);
    mAlphaCLBufferPool = ImageBufferPool::create("VFB/CL",
                        VFB_ALPHA_CL_WIDTH, VFB_ALPHA_CL_HEIGHT,
                        VFB_ALPHA_CL_FORMAT, ImageBufferPool::USAGE_HW);
    mAlphaNRBufferPool = ImageBufferPool::create("VFB/NR",
                        VFB_ALPHA_NR_WIDTH, VFB_ALPHA_NR_HEIGHT,
                        VFB_ALPHA_NR_FORMAT, ImageBufferPool::USAGE_HW);
    mPCABufferPool = ImageBufferPool::create("VFB/PCA",
                        VFB_PCA_WIDTH, VFB_PCA_HEIGHT,
                        VFB_PCA_FORMAT, ImageBufferPool::USAGE_HW);

    mWarpBufferPool->allocate(3);
    mDsImgBufferPool->allocate(3);
    mAlphaCLBufferPool->allocate(3);
    mAlphaNRBufferPool->allocate(3);
    mPCABufferPool->allocate(3);

    //if (!FEATURE_MASK_IS_EIS_ENABLED(mFeatureOnMask))
    {
        /* Allocate zero-ed Warp Map */
        mpClearWarpMapX = (MINT32*) malloc(VFB_DS_IMAGE_MAX_WIDTH * VFB_DS_IMAGE_MAX_HEIGHT * 4);
        mpClearWarpMapY = (MINT32*) malloc(VFB_DS_IMAGE_MAX_WIDTH * VFB_DS_IMAGE_MAX_HEIGHT * 4);
        memset(mpClearWarpMapX, 0, VFB_DS_IMAGE_MAX_WIDTH * VFB_DS_IMAGE_MAX_HEIGHT * 4);
        memset(mpClearWarpMapY, 0, VFB_DS_IMAGE_MAX_WIDTH * VFB_DS_IMAGE_MAX_HEIGHT * 4);
    }

    TRACE_FUNC_EXIT();
    return MTRUE;

_Exit:
    if (mpVfbHal != NULL)
    {
        mpVfbHal->destroyInstance(IsVFB);
    }
    if (mpWorkingBuffer != NULL)
    {
        free(mpWorkingBuffer);
    }
    return false;

}

MBOOL VFBNode::onUninit()
{
    TRACE_FUNC_ENTER();

    mpVfbHal->mHalVFBUninit();
    mpVfbHal->destroyInstance(IsVFB);
    if (mpWorkingBuffer != NULL)
    {
        free(mpWorkingBuffer);
    }
    //if (!FEATURE_MASK_IS_EIS_ENABLED(mFeatureOnMask))
    {
        free(mpClearWarpMapX);
        free(mpClearWarpMapY);
    }

    ImageBufferPool::destroy(mWarpBufferPool);
    ImageBufferPool::destroy(mDsImgBufferPool);
    ImageBufferPool::destroy(mAlphaCLBufferPool);
    ImageBufferPool::destroy(mAlphaNRBufferPool);
    ImageBufferPool::destroy(mPCABufferPool);
    TRACE_FUNC_EXIT();
    return MTRUE;
}

MBOOL VFBNode::onThreadStart()
{
    TRACE_FUNC_ENTER();
    TRACE_FUNC_EXIT();
    return MTRUE;
}

MBOOL VFBNode::onThreadStop()
{
    TRACE_FUNC_ENTER();
    TRACE_FUNC_EXIT();
    return MTRUE;
}

MBOOL VFBNode::onThreadLoop()
{
    TRACE_FUNC("Waitloop");
    MUINT32 requestNo;
    RequestPtr request;
    ImgBufferData p2a;
    FaceData faceData;
    ImgBufferData extWarp;

    if( !waitAllQueueSync(requestNo) )
    {
        return MFALSE;
    }
    if( !mP2ADatas.deque(requestNo, p2a) )
    {
        MY_LOGE("P2AData deque out of sync: %d", requestNo);
        return MFALSE;
    }
    if( !mFaceDatas.deque(requestNo, faceData) )
    {
        MY_LOGE("FaceData deque out of sync: %d", requestNo);
        return MFALSE;
    }
    if( !mExtWarpDatas.deque(requestNo, extWarp) )
    {
        MY_LOGE("ExtWarpData deque out of sync: %d", requestNo);
        return MFALSE;
    }
    if( p2a.mRequest == NULL ||
        p2a.mRequest->mRequestNo != requestNo ||
        p2a.mRequest != faceData.mRequest ||
        p2a.mRequest != extWarp.mRequest )
    {
        MY_LOGE("Request out of sync");
        return MFALSE;
    }
    TRACE_FUNC_ENTER();

    request = p2a.mRequest;
    request->mTimer.startVFB();
    TRACE_FUNC("Frame %d in VFB", request->mRequestNo);
    processVFB(request, p2a.mData, faceData.mData, extWarp.mData);
    request->mTimer.stopVFB();
    TRACE_FUNC_EXIT();
    return MTRUE;
}

static void postionFacesInImage(MUINT32 ImageWidth, MUINT32 ImageHeight, MINT32 numFace, MtkCameraFace* faces)
{
    for(int i = 0; i < numFace; i++)
    {
        faces[i].rect[0] = ((faces[i].rect[0] + 1000) * ImageWidth) / 2000;
        faces[i].rect[1] = ((faces[i].rect[1] + 1000) * ImageHeight) / 2000;
        faces[i].rect[2] = ((faces[i].rect[2] + 1000) * ImageWidth) / 2000;
        faces[i].rect[3] = ((faces[i].rect[3] + 1000) * ImageHeight) / 2000;

        int face_size = faces[i].rect[2] - faces[i].rect[0];
        if(face_size >= 30)
        {
            int zoom_size;
            zoom_size = face_size/15;

            if( (faces[i].rect[0] - zoom_size >= 0) &&
                    (faces[i].rect[1] - zoom_size >= 0) &&
                    (faces[i].rect[2] + zoom_size <= ImageWidth -1) &&
                    (faces[i].rect[3] + zoom_size <= ImageHeight -1))
            {
                zoom_size = face_size/12;
                if( (faces[i].rect[0] - zoom_size >= 0) &&
                        (faces[i].rect[1] - zoom_size >= 0) &&
                        (faces[i].rect[2] + zoom_size <= ImageWidth -1) &&
                        (faces[i].rect[3] + zoom_size <= ImageHeight -1))
                {
                    zoom_size = face_size/10;
                    if( (faces[i].rect[0] - zoom_size >= 0) &&
                            (faces[i].rect[1] - zoom_size >= 0) &&
                            (faces[i].rect[2] + zoom_size <= ImageWidth -1) &&
                            (faces[i].rect[3] + zoom_size <= ImageHeight -1))
                    {
                        zoom_size = face_size/8;
                        if( (faces[i].rect[0] - zoom_size >= 0) &&
                                (faces[i].rect[1] - zoom_size >= 0) &&
                                (faces[i].rect[2] + zoom_size <= ImageWidth -1) &&
                                (faces[i].rect[3] + zoom_size <= ImageHeight -1))
                        {
                            zoom_size = face_size/7;
                            if( (faces[i].rect[0] - zoom_size >= 0) &&
                                    (faces[i].rect[1] - zoom_size >= 0) &&
                                    (faces[i].rect[2] + zoom_size <= ImageWidth -1) &&
                                    (faces[i].rect[3] + zoom_size <= ImageHeight -1))
                            {
                                ;
                            }
                            else
                            {
                                zoom_size = face_size/8;
                            }
                        }
                        else
                        {
                            zoom_size = face_size/10;
                        }
                    }
                    else
                    {
                        zoom_size = face_size/12;
                    }
                }
                else
                {
                    zoom_size = face_size/15;
                }
                faces[i].rect[0] -= zoom_size;
                faces[i].rect[1] -= zoom_size;
                faces[i].rect[2] += zoom_size;
                faces[i].rect[3] += zoom_size;
            }
        }
    }
}

MBOOL VFBNode::processVFB(const RequestPtr &request, const ImgBuffer &p2aImg, MyFace &face, const ImgBuffer &extWarp)
{
    TRACE_FUNC_ENTER();
    ImgBuffer warp;
    ImgBuffer dsImg;
    ImgBuffer alphaCL;
    ImgBuffer alphaNR;
    ImgBuffer pca;

    /* Copy face position for callback */
    MINT32 FaceCount = face.mMeta.number_of_faces;
    MtkCameraFace cbFaceInfo[15];
    for (int i=0; i<15; i++)
    {
        cbFaceInfo[i] = face.mMeta.faces[i];
    }
    postionFacesInImage(p2aImg->getImageBuffer()->getImgSize().w,
                        p2aImg->getImageBuffer()->getImgSize().h,
                        FaceCount,
                        face.mMeta.faces);

    for (int i=0; i<FaceCount; i++)
    {
        MY_LOGE("Found face index %d left %d top %d right %d button %d pose %d ",i+1,face.mMeta.faces[i].rect[0],face.mMeta.faces[i].rect[1],face.mMeta.faces[i].rect[2]
               ,face.mMeta.faces[i].rect[3],face.mMeta.posInfo[i].rip_dir);
    }

    /* Set per frame process info */
    struct MTKPipeVfbProcInfo VfbProcInfo;
    struct MTKPipeVfbResultInfo VfbResultInfo;
    //Input
    VfbProcInfo.SmoothLevel = 13;//mSmoothLevel;
    VfbProcInfo.SkinColor = 13;//mSkinColor;
    VfbProcInfo.EnlargeEyeLevel = 13;//mEnlargeEyeLevel;
    VfbProcInfo.SlimFaceLevel = 13;//mSlimFaceLevel;
    VfbProcInfo.ExtremeBeauty = 1;//jobFullImg.tagImgReq.vfbExtreme;

    VfbProcInfo.TouchX = VfbProcInfo.TouchY = 0;

    VfbProcInfo.DsImgAddr = (MUINT8*) p2aImg->getImageBuffer()->getBufVA(0);
    VfbProcInfo.FaceMetadata = &face.mMeta;

    //Output
    /* Prepare buffer for next node */
    MUINT32 DSClipWidth = p2aImg->getImageBuffer()->getImgSize().w;
    MUINT32 DSClipHeight = p2aImg->getImageBuffer()->getImgSize().h;

    warp = mWarpBufferPool->requestIIBuffer();
    //GpuWarpBase::makePassThroughWarp(warp->getImageBuffer(), request->mFullImgSize);

    dsImg = mDsImgBufferPool->requestIIBuffer();
    alphaCL = mAlphaCLBufferPool->requestIIBuffer();
    alphaNR = mAlphaNRBufferPool->requestIIBuffer();
    pca = mPCABufferPool->requestIIBuffer();

    MSize dsSize = p2aImg->getImageBuffer()->getImgSize();
    dsImg->getImageBuffer()->setExtParam(dsSize);
    VfbProcInfo.WarpedDsImage = (MUINT8*) dsImg->getImageBuffer()->getBufVA(0);

    // Alpha CL
    alphaCL->getImageBuffer()->setExtParam(dsSize);
    VfbProcInfo.AlphaCL = (MUINT8*) alphaCL->getImageBuffer()->getBufVA(0);

    // Alpha NR
    alphaNR->getImageBuffer()->setExtParam(dsSize);
    VfbProcInfo.AlphaNR = (MUINT8*) alphaNR->getImageBuffer()->getBufVA(0);

    // PCA
    VfbProcInfo.PCATable = (MINT32*) pca->getImageBuffer()->getBufVA(0);

    // Warp Map
    VfbProcInfo.WarpMapX = (MINT32*) warp->getImageBuffer()->getBufVA(0);
    VfbProcInfo.WarpMapY = (MINT32*) (warp->getImageBuffer()->getBufVA(0) + warp->getImageBuffer()->getBufSizeInBytes(0) / 2);

    /* VFB Processing */
    MSize postCropSize;
    postCropSize = request->getMaxOutSize();

    VfbProcInfo.ClipWidth = postCropSize.w;//jobFullImg.tagImgReq.imgReq.mBuffer->getImgSize().w;
    VfbProcInfo.ClipHeight = postCropSize.h;//jobFullImg.tagImgReq.imgReq.mBuffer->getImgSize().h;
    VfbProcInfo.DsClipWidth = DSClipWidth;
    VfbProcInfo.DsClipHeight = DSClipHeight;
    VfbProcInfo.SrcImgWidth = postCropSize.w;//jobFullImg.tagImgReq.imgReq.mBuffer->getImgSize().w;
    VfbProcInfo.SrcImgHeight = postCropSize.h;//jobFullImg.tagImgReq.imgReq.mBuffer->getImgSize().h;
    VfbProcInfo.DsImgWidth = p2aImg->getImageBuffer()->getImgSize().w;
    VfbProcInfo.DsImgHeight = p2aImg->getImageBuffer()->getImgSize().h;
    VfbProcInfo.FDWidth = VfbProcInfo.DsImgWidth;
    VfbProcInfo.FDHeight = VfbProcInfo.DsImgHeight;

    VfbProcInfo.PreWarpMapX = mpClearWarpMapX;
    VfbProcInfo.PreWarpMapY = mpClearWarpMapY;
    VfbProcInfo.PreWarpMapSize[0] = VfbProcInfo.PreWarpMapSize[1] = VFB_DS_IMAGE_MAX_WIDTH * VFB_DS_IMAGE_MAX_HEIGHT * 4;


    MY_LOGD("mHalVFBProc Processing QVGA 0x%x, Warped QVGA 0x%x, AlphaCL 0x%x, AlphaNR 0x%x, PCA Table 0x%x, Warp Map 0x%x, 0x%x", VfbProcInfo.DsImgAddr,
              VfbProcInfo.WarpedDsImage, VfbProcInfo.AlphaCL, VfbProcInfo.AlphaNR, VfbProcInfo.PCATable, VfbProcInfo.WarpMapX, VfbProcInfo.WarpMapY);
    if (mpVfbHal->mHalVFBProc(VfbProcInfo, &VfbResultInfo) != true)
    {
        MY_LOGE("mHalVFBProc failed!");
        /* TODO: what now? */
    }

    //p2aImg->getImageBuffer()->saveToFile("/sdcard/buffer_DS_Input.yuv");
    //dsImg->getImageBuffer()->saveToFile("/sdcard/buffer_DS_Output.yuv");
    if( request->needDump() )
    {
        dumpData(request, dsImg->getImageBufferPtr(), "vfb");
    }
    //alphaCL->getImageBuffer()->saveToFile("/sdcard/vfbo_alphacl.bin");
    //alphaNR->getImageBuffer()->saveToFile("/sdcard/vfbo_alphanr.bin");

    if( request->needExVFB() )
    {
    }

    // process here
    if( request->needGPU() )
    {
        handleData(ID_VFB_TO_GPU_WARP, ImgBufferData(warp, request));
    }
    handleData(ID_VFB_TO_P2B, VFBData(VFBResult(dsImg, alphaCL, alphaNR, pca), request));

    TRACE_FUNC_EXIT();
    return MTRUE;
}

} // namespace NSFeaturePipe
} // namespace NSCamFeature
} // namespace NSCam
