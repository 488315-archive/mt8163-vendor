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

#ifndef _MTK_HARDWARE_INCLUDE_MTKCAM_V3_HWNODE_BUFFERRECEIVENODE_H_
#define _MTK_HARDWARE_INCLUDE_MTKCAM_V3_HWNODE_BUFFERRECEIVENODE_H_
//
#include <v3/stream/IStreamInfo.h>
#include <v3/stream/IStreamBuffer.h>
#include <v3/utils/streambuf/IStreamBufferPool.h>
#include <device/IExternalDevice.h>
//
#include <utils/BitSet.h>
#include <utils/RefBase.h>
#include <utils/Vector.h>
#include <utils/KeyedVector.h>
#include <utils/StrongPointer.h>


/******************************************************************************
 *
 ******************************************************************************/
namespace NSCam {
namespace v3 {


/******************************************************************************
 *
 ******************************************************************************/
class BufferReceiveNode
    : public virtual android::RefBase
{
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Implementations.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////                    Definitions.
    typedef android::KeyedVector<StreamId_T, android::sp<IImageStreamInfo> > ImageStreamInfoSetT;
    struct InitParams
    {
        /**
         * An index to indicate which camera device to open.
         */
        MINT32              openId;

        /**
         * A unique value for the node id.
         */
        MINT32              nodeId;

        /**
         * A pointer to a null-terminated string for the node name.
         */
        char const*         nodeName;

        /**
         * A pointer to External device.
         */
        android::sp<NSCam::IExternalDevice> 
                            pExternal;

        /**
         * A pointer to IAppCallback.
         */
        android::wp<NSCam::IAppCallback> 
                            pAppCallback;
    };

    struct  ConfigParams
    {
        /**
         * A pointer to a set of input app meta stream info.
         */
        android::sp<IMetaStreamInfo>  pInAppMeta;

        /**
         * A pointer to a set of output image stream info.
         */
        ImageStreamInfoSetT           pOutImage;
    };

/******************************************************************************
 *
 ******************************************************************************/
    class IRequest
        : public android::RefBase
    {
    public:

        MUINT32                     requestNo;

        uintptr_t                   timestamp;

        android::wp<IAppCallback>   pAppCallback;

        camera_metadata*            setting;

        android::KeyedVector<
            StreamId_T,
            android::sp<IImageStreamBuffer>
                            >       vOImageBuffers;

        // request meta
        android::sp<IMetaStreamBuffer>
                                    vIMetaBuffers;

        android::BitSet32           bufferTypes;
    };

    class IonBuffer;

    class RemoteBuffer
        : public android::RefBase
    {
    public:

        int                               format;

        int                               width;

        int                               height;

        android::sp<IonBuffer>            pWifiIon;

        android::sp<IonBuffer>            pIon;

        android::sp<IImageBuffer>         pBuf;

        android::sp<IMetaStreamBuffer>    pMeta;
    };

    struct  BufferBit
    {
        enum
        {
            YUV,
            BLOB,
        };
    };

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Implementations.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////                    Operations.

    static android::sp<BufferReceiveNode>      
                                    createInstance();

                                    BufferReceiveNode() {};

    virtual                         ~BufferReceiveNode() {};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Interface.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////                    Interface.

    virtual MERROR                  init(InitParams const& rParams)        = 0;

    virtual MERROR                  uninit()                               = 0;

    virtual MERROR                  configure(ConfigParams const& rParams) = 0;

    virtual MERROR                  flush()                                = 0;

    virtual MERROR                  submitRequest(
                                        android::sp<BufferReceiveNode::IRequest> pRequest
                                    )                                      = 0;
};


/******************************************************************************
 *
 ******************************************************************************/
};  //namespace v3
};  //namespace NSCam
#endif  //_MTK_HARDWARE_INCLUDE_MTKCAM_V3_HWNODE_BUFFERRECEIVENODE_H_