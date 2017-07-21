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

#define LOG_TAG "MtkCam/P2FeatureNode"
//
#include <Log.h>
#include "BaseNode.h"
#include "hwnode_utilities.h"
#include <v3/hwnode/P2FeatureNode.h>
//
#include <utils/RWLock.h>
#include <utils/Thread.h>
//
#include <sys/prctl.h>
#include <sys/resource.h>
//
#include <IHal3A.h>
#include <iopipe/PostProc/INormalStream.h>
#include <iopipe/PostProc/IPortEnum.h>
#include <iopipe/SImager/IImageTransform.h>
#include <drv/isp_reg.h>
//
#include <vector>
#include <list>
//
#include <metadata/IMetadataProvider.h>
#include <metadata/client/mtk_metadata_tag.h>
#include <mtk_platform_metadata_tag.h>
//
#if 0/*[EP_TEMP]*/
#include <featureio/eis_hal.h>
#endif
//
#include <Trace.h>
//
#include <cutils/properties.h>
//

#ifdef FEATURE_MODIFY
#include <common/eis/eis_ext.h>
#include <common/vhdr/1.0/vhdr_type.h>
#include <featurePipe/IStreamingFeaturePipe.h>
using NSCam::NSCamFeature::NSFeaturePipe::IStreamingFeaturePipe;
using NSCam::NSCamFeature::NSFeaturePipe::FeaturePipeParam;
using NSCam::NSCamFeature::VarMap;
using namespace NSCam::NSCamFeature::NSFeaturePipe;
#define VAR_P2_REQUEST "p2request"
#endif // FEATURE_MODIFY

using namespace android;
using namespace NSCam;
using namespace NSCam::v3;
using namespace NSCam::Utils::Sync;

using namespace std;
using namespace NSIoPipe;
using namespace NSIoPipe::NSPostProc;
using namespace NS3Av3;

/******************************************************************************
 *
 ******************************************************************************/
#define PROCESSOR_NAME_P2   ("P2_Feature")
#define PROCESSOR_NAME_MDP  ("MDP_P2_Feature")
#define P2THREAD_POLICY     (SCHED_OTHER)
#define P2THREAD_PRIORITY   (0)
//
#define WAITBUFFER_TIMEOUT (1000000000L)
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
#if 0
#define FUNC_START     MY_LOGD("+")
#define FUNC_END       MY_LOGD("-")
#else
#define FUNC_START
#define FUNC_END
#endif
#define P2_DEQUE_DEBUG (1)
/******************************************************************************
 *
 ******************************************************************************/
#define SUPPORT_3A               (1)
#define SUPPORT_EIS              (1)
#define SUPPORT_EIS_MV           (0)
#define FD_PORT_SUPPORT          (1)
#define FORCE_EIS_ON             (SUPPORT_EIS && (0))
#define FORCE_BURST_ON           (0)
#define DEBUG_LOG                (1)

#if 1/*[EP_TEMP]*/
#ifdef EIS_FACTOR
#undef EIS_FACTOR
#endif
#define EIS_FACTOR 120
#endif
/******************************************************************************
 *
 ******************************************************************************/
namespace P2Feature {
/******************************************************************************
 *
 ******************************************************************************/
#if 1/*[EP_TEMP]*/
#include <imageio/ispio_pipe_ports.h>
static const PortID PORT_IMGI( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_IMGI, 0);
static const PortID PORT_DEPI( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_DEPI, 0);
static const PortID PORT_WDMAO( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_WDMAO, 1);
static const PortID PORT_WROTO( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_WROTO, 1);
static const PortID PORT_IMG2O( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_IMG2O, 1);
#else
static const PortID PORT_IMGI  (EPortType_Memory, EPipePortIndex_IMGI , 0);
static const PortID PORT_WDMAO (EPortType_Memory, EPipePortIndex_WDMAO, 1);
static const PortID PORT_WROTO (EPortType_Memory, EPipePortIndex_WROTO, 1);
#if FD_PORT_SUPPORT
static const PortID PORT_IMG2O (EPortType_Memory, EPipePortIndex_IMG2O, 1);
#endif
#endif


inline
MBOOL isStream(sp<IStreamInfo> pStreamInfo, StreamId_T streamId ) {
    return pStreamInfo.get() && pStreamInfo->getStreamId() == streamId;
}


/******************************************************************************
 *
 ******************************************************************************/
class StreamControl
{
    public:
        typedef enum
        {
            eStreamStatus_NOT_USED = (0x00000000UL),
            eStreamStatus_FILLED   = (0x00000001UL),
            eStreamStatus_ERROR    = (0x00000001UL << 1),
        } eStreamStatus_t;

    public:

        virtual                         ~StreamControl() {};

    public:

        virtual MERROR                  getInfoIOMapSet(
                                            sp<IPipelineFrame> const& pFrame,
                                            IPipelineFrame::InfoIOMapSet& rIOMapSet
                                        ) const                                   = 0;

        // query in/out stream function
        virtual MBOOL                   isInImageStream(
                                            StreamId_T const streamId
                                        ) const                                   = 0;

        virtual MBOOL                   isInMetaStream(
                                            StreamId_T const streamId
                                        ) const                                   = 0;

        // image stream related
        virtual MERROR                  acquireImageStream(
                                            sp<IPipelineFrame> const& pFrame,
                                            StreamId_T const streamId,
                                            sp<IImageStreamBuffer>& rpStreamBuffer
                                        )                                         = 0;

        virtual MVOID                   releaseImageStream(
                                            sp<IPipelineFrame> const& pFrame,
                                            sp<IImageStreamBuffer> const pStreamBuffer,
                                            MUINT32 const status
                                        ) const                                   = 0;

        virtual MERROR                  acquireImageBuffer(
                                            StreamId_T const streamId,
                                            sp<IImageStreamBuffer> const pStreamBuffer,
                                            sp<IImageBuffer>& rpImageBuffer
                                        ) const                                   = 0;

        virtual MVOID                   releaseImageBuffer(
                                            sp<IImageStreamBuffer> const pStreamBuffer,
                                            sp<IImageBuffer> const pImageBuffer
                                        ) const                                   = 0;

        // meta stream related
        virtual MERROR                  acquireMetaStream(
                                            sp<IPipelineFrame> const& pFrame,
                                            StreamId_T const streamId,
                                            sp<IMetaStreamBuffer>& rpStreamBuffer
                                        )                                         = 0;

        virtual MVOID                   releaseMetaStream(
                                            sp<IPipelineFrame> const& pFrame,
                                            sp<IMetaStreamBuffer> const pStreamBuffer,
                                            MUINT32 const status
                                        ) const                                   = 0;

        virtual MERROR                  acquireMetadata(
                                            StreamId_T const streamId,
                                            sp<IMetaStreamBuffer> const pStreamBuffer,
                                            IMetadata*& rpMetadata
                                        ) const                                   = 0;

        virtual MVOID                   releaseMetadata(
                                            sp<IMetaStreamBuffer> const pStreamBuffer,
                                            IMetadata* const pMetadata
                                        ) const                                   = 0;

        // frame control related
        virtual MVOID                   onPartialFrameDone(
                                            sp<IPipelineFrame> const& pFrame
                                        )                                         = 0;

        virtual MVOID                   onFrameDone(
                                            sp<IPipelineFrame> const& pFrame
                                        )                                         = 0;

};


class MetaHandle
    : public VirtualLightRefBase
{
    public:
        typedef enum
        {
            STATE_NOT_USED,
            STATE_READABLE,
            STATE_WRITABLE,
            STATE_WRITE_OK = STATE_READABLE,
            STATE_WRITE_FAIL,
        } BufferState_t;

    public:
        static sp<MetaHandle>           create(
                                            StreamControl* const pCtrl,
                                            sp<IPipelineFrame> const& pFrame,
                                            StreamId_T const streamId
                                        );
                                        ~MetaHandle();
    protected:
                                        MetaHandle(
                                            StreamControl* pCtrl,
                                            sp<IPipelineFrame> const& pFrame,
                                            StreamId_T const streamId,
                                            sp<IMetaStreamBuffer> const pStreamBuffer,
                                            BufferState_t const init_state,
                                            IMetadata * const pMeta
                                        )
                                            : mpStreamCtrl(pCtrl)
                                            , mpFrame(pFrame)
                                            , mStreamId(streamId)
                                            , mpStreamBuffer(pStreamBuffer)
                                            , mpMetadata(pMeta)
                                            , muState(init_state)
                                        {}

    public:
        IMetadata*                      getMetadata() { return mpMetadata; }

#if 0
        virtual MERROR                  waitState(
                                            BufferState_t const state,
                                            nsecs_t const nsTimeout = WAITBUFFER_TIMEOUT
                                        )                                                   = 0;
#endif
        MVOID                           updateState(
                                            BufferState_t const state
                                        );
    private:
        Mutex                           mLock;
        //Condition                       mCond;
        StreamControl* const            mpStreamCtrl;
        sp<IPipelineFrame> const        mpFrame;
        StreamId_T const                mStreamId;
        sp<IMetaStreamBuffer> const     mpStreamBuffer;
        IMetadata* const                mpMetadata;
        MUINT32                         muState;
};


class BufferHandle
    : public VirtualLightRefBase
{
    public:
        typedef enum
        {
            STATE_NOT_USED,
            STATE_READABLE,
            STATE_WRITABLE,
            STATE_WRITE_OK = STATE_READABLE,
            STATE_WRITE_FAIL,
        } BufferState_t;

    public:
        virtual                         ~BufferHandle() {}

    public:
        virtual IImageBuffer*           getBuffer()                                         = 0;

        virtual MERROR                  waitState(
                                            BufferState_t const state,
                                            nsecs_t const nsTimeout = WAITBUFFER_TIMEOUT
                                        )                                                   = 0;
        virtual MVOID                   updateState(
                                            BufferState_t const state
                                        )                                                   = 0;
        virtual MUINT32                 getState()                                          = 0;

        virtual MUINT32                 getUsage() { return 0; };

        virtual MUINT32                 getTransform() { return 0; };

        virtual StreamId_T              getStreamId() { return 0; };
};


class StreamBufferHandle
    : public BufferHandle
{
    public:
        static sp<BufferHandle>         create(
                                            StreamControl* const pCtrl,
                                            sp<IPipelineFrame> const& pFrame,
                                            StreamId_T const streamId
                                        );
                                        ~StreamBufferHandle();
    protected:
                                        StreamBufferHandle(
                                            StreamControl* pCtrl,
                                            sp<IPipelineFrame> const& pFrame,
                                            StreamId_T const streamId,
                                            sp<IImageStreamBuffer> const pStreamBuffer,
                                            MUINT32 const uTransform,
                                            MUINT32 const uUsage
                                        )
                                            : mpStreamCtrl(pCtrl)
                                            , mpFrame(pFrame)
                                            , mStreamId(streamId)
                                            , mpStreamBuffer(pStreamBuffer)
                                            , muState(STATE_NOT_USED)
                                            , mpImageBuffer(NULL)
                                            , muTransform(uTransform)
                                            , muUsage(uUsage)
                                        {}

    public:
        IImageBuffer*                   getBuffer() { return mpImageBuffer.get(); }
        MERROR                          waitState(
                                            BufferState_t const state,
                                            nsecs_t const nsTimeout
                                        );
        MVOID                           updateState(
                                            BufferState_t const state
                                        );

        virtual MUINT32                 getState() { return muState; };

        virtual MUINT32                 getTransform() { return muTransform; };

        virtual MUINT32                 getUsage() { return muUsage; };

        virtual StreamId_T              getStreamId() { return mStreamId; };


    private:
        Mutex                           mLock;
        Condition                       mCond;
        StreamControl* const            mpStreamCtrl;
        sp<IPipelineFrame> const        mpFrame;
        StreamId_T const                mStreamId;
        sp<IImageStreamBuffer> const    mpStreamBuffer;
        MUINT32                         muState;
        MUINT32                         muTransform;
        MUINT32                         muUsage;
        sp<IImageBuffer>                mpImageBuffer;
};


class FrameLifeHolder : public VirtualLightRefBase
{
    public:
                                        FrameLifeHolder(
                                            StreamControl* const pCtrl,
                                            sp<IPipelineFrame> const& pFrame,
                                            MBOOL const enableLog
                                            )
                                            : mbEnableLog(enableLog)
                                            , mpStreamControl(pCtrl)
                                            , mpFrame(pFrame)
                                        {
                                            CAM_TRACE_ASYNC_BEGIN("P2F:FrameLifeHolder", mpFrame->getFrameNo());
                                            MY_LOGD_IF(mbEnableLog, "frame %zu +", mpFrame->getFrameNo());
                                        }

                                        ~FrameLifeHolder() {
                                            if( mpStreamControl )
                                                mpStreamControl->onFrameDone(mpFrame);
                                            MY_LOGD_IF(mbEnableLog, "frame %zu -", mpFrame->getFrameNo());
                                            CAM_TRACE_ASYNC_END("P2F:FrameLifeHolder", mpFrame->getFrameNo());
                                        }

    public:
        MVOID                           onPartialFrameDone() {
                                            if( mpStreamControl )
                                                mpStreamControl->onPartialFrameDone(mpFrame);
                                        }

    private:
        MBOOL const                     mbEnableLog;
        StreamControl* const            mpStreamControl; //stream control & dispatch
        sp<IPipelineFrame> const        mpFrame;
};

class Request;
class Processor: virtual public RefBase
{
public:
    virtual MERROR   queueRequest(sp<Request>)           = 0;
    virtual MERROR   handleRequest(sp<Request>)          = 0;
    virtual MVOID    flushRequests()                     = 0;
    virtual MVOID    waitForIdle()                       = 0;
    virtual MVOID    setNextProcessor(wp<Processor>)     = 0;
    virtual MBOOL    isAsyncProcessor()                  = 0;
    virtual MVOID    callbackAsync(sp<Request>, MERROR)  = 0;
    virtual MBOOL    close()                             = 0;
};

class Request
    : public VirtualLightRefBase
{
public:
    struct Context
    {
        MUINT8                      burst_num;
        MBOOL                       resized;
        StreamId_T                  fd_stream_id;
        sp<BufferHandle>            in_buffer;
        vector<sp<BufferHandle> >   out_buffers;
        sp<BufferHandle>            in_mdp_buffer;
        sp<MetaHandle>              in_app_meta;
#ifdef FEATURE_MODIFY
        // p1 app out meta, ex: iso for 3dnr
        sp<MetaHandle>              in_ret_app_meta;
#endif // FEATURE_MODIFY
        sp<MetaHandle>              in_hal_meta;
        sp<MetaHandle>              out_app_meta;
        sp<MetaHandle>              out_hal_meta;
    };

    struct Context context;

    typedef enum
    {
        STATE_WAIT,
        STATE_SUSPEND,
        STATE_RUNNING,
        STATE_DONE,
    } RequestState_t;

    RequestState_t          uState;

    Request(sp<FrameLifeHolder> pFrameLifeHolder):
        mpFrameLifeHolder(pFrameLifeHolder)
    {
    }

    MUINT32                    index;
    MUINT32                    frameNo;



    MVOID setCurrentOwner(wp<Processor> pProcessor)
    {
        mwpProcessor = pProcessor;
    }

    MVOID responseDone(MERROR status)
    {
        sp<Processor> spProcessor = mwpProcessor.promote();
        if(spProcessor.get())
        {
            MY_LOGD_IF(DEBUG_LOG, "perform callback %d[%d]",frameNo ,index);
            spProcessor->callbackAsync(this, status);
        }
    }

    MVOID onPartialRequestDone()
    {
        if(mpFrameLifeHolder.get())
            mpFrameLifeHolder->onPartialFrameDone();
    }

    ~Request()
    {
        if(context.in_buffer.get())
        {
            context.in_buffer.clear();
            MY_LOGW("context.in_buffer not released");
        }
        if(context.in_mdp_buffer.get())
        {
            context.in_mdp_buffer.clear();
            MY_LOGW("context.in_mdp_buffer not released");
        }

        vector<sp<BufferHandle> >::iterator iter = context.out_buffers.begin();
        while(iter != context.out_buffers.end())
        {
            if((*iter).get())
            {
                MY_LOGW("context.out_buffers[0x%x] not released", (*iter)->getStreamId());
                context.in_mdp_buffer.clear();
            }
            iter++;
        }

        if(context.in_app_meta.get())
        {
            context.in_app_meta.clear();
            MY_LOGW("context.in_app_meta not released");
        }
#ifdef FEATURE_MODIFY
        if(context.in_ret_app_meta.get())
        {
            context.in_ret_app_meta.clear();
            MY_LOGW("context.in_ret_app_meta not released");
        }
#endif // FEATURE_MODIFY
        if(context.in_hal_meta.get())
        {
            context.in_hal_meta.clear();
            MY_LOGW("context.in_hal_meta not released");
        }
        if(context.out_app_meta.get())
        {
            context.out_app_meta.clear();
            MY_LOGW("context.out_app_meta not released");
        }
        if(context.out_hal_meta.get())
        {
            context.out_hal_meta.clear();
            MY_LOGW("context.out_hal_meta not released");
        }

        onPartialRequestDone();
    }

private:
    sp<FrameLifeHolder>     mpFrameLifeHolder;
    wp<Processor>           mwpProcessor;

};


template<typename TProcedure>
struct ProcedureTraits {
    typedef typename TProcedure::InitParams  TInitParams;
    typedef typename TProcedure::FrameParams TProcParams;

    typedef MERROR (TProcedure::*TOnExtract)(Request*, TProcParams&);
    typedef MERROR (TProcedure::*TOnExecute)(sp<Request>,TProcParams const&);
    typedef MERROR (TProcedure::*TOnFinish)(TProcParams const&, MBOOL const);
    typedef MVOID  (TProcedure::*TOnFlush)();

    static constexpr TOnExtract   fnOnExtract     = &TProcedure::onExtractParams;
    static constexpr TOnExecute   fnOnExecute     = &TProcedure::onExecute;
    static constexpr TOnFinish    fnOnFinish      = &TProcedure::onFinish;
    static constexpr TOnFlush     fnOnFlush       = &TProcedure::onFlush;

    static constexpr MBOOL  isAsync               = TProcedure::isAsync;
};


template<typename TProcedure, typename TProcedureTraits = ProcedureTraits<TProcedure>>
class ProcessorBase : public Processor, public Thread
{
    typedef typename TProcedureTraits::TInitParams          TInitParams;
    typedef typename TProcedureTraits::TProcParams          TProcParams;

    typedef typename TProcedureTraits::TOnExtract           TOnExtract;
    typedef typename TProcedureTraits::TOnExecute           TOnExecute;
    typedef typename TProcedureTraits::TOnFinish            TOnFinish;
    typedef typename TProcedureTraits::TOnFlush             TOnFlush;


protected:
    TProcedure         mProcedure;

public:
    ProcessorBase(TInitParams const& initParams, char const* name)
        : Thread(false)
        , mProcedure(initParams)
        , mbRequestDrained(MTRUE)
        , mbExit(MFALSE)
        , mName(name)
    {
       run(name);
    }
    #define PROC_TAG(str) "[%s] " str, mName.string()
    virtual MBOOL close()
    {
        MY_LOGD_IF(DEBUG_LOG, PROC_TAG("close processor"));

        requestExit();
        join();
        waitForIdle();
        return MTRUE;
    }

    ~ProcessorBase()
    {
        MY_LOGD_IF(DEBUG_LOG, PROC_TAG("destroy processor"));
    }

    virtual MERROR queueRequest(sp<Request> pRequest)
    {
        Mutex::Autolock _l(mRequestLock);
        pRequest->setCurrentOwner(this);
        //  Make sure the request with a smaller frame number has a higher priority.
        vector<sp<Request> >::iterator it = mvPending.end();
        for (; it != mvPending.begin();)
        {
            --it;
            if (0 <= (MINT32)(pRequest->frameNo - (*it)->frameNo))
            {
                ++it; //insert(): insert before the current node
                break;
            }
        }
        mvPending.insert(it, pRequest);
        MY_LOGD_IF(DEBUG_LOG, PROC_TAG("after request[%d-%d] queued, pending:%d, running:%d"),
            pRequest->frameNo, pRequest->index, mvPending.size(), mvRunning.size());

        mRequestQueueCond.signal();
        return OK;
    }


    virtual MERROR handleRequest(sp<Request> pRequest)
    {
        TProcParams params;
        TOnExtract fnOnExtract = TProcedureTraits::fnOnExtract;
        TOnExecute fnOnExecute = TProcedureTraits::fnOnExecute;
        TOnFinish fnOnFinish = TProcedureTraits::fnOnFinish;

        if (OK == (mProcedure.*fnOnExtract)(pRequest.get(), params))
        {
            MERROR ret = (mProcedure.*fnOnExecute)(pRequest, params);
            if(isAsyncProcessor() && ret == OK)
            {
                // do aync processing
                Mutex::Autolock _l(mCallbackLock);
                mvRunning.push_back(make_pair(pRequest,params));
                MY_LOGD_IF(DEBUG_LOG, PROC_TAG("request[%d-%d], pending:%d, running:%d"),
                    pRequest->frameNo, pRequest->index, mvPending.size(), mvRunning.size());
                mCallbackCond.signal();
            }
            else
            {
                (mProcedure.*fnOnFinish)(params, ret == OK);
                // trigger to release buffer
                params = TProcParams();
                pRequest->onPartialRequestDone();

                sp<Processor> spProcessor = mwpNextProcessor.promote();
                if (spProcessor.get())
                {
                    spProcessor->queueRequest(pRequest);
                }
            }
            return ret;
        }
        return OK;
    }

    virtual MVOID callbackAsync(sp<Request> pRequest, MERROR status)
    {
        if (isAsyncProcessor())
        {
            Mutex::Autolock _l1(mAsyncLock);
            Mutex::Autolock _l2(mCallbackLock);
            MY_LOGD_IF(DEBUG_LOG, PROC_TAG("request[%d-%d], pending:%d, running:%d"),
                pRequest->frameNo, pRequest->index, mvPending.size(), mvRunning.size());
            TOnFinish fnOnFinish = &TProcedure::onFinish;

            MBOOL isFound = MFALSE;

            do
            {
                typename vector<pair<sp<Request>,TProcParams> >::iterator iter = mvRunning.begin();
                for(; iter != mvRunning.end(); iter++)
                   if((*iter).first == pRequest)
                   {
                       (mProcedure.*fnOnFinish)((*iter).second, status == OK);
                       mvRunning.erase(iter);
                       isFound = MTRUE;
                       break;
                   }

                if(!isFound)
                {
                     MY_LOGW_IF(1 ,PROC_TAG("request[%d-%d] callback faster than execution finished"),
                        pRequest->frameNo, pRequest->index);
                     mCallbackCond.wait(mCallbackLock);
                }
            } while (!isFound);

            MY_LOGD_IF(DEBUG_LOG, PROC_TAG("request callback async, status:%d"), status);
            pRequest->onPartialRequestDone();

            mAsyncCond.signal();
            sp<Processor> spProcessor = mwpNextProcessor.promote();
            if (spProcessor.get())
            {
                spProcessor->queueRequest(pRequest);
            }
        }
        return;
    }

    virtual MVOID setNextProcessor(wp<Processor> pProcessor)
    {
        mwpNextProcessor = pProcessor;
    }

    virtual MBOOL isAsyncProcessor()
    {
        return TProcedureTraits::isAsync;
    }

public:
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //  Thread Interface.
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // Ask this object's thread to exit. This function is asynchronous, when the
    // function returns the thread might still be running. Of course, this
    // function can be called from a different thread.
    virtual void requestExit()
    {
        Mutex::Autolock _l(mRequestLock);
        mbExit = MTRUE;
        mRequestQueueCond.signal();
    }

    // Good place to do one-time initializations
    virtual status_t readyToRun()
    {
        //
        //  thread policy & priority
        //  Notes:
        //      Even if pthread_create() with SCHED_OTHER policy, a newly-created thread
        //      may inherit the non-SCHED_OTHER policy & priority of the thread creator.
        //      And thus, we must set the expected policy & priority after a thread creation.
        MINT tid;
        struct sched_param sched_p;
        ::sched_getparam(0, &sched_p);
        if (P2THREAD_POLICY == SCHED_OTHER) {
            sched_p.sched_priority = 0;
            ::sched_setscheduler(0, P2THREAD_POLICY, &sched_p);
            ::setpriority(PRIO_PROCESS, 0, P2THREAD_PRIORITY);   //  Note: "priority" is nice value.
        } else {
            sched_p.sched_priority = P2THREAD_PRIORITY;          //  Note: "priority" is real-time priority.
            ::sched_setscheduler(0, P2THREAD_POLICY, &sched_p);
        }

        MY_LOGD_IF(1 ,PROC_TAG("tid(%d) policy(%d) priority(%d)"), ::gettid(), P2THREAD_POLICY, P2THREAD_PRIORITY);

        return OK;

    }

private:

    // Derived class must implement threadLoop(). The thread starts its life
    // here. There are two ways of using the Thread object:
    // 1) loop: if threadLoop() returns true, it will be called again if
    //          requestExit() wasn't called.
    // 2) once: if threadLoop() returns false, the thread will exit upon return.
    virtual bool threadLoop()
    {
        while(!exitPending() && OK == onWaitRequest())
        {
            if( mvPending.size() == 0 )
            {
                MY_LOGW_IF(1 ,PROC_TAG("no request"));
                return true;
            }

            sp<Request> pRequest = NULL;
            {
                Mutex::Autolock _l(mRequestLock);

                pRequest = mvPending.front();
                mvPending.erase( mvPending.begin());
            }

            MY_LOGE_IF(handleRequest(pRequest) != OK, "request execute failed");

            return true;
        }
        MY_LOGD_IF(DEBUG_LOG, PROC_TAG("exit thread"));
        return  false;
    }

public:

    virtual MVOID flushRequests()
    {
        FUNC_START;

        Mutex::Autolock _l(mRequestLock);

        mvPending.clear();

        if (!mbRequestDrained)
        {
            MY_LOGD_IF(1 ,PROC_TAG("wait for request drained"));
            mRequestDrainedCond.wait(mRequestLock);
        }

        TOnFlush fnOnFlush = TProcedureTraits::fnOnFlush;
        (mProcedure.*fnOnFlush)();

        FUNC_END;
        return;
    }

    virtual MVOID waitForIdle()
    {
        if(isAsyncProcessor())
            return;

        Mutex::Autolock _l(mAsyncLock);
        while(mvRunning.size())
        {
            MY_LOGD_IF(1 ,PROC_TAG("wait request done %d"), mvRunning.size());
            mAsyncCond.wait(mAsyncLock);
        }

        return;
    }

    virtual MERROR onWaitRequest()
    {
        Mutex::Autolock _l(mRequestLock);
        while (!mvPending.size() && !mbExit)
        {
            // set drained flag
            mbRequestDrained = MTRUE;
            mRequestDrainedCond.signal();

            status_t status = mRequestQueueCond.wait(mRequestLock);
            if (OK != status)
            {
                MY_LOGW_IF(1 ,PROC_TAG("wait status:%d:%s, request size:%zu, exit:%d"),
                    status, ::strerror(-status), mvPending.size(), mbExit
                );
            }
        }

        if (mbExit)
        {
            MY_LOGW_IF(mvPending.size(), PROC_TAG("existed mvPending.size:%zu"), mvPending.size());
            return DEAD_OBJECT;
        }

        mbRequestDrained = MFALSE;
        return OK;
    }

protected:
    vector<sp<Request> >     mvPending;
    // for async request
    vector<pair<sp<Request>,TProcParams> >
                                    mvRunning;
    wp<Processor>                   mwpNextProcessor;
    mutable Mutex                   mRequestLock;
    mutable Condition               mRequestQueueCond;
    mutable Condition               mRequestDrainedCond;
    MBOOL                           mbRequestDrained;
    MBOOL                           mbExit;
    mutable Mutex                   mAsyncLock;
    mutable Condition               mAsyncCond;
    mutable Mutex                   mCallbackLock;
    mutable Condition               mCallbackCond;
    String8                         mName;

};


class P2Procedure
{
    protected:
        class MultiFrameHandler
        {
            public:
                MultiFrameHandler(IStreamingFeaturePipe* const pPipe, MBOOL bEableLog)
                    : muMfEnqueCnt(0)
                    , muMfDequeCnt(0)
                    , mbEnableLog(bEableLog)
                    , mpPipe(pPipe)
                {
                }
                MERROR                      collect(sp<Request>, FeaturePipeParam&);
                MVOID                       flush();
                static MVOID                callback(QParams& rParams)
                {
                    MultiFrameHandler* pHandler = reinterpret_cast<MultiFrameHandler*>(rParams.mpCookie);
                    pHandler->handleCallback(rParams);
                }
                MVOID                       handleCallback(QParams& rParams);

            private:
                IStreamingFeaturePipe* const mpPipe;
                mutable Mutex               mLock;
                MBOOL const                 mbEnableLog;
                QParams                     mParamCollecting;
                MUINT32                     muMfEnqueCnt;
                MUINT32                     muMfDequeCnt;
                vector<sp<Request> >        mvReqCollecting;
                vector<vector<sp<Request> > > mvRunning;
        };


    public:
        struct InitParams
        {
            MINT32                          openId;
            P2FeatureNode::ePass2Type       type;
            MRect                           activeArray;
            IHal3A_T*                       p3A;
            IStreamingFeaturePipe*          pPipe;
            MBOOL                           enableLog;
            MBOOL                           enableDumpBuffer;
        };

        struct FrameInput
        {
            PortID                          mPortId;
            sp<BufferHandle>                mHandle;
        };

        struct FrameOutput
        {
            FrameOutput()
            : mUsage(0)
            , mTransform(0)
            {};
            PortID                          mPortId;
            sp<BufferHandle>                mHandle;
            MINT32                          mTransform;
            MUINT32                         mUsage;
        };

        struct FrameParams
        {
            FrameInput           in;
            Vector<FrameOutput>  vOut;
            //
            MBOOL                bResized;
            //
            sp<MetaHandle>       inApp;
#ifdef FEATURE_MODIFY
            sp<MetaHandle>       inRetApp;
#endif // FEATURE_MODIFY
            sp<MetaHandle>       inHal;
            sp<MetaHandle>       outApp;
            sp<MetaHandle>       outHal;
        };

    private: //private use structures
        struct eis_region
        {
            MUINT32 x_int;
            MUINT32 x_float;
            MUINT32 y_int;
            MUINT32 y_float;
            MSize   s;
#if SUPPORT_EIS_MV
            MUINT32 x_mv_int;
            MUINT32 x_mv_float;
            MUINT32 y_mv_int;
            MUINT32 y_mv_float;
            MUINT32 is_from_zzr;
#endif
#ifdef FEATURE_MODIFY
            // 3dnr vipi: needs x_int/y_int/gmvX/gmvY
            MINT32 gmvX;
            MINT32 gmvY;
#endif // FEATURE_MODIFY
        };

#if 0
        struct vector_f
        {
            MINT32  x_int;
            MINT32  y_int;
            MUINT32 x_float;
            MUINT32 y_float;
        };
#endif

        struct crop_infos
        {
            // port
            MBOOL                isResized;
            //
            MSize                sensor_size;
            // p1 crop infos
            MRect                crop_p1_sensor;
            MSize                dstsize_resizer;
            MRect                crop_dma;
            //
            simpleTransform      tranActive2Sensor;
            simpleTransform      tranSensor2Resized;
            //
            // target crop: cropRegion
            // not applied eis's mv yet, but the crop area is already reduced by
            // EIS ratio.
            // _a: active array coordinates
            // _s: sensor coordinates
            // active array coordinates
            MRect                crop_a;
            //MPoint               crop_a_p;
            //MSize                crop_a_size;
            // sensor coordinates
            //MPoint               crop_s_p;
            //MSize                crop_s_size;
            // resized coordinates
            //
            MBOOL                isEisEabled;
            vector_f             eis_mv_a; //active array coor.
            vector_f             eis_mv_s; //sensor coor.
            vector_f             eis_mv_r; //resized coor.

            MVOID                       dump() const {
                                            MY_LOGD("isResized %d", isResized);
                                            MY_LOGD("p1 info (%d,%d,%dx%d), (%dx%d), (%d,%d,%dx%d)",
                                                    crop_p1_sensor.p.x,
                                                    crop_p1_sensor.p.y,
                                                    crop_p1_sensor.s.w,
                                                    crop_p1_sensor.s.h,
                                                    dstsize_resizer.w,
                                                    dstsize_resizer.h,
                                                    crop_dma.p.x,
                                                    crop_dma.p.y,
                                                    crop_dma.s.w,
                                                    crop_dma.s.h
                                                   );
                                            MY_LOGD("tran active to sensor o %d, %d, s %dx%d -> %dx%d",
                                                    tranActive2Sensor.tarOrigin.x,
                                                    tranActive2Sensor.tarOrigin.y,
                                                    tranActive2Sensor.oldScale.w,
                                                    tranActive2Sensor.oldScale.h,
                                                    tranActive2Sensor.newScale.w,
                                                    tranActive2Sensor.newScale.h
                                                   );
                                            MY_LOGD("tran sensor to resized o %d, %d, s %dx%d -> %dx%d",
                                                    tranSensor2Resized.tarOrigin.x,
                                                    tranSensor2Resized.tarOrigin.y,
                                                    tranSensor2Resized.oldScale.w,
                                                    tranSensor2Resized.oldScale.h,
                                                    tranSensor2Resized.newScale.w,
                                                    tranSensor2Resized.newScale.h
                                                   );
                                            MY_LOGD("modified active crop %d, %d, %dx%d",
                                                    crop_a.p.x,
                                                    crop_a.p.y,
                                                    crop_a.s.w,
                                                    crop_a.s.h
                                                   );
                                            MY_LOGD("isEisOn %d", isEisEabled);
                                            MY_LOGD("mv in active %d/%d, %d/%d",
                                                    eis_mv_a.p.x, eis_mv_a.pf.x,
                                                    eis_mv_a.p.y, eis_mv_a.pf.y
                                                    );
                                            MY_LOGD("mv in sensor %d/%d, %d/%d",
                                                    eis_mv_s.p.x, eis_mv_s.pf.x,
                                                    eis_mv_s.p.y, eis_mv_s.pf.y
                                                    );
                                            MY_LOGD("mv in resized %d/%d, %d/%d",
                                                    eis_mv_r.p.x, eis_mv_r.pf.x,
                                                    eis_mv_r.p.y, eis_mv_r.pf.y
                                                    );
                                        }
        };

    public:
        static sp<Processor>            createProcessor(InitParams& params);
                                        ~P2Procedure();

                                        P2Procedure(InitParams const& params)
                                            : mInitParams(params)
                                            , mpPipe(params.pPipe)
                                            , mp3A(params.p3A)
                                            , muEnqueCnt(0)
                                            , muDequeCnt(0)
                                            , mbEnableLog(params.enableLog)
                                            , mbEnableDumpBuffer(params.enableDumpBuffer)
                                        {
                                            mpMultiFrameHandler = new MultiFrameHandler(
                                                params.pPipe, params.enableLog);
                                        }

        MERROR                          onExtractParams(Request*, FrameParams&);

    public:
        MERROR                          waitIdle();

    public:                             // used by job
        static const MBOOL              isAsync = MTRUE;

        MERROR                          onExecute(
                                            sp<Request> const pRequest,
                                            FrameParams const& params
                                        );

        MERROR                          onFinish(
                                            FrameParams const& params,
                                            MBOOL const success
                                        );

        MVOID                           onFlush();

    protected:

        MERROR                          mapPortId(
                                            StreamId_T const streamId, // [in]
                                            MUINT32 const transform,   // [in]
                                            MBOOL const isFdStream,    // [in]
                                            MUINT32& rOccupied,        // [in/out]
                                            PortID&  rPortId           // [out]
                                        ) const;

        MERROR                          checkParams(FrameParams const params) const;
#ifdef FEATURE_MODIFY
        MERROR                          getCropInfos_NoEIS(
                                            IMetadata* const inApp,
                                            IMetadata* const inHal,
                                            MBOOL const isResized,
                                            MSize const& dstSize,
                                            MCropRect& result
                                        ) const;
#endif
        MERROR                          getCropInfos(
                                            IMetadata* const inApp,
                                            IMetadata* const inHal,
                                            MBOOL const isResized,
                                            crop_infos & cropInfos
                                        ) const;

        MVOID                           queryCropRegion(
                                            IMetadata* const meta_request,
                                            MBOOL const isEisOn,
                                            MRect& targetCrop
                                        ) const;

        MVOID                           updateCropRegion(
                                            MRect const crop,
                                            IMetadata* meta_result
                                        ) const;

        MVOID                           calcCrop_viewangle(
                                            crop_infos const& cropInfos,
                                            MSize const& dstSize,
                                            MCropRect& result
                                        ) const;

        MBOOL                           isEISOn(
                                            IMetadata* const inApp
                                        ) const;

        MBOOL                           queryEisRegion(
                                            IMetadata* const inHal,
                                            eis_region& region
                                        ) const;

        MBOOL                           refineBoundary(
                                            MSize const& bufSize,
                                            MCropRect& crop
                                        ) const;

#ifdef FEATURE_MODIFY
        MVOID                           prepareFeaturePipeParam(
                                          FeaturePipeParam &featureEnqueParams,
                                          const sp<Request> &pRequest,
                                          IMetadata *pMeta_InApp,
                                          IMetadata *pMeta_InRetApp,
                                          IMetadata *pMeta_InHal,
                                          IMetadata *pMeta_OutApp,
                                          IMetadata *pMeta_OutHal,
                                          const crop_infos &cropInfos);
        MBOOL                           setP2B3A(FeaturePipeParam &param);
        static MBOOL                    featurePipeCB(FeaturePipeParam::MSG_TYPE msg, FeaturePipeParam &data);

        MVOID prepareFeatureData_3DNR(
            FeaturePipeParam &featureEnqueParams,
            MINT32 dstsize_resizer_w, MINT32 dstsize_resizer_h,
            IMetadata *pMeta_InApp,  IMetadata *pMeta_InRetApp, IMetadata *pMeta_OutApp, IMetadata *pMeta_InHal, IMetadata *pMeta_OutHal
        );
#endif // FEATURE_MODIFY
        static MVOID                    pass2CbFunc(QParams& rParams);

        MVOID                           handleDeque(QParams& rParams);

    private:
        MultiFrameHandler*            mpMultiFrameHandler;
        MBOOL const                     mbEnableLog;
        MBOOL const                     mbEnableDumpBuffer;
        //
        mutable Mutex                   mLock;
        mutable Condition               mCondJob;
        //
        InitParams const                mInitParams;
        //
        IStreamingFeaturePipe* const    mpPipe;
        IHal3A_T* const                 mp3A;
        //
        MUINT32                         muEnqueCnt;
        MUINT32                         muDequeCnt;
        vector<sp<Request> >            mvRunning;
#if P2_DEQUE_DEBUG
        vector<QParams>                 mvParams;
#endif
};


class MDPProcedure
{
    public:
        struct InitParams
        {
            MBOOL                          enableLog;
        };

        struct FrameInput
        {
            sp<BufferHandle>                mHandle;
        };

        struct FrameOutput
        {
            sp<BufferHandle>                mHandle;
            MINT32                          mTransform;
        };

        struct FrameParams
        {
            FrameInput           in;
            Vector<FrameOutput>  vOut;
            MRect                crop;
        };
    public:
        static sp<Processor>            createProcessor(InitParams& params);
                                        ~MDPProcedure() {}
                                        MDPProcedure(InitParams const& params)
                                        : mbEnableLog(params.enableLog)
                                        {}

        MERROR                          onExtractParams(Request*, FrameParams&);

    protected:

                                        MDPProcedure(MBOOL const enableLog)
                                        : mbEnableLog(enableLog)
                                        {}

    public:
        static const MBOOL              isAsync = MFALSE;

        MERROR                          waitIdle() { return OK; } // since is synchronous

        MERROR                          onExecute(
                                            sp<Request> const pRequest,

                                            FrameParams const& params
                                        );

        MERROR                          onFinish(
                                            FrameParams const& params,
                                            MBOOL const success
                                        );

        MVOID                           onFlush(){};

    private:
        MBOOL const                     mbEnableLog;

};


/******************************************************************************
 *
 ******************************************************************************/
class P2NodeImp
    : public BaseNode
    , public P2FeatureNode
    , public StreamControl
{
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Implementations.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////                                            Definitions.
    typedef android::sp<IPipelineFrame>                     QueNode_T;
    typedef android::List<QueNode_T>                        Que_T;
    //
public:     ////                    Operations.

                                    P2NodeImp(ePass2Type const type);

                                    ~P2NodeImp();

    virtual MERROR                  config(ConfigParams const& rParams);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  IPipelineNode Interface.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////                    Operations.


    virtual MERROR                  init(InitParams const& rParams);

    virtual MERROR                  uninit();

    virtual MERROR                  flush();

    virtual MERROR                  queue(
                                        android::sp<IPipelineFrame> pFrame
                                    );

protected:  ////                    Operations.
    MVOID                           onProcessFrame(
                                        android::sp<IPipelineFrame> const& pFrame
                                    );
    MERROR                          verifyConfigParams(
                                        ConfigParams const & rParams
                                    ) const;

public:     ////                    StreamControl

    MERROR                          getInfoIOMapSet(
                                        sp<IPipelineFrame> const& pFrame,
                                        IPipelineFrame::InfoIOMapSet& rIOMapSet
                                    ) const;

    MBOOL                           isInImageStream(
                                        StreamId_T const streamId
                                    ) const;

    MBOOL                           isInMetaStream(
                                        StreamId_T const streamId
                                    ) const;

    MERROR                          acquireImageStream(
                                        sp<IPipelineFrame> const& pFrame,
                                        StreamId_T const streamId,
                                        sp<IImageStreamBuffer>& rpStreamBuffer
                                    );

    MVOID                           releaseImageStream(
                                        sp<IPipelineFrame> const& pFrame,
                                        sp<IImageStreamBuffer> const pStreamBuffer,
                                        MUINT32 const status
                                    ) const;

    MERROR                          acquireImageBuffer(
                                        StreamId_T const streamId,
                                        sp<IImageStreamBuffer> const pStreamBuffer,
                                        sp<IImageBuffer>& rpImageBuffer
                                    ) const;

    MVOID                           releaseImageBuffer(
                                        sp<IImageStreamBuffer> const rpStreamBuffer,
                                        sp<IImageBuffer> const pImageBuffer
                                    ) const;

    MERROR                          acquireMetaStream(
                                        sp<IPipelineFrame> const& pFrame,
                                        StreamId_T const streamId,
                                        sp<IMetaStreamBuffer>& rpStreamBuffer
                                    );

    MVOID                           releaseMetaStream(
                                        sp<IPipelineFrame> const& pFrame,
                                        sp<IMetaStreamBuffer> const pStreamBuffer,
                                        MUINT32 const status
                                    ) const;

    MERROR                          acquireMetadata(
                                        StreamId_T const streamId,
                                        sp<IMetaStreamBuffer> const pStreamBuffer,
                                        IMetadata*& rpMetadata
                                    ) const;

    MVOID                           releaseMetadata(
                                        sp<IMetaStreamBuffer> const pStreamBuffer,
                                        IMetadata* const pMetadata
                                    ) const;

    MVOID                           onPartialFrameDone(
                                        sp<IPipelineFrame> const& pFrame
                                    );

    MVOID                           onFrameDone(
                                        sp<IPipelineFrame> const& pFrame
                                    );

public:

    MERROR                          mapToRequests(
                                        android::sp<IPipelineFrame> const& pFrame
                                    );

inline MBOOL                        isFullRawLocked(StreamId_T const streamId) const {
                                        for( size_t i = 0; i < mpvInFullRaw.size(); i++ ) {
                                            if( isStream(mpvInFullRaw[i], streamId) )
                                                return MTRUE;
                                        }
                                        return MFALSE;
                                    }

inline MBOOL                        isResizeRawLocked(StreamId_T const streamId) const {
                                        return isStream(mpInResizedRaw, streamId);
                                    }

protected:  ////                    LOGE & LOGI on/off
    MINT32                          mLogLevel;

protected:  ////                    Data Members. (Config)
    ePass2Type const                mType;
    mutable RWLock                  mConfigRWLock;
    // meta
    sp<IMetaStreamInfo>             mpInAppMeta_Request;
#ifdef FEATURE_MODIFY
    sp<IMetaStreamInfo>             mpInAppRetMeta_Request;
#endif // FEATURE_MODIFY
    sp<IMetaStreamInfo>             mpInHalMeta_P1;
    sp<IMetaStreamInfo>             mpOutAppMeta_Result;
    sp<IMetaStreamInfo>             mpOutHalMeta_Result;

    // image
    Vector<sp<IImageStreamInfo> >   mpvInFullRaw;
    sp<IImageStreamInfo>            mpInResizedRaw;
    ImageStreamInfoSetT             mvOutImages;
    sp<IImageStreamInfo>            mpOutFd;
    // feature
    MUINT8 mBurstNum;

protected:  ////                    Data Members. (Operation)
    mutable Mutex                   mOperationLock;

private:
    sp<Processor>                   mpP2Processor;
    sp<Processor>                   mpMdpProcessor;
};

/******************************************************************************
 *
 ******************************************************************************/
#ifdef FEATURE_MODIFY
MBOOL isAPEnabled_3DNR(IMetadata *appInMeta)
{
    MINT32 e3DnrMode = MTK_NR_FEATURE_3DNR_MODE_OFF;

    if( appInMeta == NULL ||
        !tryGetMetadata<MINT32>(appInMeta, MTK_NR_FEATURE_3DNR_MODE, e3DnrMode) )
    {
        MY_LOGE("no MTK_NR_FEATURE_3DNR_MODE: appInMeta: %p", appInMeta);
    }

    char EnableOption[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("camera.3dnr.enable", EnableOption, "n");
    if (EnableOption[0] == '1')
    {
        e3DnrMode = MTK_NR_FEATURE_3DNR_MODE_ON;
    }
    else if (EnableOption[0] == '0')
    {
        e3DnrMode = MTK_NR_FEATURE_3DNR_MODE_OFF;
    }

    MY_LOGD("e3DnrMode: value: %d  (ps: MTK_NR_FEATURE_3DNR_MODE_ON = %d)",
        e3DnrMode, MTK_NR_FEATURE_3DNR_MODE_ON);

    return (e3DnrMode == MTK_NR_FEATURE_3DNR_MODE_ON) ;
}

MBOOL isAPEnabled_VHDR(IMetadata *halInMeta)
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.lce.disable", value, "0"); // 0: enable, 1: disable
    MBOOL bDisable = atoi(value);

    if(bDisable) {
        MY_LOGD("disable p2 VHDR");
        return MFALSE;
    } else {
        MY_LOGD("enable p2 VHDR");
        return MTRUE;
    }
#if 0
    // Because LCEI module will be always on, so this always returns true except VFB on.
    if( isVFBOn)
        return MFALSE;

    IMetadata::IEntry entry = halInMeta->entryFor(MTK_VHDR_LCEI_DATA);
    return ! entry.isEmpty();
#endif
}

MBOOL isHALenabled_VHDR(IMetadata *halInMeta)
{
    MBOOL enabledVHDR = MFALSE;
    MUINT8 vhdrMode = 0;

    if (!tryGetMetadata<MUINT8>(halInMeta, MTK_3A_ISP_PROFILE, vhdrMode))
    {
        enabledVHDR = MFALSE;

    } else {
        if ( (vhdrMode == EIspProfile_zHDR_Preview) ||
             (vhdrMode == EIspProfile_zHDR_Video)   ||
             (vhdrMode == EIspProfile_zHDR_Capture))
        {
             enabledVHDR = MTRUE;
        }
    }

    MY_LOGD("hasFeature_VHDR: %d", enabledVHDR);

    return enabledVHDR;
}

MBOOL isAPEnabled_VFB(IMetadata *appInMeta)
{
    return MFALSE;
}

MBOOL isAPEnabled_VFB_EX(IMetadata *appInMeta)
{
    return MFALSE;
}

MBOOL isAPEnabled_EIS(IMetadata *appInMeta)
{
    MBOOL enabledStreamingEIS = MFALSE;
    MUINT8 eisMode = MTK_CONTROL_VIDEO_STABILIZATION_MODE_OFF;

    if (!tryGetMetadata<MUINT8>(appInMeta, MTK_CONTROL_VIDEO_STABILIZATION_MODE, eisMode))
    {
        MY_LOGW("no MTK_CONTROL_VIDEO_STABILIZATION_MODE");
        return MFALSE;

    }

    enabledStreamingEIS = (eisMode == MTK_CONTROL_VIDEO_STABILIZATION_MODE_ON) ? MTRUE : MFALSE;

    MY_LOGD("hasFeature_EIS: %d", enabledStreamingEIS);
    return enabledStreamingEIS;
    //return MFALSE;
}

MVOID P2Procedure::prepareFeatureData_3DNR(
    FeaturePipeParam &featureEnqueParams,
    MINT32 dstsize_resizer_w, MINT32 dstsize_resizer_h,
    IMetadata *pMeta_InApp,  IMetadata *pMeta_InRetApp, IMetadata *pMeta_OutApp, IMetadata *pMeta_InHal,  IMetadata *pMeta_OutHal
    )
{
    MY_LOGD("HAS_3DNR: 0x%x", HAS_3DNR(featureEnqueParams.mFeatureMask) );

    if( HAS_3DNR(featureEnqueParams.mFeatureMask) )
    {
        // cropInfo
        featureEnqueParams.setVar<MUINT32>("3dnr.dstsize_resizer.w", dstsize_resizer_w);
        featureEnqueParams.setVar<MUINT32>("3dnr.dstsize_resizer.h", dstsize_resizer_h);
        MY_LOGD("bbbb_cropInfo_test: w=%d, h=%d",  dstsize_resizer_w, dstsize_resizer_h);

        #if 0 // debug usage
        {
            MUINT32 www = dstsize_resizer_w;
            MUINT32 hhh = dstsize_resizer_h;
            featureEnqueParams.setVar<MUINT32>("3dnr.dstsize_resizer.w", www);
            featureEnqueParams.setVar<MUINT32>("3dnr.dstsize_resizer.h", hhh);
            MY_LOGE("DEBUG_bbbb_cropInfo_test: w=%d, h=%d", dstsize_resizer_w, dstsize_resizer_h);

            MUINT32 aaaa_www = 0;
            MUINT32 aaaa_hhh = 0;
            aaaa_www = featureEnqueParams.getVar<MUINT32>("3dnr.dstsize_resizer.w", aaaa_www);
            aaaa_hhh = featureEnqueParams.getVar<MUINT32>("3dnr.dstsize_resizer.h", aaaa_hhh);
            MY_LOGE("P2FeatureNode_aaaa_cropInfo_test: w=%d, h=%d",
            aaaa_www, aaaa_hhh);
        }
        #endif


        // === EIS info ====
        MBOOL tmp_isEISOn = isEISOn(pMeta_InApp);
        eis_region eisInfo;
        queryEisRegion(pMeta_InHal, eisInfo);

        featureEnqueParams.setVar<MBOOL>("3dnr.eis.isEisOn", tmp_isEISOn);
        featureEnqueParams.setVar<MUINT32>("3dnr.eis.x_int", eisInfo.x_int);
        featureEnqueParams.setVar<MUINT32>("3dnr.eis.y_int", eisInfo.y_int);
        featureEnqueParams.setVar<MUINT32>("3dnr.eis.gmvX", eisInfo.gmvX);
        featureEnqueParams.setVar<MUINT32>("3dnr.eis.gmvY", eisInfo.gmvY);

        MY_LOGD("bbbb_3dnr.eis: isEisOn=%d, x_int=%d, y_int=%d, gmvX=%d, gmvY=%d, ",
            tmp_isEISOn,
            eisInfo.x_int,
            eisInfo.y_int,
            eisInfo.gmvX,
            eisInfo.gmvY
            );

    // === ISO value ===
    MINT32 iso = 800; // dummy default value
    if (pMeta_InRetApp != NULL)
        {
            if ( !tryGetMetadata<MINT32>(pMeta_InRetApp, MTK_SENSOR_SENSITIVITY, iso))
            {
                MY_LOGE("bbbb_3dnr.iso: MTK_SENSOR_SENSITIVITY NOT found");
            }
        }
        else
        {
            MY_LOGD("pMeta_InRetApp == NULL");
        }
        featureEnqueParams.setVar<MUINT32>("3dnr.iso", iso);
        MY_LOGD("bbbb_3dnr.iso: iso: %d", iso);

    }
}

MVOID prepareFeatureData_VHDR(FeaturePipeParam &pipeParam, IMetadata *pMeta_InHal)
{

    if( HAS_VHDR(pipeParam.mFeatureMask) )
    {
        IMetadata::Memory lcei_mem;

        if( tryGetMetadata<IMetadata::Memory>(pMeta_InHal, MTK_VHDR_LCEI_DATA, lcei_mem) ) {


            if (lcei_mem.size() == sizeof(LCEI_CONFIG_DATA)){

                LCEI_CONFIG_DATA config;
                ::memcpy((void*)&config, lcei_mem.array(), lcei_mem.size());
                MUINT32 ret = pipeParam.setVar<LCEI_CONFIG_DATA>("vhdr_lcei_data", config);
            }else{
                MY_LOGI("[VHDR] p2 lcei metatdata size != LCEI_CONFIG_DATA");
            }

        }else{
            MY_LOGI("[VHDR] no entry in p2 for MTK_VHDR_LCEI_DATA");
        }
    }

}

MVOID prepareFeatureData_VFB(FeaturePipeParam &pipeParam)
{
    if( HAS_VFB(pipeParam.mFeatureMask) )
    {
    }
}

MVOID prepareFeatureData_EIS(FeaturePipeParam &pipeParam, IMetadata *pMeta_InHal)
{
    if( HAS_EIS(pipeParam.mFeatureMask) )
    {
        MINT32 GMV_X,GMV_Y,ConfX,ConfY;
        MINT32 ExpTime = 0,ihwTS = 0,ilwTS = 0;
        MINT32 eisMode = 0;
        MINT64 ts = 0,tmp;
        GMV_X = GMV_Y = ConfX = ConfY = 0;

        IMetadata::IEntry entryRegion = pMeta_InHal->entryFor(MTK_EIS_REGION);
        if (entryRegion.count() > 14)
        {
            GMV_X = entryRegion.itemAt(9, Type2Type<MINT32>());
            GMV_Y = entryRegion.itemAt(10, Type2Type<MINT32>());
            ConfX = entryRegion.itemAt(11, Type2Type<MINT32>());
            ConfY = entryRegion.itemAt(12, Type2Type<MINT32>());
            ExpTime = entryRegion.itemAt(13, Type2Type<MINT32>());
            ihwTS = entryRegion.itemAt(14, Type2Type<MINT32>());
            ilwTS = entryRegion.itemAt(15, Type2Type<MINT32>());
            tmp = (MINT64)(ihwTS&0xFFFFFFFF);
            ts = (tmp<<32) + ((MINT64)ilwTS&0xFFFFFFFF);
            //MY_LOGD("EisHal TS hw: (%x)  lw: (%x) %lld", ihwTS,ilwTS,ts);

        }

        pipeParam.setVar<MINT32>("eis.gmv_x", GMV_X);
        pipeParam.setVar<MINT32>("eis.gmv_y", GMV_Y);
        pipeParam.setVar<MUINT32>("eis.confX", ConfX);
        pipeParam.setVar<MUINT32>("eis.confX", ConfY);
        pipeParam.setVar<MINT32>("eis.expTime", ExpTime);
        pipeParam.setVar<MINT64>("eis.timeStamp", ts);


        IMetadata::IEntry entryMode = pMeta_InHal->entryFor(MTK_EIS_MODE);
        if (entryMode.count() > 0) {
            eisMode = entryMode.itemAt(0, Type2Type<MINT32>());
        }
        MY_LOGD("EisHal ExpTime: %d, TS: %lld", ExpTime, ts);
        pipeParam.setVar<MINT32>("eis.eisMode", eisMode);
    }
}
#endif // FEATURE_MODIFY


/******************************************************************************
 *
 ******************************************************************************/
} // namespace P2Feature

android::sp<P2FeatureNode>
P2FeatureNode::
createInstance(ePass2Type const type)
{
    if( type < 0 ||
        type >= PASS2_TYPE_TOTAL )
    {
        MY_LOGE("not supported p2 type %d", type);
        return NULL;
    }
    //
    return new P2Feature::P2NodeImp(type);
}

namespace P2Feature {

/******************************************************************************
 *
 ******************************************************************************/
P2NodeImp::
P2NodeImp(ePass2Type const type)
    : BaseNode()
    , P2FeatureNode()
    //
    , mType(type)
    , mConfigRWLock()
    //
    , mpInAppMeta_Request()
    , mpInHalMeta_P1()
#ifdef FEATURE_MODIFY
    , mpInAppRetMeta_Request()
#endif // FEATURE_MODIFY
    , mpOutAppMeta_Result()
    , mpOutHalMeta_Result()
    //
    , mpvInFullRaw()
    , mpInResizedRaw()
    , mvOutImages()
    , mpOutFd()
    //
    , mOperationLock()
    //
    , mpP2Processor(NULL)
    , mpMdpProcessor(NULL)
    , mBurstNum(0)
{
    char cLogLevel[PROPERTY_VALUE_MAX];
    ::property_get("debug.camera.log", cLogLevel, "0");
    mLogLevel = atoi(cLogLevel);
    if ( mLogLevel == 0 ) {
        ::property_get("debug.camera.log.P2FeatureNode", cLogLevel, "0");
        mLogLevel = atoi(cLogLevel);
    }
#if 1 /*[EP_TEMP]*/ //[FIXME] TempTestOnly
    #warning "[FIXME] force enable P2FeatureNode log"
    if (mLogLevel < 2) {
        mLogLevel = 2;
    }
#endif
}


/******************************************************************************
 *
 ******************************************************************************/
P2NodeImp::
~P2NodeImp()
{
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P2NodeImp::
init(InitParams const& rParams)
{
    CAM_TRACE_NAME("P2F:init");
    FUNC_START;
    //
    mOpenId = rParams.openId;
    mNodeId = rParams.nodeId;
    mNodeName = rParams.nodeName;

    MY_LOGD("OpenId %d, nodeId %d, name %s",
            getOpenId(), getNodeId(), getNodeName() );
    //
    MRect activeArray;
    {
        sp<IMetadataProvider> pMetadataProvider =
            NSMetadataProviderManager::valueFor(getOpenId());
        if( ! pMetadataProvider.get() ) {
            MY_LOGE(" ! pMetadataProvider.get() ");
            return DEAD_OBJECT;
        }
        IMetadata static_meta =
            pMetadataProvider->geMtktStaticCharacteristics();
        if( tryGetMetadata<MRect>(&static_meta,
            MTK_SENSOR_INFO_ACTIVE_ARRAY_REGION, activeArray) ) {
            MY_LOGD_IF(1,"active array(%d, %d, %dx%d)",
                    activeArray.p.x, activeArray.p.y,
                    activeArray.s.w, activeArray.s.h);
        } else {
            MY_LOGE("no static info: MTK_SENSOR_INFO_ACTIVE_ARRAY_REGION");
            #ifdef USING_MTK_LDVT /*[EP_TEMP]*/ //[FIXME] TempTestOnly
            activeArray = MRect(1600, 1200);// hard-code sensor size
            MY_LOGI("set sensor size to active array(%d, %d, %dx%d)",
                activeArray.p.x, activeArray.p.y,
                activeArray.s.w, activeArray.s.h);
            #else
            return UNKNOWN_ERROR;
            #endif
        }
        //
        {
            mpP2Processor  = NULL;
            //
            P2Procedure::InitParams param;
            param.openId      = mOpenId;
            param.type        = mType;
            param.activeArray = activeArray;
            param.enableLog   = mLogLevel >= 1;
            param.enableDumpBuffer = mLogLevel >= 3;
            //
            mpP2Processor = P2Procedure::createProcessor(param);
            if( mpP2Processor == NULL )
                return BAD_VALUE;
        }
        //
        {
            mpMdpProcessor = NULL;
            //
            MDPProcedure::InitParams param;
            param.enableLog   = mLogLevel >= 1;
            //
            mpMdpProcessor = MDPProcedure::createProcessor(param);
            if( mpMdpProcessor == NULL )
                return BAD_VALUE;
        }
        mpP2Processor->setNextProcessor(mpMdpProcessor);
    }
    //
    FUNC_END;
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P2NodeImp::
config(ConfigParams const& rParams)
{
    CAM_TRACE_NAME("P2F:config");
    //
    {
        MERROR const err = verifyConfigParams(rParams);
        if( err != OK ) {
            MY_LOGE("verifyConfigParams failed, err = %d", err);
            return err;
        }
    }
    //
    flush();
    //
    {
        RWLock::AutoWLock _l(mConfigRWLock);
        // meta
        mpInAppMeta_Request  = rParams.pInAppMeta;
        mpInHalMeta_P1       = rParams.pInHalMeta;
#ifdef FEATURE_MODIFY

#ifdef ISO_META
        mpInAppRetMeta_Request = rParams.pInAppRetMeta;
#else
        mpInAppRetMeta_Request = NULL;
#endif // ISO_META
#endif // FEATURE_MODIFY
        mpOutAppMeta_Result  = rParams.pOutAppMeta;
        mpOutHalMeta_Result  = rParams.pOutHalMeta;
        // image
        mpvInFullRaw         = rParams.pvInFullRaw;
        mpInResizedRaw       = rParams.pInResizedRaw;
        mvOutImages          = rParams.vOutImage;
        mpOutFd              = rParams.pOutFDImage;
        // property
        mBurstNum            = rParams.burstNum;
    }
    //
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P2NodeImp::
uninit()
{
    CAM_TRACE_NAME("P2F:uninit");
    FUNC_START;
    //
    if ( OK != flush() )
        MY_LOGE("flush failed");
    //
    if( mpP2Processor.get() ) {
        mpP2Processor->waitForIdle();
        mpP2Processor->close();
        //MY_LOGD("[P2] reference count:%d",mpP2Processor->getStrongCount());
        mpP2Processor = NULL;
    }
    //
    if( mpMdpProcessor.get() ) {
        mpMdpProcessor->waitForIdle();
        mpMdpProcessor->close();
        //MY_LOGD("MDP] reference count:%d",mpMdpProcessor->getStrongCount());
        mpMdpProcessor = NULL;
    }
    //
    FUNC_END;
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P2NodeImp::
flush()
{
    CAM_TRACE_NAME("P2F:flush");
    FUNC_START;
    //
    Mutex::Autolock _l(mOperationLock);
    // 1. wait for P2 thread
    mpP2Processor->flushRequests();
    // 2. wait for MDP thread
    mpMdpProcessor->flushRequests();
    //
    FUNC_END;
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P2NodeImp::
queue(android::sp<IPipelineFrame> pFrame)
{
    FUNC_START;
    //
    if( ! pFrame.get() ) {
        MY_LOGE("Null frame");
        return BAD_VALUE;
    }

    Mutex::Autolock _l(mOperationLock);
    MY_LOGD_IF(mLogLevel >= 2, "queue pass2 @ frame(%d)", pFrame->getFrameNo());

    onProcessFrame(pFrame);

    FUNC_END;
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
P2NodeImp::
onProcessFrame(
    android::sp<IPipelineFrame> const& pFrame
)
{
    //FUNC_START;
    if( mpP2Processor == NULL ) {
        MY_LOGW("may not configured yet");
        BaseNode::flush(pFrame);
        return;
    }
    //
    // map IPipelineFrame to requests
    if( OK != mapToRequests(pFrame) ) {
        MY_LOGW("map to jobs failed");
        BaseNode::flush(pFrame);
        return;
    }
    //
    //FUNC_END;
    return;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P2NodeImp::
verifyConfigParams(
    ConfigParams const & rParams
) const
{
    if  ( ! rParams.pInAppMeta.get() ) {
        MY_LOGE("no in app meta");
        return BAD_VALUE;
    }
    if  ( ! rParams.pInHalMeta.get() ) {
        MY_LOGE("no in hal meta");
        return BAD_VALUE;
    }
    //if  ( ! rParams.pOutAppMeta.get() ) {
    //    return BAD_VALUE;
    //}
    //if  ( ! rParams.pOutHalMeta.get() ) {
    //    return BAD_VALUE;
    //}
    if  (  rParams.pvInFullRaw.size() == 0
            && ! rParams.pInResizedRaw.get() ) {
        MY_LOGE("no in image fullraw or resized raw");
        return BAD_VALUE;
    }
    if  (  0 == rParams.vOutImage.size() && !rParams.pOutFDImage.get() ) {
        MY_LOGE("no out yuv image");
        return BAD_VALUE;
    }
    //
#define dumpStreamIfExist(str, stream)                         \
    do {                                                       \
        MY_LOGD_IF(stream.get(), "%s: id %#"PRIxPTR", %s",     \
                str,                                           \
                stream->getStreamId(), stream->getStreamName() \
               );                                              \
    } while(0)
    //
    dumpStreamIfExist("[meta] in app", rParams.pInAppMeta);
    dumpStreamIfExist("[meta] in hal", rParams.pInHalMeta);
#ifdef FEATURE_MODIFY
    dumpStreamIfExist("[meta] in appRet", rParams.pInAppRetMeta);
#endif // FEATURE_MODIFY
    dumpStreamIfExist("[meta] out app", rParams.pOutAppMeta);
    dumpStreamIfExist("[meta] out hal", rParams.pOutHalMeta);
    for( size_t i = 0; i < rParams.pvInFullRaw.size(); i++ ) {
        dumpStreamIfExist("[img] in full", rParams.pvInFullRaw[i]);
    }
    dumpStreamIfExist("[img] in resized", rParams.pInResizedRaw);
    for( size_t i = 0; i < rParams.vOutImage.size(); i++ ) {
        dumpStreamIfExist("[img] out yuv", rParams.vOutImage[i]);
    }
    dumpStreamIfExist("[img] out fd", rParams.pOutFDImage);
#undef dumpStreamIfExist
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P2NodeImp::
getInfoIOMapSet(
    sp<IPipelineFrame> const& pFrame,
    IPipelineFrame::InfoIOMapSet& rIOMapSet
) const
{
    if( OK != pFrame->queryInfoIOMapSet( getNodeId(), rIOMapSet ) ) {
        MY_LOGE("queryInfoIOMap failed");
        return NAME_NOT_FOUND;
    }
    //
    // do some check
    IPipelineFrame::ImageInfoIOMapSet& imageIOMapSet = rIOMapSet.mImageInfoIOMapSet;
    if( ! imageIOMapSet.size() ) {
        MY_LOGW("no imageIOMap in frame");
        return BAD_VALUE;
    }
    //
    for( size_t i = 0; i < imageIOMapSet.size(); i++ ) {
        IPipelineFrame::ImageInfoIOMap const& imageIOMap = imageIOMapSet[i];
        if( imageIOMap.vIn.size() != 1 || imageIOMap.vOut.size() == 0) {
            MY_LOGE("[img] #%d wrong size vIn %d, vOut %d",
                    i, imageIOMap.vIn.size(), imageIOMap.vOut.size());
            return BAD_VALUE;
        }
        MY_LOGD_IF(mLogLevel >= 1, "frame %zu:[img] #%zu, in %d, out %d",
                pFrame->getFrameNo(), i, imageIOMap.vIn.size(), imageIOMap.vOut.size());
    }
    //
    IPipelineFrame::MetaInfoIOMapSet& metaIOMapSet = rIOMapSet.mMetaInfoIOMapSet;
    if( ! metaIOMapSet.size() ) {
        MY_LOGW("no metaIOMap in frame");
        return BAD_VALUE;
    }
    //
    for( size_t i = 0; i < metaIOMapSet.size(); i++ ) {
        IPipelineFrame::MetaInfoIOMap const& metaIOMap = metaIOMapSet[i];
        if( !mpInAppMeta_Request.get() ||
                0 > metaIOMap.vIn.indexOfKey(mpInAppMeta_Request->getStreamId()) ) {
            MY_LOGE("[meta] no in app");
            return BAD_VALUE;
        }
        if( !mpInHalMeta_P1.get() ||
                0 > metaIOMap.vIn.indexOfKey(mpInHalMeta_P1->getStreamId()) ) {
            MY_LOGE("[meta] no in hal");
            return BAD_VALUE;
        }
        MY_LOGD_IF(mLogLevel >= 2, "frame %zu:[meta] #%zu: in %d, out %d",
                pFrame->getFrameNo(), i, metaIOMap.vIn.size(), metaIOMap.vOut.size());
    }
    //
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MBOOL
P2NodeImp::
isInImageStream(
    StreamId_T const streamId
) const
{
    RWLock::AutoRLock _l(mConfigRWLock);
    //
    if( isFullRawLocked(streamId) || isResizeRawLocked(streamId) )
        return MTRUE;
    //
    MY_LOGD_IF(0, "stream id %p is not in-stream", streamId);
    return MFALSE;
}


/******************************************************************************
 *
 ******************************************************************************/
MBOOL
P2NodeImp::
isInMetaStream(
    StreamId_T const streamId
) const
{
    RWLock::AutoRLock _l(mConfigRWLock);
#ifdef FEATURE_MODIFY
#ifdef ISO_META
    if( isStream(mpInAppRetMeta_Request, streamId) )
    {
        return MTRUE;
    }
#else
    // do nothing
#endif // ISO_META
#endif // FEATURE_MODIFY
    return isStream(mpInAppMeta_Request, streamId) || isStream(mpInHalMeta_P1, streamId);
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P2NodeImp::
acquireImageStream(
    android::sp<IPipelineFrame> const& pFrame,
    StreamId_T const streamId,
    sp<IImageStreamBuffer>& rpStreamBuffer
)
{
    return ensureImageBufferAvailable_(
            pFrame->getFrameNo(),
            streamId,
            pFrame->getStreamBufferSet(),
            rpStreamBuffer
            );
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
P2NodeImp::
releaseImageStream(
    sp<IPipelineFrame> const& pFrame,
    sp<IImageStreamBuffer> const pStreamBuffer,
    MUINT32 const status
) const
{
    IStreamBufferSet& streamBufferSet = pFrame->getStreamBufferSet();
    StreamId_T const streamId = pStreamBuffer->getStreamInfo()->getStreamId();
    //
    if( pStreamBuffer == NULL ) {
        MY_LOGE("pStreamBuffer == NULL");
        return;
    }
    //
    if( ! isInImageStream(streamId) ) {
        pStreamBuffer->markStatus(
                (status != eStreamStatus_FILLED) ?
                STREAM_BUFFER_STATUS::WRITE_ERROR :
                STREAM_BUFFER_STATUS::WRITE_OK
                );
    }
    //
    //  Mark this buffer as USED by this user.
    //  Mark this buffer as RELEASE by this user.
    streamBufferSet.markUserStatus(
            streamId,
            getNodeId(),
            ((status != eStreamStatus_NOT_USED) ? IUsersManager::UserStatus::USED : 0) |
            IUsersManager::UserStatus::RELEASE
            );
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P2NodeImp::
acquireImageBuffer(
    StreamId_T const streamId,
    sp<IImageStreamBuffer> const pStreamBuffer,
    sp<IImageBuffer>& rpImageBuffer
) const
{
    if( pStreamBuffer == NULL ) {
        MY_LOGE("pStreamBuffer == NULL");
        return BAD_VALUE;
    }
    //  Query the group usage.
    MUINT const groupUsage = pStreamBuffer->queryGroupUsage(getNodeId());
    sp<IImageBufferHeap>   pImageBufferHeap =
        isInImageStream(streamId) ?
        pStreamBuffer->tryReadLock(getNodeName()) :
        pStreamBuffer->tryWriteLock(getNodeName());

    if (pImageBufferHeap == NULL) {
        MY_LOGE("[node:%d][stream buffer:%s] cannot get ImageBufferHeap",
                getNodeId(), pStreamBuffer->getName());
        return BAD_VALUE;
    }

    rpImageBuffer = pImageBufferHeap->createImageBuffer();
    if (rpImageBuffer == NULL) {
        MY_LOGE("[node:%d][stream buffer:%s] cannot create ImageBuffer",
                getNodeId(), pStreamBuffer->getName());
        return BAD_VALUE;
    }
    rpImageBuffer->lockBuf(getNodeName(), groupUsage);

    MY_LOGD_IF(mLogLevel >= 1, "stream %#"PRIxPTR": buffer: %p, usage: %p",
        streamId, rpImageBuffer.get(), groupUsage);

    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
P2NodeImp::
releaseImageBuffer(
    sp<IImageStreamBuffer> const pStreamBuffer,
    sp<IImageBuffer> const pImageBuffer
) const
{
    if( pStreamBuffer == NULL || pImageBuffer == NULL ) {
        MY_LOGE("pStreamBuffer %p, pImageBuffer %p should not be NULL");
        return;
    }
    //
    MY_LOGD_IF(1, "track: release buffer 1");
    pImageBuffer->unlockBuf(getNodeName());
    MY_LOGD_IF(1, "track: release buffer 2");
    pStreamBuffer->unlock(getNodeName(), pImageBuffer->getImageBufferHeap());
    MY_LOGD_IF(1, "track: release buffer 3");
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P2NodeImp::
acquireMetaStream(
    android::sp<IPipelineFrame> const& pFrame,
    StreamId_T const streamId,
    sp<IMetaStreamBuffer>& rpStreamBuffer
)
{
    return ensureMetaBufferAvailable_(
            pFrame->getFrameNo(),
            streamId,
            pFrame->getStreamBufferSet(),
            rpStreamBuffer
            );
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
P2NodeImp::
releaseMetaStream(
    android::sp<IPipelineFrame> const& pFrame,
    sp<IMetaStreamBuffer> const pStreamBuffer,
    MUINT32 const status
) const
{
    IStreamBufferSet&     rStreamBufferSet = pFrame->getStreamBufferSet();
    StreamId_T const streamId = pStreamBuffer->getStreamInfo()->getStreamId();
    //
    if( pStreamBuffer.get() == NULL ) {
        MY_LOGE("StreamId %d: pStreamBuffer == NULL",
                streamId);
        return;
    }
    //
    //Buffer Producer must set this status.
    if( ! isInMetaStream(streamId) ) {
        pStreamBuffer->markStatus(
                (status != eStreamStatus_FILLED) ?
                STREAM_BUFFER_STATUS::WRITE_ERROR :
                STREAM_BUFFER_STATUS::WRITE_OK
                );
    }
    //
    //  Mark this buffer as USED by this user.
    //  Mark this buffer as RELEASE by this user.
    rStreamBufferSet.markUserStatus(
            streamId, getNodeId(),
            ((status != eStreamStatus_NOT_USED) ? IUsersManager::UserStatus::USED : 0) |
            IUsersManager::UserStatus::RELEASE
            );
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P2NodeImp::
acquireMetadata(
    StreamId_T const streamId,
    sp<IMetaStreamBuffer> const pStreamBuffer,
    IMetadata*& rpMetadata
) const
{
    rpMetadata = isInMetaStream(streamId) ?
        pStreamBuffer->tryReadLock(getNodeName()) :
        pStreamBuffer->tryWriteLock(getNodeName());

    if( rpMetadata == NULL ) {
        MY_LOGE("[node:%d][stream buffer:%s] cannot get metadata",
                getNodeId(), pStreamBuffer->getName());
        return BAD_VALUE;
    }

    MY_LOGD_IF(0,"stream %#"PRIxPTR": stream buffer %p, metadata: %p",
        streamId, pStreamBuffer.get(), rpMetadata);

    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
P2NodeImp::
releaseMetadata(
    sp<IMetaStreamBuffer> const pStreamBuffer,
    IMetadata* const pMetadata
) const
{
    if( pMetadata == NULL ) {
        MY_LOGW("pMetadata == NULL");
        return;
    }
    pStreamBuffer->unlock(getNodeName(), pMetadata);
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
P2NodeImp::
onPartialFrameDone(
    sp<IPipelineFrame> const& pFrame
)
{
    CAM_TRACE_NAME("P2F:PartialFrameDone");
    //FUNC_START;
    MY_LOGD_IF(1, "track: applyRelease+");
    IStreamBufferSet&     rStreamBufferSet = pFrame->getStreamBufferSet();
    rStreamBufferSet.applyRelease(getNodeId());
    MY_LOGD_IF(1, "track: applyRelease-");
    //FUNC_END;
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
P2NodeImp::
onFrameDone(
    sp<IPipelineFrame> const& pFrame
)
{
    CAM_TRACE_NAME("P2F:FrameDone");
    //MY_LOGD("frame %u done", pFrame->getFrameNo());
    onDispatchFrame(pFrame);
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P2NodeImp::
mapToRequests(
    android::sp<IPipelineFrame> const& pFrame
)
{
    //
    // 1. get IOMap
    IPipelineFrame::InfoIOMapSet IOMapSet;
    if( OK != getInfoIOMapSet(pFrame, IOMapSet) ) {
        MY_LOGE("queryInfoIOMap failed");
        return BAD_VALUE;
    }
    //
    // 2. create metadata handle (based on IOMap)
    sp<MetaHandle> pMeta_InApp  = mpInAppMeta_Request.get() ?
        MetaHandle::create(this, pFrame, mpInAppMeta_Request->getStreamId()) : NULL;
    sp<MetaHandle> pMeta_InHal  = mpInHalMeta_P1.get() ?
        MetaHandle::create(this, pFrame, mpInHalMeta_P1->getStreamId()) : NULL;
#ifdef FEATURE_MODIFY
#ifdef ISO_META
    sp<MetaHandle> pMeta_InRetApp  = mpInAppRetMeta_Request.get() ?
        MetaHandle::create(this, pFrame, mpInAppRetMeta_Request->getStreamId()) : NULL;
#else
    sp<MetaHandle> pMeta_InRetApp  = NULL;
#endif // ISO_META
#endif // FEATURE_MODIFY

    //
#ifdef FEATURE_MODIFY
    if( pMeta_InApp  == NULL ||
        pMeta_InHal  == NULL ||
#ifdef ISO_META
        pMeta_InRetApp == NULL )
#else
        0)
#endif // ISO_META
#endif // FEATURE_MODIFY
    {
        MY_LOGW("meta check failed");
        return BAD_VALUE;
    }
    // 3. create FrameLifeHolder
    sp<FrameLifeHolder> pFrameLife = new FrameLifeHolder(this, pFrame, mLogLevel >= 1);
    // 4. process image IO
    IPipelineFrame::ImageInfoIOMapSet& imageIOMapSet = IOMapSet.mImageInfoIOMapSet;
    for( size_t run_idx = 0 ; run_idx < imageIOMapSet.size(); run_idx++ )
    {
        IPipelineFrame::ImageInfoIOMap const& imageIOMap = imageIOMapSet[run_idx];
        sp<Request> pRequest = new Request(pFrameLife);
        pRequest->frameNo = pFrame->getFrameNo();
        pRequest->index = run_idx;
        pRequest->context.fd_stream_id = mpOutFd.get() ? mpOutFd->getStreamId() : 0;

        if (mLogLevel >= 2) {
            MY_LOGD("[StreamID] run_idx(%d) vIn.size(%d) +++",
                run_idx, imageIOMap.vIn.size());
            for (int i = 0; i < imageIOMap.vIn.size(); i++) {
                StreamId_T const sId = imageIOMap.vIn.keyAt(i);
                MY_LOGD("[StreamID] In(%d) sId(0x%x) F(%d) R(%d)",
                    i, sId, isFullRawLocked(sId), isResizeRawLocked(sId));
            }
            MY_LOGD("[StreamID] run_idx(%d) vIn.size(%d) ---",
                run_idx, imageIOMap.vIn.size());
        }
        // source
        StreamId_T const streamId = imageIOMap.vIn.keyAt(0);
        pRequest->context.in_buffer = StreamBufferHandle::create(this, pFrame, streamId);
        //
        {
            RWLock::AutoRLock _l(mConfigRWLock);
            pRequest->context.resized = isResizeRawLocked(streamId);
        }
        // determine whether burst or not
#if FORCE_BURST_ON
        pRequest->context.burst_num = pRequest->context.resized ? 4 : 0;
#else
        pRequest->context.burst_num = pRequest->context.resized ? mBurstNum : 0;
#endif
        // destination
        for( size_t i = 0; i < imageIOMap.vOut.size(); i++ )
        {
            StreamId_T const streamId = imageIOMap.vOut.keyAt(i);
            //MUINT32 const transform = imageIOMap.vOut.valueAt(i)->getTransform();
            pRequest->context.out_buffers.push_back(StreamBufferHandle::create(this, pFrame, streamId));
        }
        //
        pRequest->context.in_app_meta = pMeta_InApp;
#ifdef FEATURE_MODIFY
#ifdef ISO_META
        pRequest->context.in_ret_app_meta = pMeta_InRetApp;
#else
        pRequest->context.in_ret_app_meta = NULL;
#endif
#endif // FEATURE_MODIFY
        pRequest->context.in_hal_meta = pMeta_InHal;
        if (run_idx == 0)
        {
            pRequest->context.out_app_meta = mpOutAppMeta_Result.get() ?
                MetaHandle::create(this, pFrame, mpOutAppMeta_Result->getStreamId()) : NULL;
            pRequest->context.out_hal_meta= mpOutHalMeta_Result.get() ?
                MetaHandle::create(this, pFrame, mpOutHalMeta_Result->getStreamId()) : NULL;
        }
        // prevent HAL1 that a frame have no buffer from dispatching faster than this scope
        if (run_idx == imageIOMapSet.size() - 1)
        {
            pMeta_InApp.clear();
            pMeta_InHal.clear();
        }
        mpP2Processor->queueRequest(pRequest);
        //
    }
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P2Procedure::
mapPortId(
    StreamId_T const streamId, // [in]
    MUINT32 const transform,   // [in]
    MBOOL const isFdStream,    // [in]
    MUINT32& rOccupied,        // [in/out]
    PortID&  rPortId           // [out]
) const
{
    MERROR ret = OK;
#define PORT_WDMAO_USED  (0x1)
#define PORT_WROTO_USED  (0x2)
#define PORT_IMG2O_USED  (0x4)
    if( transform != 0 ) {
        if( !(rOccupied & PORT_WROTO_USED) ) {
            rPortId = PORT_WROTO;
            rOccupied |= PORT_WROTO_USED;
        }
        else
            ret = INVALID_OPERATION;
    }
    else {
#if FD_PORT_SUPPORT
        if( FD_PORT_SUPPORT && isFdStream ) {
            if( rOccupied & PORT_IMG2O_USED ) {
                MY_LOGW("should not be occupied");
                ret = INVALID_OPERATION;
            } else {
                rOccupied |= PORT_IMG2O_USED;
                rPortId = PORT_IMG2O;
            }
        } else
#endif
        if( !(rOccupied & PORT_WDMAO_USED) ) {
            rOccupied |= PORT_WDMAO_USED;
            rPortId = PORT_WDMAO;
        } else if( !(rOccupied & PORT_WROTO_USED) ) {
            rOccupied |= PORT_WROTO_USED;
            rPortId = PORT_WROTO;
        } else
            ret = INVALID_OPERATION;
    }
    MY_LOGD_IF(0, "stream id %#"PRIxPTR", occupied %p",
            streamId, rOccupied);
    return ret;
#undef PORT_WDMAO_USED
#undef PORT_WROTO_USED
#undef PORT_IMG2O_USED
}

/******************************************************************************
 *
 ******************************************************************************/
sp<MetaHandle>
MetaHandle::
create(
    StreamControl* const pCtrl,
    sp<IPipelineFrame> const& pFrame,
    StreamId_T const streamId
)
{
    // check StreamBuffer here
    sp<IMetaStreamBuffer> pStreamBuffer = NULL;
    if( pCtrl && OK == pCtrl->acquireMetaStream(
                pFrame,
                streamId,
                pStreamBuffer) )
    {
        IMetadata* pMeta = NULL;
        if( OK == pCtrl->acquireMetadata(
                    streamId,
                    pStreamBuffer,
                    pMeta
                    ) )
        {
            BufferState_t const init_state =
                pCtrl->isInMetaStream(streamId) ? STATE_READABLE : STATE_WRITABLE;
            return new MetaHandle(
                    pCtrl,
                    pFrame,
                    streamId,
                    pStreamBuffer,
                    init_state,
                    pMeta
                    );
        }
        else {
            pCtrl->releaseMetaStream(pFrame, pStreamBuffer, StreamControl::eStreamStatus_NOT_USED);
        }
    }
    //
    return NULL;
}


/******************************************************************************
 *
 ******************************************************************************/
MetaHandle::
~MetaHandle()
{
    if( muState != STATE_NOT_USED )
    {
        const MUINT32 status = (muState != STATE_WRITE_FAIL) ?
            StreamControl::eStreamStatus_FILLED : StreamControl::eStreamStatus_ERROR;
        //
        mpStreamCtrl->releaseMetadata(mpStreamBuffer, mpMetadata);
        mpStreamCtrl->releaseMetaStream(mpFrame, mpStreamBuffer, status);
    }
    else
    {
        mpStreamCtrl->releaseMetaStream(mpFrame, mpStreamBuffer, StreamControl::eStreamStatus_NOT_USED);
    }
    //MY_LOGD("release meta[%d] streamId[0x%x]",mpFrame->getFrameNo() ,mStreamId);
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
MetaHandle::
updateState(BufferState_t const state)
{
    Mutex::Autolock _l(mLock);
    if( muState == STATE_NOT_USED ) {
        MY_LOGW("streamId %#"PRIxPTR" state %d -> %d",
            mStreamId, muState, state);
    }
    else {
        MY_LOGW_IF(state == STATE_WRITE_FAIL, "streamId %#"PRIxPTR" set fail, state %d -> %d",
                mStreamId, muState, state);
        muState = state;
    }
    //mCond.broadcast();
}


/******************************************************************************
 *
 ******************************************************************************/
sp<BufferHandle>
StreamBufferHandle::
create(
    StreamControl* const pCtrl,
    sp<IPipelineFrame> const& pFrame,
    StreamId_T const streamId
)
{
    // check StreamBuffer here
    sp<IImageStreamBuffer> pStreamBuffer = NULL;
    if( OK == pCtrl->acquireImageStream(
                pFrame,
                streamId,
                pStreamBuffer) )
    {
        MUINT32 uTransform = pStreamBuffer->getStreamInfo()->getTransform();
        MUINT32 uUsage = pStreamBuffer->getStreamInfo()->getUsageForAllocator();
        MY_LOGD_IF(0, "create buffer handler, stream:0x%x, transform:%d, usage:%d",
            streamId, uTransform, uUsage);
        return new StreamBufferHandle(
                pCtrl, pFrame,
                streamId, pStreamBuffer,
                uTransform, uUsage);
    }
    //
    return NULL;
}


/******************************************************************************
 *
 ******************************************************************************/
StreamBufferHandle::
~StreamBufferHandle()
{
    if( muState != STATE_NOT_USED )
    {
        const MUINT32 status = (muState != STATE_WRITE_FAIL) ?
            StreamControl::eStreamStatus_FILLED : StreamControl::eStreamStatus_ERROR;
        //
        mpStreamCtrl->releaseImageBuffer(mpStreamBuffer, mpImageBuffer);
        mpStreamCtrl->releaseImageStream(mpFrame, mpStreamBuffer, status);
    }
    else
    {
        mpStreamCtrl->releaseImageStream(mpFrame, mpStreamBuffer, StreamControl::eStreamStatus_NOT_USED);
    }
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
StreamBufferHandle::
waitState(
    BufferState_t const state,
    nsecs_t const nsTimeout
)
{
    Mutex::Autolock _l(mLock);
    if( mpImageBuffer == NULL ) {
        // get buffer from streambuffer
        const MERROR ret = mpStreamCtrl->acquireImageBuffer(mStreamId, mpStreamBuffer, mpImageBuffer);
        // update initial state
        if( ret == OK )
            muState = mpStreamCtrl->isInImageStream(mStreamId) ? STATE_READABLE : STATE_WRITABLE;
        //return ret;
    }
    //
    if( muState != state ) {
        mCond.waitRelative(mLock, nsTimeout);
    }
    return (muState == state) ? OK : TIMED_OUT;
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
StreamBufferHandle::
updateState(BufferState_t const state)
{
    Mutex::Autolock _l(mLock);
    if( muState == STATE_NOT_USED ) {
        MY_LOGW("streamId %#"PRIxPTR" state %d -> %d",
            mStreamId, muState, state);
    }
    else {
        MY_LOGW_IF(state == STATE_WRITE_FAIL, "streamId %#"PRIxPTR" set fail: state %d -> %d",
                mStreamId, muState, state);
        muState = state;
    }
    mCond.broadcast();
}


/******************************************************************************
 *
 ******************************************************************************/
sp<Processor>
P2Procedure::
createProcessor(InitParams& params)
{
    CAM_TRACE_NAME("P2F:[Proc]createProcessor");
    IStreamingFeaturePipe* pPipe = NULL;
    IHal3A_T* p3A = NULL;
    ENormalStreamTag streamtag = ENormalStreamTag_Prv;
    if( params.type == P2FeatureNode::PASS2_STREAM ) {
        streamtag = ENormalStreamTag_Prv;
    }
    else if ( params.type == P2FeatureNode::PASS2_TIMESHARING ) {
        streamtag = ENormalStreamTag_Vss;
    }
    else {
        MY_LOGE("not supported type %d", params.type);
        goto lbExit;
    }
    //
    CAM_TRACE_BEGIN("P2F:[Proc]NormalStream create");
    pPipe = IStreamingFeaturePipe::createInstance(params.openId);
    //
    if( pPipe == NULL ) {
        MY_LOGE("create pipe failed");
        CAM_TRACE_END();
        goto lbExit;
    }
    //
    CAM_TRACE_BEGIN("P2F:[Proc]NormalStream init");
    if( ! pPipe->init(LOG_TAG) )
    {
        CAM_TRACE_END();
        MY_LOGE("pipe init failed");
        goto lbExit;
    }
    CAM_TRACE_END();
    //
    #if SUPPORT_3A
    CAM_TRACE_BEGIN("P2F:[Proc]3A create");
    p3A = IHal3AImp_T::createInstance(IHal3A::E_Camera_3, params.openId, LOG_TAG);
    CAM_TRACE_END();
    #endif
    if( p3A == NULL ) {
        MY_LOGE("create 3A failed");
        goto lbExit;
    }
    MY_LOGD("create processor type %d: pipe %p, 3A %p",
            params.type, pPipe, p3A);
    //
lbExit:
    if( ! pPipe || !p3A ) {
        if( pPipe ) {
            pPipe->uninit(LOG_TAG);
            pPipe->destroyInstance();
            pPipe = NULL;
        }
        if( p3A ) {
            p3A->destroyInstance(LOG_TAG);
            p3A = NULL;
        }
    }

    params.pPipe        = pPipe;
    params.p3A          = p3A;
    return pPipe ? new ProcessorBase<P2Procedure>(params, PROCESSOR_NAME_P2) : NULL;
}


/******************************************************************************
 *
 ******************************************************************************/
P2Procedure::
~P2Procedure()
{
    MY_LOGD("destroy processor %d: %p",
            mInitParams.type, mpPipe);
    //
    if( mpPipe ) {
        if( ! mpPipe->uninit(LOG_TAG) ) {
            MY_LOGE("pipe uninit failed");
        }
        mpPipe->destroyInstance();
    }

    if( mp3A ) {
        mp3A->destroyInstance(LOG_TAG);
    }

    if( mpMultiFrameHandler ) {
        delete mpMultiFrameHandler;
    }
}


/******************************************************************************
 *
 ******************************************************************************/
#ifdef FEATURE_MODIFY
static sp<Request> getP2Request(FeaturePipeParam &param)
{
    return param.getVar<sp<Request> >(VAR_P2_REQUEST, NULL);
}

static P2Procedure* getP2Procedure(FeaturePipeParam &param)
{
    P2Procedure* proc = reinterpret_cast<P2Procedure*>(param.mQParams.mpCookie);
    return proc;
}

static MVOID releaseP2ObjectReference(FeaturePipeParam &param)
{
    // release life cycle
    sp<Request> dummy;
    param.setVar<sp<Request> >(VAR_P2_REQUEST, dummy);
}

MVOID P2Procedure::prepareFeaturePipeParam(FeaturePipeParam &featureEnqueParams,
              const sp<Request> &pRequest,
              IMetadata *pMeta_InApp,
              IMetadata *pMeta_InRetApp,
              IMetadata *pMeta_InHal,
              IMetadata *pMeta_OutApp,
              IMetadata *pMeta_OutHal,
              const crop_infos &cropInfos)
{
    featureEnqueParams.setVar<sp<Request> >(VAR_P2_REQUEST, pRequest);

    featureEnqueParams.setFeatureMask(MASK_3DNR, isAPEnabled_3DNR(pMeta_InApp));
    featureEnqueParams.setFeatureMask(MASK_VHDR, isHALenabled_VHDR(pMeta_InHal));
    featureEnqueParams.setFeatureMask(MASK_EIS, isAPEnabled_EIS(pMeta_InApp));
    featureEnqueParams.setFeatureMask(MASK_VFB, isAPEnabled_VFB(pMeta_InApp));
    featureEnqueParams.setFeatureMask(MASK_VFB_EX, isAPEnabled_VFB_EX(pMeta_InApp));

    if (HAS_EIS(featureEnqueParams.mFeatureMask))
    {
        MINT32 eisMode = 0;
        IMetadata::IEntry entry = pMeta_InHal->entryFor(MTK_EIS_MODE);
        if (entry.count() > 0)
        {
            eisMode = entry.itemAt(0, Type2Type<MINT32>());
            MY_LOGD("eisMode: %d",eisMode);

            if (EIS_MODE_IS_EIS_22_ENABLED(eisMode))
            {
                //Keep EIS enabled
            }else
            {
                //Turn on EIS 1.2 only
                DISABLE_EIS(featureEnqueParams.mFeatureMask);
            }
        }else
        {
            //Turn on EIS 1.2 only
            DISABLE_EIS(featureEnqueParams.mFeatureMask);
        }
    }

    if (HAS_VHDR(featureEnqueParams.mFeatureMask))
    {
        if (HAS_VFB(featureEnqueParams.mFeatureMask) || HAS_VFB_EX(featureEnqueParams.mFeatureMask))
        {
            MY_LOGE("z/i vHDR could NOT be with vFB&vFB_EX");
            DISABLE_VFB(featureEnqueParams.mFeatureMask);
            DISABLE_VFB_EX(featureEnqueParams.mFeatureMask);
        }
    }

    if (HAS_VFB(featureEnqueParams.mFeatureMask) || HAS_VFB_EX(featureEnqueParams.mFeatureMask))
    {
        //If vFBx is enabled, turn on EIS 1.2 only
        DISABLE_EIS(featureEnqueParams.mFeatureMask);
    }


    prepareFeatureData_3DNR(
        featureEnqueParams,
        cropInfos.dstsize_resizer.w, cropInfos.dstsize_resizer.h,
        pMeta_InApp,  pMeta_InRetApp, pMeta_OutApp, pMeta_InHal, pMeta_OutHal
    );
    prepareFeatureData_VHDR(featureEnqueParams, pMeta_InHal);
    prepareFeatureData_VFB(featureEnqueParams);
    prepareFeatureData_EIS(featureEnqueParams, pMeta_InHal);
}
MBOOL P2Procedure::setP2B3A(FeaturePipeParam &param)
{
    MBOOL ret = MFALSE;
    void *pTuning;
    if( param.tryGetVar<void*>("p2b_3a_tuning", pTuning) )
    {
        sp<Request> request;
        request = getP2Request(param);
        if( request != NULL )
        {
            IMetadata *appInMeta = request->context.in_app_meta->getMetadata();
            IMetadata *halInMeta = request->context.in_hal_meta->getMetadata();
            if( halInMeta && appInMeta )
            {
                TuningParam rTuningParam = {pTuning, NULL};
                MetaSet_T inMetaSet;
                inMetaSet.appMeta = *halInMeta;
                inMetaSet.halMeta = *appInMeta;
                trySetMetadata<MUINT8>(halInMeta, MTK_3A_ISP_PROFILE, EIspProfile_VFB_PostProc);
                mp3A->setIsp(0, inMetaSet, &rTuningParam, NULL/*outMetaSet*/);
                ret = MTRUE;
            }
        }
    }
    return ret;
}

MBOOL P2Procedure::featurePipeCB(FeaturePipeParam::MSG_TYPE msg, FeaturePipeParam &param)
{
    MBOOL ret = MFALSE;

    if( msg == FeaturePipeParam::MSG_FRAME_DONE )
    {
        // release sp<Request> so P2Processor can free it
        releaseP2ObjectReference(param);
        param.mQParams.mpfnCallback(param.mQParams);
        ret = MTRUE;
    }
    else if( msg == FeaturePipeParam::MSG_DISPLAY_DONE )
    {
        // eis partial result
    }
    else if( msg == FeaturePipeParam::MSG_FD_DONE )
    {
        // FD
    }
    else if( msg == FeaturePipeParam::MSG_P2B_SET_3A )
    {
        P2Procedure *proc = getP2Procedure(param);
        if( proc )
        {
            ret = proc->setP2B3A(param);
        }
    }

    return ret;
}

#endif // FEATURE_MODIFY


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P2Procedure::
onExecute(
    sp<Request> const pRequest,
    FrameParams const& params
)
{
    CAM_TRACE_NAME("P2F:[Proc]exe");
    //
    MERROR ret = OK;
    //
    if ( OK != (ret = checkParams(params)) )
        return ret;
    // prepare metadata
    IMetadata* pMeta_InApp  = params.inApp->getMetadata();
#ifdef FEATURE_MODIFY
#ifdef ISO_META
    IMetadata* pMeta_InRetApp  = params.inRetApp->getMetadata();
#else
    IMetadata* pMeta_InRetApp  = NULL;
#endif // ISO_META
#endif // FEATURE_MODIFY
    IMetadata* pMeta_InHal  = params.inHal->getMetadata();
    IMetadata* pMeta_OutApp = params.outApp.get() ? params.outApp->getMetadata() : NULL;
    IMetadata* pMeta_OutHal = params.outHal.get() ? params.outHal->getMetadata() : NULL;
    //
    if( pMeta_InApp == NULL || pMeta_InHal == NULL ) {
        MY_LOGE("meta: in app %p, in hal %p", pMeta_InApp, pMeta_InHal);
        return BAD_VALUE;
    }
    //
    crop_infos cropInfos;
    if( OK != (ret = getCropInfos(pMeta_InApp, pMeta_InHal, params.bResized, cropInfos)) ) {
        MY_LOGE("getCropInfos failed");
        return ret;
    }
    //
    QParams enqueParams;
    //frame tag
    enqueParams.mvStreamTag.push_back(
                        NSCam::NSIoPipe::NSPostProc::ENormalStreamTag_Normal);
    //
    // input
    {
        if( OK != (ret = params.in.mHandle->waitState(BufferHandle::STATE_READABLE)) ) {
            MY_LOGW("src buffer err = %d", ret);
            return BAD_VALUE;
        }
        IImageBuffer* pSrc = params.in.mHandle->getBuffer();
        //
        Input src;
        src.mPortID       = params.in.mPortId;
        src.mPortID.group = 0;
        src.mBuffer       = pSrc;
        // update src size
        if( params.bResized )
            pSrc->setExtParam(cropInfos.dstsize_resizer);
        //
        enqueParams.mvIn.push_back(src);
        MY_LOGD_IF(mbEnableLog, "P2FeatureNode EnQ Src mPortID.index(%d) Fmt(0x%x) "
            "Size(%dx%d)", src.mPortID.index, src.mBuffer->getImgFormat(),
            src.mBuffer->getImgSize().w, src.mBuffer->getImgSize().h);
    }
    //
    // output
    for( size_t i = 0; i < params.vOut.size(); i++ )
    {
        if( params.vOut[i].mHandle == NULL ||
            OK != (ret = params.vOut[i].mHandle->waitState(BufferHandle::STATE_WRITABLE)) ) {
            MY_LOGW("dst buffer err = %d", ret);
            continue;
        }
        IImageBuffer* pDst = params.vOut[i].mHandle->getBuffer();
        //
        Output dst;
        dst.mPortID       = params.vOut[i].mPortId;
        dst.mPortID.group = 0;
        MUINT32 const uUsage = params.vOut[i].mUsage;
        dst.mPortID.capbility   = (NSIoPipe::EPortCapbility)(
            (uUsage & GRALLOC_USAGE_HW_COMPOSER) ? EPortCapbility_Disp :
            (uUsage & GRALLOC_USAGE_HW_VIDEO_ENCODER) ? EPortCapbility_Rcrd :
            EPortCapbility_None);
        dst.mBuffer       = pDst;
        dst.mTransform    = params.vOut[i].mTransform;
        //
        enqueParams.mvOut.push_back(dst);
    }

    if( enqueParams.mvOut.size() == 0 ) {
        MY_LOGW("no dst buffer");
        return BAD_VALUE;
    }

    {
        TuningParam rTuningParam = {NULL, NULL};
        void* pTuning = NULL;
        unsigned int tuningsize = sizeof(dip_x_reg_t);
        pTuning = ::malloc(tuningsize);
        if (pTuning == NULL) {
            MY_LOGE("alloc tuning buffer fail");
            return NO_MEMORY;
        }
        rTuningParam.pRegBuf = pTuning;
        MY_LOGI("pass2 setIsp malloc %p : %d", pTuning, tuningsize);
        //
        MetaSet_T inMetaSet;
        MetaSet_T outMetaSet;
        //
        inMetaSet.appMeta = *pMeta_InApp;
        inMetaSet.halMeta = *pMeta_InHal;
        //
        MBOOL const bGetResult = (pMeta_OutApp || pMeta_OutHal);
        //
        if( params.bResized ) {
            trySetMetadata<MUINT8>(&(inMetaSet.halMeta), MTK_3A_PGN_ENABLE, 0);
        } else {
            trySetMetadata<MUINT8>(&(inMetaSet.halMeta), MTK_3A_PGN_ENABLE, 1);
        }
        if( pMeta_OutHal ) {
            // FIX ME: getDebugInfo() @ setIsp() should be modified
            //outMetaSet.halMeta = *pMeta_InHal;
        }
        //
        if (mp3A) {
            MY_LOGD_IF(mbEnableLog, "P2 setIsp %p : %d", pTuning, tuningsize);
            if (0 > mp3A->setIsp(0, inMetaSet, &rTuningParam,
                                    (bGetResult ? &outMetaSet : NULL))) {
                MY_LOGW("P2 setIsp - skip tuning pushing");
                if (pTuning != NULL) {
                    MY_LOGI("P2 setIsp free %p : %d", pTuning, tuningsize);
                    ::free(pTuning);
                }
            } else {
                enqueParams.mvTuningData.push_back(pTuning);
                //
                IImageBuffer* pSrc = static_cast<IImageBuffer*>(rTuningParam.pLsc2Buf);
                if (pSrc != NULL) {
                    Input src;
                    src.mPortID       = PORT_DEPI;
                    src.mPortID.group = 0;
                    src.mBuffer       = pSrc;
                    //
                    enqueParams.mvIn.push_back(src);
                    MY_LOGD_IF(mbEnableLog, "P2Node EnQ Src mPortID.index(%d) Fmt(0x%x) "
                        "Size(%dx%d)", src.mPortID.index, src.mBuffer->getImgFormat(),
                        src.mBuffer->getImgSize().w, src.mBuffer->getImgSize().h);
                }
            }
        } else {
            MY_LOGI("P2 setIsp clear tuning %p : %d", pTuning, tuningsize);
            ::memset((unsigned char*)(pTuning), 0, tuningsize);
        }
        //
        if( pMeta_OutApp ) {
            *pMeta_OutApp = outMetaSet.appMeta;
            //
            MRect cropRegion = cropInfos.crop_a;
            if( cropInfos.isEisEabled ) {
                cropRegion.p.x += cropInfos.eis_mv_a.p.x;
                cropRegion.p.y += cropInfos.eis_mv_a.p.y;
            }
            //
            updateCropRegion(cropRegion, pMeta_OutApp);
        }
        //
        if( pMeta_OutHal ) {
            *pMeta_OutHal = *pMeta_InHal;
            *pMeta_OutHal += outMetaSet.halMeta;
        }
    }

#ifdef FEATURE_MODIFY
    FeaturePipeParam featureEnqueParams(featurePipeCB);
    prepareFeaturePipeParam(featureEnqueParams, pRequest,
                            pMeta_InApp, pMeta_InRetApp, pMeta_InHal,
                            pMeta_OutApp, pMeta_OutHal,
                            cropInfos);
    featureEnqueParams.setQParams(enqueParams);

#ifdef ISO_META
    // release retApp metadata once it is finished using
    sp<MetaHandle> tmpHandle = params.inRetApp;
    tmpHandle.clear();
    pRequest->context.in_ret_app_meta.clear();
#endif // ISO_META
#endif // FEATURE_MODIFY


    // for output group crop
    {
        Vector<Output>::const_iterator iter = enqueParams.mvOut.begin();
        while( iter != enqueParams.mvOut.end() ) {
            MCrpRsInfo crop;
            MUINT32 const uPortIndex = iter->mPortID.index;
#ifdef FEATURE_MODIFY
            MCrpRsInfo crop_NoEIS;
#endif
            if( uPortIndex == PORT_WDMAO.index ) {
                crop.mGroupID = 2;
                calcCrop_viewangle(cropInfos, iter->mBuffer->getImgSize(), crop.mCropRect);
#ifdef FEATURE_MODIFY
                if (HAS_EIS(featureEnqueParams.mFeatureMask))
                {
                    crop_NoEIS.mGroupID = 12;
                    getCropInfos_NoEIS(pMeta_InApp, pMeta_InHal, params.bResized, iter->mBuffer->getImgSize(), crop_NoEIS.mCropRect);
                    MY_LOGD("P2FeatureNode EnQ out NoEIS Crop G(%d) S(%d,%d)(%d,%d)(%dx%d) D(%dx%d)",
                             crop_NoEIS.mGroupID,
                             crop_NoEIS.mCropRect.p_integral.x, crop_NoEIS.mCropRect.p_integral.y,
                             crop_NoEIS.mCropRect.p_fractional.x, crop_NoEIS.mCropRect.p_fractional.y,
                             crop_NoEIS.mCropRect.s.w, crop_NoEIS.mCropRect.s.h,
                             crop_NoEIS.mResizeDst.w, crop_NoEIS.mResizeDst.h);
                }
#endif
            } else if ( uPortIndex == PORT_WROTO.index ) {
                crop.mGroupID = 3;
                IImageBuffer* pBuf      = iter->mBuffer;
                MINT32 const transform  = iter->mTransform;
                MSize dstSize = ( transform & eTransform_ROT_90 )
                                ? MSize(pBuf->getImgSize().h, pBuf->getImgSize().w)
                                : pBuf->getImgSize();
                calcCrop_viewangle(cropInfos, dstSize, crop.mCropRect);
#ifdef FEATURE_MODIFY
                if (HAS_EIS(featureEnqueParams.mFeatureMask))
                {
                    crop_NoEIS.mGroupID = 13;
                    getCropInfos_NoEIS(pMeta_InApp, pMeta_InHal, params.bResized, dstSize, crop_NoEIS.mCropRect);
                    MY_LOGD("P2FeatureNode EnQ out NoEIS Crop G(%d) S(%d,%d)(%d,%d)(%dx%d) D(%dx%d)",
                             crop_NoEIS.mGroupID,
                             crop_NoEIS.mCropRect.p_integral.x, crop_NoEIS.mCropRect.p_integral.y,
                             crop_NoEIS.mCropRect.p_fractional.x, crop_NoEIS.mCropRect.p_fractional.y,
                             crop_NoEIS.mCropRect.s.w, crop_NoEIS.mCropRect.s.h,
                             crop_NoEIS.mResizeDst.w, crop_NoEIS.mResizeDst.h);
                }
#endif
#if FD_PORT_SUPPORT
            } else if ( uPortIndex == PORT_IMG2O.index ) {
                crop.mGroupID = 1;
                calcCrop_viewangle(cropInfos, iter->mBuffer->getImgSize(), crop.mCropRect);
#endif
            } else {
                MY_LOGE("not supported output port %p", iter->mPortID);
                return BAD_VALUE;
            }
            crop.mResizeDst = iter->mBuffer->getImgSize();
            MY_LOGI("P2FeatureNode EnQ out 00 Crop G(%d) S(%d,%d)(%d,%d)(%dx%d) D(%dx%d)",
                crop.mGroupID,
                crop.mCropRect.p_integral.x, crop.mCropRect.p_integral.y,
                crop.mCropRect.p_fractional.x, crop.mCropRect.p_fractional.y,
                crop.mCropRect.s.w, crop.mCropRect.s.h,
                crop.mResizeDst.w, crop.mResizeDst.h
                );
            enqueParams.mvCropRsInfo.push_back(crop);
#ifdef FEATURE_MODIFY
            if (crop_NoEIS.mGroupID != 0)
            {
                enqueParams.mvCropRsInfo.push_back(crop_NoEIS);
            }
#endif

            iter++;
        }
    }

    if (pRequest->context.burst_num > 1)
    {
        if(mpMultiFrameHandler)
            return mpMultiFrameHandler->collect(pRequest, featureEnqueParams);
        else
            MY_LOGW_IF(mbEnableLog, "no burst handler");
    }
    // callback
    enqueParams.mpfnCallback = pass2CbFunc;
    enqueParams.mpCookie     = this;

    //
    #if 0
    // FIXME: need this?
    enqueParams.mvPrivaData.push_back(NULL);

    // for crop
    enqueParams.mvP1SrcCrop.push_back(cropInfos.crop_p1_sensor);
    enqueParams.mvP1Dst.push_back(cropInfos.dstsize_resizer);
    enqueParams.mvP1DstCrop.push_back(cropInfos.crop_dma);
    #endif
    //
    MY_LOGD_IF(mbEnableLog, "cnt %d, in %d, out %d",
            muEnqueCnt, enqueParams.mvIn.size(), enqueParams.mvOut.size() );
    //
    { // add request to queue
        Mutex::Autolock _l(mLock);
        mvRunning.push_back(pRequest);
#if P2_DEQUE_DEBUG
        mvParams.push_back(enqueParams);
#endif
        muEnqueCnt++;
    }
    //
    {
        MY_LOGI("enque pass2 ...");
        CAM_TRACE_NAME("P2F:[Proc]drv enq");
#ifdef FEATURE_MODIFY
        featureEnqueParams.setQParams(enqueParams);
#endif // FEATURE_MODIFY

        if( !mpPipe->enque(featureEnqueParams) )
        {
            MY_LOGE("enque pass2 failed");
            //
            { // remove job from queue
                Mutex::Autolock _l(mLock);
                vector<sp<Request> >::iterator iter = mvRunning.end();
                while( iter != mvRunning.begin() ) {
                    iter--;
                    if( *iter == pRequest ) {
                        mvRunning.erase(iter);
                        break;
                    }
                }

                MY_LOGW("cnt %d execute failed", muDequeCnt);
                muDequeCnt++;
            }
            return UNKNOWN_ERROR;
        }
        MY_LOGI("enque pass2 success");
    }
    //
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P2Procedure::
onFinish(
    FrameParams const& params,
    MBOOL const success
)
{
    CAM_TRACE_NAME("P2F:[Proc]Finish");
    //params.in.mHandle->updateState(BufferHandle::Buffer_ReadDone);
    for( size_t i = 0; i < params.vOut.size(); i++ )
        if (params.vOut[i].mHandle.get())
            params.vOut[i].mHandle->updateState(
                    success ? BufferHandle::STATE_WRITE_OK : BufferHandle::STATE_WRITE_FAIL
                    );
    #if 1
    if (mbEnableDumpBuffer)
    {
        MY_LOGI("[YUV] DequeCnt(%d) size(%d)", muDequeCnt, params.vOut.size());
        sp<IImageBuffer> pImgBuf = NULL;
        for( size_t i = 0; i < params.vOut.size(); i++ )
        {
            pImgBuf = params.vOut[i].mHandle->getBuffer();
            MY_LOGI("[YUV] [%d] (%dx%d) Fmt(0x%x)", i,
                pImgBuf->getImgSize().w, pImgBuf->getImgSize().h,
                pImgBuf->getImgFormat()
            );
            {
                char filename[256] = {0};
                sprintf(filename, "/sdcard/yuv/p2-%04d-%dx%d.yuv",
                    muDequeCnt,
                    pImgBuf->getImgSize().w,
                    pImgBuf->getImgSize().h);
                MY_LOGI("[YUV] save %s", filename);
                pImgBuf->saveToFile(filename);
            }
        }
    }
    #endif
    if( params.outApp.get() )
        params.outApp->updateState(success ? MetaHandle::STATE_WRITE_OK : MetaHandle::STATE_WRITE_FAIL);
    if( params.outHal.get() )
        params.outHal->updateState(success ? MetaHandle::STATE_WRITE_OK : MetaHandle::STATE_WRITE_FAIL);
    //
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
P2Procedure::
pass2CbFunc(QParams& rParams)
{
    //MY_LOGD_IF(mbEnableLog, "pass2CbFunc +++");
    P2Procedure* pProcedure = reinterpret_cast<P2Procedure*>(rParams.mpCookie);
    pProcedure->handleDeque(rParams);
    //MY_LOGD_IF(mbEnableLog, "pass2CbFunc ---");
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
P2Procedure::
handleDeque(QParams& rParams)
{
    CAM_TRACE_NAME("P2F:[Proc]deque");
    Mutex::Autolock _l(mLock);
    sp<Request> pRequest = NULL;
    {
        MY_LOGD_IF(mbEnableLog, "p2 done %d, success %d", muDequeCnt, rParams.mDequeSuccess);
        pRequest = mvRunning.front();
        mvRunning.erase(mvRunning.begin());
        muDequeCnt++;
        MY_LOGD_IF(mbEnableLog, "p2 done muDequeCnt:%d", muDequeCnt);

#if P2_DEQUE_DEBUG
        if( mvParams.size() )
        {
            QParams checkParam;
            checkParam = mvParams.front();
            mvParams.erase(mvParams.begin());
            //
            // make sure params are correct
            //
            #define ERROR_IF_NOT_MATCHED(item, i, expected, actual) do{             \
                if( expected != actual)                                             \
                    MY_LOGE("%s %d: expected %p != %p", item, i, expected, actual); \
            } while(0)
            //
            for( size_t i = 0 ; i < checkParam.mvIn.size() ; i++ )
            {
                if( i > rParams.mvIn.size() ) {
                    MY_LOGE("no src in dequed Params");
                    break;
                }
                //
                ERROR_IF_NOT_MATCHED("src pa of in", i,
                        checkParam.mvIn[i].mBuffer->getBufPA(0),
                        rParams.mvIn[i].mBuffer->getBufPA(0)
                        );
                ERROR_IF_NOT_MATCHED("src va of in", i,
                        checkParam.mvIn[i].mBuffer->getBufVA(0),
                        rParams.mvIn[i].mBuffer->getBufVA(0)
                        );
            }
            //
            for( size_t i = 0 ; i < checkParam.mvOut.size() ; i++ )
            {
                if( i > rParams.mvOut.size() ) {
                    MY_LOGE("no enough dst in dequed Params, %d", i);
                    break;
                }
                //
                ERROR_IF_NOT_MATCHED("dst pa of out", i,
                        checkParam.mvOut[i].mBuffer->getBufPA(0),
                        rParams.mvOut[i].mBuffer->getBufPA(0)
                        );
                ERROR_IF_NOT_MATCHED("dst va of out", i,
                        checkParam.mvOut[i].mBuffer->getBufVA(0),
                        rParams.mvOut[i].mBuffer->getBufVA(0)
                        );
            }
            //
            #undef ERROR_IF_NOT_MATCHED
        }
        else {
            MY_LOGW("params size not matched");
        }
#endif
    }
    //
    if( rParams.mvTuningData.size() > 0 ) {
        void* pTuning = rParams.mvTuningData[0];
        if( pTuning ) {
            free(pTuning);
        }
    }
    //
    pRequest->responseDone(rParams.mDequeSuccess ? OK : UNKNOWN_ERROR);
    //
    mCondJob.signal();
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P2Procedure::
checkParams(FrameParams const params) const
{
#define CHECK(val, fail_ret, ...) \
    do{                           \
        if( !(val) )              \
        {                         \
            MY_LOGE(__VA_ARGS__); \
            return fail_ret;      \
        }                         \
    } while(0)
    //
    CHECK( params.in.mHandle.get() , BAD_VALUE , "no src handle" );
    CHECK( params.vOut.size()      , BAD_VALUE , "no dst" );
    CHECK( params.inApp.get()      , BAD_VALUE , "no in app meta" );
    CHECK( params.inHal.get()      , BAD_VALUE , "no in hal meta" );
    //
#undef CHECK
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
#ifdef FEATURE_MODIFY
MERROR
P2Procedure::
getCropInfos_NoEIS(
    IMetadata* const inApp,
    IMetadata* const inHal,
    MBOOL const isResized,
    MSize const& dstSize,
    MCropRect& result
) const
{
    crop_infos cropInfos;

    if( ! tryGetMetadata<MSize>(inHal, MTK_HAL_REQUEST_SENSOR_SIZE, cropInfos.sensor_size) ) {
        MY_LOGE("cannot get MTK_HAL_REQUEST_SENSOR_SIZE");
        return BAD_VALUE;
    }
    //
    MSize const sensor = cropInfos.sensor_size;
    MSize const active = mInitParams.activeArray.s;
    //
    cropInfos.isResized = isResized;
    // get current p1 buffer crop status
    if(
            !( tryGetMetadata<MRect>(inHal, MTK_P1NODE_SCALAR_CROP_REGION, cropInfos.crop_p1_sensor) &&
               tryGetMetadata<MSize>(inHal, MTK_P1NODE_RESIZER_SIZE      , cropInfos.dstsize_resizer) &&
               tryGetMetadata<MRect>(inHal, MTK_P1NODE_DMA_CROP_REGION   , cropInfos.crop_dma)
             )
      ) {
        MY_LOGW_IF(1, "[FIXME] should sync with p1 for rrz setting");
        //
        cropInfos.crop_p1_sensor  = MRect( MPoint(0,0), sensor );
        cropInfos.dstsize_resizer = sensor;
        cropInfos.crop_dma        = MRect( MPoint(0,0), sensor );
    }

    MY_LOGD_IF(1, "SCALAR_CROP_REGION:(%d,%d)(%dx%d) RESIZER_SIZE:(%dx%d) DMA_CROP_REGION:(%d,%d)(%dx%d)",
        cropInfos.crop_p1_sensor.p.x, cropInfos.crop_p1_sensor.p.y,
        cropInfos.crop_p1_sensor.s.w, cropInfos.crop_p1_sensor.s.h,
        cropInfos.dstsize_resizer.w, cropInfos.dstsize_resizer.h,
        cropInfos.crop_dma.p.x, cropInfos.crop_dma.p.y,
        cropInfos.crop_dma.s.w, cropInfos.crop_dma.s.h);

    //
    // setup transform
    cropInfos.tranActive2Sensor = simpleTransform(
                MPoint(0,0),
                active,
                sensor
            );
    //
    cropInfos.tranSensor2Resized = simpleTransform(
                cropInfos.crop_p1_sensor.p,
                cropInfos.crop_p1_sensor.s,
                cropInfos.dstsize_resizer
            );
    //
    MBOOL const isEisOn = MFALSE;
    //
    MRect cropRegion; //active array domain
    queryCropRegion(inApp, isEisOn, cropRegion);
    cropInfos.crop_a = cropRegion;
    cropInfos.isEisEabled = MFALSE;


    // coordinates: s_: sensor
    MRect const s_crop = transform(cropInfos.tranActive2Sensor, cropInfos.crop_a);
    MRect s_viewcrop;
    //
    if( s_crop.s.w * dstSize.h > s_crop.s.h * dstSize.w ) { // pillarbox
        s_viewcrop.s.w = div_round(s_crop.s.h * dstSize.w, dstSize.h);
        s_viewcrop.s.h = s_crop.s.h;
        s_viewcrop.p.x = s_crop.p.x + ((s_crop.s.w - s_viewcrop.s.w) >> 1);
        s_viewcrop.p.y = s_crop.p.y;
    }
    else { // letterbox
        s_viewcrop.s.w = s_crop.s.w;
        s_viewcrop.s.h = div_round(s_crop.s.w * dstSize.h, dstSize.w);
        s_viewcrop.p.x = s_crop.p.x;
        s_viewcrop.p.y = s_crop.p.y + ((s_crop.s.h - s_viewcrop.s.h) >> 1);
    }
    MY_LOGD_IF(mbEnableLog, "s_cropRegion(%d, %d, %dx%d), dst %dx%d, view crop(%d, %d, %dx%d)",
            s_crop.p.x     , s_crop.p.y     ,
            s_crop.s.w     , s_crop.s.h     ,
            dstSize.w      , dstSize.h      ,
            s_viewcrop.p.x , s_viewcrop.p.y ,
            s_viewcrop.s.w , s_viewcrop.s.h
           );
    //
    if( isResized ) {
        MRect r_viewcrop = transform(cropInfos.tranSensor2Resized, s_viewcrop);

        result.s            = r_viewcrop.s;
        result.p_integral   = r_viewcrop.p;
        result.p_fractional = MPoint(0,0);

        // make sure hw limitation
        result.s.w &= ~(0x1);
        result.s.h &= ~(0x1);

        // check boundary
        if( refineBoundary(cropInfos.dstsize_resizer, result) ) {
            MY_LOGE("[FIXME] need to check crop!");
            cropInfos.dump();
        }
    }
    else {
        result.s            = s_viewcrop.s;
        result.p_integral   = s_viewcrop.p;
        result.p_fractional = MPoint(0,0);

        // make sure hw limitation
        result.s.w &= ~(0x1);
        result.s.h &= ~(0x1);

        // check boundary
        if( refineBoundary(cropInfos.sensor_size, result) ) {
            MY_LOGE("[FIXME] need to check crop!");
            cropInfos.dump();
        }
    }
    //
    MY_LOGD_IF(mbEnableLog, "resized %d, crop %d/%d, %d/%d, %dx%d",
            isResized,
            result.p_integral.x,
            result.p_integral.y,
            result.p_fractional.x,
            result.p_fractional.y,
            result.s.w,
            result.s.h
            );

    return OK;
}
#endif
/******************************************************************************
 *
 ******************************************************************************/
MERROR
P2Procedure::
getCropInfos(
    IMetadata* const inApp,
    IMetadata* const inHal,
    MBOOL const isResized,
    crop_infos & cropInfos
) const
{
    if( ! tryGetMetadata<MSize>(inHal, MTK_HAL_REQUEST_SENSOR_SIZE, cropInfos.sensor_size) ) {
        MY_LOGE("cannot get MTK_HAL_REQUEST_SENSOR_SIZE");
        return BAD_VALUE;
    }
    //
    MSize const sensor = cropInfos.sensor_size;
    MSize const active = mInitParams.activeArray.s;
    //
    cropInfos.isResized = isResized;
    // get current p1 buffer crop status
    if(
            !( tryGetMetadata<MRect>(inHal, MTK_P1NODE_SCALAR_CROP_REGION, cropInfos.crop_p1_sensor) &&
               tryGetMetadata<MSize>(inHal, MTK_P1NODE_RESIZER_SIZE      , cropInfos.dstsize_resizer) &&
               tryGetMetadata<MRect>(inHal, MTK_P1NODE_DMA_CROP_REGION   , cropInfos.crop_dma)
             )
      ) {
        MY_LOGW_IF(1, "[FIXME] should sync with p1 for rrz setting");
        //
        cropInfos.crop_p1_sensor  = MRect( MPoint(0,0), sensor );
        cropInfos.dstsize_resizer = sensor;
        cropInfos.crop_dma        = MRect( MPoint(0,0), sensor );
    }
    //
    // setup transform
    cropInfos.tranActive2Sensor = simpleTransform(
                MPoint(0,0),
                active,
                sensor
            );
    //
    cropInfos.tranSensor2Resized = simpleTransform(
                cropInfos.crop_p1_sensor.p,
                cropInfos.crop_p1_sensor.s,
                cropInfos.dstsize_resizer
            );
    //
    MBOOL const isEisOn = isEISOn(inApp);
    //
    MRect cropRegion; //active array domain
    queryCropRegion(inApp, isEisOn, cropRegion);
    cropInfos.crop_a = cropRegion;
    //
    // query EIS result
    {
        eis_region eisInfo;
        if( isEisOn && queryEisRegion(inHal, eisInfo)) {
            cropInfos.isEisEabled = MTRUE;
            // calculate mv
            vector_f* pMv_s = &cropInfos.eis_mv_s;
            vector_f* pMv_r = &cropInfos.eis_mv_r;
            MBOOL isResizedDomain = MTRUE;
#if 0
            //eis in sensor domain
            isResizedDomain = MFALSE;
            pMv_s->p.x  = eisInfo.x_int - (sensor.w * (EIS_FACTOR-100)/2/EIS_FACTOR);
            pMv_s->pf.x = eisInfo.x_float;
            pMv_s->p.y  = eisInfo.y_int - (sensor.h * (EIS_FACTOR-100)/2/EIS_FACTOR);
            pMv_s->pf.y = eisInfo.y_float;
            //
            cropInfos.eis_mv_r = transform(cropInfos.tranSensor2Resized, cropInfos.eis_mv_s);
#else
            MSize const resizer = cropInfos.dstsize_resizer;

#if SUPPORT_EIS_MV

            if (eisInfo.is_from_zzr)
            {
                pMv_r->p.x  = eisInfo.x_mv_int;
                pMv_r->pf.x = eisInfo.x_mv_float;
                pMv_r->p.y  = eisInfo.y_mv_int;
                pMv_r->pf.y = eisInfo.y_mv_float;
                cropInfos.eis_mv_s = inv_transform(cropInfos.tranSensor2Resized, cropInfos.eis_mv_r);
            }
            else
            {
                isResizedDomain = MFALSE;
                pMv_s->p.x  = eisInfo.x_mv_int;
                pMv_s->pf.x = eisInfo.x_mv_float;
                pMv_s->p.y  = eisInfo.y_mv_int;
                pMv_s->pf.y = eisInfo.y_mv_float;
                cropInfos.eis_mv_r = transform(cropInfos.tranSensor2Resized, cropInfos.eis_mv_s);
            }
#else
            //eis in resized domain
            pMv_r->p.x  = eisInfo.x_int - (resizer.w * (EIS_FACTOR-100)/2/EIS_FACTOR);
            pMv_r->pf.x = eisInfo.x_float;
            pMv_r->p.y  = eisInfo.y_int - (resizer.h * (EIS_FACTOR-100)/2/EIS_FACTOR);
            pMv_r->pf.y = eisInfo.y_float;
            //
            cropInfos.eis_mv_s = inv_transform(cropInfos.tranSensor2Resized, cropInfos.eis_mv_r);
#endif
            //
            MY_LOGD_IF(1, "mv (%s): (%d, %d, %d, %d) -> (%d, %d, %d, %d)",
                    isResizedDomain ? "r->s" : "s->r",
                    pMv_r->p.x,
                    pMv_r->pf.x,
                    pMv_r->p.y,
                    pMv_r->pf.y,
                    pMv_s->p.x,
                    pMv_s->pf.x,
                    pMv_s->p.y,
                    pMv_s->pf.y
                    );
#endif
            cropInfos.eis_mv_a = inv_transform(cropInfos.tranActive2Sensor, cropInfos.eis_mv_s);
            MY_LOGD_IF(1, "mv in active %d/%d, %d/%d",
                    cropInfos.eis_mv_a.p.x,
                    cropInfos.eis_mv_a.pf.x,
                    cropInfos.eis_mv_a.p.y,
                    cropInfos.eis_mv_a.pf.y
                    );
        }
        else {
            cropInfos.isEisEabled = MFALSE;
            //
            // no need to set 0
            //memset(&cropInfos.eis_mv_a, 0, sizeof(vector_f));
            //memset(&cropInfos.eis_mv_s, 0, sizeof(vector_f));
            //memset(&cropInfos.eis_mv_r, 0, sizeof(vector_f));
        }
    }
    // debug
    //cropInfos.dump();
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
P2Procedure::
queryCropRegion(
    IMetadata* const meta_request,
    MBOOL const isEisOn,
    MRect& cropRegion
) const
{
    if( !tryGetMetadata<MRect>(meta_request, MTK_SCALER_CROP_REGION, cropRegion) ) {
        cropRegion.p = MPoint(0,0);
        cropRegion.s = mInitParams.activeArray.s;
        MY_LOGW_IF(mbEnableLog, "no MTK_SCALER_CROP_REGION, crop full size %dx%d",
                cropRegion.s.w, cropRegion.s.h);
    }
    MY_LOGD_IF(mbEnableLog, "control: cropRegion(%d, %d, %dx%d)",
            cropRegion.p.x, cropRegion.p.y, cropRegion.s.w, cropRegion.s.h);
    //
    #if SUPPORT_EIS
    if( isEisOn ) {
        cropRegion.p.x += (cropRegion.s.w * (EIS_FACTOR-100)/2/EIS_FACTOR);
        cropRegion.p.y += (cropRegion.s.h * (EIS_FACTOR-100)/2/EIS_FACTOR);
        cropRegion.s   = cropRegion.s * 100 / EIS_FACTOR;
        MY_LOGD_IF(mbEnableLog, "EIS: factor %d, cropRegion(%d, %d, %dx%d)",
                EIS_FACTOR,
                cropRegion.p.x, cropRegion.p.y, cropRegion.s.w, cropRegion.s.h);
    }
    #endif
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
P2Procedure::
updateCropRegion(
    MRect const crop,
    IMetadata* meta_result
) const
{
    trySetMetadata<MRect>(meta_result, MTK_SCALER_CROP_REGION, crop);
    //
    MY_LOGD_IF( DEBUG_LOG && mbEnableLog, "result: cropRegion (%d, %d, %dx%d)",
            crop.p.x, crop.p.y, crop.s.w, crop.s.h);
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
P2Procedure::
calcCrop_viewangle(
    crop_infos const& cropInfos,
    MSize const& dstSize,
    MCropRect& result
) const
{
    MBOOL const isResized = cropInfos.isResized;
    // coordinates: s_: sensor
    MRect const s_crop = transform(cropInfos.tranActive2Sensor, cropInfos.crop_a);
    MRect s_viewcrop;
    //
    if( s_crop.s.w * dstSize.h > s_crop.s.h * dstSize.w ) { // pillarbox
        s_viewcrop.s.w = div_round(s_crop.s.h * dstSize.w, dstSize.h);
        s_viewcrop.s.h = s_crop.s.h;
        s_viewcrop.p.x = s_crop.p.x + ((s_crop.s.w - s_viewcrop.s.w) >> 1);
        s_viewcrop.p.y = s_crop.p.y;
    }
    else { // letterbox
        s_viewcrop.s.w = s_crop.s.w;
        s_viewcrop.s.h = div_round(s_crop.s.w * dstSize.h, dstSize.w);
        s_viewcrop.p.x = s_crop.p.x;
        s_viewcrop.p.y = s_crop.p.y + ((s_crop.s.h - s_viewcrop.s.h) >> 1);
    }
    MY_LOGD_IF(mbEnableLog, "s_cropRegion(%d, %d, %dx%d), dst %dx%d, view crop(%d, %d, %dx%d)",
            s_crop.p.x     , s_crop.p.y     ,
            s_crop.s.w     , s_crop.s.h     ,
            dstSize.w      , dstSize.h      ,
            s_viewcrop.p.x , s_viewcrop.p.y ,
            s_viewcrop.s.w , s_viewcrop.s.h
           );
    //
    if( isResized ) {
        MRect r_viewcrop = transform(cropInfos.tranSensor2Resized, s_viewcrop);
        result.s            = r_viewcrop.s;
        result.p_integral   = r_viewcrop.p + cropInfos.eis_mv_r.p;
        result.p_fractional = cropInfos.eis_mv_r.pf;

        // make sure hw limitation
        result.s.w &= ~(0x1);
        result.s.h &= ~(0x1);

        // check boundary
        if( refineBoundary(cropInfos.dstsize_resizer, result) ) {
            MY_LOGE("[FIXME] need to check crop!");
            cropInfos.dump();
        }
    }
    else {
        result.s            = s_viewcrop.s;
        result.p_integral   = s_viewcrop.p + cropInfos.eis_mv_s.p;
        result.p_fractional = cropInfos.eis_mv_s.pf;

        // make sure hw limitation
        result.s.w &= ~(0x1);
        result.s.h &= ~(0x1);

        // check boundary
        if( refineBoundary(cropInfos.sensor_size, result) ) {
            MY_LOGE("[FIXME] need to check crop!");
            cropInfos.dump();
        }
    }
    //
    MY_LOGD_IF(mbEnableLog, "resized %d, crop %d/%d, %d/%d, %dx%d",
            isResized,
            result.p_integral.x,
            result.p_integral.y,
            result.p_fractional.x,
            result.p_fractional.y,
            result.s.w,
            result.s.h
            );
}


/******************************************************************************
 *
 ******************************************************************************/
MBOOL
P2Procedure::
isEISOn(
    IMetadata* const inApp
) const
{
    MUINT8 eisMode = MTK_CONTROL_VIDEO_STABILIZATION_MODE_OFF;
    if( !tryGetMetadata<MUINT8>(inApp, MTK_CONTROL_VIDEO_STABILIZATION_MODE, eisMode) ) {
        MY_LOGW_IF(mbEnableLog, "no MTK_CONTROL_VIDEO_STABILIZATION_MODE");
    }
#if FORCE_EIS_ON
    eisMode = MTK_CONTROL_VIDEO_STABILIZATION_MODE_ON;
#endif
    return eisMode == MTK_CONTROL_VIDEO_STABILIZATION_MODE_ON;
}


/******************************************************************************
 *
 ******************************************************************************/
MBOOL
P2Procedure::
queryEisRegion(
    IMetadata* const inHal,
    eis_region& region
) const
{
    IMetadata::IEntry entry = inHal->entryFor(MTK_EIS_REGION);

#if SUPPORT_EIS_MV
    // get EIS's motion vector
    if (entry.count() > 8)
    {
        MINT32 x_mv         = entry.itemAt(6, Type2Type<MINT32>());
        MINT32 y_mv         = entry.itemAt(7, Type2Type<MINT32>());
        region.is_from_zzr  = entry.itemAt(8, Type2Type<MINT32>());
        MBOOL x_mv_negative = x_mv >> 31;
        MBOOL y_mv_negative = y_mv >> 31;
        // convert to positive for getting parts of int and float if negative
        if (x_mv_negative) x_mv = ~x_mv + 1;
        if (y_mv_negative) y_mv = ~y_mv + 1;
        //
        region.x_mv_int   = (x_mv & (~0xFF)) >> 8;
        region.x_mv_float = (x_mv & (0xFF)) << 31;
        if(x_mv_negative){
            region.x_mv_int   = ~region.x_mv_int + 1;
            region.x_mv_float = ~region.x_mv_float + 1;
        }
        region.y_mv_int   = (y_mv& (~0xFF)) >> 8;
        region.y_mv_float = (y_mv& (0xFF)) << 31;
        if(y_mv_negative){
            region.y_mv_int   = ~region.y_mv_int + 1;
            region.y_mv_float = ~region.x_mv_float + 1;
        }
        //
        MY_LOGD_IF(mbEnableLog, "EIS MV:%d, %d, %d",
                        region.s.w,
                        region.s.h,
                        region.is_from_zzr);
    }
#endif

#ifdef FEATURE_MODIFY
    if (entry.count() > 10)
    {
        region.gmvX = entry.itemAt(9, Type2Type<MINT32>());
        region.gmvY = entry.itemAt(10, Type2Type<MINT32>());
        MY_LOGD_IF(mbEnableLog, "EIS gmvX:%d, %d, gmvY: %d", region.gmvX, region.gmvY);
    }
#endif // FEATURE_MODIFY

    // get EIS's region
    if (entry.count() > 5)
    {
        region.x_int        = entry.itemAt(0, Type2Type<MINT32>());
        region.x_float      = entry.itemAt(1, Type2Type<MINT32>());
        region.y_int        = entry.itemAt(2, Type2Type<MINT32>());
        region.y_float      = entry.itemAt(3, Type2Type<MINT32>());
        region.s.w          = entry.itemAt(4, Type2Type<MINT32>());
        region.s.h          = entry.itemAt(5, Type2Type<MINT32>());
        //
        MY_LOGD_IF(mbEnableLog, "EIS Region: %d, %d, %d, %d, %dx%d",
                        region.x_int,
                        region.x_float,
                        region.y_int,
                        region.y_float,
                        region.s.w,
                        region.s.h);
       return MTRUE;
    }
    //
    MY_LOGW("wrong eis region count %zu", entry.count());
    return MFALSE;
}


/******************************************************************************
 *
 ******************************************************************************/
inline
MBOOL
P2Procedure::
refineBoundary(
    MSize const& bufSize,
    MCropRect& crop
) const
{
    MBOOL isRefined = MFALSE;
    MCropRect refined = crop;
    if( crop.p_integral.x < 0 ) {
        refined.p_integral.x = 0;
        isRefined = MTRUE;
    }
    if( crop.p_integral.y < 0 ) {
        refined.p_integral.y = 0;
        isRefined = MTRUE;
    }
    //
    int const carry_x = (crop.p_fractional.x != 0) ? 1 : 0;
    if( (refined.p_integral.x + crop.s.w + carry_x) > bufSize.w ) {
        refined.s.w = bufSize.w - refined.p_integral.x - carry_x;
        isRefined = MTRUE;
    }
    int const carry_y = (crop.p_fractional.y != 0) ? 1 : 0;
    if( (refined.p_integral.y + crop.s.h + carry_y) > bufSize.h ) {
        refined.s.h = bufSize.h - refined.p_integral.y - carry_y;
        isRefined = MTRUE;
    }
    //
    if( isRefined ) {
        MY_LOGE("buf size %dx%d, crop(%d/%d, %d/%d, %dx%d) -> crop(%d/%d, %d/%d, %dx%d)",
                bufSize.w, bufSize.h,
                crop.p_integral.x,
                crop.p_integral.y,
                crop.p_fractional.x,
                crop.p_fractional.y,
                crop.s.w,
                crop.s.h,
                refined.p_integral.x,
                refined.p_integral.y,
                refined.p_fractional.x,
                refined.p_fractional.y,
                refined.s.w,
                refined.s.h
                );
        crop = refined;
    }
    return isRefined;
}


/******************************************************************************
 *
 ******************************************************************************/
sp<Processor>
MDPProcedure::
createProcessor(InitParams& params)
{
    return new ProcessorBase<MDPProcedure>(params, PROCESSOR_NAME_MDP);
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
MDPProcedure::
onExecute(
    sp<Request> const,
    FrameParams const& params
)
{
    CAM_TRACE_NAME("P2F:[MDP]exe");
    FUNC_START;
    //
    MERROR ret = OK;
    //
    IImageBuffer* pSrc = NULL;
    vector<IImageBuffer*> vDst;
    // input
    if( params.in.mHandle.get() )
    {
        if( OK != (ret = params.in.mHandle->waitState(BufferHandle::STATE_READABLE)) ) {
            MY_LOGW("src buffer err = %d", ret);
            return ret;
        }
        pSrc = params.in.mHandle->getBuffer();
    }
    else {
        MY_LOGW("no src");
        return BAD_VALUE;
    }
    //
    // output
    for( size_t i = 0; i < params.vOut.size(); i++ )
    {
        if( params.vOut[i].mHandle == NULL ||
            OK != (ret = params.vOut[i].mHandle->waitState(BufferHandle::STATE_WRITABLE)) ) {
            MY_LOGW("dst buffer err = %d", ret);
            continue;
        }
        IImageBuffer* pDst = params.vOut[i].mHandle->getBuffer();
        //
        if( pDst != NULL )
            vDst.push_back(pDst);
    }
    //
    if( pSrc == NULL || vDst.size() == 0 ) {
        MY_LOGE("wrong mdp in/out: src %p, dst count %d", pSrc, vDst.size());
        return BAD_VALUE;
    }
    //
    MBOOL success = MFALSE;
    {
        MRect   const roi       = params.crop;
        MUINT32 const transform = 0; //TODO
        //
        #ifdef USING_MTK_LDVT
        success = MTRUE;
        #else
        NSSImager::IImageTransform* pTrans = NSSImager::IImageTransform::createInstance();
        if( !pTrans ) {
            MY_LOGE("!pTrans");
            return UNKNOWN_ERROR;
        }
        //
        success =
            pTrans->execute(
                pSrc,
                vDst[0],
                (vDst.size() > 1 )? vDst[1] : NULL,
                roi,
                transform,
                0xFFFFFFFF
                );

        pTrans->destroyInstance();
        pTrans = NULL;
        #endif
    }
    //
    return success ? OK : UNKNOWN_ERROR;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
MDPProcedure::
onFinish(
    FrameParams const& params,
    MBOOL const success
)
{
    CAM_TRACE_NAME("P2F:[MDP]Finish");
    //params.in.mHandle->updateState(BufferHandle::Buffer_ReadDone);
    for( size_t i = 0; i < params.vOut.size(); i++ )
        if (params.vOut[i].mHandle.get())
            params.vOut[i].mHandle->updateState(
                    success ? BufferHandle::STATE_WRITE_OK : BufferHandle::STATE_WRITE_FAIL
                    );
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
P2Procedure::
onExtractParams(Request* pRequest, FrameParams& param_p2)
{
    // input buffer
    {
        StreamId_T const streamId = pRequest->context.in_buffer->getStreamId();
        param_p2.in.mPortId = PORT_IMGI;
        param_p2.in.mHandle = pRequest->context.in_buffer;
        param_p2.bResized = pRequest->context.resized;
        pRequest->context.in_buffer.clear();
    }

    // output buffer
    MUINT32 occupied = 0;
    MBOOL remains = MFALSE;
    vector<sp<BufferHandle> >::iterator iter = pRequest->context.out_buffers.begin();
    for (; iter !=  pRequest->context.out_buffers.end(); iter++)
    {
        sp<BufferHandle> pOutBuffer = *iter;
        if(!pOutBuffer.get())
            continue;

        StreamId_T const streamId = pOutBuffer->getStreamId();
        MUINT32 const transform = pOutBuffer->getTransform();
        MUINT32 const usage = pOutBuffer->getUsage();

        PortID port_p2;
        MBOOL isFdStream = streamId == pRequest->context.fd_stream_id;
        if (OK == mapPortId(streamId, transform, isFdStream, occupied, port_p2))
        {
            P2Procedure::FrameOutput out;
            out.mPortId = port_p2;
            out.mHandle = pOutBuffer;
            out.mTransform = transform;
            out.mUsage = usage;

            param_p2.vOut.push_back(out);
            (*iter).clear();
        }
        else
            remains = MTRUE;
    }

    if (remains && param_p2.vOut.size() > 0)
    {
        pRequest->context.in_mdp_buffer = param_p2.vOut[param_p2.vOut.size() - 1].mHandle;
    }

    param_p2.inApp = pRequest->context.in_app_meta;
#ifdef FEATURE_MODIFY
#ifdef ISO_META
    param_p2.inRetApp = pRequest->context.in_ret_app_meta;
#else
    param_p2.inRetApp = NULL;
#endif // ISO_META

#endif // FEATURE_MODIFY
    param_p2.inHal = pRequest->context.in_hal_meta;
    param_p2.outApp = pRequest->context.out_app_meta;
    param_p2.outHal = pRequest->context.out_hal_meta;

    pRequest->context.in_app_meta.clear();
#ifdef FEATURE_MODIFY
#ifdef ISO_META
    pRequest->context.in_ret_app_meta.clear();
#endif // ISO_META
#endif // FEATURE_MODIFY
    pRequest->context.in_hal_meta.clear();
    pRequest->context.out_app_meta.clear();
    pRequest->context.out_hal_meta.clear();

    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
MDPProcedure::
onExtractParams(Request* pRequest, FrameParams& param_mdp)
{
    if (!pRequest->context.in_mdp_buffer.get())
    {
        return NOT_ENOUGH_DATA;
    }
    param_mdp.in.mHandle = pRequest->context.in_mdp_buffer;
    pRequest->context.in_mdp_buffer.clear();

    // input&output buffer
    vector<sp<BufferHandle> >::iterator iter = pRequest->context.out_buffers.begin();
    while(iter !=  pRequest->context.out_buffers.end())
    {
        sp<BufferHandle> pOutBuffer = *iter;
        if (pOutBuffer.get() && pOutBuffer->getState() == BufferHandle::STATE_NOT_USED)
        {
            MDPProcedure::FrameOutput out;
            out.mHandle = pOutBuffer;
            out.mTransform = pOutBuffer->getTransform();
            param_mdp.vOut.push_back(out);
            (*iter).clear();
        }
        iter++;
    }
    return (param_mdp.vOut.size() > 0) ? OK : UNKNOWN_ERROR;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P2Procedure::MultiFrameHandler::
collect(sp<Request> pRequest, FeaturePipeParam& featureParams)
{
#ifdef FEATURE_MODIFY
    QParams params = featureParams.getQParams();
#endif
    MINT32 index = mvReqCollecting.size();
    // tag
    for( size_t i = 0; i < params.mvStreamTag.size(); i++ )
    {
        mParamCollecting.mvStreamTag.push_back(params.mvStreamTag[i]);
    }
    // truning
    for( size_t i = 0; i < params.mvTuningData.size(); i++ )
    {
        mParamCollecting.mvTuningData.push_back(params.mvTuningData[i]);
    }
    // input
    for( size_t i = 0; i < params.mvIn.size(); i++ )
    {
        params.mvIn.editItemAt(i).mPortID.group = index;
        mParamCollecting.mvIn.push_back(params.mvIn[i]);
    }
    // output
    for( size_t i = 0; i < params.mvOut.size(); i++ )
    {
        params.mvOut.editItemAt(i).mPortID.group = index;
        mParamCollecting.mvOut.push_back(params.mvOut[i]);
    }
    // crop
    for( size_t i = 0; i < params.mvCropRsInfo.size(); i++ )
    {
        params.mvCropRsInfo.editItemAt(i).mFrameGroup = index;
        mParamCollecting.mvCropRsInfo.push_back(params.mvCropRsInfo[i]);
    }
    // module
    for( size_t i = 0; i < params.mvModuleData.size(); i++ )
    {
        params.mvModuleData.editItemAt(i).frameGroup = index;
        mParamCollecting.mvModuleData.push_back(params.mvModuleData[i]);
    }

    mvReqCollecting.push_back(pRequest);
    if(mvReqCollecting.size() >= pRequest->context.burst_num)
    {
        QParams enqueParams = mParamCollecting;
        // callback
        enqueParams.mpfnCallback = callback;
        enqueParams.mpCookie     = this;

        MY_LOGD_IF(mbEnableLog, "[burst] cnt %d, in %d, out %d",
                muMfEnqueCnt, enqueParams.mvIn.size(), enqueParams.mvOut.size() );
        muMfEnqueCnt++;
        {
#ifdef FEATURE_MODIFY
            featureParams.setQParams(enqueParams);
#endif // FEATURE_MODIFY
            MY_LOGI("[burst] Pass2 enque");
            CAM_TRACE_NAME("[burst] drv_enq");
            if (mpPipe->enque(featureParams))
            {
                Mutex::Autolock _l(mLock);
                MY_LOGI("[burst] Pass2 enque success");
                mvRunning.push_back(mvReqCollecting);
            }
            else
            {
                MY_LOGE("[burst] Pass2 enque failed");
                Mutex::Autolock _l(mLock);
                vector<sp<Request> >::iterator iter = mvReqCollecting.begin();
                while (iter != mvReqCollecting.end())
                {
                    (*iter)->responseDone(UNKNOWN_ERROR);
                    iter++;
                }

                MY_LOGW("[burst] cnt %d execute failed", muMfDequeCnt);
                muMfDequeCnt++;
            }
        }
        // clear the collected request
        mParamCollecting.mvStreamTag.clear();
        mParamCollecting.mvTuningData.clear();
        mParamCollecting.mvIn.clear();
        mParamCollecting.mvOut.clear();
        mParamCollecting.mvCropRsInfo.clear();
        mParamCollecting.mvModuleData.clear();
        mvReqCollecting.clear();
    }
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
P2Procedure::MultiFrameHandler::
handleCallback(QParams& rParams)
{
    CAM_TRACE_NAME("[burst] P2 deque");
    Mutex::Autolock _l(mLock);
    vector<sp<Request> > vpRequest;
    {
        MY_LOGD_IF(mbEnableLog, "[burst] p2 done %d, success %d", muMfDequeCnt, rParams.mDequeSuccess);
        vpRequest = mvRunning.front();
        mvRunning.erase(mvRunning.begin());
        muMfDequeCnt++;
        MY_LOGD_IF(mbEnableLog, "[burst] p2 done muDequeCnt:%d", muMfDequeCnt);
    }
    //
    for (size_t i = 0; i < rParams.mvTuningData.size(); i++ )
    {
        void* pTuning = rParams.mvTuningData[i];
        if( pTuning ) {
            free(pTuning);
        }
    }
    //
    vector<sp<Request> >::iterator iter = vpRequest.begin();
    while (iter != vpRequest.end())
    {
        (*iter)->responseDone(rParams.mDequeSuccess ? OK : UNKNOWN_ERROR);
        iter++;
    }
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
P2Procedure::MultiFrameHandler::
flush()
{
    FUNC_START;

    mParamCollecting = QParams();
    //mvReqCollecting.clear();
    vector<sp<Request> >::iterator iter = mvReqCollecting.begin();
    while (iter != mvReqCollecting.end())
    {
        (*iter)->responseDone(UNKNOWN_ERROR);
        iter = mvReqCollecting.erase(iter);
    }

    FUNC_END;
    return;
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
P2Procedure::
onFlush()
{
    CAM_TRACE_NAME("P2F:[Proc]Flush");
    if(mpMultiFrameHandler)
    {
        mpMultiFrameHandler->flush();
    }
    return;
}

} // namespace P2Feature
