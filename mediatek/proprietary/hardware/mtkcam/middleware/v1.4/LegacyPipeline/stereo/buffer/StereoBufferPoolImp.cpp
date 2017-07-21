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

#define LOG_TAG "MtkCam/StereoBufferPoolImp"
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
//
#include "StereoBufferPoolImp.h"

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

#define FUNC_START     MY_LOGD("+")
#define FUNC_END       MY_LOGD("-")

/******************************************************************************
 *
 ******************************************************************************/
#define BUFFERPOOL_NAME       ("Cam@v1StereoBufferPool")
#define BUFFERPOOL_POLICY     (SCHED_OTHER)
#define BUFFERPOOL_PRIORITY   (0)


/******************************************************************************
 *
 ******************************************************************************/
android::sp<StereoBufferPool>
StereoBufferPool::
createInstance( const char* name )
{
    return new StereoBufferPoolImp(name);
}
/******************************************************************************
 *
 ******************************************************************************/
StereoBufferPoolImp::
StereoBufferPoolImp(
    const char* name
)
    : mLogLevel(1)
    , mName(name)
{
    MY_LOGD("StereoBufferPoolImp ctor");
    mAvailableBuf.clear();
    mInUseBuf.clear();
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
StereoBufferPoolImp::
acquireFromPool(
    char const*           szCallerName,
    MINT32                rRequestNo,
    sp<IImageBufferHeap>& rpBuffer,
    MUINT32&              rTransform
)
{
    MY_LOGD_IF( mLogLevel >= 1, "%s", szCallerName);

    FUNC_START;

    Mutex::Autolock _l(mPoolLock);

    // get buffer from available set
    MY_LOGD("[%s] mAvailableBuf:%d mInUseBuf:%d",
            szCallerName, mAvailableBuf.size(), mInUseBuf.size());
    MBOOL found = MFALSE;
    if( !mAvailableBuf.empty() )
    {
        typename List< sp<IImageBufferHeap> >::iterator iter = mAvailableBuf.begin();
        mInUseBuf.push_back(*iter);
        rpBuffer = *iter;
        mAvailableBuf.erase(iter);
        //
        found = MTRUE;
    }

    if(!found){
        return UNKNOWN_ERROR;
    }

    FUNC_END;
    return OK;
}
/******************************************************************************
 *
 ******************************************************************************/
MERROR
StereoBufferPoolImp::
releaseToPool(
    char const*          szCallerName,
    MINT32               rRequestNo,
    sp<IImageBufferHeap> rpBuffer,
    MUINT64              rTimeStamp,
    bool                 rErrorResult
)
{
    MY_LOGD_IF( mLogLevel >= 1, "%s", szCallerName);

    FUNC_START;

    Mutex::Autolock _l(mPoolLock);

    // remove it from mInUseBuf
    MBOOL found = MFALSE;
    typename List< sp<IImageBufferHeap> >::iterator iter = mInUseBuf.begin();
    while( iter != mInUseBuf.end() ) {
        if ( rpBuffer == (*iter) ) {
            mInUseBuf.erase(iter);
            found = MTRUE;
            MY_LOGD_IF(
                mLogLevel >= 1,
                "[%s] mAvailableBuf:%d mInUseBuf:%d", szCallerName, mAvailableBuf.size(), mInUseBuf.size()
            );
            break;
        }
        iter++;
    }

    // return it to selector
    if(found){
        mspSelector->returnBuffer(rpBuffer);
        mCondPoolLock.signal();
    }else{
        MY_LOGE("buffer:%p not found in mInUseBuf, should not have happened!", rpBuffer.get());
        return UNKNOWN_ERROR;
    }

    FUNC_END;
    return OK;
}
/******************************************************************************
 *
 ******************************************************************************/
void
StereoBufferPoolImp::
onLastStrongRef(const void* /*id*/)
{
    MY_LOGD("StereoBufferPoolImp dtor");

    requestExit();

    // return all available buffer
    {
        Mutex::Autolock _l(mPoolLock);
        if ( mAvailableBuf.size() > 0 ) {
            typename List< sp<IImageBufferHeap> >::iterator iter = mAvailableBuf.begin();
            while( iter != mAvailableBuf.end() ) {
                mspSelector->returnBuffer((*iter));
                MY_LOGW("[%s] mAvailableBuf %p return to pool.", poolName(), (*iter).get());
                iter++;
            }
        }
    }

    // wait for all mInUseBuf returned
    {
        Mutex::Autolock _l(mPoolLock);
        while(mInUseBuf.size() > 0){
            MY_LOGD("wait for all mInUseBuf returned +, mInUseBuf.size()=%d ", mInUseBuf.size());
            mCondPoolLock.wait(mPoolLock);
        }
        MY_LOGD("wait for all mInUseBuf returned -, mInUseBuf.size()=%d", mInUseBuf.size());
    }
}
/******************************************************************************
 *
 ******************************************************************************/
MERROR
StereoBufferPoolImp::
setSelector(
    sp<ISelector> pSelector
)
{
    FUNC_START;

    mspSelector = pSelector;
    MY_LOGD("setSelector->mspSelector=%p", mspSelector.get());

    FUNC_END;
    return OK;
}
/******************************************************************************
 *
 ******************************************************************************/
MERROR
StereoBufferPoolImp::
setBuffer(
    char const* szCallerName,
    android::sp<IImageBufferHeap> pBuffer
)
{
    FUNC_START;

    Mutex::Autolock _l(mPoolLock);

    mAvailableBuf.push_back(pBuffer);

    MY_LOGD_IF(
                mLogLevel >= 1,
                "[%s] mAvailableBuf:%d mInUseBuf:%d", szCallerName, mAvailableBuf.size(), mInUseBuf.size()
            );

    FUNC_END;
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
char const*
StereoBufferPoolImp::
poolName() const
{
    return "StereoBufferPool";
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
StereoBufferPoolImp::
dumpPool() const
{
#warning "TODO"
}
/******************************************************************************
 *
 ******************************************************************************/
void
StereoBufferPoolImp::
requestExit()
{
    //let allocate thread back
    Thread::requestExitAndWait();
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
StereoBufferPoolImp::
readyToRun()
{
    // set name
    ::prctl(PR_SET_NAME, (unsigned long)BUFFERPOOL_NAME, 0, 0, 0);

    // set normal
    struct sched_param sched_p;
    sched_p.sched_priority = 0;
    ::sched_setscheduler(0, BUFFERPOOL_POLICY, &sched_p);
    ::setpriority(PRIO_PROCESS, 0, BUFFERPOOL_PRIORITY);
    //
    ::sched_getparam(0, &sched_p);

    MY_LOGD(
        "tid(%d) policy(%d) priority(%d)"
        , ::gettid(), ::sched_getscheduler(0)
        , sched_p.sched_priority
    );

    //
    return OK;

}
/******************************************************************************
 *
 ******************************************************************************/
bool
StereoBufferPoolImp::
threadLoop()
{
    bool next = false;
    // This thread was originally for allocating buffer at run-time,
    // we dont need it for now but keep this function for future demand
    return next;
}
