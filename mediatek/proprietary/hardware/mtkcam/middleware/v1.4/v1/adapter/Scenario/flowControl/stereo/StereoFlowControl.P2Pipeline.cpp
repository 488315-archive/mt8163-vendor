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

#define LOG_TAG "MtkCam/StereoFlowControl"
//
#include "MyUtils.h"
//
#include <v3/utils/streaminfo/MetaStreamInfo.h>
#include <v3/utils/streaminfo/ImageStreamInfo.h>
#include <v3/utils/streambuf/StreamBuffers.h>

#include <LegacyPipeline/StreamId.h>
#include <LegacyPipeline/NodeId.h>
#include <v1/IParamsManager.h>
#include <LegacyPipeline/ILegacyPipeline.h>
#include <LegacyPipeline/stereo/StereoLegacyPipelineDef.h>
#include <LegacyPipeline/stereo/StereoLegacyPipelineBuilder.h>
#include <LegacyPipeline/buffer/StreamBufferProviderFactory.h>

#include <v1/camutils/IImgBufQueue.h>
#include "../../../buffer/ClientBufferPool.h"

#include "StereoFlowControl.h"

using namespace NSCam;
using namespace NSCam::v1;
using namespace NSCam::v1::NSLegacyPipeline;
using namespace android;
using namespace NSCam::v3;
using namespace NSCamHW;

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

#define FUNC_START     MY_LOGD("+")
#define FUNC_END       MY_LOGD("-")

/******************************************************************************
 * create p2 prv pipeline
 ******************************************************************************/
sp<ILegacyPipeline>
StereoFlowControl::
constructP2Pipeline_Prv(
    const MetaStreamManager::metadata_info_setting* metaTable,
    const MINT32* nodeConfigData,
    ImageStreamManager::image_stream_info_pool_setting* imageTable
)
{
    FUNC_START;
    // create legacy pipeline builder
    sp<StereoLegacyPipelineBuilder> pBuilder =
            new StereoLegacyPipelineBuilder(
                                        getOpenId_P2Prv(),
                                        "P2_Pipeline",
                                        StereoLegacyPipelineMode_T::STPipelineMode_P2);
    if ( pBuilder == 0 ) {
        MY_LOGE("Cannot create LegacyPipelineBuilder.");
        return MFALSE;
    }
    sp<MetaStreamManager> pMetaStreamManager =
            MetaStreamManager::create(metaTable);
    if(pMetaStreamManager == NULL)
    {
        MY_LOGE("Create MetaStreamManager fail.");
        return NULL;
    }
    //Update P1 information to image stream table
    {
#define updateItem(streamId, rcContainer) \
        do {\
            sp<StreamBufferProvider> pProviderPass1 = rcContainer->queryConsumer(streamId); \
            if(pProviderPass1 != NULL){ \
                sp<IImageStreamInfo> streamInfo = pProviderPass1->queryImageStreamInfo(); \
                MINT32 index = getImageStreamIndex( imageTable, streamId); \
                if(index == -1) { \
                    MY_LOGW("Stream do not exist in table!! %#" PRIxPTR " .", streamId); \
                } \
                else { \
                    imageTable[index].imgSize.w = streamInfo->getImgSize().w; \
                    imageTable[index].imgSize.h = streamInfo->getImgSize().h; \
                    imageTable[index].stride = (streamInfo->getBufPlanes()[0]).rowStrideInBytes; \
                    MY_LOGD("update streamId %#" PRIxPTR "for P2 prv's table %dx%d %d", \
                        streamId, \
                        imageTable[index].imgSize.w, \
                        imageTable[index].imgSize.h, \
                        imageTable[index].stride ); \
                } \
            } \
        } while(0);

        updateItem(eSTREAMID_IMAGE_PIPE_RAW_OPAQUE, mpResourceContainier);
        updateItem(eSTREAMID_IMAGE_PIPE_RAW_RESIZER, mpResourceContainier);
        updateItem(eSTREAMID_IMAGE_PIPE_RAW_RESIZER_01, mpResourceContainierMain2);
#undef updateItem
    }
    //
    sp<ImageStreamManager> pImageStreamManager =
            ImageStreamManager::create(imageTable);
    if(pImageStreamManager == NULL)
    {
        MY_LOGE("Create ImageStreamManager fail.");
        return NULL;
    }
    //
    const NodeConfigDataManager::NodeConfigMgrSetting cfgSetting =
                                        {
                                            mOpenId,
                                            mOpenId_main2,
                                            nodeConfigData
                                        };
    sp<NodeConfigDataManager> pNodeConfigDataManager =
            NodeConfigDataManager::create("Prv", &cfgSetting);
    if(pNodeConfigDataManager == NULL)
    {
        MY_LOGE("Create NodeConfigDataManager fail.");
        return NULL;
    }

    // Set provider
    // 1. set Full raw main1
    {
        sp<IImageStreamInfo> streamInfo = pImageStreamManager->getStreamInfoByStreamId(eSTREAMID_IMAGE_PIPE_RAW_OPAQUE);
        if(streamInfo != NULL)
        {
            // create user pool
            mpStereoBufferPool_OPAQUE = StereoBufferPool::createInstance(streamInfo->getStreamName());
            mpStereoBufferPool_OPAQUE->setSelector(mpStereoSelector_OPAQUE);

            // create provider
            sp<StreamBufferProviderFactory> pFactory = StreamBufferProviderFactory::createInstance();
            pFactory->setImageStreamInfo(streamInfo);
            pFactory->setUsersPool(mpStereoBufferPool_OPAQUE);

            android::sp<StreamBufferProvider> pImgoProvider = pFactory->create(getOpenId_P2Prv(), MTRUE);

            if(pImgoProvider == NULL)
            {
                MY_LOGE("create main1 imgo buffer pool failed");
                return NULL;
            }
            pImageStreamManager->updateBufProvider(
                                            eSTREAMID_IMAGE_PIPE_RAW_OPAQUE,
                                            pImgoProvider,
                                            Vector<StreamId_T>());
        }
    }
    // 2. set resized raw main1
    {
        sp<IImageStreamInfo> streamInfo = pImageStreamManager->getStreamInfoByStreamId(eSTREAMID_IMAGE_PIPE_RAW_RESIZER);
        if(streamInfo != NULL)
        {
            // create user pool
            mpStereoBufferPool_RESIZER= StereoBufferPool::createInstance(streamInfo->getStreamName());
            mpStereoBufferPool_RESIZER->setSelector(mpStereoSelector_RESIZER);

            // create provider
            sp<StreamBufferProviderFactory> pFactory = StreamBufferProviderFactory::createInstance();
            pFactory->setImageStreamInfo(streamInfo);
            pFactory->setUsersPool(mpStereoBufferPool_RESIZER);

            android::sp<StreamBufferProvider> pRrzoProvider = pFactory->create(getOpenId_P2Prv(), MTRUE);

            if(pRrzoProvider == NULL)
            {
                MY_LOGE("create main1 rrzo buffer pool failed");
                return NULL;
            }
            pImageStreamManager->updateBufProvider(
                                            eSTREAMID_IMAGE_PIPE_RAW_RESIZER,
                                            pRrzoProvider,
                                            Vector<StreamId_T>());
        }
    }
    // 3. set resized raw main2
    {
        sp<IImageStreamInfo> streamInfo = pImageStreamManager->getStreamInfoByStreamId(eSTREAMID_IMAGE_PIPE_RAW_RESIZER_01);
        if(streamInfo != NULL)
        {
            // create user pool
            mpStereoBufferPool_RESIZER_MAIN2 = StereoBufferPool::createInstance(streamInfo->getStreamName());
            mpStereoBufferPool_RESIZER_MAIN2->setSelector(mpStereoSelector_RESIZER_MAIN2);

            // create provider
            sp<StreamBufferProviderFactory> pFactory = StreamBufferProviderFactory::createInstance();
            pFactory->setImageStreamInfo(streamInfo);
            pFactory->setUsersPool(mpStereoBufferPool_RESIZER_MAIN2);

            android::sp<StreamBufferProvider> pRrzoProvider_main2 = pFactory->create(getOpenId_P2Prv(), MTRUE);

            if(pRrzoProvider_main2 == NULL)
            {
                MY_LOGE("create main2 rrzo buffer pool failed");
                return NULL;
            }
            pImageStreamManager->updateBufProvider(
                                            eSTREAMID_IMAGE_PIPE_RAW_RESIZER_01,
                                            pRrzoProvider_main2,
                                            Vector<StreamId_T>());
        }
        else {
            MY_LOGW("main1 resize-raw does not exit. should not be happened!!");
        }
    }
    // 4-1 set display CamClient
    {
        sp<IImageStreamInfo> pImageInfo = pImageStreamManager->getStreamInfoByStreamId(eSTREAMID_IMAGE_PIPE_BOKEHNODE_PREVIEWYUV);
        if(pImageInfo != NULL)
        {
            sp<StreamBufferProviderFactory> pFactory = StreamBufferProviderFactory::createInstance();

            sp<ClientBufferPool> pClient = new ClientBufferPool(getOpenId_P2Prv());
            {
                pClient->setCamClient(
                                pImageInfo->getStreamName(),
                                mpImgBufProvidersMgr,
                                IImgBufProvider::eID_DISPLAY);
            }
            pFactory->setImageStreamInfo(pImageInfo);
            pFactory->setUsersPool(pClient);

            android::sp<StreamBufferProvider> pProvider = pFactory->create(getOpenId_P2Prv(), MTRUE);

            pImageStreamManager->updateBufProvider(
                                            pImageInfo->getStreamId(),
                                            pProvider,
                                            Vector<StreamId_T>());
        }
        else {
            MY_LOGD("PreviewYUV did not config in ImageStreamMnger!!");
        }
    }
    //
    // 4-2 set record CamClient
    {
        sp<IImageStreamInfo> pImageInfo = pImageStreamManager->getStreamInfoByStreamId(eSTREAMID_IMAGE_PIPE_BOKEHNODE_RECORDYUV);
        if(pImageInfo != NULL)
        {
            MY_LOGD("Prepare BufferProvider for Record scenario.");
            sp<StreamBufferProviderFactory> pFactory = StreamBufferProviderFactory::createInstance();
            sp<ClientBufferPool> pClient = new ClientBufferPool(getOpenId_P2Prv());
            {
                pClient->setCamClient(
                                pImageInfo->getStreamName(),
                                mpImgBufProvidersMgr,
                                IImgBufProvider::eID_REC_CB);
            }
            pFactory->setImageStreamInfo(pImageInfo);
            pFactory->setUsersPool(pClient);

            android::sp<StreamBufferProvider> pProvider = pFactory->create(getOpenId_P2Prv(), MTRUE);

            pImageStreamManager->updateBufProvider(
                                            pImageInfo->getStreamId(),
                                            pProvider,
                                            Vector<StreamId_T>());
        }
        else {
            MY_LOGD("RecordYUV did not config in ImageStreamMnger!!");
        }
    }
    // 5. set FD
    {
        sp<IImageStreamInfo> pImageInfo = pImageStreamManager->getStreamInfoByStreamId(eSTREAMID_IMAGE_PIPE_DEPTHMAPNODE_FDYUV);
        if(pImageInfo != NULL)
        {
            // Vector<StreamId_T> vMetaStreamInfoSet;
            // vMetaStreamInfoSet.add(eSTREAMID_IMAGE_PIPE_DEPTHMAPNODE_FDYUV);
            // //
            // sp<StreamBufferProviderFactory> pFactory = StreamBufferProviderFactory::createInstance();
            // sp<IImageStreamInfo> pImageInfo = pImageStreamManager->getStreamInfoByStreamId(eSTREAMID_IMAGE_PIPE_DEPTHMAPNODE_FDYUV);
            // sp<ClientBufferPool> pClient = new ClientBufferPool();
            // {
            //     pClient->setCamClient(
            //                     pImageInfo->getStreamName(),
            //                     mpImgBufProvidersMgr,
            //                     IImgBufProvider::eID_FD);
            // }
            // pFactory->setName(pImageInfo->getStreamName());
            // pFactory->setImageStreamInfo(pImageInfo);
            // pFactory->setUsersPool(pClient);
            // pImageStreamManager->updateBufProvider(
            //                                 pImageInfo->getStreamId(),
            //                                 pFactory->create(getOpenId()),
            //                                 NULL,
            //                                 vMetaStreamInfoSet);

            // workaround use HAL_POOL for FD buffer
            sp<StreamBufferProviderFactory> pFactory = StreamBufferProviderFactory::createInstance();

            pFactory->setImageStreamInfo(pImageInfo);
            pImageStreamManager->updateBufProvider(
                                            eSTREAMID_IMAGE_PIPE_DEPTHMAPNODE_FDYUV,
                                            pFactory->create(),
                                            Vector<StreamId_T>());
        }
    }
    //
    pBuilder->setMetaStreamId(
                eSTREAMID_META_HAL_DYNAMIC_P1,
                eSTREAMID_META_APP_CONTROL);

    FUNC_END;
    return pBuilder->create(pMetaStreamManager, pImageStreamManager, pNodeConfigDataManager);
}

/******************************************************************************
 * create p2 cap pipeline
 ******************************************************************************/
sp<ILegacyPipeline>
StereoFlowControl::
constructP2Pipeline_Cap(
    const MetaStreamManager::metadata_info_setting* metaTable,
    const MINT32* nodeConfigData,
    ImageStreamManager::image_stream_info_pool_setting* imageTable
)
{
    FUNC_START;
    // create legacy pipeline builder
    sp<StereoLegacyPipelineBuilder> pBuilder =
            new StereoLegacyPipelineBuilder(
                                        getOpenId(),
                                        "P2_Pipeline",
                                        StereoLegacyPipelineMode_T::STPipelineMode_P2);
    if ( pBuilder == 0 ) {
        MY_LOGE("Cannot create LegacyPipelineBuilder.");
        return MFALSE;
    }
    sp<MetaStreamManager> pMetaStreamManager =
            MetaStreamManager::create(metaTable);
    if(pMetaStreamManager == NULL)
    {
        MY_LOGE("Create MetaStreamManager fail.");
        return NULL;
    }
    //
    sp<ImageStreamManager> pImageStreamManager =
            ImageStreamManager::create(imageTable);
    if(pImageStreamManager == NULL)
    {
        MY_LOGE("Create ImageStreamManager fail.");
        return NULL;
    }
    //
    const NodeConfigDataManager::NodeConfigMgrSetting cfgSetting =
                                        {
                                            mOpenId,
                                            mOpenId_main2,
                                            nodeConfigData
                                        };
    sp<NodeConfigDataManager> pNodeConfigDataManager =
            NodeConfigDataManager::create("Prv", &cfgSetting);
    if(pNodeConfigDataManager == NULL)
    {
        MY_LOGE("Create NodeConfigDataManager fail.");
        return NULL;
    }
    // Set provider
    // 1. set Full raw main1
    {
        android::sp<StreamBufferProvider> pImgoProvider =
                IResourceContainer::getInstance(getOpenId())->queryConsumer(
                                        eSTREAMID_IMAGE_PIPE_RAW_OPAQUE);
        if(pImgoProvider == NULL)
        {
            MY_LOGE("query main1 imgo consumer fail");
            return NULL;
        }
        pImageStreamManager->updateBufProvider(
                                        eSTREAMID_IMAGE_PIPE_RAW_OPAQUE,
                                        pImgoProvider,
                                        Vector<StreamId_T>());
    }
    // 2. set resized raw main1
    {
        android::sp<StreamBufferProvider> pRrzoProvider =
                IResourceContainer::getInstance(getOpenId())->queryConsumer(
                                        eSTREAMID_IMAGE_PIPE_RAW_RESIZER);
        if(pRrzoProvider == NULL)
        {
            MY_LOGE("query main1 rrzo consumer fail");
            return NULL;
        }
        pImageStreamManager->updateBufProvider(
                                        eSTREAMID_IMAGE_PIPE_RAW_RESIZER,
                                        pRrzoProvider,
                                        Vector<StreamId_T>());
    }
    // 3. set resized raw main2
    {
        android::sp<StreamBufferProvider> pRrzoProvider =
                IResourceContainer::getInstance(getOpenId_Main2())->queryConsumer(
                                        eSTREAMID_IMAGE_PIPE_RAW_RESIZER_01);
        if(pRrzoProvider == NULL)
        {
            MY_LOGE("query main2 rrzp consumer fail");
            return NULL;
        }
        pImageStreamManager->updateBufProvider(
                                        eSTREAMID_IMAGE_PIPE_RAW_RESIZER_01,
                                        pRrzoProvider,
                                        Vector<StreamId_T>());
    }
    // Todo: add provider
    //
    pBuilder->setMetaStreamId(
                eSTREAMID_META_HAL_DYNAMIC_P1,
                eSTREAMID_META_APP_CONTROL);
    FUNC_END;
    return pBuilder->create(pMetaStreamManager, pImageStreamManager, pNodeConfigDataManager);
}
