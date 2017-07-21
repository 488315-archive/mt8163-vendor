/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2015. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

#define LOG_TAG "PipielineContextMFCTest"

#include <Log.h>

#include <stdlib.h>
#include <stdio.h>

#include <v3/pipeline/IPipelineNode.h>
#include <v3/pipeline/IPipelineNodeMapControl.h>
#include <v3/pipeline/IPipelineFrameControl.h>
#include <v3/pipeline/PipelineContext.h>

#include <v3/utils/streambuf/StreamBufferPool.h>
#include <v3/utils/streambuf/StreamBuffers.h>
#include <v3/utils/streaminfo/MetaStreamInfo.h>
#include <v3/utils/streaminfo/ImageStreamInfo.h>

#include <utils/include/imagebuf/IIonImageBufferHeap.h>

#include <IHalSensor.h>

#include <v3/hwpipeline/StreamId.h>
#include <v3/hwpipeline/NodeId.h>
#include <v3/hwnode/P1Node.h>
#include <v3/hwnode/MfllNode.h>
#include <v3/hwnode/HDRNode.h>
#include <v3/hwnode/JpegNode.h>

#include <hardware/camera3.h> // for template
#include <metadata/ITemplateRequest.h>
#include <metadata/IMetadataProvider.h>
#include <metadata/client/mtk_metadata_tag.h>

#include <mtk_platform_metadata_tag.h>

#include <aaa_hal_common.h>

using namespace NSCam;
#include <hwutils/HwInfoHelper.h>

using namespace NSCamHW;
using namespace v3;
using namespace NSCam::v3::Utils;
using namespace android;
using namespace NSCam::v3::NSPipelineContext;

#define CALLER_NAME LOG_TAG
#define PIPELINE_NAME "MFC"

// sensor open id
#define SENSOR_OPEN_ID  0

#define POSTVIEW_ENABLED       MTRUE
#define POSTVIEW_WIDTH         640
#define POSTVIEW_HEIGHT        480
#define POSTVIEW_FORMAT        eImgFmt_YV12

#define YUV_JPEG_ENABLED       MTRUE
#define YUV_JPEG_WIDTH         4096
#define YUV_JPEG_HEIGHT        3072
#define YUV_JPEG_FORMAT        eImgFmt_YUY2

#define YUV_THUMBNAIL_ENABLED  MTRUE
#define YUV_THUMBNAIL_WIDTH  160
#define YUV_THUMBNAIL_HEIGHT 128
#define YUV_THUMBNAIL_FORMAT   YUV_JPEG_FORMAT

#define JPEG_ENABLED           MTRUE
#define JPEG_FORMAT            eImgFmt_BLOB
#define JPEG_ORIENTATION       0
#define JPEG_QUALITY           90

#define JPEG_THUMBNAIL_QUALITY 90

#define ALIGN_CEIL(x,a)     (((x) + (a) - 1L) & ~((a) - 1L))

// debug options
#define DEBUG_ENABLE_AE_CAP

// ---------------------------------------------------------------------------

namespace {

// ---------------------------------------------------------------------------

    enum STREAM_ID
    {
        // image stream
        STREAM_ID_IMAGE_RAW_OPAQUE         = eSTREAMID_IMAGE_PIPE_RAW_OPAQUE_00,
        STREAM_ID_IMAGE_RAW_RESIZED        = eSTREAMID_IMAGE_PIPE_RAW_RESIZER_00,
        STREAM_ID_IMAGE_YUV_00             = 0x2,
        STREAM_ID_IMAGE_YUV_JPEG           = eSTREAMID_IMAGE_PIPE_YUV_JPEG_00,
        STREAM_ID_IMAGE_YUV_THUMBNAIL      = eSTREAMID_IMAGE_PIPE_YUV_THUMBNAIL_00,
        //STREAM_ID_IMAGE_JPEG               = eSTREAMID_IMAGE_JPEG,
        STREAM_ID_IMAGE_JPEG               = 0x9,
        // metadata stream
        STREAM_ID_METADATA_CONTROL_APP     = eSTREAMID_BEGIN_OF_APP,
        STREAM_ID_METADATA_CONTROL_HAL     = eSTREAMID_META_PIPE_CONTROL,
        STREAM_ID_METADATA_RESULT_P1_APP   = eSTREAMID_META_APP_DYNAMIC_01,
        STREAM_ID_METADATA_RESULT_P1_HAL   = eSTREAMID_META_PIPE_DYNAMIC_01,
        STREAM_ID_METADATA_RESULT_MFC_APP  = eSTREAMID_META_APP_DYNAMIC_HDR,
        STREAM_ID_METADATA_RESULT_MFC_HAL  = eSTREAMID_META_PIPE_DYNAMIC_HDR,
        STREAM_ID_METADATA_RESULT_JPEG_APP = eSTREAMID_META_APP_DYNAMIC_JPEG
    };

    enum NODE_CONFIG
    {
        eConfigNone = 0x0,
        eConfigP1   = 0x1,
        eConfigMFC  = 0x2,
        eConfigJpeg = 0x4,
    };

    IHalSensor* mpSensorHalObj;

    // request template
    const camera3_request_template REQUEST_TEMPLATE = CAMERA3_TEMPLATE_STILL_CAPTURE;

    P1Node::SensorParams        gSensorParam;

    // IMGO setting (for full raw)
    MSize                       gImgoSize;
    MINT                        gImgoFormat;
    size_t                      gImgoStride;

    // RRZO setting (for resized raw)
    MSize                       gRrzoSize;
    MINT                        gRrzoFormat;
    size_t                      gRrzoStride;

    sp<PipelineContext> gContext;

    // helper for senor information
    HwInfoHelper gHelper(SENSOR_OPEN_ID);

    // stream information
    sp<IMetaStreamInfo>         gControlMeta_App;
    sp<IMetaStreamInfo>         gControlMeta_Hal;
    sp<IMetaStreamInfo>         gResultMeta_P1_App;
    sp<IMetaStreamInfo>         gResultMeta_P1_Hal;
    sp<IMetaStreamInfo>         gResultMeta_MFC_App;
    sp<IMetaStreamInfo>         gResultMeta_MFC_Hal;
    sp<IMetaStreamInfo>         gResultMeta_Jpeg_App;

    sp<IImageStreamInfo>        gImageInfo_fullRaw;
    sp<IImageStreamInfo>        gImageInfo_resizedRaw;
    sp<IImageStreamInfo>        gImageInfo_yuv00;
    sp<IImageStreamInfo>        gImageInfo_yuvJpeg;
    sp<IImageStreamInfo>        gImageInfo_yuvThumbnail;
    sp<IImageStreamInfo>        gImageInfo_Jpeg;
}; // namespace

/* get node id/name by template class */
template<class T> MUINT32 getNodeId(void) { return 0; }
template<> MUINT32 getNodeId<HDRNode>(void) { return eNODEID_HdrNode;}
template<> MUINT32 getNodeId<MfllNode>(void) { return eNODEID_MfllNode; }


// ---------------------------------------------------------------------------

static sp<IImageStreamInfo> createRawImageStreamInfo(
        char const*  streamName,
        StreamId_T   streamId,
        MUINT32      streamType,
        size_t       maxBufNum,
        size_t       minInitBufNum,
        MUINT        usageForAllocator,
        MINT         imgFormat,
        MSize const& imgSize,
        size_t const stride);

static sp<IImageStreamInfo> createImageStreamInfo(
        char const*  streamName,
        StreamId_T   streamId,
        MUINT32      streamType,
        size_t       maxBufNum,
        size_t       minInitBufNum,
        MUINT        usageForAllocator,
        MINT         imgFormat,
        MSize const& imgSize,
        MUINT32      transform = 0);

static MVOID addStreamToSet(StreamSet& set, sp<IStreamInfo> pInfo)
{
    if (pInfo.get())
    {
        set.add(pInfo->getStreamId());
    }
}

static MBOOL isPostViewEnabled()
{
    return POSTVIEW_ENABLED;
}

static MBOOL isYuvJpegEnabled()
{
    return YUV_JPEG_ENABLED;
}

static MBOOL isYuvThumbnailEnabled()
{
    return YUV_THUMBNAIL_ENABLED;
}

static MBOOL isJpegEnabled()
{
    return JPEG_ENABLED;
}

// ---------------------------------------------------------------------------

void setupMetaStreamInfo()
{
    // control metadata
    gControlMeta_App =
        new MetaStreamInfo(
                "App:Meta:Control",
                STREAM_ID_METADATA_CONTROL_APP,
                eSTREAMTYPE_META_IN,
                0);

    gControlMeta_Hal =
        new MetaStreamInfo(
                "Hal:Meta:Control",
                STREAM_ID_METADATA_CONTROL_HAL,
                eSTREAMTYPE_META_IN,
                0);

    // P1 metadata
    gResultMeta_P1_App =
        new MetaStreamInfo(
                "App:Meta:ResultP1",
                STREAM_ID_METADATA_RESULT_P1_APP,
                eSTREAMTYPE_META_OUT,
                0);

    gResultMeta_P1_Hal =
        new MetaStreamInfo(
                "Hal:Meta:ResultP1",
                STREAM_ID_METADATA_RESULT_P1_HAL,
                eSTREAMTYPE_META_INOUT,
                0);

    // MFC metadata
    gResultMeta_MFC_App =
        new MetaStreamInfo(
                "App:Meta:ResultMFC",
                STREAM_ID_METADATA_RESULT_MFC_APP,
                eSTREAMTYPE_META_OUT,
                0);

    gResultMeta_MFC_Hal =
        new MetaStreamInfo(
                "Hal:Meta:ResultMFC",
                STREAM_ID_METADATA_RESULT_MFC_HAL,
                eSTREAMTYPE_META_INOUT,
                0);

    // Jpeg metadata
    gResultMeta_Jpeg_App =
        new MetaStreamInfo(
                "App:Meta:ResultJpeg",
                STREAM_ID_METADATA_RESULT_JPEG_APP,
                eSTREAMTYPE_META_OUT,
                0);
}

void setupImageStreamInfo()
{
    // full raw
    // set stream type to IN/OUT for reprocessing purpose
    {
        MSize const& size = gImgoSize;
        MINT const format = gImgoFormat;
        size_t const stride = gImgoStride;
        MUINT const usage = eBUFFER_USAGE_SW_READ_OFTEN | eBUFFER_USAGE_HW_CAMERA_READWRITE;

        gImageInfo_fullRaw = createRawImageStreamInfo(
                "Hal:Imge:P1_FullRaw",
                STREAM_ID_IMAGE_RAW_OPAQUE,
                eSTREAMTYPE_IMAGE_INOUT,
                4, 4,
                usage, format, size, stride);

        if (gImageInfo_fullRaw == NULL)
        {
            CAM_LOGE("create ImageStreamInfo P1_FullRaw failed");
        }
    }

    // resized raw
    // set stream type to IN/OUT for reprocessing purpose
    if (gHelper.isRaw())
    {
        MSize const& size = gRrzoSize;
        MINT const format = gRrzoFormat;
        size_t const stride = gRrzoStride;
        MUINT const usage = eBUFFER_USAGE_SW_READ_OFTEN | eBUFFER_USAGE_HW_CAMERA_READWRITE;

        gImageInfo_resizedRaw = createRawImageStreamInfo(
                "Hal:Image:P1_ResizedRaw",
                STREAM_ID_IMAGE_RAW_RESIZED,
                eSTREAMTYPE_IMAGE_INOUT,
                4, 4,
                usage, format, size, stride);

        if (gImageInfo_resizedRaw == NULL)
        {
            CAM_LOGE("create ImageStreamInfo P1_ResizedRaw failed");
        }
    }

    // postview
    // set stream type to IN/OUT for reprocessing purpose
    if (isPostViewEnabled())
    {
        MSize const& size = MSize(POSTVIEW_WIDTH, POSTVIEW_HEIGHT);
        MINT const format = POSTVIEW_FORMAT;
        MUINT const usage = 0; // not necessary here

        gImageInfo_yuv00 = createImageStreamInfo(
                "App:Image:yuv00",
                STREAM_ID_IMAGE_YUV_00,
                eSTREAMTYPE_IMAGE_OUT,
                1, 1,
                usage, format, size);

        if (gImageInfo_yuv00 == NULL)
        {
            CAM_LOGE("create ImageStreamInfo postview failed");
        }
    }

    // jpeg yuv buffer
    if (isYuvJpegEnabled())
    {
        MSize const& size = MSize(YUV_JPEG_WIDTH, YUV_JPEG_HEIGHT);
        MINT format = YUV_JPEG_FORMAT;
        MUINT const usage = eBUFFER_USAGE_SW_READ_OFTEN |
                            eBUFFER_USAGE_HW_CAMERA_READWRITE;
        // TODO: set the real transform
        MUINT32 transform = 0;

        gImageInfo_yuvJpeg = createImageStreamInfo(
                "Hal:Image:yuvJpeg",
                STREAM_ID_IMAGE_YUV_JPEG,
                eSTREAMTYPE_IMAGE_INOUT,
                1, 1,
                usage, format, size, transform);

        if (gImageInfo_yuvJpeg == NULL)
        {
            CAM_LOGE("create ImageStreamInfo yuvJpeg failed");
        }
    }

    // thumbnail yuv buffer
    if (isYuvThumbnailEnabled())
    {
        MSize const& size = MSize(YUV_THUMBNAIL_WIDTH, YUV_THUMBNAIL_HEIGHT);
        MINT format = YUV_THUMBNAIL_FORMAT;
        MUINT const usage = eBUFFER_USAGE_SW_READ_OFTEN |
                            eBUFFER_USAGE_HW_CAMERA_READWRITE;
        MUINT32 transform = 0;

        gImageInfo_yuvThumbnail = createImageStreamInfo(
                "Hal:Image:yuvThumbnail",
                STREAM_ID_IMAGE_YUV_THUMBNAIL,
                eSTREAMTYPE_IMAGE_INOUT,
                1, 1,
                usage, format, size, transform);

        if (gImageInfo_yuvThumbnail == NULL)
        {
            CAM_LOGE("create ImageStreamInfo yuvThumbnail failed");
        }
    }

    // jpeg
    if (isJpegEnabled())
    {
        MSize const& size = MSize(YUV_JPEG_WIDTH, YUV_JPEG_HEIGHT);
        MINT format = JPEG_FORMAT;
        MUINT const usage = eBUFFER_USAGE_SW_READ_OFTEN |
                            eBUFFER_USAGE_HW_CAMERA_READWRITE;
        MUINT32 transform = 0;

        gImageInfo_Jpeg = createImageStreamInfo(
                "App:Image:Jpeg",
                STREAM_ID_IMAGE_JPEG,
                eSTREAMTYPE_IMAGE_OUT,
                1, 1,
                usage, format, size, transform);

        if (gImageInfo_Jpeg == NULL)
        {
            CAM_LOGE("create ImageStreamInfo Jpeg failed");
        }
    }
}

void prepareSensor()
{
    // create sensor hal
    IHalSensorList* const pHalSensorList = IHalSensorList::get();

    // search sensors and return the number of image sensors
    pHalSensorList->searchSensors();

    // create sensor
    mpSensorHalObj = pHalSensorList->createSensor(CALLER_NAME, SENSOR_OPEN_ID);
    if (mpSensorHalObj == NULL)
    {
        CAM_LOGE("create sensor failed");
        return;
    }

    // turn on sensor
    MUINT32 sensorArray[] = { SENSOR_OPEN_ID };
    mpSensorHalObj->powerOn(CALLER_NAME, 1, sensorArray);

    // update sensor static information
    if (!gHelper.updateInfos())
    {
        CAM_LOGE("update sensor static information failed");
        return;
    }

    // set capture as sensor scenario
    gSensorParam.mode = SENSOR_SCENARIO_ID_NORMAL_CAPTURE;

    // get sensor size
    gHelper.getSensorSize(gSensorParam.mode, gSensorParam.size);

    // get sensor fps
    MINT32 sensorFps;
    gHelper.getSensorFps(gSensorParam.mode, sensorFps);
    gSensorParam.fps = sensorFps;

    // get sensor pixel mode
    gHelper.queryPixelMode(gSensorParam.mode, gSensorParam.fps, gSensorParam.pixelMode);

    CAM_LOGD("sensor params mode(%d) size(%dx%d) FPS(%d) pixelmode(%d)",
            gSensorParam.mode,
            gSensorParam.size.w, gSensorParam.size.h,
            gSensorParam.fps,
            gSensorParam.pixelMode);
}

void closeSensor()
{
    // turn off sensor
    MUINT32 sensorArray[] = { SENSOR_OPEN_ID };
    mpSensorHalObj->powerOff(CALLER_NAME, 1, sensorArray);

    // destroy sensor hal object created from IHalSensorList::createSensor
    mpSensorHalObj->destroyInstance(CALLER_NAME);
}

void prepareConfiguration()
{
#define BIT_DEPTH 10
    // add sensor's metadata provider into metadata provider manager
    {
        sp<IMetadataProvider> pMetadataProvider = IMetadataProvider::create(SENSOR_OPEN_ID);
        NSMetadataProviderManager::add(SENSOR_OPEN_ID, pMetadataProvider.get());
    }

    // full raw
    {
        MSize captureSize = gSensorParam.size;
        size_t stride;

        CAM_LOGD("capture zise (%dx%d)", captureSize.w, captureSize.h);

        if (!gHelper.getImgoFmt(BIT_DEPTH, gImgoFormat) ||
            !gHelper.alignPass1HwLimitation(
                gSensorParam.pixelMode, gImgoFormat, true, captureSize, stride))
        {
            CAM_LOGE("wrong parameter for IMGO");
            return;
        }

        gImgoSize = captureSize;
        gImgoStride = stride;
    }

    // resied raw
    {
        // pretend that one-fourth of the sensor size as the preview size
        MSize previewSize = MSize(gSensorParam.size.w / 2, gSensorParam.size.h / 2);
        MSize resultSize;
        size_t stride;

        CAM_LOGD("preview size (%dx%d)", previewSize.w, previewSize.h);

        if (gHelper.isRaw())
        {
            if (!gHelper.getRrzoFmt(BIT_DEPTH, gRrzoFormat) ||
                !gHelper.alignRrzoHwLimitation(
                    previewSize, gSensorParam.size, resultSize) ||
                !gHelper.alignPass1HwLimitation(
                    gSensorParam.pixelMode, gRrzoFormat, false, resultSize, stride))
            {
                CAM_LOGE("wrong parameter for RRZO");
                return;
            }
        }
        else
        {
            CAM_LOGW("sensor(%d) is not a RAW type", SENSOR_OPEN_ID);
        }

        gRrzoSize = resultSize;
        gRrzoStride = stride;
    }
#undef BIT_DEPTH

    CAM_LOGD("imgo size(%dx%d) format(0x%x) stride(%zu)\n" \
             "rrzo size(%dx%d) format(0x%x) stride(%zu)",
            gImgoSize.w, gImgoSize.h, gImgoFormat, gImgoStride,
            gRrzoSize.w, gRrzoSize.h, gRrzoFormat, gRrzoStride);
}

MERROR configureP1Node(sp<PipelineContext>& rpPipelineContext, MUINT32& flag)
{
    MERROR ret = OK;

    // port definition
    // incoming port: App:Meta:Control, Hal:Meta:Control
    // outgoing port: App:Meta:ResultP1, Hal:Meta:ResultP1,
    //                Hal:Imge:P1_FullRaw, Hal:Image:P1_ResizedRaw

    // metadata
    {
        StreamBuilder(eStreamType_META_APP, gResultMeta_P1_App)
            .build(rpPipelineContext);

        StreamBuilder(eStreamType_META_HAL, gResultMeta_P1_Hal)
            .build(rpPipelineContext);
    }

    // image
    {
        StreamBuilder(eStreamType_IMG_HAL_POOL, gImageInfo_fullRaw)
            .build(rpPipelineContext);

        StreamBuilder(eStreamType_IMG_HAL_POOL, gImageInfo_resizedRaw)
            .build(rpPipelineContext);

    }

    // configure node
    {
        typedef P1Node             NodeT;
        typedef NodeActor< NodeT > NodeActorT;

        CAM_LOGD("Nodebuilder p1 +");

        NodeT::InitParams initParam;
        {
            initParam.openId = SENSOR_OPEN_ID;
            initParam.nodeId = eNODEID_P1Node;
            initParam.nodeName = "P1Node";
        }

        NodeT::ConfigParams cfgParam;
        {
            cfgParam.pInAppMeta        = gControlMeta_App;
            cfgParam.pInHalMeta        = gControlMeta_Hal;
            cfgParam.pOutAppMeta       = gResultMeta_P1_App;
            cfgParam.pOutHalMeta       = gResultMeta_P1_Hal;
            cfgParam.pOutImage_resizer = gImageInfo_resizedRaw;
            cfgParam.sensorParams = gSensorParam;

            if (gImageInfo_fullRaw != 0)
            {
                cfgParam.pvOutImage_full.push_back(gImageInfo_fullRaw);
                cfgParam.pStreamPool_full =
                    rpPipelineContext->queryImageStreamPool(gImageInfo_fullRaw->getStreamId());
            }

            if (gImageInfo_resizedRaw != 0)
            {
                cfgParam.pStreamPool_resizer =
                    rpPipelineContext->queryImageStreamPool(gImageInfo_resizedRaw->getStreamId());
            }
        }

        sp<NodeActorT> pNode = new NodeActorT(NodeT::createInstance());
        pNode->setInitParam(initParam);
        pNode->setConfigParam(cfgParam);

        StreamSet vIn;
        addStreamToSet(vIn, gControlMeta_App);
        addStreamToSet(vIn, gControlMeta_Hal);

        StreamSet vOut;
        addStreamToSet(vOut, gImageInfo_fullRaw);
        addStreamToSet(vOut, gImageInfo_resizedRaw);
        addStreamToSet(vOut, gResultMeta_P1_App);
        addStreamToSet(vOut, gResultMeta_P1_Hal);

        NodeBuilder nodeBuilder(eNODEID_P1Node, pNode);

        nodeBuilder.addStream(
                NodeBuilder::eDirection_IN, vIn);
        nodeBuilder.addStream(
                NodeBuilder::eDirection_OUT, vOut);

        if (gImageInfo_fullRaw != 0)
        {
            nodeBuilder.setImageStreamUsage(
                    gImageInfo_fullRaw->getStreamId(),
                    eBUFFER_USAGE_SW_READ_OFTEN | eBUFFER_USAGE_HW_CAMERA_WRITE);
        }

        if (gImageInfo_resizedRaw != 0)
        {
            nodeBuilder.setImageStreamUsage(
                    gImageInfo_resizedRaw->getStreamId(),
                    eBUFFER_USAGE_SW_READ_OFTEN | eBUFFER_USAGE_HW_CAMERA_WRITE);
        }

        MERROR ret = nodeBuilder.build(rpPipelineContext);

        CAM_LOGD("Nodebuilder p1 -");

        if (ret != OK)
        {
            CAM_LOGE("build p1 node error");
            return ret;
        }
    }

    flag |= eConfigP1;

    return ret;
}

template<class NodeT>
MERROR configureMFCNode(sp<PipelineContext>& rpPipelineContext, MUINT32& flag)
{
    MERROR ret = OK;

    // port definition
    // incoming port: App:Meta:Control, Hal:Meta:ResultP1,
    //                Hal:Imge:P1_FullRaw, Hal:Image:P1_ResizedRaw
    // outgoing port: App:Meta:ResultMFC, Hal:Meta:ResultMFC,
    //                App:Image:yuv00, Hal:Image:yuvJpeg, Hal:Image:yuvThumbnail

    // metadata
    {
        StreamBuilder(eStreamType_META_APP, gResultMeta_MFC_App)
            .build(rpPipelineContext);

        StreamBuilder(eStreamType_META_HAL, gResultMeta_MFC_Hal)
            .build(rpPipelineContext);
    }

    // image
    {
        StreamBuilder(eStreamType_IMG_HAL_POOL, gImageInfo_yuv00)
            .build(rpPipelineContext);

        StreamBuilder(eStreamType_IMG_HAL_POOL, gImageInfo_yuvJpeg)
            .build(rpPipelineContext);

        StreamBuilder(eStreamType_IMG_HAL_POOL, gImageInfo_yuvThumbnail)
            .build(rpPipelineContext);
    }

    // configure node
    {
        typedef NodeActor< NodeT >  NodeActorT;

        CAM_LOGD("Nodebuilder " PIPELINE_NAME " +");

        typename NodeT::InitParams initParam;
        {
            initParam.openId = SENSOR_OPEN_ID;
            initParam.nodeId = getNodeId<NodeT>();
            initParam.nodeName = PIPELINE_NAME "Node";
        }

        typename NodeT::ConfigParams cfgParam;
        {
            cfgParam.pInAppMeta  = gControlMeta_App;
            cfgParam.pInHalMeta  = gResultMeta_P1_Hal;
            cfgParam.pOutAppMeta = gResultMeta_MFC_App;
            cfgParam.pOutHalMeta = gResultMeta_MFC_Hal;

            if (gImageInfo_fullRaw != 0)
            {
                cfgParam.vInFullRaw.push_back(gImageInfo_fullRaw);
            }

            cfgParam.pInResizedRaw = gImageInfo_resizedRaw;

            if (gImageInfo_yuv00 != 0)
            {
                cfgParam.vOutImage.push_back(gImageInfo_yuv00);
            }

            if (gImageInfo_yuvJpeg != 0)
            {
                cfgParam.vOutYuvJpeg = gImageInfo_yuvJpeg;
            }

            if (gImageInfo_yuvThumbnail != 0)
            {
                cfgParam.vOutYuvThumbnail = gImageInfo_yuvThumbnail;
            }
        }

        sp<NodeActorT> pNode = new NodeActorT(NodeT::createInstance());
        pNode->setInitParam(initParam);
        pNode->setConfigParam(cfgParam);

        StreamSet vIn;
        addStreamToSet(vIn, gImageInfo_fullRaw);
        addStreamToSet(vIn, gImageInfo_resizedRaw);
        addStreamToSet(vIn, gControlMeta_App);
        addStreamToSet(vIn, gResultMeta_P1_Hal);

        StreamSet vOut;
        addStreamToSet(vOut, gImageInfo_yuv00);
        addStreamToSet(vOut, gImageInfo_yuvJpeg);
        addStreamToSet(vOut, gImageInfo_yuvThumbnail);
        addStreamToSet(vOut, gResultMeta_MFC_App);
        addStreamToSet(vOut, gResultMeta_MFC_Hal);

        NodeBuilder nodeBuilder(getNodeId<NodeT>(), pNode);

        nodeBuilder.addStream(
                NodeBuilder::eDirection_IN, vIn);
        nodeBuilder.addStream(
                NodeBuilder::eDirection_OUT, vOut);

        // input
        {
            if (gImageInfo_fullRaw != 0)
            {
                nodeBuilder.setImageStreamUsage(
                        gImageInfo_fullRaw->getStreamId(),
                        eBUFFER_USAGE_SW_READ_OFTEN | eBUFFER_USAGE_HW_CAMERA_READ);
            }

            if (gImageInfo_resizedRaw != 0)
            {
                nodeBuilder.setImageStreamUsage(
                        gImageInfo_resizedRaw->getStreamId(),
                        eBUFFER_USAGE_SW_READ_OFTEN | eBUFFER_USAGE_HW_CAMERA_READ);
            }
        }

        // output
        {
            if (gImageInfo_yuv00 != 0)
            {
                nodeBuilder.setImageStreamUsage(
                        gImageInfo_yuv00->getStreamId(),
                        eBUFFER_USAGE_SW_READ_OFTEN | eBUFFER_USAGE_HW_CAMERA_WRITE);
            }

            if (gImageInfo_yuvJpeg != 0)
            {
                nodeBuilder.setImageStreamUsage(
                        gImageInfo_yuvJpeg->getStreamId(),
                        eBUFFER_USAGE_SW_READ_OFTEN | eBUFFER_USAGE_HW_CAMERA_WRITE);
            }

            if (gImageInfo_yuvThumbnail != 0)
            {
                nodeBuilder.setImageStreamUsage(
                        gImageInfo_yuvThumbnail->getStreamId(),
                        eBUFFER_USAGE_SW_READ_OFTEN | eBUFFER_USAGE_HW_CAMERA_WRITE);
            }
        }

        MERROR ret = nodeBuilder.build(rpPipelineContext);

        CAM_LOGD("Nodebuilder " PIPELINE_NAME " -");

        if (ret != OK)
        {
            CAM_LOGE("build " PIPELINE_NAME " node error");
            return ret;
        }
    }

    flag |= eConfigMFC;

    return ret;
}

MERROR configureJpegNode(sp<PipelineContext>& rpPipelineContext, MUINT32& flag)
{
    MERROR ret = OK;

    // port definition
    // incoming port: App:Meta:Control, Hal:Meta:ResultMFC,
    //                Hal:Imge:yuvJpeg, Hal:Image:yuvThumbnail
    // outgoing port: App:Meta:ResultJpeg, App:Image:Jpeg

    // metadata
    {
        StreamBuilder(eStreamType_META_APP, gResultMeta_Jpeg_App)
            .build(rpPipelineContext);
    }

    // image
    {
        StreamBuilder(eStreamType_IMG_HAL_POOL, gImageInfo_Jpeg)
            .build(rpPipelineContext);
    }

    // configure node
    {
        typedef JpegNode           NodeT;
        typedef NodeActor< NodeT > NodeActorT;

        CAM_LOGD("Nodebuilder Jpeg +");

        NodeT::InitParams initParam;
        {
            initParam.openId = SENSOR_OPEN_ID;
            initParam.nodeId = eNODEID_JpegNode;
            initParam.nodeName = "JpegNode";
        }

        NodeT::ConfigParams cfgParam;
        {
            cfgParam.pInAppMeta       = gControlMeta_App;
            cfgParam.pInHalMeta       = gResultMeta_MFC_Hal;
            cfgParam.pOutAppMeta      = gResultMeta_Jpeg_App;
            cfgParam.pInYuv_Main      = gImageInfo_yuvJpeg;
            cfgParam.pInYuv_Thumbnail = gImageInfo_yuvThumbnail;
            cfgParam.pOutJpeg         = gImageInfo_Jpeg;
        }

        sp<NodeActorT> pNode = new NodeActorT(NodeT::createInstance());
        pNode->setInitParam(initParam);
        pNode->setConfigParam(cfgParam);

        StreamSet vIn;
        addStreamToSet(vIn, gImageInfo_yuvJpeg);
        addStreamToSet(vIn, gImageInfo_yuvThumbnail);
        addStreamToSet(vIn, gControlMeta_App);
        addStreamToSet(vIn, gResultMeta_MFC_Hal);

        StreamSet vOut;
        addStreamToSet(vOut, gImageInfo_Jpeg);
        addStreamToSet(vOut, gResultMeta_Jpeg_App);

        NodeBuilder nodeBuilder(eNODEID_JpegNode, pNode);

        nodeBuilder.addStream(
                NodeBuilder::eDirection_IN, vIn);
        nodeBuilder.addStream(
                NodeBuilder::eDirection_OUT, vOut);

        // input
        {
            if (gImageInfo_yuvJpeg != 0)
            {
                nodeBuilder.setImageStreamUsage(
                        gImageInfo_yuvJpeg->getStreamId(),
                        eBUFFER_USAGE_SW_READ_OFTEN | eBUFFER_USAGE_HW_CAMERA_READWRITE);
            }

            if (gImageInfo_yuvThumbnail != 0)
            {
                nodeBuilder.setImageStreamUsage(
                        gImageInfo_yuvThumbnail->getStreamId(),
                        eBUFFER_USAGE_SW_READ_OFTEN | eBUFFER_USAGE_HW_CAMERA_READWRITE);
            }
        }

        // output
        {
            if (gImageInfo_Jpeg != 0)
            {
                nodeBuilder.setImageStreamUsage(
                        gImageInfo_Jpeg->getStreamId(),
                        eBUFFER_USAGE_SW_MASK | eBUFFER_USAGE_HW_CAMERA_READWRITE);
            }
        }

        MERROR ret = nodeBuilder.build(rpPipelineContext);

        CAM_LOGD("Nodebuilder Jpeg -");

        if (ret != OK)
        {
            CAM_LOGE("build Jpeg node error");
            return ret;
        }
    }

    flag |= eConfigJpeg;

    return ret;
}

template <class NodeT>
void setupPipelineContext()
{
    gContext = PipelineContext::create(PIPELINE_NAME);
    if (!gContext.get())
    {
        CAM_LOGE("create context failed");
        return;
    }

    gContext->beginConfigure();

    // 1. build control metadata stream
    {
        StreamBuilder(eStreamType_META_APP, gControlMeta_App)
            .build(gContext);

        StreamBuilder(eStreamType_META_HAL, gControlMeta_Hal)
            .build(gContext);
    }

    // 2. build nodes
    MUINT32 configFlag = eConfigNone;
    {
        if ((configureP1Node(gContext, configFlag) != OK) ||
            (configureMFCNode<NodeT>(gContext, configFlag) != OK) ||
            (configureJpegNode(gContext, configFlag) != OK))
        {
            CAM_LOGE("congiure nodes failed");
            gContext->endConfigure();
            return;
        }
    }

    // 3. build pipeline
    {
        // edge
        NodeEdgeSet edgeSet;
        if (configFlag & eConfigMFC)
        {
            // TODO: change to MFC node id
            edgeSet.addEdge(eNODEID_P1Node, getNodeId<NodeT>());
        }

        if (configFlag & eConfigJpeg)
        {
            edgeSet.addEdge(getNodeId<NodeT>(), eNODEID_JpegNode);
        }

        MERROR ret = PipelineBuilder()
            .setRootNode(
                    NodeSet().add(eNODEID_P1Node))
            .setNodeEdges(edgeSet)
            .build(gContext);

        if (ret != OK)
        {
            CAM_LOGE("build pipeline failed");
            return;
        }
    }

    gContext->endConfigure();
}

sp<HalMetaStreamBuffer> createMetaStreamBuffer(
        android::sp<IMetaStreamInfo> pStreamInfo,
        IMetadata const& rSettings,
        MBOOL const repeating __attribute__((unused)))
{
    HalMetaStreamBuffer* pStreamBuffer =
    HalMetaStreamBuffer::Allocator(pStreamInfo.get())(rSettings);

    return pStreamBuffer;
}

template <typename T>
inline MVOID updateEntry(
        IMetadata* pMetadata,
        MUINT32 const tag,
        T const& val)
{
    if (NULL == pMetadata)
    {
        CAM_LOGE("pMetadata == NULL");
        return;
    }

    IMetadata::IEntry entry(tag);
    entry.push_back(val, Type2Type<T>());
    pMetadata->update(tag, entry);
}

void updateRequst(IMetadata& metadata)
{
    // update jpeg info
    updateEntry<MINT32>(&metadata, MTK_JPEG_ORIENTATION, JPEG_ORIENTATION);
    updateEntry<MUINT8>(&metadata, MTK_JPEG_QUALITY, JPEG_QUALITY);
    updateEntry<MUINT8>(&metadata, MTK_JPEG_THUMBNAIL_QUALITY, JPEG_THUMBNAIL_QUALITY);
    updateEntry<MSize>(&metadata, MTK_JPEG_THUMBNAIL_SIZE, MSize(YUV_THUMBNAIL_WIDTH, YUV_THUMBNAIL_HEIGHT));

    // set HDR scene mode
    updateEntry<MUINT8>(&metadata, MTK_CONTROL_SCENE_MODE, MTK_CONTROL_SCENE_MODE_HDR);
    updateEntry<MUINT8>(&metadata, MTK_CONTROL_MODE, MTK_CONTROL_MODE_USE_SCENE_MODE);
}

sp<IMetaStreamBuffer> getDefaultRequest()
{
    // create template request object
    ITemplateRequest* obj = NSTemplateRequestManager::valueFor(SENSOR_OPEN_ID);
    if (obj == NULL)
    {
        obj = ITemplateRequest::getInstance(SENSOR_OPEN_ID);
        NSTemplateRequestManager::add(SENSOR_OPEN_ID, obj);
    }

    // get default capture request template
    IMetadata metadata = obj->getMtkData(REQUEST_TEMPLATE);

    // update request
    updateRequst(metadata);

    return createMetaStreamBuffer(gControlMeta_App, metadata, false);
}

template <class NodeT>
sp<IPipelineFrame> buildMainRequest(
        size_t requestNo, const NS3Av3::CaptureParam_T& captureParams)
{
    // build main request and return the corresponding pipeline frame
    sp<RequestBuilder> requestBuilder = new RequestBuilder();
    {
        // P1
        requestBuilder->setIOMap(
                eNODEID_P1Node,
                IOMapSet()
                .add(IOMap()
                    .addOut(gImageInfo_fullRaw->getStreamId())
                    .addOut(gImageInfo_resizedRaw->getStreamId())),
                IOMapSet()
                .add(IOMap()
                    .addIn(gControlMeta_App->getStreamId())
                    .addIn(gControlMeta_Hal->getStreamId())
                    .addOut(gResultMeta_P1_App->getStreamId())
                    .addOut(gResultMeta_P1_Hal->getStreamId())));

        // MFC
        requestBuilder->setIOMap(
                getNodeId<NodeT>(),
                IOMapSet()
                .add(IOMap()
                    .addIn(gImageInfo_fullRaw->getStreamId())
                    .addOut(gImageInfo_yuvJpeg->getStreamId())
                    .addOut(gImageInfo_yuvThumbnail->getStreamId()))
                .add(IOMap()
                    .addIn(gImageInfo_resizedRaw->getStreamId())
                    .addOut(gImageInfo_yuv00->getStreamId())),
                IOMapSet()
                .add(IOMap()
                    .addIn(gControlMeta_App->getStreamId())
                    .addIn(gResultMeta_P1_Hal->getStreamId())
                    .addOut(gResultMeta_MFC_App->getStreamId())
                    .addOut(gResultMeta_MFC_Hal->getStreamId())));

        // Jpeg
        requestBuilder->setIOMap(
                eNODEID_JpegNode,
                IOMapSet()
                .add(IOMap()
                    .addIn(gImageInfo_yuvJpeg->getStreamId())
                    .addIn(gImageInfo_yuvThumbnail->getStreamId())
                    .addOut(gImageInfo_Jpeg->getStreamId())),
                IOMapSet()
                .add(IOMap()
                    .addIn(gControlMeta_App->getStreamId())
                    .addIn(gResultMeta_MFC_Hal->getStreamId())
                    .addOut(gResultMeta_Jpeg_App->getStreamId())));

        // set root node
        requestBuilder->setRootNode(NodeSet().add(eNODEID_P1Node));

        // set node edges
        requestBuilder->setNodeEdges(
                NodeEdgeSet()
                .addEdge(eNODEID_P1Node, getNodeId<NodeT>())
                .addEdge(getNodeId<NodeT>(), eNODEID_JpegNode));

        // create meta stream buffers and set them into request builder
        {
            sp<IMetaStreamBuffer> appMetaStreamBuffer;
            sp<HalMetaStreamBuffer> halMetaStreamBuffer;

            appMetaStreamBuffer = getDefaultRequest();

            IMetadata halMetadata;
            halMetaStreamBuffer =
                createMetaStreamBuffer(gControlMeta_Hal, halMetadata, false);

            // modify hal control metadata
            IMetadata* pMetadata = halMetaStreamBuffer->tryWriteLock(LOG_TAG);
            {
                // set sensor size
                updateEntry<MSize>(pMetadata, MTK_HAL_REQUEST_SENSOR_SIZE, gSensorParam.size);

                // update capture params
                {
                    IMetadata::Memory capParams;
                    capParams.resize(sizeof(NS3Av3::CaptureParam_T));
                    memcpy(capParams.editArray(), &captureParams, sizeof(NS3Av3::CaptureParam_T));

#ifdef DEBUG_ENABLE_AE_CAP
                    updateEntry<IMetadata::Memory>(pMetadata, MTK_3A_AE_CAP_PARAM, capParams);
#endif
                }

                // add request for 3A EXIF
                updateEntry<MUINT8>(pMetadata, MTK_HAL_REQUEST_REQUIRE_EXIF, 1);
            }
            halMetaStreamBuffer->unlock(LOG_TAG, pMetadata);

            // set meta stream buffer
            requestBuilder->setMetaStreamBuffer(
                    gControlMeta_App->getStreamId(),
                    appMetaStreamBuffer);

            requestBuilder->setMetaStreamBuffer(
                    gControlMeta_Hal->getStreamId(),
                    halMetaStreamBuffer);
        }
    }

    return requestBuilder->build(requestNo, gContext);
}

template <class NodeT>
sp<IPipelineFrame> buildSubRequest(
        size_t requestNo, const NS3Av3::CaptureParam_T& captureParams)
{
    // build sub-request and return the corresponding pipeline frame
    sp<RequestBuilder> requestBuilder = new RequestBuilder();
    {
        // P1
        requestBuilder->setIOMap(
                eNODEID_P1Node,
                IOMapSet()
                .add(IOMap()
                    .addOut(gImageInfo_fullRaw->getStreamId())
                    .addOut(gImageInfo_resizedRaw->getStreamId())),
                IOMapSet()
                .add(IOMap()
                    .addIn(gControlMeta_App->getStreamId())
                    .addIn(gControlMeta_Hal->getStreamId())
                    .addOut(gResultMeta_P1_App->getStreamId())
                    .addOut(gResultMeta_P1_Hal->getStreamId())));

        // MFC
        requestBuilder->setIOMap(
                getNodeId<NodeT>(),
                IOMapSet()
                .add(IOMap()
                    .addIn(gImageInfo_fullRaw->getStreamId())),
                IOMapSet()
                .add(IOMap()
                    .addIn(gControlMeta_App->getStreamId())
                    .addIn(gResultMeta_P1_Hal->getStreamId())));

        // set root node
        requestBuilder->setRootNode(NodeSet().add(eNODEID_P1Node));

        // set node edges
        requestBuilder->setNodeEdges(
                NodeEdgeSet()
                .addEdge(eNODEID_P1Node, getNodeId<NodeT>()));

        // create meta stream buffers and set them into request builder
        {
            sp<IMetaStreamBuffer> appMetaStreamBuffer;
            sp<HalMetaStreamBuffer> halMetaStreamBuffer;

            appMetaStreamBuffer = getDefaultRequest();

            IMetadata halMetadata;
            halMetaStreamBuffer =
                createMetaStreamBuffer(gControlMeta_Hal, halMetadata, false);

            // modify hal control metadata
            IMetadata* pMetadata = halMetaStreamBuffer->tryWriteLock(LOG_TAG);
            {
                // set sensor size
                updateEntry<MSize>(pMetadata, MTK_HAL_REQUEST_SENSOR_SIZE, gSensorParam.size);

                // update capture params
                {
                    IMetadata::Memory capParams;
                    capParams.resize(sizeof(NS3Av3::CaptureParam_T));
                    memcpy(capParams.editArray(), &captureParams, sizeof(NS3Av3::CaptureParam_T));

#ifdef DEBUG_ENABLE_AE_CAP
                    updateEntry<IMetadata::Memory>(pMetadata, MTK_3A_AE_CAP_PARAM, capParams);
#endif
                }

                // add request for 3A EXIF
                updateEntry<MUINT8>(pMetadata, MTK_HAL_REQUEST_REQUIRE_EXIF, 1);
            }
            halMetaStreamBuffer->unlock(LOG_TAG, pMetadata);

            // set meta stream buffer
            requestBuilder->setMetaStreamBuffer(
                    gControlMeta_App->getStreamId(),
                    appMetaStreamBuffer);

            requestBuilder->setMetaStreamBuffer(
                    gControlMeta_Hal->getStreamId(),
                    halMetaStreamBuffer);
        }
    }

    return requestBuilder->build(requestNo, gContext);
}

template <class NodeT>
void processRequest()
{
    // TODO: get capture parameter from MFC node
    // query MFC node
    sp< NodeActor<NodeT> > nodeActor;
    if (NAME_NOT_FOUND == gContext->queryNodeActor(getNodeId<NodeT>(), nodeActor))
    {
        CAM_LOGE("cannot find HDR node");
        return;
    }

    sp<NodeT> nodeImpl = nodeActor->getNodeImpl();
    if (nodeImpl == NULL)
    {
        CAM_LOGE("get derived node failed");
        return;
    }

    // get capture parameters from Node
    Vector<NS3Av3::CaptureParam_T> vCaptureParams;
    HDRNode::getCaptureParamSet(SENSOR_OPEN_ID, vCaptureParams);

    CAM_LOGD("subrequest count(%zu)", vCaptureParams.size());

    for (size_t i = 0; i < vCaptureParams.size(); i++)
    {
        CAM_LOGD("request %zu +", i);

        // build and get pipeline frame
        sp<IPipelineFrame> frame;

        if (i == 0)
        {
            frame = buildMainRequest<NodeT>(i, vCaptureParams[i]);
        }
        else
        {
            frame = buildSubRequest<NodeT>(i, vCaptureParams[i]);
        }

        // queue pipeline frame into pipeline context
        if (frame.get())
        {
            if (OK != gContext->queue(frame))
            {
                CAM_LOGE("queue pipeline frame %zu failed", i);
            }
        }
        else
        {
            CAM_LOGE("build request %zu failed", i);
        }

        // clear frame
        frame.clear();

        CAM_LOGD("request %zu -", i);
    }
}

void finishPipelineContext()
{
    gContext->waitUntilDrained();
    gContext = NULL;
}

#define addBufPlane(planes, height, stride)                                      \
        do {                                                                     \
            size_t _height = (size_t)(height);                                   \
            size_t _stride = (size_t)(stride);                                   \
            IImageStreamInfo::BufPlane bufPlane= { _height * _stride, _stride }; \
            planes.push_back(bufPlane);                                          \
        } while(0)

sp<IImageStreamInfo> createImageStreamInfo(
    char const*         streamName,
    StreamId_T          streamId,
    MUINT32             streamType,
    size_t              maxBufNum,
    size_t              minInitBufNum,
    MUINT               usageForAllocator,
    MINT                imgFormat,
    MSize const&        imgSize,
    MUINT32             transform)
{
    IImageStreamInfo::BufPlanes_t bufPlanes;

    switch (imgFormat)
    {
        case eImgFmt_YV12:
            addBufPlane(bufPlanes, imgSize.h, imgSize.w);
            addBufPlane(bufPlanes, imgSize.h >> 1, imgSize.w >> 1);
            addBufPlane(bufPlanes, imgSize.h >> 1, imgSize.w >> 1);
            break;
        case eImgFmt_NV21:
            addBufPlane(bufPlanes, imgSize.h, imgSize.w);
            addBufPlane(bufPlanes, imgSize.h >> 1, imgSize.w);
            break;
        case eImgFmt_YUY2:
            addBufPlane(bufPlanes, imgSize.h, imgSize.w << 1);
            break;
        case eImgFmt_BLOB:
            addBufPlane(bufPlanes, 1, imgSize.w * imgSize.h * 12 / 10);
            break;
        default:
            CAM_LOGE("format not support yet %p", imgFormat);
            break;
    }

    sp<ImageStreamInfo>
        pStreamInfo = new ImageStreamInfo(
                streamName,
                streamId,
                streamType,
                maxBufNum, minInitBufNum,
                usageForAllocator, imgFormat, imgSize, bufPlanes, transform);

    if (pStreamInfo == NULL)
    {
        CAM_LOGE("create ImageStream failed, %s, %#" PRIxPTR,
                streamName, streamId);
    }
    else
    {
        CAM_LOGD("create ImageStreamInfo %s", streamName);
    }

    return pStreamInfo;
}

sp<IImageStreamInfo> createRawImageStreamInfo(
    char const*  streamName,
    StreamId_T   streamId,
    MUINT32      streamType,
    size_t       maxBufNum,
    size_t       minInitBufNum,
    MUINT        usageForAllocator,
    MINT         imgFormat,
    MSize const& imgSize,
    size_t const stride)
{
    IImageStreamInfo::BufPlanes_t bufPlanes;

    switch (imgFormat)
    {
        case eImgFmt_BAYER10:
        case eImgFmt_BAYER12:
        case eImgFmt_FG_BAYER10:
        case eImgFmt_FG_BAYER12:
            addBufPlane(bufPlanes, imgSize.h, stride);
            break;
        default:
            CAM_LOGE("format not support yet %p", imgFormat);
            break;
    }

    sp<ImageStreamInfo>
        pStreamInfo = new ImageStreamInfo(
                streamName,
                streamId,
                streamType,
                maxBufNum, minInitBufNum,
                usageForAllocator, imgFormat, imgSize, bufPlanes);

    if (pStreamInfo == NULL)
    {
        CAM_LOGE("create ImageStream failed, %s, %#" PRIxPTR,
                streamName, streamId);
    }
    else
    {
        CAM_LOGD("create ImageStreamInfo %s", streamName);
    }

    return pStreamInfo;
}
#undef  addBufPlane

template <class T>
void run_test(void)
{
    CAM_LOGD("start test");

    // prepare sensor
    prepareSensor();

    CAM_LOGE_IF(!gSensorParam.size, "invalid sensor size(%dx%d)",
            gSensorParam.size.w, gSensorParam.size.h);

    // prepare configuration
    prepareConfiguration();

    CAM_LOGE_IF(!gRrzoSize, "invalid rrzo size(%dx%d)",
        gRrzoSize.w, gRrzoSize.h);

    CAM_LOGE_IF(gRrzoStride == 0, "invalid rrzo stride(%zu)", gRrzoStride);

    // set up metadata stream information
    setupMetaStreamInfo();

    // set up image stream information
    setupImageStreamInfo();

    setupPipelineContext<T>();

    processRequest<T>();

    finishPipelineContext();

    closeSensor();

    CAM_LOGD("end of test");
}

/**
 *  To run unit test for HDRNode or MfllNode.
 *  To test HDRNode, run binary with no argument or argument value 0
 *
 *      ./test_pipelinecontext_MFC 0
 *
 *  To test MfllNode, run binary with an argument which isn't 0
 *
 *      ./test_pipelinecontext_MFC 1
 */
int main(int argc , char** argv)
{
    int testId = 0;
    for (int i = 1; i < argc; i++) {
        testId = ::atoi(argv[i]);
    }

    if (testId == 0) {
#if TEST_HDR
        run_test<HDRNode>();
#else
        CAM_LOGD("Not support HDR");
        printf("Not support HDR\n");
#endif
    }
    else {
#if TEST_MFLL
        run_test<MfllNode>();
#else
        CAM_LOGD("Not support MFLL");
        printf("Not support MFLL\n");
#endif
    }

    return 0;
}

