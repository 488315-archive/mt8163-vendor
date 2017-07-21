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

#include "StreamingFeaturePipe.h"

#define PIPE_CLASS_TAG "Pipe"
#define PIPE_TRACE TRACE_STREAMING_FEATURE_PIPE
#include <featurePipe/core/include/PipeLog.h>

#define NORMAL_STREAM_NAME "StreamingFeature"

using namespace NSCam::NSIoPipe::NSPostProc;

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {

StreamingFeaturePipe::StreamingFeaturePipe(MUINT32 sensorIndex)
    : CamPipe<StreamingFeatureNode>("StreamingFeaturePipe")
    , mForceOnMask(0)
    , mForceOffMask(~0)
    , mSensorIndex(sensorIndex)
    , mCounter(0)
    , mP2A("fpipe.p2a")
    , mGPU("fpipe.gpu")
    , mMDP("fpipe.mdp")
#if SUPPORT_VFB
    , mVFB("fpipe.vfb")
    , mFD("fpipe.fd")
    , mP2B("fpipe.p2b")
#endif // SUUPPORT_VFB
    , mEIS("fpipe.eis")
    , mHelper("fpipe.helper")
    , mNormalStream(NULL)
    , mDebugDump(0)
    , mDebugDumpCount(1)
    , mForceIMG3O(MFALSE)
    , mUsePerFrameSetting(MFALSE)
{
    TRACE_FUNC_ENTER();
    mNodes.push_back(&mP2A);
    mNodes.push_back(&mGPU);
    mNodes.push_back(&mMDP);
#if SUPPORT_VFB
    mNodes.push_back(&mVFB);
    mNodes.push_back(&mFD);
    mNodes.push_back(&mP2B);
#endif // SUPPORT_VFB
    mNodes.push_back(&mEIS);
    mNodes.push_back(&mHelper);
    TRACE_FUNC_EXIT();
}

StreamingFeaturePipe::~StreamingFeaturePipe()
{
    TRACE_FUNC_ENTER();
    // must call dispose to free CamGraph
    this->dispose();
    TRACE_FUNC_EXIT();
}

void StreamingFeaturePipe::setSensorIndex(MUINT32 sensorIndex)
{
    TRACE_FUNC_ENTER();
    this->mSensorIndex = sensorIndex;
    TRACE_FUNC_EXIT();
}

MBOOL StreamingFeaturePipe::init(const char *name)
{
    TRACE_FUNC_ENTER();
    (void)name;
    MBOOL ret;
    ret = PARENT_PIPE::init();
    mCounter = 0;
    TRACE_FUNC_EXIT();
    return ret;
}

MBOOL StreamingFeaturePipe::uninit(const char *name)
{
    TRACE_FUNC_ENTER();
    (void)name;
    MBOOL ret;
    ret = PARENT_PIPE::uninit();
    TRACE_FUNC_EXIT();
    return ret;
}

MBOOL StreamingFeaturePipe::enque(const FeaturePipeParam &param)
{
    TRACE_FUNC_ENTER();
    MBOOL ret = MFALSE;
    RequestPtr request;
    request = new StreamingFeatureRequest(param, ++mCounter);
    if(request == NULL)
    {
        MY_LOGE("OOM: Cannot allocate StreamingFeatureRequest");
    }
    else
    {
        if( mUsePerFrameSetting )
        {
            this->prepareDebugSetting();
        }
        this->applyMaskOverride(request);
        this->applyVarMapOverride(request);
        ret = CamPipe::enque(ID_ROOT_ENQUE, request);
    }
    TRACE_FUNC_EXIT();
    return ret;
}

MBOOL StreamingFeaturePipe::onInit()
{
    TRACE_FUNC_ENTER();
    MBOOL ret;
    ret = this->prepareDebugSetting() &&
          this->prepareGeneralPipe() &&
          this->prepareNodeSetting() &&
          this->prepareNodeConnection() &&
          this->prepareBuffer();

    TRACE_FUNC_EXIT();
    return ret;
}

MVOID StreamingFeaturePipe::onUninit()
{
    TRACE_FUNC_ENTER();
    this->releaseBuffer();
    this->releaseNodeSetting();
    this->releaseGeneralPipe();
    TRACE_FUNC_EXIT();
}

MBOOL StreamingFeaturePipe::onData(DataID, const RequestPtr &)
{
    TRACE_FUNC_ENTER();
    MBOOL ret = MFALSE;
    TRACE_FUNC_EXIT();
    return ret;
}

MBOOL StreamingFeaturePipe::prepareDebugSetting()
{
    TRACE_FUNC_ENTER();

    mForceOnMask = 0;
    mForceOffMask = ~0;

    #define CHECK_DEBUG_MASK(name)                                          \
    {                                                                       \
        MINT32 prop = getPropertyValue(KEY_FORCE_##name, VAL_FORCE_##name); \
        if( prop == FORCE_ON )    ENABLE_##name(mForceOnMask);              \
        if( prop == FORCE_OFF )   DISABLE_##name(mForceOffMask);            \
    }
    CHECK_DEBUG_MASK(EIS);
    CHECK_DEBUG_MASK(EIS_FU);
    CHECK_DEBUG_MASK(VFB);
    CHECK_DEBUG_MASK(VFB_EX);
    CHECK_DEBUG_MASK(3DNR);
    CHECK_DEBUG_MASK(VHDR);
    #undef CHECK_DEBUG_SETTING

    #if !SUPPORT_VFB
    DISABLE_VFB(mForceOffMask);
    DISABLE_VFB_EX(mForceOffMask);
    #endif

    mDebugDump = getPropertyValue(KEY_DEBUG_DUMP, VAL_DEBUG_DUMP);
    mDebugDumpCount = getPropertyValue(KEY_DEBUG_DUMP_COUNT, VAL_DEBUG_DUMP_COUNT);
    mForceIMG3O = getPropertyValue(KEY_FORCE_IMG3O, VAL_FORCE_IMG3O);
    mUsePerFrameSetting = getPropertyValue(KEY_USE_PER_FRAME_SETTING, VAL_USE_PER_FRAME_SETTING);

    TRACE_FUNC_EXIT();
    return MTRUE;
}

MBOOL StreamingFeaturePipe::prepareGeneralPipe()
{
    TRACE_FUNC_ENTER();
    mNormalStream = INormalStream::createInstance(mSensorIndex);
    if( mNormalStream != NULL )
    {
        mNormalStream->init(NORMAL_STREAM_NAME);
    }
    mP2A.setNormalStream(mNormalStream);
    TRACE_FUNC_EXIT();
    return (mNormalStream != NULL);
}

MBOOL StreamingFeaturePipe::prepareNodeSetting()
{
    TRACE_FUNC_ENTER();
    NODE_LIST::iterator it, end;
    for( it = mNodes.begin(), end = mNodes.end(); it != end; ++it )
    {
        (*it)->setSensorIndex(mSensorIndex);
    }

    TRACE_FUNC_EXIT();
    return MTRUE;
}

MBOOL StreamingFeaturePipe::prepareNodeConnection()
{
    TRACE_FUNC_ENTER();

    this->connectData(ID_P2A_TO_HELPER, mP2A, mHelper);

    this->connectData(ID_P2A_TO_GPU_FULLIMG, mP2A, mGPU);
    this->connectData(ID_GPU_TO_MDP_FULLIMG, mGPU, mMDP);

#if SUPPORT_VFB
    // VFB nodes
    this->connectData(ID_P2A_TO_FD_DSIMG, mP2A, mFD);
    this->connectData(ID_P2A_TO_VFB_DSIMG, mP2A, mVFB);
    this->connectData(ID_P2A_TO_P2B_FULLIMG, mP2A, mP2B);
    this->connectData(ID_FD_TO_VFB_FACE, mFD, mVFB);
    this->connectData(ID_VFB_TO_P2B, mVFB, mP2B);
    this->connectData(ID_VFB_TO_GPU_WARP, mVFB, mGPU);
    this->connectData(ID_MDP_TO_P2B_FULLIMG, mMDP, mP2B);
#endif // SUPPORT_VFB

    // EIS nodes
    this->connectData(ID_P2A_TO_EIS_CONFIG, mP2A, mEIS);
    this->connectData(ID_P2A_TO_EIS_P2DONE, mP2A, mEIS);
    this->connectData(ID_EIS_TO_GPU_WARP, mEIS, mGPU);

#if SUPPORT_VFB
    // EIS + VFB
    this->connectData(ID_EIS_TO_VFB_WARP, mEIS, mVFB);
#endif // SUPPORT_VFB

    this->setRootNode(&mP2A);
    mP2A.registerInputDataID(ID_ROOT_ENQUE);

    TRACE_FUNC_EXIT();
    return MTRUE;
}

MBOOL StreamingFeaturePipe::prepareBuffer()
{
    TRACE_FUNC_ENTER();
    mDsImgPool = ImageBufferPool::create("SFPipe.dsImg", DS_IMAGE_WIDTH, DS_IMAGE_HEIGHT, eImgFmt_YUY2, ImageBufferPool::USAGE_HW );
    mFullImgPool = GraphicBufferPool::create("SFPipe.fullImg", MAX_FULL_WIDTH, MAX_FULL_HEIGHT, HAL_PIXEL_FORMAT_YV12, GraphicBufferPool::USAGE_HW_TEXTURE);
    mGpuOutputPool = GraphicBufferPool::create("SFPipe.gpuOut", MAX_FULL_WIDTH, MAX_FULL_HEIGHT, android::PIXEL_FORMAT_RGBA_8888, GraphicBufferPool::USAGE_HW_RENDER);

    mDsImgPool->allocate(NUM_DEFAULT_BUFFER);
    mFullImgPool->allocate(NUM_DEFAULT_BUFFER);
    mGpuOutputPool->allocate(NUM_DEFAULT_BUFFER);

    mP2A.setDsImgPool(mDsImgPool);
    mP2A.setFullImgPool(mFullImgPool);
    mGPU.setInputBufferPool(mFullImgPool);
    mGPU.setOutputBufferPool(mGpuOutputPool);

    TRACE_FUNC_EXIT();
    return MTRUE;
}

MVOID StreamingFeaturePipe::releaseNodeSetting()
{
    TRACE_FUNC_ENTER();
    this->disconnect();
    TRACE_FUNC_EXIT();
}

MVOID StreamingFeaturePipe::releaseGeneralPipe()
{
    TRACE_FUNC_ENTER();
    mP2A.setNormalStream(NULL);
    if( mNormalStream )
    {
        mNormalStream->uninit(NORMAL_STREAM_NAME);
        mNormalStream->destroyInstance();
    }
    TRACE_FUNC_EXIT();
}

MVOID StreamingFeaturePipe::releaseBuffer()
{
    TRACE_FUNC_ENTER();

    mP2A.setDsImgPool(NULL);
    mP2A.setFullImgPool(NULL);
    mGPU.setInputBufferPool(NULL);
    mGPU.setOutputBufferPool(NULL);

    ImageBufferPool::destroy(mDsImgPool);
    GraphicBufferPool::destroy(mFullImgPool);
    GraphicBufferPool::destroy(mGpuOutputPool);

    TRACE_FUNC_EXIT();
}

MVOID StreamingFeaturePipe::applyMaskOverride(const RequestPtr &request)
{
    TRACE_FUNC_ENTER();
    request->mFeatureMask |= mForceOnMask;
    request->mFeatureMask &= mForceOffMask;
    request->setDumpProp(mDebugDump, mDebugDumpCount);
    request->setForceIMG3O(mForceIMG3O);
    TRACE_FUNC_EXIT();
}

MVOID StreamingFeaturePipe::applyVarMapOverride(const RequestPtr &request)
{
    TRACE_FUNC_ENTER();
    TRACE_FUNC_EXIT();
}

} // NSFeaturePipe
} // NSCamFeature
} // NSCam
