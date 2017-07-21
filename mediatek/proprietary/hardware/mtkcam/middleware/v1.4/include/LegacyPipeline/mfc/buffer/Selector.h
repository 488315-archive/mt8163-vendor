/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2015. All rights reserved.
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

#ifndef _MTK_HARDWARE_INCLUDE_MTKCAM_LEGACYPIPELINE_MFCSELECTOR_H_
#define _MTK_HARDWARE_INCLUDE_MTKCAM_LEGACYPIPELINE_MFCSELECTOR_H_

#include <LegacyPipeline/buffer/StreamBufferProvider.h>

// ---------------------------------------------------------------------------

namespace NSCam {
namespace v1 {
namespace NSLegacyPipeline {

// ---------------------------------------------------------------------------

class MfcSelector : public ISelector
{
public:
    MfcSelector();
    virtual ~MfcSelector() {};

    // interface of ISelector
    status_t selectResult(
            MINT32                requestNo,
            Vector<MetaItemSet>   results,
            sp<IImageBufferHeap>  heap,
            MBOOL                 errorResult);
    status_t getResult(
            MINT32&               requestNo,
            Vector<MetaItemSet>&  resultMeta,
            sp<IImageBufferHeap>& heap);
    status_t returnBuffer(sp<IImageBufferHeap> heap) final;
    status_t flush() final;
    status_t setPool(wp<NSCam::v1::IConsumerPool> pool) final;

    // interface of RefBase
    virtual void onLastStrongRef(const void* /*id*/);

    // interface of MfcSelector
    void setWaitRequestNo(const Vector<MINT32>& requestNo);

protected:
    struct ResultSet : public virtual VirtualLightRefBase
    {
        MINT32               requestNo;
        Vector<MetaItemSet>  resultMeta;
        sp<IImageBufferHeap> heap;
        MINT64               timestamp;

        ResultSet(
                MINT32 _requestNo,
                const Vector<MetaItemSet> _resultMeta,
                const sp<IImageBufferHeap>& _heap,
                MINT64 _timestamp)
            : requestNo(_requestNo),
              resultMeta(_resultMeta),
              heap(_heap),
              timestamp(_timestamp)
        {}
    };

    mutable Mutex     mResultSetLock;
    mutable Condition mCondResultSet;
    Vector< sp<ResultSet> > mResultSet;
    Vector<MINT32>          mWaitRequestNo;

    mutable Mutex     mBufferInUseLock;
    size_t            mBufferInUse;

    wp<IConsumerPool> mPool;
};

} // namespace NSLegacyPipeline
} // namespace v1
} // namespace NSCam

#endif // _MTK_HARDWARE_INCLUDE_MTKCAM_LEGACYPIPELINE_MFCSELECTOR_H_
