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
#define LOG_TAG "Hal3Av3Meta"

//#include <mtkcam/hal/IHalSensor.h>
#include <Log.h>
#include <common/include/common.h>
#include <utils/include/common.h>
#include <utils/Atomic.h>
#include <cutils/properties.h>
//#include <camera_feature.h>
#include <faces.h>
#include "Hal3AAdapter3.h"

#include <client/mtk_metadata_tag.h>
#include <mtk_platform_metadata_tag.h>
#include <dngInfo.h>
#include <metadata/IMetadataProvider.h>
#include <Trace.h>

#include <hwutils/HwTransform.h>
#include <hwutils/HwInfoHelper.h>

#include <debug_exif/dbg_id_param.h>



using namespace android;
using namespace NS3Av3;
using namespace NSCam;
using namespace NSCamHW;


#define GET_PROP(prop, dft, val)\
{\
   char value[PROPERTY_VALUE_MAX] = {'\0'};\
   property_get(prop, value, (dft));\
   (val) = atoi(value);\
}

#define MAX(a,b) ((a) >= (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define HAL3A_TEST_OVERRIDE (1)

#define HAL3AADAPTER3_LOG_SET_0 (1<<0)
#define HAL3AADAPTER3_LOG_SET_1 (1<<1)
#define HAL3AADAPTER3_LOG_SET_2 (1<<2)
#define HAL3AADAPTER3_LOG_GET_0 (1<<3)
#define HAL3AADAPTER3_LOG_GET_1 (1<<4)
#define HAL3AADAPTER3_LOG_GET_2 (1<<5)
#define HAL3AADAPTER3_LOG_GET_3 (1<<6)
#define HAL3AADAPTER3_LOG_GET_4 (1<<7)
#define HAL3AADAPTER3_LOG_PF    (1<<8)
#define HAL3AADAPTER3_LOG_USERS (1<<9)

#define HAL3A_REQ_PROC_KEY (2)
#define HAL3A_REQ_CAPACITY (HAL3A_REQ_PROC_KEY + 2)


MBOOL
Hal3AAdapter3::
doInit()
{

    mu1CapIntent = -1; //MTK_CONTROL_CAPTURE_INTENT_PREVIEW;
    mu4Counter = 0;
    mu1Start = 0;
    mu1FdEnable = 0;
    mCropRegion.p.x = 0;
    mCropRegion.p.y = 0;
    mCropRegion.s.w = 0;
    mCropRegion.s.h = 0;
    m_rStaticInfo.u1RollingShutterSkew = 0;

    if (m_i4SubsampleCount > 1) //Slow motion video recording
    {
        mu4MetaResultQueueCapacity = m_i4SubsampleCount * 6;
        MY_LOG("[%s] create E_FlowControl_Type(%d)", __FUNCTION__, Hal3AIf::E_FlowControl_Type_SMVR);
        mpHal3aObj = Hal3AIf::createInstance(mi4SensorIdx, Hal3AIf::E_FlowControl_Type_SMVR, m_i4SubsampleCount);
    }
    else
    {
        mu4MetaResultQueueCapacity = 6;
        mpHal3aObj = Hal3AIf::createInstance(mi4SensorIdx, Hal3AIf::E_FlowControl_Type_NORMAL);
    }
    mu4SensorDev = mpHal3aObj->getSensorDev();
    MY_LOG("[%s] sensorDev(%d), sensorIdx(%d), mpHal3aObj(%p)",
        __FUNCTION__, mu4SensorDev, mi4SensorIdx, mpHal3aObj);

    //sp<IMetadataProvider> pMetadataProvider = IMetadataProvider::create(mi4SensorIdx);
    sp<IMetadataProvider> pMetadataProvider = NSMetadataProviderManager::valueFor(mi4SensorIdx);
    if (pMetadataProvider != NULL)
    {
        mMetaStaticInfo = pMetadataProvider->geMtktStaticCharacteristics();
        //setupStaticInfo();
        // DEBUG
        // _printStaticMetadata(mMetaStaticInfo);
    }
    else
    {
        MY_ERR("[%s] Fail to get static metadata: mi4SensorIdx(0x%04x)",
            __FUNCTION__, mi4SensorIdx);
    }

#if 1
    //mMetaResultQueue.clear();
    mParams = Param_T();
    mAfParams = AF_Param_T();
    setupStaticInfo();
    mpHal3aObj->setParams(mParams);
    mpHal3aObj->setAfParams(mAfParams);
#endif
    mpHal3aObj->attachCb(this);

    return MTRUE;
}

MBOOL
Hal3AAdapter3::
doUninit()
{
    m_ScnModeOvrd.clear();
    mpHal3aObj->detachCb(this);
    mpHal3aObj->destroyInstance();
    mpHal3aObj = NULL;
    mMetaResultQueue.clear();
    m_i4SubsampleCount = 1;
    MY_LOG("[%s] sensorDev(%d), sensorIdx(%d)", __FUNCTION__, mu4SensorDev, mi4SensorIdx);
    return MTRUE;
}
