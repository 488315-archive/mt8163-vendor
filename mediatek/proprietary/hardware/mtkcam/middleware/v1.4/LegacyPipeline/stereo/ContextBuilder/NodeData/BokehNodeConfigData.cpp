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
#define LOG_TAG "ContextBuilder/BokehNodeData"
//
#include <utils/StrongPointer.h>
//
#include <LegacyPipeline/stereo/ContextBuilder/MetaStreamManager.h>
#include <LegacyPipeline/stereo/ContextBuilder/ImageStreamManager.h>
#include <LegacyPipeline/stereo/ContextBuilder/StereoBasicParameters.h>
//
#include <v3/pipeline/PipelineContext.h>
//
#include <v3/hwnode/BokehNode.h>
//
#include <LegacyPipeline/StreamId.h>
//
#include "BokehNodeConfigData.h"
/******************************************************************************
 *
 ******************************************************************************/
#define MY_LOGV(fmt, arg...)        CAM_LOGV("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_LOGD("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_LOGI("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_LOGW("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_LOGE("(%d)[%s] " fmt, ::gettid(), __FUNCTION__, ##arg)
//
#define MY_LOGV_IF(cond, ...)       do { if ( (cond) ) { MY_LOGV(__VA_ARGS__); } }while(0)
#define MY_LOGD_IF(cond, ...)       do { if ( (cond) ) { MY_LOGD(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) ) { MY_LOGI(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) ) { MY_LOGW(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) ) { MY_LOGE(__VA_ARGS__); } }while(0)
//
using namespace android;
using namespace NSCam;
using namespace v3;
using namespace NSCam::v3::NSPipelineContext;
//
typedef BokehNode                       NodeT;
typedef NodeActor< NodeT >              MyNodeActorT;
//
BokehNodeConfigData::
BokehNodeConfigData(
    MINT32 openId,
    NodeId_T nodeId,
    char const* nodeName)
    : INodeConfigBase(openId, nodeId, nodeName)
{
    initParam.openId = mOpenId;
    initParam.nodeId = mNodeId;
    initParam.nodeName = mNodeName;
}
//
BokehNodeConfigData::
~BokehNodeConfigData()
{
    MY_LOGD("dcot(%x)", this);
}
//
sp<INodeActor>
BokehNodeConfigData::
getNode()
{
    MY_LOGD("+");
    sp<MyNodeActorT> pNode = new MyNodeActorT( NodeT::createInstance() );
    //
    pNode->setInitParam(initParam);
    pNode->setConfigParam(cfgParam);
    //
    MY_LOGD("-");
    return pNode;
}
//
void
BokehNodeConfigData::
configNode(
    MetaStreamManager* metaManager,
    ImageStreamManager* imageManager,
    StereoBasicParameters* userParams,
    PipelineContext* pipeContext)
{
    MY_LOGD("CKH: +");
    //
    // Get meta stream info
    //
    if(metaManager != NULL)
    {
        MY_LOGD("CKH: metaManager+");
        addStreamBegin(MTRUE);
        cfgParam.pInAppMeta = metaManager->getStreamInfoByStreamId(eSTREAMID_META_APP_CONTROL);
        addStream(cfgParam.pInAppMeta, MTRUE);
        //
        cfgParam.pInHalMeta = metaManager->getStreamInfoByStreamId(eSTREAMID_META_HAL_DYNAMIC_P1);
        addStream(cfgParam.pInHalMeta, MTRUE);
        //
        cfgParam.pOutAppMeta = metaManager->getStreamInfoByStreamId(eSTREAMID_META_APP_DYNAMIC_BOKEH);
        addStream(cfgParam.pOutAppMeta, MFALSE);
        //
        cfgParam.pOutHalMeta = metaManager->getStreamInfoByStreamId(eSTREAMID_META_HAL_DYNAMIC_BOKEH);
        addStream(cfgParam.pOutHalMeta, MFALSE);
        addStreamEnd(MTRUE);
        MY_LOGD("CKH: metaManager+");
    }
    //
    // Get image stream info
    //
    if(imageManager != NULL)
    {
        MY_LOGD("CKH: imageManager+");
        addStreamBegin(MFALSE);
        cfgParam.pInHalImageMainImage = imageManager->getStreamInfoByStreamId(eSTREAMID_IMAGE_PIPE_DEPTHMAPNODE_MAINIMAGEYUV);
        addStream(  cfgParam.pInHalImageMainImage, MTRUE);
        //
        cfgParam.pInHalImageMainImage_Capture = imageManager->getStreamInfoByStreamId(eSTREAMID_IMAGE_PIPE_DEPTHMAPNODE_MAINIMAGE_CAPYUV);
        addStream(  cfgParam.pInHalImageMainImage_Capture, MTRUE);
        //
        cfgParam.pInHalImageMYS = imageManager->getStreamInfoByStreamId(eSTREAMID_IMAGE_PIPE_DEPTHMAPNODE_MY_SYUV);
        addStream(  cfgParam.pInHalImageMYS, MTRUE);
        //
        cfgParam.pInHalImageDMW = imageManager->getStreamInfoByStreamId(eSTREAMID_IMAGE_PIPE_DEPTHMAPNODE_DMWYUV);
        addStream(  cfgParam.pInHalImageDMW, MTRUE);
        //
        cfgParam.pOutHalImageCleanImage = imageManager->getStreamInfoByStreamId(eSTREAMID_IMAGE_PIPE_BOKEHNODE_CLEANIMAGEYUV);
        addStream(  cfgParam.pOutHalImageCleanImage, MFALSE);
        //
        cfgParam.pOutAppImageRecord = imageManager->getStreamInfoByStreamId(eSTREAMID_IMAGE_PIPE_BOKEHNODE_RECORDYUV);
        addStream(  cfgParam.pOutAppImageRecord, MFALSE);
        //
        cfgParam.pOutAppImagePreview = imageManager->getStreamInfoByStreamId(eSTREAMID_IMAGE_PIPE_BOKEHNODE_PREVIEWYUV);
        addStream(  cfgParam.pOutAppImagePreview, MFALSE);
        //
        cfgParam.pOutHalImageBokehResult = imageManager->getStreamInfoByStreamId(eSTREAMID_IMAGE_PIPE_BOKEHNODE_RESULTYUV);
        addStream(  cfgParam.pOutHalImageBokehResult, MFALSE);
        addStreamEnd(MFALSE);
        MY_LOGD("CKH: imageManager-");
    }
    MY_LOGD("CKH: -");
}
//
void
BokehNodeConfigData::
dump()
{
    MY_LOGD("CKH: Dump bokeh node config data");
    MY_LOGD("CKH: mvImageIOSet.size(%d)", mvImageIOSet.size());
    for(MUINT32 i=0;i<mvImageIOSet.size();++i)
    {
        MY_LOGD("CKH: mvImageIOSet[%d].vIn.size(%d)", i, mvImageIOSet[i].vIn.size());
        for(MUINT32 j=0;j<mvImageIOSet[i].vIn.size();++j)
        {
            MY_LOGD("InImageIOMap StreamId(%d)", mvImageIOSet[i].vIn[j]);
        }
        MY_LOGD("CKH: mvImageIOSet[%d].vOut.size(%d)", i, mvImageIOSet[i].vOut.size());
        for(MUINT32 j=0;j<mvImageIOSet[i].vOut.size();++j)
        {
            MY_LOGD("OutImageIOMap StreamId(%d)", mvImageIOSet[i].vOut[j]);
        }
    }
    MY_LOGD("CKH: mvMetaIOSet");
    MY_LOGD("CKH: mvMetaIOSet.size(%d)", mvMetaIOSet.size());
    for(MUINT32 i=0;i<mvMetaIOSet.size();++i)
    {
        MY_LOGD("CKH: mvMetaIOSet[%d].vIn.size(%d)", i, mvMetaIOSet[i].vIn.size());
        for(MUINT32 j=0;j<mvMetaIOSet[i].vIn.size();++j)
        {
            MY_LOGD("InMetaIOMap StreamId(%d)", mvMetaIOSet[i].vIn[j]);
        }
        MY_LOGD("CKH: mvMetaIOSet[%d].vOut.size(%d)", i, mvMetaIOSet[i].vOut.size());
        for(MUINT32 j=0;j<mvMetaIOSet[i].vOut.size();++j)
        {
            MY_LOGD("OutMetaIOMap StreamId(%d)", mvMetaIOSet[i].vOut[j]);
        }
    }
}
//
void
BokehNodeConfigData::
destroy()
{
    cfgParam.pInAppMeta = NULL;
    cfgParam.pInHalMeta = NULL;
    cfgParam.pOutAppMeta = NULL;
    cfgParam.pOutHalMeta = NULL;
    //
    cfgParam.pInHalImageMainImage = NULL;
    cfgParam.pInHalImageMainImage_Capture = NULL;
    cfgParam.pInHalImageMYS = NULL;
    cfgParam.pInHalImageDMW = NULL;
    //cfgParam.pOutAppImagePostView = NULL;
    cfgParam.pOutHalImageCleanImage = NULL;
    cfgParam.pOutAppImageRecord = NULL;
    cfgParam.pOutAppImagePreview = NULL;
    cfgParam.pOutHalImageBokehResult = NULL;
}