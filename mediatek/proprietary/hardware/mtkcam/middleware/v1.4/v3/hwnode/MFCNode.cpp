#include <Log.h>

/* MtkCam related headers */
#include "MFCNodeImp.h"
#include "hwnode_utilities.h"


#include <Hal3/aaa_hal_common.h>
#include <IHal3A.h>

#include <iopipe/PostProc/INormalStream.h>
#include <iopipe/PostProc/IPortEnum.h>
#include <drv/isp_reg.h>

#include <metadata/IMetadataProvider.h>
#include <mtk_platform_metadata_tag.h>
#include <IHalSensor.h>


#include <LegacyPipeline/StreamId.h>

#include <common/eis/eis_ext.h> // for EIS_REGION_INDE


/* andorid */
#include <utils/RWLock.h>
#include <utils/Thread.h>
#include <sys/prctl.h>
#include <sys/resource.h>

/* std */
#include <vector>

#define LOG_TAG "MtkCam/MFCNode"

#define MY_LOGV(fmt, arg...)        CAM_LOGV("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_LOGD("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_LOGI("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_LOGW("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_LOGE("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGA(fmt, arg...)        CAM_LOGA("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGF(fmt, arg...)        CAM_LOGF("[%s] " fmt, __FUNCTION__, ##arg)

#define MY_LOGV_IF(cond, ...)       do { if ( (cond) ) { MY_LOGV(__VA_ARGS__); } }while(0)
#define MY_LOGD_IF(cond, ...)       do { if ( (cond) ) { MY_LOGD(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) ) { MY_LOGI(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) ) { MY_LOGW(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) ) { MY_LOGE(__VA_ARGS__); } }while(0)
#define MY_LOGA_IF(cond, ...)       do { if ( (cond) ) { MY_LOGA(__VA_ARGS__); } }while(0)
#define MY_LOGF_IF(cond, ...)       do { if ( (cond) ) { MY_LOGF(__VA_ARGS__); } }while(0)

using namespace android;
using namespace NSCam;
using namespace NSCam::v3;
using namespace NSCam::Utils::Sync;
using namespace NS3Av3;
using namespace std;
using namespace NSIoPipe;
using namespace NSIoPipe::NSPostProc;

/**
 *  defined in ispio_pipe_ports.h
 *  type(port type) index(port index) inout(0:in/1:out)
 */
static const NSCam::NSIoPipe::PortID PORT_IMGI  (EPortType_Memory, EPipePortIndex_IMGI , 0);
static const NSCam::NSIoPipe::PortID PORT_WDMAO (EPortType_Memory, EPipePortIndex_WDMAO, 1);
static const NSCam::NSIoPipe::PortID PORT_WROTO (EPortType_Memory, EPipePortIndex_WROTO, 1);

android::sp<MFCNode> MFCNode::createInstance(void)
{
    return NULL;
}

MFCNodeImp::~MFCNodeImp(void)
{
}

MERROR MFCNodeImp::init(InitParams const& rParams)
{
    onInit(const_cast<InitParams&>(rParams));
    mOpenId   = rParams.openId;
    mNodeId   = rParams.nodeId;
    mNodeName = rParams.nodeName;

    return OK;
}

MERROR MFCNodeImp::uninit(void)
{
    if (flush() != OK) {
        MY_LOGE("Flush failed");
    }

    onUninit();
    return OK;
}


MERROR MFCNodeImp::config(const ConfigParams& rParams)
{
    CAM_TRACE_CALL();

    MY_LOGD("config param");

    onConfig(const_cast<ConfigParams&>(rParams));

    // check ConfigParams
    MERROR const err = verifyConfigParams(rParams);
    if (err != OK)
    {
        MY_LOGE("verifyConfigParams failed: err = %d", err);
        return err;
    }

    flush();

    // configure streams into node
    {
        RWLock::AutoWLock _l(mConfigRWLock);

        // meta
        mpInAppMeta_Request = rParams.pInAppMeta;
        mpInHalMeta_P1      = rParams.pInHalMeta;
        mpOutAppMeta_Result = rParams.pOutAppMeta;
        mpOutHalMeta_Result = rParams.pOutHalMeta; // not necessary
        // image
        mpInFullRaw         = rParams.vInFullRaw[0];
        mpInResizedRaw      = rParams.pInResizedRaw;
        mvOutImages         = rParams.vOutImage;
    }

    return OK;

}

MERROR MFCNodeImp::queue(sp<IPipelineFrame> pFrame)
{
    CAM_TRACE_NAME("queue(HDR)");

    MY_LOGD("queue frame %p", pFrame.get());

    if (!pFrame.get()) {
        MY_LOGE("null frame");
        return BAD_VALUE;
    }

    Mutex::Autolock _l(mRequestQueueLock);

    // make sure the request with a smaller frame number has a higher priority
    IPipelineFrameList_t::iterator it = mRequestQueue.end();
    for (; it != mRequestQueue.begin();) {
        --it;
        if (0 <= (MINT32)(pFrame->getFrameNo() - (*it)->getFrameNo()))
        {
            ++it;   //insert(): insert before the current node
            break;
        }
    }

    // insert before the current frame
    mRequestQueue.insert(it, pFrame);

    onQueue(pFrame); // invoke event

    CAM_TRACE_INT("request(MFC)", mRequestQueue.size());

    mRequestQueueCond.broadcast();

    return OK;
}

MERROR MFCNodeImp::deque(sp<IPipelineFrame> &rpFrame)
{
    CAM_TRACE_NAME("onDequeueRequest(MFC)");

    Mutex::Autolock _l(mRequestQueueLock);

    // Wait until the queue is not empty or this thread will exit
    // TODO: To use timeout mechanism or async call to avoid deadlock.
    //       but now we're using deadlock for debug purpose.
    while (mRequestQueue.empty()) {
        status_t status = mRequestQueueCond.wait(mRequestQueueLock);
        if (OK != status) {
            MY_LOGW("wait status(%d):%s, mRequestQueue.size(%zu)",
                    status, ::strerror(-status), mRequestQueue.size());
        }
    }

    rpFrame = *mRequestQueue.begin();
    mRequestQueue.erase(mRequestQueue.begin());

    onDequeue(rpFrame);

    CAM_TRACE_INT("request(MFC)", mRequestQueue.size());

    return OK;
}

MERROR MFCNodeImp::flush()
{
    CAM_TRACE_CALL();

    Mutex::Autolock _l(mRequestQueueLock);
    IPipelineFrameList_t::iterator it = mRequestQueue.begin();
    while (it != mRequestQueue.end())
    {
        onFlush(*it);
        BaseNode::flush(*it);
        it = mRequestQueue.erase(it);
        CAM_TRACE_INT("request(MFC)", mRequestQueue.size());
    }

    return OK;
}

MERROR MFCNodeImp::processFrame(sp<IPipelineFrame> &pFrame, FrameParams *prFrameParam)
{
    FrameParams frameParams;
    MERROR err = OK;

    const MUINT32 FRAME_NO = pFrame->getFrameNo();

    IPipelineFrame::InfoIOMapSet infoIOMapSet;
    if (getInfoIOMapSet(pFrame, infoIOMapSet, 0) != OK) {
        BaseNode::flush(pFrame); // flush
        MY_LOGE("queryInfoIOMapSet failed");
        return BAD_VALUE;
    }

    frameParams.pFrame = pFrame;

    IStreamBufferSet &rStreamBufferSet = pFrame->getStreamBufferSet();

    // image
    {
        IPipelineFrame::ImageInfoIOMapSet const& mapSet =
            infoIOMapSet.mImageInfoIOMapSet;

        // traverse image info set
        for (size_t i = 0; i < mapSet.size(); i++)
        {
            MY_LOGD("ImageInfoIOMapSet(%zu) In(%zu) Out(%zu)",
                    i, FRAME_NO, mapSet[i].vIn.size(), mapSet[i].vOut.size());

            // get input image buffer
            {
                FrameInput* in = new FrameInput;

                in->streamId = mapSet[i].vIn[0]->getStreamId();
                const char* streamName = mapSet[i].vIn[0]->getStreamName();
                MY_LOGD("    IN  streamId(%08" PRIxPTR ") name(%s)", in->streamId, streamName);

                // use IMGI port
                in->mPortId = PORT_IMGI;

                sp<IImageStreamBuffer>& inStreamBuffer(in->pStreamBuffer);
                sp<IImageBufferHeap>& inImageBufferHeap(in->pImageBufferHeap);
                sp<IImageBuffer>& inImageBuffer(in->pImageBuffer);

                inStreamBuffer    = NULL;
                inImageBufferHeap = NULL;
                inImageBuffer     = NULL;

                err = ensureImageBufferAvailable_(
                        FRAME_NO, in->streamId, rStreamBufferSet, inStreamBuffer);
                if (err != OK)
                {
                    MY_LOGE("src buffer err = %d (%s)", err, strerror(-err));
                    if (inStreamBuffer == NULL)
                    {
                        MY_LOGE("pStreamBuffer is NULL");
                    }

                    // TODO: some error handling
                    return BAD_VALUE;
                }

                // get imagebuffer from image buffer heap, which belongs to a streambuffer
                err = getImageBuffer(IN, in->streamId,
                        inStreamBuffer, inImageBufferHeap, inImageBuffer);
                if (err != OK)
                {
                    MY_LOGE("get imagebuffer failed");
                    return BAD_VALUE;
                }

                switch(in->streamId) {
                case eSTREAMID_IMAGE_PIPE_RAW_OPAQUE:
                    onImageBuffer(eIMGBUFTP_IN_RAW_FULLSIZE, inImageBuffer);
                    break;
                case eSTREAMID_IMAGE_PIPE_RAW_RESIZER:
                    onImageBuffer(eIMGBUFTP_IN_RAW_RESIZED, inImageBuffer);
                    break;
                default:
                    MY_LOGW("Un handled input buffer id(%d): %s", in->streamId, streamName);
                }
                frameParams.vpIn.push_back(in);
            }

            // get output image buffers
            for (size_t j = 0; j < mapSet[i].vOut.size(); j++)
            {
                FrameOutput* out = new FrameOutput;

                out->streamId = mapSet[i].vOut[j]->getStreamId();
                const char* streamName = mapSet[i].vOut[j]->getStreamName();
                MY_LOGD("    OUT streamId(%08" PRIxPTR ") name(%s)", out->streamId, streamName);

                // use WROTO port
                out->mPortId = PORT_WROTO;
                // TODO: should base on metadata
                out->mTransform = 0;

                // get output image buffer
                sp<IImageStreamBuffer>& outStreamBuffer(out->pStreamBuffer);
                sp<IImageBufferHeap>& outImageBufferHeap(out->pImageBufferHeap);
                sp<IImageBuffer>& outImageBuffer(out->pImageBuffer);

                outStreamBuffer    = NULL;
                outImageBufferHeap = NULL;
                outImageBuffer     = NULL;

                err = ensureImageBufferAvailable_(
                        FRAME_NO, out->streamId, rStreamBufferSet, outStreamBuffer);
                if (err != OK)
                {
                    MY_LOGE("dst buffer err = %d (%s)", err, strerror(-err));
                    if (outStreamBuffer == NULL)
                    {
                        MY_LOGE("pStreamBuffer is NULL");
                    }

                    // TODO: some error handling
                    return BAD_VALUE;
                }

                // get imagebuffer from image buffer heap, which belongs to a streambuffer
                err = getImageBuffer(OUT, out->streamId,
                        outStreamBuffer, outImageBufferHeap, outImageBuffer);
                if (err != OK)
                {
                    MY_LOGE("get imagebuffer failed");
                    return BAD_VALUE;
                }

                switch(out->streamId) {
                case eSTREAMID_IMAGE_PIPE_YUV_JPEG:
                    onImageBuffer(eIMGBUFTP_OUT_YUV_JPEG, outImageBuffer);
                    break;
                case eSTREAMID_IMAGE_PIPE_YUV_THUMBNAIL:
                    onImageBuffer(eIMGBUFTP_OUT_YUV_THUMBNAIL, outImageBuffer);
                    break;
                default:
                    MY_LOGW("Un handled output buffer id(%d): %s", out->streamId, streamName);
                }
                frameParams.vpOut.push_back(out);
            }
        } // traverse image info set
    } // image

    // metadata
    {
        IPipelineFrame::MetaInfoIOMapSet const& mapSet =
            infoIOMapSet.mMetaInfoIOMapSet;

        // traverse metadata info set
        for (size_t i = 0; i < mapSet.size(); i++)
        {
            MY_LOGD("MetaInfoIOMapSet(%zu) frameNo(%zu) In(%zu) Out(%zu)",
                    i, FRAME_NO, mapSet[i].vIn.size(), mapSet[i].vOut.size());

            // get input meta buffer
            for (size_t j = 0; j < mapSet[i].vIn.size(); j++)
            {
                StreamId_T const streamId = mapSet[i].vIn[j]->getStreamId();
                const char* streamName = mapSet[i].vIn[j]->getStreamName();
                MY_LOGD("    IN  streamId(%08" PRIxPTR ") name(%s)", streamId, streamName);

                // get in app metadata
                if (mpInAppMeta_Request->getStreamId() == streamId)
                {
                    sp<IMetaStreamBuffer>& inStreamBuffer(frameParams.pMeta_InAppStreamBuffer);

                    inStreamBuffer = NULL;

                    err = ensureMetaBufferAvailable_(
                            FRAME_NO, streamId, rStreamBufferSet, inStreamBuffer);
                    if (err != OK)
                    {
                        MY_LOGE("src metadata err = %d (%s)", err, strerror(-err));
                        if (inStreamBuffer == NULL)
                        {
                            MY_LOGE("pStreamBuffer is NULL");
                        }
                        // TODO: some error handling
                        return BAD_VALUE;
                    }

                    // get metadata from meta stream buffer
                    err = getMetadata(IN, streamId,
                            inStreamBuffer, frameParams.pMeta_InApp);
                    if (err != OK)
                    {
                        MY_LOGE("get metadata failed");
                        return BAD_VALUE;
                    }
                    onMetadata(eMDATATP_IN_APP, frameParams.pMeta_InApp);
                }

                // get in hal metadata
                if (mpInHalMeta_P1->getStreamId() == streamId)
                {
                    sp<IMetaStreamBuffer>& inStreamBuffer(frameParams.pMeta_InHalStreamBuffer);

                    inStreamBuffer = NULL;

                    err = ensureMetaBufferAvailable_(
                            FRAME_NO, streamId, rStreamBufferSet, inStreamBuffer);
                    if (err != OK)
                    {
                        MY_LOGE("src metadata err = %d (%s)", err, strerror(-err));
                        if (inStreamBuffer == NULL)
                        {
                            MY_LOGE("pStreamBuffer is NULL");
                        }
                        // TODO: some error handling
                        return BAD_VALUE;
                    }

                    // get metadata from meta stream buffer
                    err = getMetadata(IN, streamId,
                            inStreamBuffer, frameParams.pMeta_InHal);
                    if (err != OK)
                    {
                        MY_LOGE("get metadata failed");
                        return BAD_VALUE;
                    }
                    onMetadata(eMDATATP_IN_HAL, frameParams.pMeta_InHal);
                }
            }

            // get output meta buffer
            for (size_t j = 0; j < mapSet[i].vOut.size(); j++)
            {
                StreamId_T const streamId = mapSet[i].vOut[j]->getStreamId();
                const char* streamName = mapSet[i].vOut[j]->getStreamName();
                MY_LOGD("    OUT streamId(%08" PRIxPTR ") name(%s)", streamId, streamName);

                // get out app metadata
                if (mpOutAppMeta_Result->getStreamId() == streamId)
                {
                    sp<IMetaStreamBuffer>& outStreamBuffer(frameParams.pMeta_OutAppStreamBuffer);

                    outStreamBuffer = NULL;

                    err = ensureMetaBufferAvailable_(
                            FRAME_NO, streamId, rStreamBufferSet, outStreamBuffer);
                    if (err != OK)
                    {
                        MY_LOGE("dst metadata err = %d (%s)", err, strerror(-err));
                        if (outStreamBuffer == NULL)
                        {
                            MY_LOGE("pStreamBuffer is NULL");
                        }
                        // TODO: some error handling
                        return BAD_VALUE;
                    }

                    // get metadata from meta stream buffer
                    err = getMetadata(OUT, streamId,
                            outStreamBuffer, frameParams.pMeta_OutApp);
                    if (err != OK)
                    {
                        MY_LOGE("get metadata failed");
                        return BAD_VALUE;
                    }
                    onMetadata(eMDATATP_OUT_APP, frameParams.pMeta_OutApp);
                }

                // get out hal metadata
                if (mpOutHalMeta_Result->getStreamId() == streamId)
                {
                    sp<IMetaStreamBuffer>& outStreamBuffer(frameParams.pMeta_OutHalStreamBuffer);

                    outStreamBuffer = NULL;

                    err = ensureMetaBufferAvailable_(
                            FRAME_NO, streamId, rStreamBufferSet, outStreamBuffer);
                    if (err != OK)
                    {
                        MY_LOGE("dst metadata err = %d (%s)", err, strerror(-err));
                        if (outStreamBuffer == NULL)
                        {
                            MY_LOGE("pStreamBuffer is NULL");
                        }
                        // TODO: some error handling
                        return BAD_VALUE;
                    }

                    // get metadata from meta stream buffer
                    err = getMetadata(OUT, streamId,
                            outStreamBuffer, frameParams.pMeta_OutHal);
                    if (err != OK)
                    {
                        MY_LOGE("get metadata failed");
                        return BAD_VALUE;
                    }
                    onMetadata(eMDATATP_OUT_HAL, frameParams.pMeta_OutHal);
                }
            } // get output meta buffer
        } // traverse metadata info set
    } // metadata

    if ((frameParams.pMeta_InApp == NULL) || (frameParams.pMeta_InHal == NULL))
    {
        MY_LOGE_IF(frameParams.pMeta_InApp == NULL, "In app metadata is NULL");
        MY_LOGE_IF(frameParams.pMeta_InHal == NULL, "In hal metadata is NULL");
        return BAD_VALUE;
    }

    // from now on, frameParams is ready for processing
    frameParams.enqueIndex = FRAME_NO;
    (*prFrameParam) = frameParams;

    return OK;
}

MERROR MFCNodeImp::releaseFrame(FrameParams *pFrameParam)
{
    const FrameParams& frameParams(*pFrameParam);
    for (size_t j = 0; j < frameParams.vpIn.size(); j++)
    {
        MY_LOGD("release input image buffer (%d)", j);

        FrameInput* frameInput(frameParams.vpIn[j]);

        unlockImage(frameInput->pStreamBuffer, frameInput->pImageBuffer);
        markImageStream(frameParams.pFrame, frameInput->pStreamBuffer);
    }

    for (size_t j = 0; j < frameParams.vpOut.size(); j++)
    {
        MY_LOGD("release output image buffer j(%d)", j);

        FrameOutput* frameOutput(frameParams.vpOut[j]);

        unlockImage(frameOutput->pStreamBuffer, frameOutput->pImageBuffer);
        markImageStream(frameParams.pFrame, frameOutput->pStreamBuffer);
    }

    if (frameParams.pMeta_InApp != NULL)
    {
        MY_LOGD("release pMeta_InAppStreamBuffer");
        unlockMetadata(frameParams.pMeta_InAppStreamBuffer, frameParams.pMeta_InApp);
        markMetaStream(frameParams.pFrame, frameParams.pMeta_InAppStreamBuffer);
    }

    if (frameParams.pMeta_InHal != NULL)
    {
        MY_LOGD("release pMeta_InHalStreamBuffer");
        unlockMetadata(frameParams.pMeta_InHalStreamBuffer, frameParams.pMeta_InHal);
        markMetaStream(frameParams.pFrame, frameParams.pMeta_InHalStreamBuffer);
    }

    if (frameParams.pMeta_OutApp != NULL)
    {
        MY_LOGD("release pMeta_OutAppStreamBuffer");
        unlockMetadata(frameParams.pMeta_OutAppStreamBuffer, frameParams.pMeta_OutApp);
        markMetaStream(frameParams.pFrame, frameParams.pMeta_OutAppStreamBuffer);
    }

    if (frameParams.pMeta_OutHal != NULL)
    {
        MY_LOGD("release pMeta_OutHalStreamBuffer");
        unlockMetadata(frameParams.pMeta_OutHalStreamBuffer, frameParams.pMeta_OutHal);
        markMetaStream(frameParams.pFrame, frameParams.pMeta_OutHalStreamBuffer);
    }

    // apply buffers to be released
    {
        CAM_TRACE_NAME("applyRelease");
        MY_LOGD("[applyRelease] frame nodeId(%08" PRIxPTR ")", getNodeId());

        // after this call, all of RELEASE-marked buffers are released by this user
        frameParams.pFrame->getStreamBufferSet().applyRelease(getNodeId());
    }

    return OK;
}

void MFCNodeImp::releaseFrameParams(FrameParams *f, bool bDeleteSelf /* = true */)
{
    for (size_t i = 0; i < f->vpIn.size(); i++)
    {
        MY_LOGD("delete vpIn i(%d)", i);
        delete f->vpIn[i];
    }

    for (size_t i = 0; i < f->vpOut.size(); i++)
    {
        MY_LOGD("delete vpOut i(%d)", i);
        delete f->vpOut[i];
    }

    if (bDeleteSelf)
        delete f;
}

void MFCNodeImp::dumpStreamIfExist(const char* str, const sp<IStreamInfo>& streamInfo)
{
    MY_LOGD_IF(streamInfo.get(), "%s: streamId(%08" PRIxPTR ") %s",
            str, streamInfo->getStreamId(), streamInfo->getStreamName());
}

void MFCNodeImp::dumpCaptureParam(const CaptureParam_T &captureParam, const char* msg /* = NULL */)
{
    if (msg) MY_LOGD("%s", msg);

    MY_LOGD("=================");
    MY_LOGD("u4ExposureMode(%d)", captureParam.u4ExposureMode);
    MY_LOGD("u4Eposuretime(%d)", captureParam.u4Eposuretime);
    MY_LOGD("u4AfeGain(%d)", captureParam.u4AfeGain);
    MY_LOGD("u4IspGain(%d)", captureParam.u4IspGain);
    MY_LOGD("u4RealISO(%d)", captureParam.u4RealISO);
    MY_LOGD("u4FlareGain(%d)", captureParam.u4FlareGain);
    MY_LOGD("u4FlareOffset(%d)", captureParam.u4FlareOffset);
    MY_LOGD("i4LightValue_x10(%d)", captureParam.i4LightValue_x10);
}

MERROR MFCNodeImp::verifyConfigParams(const ConfigParams &rParams)
{
    if (!rParams.pInAppMeta.get()) {
        MY_LOGE("out in app metadata");
        return BAD_VALUE;
    }

    if (!rParams.pInHalMeta.get()) {
        MY_LOGE("no in hal metadata");
        return BAD_VALUE;
    }

    // TODO: need to check out metadata?
    //if  ( ! rParams.pOutAppMeta.get() ) {
    //    return BAD_VALUE;
    //}
    //if  ( ! rParams.pOutHalMeta.get() ) {
    //    return BAD_VALUE;
    //}

    if (rParams.vInFullRaw.size() == 0) {
        MY_LOGE("no in image fullraw");
        return BAD_VALUE;
    }

    // resized raw can be null; just log for notification
    if (!rParams.pInResizedRaw.get()) {
        MY_LOGD("no in resized raw");
    }

    if (rParams.vOutImage.size() == 0) {
        MY_LOGE("no out yuv image");
        return BAD_VALUE;
    }

    // dump all streams
    size_t count;

    dumpStreamIfExist("[meta] in app", rParams.pInAppMeta);
    dumpStreamIfExist("[meta] in hal", rParams.pInHalMeta);
    dumpStreamIfExist("[meta] out app", rParams.pOutAppMeta);
    dumpStreamIfExist("[meta] out hal", rParams.pOutHalMeta);

    count = rParams.vInFullRaw.size();
    for (size_t i = 0; i < count; i++) {
        dumpStreamIfExist("[image] in full", rParams.vInFullRaw[i]);
    }

    dumpStreamIfExist("[image] in resized", rParams.pInResizedRaw);

    count = rParams.vOutImage.size();
    for (size_t i = 0; i < count; i++) {
        dumpStreamIfExist("[image] out yuv", rParams.vOutImage[i]);
    }

    return OK;
}

inline MBOOL MFCNodeImp::isFullRawLocked(StreamId_T const streamId)
{
    return isStream(mpInFullRaw, streamId);
}

inline MBOOL MFCNodeImp::isResizedRawLocked(StreamId_T const streamId)
{
    return isStream(mpInResizedRaw, streamId);
}

MBOOL MFCNodeImp::isInImageStream(StreamId_T const streamId)
{
    RWLock::AutoWLock _l(mConfigRWLock);

    if (isFullRawLocked(streamId) || isResizedRawLocked(streamId))
        return MTRUE;

    MY_LOGD_IF(0, "streamId(%08" PRIxPTR ") is not in-stream", streamId);
    return MFALSE;
}

MBOOL MFCNodeImp::isInMetaStream(StreamId_T const streamId)
{
    RWLock::AutoWLock _l(mConfigRWLock);
    return isStream(mpInAppMeta_Request, streamId) || isStream(mpInHalMeta_P1, streamId);
}

MVOID MFCNodeImp::unlockImage(sp<IImageStreamBuffer> const pStreamBuffer, sp<IImageBuffer> const pImageBuffer)
{
    if (pStreamBuffer == NULL || pImageBuffer == NULL) {
        MY_LOGE("pStreamBuffer(%p) or pImageBuffer(%p) should not be NULL",
               pStreamBuffer.get(), pImageBuffer.get());
        return;
    }

    pImageBuffer->unlockBuf(getNodeName());
    pStreamBuffer->unlock(getNodeName(), pImageBuffer->getImageBufferHeap());
}

MVOID MFCNodeImp::unlockMetadata(sp<IMetaStreamBuffer> const pStreamBuffer, IMetadata* const pMetadata)
{
    if (pStreamBuffer == NULL || pMetadata == NULL) {
        MY_LOGE("pStreamBuffer(%p) or pMetadata(%p) should not be NULL",
                pStreamBuffer.get(), pMetadata);
        return;
    }

    pStreamBuffer->unlock(getNodeName(), pMetadata);
}

const char* MFCNodeImp::getFileFormatName(const MINT &f)
{
    switch (f) {
    case eImgFmt_YUY2:
        return "yuy2";
    case eImgFmt_YV12:
        return "yv12";
    case eImgFmt_YV16:
        return "yv16";
    case eImgFmt_I420:
        return "i420";
    default:
        return "N/A";
    }
}

MBOOL MFCNodeImp::retrieveGmvInfo(IMetadata *pMetadata, int &x, int &y, MSize &size)
{
    MBOOL  ret = MTRUE;
    MSize  rzoSize;
    IMetadata::IEntry entry;

    /* get size first */
    ret = tryGetMetaData(pMetadata, MTK_P1NODE_RESIZER_SIZE, rzoSize);
    if (ret != MTRUE) {
        MY_LOGE("%s: cannot get rzo size", __FUNCTION__);
        goto lbExit;
    }

    entry = pMetadata->entryFor(MTK_EIS_REGION);

    /* check if a valid EIS_REGION */
    if (entry.count() < EIS_REGION_INDEX_SIZE) {
        MY_LOGE("%s: entry is not a valid EIS_REGION, size = %d",
                __FUNCTION__,
                entry.count());
        ret = MFALSE;
        goto lbExit;
    }

    x = entry.itemAt(EIS_REGION_INDEX_GMVX, Type2Type<MINT32>());
    y = entry.itemAt(EIS_REGION_INDEX_GMVY, Type2Type<MINT32>());
    size = rzoSize;

lbExit:
    return ret;
}

MERROR MFCNodeImp::getInfoIOMapSet(
    sp<IPipelineFrame> const& pFrame,
    IPipelineFrame::InfoIOMapSet &rIOMapSet,
    MINT32 frameIndex)
{
    // get PipelineFrame's InfoIOMapSet
    if (OK != pFrame->queryInfoIOMapSet(getNodeId(), rIOMapSet)) {
        MY_LOGE("queryInfoIOMap failed");
        return NAME_NOT_FOUND;
    }

    const MUINT32 FRAME_NO = pFrame->getFrameNo();

    // check image part
    IPipelineFrame::ImageInfoIOMapSet& imageIOMapSet = rIOMapSet.mImageInfoIOMapSet;
    if (imageIOMapSet.size() == 0) {
        MY_LOGW("no imageIOMap in frame");
        return BAD_VALUE;
    }

    for (size_t i = 0; i < imageIOMapSet.size(); i++) {
        IPipelineFrame::ImageInfoIOMap const& imageIOMap = imageIOMapSet[i];

        // return if in-degree is not equal to 1
        if (imageIOMap.vIn.size() != 1) {
            MY_LOGE("[image] #%zu: wrong size vIn %zu", i, imageIOMap.vIn.size());
            return BAD_VALUE;
        }

        MY_LOGD("ImageInfoIOMapSet(%zu) frameNo(%zu) In(%zu) Out(%zu)",
                i, FRAME_NO, imageIOMap.vIn.size(), imageIOMap.vOut.size());
    }

    // check metadata part
    IPipelineFrame::MetaInfoIOMapSet& metaIOMapSet = rIOMapSet.mMetaInfoIOMapSet;
    if (metaIOMapSet.size() == 0) {
        MY_LOGW("no metaIOMap in frame");
        return BAD_VALUE;
    }

    for (size_t i = 0; i < metaIOMapSet.size(); i++) {
        IPipelineFrame::MetaInfoIOMap const& metaIOMap = metaIOMapSet[i];

        // return if app's meta request is empty or cannot be found in the current MetaInfoIOMap
        if (!mpInAppMeta_Request.get() ||
                metaIOMap.vIn.indexOfKey(mpInAppMeta_Request->getStreamId()) < 0) {
            MY_LOGE("[meta] #%zu: app's meta request is empty or cannot be found", i);
            return BAD_VALUE;
        }

        // return if P1's meta request is empty or cannot be found in the current MetaInfoIOMap
        if (!mpInHalMeta_P1.get() ||
                metaIOMap.vIn.indexOfKey(mpInHalMeta_P1->getStreamId()) < 0) {
            MY_LOGE("[meta] #%zu: P1's meta request is empty or cannot be found", i);
            return BAD_VALUE;
        }

        MY_LOGD_IF(mLogLevel >= 1, "MetaInfoIOMapSet(%zu) frameNo(%zu) In(%zu) Out(%zu)",
                i, FRAME_NO, metaIOMap.vIn.size(), metaIOMap.vOut.size());
    }

    return OK;
}

MERROR MFCNodeImp::getImageBuffer(
    MINT32 type,
    const StreamId_T &streamId,
    sp<IImageStreamBuffer>& streamBuffer,
    sp<IImageBufferHeap>& imageBufferHeap,
    sp<IImageBuffer>& imageBuffer)
{
    // query the group usage from UsersManager
    MUINT const groupUsage = streamBuffer->queryGroupUsage(getNodeId());

    // get image buffer heap from streambuffer (with read or write lock locked)
    switch (type & (IN | OUT)){
    case IN:
        imageBufferHeap = streamBuffer->tryReadLock(getNodeName());
        break;
    case OUT:
        imageBufferHeap = streamBuffer->tryWriteLock(getNodeName());
        break;
    default:
        MY_LOGE("    invalid buffer type(%d)", type);
    }

    if (imageBufferHeap == NULL) {
        MY_LOGE("    node(%d) type(%d) stream buffer(%s): cannot get imageBufferHeap",
                getNodeId(), type, streamBuffer->getName());
        return INVALID_OPERATION;
    }

    // create imagebuffer from image buffer heap
    imageBuffer = imageBufferHeap->createImageBuffer();
    if (imageBuffer == NULL) {
        MY_LOGE("    node(%d) type(%d) stream buffer(%s): cannot create imageBuffer",
                getNodeId(), type, streamBuffer->getName());
        return INVALID_OPERATION;
    }

    // a buffer is allowed to access only between the interval of
    // lockBuf() and unlockBuf()
    imageBuffer->lockBuf(getNodeName(), groupUsage);
    MY_LOGD_IF(mLogLevel >= 1, "    streamId(%08" PRIxPTR ") buffer(%p) usage(%p) type(0x%x)",
            streamId, imageBuffer.get(), groupUsage, type);

    return OK;
}

MERROR MFCNodeImp::getMetadata(
    MINT32 type,
    const StreamId_T &streamId,
    sp<IMetaStreamBuffer> &streamBuffer,
    IMetadata *&metadata)
{
    // get metadata from meta stream buffer
    switch (type & (IN | OUT)) {
    case IN:
        metadata = streamBuffer->tryReadLock(getNodeName());
        break;
    case OUT:
        metadata = streamBuffer->tryWriteLock(getNodeName());
    }

    if (metadata == NULL) {
        MY_LOGE("      node(%d) stream buffer(%s): cannot get app metadata",
                getNodeId(), streamBuffer->getName());
        return INVALID_OPERATION;
    }

    MY_LOGD_IF(mLogLevel >= 1, "      streamId(%08" PRIxPTR ") metadata(%p) type(0x%x)",
            streamId, metadata, type);

    return OK;
}

MVOID MFCNodeImp::markImageStream(sp<IPipelineFrame> const& pFrame, sp<IImageStreamBuffer> const pStreamBuffer)
{
    if ((pFrame == NULL) || (pStreamBuffer == NULL)) {
        MY_LOGE("pFrame(%d) or pStreamBuffer(%d) should not be NULL",
                pFrame.get(), pStreamBuffer.get());
        return;
    }

    IStreamBufferSet& streamBufferSet = pFrame->getStreamBufferSet();
    StreamId_T const streamId = pStreamBuffer->getStreamInfo()->getStreamId();

    // buffer Producer must set this status
    if (isInImageStream(streamId)) {
        pStreamBuffer->markStatus(STREAM_BUFFER_STATUS::WRITE_OK);
    }

    // mark this buffer as USED by this user
    // mark this buffer as RELEASE by this user
    streamBufferSet.markUserStatus(
            streamId,
            getNodeId(),
            IUsersManager::UserStatus::USED |
            IUsersManager::UserStatus::RELEASE);
}

MVOID MFCNodeImp::markMetaStream(sp<IPipelineFrame> const& pFrame,  sp<IMetaStreamBuffer> const pStreamBuffer)
{
    if ((pFrame == NULL) || (pStreamBuffer == NULL)) {
        MY_LOGE("pFrame(%d) or pStreamBuffer(%d) should not be NULL",
                pFrame.get(), pStreamBuffer.get());
        return;
    }

    IStreamBufferSet& rStreamBufferSet = pFrame->getStreamBufferSet();
    StreamId_T const streamId = pStreamBuffer->getStreamInfo()->getStreamId();


    // buffer Producer must set this status
    if (!isInMetaStream(streamId)) {
        pStreamBuffer->markStatus(STREAM_BUFFER_STATUS::WRITE_OK);
    }

    // mark this buffer as USED by this user
    // mark this buffer as RELEASE by this user
    rStreamBufferSet.markUserStatus(
            streamId,
            getNodeId(),
            IUsersManager::UserStatus::USED |
            IUsersManager::UserStatus::RELEASE);
}

