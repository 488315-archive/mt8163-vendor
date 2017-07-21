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
#ifndef _ISP_MGR_LCE_H_
#define _ISP_MGR_LCE_H_

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  OBC2
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
typedef class ISP_MGR_LCE : public ISP_MGR_BASE_T
{
    typedef ISP_MGR_LCE    MyType;
private:
    MBOOL m_bEnable;
    MBOOL m_bCCTEnable;
    MUINT32 m_u4StartAddr;

    enum
    {
        ERegInfo_DIP_X_LCE_CON,
        ERegInfo_DIP_X_LCE_ZR,
        ERegInfo_DIP_X_LCE_QUA,
        ERegInfo_DIP_X_LCE_SLM_LB,
        ERegInfo_DIP_X_LCE_SLM_SIZE,
        ERegInfo_DIP_X_LCE_OFST,
        ERegInfo_DIP_X_LCE_BIAS,
        ERegInfo_DIP_X_LCE_IMAGE_SIZE,
        ERegInfo_DIP_X_LCE_SLM,
        ERegInfo_DIP_X_LCE_OFFSET,
        ERegInfo_DIP_X_LCE_OUT,
        ERegInfo_NUM
    };

    RegInfo_T m_rIspRegInfo[ERegInfo_NUM];

protected:
    ISP_MGR_LCE(ESensorDev_T const eSensorDev)
        : ISP_MGR_BASE_T(m_rIspRegInfo, ERegInfo_NUM, m_u4StartAddr, eSensorDev)
        , m_bEnable(MFALSE)
        , m_bCCTEnable(MTRUE)
        , m_u4StartAddr(REG_ADDR_P2(DIP_X_LCE_CON))
    {
        // register info addr init
        INIT_REG_INFO_ADDR_P2(DIP_X_LCE_CON);
        INIT_REG_INFO_ADDR_P2(DIP_X_LCE_ZR);
        INIT_REG_INFO_ADDR_P2(DIP_X_LCE_QUA);
        INIT_REG_INFO_ADDR_P2(DIP_X_LCE_SLM_LB);
        INIT_REG_INFO_ADDR_P2(DIP_X_LCE_SLM_SIZE);
        INIT_REG_INFO_ADDR_P2(DIP_X_LCE_OFST);
        INIT_REG_INFO_ADDR_P2(DIP_X_LCE_BIAS);
        INIT_REG_INFO_ADDR_P2(DIP_X_LCE_IMAGE_SIZE);
        INIT_REG_INFO_ADDR_P2(DIP_X_LCE_SLM);
        INIT_REG_INFO_ADDR_P2(DIP_X_LCE_OFFSET);
        INIT_REG_INFO_ADDR_P2(DIP_X_LCE_OUT);

        INIT_REG_INFO_VALUE(DIP_X_LCE_CON, 0xFE);
        //INIT_REG_INFO_VALUE(DIP_X_LCE_ZR,);       //default :
        INIT_REG_INFO_VALUE(DIP_X_LCE_QUA, (67 | (16 << 9) | (2 << 16)));       //default : PA=67,PB=16, BA=2

        INIT_REG_INFO_VALUE(DIP_X_LCE_SLM_LB, 0x20);       //default : SLM_LB=32
        INIT_REG_INFO_VALUE(DIP_X_LCE_SLM_SIZE, (64 | (48 << 8)));       //default : WD=64, HT=48

        INIT_REG_INFO_VALUE(DIP_X_LCE_OFST, 0x0);          //default : 0
        INIT_REG_INFO_VALUE(DIP_X_LCE_BIAS, 0x0);          //default : 0
        //INIT_REG_INFO_VALUE(DIP_X_LCE_IMAGE_SIZ,);       //default : 1920x1088
    }

    virtual ~ISP_MGR_LCE() {}

public:
    static MyType&  getInstance(ESensorDev_T const eSensorDev);

public: // Interfaces.

    MBOOL
    isEnable()
    {
        return m_bEnable;
    }

    MVOID
    setEnable(MBOOL bEnable)
    {
        m_bEnable = bEnable;
    }

    MBOOL
    isCCTEnable()
    {
        return m_bCCTEnable;
    }

    MVOID
    setCCTEnable(MBOOL bEnable)
    {
        m_bCCTEnable = bEnable;
    }

    template <class ISP_xxx_T>
    MyType& put(const CROP_RZ_INFO_T& rRzCrop, ISP_xxx_T const& rParam);

    template <class ISP_xxx_T>
    MyType& get(ISP_xxx_T & rParam);

    template <class ISP_xxx_T>
    MBOOL get(ISP_xxx_T & rParam, const dip_x_reg_t* pReg) const;

    MBOOL apply(EIspProfile_T eIspProfile, dip_x_reg_t* pReg);
} ISP_MGR_LCE_T;

template <ESensorDev_T const eSensorDev>
class ISP_MGR_LCE_DEV : public ISP_MGR_LCE_T
{
public:
    static
    ISP_MGR_LCE_T&
    getInstance()
    {
        static ISP_MGR_LCE_DEV<eSensorDev> singleton;
        return singleton;
    }
    virtual MVOID destroyInstance() {}

    ISP_MGR_LCE_DEV()
        : ISP_MGR_LCE_T(eSensorDev)
    {}

    virtual ~ISP_MGR_LCE_DEV() {}

};

#endif


