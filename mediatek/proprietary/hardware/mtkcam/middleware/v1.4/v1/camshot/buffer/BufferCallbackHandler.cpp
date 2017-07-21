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

#define LOG_TAG "MtkCam/BufferCallbackHandler"
//
#include "MyUtils.h"
//
#include <utils/RWLock.h>
#include <utils/Thread.h>
//
#include <sys/prctl.h>
#include <sys/resource.h>
//
#include <ImageBufferHeap.h>
#include <v3/utils/streaminfo/ImageStreamInfo.h>
#include <v3/stream/IStreamInfo.h>
//
#include <LegacyPipeline/buffer/StreamBufferProvider.h>
#include <Format.h>
//
#include <camshot/BufferCallbackHandler.h>
#include <LegacyPipeline/StreamId.h>

using namespace android;
using namespace android::MtkCamUtils;
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
BufferCallbackHandler::
BufferCallbackHandler(
    MINT32 rOpenId
)
    : mOpenId(rOpenId)
{}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
BufferCallbackHandler::
setImageStreamInfo(
    MUINT32 const        msg,
    sp<IImageStreamInfo> pStreamInfo,
    sp<IImageBuffer>     pBuffer
)
{
    Mutex::Autolock _l(mLock);
    StreamId_T streamId = pStreamInfo->getStreamId();
    MY_LOGD("streamId:%d msg 0x%x, buffer %p", streamId, msg, pBuffer.get());
    if ( mvBufferPool.indexOfKey(streamId) >= 0 ) {
        MY_LOGW("Should not set same stream:%d", streamId);
        return BAD_VALUE;
    }
    bool isExternalBuf = false;
    sp<CallbackBufferPool> pPool = new CallbackBufferPool(pStreamInfo);
    if(pBuffer==NULL)
    {
        pPool->allocateBuffer(
                  pStreamInfo->getStreamName(),
                  pStreamInfo->getMaxBufNum(),
                  pStreamInfo->getMinInitBufNum()
                  );
    }
    else
    {
        pPool->addBuffer(pBuffer);
        isExternalBuf = true;
    }
    //
    pPool->setNotifier(this);
    //
    mvBufferPool.add( streamId, Info_T{streamId, pPool} );
    //
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
BufferCallbackHandler::
onBufferAcquired(
    MINT32           rRequestNo,
    StreamId_T       rStreamId
)
{
    FUNC_START;
    Mutex::Autolock _l(mLock);
    MY_LOGD("rRequestNo:%d rStreamId:%d",rRequestNo,rStreamId);
    ssize_t index = mvOrder.indexOfKey(rRequestNo);
    if ( index < 0 ) {
        Vector<Buffer_T> v;
        v.push_back(Buffer_T{rRequestNo, rStreamId, false, false, NULL});
        mvOrder.add(rRequestNo, v);
    } else {
        mvOrder.editValueFor(rRequestNo).push_back(Buffer_T{rRequestNo, rStreamId, false, false, NULL});
    }
    FUNC_END;
}

/******************************************************************************
 *
 ******************************************************************************/
bool
BufferCallbackHandler::
onBufferReturned(
    MINT32                         rRequestNo,
    StreamId_T                     rStreamId,
    bool                           bErrorBuffer,
    android::sp<IImageBufferHeap>& rpBuffer
)
{
    FUNC_START;
    Mutex::Autolock _l(mLock);
    MY_LOGD("rRequestNo:%d rStreamId:%d bErrorBuffer:%d rpBuffer:%p", rRequestNo, rStreamId, bErrorBuffer, rpBuffer.get());
    bool bKeepBuf;
    MINT32 i, size;

    /* check RequestNO */
    if (mvOrder.indexOfKey(rRequestNo) < 0)
    {
        MY_LOGE("no find rRequestNo. Should not happen.");
        FUNC_END;
        return false;
    }

    /* Process the return buffer */
    size     = mvOrder.editValueFor(rRequestNo).size();
    bKeepBuf = (rStreamId == eSTREAMID_IMAGE_JPEG && size > 1); // must KEEP jpeg buffer until all other buffers are released
    sp<IImageCallback> pCb = mpCallback.promote();

    for (i = 0 ; i < size ; i++)
    {
        Buffer_T* buf = &mvOrder.editValueFor(rRequestNo).editItemAt(i);
        if(buf == 0)
        {
            MY_LOGE("buf is NULL. Should not happen.");
            FUNC_END;
            return false;
        }
        if (buf->streamId == rStreamId && !buf->isReturned)
        {
            buf->buffer = rpBuffer->createImageBuffer();
            buf->isError = bErrorBuffer;

            /* Keep buffer if it is Jpeg and some non-Jpeg buffers aren't received */
            if (bKeepBuf)
            {
                buf->isReturned = false;
            }
            else
            {
                /* callback current buffer */
                pCb->onResultReceived(buf->requestNo, buf->streamId, buf->isError, buf->buffer);
                MY_LOGD("onResultReceived(%d,%#" PRIxPTR ",%d,%p)",
                    buf->requestNo, buf->streamId, buf->isError, buf->buffer.get());
                buf->isReturned = true;
            }
            break;
        }
    }

    if (i == size)
    {
        MY_LOGE("Can't find streamID(%d). Should not happen.", rStreamId);
        FUNC_END;
        return false;
    }

    /* check Jpeg buffer, callback if the other buffer is for Jpeg */
    if (!bKeepBuf && size == 2)
    {
        for (i = 0 ; i < size ; i++)
        {
            Buffer_T* buf = &mvOrder.editValueFor(rRequestNo).editItemAt(i);
            if (!buf->isReturned && buf->buffer != NULL && buf->streamId == eSTREAMID_IMAGE_JPEG)
            {
                //callback Jpeg at last
                pCb->onResultReceived(buf->requestNo, buf->streamId, buf->isError, buf->buffer);
                MY_LOGD("onResultReceived(%d,%#" PRIxPTR ",%d,%p)",
                    buf->requestNo, buf->streamId, buf->isError, buf->buffer.get());
                buf->isReturned = true;
            }
        }
    }

    /* remove returned item for this RequestNo, remove this RequestNO from mvOrder if no item insides */
    Vector<Buffer_T>::iterator it;
    for (it = mvOrder.editValueFor(rRequestNo).begin() ; it != mvOrder.editValueFor(rRequestNo).end() ;)
    {
      if (it->isReturned)
        it = mvOrder.editValueFor(rRequestNo).erase(it);
      else
        it++;
    }
    if (mvOrder.editValueFor(rRequestNo).size() == 0) mvOrder.removeItem(rRequestNo);

    FUNC_END;
    return true;
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
BufferCallbackHandler::
setBufferPool( android::sp<CallbackBufferPool> pPool )
{
    if(pPool==NULL)
    {
        MY_LOGE("Pool is NULL!");
        return;
    }
    pPool->setNotifier(this);
    android::sp<IImageStreamInfo> pStreamInfo = pPool->getStreamInfo();
    if(pStreamInfo==NULL)
    {
        MY_LOGE("Pool has no StreamInfo!");
        return;
    }
    mvBufferPool.add( pStreamInfo->getStreamId(), Info_T{pStreamInfo->getStreamId(), pPool} );
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
BufferCallbackHandler::
onLastStrongRef(const void* /*id*/)
{
    FUNC_START;
    for(int i=0; i<mvBufferPool.size(); i++)
    {
        mvBufferPool.editValueAt(i).pool->flush();
    }
    FUNC_END;
}
