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

#define PIPE_CLASS_TAG "P2A_VHDR"
#define PIPE_TRACE TRACE_P2A_VHDR
#include <featurePipe/core/include/PipeLog.h>
#include <common/vhdr/1.0/vhdr_type.h>

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {

/* NOTE : this file just implements some vhdr function in P2ANode.h
* Some object might declare in P2ANode
*/

MBOOL P2ANode::initVHDR()
{
    TRACE_FUNC_ENTER();
    TRACE_FUNC_EXIT();
    return MTRUE;
}

MVOID P2ANode::uninitVHDR()
{
    TRACE_FUNC_ENTER();
    TRACE_FUNC_EXIT();
}

MBOOL P2ANode::prepareVHDR(NSCam::NSIoPipe::NSPostProc::QParams &params, const RequestPtr &request)
{
    TRACE_FUNC_ENTER();

    LCEI_CONFIG_DATA config;
    if ( !request->mVarMap.tryGet("vhdr_lcei_data", config))
    {
        MY_LOGI("Error : Can not get lcei_mem data");
    }
    else
    {
        MY_LOGD("get LCEI_VA(0x%08x)", config.lcsoBuf->getBufVA(0)); // This buf maybe released when P1 destory

        NSCam::NSIoPipe::NSPostProc::Input input;
        input.mPortID = NSCam::NSIoPipe::PortID( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_LCEI, 0);
        input.mBuffer = config.lcsoBuf;
        input.mPortID.group=0;
        params.mvIn.push_back(input);
      /*  if (lcei_mem.size() == sizeof(LCEI_CONFIG_DATA)){

            LCEI_CONFIG_DATA config;
            ::memcpy((void*)&config, lcei_mem.array(), lcei_mem.size());

            //MY_LOGD("get LCEI_buf_ptr = %p", config.lcsoBuf);
            MY_LOGD("get LCEI_VA(0x%08x)", config.lcsoBuf->getBufVA(0)); // This buf maybe released when P1 destory

            NSCam::NSIoPipe::NSPostProc::Input input;
            input.mPortID = NSCam::NSIoPipe::PortID( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_LCEI, 0);
            input.mBuffer = config.lcsoBuf;
            input.mPortID.group=0;
            params.mvIn.push_back(input);
        }
        else
        {
            MY_LOGE("Error : lcei_mem data size(%d) != LCEI_CONFIG_DATA size(%d). ", lcei_mem.size(), sizeof(LCEI_CONFIG_DATA));
        } */

    }
    TRACE_FUNC_EXIT();
    return MTRUE;
}



} // namespace NSFeaturePipe
} // namespace NSCamFeature
} // namespace NSCam
