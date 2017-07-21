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

#define LOG_TAG "MtkCam/NCSSelector"
//
#include "MyUtils.h"
//
#include <utils/RWLock.h>
//
#include <ImageBufferHeap.h>
#include <v3/utils/streaminfo/ImageStreamInfo.h>
#include <v3/stream/IStreamInfo.h>
//
#include <LegacyPipeline/buffer/NormalCShotSelector.h>


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
NormalCShotSelector::
NormalCShotSelector()
    : mbHasResult(MFALSE)
    , mUsingBufCount(0)
{
}

/******************************************************************************
 *
 ******************************************************************************/
android::status_t
NormalCShotSelector::
selectResult(
    MINT32                          rRequestNo,
    Vector<MetaItemSet>             rvResult,
    android::sp<IImageBufferHeap>   rpHeap,
    MBOOL                           errorResult
)
{
    Mutex::Autolock _l(mResultSetLock);

    sp<IConsumerPool> pPool = mpPool.promote();
    if ( pPool == 0 ) {
        MY_LOGE("Cannot promote consumer pool to return buffer.");
        return UNKNOWN_ERROR;
    }

#define MAX_USING_BUFFER (3)
    if( mUsingBufCount >= MAX_USING_BUFFER )
    {
        MY_LOGD("mUsingBufCount >= MAX_USING_BUFFER(%d), returBuffer and leave", MAX_USING_BUFFER);
        pPool->returnBuffer( rpHeap );
        return OK;
    }
#undef MAX_USING_BUFFER

    if( mbHasResult )
    {
        pPool->returnBuffer( mResultSet.heap );
    }

    mResultSet.requestNo = rRequestNo;
    mResultSet.heap = rpHeap;
    mResultSet.resultMeta = rvResult;
    mbHasResult = MTRUE;
    MY_LOGD("mResultSetMap request:%d", mResultSet.requestNo );

    mCondResultSet.signal();

    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
NormalCShotSelector::
getResult(
    MINT32&                          rRequestNo,
    Vector<MetaItemSet>&             rvResultMeta,
    android::sp<IImageBufferHeap>&   rpHeap
)
{
    Mutex::Autolock _l(mResultSetLock);

#define WAIT_BUFFER_TIMEOUT (3000000000) //ns
    if ( !mbHasResult ) {
        MY_LOGD("No result, start waiting %lld ns", WAIT_BUFFER_TIMEOUT);
        mCondResultSet.waitRelative(mResultSetLock, WAIT_BUFFER_TIMEOUT);
        if ( !mbHasResult ) {
            MY_LOGE("Timeout, no result can get");
            return UNKNOWN_ERROR;
        }
    }
#undef WAIT_BUFFER_TIMEOUT

    rRequestNo   = mResultSet.requestNo;
    rvResultMeta = mResultSet.resultMeta;
    rpHeap       = mResultSet.heap;
    mUsingBufCount++;
    MY_LOGD("getResult, rRequestNo:%d, using count:%d", rRequestNo, mUsingBufCount);
    mbHasResult  = MFALSE;

    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
NormalCShotSelector::
returnBuffer(
    android::sp<IImageBufferHeap>   rpHeap
)
{
    Mutex::Autolock _l(mResultSetLock);

    sp<IConsumerPool> pPool = mpPool.promote();
    if ( pPool == 0 ) {
        MY_LOGE("Cannot promote consumer pool to return buffer.");
        return UNKNOWN_ERROR;
    }
    mUsingBufCount--;
    return pPool->returnBuffer( rpHeap );
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
NormalCShotSelector::
flush()
{
    Mutex::Autolock _l(mResultSetLock);

    sp<IConsumerPool> pPool = mpPool.promote();
    if ( pPool == 0 ) {
        MY_LOGE("Cannot promote consumer pool to return buffer, mbHasResult:%d", mbHasResult);
        return UNKNOWN_ERROR;
    }

    if( mbHasResult )
    {
        pPool->returnBuffer( mResultSet.heap );
    }
    mbHasResult = MFALSE;

    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
void
NormalCShotSelector::
onLastStrongRef( const void* /*id*/)
{
    FUNC_START;
    flush();
    FUNC_END;
}