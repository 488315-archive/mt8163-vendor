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

#define LOG_TAG "MtkCam/P2Node"
//
#include <Log.h>
#include "BaseNode.h"
#include "hwnode_utilities.h"
#include <v3/hwnode/P2Node.h>
//
#include <utils/RWLock.h>
#include <utils/Thread.h>
//
#include <sys/prctl.h>
#include <sys/resource.h>
//
#include <IHal3A.h>
#include <IHalSensor.h>
#include <iopipe/PostProc/INormalStream.h>
#include <iopipe/PostProc/IPortEnum.h>
#include <iopipe/SImager/IImageTransform.h>
#include <drv/isp_reg.h>
#include <isp_tuning.h>
#include <capturenr.h>
//
#include <vector>
#include <list>
#include <fstream>
//
#include <DebugScanLine.h>
//
#include <metadata/IMetadataProvider.h>
#include <metadata/client/mtk_metadata_tag.h>
#include <mtk_platform_metadata_tag.h>
//
#include <imagebuf/IIonImageBufferHeap.h>
//
#if 0/*[EP_TEMP]*/
#include <featureio/eis_hal.h>
#endif
//
#include <Trace.h>
//
#include <cutils/properties.h>
//
using namespace android;
using namespace NSCam;
using namespace NSCam::v3;
using namespace NSCam::Utils::Sync;

using namespace std;
using namespace std::chrono;
using namespace NSIoPipe;
using namespace NSIoPipe::NSPostProc;
using namespace NS3Av3;
using namespace NSIspTuning;

/******************************************************************************
 *
 ******************************************************************************/
#define PROCESSOR_NAME_P2   ("P2")
#define PROCESSOR_NAME_MDP  ("MDP")
#define PROCESSOR_NAME_SWNR ("SWNR")
#define P2THREAD_POLICY     (SCHED_OTHER)
#define P2THREAD_PRIORITY   (0)
//
#define WAITBUFFER_TIMEOUT  (1000000000L)
//
#define P2_DEBUG_DUMP_PATH          "/sdcard/camera_dump"
#define P2_DEBUG_PORT_IN_IMGO       (0x1)
#define P2_DEBUG_PORT_IN_RRZO       (0x2)
#define P2_DEBUG_PORT_OUT_WDMAO     (0x1)
#define P2_DEBUG_PORT_OUT_WROTO     (0x2)
#define P2_DEBUG_PORT_OUT_IMG2O     (0x4)
#define P2_DEBUG_PORT_OUT_IMG3O     (0x8)


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
#if 1
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
#define SUPPORT_MNR              (1)
#define SUPPORT_SWNR             (1)
#define SUPPORT_IMG3O_PORT       (1)
#define SUPPORT_FD_PORT          (1)
#define FORCE_EIS_ON             (SUPPORT_EIS  && (0))
#define FORCE_BURST_ON           (0)
#define FORCE_MNR_ON             (SUPPORT_MNR  && (0))
#define FORCE_SWNR_ON            (SUPPORT_SWNR && (0))

#define DEBUG_LOG                (1)

#if 1/*[EP_TEMP]*/
#ifdef EIS_FACTOR
#undef EIS_FACTOR
#endif
#define EIS_FACTOR 120
#endif
#define THRESHOLD_MNR_ISO 400
#define THRESHOLD_SWNR_ISO 800

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
static const PortID PORT_IMG3O( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_IMG3O, 1);

#else
static const PortID PORT_IMGI  (EPortType_Memory, EPipePortIndex_IMGI , 0);
static const PortID PORT_WDMAO (EPortType_Memory, EPipePortIndex_WDMAO, 1);
static const PortID PORT_WROTO (EPortType_Memory, EPipePortIndex_WROTO, 1);
#if SUPPORT_FD_PORT
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


class WorkingBufferHandle
    : public BufferHandle
{
    public:
        static sp<BufferHandle>         create(
                                            char const* szName,
                                            MINT32 format,
                                            MSize size
                                        );
                                        ~WorkingBufferHandle();
    protected:
                                        WorkingBufferHandle(
                                            char const* szName,
                                            sp<IImageBuffer> pImageBuffer
                                        )
                                            : muState(STATE_NOT_USED)
                                            , mpImageBuffer(pImageBuffer)
                                            , muTransform(0)
                                            , mBufferName(szName)
                                        {};

    public:
        IImageBuffer*                   getBuffer() { return mpImageBuffer.get(); }
        MERROR                          waitState(
                                            BufferState_t const state, nsecs_t const nsTimeout
                                        )
                                        { return OK; };

        MVOID                           updateState(
                                            BufferState_t const state
                                        )
                                        { muState = state; };


        virtual MUINT32                 getState() { return muState; };

        virtual MUINT32                 getTransform() { return muTransform; };


    private:
        MUINT32                         muState;
        MUINT32                         muTransform;
        sp<IImageBuffer>                mpImageBuffer;
        String8                         mBufferName;
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
                                            CAM_TRACE_ASYNC_BEGIN("P2:FrameLifeHolder", mpFrame->getFrameNo());
                                            MY_LOGD_IF(mbEnableLog, "frame %zu +", mpFrame->getFrameNo());
                                        }

                                        ~FrameLifeHolder() {
                                            if( mpStreamControl )
                                                mpStreamControl->onFrameDone(mpFrame);
                                            MY_LOGD_IF(mbEnableLog, "frame %zu -", mpFrame->getFrameNo());
                                            CAM_TRACE_ASYNC_END("P2:FrameLifeHolder", mpFrame->getFrameNo());
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


class Cropper
{
    public:
        typedef struct crop_info : public RefBase
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

        } crop_info_t;

        static MVOID calcViewAngle(
            MBOOL bEnableLog,
            crop_info_t const& cropInfos,
            MSize const& dstSize,
            MCropRect& result
        );

        static MBOOL refineBoundary(
            MSize const& bufSize,
            MCropRect& crop
        );

        static MVOID dump(
            crop_info_t const& cropInfos
        );

};


class Request
    : public VirtualLightRefBase
{
public:
    struct Context
    {
        Context()
        : burst_num(0)
        , nr_type(0)
        , iso(0)
        {};
        MUINT8                      burst_num;
        MBOOL                       resized;
        StreamId_T                  fd_stream_id;
        sp<BufferHandle>            in_buffer;
        sp<Cropper::crop_info_t>    crop_info;
        vector<sp<BufferHandle> >   out_buffers;
        sp<BufferHandle>            in_mdp_buffer;
        sp<MetaHandle>              in_app_meta;
        sp<MetaHandle>              in_hal_meta;
        sp<MetaHandle>              out_app_meta;
        sp<MetaHandle>              out_hal_meta;
        // 0:none, 1:MNR, 2:SWNR
        MUINT8                      nr_type;
        MINT32                      iso;
        sp<BufferHandle>            nr_work_buffer;
    };

    struct Context context;

    typedef enum
    {
        NR_TYPE_NONE = 0,
        NR_TYPE_MNR,
        NR_TYPE_SWNR
    } NR_TYPE_T;

/*
    typedef enum
    {
        STATE_WAIT,
        STATE_SUSPEND,
        STATE_RUNNING,
        STATE_DONE,
    } RequestState_t;

    RequestState_t          uState;
*/
    Request(sp<FrameLifeHolder> pFrameLifeHolder,
        MUINT32 uUniqueKey,
        MUINT32 mRequestNo,
        MUINT32 uFrameNo,
        MUINT32 uFrameSubNo)
        : mpFrameLifeHolder(pFrameLifeHolder)
        , muUniqueKey(uUniqueKey)
        , muRequestNo(mRequestNo)
        , muFrameNo(uFrameNo)
        , muFrameSubNo(uFrameSubNo)
        , mbReentry(MFALSE)
    {
    }

    MUINT32 getUniqueKey() {return muUniqueKey;};
    MUINT32 getRequestNo() {return muRequestNo;};
    MUINT32 getFrameNo() {return muFrameNo;};
    MUINT32 getFrameSubNo() {return muFrameSubNo;};
    MBOOL   isReentry() {return mbReentry;};
    MVOID   setReentry(MBOOL bReentry) {mbReentry = bReentry;};

    MBOOL   isFinished()
    {
        vector<sp<BufferHandle> >::iterator iter = context.out_buffers.begin();
        while (iter != context.out_buffers.end()) {
            if((*iter).get())
                return MFALSE;
            iter++;
        }
        return MTRUE;
    }

    MVOID setCurrentOwner(wp<Processor> pProcessor)
    {
        mwpProcessor = pProcessor;
    }

    MVOID responseDone(MERROR status)
    {
        sp<Processor> spProcessor = mwpProcessor.promote();
        if(spProcessor.get())
        {
            MY_LOGD_IF(DEBUG_LOG, "perform callback %d[%d]",
                muFrameNo ,muFrameSubNo);
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

#if SUPPORT_MNR || SUPPORT_SWNR
        if(context.nr_work_buffer.get())
        {
            context.nr_work_buffer.clear();
            MY_LOGW("context.nr_work_buffer not released");
        }
#endif

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
    MUINT32                 muUniqueKey;
    MUINT32                 muRequestNo;
    MUINT32                 muFrameNo;
    MUINT32                 muFrameSubNo;
    MBOOL                   mbReentry;

};


template<typename TProcedure>
struct ProcedureTraits {
    typedef typename TProcedure::InitParams  TInitParams;
    typedef typename TProcedure::FrameParams TProcParams;

    typedef MERROR (TProcedure::*TOnPullParams)(sp<Request>, TProcParams&);
    typedef MERROR (TProcedure::*TOnExecute)(sp<Request>, TProcParams const&);
    typedef MERROR (TProcedure::*TOnFinish)(TProcParams const&, MBOOL const);
    typedef MVOID  (TProcedure::*TOnFlush)();

    static constexpr TOnPullParams  fnOnPullParams  = &TProcedure::onPullParams;
    static constexpr TOnExecute     fnOnExecute     = &TProcedure::onExecute;
    static constexpr TOnFinish      fnOnFinish      = &TProcedure::onFinish;
    static constexpr TOnFlush       fnOnFlush       = &TProcedure::onFlush;

    static constexpr MBOOL  isAsync               = TProcedure::isAsync;
};


template<typename TProcedure, typename TProcedureTraits = ProcedureTraits<TProcedure>>
class ProcessorBase : public Processor, public Thread
{
    typedef typename TProcedureTraits::TInitParams          TInitParams;
    typedef typename TProcedureTraits::TProcParams          TProcParams;

    typedef typename TProcedureTraits::TOnPullParams        TOnPullParams;
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
        char threadname[10];
        sprintf(threadname, "Cam@%s", name);
        run(threadname);
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
        if (pRequest->isFinished())
            return OK;
        //
        pRequest->setCurrentOwner(this);
        // Make sure the request with a smaller frame number has a higher priority.
        vector<sp<Request> >::iterator it = mvPending.end();
        for (; it != mvPending.begin();)
        {
            --it;
            if (0 <= (MINT32)(pRequest->getFrameNo() - (*it)->getFrameNo()))
            {
                ++it; //insert(): insert before the current node
                break;
            }
        }
        mvPending.insert(it, pRequest);
        MY_LOGD_IF(DEBUG_LOG, PROC_TAG("after request[%d-%d] queued, pending:%d, running:%d"),
            pRequest->getFrameNo(), pRequest->getFrameSubNo(), mvPending.size(), mvRunning.size());

        mRequestQueueCond.signal();
        return OK;
    }


    virtual MERROR handleRequest(sp<Request> pRequest)
    {
        TProcParams params;
        TOnPullParams fnOnPullParams = TProcedureTraits::fnOnPullParams;
        TOnExecute fnOnExecute = TProcedureTraits::fnOnExecute;
        TOnFinish fnOnFinish = TProcedureTraits::fnOnFinish;
        // reset reentry flag
        pRequest->setReentry(MFALSE);

        if (OK == (mProcedure.*fnOnPullParams)(pRequest.get(), params))
        {
            MERROR ret = (mProcedure.*fnOnExecute)(pRequest, params);
            if (isAsyncProcessor() && ret == OK)
            {
                // do aync processing
                Mutex::Autolock _l(mCallbackLock);
                mvRunning.push_back(make_pair(pRequest, params));
                MY_LOGD_IF(DEBUG_LOG, PROC_TAG("request[%d-%d], pending:%d, running:%d"),
                    pRequest->getFrameNo(), pRequest->getFrameSubNo(), mvPending.size(), mvRunning.size());
                mCallbackCond.signal();
            }
            else
            {
                (mProcedure.*fnOnFinish)(params, ret == OK);
                // trigger to release buffer
                params = TProcParams();
                pRequest->onPartialRequestDone();

                sp<Processor> spProcessor = (pRequest->isReentry()) ? this : mwpNextProcessor.promote();
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
                pRequest->getFrameNo(), pRequest->getFrameSubNo(), mvPending.size(), mvRunning.size());
            TOnFinish fnOnFinish = TProcedureTraits::fnOnFinish;

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
                        pRequest->getFrameNo(), pRequest->getFrameSubNo());
                     mCallbackCond.wait(mCallbackLock);
                }
            } while (!isFound);

            MY_LOGD_IF(DEBUG_LOG, PROC_TAG("request callback async, status:%d"), status);
            pRequest->onPartialRequestDone();

            mAsyncCond.signal();
            sp<Processor> spProcessor = (pRequest->isReentry()) ? this : mwpNextProcessor.promote();
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
                MultiFrameHandler(INormalStream* const pPipe, MBOOL bEableLog)
                    : muMfEnqueCnt(0)
                    , muMfDequeCnt(0)
                    , mbEnableLog(bEableLog)
                    , mpPipe(pPipe)
                {
                }
                MERROR                      collect(sp<Request>, QParams&);
                MVOID                       flush();
                static MVOID                callback(QParams& rParams)
                {
                    MultiFrameHandler* pHandler = reinterpret_cast<MultiFrameHandler*>(rParams.mpCookie);
                    pHandler->handleCallback(rParams);
                }
                MVOID                       handleCallback(QParams& rParams);

            private:
                INormalStream* const        mpPipe;
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
            P2Node::ePass2Type              type;
            MRect                           activeArray;
            IHal3A_T*                       p3A;
            INormalStream*                  pPipe;
            MBOOL                           enableLog;
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
            MUINT32              uUniqueKey;
            MUINT32              uRequestNo;
            MUINT32              uFrameNo;
            FrameInput           in;
            Vector<FrameOutput>  vOut;
            //
            MBOOL                bResized;
#if SUPPORT_MNR
            MBOOL                bRunMnr;
#endif
#if SUPPORT_MNR || SUPPORT_SWNR
            MBOOL                bRunWorkBuffer;
#endif
            //
            sp<MetaHandle>       inApp;
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
                                            , mDebugScanLineMask(0)
                                            , mpDebugScanLine(NULL)
                                            , mbEnableLog(params.enableLog)
                                        {
                                            mpMultiFrameHandler = new MultiFrameHandler(
                                                params.pPipe, params.enableLog);

                                            muDumpBuffer = ::property_get_int32("debug.camera.dump.p2", 0);
                                            muDumpCondIn = ::property_get_int32("debug.camera.dump.p2.cond.in", 0xFF);
                                            muDumpPortIn = ::property_get_int32("debug.camera.dump.p2.in", 0xFF);
                                            muDumpPortOut = ::property_get_int32("debug.camera.dump.p2.out", 0xFF);
                                            muDumpPortImg3o = ::property_get_int32("debug.camera.dump.p2.ext.img3o", 0);
                                            muSensorFormatOrder = SENSOR_FORMAT_ORDER_NONE;

                                            if(muDumpBuffer)
                                            {
                                                IHalSensorList* const pIHalSensorList = IHalSensorList::get();
                                                if(pIHalSensorList)
                                                {
                                                    MUINT32 sensorDev = (MUINT32)pIHalSensorList->querySensorDevIdx(params.openId);

                                                    NSCam::SensorStaticInfo sensorStaticInfo;
                                                    memset(&sensorStaticInfo, 0, sizeof(NSCam::SensorStaticInfo));
                                                    pIHalSensorList->querySensorStaticInfo(sensorDev, &sensorStaticInfo);
                                                    muSensorFormatOrder = sensorStaticInfo.sensorFormatOrder;
                                                }
                                            }

                                            char cProperty[PROPERTY_VALUE_MAX];
                                            ::property_get("debug.camera.scanline.p2", cProperty, "0");
                                            mDebugScanLineMask = ::atoi(cProperty);
                                            if ( mDebugScanLineMask != 0)
                                            {
                                                mpDebugScanLine = DebugScanLine::createInstance();
                                            }
                                            //
                                            mnStreamTag = (params.type == P2Node::PASS2_TIMESHARING)
                                                ? ENormalStreamTag_Vss
                                                : ENormalStreamTag_Normal;
                                            //
                                        }

    public:
        MERROR                          waitIdle();

    public:                             // used by job
        static const MBOOL              isAsync = MTRUE;
        MERROR                          onP2PullParams(
                                            sp<Request> pRequest,
                                            FrameParams& params
                                        );

        MERROR                          onP2Execute(
                                            sp<Request> const pRequest,
                                            FrameParams const& params
                                        );

        MERROR                          onP2Finish(
                                            FrameParams const& params,
                                            MBOOL const success
                                        );

        MVOID                           onP2Flush();

    protected:

        MERROR                          mapPortId(
                                            StreamId_T const streamId, // [in]
                                            MUINT32 const transform,   // [in]
                                            MBOOL const isFdStream,    // [in]
                                            MUINT8& rOccupied,         // [in/out]
                                            PortID&  rPortId           // [out]
                                        ) const;

        MERROR                          checkParams(FrameParams const params) const;

        MERROR                          getCropInfos(
                                            IMetadata* const inApp,
                                            IMetadata* const inHal,
                                            MBOOL const isResized,
                                            Cropper::crop_info_t& cropInfos
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


        MBOOL                           isEISOn(
                                            IMetadata* const inApp
                                        ) const;

        MBOOL                           queryEisRegion(
                                            IMetadata* const inHal,
                                            eis_region& region
                                        ) const;

        static MVOID                    pass2CbFunc(QParams& rParams);

        MVOID                           handleDeque(QParams& rParams);

    private:
        MultiFrameHandler*            mpMultiFrameHandler;
        MBOOL const                     mbEnableLog;
        //
        mutable Mutex                   mLock;
        mutable Condition               mCondJob;
        //
        InitParams const                mInitParams;
        //
        INormalStream* const            mpPipe;
        MINT32                          mnStreamTag;
        IHal3A_T* const                 mp3A;
        //
        MUINT32                         muDumpBuffer;
        MUINT32                         muDumpCondIn;
        MUINT32                         muDumpPortIn;
        MUINT32                         muDumpPortOut;
        MUINT32                         muDumpPortImg3o;
        MUINT                           muSensorFormatOrder;
        //
        MUINT32                         muEnqueCnt;
        MUINT32                         muDequeCnt;
        vector<sp<Request> >            mvRunning;
        //
        #define DRAWLINE_PORT_WDMAO     0x1
        #define DRAWLINE_PORT_WROTO     0x2
        #define DRAWLINE_PORT_IMG2O     0x4
        MUINT32                         mDebugScanLineMask;
        DebugScanLine*                  mpDebugScanLine;

#if P2_DEQUE_DEBUG
        vector<QParams>                 mvParams;
#endif
};


class MdpProcedure
{
    public:
        struct InitParams
        {
            MBOOL                       enableLog;
        };

        struct FrameInput
        {
            sp<BufferHandle>            mHandle;
        };

        struct FrameOutput
        {
            sp<BufferHandle>            mHandle;
            MINT32                      mTransform;
        };

        struct FrameParams
        {
            MUINT32                     uFrameNo;
            sp<Cropper::crop_info_t>    pCropInfo;
            FrameInput                  in;
            Vector<FrameOutput>         vOut;
        };
    public:
        static sp<Processor>            createProcessor(InitParams& params);
                                        ~MdpProcedure() {}
                                        MdpProcedure(InitParams const& params)
                                        : mbEnableLog(params.enableLog)
                                        {
                                            muDumpBuffer = ::property_get_int32("debug.camera.dump.mdp", 0);
                                        }

    protected:

                                        MdpProcedure(MBOOL const enableLog)
                                        : mbEnableLog(enableLog)
                                        {}

    public:
        static const MBOOL              isAsync = MFALSE;

        MERROR                          waitIdle() { return OK; } // since is synchronous

        MERROR                          onMdpPullParams(
                                            sp<Request> pRequest,
                                            FrameParams& params
                                        );

        MERROR                          onMdpExecute(
                                            sp<Request> const pRequest,
                                            FrameParams const& params
                                        );

        MERROR                          onMdpFinish(
                                            FrameParams const& params,
                                            MBOOL const success
                                        );

        MVOID                           onMdpFlush(){};

    private:
        MBOOL const                     mbEnableLog;
        MUINT32                         muDumpBuffer;

};


class SwnrProcedure
{
    public:
        struct InitParams
        {
            MINT32                      openId;
            MBOOL                       enableLog;
        };

        struct FrameParams
        {
            sp<BufferHandle>            mpInHandle;
            MINT32                      iso;
            MUINT32                     uFrameNo;
        };
    public:
        static sp<Processor>            createProcessor(InitParams& params);
                                        ~SwnrProcedure()
                                        {
                                            delete mpSwnr;
                                        }
                                        SwnrProcedure(InitParams const& params)
                                        : mbEnableLog(params.enableLog)
                                        {
                                            mpSwnr = new SwNR(params.openId);
                                            muDumpBuffer = ::property_get_int32("debug.camera.dump.swnr", 0);
                                        }

    public:
        static const MBOOL              isAsync = MFALSE;

        MERROR                          waitIdle() { return OK; } // since is synchronous

        MERROR                          onSwnrPullParams(
                                            sp<Request> pRequest,
                                            FrameParams& params
                                        );

        MERROR                          onSwnrExecute(
                                            sp<Request> const pRequest,
                                            FrameParams const& params
                                        );

        MERROR                          onSwnrFinish(
                                            FrameParams const& params,
                                            MBOOL const success
                                        );

        MVOID                           onSwnrFlush(){};

    private:
        MBOOL const                     mbEnableLog;
        SwNR*                           mpSwnr;
        MUINT32                         muDumpBuffer;

};

#define DECLARE_PROC_TRAIT(proc) \
template<> \
struct ProcedureTraits<proc##Procedure> { \
    typedef typename proc##Procedure::InitParams  TInitParams; \
    typedef typename proc##Procedure::FrameParams TProcParams; \
    typedef MERROR (proc##Procedure::*TOnPullParams)(sp<Request>, TProcParams&);  \
    typedef MERROR (proc##Procedure::*TOnExecute)(sp<Request>, TProcParams const&); \
    typedef MERROR (proc##Procedure::*TOnFinish)(TProcParams const&, MBOOL const); \
    typedef MVOID  (proc##Procedure::*TOnFlush)(); \
    static constexpr TOnPullParams  fnOnPullParams  = &proc##Procedure::on##proc##PullParams;   \
    static constexpr TOnExecute     fnOnExecute     = &proc##Procedure::on##proc##Execute;      \
    static constexpr TOnFinish      fnOnFinish      = &proc##Procedure::on##proc##Finish;       \
    static constexpr TOnFlush       fnOnFlush       = &proc##Procedure::on##proc##Flush;        \
    static constexpr MBOOL  isAsync                 = proc##Procedure::isAsync;                 \
};

DECLARE_PROC_TRAIT(P2);
DECLARE_PROC_TRAIT(Mdp);
DECLARE_PROC_TRAIT(Swnr);

#undef DECLARE_PROC_TRAIT


/******************************************************************************
 *
 ******************************************************************************/
class P2NodeImp
    : public BaseNode
    , public P2Node
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
    MINT32                          mForceNrMode;

protected:  ////                    Data Members. (Config)
    ePass2Type const                mType;
    mutable RWLock                  mConfigRWLock;
    // meta
    sp<IMetaStreamInfo>             mpInAppMeta_Request;
    sp<IMetaStreamInfo>             mpInAppRetMeta_Request;
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
#if SUPPORT_SWNR
    sp<Processor>                   mpSwnrProcessor;
#endif
    sp<Processor>                   mpMdpProcessor;
};


/******************************************************************************
 *
 ******************************************************************************/
android::sp<P2Node>
P2Node::
createInstance(ePass2Type const type)
{
    if( type < 0 ||
        type >= PASS2_TYPE_TOTAL )
    {
        MY_LOGE("not supported p2 type %d", type);
        return NULL;
    }
    //
    return new P2NodeImp(type);
}


/******************************************************************************
 *
 ******************************************************************************/
P2NodeImp::
P2NodeImp(ePass2Type const type)
    : BaseNode()
    , P2Node()
    //
    , mType(type)
    , mConfigRWLock()
    //
    , mpInAppMeta_Request()
    , mpInAppRetMeta_Request()
    , mpInHalMeta_P1()
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
#if SUPPORT_SWNR
    , mpSwnrProcessor(NULL)
#endif
    , mpMdpProcessor(NULL)
    , mBurstNum(0)
{
    mLogLevel = ::property_get_int32("debug.camera.log", 0);
    if (mLogLevel == 0)
        mLogLevel = ::property_get_int32("debug.camera.log.p2node", 0);

    mForceNrMode = ::property_get_int32("debug.camera.force.nr", 0);

#if 1 /*[EP_TEMP]*/ //[FIXME] TempTestOnly
    #warning "[FIXME] force enable P2Node log"
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
    CAM_TRACE_NAME("P2:init");
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
            //
            mpP2Processor = P2Procedure::createProcessor(param);
            if( mpP2Processor == NULL )
                return BAD_VALUE;
        }
        //
#if SUPPORT_SWNR
        {
            mpSwnrProcessor  = NULL;
            //
            SwnrProcedure::InitParams param;
            param.openId      = mOpenId;
            param.enableLog   = mLogLevel >= 1;
            //
            mpSwnrProcessor = SwnrProcedure::createProcessor(param);
            if( mpSwnrProcessor == NULL )
                return BAD_VALUE;
        }
#endif
        //
        {
            mpMdpProcessor = NULL;
            //
            MdpProcedure::InitParams param;
            param.enableLog   = mLogLevel >= 1;
            //
            mpMdpProcessor = MdpProcedure::createProcessor(param);
            if( mpMdpProcessor == NULL )
                return BAD_VALUE;
        }
#if SUPPORT_SWNR
    mpP2Processor->setNextProcessor(mpSwnrProcessor);
    mpSwnrProcessor->setNextProcessor(mpMdpProcessor);
#else
    mpP2Processor->setNextProcessor(mpMdpProcessor);
#endif

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
    CAM_TRACE_NAME("P2:config");
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
        mpInAppMeta_Request     = rParams.pInAppMeta;
        mpInHalMeta_P1          = rParams.pInHalMeta;
        mpInAppRetMeta_Request  = rParams.pInAppRetMeta;
        mpOutAppMeta_Result     = rParams.pOutAppMeta;
        mpOutHalMeta_Result     = rParams.pOutHalMeta;
        // image
        mpvInFullRaw            = rParams.pvInFullRaw;
        mpInResizedRaw          = rParams.pInResizedRaw;
        mvOutImages             = rParams.vOutImage;
        mpOutFd                 = rParams.pOutFDImage;
        // property
        mBurstNum               = rParams.burstNum;
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
    CAM_TRACE_NAME("P2:uninit");
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
    CAM_TRACE_NAME("P2:flush");
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
    dumpStreamIfExist("[meta] in appRet", rParams.pInAppRetMeta);
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
    return isStream(mpInAppMeta_Request, streamId) ||
           isStream(mpInHalMeta_P1, streamId) ||
           isStream(mpInAppRetMeta_Request, streamId);
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
    pImageBuffer->unlockBuf(getNodeName());
    pStreamBuffer->unlock(getNodeName(), pImageBuffer->getImageBufferHeap());
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
    CAM_TRACE_NAME("P2:PartialFrameDone");
    //FUNC_START;
    IStreamBufferSet&     rStreamBufferSet = pFrame->getStreamBufferSet();
    rStreamBufferSet.applyRelease(getNodeId());
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
    CAM_TRACE_NAME("P2:FrameDone");
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
    sp<MetaHandle> pMeta_InAppRet  = mpInAppRetMeta_Request.get() ?
        MetaHandle::create(this, pFrame, mpInAppRetMeta_Request->getStreamId()) : NULL;
    //
    if( pMeta_InApp  == NULL ||
        pMeta_InHal  == NULL )
    {
        MY_LOGW("meta check failed");
        return BAD_VALUE;
    }
    //
    // 3. get pipeline's unique key
    IMetadata* pHalMeta = pMeta_InHal->getMetadata();
    MINT32 uniqueKey = 0;
    MBOOL key_status =tryGetMetadata<MINT32>(pHalMeta, MTK_PIPELINE_UNIQUE_KEY, uniqueKey);
    //
    // 4. create FrameLifeHolder
    sp<FrameLifeHolder> pFrameLife = new FrameLifeHolder(this, pFrame, mLogLevel >= 1);
    //
    // 5. process image IO
    IPipelineFrame::ImageInfoIOMapSet& imageIOMapSet = IOMapSet.mImageInfoIOMapSet;
    for( size_t run_idx = 0 ; run_idx < imageIOMapSet.size(); run_idx++ )
    {
        IPipelineFrame::ImageInfoIOMap const& imageIOMap = imageIOMapSet[run_idx];
        sp<Request> pRequest = new Request(
            pFrameLife, uniqueKey,
            pFrame->getRequestNo(), pFrame->getFrameNo(),
            run_idx);

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

        // determine NR's mode
        if(isFullRawLocked(streamId))
        {
            pRequest->context.burst_num = 0;
            IMetadata* inHal = pMeta_InHal->getMetadata();
            // get ISO
            MINT32 iso = -1;

            if (pMeta_InAppRet.get() &&
                !tryGetMetadata<MINT32>(pMeta_InAppRet->getMetadata(), MTK_SENSOR_SENSITIVITY, iso))
                 MY_LOGW_IF(1, "no MTK_SENSOR_SENSITIVITY from AppRetMeta");
            if(iso == -1) {
                IMetadata* inApp = pMeta_InApp->getMetadata();
                if (!tryGetMetadata<MINT32>(inApp, MTK_SENSOR_SENSITIVITY, iso))
                    MY_LOGW_IF(1, "no MTK_SENSOR_SENSITIVITY from AppMeta");
            }

            pRequest->context.iso = iso;
            // get NR mode
            MINT32 nrMode = MTK_NR_MODE_OFF;
            MINT32 threshold_mnr = -1;
            MINT32 threshold_swnr = -1;
            if (!tryGetMetadata<MINT32>(inHal, MTK_NR_MODE, nrMode))
                MY_LOGW_IF(0, "no MTK_NR_MODE");
            else if (nrMode == MTK_NR_MODE_MNR)
                pRequest->context.nr_type = Request::NR_TYPE_MNR;
            else if (nrMode == MTK_NR_MODE_SWNR)
                pRequest->context.nr_type = Request::NR_TYPE_SWNR;
            else if (nrMode == MTK_NR_MODE_OFF)
                pRequest->context.nr_type = Request::NR_TYPE_NONE;
            else
            {
                if( !tryGetMetadata<MINT32>(inHal, MTK_NR_MNR_THRESHOLD_ISO, threshold_mnr) ||
                    !tryGetMetadata<MINT32>(inHal, MTK_NR_SWNR_THRESHOLD_ISO, threshold_swnr)) {
                    MY_LOGW_IF(1, "cannot get nr thresholds");
                    pRequest->context.nr_type = Request::NR_TYPE_NONE;
                }
                else if (iso >= threshold_swnr)
                    pRequest->context.nr_type = Request::NR_TYPE_SWNR;
                else if (iso >= threshold_mnr)
                    pRequest->context.nr_type = Request::NR_TYPE_MNR;
                else
                    pRequest->context.nr_type = Request::NR_TYPE_NONE;
            }

            if (mForceNrMode > Request::NR_TYPE_NONE)
                pRequest->context.nr_type = mForceNrMode;

#if FORCE_MNR_ON
            pRequest->context.nr_type = Request::NR_TYPE_MNR;
#endif
#if FORCE_SWNR_ON
            pRequest->context.nr_type = Request::NR_TYPE_SWNR;
#endif
            MY_LOGD_IF(mLogLevel >= 2, "hal's NR mode:%d, actual NR mode:%d, thres %d/%d, and iso:%d",
                nrMode, pRequest->context.nr_type, threshold_mnr, threshold_swnr,iso);
        }

        // destination
        for( size_t i = 0; i < imageIOMap.vOut.size(); i++ )
        {
            StreamId_T const streamId = imageIOMap.vOut.keyAt(i);
            //MUINT32 const transform = imageIOMap.vOut.valueAt(i)->getTransform();
            pRequest->context.out_buffers.push_back(StreamBufferHandle::create(this, pFrame, streamId));
        }
        //
        pRequest->context.in_app_meta = pMeta_InApp;
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
    MUINT8& rOccupied,         // [in/out]
    PortID& rPortId            // [out]
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
#if SUPPORT_FD_PORT
        if( SUPPORT_FD_PORT && isFdStream ) {
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
    CAM_TRACE_NAME("P2:[Proc]createProcessor");
    INormalStream* pPipe = NULL;
    IHal3A_T* p3A = NULL;
    if( params.type != P2Node::PASS2_STREAM &&
        params.type != P2Node::PASS2_TIMESHARING ) {
        MY_LOGE("not supported type %d", params.type);
        goto lbExit;
    }
    //
    CAM_TRACE_BEGIN("P2:[Proc]NormalStream create");
    pPipe = INormalStream::createInstance(params.openId);
    //
    if( pPipe == NULL ) {
        MY_LOGE("create pipe failed");
        CAM_TRACE_END();
        goto lbExit;
    }
    //
    CAM_TRACE_BEGIN("P2:[Proc]NormalStream init");
    if( ! pPipe->init(LOG_TAG) )
    {
        CAM_TRACE_END();
        MY_LOGE("pipe init failed");
        goto lbExit;
    }
    CAM_TRACE_END();
    //
    #if SUPPORT_3A
    CAM_TRACE_BEGIN("P2:[Proc]3A create");
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
sp<BufferHandle>
WorkingBufferHandle::
create(
    char const* szName,
    MINT32 format,
    MSize size)
{
    String8 bufferName = String8::format(
        "%s:size%dx%d", szName, size.w, size.h);

    // query format
    MUINT32 plane = NSCam::Utils::Format::queryPlaneCount(format);
    size_t bufBoundaryInBytes[3] = {0, 0, 0};
    size_t bufStridesInBytes[3] = {0};
    //
    for (MUINT32 i = 0; i < plane; i++)
    {
        bufStridesInBytes[i] = NSCam::Utils::Format::queryPlaneWidthInPixels(format, i, size.w) *
            NSCam::Utils::Format::queryPlaneBitsPerPixel(format, i) / 8;
    }
    // create buffer
    IImageBufferAllocator::ImgParam imgParam =
        IImageBufferAllocator::ImgParam((EImageFormat)format,
            size, bufStridesInBytes, bufBoundaryInBytes, plane);
    //
    sp<IIonImageBufferHeap> pHeap =
        IIonImageBufferHeap::create(bufferName.string(), imgParam);
    if (pHeap == NULL)
    {
        MY_LOGE("working buffer[%s]: create heap failed", bufferName.string());
        return NULL;
    }
    sp<IImageBuffer> pImageBuffer = pHeap->createImageBuffer();
    if (pImageBuffer == NULL)
    {
        MY_LOGE("working buffer[%s]: create image buffer failed", bufferName.string());
        return NULL;
    }

    // lock buffer
    MUINT const usage = (GRALLOC_USAGE_SW_READ_OFTEN |
                        GRALLOC_USAGE_SW_WRITE_OFTEN |
                        GRALLOC_USAGE_HW_CAMERA_READ |
                        GRALLOC_USAGE_HW_CAMERA_WRITE);
    if (!(pImageBuffer->lockBuf(bufferName.string(), usage)))
    {
        MY_LOGE("working buffer[%s]: lock image buffer failed", bufferName.string());
        return NULL;
    }
    //
    MY_LOGD_IF(DEBUG_LOG, "working buffer[%s]: create successfully", bufferName.string());
    //
    return new WorkingBufferHandle(bufferName.string(), pImageBuffer);
}


/******************************************************************************
 *
 ******************************************************************************/
WorkingBufferHandle::
~WorkingBufferHandle()
{
    if (mpImageBuffer == NULL)
    {
        MY_LOGW("working buffer[%s]: not existed", mBufferName.string());
        return;
    }

    mpImageBuffer->unlockBuf(mBufferName.string());
    mpImageBuffer.clear();
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

    if( mpDebugScanLine != NULL )
    {
        mpDebugScanLine->destroyInstance();
        mpDebugScanLine = NULL;
    }
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P2Procedure::
onP2Execute(
    sp<Request> const pRequest,
    FrameParams const& params
)
{
    CAM_TRACE_NAME("P2:[Proc]exe");
    FUNC_START;
    //
    MERROR ret = OK;
    //
    if ( OK != (ret = checkParams(params)) )
        return ret;
    // prepare metadata
    IMetadata* pMeta_InApp  = params.inApp->getMetadata();
    IMetadata* pMeta_InHal  = params.inHal->getMetadata();
    IMetadata* pMeta_OutApp = params.outApp.get() ? params.outApp->getMetadata() : NULL;
    IMetadata* pMeta_OutHal = params.outHal.get() ? params.outHal->getMetadata() : NULL;
    //
    if( pMeta_InApp == NULL || pMeta_InHal == NULL ) {
        MY_LOGE("meta: in app %p, in hal %p", pMeta_InApp, pMeta_InHal);
        return BAD_VALUE;
    }
    //
    sp<Cropper::crop_info_t> pCropInfo = new Cropper::crop_info_t;
    if( OK != (ret = getCropInfos(pMeta_InApp, pMeta_InHal, params.bResized, *pCropInfo)) ) {
        MY_LOGE("getCropInfos failed");
        return ret;
    }
    pRequest->context.crop_info = pCropInfo;
    //
    QParams enqueParams;
    //frame tag
    enqueParams.mvStreamTag.push_back(mnStreamTag);
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
#if SUPPORT_MNR || SUPPORT_SWNR
        if (params.bRunWorkBuffer)
        {
            // do nothing
        }
        else
#endif
        if( params.bResized )
            pSrc->setExtParam(pCropInfo->dstsize_resizer);
        //
        enqueParams.mvIn.push_back(src);
        MY_LOGD_IF(mbEnableLog, "P2Node EnQ Src mPortID.index(%d) Fmt(0x%x) "
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
            trySetMetadata<MINT32>(&inMetaSet.halMeta, MTK_PIPELINE_FRAME_NUMBER, pRequest->getFrameNo());
            trySetMetadata<MINT32>(&inMetaSet.halMeta, MTK_PIPELINE_REQUEST_NUMBER, pRequest->getRequestNo());

#if SUPPORT_MNR
            if (pMeta_InHal && params.bRunMnr)
            {
                trySetMetadata<MUINT8>(&inMetaSet.halMeta, MTK_3A_ISP_PROFILE, EIspProfile_Capture_MultiPass_ANR_1);
            }
#endif
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
#if SUPPORT_IMG3O_PORT
                // dump tuning data
                if (((!params.bResized && muDumpCondIn & P2_DEBUG_PORT_IN_IMGO) ||
                     (params.bResized && muDumpCondIn & P2_DEBUG_PORT_IN_RRZO)) &&
                     muDumpPortImg3o && muDumpPortOut & P2_DEBUG_PORT_OUT_IMG3O)
                {
                    char filename[100];
                    sprintf(filename, P2_DEBUG_DUMP_PATH "/p2-%09d-%04d-%04d-tuning.data",
                        params.uUniqueKey, params.uFrameNo, params.uRequestNo);
                    std::ofstream out(filename);
                    out.write(reinterpret_cast<char*>(pTuning), tuningsize);
                }
#endif
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
            MRect cropRegion = pCropInfo->crop_a;
            if( pCropInfo->isEisEabled ) {
                cropRegion.p.x += pCropInfo->eis_mv_a.p.x;
                cropRegion.p.y += pCropInfo->eis_mv_a.p.y;
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
    // for output group crop
    {
        Vector<Output>::const_iterator iter = enqueParams.mvOut.begin();
        while( iter != enqueParams.mvOut.end() ) {
            MCrpRsInfo crop;
            MUINT32 const uPortIndex = iter->mPortID.index;
            if( uPortIndex == PORT_WDMAO.index ) {
                crop.mGroupID = 2;
#if SUPPORT_MNR || SUPPORT_SWNR
                if (params.bRunWorkBuffer)
                {
                    crop.mCropRect.p_fractional = {0, 0};
                    crop.mCropRect.p_integral = {0, 0};
                    crop.mCropRect.s = iter->mBuffer->getImgSize();
                }
                else
#endif
                    Cropper::calcViewAngle(mbEnableLog, *pCropInfo, iter->mBuffer->getImgSize(), crop.mCropRect);
            } else if ( uPortIndex == PORT_WROTO.index ) {
                crop.mGroupID = 3;
                IImageBuffer* pBuf      = iter->mBuffer;
                MINT32 const transform  = iter->mTransform;
                MSize dstSize = ( transform & eTransform_ROT_90 )
                                ? MSize(pBuf->getImgSize().h, pBuf->getImgSize().w)
                                : pBuf->getImgSize();
                Cropper::calcViewAngle(mbEnableLog, *pCropInfo, dstSize, crop.mCropRect);
#if SUPPORT_FD_PORT
            } else if ( uPortIndex == PORT_IMG2O.index ) {
                crop.mGroupID = 1;
                Cropper::calcViewAngle(mbEnableLog, *pCropInfo, iter->mBuffer->getImgSize(), crop.mCropRect);
#endif
#if SUPPORT_IMG3O_PORT
            } else if ( uPortIndex == PORT_IMG3O.index ) {
                crop.mGroupID = 4;
                crop.mCropRect.p_fractional = {0, 0};
                crop.mCropRect.p_integral = {0, 0};
                crop.mCropRect.s = iter->mBuffer->getImgSize();
#endif
            } else {
                MY_LOGE("not supported output port %p", iter->mPortID);
                return BAD_VALUE;
            }
            crop.mResizeDst = iter->mBuffer->getImgSize();
            MY_LOGI("P2Node EnQ out 00 Crop G(%d) S(%d,%d)(%d,%d)(%dx%d) D(%dx%d)",
                crop.mGroupID,
                crop.mCropRect.p_integral.x, crop.mCropRect.p_integral.y,
                crop.mCropRect.p_fractional.x, crop.mCropRect.p_fractional.y,
                crop.mCropRect.s.w, crop.mCropRect.s.h,
                crop.mResizeDst.w, crop.mResizeDst.h
                );
            enqueParams.mvCropRsInfo.push_back(crop);
            iter++;
        }
    }

    if (pRequest->context.burst_num > 1)
    {
        if(mpMultiFrameHandler)
            return mpMultiFrameHandler->collect(pRequest, enqueParams);
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
    enqueParams.mvP1SrcCrop.push_back(pCropInfo->crop_p1_sensor);
    enqueParams.mvP1Dst.push_back(pCropInfo->dstsize_resizer);
    enqueParams.mvP1DstCrop.push_back(pCropInfo->crop_dma);
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
        CAM_TRACE_NAME("P2:[Proc]drv enq");
        if( !mpPipe->enque(enqueParams) )
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
    FUNC_END;
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P2Procedure::
onP2Finish(
    FrameParams const& params,
    MBOOL const success
)
{
    CAM_TRACE_NAME("P2:[Proc]Finish");
    //params.in.mHandle->updateState(BufferHandle::Buffer_ReadDone);
    for( size_t i = 0; i < params.vOut.size(); i++ )
        if (params.vOut[i].mHandle.get())
            params.vOut[i].mHandle->updateState(
                    success ? BufferHandle::STATE_WRITE_OK : BufferHandle::STATE_WRITE_FAIL
                    );

    if (muDumpBuffer)
    {
        MY_LOGI("[YUV] frame(%d) size(%d)", params.uFrameNo, params.vOut.size());
        sp<IImageBuffer> pImgBuf = NULL;
        char filename[256] = {0};
        char portname[10] = {0};
        char formatname[10] = {0};
        MUINT32 strides[3] = {0};
        if (!NSCam::Utils::makePath(P2_DEBUG_DUMP_PATH,0660))
            MY_LOGW("makePath[%s] fails", P2_DEBUG_DUMP_PATH);

        // dump condition
        MBOOL cond = (!params.bResized && muDumpCondIn & P2_DEBUG_PORT_IN_IMGO) ||
              (params.bResized && muDumpCondIn & P2_DEBUG_PORT_IN_RRZO);

        // select output buffer if condition matched
        if (cond)
        {
#define stringifyImageFormat(e)                         \
            (e == eImgFmt_BAYER10) ? "bayer10" :        \
            (e == eImgFmt_FG_BAYER10) ? "fg_bayer10" :  \
            (e == eImgFmt_YV12) ? "yv12" :              \
            (e == eImgFmt_NV21) ? "nv21" :              \
            (e == eImgFmt_YUY2) ? "yuy2" :              \
            (e == eImgFmt_I420) ? "i420" :              \
            "undef"

#define stringifySensorFormat(e)                        \
            (e == SENSOR_FORMAT_ORDER_RAW_B)  ? "b" :   \
            (e == SENSOR_FORMAT_ORDER_RAW_Gb) ? "gb" :  \
            (e == SENSOR_FORMAT_ORDER_RAW_Gr) ? "gr" :  \
            (e == SENSOR_FORMAT_ORDER_RAW_R)  ? "r" :   \
            (e == eImgFmt_YUY2) ? "undef" :             \
            "undef"

            //input
#if SUPPORT_MNR
            // Don't dump the input of mnr tuning. It's dumped in previous run.
            if(!params.bRunMnr)
#endif
            if ((!params.bResized && muDumpPortIn & P2_DEBUG_PORT_IN_IMGO) ||
                (params.bResized && muDumpPortIn & P2_DEBUG_PORT_IN_RRZO))
            {
                sprintf(portname, "%s", params.bResized ? "rrzo" : "imgo");
                pImgBuf = params.in.mHandle->getBuffer();
                for(int i = 0 ; i < 3 ; i ++)
                    strides[i] = pImgBuf->getPlaneCount() > i ? pImgBuf->getBufStridesInBytes(i) : 0;
                {
                    sprintf(filename, P2_DEBUG_DUMP_PATH "/p2-%09d-%04d-%04d-%s-%dx%d-%d_%d_%d-%s-%s.raw",
                        params.uUniqueKey, params.uFrameNo, params.uRequestNo,
                        portname,
                        pImgBuf->getImgSize().w,
                        pImgBuf->getImgSize().h,
                        strides[0], strides[1], strides[2],
                        stringifyImageFormat(pImgBuf->getImgFormat()),
                        stringifySensorFormat(muSensorFormatOrder));

                    MY_LOGI("[raw][in] frame(%d) port(%s) (%dx%d) fmt(0x%x) stride(%d, %d, %d)",
                        params.uFrameNo, portname,
                        pImgBuf->getImgSize().w, pImgBuf->getImgSize().h,
                        pImgBuf->getImgFormat(),
                        strides[0], strides[1], strides[2]);

                    pImgBuf->saveToFile(filename);
                }
            }
#undef  stringifySensorFormat

            //output
            for( size_t i = 0; i < params.vOut.size(); i++ )
            {
                if (muDumpPortOut & P2_DEBUG_PORT_OUT_WDMAO &&
                    params.vOut[i].mPortId == PORT_WDMAO)
                    sprintf(portname, "%s", "wdmao");
                else if (muDumpPortOut & P2_DEBUG_PORT_OUT_WROTO &&
                    params.vOut[i].mPortId == PORT_WROTO)
                    sprintf(portname, "%s", "wroto");
                else if (muDumpPortOut & P2_DEBUG_PORT_OUT_IMG2O &&
                    params.vOut[i].mPortId == PORT_IMG2O)
                    sprintf(portname, "%s", "img2o");
#if SUPPORT_IMG3O_PORT
                else if (muDumpPortOut & P2_DEBUG_PORT_OUT_IMG3O &&
                    params.vOut[i].mPortId == PORT_IMG3O)
                    sprintf(portname, "%s", "img3o");
#endif
                else
                    continue;
                //
                pImgBuf = params.vOut[i].mHandle->getBuffer();
                for(int i = 0 ; i < 3 ; i ++)
                    strides[i] = pImgBuf->getPlaneCount() > i ? pImgBuf->getBufStridesInBytes(i) : 0;
                {
                    sprintf(filename, P2_DEBUG_DUMP_PATH "/p2-%09d-%04d-%04d-%s-%dx%d-%d_%d_%d-%s.yuv",
                        params.uUniqueKey, params.uFrameNo, params.uRequestNo,
                        portname,
                        pImgBuf->getImgSize().w,
                        pImgBuf->getImgSize().h,
                        strides[0], strides[1], strides[2],
                        stringifyImageFormat(pImgBuf->getImgFormat()));

                    MY_LOGI("[yuv][out] frame(%d) port(%s) size(%dx%d) fmt(0x%x) strd(%d, %d, %d)",
                        params.uFrameNo, portname,
                        pImgBuf->getImgSize().w, pImgBuf->getImgSize().h,
                        pImgBuf->getImgFormat(),
                        strides[0], strides[1], strides[2]);

                    pImgBuf->saveToFile(filename);
                }
            }
#undef  stringifyImageFormat
        }
    }

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
onP2Flush()
{
    CAM_TRACE_NAME("P2:[Proc]Flush");
    if(mpMultiFrameHandler)
    {
        mpMultiFrameHandler->flush();
    }
    return;
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
    CAM_TRACE_NAME("P2:[Proc]deque");
    Mutex::Autolock _l(mLock);
    sp<Request> pRequest = NULL;
    {
        MY_LOGD_IF(mbEnableLog, "p2 done %d, success %d", muDequeCnt, rParams.mDequeSuccess);
        pRequest = mvRunning.front();
        mvRunning.erase(mvRunning.begin());
        muDequeCnt++;
        MY_LOGD_IF(mbEnableLog, "p2 done muDequeCnt:%d", muDequeCnt);

        if( mDebugScanLineMask != 0 &&
            mpDebugScanLine != NULL)
        {
            for(size_t i = 0; i < rParams.mvOut.size(); i++)
            {
                if( (   rParams.mvOut[i].mPortID.index == PORT_WDMAO.index &&
                        mDebugScanLineMask & DRAWLINE_PORT_WDMAO) ||
                    (   rParams.mvOut[i].mPortID.index == PORT_WROTO.index &&
                        mDebugScanLineMask & DRAWLINE_PORT_WROTO) ||
                    (   rParams.mvOut[i].mPortID.index == PORT_IMG2O.index &&
                        mDebugScanLineMask & DRAWLINE_PORT_IMG2O)   )
                {
                    mpDebugScanLine->drawScanLine(
                                        rParams.mvOut[i].mBuffer->getImgSize().w,
                                        rParams.mvOut[i].mBuffer->getImgSize().h,
                                        (void*)(rParams.mvOut[i].mBuffer->getBufVA(0)),
                                        rParams.mvOut[i].mBuffer->getBufSizeInBytes(0),
                                        rParams.mvOut[i].mBuffer->getBufStridesInBytes(0));
                }
            }
        }

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
MERROR
P2Procedure::
getCropInfos(
    IMetadata* const inApp,
    IMetadata* const inHal,
    MBOOL const isResized,
    Cropper::crop_info_t& cropInfos
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
MVOID
Cropper::
calcViewAngle(
    MBOOL bEnableLog,
    crop_info_t const& cropInfos,
    MSize const& dstSize,
    MCropRect& result
)
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
    MY_LOGD_IF(bEnableLog, "s_cropRegion(%d, %d, %dx%d), dst %dx%d, view crop(%d, %d, %dx%d)",
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
            Cropper::dump(cropInfos);
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
            Cropper::dump(cropInfos);
        }
    }
    //
    MY_LOGD_IF(bEnableLog, "resized %d, crop %d/%d, %d/%d, %dx%d",
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
Cropper::
refineBoundary(
    MSize const& bufSize,
    MCropRect& crop
)
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
MVOID
Cropper::
dump(
     crop_info_t const& crop
)
{
    MY_LOGD("isResized %d", crop.isResized);
    MY_LOGD("p1 info (%d,%d,%dx%d), (%dx%d), (%d,%d,%dx%d)",
        crop.crop_p1_sensor.p.x,
        crop.crop_p1_sensor.p.y,
        crop.crop_p1_sensor.s.w,
        crop.crop_p1_sensor.s.h,
        crop.dstsize_resizer.w,
        crop.dstsize_resizer.h,
        crop.crop_dma.p.x,
        crop.crop_dma.p.y,
        crop.crop_dma.s.w,
        crop.crop_dma.s.h
    );
    MY_LOGD("tran active to sensor o %d, %d, s %dx%d -> %dx%d",
        crop.tranActive2Sensor.tarOrigin.x,
        crop.tranActive2Sensor.tarOrigin.y,
        crop.tranActive2Sensor.oldScale.w,
        crop.tranActive2Sensor.oldScale.h,
        crop.tranActive2Sensor.newScale.w,
        crop.tranActive2Sensor.newScale.h
    );
    MY_LOGD("tran sensor to resized o %d, %d, s %dx%d -> %dx%d",
        crop.tranSensor2Resized.tarOrigin.x,
        crop.tranSensor2Resized.tarOrigin.y,
        crop.tranSensor2Resized.oldScale.w,
        crop.tranSensor2Resized.oldScale.h,
        crop.tranSensor2Resized.newScale.w,
        crop.tranSensor2Resized.newScale.h
    );
    MY_LOGD("modified active crop %d, %d, %dx%d",
        crop.crop_a.p.x,
        crop.crop_a.p.y,
        crop.crop_a.s.w,
        crop.crop_a.s.h
    );
    MY_LOGD("isEisOn %d", crop.isEisEabled);
    MY_LOGD("mv in active %d/%d, %d/%d",
        crop.eis_mv_a.p.x, crop.eis_mv_a.pf.x,
        crop.eis_mv_a.p.y, crop.eis_mv_a.pf.y
    );
    MY_LOGD("mv in sensor %d/%d, %d/%d",
        crop.eis_mv_s.p.x, crop.eis_mv_s.pf.x,
        crop.eis_mv_s.p.y, crop.eis_mv_s.pf.y
    );
    MY_LOGD("mv in resized %d/%d, %d/%d",
        crop.eis_mv_r.p.x, crop.eis_mv_r.pf.x,
        crop.eis_mv_r.p.y, crop.eis_mv_r.pf.y
    );
}


/******************************************************************************
 *
 ******************************************************************************/
sp<Processor>
MdpProcedure::
createProcessor(InitParams& params)
{
    return new ProcessorBase<MdpProcedure>(params, PROCESSOR_NAME_MDP);
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
MdpProcedure::
onMdpPullParams(sp<Request> pRequest, FrameParams& param_mdp)
{
    if (!pRequest->context.in_mdp_buffer.get() && !pRequest->context.crop_info.get())
    {
        return NOT_ENOUGH_DATA;
    }
    param_mdp.uFrameNo = pRequest->getFrameNo();
    param_mdp.in.mHandle = pRequest->context.in_mdp_buffer;
    param_mdp.pCropInfo = pRequest->context.crop_info;
    pRequest->context.in_mdp_buffer.clear();

    // input&output buffer
    vector<sp<BufferHandle> >::iterator iter = pRequest->context.out_buffers.begin();
    while(iter !=  pRequest->context.out_buffers.end())
    {
        sp<BufferHandle> pOutBuffer = *iter;
        if (pOutBuffer.get() && pOutBuffer->getState() == BufferHandle::STATE_NOT_USED)
        {
            MdpProcedure::FrameOutput out;
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
MdpProcedure::
onMdpExecute(
    sp<Request> const pRequest,
    FrameParams const& params
)
{
    CAM_TRACE_NAME("P2:[MDP]exe");
    FUNC_START;
    //
    MERROR ret = OK;
    //
    IImageBuffer* pSrc = NULL;
    vector<IImageBuffer*> vDst;
    vector<MUINT32> vTransform;
    vector<MRect> vCrop;

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
    for (size_t i = 0; i < params.vOut.size(); i++)
    {
        if (params.vOut[i].mHandle == NULL ||
            OK != (ret = params.vOut[i].mHandle->waitState(BufferHandle::STATE_WRITABLE))) {
            MY_LOGW("dst buffer err = %d", ret);
            continue;
        }
        IImageBuffer* pDst = params.vOut[i].mHandle->getBuffer();
        //
        if (pDst != NULL)
        {
            MCropRect cropRect;
#if 0
            MY_LOGI("dump crop info");
            Cropper::dump(*params.pCropInfo);
#endif
            MINT32 const transform  = params.vOut[i].mTransform;
            MSize dstSize = ( transform & eTransform_ROT_90 )
                            ? MSize(pDst->getImgSize().h, pDst->getImgSize().w)
                            : pDst->getImgSize();

            Cropper::calcViewAngle(mbEnableLog, *params.pCropInfo, dstSize, cropRect);
            MRect crop =
            {
                .p = cropRect.p_integral,
                .s = cropRect.s
            };

            vCrop.push_back(crop);
            vDst.push_back(pDst);
            vTransform.push_back(params.vOut[i].mHandle->getTransform());

            MY_LOGD_IF(DEBUG_LOG, "mdp req:%d out:%d/%d size:%dx%d crop (%d,%d) %dx%d",
                pRequest->getFrameNo(), i, params.vOut.size(),
                pDst->getImgSize().w, pDst->getImgSize().h,
                crop.p.x, crop.p.y, crop.s.w, crop.s.h);
        }
        else
            MY_LOGW("mdp req:%d empty buffer", pRequest->getFrameNo());
    }
    //
    if (pSrc == NULL || vDst.size() == 0) {
        MY_LOGE("wrong mdp in/out: src %p, dst count %d", pSrc, vDst.size());
        return BAD_VALUE;
    }
    //

    MBOOL success = MFALSE;
    {
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
                vCrop[0],
                (vCrop.size() > 1 )? vCrop[1] : NULL,
                vTransform[0],
                (vTransform.size() > 1 )? vTransform[1] : NULL,
                0xFFFFFFFF
                );

        pTrans->destroyInstance();
        pTrans = NULL;
        #endif
    }
    //
    FUNC_END;
    return success ? OK : UNKNOWN_ERROR;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
MdpProcedure::
onMdpFinish(
    FrameParams const& params,
    MBOOL const success
)
{
    CAM_TRACE_NAME("P2:[MDP]Finish");
    //params.in.mHandle->updateState(BufferHandle::Buffer_ReadDone);
    if (muDumpBuffer)
    {
        MY_LOGI("[yuv] frame(%d) size(%d)", params.uFrameNo, params.vOut.size());

        sp<IImageBuffer> pImgBuf = NULL;
        if (!NSCam::Utils::makePath(P2_DEBUG_DUMP_PATH,0660))
            MY_LOGW("makePath[%s] fails", P2_DEBUG_DUMP_PATH);

        // ouput
        char filename[256] = {0};
        for( size_t i = 0; i < params.vOut.size(); i++ )
        {
            pImgBuf = params.vOut[i].mHandle->getBuffer();

            sprintf(filename, P2_DEBUG_DUMP_PATH "/mdp-%04d-out-%dx%d.yuv",
                params.uFrameNo,
                pImgBuf->getImgSize().w, pImgBuf->getImgSize().h);

            MY_LOGI("[yuv][out] %d (%dx%d) fmt(0x%x)",
                params.uFrameNo,
                pImgBuf->getImgSize().w, pImgBuf->getImgSize().h,
                pImgBuf->getImgFormat());

            pImgBuf->saveToFile(filename);
        }
    }

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
onP2PullParams(sp<Request> pRequest, FrameParams& param_p2)
{
#if SUPPORT_MNR || SUPPORT_SWNR
    MBOOL bOutWorkingBuffer = MFALSE;
#endif

    param_p2.uUniqueKey = pRequest->getUniqueKey();
    param_p2.uRequestNo = pRequest->getRequestNo();
    param_p2.uFrameNo   = pRequest->getFrameNo();

    // input buffer
    param_p2.in.mPortId = PORT_IMGI;
    param_p2.bResized = pRequest->context.resized;
#if SUPPORT_SWNR
    MBOOL bSwnrRequest = pRequest->context.nr_type == Request::NR_TYPE_SWNR;
    bOutWorkingBuffer |= bSwnrRequest;
#endif
#if SUPPORT_MNR
    // input buffer(select the source)
    MBOOL bMnrRequest = pRequest->context.nr_type == Request::NR_TYPE_MNR;
    MBOOL bMnrSecondRun = bMnrRequest && pRequest->context.nr_work_buffer.get();
    param_p2.bRunMnr = bMnrSecondRun;
    // output to a working buffer, and set two-run pass2
    if (bMnrRequest && !bMnrSecondRun)
    {
        pRequest->setReentry(MTRUE);
        bOutWorkingBuffer = MTRUE;
    }
    // set the privous working buffer as input buffer
    if (bMnrSecondRun)
    {
        MY_LOGD("input from working buffer");
        param_p2.in.mHandle = pRequest->context.nr_work_buffer;
        pRequest->context.nr_work_buffer.clear();
    }
    else
#endif
    {
        param_p2.in.mHandle = pRequest->context.in_buffer;
        pRequest->context.in_buffer.clear();
    }

    // output buffer
    MUINT8 occupied = 0;
    MBOOL remains = MFALSE;

#if SUPPORT_MNR || SUPPORT_SWNR
    //
    param_p2.bRunWorkBuffer= bOutWorkingBuffer;
    //
    if (bOutWorkingBuffer)
    {
        MY_LOGD("output to working buffer");
        MERROR ret = OK;
        if (OK != (ret = param_p2.in.mHandle->waitState(BufferHandle::STATE_READABLE)))
        {
            MY_LOGW("input buffer err = %d", ret);
            return ret;
        }

        IImageBuffer* pInImageBuffer = param_p2.in.mHandle->getBuffer();
        if (pInImageBuffer == NULL)
        {
            MY_LOGE("no input buffer");
            return UNKNOWN_ERROR;
        }
        //
        sp<BufferHandle> pBufferHandle = WorkingBufferHandle::create(
            (pRequest->context.nr_type == Request::NR_TYPE_SWNR) ? "SWNR" : "MNR",
            (pRequest->context.nr_type == Request::NR_TYPE_SWNR) ? eImgFmt_I420 : eImgFmt_YUY2,
            pInImageBuffer->getImgSize());
        //
        if (pBufferHandle.get())
        {
            // occupied WDMA0 and WROTO, except IMGO2
            occupied |= 0x3;
            //
            pRequest->context.nr_work_buffer = pBufferHandle;
            P2Procedure::FrameOutput out;
                out.mPortId = PORT_WDMAO;
                out.mHandle = pBufferHandle;
                out.mTransform = 0;
            //
            param_p2.vOut.push_back(out);
        }
    }
#endif

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

    if (param_p2.vOut.size() == 0)
    {
        return NOT_ENOUGH_DATA;
    }
    else if (pRequest->isReentry())
    {
        // skip to determine mdp's input buffer if need to re-entry
    }
    else if (remains)
    {
        pRequest->context.in_mdp_buffer = param_p2.vOut[param_p2.vOut.size() - 1].mHandle;
    }

#if SUPPORT_IMG3O_PORT
    if (muDumpPortImg3o)
    {
        MY_LOGD("output img3o");
        MERROR ret = OK;
        if (OK != (ret = param_p2.in.mHandle->waitState(BufferHandle::STATE_READABLE)))
        {
            MY_LOGW("input buffer err = %d", ret);
            return ret;
        }

        IImageBuffer* pInImageBuffer = param_p2.in.mHandle->getBuffer();
        if (pInImageBuffer == NULL)
        {
            MY_LOGE("no input buffer");
            return UNKNOWN_ERROR;
        }

        // format: YUY2
        MUINT32 stridesInBytes[3] = {pInImageBuffer->getImgSize().w << 1, 0, 0};
        //
        sp<BufferHandle> pBufferHandle = WorkingBufferHandle::create(
            "IMG3O_WB",
            eImgFmt_YUY2,
            pInImageBuffer->getImgSize());
        //
        if (pBufferHandle.get())
        {
            P2Procedure::FrameOutput out;
                out.mPortId = PORT_IMG3O;
                out.mHandle = pBufferHandle;
                out.mTransform = 0;
            param_p2.vOut.push_back(out);
        }
    }
#endif

    param_p2.inApp = pRequest->context.in_app_meta;
    param_p2.inHal = pRequest->context.in_hal_meta;
    param_p2.outApp = pRequest->context.out_app_meta;
    param_p2.outHal = pRequest->context.out_hal_meta;

    // pull meta buffers if not re-entry
    if (!pRequest->isReentry())
    {
        pRequest->context.in_app_meta.clear();
        pRequest->context.in_hal_meta.clear();
        pRequest->context.out_app_meta.clear();
        pRequest->context.out_hal_meta.clear();
    }

    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P2Procedure::MultiFrameHandler::
collect(sp<Request> pRequest, QParams& params)
{
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
            MY_LOGI("[burst] Pass2 enque");
            CAM_TRACE_NAME("P2:[Proc]burst drv enq");
            if (mpPipe->enque(enqueParams))
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
    CAM_TRACE_NAME("P2:[Proc]burst deque");
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
    CAM_TRACE_NAME("P2:[Proc]flush");
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
sp<Processor>
SwnrProcedure::
createProcessor(InitParams& params)
{
    return new ProcessorBase<SwnrProcedure>(params, PROCESSOR_NAME_SWNR);
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
SwnrProcedure::
onSwnrPullParams(sp<Request> pRequest, FrameParams& param_nr)
{
    if (pRequest->context.nr_type != Request::NR_TYPE_SWNR)
    {
        return NOT_ENOUGH_DATA;
    }

    param_nr.uFrameNo = pRequest->getFrameNo();

    if (pRequest->context.nr_work_buffer.get())
    {
        param_nr.mpInHandle = pRequest->context.nr_work_buffer;
        pRequest->context.in_mdp_buffer = pRequest->context.nr_work_buffer;
        pRequest->context.nr_work_buffer.clear();

        param_nr.iso = pRequest->context.iso;
    }
    else
    {
        MY_LOGE("no src");
        return NOT_ENOUGH_DATA;
    }

    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
SwnrProcedure::
onSwnrExecute(
    sp<Request> const,
    FrameParams const& params
)
{
    CAM_TRACE_NAME("P2:[Swnr]exe");
    FUNC_START;
    //
    MERROR ret = OK;
    //
    IImageBuffer* pSrc = NULL;
    // input
    if (params.mpInHandle.get())
    {
        if (OK != (ret = params.mpInHandle->waitState(BufferHandle::STATE_READABLE))) {
            MY_LOGW("src buffer err = %d", ret);
            return ret;
        }
        pSrc = params.mpInHandle->getBuffer();
    }
    else {
        MY_LOGW("no src");
        return BAD_VALUE;
    }
    //
    MBOOL success = MTRUE;
    {
        SwNR::SWNRParam swnr_param;
        swnr_param.iso = params.iso;
        //
        if (!mpSwnr->doSwNR(swnr_param, pSrc))
        {
            MY_LOGE("SWNR failed");
            success = MFALSE;
        }
    }
    //
    FUNC_END;
    return success ? OK : UNKNOWN_ERROR;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
SwnrProcedure::
onSwnrFinish(
    FrameParams const& params,
    MBOOL const success
)
{
    CAM_TRACE_NAME("P2:[Swnr]Finish");

    if (muDumpBuffer)
    {
        sp<IImageBuffer> pImgBuf = params.mpInHandle->getBuffer();
        if (!NSCam::Utils::makePath(P2_DEBUG_DUMP_PATH,0660))
            MY_LOGW("makePath[%s] fails", P2_DEBUG_DUMP_PATH);

        // ouput
        char filename[256] = {0};
        sprintf(filename, P2_DEBUG_DUMP_PATH "/swnr-%04d-out-%dx%d.yuv",
        params.uFrameNo,
        pImgBuf->getImgSize().w, pImgBuf->getImgSize().h);

        MY_LOGI("[yuv][out] %d (%dx%d) fmt(0x%x)",
            params.uFrameNo,
            pImgBuf->getImgSize().w, pImgBuf->getImgSize().h,
            pImgBuf->getImgFormat());

        pImgBuf->saveToFile(filename);
    }

    params.mpInHandle->updateState(
        success ? BufferHandle::STATE_WRITE_OK : BufferHandle::STATE_WRITE_FAIL);
    return OK;
}

