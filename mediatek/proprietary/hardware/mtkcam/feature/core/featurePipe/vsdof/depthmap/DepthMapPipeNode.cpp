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

#include "DepthMapPipeNode.h"
#include <featurePipe/core/include/DebugUtil.h>

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {

const char* DepthMapDataHandler::ID2Name(DataID id)
{
#define MAKE_NAME_CASE(name) \
  case name: return #name;

  switch(id)
  {
    MAKE_NAME_CASE(ID_INVALID);
    MAKE_NAME_CASE(ROOT_ENQUE);

    MAKE_NAME_CASE(P2AFM_TO_N3D_FEFM_CCin);
    MAKE_NAME_CASE(P2AFM_TO_WMF_MY_SL);
    MAKE_NAME_CASE(P2AFM_TO_FD_IMG);

    MAKE_NAME_CASE(N3D_TO_DPE_MVSV_MASK);
    MAKE_NAME_CASE(N3D_TO_OCC_LDC);
    MAKE_NAME_CASE(N3D_TO_FD_EXTDATA_MASK);
    MAKE_NAME_CASE(DPE_TO_OCC_MVSV_DMP_CFM);
    MAKE_NAME_CASE(OCC_TO_WMF_DMH_MY_S);

    // DepthMap output
    MAKE_NAME_CASE(P2AFM_OUT_MV_F);
    MAKE_NAME_CASE(P2AFM_OUT_FD);
    MAKE_NAME_CASE(P2AFM_OUT_MV_F_CAP);
    MAKE_NAME_CASE(WMF_OUT_DMW_MY_S);
    MAKE_NAME_CASE(WMF_OUT_DEPTHMAP);
    MAKE_NAME_CASE(FD_OUT_EXTRADATA);

    MAKE_NAME_CASE(DEPTHMAP_META_OUT);

    MAKE_NAME_CASE(N3D_OUT_JPS);
    #ifdef GTEST
    MAKE_NAME_CASE(UT_OUT_FE);
    #endif

  };
  MY_LOGW(" unknown id:%d", id);
  return "UNKNOWN";
#undef MAKE_NAME_CASE
}

DepthMapDataHandler::~DepthMapDataHandler()
{
}

DepthMapPipeNode::DepthMapPipeNode(const char *name, Graph_T *graph)
  : CamThreadNode(name)
{
    mbDebugLog = StereoSettingProvider::isLogEnabled(PERPERTY_DEPTHMAP_NODE_LOG);
    mbProfileLog = StereoSettingProvider::isProfileLogEnabled();

    #ifdef GTEST
    mbDebugLog = mbProfileLog = MTRUE;
    #endif

    // dump buffer size : default size 5
    char cLogLevel[PROPERTY_VALUE_MAX];
    ::property_get("depthmap.pipe.dump.size", cLogLevel, "0");
    miDumpBufSize = atoi(cLogLevel);
    if(miDumpBufSize == 0)
        miDumpBufSize = 5;
    // start frame index of dump buffer  : default 0
    ::property_get("depthmap.pipe.dump.start", cLogLevel, "0");
    miDumpStartIdx = atoi(cLogLevel);

    MY_LOGD("mbDebugLog=%d mbProfileLog=%d", mbDebugLog, mbProfileLog);



}

DepthMapPipeNode::~DepthMapPipeNode()
{
}

MBOOL DepthMapPipeNode::onDump(DataID id, FrameInfoPtr &data, const char* fileName)
{
    if(!checkToDump(id))
    {
        return MFALSE;
    }


    sp<IImageBuffer> pImgBuf;
    data->getFrameBuffer(pImgBuf);

    MUINT iReqIdx = data->getRequestNo();
    // check dump index
    if(iReqIdx < miDumpStartIdx || iReqIdx > miDumpStartIdx + miDumpBufSize)
        return MTRUE;

    char filepath[1024];
    // make path
    snprintf(filepath, 1024, "/sdcard/vsdof/%d/%s", iReqIdx, getName());
    MY_LOGD("makePath: %s", filepath);
    makePath(filepath, 0660);

    const char* writeFileName = (fileName != NULL) ? fileName : ID2Name(id);
    snprintf(filepath, 1024, "/sdcard/vsdof/%d/%s/%s_%dx%d.yuv", iReqIdx,
            getName(), writeFileName, pImgBuf->getImgSize().w, pImgBuf->getImgSize().h);
    pImgBuf->saveToFile(filepath);
    return MTRUE;
}

MBOOL DepthMapPipeNode::onDump(DataID id, ImgInfoMapPtr &data, const char* fileName)
{
    if(!checkToDump(id))
    {
        return MFALSE;
    }


    MY_LOGD("onDump: dataID:%d ", id);
    sp<EffectRequest> pEffReq = data->getRequestPtr();

    MUINT iReqIdx = pEffReq->getRequestNo();
    // check dump index
    if(iReqIdx < miDumpStartIdx || iReqIdx > miDumpStartIdx + miDumpBufSize)
        return MTRUE;

    char filepath[1024];

    // make path
    snprintf(filepath, 1024, "/sdcard/vsdof/%d/%s", pEffReq->getRequestNo(), getName());
    MY_LOGD("makePath: %s", filepath);
    makePath(filepath, 0660);

    const SmartImageBufferSet ImgBufSet = data->getImageBufferSet();
    for(size_t i=0;i<ImgBufSet.size();++i)
    {
        const DepthMapBufferID& BID = ImgBufSet.keyAt(i);
        const SmartImageBuffer& smBuf = ImgBufSet.valueAt(i);
        const char* writeFileName = (fileName != NULL) ? fileName : onDumpBIDToName(BID);
        snprintf(filepath, 1024, "/sdcard/vsdof/%d/%s/%s_%dx%d.yuv", pEffReq->getRequestNo(),
                    getName(), writeFileName, smBuf->mImageBuffer->getImgSize().w, smBuf->mImageBuffer->getImgSize().h);
        smBuf->mImageBuffer->saveToFile(filepath);
    }

    const GraphicImageBufferSet graImgBufSet = data->getGraphicBufferSet();
    for(size_t i=0;i<graImgBufSet.size();++i)
    {
        const DepthMapBufferID& BID = graImgBufSet.keyAt(i);
        const SmartGraphicBuffer& smGraBuf = graImgBufSet.valueAt(i);
        const char* writeFileName = (fileName != NULL) ? fileName : onDumpBIDToName(BID);
        snprintf(filepath, 1024, "/sdcard/vsdof/%d/%s/%s_%dx%d.yuv", pEffReq->getRequestNo(),
                        getName(), writeFileName, smGraBuf->mImageBuffer->getImgSize().w, smGraBuf->mImageBuffer->getImgSize().h);
        smGraBuf->mImageBuffer->saveToFile(filepath);
    }

    return MTRUE;
}


MBOOL DepthMapPipeNode::checkToDump(DataID id)
{
    MINT32 iNodeDump =  getPropValue();

    if(getPropValue() == 0)
    {
        return MFALSE;
    }


    if(getPropValue(id) == 0)
    {
        return MFALSE;
    }

    return MTRUE;
}

}; // namespace NSFeaturePipe
}; // namespace NSCamFeature
}; // namespace NSCam
