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

/**
* @file eis_hal.cpp
*
* EIS Hal Source File
*
*/

#include <stdlib.h>
#include <stdio.h>
#include <utils/threads.h>
#include <cutils/log.h>
#include <cutils/log.h>
#include <cutils/properties.h>
#if (MTKCAM_BASIC_PACKAGE != 1)
#include <linux/cache.h>
#endif
#include <sys/prctl.h>
#include <semaphore.h>
#include <pthread.h>
#include <queue>
#define MTK_LOG_ENABLE 1

#include "eis_drv.h"


#include <android/sensor.h>
#include <utils/include/SensorListener.h>
#include <utils/include/imagebuf/IIonImageBufferHeap.h>
#include <utils/include/imagebuf/IDummyImageBufferHeap.h>


#include <common/include/IHalSensor.h>
#include <iopipe/CamIO/IHalCamIO.h>
#include <iopipe/CamIO/PortMap.h>
#include <camera_custom_nvram.h>
#include <nvbuf_util.h>
#include <camera_custom_eis.h>
#include <utils/SystemClock.h>
#include <utils/Trace.h>


using namespace std;
using namespace android;
using namespace NSCam;
using namespace NSCam::NSIoPipe;
using namespace NSCam::NSIoPipe::NSCamIOPipe;

#include "eis_hal_imp.h"

#if 1
#undef  ATRACE_TAG
#define ATRACE_TAG ATRACE_TAG_ALWAYS
#define DP_TRACE_CALL()                    ATRACE_CALL()
#define DP_TRACE_BEGIN(name)            ATRACE_BEGIN(name)
#define DP_TRACE_END()                      ATRACE_END()

#else

#define DP_TRACE_CALL()
#define DP_TRACE_BEGIN(name)
#define DP_TRACE_END()

#endif  // CONFIG_FOR_SYSTRACE


/*******************************************************************************
*
********************************************************************************/
#define EIS_HAL_DEBUG

#ifdef EIS_HAL_DEBUG

#undef __func__
#define __func__ __FUNCTION__

#undef  LOG_TAG
#define LOG_TAG "EisHal"
#define EIS_LOG(fmt, arg...)    CAM_LOGD("[%s]" fmt, __func__, ##arg)
#define EIS_INF(fmt, arg...)    CAM_LOGI("[%s]" fmt, __func__, ##arg)
#define EIS_WRN(fmt, arg...)    CAM_LOGW("[%s] WRN(%5d):" fmt, __func__, __LINE__, ##arg)
#define EIS_ERR(fmt, arg...)    CAM_LOGE("[%s] %s ERROR(%5d):" fmt, __func__,__FILE__, __LINE__, ##arg)

#else
#define EIS_LOG(a,...)
#define EIS_INF(a,...)
#define EIS_WRN(a,...)
#define EIS_ERR(a,...)
#endif

#define EIS_HAL_NAME "EisHal"
#define intPartShift 8
#define floatPartShift (31 - intPartShift)
#define DEBUG_DUMP_FRAMW_NUM 10

template <> EisHalObj<0>* EisHalObj<0>::spInstance = 0;
template <> EisHalObj<1>* EisHalObj<1>::spInstance = 0;
template <> EisHalObj<2>* EisHalObj<2>::spInstance = 0;
template <> EisHalObj<3>* EisHalObj<3>::spInstance = 0;

template <> Mutex EisHalObj<0>::s_instMutex(::Mutex::PRIVATE);
template <> Mutex EisHalObj<1>::s_instMutex(::Mutex::PRIVATE);
template <> Mutex EisHalObj<2>::s_instMutex(::Mutex::PRIVATE);
template <> Mutex EisHalObj<3>::s_instMutex(::Mutex::PRIVATE);



#define USING_FEFM    (0)
#define EISO_MEMORY_SIZE 256    // 32 * 64 (bits) = 256 bytes
#define EISO_BUFFER_NUM 30

/*******************************************************************************
*
********************************************************************************/
static MINT32 g_debugDump = 0;
static MFLOAT gAccInfo[3] = {0};
static MFLOAT gGyroInfo[3] = {0};


typedef struct EIS_TSRecord_t
{
    MUINT64 id;
    MUINT64 ts;
}EIS_TSRecord;


static queue<EIS_GyroRecord> gGyroDataQueue;
Mutex gGyroQueueLock;
Condition gWaitGyroCond;
MUINT64 gGyroCount = 0;

MUINT64 gGyroReverse = 0; //Add for distinguish the main cam and sub cam, only one instance can enter ConfigEis
MUINT64 gLastGyroTimestamp = 0;



#define RECORD_WITHOUT_EIS_ENABLE   (0) //Only used in AIM debug, Must be disable in normal case
#if RECORD_WITHOUT_EIS_ENABLE
static EIS_GyroRecord gGyroRecord[TSRECORD_MAXSIZE];
static EIS_TSRecord gTSRecord[TSRECORD_MAXSIZE];
static EIS_TSRecord gvHDRRecord[TSRECORD_MAXSIZE];
static MUINT32 gvHDRRecordWriteID = 0;
static MUINT32 gTSRecordWriteID = 0;
static MUINT32 gGyroRecordWriteID = 0;
static MINT32 g_AIMDump = 0;
#endif

static MINT32 g_EISInterval = 5;
static MINT32 g_EMEnabled = 0;

/*******************************************************************************
*
********************************************************************************/
void mySensorListener(ASensorEvent event)
{
    static MUINT32 accCnt = 1;
    static MUINT32 gyroCnt = 1;

    switch(event.type)
    {
        case ASENSOR_TYPE_ACCELEROMETER:
        {
            if (UNLIKELY(g_debugDump >= 2))
            {
                EIS_LOG("[%u] Acc(%f,%f,%f,%lld)",
                        accCnt++,
                        event.acceleration.x,
                        event.acceleration.y,
                        event.acceleration.z,
                        event.timestamp);
            }

            gAccInfo[0] = event.acceleration.x;
            gAccInfo[1] = event.acceleration.y;
            gAccInfo[2] = event.acceleration.z;

            break;
        }
        case ASENSOR_TYPE_MAGNETIC_FIELD:
        {
            if (UNLIKELY(g_debugDump >= 2))
            {
                EIS_LOG("Mag");
            }
            break;
        }
        case ASENSOR_TYPE_GYROSCOPE:
        {
            if (UNLIKELY(g_debugDump >= 2))
            {
                EIS_LOG("[%u] Gyro(%f,%f,%f,%lld)",
                        gyroCnt++,
                        event.vector.x,
                        event.vector.y,
                        event.vector.z,
                        event.timestamp);
            }

            gGyroInfo[0] = event.vector.x;
            if (UNLIKELY(gGyroReverse == 1))
            {
                gGyroInfo[1] = -event.vector.y;
                gGyroInfo[2] = -event.vector.z;
            }else
            {
                gGyroInfo[1] = event.vector.y;
                gGyroInfo[2] = event.vector.z;
            }



#define UNREASONABLE_GYRO_VALUE (10.0f)

            if ( LIKELY((event.vector.x < UNREASONABLE_GYRO_VALUE) && (event.vector.x > -UNREASONABLE_GYRO_VALUE) &&
                 (event.vector.y < UNREASONABLE_GYRO_VALUE) && (event.vector.y > -UNREASONABLE_GYRO_VALUE) &&
                 (event.vector.z < UNREASONABLE_GYRO_VALUE) && (event.vector.z > -UNREASONABLE_GYRO_VALUE)))
            {
                EIS_GyroRecord tmp;
                tmp.x = event.vector.x;
                if (UNLIKELY(gGyroReverse == 1))
                {
                    tmp.y = -event.vector.y;
                    tmp.z = -event.vector.z;
                }else
                {
                    tmp.y = event.vector.y;
                    tmp.z = event.vector.z;
                }
                tmp.ts = event.timestamp;

                gGyroCount++;
                gGyroQueueLock.lock();
                gGyroDataQueue.push(tmp);
                gWaitGyroCond.signal();
                gGyroQueueLock.unlock();
                DP_TRACE_CALL();
#if     RECORD_WITHOUT_EIS_ENABLE
                if (g_AIMDump == 1)
                {
                    gGyroRecord[gGyroRecordWriteID].id = EIS_GYROSCOPE;
                    gGyroRecord[gGyroRecordWriteID].x = event.vector.x;
                    if (UNLIKELY(gGyroReverse == 1))
                    {
                        gGyroRecord[gGyroRecordWriteID].y = -event.vector.y;
                        gGyroRecord[gGyroRecordWriteID].z = -event.vector.z;
                    }else
                    {
                        gGyroRecord[gGyroRecordWriteID].y = event.vector.y;
                        gGyroRecord[gGyroRecordWriteID].z = event.vector.z;
                    }
                    gGyroRecord[gGyroRecordWriteID].ts = event.timestamp;
                    gGyroRecordWriteID++;
                    if (gGyroRecordWriteID >= TSRECORD_MAXSIZE)
                        gGyroRecordWriteID = 0;

                    if (UNLIKELY(g_debugDump >= 2))
                    {
                        EIS_LOG("[%d] Gyro(%f,%f,%f,%lld)",
                                    gGyroRecordWriteID,
                                    event.vector.x,
                                    event.vector.y,
                                    event.vector.z,
                                    event.timestamp);
                    }

                }
#endif



            }else
            {
                EIS_ERR("Unreasonable gyro data(%f,%f,%f,%lld)", event.vector.x, event.vector.y, event.vector.z, event.timestamp);
                gLastGyroTimestamp = event.timestamp;
                gWaitGyroCond.signal();
            }
            break;
        }
        case ASENSOR_TYPE_LIGHT:
        {
            if (UNLIKELY(g_debugDump >= 2))
            {
                EIS_LOG("Light");
            }
            break;
        }
        case ASENSOR_TYPE_PROXIMITY:
        {
            if (UNLIKELY(g_debugDump >= 2))
            {
                EIS_LOG("Proxi");
            }
            break;
        }
        default:
        {
            EIS_WRN("unknown type(%d)",event.type);
            break;
        }
    }

}


/*******************************************************************************
*
********************************************************************************/
EisHal *EisHal::CreateInstance(char const *userName,const MUINT32 &aSensorIdx)
{
    EIS_LOG("%s",userName);
    return EisHalImp::GetInstance(aSensorIdx);
}

/*******************************************************************************
*
********************************************************************************/
EisHal *EisHalImp::GetInstance(const MUINT32 &aSensorIdx)
{
    EIS_LOG("sensorIdx(%u)",aSensorIdx);

    switch(aSensorIdx)
    {
        case 0 : return EisHalObj<0>::GetInstance();
        case 1 : return EisHalObj<1>::GetInstance();
        case 2 : return EisHalObj<2>::GetInstance();
        case 3 : return EisHalObj<3>::GetInstance();
        default :
            EIS_WRN("Current limit is 4 sensors, use 0");
            return EisHalObj<0>::GetInstance();
    }
}

/*******************************************************************************
*
********************************************************************************/
MVOID EisHalImp::DestroyInstance(char const *userName)
{
    EIS_LOG("%s",userName);
}

/*******************************************************************************
*
********************************************************************************/
EisHalImp::EisHalImp(const MUINT32 &aSensorIdx) : EisHal()
{
    mUsers = 0;

    //> EIS driver object
    m_pEisDrv = NULL;

    //> member variable
    mEisInput_W = 0;
    mEisInput_H = 0;
    mP1Target_W = 0;
    mP1Target_H = 0;
    mSrzOutW = 0;
    mSrzOutH = 0;
    mDoEisCount = 0;    //Vent@20140427: Add for EIS GMV Sync Check.
    mCmvX_Int = 0;
    mCmvX_Flt = 0;
    mCmvY_Int = 0;
    mCmvY_Flt = 0;
    mGMV_X = 0;
    mGMV_Y = 0;
    mMVtoCenterX = 0;
    mMVtoCenterY = 0;
    mFrameCnt = 0;
    mEisPass1Only = 0;
    mIsEisConfig = 0;
    mIsEisPlusConfig = 0;
    mSensorIdx = aSensorIdx;
    mEisSupport = MTRUE;
    mMemAlignment = 0;
    mEisPlusCropRatio = 20;
    mEisP2UserCnt = 0;
    mGyroEnable = MFALSE;
    mAccEnable  = MFALSE;

#if EIS_ALGO_READY
    m_pEisAlg = NULL;
    m_pEisPlusAlg = NULL;
    m_pGisAlg = NULL;
#endif

    mEisLastData2EisPlus.ConfX = mEisLastData2EisPlus.ConfY = 0;
    mEisLastData2EisPlus.GMVx = mEisLastData2EisPlus.GMVy = 0.0;
    mChangedInCalibration = 0;
    mNVRAMRead = MFALSE;
    mSleepTime = 0;
    mtRSTime = 0;
    mbLastCalibration = MTRUE;
    mSensorMode = SENSOR_SCENARIO_ID_NORMAL_PREVIEW;
    mSensorPixelClock = 0;
    mSensorLinePixel = 0;
    m_pNVRAM_defParameter = NULL;
    mBufIndex = 0;
    while(!mEISOBufferList.empty())
    {
        mEISOBufferList.pop();
    }

    m_pEisDbgBuf = NULL;
    m_pEisPlusWorkBuf = NULL;
    m_pGisWorkBuf = NULL;

    //> FE
    mFeoStatData.feX = NULL;
    mFeoStatData.feY = NULL;
    mFeoStatData.feRes = NULL;
    mFeoStatData.feDes = NULL;
    mFeoStatData.feValid = NULL;

    // sensor listener
    mpSensorListener = NULL;

    // sensor
    m_pHalSensorList = NULL;
    m_pHalSensor = NULL;

    // eis result data
    while(!mEis2EisPlusGmvTS.empty())
    {
        mEis2EisPlusGmvTS.pop();
    }

    while(!mEis2EisPlusGmvX.empty())
    {
        mEis2EisPlusGmvX.pop();
    }

    while(!mEis2EisPlusGmvY.empty())
    {
        mEis2EisPlusGmvY.pop();
    }

    while(!mEis2EisPlusConfX.empty())
    {
        mEis2EisPlusConfX.pop();
    }

    while(!mEis2EisPlusConfY.empty())
    {
        mEis2EisPlusConfY.pop();
    }

    while(!mEisResultForP2.empty())
    {
        mEisResultForP2.pop();
    }

    while(!mGisEisStatisticsQ.empty())
    {
        mGisEisStatisticsQ.pop();
    }

#if EIS_WORK_AROUND

    mTgRrzRatio = 0;

#endif

}

/*******************************************************************************
*
********************************************************************************/
MINT32 EisHalImp::Init()
{
    DP_TRACE_CALL();

    //====== Check Reference Count ======
    Mutex::Autolock lock(mLock);
    MUINT32 index;
    if(mUsers > 0)
    {
        android_atomic_inc(&mUsers);
        EIS_LOG("snesorIdx(%u) has %d users",mSensorIdx,mUsers);
        return EIS_RETURN_NO_ERROR;
    }

    MINT32 err = EIS_RETURN_NO_ERROR;

    //====== Dynamic Debug ======

#if (EIS_DEBUG_FLAG)

    EIS_INF("EIS_DEBUG_FLAG on");
    g_debugDump = 1;

#else

    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get("debug.eis.dump", value, "0");
    g_debugDump = atoi(value);

    property_get("eisrecord.setinterval", value, "5");
    g_EISInterval = atoi(value);


    property_get("debug.eis.EMEnabled", value, "0");
    g_EMEnabled = atoi(value);

#if RECORD_WITHOUT_EIS_ENABLE
    property_get("debug.eis.AIMDump", value, "0");
    g_AIMDump = atoi(value);
#endif

#endif

    gGyroQueueLock.lock();
    while(!gGyroDataQueue.empty())
    {
        gGyroDataQueue.pop();
    }
    gGyroQueueLock.unlock();

    //Init GIS member data
    m_pNvram = NULL;
    mChangedInCalibration = 0;
    mNVRAMRead = MFALSE;
    mSleepTime = 0;
    mtRSTime = 0;
    gGyroCount = 0;
    gGyroReverse = 0;
    gLastGyroTimestamp = 0;
    mbLastCalibration = MTRUE;
    mSensorMode = SENSOR_SCENARIO_ID_NORMAL_PREVIEW;
    mSensorPixelClock = 0;
    mSensorLinePixel = 0;
    mbEMSaveFlag = MFALSE;
    m_pNVRAM_defParameter = new NVRAM_CAMERA_FEATURE_GIS_STRUCT;
    memset(m_pNVRAM_defParameter, 0, sizeof(NVRAM_CAMERA_FEATURE_GIS_STRUCT));
    memset(mRecordParameter, 0, sizeof(mRecordParameter));

    mBufIndex = 0;


#if RECORD_WITHOUT_EIS_ENABLE
    gTSRecordWriteID = 0;
    gGyroRecordWriteID = 0;
    gvHDRRecordWriteID = 0;
    memset(&gTSRecord, 0, sizeof(gTSRecord));
    memset(&gvHDRRecord, 0, sizeof(gvHDRRecord));
    memset(&gGyroRecord, 0, sizeof(gTSRecord));
#endif

    EIS_LOG("(%p)  mSensorIdx(%u) init", this, mSensorIdx);

    //====== Create Sensor Object ======

    m_pHalSensorList = IHalSensorList::get();
    if(m_pHalSensorList == NULL)
    {
        EIS_ERR("IHalSensorList::get fail");
        goto create_fail_exit;
    }

    if(EIS_RETURN_NO_ERROR != GetSensorInfo())
    {
        EIS_ERR("GetSensorInfo fail");
        goto create_fail_exit;
    }

    //====== Create EIS Driver ======

    m_pEisDrv = EisDrv::CreateInstance(mSensorIdx);

    if(m_pEisDrv == NULL)
    {
        EIS_ERR("EisDrv::createInstance fail");
        goto create_fail_exit;
    }

    err = m_pEisDrv->Init();
    if(err != EIS_RETURN_NO_ERROR)
    {
        EIS_ERR("EisDrv::Init fail");
        goto create_fail_exit;
    }

    //====== Create EIS Algorithm Object ======

#if EIS_ALGO_READY

    EIS_LOG("TG(%d)",mSensorDynamicInfo.TgInfo);

    //if(mSensorDynamicInfo.TgInfo == CAM_TG_1)
    //TBD: Holmes, although EIS HW is in TG_1, but EisAlg could be create and then destroy in a pair action.
    {
        m_pEisAlg = MTKEis::createInstance();

        if(m_pEisAlg == NULL)
        {
            EIS_ERR("MTKEis::createInstance fail");
            goto create_fail_exit;
        }
    }

    if(mSensorDynamicInfo.TgInfo == CAM_TG_1 && m_pEisAlg == NULL)
    {
        EIS_ERR("m_pEisAlg is NULL");
        goto create_fail_exit;
    }

    m_pEisPlusAlg = MTKEisPlus::createInstance();

    if(m_pEisPlusAlg == NULL)
    {
        EIS_ERR("MTKEisPlus::createInstance fail");
        goto create_fail_exit;
    }

    m_pGisAlg = MTKGyro::createInstance();

    if (m_pGisAlg == NULL)
    {
        EIS_ERR("MTKGyro::createInstance fail");
        goto create_fail_exit;
    }

    //> get GPU info only

    EIS_PLUS_GET_PROC_INFO_STRUCT eisPlusGetProcData;

    err = m_pEisPlusAlg->EisPlusFeatureCtrl(EIS_PLUS_FEATURE_GET_PROC_INFO, NULL, &eisPlusGetProcData);
    if(err != S_EIS_PLUS_OK)
    {
        EIS_ERR("get GPU info fail(0x%x)",err);
        goto create_fail_exit;
    }

    // only Grid_W and Grid_H are valid here

    EIS_LOG("Grid:(W/H)=(%u/%u)",eisPlusGetProcData.Grid_W,eisPlusGetProcData.Grid_H);

    mGpuGridW = eisPlusGetProcData.Grid_W;
    mGpuGridH = eisPlusGetProcData.Grid_H;
#endif
    DP_TRACE_BEGIN("CreateMultiMemBuf");

    //Create EISO output buffer
    CreateMultiMemBuf(EISO_MEMORY_SIZE, (EISO_BUFFER_NUM+1), m_pEISOMainBuffer, m_pEISOSliceBuffer);
    if (!m_pEISOSliceBuffer[0]->getBufVA(0))
    {
        EIS_ERR("EISO slice buf create ImageBuffer fail");
        return EIS_RETURN_MEMORY_ERROR;
    }
    {

        Mutex::Autolock lock(mEISOBufferListLock);
        for (index =0; index<EISO_BUFFER_NUM; index++)
        {
            mEISOBufferList.push(m_pEISOSliceBuffer[index]);
        }
    }
    DP_TRACE_END();

    //====== Create Sensor Listener Object ======
    DP_TRACE_BEGIN("SensorListener");
    mpSensorListener = SensorListener::createInstance();

    if (MTRUE != mpSensorListener->setListener(mySensorListener))
    {
        EIS_ERR("setListener fail");
    }
    else
    {
        EIS_LOG("setListener success");
    }
    DP_TRACE_END();

    //====== Create FEO Memory ======
#if USING_FEFM
    mFeoStatData.feX     = new MUINT16[MAX_FEO_SIZE];
    mFeoStatData.feY     = new MUINT16[MAX_FEO_SIZE];
    mFeoStatData.feRes   = new MUINT16[MAX_FEO_SIZE];
    mFeoStatData.feDes   = new MUINT16[MAX_FEO_SIZE*32];
    mFeoStatData.feValid = new MUINT8[MAX_FEO_SIZE];

    memset(mFeoStatData.feX,0,sizeof(MUINT16)*MAX_FEO_SIZE);
    memset(mFeoStatData.feY,0,sizeof(MUINT16)*MAX_FEO_SIZE);
    memset(mFeoStatData.feRes,0,sizeof(MUINT16)*MAX_FEO_SIZE);
    memset(mFeoStatData.feDes,0,sizeof(MUINT16)*MAX_FEO_SIZE*32);
    memset(mFeoStatData.feValid,0,sizeof(MUINT8)*MAX_FEO_SIZE);
#endif
    //====== Get EIS Plus Crop Ratio ======

    EIS_PLUS_Customize_Para_t customSetting;

    get_EIS_PLUS_CustomizeData(&customSetting);

    mEisPlusCropRatio = 100 + customSetting.crop_ratio;
    EIS_LOG("mEisPlusCropRatio(%u)",mEisPlusCropRatio);

    //====== Increase User Count ======

    android_atomic_inc(&mUsers);

    EIS_LOG("-");
    return EIS_RETURN_NO_ERROR;

create_fail_exit:

    if (m_pEisDrv != NULL)
    {
        m_pEisDrv->Uninit();
        m_pEisDrv->DestroyInstance();
        m_pEisDrv = NULL;
    }

    if (m_pHalSensorList != NULL)
    {
        m_pHalSensorList = NULL;
    }

#if EIS_ALGO_READY

    if (m_pEisAlg != NULL)
    {
        m_pEisAlg->EisReset();
        m_pEisAlg->destroyInstance();
        m_pEisAlg = NULL;
    }

    if (m_pEisPlusAlg != NULL)
    {
        m_pEisPlusAlg->EisPlusReset();
        m_pEisPlusAlg->destroyInstance(m_pEisPlusAlg);
        m_pEisPlusAlg = NULL;
    }

#endif

    EIS_LOG("-");
    return EIS_RETURN_NULL_OBJ;
}

/*******************************************************************************
*
********************************************************************************/
MINT32 EisHalImp::Uninit()
{
    Mutex::Autolock lock(mLock);

    //====== Check Reference Count ======

    if(mUsers <= 0)
    {
        EIS_LOG("mSensorIdx(%u) has 0 user",mSensorIdx);
        return EIS_RETURN_NO_ERROR;
    }

    //====== Uninitialize ======

    android_atomic_dec(&mUsers);    //decrease referebce count

    if(mUsers == 0)    // there is no user
    {
        MINT32 err = EIS_RETURN_NO_ERROR;

        EIS_LOG("mSensorIdx(%u) uninit", mSensorIdx);

        if(m_pEisDrv != NULL)
        {
            EIS_LOG("m_pEisDrv uninit");

            m_pEisDrv->Uninit();
        	m_pEisDrv->DestroyInstance();
        	m_pEisDrv = NULL;
        }

        //======  Release EIS Algo Object ======

#if EIS_ALGO_READY

        if (UNLIKELY(g_debugDump >= 2))
        {
            EIS_LOG("mIsEisPlusConfig(%d)",mIsEisPlusConfig);
            if(mIsEisPlusConfig == 1)
            {
                err = m_pEisPlusAlg->EisPlusFeatureCtrl(EIS_PLUS_FEATURE_SAVE_LOG,(MVOID *)&mTsForAlgoDebug,NULL);
                if(err != S_EIS_PLUS_OK)
        	    {
        	        EIS_ERR("EisPlusFeatureCtrl(EIS_PLUS_FEATURE_SAVE_LOG) fail(0x%x)",err);
        	    }
            }

            if(mSensorDynamicInfo.TgInfo == CAM_TG_1)
            {
            	    err = m_pEisAlg->EisFeatureCtrl(EIS_FEATURE_SAVE_LOG, NULL, NULL);
        	    if(err != S_EIS_OK)
        	    {
        	        EIS_ERR("EisFeatureCtrl(EIS_FEATURE_SAVE_LOG) fail(0x%x)",err);
        	    }
            }
        }

#if !RECORD_WITHOUT_EIS_ENABLE
        if (UNLIKELY((g_debugDump >= 2) || (g_EMEnabled== 1)))
#endif
        {
            EIS_LOG("mIsEisPlusConfig(%d)",mIsEisPlusConfig);
            if(mIsEisPlusConfig == 1)
            {
                err = m_pGisAlg->GyroFeatureCtrl(GYRO_FEATURE_SAVE_LOG, (MVOID *)&mTsForAlgoDebug, NULL);
                if(err != S_GYRO_OK)
        	    {
        	        EIS_ERR("EisPlusFeatureCtrl(EIS_PLUS_FEATURE_SAVE_LOG) fail(0x%x)",err);
        	    }
            }
        }

#if RECORD_WITHOUT_EIS_ENABLE
        if (g_AIMDump == 1)
        {
            if (gTSRecordWriteID != 0)
            {
                MUINT32 LogCount,WriteCnt;
                char LogFileName[100];
                FILE * pLogFp ;

                LogCount = (mTsForAlgoDebug/(MUINT64)1000000000LL);

                EIS_LOG("RecordWriteID, mTsForAlgoDebug (%lld), LogCount(%d)",  mTsForAlgoDebug, LogCount);
                sprintf(LogFileName, "storage/sdcard0/gyro/EIS_Gyro_LOG_%d.bin", LogCount);
                pLogFp = fopen(LogFileName, "wb");
                if (NULL == pLogFp )
                {
                    EIS_ERR("Can't open file to save EIS HAL Log\n");
                }else
                {
                    WriteCnt = fwrite((void*)(&gGyroRecordWriteID),sizeof(gGyroRecordWriteID),1,pLogFp);
                    WriteCnt = fwrite(gGyroRecord,sizeof(gGyroRecord),1,pLogFp);
                    fflush(pLogFp);
                    fclose(pLogFp);
                }
                EIS_LOG("%d, gGyroRecordWriteID(%d)", LogCount, gGyroRecordWriteID);

                sprintf(LogFileName, "storage/sdcard0/gyro/EIS_TS_LOG_%d.bin", LogCount);
                pLogFp = fopen(LogFileName, "wb");
                if (NULL == pLogFp )
                {
                    EIS_ERR("Can't open file to save EIS HAL Log\n");
                }else
                {
                    MUINT64 timewithSleep = elapsedRealtime();
                    MUINT64 timewithoutSleep = uptimeMillis();
                    MUINT64 Diff =  timewithSleep - timewithoutSleep;
                    WriteCnt = fwrite((void*)(&gTSRecordWriteID),sizeof(gTSRecordWriteID),1,pLogFp);
                    WriteCnt = fwrite((void*)(&Diff),sizeof(Diff),1,pLogFp);
                    WriteCnt = fwrite(gTSRecord,sizeof(gTSRecord),1,pLogFp);
                    fflush(pLogFp);
                    fclose(pLogFp);
                }
                EIS_LOG("%d, gTSRecordWriteID(%d)", LogCount, gTSRecordWriteID);
#if 0
                sprintf(LogFileName, "storage/sdcard0/gyro/EIS_vHDR_LOG_%d.bin", LogCount);
                pLogFp = fopen(LogFileName, "wb");
                if (NULL == pLogFp )
                {
                    EIS_ERR("Can't open file to save EIS HAL Log\n");
                }else
                {
                    MUINT32 IsFirstLE;
                    IsFirstLE = mSensorStaticInfo.iHDR_First_IS_LE;
                    WriteCnt = fwrite((void*)(&gvHDRRecordWriteID),sizeof(gvHDRRecordWriteID),1,pLogFp);
                    WriteCnt = fwrite((void*)(&IsFirstLE),sizeof(IsFirstLE),1,pLogFp);
                    WriteCnt = fwrite(gvHDRRecord,sizeof(gvHDRRecord),1,pLogFp);
                    fflush(pLogFp);
                    fclose(pLogFp);
                }
                EIS_LOG("%d, gvHDRRecordWriteID(%d)", LogCount, gvHDRRecordWriteID);
#endif
            }
            gTSRecordWriteID = 0;
            gGyroRecordWriteID = 0;
            gvHDRRecordWriteID = 0;
            memset(&gTSRecord, 0, sizeof(gTSRecord));
            memset(&gvHDRRecord, 0, sizeof(gvHDRRecord));
            memset(&gGyroRecord, 0, sizeof(gGyroRecord));
        }
#endif
        //Writeback to NVRAM
        if (mNVRAMRead)
        {
            MUINT32 sensorDev;

            //Force update the NVRAM tmp buffer
            if (m_pNvram && m_pNVRAM_defParameter)
            {
                memcpy(&(m_pNvram->gis), m_pNVRAM_defParameter, sizeof(NVRAM_CAMERA_FEATURE_GIS_STRUCT));
            }

            sensorDev = m_pHalSensorList->querySensorDevIdx(mSensorIdx);
            err = NvBufUtil::getInstance().write(CAMERA_NVRAM_DATA_FEATURE, sensorDev);
        }

        if (m_pGisAlg != NULL)
        {
            EIS_LOG("m_pGisAlg uninit");
            m_pGisAlg->GyroReset();
            m_pGisAlg->destroyInstance(m_pGisAlg);
            m_pGisAlg = NULL;
        }

        EIS_LOG("TG(%d)",mSensorDynamicInfo.TgInfo);

        if (m_pEisAlg != NULL && mSensorDynamicInfo.TgInfo == CAM_TG_1)
        {
            EIS_LOG("m_pEisAlg uninit");
            m_pEisAlg->EisReset();
            m_pEisAlg->destroyInstance();
            m_pEisAlg = NULL;
        }

        if (m_pEisPlusAlg != NULL)
        {
            EIS_LOG("m_pEisPlusAlg uninit");
            m_pEisPlusAlg->EisPlusReset();
            m_pEisPlusAlg->destroyInstance(m_pEisPlusAlg);
            m_pEisPlusAlg = NULL;
        }
#endif

        // Next-Gen EIS
        if (mpSensorListener != NULL)
        {
            EIS_LOG("mpSensorListener uninit");
            mpSensorListener->disableSensor(SensorListener::SensorType_Acc);
            mpSensorListener->disableSensor(SensorListener::SensorType_Gyro);
            mpSensorListener->destroyInstance();
            mpSensorListener = NULL;
        }


        //====== Destroy Sensor Object ======

        if(m_pHalSensorList != NULL)
        {
            m_pHalSensorList = NULL;
        }

        //======  Release Memory and IMem Object ======


        if (UNLIKELY(g_debugDump >= 2))
        {
            //>  free EIS debug buffer
            m_pEisDbgBuf->unlockBuf("EISDbgBuf");
            DestroyMemBuf(m_pEisDbgBuf);
        }

        //>  free EIS Plus working buffer
        if (m_pEisPlusWorkBuf != NULL)
        {
            m_pEisPlusWorkBuf->unlockBuf("EISPlusWorkBuf");
            DestroyMemBuf(m_pEisPlusWorkBuf);
        }

        //>  free GIS working buffer

        if (m_pGisWorkBuf != NULL)
        {
            m_pGisWorkBuf->unlockBuf("GisWorkBuf");
            DestroyMemBuf(m_pGisWorkBuf);
        }

        DestroyMultiMemBuf((EISO_BUFFER_NUM+1), m_pEISOMainBuffer, m_pEISOSliceBuffer);

        //======  Reset Member Variable ======

        mEisInput_W = 0;
        mEisInput_H = 0;
        mP1Target_W = 0;
        mP1Target_H = 0;
        mSrzOutW = 0;
        mSrzOutH = 0;
        mFrameCnt = 0; // first frmae
        mEisPass1Only = 0;
        mIsEisConfig = 0;
        mIsEisPlusConfig = 0;
        mCmvX_Int = 0;
        mDoEisCount = 0;    //Vent@20140427: Add for EIS GMV Sync Check.
        mCmvX_Flt = 0;
        mCmvY_Int = 0;
        mMVtoCenterX = 0;
        mMVtoCenterY = 0;
        mCmvY_Flt = 0;
        mGMV_X = 0;
        mGMV_Y = 0;
        mGpuGridW = 0;
        mGpuGridH = 0;
        mMemAlignment = 0;
        mEisPlusCropRatio = 20;
        mEisP2UserCnt = 0;
        mGyroEnable = MFALSE;
        mAccEnable  = MFALSE;
        mEisLastData2EisPlus.ConfX = mEisLastData2EisPlus.ConfY = 0;
        mEisLastData2EisPlus.GMVx = mEisLastData2EisPlus.GMVy = 0.0;

        mChangedInCalibration = 0;
        mGisInputW = 0;
        mGisInputH = 0;
        mNVRAMRead = MFALSE;
        m_pNvram = NULL;
        mSleepTime = 0;
        mtRSTime = 0;
        mbLastCalibration = MTRUE;
        gGyroCount = 0;
        gLastGyroTimestamp = 0;
        mSensorMode = SENSOR_SCENARIO_ID_NORMAL_PREVIEW;
        mSensorPixelClock = 0;
        mSensorLinePixel = 0;
        mbEMSaveFlag = MFALSE;

        delete m_pNVRAM_defParameter;
        m_pNVRAM_defParameter = NULL;
        memset(mRecordParameter, 0, sizeof(mRecordParameter));
        mBufIndex = 0;

        {
            Mutex::Autolock lock(mEISOBufferListLock);
            while(!mEISOBufferList.empty())
            {
                mEISOBufferList.pop();
            }
        }

    #if EIS_WORK_AROUND

        mTgRrzRatio = 0;

    #endif

        //> FE
#if     USING_FEFM

        delete [] mFeoStatData.feX;
        delete [] mFeoStatData.feY;
        delete [] mFeoStatData.feRes;
        delete [] mFeoStatData.feDes;
        delete [] mFeoStatData.feValid;

#endif
        mFeoStatData.feX = NULL;
        mFeoStatData.feY = NULL;
        mFeoStatData.feRes = NULL;
        mFeoStatData.feDes = NULL;
        mFeoStatData.feValid = NULL;

        while(!mEis2EisPlusGmvTS.empty())
        {
            mEis2EisPlusGmvTS.pop();
        }

        while(!mEis2EisPlusGmvX.empty())
        {
            mEis2EisPlusGmvX.pop();
        }

        while(!mEis2EisPlusGmvY.empty())
        {
            mEis2EisPlusGmvY.pop();
        }

        while(!mEis2EisPlusConfX.empty())
        {
            mEis2EisPlusConfX.pop();
        }

        while(!mEis2EisPlusConfY.empty())
        {
            mEis2EisPlusConfY.pop();
        }

        while(!mEisResultForP2.empty())
        {
            mEisResultForP2.pop();
        }

        while(!mGisEisStatisticsQ.empty())
        {
            mGisEisStatisticsQ.pop();
        }

    }
    else
    {
        EIS_LOG("mSensorIdx(%u) has %d users",mSensorIdx,mUsers);
    }

    gGyroQueueLock.lock();
    while(!gGyroDataQueue.empty())
    {
        gGyroDataQueue.pop();
    }
    gGyroQueueLock.unlock();

    return EIS_RETURN_NO_ERROR;
}

/*******************************************************************************
*
********************************************************************************/
MINT32 EisHalImp::CreateMultiMemBuf(MUINT32 memSize, MUINT32 num, android::sp<IImageBuffer>& spMainImageBuf, android::sp<IImageBuffer> spImageBuf[MAX_MEMORY_SIZE])
{
    MINT32 err = EIS_RETURN_NO_ERROR;
    MUINT32 totalSize = memSize*num;

    if (num >= MAX_MEMORY_SIZE)
    {
        EIS_ERR("num of image buffer is larger than MAX_MEMORY_SIZE(%d)",MAX_MEMORY_SIZE);
        return EIS_RETURN_MEMORY_ERROR;
    }

    IImageBufferAllocator::ImgParam imgParam(totalSize, 0);

    sp<IIonImageBufferHeap> pHeap = IIonImageBufferHeap::create("EIS_HAL", imgParam);
    if (pHeap == NULL)
    {
        EIS_ERR("image buffer heap create fail");
        return EIS_RETURN_MEMORY_ERROR;
    }

    MUINT const usage = (GRALLOC_USAGE_SW_READ_OFTEN |
                        GRALLOC_USAGE_HW_CAMERA_READ |
                        GRALLOC_USAGE_HW_CAMERA_WRITE);
    spMainImageBuf = pHeap->createImageBuffer();
    if (spMainImageBuf == NULL)
    {
        EIS_ERR("mainImage buffer create fail");
        return EIS_RETURN_MEMORY_ERROR;
    }
    if (!(spMainImageBuf->lockBuf("EIS_HAL", usage)))
    {
        EIS_ERR(" image buffer lock fail");
        return EIS_RETURN_MEMORY_ERROR;
    }
    MUINTPTR const iVAddr = pHeap->getBufVA(0);
    MUINTPTR const iPAddr = pHeap->getBufPA(0);
    MINT32 const iHeapId = pHeap->getHeapID();

    EIS_LOG("IIonImageBufferHeap iVAddr:%p, iPAddr:%p, iHeapId:%d\n", iVAddr, iPAddr, iHeapId);
    EIS_LOG("spMainImageBuf iVAddr:%p, iPAddr:%p\n",spMainImageBuf->getBufVA(0),spMainImageBuf->getBufPA(0));

    MUINT32 index;
    size_t bufStridesInBytes[] = {memSize, 0, 0};
    size_t bufBoundaryInBytes[] = {0, 0, 0};

    for (index = 0; index < num; index++)
    {
        MUINTPTR const cVAddr = iVAddr + ((index)*(memSize));
        MUINTPTR const virtAddr[] = {cVAddr, 0, 0};
        MUINTPTR const cPAddr = iPAddr + ((index)*(memSize));
        MUINTPTR const phyAddr[] = {cPAddr, 0, 0};
        IImageBufferAllocator::ImgParam imgParam_t = IImageBufferAllocator::ImgParam(memSize, 0);
        PortBufInfo_dummy portBufInfo = PortBufInfo_dummy(iHeapId, virtAddr, phyAddr, 1);
        sp<IImageBufferHeap> imgBufHeap = IDummyImageBufferHeap::create("EIS_HAL", imgParam_t, portBufInfo, false);
        if (imgBufHeap == NULL)
        {
            EIS_ERR("acquire EIS_HAL - image buffer heap create fail");
            return EIS_RETURN_MEMORY_ERROR;
        }

        sp<IImageBuffer> imgBuf = imgBufHeap->createImageBuffer();
        if (imgBuf == NULL)
        {
            EIS_ERR("acquire EIS_HAL - image buffer create fail");
            return EIS_RETURN_MEMORY_ERROR;
        }

        if (!(imgBuf->lockBuf("EIS_HAL", usage)))
        {
            EIS_ERR("acquire EIS_HAL - image buffer lock fail");
            return EIS_RETURN_MEMORY_ERROR;
        }

        spImageBuf[index] = imgBuf;
    }
    EIS_LOG("X");
    return err;
}

/*******************************************************************************
*
********************************************************************************/
MINT32 EisHalImp::CreateMemBuf(MUINT32 memSize, android::sp<IImageBuffer>& spImageBuf)
{
    MINT32 err = EIS_RETURN_NO_ERROR;
    IImageBufferAllocator* pImageBufferAlloc = IImageBufferAllocator::getInstance();

    EIS_LOG("Size(%u)",memSize);
    IImageBufferAllocator::ImgParam bufParam((size_t)memSize, 0);
    spImageBuf = pImageBufferAlloc->alloc("EIS_HAL", bufParam);
    EIS_LOG("X");
    return err;
}

/******************************************************************************
*
*******************************************************************************/
MINT32 EisHalImp::DestroyMultiMemBuf(MUINT32 num, android::sp<IImageBuffer>& spMainImageBuf, android::sp<IImageBuffer> spImageBuf[MAX_MEMORY_SIZE])
{
    MINT32 err = EIS_RETURN_NO_ERROR;
    MUINT32 index;
    for (index = 0; index < num; index++)
    {
        spImageBuf[index]->unlockBuf("EIS_HAL");
        spImageBuf[index] = NULL;
    }

    spMainImageBuf->unlockBuf("EIS_HAL");
    spMainImageBuf = NULL;

    EIS_LOG("X");
    return err;
}

/******************************************************************************
*
*******************************************************************************/
MINT32 EisHalImp::DestroyMemBuf(android::sp<IImageBuffer>& spImageBuf)
{
    MINT32 err = EIS_RETURN_NO_ERROR;
    IImageBufferAllocator* pImageBufferAlloc = IImageBufferAllocator::getInstance();
    EIS_LOG("DestroyMemBuf");


    if (spImageBuf != NULL)
    {
        pImageBufferAlloc->free(spImageBuf.get());
        spImageBuf = NULL;
    }

    EIS_LOG("X");
    return err;
}

/*******************************************************************************
*
********************************************************************************/
MINT32 EisHalImp::GetSensorInfo()
{
    EIS_LOG("mSensorIdx(%u)",mSensorIdx);

    mSensorDev = m_pHalSensorList->querySensorDevIdx(mSensorIdx);
    m_pHalSensorList->querySensorStaticInfo(mSensorDev,&mSensorStaticInfo);

    m_pHalSensor = m_pHalSensorList->createSensor(EIS_HAL_NAME,1,&mSensorIdx);
    if(m_pHalSensor == NULL)
    {
        EIS_ERR("m_pHalSensorList->createSensor fail");
        return EIS_RETURN_API_FAIL;
    }

    if(MFALSE == m_pHalSensor->querySensorDynamicInfo(mSensorDev,&mSensorDynamicInfo))
    {
        EIS_ERR("querySensorDynamicInfo fail");
        return EIS_RETURN_API_FAIL;
    }

    m_pHalSensor->destroyInstance(EIS_HAL_NAME);
    m_pHalSensor = NULL;

    return EIS_RETURN_NO_ERROR;
}

/*******************************************************************************
*
********************************************************************************/
MINT32 EisHalImp::ConfigEis(const EIS_PASS_ENUM &aEisPass,const EIS_HAL_CONFIG_DATA &aEisConfig)
{
    if(mEisSupport == MFALSE)
    {
        EIS_LOG("mSensorIdx(%u) not support EIS",mSensorIdx);
        return EIS_RETURN_NO_ERROR;
    }

    MINT32 err = EIS_RETURN_NO_ERROR;

#if EIS_ALGO_READY

    static EIS_SET_ENV_INFO_STRUCT eisAlgoInitData;

#endif

    if(aEisPass == EIS_PASS_1)
    {
        //====== Get Pass1 Memory Alignment ======

        mMemAlignment = aEisConfig.memAlignment;

        if (UNLIKELY(g_debugDump >= 1))
        {
            EIS_LOG("mMemAlignment(%u)",mMemAlignment);
        }

        //====== Check EIS Configure Scenario ======

        switch(aEisConfig.configSce)
        {
            case EIS_SCE_EIS :
            case EIS_SCE_MFBLL :
            case EIS_SCE_MT :
            case EIS_SCE_MAV :
                mEisPass1Only = 1;
            break;
            case EIS_SCE_EIS_PLUS:
                mEisPass1Only = 0;
            break;
            default :
                EIS_ERR("wrong EIS config scenario(%d)",aEisConfig.configSce);
                return EIS_RETURN_INVALID_PARA;
        }

        //====== Check Sensor Type ======

        EIS_SENSOR_ENUM sensorType;

        switch(aEisConfig.sensorType)
        {
            case NSCam::NSSensorType::eRAW:
                sensorType = EIS_RAW_SENSOR;
                break;
            case NSCam::NSSensorType::eYUV:
                sensorType = EIS_YUV_SENSOR;
                break;
            default:
                EIS_ERR("not support sensor type(%u), use RAW setting",aEisConfig.sensorType);
                sensorType = EIS_RAW_SENSOR;
                break;
        }

        mSensorMode = aEisConfig.sensorMode;
        if (UNLIKELY(g_debugDump >= 1))
        {
            EIS_LOG("mSensorMode: %d ",mSensorMode);
        }
#if EIS_ALGO_READY

        //====== EIS Algo Init ======

        if (UNLIKELY(g_debugDump >= 1))
        {
            EIS_LOG("mIsEisConfig(%u)",mIsEisConfig);
        }

        if (mIsEisConfig == 0)
        {
            EIS_LOG("Sce(%d)",aEisConfig.configSce);

            //> get EIS customize setting

            GetEisCustomize(&eisAlgoInitData.eis_tuning_data);

            eisAlgoInitData.Eis_Input_Path = EIS_PATH_RAW_DOMAIN;   // RAW domain

            //> init EIS algo
            if ( mSensorDynamicInfo.TgInfo == CAM_TG_NONE)
            {
                //Reget sensor information
                if(EIS_RETURN_NO_ERROR != GetSensorInfo())
                {
                    EIS_ERR("GetSensorInfo fail");
                }

                EIS_LOG("TG(%d)",mSensorDynamicInfo.TgInfo);

            }

            err = m_pEisAlg->EisInit(&eisAlgoInitData);
            if (err != S_EIS_OK)
            {
                EIS_ERR("EisInit fail(0x%x)",err);
                return EIS_RETURN_API_FAIL;
            }

            if (UNLIKELY(g_debugDump >= 2))
            {
                //====== EIS Debug Buffer ======

                MUINT32 eisMemSize = 0;
                EIS_SET_LOG_BUFFER_STRUCT eisAlgoLogInfo;

                err = m_pEisAlg->EisFeatureCtrl(EIS_FEATURE_GET_DEBUG_INFO,NULL,&eisMemSize);
                if (err != S_EIS_OK || eisMemSize == 0)
                {
                    EIS_ERR("EisFeatureCtrl(EIS_FEATURE_SET_DEBUG_INFO) fail(0x%x)",err);
                    eisAlgoLogInfo.Eis_Log_Buf_Addr = NULL;
                    eisAlgoLogInfo.Eis_Log_Buf_Size = 0;

                }
                else
                {
                    CreateMemBuf(eisMemSize, m_pEisDbgBuf);
                    m_pEisDbgBuf->lockBuf("EISDbgBuf", eBUFFER_USAGE_SW_MASK);
                    if (!m_pEisDbgBuf->getBufVA(0))
                    {
                        EIS_ERR("mEisDbgBuf create ImageBuffer fail");
                        return EIS_RETURN_MEMORY_ERROR;
                    }

                    EIS_LOG("mEisDbgBuf : size(%u),virAdd(0x%p)", eisMemSize, m_pEisDbgBuf->getBufVA(0));
                    eisAlgoLogInfo.Eis_Log_Buf_Addr = (MVOID *)m_pEisDbgBuf->getBufVA(0);
                    eisAlgoLogInfo.Eis_Log_Buf_Size = eisMemSize;

                }
                err = m_pEisAlg->EisFeatureCtrl(EIS_FEATURE_SET_DEBUG_INFO, &eisAlgoLogInfo, NULL);
                if(err != S_EIS_OK)
                {
                    EIS_ERR("EisFeatureCtrl(EIS_FEATURE_SET_DEBUG_INFO) fail(0x%x)",err);
                }
            }

            mTsForAlgoDebug = 0;

        }

        if (UNLIKELY(g_debugDump >= 1))
        {
            EIS_LOG("EIS tuning_data");
            EIS_LOG("sensitivity(%d)",eisAlgoInitData.eis_tuning_data.sensitivity);
            EIS_LOG("filter_small_motion(%u)",eisAlgoInitData.eis_tuning_data.filter_small_motion);
            EIS_LOG("adv_shake_ext(%u)",eisAlgoInitData.eis_tuning_data.adv_shake_ext);
            EIS_LOG("stabilization_strength(%u)",eisAlgoInitData.eis_tuning_data.stabilization_strength);
            EIS_LOG("advtuning_data.new_tru_th(%u)",eisAlgoInitData.eis_tuning_data.advtuning_data.new_tru_th);
            EIS_LOG("advtuning_data.vot_th(%u)",eisAlgoInitData.eis_tuning_data.advtuning_data.vot_th);
            EIS_LOG("advtuning_data.votb_enlarge_size(%u)",eisAlgoInitData.eis_tuning_data.advtuning_data.votb_enlarge_size);
            EIS_LOG("advtuning_data.min_s_th(%u)",eisAlgoInitData.eis_tuning_data.advtuning_data.min_s_th);
            EIS_LOG("advtuning_data.vec_th(%u)",eisAlgoInitData.eis_tuning_data.advtuning_data.vec_th);
            EIS_LOG("advtuning_data.spr_offset(%u)",eisAlgoInitData.eis_tuning_data.advtuning_data.spr_offset);
            EIS_LOG("advtuning_data.spr_gain1(%u)",eisAlgoInitData.eis_tuning_data.advtuning_data.spr_gain1);
            EIS_LOG("advtuning_data.spr_gain2(%u)",eisAlgoInitData.eis_tuning_data.advtuning_data.spr_gain2);
            EIS_LOG("advtuning_data.vot_his_method(%d)",eisAlgoInitData.eis_tuning_data.advtuning_data.vot_his_method);
            EIS_LOG("advtuning_data.smooth_his_step(%u)",eisAlgoInitData.eis_tuning_data.advtuning_data.smooth_his_step);
            EIS_LOG("advtuning_data.eis_debug(%u)",eisAlgoInitData.eis_tuning_data.advtuning_data.eis_debug);

            for(MINT32 i = 0; i < 4; ++i)
            {
                EIS_LOG("gmv_pan_array[%d]=%u",i,eisAlgoInitData.eis_tuning_data.advtuning_data.gmv_pan_array[i]);
                EIS_LOG("gmv_sm_array[%d]=%u",i,eisAlgoInitData.eis_tuning_data.advtuning_data.gmv_sm_array[i]);
                EIS_LOG("cmv_pan_array[%d]=%u",i,eisAlgoInitData.eis_tuning_data.advtuning_data.cmv_pan_array[i]);
                EIS_LOG("cmv_sm_array[%d]=%u",i,eisAlgoInitData.eis_tuning_data.advtuning_data.cmv_sm_array[i]);
            }
        }
#endif

        //===== EIS HW Config ======

        err = m_pEisDrv->ConfigEisReg(aEisConfig.configSce,mSensorDynamicInfo.TgInfo);
        if (err != EIS_RETURN_NO_ERROR)
        {
            EIS_ERR("ConfigEisReg fail(0x%x)",err);
            return EIS_RETURN_API_FAIL;
        }

        //====== Enable EIS ======

#if EIS_WORK_AROUND

        mTgRrzRatio = m_pEisDrv->GetTgRrzRatio();
        EIS_LOG("mTgRrzRatio(%u)",mTgRrzRatio);

        if (mTgRrzRatio == 0)
        {
            EIS_ERR("mTgRrzRatio is 0",mTgRrzRatio);
            mTgRrzRatio = 1;
        }
        else if (mTgRrzRatio == 2)
        {
            mTgRrzRatio++;
        }

#endif

        //====== Enable EIS ======

        if (mIsEisConfig == 0)
        {
            err = m_pEisDrv->EnableEis(MTRUE);
            if(err != EIS_RETURN_NO_ERROR)
            {
                EIS_ERR("Enable EIS fail(0x%x)",err);
                return EIS_RETURN_API_FAIL;
            }
        }

        //====== Turn on Eis Configure One Time Flag ======

        if (mIsEisConfig == 0)
        {
            m_pHalSensor = m_pHalSensorList->createSensor(EIS_HAL_NAME,1,&mSensorIdx);

            if (LIKELY(m_pHalSensor != NULL))
            {
                err = m_pHalSensor->sendCommand(mSensorDev,SENSOR_CMD_GET_PIXEL_CLOCK_FREQ, (MUINTPTR)&mSensorPixelClock, 0, 0);
                if (err != EIS_RETURN_NO_ERROR)
                {
                    EIS_ERR("SENSOR_CMD_GET_PIXEL_CLOCK_FREQ is fail(0x%x)",err);
                    return EIS_RETURN_API_FAIL;
                }

                err = m_pHalSensor->sendCommand(mSensorDev,SENSOR_CMD_GET_FRAME_SYNC_PIXEL_LINE_NUM, (MUINTPTR)&mSensorLinePixel, 0, 0);
                if (err != EIS_RETURN_NO_ERROR)
                {
                    EIS_ERR("SENSOR_CMD_GET_PIXEL_CLOCK_FREQ is fail(0x%x)",err);
                    return EIS_RETURN_API_FAIL;
                }

                m_pHalSensor->destroyInstance(EIS_HAL_NAME);
                m_pHalSensor = NULL;

            }else
            {
                EIS_ERR("m_pHalSensorList->createSensor fail, m_pHalSensor == NULL");
            }

            EIS_LOG("mSensorDev(%u), pixelclock (%d), pixelline(%x)",mSensorDev,mSensorPixelClock,mSensorLinePixel);

            if (mSensorDev == SENSOR_DEV_SUB)
            {
                gGyroReverse = 1;
                EIS_LOG("mSensorDev(%u), GYRO data reversed",mSensorDev);
            }else
            {
                gGyroReverse = 0;
                EIS_LOG("mSensorDev(%u), GYRO data normal",mSensorDev);
            }

            // No use for accelerameter
            //mAccEnable  = mpSensorListener->enableSensor(SensorListener::SensorType_Acc,g_EISInterval);
            mAccEnable  = MFALSE;
            mGyroEnable = mpSensorListener->enableSensor(SensorListener::SensorType_Gyro,g_EISInterval);
            EIS_LOG("EN:(Acc,Gyro)=(%d,%d)",mAccEnable,mGyroEnable);

            // set EISO thread state
            m_pEisDrv->SetEisoThreadState(EIS_SW_STATE_ALIVE);
            mIsEisConfig = 1;
        }
    }
    else if (aEisPass == EIS_PASS_2)
    {
        if (mEisPass1Only == 1)
        {
            EIS_LOG("Only use EIS pass1");
            return EIS_RETURN_NO_ERROR;
        }
        else
        {
#if EIS_ALGO_READY

            if (mIsEisPlusConfig == 0)
            {
                EIS_LOG("EIS Plus first config");

                EIS_PLUS_SET_ENV_INFO_STRUCT eisPlusAlgoInitData;
                EIS_PLUS_GET_PROC_INFO_STRUCT eisPlusGetProcData;
                EIS_PLUS_SET_WORKING_BUFFER_STRUCT eisPlusWorkBufData;

                GYRO_INIT_INFO_STRUCT  gyroAlgoInitData;
                GYRO_GET_PROC_INFO_STRUCT gyroGetProcData;
                GYRO_SET_WORKING_BUFFER_STRUCT gyroSetworkingbuffer;

                //> prepare eisPlusAlgoInitData

                eisPlusAlgoInitData.wide_angle_lens = NULL;
                eisPlusAlgoInitData.debug = MFALSE;

                // get EIS Plus customized data
                GetEisPlusCustomize(&eisPlusAlgoInitData.eis_plus_tuning_data);

                if (UNLIKELY(g_debugDump >= 2))
                {
                    eisPlusAlgoInitData.debug = MTRUE;
                    EIS_LOG("eisPlus debug(%d)",eisPlusAlgoInitData.debug);
                }

                EIS_LOG("EIS Plus tuning_data");
                EIS_LOG("warping_mode(%d),effort(%d)",eisPlusAlgoInitData.eis_plus_tuning_data.warping_mode,eisPlusAlgoInitData.eis_plus_tuning_data.effort);
                EIS_LOG("search_range(%d,%d)",eisPlusAlgoInitData.eis_plus_tuning_data.search_range_x,eisPlusAlgoInitData.eis_plus_tuning_data.search_range_y);
                EIS_LOG("crop_ratio(%d),stabilization_strength(%f)",eisPlusAlgoInitData.eis_plus_tuning_data.crop_ratio,eisPlusAlgoInitData.eis_plus_tuning_data.stabilization_strength);

                //> Init Eis plus

                eisPlusAlgoInitData.GyroValid= mGyroEnable;
                eisPlusAlgoInitData.Gvalid= mAccEnable;
                eisPlusAlgoInitData.EIS_mode = 2; //TBD:Holmes , For GIS ???

                err = m_pEisPlusAlg->EisPlusInit(&eisPlusAlgoInitData);
                if(err != S_EIS_PLUS_OK)
                {
                    EIS_ERR("EisPlusInit fail(0x%x)",err);
                    return EIS_RETURN_API_FAIL;
                }

                //> Preapre EIS Plus Working Buffer

                err = m_pEisPlusAlg->EisPlusFeatureCtrl(EIS_PLUS_FEATURE_GET_PROC_INFO, NULL, &eisPlusGetProcData);
                if (err != S_EIS_PLUS_OK)
                {
                    EIS_ERR("EisPlus: EIS_PLUS_FEATURE_GET_PROC_INFO fail(0x%x)",err);
                    return EIS_RETURN_API_FAIL;
                }

                EIS_LOG("ext_mem_size(%u)",eisPlusGetProcData.ext_mem_size);

                CreateMemBuf(eisPlusGetProcData.ext_mem_size, m_pEisPlusWorkBuf);
                m_pEisPlusWorkBuf->lockBuf("EISPlusWorkBuf", eBUFFER_USAGE_SW_MASK);
                if (!m_pEisPlusWorkBuf->getBufVA(0))
                {
                    EIS_ERR("m_pEisPlusWorkBuf create ImageBuffer fail");
                    return EIS_RETURN_MEMORY_ERROR;
                }

                EIS_LOG("m_pEisPlusWorkBuf : size(%u),virAdd(0x%8x)", eisPlusGetProcData.ext_mem_size, m_pEisPlusWorkBuf->getBufVA(0));

                eisPlusWorkBufData.extMemSize = eisPlusGetProcData.ext_mem_size;
                eisPlusWorkBufData.extMemStartAddr = (MVOID *)m_pEisPlusWorkBuf->getBufVA(0);

                err = m_pEisPlusAlg->EisPlusFeatureCtrl(EIS_PLUS_FEATURE_SET_WORK_BUF_INFO, &eisPlusWorkBufData, NULL);
                if (err != S_EIS_PLUS_OK)
                {
                    EIS_ERR("EisPlus: EIS_PLUS_FEATURE_SET_WORK_BUF_INFO fail(0x%x)",err);
                    return EIS_RETURN_API_FAIL;
                }

                memset(&gyroAlgoInitData, 0, sizeof(gyroAlgoInitData));
                MUINT64 timewithSleep = elapsedRealtime();
                MUINT64 timewithoutSleep = uptimeMillis();
                m_pEisDrv->GetRegSetting(&gyroAlgoInitData);

                //====== Read NVRAM calibration data ======
                MUINT32 sensorDev;
                sensorDev = m_pHalSensorList->querySensorDevIdx(mSensorIdx);
                err = NvBufUtil::getInstance().getBufAndRead(CAMERA_NVRAM_DATA_FEATURE, sensorDev, (void*&)m_pNvram);
                if (m_pNVRAM_defParameter && m_pNvram)
                {
                    memcpy(m_pNVRAM_defParameter, &(m_pNvram->gis), sizeof(NVRAM_CAMERA_FEATURE_GIS_STRUCT));
                }else
                {
                    EIS_ERR("m_pNVRAM_defParameter OR m_pNVRAM_defParameter is NULL\n");
                    return  EIS_RETURN_NULL_OBJ;
                }
                mNVRAMRead = MFALSE; //No write back

                mRecordParameter[0] = m_pNVRAM_defParameter->gis_defParameter3[0];
                mRecordParameter[1] = m_pNVRAM_defParameter->gis_defParameter3[1];
                mRecordParameter[2] = m_pNVRAM_defParameter->gis_defParameter3[2];
                mRecordParameter[3] = m_pNVRAM_defParameter->gis_defParameter3[3];
                mRecordParameter[4] = m_pNVRAM_defParameter->gis_defParameter3[4];
                mRecordParameter[5] = m_pNVRAM_defParameter->gis_defParameter3[5];

                //> prepare eisPlusAlgoInitData
                ////////////////////////////////////////////////////////////////////////////////////////////////////////
                if (mSensorMode >= GIS_MAXSUPPORTED_SMODE)
                    mSensorMode = 0;

                {
                    double tRS = 0.0f,numLine;
                    numLine = mSensorLinePixel&0xFFFF;
                    if (mSensorPixelClock != 0)
                    {
                        tRS = numLine / mSensorPixelClock;
                        tRS = tRS * (float)(gyroAlgoInitData.sensor_Height-1);
                    }else
                    {
                        EIS_WRN("mSensorPixelClock is %d, so can NOT get tRS",mSensorPixelClock);
                    }

                    EIS_LOG("SensorModeNum: %d, tRS in table: %f, calculated tRS: %f",mSensorMode,
                                                                                                                  m_pNVRAM_defParameter->gis_deftRS[mSensorMode],
                                                                                                                  tRS);

                    //Replace the tRS from the table by current sensor mode
                    mRecordParameter[0] = tRS;

                    tRS+= m_pNVRAM_defParameter->gis_defParameter1[5];
                    //Check 30 fps maxmum
                    if (tRS > 0.042f)
                    {
                        EIS_WRN("30 fps tRS+tOffset: %f should be small than 0.042 ms", tRS);
                    }
                    mtRSTime = (long long)((double)tRS*1000000.0f);
                    EIS_LOG("waiting gyro time: %lld", mtRSTime);

                }

                gyroAlgoInitData.param_Width = m_pNVRAM_defParameter->gis_defWidth;
                gyroAlgoInitData.param_Height= m_pNVRAM_defParameter->gis_defHeight;
                gyroAlgoInitData.param_crop_Y= m_pNVRAM_defParameter->gis_defCrop;
                gyroAlgoInitData.ProcMode = GYRO_PROC_MODE_MV;
                gyroAlgoInitData.param = mRecordParameter;
                gyroAlgoInitData.sleep_t =  timewithSleep - timewithoutSleep;
                mSleepTime = gyroAlgoInitData.sleep_t;

                EIS_LOG("def data Rec: %f    %f    %f    %f    %f    %f", mRecordParameter[0], mRecordParameter[1], mRecordParameter[2],
                                                                                             mRecordParameter[3], mRecordParameter[4], mRecordParameter[5]);

                gyroAlgoInitData.crz_crop_X = aEisConfig.cropX;
                gyroAlgoInitData.crz_crop_Y = aEisConfig.cropY;

                gyroAlgoInitData.crz_crop_Width = aEisConfig.crzOutW;
                gyroAlgoInitData.crz_crop_Height = aEisConfig.crzOutH;

#if !RECORD_WITHOUT_EIS_ENABLE
                if (UNLIKELY(g_debugDump >= 2))
#endif
                {
                    gyroAlgoInitData.debug = MTRUE;
                }
                EIS_LOG("sleep_t is (%lld)", gyroAlgoInitData.sleep_t);
                EIS_LOG("aEisConfig  IMG w(%d),h(%d)", aEisConfig.imgiW, aEisConfig.imgiH);

                EIS_LOG("crz offset x(%d),y(%d)", gyroAlgoInitData.crz_crop_X, gyroAlgoInitData.crz_crop_Y);
                EIS_LOG("crzOut w(%d),h(%d)", gyroAlgoInitData.crz_crop_Width, gyroAlgoInitData.crz_crop_Height);

                //TBD: Holmes new structure for Gyro init
                err = m_pGisAlg->GyroInit(&gyroAlgoInitData);
                if (err != S_GYRO_OK)
                {
                    EIS_ERR("GyroInit fail(0x%x)",err);
                    return EIS_RETURN_API_FAIL;
                }

                err = m_pGisAlg->GyroFeatureCtrl(GYRO_FEATURE_GET_PROC_INFO, NULL, &gyroGetProcData);
                if (err != S_GYRO_OK)
                {
                    EIS_ERR("get Gyro proc info fail(0x%x)",err);
                    return EIS_RETURN_API_FAIL;
                }

                CreateMemBuf(gyroGetProcData.ext_mem_size, m_pGisWorkBuf);
                m_pGisWorkBuf->lockBuf("GisWorkBuf", eBUFFER_USAGE_SW_MASK);
                if (!m_pGisWorkBuf->getBufVA(0))
                {
                    EIS_ERR("m_pGisWorkBuf create ImageBuffer fail");
                    return EIS_RETURN_MEMORY_ERROR;
                }

                gyroSetworkingbuffer.extMemStartAddr = (MVOID *)m_pGisWorkBuf->getBufVA(0);
                gyroSetworkingbuffer.extMemSize = gyroGetProcData.ext_mem_size;

                err = m_pGisAlg->GyroFeatureCtrl(GYRO_FEATURE_SET_WORK_BUF_INFO, &gyroSetworkingbuffer, NULL);
                if(err != S_GYRO_OK)
                {

                    EIS_ERR("mGisWorkBuf create IMem fail");
                    return EIS_RETURN_API_FAIL;
                }

                //> confige FEO
                err = m_pEisDrv->ConfigFeo();
                if(err != EIS_RETURN_NO_ERROR)
                {
                    EIS_ERR("ConfigFeo fail(0x%x)",err);
                    return EIS_RETURN_API_FAIL;
                }

                mIsEisPlusConfig = 1;
            }
#endif
        }
    }
    else
    {
        EIS_ERR("Wrong EIS config pass(%d)",aEisPass);
    }

    return EIS_RETURN_NO_ERROR;
}

/*******************************************************************************
*
********************************************************************************/
MINT32 EisHalImp::ConfigCalibration(const EIS_PASS_ENUM &aEisPass,const EIS_HAL_CONFIG_DATA &aEisConfig)
{
    if(mEisSupport == MFALSE)
    {
        EIS_LOG("mSensorIdx(%u) not support EIS",mSensorIdx);
        return EIS_RETURN_NO_ERROR;
    }

    MINT32 err = EIS_RETURN_NO_ERROR;

    if(aEisPass == EIS_PASS_2)
    {
        if(mEisPass1Only == 1)
        {
            EIS_LOG("Only use EIS pass1");
            return EIS_RETURN_NO_ERROR;
        }
        else
        {
 #if EIS_ALGO_READY

            if(mIsEisPlusConfig == 0)
            {
                EIS_LOG("GIS first config for calibration");

                GYRO_INIT_INFO_STRUCT  gyroAlgoInitData;
                GYRO_GET_PROC_INFO_STRUCT gyroGetProcData;
                GYRO_SET_WORKING_BUFFER_STRUCT gyroSetworkingbuffer;

                //====== Read NVRAM calibration data ======
                MUINT32 sensorDev;
                sensorDev = m_pHalSensorList->querySensorDevIdx(mSensorIdx);
                err = NvBufUtil::getInstance().getBufAndRead(CAMERA_NVRAM_DATA_FEATURE, sensorDev, (void*&)m_pNvram);
                if (m_pNVRAM_defParameter && m_pNvram)
                {
                    memcpy(m_pNVRAM_defParameter, &(m_pNvram->gis), sizeof(NVRAM_CAMERA_FEATURE_GIS_STRUCT));
                }else
                {
                    EIS_ERR("m_pNVRAM_defParameter OR m_pNVRAM_defParameter is NULL\n");
                    return  EIS_RETURN_NULL_OBJ;
                }
                mNVRAMRead = MTRUE;
                //> prepare eisPlusAlgoInitData

                //> Init GIS
                ////////////////////////////////////////////////////////////////////////////////////////////////////////
                memset(&gyroAlgoInitData, 0, sizeof(gyroAlgoInitData));
                MUINT64 timewithSleep = elapsedRealtime();
                MUINT64 timewithoutSleep = uptimeMillis();
                m_pEisDrv->GetRegSetting(&gyroAlgoInitData);
                m_pEisDrv->GetEisInputSize(&mGisInputW, &mGisInputH);

                //Get tRS
                if (mSensorMode >= GIS_MAXSUPPORTED_SMODE)
                    mSensorMode = 0;

                {
                    double tRS = 0.0f,numLine;
                    numLine = mSensorLinePixel&0xFFFF;
                    if (mSensorPixelClock != 0)
                    {
                        tRS = numLine / (float)mSensorPixelClock;
                        EIS_LOG("Small tRS: %f ",tRS);
                        tRS = tRS * (float)(gyroAlgoInitData.sensor_Height-1);
                        EIS_LOG("Big tRS: %f ",tRS);
                    }else
                    {
                        EIS_WRN("mSensorPixelClock is %d, so can NOT get tRS",mSensorPixelClock);
                    }

                    EIS_LOG("SensorModeNum: %d, tRS in table: %f, calculated tRS: %f",mSensorMode,
                                                                                                                  m_pNVRAM_defParameter->gis_deftRS[mSensorMode],
                                                                                                                  tRS);

                    m_pNVRAM_defParameter->gis_defParameter1[0] = tRS;
                    m_pNVRAM_defParameter->gis_defParameter2[0] = tRS;
                    m_pNVRAM_defParameter->gis_defParameter3[0] = tRS;
                }


                EIS_LOG("def data 0: %f    %f    %f    %f    %f    %f", m_pNVRAM_defParameter->gis_defParameter1[0], m_pNVRAM_defParameter->gis_defParameter1[1],
                                                                        m_pNVRAM_defParameter->gis_defParameter1[2], m_pNVRAM_defParameter->gis_defParameter1[3],
                                                                        m_pNVRAM_defParameter->gis_defParameter1[4], m_pNVRAM_defParameter->gis_defParameter1[5]);

                EIS_LOG("def data 1: %f    %f    %f    %f    %f    %f", m_pNVRAM_defParameter->gis_defParameter2[0], m_pNVRAM_defParameter->gis_defParameter2[1],
                                                                        m_pNVRAM_defParameter->gis_defParameter2[2], m_pNVRAM_defParameter->gis_defParameter2[3],
                                                                        m_pNVRAM_defParameter->gis_defParameter2[4], m_pNVRAM_defParameter->gis_defParameter2[5]);

                EIS_LOG("def data 2: %f    %f    %f    %f    %f    %f", m_pNVRAM_defParameter->gis_defParameter3[0], m_pNVRAM_defParameter->gis_defParameter3[1],
                                                                        m_pNVRAM_defParameter->gis_defParameter3[2], m_pNVRAM_defParameter->gis_defParameter3[3],
                                                                        m_pNVRAM_defParameter->gis_defParameter3[4], m_pNVRAM_defParameter->gis_defParameter3[5]);

                mRecordParameter[0] = m_pNVRAM_defParameter->gis_defParameter3[0];
                mRecordParameter[1] = m_pNVRAM_defParameter->gis_defParameter3[1];
                mRecordParameter[2] = m_pNVRAM_defParameter->gis_defParameter3[2];
                mRecordParameter[3] = m_pNVRAM_defParameter->gis_defParameter3[3];
                mRecordParameter[4] = m_pNVRAM_defParameter->gis_defParameter3[4];
                mRecordParameter[5] = m_pNVRAM_defParameter->gis_defParameter3[5];

                gyroAlgoInitData.param_Width = m_pNVRAM_defParameter->gis_defWidth;
                gyroAlgoInitData.param_Height= m_pNVRAM_defParameter->gis_defHeight;
                gyroAlgoInitData.param_crop_Y= m_pNVRAM_defParameter->gis_defCrop;

                gyroAlgoInitData.crz_crop_X = aEisConfig.cropX;
                gyroAlgoInitData.crz_crop_Y = aEisConfig.cropY;

                gyroAlgoInitData.crz_crop_Width = aEisConfig.crzOutW;
                gyroAlgoInitData.crz_crop_Height = aEisConfig.crzOutH;

                gyroAlgoInitData.ProcMode = GYRO_PROC_MODE_CAL;
                gyroAlgoInitData.param = m_pNVRAM_defParameter->gis_defParameter1;
                gyroAlgoInitData.sleep_t =  timewithSleep - timewithoutSleep;
                EIS_LOG("sleep_t is (%lld)", gyroAlgoInitData.sleep_t);
                EIS_LOG("rrz crop w(%d),h(%d)", gyroAlgoInitData.rrz_crop_Width, gyroAlgoInitData.rrz_crop_Height);
                EIS_LOG("rrzOut w(%d),h(%d)", gyroAlgoInitData.rrz_scale_Width, gyroAlgoInitData.rrz_scale_Height);
                EIS_LOG("crz offset x(%d),y(%d)", gyroAlgoInitData.crz_crop_X, gyroAlgoInitData.crz_crop_Y);
                EIS_LOG("crzOut w(%d),h(%d)", gyroAlgoInitData.crz_crop_Width, gyroAlgoInitData.crz_crop_Height);
                EIS_LOG("aEisConfig  PIXEL_MODE(%d),op h(%d), opv(%d), rp h(%d), rp v(%d)", gyroAlgoInitData.GyroCalInfo.PIXEL_MODE,
                                                                                gyroAlgoInitData.GyroCalInfo.EIS_OP_H_step, gyroAlgoInitData.GyroCalInfo.EIS_OP_V_step,
                                                                                gyroAlgoInitData.GyroCalInfo.EIS_RP_H_num, gyroAlgoInitData.GyroCalInfo.EIS_RP_V_num);
                if (UNLIKELY(g_EMEnabled == 1))
                {
                    gyroAlgoInitData.debug = MTRUE;
                    gyroAlgoInitData.EMmode = MTRUE;
                    mbEMSaveFlag = MFALSE;
                }

#if !RECORD_WITHOUT_EIS_ENABLE
                if (UNLIKELY(g_debugDump >= 2))
#endif
                {
                    gyroAlgoInitData.debug = MTRUE;
                }

                err = m_pGisAlg->GyroInit(&gyroAlgoInitData);
                if(err != S_GYRO_OK)
                {
                    EIS_ERR("GyroInit fail(0x%x)",err);
                    return EIS_RETURN_API_FAIL;
                }

                err = m_pGisAlg->GyroFeatureCtrl(GYRO_FEATURE_GET_PROC_INFO, NULL, &gyroGetProcData);
                if(err != S_GYRO_OK)
                {
                    EIS_ERR("get Gyro proc info fail(0x%x)",err);
                    return EIS_RETURN_API_FAIL;
                }

                CreateMemBuf(gyroGetProcData.ext_mem_size, m_pGisWorkBuf);
                m_pGisWorkBuf->lockBuf("GisWorkBuf", eBUFFER_USAGE_SW_MASK);
                if (!m_pGisWorkBuf->getBufVA(0))
                {
                    EIS_ERR("m_pGisWorkBuf create ImageBuffer fail");
                    return EIS_RETURN_MEMORY_ERROR;
                }

                gyroSetworkingbuffer.extMemStartAddr = (MVOID *)m_pGisWorkBuf->getBufVA(0);
                gyroSetworkingbuffer.extMemSize = gyroGetProcData.ext_mem_size;

                err = m_pGisAlg->GyroFeatureCtrl(GYRO_FEATURE_SET_WORK_BUF_INFO, &gyroSetworkingbuffer, NULL);
                if(err != S_GYRO_OK)
                {

                    EIS_ERR("mGisWorkBuf GYRO_FEATURE_SET_WORK_BUF_INFO fail");
                    return EIS_RETURN_API_FAIL;
                }
                mIsEisPlusConfig = 1;
            }
#endif
        }
    }
    else
    {
        EIS_ERR("Wrong EIS config pass(%d) for calibration",aEisPass);
    }

    return EIS_RETURN_NO_ERROR;
}


MINT32 EisHalImp::ForcedDoEisPass2()
{
    DP_TRACE_CALL();
    gGyroQueueLock.lock();
    mSkipWaitGyro = MTRUE;
    gWaitGyroCond.signal();
    gGyroQueueLock.unlock();
    return EIS_RETURN_NO_ERROR;
}


MINT32 EisHalImp::AbortP2Calibration()
{
    MINT32 err = S_GYRO_OK;

#if EIS_ALGO_READY
    if (m_pGisAlg != NULL)
    {
        err = m_pGisAlg->GyroFeatureCtrl(GYRO_FEATURE_SET_CAL_NONACTIVE, NULL, NULL);
        if(err != S_GYRO_OK)
        {
            EIS_ERR("abort Calibration fail(0x%x)",err);
            return EIS_RETURN_API_FAIL;
        }
    }
#endif
    return EIS_RETURN_NO_ERROR;
}

/*******************************************************************************
*
********************************************************************************/
MINT32 EisHalImp::DoEis(const EIS_PASS_ENUM &aEisPass, QBufInfo const &pBufInfo)
{
    MINT32 err = EIS_RETURN_NO_ERROR;
    const MUINT64 aTimeStamp = pBufInfo.mvOut[0].mMetaData.mTimeStamp; //Maybe framedone

    if (mEisSupport == MFALSE)
    {
        EIS_LOG("mSensorIdx(%u) not support EIS",mSensorIdx);
        return EIS_RETURN_NO_ERROR;
    }

    if (UNLIKELY(mTsForAlgoDebug == 0))
    {
        mTsForAlgoDebug = aTimeStamp;
    }

    if (UNLIKELY(g_debugDump >= 1))
    {
        EIS_LOG("aEisPass(%d),mEisPass1Only(%u)",aEisPass,mEisPass1Only);
    }

    if(aEisPass == EIS_PASS_1)
    {

        if (aTimeStamp <= 0)
        {
            m_pEisDrv->GetEisHwStatistic(NULL,0);   // MW drop frame
        }
        else
        {
#if EIS_ALGO_READY

            //====== Setting EIS Algo Process Data ======

            EIS_RESULT_INFO_STRUCT   eisCMVResult;
            MINTPTR eisoBufferVA = NULL;

            //rrzo
            for (size_t i = 0; i < pBufInfo.mvOut.size(); i++)
            {
                if (pBufInfo.mvOut[i].mPortID.index == PORT_RRZO.index)
                {
                    //crop region
                    mEisInput_W = pBufInfo.mvOut.at(i).mMetaData.mDstSize.w;
                    mEisInput_H = pBufInfo.mvOut.at(i).mMetaData.mDstSize.h;
                }

                if (pBufInfo.mvOut[i].mPortID.index == PORT_EISO.index)
                {
                    eisoBufferVA= pBufInfo.mvOut.at(i).mBuffer->getBufVA(0);
                }
            }


            {
                //> EIS input image size
                Mutex::Autolock lock(mP1Lock);

                //TBD!!!!Holmes
                //mEisInput_W = apEisConfig->p1ImgW;
                //mEisInput_H = apEisConfig->p1ImgH;

                if(m_pEisDrv->Get2PixelMode() == 1)
                {
                    mEisInput_W >>= 1;
                }else if(m_pEisDrv->Get2PixelMode() == 2)
                {
                    mEisInput_W >>= 2;
                }

                mEisInput_W -= 4;   //ryan wang request to -4
                mEisInput_H -= 4;   //ryan wang request to -4

                if(mEisPass1Only == 1)
                {
                    if(g_debugDump == 1)
                    {
                        EIS_LOG("EIS Pass1 Only");
                    }

                    mP1Target_W = (mEisInput_W / (EIS_FACTOR / 100.0));
                    mP1Target_H = (mEisInput_H / (EIS_FACTOR / 100.0));
                }
                else
                {
                    if(g_debugDump == 1)
                    {
                        EIS_LOG("mEisPlusCropRatio(%u)",mEisPlusCropRatio);
                    }

                    mP1Target_W = (mEisInput_W / (mEisPlusCropRatio / 100.0));
                    mP1Target_H = (mEisInput_H / (mEisPlusCropRatio / 100.0));
                }

                mEisAlgoProcData.eis_image_size_config.InputWidth   = mEisInput_W;
                mEisAlgoProcData.eis_image_size_config.InputHeight  = mEisInput_H;
                mEisAlgoProcData.eis_image_size_config.TargetWidth  = mP1Target_W;
                mEisAlgoProcData.eis_image_size_config.TargetHeight = mP1Target_H;
            }

            EIS_LOG("EisIn(%u,%u),P1T(%u,%u)",mEisInput_W,mEisInput_H,mP1Target_W,mP1Target_H);

            //> get EIS HW statistic
            if (EIS_RETURN_EISO_MISS == m_pEisDrv->GetEisHwStatistic(eisoBufferVA, &mEisAlgoProcData.eis_state))
            {
                EIS_WRN("EISO data miss");

                //> use latst data

                mEis2EisPlusGmvTS.push(aTimeStamp);
                mEis2EisPlusGmvX.push(mEisLastData2EisPlus.GMVx);
                mEis2EisPlusGmvY.push(mEisLastData2EisPlus.GMVy);
                mEis2EisPlusConfX.push(mEisLastData2EisPlus.ConfX);
                mEis2EisPlusConfY.push(mEisLastData2EisPlus.ConfY);

                if (mEisP2UserCnt != 0)
                {
                    SaveEisResultForP2(mEisLastData2EisPlus.ConfX,mEisLastData2EisPlus.ConfY,aTimeStamp);
                }

                return EIS_RETURN_NO_ERROR;
            }

            if (UNLIKELY(g_debugDump == 3))
            {
                DumpStatistic(mEisAlgoProcData.eis_state);
            }


            GyroEISStatistics eisResult;
            eisResult.ts = aTimeStamp;
            for (int i=0; i<EIS_WIN_NUM; i++)
            {
                eisResult.eis_data[4*i + 0] = mEisAlgoProcData.eis_state.i4LMV_X[i];
                eisResult.eis_data[4*i + 1] = mEisAlgoProcData.eis_state.i4LMV_Y[i];
                eisResult.eis_data[4*i + 2] = mEisAlgoProcData.eis_state.NewTrust_X[i];
                eisResult.eis_data[4*i + 3] = mEisAlgoProcData.eis_state.NewTrust_Y[i];
                //EIS_LOG("frame: %d, %d, (LMV_X,LMV_Y,TX,TY = (%d, %d, %d, %d)", mDoEisCount, i, eisResult.eis_data[4*i + 0], eisResult.eis_data[4*i + 1], eisResult.eis_data[4*i + 2], eisResult.eis_data[4*i + 3]);
            }
            GisEisStatisticsQLock.lock();
            mGisEisStatisticsQ.push(eisResult);
            GisEisStatisticsQLock.unlock();


            //> get EIS HW setting of eis_op_vert and eis_op_hori

            mEisAlgoProcData.DivH = m_pEisDrv->GetEisDivH();
            mEisAlgoProcData.DivV = m_pEisDrv->GetEisDivV();

            //> get MB number

            mEisAlgoProcData.EisWinNum = m_pEisDrv->GetEisMbNum();

            //> get Acc & Gyro info
            //TBD: Holmes,  Deprecated in GIS or later
            //mEisAlgoProcData.sensor_info.GyroValid = mGyroEnable;
            //mEisAlgoProcData.sensor_info.Gvalid    = mAccEnable;

            for (MUINT32 i = 0; i < 3; i++)
            {
                mEisAlgoProcData.sensor_info.AcceInfo[i] = gAccInfo[i];
                mEisAlgoProcData.sensor_info.GyroInfo[i] = gGyroInfo[i];
            }

            if (UNLIKELY(g_debugDump >= 1))
            {
                EIS_LOG("EN:(Acc,Gyro)=(%d,%d)",mAccEnable,mGyroEnable);
                EIS_LOG("EIS:Acc(%f,%f,%f)",mEisAlgoProcData.sensor_info.AcceInfo[0],mEisAlgoProcData.sensor_info.AcceInfo[1],mEisAlgoProcData.sensor_info.AcceInfo[2]);
                EIS_LOG("EIS:Gyro(%f,%f,%f)",mEisAlgoProcData.sensor_info.GyroInfo[0],mEisAlgoProcData.sensor_info.GyroInfo[1],mEisAlgoProcData.sensor_info.GyroInfo[2]);
            }

            //====== EIS Algorithm ======

            err = m_pEisAlg->EisFeatureCtrl(EIS_FEATURE_SET_PROC_INFO, &mEisAlgoProcData, NULL);
            if (err != S_EIS_OK)
            {
                EIS_ERR("EisAlg:EIS_FEATURE_SET_PROC_INFO fail(0x%x)",err);
                err = EIS_RETURN_API_FAIL;
                return err;
            }

            err = m_pEisAlg->EisMain(&eisCMVResult);
            if (err != S_EIS_OK)
            {
                EIS_ERR("EisAlg:EisMain fail(0x%x)",err);
                err = EIS_RETURN_API_FAIL;
                return err;
            }

            //====== Get EIS Result to EIS Plus ======

            EIS_GET_PLUS_INFO_STRUCT eisData2EisPlus;

            err = m_pEisAlg->EisFeatureCtrl(EIS_FEATURE_GET_EIS_PLUS_DATA, NULL, &eisData2EisPlus);
            if (err != S_EIS_OK)
            {
                EIS_ERR("EisAlg:EIS_FEATURE_GET_EIS_PLUS_DATA fail(0x%x)",err);
                err = EIS_RETURN_API_FAIL;
                return err;
            }

            {
                Mutex::Autolock lock(mP2Lock);

                if(m_pEisDrv->Get2PixelMode() == 1)
                {
                    if(g_debugDump > 0)
                    {
                        EIS_LOG("eisData2EisPlus.GMVx *= 2");
                    }
                    eisData2EisPlus.GMVx *= 2.0f;
                }else if(m_pEisDrv->Get2PixelMode() == 2)
                {
                    if(g_debugDump > 0)
                    {
                        EIS_LOG("eisData2EisPlus.GMVx *= 4");
                    }
                    eisData2EisPlus.GMVx *= 4.0f;
                }

                //> keep lateset result for EisPlus

                mEisLastData2EisPlus.GMVx  = eisData2EisPlus.GMVx;
                mEisLastData2EisPlus.GMVy  = eisData2EisPlus.GMVy;
                mEisLastData2EisPlus.ConfX = eisData2EisPlus.ConfX;
                mEisLastData2EisPlus.ConfY = eisData2EisPlus.ConfY;

                //> save to queue
                mEis2EisPlusGmvTS.push(aTimeStamp);
                mEis2EisPlusGmvX.push(eisData2EisPlus.GMVx);
                mEis2EisPlusGmvY.push(eisData2EisPlus.GMVy);
                mEis2EisPlusConfX.push(eisData2EisPlus.ConfX);
                mEis2EisPlusConfY.push(eisData2EisPlus.ConfY);
            }

            //====== Get GMV ======

            EIS_GMV_INFO_STRUCT eisGMVResult;

            err = m_pEisAlg->EisFeatureCtrl(EIS_FEATURE_GET_ORI_GMV, NULL, &eisGMVResult);
            if(err != S_EIS_OK)
            {
                EIS_ERR("EisAlg:EIS_FEATURE_GET_ORI_GMV fail(0x%x)",err);
                err = EIS_RETURN_API_FAIL;
                return err;
            }

            //====== Save EIS CMV and GMV =======

            if(m_pEisDrv->Get2PixelMode() == 1)
            {
                if(g_debugDump > 0)
                {
                    EIS_LOG("eisGMVResult.EIS_GMVx *= 2");
                }

                eisGMVResult.EIS_GMVx *= 2.0f;
            }else if(m_pEisDrv->Get2PixelMode() == 2)
            {
                if(g_debugDump > 0)
                {
                    EIS_LOG("eisGMVResult.EIS_GMVx *= 4");
                }

                eisGMVResult.EIS_GMVx *= 4.0f;
            }

            mGMV_X = eisGMVResult.EIS_GMVx;
            mGMV_Y = eisGMVResult.EIS_GMVy;

            //====== Prepare EIS Result ======

            PrepareEisResult(eisCMVResult.CMV_X,eisCMVResult.CMV_Y,eisData2EisPlus.ConfX,eisData2EisPlus.ConfY,aTimeStamp);

            //====== Get First Frame Info ======

            mFrameCnt = m_pEisDrv->GetFirstFrameInfo();

            if (UNLIKELY(g_debugDump >= 1))
            {
                EIS_LOG("mFrameCnt(%u)",mFrameCnt);
            }

            //====== Not The First Frame ======

            if (mFrameCnt == 0)
            {
                EIS_LOG("not first frame");
                mFrameCnt = 1;

                // move to EIS_DRV to handle this
        #if 0
                err = m_pEisDrv->SetFirstFrame(0);
                if(err != EIS_RETURN_NO_ERROR)
                {
                    EIS_ERR("set first frame fail(0x%08x)",err);
                }
                else
                {
                     mFrameCnt = 1;
                }
        #endif
            }

            //====== Dynamic Debug ======

            if (UNLIKELY(g_debugDump >= 1 && mFrameCnt < DEBUG_DUMP_FRAMW_NUM))
            {
                if (UNLIKELY(g_debugDump == 3))
                {
                    m_pEisDrv->DumpReg(EIS_PASS_1);
                }

                if (mEisPass1Only == 1)
                {
                    ++mFrameCnt;
                }
            }

#endif
        }
    }

    mDoEisCount++;    //Vent@20140427: Count how many times DoEis() is run. Add for EIS GMV Sync Check.

    return EIS_RETURN_NO_ERROR;
}

MINT32 EisHalImp::DoEis(const EIS_PASS_ENUM &aEisPass, EIS_HAL_CONFIG_DATA *apEisConfig, MINT64 aTimeStamp, MINT64 aExpTime)
{
    MINT32 err = EIS_RETURN_NO_ERROR;

    if(mEisSupport == MFALSE)
    {
        EIS_LOG("mSensorIdx(%u) not support EIS",mSensorIdx);
        return EIS_RETURN_NO_ERROR;
    }

    if (UNLIKELY(g_debugDump >= 1))
    {
        EIS_LOG("aEisPass(%d),mEisPass1Only(%u)",aEisPass,mEisPass1Only);
    }

    if(aEisPass == EIS_PASS_1)
    {
        EIS_WRN("EIS pass1 use another interface!!!");
    }
    else if (aEisPass == EIS_PASS_2)
    {
        DP_TRACE_CALL();

        MUINT32 i;

        if (mEisPass1Only == 1)
        {
            EIS_WRN("Only use EIS pass1");
            return EIS_RETURN_NO_ERROR;
        }
        else
        {

#if EIS_ALGO_READY
            //====== Check Config Data ======

            if(apEisConfig == NULL)
            {
                EIS_ERR("apEisConfig is NULL");
                err = EIS_RETURN_NULL_OBJ;
                return err;
            }

            //====== Setting EIS Plus Algo Process Data ======

            EIS_GET_PLUS_INFO_STRUCT eisDataForEisPlus;
            EIS_PLUS_SET_PROC_INFO_STRUCT eisPlusProcData;
            memset(&eisDataForEisPlus, 0, sizeof(eisDataForEisPlus));
            memset(&eisPlusProcData, 0, sizeof(eisPlusProcData));

            {
                MUINT64 gmvTS;
                MBOOL    queueNotEmpty = MTRUE;
                Mutex::Autolock lock(mP2Lock);


                //Checking the correct GMV from P1 EIS HW and calculation
                {
                    gmvTS = 0;
                    while (aTimeStamp > gmvTS)
                    {

                        if ( mEis2EisPlusGmvTS.empty() ||
                             mEis2EisPlusGmvX.empty() ||
                             mEis2EisPlusGmvY.empty() ||
                             mEis2EisPlusConfX.empty() ||
                             mEis2EisPlusConfY.empty())
                        {
                            EIS_ERR("EisPlusGmv is empty queue(%d,%d,%d,%d,%d)",mEis2EisPlusGmvTS.size(),
                                                                  mEis2EisPlusGmvX.size(),
                                                       mEis2EisPlusGmvY.size(),
                                                       mEis2EisPlusConfX.size(),
                                                       mEis2EisPlusConfY.size());
                            queueNotEmpty = MFALSE;
                            break;
                        }

                        gmvTS = mEis2EisPlusGmvTS.front();
                        eisDataForEisPlus.GMVx  = mEis2EisPlusGmvX.front();
                        eisDataForEisPlus.GMVy  = mEis2EisPlusGmvY.front();
                        eisDataForEisPlus.ConfX = mEis2EisPlusConfX.front();
                        eisDataForEisPlus.ConfY = mEis2EisPlusConfY.front();

                        if (aTimeStamp >= gmvTS) // Removing the old GMV data in queue.
                        {
                            mEis2EisPlusGmvTS.pop();
                            mEis2EisPlusGmvX.pop();
                            mEis2EisPlusGmvY.pop();
                            mEis2EisPlusConfX.pop();
                            mEis2EisPlusConfY.pop();
                        }

                        if (UNLIKELY(g_debugDump >= 1))
                        {
                            EIS_LOG("pop mEis2EisPlusGmv data");
                        }
                    }
                }

                if ( (aTimeStamp == gmvTS) && queueNotEmpty)
                {
                    if (UNLIKELY(g_debugDump >= 1))
                    {
                        EIS_LOG("eisDataForEisPlus is matched");
                    }
                }else
                {
                    eisDataForEisPlus.GMVx  = 0;
                    eisDataForEisPlus.GMVy  = 0;
                    eisDataForEisPlus.ConfX = 0;
                    eisDataForEisPlus.ConfY = 0;
                    EIS_WRN("eisDataForEisPlus is missed");
                }

            }

            if (UNLIKELY(g_debugDump >= 1))
            {
                EIS_LOG("eisDataForEisPlus.GMVx(%f)",eisDataForEisPlus.GMVx);
                EIS_LOG("eisDataForEisPlus.GMVy(%f)",eisDataForEisPlus.GMVy);
                EIS_LOG("eisDataForEisPlus.ConfX(%d)",eisDataForEisPlus.ConfX);
                EIS_LOG("eisDataForEisPlus.ConfY(%d)",eisDataForEisPlus.ConfY);
            }

            //> Set EisPlusProcData

            eisPlusProcData.eis_info.eis_gmv_conf[0] = eisDataForEisPlus.ConfX;
            eisPlusProcData.eis_info.eis_gmv_conf[1] = eisDataForEisPlus.ConfY;
            eisPlusProcData.eis_info.eis_gmv[0]      = eisDataForEisPlus.GMVx;
            eisPlusProcData.eis_info.eis_gmv[1]      = eisDataForEisPlus.GMVy;

            //> get FE block number

            MUINT32 feBlockNum = 0;

            if(apEisConfig->srzOutW <= D1_WIDTH && apEisConfig->srzOutH <= D1_HEIGHT)
            {
                feBlockNum = 8;
            }
            else if(apEisConfig->srzOutW <= EIS_FE_MAX_INPUT_W && apEisConfig->srzOutH <= EIS_FE_MAX_INPUT_H)
            {
                feBlockNum = 16;
            }
            else
            {
                feBlockNum = 32;
                EIS_WRN("FE should be disabled");
            }

            eisPlusProcData.block_size   = feBlockNum;
            eisPlusProcData.imgiWidth    = apEisConfig->imgiW;
            eisPlusProcData.imgiHeight   = apEisConfig->imgiH;
            eisPlusProcData.CRZoWidth    = apEisConfig->crzOutW;
            eisPlusProcData.CRZoHeight   = apEisConfig->crzOutH;
            eisPlusProcData.SRZoWidth    = apEisConfig->srzOutW;
            eisPlusProcData.SRZoHeight   = apEisConfig->srzOutH;
            eisPlusProcData.oWidth       = apEisConfig->feTargetW;
            eisPlusProcData.oHeight      = apEisConfig->feTargetH;
            eisPlusProcData.TargetWidth  = apEisConfig->gpuTargetW;
            eisPlusProcData.TargetHeight = apEisConfig->gpuTargetH;
            eisPlusProcData.cropX        = apEisConfig->cropX;
            eisPlusProcData.cropY        = apEisConfig->cropY;

            //> config EIS Plus data

            mSrzOutW = apEisConfig->srzOutW;
            mSrzOutH = apEisConfig->srzOutH;

            //> set FE block number to driver

#if USING_FEFM
            m_pEisDrv->SetFeBlockNum(mSrzOutW,mSrzOutH,feBlockNum);
#endif
            if (UNLIKELY(g_debugDump >= 1))
            {
                EIS_LOG("mImgi(%u,%u)",eisPlusProcData.imgiWidth,eisPlusProcData.imgiHeight);
                EIS_LOG("CrzOut(%u,%u)",eisPlusProcData.CRZoWidth,eisPlusProcData.CRZoHeight);
                EIS_LOG("SrzOut(%u,%u)",eisPlusProcData.SRZoWidth,eisPlusProcData.SRZoHeight);
                EIS_LOG("FeTarget(%u,%u)",eisPlusProcData.oWidth,eisPlusProcData.oHeight);
                EIS_LOG("GpuTarget(%u,%u)",eisPlusProcData.TargetWidth,eisPlusProcData.TargetHeight);
                EIS_LOG("mCrop(%u,%u)",eisPlusProcData.cropX,eisPlusProcData.cropY);
                EIS_LOG("feBlockNum(%u)",feBlockNum);
            }

            //> get FEO statistic
#if USING_FEFM
            GetFeoStatistic();

            eisPlusProcData.fe_info.FE_X     = mFeoStatData.feX;
            eisPlusProcData.fe_info.FE_Y     = mFeoStatData.feY;
            eisPlusProcData.fe_info.FE_RES   = mFeoStatData.feRes;
            eisPlusProcData.fe_info.FE_DES   = mFeoStatData.feDes;
            eisPlusProcData.fe_info.FE_VALID = mFeoStatData.feValid;
#endif

            if (UNLIKELY(g_debugDump >= 1))
            {
                EIS_LOG("eisPlusProcData");
                EIS_LOG("eis_gmv_conf[0](%d)",eisPlusProcData.eis_info.eis_gmv_conf[0]);
                EIS_LOG("eis_gmv_conf[1](%d)",eisPlusProcData.eis_info.eis_gmv_conf[1]);
                EIS_LOG("eis_gmv[0](%f)",eisPlusProcData.eis_info.eis_gmv[0]);
                EIS_LOG("eis_gmv[1](%f)",eisPlusProcData.eis_info.eis_gmv[1]);
                EIS_LOG("block_size(%d)",eisPlusProcData.block_size);
                EIS_LOG("imgi(%d,%d)",eisPlusProcData.imgiWidth,eisPlusProcData.imgiHeight);
                EIS_LOG("CRZ(%d,%d)",eisPlusProcData.CRZoWidth,eisPlusProcData.CRZoHeight);
                EIS_LOG("SRZ(%d,%d)",eisPlusProcData.SRZoWidth,eisPlusProcData.SRZoHeight);
                EIS_LOG("target(%d,%d)",eisPlusProcData.TargetWidth,eisPlusProcData.TargetHeight);
                EIS_LOG("crop(%d,%d)",eisPlusProcData.cropX,eisPlusProcData.cropY);
            }

            GYRO_INIT_INFO_STRUCT  gyroAlgoInitData;
            GYRO_SET_PROC_INFO_STRUCT gyroSetProcData;
            memset(&gyroSetProcData,0,sizeof(gyroSetProcData));
            memset(&gyroAlgoInitData, 0, sizeof(gyroAlgoInitData));

            if ( NULL != m_pEisDrv)
            {
                m_pEisDrv->GetRegSetting(&gyroAlgoInitData);
            }

            MUINT64 gyro_t_frame_array[GYRO_DATA_PER_FRAME];
            double gyro_xyz_frame_array[GYRO_DATA_PER_FRAME*3];

            if ((aTimeStamp != 0) && (gGyroCount != 0) )
            {
                MUINT32 waitTime = 0;
                EIS_GyroRecord lastGyro;
                lastGyro.ts = 0;
                const MUINT64 currentTarget = aTimeStamp + (mSleepTime*1000L) + (mtRSTime*1000L);
                do
                {
                    gGyroQueueLock.lock();
                    mSkipWaitGyro = MFALSE;
                    if (!gGyroDataQueue.empty())
                    {
                         lastGyro = gGyroDataQueue.back();
                        if( lastGyro.ts <= currentTarget)
                        {
                            //if (UNLIKELY(g_debugDump >= 1))
                            {
                                EIS_LOG("video (%lld) > gyro.ts (%lld) => wait",currentTarget,lastGyro.ts);
                            }
                            gWaitGyroCond.wait(gGyroQueueLock);
                        }else
                        {
                            if (UNLIKELY(g_debugDump >= 1))
                            {
                                EIS_LOG("video (%lld) < gyro.ts (%lld) => go GIS",currentTarget,lastGyro.ts);
                            }
                        }
                    }else
                    {
                        //if (UNLIKELY(g_debugDump >= 1))
                        {
                            EIS_LOG("video (%lld) > gyro.ts (%lld) => wait",currentTarget,lastGyro.ts);
                        }
                        gWaitGyroCond.wait(gGyroQueueLock);
                    }

                    if (gLastGyroTimestamp >= currentTarget)
                    {
                        EIS_LOG("video (%lld) < global Gyro timestamp(%lld) => go GIS ", currentTarget, gLastGyroTimestamp);
                        gGyroQueueLock.unlock();
                        break;
                    }

                    if (mSkipWaitGyro == MTRUE)
                    {
                        EIS_LOG("skip wait Gyro: %d by next video trigger",waitTime);
                        gGyroQueueLock.unlock();
                        break;
                    }

                    gGyroQueueLock.unlock();
                    waitTime++;

                    //if (UNLIKELY(g_debugDump >= 1))
                    {
                        if (UNLIKELY(waitTime > 1))
                        {
                            EIS_LOG("wait Gyro time: %d",waitTime);
                        }
                    }
                }while( lastGyro.ts < currentTarget);
            }

#if RECORD_WITHOUT_EIS_ENABLE
            if (g_AIMDump == 1)
            {
#if 0
                if (UNLIKELY(g_debugDump >= 1))
                {
                    EIS_LOG("vHDR LE(%d), SE(%d)",OutputParam.u4videoHDRLE_us, OutputParam.u4videoHDRSE_us);
                }

                if(gvHDRRecordWriteID >= TSRECORD_MAXSIZE)
                {
                        gvHDRRecordWriteID = 0;
                }
                gvHDRRecord[gvHDRRecordWriteID].id = 5;
                gvHDRRecord[gvHDRRecordWriteID].ts = ((MUINT64)OutputParam.u4videoHDRLE_us<<32);
                gvHDRRecord[gvHDRRecordWriteID].ts += (MUINT64)OutputParam.u4videoHDRSE_us;
                EIS_LOG("gTSRecord[gvHDRRecordWriteID].ts(%d)",gvHDRRecord[gvHDRRecordWriteID].ts);
                gvHDRRecordWriteID++;
#endif
                if(gTSRecordWriteID >= TSRECORD_MAXSIZE)
                {
                    gTSRecordWriteID = 0;
                }
                gTSRecord[gTSRecordWriteID].id = 3;
                gTSRecord[gTSRecordWriteID].ts = aExpTime;
                gTSRecordWriteID++;

                if ( gGyroRecordWriteID > 1)
                {
                    if(gTSRecordWriteID >= TSRECORD_MAXSIZE)
                    {
                        gTSRecordWriteID = 0;
                    }
                    gTSRecord[gTSRecordWriteID].id = 2;
                    gTSRecord[gTSRecordWriteID].ts = 0; //No finding gyro timestamp

                    for (int i=gGyroRecordWriteID-1;i>0;i--)
                    {
                        if (gGyroRecord[i].id == EIS_GYROSCOPE)
                        {
                            gTSRecord[gTSRecordWriteID].ts = gGyroRecord[i].ts;
                            break;
                        }
                    }
                    gTSRecordWriteID++;
                }else
                {
                    if(gTSRecordWriteID >= TSRECORD_MAXSIZE)
                    {
                        gTSRecordWriteID = 0;
                    }

                    gTSRecord[gTSRecordWriteID].id = 2;
                    gTSRecord[gTSRecordWriteID].ts = 0; //No gyro data exist
                    gTSRecordWriteID++;
                }

                if(gTSRecordWriteID >= TSRECORD_MAXSIZE)
                {
                    gTSRecordWriteID = 0;
                }
                gTSRecord[gTSRecordWriteID].id = 1;
                gTSRecord[gTSRecordWriteID].ts = aTimeStamp;
                gTSRecordWriteID++;

            }
#endif




            gGyroQueueLock.lock();
            while (!gGyroDataQueue.empty())
            {
                EIS_GyroRecord tmp = gGyroDataQueue.front();

                if (gyroSetProcData.gyro_num >= GYRO_DATA_PER_FRAME)
                {
                    gyroSetProcData.gyro_num = 0;
                }

                gyro_t_frame_array[gyroSetProcData.gyro_num] = tmp.ts;
                gyro_xyz_frame_array[3*(gyroSetProcData.gyro_num) + 0] = tmp.x;
                gyro_xyz_frame_array[3*(gyroSetProcData.gyro_num) + 1] = tmp.y;
                gyro_xyz_frame_array[3*(gyroSetProcData.gyro_num) + 2] = tmp.z;

                if (UNLIKELY(g_debugDump >= 1))
                {
                    EIS_LOG("Gyro(%f,%f,%f,%lld)",tmp.x, tmp.y, tmp.z, tmp.ts);
                }

                gGyroDataQueue.pop();
                gyroSetProcData.gyro_num++;
            }
            gGyroQueueLock.unlock();

            if (UNLIKELY(g_debugDump >= 1))
            {
                EIS_LOG("Gyro data num: %d, video ts: %lld", gyroSetProcData.gyro_num, aTimeStamp);
            }

            gyroSetProcData.frame_t = aTimeStamp;
            gyroSetProcData.frame_AE = aExpTime;

            gyroSetProcData.gyro_t_frame = gyro_t_frame_array;
            gyroSetProcData.gyro_xyz_frame = gyro_xyz_frame_array;

            gyroSetProcData.rrz_crop_X = gyroAlgoInitData.rrz_crop_X;
            gyroSetProcData.rrz_crop_Y = gyroAlgoInitData.rrz_crop_Y;

            gyroSetProcData.rrz_crop_Width = gyroAlgoInitData.rrz_crop_Width;
            gyroSetProcData.rrz_crop_Height = gyroAlgoInitData.rrz_crop_Height;

            gyroSetProcData.rrz_scale_Width = gyroAlgoInitData.rrz_scale_Width;
            gyroSetProcData.rrz_scale_Height = gyroAlgoInitData.rrz_scale_Height;

            gyroSetProcData.crz_crop_X = apEisConfig->cropX;
            gyroSetProcData.crz_crop_Y = apEisConfig->cropY;

            gyroSetProcData.crz_crop_Width = apEisConfig->crzOutW;
            gyroSetProcData.crz_crop_Height = apEisConfig->crzOutH;

            if (UNLIKELY(g_debugDump >= 1))
            {
                EIS_LOG("crz offset x(%d),y(%d)", gyroSetProcData.crz_crop_X, gyroSetProcData.crz_crop_Y);
                EIS_LOG("crzOut w(%d),h(%d)", gyroSetProcData.crz_crop_Width, gyroSetProcData.crz_crop_Height);
            }
            //====== GIS Algorithm ======

            err = m_pGisAlg->GyroFeatureCtrl(GYRO_FEATURE_SET_PROC_INFO,&gyroSetProcData, NULL);
            if(err != S_GYRO_OK)
            {
                    EIS_ERR("GIS:GYRO_FEATURE_SET_PROC_INFO fail(0x%x)",err);
                    err = EIS_RETURN_API_FAIL;
                    return err;
            }

            err = m_pGisAlg->GyroMain();
            if(err != S_GYRO_OK)
            {
                    EIS_ERR("GIS:GyroMain fail(0x%x)",err);
                    err = EIS_RETURN_API_FAIL;
                    return err;
            }


            GYRO_MV_RESULT_INFO_STRUCT gyroMVresult;
            err = m_pGisAlg->GyroFeatureCtrl(GYRO_FEATURE_GET_MV_RESULT_INFO, NULL, &gyroMVresult);
            if(err != S_GYRO_OK)
            {
                EIS_ERR("GIS:GYRO_FEATURE_SET_PROC_INFO fail(0x%x)",err);
                err = EIS_RETURN_API_FAIL;
                return err;
            }

            for(i = 0; i < 3; i++)
            {
                eisPlusProcData.sensor_info.AcceInfo[i] = gAccInfo[i];
                eisPlusProcData.sensor_info.GyroInfo[i] = gGyroInfo[i];
            }

            eisPlusProcData.sensor_info.gyro_in_mv = gyroMVresult.mv;
            eisPlusProcData.sensor_info.valid_gyro_num  = gyroMVresult.valid_gyro_num;

            if (UNLIKELY(g_debugDump >= 1))
            {
                EIS_LOG("EN:(Acc,Gyro)=(%d,%d)",mAccEnable,mGyroEnable);
                EIS_LOG("EISPlus:Acc(%f,%f,%f)",eisPlusProcData.sensor_info.AcceInfo[0],eisPlusProcData.sensor_info.AcceInfo[1],eisPlusProcData.sensor_info.AcceInfo[2]);
                EIS_LOG("EISPlus:Gyro(%f,%f,%f)",eisPlusProcData.sensor_info.GyroInfo[0],eisPlusProcData.sensor_info.GyroInfo[1],eisPlusProcData.sensor_info.GyroInfo[2]);
            }

            //====== EIS Plus Algorithm ======

            err = m_pEisPlusAlg->EisPlusFeatureCtrl(EIS_PLUS_FEATURE_SET_PROC_INFO,&eisPlusProcData, NULL);
            if(err != S_EIS_PLUS_OK)
            {
                EIS_ERR("EisPlus:EIS_PLUS_FEATURE_SET_PROC_INFO fail(0x%x)",err);
                err = EIS_RETURN_API_FAIL;
                return err;
            }

            err = m_pEisPlusAlg->EisPlusMain(&mEisPlusResult);
            if(err != S_EIS_PLUS_OK)
            {
                EIS_ERR("EisPlus:EisMain fail(0x%x)",err);
                err = EIS_RETURN_API_FAIL;
                return err;
            }

            //if (UNLIKELY(g_debugDump >= 1))
            {
                EIS_LOG("EisPlusMain- X: %d  Y: %d\n", mEisPlusResult.ClipX, mEisPlusResult.ClipY);
            }

            //====== Dynamic Debug ======
#if RECORD_WITHOUT_EIS_ENABLE
            if (g_AIMDump == 1)
            {
                mEisPlusResult.GridX[0] = 0;
                mEisPlusResult.GridX[1] = (apEisConfig->gpuTargetW-1)*16;
                mEisPlusResult.GridX[2] = 0;
                mEisPlusResult.GridX[3] = (apEisConfig->gpuTargetW-1)*16;

                mEisPlusResult.GridY[0] = 0;
                mEisPlusResult.GridY[1] = 0;
                mEisPlusResult.GridY[2] = (apEisConfig->gpuTargetH-1)*16;
                mEisPlusResult.GridY[3] = (apEisConfig->gpuTargetH-1)*16;
            }
#endif


            if (UNLIKELY(g_debugDump >= 1))
            {
                EIS_INF("EIS GPU WARP MAP");
                for(MUINT32  i = 0; i < mGpuGridW*mGpuGridH; ++i)
                {
                    EIS_LOG("X[%u]=%d",i,mEisPlusResult.GridX[i]);
                    EIS_LOG("Y[%u]=%d",i,mEisPlusResult.GridY[i]);
                }
            }

            if(g_debugDump >= 1 && mFrameCnt < DEBUG_DUMP_FRAMW_NUM)
            {
                if(g_debugDump == 3)
                {
                    m_pEisDrv->DumpReg(EIS_PASS_2);
                }
                ++mFrameCnt;
            }

 #endif
        }

    }

    if(g_debugDump >= 1)
    {
        EIS_LOG("-");
    }


    return EIS_RETURN_NO_ERROR;
}

MINT32 EisHalImp::DoCalibration(const EIS_PASS_ENUM &aEisPass,EIS_HAL_CONFIG_DATA *apEisConfig,MINT64 aTimeStamp, MINT64 aExpTime)
{
    MINT32 err = EIS_RETURN_NO_ERROR;
    MUINT32 i;
    MUINT32 aWidth = 0,aHeight = 0;

    if (mEisSupport == MFALSE)
    {
        EIS_LOG("mSensorIdx(%u) not support EIS",mSensorIdx);
        return EIS_RETURN_NO_ERROR;
    }

    if (UNLIKELY(g_debugDump >= 1))
    {
        EIS_LOG("aEisPass(%d),mEisPass1Only(%u)",aEisPass,mEisPass1Only);
    }

    if (aEisPass == EIS_PASS_2)
    {

        if(mEisPass1Only == 1)
        {
            EIS_WRN("Only use EIS pass1");
            return EIS_RETURN_NO_ERROR;
        }
        else
        {

#if RECORD_WITHOUT_EIS_ENABLE
            if (g_AIMDump == 1)
            {

#if 0
                EIS_LOG("vHDR LE(%d), SE(%d)",OutputParam.u4videoHDRLE_us, OutputParam.u4videoHDRSE_us);
                if(gvHDRRecordWriteID >= TSRECORD_MAXSIZE)
                {
                        gvHDRRecordWriteID = 0;
                }
                gvHDRRecord[gvHDRRecordWriteID].id = 5;
                gvHDRRecord[gvHDRRecordWriteID].ts = ((MUINT64)OutputParam.u4videoHDRLE_us<<32);
                gvHDRRecord[gvHDRRecordWriteID].ts += (MUINT64)OutputParam.u4videoHDRSE_us;
                EIS_LOG("gTSRecord[gvHDRRecordWriteID].ts(%d)",gvHDRRecord[gvHDRRecordWriteID].ts);
                gvHDRRecordWriteID++;
#endif
                if(gTSRecordWriteID >= TSRECORD_MAXSIZE)
                {
                        gTSRecordWriteID = 0;
                }
                gTSRecord[gTSRecordWriteID].id = 3;
                gTSRecord[gTSRecordWriteID].ts = aExpTime;
                gTSRecordWriteID++;

                if ( gGyroRecordWriteID > 1)
                {
                        if(gTSRecordWriteID >= TSRECORD_MAXSIZE)
                        {
                            gTSRecordWriteID = 0;
                        }
                        gTSRecord[gTSRecordWriteID].id = 2;
                        gTSRecord[gTSRecordWriteID].ts = 0; //No finding gyro timestamp

                        for (int i=gGyroRecordWriteID-1;i>0;i--)
                        {
                            if (gGyroRecord[i].id == EIS_GYROSCOPE)
                            {
                                gTSRecord[gTSRecordWriteID].ts = gGyroRecord[i].ts;
                                break;
                            }
                        }
                        gTSRecordWriteID++;
                }else
                {
                    if(gTSRecordWriteID >= TSRECORD_MAXSIZE)
                    {
                            gTSRecordWriteID = 0;
                    }

                    gTSRecord[gTSRecordWriteID].id = 2;
                    gTSRecord[gTSRecordWriteID].ts = 0; //No gyro data exist
                    gTSRecordWriteID++;
                }

                if(gTSRecordWriteID >= TSRECORD_MAXSIZE)
                {
                        gTSRecordWriteID = 0;
                }
                gTSRecord[gTSRecordWriteID].id = 1;
                gTSRecord[gTSRecordWriteID].ts = aTimeStamp;
                gTSRecordWriteID++;

            }
#endif

            //====== Check Config Data ======

            if(apEisConfig == NULL)
            {
                EIS_ERR("apEisConfig is NULL");
                err = EIS_RETURN_NULL_OBJ;
                return err;
            }

            GYRO_INIT_INFO_STRUCT  gyroAlgoInitData;
            memset(&gyroAlgoInitData, 0, sizeof(gyroAlgoInitData));

            if (NULL != m_pEisDrv)
            {
                m_pEisDrv->GetEisInputSize(&aWidth, &aHeight);
                m_pEisDrv->GetRegSetting(&gyroAlgoInitData);

            }else
            {
                EIS_ERR("m_pEisDrv is NULL");
            }

            if ((aWidth != mGisInputW) || (aHeight != mGisInputH))
            {
                mChangedInCalibration++;
            }


            GYRO_SET_PROC_INFO_STRUCT gyroSetProcData;
            memset(&gyroSetProcData,0,sizeof(gyroSetProcData));

            gyroSetProcData.frame_t= aTimeStamp;
            gyroSetProcData.frame_AE = (MINT32)(aExpTime / 1000);

            gyroSetProcData.rrz_crop_X = gyroAlgoInitData.rrz_crop_X;
            gyroSetProcData.rrz_crop_Y = gyroAlgoInitData.rrz_crop_Y;

            gyroSetProcData.rrz_crop_Width = gyroAlgoInitData.rrz_crop_Width;
            gyroSetProcData.rrz_crop_Height = gyroAlgoInitData.rrz_crop_Height;

            gyroSetProcData.rrz_scale_Width = gyroAlgoInitData.rrz_scale_Width;
            gyroSetProcData.rrz_scale_Height = gyroAlgoInitData.rrz_scale_Height;

            //CRZ relative information is deprecated later!!!
            gyroSetProcData.crz_crop_X = apEisConfig->cropX;
            gyroSetProcData.crz_crop_Y = apEisConfig->cropY;

            gyroSetProcData.crz_crop_Width = apEisConfig->crzOutW;
            gyroSetProcData.crz_crop_Height = apEisConfig->crzOutH;

            //EIS_LOG("crz offset x(%d),y(%d)", gyroSetProcData.crz_crop_X, gyroSetProcData.crz_crop_Y);
            //EIS_LOG("crzOut w(%d),h(%d)", gyroSetProcData.crz_crop_Width, gyroSetProcData.crz_crop_Height);

            EIS_LOG("frame_AE(%d), rrz_crop_X(%d), rrz_crop_Y(%d), rrz_crop_Width(%d), rrz_crop_Height(%d), rrz_out_Width(%d), rrz_out_Height(%d)",
                     gyroSetProcData.frame_AE, gyroSetProcData.rrz_crop_X, gyroSetProcData.rrz_crop_Y,
                     gyroSetProcData.rrz_crop_Width, gyroSetProcData.rrz_crop_Height,
                     gyroSetProcData.rrz_scale_Width, gyroSetProcData.rrz_scale_Height);


            MBOOL bDoCalibration = MTRUE;
            MUINT64 gyro_t_frame_array[GYRO_DATA_PER_FRAME];
            double gyro_xyz_frame_array[GYRO_DATA_PER_FRAME*3];
            GyroEISStatistics currentEisResult;

            gGyroQueueLock.lock();
            while (!gGyroDataQueue.empty())
            {
                EIS_GyroRecord tmp = gGyroDataQueue.front();

                if (gyroSetProcData.gyro_num >= GYRO_DATA_PER_FRAME)
                {
                    gyroSetProcData.gyro_num = 0;
                }

                gyro_t_frame_array[gyroSetProcData.gyro_num] = tmp.ts;
                gyro_xyz_frame_array[3*(gyroSetProcData.gyro_num) + 0] = tmp.x;
                gyro_xyz_frame_array[3*(gyroSetProcData.gyro_num) + 1] = tmp.y;
                gyro_xyz_frame_array[3*(gyroSetProcData.gyro_num) + 2] = tmp.z;

                if (UNLIKELY(g_debugDump >= 1))
                {
                    EIS_LOG("Gyro(%f,%f,%f,%lld)",tmp.x, tmp.y, tmp.z, tmp.ts);
                }

                gGyroDataQueue.pop();
                gyroSetProcData.gyro_num++;
            }
            gGyroQueueLock.unlock();

            if (UNLIKELY(g_debugDump >= 1))
            {
                EIS_LOG("Gyro data num: %d, video ts: %lld", gyroSetProcData.gyro_num, aTimeStamp);
            }

            gyroSetProcData.gyro_t_frame = gyro_t_frame_array;
            gyroSetProcData.gyro_xyz_frame = gyro_xyz_frame_array;

            GisEisStatisticsQLock.lock();
            while (!mGisEisStatisticsQ.empty())
            {
                currentEisResult = mGisEisStatisticsQ.front();
                if (currentEisResult.ts < aTimeStamp)
                {
                    EIS_LOG("video ts: %lld > eis statistics is %lld, drop eis statistics", aTimeStamp, currentEisResult.ts);
                    mGisEisStatisticsQ.pop();
                }else if (currentEisResult.ts == aTimeStamp)
                {
                    if (UNLIKELY(g_debugDump >= 1))
                    {
                        EIS_LOG("eis statistics matched!!!");
                    }
                    mGisEisStatisticsQ.pop();
                    break;
                }else
                {
                    EIS_LOG("No EIS statistics exist! video ts: %lld",aTimeStamp);
                    bDoCalibration = MFALSE;
                    break;
                }
            }
            GisEisStatisticsQLock.unlock();

#if EIS_ALGO_READY

            //====== GIS Algorithm ======
            if (bDoCalibration)
            {
                gyroSetProcData.val_LMV = mbLastCalibration;
                mbLastCalibration = MTRUE;
                gyroSetProcData.EIS_LMV = currentEisResult.eis_data;
                err = m_pGisAlg->GyroFeatureCtrl(GYRO_FEATURE_SET_PROC_INFO,&gyroSetProcData, NULL);
                if(err != S_GYRO_OK)
                {
                    EIS_ERR("GIS:GYRO_FEATURE_SET_PROC_INFO fail(0x%x)",err);
                    err = EIS_RETURN_API_FAIL;
                    return err;
                }

                err = m_pGisAlg->GyroMain();
                if(err != S_GYRO_OK)
                {
                    EIS_ERR("GIS:GyroMain fail(0x%x)",err);
                    err = EIS_RETURN_API_FAIL;
                    return err;
                }
            }else
            {
                mbLastCalibration = MFALSE;
                EIS_LOG("Bypass calibration");
            }

            GYRO_CAL_RESULT_INFO_STRUCT gyroCal;
            memset(&gyroCal,0,sizeof(gyroCal));
            err = m_pGisAlg->GyroFeatureCtrl(GYRO_FEATURE_GET_CAL_RESULT_INFO, NULL,&gyroCal);
            if(err != S_GYRO_OK)
            {
                EIS_ERR("GIS:GYRO_FEATURE_SET_PROC_INFO fail(0x%x)",err);
                err = EIS_RETURN_API_FAIL;
                return err;
            }

            {
                EIS_LOG("Cal:frame(%d) valid (%d) , %f    %f    %f    %f    %f    %f", mDoEisCount, gyroCal.dataValid, gyroCal.paramFinal[0], gyroCal.paramFinal[1], gyroCal.paramFinal[2],
                                                                                                               gyroCal.paramFinal[3], gyroCal.paramFinal[4], gyroCal.paramFinal[5] );
            }

            if (mChangedInCalibration > 0)
            {
                gyroCal.dataValid = 3; //Means size is changed in the calibration
            }

            if (mChangedInCalibration == 1)
            {
                EIS_LOG("Cal:frame(%d), Initial W/H (%d/%d) , Current W/H    (%d/%d)", mDoEisCount, mGisInputW, mGisInputH, aWidth, aHeight);
            }

            if (gyroCal.dataValid == 0) //Calibration pass and write to the nvram
            {
                m_pNVRAM_defParameter->gis_defParameter3[0] = gyroCal.paramFinal[0];
                m_pNVRAM_defParameter->gis_defParameter3[1] = gyroCal.paramFinal[1];
                m_pNVRAM_defParameter->gis_defParameter3[2] = gyroCal.paramFinal[2];
                m_pNVRAM_defParameter->gis_defParameter3[3] = gyroCal.paramFinal[3];
                m_pNVRAM_defParameter->gis_defParameter3[4] = gyroCal.paramFinal[4];
                m_pNVRAM_defParameter->gis_defParameter3[5] = gyroCal.paramFinal[5];

                mRecordParameter[0] = gyroCal.paramFinal[0];
                mRecordParameter[1] = gyroCal.paramFinal[1];
                mRecordParameter[2] = gyroCal.paramFinal[2];
                mRecordParameter[3] = gyroCal.paramFinal[3];
                mRecordParameter[4] = gyroCal.paramFinal[4];
                mRecordParameter[5] = gyroCal.paramFinal[5];

            }else if ( (gyroCal.dataValid == 1) || (gyroCal.dataValid == 2) || (gyroCal.dataValid == 4)) //Calibration failed and only used in recording
            {
                mRecordParameter[0] = gyroCal.paramFinal[0];
                mRecordParameter[1] = gyroCal.paramFinal[1];
                mRecordParameter[2] = gyroCal.paramFinal[2];
                mRecordParameter[3] = gyroCal.paramFinal[3];
                mRecordParameter[4] = gyroCal.paramFinal[4];
                mRecordParameter[5] = gyroCal.paramFinal[5];
            }else
            {
                mRecordParameter[0] = m_pNVRAM_defParameter->gis_defParameter3[0];
                mRecordParameter[1] = m_pNVRAM_defParameter->gis_defParameter3[1];
                mRecordParameter[2] = m_pNVRAM_defParameter->gis_defParameter3[2];
                mRecordParameter[3] = m_pNVRAM_defParameter->gis_defParameter3[3];
                mRecordParameter[4] = m_pNVRAM_defParameter->gis_defParameter3[4];
                mRecordParameter[5] = m_pNVRAM_defParameter->gis_defParameter3[5];
            }

            if (UNLIKELY(g_EMEnabled == 1))
            {
                if ((gyroCal.dataValid == 0) || (gyroCal.dataValid == 2) || (gyroCal.dataValid == 5))
                {
                    if (mbEMSaveFlag == MFALSE)
                    {
                        err = m_pGisAlg->GyroFeatureCtrl(GYRO_FEATURE_SAVE_EM_INFO, (MVOID *)&mTsForAlgoDebug, NULL);
                        if(err != S_GYRO_OK)
                        {
                            EIS_ERR("EisPlusFeatureCtrl(GYRO_FEATURE_SAVE_EM_INFO) fail(0x%x)",err);
                        }
                        mbEMSaveFlag = MTRUE;
                    }
                }
            }

#endif

        }
    }

    if (UNLIKELY(g_debugDump >= 1))
    {
        EIS_LOG("-");
    }

    return EIS_RETURN_NO_ERROR;
}



/*******************************************************************************
*
********************************************************************************/
MVOID EisHalImp::SetEisPlusGpuInfo(MINT32 * const aGridX, MINT32 * const aGridY)
{
    mEisPlusResult.GridX = aGridX;
    mEisPlusResult.GridY = aGridY;

    if(g_debugDump >= 1)
    {
        EIS_LOG("[IN]grid VA(0x%08x,0x%08x)",aGridX,aGridY);
        EIS_LOG("[MEMBER]grid VA(0x%08x,0x%08x)",mEisPlusResult.GridX,mEisPlusResult.GridY);
    }
}

/*******************************************************************************
*
********************************************************************************/
MVOID EisHalImp::SetEisP2User(char const *userName)
{
    EIS_LOG("%s(%d)",userName,++mEisP2UserCnt);
}

/*******************************************************************************
*
********************************************************************************/
MVOID EisHalImp::PrepareEisResult(const MINT32 &cmvX, const MINT32 &cmvY,const MINT32 &aGmvConfidX,const MINT32 &aGmvConfidY,const MINT64 &aTimeStamp)
{
    if (UNLIKELY(g_debugDump >= 1))
    {
        EIS_LOG("cmvX(%d),cmvY(%d)",cmvX,cmvY);
    }

    Mutex::Autolock lock(mP1Lock);

    //====== Boundary Checking ======

    if(cmvX < 0)
    {
        EIS_ERR("cmvX should not be negative(%u), fix to 0",cmvX);

        mCmvX_Int = mCmvX_Flt = 0;
    }
    else
    {
        MFLOAT tempCMV_X = cmvX / 256.0;
        MINT32 tempFinalCmvX = cmvX;
        mMVtoCenterX = cmvX;

        if((tempCMV_X + (MFLOAT)mP1Target_W) > (MFLOAT)mEisInput_W)
        {
            EIS_LOG("cmvX too large(%u), fix to %u",cmvX,(mEisInput_W - mP1Target_W));

            tempFinalCmvX = (mEisInput_W - mP1Target_W);
        }

        mMVtoCenterX -=  ((mEisInput_W-mP1Target_W)<<(intPartShift-1)); //Make mv for the top-left of center

        if(m_pEisDrv->Get2PixelMode() == 1)
        {
            if(g_debugDump > 0)
            {
                EIS_LOG("tempFinalCmvX *= 2");
            }

            tempFinalCmvX *= 2;
            mMVtoCenterX *= 2;
        }else if(m_pEisDrv->Get2PixelMode() == 2)
        {
            if(g_debugDump > 0)
            {
                EIS_LOG("tempFinalCmvX *= 4");
            }

            tempFinalCmvX *= 4;
            mMVtoCenterX *= 4;
        }

        mCmvX_Int = (tempFinalCmvX & (~0xFF)) >> intPartShift;
        mCmvX_Flt = (tempFinalCmvX & (0xFF)) << floatPartShift;
    }

    if(cmvY < 0)
    {
        EIS_ERR("cmvY should not be negative(%u), fix to 0",cmvY);

        mCmvY_Int = mCmvY_Flt = 0;
    }
    else
    {
        MFLOAT tempCMV_Y = cmvY / 256.0;
        MINT32 tempFinalCmvY = cmvY;
        mMVtoCenterY = cmvY;

        if((tempCMV_Y + (MFLOAT)mP1Target_H) > (MFLOAT)mEisInput_H)
        {
            EIS_LOG("cmvY too large(%u), fix to %u",cmvY,(mEisInput_H - mP1Target_H));

            tempFinalCmvY = (mEisInput_H - mP1Target_H);
        }
        mMVtoCenterY -=  ((mEisInput_H-mP1Target_H)<<(intPartShift-1)); //Make mv for the top-left of center

        mCmvY_Int = (tempFinalCmvY & (~0xFF)) >> intPartShift;
        mCmvY_Flt = (tempFinalCmvY & (0xFF)) << floatPartShift;
    }

    EIS_LOG("X(%u,%u),Y(%u,%u)",mCmvX_Int,mCmvX_Flt,mCmvY_Int,mCmvY_Flt);
    EIS_LOG("MVtoCenter (%d,%d)",mMVtoCenterX, mMVtoCenterY);

    //====== Save for Pass2 User if Needed ======

    if(mEisP2UserCnt != 0)
    {
        SaveEisResultForP2(aGmvConfidX,aGmvConfidY,aTimeStamp);
    }
}

/*******************************************************************************
*
********************************************************************************/
MVOID EisHalImp::SaveEisResultForP2(const MINT32 &aGmvConfidX,const MINT32 &aGmvConfidY,const MINT64 &aTimeStamp)
{
    Mutex::Autolock lock(mLock);

    EIS_P1_RESULT_INFO eisP1Result;

    eisP1Result.DoEisCount = mDoEisCount;    //Vent@20140427: Add for EIS GMV Sync Check.

    eisP1Result.cmvX_Int   = mCmvX_Int;
    eisP1Result.cmvX_Flt   = mCmvX_Flt;
    eisP1Result.cmvY_Int   = mCmvY_Int;
    eisP1Result.cmvY_Flt   = mCmvY_Flt;
    eisP1Result.gmvX       = mGMV_X;
    eisP1Result.gmvY       = mGMV_Y;
    eisP1Result.gmvConfidX = aGmvConfidX;
    eisP1Result.gmvConfidY = aGmvConfidY;
    eisP1Result.timeStamp  = aTimeStamp;

    if(g_debugDump > 0)
    {
        EIS_LOG("Gmv(%d,%d),gmvConfid(%d,%d),Int(%d,%d),Flt(%d,%d),Count(%d),TS(%lld)",eisP1Result.gmvX,
                                                                                       eisP1Result.gmvY,
                                                                                       eisP1Result.gmvConfidX,
                                                                                       eisP1Result.gmvConfidY,
                                                                                       eisP1Result.cmvX_Int,
                                                                                       eisP1Result.cmvY_Int,
                                                                                       eisP1Result.cmvX_Flt,
                                                                                       eisP1Result.cmvY_Flt,
                                                                                       eisP1Result.DoEisCount,
                                                                                       eisP1Result.timeStamp);
    }

    mEisResultForP2.push(eisP1Result);

    if(mEisResultForP2.size() > 30)
    {
        EIS_LOG("too much unused data");
        while(mEisResultForP2.size() > 30)
        {
            mEisResultForP2.pop();
        }
    }
}

/*******************************************************************************
*
********************************************************************************/
MVOID EisHalImp::GetEisResult(MUINT32 &a_CMV_X_Int,
                                 MUINT32 &a_CMV_X_Flt,
                                 MUINT32 &a_CMV_Y_Int,
                                 MUINT32 &a_CMV_Y_Flt,
                                 MUINT32 &a_TarWidth,
                                 MUINT32 &a_TarHeight,
                                 MINT32  &a_MVtoCenterX,
                                 MINT32  &a_MVtoCenterY,
                                 MUINT32 &a_isFromRRZ)
{
    if(mEisSupport == MFALSE)
    {
        EIS_LOG("mSensorIdx(%u) not support EIS",mSensorIdx);
        a_CMV_X_Int = 0;
        a_CMV_X_Flt = 0;
        a_CMV_Y_Int = 0;
        a_CMV_Y_Flt = 0;
        a_TarWidth  = 0;
        a_TarHeight = 0;
        a_MVtoCenterX = 0;
        a_MVtoCenterY = 0;
        a_isFromRRZ = 0;
        return;
    }

    {
        Mutex::Autolock lock(mP1Lock);

        a_CMV_X_Int = mCmvX_Int;
        a_CMV_X_Flt = mCmvX_Flt;
        a_CMV_Y_Int = mCmvY_Int;
        a_CMV_Y_Flt = mCmvY_Flt;
        a_TarWidth  = mP1Target_W;
        a_TarHeight = mP1Target_H;
        a_MVtoCenterX = mMVtoCenterX;
        a_MVtoCenterY = mMVtoCenterY;
        a_isFromRRZ = 1; //Hardcode MUST be fix later!!!!
    }

    if(g_debugDump >= 1)
    {
        EIS_LOG("X(%u,%u),Y(%u,%u)",a_CMV_X_Int,a_CMV_X_Flt,a_CMV_Y_Int,a_CMV_Y_Flt);
    }
}

/*******************************************************************************
*
********************************************************************************/
MVOID EisHalImp::GetEisPlusResult(P_EIS_PLUS_RESULT_INFO_STRUCT apEisPlusResult)
{
    if(mEisSupport == MFALSE)
    {
        EIS_LOG("mSensorIdx(%u) not support EIS",mSensorIdx);
        return;
    }

    apEisPlusResult->ClipX = mEisPlusResult.ClipX;
    apEisPlusResult->ClipY = mEisPlusResult.ClipY;

    if(g_debugDump >= 1)
    {
        EIS_LOG("Clip(%u,%u)",apEisPlusResult->ClipX,apEisPlusResult->ClipY);
    }
}


/*******************************************************************************
*
********************************************************************************/
MVOID EisHalImp::GetEisGmv(MINT32 &aGMV_X, MINT32 &aGMV_Y, MUINT32 *confX, MUINT32 *confY)
{
    if(mEisSupport == MFALSE)
    {
        EIS_LOG("mSensorIdx(%u) not support EIS",mSensorIdx);
        return;
    }

    aGMV_X = mGMV_X;
    aGMV_Y = mGMV_Y;

    {
        Mutex::Autolock lock(mP1Lock);

        if(confX != NULL)
        {
            *confX = mEisLastData2EisPlus.ConfX;
            EIS_LOG("ConfX(%d)",*confX);
        }

        if(confY != NULL)
        {
            *confY = mEisLastData2EisPlus.ConfY;
            EIS_LOG("ConfY(%d)",*confY);
        }
    }

    if (UNLIKELY(g_debugDump >= 1))
    {
        if (confX && confY)
        {
            EIS_LOG("GMV(%d,%d),Conf(%d,%d)", aGMV_X, aGMV_Y, *confX, *confY);
        }else
        {
            EIS_LOG("GMV(%d,%d)", aGMV_X, aGMV_Y);
        }
    }
}

/*******************************************************************************
*
********************************************************************************/
MVOID EisHalImp::GetFeoRegInfo(FEO_REG_INFO *aFeoRegInfo)
{
    if(mEisSupport == MFALSE)
    {
        EIS_LOG("mSensorIdx(%u) not support EIS",mSensorIdx);
        return;
    }

    FEO_COFIG_DATA feoRegInfo;

    m_pEisDrv->GetFeoRegInfo(&feoRegInfo);

    aFeoRegInfo->xSize       = feoRegInfo.xSize;
    aFeoRegInfo->ySize       = feoRegInfo.ySize;
    aFeoRegInfo->stride      = feoRegInfo.stride;

  #if 0
    aFeoRegInfo->memID       = feoRegInfo.memInfo.memID;
    aFeoRegInfo->size        = feoRegInfo.memInfo.size;
    aFeoRegInfo->va          = feoRegInfo.memInfo.virtAddr;
    aFeoRegInfo->pa          = feoRegInfo.memInfo.phyAddr;
    aFeoRegInfo->bufSecu     = feoRegInfo.memInfo.bufSecu;
    aFeoRegInfo->bufCohe     = feoRegInfo.memInfo.bufCohe;
    aFeoRegInfo->useNoncache = feoRegInfo.memInfo.useNoncache;
#endif

    if(g_debugDump >= 1)
    {
        EIS_LOG("W(%u),H(%u)",aFeoRegInfo->inputW,aFeoRegInfo->inputH);
        EIS_LOG("X(%u),Y(%u),STRIDE(%u)",aFeoRegInfo->xSize,aFeoRegInfo->ySize,aFeoRegInfo->stride);
        EIS_LOG("feo: ID(%d),size(%u),VA(0x%8x),PA(0x%8x),Secu(%d),Cohe(%d),Noncache(%d)",aFeoRegInfo->memID,
                                                                                          aFeoRegInfo->size,
                                                                                          aFeoRegInfo->va,
                                                                                          aFeoRegInfo->pa,
                                                                                          aFeoRegInfo->bufCohe,
                                                                                          aFeoRegInfo->bufSecu,
                                                                                          aFeoRegInfo->useNoncache);
    }
}

/*******************************************************************************
*
********************************************************************************/
MVOID EisHalImp::GetEisPlusGpuInfo(EIS_PLUS_GPU_INFO *aEisPlusGpu)
{
    aEisPlusGpu->gridW = mGpuGridW;
    aEisPlusGpu->gridH = mGpuGridH;

    if(g_debugDump >= 1)
    {
        EIS_LOG("W(%u),H(%u)",aEisPlusGpu->gridW,aEisPlusGpu->gridH);
    }
}


/*******************************************************************************
*
********************************************************************************/
MBOOL EisHalImp::GetEisSupportInfo(const MUINT32 &aSensorIdx)
{
    mEisSupport = m_pEisDrv->GetEisSupportInfo(aSensorIdx);
    return mEisSupport;
}

/*******************************************************************************
*
********************************************************************************/
MVOID EisHalImp::GetFeoStatistic()
{
    m_pEisDrv->GetFeoStatistic(&mFeoStatData);
}

/*******************************************************************************
*
********************************************************************************/
MUINT32 EisHalImp::GetEisPlusCropRatio()
{
#if RECORD_WITHOUT_EIS_ENABLE
    if (g_AIMDump == 1)
    {
        return 100;
    }
#endif
    if(g_debugDump >= 1)
    {
        EIS_LOG("mEisPlusCropRatio(%d)",mEisPlusCropRatio);
    }
    return mEisPlusCropRatio;
}

/*******************************************************************************
*
********************************************************************************/
MVOID EisHalImp::GetEisInfoAtP2(char const *userName,EIS_P1_RESULT_INFO *apEisResult,const MINT64 &aTimeStamp,MBOOL lastUser)
{
    if(mEisP2UserCnt == 0)
    {
        EIS_LOG("No P2 user");
    }
    else if(mEisResultForP2.empty())
    {
        EIS_ERR("(%s)mEisResultForP2 is empty",userName);
        apEisResult->timeStamp = EIS_EISO_SYNC_FAIL;
    }
    else
    {
        EIS_LOG("%s(%d)",userName,lastUser);

        Mutex::Autolock lock(mLock);

        MUINT32 hasMatch = 0;

        while(!mEisResultForP2.empty())
        {
            if(mEisResultForP2.front().timeStamp < aTimeStamp)
            {
                EIS_LOG("drop,TS(%lld)",mEisResultForP2.front().timeStamp);
                mEisResultForP2.pop();
            }
            else if(mEisResultForP2.front().timeStamp == aTimeStamp)
            {
                hasMatch = 1;
                break;
            }
            else
            {
                hasMatch = 0;
                break;
            }
        }

        if(hasMatch == 1)
        {
            apEisResult->cmvX_Int   = mEisResultForP2.front().cmvX_Int;
            apEisResult->cmvX_Flt   = mEisResultForP2.front().cmvX_Flt;
            apEisResult->cmvY_Int   = mEisResultForP2.front().cmvY_Int;
            apEisResult->cmvY_Flt   = mEisResultForP2.front().cmvY_Flt;
            apEisResult->gmvX       = mEisResultForP2.front().gmvX;
            apEisResult->gmvY       = mEisResultForP2.front().gmvY;
            apEisResult->DoEisCount = mEisResultForP2.front().DoEisCount;
            apEisResult->gmvConfidX = mEisResultForP2.front().gmvConfidX;
            apEisResult->gmvConfidY = mEisResultForP2.front().gmvConfidY;
            apEisResult->timeStamp  = mEisResultForP2.front().timeStamp;

            if(g_debugDump > 0)
            {
                EIS_LOG("match,TS(%lld)",mEisResultForP2.front().timeStamp);
            }

            if(lastUser == MTRUE)
            {
                mEisResultForP2.pop();
            }
        }
        else
        {
            apEisResult->timeStamp = EIS_EISO_SYNC_FAIL;
            EIS_ERR("no match");
        }

        if(g_debugDump > 0)
        {
            EIS_LOG("cmvX(%u,%u),cmvY(%u,%u),GMV(%d,%d),Count(%d),Confid(%d,%d),TS(%lld)",apEisResult->cmvX_Int,
                                                                                          apEisResult->cmvX_Flt,
                                                                                          apEisResult->cmvY_Int,
                                                                                          apEisResult->cmvY_Flt,
                                                                                          apEisResult->gmvX,
                                                                                          apEisResult->gmvY,
                                                                                          apEisResult->DoEisCount,
                                                                                          apEisResult->gmvConfidX,
                                                                                          apEisResult->gmvConfidY,
                                                                                          apEisResult->timeStamp);
        }
    }
}

/*******************************************************************************
*
********************************************************************************/
MVOID EisHalImp::GetEisInputSize(MUINT32 *aWidth, MUINT32 *aHeight)
{
    if(NULL != m_pEisDrv)
    {
        m_pEisDrv->GetEisInputSize(aWidth, aHeight);
    }else
    {
        EIS_ERR("m_pEisDrv is NULL");
    }
}

/*******************************************************************************
*
********************************************************************************/
MUINT32 EisHalImp::GetEisDivH()
{
    if(NULL != m_pEisDrv)
    {
        return m_pEisDrv->GetEisDivH();
    }else
    {
        EIS_ERR("m_pEisDrv is NULL");
    }
    return 1;
}

/*******************************************************************************
*
********************************************************************************/
MUINT32 EisHalImp::GetEisDivV()
{
    if(NULL != m_pEisDrv)
    {
        return m_pEisDrv->GetEisDivV();
    }else
    {
        EIS_ERR("m_pEisDrv is NULL");
    }
     return 1;
}


/*******************************************************************************
*
********************************************************************************/
MVOID EisHalImp::FlushMemory(const EIS_DMA_ENUM &aDma,const EIS_FLUSH_ENUM &aFlush)
{
    m_pEisDrv->FlushMemory(aDma,aFlush);
}


MINT32 EisHalImp::GetBufEis(android::sp<IImageBuffer>& spBuf)
{
    Mutex::Autolock lock(mEISOBufferListLock);

    if (!mEISOBufferList.empty())
    {
        spBuf = mEISOBufferList.front();
        mEISOBufferList.pop();
        if (UNLIKELY(g_debugDump >= 1))
        {
            EIS_LOG("GetBufEis : %d",mEISOBufferList.size());
        }
    }else
    {
        spBuf = m_pEISOSliceBuffer[EISO_BUFFER_NUM];

        EIS_WRN("GetBufEis empty!!");
    }
    return EIS_RETURN_NO_ERROR;
}

MSize EisHalImp::QueryMinSize(MBOOL isEISOn, MSize sensorSize, MSize requestSize)
{
    MSize retSize;
    MUINT32 out_width;
    MUINT32 out_height;
    if (MFALSE == isEISOn)
    {
        out_width = (requestSize.w <= 160)? 160 : requestSize.w;
        out_height = (requestSize.h <= 160)? 160 : requestSize.h;
    }else
    {
        out_width = (requestSize.w <= EIS_FE_MAX_INPUT_W)? EIS_FE_MAX_INPUT_W : requestSize.w;
        out_height = (requestSize.h <= EIS_FE_MAX_INPUT_H)? EIS_FE_MAX_INPUT_H : requestSize.h;
    }
    retSize = MSize(out_width,out_height);


    return retSize;
}


MINT32 EisHalImp::NotifyEis(QBufInfo&   pBufInfo)
{
    android::sp<IImageBuffer> retBuf;
    for (size_t i = 0; i < pBufInfo.mvOut.size(); i++)
    {
        if (pBufInfo.mvOut[i].mPortID.index == PORT_EISO.index)
        {

            Mutex::Autolock lock(mEISOBufferListLock);

            retBuf = (pBufInfo.mvOut.at(i).mBuffer);
            mEISOBufferList.push(retBuf);
            if (UNLIKELY(g_debugDump >= 1))
            {
                EIS_LOG("NotifyEis : %d",mEISOBufferList.size());
            }
        }
    }

    return EIS_RETURN_NO_ERROR;
}

MINT32 EisHalImp::NotifyEis(android::sp<NSCam::IImageBuffer>& spBuf)
{
    Mutex::Autolock lock(mEISOBufferListLock);

    if (spBuf != 0)
    {
        mEISOBufferList.push(spBuf);
        if (UNLIKELY(g_debugDump >= 1))
        {
            EIS_LOG("NotifyEis : %d - Drop",mEISOBufferList.size());
        }
    }

    return EIS_RETURN_NO_ERROR;
}


/*******************************************************************************
*
********************************************************************************/
MVOID EisHalImp::SendCommand(EIS_CMD_ENUM aCmd,MINT32 arg1, MINT32 arg2, MINT32 arg3)
{
    switch(aCmd)
    {
        case EIS_CMD_SET_STATE:
            m_pEisDrv->SetEisoThreadState((EIS_SW_STATE_ENUM)arg1);
            break;
        case EIS_CMD_CHECK_STATE:
            m_pEisDrv->SetEisoThreadState((EIS_SW_STATE_ENUM)arg1);
            break;
        default:
            EIS_ERR("wrong cmd(%d)",aCmd);
    }
}

#if EIS_ALGO_READY

/*******************************************************************************
*
********************************************************************************/
MVOID EisHalImp::GetEisStatistic(EIS_STATISTIC_STRUCT *a_pEIS_Stat)
{
    for(MINT32 i = 0; i < EIS_MAX_WIN_NUM; ++i)
    {
        a_pEIS_Stat->i4LMV_X[i]    = mEisAlgoProcData.eis_state.i4LMV_X[i];
        a_pEIS_Stat->i4LMV_Y[i]    = mEisAlgoProcData.eis_state.i4LMV_Y[i];
        a_pEIS_Stat->i4LMV_X2[i]   = mEisAlgoProcData.eis_state.i4LMV_X2[i];
        a_pEIS_Stat->i4LMV_Y2[i]   = mEisAlgoProcData.eis_state.i4LMV_Y2[i];
        a_pEIS_Stat->NewTrust_X[i] = mEisAlgoProcData.eis_state.NewTrust_X[i];
        a_pEIS_Stat->NewTrust_Y[i] = mEisAlgoProcData.eis_state.NewTrust_Y[i];
        a_pEIS_Stat->SAD[i]        = mEisAlgoProcData.eis_state.SAD[i];
        a_pEIS_Stat->SAD2[i]       = mEisAlgoProcData.eis_state.SAD2[i];
        a_pEIS_Stat->AVG_SAD[i]    = mEisAlgoProcData.eis_state.AVG_SAD[i];
    }
}

/*******************************************************************************
*
********************************************************************************/
MVOID EisHalImp::GetEisCustomize(EIS_TUNING_PARA_STRUCT *a_pDataOut)
{
    if(g_debugDump >= 1)
    {
        EIS_LOG("+");
    }

    EIS_Customize_Para_t customSetting;

    get_EIS_CustomizeData(&customSetting);

    a_pDataOut->sensitivity            = (EIS_SENSITIVITY_ENUM)customSetting.sensitivity;
    a_pDataOut->filter_small_motion    = customSetting.filter_small_motion;
    a_pDataOut->adv_shake_ext          = customSetting.adv_shake_ext;  // 0 or 1
    a_pDataOut->stabilization_strength = customSetting.stabilization_strength;  // 0.5~0.95

    a_pDataOut->advtuning_data.new_tru_th        = customSetting.new_tru_th;         // 0~100
    a_pDataOut->advtuning_data.vot_th            = customSetting.vot_th;             // 1~16
    a_pDataOut->advtuning_data.votb_enlarge_size = customSetting.votb_enlarge_size;  // 0~1280
    a_pDataOut->advtuning_data.min_s_th          = customSetting.min_s_th;           // 10~100
    a_pDataOut->advtuning_data.vec_th            = customSetting.vec_th;             // 0~11   should be even
    a_pDataOut->advtuning_data.spr_offset        = customSetting.spr_offset;         //0 ~ MarginX/2
    a_pDataOut->advtuning_data.spr_gain1         = customSetting.spr_gain1;          // 0~127
    a_pDataOut->advtuning_data.spr_gain2         = customSetting.spr_gain2;          // 0~127

    a_pDataOut->advtuning_data.gmv_pan_array[0] = customSetting.gmv_pan_array[0];   //0~5
    a_pDataOut->advtuning_data.gmv_pan_array[1] = customSetting.gmv_pan_array[1];   //0~5
    a_pDataOut->advtuning_data.gmv_pan_array[2] = customSetting.gmv_pan_array[2];   //0~5
    a_pDataOut->advtuning_data.gmv_pan_array[3] = customSetting.gmv_pan_array[3];   //0~5

    a_pDataOut->advtuning_data.gmv_sm_array[0] = customSetting.gmv_sm_array[0];    //0~5
    a_pDataOut->advtuning_data.gmv_sm_array[1] = customSetting.gmv_sm_array[1];    //0~5
    a_pDataOut->advtuning_data.gmv_sm_array[2] = customSetting.gmv_sm_array[2];    //0~5
    a_pDataOut->advtuning_data.gmv_sm_array[3] = customSetting.gmv_sm_array[3];    //0~5

    a_pDataOut->advtuning_data.cmv_pan_array[0] = customSetting.cmv_pan_array[0];   //0~5
    a_pDataOut->advtuning_data.cmv_pan_array[1] = customSetting.cmv_pan_array[1];   //0~5
    a_pDataOut->advtuning_data.cmv_pan_array[2] = customSetting.cmv_pan_array[2];   //0~5
    a_pDataOut->advtuning_data.cmv_pan_array[3] = customSetting.cmv_pan_array[3];   //0~5

    a_pDataOut->advtuning_data.cmv_sm_array[0] = customSetting.cmv_sm_array[0];    //0~5
    a_pDataOut->advtuning_data.cmv_sm_array[1] = customSetting.cmv_sm_array[1];    //0~5
    a_pDataOut->advtuning_data.cmv_sm_array[2] = customSetting.cmv_sm_array[2];    //0~5
    a_pDataOut->advtuning_data.cmv_sm_array[3] = customSetting.cmv_sm_array[3];    //0~5

    a_pDataOut->advtuning_data.vot_his_method  = (EIS_VOTE_METHOD_ENUM)customSetting.vot_his_method; //0 or 1
    a_pDataOut->advtuning_data.smooth_his_step = customSetting.smooth_his_step; // 2~6

    a_pDataOut->advtuning_data.eis_debug = customSetting.eis_debug;

    if(g_debugDump >= 1)
    {
        EIS_LOG("-");
    }
}

/*******************************************************************************
*
********************************************************************************/
MVOID EisHalImp::GetEisPlusCustomize(EIS_PLUS_TUNING_PARA_STRUCT *a_pTuningData)
{
    if(g_debugDump >= 1)
    {
        EIS_LOG("+");
    }

    EIS_PLUS_Customize_Para_t customSetting;

    get_EIS_PLUS_CustomizeData(&customSetting);

    a_pTuningData->warping_mode           = static_cast<MINT32>(customSetting.warping_mode);
    a_pTuningData->effort                 = 2;  // limit to 400 points
    a_pTuningData->search_range_x         = customSetting.search_range_x;
    a_pTuningData->search_range_y         = customSetting.search_range_y;
    a_pTuningData->crop_ratio             = customSetting.crop_ratio;
    a_pTuningData->gyro_still_time_th = customSetting.gyro_still_time_th;
    a_pTuningData->gyro_max_time_th = customSetting.gyro_max_time_th;
    a_pTuningData->gyro_similar_th = customSetting.gyro_similar_th;
    a_pTuningData->stabilization_strength = customSetting.stabilization_strength;

    if(g_debugDump >= 1)
    {
        EIS_LOG("-");
    }
}

/*******************************************************************************
*
********************************************************************************/
MVOID EisHalImp::DumpStatistic(const EIS_STATISTIC_STRUCT &aEisStat)
{
    EIS_LOG("+");

    for(MUINT32 i = 0; i < EIS_MAX_WIN_NUM; ++i)
    {
        EIS_INF("MB%d%d,(LMV_X,LMV_Y)=(%d,%d)",(i/4),(i%4),aEisStat.i4LMV_X[i],aEisStat.i4LMV_Y[i]);
    }

    for(MUINT32 i = 0; i < EIS_MAX_WIN_NUM; ++i)
    {
        EIS_INF("MB%d%d,(LMV_X2,LMV_Y2)=(%d,%d)",(i/4),(i%4),aEisStat.i4LMV_X2[i],aEisStat.i4LMV_Y2[i]);
    }

    for(MUINT32 i = 0; i < EIS_MAX_WIN_NUM; ++i)
    {
        EIS_INF("MB%d%d,MinSAD(%u)",(i/4),(i%4),aEisStat.SAD[i]);
    }

    for(MUINT32 i = 0; i < EIS_MAX_WIN_NUM; ++i)
    {
        EIS_INF("MB%d%d,(NewTrust_X,NewTrust_Y)=(%u,%u)",(i/4),(i%4),aEisStat.NewTrust_X[i],aEisStat.NewTrust_Y[i]);
    }

    for(MUINT32 i = 0; i < EIS_MAX_WIN_NUM; ++i)
    {
        EIS_INF("MB%d%d,MinSAD2(%u)",(i/4),(i%4),aEisStat.SAD2[i]);
    }

    for(MUINT32 i = 0; i < EIS_MAX_WIN_NUM; ++i)
    {
        EIS_INF("MB%d%d,AvgSAD(%u)",(i/4),(i%4),aEisStat.AVG_SAD[i]);
    }

    EIS_LOG("-");
}

#endif

