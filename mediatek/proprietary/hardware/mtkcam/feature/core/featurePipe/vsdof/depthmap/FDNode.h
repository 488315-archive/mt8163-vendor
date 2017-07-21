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
 
#ifndef _MTK_CAMERA_INCLUDE_DEPTHMAP_FEATURE_PIPE_FD_NODE_H
#define _MTK_CAMERA_INCLUDE_DEPTHMAP_FEATURE_PIPE_FD_NODE_H

#include "DepthMapPipeNode.h"
#include "DepthMapPipe_Common.h"

#include <utils/Mutex.h>
#include <utils/String8.h>
#include <utils/KeyedVector.h>
#include <vsdof/hal/json_util.h>
#include <featurePipe/core/include/WaitQueue.h>
#include <FaceDetection/FD4.0/fd_hal_base.h>

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {

class StereoFDNode: public DepthMapPipeNode
{
    
public:
    StereoFDNode(const char* name, Graph_T* graph);
    virtual ~StereoFDNode();
    virtual MBOOL onData(DataID id, ImgInfoMapPtr& pImgInfo);
    virtual MBOOL onData(DataID data, ExtraDataInfoPtr& N3DExtraData);
    
protected:
    virtual MBOOL onInit();
    virtual MBOOL onUninit();
    virtual MBOOL onThreadLoop();
    virtual MBOOL onThreadStart();
    virtual MBOOL onThreadStop();
    
private:
    MVOID calcRotatedFDResult(MtkCameraFace inputFaceInfo, MUINT32 iRotation, FD_DATA_STEREO_T &rRotatedFDResult);
    MVOID calcRotatedFDResult_v2(MtkCameraFace inputFaceInfo, MUINT32 iRotation, FD_DATA_STEREO_T &rRotatedFDResult);
    
    MVOID mergeExtraData(EffectRequestPtr pEffReqPtr);
    
private:
    WaitQueue<ImgInfoMapPtr> mJobQueue;
    halFDBase* mpFDHalObj;    
    MUINT8* mpFDWorkingBuffer;
    MUINT8* mpDDPBuffer;
    MUINT8* mpExtractYBuffer;
    MtkCameraFaceMetadata* mpDetectedFaces;
    JSON_Util* mpJsonUtil;
    
    KeyedVector<MUINT32, String8> mvFDJsonMap;
    KeyedVector<MUINT32, String8> mvN3DJsonMap;
    android::Mutex mJsonMutex;
    
    MSize P2AFM_FD_IMG_SIZE;
    
};

}; //NSFeaturePipe
}; //NSCamFeature
}; //NSCam

#endif