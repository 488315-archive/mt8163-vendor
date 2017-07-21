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
//
#define PIPE_MODULE_TAG "BokehPipe"
#define PIPE_CLASS_TAG "BokehPipe"
//
#include <PipeLog.h>
#include <core/include/CamPipe.h>
#include <mmsdk/EffectRequest.h>
#include <featurePipe/vsdof/IBokehPipe.h>
//
#include "BokehPipeNode.h"
#include "BokehPipeImp.h"
//
using namespace NSCam::NSCamFeature::NSFeaturePipe;
//************************************************************************
MBOOL
BokehPipeImp::
init(const char *name)
{
    TRACE_FUNC_ENTER();
    MY_LOGD("CKH: init pipe");
    (void)name;
    MBOOL ret;
    ret = PARENT_PIPE::init();
    TRACE_FUNC_EXIT();
    return ret;
}
//************************************************************************
MBOOL
BokehPipeImp::
uninit(const char *name)
{
    TRACE_FUNC_ENTER();
    (void)name;
    MBOOL ret;
    ret = PARENT_PIPE::uninit();
    TRACE_FUNC_EXIT();
    return ret;
}
//************************************************************************
MBOOL
BokehPipeImp::
enque(EffectRequestPtr &effectPtr)
{
    TRACE_FUNC_ENTER();
    MBOOL ret = MFALSE;
    ret = PARENT_PIPE::enque(ID_ROOT_ENQUE, effectPtr);
    TRACE_FUNC_EXIT();
    return ret;
}
//************************************************************************

MVOID
BokehPipeImp::
flush()
{
    TRACE_FUNC_ENTER();
    MY_LOGD("CKH: Flush");
    MBOOL ret = MFALSE;
    PARENT_PIPE::flush();
    TRACE_FUNC_EXIT();
}
//************************************************************************

MVOID
BokehPipeImp::
sync()
{
    TRACE_FUNC_ENTER();
    MY_LOGD("CKH: SYNC");
    MBOOL ret = MFALSE;
    PARENT_PIPE::sync();
    TRACE_FUNC_EXIT();
}
//************************************************************************
BokehPipeImp::BokehPipeImp(MINT32 openSensorIndex, MINT32 runPath)
  : CamPipe<BokehPipeNode>("BokehPipe"),
    mGFNode("GFNode", &mCamGraph, runPath),
    mP2BNode("P2BNode", &mCamGraph, openSensorIndex, runPath),
    mMDPNode("MDPNode", &mCamGraph, openSensorIndex),
    miOpenId(openSensorIndex)
{
    if(runPath == GENERAL)
    {
        MY_LOGD("CKH: GENERAL path");
        buildGeneralPath();
    }
    else
    {
        MY_LOGE("Path(%d) is not support.", runPath);
    }
    MY_LOGD("OpenId(%d) runPath(%d)", miOpenId, runPath);
}
//************************************************************************
BokehPipeImp::~BokehPipeImp()
{
    // must call dispose to free CamGraph
    this->dispose();
}
//************************************************************************
MBOOL BokehPipeImp::onInit()
{
    return MTRUE;
}
//************************************************************************
MVOID BokehPipeImp::onUninit()
{
}
//************************************************************************
MBOOL BokehPipeImp::onData(DataID id, EffectRequestPtr &data)
{
    MY_LOGD("+");
    android::String8 status;
    if(id == P2B_OUT)
    {
        status = "Done";
        MY_LOGD("CKH: receive request done.");
    }
    else if(id == MDP_OUT)
    {
        status = "Capture Done";
        MY_LOGD("CKH: receive capture request done.");
    }
    /*else if(id == GF_ERROR)
    {
        status = "Failed";
        const sp<EffectParameter> request_params = data->getRequestParameter();
        int msg = request_params->getInt(BOKEH_EFFECT_REQUEST_ERROR_MSG);
        MY_LOGD("CKH: receive GF_ERROR. msg(%d)", msg);
    }
    else if(id == P2B_ERROR)
    {
        status = "Failed";
        const sp<EffectParameter> request_params = data->getRequestParameter();
        int msg = request_params->getInt(BOKEH_EFFECT_REQUEST_ERROR_MSG);
        MY_LOGD("CKH: receive P2B_ERROR. msg(%d)", msg);
    }*/
    //
    MY_LOGD("1");
    if(data->mpOnRequestProcessed)
    {
        data->mpOnRequestProcessed(data->mpTag, status, data);
        MY_LOGD("CKH: Send callback.");
    }
    MY_LOGD("2");
    return MTRUE;
}
//************************************************************************
MBOOL BokehPipeImp::onData(DataID id, FrameInfoPtr &data)
{
    MY_LOGE("Not implement.");
    return MTRUE;
}
//************************************************************************
MBOOL BokehPipeImp::onData(DataID id, SmartImageBuffer &data)
{
    MY_LOGE("Not implement.");
    return MTRUE;
}
//************************************************************************
void
BokehPipeImp::
buildGeneralPath()
{
    this->connectData(GF_BOKEH_REQUEST, GF_BOKEH_REQUEST, mGFNode, mP2BNode);
    this->connectData(GF_BOKEH_DMG, GF_BOKEH_DMG, mGFNode, mP2BNode);
    this->connectData(GF_BOKEH_DMBG, GF_BOKEH_DMBG, mGFNode, mP2BNode);
    this->connectData(P2B_OUT, P2B_OUT, mP2BNode, mMDPNode);
    // preview or record will use this path
    this->connectData(P2B_OUT, P2B_OUT, mP2BNode, this);
    // capture will use this path
    this->connectData(P2B_MDP_REQUEST, P2B_MDP_REQUEST, mP2BNode, mMDPNode);
    this->connectData(P2B_MDP_WDMA, P2B_MDP_WDMA, mP2BNode, mMDPNode);
    this->connectData(MDP_OUT, MDP_OUT, mMDPNode, this);
    // error message
    this->connectData(GF_ERROR, GF_ERROR, mGFNode, this);
    this->connectData(P2B_ERROR, P2B_ERROR, mP2BNode, this);
    //
    this->setRootNode(&mGFNode);
}
//************************************************************************
void
BokehPipeImp::
setStopMode(MBOOL flushOnStop)
{
    MY_LOGD("stopMode(%d)", flushOnStop);
    this->setFlushOnStop(flushOnStop);
}