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
 * MediaTek Inc. (C) 2015. All rights reserved.
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
#ifndef _MTK_PLATFORM_HARDWARE_INCLUDE_MTKCAM_V3_HWNODE_MFCNODEIMP_H_
#define _MTK_PLATFORM_HARDWARE_INCLUDE_MTKCAM_V3_HWNODE_MFCNODEIMP_H_

#include <utils/String8.h>
#include <utils/RWLock.h>

#include <iopipe/PostProc/IPortEnum.h>
#include <v3/stream/IStreamBuffer.h>
#include <v3/utils/streambuf/IStreamBufferPool.h>
#include <Hal3/aaa_hal_common.h> // NS3Av3::CaptureParam_T
#include <iopipe/Port.h> // NSCam::NSIoPipe::PortID

#include <v3/hwnode/MFCNode.h>

#include "BaseNode.h"

using namespace android;
using namespace std;
using NSCam::NSIoPipe::PortID;
using NS3Av3::CaptureParam_T;

// ---------------------------------------------------------------------------

namespace NSCam {
namespace v3 {

/**
 *  Multi-Frame Capture Node Implementation (MFCNodeImp) is a super class for the
 *  implementations of HDRN and MFNR hence lots of common parts can be re-used
 *  for multi-frame capture node. Since MFCNodeImp is also a pure virtual class
 *  which means that user who can not use MFCNodeImp directly either but may use
 *  derived classes. So derived classes have responsibility to implement the pure
 *  virtual interfaces.
 */
class MFCNodeImp
    : public MFCNode    /* Implement this */
    , public BaseNode   /* Using common parts */
{
protected:
    virtual ~MFCNodeImp();

/* typedefs & enumerations */
public:
    /**
     *  Middleware frame information (for input)
     *  To determine what kind of IImageBuffer is, to see StreamId_T
     *
     *  eSTREAMID_IMAGE_PIPE_RAW_OPAQUE_00:
     *      Full size RAW.
     *
     *  eSTREAMID_IMAGE_PIPE_RAW_RESIZER_00,
     *      Resized RAW.
     *
     */
    typedef struct _FrameInput {
        PortID                  mPortId;

        StreamId_T              streamId;

        sp<IImageStreamBuffer>  pStreamBuffer;
        sp<IImageBufferHeap>    pImageBufferHeap;
        sp<IImageBuffer>        pImageBuffer;
    } FrameInput;

    /**
     *  Middleware frame information (for output)
     *  To determine what kind of IImageBuffer is, to see StreamId_T.
     *
     *  eSTREAMID_IMAGE_PIPE_YUV_JPEG_00:
     *      YUV buffer for output, usually pass to JPEG node.

     *  eSTREAMID_IMAGE_PIPE_YUV_THUMBNAIL_00
     *      Thumbnail YUV buffer, usually pass to JPEG node.
     */
    typedef struct _FrameOutput {
        PortID                  mPortId;

        StreamId_T              streamId;
        MINT32                  mTransform;

        sp<IImageStreamBuffer>  pStreamBuffer;
        sp<IImageBufferHeap>    pImageBufferHeap;
        sp<IImageBuffer>        pImageBuffer;
    } FrameOutput;

    /**
     *  Frame parameters of IPipelineFrame, the first attribute pFrame hosts the
     *  reference of IPipelineFrame from middleware, and other information will
     *  be retrieved while queued the frame
     */
    typedef struct _FrameParams {
        sp<IPipelineFrame>      pFrame;
        MINT32                  enqueIndex;
        Vector<FrameInput*>   vpIn;   // full_raw, resized_raw
        Vector<FrameOutput*>  vpOut;  // yuv_jpeg, tb_jpeg, yuv_preview

        //MBOOL                  bResized;

        IMetadata               *pMeta_InApp;
        IMetadata               *pMeta_InHal;
        IMetadata               *pMeta_OutApp;
        IMetadata               *pMeta_OutHal;
        sp<IMetaStreamBuffer>   pMeta_InAppStreamBuffer;
        sp<IMetaStreamBuffer>   pMeta_InHalStreamBuffer;
        sp<IMetaStreamBuffer>   pMeta_OutAppStreamBuffer;
        sp<IMetaStreamBuffer>   pMeta_OutHalStreamBuffer;

        _FrameParams()
            : enqueIndex(-1)
            , pMeta_InApp(NULL)
            , pMeta_InHal(NULL)
            , pMeta_OutApp(NULL)
            , pMeta_OutHal(NULL)
        {}
    } FrameParams;

    /* Identifications for onImageBuffer usage, much simpler for MFCNodeImp */
    typedef enum IMGBUFTP {
        eIMGBUFTP_IN_RAW_FULLSIZE = 0,  // Full size RAW buffer
        eIMGBUFTP_IN_RAW_RESIZED,       // Resized RAW buffer
        eIMGBUFTP_OUT_YUV_JPEG,         // A output YUV for JPEG node encoding
        eIMGBUFTP_OUT_YUV_THUMBNAIL,    // A output YUV for JPEG node as thumbnail
        // TODO: where is postview??

        /* size */
        eIMGBUFTP_SIZE
    } eIMGBUFTP;

    /* Identifications for metadata */
    typedef enum MDATATP {
        eMDATATP_IN_APP,
        eMDATATP_IN_HAL,
        eMDATATP_OUT_APP,
        eMDATATP_OUT_HAL,

        /* size */
        eMDATATP_SIZE
    } eMDATATP;

    enum {
        IN = 1,
        OUT = 1 << 1
    };
/**
 *  Event driven methods, derived classes should implement these
 */
protected:
    /**
     *  MFCNodeImp will invoke this method before anything is inited, and derived class
     *  has chance to modify rParams. If derived class wants to totally reimplement
     *  this method, please override MFCNodeImp::init.
     *
     *  @param rParams      - InitParams from caller
     *  @note               - This method is thread-safe, and it's synchronized call.
     */
    virtual void onInit(InitParams &rParams) = 0;

    /**
     *  MFCNodeImp will invoke this method after invoked MFCNodeImp::flush. If derived
     *  class wants to totally reimplement this method, please override
     *  MFCNodeImp::uninit
     *
     *  @note               - This method is thread-safe, and it's synchronized call.
     */
    virtual void onUninit(void) = 0;

    /**
     *  MFCNodeImp will invoke this method before anything configured therefore derived
     *  class has chance to modify configurations. If derived class wants to totally
     *  reimplement this method, please override MFCNodeImp::config.
     *
     *  @param rParams.
     */
    virtual void onConfig(ConfigParams &rParams) = 0;

    /**
     *  MFCNodeImp will invoke this method while a frame just enqued.
     *  @param f            - IPipelineFrame
     *  @note               - This method is thread-safe, and it's synchronized call.
     */
    virtual void onQueue(sp<IPipelineFrame> &f) = 0;

    /**
     *  MFCNodeImp will invoke this method while flushing frames.
     *  Derived class has responsibility to invoke BaseNode::flush(f).
     *
     *  @param f            - IPipelineFrame
     *  @note               - This method is thread-safe, and it's synchronized call.
     *                        Derived class has responsibility to invoke BaseNode::flush(f)
     */
    virtual void onFlush(sp<IPipelineFrame> &f) = 0;

    /**
     *  MFCNodeImp will invoke this method while a frame is dequeued.
     *  Derived class may invoke MFCNodeImp::processFrame to retrieve FrameParam
     *  information which is much easier to understand.
     *
     *  @param f            - IPipelineFrame instance
     *  @note               - This method is thread-safe, and it's synchronized call.
     */
    virtual void onDequeue(sp<IPipelineFrame> &f) = 0;

    /**
     *  While processing frame by invoking MFCNodeImp::processFrame, this method will
     *  be invoked if get an IImageBuffer from middleware.
     *
     *  @param t            - Type of IImageBuffer is belong to
     *  @param img          - IIamgeBuffer is ready to access.
     */
    virtual void onImageBuffer(const eIMGBUFTP &t, sp<IImageBuffer> &img) = 0;

    /**
     *  While processing frame by invoking MFCNodeImp::processFrame, this method will
     *  be invoked if get an IMetadata from middleware.
     *
     *  @param t            - Type of IMetadata is belong to
     *  @param m            - IMetadata is ready to be accessed
     */
    virtual void onMetadata(const eMDATATP &t, IMetadata *m) = 0;

/**
 *  IPipelineNode interfaces, MFCNodeImp Implementations, common parts.
 */
public:
    /**
     *  Derived class should set InitParams::nodeId...etc to BaseNode::mNodeId.
     *
     *  @params rParams     - IPipelineFrame::InitParams for initialization.
     *  @return             - Returns OK if no error.
     */
    virtual MERROR init(InitParams const& rParams);

    /**
     *  Middleware invokes this method while the node is going to be released.
     *  To release resource which is no need here.
     *
     *  @return             - Returns OK if no error.
     */
    virtual MERROR uninit(void);

    /**
     *  This is not interface which defined in IPipelineNode but PipelineContex
     *  which will be invoked while the node is being configured.
     *
     *  @param rParams      - MFCNode::ConfigParam for configuring node.
     *  @return             - Returns OK if no error.
     */
    virtual MERROR config(const ConfigParams& rParams);

    /**
     *  While an IPipelineFrame is queued, this mthod will be invoked.
     *  @param pFrame       - IPipelineFrame instance.
     *  @notice             - Middleware invokes IPipelineNode::queue with synchronized
     *                        call which means that you shouldn't block or execute
     *                        complicated operations here or performance will goes low.
     */
    virtual MERROR queue(sp<IPipelineFrame> pFrame);

    /**
     *  Middleware will invoke IPipelineNode::flush to flush all frames.
     *  Node has responsibility to invoke Base::flush to flush all queued
     *  IPipelineFrame instances.
     *
     *  @return             - Returns OK if no error.
     */
    virtual MERROR flush(void); // Implement for IPipelineFrame

    /**
     *  Deque an IPipeline frame from queue, this method will block caller thead
     *  until deque a frame.
     */
    virtual MERROR deque(sp<IPipelineFrame> &pFrame);

    /**
     *  To retrieve information of IPipelineFrame as a FrameParams. Remember that,
     *  method processFrame will create instances FrameInput and FrameOutput to
     *  FrameParams::vpIn and mvFrameParams_t::vpOut, hence caller has responsibility
     *  to delete them by traveling vector, or call MFCNodeImp::releaseFrameParams to
     *  release a FrameParams instance.
     *
     *  @param pFrame           - IPipelineFrame instance
     *  @param prFrameParam     - A FrameParams instance which will hosts
     *                            information of IPipelineFrame.
     *  @note                   - If an error occured during procssing frame,
     *                            onFlush will be invoked to flush the current frame.
     */
    virtual MERROR processFrame(sp<IPipelineFrame> &pFrame, FrameParams *prFrameParam);

    /**
     *  To release ownership of these IPipelineFrame and makes these frames be available
     *  for the next node processing.
     *
     *  @param pFrameParam          - FrameParams instance contains IPipelineFrame
     *                                information which is retrieved by MFCNodeImp::processFrame
     */
    virtual MERROR releaseFrame(FrameParams *pFrameParam);

/**
 *  Useful utils
 */
public:
    /**
     *  To release FrameParams object deeply. If FrameParams is not created by
     *  dynamically allocation, to specify bDeleteSelf to false for releasing resource
     *  of FrameParams only but not included FrameParams object itself.
     *
     *  @param f                - Pointer of FrameParams for releasing.
     *  @param bDeleteSelf      - True for release *f too.
     */
    static void releaseFrameParams(FrameParams *f, bool bDeleteSelf = true);

    /**
     *  This is a helper function that print out stream information
     *
     *  @param str              - Prefix string of log.
     *  @param streamInfo       - IStreamInfo object that to dump
     */
    static void dumpStreamIfExist(const char* str, const sp<IStreamInfo>& streamInfo);

    /**
     *  This is a helper function that print out information of NS3Av3::CaptureParam_T
     *
     *  @param captureParam     - Parameter to be dumped.
     *  @param msg              - const char* to a prfix message, can be NULL.
     */
    static void dumpCaptureParam(const CaptureParam_T &captureParam, const char* msg = NULL);

    /**
     *  A helper function to retrieve string of image format. E.g.: return ".yuy2"
     *
     *  @param f                - eImgFmt enumeration to query
     *  @return                 - Address of const char*.
     */
    static const char* getFileFormatName(const MINT &f);

    /**
     *  To verify the MFCNode::ConfigParams is valid or not. This method will be
     *  invoked in MFCNodeImp::config which means that when PipelineContext is
     *  configuring MFCNode, this method will be used as checking if the configuration
     *  is ok or not. Derived class maybe have to re-implement this method for
     *  it's proposal.
     *
     *  @param rParams          - Parameter to check.
     *  @return                 - Returns OK if no error.
     *  @note                   - Derived class maybe have to re-implement this method
     *                            for it's proposal.
     */
    static MERROR verifyConfigParams(const ConfigParams &rParams);

/* inline & template functions */
public:
    static inline MBOOL isStream(const sp<IStreamInfo> &s, const StreamId_T &id)
    {
        return s.get() && (s->getStreamId() == id);
    }

    /* function to try to get metadata with prototype T */
    template <class T>
    static MBOOL tryGetMetaData(IMetadata *pMetadata, MUINT32 const tag, T &rVal)
    {
        if (pMetadata == NULL) {
            return MFALSE;
        }

        IMetadata::IEntry entry = pMetadata->entryFor(tag);
        if (!entry.isEmpty()) {
            rVal = entry.itemAt(0, Type2Type<T>());
            return MTRUE;
        }

        return MFALSE;
    }

    /* function to update entry with prototype T */
    template <typename T>
    static MVOID updateEntry(IMetadata* pMetadata, MUINT32 const tag, T const& val)
    {
        if (pMetadata == NULL) {
            return;
        }

        IMetadata::IEntry entry(tag);
        entry.push_back(val, Type2Type<T>());
        pMetadata->update(tag, entry);
    }

    /**
     *  function to retrieve GMV from IMetadata of HAL
     *  @param pMetadata        IMetadata
     *  @param [out] x          Retrieved value of x
     *  @param [out] y          Retrieved value of y
     *  @param [out] size       The resized image resolution that for GMV calculating
     *  @return                 If ok returns MTRUE
     *  @note                   The unit of GMV is based on the image resolution
     *                          from size, you need to map the unit from resized
     *                          domain to your domain
     */
    static MBOOL retrieveGmvInfo(IMetadata *pMetadata, int &x, int &y, MSize &size);

/**
 *  Functions that MFCNodeImp will use only, no need to be inherited.
 */
private:
    virtual MERROR getInfoIOMapSet( sp<IPipelineFrame> const& pFrame,
                                    IPipelineFrame::InfoIOMapSet &rIOMapSet,
                                    MINT32 frameIndex);

    /* A function that help you to retrieve ImageBuffer much easier */
    virtual MERROR getImageBuffer(
                    MINT32 type,
                    const StreamId_T &streamId,
                    sp<IImageStreamBuffer> &streamBuffer,
                    sp<IImageBufferHeap> &imageBufferHeap,
                    sp<IImageBuffer> &ImageBuffer);

    virtual MERROR getMetadata(
                    MINT32 type,
                    const StreamId_T &streamId,
                    sp<IMetaStreamBuffer> &streamBuffer,
                    IMetadata *&metadata);

    virtual inline MBOOL isFullRawLocked(StreamId_T const streamId);
    virtual inline MBOOL isResizedRawLocked(StreamId_T const streamId);
    virtual MBOOL isInImageStream(StreamId_T const streamId);
    virtual MBOOL isInMetaStream(StreamId_T const streamId);
    virtual MVOID unlockImage(sp<IImageStreamBuffer> const pStreamBuffer, sp<IImageBuffer> const pImageBuffer);
    virtual MVOID unlockMetadata(sp<IMetaStreamBuffer> const pStreamBuffer, IMetadata* const pMetadata);
    virtual MVOID markImageStream(sp<IPipelineFrame> const& pFrame, sp<IImageStreamBuffer> const pStreamBuffer);
    virtual MVOID markMetaStream(sp<IPipelineFrame> const& pFrame,  sp<IMetaStreamBuffer> const pStreamBuffer);

/**
 *  Attributes
 */
private:
    /* Request queue contains IPipelineFrame queued from middleware */
    mutable Mutex           mRequestQueueLock;
    Condition               mRequestQueueCond;
    IPipelineFrameList_t    mRequestQueue;
    /* Configuration lock */
    mutable RWLock          mConfigRWLock;

/**
 *  Attributes which may be inherited
 */
protected:
    /* Node topology info */
    sp<IMetaStreamInfo>     mpInAppMeta_Request;
    sp<IMetaStreamInfo>     mpInHalMeta_P1;
    sp<IMetaStreamInfo>     mpOutAppMeta_Result;
    sp<IMetaStreamInfo>     mpOutHalMeta_Result;
    sp<IImageStreamInfo>    mpInFullRaw;
    sp<IImageStreamInfo>    mpInResizedRaw;
    ImageStreamInfoSetT     mvOutImages; // output image port

    // log level
    MINT32 mLogLevel;

}; /* class MFCNodeImp */
}; /* namespace v3 */
}; /* namespace NSCam */
#endif/*_MTK_PLATFORM_HARDWARE_INCLUDE_MTKCAM_V3_HWNODE_MFCNODEIMP_H_*/
