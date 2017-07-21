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

#ifndef _MTK_CAMERA_INC_COMMON_IPARAMSMANAGER_V3_H_
#define _MTK_CAMERA_INC_COMMON_IPARAMSMANAGER_V3_H_

//#include <vector>
#include <utils/String8.h>

#include <common.h>
#include <utils/include/common.h>
#include <camera/CameraParameters.h>

#include <camera/MtkCameraParameters.h>

namespace android
{
/******************************************************************************
 *
 ******************************************************************************/
class IParamsManagerV3
        : public virtual RefBase
{

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Interfaces.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////                    Creation.
    static sp<IParamsManagerV3>    	    createInstance(
                                            String8 const &rName,
                                            MINT32 const cameraId,
                                            sp<IParamsManager> pParamsMgr
                                        );
    //
    virtual ~IParamsManagerV3() {}
    //
    virtual bool                       init() = 0;
    //
    virtual bool                       uninit() = 0;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Attributes .
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    virtual char const*                getName()    const = 0;
    virtual int32_t                    getOpenId()   const = 0;
    virtual sp<IParamsManager>         getParamsMgr()   const = 0;


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Operation.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////     Operations .
    virtual status_t                updateRequest(NSCam::IMetadata *request, MINT32 sensorMode)    = 0;
    virtual status_t                updateRequestHal(NSCam::IMetadata *request) = 0;
    virtual status_t                updateRequestJpeg(NSCam::IMetadata *request)   const = 0;
    virtual status_t                updateRequestPreview(NSCam::IMetadata *request)   const = 0;
    virtual status_t                updateRequestRecord(NSCam::IMetadata *request)   const = 0;
    virtual status_t                updateRequestEng(NSCam::IMetadata *request)    const = 0;

    virtual status_t                setStaticData() = 0;
    virtual void                    setPreviewMaxFps(MUINT32 fps) = 0;
    virtual status_t                calculateCropRegion(MINT32 sensorMode) = 0;
    virtual status_t                getCropRegion(NSCam::MRect& activeArrayCrop, NSCam::MRect& sensorSizeCrop) const = 0;

    //virtual status_t                convert(MtkCameraParameters const& paramString, IMetadata *request) = 0;

};
};
#endif
