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

#define LOG_TAG "MtkCam/Selector"
//
#include "MyUtils.h"
//
#include <utils/RWLock.h>
//
#include <ImageBufferHeap.h>
#include <v3/utils/streaminfo/ImageStreamInfo.h>
#include <v3/stream/IStreamInfo.h>
//
#include <LegacyPipeline/buffer/Selector.h>
//
#include <camera_custom_zsd.h>


using namespace android;
using namespace NSCam;
using namespace NSCam::v1;
using namespace NSCam::v1::NSLegacyPipeline;
using namespace NSCam::Utils;

using namespace NSCam::v3::Utils;
/******************************************************************************
 *
 ******************************************************************************/
#define MY_LOGV(fmt, arg...)        CAM_LOGV("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_LOGD("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_LOGI("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_LOGW("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_LOGE("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGA(fmt, arg...)        CAM_LOGA("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGF(fmt, arg...)        CAM_LOGF("[%s] " fmt, __FUNCTION__, ##arg)
//
#define MY_LOGV_IF(cond, ...)       do { if ( (cond) ) { MY_LOGV(__VA_ARGS__); } }while(0)
#define MY_LOGD_IF(cond, ...)       do { if ( (cond) ) { MY_LOGD(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) ) { MY_LOGI(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) ) { MY_LOGW(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) ) { MY_LOGE(__VA_ARGS__); } }while(0)
#define MY_LOGA_IF(cond, ...)       do { if ( (cond) ) { MY_LOGA(__VA_ARGS__); } }while(0)
#define MY_LOGF_IF(cond, ...)       do { if ( (cond) ) { MY_LOGF(__VA_ARGS__); } }while(0)

#if 1
#define FUNC_START     MY_LOGD("+")
#define FUNC_END       MY_LOGD("-")
#else
#define FUNC_START
#define FUNC_END
#endif

/******************************************************************************
 *
 ******************************************************************************/
ZsdSelector::
ZsdSelector()
    : mDelayTimeMs(0)
    , mTolerationMs(0)
    , miLogLevel(1)
    , mUsingBufCount(0)
{
    miLogLevel = ::property_get_int32("debug.camera.selector.loglevel", 1);
    //
    mDelayTimeMs = get_zsd_cap_stored_delay_time_ms();
    mTolerationMs = get_zsd_cap_stored_delay_toleration_ms();
    //
    MY_LOGD("new ZsdSelector delay(%d) toler(%d)",mDelayTimeMs,mTolerationMs);
}

/******************************************************************************
 *
 ******************************************************************************/
android::status_t
ZsdSelector::
selectResult(
    MINT32                             rRequestNo,
    Vector<MetaItemSet>                rvResult,
    android::sp<IImageBufferHeap>      rpHeap,
    MBOOL                              errorResult
)
{
    Mutex::Autolock _l(mResultSetLock);

    sp<IConsumerPool> pPool = mpPool.promote();
    if ( pPool == 0 ) {
        MY_LOGE("Cannot promote consumer pool to return buffer.");
        return UNKNOWN_ERROR;
    }
    // get timestamp
    MINT64 newTimestamp = 0;
    for(int i=0; i<rvResult.size(); i++)
    {
        IMetadata::IEntry const entry = rvResult[i].meta.entryFor(MTK_SENSOR_TIMESTAMP);
        if(! entry.isEmpty())
        {
            newTimestamp = entry.itemAt(0, Type2Type<MINT64>());
            break;
        }
    }
    if( newTimestamp == 0 )
    {
        MY_LOGW("timestamp == 0");
    }
    //
#define MAX_USING_BUFFER (5)
    MY_LOGD_IF( (2<=miLogLevel), "new timestamp(%lld) mResultSetMap.size(%d) mUsingBufCount(%d) MAX_USING_BUFFER(%d)",
        newTimestamp,mResultSetMap.size(),mUsingBufCount,MAX_USING_BUFFER);
    if ( mResultSetMap.size() >= 1 )    //there are old buffer, need to check the timestamp
    {
        MY_LOGD_IF( (2<=miLogLevel), "new(%lld) old(%lld) the duration(%lld)",
            newTimestamp,
            mResultSetMap.editItemAt(0).timestamp,
            (newTimestamp - mResultSetMap.editItemAt(0).timestamp)/1000000);
        //
        //check previous buffer timestame
        if((newTimestamp - mResultSetMap.editItemAt(0).timestamp) > (mDelayTimeMs + mTolerationMs)*1000000) //the previous buffer is too long ago, need to be return
        {
            int stop = -1;
            int n = mResultSetMap.size();
            // check which previous buffers need to be returned
            for(int i=0; i<n; i++)
            {
                if(i==n-1 &&
                   (newTimestamp - mResultSetMap.editItemAt(i).timestamp) >= (mDelayTimeMs - mTolerationMs)*1000000 &&
                   (newTimestamp - mResultSetMap.editItemAt(i).timestamp) <= (mDelayTimeMs + mTolerationMs)*1000000 )
                {
                    break;
                }
                //
                if((newTimestamp - mResultSetMap.editItemAt(i).timestamp) <= (mDelayTimeMs - mTolerationMs)*1000000)
                {
                    stop--;
                    break;
                }
                else
                {
                    stop = i;
                }
            }
            //
            for(int i=0; i<n; i++)
            {
                MY_LOGD_IF( (2<=miLogLevel), "Map[%d] rNo(%d) heap(%p) timestamp(%lld) dur(%d)",
                    i,
                    mResultSetMap.editItemAt(i).requestNo,
                    mResultSetMap.editItemAt(i).heap.get(),
                    mResultSetMap.editItemAt(i).timestamp,
                    ((newTimestamp - mResultSetMap.editItemAt(i).timestamp)/1000000)
                    );
            }
            MY_LOGD_IF( (2<=miLogLevel), "stop (%d)", stop);
            //
            // return the too long ago buffers
            for(int i=0; i<=stop; i++)
            {
                MY_LOGD_IF( (2<=miLogLevel), "mResultSetMap.size:%d returnBuffer:%d (%p)",
                    mResultSetMap.size(),
                    mResultSetMap.editItemAt(0).requestNo,
                    mResultSetMap.editItemAt(0).heap.get());
                //
                pPool->returnBuffer( mResultSetMap.editItemAt(0).heap );
                mResultSetMap.removeItemsAt(0);
            }
        }
        // insert the new buffer
        if ( mResultSetMap.size() + mUsingBufCount >= MAX_USING_BUFFER )
        {
            if ( mResultSetMap.size() >= 1 ) {
                //
                MY_LOGD_IF( (2<=miLogLevel), "mResultSetMap.size:%d returnBuffer:%d", mResultSetMap.size(), mResultSetMap.editItemAt(0).requestNo);
                //
                pPool->returnBuffer( mResultSetMap.editItemAt(0).heap );
                mResultSetMap.removeItemsAt(0);
                //
                MY_LOGD_IF( (2<=miLogLevel), "add buffer:%d (%p)", rRequestNo, rpHeap.get());
                mResultSetMap.add( ResultSet_T{rRequestNo, rpHeap, rvResult, newTimestamp} );
                mCondResultSet.signal();
            }
            else
            {
                MY_LOGD_IF( (2<=miLogLevel), "immediately return new buffer:%d (%p)", rRequestNo, rpHeap.get());
                pPool->returnBuffer( rpHeap );
            }
        }
        else
        {
            MY_LOGD_IF( (2<=miLogLevel), "add buffer:%d (%p)", rRequestNo, rpHeap.get());
            mResultSetMap.add( ResultSet_T{rRequestNo, rpHeap, rvResult, newTimestamp} );
            mCondResultSet.signal();
        }
        //
        MY_LOGD_IF( (2<=miLogLevel), "final mResultSetMap.size:%d duration(%lld)",
            mResultSetMap.size(), (newTimestamp - mResultSetMap.editItemAt(0).timestamp)/1000000);
    }
    else    //no old buffer need to check, just insert the new buffer
    {
        if ( mUsingBufCount >= MAX_USING_BUFFER ) {
            //
            MY_LOGD_IF( (2<=miLogLevel), "immediately return new buffer:%d (%p)", rRequestNo, rpHeap.get());
            //
            pPool->returnBuffer( rpHeap );
        }
        else
        {
            MY_LOGD_IF( (2<=miLogLevel), "add buffer:%d (%p)", rRequestNo, rpHeap.get());
            //
            mResultSetMap.add( ResultSet_T{rRequestNo, rpHeap, rvResult, newTimestamp} );
            mCondResultSet.signal();
            //
            MY_LOGD_IF( (2<=miLogLevel), "final mResultSetMap.size:%d duration(%lld)",
                mResultSetMap.size(), (newTimestamp - mResultSetMap.editItemAt(0).timestamp)/1000000);
        }
    }
#undef MAX_USING_BUFFER
    //
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
ZsdSelector::
getResult(
    MINT32&                          rRequestNo,
    Vector<MetaItemSet>&             rvResultMeta,
    android::sp<IImageBufferHeap>&   rpHeap
)
{
    FUNC_START;
    Mutex::Autolock _l(mResultSetLock);

#define WAIT_BUFFER_TIMEOUT (3000000000) //ns
    if ( mResultSetMap.isEmpty() ) {
        MY_LOGD("No result, start waiting %lld ns", WAIT_BUFFER_TIMEOUT);
        mCondResultSet.waitRelative(mResultSetLock, WAIT_BUFFER_TIMEOUT);
        if ( mResultSetMap.isEmpty() ) {
            MY_LOGE("Timeout, no result can get");
            return UNKNOWN_ERROR;
        }
    }
#undef WAIT_BUFFER_TIMEOUT

    rRequestNo   = mResultSetMap.editItemAt(0).requestNo;
    rvResultMeta = mResultSetMap.editItemAt(0).resultMeta;
    rpHeap       = mResultSetMap.editItemAt(0).heap;

    mResultSetMap.removeItemsAt(0);
    mUsingBufCount++;

    FUNC_END;
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
ZsdSelector::
returnBuffer(
    android::sp<IImageBufferHeap>   rpHeap
)
{
    sp<IConsumerPool> pPool = mpPool.promote();
    if ( pPool == 0 ) {
        MY_LOGE("Cannot promote consumer pool to return buffer.");
        return UNKNOWN_ERROR;
    }

    MY_LOGD_IF( 1, "return buf:%p", rpHeap.get());
    mUsingBufCount--;
    return pPool->returnBuffer( rpHeap );
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
ZsdSelector::
flush()
{
    FUNC_START;
    Mutex::Autolock _l(mResultSetLock);

    sp<IConsumerPool> pPool = mpPool.promote();
    if ( pPool == 0 ) {
        MY_LOGE("Cannot promote consumer pool to return buffer.");
        FUNC_END;
        return UNKNOWN_ERROR;
    }

    for ( size_t i = 0; i < mResultSetMap.size(); ++i ) {
        pPool->returnBuffer( mResultSetMap.editItemAt(i).heap );
    }

    mResultSetMap.clear();

    FUNC_END;
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
void
ZsdSelector::
onLastStrongRef( const void* /*id*/)
{
    FUNC_START;
    flush();
    FUNC_END;
}

/******************************************************************************
 *
 ******************************************************************************/
void
ZsdSelector::
setDelayTime( MINT32 delayTimeMs, MINT32 tolerationTimeMs )
{
    FUNC_START;
    Mutex::Autolock _l(mResultSetLock);
    mDelayTimeMs = delayTimeMs;
    mTolerationMs = tolerationTimeMs;
    MY_LOGD("Set DelayTime (%d) ms Toleration (%d) ms", mDelayTimeMs, mTolerationMs);
    FUNC_END;
}


/******************************************************************************
 *
 ******************************************************************************/
ZsdFlashSelector::
ZsdFlashSelector()
    : bAlreadyGotResult(false)
{
    MY_LOGD("new ZsdFlashSelector");
}

/******************************************************************************
 *
 ******************************************************************************/
android::status_t
ZsdFlashSelector::
selectResult(
    MINT32                             rRequestNo,
    Vector<MetaItemSet>                rvResult,
    android::sp<IImageBufferHeap>      rpHeap,
    MBOOL                              errorResult
)
{
    FUNC_START;
    Mutex::Autolock _l(mResultSetLock);

    MY_LOGD("In ZsdFlashSelector selectResult rRequestNo(%d)",rRequestNo);

    sp<IConsumerPool> pPool = mpPool.promote();
    if ( pPool == 0 ) {
        MY_LOGE("Cannot promote consumer pool to return buffer.");
        return UNKNOWN_ERROR;
    }

    if(waitRequestNo == rRequestNo)
    {
        if( resultHeap != NULL )
        {
            pPool->returnBuffer( resultHeap );
        }
        resultHeap = rpHeap;
        resultMeta = rvResult;
        MY_LOGD("find data : rRequestNo(%d) rvResult size(%d) resultHeap(%p)",rRequestNo, rvResult.size(), resultHeap.get());
        mResultQueueCond.broadcast();
    }
    else
    {
        pPool->returnBuffer( rpHeap );
    }
    FUNC_END;
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
ZsdFlashSelector::
getResult(
    MINT32&                          rRequestNo,
    Vector<MetaItemSet>&             rvResultMeta,
    android::sp<IImageBufferHeap>&   rpHeap
)
{
    FUNC_START;
    Mutex::Autolock _l(mResultSetLock);

    MY_LOGD("In ZsdFlashSelector getResult waitRequestNo(%d)",waitRequestNo);

    nsecs_t timeout = 3000000000LL; //wait for 3 sec
    status_t status = mResultQueueCond.waitRelative(mResultSetLock,timeout);
    if(status != OK)
    {
        MY_LOGE("wait result timeout...");
        FUNC_END;
        return status;
    }

    rRequestNo   = waitRequestNo;
    rvResultMeta = resultMeta;
    rpHeap       = resultHeap;

    FUNC_END;
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
ZsdFlashSelector::
returnBuffer(
    android::sp<IImageBufferHeap>   rpHeap
)
{
    FUNC_START;
    Mutex::Autolock _l(mResultSetLock);
    MY_LOGD("In ZsdFlashSelector");
    sp<IConsumerPool> pPool = mpPool.promote();
    if ( pPool == 0 ) {
        MY_LOGE("Cannot promote consumer pool to return buffer.");
        return UNKNOWN_ERROR;
    }

    MY_LOGD_IF( 1, "return buf:%p", rpHeap.get());
    FUNC_END;
    return pPool->returnBuffer( rpHeap );
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
ZsdFlashSelector::
flush()
{
    FUNC_START;
    Mutex::Autolock _l(mResultSetLock);
    MY_LOGD("In ZsdFlashSelector");

    sp<IConsumerPool> pPool = mpPool.promote();
    if ( pPool == 0 ) {
        MY_LOGE("Cannot promote consumer pool to return buffer.");
        FUNC_END;
        return UNKNOWN_ERROR;
    }

    if(resultHeap!=NULL)
        pPool->returnBuffer(resultHeap);

    FUNC_END;
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
void
ZsdFlashSelector::
onLastStrongRef( const void* /*id*/)
{
    FUNC_START;
    MY_LOGD("In ZsdFlashSelector");
    flush();
    FUNC_END;
}

/******************************************************************************
 *
 ******************************************************************************/
void
ZsdFlashSelector::
setWaitRequestNo(MUINT32 requestNo)
{
    MY_LOGD("setWaitRequestNo(%d)",requestNo);
    waitRequestNo = requestNo;
}


/******************************************************************************
 *
 ******************************************************************************/
VssSelector::
VssSelector()
{
}

/******************************************************************************
 *
 ******************************************************************************/
android::status_t
VssSelector::
selectResult(
    MINT32                             rRequestNo,
    Vector<MetaItemSet>                rvResult,
    android::sp<IImageBufferHeap>      rpHeap,
    MBOOL                              errorResult
)
{
    Mutex::Autolock _l(mResultSetLock);

    sp<IConsumerPool> pPool = mpPool.promote();
    if ( pPool == 0 ) {
        MY_LOGE("Cannot promote consumer pool to return buffer.");
        return UNKNOWN_ERROR;
    }

    mResultSetMap.add( ResultSet_T{rRequestNo, rpHeap, rvResult} );

    MY_LOGD_IF( 1, "mResultSetMap.size:%d", mResultSetMap.size());
#if 1
    for ( size_t i = 0; i < mResultSetMap.size(); ++i )
        MY_LOGD_IF( 1, "mResultSetMap.size:%d request:%d", mResultSetMap.size(), mResultSetMap.editItemAt(i).requestNo );
#endif
    mCondResultSet.signal();
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
VssSelector::
getResult(
    MINT32&                          rRequestNo,
    Vector<MetaItemSet>&             rvResultMeta,
    android::sp<IImageBufferHeap>&   rpHeap
)
{

    Mutex::Autolock _l(mResultSetLock);
#define WAIT_BUFFER_TIMEOUT (3000000000) //ns
    if ( mResultSetMap.isEmpty() )
    {
        MY_LOGD("Wait result - E");
        mCondResultSet.waitRelative(mResultSetLock, WAIT_BUFFER_TIMEOUT);
        MY_LOGD("Wait result - X");
        if(mResultSetMap.isEmpty())
        {
            MY_LOGE("Time Out, no result can get.");
            return UNKNOWN_ERROR;
        }
    }
#undef WAIT_BUFFER_TIMEOUT
    rRequestNo   = mResultSetMap.editItemAt(0).requestNo;
    rvResultMeta = mResultSetMap.editItemAt(0).resultMeta;
    rpHeap       = mResultSetMap.editItemAt(0).heap;

    mResultSetMap.removeItemsAt(0);

    return OK;
}
/******************************************************************************
 *
 ******************************************************************************/
status_t
VssSelector::
returnBuffer(
    android::sp<IImageBufferHeap>   rpHeap
)
{
    FUNC_START;
    sp<IConsumerPool> pPool = mpPool.promote();
    if ( pPool == 0 )
    {
        MY_LOGE("Cannot promote consumer pool to return buffer.");
        return UNKNOWN_ERROR;
    }
    FUNC_END;
    return pPool->returnBuffer( rpHeap );
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
VssSelector::
flush()
{
    FUNC_START;
    Mutex::Autolock _l(mResultSetLock);

    sp<IConsumerPool> pPool = mpPool.promote();
    if ( pPool == 0 )
    {
        MY_LOGE("Cannot promote consumer pool to return buffer.");
        return UNKNOWN_ERROR;
    }

    for ( size_t i = 0; i < mResultSetMap.size(); ++i )
    {
        pPool->returnBuffer( mResultSetMap.editItemAt(i).heap );
    }

    mResultSetMap.clear();
    FUNC_END;
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
void
VssSelector::
onLastStrongRef( const void* /*id*/)
{
    FUNC_START;
    flush();
    FUNC_END;
}