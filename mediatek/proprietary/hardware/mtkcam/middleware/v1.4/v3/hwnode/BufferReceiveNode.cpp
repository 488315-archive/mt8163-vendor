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

#define LOG_TAG "MtkCam/BufferReceiveNode"
//
#include <Log.h>
#include <v3/hwnode/BufferReceiveNode.h>
//
#include <utils/RWLock.h>
#include <utils/Thread.h>
//
#include <sys/prctl.h>
#include <sys/resource.h>
//
#include <cutils/properties.h>
//
#include <vector>
//
#include <metadata/IMetadataProvider.h>
#include <metadata/IMetadataConverter.h>
#include <metadata/client/mtk_metadata_tag.h>
#include <mtk_platform_metadata_tag.h>
#include <system/camera_metadata.h>
//
#include <iopipe/PostProc/INormalStream.h>
#include <iopipe/PostProc/IPortEnum.h>
#include <iopipe/SImager/IImageTransform.h>
//
#include <ImageBufferHeap.h>
#include <Trace.h>
#include <v3/hwnode/BufferReceiveNode.h>
#include <v3/utils/streambuf/StreamBuffers.h>
#include <v3/utils/streaminfo/MetaStreamInfo.h>
#include <v3/stream/IStreamInfo.h>
//
#include <ion/ion.h>
#include <ion.h>
#include <sys/mman.h>
#include <linux/mman-proprietary.h>

using namespace android;
using namespace NSCam;
using namespace NSCam::v3;
using namespace NSCam::Utils::Sync;

using namespace NSIoPipe;
using namespace std;

typedef NSCam::v3::Utils::HalMetaStreamBuffer       HalMetaStreamBuffer;
typedef NSCam::v3::Utils::HalImageStreamBuffer      HalImageStreamBuffer;
typedef HalMetaStreamBuffer::Allocator              HalMetaStreamBufferAllocatorT;

/******************************************************************************
 *
 ******************************************************************************/
#define REQUESTTHREAD_NAME        ("Cam@BrRequest")
#define BUFFERTHREAD_NAME         ("Cam@BrBuffer")
#define RECEIVERTHREAD_POLICY     (SCHED_OTHER)
#define RECEIVERTHREAD_PRIORITY   (0)

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
//
#define FUNCTION_IN        MY_LOGD("+")
#define FUNCTION_OUT       MY_LOGD("-")

/******************************************************************************
 *
 ******************************************************************************/
template <typename T>
inline MBOOL
tryGetMetadata(
    IMetadata* pMetadata,
    MUINT32 const tag,
    T & rVal
)
{
    if( pMetadata == NULL ) {
        MY_LOGW("pMetadata == NULL");
        return MFALSE;
    }

    IMetadata::IEntry entry = pMetadata->entryFor(tag);
    if( !entry.isEmpty() ) {
        rVal = entry.itemAt(0, Type2Type<T>());
        return MTRUE;
    }
    return MFALSE;
}


/******************************************************************************
 *
 ******************************************************************************/
template <typename T>
inline MVOID
updateEntry(
    IMetadata* pMetadata,
    MUINT32 const tag,
    T const& val
)
{
    if( pMetadata == NULL ) {
        MY_LOGW("pMetadata == NULL");
        return;
    }

    IMetadata::IEntry entry(tag);
    entry.push_back(val, Type2Type<T>());
    pMetadata->update(tag, entry);
}

/******************************************************************************
 *
 ******************************************************************************/
template <typename T>
inline MVOID
copyTag(
    IMetadata* pSrc,
    IMetadata* pDst,
    MUINT32 const tag
)
{
    if( pSrc == NULL ) {
        MY_LOGW("pSrc == NULL");
        return;
    }
    //
    if( pDst == NULL ) {
        MY_LOGW("pDst == NULL");
        return;
    }

    T val;
    if ( !tryGetMetadata<T>(pSrc, tag, val)) {
        MY_LOGD("cannot get tag");
    } else {
        updateEntry<T>(pDst, tag, val);
    }
}

/*******************************************************************************
 *
 ********************************************************************************/
class BufferReceiveNode::IonBuffer
    : public android::RefBase
{
public:

    ion_user_handle_t handle;

    int32_t ionDevice;

    int32_t shareFd;

    uintptr_t ionVA;

    int32_t buffer_size;
};

/*******************************************************************************
 *
 ********************************************************************************/
class BufferReceiveNodeImp
    : public BnResultCallback,
      public BufferReceiveNode
{
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Implementations.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////                                            Definitions.
    typedef android::sp< IRequest >                         QueNode_T;
    typedef android::List< QueNode_T >                      Que_T;
    typedef android::List< sp< RemoteBuffer > >             BufQue_T;
    typedef android::List< uintptr_t >                      TimeQue_T;


protected:

    class RequestThread : public Thread
    {
    public:

                                    RequestThread(BufferReceiveNodeImp* pNodeImp)
                                        : mpNodeImp(pNodeImp) {}

                                    ~RequestThread() {}
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //  Thread Interface.
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    public:
                    // Ask this object's thread to exit. This function is asynchronous, when the
                    // function returns the thread might still be running. Of course, this
                    // function can be called from a different thread.
                    virtual void        requestExit();

                    // Good place to do one-time initializations
                    virtual status_t    readyToRun();

    private:
                    // Derived class must implement threadLoop(). The thread starts its life
                    // here. There are two ways of using the Thread object:
                    // 1) loop: if threadLoop() returns true, it will be called again if
                    //          requestExit() wasn't called.
                    // 2) once: if threadLoop() returns false, the thread will exit upon return.
                    virtual bool        threadLoop();

    private:
        BufferReceiveNodeImp*           mpNodeImp;
    };

    class CaptureThread : public Thread
    {
    public:

                                    CaptureThread(BufferReceiveNodeImp* pNodeImp)
                                        : mpNodeImp(pNodeImp) {}

                                    ~CaptureThread() {}
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //  Thread Interface.
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    public:
                    // Ask this object's thread to exit. This function is asynchronous, when the
                    // function returns the thread might still be running. Of course, this
                    // function can be called from a different thread.
                    virtual void        requestExit();

                    // Good place to do one-time initializations
                    virtual status_t    readyToRun();

    private:
                    // Derived class must implement threadLoop(). The thread starts its life
                    // here. There are two ways of using the Thread object:
                    // 1) loop: if threadLoop() returns true, it will be called again if
                    //          requestExit() wasn't called.
                    // 2) once: if threadLoop() returns false, the thread will exit upon return.
                    virtual bool        threadLoop();

    private:
        BufferReceiveNodeImp*           mpNodeImp;
    };

    struct  StreamStatus
    {
        enum
        {
            NOT_USED                     = (0x00000001 << 0),
            ERROR                        = (0x00000001 << 1),
            FILLED                       = (0x00000001 << 2),
        };
    };

public:     ////                    Operations.
                                    BufferReceiveNodeImp();

                                    ~BufferReceiveNodeImp();

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  BufferReceiveNode Interface.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////                    Operations.
    virtual MERROR                  init( InitParams const& rParams );

    virtual MERROR                  configure( ConfigParams const& rParams );

    virtual MERROR                  uninit();

    virtual MERROR                  flush();

    virtual MERROR                  submitRequest( sp< IRequest > pRequest );

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  IResultCallback Interface.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////                    Operations.
    virtual MERROR                  onResultReceived ( NSCam::Result* result );

    virtual MVOID                   onLastStrongRef( const void* /*id*/);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  BufferReceiveNodeImp Operations. (wifi)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
protected:  ////                    wifi buffer operations.
    MERROR                          dequeNonStallBuffer(
                                        sp<RemoteBuffer>&  pBuf,
                                        nsecs_t            nsTimeout
                                    );

    MERROR                          dequeStallBuffer(
                                        sp<RemoteBuffer>&  pBuf,
                                        nsecs_t            nsTimeout
                                    );

    MVOID                           releaseBuffer(
                                        sp<RemoteBuffer>&  pRemoteBuffer
                                    );

    MVOID                           returnBuffer(
                                        sp<RemoteBuffer>&  pRemoteBuffer
                                    );

    MVOID                           releasePreviewBuffer(
                                        sp<RemoteBuffer>&  pRemoteBuffer
                                    );

    IImageBuffer*                   createImageBuffer(
                                        sp<RemoteBuffer>&  pBuffer,
                                        Result*            result
                                    );

protected:  ////                    wifi ion buffer operation.
    MERROR                          mapIonBuffer(
                                        sp<RemoteBuffer>&  pBuf,
                                        Result*            result
                                    );

    MERROR                          unmapIonBuffer(
                                        sp<RemoteBuffer>&  pBuf
                                    );

protected:  ////                    wifi request.
    MVOID                           setImageInfo(
                                        ImageInfo&         aInfo,
                                        int                aWidth,
                                        int                aHeight,
                                        int                aFormat
                                    );

protected:  ////                    control external device.
    MVOID                           closeDevice();

    MVOID                           openDevice();

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  BufferReceiveNodeImp Operations. (framework)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
protected:  ////                    framework request.
    MERROR                          dequeRequest(
                                        sp< IRequest >&                rpFrame
                                    );

    MVOID                           pushToWaitingQueue(
                                        sp< IRequest >                 rpFrame
                                    );

    MERROR                          dequeWaitingRequest(
                                        sp< IRequest >&                rpFrame
                                    );

    MERROR                          checkYuvRequest(
                                        sp< IRequest >&                pRequest,
                                        sp< RemoteBuffer >&            pBuf
                                    );

protected:  ////                    framework image buffer operation.
    MERROR                          acquireImageBuffer(
                                        StreamId_T const               streamId,
                                        sp< IImageStreamBuffer > const pStreamBuffer,
                                        sp< IImageBuffer >&            rpImageBuffer,
                                        sp< IImageBufferHeap >&        rpImageBufferHeap
                                    ) const;

    MVOID                           releaseImageBuffer(
                                        sp< IImageStreamBuffer > const pStreamBuffer,
                                        sp< IImageBuffer > const       pImageBuffer,
                                        MUINT32 const                  status
                                    ) const;

protected:  ////                    framework meta buffer operation.
    IMetaStreamBuffer*              createMetaStreamBuffer(
                                        camera_metadata*               pSetting,
                                        sp< IMetaStreamInfo >          pStreamInfo
                                    );

    IMetaStreamBuffer*              createJpegMetaStreamBuffer(
                                        camera_metadata*               pSetting,
                                        sp< IMetaStreamInfo >          pStreamInfo
                                    );

    IMetaStreamBuffer*              createMetaStreamBuffer(
                                        const IMetadata&               pSetting,
                                        sp< IMetaStreamInfo >          pStreamInfo
                                    );

    MERROR                          acquireMetadata(
                                        sp< IMetaStreamBuffer > const  pStreamBuffer,
                                        IMetadata*&                    rpMetadata
                                    ) const;

    MERROR                          releaseMetadata(
                                        sp< IMetaStreamBuffer > const  pStreamBuffer,
                                        IMetadata*&                    rpMetadata
                                    ) const;

    MERROR                          packJpegMetadata(
                                        sp< RemoteBuffer >             pBuf,
                                        sp< IMetaStreamBuffer >&       result
                                    );

protected:  ////                    meta user manager.
    MVOID                           setMetaUserManager(
                                        IMetaStreamBuffer*&            pBuf,
                                        const IMetaStreamInfo*         pInfo
                                    );

    MVOID                           markMetaUserManager(
                                        sp< IMetaStreamBuffer > const  pStreamBuffer,
                                        MUINT32 const                  status
                                    ) const;

protected:  ////                    image user manager.
    MVOID                           setImageUserManager(
                                        IImageStreamBuffer&            pBuf,
                                        const IImageStreamInfo*        pInfo
                                    );

    MVOID                           markImageUserManager(
                                        sp< IImageStreamBuffer > const pStreamBuffer,
                                        MUINT32 const                  status
                                    ) const;

protected:  ////                    callback.
    MVOID                           updateFrame(
                                        MUINT32 const                  requestNo,
                                        bool    const                  lastPart,
                                        Vector< sp< IMetaStreamBuffer > >
                                                                       vOutMeta
                                    );

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  BufferReceiveNodeImp Operations.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
protected:  ////                    basic operation.
    char const*                     getNodeName() const {return "BufferReceiver"; };

    MINTPTR                         getNodeId() const { return 22; };

    sp<IMetadataConverter>          getMetadataConverter();

protected:  ////                    thread.
    MVOID                           waitForRequestDrained() const;

    MVOID                           waitForCaptureDrained() const;

    MERROR                          threadSetting();

protected:  ////                    frame processing.
    MVOID                           onProcessErrorCaptureFrame(
                                        sp< IRequest > const&    pRequest
                                    );

    MVOID                           onProcessErrorFrame(
                                        sp< IRequest > const&    pRequest
                                    );

    MVOID                           onProcessCaptureFrame(
                                        sp< IRequest > const&    pRequest,
                                        sp< RemoteBuffer >       pSrc
                                    );

    MVOID                           onProcessPreviewFrame(
                                        sp< IRequest > const&    pFrame,
                                        sp< RemoteBuffer >       pSrc
                                    );

protected:  ////                    timestamp.
    MVOID                           queTimestamp(
                                        uintptr_t                aTime
                                    );

    MVOID                           dequeTimestamp(
                                        uintptr_t&               aTime
                                    );

    MVOID                           updateTimestamp(
                                        sp< IMetaStreamBuffer >& meta,
                                        uintptr_t                aTime
                                    );

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Data Members.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
protected: ////                     Data Members.
    sp< IMetadataConverter >        mpMetadataConverter;
    mutable Mutex                   mMetadataConverterLock;

protected: ////                     Threads
    sp< RequestThread >             mpRequestThread;
    sp< CaptureThread >             mpCaptureThread;

protected: ////                     Log on / off
    MINT32                          mLogLevel;

protected:  ////                    Config
    mutable Mutex                   mConfigLock;
    sp< IMetaStreamInfo >           mpInAppMeta_Request;
    sp< IMetaStreamInfo >           mpOutMeta_yuv;
    sp< IMetaStreamInfo >           mpOutMeta_jpeg;
    ImageStreamInfoSetT             mpOutYuv;
    sp< IImageStreamInfo >          mpOutJpeg;

protected:  ////                    callback
    wp< NSCam::IAppCallback >       mpAppCallback;

protected:  ////                    ion
    int32_t                         mIonDevice;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Data Members. (framework)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
protected:  ////                    App request Queue
    mutable Mutex                   mRequestQueueLock;
    Condition                       mRequestQueueCond;
    Que_T                           mRequestQueue;
    MINT32                          mRequestQueueDepth;
    MBOOL                           mbRequestDrained;
    mutable Condition               mbRequestDrainedCond;

protected:  ////                    flush & uninit
    MBOOL                           mbRequestExit;
    MBOOL                           mbFlush;
    MBOOL                           mbUninit;

protected:  ////                    Jpeg Waiting queue
    mutable Mutex                   mWaitingQueueLock;
    Condition                       mWaitingQueueCond;
    Que_T                           mWaitingQueue;
    MBOOL                           mbWaitingDrained;
    mutable Condition               mbWaitingDrainedCond;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Data Members. (Wifi)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
protected: ////                     handle wifi delay.
    sp<RemoteBuffer>                mpLatestBuffer;
    mutable Mutex                   mLatestBufferLock;
    MINT32                          mDropImage;

protected: ////                     external device.
    sp<NSCam::IExternalDevice>      mpExternal;
    MBOOL                           mOpened;
    mutable Mutex                   mOpenLock;

protected:  ////                    Wifi non-stall buffer queue.
    mutable Mutex                   mNonStallBufferQueueLock;
    Condition                       mNonStallBufferQueueCond;
    BufQue_T                        mNonStallBufferQueue;

protected:  ////                    Wifi stall buffer queue.
    mutable Mutex                   mStallBufferQueueLock;
    Condition                       mStallBufferQueueCond;
    BufQue_T                        mStallBufferQueue;

protected:  ////                    Wifi result time stamp queue.
    mutable Mutex                   mTimestampQueueLock;
    TimeQue_T                       mTimestampQueue;

};

/*******************************************************************************
 *
 ********************************************************************************/
android::sp<BufferReceiveNode>
BufferReceiveNode::
createInstance()
{
    return new BufferReceiveNodeImp();
}

/******************************************************************************
 *
 ******************************************************************************/
BufferReceiveNodeImp::
BufferReceiveNodeImp()
    : mpExternal(NULL),
      mOpened(false),
      //
      mRequestQueueDepth(3),
      //
      mbFlush(false),
      mbUninit(false),
      mbRequestExit(false),
      //
      mbRequestDrained(true),
      mbWaitingDrained(true),
      //
      mpLatestBuffer(0)
{
    char cLogLevel[PROPERTY_VALUE_MAX];
    ::property_get("debug.camera.log", cLogLevel, "0");
    mLogLevel = atoi(cLogLevel);
    if ( mLogLevel == 0 ) {
        ::property_get("debug.camera.log.BRNode", cLogLevel, "1");
        mLogLevel = atoi(cLogLevel);
    }
}

/******************************************************************************
 *
 ******************************************************************************/
BufferReceiveNodeImp::
~BufferReceiveNodeImp()
{
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
BufferReceiveNodeImp::
init(InitParams const& rParams)
{
    FUNCTION_IN;
    //
    //mOpenId    = rParams.openId;
    //mNodeId    = rParams.nodeId;
    //mNodeName  = rParams.nodeName;
    mpExternal    = rParams.pExternal;
    mpAppCallback = rParams.pAppCallback;
    //
    mpRequestThread = new RequestThread(this);
    if( mpRequestThread->run(REQUESTTHREAD_NAME) != OK ) {
        return UNKNOWN_ERROR;
    }
    //
    mpCaptureThread = new CaptureThread(this);
    if( mpCaptureThread->run(BUFFERTHREAD_NAME) != OK ) {
        return UNKNOWN_ERROR;
    }
    //
    mIonDevice = open("dev/ion", O_RDONLY);
    //
    FUNCTION_OUT;

    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
BufferReceiveNodeImp::
configure(ConfigParams const& rParams)
{
    FUNCTION_IN;
    //
    flush();
    mbFlush = MFALSE;
    //
    {
        Mutex::Autolock _l(mConfigLock);

        // meta
        mpInAppMeta_Request = rParams.pInAppMeta;
        // image
        for (size_t i = 0; i < rParams.pOutImage.size(); i++) {
            if  ( IImageStreamInfo* pStreamInfo = rParams.pOutImage.valueAt(i).get() )
            {
                switch  (pStreamInfo->getImgFormat())
                {
                case eImgFmt_BLOB:
                    mpOutJpeg = pStreamInfo;
                    MY_LOGI("[streamId:%d] jpeg", pStreamInfo->getStreamId());
                    pStreamInfo->setMaxBufNum(1);
                    break;
                    //
                case eImgFmt_YV12:
                case eImgFmt_NV21:
                case eImgFmt_YUY2:
                case eImgFmt_I420:
                    MY_LOGI("[streamId:%d] yuv", pStreamInfo->getStreamId());
                    mpOutYuv.add(rParams.pOutImage.keyAt(i), pStreamInfo);
                    pStreamInfo->setMaxBufNum(8);
                    break;
                    //
                default:
                    MY_LOGE("Unsupported format:0x%x", pStreamInfo->getImgFormat());
                    break;
                }
            }
        }
    }
    //
    {
#warning "[TODO] eSTREAMID_META_APP_DYNAMIC_01"
        // out yuv meta
        sp<IMetaStreamInfo> pStreamInfo =
            new NSCam::v3::Utils::MetaStreamInfo(
                    "App:Meta:YUV",
                    1,//eSTREAMID_META_APP_DYNAMIC_01,
                    eSTREAMTYPE_META_OUT,
                    10, 1
                    );
        mpOutMeta_yuv = pStreamInfo;
    }
    //
    {
#warning "[TODO] eSTREAMID_META_APP_DYNAMIC_02"
        // out jpeg meta
        sp<IMetaStreamInfo> pStreamInfo =
            new NSCam::v3::Utils::MetaStreamInfo(
                    "App:Meta:JPEG",
                    2,//eSTREAMID_META_APP_DYNAMIC_02,
                    eSTREAMTYPE_META_OUT,
                    10, 1
                    );
        mpOutMeta_jpeg = pStreamInfo;
    }
    //
    // open device
    openDevice();
    //
    FUNCTION_OUT;
    //
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
BufferReceiveNodeImp::
uninit()
{
    FUNCTION_IN;
    mbUninit = MTRUE;
    //
    android::wp< IResultCallback > r = 0;
    mpExternal->setCallback(r);
    //
    if ( OK != flush() )
        MY_LOGE("flush failed");
    //
    // exit threads
    mpRequestThread->requestExit();
    mpCaptureThread->requestExit();
    // join
    mpRequestThread->join();
    mpCaptureThread->join();
    //
    mpRequestThread = NULL;
    mpCaptureThread = NULL;
    //
    closeDevice();
    //
    close(mIonDevice);
    //
    FUNCTION_OUT;
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
sp<IMetadataConverter>
BufferReceiveNodeImp::
getMetadataConverter()
{
    Mutex::Autolock _l(mMetadataConverterLock);
    if  ( mpMetadataConverter == 0 ) {
        mpMetadataConverter = IMetadataConverter::createInstance(IDefaultMetadataTagSet::singleton()->getTagSet());
        MY_LOGE_IF(mpMetadataConverter == 0, "IMetadataConverter::createInstance");
    }
    //
    return mpMetadataConverter;
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
BufferReceiveNodeImp::
setImageInfo(ImageInfo& aInfo, int aWidth, int aHeight, int aFormat)
{
    aInfo.width  = aWidth;
    aInfo.height = aHeight;
    aInfo.format = aFormat;
    aInfo.thumbnailWidth = aInfo.thumbnailHeight = 0;
    aInfo.planeCount = 0;
    aInfo.imgStrideInBytes[0] = aInfo.imgStrideInBytes[1] = aInfo.imgStrideInBytes[2] = 0;
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
BufferReceiveNodeImp::
setImageUserManager(
    IImageStreamBuffer& pBuf,
    const IImageStreamInfo*   pInfo
)
{
    //User graph of current buffer
    android::sp<IUsersManager::IUserGraph> userGraph = pBuf.createGraph();
    //
    IUsersManager::User user;
    user.mUserId = getNodeId();
    user.mCategory = IUsersManager::Category::PRODUCER;
    user.mUsage = pInfo->getUsageForConsumer();
    //
    userGraph->addUser(user);
    //
    //Enqueue graph
    pBuf.enqueUserGraph(userGraph);
    pBuf.finishUserSetup();
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
BufferReceiveNodeImp::
setMetaUserManager(
    IMetaStreamBuffer*& pBuf,
    const IMetaStreamInfo*   /*pInfo*/
)
{
    //User graph of current buffer
    android::sp<IUsersManager::IUserGraph> userGraph = pBuf->createGraph();
    //
    IUsersManager::User user;
    user.mUserId = getNodeId();
    user.mCategory = IUsersManager::Category::PRODUCER;
    //
    userGraph->addUser(user);
    //
    //Enqueue graph
    pBuf->enqueUserGraph(userGraph);
    pBuf->finishUserSetup();
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
BufferReceiveNodeImp::
markImageUserManager(
    sp<IImageStreamBuffer> const pStreamBuffer,
    MUINT32 const status
) const
{
    if( pStreamBuffer == NULL ) {
        MY_LOGE("pStreamBuffer == NULL");
        return;
    }
    //
    pStreamBuffer->markStatus(
            (status&StreamStatus::ERROR) ?
            STREAM_BUFFER_STATUS::WRITE_ERROR :
            STREAM_BUFFER_STATUS::WRITE_OK
            );
    //
    //  Mark this buffer as USED by this user.
    //  Mark this buffer as RELEASE by this user.
    pStreamBuffer->markUserStatus(
            getNodeId(),
            ((status != StreamStatus::NOT_USED) ? IUsersManager::UserStatus::USED : 0) |
            IUsersManager::UserStatus::RELEASE
            );
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
BufferReceiveNodeImp::
markMetaUserManager(
    sp<IMetaStreamBuffer> const pStreamBuffer,
    MUINT32 const status
) const
{
    if( pStreamBuffer == NULL ) {
        MY_LOGE("pStreamBuffer == NULL");
        return;
    }
    //
    pStreamBuffer->markStatus(
            (status&StreamStatus::ERROR) ?
            STREAM_BUFFER_STATUS::WRITE_ERROR :
            STREAM_BUFFER_STATUS::WRITE_OK
            );
    //
    //  Mark this buffer as USED by this user.
    //  Mark this buffer as RELEASE by this user.
    pStreamBuffer->markUserStatus(
            getNodeId(),
            ((status != StreamStatus::NOT_USED) ? IUsersManager::UserStatus::USED : 0) |
            IUsersManager::UserStatus::RELEASE
            );
}
/******************************************************************************
 *
 ******************************************************************************/
MERROR
BufferReceiveNodeImp::
submitRequest(android::sp<BufferReceiveNode::IRequest> pRequest)
{
    FUNCTION_IN;

    CAM_TRACE_CALL();
    {
        Mutex::Autolock _l(mRequestQueueLock);
        //
        // 1. mark buffer bit & set buffer's user manager
        for (size_t i = 0; i < pRequest->vOImageBuffers.size(); ++i) {
            MINT bufferType = pRequest->vOImageBuffers[i]->getStreamInfo()->getImgFormat();
            switch (bufferType) {
                case eImgFmt_BLOB:
                    pRequest->bufferTypes.markBit(BufferBit::BLOB);
                    setImageUserManager(*(pRequest->vOImageBuffers[i].get()), pRequest->vOImageBuffers[i]->getStreamInfo());
                    MY_LOGD_IF( 1 , "[%d] Jpeg IImageStreamBuffer:%p", pRequest->requestNo, pRequest->vOImageBuffers[i].get());
                break;
                //
                case eImgFmt_YV12:
                case eImgFmt_NV21:
                case eImgFmt_YUY2:
                case eImgFmt_I420:
                    pRequest->bufferTypes.markBit(BufferBit::YUV);
                    setImageUserManager(*(pRequest->vOImageBuffers[i].get()), pRequest->vOImageBuffers[i]->getStreamInfo());
                    MY_LOGD_IF( 1 , "[%d] yuv IImageStreamBuffer:%p", pRequest->requestNo ,pRequest->vOImageBuffers[i].get());
                break;
                //
                default:
                    MY_LOGE("Unsupported format:0x%x", bufferType);
                break;
            }
        }
        // 2. acquire setting metadata
        IMetadata* pSetting = NULL;
        if( OK != acquireMetadata(
                    pRequest->vIMetaBuffers,
                    pSetting
                    ) ) {
            return -ENODEV;
        }
        // 3. check null setting
        //    if the setting is null -> repeating setting that has been sent to remote device
        MUINT8 capture_intent = 20;
        if( pRequest->setting ) {
            if ( !tryGetMetadata<MUINT8>(pSetting, MTK_CONTROL_CAPTURE_INTENT, capture_intent))
                MY_LOGD("cannot get MTK_CONTROL_CAPTURE_INTENT");
        }
        releaseMetadata( pRequest->vIMetaBuffers, pSetting );
        //

        //block condition
        while (mRequestQueue.size() > (size_t)mRequestQueueDepth) {
            MY_LOGD_IF( 1, "wait: %d > %d", mRequestQueue.size(), (size_t)mRequestQueueDepth);
            status_t status = mRequestQueueCond.wait(mRequestQueueLock);
            MY_LOGD_IF( 1, "wait-");
            if  ( OK != status ) {
                MY_LOGW(
                    "wait status:%d:%s, mRequestQueue.size:%zu",
                    status, ::strerror(-status), mRequestQueue.size()
                );
            }
        }
        //
        if ( mbRequestExit || mbFlush || mbUninit ) {
            onProcessErrorFrame(pRequest);
            return OK;
        }
        //
        // 4. handle request setting that needs to be sent to remote device
        //check capture request --> send to direct renderer
        //Capture request: camera_meta_t + repeating + ImageInfo
        //Capture response: camera_meta_t + YUV or JPEG buffer
#warning "[TODO] should remove blob condition may capture yuv in future"
        // temp condition, cause only capture jpeg now
        // what about capture YUV
        if ((   capture_intent == MTK_CONTROL_CAPTURE_INTENT_VIDEO_SNAPSHOT
             || capture_intent == MTK_CONTROL_CAPTURE_INTENT_STILL_CAPTURE)
             && pRequest->bufferTypes.hasBit(BufferBit::BLOB))
        {
            Request request;
            setImageInfo (request.info, mpOutJpeg->getImgSize().w, mpOutJpeg->getImgSize().h, mpOutJpeg->getImgFormat());
            request.settings = pRequest->setting;
            request.isRepeating = false;
            mpExternal->submitRequest( &request );
        }
        //
        //check preview repeating request --> send repeating to direct renderer
        //Preview request : camera_meta_t + repeating
        //Preview response: camera_meta_t + frame(from playback session)
        else if (   capture_intent == MTK_CONTROL_CAPTURE_INTENT_PREVIEW
                 || capture_intent == MTK_CONTROL_CAPTURE_INTENT_VIDEO_RECORD) {
            RepeatingRequest repeatRequest;
            repeatRequest.settings = pRequest->setting;
            repeatRequest.isRepeating = true;
            mpExternal->submitRepeatingRequest(&repeatRequest);
        }
        //
        mRequestQueue.push_back(pRequest);
        mRequestQueueCond.broadcast();
        MY_LOGD("mRequestQueue.size:%d", mRequestQueue.size());
    }

    FUNCTION_OUT;
    //
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
IImageBuffer*
BufferReceiveNodeImp::
createImageBuffer(
    sp<RemoteBuffer>& pBuffer,
    Result* result
)
{
    FUNCTION_IN;
    //
    size_t bufStridesInBytes[] = { (size_t)result->buffers.imageInfo.imgStrideInBytes[0],
                                   (size_t)result->buffers.imageInfo.imgStrideInBytes[1],
                                   (size_t)result->buffers.imageInfo.imgStrideInBytes[2]
                                 };
    size_t bufBoundaryInBytes[] = {0,0,0};
    IImageBufferAllocator::ImgParam imgParam = IImageBufferAllocator::ImgParam(
                                                    result->buffers.imageInfo.format,
                                                    MSize(result->buffers.imageInfo.width, result->buffers.imageInfo.height),
                                                    bufStridesInBytes,
                                                    bufBoundaryInBytes,
                                                    result->buffers.imageInfo.planeCount
                                               );
    PortBufInfo_v1 portBufInfo = PortBufInfo_v1(
                                    pBuffer->pIon->shareFd,
                                    pBuffer->pIon->ionVA,
                                    0,
                                    0,
                                    0
                                 );
    //
    sp<ImageBufferHeap> pHeap = ImageBufferHeap::create(
                                    LOG_TAG,
                                    imgParam,
                                    portBufInfo
                                );
    if(pHeap == 0)
    {
        MY_LOGE("pHeap is NULL");
        return MFALSE;
    }
    //
    FUNCTION_OUT;

    return pHeap->createImageBuffer();
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
BufferReceiveNodeImp::
releaseBuffer( sp<RemoteBuffer>& pRemoteBuffer )
{
    FUNCTION_IN;
    //
    // no need to return jpeg buffer to wifi, free ion directly
    if(pRemoteBuffer->format == eImgFmt_BLOB) {
        MY_LOGE("3");
        unmapIonBuffer(pRemoteBuffer);
        pRemoteBuffer = NULL;
    } else {
        pRemoteBuffer->pBuf->unlockBuf(LOG_TAG);
        releasePreviewBuffer(pRemoteBuffer);
    }
    FUNCTION_OUT;
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
BufferReceiveNodeImp::
returnBuffer( sp<RemoteBuffer>& pRemoteBuffer )
{
    FUNCTION_IN;
    //
    unmapIonBuffer(pRemoteBuffer);
    //
    Buffer buf;
    buf.ionFd = pRemoteBuffer->pWifiIon->shareFd;
    buf.crossMountVA = pRemoteBuffer->pWifiIon->ionVA;
    buf.bitstream_size = pRemoteBuffer->pWifiIon->buffer_size;
    //
    mpExternal->releaseBuffer(&buf);
    pRemoteBuffer = NULL;
    //
    FUNCTION_OUT;
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
BufferReceiveNodeImp::
releasePreviewBuffer( sp<RemoteBuffer>& pRemoteBuffer )
{
    FUNCTION_IN;
    // do not return mpLatestBuffer if
    //  1.we start to use it as wifi input
    //  2.device is not closed
    if ( mpLatestBuffer == pRemoteBuffer && !mbFlush) {
        MY_LOGW("no wifi buffer, keep latest one fd:%d", mpLatestBuffer->pWifiIon->shareFd);
        FUNCTION_OUT;
        return;
    }
    //
    if (mbFlush || mbUninit) {
        returnBuffer(pRemoteBuffer);
        FUNCTION_OUT;
        return;
    }
    //
    // return latest buffer to wifi and keep new one
    {
        Mutex::Autolock _l(mLatestBufferLock);
        //
        if( mpLatestBuffer != 0) {
            returnBuffer(mpLatestBuffer);
        }
    }
    mpLatestBuffer = pRemoteBuffer;
    pRemoteBuffer = NULL;
    //
    FUNCTION_OUT;
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
BufferReceiveNodeImp::
queTimestamp(uintptr_t aTime)
{
    MY_LOGI("mTimestampQueue %lld", aTime);
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
BufferReceiveNodeImp::
dequeTimestamp(uintptr_t& aTime)
{
    aTime = static_cast<MUINT64>(systemTime(SYSTEM_TIME_PROCESS));
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
BufferReceiveNodeImp::
updateTimestamp(
    sp<IMetaStreamBuffer>& meta,
    uintptr_t aTime
)
{
    IMetadata * rpMetadata;
    acquireMetadata(
        meta,
        rpMetadata
        );
    updateEntry<MINT64>(rpMetadata, MTK_SENSOR_TIMESTAMP, aTime); //fake timestamp
    //
    releaseMetadata(meta, rpMetadata);
}

/******************************************************************************
 * receive buffer from wifi
 ******************************************************************************/
MERROR
BufferReceiveNodeImp::
onResultReceived (
    NSCam::Result* result
)
{
    FUNCTION_IN;
    //
    MY_LOGI("format:0x%x mbRequestExit:%d mbFlush:%d mbUninit:%d", result->buffers.imageInfo.format, mbRequestExit, mbFlush, mbUninit);
    //
    if ( !mbRequestExit && !mbFlush && !mbUninit ) {
        //
        sp<RemoteBuffer> pRemoteBuffer = new RemoteBuffer();
        pRemoteBuffer->format = result->buffers.imageInfo.format;
        pRemoteBuffer->width  = result->buffers.imageInfo.width;
        pRemoteBuffer->height = result->buffers.imageInfo.height;
        //
        //
        if(result->buffers.imageInfo.format == eImgFmt_BLOB) {
            // jpeg buffer --> stall
            // no need to convert jpeg to IImageBuffer
            Mutex::Autolock _l(mStallBufferQueueLock);
            //
            // drop capture buffer cause we return capture fail for last capture request
            if (mDropImage > 0) {
                mDropImage--;
                MY_LOGW("drop capture buffer, mDropImage:%d", mDropImage);
                //
                //FUNCTION_OUT;
                //
                //return -ENODEV;
            }
            //
            if ( mapIonBuffer(pRemoteBuffer, result) != OK ) {
                pRemoteBuffer = NULL;
                MY_LOGE("mapIonBuffer fail");
                //
                FUNCTION_OUT;
                //
                return -ENODEV;
            }
            //
            pRemoteBuffer->pBuf   = NULL;
            //pRemoteBuffer->pMeta  = createMetaStreamBuffer(result->result, mpOutMeta_jpeg);
            // workaround
            pRemoteBuffer->pMeta  = createJpegMetaStreamBuffer(result->result, mpOutMeta_jpeg);
            //
            mStallBufferQueue.push_back(pRemoteBuffer);
            mStallBufferQueueCond.broadcast();
            //
            FUNCTION_OUT;
            //
            return OK;
        } else { // yuv buffer --> non-stall
            Mutex::Autolock _l(mNonStallBufferQueueLock);
            //
            if ( mapIonBuffer(pRemoteBuffer, result) != OK ) {
                pRemoteBuffer = NULL;
                MY_LOGE("mapIonBuffer fail");
                //
                FUNCTION_OUT;
                //
                return -ENODEV;
            }
            //
            pRemoteBuffer->pBuf   = createImageBuffer(pRemoteBuffer, result);
            pRemoteBuffer->pMeta  = createMetaStreamBuffer(result->result, mpOutMeta_yuv);
            //
            mNonStallBufferQueue.push_back(pRemoteBuffer);
            mNonStallBufferQueueCond.broadcast();
            //
            FUNCTION_OUT;
            //
            return OK;
        }
    }
    //
    FUNCTION_OUT;
    //
    return -ENODEV;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
BufferReceiveNodeImp::
dequeRequest( sp<BufferReceiveNode::IRequest>& rpFrame )
{
    FUNCTION_IN;
    //
    Mutex::Autolock _l(mRequestQueueLock);
    //
    //  Wait until the queue is not empty or not going exit
    while ( mRequestQueue.empty() && ! mbRequestExit )
    {
        // set drained flag
        mbRequestDrained = MTRUE;
        mbRequestDrainedCond.signal();
        //
        status_t status = mRequestQueueCond.wait(mRequestQueueLock);
        if  ( OK != status ) {
            MY_LOGW(
                "wait status:%d:%s, mRequestQueue.size:%zu",
                status, ::strerror(-status), mRequestQueue.size()
            );
        }
    }
    //
    if  ( mbRequestExit || mbFlush ) {
        MY_LOGW_IF(!mRequestQueue.empty(), "[flush] mRequestQueue.size:%zu", mRequestQueue.size());
        return DEAD_OBJECT;
    }
    //
    //  Here the queue is not empty, take the first request from the queue.
    mbRequestDrained = MFALSE;
    rpFrame = *mRequestQueue.begin();
    mRequestQueue.erase(mRequestQueue.begin());
    mRequestQueueCond.broadcast();
    //
    dequeTimestamp(rpFrame->timestamp);
    //
    CAM_TRACE_INT("request(BRNode) %d", mRequestQueue.size());
    //
    FUNCTION_OUT;
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
BufferReceiveNodeImp::
dequeWaitingRequest( sp<BufferReceiveNode::IRequest>& rpFrame )
{
    FUNCTION_IN;
    //
    Mutex::Autolock _l(mWaitingQueueLock);
    //
    // Wait until the queue is not empty or not going exit
    while ( mWaitingQueue.empty() && ! mbRequestExit )
    {
        mbWaitingDrained = MTRUE;
        mbWaitingDrainedCond.signal();
        //
        status_t status = mWaitingQueueCond.wait(mWaitingQueueLock);
        if  ( OK != status ) {
            MY_LOGW(
                "wait status:%d:%s, mWaitingQueue.size:%zu",
                status, ::strerror(-status), mWaitingQueue.size()
            );
        }
    }
    //
    if  ( mbRequestExit ) {
        MY_LOGW_IF(!mWaitingQueue.empty(), "[flush] mWaitingQueue.size:%zu", mWaitingQueue.size());
        return DEAD_OBJECT;
    }
    //
    //  Here the queue is not empty, take the first request from the queue.
    mbWaitingDrained = MFALSE;
    rpFrame = *mWaitingQueue.begin();
    mWaitingQueue.erase(mWaitingQueue.begin());
    //
    CAM_TRACE_INT("request(BRNode)", mWaitingQueue.size());
    //
    FUNCTION_OUT;
    return OK;
}

/******************************************************************************
 * workaround
 ******************************************************************************/
IMetaStreamBuffer*
BufferReceiveNodeImp::
createJpegMetaStreamBuffer(
    camera_metadata*    pSetting,
    sp<IMetaStreamInfo> pStreamInfo
)
{
    IMetadata resultMeta;
    //
    //android.jpeg.gpsCoordinates
    IMetadata::IEntry gpsCoordinates(MTK_JPEG_GPS_COORDINATES);
    gpsCoordinates.push_back(0.0, Type2Type< MDOUBLE >());
    resultMeta.update(MTK_JPEG_GPS_COORDINATES, gpsCoordinates);
    //
    //android.jpeg.gpsProcessingMethod
    IMetadata::IEntry gpsProcessingMethod(MTK_JPEG_GPS_PROCESSING_METHOD);
    gpsProcessingMethod.push_back(0, Type2Type< MUINT8 >());
    resultMeta.update(MTK_JPEG_GPS_PROCESSING_METHOD, gpsProcessingMethod);
    //
    //android.jpeg.gpsTimestamp
    IMetadata::IEntry gpsTimestamp(MTK_JPEG_GPS_TIMESTAMP);
    gpsTimestamp.push_back(static_cast<MUINT64>(systemTime(SYSTEM_TIME_PROCESS)), Type2Type< MINT64 >());
    resultMeta.update(MTK_JPEG_GPS_TIMESTAMP, gpsTimestamp);
    //
    //android.jpeg.orientation
    IMetadata::IEntry orientation(MTK_JPEG_ORIENTATION);
    orientation.push_back(90, Type2Type< MINT32 >());
    resultMeta.update(MTK_JPEG_ORIENTATION, orientation);
    //
    //android.jpeg.quality
    IMetadata::IEntry quality(MTK_JPEG_QUALITY);
    quality.push_back(90, Type2Type< MUINT8 >());
    resultMeta.update(MTK_JPEG_QUALITY, quality);
    //
    //android.jpeg.thumbnailquality
    IMetadata::IEntry thumbnailquality(MTK_JPEG_THUMBNAIL_QUALITY);
    thumbnailquality.push_back(100, Type2Type< MUINT8 >());
    resultMeta.update(MTK_JPEG_THUMBNAIL_QUALITY, thumbnailquality);
    //
    //android.jpeg.thumbnailsize
    IMetadata::IEntry thumbnailsize(MTK_JPEG_THUMBNAIL_SIZE);
    thumbnailsize.push_back(MSize(176, 128), Type2Type< MSize >());
    resultMeta.update(MTK_JPEG_THUMBNAIL_SIZE, thumbnailsize);
    //getMetadataConverter()->convert(pSetting, resultMeta);
    //
    if  ( resultMeta.isEmpty() ) {
        MY_LOGW("IMetadataConverter->convert fail");
        return NULL;
    }
    //
    return createMetaStreamBuffer(resultMeta, pStreamInfo);
}


/******************************************************************************
 *
 ******************************************************************************/
IMetaStreamBuffer*
BufferReceiveNodeImp::
createMetaStreamBuffer(
    camera_metadata*    pSetting,
    sp<IMetaStreamInfo> pStreamInfo
)
{
    IMetadata resultMeta;
    getMetadataConverter()->convert(pSetting, resultMeta);
    // que timestamp
    IMetadata::IEntry entry = resultMeta.entryFor(ANDROID_SENSOR_TIMESTAMP);
    // hack : do not que jpeg timestamp
    IMetadata::IEntry jpeg_entry = resultMeta.entryFor(MTK_JPEG_ORIENTATION);
    if( !entry.isEmpty() && jpeg_entry.isEmpty() ) {
        queTimestamp((uintptr_t)entry.itemAt(0, Type2Type<MINT64>()));
    }
    //
    if  ( resultMeta.isEmpty() ) {
        MY_LOGW("IMetadataConverter->convert fail");
        return NULL;
    }
    //
    return createMetaStreamBuffer(resultMeta, pStreamInfo);
}

/******************************************************************************
 *
 ******************************************************************************/
IMetaStreamBuffer*
BufferReceiveNodeImp::
createMetaStreamBuffer(
    const IMetadata&    pSetting,
    sp<IMetaStreamInfo> pStreamInfo
)
{
    // allocate buffer
    IMetaStreamBuffer*
    pStreamBuffer =
    HalMetaStreamBufferAllocatorT(pStreamInfo.get())(pSetting);
    //
    // set user manager
    setMetaUserManager(pStreamBuffer, pStreamInfo.get());
    //
    return pStreamBuffer;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
BufferReceiveNodeImp::
mapIonBuffer(
    sp<RemoteBuffer>& pBuffer,
    Result* result
)
{
    FUNCTION_IN;
#warning "[TODO] error handling"
    pBuffer->pIon = new IonBuffer();
    //
    pBuffer->pIon->buffer_size = 0;
    ImageInfo* info = &result->buffers.imageInfo;
    //
    if( info->format == eImgFmt_YV12 || info->format == eImgFmt_I420) {
        pBuffer->pIon->buffer_size += info->height * info->imgStrideInBytes[0];
        pBuffer->pIon->buffer_size += info->height * info->imgStrideInBytes[1] * 0.5;
        pBuffer->pIon->buffer_size += info->height * info->imgStrideInBytes[2] * 0.5;
    } else
    if( info->format == eImgFmt_NV21 ) {
        pBuffer->pIon->buffer_size += info->height * info->imgStrideInBytes[0];
        pBuffer->pIon->buffer_size += info->height * info->imgStrideInBytes[1] * 0.5;
    } else
    if ( info->format == eImgFmt_BLOB ) {
        pBuffer->pIon->buffer_size = result->buffers.bitstream_size;
    }
    //
    MY_LOGD("wifi fd = %d, addr = 0x%p (w,h)=(%d,%d) planeCount:%d buf size:%d",
            result->buffers.ionFd, result->buffers.crossMountVA,
            info->width, info->height, info->planeCount, pBuffer->pIon->buffer_size);
    //
    if ( pBuffer->pIon->buffer_size == 0) {
        pBuffer->pIon = NULL;
        MY_LOGE("invalid buffer size.");
        return -ENODEV;
    }
    //
    pBuffer->pIon->ionDevice = mIonDevice;
    if (pBuffer->pIon->ionDevice < 0) {
        pBuffer->pIon = NULL;
        MY_LOGE("Cannot open ion device.");
        return -ENODEV;
    }
    //
    if (result->buffers.ionFd < 0) {
        pBuffer->pIon = NULL;
        MY_LOGE("Bad wifi ionFd.");
        return -ENODEV;
    }
    //
    ion_import(pBuffer->pIon->ionDevice, result->buffers.ionFd, &pBuffer->pIon->handle);
    //
    if (ion_share(pBuffer->pIon->ionDevice, pBuffer->pIon->handle, &pBuffer->pIon->shareFd)) {
        pBuffer->pIon = NULL;
        MY_LOGE("IOCTL[ION_IOC_SHARE] failed!");
        return -ENODEV;
    }
    pBuffer->pIon->ionVA = (uintptr_t)ion_mmap(
        pBuffer->pIon->ionDevice, NULL, pBuffer->pIon->buffer_size,
        PROT_READ|PROT_WRITE|PROT_NOCACHE,
        MAP_SHARED, pBuffer->pIon->shareFd, 0
    );
    //
    // keep wifi ion information
    pBuffer->pWifiIon = new IonBuffer();
    pBuffer->pWifiIon->shareFd = result->buffers.ionFd;
    pBuffer->pWifiIon->ionVA = result->buffers.crossMountVA;
    pBuffer->pWifiIon->buffer_size = result->buffers.bitstream_size;
    //
    MY_LOGD("-");
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
BufferReceiveNodeImp::
unmapIonBuffer(
    sp<RemoteBuffer>& pBuffer
)
{
    MY_LOGD("+");
    MY_LOGD("wifi fd = %d, addr = 0x%p",
            pBuffer->pWifiIon->shareFd, pBuffer->pWifiIon->ionVA);
    //
    ion_munmap(pBuffer->pIon->ionDevice, (void*)pBuffer->pIon->ionVA, pBuffer->pIon->buffer_size);
    ion_share_close(pBuffer->pIon->ionDevice, pBuffer->pIon->shareFd);
    ion_free(pBuffer->pIon->ionDevice, pBuffer->pIon->handle);
    //
    MY_LOGD("-");
    //
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
BufferReceiveNodeImp::
dequeNonStallBuffer(
    sp<RemoteBuffer>&         pBuf,
    nsecs_t                   nsTimeout
)
{
    Mutex::Autolock _l(mNonStallBufferQueueLock);
    //
    //  Wait until the queue is not empty or not going exit
    while ( mNonStallBufferQueue.empty() && ! mbRequestExit && !mbFlush)
    {
        //
        status_t status = mNonStallBufferQueueCond.waitRelative(mNonStallBufferQueueLock, nsTimeout);
        if  ( OK != status && ! mbRequestExit && !mbFlush ) {
            MY_LOGW_IF( 
                mLogLevel >= 1,
                "wait status:%d:%s, mNonStallBufferQueue.size:%zu",
                status, ::strerror(-status), mNonStallBufferQueue.size()
            );
            //
            pBuf = mpLatestBuffer;
            if ( pBuf != 0 ) {
                if ( pBuf->pBuf->lockBuf(
                    LOG_TAG,
                    eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN
                    )
                ) {
                    MY_LOGW("use latest fd = %d, addr = 0x%p", pBuf->pWifiIon->shareFd, pBuf->pWifiIon->ionVA);
                    return OK;
                } else {
                    MY_LOGE("use latest fail to lock buffer fd = %d, addr = 0x%p", pBuf->pWifiIon->shareFd, pBuf->pWifiIon->ionVA);
                    returnBuffer( mpLatestBuffer );
                    pBuf = NULL;
                    return DEAD_OBJECT;
                }
            } else {
                return DEAD_OBJECT;
            }
        }
    }
    //
    if  ( mbRequestExit || mbFlush ) {
        MY_LOGW_IF(!mNonStallBufferQueue.empty(), "[flush] mNonStallBufferQueue.size:%zu", mNonStallBufferQueue.size());
        return DEAD_OBJECT;
    }
    //
    //  Here the queue is not empty, take the first request from the queue.
    pBuf = *mNonStallBufferQueue.begin();
    mNonStallBufferQueue.erase(mNonStallBufferQueue.begin());
    //
    if ( pBuf != 0 ) {
        if ( pBuf->pBuf->lockBuf(
            LOG_TAG,
            eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN
            )
        ) {
            return OK;
        } else {
            MY_LOGE("fail to lock buffer fd = %d, addr = 0x%p", pBuf->pWifiIon->shareFd, pBuf->pWifiIon->ionVA);
            returnBuffer( pBuf );
            return DEAD_OBJECT;
        }
    } else {
        return DEAD_OBJECT;
    }
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
BufferReceiveNodeImp::
pushToWaitingQueue(
    sp< IRequest > pRequest
)
{
    MY_LOGI("request:%d", pRequest->requestNo);
    //
    mWaitingQueue.push_back(pRequest);
    mWaitingQueueCond.broadcast();
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
BufferReceiveNodeImp::
updateFrame(
    MUINT32 const requestNo,
    bool    const lastPart,
    Vector< sp< IMetaStreamBuffer > > vOutMeta
)
{
    FUNCTION_IN;
    //
    sp< NSCam::IAppCallback > cb = mpAppCallback.promote();
    if ( cb == 0 ) {
        MY_LOGW("mpAppCallback promote fail");
        return;
    }
    MY_LOGI("requestNo:%d user:%d lastPart:%d", requestNo, getNodeId(), lastPart);
    cb->updateFrame(requestNo, getNodeId(), lastPart, vOutMeta);
    //
    FUNCTION_OUT;
}

/******************************************************************************
 * check if request really needs yuv buffer. handle only jpeg capture case
 ******************************************************************************/
MERROR
BufferReceiveNodeImp::
checkYuvRequest(
    sp< IRequest >&     pRequest,
    sp< RemoteBuffer >& pBuf
)
{
    while( !pRequest->bufferTypes.hasBit(BufferBit::YUV) ) {
#warning "fake pass1 & pass2 metadata & shutter callback for only jpeg request"
#warning "may return jpeg Tag twice."
        markMetaUserManager(pBuf->pMeta, StreamStatus::FILLED);
        android::Vector<android::sp<IMetaStreamBuffer> > vOutMeta;
        updateTimestamp(pBuf->pMeta, pRequest->timestamp);
        vOutMeta.push_back(pBuf->pMeta);
        //
        updateFrame(pRequest->requestNo, false, vOutMeta);
        //
        releaseBuffer(pBuf);
        pBuf = 0;
        //
        if ( OK != dequeNonStallBuffer(pBuf, ::ms2ns(60)) && pBuf == 0) {
            // capture request with one jpeg buffer
            onProcessErrorCaptureFrame(pRequest);
            mDropImage++;
            return -ENODEV;
        }
        //
        pushToWaitingQueue(pRequest);
        dequeRequest(pRequest);
    }
    //
    return OK;
}

/******************************************************************************
 * handle yuv buffer
 *     1. deque request & yuv buffer
 *     2. process yuv buffer
 *     3. unlock wifi buffer & return to direct renderer
 ******************************************************************************/
bool
BufferReceiveNodeImp::RequestThread::
threadLoop()
{
    sp<IRequest>     pRequest;
    sp<RemoteBuffer> pBuf;
    //
    if (   !exitPending()
        && !mpNodeImp->mbRequestExit
       )
    {
        if ( OK != mpNodeImp->dequeNonStallBuffer(pBuf, ::ms2ns(60))) {
            MY_LOGW_IF( 0 , "dequeNonStallBuffer fail");
            return true;
        }
        //
        if ( OK != mpNodeImp->dequeRequest(pRequest) || pRequest == 0) {
            MY_LOGW("dequeRequest fail");
            if ( pBuf != 0 ) {
                mpNodeImp->releaseBuffer(pBuf);
            }
            return false;
        }
        //
        if ( OK != mpNodeImp->checkYuvRequest(pRequest, pBuf) ) {
            MY_LOGW("checkYuvRequest error");
            return true;
        }
        //
        mpNodeImp->onProcessPreviewFrame(pRequest, pBuf);
        //
        mpNodeImp->releaseBuffer(pBuf);
        //
        return true;
    }
    //
    MY_LOGD("exit request thread");
    //
    return  false;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
BufferReceiveNodeImp::
dequeStallBuffer(
    sp< RemoteBuffer >& pBuf,
    nsecs_t             nsTimeout
)
{
    FUNCTION_IN;
    //
    Mutex::Autolock _l(mStallBufferQueueLock);
    //
    //  Wait until the queue is not empty or not going exit
    while ( mStallBufferQueue.empty() && ! mbRequestExit && ! mbFlush )
    {
        status_t status = mStallBufferQueueCond.waitRelative(mStallBufferQueueLock, nsTimeout);
        if  ( OK != status ) {
            MY_LOGW(
                "wait status:%d:%s, mStallBufferQueue.size:%zu",
                status, ::strerror(-status), mStallBufferQueue.size()
            );
            mDropImage++;
            return TIMED_OUT;
        }
    }
    //
    if  ( mbRequestExit || mbFlush ) {
        MY_LOGW_IF(!mStallBufferQueue.empty(), "[flush] mStallBufferQueue.size:%zu", mStallBufferQueue.size());
        return DEAD_OBJECT;
    }
    //
    MY_LOGD("mStallBufferQueue size:%d", mStallBufferQueue.size());
    pBuf = *mStallBufferQueue.begin();
    mStallBufferQueue.erase(mStallBufferQueue.begin());
    //
    //
    CAM_TRACE_INT("deque stall wifi buffer(BRNode)", mStallBufferQueue.size());
    //
    FUNCTION_OUT;
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
BufferReceiveNodeImp::
threadSetting()
{
    //  thread policy & priority
    //  Notes:
    //      Even if pthread_create() with SCHED_OTHER policy, a newly-created thread
    //      may inherit the non-SCHED_OTHER policy & priority of the thread creator.
    //      And thus, we must set the expected policy & priority after a thread creation.
    MINT tid;
    struct sched_param sched_p;
    ::sched_getparam(0, &sched_p);
    if (RECEIVERTHREAD_POLICY == SCHED_OTHER) {
        sched_p.sched_priority = 0;
        ::sched_setscheduler(0, RECEIVERTHREAD_POLICY, &sched_p);
        ::setpriority(PRIO_PROCESS, 0, RECEIVERTHREAD_PRIORITY);   //  Note: "priority" is nice value.
    } else {
        sched_p.sched_priority = RECEIVERTHREAD_PRIORITY;          //  Note: "priority" is real-time priority.
        ::sched_setscheduler(0, RECEIVERTHREAD_POLICY, &sched_p);
    }

    MY_LOGD("tid(%d) policy(%d) priority(%d)", ::gettid(), RECEIVERTHREAD_POLICY, RECEIVERTHREAD_PRIORITY);

    return OK;

}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
BufferReceiveNodeImp::
packJpegMetadata(
    sp< RemoteBuffer >       pBuf,
    sp< IMetaStreamBuffer >& result
)
{
    IMetadata  destMeta;
    IMetadata* sourceMeta;
    acquireMetadata(pBuf->pMeta, sourceMeta);
    //
    //android.jpeg.gpsCoordinates
    copyTag<MDOUBLE>(sourceMeta, &destMeta, MTK_JPEG_GPS_COORDINATES);
    //
    //android.jpeg.gpsProcessingMethod
    copyTag<MUINT8>(sourceMeta, &destMeta, MTK_JPEG_GPS_PROCESSING_METHOD);
    //
    //android.jpeg.gpsTimestamp
    copyTag<MINT64>(sourceMeta, &destMeta, MTK_JPEG_GPS_TIMESTAMP);
    //
    //android.jpeg.orientation
    copyTag<MINT32>(sourceMeta, &destMeta, MTK_JPEG_ORIENTATION);
    //
    //android.jpeg.orientation
    copyTag<MUINT8>(sourceMeta, &destMeta, MTK_JPEG_QUALITY);
    //
    //android.jpeg.orientation
    copyTag<MUINT8>(sourceMeta, &destMeta, MTK_JPEG_THUMBNAIL_QUALITY);
    //
    //android.jpeg.orientation
    copyTag<MSize>(sourceMeta, &destMeta, MTK_JPEG_THUMBNAIL_SIZE);

    releaseMetadata(pBuf->pMeta, sourceMeta);

    result = createMetaStreamBuffer(destMeta, mpOutMeta_jpeg);

    return OK;
}

/******************************************************************************
 * handle capture buffer
 *     1. jpeg timeout
 *     2. jpeg on time
 ******************************************************************************/
bool
BufferReceiveNodeImp::CaptureThread::
threadLoop()
{
    sp<IRequest>           pRequest;
    sp<RemoteBuffer>       pBuf;
    //
    if ( ! exitPending()
        && OK == mpNodeImp->dequeWaitingRequest(pRequest)
        && pRequest != 0)
    {
        MERROR status = mpNodeImp->dequeStallBuffer(pBuf, ::ms2ns(5000)); // 5s
        //
        // 1. jpeg timeout, return capture fail & ignore 1 wifi jpeg buffer next time
        if (TIMED_OUT == status)
        {
            mpNodeImp->onProcessErrorCaptureFrame(pRequest);
            // no need to return buffer to direct renderer
        } else 
        if ( OK == status ) {
            //
            // 2. jpeg on time
            if (pBuf != 0)
            {
                mpNodeImp->onProcessCaptureFrame(pRequest, pBuf);
                //
                // unlock wifi buffer & no need to return to direct renderer
                mpNodeImp->releaseBuffer(pBuf);
            }
        }
        return true;
    }
    //
    MY_LOGD("exit capture thread");
    return  false;

}
/******************************************************************************
 *
 ******************************************************************************/
void
BufferReceiveNodeImp::RequestThread::
requestExit()
{
    FUNCTION_IN;
    //
    Mutex::Autolock _l(mpNodeImp->mRequestQueueLock);
    mpNodeImp->mbRequestExit = MTRUE;
    mpNodeImp->mRequestQueueCond.broadcast();
    //
    FUNCTION_OUT;
}

/******************************************************************************
 *
 ******************************************************************************/
void
BufferReceiveNodeImp::CaptureThread::
requestExit()
{
    FUNCTION_IN;
    //
    Mutex::Autolock _l(mpNodeImp->mWaitingQueueLock);
    mpNodeImp->mbRequestExit = MTRUE;
    mpNodeImp->mWaitingQueueCond.broadcast();
    //
    FUNCTION_OUT;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
BufferReceiveNodeImp::RequestThread::
readyToRun()
{
    return mpNodeImp->threadSetting();
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
BufferReceiveNodeImp::CaptureThread::
readyToRun()
{
    return mpNodeImp->threadSetting();
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
BufferReceiveNodeImp::
acquireMetadata(
    sp<IMetaStreamBuffer> const pStreamBuffer,
    IMetadata*&                 rpMetadata
) const
{
    rpMetadata = pStreamBuffer->tryReadLock(getNodeName());

    if( rpMetadata == NULL ) {
        MY_LOGE("[stream buffer:%s] cannot get metadata",
                pStreamBuffer->getName());
        return BAD_VALUE;
    }

    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
BufferReceiveNodeImp::
releaseMetadata(
    sp<IMetaStreamBuffer> const pStreamBuffer,
    IMetadata*&                 rpMetadata
) const
{
    pStreamBuffer->unlock(getNodeName(), rpMetadata);

    return OK;
}

/******************************************************************************
 * IImageStreamBuffer --> IImageBufferHeap --> IImageBuffer
 ******************************************************************************/
MERROR
BufferReceiveNodeImp::
acquireImageBuffer(
    StreamId_T const               streamId,
    sp< IImageStreamBuffer > const pStreamBuffer,
    sp< IImageBuffer >&            rpImageBuffer,
    sp< IImageBufferHeap >&        rpImageBufferHeap
) const
{
    if( pStreamBuffer == NULL ) {
        MY_LOGE("pStreamBuffer == NULL");
        return BAD_VALUE;
    }
    //  Query the group usage.
#warning "[TODO] hard code usage"
    MUINT const groupUsage = eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN | eBUFFER_USAGE_SW_WRITE_OFTEN;//pStreamBuffer->queryGroupUsage(getNodeId());
    //
    rpImageBufferHeap = pStreamBuffer->tryWriteLock(getNodeName());

    if (rpImageBufferHeap == NULL) {
        MY_LOGE("[node:%d][stream buffer:%s] cannot get ImageBufferHeap",
                getNodeId(), pStreamBuffer->getName());
        return BAD_VALUE;
    }

    rpImageBuffer = rpImageBufferHeap->createImageBuffer();
    if (rpImageBuffer == NULL) {
        MY_LOGE("[node:%d][stream buffer:%s] cannot create ImageBuffer",
                getNodeId(), pStreamBuffer->getName());
        return BAD_VALUE;
    }
    rpImageBuffer->lockBuf(getNodeName(), groupUsage);

    MY_LOGD_IF( 1, "stream %#"PRIxPTR": buffer: %p, usage: %p",
        streamId, rpImageBuffer.get(), groupUsage);

    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
BufferReceiveNodeImp::
releaseImageBuffer(
    sp< IImageStreamBuffer > const pStreamBuffer,
    sp< IImageBuffer > const       pImageBuffer,
    MUINT32 const                  status
) const
{
    if( pStreamBuffer == NULL || pImageBuffer == NULL ) {
        MY_LOGE("pStreamBuffer %p, pImageBuffer %p should not be NULL");
        return;
    }
    //
    markImageUserManager(pStreamBuffer, status);
    //
    pImageBuffer->unlockBuf(getNodeName());
    pStreamBuffer->unlock(getNodeName(), pImageBuffer->getImageBufferHeap());
}

/******************************************************************************
 * process YUV buffer and mark status
 ******************************************************************************/
MVOID
BufferReceiveNodeImp::
onProcessPreviewFrame(
    android::sp< IRequest > const& pRequest,
    android::sp< RemoteBuffer >    pSrc
)
{
    FUNCTION_IN;
    MY_LOGI("request:%d fd = %d, addr = 0x%p", pRequest->requestNo, pSrc->pWifiIon->shareFd, pSrc->pWifiIon->ionVA);
    //
    vector< sp<IImageBuffer> > vDst;
    //
    // output
    // acquire buffer & lock
    for(size_t i = 0; i < mpOutYuv.size(); ++i) {
        StreamId_T id = mpOutYuv.valueAt(i)->getStreamId();
        size_t index = pRequest->vOImageBuffers.indexOfKey(id);
        MY_LOGI("[%d] StreamId_T:%d index:%d", i, id, index);
        if ((ssize_t)index == NAME_NOT_FOUND) continue;
        //
        sp<IImageBuffer> buf;
        sp<IImageBufferHeap> bufHeap;
        acquireImageBuffer(
                    id,
                    pRequest->vOImageBuffers.valueAt(index),
                    buf,
                    bufHeap
                    );
        if(buf == 0) {
            MY_LOGE("buf == NULL");
        }
        vDst.push_back(buf);
    }
    //
    if( pSrc->pBuf == NULL || vDst.size() == 0 ) {
        MY_LOGE("wrong mdp in/out: src %p, dst count %d", pSrc->pBuf.get(), vDst.size());
        return;
    }
    //
    MBOOL success = MFALSE;
    {
        MRect   const roi       = MRect(pSrc->pBuf->getImgSize().w, pSrc->pBuf->getImgSize().h);
        MUINT32 const transform = 0; //TODO
        //
        NSSImager::IImageTransform* pTrans = NSSImager::IImageTransform::createInstance();
        if( !pTrans ) {
            MY_LOGE("!pTrans");
            return;
        }
        //
        IImageBuffer* src = pSrc->pBuf.get();
        success =
            pTrans->execute(
                src,
                vDst[0].get(),
                (vDst.size() > 1 )? vDst[1].get() : NULL,
                roi,
                transform,
                0xFFFFFFFF
                );
        //
        if( !success ) {
            MY_LOGW("pTrans->execute not success");
        }
        pTrans->destroyInstance();
        pTrans = NULL;
    }
    //
    for(size_t i = 0, j = 0; i < mpOutYuv.size(); ++i) {
        StreamId_T id = mpOutYuv.valueAt(i)->getStreamId();
        size_t index = pRequest->vOImageBuffers.indexOfKey(id);
        if ((ssize_t)index == NAME_NOT_FOUND) continue;
        //
        android::sp<IImageBuffer> buf = vDst[j++];
        releaseImageBuffer(pRequest->vOImageBuffers.valueAt(index), buf, success ? StreamStatus::FILLED
                                                                                 : StreamStatus::ERROR);
    }
    //
    // process meta
#warning "may return jpeg Tag twice."
    markMetaUserManager(pSrc->pMeta, success ? StreamStatus::FILLED
                                             : StreamStatus::ERROR);
    android::Vector<android::sp<IMetaStreamBuffer> > vOutMeta;
    updateTimestamp(pSrc->pMeta, pRequest->timestamp);
    vOutMeta.push_back(pSrc->pMeta);
    //
    // if request also need jpeg buffer, queue in waiting list
    if (pRequest->bufferTypes.hasBit(BufferBit::BLOB)) {
        updateFrame(pRequest->requestNo, false, vOutMeta);
        pushToWaitingQueue(pRequest);
    }
    // request only need YUV buffer
    else {
        updateFrame(pRequest->requestNo, true, vOutMeta);
    }

    FUNCTION_OUT;
}

/******************************************************************************
 * process JPEG buffer and mark status
 *     1. memory copy jpeg buffer
 *     2. fake partial result.
 *     3. mark image status
 *     4. callback
 ******************************************************************************/
MVOID
BufferReceiveNodeImp::
onProcessCaptureFrame(
    android::sp< IRequest > const& pRequest,
    android::sp< RemoteBuffer >    pBuf
)
{
    FUNCTION_IN;
    // 1. memory copy jpeg buffer
    // acquire framework jpeg buffer
    sp<IImageBuffer> pJpegBuf;
    sp<IImageBufferHeap> bufHeap;
    StreamId_T id = mpOutJpeg->getStreamId();
    size_t index = pRequest->vOImageBuffers.indexOfKey(id);
    if ((ssize_t)index != NAME_NOT_FOUND) {
        MERROR status = acquireImageBuffer(
                            id,
                            pRequest->vOImageBuffers[index],
                            pJpegBuf,
                            bufHeap
                        );
        if (status != OK) {
            onProcessErrorCaptureFrame(pRequest);
            releaseBuffer(pBuf);
            return;
        }
    } else {
        MY_LOGW("cannot find jpeg stream %d", id);
        return;
    }
    // check requested size
#warning "check requested size"
    if (   pBuf->width  != pJpegBuf->getImgSize().w
        || pBuf->height != pJpegBuf->getImgSize().h) {
            MY_LOGW("jpeg requested size (%d,%d) actual (%d,%d)"
                , pJpegBuf->getImgSize().w, pJpegBuf->getImgSize().h
                , pBuf->width
                , pBuf->height);
            // return capture error if size not correct
            //onProcessErrorCaptureFrame(pRequest);
            //releaseBuffer(pBuf);
            //return;
    }
    //
    uintptr_t fwVa = pJpegBuf->getBufVA(0);
    memcpy((void*)fwVa, (void*)pBuf->pIon->ionVA, pBuf->pWifiIon->buffer_size);
    bufHeap->setBitstreamSize(pBuf->pWifiIon->buffer_size);
    //
    // release framework buffer
    if ((ssize_t)index != NAME_NOT_FOUND) {
        releaseImageBuffer(pRequest->vOImageBuffers.valueAt(index), pJpegBuf, StreamStatus::FILLED);
    }
    //
    android::Vector<android::sp<IMetaStreamBuffer> > vOutMeta;
    //
    // 2. fake partial result
    android::sp<IMetaStreamBuffer> result;
    if (pRequest->bufferTypes.hasBit(BufferBit::YUV)) {
        packJpegMetadata(pBuf, result); // only jpeg related metadata.
    } else {
        updateTimestamp(pBuf->pMeta, pRequest->timestamp);
        result = pBuf->pMeta;
    }
    markMetaUserManager(result, StreamStatus::FILLED);
    vOutMeta.push_back(result);

    // 3. mark image status
    markImageUserManager(
        pRequest->vOImageBuffers[index],
        StreamStatus::FILLED
        );

    // 4. callback
    updateFrame(pRequest->requestNo, true, vOutMeta);
    //
    FUNCTION_OUT;
}

/******************************************************************************
 * process error capture buffer and mark status
 *     1. create error meta
 *     2. mark image error
 *     3. callback
 ******************************************************************************/
MVOID
BufferReceiveNodeImp::
onProcessErrorCaptureFrame(
    sp< IRequest > const& pRequest
)
{
    FUNCTION_IN;
    // 1. create error meta
    IMetadata m;
    sp<IMetaStreamBuffer> meta_stream = createMetaStreamBuffer(m, mpOutMeta_jpeg);
    markMetaUserManager(meta_stream, StreamStatus::ERROR);
    android::Vector<android::sp<IMetaStreamBuffer> > vOutMeta;
    vOutMeta.push_back(meta_stream);
    //
    // 2. mark image error
    StreamId_T id = mpOutJpeg->getStreamId();
    size_t index = pRequest->vOImageBuffers.indexOfKey(id);
    if ((ssize_t)index != NAME_NOT_FOUND) {
        markImageUserManager(
            pRequest->vOImageBuffers.valueAt(index),
            StreamStatus::ERROR|StreamStatus::NOT_USED
        );
    } else {
        MY_LOGW("cannot find jpeg stream %d", id);
    }
    // 3. callback
    updateFrame(pRequest->requestNo, true, vOutMeta);
    //
    FUNCTION_OUT;
}


/******************************************************************************
 * process error preview buffer and mark status
 *     1. create error meta
 *     2. mark image error
 *     3. callback
 ******************************************************************************/
MVOID
BufferReceiveNodeImp::
onProcessErrorFrame(
    sp< IRequest > const& pRequest
)
{
    // 1. create error meta
    IMetadata m;
    sp<IMetaStreamBuffer> meta_stream = createMetaStreamBuffer(m, mpOutMeta_yuv);
    markMetaUserManager(meta_stream, StreamStatus::ERROR);
    android::Vector<android::sp<IMetaStreamBuffer> > vOutMeta;
    vOutMeta.push_back(meta_stream);
    //
    // 2. mark image error
    for(size_t i = 0; i < mpOutYuv.size(); ++i) {
        StreamId_T id = mpOutYuv.valueAt(i)->getStreamId();
        size_t index = pRequest->vOImageBuffers.indexOfKey(id);
        if ((ssize_t)index == NAME_NOT_FOUND) continue;
        //
        markImageUserManager(
            pRequest->vOImageBuffers.valueAt(index),
            StreamStatus::ERROR|StreamStatus::NOT_USED
        );
    }
    //
    // 3. callback
    if (pRequest->bufferTypes.hasBit(BufferBit::BLOB)) {
        updateFrame(pRequest->requestNo, false, vOutMeta);
        onProcessErrorCaptureFrame(pRequest);
    }
    else {
        updateFrame(pRequest->requestNo, true, vOutMeta);
    }
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
BufferReceiveNodeImp::
waitForRequestDrained() const
{
    FUNCTION_IN;
    //
    Mutex::Autolock _l(mRequestQueueLock);
    if( !mbRequestDrained ) {
        MY_LOGD("wait for request drained");
        mbRequestDrainedCond.wait(mRequestQueueLock);
    }
    //
    FUNCTION_OUT;
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
BufferReceiveNodeImp::
waitForCaptureDrained() const
{
    FUNCTION_IN;
    //
    Mutex::Autolock _l(mWaitingQueueLock);
    if( !mbWaitingDrained ) {
        MY_LOGD("wait for request drained");
        mbWaitingDrainedCond.wait(mWaitingQueueLock);
    }
    //
    FUNCTION_OUT;
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
BufferReceiveNodeImp::
closeDevice()
{
    Mutex::Autolock _l(mOpenLock);
    if ( mOpened ) {
        mOpened = MFALSE;
        mpExternal->closeDevice();
        mpExternal = NULL;
    }

}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
BufferReceiveNodeImp::
openDevice()
{
    Mutex::Autolock _l(mOpenLock);
    if ( !mOpened ) {
        // temp
        android::wp< IResultCallback > r = this;
        mpExternal->setCallback(r);
        //
        mOpened = MTRUE;
        mpExternal->openDevice();
    }
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
BufferReceiveNodeImp::
flush()
{
    FUNCTION_IN;
    //
    mbFlush = MTRUE;
    //
    // 0. wake up queue thread.
    {
        Mutex::Autolock _l(mRequestQueueLock);
        mRequestQueueCond.broadcast();
    }
    // 1. clear requests
    {
        Mutex::Autolock _l(mRequestQueueLock);
        //
        MY_LOGD("mRequestQueue.size:%d", mRequestQueue.size());
        Que_T::iterator it = mRequestQueue.begin();
        while ( it != mRequestQueue.end() ) {
            onProcessErrorFrame(*it);
            it = mRequestQueue.erase(it);
        }
        mRequestQueueCond.broadcast();
        //
        if ( mRequestQueue.empty() ) {
            mbRequestDrained = MTRUE;
            mbRequestDrainedCond.signal();
        }
    }
    // make sure request empty
    if ( !mRequestQueue.empty() ) {
        Mutex::Autolock _l(mRequestQueueLock);
        //
        MY_LOGD("mRequestQueue.size:%d", mRequestQueue.size());
        Que_T::iterator it = mRequestQueue.begin();
        while ( it != mRequestQueue.end() ) {
            onProcessErrorFrame(*it);
            it = mRequestQueue.erase(it);
        }
        mRequestQueueCond.broadcast();
        //
        mbRequestDrained = MTRUE;
        mbRequestDrainedCond.signal();
    }
    // 2. clear capture request
    {
        Mutex::Autolock _l(mWaitingQueueLock);
        //
        MY_LOGD("mWaitingQueue.size:%d", mWaitingQueue.size());
        Que_T::iterator it = mWaitingQueue.begin();
        while ( it != mWaitingQueue.end() ) {
            onProcessErrorCaptureFrame(*it);
            it = mWaitingQueue.erase(it);
        }
        //
        mWaitingQueueCond.broadcast();
        //
        mbWaitingDrained = MTRUE;
        mbWaitingDrainedCond.signal();
    }
    // 3. release wifi buffer
    {
        Mutex::Autolock _l(mNonStallBufferQueueLock);
        //
        MY_LOGD("mNonStallBufferQueue.size:%d", mNonStallBufferQueue.size());
        BufQue_T::iterator it = mNonStallBufferQueue.begin();
        while ( it != mNonStallBufferQueue.end() ) {
            returnBuffer(*it);
            it = mNonStallBufferQueue.erase(it);
        }
    }
    {
        Mutex::Autolock _l(mLatestBufferLock);
        //
        if (mpLatestBuffer != 0) {
            MY_LOGD("mpLatestBuffer != 0");
            returnBuffer(mpLatestBuffer);
        }
    }
    {
        Mutex::Autolock _l(mStallBufferQueueLock);
        //
        MY_LOGD("mStallBufferQueue.size:%d", mStallBufferQueue.size());
        BufQue_T::iterator it = mStallBufferQueue.begin();
        while ( it != mStallBufferQueue.end() ) {
            // no need to return jpeg buffer to wifi
            unmapIonBuffer(*it);
            it = mStallBufferQueue.erase(it);
        }
    }
    //
    // 4. wait request thread
    waitForRequestDrained();
    //
    // 5. wait capture thread
    waitForCaptureDrained();
    //
    FUNCTION_OUT;

    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
BufferReceiveNodeImp::
onLastStrongRef(const void* /*id*/)
{
    //uninit();
}
