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
#define LOG_TAG "paramctrl_per_frame"

#ifndef ENABLE_MY_LOG
    #define ENABLE_MY_LOG       (1)
#endif

#include <cutils/properties.h>
#include <aaa_types.h>
#include <aaa_log.h>
#include <aaa_error_code.h>
//#include <mtkcam/hal/aaa/aaa_hal_if.h>
//#include <mtkcam/hal/aaa/aaa_hal.h>
#include <camera_custom_nvram.h>
#include <awb_param.h>
#include <isp_tuning.h>
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
#include <lib3a/dynamic_ccm.h>
#include <lib3a/isp_interpolation.h>
#include <ccm_mgr.h>
#include <ggm_mgr.h>
#include <gma_mgr.h>
#include <ILscMgr.h>
#include "paramctrl_if.h"
#include "paramctrl.h"
#include <hwutils/CameraProfile.h>
//#include "vfb_hal_base.h"
//#include <mtkcam/featureio/capturenr.h>

//define log control
#define EN_3A_FLOW_LOG        1
#define EN_3A_SCHEDULE_LOG    2

using namespace android;
using namespace NS3Av3;
using namespace NSIspTuning;
using namespace NSIspTuningv3;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
applyToHw_PerFrame_All(RequestSet_T const RequestSet, MINT32 i4SubsampleIdex)
{
    MBOOL fgRet = MTRUE;
    MINT32 i4FrameID = RequestSet.vNumberSet[0];

    MY_LOG_IF(m_bDebugEnable, "[%s] + i4FrameID(%d), i4SubsampleIdex(%d)", __FUNCTION__, i4FrameID, i4SubsampleIdex);

    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("debug.aaa.pvlog.enable", value, "0");
    int r3ALogEnable = atoi(value);
    AaaTimer localTimer("applyToHwAll", m_eSensorDev, (r3ALogEnable & EN_3A_SCHEDULE_LOG));

    MINT32 i4Magic = i4FrameID;
    MINT32 i4NumInRequestSet;
    RequestSet_T RequestTuningSet =  RequestSet;
    m_pTuning->dequeBuffer(RequestTuningSet.vNumberSet.data());

//if (RequestSet.vNumberSet[0] == 1)
{
    for (MINT32 i4InitNum = 0; i4InitNum < m_i4SubsampleCount; i4InitNum++)
    {
        fgRet = ISP_MGR_AE_STAT_CONFIG::getInstance(m_eSensorDev).apply(*m_pTuning, i4InitNum)
            &&  ISP_MGR_AWB_STAT_CONFIG::getInstance(m_eSensorDev).apply(*m_pTuning, i4InitNum)
            &&  ISP_MGR_AF_STAT_CONFIG_T::getInstance(m_eSensorDev).apply(*m_pTuning, i4Magic, i4InitNum)
        //        &&  ISP_MGR_FLK_CONFIG_T::getInstance(m_eSensorDev).apply(*m_pTuning)
            &&  ISP_MGR_DBS_T::getInstance(m_eSensorDev).apply(m_rIspCamInfo.eIspProfile, *m_pTuning, i4InitNum)
            &&  ISP_MGR_OBC_T::getInstance(m_eSensorDev).apply(m_rIspCamInfo.eIspProfile, *m_pTuning, i4InitNum)
            &&  ISP_MGR_BNR_T::getInstance(m_eSensorDev).apply(m_rIspCamInfo.eIspProfile, *m_pTuning, i4InitNum)
            &&  ISP_MGR_RMM_T::getInstance(m_eSensorDev).apply(m_rIspCamInfo, *m_pTuning, i4InitNum)
            &&  ISP_MGR_LSC_T::getInstance(m_eSensorDev).apply(m_rIspCamInfo.eIspProfile, *m_pTuning, i4InitNum)
            &&  ISP_MGR_RPG_T::getInstance(m_eSensorDev).apply(m_rIspCamInfo.eIspProfile, *m_pTuning, i4InitNum)
            &&  ISP_MGR_RMG_T::getInstance(m_eSensorDev).apply(m_rIspCamInfo, *m_pTuning, i4InitNum)
            ;
    }
}
/*else
{
    fgRet = ISP_MGR_AE_STAT_CONFIG::getInstance(m_eSensorDev).apply(*m_pTuning, i4SubsampleIdex)
        &&  ISP_MGR_AWB_STAT_CONFIG::getInstance(m_eSensorDev).apply(*m_pTuning, i4SubsampleIdex)
        &&  ISP_MGR_AF_STAT_CONFIG_T::getInstance(m_eSensorDev).apply(*m_pTuning, i4Magic, i4SubsampleIdex)
    //        &&  ISP_MGR_FLK_CONFIG_T::getInstance(m_eSensorDev).apply(*m_pTuning)
        &&  ISP_MGR_DBS_T::getInstance(m_eSensorDev).apply(m_rIspCamInfo.eIspProfile, *m_pTuning, i4SubsampleIdex)
        &&  ISP_MGR_OBC_T::getInstance(m_eSensorDev).apply(m_rIspCamInfo.eIspProfile, *m_pTuning, i4SubsampleIdex)
        &&  ISP_MGR_BNR_T::getInstance(m_eSensorDev).apply(m_rIspCamInfo.eIspProfile, *m_pTuning, i4SubsampleIdex)
        &&  ISP_MGR_RMM_T::getInstance(m_eSensorDev).apply(m_rIspCamInfo, *m_pTuning, i4SubsampleIdex)
        &&  ISP_MGR_LSC_T::getInstance(m_eSensorDev).apply(m_rIspCamInfo.eIspProfile, *m_pTuning, i4SubsampleIdex)
        &&  ISP_MGR_RPG_T::getInstance(m_eSensorDev).apply(m_rIspCamInfo.eIspProfile, *m_pTuning, i4SubsampleIdex)
        &&  ISP_MGR_RMG_T::getInstance(m_eSensorDev).apply(m_rIspCamInfo, *m_pTuning, i4SubsampleIdex)
        ;
}*/


    m_pTuning->enqueBuffer();

    localTimer.End();

    return fgRet;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

MBOOL
Paramctrl::
applyToHw_PerFrame_P2(MINT32 flowType, const ISP_INFO_T& rIspInfo, void* pRegBuf)
{
    MBOOL fgRet = MTRUE;

    MY_LOG_IF(m_bDebugEnable, "[%s] + pRegBuf(%p)", __FUNCTION__, pRegBuf);

    const RAWIspCamInfo& rIspCamInfo = rIspInfo.rCamInfo;
    dip_x_reg_t* pReg = reinterpret_cast<dip_x_reg_t*>(pRegBuf);
    ::memset((void*)pReg, 0, sizeof(dip_x_reg_t));

    fgRet = MTRUE
        &&  ISP_MGR_DBS2_T::getInstance(m_eSensorDev).apply(rIspCamInfo.eIspProfile, pReg)
        &&  ISP_MGR_OBC2_T::getInstance(m_eSensorDev).apply(rIspCamInfo.eIspProfile, pReg)
        &&  ISP_MGR_BNR2_T::getInstance(m_eSensorDev).apply(rIspCamInfo.eIspProfile, pReg)
        &&  ISP_MGR_LSC2_T::getInstance(m_eSensorDev).apply(rIspCamInfo.eIspProfile, pReg)
        &&  ISP_MGR_SL2_T::getInstance(m_eSensorDev).apply(rIspCamInfo.eIspProfile, pReg)
        &&  ISP_MGR_PGN_T::getInstance(m_eSensorDev).apply(rIspCamInfo.eIspProfile, pReg)
        &&  ISP_MGR_UDM_T::getInstance(m_eSensorDev).apply(rIspCamInfo.eIspProfile, pReg)
        &&  ISP_MGR_CCM_T::getInstance(m_eSensorDev).apply(rIspCamInfo.eIspProfile, pReg)
        &&  ISP_MGR_GGM_T::getInstance(m_eSensorDev).apply(rIspCamInfo.eIspProfile, pReg)
        &&  ISP_MGR_G2C_T::getInstance(m_eSensorDev).apply(rIspCamInfo.eIspProfile, pReg)
        &&  ISP_MGR_G2C_SHADE_T::getInstance(m_eSensorDev).apply(rIspCamInfo.eIspProfile, pReg)
        &&  ISP_MGR_LCE_T::getInstance(m_eSensorDev).apply(rIspCamInfo.eIspProfile, pReg)
        &&  ISP_MGR_NBC_T::getInstance(m_eSensorDev).apply(rIspCamInfo.eIspProfile, pReg)
        &&  ISP_MGR_NBC2_T::getInstance(m_eSensorDev).apply(rIspCamInfo.eIspProfile, pReg)
        &&  ISP_MGR_PCA_T::getInstance(m_eSensorDev, m_ePCAMode).apply(rIspCamInfo.eIspProfile, pReg)
        &&  ISP_MGR_SEEE_T::getInstance(m_eSensorDev).apply(rIspCamInfo.eIspProfile, pReg)
        &&  ISP_MGR_NR3D_T::getInstance(m_eSensorDev).apply(rIspCamInfo.eIspProfile, pReg)
        &&  ISP_MGR_MFB_T::getInstance(m_eSensorDev).apply(rIspCamInfo.eIspProfile, pReg)
        &&  ISP_MGR_MIXER3_T::getInstance(m_eSensorDev).apply(rIspCamInfo.eIspProfile, pReg)
        //&&  ISP_MGR_RMM2_T::getInstance(m_eSensorDev).apply(rIspCamInfo, pReg)
        //&&  ISP_MGR_RMG2_T::getInstance(m_eSensorDev).apply(rIspCamInfo, pReg)
        ;

    // the protection of HW limitations
    ispHWLimitSet(pReg);   // the protection of HW limitations

    MY_LOG_IF(m_bDebugEnable, "[%s] -", __FUNCTION__);
    return fgRet;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_All()
{
    MBOOL fgRet = MTRUE;

    //CPTLog(Event_Pipe_3A_ISP_VALIDATE_PERFRAME_PREPARE, CPTFlagStart); // Profiling Start.

    //  (1) reset: read register setting to ispmgr
    fgRet = MTRUE
        &&  ISP_MGR_DBS_T::getInstance(m_eSensorDev).reset()
        &&  ISP_MGR_OBC_T::getInstance(m_eSensorDev).reset()
        &&  ISP_MGR_BNR_T::getInstance(m_eSensorDev).reset()
        //&&  ISP_MGR_RMM_T::getInstance(m_eSensorDev).reset()
        //&&  ISP_MGR_RMG_T::getInstance(m_eSensorDev).reset()
        //&&  ISP_MGR_LCE_T::getInstance(m_eSensorDev).reset()
        //&&  ISP_MGR_UDM_T::getInstance(m_eSensorDev).reset()
        //&&  ISP_MGR_CCM_T::getInstance(m_eSensorDev).reset()
        //&&  ISP_MGR_GGM_T::getInstance(m_eSensorDev).reset()
        //&&  ISP_MGR_G2C_T::getInstance(m_eSensorDev).reset()
        //&&  ISP_MGR_NBC_T::getInstance(m_eSensorDev).reset()
        //&&  ISP_MGR_NBC2_T::getInstance(m_eSensorDev).reset()
        //&&  ISP_MGR_SEEE_T::getInstance(m_eSensorDev).reset()
        ;

    if  ( ! fgRet )
    {
        MY_ERR("reset error");
        goto lbExit;
    }

    //  (3) prepare something and fill buffers.
    fgRet = MTRUE
        &&  prepareHw_PerFrame_DBS()
        &&  prepareHw_PerFrame_OBC()
        &&  prepareHw_PerFrame_BPC()
        &&  prepareHw_PerFrame_NR1()
        &&  prepareHw_PerFrame_PDC()
        &&  prepareHw_PerFrame_RMM()
        &&  prepareHw_PerFrame_LSC()
        &&  prepareHw_PerFrame_RPG()
        //pass2
        &&  prepareHw_PerFrame_CCM()
        &&  prepareHw_PerFrame_GGM()

        ;

    //CPTLog(Event_Pipe_3A_ISP_VALIDATE_PERFRAME_PREPARE, CPTFlagEnd);   // Profiling End.

    if  ( ! fgRet )
    {
        MY_ERR("prepareHw error");
        goto lbExit;
    }

lbExit:
    return  fgRet;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_All(const ISP_INFO_T& rIspInfo, const IndexMgr& rIdx)
{
    MY_LOG_IF(m_bDebugEnable, "[%s] +", __FUNCTION__);

    MBOOL fgRet = MTRUE
        &&  prepareHw_PerFrame_DBS_2(rIspInfo.rCamInfo, rIdx)
        &&  prepareHw_PerFrame_OBC_2(rIspInfo.rCamInfo, rIdx)
        &&  prepareHw_PerFrame_BPC_2(rIspInfo.rCamInfo, rIdx)
        &&  prepareHw_PerFrame_NR1_2(rIspInfo.rCamInfo, rIdx)
        &&  prepareHw_PerFrame_PDC_2(rIspInfo.rCamInfo, rIdx)
        &&  prepareHw_PerFrame_LSC_2(rIspInfo)
        &&  prepareHw_PerFrame_PGN(rIspInfo.rCamInfo, rIdx)
        &&  prepareHw_PerFrame_SL2(rIspInfo, rIdx)
        &&  prepareHw_PerFrame_UDM(const_cast<RAWIspCamInfo&>(rIspInfo.rCamInfo), rIdx)
        &&  prepareHw_PerFrame_CCM(rIspInfo.rCamInfo, rIdx)
        &&  prepareHw_PerFrame_GGM(rIspInfo.rCamInfo, rIdx)
        &&  prepareHw_PerFrame_ANR(const_cast<RAWIspCamInfo&>(rIspInfo.rCamInfo), rIdx)
        &&  prepareHw_PerFrame_ANR2(const_cast<RAWIspCamInfo&>(rIspInfo.rCamInfo), rIdx)
        &&  prepareHw_PerFrame_CCR(rIspInfo.rCamInfo, rIdx)
        &&  prepareHw_PerFrame_BOK(rIspInfo.rCamInfo, rIdx)
        &&  prepareHw_PerFrame_PCA(rIspInfo.rCamInfo, rIdx)
        &&  prepareHw_PerFrame_EE(const_cast<RAWIspCamInfo&>(rIspInfo.rCamInfo), rIdx)
        &&  prepareHw_PerFrame_EFFECT(rIspInfo.rCamInfo, rIdx)
        &&  prepareHw_PerFrame_NR3D(rIspInfo.rCamInfo, rIdx)
        &&  prepareHw_PerFrame_MFB(rIspInfo.rCamInfo, rIdx)
        &&  prepareHw_PerFrame_MIXER3(rIspInfo.rCamInfo, rIdx)
        &&  prepareHw_PerFrame_LCE(rIspInfo.rCamInfo, rIdx)
        ;

    MY_LOG_IF(m_bDebugEnable, "[%s] -", __FUNCTION__);

    return  fgRet;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// For dynamic bypass application
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_Partial()
{
    MBOOL fgRet = MTRUE;

    //CPTLog(Event_Pipe_3A_ISP_VALIDATE_PERFRAME_PREPARE, CPTFlagStart); // Profiling Start.

    //  (1) reset: read register setting to ispmgr
    fgRet = MTRUE
        &&  ISP_MGR_OBC_T::getInstance(m_eSensorDev).reset()
        &&  ISP_MGR_G2C_T::getInstance(m_eSensorDev).reset()
            ;

    //  Exception of dynamic CCM
    if(isDynamicCCM())
        fgRet &= ISP_MGR_CCM_T::getInstance(m_eSensorDev).reset();


    if  ( ! fgRet )
    {
        goto lbExit;
    }

    //  (3) prepare something and fill buffers.
    fgRet = MTRUE
        &&  prepareHw_DynamicBypass_OBC()
        &&  prepareHw_PerFrame_LSC()
        //&&  prepareHw_PerFrame_PGN()
            ;


    //Exception of dynamic CCM
    if(isDynamicCCM())
        fgRet &= prepareHw_PerFrame_CCM();


    //CPTLog(Event_Pipe_3A_ISP_VALIDATE_PERFRAME_PREPARE, CPTFlagEnd);   // Profiling End.

    if  ( ! fgRet )
    {
        goto lbExit;
    }

lbExit:
    MY_LOG_IF(m_bDebugEnable, "[prepareHw_PerFrame_Partial()] exit\n");
    return  fgRet;

}

MBOOL
Paramctrl::
prepareHw_PerFrame_Partial(const ISP_INFO_T& rIspInfo, const IndexMgr& rIdx)
{
    return MTRUE;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID
Paramctrl::
ispHWLimitSet(dip_x_reg_t* pReg)
{
    MBOOL bBNR2_EN = pReg->DIP_X_CTL_RGB_EN.Bits.BNR2_EN;
    MBOOL bBPC2_EN = pReg->DIP_X_BNR2_BPC_CON.Bits.BPC_EN;

    //Limit 1
    if (bBNR2_EN && (bBPC2_EN == 0))
    {
        pReg->DIP_X_CTL_RGB_EN.Bits.BNR2_EN = 0;
        pReg->DIP_X_BNR2_BPC_CON.Bits.BPC_EN = 0;
        pReg->DIP_X_BNR2_NR1_CON.Bits.NR1_CT_EN = 0;
        //pReg->DIP_X_BNR2_PDC_CON.Bits.PDC_EN = 0;

        MY_LOG("BNR2::apply warning: set (bBNR2_EN, bBPC2_EN) = (%d, %d, %d)", bBNR2_EN, bBPC2_EN);
    }

    MBOOL bSL2_EN  = pReg->DIP_X_CTL_RGB_EN.Bits.SL2_EN;

    //Limit 2
    if (!bSL2_EN)
    {

        pReg->DIP_X_CTL_YUV_EN.Bits.SL2B_EN = 0;
        pReg->DIP_X_CTL_YUV_EN.Bits.SL2C_EN = 0;
        pReg->DIP_X_CTL_YUV_EN.Bits.SL2D_EN = 0;
        pReg->DIP_X_CTL_YUV_EN.Bits.SL2E_EN = 0;

        pReg->DIP_X_UDM_SL_CTL.Bits.UDM_SL_EN = 0;
    }

    MBOOL bNBC_EN  = pReg->DIP_X_CTL_YUV_EN.Bits.NBC_EN;
    MBOOL bNBC2_EN = pReg->DIP_X_CTL_YUV_EN.Bits.NBC2_EN;
    MBOOL bSEEE_EN = pReg->DIP_X_CTL_YUV_EN.Bits.SEEE_EN;
    MBOOL bNR3D_EN = pReg->DIP_X_CTL_YUV_EN.Bits.NR3D_EN;

    //Limit 3
    if (!bNBC_EN){
        pReg->DIP_X_CTL_YUV_EN.Bits.SL2B_EN = 0;
    }

    //Limit 4
    if (!bNBC2_EN){
        pReg->DIP_X_CTL_YUV_EN.Bits.SL2C_EN = 0;
    }

#if 0
    //Limit 4
    if (!bSEEE_EN){
        pReg->DIP_X_CTL_YUV_EN.Bits.SL2D_EN = 0;
    }

    //Limit 4
    if (!bNR3D_EN){
        pReg->DIP_X_CTL_YUV_EN.Bits.SL2E_EN = 0;
    }
#endif


    MBOOL bSL2B_EN  = pReg->DIP_X_CTL_YUV_EN.Bits.SL2B_EN;
    MBOOL bSL2C_EN  = pReg->DIP_X_CTL_YUV_EN.Bits.SL2C_EN;
    MBOOL bSL2D_EN  = pReg->DIP_X_CTL_YUV_EN.Bits.SL2D_EN;
    MBOOL bSL2E_EN  = pReg->DIP_X_CTL_YUV_EN.Bits.SL2E_EN;

    //Limit 5
    if (bNBC_EN && (bSL2B_EN == 0) ){
        pReg->DIP_X_ANR_CON1.Bits.ANR_LCE_LINK = 0;
    }

    //Limit 6
    if (bNBC2_EN ){
        pReg->DIP_X_ANR2_CON1.Bits.ANR2_MODE = 0;
        if (bSL2C_EN == 0){
             pReg->DIP_X_ANR2_CON1.Bits.ANR2_LCE_LINK = 0;
        }
    }

    //Limit 7
    if (bSEEE_EN && (bSL2D_EN == 0) ){
        pReg->DIP_X_SEEE_SLNK_CTRL_2.Bits.SEEE_GLUT_LINK_EN = 0;
    }

    //Limit 8
    if (bNR3D_EN && (bSL2E_EN == 0) ){
        pReg->DIP_X_NR3D_ON_CON.Bits.NR3D_SL2_OFF = 1;
    }

}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID
Paramctrl::
prepareHw_PerFrame_Default()
{

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_DBS()
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.dbs.disable", value, "0"); // 0: enable, 1: disable
    MBOOL bDisable = atoi(value);
    ISP_MGR_DBS_T& rDbs = ISP_MGR_DBS_T::getInstance(m_eSensorDev);

    m_rIspCamInfo.fgCCTInfo.Bits.fgDBS = rDbs.isCCTEnable();


    if ((bDisable) || (!rDbs.isCCTEnable()) )
    {
        rDbs.setEnable(MFALSE);
    }
    else {
        //if (getOperMode() != EOperMode_Meta)
        rDbs.setEnable(MTRUE);

        // Get default NVRAM parameter
        ISP_NVRAM_DBS_T dbs = m_IspNvramMgr.getDBS();

        // Invoke callback for customers to modify.
        if  ( isDynamicTuning() )
        {   //  Dynamic Tuning: Enable
            m_pIspTuningCustom->refine_DBS(m_rIspCamInfo, m_IspNvramMgr, dbs);
        }

        // Load it to ISP manager buffer.
        rDbs.put(dbs);
    }

    return  MTRUE;

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_OBC()
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.obc.disable", value, "0"); // 0: enable, 1: disable
    MBOOL bDisable = atoi(value);
    ISP_MGR_OBC_T& rObc = ISP_MGR_OBC_T::getInstance(m_eSensorDev);

    m_rIspCamInfo.fgCCTInfo.Bits.fgOBC = rObc.isCCTEnable();

    if ((bDisable) || (!rObc.isCCTEnable()) )
    {
        rObc.setEnable(MFALSE);
    }
    else {
        //if (getOperMode() != EOperMode_Meta)
        rObc.setEnable(MTRUE);

        // Get default NVRAM parameter
        ISP_NVRAM_OBC_T obc = m_IspNvramMgr.getOBC();

        // Invoke callback for customers to modify.
        if  ( isDynamicTuning() )
        {   //  Dynamic Tuning: Enable
            m_pIspTuningCustom->refine_OBC(m_rIspCamInfo, m_IspNvramMgr, obc);
        }

        this->setPureOBCInfo(&obc);

        // Load it to ISP manager buffer.
        rObc.put(obc);
    }

    return  MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_DynamicBypass_OBC()
{
    // Get backup NVRAM parameter
    ISP_NVRAM_OBC_T obc;

    this->getPureOBCInfo(&obc);

    // Load it to ISP manager buffer.
    ISP_MGR_OBC_T::getInstance(m_eSensorDev).put(obc);
    return  MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_BPC()
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.bpc.disable", value, "0"); // 0: enable, 1: disable
    MBOOL bDisable = atoi(value);
    ISP_MGR_BNR_T& rBnr = ISP_MGR_BNR_T::getInstance(m_eSensorDev);

    m_rIspCamInfo.fgCCTInfo.Bits.fgBPC = rBnr.isCCTBPCEnable();

    if ((bDisable) || (!rBnr.isCCTBPCEnable()) )
    {
        rBnr.setBPCEnable(MFALSE);
    }
    else {
        //if (getOperMode() != EOperMode_Meta)
        rBnr.setBPCEnable(MTRUE);

        // Get default NVRAM parameter
        ISP_NVRAM_BNR_BPC_T bpc = m_IspNvramMgr.getBPC();

#if 0
        if (m_pIspTuningCustom->is_to_invoke_isp_interpolation(m_rIspCamInfo)) {
             if (m_rIspCamInfo.u4ISOValue >= m_pIspTuningCustom->map_ISO_index_to_value(m_rIspCamInfo.eIdx_ISO)) {
                 if (m_rIspCamInfo.eIdx_ISO < eIDX_ISO_3200) {
                    MUINT32 u4BPCUpperISO = m_pIspTuningCustom->map_ISO_index_to_value(static_cast<EIndex_ISO_T>(m_rIspCamInfo.eIdx_ISO+1));
                    MUINT32 u4BPCLowerISO = m_pIspTuningCustom->map_ISO_index_to_value(m_rIspCamInfo.eIdx_ISO);
                    MUINT16 u2BPCUpperIdx = getISPIndex_byISO(m_rIspCamInfo.eIdx_ISO+1).BPC;
                    MUINT16 u2BPCLowerIdx = getISPIndex_byISO(m_rIspCamInfo.eIdx_ISO).BPC;

                    MY_LOG_IF(m_bDebugEnable,"%s(): m_rIspCamInfo.u4ISOValue = %d, u4UpperISO = %d, u4LowerISO = %d, u2UpperISOIdx = %d, u2LowerISOIdx = %d\n",
                              __FUNCTION__, m_rIspCamInfo.u4ISOValue,
                              u4BPCUpperISO,
                              u4BPCLowerISO,
                              m_rIspCamInfo.eIdx_ISO+1,
                              m_rIspCamInfo.eIdx_ISO);

                    MY_LOG_IF(m_bDebugEnable,"%s ISP_NVRAM_BPC_TH1_T: Upper(%d, %x), Lower(%d, %x)"
                        , __FUNCTION__
                        , u2BPCUpperIdx, m_IspNvramMgr.getBPC(u2BPCUpperIdx).th1.val
                        , u2BPCLowerIdx, m_IspNvramMgr.getBPC(u2BPCLowerIdx).th1.val);

                    SmoothBPC(m_rIspCamInfo.u4ISOValue,  // Real ISO
                             u4BPCUpperISO, // Upper ISO
                             u4BPCLowerISO, // Lower ISO
                             m_IspNvramMgr.getBPC(u2BPCUpperIdx), // BPC settings for upper ISO
                             m_IspNvramMgr.getBPC(u2BPCLowerIdx), // BPC settings for lower ISO
                             bpc);  // Output

                 }
             }
             else {
                 if (m_rIspCamInfo.eIdx_ISO > eIDX_ISO_100) {
                    MUINT32 u4BPCUpperISO = m_pIspTuningCustom->map_ISO_index_to_value(m_rIspCamInfo.eIdx_ISO);
                    MUINT32 u4BPCLowerISO = m_pIspTuningCustom->map_ISO_index_to_value(static_cast<EIndex_ISO_T>(m_rIspCamInfo.eIdx_ISO-1));
                    MUINT16 u2BPCUpperIdx = getISPIndex_byISO(m_rIspCamInfo.eIdx_ISO).BPC;
                    MUINT16 u2BPCLowerIdx = getISPIndex_byISO(m_rIspCamInfo.eIdx_ISO-1).BPC;

                    MY_LOG_IF(m_bDebugEnable,"%s(): m_rIspCamInfo.u4ISOValue = %d, u4UpperISO = %d, u4LowerISO = %d, u2UpperISOIdx = %d, u2LowerISOIdx = %d\n",
                              __FUNCTION__, m_rIspCamInfo.u4ISOValue,
                              u4BPCUpperISO,
                              u4BPCLowerISO,
                              m_rIspCamInfo.eIdx_ISO,
                              m_rIspCamInfo.eIdx_ISO-1);

                    MY_LOG_IF(m_bDebugEnable,"%s ISP_NVRAM_BPC_TH1_T: Upper(%d, %x), Lower(%d, %x)"
                        , __FUNCTION__
                        , u2BPCUpperIdx, m_IspNvramMgr.getBPC(u2BPCUpperIdx).th1.val
                        , u2BPCLowerIdx, m_IspNvramMgr.getBPC(u2BPCLowerIdx).th1.val);

                    SmoothBPC(m_rIspCamInfo.u4ISOValue,  // Real ISO
                             u4BPCUpperISO, // Upper ISO
                             u4BPCLowerISO, // Lower ISO
                             m_IspNvramMgr.getBPC(u2BPCUpperIdx), // BPC settings for upper ISO
                             m_IspNvramMgr.getBPC(u2BPCLowerIdx), // BPC settings for lower ISO
                             bpc);  // Output

                 }
             }
         }

#endif

        // Invoke callback for customers to modify.
        if  ( isDynamicTuning() )
        {   //  Dynamic Tuning: Enable
            m_pIspTuningCustom->refine_BPC(m_rIspCamInfo, m_IspNvramMgr, bpc);
        }

        // Load it to ISP manager buffer.
        rBnr.put(bpc);
    }

    return  MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_NR1()
{

    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.nr1.disable", value, "0"); // 0: enable, 1: disable
    MBOOL bDisable = atoi(value);
    ISP_MGR_BNR_T& rBnr = ISP_MGR_BNR_T::getInstance(m_eSensorDev);

    m_rIspCamInfo.fgCCTInfo.Bits.fgNR1 = rBnr.isCCTCTEnable();

    if ((bDisable) ||
        (!rBnr.isCCTCTEnable()) ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_IHDR_Preview) ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_IHDR_Video)  ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_zHDR_Preview) ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_zHDR_Video)   ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_zHDR_Capture))
    {
        rBnr.setCTEnable(MFALSE);
    }
    else {
        //if (getOperMode() != EOperMode_Meta)
            rBnr.setCTEnable(MTRUE);

        // Get default NVRAM parameter
        ISP_NVRAM_BNR_NR1_T nr1 = m_IspNvramMgr.getNR1();

#if 0
        if (m_pIspTuningCustom->is_to_invoke_isp_interpolation(m_rIspCamInfo)) {
           if (m_rIspCamInfo.u4ISOValue >= m_pIspTuningCustom->map_ISO_index_to_value(m_rIspCamInfo.eIdx_ISO)) {
              if (m_rIspCamInfo.eIdx_ISO < eIDX_ISO_3200) {
                 MUINT32 u4NR1UpperISO = m_pIspTuningCustom->map_ISO_index_to_value(static_cast<EIndex_ISO_T>(m_rIspCamInfo.eIdx_ISO+1));
                 MUINT32 u4NR1LowerISO = m_pIspTuningCustom->map_ISO_index_to_value(m_rIspCamInfo.eIdx_ISO);
                 MUINT16 u2NR1UpperIdx = getISPIndex_byISO(m_rIspCamInfo.eIdx_ISO+1).NR1;
                 MUINT16 u2NR1LowerIdx = getISPIndex_byISO(m_rIspCamInfo.eIdx_ISO).NR1;

                 MY_LOG_IF(m_bDebugEnable,"%s(): m_rIspCamInfo.u4ISOValue = %d, u4UpperISO = %d, u4LowerISO = %d, u2UpperISOIdx = %d, u2LowerISOIdx = %d\n",
                            __FUNCTION__, m_rIspCamInfo.u4ISOValue,
                            u4NR1UpperISO,
                            u4NR1LowerISO,
                            m_rIspCamInfo.eIdx_ISO+1,
                            m_rIspCamInfo.eIdx_ISO);
                 MY_LOG_IF(m_bDebugEnable,"%s ISP_NVRAM_NR1_CT_CON_T: Upper(%d, %x), Lower(%d, %x)"
                           , __FUNCTION__
                           , u2NR1UpperIdx, m_IspNvramMgr.getNR1(u2NR1UpperIdx).ct_con.val
                           , u2NR1LowerIdx, m_IspNvramMgr.getNR1(u2NR1LowerIdx).ct_con.val);

                 SmoothNR1(m_rIspCamInfo.u4ISOValue,  // Real ISO
                           u4NR1UpperISO, // Upper ISO
                           u4NR1LowerISO, // Lower ISO
                           m_IspNvramMgr.getNR1(u2NR1UpperIdx), // NR1 settings for upper ISO
                           m_IspNvramMgr.getNR1(u2NR1LowerIdx), // NR1 settings for lower ISO
                           nr1);  // Output

              }
           }
           else {
              if (m_rIspCamInfo.eIdx_ISO > eIDX_ISO_100) {
                 MUINT32 u4NR1UpperISO = m_pIspTuningCustom->map_ISO_index_to_value(m_rIspCamInfo.eIdx_ISO);
                 MUINT32 u4NR1LowerISO = m_pIspTuningCustom->map_ISO_index_to_value(static_cast<EIndex_ISO_T>(m_rIspCamInfo.eIdx_ISO-1));
                 MUINT16 u2NR1UpperIdx = getISPIndex_byISO(m_rIspCamInfo.eIdx_ISO).NR1;
                 MUINT16 u2NR1LowerIdx = getISPIndex_byISO(m_rIspCamInfo.eIdx_ISO-1).NR1;

                 MY_LOG_IF(m_bDebugEnable,"%s(): m_rIspCamInfo.u4ISOValue = %d, u4UpperISO = %d, u4LowerISO = %d, u2UpperISOIdx = %d, u2LowerISOIdx = %d\n",
                            __FUNCTION__, m_rIspCamInfo.u4ISOValue,
                            u4NR1UpperISO,
                            u4NR1LowerISO,
                            m_rIspCamInfo.eIdx_ISO,
                            m_rIspCamInfo.eIdx_ISO-1);
                 MY_LOG_IF(m_bDebugEnable,"%s ISP_NVRAM_NR1_CT_CON_T: Upper(%d, %x), Lower(%d, %x)"
                           , __FUNCTION__
                           , u2NR1UpperIdx, m_IspNvramMgr.getNR1(u2NR1UpperIdx).ct_con.val
                           , u2NR1LowerIdx, m_IspNvramMgr.getNR1(u2NR1LowerIdx).ct_con.val);

                 SmoothNR1(m_rIspCamInfo.u4ISOValue,  // Real ISO
                           u4NR1UpperISO, // Upper ISO
                           u4NR1LowerISO, // Lower ISO
                           m_IspNvramMgr.getNR1(u2NR1UpperIdx), // NR1 settings for upper ISO
                           m_IspNvramMgr.getNR1(u2NR1LowerIdx), // NR1 settings for lower ISO
                           nr1);  // Output

              }
           }
         }


#endif
        // Invoke callback for customers to modify.
        if  ( isDynamicTuning() )
        {   //  Dynamic Tuning: Enable
            m_pIspTuningCustom->refine_NR1(m_rIspCamInfo, m_IspNvramMgr, nr1);
        }

        // Load it to ISP manager buffer.
        rBnr.put(nr1);
    }

    return  MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_PDC()
{

    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.pdc.disable", value, "0"); // 0: enable, 1: disable
    MBOOL bDisable = atoi(value);
    ISP_MGR_BNR_T& rBnr = ISP_MGR_BNR_T::getInstance(m_eSensorDev);

    m_rIspCamInfo.fgCCTInfo.Bits.fgPDC = rBnr.isCCTPDCEnable();

    if ((bDisable) || (!rBnr.isCCTPDCEnable()) )
    {
        rBnr.setPDCEnable(MFALSE);
    }
    else {
        //if (getOperMode() != EOperMode_Meta)
        rBnr.setPDCEnable(MTRUE);

        // Get default NVRAM parameter
        ISP_NVRAM_BNR_PDC_T pdc = m_IspNvramMgr.getPDC();


#if 0
        if (m_pIspTuningCustom->is_to_invoke_isp_interpolation(m_rIspCamInfo)) {
           if (m_rIspCamInfo.u4ISOValue >= m_pIspTuningCustom->map_ISO_index_to_value(m_rIspCamInfo.eIdx_ISO)) {
              if (m_rIspCamInfo.eIdx_ISO < eIDX_ISO_3200) {
                 MUINT32 u4NR1UpperISO = m_pIspTuningCustom->map_ISO_index_to_value(static_cast<EIndex_ISO_T>(m_rIspCamInfo.eIdx_ISO+1));
                 MUINT32 u4NR1LowerISO = m_pIspTuningCustom->map_ISO_index_to_value(m_rIspCamInfo.eIdx_ISO);
                 MUINT16 u2NR1UpperIdx = getISPIndex_byISO(m_rIspCamInfo.eIdx_ISO+1).NR1;
                 MUINT16 u2NR1LowerIdx = getISPIndex_byISO(m_rIspCamInfo.eIdx_ISO).NR1;

                 MY_LOG_IF(m_bDebugEnable,"%s(): m_rIspCamInfo.u4ISOValue = %d, u4UpperISO = %d, u4LowerISO = %d, u2UpperISOIdx = %d, u2LowerISOIdx = %d\n",
                            __FUNCTION__, m_rIspCamInfo.u4ISOValue,
                            u4NR1UpperISO,
                            u4NR1LowerISO,
                            m_rIspCamInfo.eIdx_ISO+1,
                            m_rIspCamInfo.eIdx_ISO);
                 MY_LOG_IF(m_bDebugEnable,"%s ISP_NVRAM_NR1_CT_CON_T: Upper(%d, %x), Lower(%d, %x)"
                           , __FUNCTION__
                           , u2NR1UpperIdx, m_IspNvramMgr.getNR1(u2NR1UpperIdx).ct_con.val
                           , u2NR1LowerIdx, m_IspNvramMgr.getNR1(u2NR1LowerIdx).ct_con.val);

                 SmoothNR1(m_rIspCamInfo.u4ISOValue,  // Real ISO
                           u4NR1UpperISO, // Upper ISO
                           u4NR1LowerISO, // Lower ISO
                           m_IspNvramMgr.getNR1(u2NR1UpperIdx), // NR1 settings for upper ISO
                           m_IspNvramMgr.getNR1(u2NR1LowerIdx), // NR1 settings for lower ISO
                           nr1);  // Output

              }
           }
           else {
              if (m_rIspCamInfo.eIdx_ISO > eIDX_ISO_100) {
                 MUINT32 u4NR1UpperISO = m_pIspTuningCustom->map_ISO_index_to_value(m_rIspCamInfo.eIdx_ISO);
                 MUINT32 u4NR1LowerISO = m_pIspTuningCustom->map_ISO_index_to_value(static_cast<EIndex_ISO_T>(m_rIspCamInfo.eIdx_ISO-1));
                 MUINT16 u2NR1UpperIdx = getISPIndex_byISO(m_rIspCamInfo.eIdx_ISO).NR1;
                 MUINT16 u2NR1LowerIdx = getISPIndex_byISO(m_rIspCamInfo.eIdx_ISO-1).NR1;

                 MY_LOG_IF(m_bDebugEnable,"%s(): m_rIspCamInfo.u4ISOValue = %d, u4UpperISO = %d, u4LowerISO = %d, u2UpperISOIdx = %d, u2LowerISOIdx = %d\n",
                            __FUNCTION__, m_rIspCamInfo.u4ISOValue,
                            u4NR1UpperISO,
                            u4NR1LowerISO,
                            m_rIspCamInfo.eIdx_ISO,
                            m_rIspCamInfo.eIdx_ISO-1);
                 MY_LOG_IF(m_bDebugEnable,"%s ISP_NVRAM_NR1_CT_CON_T: Upper(%d, %x), Lower(%d, %x)"
                           , __FUNCTION__
                           , u2NR1UpperIdx, m_IspNvramMgr.getNR1(u2NR1UpperIdx).ct_con.val
                           , u2NR1LowerIdx, m_IspNvramMgr.getNR1(u2NR1LowerIdx).ct_con.val);

                 SmoothNR1(m_rIspCamInfo.u4ISOValue,  // Real ISO
                           u4NR1UpperISO, // Upper ISO
                           u4NR1LowerISO, // Lower ISO
                           m_IspNvramMgr.getNR1(u2NR1UpperIdx), // NR1 settings for upper ISO
                           m_IspNvramMgr.getNR1(u2NR1LowerIdx), // NR1 settings for lower ISO
                           nr1);  // Output

              }
           }
         }


#endif
        // Invoke callback for customers to modify.
        if  ( isDynamicTuning() )
        {   //  Dynamic Tuning: Enable
            m_pIspTuningCustom->refine_PDC(m_rIspCamInfo, m_IspNvramMgr, pdc);
        }

        // Load it to ISP manager buffer.
        rBnr.put(pdc);
    }

    return  MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_RMM()
{

// RMM enable is currently controlled by VHDR Hal
#if 0
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.rmm.disable", value, "0"); // 0: enable, 1: disable
    MBOOL bDisable = atoi(value);
    ISP_MGR_RMM_T& rRmm = ISP_MGR_RMM_T::getInstance(m_eSensorDev);

    m_rIspCamInfo.fgCCTInfo.Bits.fgRMM = rRmm.isCCTEnable();

    if ((bDisable) || (!rRmm.isCCTEnable()) )
    {
        rRmm.setEnable(MFALSE);
    }
    else {
        //if (getOperMode() != EOperMode_Meta)
        rRmm.setEnable(MTRUE);
#endif
        // Get default NVRAM parameter
        ISP_NVRAM_RMM_T rmm = m_IspNvramMgr.getRMM();


#if 0
        if (m_pIspTuningCustom->is_to_invoke_isp_interpolation(m_rIspCamInfo)) {
           if (m_rIspCamInfo.u4ISOValue >= m_pIspTuningCustom->map_ISO_index_to_value(m_rIspCamInfo.eIdx_ISO)) {
              if (m_rIspCamInfo.eIdx_ISO < eIDX_ISO_3200) {
                 MUINT32 u4NR1UpperISO = m_pIspTuningCustom->map_ISO_index_to_value(static_cast<EIndex_ISO_T>(m_rIspCamInfo.eIdx_ISO+1));
                 MUINT32 u4NR1LowerISO = m_pIspTuningCustom->map_ISO_index_to_value(m_rIspCamInfo.eIdx_ISO);
                 MUINT16 u2NR1UpperIdx = getISPIndex_byISO(m_rIspCamInfo.eIdx_ISO+1).NR1;
                 MUINT16 u2NR1LowerIdx = getISPIndex_byISO(m_rIspCamInfo.eIdx_ISO).NR1;

                 MY_LOG_IF(m_bDebugEnable,"%s(): m_rIspCamInfo.u4ISOValue = %d, u4UpperISO = %d, u4LowerISO = %d, u2UpperISOIdx = %d, u2LowerISOIdx = %d\n",
                            __FUNCTION__, m_rIspCamInfo.u4ISOValue,
                            u4NR1UpperISO,
                            u4NR1LowerISO,
                            m_rIspCamInfo.eIdx_ISO+1,
                            m_rIspCamInfo.eIdx_ISO);
                 MY_LOG_IF(m_bDebugEnable,"%s ISP_NVRAM_NR1_CT_CON_T: Upper(%d, %x), Lower(%d, %x)"
                           , __FUNCTION__
                           , u2NR1UpperIdx, m_IspNvramMgr.getNR1(u2NR1UpperIdx).ct_con.val
                           , u2NR1LowerIdx, m_IspNvramMgr.getNR1(u2NR1LowerIdx).ct_con.val);

                 SmoothNR1(m_rIspCamInfo.u4ISOValue,  // Real ISO
                           u4NR1UpperISO, // Upper ISO
                           u4NR1LowerISO, // Lower ISO
                           m_IspNvramMgr.getNR1(u2NR1UpperIdx), // NR1 settings for upper ISO
                           m_IspNvramMgr.getNR1(u2NR1LowerIdx), // NR1 settings for lower ISO
                           nr1);  // Output

              }
           }
           else {
              if (m_rIspCamInfo.eIdx_ISO > eIDX_ISO_100) {
                 MUINT32 u4NR1UpperISO = m_pIspTuningCustom->map_ISO_index_to_value(m_rIspCamInfo.eIdx_ISO);
                 MUINT32 u4NR1LowerISO = m_pIspTuningCustom->map_ISO_index_to_value(static_cast<EIndex_ISO_T>(m_rIspCamInfo.eIdx_ISO-1));
                 MUINT16 u2NR1UpperIdx = getISPIndex_byISO(m_rIspCamInfo.eIdx_ISO).NR1;
                 MUINT16 u2NR1LowerIdx = getISPIndex_byISO(m_rIspCamInfo.eIdx_ISO-1).NR1;

                 MY_LOG_IF(m_bDebugEnable,"%s(): m_rIspCamInfo.u4ISOValue = %d, u4UpperISO = %d, u4LowerISO = %d, u2UpperISOIdx = %d, u2LowerISOIdx = %d\n",
                            __FUNCTION__, m_rIspCamInfo.u4ISOValue,
                            u4NR1UpperISO,
                            u4NR1LowerISO,
                            m_rIspCamInfo.eIdx_ISO,
                            m_rIspCamInfo.eIdx_ISO-1);
                 MY_LOG_IF(m_bDebugEnable,"%s ISP_NVRAM_NR1_CT_CON_T: Upper(%d, %x), Lower(%d, %x)"
                           , __FUNCTION__
                           , u2NR1UpperIdx, m_IspNvramMgr.getNR1(u2NR1UpperIdx).ct_con.val
                           , u2NR1LowerIdx, m_IspNvramMgr.getNR1(u2NR1LowerIdx).ct_con.val);

                 SmoothNR1(m_rIspCamInfo.u4ISOValue,  // Real ISO
                           u4NR1UpperISO, // Upper ISO
                           u4NR1LowerISO, // Lower ISO
                           m_IspNvramMgr.getNR1(u2NR1UpperIdx), // NR1 settings for upper ISO
                           m_IspNvramMgr.getNR1(u2NR1LowerIdx), // NR1 settings for lower ISO
                           nr1);  // Output

              }
           }
         }


#endif
        // Invoke callback for customers to modify.
        if  ( isDynamicTuning() )
        {   //  Dynamic Tuning: Enable
            m_pIspTuningCustom->refine_RMM(m_rIspCamInfo, m_IspNvramMgr, rmm);
        }

        // Load it to ISP manager buffer.
        ISP_MGR_RMM_T::getInstance(m_eSensorDev).put(rmm);
// RMM enable is currently controlled by VHDR Hal
#if 0
    }
#endif

    return  MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_LSC()
{
    MY_LOG_IF(m_bDebugEnable,"%s", __FUNCTION__);

    //////////////////////////////////////
    MUINT32 new_cct_idx = eIDX_Shading_CCT_BEGIN;
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("debug.lsc_mgr.manual_tsf", value, "-1");
    MINT32 i4Tsf = atoi(value);
    property_get("debug.lsc_mgr.ratio", value, "-1");
    MINT32 i4Rto = atoi(value);
    property_get("debug.lsc_mgr.enable", value, "-1");
    MINT32 i4OnOff = atoi(value);

    if (i4Tsf != -1){
        m_pLscMgr->setTsfOnOff(i4Tsf ? MTRUE : MFALSE);
    }

    if (i4OnOff != -1){
        m_pLscMgr->setOnOff(i4OnOff ? MTRUE : MFALSE);
    }

    // Check to see if it is needed to load LUT.
    MY_LOG_IF(m_bDebugEnable,"[%s] m_pLscMgr OperMode(%d)\n", __FUNCTION__, getOperMode());

    // Invoke callback for customers to modify.
    if  (m_fgDynamicShading)
    {
        // Dynamic Tuning: Enable
        new_cct_idx = m_pIspTuningCustom->evaluate_Shading_CCT_index(m_rIspCamInfo);
        m_pLscMgr->setCTIdx(new_cct_idx);

        if (i4Rto == -1)
        {
            i4Rto = 32;//m_pIspTuningCustom->evaluate_Shading_Ratio(m_rIspCamInfo);
            MY_LOG("[%s] (flash, iso, rto) = (%d, %d, %d)", __FUNCTION__,
                m_rIspCamInfo.rFlashInfo.isFlash,
                m_rIspCamInfo.rAEInfo.u4RealISOValue, i4Rto);
        }
    }
    else
    {
        if (i4Rto == -1) i4Rto = 32;
    }

    m_pLscMgr->setRatio(i4Rto);

    if (!m_pLscMgr->getTsfOnOff())
        m_pLscMgr->updateLsc();

    // debug message
    m_rIspCamInfo.eIdx_Shading_CCT = (NSIspTuning::EIndex_Shading_CCT_T)m_pLscMgr->getCTIdx();
    ILscMgr::SL2_CFG_T rSl2Cfg = m_pLscMgr->getSl2();
    MUINT32 u4RawWd, u4RawHt;
    m_pLscMgr->getRawSize(m_pLscMgr->getSensorMode(), u4RawWd, u4RawHt);
    m_rIspCamInfo.rCropRzInfo.i4FullW = u4RawWd;
    m_rIspCamInfo.rCropRzInfo.i4FullH = u4RawHt;
    m_rIspCamInfo.rSl2Info.i4CenterX  = rSl2Cfg.i4CenterX;
    m_rIspCamInfo.rSl2Info.i4CenterY  = rSl2Cfg.i4CenterY;
    m_rIspCamInfo.rSl2Info.i4R0       = rSl2Cfg.i4R0;
    m_rIspCamInfo.rSl2Info.i4R1       = rSl2Cfg.i4R1;
    m_rIspCamInfo.rSl2Info.i4R2       = rSl2Cfg.i4R2;
    m_IspNvramMgr.setIdx_LSC(new_cct_idx);
    //////////////////////////////////////
    return  MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_RPG()
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.rpg.disable", value, "0"); // 0: enable, 1: disable
    MBOOL bDisable = atoi(value);
    ISP_MGR_RPG_T& rRpg = ISP_MGR_RPG_T::getInstance(m_eSensorDev);

    // Get default NVRAM parameter
    ISP_NVRAM_RPG_T rpg;

    AWB_GAIN_T rCurrentAWBGain = m_rIspCamInfo.rAWBInfo.rCurrentAWBGain;

    MY_LOG("[%s] En(%d), AWB(%d,%d,%d)", __FUNCTION__,
        isRPGEnable(), rCurrentAWBGain.i4R, rCurrentAWBGain.i4G, rCurrentAWBGain.i4B);

    rRpg.setIspAWBGain(rCurrentAWBGain);

    if (isRPGEnable() && (!bDisable)) {// RPG is enabled

        rRpg.setEnable(MTRUE);
        getIspHWBuf(m_eSensorDev, rpg );

        // Invoke callback for customers to modify.
        if  ( isDynamicTuning() )
        {   //  Dynamic Tuning: Enable
            m_pIspTuningCustom->refine_RPG(m_rIspCamInfo, m_IspNvramMgr, rpg);
        }

        // Load it to ISP manager buffer.
        rRpg.put(rpg);
    }
    else {
        rRpg.setEnable(MFALSE);
    }

    return  MTRUE;
}




//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_CCM()
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.ccm.disable", value, "0"); // 0: enable, 1: disable
    MBOOL bDisable = atoi(value);

    if ((bDisable) ||
        (!ISP_MGR_CCM_T::getInstance(m_eSensorDev).isCCTEnable())              ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_MFB_Blending_All_Off)        ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_MFB_Blending_All_Off_SWNR)   ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_MFB_PostProc_Mixing)         ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_MFB_PostProc_Mixing_SWNR)    ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_VFB_PostProc)                ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_Capture_MultiPass_ANR_1)     ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_Capture_MultiPass_ANR_2)     ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_VSS_Capture_MultiPass_ANR_1) ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_VSS_Capture_MultiPass_ANR_2) ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_MFB_MultiPass_ANR_1)         ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_MFB_MultiPass_ANR_2)) {
        ISP_MGR_CCM_T::getInstance(m_eSensorDev).setEnable(MFALSE);
    }
    else {
        //if (getOperMode() != EOperMode_Meta)
            ISP_MGR_CCM_T::getInstance(m_eSensorDev).setEnable(MTRUE);

        // Get default NVRAM parameter
        ISP_NVRAM_CCM_T ccm = m_pCcmMgr->getCCM();

        // Invoke callback for customers to modify.
        if  ( isDynamicTuning() )
        {   //  Dynamic Tuning: Enable
            m_pIspTuningCustom->refine_CCM(m_rIspCamInfo, m_IspNvramMgr, ccm);
        }

        // Load it to ISP manager buffer.
        //putIspHWBuf(m_eSensorDev, ccm );
        m_rIspCamInfo.rMtkCCM = ccm;
    }

    return  MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_GGM()
{
    if (m_rIspCamInfo.eToneMapMode == MTK_TONEMAP_MODE_CONTRAST_CURVE)
    {
        m_rIspCamInfo.rMtkGGM = m_pGgmMgr->getGGM();
    }
    else
    {
        // Get default NVRAM parameter
        if ((m_rIspCamInfo.eIspProfile == EIspProfile_IHDR_Preview) ||
            (m_rIspCamInfo.eIspProfile == EIspProfile_IHDR_Video)   ||
            (m_rIspCamInfo.eIspProfile == EIspProfile_zHDR_Preview) ||
            (m_rIspCamInfo.eIspProfile == EIspProfile_zHDR_Video)   ||
            (m_rIspCamInfo.eIspProfile == EIspProfile_zHDR_Capture) ||
            (m_rIspCamInfo.eIspProfile == EIspProfile_MHDR_Preview) ||
            (m_rIspCamInfo.eIspProfile == EIspProfile_MHDR_Video)) {
            m_rIspCamInfo.rMtkGGM = m_IspNvramMgr.getIHDRGGM(m_rIspCamInfo.rAEInfo.i4GammaIdx);
            MY_LOG_IF(m_bDebugEnable,"%s: m_rIspCamInfo.rAEInfo.i4GammaIdx = %d", __FUNCTION__, m_rIspCamInfo.rAEInfo.i4GammaIdx);
        }
        else {
            //ggm = m_IspNvramMgr.getGGM();
            m_rIspCamInfo.rMtkGGM = m_IspNvramMgr.getGGM();
        }

         m_pGmaMgr->calculateGGM(&m_rIspCamInfo.rMtkGGM, &m_GmaExifInfo);

        // Invoke callback for customers to modify.
        if ( isDynamicTuning() )
        {   //  Dynamic Tuning: Enable
            m_pIspTuningCustom->refine_GGM(m_rIspCamInfo,  m_IspNvramMgr, m_rIspCamInfo.rMtkGGM);
            m_pIspTuningCustom->userSetting_EFFECT_GGM(m_rIspCamInfo, m_eIdx_Effect, m_rIspCamInfo.rMtkGGM);
        }

        // Load it to ISP manager buffer.
        //putIspHWBuf(m_eSensorDev, ggm );
        //m_rIspCamInfo.rMtkGGM = ggm;
        m_pGgmMgr->updateGGM(m_rIspCamInfo.rMtkGGM);
    }

    return  MTRUE;
}





//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// prepare HW in pass 2
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

MBOOL
Paramctrl::
prepareHw_PerFrame_DBS_2(const RAWIspCamInfo& rIspCamInfo, const IndexMgr& rIdx)
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.dbs2.disable", value, "0"); // 0: enable, 1: disable
    MBOOL bDisable = atoi(value);
    MBOOL fgPgnEn = !rIspCamInfo.fgRPGEnable;
    MBOOL fgPureRaw = rIspCamInfo.i4RawType == NSIspTuning::ERawType_Pure;
    MBOOL fgDbs2En = fgPureRaw && fgPgnEn;
    ISP_MGR_DBS2_T& rDbs2 = ISP_MGR_DBS2_T::getInstance(m_eSensorDev);

    // Get default NVRAM parameter
    if (!fgDbs2En|| bDisable || (!rIspCamInfo.fgCCTInfo.Bits.fgDBS) )
    {
        rDbs2.setEnable(MFALSE);
    }
    else
    {
        //if (getOperMode() != EOperMode_Meta)
        rDbs2.setEnable(MTRUE);

        // Get default NVRAM parameter
        ISP_NVRAM_DBS_T dbs = m_IspNvramMgr.getDBS(rIdx.getIdx_DBS());

        // Invoke callback for customers to modify.
        if  ( isDynamicTuning() )
        {   //  Dynamic Tuning: Enable
            m_pIspTuningCustom->refine_DBS(rIspCamInfo, m_IspNvramMgr, dbs);
        }

        // Load it to ISP manager buffer.
        rDbs2.put(dbs);

    }

    return  MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_OBC_2(const RAWIspCamInfo& rIspCamInfo, const IndexMgr& rIdx)
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.obc2.disable", value, "0"); // 0: enable, 1: disable
    MBOOL bDisable = atoi(value);
    MBOOL fgPgnEn = !rIspCamInfo.fgRPGEnable;
    MBOOL fgPureRaw = rIspCamInfo.i4RawType == NSIspTuning::ERawType_Pure;
    MBOOL fgObc2En = fgPureRaw && fgPgnEn;
    ISP_MGR_OBC2_T& rObc2 = ISP_MGR_OBC2_T::getInstance(m_eSensorDev);

    // Get default NVRAM parameter
    if ( !fgObc2En || bDisable || (!rIspCamInfo.fgCCTInfo.Bits.fgOBC) )
    {
        rObc2.setEnable(MFALSE);
    }
    else
    {
        //if (getOperMode() != EOperMode_Meta)
        rObc2.setEnable(MTRUE);

        // Get default NVRAM parameter
        ISP_NVRAM_OBC_T obc = m_IspNvramMgr.getOBC(rIdx.getIdx_OBC());
        rObc2.setIspAEGain(rIspCamInfo.rAEInfo.u4IspGain>>1);

        // Invoke callback for customers to modify.
        if  ( isDynamicTuning() )
        {   //  Dynamic Tuning: Enable
            m_pIspTuningCustom->refine_OBC(rIspCamInfo, m_IspNvramMgr, obc);
        }

        this->setPureOBCInfo(&obc);

        // Load it to ISP manager buffer.
        rObc2.put(obc);
    }

    return  MTRUE;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_BPC_2(const RAWIspCamInfo& rIspCamInfo, const IndexMgr& rIdx)
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.bpc2.disable", value, "0"); // 0: enable, 1: disable
    MBOOL bDisable = atoi(value);
    MBOOL fgPgnEn = !rIspCamInfo.fgRPGEnable;
    MBOOL fgPureRaw = rIspCamInfo.i4RawType == NSIspTuning::ERawType_Pure;
    MBOOL fgBpc2En = fgPureRaw && fgPgnEn;
    ISP_MGR_BNR2_T& rBnr2 = ISP_MGR_BNR2_T::getInstance(m_eSensorDev);

    // Get default NVRAM parameter
    if (!fgBpc2En|| bDisable || (!rIspCamInfo.fgCCTInfo.Bits.fgBPC) )
    {
        rBnr2.setBPC2Enable(MFALSE);
    }
    else
    {
        //if (getOperMode() != EOperMode_Meta)
        rBnr2.setBPC2Enable(MTRUE);

        // Get default NVRAM parameter
        ISP_NVRAM_BNR_BPC_T bpc = m_IspNvramMgr.getBPC(rIdx.getIdx_BPC());

        // Invoke callback for customers to modify.
        if  ( isDynamicTuning() )
        {   //  Dynamic Tuning: Enable
            m_pIspTuningCustom->refine_BPC(rIspCamInfo, m_IspNvramMgr, bpc);
        }

        // Load it to ISP manager buffer.
        rBnr2.put(bpc);
    }

    return  MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_NR1_2(const RAWIspCamInfo& rIspCamInfo, const IndexMgr& rIdx)
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.nr12.disable", value, "0"); // 0: enable, 1: disable
    MBOOL bDisable = atoi(value);
    MBOOL fgPgnEn = !rIspCamInfo.fgRPGEnable;
    MBOOL fgPureRaw = rIspCamInfo.i4RawType == NSIspTuning::ERawType_Pure;
    MBOOL fgNr12En = fgPureRaw && fgPgnEn;
    ISP_MGR_BNR2_T& rBnr2 = ISP_MGR_BNR2_T::getInstance(m_eSensorDev);

    // Get default NVRAM parameter
    if ( !fgNr12En || bDisable || (!rIspCamInfo.fgCCTInfo.Bits.fgNR1) ||
        (rIspCamInfo.eIspProfile == EIspProfile_IHDR_Preview) ||
        (rIspCamInfo.eIspProfile == EIspProfile_IHDR_Video)   ||
        (rIspCamInfo.eIspProfile == EIspProfile_zHDR_Preview) ||
        (rIspCamInfo.eIspProfile == EIspProfile_zHDR_Video)   ||
        (rIspCamInfo.eIspProfile == EIspProfile_zHDR_Capture))
    {
        rBnr2.setCT2Enable(MFALSE);
    }
    else
    {
        //if (getOperMode() != EOperMode_Meta)
        rBnr2.setCT2Enable(MTRUE);

        // Get default NVRAM parameter
        ISP_NVRAM_BNR_NR1_T nr1 = m_IspNvramMgr.getNR1(rIdx.getIdx_NR1());

        // Invoke callback for customers to modify.
        if  ( isDynamicTuning() )
        {   //  Dynamic Tuning: Enable
            m_pIspTuningCustom->refine_NR1(rIspCamInfo, m_IspNvramMgr, nr1);
        }

        // Load it to ISP manager buffer.
        rBnr2.put(nr1);
    }

    return  MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_PDC_2(const RAWIspCamInfo& rIspCamInfo, const IndexMgr& rIdx)
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.pdc2.disable", value, "0"); // 0: enable, 1: disable
    MBOOL bDisable = atoi(value);
    MBOOL fgPgnEn = !rIspCamInfo.fgRPGEnable;
    MBOOL fgPureRaw = rIspCamInfo.i4RawType == NSIspTuning::ERawType_Pure;
    MBOOL fgPdc2En = fgPureRaw && fgPgnEn;
    ISP_MGR_BNR2_T& rBnr2 = ISP_MGR_BNR2_T::getInstance(m_eSensorDev);

    // Get default NVRAM parameter
    if (!fgPdc2En|| bDisable || (!rIspCamInfo.fgCCTInfo.Bits.fgPDC) )
    {
        rBnr2.setPDC2Enable(MFALSE);
    }
    else
    {
        //if (getOperMode() != EOperMode_Meta)
        rBnr2.setPDC2Enable(MTRUE);

        // Get default NVRAM parameter
        ISP_NVRAM_BNR_PDC_T pdc = m_IspNvramMgr.getPDC(rIdx.getIdx_PDC());

        // Invoke callback for customers to modify.
        if  ( isDynamicTuning() )
        {   //  Dynamic Tuning: Enable
            m_pIspTuningCustom->refine_PDC(rIspCamInfo, m_IspNvramMgr, pdc);
        }

        // Load it to ISP manager buffer.
        rBnr2.put(pdc);
    }

    return  MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_LSC_2(const ISP_INFO_T& rIspInfo)
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.lsc2.disable", value, "0"); // 0: enable, 1: disable
    MBOOL fgRet = MFALSE;
    MBOOL bDisable = atoi(value);
    MBOOL fgPgnEn = !rIspInfo.rCamInfo.fgRPGEnable;
    MBOOL fgPureRaw = rIspInfo.rCamInfo.i4RawType == NSIspTuning::ERawType_Pure;
    MBOOL fgLsc2En = fgPureRaw && fgPgnEn;
    ISP_MGR_LSC2_T& rLsc2 = ISP_MGR_LSC2_T::getInstance(m_eSensorDev);

    if (rIspInfo.rLscData.size())
    {
        ILscTbl::Config rCfg;
        ::memcpy(&rCfg, rIspInfo.rLscData.data(), sizeof(ILscTbl::Config));
        if (rCfg.i4GridX > 17 || rCfg.i4GridY > 17 || rCfg.i4GridX <= 0 || rCfg.i4GridY <= 0 || rCfg.i4ImgWd <= 0 || rCfg.i4ImgHt <= 0)
        {
            MY_ERR("Abnormal config (%d,%d,%d,%d)", rCfg.i4GridX, rCfg.i4GridY, rCfg.i4ImgWd, rCfg.i4ImgHt);
        }
        else
        {
            MUINT32 u4W, u4H;
            if (m_pLscMgr->getRawSize(m_pLscMgr->getSensorMode(), u4W, u4H))
            {
                MY_LOG("[%s] Process Lsc Data @(%d), Modify Size(%dx%d -> %dx%d), grid(%dx%d)", __FUNCTION__,
                    rIspInfo.rCamInfo.u4Id, rCfg.i4ImgWd, rCfg.i4ImgHt, u4W, u4H, rCfg.i4GridX, rCfg.i4GridY);
                #if CAM3_LSC2_USE_GAIN
                ILscTbl rTbl(ILscTbl::GAIN_FIXED, u4W, u4H, rCfg.i4GridX, rCfg.i4GridY);
                #else
                ILscTbl rTbl(ILscTbl::HWTBL, u4W, u4H, rCfg.i4GridX, rCfg.i4GridY);
                #endif
                rTbl.setData(rIspInfo.rLscData.data()+sizeof(ILscTbl::Config), rTbl.getSize());
                m_pLscMgr->syncTbl(rTbl, ILscMgr::LSC_P2);
                fgRet = MTRUE;
            }
            else
            {
                MY_ERR("Fail to get RAW size!");
            }
        }
    }

    MY_LOG_IF(m_bDebugEnable, "[%s] force off(%d), lsc2(%d), pgn(%d), pureraw(%d), fgRet(%d)", __FUNCTION__, bDisable, fgLsc2En, fgPgnEn, fgPureRaw, fgRet);

    if (!fgLsc2En || bDisable || (fgRet == MFALSE))
    {
        rLsc2.enableLsc(MFALSE);
    }
    else
    {
        rLsc2.enableLsc(MTRUE);
    }

    return MTRUE;
}



//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_SL2(const ISP_INFO_T& rIspInfo, const IndexMgr& rIdx)
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.sl2.disable", value, "0"); // 0: enable, 1: disable
    MUINT32 bDisable = atoi(value);
    MINT32 dbg_enable = ::property_get_int32("debug.lsc_mgr.nsl2", -1);
    MINT32 i4Sl2Case = (dbg_enable != -1) ? dbg_enable : isEnableSL2(m_eSensorDev);

    const NSIspTuning::RAWIspCamInfo& rIspCamInfo = rIspInfo.rCamInfo;
    ISP_MGR_SL2_T& rSl2 = ISP_MGR_SL2_T::getInstance(m_eSensorDev);

    if ((bDisable)||
        (!rSl2.isCCTEnable())               ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_VFB_PostProc)                 ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_Capture_MultiPass_ANR_1)      ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_VSS_Capture_MultiPass_ANR_1)  ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_MFB_MultiPass_ANR_1)          ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_MFB_Blending_All_Off_SWNR)    ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_MFB_Blending_All_Off)         ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_MFB_PostProc_Mixing_SWNR)     ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_MFB_PostProc_Mixing)          ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_VSS_MFB_MultiPass_ANR_1)      ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_VSS_MFB_Blending_All_Off_SWNR)||
        (m_rIspCamInfo.eIspProfile == EIspProfile_VSS_MFB_Blending_All_Off)     ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_VSS_MFB_PostProc_Mixing_SWNR) ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_VSS_MFB_PostProc_Mixing) )
    {
        rSl2.setEnable(MFALSE);
        rSl2.setSL2BEnable(MFALSE);
        rSl2.setSL2CEnable(MFALSE);
        rSl2.setSL2DEnable(MFALSE);
        rSl2.setSL2EEnable(MFALSE);
    }
    else
    {
        rSl2.setEnable(MTRUE);
        rSl2.setSL2BEnable(MTRUE);
        rSl2.setSL2CEnable(MTRUE);
        rSl2.setSL2DEnable(MTRUE);
        rSl2.setSL2EEnable(MTRUE);

        ISP_NVRAM_SL2_T sl2 = m_IspNvramMgr.getSL2(rIdx.getIdx_SL2());
        if (i4Sl2Case == 1)
        {
            // use LSC determined SL2
            sl2.cen.bits.SL2_CENTR_X = rIspCamInfo.rSl2Info.i4CenterX;
            sl2.cen.bits.SL2_CENTR_Y = rIspCamInfo.rSl2Info.i4CenterY;
            sl2.rr_con0.bits.SL2_R_0 = rIspCamInfo.rSl2Info.i4R0;
            sl2.rr_con0.bits.SL2_R_1 = rIspCamInfo.rSl2Info.i4R1;
            sl2.rr_con1.bits.SL2_R_2 = rIspCamInfo.rSl2Info.i4R2;
        }

        if  ( isDynamicTuning() )
        {   //  Dynamic Tuning: Enable
            m_pIspTuningCustom->refine_SL2(rIspCamInfo, m_IspNvramMgr, sl2);
        }

        // put SL2, SL2B, SL2C, SL2D
        rSl2.put(rIspCamInfo.rCropRzInfo, sl2);

        // get SL2D register and put SL2E
        rSl2.get(sl2);
        rSl2.put_sl2e(rIspInfo.rIspP2CropInfo, sl2);
    }


    return  MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_PGN(const RAWIspCamInfo& rIspCamInfo, const IndexMgr& rIdx)
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.pgn.disable", value, "0"); // 0: enable, 1: disable
    MBOOL bDisable = atoi(value);
    MBOOL fgRPGEnable = rIspCamInfo.fgRPGEnable;
    ISP_MGR_PGN_T& rPgn = ISP_MGR_PGN_T::getInstance(m_eSensorDev);

    // Get default NVRAM parameter
    ISP_NVRAM_PGN_T pgn;

    AWB_GAIN_T rCurrentAWBGain = rIspCamInfo.rAWBInfo.rCurrentAWBGain;

    MY_LOG("[%s] En(%d), AWB(%d,%d,%d)", __FUNCTION__,
            !fgRPGEnable, rCurrentAWBGain.i4R, rCurrentAWBGain.i4G, rCurrentAWBGain.i4B);

    ISP_MGR_PGN_T::getInstance(m_eSensorDev).setIspAWBGain(rCurrentAWBGain);

    if (fgRPGEnable || (bDisable)) {// RPG is enabled
        rPgn.setEnable(MFALSE);
    }
    else
    {
        rPgn.setEnable(MTRUE);

        getIspHWBuf(m_eSensorDev, pgn );

        // Invoke callback for customers to modify.
        if  ( isDynamicTuning() )
        {   //  Dynamic Tuning: Enable
            m_pIspTuningCustom->refine_PGN(rIspCamInfo, m_IspNvramMgr, pgn);
        }

        // Load it to ISP manager buffer.
        rPgn.put(pgn);
    }

    return  MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_UDM(RAWIspCamInfo& rIspCamInfo, const IndexMgr& rIdx)
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.udm.disable", value, "0"); // 0: enable, 1: disable
    MBOOL bDisable = atoi(value);
    MBOOL fgRPGEnable = rIspCamInfo.fgRPGEnable;
    ISP_MGR_UDM_T& rUdm = ISP_MGR_UDM_T::getInstance(m_eSensorDev);

    MY_LOG_IF(m_bDebugEnable, "[%s] fgRPGEnable(%d)", __FUNCTION__, fgRPGEnable);

    if ((rIspCamInfo.eIspProfile == EIspProfile_MFB_Blending_All_Off)        ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_Blending_All_Off_SWNR)   ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_PostProc_Mixing)         ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_PostProc_Mixing_SWNR)    ||
        (rIspCamInfo.eIspProfile == EIspProfile_VFB_PostProc)                ||
        (rIspCamInfo.eIspProfile == EIspProfile_Capture_MultiPass_ANR_1)     ||
        (rIspCamInfo.eIspProfile == EIspProfile_VSS_Capture_MultiPass_ANR_1) ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_MultiPass_ANR_1)) {
        return MTRUE;
    }

    // Get default NVRAM parameter
    ISP_NVRAM_UDM_T udm = m_IspNvramMgr.getUDM(rIdx.getIdx_UDM());

#if 0
    if (m_pIspTuningCustom->is_to_invoke_isp_interpolation(rIspCamInfo)) {
        if (rIspCamInfo.u4ISOValue >= m_pIspTuningCustom->map_ISO_index_to_value(rIspCamInfo.eIdx_ISO)) {
             if (rIspCamInfo.eIdx_ISO < eIDX_ISO_3200) {
                rIspCamInfo.rIspIntInfo.u4UdmUpperISO = m_pIspTuningCustom->map_ISO_index_to_value(static_cast<EIndex_ISO_T>(rIspCamInfo.eIdx_ISO+1));
                rIspCamInfo.rIspIntInfo.u4UdmLowerISO = m_pIspTuningCustom->map_ISO_index_to_value(rIspCamInfo.eIdx_ISO);
                rIspCamInfo.rIspIntInfo.u2UdmUpperIdx = m_IspNvramMgr.getIdx_UDM()+1;
                rIspCamInfo.rIspIntInfo.u2UdmLowerIdx = m_IspNvramMgr.getIdx_UDM();

                MY_LOG_IF(m_bDebugEnable,"%s(): m_rIspCamInfo.u4ISOValue = %d, u4UpperISO = %d, u4LowerISO = %d, u2UpperIdx = %d, u2LowerIdx = %d\n",
                          __FUNCTION__, rIspCamInfo.u4ISOValue,
                          rIspCamInfo.rIspIntInfo.u4UdmUpperISO,
                          rIspCamInfo.rIspIntInfo.u4UdmLowerISO,
                          rIspCamInfo.rIspIntInfo.u2UdmUpperIdx,
                          rIspCamInfo.rIspIntInfo.u2UdmLowerIdx);

                SmoothUDM(rIspCamInfo.u4ISOValue,  // Real ISO
                          rIspCamInfo.rIspIntInfo.u4UdmUpperISO, // Upper ISO
                          rIspCamInfo.rIspIntInfo.u4UdmLowerISO, // Lower ISO
                          m_IspNvramMgr.getUDM(rIspCamInfo.rIspIntInfo.u2UdmUpperIdx), // CFA settings for upper ISO
                          m_IspNvramMgr.getUDM(rIspCamInfo.rIspIntInfo.u2UdmLowerIdx), // CFA settings for lower ISO
                          udm);  // Output

             }
         }
         else {
             if (rIspCamInfo.eIdx_ISO > eIDX_ISO_100) {
                rIspCamInfo.rIspIntInfo.u4UdmUpperISO = m_pIspTuningCustom->map_ISO_index_to_value(rIspCamInfo.eIdx_ISO);
                rIspCamInfo.rIspIntInfo.u4UdmLowerISO = m_pIspTuningCustom->map_ISO_index_to_value(static_cast<EIndex_ISO_T>(rIspCamInfo.eIdx_ISO-1));
                rIspCamInfo.rIspIntInfo.u2UdmUpperIdx = m_IspNvramMgr.getIdx_UDM();
                rIspCamInfo.rIspIntInfo.u2UdmLowerIdx = m_IspNvramMgr.getIdx_UDM()-1;

                MY_LOG_IF(m_bDebugEnable,"%s(): m_rIspCamInfo.u4ISOValue = %d, u4UpperISO = %d, u4LowerISO = %d, u2UpperIdx = %d, u2LowerIdx = %d\n",
                          __FUNCTION__, rIspCamInfo.u4ISOValue,
                          rIspCamInfo.rIspIntInfo.u4UdmUpperISO,
                          rIspCamInfo.rIspIntInfo.u4UdmLowerISO,
                          rIspCamInfo.rIspIntInfo.u2UdmUpperIdx,
                          rIspCamInfo.rIspIntInfo.u2UdmLowerIdx);

                SmoothUDM(rIspCamInfo.u4ISOValue,  // Real ISO
                rIspCamInfo.rIspIntInfo.u4UdmUpperISO, // Upper ISO
                rIspCamInfo.rIspIntInfo.u4UdmLowerISO, // Lower ISO
                m_IspNvramMgr.getUDM(rIspCamInfo.rIspIntInfo.u2UdmUpperIdx), // CFA settings for upper ISO
                m_IspNvramMgr.getUDM(rIspCamInfo.rIspIntInfo.u2UdmLowerIdx), // CFA settings for lower ISO
                udm);  // Output

             }
         }
    }

#endif

    // Invoke callback for customers to modify.
    if  ( isDynamicTuning() )
    {   //  Dynamic Tuning: Enable
        m_pIspTuningCustom->refine_UDM(rIspCamInfo, m_IspNvramMgr, udm);
    }

    // FG mode protection
    if (fgRPGEnable && ((rIspCamInfo.eIspProfile == EIspProfile_Preview)      ||
                        (rIspCamInfo.eIspProfile == EIspProfile_Video)        ||
                        (rIspCamInfo.eIspProfile == EIspProfile_N3D_Preview)  ||
                        (rIspCamInfo.eIspProfile == EIspProfile_N3D_Video)    ||
                        (rIspCamInfo.eIspProfile == EIspProfile_IHDR_Preview) ||
                        (rIspCamInfo.eIspProfile == EIspProfile_IHDR_Video)   ||
                        (rIspCamInfo.eIspProfile == EIspProfile_MHDR_Preview) ||
                        (rIspCamInfo.eIspProfile == EIspProfile_MHDR_Video)   ||
                        (rIspCamInfo.eIspProfile == EIspProfile_zHDR_Preview) ||
                        (rIspCamInfo.eIspProfile == EIspProfile_zHDR_Video)     ))
    {
        udm.dsb.bits.UDM_FL_MODE = 1;
    }
    else {
        udm.dsb.bits.UDM_FL_MODE = 0;
    }

    if ( (m_bMono) || (!rUdm.isCCTEnable()) || (bDisable)) // bypass UDM
    {
        //udm = m_IspNvramMgr.getUDM(NVRAM_UDM_DISABLE_IDX);
        udm.intp_crs.bits.UDM_BYP = 1;
    }

    // Load it to ISP manager buffer.
    rUdm.put(udm);


    return  MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_CCM(const RAWIspCamInfo& rIspCamInfo, const IndexMgr& rIdx)
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.ccm.disable", value, "0"); // 0: enable, 1: disable
    MBOOL bDisable = atoi(value);

    MBOOL fgRPGEnable = rIspCamInfo.fgRPGEnable;
    MY_LOG_IF(m_bDebugEnable, "[%s] fgRPGEnable(%d)", __FUNCTION__, fgRPGEnable);

    if ((bDisable) ||
        (m_bMono)  ||
        (!ISP_MGR_CCM_T::getInstance(m_eSensorDev).isCCTEnable())            ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_Blending_All_Off)        ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_Blending_All_Off_SWNR)   ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_PostProc_Mixing)         ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_PostProc_Mixing_SWNR)    ||
        (rIspCamInfo.eIspProfile == EIspProfile_VFB_PostProc)                ||
        (rIspCamInfo.eIspProfile == EIspProfile_Capture_MultiPass_ANR_1)     ||
        (rIspCamInfo.eIspProfile == EIspProfile_Capture_MultiPass_ANR_2)     ||
        (rIspCamInfo.eIspProfile == EIspProfile_VSS_Capture_MultiPass_ANR_1) ||
        (rIspCamInfo.eIspProfile == EIspProfile_VSS_Capture_MultiPass_ANR_2) ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_MultiPass_ANR_1)         ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_MultiPass_ANR_2)) {
        ISP_MGR_CCM_T::getInstance(m_eSensorDev).setEnable(MFALSE);
    }
    else {
        //if (getOperMode() != EOperMode_Meta)
            ISP_MGR_CCM_T::getInstance(m_eSensorDev).setEnable(MTRUE);

        // Load it to ISP manager buffer.
        ISP_MGR_CCM_T::getInstance(m_eSensorDev).put(rIspCamInfo.rMtkCCM);
    }

    ISP_MGR_CCM_T::getInstance(m_eSensorDev).setISO(rIspCamInfo.u4ISOValue);

    return  MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_GGM(const RAWIspCamInfo& rIspCamInfo, const IndexMgr& rIdx)
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.ggm.disable", value, "0"); // 0: enable, 1: disable
    MBOOL bDisable = atoi(value);
    MBOOL fgRPGEnable = rIspCamInfo.fgRPGEnable;
    MY_LOG_IF(m_bDebugEnable, "[%s] fgRPGEnable(%d)", __FUNCTION__, fgRPGEnable);

    ISP_MGR_GGM_T& rGgm = ISP_MGR_GGM_T::getInstance(m_eSensorDev);

    if ((rIspCamInfo.eIspProfile == EIspProfile_MFB_Blending_All_Off)        ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_Blending_All_Off_SWNR)   ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_PostProc_Mixing)         ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_PostProc_Mixing_SWNR)    ||
        (rIspCamInfo.eIspProfile == EIspProfile_VFB_PostProc)                ||
        (rIspCamInfo.eIspProfile == EIspProfile_Capture_MultiPass_ANR_1)     ||
        (rIspCamInfo.eIspProfile == EIspProfile_Capture_MultiPass_ANR_2)     ||
        (rIspCamInfo.eIspProfile == EIspProfile_VSS_Capture_MultiPass_ANR_1) ||
        (rIspCamInfo.eIspProfile == EIspProfile_VSS_Capture_MultiPass_ANR_2) ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_MultiPass_ANR_1)         ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_MultiPass_ANR_2)) {
        return  MTRUE;
    }


    if ( (!rGgm.isCCTEnable()) || (bDisable) ) // GGM_LNR = 1
    {
        rGgm.setLNREnable(MTRUE);
    }
    else 
    {
        rGgm.setLNREnable(MFALSE);

        // Load it to ISP manager buffer.
        rGgm.put(rIspCamInfo.rMtkGGM);
    }


    return  MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_ANR(RAWIspCamInfo& rIspCamInfo, const IndexMgr& rIdx)
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.anr.disable", value, "0"); // 0: enable, 1: disable
    //0807forNE, disable ANR module
    MBOOL bDisable = atoi(value);
    MBOOL fgRPGEnable = rIspCamInfo.fgRPGEnable;
    MY_LOG_IF(m_bDebugEnable, "[%s] fgRPGEnable(%d)", __FUNCTION__, fgRPGEnable);

    ISP_MGR_NBC_T& rNbc = ISP_MGR_NBC_T::getInstance(m_eSensorDev);


    if ((bDisable) ||
        (!rNbc.isCCTANR1Enable()) ||
        (rIspCamInfo.eNRMode == MTK_NOISE_REDUCTION_MODE_OFF) ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_Blending_All_Off) ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_Blending_All_Off_SWNR)||
        (m_rIspCamInfo.eIspProfile == EIspProfile_VSS_MFB_Blending_All_Off)      ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_VSS_MFB_Blending_All_Off_SWNR)
//        (rIspCamInfo.eIspProfile == EIspProfile_Capture_SWNR) ||
//        (rIspCamInfo.eIspProfile == EIspProfile_VSS_Capture_SWNR) ||
//        (rIspCamInfo.eIspProfile == EIspProfile_PureRAW_Capture_SWNR) ||
//        (rIspCamInfo.eIspProfile == EIspProfile_MFB_Capture_EE_Off_SWNR) ||
//        (rIspCamInfo.eIspProfile == EIspProfile_MFB_PostProc_ANR_EE_SWNR) ||
//        (rIspCamInfo.eIspProfile == EIspProfile_MFB_PostProc_Mixing_SWNR)) )
    )
    {

        rNbc.setANR1Enable(MFALSE);
    }
    else {

        rNbc.setANR1Enable(MTRUE);

        // Get default NVRAM parameter
        ISP_NVRAM_ANR_T anr = m_IspNvramMgr.getANR(rIdx.getIdx_ANR());


#if 0

        if (m_pIspTuningCustom->is_to_invoke_isp_interpolation(rIspCamInfo)) {
             if (rIspCamInfo.u4ISOValue >= m_pIspTuningCustom->map_ISO_index_to_value(rIspCamInfo.eIdx_ISO)) {
                 if (rIspCamInfo.eIdx_ISO < eIDX_ISO_3200) {
                    rIspCamInfo.rIspIntInfo.u4AnrUpperISO = m_pIspTuningCustom->map_ISO_index_to_value(static_cast<EIndex_ISO_T>(rIspCamInfo.eIdx_ISO+1));
                    rIspCamInfo.rIspIntInfo.u4AnrLowerISO = m_pIspTuningCustom->map_ISO_index_to_value(rIspCamInfo.eIdx_ISO);
                    rIspCamInfo.rIspIntInfo.u2AnrUpperIdx = m_IspNvramMgr.getIdx_ANR()+1;
                    rIspCamInfo.rIspIntInfo.u2AnrLowerIdx = m_IspNvramMgr.getIdx_ANR();

                    MY_LOG_IF(m_bDebugEnable,"%s(): rIspCamInfo.u4ISOValue = %d, u4UpperISO = %d, u4LowerISO = %d, u2UpperIdx = %d, u2LowerIdx = %d\n",
                              __FUNCTION__, rIspCamInfo.u4ISOValue,
                              rIspCamInfo.rIspIntInfo.u4AnrUpperISO,
                              rIspCamInfo.rIspIntInfo.u4AnrLowerISO,
                              rIspCamInfo.rIspIntInfo.u2AnrUpperIdx,
                              rIspCamInfo.rIspIntInfo.u2AnrLowerIdx);

                    SmoothANR(rIspCamInfo.u4ISOValue,  // Real ISO
                              rIspCamInfo.rIspIntInfo.u4AnrUpperISO, // Upper ISO
                              rIspCamInfo.rIspIntInfo.u4AnrLowerISO, // Lower ISO
                              m_IspNvramMgr.getANR(rIspCamInfo.rIspIntInfo.u2AnrUpperIdx), // ANR settings for upper ISO
                              m_IspNvramMgr.getANR(rIspCamInfo.rIspIntInfo.u2AnrLowerIdx), // ANR settings for lower ISO
                              anr);  // Output

                 }
             }
             else {
                 if (rIspCamInfo.eIdx_ISO > eIDX_ISO_100) {
                    rIspCamInfo.rIspIntInfo.u4AnrUpperISO = m_pIspTuningCustom->map_ISO_index_to_value(rIspCamInfo.eIdx_ISO);
                    rIspCamInfo.rIspIntInfo.u4AnrLowerISO = m_pIspTuningCustom->map_ISO_index_to_value(static_cast<EIndex_ISO_T>(rIspCamInfo.eIdx_ISO-1));
                    rIspCamInfo.rIspIntInfo.u2AnrUpperIdx = m_IspNvramMgr.getIdx_ANR();
                    rIspCamInfo.rIspIntInfo.u2AnrLowerIdx = m_IspNvramMgr.getIdx_ANR()-1;

                    MY_LOG_IF(m_bDebugEnable,"%s(): rIspCamInfo.u4ISOValue = %d, u4UpperISO = %d, u4LowerISO = %d, u2UpperIdx = %d, u2LowerIdx = %d\n",
                              __FUNCTION__, rIspCamInfo.u4ISOValue,
                              rIspCamInfo.rIspIntInfo.u4AnrUpperISO,
                              rIspCamInfo.rIspIntInfo.u4AnrLowerISO,
                              rIspCamInfo.rIspIntInfo.u2AnrUpperIdx,
                              rIspCamInfo.rIspIntInfo.u2AnrLowerIdx);

                    SmoothANR(rIspCamInfo.u4ISOValue,  // Real ISO
                              rIspCamInfo.rIspIntInfo.u4AnrUpperISO, // Upper ISO
                              rIspCamInfo.rIspIntInfo.u4AnrLowerISO, // Lower ISO
                              m_IspNvramMgr.getANR(rIspCamInfo.rIspIntInfo.u2AnrUpperIdx), // ANR settings for upper ISO
                              m_IspNvramMgr.getANR(rIspCamInfo.rIspIntInfo.u2AnrLowerIdx), // ANR settings for lower ISO
                              anr);  // Output

                 }
             }
         }

#endif

        // Invoke callback for customers to modify.
        if  ( isDynamicTuning() )
        {   //  Dynamic Tuning: Enable
            m_pIspTuningCustom->refine_ANR(rIspCamInfo, m_IspNvramMgr, anr);
        }

        // set ANR_LCE_LINK
#warning "SWNR linker error"
#if 0
        SwNRParam::getInstance(m_i4SensorIdx)->setANR_LCE_LINK(static_cast<MBOOL>(anr.con1.bits.ANR_LCE_LINK));
        m_pLscMgr->setSwNr();
#endif
        // Load it to ISP manager buffer.
        rNbc.put(anr);
    }

    return  MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_ANR2(RAWIspCamInfo& rIspCamInfo, const IndexMgr& rIdx)
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.anr2.disable", value, "0"); // 0: enable, 1: disable
    MBOOL bDisable = atoi(value);
    MBOOL fgRPGEnable = rIspCamInfo.fgRPGEnable;
    MY_LOG_IF(m_bDebugEnable, "[%s] fgRPGEnable(%d)", __FUNCTION__, fgRPGEnable);

    ISP_MGR_NBC2_T& rNbc2 = ISP_MGR_NBC2_T::getInstance(m_eSensorDev);

    if ((bDisable) ||
        (!rNbc2.isCCTANR2Enable()) ||
        (rIspCamInfo.eNRMode == MTK_NOISE_REDUCTION_MODE_OFF) ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_Blending_All_Off) ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_Blending_All_Off_SWNR)||
        (m_rIspCamInfo.eIspProfile == EIspProfile_VSS_MFB_Blending_All_Off)      ||
        (m_rIspCamInfo.eIspProfile == EIspProfile_VSS_MFB_Blending_All_Off_SWNR)
//        (rIspCamInfo.eIspProfile == EIspProfile_Capture_SWNR) ||
//        (rIspCamInfo.eIspProfile == EIspProfile_VSS_Capture_SWNR) ||
//        (rIspCamInfo.eIspProfile == EIspProfile_PureRAW_Capture_SWNR) ||
//        (rIspCamInfo.eIspProfile == EIspProfile_MFB_Capture_EE_Off_SWNR) ||
//        (rIspCamInfo.eIspProfile == EIspProfile_MFB_PostProc_ANR_EE_SWNR) ||
//        (rIspCamInfo.eIspProfile == EIspProfile_MFB_PostProc_Mixing_SWNR)) )
    )
    {

        rNbc2.setANR2Enable(MFALSE);
    }

    else {
        rNbc2.setANR2Enable(MTRUE);

        // Get default NVRAM parameter
        ISP_NVRAM_ANR2_T anr2 = m_IspNvramMgr.getANR2(rIdx.getIdx_ANR2());
#if 0
        if (m_pIspTuningCustom->is_to_invoke_isp_interpolation(rIspCamInfo)) {
             if (rIspCamInfo.u4ISOValue >= m_pIspTuningCustom->map_ISO_index_to_value(rIspCamInfo.eIdx_ISO)) {
                 if (rIspCamInfo.eIdx_ISO < eIDX_ISO_3200) {
                    rIspCamInfo.rIspIntInfo.u4AnrUpperISO = m_pIspTuningCustom->map_ISO_index_to_value(static_cast<EIndex_ISO_T>(rIspCamInfo.eIdx_ISO+1));
                    rIspCamInfo.rIspIntInfo.u4AnrLowerISO = m_pIspTuningCustom->map_ISO_index_to_value(rIspCamInfo.eIdx_ISO);
                    rIspCamInfo.rIspIntInfo.u2Anr2UpperIdx = m_IspNvramMgr.getIdx_ANR2()+1;
                    rIspCamInfo.rIspIntInfo.u2Anr2LowerIdx = m_IspNvramMgr.getIdx_ANR2();

                    MY_LOG_IF(m_bDebugEnable,"%s(): rIspCamInfo.u4ISOValue = %d, u4UpperISO = %d, u4LowerISO = %d, u2UpperIdx = %d, u2LowerIdx = %d\n",
                              __FUNCTION__, rIspCamInfo.u4ISOValue,
                              rIspCamInfo.rIspIntInfo.u4AnrUpperISO,
                              rIspCamInfo.rIspIntInfo.u4AnrLowerISO,
                              rIspCamInfo.rIspIntInfo.u2Anr2UpperIdx,
                              rIspCamInfo.rIspIntInfo.u2Anr2LowerIdx);

                    SmoothANR2(rIspCamInfo.u4ISOValue,  // Real ISO
                              rIspCamInfo.rIspIntInfo.u4AnrUpperISO, // Upper ISO
                              rIspCamInfo.rIspIntInfo.u4AnrLowerISO, // Lower ISO
                              m_IspNvramMgr.getANR2(rIspCamInfo.rIspIntInfo.u2Anr2UpperIdx), // ANR settings for upper ISO
                              m_IspNvramMgr.getANR2(rIspCamInfo.rIspIntInfo.u2Anr2LowerIdx), // ANR settings for lower ISO
                              anr2);  // Output

                 }
             }
             else {
                 if (rIspCamInfo.eIdx_ISO > eIDX_ISO_100) {
                    rIspCamInfo.rIspIntInfo.u4AnrUpperISO = m_pIspTuningCustom->map_ISO_index_to_value(rIspCamInfo.eIdx_ISO);
                    rIspCamInfo.rIspIntInfo.u4AnrLowerISO = m_pIspTuningCustom->map_ISO_index_to_value(static_cast<EIndex_ISO_T>(rIspCamInfo.eIdx_ISO-1));
                    rIspCamInfo.rIspIntInfo.u2Anr2UpperIdx = m_IspNvramMgr.getIdx_ANR2();
                    rIspCamInfo.rIspIntInfo.u2Anr2LowerIdx = m_IspNvramMgr.getIdx_ANR2()-1;

                    MY_LOG_IF(m_bDebugEnable,"%s(): rIspCamInfo.u4ISOValue = %d, u4UpperISO = %d, u4LowerISO = %d, u2UpperIdx = %d, u2LowerIdx = %d\n",
                              __FUNCTION__, rIspCamInfo.u4ISOValue,
                              rIspCamInfo.rIspIntInfo.u4AnrUpperISO,
                              rIspCamInfo.rIspIntInfo.u4AnrLowerISO,
                              rIspCamInfo.rIspIntInfo.u2Anr2UpperIdx,
                              rIspCamInfo.rIspIntInfo.u2Anr2LowerIdx);

                    SmoothANR2(rIspCamInfo.u4ISOValue,  // Real ISO
                              rIspCamInfo.rIspIntInfo.u4AnrUpperISO, // Upper ISO
                              rIspCamInfo.rIspIntInfo.u4AnrLowerISO, // Lower ISO
                              m_IspNvramMgr.getANR2(rIspCamInfo.rIspIntInfo.u2Anr2UpperIdx), // ANR settings for upper ISO
                              m_IspNvramMgr.getANR2(rIspCamInfo.rIspIntInfo.u2Anr2LowerIdx), // ANR settings for lower ISO
                              anr2);  // Output

                 }
             }
         }

#endif

        // Invoke callback for customers to modify.
        if  ( isDynamicTuning() )
        {   //  Dynamic Tuning: Enable
            m_pIspTuningCustom->refine_ANR2(rIspCamInfo, m_IspNvramMgr, anr2);
        }

        // Load it to ISP manager buffer.
        rNbc2.put(anr2);
    }

    return  MTRUE;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_CCR(const RAWIspCamInfo& rIspCamInfo, const IndexMgr& rIdx)
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.ccr.disable", value, "0"); // 0: enable, 1: disable
    MBOOL bDisable = atoi(value);

    ISP_MGR_NBC2_T& rNbc2 = ISP_MGR_NBC2_T::getInstance(m_eSensorDev);
    MBOOL fgRPGEnable = rIspCamInfo.fgRPGEnable;
    MY_LOG_IF(m_bDebugEnable, "[%s] fgRPGEnable(%d)", __FUNCTION__, fgRPGEnable);

    if ((bDisable) ||
        (!rNbc2.isCCTCCREnable()) ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_Blending_All_Off)      ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_Blending_All_Off_SWNR) ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_PostProc_Mixing)       ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_PostProc_Mixing_SWNR)) {
        rNbc2.setCCREnable(MFALSE);
    }
    else {
        //if (getOperMode() != EOperMode_Meta)
        rNbc2.setCCREnable(MTRUE);

        // Get default NVRAM parameter
        ISP_NVRAM_CCR_T ccr = m_IspNvramMgr.getCCR(rIdx.getIdx_CCR());

        // Invoke callback for customers to modify.
        if  ( isDynamicTuning() )
        {   //  Dynamic Tuning: Enable
            m_pIspTuningCustom->refine_CCR(rIspCamInfo, m_IspNvramMgr, ccr);
        }

        // Load it to ISP manager buffer.
        rNbc2.put(ccr);
    }

    return  MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


MBOOL
Paramctrl::
prepareHw_PerFrame_BOK(const RAWIspCamInfo& rIspCamInfo, const IndexMgr& rIdx)

{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.bok.disable", value, "0"); // 0: enable, 1: disable
    MBOOL bDisable = atoi(value);
    ISP_MGR_NBC2_T& rNbc2 = ISP_MGR_NBC2_T::getInstance(m_eSensorDev);
    MBOOL fgRPGEnable = rIspCamInfo.fgRPGEnable;
    MY_LOG_IF(m_bDebugEnable, "[%s] fgRPGEnable(%d)", __FUNCTION__, fgRPGEnable);

    if ((bDisable) ||(!rNbc2.isCCTBOKEnable()) )
    {
        rNbc2.setBOKEnable(MFALSE);
    }
    else {
        //if (getOperMode() != EOperMode_Meta)
        rNbc2.setBOKEnable(MTRUE);

        // Get default NVRAM parameter
        ISP_NVRAM_BOK_T bok = m_IspNvramMgr.getBOK(rIdx.getIdx_BOK());

        // Invoke callback for customers to modify.
        if  ( isDynamicTuning() )
        {   //  Dynamic Tuning: Enable
            m_pIspTuningCustom->refine_BOK(rIspCamInfo, m_IspNvramMgr, bok);
        }

        // Load it to ISP manager buffer.
        rNbc2.put(bok);
    }

    return  MTRUE;
}

MBOOL
Paramctrl::
prepareHw_PerFrame_PCA(const RAWIspCamInfo& rIspCamInfo, const IndexMgr& rIdx)
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.pca.disable", value, "0"); // 0: enable, 1: disable
    MBOOL bDisable = atoi(value);
    ISP_MGR_PCA_T& rPca = ISP_MGR_PCA_T::getInstance(m_eSensorDev, m_ePCAMode);

    MBOOL fgIsToLoadLut = MFALSE;   //  MTRUE indicates to load LUT.
    MBOOL fgRPGEnable = rIspCamInfo.fgRPGEnable;
    MY_LOG_IF(m_bDebugEnable, "[%s] fgRPGEnable(%d)", __FUNCTION__, fgRPGEnable);

    if (rIspCamInfo.eIspProfile == EIspProfile_VFB_PostProc) {
        m_ePCAMode = EPCAMode_360BIN;
    }
    else {
        m_ePCAMode = EPCAMode_180BIN;
    }

    if ((bDisable) ||
        (!rPca.isCCTEnable())||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_Blending_All_Off)        ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_Blending_All_Off_SWNR)   ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_PostProc_Mixing)         ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_PostProc_Mixing_SWNR)    ||
        (rIspCamInfo.eIspProfile == EIspProfile_Capture_MultiPass_ANR_1)     ||
        (rIspCamInfo.eIspProfile == EIspProfile_VSS_Capture_MultiPass_ANR_1) ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_MultiPass_ANR_1)) {
        rPca.setEnable(MFALSE);
    }
    else {
        //if (getOperMode() != EOperMode_Meta)
        rPca.setEnable(MTRUE);
    }

    //  (1) Check to see whether PCA is enabled?
    if  (! m_pPcaMgr->isEnable())
    {
        return  MTRUE;
    }

    if (m_ePCAMode == EPCAMode_360BIN) { // for VFB
#warning "VFB linker error"
#if 0
        MY_LOG_IF(m_bDebugEnable,"%s: loading vFB PCA (0x%x) ...", __FUNCTION__, halVFBTuning::getInstance().mHalVFBTuningGetPCA());
        ISP_MGR_PCA_T::getInstance(m_eSensorDev, m_ePCAMode).loadLut(halVFBTuning::getInstance().mHalVFBTuningGetPCA());
#endif
    }
    else {
        // (2) Invoke callback for customers to modify.
        if  (isDynamicTuning())
        {   // Dynamic Tuning: Enable
            m_pPcaMgr->setIdx(static_cast<MUINT32>(rIspCamInfo.eIdx_PCA_LUT));
        }

        // Check to see if it is needed to load LUT.
        switch  (getOperMode())
        {
        case EOperMode_Normal:
        case EOperMode_PureRaw:
            fgIsToLoadLut = m_pPcaMgr->isChanged();   // Load if changed.
            break;
        default:
            fgIsToLoadLut = MTRUE;                  // Force to load.
            break;
        }

        if (fgIsToLoadLut) {
            m_pPcaMgr->loadLut();
            m_pPcaMgr->loadConfig();
        }
    }

    return  MTRUE;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_EE(RAWIspCamInfo& rIspCamInfo, const IndexMgr& rIdx)
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.ee.disable", value, "0"); // 0: enable, 1: disable
    MBOOL bDisable = atoi(value);
    ISP_MGR_SEEE_T& rSeee = ISP_MGR_SEEE_T::getInstance(m_eSensorDev);
    MBOOL fgRPGEnable = rIspCamInfo.fgRPGEnable;
    MY_LOG_IF(m_bDebugEnable, "[%s] fgRPGEnable(%d)", __FUNCTION__, fgRPGEnable);

    if ((bDisable) ||
        (!rSeee.isCCTEnable())           ||
        (rIspCamInfo.eEdgeMode == MTK_EDGE_MODE_OFF)                         ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_Capture_EE_Off)          ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_Capture_EE_Off_SWNR)     ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_Blending_All_Off)        ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_Blending_All_Off_SWNR)   ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_PostProc_EE_Off)         ||
        (rIspCamInfo.eIspProfile == EIspProfile_VFB_PostProc)                ||
        (rIspCamInfo.eIspProfile == EIspProfile_Capture_MultiPass_ANR_1)     ||
        (rIspCamInfo.eIspProfile == EIspProfile_VSS_Capture_MultiPass_ANR_1) ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_MultiPass_ANR_1))
    {
        rSeee.setEnable(MFALSE);
    }
    else {
        //if (getOperMode() != EOperMode_Meta)
        rSeee.setEnable(MTRUE);

        // Get default NVRAM parameter
        ISP_NVRAM_EE_T ee = m_IspNvramMgr.getEE(rIdx.getIdx_EE());

#if 0
        if (m_pIspTuningCustom->is_to_invoke_isp_interpolation(rIspCamInfo)) {
             if (rIspCamInfo.u4ISOValue >= m_pIspTuningCustom->map_ISO_index_to_value(rIspCamInfo.eIdx_ISO)) {
                 if (rIspCamInfo.eIdx_ISO < eIDX_ISO_3200) {
                    rIspCamInfo.rIspIntInfo.u4EEUpperISO = m_pIspTuningCustom->map_ISO_index_to_value(static_cast<EIndex_ISO_T>(rIspCamInfo.eIdx_ISO+1));
                    rIspCamInfo.rIspIntInfo.u4EELowerISO = m_pIspTuningCustom->map_ISO_index_to_value(rIspCamInfo.eIdx_ISO);
                    rIspCamInfo.rIspIntInfo.u2EEUpperIdx = m_IspNvramMgr.getIdx_EE()+1;
                    rIspCamInfo.rIspIntInfo.u2EELowerIdx = m_IspNvramMgr.getIdx_EE();

                    MY_LOG_IF(m_bDebugEnable,"%s(): rIspCamInfo.u4ISOValue = %d, u4UpperISO = %d, u4LowerISO = %d, u2UpperIdx = %d, u2LowerIdx = %d\n",
                              __FUNCTION__, rIspCamInfo.u4ISOValue,
                              rIspCamInfo.rIspIntInfo.u4EEUpperISO,
                              rIspCamInfo.rIspIntInfo.u4EELowerISO,
                              rIspCamInfo.rIspIntInfo.u2EEUpperIdx,
                              rIspCamInfo.rIspIntInfo.u2EELowerIdx);

                    SmoothEE(rIspCamInfo.u4ISOValue,  // Real ISO
                             rIspCamInfo.rIspIntInfo.u4EEUpperISO, // Upper ISO
                             rIspCamInfo.rIspIntInfo.u4EELowerISO, // Lower ISO
                             m_IspNvramMgr.getEE(rIspCamInfo.rIspIntInfo.u2EEUpperIdx), // EE settings for upper ISO
                             m_IspNvramMgr.getEE(rIspCamInfo.rIspIntInfo.u2EELowerIdx), // EE settings for lower ISO
                             ee);  // Output

                 }
             }
             else {
                 if (rIspCamInfo.eIdx_ISO > eIDX_ISO_100) {
                    rIspCamInfo.rIspIntInfo.u4EEUpperISO = m_pIspTuningCustom->map_ISO_index_to_value(rIspCamInfo.eIdx_ISO);
                    rIspCamInfo.rIspIntInfo.u4EELowerISO = m_pIspTuningCustom->map_ISO_index_to_value(static_cast<EIndex_ISO_T>(rIspCamInfo.eIdx_ISO-1));
                    rIspCamInfo.rIspIntInfo.u2EEUpperIdx = m_IspNvramMgr.getIdx_EE();
                    rIspCamInfo.rIspIntInfo.u2EELowerIdx = m_IspNvramMgr.getIdx_EE()-1;

                    MY_LOG_IF(m_bDebugEnable,"%s(): rIspCamInfo.u4ISOValue = %d, u4UpperISO = %d, u4LowerISO = %d, u2UpperIdx = %d, u2LowerIdx = %d\n",
                              __FUNCTION__, rIspCamInfo.u4ISOValue,
                              rIspCamInfo.rIspIntInfo.u4EEUpperISO,
                              rIspCamInfo.rIspIntInfo.u4EELowerISO,
                              rIspCamInfo.rIspIntInfo.u2EEUpperIdx,
                              rIspCamInfo.rIspIntInfo.u2EELowerIdx);

                    SmoothEE(rIspCamInfo.u4ISOValue,  // Real ISO
                             rIspCamInfo.rIspIntInfo.u4EEUpperISO, // Upper ISO
                             rIspCamInfo.rIspIntInfo.u4EELowerISO, // Lower ISO
                             m_IspNvramMgr.getEE(rIspCamInfo.rIspIntInfo.u2EEUpperIdx), // EE settings for upper ISO
                             m_IspNvramMgr.getEE(rIspCamInfo.rIspIntInfo.u2EELowerIdx), // EE settings for lower ISO
                             ee);  // Output

                 }
             }
         }

#endif

        // Invoke callback for customers to modify.
        if  ( isDynamicTuning() )
        {   //  Dynamic Tuning: Enable
            m_pIspTuningCustom->refine_EE(rIspCamInfo, m_IspNvramMgr, ee);

            if (rIspCamInfo.rIspUsrSelectLevel.eIdx_Edge != MTK_CONTROL_ISP_EDGE_MIDDLE)
            {
                // User setting
                m_pIspTuningCustom->userSetting_EE(rIspCamInfo, rIspCamInfo.rIspUsrSelectLevel.eIdx_Edge, ee);
            }
        }

        // Load it to ISP manager buffer.
        rSeee.put(ee);
    }

    return  MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// HSBC + Effect
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_EFFECT(const RAWIspCamInfo& rIspCamInfo, const IndexMgr& rIdx)
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.g2c.disable", value, "0"); // 0: enable, 1: disable
    MBOOL bDisable = atoi(value);
    ISP_MGR_G2C_T& rG2c = ISP_MGR_G2C_T::getInstance(m_eSensorDev);
    MBOOL fgRPGEnable = rIspCamInfo.fgRPGEnable;
    MY_LOG_IF(m_bDebugEnable, "[%s] fgRPGEnable(%d)", __FUNCTION__, fgRPGEnable);

    if ((bDisable) ||
        (m_bMono)    ||
        (!rG2c.isCCTEnable())              ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_Blending_All_Off)        ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_Blending_All_Off_SWNR)   ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_PostProc_Mixing)         ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_PostProc_Mixing_SWNR)    ||
        (rIspCamInfo.eIspProfile == EIspProfile_VFB_PostProc)                ||
        (rIspCamInfo.eIspProfile == EIspProfile_Capture_MultiPass_ANR_1)     ||
        (rIspCamInfo.eIspProfile == EIspProfile_VSS_Capture_MultiPass_ANR_1) ||
        (rIspCamInfo.eIspProfile == EIspProfile_MFB_MultiPass_ANR_1)) {
        rG2c.setEnable(MFALSE);
    }
    else {
        //if (getOperMode() != EOperMode_Meta) {
        rG2c.setEnable(MTRUE);
        //}

        ISP_NVRAM_G2C_T g2c;
        ISP_NVRAM_G2C_SHADE_T g2c_shade;
        ISP_NVRAM_SE_T se;
        ISP_NVRAM_GGM_T ggm;

        // Get ISP HW buffer
        getIspHWBuf(m_eSensorDev, g2c);
        getIspHWBuf(m_eSensorDev, g2c_shade);
        getIspHWBuf(m_eSensorDev, se);
//return to GGM setting        getIspHWBuf(m_eSensorDev, ggm);

        // Invoke callback for customers to modify.
        if  ( isDynamicTuning() )
        {   //  Dynamic Tuning: Enable
            m_pIspTuningCustom->userSetting_EFFECT(rIspCamInfo, rIspCamInfo.eIdx_Effect, rIspCamInfo.rIspUsrSelectLevel, g2c, g2c_shade, se, ggm);
        }

        // Load it to ISP manager buffer.
        rG2c.put(g2c);
        ISP_MGR_G2C_SHADE_T::getInstance(m_eSensorDev).put(g2c_shade);
        ISP_MGR_SEEE_T::getInstance(m_eSensorDev).put(se);
//return to GGM setting        putIspHWBuf(m_eSensorDev, ggm);
    }

    return  MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// NR3D
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_NR3D(const RAWIspCamInfo& rIspCamInfo, const IndexMgr& rIdx)
{
    // Get default NVRAM parameter
    ISP_NVRAM_NR3D_T nr3d = m_IspNvramMgr.getNR3D(rIdx.getIdx_NR3D());
    MBOOL fgRPGEnable = rIspCamInfo.fgRPGEnable;
    MY_LOG_IF(m_bDebugEnable, "[%s] fgRPGEnable(%d)", __FUNCTION__, fgRPGEnable);

    // Invoke callback for customers to modify.
    if  ( isDynamicTuning() )
    {   //  Dynamic Tuning: Enable
        m_pIspTuningCustom->refine_NR3D(rIspCamInfo, m_IspNvramMgr, nr3d);
    }

    // Load it to ISP manager buffer
    ISP_MGR_NR3D_T::getInstance(m_eSensorDev).put(nr3d);

    return  MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// MFB
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_MFB(const RAWIspCamInfo& rIspCamInfo, const IndexMgr& rIdx)
{
    // Get default NVRAM parameter
    ISP_NVRAM_MFB_T mfb = m_IspNvramMgr.getMFB(rIdx.getIdx_MFB());
    MBOOL fgRPGEnable = rIspCamInfo.fgRPGEnable;
    MY_LOG_IF(m_bDebugEnable, "[%s] fgRPGEnable(%d)", __FUNCTION__, fgRPGEnable);

    // Invoke callback for customers to modify.
    if  ( isDynamicTuning() )
    {   //  Dynamic Tuning: Enable
        m_pIspTuningCustom->refine_MFB(rIspCamInfo, m_IspNvramMgr, mfb);
    }

    // Load it to ISP manager buffer
    ISP_MGR_MFB_T::getInstance(m_eSensorDev).put(mfb);

    return  MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Mixer3
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_MIXER3(const RAWIspCamInfo& rIspCamInfo, const IndexMgr& rIdx)
{
#if 0
    // Get default NVRAM parameter
    ISP_NVRAM_MIXER3_T mixer3 = m_rIspParam.ISPMfbMixer;
#else
    // Since the original design can't apply mixer3 by ISO, and the nvram of mfb mix has never been used.
    // We steal the nvram of mfb mix and apply it to mixer3.
    // Get default NVRAM parameter
    ISP_NVRAM_MFB_T mfb = m_IspNvramMgr.getMFB(rIdx.getIdx_MFB());
    ISP_NVRAM_MIXER3_T mixer3;
    MBOOL fgRPGEnable = rIspCamInfo.fgRPGEnable;
    MY_LOG_IF(m_bDebugEnable, "[%s] fgRPGEnable(%d)", __FUNCTION__, fgRPGEnable);


/*
    How to mapping?
    //
    mixer3.ctrl_0.bits.MIX3_WT_SEL = 1;
    mixer3.ctrl_0.bits.MIX3_B0 = mfb.ll_con6.bits.BLD_LL_MX_B0;
    mixer3.ctrl_0.bits.MIX3_B1 = mfb.ll_con6.bits.BLD_LL_MX_B1;
    mixer3.ctrl_0.bits.MIX3_DT = mfb.ll_con5.bits.BLD_LL_MX_DT;
    //
    mixer3.ctrl_1.bits.MIX3_M0 = mfb.ll_con5.bits.BLD_LL_MX_M0;
    mixer3.ctrl_1.bits.MIX3_M1 = mfb.ll_con5.bits.BLD_LL_MX_M1;

*/

#endif

    // Invoke callback for customers to modify.
    if  ( isDynamicTuning() )
    {   //  Dynamic Tuning: Enable
        m_pIspTuningCustom->refine_MIXER3(rIspCamInfo, m_IspNvramMgr, mixer3);
    }

    // Load it to ISP manager buffer
    ISP_MGR_MIXER3_T::getInstance(m_eSensorDev).put(mixer3);

    return  MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// LCE
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
Paramctrl::
prepareHw_PerFrame_LCE(const RAWIspCamInfo& rIspCamInfo, const IndexMgr& rIdx)
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("isp.lce.disable", value, "0"); // 0: enable, 1: disable
    MBOOL bDisable = atoi(value);
    ISP_MGR_LCE_T& rLce = ISP_MGR_LCE_T::getInstance(m_eSensorDev);
    MBOOL fgRPGEnable = rIspCamInfo.fgRPGEnable;
    MY_LOG_IF(m_bDebugEnable, "[%s] fgRPGEnable(%d)", __FUNCTION__, fgRPGEnable);

    if ((bDisable) ||(!rLce.isCCTEnable()) )
    {
        rLce.setEnable(MFALSE);
    }
    else {
        //if (getOperMode() != EOperMode_Meta)

         if ( (rIspCamInfo.eIspProfile == EIspProfile_zHDR_Preview) ||
              (rIspCamInfo.eIspProfile == EIspProfile_zHDR_Video)   ||
              (rIspCamInfo.eIspProfile == EIspProfile_zHDR_Capture))
        {
            rLce.setEnable(MTRUE);
        } else {
            rLce.setEnable(MFALSE);
        }

        // Get default NVRAM parameter
        ISP_NVRAM_LCE_T lce = m_IspNvramMgr.getLCE(rIdx.getIdx_LCE());

        // Invoke callback for customers to modify.
        if  ( isDynamicTuning() )
        {   //  Dynamic Tuning: Enable
            m_pIspTuningCustom->refine_LCE(rIspCamInfo, m_IspNvramMgr, lce);
        }

        // Load it to ISP manager buffer.

        rLce.put(rIspCamInfo.rCropRzInfo, lce);

    }

    return  MTRUE;
}

