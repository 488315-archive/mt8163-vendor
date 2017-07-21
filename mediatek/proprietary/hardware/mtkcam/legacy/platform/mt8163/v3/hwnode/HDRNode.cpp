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

#define LOG_TAG "MtkCam/HDRNode"
//
#include <mtkcam/Log.h>
#include "BaseNode.h"
#include <mtkcam/v3/hwnode/HDRNode.h>
#include <mtkcam/metadata/client/mtk_metadata_tag.h>
//
#include <utils/RWLock.h>
#include <utils/Thread.h>
//
#include <sys/prctl.h>
#include <sys/resource.h>

//
using namespace android;
using namespace NSCam;
using namespace NSCam::v3;
using namespace NSCam::Utils::Sync;


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


/******************************************************************************
 *
 ******************************************************************************/
namespace {
/******************************************************************************
 *
 ******************************************************************************/
class HDRNodeImp
    : public BaseNode
    , public HDRNode
    , protected Thread
{
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Implementations.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////                                            Definitions.
    typedef android::sp<IPipelineFrame>                     QueNode_T;
    typedef android::List<QueNode_T>                        Que_T;
    typedef android::Vector<android::sp<IMetaStreamInfo> >  MetaStreamInfoSetT;
    typedef android::Vector<android::sp<IImageStreamInfo> > ImageStreamInfoSetT;

protected:  ////                    Data Members. (Config)
    mutable RWLock                  mConfigRWLock;
    sp<IMetaStreamInfo>             mpMetaStreamInfo_InApp;
    sp<IMetaStreamInfo>             mpMetaStreamInfo_InHal;
    sp<IMetaStreamInfo>             mpMetaStreamInfo_OutApp;
    sp<IMetaStreamInfo>             mpMetaStreamInfo_OutHal;
    sp<IImageStreamInfo>            mpImgStreamInfo_Raw;
    ImageStreamInfoSetT             mvImgStreamInfo_Yuv;

public:     ////                    Operations.
                                    HDRNodeImp();
    virtual                        ~HDRNodeImp();

    virtual MERROR                  init(InitParams const& rParams);
    virtual MERROR                  config(ConfigParams const& rParams);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  IPipelineNode Interface.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////                    Operations.

    virtual MERROR                  uninit();

    virtual MERROR                  flush();

    virtual MERROR                  queue(
                                        sp<IPipelineFrame> pFrame
                                    );

protected:  ////                    Operations.
    MERROR                          onDequeRequest(
                                        android::sp<IPipelineFrame>& rpFrame
                                    );
    MVOID                           onProcessFrame(
                                        android::sp<IPipelineFrame> const& pFrame
                                    );

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Operations in base class Thread
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////
    // Ask this object's thread to exit. This function is asynchronous, when the
    // function returns the thread might still be running. Of course, this
    // function can be called from a different thread.
    virtual void                    requestExit();

    // Good place to do one-time initializations
    virtual status_t                readyToRun();

private:
    // Derived class must implement threadLoop(). The thread starts its life
    // here. There are two ways of using the Thread object:
    // 1) loop: if threadLoop() returns true, it will be called again if
    //          requestExit() wasn't called.
    // 2) once: if threadLoop() returns false, the thread will exit upon return.
    virtual bool                    threadLoop();

protected:  ////                    Data Members. (Request Queue)
    mutable Mutex                   mRequestQueueLock;
    Condition                       mRequestQueueCond;
    Que_T                           mRequestQueue;
};
};  //namespace


/******************************************************************************
 *
 ******************************************************************************/
android::sp<HDRNode>
HDRNode::
createInstance()
{
    return new HDRNodeImp();
}

/******************************************************************************
 *
 ******************************************************************************/
android::Vector< HDRNode::CaptureParam_T >*
HDRNode::
getCaptureParamSet()
{
    android::Vector< HDRNode::CaptureParam_T > vCaptureParams;
    HDRNode::CaptureParam_T pCaptureParam;
    pCaptureParam.u4ExposureMode   = 0;
    pCaptureParam.u4Eposuretime    = 0;    //!<: Exposure time in us
    pCaptureParam.u4AfeGain        = 0;    //!<: sensor gain
    pCaptureParam.u4IspGain        = 0;    //!<: raw gain
    pCaptureParam.u4RealISO        = 0;    //!<: Real ISO speed
    pCaptureParam.u4FlareOffset    = 0;
    pCaptureParam.u4FlareGain      = 0;    // 512 is 1x
    pCaptureParam.i4LightValue_x10 = 0;    // 10 base LV value
    pCaptureParam.u43ACapMode      = 0;    //refer to enum E3ACaptureMode_T
    pCaptureParam.i4YuvEvIdx       = 0;    // for yuv sensor

    vCaptureParams.add(pCaptureParam);
    vCaptureParams.add(pCaptureParam);
    vCaptureParams.add(pCaptureParam);

    return &vCaptureParams;
}


/******************************************************************************
 *
 ******************************************************************************/
HDRNodeImp::
HDRNodeImp()
{
}


/******************************************************************************
 *
 ******************************************************************************/
HDRNodeImp::
~HDRNodeImp()
{
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
HDRNodeImp::
init(InitParams const& rParams)
{
    {
        RWLock::AutoWLock _l(mConfigRWLock);
        //
        mOpenId  = rParams.openId;
        mNodeId  = rParams.nodeId;
        mNodeName= rParams.nodeName;
    }
    //
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
HDRNodeImp::
uninit()
{
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
HDRNodeImp::
config(ConfigParams const& rParams)
{
    // check ConfigParams
    if  ( ! rParams.pInAppMeta.get() ||
          ! rParams.pInHalMeta.get() ||
          ! rParams.pOutAppMeta.get() ||
          /*! rParams.pOutHalMeta ||*/
          ! rParams.pvInFullRaw[0].get() ||
          ! rParams.vOutImage.size()
          )
    {
        MY_LOGE("wrong config params: meta %p/%p/%p/%p, raw %p, out yuv cnt %d",
                rParams.pInAppMeta.get(),
                rParams.pInHalMeta.get(),
                rParams.pOutAppMeta.get(),
                rParams.pOutHalMeta.get(),
                rParams.pvInFullRaw[0].get(),
                rParams.vOutImage.size()
                );
        return BAD_VALUE;
    }
    //
    RWLock::AutoWLock _l(mConfigRWLock);
    //
    mpMetaStreamInfo_InApp  = rParams.pInAppMeta;
    mpMetaStreamInfo_InHal  = rParams.pInHalMeta;
    mpMetaStreamInfo_OutApp = rParams.pOutAppMeta;
    mpMetaStreamInfo_OutHal = rParams.pOutHalMeta; // not necessary
    mpImgStreamInfo_Raw     = rParams.pvInFullRaw[0];
    mvImgStreamInfo_Yuv     = rParams.vOutImage;
    //
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
HDRNodeImp::
flush()
{
    MY_LOGW("Not Implement !!!");

    return INVALID_OPERATION;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
HDRNodeImp::
queue(
    android::sp<IPipelineFrame> pFrame
)
{
    Mutex::Autolock _l(mRequestQueueLock);
    //
    //  Make sure the request with a smaller frame number has a higher priority.
    Que_T::iterator it = mRequestQueue.end();
    for (; it != mRequestQueue.begin(); ) {
        --it;
        if  ( 0 <= (MINT32)(pFrame->getFrameNo() - (*it)->getFrameNo()) ) {
            ++it;   //insert(): insert before the current node
            break;
        }
    }
    mRequestQueue.insert(it, pFrame);
    mRequestQueueCond.broadcast();
    //
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
HDRNodeImp::
onProcessFrame(
    android::sp<IPipelineFrame> const& pFrame
)
{
    MY_LOGD("frameNo:%"PRIxPTR" nodeId:%"PRIxPTR" +", pFrame->getFrameNo(), getNodeId());
    //
    IPipelineFrame::InfoIOMapSet aIOMapSet;
    IPipelineFrame::ImageInfoIOMapSet const& rImageIOMapSet = aIOMapSet.mImageInfoIOMapSet;
    IPipelineFrame::MetaInfoIOMapSet const&  rMetaIOMapSet = aIOMapSet.mMetaInfoIOMapSet;
    //
    pFrame->queryInfoIOMapSet(getNodeId(), aIOMapSet);
    IStreamBufferSet& rStreamBufferSet = pFrame->getStreamBufferSet();


    ////////////////////////////////////////////////////////////////////////////
    //  Ensure buffers available.
    ////////////////////////////////////////////////////////////////////////////

    //  Image
    {
        IPipelineFrame::ImageInfoIOMapSet const& rMapSet = rImageIOMapSet;
        for (size_t i = 0; i < rMapSet.size(); i++)
        {
            // get input image buffer
            for (size_t j = 0; j < rMapSet[i].vIn.size(); j++)
            {
                StreamId_T const streamId = rMapSet[i].vIn.keyAt(j);
                MY_LOGD("[Image] IN streamId:%"PRIxPTR, streamId);
                //
                sp<IImageStreamBuffer> pStreamBuffer = NULL;
                sp<IImageBuffer> pImageBuffer = NULL;
                MERROR err = ensureImageBufferAvailable_(
                        pFrame->getFrameNo(),
                        streamId,
                        rStreamBufferSet,
                        pStreamBuffer
                        );
                if( err != OK ) {
                    // TODO: some error handling
                }
#if 0 // get imagebuffer from streambuffer
                //
                MUINT const groupUsage = pStreamBuffer->queryGroupUsage(getNodeId());
                sp<IImageBufferHeap>  pOutpImageBufferHeap = pStreamBuffer->tryReadLock(getNodeName());
                if (pOutpImageBufferHeap == NULL) {
                    MY_LOGE("pOutpImageBufferHeap == NULL");
                    return BAD_VALUE;
                }
                pImageBuffer = pOutpImageBufferHeap->createImageBuffer();
                pImageBuffer->lockBuf(getNodeName(), groupUsage);
                //
                // TODO: some process
                //
                // unlock image buffer
                pImageBuffer->unlockBuf(getNodeName());
                // unlock streambuffer
                pStreamBuffer->unlock(getNodeName(), pOutpImageBufferHeap);
#endif
            }
            // get output image buffer
            for (size_t j = 0; j < rMapSet[i].vOut.size(); j++)
            {
                StreamId_T const streamId = rMapSet[i].vOut.keyAt(j);
                MY_LOGD("[Image] OUT streamId:%"PRIxPTR, streamId);
                //
                sp<IImageStreamBuffer> pStreamBuffer = NULL;
                sp<IImageBuffer> pImageBuffer = NULL;
                MERROR err = ensureImageBufferAvailable_(
                        pFrame->getFrameNo(),
                        streamId,
                        rStreamBufferSet,
                        pStreamBuffer
                        );
                if( err != OK ) {
                    // TODO: some error handling
                }
#if 0
                //
                MUINT const groupUsage = pStreamBuffer->queryGroupUsage(getNodeId());
                sp<IImageBufferHeap>  pOutpImageBufferHeap = pStreamBuffer->tryWriteLock(getNodeName());
                if (pOutpImageBufferHeap == NULL) {
                    MY_LOGE("pOutpImageBufferHeap == NULL");
                    return BAD_VALUE;
                }
                pImageBuffer = pOutpImageBufferHeap->createImageBuffer();
                pImageBuffer->lockBuf(getNodeName(), groupUsage);
                //
                // TODO: some process
                //
                // unlock image buffer
                pImageBuffer->unlockBuf(getNodeName());
                // unlock streambuffer
                pStreamBuffer->unlock(getNodeName(), pOutpImageBufferHeap);
#endif
            }
        }
    }
    //
    //  Meta
    {
        IPipelineFrame::MetaInfoIOMapSet const& rMapSet = rMetaIOMapSet;
        for (size_t i = 0; i < rMapSet.size(); i++)
        {
            //
            for (size_t j = 0; j < rMapSet[i].vIn.size(); j++)
            {
                StreamId_T const streamId = rMapSet[i].vIn.keyAt(j);
                MY_LOGD("[Meta] IN streamId:%"PRIxPTR, streamId);
                //
                sp<IMetaStreamBuffer> pStreamBuffer = NULL;
                MERROR err = ensureMetaBufferAvailable_(
                        pFrame->getFrameNo(),
                        streamId,
                        rStreamBufferSet,
                        pStreamBuffer
                        );
                if( err != OK ) {
                    // TODO: some error handling
                }
                //
#if 0
                IMetadata* pInMetadata = *pStreamBuffer->tryReadLock(getNodeName());
                //
                // TODO: access
                //
                pStreamBuffer->unlock(getNodeName(), pInMetadata);
#endif
            }
            //
            for (size_t j = 0; j < rMapSet[i].vOut.size(); j++)
            {
                StreamId_T const streamId = rMapSet[i].vOut.keyAt(j);
                MY_LOGD("[Meta] OUT streamId:%"PRIxPTR, streamId);
                //
                sp<IMetaStreamBuffer> pStreamBuffer = NULL;
                MERROR err = ensureMetaBufferAvailable_(
                        pFrame->getFrameNo(),
                        streamId,
                        rStreamBufferSet,
                        pStreamBuffer
                        );
                if( err != OK ) {
                    // TODO: some error handling
                }
                //
#if 0
                IMetadata* pOutMetadata = pStreamBuffer->tryWriteLock(getNodeName());
                //
                // TODO: access
                //
                pStreamBuffer->unlock(getNodeName(), pOutMetadata);
#endif
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////
    //  Access buffers.
    ////////////////////////////////////////////////////////////////////////////

    // TODO: do some process

    //  Image
    {
        IPipelineFrame::ImageInfoIOMapSet const& rMapSet = rImageIOMapSet;
        for (size_t i = 0; i < rMapSet.size(); i++)
        {
            //
            for (size_t j = 0; j < rMapSet[i].vOut.size(); j++)
            {
                StreamId_T const streamId = rMapSet[i].vOut.keyAt(j);
                //
                sp<IImageStreamBuffer> pStreamBuffer = rStreamBufferSet.getImageBuffer(streamId, getNodeId());
                pStreamBuffer->markStatus(STREAM_BUFFER_STATUS::WRITE_OK);
#if 0
                pStreamBuffer->markStatus(STREAM_BUFFER_STATUS::WRITE_ERROR);
#endif
            }
        }
    }

    //  Meta
    {
        IPipelineFrame::MetaInfoIOMapSet const& rMapSet = rMetaIOMapSet;
        for (size_t i = 0; i < rMapSet.size(); i++)
        {
            for (size_t j = 0; j < rMapSet[i].vOut.size(); j++)
            {
                StreamId_T const streamId = rMapSet[i].vOut.keyAt(j);
                //
                sp<IMetaStreamBuffer> pStreamBuffer = rStreamBufferSet.getMetaBuffer(streamId, getNodeId());
                pStreamBuffer->markStatus(STREAM_BUFFER_STATUS::WRITE_OK);
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    //  Release buffers after using.
    ////////////////////////////////////////////////////////////////////////////

    //  Image
    {
        IPipelineFrame::ImageInfoIOMapSet const& rMapSet = rImageIOMapSet;
        for (size_t i = 0; i < rMapSet.size(); i++)
        {
            //
            for (size_t j = 0; j < rMapSet[i].vIn.size(); j++)
            {
                StreamId_T const streamId = rMapSet[i].vIn.keyAt(j);
                rStreamBufferSet.markUserStatus(
                    streamId, getNodeId(),
                    IUsersManager::UserStatus::USED |
                    IUsersManager::UserStatus::RELEASE
                );
            }
            //
            for (size_t j = 0; j < rMapSet[i].vOut.size(); j++)
            {
                StreamId_T const streamId = rMapSet[i].vOut.keyAt(j);
                rStreamBufferSet.markUserStatus(
                    streamId, getNodeId(),
                    IUsersManager::UserStatus::USED |
                    IUsersManager::UserStatus::RELEASE
                );
            }
        }
    }
    //
    //  Meta
    {
        IPipelineFrame::MetaInfoIOMapSet const& rMapSet = rMetaIOMapSet;
        for (size_t i = 0; i < rMapSet.size(); i++)
        {
            //
            for (size_t j = 0; j < rMapSet[i].vIn.size(); j++)
            {
                StreamId_T const streamId = rMapSet[i].vIn.keyAt(j);
                rStreamBufferSet.markUserStatus(
                    streamId, getNodeId(),
                    IUsersManager::UserStatus::USED |
                    IUsersManager::UserStatus::RELEASE
                );
            }
            //
            for (size_t j = 0; j < rMapSet[i].vOut.size(); j++)
            {
                StreamId_T const streamId = rMapSet[i].vOut.keyAt(j);
                rStreamBufferSet.markUserStatus(
                    streamId, getNodeId(),
                    IUsersManager::UserStatus::USED |
                    IUsersManager::UserStatus::RELEASE
                );
            }
        }
    }


    ////////////////////////////////////////////////////////////////////////////
    //  Apply buffers to release.
    ////////////////////////////////////////////////////////////////////////////
    MY_LOGD("[applyRelease] frameNo:%"PRIxPTR" nodeId:%"PRIxPTR" +", pFrame->getFrameNo(), getNodeId());
    rStreamBufferSet.applyRelease(getNodeId());
    MY_LOGD("[applyRelease] frameNo:%"PRIxPTR" nodeId:%"PRIxPTR" -", pFrame->getFrameNo(), getNodeId());

    ////////////////////////////////////////////////////////////////////////////
    //  Dispatch
    ////////////////////////////////////////////////////////////////////////////
    onDispatchFrame(pFrame);
}


/******************************************************************************
 *
 ******************************************************************************/
// Ask this object's thread to exit. This function is asynchronous, when the
// function returns the thread might still be running. Of course, this
// function can be called from a different thread.
void
HDRNodeImp::
requestExit()
{
    MY_LOGW("Not Implement !!!");
}


/******************************************************************************
 *
 ******************************************************************************/
// Good place to do one-time initializations
status_t
HDRNodeImp::
readyToRun()
{
    ::prctl(PR_SET_NAME, (unsigned long)"Cam@HDRNode", 0, 0, 0);
    //
    //  thread policy & priority
    //  Notes:
    //      Even if pthread_create() with SCHED_OTHER policy, a newly-created thread
    //      may inherit the non-SCHED_OTHER policy & priority of the thread creator.
    //      And thus, we must set the expected policy & priority after a thread creation.
    MY_LOGW("Not Implement !!! Need set thread priority & policy");
    //
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
bool
HDRNodeImp::
threadLoop()
{
    sp<IPipelineFrame> pFrame;
    if  (
            OK == onDequeRequest(pFrame)
        &&  pFrame != 0
        )
    {
        onProcessFrame(pFrame);
        // keep loop
        return true;
    }

    // exit the loop
    return  false;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
HDRNodeImp::
onDequeRequest(
    android::sp<IPipelineFrame>& rpFrame
)
{
    Mutex::Autolock _l(mRequestQueueLock);
    //
    //  Wait until the queue is not empty or this thread will exit.
    while ( mRequestQueue.empty() && ! exitPending() )
    {
        status_t status = mRequestQueueCond.wait(mRequestQueueLock);
        if  ( OK != status ) {
            MY_LOGW(
                "wait status:%d:%s, mRequestQueue.size:%zu, exitPending:%d",
                status, ::strerror(-status), mRequestQueue.size(), exitPending()
            );
        }
    }
    //
    if  ( exitPending() ) {
        MY_LOGW("[exitPending] mRequestQueue.size:%zu", mRequestQueue.size());
        return DEAD_OBJECT;
    }
    //
    //  Here the queue is not empty, take the first request from the queue.
    rpFrame = *mRequestQueue.begin();
    mRequestQueue.erase(mRequestQueue.begin());
    return OK;
}

