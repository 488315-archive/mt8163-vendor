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

#define LOG_TAG "MtkCam/LPipelinePrv"

//
#include "MyUtils.h"
#include "PipelineBuilderPreview.h"
#include "LegacyPipelinePreview.h"

#include <v3/utils/streambuf/StreamBuffers.h>
#include <mtk_platform_metadata_tag.h>

using namespace android;
using namespace NSCam;
using namespace NSCam::Utils;
using namespace NSCam::v3;
using namespace NSCam::v3::NSPipelineContext;
using namespace NSCam::v1;
using namespace NSCam::v1::NSLegacyPipeline;

typedef NSCam::v3::Utils::HalMetaStreamBuffer::Allocator  HalMetaStreamBufferAllocatorT;

/******************************************************************************
*
*******************************************************************************/
#define MY_LOGV(fmt, arg...)        CAM_LOGV("[%d:%s] " fmt, getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_LOGD("[%d:%s] " fmt, getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_LOGI("[%d:%s] " fmt, getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_LOGW("[%d:%s] " fmt, getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_LOGE("[%d:%s] " fmt, getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGA(fmt, arg...)        CAM_LOGA("[%d:%s] " fmt, getOpenId(), __FUNCTION__, ##arg)
#define MY_LOGF(fmt, arg...)        CAM_LOGF("[%d:%s] " fmt, getOpenId(), __FUNCTION__, ##arg)
//
#define MY_LOGV_IF(cond, ...)       do { if ( (cond) ) { MY_LOGV(__VA_ARGS__); } }while(0)
#define MY_LOGD_IF(cond, ...)       do { if ( (cond) ) { MY_LOGD(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) ) { MY_LOGI(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) ) { MY_LOGW(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) ) { MY_LOGE(__VA_ARGS__); } }while(0)
#define MY_LOGA_IF(cond, ...)       do { if ( (cond) ) { MY_LOGA(__VA_ARGS__); } }while(0)
#define MY_LOGF_IF(cond, ...)       do { if ( (cond) ) { MY_LOGF(__VA_ARGS__); } }while(0)
//
#define FUNC_START                  MY_LOGD("+")
#define FUNC_END                    MY_LOGD("-")
//
/******************************************************************************
*
*******************************************************************************/
LegacyPipelinePreview::
LegacyPipelinePreview(MINT32 openId)
    : mpPipelineContext(NULL)
    , mpResultProcessor(NULL)
    , mpTimestampProcessor(NULL)
    , mOpenId(openId)
{
    mLogLevel = property_get_int32("debug.camera.log", 0);
    if ( mLogLevel == 0 ) {
        mLogLevel = property_get_int32("debug.camera.log.LegacyPipeline", 1);
    }
    mTimestamp = TimeTool::getReadableTime();
}


/******************************************************************************
*
*******************************************************************************/
LegacyPipelinePreview::
~LegacyPipelinePreview()
{}

/******************************************************************************
*
*******************************************************************************/
MVOID
LegacyPipelinePreview::
onLastStrongRef( const void* /*id*/)
{
    FUNC_START;

    if ( mpPipelineContext != 0 ){
        mpPipelineContext->flush();
        mpPipelineContext->waitUntilDrained();
        mpPipelineContext = NULL;
    }
    mpResultProcessor    = NULL;
    mpTimestampProcessor = NULL;
    mpRequestBuilder     = NULL;
    //
    int n = vStreamBufferProvider.size();
    for(int i=0; i<n; i++)
    {
        vStreamBufferProvider.editValueAt(i)->flush();
    }
    //
    vStreamBufferProvider.clear();
    vImageStreamInfo.clear();

    FUNC_END;
}

/******************************************************************************
*
*******************************************************************************/
MERROR
LegacyPipelinePreview::
submitSetting(
    MINT32 const requestNo,
    IMetadata& appMeta,
    IMetadata& halMeta,
    ResultSet* /*pResultSet*/
)
{
    MY_LOGD_IF( mLogLevel >= 1, "submitSetting %d +", requestNo);
    CAM_TRACE_FMT_BEGIN("LP:[Prv]submitSetting %d",requestNo);

    if( !trySetMetadata<MINT32>(halMeta, MTK_PIPELINE_UNIQUE_KEY, mTimestamp) )
        MY_LOGE("set unique key failed");

    sp<HalMetaStreamBuffer> pHalMetaStreamBuffer =
           HalMetaStreamBufferAllocatorT(
                queryMetaStreamInfo(eSTREAMID_META_HAL_CONTROL).get()
           )(halMeta);

    sp<IMetaStreamBuffer> pAppMetaStreamBuffer =
           HalMetaStreamBufferAllocatorT(
                queryMetaStreamInfo(eSTREAMID_META_APP_CONTROL).get()
           )(appMeta);

    mpRequestBuilder->setMetaStreamBuffer(
                eSTREAMID_META_APP_CONTROL,
                pAppMetaStreamBuffer
                );
    mpRequestBuilder->setMetaStreamBuffer(
                eSTREAMID_META_HAL_CONTROL,
                pHalMetaStreamBuffer
                );
    sp<IPipelineFrame> pFrame =
        mpRequestBuilder->build(requestNo, mpPipelineContext);

    if( ! pFrame.get() ) {
        MY_LOGE("build request failed");
        CAM_TRACE_FMT_END();
        return UNKNOWN_ERROR;
    }

    mpPipelineContext->queue(pFrame);
    //
    MY_LOGD_IF( mLogLevel >= 1, "submitSetting %d -", requestNo);
    CAM_TRACE_FMT_END();
    return OK;
}

/******************************************************************************
*
*******************************************************************************/
MERROR
LegacyPipelinePreview::
submitRequest(
    MINT32 const requestNo,
    IMetadata& appMeta,
    IMetadata& halMeta,
    Vector<StreamId> vDstStreams
)
{
    MY_LOGD_IF( mLogLevel >= 1, "submitSetting %d vDstStreams:%d +", requestNo, vDstStreams.size());
    CAM_TRACE_FMT_BEGIN("LP:[Prv]submitRequest No%d",requestNo);
    RequestBuilder aRequestBuilder;
    sp<HalMetaStreamBuffer> pHalMetaStreamBuffer =
           HalMetaStreamBufferAllocatorT(
                queryMetaStreamInfo(eSTREAMID_META_HAL_CONTROL).get()
           )(halMeta);

    sp<IMetaStreamBuffer> pAppMetaStreamBuffer =
           HalMetaStreamBufferAllocatorT(
                queryMetaStreamInfo(eSTREAMID_META_APP_CONTROL).get()
           )(appMeta);

    // p1 node
    {
        MBOOL aNeedP1 = false;
        IOMap p1_Image_IOMap;
        for ( size_t i = 0; i < vDstStreams.size(); ++i ) {
            if ( vDstStreams[i] == eSTREAMID_IMAGE_PIPE_RAW_OPAQUE ) {
                aNeedP1 = true;
                p1_Image_IOMap.addOut(eSTREAMID_IMAGE_PIPE_RAW_OPAQUE);
            } else
            if ( vDstStreams[i] == eSTREAMID_IMAGE_PIPE_RAW_RESIZER ) {
                aNeedP1 = true;
                p1_Image_IOMap.addOut(eSTREAMID_IMAGE_PIPE_RAW_RESIZER);
            }
        }
        if ( aNeedP1 ) {
            aRequestBuilder.setIOMap(
                    eNODEID_P1Node,
                    IOMapSet().add(
                        p1_Image_IOMap
                        ),
                    IOMapSet().add(
                        IOMap()
                        .addIn(eSTREAMID_META_APP_CONTROL)
                        .addIn(eSTREAMID_META_HAL_CONTROL)
                        .addOut(eSTREAMID_META_APP_DYNAMIC_P1)
                        .addOut(eSTREAMID_META_HAL_DYNAMIC_P1)
                        )
                    );
            aRequestBuilder.setRootNode(
                NodeSet().add(eNODEID_P1Node)
                );
        } else {
            MY_LOGE("request:%d No P1 node. preview pipeline does not support memory in.", requestNo);
            CAM_TRACE_FMT_END();
            return BAD_VALUE;
        }
    }

    // p2 node
    {
        MBOOL aNeedP2 = false;
        MBOOL aHasResizeRaw = false;
        IOMap p2_Image_IOMap;
        for ( size_t i = 0; i < vDstStreams.size(); ++i ) {
            if ( vDstStreams[i] == eSTREAMID_IMAGE_PIPE_YUV_00 ) {
                aNeedP2 = true;
                p2_Image_IOMap.addOut(eSTREAMID_IMAGE_PIPE_YUV_00);
            } else
            if ( vDstStreams[i] == eSTREAMID_IMAGE_PIPE_YUV_01 ) {
                aNeedP2 = true;
                p2_Image_IOMap.addOut(eSTREAMID_IMAGE_PIPE_YUV_01);
            } else
            if ( vDstStreams[i] == eSTREAMID_IMAGE_YUV_FD ) {
                aNeedP2 = true;
                p2_Image_IOMap.addOut(eSTREAMID_IMAGE_YUV_FD);
            } else
            if ( vDstStreams[i] == eSTREAMID_IMAGE_PIPE_RAW_RESIZER ) {
                aHasResizeRaw = true;
            }
        }
        if ( aNeedP2 ) {
            if ( aHasResizeRaw ) p2_Image_IOMap.addIn(eSTREAMID_IMAGE_PIPE_RAW_RESIZER);
            else MY_LOGW("request:%d No resized raw as p2 input, something wrong ?", requestNo);

            aRequestBuilder.setIOMap(
                    eNODEID_P2Node,
                    IOMapSet().add(
                        p2_Image_IOMap
                        ),
                    IOMapSet().add(
                        IOMap()
                        .addIn(eSTREAMID_META_APP_CONTROL)
                        .addIn(eSTREAMID_META_HAL_DYNAMIC_P1)
                        .addOut(eSTREAMID_META_APP_DYNAMIC_P2)
                        .addOut(eSTREAMID_META_HAL_DYNAMIC_P1)
                        )
                    );
            aRequestBuilder.setNodeEdges(
                NodeEdgeSet().addEdge(eNODEID_P1Node, eNODEID_P2Node)
                );
        } else {
            MY_LOGD("request:%d No P2 node", requestNo);
        }
    }

    aRequestBuilder.updateFrameCallback(mpResultProcessor);

    sp<IPipelineFrame> pFrame =
        aRequestBuilder
        .setMetaStreamBuffer(
                eSTREAMID_META_APP_CONTROL,
                pAppMetaStreamBuffer
                )
        .setMetaStreamBuffer(
                eSTREAMID_META_HAL_CONTROL,
                pHalMetaStreamBuffer
                )
        .build(requestNo, mpPipelineContext);

    if( ! pFrame.get() ) {
        MY_LOGE("build request failed");
        CAM_TRACE_FMT_END();
        return UNKNOWN_ERROR;
    }

    mpPipelineContext->queue(pFrame);
    //
    MY_LOGD_IF( mLogLevel >= 1, "submitSetting %d vDstStreams:%d -", requestNo, vDstStreams.size());
    CAM_TRACE_FMT_END();
    return OK;
}

/******************************************************************************
*
*******************************************************************************/
MVOID
LegacyPipelinePreview::
setStreamBufferProvider(
    sp<StreamBufferProvider> const pStreamBufferProvider
)
{
    if ( pStreamBufferProvider == 0 ) {
        MY_LOGE("NULL StreamBufferProvider");
        return;
    }

    vStreamBufferProvider.add(pStreamBufferProvider->queryImageStreamInfo()->getStreamId(), pStreamBufferProvider);
}

/******************************************************************************
*
*******************************************************************************/
MVOID
LegacyPipelinePreview::
setImageStreamInfo(
    sp<IImageStreamInfo> const pImageStreamInfo
)
{
    if ( pImageStreamInfo == 0 ) {
        MY_LOGE("NULL ImageStreamInfo");
        return;
    }

    vImageStreamInfo.add(pImageStreamInfo->getStreamId(), pImageStreamInfo);
}

/******************************************************************************
*
*******************************************************************************/
MVOID
LegacyPipelinePreview::
setMetaStreamInfo(
    sp<IMetaStreamInfo> const pMetaStreamInfo
)
{
    if ( pMetaStreamInfo == 0 ) {
        MY_LOGE("NULL MetaStreamInfo");
        return;
    }

    vMetaStreamInfo.add(pMetaStreamInfo->getStreamId(), pMetaStreamInfo);
}
