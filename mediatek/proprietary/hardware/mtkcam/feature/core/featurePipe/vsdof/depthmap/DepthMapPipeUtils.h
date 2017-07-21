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

#ifndef _MTK_CAMERA_INCLUDE_STEREO_DEPTHMAP_FEATUREPIPE_UTILS_H_
#define _MTK_CAMERA_INCLUDE_STEREO_DEPTHMAP_FEATUREPIPE_UTILS_H_

#include <utils/Vector.h>
#include <utils/KeyedVector.h>
#include <metadata/IMetadata.h>
#include <mmsdk/EffectRequest.h>
#include <mtkcam/common/include/common.h>
#include <iopipe/Port.h>
#include <iopipe/PostProc/IHalPostProcPipe.h>
#include <iopipe/PostProc/INormalStream.h>
#include <drv/isp_reg.h>

#define PIPE_MODULE_TAG "DepthMapPipe"
#define PIPE_CLASS_TAG "Utils"
#include <featurePipe/core/include/PipeLog.h>

using namespace NSCam::NSIoPipe::NSPostProc;
using namespace android;
using NSCam::NSIoPipe::PortID;

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {

/**
 * Try to get metadata value
 *
 * @param[in]  pMetadata: IMetadata instance
 * @param[in]  tag: the metadata tag to retrieve
 * @param[out]  rVal: the metadata value
 *
 *
 * @return
 *  -  true if successful; otherwise false.
 */
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
    //
    IMetadata::IEntry entry = pMetadata->entryFor(tag);
    if( !entry.isEmpty() ) {
        rVal = entry.itemAt(0, Type2Type<T>());
        return MTRUE;
    }
    return MFALSE;
}

/**
 * Try to set metadata value
 *
 * @param[in]  pMetadata: IMetadata instance
 * @param[in]  tag: the metadata tag to set
 * @param[in]  val: the metadata value to be configured
 *
 *
 * @return
 *  -  true if successful; otherwise false.
 */
template <typename T>
inline MVOID
trySetMetadata(
    IMetadata* pMetadata,
    MUINT32 const tag,
    T const& val
)
{
    if( pMetadata == NULL ) {
        MY_LOGW("pMetadata == NULL");
        return;
    }
    //
    IMetadata::IEntry entry(tag);
    entry.push_back(val, Type2Type<T>());
    pMetadata->update(tag, entry);
}

template <typename T>
inline MVOID
updateEntry(
    IMetadata* pMetadata,
    MUINT32 const tag,
    T const& val
)
{
    if( pMetadata == NULL ) {
        MY_LOGW("pMetadata == NULL");
        return;
    }

    IMetadata::IEntry entry(tag);
    entry.push_back(val, Type2Type<T>());
    pMetadata->update(tag, entry);
}

enum eCropGroup
{
    eCROP_CRZ=1,
    eCROP_WDMA=2,
    eCROP_WROT=3
};



/**
 * Store the location information during the template generator
 *
 **/
 struct TemplateLocStat
{
    // key = portID or groupID, value= position index
    KeyedVector<MUINT32, MUINT32> mKeyToLocMap;
};
/**
 * Store the location information of mvIn, mvOut, mvCropRsInfo for each frame
 *
 **/
struct TemplateStatistics
{
public:
    // key = frame ID, value= position index
    KeyedVector<MUINT32, TemplateLocStat> mFrameIDToLocMap_mvIn;
    KeyedVector<MUINT32, TemplateLocStat> mFrameIDToLocMap_mvOut;
    KeyedVector<MUINT32, TemplateLocStat> mFrameIDToLocMap_mvCrop;

public:
    TemplateStatistics(): miActiveFrameID(-1), mvInTotalSize(0), mvOutTotalSize(0),
                          mvCropTotalSize(0){}

    MVOID inherit(TemplateStatistics& stat);
    MVOID setActiveFrame(MUINT32 frameID);
    MVOID addCrop(eCropGroup groupID);
    MVOID addMvIn(MUINT32 portIdx);
    MVOID addMvOut(MUINT32 portIdx);

    MUINT32 mvInTotalSize;
    MUINT32 mvOutTotalSize;
    MUINT32 mvCropTotalSize;

    MINT32 miActiveFrameID;
};


class QParamTemplateGenerator;

class QParamTemplateGenerator
{
private:
    QParamTemplateGenerator();
    MBOOL checkValid();
public:
    QParamTemplateGenerator(MUINT32 frameID, ENormalStreamTag streamTag, TemplateStatistics* pLearningStat=NULL);

    QParamTemplateGenerator& addCrop(eCropGroup groupID, MPoint startLoc, MSize cropSize, MSize resizeDst);
    QParamTemplateGenerator& addInput(NSCam::NSIoPipe::PortID portID);
    QParamTemplateGenerator& addOutput(NSCam::NSIoPipe::PortID portID, MINT32 transform=0);
    QParamTemplateGenerator& addModuleInfo(MUINT32 moduleTag, MVOID* moduleStruct);
    MBOOL generate(QParams& rOutputParams, TemplateStatistics& rStats);
public:
    MUINT32 miFrameID;
    MINT32 mStreamTag;
    android::Vector<Input>      mvIn;
    android::Vector<Output>     mvOut;
    android::Vector<MCrpRsInfo> mvCropRsInfo;
    android::Vector<ModuleInfo> mvModuleData;
    TemplateStatistics mStats;
};


class QParamTemplateFiller
{

public:
    QParamTemplateFiller(QParams& target, const TemplateStatistics& tempStats);
    QParamTemplateFiller& insertTuningBuf(MUINT frameID, MVOID* pTuningBuf);
    QParamTemplateFiller& insertInputBuf(MUINT frameID, NSCam::NSIoPipe::PortID portID, IImageBuffer* pImggBuf);
    QParamTemplateFiller& insertInputBuf(MUINT frameID, NSCam::NSIoPipe::PortID portID, sp<EffectFrameInfo> pFrameInfo);
    QParamTemplateFiller& insertOutputBuf(MUINT frameID, NSCam::NSIoPipe::PortID portID, IImageBuffer* pImggBuf);
    QParamTemplateFiller& insertOutputBuf(MUINT frameID, NSCam::NSIoPipe::PortID portID, sp<EffectFrameInfo> pFrameInfo);
    QParamTemplateFiller& setCrop(MUINT frameID, eCropGroup groupID, MPoint startLoc, MSize cropSize, MSize resizeDst);
    QParamTemplateFiller& delOutputPort(MUINT frameID, NSCam::NSIoPipe::PortID portID, eCropGroup cropGID);
    /**
     * Validate the template filler status
     *
     * @return
     *  -  true if successful; otherwise false.
     **/
    MBOOL validate();
private:

    // the location to be deleted vector
    Vector<MUINT> mDelOutPortLocVec;
    Vector<MUINT>  mDelCropLocVec;
public:
    QParams& mTargetQParam;
    const TemplateStatistics& mTemplStats;
    MBOOL mbSuccess;

};

enum tuning_tag
{
    tuning_tag_G2G = 0,
    tuning_tag_G2C,
    tuning_tag_GGM,
    tuning_tag_UDM,
};
void SetDefaultTuning(dip_x_reg_t* pIspReg, MUINT32* tuningBuf, tuning_tag tag, int enFgMode);


}
}
}
#endif
