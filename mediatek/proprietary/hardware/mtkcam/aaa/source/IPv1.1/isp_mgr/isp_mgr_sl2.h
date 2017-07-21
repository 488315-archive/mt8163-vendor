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
#ifndef _ISP_MGR_SL2_H_
#define _ISP_MGR_SL2_H_

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  SL2
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
typedef class ISP_MGR_SL2 : public ISP_MGR_BASE_T
{
    typedef ISP_MGR_SL2    MyType;
private:
    MBOOL m_bEnable;
    MBOOL m_bSL2BOnOff;
    MBOOL m_bSL2COnOff;
    MBOOL m_bSL2DOnOff;
    MBOOL m_bSL2EOnOff;
    MBOOL m_bCCTEnable;
    MUINT32 m_u4StartAddr; // for debug purpose: 0x4F40
    MUINT32 m_SL2width;
    MUINT32 m_SL2height;

    enum
    {
        ERegInfo_DIP_X_SL2_CEN,
        ERegInfo_DIP_X_SL2_RR_CON0,
        ERegInfo_DIP_X_SL2_RR_CON1,
        ERegInfo_DIP_X_SL2_GAIN,
        ERegInfo_DIP_X_SL2_RZ,
        ERegInfo_DIP_X_SL2_XOFF,
        ERegInfo_DIP_X_SL2_YOFF,
        ERegInfo_DIP_X_SL2_SLP_CON0,
        ERegInfo_DIP_X_SL2_SLP_CON1,
        ERegInfo_DIP_X_SL2_SIZE,

        ERegInfo_DIP_X_SL2B_CEN,
        ERegInfo_DIP_X_SL2B_RR_CON0,
        ERegInfo_DIP_X_SL2B_RR_CON1,
        ERegInfo_DIP_X_SL2B_GAIN,
        ERegInfo_DIP_X_SL2B_RZ,
        ERegInfo_DIP_X_SL2B_XOFF,
        ERegInfo_DIP_X_SL2B_YOFF,
        ERegInfo_DIP_X_SL2B_SLP_CON0,
        ERegInfo_DIP_X_SL2B_SLP_CON1,
        ERegInfo_DIP_X_SL2B_SIZE,

        ERegInfo_DIP_X_SL2C_CEN,
        ERegInfo_DIP_X_SL2C_RR_CON0,
        ERegInfo_DIP_X_SL2C_RR_CON1,
        ERegInfo_DIP_X_SL2C_GAIN,
        ERegInfo_DIP_X_SL2C_RZ,
        ERegInfo_DIP_X_SL2C_XOFF,
        ERegInfo_DIP_X_SL2C_YOFF,
        ERegInfo_DIP_X_SL2C_SLP_CON0,
        ERegInfo_DIP_X_SL2C_SLP_CON1,
        ERegInfo_DIP_X_SL2C_SIZE,

        ERegInfo_DIP_X_SL2D_CEN,
        ERegInfo_DIP_X_SL2D_RR_CON0,
        ERegInfo_DIP_X_SL2D_RR_CON1,
        ERegInfo_DIP_X_SL2D_GAIN,
        ERegInfo_DIP_X_SL2D_RZ,
        ERegInfo_DIP_X_SL2D_XOFF,
        ERegInfo_DIP_X_SL2D_YOFF,
        ERegInfo_DIP_X_SL2D_SLP_CON0,
        ERegInfo_DIP_X_SL2D_SLP_CON1,
        ERegInfo_DIP_X_SL2D_SIZE,

        ERegInfo_DIP_X_SL2E_CEN,
        ERegInfo_DIP_X_SL2E_RR_CON0,
        ERegInfo_DIP_X_SL2E_RR_CON1,
        ERegInfo_DIP_X_SL2E_GAIN,
        ERegInfo_DIP_X_SL2E_RZ,
        ERegInfo_DIP_X_SL2E_XOFF,
        ERegInfo_DIP_X_SL2E_YOFF,
        ERegInfo_DIP_X_SL2E_SLP_CON0,
        ERegInfo_DIP_X_SL2E_SLP_CON1,
        ERegInfo_DIP_X_SL2E_SIZE,

        ERegInfo_NUM
    };

    RegInfo_T m_rIspRegInfo[ERegInfo_NUM];

protected:
    ISP_MGR_SL2(ESensorDev_T const eSensorDev)
        : ISP_MGR_BASE_T(m_rIspRegInfo, ERegInfo_NUM, m_u4StartAddr, eSensorDev)
        , m_bEnable(MTRUE)
        , m_bSL2BOnOff(MTRUE)
        , m_bSL2COnOff(MTRUE)
        , m_bSL2DOnOff(MTRUE)
        , m_bSL2EOnOff(MTRUE)
        , m_bCCTEnable(MTRUE)
        , m_u4StartAddr(REG_ADDR_P2(DIP_X_SL2_CEN))
    {
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2_CEN);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2_RR_CON0);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2_RR_CON1);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2_GAIN);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2_RZ);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2_XOFF);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2_YOFF);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2_SLP_CON0);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2_SLP_CON1);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2_SIZE);

        INIT_REG_INFO_ADDR_P2(DIP_X_SL2B_CEN);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2B_RR_CON0);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2B_RR_CON1);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2B_GAIN);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2B_RZ);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2B_XOFF);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2B_YOFF);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2B_SLP_CON0);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2B_SLP_CON1);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2B_SIZE);

        INIT_REG_INFO_ADDR_P2(DIP_X_SL2C_CEN);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2C_RR_CON0);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2C_RR_CON1);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2C_GAIN);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2C_RZ);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2C_XOFF);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2C_YOFF);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2C_SLP_CON0);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2C_SLP_CON1);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2C_SIZE);

        INIT_REG_INFO_ADDR_P2(DIP_X_SL2D_CEN);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2D_RR_CON0);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2D_RR_CON1);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2D_GAIN);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2D_RZ);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2D_XOFF);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2D_YOFF);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2D_SLP_CON0);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2D_SLP_CON1);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2D_SIZE);

        INIT_REG_INFO_ADDR_P2(DIP_X_SL2E_CEN);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2E_RR_CON0);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2E_RR_CON1);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2E_GAIN);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2E_RZ);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2E_XOFF);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2E_YOFF);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2E_SLP_CON0);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2E_SLP_CON1);
        INIT_REG_INFO_ADDR_P2(DIP_X_SL2E_SIZE);
    }

    virtual ~ISP_MGR_SL2() {}

public:
    static MyType&  getInstance(ESensorDev_T const eSensorDev);

public: // Interfaces.

    MBOOL
    isEnable()
    {
        return m_bEnable;
    }

    MBOOL getSL2BOnOff()
    {
        return m_bSL2BOnOff;
    }

    MBOOL getSL2COnOff()
    {
        return m_bSL2COnOff;
    }

    MBOOL getSL2DOnOff()
    {
        return m_bSL2DOnOff;
    }

    MBOOL getSL2EOnOff()
    {
        return m_bSL2EOnOff;
    }

    MVOID
    setEnable(MBOOL bEnable)
    {
        m_bEnable = bEnable;
    }

    MVOID
    setSL2Size(MUINT32 width, MUINT32 height)
    {
        m_SL2width  = width;
        m_SL2height = height;
    }

    MVOID
    setSL2BEnable(MBOOL bEnable)
    {
        m_bSL2BOnOff = bEnable;
    }

    MVOID
    setSL2CEnable(MBOOL bEnable)
    {
        m_bSL2COnOff = bEnable;
    }

    MVOID
    setSL2DEnable(MBOOL bEnable)
    {
        m_bSL2DOnOff = bEnable;
    }

    MVOID
    setSL2EEnable(MBOOL bEnable)
    {
        m_bSL2EOnOff = bEnable;
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
    MyType& put(const CROP_RZ_INFO_T& rCropRz, ISP_xxx_T const& rParam);

    template <class ISP_xxx_T>
    MyType& put_sl2e(const CROP_RZ_INFO_T& rCropRz, ISP_xxx_T const& rParam);

    template <class ISP_xxx_T>
    MyType& get(ISP_xxx_T & rParam);

    template <class ISP_xxx_T>
    MBOOL get(ISP_xxx_T & rParam, const dip_x_reg_t* pReg) const;

    MBOOL apply(EIspProfile_T eIspProfile, dip_x_reg_t* pReg);
} ISP_MGR_SL2_T;

template <ESensorDev_T const eSensorDev>
class ISP_MGR_SL2_DEV : public ISP_MGR_SL2_T
{
public:
    static
    ISP_MGR_SL2_T&
    getInstance()
    {
        static ISP_MGR_SL2_DEV<eSensorDev> singleton;
        return singleton;
    }
    virtual MVOID destroyInstance() {}

    ISP_MGR_SL2_DEV()
        : ISP_MGR_SL2_T(eSensorDev)
    {}

    virtual ~ISP_MGR_SL2_DEV() {}

};



#endif

