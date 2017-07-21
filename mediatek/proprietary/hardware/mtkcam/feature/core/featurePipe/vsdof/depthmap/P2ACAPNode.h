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
#ifndef _MTK_CAMERA_INCLUDE_DEPTHMAP_FEATURE_PIPE_P2A_CAP_NODE_H
#define _MTK_CAMERA_INCLUDE_DEPTHMAP_FEATURE_PIPE_P2A_CAP_NODE_H

#include "DepthMapPipeNode.h"

#include "DepthMapPipe_Common.h"

#include <IHal3A.h>
#include <featurePipe/core/include/WaitQueue.h>
#include <iopipe/PostProc/IHalPostProcPipe.h>
#include <iopipe/PostProc/INormalStream.h>
#include <featurePipe/vsdof/util/TuningBufferPool.h>

using androdi::KeyedVector;
using namespace NSCam::NSIoPipe::NSPostProc;

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {

class IHal3A;
class INormalStream;

class P2ACAPNode : public DepthMapPipeNode
{
public:
    P2ACAPNode(const char* name, Graph_T* graph, MUINT32 iSensorIdx_Main1, MUINT32 iSensorIdx_Main2);
    virtual ~P2ACAPNode();
    virtual MBOOL onData(DataID id, ImgInfoMapPtr &data);
    MVOID setSensorIdx(MUINT32 main1_idx, MUINT32 main2_idx);
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
        EffectRequestPtr mRequest;
        P2ACAPNode* mNode;
        Vector<SmartTuningBuffer> mEnqueTuningBuffer;
        KeyedVector<DepthMapBufferID, SmartImageBuffer> mEnquedSmartImgBufMap;

        EnquedBufPool(EffectRequestPtr req, P2ACAPNode* pNode)
        : mRequest(req), mEnquedSmartImgBufMap(), mNode(pNode)
        {
        }

        MVOID addBuffData(DepthMapBufferID rID, SmartImageBuffer pSmBuf)
        { mEnquedSmartImgBufMap.add(rID, pSmBuf); }

        MVOID addTuningData(SmartTuningBuffer pSmBuf)
        { mEnqueTuningBuffer.add(pSmBuf); }
    };

    MVOID cleanUp();

    static MVOID onP2Callback(QParams& rParams);
    MVOID handleP2Done(QParams& rParams, EnquedBufPool* pEnqueBufPool);
    // tuning buffer pool
    android::sp<TuningBufferPool> mpTuningBufferPool;
    // P2 NormalStream
    INormalStream*  mpINormalStream;
    // 3A hal
    IHal3A* mp3A;
    // sensor index
    MUINT32 miSensorIdx_Main1;
    MUINT32 miSensorIdx_Main2;
    // Job queue
    WaitQueue<ImgInfoMapPtr> mJobQueue;
    // size constant
    MSize P2AFM_MAIN_IMAGE_CAP_SIZE;
};


} //NSFeaturePipe
} //NSCamFeature
} //NSCam



#endif