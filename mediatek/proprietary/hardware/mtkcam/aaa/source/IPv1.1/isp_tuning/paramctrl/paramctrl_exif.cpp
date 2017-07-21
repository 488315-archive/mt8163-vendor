/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
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

/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
#define LOG_TAG "paramctrl_exif"

#ifndef ENABLE_MY_LOG
    #define ENABLE_MY_LOG       (1)
#endif

#include <aaa_types.h>
#include <aaa_log.h>
#include <aaa_hal_if.h>
#include <camera_custom_nvram.h>
#include <isp_tuning.h>
#include <camera_feature.h>
#include <awb_param.h>
#include <ae_param.h>
#include <af_param.h>
#include <flash_param.h>
#include <isp_tuning_cam_info.h>
#include <isp_tuning_idx.h>
#include <isp_tuning_custom.h>
#include <isp_mgr.h>
#include <isp_mgr_helper.h>
#include <pca_mgr.h>
#include <ccm_mgr.h>
#include "paramctrl.h"

using namespace android;
using namespace NSFeature;
using namespace NSIspTuning;
using namespace NSIspTuningv3;
using namespace NSIspExifDebug;


namespace
{
//  Tag <- Isp Index.
inline
MVOID
setIspIdx(
    IspDebugTag (&rTags)[TagID_Total_Num],
    IspDebugTagID const eTagID,
    MUINT32 const u4Idx
)
{
    rTags[eTagID].u4ID  = getIspTag(eTagID);
    rTags[eTagID].u4Val = u4Idx;
}

//  Tag <- Isp Regs.
template <class ISP_xxx_T, MUINT32 TagID_begin>
inline
MVOID
setIspTags(
    ESensorDev_T eSensorDev,
    MBOOL fgReadFromHW,
    IspDebugTag (&rTags)[TagID_Total_Num]
)
{
    enum { E_NUM = ISP_xxx_T::COUNT };
    ISP_xxx_T param;

    //MY_LOG("fgReadFromHW = %d", fgReadFromHW);

    if (fgReadFromHW) {
        getIspReg(eSensorDev, param);
    }
    else {
        getIspHWBuf(eSensorDev, param);
    }

    for (MUINT32 i = 0; i < E_NUM; i++)
    {
        MUINT32 const u4TagID = TagID_begin + i;
        rTags[u4TagID].u4ID   = getIspTag(u4TagID);
        rTags[u4TagID].u4Val  = param.set[i];
        //MY_LOG("[%d]=0x%08X", i, rTags[u4TagID].u4Val);
    }
    STATIC_CHECK(
        TagID_begin+E_NUM-1 < TagID_Total_Num,
        tag_index_over_total_num
    );
}

template <class ISP_xxx_T, typename ISP_MGR_T, MUINT32 TagID_begin>
inline
MVOID
setIspTags2(
    ESensorDev_T eSensorDev,
    MBOOL fgReadFromHW,
    IspDebugTag (&rTags)[TagID_Total_Num]
)
{
    enum { E_NUM = ISP_xxx_T::COUNT };
    ISP_xxx_T param;

    //MY_LOG("fgReadFromHW = %d", fgReadFromHW);
    ISP_MGR_T::getInstance(eSensorDev).get(param);

    for (MUINT32 i = 0; i < E_NUM; i++)
    {
        MUINT32 const u4TagID = TagID_begin + i;
        rTags[u4TagID].u4ID   = getIspTag(u4TagID);
        rTags[u4TagID].u4Val  = param.set[i];
        //MY_LOG("[%d]=0x%08X", i, rTags[u4TagID].u4Val);
    }
    STATIC_CHECK(
        TagID_begin+E_NUM-1 < TagID_Total_Num,
        tag_index_over_total_num
    );
}

template <class ISP_xxx_T, typename ISP_MGR_T, MUINT32 TagID_begin>
inline
MVOID
setIspTags2(
    ESensorDev_T eSensorDev,
    const dip_x_reg_t* pReg,
    IspDebugTag (&rTags)[TagID_Total_Num]
)
{
    enum { E_NUM = ISP_xxx_T::COUNT };
    ISP_xxx_T param;

    ISP_MGR_T::getInstance(eSensorDev).get(param, pReg);

    for (MUINT32 i = 0; i < E_NUM; i++)
    {
        MUINT32 const u4TagID = TagID_begin + i;
        rTags[u4TagID].u4ID   = getIspTag(u4TagID);
        rTags[u4TagID].u4Val  = param.set[i];
        //MY_LOG("[%d]=0x%08X", i, rTags[u4TagID].u4Val);
    }
    STATIC_CHECK(
        TagID_begin+E_NUM-1 < TagID_Total_Num,
        tag_index_over_total_num
    );
}

#if 0
//  Tag <- Isp Regs.
template <class ISP_xxx_T, MUINT32 TagID_begin>
inline
MVOID
setIspTags(
    ESensorDev_T eSensorDev,
    ESensorTG_T eSensorTG,
    MBOOL fgReadFromHW,
    IspDebugTag (&rTags)[TagID_Total_Num]
)
{
    enum { E_NUM = ISP_xxx_T::COUNT };
    ISP_xxx_T param;

    if (fgReadFromHW) {
        getIspReg(eSensorDev, eSensorTG, param);
    }
    else {
        getIspHWBuf(eSensorDev, eSensorTG, param);
    }

    for (MUINT32 i = 0; i < E_NUM; i++)
    {
        MUINT32 const u4TagID = TagID_begin + i;
        rTags[u4TagID].u4ID   = getIspTag(u4TagID);
        rTags[u4TagID].u4Val  = param.set[i];
        //MY_LOG("[%d]=0x%08X", i, rTags[u4TagID].u4Val);
    }
    STATIC_CHECK(
        TagID_begin+E_NUM-1 < TagID_Total_Num,
        tag_index_over_total_num
    );
}
#endif
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MERROR_ENUM
Paramctrl::
getDebugInfoP1(NSIspExifDebug::IspExifDebugInfo_T& rDebugInfo, MBOOL const fgReadFromHW) const
{
    Mutex::Autolock lock(m_Lock);

    ////////////////////////////////////////////////////////////////////////////
    //  (1) Header.
    ////////////////////////////////////////////////////////////////////////////
    rDebugInfo.hdr  = g_rIspExifDebugInfoHdr;

    ////////////////////////////////////////////////////////////////////////////
    //  (2) Body.
    ////////////////////////////////////////////////////////////////////////////
    IspDebugTag (&rTags)[TagID_Total_Num] = rDebugInfo.debugInfo.tags;

    // ISP debug tag version
    setIspIdx(rTags, IspTagVersion, IspDebugTagVersion);

    ////////////////////////////////////////////////////////////////////////////
    //  (2.1) ISPRegs
    ////////////////////////////////////////////////////////////////////////////

    // CAM_CTL_EN_P1
    MY_LOG_IF(m_bDebugEnable,"CAM_CTL_EN_P1:");
    setIspTags<ISP_NVRAM_CTL_EN_P1_T, CAM_CTL_EN_P1_Begin>(m_eSensorDev, fgReadFromHW, rTags);

    // DBS
    MY_LOG_IF(m_bDebugEnable,"DBS:");
    setIspIdx(rTags, IDX_DBS, m_IspNvramMgr.getIdx_DBS());
    setIspTags<ISP_NVRAM_DBS_T, CAM_DBS_Begin>(m_eSensorDev, fgReadFromHW, rTags);

    // OBC
    MY_LOG_IF(m_bDebugEnable,"OBC:");
    setIspIdx(rTags, IDX_OBC, m_IspNvramMgr.getIdx_OBC());
    setIspTags<ISP_NVRAM_OBC_T, CAM_OBC_Begin>(m_eSensorDev, fgReadFromHW, rTags);

    // BPC
    MY_LOG_IF(m_bDebugEnable,"BPC:");
    setIspIdx(rTags, IDX_BPC, m_IspNvramMgr.getIdx_BPC());
    setIspTags<ISP_NVRAM_BNR_BPC_T, CAM_BPC_Begin>(m_eSensorDev, fgReadFromHW, rTags);

    // NR1
    MY_LOG_IF(m_bDebugEnable,"NR1:");
    setIspIdx(rTags, IDX_NR1, m_IspNvramMgr.getIdx_NR1());
    setIspTags<ISP_NVRAM_BNR_NR1_T, CAM_NR1_Begin>(m_eSensorDev, fgReadFromHW, rTags);

    // PDC
    MY_LOG_IF(m_bDebugEnable,"PDC:");
    setIspIdx(rTags, IDX_PDC, m_IspNvramMgr.getIdx_PDC());
    setIspTags<ISP_NVRAM_BNR_PDC_T, CAM_PDC_Begin>(m_eSensorDev, fgReadFromHW, rTags);

    // RMM
    MY_LOG_IF(m_bDebugEnable,"RMM:");
    setIspIdx(rTags, IDX_RMM, m_IspNvramMgr.getIdx_RMM());
    setIspTags<ISP_NVRAM_RMM_T, CAM_RMM_Begin>(m_eSensorDev, fgReadFromHW, rTags);

    // LSC
    MY_LOG_IF(m_bDebugEnable,"LSC:");
    setIspTags<ISP_NVRAM_LSC_T, CAM_LSC_Begin>(m_eSensorDev, fgReadFromHW, rTags);

    // RPG
    MY_LOG_IF(m_bDebugEnable,"RPG:");
    setIspTags<ISP_NVRAM_RPG_T, CAM_RPG_Begin>(m_eSensorDev, fgReadFromHW, rTags);


#warning "FIXME: shoudn't be in P1"
    ISP_CCM_WEIGHT_T CCM_Weight = m_pCcmMgr->getCCMWeight();
    setIspIdx(rTags, SMOOTH_CCM, m_rIspParam.bInvokeSmoothCCM);
    setIspIdx(rTags, CCM_Weight_Strobe, CCM_Weight.i4Strobe);
    setIspIdx(rTags, CCM_Weight_A, CCM_Weight.i4A);
    setIspIdx(rTags, CCM_Weight_TL84, CCM_Weight.i4TL84);
    setIspIdx(rTags, CCM_Weight_CWF, CCM_Weight.i4CWF);
    setIspIdx(rTags, CCM_Weight_D65, CCM_Weight.i4D65);
    setIspIdx(rTags, CCM_Weight_RSV1, CCM_Weight.i4RSV1);
    setIspIdx(rTags, CCM_Weight_RSV2, CCM_Weight.i4RSV2);
    setIspIdx(rTags, CCM_Weight_RSV3, CCM_Weight.i4RSV3);

    return  MERR_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MERROR_ENUM
Paramctrl::
getDebugInfo(const NSIspTuning::ISP_INFO_T& rIspInfo, NSIspExifDebug::IspExifDebugInfo_T& rDebugInfo, const void* pRegBuf) const
{
    Mutex::Autolock lock(m_Lock);
    const MBOOL fgReadFromHW = MFALSE;
    ////////////////////////////////////////////////////////////////////////////
    //  (1) Header.
    ////////////////////////////////////////////////////////////////////////////
    rDebugInfo.hdr  = g_rIspExifDebugInfoHdr;

    ////////////////////////////////////////////////////////////////////////////
    //  (2) Body.
    ////////////////////////////////////////////////////////////////////////////
    IspDebugTag (&rTags)[TagID_Total_Num] = rDebugInfo.debugInfo.tags;
    const NSIspTuning::RAWIspCamInfo& rCamInfo = rIspInfo.rCamInfo;

    // ISP debug tag version
    setIspIdx(rTags, IspTagVersion, IspDebugTagVersion);
    setIspIdx(rTags, UniqueKey, rIspInfo.i4UniqueKey);
    ////////////////////////////////////////////////////////////////////////////
    //  (2.1) ISPRegs
    ////////////////////////////////////////////////////////////////////////////
    INDEX_T const*const pDefaultIndex = m_pIspTuningCustom->getDefaultIndex(
            rCamInfo.eIspProfile, rCamInfo.eSensorMode, rCamInfo.eIdx_Scene, rCamInfo.eIdx_ISO);
    if  ( ! pDefaultIndex )
    {
        MY_ERR("[ERROR][getDebugInfo]pDefaultIndex==NULL");
        return MERR_CUSTOM_DEFAULT_INDEX_NOT_FOUND;
    }
    IndexMgr idxmgr = *pDefaultIndex;

    // CAM_CTL_EN_P2
    MY_LOG_IF(m_bDebugEnable,"CAM_CTL_EN_P2:");
    setIspTags2<ISP_NVRAM_CTL_EN_P2_T, ISP_MGR_CTL_EN_P2_T, CAM_CTL_EN_P2_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);

    // DBS2
    MY_LOG_IF(m_bDebugEnable,"DBS2:");
    setIspIdx(rTags, IDX_DBS2, idxmgr.getIdx_DBS());
    setIspTags2<ISP_NVRAM_DBS_T, ISP_MGR_DBS2_T, DIP_X_DBS2_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);

    // OBC2
    MY_LOG_IF(m_bDebugEnable,"OBC2:");
    setIspIdx(rTags, IDX_OBC2, idxmgr.getIdx_OBC());
    setIspTags2<ISP_NVRAM_OBC_T, ISP_MGR_OBC2_T, DIP_X_OBC2_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);

    // BPC2
    MY_LOG_IF(m_bDebugEnable,"BPC2:");
    setIspIdx(rTags, IDX_BPC2, idxmgr.getIdx_BPC());
    setIspTags2<ISP_NVRAM_BNR_BPC_T, ISP_MGR_BNR2_T, DIP_X_BPC2_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);

    // NR12
    MY_LOG_IF(m_bDebugEnable,"NR12:");
    setIspIdx(rTags, IDX_NR12, idxmgr.getIdx_NR1());
    setIspTags2<ISP_NVRAM_BNR_NR1_T, ISP_MGR_BNR2_T, DIP_X_NR12_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);

    // PDC2
    MY_LOG_IF(m_bDebugEnable,"PDC2:");
    setIspIdx(rTags, IDX_PDC2, idxmgr.getIdx_PDC());
    setIspTags2<ISP_NVRAM_BNR_PDC_T, ISP_MGR_BNR2_T, DIP_X_PDC2_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);

    // RMM2
    MY_LOG_IF(m_bDebugEnable,"PDC2:");
    setIspIdx(rTags, IDX_RMM2, idxmgr.getIdx_RMM());
    setIspTags2<ISP_NVRAM_RMM_T, ISP_MGR_RMM2_T, DIP_X_RMM2_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);

    // LSC2
    MY_LOG_IF(m_bDebugEnable,"LSC2:");
    setIspTags2<ISP_NVRAM_LSC_T, ISP_MGR_LSC2_T, DIP_X_LSC2_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);

    // PGN
    MY_LOG_IF(m_bDebugEnable,"PGN:");
    setIspTags2<ISP_NVRAM_PGN_T, ISP_MGR_PGN_T, DIP_X_PGN_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);

    // SL2
    MY_LOG_IF(m_bDebugEnable,"SL2:");
    setIspIdx(rTags, IDX_SL2, idxmgr.getIdx_SL2());
    setIspTags2<ISP_NVRAM_SL2_GET_T,  ISP_MGR_SL2_T, DIP_X_SL2_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);
    setIspTags2<ISP_NVRAM_SL2B_GET_T, ISP_MGR_SL2_T, DIP_X_SL2B_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);
    setIspTags2<ISP_NVRAM_SL2C_GET_T, ISP_MGR_SL2_T, DIP_X_SL2C_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);
    setIspTags2<ISP_NVRAM_SL2D_GET_T, ISP_MGR_SL2_T, DIP_X_SL2D_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);
    setIspTags2<ISP_NVRAM_SL2E_GET_T, ISP_MGR_SL2_T, DIP_X_SL2E_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);

    // UDM
    MY_LOG_IF(m_bDebugEnable,"UDM:");
    setIspIdx(rTags, IDX_UDM, idxmgr.getIdx_UDM());
    setIspIdx(rTags, UDM_UPPER_ISO, rCamInfo.rIspIntInfo.u4UdmUpperISO);
    setIspIdx(rTags, UDM_LOWER_ISO, rCamInfo.rIspIntInfo.u4UdmLowerISO);
    setIspIdx(rTags, UDM_UPPER_IDX, rCamInfo.rIspIntInfo.u2UdmUpperIdx);
    setIspIdx(rTags, UDM_LOWER_IDX, rCamInfo.rIspIntInfo.u2UdmLowerIdx);
    setIspTags2<ISP_NVRAM_UDM_T, ISP_MGR_UDM_T, DIP_X_UDM_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);

    // G2G
    MY_LOG_IF(m_bDebugEnable,"G2G:");
    setIspTags2<ISP_NVRAM_CCM_T, ISP_MGR_CCM_T, DIP_X_G2G_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);

#warning "FIXME: should be back to P2"
#if 0
    ISP_CCM_WEIGHT_T CCM_Weight = m_pCcmMgr->getCCMWeight();
    setIspIdx(rTags, SMOOTH_CCM, m_rIspParam.bInvokeSmoothCCM);
    setIspIdx(rTags, CCM_Weight_Strobe, CCM_Weight.i4Strobe);
    setIspIdx(rTags, CCM_Weight_A, CCM_Weight.i4A);
    setIspIdx(rTags, CCM_Weight_TL84, CCM_Weight.i4TL84);
    setIspIdx(rTags, CCM_Weight_CWF, CCM_Weight.i4CWF);
    setIspIdx(rTags, CCM_Weight_D65, CCM_Weight.i4D65);
    setIspIdx(rTags, CCM_Weight_RSV1, CCM_Weight.i4RSV1);
    setIspIdx(rTags, CCM_Weight_RSV2, CCM_Weight.i4RSV2);
    setIspIdx(rTags, CCM_Weight_RSV3, CCM_Weight.i4RSV3);
#endif

    // G2G_CFC
    MY_LOG_IF(m_bDebugEnable,"G2G_CFC:");
    setIspTags2<ISP_NVRAM_CCM_CFC_T, ISP_MGR_CCM_T, DIP_X_G2G_CFC_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);

    // GGM
    setIspIdx(rTags, IDX_GGM, idxmgr.getIdx_GGM());

    rDebugInfo.GGMTableInfo.u4TableSize = 288;
    ISP_NVRAM_GGM_T& rGGM = *(reinterpret_cast<ISP_NVRAM_GGM_T*>(rDebugInfo.GGMTableInfo.GGM));
    ISP_MGR_GGM_T::getInstance(m_eSensorDev).get(rGGM, static_cast<const dip_x_reg_t*>(pRegBuf));

    // G2C
    MY_LOG_IF(m_bDebugEnable,"G2C:");
    setIspTags2<ISP_NVRAM_G2C_T, ISP_MGR_G2C_T, DIP_X_G2C_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);

    // ANR
    MY_LOG_IF(m_bDebugEnable,"ANR:");
    setIspIdx(rTags, IDX_ANR, idxmgr.getIdx_ANR());
    setIspIdx(rTags, ANR_UPPER_ISO, rCamInfo.rIspIntInfo.u4AnrUpperISO);
    setIspIdx(rTags, ANR_LOWER_ISO, rCamInfo.rIspIntInfo.u4AnrLowerISO);
    setIspIdx(rTags, ANR_UPPER_IDX, rCamInfo.rIspIntInfo.u2AnrUpperIdx);
    setIspIdx(rTags, ANR_LOWER_IDX, rCamInfo.rIspIntInfo.u2AnrLowerIdx);
    setIspTags2<ISP_NVRAM_ANR_T, ISP_MGR_NBC_T, DIP_X_ANR_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);

    // ANR2
    MY_LOG_IF(m_bDebugEnable,"ANR:");
    setIspIdx(rTags, IDX_ANR2, idxmgr.getIdx_ANR2());
    setIspIdx(rTags, ANR2_UPPER_ISO, rCamInfo.rIspIntInfo.u4AnrUpperISO);
    setIspIdx(rTags, ANR2_LOWER_ISO, rCamInfo.rIspIntInfo.u4AnrLowerISO);
    setIspIdx(rTags, ANR2_UPPER_IDX, rCamInfo.rIspIntInfo.u2Anr2UpperIdx);
    setIspIdx(rTags, ANR2_LOWER_IDX, rCamInfo.rIspIntInfo.u2Anr2LowerIdx);
    setIspTags2<ISP_NVRAM_ANR2_T, ISP_MGR_NBC2_T, DIP_X_ANR2_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);

    // CCR
    MY_LOG_IF(m_bDebugEnable,"CCR:");
    setIspIdx(rTags, IDX_CCR, idxmgr.getIdx_CCR());
    setIspTags2<ISP_NVRAM_CCR_T, ISP_MGR_NBC2_T, DIP_X_CCR_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);

    // BOK
    MY_LOG_IF(m_bDebugEnable,"BOK:");
    setIspIdx(rTags, IDX_BOK, idxmgr.getIdx_BOK());
    setIspTags2<ISP_NVRAM_BOK_T, ISP_MGR_NBC2_T, DIP_X_BOK_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);

    // PCA
    MY_LOG_IF(m_bDebugEnable,"PCA:");
    setIspIdx(rTags, IDX_PCA, m_pPcaMgr->getIdx());
    setIspIdx(rTags, PCA_SLIDER, m_pPcaMgr->getSliderValue());
    setIspTags2<ISP_NVRAM_PCA_T, ISP_MGR_PCA_T, DIP_X_PCA_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);

    rDebugInfo.PCATableInfo.u4TableSize = 180;
    ISP_MGR_PCA_T::getInstance(m_eSensorDev, m_ePCAMode).getLut(rDebugInfo.PCATableInfo.PCA);

    //  EE
    MY_LOG_IF(m_bDebugEnable,"EE:");
    setIspIdx(rTags, IDX_EE, idxmgr.getIdx_EE());
    setIspIdx(rTags, EE_UPPER_ISO, rCamInfo.rIspIntInfo.u4EEUpperISO);
    setIspIdx(rTags, EE_LOWER_ISO, rCamInfo.rIspIntInfo.u4EELowerISO);
    setIspIdx(rTags, EE_UPPER_IDX, rCamInfo.rIspIntInfo.u2EEUpperIdx);
    setIspIdx(rTags, EE_LOWER_IDX, rCamInfo.rIspIntInfo.u2EELowerIdx);
    setIspTags2<ISP_NVRAM_EE_T, ISP_MGR_SEEE_T, DIP_X_EE_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);

    // SE
    MY_LOG_IF(m_bDebugEnable,"SE:");
    setIspTags2<ISP_NVRAM_SE_T, ISP_MGR_SEEE_T, DIP_X_SE_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);

    // NR3D
    MY_LOG_IF(m_bDebugEnable,"NR3D:");
    setIspIdx(rTags, IDX_NR3D, idxmgr.getIdx_NR3D());
    setIspTags2<ISP_NVRAM_NR3D_T, ISP_MGR_NR3D_T, DIP_X_NR3D_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);

    // MFB
    MY_LOG_IF(m_bDebugEnable,"MFB:");
    setIspIdx(rTags, IDX_MFB, idxmgr.getIdx_MFB());
    setIspTags2<ISP_NVRAM_MFB_T, ISP_MGR_MFB_T, DIP_X_MFB_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);

    // MIXER3
    MY_LOG_IF(m_bDebugEnable,"MIXER3:");
    setIspTags2<ISP_NVRAM_MIXER3_T, ISP_MGR_MIXER3_T, DIP_X_MIXER3_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);

    // LCE
    MY_LOG_IF(m_bDebugEnable,"LCE:");
    setIspIdx(rTags, IDX_LCE, idxmgr.getIdx_LCE());
    setIspTags2<ISP_NVRAM_LCE_T, ISP_MGR_LCE_T, DIP_X_LCE_Begin>(m_eSensorDev, static_cast<const dip_x_reg_t*>(pRegBuf), rTags);

    //dynamic Gamma
    // adaptive Gamma
    MY_LOG_IF(m_bDebugEnable,"adaptive Gamma:");
    setIspIdx(rTags, DIP_X_GMA_GMAMode, m_GmaExifInfo.i4GMAMode);
    setIspIdx(rTags, DIP_X_GMA_SensorMode, m_GmaExifInfo.i4SensorMode);
    setIspIdx(rTags, DIP_X_GMA_EVRatio, m_GmaExifInfo.i4EVRatio);
    setIspIdx(rTags, DIP_X_GMA_LowContrastThr, m_GmaExifInfo.i4LowContrastThr);
    setIspIdx(rTags, DIP_X_GMA_EVLowContrastThr, m_GmaExifInfo.i4EVLowContrastThr);
    setIspIdx(rTags, DIP_X_GMA_Contrast, m_GmaExifInfo.i4Contrast);
    setIspIdx(rTags, DIP_X_GMA_ContrastY, m_GmaExifInfo.i4ContrastY);
    setIspIdx(rTags, DIP_X_GMA_EVContrastY, m_GmaExifInfo.i4EVContrastY);
    setIspIdx(rTags, DIP_X_GMA_ContrastWeight, m_GmaExifInfo.i4ContrastWeight);
    setIspIdx(rTags, DIP_X_GMA_LV, m_GmaExifInfo.i4LV);
    setIspIdx(rTags, DIP_X_GMA_LVWeight, m_GmaExifInfo.i4LVWeight);
    setIspIdx(rTags, DIP_X_GMA_SmoothEnable, m_GmaExifInfo.i4SmoothEnable);
    setIspIdx(rTags, DIP_X_GMA_SmoothSpeed, m_GmaExifInfo.i4SmoothSpeed);
    setIspIdx(rTags, DIP_X_GMA_SmoothWaitAE, m_GmaExifInfo.i4SmoothWaitAE);
    setIspIdx(rTags, DIP_X_GMA_GMACurveEnable, m_GmaExifInfo.i4GMACurveEnable);
    setIspIdx(rTags, DIP_X_GMA_CenterPt, m_GmaExifInfo.i4CenterPt);
    setIspIdx(rTags, DIP_X_GMA_LowCurve, m_GmaExifInfo.i4LowCurve);
    setIspIdx(rTags, DIP_X_GMA_SlopeL, m_GmaExifInfo.i4SlopeL);
    setIspIdx(rTags, DIP_X_GMA_FlareEnable, m_GmaExifInfo.i4FlareEnable);
    setIspIdx(rTags, DIP_X_GMA_FlareOffset, m_GmaExifInfo.i4FlareOffset);


    ////////////////////////////////////////////////////////////////////////////
    //  (2.2) ISPComm
    ////////////////////////////////////////////////////////////////////////////
    for (MUINT32 i = 0; i < sizeof(ISP_NVRAM_COMMON_STRUCT)/sizeof(MUINT32); i++)
    {
        MUINT32 const u4TagID = COMM_Begin + i;
        rTags[u4TagID].u4ID   = getIspTag(u4TagID);
        rTags[u4TagID].u4Val  = m_rIspComm.CommReg[i];
    }

    //  (2.3) RAWIspCamInfo
    MY_LOG_IF(m_bDebugEnable,"RAWIspCamInfo:");
    setIspIdx(rTags, IspProfile, rCamInfo.eIspProfile);
    setIspIdx(rTags, SensorMode, rCamInfo.eSensorMode);
    setIspIdx(rTags, SceneIdx, rCamInfo.eIdx_Scene);
    setIspIdx(rTags, ISOValue, rCamInfo.u4ISOValue);
    setIspIdx(rTags, ISOIdx, rCamInfo.eIdx_ISO);
    setIspIdx(rTags, ShadingIdx, rCamInfo.eIdx_Shading_CCT);
    setIspIdx(rTags, ZoomRatio_x100, rCamInfo.i4ZoomRatio_x100);
    setIspIdx(rTags, LightValue_x10, rCamInfo.i4LightValue_x10);
    setIspIdx(rTags, SwnrEncEnableIsoThreshold, m_u4SwnrEncEnableIsoThreshold);
    //
    //  (2.4) EffectMode
    MY_LOG_IF(m_bDebugEnable,"EffectMode:");
    setIspIdx(rTags, EffectMode, rCamInfo.eIdx_Effect);
    //
    //  (2.5) UserSelectLevel
    MY_LOG_IF(m_bDebugEnable,"UserSelectLevel:");
    setIspIdx(rTags, EdgeIdx, rCamInfo.rIspUsrSelectLevel.eIdx_Edge);
    setIspIdx(rTags, HueIdx, rCamInfo.rIspUsrSelectLevel.eIdx_Hue);
    setIspIdx(rTags, SatIdx, rCamInfo.rIspUsrSelectLevel.eIdx_Sat);
    setIspIdx(rTags, BrightIdx, rCamInfo.rIspUsrSelectLevel.eIdx_Bright);
    setIspIdx(rTags, ContrastIdx, rCamInfo.rIspUsrSelectLevel.eIdx_Contrast);

    return  MERR_OK;
}

