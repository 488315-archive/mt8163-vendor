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
#define LOG_TAG "aaa_state_af_n3d"

#ifndef ENABLE_MY_LOG
    #define ENABLE_MY_LOG       (1)
#endif

#include <aaa_types.h>
#include <aaa_state_n3d.h>
#include <aaa_state_mgr.h>
#include <aaa_common_custom.h>
#include <aaa_scheduling_custom.h>
#include <aaa_hal_if.h>
#include <buf_mgr/aaa_buf_mgr.h>

#include <debug_exif/aaa/dbg_aaa_param.h>
#include <debug_exif/aaa/dbg_af_param.h>

#include <aaa/af_param.h>
#include <aaa/awb_param.h>
#include <aaa/flash_param.h>

#include <af_feature.h>
#include <af_algo_if.h>
#include <af_mgr/af_mgr_if.h>

#include <ae_mgr/ae_mgr_if.h>
#include <awb_mgr/awb_mgr_if.h>
#include <lsc_mgr/ILscTsf.h>
#include <flash_mgr/flash_mgr.h>
#include <flash_feature.h>
#include <flicker/flicker_hal_base.h>
#include <sensor_mgr/aaa_sensor_buf_mgr.h>

#include <awb_tuning_custom.h>
#include <flash_awb_param.h>
#include <flash_awb_tuning_custom.h>
#include <flash_tuning_custom.h>

#include <ISync3A.h>

using namespace NS3Av3;
using namespace NSIspTuning;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  StateAFN3d
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
StateAFN3d::
StateAFN3d(MINT32 sensorDevId, StateMgr* pStateMgr)
    : StateAF(sensorDevId, pStateMgr)
{
	 MY_LOG("[%s] sensorDevId(%d)", __FUNCTION__, sensorDevId);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  eIntent_VsyncUpdate
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT
StateAFN3d::
sendIntent(intent2type<eIntent_VsyncUpdate>)
{
  ::sem_wait(&m_pStateMgr->mSemAF);
    MRESULT err = S_3A_OK;

    //update frame count
    m_pStateMgr->updateFrameCount();
    MY_LOG("[StateAF::sendIntent]<eIntent_VsyncUpdate> line=%d, frameCnt=%d, EAFState=%d"
        , __LINE__
        , m_pStateMgr->getFrameCount()
        , static_cast<int>(m_pStateMgr->getAFState()));
    StatisticBufInfo* rBufInfo;

    // Dequeue AAO buffer from SW buffer
    rBufInfo = IAAABufMgr::getInstance().dequeueSwBuf(m_SensorDevId, STT_AAO);
    m_pStateMgr->updateMagicNumber(rBufInfo->mMagicNumber);
    MY_LOG_IF(m_i4EnableLog, "[%s] magic number = %d",__FUNCTION__, m_pStateMgr->queryMagicNumber());

    if (m_pStateMgr->getAFState() == eAFState_PreAF)
        err = sendAFIntent(intent2type<eIntent_VsyncUpdate>(), state2type<eAFState_PreAF>(), rBufInfo);

    if (m_pStateMgr->getAFState() == eAFState_AF)
        err = sendAFIntent(intent2type<eIntent_VsyncUpdate>(), state2type<eAFState_AF>(), rBufInfo);

    if (m_pStateMgr->getAFState() == eAFState_PostAF)
        err = sendAFIntent(intent2type<eIntent_VsyncUpdate>(), state2type<eAFState_PostAF>(), rBufInfo);

    if (m_pStateMgr->getAFState() == eAFState_Num) //at the end of AF flow, transitState & CallbackNotify
    {
        m_pStateMgr->mAFStateCntSet.resetAll(); //reset all AFState cnt, flags
        if(m_pStateMgr->getStateStatus().eNextState!=eState_Invalid)
        {
            m_pStateMgr->transitState(eState_AF, m_pStateMgr->getStateStatus().eNextState);
            m_pStateMgr->setNextState(eState_Invalid);
        }
        else
            m_pStateMgr->transitState(eState_AF, m_pStateMgr->getStateStatus().ePrevState);

        if (ISync3AMgr::getInstance()->isActive() && ISync3AMgr::getSync3A()->isSyncEnable())
        {
            MY_LOG("[%s] Sync 2A: Sensor(%d), AF End", __FUNCTION__, m_SensorDevId);
            ISync3AMgr::getInstance()->setAFState(ISync3AMgr::E_SYNC3AMGR_AF_STATE_IDLE);
        }
#if CAM3_FLASH_FEATURE_EN
        FlashMgr::getInstance().notifyAfExit(m_SensorDevId);
#endif
    }
    ::sem_post(&m_pStateMgr->mSemAF);

    return  err;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  eIntent_PrecaptureStart
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT
StateAFN3d::
sendIntent(intent2type<eIntent_PrecaptureStart>)
{
    MY_LOG("[StateAF::sendIntent]<eIntent_PrecaptureStart>");

    m_pStateMgr->setNextState(eState_Precapture);
    return  S_3A_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  eAFState_PreAF
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

MRESULT
StateAFN3d::
sendAFIntent(intent2type<eIntent_VsyncUpdate>, state2type<eAFState_PreAF>, MVOID* pBufInfo)
{
#define AFLAMP_PREPARE_FRAME 2

    MRESULT err = S_3A_OK;

    // Update frame count
    m_pStateMgr->mAFStateCntSet.PreAFFrmCnt++;
    MY_LOG("[StateAF::sendAFIntent](eIntent_VsyncUpdate,eAFState_PreAF) PreAFFrmCnt=%d"
        , m_pStateMgr->mAFStateCntSet.PreAFFrmCnt);

    if(!CUST_ONE_SHOT_AE_BEFORE_TAF())
    {
        // change to next state directly
        MY_LOG("IsDoAEInPreAF is MFALSE, triggerAF, proceedAFState()");
        IAfMgr::getInstance().triggerAF(m_SensorDevId);
        m_pStateMgr->proceedAFState();
        return  S_3A_OK;
    }

    MINT32 i4SyncFrmCount = ISync3A::getInstance()->getFrameCount();
    MINT32 i4ActiveAeItem = ISync3A::getInstance()->getAeSchedule();
    MINT32 i4SyncOpt;
    i4SyncOpt |= ((i4ActiveAeItem & E_AE_AE_CALC) ? ISync3A::E_SYNC3A_DO_AE : 0);
    i4SyncOpt |= ISync3A::E_SYNC3A_DO_AWB;

    // do AE/AWB before AF start
    StatisticBufInfo* pBuf = reinterpret_cast<StatisticBufInfo*>(pBufInfo);

    // if lamp is off, or lamp-on is ready
    if ((m_pStateMgr->mAFStateCntSet.PreAF_bNeedToTurnOnLamp == 0) ||
        (m_pStateMgr->mAFStateCntSet.PreAFFrmCnt >= (1+AFLAMP_PREPARE_FRAME)))
    {
        // AE
        IAeMgr::getInstance().doAFAE(m_SensorDevId, i4SyncFrmCount
                                   , reinterpret_cast<MVOID *>(pBuf->mVa)
                                   , i4ActiveAeItem, 1, 0);

        // workaround for iVHDR
        MUINT32 u4AFSGG1Gain;
        IAeMgr::getInstance().getAESGG1Gain(m_SensorDevId, &u4AFSGG1Gain);
#if CAM3_AF_FEATURE_EN
        IAfMgr::getInstance().setSGGPGN(m_SensorDevId, (MINT32) u4AFSGG1Gain);
#endif

        // AWB
        IAwbMgr::getInstance().doAFAWB(m_SensorDevId, reinterpret_cast<MVOID *>(pBuf->mVa));

        // Sync AE/AWB
        if (ISync3A::getInstance()->isSyncEnable())
        {
            MY_LOG_IF(m_i4EnableLog, "[%s] Sync 2A: Sensor(%d) =============", __FUNCTION__, m_SensorDevId);
            MINT32 i4Sync = ISync3AMgr::getSync3A()->sync(m_SensorDevId, i4SyncOpt);
            MY_LOG_IF(m_i4EnableLog, "[%s] Sync 2A: Sensor(%d) Ready to validate (%d)", __FUNCTION__, m_SensorDevId, i4Sync);
        }

        IAeMgr::getInstance().doAFAE(m_SensorDevId, i4SyncFrmCount, reinterpret_cast<MVOID *>(pBuf->mVa),
            E_AE_AE_APPLY, (i4ActiveAeItem&(E_AE_AE_CALC))?0:1, 1);
        if (IAeMgr::getInstance().IsNeedUpdateSensor(m_SensorDevId))
        {
        #if USE_AE_THD
            m_pStateMgr->postToAESenThread();
        #else
            IAeMgr::getInstance().updateSensorbyI2C(m_SensorDevId);
        #endif
        }

        if(IAeMgr::getInstance().IsAEStable(m_SensorDevId) == MTRUE) {
            m_pStateMgr->postToAFTrigger();
            m_pStateMgr->proceedAFState();
            MY_LOG("eAFState_PreAF, proceedAFState()");
        }
    }

    return  S_3A_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  eAFState_AF
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT
StateAFN3d::
sendAFIntent(intent2type<eIntent_VsyncUpdate>, state2type<eAFState_AF>, MVOID* pBufInfo)
{
#define AFLAMP_OFF_PREPARE_FRAME 2

    MY_LOG("[StateAF::sendAFIntent](eIntent_VsyncUpdate,eAFState_AF), AFFrmCnt=%d"
        , m_pStateMgr->mAFStateCntSet.AFFrmCnt);

    if (!IAfMgr::getInstance().isFocusFinish(m_SensorDevId))
    {
        if((FlashMgr::getInstance().isAFLampOn(m_SensorDevId)==1)
            && (FlashMgr::getInstance().getFlashMode(m_SensorDevId)!= LIB3A_FLASH_MODE_FORCE_TORCH)
            && (!m_pStateMgr->mAFStateCntSet.bLampAlreadyOnBeforeSingleAF))
            m_pStateMgr->mAFStateCntSet.AF_bNeedToTurnOffLamp=1;
        else
            m_pStateMgr->mAFStateCntSet.AF_bNeedToTurnOffLamp=0;

        if (ISync3A::getInstance()->isSyncEnable())
        {
            // 2A sync: independent AE/AWB
            MY_LOG_IF(m_i4EnableLog, "[%s] Sync 2A: Sensor(%d) =============", __FUNCTION__, m_SensorDevId);
            MINT32 i4Sync = ISync3AMgr::getSync3A()->sync(m_SensorDevId, ISync3A::E_SYNC3A_BYP_AE);
            // apply
            MY_LOG_IF(m_i4EnableLog, "[%s] Sync 2A: Sensor(%d) Ready to validate (%d)", __FUNCTION__, m_SensorDevId, i4Sync);
        }

        return S_3A_OK;
    }
    //now, isFocusFinish() == MTRUE

    m_pStateMgr->mAFStateCntSet.AFFrmCnt++;
    MY_LOG("isFocusFinish() == MTRUE, AFFrmCnt=%d, AF_bNeedToTurnOffLamp=%d"
        , m_pStateMgr->mAFStateCntSet.AFFrmCnt
        , m_pStateMgr->mAFStateCntSet.AF_bNeedToTurnOffLamp);

    if ((m_pStateMgr->mAFStateCntSet.AF_bNeedToTurnOffLamp == 0) ||
        (m_pStateMgr->mAFStateCntSet.AFFrmCnt >= (1+AFLAMP_OFF_PREPARE_FRAME)))
    {
        if (ISync3A::getInstance()->isSyncEnable())
        {
            // 2A sync: independent AE/AWB
            MY_LOG_IF(m_i4EnableLog, "[%s] Sync 2A: Sensor(%d) =============", __FUNCTION__, m_SensorDevId);
            MINT32 i4Sync = ISync3AMgr::getSync3A()->sync(m_SensorDevId, ISync3A::E_SYNC3A_BYP_AE);
            // apply
            MY_LOG_IF(m_i4EnableLog, "[%s] Sync 2A: Sensor(%d) Ready to validate (%d)", __FUNCTION__, m_SensorDevId, i4Sync);
        }
        m_pStateMgr->proceedAFState();
        MY_LOG("eAFState_AF, proceedAFState()");
        return  S_3A_OK;
    }
    //now, AF_isAFLampOn == 1 AND AFFrmCnt < 1+AFLAMP_OFF_PREPARE_FRAME
    //which means we need to do/continue our AF Lamp-off flow

    StatisticBufInfo* pBuf = reinterpret_cast<StatisticBufInfo*>(pBufInfo);

    if (m_pStateMgr->mAFStateCntSet.AFFrmCnt == 1) IAeMgr::getInstance().doRestoreAEInfo(m_SensorDevId, MFALSE);

    IAeMgr::getInstance().setRestore(m_SensorDevId, m_pStateMgr->mAFStateCntSet.AFFrmCnt/*-1*/); //-1 --> +0: is to advance by 1 frame //-1 is to align starting from 0

    if ((m_pStateMgr->mAFStateCntSet.AFFrmCnt == 1+1/*2*/) && //+2 --> +1: is to advance by 1 frame
        (FlashMgr::getInstance().getFlashMode(m_SensorDevId)!= LIB3A_FLASH_MODE_FORCE_TORCH)       )
    {
#ifdef MTK_AF_SYNC_RESTORE_SUPPORT
        MY_LOG("af sync support");
        usleep(33000);
#else
        MY_LOG("af sync NOT support");
#endif
        FlashMgr::getInstance().setAFLampOnOff(m_SensorDevId, 0);

        IAwbMgr::getInstance().setStrobeMode(m_SensorDevId, AWB_STROBE_MODE_OFF);
        IAeMgr::getInstance().setStrobeMode(m_SensorDevId, MFALSE);
    }

    IAwbMgr::getInstance().doAFAWB(m_SensorDevId, reinterpret_cast<MVOID *>(pBuf->mVa));

    return  S_3A_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  eAFState_PostAF
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT
StateAFN3d::
sendAFIntent(intent2type<eIntent_VsyncUpdate>, state2type<eAFState_PostAF>, MVOID* pBufInfo)
{
    MRESULT err = S_3A_OK;

    MY_LOG("[StateAF::sendAFIntent](eIntent_VsyncUpdate,eAFState_PostAF)");

    if(CUST_ONE_SHOT_AE_BEFORE_TAF())
    {
        m_pStateMgr->proceedAFState();
        return S_3A_OK;
    }
    // now, IsDoAEInPreAF == MFALSE
    // do AE/AWB after AF done
    StatisticBufInfo* pBuf = reinterpret_cast<StatisticBufInfo*>(pBufInfo);

        // AE
        /*NeedUpdate*///CPTLog(Event_Pipe_3A_AE, CPTFlagStart);    // Profiling Start.
    IAeMgr::getInstance().doAFAE(m_SensorDevId, m_pStateMgr->getFrameCount()
                               , reinterpret_cast<MVOID *>(pBuf->mVa)
                               , 0, 1, 0);
    if (IAeMgr::getInstance().IsNeedUpdateSensor(m_SensorDevId))
    {
    #if USE_AE_THD
        //m_pHal3A->mbPostAESenThd = MTRUE;
        m_pStateMgr->postToAESenThread();
    #else
        IAeMgr::getInstance().updateSensorbyI2C(m_SensorDevId);
    #endif
    }

    /*NeedUpdate*///CPTLog(Event_Pipe_3A_AE, CPTFlagEnd);    // Profiling Start.

    IAwbMgr::getInstance().doAFAWB(m_SensorDevId, reinterpret_cast<MVOID *>(pBuf->mVa));

    if(IAeMgr::getInstance().IsAEStable(m_SensorDevId) == MTRUE)
    {
        m_pStateMgr->proceedAFState();
        MY_LOG("eAFState_PostAF, proceedAFState()");
        return S_3A_OK;
    }


    return  S_3A_OK;
}

