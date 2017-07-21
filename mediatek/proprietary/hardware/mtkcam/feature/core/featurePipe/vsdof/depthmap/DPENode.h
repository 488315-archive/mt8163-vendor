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

#ifndef _MTK_CAMERA_INCLUDE_DEPTHMAP_FEATURE_PIPE_DPE_NODE_H
#define _MTK_CAMERA_INCLUDE_DEPTHMAP_FEATURE_PIPE_DPE_NODE_H

#include "DepthMapPipeNode.h"
#include "DepthMapPipe_Common.h"

// TODO: make sure the dpecommon.h header location
#include <drv/dpecommon.h>
// TODO: make sure the dpeStream.h header location
#include <iopipe/PostProc/DpeStream.h>

#include <featurePipe/core/include/WaitQueue.h>
// TODO: make sure the IHalDpePipe.h header location
#include <iopipe/PostProc/IHalDpePipe.h>

using namespace NSCam::NSIoPipe::NSDpe;

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {

class DPENode: public DepthMapPipeNode
{
public:
    DPENode(const char* name, Graph_T* graph);
    virtual ~DPENode();
    virtual MBOOL onData(DataID id, ImgInfoMapPtr& data);

protected:
    virtual MBOOL onInit();
    virtual MBOOL onUninit();
    virtual MBOOL onThreadLoop();
    virtual MBOOL onThreadStart();
    virtual MBOOL onThreadStop();
    virtual const char* onDumpBIDToName(DepthMapBufferID BID);
private:
    struct EnquedBufPool
    {
    public:
        ImgInfoMapPtr mpImgBufInfo;
        DPENode* mpNode;
        KeyedVector<DepthMapBufferID, SmartImageBuffer> mEnquedSmartImgBufMap;

        EnquedBufPool(ImgInfoMapPtr infoPtr, DPENode* pDPENode)
        : mpImgBufInfo(infoPtr), mEnquedSmartImgBufMap(), mpNode(pDPENode)
        {
        }

        MVOID addBuffData(DepthMapBufferID rID, SmartImageBuffer pSmBuf)
        { mEnquedSmartImgBufMap.add(rID, pSmBuf); }

        MBOOL getBuffData(DepthMapBufferID rID, SmartImageBuffer& rpSmImgBuf)
        {
            if(mEnquedSmartImgBufMap.indexOfKey(rID)>=0)
            {
                rpSmImgBuf = mEnquedSmartImgBufMap.valueFor(rID);
                return MTRUE;
            }
            return MFALSE;
        }
    };

    class BufferSizeConfig
    {
    public:
        BufferSizeConfig();
        virtual ~BufferSizeConfig();

    public:
        MSize VSDOF_DMP_SIZE;
        MSize VSDOF_CFM_SIZE;
        MSize VSDOF_RESPO_SIZE;
        // stride size
        MUINT32 VSDOF_DMP_STRIDE_SIZE;
        MUINT32 VSDOF_CFM_STRIDE_SIZE;
        MUINT32 VSDOF_RESPO_STRIDE_SIZE;
        MUINT32 VSDOF_ORG_IMG_WIDTH;
        MUINT32 VSDOF_ORG_IMG_HEIGHT;
    };

    class BufferPoolSet
    {
    public:
        BufferPoolSet();
        virtual ~BufferPoolSet();

        MBOOL init(const BufferSizeConfig& config);
        MBOOL uninit();
    public:
        android::sp<ImageBufferPool> mDMPBuffPool;
        android::sp<ImageBufferPool> mCFMBuffPool;
        android::sp<ImageBufferPool> mRespBuffPool;
    };

    MVOID cleanUp();
    MBOOL prepareDPEEnqueConfig(ImgInfoMapPtr pN3DImgInfo, DVEConfig& rDPEConfig, EnquedBufPool *pEnqueBufPool);
    MBOOL setupDPEBufInfo(DPEDMAPort dmaPort, SmartImageBuffer &pSmImgBuf, DPEBufInfo& rBufInfo);
    MBOOL setupDPEBufInfo(DPEDMAPort dmaPort, SmartImageBuffer &pSmImgBuf, DPEBufInfo& rBufInfo, EnquedBufPool *pEnqueBufPool, DepthMapBufferID bufID, MSize szActiveArea);

    static MVOID onDPEEnqueDone(DVEParams& rParams);
    MVOID handleDPEEnqueDone(DVEParams& rParams, EnquedBufPool* pEnqueBufPool);
    MVOID debugDPEConfig(DVEConfig& config);

private:
    // job queue
    WaitQueue<ImgInfoMapPtr> mJobQueue;
    MBOOL mbFirst;
    // Last frame DMP
    SmartImageBuffer mLastDMP_L;
    SmartImageBuffer mLastDMP_R;
    // Buffer pool set
    BufferPoolSet mBufPoolSet;
    // Buffer size settings
    BufferSizeConfig mBufConfig;
    // DPE stream
    IDpeStream* mpDPEStream;
};





}; //NSFeaturePipe
}; //NSCamFeature
}; //NSCam

#endif