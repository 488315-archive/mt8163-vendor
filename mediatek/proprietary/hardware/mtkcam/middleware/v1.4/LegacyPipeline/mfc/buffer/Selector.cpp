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

#define LOG_TAG "MtkCam/MfcSelector"

#include <LegacyPipeline/mfc/buffer/Selector.h>

#include <metadata/client/mtk_metadata_tag.h>

#include <utils/Timers.h>

#include <Log.h>
#include <Trace.h>

#define FUNCTION_LOG_START      CAM_LOGD("[%s] +", __FUNCTION__)
#define FUNCTION_LOG_END        CAM_LOGD("[%s] -", __FUNCTION__)

using namespace NSCam::v1;
using namespace NSCam::v1::NSLegacyPipeline;

// ---------------------------------------------------------------------------

MfcSelector::MfcSelector()
    : mBufferInUse(0)
{
}

status_t MfcSelector::selectResult(
            MINT32                requestNo,
            Vector<MetaItemSet>   resultMeta,
            sp<IImageBufferHeap>  heap,
            MBOOL                 /*errorResult*/)
{
    FUNCTION_LOG_START;

    CAM_LOGD("requestNo(%d)", requestNo);

    CAM_TRACE_CALL();

    Mutex::Autolock _l(mResultSetLock);

    sp<IConsumerPool> pool = mPool.promote();
    if (pool == NULL)
    {
        CAM_LOGE("promote consumer pool failed");
        return NO_INIT;
    }

    status_t err = OK;

    // get sensor's timestamp (nanosecond)
    const MINT64 timestamp = [&]() -> MINT64
    {
        for (size_t i = 0; i < resultMeta.size(); i++)
        {
            // refers to Camera HAL v3.2 properties - android.sensor.timestamp
            const IMetadata::IEntry entry =
                resultMeta[i].meta.entryFor(MTK_SENSOR_TIMESTAMP);
            if (!entry.isEmpty())
            {
                return entry.itemAt(0, Type2Type<MINT64>());
            }
        }

        // return 0 if cannot find timestamp
        return 0;
    }();
    CAM_LOGW_IF(timestamp <= 0, "timestamp should be larger than 0");

    // add the wanted buffer if found
    size_t index = -1;
    MBOOL found = MFALSE;
    for (size_t i = 0; i < mWaitRequestNo.size(); i++)
    {
        if (requestNo == mWaitRequestNo[i])
        {
            // the result metadata is paired
            // defined in in DefaultFlowControl/FeatureFlowControl
            if (resultMeta.size() != 2)
            {
                CAM_LOGE("result metadata from HDRSelector is incomplete");
                err = NOT_ENOUGH_DATA;
                pool->returnBuffer(heap);
                break;
            }

            index = i;
            found = MTRUE;
            break;
        }
    }

    if (found)
    {
        // the incomplete result metadata is what we want,
        // clear the pending preview request and add result metadata to result set
        mWaitRequestNo.removeAt(index);

        mResultSet.add(new ResultSet(requestNo, resultMeta, heap, timestamp));
        mCondResultSet.signal();
    }
    else
    {
        // the incoming result metadata is not what we want,
        // send back to pool directly
        err = pool->returnBuffer(heap);
    }

    FUNCTION_LOG_END;

    return err;
}

status_t MfcSelector::getResult(
            MINT32&               requestNo,
            Vector<MetaItemSet>&  resultMeta,
            sp<IImageBufferHeap>& heap)
{
    FUNCTION_LOG_START;

    CAM_TRACE_CALL();

    Mutex::Autolock _l(mResultSetLock);

    // wait for 10 second; exit when buffer received or timeout
    const nsecs_t timeout_ns = s2ns(10);
    if (mResultSet.isEmpty())
    {
        CAM_LOGD("no result, start waiting %lld s", ns2s(timeout_ns));
        mCondResultSet.waitRelative(mResultSetLock, timeout_ns);
        if (mResultSet.isEmpty())
        {
            CAM_LOGE("timeout, no result can get");
            return TIMED_OUT;
        }
    }

    const sp<ResultSet>& resultSet(mResultSet.editItemAt(0));
    if (resultSet.get() == NULL)
    {
        CAM_LOGE("result set is empty");
        return BAD_VALUE;
    }
    requestNo  = resultSet->requestNo;
    resultMeta = resultSet->resultMeta;
    heap       = resultSet->heap;
    mResultSet.removeAt(0);

    {
        Mutex::Autolock _l(mBufferInUseLock);
        mBufferInUse++;
    }

    FUNCTION_LOG_END;

    return OK;
}

status_t MfcSelector::returnBuffer(sp<IImageBufferHeap> heap)
{
    FUNCTION_LOG_START;

    CAM_TRACE_CALL();

    sp<IConsumerPool> pool = mPool.promote();
    if (pool == NULL)
    {
        CAM_LOGE("promote consumer pool failed");
        return NO_INIT;
    }

    {
        Mutex::Autolock _l(mBufferInUseLock);
        mBufferInUse--;
    }

    FUNCTION_LOG_END;

    return pool->returnBuffer(heap);
}

status_t MfcSelector::flush()
{
    FUNCTION_LOG_START;

    CAM_TRACE_CALL();

    Mutex::Autolock _l(mResultSetLock);

    sp<IConsumerPool> pool = mPool.promote();
    if (pool == NULL)
    {
        CAM_LOGE("promote consumer pool failed");
        return NO_INIT;
    }

    // return all buffers to buffer pool
    for (size_t i = 0; i < mResultSet.size(); ++i)
    {
        pool->returnBuffer(mResultSet.editItemAt(i)->heap);
    }

    mResultSet.clear();

    FUNCTION_LOG_END;

    return OK;
}

status_t MfcSelector::setPool(wp<IConsumerPool> pool)
{
    mPool = pool;
    return OK;
}

void MfcSelector::onLastStrongRef(const void* /*id*/)
{
    FUNCTION_LOG_START;

    flush();

    FUNCTION_LOG_END;
}

void MfcSelector::setWaitRequestNo(const Vector<MINT32>& requestNo)
{
    FUNCTION_LOG_START;

    Mutex::Autolock _l(mResultSetLock);

    mWaitRequestNo = requestNo;

    String8 msg("setWaitRequestNo -");
    for (size_t i = 0; i < mWaitRequestNo.size(); i++)
    {
        msg.appendFormat(" %d", mWaitRequestNo[i]);
    }

    CAM_LOGD("%s", msg.string());

    FUNCTION_LOG_END;
}
