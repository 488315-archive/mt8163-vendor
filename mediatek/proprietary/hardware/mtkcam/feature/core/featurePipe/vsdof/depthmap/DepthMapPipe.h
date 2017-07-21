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

#ifndef _MTK_CAMERA_FEATURE_PIPE_DEPTH_MAP_PIPE_H_
#define _MTK_CAMERA_FEATURE_PIPE_DEPTH_MAP_PIPE_H_

#include "FDNode.h"
#include "OCCNode.h"
#include "N3DNode.h"
#include "DPENode.h"
#include "WMFNode.h"
#include "P2AFMNode.h"
#include "DepthMapPipeNode.h"

#include <utils/KeyedVector.h>
#include <featurePipe/core/include/CamPipe.h>
#include <featurePipe/vsdof/IDepthMapPipe.h>

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {

class DepthMapPipe : public CamPipe<DepthMapPipeNode>, public DepthMapPipeNode::Handler_T, public IDepthMapPipe
{
public:
    DepthMapPipe(MINT32 iSensorIdx_Main1, MINT32 iSensorIdx_Main2, MUINT32 iFeatureMask=0);
    virtual ~DepthMapPipe();
    // enque function
    MBOOL enque(EffectRequestPtr& request);
public:
    MBOOL init() { return CamPipe<DepthMapPipeNode>::init(); }
    MBOOL uninit() { return CamPipe<DepthMapPipeNode>::uninit(); }
    MVOID flush();
    MVOID setFlushOnStop(MBOOL flushOnStop) { CamPipe<DepthMapPipeNode>::setFlushOnStop(flushOnStop); }

protected:
    virtual MBOOL onInit();
    virtual MVOID onUninit();

protected:
    virtual MBOOL onData(DataID id, EffectRequestPtr &data);
    virtual MBOOL onData(DataID id, FrameInfoPtr &data);
    virtual MBOOL onData(DataID id, SmartImageBuffer &data);

private:
    // mutex for request map
    android::Mutex mReqMapLock;
    // EffectRequest collections
    KeyedVector<MUINT32, EffectRequestPtr> mvRequestPtrMap;
    // feature mask
    MUINT32 mFeatureOnMask;
    // feature pipe nodes
    P2AFMNode mP2AFMNode;
    N3DNode mN3DNode;
    DPENode mDPENode;
    OCCNode mOCCNode;
    WMFNode mWMFNode;
    StereoFDNode mFDNode;

    // Logging flag
    MBOOL mbProfileLog;
    MBOOL mbDebugLog;
};


}; // NSFeaturePipe
}; // NSCamFeature
}; // NSCam

#endif // _MTK_CAMERA_FEATURE_PIPE_DEPTH_MAP_PIPE_H_
