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


//

//#include <mtkcam/common.h>

#include "BaseNode.h"
#include <Log.h>


#include <mmsdk/EffectRequest.h>
#include <v3/stream/IStreamInfo.h>
#include <v3/stream/IStreamBuffer.h>
#include <v3/hwnode/DepthMapNode.h>
#include <featurePipe/vsdof/IDepthMapPipe.h>
#include <utils/include/ImageBufferHeap.h>
#include <common/vsdof/effecthal/DepthMapEffectHal.h>
//
#include <utils/RWLock.h>
#include <utils/Thread.h>
#include <utils/KeyedVector.h>
#include <cutils/properties.h>

//
#include <sys/prctl.h>
#include <sys/resource.h>
//
#include <metadata/mtk_platform_metadata_tag.h>

#define LOG_TAG "MtkCam/DepthMapNode"

using namespace android;
using namespace NSCam;
using namespace NSCam::v3;
using namespace NSCam::Utils::Sync;
using namespace NSCam::NSCamFeature::NSFeaturePipe;

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


class DepthMapNodeImp
    : public BaseNode
    , public DepthMapNode

{
public:

    DepthMapNodeImp();
    virtual ~DepthMapNodeImp();
    virtual MERROR config(ConfigParams const& rParams);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  IPipelineNode Interface.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////                    Operations.

    virtual MERROR init(InitParams_Base const& rParams);
    virtual MERROR init(InitParams const& rParams);

    virtual MERROR uninit();

    virtual MERROR flush();

    virtual MERROR queue(sp<IPipelineFrame> pFrame);

protected:  ////                    LOGE & LOGI on/off
    MINT32 mLogLevel;
    MINT32 mOpenId_Main2;
    MINT32 mDumpPass1raw;

protected:

    // meta
    sp<IMetaStreamInfo> mpInAppMeta;
    sp<IMetaStreamInfo> mpInHalMeta;
    sp<IMetaStreamInfo> mpInHalMeta_Main2;
    sp<IMetaStreamInfo> mpOutAppMeta;
    sp<IMetaStreamInfo> mpOutHalMeta;
    // image
    sp<IImageStreamInfo> mpInHalResizeRaw1;
    sp<IImageStreamInfo> mpInHalResizeRaw2;
    sp<IImageStreamInfo> mpInHalFullRaw;

    sp<IImageStreamInfo> mpOutAppImageDepthMap;
    sp<IImageStreamInfo> mpOutHalImageMainImage;
    sp<IImageStreamInfo> mpOutHalImageMainImage_Cap;
    sp<IImageStreamInfo> mpOutHalImageMY_S;
    sp<IImageStreamInfo> mpOutHalImageDMW;
    sp<IImageStreamInfo> mpOutHalImageFD;

    sp<IImageStreamInfo> mpOutHalImageJPSMain1;
    sp<IImageStreamInfo> mpOutHalImageJPSMain2;

    DepthMapEffectHal* mpDepthEffectHal;

    KeyedVector< MUINT32, sp<IPipelineFrame> > mvPipelineFrameMap;

    mutable RWLock                  mConfigRWLock;
    mutable RWLock                  mQueueRWLock;
    bool                            mbInit;



private:
    static MVOID onEffectRequestFinished(MVOID* tag, String8 status, sp<EffectRequest>& request);

    MERROR onEffectReqDone(String8 status, sp<EffectRequest>& request);

    MERROR retrieveImageBuffer(sp<IPipelineFrame> const& pFrame, StreamId_T const streamId,
                                MBOOL toRead, sp<IImageBuffer>& rpImageBuffer);

    MERROR retrieveMetadata(sp<IPipelineFrame> const& pFrame, StreamId_T const streamId,
                                MBOOL toRead, IMetadata*& rpMetadata);

    MERROR releaseImageBuffer(sp<IPipelineFrame> const& pFrame, StreamId_T const streamId,
                                sp<IImageBuffer> const& pImageBuffer);

    MERROR releaseMetadata(sp<IPipelineFrame> const& pFrame, StreamId_T const streamId, IMetadata* pMetadata);

    MERROR mapToBufferID(StreamId_T streamId, DepthMapBufferID &rBufID);
    MERROR mapToStreamID(DepthMapBufferID bufID, StreamId_T& rStreamId);

    MERROR unlockBuffInEffectReq(sp<IPipelineFrame> const& pFrame, sp<EffectRequest> request);
    MERROR suspendThisFrame(sp<IPipelineFrame> const& pFrame, sp<EffectRequest> const& request);

    MERROR generateBIDMaps();

    //BID To StreamID Map
    KeyedVector<DepthMapBufferID, StreamId_T> mDepthBIDToStreamIDMap;
    //StreamID to BID Map
    KeyedVector<StreamId_T, DepthMapBufferID> mStreamIDMapToDepthBID;

};

/******************************************************************************
 *
 ******************************************************************************/
sp<DepthMapNode>
DepthMapNode::
createInstance()
{
    return new DepthMapNodeImp();
}


/******************************************************************************
 *
 ******************************************************************************/
DepthMapNodeImp::
DepthMapNodeImp()
    : BaseNode()
    , DepthMapNode()
    , mbInit(MFALSE)
    , mvPipelineFrameMap()

{
    char cLogLevel[PROPERTY_VALUE_MAX];
    ::property_get("debug.camera.log", cLogLevel, "0");
    mLogLevel = atoi(cLogLevel);
    if ( mLogLevel == 0 ) {
        ::property_get("debug.camera.log.depthMap", cLogLevel, "0");
        mLogLevel = atoi(cLogLevel);
    }

    ::property_get("debug.camera.stereo.p1dump", cLogLevel, "0");
    mDumpPass1raw = atoi(cLogLevel);
}


/******************************************************************************
 *
 ******************************************************************************/
DepthMapNodeImp::
~DepthMapNodeImp()
{
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
DepthMapNodeImp::
init(InitParams_Base const& rParams)
{
    MY_LOGE("Error: need to use the DepthMapNode::InitParams!");
    return INVALID_OPERATION;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
DepthMapNodeImp::
init(InitParams const& rParams)
{
    FUNC_START;

    if(mbInit)
    {
        MY_LOGW("Already perform init operations!");
        return INVALID_OPERATION;
    }

    RWLock::AutoWLock _l(mConfigRWLock);

    mbInit = MTRUE;
    mOpenId = rParams.openId;
    mOpenId_Main2 = rParams.openId_main2;
    mNodeId = rParams.nodeId;
    mNodeName = rParams.nodeName;

    //
    MY_LOGD("OpenId %d, nodeId %d, name %s, openID=%d openID_main2=%d",
            getOpenId(), getNodeId(), getNodeName(), mOpenId, mOpenId_Main2);

    mpDepthEffectHal = new DepthMapEffectHal();
    mpDepthEffectHal->init();

    // prepare sensor idx parameters
    sp<EffectParameter> effParam = new EffectParameter();
    effParam->set(SENSOR_IDX_MAIN1, mOpenId);
    effParam->set(SENSOR_IDX_MAIN2, mOpenId_Main2);
    mpDepthEffectHal->setParameters(effParam);

    FUNC_END;
    return OK;
}



/******************************************************************************
 *
 ******************************************************************************/
MERROR
DepthMapNodeImp::
config(ConfigParams const& rParams)
{
    FUNC_START;

    if(!mbInit)
    {
        MY_LOGW("Not perform init operations yet!");
        return INVALID_OPERATION;
    }


    {
        RWLock::AutoWLock _l(mConfigRWLock);
        mpInAppMeta = rParams.pInAppMeta;
        mpInHalMeta = rParams.pInHalMeta;
        mpInHalMeta_Main2 = rParams.pInHalMeta_Main2;
        mpOutAppMeta = rParams.pOutAppMeta;
        mpOutHalMeta = rParams.pOutHalMeta;

        mpInHalResizeRaw1 = rParams.pInResizedRaw1;
        mpInHalResizeRaw2 = rParams.pInResizedRaw2;
        mpInHalFullRaw = rParams.pInFullRaw;

        mpOutAppImageDepthMap = rParams.pAppImageDepthMap;
        mpOutHalImageMainImage = rParams.pHalImageMainImg;
        mpOutHalImageMY_S = rParams.pHalImageMY_S;
        mpOutHalImageDMW = rParams.pHalImageDMW;
        mpOutHalImageFD = rParams.pHalImageFD;

        if(rParams.pHalImageMainImg_Cap != NULL) {
            mpOutHalImageMainImage_Cap = rParams.pHalImageMainImg_Cap;}


        if(rParams.pHalImageJPSMain1 != NULL) {
            mpOutHalImageJPSMain1 = rParams.pHalImageJPSMain1;}

        if(rParams.pHalImageJPSMain2 != NULL) {
            mpOutHalImageJPSMain2 = rParams.pHalImageJPSMain2;}


        mpDepthEffectHal->configure();
        mpDepthEffectHal->start();
    }

    // Generate the BID to StreamID map
    generateBIDMaps();

    FUNC_END;

    return OK;


}

#define ADD_BID_STREAMID_MAP(streamInfo, depthBID)\
    if(streamInfo.get()) \
    { \
        mDepthBIDToStreamIDMap.add(depthBID, streamInfo->getStreamId()); \
        mStreamIDMapToDepthBID.add(streamInfo->getStreamId(), depthBID); \
        MY_LOGD_IF(mLogLevel >= 1, "===========> BID=%d streamID=%#"PRIxPTR"", depthBID, streamInfo->getStreamId());\
    }


/******************************************************************************
 *
 ******************************************************************************/
MERROR
DepthMapNodeImp::
generateBIDMaps()
{
    FUNC_START;

    ADD_BID_STREAMID_MAP(mpInHalResizeRaw1, BID_P2AFM_IN_RSRAW1);
    ADD_BID_STREAMID_MAP(mpInHalResizeRaw2, BID_P2AFM_IN_RSRAW2);
    ADD_BID_STREAMID_MAP(mpInHalFullRaw, BID_P2AFM_IN_FSRAW);
    ADD_BID_STREAMID_MAP(mpOutAppImageDepthMap, BID_OUT_DEPTHMAP);
    ADD_BID_STREAMID_MAP(mpOutHalImageMY_S, BID_OUT_MY_S);
    ADD_BID_STREAMID_MAP(mpOutHalImageDMW, BID_OUT_DMW);
    ADD_BID_STREAMID_MAP(mpOutHalImageMainImage, BID_OUT_MV_F);
    ADD_BID_STREAMID_MAP(mpOutHalImageMainImage_Cap, BID_OUT_MV_F_CAP);
    ADD_BID_STREAMID_MAP(mpOutHalImageJPSMain1, BID_OUT_JPS_MAIN1);
    ADD_BID_STREAMID_MAP(mpOutHalImageJPSMain2, BID_OUT_JPS_MAIN2);
    ADD_BID_STREAMID_MAP(mpOutHalImageFD, BID_OUT_FD);

    ADD_BID_STREAMID_MAP(mpInAppMeta, BID_META_IN_APP);
    ADD_BID_STREAMID_MAP(mpInHalMeta, BID_META_IN_HAL);
    ADD_BID_STREAMID_MAP(mpOutAppMeta, BID_META_OUT_APP);
    ADD_BID_STREAMID_MAP(mpOutHalMeta, BID_META_OUT_HAL);

    FUNC_END;
    return OK;

}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
DepthMapNodeImp::
uninit()
{
    FUNC_START;

    if(!mbInit)
    {
        MY_LOGW("Not perform init operations yet!");
        return INVALID_OPERATION;
    }

    flush();


    {
        RWLock::AutoWLock _l(mConfigRWLock);
        mpDepthEffectHal->abort();
        mpDepthEffectHal->unconfigure();
        mpDepthEffectHal->uninit();
        delete mpDepthEffectHal;
        mbInit = MFALSE;
    }
    FUNC_END;
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
DepthMapNodeImp::
flush()
{
    FUNC_START;
    // TODO: do effectHAL flush

    FUNC_END;
    return OK;
}

#define DECLARE_RELEASE_FRAME(pFrame) \
    android::sp<IPipelineFrame> pPipeFrame = pFrame; \
    sp<IImageBuffer> pImgBuf; \
    sp<EffectFrameInfo> effectFrame; \
    sp<EffectParameter> effParam; \
    IMetadata* metaBuf;

#define RELEASE_IMGBUF(request, targetFrame, stremInfo, frameID) \
    request->targetFrame.valueFor(frameID)->getFrameBuffer(pImgBuf); \
    releaseImageBuffer(pPipeFrame, stremInfo->getStreamId(), pImgBuf);\
    MY_LOGD_IF(mLogLevel >= 1, "release imagebuffer, reqIdx=%d bufID=%d", pPipeFrame->getRequestNo(), frameID);

#define RELEASE_METABUF(request, targetFrame, stremInfo, frameID)  \
    effParam = request->targetFrame.valueFor(frameID)->getFrameParameter(); \
    metaBuf = (IMetadata*) effParam->getPtr(VSDOF_EFFECT_PARAMS_KEY); \
    releaseMetadata(pPipeFrame, stremInfo->getStreamId(), metaBuf); \
    MY_LOGD_IF(mLogLevel >= 1, "release metadata, reqIdx=%d bufID=%d", pPipeFrame->getRequestNo(), frameID);

#define RELEASE_INPUT_IMGBUF(request, streaminfo, frameID) \
    RELEASE_IMGBUF(request, vInputFrameInfo, streaminfo, frameID)

#define RELEASE_OUTPUT_IMGBUF(request, streaminfo, frameID) \
    RELEASE_IMGBUF(request, vOutputFrameInfo, streaminfo, frameID)

#define RELEASE_INPUT_METABUF(request, streaminfo, frameID) \
    RELEASE_METABUF(request, vInputFrameInfo, streaminfo, frameID)

#define RELEASE_OUTPUT_METABUF(request, streaminfo, frameID) \
    RELEASE_METABUF(request, vOutputFrameInfo, streaminfo, frameID)
/******************************************************************************
 *
 ******************************************************************************/
MVOID
DepthMapNodeImp::
onEffectRequestFinished(MVOID* tag, String8 status, sp<EffectRequest>& request)
{
    FUNC_START;
    DepthMapNodeImp *depthMapNode = (DepthMapNodeImp *) tag;
    depthMapNode->onEffectReqDone(status, request);
    FUNC_END;
}


MERROR
DepthMapNodeImp::
onEffectReqDone(String8 status, sp<EffectRequest>& request)
{

    MUINT32 reqID = request->getRequestNo();

    MY_LOGD_IF(mLogLevel >= 1, "+ , reqId=%d", reqID);

    sp<IPipelineFrame> pFrame;

    {
        RWLock::AutoWLock _l(mQueueRWLock);
        pFrame = mvPipelineFrameMap.valueFor(reqID);
        mvPipelineFrameMap.removeItem(reqID);
    }

    // ------------ release the in/out stream buffer  releaseImageBuffer -----------
    DECLARE_RELEASE_FRAME(pFrame);

    IPipelineFrame::InfoIOMapSet IOMapSet;

    if( OK != pFrame->queryInfoIOMapSet( getNodeId(), IOMapSet ) ) {
        MY_LOGE("queryInfoIOMap failed");
        return NAME_NOT_FOUND;
    }

    // get image io
    IPipelineFrame::ImageInfoIOMapSet& imageIOMapSet = IOMapSet.mImageInfoIOMapSet;
    IPipelineFrame::ImageInfoIOMap const& imageIOMap = imageIOMapSet[0];

    MY_LOGD_IF(mLogLevel >= 1, "ImageBuffer: vIn size=%d, vOut size=%d", imageIOMap.vIn.size(), imageIOMap.vOut.size());

    // input image stream
    for(size_t index=0; index<imageIOMap.vIn.size(); index++)
    {
        StreamId_T streamId = imageIOMap.vIn.keyAt(index);
        android::sp<IImageStreamInfo> streamInfo = imageIOMap.vIn.valueAt(index);
        DepthMapBufferID bufId;
        MERROR ret = mapToBufferID(streamId, bufId);

        if (ret != OK)
            return BAD_VALUE;

        RELEASE_INPUT_IMGBUF(request, streamInfo, bufId);
    }
    // output image stream
    for(size_t index=0; index<imageIOMap.vOut.size(); index++)
    {
        StreamId_T streamId = imageIOMap.vOut.keyAt(index);
        android::sp<IImageStreamInfo> streamInfo = imageIOMap.vOut.valueAt(index);
        DepthMapBufferID bufId;
        MERROR ret = mapToBufferID(streamId, bufId);

        if (ret != OK)
            return BAD_VALUE;

        RELEASE_OUTPUT_IMGBUF(request, streamInfo, bufId);
    }

    //get meta io
    IPipelineFrame::MetaInfoIOMapSet& metaIOMapSet =IOMapSet.mMetaInfoIOMapSet;

    IPipelineFrame::MetaInfoIOMap const& metaIOMap = metaIOMapSet[0];

    MY_LOGD_IF(mLogLevel >= 1, "Metadata: vIn size=%d, vOut size=%d", metaIOMap.vIn.size(), metaIOMap.vOut.size());

    // input meta stream
    for(size_t index=0; index<metaIOMap.vIn.size(); index++)
    {
        StreamId_T streamId = metaIOMap.vIn.keyAt(index);
        android::sp<IMetaStreamInfo> streamInfo = metaIOMap.vIn.valueAt(index);
        DepthMapBufferID bufId;
        MERROR ret = mapToBufferID(streamId, bufId);

        if (ret != OK)
            return BAD_VALUE;

        RELEASE_INPUT_METABUF(request, streamInfo, bufId);
    }
    // output meta stream
    for(size_t index=0; index<metaIOMap.vOut.size(); index++)
    {
        StreamId_T streamId = metaIOMap.vOut.keyAt(index);
        android::sp<IMetaStreamInfo> streamInfo = metaIOMap.vOut.valueAt(index);
        DepthMapBufferID bufId;
        MERROR ret = mapToBufferID(streamId, bufId);

        if (ret != OK)
            return BAD_VALUE;

        RELEASE_OUTPUT_METABUF(request, streamInfo, bufId);
    }

    // dispatch
    MY_LOGD_IF(mLogLevel >= 1, "DepthMapNode: applyRelease reqID=%d", reqID);
    pFrame->getStreamBufferSet().applyRelease(getNodeId());
    MY_LOGD_IF(mLogLevel >= 1, "DepthMapNode: onDispatchFrame reqID=%d", reqID);
    onDispatchFrame(pFrame);
    MY_LOGD_IF(mLogLevel >= 1, "-");

    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
DepthMapNodeImp::
unlockBuffInEffectReq(sp<IPipelineFrame> const& pFrame, sp<EffectRequest> request)
{
    FUNC_START;
    IStreamBufferSet& rStreamBufferSet  = pFrame->getStreamBufferSet();
    DefaultKeyedVector<frame_no, sp<EffectFrameInfo> >* framInfoMapArr[]  = {&request->vInputFrameInfo,
                                                                            &request->vOutputFrameInfo};

    MERROR ret;
    StreamId_T streamId;
    sp<IImageBuffer> pImageBuffer;
    IMetadata* pMetaData;
    for(int i=0;i<2;i++)
    {
        for(size_t index=0; index<framInfoMapArr[i]->size();index++)
        {
            DepthMapBufferID bufID = (DepthMapBufferID)framInfoMapArr[i]->keyAt(index);
            sp<EffectFrameInfo> frameInfo = framInfoMapArr[i]->valueAt(index);
            ret = mapToStreamID(bufID, streamId);
            if(ret != OK)
                return BAD_VALUE;

            // if image buffer
            sp<IImageStreamBuffer> pStreamBuffer = rStreamBufferSet.getImageBuffer(streamId, getNodeId());
            if (pStreamBuffer != 0)
            {
                frameInfo->getFrameBuffer(pImageBuffer);
                pImageBuffer->unlockBuf(getNodeName());
                pStreamBuffer->unlock(getNodeName(), pImageBuffer->getImageBufferHeap());
                continue;
            }
            //if metadata
            sp<IMetaStreamBuffer> pMetaStreamBuffer = rStreamBufferSet.getMetaBuffer(streamId, getNodeId());
            if (pMetaStreamBuffer != 0)
            {
                pMetaData = reinterpret_cast<IMetadata*>(frameInfo->getFrameParameter()->getPtr(VSDOF_EFFECT_PARAMS_KEY));
                pMetaStreamBuffer->unlock(getNodeName(), pMetaData);
            }
            else
            {
                MY_LOGW("Stream ID=%#"PRIxPTR" not found, cannot unlock!", streamId);
            }

        }
    }
    FUNC_END;
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
DepthMapNodeImp::
suspendThisFrame(android::sp<IPipelineFrame> const& pFrame, sp<EffectRequest> const& request)
{
    MY_LOGE("Discard request id=%d", pFrame->getRequestNo());

    //unlock the current buffer inside the EffectRequest
    unlockBuffInEffectReq(pFrame, request);

    MERROR err = BaseNode::flush(pFrame);

    return err;
}


#define DECLARE_EFFECT_REQ_PACKING(pFrame, effectReq) \
    android::sp<IPipelineFrame> pPipeFrame = pFrame; \
    MUINT32 reqIdx = pFrame->getRequestNo(); \
    effectReq = new EffectRequest(reqIdx, onEffectRequestFinished, (void*)this); \
    sp<IImageBuffer> imgBuf; \
    sp<EffectFrameInfo> effectFrame; \
    IMetadata* metaBuf; \
    MY_LOGD("+: reqID=%d", reqIdx);

#define ADD_IMGBUF_FRAME(res, frameMapName, streamInfo, frameId, toRead) \
    res = retrieveImageBuffer(pPipeFrame, streamInfo->getStreamId(), toRead, imgBuf); \
    if(res == OK) \
    { \
        effectFrame = new EffectFrameInfo(reqIdx, frameId); \
        effectFrame->setFrameBuffer(imgBuf); \
        effectReq->frameMapName.add(frameId, effectFrame); \
        MY_LOGD_IF(mLogLevel >= 2, "add imagebuffer into request, reqIdx=%d bufID=%d", reqIdx, frameId); \
    }else \
    { \
        MY_LOGE("retrieve imagebuffer failed!! res=%d BufID=%d reqIdx=%d", res, frameId, reqIdx); \
    }

#define ADD_METABUF_FRAME(res, frameMapName, streamInfo, frameId, toRead) \
    res = retrieveMetadata(pPipeFrame, streamInfo->getStreamId(), toRead, metaBuf); \
    if(res == OK) \
    { \
        effectFrame = new EffectFrameInfo(reqIdx, frameId); \
        sp<EffectParameter> effParam = new EffectParameter(); \
        effParam->setPtr(VSDOF_EFFECT_PARAMS_KEY, (void*)metaBuf); \
        effectFrame->setFrameParameter(effParam); \
        effectReq->frameMapName.add(frameId, effectFrame); \
        MY_LOGD_IF(mLogLevel >= 2, "add metadata into request, reqIdx=%d bufID=%d", reqIdx, frameId); \
    }else \
    { \
        MY_LOGE("retrieve metadata failed!! res=%d BufID=%d reqIdx=%d", res, frameId, reqIdx); \
    }


#define ADD_IMGBUF_INPUT_FRAME(res, streamInfo, frameId)  \
    ADD_IMGBUF_FRAME(res, vInputFrameInfo, streamInfo, frameId, MTRUE)

#define ADD_IMGBUF_OUT_FRAME(res, streamInfo, frameId)  \
    ADD_IMGBUF_FRAME(res, vOutputFrameInfo, streamInfo, frameId, MFALSE)

#define ADD_METABUF_INPUT_FRAME(res, streamInfo, frameId)  \
    ADD_METABUF_FRAME(res, vInputFrameInfo, streamInfo, frameId, MTRUE)

#define ADD_METABUF_OUTPUT_FRAME(res, streamInfo, frameId)  \
    ADD_METABUF_FRAME(res, vOutputFrameInfo, streamInfo, frameId, MFALSE)



/******************************************************************************
 *
 ******************************************************************************/
MERROR
DepthMapNodeImp::
queue(android::sp<IPipelineFrame> pFrame)
{


    if( ! pFrame.get() ) {
        MY_LOGE("Null frame");
        return BAD_VALUE;
    }

    IPipelineFrame::InfoIOMapSet IOMapSet;

    if( OK != pFrame->queryInfoIOMapSet( getNodeId(), IOMapSet ) ) {
        MY_LOGE("queryInfoIOMap failed");
        return NAME_NOT_FOUND;
    }



    // ----- EffectRequest packing ----
    sp<EffectRequest> effectReq;
    DECLARE_EFFECT_REQ_PACKING(pFrame, effectReq);



    // get image io
    IPipelineFrame::ImageInfoIOMapSet& imageIOMapSet = IOMapSet.mImageInfoIOMapSet;

    // IOMapSet size should be only 1
    if(imageIOMapSet.size() != 1) {
        MY_LOGE("imageIOMapSet size should be 1. size=%d", imageIOMapSet.size());
        return BAD_VALUE;
    }

    IPipelineFrame::ImageInfoIOMap const& imageIOMap = imageIOMapSet[0];


    // input image stream
    for(size_t index=0; index<imageIOMap.vIn.size(); index++)
    {
        StreamId_T streamId = imageIOMap.vIn.keyAt(index);
        android::sp<IImageStreamInfo> streamInfo = imageIOMap.vIn.valueAt(index);
        DepthMapBufferID bufId;
        MERROR ret = mapToBufferID(streamId, bufId);

        if (ret != OK)
            return BAD_VALUE;

        ADD_IMGBUF_INPUT_FRAME(ret, streamInfo, bufId);
        if (ret != OK)
        {
            suspendThisFrame(pFrame, effectReq);
            return BAD_VALUE;
        }

        MY_LOGD_IF(mLogLevel >= 2, "input imagBuf stream bid=%d",bufId);
    }

    // check this request is rec or pv.
    bool bIsExistRecYUV = false;
    bool bIsExistPVYUV = false;

    // output image stream
    for(size_t index=0; index<imageIOMap.vOut.size(); index++)
    {
        StreamId_T streamId = imageIOMap.vOut.keyAt(index);
        android::sp<IImageStreamInfo> streamInfo = imageIOMap.vOut.valueAt(index);
        DepthMapBufferID bufId;
        MERROR ret = mapToBufferID(streamId, bufId);

        if (ret != OK)
            return BAD_VALUE;

        ADD_IMGBUF_OUT_FRAME(ret, streamInfo, bufId);
        if (ret != OK)
        {
            suspendThisFrame(pFrame, effectReq);
            return BAD_VALUE;
        }
       MY_LOGD_IF(mLogLevel >= 2, "output imagBuf stream bid=%d",bufId);
    }
    //get meta io
    IPipelineFrame::MetaInfoIOMapSet& metaIOMapSet =IOMapSet.mMetaInfoIOMapSet;

    // metaIOMapSet size should be only 1
    if(metaIOMapSet.size() != 1) {
        MY_LOGE("metaIOMapSet size should be 1. size=%d", metaIOMapSet.size());
        return BAD_VALUE;
    }

    IPipelineFrame::MetaInfoIOMap const& metaIOMap = metaIOMapSet[0];

    // input meta stream
    for(size_t index=0; index<metaIOMap.vIn.size(); index++)
    {
        StreamId_T streamId = metaIOMap.vIn.keyAt(index);
        android::sp<IMetaStreamInfo> streamInfo = metaIOMap.vIn.valueAt(index);
        DepthMapBufferID bufId;
        MERROR ret = mapToBufferID(streamId, bufId);

        if (ret != OK)
            return BAD_VALUE;

        if(mpInHalMeta->getStreamId() == streamId)
        {
            if(retrieveMetadata(pPipeFrame, streamId, MTRUE, metaBuf) == OK)
            {
                IMetadata *tempMetadata = new IMetadata();
                if( ! tryGetMetadata<IMetadata>(const_cast<IMetadata*>(metaBuf), MTK_P1NODE_MAIN2_HAL_META, *tempMetadata) ){
                    MY_LOGE("cannot get MTK_P1NODE_MAIN2_HAL_META after updating request");
                }
                effectFrame = new EffectFrameInfo(reqIdx, BID_META_IN_HAL);
                sp<EffectParameter> effParam = new EffectParameter();
                effParam->setPtr(VSDOF_EFFECT_PARAMS_KEY, (void*)metaBuf);
                effectFrame->setFrameParameter(effParam);
                effectReq->vInputFrameInfo.add(BID_META_IN_HAL, effectFrame);
                //
                effectFrame = new EffectFrameInfo(reqIdx, BID_META_IN_HAL_MAIN2);
                effParam = new EffectParameter();
                effParam->setPtr(VSDOF_EFFECT_PARAMS_KEY, (void*)tempMetadata);
                effectFrame->setFrameParameter(effParam);
                effectReq->vInputFrameInfo.add(BID_META_IN_HAL_MAIN2, effectFrame);
            }
            else
            {
                MY_LOGE("get mpInHalMeta metadata fail.");
                suspendThisFrame(pFrame, effectReq);
                return BAD_VALUE;
            }
        }
        else
        {
            ADD_METABUF_INPUT_FRAME(ret, streamInfo, bufId);
            if (ret != OK)
            {
                suspendThisFrame(pFrame, effectReq);
                return BAD_VALUE;
            }
        }
        MY_LOGD_IF(mLogLevel >= 2, "input metaBuf stream bid=%d",bufId);
    }
    // output meta stream
    for(size_t index=0; index<metaIOMap.vOut.size(); index++)
    {
        StreamId_T streamId = metaIOMap.vOut.keyAt(index);
        android::sp<IMetaStreamInfo> streamInfo = metaIOMap.vOut.valueAt(index);
        DepthMapBufferID bufId;
        MERROR ret = mapToBufferID(streamId, bufId);

        if (ret != OK)
            return BAD_VALUE;

        ADD_METABUF_OUTPUT_FRAME(ret, streamInfo, bufId);
        if (ret != OK)
        {
            suspendThisFrame(pFrame, effectReq);
            return BAD_VALUE;
        }
        MY_LOGD_IF(mLogLevel >= 2, "output metaBuf stream bid=%d",bufId);
    }

    {
        RWLock::AutoWLock _l(mQueueRWLock);
        //record the to-be-enque frame
        mvPipelineFrameMap.add(reqIdx, pFrame);
    }

    // decide the scenario
    sp<EffectParameter> pEffectParam = new EffectParameter();


    DepthNodeOpState state;
    if(effectReq->vOutputFrameInfo.indexOfKey(BID_OUT_MV_F_CAP) >=0)
        state = STATE_CAPTURE;
    else
        state = STATE_NORMAL;
    // set state to EffectParameter
    pEffectParam->set(DEPTHMAP_REQUEST_STATE_KEY, state);
    // set effect parameter to request
    effectReq->setRequestParameter(pEffectParam);
    // enque the effect Request
    mpDepthEffectHal->updateEffectRequest(effectReq);

    if(mDumpPass1raw > 0){mDumpPass1raw --;}


    FUNC_END;
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
DepthMapNodeImp::
mapToStreamID(DepthMapBufferID bufID, StreamId_T& rStreamId)
{
    MY_LOGD_IF(mLogLevel >= 1, "mapToStreamID: bufID=%d", bufID);

    if(mDepthBIDToStreamIDMap.indexOfKey(bufID)>=0)
    {
        rStreamId = mDepthBIDToStreamIDMap.valueFor(bufID);
        return OK;
    }
    else
    {
        MY_LOGE("mapToStreamID Failed!!! bufID=%d", bufID);
        return BAD_VALUE;
    }
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
DepthMapNodeImp::
mapToBufferID(StreamId_T streamId, DepthMapBufferID &rBufID)
{



    if(mStreamIDMapToDepthBID.indexOfKey(streamId)>=0)
    {
        rBufID = mStreamIDMapToDepthBID.valueFor(streamId);
        MY_LOGD_IF(mLogLevel >= 1, "mapToBufferID: streamId=%#"PRIxPTR" BufID=%d", streamId, rBufID);
        return OK;
    }
    else
    {
        MY_LOGE("mapToBufferID Failed!!! streamId=%#"PRIxPTR"", streamId);
        return BAD_VALUE;
    }
}
/******************************************************************************
 *
 ******************************************************************************/
MERROR
DepthMapNodeImp::
retrieveImageBuffer(android::sp<IPipelineFrame> const& pFrame, StreamId_T const streamId, MBOOL toRead, sp<IImageBuffer>& rpImageBuffer)
{
    IStreamBufferSet& rStreamBufferSet  = pFrame->getStreamBufferSet();

    sp<IImageStreamBuffer>  pImageStreamBuffer = NULL;

    MERROR const err = ensureImageBufferAvailable_(
        pFrame->getFrameNo(),
        streamId,
        rStreamBufferSet,
        pImageStreamBuffer
    );

    if(err != OK)
    {
        MY_LOGE("streamId: %#" PRIxPTR ", err = %d , failed to get pImageStreamBuffer! ", streamId, err);
        return err;
    }

    MUINT groupUsage = pImageStreamBuffer->queryGroupUsage(getNodeId());
    sp<IImageBufferHeap> pImageBufferHeap = toRead ?
        pImageStreamBuffer->tryReadLock(getNodeName()) :
        pImageStreamBuffer->tryWriteLock(getNodeName());

    if(pImageBufferHeap == NULL)
    {
        MY_LOGE("[node:%#" PRIxPTR "][stream buffer:%s] cannot get ImageBufferHeap",
                getNodeId(), pImageStreamBuffer->getName());
        return BAD_VALUE;
    }

    rpImageBuffer = pImageBufferHeap->createImageBuffer();
    if (rpImageBuffer == NULL) {
        MY_LOGE("[node:%#" PRIxPTR "][stream buffer:%s] cannot create ImageBuffer",
                getNodeId(), pImageStreamBuffer->getName());
        return BAD_VALUE;
    }
    MBOOL bRet = rpImageBuffer->lockBuf(getNodeName(), groupUsage);
    if(!bRet)
    {
        MY_LOGE("LockBuf failed, stream%#" PRIxPTR "!", streamId);
        return BAD_VALUE;
    }

    MY_LOGD_IF(mLogLevel >= 1, "stream %#" PRIxPTR ": buffer: %p, usage: %p",
        streamId, rpImageBuffer.get(), groupUsage);

    // dump pass1 raw
    if(mDumpPass1raw != 0 &&
        (
            (streamId==mpInHalFullRaw->getStreamId())    ||
            (streamId==mpInHalResizeRaw1->getStreamId()) ||
            (streamId==mpInHalResizeRaw2->getStreamId())
        )
    ){
        MY_LOGD("dump p1 raw => number left to dump %d ( <0 means dump all)", mDumpPass1raw);

        char prefix[256];
        char filename[256];

        if(streamId==mpInHalFullRaw->getStreamId())
        {
            sprintf(prefix, "/sdcard/vsdof/full_");
        }
        if(streamId==mpInHalResizeRaw1->getStreamId())
        {
            sprintf(prefix, "/sdcard/vsdof/resized_");
        }
        if(streamId==mpInHalResizeRaw2->getStreamId())
        {
            sprintf(prefix, "/sdcard/vsdof/resized_main2_");
        }

        sprintf(filename, "%s_%dx%d_%d_frame_%d.raw",
                prefix,
                rpImageBuffer->getImgSize().w,
                rpImageBuffer->getImgSize().h,
                rpImageBuffer->getBufStridesInBytes(0),
                pFrame->getFrameNo()
            );
        NSCam::Utils::makePath("/sdcard/vsdof/",0660);
        NSCam::Utils::saveBufToFile(filename, (unsigned char*)rpImageBuffer->getBufVA(0), rpImageBuffer->getBufSizeInBytes(0));
    }
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
DepthMapNodeImp::
releaseImageBuffer(android::sp<IPipelineFrame> const& pFrame, StreamId_T const streamId, sp<IImageBuffer> const& pImageBuffer)
{

    IStreamBufferSet& rStreamBufferSet  = pFrame->getStreamBufferSet();

    sp<IImageStreamBuffer> pStreamBuffer = rStreamBufferSet.getImageBuffer(streamId, getNodeId());

    pImageBuffer->unlockBuf(getNodeName());
    pStreamBuffer->unlock(getNodeName(), pImageBuffer->getImageBufferHeap());

    pStreamBuffer->markStatus(STREAM_BUFFER_STATUS::WRITE_OK);
    //
    //  Mark this buffer as USED by this user.
    //  Mark this buffer as RELEASE by this user.
    rStreamBufferSet.markUserStatus(
            streamId,
            getNodeId(),
            IUsersManager::UserStatus::USED|IUsersManager::UserStatus::RELEASE);
    return OK;
}



/******************************************************************************
 *
 ******************************************************************************/
MERROR
DepthMapNodeImp::
retrieveMetadata(android::sp<IPipelineFrame> const& pFrame, StreamId_T const streamId, MBOOL toRead, IMetadata*& rpMetadata)
{
    IStreamBufferSet& rStreamBufferSet  = pFrame->getStreamBufferSet();

    sp<IMetaStreamBuffer>  pMetaStreamBuffer = NULL;

    MERROR const err = ensureMetaBufferAvailable_(
        pFrame->getFrameNo(),
        streamId,
        rStreamBufferSet,
        pMetaStreamBuffer
    );

    if(err != OK)
    {
        MY_LOGE("streamId: %#" PRIxPTR " err=%d, failed to get pMetaStreamBuffer!", streamId, err);
        return BAD_VALUE;
    }

    rpMetadata = toRead ?
        pMetaStreamBuffer->tryReadLock(getNodeName()) :
        pMetaStreamBuffer->tryWriteLock(getNodeName());


    if(rpMetadata == NULL)
    {
        MY_LOGE("[node:%#" PRIxPTR "][stream buffer:%s] cannot get Metadata",
                getNodeId(), pMetaStreamBuffer->getName());

    }


    return OK;

}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
DepthMapNodeImp::
releaseMetadata(android::sp<IPipelineFrame> const& pFrame, StreamId_T const streamId, IMetadata* const pMetadata)
{
    IStreamBufferSet& rStreamBufferSet  = pFrame->getStreamBufferSet();
    sp<IMetaStreamBuffer>  pMetaStreamBuffer = rStreamBufferSet.getMetaBuffer(streamId, getNodeId());

    pMetaStreamBuffer->unlock(getNodeName(), pMetadata);
    pMetaStreamBuffer->markStatus(STREAM_BUFFER_STATUS::WRITE_OK);
    rStreamBufferSet.markUserStatus(streamId, getNodeId(), IUsersManager::UserStatus::USED|IUsersManager::UserStatus::RELEASE);

    return OK;
}

