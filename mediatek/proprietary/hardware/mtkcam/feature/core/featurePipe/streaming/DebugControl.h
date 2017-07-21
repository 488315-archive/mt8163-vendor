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

#ifndef _MTK_CAMERA_STREAMING_FEATURE_PIPE_DEBUG_CONTROL_H_
#define _MTK_CAMERA_STREAMING_FEATURE_PIPE_DEBUG_CONTROL_H_

#define KEY_FORCE_EIS             "debug.fpipe.force.eis"
#define KEY_FORCE_EIS_FU          "debug.fpipe.force.eis.fu"
#define KEY_FORCE_VFB             "debug.fpipe.force.vfb"
#define KEY_FORCE_VFB_EX          "debug.fpipe.force.vfb.ex"
#define KEY_FORCE_3DNR            "debug.fpipe.force.3dnr"
#define KEY_FORCE_VHDR            "debug.fpipe.force.vhdr"
#define KEY_FORCE_GPU_PASS        "debug.fpipe.force.gpu.pass"
#define KEY_FORCE_IMG3O           "debug.fpipe.force.img3o"
#define KEY_USE_PER_FRAME_SETTING "debug.fpipe.frame.setting"
#define KEY_DEBUG_DUMP            "debug.fpipe.force.dump"
#define KEY_DEBUG_DUMP_COUNT      "debug.fpipe.force.dump.count"

#define SUPPORT_VFB               0

#define DEBUG_TIMER               1

#define DEV_VFB_READY             0
#define DEV_P2B_READY             0
#define DEV_FULLIMG_FROM_MDP      0

#define NO_FORCE  0
#define FORCE_ON  1
#define FORCE_OFF 2

#define VAL_FORCE_EIS             0
#define VAL_FORCE_EIS_FU          0
#define VAL_FORCE_VFB             0
#define VAL_FORCE_VFB_EX          0
#define VAL_FORCE_3DNR            0
#define VAL_FORCE_VHDR            0
#define VAL_FORCE_GPU_PASS        0
#define VAL_FORCE_IMG3O           0
#define VAL_DEBUG_DUMP            0
#define VAL_DEBUG_DUMP_COUNT      1
#define VAL_USE_PER_FRAME_SETTING 0

#define PIPE_MODULE_TAG "MtkCam/StreamingPipe"

#define TRACE_STREAMING_FEATURE_COMMON  0
#define TRACE_STREAMING_FEATURE_NODE    0
#define TRACE_STREAMING_FEATURE_PIPE    0
#define TRACE_STREAMING_FEATURE_TIMER   0
#define TRACE_P2A_NODE                  0
#define TRACE_P2A_3DNR                  0
#define TRACE_P2A_VHDR                  0
#define TRACE_EIS_NODE                  0
#define TRACE_GPU_NODE                  0
#define TRACE_MDP_NODE                  0
#define TRACE_VFB_NODE                  0
#define TRACE_FD_NODE                   0
#define TRACE_P2B_NODE                  0
#define TRACE_GPU_WARP                  0
#define TRACE_QPARAMS_BASE              0
#define TRACE_HELPER_NODE               0

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {

} // namespace NSFeaturePipe
} // namespace NSCamFeature
} // namespace NSCam

#endif // _MTK_CAMERA_STREAMING_FEATURE_PIPE_DEBUG_CONTROL_H_
