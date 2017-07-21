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

#define LOG_TAG "MtkCam/P1NodeImp"
//
#include <Log.h>
#include "BaseNode.h"
#include "hwnode_utilities.h"
#include <v3/hwnode/P1Node.h>
#include <v3/stream/IStreamInfo.h>
#include <v3/stream/IStreamBuffer.h>
#include <v3/utils/streambuf/IStreamBufferPool.h>
//
#include <utils/RWLock.h>
#include <utils/Thread.h>
//
#include <sys/prctl.h>
#include <sys/resource.h>
#include <system/thread_defs.h>

//
#include <IHal3A.h>
#include <iopipe/CamIO/IHalCamIO.h>
#include <iopipe/CamIO/INormalPipe.h>
#include <vector>
#include <iopipe/CamIO/PortMap.h>/*#include <iopipe_FrmB/CamIO/PortMap_FrmB.h>*/
#include <common/eis/eis_hal.h>
#include <common/vhdr/1.0/vhdr_hal.h>

//
#include <mtk_platform_metadata_tag.h>
#include <metadata/client/mtk_metadata_tag.h>
//
#include "Profile.h"
#include <Trace.h>
#include <cutils/properties.h>
#include <utils/Atomic.h>
//
#include <DebugScanLine.h>
//
#include <imageio/ispio_utility.h> //(SUPPORT_SCALING_CROP)
#include <metadata/IMetadataProvider.h>
//
#include <imagebuf/IIonImageBufferHeap.h>
#include <imagebuf/IDummyImageBufferHeap.h>
//
#if (HWNODE_HAVE_AEE_FEATURE)
#include <aee.h>
#ifdef AEE_ASSERT
#undef AEE_ASSERT
#endif
#define AEE_ASSERT(String) \
    do { \
        CAM_LOGE("ASSERT("#String") fail"); \
        aee_system_exception( \
            LOG_TAG, \
            NULL, \
            DB_OPT_DEFAULT, \
            String); \
    } while(0)
#else
#define AEE_ASSERT(String)
#endif
//
using namespace std;
using namespace android;
using namespace NSCam;
using namespace NSCam::v3;
//using namespace NSCam::Utils;
using namespace NSCam::Utils::Sync;
using namespace NSCam::NSIoPipe;
using namespace NSCam::NSIoPipe::NSCamIOPipe;
using namespace NS3Av3;
using namespace NSImageio;
using namespace NSIspio;

/******************************************************************************
 *
 ******************************************************************************/
#define MY_LOGV(fmt, arg...)                  CAM_LOGV("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)                  CAM_LOGD("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)                  CAM_LOGI("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)                  CAM_LOGW("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)                  CAM_LOGE("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGA(fmt, arg...)                  CAM_LOGA("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGF(fmt, arg...)                  CAM_LOGF("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGD_WITH_OPENID(fmt, arg...)      CAM_LOGD("[%s] (%d) " fmt, __FUNCTION__, getOpenId(), ##arg)

//
#define MY_LOGV_IF(cond, ...)       do { if ( (cond) ) { MY_LOGV(__VA_ARGS__); } }while(0)
#define MY_LOGD_IF(cond, ...)       do { if ( (cond) ) { MY_LOGD(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) ) { MY_LOGI(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) ) { MY_LOGW(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) ) { MY_LOGE(__VA_ARGS__); } }while(0)
#define MY_LOGA_IF(cond, ...)       do { if ( (cond) ) { MY_LOGA(__VA_ARGS__); } }while(0)
#define MY_LOGF_IF(cond, ...)       do { if ( (cond) ) { MY_LOGF(__VA_ARGS__); } }while(0)
#define MY_LOGD_IF_P1(cond, ...)    do { if ( (cond) ) { MY_LOGD_WITH_OPENID(__VA_ARGS__); } }while(0)

/******************************************************************************
 *
 ******************************************************************************/
#define SUPPORT_3A              (1)
#define SUPPORT_ISP             (1)
#define SUPPORT_PERFRAME_CTRL   (0)
#define SUPPORT_EIS             (1)
#if MTK_CAM_VHDR_SUPPORT
#define SUPPORT_VHDR            (1)
#else
#define SUPPORT_VHDR            (0)
#endif

#define SUPPORT_SCALING_CROP    (1)
#define SUPPORT_SCALING_CROP_IMGO   (SUPPORT_SCALING_CROP && (0))
#define SUPPORT_SCALING_CROP_RRZO   (SUPPORT_SCALING_CROP && (1))

#define SUPPORT_STORE_STUFF_BUFFER  (1)

#define FORCE_EIS_ON                (SUPPORT_EIS && (0))
#define FORCE_3DNR_ON               (SUPPORT_EIS && (0))
#define DISABLE_BLOB_DUMMY_BUF      (0)

/******************************************************************************
 *
 ******************************************************************************/

#define FUNCTION_IN             MY_LOGD_IF(1<=mLogLevel, "+");
#define FUNCTION_OUT            MY_LOGD_IF(1<=mLogLevel, "-");
#if 0
#define PUBLIC_API_IN           MY_LOGD_IF(1<=mLogLevel, "API +");
#define PUBLIC_API_OUT          MY_LOGD_IF(1<=mLogLevel, "API -");
#define MY_LOGD1(...)           MY_LOGD_IF(1<=mLogLevel, __VA_ARGS__)
#define MY_LOGD2(...)           MY_LOGD_IF(2<=mLogLevel, __VA_ARGS__)
#else
#define PUBLIC_API_IN           MY_LOGD_IF_P1(1<=mLogLevel, "API +");
#define PUBLIC_API_OUT          MY_LOGD_IF_P1(1<=mLogLevel, "API -");
#define MY_LOGD1(...)           MY_LOGD_IF_P1(1<=mLogLevel, __VA_ARGS__)
#define MY_LOGD2(...)           MY_LOGD_IF_P1(2<=mLogLevel, __VA_ARGS__)
#endif

#define FUNCTION_IN_P1          MY_LOGD_IF_P1(1<=mLogLevel, "+");
#define FUNCTION_OUT_P1         MY_LOGD_IF_P1(1<=mLogLevel, "-");


#define P1THREAD_POLICY         (SCHED_OTHER)
#define P1THREAD_PRIORITY       (ANDROID_PRIORITY_FOREGROUND-2)

#define P1SOFIDX_INIT_VAL       (0)
#define P1SOFIDX_LAST_VAL       (0xFF)
#define P1SOFIDX_NULL_VAL       (0xFFFFFFFF)

/******************************************************************************
 *
 ******************************************************************************/
namespace {
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#if 0
class AAAResult {
    protected:
        struct info{
            sp<IPipelineFrame> spFrame;
            IMetadata          resultVal;
            MUINT32            mFlag;
            info()
                : spFrame(0)
                , resultVal()
                , mFlag(0)
                {}
        };

    protected:
        enum KeyType {
            KeyType_StrobeState = 1,
            KeyType_Rest        = 2, //1,2,4,8,...
        };

    protected:
        mutable Mutex              mLock;
        KeyedVector<MUINT32, info> mData; //key: magicnum, val: info
        MUINT32                    mAllKey;

    public:

        AAAResult()
            : mLock()
            , mData()
            , mAllKey(KeyType_Rest)
            //, mAllKey(KeyType_StrobeState|KeyType_Rest)
            {}

        void add(MUINT32 magicNum, MUINT32 key, MUINT32 val)
        {
             Mutex::Autolock lock(mLock);
             if(key != MTK_FLASH_STATE) {
                 //unSupported
                 return;
             }

             IMetadata::IEntry entry(MTK_FLASH_STATE);
             entry.push_back(val, Type2Type< MUINT8 >()); //{MTK_FLASH_STATE, MUINT8}
             ssize_t i = mData.indexOfKey(magicNum);
             if(i < 0) {
                 info data;
                 data.resultVal.update(MTK_FLASH_STATE, entry);

    data.mFlag |= KeyType_StrobeState;
                 mData.add(magicNum, data);
             } else {
                 info& data = mData.editValueFor(magicNum);
                 data.resultVal.update(MTK_FLASH_STATE, entry);

    data.mFlag |= KeyType_StrobeState;
             }
        }

        void add(MUINT32 magicNum, sp<IPipelineFrame> pframe, IMetadata &rVal)
        {
             Mutex::Autolock lock(mLock);
             ssize_t i = mData.indexOfKey(magicNum);
             if(i < 0) {
                 info data;
                 data.spFrame = pframe;
                 data.resultVal = rVal;

data.mFlag |= KeyType_Rest;
                 mData.add(magicNum, data);
             } else {
                 info& data = mData.editValueFor(magicNum);
                 data.spFrame = pframe;
                 data.resultVal += rVal;
                 data.mFlag |= KeyType_Rest;
             }
        }

        const info& valueFor(const MUINT32& magicNum) const {
            return mData.valueFor(magicNum);
        }

        bool isCompleted(MUINT32 magicNum) {
            Mutex::Autolock lock(mLock);
            return (mData.valueFor(magicNum).mFlag & mAllKey) == mAllKey;
        }

        void removeItem(MUINT32 key) {
            Mutex::Autolock lock(mLock);
            mData.removeItem(key);
        }

        void clear() {
            debug();
            Mutex::Autolock lock(mLock);
            mData.clear();
        }

        void debug() {
            Mutex::Autolock lock(mLock);
            for(size_t i = 0; i < mData.size(); i++) {
                MY_LOGW_IF((mData.valueAt(i).mFlag & KeyType_StrobeState) == 0,
                           "No strobe result: (%d)", mData.keyAt(i));
                MY_LOGW_IF((mData.valueAt(i).mFlag & KeyType_Rest) == 0,
                           "No rest result: (%d)", mData.keyAt(i));
            }
        }
};
#endif
#if 0
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class Storage {

    protected:
        typedef DefaultKeyedVector<MINTPTR, sp<IImageBuffer> >  MapType;
        MapType                    mvStorageQ;
        mutable Mutex              mStorageLock;
        MINT32                     mLogEnable;
    public:
                                   Storage()
                                       : mvStorageQ()
                                       , mStorageLock()
                                       , mLogEnable(0)
                                       {}

        virtual                   ~Storage(){};

        void                       init(MINT32 logEnable)
                                   {
                                       mvStorageQ.clear();
                                       mLogEnable = logEnable;
                                   }

        void                       uninit()
                                   {
                                       mvStorageQ.clear();
                                   }

        void                       enque(sp<IImageStreamBuffer> const& key, sp<IImageBuffer> &value) {
                                       Mutex::Autolock lock(mStorageLock);
                                       MY_LOGD_IF(mLogEnable, "Storage-enque::(key)0x%x/(val)0x%x",
                                           key.get(), value.get());
                                       MY_LOGD_IF(mLogEnable, "Info::(val-pa)0x%x/%d/%d/%d/%d/%d",
                                        value->getBufPA(0),value->getImgSize().w, value->getImgSize().h,
                                        value->getBufStridesInBytes(0), value->getBufSizeInBytes(0), value->getPlaneCount());

                                       mvStorageQ.add(reinterpret_cast<MINTPTR>(key.get()), value);
                                   };


        sp<IImageBuffer>           deque(MINTPTR key) {
                                       Mutex::Autolock lock(mStorageLock);
                                       sp<IImageBuffer> pframe = mvStorageQ.valueFor(key);
                                       if (pframe != NULL)
                                       {
                                           mvStorageQ.removeItem(key); //should un-mark
                                           MY_LOGD_IF(mLogEnable, "Storage-deque::(key)0x%x/(val)0x%x",
                                            key, pframe.get());
                                           MY_LOGD_IF(mLogEnable, "(val-pa)0x%x",
                                            pframe->getBufPA(0));
                                           return pframe;
                                       }
                                       return NULL;
                                   }
        sp<IImageBuffer>           query(MINTPTR key) {
                                       Mutex::Autolock lock(mStorageLock);
                                       sp<IImageBuffer> pframe = mvStorageQ.valueFor(key);
                                       if (pframe != NULL)
                                       {
                                           MY_LOGD_IF(mLogEnable, "Storage-deque::(key)0x%x/(val)0x%x",
                                            key, pframe.get());
                                           MY_LOGD_IF(mLogEnable, "Info::(val-pa)0x%x",
                                            pframe->getBufPA(0));
                                           return pframe;
                                       }
                                       return NULL;
                                   }
};
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//#if (SUPPORT_SCALING_CROP)
#ifdef MAX
#undef MAX
#endif
#ifdef MIN
#undef MIN
#endif
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#ifdef ALIGN_UPPER
#undef ALIGN_UPPER
#endif
#ifdef ALIGN_LOWER
#undef ALIGN_LOWER
#endif
#define ALIGN_UPPER(x,a)    (((x)+((typeof(x))(a)-1))&~((typeof(x))(a)-1))
#define ALIGN_LOWER(x,a)    (((x))&~((typeof(x))(a)-1))


#ifdef CHECK_LAST_FRAME_SKIPPED
#undef CHECK_LAST_FRAME_SKIPPED
#endif
#define CHECK_LAST_FRAME_SKIPPED(LAST_SOF_IDX, THIS_SOF_IDX)\
    ((LAST_SOF_IDX == P1SOFIDX_NULL_VAL) ?\
        (true) :\
        ((LAST_SOF_IDX == P1SOFIDX_LAST_VAL) ?\
            ((THIS_SOF_IDX != 0) ? (true) : (false)) :\
            ((THIS_SOF_IDX != (LAST_SOF_IDX + 1)) ? (true) : (false))));

#ifdef RESIZE_RATIO_MAX_10X
#undef RESIZE_RATIO_MAX_10X
#endif
#define RESIZE_RATIO_MAX_10X    (4)

#ifdef P1_EISO_MIN_HEIGHT
#undef P1_EISO_MIN_HEIGHT
#endif
#define P1_EISO_MIN_HEIGHT      (160)

#ifdef P1_STUFF_BUF_HEIGHT
#undef P1_STUFF_BUF_HEIGHT
#endif
#define P1_STUFF_BUF_HEIGHT(rrzo, config)   (\
    (rrzo && IS_PORT(CONFIG_PORT_EISO, config)) ? (P1_EISO_MIN_HEIGHT) : (1))
//
enum REQ_TYPE
{
    REQ_TYPE_UNKNOWN        = 0,
    REQ_TYPE_NORMAL,
    REQ_TYPE_INITIAL,
    //REQ_TYPE_PADDING,
    //REQ_TYPE_DUMMY,
    REQ_TYPE_REDO
};
//
#ifdef REQ_SET
#undef REQ_SET
#endif
#define REQ_SET(bit)        ((MUINT32)(0x1 << bit))
#ifdef REQ_SET_NONE
#undef REQ_SET_NONE
#endif
#define REQ_SET_NONE        (0x0)
#ifdef IS_OUT
#undef IS_OUT
#endif
#define IS_OUT(out, set)    ((set & REQ_SET(out)) == REQ_SET(out))
enum REQ_OUT
{
    REQ_OUT_RESIZER         = 0,
    REQ_OUT_RESIZER_STUFF,
    REQ_OUT_FULL_PROC,
    REQ_OUT_FULL_PURE,
    REQ_OUT_FULL_COMBO,
    REQ_OUT_FULL_STUFF,
    REQ_OUT_MAX
};
//
#ifdef REQ_REC
#undef REQ_REC
#endif
#define REQ_REC(bit)        ((MUINT32)(0x1 << bit))
#ifdef REQ_REC_NONE
#undef REQ_REC_NONE
#endif
#define REQ_REC_NONE        (0x0)
#ifdef IS_EXP
#undef IS_EXP
#endif
#define IS_EXP(exp, rec)    ((rec & REQ_REC(exp)) == REQ_REC(exp))
enum REQ_EXP
{
    REQ_EXP_UNKNOWN         = 0,
    REQ_EXP_NOBUF_RRZO,
    REQ_EXP_NOBUF_IMGO,
    REQ_EXP_NOBUF_EISO,
    REQ_EXP_NOBUF_LSCO,
    REQ_EXP_MAX
};
//
#ifdef IS_PORT
#undef IS_PORT
#endif
#define IS_PORT(port, set)  ((set & port) == port)
enum CONFIG_PORT
{
    CONFIG_PORT_NONE        = (0x0),
    CONFIG_PORT_RRZO        = (0x1 << 0),
    CONFIG_PORT_IMGO        = (0x1 << 1),
    CONFIG_PORT_EISO        = (0x1 << 2),
    CONFIG_PORT_LCSO        = (0x1 << 3),
    CONFIG_PORT_ALL         = (0xFFFFFFFF) // MUINT32
};

#ifdef BIN_RESIZE
#undef BIN_RESIZE
#endif
#define BIN_RESIZE(x)  (x = (x >> 1))

#ifdef BIN_REVERT
#undef BIN_REVERT
#endif
#define BIN_REVERT(x)  (x = (x << 1))

/******************************************************************************
 *
 ******************************************************************************/
inline MBOOL
isEISOn(
    IMetadata* const inApp
)
{
    if (inApp == NULL) {
        return false;
    }
    MUINT8 eisMode = MTK_CONTROL_VIDEO_STABILIZATION_MODE_OFF;
    if(!tryGetMetadata<MUINT8>(inApp,
        MTK_CONTROL_VIDEO_STABILIZATION_MODE, eisMode)) {
        MY_LOGW_IF(1, "no MTK_CONTROL_VIDEO_STABILIZATION_MODE");
    }
#if FORCE_EIS_ON
    eisMode = MTK_CONTROL_VIDEO_STABILIZATION_MODE_ON;
#endif
    return eisMode == MTK_CONTROL_VIDEO_STABILIZATION_MODE_ON;
}

/******************************************************************************
 *
 ******************************************************************************/
inline MBOOL
is3DNROn(
    IMetadata* const inApp
)
{
    if (inApp == NULL) {
        return false;
    }
    MINT32 e3DnrMode = MTK_NR_FEATURE_3DNR_MODE_OFF;
    if(!tryGetMetadata<MINT32>(inApp,
        MTK_NR_FEATURE_3DNR_MODE, e3DnrMode)) {
        MY_LOGW_IF(1, "no MTK_NR_FEATURE_3DNR_MODE");
    }
#if FORCE_3DNR_ON
    e3DnrMode = MTK_NR_FEATURE_3DNR_MODE_ON;
#endif
    return e3DnrMode == MTK_NR_FEATURE_3DNR_MODE_ON;
}

#if 0
#ifdef P1_REQ_CROP_TAG
#undef P1_REQ_CROP_TAG
#endif
#define P1_REQ_CROP_TAG (MTK_P1NODE_SCALAR_CROP_REGION) // [FIXME] sync correct tag
#endif

/******************************************************************************
 *
 ******************************************************************************/
MBOOL calculateCropInfoFull(
    MUINT32 pixelMode,
    MSize const& sensorSize,
    MSize const& bufferSize,
    MRect const& querySrcRect,
    MRect& resultSrcRect,
    MSize& resultDstSize,
    MINT32 mLogLevel = 0
)
{
    if ((querySrcRect.size().w == sensorSize.w) &&
        (querySrcRect.size().h == sensorSize.h)) {
        return false;
    }
    if ((querySrcRect.size().w > bufferSize.w || // cannot over buffer size
        querySrcRect.size().h > bufferSize.h) ||
        (((querySrcRect.leftTop().x + querySrcRect.size().w) > sensorSize.w) ||
        ((querySrcRect.leftTop().y + querySrcRect.size().h) > sensorSize.h))
        ) {
        MY_LOGD_IF((1 <= mLogLevel), "calculateCropInfoFull input invalid "
            "pixelMode(%d) sensorSize(%dx%d) bufferSize(%dx%d) "
            "querySrcRect_size(%dx%d) querySrcRect_start(%d,%d)", pixelMode,
            sensorSize.w, sensorSize.h, bufferSize.w, bufferSize.h,
            querySrcRect.size().w, querySrcRect.size().h,
            querySrcRect.leftTop().x, querySrcRect.leftTop().y);
        return false;
    }
    // TODO: query the valid value, currently do not crop in IMGO
    resultDstSize = MSize(sensorSize.w, sensorSize.h);
    resultSrcRect = MRect(MPoint(0, 0), resultDstSize);

    return true;
}

/******************************************************************************
 *
 ******************************************************************************/
MBOOL calculateCropInfoResizer(
    MUINT32 pixelMode,
    MUINT32 imageFormat,
    MSize const& sensorSize,
    MSize const& bufferSize,
    MRect const& querySrcRect,
    MRect& resultSrcRect,
    MSize& resultDstSize,
    MINT32 mLogLevel = 0
)
{
    if ((querySrcRect.size().w == sensorSize.w) &&
        (querySrcRect.size().h == sensorSize.h)) {
        return false;
    }
    if ((((querySrcRect.leftTop().x + querySrcRect.size().w) > sensorSize.w) ||
        ((querySrcRect.leftTop().y + querySrcRect.size().h) > sensorSize.h))
        ) {
        MY_LOGD_IF((1 <= mLogLevel), "calculateCropInfoResizer input invalid "
            "pixelMode(%d) sensorSize(%dx%d) bufferSize(%dx%d) "
            "querySrcRect_size(%dx%d) querySrcRect_start(%d,%d)", pixelMode,
            sensorSize.w, sensorSize.h, bufferSize.w, bufferSize.h,
            querySrcRect.size().w, querySrcRect.size().h,
            querySrcRect.leftTop().x, querySrcRect.leftTop().y);
        return false;
    }
    //
    MPoint::value_type src_crop_x = querySrcRect.leftTop().x;
    MPoint::value_type src_crop_y = querySrcRect.leftTop().y;
    MSize::value_type src_crop_w = querySrcRect.size().w;
    MSize::value_type src_crop_h = querySrcRect.size().h;
    MSize::value_type dst_size_w = 0;
    MSize::value_type dst_size_h = 0;
    if (querySrcRect.size().w < bufferSize.w) {
        dst_size_w = querySrcRect.size().w;
        // check start.x
        {
            NSCam::NSIoPipe::NSCamIOPipe::NormalPipe_QueryInfo info;
            NSCam::NSIoPipe::NSCamIOPipe::INormalPipe::query(
                NSCam::NSIoPipe::PORT_RRZO.index,
                NSCam::NSIoPipe::NSCamIOPipe::ENPipeQueryCmd_CROP_START_X,
                (EImageFormat)imageFormat,
                src_crop_x, info);
            src_crop_x = info.crop_x;
        }
        // check size.w
        {
            NSCam::NSIoPipe::NSCamIOPipe::NormalPipe_QueryInfo info;
            NSCam::NSIoPipe::NSCamIOPipe::INormalPipe::query(
                NSCam::NSIoPipe::PORT_RRZO.index,
                NSCam::NSIoPipe::NSCamIOPipe::ENPipeQueryCmd_X_PIX|
                NSCam::NSIoPipe::NSCamIOPipe::ENPipeQueryCmd_STRIDE_BYTE,
                (EImageFormat)imageFormat,
                dst_size_w, info);
            dst_size_w = info.x_pix;
        }
        //
        dst_size_w = MIN(dst_size_w, sensorSize.w);
        src_crop_w = dst_size_w;
        if (src_crop_w > querySrcRect.size().w) {
            if ((src_crop_x + src_crop_w) > sensorSize.w) {
                src_crop_x = sensorSize.w - src_crop_w;
            }
        }
    } else {
        if ((src_crop_w * RESIZE_RATIO_MAX_10X) > (bufferSize.w * 10)) {
            MY_LOGW("calculateCropInfoResizer resize width invalid "
                    "(%d):(%d)", src_crop_w, bufferSize.w);
            return false;
        }
        dst_size_w = bufferSize.w;
    }
    if (querySrcRect.size().h < bufferSize.h) {
        dst_size_h = querySrcRect.size().h;
        dst_size_h = MIN(ALIGN_UPPER(dst_size_h, 2), sensorSize.h);
        src_crop_h = dst_size_h;
        if (src_crop_h > querySrcRect.size().h) {
            if ((src_crop_y + src_crop_h) > sensorSize.h) {
                src_crop_y = sensorSize.h - src_crop_h;
            }
        }
    } else {
        if ((src_crop_h * RESIZE_RATIO_MAX_10X) > (bufferSize.h * 10)) {
            MY_LOGW("calculateCropInfoResizer resize height invalid "
                    "(%d):(%d)", src_crop_h, bufferSize.h);
            return false;
        }
        dst_size_h = bufferSize.h;
    }
    resultDstSize = MSize(dst_size_w, dst_size_h);
    resultSrcRect = MRect(MPoint(src_crop_x, src_crop_y),
                            MSize(src_crop_w, src_crop_h));
    return true;
}
//#endif

#if 1
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class StuffBufferPool {

    #define STUFF_BUFFER_WATER_MARK 8   // "0" the pool will not store buffer
    #define STUFF_BUFFER_MAX_AMOUNT 16  // the max amount for general use case

    enum BUF_STATE
    {
        BUF_STATE_RELEASED  = 0,
        BUF_STATE_ACQUIRED
    };

    struct BufNote {
        public:
                            BufNote()
                                : msName("")
                                , mState(BUF_STATE_RELEASED)
                            {};
                            BufNote(android::String8 name, BUF_STATE state)
                                : msName(name)
                                , mState(state)
                            {};
            virtual         ~BufNote()
                            {};

            android::String8    msName;
            BUF_STATE           mState;
    };

public:
                        StuffBufferPool()
                            : mLogLevel(0)
                            , msName("")
                            , mFormat(0)
                            , mSize(0, 0)
                            , mStride(0)
                            , mSerialNum(0)
                            , mWaterMark(STUFF_BUFFER_WATER_MARK)
                            , mMaxAmount(STUFF_BUFFER_MAX_AMOUNT)
                        {
                            MY_LOGD_IF(mLogLevel > 0, "+++");
                            mUsage = (GRALLOC_USAGE_SW_READ_OFTEN |
                                        GRALLOC_USAGE_HW_CAMERA_READ |
                                        GRALLOC_USAGE_HW_CAMERA_WRITE);
                            if(::property_get_int32("debug.camera.scanline.p1", 0) != 0)
                            {
                                mUsage |= GRALLOC_USAGE_SW_WRITE_OFTEN;
                            }
                            mvInfoMap.clear();
                            MY_LOGD_IF(mLogLevel > 0, "---");
                        };

                        StuffBufferPool(
                            char const * szName,
                            MINT32 format,
                            MSize size,
                            MUINT32 stride,
                            MINT32  log
                        )
                            : mLogLevel(log)
                            , msName(szName)
                            , mFormat(format)
                            , mSize(size)
                            , mStride(stride)
                            , mSerialNum(0)
                            , mWaterMark(STUFF_BUFFER_WATER_MARK)
                            , mMaxAmount(STUFF_BUFFER_MAX_AMOUNT)
                        {
                            MY_LOGD_IF(mLogLevel > 0, "+++");
                            MY_LOGD_IF(mLogLevel > 0, "[%s] 0x%x-%dx%d-%d",
                                szName, format, size.w, size.h, stride);
                            mUsage = (GRALLOC_USAGE_SW_READ_OFTEN |
                                        GRALLOC_USAGE_HW_CAMERA_READ |
                                        GRALLOC_USAGE_HW_CAMERA_WRITE);
                            if(::property_get_int32("debug.camera.scanline.p1", 0) != 0)
                            {
                                mUsage |= GRALLOC_USAGE_SW_WRITE_OFTEN;
                            }
                            mvInfoMap.clear();
                            MY_LOGD_IF(mLogLevel > 0, "---");
                        };

    virtual             ~StuffBufferPool()
                        {
                            MY_LOGD_IF(mLogLevel > 0, "+++");
                            while (mvInfoMap.size() > 0) {
                                destroyBuffer(0); // it remove one of mvInfoMap
                            }
                            mvInfoMap.clear();
                            MY_LOGD_IF(mLogLevel > 0, "[%s] 0x%x-%dx%d-%d",
                                msName.string(), mFormat, mSize.w, mSize.h,
                                mStride);
                            MY_LOGD_IF(mLogLevel > 0, "---");
                        };

    MBOOL               compareLayout(
                            MINT32 format,
                            MSize size,
                            MUINT32 stride
                        )
                        {
                            return ((format == mFormat) &&
                                    (stride == mStride) &&
                                    (size == mSize));
                        };

    MERROR              acquireBuffer(
                            sp<IImageBuffer> & imageBuffer
                        );

    MERROR              releaseBuffer(
                            sp<IImageBuffer> & imageBuffer
                        );

    MERROR              createBuffer(
                            sp<IImageBuffer> & imageBuffer
                        );

    MERROR              destroyBuffer(
                            sp<IImageBuffer> & imageBuffer
                        );

    MERROR              destroyBuffer(
                            size_t index
                        );

private:
    MINT32              mLogLevel;
    android::String8    msName;
    MINT32              mFormat;
    MSize               mSize;
    MUINT32             mStride;
    MUINT               mUsage;
    MUINT32             mSerialNum;
    // it will destroy buffer while releasing, if pool_size > WaterMark
    MUINT32             mWaterMark;
    // it will not create buffer while acquiring, if pool_size >= MaxAmount
    MUINT32             mMaxAmount;
    DefaultKeyedVector< sp<IImageBuffer>, BufNote >
                        mvInfoMap;
};

/******************************************************************************
 *
 ******************************************************************************/
MERROR
StuffBufferPool::
acquireBuffer(sp<IImageBuffer> & imageBuffer)
{
    FUNCTION_IN;
    //
    MERROR ret = OK;
    sp<IImageBuffer> pImgBuf = NULL;
    BufNote bufNote;
    size_t i = 0;
    imageBuffer = NULL;
    //
    for (i = 0; i < mvInfoMap.size(); i++) {
        bufNote = mvInfoMap.editValueAt(i);
        if (BUF_STATE_RELEASED == bufNote.mState) {
            sp<IImageBuffer> const pImageBuffer = mvInfoMap.keyAt(i);
            bufNote.mState = BUF_STATE_ACQUIRED;
            mvInfoMap.replaceValueAt(i, bufNote);
            pImgBuf = pImageBuffer;
            break;
        }
    }
    //
    if (pImgBuf != NULL) {
        MY_LOGD_IF(mLogLevel > 1, "Acquire Stuff Buffer (%s) index(%d) (%d/%d)",
            bufNote.msName.string(), i, mvInfoMap.size(), mWaterMark);
        if (!(pImgBuf->lockBuf(bufNote.msName.string(), mUsage))) {
            MY_LOGE("[%s] Stuff ImgBuf lock fail", bufNote.msName.string());
            return BAD_VALUE;
        }
        imageBuffer = pImgBuf;
        return OK;
    }
    //
    MY_LOGD_IF(mLogLevel > 1, "Acquire Stuff Buffer (NoAvailable) (%d/%d)",
            mvInfoMap.size(), mWaterMark);
    //
    ret = createBuffer(imageBuffer);
    FUNCTION_OUT;
    return ret;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
StuffBufferPool::
releaseBuffer(sp<IImageBuffer> & imageBuffer)
{
    FUNCTION_IN;
    //
    MERROR ret = OK;
    if (imageBuffer == NULL) {
        MY_LOGW("Stuff ImageBuffer not exist");
        return BAD_VALUE;
    }
    ssize_t index = mvInfoMap.indexOfKey(imageBuffer);
    imageBuffer->unlockBuf(mvInfoMap.valueAt(index).msName.string());
    BufNote bufNote = mvInfoMap.editValueAt(index);
    bufNote.mState = BUF_STATE_RELEASED;
    mvInfoMap.replaceValueAt(index, bufNote);
    //
    if (mvInfoMap.size() > mWaterMark) {
        ret = destroyBuffer(index);
    }
    //
    MY_LOGD_IF(mLogLevel > 1, "Release Stuff Buffer (%s) index(%d) (%d/%d)",
        bufNote.msName.string(), index, mvInfoMap.size(), mWaterMark);
    //
    FUNCTION_OUT;
    return ret;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
StuffBufferPool::
createBuffer(sp<IImageBuffer> & imageBuffer)
{
    FUNCTION_IN;
    //
    imageBuffer = NULL;
    // add information to buffer name
    android::String8 imgBufName = android::String8(msName);
    char str[256] = {0};
    snprintf(str, sizeof(str), ":Size%dx%d:Stride%d:Sn%d",
        mSize.w, mSize.h, mStride, ++mSerialNum);
    imgBufName += str;
    //
    if (mvInfoMap.size() >= mMaxAmount) {
        MY_LOGW("[%s] the pool size is over max amount, "
            "please check the buffer usage and situation (%d/%d)",
            imgBufName.string(), mvInfoMap.size(), mMaxAmount);
        return NO_MEMORY;
    }
    // create buffer
    MINT32 bufBoundaryInBytes[3] = {0, 0, 0};
    MUINT32 bufStridesInBytes[3] = {mStride, 0, 0};
    IImageBufferAllocator::ImgParam imgParam =
        IImageBufferAllocator::ImgParam((EImageFormat)mFormat,
            mSize, bufStridesInBytes, bufBoundaryInBytes, 1);
    sp<IIonImageBufferHeap> pHeap =
        IIonImageBufferHeap::create(imgBufName.string(), imgParam);
    if (pHeap == NULL) {
        MY_LOGE("[%s] Stuff ImageBufferHeap create fail", imgBufName.string());
        return BAD_VALUE;
    }
    sp<IImageBuffer> pImgBuf = pHeap->createImageBuffer();
    if (pImgBuf == NULL) {
        MY_LOGE("[%s] Stuff ImageBuffer create fail", imgBufName.string());
        return BAD_VALUE;
    }
    // lock buffer
    if (!(pImgBuf->lockBuf(imgBufName.string(), mUsage))) {
        MY_LOGE("[%s] Stuff ImageBuffer lock fail", imgBufName.string());
        return BAD_VALUE;
    }
    BufNote bufNote(imgBufName, BUF_STATE_ACQUIRED);
    mvInfoMap.add(pImgBuf, bufNote);
    imageBuffer = pImgBuf;
    //
    MY_LOGD_IF(mLogLevel > 1, "Create Stuff Buffer (%s) (%d/%d)",
        imgBufName.string(), mvInfoMap.size(), mWaterMark);
    //
    FUNCTION_OUT;
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
StuffBufferPool::
destroyBuffer(sp<IImageBuffer> & imageBuffer)
{
    FUNCTION_IN;
    //
    MERROR ret = OK;
    if (imageBuffer == NULL) {
        MY_LOGW("Stuff ImageBuffer not exist");
        return BAD_VALUE;
    }
    //
    ret = destroyBuffer(mvInfoMap.indexOfKey(imageBuffer));
    FUNCTION_OUT;
    return ret;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
StuffBufferPool::
destroyBuffer(size_t index)
{
    FUNCTION_IN;
    //
    if (index >= mvInfoMap.size()) {
        MY_LOGW("index(%d) not exist, size(%d)", index, mvInfoMap.size());
        return BAD_VALUE;
    }
    BufNote bufNote = mvInfoMap.valueAt(index);
    sp<IImageBuffer> const pImageBuffer = mvInfoMap.keyAt(index);
    MY_LOGD_IF(mLogLevel > 1, "Destroy Stuff Buffer (%s) index(%d) state(%d)",
        bufNote.msName.string(), index, bufNote.mState);
    if (bufNote.mState == BUF_STATE_ACQUIRED) {
        sp<IImageBuffer> pImgBuf = pImageBuffer;
        pImgBuf->unlockBuf(bufNote.msName.string());
    }
    // destroy buffer
    mvInfoMap.removeItemsAt(index);
    //pImgBuf = NULL;
    //
    FUNCTION_OUT;
    return OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class StuffBufferManager {

public:
                        StuffBufferManager()
                            : mLogLevel(0)
                            , mLock()
                        {
                            MY_LOGD_IF(mLogLevel > 0, "+++");
                            mvPoolSet.clear();
                            MY_LOGD_IF(mLogLevel > 0, "---");
                        };

                        StuffBufferManager(MINT32 log)
                            : mLogLevel(log)
                            , mLock()
                        {
                            MY_LOGD_IF(mLogLevel > 0, "+++");
                            mvPoolSet.clear();
                            MY_LOGD_IF(mLogLevel > 0, "---");
                        };

    virtual             ~StuffBufferManager()
                        {
                            MY_LOGD_IF(mLogLevel > 0, "+++");
                            mvPoolSet.clear();
                            MY_LOGD_IF(mLogLevel > 0, "---");
                        };

    void                setLog(MINT32 log)
                        {
                            Mutex::Autolock _l(mLock);
                            mLogLevel = log;
                        }

    MERROR              acquireStoreBuffer(
                            sp<IImageBuffer> & imageBuffer,
                            char const * szName,
                            MINT32 format,
                            MSize size,
                            MUINT32 stride
                        );

    MERROR              releaseStoreBuffer(
                            sp<IImageBuffer> & imageBuffer
                        );

private:
    MINT32              mLogLevel;
    mutable Mutex       mLock;
    Vector<StuffBufferPool>
                        mvPoolSet;
};

/******************************************************************************
 *
 ******************************************************************************/
MERROR
StuffBufferManager::
acquireStoreBuffer(sp<IImageBuffer> & imageBuffer,
    char const* szName, MINT32 format, MSize size, MUINT32 stride)
{
    FUNCTION_IN;
    //
    Mutex::Autolock _l(mLock);
    MERROR ret = OK;
    //
    StuffBufferPool* bufPool = NULL;
    imageBuffer = NULL;
    //
    Vector<StuffBufferPool>::iterator it = mvPoolSet.begin();
    for(; it != mvPoolSet.end(); it++) {
        if (it->compareLayout(format, size, stride)) {
            bufPool = it;
            break;
        }
    }
    //
    if (bufPool == NULL) {
        StuffBufferPool newPool(szName, format, size, stride, mLogLevel);
        mvPoolSet.push_back(newPool);
        it = mvPoolSet.end();
        bufPool = (it - 1);
        MY_LOGD_IF(mLogLevel > 0, "PoolSet.size(%d)", mvPoolSet.size());
    }
    //
    if (bufPool == NULL) {
        MY_LOGE("Cannot create stuff buffer pool");
        return BAD_VALUE;
    } else {
        ret = bufPool->acquireBuffer(imageBuffer);
    }
    //
    FUNCTION_OUT;
    return ret;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
StuffBufferManager::
releaseStoreBuffer(sp<IImageBuffer> & imageBuffer)
{
    FUNCTION_IN;
    //
    Mutex::Autolock _l(mLock);
    //
    if (imageBuffer == NULL) {
        MY_LOGW("Stuff ImageBuffer not exist");
        return BAD_VALUE;
    }
    //
    MINT const format = imageBuffer->getImgFormat();
    MSize const size = imageBuffer->getImgSize();
    MUINT32 const stride = imageBuffer->getBufStridesInBytes(0);
    //
    StuffBufferPool* bufPool = NULL;
    Vector<StuffBufferPool>::iterator it = mvPoolSet.begin();
    for(; it != mvPoolSet.end(); it++) {
        if (it->compareLayout(format, size, stride)) {
            bufPool = it;
            break;
        }
    }
    //
    if (bufPool == NULL) {
        MY_LOGE("Cannot find stuff buffer pool");
        return BAD_VALUE;
    } else {
        bufPool->releaseBuffer(imageBuffer);
    }
    //
    FUNCTION_OUT;
    return OK;
}
#endif


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  .
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class P1NodeImp
    : public BaseNode
    , public P1Node
    , public IHal3ACb
    , protected Thread
{

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Implementations.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////
    struct QueNode_T {
        MUINT32                             magicNum;
        MUINT32                             sofIdx;
        sp<IPipelineFrame>                  appFrame;
        sp<IImageStreamBuffer>              halFrame_full;
        sp<IImageStreamBuffer>              halFrame_resizer;
        sp<IImageBuffer>                    img_full;
        sp<IImageBuffer>                    img_resizer;
        sp<IImageBuffer>                    buffer_eiso;
        sp<IImageBuffer>                    buffer_lcso;
        //sp<IImageBuffer>                    img_combo;
        MUINT32                             reqType;    /*REQ_TYPE*/
        MUINT32                             reqOutSet;  /*REQ_SET(REQ_OUT)*/
        MUINT32                             reqExpRec;  /*REQ_REC(REQ_EXP)*/
        MUINT32                             capType;    /*E_CAPTURE_TYPE*/
        #if (SUPPORT_SCALING_CROP)
        MSize                               dstSize_full;
        MSize                               dstSize_resizer;
        MRect                               cropRect_full;
        MRect                               cropRect_resizer;
        #endif
        QueNode_T()
            : magicNum(0)
            , sofIdx(P1SOFIDX_INIT_VAL)
            , appFrame(NULL)
            , halFrame_full(NULL)
            , halFrame_resizer(NULL)
            , img_full(NULL)
            , img_resizer(NULL)
            //, img_combo(NULL)
            , buffer_eiso(NULL)
            , buffer_lcso(NULL)
            , reqType(REQ_TYPE_UNKNOWN)
            , reqOutSet(REQ_SET_NONE)
            , reqExpRec(REQ_REC_NONE)
            , capType(E_CAPTURE_NORMAL)
        {}
    };

    struct QueJob_T {
    public:
        Vector<QueNode_T>           mSet;
        MUINT8                      mMaxNum;
        MUINT32                     mFirstMagicNum;
                                    QueJob_T()
                                    : mMaxNum(1)
                                    , mFirstMagicNum(0)
                                    {
                                        mSet.clear();
                                        mSet.setCapacity(mMaxNum);
                                    };
                                    QueJob_T(MUINT8 num)
                                    : mMaxNum(num)
                                    , mFirstMagicNum(0)
                                    {
                                        mSet.clear();
                                        mSet.setCapacity(mMaxNum);
                                    };
        virtual                     ~QueJob_T()
                                    {
                                        mSet.clear();
                                    };
    };

    typedef Vector<QueJob_T>        Que_T;

protected:  ////                    Data Members. (Config)
    mutable RWLock                  mConfigRWLock;
    mutable Mutex                   mInitLock;
    MBOOL                           mInit;

    SortedVector<StreamId_T>        mInStreamIds;
    sp<IMetaStreamInfo>             mInAppMeta;
    sp<IMetaStreamInfo>             mOutAppMeta;
    sp<IMetaStreamInfo>             mInHalMeta;
    sp<IMetaStreamInfo>             mOutHalMeta;
    ImageStreamInfoSetT             mvOutImage_full;
    sp<IImageStreamInfo>            mOutImage_resizer;
    sp<IImageStreamInfo>            mInImage_combo;
    sp<IImageStreamInfo>            mOutImage_combo;
    SensorParams                    mSensorParams;
    //RawParams                       mRawParams;
    sp<IImageStreamBufferPoolT>     mpStreamPool_full;
    sp<IImageStreamBufferPoolT>     mpStreamPool_resizer;
    MUINT8                          mBurstNum;
    QueJob_T                        mQueueJob;

    /**
     * the raw default type, if the request do not set the raw type,
     * it will use this setting to driver
     */
    MUINT32                         mRawDefType;

    /**
     * the raw type option means the capability recorded in each bit,
     * it is decided after the driver configuration
     */
    MUINT32                         mRawOption;

    MBOOL                           mEnableEISO;
    MBOOL                           mEnableLCSO;

    //Storage                         mImageStorage;

protected:  ////                    Data Members. (System capability)
    static const int                mNumInMeta = 2;
    static const int                mNumOutMeta = 3;
    int                             m3AProcessedDepth;
    int                             mNumHardwareBuffer;
    int                             mDelayframe;

protected:  ////
    MUINT32                         mlastNum;
    mutable Mutex                   mlastNumLock;
    MUINT32                         mLastSofIdx;

protected:  ////                    Data Members. (Hardware)
    mutable Mutex                   mHardwareLock;
    mutable Mutex                   mActiveLock;
    MBOOL                           mActive;
    mutable Mutex                   mReadyLock;
    MBOOL                           mReady;
    INormalPipe*                    mpCamIO;
    IHal3A_T*                       mp3A;
    #if SUPPORT_EIS
    EisHal*                         mpEIS;
    #endif
    #if SUPPORT_VHDR
    VHdrHal*                        mpVhdr;
    #endif
    #if SUPPORT_SCALING_CROP
    MRect                           mActiveArray;
    MUINT32                         mPixelMode;
    #endif
    MUINT32                         mConfigPort;
    MBOOL                           mIsBinEn;
    //
    DefaultKeyedVector< sp<IImageBuffer>, android::String8 >
                                    mvStuffBufferInfo;
    mutable Mutex                   mStuffBufferLock;
    //
    StuffBufferManager              mStuffBufMgr;
    //
    #define DRAWLINE_PORT_RRZO      0x1
    #define DRAWLINE_PORT_IMGO      0x2
    MUINT32                         mDebugScanLineMask;
    DebugScanLine*                  mpDebugScanLine;

protected:  ////                    Data Members. (Queue: Request)
    mutable Mutex                   mRequestQueueLock;
    Condition                       mRequestQueueCond;
    Que_T                           mRequestQueue;

protected:  ////                    Data Members. (Queue: 3Alist)
    mutable Mutex                   mControls3AListLock;
    List<MetaSet_T>                 mControls3AList;
    Condition                       mControls3AListCond;

protected:  ////
    //AAAResult                       m3AStorage;

protected:  ////                    Data Members. (Queue: Processing)
    mutable Mutex                   mProcessingQueueLock;
    Condition                       mProcessingQueueCond;
    Que_T                           mProcessingQueue;

protected:  ////                    Data Members. (Queue: drop)
    mutable Mutex                   mDropQueueLock;
    Vector<MUINT>                   mDropQueue;

protected:  ////                    Data Members.
    mutable Mutex                   mThreadLock;
    Condition                       mThreadCond;

protected:  ////                    Data Members.
    DurationProfile                 mDequeThreadProfile;

protected:  ////                    Data Members.
    mutable Mutex                   mPublicLock;

protected:  ////                    Data Members.
    MINT32                          mInFlightRequestCnt;

protected:
    MINT32                          mLogLevel;

protected:  ////                    Data Members.
    MBOOL                           mEnableFrameSync;

protected:  ////                    Operations.

    MVOID                           setActive(
                                        MBOOL active
                                    );

    MBOOL                           getActive(
                                        void
                                    );

    MVOID                           setReady(
                                        MBOOL ready
                                    );

    MBOOL                           getReady(
                                        void
                                    );

    MVOID                           setInit(
                                        MBOOL init
                                    );

    MBOOL                           getInit(
                                        void
                                    );

    MVOID                           onProcessFrame(
                                        MBOOL initial,
                                        RequestSet_T* reqSet = NULL,//MUINT32 magicNum = 0,
                                        MUINT32 sofIdx = P1SOFIDX_INIT_VAL,
                                        CapParam_T* capParam = NULL
                                    );

#if 0
    MVOID                           onProcess3AResult(
                                        MUINT32 magicNum,
                                        MUINT32 key,
                                        MUINT32 val
                                    );
#endif

    MVOID                           onProcessEIS(
                                        QueNode_T const &node,
                                        IMetadata &resultEIS,
                                        QBufInfo const &deqBuf,
                                        MINT32 eisMode,
                                        MUINT8 captureIntent,
                                        MINT64 exposureTime
                                    );

    MERROR                          onProcessEnqueFrame(
                                        QueJob_T &job
                                    );

    MERROR                          onProcessDequedFrame(
                                    );

    MERROR                          onProcessDropFrame(
                                    );

    MBOOL                           getProcessingFrame_ByAddr(
                                        IImageBuffer* const imgBuffer,
                                        MUINT32 magicNum,
                                        QueJob_T &job
                                    );

    QueJob_T                        getProcessingFrame_ByNumber(
                                        MUINT32 magicNum
                                    );


    MVOID                           onHandleFlush(
                                        MBOOL wait
                                    );

    MVOID                           onReturnProcessingFrame(
                                        QueNode_T const &node,
                                        QBufInfo const &deqBuf,
                                        MetaSet_T const &result3A,
                                        IMetadata const &resultEIS
                                    );

    MVOID                           onFlushProcessingFrame(
                                        QueNode_T const& node
                                    );

    MVOID                           onFlushRequestFrame(
                                        QueNode_T const& node
                                    );

    MVOID                           createNode(sp<IPipelineFrame> appframe,
                                               QueJob_T *job,
                                               Que_T *Queue,
                                               Mutex *QueLock,
                                               List<MetaSet_T> *list,
                                               Mutex *listLock
                                    );

    MVOID                           createNode(List<MetaSet_T> *list,
                                               Mutex *listLock
                                    );

    MVOID                           createNode(Que_T &Queue);

protected:  ////                    Hardware Operations.
    MERROR                          hardwareOps_start(
                                    );

    MERROR                          hardwareOps_enque(
                                        QueJob_T &job,
                                        MBOOL toPush = false
                                    );

    MERROR                          hardwareOps_deque(
                                        QBufInfo &deqBuf
                                    );

    MERROR                          hardwareOps_stop(
                                    );

    MERROR                          setupNodeBufInfo(
                                        QueNode_T &node,
                                        QBufInfo &info
                                    );

    MERROR                          createStuffBuffer(
                                        sp<IImageBuffer> & imageBuffer,
                                        sp<IImageStreamInfo> const& streamInfo,
                                        NSCam::MSize::value_type const
                                            changeHeight = 0
                                    );

    MERROR                          createStuffBuffer(
                                        sp<IImageBuffer> & imageBuffer,
                                        char const * szName,
                                        MINT32 format,
                                        MSize size,
                                        MUINT32 stride
                                    );

    MERROR                          destroyStuffBuffer(
                                        sp<IImageBuffer> & imageBuffer
                                    );

    MVOID                           generateAppMeta(
                                        sp<IPipelineFrame> const &request,
                                        MetaSet_T const &result3A,
                                        QBufInfo const &deqBuf,
                                        IMetadata &appMetadata
                                    );

    MVOID                           generateHalMeta(
                                        MetaSet_T const &result3A,
                                        QBufInfo const &deqBuf,
                                        IMetadata const &resultEIS,
                                        IMetadata const &inHalMetadata,
                                        IMetadata &halMetadata
                                    );

    #if (SUPPORT_SCALING_CROP)
    #if 0
    MVOID                           prepareCropInfo(
                                       IMetadata const* pAppMeta,
                                       IMetadata const* pHalMeta,
                                       QueNode_T& node
                                   );
    #endif

    MVOID                           prepareCropInfo(
                                       IMetadata* pAppMetadata,
                                       IMetadata* pHalMetadata,
                                       QueNode_T& node
                                   );
    #endif

    MERROR                          check_config(
                                        ConfigParams const& rParams
                                    );

protected:  ///

    MERROR                          lockMetadata(
                                        sp<IPipelineFrame> const& pFrame,
                                        StreamId_T const streamId,
                                        IMetadata &rMetadata
                                    );

    MERROR                          returnLockedMetadata(
                                        sp<IPipelineFrame> const& pFrame,
                                        StreamId_T const streamId,
                                        MBOOL success = MTRUE
                                    );

    MERROR                          returnUnlockedMetadata(
                                        sp<IPipelineFrame> const& pFrame,
                                        StreamId_T const streamId
                                    );

    MERROR                          lock_and_returnMetadata(
                                        sp<IPipelineFrame> const& pFrame,
                                        StreamId_T const streamId,
                                        IMetadata &metadata
                                    );


    MERROR                          lockImageBuffer(
                                        sp<IPipelineFrame> const& pFrame,
                                        StreamId_T const streamId,
                                        sp<IImageStreamBuffer>  &pOutpImageStreamBuffer,
                                        sp<IImageBuffer> &rImageBuffer
                                    );

    MERROR                          returnLockedImageBuffer(
                                        sp<IPipelineFrame> const& pFrame,
                                        StreamId_T const streamId,
                                        sp<IImageBuffer> const& pImageBuffer,
                                        MBOOL success = MTRUE
                                    );

    MERROR                          returnUnlockedImageBuffer(
                                        sp<IPipelineFrame> const& pFrame,
                                        StreamId_T const streamId
                                    );

    MERROR                          lockImageBuffer(
                                        sp<IImageStreamBuffer> const& pStreamBuffer,
                                        sp<IImageBuffer> &pImageBuffer
                                    );

    MERROR                          returnLockedImageBuffer(
                                        sp<IImageBuffer> const &pImageBuffer,
                                        sp<IImageStreamBuffer> const &pStreamBuffer,
                                        sp<IImageStreamBufferPoolT> const &pStreamPool
                                    );

    MERROR                          returnUnlockedImageBuffer(
                                        sp<IImageStreamBuffer> const& pStreamBuffer,
                                        sp<IImageStreamBufferPoolT> const &pStreamPool
                                    );

    MUINT32                         get_and_increase_magicnum()
                                    {
                                        Mutex::Autolock _l(mlastNumLock);
                                        MUINT32 ret = mlastNum++;
                                        //skip num = 0 as 3A would callback 0 when request stack is empty
                                        //skip -1U as a reserved number to indicate that which would never happen in 3A queue
                                        if(ret==0 || ret==-1U) ret=mlastNum=1;
                                        return ret;
                                    }
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Operations in base class Thread
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////
    // Ask this object's thread to exit. This function is asynchronous, when the
    // function returns the thread might still be running. Of course, this
    // function can be called from a different thread.
    virtual void                    requestExit();

    // Good place to do one-time initializations
    virtual status_t                readyToRun();

private:
    // Derived class must implement threadLoop(). The thread starts its life
    // here. There are two ways of using the Thread object:
    // 1) loop: if threadLoop() returns true, it will be called again if
    //          requestExit() wasn't called.
    // 2) once: if threadLoop() returns false, the thread will exit upon return.
    virtual bool                    threadLoop();

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Interface.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////                    Operations.
                                    P1NodeImp();
    virtual                        ~P1NodeImp();
    virtual MERROR                  config(ConfigParams const& rParams);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  IPipelineNode Interface.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////                    Operations.

    virtual MERROR                  init(InitParams const& rParams);

    virtual MERROR                  uninit();

    virtual MERROR                  flush();

    virtual MERROR                  queue(
                                        sp<IPipelineFrame> pFrame
                                    );

public:     ////                    Operations.

    virtual void                    doNotifyCb (
                                        MINT32  _msgType,
                                        MINTPTR _ext1,
                                        MINTPTR _ext2,
                                        MINTPTR _ext3
                                    );

    static void                     doNotifyDropframe(MUINT magicNum, void* cookie);

};
};  //namespace


/******************************************************************************
 *
 ******************************************************************************/
P1NodeImp::
P1NodeImp()
    : BaseNode()
    , P1Node()
    , mConfigRWLock()
    , mInitLock()
    , mInit(MTRUE)
    //
    , mInAppMeta(NULL)
    , mOutAppMeta(NULL)
    , mInHalMeta(NULL)
    , mOutHalMeta(NULL)
    , mpStreamPool_full(NULL)
    , mpStreamPool_resizer(NULL)
    , mBurstNum(1)
    , mQueueJob(1)
    , mRawDefType(EPipe_PURE_RAW)
    , mRawOption(0)
    , mEnableEISO(MFALSE)
    , mEnableLCSO(MFALSE)
    //
    //, mImageStorage()
    //
    , m3AProcessedDepth(3)
    , mNumHardwareBuffer(3)
    , mDelayframe(3)
    , mlastNum(1)
    , mlastNumLock()
    , mLastSofIdx(P1SOFIDX_NULL_VAL)
    , mHardwareLock()
    , mActiveLock()
    , mActive(MFALSE)
    , mReadyLock()
    , mReady(MFALSE)
    , mpCamIO(NULL)
    , mp3A(NULL)
    #if SUPPORT_EIS
    , mpEIS(NULL)
    #endif
    #if SUPPORT_VHDR
    , mpVhdr(NULL)
    #endif
    //
    #if (SUPPORT_SCALING_CROP)
    , mPixelMode(0)
    #endif
    , mConfigPort(CONFIG_PORT_NONE)
    , mIsBinEn(false)
    //
    , mStuffBufMgr()
    //
    , mDebugScanLineMask(0)
    , mpDebugScanLine(NULL)
    //
    , mRequestQueueLock()
    , mRequestQueueCond()
    , mRequestQueue()
    //
    , mControls3AListLock()
    , mControls3AList()
    , mControls3AListCond()
    //
    //, m3AStorage()
    //
    , mProcessingQueueLock()
    , mProcessingQueueCond()
    , mProcessingQueue()
    //
    , mDropQueueLock()
    , mDropQueue()
    //
    , mThreadLock()
    , mThreadCond()
    //
    , mDequeThreadProfile("P1Node::deque", 15000000LL)
    , mInFlightRequestCnt(0)
    //
    , mEnableFrameSync(MFALSE)
{
    char cProperty[PROPERTY_VALUE_MAX];
    ::property_get("debug.camera.log", cProperty, "0");
    mLogLevel = ::atoi(cProperty);
    if ( 0 == mLogLevel ) {
        ::property_get("debug.camera.log.p1node", cProperty, "0");
        mLogLevel = ::atoi(cProperty);
    }
#if 1 //[FIXME] TempTestOnly
    #warning "[FIXME] force enable P1Node log"
    if (mLogLevel < 2) {
        mLogLevel = 2;
    }
#endif
    mStuffBufMgr.setLog(mLogLevel);
    //
    mDebugScanLineMask = ::property_get_int32("debug.camera.scanline.p1", 0);
    if ( mDebugScanLineMask != 0)
    {
        mpDebugScanLine = DebugScanLine::createInstance();
    }
}


/******************************************************************************
 *
 ******************************************************************************/
P1NodeImp::
~P1NodeImp()
{
    MY_LOGD("");
    if( mpDebugScanLine != NULL )
    {
        mpDebugScanLine->destroyInstance();
        mpDebugScanLine = NULL;
    }
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
init(InitParams const& rParams)
{
    FUNCTION_IN;

    {
        RWLock::AutoWLock _l(mConfigRWLock);
        //
        mOpenId  = rParams.openId;
        mNodeId  = rParams.nodeId;
        mNodeName= rParams.nodeName;
    }

    MERROR err = run();

    FUNCTION_OUT;

    return err;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
uninit()
{
    PUBLIC_API_IN;

    // flush the left frames if exist
    onHandleFlush(MFALSE);

    requestExit();

    PUBLIC_API_OUT;

    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
check_config(ConfigParams const& rParams)
{
    CAM_TRACE_NAME("P1:check_config");

    if (rParams.pInAppMeta == NULL ) {
        MY_LOGE("in metadata is null");
        return BAD_VALUE;
    }

    if (rParams.pOutAppMeta == NULL) {
        MY_LOGE("out app metadata is null");
        return BAD_VALUE;
    }

    if (rParams.pOutHalMeta == NULL) {
        MY_LOGE("out hal metadata is null");
        return BAD_VALUE;
    }

    if (rParams.pvOutImage_full.size() == 0 && rParams.pOutImage_resizer == NULL) {
        MY_LOGE("image is empty");
        return BAD_VALUE;
    }


    if (rParams.pStreamPool_full != NULL && rParams.pvOutImage_full.size() == 0) {
        MY_LOGE("wrong full input");
        return BAD_VALUE;
    }

    if (rParams.pStreamPool_resizer != NULL && rParams.pOutImage_resizer == NULL) {
        MY_LOGE("wrong resizer input");
        return BAD_VALUE;
    }

    //
    {
        RWLock::AutoWLock _l(mConfigRWLock);
        //
        if(rParams.pInAppMeta != 0) {
            mInAppMeta = rParams.pInAppMeta;
            mInStreamIds.add(mInAppMeta->getStreamId());
        } else {
            mInAppMeta = NULL;
            mInStreamIds.clear();
        }
        //
        if(rParams.pInHalMeta != 0) {
            mInHalMeta = rParams.pInHalMeta;
            mInStreamIds.add(mInHalMeta->getStreamId());
        } else {
            mInHalMeta = NULL;
            mInStreamIds.clear();
        }
        mOutAppMeta          = (rParams.pOutAppMeta != 0)?         rParams.pOutAppMeta         : NULL;
        mOutHalMeta          = (rParams.pOutHalMeta != 0)?         rParams.pOutHalMeta         : NULL;
        mOutImage_resizer    = (rParams.pOutImage_resizer != 0)?   rParams.pOutImage_resizer   : NULL;
        mpStreamPool_full    = (rParams.pStreamPool_full != 0)?    rParams.pStreamPool_full    : NULL;
        mpStreamPool_resizer = (rParams.pStreamPool_resizer != 0)? rParams.pStreamPool_resizer : NULL;
        if(rParams.pvOutImage_full.size() != 0) {
            mvOutImage_full  = rParams.pvOutImage_full;
        } else {
            mvOutImage_full.clear();
        }
        //
        mBurstNum = MAX(rParams.burstNum, 1);
        #if 1 //[FIXME] TempTestOnly
        #warning "[FIXME] force to change p1 burst number"
        {
            char cProperty[PROPERTY_VALUE_MAX] = {0};
            ::property_get("debug.camera.p1burst", cProperty, "0");
            MUINT8 burst_num = ::atoi(cProperty);
            if (burst_num > 0) {
                mBurstNum = burst_num;
            }
            MY_LOGI("debug.camera.p1burst = %d  -  BurstNum = %d",
                burst_num, mBurstNum);
        }
        #endif
        //
        mSensorParams = rParams.sensorParams;
        //
        mRawDefType = EPipe_PURE_RAW;
        mRawOption = (1 << EPipe_PURE_RAW);
        if (rParams.rawProcessed == MTRUE) {
            mRawDefType = EPipe_PROCESSED_RAW;
            mRawOption |= (1 << EPipe_PROCESSED_RAW);
        }
        //
        mEnableEISO = rParams.enableEIS;
        mEnableLCSO = rParams.enableLCS;
		mEnableFrameSync = rParams.enableFrameSync;
    }

    MY_LOGI("[Config] sensor(%d, %d, %d, %dx%d) raw(%d)(0x%x)"
        "EIS(%d) LCS(%d) burst(%d)",
        mSensorParams.mode, mSensorParams.fps, mSensorParams.pixelMode,
        mSensorParams.size.w, mSensorParams.size.h, mRawDefType, mRawOption,
        mEnableEISO, mEnableLCSO, mBurstNum);

    MY_LOGI_IF(mOutAppMeta!= NULL, "[Config] "
        "In Meta Id: 0x%x, Out APP Meta Id: 0x%x, Out Hal Meta Id: 0x%x",
        mInAppMeta->getStreamId(),
        mOutAppMeta->getStreamId(),
        mOutHalMeta->getStreamId());

    for(size_t i = 0; i < mvOutImage_full.size(); i++) {
        MY_LOGI("[Config] full image Id: 0x%x (%d,%d) pool(%p)",
                 mvOutImage_full[i]->getStreamId(),
                 mvOutImage_full[i]->getImgSize().w,
                 mvOutImage_full[i]->getImgSize().h,
                 ((mpStreamPool_full != NULL) ?
                 (mpStreamPool_full.get()) : (NULL)));
    }
    if (mOutImage_resizer != NULL) {
        MY_LOGI("[Config] resizer image Id: 0x%x (%d,%d) pool(%p)",
                 mOutImage_resizer->getStreamId(),
                 mOutImage_resizer->getImgSize().w,
                 mOutImage_resizer->getImgSize().h,
                 ((mpStreamPool_resizer != NULL) ?
                 (mpStreamPool_resizer.get()) : (NULL)));

    }

    MY_LOGI("[Config] enableFrameSync:%d", mEnableFrameSync);

#if (SUPPORT_SCALING_CROP)
    {
        sp<IMetadataProvider> pMetadataProvider = NSMetadataProviderManager::valueFor(getOpenId());
        if( ! pMetadataProvider.get() ) {
            MY_LOGE(" ! pMetadataProvider.get() ");
            return DEAD_OBJECT;
        }
        IMetadata static_meta =
            pMetadataProvider->geMtktStaticCharacteristics();
        if( tryGetMetadata<MRect>(&static_meta,
            MTK_SENSOR_INFO_ACTIVE_ARRAY_REGION, mActiveArray) ) {
            MY_LOGD_IF(1,"active array(%d, %d, %dx%d)",
                    mActiveArray.p.x, mActiveArray.p.y,
                    mActiveArray.s.w, mActiveArray.s.h);
        } else {
            MY_LOGE("no static info: MTK_SENSOR_INFO_ACTIVE_ARRAY_REGION");
            #ifdef USING_MTK_LDVT
            mActiveArray = MRect(mSensorParams.size.w, mSensorParams.size.h);
            MY_LOGI("set sensor size to active array(%d, %d, %dx%d)",
                    mActiveArray.p.x, mActiveArray.p.y,
                    mActiveArray.s.w, mActiveArray.s.h);
            #else
            return UNKNOWN_ERROR;
            #endif
        }
    }
#endif

    return OK;

}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
config(ConfigParams const& rParams)
{
    PUBLIC_API_IN

    Mutex::Autolock _l(mPublicLock);
    //(1) check
    MERROR err = check_config(rParams);
    if (err != OK) {
        return err;
    }

    //(2) configure hardware

    if(getActive()) {
        MY_LOGD("active=%d", getActive());
        onHandleFlush(MFALSE);
    }

    err = hardwareOps_start();
    if (err != OK) {
        return err;
    }

    PUBLIC_API_OUT

    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
queue(
    sp<IPipelineFrame> pFrame
)
{
    PUBLIC_API_IN

    CAM_TRACE_NAME("P1:queue");
    MY_LOGD1("active=%d", getActive());

    if (!getActive()) {
        MY_LOGI("[%s#%d]", __FUNCTION__,  __LINE__);
        sp<IPipelineFrame> t1Frame;
        MY_LOGI("[%s#%d]", __FUNCTION__,  __LINE__);
        hardwareOps_start();
        MY_LOGI("[%s#%d]", __FUNCTION__,  __LINE__);
        sp<IPipelineFrame> t2Frame;
        MY_LOGI("[%s#%d]", __FUNCTION__,  __LINE__);
    }

    {
        Mutex::Autolock _l(mControls3AListLock);

        //block condition 1: if pipeline is full
        while (mControls3AList.size() > (size_t)(((m3AProcessedDepth + 1) * mBurstNum) - 1))
        {
            MY_LOGD1("wait: %d > %d", mControls3AList.size(), (size_t)m3AProcessedDepth);
            status_t status = mControls3AListCond.wait(mControls3AListLock);
            MY_LOGD1("wait-");
            if  ( OK != status ) {
                MY_LOGW(
                    "wait status:%d:%s, mControls3AList.size:%zu",
                    status, ::strerror(-status), mControls3AList.size()
                );
            }
        }
        //compensate to the number of mProcessedDepth
        while(mControls3AList.size() < (size_t)(m3AProcessedDepth * mBurstNum))
        {
            createNode(&mControls3AList, NULL);
        }

        //push node from appFrame
        createNode(pFrame, &mQueueJob, &mRequestQueue, &mRequestQueueLock, &mControls3AList, NULL);
        if (getInit() && (!mRequestQueue.empty())) {
            Mutex::Autolock _l(mRequestQueueLock);
            if (!mRequestQueue.empty()) {
                mRequestQueueCond.broadcast();
            }
        }
        android_atomic_inc(&mInFlightRequestCnt);
        ATRACE_INT("P1_request_cnt", android_atomic_acquire_load(&mInFlightRequestCnt));
    }

    PUBLIC_API_OUT

    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
flush()
{
    PUBLIC_API_IN

    CAM_TRACE_NAME("P1:flush");

    Mutex::Autolock _l(mPublicLock);

    onHandleFlush(MFALSE);

    //[TODO]
    //wait until deque thread going back to waiting state;
    //in case next node receives queue() after flush()

    PUBLIC_API_OUT

    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
void
P1NodeImp::
requestExit()
{
    FUNCTION_IN_P1

    //let deque thread back
    Thread::requestExit();
    {
        Mutex::Autolock _l(mThreadLock);
        mThreadCond.broadcast();
    }
    //
    if (getInit()) {
        Mutex::Autolock _l(mRequestQueueLock);
        setInit(MFALSE);
        mRequestQueueCond.broadcast();
    }
    join();

    //let enque thread back
    Mutex::Autolock _l(mControls3AListLock);
    mControls3AListCond.broadcast();

    FUNCTION_OUT_P1
}


/******************************************************************************
 *
 ******************************************************************************/
status_t
P1NodeImp::
readyToRun()
{
    // set name
    ::prctl(PR_SET_NAME, (unsigned long)"Cam@P1NodeImp", 0, 0, 0);

    // set normal
    struct sched_param sched_p;
    sched_p.sched_priority = 0;
    ::sched_setscheduler(0, P1THREAD_POLICY, &sched_p);
    ::setpriority(PRIO_PROCESS, 0, P1THREAD_PRIORITY);   //  Note: "priority" is nice value.
    //
    ::sched_getparam(0, &sched_p);
    MY_LOGD(
        "Tid: %d, policy: %d, priority: %d"
        , ::gettid(), ::sched_getscheduler(0)
        , sched_p.sched_priority
    );

    //
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
bool
P1NodeImp::
threadLoop()
{
    // check if going to leave thread
    CAM_TRACE_NAME("P1:threadLoop");
    {
        Mutex::Autolock _l(mThreadLock);

        if (!getReady()) {
            MY_LOGD("wait+");
            mThreadCond.wait(mThreadLock);
            MY_LOGD("wait-");
        }

        if (exitPending()) {
            MY_LOGD("leaving");
            return false;
        }
    }

    // deque buffer, and handle frame and metadata
    onProcessDequedFrame();


    // trigger point for the first time
    {
        RWLock::AutoWLock _l(mConfigRWLock);
        if (getInit()) {
            {
                Mutex::Autolock _l(mRequestQueueLock);
                while(getInit() && mRequestQueue.empty()) {
                    MY_LOGD1("init request wait+");
                    mRequestQueueCond.wait(mRequestQueueLock);
                    MY_LOGD1("init request wait-");
                }
            }
            onProcessFrame(MTRUE);
            setInit(MFALSE);
        }
    }

    onProcessDropFrame();
    return true;
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
P1NodeImp::
setActive(
    MBOOL active
)
{
    Mutex::Autolock _l(mActiveLock);
    mActive = active;
}


/******************************************************************************
 *
 ******************************************************************************/
MBOOL
P1NodeImp::
getActive(
    void
)
{
    Mutex::Autolock _l(mActiveLock);
    return mActive;
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
P1NodeImp::
setReady(
    MBOOL ready
)
{
    Mutex::Autolock _l(mReadyLock);
    mReady = ready;
}


/******************************************************************************
 *
 ******************************************************************************/
MBOOL
P1NodeImp::
getReady(
    void
)
{
    Mutex::Autolock _l(mReadyLock);
    return mReady;
}



/******************************************************************************
 *
 ******************************************************************************/
MVOID
P1NodeImp::
setInit(
    MBOOL init
)
{
    Mutex::Autolock _l(mInitLock);
    mInit = init;
}


/******************************************************************************
 *
 ******************************************************************************/
MBOOL
P1NodeImp::
getInit(
    void
)
{
    Mutex::Autolock _l(mInitLock);
    return mInit;
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
P1NodeImp::
onProcessFrame(
    MBOOL initial,
    RequestSet_T* reqSet,//MUINT32 magicNum,
    MUINT32 sofIdx,
    CapParam_T* capParam
)
{
    FUNCTION_IN_P1

    MUINT32 magicNum = 0;

    if (reqSet != NULL) {
        magicNum = reqSet->vNumberSet[0];
    }

    //(1)
    if(!initial) { // [TODO] && VALID 3A PROCESSED NOTIFY
        QueJob_T job(mBurstNum);
        bool exist = false;
        {
            Mutex::Autolock _l(mRequestQueueLock);
            Que_T::iterator it = mRequestQueue.begin();
            for(; it != mRequestQueue.end(); it++) {
                if ((*it).mFirstMagicNum == magicNum) {
                    job = *it;
                    for (MUINT8 i = 0; i < job.mSet.size(); i++) {
                        job.mSet.editItemAt(i).sofIdx = sofIdx;
                        if (capParam != NULL) {
                            job.mSet.editItemAt(i).capType =
                                capParam->u4CapType;
                            if ((job.mSet[i].capType != E_CAPTURE_NORMAL) &&
                                (job.mSet[i].appFrame != NULL)) {
                                MY_LOGI_IF((mLogLevel > 0),
                                "Job(%d) - CapType(%d) @ M(%d) F(%d) R(%d)",
                                job.mFirstMagicNum,
                                job.mSet[i].capType, job.mSet[i].magicNum,
                                job.mSet[i].appFrame->getFrameNo(),
                                job.mSet[i].appFrame->getRequestNo());
                            }
                        } else {
                            MY_LOGW("cannot find cap param (%d)", magicNum);
                        }
                    }
                    mRequestQueue.erase(it);
                    exist = true;
                    break;
                }
            }
        }
        if (exist) {
            if (OK != onProcessEnqueFrame(job)) {
                MY_LOGE("frame en-queue fail (%d)", magicNum);
                for (MUINT8 i = 0; i < job.mSet.size(); i++) {
                    onFlushRequestFrame(job.mSet[i]);
                }
            }
        } else {
            MY_LOGW_IF(magicNum!=0, "no: %d", magicNum);
            Mutex::Autolock _l(mRequestQueueLock);
            String8 str;
            str += String8::format("[req/size(%d)]: ", (int)(mRequestQueue.size()));
            Que_T::iterator it = mRequestQueue.begin();
            for(; it != mRequestQueue.end(); it++) {
                str += String8::format("%d ", (*it).mFirstMagicNum);
            }
            MY_LOGD("%s", str.string());
        }
    }

    //(2)
    {
        Mutex::Autolock _l(mControls3AListLock);
        if (!mControls3AList.empty()) {
            for (MUINT8 n = 0; n < mBurstNum; n++) {
                mControls3AList.erase(mControls3AList.begin());
            }
        }
        mControls3AListCond.broadcast();

        //dump
        MY_LOGD1("mControls3AList size %d", mControls3AList.size());
        String8 str("[3A]: ");
        List<MetaSet_T>::iterator it = mControls3AList.begin();
        for (; it != mControls3AList.end(); it++) {
            str += String8::format("%d ", it->halMeta.entryFor(MTK_P1NODE_PROCESSOR_MAGICNUM).itemAt(0, Type2Type< MINT32 >()));
        }
        MY_LOGD1("%s", str.string());

    }

    //(3)
    #if SUPPORT_3A
    if (mp3A) {
        mp3A->set(mControls3AList);
    }
    #endif
    FUNCTION_OUT_P1
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
onProcessEnqueFrame(
    QueJob_T &job
)
{
    FUNCTION_IN_P1;

    //(1)
    //pass request directly if it's a reprocessing one
    //[TODO]
    //if( mInHalMeta == NULL) {
    //    onDispatchFrame(pFrame);
    //    return;
    //}

    //(2)
    return hardwareOps_enque(job, true);

    FUNCTION_OUT_P1;
}


/******************************************************************************
 *
 ******************************************************************************/
P1NodeImp::QueJob_T
P1NodeImp::
getProcessingFrame_ByNumber(MUINT32 magicNum)
{
    FUNCTION_IN_P1;
    QueJob_T job(mBurstNum);

    Mutex::Autolock _l(mProcessingQueueLock);
    if (mProcessingQueue.empty()) {
        MY_LOGE("mProcessingQueue is empty");
        return job;
    }

    #if 1
        Que_T::iterator it = mProcessingQueue.begin();
        for (; it != mProcessingQueue.end(); it++) {
            job = *it;
            if (job.mFirstMagicNum == magicNum) {
                break;
            }
        }
        if (it == mProcessingQueue.end()) {
            MY_LOGE("cannot find the right node for num: %d", magicNum);
            return job;
        }
        else {
            job = *it;
            mProcessingQueue.erase(it);
            mProcessingQueueCond.broadcast();
        }
    #else
        job = *mProcessingQueue.begin();
        mProcessingQueue.erase(mProcessingQueue.begin());
        mProcessingQueueCond.broadcast();
    #endif

    FUNCTION_OUT_P1;
    //
    return job;
}


/******************************************************************************
 *
 ******************************************************************************/
MBOOL
P1NodeImp::
getProcessingFrame_ByAddr(IImageBuffer* const imgBuffer,
                          MUINT32 magicNum,
                          QueJob_T &job
)
{
    FUNCTION_IN_P1;

    MBOOL ret = MFALSE;
    if (imgBuffer == NULL) {
        MY_LOGE("imgBuffer == NULL");
        return ret;
    }

    // get the right node from mProcessingQueue
    Mutex::Autolock _l(mProcessingQueueLock);
    if (mProcessingQueue.empty()) {
        MY_LOGE("ProQ is empty");
        return ret;
    }

    Que_T::iterator it = mProcessingQueue.begin();
    for (; it != mProcessingQueue.end(); it++) {
        job = *it;
        if (imgBuffer == (*it).mSet[0].img_full.get() ||
            imgBuffer == (*it).mSet[0].img_resizer.get()) {
            if (job.mFirstMagicNum == magicNum) {
                ret = MTRUE;
            } else {
                #if SUPPORT_PERFRAME_CTRL
                MY_LOGE("magicNum from driver(%d), should(%d)",
                       magicNum, job.mFirstMagicNum);
                #else
                if((magicNum & 0x40000000) != 0) {
                    MY_LOGW("magicNum from driver(%b) is uncertain",
                          magicNum);
                    ret = MFALSE;
                } else {
                    ret = MTRUE;
                    MY_LOGW("magicNum from driver(%d), should(%d)",
                          magicNum, job.mFirstMagicNum);
                }
                #endif
            }
            break;
        } else {
            continue;
        }
    }

    if (it == mProcessingQueue.end()) {
        MY_LOGE("no node with imagebuf(0x%x), PA(0x%x), num(%d)",
                 imgBuffer, imgBuffer->getBufPA(0), magicNum);
        for (Que_T::iterator it = mProcessingQueue.begin(); it != mProcessingQueue.end(); it++) {
            MY_LOGW("[proQ] num(%d)", (*it).mSet[0].magicNum);
            MY_LOGW_IF((*it).mSet[0].img_full!=NULL, "[proQ] imagebuf(0x%x), PA(0x%x)",
                (*it).mSet[0].img_full.get(), (*it).mSet[0].img_full->getBufPA(0));
            MY_LOGW_IF((*it).mSet[0].img_resizer!=NULL, "[proQ] imagebuf(0x%x), PA(0x%x)",
                (*it).mSet[0].img_resizer.get(), (*it).mSet[0].img_resizer->getBufPA(0));
        }
        for (Que_T::iterator it = mRequestQueue.begin(); it != mRequestQueue.end(); it++) {
            MY_LOGW("[reqQ] magic %d:", (*it).mFirstMagicNum);
        }
    }
    else {
        job = *it;
        mProcessingQueue.erase(it);
        mProcessingQueueCond.broadcast();
        MY_LOGD1("magic: %d", magicNum);
    }

    FUNCTION_OUT_P1;
    //
    return ret;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
onProcessDropFrame()
{
    Vector<QueNode_T > nodeQ;
    {
        Mutex::Autolock _l(mDropQueueLock);
        for(size_t i = 0; i < mDropQueue.size(); i++) {
            QueJob_T job = getProcessingFrame_ByNumber(mDropQueue[i]);
            // if getProcessingFrame_ByNumber can not find the job
            // the job set size is 0
            for (MUINT8 i = 0; i < job.mSet.size(); i++) {
                QueNode_T node = job.mSet.editItemAt(i);
                nodeQ.push_back(node);
            }
            MY_LOGD("drop: %d", mDropQueue[i]);
        }
        mDropQueue.clear();
    }

    for(size_t i = 0; i < nodeQ.size(); i++) {
        #if SUPPORT_EIS
        if (mpEIS && (nodeQ[i].buffer_eiso != 0)) {
            sp<IImageBuffer> pImgBuf = nodeQ[i].buffer_eiso;
            mpEIS->NotifyEis(pImgBuf);
        }
        #endif
        #if SUPPORT_VHDR
        if (mpVhdr && (nodeQ[i].buffer_lcso != 0)) {
            sp<IImageBuffer> pImgBuf = nodeQ[i].buffer_lcso;
            //mpVhdr->NotifyLcs(pImgBuf);
        }
        #endif
        onFlushProcessingFrame(nodeQ[i]);
    }

    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
onProcessDequedFrame()
{

#if 0
    // [FIXME]  temp-WA for DRV currently not implement self-signal
    //          the dequeue might be blocked while en-queue empty
    //          it should be removed after DRV self-signal ready
    {
        Mutex::Autolock _ll(mProcessingQueueLock);
        if (mProcessingQueue.empty()) {
            return OK;
        }
    }
#endif

    FUNCTION_IN_P1;

    CAM_TRACE_NAME("P1:ProcessDequedFrame");

    MERROR ret;
    QBufInfo deqBuf;
    MetaSet_T result3A;
    if(hardwareOps_deque(deqBuf) != OK) {
        return BAD_VALUE;
    }

    if (deqBuf.mvOut.size() == 0) {
        MY_LOGI("DeqBuf Out Size is 0");
        return ((getActive()) ? BAD_VALUE : OK);
    }

    QueJob_T job(mBurstNum);
    MBOOL match = getProcessingFrame_ByAddr(deqBuf.mvOut[0].mBuffer,
                    deqBuf.mvOut[0].mMetaData.mMagicNum_hal, job);
    for (MUINT8 i = 0; i < job.mSet.size(); i++) {
        QueNode_T node = job.mSet.editItemAt(i);
        MY_LOGD2("job(%d)[%d] = node(%d)",
            job.mFirstMagicNum, i, node.magicNum);
        #if SUPPORT_3A
        if (getActive() && mp3A) {
            mp3A->notifyP1Done(deqBuf.mvOut[0].mMetaData.mMagicNum_hal);
            if (match && node.capType == E_CAPTURE_HIGH_QUALITY_CAPTURE) {
                MY_LOGD1("GetCurrent(%d) +++", node.magicNum);
                mp3A->getCur(deqBuf.mvOut[0].mMetaData.mMagicNum_hal, result3A);
                MY_LOGD1("GetCurrent(%d) ---", node.magicNum);
            } else {
                mp3A->get(deqBuf.mvOut[0].mMetaData.mMagicNum_hal, result3A);
            }
        }
        #endif

        // check the ReqErrRec
        if (match && (node.reqExpRec != REQ_REC_NONE)) {
            MY_LOGI("Mnum(%d) ReqRec:0x%x", node.magicNum, node.reqExpRec);
            #if 0 // flush this frame
            match = MFALSE;
            #endif
        }
        // check the result of raw type
        if (match) {
            MBOOL raw_match = MTRUE;
            for (size_t i = 0; i < deqBuf.mvOut.size(); i++) {
                if (deqBuf.mvOut.at(i).mPortID.index == PORT_IMGO.index) {
                    if (IS_OUT(REQ_OUT_FULL_PROC, node.reqOutSet) &&
                        deqBuf.mvOut[i].mMetaData.mRawType != EPipe_PROCESSED_RAW) {
                        raw_match = MFALSE;
                    }
                    break;
                }
            }
            //MY_LOGD2("raw match: %d", raw_match);
            if (!raw_match) {
                MY_LOGE("Raw type not match (0x%x) @ (%d)(%d)", node.reqOutSet,
                    node.magicNum, deqBuf.mvOut[0].mMetaData.mMagicNum_hal);
                #if 1 // flush this frame
                match = MFALSE;
                #endif
            }
        }
        //
        #if 0
        if (node.magicNum > 0 && node.magicNum < 15) {
            printf("[%d]node.img_resizer.get() = [%p]\n",
                node.magicNum, node.img_resizer.get());
            if (node.img_resizer.get() != NULL) {
                char filename[256] = {0};
                sprintf(filename, "/sdcard/raw/P1B_%d_%dx%d.raw",
                    node.magicNum,
                    node.img_resizer->getImgSize().w,
                    node.img_resizer->getImgSize().h
                    );
                printf("SAVE BUF [%s]\n", filename);
                node.img_resizer->saveToFile(filename);
            }
        }
        #endif
        if (match == MFALSE) {
            onFlushProcessingFrame(node);
            ret = BAD_VALUE;
        }
        else {
            IMetadata resultEIS;
            IMetadata inAPP,inHAL;
            if ((IS_PORT(CONFIG_PORT_EISO, mConfigPort)) && (OK ==
                lockMetadata(node.appFrame, mInAppMeta->getStreamId(), inAPP))) {
                if (isEISOn(&inAPP) || is3DNROn(&inAPP)) {

                    MUINT8 mode = 0;
                    MINT64 exposureTime = 0;
                    MINT32 eisMode = 0;

                    if (!tryGetMetadata<MUINT8>(&inAPP, MTK_CONTROL_CAPTURE_INTENT, mode)) {
                        MY_LOGW("no MTK_CONTROL_CAPTURE_INTENT");
                    }

                    if (!tryGetMetadata<MINT64>(&result3A.appMeta, MTK_SENSOR_EXPOSURE_TIME, exposureTime)) {
                        MY_LOGW("no MTK_SENSOR_EXPOSURE_TIME");
                    }

                    if(OK == lockMetadata(node.appFrame, mInHalMeta->getStreamId(), inHAL)) {
                        IMetadata::IEntry entry = inHAL.entryFor(MTK_EIS_MODE);
                        if (entry.count() > 0) {
                            eisMode = entry.itemAt(0, Type2Type<MINT32>());
                        }
                    }
                    onProcessEIS(node, resultEIS, deqBuf, eisMode, mode, exposureTime);
                }
            }
            onReturnProcessingFrame(node, deqBuf, result3A, resultEIS);
            mLastSofIdx = node.sofIdx;
            ret = OK;
        }
    }

    #if SUPPORT_EIS
    if (IS_PORT(CONFIG_PORT_EISO, mConfigPort)) {
        // call EIS notify function
        if (mpEIS)
        {
            mpEIS->NotifyEis(deqBuf);
        }
    }
    #endif
    #if SUPPORT_VHDR
    if (IS_PORT(CONFIG_PORT_LCSO, mConfigPort)) {
        // call LCS notify function -> VhdrHal no need to call notify
        // mpLCS->NotifyLcs(deqBuf);

    }
    #endif

    FUNCTION_OUT_P1;

    return ret;
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
P1NodeImp::
onHandleFlush(
    MBOOL wait
)
{
    FUNCTION_IN_P1;
    CAM_TRACE_NAME("P1:HandleFlush");

    //wake up queue thread.
    {
        Mutex::Autolock _l(mControls3AListLock);
        mControls3AListCond.broadcast();
    }

    //stop hardware
    if (!wait) {
        hardwareOps_stop(); //include hardware and 3A
    }

    //(1) clear request queue
    {
        Mutex::Autolock _l(mRequestQueueLock);
        while(!mRequestQueue.empty()) {
            QueJob_T job = *mRequestQueue.begin();
            mRequestQueue.erase(mRequestQueue.begin());
            for (MUINT8 i = 0; i < job.mSet.size(); i++) {
                QueNode_T node = job.mSet[i];
                onFlushRequestFrame(node);
            }
        }
        if (!mQueueJob.mSet.empty()) {
            for (MUINT8 i = 0; i < mQueueJob.mSet.size(); i++) {
                QueNode_T node = mQueueJob.mSet[i];
                onFlushRequestFrame(node);
            }
            mQueueJob.mSet.clear();
        }
    }

    //(2) clear processing queue
    //     wait until processing frame coming out
    if (wait) {
        Mutex::Autolock _l(mProcessingQueueLock);
        while(!mProcessingQueue.empty()) {
            mProcessingQueueCond.wait(mProcessingQueueLock);
        }
    } else {
        // must guarantee hardware has been stopped.
        Mutex::Autolock _l(mProcessingQueueLock);
        while(!mProcessingQueue.empty()) {
            QueJob_T job = *mProcessingQueue.begin();
            mProcessingQueue.erase(mProcessingQueue.begin());
            for (MUINT8 i = 0; i < job.mSet.size(); i++) {
                QueNode_T node = job.mSet[i];
                onFlushProcessingFrame(node);
            }
        }
    }

    //(3) clear dummy queue

    //(4) clear drop frame queue
    onProcessDropFrame();


    //(5) clear all
    mRequestQueue.clear(); //suppose already clear
    mProcessingQueue.clear(); //suppose already clear
    mControls3AList.clear();
    //mImageStorage.uninit();
    //m3AStorage.clear();
    mlastNum = 1;

    FUNCTION_OUT_P1;
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
P1NodeImp::
onReturnProcessingFrame(
    QueNode_T const &node,//&pFrame,
    QBufInfo const &deqBuf,
    MetaSet_T const &result3A,
    IMetadata const &resultEIS
)
{
    FUNCTION_IN_P1;

    CAM_TRACE_NAME("P1:ReturnProcessingFrame");

    if (node.appFrame != 0) {

        // Out Image Stream
        if (mOutImage_resizer != NULL) {
            if (IS_OUT(REQ_OUT_RESIZER_STUFF, node.reqOutSet)) {
                if (mpStreamPool_resizer != NULL) {
                    returnLockedImageBuffer(node.img_resizer,
                        node.halFrame_resizer, mpStreamPool_resizer);
                } else {
                    sp<IImageBuffer> pImgBuf = node.img_resizer;
                    destroyStuffBuffer(pImgBuf);
                }
            } else if (IS_EXP(REQ_EXP_NOBUF_RRZO, node.reqExpRec)) {
                sp<IImageBuffer> pImgBuf = node.img_resizer;
                destroyStuffBuffer(pImgBuf);
            } else {
                returnLockedImageBuffer(node.appFrame,
                    mOutImage_resizer->getStreamId(),
                    node.img_resizer, true);
            }
        }
        for(size_t i = 0; i < mvOutImage_full.size(); i++) {
            if (IS_OUT(REQ_OUT_FULL_STUFF, node.reqOutSet)) {
                if (mpStreamPool_full != NULL) {
                    returnLockedImageBuffer(node.img_full,
                        node.halFrame_full, mpStreamPool_full);
                } else {
                    sp<IImageBuffer> pImgBuf = node.img_full;
                    destroyStuffBuffer(pImgBuf);
                }
            } else if (IS_EXP(REQ_EXP_NOBUF_IMGO, node.reqExpRec)) {
                sp<IImageBuffer> pImgBuf = node.img_full;
                destroyStuffBuffer(pImgBuf);
            } else {
                returnLockedImageBuffer(node.appFrame,
                    mvOutImage_full[i]->getStreamId(),
                    node.img_full, true);
            }
        }

        // APP in Meta Stream
        if (mInAppMeta != NULL) {
            StreamId_T const streamId_InAppMeta = mInAppMeta->getStreamId();
            returnLockedMetadata(node.appFrame, streamId_InAppMeta);
        }

        // APP out Meta Stream
        if (mOutAppMeta != NULL){
            IMetadata appMetadata;
            StreamId_T const streamId_OutAppMeta = mOutAppMeta->getStreamId();
            generateAppMeta(node.appFrame, result3A, deqBuf, appMetadata);
            #if 1
            lock_and_returnMetadata(node.appFrame, streamId_OutAppMeta, appMetadata);
            #else
            m3AStorage.add(node.magicNum, node.appFrame, appMetadata);
            appMetadata = m3AStorage.valueFor(node.magicNum).resultVal;
            if(m3AStorage.isCompleted(node.magicNum)) {
                lock_and_returnMetadata(node.appFrame, streamId_OutAppMeta, appMetadata);
                m3AStorage.removeItem(node.magicNum);
            }
            #endif
        }

        // HAL out Meta Stream
        if (mOutHalMeta != NULL){
            IMetadata inHalMetadata;
            IMetadata outHalMetadata;
            StreamId_T const streamId_OutHalMeta = mOutHalMeta->getStreamId();

            if (mInHalMeta != NULL) lockMetadata(node.appFrame, mInHalMeta->getStreamId(), inHalMetadata);
            generateHalMeta(result3A, deqBuf, resultEIS, inHalMetadata, outHalMetadata);
            lock_and_returnMetadata(node.appFrame, streamId_OutHalMeta, outHalMetadata);
        }

        // HAL in Meta Stream
        if (mInHalMeta != NULL) {
            StreamId_T const streamId_InHalMeta = mInHalMeta->getStreamId();
            returnLockedMetadata(node.appFrame, streamId_InHalMeta);
        }

        // Apply buffers to release
        IStreamBufferSet& rStreamBufferSet  = node.appFrame->getStreamBufferSet();
        rStreamBufferSet.applyRelease(getNodeId());

        // dispatch to next node
        onDispatchFrame(node.appFrame);
        MY_LOGD1("[return OK]: (%d, %d)", node.appFrame->getFrameNo(), node.magicNum);
        android_atomic_dec(&mInFlightRequestCnt);
        ATRACE_INT("P1_request_cnt", android_atomic_acquire_load(&mInFlightRequestCnt));
    } else {
        if (node.reqType == REQ_TYPE_INITIAL) {
            if (IS_OUT(REQ_OUT_RESIZER, node.reqOutSet) ||
                IS_OUT(REQ_OUT_RESIZER_STUFF, node.reqOutSet)) {
                if (mpStreamPool_resizer != NULL) {
                    returnLockedImageBuffer(node.img_resizer, node.halFrame_resizer, mpStreamPool_resizer);
                } else {
                    sp<IImageBuffer> pImgBuf = node.img_resizer;
                    destroyStuffBuffer(pImgBuf);
                }
            }
            if (IS_OUT(REQ_OUT_FULL_PROC, node.reqOutSet) ||
                IS_OUT(REQ_OUT_FULL_PURE, node.reqOutSet) ||
                IS_OUT(REQ_OUT_FULL_STUFF, node.reqOutSet)) {
                if (mpStreamPool_full != NULL) {
                    returnLockedImageBuffer(node.img_full, node.halFrame_full, mpStreamPool_full);
                } else {
                    sp<IImageBuffer> pImgBuf = node.img_full;
                    destroyStuffBuffer(pImgBuf);
                }
            }
        }
    }

    FUNCTION_OUT_P1;
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
P1NodeImp::
onFlushRequestFrame(
    QueNode_T const& node
)
{
    FUNCTION_IN_P1;
    CAM_TRACE_NAME("P1:FlushRequestFrame");

    if (node.appFrame != 0) {

        // Out Image Stream
        Vector<StreamId_T> streamId_Images;
        if (mOutImage_resizer != NULL) {
            streamId_Images.push_back(mOutImage_resizer->getStreamId());
        }
        for(size_t i = 0; i < mvOutImage_full.size(); i++) {
            streamId_Images.push_back(mvOutImage_full[i]->getStreamId());
        }

        for(size_t i = 0; i < streamId_Images.size(); i++) {
            StreamId_T const streamId_Image = streamId_Images[i];
            returnUnlockedImageBuffer(node.appFrame, streamId_Image);
        }


        // APP in Meta Stream
        if (mInAppMeta != NULL) {
            StreamId_T const streamId_InAppMeta = mInAppMeta->getStreamId();
            returnUnlockedMetadata(node.appFrame, streamId_InAppMeta);
        }

        // HAL in Meta Stream
        if (mInHalMeta != NULL) {
            StreamId_T const streamId_InHalMeta = mInHalMeta->getStreamId();
            returnUnlockedMetadata(node.appFrame, streamId_InHalMeta);
        }

        // APP out Meta Stream
        if (mOutAppMeta != NULL) {
            StreamId_T const streamId_OutAppMeta = mOutAppMeta->getStreamId();
            returnUnlockedMetadata(node.appFrame, streamId_OutAppMeta);
        }

        // HAL out Meta Stream
        if (mOutHalMeta != NULL) {
            StreamId_T const streamId_OutHalMeta = mOutHalMeta->getStreamId();
            returnUnlockedMetadata(node.appFrame, streamId_OutHalMeta);
        }

        // Apply buffers to release.
        IStreamBufferSet& rStreamBufferSet  = node.appFrame->getStreamBufferSet();
        rStreamBufferSet.applyRelease(getNodeId());

        // dispatch to next node
        onDispatchFrame(node.appFrame);
        MY_LOGD1("[return flush]: (%d, %d)", node.appFrame->getFrameNo(), node.magicNum);
        android_atomic_dec(&mInFlightRequestCnt);
        ATRACE_INT("P1_request_cnt", android_atomic_acquire_load(&mInFlightRequestCnt));
    }
    else {
        if (node.halFrame_full != 0) {
            returnUnlockedImageBuffer(node.halFrame_full, mpStreamPool_full);
        }
        if (node.halFrame_resizer != 0) {
            returnUnlockedImageBuffer(node.halFrame_resizer, mpStreamPool_resizer);
        }
    }


    FUNCTION_OUT_P1;
}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
P1NodeImp::
onFlushProcessingFrame(
    QueNode_T const& node
)
{
    FUNCTION_IN_P1;
    CAM_TRACE_NAME("P1:FlushProcessingFrame");

    if (node.appFrame != 0) {
        MBOOL success = MFALSE;

        // Out Image Stream
        if (mOutImage_resizer != NULL) {
            if (IS_OUT(REQ_OUT_RESIZER_STUFF, node.reqOutSet)) {
                if (mpStreamPool_resizer != NULL) {
                    returnLockedImageBuffer(node.img_resizer,
                        node.halFrame_resizer, mpStreamPool_resizer);
                } else {
                    sp<IImageBuffer> pImgBuf = node.img_resizer;
                    destroyStuffBuffer(pImgBuf);
                }
            } else if (IS_EXP(REQ_EXP_NOBUF_RRZO, node.reqExpRec)) {
                sp<IImageBuffer> pImgBuf = node.img_resizer;
                destroyStuffBuffer(pImgBuf);
            } else {
                returnLockedImageBuffer(node.appFrame,
                    mOutImage_resizer->getStreamId(),
                    node.img_resizer, success);
            }
        }
        for(size_t i = 0; i < mvOutImage_full.size(); i++) {
            if (IS_OUT(REQ_OUT_FULL_STUFF, node.reqOutSet)) {
                if (mpStreamPool_full != NULL) {
                    returnLockedImageBuffer(node.img_full,
                        node.halFrame_full, mpStreamPool_full);
                } else {
                    sp<IImageBuffer> pImgBuf = node.img_full;
                    destroyStuffBuffer(pImgBuf);
                }
            } else if (IS_EXP(REQ_EXP_NOBUF_IMGO, node.reqExpRec)) {
                sp<IImageBuffer> pImgBuf = node.img_full;
                destroyStuffBuffer(pImgBuf);
            } else {
                returnLockedImageBuffer(node.appFrame,
                    mvOutImage_full[i]->getStreamId(),
                    node.img_full, success);
            }
        }

        // APP in Meta Stream
        if (mInAppMeta != NULL) {
            StreamId_T const streamId_InAppMeta = mInAppMeta->getStreamId();
            returnLockedMetadata(node.appFrame, streamId_InAppMeta, success);
        }

        // HAL in Meta Stream
        if (mInHalMeta != NULL) {
            StreamId_T const streamId_InHalMeta = mInHalMeta->getStreamId();
            returnLockedMetadata(node.appFrame, streamId_InHalMeta, success);
        }

        // APP out Meta Stream
        if (mOutAppMeta != NULL) {
            StreamId_T const streamId_OutAppMeta = mOutAppMeta->getStreamId();
            returnLockedMetadata(node.appFrame, streamId_OutAppMeta, success);
        }

        // HAL out Meta Stream
        if (mOutHalMeta != NULL) {
            StreamId_T const streamId_OutHalMeta  = mOutHalMeta->getStreamId();
            returnLockedMetadata(node.appFrame, streamId_OutHalMeta, success);
        }

        // Apply buffers to release
        IStreamBufferSet& rStreamBufferSet  = node.appFrame->getStreamBufferSet();
        rStreamBufferSet.applyRelease(getNodeId());

        // dispatch to next node
        onDispatchFrame(node.appFrame);
        MY_LOGD1("[return flush]: (%d, %d)", node.appFrame->getFrameNo(), node.magicNum);
        android_atomic_dec(&mInFlightRequestCnt);
        ATRACE_INT("P1_request_cnt", android_atomic_acquire_load(&mInFlightRequestCnt));
    } else {
        if (node.reqType == REQ_TYPE_INITIAL) {
            if (IS_OUT(REQ_OUT_RESIZER, node.reqOutSet) ||
                IS_OUT(REQ_OUT_RESIZER_STUFF, node.reqOutSet)) {
                if (mpStreamPool_resizer != NULL) {
                    returnLockedImageBuffer(node.img_resizer, node.halFrame_resizer, mpStreamPool_resizer);
                } else {
                    sp<IImageBuffer> pImgBuf = node.img_resizer;
                    destroyStuffBuffer(pImgBuf);
                }
            }
            if (IS_OUT(REQ_OUT_FULL_PROC, node.reqOutSet) ||
                IS_OUT(REQ_OUT_FULL_PURE, node.reqOutSet) ||
                IS_OUT(REQ_OUT_FULL_STUFF, node.reqOutSet)) {
                if (mpStreamPool_full != NULL) {
                    returnLockedImageBuffer(node.img_full, node.halFrame_full, mpStreamPool_full);
                } else {
                    sp<IImageBuffer> pImgBuf = node.img_full;
                    destroyStuffBuffer(pImgBuf);
                }
            }
        }
    }

    FUNCTION_OUT_P1;
}

#if 0
/******************************************************************************
 *
 ******************************************************************************/
MVOID
P1NodeImp::
onProcess3AResult(
    MUINT32 magicNum,
    MUINT32 key,
    MUINT32 val
)
{
    MY_LOGD2("%d", magicNum);

    if(magicNum == 0) return;

    m3AStorage.add(magicNum, key, val);
    if(m3AStorage.isCompleted(magicNum)) {
        sp<IPipelineFrame> spFrame = m3AStorage.valueFor(magicNum).spFrame;
        StreamId_T const streamId_OutAppMeta = mOutAppMeta->getStreamId();
        IMetadata appMetadata = m3AStorage.valueFor(magicNum).resultVal;
        lock_and_returnMetadata(spFrame, streamId_OutAppMeta, appMetadata);
        m3AStorage.removeItem(magicNum);

        IStreamBufferSet& rStreamBufferSet  = spFrame->getStreamBufferSet();
        rStreamBufferSet.applyRelease(getNodeId());
    }
}
#endif

/******************************************************************************
 *
 ******************************************************************************/
MVOID
P1NodeImp::
onProcessEIS(
    QueNode_T const &node,
    IMetadata &resultEIS,
    QBufInfo const &deqBuf,
    MINT32 eisMode,
    MUINT8 captureIntent,
    MINT64 exposureTime
)
{
    if(node.appFrame == NULL) {
        MY_LOGW("request not exist (%d)", node.magicNum);
        return;
    }
    if (deqBuf.mvOut.size() == 0) {
        MY_LOGW("DeQ Buf is empty, result count (%d)", resultEIS.count());
        return;
    }
    #if SUPPORT_EIS
    if(mpEIS == NULL) {
        MY_LOGW("EIS not ready (%d)", node.magicNum);
        return;
    }

    //One-shot only for a session
    if (EIS_MODE_IS_CALIBRATION_ENABLED(eisMode)) {//Calibration Mode

        EIS_HAL_CONFIG_DATA  config;
        config.sensorType = IHalSensorList::get()->queryType(getOpenId());
        config.configSce = EIS_SCE_EIS_PLUS;
        config.cropX = 0;
        config.cropY = 0;
        config.crzOutH = 0;
        config.crzOutW = 0;
        mpEIS->ConfigCalibration(EIS_PASS_2, config);
    }

    mpEIS->DoEis(EIS_PASS_1, deqBuf);
    MBOOL isLastSkipped = CHECK_LAST_FRAME_SKIPPED(mLastSofIdx, node.sofIdx);
    MUINT32 X_INT, Y_INT, X_FLOAT, Y_FLOAT, WIDTH, HEIGHT, ISFROMRRZ;
    MINT32 GMV_X, GMV_Y, MVtoCenterX, MVtoCenterY,iExpTime,ihwTS,ilwTS;
    MUINT32 ConfX,ConfY;
    const MINT64 aTimestamp = deqBuf.mvOut[0].mMetaData.mTimeStamp;

    mpEIS->GetEisResult(X_INT, X_FLOAT, Y_INT, Y_FLOAT, WIDTH, HEIGHT, MVtoCenterX, MVtoCenterY, ISFROMRRZ);
    mpEIS->GetEisGmv(GMV_X, GMV_Y, &ConfX, &ConfY);

    iExpTime = exposureTime/1000;//(ns to us) << frame duration
    ihwTS = (aTimestamp >> 32)&0xFFFFFFFF; //High word
    ilwTS = (aTimestamp & 0xFFFFFFFF);     //Low word
    IMetadata::IEntry entry(MTK_EIS_REGION);
    entry.push_back(X_INT, Type2Type< MINT32 >());
    entry.push_back(X_FLOAT, Type2Type< MINT32 >());
    entry.push_back(Y_INT, Type2Type< MINT32 >());
    entry.push_back(Y_FLOAT, Type2Type< MINT32 >());
    entry.push_back(WIDTH, Type2Type< MINT32 >());
    entry.push_back(HEIGHT, Type2Type< MINT32 >());
    entry.push_back(MVtoCenterX, Type2Type< MINT32 >());
    entry.push_back(MVtoCenterY, Type2Type< MINT32 >());
    entry.push_back(ISFROMRRZ, Type2Type< MINT32 >());
    entry.push_back(GMV_X, Type2Type< MINT32 >());
    entry.push_back(GMV_Y, Type2Type< MINT32 >());
    entry.push_back(ConfX, Type2Type< MINT32 >());
    entry.push_back(ConfY, Type2Type< MINT32 >());
    entry.push_back(iExpTime, Type2Type< MINT32 >());
    entry.push_back(ihwTS, Type2Type< MINT32 >());
    entry.push_back(ilwTS, Type2Type< MINT32 >());
    resultEIS.update(MTK_EIS_REGION, entry);
    MY_LOGD1("%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
              X_INT, X_FLOAT, Y_INT, Y_FLOAT, WIDTH, HEIGHT, MVtoCenterX, MVtoCenterY,
              GMV_X, GMV_Y, ConfX, ConfY, isLastSkipped);

    if (EIS_MODE_IS_CALIBRATION_ENABLED(eisMode)) {//Calibration Mode
        EIS_HAL_CONFIG_DATA  config;
        config.cropX = 0;
        config.cropY = 0;
        config.crzOutW = 0;
        config.crzOutH = 0;

        MY_LOGD1("[EisHal] captureIntent: %d, eisMode: %d \n", captureIntent, eisMode);

        //GIS calibration is in preview ONLY!!
        if (captureIntent ==  MTK_CONTROL_CAPTURE_INTENT_PREVIEW)
        {
            mpEIS->DoCalibration(EIS_PASS_2, &config, aTimestamp, exposureTime);
        }
    }

    #endif
}


/******************************************************************************
 *
 ******************************************************************************/
void
P1NodeImp::
doNotifyCb(
    MINT32  _msgType,
    MINTPTR _ext1,
    MINTPTR _ext2,
    MINTPTR _ext3
)
{
    FUNCTION_IN_P1;
    MY_LOGD1("P1 doNotifyCb(%d) %d %d", _msgType, _ext1, _ext2);
    switch(_msgType)
    {
        case IHal3ACb::eID_NOTIFY_3APROC_FINISH:
            if (_ext3 == 0) {
                MY_LOGE("CapParam NULL (%d) %d %d", _msgType, _ext1, _ext2);
            } else {
                RequestSet_T set = *(RequestSet_T*)(_ext1);
                CapParam_T param = *(CapParam_T*)(_ext3);
                onProcessFrame(MFALSE, &set, (MUINT32)_ext2, &param);
            }
            break;
        case IHal3ACb::eID_NOTIFY_CURR_RESULT:
            //onProcess3AResult((MUINT32)_ext1,(MUINT32)_ext2, (MUINT32)_ext3); //magic, key, val
            break;
        default:
            break;
    }
    FUNCTION_OUT_P1;
}


/******************************************************************************
 *
 ******************************************************************************/
void
P1NodeImp::
doNotifyDropframe(MUINT magicNum, void* cookie)
{
   if (cookie == NULL) {
       MY_LOGE("return cookie is NULL");
       return;
   }

   Mutex::Autolock _l(reinterpret_cast<P1NodeImp*>(cookie)->mDropQueueLock);
   reinterpret_cast<P1NodeImp*>(cookie)->mDropQueue.push_back(magicNum);
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
createStuffBuffer(sp<IImageBuffer> & imageBuffer,
    sp<IImageStreamInfo> const& streamInfo,
    NSCam::MSize::value_type const changeHeight)
{
    MUINT32 stride = streamInfo->getBufPlanes()[0].rowStrideInBytes;
    MSize size = streamInfo->getImgSize();
    // change the height while changeHeight > 0
    if (changeHeight > 0) {
        size.h = changeHeight;
    }
    //
    return createStuffBuffer(imageBuffer,
        streamInfo->getStreamName(), streamInfo->getImgFormat(), size, stride);
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
createStuffBuffer(sp<IImageBuffer> & imageBuffer,
    char const* szName, MINT32 format, MSize size, MUINT32 stride)
{
    #if (SUPPORT_STORE_STUFF_BUFFER)
    return mStuffBufMgr.acquireStoreBuffer(imageBuffer,
                                            szName, format, size, stride);
    #endif
    Mutex::Autolock _l(mStuffBufferLock);
    imageBuffer = NULL;
    // add information to buffer name
    android::String8 imgBufName = android::String8(szName);
    char str[64] = {0};
    snprintf(str, sizeof(str), ":Size%dx%d:Stride%d:Id%d",
        size.w, size.h, stride, (int)(mvStuffBufferInfo.size()));
    imgBufName += str;
    // create buffer
    MINT32 bufBoundaryInBytes[3] = {0, 0, 0};
    MUINT32 bufStridesInBytes[3] = {stride, 0, 0};
    IImageBufferAllocator::ImgParam imgParam =
        IImageBufferAllocator::ImgParam((EImageFormat)format,
            size, bufStridesInBytes, bufBoundaryInBytes, 1);
    sp<IIonImageBufferHeap> pHeap =
        IIonImageBufferHeap::create(imgBufName.string(), imgParam);
    if (pHeap == NULL) {
        MY_LOGE("[%s] Stuff ImageBufferHeap create fail", imgBufName.string());
        return BAD_VALUE;
    }
    sp<IImageBuffer> pImgBuf = pHeap->createImageBuffer();
    if (pImgBuf == NULL) {
        MY_LOGE("[%s] Stuff ImageBuffer create fail", imgBufName.string());
        return BAD_VALUE;
    }
    // lock buffer
    MUINT const usage = (GRALLOC_USAGE_SW_READ_OFTEN |
                        GRALLOC_USAGE_HW_CAMERA_READ |
                        GRALLOC_USAGE_HW_CAMERA_WRITE);
    if (!(pImgBuf->lockBuf(imgBufName.string(), usage))) {
        MY_LOGE("[%s] Stuff ImageBuffer lock fail", imgBufName.string());
        return BAD_VALUE;
    }
    mvStuffBufferInfo.add(pImgBuf, imgBufName);
    imageBuffer = pImgBuf;
    //
    MY_LOGD1("Create Stuff Buffer (%s) OK", imgBufName.string());
    //
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
destroyStuffBuffer(sp<IImageBuffer> & imageBuffer)
{
    #if (SUPPORT_STORE_STUFF_BUFFER)
    return mStuffBufMgr.releaseStoreBuffer(imageBuffer);
    #endif
    Mutex::Autolock _l(mStuffBufferLock);
    if (imageBuffer == NULL) {
        MY_LOGW("Stuff ImageBuffer not exist");
        return BAD_VALUE;
    }
    ssize_t index = mvStuffBufferInfo.indexOfKey(imageBuffer);
    imageBuffer->unlockBuf(mvStuffBufferInfo.valueAt(index).string());
    // destroy buffer
    mvStuffBufferInfo.removeItemsAt(index);
    //imageBuffer = NULL;
    //
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
hardwareOps_start()
{
#if SUPPORT_ISP
    FUNCTION_IN_P1;
    CAM_TRACE_NAME("P1:hardwareOps_start");

    Mutex::Autolock _l(mHardwareLock);

    setActive(MTRUE);
    setInit(MTRUE);
    mLastSofIdx = P1SOFIDX_NULL_VAL;

    #if SUPPORT_EIS
    mpEIS = NULL;
    #endif
    #if SUPPORT_VHDR
    mpVhdr = NULL;
    #endif
    mConfigPort = CONFIG_PORT_NONE;

    mDequeThreadProfile.reset();
    //mImageStorage.init(mLogLevel);

    //
    CAM_TRACE_BEGIN("P1:CamIO init");
    #ifdef USING_MTK_LDVT
    mpCamIO = INormalPipe::createInstance(getOpenId(), "iopipeUseTM");
    #else
    mpCamIO = INormalPipe::createInstance(getOpenId(), getNodeName());
    #endif
    MY_LOGD2("mpCamIO->init +++");
    if(!mpCamIO || !mpCamIO->init())
    {
        MY_LOGE("hardware init fail");
        return DEAD_OBJECT;
    }
    MY_LOGD2("mpCamIO->init ---");
    CAM_TRACE_END();

#if SUPPORT_VHDR
    sp<IImageBuffer> pVhdrBuf = NULL;
    if (mEnableLCSO) {
        CAM_TRACE_BEGIN("P1:vhdr config");

        mpVhdr = VHdrHal::CreateInstance(LOG_TAG, getOpenId());
        if( mpVhdr->Init(mSensorParams.vhdrMode) == VHDR_RETURN_NO_ERROR)
        {
            mpVhdr->GetBufLCSO(pVhdrBuf);
        }
        else
        {
            //mpVhdr->DestroyInstance(LOG_TAG); // instance always exist until process kill
            mpVhdr = NULL;
        }
        CAM_TRACE_END();
    }
#endif

#if SUPPORT_EIS
    sp<IImageBuffer> pEISOBuf = NULL;
    EIS_HAL_CONFIG_DATA  config;
    if (mEnableEISO) {
        CAM_TRACE_BEGIN("P1:eis config");
        mpEIS = EisHal::CreateInstance(LOG_TAG, getOpenId());
        mpEIS->Init();
        config.sensorType = IHalSensorList::get()->queryType(getOpenId());
        //config.memAlignment //[AWARE] may need to modify by platform
#if SUPPORT_VHDR
    if (! (mpVhdr && mpVhdr->isVhdrOn_IorZ()) )
#else
        if (1) //if vHDR is disable
#endif
        {
            config.configSce = EIS_SCE_EIS_PLUS;
        }else
        {
            config.configSce = EIS_SCE_EIS;
        }
        CAM_TRACE_END();

        // [TODO] get pEISOBuf from EIS
        mpEIS->GetBufEis(pEISOBuf);
    }
#endif

    //
    IHalSensor::ConfigParam sensorCfg =
    {
        (MUINT)getOpenId(),                 /* index            */
        mSensorParams.size,                 /* crop */
        mSensorParams.mode,                 /* scenarioId       */
        0,                                  /* isBypassScenario */
        1,                                  /* isContinuous     */
#if SUPPORT_VHDR
        mSensorParams.vhdrMode,             /* vHDROn mode          */
#else
        MFALSE,                             /* vHDROn mode          */
#endif
        #ifdef USING_MTK_LDVT
        1,
        #else
        mSensorParams.fps,                  /* framerate        */
        #endif
        0,                                  /* two pixel on     */
        0,                                  /* debugmode        */
    };

    vector<IHalSensor::ConfigParam> vSensorCfg;
    vSensorCfg.push_back(sensorCfg);

    //
    vector<portInfo> vPortInfo;
    if (mvOutImage_full.size() != 0) //[TODO] || (mOutImage_combo != NULL)
    {
        portInfo OutPort(
                PORT_IMGO,
                (EImageFormat)mvOutImage_full[0]->getImgFormat(),
                mvOutImage_full[0]->getImgSize(),
                MRect(MPoint(0,0), mSensorParams.size),
                mvOutImage_full[0]->getBufPlanes().itemAt(0).rowStrideInBytes,
                0, //pPortCfg->mStrideInByte[1],
                0, //pPortCfg->mStrideInByte[2],
                0, // pureraw
                MTRUE);              //packed

        vPortInfo.push_back(OutPort);
        mConfigPort |= CONFIG_PORT_IMGO;
    }


    if (mOutImage_resizer != NULL)
    {
        portInfo OutPort(
                PORT_RRZO,
                (EImageFormat)mOutImage_resizer->getImgFormat(),
                mOutImage_resizer->getImgSize(),
                MRect(MPoint(0,0), mSensorParams.size),
                mOutImage_resizer->getBufPlanes().itemAt(0).rowStrideInBytes,
                0, //pPortCfg->mStrideInByte[1],
                0, //pPortCfg->mStrideInByte[2],
                0, // pureraw
                MTRUE);              //packed

        vPortInfo.push_back(OutPort);
        mConfigPort |= CONFIG_PORT_RRZO;
    }

    #if SUPPORT_EIS
    if (pEISOBuf != NULL)
    {
        portInfo OutPort(
                PORT_EISO,
                (EImageFormat)pEISOBuf->getImgFormat(),
                pEISOBuf->getImgSize(),
                MRect(MPoint(0,0),  pEISOBuf->getImgSize()),
                pEISOBuf->getBufStridesInBytes(0),
                0, //pPortCfg->mStrideInByte[1],
                0, //pPortCfg->mStrideInByte[2],
                0, // pureraw
                MTRUE);              //packed

        vPortInfo.push_back(OutPort);
        mConfigPort |= CONFIG_PORT_EISO;
    }
    #endif

    #if SUPPORT_VHDR
    if (pVhdrBuf != NULL)
    {
        portInfo OutPort(
                PORT_LCSO,
                eImgFmt_BLOB,
                pVhdrBuf->getImgSize(),
                MRect(MPoint(0,0),  pVhdrBuf->getImgSize()),
                pVhdrBuf->getBufStridesInBytes(0),
                0, //pPortCfg->mStrideInByte[1],
                0, //pPortCfg->mStrideInByte[2],
                0, // pureraw
                MTRUE);              //packed

        vPortInfo.push_back(OutPort);
        mConfigPort |= CONFIG_PORT_LCSO;
    }
    #endif


    MBOOL bDynamicRawType = MTRUE;  // true:[ON] ; false:[OFF]
    QInitParam halCamIOinitParam(
               0,                           /*sensor test pattern */
               10,                          /* bit depth*/
               vSensorCfg,
               vPortInfo,
               bDynamicRawType);
    if ((mRawOption & (1 << EPipe_PROCESSED_RAW)) > 0) {
        halCamIOinitParam.m_bOffBin = MTRUE;
    } else {
        halCamIOinitParam.m_bOffBin = MFALSE;
    }
    halCamIOinitParam.m_DropCB = doNotifyDropframe;
    halCamIOinitParam.m_returnCookie = this;

    // enable frame sync
    if(mEnableFrameSync){
      MY_LOGI("P1 node(%d) is in synchroized mode", getOpenId());
      halCamIOinitParam.m_bN3D = MTRUE;
    }else{
      halCamIOinitParam.m_bN3D = MFALSE;
    }

    //
    mIsBinEn = false;
    CAM_TRACE_BEGIN("P1:CamIO configPipe");
    MY_LOGD2("mpCamIO->configPipe +++");
    if(!mpCamIO->configPipe(halCamIOinitParam, mBurstNum))
    {
        MY_LOGE("hardware config pipe fail");
        return BAD_VALUE;
    } else {
        MSize binInfoSize = mSensorParams.size;
        if (mpCamIO->sendCommand(ENPipeCmd_GET_BIN_INFO,
            (MINTPTR)&binInfoSize.w, (MINTPTR)&binInfoSize.h, (MINTPTR)NULL)) {
            if (binInfoSize.w < mSensorParams.size.w ||
                binInfoSize.h < mSensorParams.size.h) {
                mIsBinEn = true;
            }
        }
        MY_LOGI("BinSize:%dx%d BinEn:%d",
            binInfoSize.w, binInfoSize.h, mIsBinEn);
    }
    MY_LOGD2("mpCamIO->configPipe ---");
    CAM_TRACE_END();

#if SUPPORT_3A
    CAM_TRACE_BEGIN("P1:3A create and setSensorMode");
    mp3A = IHal3AImp_T::createInstance(IHal3A::E_Camera_3, getOpenId(), getNodeName());
    mp3A->setSensorMode(mSensorParams.mode);
    CAM_TRACE_END();
#endif

    #if SUPPORT_EIS
    if ((mpEIS != NULL) && (mpEIS->GetEisSupportInfo(getOpenId())))
    {
        mpEIS->ConfigEis(EIS_PASS_1, config);
    }
    #endif

    #if SUPPORT_VHDR
    if(mpVhdr)
    {
        VHDR_HAL_CONFIG_DATA vhdrConfig;
        vhdrConfig.cameraVer = VHDR_CAMERA_VER_3;
        mpVhdr->ConfigVHdr(vhdrConfig);
    }
    #endif


    #if SUPPORT_3A
    CAM_TRACE_BEGIN("P1:3A config");
    if (mp3A) {
        mp3A->attachCb(IHal3ACb::eID_NOTIFY_3APROC_FINISH, this);
        mp3A->attachCb(IHal3ACb::eID_NOTIFY_CURR_RESULT, this);
        mp3A->config((MINT32)(mBurstNum > 1) ? mBurstNum : 1);
        //m3AProcessedDepth = mp3A->getCapacity();
    }
    CAM_TRACE_END();
    #endif

    #if SUPPORT_3A
    CAM_TRACE_BEGIN("P1:3A start");
    if (mp3A) {
        mp3A->start();
    }
    CAM_TRACE_END();
    #endif


    //register 3 real frames and 3 dummy frames
    //[TODO] in case that delay frame is above 3 but memeory has only 3, pending aquirefromPool
    CAM_TRACE_BEGIN("P1:create node");
    {
        QueJob_T job(mBurstNum);
        for (MUINT8 n = 0; n < mBurstNum; n++) {
            createNode(NULL, &job, &mProcessingQueue, &mProcessingQueueLock,
                                 &mControls3AList, &mControls3AListLock);
        }
        hardwareOps_enque(mProcessingQueue.editItemAt(mProcessingQueue.size()-1));
        // Due to pipeline latency, delay frame should be above 3
        // if delay frame is more than 3, add node to mProcessingQueue here.
        for (int i = 0; i < mDelayframe - mNumHardwareBuffer; i++) {
            for (MUINT8 n = 0; n < mBurstNum; n++) {
                createNode(&mControls3AList, &mControls3AListLock);
            }
        }
        mQueueJob.mMaxNum = mBurstNum;
        mQueueJob.mSet.setCapacity(mBurstNum);
        mQueueJob.mSet.clear();
    }
    CAM_TRACE_END();
    //
    CAM_TRACE_BEGIN("P1:CamIO start");
    #if 1
    MY_LOGD2("mpCamIO->start +++");
    if(!mpCamIO->start()) {
        MY_LOGE("hardware start fail");
        return BAD_VALUE;
    }
    MY_LOGD2("mpCamIO->start ---");
    #endif
    CAM_TRACE_END();


    {
        Mutex::Autolock _l(mThreadLock);
        setReady(MTRUE);
        mThreadCond.broadcast();
    }

    FUNCTION_OUT_P1;

    return OK;
#else
    return OK;
#endif

}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
P1NodeImp::
generateAppMeta(sp<IPipelineFrame> const &request, MetaSet_T const &result3A,
                QBufInfo const &deqBuf, IMetadata &appMetadata)
{
    //[3A/Flash/sensor section]
    appMetadata = result3A.appMeta;

    //[request section]
    // android.request.frameCount
    {
        IMetadata::IEntry entry(MTK_REQUEST_FRAME_COUNT);
        entry.push_back( request->getFrameNo(), Type2Type< MINT32 >());
        appMetadata.update(MTK_REQUEST_FRAME_COUNT, entry);
    }
    // android.request.metadataMode
    {
        IMetadata::IEntry entry(MTK_REQUEST_METADATA_MODE);
        entry.push_back(MTK_REQUEST_METADATA_MODE_FULL, Type2Type< MUINT8 >());
        appMetadata.update(MTK_REQUEST_METADATA_MODE, entry);
    }

    //[sensor section]
    // android.sensor.timestamp
    {
        MINT64 frame_duration = 0; //IMetadata::IEntry entry(MTK_SENSOR_FRAME_DURATION);
                                   //should get from control.
        MINT64 timestamp = deqBuf.mvOut[0].mMetaData.mTimeStamp - frame_duration;
        IMetadata::IEntry entry(MTK_SENSOR_TIMESTAMP);
        entry.push_back(timestamp, Type2Type< MINT64 >());
        appMetadata.update(MTK_SENSOR_TIMESTAMP, entry);
    }

    //[sensor section]
    // android.sensor.rollingshutterskew
    // [TODO] should query from sensor
    {
        IMetadata::IEntry entry(MTK_SENSOR_ROLLING_SHUTTER_SKEW);
        entry.push_back(33000000, Type2Type< MINT64 >());
        appMetadata.update(MTK_SENSOR_ROLLING_SHUTTER_SKEW, entry);
    }


}


/******************************************************************************
 *
 ******************************************************************************/
MVOID
P1NodeImp::
generateHalMeta(MetaSet_T const &result3A, QBufInfo const &deqBuf, IMetadata const &resultEIS, IMetadata const &inHalMetadata, IMetadata &halMetadata)
{
    if (deqBuf.mvOut.size() == 0) {
        MY_LOGE("deqBuf is empty");
        return;
    }

    //3a tuning
    halMetadata = result3A.halMeta;

    //eis
    halMetadata += resultEIS;

    // in hal meta
    halMetadata += inHalMetadata;

    {
        IMetadata::IEntry entry(MTK_P1NODE_SENSOR_MODE);
        entry.push_back(mSensorParams.mode, Type2Type< MINT32 >());
        halMetadata.update(MTK_P1NODE_SENSOR_MODE, entry);
    }

    //rrzo
    for (size_t i = 0; i < deqBuf.mvOut.size(); i++) {
        if (deqBuf.mvOut[i].mPortID.index == PORT_RRZO.index) {
            //crop region
            {
                MRect crop = deqBuf.mvOut[i].mMetaData.mCrop_s;
                if (mIsBinEn) {
                    BIN_REVERT(crop.p.x);
                    BIN_REVERT(crop.p.y);
                    BIN_REVERT(crop.s.w);
                    BIN_REVERT(crop.s.h);
                }
                IMetadata::IEntry entry(MTK_P1NODE_SCALAR_CROP_REGION);
                entry.push_back(crop, Type2Type< MRect >());
                halMetadata.update(MTK_P1NODE_SCALAR_CROP_REGION, entry);
            }
            //
            {
                IMetadata::IEntry entry(MTK_P1NODE_DMA_CROP_REGION);
                entry.push_back(deqBuf.mvOut[i].mMetaData.mCrop_d, Type2Type< MRect >());
                halMetadata.update(MTK_P1NODE_DMA_CROP_REGION, entry);
            }
            //
            {
                IMetadata::IEntry entry(MTK_P1NODE_RESIZER_SIZE);
                entry.push_back(deqBuf.mvOut[i].mMetaData.mDstSize, Type2Type< MSize >());
                halMetadata.update(MTK_P1NODE_RESIZER_SIZE, entry);
            }
            /*
            MY_LOGD("[CropInfo] CropS(%d, %d, %dx%d) "
                "CropD(%d, %d, %dx%d) DstSize(%dx%d)",
                deqBuf.mvOut[i].mMetaData.mCrop_s.leftTop().x,
                deqBuf.mvOut[i].mMetaData.mCrop_s.leftTop().y,
                deqBuf.mvOut[i].mMetaData.mCrop_s.size().w,
                deqBuf.mvOut[i].mMetaData.mCrop_s.size().h,
                deqBuf.mvOut[i].mMetaData.mCrop_d.leftTop().x,
                deqBuf.mvOut[i].mMetaData.mCrop_d.leftTop().y,
                deqBuf.mvOut[i].mMetaData.mCrop_d.size().w,
                deqBuf.mvOut[i].mMetaData.mCrop_d.size().h,
                deqBuf.mvOut[i].mMetaData.mDstSize.w,
                deqBuf.mvOut[i].mMetaData.mDstSize.h);
            */
        } else {
            continue;
        }
    }

    #if SUPPORT_VHDR
    for (size_t i = 0; i < deqBuf.mvOut.size(); i++) {
        if (deqBuf.mvOut[i].mPortID.index == PORT_LCSO.index && mpVhdr) {
            mpVhdr->PushMetadata(halMetadata, deqBuf.mvOut[i].mBuffer);
            break;
        }
    }
    #endif
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
setupNodeBufInfo(
    QueNode_T &node,
    QBufInfo &info
)
{
    FUNCTION_IN_P1;
#if SUPPORT_ISP
    MUINT32 out = 0;
    //
    sp<IImageStreamBufferPoolT> pBufPool = NULL;
    sp<IImageStreamInfo> pImgStreamInfo = NULL;
    sp<IImageStreamBuffer> pImgStreamBuf = NULL;
    sp<IImageBuffer> pImgBuf = NULL;
    //
    NSCam::NSIoPipe::PortID portID = NSCam::NSIoPipe::PortID();
    MSize dstSize = MSize(0, 0);
    MRect cropRect = MRect(MPoint(0, 0), MSize(0, 0));
    MUINT32 rawOutFmt = 0;
    //
    sp<IImageStreamBuffer> * ppOutImgStreamBuf = NULL;
    sp<IImageBuffer> * ppOutImgBuf = NULL;
    //
    if ((node.reqType == REQ_TYPE_UNKNOWN) ||
        (node.reqType == REQ_TYPE_REDO)) {
        MY_LOGW("(%d) no need to setup type(%d)", node.magicNum, node.reqType);
        return BAD_VALUE;
    }
    //
    for (out = 0; out < REQ_OUT_MAX; out++) {
        if (!(IS_OUT(out, node.reqOutSet))) {
            continue;
        }
        pBufPool = NULL;
        pImgStreamInfo = NULL;
        pImgStreamBuf = NULL;
        pImgBuf = NULL;
        switch (out) {
            case REQ_OUT_RESIZER:
            case REQ_OUT_RESIZER_STUFF:
                if (mOutImage_resizer != NULL) {
                    pImgStreamInfo = mOutImage_resizer;
                }
                if (pImgStreamInfo == NULL){
                    MY_LOGE("(%d) no image stream info - resizer",
                        node.magicNum);
                    return BAD_VALUE;
                }
                portID = PORT_RRZO;
                dstSize = node.dstSize_resizer;
                cropRect = node.cropRect_resizer;
                rawOutFmt = (EPipe_PROCESSED_RAW);
                            //(MUINT32)((node.reqOutSet & REQ_OUT_FULL_PURE) ?
                            //(EPipe_PROCESSED_RAW) : (EPipe_PURE_RAW));
                ppOutImgStreamBuf = &(node.halFrame_resizer);
                ppOutImgBuf = &(node.img_resizer);
                if (node.reqType == REQ_TYPE_INITIAL ||
                    out == REQ_OUT_RESIZER_STUFF) {
                    if (mpStreamPool_resizer != NULL) {
                        pBufPool = mpStreamPool_resizer;
                    } else { // use stuff buffer with height:1
                        dstSize.h = P1_STUFF_BUF_HEIGHT(MTRUE, mConfigPort);
                    }
                    cropRect.s = dstSize;
                }
                break;

            case REQ_OUT_FULL_PROC:
            case REQ_OUT_FULL_PURE:
            case REQ_OUT_FULL_COMBO:
                // [TODO] for combo raw
                // [TODO] need to check some value of stream info before use
            case REQ_OUT_FULL_STUFF:
                if  (mvOutImage_full.size() > 0) {
                    MUINT32 cnt = 0;
                    for (cnt = 0; cnt < mvOutImage_full.size(); cnt++) {
                        if (mvOutImage_full[cnt] != NULL) {
                            pImgStreamInfo = mvOutImage_full[cnt];
                            break;
                        }
                    }
                }
                if (pImgStreamInfo == NULL){
                    MY_LOGE("(%d) no image stream info - full",
                        node.magicNum);
                    return BAD_VALUE;
                }
                portID = PORT_IMGO;
                dstSize = node.dstSize_full;
                cropRect = node.cropRect_full;
                rawOutFmt = (MUINT32)((out == REQ_OUT_FULL_PROC) ?
                            (EPipe_PROCESSED_RAW) : (EPipe_PURE_RAW));
                ppOutImgStreamBuf = &(node.halFrame_full);
                ppOutImgBuf = &(node.img_full);
                if (node.reqType == REQ_TYPE_INITIAL ||
                    out == REQ_OUT_FULL_STUFF) {
                    if (mpStreamPool_full != NULL) {
                        pBufPool = mpStreamPool_full;
                    } else { // use stuff buffer with height:1
                        dstSize.h = P1_STUFF_BUF_HEIGHT(MFALSE, mConfigPort);
                    }
                    cropRect.s = dstSize;
                }
                break;

            case REQ_OUT_MAX:
            default:
                continue;
        };
        //
        if (node.reqType == REQ_TYPE_INITIAL ||
            (out == REQ_OUT_FULL_STUFF || out == REQ_OUT_RESIZER_STUFF)) {
            MERROR err = OK;
            if (pBufPool != NULL) {
                err = pBufPool->acquireFromPool(
                    getNodeName(), pImgStreamBuf, ::s2ns(300));
                if (err != OK) {
                    if(err == TIMED_OUT) {
                        MY_LOGW("(%d) acquire timeout", node.magicNum);
                    } else {
                        MY_LOGE("(%d) acquire failed", node.magicNum);
                    }
                    pBufPool->dumpPool();
                    return BAD_VALUE;
                }
                lockImageBuffer(pImgStreamBuf, pImgBuf);
            } else if (pImgStreamInfo != NULL) {
                err = createStuffBuffer(pImgBuf, pImgStreamInfo, dstSize.h);
                if (err != OK) {
                    MY_LOGE("(%d) create stuff buffer with stream info failed",
                        node.magicNum);
                    return BAD_VALUE;
                }
            } else {
                char const* szName = "Hal:Image:P1:INITraw";
                MINT format = pImgStreamInfo->getImgFormat();
                MUINT32 stride = pImgStreamInfo->
                    getBufPlanes().itemAt(0).rowStrideInBytes;
                err = createStuffBuffer(pImgBuf,
                        szName, format, dstSize, stride);
                if (err != OK) {
                    MY_LOGE("(%d) create stuff buffer failed", node.magicNum);
                    return BAD_VALUE;
                }
            }
            *(ppOutImgStreamBuf) = pImgStreamBuf;
            *(ppOutImgBuf) = pImgBuf;
        } else  { // REQ_TYPE_NORMAL
            if (node.appFrame == NULL) {
                MY_LOGE("(%d) lock image buffer with NULL pipeline frame",
                    node.magicNum);
                return BAD_VALUE;
            }
            if (OK != lockImageBuffer(node.appFrame,
                    pImgStreamInfo->getStreamId(), pImgStreamBuf, pImgBuf)) {
                #if 1 // keep en-queue/de-queue processing
                MY_LOGI("(%d) lockImageBuffer cannot get StreamId=0x%X",
                    node.magicNum, pImgStreamInfo->getStreamId());
                MERROR err = OK;
                dstSize.h = P1_STUFF_BUF_HEIGHT(
                    (out == REQ_OUT_RESIZER ? MTRUE : MFALSE), mConfigPort);
                cropRect.s.h = dstSize.h;
                err = createStuffBuffer(pImgBuf, pImgStreamInfo, dstSize.h);
                if (err != OK) {
                    MY_LOGE("(%d) create stuff buffer with stream info failed",
                        node.magicNum);
                    return BAD_VALUE;
                }
                *(ppOutImgStreamBuf) = NULL;
                *(ppOutImgBuf) = pImgBuf;
                if (out == REQ_OUT_RESIZER) {
                    node.reqExpRec |= REQ_REC(REQ_EXP_NOBUF_RRZO);
                } else {
                    node.reqExpRec |= REQ_REC(REQ_EXP_NOBUF_IMGO);
                }
                #else
                MY_LOGE("(%d) lockImageBuffer failed on StreamId=0x%X",
                    node.magicNum, pImgStreamInfo->getStreamId());
                return BAD_VALUE;
                #endif
            } else {
                *(ppOutImgStreamBuf) = pImgStreamBuf;
                *(ppOutImgBuf) = pImgBuf;
            }
        }
        //
        if (pImgBuf == NULL) {
            MY_LOGE("(%d) Can not get image buffer", node.magicNum);
            return BAD_VALUE;
        }
        //
        MY_LOGD1("[ %d @%d](%d:%d)(%s-%d-%d) "
            "[Buf](%dx%d-%d-%d P:%p V:%p) "
            "[Crop](%d,%d-%dx%d)(%dx%d)",
            node.magicNum, node.sofIdx,
            ((node.appFrame == NULL) ?
            ((MUINT32)0xFFFFFFFF) : (node.appFrame->getFrameNo())),
            ((node.appFrame == NULL) ?
            ((MUINT32)0xFFFFFFFF) : (node.appFrame->getRequestNo())),
            ((portID.index == PORT_RRZO.index) ? "Resizer" : "FullImg"),
            node.reqType, out,
            pImgBuf->getImgSize().w, pImgBuf->getImgSize().h,
            pImgBuf->getBufStridesInBytes(0), pImgBuf->getBufSizeInBytes(0),
            (void*)pImgBuf->getBufPA(0), (void*)pImgBuf->getBufVA(0),
            cropRect.p.x, cropRect.p.y, cropRect.s.w, cropRect.s.h,
            dstSize.w, dstSize.h);
        NSCam::NSIoPipe::NSCamIOPipe::BufInfo rBufInfo(
            portID,
            pImgBuf.get(),
            dstSize,
            cropRect,
            node.magicNum,
            node.sofIdx,
            rawOutFmt);
        info.mvOut.push_back(rBufInfo);
    }
    //
    {

        MSize dstSizeNone = MSize(0, 0);
        MRect cropRectNone = MRect(MPoint(0, 0), MSize(0, 0));
        // EISO
        #if SUPPORT_EIS
        if (IS_PORT(CONFIG_PORT_EISO, mConfigPort)) {
            sp<IImageBuffer> pImgBuf = NULL;
            // [TODO] get pImgBuf from EIS
            mpEIS->GetBufEis(pImgBuf);
            MY_LOGD1("GetBufEis: %p ",pImgBuf->getBufVA(0));
            node.buffer_eiso = pImgBuf;
            NSCam::NSIoPipe::NSCamIOPipe::BufInfo rBufInfo(
                PORT_EISO,
                pImgBuf.get(),
                pImgBuf->getImgSize(),
                MRect(MPoint(0, 0), pImgBuf->getImgSize()),
                node.magicNum,
                node.sofIdx);
            info.mvOut.push_back(rBufInfo);
        }
        #endif
        // LCSO
        #if SUPPORT_VHDR
        if (IS_PORT(CONFIG_PORT_LCSO, mConfigPort)) {
            sp<IImageBuffer> pImgBuf = NULL;
            // [TODO] get pImgBuf from LCS
            mpVhdr->GetBufLCSO(pImgBuf);
            node.buffer_lcso = pImgBuf;
            NSCam::NSIoPipe::NSCamIOPipe::BufInfo rBufInfo(
                PORT_LCSO,
                pImgBuf.get(),
                pImgBuf->getImgSize(),
                MRect(MPoint(0, 0), pImgBuf->getImgSize()),
                node.magicNum,
                node.sofIdx);
            info.mvOut.push_back(rBufInfo);
        }
        #endif
    }
    //#endif
#endif
    FUNCTION_OUT_P1;
    return OK;
}



/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
hardwareOps_enque(
    QueJob_T &job,
    MBOOL toPush
)
{
    FUNCTION_IN_P1;
    CAM_TRACE_NAME("P1:enque");

    if (!getActive()) {
        return BAD_VALUE;
    }
#if SUPPORT_ISP
    QBufInfo enBuf;
    for (MUINT8 i = 0; i < job.mSet.size(); i++) {
        MY_LOGD2("job(%d)(%d/%d)", job.mFirstMagicNum, i, job.mSet.size());
        if (OK != setupNodeBufInfo(job.mSet.editItemAt(i), enBuf)) {
            MY_LOGE("setup enque node fail");
            return BAD_VALUE;
        }
    }
    //
    if (toPush) {
        Mutex::Autolock _l(mProcessingQueueLock);
        mProcessingQueue.push_back(job);
        MY_LOGD2("Push(%d) to ProQ(%d)",
            job.mFirstMagicNum, mProcessingQueue.size());
    }
    //

    CAM_TRACE_FMT_BEGIN("P1:drv enq #(%d/%d)",
        job.mSet[0].appFrame != NULL ? job.mSet[0].appFrame->getFrameNo() : 0,
        job.mSet[0].magicNum);
    MY_LOGD2("mpCamIO->enque +++");
    if(!mpCamIO->enque(enBuf)) {
        MY_LOGE("enque fail");
        CAM_TRACE_FMT_END();
        if (toPush) {
            Mutex::Autolock _l(mProcessingQueueLock);
            Que_T::iterator it = mProcessingQueue.begin();
            for (; it != mProcessingQueue.end(); it++) {
                if ((*it).mFirstMagicNum == job.mFirstMagicNum) {
                    break;
                }
            }
            if (it != mProcessingQueue.end()) {
                mProcessingQueue.erase(it);
            }
            MY_LOGD2("Erase(%d) from ProQ(%d)",
                job.mFirstMagicNum, mProcessingQueue.size());
        }
        return BAD_VALUE;
    }
    MY_LOGD2("mpCamIO->enque ---");
    CAM_TRACE_FMT_END();
#endif
    FUNCTION_OUT_P1;
    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
hardwareOps_deque(QBufInfo &deqBuf)
{

#if SUPPORT_ISP

    FUNCTION_IN_P1
    //CAM_TRACE_NAME("P1:deque");

    if (!getActive()) {
        return BAD_VALUE;
    }

    Mutex::Autolock _l(mHardwareLock);
    if (!getActive()) {
        return BAD_VALUE;
    }

    {
        // deque buffer, and handle frame and metadata
        MY_LOGD1("%ld, %f", mDequeThreadProfile.getAvgDuration(), mDequeThreadProfile.getFps());
        QPortID PortID;
        if (IS_PORT(CONFIG_PORT_IMGO, mConfigPort)) {//(mvOutImage_full.size() > 0) {
            PortID.mvPortId.push_back(PORT_IMGO);
        }
        if (IS_PORT(CONFIG_PORT_RRZO, mConfigPort)) {//(mOutImage_resizer != NULL) {
            PortID.mvPortId.push_back(PORT_RRZO);
        }
        #if SUPPORT_EIS
        if (IS_PORT(CONFIG_PORT_EISO, mConfigPort)) {
            PortID.mvPortId.push_back(PORT_EISO);
        }
        #endif
        #if SUPPORT_VHDR
        if (IS_PORT(CONFIG_PORT_LCSO, mConfigPort)) {
            PortID.mvPortId.push_back(PORT_LCSO);
        }
        #endif
        mDequeThreadProfile.pulse_down();
        MY_LOGD2("mpCamIO->deque +++");
        CAM_TRACE_BEGIN("P1:drv deq");
        if(!mpCamIO->deque(PortID, deqBuf)) {
            MY_LOGE("deque fail");
            AEE_ASSERT("\nCRDISPATCH_KEY:MtkCam/P1Node:ISP pass1 deque fail");
            CAM_TRACE_END();
            return BAD_VALUE;
        }
        CAM_TRACE_END();
        MY_LOGD2("mpCamIO->deque ---");
        //
        mDequeThreadProfile.pulse_up();
    }
    //
    if( mDebugScanLineMask != 0 &&
        mpDebugScanLine != NULL)
    {
        for(size_t i = 0; i < deqBuf.mvOut.size(); i++)
        {
            if( (   deqBuf.mvOut[i].mPortID.index == PORT_RRZO.index &&
                    mDebugScanLineMask & DRAWLINE_PORT_RRZO  ) ||
                (   deqBuf.mvOut[i].mPortID.index == PORT_IMGO.index &&
                    mDebugScanLineMask & DRAWLINE_PORT_IMGO  )   )
            {
                mpDebugScanLine->drawScanLine(
                                    deqBuf.mvOut[i].mBuffer->getImgSize().w,
                                    deqBuf.mvOut[i].mBuffer->getImgSize().h,
                                    (void*)(deqBuf.mvOut[i].mBuffer->getBufVA(0)),
                                    deqBuf.mvOut[i].mBuffer->getBufSizeInBytes(0),
                                    deqBuf.mvOut[i].mBuffer->getBufStridesInBytes(0));

            }
        }
    }
    static bool shouldPrint = false;
    if (shouldPrint) {
        for(size_t i = 0; i < deqBuf.mvOut.size(); i++) {
            char filename[256];
            sprintf(filename, "/data/P1_%d_%d_%d.raw", deqBuf.mvOut.at(i).mMetaData.mMagicNum_hal,
                deqBuf.mvOut.at(i).mBuffer->getImgSize().w,
                deqBuf.mvOut.at(i).mBuffer->getImgSize().h);
            NSCam::Utils::saveBufToFile(filename, (unsigned char*)deqBuf.mvOut.at(i).mBuffer->getBufVA(0), deqBuf.mvOut.at(i).mBuffer->getBufSizeInBytes(0));
            shouldPrint = false;
        }
    }
    #if 0
    if (mLogLevel >= 2) {
        MUINT32 num = deqBuf.mvOut.at(0).mMetaData.mMagicNum_hal;
        if (num > 0 && num < 2)
        {
            for(size_t i = 0; i < deqBuf.mvOut.size(); i++) {
                char filename[256] = {0};
                sprintf(filename, "/sdcard/raw/p1%d_%04dx%04d_%04d_%d.raw",
                    ((deqBuf.mvOut.at(i).mPortID.index == PORT_RRZO.index) ?
                    (0) : (1)),
                    (int)deqBuf.mvOut.at(i).mBuffer->getImgSize().w,
                    (int)deqBuf.mvOut.at(i).mBuffer->getImgSize().h,
                    (int)deqBuf.mvOut.at(i).mBuffer->getBufStridesInBytes(0),
                    num);
                deqBuf.mvOut.at(i).mBuffer->saveToFile(filename);
                MY_LOGI("save to file : %s", filename);
            }
        }
    }
    #endif

    FUNCTION_OUT_P1

    return OK;
#else
    return OK;
#endif

}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
hardwareOps_stop()
{
#if SUPPORT_ISP
    CAM_TRACE_NAME("P1:hardwareOps_stop");

    FUNCTION_IN_P1

    //(1) handle active flag
    if (!getActive()) {
        MY_LOGD("active=%d", getActive());
        return OK;
    }
    setActive(MFALSE);
    setReady(MFALSE);

    if (getInit()) {
        MY_LOGI("mHardwareLock waiting");
        Mutex::Autolock _l(mHardwareLock);
    }
    MY_LOGI("mHardwareLock wait OK");

    //(2.1) stop EIS thread
    #if 0 //No need due to EIS skip waiting vsync
    CAM_TRACE_BEGIN("eis thread stop");
    if(mpEIS) {
        mpEIS->EisThreadStop();
    }
    CAM_TRACE_END();
    #endif

    //(2.2) stop 3A
    #if SUPPORT_3A
    CAM_TRACE_BEGIN("P1:3A stop");
    if (mp3A) {
        mp3A->detachCb(IHal3ACb::eID_NOTIFY_3APROC_FINISH, this);
        mp3A->detachCb(IHal3ACb::eID_NOTIFY_CURR_RESULT, this);
        mp3A->stop();
    }
    CAM_TRACE_END();
    #endif

    //(2.3) stop isp
    {
        //Mutex::Autolock _l(mHardwareLock);
        CAM_TRACE_BEGIN("P1:CamIO stop");
        MY_LOGD2("mpCamIO->stop +++");
        if(!mpCamIO || !mpCamIO->stop()) {
            MY_LOGE("hardware stop fail");
            return BAD_VALUE;
        }
        MY_LOGD2("mpCamIO->stop ---");
        CAM_TRACE_END();
    }

    //(3.1) destroy 3A
    #if SUPPORT_3A
    CAM_TRACE_BEGIN("P1:3A destroy");
    if (mp3A) {
        mp3A->destroyInstance(getNodeName());
        mp3A = NULL;
    }
    CAM_TRACE_END();
    #endif

    //(3.2) destroy isp
    {
        //Mutex::Autolock _l(mHardwareLock);
        #if SUPPORT_EIS
        if(mpEIS) {
            mpEIS->Uninit();
            mpEIS->DestroyInstance(LOG_TAG);
            mpEIS = NULL;
        }
        #endif

        #if SUPPORT_VHDR
        if(mpVhdr)
        {
            mpVhdr->Uninit();
            //mpVhdr->DestroyInstance(LOG_TAG); // instance always exist until process kill
            mpVhdr = NULL;
        }
        #endif
        //
        CAM_TRACE_BEGIN("P1:CamIO uninit");
        MY_LOGD2("mpCamIO->uninit +++");
        if(!mpCamIO->uninit() )
        {
            MY_LOGE("hardware uninit fail");
            return BAD_VALUE;
        }
        MY_LOGD2("mpCamIO->uninit ---");
        MY_LOGD2("mpCamIO->destroyInstance +++");
        #ifdef USING_MTK_LDVT
        mpCamIO->destroyInstance("iopipeUseTM");
        #else
        mpCamIO->destroyInstance(getNodeName());
        #endif
        MY_LOGD2("mpCamIO->destroyInstance ---");
        mpCamIO = NULL;
        CAM_TRACE_END();
    }
    //
    FUNCTION_OUT_P1

    return OK;

#else
    return OK;
#endif

}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
lockMetadata(sp<IPipelineFrame> const& pFrame, StreamId_T const streamId, IMetadata &rMetadata)
{
    if (pFrame == NULL) {
        //MY_LOGE("nput is NULL");
        return INVALID_OPERATION;
    }

    //  Input Meta Stream: Request
    IStreamBufferSet& rStreamBufferSet  = pFrame->getStreamBufferSet();
    sp<IMetaStreamBuffer> pMetaStreamBuffer = NULL;
    MERROR const err = ensureMetaBufferAvailable_(
        pFrame->getFrameNo(),
        streamId,
        rStreamBufferSet,
        pMetaStreamBuffer
    );
    if (err != OK) {
        return err;
    }
    rMetadata = *pMetaStreamBuffer->tryReadLock(getNodeName());

    return err;

}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
returnLockedMetadata(sp<IPipelineFrame> const& pFrame, StreamId_T const streamId, MBOOL success)
{
    if (pFrame == NULL) {
        //MY_LOGE("nput is  NULL");
        return INVALID_OPERATION;
    }

    //  Input Meta Stream: Request
    IStreamBufferSet& rStreamBufferSet  = pFrame->getStreamBufferSet();
    sp<IMetaStreamBuffer> pMetaStreamBuffer = NULL;
    MERROR const err = ensureMetaBufferAvailable_(
        pFrame->getFrameNo(),
        streamId,
        rStreamBufferSet,
        pMetaStreamBuffer,
        MFALSE
    );
    if (err != OK) {
        return err;
    }

    if  ( 0 > mInStreamIds.indexOf(streamId) ) {
        if (success) {
            pMetaStreamBuffer->markStatus(STREAM_BUFFER_STATUS::WRITE_OK);
        } else {
            pMetaStreamBuffer->markStatus(STREAM_BUFFER_STATUS::WRITE_ERROR);
        }
    }

    //
    //  Mark this buffer as USED by this user.
    //  Mark this buffer as RELEASE by this user.
    rStreamBufferSet.markUserStatus(
        streamId, getNodeId(),
        IUsersManager::UserStatus::USED |
        IUsersManager::UserStatus::RELEASE
    );

    return err;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
returnUnlockedMetadata(sp<IPipelineFrame> const& pFrame, StreamId_T const streamId)
{
    if (pFrame == NULL) {
        //MY_LOGE("nput is NULL");
        return INVALID_OPERATION;
    }

    //  Input Meta Stream: Request
    IStreamBufferSet& rStreamBufferSet  = pFrame->getStreamBufferSet();
    //
    //  Mark this buffer as RELEASE by this user.
    rStreamBufferSet.markUserStatus(
        streamId, getNodeId(),
        IUsersManager::UserStatus::RELEASE
    );

    return OK;

}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
lock_and_returnMetadata(sp<IPipelineFrame> const& pFrame, StreamId_T const streamId, IMetadata &metadata)
{
    if (pFrame == NULL) {
        //MY_LOGE("input is NULL");
        return INVALID_OPERATION;
    }

    IStreamBufferSet& rStreamBufferSet  = pFrame->getStreamBufferSet();

    //
    sp<IMetaStreamBuffer>   pMetaStreamBuffer  = NULL;
    MERROR err = ensureMetaBufferAvailable_(
        pFrame->getFrameNo(),
        streamId,
        rStreamBufferSet,
        pMetaStreamBuffer
    );
    if (err != OK) {
        return err;
    }
    IMetadata* pMetadata = pMetaStreamBuffer->tryWriteLock(getNodeName());
    if (pMetadata == NULL) {
        MY_LOGE("pMetadata == NULL");
        return BAD_VALUE;
    }

    *pMetadata = metadata;

    pMetaStreamBuffer->markStatus(STREAM_BUFFER_STATUS::WRITE_OK);
    pMetaStreamBuffer->unlock(getNodeName(), pMetadata);
    //
    //  Mark this buffer as USED by this user.
    //  Mark this buffer as RELEASE by this user.
    rStreamBufferSet.markUserStatus(
        streamId, getNodeId(),
        IUsersManager::UserStatus::USED |
        IUsersManager::UserStatus::RELEASE
    );

    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
lockImageBuffer(sp<IPipelineFrame> const& pFrame, StreamId_T const streamId,
                  sp<IImageStreamBuffer>  &pOutpImageStreamBuffer, sp<IImageBuffer> &rImageBuffer)
{
    if (pFrame == NULL) {
        //MY_LOGE("input is NULL");
        return INVALID_OPERATION;
    }

    IStreamBufferSet& rStreamBufferSet  = pFrame->getStreamBufferSet();

    MERROR const err = ensureImageBufferAvailable_(
        pFrame->getFrameNo(),
        streamId,
        rStreamBufferSet,
        pOutpImageStreamBuffer
    );
    if (err != OK) {
        return err;
    }

    MUINT groupUsage = pOutpImageStreamBuffer->queryGroupUsage(getNodeId());
    if(mDebugScanLineMask != 0)
    {
        groupUsage |= GRALLOC_USAGE_SW_WRITE_OFTEN;
    }
    sp<IImageBufferHeap>  pOutpImageBufferHeap = pOutpImageStreamBuffer->tryWriteLock(getNodeName());
    if (pOutpImageBufferHeap == NULL) {
        MY_LOGE("pOutpImageBufferHeap == NULL");
        return BAD_VALUE;
    }
    rImageBuffer = pOutpImageBufferHeap->createImageBuffer();
    rImageBuffer->lockBuf(getNodeName(), groupUsage);

    //mImageStorage.enque(pOutpImageStreamBuffer, rImageBuffer);


    MY_LOGD1("stream buffer: 0x%x, heap: 0x%x, buffer: 0x%x, usage: 0x%x",
        pOutpImageStreamBuffer.get(), pOutpImageBufferHeap.get(), rImageBuffer.get(), groupUsage);

    return err;

}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
returnLockedImageBuffer(sp<IPipelineFrame> const& pFrame, StreamId_T const streamId, sp<IImageBuffer> const& pImageBuffer, MBOOL success)
{
    if (pFrame == NULL) {
        //MY_LOGE("input is NULL");
        return INVALID_OPERATION;
    }

    IStreamBufferSet& rStreamBufferSet  = pFrame->getStreamBufferSet();

    sp<IImageStreamBuffer>  pOutpImageStreamBuffer = NULL;
    MERROR const err = ensureImageBufferAvailable_(
        pFrame->getFrameNo(),
        streamId,
        rStreamBufferSet,
        pOutpImageStreamBuffer,
        MFALSE
    );
    if (err != OK) {
        return err;
    }

    sp<IImageBuffer> pOutpImageBuffer = pImageBuffer;// mImageStorage.deque(reinterpret_cast<MINTPTR>(pOutpImageStreamBuffer.get()));
    if (pOutpImageBuffer == NULL) {
        MY_LOGE("pImageBuffer == NULL");
        return BAD_VALUE;
    }

    if  ( 0 > mInStreamIds.indexOf(streamId) ) {
        if (success) {
            pOutpImageStreamBuffer->markStatus(STREAM_BUFFER_STATUS::WRITE_OK);
        } else {
            pOutpImageStreamBuffer->markStatus(STREAM_BUFFER_STATUS::WRITE_ERROR);
        }
    }

    pOutpImageBuffer->unlockBuf(getNodeName());
    pOutpImageStreamBuffer->unlock(getNodeName(), pOutpImageBuffer->getImageBufferHeap());
    //
    //  Mark this buffer as USED by this user.
    //  Mark this buffer as RELEASE by this user.
    rStreamBufferSet.markUserStatus(
        streamId, getNodeId(),
        IUsersManager::UserStatus::USED |
        IUsersManager::UserStatus::RELEASE
    );


    return OK;

}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
returnUnlockedImageBuffer(sp<IPipelineFrame> const& pFrame, StreamId_T const streamId)
{
    if (pFrame == NULL) {
        //MY_LOGE("nput is  NULL");
        return INVALID_OPERATION;
    }

    IStreamBufferSet& rStreamBufferSet  = pFrame->getStreamBufferSet();
    //  Mark this buffer as USED by this user.
    //  Mark this buffer as RELEASE by this user.
    rStreamBufferSet.markUserStatus(
        streamId, getNodeId(),
        IUsersManager::UserStatus::RELEASE
    );

    return OK;
}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
lockImageBuffer(sp<IImageStreamBuffer> const& pStreamBuffer, sp<IImageBuffer> &pImageBuffer)
{
    if (pStreamBuffer == NULL) {
        return BAD_VALUE;
    }
    MUINT usage = GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_HW_CAMERA_READ | GRALLOC_USAGE_HW_CAMERA_WRITE;
    if(mDebugScanLineMask != 0)
    {
        usage |= GRALLOC_USAGE_SW_WRITE_OFTEN;
    }
    sp<IImageBufferHeap>  pOutpImageBufferHeap = pStreamBuffer->tryWriteLock(getNodeName());
    if (pOutpImageBufferHeap == NULL) {
        MY_LOGE("pOutpImageBufferHeap == NULL");
        return BAD_VALUE;
    }
    pImageBuffer = pOutpImageBufferHeap->createImageBuffer();
    pImageBuffer->lockBuf(getNodeName(), usage);

    //mImageStorage.enque(pStreamBuffer, pImageBuffer);

    MY_LOGD1("streambuffer: 0x%x, heap: 0x%x, buffer: 0x%x, usage: 0x%x",
        pStreamBuffer.get(), pOutpImageBufferHeap.get(), pImageBuffer.get(), usage);

    return OK;

}


/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
returnLockedImageBuffer(sp<IImageBuffer> const &pImageBuffer,
                        sp<IImageStreamBuffer> const &pStreamBuffer,
                        sp<IImageStreamBufferPoolT> const &pStreamPool)
{
    if (pImageBuffer == NULL || pStreamBuffer == NULL || pStreamPool == NULL) {
        MY_LOGE_IF(pImageBuffer == NULL,  "pImageBuffer == NULL");
        MY_LOGE_IF(pStreamBuffer == NULL, "pStreamBuffer == NULL");
        MY_LOGE_IF(pStreamPool == NULL,   "pStreamPool == NULL");
        return BAD_VALUE;
    }

    pImageBuffer->unlockBuf(getNodeName());
    pStreamBuffer->unlock(getNodeName(), pImageBuffer->getImageBufferHeap());

    if(pStreamPool != NULL) {
        pStreamPool->releaseToPool(getNodeName(), pStreamBuffer);
    }

    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MERROR
P1NodeImp::
returnUnlockedImageBuffer(sp<IImageStreamBuffer> const &pStreamBuffer,
                          sp<IImageStreamBufferPoolT> const &pStreamPool)
{
    if (pStreamBuffer == NULL || pStreamPool == NULL ) {
        MY_LOGE_IF(pStreamBuffer == NULL, "pStreamBuffer == NULL");
        MY_LOGE_IF(pStreamPool == NULL,   "pStreamPool == NULL");
        return BAD_VALUE;
    }

    pStreamPool->releaseToPool(getNodeName(), pStreamBuffer);

    return OK;
}

#if (SUPPORT_SCALING_CROP)
#if 0
/******************************************************************************
 *
 *****************************************************************************/
MVOID
P1NodeImp::
prepareCropInfo(
    IMetadata const* pAppMeta,
    IMetadata const* pHalMeta,
    QueNode_T& node)
{
    // set the default value
    if (mvOutImage_full.size() > 0) {
        node.dstSize_full = mvOutImage_full[0]->getImgSize();
        node.cropRect_full = MRect(MPoint(0, 0), mSensorParams.size);
    } else {
        node.dstSize_full = MSize(0, 0);
        node.cropRect_full = MRect(MPoint(0, 0), MSize(0, 0));
    }
    if (mOutImage_resizer != NULL) {
        node.dstSize_resizer = mOutImage_resizer->getImgSize();
        node.cropRect_resizer = MRect(MPoint(0, 0), mSensorParams.size);
    } else {
        node.dstSize_resizer= MSize(0, 0);
        node.cropRect_resizer = MRect(MPoint(0, 0), MSize(0, 0));
        return; // no need to check resizer
    }
    // check app meta
    if (pAppMeta != NULL) {
        // [TODO] check crop from app metadata
    }
    // check hal meta
    if (pHalMeta != NULL) {
        if( !tryGetMetadata<MRect>(pHalMeta, P1_REQ_CROP_TAG,
            node.cropRect_resizer) ) {
            MY_LOGI("Metadata exist - no P1_REQ_CROP_TAG");
        } else {
            if (node.dstSize_resizer.w > node.cropRect_resizer.s.w) {
                node.dstSize_resizer.w = node.cropRect_resizer.s.w;
            }
            if (node.dstSize_resizer.h > node.cropRect_resizer.s.h) {
                node.dstSize_resizer.h = node.cropRect_resizer.s.h;
            }
        }
    }
    // calculate resizer crop with its information
    if (mIsBinEn) {
        BIN_RESIZE(node.cropRect_resizer.p.x);
        BIN_RESIZE(node.cropRect_resizer.p.y);
        BIN_RESIZE(node.cropRect_resizer.s.w);
        BIN_RESIZE(node.cropRect_resizer.s.h);
    }
    // check start.x
    {
        NSCam::NSIoPipe::NSCamIOPipe::NormalPipe_QueryInfo info;
        NSCam::NSIoPipe::NSCamIOPipe::INormalPipe::query(
            NSCam::NSIoPipe::PORT_RRZO.index,
            NSCam::NSIoPipe::NSCamIOPipe::ENPipeQueryCmd_CROP_START_X,
            (EImageFormat)(mOutImage_resizer->getImgFormat()),
            node.cropRect_resizer.p.x, info);
        node.cropRect_resizer.p.x = info.crop_x;
    }
    // check size.w
    {
        NSCam::NSIoPipe::NSCamIOPipe::NormalPipe_QueryInfo info;
        NSCam::NSIoPipe::NSCamIOPipe::INormalPipe::query(
            NSCam::NSIoPipe::PORT_RRZO.index,
            NSCam::NSIoPipe::NSCamIOPipe::ENPipeQueryCmd_X_PIX|
            NSCam::NSIoPipe::NSCamIOPipe::ENPipeQueryCmd_STRIDE_BYTE,
            (EImageFormat)(mOutImage_resizer->getImgFormat()),
            node.cropRect_resizer.s.w, info);
        node.cropRect_resizer.s.w = info.x_pix;
    }
    // no scale up
    if (node.dstSize_resizer.w > node.cropRect_resizer.s.w) {
        node.dstSize_resizer.w = node.cropRect_resizer.s.w;
    }
    if (node.dstSize_resizer.h > node.cropRect_resizer.s.h) {
        node.dstSize_resizer.h = node.cropRect_resizer.s.h;
    }
    //
    MY_LOGD1("Crop Info F(%d,%d,%dx%d)(%dx%d) R(%d,%d,%dx%d)(%dx%d)",
                node.cropRect_full.p.x, node.cropRect_full.p.y,
                node.cropRect_full.s.w, node.cropRect_full.s.h,
                node.dstSize_full.w, node.dstSize_full.h,
                node.cropRect_resizer.p.x, node.cropRect_resizer.p.y,
                node.cropRect_resizer.s.w, node.cropRect_resizer.s.h,
                node.dstSize_resizer.w, node.dstSize_resizer.h);
}
#endif

/******************************************************************************
 *
 *****************************************************************************/
MVOID
P1NodeImp::
prepareCropInfo(
    IMetadata* pAppMetadata,
    IMetadata* pHalMetadata,
    QueNode_T& node)
{
    if (mvOutImage_full.size() > 0) {
        node.dstSize_full = mvOutImage_full[0]->getImgSize();
        node.cropRect_full = MRect(MPoint(0, 0), mSensorParams.size);
    } else {
        node.dstSize_full = MSize(0, 0);
        node.cropRect_full = MRect(MPoint(0, 0), MSize(0, 0));
    }
    if (mOutImage_resizer != NULL) {
        node.dstSize_resizer = mOutImage_resizer->getImgSize();
        node.cropRect_resizer = MRect(MPoint(0, 0), mSensorParams.size);
        if (mIsBinEn) {
            BIN_RESIZE(node.cropRect_resizer.p.x);
            BIN_RESIZE(node.cropRect_resizer.p.y);
            BIN_RESIZE(node.cropRect_resizer.s.w);
            BIN_RESIZE(node.cropRect_resizer.s.h);
        }
    } else {
        node.dstSize_resizer= MSize(0, 0);
        node.cropRect_resizer = MRect(MPoint(0, 0), MSize(0, 0));
    }
    //
    if (pAppMetadata != NULL && pHalMetadata != NULL) {
        MRect cropRect_metadata;    // get from metadata
        MRect cropRect_control;     // set to node

        MY_LOGD2("mIsBinEn:%d", mIsBinEn);
        if( !tryGetMetadata<MRect>(pAppMetadata, MTK_SCALER_CROP_REGION, cropRect_metadata) ) {
            MY_LOGI("Metadata exist - no MTK_SCALER_CROP_REGION, "
                "crop size set to full(%dx%d) resizer(%dx%d)",
                node.dstSize_full.w, node.dstSize_full.h,
                node.dstSize_resizer.w, node.dstSize_resizer.h);
        } else {
            if( !tryGetMetadata<MRect>(pHalMetadata, MTK_P1NODE_SENSOR_CROP_REGION, cropRect_control) )
            {
                MY_LOGD2("cannot get MTK_P1NODE_SENSOR_CROP_REGION, use MTK_SCALER_CROP_REGION");
                if (mIsBinEn) {
                    BIN_RESIZE(cropRect_metadata.p.x);
                    BIN_RESIZE(cropRect_metadata.p.y);
                    BIN_RESIZE(cropRect_metadata.s.w);
                    BIN_RESIZE(cropRect_metadata.s.h);
                }
                simpleTransform tranActive2Sensor = simpleTransform(
                        MPoint(0,0), mActiveArray.size(), mSensorParams.size);
                cropRect_control.p = transform(tranActive2Sensor,
                                                cropRect_metadata.leftTop());
                cropRect_control.s = transform(tranActive2Sensor,
                                                cropRect_metadata.size());
            }
            else
            {
                MY_LOGD2("get MTK_P1NODE_SENSOR_CROP_REGION success");
                if (mIsBinEn) {
                    BIN_RESIZE(cropRect_control.p.x);
                    BIN_RESIZE(cropRect_control.p.y);
                    BIN_RESIZE(cropRect_control.s.w);
                    BIN_RESIZE(cropRect_control.s.h);
                }
            }
            MY_LOGD2("crop size set to (%d,%d,%dx%d))",
                cropRect_control.p.x, cropRect_control.p.y,
                cropRect_control.s.w, cropRect_control.s.h);

            #if SUPPORT_EIS
            if (mpEIS)
            {
                MBOOL isEisOn = false;
                MRect const requestRect = MRect(cropRect_control);
                MSize const sensorSize = MSize(mSensorParams.size);
                MPoint const requestCenter=
                    MPoint((requestRect.p.x + (requestRect.s.w >> 1)),
                            (requestRect.p.y + (requestRect.s.h >> 1)));
                isEisOn = isEISOn(pAppMetadata);
                cropRect_control.s = mpEIS->QueryMinSize(isEisOn, sensorSize,
                                                        requestRect.size());

                if (cropRect_control.s.w != requestRect.size().w)
                {
                    MSize::value_type half_len =
                        ((cropRect_control.s.w + 1) >> 1);
                    if (requestCenter.x < half_len) {
                        cropRect_control.p.x = 0;
                    } else if ((requestCenter.x + half_len) > sensorSize.w) {
                        cropRect_control.p.x = sensorSize.w -
                                                cropRect_control.s.w;
                    } else {
                        cropRect_control.p.x = requestCenter.x - half_len;
                    }
                }
                if (cropRect_control.s.w != requestRect.size().w)
                {
                    MSize::value_type half_len =
                        ((cropRect_control.s.h + 1) >> 1);
                    if (requestCenter.y < half_len) {
                        cropRect_control.p.y = 0;
                    } else if ((requestCenter.y + half_len) > sensorSize.h) {
                        cropRect_control.p.y = sensorSize.h -
                                                cropRect_control.s.h;
                    } else {
                        cropRect_control.p.y = requestCenter.y - half_len;
                    }
                }
            }
            #endif
            /*
            MY_LOGD("[CropInfo] metadata(%d, %d, %dx%d) "
                "control(%d, %d, %dx%d) "
                "active(%d, %d, %dx%d) "
                "sensor(%dx%d)",
                cropRect_metadata.leftTop().x,
                cropRect_metadata.leftTop().y,
                cropRect_metadata.size().w, cropRect_metadata.size().h,
                cropRect_control.leftTop().x,
                cropRect_control.leftTop().y,
                cropRect_control.size().w, cropRect_control.size().h,
                mActiveArray.leftTop().x,
                mActiveArray.leftTop().y,
                mActiveArray.size().w, mActiveArray.size().h,
                mSensorParams.size.w, mSensorParams.size.h);
            */
            // TODO: check more case about crop region
            if ((cropRect_control.size().w < 0) ||
                (cropRect_control.size().h < 0) ||
                (cropRect_control.leftTop().x < 0) ||
                (cropRect_control.leftTop().y < 0) ||
                (cropRect_control.leftTop().x >= mSensorParams.size.w) ||
                (cropRect_control.leftTop().y >= mSensorParams.size.h)) {
                MY_LOGW("Metadata exist - invalid cropRect_control"
                    "(%d, %d, %dx%d) sensor(%dx%d)",
                    cropRect_control.leftTop().x,
                    cropRect_control.leftTop().y,
                    cropRect_control.size().w, cropRect_control.size().h,
                    mSensorParams.size.w, mSensorParams.size.h);
                return;
            }
            if ((cropRect_control.p.x + cropRect_control.s.w) >
                mSensorParams.size.w) {
                cropRect_control.s.w = mSensorParams.size.w -
                                        cropRect_control.p.x;
            }
            if ((cropRect_control.p.y + cropRect_control.s.h) >
                mSensorParams.size.h) {
                cropRect_control.s.h = mSensorParams.size.h -
                                        cropRect_control.p.y;
            }
            // calculate the crop region validity
            if (mvOutImage_full.size() > 0) {
                calculateCropInfoFull(mSensorParams.pixelMode,
                                        mSensorParams.size,
                                        mvOutImage_full[0]->getImgSize(),
                                        cropRect_control,
                                        node.cropRect_full,
                                        node.dstSize_full,
                                        mLogLevel);
            }
            if (mOutImage_resizer != NULL) {
                calculateCropInfoResizer(mSensorParams.pixelMode,
                                        (mOutImage_resizer->getImgFormat()),
                                        mSensorParams.size,
                                        (mOutImage_resizer->getImgSize()),
                                        cropRect_control,
                                        node.cropRect_resizer,
                                        node.dstSize_resizer,
                                        mLogLevel);
            }
        }
    }
    MY_LOGD1("Crop-Info F(%d,%d,%dx%d)(%dx%d) R(%d,%d,%dx%d)(%dx%d)",
            node.cropRect_full.p.x, node.cropRect_full.p.y,
            node.cropRect_full.s.w, node.cropRect_full.s.h,
            node.dstSize_full.w, node.dstSize_full.h,
            node.cropRect_resizer.p.x, node.cropRect_resizer.p.y,
            node.cropRect_resizer.s.w, node.cropRect_resizer.s.h,
            node.dstSize_resizer.w, node.dstSize_resizer.h);
}
#endif


/******************************************************************************
 *
 *****************************************************************************/
MVOID
P1NodeImp::
createNode(sp<IPipelineFrame> appframe,
           QueJob_T *job,
           Que_T *Queue,
           Mutex *QueLock,
           List<MetaSet_T> *list,
           Mutex *listLock)
{
    //create queue node
    MUINT32 newNum = get_and_increase_magicnum();
    MetaSet_T metaInfo;
    IMetadata* pAppMeta = NULL;
    IMetadata* pHalMeta = NULL;
    MUINT8 isHQC = 0;
    //
    MUINT32 nodeOut = REQ_SET_NONE;
    MUINT32 nodeType = REQ_TYPE_UNKNOWN;
    MBOOL isRedoRequest = false;
    IPipelineFrame::InfoIOMapSet rIOMapSet;
    //
    MINT32 meta_raw_type = (MINT32)mRawDefType;
    MBOOL meta_raw_exist = MFALSE;
    //
    if (appframe != NULL) {
        if(OK != appframe->queryInfoIOMapSet(getNodeId(), rIOMapSet)) {
            MY_LOGE("queryInfoIOMap failed");
            return;
        }
        //
        // do some check
        IPipelineFrame::ImageInfoIOMapSet& imageIOMapSet =
                                                rIOMapSet.mImageInfoIOMapSet;
        if(imageIOMapSet.isEmpty()) {
            MY_LOGW("no imageIOMap in frame");
            return;
        }
        //
        if (mInAppMeta != NULL) {
            if (OK == lockMetadata(
                appframe, mInAppMeta->getStreamId(), metaInfo.appMeta)) {
                pAppMeta = &(metaInfo.appMeta);
            } else {
                MY_LOGI("can not lock the app metadata");
            }
        }
        if (mInHalMeta != NULL) {
            if (OK == lockMetadata(
                appframe, mInHalMeta->getStreamId(), metaInfo.halMeta)) {
                pHalMeta = &(metaInfo.halMeta);
            } else {
                MY_LOGI("can not lock the hal metadata");
            }
        }
        #if 1 // add raw type from hal meta
        if (pHalMeta != NULL) {
            MINT32 raw_type = meta_raw_type;
            if (tryGetMetadata<MINT32>(
                pHalMeta, MTK_P1NODE_RAW_TYPE, raw_type)) {
                MY_LOGD1("raw type set from outside %d", raw_type);
                if ((mRawOption & (MUINT32)(1 << raw_type)) > 0) {
                    meta_raw_type = raw_type;
                    meta_raw_exist = MTRUE;
                }
            }
        }
        #endif
        //
        for (size_t i = 0; i < imageIOMapSet.size(); i++) {
            IPipelineFrame::ImageInfoIOMap const& imageIOMap = imageIOMapSet[i];
            if (imageIOMap.vIn.size() > 1) {
                isRedoRequest = true;
                break;
            } else {
                for (size_t j = 0; j < imageIOMap.vOut.size(); j++) {
                    StreamId_T const streamId = imageIOMap.vOut.keyAt(j);
                    if (mOutImage_resizer != NULL) {
                        if (streamId == mOutImage_resizer->getStreamId()) {
                            nodeOut |= REQ_SET(REQ_OUT_RESIZER);
                        }
                    }
                    for (size_t k = 0; k < mvOutImage_full.size(); k++) {
                        if (streamId == mvOutImage_full[k]->getStreamId()) {
                            if (meta_raw_type == EPipe_PROCESSED_RAW) {
                                nodeOut |= REQ_SET(REQ_OUT_FULL_PROC);
                            } else {
                                nodeOut |= REQ_SET(REQ_OUT_FULL_PURE);
                            }
                        }
                    }
                    /*
                    if (mOutImage_combo != NULL) {
                        if (streamId == mOutImage_combo->getStreamId()) {
                            nodeOut |= REQ_SET(REQ_OUT_FULL_COMBO);
                        }
                    }
                    */
                }
            }
        }
    }

    if (isRedoRequest) {
        nodeType = REQ_TYPE_REDO;
        nodeOut = REQ_SET_NONE;
    } else if (list != NULL) { // create list for 3A
        nodeType = (appframe == NULL) ? REQ_TYPE_INITIAL : REQ_TYPE_NORMAL;
        //fill in App metadata
        if (appframe != NULL) {

        }
        //fill in hal metadata
        IMetadata::IEntry entry1(MTK_P1NODE_PROCESSOR_MAGICNUM);
        entry1.push_back(newNum, Type2Type< MINT32 >());
        metaInfo.halMeta.update(MTK_P1NODE_PROCESSOR_MAGICNUM, entry1);

        IMetadata::IEntry entry2(MTK_HAL_REQUEST_REPEAT);
        entry2.push_back(0, Type2Type< MUINT8 >());
        metaInfo.halMeta.update(MTK_HAL_REQUEST_REPEAT, entry2);


        MUINT8 isdummy =  appframe == NULL ? 1 : 0;
        IMetadata::IEntry entry3(MTK_HAL_REQUEST_DUMMY);
        entry3.push_back(isdummy, Type2Type< MUINT8 >());
        metaInfo.halMeta.update(MTK_HAL_REQUEST_DUMMY, entry3);


        #if 1 // add raw type to hal meta
        if (!meta_raw_exist) {
            IMetadata::IEntry entryRawType(MTK_P1NODE_RAW_TYPE);
            entryRawType.push_back(meta_raw_type, Type2Type< MINT32 >());
            metaInfo.halMeta.update(MTK_P1NODE_RAW_TYPE, entryRawType);
        }
        #endif

        if(listLock != NULL) {
            Mutex::Autolock _l(*listLock);
            (*list).push_back(metaInfo);
        } else {
            (*list).push_back(metaInfo);
        }

    }

    if (nodeType == REQ_TYPE_NORMAL || nodeType == REQ_TYPE_INITIAL) {
        if (IS_PORT(CONFIG_PORT_IMGO, mConfigPort)
            && (0 == (IS_OUT(REQ_OUT_FULL_PROC, nodeOut) ||
                    IS_OUT(REQ_OUT_FULL_PURE, nodeOut) ||
                    IS_OUT(REQ_OUT_FULL_COMBO, nodeOut))
                )
            ) {
                nodeOut |= REQ_SET(REQ_OUT_FULL_STUFF);
            }
        if (IS_PORT(CONFIG_PORT_RRZO, mConfigPort)
            && (0 == IS_OUT(REQ_OUT_RESIZER, nodeOut))) {
                nodeOut |= REQ_SET(REQ_OUT_RESIZER_STUFF);
            }
    }
    MY_LOGD("node type(0x%X) out(0x%X)", nodeType, nodeOut);
    if (nodeType == REQ_TYPE_UNKNOWN) {
        MY_LOGW("request type UNKNOWN");
    } else if (nodeType == REQ_TYPE_REDO) {
        MY_LOGD("request type REDO");
        /*
        Mutex::Autolock _l(mRedoQueueLock);
        QueNode_T node;
        node.magicNum = newNum;
        node.sofIdx = P1SOFIDX_INIT_VAL;
        node.appFrame = appframe;
        node.reqOut = nodeOut;
        node.reqType = nodeType;
        mRedoQueue.push_back(node);
        */
    } else if ((nodeType == REQ_TYPE_NORMAL) && (nodeOut == REQ_SET_NONE)) {
        MY_LOGW("request out NONE");
    } else if(job != NULL) {
        // node type is REQ_TYPE_NORMAL-with-output or REQ_TYPE_PADDING
        Mutex::Autolock _l(*QueLock);
        QueNode_T node;
        node.magicNum = newNum;
        node.sofIdx = P1SOFIDX_INIT_VAL;
        node.appFrame = appframe;
        node.reqType = nodeType;
        node.reqOutSet = nodeOut;
        node.reqExpRec = REQ_REC_NONE;
        //
        #if (SUPPORT_SCALING_CROP)
        //prepareCropInfo(pAppMeta, node);
        prepareCropInfo(pAppMeta, pHalMeta, node);
        #endif
        job->mSet.push_back(node);
        if (job->mSet.size() == 1) {
            job->mFirstMagicNum = newNum;
        }
    }
    //
    MY_LOGD1("job(%d)-(%d/%d) list(%d)",
        job->mFirstMagicNum, job->mSet.size(), job->mMaxNum,
        mControls3AList.size());
    //
    if(Queue != NULL) {
        Mutex::Autolock _l(*QueLock);
        if (job->mSet.size() == job->mMaxNum) {
            (*Queue).push_back((*job));
            job->mSet.clear();
        }
    }
    //
    if (appframe != NULL) {
        MY_LOGD1("[New Request] frameNo: %u, magic Num: %d",
            appframe->getFrameNo(), newNum);
    } else {
        MY_LOGD1("[New Request: dummy] magic Num: %d",
            newNum);
    }
    return;
}


/******************************************************************************
 *
 *****************************************************************************/
MVOID
P1NodeImp::
createNode(List<MetaSet_T> *list,
           Mutex *listLock)
{
    if (list == NULL) {
        MY_LOGW("list not exist");
        return;
    }
    MUINT32 newNum = get_and_increase_magicnum();
    MetaSet_T metaInfo;
    //fill in hal metadata
    IMetadata::IEntry entry1(MTK_P1NODE_PROCESSOR_MAGICNUM);
    entry1.push_back(newNum, Type2Type< MINT32 >());
    metaInfo.halMeta.update(MTK_P1NODE_PROCESSOR_MAGICNUM, entry1);
    //
    IMetadata::IEntry entry2(MTK_HAL_REQUEST_REPEAT);
    entry2.push_back(0, Type2Type< MUINT8 >());
    metaInfo.halMeta.update(MTK_HAL_REQUEST_REPEAT, entry2);
    //
    IMetadata::IEntry entry3(MTK_HAL_REQUEST_DUMMY);
    entry3.push_back(1, Type2Type< MUINT8 >());
    metaInfo.halMeta.update(MTK_HAL_REQUEST_DUMMY, entry3);

    if(listLock != NULL) {
        Mutex::Autolock _l(*listLock);
        (*list).push_back(metaInfo);
    } else {
        (*list).push_back(metaInfo);
    }

    MY_LOGD1("[New Request: padding] magic Num: %d", newNum);
    MY_LOGD1("#%d mControls3AList size(%d)", __LINE__, mControls3AList.size());
    return;
}


/******************************************************************************
 *
 *****************************************************************************/
MVOID
P1NodeImp::
createNode(Que_T &Queue)
{
    QueJob_T job(mBurstNum);
    for (MUINT8 n = 0; n < mBurstNum; n++) {
        MUINT32 newNum = get_and_increase_magicnum();
        {
            QueNode_T node;
            node.magicNum = newNum;
            node.sofIdx = P1SOFIDX_INIT_VAL;
            node.appFrame = NULL;
            job.mSet.push_back(node);
            //job.mCurNum ++;
            if (n == 0) {
                job.mFirstMagicNum = newNum;
            }
        }
        MY_LOGD1("[New Request: dummy] magic Num: %d", newNum);
        MY_LOGD1("job(%d)-(%d/%d) dummy",
            job.mFirstMagicNum, job.mSet.size(), job.mMaxNum);
    }

    Queue.push_back(job);

    return;
}


/******************************************************************************
 *
 ******************************************************************************/
sp<P1Node>
P1Node::
createInstance()
{
    return new P1NodeImp();

}

