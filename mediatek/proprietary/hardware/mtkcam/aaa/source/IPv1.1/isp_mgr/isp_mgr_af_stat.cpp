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
#define LOG_TAG "isp_mgr_af_stat"

#ifndef ENABLE_MY_LOG
#define ENABLE_MY_LOG       (1)
#endif

#include <cutils/properties.h>
#include <aaa_types.h>
#include <aaa_error_code.h>
#include <aaa_log.h>
#include <tuning_mgr.h>
#include <isp_mgr_af_stat.h>

namespace NSIspTuningv3
{

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// AF Statistics Config
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ISP_MGR_AF_STAT_CONFIG_T&
ISP_MGR_AF_STAT_CONFIG_T::
getInstance(ESensorDev_T const eSensorDev)
{
    switch (eSensorDev)
    {
    case ESensorDev_Main: //  Main Sensor
        return  ISP_MGR_AF_STAT_CONFIG_DEV<ESensorDev_Main>::getInstance();
    case ESensorDev_MainSecond: //  Main Second Sensor
        return  ISP_MGR_AF_STAT_CONFIG_DEV<ESensorDev_MainSecond>::getInstance();
    case ESensorDev_Sub: //  Sub Sensor
        return  ISP_MGR_AF_STAT_CONFIG_DEV<ESensorDev_Sub>::getInstance();
    default:
        MY_ERR("eSensorDev = %d", eSensorDev);
        return  ISP_MGR_AF_STAT_CONFIG_DEV<ESensorDev_Main>::getInstance();
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID
ISP_MGR_AF_STAT_CONFIG_T::
init()
{
    memcpy( m_rIspRegInfoPre, m_rIspRegInfo, ERegInfo_NUM*sizeof(RegInfo_T));
    {
        Mutex::Autolock lock(m_Lock);
        m_bIsApplied = MFALSE;
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID
ISP_MGR_AF_STAT_CONFIG_T::
CheckBLKNumX( MINT32 &i4InHWBlkNumX, MINT32 &i4OutHWBlkNumX)
{
    if( i4InHWBlkNumX<MIN_AF_HW_WIN_X)
    {
        MY_LOG("%s WAR : set x blk to 1", __FUNCTION__);
        i4OutHWBlkNumX = MIN_AF_HW_WIN_X;
    }
    else if( i4InHWBlkNumX>MAX_AF_HW_WIN_X)
    {
        MY_LOG("%s WAR : set x blk to 128", __FUNCTION__);
        i4OutHWBlkNumX = MAX_AF_HW_WIN_X;
    }
    else
    {
        i4OutHWBlkNumX = i4InHWBlkNumX;
    }

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID
ISP_MGR_AF_STAT_CONFIG_T::
CheckBLKNumY( MINT32 &i4InHWBlkNumY, MINT32 &i4OutHWBlkNumY)
{
    if( i4InHWBlkNumY<MIN_AF_HW_WIN_Y)
    {
        MY_LOG("%s WAR : set y blk to 1", __FUNCTION__);
        i4OutHWBlkNumY = MIN_AF_HW_WIN_Y;
    }
    else if( i4InHWBlkNumY>MAX_AF_HW_WIN_Y)
    {
        MY_LOG("%s WAR : set y blk to 128", __FUNCTION__);
        i4OutHWBlkNumY = MAX_AF_HW_WIN_Y;
    }
    else
    {
        i4OutHWBlkNumY =i4InHWBlkNumY;
    }

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL
ISP_MGR_AF_STAT_CONFIG_T::
config(AF_CONFIG_T &a_sAFConfig, AREA_T &sOutHWROI, MINT32 &i4OutHWBlkNumX, MINT32 &i4OutHWBlkNumY)
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("debug.isp_mgr_af.enable", value, "0");
    m_bDebugEnable = atoi(value);

    MINT32  BinW = a_sAFConfig.sBIN_SZ.i4W;
    MINT32  BinH = a_sAFConfig.sBIN_SZ.i4H;
    MUINT32 af_v_gonly   = a_sAFConfig.AF_V_GONLY;
    MUINT32 af_v_avg_lvl = a_sAFConfig.AF_V_AVG_LVL;

    /**************************     CAM_REG_AF_CON     ********************************/
    CAM_REG_AF_CON reg_af_con;
    reg_af_con.Raw = 0x200000; //default value
    reg_af_con.Bits.AF_BLF_EN       = a_sAFConfig.AF_BLF[0]; /* AF_BLF_EN */
    reg_af_con.Bits.AF_BLF_D_LVL    = a_sAFConfig.AF_BLF[1]; /* AF_BLF_D_LVL */
    reg_af_con.Bits.AF_BLF_R_LVL    = a_sAFConfig.AF_BLF[2]; /* AF_BLF_R_LVL */
    reg_af_con.Bits.AF_BLF_VFIR_MUX = a_sAFConfig.AF_BLF[3]; /* AF_BLF_VFIR_MUX*/
    reg_af_con.Bits.AF_H_GONLY      = a_sAFConfig.AF_H_GONLY;
    reg_af_con.Bits.AF_V_GONLY      = af_v_gonly;
    reg_af_con.Bits.AF_V_AVG_LVL    = af_v_avg_lvl;
    MY_LOG_IF( m_bDebugEnable, "(0x1A004800) : AFCon 0x%x, V_Gonly %d, V_AVG_LVL %d",
           reg_af_con.Raw,
           af_v_gonly,
           af_v_avg_lvl);

    /**************************     CAM_REG_AF_SIZE     ********************************/
    CAM_REG_AF_SIZE reg_af_size;
    reg_af_size.Raw = 0;
    reg_af_size.Bits.AF_IMAGE_WD = a_sAFConfig.sBIN_SZ.i4W; /*case for no frontal binning*/


    /*************************     Configure ROI setting     *******************************/
    // Convert ROI coordinate from TG coordinate to BIN block coordinate.
    AREA_T Roi2HWCoord = AREA_T( a_sAFConfig.sRoi.i4X * a_sAFConfig.sBIN_SZ.i4W / a_sAFConfig.sTG_SZ.i4W,
                                 a_sAFConfig.sRoi.i4Y * a_sAFConfig.sBIN_SZ.i4H / a_sAFConfig.sTG_SZ.i4H,
                                 a_sAFConfig.sRoi.i4W * a_sAFConfig.sBIN_SZ.i4W / a_sAFConfig.sTG_SZ.i4W,
                                 a_sAFConfig.sRoi.i4H * a_sAFConfig.sBIN_SZ.i4H / a_sAFConfig.sTG_SZ.i4H,
                                 0);
    //min constraint
    MUINT32 minHSz=8;
    MUINT32 minVSz=1;
    if(     (af_v_avg_lvl == 3) && (af_v_gonly == 1)) minHSz = 32;
    else if((af_v_avg_lvl == 3) && (af_v_gonly == 0)) minHSz = 16;
    else if((af_v_avg_lvl == 2) && (af_v_gonly == 1)) minHSz = 16;
    else                                              minHSz =  8;

    // ROI boundary check :
    if( (Roi2HWCoord.i4X<0     ) ||
            (Roi2HWCoord.i4X>BinW  ) ||
            (Roi2HWCoord.i4W<minHSz) ||
            (BinW < (Roi2HWCoord.i4X+Roi2HWCoord.i4W))) /*X*/
    {
        MINT32 x = BinW*30/100;
        MINT32 w = BinW*40/100;
        MY_LOG("HW-%s WAR : X %d->%d, W %d->%d", __FUNCTION__, Roi2HWCoord.i4X, x, Roi2HWCoord.i4W, w);
        Roi2HWCoord.i4X = x;
        Roi2HWCoord.i4W = w;
    }
    if( (Roi2HWCoord.i4Y<0     ) ||
            (Roi2HWCoord.i4Y>BinH  ) ||
            (Roi2HWCoord.i4H<minVSz) ||
            (BinH < (Roi2HWCoord.i4Y+Roi2HWCoord.i4H))) /*Y*/
    {
        MINT32 y = BinH*30/100;
        MINT32 h = BinH*40/100;
        MY_LOG("HW-%s WAR : Y %d->%d, H %d->%d", __FUNCTION__, Roi2HWCoord.i4Y, y, Roi2HWCoord.i4H, h);
        Roi2HWCoord.i4Y = y;
        Roi2HWCoord.i4H = h;
    }
    AREA_T Roi2HWCoordTmp = Roi2HWCoord;

    /**************************     CAM_REG_AF_BLK_0     ******************************/
    CAM_REG_AF_BLK_0 reg_af_blk_0;
    reg_af_blk_0.Raw = 0;

    //-------------
    // AF block sz width
    //-------------
    CheckBLKNumX( a_sAFConfig.AF_BLK_XNUM, i4OutHWBlkNumX);

    MUINT32 win_h_size = Roi2HWCoord.i4W / i4OutHWBlkNumX;
    if( win_h_size > 254)
    {
        win_h_size = 254;
        MINT32 nh = Roi2HWCoord.i4W / win_h_size;
        MY_LOG("HW-%s WAR : BlkSzW set to 254, blkNumX %d->%d", __FUNCTION__, i4OutHWBlkNumX, nh);

        //Because block size is changed, check blcok number again.
        CheckBLKNumX( nh, i4OutHWBlkNumX);
    }
    else if( win_h_size < minHSz)
    {
        win_h_size = minHSz;
        MINT32 nh = Roi2HWCoord.i4W / win_h_size;
        MY_LOG("HW-%s WAR : BlkSzW set to min Sz %d, (%d,%d), blkNumX %d->%d", __FUNCTION__, minHSz, af_v_avg_lvl, af_v_gonly, i4OutHWBlkNumX, nh);

        //Because block size is changed, check blcok number again.
        CheckBLKNumX( nh, i4OutHWBlkNumX);
    }
    if (af_v_gonly == 1)
        win_h_size = win_h_size/4 * 4;
    else
        win_h_size = win_h_size/2 * 2;

    //-------------
    // AF block sz height
    //-------------
    CheckBLKNumY( a_sAFConfig.AF_BLK_YNUM, i4OutHWBlkNumY);

    MUINT32 win_v_size = Roi2HWCoord.i4H / i4OutHWBlkNumY;
    if (win_v_size > 255)
    {
        win_v_size = 255;
        MINT32 nv = Roi2HWCoord.i4H / win_v_size;
        MY_LOG("HW-%s WAR : BlkSzH set to 255, blkNumY %d->%d", __FUNCTION__, i4OutHWBlkNumY, nv);

        //Because block size is changed, check blcok number again.
        CheckBLKNumY( nv, i4OutHWBlkNumY);
    }
    else if( win_v_size < minVSz)
    {
        win_v_size = minVSz;
        MINT32 nv = Roi2HWCoord.i4H / win_v_size;
        MY_LOG("HW-%s WAR : BlkSzH set to min Sz 1, blkNumY %d->%d", __FUNCTION__, i4OutHWBlkNumY, nv);

        //Because block size is changed, check blcok number again.
        CheckBLKNumY( nv, i4OutHWBlkNumY);
    }



    //-------------
    // Set AF block size.
    //-------------
    reg_af_blk_0.Bits.AF_BLK_XSIZE = win_h_size;
    reg_af_blk_0.Bits.AF_BLK_YSIZE = win_v_size;
    MY_LOG_IF( m_bDebugEnable, "(0x1A004838) : AF_blk_sz [W]%d [H]%d",
           win_h_size,
           win_v_size);


    /**************************     CAM_REG_AF_VLD     ********************************/
    CAM_REG_AF_VLD reg_af_vld;
    reg_af_vld.Raw = 0;

    //-------------
    // Final HW ROI.
    //-------------
    Roi2HWCoord.i4W = i4OutHWBlkNumX*win_h_size;
    Roi2HWCoord.i4H = i4OutHWBlkNumY*win_v_size;
    Roi2HWCoord.i4X = Roi2HWCoordTmp.i4X+Roi2HWCoordTmp.i4W/2-Roi2HWCoord.i4W/2;
    Roi2HWCoord.i4Y = Roi2HWCoordTmp.i4Y+Roi2HWCoordTmp.i4H/2-Roi2HWCoord.i4H/2;

    //-------------
    // HW ROI Size checking.
    //-------------
    if( BinW < (Roi2HWCoord.i4X+i4OutHWBlkNumX*win_h_size))
    {
        MINT32 x = BinW - i4OutHWBlkNumX * win_h_size;
        MY_LOG("HW-%s WAR : X %d->%d", __FUNCTION__, Roi2HWCoord.i4X, x);
        Roi2HWCoord.i4X = x;
    }

    if( BinH < (Roi2HWCoord.i4Y+i4OutHWBlkNumY*win_v_size))
    {
        MINT32 y = BinH - i4OutHWBlkNumY * win_v_size;
        MY_LOG("HW-%s WAR : Y %d->%d", __FUNCTION__, Roi2HWCoord.i4Y, y);
        Roi2HWCoord.i4Y = y;
    }

    //constraint : The window start point must be multiples of 2
    Roi2HWCoord.i4X = (Roi2HWCoord.i4X/2)*2;
    Roi2HWCoord.i4Y = (Roi2HWCoord.i4Y/2)*2;

    reg_af_vld.Bits.AF_VLD_XSTART = Roi2HWCoord.i4X;
    reg_af_vld.Bits.AF_VLD_YSTART = Roi2HWCoord.i4Y;


    /**************************     CAM_REG_AF_BLK_1     ******************************/
    CAM_REG_AF_BLK_1 reg_af_blk_1;
    reg_af_blk_1.Raw = 0;
    //window num
    reg_af_blk_1.Bits.AF_BLK_XNUM = i4OutHWBlkNumX;
    reg_af_blk_1.Bits.AF_BLK_YNUM = i4OutHWBlkNumY;
    MY_LOG_IF( m_bDebugEnable, "(0x1A00483C) : AF_blk_num [nX]%d [nY]%d",
           i4OutHWBlkNumX,
           i4OutHWBlkNumY);


    // error check : should not be happened.
    if( BinW<(reg_af_vld.Bits.AF_VLD_XSTART+reg_af_blk_1.Bits.AF_BLK_XNUM*reg_af_blk_0.Bits.AF_BLK_XSIZE))
    {
        MY_ERR("HW-Configure AF ROI fail : [StartX] %d, [NumX] %d, [BlkSzX] %d, [ImgSzX] %d",
               reg_af_vld.Bits.AF_VLD_XSTART,
               reg_af_blk_1.Bits.AF_BLK_XNUM,
               reg_af_blk_0.Bits.AF_BLK_XSIZE,
               a_sAFConfig.sBIN_SZ.i4W);

    }
    if( BinH<(reg_af_vld.Bits.AF_VLD_YSTART+reg_af_blk_1.Bits.AF_BLK_YNUM*reg_af_blk_0.Bits.AF_BLK_YSIZE))
    {
        MY_ERR("HW-Configure AF ROI fail : [StartY] %d, [NumY] %d, [BlkSzY] %d, [ImgSzY] %d",
               reg_af_vld.Bits.AF_VLD_YSTART,
               reg_af_blk_1.Bits.AF_BLK_YNUM,
               reg_af_blk_0.Bits.AF_BLK_YSIZE,
               a_sAFConfig.sBIN_SZ.i4H);

    }


    MY_LOG("ROI-BIN : [X]%4d [Y]%4d [W]%4d [H]%4d -> [X]%4d [Y]%4d [W]%4d [H]%4d",
           Roi2HWCoordTmp.i4X,
           Roi2HWCoordTmp.i4Y,
           Roi2HWCoordTmp.i4W,
           Roi2HWCoordTmp.i4H,
           Roi2HWCoord.i4X,
           Roi2HWCoord.i4Y,
           Roi2HWCoord.i4W,
           Roi2HWCoord.i4H);

    // Now Roi2HWCoordTmp is the AF region which is output from algorithm.
    sOutHWROI = AREA_T( Roi2HWCoord.i4X * a_sAFConfig.sTG_SZ.i4W / a_sAFConfig.sBIN_SZ.i4W,
                        Roi2HWCoord.i4Y * a_sAFConfig.sTG_SZ.i4H / a_sAFConfig.sBIN_SZ.i4H,
                        Roi2HWCoord.i4W * a_sAFConfig.sTG_SZ.i4W / a_sAFConfig.sBIN_SZ.i4W,
                        Roi2HWCoord.i4H * a_sAFConfig.sTG_SZ.i4H / a_sAFConfig.sBIN_SZ.i4H,
                        0);

    MY_LOG("ROI-TG  : [X]%4d [Y]%4d [W]%4d [H]%4d -> [X]%4d [Y]%4d [W]%4d [H]%4d",
           a_sAFConfig.sRoi.i4X,
           a_sAFConfig.sRoi.i4Y,
           a_sAFConfig.sRoi.i4W,
           a_sAFConfig.sRoi.i4H,
           sOutHWROI.i4X,
           sOutHWROI.i4Y,
           sOutHWROI.i4W,
           sOutHWROI.i4H);


    /**************************     CAM_REG_AF_TH_0     ******************************/
    CAM_REG_AF_TH_0 reg_af_th_0;
    reg_af_th_0.Raw =0;
    reg_af_th_0.Bits.AF_H_TH_0 = a_sAFConfig.AF_TH_H[0];
    reg_af_th_0.Bits.AF_H_TH_1 = a_sAFConfig.AF_TH_H[1];
    /**************************     CAM_REG_AF_TH_1     ******************************/
    CAM_REG_AF_TH_1 reg_af_th_1;
    reg_af_th_1.Raw =0;
    reg_af_th_1.Bits.AF_V_TH     = a_sAFConfig.AF_TH_V;
    reg_af_th_1.Bits.AF_G_SAT_TH = a_sAFConfig.AF_TH_G_SAT;
    /**************************     CAM_REG_AF_FLT_1     ******************************/
    CAM_REG_AF_FLT_1 reg_af_flt_1;
    reg_af_flt_1.Raw =0;
    reg_af_flt_1.Bits.AF_HFLT0_P1 = a_sAFConfig.AF_FIL_H0[0];
    reg_af_flt_1.Bits.AF_HFLT0_P2 = a_sAFConfig.AF_FIL_H0[1];
    reg_af_flt_1.Bits.AF_HFLT0_P3 = a_sAFConfig.AF_FIL_H0[2];
    reg_af_flt_1.Bits.AF_HFLT0_P4 = a_sAFConfig.AF_FIL_H0[3];
    /**************************     CAM_REG_AF_FLT_2     ******************************/
    CAM_REG_AF_FLT_2 reg_af_flt_2;
    reg_af_flt_2.Raw =0;
    reg_af_flt_2.Bits.AF_HFLT0_P5 = a_sAFConfig.AF_FIL_H0[4];
    reg_af_flt_2.Bits.AF_HFLT0_P6 = a_sAFConfig.AF_FIL_H0[5];
    reg_af_flt_2.Bits.AF_HFLT0_P7 = a_sAFConfig.AF_FIL_H0[6];
    reg_af_flt_2.Bits.AF_HFLT0_P8 = a_sAFConfig.AF_FIL_H0[7];
    /**************************     CAM_REG_AF_FLT_3     ******************************/
    CAM_REG_AF_FLT_3 reg_af_flt_3;
    reg_af_flt_3.Raw =0;
    reg_af_flt_3.Bits.AF_HFLT0_P9  = a_sAFConfig.AF_FIL_H0[ 8];
    reg_af_flt_3.Bits.AF_HFLT0_P10 = a_sAFConfig.AF_FIL_H0[ 9];
    reg_af_flt_3.Bits.AF_HFLT0_P11 = a_sAFConfig.AF_FIL_H0[10];
    reg_af_flt_3.Bits.AF_HFLT0_P12 = a_sAFConfig.AF_FIL_H0[11];
    /**************************     CAM_REG_AF_FLT_4     ******************************/
    CAM_REG_AF_FLT_4 reg_af_flt_4;
    reg_af_flt_4.Raw =0;
    reg_af_flt_4.Bits.AF_HFLT1_P1 = a_sAFConfig.AF_FIL_H1[0];
    reg_af_flt_4.Bits.AF_HFLT1_P2 = a_sAFConfig.AF_FIL_H1[1];
    reg_af_flt_4.Bits.AF_HFLT1_P3 = a_sAFConfig.AF_FIL_H1[2];
    reg_af_flt_4.Bits.AF_HFLT1_P4 = a_sAFConfig.AF_FIL_H1[3];
    /**************************     CAM_REG_AF_FLT_5     ******************************/
    CAM_REG_AF_FLT_5 reg_af_flt_5;
    reg_af_flt_5.Raw =0;
    reg_af_flt_5.Bits.AF_HFLT1_P5 = a_sAFConfig.AF_FIL_H1[4];
    reg_af_flt_5.Bits.AF_HFLT1_P6 = a_sAFConfig.AF_FIL_H1[5];
    reg_af_flt_5.Bits.AF_HFLT1_P7 = a_sAFConfig.AF_FIL_H1[6];
    reg_af_flt_5.Bits.AF_HFLT1_P8 = a_sAFConfig.AF_FIL_H1[7];
    /**************************     CAM_REG_AF_FLT_6     ******************************/
    CAM_REG_AF_FLT_6 reg_af_flt_6;
    reg_af_flt_6.Raw =0;
    reg_af_flt_6.Bits.AF_HFLT1_P9  = a_sAFConfig.AF_FIL_H1[ 8];
    reg_af_flt_6.Bits.AF_HFLT1_P10 = a_sAFConfig.AF_FIL_H1[ 9];
    reg_af_flt_6.Bits.AF_HFLT1_P11 = a_sAFConfig.AF_FIL_H1[10];
    reg_af_flt_6.Bits.AF_HFLT1_P12 = a_sAFConfig.AF_FIL_H1[11];
    /**************************     CAM_REG_AF_FLT_7     ******************************/
    CAM_REG_AF_FLT_7 reg_af_flt_7;
    reg_af_flt_7.Raw =0;
    reg_af_flt_7.Bits.AF_VFLT_X0 = a_sAFConfig.AF_FIL_V[0];
    reg_af_flt_7.Bits.AF_VFLT_X1 = a_sAFConfig.AF_FIL_V[1];
    /**************************     CAM_REG_AF_FLT_8     ******************************/
    CAM_REG_AF_FLT_8 reg_af_flt_8;
    reg_af_flt_8.Raw =0;
    reg_af_flt_8.Bits.AF_VFLT_X2 = a_sAFConfig.AF_FIL_V[2];
    reg_af_flt_8.Bits.AF_VFLT_X3 = a_sAFConfig.AF_FIL_V[3];
    /**************************     CAM_REG_SGG1_PGN     ******************************/
    CAM_REG_SGG1_PGN reg_sgg1_pgn;
    reg_sgg1_pgn.Raw =0;
    reg_sgg1_pgn.Bits.SGG_GAIN = a_sAFConfig.i4SGG_GAIN;
    /**************************     CAM_REG_SGG1_GMRC_1     ******************************/
    CAM_REG_SGG1_GMRC_1 reg_sgg1_gmrc_1;
    reg_sgg1_gmrc_1.Raw =0;
    reg_sgg1_gmrc_1.Bits.SGG_GMR_1 = a_sAFConfig.i4SGG_GMR1;
    reg_sgg1_gmrc_1.Bits.SGG_GMR_2 = a_sAFConfig.i4SGG_GMR2;
    reg_sgg1_gmrc_1.Bits.SGG_GMR_3 = a_sAFConfig.i4SGG_GMR3;
    reg_sgg1_gmrc_1.Bits.SGG_GMR_4 = a_sAFConfig.i4SGG_GMR4;
    /**************************     CAM_REG_SGG1_GMRC_2     ******************************/
    CAM_REG_SGG1_GMRC_2 reg_sgg1_gmrc_2;
    reg_sgg1_gmrc_2.Raw =0;
    reg_sgg1_gmrc_2.Bits.SGG_GMR_5 = a_sAFConfig.i4SGG_GMR5;
    reg_sgg1_gmrc_2.Bits.SGG_GMR_6 = a_sAFConfig.i4SGG_GMR6;
    reg_sgg1_gmrc_2.Bits.SGG_GMR_7 = a_sAFConfig.i4SGG_GMR7;

    //xsize/ysize
    MUINT32 xsize = reg_af_blk_1.Bits.AF_BLK_XNUM*16;
    MUINT32 ysize = reg_af_blk_1.Bits.AF_BLK_YNUM;
    REG_INFO_VALUE(CAM_AFO_XSIZE)   = xsize;
    REG_INFO_VALUE(CAM_AFO_YSIZE)   = ysize;
    //REG_INFO_VALUE(CAM_AFO_STRIDE) = xsize+1;
    REG_INFO_VALUE(CAM_AF_SIZE)     = reg_af_size.Raw;
    REG_INFO_VALUE(CAM_AF_CON)      = reg_af_con.Raw;
    REG_INFO_VALUE(CAM_AF_VLD)      = reg_af_vld.Raw;
    REG_INFO_VALUE(CAM_AF_BLK_0)    = reg_af_blk_0.Raw;
    REG_INFO_VALUE(CAM_AF_BLK_1)    = reg_af_blk_1.Raw;
    REG_INFO_VALUE(CAM_AF_TH_0)     = reg_af_th_0.Raw;
    REG_INFO_VALUE(CAM_AF_TH_1)     = reg_af_th_1.Raw;
    REG_INFO_VALUE(CAM_AF_FLT_1)    = reg_af_flt_1.Raw;
    REG_INFO_VALUE(CAM_AF_FLT_2)    = reg_af_flt_2.Raw;
    REG_INFO_VALUE(CAM_AF_FLT_3)    = reg_af_flt_3.Raw;
    REG_INFO_VALUE(CAM_AF_FLT_4)    = reg_af_flt_4.Raw;
    REG_INFO_VALUE(CAM_AF_FLT_5)    = reg_af_flt_5.Raw;
    REG_INFO_VALUE(CAM_AF_FLT_6)    = reg_af_flt_6.Raw;
    REG_INFO_VALUE(CAM_AF_FLT_7)    = reg_af_flt_7.Raw;
    REG_INFO_VALUE(CAM_AF_FLT_8)    = reg_af_flt_8.Raw;
    REG_INFO_VALUE(CAM_SGG1_PGN)    = reg_sgg1_pgn.Raw;
    REG_INFO_VALUE(CAM_SGG1_GMRC_1) = reg_sgg1_gmrc_1.Raw;
    REG_INFO_VALUE(CAM_SGG1_GMRC_2) = reg_sgg1_gmrc_2.Raw;

    if( memcmp( m_rIspRegInfo, m_rIspRegInfoPre, ERegInfo_NUM*sizeof(RegInfo_T))!=0)
    {
        {
            Mutex::Autolock lock(m_Lock);
            m_bIsApplied = MFALSE;

            MY_LOG("HW-AFOSz %d %d", xsize, ysize);
            MY_LOG("HW-AFSz %d", reg_af_size.Bits.AF_IMAGE_WD);
            MY_LOG("HW-BLF %d, %d, %d ,%d",
                   reg_af_con.Bits.AF_BLF_EN,
                   reg_af_con.Bits.AF_BLF_D_LVL,
                   reg_af_con.Bits.AF_BLF_R_LVL,
                   reg_af_con.Bits.AF_BLF_VFIR_MUX);
            MY_LOG("HW-GONLY H %d, V %d",
                   reg_af_con.Bits.AF_H_GONLY,
                   reg_af_con.Bits.AF_V_GONLY);
            MY_LOG("HW-VAvgLV %d", reg_af_con.Bits.AF_V_AVG_LVL);
            MY_LOG("HW-Start %d %d", reg_af_vld.Bits.AF_VLD_XSTART, reg_af_vld.Bits.AF_VLD_YSTART);
            MY_LOG("HW-BlkSz %d %d", reg_af_blk_0.Bits.AF_BLK_XSIZE, reg_af_blk_0.Bits.AF_BLK_YSIZE);
            MY_LOG("HW-BlkNum %d %d", reg_af_blk_1.Bits.AF_BLK_XNUM, reg_af_blk_1.Bits.AF_BLK_YNUM);
            MY_LOG("HW-TH_H %d %d", reg_af_th_0.Bits.AF_H_TH_0, reg_af_th_0.Bits.AF_H_TH_1);
            MY_LOG("HW-TH_V %d TH_G %d", reg_af_th_1.Bits.AF_V_TH, reg_af_th_1.Bits.AF_G_SAT_TH);
            MY_LOG("HW-FIL0 %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
                   reg_af_flt_1.Bits.AF_HFLT0_P1,
                   reg_af_flt_1.Bits.AF_HFLT0_P2,
                   reg_af_flt_1.Bits.AF_HFLT0_P3,
                   reg_af_flt_1.Bits.AF_HFLT0_P4,
                   reg_af_flt_2.Bits.AF_HFLT0_P5,
                   reg_af_flt_2.Bits.AF_HFLT0_P6,
                   reg_af_flt_2.Bits.AF_HFLT0_P7,
                   reg_af_flt_2.Bits.AF_HFLT0_P8,
                   reg_af_flt_3.Bits.AF_HFLT0_P9,
                   reg_af_flt_3.Bits.AF_HFLT0_P10,
                   reg_af_flt_3.Bits.AF_HFLT0_P11,
                   reg_af_flt_3.Bits.AF_HFLT0_P12);
            MY_LOG("HW-FIL1 %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
                   reg_af_flt_4.Bits.AF_HFLT1_P1,
                   reg_af_flt_4.Bits.AF_HFLT1_P2,
                   reg_af_flt_4.Bits.AF_HFLT1_P3,
                   reg_af_flt_4.Bits.AF_HFLT1_P4,
                   reg_af_flt_5.Bits.AF_HFLT1_P5,
                   reg_af_flt_5.Bits.AF_HFLT1_P6,
                   reg_af_flt_5.Bits.AF_HFLT1_P7,
                   reg_af_flt_5.Bits.AF_HFLT1_P8,
                   reg_af_flt_6.Bits.AF_HFLT1_P9,
                   reg_af_flt_6.Bits.AF_HFLT1_P10,
                   reg_af_flt_6.Bits.AF_HFLT1_P11,
                   reg_af_flt_6.Bits.AF_HFLT1_P12);
            MY_LOG("HW-FILV %d, %d, %d, %d",
                   reg_af_flt_7.Bits.AF_VFLT_X0,
                   reg_af_flt_7.Bits.AF_VFLT_X1,
                   reg_af_flt_8.Bits.AF_VFLT_X2,
                   reg_af_flt_8.Bits.AF_VFLT_X3);
            MY_LOG("HW-SGGGain %d", reg_sgg1_pgn.Bits.SGG_GAIN);
            MY_LOG("HW-SGG %d, %d, %d, %d, %d, %d, %d",
                   reg_sgg1_gmrc_1.Bits.SGG_GMR_1,
                   reg_sgg1_gmrc_1.Bits.SGG_GMR_2,
                   reg_sgg1_gmrc_1.Bits.SGG_GMR_3,
                   reg_sgg1_gmrc_1.Bits.SGG_GMR_4,
                   reg_sgg1_gmrc_2.Bits.SGG_GMR_5,
                   reg_sgg1_gmrc_2.Bits.SGG_GMR_6,
                   reg_sgg1_gmrc_2.Bits.SGG_GMR_7);

        }
        memcpy( m_rIspRegInfoPre, m_rIspRegInfo, ERegInfo_NUM*sizeof(RegInfo_T));
        MY_LOG("HW-%s", __FUNCTION__);
    }

    return MTRUE;
}

MBOOL
ISP_MGR_AF_STAT_CONFIG_T::
apply(TuningMgr& rTuning, MINT32 &i4Magic, MINT32 i4SubsampleIdex)
{
    rTuning.updateEngine(eTuningMgrFunc_AF, MTRUE, i4SubsampleIdex);
    rTuning.updateEngine(eTuningMgrFunc_SGG1, MTRUE, i4SubsampleIdex);

    // Register setting
    rTuning.tuningMgrWriteRegs( static_cast<TUNING_MGR_REG_IO_STRUCT*>(m_pRegInfo), m_u4RegInfoNum, i4SubsampleIdex);

    dumpRegInfo("af_stat_cfg");

    {
        Mutex::Autolock lock(m_Lock);
        if( m_bIsApplied==MFALSE)
        {
            m_bIsApplied = MTRUE;
            m_i4Magic    = (MUINT32)i4Magic;
            MY_LOG("HW-%s %d", __FUNCTION__, m_i4Magic);
        }
    }

    MY_LOG_IF(m_bDebugEnable, "%s", __FUNCTION__);
    return MTRUE;
}

MBOOL
ISP_MGR_AF_STAT_CONFIG_T::
isHWRdy( MUINT32  &i4Magic)
{
    MBOOL ret=MFALSE;
    {
        Mutex::Autolock lock(m_Lock);
        MY_LOG_IF(m_bDebugEnable, "%s %d, %d", __FUNCTION__, m_i4Magic, i4Magic);
        if( m_bIsApplied==MTRUE)
        {
            ret = i4Magic>=m_i4Magic ? MTRUE : MFALSE;
            if( i4Magic==m_i4Magic)
            {
                MY_LOG("HW-Rdy");
            }
        }
        else
        {
            ret = MFALSE;
        }
    }
    return ret;
}

}
