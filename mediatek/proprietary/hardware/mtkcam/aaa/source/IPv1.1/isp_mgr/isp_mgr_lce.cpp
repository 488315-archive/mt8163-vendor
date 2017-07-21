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
#define LOG_TAG "isp_mgr_lce"

#ifndef ENABLE_MY_LOG
    #define ENABLE_MY_LOG       (1)
#endif

#include <cutils/properties.h>
#include <aaa_types.h>
#include <aaa_error_code.h>
#include <aaa_log.h>
#include "isp_mgr.h"

#if defined(HAVE_AEE_FEATURE)
#include <aee.h>
#define AEE_ASSERT_LCE(String) \
          do { \
              aee_system_exception( \
                  LOG_TAG, \
                  NULL, \
                  DB_OPT_DEFAULT, \
                  String); \
          } while(0)
#else
#define AEE_ASSERT_LCE(String)
#endif

namespace NSIspTuningv3
{
#define CLAMP(x,min,max)       (((x) > (max)) ? (max) : (((x) < (min)) ? (min) : (x)))

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  LCE
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ISP_MGR_LCE_T&
ISP_MGR_LCE_T::
getInstance(ESensorDev_T const eSensorDev)
{
    switch (eSensorDev)
    {
    case ESensorDev_Main: //  Main Sensor
        return  ISP_MGR_LCE_DEV<ESensorDev_Main>::getInstance();
    case ESensorDev_MainSecond: //  Main Second Sensor
        return  ISP_MGR_LCE_DEV<ESensorDev_MainSecond>::getInstance();
    case ESensorDev_Sub: //  Sub Sensor
        return  ISP_MGR_LCE_DEV<ESensorDev_Sub>::getInstance();
    default:
        MY_ERR("eSensorDev(%d)", eSensorDev);
        return  ISP_MGR_LCE_DEV<ESensorDev_Main>::getInstance();
    }
}

template <>
ISP_MGR_LCE_T&
ISP_MGR_LCE_T::
put(const CROP_RZ_INFO_T& rRzCrop, ISP_NVRAM_LCE_T const& rParam)
{
    PUT_REG_INFO(DIP_X_LCE_QUA, qua);

    MINT32 i4RrzInX     = rRzCrop.i4FullW;
    MINT32 i4RrzInY     = rRzCrop.i4FullH;
    MINT32 i4RrzXoff    = rRzCrop.i4OfstX;
    MINT32 i4RrzYoff    = rRzCrop.i4OfstY;
    MINT32 i4RrzCropInX = rRzCrop.i4Width;
    MINT32 i4RrzCropInY = rRzCrop.i4Height;
    MINT32 i4RrzOutX    = rRzCrop.i4RzWidth;
    MINT32 i4RrzOutY    = rRzCrop.i4RzHeight;

    DIP_X_REG_LCE_SLM_SIZE slmSize;
    slmSize.Raw = REG_INFO_VALUE(DIP_X_LCE_SLM_SIZE);

    if (i4RrzCropInX == 0 || i4RrzCropInY == 0)
    {
        MY_ERR("Unexpected crop info width(%d), height(%d)", i4RrzCropInX, i4RrzCropInY);
        AEE_ASSERT_LCE("Unexpected crop info for LCE");
    }
    MINT32 lce_full_slm_wd  = slmSize.Bits.LCE_SLM_WD;
    MINT32 lce_full_slm_ht  = slmSize.Bits.LCE_SLM_HT;
    MINT32 lce_full_xoff    = i4RrzXoff * i4RrzOutX / i4RrzCropInX;
    MINT32 lce_full_yoff    = i4RrzYoff * i4RrzOutY / i4RrzCropInY;
    MINT32 lce_full_out_ht  = i4RrzInY * i4RrzOutY / i4RrzCropInY;
    MINT32 lce_bcmk_x       = 32768*(lce_full_slm_wd-1)/(i4RrzOutX*i4RrzInX/i4RrzCropInX-1);
    MINT32 lce_bcmk_y       = 32768*(lce_full_slm_ht-1)/(i4RrzOutY*i4RrzInY/i4RrzCropInY-1);

    DIP_X_REG_LCE_TPIPE_SLM     slm = {0};
    DIP_X_REG_LCE_TPIPE_OFFSET  offset = {0};
    DIP_X_REG_LCE_TPIPE_OUT     out = {0};
    DIP_X_REG_LCE_ZR            zr = {0};

    slm.Bits.LCE_TPIPE_SLM_WD       = lce_full_slm_wd;
    slm.Bits.LCE_TPIPE_SLM_HT       = lce_full_slm_ht;
    offset.Bits.LCE_TPIPE_OFFSET_X  = lce_full_xoff;
    offset.Bits.LCE_TPIPE_OFFSET_Y  = lce_full_yoff;
    out.Bits.LCE_TPIPE_OUT_HT       = lce_full_out_ht;
    zr.Bits.LCE_BCMK_X              = lce_bcmk_x;
    zr.Bits.LCE_BCMK_Y              = lce_bcmk_y;

    REG_INFO_VALUE(DIP_X_LCE_SLM)   = slm.Raw;
    REG_INFO_VALUE(DIP_X_LCE_OFFSET)= offset.Raw;
    REG_INFO_VALUE(DIP_X_LCE_OUT)   = out.Raw;
    REG_INFO_VALUE(DIP_X_LCE_ZR)    = zr.Raw;

    return  (*this);
}

template <>
ISP_MGR_LCE_T&
ISP_MGR_LCE_T::
get(ISP_NVRAM_LCE_T& rParam)
{
    GET_REG_INFO(DIP_X_LCE_QUA, qua);

    return  (*this);
}

template <>
MBOOL
ISP_MGR_LCE_T::
get(ISP_NVRAM_LCE_T& rParam, const dip_x_reg_t* pReg) const
{
    if (pReg)
    {
        GET_REG_INFO_BUF(DIP_X_LCE_QUA, qua);
    }
    return MTRUE;
}

MBOOL
ISP_MGR_LCE_T::
apply(EIspProfile_T eIspProfile, dip_x_reg_t* pReg)
{
    MBOOL bLCE_EN = isEnable();

    // TOP
    ISP_WRITE_ENABLE_BITS(pReg, DIP_X_CTL_RGB_EN, LCE_EN, bLCE_EN);
    ISP_MGR_CTL_EN_P2_T::getInstance(m_eSensorDev).setEnable_LCE(bLCE_EN);

    // Register setting
    writeRegs(static_cast<RegInfo_T*>(m_pRegInfo), m_u4RegInfoNum, pReg);

    dumpRegInfo("LCE");
    return  MTRUE;
}

}

