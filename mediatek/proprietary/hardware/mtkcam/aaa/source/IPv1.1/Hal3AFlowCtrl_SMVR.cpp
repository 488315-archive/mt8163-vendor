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
#define LOG_TAG "Hal3AFlowCtrl_SMVR"

#ifndef ENABLE_MY_LOG
    #define ENABLE_MY_LOG       (1)
#endif

#include <Hal3AFlowCtrl_SMVR.h>
#include <cutils/properties.h>
#include <aaa_log.h>
#include <vector>

#include <IHalSensor.h>
#ifdef USING_MTK_LDVT /*[EP_TEMP]*/ //[FIXME] TempTestOnly - USING_FAKE_SENSOR
#include <drv/src/isp/mt6797/iopipe/CamIO/FakeSensor.h>
#endif

using namespace NS3Av3;
using namespace android;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
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

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// instancing (no user count protection, only referenced by Hal3AAdpater which
// controls the life cycle from init to uninit)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Hal3AIf*
Hal3AFlowCtrl_SMVR::
createInstance(MINT32 i4SensorOpenIndex, MINT32 i4SubsampleCount)
{
#ifdef USING_MTK_LDVT
    IHalSensorList*const pHalSensorList = TS_FakeSensorList::getTestModel();
#else
    IHalSensorList*const pHalSensorList = IHalSensorList::get();
#endif
    if (!pHalSensorList) return NULL;

    MINT32 i4SensorDev = pHalSensorList->querySensorDevIdx(i4SensorOpenIndex);

    switch (i4SensorDev)
    {
    case SENSOR_DEV_MAIN:
        Hal3AFlowCtrl_SMVR_Dev<SENSOR_DEV_MAIN>::getInstance()->init(i4SensorDev, i4SensorOpenIndex, i4SubsampleCount);
        return Hal3AFlowCtrl_SMVR_Dev<SENSOR_DEV_MAIN>::getInstance();
    break;
    case SENSOR_DEV_SUB:
        Hal3AFlowCtrl_SMVR_Dev<SENSOR_DEV_SUB>::getInstance()->init(i4SensorDev, i4SensorOpenIndex, i4SubsampleCount);
        return Hal3AFlowCtrl_SMVR_Dev<SENSOR_DEV_SUB>::getInstance();
    break;
    case SENSOR_DEV_MAIN_2:
        Hal3AFlowCtrl_SMVR_Dev<SENSOR_DEV_MAIN_2>::getInstance()->init(i4SensorDev, i4SensorOpenIndex, i4SubsampleCount);
        return Hal3AFlowCtrl_SMVR_Dev<SENSOR_DEV_MAIN_2>::getInstance();
    break;
    default:
        MY_ERR("Unsupport sensor device ID: %d\n", i4SensorDev);
        AEE_ASSERT_3A_HAL("Unsupport sensor device.");
        return MNULL;
    }
}

MVOID
Hal3AFlowCtrl_SMVR::
destroyInstance()
{
    uninit();
}

Hal3AFlowCtrl_SMVR::
Hal3AFlowCtrl_SMVR()
    : Hal3AFlowCtrl()////fix resultsize
{}

Hal3AFlowCtrl_SMVR::
~Hal3AFlowCtrl_SMVR()
{}

MRESULT
Hal3AFlowCtrl_SMVR::
init(MINT32 i4SensorDev, MINT32 i4SensorOpenIndex, MINT32 i4SubsampleCount) //can be called only once by RAW or YUV, no user count
{
    m_rResultBuf.updateCapacity(i4SubsampleCount * 6);
    m_rResultBufCur.updateCapacity(i4SubsampleCount * 6);
    m_i4SubsampleCount = i4SubsampleCount;

    MY_LOG("[%s] +", __FUNCTION__);

    char cLogLevel[PROPERTY_VALUE_MAX];
    char value[PROPERTY_VALUE_MAX];
    ::property_get("debug.camera.log", cLogLevel, "0");
    m_fgLogEn = atoi(cLogLevel);
    if ( m_fgLogEn == 0 ) {
        ::property_get("debug.camera.log.hal3a", cLogLevel, "0");
        m_fgLogEn = atoi(cLogLevel);
    }

    MRESULT ret = S_3A_OK;
    MBOOL bRet = MTRUE;

#ifdef USING_MTK_LDVT
    IHalSensorList*const pHalSensorList = TS_FakeSensorList::getTestModel();
#else
    IHalSensorList*const pHalSensorList = IHalSensorList::get();
#endif
    if (!pHalSensorList) return NULL;


    I3AWrapper::E_TYPE_T eType = I3AWrapper::E_TYPE_SMVR;


    m_i4SensorDev = i4SensorDev;
    m_i4SensorOpenIdx = i4SensorOpenIndex;

    MY_LOG("[%s] eType(%d), sensor(%d), sensorIdx(%d)", __FUNCTION__, eType, i4SensorDev, i4SensorOpenIndex);

    // create 3A wrapper
    if (m_p3AWrap == NULL)
    {
        m_p3AWrap = I3AWrapper::createInstance(eType, i4SensorOpenIndex, i4SubsampleCount);
        if (m_p3AWrap)
        {
            MY_LOG("[%s] m_p3AWrapper(%p) eType(%d) created OK", __FUNCTION__, m_p3AWrap, eType);
        }
        else
        {
            MY_ERR("m_p3AWrapper created fail!");
            AEE_ASSERT_3A_HAL("m_p3AWrapper created fail!");
        }
    }

    // create Vsync event
    //IEventIrq::ConfigParam IrqConfig(i4SensorDev, i4SensorOpenIndex, 5000, IEventIrq::E_Event_Vsync);
    //m_pEventIrq = IEventIrq::createInstance(IrqConfig, "VSIrq");

    // create statistic control
#if (CAM3_3ATESTLVL >= CAM3_3ASTTUT)
    if (m_p3ASttCtrl == NULL)
    {
        m_p3ASttCtrl = Hal3ASttCtrl::createInstance(i4SensorDev, i4SensorOpenIndex);
        if (m_p3ASttCtrl)
        {
            MY_LOG("[%s] m_p3ASttCtrl(%p) created OK", __FUNCTION__, m_p3ASttCtrl);
        }
        else
        {
            MY_ERR("m_p3ASttCtrl created fail!");
            AEE_ASSERT_3A_HAL("m_p3ASttCtrl created fail!");
        }
    }
#endif

    // create AA thread
    if (m_pThread == NULL)
    {
        m_pThread = IThread3A::createInstance(this);
        if (m_pThread)
        {
            MY_LOG("[%s] m_pThread(%p) created OK", __FUNCTION__, m_pThread);
        }
        else
        {
            MY_ERR("m_pThread created fail!");
            AEE_ASSERT_3A_HAL("m_pThread created fail!");
        }
    }
    // create Sensor Buffer mode thread

  /*
    if (m_pBufferSensorThread == NULL)
    {
        m_pBufferSensorThread = ThreadSensorBufferMode::createInstance(m_i4SensorDev, m_i4SensorOpenIdx);
        if (m_pBufferSensorThread)
        {
            MY_LOG("[%s] m_pBufferSensorThread(%p) created OK", __FUNCTION__, m_pBufferSensorThread);
        }
        else
        {
            MY_ERR("m_pBufferSensorThread created fail!");
            AEE_ASSERT_3A_HAL("m_pBufferSensorThread created fail!");
        }
    }
*/
    MY_LOG("[%s] -", __FUNCTION__);
    return S_3A_OK;
}

MRESULT
Hal3AFlowCtrl_SMVR::
uninit() //can be called only once by RAW or YUV, no user count
{
    MY_LOG("[%s] +", __FUNCTION__);

    MRESULT ret = S_3A_OK;
    MBOOL bRet = MTRUE;

    if (m_pThread)
    {
        m_pThread->destroyInstance();
        m_pThread = NULL;
    }
/*
    if (m_pBufferSensorThread != NULL)
    {
        m_pBufferSensorThread->destroyInstance();
        m_pBufferSensorThread = NULL;
    }
*/
#if (CAM3_3ATESTLVL >= CAM3_3ASTTUT)
    if (m_p3ASttCtrl)
    {
        m_p3ASttCtrl->destroyInstance();
        m_p3ASttCtrl = NULL;
    }
#endif

    if (m_p3AWrap)
    {
        m_p3AWrap->destroyInstance();
        m_p3AWrap = NULL;
    }

    if (m_pCbSet)
    {
        m_pCbSet = NULL;
        MY_ERR("User did not detach callbacks!");
    }

    m_rResultBuf.clearAll();
    m_rResultBufCur.clearAll();

    MY_LOG("[%s] -", __FUNCTION__);
    return S_3A_OK;
}

MINT32
Hal3AFlowCtrl_SMVR::
config()
{
    MY_LOG("[%s]", __FUNCTION__);
#if (CAM3_3ATESTLVL >= CAM3_3ASTTUT)
        m_p3ASttCtrl->initStt(m_i4SensorDev, m_i4SensorOpenIdx, m_i4SubsampleCount);
#endif
    return m_p3AWrap->config();
}

MBOOL
Hal3AFlowCtrl_SMVR::
postCommand(ECmd_T const eCmd, const ParamIspProfile_T* pParam)
{
    MY_LOG_IF(m_fgLogEn, "[%s] enter(%d)", __FUNCTION__, eCmd);
    MUINT32 u4MagicNum = 0;
    MUINT32 u4MagicNumCur = 0;
    MBOOL bMainFlash = MFALSE;
    MBOOL bPre = MFALSE;
    ParamIspProfile_T rParamIspProf = *pParam;
    m_rCapParam.u4CapType = E_CAPTURE_NORMAL;
    RequestSet_T rRequestSet;

    MBOOL fgUpdate = (eCmd == ECmd_Update) && (rParamIspProf.iValidateOpt != ParamIspProfile_T::EParamValidate_None);
    if (eCmd == ECmd_CameraPreviewStart)
    {
        m_rResultBuf.clearAll();
        m_rResultBufCur.clearAll();
        if (m_pEventIrq == NULL)
        {
            IEventIrq::ConfigParam IrqConfig(m_i4SensorDev, m_i4SensorOpenIdx, 5000000, IEventIrq::E_Event_Vsync);
            m_pEventIrq = IEventIrq::createInstance(IrqConfig, "VSIrq");
        }
#if (CAM3_3ATESTLVL >= CAM3_3ASTTUT)
        m_p3ASttCtrl->startStt();
#endif
        m_p3AWrap->start();
    }

    if (fgUpdate)
    {
        u4MagicNum = rParamIspProf.i4MagicNum;
        u4MagicNumCur = rParamIspProf.i4MagicNumCur;
        rRequestSet = rParamIspProf.rRequestSet;

        bMainFlash = m_p3AWrap->chkMainFlashOnCond();
        bPre = m_p3AWrap->chkPreFlashOnCond();
        rParamIspProf.bMainFlash = bMainFlash;

        if (bMainFlash||bPre) m_u4FlashOnIdx = u4MagicNum;
        if (bMainFlash) m_p3AWrap->setFlashLightOnOff(1, 1);
        if (bPre) m_p3AWrap->setFlashLightOnOff(1, 0);
    }

    m_p3AWrap->postCommand(eCmd, &rParamIspProf);

    if (fgUpdate)
    {
        if (bMainFlash)// hign quality capture call back
        {
            if (m_p3AWrap->chkCapFlash())
                m_rCapParam.u4CapType = E_CAPTURE_HIGH_QUALITY_CAPTURE;
        }
        on3AProcFinish(rRequestSet, u4MagicNumCur);
    }

    if (eCmd == ECmd_CameraPreviewEnd)
    {
        m_p3AWrap->stop();
#if (CAM3_3ATESTLVL >= CAM3_3ASTTUT)
        m_p3ASttCtrl->stopStt();
        m_p3ASttCtrl->uninitStt();
#endif
        if (m_pEventIrq)
        {
            m_pEventIrq->destroyInstance("VSIrq");
            m_pEventIrq = NULL;
        }
    }

    return MTRUE;
}

MVOID
Hal3AFlowCtrl_SMVR::
on3AProcFinish(const RequestSet_T rRequestSet, MUINT32 u4MagicNumCur)
{
    RequestSet_T RequestCbSet;
    RequestCbSet = rRequestSet;
    std::vector<MINT32> rNumberSet = RequestCbSet.vNumberSet;
    std::vector<MINT32>::iterator it;
    MUINT32 u4MagicNum = 0;
    MINT32 i4SubsampleIndex = 0;
    for (it = rNumberSet.begin(); it != rNumberSet.end(); it++)
    {
        u4MagicNum = (MUINT32)(*it);
        CAM_TRACE_FMT_BEGIN("updateResult#(%d,%d)", u4MagicNum, u4MagicNumCur);
        updateResult(u4MagicNum, i4SubsampleIndex);
        CAM_TRACE_FMT_END();
        i4SubsampleIndex++;
    }
    u4MagicNum = rNumberSet[0];
    if (m_pCbSet)
    {
        MINT32 i4CurId = m_p3AWrap->getCurrentHwId();

        MY_LOG_IF(m_fgLogEn, "[%s] #(%d,%d), SOF(0x%x), m_rCapParam.capType(%d)", __FUNCTION__, u4MagicNum, u4MagicNumCur, i4CurId, m_rCapParam.u4CapType);
        CAM_TRACE_FMT_BEGIN("3A_CB #(%d), SOF(%d)", u4MagicNum, i4CurId);
        m_pCbSet->doNotifyCb(
            I3ACallBack::eID_NOTIFY_3APROC_FINISH,
            reinterpret_cast<MINTPTR> (&RequestCbSet),     // magic number
            i4CurId,        // SOF idx
            reinterpret_cast<MINTPTR> (&m_rCapParam));

#warning "FIXME"
/*
        // for flash state
        if (!(m_rParam.u4HalFlag & HAL_FLG_DUMMY))
        {
            m_pCbSet->doNotifyCb(
                I3ACallBack::eID_NOTIFY_CURR_RESULT,
                u4MagicNumCur,
                MTK_FLASH_STATE,
                m_rResult.u1FlashState);
        }
*/
        CAM_TRACE_FMT_END();
    }
}

MVOID
Hal3AFlowCtrl_SMVR::
updateResult(MUINT32 u4MagicNum, MINT32 i4SubsampleIndex)
{
    MY_LOG_IF(m_fgLogEn, "[%s] u4MagicNum(%d)", __FUNCTION__, u4MagicNum);
    // pass1 result
    m_p3AWrap->getCurrResult(u4MagicNum, m_rResult, i4SubsampleIndex);
    m_rResultBuf.updateResult(u4MagicNum, m_rResult);
    // result for high quality
    Mutex::Autolock autoLock(m_rResultMtx);

    m_rResultBufCur.updateResult(m_u4FrmIdStat, m_rResult);
    MY_LOG_IF(m_fgLogEn, "[%s] u4MagicNum(%d) HQC", __FUNCTION__, u4MagicNum);
    m_rResultCond.broadcast();
}