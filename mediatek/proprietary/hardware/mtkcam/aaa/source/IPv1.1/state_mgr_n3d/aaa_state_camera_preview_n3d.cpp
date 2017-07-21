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
#define LOG_TAG "aaa_state_camera_preview_n3d"

#ifndef ENABLE_MY_LOG
    #define ENABLE_MY_LOG       (1)
#endif

#include <aaa_types.h>
#include <aaa_state_n3d.h>
#include <aaa_state_mgr.h>
#include <aaa_scheduling_custom.h>
#include <aaa_common_custom.h>
#include <aaa_hal_if.h>
#include <buf_mgr/aaa_buf_mgr.h>

#include <debug_exif/aaa/dbg_aaa_param.h>
#include <debug_exif/aaa/dbg_af_param.h>

#include <aaa/af_param.h>
#include <aaa/awb_param.h>
#include <aaa/flash_param.h>

#include <af_feature.h>
#include <af_algo_if.h>

#include <ae_mgr/ae_mgr_if.h>
#include <af_mgr/af_mgr_if.h>
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
//  StateCameraPreview
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
StateCameraPreviewN3d::
StateCameraPreviewN3d(MINT32 sensorDevId, StateMgr* pStateMgr)
    : StateCameraPreview(sensorDevId, pStateMgr)
{
    MY_LOG("[%s] sensorDevId(%d)", __FUNCTION__, sensorDevId);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  eIntent_CameraPreviewEnd
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT
StateCameraPreviewN3d::
sendIntent(intent2type<eIntent_CameraPreviewEnd>)
{
    if (ISync3AMgr::getInstance()->isActive() && ISync3A::getInstance()->isSyncEnable())
    {
        MY_LOG("[%s] Sync 2A: Sensor(%d), Sync OFF", __FUNCTION__, m_SensorDevId);
        ISync3A::getInstance()->enableSync(MFALSE);
    }

    return StateCameraPreview::sendIntent(intent2type<eIntent_CameraPreviewEnd>());

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  eIntent_VsyncUpdate
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT
StateCameraPreviewN3d::
sendIntent(intent2type<eIntent_VsyncUpdate>)
{
    MY_LOG("CapQueueEmpty(%d)",m_pStateMgr->isCapQueueEmpty());
    if (!m_pStateMgr->isCapQueueEmpty())
    {
        doCaptureEnd();
    }
    else
        doUpdate();
    return  S_3A_OK;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  doCaptureEnd
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT
StateCameraPreviewN3d::
doCaptureEnd()
{
    MINT32 i4SceneLv = 80;
    MINT32 i4AoeCompLv = 80;
    MBOOL bAEStable = MTRUE;


    StatisticBufInfo* rBufInfo;

    rBufInfo = IAAABufMgr::getInstance().dequeueSwBuf(m_SensorDevId, STT_AAO);
    MY_LOG_IF(m_i4EnableLog, "[StateCameraPreview::doCaptureEnd()] dequeueSwBuf AAO done\n");
    StateCameraPreview::m_pStateMgr->updateMagicNumber(rBufInfo->mMagicNumber);
    MY_LOG_IF(m_i4EnableLog, "[%s] magic number = %d",__FUNCTION__, m_pStateMgr->queryMagicNumber());

    MINT32 i4Ret = 0;
    i4Ret = m_pStateMgr->queryCapQueue(rBufInfo->mMagicNumber);//if aao magic# is in queryCapQueue, do capEnd
    if (i4Ret)
    {
        MY_LOG("rBufInfo->mMagicNumber found in CapQueue, do captureEnd");
        i4SceneLv = IAeMgr::getInstance().getLVvalue(m_SensorDevId, MFALSE);
        i4AoeCompLv = IAeMgr::getInstance().getAOECompLVvalue(m_SensorDevId, MFALSE);

        if(FlashMgr::getFlashSpMode()!=e_SpModeCalibration
           && FlashMgr::getFlashSpMode()!=e_SpModeQuickCalibration
           && FlashMgr::getFlashSpMode()!=e_SpModeQuickCalibration2 )
        {// One-shot AWB
               IAwbMgr::getInstance().doCapAWB(m_SensorDevId, i4AoeCompLv, reinterpret_cast<MVOID *>(rBufInfo->mVa));
        }
        MY_LOG("IAwbMgr::getInstance().doCapAWB() END");

        IAeMgr::getInstance().doCapFlare(m_SensorDevId, reinterpret_cast<MVOID *>(rBufInfo->mVa),
        FlashMgr::getInstance().isFlashOnCapture(m_SensorDevId) );
        MY_LOG("IAeMgr::getInstance().doCapFlare() END");

#if CAM3_LSC_FEATURE_EN
    // TSF
        AWB_OUTPUT_T rAWBOutput;
        IAwbMgr::getInstance().getAWBOutput(m_SensorDevId, rAWBOutput);
        ILscMgr::TSF_AWB_INFO rAwbInfo;
        ILscMgr::TSF_INPUT_INFO_T rTsfInfo;
        ILscMgr* pLsc = ILscMgr::getInstance(static_cast<ESensorDev_T>(m_SensorDevId));
        rAwbInfo.m_i4LV        = IAeMgr::getInstance().getLVvalue(m_SensorDevId, MTRUE);
        rAwbInfo.m_u4CCT    = IAwbMgr::getInstance().getAWBCCT(m_SensorDevId);
        rAwbInfo.m_RGAIN    = rAWBOutput.rAWBInfo.rCurrentAWBGain.i4R;
        rAwbInfo.m_GGAIN    = rAWBOutput.rAWBInfo.rCurrentAWBGain.i4G;
        rAwbInfo.m_BGAIN    = rAWBOutput.rAWBInfo.rCurrentAWBGain.i4B;
        rAwbInfo.m_FLUO_IDX = rAWBOutput.rAWBInfo.i4FluorescentIndex;
        rAwbInfo.m_DAY_FLUO_IDX = rAWBOutput.rAWBInfo.i4DaylightFluorescentIndex;
        rTsfInfo.eCmd = ILscMgr::E_TSF_CMD_BATCH_CAP;
        rTsfInfo.u4FrmId = rBufInfo->mMagicNumber;
        rTsfInfo.rAwbInfo = rAwbInfo;
        rTsfInfo.prAwbStat = reinterpret_cast<MUINT8*>(rBufInfo->mVa);
        rTsfInfo.u4SizeAwbStat = rBufInfo->mSize;

        pLsc->updateTsf(rTsfInfo);

        MY_LOG("lv(%d),cct(%d),rgain(%d),bgain(%d),ggain(%d),fluoidx(%d), dayflouidx(%d)",
                rAwbInfo.m_i4LV,
                rAwbInfo.m_u4CCT,
                rAwbInfo.m_RGAIN,
                rAwbInfo.m_GGAIN,
                rAwbInfo.m_BGAIN,
                rAwbInfo.m_FLUO_IDX,
                rAwbInfo.m_DAY_FLUO_IDX
                );
#endif

        MY_LOG_IF(m_i4EnableLog, "doCaptureEnd Finish");
        }else {
        MY_LOG_IF(m_i4EnableLog, "skip update only deque, waiting CaptureEnd");
    }
    return  S_3A_OK;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  doCaptureEnd
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT
StateCameraPreviewN3d::
doUpdate()
{
    MINT32 i4SceneLv = 80;
    MINT32 i4AoeCompLv = 80;
    MINT32 i4FrmCount;
    MBOOL bAEStable = MTRUE;

    // N3D member
    MINT32 i4ActiveAeItem = 0;
    MINT32 i4SyncFrmCount = 0;
    MINT32 i4AfState = 0;
    MINT32 i4SyncOpt = 0;
    MBOOL bSyncEnable = MFALSE;

    bSyncEnable = ISync3A::getInstance()->isSyncEnable();
    i4SyncFrmCount = ISync3A::getInstance()->getFrameCount();
    i4AfState = ISync3AMgr::getInstance()->getAFState();
    
    i4ActiveAeItem = ISync3A::getInstance()->getAeSchedule();
    i4SyncOpt |= ((i4ActiveAeItem & E_AE_AE_CALC) ? ISync3A::E_SYNC3A_DO_AE : 0);
    i4SyncOpt |= ISync3A::E_SYNC3A_DO_AWB;

    // Update frame count
    m_pStateMgr->updateFrameCount();
    i4FrmCount = m_pStateMgr->getFrameCount();
    MY_LOG_IF(m_i4EnableLog
            , "[StateCameraPreview::doUpdate()] Sensor(%d), bSyncEnable(%d), i4AfState(%d), frameCnt(%d)"
            , m_SensorDevId, bSyncEnable, i4AfState, i4FrmCount);

    if (i4FrmCount < 0)
    {   // AAO statistics is not ready
        return S_3A_OK;
    }

    // Dequeue AAO buffer from SW buffer
    StatisticBufInfo* rBufInfo;
    rBufInfo = IAAABufMgr::getInstance().dequeueSwBuf(m_SensorDevId, STT_AAO);
    MY_LOG_IF(m_i4EnableLog, "[StateCameraPreview::sendIntent<eIntent_VsyncUpdate>] dequeueSwBuf AAO done\n");
    StateCameraPreview::m_pStateMgr->updateMagicNumber(rBufInfo->mMagicNumber);
    MY_LOG_IF(m_i4EnableLog, "[%s] magic number = %d",__FUNCTION__, m_pStateMgr->queryMagicNumber());

#if CAM3_AF_FEATURE_EN
    if(CUST_LOCK_AE_DURING_CAF())
    {
        if (IAfMgr::getInstance().isFocusFinish(m_SensorDevId) || //if =1, lens are fixed, do AE as usual; if =0, lens are moving, don't do AE
            (IAeMgr::getInstance().IsAEStable(m_SensorDevId) == MFALSE)) //guarantee AE can doPvAE at beginning, until IsAEStable()=1
        {
             IAeMgr::getInstance().setAFAELock(m_SensorDevId, MFALSE);
        }
        else
        {
             IAeMgr::getInstance().setAFAELock(m_SensorDevId, MTRUE);
        }
    }
    else //always do AE, no matter whether lens are moving or not
    {
        IAeMgr::getInstance().setAFAELock(m_SensorDevId, MFALSE);
    }
#endif

    // AE
    i4SceneLv = IAeMgr::getInstance().getLVvalue(m_SensorDevId, MFALSE);
    i4AoeCompLv = IAeMgr::getInstance().getAOECompLVvalue(m_SensorDevId, MFALSE);
    bAEStable = IAeMgr::getInstance().IsAEStable(m_SensorDevId);

    if (i4AfState != ISync3AMgr::E_SYNC3AMGR_AF_STATE_SCANNING)
    {
        IAeMgr::getInstance().doPvAE(m_SensorDevId, i4SyncFrmCount, reinterpret_cast<MVOID *>(rBufInfo->mVa),
        (i4ActiveAeItem & E_AE_AE_CALC), 1, 1);

        // notify AE RT params.
        FrameOutputParam_T rRTParams;
        IAeMgr::getInstance().getRTParams(m_SensorDevId,rRTParams);
        m_pStateMgr->doNotifyCb(I3ACallBack::eID_NOTIFY_AE_RT_PARAMS, rBufInfo->mMagicNumber, (MINTPTR)&rRTParams, 0);
    } else
    {
        i4SyncOpt |= ISync3A::E_SYNC3A_BYP_AE;
    }
    MY_LOG_IF(m_i4EnableLog, "[StateCameraPreview::sendIntent<eIntent_VsyncUpdate>] doPvAE done\n");

    // workaround for iVHDR
    MUINT32 u4AFSGG1Gain;
    IAeMgr::getInstance().getAESGG1Gain(m_SensorDevId, &u4AFSGG1Gain);
    IAfMgr::getInstance().setSGGPGN(m_SensorDevId, (MINT32) u4AFSGG1Gain);

    // AWB
    IAwbMgr::getInstance().doPvAWB(m_SensorDevId, i4SyncFrmCount, bAEStable, i4AoeCompLv, reinterpret_cast<MVOID *>(rBufInfo->mVa));
    MY_LOG_IF(m_i4EnableLog, "[StateCameraPreview::sendIntent<eIntent_VsyncUpdate>] doPvAWB done\n");

#if CAM3_LSC_FEATURE_EN
    // TSF
    AWB_OUTPUT_T rAWBOutput;
    IAwbMgr::getInstance().getAWBOutput(m_SensorDevId, rAWBOutput);
    ILscMgr::TSF_AWB_INFO rAwbInfo;
    ILscMgr::TSF_INPUT_INFO_T rTsfInfo;
    ILscMgr* pLsc = ILscMgr::getInstance(static_cast<ESensorDev_T>(m_SensorDevId));
    rAwbInfo.m_i4LV        = IAeMgr::getInstance().getLVvalue(m_SensorDevId, MTRUE);
    rAwbInfo.m_u4CCT    = IAwbMgr::getInstance().getAWBCCT(m_SensorDevId);
    rAwbInfo.m_RGAIN    = rAWBOutput.rAWBInfo.rCurrentAWBGain.i4R;
    rAwbInfo.m_GGAIN    = rAWBOutput.rAWBInfo.rCurrentAWBGain.i4G;
    rAwbInfo.m_BGAIN    = rAWBOutput.rAWBInfo.rCurrentAWBGain.i4B;
    rAwbInfo.m_FLUO_IDX = rAWBOutput.rAWBInfo.i4FluorescentIndex;
    rAwbInfo.m_DAY_FLUO_IDX = rAWBOutput.rAWBInfo.i4DaylightFluorescentIndex;
    rTsfInfo.eCmd = (0 == i4FrmCount) ? ILscMgr::E_TSF_CMD_BATCH : ILscMgr::E_TSF_CMD_RUN;
    rTsfInfo.u4FrmId = rBufInfo->mMagicNumber;
    rTsfInfo.rAwbInfo = rAwbInfo;
    rTsfInfo.prAwbStat = reinterpret_cast<MUINT8*>(rBufInfo->mVa);
    rTsfInfo.u4SizeAwbStat = rBufInfo->mSize;

    pLsc->updateTsf(rTsfInfo);

    MY_LOG("lv(%d),cct(%d),rgain(%d),bgain(%d),ggain(%d),fluoidx(%d), dayflouidx(%d)",
            rAwbInfo.m_i4LV,
            rAwbInfo.m_u4CCT,
            rAwbInfo.m_RGAIN,
            rAwbInfo.m_GGAIN,
            rAwbInfo.m_BGAIN,
            rAwbInfo.m_FLUO_IDX,
            rAwbInfo.m_DAY_FLUO_IDX
            );
#endif

#if CAM3_FLICKER_FEATURE_EN
    if (m_pStateMgr->mbIsRecording == MFALSE)
    {
        {
            FlickerInput flkIn;
            FlickerOutput flkOut;
            AE_MODE_CFG_T previewInfo;
            IAeMgr::getInstance().getPreviewParams(m_SensorDevId, previewInfo);
            flkIn.aeExpTime = previewInfo.u4Eposuretime;
            flkIn.afFullStat = IAfMgr::getInstance().getFLKStat(m_SensorDevId);
            //MY_LOG("qq1 bValid %d", flkIn.afFullStat.bValid);
            if(flkIn.afFullStat.bValid==1)
            {
                if(m_pHal3A->getTGInfo() == CAM_TG_1)
                {
                FlickerHalBase::getInstance().update(m_SensorDevId, &flkIn, &flkOut);
                    //MY_LOG("qq2 CAM_TG_1 %d", flkOut.flickerResult);
                }
                else
                {
                    int flkResult;
                    FlickerHalBase::getInstance().getFlickerResult(flkResult);
                    flkOut.flickerResult = flkResult;

                    //MY_LOG("qq2 CAM_TG_2 %d", flkOut.flickerResult);
                }

                if(flkOut.flickerResult == HAL_FLICKER_AUTO_60HZ)
                {
                    MY_LOG_IF(m_i4EnableLog, "setaeflicker 60hz");
                    IAeMgr::getInstance().setAEAutoFlickerMode(m_SensorDevId, 1);
                }
                else
                {
                    MY_LOG_IF(m_i4EnableLog, "setaeflicker 50hz");
                    IAeMgr::getInstance().setAEAutoFlickerMode(m_SensorDevId, 0);
                }

            }
            else
            {
                int flkResult;
                FlickerHalBase::getInstance().getFlickerResult(flkResult);
                flkOut.flickerResult = flkResult;
                if(flkOut.flickerResult == HAL_FLICKER_AUTO_60HZ)
                {
                    MY_LOG_IF(m_i4EnableLog, "setaeflicker 60hz");
                    IAeMgr::getInstance().setAEAutoFlickerMode(m_SensorDevId, 1);
            }
            else
                {
                    MY_LOG_IF(m_i4EnableLog, "setaeflicker 50hz");
                    IAeMgr::getInstance().setAEAutoFlickerMode(m_SensorDevId, 0);
                }
                //MY_LOG("qq1 skip bValid %d %d %d", flkIn.afFullStat.bValid,__LINE__,flkOut.flickerResult);
                MY_LOG_IF(m_i4EnableLog, "skip flicker");
            }

        }

    }
#endif

    if (bSyncEnable)
    {
        // 2A sync: independent AE/AWB
        MY_LOG_IF(m_i4EnableLog, "[%s] Sync 2A: Sensor(%d) =============", __FUNCTION__, m_SensorDevId);
        MINT32 i4Sync = ISync3AMgr::getSync3A()->sync(m_SensorDevId, i4SyncOpt);
        // apply
        MY_LOG_IF(m_i4EnableLog, "[%s] Sync 2A: Sensor(%d) Ready to validate (%d)", __FUNCTION__, m_SensorDevId, i4Sync);
    }
    // AAOUpdate is already true if AE needs calculation.
    if (i4AfState != ISync3AMgr::E_SYNC3AMGR_AF_STATE_SCANNING)
    {
        IAeMgr::getInstance().doPvAE(m_SensorDevId, i4SyncFrmCount, reinterpret_cast<MVOID *>(rBufInfo->mVa),
            E_AE_AE_APPLY, (i4ActiveAeItem&(E_AE_AE_CALC))?0:1, 1);
        if (IAeMgr::getInstance().IsNeedUpdateSensor(m_SensorDevId))
        {
        #if USE_AE_THD
            //m_pHal3A->mbPostAESenThd = MTRUE;
            m_pStateMgr->postToAESenThread();
        #else
            IAeMgr::getInstance().updateSensorbyI2C(m_SensorDevId);
        #endif
        }
    }

    return  S_3A_OK;

}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  eIntent_PrecaptureStart
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT
StateCameraPreviewN3d::
sendIntent(intent2type<eIntent_PrecaptureStart>)
{
    if (ISync3AMgr::getInstance()->isActive() && ISync3AMgr::getSync3A()->isSyncEnable())
    {
        MY_LOG("[%s] Sync 2A: Sensor(%d), Sync OFF", __FUNCTION__, m_SensorDevId);
        ISync3AMgr::getSync3A()->enableSync(MFALSE);
        ISync3AMgr::getSync3A(ISync3AMgr::E_SYNC3AMGR_CAPTURE)->enableSync(MTRUE);
    }

    return StateCameraPreview::sendIntent(intent2type<eIntent_PrecaptureStart>());
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  eIntent_AFStart
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT
StateCameraPreviewN3d::
sendIntent(intent2type<eIntent_AFStart>)
{
    if (ISync3AMgr::getInstance()->isActive() && ISync3AMgr::getSync3A()->isSyncEnable())
    {
        MY_LOG("[%s] Sync 2A: Sensor(%d), AFAE Start", __FUNCTION__, m_SensorDevId);
        ISync3AMgr::getInstance()->setAFState(ISync3AMgr::E_SYNC3AMGR_AF_STATE_BEGIN);
    }

    return StateCameraPreview::sendIntent(intent2type<eIntent_AFStart>());
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  eIntent_AFEnd
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MRESULT
StateCameraPreviewN3d::
sendIntent(intent2type<eIntent_AFEnd>)
{
    return StateCameraPreview::sendIntent(intent2type<eIntent_AFEnd>());
}

