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
#define LOG_TAG "isp_mgr_nr3d"

#ifndef ENABLE_MY_LOG
    #define ENABLE_MY_LOG       (0)
#endif

#include <cutils/properties.h>
#include <aaa_types.h>
#include <aaa_error_code.h>
#include <aaa_log.h>
#include <camera_custom_nvram.h>
#include "isp_mgr.h"

namespace NSIspTuningv3
{

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// NR3D
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ISP_MGR_NR3D_T&
ISP_MGR_NR3D_T::
getInstance(ESensorDev_T const eSensorDev)
{
    switch (eSensorDev)
    {
    case ESensorDev_Main: //  Main Sensor
        return  ISP_MGR_NR3D_DEV<ESensorDev_Main>::getInstance();
    case ESensorDev_MainSecond: //  Main Second Sensor
        return  ISP_MGR_NR3D_DEV<ESensorDev_MainSecond>::getInstance();
    case ESensorDev_Sub: //  Sub Sensor
        return  ISP_MGR_NR3D_DEV<ESensorDev_Sub>::getInstance();
    default:
        MY_ERR("eSensorDev = %d", eSensorDev);
        return  ISP_MGR_NR3D_DEV<ESensorDev_Main>::getInstance();
    }
}

template <>
ISP_MGR_NR3D_T&
ISP_MGR_NR3D_T::
put(ISP_NVRAM_NR3D_T const& rParam)
{
    PUT_REG_INFO(DIP_X_NR3D_AY_CON1    , ay_con1);
    PUT_REG_INFO(DIP_X_NR3D_AY_CON2    , ay_con2);
    PUT_REG_INFO(DIP_X_NR3D_AY_CON3    , ay_con3);
    PUT_REG_INFO(DIP_X_NR3D_AY_CON4    , ay_con4);
    PUT_REG_INFO(DIP_X_NR3D_AY_CON5    , ay_con5);
    PUT_REG_INFO(DIP_X_NR3D_AC_CON1    , ac_con1);
    PUT_REG_INFO(DIP_X_NR3D_AC_CON2    , ac_con2);
    PUT_REG_INFO(DIP_X_NR3D_AC_CON3    , ac_con3);
    PUT_REG_INFO(DIP_X_NR3D_AC_CON4    , ac_con4);
    PUT_REG_INFO(DIP_X_NR3D_AC_CON5    , ac_con5);
    PUT_REG_INFO(DIP_X_NR3D_B1Y_CON1   , b1y_con1);
    PUT_REG_INFO(DIP_X_NR3D_B1Y_CON2   , b1y_con2);
    PUT_REG_INFO(DIP_X_NR3D_B1Y_CON3   , b1y_con3);
    PUT_REG_INFO(DIP_X_NR3D_B1C_CON1   , b1c_con1);
    PUT_REG_INFO(DIP_X_NR3D_B1C_CON2   , b1c_con2);
    PUT_REG_INFO(DIP_X_NR3D_B1C_CON3   , b1c_con3);
    PUT_REG_INFO(DIP_X_NR3D_LSCG1_XCON , lscg1_xcon);
    PUT_REG_INFO(DIP_X_NR3D_LSCG1_YCON , lscg1_ycon);
    PUT_REG_INFO(DIP_X_NR3D_LSCG1_CON  , lscg1_con);
    PUT_REG_INFO(DIP_X_NR3D_LSCG1_SCON , lscg1_scon);
    PUT_REG_INFO(DIP_X_NR3D_NLY_CON1   , nly_con1);
    PUT_REG_INFO(DIP_X_NR3D_NLY_CON2   , nly_con2);
    PUT_REG_INFO(DIP_X_NR3D_MLY_CON    , mly_con);
    PUT_REG_INFO(DIP_X_NR3D_NLC_CON1   , nlc_con1);
    PUT_REG_INFO(DIP_X_NR3D_NLC_CON2   , nlc_con2);
    PUT_REG_INFO(DIP_X_NR3D_MLC_CON    , mlc_con);
    PUT_REG_INFO(DIP_X_NR3D_SLY_CON    , sly_con);
    PUT_REG_INFO(DIP_X_NR3D_SLC_CON    , slc_con);
    PUT_REG_INFO(DIP_X_NR3D_DIFG_CON   , difg_con);
    PUT_REG_INFO(DIP_X_NR3D_YCJC_CON1  , ycjc_con1);
    PUT_REG_INFO(DIP_X_NR3D_YCJC_CON2  , ycjc_con2);


    return  (*this);
}

template <>
ISP_MGR_NR3D_T&
ISP_MGR_NR3D_T::
get(ISP_NVRAM_NR3D_T& rParam)
{
    GET_REG_INFO(DIP_X_NR3D_AY_CON1    , ay_con1);
    GET_REG_INFO(DIP_X_NR3D_AY_CON2    , ay_con2);
    GET_REG_INFO(DIP_X_NR3D_AY_CON3    , ay_con3);
    GET_REG_INFO(DIP_X_NR3D_AY_CON4    , ay_con4);
    GET_REG_INFO(DIP_X_NR3D_AY_CON5    , ay_con5);
    GET_REG_INFO(DIP_X_NR3D_AC_CON1    , ac_con1);
    GET_REG_INFO(DIP_X_NR3D_AC_CON2    , ac_con2);
    GET_REG_INFO(DIP_X_NR3D_AC_CON3    , ac_con3);
    GET_REG_INFO(DIP_X_NR3D_AC_CON4    , ac_con4);
    GET_REG_INFO(DIP_X_NR3D_AC_CON5    , ac_con5);
    GET_REG_INFO(DIP_X_NR3D_B1Y_CON1   , b1y_con1);
    GET_REG_INFO(DIP_X_NR3D_B1Y_CON2   , b1y_con2);
    GET_REG_INFO(DIP_X_NR3D_B1Y_CON3   , b1y_con3);
    GET_REG_INFO(DIP_X_NR3D_B1C_CON1   , b1c_con1);
    GET_REG_INFO(DIP_X_NR3D_B1C_CON2   , b1c_con2);
    GET_REG_INFO(DIP_X_NR3D_B1C_CON3   , b1c_con3);
    GET_REG_INFO(DIP_X_NR3D_LSCG1_XCON , lscg1_xcon);
    GET_REG_INFO(DIP_X_NR3D_LSCG1_YCON , lscg1_ycon);
    GET_REG_INFO(DIP_X_NR3D_LSCG1_CON  , lscg1_con);
    GET_REG_INFO(DIP_X_NR3D_LSCG1_SCON , lscg1_scon);
    GET_REG_INFO(DIP_X_NR3D_NLY_CON1   , nly_con1);
    GET_REG_INFO(DIP_X_NR3D_NLY_CON2   , nly_con2);
    GET_REG_INFO(DIP_X_NR3D_MLY_CON    , mly_con);
    GET_REG_INFO(DIP_X_NR3D_NLC_CON1   , nlc_con1);
    GET_REG_INFO(DIP_X_NR3D_NLC_CON2   , nlc_con2);
    GET_REG_INFO(DIP_X_NR3D_MLC_CON    , mlc_con);
    GET_REG_INFO(DIP_X_NR3D_SLY_CON    , sly_con);
    GET_REG_INFO(DIP_X_NR3D_SLC_CON    , slc_con);
    GET_REG_INFO(DIP_X_NR3D_DIFG_CON   , difg_con);
    GET_REG_INFO(DIP_X_NR3D_YCJC_CON1  , ycjc_con1);
    GET_REG_INFO(DIP_X_NR3D_YCJC_CON2  , ycjc_con2);

    return  (*this);
}

template <>
MBOOL
ISP_MGR_NR3D_T::
get(ISP_NVRAM_NR3D_T& rParam, const dip_x_reg_t* pReg) const
{
    if (pReg)
    {
        GET_REG_INFO_BUF(DIP_X_NR3D_AY_CON1    , ay_con1);
        GET_REG_INFO_BUF(DIP_X_NR3D_AY_CON2    , ay_con2);
        GET_REG_INFO_BUF(DIP_X_NR3D_AY_CON3    , ay_con3);
        GET_REG_INFO_BUF(DIP_X_NR3D_AY_CON4    , ay_con4);
        GET_REG_INFO_BUF(DIP_X_NR3D_AY_CON5    , ay_con5);
        GET_REG_INFO_BUF(DIP_X_NR3D_AC_CON1    , ac_con1);
        GET_REG_INFO_BUF(DIP_X_NR3D_AC_CON2    , ac_con2);
        GET_REG_INFO_BUF(DIP_X_NR3D_AC_CON3    , ac_con3);
        GET_REG_INFO_BUF(DIP_X_NR3D_AC_CON4    , ac_con4);
        GET_REG_INFO_BUF(DIP_X_NR3D_AC_CON5    , ac_con5);
        GET_REG_INFO_BUF(DIP_X_NR3D_B1Y_CON1   , b1y_con1);
        GET_REG_INFO_BUF(DIP_X_NR3D_B1Y_CON2   , b1y_con2);
        GET_REG_INFO_BUF(DIP_X_NR3D_B1Y_CON3   , b1y_con3);
        GET_REG_INFO_BUF(DIP_X_NR3D_B1C_CON1   , b1c_con1);
        GET_REG_INFO_BUF(DIP_X_NR3D_B1C_CON2   , b1c_con2);
        GET_REG_INFO_BUF(DIP_X_NR3D_B1C_CON3   , b1c_con3);
        GET_REG_INFO_BUF(DIP_X_NR3D_LSCG1_XCON , lscg1_xcon);
        GET_REG_INFO_BUF(DIP_X_NR3D_LSCG1_YCON , lscg1_ycon);
        GET_REG_INFO_BUF(DIP_X_NR3D_LSCG1_CON  , lscg1_con);
        GET_REG_INFO_BUF(DIP_X_NR3D_LSCG1_SCON , lscg1_scon);
        GET_REG_INFO_BUF(DIP_X_NR3D_NLY_CON1   , nly_con1);
        GET_REG_INFO_BUF(DIP_X_NR3D_NLY_CON2   , nly_con2);
        GET_REG_INFO_BUF(DIP_X_NR3D_MLY_CON    , mly_con);
        GET_REG_INFO_BUF(DIP_X_NR3D_NLC_CON1   , nlc_con1);
        GET_REG_INFO_BUF(DIP_X_NR3D_NLC_CON2   , nlc_con2);
        GET_REG_INFO_BUF(DIP_X_NR3D_MLC_CON    , mlc_con);
        GET_REG_INFO_BUF(DIP_X_NR3D_SLY_CON    , sly_con);
        GET_REG_INFO_BUF(DIP_X_NR3D_SLC_CON    , slc_con);
        GET_REG_INFO_BUF(DIP_X_NR3D_DIFG_CON   , difg_con);
        GET_REG_INFO_BUF(DIP_X_NR3D_YCJC_CON1  , ycjc_con1);
        GET_REG_INFO_BUF(DIP_X_NR3D_YCJC_CON2  , ycjc_con2);
    }
    return MTRUE;
}


MBOOL
ISP_MGR_NR3D_T::
apply(EIspProfile_T eIspProfile, dip_x_reg_t* pReg)
{

    MBOOL bNR3D_EN = isEnable();

    // TOP
    ISP_WRITE_ENABLE_BITS(pReg, DIP_X_CTL_YUV_EN, NR3D_EN, bNR3D_EN);
    ISP_MGR_CTL_EN_P2_T::getInstance(m_eSensorDev).setEnable_NR3D(bNR3D_EN);

    writeRegs(static_cast<RegInfo_T*>(m_pRegInfo), m_u4RegInfoNum, pReg);

    dumpRegInfo("NR3D");

    return  MTRUE;
}


}
