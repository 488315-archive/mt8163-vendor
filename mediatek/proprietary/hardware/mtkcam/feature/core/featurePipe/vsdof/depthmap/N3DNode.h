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

#ifndef _MTK_CAMERA_INCLUDE_DEPTHMAP_FEATURE_PIPE_N3D_NODE_H
#define _MTK_CAMERA_INCLUDE_DEPTHMAP_FEATURE_PIPE_N3D_NODE_H

#include "DepthMapPipeNode.h"
#include "DepthMapPipe_Common.h"

#include <vsdof/hal/n3d_hal.h>
#include <featurePipe/core/include/WaitQueue.h>
#include <featurePipe/core/include/GraphicBufferPool.h>
namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {

class N3DNode: public DepthMapPipeNode
{
public:
    N3DNode(const char* name, Graph_T* graph);
    virtual ~N3DNode();
    virtual MBOOL onData(DataID id, ImgInfoMapPtr& data);

protected:
    virtual MBOOL onInit();
    virtual MBOOL onUninit();
    virtual MBOOL onThreadLoop();
    virtual MBOOL onThreadStart();
    virtual MBOOL onThreadStop();
    virtual const char* onDumpBIDToName(DepthMapBufferID BID);
private:
    class BufferSizeConfig
    {
    public:
        BufferSizeConfig();
        virtual ~BufferSizeConfig();

    public:
        // buffer size const
        MSize VSDOF_N3D_OUTPUT_IMG_SIZE;
        MSize VSDOF_N3D_OUTPUT_MASK_SIZE;
        MSize VSDOF_N3D_LDC_SIZE;
    };

    class BufferPoolSet
    {
    public:
        BufferPoolSet();
        virtual ~BufferPoolSet();

        MBOOL init(const BufferSizeConfig& config);
        MBOOL uninit();
    public:
        android::sp<ImageBufferPool> mN3DImgBufPool;
        android::sp<ImageBufferPool> mN3DMaskBufPool;
        android::sp<ImageBufferPool> mLDCBufPool;
    };

    MVOID cleanUp();
    MBOOL performN3DALGO_VRPV(ImgInfoMapPtr& rP2ImgInfo);
    MBOOL performN3DALGO_CAP(ImgInfoMapPtr& rP2ImgInfo);
    // input N3DParams
    MBOOL prepareN3DInputParam(ImgInfoMapPtr& rP2ImgInfo, N3D_HAL_PARAM& rN3dParam);
    MBOOL prepareN3DInputParam_CAP(ImgInfoMapPtr& rP2ImgInfo, N3D_HAL_PARAM_CAPTURE& rN3dParam);
    // output N3DParams
    MBOOL prepareN3DOutputParam(N3D_HAL_OUTPUT& rN3dParam, ImgInfoMapPtr& pToDPEImgInfo, 
                                ImgInfoMapPtr& pToOCCImgInfo);
    MBOOL prepareN3DOutputParam_CAP(ImgInfoMapPtr& rP2ImgInfo, N3D_HAL_OUTPUT_CAPTURE& rN3dParam,
                                ImgInfoMapPtr& pToDPEImgInfo, ImgInfoMapPtr& pToOCCImgInfo);

    MBOOL prepareFEFMData(ImgInfoMapPtr& rP2ImgInfo, HWFEFM_DATA& rFefmData);
    MBOOL prepareEISData(ImgInfoMapPtr& rP2ImgInfo, EIS_DATA& rEISData);

    MBOOL copyImageBuffer(sp<IImageBuffer> srcImgBuf, sp<IImageBuffer> dstImgBuf);

    MVOID debugN3DParams(N3D_HAL_PARAM& param, N3D_HAL_OUTPUT& output);

private:
    WaitQueue<ImgInfoMapPtr> mJobQueue;

    N3D_HAL* mpN3DHAL_CAP;
    N3D_HAL* mpN3DHAL_VRPV;

    // BufferPoolSet
    BufferPoolSet mBufPoolSet;
    // Buffer size settings
    BufferSizeConfig mBufConfig;
};


}; //NSFeaturePipe
}; //NSCamFeature
}; //NSCam

#endif


