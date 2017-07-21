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
#define LOG_TAG "Hal3ARawSMVR"

#ifndef ENABLE_MY_LOG
    #define ENABLE_MY_LOG       (1)
#endif

#include <cutils/properties.h>
#include <string.h>
#include <aaa_log.h>
#include <IThreadRaw.h>
#include <IHalSensor.h>
#ifdef USING_MTK_LDVT /*[EP_TEMP]*/ //[FIXME] TempTestOnly - USING_FAKE_SENSOR
#include <drv/src/isp/mt6797/iopipe/CamIO/FakeSensor.h>
#endif

#include "Hal3ARawSMVR.h"
#include <ae_mgr/ae_mgr.h>
#include <awb_mgr/awb_mgr_if.h>
#include <sensor_mgr/aaa_sensor_mgr.h>
#include <sensor_mgr/aaa_sensor_buf_mgr.h>
#include <isp_tuning/isp_tuning_mgr.h>
#include <isp_mgr/isp_mgr.h>
#include <state_mgr/aaa_state_mgr.h>
#include <aaa_common_custom.h>

#include <CamIO/INormalPipe.h>
#include <exif/IBaseCamExif.h>
#include <debug_exif/aaa/dbg_aaa_param.h>

#if CAM3_AF_FEATURE_EN
#include <state_mgr_af/af_state_mgr.h>
#include <debug_exif/aaa/dbg_af_param.h>
#include <af_feature.h>
#include <af_algo_if.h>
#include <af_mgr/af_mgr_if.h>
#endif

#if CAM3_FLASH_FEATURE_EN
#include <flash_mgr/flash_mgr.h>
#endif

#if CAM3_FLICKER_FEATURE_EN
#include <flicker_hal.h>
#endif

#if CAM3_LSC_FEATURE_EN
#include <ILscMgr.h>
#endif

#if defined(HAVE_AEE_FEATURE)
#include <aee.h>
#define AEE_ASSERT_3A_HAL(String) \
          do { \
              aee_system_exception( \
                  "Hal3A", \
                  NULL, \
                  DB_OPT_DEFAULT, \
                  String); \
          } while(0)
#else
#define AEE_ASSERT_3A_HAL(String)
#endif

#define GET_PROP(prop, init, val)\
{\
    char value[PROPERTY_VALUE_MAX] = {'\0'};\
    property_get(prop, value, (init));\
    (val) = atoi(value);\
}

using namespace android;
using namespace NS3Av3;
using namespace NSIspTuningv3;

class Hal3ARawSMVRImp : public Hal3ARawSMVR
{
public:
    static I3AWrapper*  createInstance(MINT32 const i4SensorOpenIndex, MINT32 i4SubsampleCount);
    virtual MVOID       destroyInstance();
    virtual MBOOL       start();
    virtual MBOOL       stop();
    virtual MVOID       setSensorMode(MINT32 i4SensorMode);
    virtual MBOOL       generateP2(MINT32 flowType, const NSIspTuning::ISP_INFO_T& rIspInfo, void* pRegBuf, ResultP2_T* pResultP2);
    virtual MBOOL       validateP1(const ParamIspProfile_T& rParamIspProfile, MBOOL fgPerframe);
    virtual MBOOL       setParams(Param_T const &rNewParam);
    virtual MBOOL       setAfParams(AF_Param_T const &rNewParam);
    virtual MBOOL       autoFocus();
    virtual MBOOL       cancelAutoFocus();
    virtual MVOID       setFDEnable(MBOOL fgEnable);
    virtual MBOOL       setFDInfo(MVOID* prFaces, MVOID* prAFFaces);
    virtual MBOOL       setFlashLightOnOff(MBOOL bOnOff/*1=on; 0=off*/, MBOOL bMainPre/*1=main; 0=pre*/);
    virtual MBOOL       chkMainFlashOnCond() const;
    virtual MBOOL       chkPreFlashOnCond() const;
    virtual MBOOL       isStrobeBVTrigger() const;
    virtual MBOOL       chkCapFlash() const {return m_bIsHighQualityCaptureOn;}
    virtual MINT32      getCurrResult(MUINT32 i4FrmId, Result_T& rResult, MINT32 i4SubsampleIndex = 0) const;
    virtual MINT32      getCurrentHwId() const;
    virtual MBOOL       postCommand(ECmd_T const r3ACmd, const ParamIspProfile_T* pParam = 0);
    virtual MINT32      send3ACtrl(E3ACtrl_T e3ACtrl, MINTPTR iArg1, MINTPTR iArg2);
    virtual MUINT32     queryMagicNumber() const {return m_u4MagicNumber;}
    virtual VOID        queryTgSize(MINT32 &i4TgWidth, MINT32 &i4TgHeight);
    virtual MINT32      attachCb(I3ACallBack* cb);
    virtual MINT32      detachCb(I3ACallBack* cb);
    virtual VOID        queryHbinSize(MINT32 &i4HbinWidth, MINT32 &i4HbinHeight);

protected:  //    Ctor/Dtor.
                        Hal3ARawSMVRImp(MUINT32 const i4SensorDevId, MINT32 const i4SensorOpenIndex);
    virtual             ~Hal3ARawSMVRImp(){}

                        Hal3ARawSMVRImp(const Hal3ARawSMVRImp&);
                        Hal3ARawSMVRImp& operator=(const Hal3ARawSMVRImp&);

    MBOOL               init(MINT32 i4SubsampleCount);
    MBOOL               uninit();
    MRESULT             updateTGInfo();
    MINT32              config();
    MBOOL               get3AEXIFInfo(EXIF_3A_INFO_T& rExifInfo) const;
    MBOOL               getASDInfo(ASDInfo_T &a_rASDInfo) const;
    MBOOL               getP1DbgInfo(AAA_DEBUG_INFO_T& rDbg3AInfo, DEBUG_SHAD_ARRAY_INFO_T& rDbgShadTbl, NSIspExifDebug::IspExifDebugInfo_T& rDbgIspInfo) const;

private:
    MINT32              m_3ALogEnable;
    volatile int        m_Users;
    mutable Mutex       m_Lock;
    Mutex               mP2Mtx;
    Mutex               m3AOperMtx1;
    Mutex               m3AOperMtx2;
    MINT32              m_i4SensorIdx;
    MINT32              m_i4SensorDev;
    MUINT32             m_u4SensorMode;
    MUINT               m_u4TgInfo;
    MBOOL               m_bEnable3ASetParams;
    MBOOL               m_bFaceDetectEnable;
    MINT32              m_i4InCaptureProcess;
    MINT32              m_i4TgWidth;
    MINT32              m_i4TgHeight;
    MBOOL               mbIsHDRShot;
    MINT32              m_i4HbinWidth;
    MINT32              m_i4HbinHeight;
    MBOOL               m_fgEnableShadingMeta;
    MINT32              m_i4SubsampleCount;
    Result_T            m_rResultLastSof;

private:
    NSCam::NSIoPipe::NSCamIOPipe::INormalPipe* m_pCamIO;
    IspTuningMgr*       m_pTuning;
    IThreadRaw*         m_pThreadRaw;
    StateMgr*           m_pStateMgr;
    AfStateMgr*         m_pAfStateMgr;
    Param_T             m_rParam;
    AF_Param_T          m_rAfParam;
    I3ACallBack*        m_pCbSet;
    MUINT32             m_u4MagicNumber;
    MBOOL               m_bIsHighQualityCaptureOn;
    SMVR_GAIN_DELAY_T   m_rGainDelay;
};

template <MINT32 sensorDevId>
class Hal3ARawSMVRImpDev : public Hal3ARawSMVRImp
{
public:
    static Hal3ARawSMVRImpDev* getInstance(MUINT32 const i4SensorDevId, MINT32 const i4SensorOpenIndex)
    {
        static Hal3ARawSMVRImpDev<sensorDevId> singleton(i4SensorDevId, i4SensorOpenIndex);
        return &singleton;
    }
    Hal3ARawSMVRImpDev(MUINT32 const i4SensorDevId, MINT32 const i4SensorOpenIndex)
        : Hal3ARawSMVRImp(i4SensorDevId, i4SensorOpenIndex)
    {
    }

private:

};

I3AWrapper*
Hal3ARawSMVR::
createInstance(MINT32 const i4SensorOpenIndex, MINT32 i4SubsampleCount)
{
    return Hal3ARawSMVRImp::createInstance(i4SensorOpenIndex, i4SubsampleCount);
}

I3AWrapper*
Hal3ARawSMVRImp::
createInstance(MINT32 const i4SensorOpenIndex, MINT32 i4SubsampleCount)
{
#ifdef USING_MTK_LDVT
    IHalSensorList*const pHalSensorList = TS_FakeSensorList::getTestModel();
#else
    IHalSensorList*const pHalSensorList = IHalSensorList::get();
#endif
    MUINT32 i4SensorDevId = pHalSensorList->querySensorDevIdx(i4SensorOpenIndex);



    switch (i4SensorDevId)
    {
        case SENSOR_DEV_MAIN:
            Hal3ARawSMVRImpDev<SENSOR_DEV_MAIN>::getInstance(i4SensorDevId, i4SensorOpenIndex)->init(i4SubsampleCount);
            return Hal3ARawSMVRImpDev<SENSOR_DEV_MAIN>::getInstance(i4SensorDevId, i4SensorOpenIndex);
        break;
        case SENSOR_DEV_SUB:
            Hal3ARawSMVRImpDev<SENSOR_DEV_SUB>::getInstance(i4SensorDevId, i4SensorOpenIndex)->init(i4SubsampleCount);
            return Hal3ARawSMVRImpDev<SENSOR_DEV_SUB>::getInstance(i4SensorDevId, i4SensorOpenIndex);
        break;
        case SENSOR_DEV_MAIN_2:
            Hal3ARawSMVRImpDev<SENSOR_DEV_MAIN_2>::getInstance(i4SensorDevId, i4SensorOpenIndex)->init(i4SubsampleCount);
            return Hal3ARawSMVRImpDev<SENSOR_DEV_MAIN_2>::getInstance(i4SensorDevId, i4SensorOpenIndex);
        break;
        default:
            MY_ERR("Unsupport sensor device ID: %d\n", i4SensorDevId);
            AEE_ASSERT_3A_HAL("Unsupport sensor device.");
            return MNULL;
    }
}

MVOID
Hal3ARawSMVRImp::
destroyInstance()
{
    MY_LOG("[%s]", __FUNCTION__);
    uninit();
}

Hal3ARawSMVRImp::
Hal3ARawSMVRImp(MUINT32 const i4SensorDevId, MINT32 const i4SensorOpenIndex)
    : m_pCamIO(NULL)
    , m_pTuning(NULL)
    , m_i4SensorIdx(i4SensorOpenIndex)
    , m_i4SensorDev(i4SensorDevId)
    , m_u4SensorMode(0)
    , m_bEnable3ASetParams(MTRUE)
    , m_u4MagicNumber(0)
    , mbIsHDRShot(MFALSE)
    , m_pCbSet(NULL)
    , m_i4HbinWidth (1000)
    , m_i4HbinHeight(1000)
    , m_i4TgWidth   (1000)
    , m_i4TgHeight  (1000)
    , m_fgEnableShadingMeta(MTRUE)
    , m_i4SubsampleCount(1)
{
    MY_LOG("[%s] sensorDev(%d) sensorIdx(%d)", __FUNCTION__, i4SensorDevId, i4SensorOpenIndex);
}

MBOOL
Hal3ARawSMVRImp::
init(MINT32 i4SubsampleCount)
{
    GET_PROP("debug.camera.log", "0", m_3ALogEnable);
    if ( m_3ALogEnable == 0 ) {
        GET_PROP("debug.camera.log.hal3a", "0", m_3ALogEnable);
    }

    MY_LOG("[%s] m_Users: %d \n", __FUNCTION__, m_Users);

    // check user count
    MRESULT ret = S_3A_OK;
    MBOOL bRet = MTRUE;
    Mutex::Autolock lock(m_Lock);

    if (m_Users > 0)
    {
        MY_LOG("[%s] %d has created \n", __FUNCTION__, m_Users);
        android_atomic_inc(&m_Users);
        return S_3A_OK;
    }


    m_i4SubsampleCount = i4SubsampleCount;
    m_rResultLastSof.i4FrmId = -1; //result havn't update yet
    m_fgEnableShadingMeta = MTRUE;

    // init Thread and state mgr
    m_pThreadRaw = IThreadRaw::createInstance((Hal3ARaw*)this, m_i4SensorDev, m_i4SensorIdx);
    m_pStateMgr = new StateMgr(m_i4SensorDev);
    m_pAfStateMgr = new AfStateMgr(m_i4SensorDev);

    m_pStateMgr->setThreadRaw(m_pThreadRaw);

    // AE init

    IAeMgr::getInstance().cameraPreviewInit(m_i4SensorDev, m_i4SensorIdx, m_rParam);
    //IAeMgr::getInstance().SMVR(m_i1SubSampleNum);

    // AWB init
    IAwbMgr::getInstance().init(m_i4SensorDev, m_i4SensorIdx);

    // AF init
#if CAM3_AF_FEATURE_EN
    //IAfMgr::getInstance().setCallbacks(m_i4SensorDev, mpCbSet);
    IAfMgr::getInstance().init(m_i4SensorDev, m_i4SensorIdx);
#endif

    //FLASH init
#if CAM3_FLASH_FEATURE_EN
    FlashMgr::getInstance().init(m_i4SensorDev, m_i4SensorIdx);
#endif

    // TuningMgr init
    if (m_pTuning == NULL)
    {
        m_pTuning = &IspTuningMgr::getInstance();
        if (!m_pTuning->init(m_i4SensorDev, m_i4SensorIdx, i4SubsampleCount))
        {
            MY_ERR("Fail to init IspTuningMgr (%d,%d)", m_i4SensorDev, m_i4SensorIdx);
            AEE_ASSERT_3A_HAL("Fail to init IspTuningMgr");
            return MFALSE;
        }
    }

    // state mgr transit to Init state.
    bRet = postCommand(ECmd_Init);
    if (!bRet) AEE_ASSERT_3A_HAL("ECmd_Init fail.");

    MY_LOG("[%s] done\n", __FUNCTION__);
    android_atomic_inc(&m_Users);
    return S_3A_OK;
}

MBOOL
Hal3ARawSMVRImp::
uninit()
{
    MRESULT ret = S_3A_OK;
    MBOOL bRet = MTRUE;

    Mutex::Autolock lock(m_Lock);

    // If no more users, return directly and do nothing.
    if (m_Users <= 0)
    {
        return S_3A_OK;
    }
    MY_LOG("[%s] m_Users: %d \n", __FUNCTION__, m_Users);

    // More than one user, so decrease one User.
    android_atomic_dec(&m_Users);

    if (m_Users == 0) // There is no more User after decrease one User
    {
        bRet = postCommand(ECmd_Uninit);
        if (!bRet) AEE_ASSERT_3A_HAL("ECmd_Uninit fail.");

        m_pThreadRaw->destroyInstance();

        MRESULT err = S_3A_OK;

        // AE uninit
        err = IAeMgr::getInstance().uninit(m_i4SensorDev);
        if (FAILED(err)){
            MY_ERR("IAeMgr::getInstance().uninit() fail\n");
            return err;
        }

        // AWB uninit
        err = IAwbMgr::getInstance().uninit(m_i4SensorDev);
        if (FAILED(err)){
            MY_ERR("IAwbMgr::getInstance().uninit() fail\n");
            return E_3A_ERR;
        }

#if CAM3_AF_FEATURE_EN
        // AF uninit
        err = IAfMgr::getInstance().uninit(m_i4SensorDev);
        if (FAILED(err)) {
            MY_ERR("IAfMgr::getInstance().uninit() fail\n");
            return err;
        }
#endif

#if CAM3_FLASH_FEATURE_EN
        // FLASH uninit
        err = FlashMgr::getInstance().uninit(m_i4SensorDev);
        if (FAILED(err)) {
            MY_ERR("FlashMgr::getInstance().uninit() fail\n");
            return err;
        }
#endif

        // TuningMgr uninit
        if (m_pTuning)
        {
            m_pTuning->uninit(m_i4SensorDev);
            m_pTuning = NULL;
        }

        if (m_pCbSet)
        {
            m_pCbSet = NULL;
            MY_ERR("User did not detach callbacks!");
        }

        delete m_pStateMgr;
        m_pStateMgr = NULL;
        delete m_pAfStateMgr;
        m_pAfStateMgr = NULL;

        MY_LOG("[%s] done\n", __FUNCTION__);

    }
    else    // There are still some users.
    {
        MY_LOG("[%s] Still %d users \n", __FUNCTION__, m_Users);
    }

    return S_3A_OK;
}

MINT32
Hal3ARawSMVRImp::
config()
{
    MY_LOG("[%s]+ sensorDev(%d), Mode(%d)", __FUNCTION__, m_i4SensorDev, m_u4SensorMode);

    // NormalIOPipe create instance
    if (m_pCamIO == NULL)
    {
        m_pCamIO = NSCam::NSIoPipe::NSCamIOPipe::INormalPipe::
                        createInstance(m_i4SensorIdx, LOG_TAG);
        if (m_pCamIO == NULL)
        {
            MY_ERR("Fail to create NormalPipe");
            return MFALSE;
        }
    }

    MUINT32 u4AAWidth, u4AAHight;
    MUINT32 u4AFWidth, u4AFHeight;
    MRESULT err = S_3A_OK;

    // query input size info for AAO and FLKO
    m_pCamIO->sendCommand(NSCam::NSIoPipe::NSCamIOPipe::ENPipeCmd_GET_HBIN_INFO,
                        (MINTPTR)&u4AAWidth, (MINTPTR)&u4AAHight, 0);
    MY_LOG("[%s] u4AAWidth(%d), u4AAHight(%d)", __FUNCTION__, u4AAWidth, u4AAHight);
    m_i4HbinWidth = u4AAWidth;
    m_i4HbinHeight= u4AAHight;

    // query input size info for AFO
    m_pCamIO->sendCommand(NSCam::NSIoPipe::NSCamIOPipe::ENPipeCmd_GET_BIN_INFO,
                        (MINTPTR)&u4AFWidth, (MINTPTR)&u4AFHeight, 0);
    MY_LOG("[%s] u4AFWidth(%d), u4AFHeight(%d)", __FUNCTION__, u4AFWidth, u4AFHeight);

    // update HBIN and BIN info to AE/AWB/AF/FLICKER

    // set sensor mode to 3A modules
    IAeMgr::getInstance().setSensorMode(m_i4SensorDev, m_u4SensorMode, u4AAWidth, u4AAHight);
    IAwbMgr::getInstance().setSensorMode(m_i4SensorDev, m_u4SensorMode,  u4AAWidth, u4AAHight);
    IAEBufMgr::getInstance().setSensorMode(m_i4SensorDev, m_u4SensorMode);
    NSIspTuning::ILscMgr::getInstance(static_cast<ESensorDev_T>(m_i4SensorDev))->setSensorMode(
        static_cast<ESensorMode_T>(m_u4SensorMode), u4AFWidth, u4AFHeight, MFALSE);

#if CAM3_AF_FEATURE_EN
    IAfMgr::getInstance().setSensorMode(m_i4SensorDev, m_u4SensorMode, u4AFWidth, u4AFHeight);
#endif

#if CAM3_FLICKER_FEATURE_EN
    FlickerHalBase::getInstance().setSensorMode(m_i4SensorDev, m_u4SensorMode, u4AAWidth, u4AAHight);
#endif

    m_pTuning->setSensorMode(m_i4SensorDev, m_u4SensorMode);
    m_pTuning->setIspProfile(m_i4SensorDev, NSIspTuning::EIspProfile_Preview);

    // FIXME (remove): update TG Info to 3A modules
    updateTGInfo();

    // AE start
    err = IAeMgr::getInstance().Start(m_i4SensorDev);
    if (FAILED(err)) {
    MY_ERR("IAeMgr::getInstance().Start() fail\n");
        return err;
    }

    // AWB start
    err = IAwbMgr::getInstance().start(m_i4SensorDev);
    if (!err) {
        MY_ERR("IAwbMgr::getInstance().start() fail\n");
        return E_3A_ERR;
    }

#if CAM3_FLASH_FEATURE_EN
    // Flash cameraPreviewStart + start
    err = FlashMgr::getInstance().cameraPreviewStart(m_i4SensorDev);
    if (!err) {
        MY_ERR("FlashMgr::getInstance().cameraPreviewStart() fail\n");
        return E_3A_ERR;
    }
    err = FlashMgr::getInstance().start(m_i4SensorDev);
    if (!err) {
        MY_ERR("FlashMgr::getInstance().start() fail\n");
        return E_3A_ERR;
    }
#endif

#if CAM3_AF_FEATURE_EN
    // AF start
    err = IAfMgr::getInstance().Start(m_i4SensorDev);
    if (FAILED(err)) {
        MY_ERR("AfMgr::getInstance().Start() fail\n");
        return err;
    }
#endif

#if CAM3_FLICKER_FEATURE_EN
    // Flicker start
    // AAASensorMgr::getInstance().setFlickerFrameRateActive(m_i4SensorDev, 1);
    FlickerHalBase::getInstance().open(m_i4SensorDev, m_i4SensorIdx, m_u4TgInfo);
#endif

    MY_LOG("[%s] m_i4SubsampleCount(%d)", __FUNCTION__, m_i4SubsampleCount);

    // apply 3A module's config
    MINT32 i4Num;
    RequestSet_T rRequestSet;
    rRequestSet.vNumberSet.clear();
    for (i4Num = 1; i4Num <= m_i4SubsampleCount; i4Num++)
    {
        rRequestSet.vNumberSet.push_back(i4Num);
    }
    IspTuningMgr::getInstance().validate(m_i4SensorDev, rRequestSet, MTRUE);

    MY_LOG("[%s]-", __FUNCTION__);

    return S_3A_OK;
}

MBOOL
Hal3ARawSMVRImp::
start()
{
    MY_LOG("[%s] +", __FUNCTION__);
    MRESULT isAFLampOn = MFALSE;
    m_i4InCaptureProcess = 0;
    m_u4MagicNumber = 0;

#if CAM3_AF_FEATURE_EN
    // enable AF thread
    m_pThreadRaw->enableAFThread(m_pAfStateMgr);
#endif

#if CAM3_FLASH_FEATURE_EN
    isAFLampOn = FlashMgr::getInstance().isAFLampOn(m_i4SensorDev);
#endif

    // setStrobeMode
    IAeMgr::getInstance().setStrobeMode(m_i4SensorDev, isAFLampOn ? MTRUE : MFALSE);
    IAwbMgr::getInstance().setStrobeMode(m_i4SensorDev, isAFLampOn ? AWB_STROBE_MODE_ON : AWB_STROBE_MODE_OFF);


    m_rGainDelay.u4SensorGain_Delay = 2; //Query from AE once
    m_rGainDelay.u4SutterGain_Delay = 1;
    m_rGainDelay.u4ISPGain_Delay = 2;

    MY_LOG("[%s] -", __FUNCTION__);
    return MTRUE;
}

MBOOL
Hal3ARawSMVRImp::
stop()
{
    MY_LOG("[%s] +", __FUNCTION__);
    MRESULT err = S_3A_OK;
    m_u4MagicNumber = 0;

    // AE stop
    err = IAeMgr::getInstance().Stop(m_i4SensorDev);
    if (FAILED(err)) {
    MY_ERR("IAeMgr::getInstance().Stop() fail\n");
        return err;
    }

    // AWB stop
    err = IAwbMgr::getInstance().stop(m_i4SensorDev);
    if (!err) {
        MY_ERR("IAwbMgr::getInstance().stop() fail\n");
        return E_3A_ERR;
    }

#if CAM3_FLASH_FEATURE_EN
    // Flash cameraPreviewEnd + stop
    err = FlashMgr::getInstance().cameraPreviewEnd(m_i4SensorDev);
    if (!err) {
        MY_ERR("FlashMgr::getInstance().cameraPreviewEnd() fail\n");
        return E_3A_ERR;
    }
    err = FlashMgr::getInstance().stop(m_i4SensorDev);
    if (!err) {
        MY_ERR("FlashMgr::getInstance().stop() fail\n");
        return E_3A_ERR;
    }
#endif

#if CAM3_AF_FEATURE_EN
    // AF stop
    err = IAfMgr::getInstance().Stop(m_i4SensorDev);
    if (FAILED(err)) {
        MY_ERR("AfMgr::getInstance().Stop() fail\n");
        return err;
    }
    // disable AF thread
    m_pThreadRaw->disableAFThread();
#endif

#if CAM3_FLICKER_FEATURE_EN
    // Flicker close
    FlickerHalBase::getInstance().close(m_i4SensorDev);
#endif

    // NormalIOPipe destroy instance
    if (m_pCamIO != NULL)
    {
        m_pCamIO->destroyInstance(LOG_TAG);
        m_pCamIO = NULL;
    }

    MY_LOG("[%s] -", __FUNCTION__);
    return MTRUE;
}

MBOOL
Hal3ARawSMVRImp::
generateP2(MINT32 flowType, const NSIspTuning::ISP_INFO_T& rIspInfo, void* pTuningBuf, ResultP2_T* pResultP2)
{
    Mutex::Autolock lock(mP2Mtx);

    void* pRegBuf = ((TuningParam*)pTuningBuf)->pRegBuf;
    MY_LOG("[%s] + flow(%d), buf(%p)", __FUNCTION__, flowType, pRegBuf);
    IspTuningMgr::getInstance().validatePerFrameP2(m_i4SensorDev, flowType, rIspInfo, pRegBuf);
#if CAM3_LSC_FEATURE_EN
    ILscBuf* pLscBuf = NSIspTuning::ILscMgr::getInstance(static_cast<ESensorDev_T>(m_i4SensorDev))->getP2Buf();
    if (pLscBuf)
        ((TuningParam*)pTuningBuf)->pLsc2Buf = pLscBuf->getBuf();
    else
        ((TuningParam*)pTuningBuf)->pLsc2Buf = NULL;
#endif
    // debug info
    if (pResultP2)
    {
        MY_LOG("[%s] get debug info p2 #(%d)", __FUNCTION__, rIspInfo.rCamInfo.u4Id);
        if (0 == pResultP2->vecDbgIspP2.size())
        {
            MY_LOG("[%s] Need to allocate P2 result", __FUNCTION__);
            pResultP2->vecDbgIspP2.resize(sizeof(NSIspExifDebug::IspExifDebugInfo_T));
        }
        NSIspExifDebug::IspExifDebugInfo_T& rIspExifDebugInfo = *reinterpret_cast<NSIspExifDebug::IspExifDebugInfo_T*>(pResultP2->vecDbgIspP2.editArray());
        IspTuningMgr::getInstance().getDebugInfo(m_i4SensorDev, rIspInfo, rIspExifDebugInfo, MFALSE);
    }

    MY_LOG("[%s] -", __FUNCTION__);
    return MTRUE;
}

MBOOL
Hal3ARawSMVRImp::
validateP1(const ParamIspProfile_T& rParamIspProfile, MBOOL fgPerframe)
{
    MY_LOG("[%s]", __FUNCTION__);
    m_pTuning->setIspProfile(m_i4SensorDev, rParamIspProfile.eIspProfile);
    m_pTuning->notifyRPGEnable(m_i4SensorDev, rParamIspProfile.iEnableRPG);
    //isp tuning query here
    m_pTuning->validatePerFrame(m_i4SensorDev, rParamIspProfile.rRequestSet, fgPerframe, (MINT32)m_rGainDelay.u4ISPGain_Delay);
    return MTRUE;
}

MBOOL
Hal3ARawSMVRImp::
setParams(Param_T const &rNewParam)
{
    MY_LOG_IF(m_3ALogEnable, "[%s] +", __FUNCTION__);
    Mutex::Autolock autoLock(m3AOperMtx2);

    if (!m_bEnable3ASetParams) return MTRUE;

#if CAM3_LSC_FEATURE_EN
    // ====================================== Shading =============================================
    if (m_fgEnableShadingMeta)
        NSIspTuning::ILscMgr::getInstance(static_cast<ESensorDev_T>(m_i4SensorDev))->setOnOff(rNewParam.u1ShadingMode ? MTRUE : MFALSE);
#endif

    // ====================================== ISP tuning =============================================
    IspTuningMgr::getInstance().setSceneMode(m_i4SensorDev, rNewParam.u4SceneMode);
    IspTuningMgr::getInstance().setEffect(m_i4SensorDev, rNewParam.u4EffectMode);
    IspTuningMgr::getInstance().setIspUserIdx_Bright(m_i4SensorDev, rNewParam.i4BrightnessMode);
    IspTuningMgr::getInstance().setIspUserIdx_Hue(m_i4SensorDev, rNewParam.i4HueMode);
    IspTuningMgr::getInstance().setIspUserIdx_Sat(m_i4SensorDev, rNewParam.i4SaturationMode);
    IspTuningMgr::getInstance().setIspUserIdx_Edge(m_i4SensorDev, rNewParam.i4EdgeMode);
    IspTuningMgr::getInstance().setIspUserIdx_Contrast(m_i4SensorDev, rNewParam.i4ContrastMode);
    IspTuningMgr::getInstance().setEdgeMode(m_i4SensorDev, rNewParam.u1EdgeMode ? MTK_EDGE_MODE_FAST : MTK_EDGE_MODE_OFF);
    IspTuningMgr::getInstance().setNoiseReductionMode(m_i4SensorDev, rNewParam.u1NRMode);
    IspTuningMgr::getInstance().setToneMapMode(m_i4SensorDev, rNewParam.u1TonemapMode);
    if (rNewParam.u1TonemapMode == MTK_TONEMAP_MODE_CONTRAST_CURVE)
    {
        MINT32 i = 0;
        android::Vector<MFLOAT> vecIn, vecOut;
        MINT32 i4Cnt = rNewParam.vecTonemapCurveBlue.size() / 2;
        vecIn.resize(i4Cnt);
        vecOut.resize(i4Cnt);
        MFLOAT* pArrayIn = vecIn.editArray();
        MFLOAT* pArrayOut = vecOut.editArray();
        const MFLOAT* pCurve = rNewParam.vecTonemapCurveBlue.array();
        for (i = i4Cnt; i != 0; i--)
        {
            MFLOAT x, y;
            x = *pCurve++;
            y = *pCurve++;
            *pArrayIn++ = x;
            *pArrayOut++ = y;
            MY_LOG_IF(m_3ALogEnable, "[Blue]#%d(%f,%f)", rNewParam.u4MagicNum, x, y);
        }
        IspTuningMgr::getInstance().setTonemapCurve_Blue(m_i4SensorDev, vecIn.editArray(), vecOut.editArray(), &i4Cnt);

        i4Cnt = rNewParam.vecTonemapCurveGreen.size() / 2;
        vecIn.resize(i4Cnt);
        vecOut.resize(i4Cnt);
        pArrayIn = vecIn.editArray();
        pArrayOut = vecOut.editArray();
        pCurve = rNewParam.vecTonemapCurveGreen.array();
        for (i = i4Cnt; i != 0; i--)
        {
            MFLOAT x, y;
            x = *pCurve++;
            y = *pCurve++;
            *pArrayIn++ = x;
            *pArrayOut++ = y;
            MY_LOG_IF(m_3ALogEnable, "[Green]#%d(%f,%f)", rNewParam.u4MagicNum, x, y);
        }
        IspTuningMgr::getInstance().setTonemapCurve_Green(m_i4SensorDev, vecIn.editArray(), vecOut.editArray(), &i4Cnt);

        i4Cnt = rNewParam.vecTonemapCurveRed.size() / 2;
        vecIn.resize(i4Cnt);
        vecOut.resize(i4Cnt);
        pArrayIn = vecIn.editArray();
        pArrayOut = vecOut.editArray();
        pCurve = rNewParam.vecTonemapCurveRed.array();
        for (i = i4Cnt; i != 0; i--)
        {
            MFLOAT x, y;
            x = *pCurve++;
            y = *pCurve++;
            *pArrayIn++ = x;
            *pArrayOut++ = y;
            MY_LOG_IF(m_3ALogEnable, "[Red]#%d(%f,%f)", rNewParam.u4MagicNum, x, y);
        }
        IspTuningMgr::getInstance().setTonemapCurve_Red(m_i4SensorDev, vecIn.editArray(), vecOut.editArray(), &i4Cnt);
    }

    // ====================================== AE ==============================================
    IAeMgr::getInstance().setAEMinMaxFrameRate(m_i4SensorDev, rNewParam.i4MinFps, rNewParam.i4MaxFps);
    IAeMgr::getInstance().setAEMeteringMode(m_i4SensorDev, rNewParam.u4AeMeterMode);
    IAeMgr::getInstance().setAERotateDegree(m_i4SensorDev, rNewParam.i4RotateDegree);
    IAeMgr::getInstance().setAEISOSpeed(m_i4SensorDev, rNewParam.u4IsoSpeedMode);
    IAeMgr::getInstance().setAEMeteringArea(m_i4SensorDev, &rNewParam.rMeteringAreas);
    IAeMgr::getInstance().setAPAELock(m_i4SensorDev, rNewParam.bIsAELock);
    IAeMgr::getInstance().setAEEVCompIndex(m_i4SensorDev, rNewParam.i4ExpIndex, rNewParam.fExpCompStep);
    IAeMgr::getInstance().setAEMode(m_i4SensorDev, rNewParam.u4AeMode);
    IAeMgr::getInstance().setAEFlickerMode(m_i4SensorDev, rNewParam.u4AntiBandingMode);
    IAeMgr::getInstance().setAECamMode(m_i4SensorDev, rNewParam.u4CamMode);
    IAeMgr::getInstance().setAEShotMode(m_i4SensorDev, rNewParam.u4ShotMode);
    IAeMgr::getInstance().setSceneMode(m_i4SensorDev, rNewParam.u4SceneMode);
    IAeMgr::getInstance().bBlackLevelLock(m_i4SensorDev, rNewParam.u1BlackLvlLock);
    IAeMgr::getInstance().setZoomWinInfo(m_i4SensorDev, rNewParam.rScaleCropRect.i4Xoffset,rNewParam.rScaleCropRect.i4Yoffset,rNewParam.rScaleCropRect.i4Xwidth,rNewParam.rScaleCropRect.i4Yheight);
    if (rNewParam.u4AeMode == MTK_CONTROL_AE_MODE_OFF)
    {
        AE_SENSOR_PARAM_T strSensorParams;
        strSensorParams.u4Sensitivity   = rNewParam.i4Sensitivity;
        strSensorParams.u8ExposureTime  = rNewParam.i8ExposureTime;
        strSensorParams.u8FrameDuration = rNewParam.i8FrameDuration;
        IAeMgr::getInstance().UpdateSensorParams(m_i4SensorDev, strSensorParams);
    }
    MY_LOG_IF(m_3ALogEnable, "[%s] setAEMode(%d)", __FUNCTION__, rNewParam.u4AeMode);

    // ====================================== AWB ==============================================
    IAwbMgr::getInstance().setAWBLock(m_i4SensorDev, rNewParam.bIsAWBLock);
    IAwbMgr::getInstance().setAWBMode(m_i4SensorDev, rNewParam.u4AwbMode);
    IAwbMgr::getInstance().setColorCorrectionMode(m_i4SensorDev, rNewParam.u1ColorCorrectMode);
    IspTuningMgr::getInstance().setColorCorrectionMode(m_i4SensorDev, rNewParam.u1ColorCorrectMode);
    if (rNewParam.u4AwbMode == MTK_CONTROL_AWB_MODE_OFF &&
        rNewParam.u1ColorCorrectMode == MTK_COLOR_CORRECTION_MODE_TRANSFORM_MATRIX)
    {
        IAwbMgr::getInstance().setColorCorrectionGain(m_i4SensorDev, rNewParam.fColorCorrectGain[0], rNewParam.fColorCorrectGain[1], rNewParam.fColorCorrectGain[3]);
        IspTuningMgr::getInstance().setColorCorrectionTransform(m_i4SensorDev,
            rNewParam.fColorCorrectMat[0], rNewParam.fColorCorrectMat[1], rNewParam.fColorCorrectMat[2],
            rNewParam.fColorCorrectMat[3], rNewParam.fColorCorrectMat[4], rNewParam.fColorCorrectMat[5],
            rNewParam.fColorCorrectMat[6], rNewParam.fColorCorrectMat[7], rNewParam.fColorCorrectMat[8]);
    }

#if CAM3_FLASH_FEATURE_EN
    // ====================================== Flash ==============================================
    FlashMgr::getInstance().setAeFlashMode(m_i4SensorDev, rNewParam.u4AeMode, rNewParam.u4StrobeMode);
    int bMulti;
    if(rNewParam.u4CapType == ECapType_MultiCapture)
        bMulti=1;
    else
        bMulti=0;
    FlashMgr::getInstance().setCamMode(m_i4SensorDev, rNewParam.u4CamMode);
    FlashMgr::getInstance().setEvComp(m_i4SensorDev, rNewParam.i4ExpIndex, rNewParam.fExpCompStep);
#endif

#if CAM3_FLICKER_FEATURE_EN
    // ====================================== Flicker ==============================================
    FlickerHalBase::getInstance().setFlickerMode(m_i4SensorDev, rNewParam.u4AntiBandingMode);
#endif

    // ====================================== FlowCtrl ==============================================
    m_rParam = rNewParam;

    MY_LOG_IF(m_3ALogEnable, "[%s] m_rParam.u1ShadingMapMode(%d)", __FUNCTION__, m_rParam.u1ShadingMapMode);

    MY_LOG_IF(m_3ALogEnable, "[%s] -", __FUNCTION__);
    return MTRUE;
}

MBOOL
Hal3ARawSMVRImp::
setAfParams(AF_Param_T const &rNewParam)
{
        if (!m_bEnable3ASetParams) return MTRUE;
#if CAM3_AF_FEATURE_EN
        // ====================================== AF ==============================================
        IThreadRaw::AFParam_T rAFPAram;
        rAFPAram.u4MagicNum = rNewParam.u4MagicNum;
        rAFPAram.u4AfMode = rNewParam.u4AfMode;
        rAFPAram.rFocusAreas = rNewParam.rFocusAreas;
        rAFPAram.bIsSupportAndroidService = rNewParam.bIsSupportAndroidService;
        if( IspTuningMgr::getInstance().getOperMode(m_i4SensorDev) != NSIspTuning::EOperMode_EM)
            rAFPAram.fFocusDistance = rNewParam.fFocusDistance;
        else
            rAFPAram.fFocusDistance = -1;
        rAFPAram.bFaceDetectEnable = m_bFaceDetectEnable;
        rAFPAram.rScaleCropArea = rNewParam.rScaleCropArea;
        m_pThreadRaw->sendRequest(ECmd_AFUpdate,(MINTPTR)&rAFPAram);
        m_rAfParam = rNewParam;
#endif
    return MTRUE;
}


MBOOL
Hal3ARawSMVRImp::
autoFocus()
{
#if CAM3_AF_FEATURE_EN
    MY_LOG("[%s()] + \n", __FUNCTION__);
    m_pThreadRaw->sendRequest(ECmd_AFStart);

    if ((m_rAfParam.u4AfMode != MTK_CONTROL_AF_MODE_CONTINUOUS_PICTURE) && (m_rAfParam.u4AfMode != MTK_CONTROL_AF_MODE_CONTINUOUS_VIDEO))   {
        m_pStateMgr->sendCmd(ECmd_AFStart);
    }
    MY_LOG("[%s()] - \n", __FUNCTION__);
#endif
    return MTRUE;
}

MBOOL
Hal3ARawSMVRImp::
cancelAutoFocus()
{
#if CAM3_AF_FEATURE_EN
    MY_LOG("[%s()] + \n", __FUNCTION__);
    m_pThreadRaw->sendRequest(ECmd_AFEnd);

    if ((m_rAfParam.u4AfMode != MTK_CONTROL_AF_MODE_CONTINUOUS_PICTURE) && (m_rAfParam.u4AfMode != MTK_CONTROL_AF_MODE_CONTINUOUS_VIDEO))   {
        m_pStateMgr->sendCmd(ECmd_AFEnd);
    }
    MY_LOG("[%s()] - \n", __FUNCTION__);
#endif
    return MTRUE;
}

MVOID
Hal3ARawSMVRImp::
setFDEnable(MBOOL fgEnable)
{
    MY_LOG_IF(m_3ALogEnable, "[%s] fgEnable(%d)", __FUNCTION__, fgEnable);
    m_bFaceDetectEnable = fgEnable;
    IAeMgr::getInstance().setFDenable(m_i4SensorDev, fgEnable);
}

MBOOL
Hal3ARawSMVRImp::
setFDInfo(MVOID* prFaces, MVOID* prAFFaces)
{
    MY_LOG_IF(m_3ALogEnable, "[%s] m_bFaceDetectEnable(%d)", __FUNCTION__, m_bFaceDetectEnable);
    if (m_bFaceDetectEnable)
    {
#if CAM3_AF_FEATURE_EN
        IAfMgr::getInstance().setFDInfo(m_i4SensorDev, prAFFaces);
#endif
        IAeMgr::getInstance().setFDInfo(m_i4SensorDev, prFaces);
    }
    return MTRUE;
}

MBOOL
Hal3ARawSMVRImp::
setFlashLightOnOff(MBOOL bOnOff, MBOOL bMainPre)
{
#if CAM3_FLASH_FEATURE_EN
    MY_LOG("[%s] bOnOff(%d) + ", __FUNCTION__, bOnOff);
    if (!bOnOff)
    {
        if (m_i4InCaptureProcess)
        {
            //modified to update strobe state after capture for ae/flash manager
            FlashMgr::getInstance().cameraPreviewStart(m_i4SensorDev);
            IAeMgr::getInstance().setStrobeMode(m_i4SensorDev, MFALSE);
            m_i4InCaptureProcess = 0;
        }
        FlashMgr::getInstance().turnOffFlashDevice(m_i4SensorDev);
    }
    else //flash on
    {
        if (bMainPre) FlashMgr::getInstance().setCaptureFlashOnOff(m_i4SensorDev, 1);
        else FlashMgr::getInstance().setTorchOnOff(m_i4SensorDev, 1);
    }
    MY_LOG("[%s] - ", __FUNCTION__);
#endif
    return MTRUE;
}

MBOOL
Hal3ARawSMVRImp::
chkMainFlashOnCond() const
{
    MY_LOG("[%s]", __FUNCTION__);
    return
        (((m_rParam.u4AeMode == MTK_CONTROL_AE_MODE_OFF)||(m_rParam.u4AeMode == MTK_CONTROL_AE_MODE_ON)) && (m_rParam.u4StrobeMode == MTK_FLASH_MODE_SINGLE))
        ||
        ( ((m_rParam.u4AeMode == MTK_CONTROL_AE_MODE_ON_ALWAYS_FLASH)||((m_rParam.u4AeMode == MTK_CONTROL_AE_MODE_ON_AUTO_FLASH) && isStrobeBVTrigger()))
           && (m_rParam.u1CaptureIntent == MTK_CONTROL_CAPTURE_INTENT_STILL_CAPTURE) );
}

MBOOL
Hal3ARawSMVRImp::
chkPreFlashOnCond() const
{
    MY_LOG("[%s]", __FUNCTION__);
    return
        (((m_rParam.u4AeMode == MTK_CONTROL_AE_MODE_OFF)||(m_rParam.u4AeMode == MTK_CONTROL_AE_MODE_ON)) && (m_rParam.u4StrobeMode == MTK_FLASH_MODE_TORCH));
}

MBOOL
Hal3ARawSMVRImp::
isStrobeBVTrigger() const
{
    return IAeMgr::getInstance().IsStrobeBVTrigger(m_i4SensorDev);
}

MINT32
Hal3ARawSMVRImp::
getCurrResult(MUINT32 i4FrmId, Result_T& rResult, MINT32 i4SubsampleIndex) const
{
    if (m_rResultLastSof.i4FrmId == -1)
    {
        MY_LOG_IF(m_3ALogEnable, "[%s] + i4MagicNum(%d), i4SubsampleIndex", __FUNCTION__, i4FrmId);
        mtk_camera_metadata_enum_android_control_awb_state_t eAwbState;
        rResult.i4FrmId = i4FrmId;

        // clear memory
        rResult.vecShadingMap.clear();
        rResult.vecExifInfo.clear();
        rResult.vecTonemapCurveRed.clear();
        rResult.vecTonemapCurveGreen.clear();
        rResult.vecTonemapCurveBlue.clear();
        rResult.vecColorCorrectMat.clear();
        rResult.vecDbg3AInfo.clear();
        rResult.vecDbgShadTbl.clear();

        rResult.u1SceneMode = m_rParam.u4SceneMode;

        // AE
        MUINT8 u1AeState = IAeMgr::getInstance().getAEState(m_i4SensorDev);
        if (IAeMgr::getInstance().IsStrobeBVTrigger(m_i4SensorDev) && u1AeState == MTK_CONTROL_AE_STATE_CONVERGED)
            rResult.u1AeState = MTK_CONTROL_AE_STATE_FLASH_REQUIRED;
        else
            rResult.u1AeState = u1AeState;

        AE_SENSOR_PARAM_T rAESensorInfo;
        IAeMgr::getInstance().getSensorParams(m_i4SensorDev, rAESensorInfo);
        rResult.i8SensorExposureTime = rAESensorInfo.u8ExposureTime;
        rResult.i8SensorFrameDuration = rAESensorInfo.u8FrameDuration;
        rResult.i4SensorSensitivity = rAESensorInfo.u4Sensitivity;
        if (m_rParam.u1RollingShutterSkew)
            rResult.i8SensorRollingShutterSkew = IAeMgr::getInstance().getSensorRollingShutter(m_i4SensorDev);

        // AWB
        IAwbMgr::getInstance().getAWBState(m_i4SensorDev, eAwbState);
        rResult.u1AwbState= eAwbState;
        AWB_GAIN_T rAwbGain;
        IAwbMgr::getInstance().getAWBGain(m_i4SensorDev, rAwbGain, rResult.i4AwbGainScaleUint);
        rResult.i4AwbGain[0] = rAwbGain.i4R;
        rResult.i4AwbGain[1] = rAwbGain.i4G;
        rResult.i4AwbGain[2] = rAwbGain.i4B;
        IAwbMgr::getInstance().getColorCorrectionGain(m_i4SensorDev, rResult.fColorCorrectGain[0],rResult.fColorCorrectGain[1],rResult.fColorCorrectGain[3]);
        rResult.fColorCorrectGain[2] = rResult.fColorCorrectGain[1];
        if (m_rParam.u1ColorCorrectMode != MTK_COLOR_CORRECTION_MODE_TRANSFORM_MATRIX)
        {
            rResult.vecColorCorrectMat.resize(9);
            MFLOAT* pfColorCorrectMat = rResult.vecColorCorrectMat.editArray();
        IspTuningMgr::getInstance().getColorCorrectionTransform(m_i4SensorDev,
                pfColorCorrectMat[0], pfColorCorrectMat[1], pfColorCorrectMat[2],
                pfColorCorrectMat[3], pfColorCorrectMat[4], pfColorCorrectMat[5],
                pfColorCorrectMat[6], pfColorCorrectMat[7], pfColorCorrectMat[8]
        );
    }

#if CAM3_AF_FEATURE_EN
        // AF
        rResult.u1AfState = IAfMgr::getInstance().getAFState(m_i4SensorDev);
        IAfMgr::getInstance().getFocusAreaResult(m_i4SensorDev, rResult.vecFocusAreaPos, rResult.vecFocusAreaRes, rResult.i4FocusAreaSz[0], rResult.i4FocusAreaSz[1]);
        if (m_pStateMgr->getStateStatus().eCurrState == eState_AF)
            rResult.u1AfState = MTK_CONTROL_AF_STATE_ACTIVE_SCAN;
        rResult.u1LensState = IAfMgr::getInstance().getLensState(m_i4SensorDev);
        rResult.fLensFocusDistance = IAfMgr::getInstance().getFocusDistance(m_i4SensorDev);
        IAfMgr::getInstance().getFocusRange(m_i4SensorDev, &rResult.fLensFocusRange[0], &rResult.fLensFocusRange[1]);
#endif

#if CAM3_FLASH_FEATURE_EN
        // Flash
        //rResult.u1FlashState = FlashMgr::getInstance()->getFlashState();
#if 1 //mark out temporarily
        rResult.u1FlashState =
            (FlashMgr::getInstance().getFlashState(m_i4SensorDev) == MTK_FLASH_STATE_UNAVAILABLE) ?
            MTK_FLASH_STATE_UNAVAILABLE :
            (FlashMgr::getInstance().isAFLampOn(m_i4SensorDev) ? MTK_FLASH_STATE_FIRED : MTK_FLASH_STATE_READY);
#endif
#endif

#if CAM3_FLICKER_FEATURE_EN
        // Flicker
        MINT32 i4FlkStatus = IAeMgr::getInstance().getAEAutoFlickerState(m_i4SensorDev);
        //FlickerHalBase::getInstance().getFlickerResult(i4FlkStatus);
        MUINT8 u1ScnFlk = MTK_STATISTICS_SCENE_FLICKER_NONE;
        if (i4FlkStatus == 0) u1ScnFlk = MTK_STATISTICS_SCENE_FLICKER_50HZ;
        if (i4FlkStatus == 1) u1ScnFlk = MTK_STATISTICS_SCENE_FLICKER_60HZ;
        rResult.u1SceneFlk = (MUINT8)u1ScnFlk;
#endif

#if CAM3_LSC_FEATURE_EN
        NSIspTuning::ILscMgr* pLsc = NSIspTuning::ILscMgr::getInstance(static_cast<ESensorDev_T>(m_i4SensorDev));
        rResult.vecLscData.clear();
        if (pLsc->getOnOff() &&
            (m_rParam.u1CaptureIntent == MTK_CONTROL_CAPTURE_INTENT_STILL_CAPTURE ||
             m_rParam.u1CaptureIntent == MTK_CONTROL_CAPTURE_INTENT_VIDEO_SNAPSHOT ||
             m_rParam.u1CaptureIntent == MTK_CONTROL_CAPTURE_INTENT_ZERO_SHUTTER_LAG) &&
            m_rParam.i4RawType == NSIspTuning::ERawType_Pure /*IMGO pure raw*/)
        {
            MY_LOG("[%s] #(%d) LSC table to metadata", __FUNCTION__, i4FrmId);
            ILscTbl rLscTbl(ILscTbl::GAIN_FIXED);
            pLsc->getCurrTbl(rLscTbl);
            rResult.vecLscData.resize(rLscTbl.getSize()+sizeof(ILscTbl::Config));
            MUINT8* pDest = rResult.vecLscData.editArray();
            ::memcpy(pDest, &rLscTbl.getConfig(), sizeof(ILscTbl::Config));
            ::memcpy(pDest+sizeof(ILscTbl::Config), rLscTbl.getData(), rLscTbl.getSize());
        }
        // Shading map
        if (m_rParam.u1ShadingMapMode == MTK_STATISTICS_LENS_SHADING_MAP_MODE_ON)
        {
            rResult.vecShadingMap.resize(m_rParam.u1ShadingMapXGrid*m_rParam.u1ShadingMapYGrid*4);
            MFLOAT* pfShadingMap = rResult.vecShadingMap.editArray();
            NSIspTuning::ILscMgr::getInstance(static_cast<ESensorDev_T>(m_i4SensorDev))->getGainTable(3, m_rParam.u1ShadingMapXGrid, m_rParam.u1ShadingMapYGrid, pfShadingMap);
        }
#endif

        // Tonemap
        if (m_rParam.u1TonemapMode != MTK_TONEMAP_MODE_CONTRAST_CURVE)
        {
            MINT32 i = 0;
            MFLOAT *pIn, *pOut;
            MINT32 i4NumPt;
            IspTuningMgr::getInstance().getTonemapCurve_Blue(m_i4SensorDev, pIn, pOut, &i4NumPt);
            for (i = 0; i < i4NumPt; i++)
            {
                rResult.vecTonemapCurveBlue.push_back(*pIn++);
                rResult.vecTonemapCurveBlue.push_back(*pOut++);
            }
            IspTuningMgr::getInstance().getTonemapCurve_Green(m_i4SensorDev, pIn, pOut, &i4NumPt);
            for (i = 0; i < i4NumPt; i++)
            {
                rResult.vecTonemapCurveGreen.push_back(*pIn++);
                rResult.vecTonemapCurveGreen.push_back(*pOut++);
            }
            IspTuningMgr::getInstance().getTonemapCurve_Red(m_i4SensorDev, pIn, pOut, &i4NumPt);
            for (i = 0; i < i4NumPt; i++)
            {
                rResult.vecTonemapCurveRed.push_back(*pIn++);
                rResult.vecTonemapCurveRed.push_back(*pOut++);
            }
        }

        // Cam Info
        if (!IspTuningMgr::getInstance().getCamInfo(m_i4SensorDev, rResult.rCamInfo))
        {
            MY_ERR("Fail to get CamInfo");
        }

        // Exif
        if (m_rParam.u1IsGetExif)
        {
            rResult.vecExifInfo.resize(1);
            get3AEXIFInfo(rResult.vecExifInfo.editTop());
            // debug exif
            rResult.vecDbg3AInfo.resize(sizeof(AAA_DEBUG_INFO_T));
            rResult.vecDbgShadTbl.resize(sizeof(DEBUG_SHAD_ARRAY_INFO_T));
            rResult.vecDbgIspInfo.resize(sizeof(NSIspExifDebug::IspExifDebugInfo_T));
            AAA_DEBUG_INFO_T& rDbg3AInfo = *reinterpret_cast<AAA_DEBUG_INFO_T*>(rResult.vecDbg3AInfo.editArray());
            DEBUG_SHAD_ARRAY_INFO_T& rDbgShadTbl = *reinterpret_cast<DEBUG_SHAD_ARRAY_INFO_T*>(rResult.vecDbgShadTbl.editArray());
            NSIspExifDebug::IspExifDebugInfo_T& rDbgIspInfo = *reinterpret_cast<NSIspExifDebug::IspExifDebugInfo_T*>(rResult.vecDbgIspInfo.editArray());
            getP1DbgInfo(rDbg3AInfo, rDbgShadTbl, rDbgIspInfo);
        }

        MY_LOG_IF(m_3ALogEnable, "[%s] - i4MagicNum(%d)", __FUNCTION__, i4FrmId);
        return 0;
    }
    else
    {
        rResult = m_rResultLastSof;
        rResult.i4FrmId = i4FrmId;;
        //update neccesary result here
        return 0;
    }



}

MINT32
Hal3ARawSMVRImp::
getCurrentHwId() const
{
    MINT32 idx = 0;
    m_pCamIO->sendCommand(NSCam::NSIoPipe::NSCamIOPipe::ENPipeCmd_GET_CUR_SOF_IDX,
                        (MINTPTR)&idx, 0, 0);
    MY_LOG("[%s] idx(%d)", __FUNCTION__, idx);
    return idx;
}

MVOID
Hal3ARawSMVRImp::
setSensorMode(MINT32 i4SensorMode)
{
    MY_LOG("[%s] mode(%d)", __FUNCTION__, i4SensorMode);
    m_u4SensorMode = i4SensorMode;
}

MBOOL
Hal3ARawSMVRImp::
postCommand(ECmd_T const r3ACmd, const ParamIspProfile_T* pParam)
{
    MY_LOG("[%s]+", __FUNCTION__);
#if CAM3_AF_FEATURE_EN
    // for AF state
    switch(r3ACmd)
    {
        case ECmd_Init:
        case ECmd_Uninit:
        case ECmd_CameraPreviewStart:
        case ECmd_CameraPreviewEnd:
            m_pAfStateMgr->sendCmd(r3ACmd);
            break;
    }
#endif
    MY_LOG_IF(m_3ALogEnable,"AF command done.");

    // for 3A state

    // back next state when previewEnd or uninit command on AF state.
    if(r3ACmd == ECmd_CameraPreviewEnd || r3ACmd == ECmd_Uninit)
    {
        if(m_pStateMgr->getStateStatus().eCurrState == eState_AF)
        {
            m_pStateMgr->mAFStateCntSet.resetAll(); //reset all AFState cnt, flags
            if(m_pStateMgr->getStateStatus().eNextState!=eState_Invalid)
            {
                m_pStateMgr->transitState(eState_AF, m_pStateMgr->getStateStatus().eNextState);
                m_pStateMgr->setNextState(eState_Invalid);
            }
            else
                m_pStateMgr->transitState(eState_AF, m_pStateMgr->getStateStatus().ePrevState);
        }
    }
    if (r3ACmd == ECmd_Update)
    {
        if(m_pStateMgr != NULL)
        {
             m_pStateMgr->updateMagicNumberRequest(pParam->i4MagicNum);
             MY_LOG_IF(m_3ALogEnable,"[HQC] updateMagicNumberRequest(%d)", pParam->i4MagicNum);
             m_bIsHighQualityCaptureOn = MFALSE;
        }
    }
    // capture flow.
    if ((m_rParam.u1CaptureIntent == MTK_CONTROL_CAPTURE_INTENT_STILL_CAPTURE) && (r3ACmd == ECmd_Update) && (m_rParam.u4AeMode != MTK_CONTROL_AE_MODE_OFF))
    {
        if (m_pStateMgr->getStateStatus().eCurrState == eState_Precapture)
        {
            MY_LOG("Unexpected Operation since precapture is not finished.");
            m_pStateMgr->sendCmd(ECmd_PrecaptureEnd);
        }

#if CAM3_FLASH_FEATURE_EN
        if(!pParam->bMainFlash && !mbIsHDRShot)
#endif
        {
            AE_MODE_CFG_T previewInfo;
            IAeMgr::getInstance().getPreviewParams(m_i4SensorDev, previewInfo);
            IAeMgr::getInstance().updateCaptureParams(m_i4SensorDev, previewInfo);
        }
        if (mbIsHDRShot)
        {
            mbIsHDRShot = MFALSE;
        }

        {
            m_i4InCaptureProcess = 1;
            m_pStateMgr->sendCmd(ECmd_CaptureStart);
        }

        if(pParam->bMainFlash)
        {
            MINT32 i4MagicNumbeRequest;
            i4MagicNumbeRequest = m_pStateMgr->queryMagicNumberRequest();
            m_pStateMgr->updateCapQueue(i4MagicNumbeRequest);
            m_bIsHighQualityCaptureOn = MTRUE;
            MY_LOG_IF(m_3ALogEnable,"[HQC] i4MagicNumbeRequest = %d", i4MagicNumbeRequest);
        }
    }
    else m_pStateMgr->sendCmd(r3ACmd);
    MY_LOG_IF(m_3ALogEnable,"3A command done.");

    // query magic number after deque sw buffer.
    m_u4MagicNumber = m_pStateMgr->queryMagicNumber();
    MY_LOG_IF(m_3ALogEnable,"[%s] m_u4MagicNumber = %d",__FUNCTION__, m_u4MagicNumber);

    if (ECmd_Update == r3ACmd)
    {
        if (m_pStateMgr->getFrameCount() >= 0)
        {
#if CAM3_FLASH_FEATURE_EN
            FlashMgr::getInstance().doPreviewOneFrame(m_i4SensorDev);
#endif
            validateP1(*pParam, MTRUE);

        }
    }
    MY_LOG("[%s]-", __FUNCTION__);
    return MTRUE;
}

MRESULT
Hal3ARawSMVRImp::
updateTGInfo()
{
    //Before wait for VSirq of IspDrv, we need to query IHalsensor for the current TG info
    IHalSensorList*const pHalSensorList = IHalSensorList::get();
    if (!pHalSensorList)
    {
        MY_ERR("IHalSensorList::get() == NULL");
        return E_3A_ERR;
    }
    const char* const callerName = "Hal3AQueryTG";
    IHalSensor* pHalSensor = pHalSensorList->createSensor(callerName, m_i4SensorIdx);
    //Note that Middleware has configured sensor before
    SensorDynamicInfo senInfo;
    MINT32 i4SensorDevId = pHalSensor->querySensorDynamicInfo(m_i4SensorDev, &senInfo);
    pHalSensor->destroyInstance(callerName);

    MY_LOG("m_i4SensorDev = %d, senInfo.TgInfo = %d\n", m_i4SensorDev, senInfo.TgInfo);

    if ((senInfo.TgInfo != CAM_TG_1) && (senInfo.TgInfo != CAM_TG_2))
    {
        MY_ERR("RAW sensor is connected with TgInfo: %d\n", senInfo.TgInfo);
        return E_3A_ERR;
    }

    m_u4TgInfo = senInfo.TgInfo; //now, TG info is obtained! TG1 or TG2

    IAwbMgr::getInstance().setTGInfo(m_i4SensorDev, m_u4TgInfo);
    IAEBufMgr::getInstance().setTGInfo(m_i4SensorDev, m_u4TgInfo);
    IspTuningMgr::getInstance().setTGInfo(m_i4SensorDev, m_u4TgInfo);

    m_pCamIO->sendCommand( NSCam::NSIoPipe::NSCamIOPipe::ENPipeCmd_GET_TG_OUT_SIZE, (MINTPTR)&m_i4TgWidth, (MINTPTR)&m_i4TgHeight, 0);
    MY_LOG("[%s] TG size(%d,%d)", __FUNCTION__, m_i4TgWidth, m_i4TgHeight);

    return S_3A_OK;
}

MBOOL
Hal3ARawSMVRImp::
get3AEXIFInfo(EXIF_3A_INFO_T& rExifInfo) const
{
    AE_DEVICES_INFO_T rDeviceInfo;
    AE_SENSOR_PARAM_T rAESensorInfo;

    IAeMgr::getInstance().getSensorParams(m_i4SensorDev, rAESensorInfo);
    rExifInfo.u4CapExposureTime = rAESensorInfo.u8ExposureTime / 1000;  // naro sec change to micro sec
#if CAM3_FLASH_FEATURE_EN
    if (FlashMgr::getInstance().isAFLampOn(m_i4SensorDev))
        rExifInfo.u4FlashLightTimeus = 30000;
    else
#endif
        rExifInfo.u4FlashLightTimeus = 0;

    IAeMgr::getInstance().getSensorDeviceInfo(m_i4SensorDev, rDeviceInfo);
    rExifInfo.u4FNumber     = rDeviceInfo.u4LensFno; // Format: F2.8 = 28
    rExifInfo.u4FocalLength = rDeviceInfo.u4FocusLength_100x; // Format: FL 3.5 = 350
    //rExifInfo.u4SceneMode   = m_rParam.u4SceneMode; // Scene mode   (SCENE_MODE_XXX)
    switch (IAeMgr::getInstance().getAEMeterMode(m_i4SensorDev))
    {
    case LIB3A_AE_METERING_MODE_AVERAGE:
        rExifInfo.u4AEMeterMode = eMeteringMode_Average;
        break;
    case LIB3A_AE_METERING_MODE_CENTER_WEIGHT:
        rExifInfo.u4AEMeterMode = eMeteringMode_Center;
        break;
    case LIB3A_AE_METERING_MODE_SOPT:
        rExifInfo.u4AEMeterMode = eMeteringMode_Spot;
        break;
    default:
        rExifInfo.u4AEMeterMode = eMeteringMode_Other;
        break;
    }
    rExifInfo.i4AEExpBias   = IAeMgr::getInstance().getEVCompensateIndex(m_i4SensorDev); // Exposure index  (AE_EV_COMP_XX)
    MINT32 i4AEISOSpeedMode = IAeMgr::getInstance().getAEISOSpeedMode(m_i4SensorDev);
    rExifInfo.u4AEISOSpeed  = (i4AEISOSpeedMode ==LIB3A_AE_ISO_SPEED_AUTO) ? rAESensorInfo.u4Sensitivity : i4AEISOSpeedMode;

    rExifInfo.u4AWBMode     = (m_rParam.u4AwbMode == MTK_CONTROL_AWB_MODE_AUTO) ? 0 : 1;
    switch (m_rParam.u4AwbMode)
    {
    case MTK_CONTROL_AWB_MODE_AUTO:
    case MTK_CONTROL_AWB_MODE_WARM_FLUORESCENT:
    case MTK_CONTROL_AWB_MODE_TWILIGHT:
    case MTK_CONTROL_AWB_MODE_INCANDESCENT:
        rExifInfo.u4LightSource = eLightSourceId_Other;
        break;
    case MTK_CONTROL_AWB_MODE_DAYLIGHT:
        rExifInfo.u4LightSource = eLightSourceId_Daylight;
        break;
    case MTK_CONTROL_AWB_MODE_FLUORESCENT:
        rExifInfo.u4LightSource = eLightSourceId_Fluorescent;
        break;
#if 0
    case MTK_CONTROL_AWB_MODE_TUNGSTEN:
        rExifInfo.u4LightSource = eLightSourceId_Tungsten;
        break;
#endif
    case MTK_CONTROL_AWB_MODE_CLOUDY_DAYLIGHT:
        rExifInfo.u4LightSource = eLightSourceId_Cloudy;
        break;
    case MTK_CONTROL_AWB_MODE_SHADE:
        rExifInfo.u4LightSource = eLightSourceId_Shade;
        break;
    default:
        rExifInfo.u4LightSource = eLightSourceId_Other;
        break;
    }

    switch (m_rParam.u4SceneMode)
    {
    case MTK_CONTROL_SCENE_MODE_PORTRAIT:
        rExifInfo.u4ExpProgram = eExpProgramId_Portrait;
        break;
    case MTK_CONTROL_SCENE_MODE_LANDSCAPE:
        rExifInfo.u4ExpProgram = eExpProgramId_Landscape;
        break;
    default:
        rExifInfo.u4ExpProgram = eExpProgramId_NotDefined;
        break;
    }

    switch (m_rParam.u4SceneMode)
    {
    case MTK_CONTROL_SCENE_MODE_DISABLED:
    case MTK_CONTROL_SCENE_MODE_NORMAL:
    case MTK_CONTROL_SCENE_MODE_NIGHT_PORTRAIT:
    case MTK_CONTROL_SCENE_MODE_THEATRE:
    case MTK_CONTROL_SCENE_MODE_BEACH:
    case MTK_CONTROL_SCENE_MODE_SNOW:
    case MTK_CONTROL_SCENE_MODE_SUNSET:
    case MTK_CONTROL_SCENE_MODE_STEADYPHOTO:
    case MTK_CONTROL_SCENE_MODE_FIREWORKS:
    case MTK_CONTROL_SCENE_MODE_SPORTS:
    case MTK_CONTROL_SCENE_MODE_PARTY:
    case MTK_CONTROL_SCENE_MODE_CANDLELIGHT:
        rExifInfo.u4SceneCapType = eCapTypeId_Standard;
        break;
    case MTK_CONTROL_SCENE_MODE_PORTRAIT:
        rExifInfo.u4SceneCapType = eCapTypeId_Portrait;
        break;
    case MTK_CONTROL_SCENE_MODE_LANDSCAPE:
        rExifInfo.u4SceneCapType = eCapTypeId_Landscape;
        break;
    case MTK_CONTROL_SCENE_MODE_NIGHT:
        rExifInfo.u4SceneCapType = eCapTypeId_Night;
        break;
    default:
        rExifInfo.u4SceneCapType = eCapTypeId_Standard;
        break;
    }

    return MTRUE;
}

MBOOL
Hal3ARawSMVRImp::
getASDInfo(ASDInfo_T &a_rASDInfo) const
{
    // AWB
    AWB_ASD_INFO_T rAWBASDInfo;
    IAwbMgr::getInstance().getASDInfo(m_i4SensorDev, rAWBASDInfo);
    a_rASDInfo.i4AWBRgain_X128 = rAWBASDInfo.i4AWBRgain_X128;
    a_rASDInfo.i4AWBBgain_X128 = rAWBASDInfo.i4AWBBgain_X128;
    a_rASDInfo.i4AWBRgain_D65_X128 = rAWBASDInfo.i4AWBRgain_D65_X128;
    a_rASDInfo.i4AWBBgain_D65_X128 = rAWBASDInfo.i4AWBBgain_D65_X128;
    a_rASDInfo.i4AWBRgain_CWF_X128 = rAWBASDInfo.i4AWBRgain_CWF_X128;
    a_rASDInfo.i4AWBBgain_CWF_X128 = rAWBASDInfo.i4AWBBgain_CWF_X128;
    a_rASDInfo.bAWBStable = rAWBASDInfo.bAWBStable;

#if CAM3_AF_FEATURE_EN
    a_rASDInfo.i4AFPos = IAfMgr::getInstance().getAFPos(m_i4SensorDev);
    a_rASDInfo.pAFTable = IAfMgr::getInstance().getAFTable(m_i4SensorDev);
    a_rASDInfo.i4AFTableOffset = IAfMgr::getInstance().getAFTableOffset(m_i4SensorDev);
    a_rASDInfo.i4AFTableMacroIdx = IAfMgr::getInstance().getAFTableMacroIdx(m_i4SensorDev);
    a_rASDInfo.i4AFTableIdxNum = IAfMgr::getInstance().getAFTableIdxNum(m_i4SensorDev);
    a_rASDInfo.bAFStable = IAfMgr::getInstance().getAFStable(m_i4SensorDev);
#endif

    a_rASDInfo.i4AELv_x10 = IAeMgr::getInstance().getLVvalue(m_i4SensorDev, MTRUE);
    a_rASDInfo.bAEBacklit = IAeMgr::getInstance().getAECondition(m_i4SensorDev, AE_CONDITION_BACKLIGHT);
    a_rASDInfo.bAEStable = IAeMgr::getInstance().IsAEStable(m_i4SensorDev);
    a_rASDInfo.i2AEFaceDiffIndex = IAeMgr::getInstance().getAEFaceDiffIndex(m_i4SensorDev);

    return MTRUE;
}

MBOOL
Hal3ARawSMVRImp::
getP1DbgInfo(AAA_DEBUG_INFO_T& rDbg3AInfo, DEBUG_SHAD_ARRAY_INFO_T& rDbgShadTbl, NSIspExifDebug::IspExifDebugInfo_T& rDbgIspInfo) const
{
    // 3A debug info
    rDbg3AInfo.hdr.u4KeyID = AAA_DEBUG_KEYID;
    rDbg3AInfo.hdr.u4ModuleCount = MODULE_NUM(9,6);

    rDbg3AInfo.hdr.u4AEDebugInfoOffset        = sizeof(rDbg3AInfo.hdr);
    rDbg3AInfo.hdr.u4AFDebugInfoOffset        = rDbg3AInfo.hdr.u4AEDebugInfoOffset + sizeof(AE_DEBUG_INFO_T);
    rDbg3AInfo.hdr.u4AWBDebugInfoOffset       = rDbg3AInfo.hdr.u4AFDebugInfoOffset + sizeof(AF_DEBUG_INFO_T);
    rDbg3AInfo.hdr.u4FlashDebugInfoOffset     = rDbg3AInfo.hdr.u4AWBDebugInfoOffset + sizeof(AWB_DEBUG_INFO_T);
    rDbg3AInfo.hdr.u4FlickerDebugInfoOffset   = rDbg3AInfo.hdr.u4FlashDebugInfoOffset + sizeof(FLASH_DEBUG_INFO_T);
    rDbg3AInfo.hdr.u4ShadingDebugInfoOffset   = rDbg3AInfo.hdr.u4FlickerDebugInfoOffset + sizeof(FLICKER_DEBUG_INFO_T);
    rDbg3AInfo.hdr.u4AWBDebugDataOffset       = rDbg3AInfo.hdr.u4ShadingDebugInfoOffset + sizeof(SHADING_DEBUG_INFO_T);
    rDbg3AInfo.hdr.u4AEPlineDataOffset        = rDbg3AInfo.hdr.u4AWBDebugDataOffset + sizeof(AWB_DEBUG_DATA_T);
    rDbg3AInfo.hdr.u4ShadingTableOffset       = rDbg3AInfo.hdr.u4AEPlineDataOffset + sizeof(AE_PLINE_DEBUG_INFO_T);


    AE_PLINE_DEBUG_INFO_T tmpBuff;
    // AE
    IAeMgr::getInstance().getDebugInfo(m_i4SensorDev, rDbg3AInfo.rAEDebugInfo, tmpBuff);

#if CAM3_AF_FEATURE_EN
    // AF
    IAfMgr::getInstance().getDebugInfo(m_i4SensorDev, rDbg3AInfo.rAFDebugInfo);
#endif

    // AWB
    IAwbMgr::getInstance().getDebugInfo(m_i4SensorDev, rDbg3AInfo.rAWBDebugInfo, rDbg3AInfo.rAWBDebugData);

#if CAM3_FLASH_FEATURE_EN
    // Flash
    FlashMgr::getInstance().getDebugInfo(m_i4SensorDev, &rDbg3AInfo.rFlashDebugInfo);
#endif

    // Flicker
    //Flicker::getInstance()->getDebugInfo(&r3ADebugInfo.rFlickerDebugInfo);

#if CAM3_LSC_FEATURE_EN
    // LSC
    ILscMgr::getInstance(static_cast<ESensorDev_T>(m_i4SensorDev))->getDebugInfo(rDbg3AInfo.rShadigDebugInfo);
    ILscMgr::getInstance(static_cast<ESensorDev_T>(m_i4SensorDev))->getDebugTbl(rDbgShadTbl, rDbg3AInfo.rShadingRestTbl);
#endif

    // P1 ISP
    IspTuningMgr::getInstance().getDebugInfoP1(m_i4SensorDev, rDbgIspInfo, MFALSE);

    return MTRUE;
}

MBOOL
Hal3ARawSMVRImp::
send3ACtrl(E3ACtrl_T e3ACtrl, MINTPTR iArg1, MINTPTR iArg2)
{
    MINT32 i4Ret = 0;
    switch (e3ACtrl)
    {
        // --------------------------------- AE ---------------------------------
        case E3ACtrl_SetHalHdr:
            if (iArg1)
            {
                AE_MODE_CFG_T rCaptureInfo;
                CaptureParam_T &a_rCaptureInfo = *reinterpret_cast<CaptureParam_T*>(iArg2);
                rCaptureInfo.u4ExposureMode = a_rCaptureInfo.u4ExposureMode;
                rCaptureInfo.u4Eposuretime  = a_rCaptureInfo.u4Eposuretime;
                rCaptureInfo.u4AfeGain      = a_rCaptureInfo.u4AfeGain;
                rCaptureInfo.u4IspGain      = a_rCaptureInfo.u4IspGain;
                rCaptureInfo.u4RealISO      = a_rCaptureInfo.u4RealISO;
                rCaptureInfo.i2FlareGain    = a_rCaptureInfo.u4FlareGain;
                rCaptureInfo.i2FlareOffset  = a_rCaptureInfo.u4FlareOffset;
                IAeMgr::getInstance().updateCaptureParams(m_i4SensorDev, rCaptureInfo);
                IAeMgr::getInstance().IsHDRShot(m_i4SensorDev, MTRUE);
                mbIsHDRShot = MTRUE;
            }
            else
            {
                IAeMgr::getInstance().IsHDRShot(m_i4SensorDev, MFALSE);
            }
            break;
        case E3ACtrl_GetCurrentEV:
            IAeMgr::getInstance().CCTOPAEGetCurrentEV(m_i4SensorDev, reinterpret_cast<MINT32 *>(iArg1), reinterpret_cast<MUINT32 *>(iArg2));
            break;
        case E3ACtrl_GetNVRAMParam:
            IAeMgr::getInstance().CCTOPAEGetNVRAMParam(m_i4SensorDev, reinterpret_cast<MVOID *>(iArg1), reinterpret_cast<MUINT32 *>(iArg2));
            break;
        case E3ACtrl_SetAEIsoSpeedMode:
            IAeMgr::getInstance().setAEISOSpeed(m_i4SensorDev, iArg1);
            break;
        case E3ACtrl_SetAETargetMode:
            IAeMgr::getInstance().SetAETargetMode(m_i4SensorDev, (eAETargetMODE)iArg1);
            break;
        case E3ACtrl_SetAELimiterMode:
            IAeMgr::getInstance().setAELimiterMode(m_i4SensorDev, iArg1);
            break;
        case E3ACtrl_SetAECamMode:
            IAeMgr::getInstance().setAECamMode(m_i4SensorDev, iArg1);
            break;
        case E3ACtrl_EnableDisableAE:
            if (iArg1) i4Ret = IAeMgr::getInstance().enableAE(m_i4SensorDev);
            else       i4Ret = IAeMgr::getInstance().disableAE(m_i4SensorDev);
            break;
        case E3ACtrl_GetExposureInfo:
            i4Ret = IAeMgr::getInstance().getExposureInfo(m_i4SensorDev, *reinterpret_cast<ExpSettingParam_T*>(iArg1));
            break;
        case E3ACtrl_GetAECapPLineTable:
            IAeMgr::getInstance().getAECapPlineTable(m_i4SensorDev, reinterpret_cast<MINT32*>(iArg1), *reinterpret_cast<strAETable*>(iArg2));
            break;
        case E3ACtrl_GetInitExposureTime:
            AE_MODE_CFG_T rPreviewInfo;
            i4Ret = IAeMgr::getInstance().getPreviewParams(m_i4SensorDev, rPreviewInfo);
            *(reinterpret_cast<MUINT32*>(iArg1)) = rPreviewInfo.u4Eposuretime;
            break;
        case E3ACtrl_GetExposureParam:
            CaptureParam_T* a_rCaptureInfo;
            a_rCaptureInfo = reinterpret_cast<CaptureParam_T*>(iArg1);

            AE_MODE_CFG_T rCaptureInfo;
            i4Ret = IAeMgr::getInstance().getCaptureParams(m_i4SensorDev, rCaptureInfo);

            a_rCaptureInfo->u4ExposureMode = rCaptureInfo.u4ExposureMode;
            a_rCaptureInfo->u4Eposuretime = rCaptureInfo.u4Eposuretime;
            a_rCaptureInfo->u4AfeGain = rCaptureInfo.u4AfeGain;
            a_rCaptureInfo->u4IspGain = rCaptureInfo.u4IspGain;
            a_rCaptureInfo->u4RealISO = rCaptureInfo.u4RealISO;
            a_rCaptureInfo->u4FlareGain = (MUINT32) rCaptureInfo.i2FlareGain;
            a_rCaptureInfo->u4FlareOffset = (MUINT32) rCaptureInfo.i2FlareOffset;
            a_rCaptureInfo->i4LightValue_x10 = IAeMgr::getInstance().getCaptureLVvalue(m_i4SensorDev);
            break;
        case E3ACtrl_GetIsAEStable:
            *(reinterpret_cast<MUINT32*>(iArg1)) = IAeMgr::getInstance().IsAEStable(m_i4SensorDev);
            break;
        // ----------------------------------ISP----------------------------------
        case E3ACtrl_GetIspGamma:
            IspTuningMgr::getInstance().sendIspTuningIOCtrl(m_i4SensorDev, IspTuningMgr::E_ISPTUNING_GET_ISP_GAMMA, iArg1, iArg2);
            break;

        // --------------------------------- AWB ---------------------------------
        case E3ACtrl_SetAwbBypCalibration:
            i4Ret = IAwbMgr::getInstance().CCTOPAWBBypassCalibration(m_i4SensorDev, (iArg1 ? MTRUE : MFALSE));
            break;

#if CAM3_AF_FEATURE_EN
        // --------------------------------- AF ---------------------------------
        case E3ACtrl_SetAFMode:
            IAfMgr::getInstance().setAFMode(m_i4SensorDev, iArg1);
            break;
        case E3ACtrl_SetAFMFPos:
            IAfMgr::getInstance().setMFPos(m_i4SensorDev, iArg1);
            break;
        case E3ACtrl_SetAFFullScanStep:
            IAfMgr::getInstance().setFullScanstep(m_i4SensorDev, iArg1);
            break;
        case E3ACtrl_SetAFCamMode:
            IAfMgr::getInstance().setCamMode(m_i4SensorDev, iArg1);
            break;
        case E3ACtrl_GetAFDAFTable:
            IAfMgr::getInstance().getDAFtbl(m_i4SensorDev, (MVOID**)iArg1);
            break;
#endif

#if CAM3_LSC_FEATURE_EN
        // --------------------------------- Shading ---------------------------------
        case E3ACtrl_SetShadingSdblkCfg:
            i4Ret = ILscMgr::getInstance(static_cast<ESensorDev_T>(m_i4SensorDev))->CCTOPSetSdblkFileCfg((iArg1 ? MTRUE : MFALSE), reinterpret_cast<const char*>(iArg2));
            break;
        case E3ACtrl_SetShadingEngMode:
            m_fgEnableShadingMeta = MFALSE;
            switch (iArg1)
            {
            case 0: // Disable
                IspTuningMgr::getInstance().enableDynamicShading(m_i4SensorDev, MFALSE);
                ILscMgr::getInstance(static_cast<ESensorDev_T>(m_i4SensorDev))->setTsfOnOff(MFALSE);
                ILscMgr::getInstance(static_cast<ESensorDev_T>(m_i4SensorDev))->setOnOff(MFALSE);
                ILscMgr::getInstance(static_cast<ESensorDev_T>(m_i4SensorDev))->setCTIdx(2);
                break;
            case 1: // Auto
                IspTuningMgr::getInstance().enableDynamicShading(m_i4SensorDev, MTRUE);
                ILscMgr::getInstance(static_cast<ESensorDev_T>(m_i4SensorDev))->setTsfOnOff(MFALSE);
                ILscMgr::getInstance(static_cast<ESensorDev_T>(m_i4SensorDev))->setOnOff(MTRUE);
                break;
            case 2: // Low
                IspTuningMgr::getInstance().enableDynamicShading(m_i4SensorDev, MFALSE);
                ILscMgr::getInstance(static_cast<ESensorDev_T>(m_i4SensorDev))->setTsfOnOff(MFALSE);
                ILscMgr::getInstance(static_cast<ESensorDev_T>(m_i4SensorDev))->setOnOff(MTRUE);
                ILscMgr::getInstance(static_cast<ESensorDev_T>(m_i4SensorDev))->setCTIdx(0);
                break;
            case 3: // Mid
                IspTuningMgr::getInstance().enableDynamicShading(m_i4SensorDev, MFALSE);
                ILscMgr::getInstance(static_cast<ESensorDev_T>(m_i4SensorDev))->setTsfOnOff(MFALSE);
                ILscMgr::getInstance(static_cast<ESensorDev_T>(m_i4SensorDev))->setOnOff(MTRUE);
                ILscMgr::getInstance(static_cast<ESensorDev_T>(m_i4SensorDev))->setCTIdx(1);
                break;
            case 4: // High
                IspTuningMgr::getInstance().enableDynamicShading(m_i4SensorDev, MFALSE);
                ILscMgr::getInstance(static_cast<ESensorDev_T>(m_i4SensorDev))->setTsfOnOff(MFALSE);
                ILscMgr::getInstance(static_cast<ESensorDev_T>(m_i4SensorDev))->setOnOff(MTRUE);
                ILscMgr::getInstance(static_cast<ESensorDev_T>(m_i4SensorDev))->setCTIdx(2);
                break;
            case 5: // TSF
            default:
                IspTuningMgr::getInstance().enableDynamicShading(m_i4SensorDev, MTRUE);
                ILscMgr::getInstance(static_cast<ESensorDev_T>(m_i4SensorDev))->setTsfOnOff(MTRUE);
                ILscMgr::getInstance(static_cast<ESensorDev_T>(m_i4SensorDev))->setOnOff(MTRUE);
                ILscMgr::getInstance(static_cast<ESensorDev_T>(m_i4SensorDev))->setCTIdx(2);
                break;
            }
            break;
        case E3ACtrl_SetShadingByp123:
            ILscMgr::getInstance(static_cast<ESensorDev_T>(m_i4SensorDev))->CCTOPSetBypass123(iArg1 ? MTRUE : MFALSE);
            break;
#endif

#if CAM3_FLASH_FEATURE_EN
        // --------------------------------- Flash ---------------------------------
        case E3ACtrl_GetQuickCalibration:
            i4Ret = FlashMgr::getInstance().cctGetQuickCalibrationResult(m_i4SensorDev);
            MY_LOG("getFlashQuickCalibrationResult ret=%d", i4Ret);
            break;
        case E3ACtrl_EnableFlashQuickCalibration:
            if(iArg1==1)
                i4Ret = FlashMgr::getInstance().cctSetSpModeQuickCalibration2(m_i4SensorDev);
            else
                i4Ret = FlashMgr::getInstance().cctSetSpModeNormal(m_i4SensorDev);
            break;
#endif

        // --------------------------------- flow control ---------------------------------
        case E3ACtrl_Enable3ASetParams:
            m_bEnable3ASetParams = iArg1;
            break;
        case E3ACtrl_SetOperMode:
            i4Ret = IspTuningMgr::getInstance().setOperMode(m_i4SensorDev, iArg1);
            break;
        case E3ACtrl_GetOperMode:
            i4Ret = IspTuningMgr::getInstance().getOperMode(m_i4SensorDev);
            break;

        // --------------------------------- get info ---------------------------------
        case E3ACtrl_GetAsdInfo:
            i4Ret = getASDInfo(*reinterpret_cast<ASDInfo_T*>(iArg1));
            break;
        case E3ACtrl_GetExifInfo:
            i4Ret = get3AEXIFInfo(*reinterpret_cast<EXIF_3A_INFO_T*>(iArg1));
            break;
        case E3ACtrl_GetSupportedInfo:
            AE_DEVICES_INFO_T rDeviceInfo;
            i4Ret = IAeMgr::getInstance().getNvramData(m_i4SensorDev);
            IAeMgr::getInstance().getSensorDeviceInfo(m_i4SensorDev, rDeviceInfo);

            FeatureParam_T* rFeatureParam;
            rFeatureParam = reinterpret_cast<FeatureParam_T*>(iArg1);
            rFeatureParam->bAutoWhiteBalanceLockSupported = IAwbMgr::getInstance().isAWBLockSupported();
            rFeatureParam->bExposureLockSupported = IAeMgr::getInstance().isAELockSupported();
            rFeatureParam->u4MaxMeterAreaNum = IAeMgr::getInstance().getAEMaxMeterAreaNum(m_i4SensorDev);
            rFeatureParam->u4FocusLength_100x = rDeviceInfo.u4FocusLength_100x; // Format: FL 3.5 = 350
            rFeatureParam->u4MaxFocusAreaNum = IAfMgr::getInstance().getAFMaxAreaNum(m_i4SensorDev);
            rFeatureParam->bEnableDynamicFrameRate = CUST_ENABLE_VIDEO_DYNAMIC_FRAME_RATE();
            rFeatureParam->i4MaxLensPos = IAfMgr::getInstance().getMaxLensPos(m_i4SensorDev);
            rFeatureParam->i4MinLensPos = IAfMgr::getInstance().getMinLensPos(m_i4SensorDev);
            rFeatureParam->i4AFBestPos = IAfMgr::getInstance().getAFBestPos(m_i4SensorDev);
            rFeatureParam->u4PureRawInterval = IAfMgr::getInstance().PDPureRawInterval(m_i4SensorDev);
            break;
        default:
            MY_LOG("[%s] Unsupport Command(%d)", __FUNCTION__, e3ACtrl);
            return MFALSE;
    }
    if(i4Ret == S_3A_OK)
        return MTRUE;
    else
        return MFALSE;
}

VOID
Hal3ARawSMVRImp::
queryTgSize(MINT32 &i4TgWidth, MINT32 &i4TgHeight)
{
    i4TgWidth = m_i4TgWidth;
    i4TgHeight = m_i4TgHeight;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setCallbacks
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32
Hal3ARawSMVRImp::
attachCb(I3ACallBack* cb)
{
    MY_LOG("[%s] m_pCbSet(0x%08x), cb(0x%08x)", __FUNCTION__, m_pCbSet, cb);
    m_pCbSet = cb;
    m_pStateMgr->attachCb(cb);
    return 0;
}

MINT32
Hal3ARawSMVRImp::
detachCb(I3ACallBack* cb)
{
    MY_LOG("[%s] m_pCbSet(0x%08x), cb(0x%08x)", __FUNCTION__, m_pCbSet, cb);
    m_pStateMgr->detachCb(cb);
    m_pCbSet = NULL;
    return 0;

}

VOID
Hal3ARawSMVRImp::
queryHbinSize(MINT32 &i4HbinWidth, MINT32 &i4HbinHeight)
{

    i4HbinWidth = m_i4HbinWidth;
    i4HbinHeight = m_i4HbinHeight;

}


