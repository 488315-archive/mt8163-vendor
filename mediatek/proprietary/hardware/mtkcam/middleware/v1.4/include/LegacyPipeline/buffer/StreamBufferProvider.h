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

#ifndef _MTK_HARDWARE_INCLUDE_MTKCAM_LEGACYPIPELINE_BUFFER_STREAMBUFFERPROVIDER_H_
#define _MTK_HARDWARE_INCLUDE_MTKCAM_LEGACYPIPELINE_BUFFER_STREAMBUFFERPROVIDER_H_
//
#include <utils/RefBase.h>
#include <utils/Vector.h>
//
#include <v3/stream/IStreamInfo.h>
#include <v3/utils/streaminfo/MetaStreamInfo.h>
#include <v3/utils/streaminfo/ImageStreamInfo.h>
#include <v3/utils/streambuf/StreamBuffers.h>
#include <v3/utils/streambuf/StreamBufferProvider.h>
#include <LegacyPipeline/processor/TimestampProcessor.h>
#include <v1/camutils/IBuffer.h>

typedef NSCam::v3::Utils::HalImageStreamBuffer      HalImageStreamBuffer;
typedef NSCam::v3::IImageStreamInfo                 IImageStreamInfo;
typedef NSCam::v3::Utils::IStreamBufferProvider     IStreamBufferProvider;

using namespace android::MtkCamUtils;

/******************************************************************************
 *
 ******************************************************************************/
//
namespace NSCam {
namespace v1 {

/******************************************************************************
 *
 ******************************************************************************/


class IBufferPool
    : public virtual android::RefBase
{
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Interface.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:   //// operations.

    /**
     * Try to acquire a buffer from the pool.
     *
     * @param[in] szCallerName: a null-terminated string for a caller name.
     *
     * @param[out] rpBuffer: a reference to a newly acquired buffer.
     *
     * @return 0 indicates success; non-zero indicates an error code.
     */
    virtual MERROR                      acquireFromPool(
                                            char const*                    szCallerName,
                                            MINT32                         rRequestNo,
                                            android::sp<IImageBufferHeap>& rpBuffer,
                                            MUINT32&                       rTransform
                                        )                                                       = 0;

    /**
     * Release a buffer to the pool.
     *
     * @param[in] szCallerName: a null-terminated string for a caller name.
     *
     * @param[in] pBuffer: a buffer to release.
     *
     * @return
     *      0 indicates success; non-zero indicates an error code.
     */
    virtual MERROR                      releaseToPool(
                                            char const*                   szCallerName,
                                            MINT32                        rRequestNo,
                                            android::sp<IImageBufferHeap> pBuffer,
                                            MUINT64                       rTimeStamp,
                                            bool                          rErrorResult
                                        )                                                       = 0;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Interface.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:    //// debug
    /**
     * Pool name.
     */
    virtual char const*                 poolName() const                                        = 0;

    /**
     * Dump information for debug.
     */
    virtual MVOID                       dumpPool() const                                        = 0;

};


/******************************************************************************
 *
 ******************************************************************************/
class IConsumerPool
    : public virtual android::RefBase
{
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Interface.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
    virtual android::status_t           returnBuffer(
                                            android::sp<IImageBufferHeap>    rpHeap
                                        )                                                       = 0;
};

/******************************************************************************
 *
 ******************************************************************************/
class ISelector
    : public virtual android::RefBase
{
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Interface.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
    struct MetaItemSet{
        StreamId_T id;
        IMetadata  meta;
    };

public:    //// interface to receive result from stream buffer provider.
    /**
     * Send result metadata & buffer to selector.
     * Should implement this interface to get result from StreamBufferProvider.
     *
     * @param[in] rRequestNo: request number.
     *
     * @param[in] rResult: request number of error result.
     *
     * @param[in] rpHeap: result buffer.
     *
     * @param[in] errorResult: result is error or not.
     *
     */
    virtual android::status_t           selectResult(
                                            MINT32                          rRequestNo,
                                            Vector<MetaItemSet>             rvResult,
                                            android::sp<IImageBufferHeap>   rpHeap,
                                            MBOOL                           errorResult
                                        )                                                       = 0;

public:    //// interface to get & return buffer.

    /**
     *
     * Interface for user who wants to get a set of
     * result [buffer, meta] from selector.
     *
     *
     * @return
     *      0 indicates success; otherwise failure.
     */
    virtual android::status_t           getResult(
                                            MINT32&                          rRequestNo,
                                            Vector<MetaItemSet>&             rvResultMeta,
                                            android::sp<IImageBufferHeap>&   rpHeap
                                        )                                                       = 0;

    /**
     *
     * Interface for user to return Buffer to selector.
     *
     * @return
     *      0 indicates success; otherwise failure.
     */
    virtual android::status_t           returnBuffer(
                                            android::sp<IImageBufferHeap>    rpHeap
                                        )                                                       = 0;

public:    //// interface for stream buffer provider.
    /**
     * Start return all buffer to buffer pool.
     * StreamBufferProvider will use this API.
     *
     */
    virtual android::status_t           flush()                                                 = 0;

    /**
     * use rpPool to return buffer to buffer pool.
     * StreamBufferProvider will use this API.
     *
     */
    virtual android::status_t           setPool(
                                            android::wp<IConsumerPool>      rpPool
                                        )                                                       = 0;
};

/******************************************************************************
 *
 ******************************************************************************/
class PairMetadata
    : public virtual android::RefBase
    , public ResultProcessor::IListener
{
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Interface.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:

    static android::sp<PairMetadata>        createInstance( const char* name );

    /**
     *
     * Get timestamp & buffer pair from stream buffer provider.
     *
     * @return
     *      0 indicates success; otherwise failure.
     */
    virtual android::status_t               getResultBufferFromProvider(
                                                MUINT32                       rRequestNo,
                                                android::sp<IImageBufferHeap> rpHep,
                                                MUINT64                       rTimeStamp,
                                                MBOOL                         rbErrorBuffer
                                            )                                                   = 0;

    /**
     *
     * Set number of metadata that need to pair with buffer.
     *
     * @return
     *      0 indicates success; otherwise failure.
     */
    virtual android::status_t               setNumberOfMetadata( MINT32 num )                   = 0;

    /**
     *
     * Set buffer select rule for consumer.
     * Will trigger flush for previous rule.
     *
     * @return
     *      0 indicates success; otherwise failure.
     */
    virtual android::status_t           setSelector(
                                            android::sp< ISelector > pRule
                                        )                                                       = 0;

    /**
     *
     * Query selector.
     *
     * @return
     *      0 indicates success; otherwise failure.
     */
    virtual android::sp< ISelector >    querySelector()                                         = 0;

    /**
     *
     * flush.
     *
     * @return
     *      0 indicates success; otherwise failure.
     */
    virtual android::status_t           flush()                                                 = 0;

};

/******************************************************************************
 *
 ******************************************************************************/
class StreamBufferProvider
    : public IStreamBufferProvider
    , public IConsumerPool
    , public ITimestampCallback
{
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Interface.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:

    virtual                             ~StreamBufferProvider() {};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Implementations.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:

    /**
     *
     * Set image stream info.
     *
     * @param[in] pStreamInfo : IImageStreamInfo.
     *
     * @return
     *      0 indicates success; otherwise failure.
     */
    virtual android::status_t           setImageStreamInfo(
                                            android::sp<IImageStreamInfo> pStreamInfo
                                        )                                                       = 0;

    /**
     *
     * Query image stream info.
     *
     * @param[out] pStreamInfo : IImageStreamInfo.
     *
     * @return
     *      0 indicates success; otherwise failure.
     */
    virtual android::sp<IImageStreamInfo> queryImageStreamInfo()                                = 0;

    /**
     *
     * Set buffer pool to buffer provider.
     *
     * @param[in] pBufProvider : buffer pool.
     *
     * @return
     *      0 indicates success; otherwise failure.
     */
    virtual android::status_t           setBufferPool(
                                            android::sp< IBufferPool >    pBufProvider
                                        )                                                       = 0;

    /**
     *
     * Set buffer select rule for consumer.
     * Will trigger flush for previous rule.
     *
     * @return
     *      0 indicates success; otherwise failure.
     */
    virtual android::status_t           setSelector(
                                            android::sp< ISelector > pRule
                                        )                                                       = 0;

    /**
     *
     * Query selector from stream buffer provider.
     *
     * @return
     *      0 indicates success; otherwise failure.
     */
    virtual android::sp< ISelector >    querySelector()                                         = 0;

    /**
     *
     * Return all buffer to pool.
     *
     * @return
     *      0 indicates success; otherwise failure.
     */
    virtual void                        flush()                                                 = 0;

};
};  //namespace v1
};  //namespace NSCam
#endif  //_MTK_HARDWARE_INCLUDE_MTKCAM_LEGACYPIPELINE_BUFFER_STREAMBUFFERPROVIDER_H_

