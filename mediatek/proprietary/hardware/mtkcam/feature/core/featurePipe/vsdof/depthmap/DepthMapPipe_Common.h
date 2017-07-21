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

#ifndef _MTK_CAMERA_INCLUDE_STEREO_DEPTHMAP_FEATUREPIPE_COMMON_H_
#define _MTK_CAMERA_INCLUDE_STEREO_DEPTHMAP_FEATUREPIPE_COMMON_H_



#include <iopipe/Port.h>
#include <utils/RefBase.h>
#include <utils/KeyedVector.h>
#include <metadata/IMetadata.h>
#include <mmsdk/EffectRequest.h>
#include <mtkcam/common/include/common.h>
#include <vsdof/hal/stereo_size_provider.h>
#include <featurePipe/vsdof/IDepthMapPipe.h>
#include <featurePipe/core/include/ImageBufferPool.h>
#include <featurePipe/core/include/GraphicBufferPool.h>


#define PIPE_MODULE_TAG "DepthMapPipe"
#define PIPE_CLASS_TAG "Common"
#include <featurePipe/core/include/PipeLog.h>

using android::String8;
using android::KeyedVector;
using namespace android;

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {

class ImageBufInfoMap;
struct ExtraDataInfo;

const int VSDOF_CONST_FE_EXEC_TIMES = 2;
const int VSDOF_BURST_FRAME_SIZE = 10;
const int VSDOF_WORKING_BUF_SET = 2;

typedef android::sp<EffectRequest> EffectRequestPtr;
typedef android::sp<EffectFrameInfo> FrameInfoPtr;
typedef android::sp<ImageBufInfoMap> ImgInfoMapPtr;
typedef android::sp<ExtraDataInfo> ExtraDataInfoPtr;

// ==========================  logging  section ============================= //
#define MY_LOGV_IF(cond, ...)       do { if ( (cond) ) { MY_LOGV(__VA_ARGS__); } }while(0)
#define MY_LOGD_IF(cond, ...)       do { if ( (cond) ) { MY_LOGD(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) ) { MY_LOGI(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) ) { MY_LOGW(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) ) { MY_LOGE(__VA_ARGS__); } }while(0)
#define MY_LOGA_IF(cond, ...)       do { if ( (cond) ) { MY_LOGA(__VA_ARGS__); } }while(0)
#define MY_LOGF_IF(cond, ...)       do { if ( (cond) ) { MY_LOGF(__VA_ARGS__); } }while(0)

#define VSDOF_PRFLOG(fmt, arg...)  do { if(this->mbProfileLog) {MY_LOGD("[VSDOF_Profile] %d: " fmt, __LINE__, ##arg);} } while(0)
#define VSDOF_LOGD(fmt, arg...)     do { if(this->mbDebugLog) {MY_LOGD("%d: " fmt, __LINE__, ##arg);} } while(0)

// ==========================  logging  section ============================= //

typedef KeyedVector<DepthMapBufferID, SmartImageBuffer> SmartImageBufferSet;
typedef KeyedVector<DepthMapBufferID, SmartGraphicBuffer> GraphicImageBufferSet;

class ImageBufInfoMap: public LightRefBase<ImageBufInfoMap>
{
public:
    ImageBufInfoMap(sp<EffectRequest> ptr);
    virtual ~ImageBufInfoMap() {clear();}
    // ImageBuffer
    ssize_t addImageBuffer(const DepthMapBufferID& key, const SmartImageBuffer& value);
    ssize_t delImageBuffer(const DepthMapBufferID& key);
    const SmartImageBuffer& getImageBuffer(const DepthMapBufferID& key);
    // GraphicBuffer
    ssize_t addGraphicBuffer(const DepthMapBufferID& key, const SmartGraphicBuffer& value);
    ssize_t delGraphicBuffer(const DepthMapBufferID& key);
    const SmartGraphicBuffer& getGraphicBuffer(const DepthMapBufferID& key);

    MVOID clear() {mvImgBufData.clear(); mvGraImgBufData.clear();}
    MVOID setRequestPtr(sp<EffectRequest>& ptr) { mpReqPtr = ptr;}
    sp<EffectRequest> getRequestPtr() { return mpReqPtr;}

    const SmartImageBufferSet& getImageBufferSet() { return mvImgBufData; }
    const GraphicImageBufferSet& getGraphicBufferSet() { return mvGraImgBufData; }
private:
    SmartImageBufferSet mvImgBufData;
    GraphicImageBufferSet mvGraImgBufData;
    sp<EffectRequest> mpReqPtr;
};

struct ExtraDataInfo: public LightRefBase<ImageBufInfoMap>
{
public:
    ExtraDataInfo(sp<EffectRequest> pReq, char* sExtraDataJson)
    : mpReqPtr(pReq), msExtraData(sExtraDataJson) {}

    sp<EffectRequest> getRequestPtr() { return mpReqPtr;}
public:
    sp<EffectRequest> mpReqPtr;
    String8 msExtraData;

};



DepthNodeOpState getRequestState(EffectRequestPtr &request);

//  ============================  Port definition  ============================
// input
static const NSCam::NSIoPipe::PortID PORT_DEPI( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_DEPI, 0);
static const NSCam::NSIoPipe::PortID PORT_DMGI( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_DMGI , 0);
static const NSCam::NSIoPipe::PortID PORT_IMGI( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_IMGI, 0);
// output
static const NSCam::NSIoPipe::PortID PORT_IMG2O( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_IMG2O, 1);
static const NSCam::NSIoPipe::PortID PORT_IMG3O( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_IMG3O, 1);
static const NSCam::NSIoPipe::PortID PORT_WDMA( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_WDMAO, 1);
static const NSCam::NSIoPipe::PortID PORT_WROT( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_WROTO, 1);
static const NSCam::NSIoPipe::PortID PORT_FEO( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_FEO, 1);
static const NSCam::NSIoPipe::PortID PORT_MFBO( NSIoPipe::EPortType_Memory, NSImageio::NSIspio::EPortIndex_MFBO, 1);

//  ============================  EIS utility  ============================
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
MBOOL queryEisRegion(IMetadata* const inHal, eis_region& region, android::sp<EffectRequest> request);
MBOOL isEISOn(IMetadata* const inApp);

//  ============================  utility macro definition  ============================
#define RETRIEVE_OFRAMEINFO(request, rFrameInfo, BufID) \
    if (request->vOutputFrameInfo.indexOfKey(BufID) >= 0) \
    {\
        rFrameInfo = request->vOutputFrameInfo.valueFor(BufID);\
    }

#define RETRIEVE_OFRAMEINFO_WITH_WARNING(request, rFrameInfo, BufID) \
    if (request->vOutputFrameInfo.indexOfKey(BufID) >= 0) \
    {\
        rFrameInfo = request->vOutputFrameInfo.valueFor(BufID);\
    }\
    else\
    {\
        MY_LOGW("Cannot find the frameBuffer in the effect request, frameID=%d", BufID);\
    }

#define RETRIEVE_OFRAMEINFO_IMGBUF(request, rFrameInfo, BufID, rImgBuf) \
    if (request->vOutputFrameInfo.indexOfKey(BufID) >= 0) \
    { \
        rFrameInfo = request->vOutputFrameInfo.valueFor(BufID);\
        rFrameInfo->getFrameBuffer(rImgBuf); \
    }

#define RETRIEVE_OFRAMEINFO_IMGBUF_WITH_WARNING(request, rFrameInfo, BufID, rImgBuf) \
    if (request->vOutputFrameInfo.indexOfKey(BufID) >= 0) \
    { \
        rFrameInfo = request->vOutputFrameInfo.valueFor(BufID);\
        rFrameInfo->getFrameBuffer(rImgBuf); \
    }\
    else\
    {\
        MY_LOGW("Cannot find the frameBuffer in the effect request, frameID=%d", BufID); \
    }

#define RETRIEVE_OFRAMEINFO_IMGBUF_ERROR_RETURN(request, rFrameInfo, BufID, rImgBuf) \
    if (request->vOutputFrameInfo.indexOfKey(BufID) >= 0) \
    { \
        rFrameInfo = request->vOutputFrameInfo.valueFor(BufID);\
        rFrameInfo->getFrameBuffer(rImgBuf); \
    }\
    else\
    {\
        MY_LOGE("Cannot find the frameBuffer in the effect request, frameID=%d! Return!", BufID); \
        return MFALSE; \
    }

#define CREATE_IMGBUF_POOL(rBufPool, name, size, format, usage, bSingle) \
    MY_LOGD("Create ImageBufferPool:: "#name " size=%dx%d format=%x usage=%x single=%d", \
                size.w, size.h, format, usage, bSingle); \
    rBufPool = ImageBufferPool::create(name, size.w, size.h, format, usage, bSingle);

#define ALLOCATE_BUFFER_POOL(bufPool, size) \
    if(!bufPool->allocate(size))\
    { \
        MY_LOGE("Allocate "#bufPool" image buffer failed"); \
        uninit(); \
        return MFALSE; \
    }

/**
 * Get the metadata inside the FrameInfo
 *
 */
IMetadata* getMetadataFromFrameInfoPtr(sp<EffectFrameInfo> pFrameInfo);


}; // NSFeaturePipe
}; // NSCamFeature
}; // NSCam

#endif // _MTK_CAMERA_INCLUDE_STEREO_DEPTHMAP_FEATUREPIPE_COMMON_H_
