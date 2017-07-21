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

#define LOG_TAG "MtkCam/BufferPoolImp"
//
#include "MyUtils.h"
//
#include <utils/RWLock.h>
#include <utils/Thread.h>
//
#include <sys/prctl.h>
#include <sys/resource.h>
//
#include <utils/include/ImageBufferHeap.h>
#include <v3/utils/streaminfo/ImageStreamInfo.h>
#include <v3/stream/IStreamInfo.h>
//
#include <LegacyPipeline/buffer/StreamBufferProvider.h>
#include <utils/include/Format.h>

#include <LegacyPipeline/buffer/BufferPoolImp.h>

using namespace android;
using namespace NSCam;
using namespace NSCam::v1;
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
#define BUFFERPOOL_NAME       ("Cam@v1BufferPool")
#define BUFFERPOOL_POLICY     (SCHED_OTHER)
#define BUFFERPOOL_PRIORITY   (0)


/******************************************************************************
 *
 ******************************************************************************/
BufferPoolImp::
BufferPoolImp(
    sp<IImageStreamInfo> pStreamInfo
)
    : mLogLevel(1)
    , mpStreamInfo(pStreamInfo)
{}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
BufferPoolImp::
acquireFromPool(
    char const*           szCallerName,
    MINT32                rRequestNo,
    sp<IImageBufferHeap>& rpBuffer,
    MUINT32&              rTransform
)
{
    MY_LOGD_IF( mLogLevel >= 1, "%s", szCallerName);

    Mutex::Autolock _l(mPoolLock);
    return getBufferFromPool_l(
                      szCallerName,
                      rpBuffer,
                      rTransform
                   );

}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
BufferPoolImp::
getBufferFromPool_l(
    char const*           szCallerName,
    sp<IImageBufferHeap>& rpBuffer,
    MUINT32&              rTransform
)
{
    FUNC_START;

    MY_LOGD("[%s] mAvailableBuf:%d mInUseBuf:%d",
            szCallerName, mAvailableBuf.size(), mInUseBuf.size());
    if( !mAvailableBuf.empty() )
    {
        typename List< sp<IImageBufferHeap> >::iterator iter = mAvailableBuf.begin();
        mInUseBuf.push_back(*iter);
        rpBuffer = *iter;
        rTransform = mpStreamInfo->getTransform();
        mAvailableBuf.erase(iter);
        //
        FUNC_END;
        return OK;
    }

    FUNC_END;
    return TIMED_OUT;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
BufferPoolImp::
releaseToPool(
    char const*          szCallerName,
    MINT32               rRequestNo,
    sp<IImageBufferHeap> rpBuffer,
    MUINT64              rTimeStamp,
    bool                 rErrorResult
)
{
    MY_LOGD_IF( mLogLevel >= 1, "%s", szCallerName);

    //FUNC_START;

    Mutex::Autolock _l(mPoolLock);
    return returnBufferToPool_l(
                      szCallerName,
                      rpBuffer
                   );
    //FUNC_END;

    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
BufferPoolImp::
returnBufferToPool_l(
    char const* szCallerName,
    sp<IImageBufferHeap> rpBuffer
)
{
    FUNC_START;
    if(mInUseBuf.size()==0)
    {
        MY_LOGE("mInUseBuf is empty!  [%s] mAvailableBuf:%d", szCallerName, mAvailableBuf.size());
        FUNC_END;
        return UNKNOWN_ERROR;
    }
    typename List< sp<IImageBufferHeap> >::iterator iter = mInUseBuf.begin();
    while( iter != mInUseBuf.end() ) {
        if ( rpBuffer == (*iter) ) {
            mAvailableBuf.push_back(*iter);
            mInUseBuf.erase(iter);

            MY_LOGD_IF(
                mLogLevel >= 1,
                "[%s] mAvailableBuf:%d mInUseBuf:%d return buf:%p", szCallerName, mAvailableBuf.size(), mInUseBuf.size(), rpBuffer.get()
            );
            FUNC_END;
            return OK;
        }
        iter++;
    }

    MY_LOGE("[%s] Cannot find buffer %p.", szCallerName, rpBuffer.get());

    FUNC_END;

    return UNKNOWN_ERROR;
}

/******************************************************************************
 *
 ******************************************************************************/
char const*
BufferPoolImp::
poolName() const
{
    return (mpStreamInfo == 0) ? mpStreamInfo->getStreamName() : "Pool";
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
BufferPoolImp::
dumpPool() const
{
#warning "TODO"
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
BufferPoolImp::
allocateBuffer(
    char const* szCallerName,
    size_t maxNumberOfBuffers,
    size_t minNumberOfInitialCommittedBuffers
)
{
    FUNC_START;

    if ( mpStreamInfo == 0 ) {
        MY_LOGE("No ImageStreamInfo.");
        return UNKNOWN_ERROR;
    }

    mMaxBuffer = maxNumberOfBuffers;
    mMinBuffer = minNumberOfInitialCommittedBuffers;

    if ( mMinBuffer > mMaxBuffer) {
        MY_LOGE("mMinBuffer:%d > mMaxBuffer:%d", mMinBuffer, mMaxBuffer);
        return UNKNOWN_ERROR;
    }

    for (size_t i = 0; i < mMinBuffer; ++i ) {
        sp<IImageBufferHeap> pHeap;
        if( do_construct(pHeap) == NO_MEMORY ) {
            MY_LOGE("do_construct allocate buffer failed");
            continue;
        }
        {
            Mutex::Autolock _l(mPoolLock);
            mAvailableBuf.push_back(pHeap);
        }
    }

    status_t status = run();
    if  ( OK != status ) {
        MY_LOGE("Fail to run the thread - status:%d(%s)", status, ::strerror(-status));
        return UNKNOWN_ERROR;
    }

    MY_LOGD("%s (min,max):(%d,%d)", szCallerName, minNumberOfInitialCommittedBuffers, maxNumberOfBuffers);

    FUNC_END;
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
void
BufferPoolImp::
onLastStrongRef(const void* /*id*/)
{
    requestExit();
    //
    mAvailableBuf.clear();

    if ( mInUseBuf.size() > 0 ) {
        typename List< sp<IImageBufferHeap> >::iterator iter = mInUseBuf.begin();
        while( iter != mInUseBuf.end() ) {
            MY_LOGW("[%s] buffer %p not return to pool.", poolName(), (*iter).get());
            iter++;
        }
    }

    mInUseBuf.clear();

}

/******************************************************************************
 *
 ******************************************************************************/
void
BufferPoolImp::
requestExit()
{
    //let allocate thread back
    Thread::requestExitAndWait();
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
BufferPoolImp::
readyToRun()
{
    // set name
    ::prctl(PR_SET_NAME, (unsigned long)BUFFERPOOL_NAME, 0, 0, 0);

    int policy, priority;
    setThreadPriority(BUFFERPOOL_POLICY, BUFFERPOOL_PRIORITY);
    getThreadPriority(policy, priority);

    MY_LOGD(
        "tid(%d) policy(%d/%d) priority(%d/%d)"
        , ::gettid(), policy, BUFFERPOOL_POLICY
        , priority, BUFFERPOOL_PRIORITY
    );

    //
    return OK;

}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
BufferPoolImp::
do_construct(
    sp<IImageBufferHeap>& pImageBufferHeap
)
{
    IImageStreamInfo::BufPlanes_t const& bufPlanes = mpStreamInfo->getBufPlanes();
    size_t bufStridesInBytes[3] = {0};
    size_t bufBoundaryInBytes[3]= {0};
    for (size_t i = 0; i < bufPlanes.size(); i++) {
        bufStridesInBytes[i] = bufPlanes[i].rowStrideInBytes;
    }

    if ( eImgFmt_JPEG == mpStreamInfo->getImgFormat() ||
         eImgFmt_BLOB == mpStreamInfo->getImgFormat() )
    {
        IImageBufferAllocator::ImgParam imgParam(
                mpStreamInfo->getImgSize(),
                (*bufStridesInBytes),
                (*bufBoundaryInBytes));
        imgParam.imgFormat = eImgFmt_BLOB;
        MY_LOGD("eImgFmt_JPEG -> eImgFmt_BLOB");
        pImageBufferHeap = IIonImageBufferHeap::create(
                                mpStreamInfo->getStreamName(),
                                imgParam,
                                IIonImageBufferHeap::AllocExtraParam(),
                                MFALSE
                            );
    }
    else
    {
        IImageBufferAllocator::ImgParam imgParam(
            mpStreamInfo->getImgFormat(),
            mpStreamInfo->getImgSize(),
            bufStridesInBytes, bufBoundaryInBytes,
            bufPlanes.size()
            );
        MY_LOGD("format:%x, size:(%d,%d), stride:%d, boundary:%d, planes:%d",
            mpStreamInfo->getImgFormat(),
            mpStreamInfo->getImgSize().w, mpStreamInfo->getImgSize().h,
            bufStridesInBytes[0], bufBoundaryInBytes[0], bufPlanes.size()
            );
        pImageBufferHeap = IIonImageBufferHeap::create(
                                mpStreamInfo->getStreamName(),
                                imgParam,
                                IIonImageBufferHeap::AllocExtraParam(),
                                MFALSE
                            );
    }

    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
bool
BufferPoolImp::
threadLoop()
{
    bool next = false;
    sp<IImageBufferHeap> pHeap;
    if( do_construct(pHeap) == NO_MEMORY ) {
        MY_LOGE("do_construct allocate buffer failed");
        return true;
    }

    {
        Mutex::Autolock _l(mPoolLock);
        mAvailableBuf.push_back(pHeap);

        next = (mAvailableBuf.size() + mInUseBuf.size() ) < mMaxBuffer;
    }
    return next;
}
