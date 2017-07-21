/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

#ifndef _MTK_PLATFORM_HARDWARE_MTKCAM_V3_DEVICE_EXTERNAL_CAM3EXTERNALDEVICEIMP_H_
#define _MTK_PLATFORM_HARDWARE_MTKCAM_V3_DEVICE_EXTERNAL_CAM3EXTERNALDEVICEIMP_H_
//
#include <v3/app/IAppStreamManager.h>
#include <device/Cam3DeviceBase.h>
//
#include <cutils/properties.h>
//
#include <device/IExternalDevice.h>
//
#include <v3/hwnode/BufferReceiveNode.h>

/******************************************************************************
 *
 ******************************************************************************/
namespace NSCam {
namespace v3 {
namespace NSDefaultCam3Device {


/******************************************************************************
 *  Camera3 Device Implementation for Default.
 ******************************************************************************/
class Cam3ExternalDeviceImp
    : public Cam3DeviceBase
    , public NSCam::IAppCallback
{
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Data Members.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
protected:  ////                    Data Members.
    struct  AppContext
    {
        android::sp<IAppStreamManager>             pAppStreamManager;
        android::sp<NSCam::v3::BufferReceiveNode>  pBufferReceiveNode;
    };
    AppContext                      mAppContext;
    mutable android::Mutex          mAppContextLock;

protected:  ////                    Data Members.
    mutable android::Mutex          mRequestingLock;
    MINT32                          mRequestingAllowed;

protected:  ////                    duration information for debug
    MUINT64                         mProcessRequestStart;
    MUINT64                         mProcessRequestEnd;

protected:  ////                    external information
    android::sp<IExternalDevice>    pExternal;
    CameraInfo*                     pInfo;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Operations.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////                    Instantiation.
    virtual                         ~Cam3ExternalDeviceImp();
                                    Cam3ExternalDeviceImp(
                                        String8 const&               rDevName,
                                        int32_t const                i4OpenId,
                                        NSCam::IExternalDevice*      aExternal,
                                        CameraInfo*                  aInfo
                                    );

protected:  ////                    Operations.
    virtual AppContext              getSafeAppContext() const;
    virtual status_t                flushAndWait(AppContext const& appContext);
    virtual MVOID                   flushRequest(
                                        camera3_capture_request_t* request
                                    )   const;

protected:  ////                    Operations.
    MVOID                           enableRequesting();
    MVOID                           disableRequesting();

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  [Template method] Operations.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
protected:  ////                    Operations.
    virtual status_t                onInitializeLocked();
    virtual status_t                onUninitializeLocked();
    virtual status_t                onConfigureStreamsLocked(
                                        camera3_stream_configuration_t* stream_list
                                    );
    virtual status_t                onFlushLocked();

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Cam3Device Interfaces.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////                    Request creation and submission
    virtual camera_metadata const*  i_construct_default_request_settings(
                                        int type
                                    );

public:     ////                    Request creation and submission
    virtual status_t                i_process_capture_request(
                                        camera3_capture_request_t* request
                                    );

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  IAppCallback Interfaces.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////                    Request creation and submission
    virtual MVOID                   updateFrame(
                                        MUINT32 const requestNo,
                                        MINTPTR const userId,
                                        bool    const lastPart,
                                        android::Vector<android::sp<IMetaStreamBuffer> > vOutMeta
                                    );
};


/******************************************************************************
 *
 ******************************************************************************/
};  //namespace NSDefaultCam3Device
};  //namespace v3
};  //namespace NSCam
#endif  //_MTK_PLATFORM_HARDWARE_MTKCAM_V3_DEVICE_EXTERNAL_CAM3EXTERNALDEVICEIMP_H_

