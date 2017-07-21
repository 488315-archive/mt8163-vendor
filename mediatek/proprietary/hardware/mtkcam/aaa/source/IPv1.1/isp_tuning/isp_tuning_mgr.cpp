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
#define LOG_TAG "isp_tuning_mgr"

#ifndef ENABLE_MY_LOG
    #define ENABLE_MY_LOG       (1)
#endif

#include <cutils/properties.h>
#include <stdlib.h>
#include <aaa_types.h>
#include <aaa_log.h>
#include <aaa_hal_if.h>
#include <camera_custom_nvram.h>
#include <isp_tuning.h>
#include <awb_param.h>
#include <af_param.h>
#include <flash_param.h>
#include <ae_param.h>
#include <isp_tuning_cam_info.h>
#include <paramctrl_if.h>
#include "isp_tuning_mgr.h"

using namespace NS3Av3;
using namespace NSIspTuning;
using namespace NSIspTuningv3;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
IspTuningMgr&
IspTuningMgr::
getInstance()
{
    static  IspTuningMgr singleton;
    return  singleton;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
IspTuningMgr::
IspTuningMgr()
    : m_pParamctrl_Main(MNULL)
    , m_pParamctrl_Sub(MNULL)
    , m_pParamctrl_Main2(MNULL)
    , m_i4SensorDev(0)
    , m_bDebugEnable(MFALSE)
    , m_i4IspProfile(0)
    , m_i4SensorIdx(0)
{


}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
IspTuningMgr::
~IspTuningMgr()
{

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::init(MINT32 const i4SensorDev, MINT32 const i4SensorIdx, MINT32 const i4SubsampleCount)
{
    m_i4SensorDev = i4SensorDev;
    m_i4SensorIdx = i4SensorIdx;

    MY_LOG("m_i4SensorDev= %d, m_i4SensorIdx = %d\n", m_i4SensorDev, m_i4SensorIdx);

    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("debug.isp_tuning_mgr.enable", value, "0");
    m_bDebugEnable = atoi(value);

    if (i4SensorDev & ESensorDev_Main) {
        if (!m_pParamctrl_Main) {
            m_pParamctrl_Main = IParamctrl::createInstance(ESensorDev_Main, m_i4SensorIdx);

            if (m_pParamctrl_Main) {
                m_pParamctrl_Main->init(i4SubsampleCount);
            }
            else {
                MY_ERR("m_pParamctrl_Main is NULL");
                return MFALSE;
            }
        }
        else {
            MY_ERR("m_pParamctrl_Main is not NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_Sub) {
        if (!m_pParamctrl_Sub) {
            m_pParamctrl_Sub = IParamctrl::createInstance(ESensorDev_Sub, m_i4SensorIdx);

            if (m_pParamctrl_Sub) {
                m_pParamctrl_Sub->init(i4SubsampleCount);
            }
            else {
                MY_ERR("m_pParamctrl_Sub is NULL");
                return MFALSE;
            }
        }
        else {
            MY_ERR("m_pParamctrl_Sub is not NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_MainSecond) {
        if (!m_pParamctrl_Main2) {
            m_pParamctrl_Main2 = IParamctrl::createInstance(ESensorDev_MainSecond, m_i4SensorIdx);

            if (m_pParamctrl_Main2) {
                m_pParamctrl_Main2->init(i4SubsampleCount);
            }
            else {
                MY_ERR("m_pParamctrl_Main2 is NULL");
                return MFALSE;
            }

        }
        else {
            MY_ERR("m_pParamctrl_Main2 is not NULL");
            return MFALSE;
        }
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::uninit(MINT32 const i4SensorDev)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->uninit();
            m_pParamctrl_Main->destroyInstance();
            m_pParamctrl_Main = MNULL;
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->uninit();
            m_pParamctrl_Sub->destroyInstance();
            m_pParamctrl_Sub = MNULL;
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->uninit();
            m_pParamctrl_Main2->destroyInstance();
            m_pParamctrl_Main2 = MNULL;
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setIspProfile(MINT32 const i4SensorDev, MINT32 const i4IspProfile)
{
    MY_LOG_IF(m_bDebugEnable,"setIspProfile: %d\n", i4IspProfile);

    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setIspProfile(static_cast<EIspProfile_T>(i4IspProfile));
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setIspProfile(static_cast<EIspProfile_T>(i4IspProfile));
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setIspProfile(static_cast<EIspProfile_T>(i4IspProfile));
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }

    m_i4IspProfile = i4IspProfile;

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setSceneMode(MINT32 const i4SensorDev, MUINT32 const u4Scene)
{
    MY_LOG_IF(m_bDebugEnable,"setSceneMode: %d\n", u4Scene);

    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setSceneMode(static_cast<EIndex_Scene_T>(u4Scene));
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setSceneMode(static_cast<EIndex_Scene_T>(u4Scene));
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setSceneMode(static_cast<EIndex_Scene_T>(u4Scene));
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setEffect(MINT32 const i4SensorDev, MUINT32 const u4Effect)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setEffect(static_cast<EIndex_Effect_T>(u4Effect));
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setEffect(static_cast<EIndex_Effect_T>(u4Effect));
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setEffect(static_cast<EIndex_Effect_T>(u4Effect));
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setOperMode(MINT32 const i4SensorDev, MINT32 const i4OperMode)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setOperMode(static_cast<EOperMode_T>(i4OperMode));
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setOperMode(static_cast<EOperMode_T>(i4OperMode));
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setOperMode(static_cast<EOperMode_T>(i4OperMode));
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }

    return MTRUE;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

MINT32 IspTuningMgr::getOperMode(MINT32 const i4SensorDev)
{
    MINT32 operMode = EOperMode_Normal;

    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
             operMode = m_pParamctrl_Main->getOperMode();
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            operMode = -1;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            operMode = m_pParamctrl_Sub->getOperMode();
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            operMode = -1;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            operMode = m_pParamctrl_Main2->getOperMode();
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            operMode = -1;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::getOperMode()\n");
        operMode = -1;
    }

    return operMode;

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setDynamicBypass(MINT32 const i4SensorDev, MBOOL i4Bypass)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->enableDynamicBypass(static_cast<MBOOL>(i4Bypass));
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->enableDynamicBypass(static_cast<MBOOL>(i4Bypass));
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->enableDynamicBypass(static_cast<MBOOL>(i4Bypass));
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 IspTuningMgr::getDynamicBypass(MINT32 const i4SensorDev)
{
    MINT32 bypass = MFALSE;

    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
             bypass = m_pParamctrl_Main->isDynamicBypass();
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            bypass = -1;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            bypass = m_pParamctrl_Sub->isDynamicBypass();
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            bypass = -1;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            bypass = m_pParamctrl_Main2->isDynamicBypass();
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            bypass = -1;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::getDynamicBypass()\n");
        bypass = -1;
    }

    return bypass;

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setDynamicCCM(MINT32 const i4SensorDev, MBOOL bdynamic_ccm)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->enableDynamicCCM(static_cast<MBOOL>(bdynamic_ccm));
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->enableDynamicCCM(static_cast<MBOOL>(bdynamic_ccm));
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->enableDynamicCCM(static_cast<MBOOL>(bdynamic_ccm));
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 IspTuningMgr::getDynamicCCM(MINT32 const i4SensorDev)
{
    MINT32 bypass = MFALSE;

    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
             bypass = m_pParamctrl_Main->isDynamicCCM();
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            bypass = -1;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            bypass = m_pParamctrl_Sub->isDynamicCCM();
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            bypass = -1;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            bypass = m_pParamctrl_Main2->isDynamicCCM();
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            bypass = -1;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::getDynamicCCM()\n");
        bypass = -1;
    }

    return bypass;

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::enableDynamicShading(MINT32 const i4SensorDev, MBOOL const fgEnable)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->enableDynamicShading(static_cast<MBOOL>(fgEnable));
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->enableDynamicShading(static_cast<MBOOL>(fgEnable));
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->enableDynamicShading(static_cast<MBOOL>(fgEnable));
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }

    return MTRUE;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setSensorMode(MINT32 const i4SensorDev, MINT32 const i4SensorMode)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setSensorMode(static_cast<ESensorMode_T>(i4SensorMode));
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setSensorMode(static_cast<ESensorMode_T>(i4SensorMode));
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setSensorMode(static_cast<ESensorMode_T>(i4SensorMode));
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }

    return MTRUE;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

MINT32 IspTuningMgr::getSensorMode(MINT32 const i4SensorDev)
{
    MINT32 sensorMode = ESensorMode_Capture;

    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
             sensorMode = m_pParamctrl_Main->getSensorMode();
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            sensorMode = -1;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            sensorMode = m_pParamctrl_Sub->getSensorMode();
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            sensorMode = -1;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            sensorMode = m_pParamctrl_Main2->getSensorMode();
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            sensorMode = -1;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::getSensorMode()\n");
        sensorMode = -1;
    }

    return sensorMode;

}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setZoomRatio(MINT32 const i4SensorDev, MINT32 const i4ZoomRatio_x100)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setZoomRatio(i4ZoomRatio_x100);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setZoomRatio(i4ZoomRatio_x100);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setZoomRatio(i4ZoomRatio_x100);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setAWBInfo(MINT32 const i4SensorDev, AWB_INFO_T const &rAWBInfo)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setAWBInfo(rAWBInfo);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setAWBInfo(rAWBInfo);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setAWBInfo(rAWBInfo);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setAEInfo(MINT32 const i4SensorDev, AE_INFO_T const &rAEInfo)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setAEInfo(rAEInfo);
        } else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setAEInfo(rAEInfo);
        } else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setAEInfo(rAEInfo);
        } else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setAFInfo(MINT32 const i4SensorDev, AF_INFO_T const &rAFInfo)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setAFInfo(rAFInfo);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setAFInfo(rAFInfo);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setAFInfo(rAFInfo);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setFlashInfo(MINT32 const i4SensorDev, FLASH_INFO_T const &rFlashInfo)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setFlashInfo(rFlashInfo);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setFlashInfo(rFlashInfo);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setFlashInfo(rFlashInfo);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setFlashDynamicInfo(MINT32 const i4SensorDev, MINT32 const i4FlashOnOff)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setFlashDynamicInfo(i4FlashOnOff);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setFlashDynamicInfo(i4FlashOnOff);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setFlashDynamicInfo(i4FlashOnOff);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }

    return MTRUE;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

MBOOL IspTuningMgr::setPureOBCInfo(MINT32 const i4SensorDev, const ISP_NVRAM_OBC_T *pOBCInfo)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setPureOBCInfo(pOBCInfo);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setPureOBCInfo(pOBCInfo);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setPureOBCInfo(pOBCInfo);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }

    return MTRUE;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

MBOOL IspTuningMgr::setIndex_Shading(MINT32 const i4SensorDev, MINT32 const i4IDX)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setIndex_Shading(i4IDX);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setIndex_Shading(i4IDX);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setIndex_Shading(i4IDX);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::getIndex_Shading(MINT32 const i4SensorDev, MVOID*const pCmdArg)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->getIndex_Shading(pCmdArg);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->getIndex_Shading(pCmdArg);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->getIndex_Shading(pCmdArg);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::getIndex_Shading()\n");
        return MFALSE;
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::getCamInfo(MINT32 const i4SensorDev, RAWIspCamInfo& rCamInfo) const
{
    MY_LOG_IF(m_bDebugEnable,"%s()\n", __FUNCTION__);

    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            rCamInfo = m_pParamctrl_Main->getCamInfo();
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            rCamInfo = m_pParamctrl_Sub->getCamInfo();
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            rCamInfo = m_pParamctrl_Main2->getCamInfo();
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::getColorCorrectionTransform()\n");
        return MFALSE;
    }
    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setIspUserIdx_Edge(MINT32 const i4SensorDev, MUINT32 const u4Index)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setIspUserIdx_Edge(static_cast<EIndex_Isp_Edge_T>(u4Index));
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setIspUserIdx_Edge(static_cast<EIndex_Isp_Edge_T>(u4Index));
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setIspUserIdx_Edge(static_cast<EIndex_Isp_Edge_T>(u4Index));
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setIspUserIdx_Hue(MINT32 const i4SensorDev, MUINT32 const u4Index)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setIspUserIdx_Hue(static_cast<EIndex_Isp_Hue_T>(u4Index));
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setIspUserIdx_Hue(static_cast<EIndex_Isp_Hue_T>(u4Index));
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setIspUserIdx_Hue(static_cast<EIndex_Isp_Hue_T>(u4Index));
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setIspUserIdx_Sat(MINT32 const i4SensorDev, MUINT32 const u4Index)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setIspUserIdx_Sat(static_cast<EIndex_Isp_Saturation_T>(u4Index));
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setIspUserIdx_Sat(static_cast<EIndex_Isp_Saturation_T>(u4Index));
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setIspUserIdx_Sat(static_cast<EIndex_Isp_Saturation_T>(u4Index));
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setIspUserIdx_Bright(MINT32 const i4SensorDev, MUINT32 const u4Index)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setIspUserIdx_Bright(static_cast<EIndex_Isp_Brightness_T>(u4Index));
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setIspUserIdx_Bright(static_cast<EIndex_Isp_Brightness_T>(u4Index));
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setIspUserIdx_Bright(static_cast<EIndex_Isp_Brightness_T>(u4Index));
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setIspUserIdx_Contrast(MINT32 const i4SensorDev, MUINT32 const u4Index)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setIspUserIdx_Contrast(static_cast<EIndex_Isp_Contrast_T>(u4Index));
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setIspUserIdx_Contrast(static_cast<EIndex_Isp_Contrast_T>(u4Index));
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setIspUserIdx_Contrast(static_cast<EIndex_Isp_Contrast_T>(u4Index));
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::validate(MINT32 const i4SensorDev,  RequestSet_T const RequestSet, MBOOL const fgForce, MINT32 i4SubsampleIdex)
{
    MINT32 i4FrameID = RequestSet.vNumberSet[0];
    MY_LOG_IF(m_bDebugEnable,"%s(): i4SensorDev = %d, i4FrameID = %d\n", __FUNCTION__, i4SensorDev, i4FrameID);

    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->validate(RequestSet, fgForce, i4SubsampleIdex);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->validate(RequestSet, fgForce, i4SubsampleIdex);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->validate(RequestSet, fgForce, i4SubsampleIdex);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::validatePerFrame(MINT32 const i4SensorDev, RequestSet_T const RequestSet, MBOOL const fgForce, MINT32 i4SubsampleIdex)
{
    MINT32 i4FrameID =  RequestSet.vNumberSet.front();
    MY_LOG_IF(m_bDebugEnable,"%s(): i4SensorDev = %d, i4FrameID = %d\n", __FUNCTION__, i4SensorDev, i4FrameID);

    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->validatePerFrame(RequestSet, fgForce, i4SubsampleIdex);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->validatePerFrame(RequestSet, fgForce, i4SubsampleIdex);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->validatePerFrame(RequestSet, fgForce, i4SubsampleIdex);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::validatePerFrameP2(MINT32 const i4SensorDev, MINT32 flowType, const ISP_INFO_T& rIspInfo, void* pRegBuf)
{
    MY_LOG_IF(m_bDebugEnable,"%s(): i4SensorDev = %d\n", __FUNCTION__, i4SensorDev);

    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->validatePerFrameP2(flowType, rIspInfo, pRegBuf);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->validatePerFrameP2(flowType, rIspInfo, pRegBuf);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }

    if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->validatePerFrameP2(flowType, rIspInfo, pRegBuf);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::getDebugInfoP1(MINT32 const i4SensorDev, NSIspExifDebug::IspExifDebugInfo_T& rIspExifDebugInfo, MBOOL const fgReadFromHW) const
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->getDebugInfoP1(rIspExifDebugInfo, fgReadFromHW);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->getDebugInfoP1(rIspExifDebugInfo, fgReadFromHW);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->getDebugInfoP1(rIspExifDebugInfo, fgReadFromHW);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::getDebugInfoP1()\n");
        return MFALSE;
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::getDebugInfo(MINT32 const i4SensorDev, const NSIspTuning::ISP_INFO_T& rIspInfo, NSIspExifDebug::IspExifDebugInfo_T& rIspExifDebugInfo, const void* pRegBuf)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->getDebugInfo(rIspInfo, rIspExifDebugInfo, pRegBuf);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->getDebugInfo(rIspInfo, rIspExifDebugInfo, pRegBuf);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->getDebugInfo(rIspInfo, rIspExifDebugInfo, pRegBuf);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::getDebugInfo()\n");
        return MFALSE;
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::notifyRPGEnable(MINT32 const i4SensorDev, MBOOL const bRPGEnable)
{
    MY_LOG_IF(m_bDebugEnable,"bRPGEnable: %d\n", bRPGEnable);

    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->notifyRPGEnable(bRPGEnable);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->notifyRPGEnable(bRPGEnable);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->notifyRPGEnable(bRPGEnable);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::notifyRPGEnable()\n");
        return MFALSE;
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setAWBGain(MINT32 const i4SensorDev, AWB_GAIN_T& rIspAWBGain)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setAWBGain(rIspAWBGain);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setAWBGain(rIspAWBGain);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setAWBGain(rIspAWBGain);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::setAWBGain()\n");
        return MFALSE;
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setISPAEGain(MINT32 const i4SensorDev, MBOOL bEnableWorkAround, MUINT32 u4IspGain)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setISPAEGain(bEnableWorkAround, u4IspGain);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setISPAEGain(bEnableWorkAround, u4IspGain);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setISPAEGain(bEnableWorkAround, u4IspGain);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::setISPAEGain()\n");
        return MFALSE;
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setIspAEPreGain2(MINT32 const i4SensorDev, MINT32 i4SensorIndex, AWB_GAIN_T& rNewIspAEPreGain2)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setIspAEPreGain2(i4SensorIndex, rNewIspAEPreGain2);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setIspAEPreGain2(i4SensorIndex, rNewIspAEPreGain2);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setIspAEPreGain2(i4SensorIndex, rNewIspAEPreGain2);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::setIspAEPreGain2()\n");
        return MFALSE;
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setIspFlareGainOffset(MINT32 const i4SensorDev, MINT16 i2FlareGain, MINT16 i2FlareOffset)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setIspFlareGainOffset(i2FlareGain, i2FlareOffset);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setIspFlareGainOffset(i2FlareGain, i2FlareOffset);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setIspFlareGainOffset(i2FlareGain, i2FlareOffset);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::setIspFlareGainOffset()\n");
        return MFALSE;
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setTGInfo(MINT32 const i4SensorDev, MINT32 const i4TGInfo)
{
    MY_LOG_IF(m_bDebugEnable,"%s(): i4SensorDev = %d, i4TGInfo = %d\n", __FUNCTION__, i4SensorDev, i4TGInfo);

    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setTGInfo(i4TGInfo);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setTGInfo(i4TGInfo);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setTGInfo(i4TGInfo);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::setTGInfo()\n");
        return MFALSE;
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID* IspTuningMgr::getDefaultISPIndex(MINT32 const i4SensorDev, MUINT32 eIspProfile, MUINT32 eSensorMode, MUINT32 eIdx_ISO)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            return m_pParamctrl_Main->getDefaultISPIndex(eIspProfile, eSensorMode, eIdx_ISO);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return NULL;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            return m_pParamctrl_Sub->getDefaultISPIndex(eIspProfile, eSensorMode, eIdx_ISO);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return NULL;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            return m_pParamctrl_Main2->getDefaultISPIndex(eIspProfile, eSensorMode, eIdx_ISO);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return NULL;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::getDefaultISPIndex()\n");
        return NULL;
    }

    return NULL;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID IspTuningMgr::sendIspTuningIOCtrl(MINT32 const i4SensorDev, E_ISPTUNING_CTRL const ctrl, MINTPTR arg1, MINTPTR arg2)
{
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->sendIspTuningIOCtrl(ctrl, arg1, arg2);
            return;
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->sendIspTuningIOCtrl(ctrl, arg1, arg2);
            return;
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->sendIspTuningIOCtrl(ctrl, arg1, arg2);
            return;
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::sendIspTuningIOCtrl()\n");
        return;
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::forceValidate(MINT32 const i4SensorDev)
{
    MY_LOG_IF(m_bDebugEnable,"%s(): i4SensorDev = %d\n", __FUNCTION__, i4SensorDev);

    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->forceValidate();
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->forceValidate();
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->forceValidate();
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::forceValidate()\n");
        return MFALSE;
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setCamMode(MINT32 const i4SensorDev, MINT32 const i4CamMode)
{
    MY_LOG_IF(m_bDebugEnable,"%s(): i4SensorDev = %d\n", __FUNCTION__, i4SensorDev);

    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setCamMode(i4CamMode);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setCamMode(i4CamMode);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setCamMode(i4CamMode);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::setCamMode()\n");
        return MFALSE;
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::getColorCorrectionTransform(MINT32 const i4SensorDev,
                                                 MFLOAT& M11, MFLOAT& M12, MFLOAT& M13,
                                                 MFLOAT& M21, MFLOAT& M22, MFLOAT& M23,
                                                 MFLOAT& M31, MFLOAT& M32, MFLOAT& M33)
{
    MY_LOG_IF(m_bDebugEnable,"%s()\n", __FUNCTION__);

    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->getColorCorrectionTransform(M11, M12, M13, M21, M22, M23, M31, M32, M33);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->getColorCorrectionTransform(M11, M12, M13, M21, M22, M23, M31, M32, M33);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->getColorCorrectionTransform(M11, M12, M13, M21, M22, M23, M31, M32, M33);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::getColorCorrectionTransform()\n");
        return MFALSE;
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setColorCorrectionTransform(MINT32 const i4SensorDev,
                                                 MFLOAT M11, MFLOAT M12, MFLOAT M13,
                                                 MFLOAT M21, MFLOAT M22, MFLOAT M23,
                                                 MFLOAT M31, MFLOAT M32, MFLOAT M33)
{
    MY_LOG_IF(m_bDebugEnable,"%s()\n", __FUNCTION__);

    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setColorCorrectionTransform(M11, M12, M13, M21, M22, M23, M31, M32, M33);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setColorCorrectionTransform(M11, M12, M13, M21, M22, M23, M31, M32, M33);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setColorCorrectionTransform(M11, M12, M13, M21, M22, M23, M31, M32, M33);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::setColorCorrectionTransform()\n");
        return MFALSE;
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setColorCorrectionMode(MINT32 const i4SensorDev, MINT32 i4ColorCorrectionMode)
{
    MY_LOG_IF(m_bDebugEnable,"%s()\n", __FUNCTION__);

    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setColorCorrectionMode(i4ColorCorrectionMode);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setColorCorrectionMode(i4ColorCorrectionMode);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setColorCorrectionMode(i4ColorCorrectionMode);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::setColorCorrectionMode()\n");
        return MFALSE;
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setEdgeMode(MINT32 const i4SensorDev, MINT32 i4EdgeMode)
{
    MY_LOG_IF(m_bDebugEnable,"%s()\n", __FUNCTION__);

    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setEdgeMode(i4EdgeMode);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setEdgeMode(i4EdgeMode);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setEdgeMode(i4EdgeMode);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::setEdgeMode()\n");
        return MFALSE;
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setNoiseReductionMode(MINT32 const i4SensorDev, MINT32 i4NRMode)
{
    MY_LOG_IF(m_bDebugEnable,"%s()\n", __FUNCTION__);

    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setNoiseReductionMode(i4NRMode);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setNoiseReductionMode(i4NRMode);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setNoiseReductionMode(i4NRMode);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::setNoiseReductionMode()\n");
        return MFALSE;
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setToneMapMode(MINT32 const i4SensorDev, MINT32 i4ToneMapMode)
{
    MY_LOG_IF(m_bDebugEnable,"%s()\n", __FUNCTION__);

    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setToneMapMode(i4ToneMapMode);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setToneMapMode(i4ToneMapMode);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setToneMapMode(i4ToneMapMode);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::setToneMapMode()\n");
        return MFALSE;
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::getTonemapCurve_Red(MINT32 const i4SensorDev, MFLOAT*& p_in_red, MFLOAT*& p_out_red, MINT32 *pCurvePointNum)
{
    MY_LOG_IF(m_bDebugEnable,"%s()\n", __FUNCTION__);

    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->getTonemapCurve_Red(p_in_red, p_out_red, pCurvePointNum);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->getTonemapCurve_Red(p_in_red, p_out_red, pCurvePointNum);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->getTonemapCurve_Red(p_in_red, p_out_red, pCurvePointNum);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::getTonemapCurve_Red()\n");
        return MFALSE;
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::getTonemapCurve_Green(MINT32 const i4SensorDev, MFLOAT*& p_in_green, MFLOAT*& p_out_green, MINT32 *pCurvePointNum)
{
    MY_LOG_IF(m_bDebugEnable,"%s()\n", __FUNCTION__);

    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->getTonemapCurve_Green(p_in_green, p_out_green, pCurvePointNum);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->getTonemapCurve_Green(p_in_green, p_out_green, pCurvePointNum);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->getTonemapCurve_Green(p_in_green, p_out_green, pCurvePointNum);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::getTonemapCurve_Green()\n");
        return MFALSE;
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::getTonemapCurve_Blue(MINT32 const i4SensorDev, MFLOAT*& p_in_blue, MFLOAT*& p_out_blue, MINT32 *pCurvePointNum)
{
    MY_LOG_IF(m_bDebugEnable,"%s()\n", __FUNCTION__);

    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->getTonemapCurve_Blue(p_in_blue, p_out_blue, pCurvePointNum);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->getTonemapCurve_Blue(p_in_blue, p_out_blue, pCurvePointNum);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->getTonemapCurve_Blue(p_in_blue, p_out_blue, pCurvePointNum);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::getTonemapCurve_Blue()\n");
        return MFALSE;
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setTonemapCurve_Red(MINT32 const i4SensorDev, MFLOAT *p_in_red, MFLOAT *p_out_red, MINT32 *pCurvePointNum)
{
    MY_LOG_IF(m_bDebugEnable,"%s()\n", __FUNCTION__);

    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setTonemapCurve_Red(p_in_red, p_out_red, pCurvePointNum);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setTonemapCurve_Red(p_in_red, p_out_red, pCurvePointNum);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setTonemapCurve_Red(p_in_red, p_out_red, pCurvePointNum);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::setTonemapCurve_Red()\n");
        return MFALSE;
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setTonemapCurve_Green(MINT32 const i4SensorDev, MFLOAT *p_in_green, MFLOAT *p_out_green, MINT32 *pCurvePointNum)
{
    MY_LOG_IF(m_bDebugEnable,"%s()\n", __FUNCTION__);

    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setTonemapCurve_Green(p_in_green, p_out_green, pCurvePointNum);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setTonemapCurve_Green(p_in_green, p_out_green, pCurvePointNum);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setTonemapCurve_Green(p_in_green, p_out_green, pCurvePointNum);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::setTonemapCurve_Green()\n");
        return MFALSE;
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::setTonemapCurve_Blue(MINT32 const i4SensorDev, MFLOAT *p_in_blue, MFLOAT *p_out_blue, MINT32 *pCurvePointNum)
{
    MY_LOG_IF(m_bDebugEnable,"%s()\n", __FUNCTION__);

    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            m_pParamctrl_Main->setTonemapCurve_Blue(p_in_blue, p_out_blue, pCurvePointNum);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            m_pParamctrl_Sub->setTonemapCurve_Blue(p_in_blue, p_out_blue, pCurvePointNum);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            m_pParamctrl_Main2->setTonemapCurve_Blue(p_in_blue, p_out_blue, pCurvePointNum);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::setTonemapCurve_Blue()\n");
        return MFALSE;
    }

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL IspTuningMgr::convertPtPairsToGMA(MINT32 const i4SensorDev, const MFLOAT* inPtPairs, MUINT32 u4NumOfPts, MINT32* outGMA)
{
    MY_LOG_IF(m_bDebugEnable,"%s(), i4SensorDev(0x%x)\n", __FUNCTION__, i4SensorDev);
    if (i4SensorDev & ESensorDev_Main) {
        if (m_pParamctrl_Main) {
            return m_pParamctrl_Main->convertPtPairsToGMA(inPtPairs, u4NumOfPts, outGMA);
        }
        else {
            MY_ERR("m_pParamctrl_Main is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_Sub) {
        if (m_pParamctrl_Sub) {
            return m_pParamctrl_Sub->convertPtPairsToGMA(inPtPairs, u4NumOfPts, outGMA);
        }
        else {
            MY_ERR("m_pParamctrl_Sub is NULL");
            return MFALSE;
        }
    }
    else if (i4SensorDev & ESensorDev_MainSecond) {
        if (m_pParamctrl_Main2) {
            return m_pParamctrl_Main2->convertPtPairsToGMA(inPtPairs, u4NumOfPts, outGMA);
        }
        else {
            MY_ERR("m_pParamctrl_Main2 is NULL");
            return MFALSE;
        }
    }
    else {
        MY_ERR("Err IspTuningMgr::convertPtPairsToGMA()\n");
        return MFALSE;
    }
}
