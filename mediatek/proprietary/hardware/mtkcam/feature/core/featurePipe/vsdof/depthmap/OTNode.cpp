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

#include "OTNode.h"
#include "DepthMapPipe_Common.h"

#define PIPE_CLASS_TAG "OTNode"
#include <featurePipe/core/include/PipeLog.h>

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {


OTNode::
OTNode(const char* name, Graph_T *graph)
:DepthMapPipeNode(name, graph), mpOTHalObj(NULL)
{

    this->addWaitQueue(&mJobQueue);
    mbOTStart = MFALSE;
}

OTNode::
~OTNode()
{
}

MBOOL
OTNode::
onInit()
{
    return MTRUE;
}

MBOOL
OTNode::
enableOT(MBOOL bEnable)
{
    mbOTStart = bEnable;
/*
    if(mbOTStart)
    {
        if(mpOTHalObj!=NULL)
        {
            mpOTHalObj->halOTUninit();
            mpOTHalObj->destroyInstance();
        }
        mpOTHalObj = halOTBase::createInstance(HAL_OT_OBJ_SW);
        mpOTHalObj->halOTInit(VSDOF_OT_IMG_WIDTH, VSDOF_OT_IMG_HEIGHT, 0, 0);
    }
    else
    {
        mpOTHalObj->halOTUninit();
        mpOTHalObj->destroyInstance();
        mpOTHalObj = NULL;
    }
*/
    return MTRUE;
}

MBOOL
OTNode::
onUninit()
{
    if (mpOTHalObj != 0)
    {
        mpOTHalObj->halOTUninit();
        mpOTHalObj->destroyInstance();
        mpOTHalObj = NULL;
    }
    return MTRUE;
}

MBOOL
OTNode::
onThreadStart()
{
    return MTRUE;
}

MBOOL
OTNode::
onThreadStop()
{
    return MTRUE;
}

MBOOL
OTNode::
onData(DataID data, ImgInfoMapPtr& OTImgInfo)
{
    MBOOL ret  = MTRUE;
    TRACE_FUNC_ENTER();

    switch(data)
    {
        /*
        case P2AFM_TO_OT_OTIMG:
            mJobQueue.enque(OTImgInfo);
            ret = MTRUE;
            break;
        */
        default:
            ret = MFALSE;
            break;
    }

    TRACE_FUNC_EXIT();
    return ret;
}

MBOOL
OTNode::
onThreadLoop()
{
    ImgInfoMapPtr OTImgInfo;
    EffectRequestPtr reqPtr;

    if( !waitAllQueue() )
    {
        // mJobs.abort() called
        MY_LOGD("waitAllQueue failed");
        return MFALSE;
    }

    if( !mJobQueue.deque(OTImgInfo) )
    {
        MY_LOGD("N3DNode mJobQueue.deque() failed");
        return MFALSE;
    }

    reqPtr = OTImgInfo->getRequestPtr();


    return MTRUE;

}




}; //NSFeaturePipe
}; //NSCamFeature
}; //NSCam