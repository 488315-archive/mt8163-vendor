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
#ifndef _MTK_CAMERA_FEATURE_PIPE_BOKEH_PIPE_H_
#define _MTK_CAMERA_FEATURE_PIPE_BOKEH_PIPE_H_
//
using namespace android;
//
namespace NSCam {
class EffectRequest;
namespace NSCamFeature {
namespace NSFeaturePipe {
//
//
class IBokehPipe : public virtual android::RefBase
{
    public:
        /*
        * Return sp<IBokehPipe> instance.
        * @param openSensorIndex open sensor index
        * @param runPath decide feature pipe data flow
        */
        static sp<IBokehPipe> createInstance(MINT32 openSensorIndex, MINT32 runPath = 0);
        /*
        * destroy instance
        */
        MBOOL destroyInstance(const char *name=NULL);
        /*
        * BokekPipe initialization
        */
        virtual MBOOL init(const char *name=NULL) = 0;
        /*
        * uninit BokehPipe
        */
        virtual MBOOL uninit(const char *name=NULL) = 0;
        /*
        * Pass EffectRequest to BokehPipe.
        * EffectRequest contains MY_S, DMW and Main image buffers, related
        * output buffer by scenario and AF info.
        * After processing finished, BokehPipe will call callback function
        * (defined in EffectRequest) to return result.
        */
        virtual MBOOL enque(sp<EffectRequest> &param) = 0;
        /*
        * Discard all request in wait queue.
        * If request is been enque in HW, it will wait HW processing finish.
        */
        virtual MVOID flush() = 0;
        /*
        * Sync function will wait until all EffectRequest is processed done.
        */
        virtual MVOID sync() = 0;
        /*
        * Set stop mode to BokehPipe.
        * If flushOnStop is true, all EffectRequest will be discard when user
        * call uninit.
        * If flushOnStop is false, BokehPipe will uninit until all EffectRequest
        * is processed done.
        */
        virtual MVOID setStopMode(MBOOL flushOnStop) = 0;
};
}; // NSFeaturePipe
}; // NSCamFeature
}; // NSCam

#endif // _MTK_CAMERA_FEATURE_PIPE_BOKEH_PIPE_H_
