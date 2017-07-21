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
#define LOG_TAG "af_mgr_v3"

#ifndef ENABLE_MY_LOG
#define ENABLE_MY_LOG       (1)
#endif

#include <utils/threads.h>  // For Mutex::Autolock.
#include <sys/time.h>
#include <cutils/properties.h>
#include <aaa_types.h>
#include <aaa_error_code.h>
#include <kd_camera_feature.h>
#include <aaa_log.h>
#include <faces.h>
#include <Hal3/aaa_hal_common.h>
#include <camera_custom_nvram.h>
#include <af_param.h>
#include <pd_param.h>
#include <awb_param.h>
#include <ae_param.h>
#include <af_tuning_custom.h>
#include <mcu_drv.h>
#include <isp_reg.h>
#include <isp_drv.h>
#include <IHalSensor.h>
#include <aaa_sensor_mgr.h>
#include <iopipe/CamIO/INormalPipe.h>
#include <nvram_drv.h>
#include <nvram_drv_mgr.h>
#include <cct_feature.h>
#include <flash_param.h>
#include <isp_tuning.h>
#include <isp_tuning_mgr.h>
#include <af_feature.h>

//#include "af_mgr_if.h"
#include "af_mgr.h"
#include "camera_custom_cam_cal.h"  //seanlin 121022 for test
#include "cam_cal_drv.h" //seanlin 121022 for test
#include "nvbuf_util.h"
#include <isp_mgr.h>
#include "aaa_common_custom.h"
#include "aaa_sensor_buf_mgr.h"
#include <pd_mgr_if.h>

//configure HW
#include <isp_mgr_af_stat.h>

#include <math.h>
#include <android/sensor.h>             // for g/gyro sensor listener
#include <utils/include/SensorListener.h>    // for g/gyro sensor listener
#define SENSOR_ACCE_POLLING_MS  33
#define SENSOR_GYRO_POLLING_MS  33
#define SENSOR_ACCE_SCALE       100
#define SENSOR_GYRO_SCALE       100

#define LASER_TOUCH_REGION_W    0
#define LASER_TOUCH_REGION_H    0

#define AF_ENLOG_STATISTIC 2
#define AF_ENLOG_ROI 4

using namespace NS3Av3;
using namespace NSIspTuning;
using namespace NSIspTuningv3;
using namespace NSCam;
using namespace NSIoPipe;
using namespace NSCamIOPipe;
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AfMgr* AfMgr::s_pAfMgr = MNULL;

template <ESensorDev_T const eSensorDev>
class AfMgrDev : public AfMgr
{
public:
    static AfMgr& getInstance()
    {
        static AfMgrDev<eSensorDev> singleton;
        AfMgr::s_pAfMgr = &singleton;
        return singleton;
    }

    AfMgrDev() : AfMgr( eSensorDev) {}
    virtual ~AfMgrDev() {}
};
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// g/gyro sensor listener handler and data
static MINT32  gAcceInfo[3];
static MUINT64 gAcceTS;
static MUINT64 gPreAcceTS;
static MINT32  gGyroInfo[3];
static MUINT64 gGyroTS;
static MUINT64 gPreGyroTS;
static SensorListener* gpSensorListener=NULL;
static Mutex gCommonLock;
void afListenHandler( ASensorEvent event)
{
    switch( event.type)
    {
    case ASENSOR_TYPE_ACCELEROMETER:
    {
        gAcceInfo[0] = event.acceleration.x * SENSOR_ACCE_SCALE;
        gAcceInfo[1] = event.acceleration.y * SENSOR_ACCE_SCALE;
        gAcceInfo[2] = event.acceleration.z * SENSOR_ACCE_SCALE;
        gAcceTS = event.timestamp;
        /*MY_LOG("SensorEventAF Acc(%f,%f,%f,%lld)",
            event.acceleration.x,
            event.acceleration.y,
            event.acceleration.z,
            event.timestamp);*/
        break;
    }
    case ASENSOR_TYPE_GYROSCOPE:
    {
        gGyroInfo[0] = event.vector.x * SENSOR_GYRO_SCALE;
        gGyroInfo[1] = event.vector.y * SENSOR_GYRO_SCALE;
        gGyroInfo[2] = event.vector.z * SENSOR_GYRO_SCALE;
        gGyroTS = event.timestamp;
        /*MY_LOG("SensorEventAF Gyro(%f,%f,%f,%lld)",
            event.vector.x,
            event.vector.y,
            event.vector.z,
            event.timestamp);*/
        break;
    }
    case ASENSOR_TYPE_MAGNETIC_FIELD:
    case ASENSOR_TYPE_LIGHT:
    case ASENSOR_TYPE_PROXIMITY:
    default:
    {
        MY_LOG( "unknown type(%d)", event.type);
        break;
    }
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AfMgr& AfMgr::getInstance( MINT32 const i4SensorDev)
{
    switch( i4SensorDev)
    {
    /* Main Sensor*/
    case ESensorDev_Main :
        return AfMgrDev<ESensorDev_Main>::getInstance();

    /* Main Second Sensor */
    case ESensorDev_MainSecond :
        return AfMgrDev<ESensorDev_MainSecond>::getInstance();

    /* Sub Sensor */
    case ESensorDev_Sub :
        return AfMgrDev<ESensorDev_Sub>::getInstance();

    default:
        MY_LOG( "i4SensorDev = %d", i4SensorDev);
        if( AfMgr::s_pAfMgr)
        {
            return *AfMgr::s_pAfMgr;
        }
        else
        {
            return AfMgrDev<ESensorDev_Main>::getInstance();
        }
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AfMgr::AfMgr( ESensorDev_T eSensorDev)
{
    m_i4CurrSensorDev = (MINT32)eSensorDev;
    m_i4Users   = 0;
    m_CCTUsers  = 0;
    m_pMcuDrv   = NULL;
    m_pLaserDrv = NULL;
    m_pIAfAlgo  = NULL;
    m_bAcceEn   = MFALSE;
    m_bGyroEn   = MFALSE;
    memset( &m_sAFInput,     0, sizeof(AF_INPUT_T));
    memset( &m_sAFOutput,    0, sizeof(AF_OUTPUT_T));
    memset( &m_sAFParam,     0, sizeof(AF_PARAM_T));
    memset( &m_sNVRam,       0, sizeof(NVRAM_LENS_PARA_STRUCT));
    memset( &m_sArea_Center, 0, sizeof(AREA_T));
    memset( &m_sArea_APCmd,  0, sizeof(AREA_T));
    memset( &m_sArea_OTFD,   0, sizeof(AREA_T));

    m_i4CurrSensorId = 0x1;
    m_i4TGSzW        = 0;
    m_i4TGSzH        = 0;
    m_i4CurrLensId   = 0;
    m_eCurrAFMode    = NUM_OF_AF_MODE; /*force to waitting AP send setting mode command. [Ref : setafmode function]*/
    m_bIsFullScan    = FALSE;
    m_i4FullScanStep = 0;
    m_i4EnableAF     = -1;
    m_eLIB3A_AFMode  = LIB3A_AF_MODE_AFS;
    m_i4DgbLogLv     = 0;
    m_i4SensorIdx    = 0;
    m_i4SensorMode   = SENSOR_SCENARIO_ID_NORMAL_PREVIEW;
    m_i4BINSzW       = 0;
    m_i4BINSzH       = 0;

    m_i4PDSensorType=0;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AfMgr::~AfMgr()
{}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::Hal3AInitState( MINT32 i4SensorIdx, MINT32 isInitMCU)
{
    // --- init Laser ---
    if( (m_pLaserDrv==NULL) && (m_i4CurrSensorDev==ESensorDev_Main))
    {
        m_pLaserDrv = LaserDrv::getInstance();

        if( m_pLaserDrv)
        {
            if( m_pLaserDrv->init()==0)
            {
                m_pLaserDrv = NULL;
                MY_LOG_IF( m_i4DgbLogLv, "[%s] m_pLaserDrv->init() fail", __FUNCTION__);
            }
            else
            {
                MY_LOG( "[%s] m_pLaserDrv->init() done", __FUNCTION__);
            }
        }
    }

    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::Hal3AUninitState( MINT32 isInitMCU)
{
    if( m_pLaserDrv)
    {
        m_pLaserDrv->uninit();
        m_pLaserDrv = NULL;
    }

    return S_AF_OK;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AfMgr::sendAFNormalPipe( MINT32 cmd, MINTPTR arg1, MINTPTR arg2, MINTPTR arg3)
{
    MBOOL ret = MFALSE;

    NSCam::NSIoPipe::NSCamIOPipe::INormalPipe *pPipe;
    pPipe = NSCam::NSIoPipe::NSCamIOPipe::INormalPipe::createInstance( m_i4SensorIdx, LOG_TAG);

    if( pPipe==NULL)
    {
        MY_ERR( "Fail to create NormalPipe");
    }
    else
    {
        ret = pPipe->sendCommand( cmd, arg1, arg2, arg3);
        pPipe->destroyInstance( LOG_TAG);
    }

    return ret;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::init( MINT32 i4SensorIdx, MINT32 isInitMCU)
{
    MY_LOG( "AF-%-15s: + Dev %d", __FUNCTION__, m_i4CurrSensorDev);

    MRESULT ret = E_3A_ERR;

    Mutex::Autolock lock(m_Lock);

    if( m_i4Users==0)
    {
        /* Init af_mgr, when no user calls af_mgr init.*/

        // initial property
        char value[PROPERTY_VALUE_MAX] = {'\0'};
        property_get( "debug.af_mgr.enable", value, "0");
        m_i4DgbLogLv = atoi(value);
        property_get( "debug.pd_vc.enable", value, "0");
        m_bPDVCTest  = atoi(value);

        //initial nonvolatilize data.
        memset( &m_NonValInfo, 0, sizeof(AF_NON_VOLATILIZE_INFO_T));
        m_NonValInfo.i4AFMode     = MTK_CONTROL_AF_MODE_CONTINUOUS_PICTURE;
        m_NonValInfo.eLib3AAFMode = LIB3A_AF_MODE_AFC;

        // initial flow control parameters.
        m_i4EnableAF  = -1;
        m_prePosition = 0;
        m_i4SensorIdx = i4SensorIdx;
        m_bPWrInit    = MTRUE; /*HAL3 : control by hal flow*/
        m_bMZHostEn   = MFALSE; //TODO

        // Get hybrid AF instance.
#if USE_OPEN_SOURCE_AF
        m_pIAfAlgo=NS3A::IAfAlgo::createInstance<NS3A::EAAAOpt_OpenSource>( m_i4CurrSensorDev);
#else
        m_pIAfAlgo=NS3A::IAfAlgo::createInstance<NS3A::EAAAOpt_MTK>( m_i4CurrSensorDev);
#endif

        //initial isp_mgr_af_stat for configure HW
        ISP_MGR_AF_STAT_CONFIG_T::getInstance( static_cast<ESensorDev_T>(m_i4CurrSensorDev)).init();

        ret = S_3A_OK;

    }
    else
    {
        /**
         *  Do not init af_mgr :
         *  1. User Cnt >= 1 : af_mgr is still used.
         *  2. User Cnt   < 0  : wrong host flow.
         */
        MY_LOG( "AF-%-15s: no init, user %d", __FUNCTION__, m_i4Users);
    }

    android_atomic_inc( &m_i4Users);
    MY_LOG( "AF-%-15s: EnAF %d, Algo(0x%x), users %d", __FUNCTION__, m_i4EnableAF, m_pIAfAlgo, m_i4Users);

    MY_LOG( "AF-%-15s: -", __FUNCTION__);
    return ret;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::uninit( MINT32 isInitMCU)
{
    MY_LOG( "AF-%-15s: +", __FUNCTION__);

    MRESULT ret = E_3A_ERR;

    Mutex::Autolock lock(m_Lock);

    // Clear full scan flag when leaving camera
    m_bIsFullScan = MFALSE;

    CamPwrOffState();

    if( m_i4Users==1)
    {
        /* Uninit af_mgr, when last user calls af_mgr uninit.*/

        // uninit hybrid AF
        if( m_pIAfAlgo)
        {
            m_pIAfAlgo->destroyInstance();
            m_pIAfAlgo = NULL;
        }

        // uninit g/gyro sensor listener
        {
            Mutex::Autolock lock(gCommonLock);
            if( (m_sNVRam.rAFNVRAM.i4SensorEnable > 0)  &&(gpSensorListener!=NULL))
            {
                if( m_bAcceEn)
                {
                    gpSensorListener->disableSensor(SensorListener::SensorType_Acc);
                }

                if( m_bGyroEn)
                {
                    gpSensorListener->disableSensor(SensorListener::SensorType_Gyro);
                }

                gpSensorListener->destroyInstance();
                gpSensorListener = NULL;
            }
        }

        MY_LOG( "AF-%-15s: sensor listener 0x%x", __FUNCTION__, gpSensorListener);
        ret = S_3A_OK;
    }
    else
    {
        /**
         *  Do not uninit af_mgr :
         *  1. User Cnt   >1 : af_mgr is still used.
         *  2. User Cnt <=0  : wrong host flow.
         */
        MY_LOG( "AF-%-15s: no uninit, user %d", __FUNCTION__, m_i4Users);
    }

    android_atomic_dec( &m_i4Users);
    MY_LOG( "AF-%-15s: Algo(0x%x), users %d", __FUNCTION__, m_pIAfAlgo, m_i4Users);

    MY_LOG( "AF-%-15s: -", __FUNCTION__);
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AfMgr::Start()
{
    MY_LOG( "AF-%-15s: + Dev %d", __FUNCTION__, m_i4CurrSensorDev);

    //reset member.
    memset( &m_sAFInput,        0, sizeof(m_sAFInput));
    memset( &m_sAFOutput,       0, sizeof(m_sAFOutput));
    memset( &m_sHWCfg,          0, sizeof(AF_CONFIG_T));
    memset( &m_sCropRegionInfo, 0, sizeof(AREA_T));
    memset( &m_sArea_Focusing,  0, sizeof(AREA_T));
    memset( &m_sArea_Center,    0, sizeof(AREA_T));
    memset( &m_sArea_APCmd,     0, sizeof(AREA_T));
    memset( &m_sArea_OTFD,      0, sizeof(AREA_T));
    memset( &m_sArea_HW,        0, sizeof(AREA_T));
    memset( &m_sArea_PD[0],     0, sizeof(AREA_T)*AF_PSUBWIN_NUM);
    memset( &m_aeBlockV,        0, sizeof(MUINT8)*25);
    memset( &m_sCurLensInfo,    0, sizeof(LENS_INFO_T));

    m_vFrmInfo.clear();
    m_i4PDAreaNum  = 0;
    m_i4CurLensPos = 0;
    m_i4PreLensPos = 0;
    m_i4DZFactor   = 100; /* Initial digital zoom factor. */
    m_i4ROISel_PreState   = AF_ROI_SEL_NONE;
    m_sFocusDis.i4LensPos = 0; /* default value, ref: af_tuning_customer.cpp*/
    m_sFocusDis.i4Dist    = 0.33; /* default value, ref: af_tuning_customer.cpp*/
    m_i4MvLensTo   = 0;
    m_i4HWBlkNumX  = 0;
    m_i4HWBlkNumY  = 0;
    m_i4IsFocused  = 0;
    m_i4CurFrmNum  = 0;
    m_i4RawSel     = 0;
    m_i4PDResNum   = 0;
    m_i4IsMonitorFV_PreState = MTRUE; /*Force to select ROI to focusing as first in, ref:SelROIToFocusing */
    m_i4IsMonitorFV_CurState = MTRUE;
    m_bIsROICmd    = MFALSE;
    m_bLatchROI    = MFALSE;
    m_bRunPDEn     = MFALSE;
    m_bLock        = MFALSE;
    m_bDAFEn       = MFALSE;
    m_bMZEn        = MFALSE;
    m_bGetMetaData = MFALSE;
    m_ptrNVRam     = NULL;



    //------------------------------------------- check algo instance-----------------------------------------
    if( !m_pIAfAlgo)
    {
        MY_ERR("AF-%-15s: AfAlgo pointer NULL", __FUNCTION__);
        m_i4EnableAF = 0;
        return S_AF_OK;
    }
    //-------------------------------------get sensor related information-----------------------------------
    SensorStaticInfo rSensorStaticInfo;
    if( m_i4EnableAF == -1)
    {
        IHalSensorList* const pIHalSensorList = IHalSensorList::get();
        IHalSensor* pIHalSensor = pIHalSensorList->createSensor( "af_mgr", m_i4SensorIdx);
        SensorDynamicInfo rSensorDynamicInfo;
        m_sMetaData = pIHalSensorList->queryStaticInfo( m_i4SensorIdx);
        m_bGetMetaData = MTRUE;

        switch( m_i4CurrSensorDev)
        {
        case ESensorDev_Main :
            pIHalSensorList->querySensorStaticInfo( NSCam::SENSOR_DEV_MAIN, &rSensorStaticInfo);
            pIHalSensor->querySensorDynamicInfo( NSCam::SENSOR_DEV_MAIN, &rSensorDynamicInfo);
            break;
        case ESensorDev_Sub:
            pIHalSensorList->querySensorStaticInfo( NSCam::SENSOR_DEV_SUB, &rSensorStaticInfo);
            pIHalSensor->querySensorDynamicInfo( NSCam::SENSOR_DEV_SUB, &rSensorDynamicInfo);
            break;
        case ESensorDev_MainSecond:
            pIHalSensorList->querySensorStaticInfo( NSCam::SENSOR_DEV_MAIN_2, &rSensorStaticInfo);
            pIHalSensor->querySensorDynamicInfo( NSCam::SENSOR_DEV_MAIN_2, &rSensorDynamicInfo);
            break;
        default:
            MY_ERR( "AF-%-15s: Invalid sensor device: %d", __FUNCTION__, m_i4CurrSensorDev);
            break;
        }

        if( pIHalSensor)
        {
            pIHalSensor->destroyInstance( "af_mgr");
        }

        /*0: NO PDAF, 1: PDAF Raw Data mode, 2:PDAF VC mode(HDR), 3:PDAF VC mode(Binning)*/
        MY_LOG( "AF-%-15s: PD sensor support info %d",
                __FUNCTION__,
                rSensorStaticInfo.PDAF_Support);

        /* 0:BGGR, 1:GBRG, 2GRBG, 3RGGB */
        m_Pix_Id = (MINT32)rSensorStaticInfo.sensorFormatOrder;
        MY_LOG( "AF-%-15s: PixId %d (0:B, 1:GB, 2:GR, 3:R)",
                __FUNCTION__,
                m_Pix_Id);

        /* Lens search */
        m_i4CurrSensorId = rSensorStaticInfo.sensorDevID;
        MCUDrv::lensSearch( m_i4CurrSensorDev, m_i4CurrSensorId);
        m_i4CurrLensId = MCUDrv::getCurrLensID( m_i4CurrSensorDev);
        MY_LOG( "AF-%-15s: Dev 0x%04x, SensorID 0x%04x, LensId 0x%04x",
                __FUNCTION__,
                m_i4CurrSensorDev,
                m_i4CurrSensorId,
                m_i4CurrLensId);

        if( m_i4CurrLensId == 0xFFFF) m_i4EnableAF = 0;
        else                          m_i4EnableAF = 1;

    }

    //---------------------------------------- init MCU ------------------------------
    CamPwrOnState();

    //---------------------------------------- init sensor related information -------------------------------------------
    // Get sensor information :
    // TG size
    sendAFNormalPipe( NSCam::NSIoPipe::NSCamIOPipe::ENPipeCmd_GET_TG_OUT_SIZE, (MINTPTR)(&m_i4TGSzW), (MINTPTR)(&m_i4TGSzH),0);
    // TG after BIN Blk size : for HPF coordinate setting.
    sendAFNormalPipe( NSCam::NSIoPipe::NSCamIOPipe::ENPipeCmd_GET_BIN_INFO, (MINTPTR)(&m_i4BINSzW), (MINTPTR)(&m_i4BINSzH), 0);
    // Checking sensor mode.
    MY_LOG( "AF-%-15s: Dev 0x%04x, TGSZ: W %d, H %d, BINSZ: W %d, H %d",
            __FUNCTION__,
            m_i4CurrSensorDev,
            m_i4TGSzW,
            m_i4TGSzH,
            m_i4BINSzW,
            m_i4BINSzH);

    if( (m_sAFInput.sEZoom.i4W == 0) || (m_sAFInput.sEZoom.i4H == 0))
    {
        m_sAFInput.sEZoom.i4W = m_i4TGSzW;
        m_sAFInput.sEZoom.i4H = m_i4TGSzH;
    }

    //check ZSD or not
    MUINT32 isZSD = ( m_i4TGSzW==(MINT32)rSensorStaticInfo.captureWidth) ? TRUE : FALSE;
    m_sAFInput.i4IsZSD = isZSD;


    //------------------------------------------- Get parameters -----------------------------------------
    //===============
    // Get parameters
    //===============
    m_sAFParam = getAFParam();

    //===============
    // Get data from NVRam
    //===============
    MINT32 err = NvBufUtil::getInstance().getBufAndRead( CAMERA_NVRAM_DATA_LENS, m_i4CurrSensorDev, (void*&)m_ptrNVRam);

    if( err!=0)
    {
        MY_ERR( "AF-%-15s: AfAlgo NvBufUtil get buf fail!", __FUNCTION__);
    }
    memcpy( &m_sNVRam, m_ptrNVRam, sizeof(NVRAM_LENS_PARA_STRUCT));


    MY_LOG( "AF-%-15s: [nvram][Version] %d %d",
            __FUNCTION__,
            m_sNVRam.Version,
            m_ptrNVRam->Version);
    //MY_LOG("[nvram][THRES_MAIN]%d", m_sNVRam.rAFNVRAM.sAF_Coef.i4THRES_MAIN);
    //MY_LOG("[nvram][SUB_MAIN]%d", m_sNVRam.rAFNVRAM.sAF_Coef.i4THRES_SUB);
    MY_LOG( "AF-%-15s: [nvram][Normal Num]%d [Macro Num]%d",
            __FUNCTION__,
            m_sNVRam.rAFNVRAM.sAF_Coef.sTABLE.i4NormalNum,
            m_sNVRam.rAFNVRAM.sAF_Coef.sTABLE.i4MacroNum);
    //MY_LOG("[nvram][VAFC Fail Cnt]%d", m_sNVRam.rAFNVRAM.i4VAFC_FAIL_CNT);
    //MY_LOG("[nvram][LV thres]%d", m_sNVRam.rAFNVRAM.i4LV_THRES);
    //MY_LOG("[nvram][PercentW]%d [PercentH]%d", m_sNVRam.rAFNVRAM.i4SPOT_PERCENT_W, m_sNVRam.rAFNVRAM.i4SPOT_PERCENT_H);
    //MY_LOG("[nvram][AFC step]%d", m_sNVRam.rAFNVRAM.i4AFC_STEP_SIZE);
    //MY_LOG("[nvram][InfPos]%d", m_sNVRam.rFocusRange.i4InfPos);
    //MY_LOG("[nvram][MacroPos]%d", m_sNVRam.rFocusRange.i4MacroPos);
    MY_LOG( "AF-%-15s: [Param][Normal min step]%d [Macro min step]%d",
            __FUNCTION__,
            m_sNVRam.rAFNVRAM.i4AFS_STEP_MIN_NORMAL,
            m_sNVRam.rAFNVRAM.i4AFS_STEP_MIN_MACRO);



#ifdef AFEXTENDCOEF
    //===============
    // Get extend parameters
    //===============
    CustAF_THRES_T sAFExtCoef = getAFExtendCoef( m_i4TGSzW, m_i4TGSzH, m_i4CurrSensorId);
    if( sAFExtCoef.i4ISONum)
    {
        memcpy( &m_sNVRam.rAFNVRAM.sREV2_AF_TH, &sAFExtCoef, sizeof(sAFExtCoef));
        m_sAFInput.i4IsRevMode2=TRUE;
    }
    else
    {
        m_sAFInput.i4IsRevMode2=FALSE;
    }
    MY_LOG( "AF-%-15s: AFExtCoef %d",
            __FUNCTION__,
            m_sAFInput.i4IsRevMode2);
#endif


    //------------------------------------------- init g/gyro sensor listener -----------------------------------------
    {
        Mutex::Autolock lock(gCommonLock);
        if( (m_sNVRam.rAFNVRAM.i4SensorEnable>0) && (gpSensorListener==NULL))
        {
            gpSensorListener = SensorListener::createInstance();
            gpSensorListener->setListener( afListenHandler);

            gAcceInfo[0] = gAcceInfo[1] = gAcceInfo[2] = 0;
            m_bAcceEn = gpSensorListener->enableSensor( SensorListener::SensorType_Acc, SENSOR_ACCE_POLLING_MS);
            if( !m_bAcceEn)
            {
                gpSensorListener->disableSensor( SensorListener::SensorType_Acc);
            }

            gGyroInfo[0] = gGyroInfo[1] = gGyroInfo[2] = 0;
            m_bGyroEn = gpSensorListener->enableSensor( SensorListener::SensorType_Gyro, SENSOR_GYRO_POLLING_MS);
            if( !m_bGyroEn)
            {
                gpSensorListener->disableSensor( SensorListener::SensorType_Gyro);
            }
        }
    }

    //------------------------------------------- Set Depth AF Info ----------------------------------------
    if( m_i4CurrLensId != 0xffff)
    {
        MINT32 AFtbl_Num;
        MINT32 AFtbl_Marco;
        MINT32 AFtbl_Inf;

        m_bDAFEn = (m_sNVRam.rAFNVRAM.i4Coefs[11]>0) ? MTRUE : MFALSE;

        if( (m_sAFInput.i4IsVDO==TRUE) && (m_eLIB3A_AFMode == LIB3A_AF_MODE_AFC_VIDEO) )
        {
            AFtbl_Num   = m_sNVRam.rAFNVRAM.sVAFC_Coef.sTABLE.i4NormalNum;
            AFtbl_Inf   = m_sNVRam.rAFNVRAM.sVAFC_Coef.sTABLE.i4Offset + m_sNVRam.rAFNVRAM.sVAFC_Coef.sTABLE.i4Pos[0];
            AFtbl_Marco = m_sNVRam.rAFNVRAM.sVAFC_Coef.sTABLE.i4Offset + m_sNVRam.rAFNVRAM.sVAFC_Coef.sTABLE.i4Pos[AFtbl_Num-1];
        }
        else
        {
            AFtbl_Num   = m_sNVRam.rAFNVRAM.sAF_Coef.sTABLE.i4NormalNum;
            AFtbl_Inf   = m_sNVRam.rAFNVRAM.sAF_Coef.sTABLE.i4Offset + m_sNVRam.rAFNVRAM.sAF_Coef.sTABLE.i4Pos[0];
            AFtbl_Marco = m_sNVRam.rAFNVRAM.sAF_Coef.sTABLE.i4Offset + m_sNVRam.rAFNVRAM.sAF_Coef.sTABLE.i4Pos[AFtbl_Num-1];
        }

        m_sDAF_TBL.is_daf_run = 0;
        m_sDAF_TBL.af_dac_min = AFtbl_Inf;
        m_sDAF_TBL.af_dac_max = AFtbl_Marco;
    }

    //-------------------------------------------- init PD mgr ---------------------------------------------

    // check support PDAF or not
    SENSOR_VC_INFO_T SensorVCInfo;
    AAASensorMgr::getInstance().getSensorVCinfo( m_i4CurrSensorDev, &SensorVCInfo, m_i4SensorMode);

    //===============
    // pd profile for getting pd_mgr
    //===============
    SPDProfile_t pdprofile;
    pdprofile.i4CurrSensorId = m_i4CurrSensorId;
    pdprofile.u4IsZSD        = isZSD;

    // Checking PD pixel is supported on currently sensor or not.
    switch( rSensorStaticInfo.PDAF_Support)
    {
    /*0: NO PDAF, 1: PDAF Raw Data mode, 2:PDAF VC mode(HDR), 3:PDAF VC mode(Binning)*/
    case 1:
        m_i4PDSensorType = 1;
        pdprofile.uSensorType = 1;
        break;
    case 2 :
        m_i4PDSensorType = 2;
        pdprofile.uSensorType = 2;
        break;
    case 3 :
        m_i4PDSensorType = 2;
        pdprofile.uSensorType = 3;
        break;
    case 0 :
    default :
        m_i4PDSensorType = 0;
        pdprofile.uSensorType = 0;
        break;
    }

    pdprofile.u4Pdmode = m_i4PDSensorType;
    pdprofile.uImgXsz  = m_i4TGSzW;
    pdprofile.uImgYsz  = m_i4TGSzH;
    pdprofile.uFullXsz = (MINT32)rSensorStaticInfo.captureWidth;
    pdprofile.uFullYsz = (MINT32)rSensorStaticInfo.captureHeight;

    MY_LOG( "AF-%-15s: PD mode %d, ImgSz(%d, %d), FullSz(%d, %d), IsZSD %d",
            __FUNCTION__,
            m_i4PDSensorType,
            pdprofile.uImgXsz,
            pdprofile.uImgYsz,
            pdprofile.uFullXsz,
            pdprofile.uFullYsz,
            isZSD);

    //get pd_mgr
    m_PDBuf_Type = IPDMgr::getInstance().init( m_i4CurrSensorDev, &pdprofile);
    if( m_PDBuf_Type==EPDBuf_NotDef)
    {
        m_i4PDSensorType=0;
        MY_LOG( "AF-%-15s: PD buf type isn't defined", __FUNCTION__);
    }

    //===============
    // initial pd_mgr
    //===============
    if( m_i4PDSensorType!=0)
    {
        MRESULT ret=E_3A_ERR;

        //Get PD information from PD mgr and set  to Hybird AF.
        MINT32  PDInfoSz=0;
        MINT32 *PDInfo=NULL;

        IHalSensorList* const pIHalSensorList = IHalSensorList::get();
        IHalSensor* pIHalSensor = pIHalSensorList->createSensor( "PDAF-af_mgr", m_i4SensorIdx);

        if( m_sNVRam.rPDNVRAM.rCaliData.i4Size==0)
        {
            MY_LOG( "AF-%-15s: NVRAM NO PDAF calib data, read from EEPROM !!", __FUNCTION__);

            const MUINT32 _sz = 0x800;
            MUINT8  buf[_sz];
            MUINT8 *pbuf = buf;
            MINT32 bufsz  = _sz;
            MINT32 offset = 0x100;

            memset( buf, 0xFF, sizeof(MUINT8)*_sz);
            pIHalSensor->sendCommand( m_i4CurrSensorDev, SENSOR_CMD_GET_PDAF_DATA, (MUINTPTR)&offset, (MUINTPTR)&pbuf, (MUINTPTR)&bufsz);

            memcpy( m_sNVRam.rPDNVRAM.rCaliData.uData, buf, sizeof(MUINT8)*_sz);
            m_sNVRam.rPDNVRAM.rCaliData.i4Size = bufsz;

            memcpy( m_ptrNVRam->rPDNVRAM.rCaliData.uData, buf, sizeof(MUINT8)*_sz);
            m_ptrNVRam->rPDNVRAM.rCaliData.i4Size = bufsz;

            NvBufUtil::getInstance().write( CAMERA_NVRAM_DATA_LENS, m_i4CurrSensorDev);
        }
        else
        {
            MY_LOG( "AF-%-15s: NVRAM Have PDAF calib data %d",
                    __FUNCTION__,
                    m_sNVRam.rPDNVRAM.rCaliData.i4Size);
        }
        if(pIHalSensor) pIHalSensor->destroyInstance( "PDAF-af_mgr");

        //set pd calibration data to PD_mgr.
        ret = IPDMgr::getInstance().setPDCaliData( m_i4CurrSensorDev, reinterpret_cast<MVOID *>(&m_sNVRam.rPDNVRAM), PDInfoSz, &PDInfo);

        //PD information to hybrid AF
        if( ret==S_3A_OK)
        {
            memcpy( m_sAFInput.i4PDInfo, PDInfo, PDInfoSz);
            MY_LOG( "AF-%-15s: PD info (Sz=%d) to Hybrid AF after PD_mgr initailed %d %d",
                    __FUNCTION__,
                    PDInfoSz,
                    m_sAFInput.i4PDInfo[0],
                    m_sAFInput.i4PDInfo[1]);
        }
        else
        {
            MY_LOG( "AF-%-15s: Set PD calibration to PD mgr fail, pd buffer type (%d)",
                    __FUNCTION__,
                    m_PDBuf_Type);
            m_i4PDSensorType = 0;
            m_PDBuf_Type = EPDBuf_NotDef;
        }

        SPDLibVersion_t PdLibVersion;
        IPDMgr::getInstance().GetVersionOfPdafLibrary( m_i4CurrSensorDev, PdLibVersion);

    }
    else if( m_bPDVCTest)
    {
        //for engineer checking DMA data only.
        MY_LOG( "AF-%-15s: Debug virtual channel only : for engineer testing only!!", __FUNCTION__);
    }

    MY_LOG( "AF-%-15s: PD Sensor support mode %d\n", __FUNCTION__, m_i4PDSensorType);

    //-------------------------------------------------- init laser driver  ----------------------------------------------------
    if( m_pLaserDrv )
    {
        int Disable = 0;
        char value[PROPERTY_VALUE_MAX] = {'\0'};
        property_get( "laser.calib.disable", value, "0");
        Disable = atoi(value);

        if( Disable == 0 )
        {
            m_pLaserDrv->setLaserCalibrationData( (MUINT32)m_sNVRam.rAFNVRAM.i4Coefs[86], (MUINT32)m_sNVRam.rAFNVRAM.i4Coefs[87]);
        }
        m_pLaserDrv->setLaserGoldenTable( (MUINT32*)&m_sNVRam.rAFNVRAM.i4Coefs[71]);
        m_sAFInput.sLaserInfo.i4AfWinPosCnt = 0;

        Disable = 0;
        property_get( "laser.disable", value, "0");
        Disable = atoi(value);
        if( m_pLaserDrv->checkHwSetting() == 0 || Disable == 1)
        {
            m_pLaserDrv->uninit();
            m_pLaserDrv = NULL;
            MY_ERR( "AF-%-15s: m_pLaserDrv->checkHwSetting() fail", __FUNCTION__);
        }
    }

    //------------------------------------------- initial Hybrid AF algorithm ----------------------------------
    // Currently, multi zone is supported as PDAF is on.
    m_bMZEn = (m_i4PDSensorType!=0)&&m_bMZHostEn ? MTRUE : MFALSE;
    // Initial crop region information and center ROI coordinate will be updated automatically in SetCropRegionInfo.
    SetCropRegionInfo( 0, 0, (MUINT32)m_i4TGSzW, (MUINT32)m_i4TGSzH, AF_MGR_CALLER);
    // default using center ROI. m_sArea_Center is updated after calling SetCropRegionInfo.
    m_sAFInput.sAFArea.i4Count = 1;
    m_sAFInput.sAFArea.sRect[0] = m_sArea_Focusing = m_sArea_APCmd = m_sArea_OTFD = m_sArea_HW = m_sArea_Center;

    // update non-volatilize information for hybrid AF input
    memcpy( &(m_sAFInput.sLensInfo), &(m_NonValInfo.sLensPos), sizeof(LENS_INFO_T));

    // set parameters to hybrid AF
    m_pIAfAlgo->setAFParam( m_sAFParam, getAFConfig(), m_sNVRam.rAFNVRAM);

    // input TG and HW coordinate to hybridAF
    m_sAFInput.sTGSz = AF_COORDINATE_T( m_i4TGSzW,  m_i4TGSzH);
    m_sAFInput.sHWSz = AF_COORDINATE_T( m_i4BINSzW, m_i4BINSzH);

    // Get AF calibration data. Should do this after setAFParam is called.
    if( m_sNVRam.rAFNVRAM.i4ReadOTP==TRUE)
    {
        readOTP();
    }

    // initial hybrid AF algorithm
    m_pIAfAlgo->initAF( m_sAFInput, m_sAFOutput);

    //------------------------------------------- Configure AF HW -----------------------------------------
    // HW setting is got after calling initAF.
    ConfigHWReg( m_sAFOutput.sAFStatConfig, m_sArea_HW, m_i4HWBlkNumX, m_i4HWBlkNumY);

    //------------------------------------------- Control Hybrid AF -----------------------------------------
    // set mode to AF algo to start AF state machine
    m_eCurrAFMode   = m_NonValInfo.i4AFMode;
    if( m_bIsFullScan == MTRUE)
    {
        m_eLIB3A_AFMode = LIB3A_AF_MODE_FULLSCAN;
        m_pIAfAlgo->setAFMode(m_eLIB3A_AFMode);
    }
    else
    {
        m_eLIB3A_AFMode = m_NonValInfo.eLib3AAFMode;
        m_pIAfAlgo->setAFMode( m_eLIB3A_AFMode);
    }
    m_eAFStatePre = m_eAFState = NS3A::E_AF_INACTIVE;
#if 0
    if( m_bPWrInit==MFALSE && (m_eLIB3A_AFMode==LIB3A_AF_MODE_AFC || m_eLIB3A_AFMode==LIB3A_AF_MODE_AFC_VIDEO))
        m_pIAfAlgo->setAFMode( LIB3A_AF_MODE_AFC_MONITOR);
    else
        m_pIAfAlgo->setAFMode( m_eLIB3A_AFMode);
    m_bPWrInit=MFALSE; /*HAL3 power on initial sequence*/
#endif



    //------------------------------------------- init af_mgr flow control ------------------------------------
    m_i4IsMonitorFV_PreState = m_i4IsMonitorFV_CurState = m_sAFOutput.i4IsMonitorFV;

    //-------------------------------------------------- start searching  ----------------------------------------------------
    //Trigger AF to start searching.
    if( m_bIsFullScan == TRUE)
    {
        // Full Scan Mode don't trigger AF to start first searching
    }
    else if( m_eLIB3A_AFMode == LIB3A_AF_MODE_AFC_VIDEO || m_eLIB3A_AFMode == LIB3A_AF_MODE_AFC)
    {
        m_pIAfAlgo->trigger();
    }

    m_sAFInput.i4FullScanStep = m_i4FullScanStep;

    MY_LOG( "AF-%-15s: -", __FUNCTION__);
    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AfMgr::Stop()
{
    //store Nno-volatilize informaiton.
    getLensInfo( m_NonValInfo.sLensPos);
    MY_LOG( "AF-%-15s: + Dev %d, Record : Mode (%d)%d, Pos %d",
            __FUNCTION__,
            m_i4CurrSensorDev,
            m_NonValInfo.i4AFMode,
            m_NonValInfo.eLib3AAFMode,
            m_NonValInfo.sLensPos.i4CurrentPos );

    //reset parameters.
    m_bRunPDEn = MFALSE;

    IPDMgr::getInstance().uninit( m_i4CurrSensorDev);

    m_i4PDSensorType =  0;
    m_i4EnableAF     = -1;

    MY_LOG( "AF-%-15s: -", __FUNCTION__);
    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AfMgr::CamPwrOnState()
{
    MY_LOG( "AF-%-15s: + Dev %d, moveMCU %d",
            __FUNCTION__,
            m_i4CurrSensorDev,
            m_NonValInfo.sLensPos.i4CurrentPos);

    if( m_pMcuDrv == NULL)
    {
        m_pMcuDrv = MCUDrv::createInstance(m_i4CurrLensId);
        if( !m_pMcuDrv)
        {
            MY_ERR( "AF-%-15s: McuDrv::createInstance fail", __FUNCTION__);
            m_i4EnableAF = 0;
        }

        if( m_pMcuDrv->init(m_i4CurrSensorDev) < 0)
        {
            MY_ERR( "AF-%-15s: m_pMcuDrv->init() fail", __FUNCTION__);
            m_i4EnableAF = 0;
        }
        else
        {
            m_i8MvLensTS = MoveLensTo( m_NonValInfo.sLensPos.i4CurrentPos);
        }
    }

    MY_LOG( "AF-%-15s: -", __FUNCTION__);
    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AfMgr::CamPwrOffState()
{
    MY_LOG( "AF-%-15s: + Dev %d", __FUNCTION__, m_i4CurrSensorDev);

    if( m_pMcuDrv)
    {
        MY_LOG( "AF-%-15s: uninitMcuDrv - Dev: %d", __FUNCTION__, m_i4CurrSensorDev);
        m_pMcuDrv->uninit( m_i4CurrSensorDev);
        m_pMcuDrv->destroyInstance();
        m_pMcuDrv = NULL;
    }

    MY_LOG( "AF-%-15s: -", __FUNCTION__);
    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::CCTMCUNameinit( MINT32 i4SensorIdx)
{
    MY_LOG( "%s +", __FUNCTION__);
    Mutex::Autolock lock(m_Lock);

    if (m_CCTUsers > 0)
    {
        MY_LOG( "[CCTMCUNameinit] no init, %d has created", m_CCTUsers);
        android_atomic_inc(&m_CCTUsers);
        return S_3A_OK;
    }
    android_atomic_inc(&m_CCTUsers);
    SensorStaticInfo rSensorStaticInfo;
    IHalSensorList* const pIHalSensorList = IHalSensorList::get();
    switch(m_i4CurrSensorDev)
    {
    case ESensorDev_Main:
        pIHalSensorList->querySensorStaticInfo(NSCam::SENSOR_DEV_MAIN, &rSensorStaticInfo);
        break;
    case ESensorDev_Sub:
        pIHalSensorList->querySensorStaticInfo(NSCam::SENSOR_DEV_SUB, &rSensorStaticInfo);
        break;
    case ESensorDev_MainSecond:
        pIHalSensorList->querySensorStaticInfo(NSCam::SENSOR_DEV_MAIN_2, &rSensorStaticInfo);
        break;
    default:
        MY_ERR( "Invalid sensor device: %d", m_i4CurrSensorDev);
        break;
    }
    m_i4CurrSensorId=rSensorStaticInfo.sensorDevID;
    MCUDrv::lensSearch( m_i4CurrSensorDev, m_i4CurrSensorId);
    m_i4CurrLensId = MCUDrv::getCurrLensID(m_i4CurrSensorDev);
    MY_LOG( "[lens][SensorDev]0x%04x, [SensorId]0x%04x, [CurrLensId]0x%04x", m_i4CurrSensorDev, m_i4CurrSensorId, m_i4CurrLensId);

    if( m_i4CurrLensId==0xFFFF) m_i4EnableAF = 0;
    else                        m_i4EnableAF = 1;

    int err;
    err = NvBufUtil::getInstance().getBufAndRead( CAMERA_NVRAM_DATA_LENS, m_i4CurrSensorDev, (void*&)m_ptrNVRam);
    if(err!=0)
    {
        MY_ERR( "AfAlgo NvBufUtil get buf fail!");
    }


    MY_LOG( "%s -", __FUNCTION__);
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::CCTMCUNameuninit()
{
    MY_LOG_IF( m_i4DgbLogLv, "%s +", __FUNCTION__);
    Mutex::Autolock lock(m_Lock);

    if( m_CCTUsers<=0)
    {
        return S_3A_OK;
    }

    android_atomic_dec(&m_CCTUsers);
    if( m_CCTUsers!=0)
    {
        MY_LOG( "[CCTMCUNameuninit] Still %d users", m_CCTUsers);
        return S_AF_OK;
    }

    m_i4EnableAF = -1;
    MY_LOG( "%s - %d %d", __FUNCTION__, m_i4EnableAF, m_CCTUsers);
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPAFOpeartion()
{
    MY_LOG( "%s", __FUNCTION__);
    setAFMode( AF_MODE_AFS, AF_MGR_CALLER);
    triggerAF( AF_MGR_CALLER);
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPMFOpeartion( MINT32 a_i4MFpos)
{
    MINT32 i4TimeOutCnt = 0;

    MY_LOG( "%s %d", __FUNCTION__, a_i4MFpos);

    setAFMode( AF_MODE_MF, AF_MGR_CALLER);
    triggerAF( AF_MGR_CALLER);
    setMFPos(  a_i4MFpos, AF_MGR_CALLER);

    while( !isFocusFinish())
    {
        usleep( 5000); // 5ms
        i4TimeOutCnt++;
        if( i4TimeOutCnt>100)
        {
            break;
        }
    }

    //[TODO]:MY_LOG("[MF]pos:%d, value:%lld\n", a_i4MFpos, m_sAFInput.sAFStat.i8Stat24);

    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPAFGetAFInfo( MVOID *a_pAFInfo, MUINT32 *a_pOutLen)
{
    ACDK_AF_INFO_T *pAFInfo = (ACDK_AF_INFO_T *)a_pAFInfo;

    MY_LOG_IF( m_i4DgbLogLv, "%s", __FUNCTION__);

    pAFInfo->i4AFMode  = m_eLIB3A_AFMode;
    pAFInfo->i4AFMeter = LIB3A_AF_METER_SPOT;
    pAFInfo->i4CurrPos = m_sAFOutput.i4AFPos;

    *a_pOutLen = sizeof(ACDK_AF_INFO_T);

    MY_LOG( "[AF Mode] = %d", pAFInfo->i4AFMode);
    MY_LOG( "[AF Meter] = %d", pAFInfo->i4AFMeter);
    MY_LOG( "[AF Current Pos] = %d", pAFInfo->i4CurrPos);

    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPAFGetBestPos( MINT32 *a_pAFBestPos, MUINT32 *a_pOutLen)
{
    MY_LOG( "%s %d", __FUNCTION__, m_sAFOutput.i4AFBestPos);
    *a_pAFBestPos = m_sAFOutput.i4AFBestPos;
    *a_pOutLen    = sizeof(MINT32);
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPAFCaliOperation( MVOID *a_pAFCaliData, MUINT32 *a_pOutLen)
{
    ACDK_AF_CALI_DATA_T *pAFCaliData = (ACDK_AF_CALI_DATA_T *)a_pAFCaliData;
    AF_DEBUG_INFO_T rAFDebugInfo;
    MUINT32 aaaDebugSize;
    MINT32 i4TimeOutCnt = 0;

    MY_LOG( "%s", __FUNCTION__);

    setAFMode( AF_MODE_AFS, AF_MGR_CALLER);
    usleep( 500000);    // 500ms
    m_eLIB3A_AFMode = LIB3A_AF_MODE_CALIBRATION;
    m_pIAfAlgo->setAFMode( m_eLIB3A_AFMode);

    usleep( 500000);    // 500ms
    while( !isFocusFinish())
    {
        usleep( 30000); // 30ms
        i4TimeOutCnt++;
        if( i4TimeOutCnt>2000)
        {
            break;
        }
    }

    getDebugInfo( rAFDebugInfo);
    pAFCaliData->i4Gap = (MINT32)rAFDebugInfo.Tag[3].u4FieldValue;

    for( MINT32 i=0; i<512; i++)
    {
        if( rAFDebugInfo.Tag[i+4].u4FieldValue != 0)
        {
            pAFCaliData->i8Vlu[i] = (MINT64)rAFDebugInfo.Tag[i+4].u4FieldValue;  // need fix it
            pAFCaliData->i4Num = i+1;
        }
        else
        {
            break;
        }
    }

    pAFCaliData->i4BestPos = m_sAFOutput.i4AFBestPos;

    MY_LOG( "[AFCaliData] Num = %d", pAFCaliData->i4Num);
    MY_LOG( "[AFCaliData] Gap = %d", pAFCaliData->i4Gap);

    for( MINT32 i=0; i<pAFCaliData->i4Num; i++)
    {
        MY_LOG( "[AFCaliData] Vlu %d = %lld", i, pAFCaliData->i8Vlu[i]);
    }

    MY_LOG( "[AFCaliData] Pos = %d", pAFCaliData->i4BestPos);

    setAFMode( AF_MODE_AFS, AF_MGR_CALLER);
    *a_pOutLen = sizeof(MINT32);

    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPAFSetFocusRange( MVOID *a_pFocusRange)
{
    MY_LOG( "%s", __FUNCTION__);
    FOCUS_RANGE_T *pFocusRange = (FOCUS_RANGE_T *)a_pFocusRange;

    m_sNVRam.rFocusRange = *pFocusRange;

    if( m_pMcuDrv)
    {
        m_pMcuDrv->setMCUInfPos( m_sNVRam.rFocusRange.i4InfPos,m_i4CurrSensorDev);
        m_pMcuDrv->setMCUMacroPos( m_sNVRam.rFocusRange.i4MacroPos,m_i4CurrSensorDev);
    }

    if( m_pIAfAlgo)
    {
        m_pIAfAlgo->setAFParam( m_sAFParam, getAFConfig(), m_sNVRam.rAFNVRAM);
    }

    MY_LOG( "[Inf Pos] = %d", m_sNVRam.rFocusRange.i4InfPos);
    MY_LOG( "[Marco Pos] = %d", m_sNVRam.rFocusRange.i4MacroPos);
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPAFGetFocusRange( MVOID *a_pFocusRange, MUINT32 *a_pOutLen)
{
    MY_LOG( "%s", __FUNCTION__);
    FOCUS_RANGE_T *pFocusRange = (FOCUS_RANGE_T *)a_pFocusRange;

    *pFocusRange = m_sNVRam.rFocusRange;
    *a_pOutLen   = sizeof(FOCUS_RANGE_T);

    MY_LOG( "[Inf Pos] = %d",   pFocusRange->i4InfPos);
    MY_LOG( "[Marco Pos] = %d", pFocusRange->i4MacroPos);
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPAFGetNVRAMParam( MVOID *a_pAFNVRAM, MUINT32 *a_pOutLen)
{
    MY_LOG( "%s", __FUNCTION__);

    NVRAM_LENS_PARA_STRUCT *pAFNVRAM = reinterpret_cast<NVRAM_LENS_PARA_STRUCT*>(a_pAFNVRAM);

    MINT32 err = NvBufUtil::getInstance().getBufAndRead( CAMERA_NVRAM_DATA_LENS, m_i4CurrSensorDev, (void*&)m_ptrNVRam);

    if( err!=0)
    {
        MY_ERR( "CCTOPAFGetNVRAMParam NvBufUtil get buf fail! \n");
    }

    memcpy( &m_sNVRam, m_ptrNVRam, sizeof(NVRAM_LENS_PARA_STRUCT));

    *pAFNVRAM  = m_sNVRam;
    *a_pOutLen = sizeof(NVRAM_LENS_PARA_STRUCT);

    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPAFApplyNVRAMParam( MVOID *a_pAFNVRAM)
{
    MY_LOG( "%s", __FUNCTION__);

    NVRAM_LENS_PARA_STRUCT *ptrNewNVRAM = (NVRAM_LENS_PARA_STRUCT *)a_pAFNVRAM;

    memcpy( &m_sNVRam, ptrNewNVRAM, sizeof(NVRAM_LENS_PARA_STRUCT));

    MY_LOG( "Apply to Phone[Thres Main] %d", m_sNVRam.rAFNVRAM.sAF_Coef.i4THRES_MAIN);
    MY_LOG( "Apply to Phone[Thres Sub] %d",  m_sNVRam.rAFNVRAM.sAF_Coef.i4THRES_MAIN);
    MY_LOG( "Apply to Phone[HW_TH] %d",      m_sNVRam.rAFNVRAM.sAF_TH.i4HW_TH[0]);
    MY_LOG( "Apply to Phone[Statgain] %d",   m_sNVRam.rAFNVRAM.i4StatGain);

    if( m_pIAfAlgo)
    {
        m_pIAfAlgo->setAFParam( m_sAFParam, getAFConfig(), m_sNVRam.rAFNVRAM);
    }
    if( m_pMcuDrv)
    {
        m_pMcuDrv->setMCUInfPos( m_sNVRam.rFocusRange.i4InfPos,m_i4CurrSensorDev);
        m_pMcuDrv->setMCUMacroPos( m_sNVRam.rFocusRange.i4MacroPos,m_i4CurrSensorDev);
    }
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPAFSaveNVRAMParam()
{
    MY_LOG( "%s", __FUNCTION__);

    MY_LOG( "WriteNVRAM from Phone[Thres Main]%d\n", m_sNVRam.rAFNVRAM.sAF_Coef.i4THRES_MAIN);
    MY_LOG( "WriteNVRAM from Phone[Thres Sub]%d\n",  m_sNVRam.rAFNVRAM.sAF_Coef.i4THRES_MAIN);
    MY_LOG( "WriteNVRAM from Phone[HW_TH]%d\n",      m_sNVRam.rAFNVRAM.sAF_TH.i4HW_TH[0]);
    MY_LOG( "WriteNVRAM from Phone[Statgain]%d\n",   m_sNVRam.rAFNVRAM.i4StatGain);

    NVRAM_LENS_PARA_STRUCT *ptrNVRAMBuf;

    MINT32 err1 = NvBufUtil::getInstance().getBufAndRead(CAMERA_NVRAM_DATA_LENS, m_i4CurrSensorDev, (void*&)ptrNVRAMBuf);

    memcpy( ptrNVRAMBuf, &m_sNVRam, sizeof(NVRAM_LENS_PARA_STRUCT));

    MINT32 err2 = NvBufUtil::getInstance().write( CAMERA_NVRAM_DATA_LENS, m_i4CurrSensorDev);

    if( err1!=0)
        return err1;
    else
        return err2;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPAFGetFV( MVOID *a_pAFPosIn, MVOID *a_pAFValueOut, MUINT32 *a_pOutLen)
{
    ACDK_AF_POS_T *pAFPos   = (ACDK_AF_POS_T *) a_pAFPosIn;
    ACDK_AF_VLU_T *pAFValue = (ACDK_AF_VLU_T *) a_pAFValueOut;

    MY_LOG( "%s", __FUNCTION__);

    pAFValue->i4Num = pAFPos->i4Num;
    setAFMode( AF_MODE_AFS, AF_MGR_CALLER);
    usleep( 500000); // 500ms
    setAFMode( AF_MODE_MF, AF_MGR_CALLER);

    for( MINT32 i=0; i<pAFValue->i4Num; i++)
    {
        setMFPos(pAFPos->i4Pos[i], AF_MGR_CALLER);
        usleep( 500000); // 500ms
        pAFValue->i8Vlu[i] = TransStatProfileToAlgo( m_sAFInput.sStatProfile);
        MY_LOG( "[FV]pos = %d, value = %lld\n", pAFPos->i4Pos[i], pAFValue->i8Vlu[i]);
    }

    setAFMode(AF_MODE_AFS, AF_MGR_CALLER);
    *a_pOutLen = sizeof(ACDK_AF_VLU_T);
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPAFEnable()
{
    MY_LOG( "%s", __FUNCTION__);
    m_i4EnableAF = 1;
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPAFDisable()
{
    MY_LOG( "%s", __FUNCTION__);
    m_i4EnableAF = 0;
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::CCTOPAFGetEnableInfo( MVOID *a_pEnableAF, MUINT32 *a_pOutLen)
{
    MY_LOG( "%s %d", __FUNCTION__, m_i4EnableAF);

    MINT32 *pEnableAF = (MINT32 *)a_pEnableAF;
    *pEnableAF = m_i4EnableAF;
    *a_pOutLen = sizeof(MINT32);
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::triggerAF( MUINT32 u4Caller)
{
    if( u4Caller==AF_MGR_CALLER)
        MY_LOG( "%s  Dev %d : lib_afmode %d\n", __FUNCTION__, m_i4CurrSensorDev, m_eLIB3A_AFMode);
    else
        MY_LOG( "cmd-%s  Dev %d : lib_afmode %d\n", __FUNCTION__, m_i4CurrSensorDev, m_eLIB3A_AFMode);

    if( m_pIAfAlgo)
        m_pIAfAlgo->trigger();

    m_sAFOutput.i4IsAFDone = MFALSE;
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::setAFMode( MINT32 eAFMode, MUINT32 u4Caller)
{
    if( m_eCurrAFMode==eAFMode)
    {
        return S_AF_OK;
    }

    if( m_bIsFullScan==MTRUE)
    {
        return S_AF_OK;
    }

    /**
     *  Before new af mode is set, setting af area command is sent.
     */
    if( u4Caller==AF_MGR_CALLER)
    {
        MY_LOG( "%s Dev %d : ctl_afmode [%d]",
                __FUNCTION__,
                m_i4CurrSensorDev,
                eAFMode);
    }
    else
    {
        MY_LOG( "cmd-%s Dev %d : ctl_afmode [%d]",
                __FUNCTION__,
                m_i4CurrSensorDev,
                eAFMode);
    }

    m_eCurrAFMode = eAFMode;

    LIB3A_AF_MODE_T preLib3A_AfMode = m_eLIB3A_AFMode;
    switch( m_eCurrAFMode)
    {
    case 0 : /*MTK_CONTROL_AF_MODE_OFF*/
        m_eLIB3A_AFMode = LIB3A_AF_MODE_MF; /*API2:The auto-focus routine does not control the lens. Lens  is controlled by the application.*/
        break;
    case 1 : /*MTK_CONTROL_AF_MODE_AUTO*/
        m_eLIB3A_AFMode = LIB3A_AF_MODE_AFS;
        break;
    case 2 : /*MTK_CONTROL_AF_MODE_MACRO*/
        m_eLIB3A_AFMode = LIB3A_AF_MODE_MACRO;
        break;
    case 3 : /*MTK_CONTROL_AF_MODE_CONTINUOUS_VIDEO*/
        m_eLIB3A_AFMode = LIB3A_AF_MODE_AFC_VIDEO;
        break;
    case 4 : /*MTK_CONTROL_AF_MODE_CONTINUOUS_PICTURE*/
        m_eLIB3A_AFMode = LIB3A_AF_MODE_AFC;
        break;
    case 5 : /*MTK_CONTROL_AF_MODE_EDOF*/
        m_eLIB3A_AFMode = LIB3A_AF_MODE_OFF;
        break;
    default :
        m_eLIB3A_AFMode = LIB3A_AF_MODE_AFS;
        break;
    }

    if( m_eLIB3A_AFMode==LIB3A_AF_MODE_AFS)
    {
        /**
         *  flow control:
         *  For normal TAF flow, AF HAL got both auto mode and AF region information.
         *  Setting m_bIsROICmd and m_bLatchROI as MTRUE here to wait trigger searching..
         */
        m_bIsROICmd = MTRUE;
        m_bLatchROI = MTRUE;
    }
    else
    {
        //reset : if Auto mode command is got without trigger command m_bIsROICmd is keep TRUE.
        m_bIsROICmd = MFALSE;
        m_bLatchROI = MFALSE;
    }

    // log only.
    if( u4Caller==AF_MGR_CALLER)
    {
        MY_LOG( "%s Dev %d : lib_afmode %d->%d\n",
                __FUNCTION__,
                m_i4CurrSensorDev,
                preLib3A_AfMode,
                m_eLIB3A_AFMode);
    }
    else
    {
        MY_LOG( "cmd-%s Dev %d : lib_afmode %d->%d\n",
                __FUNCTION__,
                m_i4CurrSensorDev,
                preLib3A_AfMode,
                m_eLIB3A_AFMode);
    }

    //Set mode to hybrid AF algorithm
    if( m_pIAfAlgo)
        m_pIAfAlgo->setAFMode(m_eLIB3A_AFMode);
    else
        MY_LOG( "Null m_pIAfAlgo\n");


    //update parameters and status.
    UpdateState( EVENT_CMD_CHANGE_MODE);
    m_NonValInfo.i4AFMode     = m_eCurrAFMode;
    m_NonValInfo.eLib3AAFMode = m_eLIB3A_AFMode;
    return S_AF_OK;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
NS3A::EAfState_T AfMgr::UpdateStateAutoMode( NS3A::EAfState_T &eInCurSate, AF_EVENT_T &sInEvent)
{
    NS3A::EAfState_T NewState = NS3A::E_AF_INACTIVE;
    switch( sInEvent)
    {
    case EVENT_CMD_CHANGE_MODE :
        NewState = NS3A::E_AF_INACTIVE;
        break;

    case EVENT_CMD_AUTOFOCUS :
        if(      eInCurSate == NS3A::E_AF_INACTIVE          ) NewState = NS3A::E_AF_ACTIVE_SCAN;
        else if( eInCurSate == NS3A::E_AF_FOCUSED_LOCKED    ) NewState = NS3A::E_AF_ACTIVE_SCAN;
        else if( eInCurSate == NS3A::E_AF_NOT_FOCUSED_LOCKED) NewState = NS3A::E_AF_ACTIVE_SCAN;
        else                                                  NewState = eInCurSate; /*Should be at E_AF_ACTIVE_SCAN*/
        break;

    case EVENT_CMD_AUTOFOCUS_CANCEL :
        if(      eInCurSate == NS3A::E_AF_ACTIVE_SCAN       ) NewState = NS3A::E_AF_INACTIVE;
        else if( eInCurSate == NS3A::E_AF_FOCUSED_LOCKED    ) NewState = NS3A::E_AF_INACTIVE;
        else if( eInCurSate == NS3A::E_AF_NOT_FOCUSED_LOCKED) NewState = NS3A::E_AF_INACTIVE;
        else                                                  NewState = eInCurSate; /*Should be at E_AF_INACTIVE*/
        break;

    case EVENT_SEARCHING_START :
        NewState = NS3A::E_AF_ACTIVE_SCAN ;
        break;

    case EVENT_SEARCHING_END :
        if( eInCurSate == NS3A::E_AF_ACTIVE_SCAN)
        {
            m_bIsROICmd=MFALSE;
            if( m_i4IsFocused==1) NewState = NS3A::E_AF_FOCUSED_LOCKED;
            else                  NewState = NS3A::E_AF_NOT_FOCUSED_LOCKED;
        }
        else
        {
            NewState = eInCurSate;
        }


        break;

    default : /*Should not be happened*/
        NewState = eInCurSate;
        break;
    }

    return NewState;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
NS3A::EAfState_T AfMgr::UpdateStateContinuousPicMode( NS3A::EAfState_T &eInCurSate, AF_EVENT_T &sInEvent)
{
    NS3A::EAfState_T NewState = NS3A::E_AF_INACTIVE;
    switch( sInEvent)
    {
    case EVENT_CMD_CHANGE_MODE :
        if( m_i4IsMonitorFV_CurState==MTRUE)
        {
            NewState = UpdateState( EVENT_SEARCHING_END);
        }
        else
        {
            NewState = NS3A::E_AF_INACTIVE;
        }

        //Reset parameters
        m_bLock = MFALSE;
        break;

    case EVENT_CMD_AUTOFOCUS :
        if(      eInCurSate == NS3A::E_AF_INACTIVE          ) NewState = NS3A::E_AF_NOT_FOCUSED_LOCKED;
        else if( eInCurSate == NS3A::E_AF_PASSIVE_SCAN      ) NewState = eInCurSate;
        else if( eInCurSate == NS3A::E_AF_PASSIVE_FOCUSED   ) NewState = NS3A::E_AF_FOCUSED_LOCKED;
        else if( eInCurSate == NS3A::E_AF_PASSIVE_UNFOCUSED ) NewState = NS3A::E_AF_NOT_FOCUSED_LOCKED;
        else if( eInCurSate == NS3A::E_AF_FOCUSED_LOCKED    ) NewState = NS3A::E_AF_FOCUSED_LOCKED;
        else if( eInCurSate == NS3A::E_AF_NOT_FOCUSED_LOCKED) NewState = NS3A::E_AF_NOT_FOCUSED_LOCKED;
        else                                                  NewState = eInCurSate; /*Should not be happened*/

        if( eInCurSate != NS3A::E_AF_PASSIVE_SCAN)
        {
            //cancel and lock AF.
            m_pIAfAlgo->cancel();
            m_pIAfAlgo->lock();
        }
        else
        {
            MY_LOG( "lockAF until searching done");
        }
        m_bLock = MTRUE;
        break;

    case EVENT_CMD_AUTOFOCUS_CANCEL :
        if( (eInCurSate == NS3A::E_AF_PASSIVE_SCAN) ||
                (eInCurSate == NS3A::E_AF_FOCUSED_LOCKED) ||
                (eInCurSate == NS3A::E_AF_NOT_FOCUSED_LOCKED) )
        {
            NewState = NS3A::E_AF_INACTIVE;

            //unlock AF and trigger one time searching..
            m_pIAfAlgo->unlock();
            MY_LOG( "unlockAF");
        }
        else
        {
            NewState = eInCurSate;
        }
        m_bLock = MFALSE;
        break;

    case EVENT_SEARCHING_START :
        if(      eInCurSate == NS3A::E_AF_INACTIVE         ) NewState = NS3A::E_AF_PASSIVE_SCAN;
        else if( eInCurSate == NS3A::E_AF_PASSIVE_FOCUSED  ) NewState = NS3A::E_AF_PASSIVE_SCAN;
        else if( eInCurSate == NS3A::E_AF_PASSIVE_UNFOCUSED) NewState = NS3A::E_AF_PASSIVE_SCAN;
        else                                                 NewState = eInCurSate;  /*Should not be happened*/
        break;

    case EVENT_SEARCHING_END :
        if( m_bLock==MTRUE)
        {
            m_bIsROICmd=MFALSE; /*Only change ROI to trigger searching. Need reset once searching done.*/

            if( m_i4IsFocused==1) NewState = NS3A::E_AF_FOCUSED_LOCKED;
            else                  NewState = NS3A::E_AF_NOT_FOCUSED_LOCKED;

            //lock
            m_pIAfAlgo->cancel();
            m_pIAfAlgo->lock();
            MY_LOG( "lockAF");
        }
        else
        {
            if( m_i4IsFocused==1) NewState = NS3A::E_AF_PASSIVE_FOCUSED;
            else                  NewState = NS3A::E_AF_PASSIVE_UNFOCUSED;
        }
        break;

    default : /*Should not be happened*/
        NewState = eInCurSate;
        break;
    }

    return NewState;

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
NS3A::EAfState_T AfMgr::UpdateStateContinuousVideoMode( NS3A::EAfState_T &eInCurSate, AF_EVENT_T &sInEvent)
{
    NS3A::EAfState_T NewState = NS3A::E_AF_INACTIVE;
    switch( sInEvent)
    {
    case EVENT_CMD_CHANGE_MODE :
        if( m_i4IsMonitorFV_CurState==MTRUE)
        {
            NewState = UpdateState( EVENT_SEARCHING_END);
        }
        else
        {
            NewState = NS3A::E_AF_INACTIVE;
        }

        //Reset parameters
        m_bLock = MFALSE;
        break;

    case EVENT_CMD_AUTOFOCUS :
        if(      eInCurSate == NS3A::E_AF_INACTIVE          ) NewState = NS3A::E_AF_NOT_FOCUSED_LOCKED;
        else if( eInCurSate == NS3A::E_AF_PASSIVE_SCAN      ) NewState = NS3A::E_AF_NOT_FOCUSED_LOCKED;
        else if( eInCurSate == NS3A::E_AF_PASSIVE_FOCUSED   ) NewState = NS3A::E_AF_FOCUSED_LOCKED;
        else if( eInCurSate == NS3A::E_AF_PASSIVE_UNFOCUSED ) NewState = NS3A::E_AF_NOT_FOCUSED_LOCKED;
        else if( eInCurSate == NS3A::E_AF_FOCUSED_LOCKED    ) NewState = NS3A::E_AF_FOCUSED_LOCKED;
        else if( eInCurSate == NS3A::E_AF_NOT_FOCUSED_LOCKED) NewState = NS3A::E_AF_NOT_FOCUSED_LOCKED;
        else                                                  NewState = eInCurSate; /*Should not be happened*/

        //cancel and lock AF.
        m_pIAfAlgo->cancel();
        m_pIAfAlgo->lock();
        m_bLock = MTRUE;
        break;

    case EVENT_CMD_AUTOFOCUS_CANCEL :
        if( (eInCurSate == NS3A::E_AF_PASSIVE_SCAN) ||
                (eInCurSate == NS3A::E_AF_FOCUSED_LOCKED) ||
                (eInCurSate == NS3A::E_AF_NOT_FOCUSED_LOCKED) )
        {
            NewState = NS3A::E_AF_INACTIVE;
            // unlock AF and trigger one time searching..
            m_pIAfAlgo->unlock();
        }
        else
        {
            NewState = eInCurSate;
        }

        m_bLock = MFALSE;
        break;

    case EVENT_SEARCHING_START :
        if(      eInCurSate == NS3A::E_AF_INACTIVE         ) NewState = NS3A::E_AF_PASSIVE_SCAN;
        else if( eInCurSate == NS3A::E_AF_PASSIVE_FOCUSED  ) NewState = NS3A::E_AF_PASSIVE_SCAN;
        else if( eInCurSate == NS3A::E_AF_PASSIVE_UNFOCUSED) NewState = NS3A::E_AF_PASSIVE_SCAN;
        else                                                 NewState = eInCurSate;
        break;

    case EVENT_SEARCHING_END :
        if( m_bLock==MTRUE)
        {
            m_bIsROICmd = MFALSE; /*Only change ROI to trigger searching. Need reset once searching done.*/

            NewState = eInCurSate;
        }
        else
        {
            if( m_i4IsFocused==1) NewState = NS3A::E_AF_PASSIVE_FOCUSED;
            else                  NewState = NS3A::E_AF_PASSIVE_UNFOCUSED;
        }
        break;

    default : /*Should not be happened*/
        NewState = eInCurSate;
        break;
    }

    return NewState;


}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
NS3A::EAfState_T AfMgr::UpdateStateOFFMode( NS3A::EAfState_T &eInCurSate, AF_EVENT_T &sInEvent)
{
    NS3A::EAfState_T NewState = NS3A::E_AF_INACTIVE;
    switch( sInEvent)
    {
    case EVENT_CMD_CHANGE_MODE :
    case EVENT_CMD_AUTOFOCUS :
    case EVENT_CMD_AUTOFOCUS_CANCEL :
    case EVENT_SEARCHING_START :
    case EVENT_SEARCHING_END :
    default :
        NewState = NS3A::E_AF_INACTIVE;
        break;
    }

    return NewState;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
NS3A::EAfState_T AfMgr::UpdateStateMFMode( NS3A::EAfState_T &eInCurSate, AF_EVENT_T &sInEvent)
{
    NS3A::EAfState_T NewState = NS3A::E_AF_INACTIVE;
    switch( sInEvent)
    {
    case EVENT_CMD_CHANGE_MODE :
    case EVENT_CMD_AUTOFOCUS :
    case EVENT_CMD_AUTOFOCUS_CANCEL :
    case EVENT_SEARCHING_START :
    case EVENT_SEARCHING_END :
    default :
        NewState = NS3A::E_AF_INACTIVE;
        break;
    }

    return NewState;

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
NS3A::EAfState_T AfMgr::UpdateStateFullScanMode( NS3A::EAfState_T &eInCurSate, AF_EVENT_T &sInEvent)
{
    NS3A::EAfState_T NewState = NS3A::E_AF_INACTIVE;
    switch( sInEvent)
    {
    case EVENT_CMD_CHANGE_MODE :
        NewState = NS3A::E_AF_INACTIVE;
        break;

    case EVENT_CMD_AUTOFOCUS :
        if(      eInCurSate == NS3A::E_AF_INACTIVE          ) NewState = NS3A::E_AF_ACTIVE_SCAN;
        else if( eInCurSate == NS3A::E_AF_FOCUSED_LOCKED    ) NewState = NS3A::E_AF_ACTIVE_SCAN;
        else if( eInCurSate == NS3A::E_AF_NOT_FOCUSED_LOCKED) NewState = NS3A::E_AF_ACTIVE_SCAN;
        else                                                  NewState = eInCurSate; /*Should be at E_AF_ACTIVE_SCAN*/
        break;

    case EVENT_CMD_AUTOFOCUS_CANCEL :
        if(      eInCurSate == NS3A::E_AF_ACTIVE_SCAN       ) NewState = NS3A::E_AF_INACTIVE;
        else if( eInCurSate == NS3A::E_AF_FOCUSED_LOCKED    ) NewState = NS3A::E_AF_INACTIVE;
        else if( eInCurSate == NS3A::E_AF_NOT_FOCUSED_LOCKED) NewState = NS3A::E_AF_INACTIVE;
        else                                                  NewState = eInCurSate; /*Should be at E_AF_INACTIVE*/
        break;

    case EVENT_SEARCHING_START :
        NewState = NS3A::E_AF_ACTIVE_SCAN ;
        break;

    case EVENT_SEARCHING_END :
        if( eInCurSate == NS3A::E_AF_ACTIVE_SCAN)
        {
            m_bIsROICmd=MFALSE;
            if( m_i4IsFocused==1) NewState = NS3A::E_AF_FOCUSED_LOCKED;
            else                  NewState = NS3A::E_AF_NOT_FOCUSED_LOCKED;
        }
        else
        {
            NewState = eInCurSate;
        }


        break;

    default : /*Should not be happened*/
        NewState = eInCurSate;
        break;
    }

    return NewState;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
NS3A::EAfState_T AfMgr::UpdateState( AF_EVENT_T sInEvent)
{
    NS3A::EAfState_T NewState;

    switch( sInEvent)
    {
    case EVENT_CMD_CHANGE_MODE:
        MY_LOG( "EVENT_CMD_CHANGE_MODE");
        break;
    case EVENT_CMD_AUTOFOCUS:
        MY_LOG( "EVENT_CMD_AUTOFOCUS");
        break;
    case EVENT_CMD_AUTOFOCUS_CANCEL:
        MY_LOG( "EVENT_CMD_AUTOFOCUS_CANCEL");
        break;
    case EVENT_SEARCHING_START:
        MY_LOG( "EVENT_SEARCHING_START");
        break;
    case EVENT_SEARCHING_END:
        MY_LOG( "EVENT_SEARCHING_END");
        break;
    default:
        MY_LOG( "EVENT_WRONG");
        break;
    }

    switch( m_eLIB3A_AFMode)
    {
    case LIB3A_AF_MODE_MF :
        NewState = UpdateStateMFMode( m_eAFState, sInEvent);
        break;
    case LIB3A_AF_MODE_AFS :
        NewState = UpdateStateAutoMode( m_eAFState, sInEvent);
        break;
    case LIB3A_AF_MODE_MACRO :
        NewState = UpdateStateAutoMode( m_eAFState, sInEvent);
        break;
    case LIB3A_AF_MODE_AFC_VIDEO :
        NewState = UpdateStateContinuousVideoMode( m_eAFState, sInEvent);
        break;
    case LIB3A_AF_MODE_AFC :
        NewState = UpdateStateContinuousPicMode( m_eAFState, sInEvent);
        break;
    case LIB3A_AF_MODE_OFF :
        NewState = UpdateStateOFFMode( m_eAFState, sInEvent);
        break;
    case LIB3A_AF_MODE_FULLSCAN :
        NewState = UpdateStateFullScanMode( m_eAFState, sInEvent);
        break;
    default :
        NewState = UpdateStateOFFMode( m_eAFState, sInEvent);
        break;
    }

    if( m_eAFState!=NewState)
    {
        MY_LOG( "%s Dev %d : %d->%d",
                __FUNCTION__,
                m_i4CurrSensorDev,
                m_eAFState,
                NewState);
    }

    m_eAFState = NewState;

    return NewState;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
NS3A::EAfState_T AfMgr::getAFState()
{
    if( m_eAFStatePre!=m_eAFState)
    {
        MY_LOG( "cmd-%s Dev %d : %d->%d",
                __FUNCTION__,
                m_i4CurrSensorDev,
                m_eAFStatePre,
                m_eAFState);
    }

    m_eAFStatePre = m_eAFState;

    return m_eAFState;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::getFocusAreaResult( android::Vector<MINT32> &vecOutPos, android::Vector<MUINT8> &vecOutRes, MINT32 &i4OutSzW, MINT32 &i4OutSzH)
{
    //TODO:
    vecOutPos.clear(); /*The order is (x1)->(y1)->(x2)->(y2)*/
    vecOutRes.clear();
    i4OutSzW = 0;
    i4OutSzH = 0;
#if 0
    MINT32 num = m_sAFOutput.sROIStatus.i4TotalNum;
    if( 0<=num && num<=MAX_MULTI_ZONE_WIN_NUM)
    {
        MINT32 x,y;
        MUINT8 res;
        for( MUINT32 i=0; i<num; i++)
        {
            x   = m_sAFOutput.sROIStatus.sROI[i].i4X + m_sAFOutput.sROIStatus.sROI[i].i4W/2;
            y   = m_sAFOutput.sROIStatus.sROI[i].i4Y + m_sAFOutput.sROIStatus.sROI[i].i4H/2;
            res = m_sAFOutput.sROIStatus.sROI[i].i4Info;
            vecOutPos.push_back(x);
            vecOutPos.push_back(y);
            vecOutRes.push_back(res);
        }
        i4OutSzW = m_sAFOutput.sROIStatus.sROI[i].i4W;
        i4OutSzH = m_sAFOutput.sROIStatus.sROI[i].i4H;
    }

#else

    MINT32 _w = m_sArea_Center.i4W/3;
    MINT32 _h = m_sArea_Center.i4H/3;

    MINT32 __w = _w/2;
    MINT32 __h = _h/2;

    for( MUINT32 j=0; j<3; j++)
    {
        for( MUINT32 i=0; i<3; i++)
        {
            //center of ROI.
            MINT32 x = m_sArea_Center.i4X + i*_w + __w;
            vecOutPos.push_back(x);
            MINT32 y = m_sArea_Center.i4Y + j*_h + __h;
            vecOutPos.push_back(y);
            //result.
            MUINT8 idx = i+j;
            MUINT8 res = (idx==0 || idx==2 || idx==4) ? 1 : 0;
            vecOutRes.push_back(res);
        }
    }

    //size.
    i4OutSzW = _w;
    i4OutSzH = _h;
#endif

    MY_LOG_IF( m_i4DgbLogLv,
               "%s %d %d",
               __FUNCTION__,
               vecOutPos.size(),
               vecOutRes.size());

    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::setAFArea( CameraFocusArea_T &sInAFArea)
{
    MRESULT ret = E_3A_ERR;

    /*TG base coordinate*/
    MY_LOG_IF( m_i4DgbLogLv,
               "cmd-%s [Cnt]%d [L]%d [R]%d [U]%d [B]%d",
               __FUNCTION__,
               sInAFArea.u4Count,
               sInAFArea.rAreas[0].i4Left,
               sInAFArea.rAreas[0].i4Right,
               sInAFArea.rAreas[0].i4Top,
               sInAFArea.rAreas[0].i4Bottom);

    if( 1<=sInAFArea.u4Count)
    {
        //boundary check.
        if( (sInAFArea.rAreas[0].i4Left<sInAFArea.rAreas[0].i4Right ) &&
                (sInAFArea.rAreas[0].i4Top <sInAFArea.rAreas[0].i4Bottom) &&
                (0<=sInAFArea.rAreas[0].i4Left && sInAFArea.rAreas[0].i4Right<=m_i4TGSzW) &&
                (0<=sInAFArea.rAreas[0].i4Top  && sInAFArea.rAreas[0].i4Bottom<=m_i4TGSzH))
        {
            AREA_T roi = AREA_T( sInAFArea.rAreas[0].i4Left,
                                 sInAFArea.rAreas[0].i4Top,
                                 sInAFArea.rAreas[0].i4Right  - sInAFArea.rAreas[0].i4Left,
                                 sInAFArea.rAreas[0].i4Bottom - sInAFArea.rAreas[0].i4Top,
                                 AF_MARK_NONE);

            if( memcmp( &roi, &m_sArea_APCmd, sizeof(AREA_T))!=0)
            {
                //store command.
                m_sArea_APCmd = roi;

                /**
                 *  flow control:
                 *  For normal TAF flow, AF HAL got both auto mode and AF region information.
                 *  So both m_bIsROICmd and m_bLatchROI will be set MTURE under normal TAF flow.
                 *  If TAF is processed under continuous mode, setting m_bIsROICmd and m_bLatchROI as MTRUE here to force trigger searching.
                 */
                m_bIsROICmd = MTRUE;
                m_bLatchROI = MTRUE;

                MY_LOG( "cmd-%s Got ROI changed cmd. [Cnt]%d [L]%d [R]%d [U]%d [B]%d -> [W]%d [H]%d [X]%d [Y]%d",
                        __FUNCTION__,
                        sInAFArea.u4Count,
                        sInAFArea.rAreas[0].i4Left,
                        sInAFArea.rAreas[0].i4Right,
                        sInAFArea.rAreas[0].i4Top,
                        sInAFArea.rAreas[0].i4Bottom,
                        m_sArea_APCmd.i4W,
                        m_sArea_APCmd.i4H,
                        m_sArea_APCmd.i4X,
                        m_sArea_APCmd.i4Y);

                // control laser AF touch behavior.
                if( m_pLaserDrv )
                {
                    m_sAFInput.sLaserInfo.i4AfWinPosX = ( sInAFArea.rAreas[0].i4Left + sInAFArea.rAreas[0].i4Right ) / 2;
                    m_sAFInput.sLaserInfo.i4AfWinPosY = ( sInAFArea.rAreas[0].i4Top  + sInAFArea.rAreas[0].i4Bottom ) / 2;

                    if( abs(m_sAFInput.sLaserInfo.i4AfWinPosX) < LASER_TOUCH_REGION_W && abs(m_sAFInput.sLaserInfo.i4AfWinPosY) < LASER_TOUCH_REGION_H )
                    {
                        if( (m_eLIB3A_AFMode == LIB3A_AF_MODE_AFS) || (m_eLIB3A_AFMode == LIB3A_AF_MODE_MACRO) )
                        {
                            m_sAFInput.sLaserInfo.i4AfWinPosCnt++;
                        }
                    }
                    else
                    {
                        m_sAFInput.sLaserInfo.i4AfWinPosCnt = 0;
                    }
                }

                ret = S_AF_OK;
            }
            else
            {
                MY_LOG_IF( m_i4DgbLogLv, "cmd-%s same ROI cmd", __FUNCTION__);
            }
        }
        else
        {
            //command is not vaild, using center window.
            MY_LOG_IF( m_i4DgbLogLv, "cmd-%s ROI cmd is not correct", __FUNCTION__);
        }
    }
    else
    {
        //command is not vaild, using center window.
        MY_LOG_IF( m_i4DgbLogLv, "cmd-%s cnt=0!!", __FUNCTION__);
    }

    return ret;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::setCamMode( MINT32 a_eCamMode)
{
    MY_LOG( "[setCamMode]%d", a_eCamMode);
    m_sAFInput.i4IsVDO = FALSE;

    if( a_eCamMode==eAppMode_VideoMode)
    {
        m_sAFInput.i4IsVDO = TRUE;
        m_sAFInput.i4IsZSD = FALSE;
    }
    else
    {
        m_sAFInput.i4IsVDO = FALSE;
    }

    if( m_sAFParam.i4AFS_MODE==2)  //AF auto mode always don't AFC
    {
        m_sAFInput.i4IsVDO= FALSE;
    }

    //------------------------------------------- Set Depth AF Info ----------------------------------------
    if( m_i4CurrLensId!=0xffff)
    {
        MINT32 AFtbl_Num;
        MINT32 AFtbl_Marco;
        MINT32 AFtbl_Inf;

        if( (m_sAFInput.i4IsVDO==TRUE) && (m_eLIB3A_AFMode == LIB3A_AF_MODE_AFC_VIDEO) )
        {
            AFtbl_Num   = m_sNVRam.rAFNVRAM.sVAFC_Coef.sTABLE.i4NormalNum;
            AFtbl_Inf   = m_sNVRam.rAFNVRAM.sVAFC_Coef.sTABLE.i4Offset + m_sNVRam.rAFNVRAM.sVAFC_Coef.sTABLE.i4Pos[0];
            AFtbl_Marco = m_sNVRam.rAFNVRAM.sVAFC_Coef.sTABLE.i4Offset + m_sNVRam.rAFNVRAM.sVAFC_Coef.sTABLE.i4Pos[AFtbl_Num-1];
        }
        else
        {
            AFtbl_Num   = m_sNVRam.rAFNVRAM.sAF_Coef.sTABLE.i4NormalNum;
            AFtbl_Inf   = m_sNVRam.rAFNVRAM.sAF_Coef.sTABLE.i4Offset + m_sNVRam.rAFNVRAM.sAF_Coef.sTABLE.i4Pos[0];
            AFtbl_Marco = m_sNVRam.rAFNVRAM.sAF_Coef.sTABLE.i4Offset + m_sNVRam.rAFNVRAM.sAF_Coef.sTABLE.i4Pos[AFtbl_Num-1];
        }

        m_sDAF_TBL.af_dac_min = AFtbl_Inf;
        m_sDAF_TBL.af_dac_max = AFtbl_Marco;
    }

    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::SetCropRegionInfo( MUINT32 u4XOffset, MUINT32 u4YOffset, MUINT32 u4Width, MUINT32 u4Height, MUINT32 u4Caller)
{
    if( (u4XOffset>=0 && u4XOffset<m_i4TGSzW) &&
            (u4YOffset>=0 && u4XOffset<m_i4TGSzH) &&
            (u4Width>0 && u4XOffset<m_i4TGSzW) &&
            (u4Height>0 && u4XOffset<m_i4TGSzH) &&
            (u4XOffset+u4Width<=m_i4TGSzW) &&
            (u4YOffset+u4Height<=m_i4TGSzH) )
    {
        // set crop region information and update center ROI automatically.
        if( m_sCropRegionInfo.i4X != (MINT32)u4XOffset ||
                m_sCropRegionInfo.i4Y != (MINT32)u4YOffset ||
                m_sCropRegionInfo.i4W != (MINT32)u4Width   ||
                m_sCropRegionInfo.i4H != (MINT32)u4Height )
        {
            if( u4Caller==AF_MGR_CALLER)
                MY_LOG( "%s [w]%d, [h]%d, [x]%d, [y]%d\n", __FUNCTION__, u4Width, u4Height, u4XOffset, u4YOffset);
            else
                MY_LOG( "cmd-%s [w]%d, [h]%d, [x]%d, [y]%d\n", __FUNCTION__, u4Width, u4Height, u4XOffset, u4YOffset);

            m_sCropRegionInfo.i4X = (MINT32)u4XOffset;
            m_sCropRegionInfo.i4Y = (MINT32)u4YOffset;
            m_sCropRegionInfo.i4W = (MINT32)u4Width;
            m_sCropRegionInfo.i4H = (MINT32)u4Height;

            //calculate zoom information : 1X-> 100, 2X->200, ...
            MUINT32 dzfX = 100*m_i4TGSzW/m_sCropRegionInfo.i4W;
            MUINT32 dzfY = 100*m_i4TGSzH/m_sCropRegionInfo.i4H;
            //Should be the same.
            m_i4DZFactor = dzfX<dzfY ? dzfX : dzfY;
            MY_LOG( "[dzX]%d, [dzY]%d, [Fac] %d", dzfX, dzfY, m_i4DZFactor);

            //Accroding to crop region, updating center ROI coordinate automatically
            UpdateCenterROI( m_sArea_Center);
        }
        else
        {
            MY_LOG_IF( m_i4DgbLogLv, "%s same cmd", __FUNCTION__);
        }
    }
    else
    {
        MY_LOG_IF( m_i4DgbLogLv, "%s not valid", __FUNCTION__);
    }
    return S_AF_OK;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::ApplyZoomEffect( AREA_T &sOutAFRegion)
{
    // zoom effect of ROI is configured from customized parameter
    m_i4DzWinCfg = (MUINT32)m_sNVRam.rAFNVRAM.i4ZoomInWinChg;

    // error check
    if(      m_i4DzWinCfg<1) m_i4DzWinCfg = 1;
    else if( 4<m_i4DzWinCfg) m_i4DzWinCfg = 4;

    // scale should be same as m_i4DZFactor
    m_i4DzWinCfg *= 100;
    MY_LOG( "%s [DZ]%d, [Cfg]%d",
            __FUNCTION__,
            m_i4DZFactor,
            m_i4DzWinCfg);


    if( m_i4DzWinCfg==400)
    {
        // i4WinCfg=4-> DigZoomFac>4, AF win align digital effect.
    }
    else
    {
        /**
         * i4WinCfg = 1~3
         * i4WinCfg = 1 : DigZoomFac>1, AF win no change
         * i4WinCfg = 2 : DigZoomFac>2, AF win no change
         * i4WinCfg = 3 : DigZoomFac>3, AF win no change
         */
        if( m_i4DzWinCfg <= m_i4DZFactor) /* fix to upper bound */
        {

            //scale up window.
            AREA_T scaledWin;
            scaledWin.i4W = sOutAFRegion.i4W*m_i4DZFactor/m_i4DzWinCfg;
            scaledWin.i4H = sOutAFRegion.i4H*m_i4DZFactor/m_i4DzWinCfg;
            scaledWin.i4X = (sOutAFRegion.i4X + sOutAFRegion.i4W/2) -  scaledWin.i4W/2;
            scaledWin.i4Y = (sOutAFRegion.i4Y + sOutAFRegion.i4H/2) -  scaledWin.i4H/2;

            MY_LOG( "%s [W]%d [H]%d [X]%d [Y]%d -> [W]%d [H]%d [X]%d [Y]%d",
                    __FUNCTION__,
                    scaledWin.i4W,
                    scaledWin.i4H,
                    scaledWin.i4X,
                    scaledWin.i4Y,
                    sOutAFRegion.i4W,
                    sOutAFRegion.i4H,
                    sOutAFRegion.i4X,
                    sOutAFRegion.i4Y );

            sOutAFRegion = scaledWin;

        }
        else /* (i4DzFactor < i4WinCfg*100), AF win change aligning to digital zoom factor */
        {
            //MY_LOG("[applyZoomInfo] <bound2DZ, DZ=%d, Bound=%d \n",i4DzFactor,i4WinCfg*100);
        }
    }


    //MY_LOG("[applyZoomInfo] >bound2fix, DZ=%d, Bound=%d\n",i4DzFactor,i4WinCfg*100);

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::setOTFDInfo( MVOID* sInROIs, MINT32 i4Type)
{
    MRESULT ret=E_3A_ERR;

    /*TG base coordinate*/
    MtkCameraFaceMetadata *ptrWins = (MtkCameraFaceMetadata *)sInROIs;

    //prepare information and set to hybrid AF
    AF_AREA_T sAreaInfo;
    sAreaInfo.i4Score  = i4Type!=0 ? ptrWins->faces->score : 0;  /*for algorithm to check input is FD or object, 0:face, 100:object*/
    sAreaInfo.i4Count  = 0;
    sAreaInfo.sRect[0] = AREA_T( 0, 0, 0, 0, AF_MARK_NONE);

    if( ptrWins!=NULL)
    {
        if( ptrWins->number_of_faces!=0)
        {
            if( ptrWins->faces!=NULL)
            {
                //TODO : currently always get the first ROI for focusing.
                // rect => 0:left, 1:top, 2:right, 3:bottom
                MINT32 i4X = ptrWins->faces[0].rect[0];
                MINT32 i4Y = ptrWins->faces[0].rect[1];
                MINT32 i4W = ptrWins->faces[0].rect[2] - ptrWins->faces[0].rect[0];
                MINT32 i4H = ptrWins->faces[0].rect[3] - ptrWins->faces[0].rect[1];
                // scale OTFD ROI
                MINT32 i4scale = m_sNVRam.rAFNVRAM.i4FDWinPercent&0xFFFF;
                if( i4scale > 150)
                {
                    i4scale = 150;
                }
                else if( i4scale < 0)
                {
                    i4scale = 100;
                }
                // scaling
                MINT32 scaledW = i4W*i4scale/100;
                MINT32 scaledH = i4H*i4scale/100;
                MINT32 scaledX = (i4X+i4W/2) - scaledW/2;
                MINT32 scaledY = (i4Y+i4H/2) - scaledH/2;

                if( (scaledW!=0) || (scaledY!=0))
                {
                    // OTFD data is valid.
                    sAreaInfo.i4Count  = 1;
                    sAreaInfo.sRect[0] = AREA_T( scaledX, scaledY, scaledW, scaledH, AF_MARK_NONE);
                    //latch last vaild FD information.
                    m_sArea_OTFD = sAreaInfo.sRect[0];

                    MY_LOG( "[%s]cnt:%d, type %d, scale:%d, [X]%d [Y]%d [W]%d [H]%d -> [X]%d [Y]%d [W]%d [H]%d",
                            __FUNCTION__,
                            sAreaInfo.i4Count,
                            sAreaInfo.i4Score,
                            i4scale,
                            i4X,
                            i4Y,
                            i4W,
                            i4H,
                            scaledX,
                            scaledY,
                            scaledW,
                            scaledH);

                    ret = S_AF_OK;
                }
                else
                {
                    MY_LOG("[%s] data is not valid", __FUNCTION__);
                }
            }
            else
            {
                MY_LOG("[%s] data is NULL ptr", __FUNCTION__);
            }
        }
        else
        {
            MY_LOG("[%s] num 0", __FUNCTION__);
        }
    }
    else
    {
        MY_LOG("[%s] Input NULL ptr", __FUNCTION__);
    }

    m_pIAfAlgo->setFDWin( sAreaInfo);
    return ret;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::setFDInfo( MVOID *a_sFaces)
{
    MRESULT ret = E_3A_ERR;

    if ( m_pIAfAlgo!=NULL)
    {
        ret = setOTFDInfo( a_sFaces, 0);
    }
    else
    {
        MY_LOG( "[%s] Null algo ptr", __FUNCTION__);
    }

    return ret;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::setOTInfo( MVOID *a_sOT)
{
    MRESULT ret = E_3A_ERR;

    if ( m_pIAfAlgo!=NULL)
    {
        ret = setOTFDInfo( a_sOT, 1);
    }
    else
    {
        MY_LOG( "[%s] Null algo ptr", __FUNCTION__);
    }

    return ret;
}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::getAFMaxAreaNum()
{
    IHalSensorList* const pIHalSensorList = IHalSensorList::get();
    SensorStaticInfo rSensorStaticInfo;
    switch( m_i4CurrSensorDev)
    {
    case ESensorDev_Main:
        pIHalSensorList->querySensorStaticInfo( NSCam::SENSOR_DEV_MAIN, &rSensorStaticInfo);
        break;
    case ESensorDev_Sub:
        pIHalSensorList->querySensorStaticInfo( NSCam::SENSOR_DEV_SUB, &rSensorStaticInfo);
        break;
    case ESensorDev_MainSecond:
        pIHalSensorList->querySensorStaticInfo( NSCam::SENSOR_DEV_MAIN_2, &rSensorStaticInfo);
        break;
    default:
        MY_ERR( "Invalid sensor device: %d", m_i4CurrSensorDev);
    }
    m_i4CurrSensorId = rSensorStaticInfo.sensorDevID;
    MCUDrv::lensSearch( m_i4CurrSensorDev, m_i4CurrSensorId);
    m_i4CurrLensId = MCUDrv::getCurrLensID( m_i4CurrSensorDev);
    MY_LOG( "[lens][SensorDev]0x%04x, [SensorId]0x%04x, [CurrLensId]0x%04x", m_i4CurrSensorDev, m_i4CurrSensorId, m_i4CurrLensId);

    if( m_i4CurrLensId==0xFFFF)
    {
        MY_LOG( "[getAFMaxAreaNum][AF disable]%d", 0);
        return 0;
    }
    else
    {
        if( m_i4EnableAF==0)
        {
            MY_ERR( "Open AF driver fail!");
            return 0;
        }
        else
        {
            MY_LOG( "[getAFMaxAreaNum]%d", AF_WIN_NUM_SPOT);
            return AF_WIN_NUM_SPOT;
        }
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::getMaxLensPos()
{
    if( m_pMcuDrv)
    {
        mcuMotorInfo MotorInfo;
        m_pMcuDrv->getMCUInfo( &MotorInfo,m_i4CurrSensorDev);
        MY_LOG( "[getMaxLensPos]%d", (MINT32)MotorInfo.u4MacroPosition);
        return (MINT32)MotorInfo.u4MacroPosition;
    }
    else
    {
        MY_LOG( "[getMaxLensPos]m_pMcuDrv NULL");
        return 0;
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::getMinLensPos()
{
    if( m_pMcuDrv)
    {
        mcuMotorInfo MotorInfo;
        m_pMcuDrv->getMCUInfo( &MotorInfo,m_i4CurrSensorDev);
        MY_LOG( "[getMinLensPos]%d", (MINT32)MotorInfo.u4InfPosition);
        return (MINT32)MotorInfo.u4InfPosition;
    }
    else
    {
        MY_LOG( "[getMinLensPos]m_pMcuDrv NULL");
        return 0;
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::getAFBestPos()
{
    return m_sAFOutput.i4AFBestPos;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::getAFPos()
{
    return m_sAFOutput.i4AFPos;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::getAFStable()
{
    return m_sAFOutput.i4IsAFDone;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::getLensMoving()
{
    return m_sAFInput.sLensInfo.bIsMotorMoving;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::getAFTableOffset()
{
    if( m_eLIB3A_AFMode==LIB3A_AF_MODE_AFC_VIDEO)
        return m_sNVRam.rAFNVRAM.sVAFC_Coef.sTABLE.i4Offset;
    else
        return m_sNVRam.rAFNVRAM.sAF_Coef.sTABLE.i4Offset;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::getAFTableMacroIdx()
{
    if( m_eLIB3A_AFMode == LIB3A_AF_MODE_AFC_VIDEO)
        return m_sNVRam.rAFNVRAM.sVAFC_Coef.sTABLE.i4NormalNum;
    else
        return m_sNVRam.rAFNVRAM.sAF_Coef.sTABLE.i4NormalNum;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::getAFTableIdxNum()
{
    return AF_TABLE_NUM;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID* AfMgr::getAFTable()
{
    if( m_eLIB3A_AFMode == LIB3A_AF_MODE_AFC_VIDEO)
        return (MVOID*)m_sNVRam.rAFNVRAM.sVAFC_Coef.sTABLE.i4Pos;

    else
        return (MVOID*)m_sNVRam.rAFNVRAM.sAF_Coef.sTABLE.i4Pos;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::setMFPos( MINT32 a_i4Pos, MUINT32 u4Caller)
{
    if( u4Caller==AF_MGR_CALLER)
    {
        MY_LOG( "%s Dev %d : %d",
                __FUNCTION__,
                m_i4CurrSensorDev,
                a_i4Pos);
    }
    else
    {
        MY_LOG_IF( m_i4DgbLogLv,
                   "cmd-%s Dev %d : %d",
                   __FUNCTION__,
                   m_i4CurrSensorDev,
                   a_i4Pos);
    }



    if( (m_eLIB3A_AFMode == LIB3A_AF_MODE_MF) &&
            (m_sAFOutput.i4AFPos != a_i4Pos) &&
            (0<=a_i4Pos) &&
            (a_i4Pos<=1023))
    {
        if( u4Caller==AF_MGR_CALLER)
        {
            MY_LOG( "%s set MF pos (%d)->(%d)",
                    __FUNCTION__,
                    m_sAFOutput.i4AFPos,
                    a_i4Pos);
        }
        else
        {
            MY_LOG( "cmd-%s set MF pos (%d)->(%d)",
                    __FUNCTION__,
                    m_sAFOutput.i4AFPos,
                    a_i4Pos);
        }

        m_i4MFPos = a_i4Pos;

        if( m_pIAfAlgo)
        {
            m_pIAfAlgo->setMFPos( m_i4MFPos);
            m_pIAfAlgo->trigger();
        }
        else
        {
            MY_LOG( "Null m_pIAfAlgo");
        }
    }
    else
    {
        if( u4Caller==AF_MGR_CALLER)
        {
            MY_LOG( "%s skip set MF pos (%d)->(%d), lib_afmode(%d)",
                    __FUNCTION__,
                    m_sAFOutput.i4AFPos,
                    a_i4Pos,
                    m_eLIB3A_AFMode);
        }
        else
        {
            MY_LOG_IF( m_i4DgbLogLv,
                       "cmd-%s skip set MF pos (%d)->(%d), lib_afmode(%d)",
                       __FUNCTION__,
                       m_sAFOutput.i4AFPos,
                       a_i4Pos,
                       m_eLIB3A_AFMode);
        }
    }
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::setFullScanstep( MINT32 a_i4Step)
{
    //if (m_i4FullScanStep != a_i4Step)
    {
        MY_LOG("[setFullScanstep]%d\n", a_i4Step);

        m_i4FullScanStep = a_i4Step;

        if( m_i4FullScanStep>0) /* Step > 0 , set Full Scan Mode */
        {
            m_bIsFullScan = MTRUE;
            m_sAFInput.i4FullScanStep = m_i4FullScanStep;
            m_eLIB3A_AFMode = LIB3A_AF_MODE_FULLSCAN;
            m_pIAfAlgo->setAFMode(m_eLIB3A_AFMode);
        }
        else
        {
            m_bIsFullScan = MFALSE;
            m_sAFInput.i4FullScanStep = 1;
        }
    }
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AF_FULL_STAT_T AfMgr::getFLKStat()
{
    AF_FULL_STAT_T sFullStat;
    memset( &sFullStat, 0, sizeof( AF_FULL_STAT_T));

    // auto detecting flk is always on
    sFullStat.bValid = MTRUE;

    MY_LOG_IF( (m_i4DgbLogLv&1),
               "[%s] FLK_bValid %d",
               sFullStat.bValid);

    return sFullStat;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::getAFRefWin( CameraArea_T &rWinSize)
{
    rWinSize.i4Left   = 0;
    rWinSize.i4Right  = 0;
    rWinSize.i4Top    = 0;
    rWinSize.i4Bottom = 0;

#if 0
    //The coordinate is -1000~1000
    if (m_i4EnableAF == 0)
    {
        MY_LOG("[getAFRefWin] disableAF");
        rWinSize.i4Left   = 0;
        rWinSize.i4Right  = 0;
        rWinSize.i4Top    = 0;
        rWinSize.i4Bottom = 0;
        return;
    }
    rWinSize.i4Left   = m_CameraFocusArea.rAreas[0].i4Left;
    rWinSize.i4Right  = m_CameraFocusArea.rAreas[0].i4Right;
    rWinSize.i4Top    = m_CameraFocusArea.rAreas[0].i4Top;
    rWinSize.i4Bottom = m_CameraFocusArea.rAreas[0].i4Bottom;
#endif
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::setAE2AFInfo( AE2AFInfo_T &rAEInfo)
{

    if (m_i4EnableAF == 0)
    {
        m_sAFInput.i8GSum = 50;
        m_sAFInput.i4ISO = 100;
        m_sAFInput.i4IsAEStable = 1;
        m_sAFInput.i4SceneLV = 80;
        return;
    }

    MY_LOG_IF( m_i4DgbLogLv,
               "cmd-%s Dev %d",
               __FUNCTION__,
               m_i4CurrSensorDev);

    m_sAFInput.i8GSum         = rAEInfo.iYvalue; /* [TODO] :: check should be remove or not*/
    m_sAFInput.i4ISO          = rAEInfo.i4ISO;
    m_sAFInput.i4IsAEStable   = ((rAEInfo.i4IsAEStable==1)||(rAEInfo.i4IsAELocked==1)) ? 1: 0;
    m_sAFInput.i4SceneLV      = rAEInfo.i4SceneLV;
    m_sAFInput.i4ShutterValue = rAEInfo.ishutterValue;
    //memcpy(m_aeBlockV, rAEInfo.aeBlockV,25);

    m_sAFInput.i4IsFlashFrm       = rAEInfo.i4IsFlashFrm;
    m_sAFInput.i4AEBlockAreaYCnt  = rAEInfo.i4AEBlockAreaYCnt;
    m_sAFInput.pAEBlockAreaYvalue = rAEInfo.pAEBlockAreaYvalue;

    if( m_i4DgbLogLv)
    {
        MY_LOG_IF( m_i4DgbLogLv, "GSum %lld, ISO %d",      m_sAFInput.i8GSum, m_sAFInput.i4ISO);
        MY_LOG_IF( m_i4DgbLogLv, "AEStb %d, %d, %d",       m_sAFInput.i4IsAEStable, rAEInfo.i4IsAEStable, rAEInfo.i4IsAELocked);
        MY_LOG_IF( m_i4DgbLogLv, "SceneLV %d, Shutter %d", rAEInfo.i4SceneLV, rAEInfo.ishutterValue);
        MY_LOG_IF( m_i4DgbLogLv, "Flash %d",               m_sAFInput.i4IsFlashFrm);
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::ConfigHybridAFMode()
{
    m_sAFInput.i4HybridAFMode = 0;

    //PDAF
    if( (m_i4PDSensorType==1) && (m_PDBuf_Type==EPDBuf_Raw))
    {
        m_sAFInput.i4HybridAFMode |= 2;  //2'b 0010
    }
    else if( (m_i4PDSensorType==2) && (m_PDBuf_Type==EPDBuf_VC))
    {
        m_sAFInput.i4HybridAFMode |= 2;  //2'b 1010
    }
    else if( (m_i4PDSensorType==2) && (m_PDBuf_Type==EPDBuf_VC_Open))
    {
        m_sAFInput.i4HybridAFMode |= 10; //2'b 1010
    }

    //Laser AF
    if( m_pLaserDrv)
    {
        m_sAFInput.i4HybridAFMode |= 4;
    }

    MY_LOG_IF( m_i4DgbLogLv, "%s HybridAFMode : %x", __FUNCTION__, m_sAFInput.i4HybridAFMode);

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::HybridAFPreprocessing()
{
    /* for reference */
    MINT32 EnablePD = ( ( m_PDBuf_Type!= EPDBuf_NotDef ) && ( m_i4PDSensorType!=0 ) );

    MINT32 i4ISO        =  m_sAFInput.i4ISO;
    MINT32 i4IsAEStable = m_sAFInput.i4IsAEStable;
    AREA_T LastAFArea   = m_sArea_Focusing;
    MUINT8 AEBlockV[25];

    memcpy( AEBlockV, m_aeBlockV, 25);

    /* changed parameter */
    MINT32   i = 0;
    MINT32   PDOut_numRes = 0;
    MUINT16 *pPDAF_DAC;
    MUINT16 *pPDAF_Conf;

    if( EnablePD)
    {
        PDOut_numRes = m_i4PDResNum;
        pPDAF_DAC = m_sAFInput.i4PDafDacIndex;
        pPDAF_Conf = m_sAFInput.i4PDafConfidence;
    }

    if( m_i4DgbLogLv)
    {
        for( i=0; i<PDOut_numRes; i++)
        {
            MY_LOG_IF( EnablePD, "%s PD%d (%d, %d)", __FUNCTION__, i, pPDAF_DAC[i], pPDAF_Conf[i]);
        }
    }

    /* TODO */
    MY_LOG_IF( m_i4DgbLogLv, "%s Dev %d, NA, ", __FUNCTION__, m_i4CurrSensorDev);


    if( m_i4DgbLogLv)
    {
        for( i=0; i<PDOut_numRes; i++)
        {
            MY_LOG_IF( EnablePD, "%s PD%d (%d, %d)", __FUNCTION__, i, pPDAF_DAC[i], pPDAF_Conf[i]);
        }
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
AREA_T& AfMgr::SelROIToFocusing( AF_OUTPUT_T &sInAFInfo)
{
    MY_LOG_IF( m_i4DgbLogLv,
               "%s Dev %d",
               __FUNCTION__,
               m_i4CurrSensorDev);

    MY_LOG_IF( m_i4DgbLogLv,
               "ROI-C [X]%d [Y]%d [W]%d [H]%d",
               m_sArea_Center.i4X,
               m_sArea_Center.i4Y,
               m_sArea_Center.i4W,
               m_sArea_Center.i4H);

    MY_LOG_IF( m_i4DgbLogLv,
               "ROI-A [X]%d [Y]%d [W]%d [H]%d",
               m_sArea_APCmd.i4X,
               m_sArea_APCmd.i4Y,
               m_sArea_APCmd.i4W,
               m_sArea_APCmd.i4H);

    MY_LOG_IF( m_i4DgbLogLv,
               "ROI-O [X]%d [Y]%d [W]%d [H]%d",
               m_sArea_OTFD.i4X,
               m_sArea_OTFD.i4Y,
               m_sArea_OTFD.i4W,
               m_sArea_OTFD.i4H);


    MY_LOG_IF( m_i4DgbLogLv,
               "ROI-H [X]%d [Y]%d [W]%d [H]%d",
               m_sArea_HW.i4X,
               m_sArea_HW.i4Y,
               m_sArea_HW.i4W,
               m_sArea_HW.i4H);



    if( (m_i4ROISel_PreState != sInAFInfo.i4ROISel) ||
            (m_i4IsMonitorFV_PreState!=m_i4IsMonitorFV_CurState) ||
            (m_bIsROICmd == TRUE)||
            (m_i4DgbLogLv))
    {
        MY_LOG( "%s [Status] %d->%d, %d, %d, %d",
                __FUNCTION__,
                m_i4IsMonitorFV_PreState,
                m_i4IsMonitorFV_CurState,
                m_bLatchROI,
                m_bIsROICmd,
                sInAFInfo.i4ROISel);

    }

    if( sInAFInfo.i4ROISel==AF_ROI_SEL_NONE)
    {
        /**
         *  Do nothing
         *  This case is just happened after af is inited.
         *  Wait algo to check using FD or center ROI to do focusing.
         *  Should get i4IsMonitorFV==TRUE. i4IsMonitorFV will be FALSE when ROI is selected.
         */
    }
    else if( m_bIsROICmd==MTRUE) /* Got changing ROI command from host. Should be trigger searching.*/
    {
        /**
         *   force using the new ROI which is sent from host, and do one time searching :
         *   @LIB3A_AF_MODE_AFS -> wiait autofocus command.
         *   @LIB3A_AF_MODE_AFC_VIDEO, LIB3A_AF_MODE_AFC ->  focuse to trigger searching by switching af mode to auto mode in AF HAL.
         */

        //Searching done.
        if( m_i4IsMonitorFV_PreState!=m_i4IsMonitorFV_CurState)
        {
            if( m_i4IsMonitorFV_CurState==MTRUE)
            {
                //force to switch back to continuous mode once searching done when changing ROI at continuous mode.
                if( (m_eLIB3A_AFMode==LIB3A_AF_MODE_AFC_VIDEO) || (m_eLIB3A_AFMode==LIB3A_AF_MODE_AFC))
                {
                    MY_LOG("%s Seaching done. Back to continuous mode.", __FUNCTION__);
                    m_pIAfAlgo->setAFMode(m_eLIB3A_AFMode);
                }
            }
        }

        //Latch ROI to do searching.
        if( m_bLatchROI==MTRUE)
        {
            m_sArea_Focusing = m_sArea_APCmd;
            MY_LOG("%s [CMD] %d[X]%d [Y]%d [W]%d [H]%d",
                   __FUNCTION__,
                   sInAFInfo.i4ROISel,
                   m_sArea_Focusing.i4X,
                   m_sArea_Focusing.i4Y,
                   m_sArea_Focusing.i4W,
                   m_sArea_Focusing.i4H);

            //apply zoom information.
            ApplyZoomEffect( m_sArea_Focusing);
            //used to control select AF ROI at AFS mode.
            m_bLatchROI=MFALSE;
            //force to trigger searching when changing ROI at continuous mode.
            if( (m_eLIB3A_AFMode==LIB3A_AF_MODE_AFC_VIDEO) || (m_eLIB3A_AFMode==LIB3A_AF_MODE_AFC))
            {
                MY_LOG( "%s Force trigger searching", __FUNCTION__);
                m_pIAfAlgo->setAFMode(LIB3A_AF_MODE_AFS);
                m_pIAfAlgo->trigger();
            }

        }
    }
    else if( m_i4IsMonitorFV_PreState!=m_i4IsMonitorFV_CurState) /* Without got changing ROI command from host, and need to do searching*/
    {
        //ready to do searching
        if( m_i4IsMonitorFV_CurState==MFALSE)
        {
            switch( sInAFInfo.i4ROISel)
            {
            case AF_ROI_SEL_NONE :
                //This case cannot be happened.
                break;
            case AF_ROI_SEL_AP :
                //This case cannot be happened.
                m_sArea_Focusing = m_sArea_APCmd;
                break;
            case AF_ROI_SEL_OT :
            case AF_ROI_SEL_FD :
                m_sArea_Focusing = m_sArea_OTFD;
                break;
            case AF_ROI_SEL_CENTER :
            case AF_ROI_SEL_DEFAULT :
            default :
                m_sArea_Focusing = m_sArea_Center;
                break;
            }
            MY_LOG( "%s [SEL] %d[X]%d [Y]%d [W]%d [H]%d",
                    __FUNCTION__,
                    sInAFInfo.i4ROISel,
                    m_sArea_Focusing.i4X,
                    m_sArea_Focusing.i4Y,
                    m_sArea_Focusing.i4W,
                    m_sArea_Focusing.i4H);


            //apply zoom information.
            ApplyZoomEffect( m_sArea_Focusing);
        }
    }

    //update.
    m_i4ROISel_PreState = sInAFInfo.i4ROISel;

    return m_sArea_Focusing;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::doAF( MVOID *ptrInAFData)
{
    if( m_i4EnableAF == 0)
    {
        m_sAFOutput.i4IsAFDone = 1;
        m_sAFOutput.i4IsFocused = 0;
        m_sAFOutput.i4AFPos = 0;
        m_bRunPDEn = MTRUE;
        MY_LOG( "disableAF");
        return S_AF_OK;
    }

    MY_LOG_IF( m_i4DgbLogLv, "[%s]+", __FUNCTION__);

    MUINT32 i4curFrmNum = m_i4CurFrmNum;

    if( (m_sAFInput.i4IsVDO!=TRUE) || (m_sAFOutput.i4IsAFDone==FALSE ))
    {
        char value[PROPERTY_VALUE_MAX] = {'\0'};
        property_get( "debug.af_motor.disable",  value, "0");
        m_i4DbgMotorDisable = atoi(value);
        property_get( "debug.af_motor.position", value, "0");
        m_i4DbgMotorMPos = atoi(value);

    }
    //-------------------------------------------------------------------------------------------------------------
    // i4Coef[7] > 0: enable new scenechange , else: disable
    if( (m_sNVRam.rAFNVRAM.i4SceneMonitorLevel > 0) ||
            (m_sNVRam.rAFNVRAM.i4VdoSceneMonitorLevel > 0))
    {
        m_pIAfAlgo->setAEBlockInfo(m_aeBlockV, 25);
    }
    //-------------------------------------------------------------------------------------------------------------
    // i4Coef[5] == 1:enable SensorListener, 0: disable
    if(m_sNVRam.rAFNVRAM.i4SensorEnable > 0)
    {
        MY_LOG_IF( m_i4DgbLogLv,
                   "Acce En=%d preTS=%lld TS=%lld Info=(%d %d %d)",
                   m_bAcceEn,
                   gPreAcceTS,
                   gAcceTS,
                   gAcceInfo[0],
                   gAcceInfo[1],
                   gAcceInfo[2]);

        MY_LOG_IF( m_i4DgbLogLv,
                   "Gyro En=%d preTS=%lld TS=%lld Info=(%d %d %d)",
                   m_bGyroEn,
                   gPreGyroTS,
                   gGyroTS,
                   gGyroInfo[0],
                   gGyroInfo[1],
                   gGyroInfo[2]);

        if( m_bAcceEn && (gAcceTS!=gPreAcceTS) )
        {
            m_pIAfAlgo->setAcceSensorInfo( gAcceInfo, SENSOR_ACCE_SCALE);
        }
        else
        {
            MY_LOG_IF( m_i4DgbLogLv, "Acce InValid!");
            m_pIAfAlgo->setAcceSensorInfo(gAcceInfo, 0);    // set scale 0 means invalid to algo
        }
        gPreAcceTS = gAcceTS;
        if( m_bGyroEn && (gGyroTS!=gPreGyroTS) )
        {
            m_pIAfAlgo->setGyroSensorInfo( gGyroInfo, SENSOR_GYRO_SCALE);
        }
        else
        {
            MY_LOG_IF( m_i4DgbLogLv, "Gyro InValid!");
            m_pIAfAlgo->setGyroSensorInfo(gGyroInfo, 0);    // set scale 0 means invalid to algo
        }
        gPreGyroTS = gGyroTS;
    }
    //----------------------------------Prepare AF Algorithm input data------------------------------------------------
    //==========
    // depth AF
    //==========
    if( m_bDAFEn)
    {
        //Input Depth Info
        MUINT32 QueryDafTblIdx = m_next_query_FrmNum % DAF_TBL_QLEN;

        m_sDAF_TBL.curr_p1_frm_num = i4curFrmNum;

        if( m_sDAF_TBL.is_daf_run == 1)
        {
            // af_mgr -> af_algo
            m_sAFInput.i4CurrP1FrmNum = i4curFrmNum;
            if(m_next_query_FrmNum == 0xFFFFFFFF)
            {
                m_sAFInput.i4DafDacIndex = 0;
                m_sAFInput.i4DafConfidence = 0;
            }
            else
            {
                m_sAFInput.i4DafDacIndex   = m_sDAF_TBL.daf_vec[QueryDafTblIdx].daf_dac_index;
                m_sAFInput.i4DafConfidence = m_sDAF_TBL.daf_vec[QueryDafTblIdx].daf_confidence;
            }

            MY_LOG( "DAF--[Mode]%d [cp1#]%d [cp2#]%d [nextF#]%d [DafDac]%d [DafConf]%d [daf_dist]%d",
                    (MINT32)m_sAFInput.i4HybridAFMode,
                    (MINT32)m_sAFInput.i4CurrP1FrmNum,
                    (MINT32)m_sDAF_TBL.curr_p2_frm_num,
                    (MINT32)m_next_query_FrmNum,
                    (MINT32)m_sAFInput.i4DafDacIndex,
                    (MINT32)m_sAFInput.i4DafConfidence,
                    m_sDAF_TBL.daf_vec[QueryDafTblIdx].daf_distance);

            if( m_sDAF_TBL.daf_vec[QueryDafTblIdx].daf_confidence)
            {
                MY_LOG( "DAFAA-%d %d",
                        m_daf_distance,
                        m_sDAF_TBL.daf_vec[QueryDafTblIdx].daf_confidence);

                m_daf_distance = (MINT32)m_sDAF_TBL.daf_vec[QueryDafTblIdx].daf_distance;
            }
        }
    }
    //==========
    // LaserAF
    //==========
    if( m_pLaserDrv )
    {
        m_pLaserDrv->getLaserCurPos( &m_sAFInput.sLaserInfo.i4CurPosDAC, &m_sAFInput.sLaserInfo.i4CurPosDist);

        MY_LOG( "LaserData : %d - %d", m_sAFInput.sLaserInfo.i4CurPosDAC, m_sAFInput.sLaserInfo.i4CurPosDist);

        if( m_sAFInput.sLaserInfo.i4CurPosDAC>0)
        {
            m_sAFInput.sLaserInfo.i4Confidence = 80;
        }
        else
        {
            m_sAFInput.sLaserInfo.i4Confidence  = 20;
            m_sAFInput.sLaserInfo.i4AfWinPosCnt = 0;
        }

        //Touch AF : if the ROI isn't in the center, the laser data need to set low confidence.
        if( m_sArea_Focusing.i4Info==1) // 1 : TAF
        {
            if( abs(m_sAFInput.sLaserInfo.i4AfWinPosX) >= LASER_TOUCH_REGION_W ||
                    abs(m_sAFInput.sLaserInfo.i4AfWinPosY) >= LASER_TOUCH_REGION_H ||
                    m_sAFInput.sLaserInfo.i4AfWinPosCnt > 1 )
            {
                m_sAFInput.sLaserInfo.i4Confidence = 20;
                m_sAFInput.sLaserInfo.i4CurPosDAC = 0;
            }
        }
    }
    //==========
    //   PDAF
    //==========
    if( (m_PDBuf_Type!=EPDBuf_NotDef) && (m_i4PDSensorType!=0))
    {
        SPDOutputData_t PDOutput( m_i4PDAreaNum);
        MRESULT res = IPDMgr::getInstance().getPDTaskResult( m_i4CurrSensorDev, &PDOutput);
        if( res==S_3A_OK)
        {
            m_i4PDResNum = PDOutput.numRes;

            for(MINT32 i = 0 ; i < PDOutput.numRes ; i++)
            {
                m_sAFInput.i4PDafDacIndex[i]   = PDOutput.Res[i].Defocus;
                m_sAFInput.i4PDafConfidence[i] = (MUINT32)PDOutput.Res[i].DefocusConfidenceLevel;
                m_sAFInput.i4PDafConverge[i]   = PDOutput.Res[i].PhaseDifference;

                //Show pd result during searching.
                if( m_i4IsMonitorFV_CurState==MFALSE)
                {
                    MY_LOG_IF( m_i4DgbLogLv,
                               "PD Res %d : Target Pos %4d, PD %4d, CL %4d",
                               i,
                               m_sAFInput.i4PDafDacIndex[i],
                               m_sAFInput.i4PDafConverge[i],
                               m_sAFInput.i4PDafConfidence[i]);
                }
            }

        }

        //direct control
        m_sAFInput.i4PDPureRawfrm = 0x01&m_i4RawSel;
        m_i4RawSel = ~m_i4RawSel;

        if( res==E_AF_BUSY)
        {
            MY_LOG( "PD res not rdy");
        }
    }
    //---------------------------------------Statistic data information--------------------------------------------
    StatisticBufInfo *ptrStatInfo = reinterpret_cast<StatisticBufInfo *>( ptrInAFData);

    //---------------------------------------Run hybrid AF core flow---------------------------------------------
    if( ptrInAFData==NULL)
    {
        MY_ERR( "AFO fail");
    }
    else if( (ISP_MGR_AF_STAT_CONFIG_T::getInstance(static_cast<ESensorDev_T>(m_i4CurrSensorDev)).isHWRdy( ptrStatInfo->mMagicNumber)==MFALSE) &&
             (m_sAFOutput.i4ROISel!=AF_ROI_SEL_NONE))
    {
        /**
         * The condition AF_ROI_SEL_NONE is for the first time lunch camera only. [Ref] SelROIToFocusing
         */
        MY_LOG( "HW-Preparing");
    }
    else
    {
        //get current lens position.
        getLensInfo( m_sCurLensInfo);
        //Do AF is triggered when Vsync is came, so use previours lens information.
        m_sAFInput.sLensInfo = m_sCurLensInfo;
        //Got AF statistic from DMA buffer.
        ConvertDMABufToStat( m_sAFInput.sLensInfo.i4CurrentPos, ptrInAFData, m_sAFInput.sStatProfile);
        //Pre-processing input data for hybrid AF.
        HybridAFPreprocessing();
        //select focusing ROI.
        m_sAFInput.sAFArea.i4Count  = 1;
        m_sAFInput.sAFArea.sRect[0] = SelROIToFocusing( m_sAFOutput);
        //select hybrid af behavior.
        ConfigHybridAFMode();
        //Run algorithm
        m_pIAfAlgo->handleAF( m_sAFInput, m_sAFOutput);
        //Move lens position.
        m_i8MvLensTS = MoveLensTo( m_sAFOutput.i4AFPos);

        //----------------------------------AF Algorithm output data------------------------------------------------
        //==========
        // depth AF
        //==========
        if( m_bDAFEn)
        {
            MUINT32 CurDafTblIdx = i4curFrmNum % DAF_TBL_QLEN;

            m_sDAF_TBL.daf_vec[CurDafTblIdx].af_dac_index = m_sCurLensInfo.i4CurrentPos;

            if( m_sDAF_TBL.is_daf_run == 1)
            {
                m_sDAF_TBL.daf_vec[CurDafTblIdx].frm_mun        = i4curFrmNum;
                m_sDAF_TBL.daf_vec[CurDafTblIdx].is_learning    = m_sAFOutput.i4IsLearning;
                m_sDAF_TBL.daf_vec[CurDafTblIdx].is_querying    = m_sAFOutput.i4IsQuerying;
                m_sDAF_TBL.daf_vec[CurDafTblIdx].af_valid       = m_sAFOutput.i4AfValid;
                m_sDAF_TBL.daf_vec[CurDafTblIdx].af_dac_index   = m_sAFOutput.i4AfDacIndex;
                m_sDAF_TBL.daf_vec[CurDafTblIdx].af_confidence  = m_sAFOutput.i4AfConfidence;

                m_sDAF_TBL.daf_vec[CurDafTblIdx].af_win_start_x = (MUINT16)m_sArea_Focusing.i4X;
                m_sDAF_TBL.daf_vec[CurDafTblIdx].af_win_start_y = (MUINT16)m_sArea_Focusing.i4Y;
                m_sDAF_TBL.daf_vec[CurDafTblIdx].af_win_end_x   = (MUINT16)m_sArea_Focusing.i4X + m_sArea_Focusing.i4W;
                m_sDAF_TBL.daf_vec[CurDafTblIdx].af_win_end_y   = (MUINT16)m_sArea_Focusing.i4Y + m_sArea_Focusing.i4H;

                m_sDAF_TBL.daf_vec[CurDafTblIdx].daf_dac_index  = 0;
                m_sDAF_TBL.daf_vec[CurDafTblIdx].daf_confidence = 0;
                m_sDAF_TBL.daf_vec[CurDafTblIdx].daf_distance   = 0;

                m_sDAF_TBL.is_query_happen = m_sAFOutput.i4QueryFrmNum;

                m_next_query_FrmNum = m_sAFOutput.i4QueryFrmNum;

                MY_LOG( "DAF--[islrn]%d [isqry]%d [afVld]%d [af_dac]%d [afConf]%d [af_win]%d %d %d %d [nextF#]%d\n",
                        (MINT32)m_sDAF_TBL.daf_vec[CurDafTblIdx].is_learning,
                        (MINT32)m_sDAF_TBL.daf_vec[CurDafTblIdx].is_querying,
                        (MINT32)m_sDAF_TBL.daf_vec[CurDafTblIdx].af_valid,
                        (MINT32)m_sDAF_TBL.daf_vec[CurDafTblIdx].af_dac_index,
                        (MINT32)m_sDAF_TBL.daf_vec[CurDafTblIdx].af_confidence,
                        (MINT32)m_sDAF_TBL.daf_vec[CurDafTblIdx].af_win_start_x,
                        (MINT32)m_sDAF_TBL.daf_vec[CurDafTblIdx].af_win_start_y,
                        (MINT32)m_sDAF_TBL.daf_vec[CurDafTblIdx].af_win_end_x,
                        (MINT32)m_sDAF_TBL.daf_vec[CurDafTblIdx].af_win_end_y,
                        (MINT32)m_next_query_FrmNum);
            }
        }
    }

    //---------------------------------------Configure/update HW setting---------------------------------------
    MBOOL res = ConfigHWReg( m_sAFOutput.sAFStatConfig, m_sArea_HW, m_i4HWBlkNumX, m_i4HWBlkNumY);
    //New HW setting is applied.
    if( res==MTRUE || m_bRunPDEn==MFALSE)
    {
        /**
         * After ConfigHWReg is called, m_sAFOutput.sAFStatConfig will be changed to fit HW constraint.
         * The original HW setting is recorded in m_sHWCfg. [ref] ConfigHWReg function.
         */
        UpdatePDParam( m_sHWCfg.sRoi, m_sArea_HW);
    }
    //------------------------------------Update parameter for flow controlling----------------------------------
    m_i4IsMonitorFV_PreState = m_i4IsMonitorFV_CurState;
    m_i4IsMonitorFV_CurState = m_sAFOutput.i4IsMonitorFV;
    m_i4IsFocused = m_sAFOutput.i4IsFocused;

    //Event
    if(  m_i4IsMonitorFV_PreState!=m_i4IsMonitorFV_CurState)
    {
        if( m_i4IsMonitorFV_CurState==MFALSE)
        {
            UpdateState( EVENT_SEARCHING_START);
            MY_LOG( "_Searching_Start_");
        }
        else
        {
            UpdateState( EVENT_SEARCHING_END);
            MY_LOG( "_Searching_End_");
        }
    }

    MY_LOG_IF( m_i4DgbLogLv,
               "[%s] %d %d",
               __FUNCTION__,
               m_sAFOutput.i4IsMonitorFV,
               m_sAFOutput.i4IsAFDone);

    //---------------------------------------------------------------------------------------------------
    // set AF info
    AF_INFO_T sAFInfo;
    sAFInfo.i4AFPos = m_sAFOutput.i4AFPos;
    IspTuningMgr::getInstance().setAFInfo(m_i4CurrSensorDev, sAFInfo);

    MY_LOG_IF( m_i4DgbLogLv, "[%s]-", __FUNCTION__);
    return S_AF_OK;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::doPDVC( MVOID *ptrInVCData)
{
    MY_LOG_IF( m_i4DgbLogLv, "%s", __FUNCTION__);

    //return directly during camera is opening(init)
    if( (m_bRunPDEn == MFALSE) || (m_i4EnableAF!= 1))
    {
        return S_AF_OK;
    }

    {
        //mutex lock during init/uninit
        Mutex::Autolock lock( m_Lock);

        //return directly once af_mgr is uninitated
        if( m_bRunPDEn==MFALSE)
        {
            return S_AF_OK;
        }

        //return diretcly once PDAF is not support.
        if( m_i4PDSensorType!=2)
        {
            if( m_bPDVCTest)
            {
                //for engineer check only.
                MY_LOG_IF( m_i4DgbLogLv, "Debug virtual channel only : for engineer testing only!!");

                MUINT32 frmnum = m_i4CurFrmNum++;
                StatisticBufInfo *ptrPDdata = reinterpret_cast<StatisticBufInfo *>( ptrInVCData);

                char fileName[64];
                sprintf( fileName, "/sdcard/vc/ae%d.raw", frmnum++);
                FILE *fp = fopen( fileName, "w");
                if( NULL==fp)
                {
                    MY_ERR( "fail to open file to save img: %s", fileName);
                    MINT32 err = mkdir( "/sdcard/vc", S_IRWXU | S_IRWXG | S_IRWXO);
                    MY_LOG( "err = %d", err);
                    return S_AF_OK;
                }
                fwrite( reinterpret_cast<void *>(ptrPDdata->mVa), 1, ptrPDdata->mSize, fp);
                fclose( fp);
            }
            else
            {
                MY_LOG_IF( m_i4DgbLogLv,
                           "No PD task [Dev %d]: %d, %d",
                           m_i4CurrSensorDev,
                           m_i4EnableAF,
                           m_i4PDSensorType);
            }
            return S_AF_OK;
        }
        else
        {
            //store lens position.
            LENS_INFO_T lensInfo;
            lensInfo = m_sCurLensInfo;
            getLensInfo( lensInfo);
            if( 0<=lensInfo.i4CurrentPos && lensInfo.i4CurrentPos<=1023)
            {
                m_i4PreLensPos = m_i4CurLensPos;
                m_i4CurLensPos = lensInfo.i4CurrentPos;
            }
            else
            {
                return S_AF_OK;
            }

            MUINT32 frmnum = m_i4CurFrmNum++;
            StatisticBufInfo *ptrPDdata = reinterpret_cast<StatisticBufInfo *>( ptrInVCData);

            //allocate pd input data
            SPDInputData_t PDInput( m_i4PDAreaNum, ptrPDdata->mSize, reinterpret_cast<MUINT8 *>(ptrPDdata->mVa));

            PDInput.frmNum = frmnum;

            MINT64 timestamp = getTimeStamp();
            MY_LOG_IF( m_i4DgbLogLv,
                       "TS mvMCU->VC %3d, curPos %4d",
                       (MINT32)(timestamp-m_i8MvLensTS),
                       m_i4CurLensPos);

            PDInput.curLensPos   = m_i4CurLensPos;
            PDInput.XSizeOfImage = m_i4TGSzW;
            PDInput.YSizeOfImage = m_i4TGSzH;
            for( MUINT32 i=0; i<PDInput.numROI; i++)
            {
                PDInput.ROI[i].i4XStart = m_sArea_PD[i].i4X;
                PDInput.ROI[i].i4YStart = m_sArea_PD[i].i4Y;
                PDInput.ROI[i].i4XEnd = m_sArea_PD[i].i4X + m_sArea_PD[i].i4W;
                PDInput.ROI[i].i4YEnd = m_sArea_PD[i].i4Y + m_sArea_PD[i].i4H;
                PDInput.ROI[i].i4Info = m_sArea_PD[i].i4Info;

                MY_LOG_IF( m_i4DgbLogLv,
                           "[Core][PDAF]win %d [WinPos] %d, %d, %d, %d\n",
                           i,
                           PDInput.ROI[i].i4XStart,
                           PDInput.ROI[i].i4YStart,
                           PDInput.ROI[i].i4XEnd,
                           PDInput.ROI[i].i4YEnd);

            }
            IPDMgr::getInstance().postToPDTask( m_i4CurrSensorDev, &PDInput);
        }
    }
    return S_AF_OK;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::doPDO( MVOID *ptrInPDOData)
{
    //TODO:
    return S_AF_OK;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::UpdateCenterROI( AREA_T &sOutAreaCenter)
{
    MUINT32 scalex = 100;
    MUINT32 scaley = 100;

    if( m_bMZHostEn)
    {
        scalex = m_sNVRam.rAFNVRAM.i4MultiAFCoefs[0];
        scaley = m_sNVRam.rAFNVRAM.i4MultiAFCoefs[1];
    }
    else if( m_i4PDSensorType!=0)
    {
        scalex = m_sNVRam.rAFNVRAM.i4Coefs[31];
        scaley = m_sNVRam.rAFNVRAM.i4Coefs[32];
    }
    else
    {
        scalex = m_sNVRam.rAFNVRAM.i4SPOT_PERCENT_W;
        scaley = m_sNVRam.rAFNVRAM.i4SPOT_PERCENT_H;
    }

    MY_LOG( "%d %d %d %d %d %d",
            m_sNVRam.rAFNVRAM.i4MultiAFCoefs[0],
            m_sNVRam.rAFNVRAM.i4MultiAFCoefs[1],
            m_sNVRam.rAFNVRAM.i4Coefs[31],
            m_sNVRam.rAFNVRAM.i4Coefs[32],
            m_sNVRam.rAFNVRAM.i4SPOT_PERCENT_W,
            m_sNVRam.rAFNVRAM.i4SPOT_PERCENT_H);


    if(     100<scalex) scalex=100;
    else if( scalex<=0) scalex=15;

    if(     100<scaley) scaley=100;
    else if( scaley<=0) scaley=15;

    MY_LOG( "scale %d %d",
            scalex,
            scaley);

    MUINT32 croiw = m_sCropRegionInfo.i4W*scalex/100;
    MUINT32 croih = m_sCropRegionInfo.i4H*scaley/100;
    MUINT32 croix = m_sCropRegionInfo.i4X + (m_sCropRegionInfo.i4W-croiw)/2;
    MUINT32 croiy = m_sCropRegionInfo.i4Y + (m_sCropRegionInfo.i4H-croih)/2;

    //updateing.
    sOutAreaCenter = AREA_T( croix, croiy, croiw, croih, AF_MARK_NONE);

    MY_LOG( "ROI-C [X]%d [Y]%d [W]%d [H]%d",
            sOutAreaCenter.i4X,
            sOutAreaCenter.i4Y,
            sOutAreaCenter.i4W,
            sOutAreaCenter.i4H);

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::UpdatePDParam( AREA_T &sInArea, AREA_T &sInHWArea)
{
    MY_LOG_IF( m_i4DgbLogLv, "%s", __FUNCTION__);


    if( (m_PDBuf_Type!=EPDBuf_NotDef) && (m_i4PDSensorType!=0))
    {
        /**
         * Input -
         * sInArea      - for print log only (based on TG coordinate and without fit HW constraint).
         * sInHWArea - (based on TG coordinate and  fit HW constraint).
         */
        AREA_T PDRoi = sInHWArea;
        PDRoi.i4Info = 0;

        MRESULT res = m_pIAfAlgo->getFocusWindows(PDRoi, AF_PSUBWIN_NUM, &m_sArea_PD[0], &m_i4PDAreaNum);
        if( res!=S_3A_OK)
        {
            m_i4PDAreaNum = 1;
            m_sArea_PD[0] = sInHWArea;
            m_sArea_PD[0].i4Info = 1;
        }

        MY_LOG( "%s : [X]%d [Y]%d [W]%d [H]%d -> [X]%d [Y]%d [W]%d [H]%d",
                __FUNCTION__,
                sInArea.i4X,
                sInArea.i4Y,
                sInArea.i4W,
                sInArea.i4H,
                PDRoi.i4X,
                PDRoi.i4Y,
                PDRoi.i4W,
                PDRoi.i4H);

        // error check
        for( MINT32 i=0; i<m_i4PDAreaNum; i++)
        {
            if( (m_sArea_PD[i].i4W <= 0) ||
                    (m_sArea_PD[i].i4H <= 0) ||
                    (m_sArea_PD[i].i4X <  0) ||
                    (m_sArea_PD[i].i4Y <  0) )
            {
                m_i4PDAreaNum = 0;
            }
            MY_LOG( "%s : %d [X]%d [Y]%d [W]%d [H]%d",
                    __FUNCTION__,
                    i,
                    m_sArea_PD[i].i4X,
                    m_sArea_PD[i].i4Y,
                    m_sArea_PD[i].i4W,
                    m_sArea_PD[i].i4H);
        }

        if( m_i4PDAreaNum==0)
        {
            m_i4PDAreaNum = 1;
            m_sArea_PD[0] = sInArea;
        }

        m_bRunPDEn = MTRUE;
    }

}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::doPDBuffer( MVOID * buffer, MINT32 w,MINT32 h, MINT32 stride, MUINT32 i4FrmNum)
{
    m_pdaf_rawbuf=buffer;
    m_pdaf_w=w;
    m_pdaf_h=h;
    m_pdaf_stride=stride;
    m_pdaf_raw_frmnum = i4FrmNum;
    MY_LOG( "[doPDBuffer][%d] %d %d %d\n", m_pdaf_raw_frmnum, m_pdaf_w, m_pdaf_h, m_pdaf_stride);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::PDPureRawInterval ()
{
#ifdef PD_PURE_RAW_INTERVAL
    return PD_PURE_RAW_INTERVAL;
#else
    return 1;
#endif
}

MRESULT AfMgr::doPDTask()
{
    //return directly during camera is opening(init)
    if( m_bRunPDEn==MFALSE)
        return S_AF_OK;

    {
        static MUINT32 framCnt=0;
        //mutex lock during init/uninit
        Mutex::Autolock lock(m_Lock);

        //return directly once af_mgr is uninitated
        if( m_bRunPDEn==MFALSE)
            return S_AF_OK;

        if( (m_i4EnableAF == 0) || (m_i4PDSensorType!=1))
        {
            MY_LOG_IF( m_i4DgbLogLv, "No PD task [Dev %d]: %d, %d", m_i4CurrSensorDev, m_i4EnableAF, m_i4PDSensorType);
            return S_AF_OK;
        }

        MY_LOG( "[Signal]doPDTask");

        MINT32 ret = S_3A_OK;
        if( m_pdaf_rawbuf)
        {
            //allocate pd input data
            SPDInputData_t PDInput( m_i4PDAreaNum, m_i4TGSzH*m_i4TGSzW*10/8, reinterpret_cast<MUINT8 *>(m_pdaf_rawbuf));

            //dump frame information.
            if( m_i4DgbLogLv)
            {
                MY_LOG_IF( (m_i4DgbLogLv&1), "[PDAF] dump queue+");
                for( MINT32 i=0; i<m_vFrmInfo.size(); i++)
                {
                    MY_LOG_IF( m_i4DgbLogLv,
                               "[PDAF] %d, Frm %d, RawFmt %d, LensPos %d\n",
                               i,
                               m_vFrmInfo[i].i4FrmNum,
                               m_vFrmInfo[i].i4RawFmt,
                               m_vFrmInfo[i].i4LensPos);

                }
                MY_LOG_IF( m_i4DgbLogLv, "[PDAF] dump queue-");
            }
            //match lens position by magic number
            if( m_pdaf_raw_frmnum!=0)
            {
                MBOOL seachDone = FALSE;
                vector<AF_FRM_INFO_T>::iterator itor;
                for( itor=m_vFrmInfo.end()-1; itor!=m_vFrmInfo.begin(); )
                {
                    if( itor->i4FrmNum-1 == m_pdaf_raw_frmnum)
                    {
                        MY_LOG( "[PDAF] PureRaw @%d with Lens @%d", m_pdaf_raw_frmnum, itor->i4LensPos);
                        m_i4PreLensPos = itor->i4LensPos;
                        seachDone = TRUE;
                        break;
                    }
                    itor--;
                }
                m_vFrmInfo.erase( m_vFrmInfo.begin(), itor-1);

                //lose frame.
                if( seachDone==FALSE)
                {
                    MY_LOG( "[PDAF] Updating PD info is fail @%d", m_pdaf_raw_frmnum);
                    return E_AF_BUSY;
                }
            }
            else
            {
                m_vFrmInfo.clear();
            }


            PDInput.frmNum = framCnt++; //debug only
            PDInput.curLensPos   = m_i4PreLensPos;
            PDInput.XSizeOfImage = m_i4TGSzW;
            PDInput.YSizeOfImage = m_i4TGSzH;
            for( MUINT32 i=0; i<PDInput.numROI; i++)
            {
                PDInput.ROI[i].i4XStart = m_sArea_PD[i].i4X;
                PDInput.ROI[i].i4YStart = m_sArea_PD[i].i4Y;
                PDInput.ROI[i].i4XEnd = m_sArea_PD[i].i4X + m_sArea_PD[i].i4W;
                PDInput.ROI[i].i4YEnd = m_sArea_PD[i].i4Y + m_sArea_PD[i].i4H;
                PDInput.ROI[i].i4Info = m_sArea_PD[i].i4Info;

                MY_LOG( "[Core][PDAF]win %d [WinPos] %d, %d, %d, %d",
                        i,
                        PDInput.ROI[i].i4XStart,
                        PDInput.ROI[i].i4YStart,
                        PDInput.ROI[i].i4XEnd,
                        PDInput.ROI[i].i4YEnd);

            }
            ret=IPDMgr::getInstance().postToPDTask( m_i4CurrSensorDev, &PDInput);

            if( ret==E_3A_ERR)
            {
                MY_LOG_IF( m_i4DgbLogLv, "[DAF] post to pd mgr error");
            }
        }
        else
        {
            ret = E_3A_ERR;
            MY_LOG( "[DAF] raw buffer is NULL");
        }

        if( ret==E_3A_ERR)
        {
            m_i4PDSensorType = 0;
        }
    }


    return S_AF_OK;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::setSGGPGN( MINT32 i4SGG_PGAIN)
{
    // workaround for iVHDR, no use
#if 0
    MINT32 i4AESetPGN = i4SGG_PGAIN;
#endif
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::ConvertDMABufToStat( MINT32 &i4CurPos, MVOID *ptrInStatBuf, AF_STAT_PROFILE_T &sOutSata)
{
    StatisticBufInfo *ptrStatInfo = reinterpret_cast<StatisticBufInfo *>( ptrInStatBuf);
    AF_HW_STAT_T *ptrDMABuf = reinterpret_cast<AF_HW_STAT_T *>( ptrStatInfo->mVa);
    MUINT32 i4StatMagicNum  = ptrStatInfo->mMagicNumber;
    MUINT32 i4StatSize      = ptrStatInfo->mSize;

    //reset last time data
    memset(&sOutSata, 0, sizeof(AF_STAT_PROFILE_T));

    if( m_i4HWBlkNumX<=0 || m_i4HWBlkNumY<=0 || m_i4HWBlkNumX>MAX_AF_HW_WIN_X || m_i4HWBlkNumY>MAX_AF_HW_WIN_Y)
    {
        //Should not be happened.
        MY_ERR( "HW-Setting Fail");
    }
    else if( ptrDMABuf==NULL)
    {
        //Should not be happened.
        MY_ERR( "AFO Buffer NULL");
    }
    else
    {
        //number of AF statistic blocks.
        MUINT32 nblkW = m_i4HWBlkNumX;
        MUINT32 nblkH = m_i4HWBlkNumY;
        MUINT32 nblk  = nblkW*nblkH;

        //Debug only.
        MUINT64 FV_H0=0;
        MUINT64 FV_H1=0;
        MUINT64 FV_V=0;

        //==========
        // Outputs
        //==========
        //statistic information.
        sOutSata.u4NumBlkX = nblkW;
        sOutSata.u4NumBlkY = nblkH;
        //AF statistic
        AF_STAT_T *ptrSata = sOutSata.ptrStat;
        for( MUINT32 i=0; i<nblk; i++)
        {
            ptrSata[i].u4FILV  = ptrDMABuf[i].byte_00_03 & 0x007FFFFF;
            ptrSata[i].u4FILH0 = ptrDMABuf[i].byte_04_07 & 0x3FFFFFFF;
            ptrSata[i].u4FILH1 = ptrDMABuf[i].byte_08_11 & 0x3FFFFFFF;
            ptrSata[i].u4GSum  = ptrDMABuf[i].byte_12_15 & 0x003FFFFF;
            ptrSata[i].u4SCnt  = ((ptrDMABuf[i].byte_00_03 >> 23      ) & 0x01FF) |
                                 ((ptrDMABuf[i].byte_04_07 >> 30 <<  9) & 0x0600) |
                                 ((ptrDMABuf[i].byte_12_15 >> 24 << 11) & 0x3800) ;

            FV_H0 += ptrSata[i].u4FILH0;
            FV_H1 += ptrSata[i].u4FILH1;
            FV_V  += ptrSata[i].u4FILV;
        }

        // For debug FV only.
        MY_LOG_IF( m_i4DgbLogLv,
                   "%4d [Pos]%4d [H0]%10lld [H1]%10lld [V] %lld",
                   i4StatMagicNum,
                   i4CurPos,
                   FV_H0,
                   FV_H1,
                   FV_V);

        //debug print.
        if( (m_i4DgbLogLv&0x2)!=0)
        {
            for( MUINT32 j=0; j<nblkH; j++)
            {
                for( MUINT32 i=0; i<nblkW; i++)
                {
                    MY_LOG_IF( m_i4DgbLogLv&0x2,
                               "[%3d][%3d] : [V]%8d, [H0]%8d, [H1]%8d, [GSum]%8d, [SCnt]%8d\n",
                               i,
                               j,
                               sOutSata.ptrStat[i].u4FILV,
                               sOutSata.ptrStat[i].u4FILH0,
                               sOutSata.ptrStat[i].u4FILH1,
                               sOutSata.ptrStat[i].u4GSum,
                               sOutSata.ptrStat[i].u4SCnt);

                }
            }
        }
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT64 AfMgr::TransStatProfileToAlgo( AF_STAT_PROFILE_T &sInSataProfile)
{
    /*[TODO] For preview IT only.*/
    MINT64 i8SataH0=0;

    //analyze center 6x6 blocks
    MUINT32 ablkW = 6;
    MUINT32 ablkH = 6;
    //total blocks
    MUINT32 tblkW = sInSataProfile.u4NumBlkX;
    MUINT32 tblkH = sInSataProfile.u4NumBlkY;
    //start address of block index.
    MUINT32 startX = (tblkW-ablkW)/2;
    MUINT32 startY = (tblkH-ablkH)/2;


    for( MUINT32 j=0; j<ablkH; j++)
    {
        for( MUINT32 i=0; i<ablkW; i++)
        {
            MUINT32 idx = (startY+j)*tblkW + (startX+i);
            i8SataH0 += sInSataProfile.ptrStat[idx].u4FILH0;
        }
    }

    //[TODO] m_pIAfAlgo->setAFStats(&statsH[0], &statsV[0], MAX_AF_HW_WIN);

    return i8SataH0;

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::isFocusFinish()
{
    return m_sAFOutput.i4IsAFDone;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::isFocused()
{
    return m_sAFOutput.i4IsFocused;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::getDebugInfo( AF_DEBUG_INFO_T &rAFDebugInfo)
{
    MRESULT ret = E_3A_ERR;

    //reset.
    memset( &rAFDebugInfo, 0, sizeof(AF_DEBUG_INFO_T));

    /* Do not modify following oder: */

    //1. Hybrid AF library
    if( m_pIAfAlgo)
    {
        ret = m_pIAfAlgo->getDebugInfo( rAFDebugInfo);
    }

    //2. PD library
    if( m_i4PDSensorType!=0)
    {
        //Not support open pd library.
        if( m_PDBuf_Type==EPDBuf_Raw || m_PDBuf_Type==EPDBuf_VC)
        {
            ret = IPDMgr::getInstance().GetDebugInfo( m_i4CurrSensorDev, rAFDebugInfo);
        }
    }

    //3. af mgr
    ret = GetMgrDbgInfo( rAFDebugInfo);

    return ret;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::WDBGInfo( AF_DEBUG_INFO_T &sOutDbgInfo, MUINT32 i4InIdx, MUINT32 i4InTag, MUINT32 i4InVal, MUINT32 i4InLineKeep)
{
    sOutDbgInfo.Tag[i4InIdx].u4FieldID    = AAATAG( AAA_DEBUG_AF_MODULE_ID, i4InTag, i4InLineKeep);
    sOutDbgInfo.Tag[i4InIdx].u4FieldValue = i4InVal;
    return S_3A_OK;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::GetMgrDbgInfo( AF_DEBUG_INFO_T &sOutDbgInfo)
{
    MRESULT ret = E_3A_ERR;
    MUINT32 idx = AF_DEBUG_TAG_SIZE;

    for( MUINT32 i=0; i<AF_DEBUG_TAG_SIZE; i++)
    {
        if( sOutDbgInfo.Tag[i].u4FieldID==0)
        {
            idx = i;
            break;
        }
    }

    MINT32 num = m_sAFOutput.sROIStatus.i4TotalNum;
    if( 0<=num && num<=MAX_MULTI_ZONE_WIN_NUM)
    {
        if( (AF_DEBUG_TAG_SIZE-idx)>=(num*3+3))
        {
            WDBGInfo( sOutDbgInfo, idx++, MZ_WIN_NUM, m_sAFOutput.sROIStatus.i4TotalNum , 0);
            WDBGInfo( sOutDbgInfo, idx++, MZ_WIN_W  , m_sAFOutput.sROIStatus.sROI[0].i4W, 0);
            WDBGInfo( sOutDbgInfo, idx++, MZ_WIN_H  , m_sAFOutput.sROIStatus.sROI[0].i4H, 1);

            for( MUINT32 i=0; i<num; i++)
            {
                WDBGInfo( sOutDbgInfo, idx++, MZ_WIN_X  , m_sAFOutput.sROIStatus.sROI[i].i4X   , 0);
                WDBGInfo( sOutDbgInfo, idx++, MZ_WIN_Y  , m_sAFOutput.sROIStatus.sROI[i].i4Y   , 1);
                WDBGInfo( sOutDbgInfo, idx++, MZ_WIN_RES, m_sAFOutput.sROIStatus.sROI[i].i4Info, 1);
            }
        }
    }

    if( (AF_DEBUG_TAG_SIZE-idx)>=31)
    {
        WDBGInfo( sOutDbgInfo, idx++, MGR_TG_W          , m_i4TGSzW                         , 0);
        WDBGInfo( sOutDbgInfo, idx++, MGR_TG_H          , m_i4TGSzH                         , 1);
        WDBGInfo( sOutDbgInfo, idx++, MGR_BIN_W         , m_i4BINSzW                        , 0);
        WDBGInfo( sOutDbgInfo, idx++, MGR_BIN_H         , m_i4BINSzH                        , 1);
        WDBGInfo( sOutDbgInfo, idx++, MGR_CROP_WIN_X    , m_sCropRegionInfo.i4X             , 0);
        WDBGInfo( sOutDbgInfo, idx++, MGR_CROP_WIN_Y    , m_sCropRegionInfo.i4Y             , 1);
        WDBGInfo( sOutDbgInfo, idx++, MGR_CROP_WIN_W    , m_sCropRegionInfo.i4W             , 1);
        WDBGInfo( sOutDbgInfo, idx++, MGR_CROP_WIN_H    , m_sCropRegionInfo.i4H             , 1);
        WDBGInfo( sOutDbgInfo, idx++, MGR_DZ_CFG        , m_i4DzWinCfg                      , 0);
        WDBGInfo( sOutDbgInfo, idx++, MGR_DZ_FACTOR     , m_i4DZFactor                      , 1);
        WDBGInfo( sOutDbgInfo, idx++, MGR_FOCUSING_WIN_X, m_sArea_Focusing.i4X              , 0);
        WDBGInfo( sOutDbgInfo, idx++, MGR_FOCUSING_WIN_Y, m_sArea_Focusing.i4Y              , 1);
        WDBGInfo( sOutDbgInfo, idx++, MGR_FOCUSING_WIN_W, m_sArea_Focusing.i4W              , 1);
        WDBGInfo( sOutDbgInfo, idx++, MGR_FOCUSING_WIN_H, m_sArea_Focusing.i4H              , 1);
        WDBGInfo( sOutDbgInfo, idx++, MGR_OTFD_WIN_X    , m_sArea_OTFD.i4X                  , 0);
        WDBGInfo( sOutDbgInfo, idx++, MGR_OTFD_WIN_Y    , m_sArea_OTFD.i4Y                  , 1);
        WDBGInfo( sOutDbgInfo, idx++, MGR_OTFD_WIN_W    , m_sArea_OTFD.i4W                  , 1);
        WDBGInfo( sOutDbgInfo, idx++, MGR_OTFD_WIN_H    , m_sArea_OTFD.i4H                  , 1);
        WDBGInfo( sOutDbgInfo, idx++, MGR_CENTER_WIN_X  , m_sArea_Center.i4X                , 0);
        WDBGInfo( sOutDbgInfo, idx++, MGR_CENTER_WIN_Y  , m_sArea_Center.i4Y                , 1);
        WDBGInfo( sOutDbgInfo, idx++, MGR_CENTER_WIN_W  , m_sArea_Center.i4W                , 1);
        WDBGInfo( sOutDbgInfo, idx++, MGR_CENTER_WIN_H  , m_sArea_Center.i4H                , 1);
        WDBGInfo( sOutDbgInfo, idx++, MGR_CMD_WIN_X     , m_sArea_APCmd.i4X                 , 0);
        WDBGInfo( sOutDbgInfo, idx++, MGR_CMD_WIN_Y     , m_sArea_APCmd.i4Y                 , 1);
        WDBGInfo( sOutDbgInfo, idx++, MGR_CMD_WIN_W     , m_sArea_APCmd.i4W                 , 1);
        WDBGInfo( sOutDbgInfo, idx++, MGR_CMD_WIN_H     , m_sArea_APCmd.i4H                 , 1);
        WDBGInfo( sOutDbgInfo, idx++, MGR_PD_BUF_TYPE   , m_PDBuf_Type                      , 0);
        WDBGInfo( sOutDbgInfo, idx++, MGR_PD_SEN_TYPE   , m_i4PDSensorType                  , 0);
        WDBGInfo( sOutDbgInfo, idx++, MGR_LASER_VAL     , m_sAFInput.sLaserInfo.i4CurPosDist, 0);
        WDBGInfo( sOutDbgInfo, idx++, MGR_FOCUSING_POS  , m_sFocusDis.i4LensPos             , 0);
        WDBGInfo( sOutDbgInfo, idx++, MGR_FOCUSING_DST  , m_sFocusDis.i4Dist                , 0);
        ret = S_3A_OK;
    }

    return ret;

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::getLensInfo( LENS_INFO_T &a_rLensInfo)
{
    MRESULT ret = E_3A_ERR;
    mcuMotorInfo rMotorInfo;

    if( m_pMcuDrv)
    {
        ret = m_pMcuDrv->getMCUInfo(&rMotorInfo, m_i4CurrSensorDev);

        if( a_rLensInfo.i4CurrentPos!=(MINT32)rMotorInfo.u4CurrentPosition)
        {
            MY_LOG( "%s Dev %d, curPos %d, ",
                    __FUNCTION__,
                    m_i4CurrSensorDev,
                    (MINT32)rMotorInfo.u4CurrentPosition);
        }

        a_rLensInfo.i4CurrentPos   = (MINT32)rMotorInfo.u4CurrentPosition;
        a_rLensInfo.bIsMotorOpen   = rMotorInfo.bIsMotorOpen;
        a_rLensInfo.bIsMotorMoving = rMotorInfo.bIsMotorMoving;
        a_rLensInfo.i4InfPos       = (MINT32)rMotorInfo.u4InfPosition;
        a_rLensInfo.i4MacroPos     = (MINT32)rMotorInfo.u4MacroPosition;
        a_rLensInfo.bIsSupportSR   = rMotorInfo.bIsSupportSR;

        MY_LOG_IF( m_i4DgbLogLv,
                   "%s Dev %d, %d, %d, %d, %d, %d, %d",
                   __FUNCTION__,
                   m_i4CurrSensorDev,
                   a_rLensInfo.i4CurrentPos,
                   a_rLensInfo.bIsMotorOpen,
                   a_rLensInfo.bIsMotorMoving,
                   a_rLensInfo.i4InfPos,
                   a_rLensInfo.i4MacroPos,
                   a_rLensInfo.bIsSupportSR);

        ret = S_AF_OK;
    }
    else
    {
        MY_LOG_IF( m_i4DgbLogLv,
                   "%s Fail, Dev %d",
                   __FUNCTION__,
                   m_i4CurrSensorDev);
    }
    return ret;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::setBestShotConfig()
{
    MY_LOG( "[setBestShotConfig] Not use");
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::calBestShotValue( MVOID *pAFStatBuf)
{
    pAFStatBuf=NULL;

    MY_LOG( "[calBestShotValue] Not use");
    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT64 AfMgr::getBestShotValue()
{
    MY_LOG( "[getBestShotValue] Not use");
    return 0;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT64 AfMgr::getTimeStamp()
{
    struct timespec t;

    t.tv_sec = t.tv_nsec = 0;
    clock_gettime(CLOCK_MONOTONIC, &t);

    MINT64 timestamp =((t.tv_sec) * 1000000000LL + t.tv_nsec)/1000000;
    return timestamp; // from nano to ms
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT64 AfMgr::MoveLensTo( MINT32 &i4TargetPos)
{
    if( m_pMcuDrv)
    {
        if( m_i4DbgMotorDisable==1)
        {
            if( m_i4MvLensTo!=m_i4DbgMotorMPos)
            {
                m_i4MvLensTo = m_i4DbgMotorMPos;
                m_pMcuDrv->moveMCU( m_i4DbgMotorMPos, m_i4CurrSensorDev);
            }
        }
        else if( m_i4MvLensTo!=i4TargetPos)
        {
            MY_LOG("%s %d, Dev %d", __FUNCTION__, i4TargetPos, m_i4CurrSensorDev);

            m_i4MvLensTo = i4TargetPos;

            MY_LOG_IF( m_i4DgbLogLv, "%s+", __FUNCTION__);
            m_pMcuDrv->moveMCU( m_i4MvLensTo, m_i4CurrSensorDev);
            MY_LOG_IF( m_i4DgbLogLv, "%s-", __FUNCTION__);
        }
    }
    else
    {
        MY_LOG_IF( m_i4DgbLogLv, "%s Fail, Dev %d", __FUNCTION__, m_i4CurrSensorDev);
    }

    return getTimeStamp();

}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT AfMgr::readOTP()
{
    MUINT32 result = 0;
    CAM_CAL_DATA_STRUCT GetCamCalData;
    CamCalDrvBase *pCamCalDrvObj = CamCalDrvBase::createInstance();
    MINT32 i4SensorDevID;

    CAMERA_CAM_CAL_TYPE_ENUM enCamCalEnum = CAMERA_CAM_CAL_DATA_3A_GAIN;

    switch( m_i4CurrSensorDev)
    {
    case ESensorDev_Main:
        i4SensorDevID = SENSOR_DEV_MAIN;
        break;
    case ESensorDev_Sub:
        i4SensorDevID = SENSOR_DEV_SUB;
        break;
    case ESensorDev_MainSecond:
        i4SensorDevID = SENSOR_DEV_MAIN_2;
        return S_AWB_OK;
    case ESensorDev_Main3D:
        i4SensorDevID = SENSOR_DEV_MAIN_3D;
        return S_AWB_OK;
    default:
        i4SensorDevID = SENSOR_DEV_NONE;
        return S_AWB_OK;
    }

    result = pCamCalDrvObj->GetCamCalCalData(i4SensorDevID, enCamCalEnum, (void *)&GetCamCalData);
    MY_LOG( "(0x%8x)=pCamCalDrvObj->GetCamCalCalData", result);

    if (result&CamCalReturnErr[enCamCalEnum])
    {
        MY_LOG( "err (%s)", CamCalErrString[enCamCalEnum]);
        return E_AF_NOSUPPORT;
    }

    MY_LOG( "OTP data [S2aBitEn]%d [S2aAfBitflagEn]%d [S2aAf0]%d [S2aAf1]%d",
            GetCamCalData.Single2A.S2aBitEn,
            GetCamCalData.Single2A.S2aAfBitflagEn,
            GetCamCalData.Single2A.S2aAf[0],
            GetCamCalData.Single2A.S2aAf[1]);

    MINT32 i4InfPos, i4MacroPos, i450cmPos;

    i4InfPos   = GetCamCalData.Single2A.S2aAf[0];
    i4MacroPos = GetCamCalData.Single2A.S2aAf[1];
    i450cmPos  = GetCamCalData.Single2A.S2aAf[3];

    if( 0<i4MacroPos && i4MacroPos<1024 && i4MacroPos>i4InfPos )
    {
        MY_LOG( "OTP [Inf]%d [Macro]%d", i4InfPos, i4MacroPos);

        if( (i4MacroPos>i450cmPos) && (i450cmPos>i4InfPos) )
        {
            if( m_pLaserDrv )
            {
                m_pLaserDrv->setLensCalibrationData( i4MacroPos, i450cmPos);
            }

            MY_LOG( "OTP [50cm]%d", i450cmPos);

            // pass laserAF 50cm otp data to algo
            if( m_sNVRam.rAFNVRAM.i4Coefs[5] > 0)
            {
                i4MacroPos = i450cmPos * 10000 + i4MacroPos;
            }
        }

        if( m_pIAfAlgo)
        {
            m_pIAfAlgo->updateAFtableBoundary( i4InfPos, i4MacroPos);
        }


    }
    else
    {
        MY_LOG( "OTP abnormal return [Inf]%d [Macro]%d", i4InfPos, i4MacroPos);

        if( m_pMcuDrv)
        {
            mcuMotorCalPos rMotorCalPos;
            m_pMcuDrv->getMCUCalPos( &rMotorCalPos, m_i4CurrSensorDev);

            i4InfPos   = rMotorCalPos.u4InfPos;
            i4MacroPos = rMotorCalPos.u4MacroPos;

            MY_LOG( "OTP from driver [Inf]%d [Macro]%d", i4InfPos, i4MacroPos);

            if( 0<i4MacroPos && i4MacroPos<1024 && i4MacroPos>i4InfPos)
            {
                if( m_pIAfAlgo)
                {
                    m_pIAfAlgo->updateAFtableBoundary( i4InfPos, i4MacroPos);
                }
            }
        }
    }

    return S_AF_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::autoFocus()
{
    MY_LOG( "cmd-%s Dev %d : lib_afmode %d", __FUNCTION__, m_i4CurrSensorDev, m_eLIB3A_AFMode);

    if( m_i4EnableAF==0) // dummy lens
    {
        MY_LOG( "autofocus : dummy lens");
        return;
    }


    m_sAFOutput.i4IsAFDone = MFALSE;

    UpdateState( EVENT_CMD_AUTOFOCUS);


    //calibration flow testing
    if( m_pLaserDrv)
    {
        int Offset = 0;
        int XTalk = 0;

        char value[PROPERTY_VALUE_MAX] = {'\0'};
        int Mode = 0;
        property_get( "laser.calib.mode", value, "0");
        Mode = atoi(value);

        if( Mode == 1 )
        {
            MY_LOG( "LaserCali : getLaserOffsetCalib Start");
            m_pLaserDrv->getLaserOffsetCalib(&Offset);
            MY_LOG( "LaserCali : getLaserOffsetCalib : %d", Offset);
            MY_LOG( "LaserCali : getLaserOffsetCalib End");
        }

        if( Mode == 2 )
        {
            MY_LOG( "LaserCali : getLaserXTalkCalib Start");
            m_pLaserDrv->getLaserXTalkCalib(&XTalk);
            MY_LOG( "LaserCali : getLaserXTalkCalib : %d", XTalk);
            MY_LOG( "LaserCali : getLaserXTalkCalib End");
        }
    }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::cancelAutoFocus()
{
    MY_LOG( "cmd-%s Dev %d : %d",
            __FUNCTION__,
            m_i4CurrSensorDev,
            m_eLIB3A_AFMode);

    //update parameters and status.
    UpdateState(EVENT_CMD_AUTOFOCUS_CANCEL);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::TimeOutHandle()
{
    MY_LOG( "%s", __FUNCTION__);
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::setAndroidServiceState( MBOOL a_state)
{
}
//Camera 3.0

MVOID AfMgr::setAperture( MFLOAT lens_aperture)
{

}
MFLOAT AfMgr::getAperture()
{
    MFLOAT lens_aperture=0;
    if( m_bGetMetaData==MFALSE)
    {
        return 0;
    }

    for( MUINT8 ii=0; ii<m_sMetaData.entryFor( MTK_LENS_INFO_AVAILABLE_APERTURES).count(); ii++)
    {
        lens_aperture = m_sMetaData.entryFor( MTK_LENS_INFO_AVAILABLE_APERTURES).itemAt( ii, Type2Type<MFLOAT>());
        MY_LOG( "AFmeta APERTURES %d  %f", ii, lens_aperture);
    }
    return lens_aperture;
}
MVOID AfMgr::setFilterDensity( MFLOAT lens_filterDensity) {}
MFLOAT AfMgr::getFilterDensity()
{
    MFLOAT lens_filterDensity=0;
    if( m_bGetMetaData==MFALSE)
    {
        return 0;
    }

    for(MUINT8 ii=0; ii<m_sMetaData.entryFor( MTK_LENS_INFO_AVAILABLE_FILTER_DENSITIES).count(); ii++)
    {
        lens_filterDensity = m_sMetaData.entryFor( MTK_LENS_INFO_AVAILABLE_FILTER_DENSITIES).itemAt( ii, Type2Type<MFLOAT>());
        MY_LOG( "AFmeta FILTER_DENSITIES %d  %f", ii, lens_filterDensity);
    }
    return lens_filterDensity;
}
MVOID AfMgr::setFocalLength( MFLOAT lens_focalLength) {}
MFLOAT AfMgr::getFocalLength ()
{
    MFLOAT lens_focalLength =34.0;
    if( m_bGetMetaData==MFALSE)
    {
        return 0;
    }

    for( MUINT8 ii=0; ii<m_sMetaData.entryFor( MTK_LENS_INFO_AVAILABLE_FOCAL_LENGTHS).count(); ii++)
    {
        lens_focalLength = m_sMetaData.entryFor( MTK_LENS_INFO_AVAILABLE_FOCAL_LENGTHS).itemAt( ii, Type2Type<MFLOAT>());
        MY_LOG( "AFmeta APERTURES %d  %f", ii, lens_focalLength);
    }
    return lens_focalLength;
}

MVOID AfMgr::setFocusDistance( MFLOAT lens_focusDistance)
{
    MINT32 fdistidx = 0;
    MINT32 i4distmm;
    MINT32 i4tblLL;
    MINT32 i4ResultDac;

    if( lens_focusDistance<0)
    {
        return;
    }
    //lens_focusDistance is in unit dipoter, means 1/distance,
    //if distance is 100 cm,  then the value is  1/(0.1M) =10,
    // 10 cm => dipoter 100
    // 1 cm => dipoter 1000
    MY_LOG_IF( m_i4DgbLogLv,
               "cmd-%s Dev %d : %f\n",
               __FUNCTION__,
               m_i4CurrSensorDev,
               lens_focusDistance);

    i4tblLL = m_sAFParam.i4TBLL;
    if( lens_focusDistance <= (1000.0f/m_sAFParam.i4Dist[i4tblLL-1])) /*infinity*/
    {
        i4ResultDac=m_sAFParam.i4Dacv[i4tblLL-1];
    }
    else if( (1000.0f/m_sAFParam.i4Dist[0]) <= lens_focusDistance)  /*marco*/
    {
        i4ResultDac = m_sAFParam.i4Dacv[0];
    }
    else
    {
        i4distmm = (MINT32)(1000/lens_focusDistance);

        for( fdistidx=0; fdistidx< i4tblLL ; fdistidx++)
        {
            if( i4distmm<m_sAFParam.i4Dist[fdistidx])
                break;

        }

        if( fdistidx==0)
        {
            i4ResultDac = m_sAFParam.i4Dacv[0];
        }
        else
        {
            i4ResultDac=
                ( m_sAFParam.i4Dacv[fdistidx]   * (i4distmm - m_sAFParam.i4Dist[fdistidx-1])
                  + m_sAFParam.i4Dacv[fdistidx-1] * (m_sAFParam.i4Dist[fdistidx] - i4distmm ))
                /(m_sAFParam.i4Dist[fdistidx] - m_sAFParam.i4Dist[fdistidx-1] );
        }
    }

    // API2: At MTK_CONTROL_AF_MODE_OFF mode, configure algorithm as MF mode.
    //          The auto-focus routine does not control the lens. Lens  is controlled by the application.
    if( (m_eLIB3A_AFMode == LIB3A_AF_MODE_MF) &&
            (m_sAFOutput.i4AFPos != i4ResultDac) &&
            (0<=i4ResultDac) &&
            (i4ResultDac<=1023))
    {
        MY_LOG( "cmd-%s set lens pos (%d)->(%d) by dis %f",
                __FUNCTION__,
                m_sAFOutput.i4AFPos,
                i4ResultDac,
                lens_focusDistance);

        m_i4MFPos = i4ResultDac;

        if (m_pIAfAlgo)
        {
            m_pIAfAlgo->setMFPos(m_i4MFPos);
            m_pIAfAlgo->trigger();
        }
        else
        {
            MY_LOG("Null m_pIAfAlgo");
        }
    }
    else
    {
        MY_LOG( "cmd-%s skip set lens pos (%d)->(%d), lib_afmode(%d)",
                __FUNCTION__,
                m_sAFOutput.i4AFPos,
                i4ResultDac,
                m_eLIB3A_AFMode);
    }

}

MFLOAT AfMgr::getFocusDistance()
{
    MINT32 fdacidx=0;
    MINT32 i4tblLL;
    MINT32 i4ResultDist  = 100;

    if( m_i4EnableAF==0)
    {
        MY_LOG( "cmd-%s no AF", __FUNCTION__);
        return 0.0;
    }


    MINT32 LensPos;
    if( m_eLIB3A_AFMode == LIB3A_AF_MODE_MF)
    {
        /* In MF mode, algorithm will take some step to target position.
                  So directly using MFpos instead of using m_sAFOutput.i4AFPos.*/
        LensPos = m_i4MFPos;
    }
    else
    {
        LensPos = m_sAFOutput.i4AFPos;
    }


    // Lens position is changed, calculating new focus distance :
    if( m_sFocusDis.i4LensPos!=LensPos)
    {
        i4tblLL = m_sAFParam.i4TBLL;
        if( LensPos <= m_sAFParam.i4Dacv[i4tblLL-1]) /*infinity*/
        {
            i4ResultDist = m_sAFParam.i4Dist[i4tblLL-1];
        }
        else if( m_sAFParam.i4Dacv[0] <= LensPos) /*macro*/
        {
            i4ResultDist = m_sAFParam.i4Dist[0];

        }
        else
        {
            for(fdacidx=0; fdacidx< i4tblLL ; fdacidx++)
            {
                if( LensPos >  m_sAFParam.i4Dacv[fdacidx])
                    break;
            }
            if(fdacidx==0)
            {
                i4ResultDist = m_sAFParam.i4Dist[0];
            }
            else
            {
                i4ResultDist=
                    ( m_sAFParam.i4Dist[fdacidx]   * (m_sAFParam.i4Dacv[fdacidx-1] - LensPos)
                      + m_sAFParam.i4Dist[fdacidx-1] * (LensPos - m_sAFParam.i4Dacv[fdacidx]))
                    /(m_sAFParam.i4Dacv[fdacidx-1] - m_sAFParam.i4Dacv[fdacidx] );
            }
            if( i4ResultDist<=0)
            {
                i4ResultDist = m_sAFParam.i4Dist[i4tblLL-1];
            }
        }

        MFLOAT i4Dist = 1000.0/ ((MFLOAT)i4ResultDist);

        MY_LOG( "cmd-%s Pos %d->%d, Dis %f->%f\n",
                __FUNCTION__,
                m_sFocusDis.i4LensPos,
                LensPos,
                m_sFocusDis.i4Dist,
                i4Dist);

        //record.
        m_sFocusDis.i4LensPos = LensPos;
        m_sFocusDis.i4Dist = i4Dist;
    }

    return m_sFocusDis.i4Dist;
}

MVOID AfMgr::setOpticalStabilizationMode( MINT32 ois_OnOff)
{
    MUINT8 oismode=0;
    if( m_bGetMetaData==MFALSE)
    {
        return;
    }

    for( MUINT8 ii=0; ii<m_sMetaData.entryFor(MTK_LENS_INFO_AVAILABLE_OPTICAL_STABILIZATION).count(); ii++)
    {
        oismode = m_sMetaData.entryFor(MTK_LENS_INFO_AVAILABLE_OPTICAL_STABILIZATION).itemAt( ii, Type2Type<MUINT8>());
        MY_LOG( "AFmeta OPTICAL_STABILIZATION %d  %d",ii, oismode);
        if(oismode==1)
        {
            //set_ois_drv_on(ois_OnOff);
            break;
        }
    }
}

MINT32 AfMgr::getOpticalStabilizationMode()
{
    MUINT8 oismode=0;
    if( m_bGetMetaData==MFALSE)
    {
        return 0;
    }

    for( MUINT8 ii=0; ii<m_sMetaData.entryFor( MTK_LENS_INFO_AVAILABLE_OPTICAL_STABILIZATION).count(); ii++)
    {
        oismode = m_sMetaData.entryFor( MTK_LENS_INFO_AVAILABLE_OPTICAL_STABILIZATION).itemAt( ii, Type2Type< MUINT8 >());
        MY_LOG( "AFmeta OPTICAL_STABILIZATION %d  %d", ii, oismode);
    }
    return oismode;/*OFF, ON */
}

MVOID AfMgr::getFocusRange( MFLOAT *vnear, MFLOAT *vfar)
{
    MINT32 i4tblLL;
    MINT32 fdacidx       =0;
    MINT32 i4ResultDist  = 100;
    MINT32 i4ResultRange = 100;
    MINT32 i4ResultNear  = 100;
    MINT32 i4ResultFar   = 100;
    MFLOAT retDist       = 0.0;

    if( m_i4EnableAF==0)
    {
        MY_LOG( "no focus lens");
        *vnear = 1/0.6;
        *vfar  = 1/3.0;
        return;
    }

    i4tblLL = m_sAFParam.i4TBLL;
    for( fdacidx=0; fdacidx<i4tblLL; fdacidx++)
    {
        if( m_sAFOutput.i4AFPos>m_sAFParam.i4Dacv[fdacidx])
            break;
    }

    if(fdacidx==0)
    {
        i4ResultDist = m_sAFParam.i4Dist[0];
    }
    else
    {
        i4ResultDist=
            ( m_sAFParam.i4Dist[fdacidx  ] * (m_sAFParam.i4Dacv[fdacidx-1] - m_sAFOutput.i4AFPos       )  +
              m_sAFParam.i4Dist[fdacidx-1] * (m_sAFOutput.i4AFPos          - m_sAFParam.i4Dacv[fdacidx])) /
            ( m_sAFParam.i4Dacv[fdacidx-1] - m_sAFParam.i4Dacv[fdacidx]);

        i4ResultRange=
            ( m_sAFParam.i4FocusRange[fdacidx  ] * (m_sAFParam.i4Dacv[fdacidx-1] - m_sAFOutput.i4AFPos       )  +
              m_sAFParam.i4FocusRange[fdacidx-1] * (m_sAFOutput.i4AFPos          - m_sAFParam.i4Dacv[fdacidx])) /
            ( m_sAFParam.i4Dacv[fdacidx-1] - m_sAFParam.i4Dacv[fdacidx]);
    }
    if( i4ResultDist <=0)  i4ResultDist= m_sAFParam.i4Dist[i4tblLL-1];
    if( i4ResultRange<=0) i4ResultRange= m_sAFParam.i4Dist[i4tblLL-1];

    i4ResultNear = i4ResultDist - (i4ResultRange/2);
    i4ResultFar  = i4ResultDist + (i4ResultRange/2);

    *vnear = 1000.0/ ((MFLOAT)i4ResultNear);
    *vfar  = 1000.0/ ((MFLOAT)i4ResultFar);

    //MY_LOG("[getFocusRange] [%f, %f]", *vnear,*vfar);
}

MINT32 AfMgr::getLensState  ()
{

    MUINT8 lens_item_ivalue;

    if( m_prePosition!=m_sAFOutput.i4AFPos)
    {
        m_prePosition = m_sAFOutput.i4AFPos;
        return 1; /* MOVING */
    }
    else
    {
        return 0;  /* STATIONARY */
    }

    return 0;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AfMgr::setSensorMode( MINT32 &i4NewSensorMode, MUINT32 &i4BINInfo_SzW, MUINT32 &i4BINInfo_SzH)
{
    m_i4SensorMode = i4NewSensorMode;
    m_i4BINSzW = i4BINInfo_SzW;
    m_i4BINSzH = i4BINInfo_SzH;

    MY_LOG( "cmd-%s Dev %d, Mode %d, BINSZ(%d, %d)\n",
            __FUNCTION__,
            m_i4CurrSensorDev,
            m_i4SensorMode,
            m_i4BINSzW,
            m_i4BINSzH);

    return MTRUE;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::PrintHWRegSetting( AF_CONFIG_T &sAFHWCfg)
{
    MY_LOG( "HW-TGSZ %d, %d, BINSZ %d, %d",
            sAFHWCfg.sTG_SZ.i4W,
            sAFHWCfg.sTG_SZ.i4H,
            sAFHWCfg.sBIN_SZ.i4W,
            sAFHWCfg.sBIN_SZ.i4H);

    MY_LOG( "HW-sROI %d, %d %d, %d %d",
            sAFHWCfg.sRoi.i4X,
            sAFHWCfg.sRoi.i4Y,
            sAFHWCfg.sRoi.i4W,
            sAFHWCfg.sRoi.i4H,
            sAFHWCfg.sRoi.i4Info);

    MY_LOG( "HW-nBLK %d, %d",
            sAFHWCfg.AF_BLK_XNUM,
            sAFHWCfg.AF_BLK_YNUM);

    MY_LOG( "HW-SGG %d, %d, %d, %d, %d, %d, %d, %d",
            sAFHWCfg.i4SGG_GAIN,
            sAFHWCfg.i4SGG_GMR1,
            sAFHWCfg.i4SGG_GMR2,
            sAFHWCfg.i4SGG_GMR3,
            sAFHWCfg.i4SGG_GMR4,
            sAFHWCfg.i4SGG_GMR5,
            sAFHWCfg.i4SGG_GMR6,
            sAFHWCfg.i4SGG_GMR7);


    MY_LOG( "HW-HVGL %d, %d, %d",
            sAFHWCfg.AF_H_GONLY,
            sAFHWCfg.AF_V_GONLY,
            sAFHWCfg.AF_V_AVG_LVL);

    MY_LOG( "HW-BLF %d, %d, %d, %d",
            sAFHWCfg.AF_BLF[0],
            sAFHWCfg.AF_BLF[1],
            sAFHWCfg.AF_BLF[2],
            sAFHWCfg.AF_BLF[3]);

    MY_LOG( "HW-TH %d, %d, %d, %d",
            sAFHWCfg.AF_TH_H[0],
            sAFHWCfg.AF_TH_H[1],
            sAFHWCfg.AF_TH_V,
            sAFHWCfg.AF_TH_G_SAT);

    MY_LOG( "HW-FIL0 %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
            sAFHWCfg.AF_FIL_H0[0],
            sAFHWCfg.AF_FIL_H0[1],
            sAFHWCfg.AF_FIL_H0[2],
            sAFHWCfg.AF_FIL_H0[3],
            sAFHWCfg.AF_FIL_H0[4],
            sAFHWCfg.AF_FIL_H0[5],
            sAFHWCfg.AF_FIL_H0[6],
            sAFHWCfg.AF_FIL_H0[7],
            sAFHWCfg.AF_FIL_H0[8],
            sAFHWCfg.AF_FIL_H0[9],
            sAFHWCfg.AF_FIL_H0[10],
            sAFHWCfg.AF_FIL_H0[11]);

    MY_LOG( "HW-FIL1 %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
            sAFHWCfg.AF_FIL_H1[0],
            sAFHWCfg.AF_FIL_H1[1],
            sAFHWCfg.AF_FIL_H1[2],
            sAFHWCfg.AF_FIL_H1[3],
            sAFHWCfg.AF_FIL_H1[4],
            sAFHWCfg.AF_FIL_H1[5],
            sAFHWCfg.AF_FIL_H1[6],
            sAFHWCfg.AF_FIL_H1[7],
            sAFHWCfg.AF_FIL_H1[8],
            sAFHWCfg.AF_FIL_H1[9],
            sAFHWCfg.AF_FIL_H1[10],
            sAFHWCfg.AF_FIL_H1[11]);

    MY_LOG( "HW-FILV %d, %d, %d, %d",
            sAFHWCfg.AF_FIL_V[0],
            sAFHWCfg.AF_FIL_V[1],
            sAFHWCfg.AF_FIL_V[2],
            sAFHWCfg.AF_FIL_V[3]);

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MBOOL AfMgr::ConfigHWReg( AF_CONFIG_T &sInHWCfg, AREA_T &sOutHWROI, MINT32 &i4OutHWBlkNumX, MINT32 &i4OutHWBlkNumY)
{
    MBOOL ret = MFALSE;

    //-------------
    //AF HAL control flow :
    //-------------
    sInHWCfg.sTG_SZ.i4W  = m_i4TGSzW;
    sInHWCfg.sTG_SZ.i4H  = m_i4TGSzH;
    sInHWCfg.sBIN_SZ.i4W = m_i4BINSzW;
    sInHWCfg.sBIN_SZ.i4H = m_i4BINSzH;

    MY_LOG_IF( m_i4DgbLogLv,
               "%s-ROI : [X]%d [Y]%d [W]%d [H]%d",
               __FUNCTION__,
               sInHWCfg.sRoi.i4X,
               sInHWCfg.sRoi.i4Y,
               sInHWCfg.sRoi.i4W,
               sInHWCfg.sRoi.i4H);

    // If HW setting is changed, set HW setting to isp_mgr_af_stat
    if( memcmp( &m_sHWCfg, &sInHWCfg, sizeof(AF_CONFIG_T))!=0)
    {
        MY_LOG( "%s +", __FUNCTION__);

        // record original HW setting which is from AF algorithm.
        memcpy( &m_sHWCfg, &sInHWCfg, sizeof(AF_CONFIG_T));

        /**
         * configure HW :
         * Output parameters :
         * Because HW constraint is applied, HW setting maybe be changed.
         * sOutHWROI is used to align HW analyzed region to any other algorithm, for example, phase difference algorithm.
         */
        ISP_MGR_AF_STAT_CONFIG_T::getInstance(static_cast<ESensorDev_T>(m_i4CurrSensorDev)).config(sInHWCfg, sOutHWROI, i4OutHWBlkNumX, i4OutHWBlkNumY);

        // error log : should not be happened.
        if( m_sHWCfg.AF_BLK_XNUM != i4OutHWBlkNumX  ||
                m_sHWCfg.AF_BLK_YNUM != i4OutHWBlkNumY )
        {
            MY_ERR( "WAR-ROI : [X]%d [Y]%d [W]%d [H]%d -> [X]%d [Y]%d [W]%d [H]%d, [XNUM] %d->%d, [YNUM] %d->%d",
                    sInHWCfg.sRoi.i4X,
                    sInHWCfg.sRoi.i4Y,
                    sInHWCfg.sRoi.i4W,
                    sInHWCfg.sRoi.i4H,
                    sOutHWROI.i4X,
                    sOutHWROI.i4Y,
                    sOutHWROI.i4W,
                    sOutHWROI.i4H,
                    sInHWCfg.AF_BLK_XNUM,
                    i4OutHWBlkNumX,
                    sInHWCfg.AF_BLK_YNUM,
                    i4OutHWBlkNumY);
        }

        MY_LOG( "%s -", __FUNCTION__);

        ret = MTRUE;

    }

    return ret;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID AfMgr::SetCurFrmNum( MUINT32 &i4FrmNum)
{
    //m_i4CurFrmNum=i4FrmNum;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Depth AF API
MINT32 AfMgr::getDAFtbl( MVOID **ptbl)
{
    *ptbl=&m_sDAF_TBL;
    return DAF_TBL_QLEN;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 AfMgr::setMultiZoneEnable( MBOOL &bEn)
{
    m_bMZHostEn = bEn;

    //Accroding to option, updating center ROI coordinate automatically
    UpdateCenterROI( m_sArea_Center);

    return S_AF_OK;
}


