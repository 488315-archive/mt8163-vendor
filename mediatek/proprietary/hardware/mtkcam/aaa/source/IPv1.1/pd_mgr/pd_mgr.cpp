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
#define LOG_TAG "pd_mgr"

#ifndef ENABLE_MY_LOG
#define ENABLE_MY_LOG       (1)
#endif



#include <cutils/properties.h>
#include <utils/threads.h>



#include <aaa_types.h>
#include <aaa_error_code.h>
#include <aaa_log.h>
#include "aaa_sensor_buf_mgr.h"

#include "pd_mgr.h"
#include <pd_buf_mgr_open.h>
#include <pd_buf_mgr.h>
#include <Local.h>

#include <isp_tuning.h>
#include <kd_camera_feature.h>
#include "kd_imgsensor.h"
#include <lib3a/pd_algo_if.h>
#include <IHalSensor.h>
#include <kd_imgsensor_define.h>





//------------Thread-------------
#include <v1/config/PriorityDefs.h>
#include <sys/prctl.h>
#include <utils/include/common.h>

//-------------------------------

using namespace NSCam::Utils;
using namespace NSCam;


namespace NS3Av3
{
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//                                Multi-instance.
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PDMgr* PDMgr::s_pPDMgr = MNULL;
//----------------------------------------------------------------------------------------------------
template <ESensorDev_T const eSensorDev>
class PDMgrDev : public PDMgr
{
public:
    static PDMgr& getInstance()
    {
        static PDMgrDev<eSensorDev> singleton;
        PDMgr::s_pPDMgr = &singleton;
        return singleton;
    }

    PDMgrDev() : PDMgr(eSensorDev) {}
    virtual ~PDMgrDev() {}
};
//----------------------------------------------------------------------------------------------------
PDMgr &PDMgr::getInstance(MINT32 const i4SensorDev)
{
    switch ( i4SensorDev)
    {
    case ESensorDev_Main: //  Main Sensor
        return  PDMgrDev<ESensorDev_Main>::getInstance();
    case ESensorDev_MainSecond: //  Main Second Sensor
        return  PDMgrDev<ESensorDev_MainSecond>::getInstance();
    case ESensorDev_Sub: //  Sub Sensor
        return  PDMgrDev<ESensorDev_Sub>::getInstance();
    default:
        MY_LOG("i4SensorDev = %d", i4SensorDev);
        if ( PDMgr::s_pPDMgr)
        {
            return  *PDMgr::s_pPDMgr;
        }
        else
        {
            return  PDMgrDev<ESensorDev_Main>::getInstance();
        }
    }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//                                init function.
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PDMgr::PDMgr(ESensorDev_T eSensorDev)
{
    m_i4CurrSensorDev=(MINT32)eSensorDev;
}

//----------------------------------------------------------------------------------------------------
PDMgr::~PDMgr()
{
}

//----------------------------------------------------------------------------------------------------
EPDBuf_Type_t PDMgr::init(SPDProfile_t *iPDProfile)
{
    MY_LOG("[init]+");

    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get( "debug.af_mgr.enable", value, "0");
    m_bDebugEnable = atoi(value);
    property_get( "debug.pd_vc.enable", value, "0");
    m_bPDVCTest = atoi(value);

    m_frmCnt = 0;



    //initial member.
    m_Status        = EPD_Not_Enabled;
    m_numROI        = 0;
    m_databuf_size  = 0;
    m_databuf       = NULL;
    m_pPDBufMgrCore = NULL;
    m_pPDBufMgrOpen = NULL;
    m_pIPdAlgo      = NULL;

    memset( m_sPDInput,          0, sizeof(PD_INPUT_T)*AF_PSUBWIN_NUM);
    memset( m_sPDOutput,         0, sizeof(PD_OUTPUT_T)*AF_PSUBWIN_NUM);
    memset( m_i4PDInfo2HybirdAF, 0, sizeof(MINT32)*10);
    memset( m_ROI,               0, sizeof(AREA_T)*AF_PSUBWIN_NUM);

    m_vPDOutput.clear();

    //get main information from host
    memcpy( &m_profile, iPDProfile, sizeof(m_profile));

    //get pd buffer type from custom setting.
    m_profile.BufType = GetPDBuf_Type( m_i4CurrSensorDev, m_profile.i4CurrSensorId);
    MY_LOG( "SensorID %x, Pdmode %d, BufType %d",
            m_profile.i4CurrSensorId,
            m_profile.u4Pdmode,
            m_profile.BufType);

    //check sensor output pd data mode and PD buffer manager setting.
    if( ( m_profile.u4Pdmode==2 && (m_profile.BufType==EPDBuf_VC ||m_profile.BufType==EPDBuf_VC_Open ) )||
            ( m_profile.u4Pdmode==1 && (m_profile.BufType==EPDBuf_Raw||m_profile.BufType==EPDBuf_Raw_Open) )  )
    {
        //select data path
        switch( m_profile.BufType)
        {
        case EPDBuf_VC :
        case EPDBuf_Raw :
            m_pPDBufMgrCore = ::PDBufMgr::createInstance( m_profile);
            m_pIPdAlgo      = NS3A::IPdAlgo::createInstance( m_i4CurrSensorDev);
            break;

        case EPDBuf_VC_Open :
            m_pPDBufMgrOpen = ::PDBufMgrOpen::createInstance( m_profile);
            break;

        case EPDBuf_NotDef :
        case EPDBuf_Raw_Open :
            m_pPDBufMgrCore = NULL;
            m_pPDBufMgrOpen = NULL;
            m_pIPdAlgo      = NULL;
        default :
            break;

        }
    }

    //engineer check only.
    if( m_bPDVCTest)
    {
        m_pPDBufMgrCore = NULL;
        m_pPDBufMgrOpen = NULL;
        m_pIPdAlgo      = NULL;
        MY_LOG( "Debug virtual channel only : for engineer testing only!!");
    }

    //check PD mgr initial status to deside thread should be created or not.
    if( (m_pPDBufMgrCore && m_pIPdAlgo)|| m_pPDBufMgrOpen)
    {
        //start thread.
        createThread();
    }
    else
    {
        m_profile.BufType = EPDBuf_NotDef;
        MY_LOG( "PD buffer type is not defined %x %x %x", m_pPDBufMgrCore, m_pIPdAlgo, m_pPDBufMgrOpen);
    }

    //output
    iPDProfile->BufType = m_profile.BufType;


    MY_LOG( "[init]-");
    return m_profile.BufType;
}
//----------------------------------------------------------------------------------------------------
MRESULT PDMgr::uninit()
{
    MY_LOG("[uninit] +");

    if( m_bEnPDBufThd)
    {
        //close thread
        closeThread();
    }

    //uninit member
    m_databuf_size=0;

    if( m_databuf)
        delete m_databuf;
    m_databuf=NULL;

    m_pPDBufMgrCore = NULL;
    m_pPDBufMgrOpen = NULL;

    m_pIPdAlgo = NULL;

    m_vPDOutput.clear();

    MY_LOG("[uninit] -");


    return S_3A_OK;
}
//----------------------------------------------------------------------------------------------------
MINT32 PDMgr::Boundary(MINT32 a_i4Min, MINT32 a_i4Vlu, MINT32 a_i4Max)
{
    if (a_i4Max < a_i4Min)
    {
        a_i4Max = a_i4Min;
    }
    if (a_i4Vlu < a_i4Min)
    {
        a_i4Vlu = a_i4Min;
    }
    if (a_i4Vlu > a_i4Max)
    {
        a_i4Vlu = a_i4Max;
    }
    return a_i4Vlu;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//                                   Thread
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MVOID PDMgr::createThread()
{
    MY_LOG( "create PD mgr thread");
    //create thread
    m_bEnPDBufThd = MTRUE;
    sem_init( &m_semPDBuf, 0, 0);
    sem_init( &m_semPDBufThdEnd, 0, 1);
    pthread_create( &m_PDBufThread, NULL, PDBufThreadLoop, this);
}
//----------------------------------------------------------------------------------------------------
MVOID PDMgr::closeThread()
{
    MY_LOG("close PD mgr thread");
    //close thread
    m_bEnPDBufThd = MFALSE;
    ::sem_post(&m_semPDBuf);
    pthread_join( m_PDBufThread, NULL);
}
//----------------------------------------------------------------------------------------------------
MRESULT PDMgr::postToPDBufThread()
{
    int Val;
    ::sem_getvalue( &m_semPDBufThdEnd, &Val);
    MY_LOG_IF( m_bDebugEnable, "[postToPDBufThread] sem_post m_semPDBuf, m_semPDBufThdEnd before wait = %d\n", Val);
    if( Val==1)
    {
        ::sem_wait( &m_semPDBufThdEnd); //to be 0, it won't block, 0 means PD task not ready yet
        ::sem_post( &m_semPDBuf);
    }
    return S_3A_OK;
}
//----------------------------------------------------------------------------------------------------
MVOID PDMgr::changePDBufThreadSetting()
{
    // (1) set name
    ::prctl( PR_SET_NAME, "PDBufThd", 0, 0, 0);

    // (2) set policy/priority
    {
        int const expect_policy     = SCHED_OTHER;
        int const expect_priority   = NICE_CAMERA_AF;
        int policy = 0, priority = 0;
        setThreadPriority( expect_policy, expect_priority);
        getThreadPriority (policy, priority);
        //

        MY_LOG_IF( m_bDebugEnable,
                   "[PDMgr::PDBufThreadLoop] policy:(expect, result)=(%d, %d), priority:(expect, result)=(%d, %d)",
                   expect_policy,
                   policy,
                   expect_priority,
                   priority);
    }

}
//----------------------------------------------------------------------------------------------------
MVOID* PDMgr::PDBufThreadLoop(MVOID *arg)
{
    MRESULT ret=E_3A_ERR;

    PDMgr *_this = reinterpret_cast<PDMgr*>(arg);

    MY_LOG( "PDBufThreadLoop +\n");

    // (1) change thread setting
    _this->changePDBufThreadSetting();
    _this->m_Status = EPD_Init;


    // (2) thread-in-loop
    while(1)
    {
        ::sem_wait( &_this->m_semPDBuf);
        if ( ! _this->m_bEnPDBufThd)
        {
            break;
        }
        MY_LOG_IF( _this->m_bDebugEnable, "[PDBufThreadLoop] PD task, dev:%d", _this->m_i4CurrSensorDev);



        _this->m_Status = EPD_BZ;

        MY_LOG_IF( _this->m_bDebugEnable, "Run Core Flow +");

        CAM_TRACE_FMT_BEGIN( "PDBufThreadLoop");
        //run core pd flow to get pd algorithm result.
        switch( _this->m_profile.BufType)
        {
        case EPDBuf_VC :
        case EPDBuf_Raw :
            ret = _this->PDCoreFlow();
            break;

        case EPDBuf_VC_Open :
        case EPDBuf_Raw_Open :
            ret = _this->PDOpenCoreFlow();
            break;

        default :
            break;

        }
        MY_LOG_IF( _this->m_bDebugEnable, "Run Core Flow -");

        //thread control
        {
            int Val;
            ::sem_getvalue( &_this->m_semPDBufThdEnd, &Val);
            MY_LOG_IF( _this->m_bDebugEnable, "[PDBufThreadLoop] semPDThdEnd before post = %d\n", Val);
            if( Val==0)
            {
                ::sem_post(&_this->m_semPDBufThdEnd); //to be 1, 1 means PD task done
            }
        }

        //ready output data
        _this->m_Status = EPD_Data_Ready;

        //check pd flow status.
        if( ret==E_3A_ERR)
        {
            MY_LOG_IF( _this->m_bDebugEnable, "[PDBufThreadLoop] PD mgr thread is colsed because of PD flow error!!");
            break;
        }
        CAM_TRACE_FMT_END();
    }

    _this->m_bEnPDBufThd = MFALSE;
    _this->m_Status      = EPD_Not_Enabled;

    MY_LOG( "PDBufThreadLoop -");
    return NULL;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//                      Data path 1 : using protect PD algorithm
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT PDMgr::SetCaliData2PDCore(MVOID* ptrInCaliData, MINT32 i4InPDInfoSz, MINT *ptrOutPDInfo)
{
    MY_LOG( "[Core] set calibration data flow");

    MRESULT ret = E_3A_ERR;

    PD_NVRAM_T *ptrCaliData = reinterpret_cast<PD_NVRAM_T *>(ptrInCaliData);

    PD_INIT_T initPdData;

    //get calibration data from host
    initPdData.rPDNVRAM.rCaliData.i4Size = m_pPDBufMgrCore->GetPDCalSz();
    memcpy( initPdData.rPDNVRAM.rCaliData.uData, ptrCaliData->rCaliData.uData, ptrCaliData->rCaliData.i4Size);


    //get tuning data
    initPdData.rPDNVRAM.rTuningData=ptrCaliData->rTuningData;


    MY_LOG( "Get calibration/tuning data");


    MY_LOG( "T %d %d %d %d %d %d %d %d %d %d",
            initPdData.rPDNVRAM.rTuningData.i4ConfIdx1[0],
            initPdData.rPDNVRAM.rTuningData.i4ConfIdx1[1],
            initPdData.rPDNVRAM.rTuningData.i4ConfIdx1[2],
            initPdData.rPDNVRAM.rTuningData.i4ConfIdx1[3],
            initPdData.rPDNVRAM.rTuningData.i4ConfIdx1[4],
            initPdData.rPDNVRAM.rTuningData.i4ConfIdx2[0],
            initPdData.rPDNVRAM.rTuningData.i4ConfIdx2[1],
            initPdData.rPDNVRAM.rTuningData.i4ConfIdx2[2],
            initPdData.rPDNVRAM.rTuningData.i4ConfIdx2[3],
            initPdData.rPDNVRAM.rTuningData.i4ConfIdx2[4]);

    for( MINT32 i=0; i<6; i++)
    {
        MY_LOG( "- %d %d %d %d %d %d",
                initPdData.rPDNVRAM.rTuningData.i4ConfTbl[i][0],
                initPdData.rPDNVRAM.rTuningData.i4ConfTbl[i][1],
                initPdData.rPDNVRAM.rTuningData.i4ConfTbl[i][2],
                initPdData.rPDNVRAM.rTuningData.i4ConfTbl[i][3],
                initPdData.rPDNVRAM.rTuningData.i4ConfTbl[i][4],
                initPdData.rPDNVRAM.rTuningData.i4ConfTbl[i][5]);
    }

    MY_LOG( "%d %d %d %d",
            initPdData.rPDNVRAM.rTuningData.i4ConfThr,
            initPdData.rPDNVRAM.rTuningData.i4SaturateLevel,
            initPdData.rPDNVRAM.rTuningData.i4SaturateThr,
            sizeof(NVRAM_LENS_PARA_STRUCT));


    MY_LOG( "FocusPDSzX %d, FocusPDSzY %d",
            initPdData.rPDNVRAM.rTuningData.i4FocusPDSizeX,
            initPdData.rPDNVRAM.rTuningData.i4FocusPDSizeY);

    //(1) set calibration data to PD core.
    ret = m_pIPdAlgo->initPD( initPdData);

    /*
    SET_PD_BLOCK_INFO_T PDsensorIF;
    IHalSensorList* const pIHalSensorL = IHalSensorList::get();
    IHalSensor* pIHalS = pIHalSensorL->createSensor("pdaf_mgr", m_i4CurrSensorDev);

    MINT32 sscen=SENSOR_SCENARIO_ID_NORMAL_CAPTURE;
    pIHalS->sendCommand(m_i4CurrSensorDev, SENSOR_CMD_GET_SENSOR_PDAF_INFO,(MINTPTR)&sscen, (MINTPTR)&PDsensorIF, 0);


    MY_LOG( "[Core] m_stPDsensorIF %d %d %d %d %d %d %d",
            PDsensorIF.i4OffsetX,
            PDsensorIF.i4OffsetY,
            PDsensorIF.i4PitchX,
            PDsensorIF.i4PitchY,
            PDsensorIF.i4SubBlkW,
            PDsensorIF.i4SubBlkH,
            PDsensorIF.i4PairNum);


    if(pIHalS) pIHalS->destroyInstance("pdaf_mgr");
    //*/


    if( ret==S_3A_OK)
    {

        SET_PD_BLOCK_INFO_T sPDSensorInfo;
        IHalSensorList* const pIHalSensorList = IHalSensorList::get();
        IHalSensor* pIHalSensor = pIHalSensorList->createSensor( "pd_mgr", m_i4CurrSensorDev);

        MINT32 scen = SENSOR_SCENARIO_ID_NORMAL_CAPTURE;
        pIHalSensor->sendCommand( m_i4CurrSensorDev, SENSOR_CMD_GET_SENSOR_PDAF_INFO,(MINTPTR)&scen, (MINTPTR)&sPDSensorInfo, 0);

        MY_LOG( "SensorInfo %d %d %d %d %d %d %d",
                sPDSensorInfo.i4OffsetX,
                sPDSensorInfo.i4OffsetY,
                sPDSensorInfo.i4PitchX,
                sPDSensorInfo.i4PitchY,
                sPDSensorInfo.i4SubBlkW,
                sPDSensorInfo.i4SubBlkH,
                sPDSensorInfo.i4PairNum);

        m_pPDBufMgrCore->SetPDBlockInfo( sPDSensorInfo);

        if( pIHalSensor)
        {
            pIHalSensor->destroyInstance("pd_mgr");
        }

        ePDWIN_ORIENTATION_T PDOrientation;
        switch( sPDSensorInfo.iMirrorFlip)
        {
        /* 0:IMAGE_NORMAL,1:IMAGE_H_MIRROR,2:IMAGE_V_MIRROR,3:IMAGE_HV_MIRROR*/
        case 3 :
            PDOrientation = ePDWIN_M1F1;
            break;
        case 2 :
            PDOrientation = ePDWIN_M0F1;
            break;
        case 1 :
            PDOrientation = ePDWIN_M1F0;
            break;
        case 0 :
        default :
            PDOrientation = ePDWIN_M0F0;
            break;
        }
        MY_LOG( "Orientation %d", PDOrientation);


        PD_CONFIG_T a_sPDConfig;
        a_sPDConfig.i4Bits      = 10;
        a_sPDConfig.i4IsPacked  = 1;
        a_sPDConfig.i4RawHeight = m_profile.uImgYsz;
        a_sPDConfig.i4RawStride = m_profile.uImgXsz*10/8;;
        a_sPDConfig.i4RawWidth  = m_profile.uImgXsz;
        a_sPDConfig.sPdBlockInfo.i4BlockNumX = (m_profile.uImgXsz-2*sPDSensorInfo.i4OffsetX)/sPDSensorInfo.i4PitchX;
        a_sPDConfig.sPdBlockInfo.i4BlockNumY = (m_profile.uImgYsz-2*sPDSensorInfo.i4OffsetY)/sPDSensorInfo.i4PitchY;
        a_sPDConfig.sPdBlockInfo.i4OffsetX   = sPDSensorInfo.i4OffsetX;
        a_sPDConfig.sPdBlockInfo.i4OffsetY   = sPDSensorInfo.i4OffsetY;
        if( sPDSensorInfo.i4PairNum >16)
        {
            sPDSensorInfo.i4PairNum = 16;
        }
        a_sPDConfig.sPdBlockInfo.i4PairNum = sPDSensorInfo.i4PairNum;
        a_sPDConfig.sPdBlockInfo.i4PitchX  = sPDSensorInfo.i4PitchX;
        a_sPDConfig.sPdBlockInfo.i4PitchY  = sPDSensorInfo.i4PitchY;
        a_sPDConfig.sPdBlockInfo.i4SubBlkH = sPDSensorInfo.i4SubBlkH;
        a_sPDConfig.sPdBlockInfo.i4SubBlkW = sPDSensorInfo.i4SubBlkW;
        for( MUINT32 Pidx=0; Pidx<sPDSensorInfo.i4PairNum; Pidx++)
        {
            MUINT32 PosL_X = sPDSensorInfo.i4PosL[Pidx][0];
            MUINT32 PosL_Y = sPDSensorInfo.i4PosL[Pidx][1];
            MUINT32 PosR_X = sPDSensorInfo.i4PosR[Pidx][0];
            MUINT32 PosR_Y = sPDSensorInfo.i4PosR[Pidx][1];
            MUINT32 idx    = Pidx;
            /* Flip*/
            if( PDOrientation==ePDWIN_M0F1 || PDOrientation==ePDWIN_M1F1)
            {
                PosL_Y = m_profile.uImgYsz - ( PosL_Y + (a_sPDConfig.sPdBlockInfo.i4BlockNumY-1)*sPDSensorInfo.i4PitchY) - 1;
                PosR_Y = m_profile.uImgYsz - ( PosR_Y + (a_sPDConfig.sPdBlockInfo.i4BlockNumY-1)*sPDSensorInfo.i4PitchY) - 1;
                idx    = (sPDSensorInfo.i4PairNum - 1) - Pidx; /* For configuring PD algo, the first element Y coordinate should be the smallest. */
            }

            /* Mirror*/
            if( PDOrientation==ePDWIN_M1F0 || PDOrientation==ePDWIN_M1F1)
            {
                PosL_X = m_profile.uImgXsz - ( PosL_X + (a_sPDConfig.sPdBlockInfo.i4BlockNumX-1)*sPDSensorInfo.i4PitchX) - 1;
                PosR_X = m_profile.uImgXsz - ( PosR_X + (a_sPDConfig.sPdBlockInfo.i4BlockNumX-1)*sPDSensorInfo.i4PitchX) - 1;
            }

            MY_LOG( "PDPos [L](%d, %d)->(%d, %d), [R](%d, %d)->(%d, %d)",
                    sPDSensorInfo.i4PosL[Pidx][0],
                    sPDSensorInfo.i4PosL[Pidx][1],
                    PosL_X,
                    PosL_Y,
                    sPDSensorInfo.i4PosR[Pidx][0],
                    sPDSensorInfo.i4PosR[Pidx][1],
                    PosR_X,
                    PosR_Y);

            sPDSensorInfo.i4PosL[Pidx][0] = PosL_X;
            sPDSensorInfo.i4PosL[Pidx][1] = PosL_Y;
            sPDSensorInfo.i4PosR[Pidx][0] = PosR_X;
            sPDSensorInfo.i4PosR[Pidx][1] = PosR_Y;

            a_sPDConfig.sPdBlockInfo.i4PosL[idx][0] = sPDSensorInfo.i4PosL[Pidx][0];
            a_sPDConfig.sPdBlockInfo.i4PosL[idx][1] = sPDSensorInfo.i4PosL[Pidx][1];
            a_sPDConfig.sPdBlockInfo.i4PosR[idx][0] = sPDSensorInfo.i4PosR[Pidx][0];
            a_sPDConfig.sPdBlockInfo.i4PosR[idx][1] = sPDSensorInfo.i4PosR[Pidx][1];
        }
        MY_LOG_IF( 1,
                   "[Core] %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
                   a_sPDConfig.i4Bits,
                   a_sPDConfig.i4IsPacked,
                   a_sPDConfig.i4RawHeight,
                   a_sPDConfig.i4RawStride,
                   a_sPDConfig.i4RawWidth,
                   a_sPDConfig.sPdBlockInfo.i4BlockNumX,
                   a_sPDConfig.sPdBlockInfo.i4BlockNumY,
                   a_sPDConfig.sPdBlockInfo.i4OffsetX,
                   a_sPDConfig.sPdBlockInfo.i4OffsetY,
                   a_sPDConfig.sPdBlockInfo.i4PairNum,
                   a_sPDConfig.sPdBlockInfo.i4PitchX,
                   a_sPDConfig.sPdBlockInfo.i4PitchY,
                   a_sPDConfig.sPdBlockInfo.i4SubBlkH,
                   a_sPDConfig.sPdBlockInfo.i4SubBlkW);

        //(2) configure pd orientation information.
        m_pIPdAlgo->setPDOrientation( PDOrientation);

        //(3) configure pd algorithm.
        ret = m_pIPdAlgo->setPDBlockInfo(a_sPDConfig);

        //(4) get information for hybrid af.
        if( ret==S_3A_OK)
        {
            memset( ptrOutPDInfo, 0, i4InPDInfoSz*sizeof(MINT32));
            m_pIPdAlgo->getInfoForHybridAF( ptrOutPDInfo);
            MY_LOG( "[Core] configure PD algo done %d %d", ptrOutPDInfo[0], ptrOutPDInfo[1]);
        }
    }
    else
    {
        MY_LOG( "Load PDAF calib data error!!");

        for( MINT32 kidx=0; kidx<0x600; kidx=kidx+16)
        {
            MY_LOG( "[Core] ===0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x,\n",
                    initPdData.rPDNVRAM.rCaliData.uData[kidx   ],
                    initPdData.rPDNVRAM.rCaliData.uData[kidx+ 1],
                    initPdData.rPDNVRAM.rCaliData.uData[kidx+ 2],
                    initPdData.rPDNVRAM.rCaliData.uData[kidx+ 3],
                    initPdData.rPDNVRAM.rCaliData.uData[kidx+ 4],
                    initPdData.rPDNVRAM.rCaliData.uData[kidx+ 5],
                    initPdData.rPDNVRAM.rCaliData.uData[kidx+ 6],
                    initPdData.rPDNVRAM.rCaliData.uData[kidx+ 7],
                    initPdData.rPDNVRAM.rCaliData.uData[kidx+ 8],
                    initPdData.rPDNVRAM.rCaliData.uData[kidx+ 9],
                    initPdData.rPDNVRAM.rCaliData.uData[kidx+10],
                    initPdData.rPDNVRAM.rCaliData.uData[kidx+11],
                    initPdData.rPDNVRAM.rCaliData.uData[kidx+12],
                    initPdData.rPDNVRAM.rCaliData.uData[kidx+13],
                    initPdData.rPDNVRAM.rCaliData.uData[kidx+14],
                    initPdData.rPDNVRAM.rCaliData.uData[kidx+15]);
        }
    }


    //check configure core PD algorithm is correct or not.
    if( ret==S_3A_OK)
        MY_LOG("[Core] PD init data done");
    else
        MY_LOG("[Core] PD init data error, close PDAF");


    return ret;
}

MRESULT PDMgr::PDCoreFlow()
{
    MY_LOG_IF( m_bDebugEnable, "%s +", __FUNCTION__);

    MRESULT ret=E_3A_ERR;

    if( (m_pIPdAlgo!=NULL))
    {
        MUINT16 *ptrbuf = NULL;
        PD_EXTRACT_INPUT_T a_sPDInput;
        PD_EXTRACT_DATA_T  a_sPDOutput[AF_PSUBWIN_NUM];

        //(1) select data buffer type
        if( m_profile.BufType==EPDBuf_VC)
        {
            a_sPDInput.pRawBuf = NULL; //  = (MVOID *)m_pdaf_rawbuf;
            ptrbuf = m_pPDBufMgrCore->ConvertPDBufFormat( m_databuf_size, m_databuf, m_frmCnt);
            a_sPDInput.pPDBuf = reinterpret_cast<MVOID *>(ptrbuf);
        }
        else
        {
            a_sPDInput.pRawBuf = reinterpret_cast<MVOID *>(m_databuf);
            a_sPDInput.pPDBuf  = NULL;
        }


        char value[200] = {'\0'};
        property_get("vc.dump.enable", value, "0");
        MBOOL bEnable = atoi(value);
        if (bEnable)
        {
            char fileName[64];
            sprintf(fileName, "/sdcard/vc/%d_dma_in_thread.raw", m_frmCnt);
            FILE *fp = fopen(fileName, "w");
            if (NULL == fp)
            {
                return MFALSE;
            }
            fwrite(reinterpret_cast<void *>(m_databuf), 1, m_databuf_size, fp);
            fclose(fp);
        }



        for( MINT32 i=0; i<m_numROI; i++)
        {
            a_sPDInput.sFocusWin.i4W = m_ROI[i].i4W;
            a_sPDInput.sFocusWin.i4H = m_ROI[i].i4H;
            a_sPDInput.sFocusWin.i4X = m_ROI[i].i4X;
            a_sPDInput.sFocusWin.i4Y = m_ROI[i].i4Y;

#if 0
            MY_LOG_IF( m_bDebugEnable, "win %d [WinPos] %d, %d, %d, %d\n",
                       i,
                       a_sPDInput.sFocusWin.i4X,
                       a_sPDInput.sFocusWin.i4Y,
                       a_sPDInput.sFocusWin.i4W,
                       a_sPDInput.sFocusWin.i4H);
#endif
            ret = m_pIPdAlgo->extractPD( a_sPDInput, a_sPDOutput[i]);
            if( ret==E_3A_ERR) break;
        }

        if( ret==S_3A_OK)
        {
            SPDResult_T PDRes;
            //reset output result.
            memset( &PDRes, 0, sizeof(SPDResult_T));

            //output all 0	result directly once current lens position is 0
            if( m_curLensPos)
            {
                for (MINT32 i = 0; i < m_numROI; i++)
                {
                    m_sPDInput[i].bIsFace      = 0;
                    m_sPDInput[i].i4CurLensPos = m_curLensPos;
                    m_sPDInput[i].sPDExtractData.pPDLData   = a_sPDOutput[i].pPDLData;
                    m_sPDInput[i].sPDExtractData.pPDLPos    = a_sPDOutput[i].pPDLPos;
                    m_sPDInput[i].sPDExtractData.pPDRData   = a_sPDOutput[i].pPDRData;
                    m_sPDInput[i].sPDExtractData.pPDRPos    = a_sPDOutput[i].pPDRPos;
                    m_sPDInput[i].sPDExtractData.sPdWin.i4H = a_sPDOutput[i].sPdWin.i4H;
                    m_sPDInput[i].sPDExtractData.sPdWin.i4W = a_sPDOutput[i].sPdWin.i4W;
                    m_sPDInput[i].sPDExtractData.sPdWin.i4X = a_sPDOutput[i].sPdWin.i4X;
                    m_sPDInput[i].sPDExtractData.sPdWin.i4Y = a_sPDOutput[i].sPdWin.i4Y;

                    ret = m_pIPdAlgo->handlePD(m_sPDInput[i],m_sPDOutput[i]);

#if 0
                    MY_LOG_IF( m_bDebugEnable, "win %d [Confidence] %d [LensPos] %d->%d [value] %d\n",
                               i,
                               m_sPDOutput[i].i4ConfidenceLevel,
                               m_curLensPos,
                               m_sPDOutput[i].i4FocusLensPos,
                               (MINT32)(m_sPDOutput[i].fPdValue*1000));
#endif

                    if( ret==E_3A_ERR)
                    {
                        MY_ERR("%s", __FUNCTION__);
                        break;
                    }

                    //set output data puffer.
                    PDRes.ROIRes[i].Defocus                = Boundary(0, m_sPDOutput[i].i4FocusLensPos, 1023);
                    PDRes.ROIRes[i].DefocusConfidence      = m_sPDOutput[i].i4ConfidenceLevel;
                    PDRes.ROIRes[i].DefocusConfidenceLevel = m_sPDOutput[i].i4ConfidenceLevel;
                    PDRes.ROIRes[i].PhaseDifference        = (MINT32)(m_sPDOutput[i].fPdValue*1000);
                }
            }

            if( ret==S_3A_OK)
            {
                Mutex::Autolock lock(m_Lock);
                m_vPDOutput.push_back( PDRes);
                if( 2<m_vPDOutput.size())
                {
                    //pop_front and keep only last 2 result
                    m_vPDOutput.erase( m_vPDOutput.begin(), m_vPDOutput.end()-1);
                    MY_LOG("pop_front, sz=%d\n", m_vPDOutput.size());
                }
            }
        }
    }

    MY_LOG_IF( m_bDebugEnable, "%s -", __FUNCTION__);
    return ret;

}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//                     Data path 2 : using 3rd party PD algorithm
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


MRESULT PDMgr::SetCaliData2PDOpenCore(MVOID* ptrInCaliData, MINT32 i4InPDInfoSz, MINT32 *ptrOutPDInfo)
{
    MY_LOG("[Open] Set calibration data flow");

    PD_NVRAM_T *ptrCaliData = reinterpret_cast<PD_NVRAM_T *>(ptrInCaliData);
    MUINT8 *ptrData = ptrCaliData->rCaliData.uData;
    MINT32 sz = m_pPDBufMgrOpen->GetPDCalSz();

    //set calibration to PD open core.
    m_pPDBufMgrOpen->SetCalibrationData( sz, ptrData);

    //get PD related information form Hybrid AF.
    m_pPDBufMgrOpen->GetPDInfo2HybridAF( i4InPDInfoSz, ptrOutPDInfo);

    return S_3A_OK;

}

MRESULT PDMgr::PDOpenCoreFlow()
{
    MY_LOG( "[Open] defocus flow");

    SPDROIInput_T iPDInputData;
    SPDROIResult_T oPdOutputData;


    m_pPDBufMgrOpen->SetDataBuf( m_databuf_size, m_databuf, m_frmCnt);

    MY_LOG( "[Open] number ROI=%d", m_numROI);

    SPDResult_T PDRes;

    //reset output result.
    memset( &PDRes, 0, sizeof(SPDResult_T));

    //output all 0	result directly once current lens position is 0
    if( m_curLensPos)
    {
        for( MINT32 i=0; i<m_numROI; i++)
        {
            iPDInputData.curLensPos   = m_curLensPos;
            iPDInputData.XSizeOfImage = m_profile.uImgXsz;
            iPDInputData.YSizeOfImage = m_profile.uImgYsz;
            iPDInputData.ROI.i4XStart = m_ROI[i].i4X;
            iPDInputData.ROI.i4YStart = m_ROI[i].i4Y;
            iPDInputData.ROI.i4XEnd   = m_ROI[i].i4X + m_ROI[i].i4W;
            iPDInputData.ROI.i4YEnd   = m_ROI[i].i4Y + m_ROI[i].i4H;
            iPDInputData.ROI.i4Info   = m_ROI[i].i4Info;

            m_pPDBufMgrOpen->GetDefocus( iPDInputData, oPdOutputData);


            PDRes.ROIRes[i].Defocus                = Boundary(0, oPdOutputData.Defocus, 1023);
            PDRes.ROIRes[i].DefocusConfidence      = oPdOutputData.DefocusConfidence;
            PDRes.ROIRes[i].DefocusConfidenceLevel = oPdOutputData.DefocusConfidenceLevel;
            PDRes.ROIRes[i].PhaseDifference        = oPdOutputData.PhaseDifference;


            MY_LOG( "[PDOpenCoreFlow][PDAF]win %d [Confidence] %d [LensPos] %d->%d [value] %d",
                    i,
                    PDRes.ROIRes[i].DefocusConfidence,
                    m_curLensPos,
                    PDRes.ROIRes[i].Defocus,
                    PDRes.ROIRes[i].PhaseDifference);
        }
    }


    {
        Mutex::Autolock lock( m_Lock);
        m_vPDOutput.push_back( PDRes);
        if( 2<m_vPDOutput.size())
        {
            //pop_front and keep only last 2 result
            m_vPDOutput.erase( m_vPDOutput.begin(), m_vPDOutput.end()-1);
            MY_LOG("pop_front, sz=%d", m_vPDOutput.size());
        }
    }
    return S_3A_OK;

}


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//                                   Interface
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT PDMgr::GetVersionOfPdafLibrary(SPDLibVersion_t &tOutSWVer)
{
    MRESULT ret=E_3A_ERR;

    if( m_bEnPDBufThd)
    {

        switch( m_profile.BufType)
        {
        case EPDBuf_VC :
        case EPDBuf_Raw :
            tOutSWVer.MajorVersion = 1;
            tOutSWVer.MinorVersion = 1;
            ret = S_3A_OK;
            break;

        case EPDBuf_VC_Open :
        case EPDBuf_Raw_Open :
            //set calibration to PD open core.
            ret = m_pPDBufMgrOpen->GetVersionOfPdafLibrary( tOutSWVer);
            break;

        default :
            break;

        }

        MY_LOG( "%s = %d.%d", __FUNCTION__, (int)tOutSWVer.MajorVersion, (int)tOutSWVer.MinorVersion);
    }


    return ret;


}

MRESULT PDMgr::setPDCaliData( MVOID *ptrInCaliData, MINT32 &i4OutInfoSz, MINT32 **ptrOutInfo)
{

    //This function is used to set PD calibration data and output PD related data for Hybrid AF.
    MRESULT ret=E_3A_ERR;

    if( m_bEnPDBufThd)
    {

        switch( m_profile.BufType)
        {
        case EPDBuf_VC :
        case EPDBuf_Raw :
            ret = SetCaliData2PDCore( ptrInCaliData, 10, m_i4PDInfo2HybirdAF);
            break;

        case EPDBuf_VC_Open :
        case EPDBuf_Raw_Open :
            ret = SetCaliData2PDOpenCore( ptrInCaliData, 10, m_i4PDInfo2HybirdAF);
            break;

        default :
            break;

        }

        if( ret==E_3A_ERR)
        {
            closeThread();
            i4OutInfoSz   = 0;
            (*ptrOutInfo) = NULL;
        }
        else
        {
            //This m_i4PDInfo2HybirdAF arrary size is defined by core pd algorithm
            i4OutInfoSz   = 10;
            (*ptrOutInfo) = m_i4PDInfo2HybirdAF;
        }
    }


    return ret;
}

//----------------------------------------------------------------------------------------------------
MRESULT PDMgr::postToPDTask( SPDInputData_t *ptrInputData)
{
    MRESULT ret = E_3A_ERR;

    if( m_bEnPDBufThd)
    {
        if( ptrInputData)
        {

            //Run thread if PD task is not busy.
            int Val;
            ::sem_getvalue( &m_semPDBufThdEnd, &Val);
            MY_LOG_IF( m_bDebugEnable, "[postToPDBufThread] sem_post m_semPDBuf, m_semPDBufThdEnd before wait = %d", Val);
            //checking PD thread is busy or not.
            if( Val==1)
            {
                m_frmCnt = ptrInputData->frmNum;

                //(1) get data buffer. this size is dma size.
                m_databuf_size = ptrInputData->databuf_size;
                if( !m_databuf)
                    m_databuf = new MUINT8 [m_databuf_size];

                memcpy( m_databuf, ptrInputData->databuf_virtAddr, sizeof(MUINT8)*m_databuf_size);


                char value[200] = {'\0'};
                property_get( "vc.dump.enable", value, "0");
                MBOOL bEnable = atoi(value);
                if( bEnable)
                {
                    char fileName[64];
                    sprintf(fileName, "/sdcard/vc/%d_dma_in.raw", m_frmCnt);
                    FILE *fp = fopen(fileName, "w");
                    if (NULL == fp)
                    {
                        return MFALSE;
                    }
                    fwrite(reinterpret_cast<void *>(m_databuf), 1, m_databuf_size, fp);
                    fclose(fp);
                }


                //(2) get PD ROI from host.
                m_numROI = ptrInputData->numROI>AF_PSUBWIN_NUM ? AF_PSUBWIN_NUM : ptrInputData->numROI;
                for( MUINT32 i=0; i<m_numROI; i++)
                {
                    m_ROI[i].i4X    = ptrInputData->ROI[i].i4XStart;
                    m_ROI[i].i4Y    = ptrInputData->ROI[i].i4YStart;
                    m_ROI[i].i4W    = ptrInputData->ROI[i].i4XEnd - ptrInputData->ROI[i].i4XStart;
                    m_ROI[i].i4H    = ptrInputData->ROI[i].i4YEnd - ptrInputData->ROI[i].i4YStart;
                    m_ROI[i].i4Info = ptrInputData->ROI[i].i4Info;
                }

                //(3) img size should be same as profile information
                if( m_profile.uImgXsz!=ptrInputData->XSizeOfImage || m_profile.uImgYsz!=ptrInputData->YSizeOfImage)
                {
                    MY_LOG( "[PDAF] profile (%d, %d), coordinate (%d, %d)",
                            m_profile.uImgXsz,
                            m_profile.uImgYsz,
                            ptrInputData->XSizeOfImage,
                            ptrInputData->YSizeOfImage);

                    m_profile.uImgXsz = ptrInputData->XSizeOfImage;
                    m_profile.uImgYsz = ptrInputData->YSizeOfImage;
                }


                //(4) get current lens' position
                m_curLensPos = ptrInputData->curLensPos;
                //(5) run task.
                postToPDBufThread();
            }
            else
            {
                MY_LOG( "[PDAF] Can not post to PD thread because thread is busy.");

            }
            ret = S_3A_OK;

        }
        else
        {
            MY_LOG_IF( m_bDebugEnable, "[postToPDBufThread] ERR : input is NULL, close thread");
            ret = E_3A_ERR;
        }


        if( ret==E_3A_ERR)
        {
            closeThread();
        }
    }
    else
    {
        MY_LOG_IF( m_bDebugEnable, "Can not post to pd thread because of thread is closed");
        ret = E_3A_ERR;
    }


    return ret;
}


MRESULT PDMgr::getPDTaskResult( SPDOutputData_t *ptrOutputRes)
{
    MRESULT ret = E_3A_ERR;

    if( m_bEnPDBufThd && ptrOutputRes->numRes!=0)
    {
        Mutex::Autolock lock(m_Lock);
        if( m_vPDOutput.size())
        {
            //output
            ptrOutputRes->numRes = ptrOutputRes->numRes>m_numROI ? m_numROI : ptrOutputRes->numRes;
            memcpy( ptrOutputRes->Res, m_vPDOutput.begin()->ROIRes, sizeof(SPDROIResult_T)*(ptrOutputRes->numRes));

            for( MUINT32 i=0; i<ptrOutputRes->numRes; i++)
            {

                MY_LOG_IF( m_bDebugEnable,
                           "[getPDRes][%d] ToPos %4d, C %3d, CL %3d, PD %5d\n",
                           i,
                           ptrOutputRes->Res[i].Defocus,
                           ptrOutputRes->Res[i].DefocusConfidence,
                           ptrOutputRes->Res[i].DefocusConfidenceLevel,
                           ptrOutputRes->Res[i].PhaseDifference);
            }

            //pop_front
            m_vPDOutput.erase( m_vPDOutput.begin());
            ret = S_3A_OK;
        }
        else
        {
            MY_LOG_IF( m_bDebugEnable, "[getPDRes] WAR : np PD result");
            ret = E_AF_BUSY;
        }
    }

    return ret;

}


MRESULT PDMgr::GetDebugInfo( AF_DEBUG_INFO_T &sOutDbgInfo)
{
    MRESULT ret = E_3A_ERR;

    //Not support open pd library.
    switch( m_profile.BufType)
    {
    case EPDBuf_VC :
    case EPDBuf_Raw :
        if( m_pIPdAlgo)
        {
            ret = m_pIPdAlgo->getDebugInfo(sOutDbgInfo);
        }
        ret = S_AE_OK;
        break;
    }

    return ret;
}

};  //  namespace NS3A

