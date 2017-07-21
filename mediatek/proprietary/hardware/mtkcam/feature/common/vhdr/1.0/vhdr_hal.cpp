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
*      TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
#ifdef MTK_CAM_VHDR_SUPPORT

//! \file vhdr_hal.cpp

#include <stdlib.h>
#include <stdio.h>
#include <utils/threads.h>
#include <cutils/log.h>
#include <cutils/properties.h>
#include <sys/prctl.h>
#include <semaphore.h>
#include <pthread.h>

using namespace android;

#include "v1/config/PriorityDefs.h"
#include "IHalSensor.h"

using namespace NSCam;

#include "IHal3A.h"
using namespace NS3Av3;

#include <utils/include/common.h>
#include <utils/include/imagebuf/IIonImageBufferHeap.h>
using namespace NSCam::Utils;

#include "ae_param.h"

#include "vhdr_drv.h"
#include "vhdr_hal_imp.h"
#include "common/vhdr/1.0/vhdr_debug.h"

#include <mtk_platform_metadata_tag.h>
#include <metadata/IMetadata.h>

/*******************************************************************************
*
********************************************************************************/
/* LCSO BUF number*/
#define LCSO_BUFFER_NUM 10

#define VHDR_HAL_DEBUG

#ifdef VHDR_HAL_DEBUG

#undef __func__
#define __func__ __FUNCTION__

#define LOG_TAG "VHDRHal"
#define VHDR_LOG(fmt, arg...)    CAM_LOGD("[%s]" fmt, __func__, ##arg)
#define VHDR_INF(fmt, arg...)    CAM_LOGI("[%s]" fmt, __func__, ##arg)
#define VHDR_WRN(fmt, arg...)    CAM_LOGW("[%s] WRN(%5d):" fmt, __func__, __LINE__, ##arg)
#define VHDR_ERR(fmt, arg...)    CAM_LOGE("[%s] %s ERROR(%5d):" fmt, __func__,__FILE__, __LINE__, ##arg)

#else
#define VHDR_LOG(a,...)
#define VHDR_INF(a,...)
#define VHDR_WRN(a,...)
#define VHDR_ERR(a,...)
#endif

#define MY_LOGD_IF(cond, ...)       do { if ( (cond) >= (2) ) { VHDR_LOG(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) >= (1) ) { VHDR_INF(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) >= (1) ) { VHDR_WRN(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) >= (0) ) { VHDR_ERR(__VA_ARGS__); } }while(0)

#define VHDR_HAL_NAME "VHDRHal"

// ==== LCS Out 64x48 ======
#define LCS_OUT_W 0x40
#define LCS_OUT_H 0x30

// ====  Always LCS + LCE on =====
#define LCS_LCE_ALWAYS_ON 0
#define IVHDR_NOT_READY

static MINT32 g_debugDump = 0;
static MINT32 g_disable = 0;
/**
* Dump LCSO buffer every {g_dumpLCSO} buffers.
* Ex: g_dumpLCSO = 10,  it will dump LCSO buffer every 10 frame.
*/
static MINT32 g_dumpLCSO = 0;

/*******************************************************************************
*  Test
********************************************************************************/
template <typename T>
inline MBOOL
tryGetMetadata(
    IMetadata& pMetadata,
    MUINT32 const tag,
    T & rVal
)
{

    IMetadata::IEntry entry = pMetadata.entryFor(tag);
    if( !entry.isEmpty() ) {
        rVal = entry.itemAt(0, Type2Type<T>());
        return MTRUE;
    }
    return MFALSE;
}



/*******************************************************************************
*
********************************************************************************/
MBOOL
VHdrHalImp::isVhdrOn_IorZ()
{
    return (mVhdrMode == SENSOR_VHDR_MODE_IVHDR || mVhdrMode == SENSOR_VHDR_MODE_ZVHDR);
}


/*******************************************************************************
*
********************************************************************************/
template<typename T>
inline MINT32 UPDATE_ENTRY_SINGLE(IMetadata& metadata, MINT32 entry_tag, T value)
{
    IMetadata::IEntry entry(entry_tag);
    entry.push_back(value, Type2Type< T >());
    return metadata.update(entry_tag, entry);
}

/*******************************************************************************
*  Get AE Mode
********************************************************************************/
eAETargetMODE
getCurrentAEMode(
    MUINT32 vhdrMode
)
{
    switch(vhdrMode)
    {
        case SENSOR_VHDR_MODE_ZVHDR:
            return AE_MODE_ZVHDR_TARGET;
        case SENSOR_VHDR_MODE_IVHDR:
            return AE_MODE_IVHDR_TARGET;
        case SENSOR_VHDR_MODE_MVHDR:
            return AE_MODE_MVHDR_TARGET;
    }
    return AE_MODE_NORMAL;
}

/*******************************************************************************
*
********************************************************************************/
VHdrHal *VHdrHal::CreateInstance(char const *userName, const MUINT32 &aSensorIdx)
{
    VHDR_LOG("%s",userName);
    return VHdrHalImp::GetInstance(aSensorIdx);
}

/*******************************************************************************
*
********************************************************************************/
VHdrHal *VHdrHalImp::GetInstance(const MUINT32 &aSensorIdx)
{
    VHDR_LOG( "aSensorIdx(%u)",aSensorIdx);

    switch(aSensorIdx)
    {
        case 0 : return VHdrHalObj<0>::GetInstance();
        case 1 : return VHdrHalObj<1>::GetInstance();
        case 2 : return VHdrHalObj<2>::GetInstance();
        case 3 : return VHdrHalObj<3>::GetInstance();
        default :
            VHDR_LOG( "current limit is 4 sensors, use 0");
            return VHdrHalObj<0>::GetInstance();
    }
}

/*******************************************************************************
*
********************************************************************************/
MVOID VHdrHalImp::DestroyInstance(char const *userName)
{
    // We don't delete instance because we have LCSO Buffers.
    // If we deallocate buffers when P2 Thread not done yet, it may cause error.
    VHDR_LOG( "%s",userName);
}

/*******************************************************************************
*
********************************************************************************/
VHdrHalImp::VHdrHalImp(const MUINT32 &aSensorIdx) : VHdrHal()
{
    mUsers = 0;

	m_pVHdrDrv = NULL;
    m_pHalSensorList = NULL;
    m_pHalSensor = NULL;
    m_p3aHal = NULL;
    m_pVHdrCb3A = NULL;

    mSensorIdx = aSensorIdx;
    mSensorDev = 0;

    mState = VHDR_STATE_NONE;
    mVhdrMode = 0;

}

/*******************************************************************************
*IVHDR_NOT_READY
********************************************************************************/
VHdrHalImp::~VHdrHalImp()
{
    VHDR_LOG( "VHDR Hal destrcutor");

    mVhdrMode = 0;

    DestroyLcsoBuffers(m_vSliceBufs);
}


/*******************************************************************************
*
********************************************************************************/
MVOID *VHdrHalImp::VHdrThreadLoop(MVOID *arg)
{
#if VHDR_RUN_LEGACY
    VHdrHalImp *_this = reinterpret_cast<VHdrHalImp *>(arg);
    MINT32 err = VHDR_RETURN_NO_ERROR;

    //====== Change Thread Setting ======

    _this->ChangeThreadSetting();

    //====== Main Loop ======

    VHDR_STATE_ENUM eState = _this->GetVHdrState();

    while(eState != VHDR_STATE_UNINIT)
    {
        MY_LOGD_IF(g_debugDump, "wait mVsyncSem");
        ::sem_wait(&_this->mVsyncSem); // wait here until someone use sem_post() to wake this semaphore up
        MY_LOGD_IF(g_debugDump, "got mVsyncSem");

        eState = _this->GetVHdrState();

        switch(eState)
        {
            case VHDR_STATE_ALIVE:
                    err = _this->UpdateVHdr();
                    if(err != VHDR_RETURN_NO_ERROR)
                    {
                        MY_LOGE_IF(g_debugDump, "UpdateVHdr fail(%d)",err);
                    }
                break;
            case VHDR_STATE_UNINIT :
                MY_LOGD_IF(g_debugDump, "UNINIT");
                break;
            default:
                MY_LOGE_IF(g_debugDump, "State Error(%d)",eState);
        }

        eState = _this->GetVHdrState();
    }

    ::sem_post(&_this->mVsyncSemEnd);
    MY_LOGD_IF(g_debugDump, "post mVsyncSemEnd");
#endif
    return NULL;
}


/*******************************************************************************
*
********************************************************************************/
MINT32 VHdrHalImp::Init(const MUINT32 &aMode)
{
    Mutex::Autolock lock(mLock);


    //====== Check Reference Count ======

    if(mUsers > 0)
    {
        android_atomic_inc(&mUsers);
        VHDR_LOG( "mSensorIdx(%u) has %d users",mSensorIdx,mUsers);
        return VHDR_RETURN_NO_ERROR;
    }

    MINT32 err = VHDR_RETURN_NO_ERROR;


    // ======== Debug Property Check =========

    g_debugDump = ::property_get_int32(VHDR_DEBUG_PROP, 0);

    g_disable = ::property_get_int32(VHDR_DISABLE_PROP, 0);

    g_dumpLCSO = ::property_get_int32(VHDR_LCSO_PROP, 0);

    // force open AE Target
    MINT32 forceAE = ::property_get_int32("debug.vhdr.force_ae_mode", 0);

    VHDR_INF( "mSensorIdx(%u),mode(%u) init",mSensorIdx,aMode);
    VHDR_LOG("debug dump = %d, disable = %d, forceAE = %d", g_debugDump, g_disable, forceAE);

    // ===== force to set AE ====
    if(forceAE != 0){
        VHDR_LOG( "force AE Target Mode %d",forceAE);
        m_p3aHal = IHal3A::createInstance(ConvertCameraVer(VHDR_CAMERA_VER_3),static_cast<MINT32>(mSensorIdx),VHDR_HAL_NAME);
        m_p3aHal->send3ACtrl(E3ACtrl_SetAETargetMode, forceAE, NULL);
    }
    //====== Set State ======

    SetVHdrState(VHDR_STATE_ALIVE);

    //====== Save Mode ======

    mVhdrMode = aMode;
#ifdef IVHDR_NOT_READY
    if(mVhdrMode == SENSOR_VHDR_MODE_IVHDR)
    {
        mVhdrMode = 0;
        VHDR_WRN("ivHDR not support, change mode to none.");
    }
#endif

    //====== Create Sensor Object ======

    m_pHalSensorList = IHalSensorList::get();
    if(m_pHalSensorList == NULL)
    {
        MY_LOGE_IF(g_debugDump, "IHalSensorList::get fail");
        goto create_fail_exit;
    }

    if(mVhdrMode == SENSOR_VHDR_MODE_IVHDR
#if LCS_LCE_ALWAYS_ON
        || mVhdrMode == SENSOR_VHDR_MODE_NONE
#endif
        || mVhdrMode == SENSOR_VHDR_MODE_ZVHDR)
    {
        // ====  Init LCSO output size, usually fix in 64x48 =====
        mLcs_outW = LCS_OUT_W;
        mLcs_outH = LCS_OUT_H;
        //====== Create VHDR Driver ======

        m_pVHdrDrv = VHdrDrv::CreateInstance(mSensorIdx);

        if(m_pVHdrDrv == NULL)
        {
            MY_LOGE_IF(g_debugDump, "VHdrDrv::createInstance fail");
            goto create_fail_exit;
        }
#if 0
         //====== Get Sensor Info first, because allocate memory need it ======

        err = GetSensorInfo();  // Jou-Feng change move it to Init()
        if(err != VHDR_RETURN_NO_ERROR)
        {
            MY_LOGE_IF(g_debugDump, "GetSensorInfo fail(%d)", err);
            return VHDR_RETURN_API_FAIL;
        }
//#if VHDR_HW_DRV_READY
        m_pVHdrDrv->SetSensorInfo(mSensorDev,mSensorDynamicInfo.TgInfo);
//#else
        //m_pVHdrDrv->SetSensorInfo(mSensorDev,1);  // Fake sensor test....
//#endif
#endif




        // ===== Init will do allocate memory ==== (Jou-Feng change)
        err = m_pVHdrDrv->Init(mLcs_outW, mLcs_outH);
        if(err != VHDR_RETURN_NO_ERROR)
        {
            MY_LOGE_IF(g_debugDump, "VHdrDrv::Init fail");
            goto create_fail_exit;
        }

        // =====  Create LCSO Buffers if not created yet ===========
        if(m_vSliceBufs.size() == 0)
            CreateLcsoBuffers(mLcs_outW, mLcs_outH, LCSO_BUFFER_NUM, m_vSliceBufs);

        //====== Create 3A Callback ======

		m_pVHdrCb3A = new VHdrHalCallBack(this);
#if VHDR_RUN_LEGACY
        //====== Create Thread  ======

        //> init semphore

        ::sem_init(&mVsyncSem, 0, 0);
        ::sem_init(&mVsyncSemEnd, 0, 0);

        //> create thread

        pthread_create(&mVsyncThread, NULL, VHdrThreadLoop, this);
#endif

        android_atomic_inc(&mUsers);
        MY_LOGD_IF(g_debugDump, "-");
        return VHDR_RETURN_NO_ERROR;
    }
    else if(mVhdrMode == SENSOR_VHDR_MODE_MVHDR)
    {
        android_atomic_inc(&mUsers);
        MY_LOGD_IF(g_debugDump, "-");
        return VHDR_RETURN_NO_ERROR;
    }
    else
    {
        MY_LOGW_IF(g_debugDump, "unknown VHDR mode(%u)",mVhdrMode);
    }

create_fail_exit:

    if(m_pVHdrDrv != NULL)
    {
        m_pVHdrDrv->Uninit();
        m_pVHdrDrv->DestroyInstance();
        m_pVHdrDrv = NULL;
    }

    if(m_pHalSensorList != NULL)
    {
        m_pHalSensorList = NULL;
    }

    MY_LOGD_IF(g_debugDump, "-");
    return VHDR_RETURN_INVALID_DRIVER;
}

/*******************************************************************************
*
********************************************************************************/
MINT32 VHdrHalImp::Uninit()
{
    Mutex::Autolock lock(mLock);

    //====== Check Reference Count ======

    if(mUsers <= 0)
    {
        VHDR_LOG( "mSensorIdx(%u) has 0 user",mSensorIdx);
        return VHDR_RETURN_NO_ERROR;
    }

    //====== Uninitialize ======

    android_atomic_dec(&mUsers);    //decrease referebce count

    if(mUsers == 0)
    {
       VHDR_LOG("mSensorIdx(%u),mode(%u) uninit",mSensorIdx,mVhdrMode);

        MINT32 err = VHDR_RETURN_NO_ERROR;

        //====== Set State ======

        SetVHdrState(VHDR_STATE_UNINIT);

        //====== Destroy Sensor Object ======

        if(m_pHalSensorList != NULL)
        {
            m_pHalSensorList = NULL;
        }

        // =====  cancel AE Mode =======
        if(m_p3aHal != NULL)
        {
            VHDR_LOG( "set AE to normal");
            m_p3aHal->send3ACtrl(E3ACtrl_SetAETargetMode, AE_MODE_NORMAL, NULL);
        }

        if(mVhdrMode == SENSOR_VHDR_MODE_IVHDR
#if LCS_LCE_ALWAYS_ON
            || mVhdrMode == SENSOR_VHDR_MODE_NONE
#endif
            || mVhdrMode == SENSOR_VHDR_MODE_ZVHDR)
        {
            //====== Delete 3A  ======

            if(m_p3aHal != NULL)
            {

                m_p3aHal->detachCb(IHal3ACb::eID_NOTIFY_AE_RT_PARAMS, m_pVHdrCb3A);

                m_p3aHal->destroyInstance(VHDR_HAL_NAME);
                m_p3aHal = NULL;

                if(m_pVHdrCb3A != NULL)
                {
                    delete m_pVHdrCb3A;
                    m_pVHdrCb3A = NULL;
                }
            }

            //====== Destroy VHDR Driver ======

            if(m_pVHdrDrv != NULL)
            {
                //> disable VHDR

                //m_pVHdrDrv->EnableLcs(MFALSE);    //ISP driver will reset
                if(isVhdrOn_IorZ())
                {
                    m_pVHdrDrv->EnableLce(MFALSE);
                    m_pVHdrDrv->EnableRmg(MFALSE);
                    m_pVHdrDrv->EnableRmm(MFALSE);
                }

                m_pVHdrDrv->Uninit();
                m_pVHdrDrv->DestroyInstance();
                m_pVHdrDrv = NULL;

            }

            //====== Destroy Thread ======
#if VHDR_RUN_LEGACY
            MY_LOGD_IF(g_debugDump, "post mVsyncSem");
            ::sem_post(&mVsyncSem);

            //wait sem
            MY_LOGD_IF(g_debugDump, "wait mVsyncSemEnd");
            ::sem_wait(&mVsyncSemEnd);
            MY_LOGD_IF(g_debugDump, "got mVsyncSemEnd");

            pthread_join(mVsyncThread, NULL);
#endif
        }
        else if(mVhdrMode == SENSOR_VHDR_MODE_MVHDR)
        {
            //====== Delete 3A  ======

            if(m_p3aHal != NULL)
            {

                //m_p3aHal->SetAETargetMode(AE_MODE_NORMAL); // NO Need because we set normal before
                m_p3aHal->destroyInstance(VHDR_HAL_NAME);
                m_p3aHal = NULL;

            }
        }
        else
        {
            MY_LOGW_IF(g_debugDump, "unknwon VHDR mode(%u)",mVhdrMode);
        }

        //====== Set State ======

        mVhdrMode = 0;
        SetVHdrState(VHDR_STATE_NONE);
    }
    else
    {
        VHDR_LOG( "mSensorIdx(%u) has %d users",mSensorIdx,mUsers);
    }

    MY_LOGD_IF(g_debugDump, "-");
    return VHDR_RETURN_NO_ERROR;
}

/*******************************************************************************
*
********************************************************************************/
MINT32 VHdrHalImp::GetSensorInfo()
{
    MY_LOGD_IF(g_debugDump, "mSensorIdx(%u)",mSensorIdx);

    mSensorDev = m_pHalSensorList->querySensorDevIdx(mSensorIdx);
    m_pHalSensorList->querySensorStaticInfo(mSensorDev,&mSensorStaticInfo);

    m_pHalSensor = m_pHalSensorList->createSensor(VHDR_HAL_NAME,1,&mSensorIdx);
    if(m_pHalSensor == NULL)
    {
        MY_LOGE_IF(g_debugDump, "m_pHalSensorList->createSensor fail");
        return VHDR_RETURN_API_FAIL;
    }

    if(MFALSE == m_pHalSensor->querySensorDynamicInfo(mSensorDev,&mSensorDynamicInfo))
    {
        MY_LOGE_IF(g_debugDump, "querySensorDynamicInfo fail");
        return VHDR_RETURN_API_FAIL;
    }

    VHDR_INF( "Sensor Static Info, previewW,H = %d,%d,",mSensorStaticInfo.previewWidth , mSensorStaticInfo.previewHeight);

    m_pHalSensor->destroyInstance(VHDR_HAL_NAME);
    m_pHalSensor = NULL;

    return VHDR_RETURN_NO_ERROR;
}


/*******************************************************************************
*
********************************************************************************/
MINT32 VHdrHalImp::ConfigVHdr(const VHDR_HAL_CONFIG_DATA &aConfigData)
{
    MY_LOGI_IF(g_debugDump, "mode(%u)",mVhdrMode);

    MINT32 err = VHDR_RETURN_NO_ERROR;

    //====== Get Sensor Info ======

    err = GetSensorInfo();
    if(err != VHDR_RETURN_NO_ERROR)
    {
        MY_LOGE_IF(g_debugDump, "GetSensorInfo fail(%d)", err);
        return VHDR_RETURN_API_FAIL;
    }

    //====== Create 3A Object ======

    m_p3aHal = IHal3A::createInstance(ConvertCameraVer(aConfigData.cameraVer),static_cast<MINT32>(mSensorIdx),VHDR_HAL_NAME);
    if(m_p3aHal == NULL)
    {
        MY_LOGE_IF(g_debugDump, "m_p3aHal create fail");
        return VHDR_RETURN_NULL_OBJ;
    }

    //====== Set AE Target Mode ======

    if( ! (g_disable & VHDR_DISABLE_AE_TARGET) ){
        VHDR_INF( "setAETartgetMode %d", getCurrentAEMode(mVhdrMode));
        m_p3aHal->send3ACtrl(E3ACtrl_SetAETargetMode, getCurrentAEMode(mVhdrMode), NULL);
    }else{
        VHDR_LOG( "disable setAETargetMode");
    }


    if(mVhdrMode == SENSOR_VHDR_MODE_IVHDR
#if LCS_LCE_ALWAYS_ON
        || mVhdrMode == SENSOR_VHDR_MODE_NONE
#endif
        || mVhdrMode == SENSOR_VHDR_MODE_ZVHDR)
    {
        //> set to VHDR driver

        m_pVHdrDrv->SetSensorInfo(mSensorDev,mSensorDynamicInfo.TgInfo); // Jou-Feng change move it to Init()

        //====== Add 3A Callback ======

        m_p3aHal->attachCb(IHal3ACb::eID_NOTIFY_AE_RT_PARAMS ,m_pVHdrCb3A);


        //====== Config LCS : Now we don't have this api, so comment it (Jou-Feng) ======

        //FrameOutputParam_T flareInfo;
        //m_p3aHal->getRTParams(flareInfo);

        // set flare info befoer configure LCS
        //m_pVHdrDrv->SetFlareInfo(flareInfo.i2FlareGain,flareInfo.i2FlareOffset);

        err = m_pVHdrDrv->ConfigLcs(); // also register P1 Tuning Notify callback for LCS setting
        if(err != VHDR_RETURN_NO_ERROR)
        {
            MY_LOGE_IF(g_debugDump, "ConfigLcs fail(%d)", err);
            return VHDR_RETURN_API_FAIL;
        }

        //====== Config LCSO ======  Now no need
#if 0
        err = m_pVHdrDrv->ConfigLcso();
        if(err != VHDR_RETURN_NO_ERROR)
        {
            MY_LOGE_IF(g_debugDump, "ConfigLcso fail(%d)", err);
            return VHDR_RETURN_API_FAIL;
        }
#endif

        //====== Config RMG  ======

        if(mVhdrMode == SENSOR_VHDR_MODE_IVHDR)
        {
            MUINT32 isLeFirst = static_cast<MUINT32>(mSensorStaticInfo.iHDR_First_IS_LE);
            MY_LOGI_IF(g_debugDump, "isLeFirst(%u)",isLeFirst);    // 0 : first two rows are SE,  1 : first two rows are LE

            m_pVHdrDrv->ConfigRmg_ivHDR(isLeFirst);
        }
        else if(mVhdrMode == SENSOR_VHDR_MODE_ZVHDR)
        {
            MUINT32 zPattern = static_cast<MUINT32>(mSensorStaticInfo.ZHDR_MODE);
            MY_LOGI_IF(g_debugDump, "Z-Pattern(%u)",zPattern);    /*0: zhdr no support , 1~ 8 suppport pattern*/

            m_pVHdrDrv->ConfigRmg_zvHDR(zPattern);
        }

        //====== Enable VHDR related module ======

        if(isVhdrOn_IorZ())
        {
            if( ! (g_disable & VHDR_DISABLE_LCS_LCE) ){
                m_pVHdrDrv->EnableLce(MTRUE);
            }else{
                MY_LOGI_IF(g_debugDump, "Disable LCE");

                m_pVHdrDrv->EnableLce(MFALSE);
            }

            if( ! (g_disable & VHDR_DISABLE_RMG) ){
                m_pVHdrDrv->EnableRmg(MTRUE);
            }else{
                MY_LOGI_IF(g_debugDump, "Disable RMG");
                m_pVHdrDrv->EnableRmg(MFALSE);
            }

            if( ! (g_disable & VHDR_DISABLE_RMM) ){
                m_pVHdrDrv->EnableRmm(MTRUE);
            }else{
                MY_LOGI_IF(g_debugDump, "Disable RMM");
                m_pVHdrDrv->EnableRmm(MFALSE);
            }
        }

        //====== Set Configure Done ======

        m_pVHdrDrv->SetLcsoThreadState(VHDR_STATE_ALIVE);
    }
    else if(mVhdrMode == SENSOR_VHDR_MODE_MVHDR)
    {
    // now no need to do anything, because AE mode already set.
#if 0
        //====== Create 3A Object ======

        m_p3aHal = IHal3A::createInstance(ConvertCameraVer(aConfigData.cameraVer),static_cast<MINT32>(mSensorIdx),VHDR_HAL_NAME);
        if(m_p3aHal == NULL)
        {
            MY_LOGE_IF(g_debugDump, "m_p3aHal create fail");
            return VHDR_RETURN_NULL_OBJ;
        }

        //====== Set AE Target Mode ======
#if VHDR_HW_DRV_READY
        m_p3aHal->SetAETargetMode(AE_MODE_MVHDR_TARGET);
#endif
#endif
    }
    else
    {
        MY_LOGW_IF(g_debugDump, "wrong mode(%u)",mVhdrMode);
    }

    MY_LOGD_IF(g_debugDump, "-");
    return VHDR_RETURN_NO_ERROR;
}

/*******************************************************************************
*
********************************************************************************/
MVOID VHdrHalImp::DoVHdr(const MINT64 &aTimeStamp)
{
    if(mVhdrMode == SENSOR_VHDR_MODE_IVHDR || mVhdrMode == SENSOR_VHDR_MODE_ZVHDR)
    {
        m_pVHdrDrv->UpdateLcsoTimeStamp(aTimeStamp);
    }
}

/*******************************************************************************
*
********************************************************************************/
MINT32 VHdrHalImp::GetBufLCSO(android::sp<IImageBuffer>& spBuf)
{
    if(mVhdrMode == SENSOR_VHDR_MODE_IVHDR
#if LCS_LCE_ALWAYS_ON
        || mVhdrMode == SENSOR_VHDR_MODE_NONE
#endif
        || mVhdrMode == SENSOR_VHDR_MODE_ZVHDR)
    {
        if( ! (g_disable & VHDR_DISABLE_LCS_LCE) )
        {

            static MUINT32 currentSliceIndex = 0;

            MY_LOGD_IF(g_debugDump, "GetBufLCSO, currentIndex =  %d, buf slice list size = %d", currentSliceIndex, m_vSliceBufs.size());
            if(currentSliceIndex < m_vSliceBufs.size())
            {
                spBuf = m_vSliceBufs[currentSliceIndex];
                currentSliceIndex++;
                if(currentSliceIndex >= m_vSliceBufs.size())
                    currentSliceIndex = 0;

                //spBuf->syncCache(eCACHECTRL_INVALID);
                //MUINT8* pt = (MUINT8*)spBuf->getBufVA(0);
                //MY_LOGD_IF(g_debugDump, "orig buf VA(0x%08x), qqqq[50]=%d", spBuf->getBufVA(0), *(pt + 50));
                MY_LOGD_IF(g_debugDump, "GetBufLCSO, successfully. Next get index = %d", currentSliceIndex);
                return VHDR_RETURN_NO_ERROR;
            }
            else
            {
                MY_LOGE_IF(g_debugDump, "GetBufLCSO failed. currentIndex = %d", currentSliceIndex);
                return VHDR_RETURN_UNKNOWN_ERROR;
            }
            //return m_pVHdrDrv->GetBufLCSO(spBuf);
        }
    }
    return 0;
}

/*******************************************************************************
*
********************************************************************************/
#if 0
MVOID VHdrHalImp::DoVHdr(LCEI_DMA_INFO *apLceiDmaInfo,const android::sp<IImageBuffer>& lcsoBuf)
{
    if(mVhdrMode == SENSOR_VHDR_MODE_IVHDR || mVhdrMode == SENSOR_VHDR_MODE_ZVHDR)
    {
        if(m_pVHdrDrv != NULL)
        {
            return m_pVHdrDrv->DoVHdr(apLceiDmaInfo, lcsoBuf);
        }
    }
}
#endif
MVOID VHdrHalImp::PushMetadata(IMetadata& halMetaData,const IImageBuffer* lcsoBuf)
{
    if(mVhdrMode == SENSOR_VHDR_MODE_IVHDR
#if LCS_LCE_ALWAYS_ON
        || mVhdrMode == SENSOR_VHDR_MODE_NONE
#endif
        || mVhdrMode == SENSOR_VHDR_MODE_ZVHDR)
    {
        if( (g_disable & VHDR_DISABLE_LCS_LCE) )
            return;

        LCEI_CONFIG_DATA aLceiDma;

        aLceiDma.lcsoBuf = const_cast<IImageBuffer*>(lcsoBuf);

        //MUINT8* pt = (MUINT8*)aLceiDma.lcsoBuf->getBufVA(0);
        //aLceiDma.lcsoBuf->syncCache(eCACHECTRL_INVALID);
        VHDR_LOG( "push LCEI to metadata,bufAddr = %p %p, VA(0x%08x)  PA(0x%08x)", aLceiDma.lcsoBuf, lcsoBuf, aLceiDma.lcsoBuf->getBufVA(0), aLceiDma.lcsoBuf->getBufPA(0));

        // ==== New Dump LCSO mthod ===
        if(g_dumpLCSO != 0)
        {
            static MINT32 incomingCount = 0;
            incomingCount++;
            if( incomingCount % g_dumpLCSO == 0)
            {
                // start dump
                char outputPath[64];
                sprintf(outputPath, "/sdcard/lcso_pack.raw", incomingCount);
                MY_LOGD_IF(g_debugDump, "dump lcso %s, num(%d)",outputPath, incomingCount);
                NSCam::Utils::saveBufToFile(outputPath, (unsigned char*)aLceiDma.lcsoBuf->getBufVA(0), aLceiDma.lcsoBuf->getBufSizeInBytes(0));
                incomingCount = 0;
            }
        }
#if 0
    // old dump method
        if(g_debugDump >= 4) // Check need lcso dump or not
        {
            static MINT32 incomingCount = 0;
            static MINT32 needDumpNum = 0;
            static MINT32 lastPropNum = 0;

            char name[PROPERTY_VALUE_MAX];
            ::property_get("debug.lcso.dump", name, "0");
            MINT32 dumpNum = ::atoi(name);

            if(dumpNum != 0)
            {
                if(dumpNum != lastPropNum){
                    lastPropNum = dumpNum;
                    needDumpNum = dumpNum;
                }

                //::property_set("debug.lcso.dump", "0"); // System has no permission to set debug.xxx prop
            }else{
                lastPropNum = 0;
            }

            if(needDumpNum > 0)
            {
                char outputPath[64];
                sprintf(outputPath, "/sdcard/lcso_pack.raw", incomingCount);
                MY_LOGD_IF(g_debugDump, "dump lcso %s",outputPath);
                NSCam::Utils::saveBufToFile(outputPath, (unsigned char*)aLceiDma.lcsoBuf->getBufVA(0), aLceiDma.lcsoBuf->getBufSizeInBytes(0));
                //aLceiDma.lcsoBuf->saveToFile(outputPath);
                needDumpNum--;
                incomingCount++;
            }
        }
#endif
        //MY_LOGD_IF(g_debugDump, "new qqqq[50]=%d", *(pt + 50));


        // push LCEI info to IMetadata by IMetadata::Memory
        IMetadata::Memory lcei_mem;
        lcei_mem.resize(sizeof(LCEI_CONFIG_DATA));
        ::memcpy(lcei_mem.editArray(), (void*)&aLceiDma, lcei_mem.size());

        ::UPDATE_ENTRY_SINGLE(halMetaData, MTK_VHDR_LCEI_DATA, lcei_mem);

    }
}



/*******************************************************************************
*
********************************************************************************/
MVOID VHdrHalImp::GetLceiDmaInfo(LCEI_DMA_INFO *apLceiDmaInfo,const MINT64 &aTimeStamp)
{
    if(mVhdrMode == SENSOR_VHDR_MODE_IVHDR || mVhdrMode == SENSOR_VHDR_MODE_ZVHDR)
    {
        if(mSensorStaticInfo.iHDRSupport != MTRUE)
        {
            MY_LOGE_IF(g_debugDump, "not support iVHDR(%d)",mSensorStaticInfo.iHDRSupport);
            apLceiDmaInfo->memID= VHDR_LCSO_SYNC_FAIL;
        }
        else
        {
            m_pVHdrDrv->GetLceiDmaInfo(apLceiDmaInfo,aTimeStamp);
        }
    }
}

/*******************************************************************************
*
********************************************************************************/
MINT32 VHdrHalImp::UpdateVHdr()
{
    if(g_debugDump > 0)
    {
        MY_LOGD_IF(g_debugDump, "+");
    }

    MINT32 err = VHDR_RETURN_NO_ERROR;

    //====== Updade Flare for LCS ======

    if(m_p3aHal != NULL)
    {
#if VHDR_HW_DRV_READY
        FrameOutputParam_T flareInfo;
        m_p3aHal->getRTParams(flareInfo);
        m_pVHdrDrv->SetFlareInfo(flareInfo.i2FlareGain,flareInfo.i2FlareOffset);
#endif
    }
    else
    {
        MY_LOGE_IF(g_debugDump, "m_p3aHal is NULL");
    }

    //====== Update LCS ======

#if 0   //no need. use callback to set
    err = m_pVHdrDrv->UpdateLcs();
    if(err != VHDR_RETURN_NO_ERROR)
    {
        MY_LOGE_IF(g_debugDump, "UpdateLcs fail(%d)", err);
        return VHDR_RETURN_API_FAIL;
    }
#endif

    if(g_debugDump > 0)
    {
        MY_LOGD_IF(g_debugDump, "-");
    }
    return err;
}

/*******************************************************************************
*
********************************************************************************/
MVOID VHdrHalImp::SendCommand(VHDR_CMD_ENUM aCmd,MINT32 arg1, MINT32 arg2, MINT32 arg3)
{
    if(mVhdrMode == SENSOR_VHDR_MODE_IVHDR || mVhdrMode == SENSOR_VHDR_MODE_ZVHDR)
    {
        switch(aCmd)
        {
            case VHDR_CMD_SET_STATE:
                m_pVHdrDrv->SetLcsoThreadState((VHDR_STATE_ENUM)arg1);
                break;
            case VHDR_CMD_SET_VR_EIS_ON_OFF:
                m_pVHdrDrv->SetVideoSizeInfo(aCmd,arg1);
                break;
            case VHDR_CMD_SET_VIDEO_SIZE:
                m_pVHdrDrv->SetVideoSizeInfo(aCmd,arg1,arg2);
                break;
            case VHDR_CMD_SET_ZOOM_RATIO:
                m_pVHdrDrv->SetVideoSizeInfo(aCmd,arg1);
                break;
            case VHDR_CMD_SET_PASS1_OUT_SIZE:
                m_pVHdrDrv->SetVideoSizeInfo(aCmd,arg1,arg2);
                break;
            default:
                MY_LOGE_IF(g_debugDump, "wrong cmd(%d)",aCmd);
        }
    }
}

/*******************************************************************************
*
********************************************************************************/
MVOID VHdrHalImp::SetVHdrState(const VHDR_STATE_ENUM &aState)
{
    mState = aState;
    MY_LOGD_IF(g_debugDump, "aState(%d),mState(%d)",aState,mState);
}

/*******************************************************************************
*
********************************************************************************/
VHDR_STATE_ENUM VHdrHalImp::GetVHdrState()
{
    return mState;
}

/*******************************************************************************
*
********************************************************************************/
MVOID VHdrHalImp::ChangeThreadSetting()
{
    //> set name
#if VHDR_RUN_LEGACY
    ::prctl(PR_SET_NAME,"VHdrThread", 0, 0, 0);

    //> set policy/priority
    {
        const MINT32 expect_policy   = SCHED_OTHER;
        const MINT32 expect_priority = NICE_CAMERA_3A_MAIN;
        MINT32 policy = 0, priority = 0;
        setThreadPriority(expect_policy, expect_priority);
        getThreadPriority(policy, priority);

        MY_LOGD_IF(g_debugDump, "policy:(expect,result)=(%d,%d),priority:(expect,result)=(%d, %d)",expect_policy,policy,expect_priority,priority);
    }
#endif
}

/*******************************************************************************
*
********************************************************************************/
IHal3A::E_VER VHdrHalImp::ConvertCameraVer(const VHDR_CAMERA_VER_ENUM &aVer)
{
    switch(aVer)
    {
        case VHDR_CAMERA_VER_1 : return IHal3A::E_Camera_1;
        case VHDR_CAMERA_VER_3 : return IHal3A::E_Camera_3;
        default : MY_LOGW_IF(g_debugDump, "unknown ver(%d) return E_Camera_1",aVer);
                  return IHal3A::E_Camera_1;
    }
}

/*******************************************************************************
*
********************************************************************************/
MVOID VHdrHalImp::VHdrSync3A(MINTPTR pFlareInfo)
{
#if VHDR_RUN_LEGACY
    ::sem_post(&mVsyncSem);

#else
        // no need other thread
        VHDR_STATE_ENUM eState = GetVHdrState();
        MINT32 err = NO_ERROR;
        FrameOutputParam_T* flareInfo;
        switch(eState)
        {
            case VHDR_STATE_ALIVE:
                    flareInfo = reinterpret_cast<FrameOutputParam_T*>(pFlareInfo);
                    if(m_pVHdrDrv != NULL)
                        m_pVHdrDrv->SetFlareInfo(flareInfo->i2FlareGain,flareInfo->i2FlareOffset);
#if 0
        m_p3aHal->getRTParams(flareInfo);
        m_pVHdrDrv->SetFlareInfo(flareInfo.i2FlareGain,flareInfo.i2FlareOffset);
                    err = UpdateVHdr();
                    if(err != VHDR_RETURN_NO_ERROR)
                    {
                        MY_LOGE_IF(g_debugDump, "UpdateVHdr fail(%d)",err);
                    }
#endif
                break;
            case VHDR_STATE_UNINIT :
                MY_LOGD_IF(g_debugDump, "UNINIT");
                break;
            default:
                MY_LOGW_IF(g_debugDump, "State Error(%d)",eState);
        }

#endif
    MY_LOGD_IF(g_debugDump, "post mVsyncSem");
}

/*******************************************************************************
*
********************************************************************************/
MINT32 VHdrHalImp::CreateLcsoBuffers(MUINT32 w, MUINT32 h, MUINT32 num, VecPImgBuf& vecSliceBufs)
{
    MINT32 err = VHDR_RETURN_NO_ERROR;


    MUINT32 bufStridesInBytes[3] = {w, 0, 0};
    MINT32 bufBoundaryInBytes[3] = {0, 0, 0};

    IImageBufferAllocator::ImgParam imgParam(
                eImgFmt_BAYER8,
                MSize(w,h),
                bufStridesInBytes,
                bufBoundaryInBytes,
                1
                );

    MUINT32 ind = 0;
    for ( ind = 0; ind < num ; ind++){

        sp<IImageBuffer> pBuf = (IImageBufferAllocator::getInstance())->alloc_ion("VHDR_HAL", imgParam);
        if ( pBuf == NULL || !pBuf->lockBuf( LOG_TAG, eBUFFER_USAGE_HW_CAMERA_READWRITE | eBUFFER_USAGE_SW_READ_OFTEN ) )
        {
            MY_LOGE_IF(g_debugDump, "Allocate vhdr slice buffer failed!!");
            return VHDR_RETURN_MEMORY_ERROR;
        }
        //pBuf->syncCache(eCACHECTRL_INVALID);  // not cacheable?? , need confirm...
        vecSliceBufs.push_back(pBuf);
        MY_LOGD_IF(g_debugDump, "Slice Buf w=%d, h=%d, plane=%d, stride=%d, VA(0x%08x), PA(0x%08x)", pBuf->getImgSize().w, pBuf->getImgSize().h, pBuf->getPlaneCount(), pBuf->getBufStridesInBytes(0), pBuf->getBufVA(0), pBuf->getBufPA(0));
    }
    MY_LOGD_IF(g_debugDump, " CreateLcsoBuffers end");
    return err;
}

 /******************************************************************************
*
*******************************************************************************/
MINT32 VHdrHalImp::DestroyLcsoBuffers(VecPImgBuf& vecSliceBufs)
{

    MINT32 err = VHDR_RETURN_NO_ERROR;
    for(int i = 0; i < vecSliceBufs.size() ; i++)
    {
        sp<IImageBuffer> pbuf = vecSliceBufs[i];
        pbuf->unlockBuf("VHDR_HAL");
        (IImageBufferAllocator::getInstance())->free(pbuf.get());
    }
    vecSliceBufs.clear();
    return err;
}

/*******************************************************************************
*
********************************************************************************/
VHdrHalCallBack::VHdrHalCallBack(MVOID *arg)
{
    m_pVHdrHalImp = arg;
}

/*******************************************************************************
*
********************************************************************************/
VHdrHalCallBack::~VHdrHalCallBack()
{
    m_pVHdrHalImp = NULL;
}

/*******************************************************************************
*
********************************************************************************/
//void VHdrHalCallBack::doNotifyCb(int32_t _msgType,int32_t _ext1,int32_t _ext2,int32_t _ext3, MINTPTR _ext4)
void VHdrHalCallBack::doNotifyCb(MINT32 _msgType, MINTPTR _ext1,MINTPTR _ext2,MINTPTR _ext3)
{
    MY_LOGD_IF(g_debugDump, "msgType(%d)",_msgType);
    VHdrHalImp *_this = reinterpret_cast<VHdrHalImp *>(m_pVHdrHalImp);

    if(_msgType == IHal3ACb::eID_NOTIFY_AE_RT_PARAMS)
    {
        _this->VHdrSync3A(_ext2);
    }

    MY_LOGD_IF(g_debugDump, "-");
}
#if 0
/*******************************************************************************
*
********************************************************************************/
void VHdrHalCallBack::doDataCb(int32_t _msgType,void *_data,uint32_t _size)
{
}
#endif

#endif
