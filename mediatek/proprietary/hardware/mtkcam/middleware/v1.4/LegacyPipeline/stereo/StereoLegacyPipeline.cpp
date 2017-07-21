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
#include <LegacyPipeline/stereo/ContextBuilder/ImageStreamManager.h>
#include <LegacyPipeline/stereo/ContextBuilder/MetaStreamManager.h>
#include "StereoLegacyPipeline.h"
#include <v3/utils/streambuf/StreamBuffers.h>
#include <mtk_platform_metadata_tag.h>
//
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
#define MY_LOGV(fmt, arg...)        CAM_LOGV("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_LOGD("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_LOGI("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_LOGW("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_LOGE("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGA(fmt, arg...)        CAM_LOGA("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGF(fmt, arg...)        CAM_LOGF("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)

#define MY_LOGV_IF(cond, ...)       do { if ( (cond) ) { MY_LOGV(__VA_ARGS__); } }while(0)
#define MY_LOGD_IF(cond, ...)       do { if ( (cond) ) { MY_LOGD(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) ) { MY_LOGI(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) ) { MY_LOGW(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) ) { MY_LOGE(__VA_ARGS__); } }while(0)
#define MY_LOGA_IF(cond, ...)       do { if ( (cond) ) { MY_LOGA(__VA_ARGS__); } }while(0)
#define MY_LOGF_IF(cond, ...)       do { if ( (cond) ) { MY_LOGF(__VA_ARGS__); } }while(0)

#define FUNC_START                  MY_LOGD("+")
#define FUNC_END                    MY_LOGD("-")

/******************************************************************************
*
*******************************************************************************/
StereoLegacyPipeline::
StereoLegacyPipeline(
    MINT32 const openId,
    MINT32 const mode
) : miOpenId(openId),
    miMode(mode)
{
    mTimestamp = TimeTool::getReadableTime();
}
/******************************************************************************
*
*******************************************************************************/
StereoLegacyPipeline::
~StereoLegacyPipeline()
{}
/******************************************************************************
*
*******************************************************************************/
sp<StereoLegacyPipeline>
StereoLegacyPipeline::
createInstance(
    MINT32 const openId,
    MINT32 const mode
)
{
    return new StereoLegacyPipeline(openId, mode);
}
/******************************************************************************
*
*******************************************************************************/
MVOID
StereoLegacyPipeline::
onLastStrongRef( const void* /*id*/)
{
    FUNC_START;

    if ( mpPipelineContext != 0 ){
        mpPipelineContext->flush();
        mpPipelineContext->waitUntilDrained();
        mpPipelineContext = NULL;
    }
    mpPipelineContext       = NULL;
    mpResultProcessor       = NULL;
    mpTimestampProcessor    = NULL;
    mpRequestBuilder        = NULL;
    mpImageStreamManager    = NULL;
    mpMetaStreamManager     = NULL;
    mpNodeConfigDataManager = NULL;
    //

    FUNC_END;
}

/******************************************************************************
*
*******************************************************************************/
MERROR
StereoLegacyPipeline::
submitSetting(
    MINT32    const requestNo,
    IMetadata& appMeta,
    IMetadata& halMeta,
    ResultSet* /*pResultSet*/
)
{
    if( !trySetMetadata<MINT32>(halMeta, MTK_PIPELINE_UNIQUE_KEY, mTimestamp) )
       MY_LOGE("set unique key failed");

    sp<HalMetaStreamBuffer> pHalMetaStreamBuffer =
           HalMetaStreamBufferAllocatorT(
                queryMetaStreamInfo(mHalMetaStreamId).get()
           )(halMeta);

    sp<IMetaStreamBuffer> pAppMetaStreamBuffer =
           HalMetaStreamBufferAllocatorT(
                queryMetaStreamInfo(mAppMetaStreamId).get()
           )(appMeta);

#warning "WHP repeating"
    //pAppMetaStreamBuffer->setRepeating(repeating);??

    mpRequestBuilder->setMetaStreamBuffer(
                mAppMetaStreamId,
                pAppMetaStreamBuffer
                );
    mpRequestBuilder->setMetaStreamBuffer(
                mHalMetaStreamId,
                pHalMetaStreamBuffer
                );
    sp<IPipelineFrame> pFrame =
        mpRequestBuilder->build(requestNo, mpPipelineContext);

    if( ! pFrame.get() ) {
        MY_LOGE("build request failed");
        return UNKNOWN_ERROR;
    }

    mpPipelineContext->queue(pFrame);

    return OK;
}

/******************************************************************************
*
*******************************************************************************/
MERROR
StereoLegacyPipeline::
submitRequest(
    MINT32    const requestNo,
    IMetadata& appMeta,
    IMetadata& halMeta,
    Vector<StreamId> vDstStreams
)
{
    MY_LOGE("Not implement.");
    return OK;
}

/******************************************************************************
*
*******************************************************************************/
sp<IImageStreamInfo>
StereoLegacyPipeline::
queryImageStreamInfo(StreamId const streamId)
{
    if(mpImageStreamManager == NULL)
    {
        MY_LOGE("ImageStreamManager is NULL.");
        return NULL;
    }

    return mpImageStreamManager->getStreamInfoByStreamId(streamId);
}

/******************************************************************************
*
*******************************************************************************/
sp<IMetaStreamInfo>
StereoLegacyPipeline::
queryMetaStreamInfo(StreamId const streamId)
{
    if(mpMetaStreamManager == NULL)
    {
        MY_LOGE("MetaStreamManager is NULL.");
        return NULL;
    }

    return mpMetaStreamManager->getStreamInfoByStreamId(streamId);
}

/******************************************************************************
*
*******************************************************************************/
sp<StreamBufferProvider>
StereoLegacyPipeline::
queryProvider(StreamId const streamId)
{
    if(mpImageStreamManager == NULL)
    {
        MY_LOGE("ImageStreamManager is NULL.");
        return NULL;
    }

    return mpImageStreamManager->getBufProviderByStreamId(streamId);
}
