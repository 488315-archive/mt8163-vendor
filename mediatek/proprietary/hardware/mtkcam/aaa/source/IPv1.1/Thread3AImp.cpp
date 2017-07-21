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
#define LOG_TAG "Hal3AFlowCtrl/thread"

#ifndef ENABLE_MY_LOG
    #define ENABLE_MY_LOG   (1)
#endif
#define LOG_PERFRAME_ENABLE (1)

#include <utils/threads.h>
#include <utils/List.h>
#include <v1/config/PriorityDefs.h>
#include <utils/include/common.h>

#include <pthread.h>
#include <semaphore.h>
#include <sys/prctl.h>
#include <sys/resource.h>
#include <IThread3A.h>

#include <aaa_error_code.h>
#include <aaa_log.h>
//#include <Hal3AFlowCtrl.h>

#include <cutils/properties.h>

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

#define EN_LOG_3A_THREAD 1
#define SEM_TIME_LIMIT_NS       16000000000L

using namespace android;
using namespace NSCam::Utils;
using namespace NS3Av3;

/******************************************************************************
*
*******************************************************************************/
class Thread3AImp : public IThread3A
{
public:
    //
    static IThread3A* createInstance(Hal3AIf* pHal3A);
    virtual MBOOL destroyInstance();
    virtual MBOOL sendCommand(ECmd_T const eCmd, MUINTPTR const i4Arg = 0);

protected: //private:
    Thread3AImp(){}
    Thread3AImp(Hal3AIf* pHal3A);
    virtual ~Thread3AImp();

    Thread3AImp(const Thread3AImp&);
    Thread3AImp& operator=(const Thread3AImp&);

    /**
    * @brief create AE/AWB thread
    */
    virtual MVOID createThread();
    /**
    * @brief destroy AE/AWB thread
    */
    virtual MVOID destroyThread();
    /**
    * @brief change AE/AWB thread setting
    */
    virtual MVOID changeThreadSetting();
    /**
    * @brief AE/AWB thread execution function
    */
    static  MVOID*  onThreadLoop(MVOID*);
    /**
    * @brief add 3A commands in command queue
    * @param [in] r3ACmd 3A commands; please refer to aaa_hal_if.h
    */
    virtual MVOID addCommandQ(ECmd_T const &r3ACmd, MUINTPTR i4Arg = 0);
    /**
    * @brief clear all ECmd_Update commands in current command queue
    */
    virtual MVOID clearCommandQ();
    /**
    * @brief get 3A command from the head of 3A command queue
    * @param [in] rCmd 3A commands; please refer to aaa_hal_if.h
    * @param [out] bGetCmd bool to indicate whether command is obtained
    * @param [in] en_timeout bool to indicate whether to enable timeout mechanism or not
    */
    virtual MBOOL getCommand(IThread3A::CmdQ_T &rCmd, MBOOL &bGetCmd, MBOOL en_timeout = MFALSE);

    MBOOL sem_wait_rt(sem_t *pSem, nsecs_t reltime, const char* info);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Data member
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
protected: //private:
    Hal3AIf*        mpHal3A;
    MUINT32         mu4LogEn;
    MBOOL           mbListenUpdate;
    MINT32          mi4User;
    Mutex           mLock;
    pthread_t       mThread;
    List<CmdQ_T>    mCmdQ;
    Mutex           mModuleMtx;
    Condition       mCmdQCond;
    sem_t           mSem;
};

IThread3A*
IThread3A::createInstance(Hal3AIf* pHal3A)
{
    return Thread3AImp::createInstance(pHal3A);
}

IThread3A*
Thread3AImp::
createInstance(Hal3AIf* pHal3A)
{
    //static Thread3AImp singleton(pHal3A);
    //singleton.createThread();   // user count protected
    //return &singleton;
    Thread3AImp* pThd3A = new Thread3AImp(pHal3A);
    if (pThd3A)
        pThd3A->createThread();
    return pThd3A;
}

MBOOL
Thread3AImp::
destroyInstance()
{
    destroyThread();            // user count protected
    delete this;
    return MTRUE;
}

Thread3AImp::
Thread3AImp(Hal3AIf* pHal3A)
    : mpHal3A(pHal3A)
    , mu4LogEn(MFALSE)
    , mbListenUpdate(MFALSE)
    , mi4User(0)
    , mLock()
{
    MY_LOG("[%s] Enter (%d)", __FUNCTION__, pHal3A->getSensorDev());
}

Thread3AImp::
~Thread3AImp()
{
    MY_LOG("[%s] Exit (%d)", __FUNCTION__, mpHal3A->getSensorDev());
}
/******************************************************************************
*
*******************************************************************************/
MVOID
Thread3AImp::createThread()
{
    Mutex::Autolock lock(mLock);

    GET_PROP("debug.3a.log", "0", mu4LogEn);

    if (mi4User > 0)
    {
    }
    else
    {
        MY_LOG("[%s] +", __FUNCTION__);
        sem_init(&mSem, 0, 0);
        pthread_create(&mThread, NULL, onThreadLoop, this);
        MY_LOG("[%s] -", __FUNCTION__);
    }
    android_atomic_inc(&mi4User);
}

/******************************************************************************
*
*******************************************************************************/
MVOID
Thread3AImp::destroyThread()
{
    Mutex::Autolock lock(mLock);
    
    if (mi4User > 0)
    {
        // More than one user, so decrease one User.
        android_atomic_dec(&mi4User);

        if (mi4User == 0) // There is no more User after decrease one User
        {
            MY_LOG("[%s] +", __FUNCTION__);
            // post exit
            addCommandQ(ECmd_Uninit);
            //
            pthread_join(mThread, NULL);
            //
            MY_LOG("[%s] -", __FUNCTION__);
        }
        else    // There are still some users.
        {
             MY_LOG_IF(mu4LogEn, "[%s] mi4User(%d)", __FUNCTION__, mi4User);
        }
    }
}

/******************************************************************************
*
*******************************************************************************/
MVOID
Thread3AImp::changeThreadSetting()
{
    // (1) set name
    ::prctl(PR_SET_NAME, "3ATHREAD", 0, 0, 0);

    // (2) set policy/priority
    {
        int const expect_policy     = SCHED_OTHER;
        int const expect_priority   = NICE_CAMERA_3A_MAIN;
        int policy = 0, priority = 0;
        setThreadPriority(expect_policy, expect_priority);
        getThreadPriority(policy, priority);
        //
        MY_LOG(
            "[Hal3AFlowCtrl::onThreadLoop] policy:(expect, result)=(%d, %d), priority:(expect, result)=(%d, %d)"
            , expect_policy, policy, expect_priority, priority
        );
    }

}


/******************************************************************************
*
*******************************************************************************/
MBOOL
Thread3AImp::
sem_wait_rt(sem_t *pSem, nsecs_t reltime, const char* info)
{
    MY_LOG_IF(mu4LogEn, "[%s] pSem(%p), reltime(%lld), info(%s)\n", __FUNCTION__, pSem, reltime, info);
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
        MY_ERR("error in clock_gettime! Please check\n");

    ts.tv_sec  += reltime/1000000000;
    ts.tv_nsec += reltime%1000000000;
    if (ts.tv_nsec >= 1000000000)
    {
        ts.tv_nsec -= 1000000000;
        ts.tv_sec += 1;
    }
    int s = sem_timedwait(pSem, &ts);
    if (s == -1)
    {
        if (errno == ETIMEDOUT)
        {
            MY_ERR("[%s][%s]sem_timedwait() timed out\n", __FUNCTION__, info);
            return MFALSE;
        }
        else
            MY_ERR("[%s][%s]sem_timedwait() errno = %d\n", __FUNCTION__, info, errno);
    }
    else
        MY_LOG_IF(mu4LogEn, "[%s][%s]sem_timedwait() succeeded\n", __FUNCTION__, info);

    return MTRUE;
}

/******************************************************************************
*
*******************************************************************************/
MBOOL
Thread3AImp::sendCommand(ECmd_T const r3ACmd, MUINTPTR const i4Arg)
{
    MBOOL bRet = MTRUE;

    //sendCommandDerived(r3ACmd, i4Arg);

    switch (r3ACmd){
        //type 1: run command by 3A thread
        case ECmd_CameraPreviewStart:
            mbListenUpdate = MTRUE;
            addCommandQ(r3ACmd);
            bRet = sem_wait_rt(&mSem, SEM_TIME_LIMIT_NS, "ECmd_CameraPreviewStart");
            if (!bRet)
                AEE_ASSERT_3A_HAL("ECmd_CameraPreviewStart no reponse");
            break;

        //type 2: without wait
        case ECmd_Update:
        //case ECmd_CaptureStart:
            addCommandQ(r3ACmd, i4Arg);
            break;

        //type 3: clear previous commands and run current command by 3A thread
        case ECmd_CameraPreviewEnd:
            mbListenUpdate = MFALSE;
            clearCommandQ();
            addCommandQ(r3ACmd);
            bRet = sem_wait_rt(&mSem, SEM_TIME_LIMIT_NS, "sendCommand::mSem(2)");
            if (!bRet)
                AEE_ASSERT_3A_HAL("ECmd_CameraPreviewEnd no response");
            break;
        //case ECmd_CaptureEnd:
        case ECmd_Uninit:
            clearCommandQ();
            addCommandQ(r3ACmd);
            bRet = sem_wait_rt(&mSem, SEM_TIME_LIMIT_NS, "sendCommand::mSem(2)");
            if (!bRet)
                AEE_ASSERT_3A_HAL("ECmd_Uninit no response");
            break;

        case ECmd_PrecaptureStart: //from type1 to type3, avoid capture delay
            clearCommandQ();
            addCommandQ(r3ACmd);
            break;

        //type 4: run current command directly by caller thread
        default:
            bRet = mpHal3A->postCommand(r3ACmd, reinterpret_cast<const ParamIspProfile_T*>(i4Arg));
            if (!bRet)
                AEE_ASSERT_3A_HAL("type4 postCommand fail.");
            break;
    }

    return MTRUE;
}

/******************************************************************************
*
*******************************************************************************/
MVOID
Thread3AImp::clearCommandQ()
{
    MY_LOG("[%s]+", __FUNCTION__);
    Mutex::Autolock autoLock(mModuleMtx);

    int Qsize = mCmdQ.size();

    for (List<CmdQ_T>::iterator it = mCmdQ.begin(); it != mCmdQ.end();)
    {
        ECmd_T eCmd = it->eCmd;
        if ((eCmd == ECmd_Update) && (mCmdQ.size() >= 2)) //add size limitation to avoid mCmdQ deadlock
        {
            MY_LOG("[%s] cmd size(%d), clear(%d)", __FUNCTION__, mCmdQ.size(), eCmd);
            it = mCmdQ.erase(it);
        }
        else //include 2 cases; 1). keep all cmds except for ECmd_Update, 2). keep at least 1 ECmd_Update in mCmdQ
        {
            MY_LOG("[%s] Command not killed: cmd size(%d), beginning cmd(%d)", __FUNCTION__, mCmdQ.size(), eCmd);
            it++;
        }
    }
    MY_LOG("[%s]- Qsize(%d)", __FUNCTION__, Qsize);
}

/******************************************************************************
*
*******************************************************************************/
MVOID
Thread3AImp::addCommandQ(ECmd_T const &r3ACmd, MUINTPTR i4Arg)
{
    Mutex::Autolock autoLock(mModuleMtx);

    ParamIspProfile_T rParam;
    // data clone
    if (i4Arg != 0)
    {
        rParam = *reinterpret_cast<ParamIspProfile_T*>(i4Arg);
    }

    mCmdQ.push_back(CmdQ_T(r3ACmd, rParam));
    mCmdQCond.broadcast();

    MY_LOG_IF(mu4LogEn, "[%s] mCmdQ size(%d) + cmd(%d) magic(%d)", __FUNCTION__, mCmdQ.size(), r3ACmd, rParam.i4MagicNum);
}

/******************************************************************************
*
*******************************************************************************/
MBOOL
Thread3AImp::getCommand(IThread3A::CmdQ_T &rCmd, MBOOL &bGetCmd, MBOOL en_timeout)
{
    //returning MFALSE means break onThreadLoop
    MBOOL fgLogEn = (mu4LogEn & EN_LOG_3A_THREAD) ? MTRUE : MFALSE;
    MY_LOG_IF(fgLogEn, "[%s] +", __FUNCTION__);
    Mutex::Autolock autoLock(mModuleMtx);
    MY_LOG_IF(fgLogEn, "[%s] mCmdQ.size()=%d, en_timeout(%d)", __FUNCTION__, mCmdQ.size(), en_timeout);

    if (mCmdQ.size() == 0)
    {
        if (en_timeout)
        {
            MY_LOG_IF(fgLogEn, "[%s] mCmdQCond.waitRelative +", __FUNCTION__);
            mCmdQCond.waitRelative(mModuleMtx, (long long int)27000000);
            MY_LOG_IF(fgLogEn, "[%s] mCmdQCond.waitRelative -", __FUNCTION__);
        }
        else
        {
            MY_LOG_IF(fgLogEn, "[%s] mCmdQCond.wait +", __FUNCTION__);
            mCmdQCond.wait(mModuleMtx);
            MY_LOG_IF(fgLogEn, "[%s] mCmdQCond.wait -", __FUNCTION__);
        }
    }
    if (mCmdQ.size() == 0) //this is only for en_timeout == 1 & timeout case
    {
        MY_LOG("[%s] mCmdQ.size() = %d after mCmdQCond.wait/waitRelative\n", __FUNCTION__, mCmdQ.size());
        bGetCmd = MFALSE; // timeout, no command received
        return MTRUE;
    }
    bGetCmd = MTRUE;
    // two cases: 1. en_timeout == 1, but get command in time
    //                 2. en_timeout == 0, finally get command

    rCmd = *mCmdQ.begin();
    MY_LOG_IF(fgLogEn, "CmdQ-size(%d), todo(%d, %d)", mCmdQ.size(), rCmd.eCmd, rCmd.rParamIspProfile.i4MagicNum);

    mCmdQ.erase(mCmdQ.begin());

    if (rCmd.eCmd == ECmd_Uninit){
        return MFALSE;
    }
    else {
        return MTRUE;
    }
}

/******************************************************************************
*
*******************************************************************************/
MVOID*
Thread3AImp::onThreadLoop(MVOID *arg)
{
    MY_LOG("+");

    CmdQ_T rCmd(ECmd_Update, ParamIspProfile_T());
    Thread3AImp *_this = reinterpret_cast<Thread3AImp*>(arg);
//    AaaTimer VsyncTimer;
    MBOOL bGetCmd;
    MBOOL fgLogEn = (_this->mu4LogEn & EN_LOG_3A_THREAD) ? MTRUE : MFALSE;

    // (1) change thread setting
    _this->changeThreadSetting();

    // (2) thread-in-loop

    while (_this->getCommand(rCmd, bGetCmd, MFALSE))
    {
        //MY_LOG_IF(1, "rCmd(%d)+", rCmd);
        //
        switch (rCmd.eCmd)
        {
            case ECmd_Update:
                //
                if (!_this->mbListenUpdate)
                {
                    MY_LOG("[%s] Exit 3A thread", __FUNCTION__);
                    break;
                }
                MY_LOG_IF(fgLogEn, "rCmd(%d)+", rCmd.eCmd);

                if (!_this->mpHal3A->doUpdateCmd(&rCmd.rParamIspProfile))
                {
                    char strErr[512] = {'\0'};
                    sprintf(strErr, "ECmd_Update failed #(%d)", rCmd.rParamIspProfile.i4MagicNum);
                    MY_ERR("%s", strErr);
                    AEE_ASSERT_3A_HAL(strErr);
                }
                
                MY_LOG_IF(fgLogEn, "rCmd(%d), magic#(%d)-", rCmd.eCmd, rCmd.rParamIspProfile.i4MagicNum);
            break;

            default:
                MY_LOG_IF(fgLogEn, "rCmd(%d)+", rCmd.eCmd);
                //
                if ( ! _this->mpHal3A->postCommand(rCmd.eCmd, &rCmd.rParamIspProfile))
                {
                    MY_ERR("Cmd(%d) failed", rCmd.eCmd);
                    AEE_ASSERT_3A_HAL("onThreadLoop postCommand fail(2).");
                }
                //
                if (rCmd.eCmd != ECmd_PrecaptureStart)
                {
                    ::sem_post(&_this->mSem);
                }
                MY_LOG_IF(fgLogEn, "rCmd(%d), magic#(%d)-", rCmd.eCmd, rCmd.rParamIspProfile.i4MagicNum);
            break;
        }
        //

    }

    MY_LOG("-");

    return NULL;
}

