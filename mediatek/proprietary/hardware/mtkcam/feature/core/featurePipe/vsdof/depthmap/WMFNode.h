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

#ifndef _MTK_CAMERA_INCLUDE_DEPTHMAP_FEATURE_PIPE_WMF_NODE_H
#define _MTK_CAMERA_INCLUDE_DEPTHMAP_FEATURE_PIPE_WMF_NODE_H

#include "DepthMapPipeNode.h"
#include "DepthMapPipe_Common.h"


// TODO: make sure the dpecommon.h header location
#include <drv/dpecommon.h>
#include <utils/KeyedVector.h>
#include <featurePipe/core/include/WaitQueue.h>
// TODO: make sure the dpeStream.h header location
#include <iopipe/PostProc/DpeStream.h>
// TODO: make sure the IHalDpePipe.h header location
#include <iopipe/PostProc/IHalDpePipe.h>

using namespace NSCam::NSIoPipe::NSDpe;
using namespace NSCam::NSIoPipe;

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {

class WMFNode: public DepthMapPipeNode
{

public:
    WMFNode(const char* name, Graph_T* graph);
    virtual ~WMFNode();
    virtual MBOOL onData(DataID id, ImgInfoMapPtr& data);

protected:
    virtual MBOOL onInit();
    virtual MBOOL onUninit();
    virtual MBOOL onThreadLoop();
    virtual MBOOL onThreadStart();
    virtual MBOOL onThreadStop();
private:
    struct EnquedBufPool
    {
    public:
        ImgInfoMapPtr mpImgBufInfo;
        WMFNode* mpNode;
        KeyedVector<DepthMapBufferID, SmartImageBuffer> mEnquedSmartImgBufMap;

        EnquedBufPool(ImgInfoMapPtr infoPtr, WMFNode* pWMFNode)
        : mpImgBufInfo(infoPtr), mEnquedSmartImgBufMap(), mpNode(pWMFNode)
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

    class BufferPoolSet
    {
    public:
        BufferPoolSet();
        virtual ~BufferPoolSet();

        MBOOL init();
        MBOOL uninit();
    public:
        android::sp<ImageBufferPool> mpTbliImgBufPool;
        android::sp<ImageBufferPool> mpInternalDMWImgBufPool;
    };

    MVOID cleanUp();
    // WMF enque parameters
    MVOID prepareWMFEnqueConfig(sp<IImageBuffer> pImgBuf_SrcImg, sp<IImageBuffer> pImgBuf_SrcDepth, sp<IImageBuffer> pImgBuf_OutputDepth, WMFEConfig &rWMFConfig,
    EnquedBufPool* pEnqueBufPool);

    // internal thread loop : first pass
    MBOOL threadLoop_WMFFirstPass(ImgInfoMapPtr& rpOCCImgInfo);
    static MVOID onWMFEnqueDone_FirstPass(WMFEParams& rParams);
    MVOID handleWMFEnqueDone_FirstPass(WMFEParams& rParams, EnquedBufPool* pEnqueBufPool);
    // internal thread loop : second pass
    MBOOL threadLoop_WMFSecPass(ImgInfoMapPtr& rpWMF_ImgInfo);
    static MVOID onWMFEnqueDone_SecPass(WMFEParams& rParams);
    MVOID handleWMFEnqueDone_SecPass(WMFEParams& rParams, EnquedBufPool* pEnqueBufPool);
    // pack data intot DPEBufInfo
    MBOOL setupDPEBufInfo(DPEDMAPort dmaPort, android::sp<IImageBuffer> &pImgBuf, DPEBufInfo& rBufInfo);
    // debug WMF param
    MVOID debugWMFParam(WMFEConfig &config);
private:
    WaitQueue<ImgInfoMapPtr> mOCCInfoQueue;
    // second pass WMF operation Queue
    WaitQueue<ImgInfoMapPtr> mWMFSecQueue;
    // MYSL buffer container
    KeyedVector<MUINT32, SmartImageBuffer> mMYSLSImageBufMap;
    android::Mutex mMYSLMutex;
    // DPE stream
    IDpeStream* mpDPEStream;
    // Buffer pool set
    BufferPoolSet mBufPoolSet;
    // TBLI buffer
    SmartImageBuffer mpTbliImgBuf;
    WMFEFILTERSIZE  eWMFFilterSize;
};


}; //NSFeaturePipe
}; //NSCamFeature
}; //NSCam

#endif