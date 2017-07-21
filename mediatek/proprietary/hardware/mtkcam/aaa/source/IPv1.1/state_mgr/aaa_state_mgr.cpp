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
#ifndef LOG_TAG
#define LOG_TAG "aaa_state_mgr"
#endif

#include <aaa_state_mgr.h>
#include <aaa_state_n3d.h>

#ifndef ENABLE_MY_LOG
    #define ENABLE_MY_LOG       (1)
#endif

 using namespace NS3Av3;

StateMgr::StateMgr()
    : m_Lock()
    , m_eAFState(static_cast<IState::EAFState_T>(0))
    , m_ePrecapState(static_cast<IState::EPrecapState_T>(0))
    , m_i4FrameCount(0)
    , m_u4MagicNumber(0)
    , m_pCbSet(NULL)
    , m_u4MagicNumberRequest(0)
{
    mpCurrentState = NULL; //mpIState[eState_Uninit];
}

 StateMgr::StateMgr(MINT32 sensorDevId)
   : m_Lock()
   , m_eAFState(static_cast<IState::EAFState_T>(0))
   , m_ePrecapState(static_cast<IState::EPrecapState_T>(0))
   , m_i4FrameCount(0)
   , mSensorDevId(sensorDevId)
   , mbIsRecording(MFALSE)
   , m_u4MagicNumber(0)
   , m_pCbSet(NULL)
   , m_u4MagicNumberRequest(0)
 {
  #define STATE_INITIALIZE(_state_)\
     mpIState[eState_##_state_] = new State##_state_(sensorDevId, this);

   STATE_INITIALIZE(Init);
   STATE_INITIALIZE(Uninit);
   STATE_INITIALIZE(CameraPreview);
   STATE_INITIALIZE(Precapture);
     //STATE_INITIALIZE(Capture);
   STATE_INITIALIZE(AF);

   mpCurrentState = mpIState[eState_Uninit];
 }

 StateMgr::~StateMgr()
 {
  #define STATE_UNINITIALIZE(_state_)\
     delete mpIState[eState_##_state_];\
     mpIState[eState_##_state_] = NULL;

   STATE_UNINITIALIZE(Init);
   STATE_UNINITIALIZE(Uninit);
   STATE_UNINITIALIZE(CameraPreview);
   STATE_UNINITIALIZE(Precapture);
     //STATE_UNINITIALIZE(Capture);
   STATE_UNINITIALIZE(AF);
   m_u4MagicNumber = 0;
   m_u4MagicNumberRequest= 0;
   m_pThreadRaw = NULL;
   mpCurrentState = NULL;
 }

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// setCallbacks
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
MINT32 StateMgr::attachCb(I3ACallBack* cb)
{
    MY_LOG("[%s] m_pCbSet(0x%08x), cb(0x%08x)", __FUNCTION__, m_pCbSet, cb);
    m_pCbSet = cb;
    return 0;
}

MINT32 StateMgr::detachCb(I3ACallBack* cb)
{
    MY_LOG("[%s] m_pCbSet(0x%08x), cb(0x%08x)", __FUNCTION__, m_pCbSet, cb);
    m_pCbSet = NULL;
    return 0;

}

VOID StateMgr::doNotifyCb(MINT32 _msgType, MINTPTR _ext1, MINTPTR _ext2, MINTPTR _ext3)
{
    if (m_pCbSet)
    {
        m_pCbSet->doNotifyCb( _msgType, _ext1, _ext2, _ext3);
    }
    else
    {
        MY_LOG("[%s] m_pCbSet is NULL!", __FUNCTION__);
    }
}



MVOID StateMgr::setThreadRaw(IThreadRaw* pThreadRaw)
{
    m_pThreadRaw = pThreadRaw;
}

MVOID StateMgr::postToAESenThread()
{
    m_pThreadRaw->postToAESenThread();
}

MVOID StateMgr::postToAFTrigger()
{
    m_pThreadRaw->sendRequest(ECmd_AFTrigger);
}


 MVOID StateMgr::resetFrameCount()
 {
 #warning "FIXME"
     m_i4FrameCount = -1; // delay 2 frames for 3A statistics ready
     //m_i4FrameCount = ::getResetFrameCount(mSensorDevId);
 }

 MRESULT StateMgr::transitState(EState_T const eCurrState, EState_T const eNewState)
 {
   if (eCurrState != mStateStatus.eCurrState)
   {
     MY_ERR("[StateMgr::transitState] eCurrState != mStateStatus.eCurrState");
     return E_3A_INCORRECT_STATE;
   }

   MY_LOG("[StateCommon::transitState] %s --> %s", mpIState[mStateStatus.eCurrState]->getName(), mpIState[eNewState]->getName());
   mpCurrentState = mpIState[eNewState];
   mStateStatus.eCurrState = eNewState;
   mStateStatus.ePrevState = eCurrState;
   return S_3A_OK;
 }

MRESULT StateMgr::sendCmd(ECmd_T eCmd)
{
   Mutex::Autolock lock(m_Lock);

   EIntent_T eNewIntent = static_cast<EIntent_T>(eCmd);

  #define SEND_INTENT(_intent_)\
   case _intent_: return mpCurrentState->sendIntent(intent2type<_intent_>());\

   switch (eNewIntent)
   {
   SEND_INTENT(eIntent_CameraPreviewStart)
   SEND_INTENT(eIntent_CameraPreviewEnd)
   SEND_INTENT(eIntent_PrecaptureStart)
   SEND_INTENT(eIntent_PrecaptureEnd)
   SEND_INTENT(eIntent_CaptureStart)
   SEND_INTENT(eIntent_CaptureEnd)
   SEND_INTENT(eIntent_RecordingStart)
   SEND_INTENT(eIntent_RecordingEnd)
   SEND_INTENT(eIntent_VsyncUpdate)
   SEND_INTENT(eIntent_AFUpdate)
   SEND_INTENT(eIntent_AFStart)
   SEND_INTENT(eIntent_AFEnd)
   SEND_INTENT(eIntent_Init)
   SEND_INTENT(eIntent_Uninit)
   }
   return  -1;
}

#if CAM3_STEREO_FEATURE_EN
StateMgrN3d::
StateMgrN3d(MINT32 sensorDevId)
    : StateMgr()
{
    mpIState[eState_Init]               = new StateInitN3d(sensorDevId, this);
    mpIState[eState_Uninit]             = new StateUninit(sensorDevId, this);
    mpIState[eState_CameraPreview]      = new StateCameraPreviewN3d(sensorDevId, this);
    mpIState[eState_Precapture]         = new StatePrecaptureN3d(sensorDevId, this);
    mpIState[eState_AF]                 = new StateAFN3d(sensorDevId, this);

    mpCurrentState = mpIState[eState_Uninit];
}
#endif

MBOOL StateMgr::isCapQueueEmpty()
{
    return m_capQueue.empty();
}

MBOOL StateMgr::queryCapQueue(MINT32 maginNum)
{
    List<MINT32>::iterator it = m_capQueue.begin();
    for (; it != m_capQueue.end(); it++)
    {
        if (*it == maginNum)//found capture intent magic#
        {
            MY_LOG("[queryCapQueue] matched mag=%d", maginNum);
            m_capQueue.erase(it);//erase from queue
            return MTRUE;
        }
    }
    return MFALSE;
}
MVOID StateMgr::updateCapQueue(MINT32 maginNum)
{
    MY_LOG("[updateCapQueue] update mag=%d",maginNum);
    m_capQueue.push_back(maginNum);
}
