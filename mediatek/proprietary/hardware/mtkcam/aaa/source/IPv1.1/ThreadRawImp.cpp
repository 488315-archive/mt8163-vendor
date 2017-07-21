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
#define LOG_TAG "Hal3ARawImp/thread"

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
#include <IThreadRaw.h>

#include <aaa_error_code.h>
#include <aaa_log.h>
#include <aaa/af_feature.h>
#include <pd_buf_mgr/pd_buf_mgr.h>
#include <ae_mgr/ae_mgr_if.h>
#include <af_mgr/af_mgr.h>
#include <af_mgr/af_mgr_if.h>
#include <IEventIrq.h>

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

#define EN_LOG_RAW_THREAD 1
#define SEM_TIME_LIMIT_NS       16000000000L

using namespace android;
using namespace NSCam::Utils;
using namespace NS3Av3;

/******************************************************************************
*
*******************************************************************************/
class ThreadRawImp : public IThreadRaw
{
public:
    //
    static IThreadRaw* createInstance(Hal3ARaw* pHal3A, MINT32 iSensorDev, MINT32 iSensorIdx);
    virtual MBOOL destroyInstance();
    virtual MVOID postToAESenThread();
    virtual MVOID enableAFThread(AfStateMgr* pAfStateMgr);
    virtual MVOID disableAFThread();

    virtual MBOOL sendRequest(ECmd_T const eCmd, MUINTPTR const i4Arg = 0);


protected: //private:
    ThreadRawImp(){}
    ThreadRawImp(Hal3ARaw* pHal3A, MINT32 iSensorDev, MINT32 iSensorIdx);
    virtual ~ThreadRawImp();

    ThreadRawImp(const ThreadRawImp&);
    ThreadRawImp& operator=(const ThreadRawImp&);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  AE Thread
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /**
    * @brief create AE thread
    */
    virtual MVOID createAEThread();
    /**
    * @brief destroy AE thread
    */
    virtual MVOID destroyAEThread();
    /**
    * @brief change AE thread setting
    */
    virtual MVOID changeAEThreadSetting();
    /**
    * @brief AE thread execution function
    */
    static  MVOID*  onAEThreadLoop(MVOID*);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  AF Thread
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    /**
    * @brief change AF thread setting
    */
    virtual MVOID changeAFThreadSetting();
    /**
    * @brief AF thread execution function
    */
    static  MVOID*  onAFThreadLoop(MVOID*);

    /**
    * @brief add AF requests in request queue
    * @param [in] AF requests; please refer to IThreadRaw.h
    */
    virtual MVOID addReqestQ(IThreadRaw::ReqQ_T &rReq);
    /**
    * @brief clear all requests in current request queue
    */
    virtual MVOID clearReqestQ();
    /**
    * @brief get AF request from the head of AF request queue
    * @param [in] AF requests; please refer to IThreadRaw.h
    */
    virtual MBOOL getRequest(IThreadRaw::ReqQ_T &rReq);

    MBOOL sem_wait_rt(sem_t *pSem, nsecs_t reltime, const char* info);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Data member
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
protected: //private:
    Hal3ARaw*        mpHal3A;
    MUINT32         mu4LogEn;
    MINT32          mi4User;
    MINT32          mi4SensorDev;
    MINT32          mi4SensorIdx;
    Mutex           mLock;
    Mutex           mModuleMtx;
    // AE Thread
    Mutex           mAESenMtx;
    sem_t           semAESen;
    sem_t           semAESenThdEnd;
    MBOOL           mbEnAESenThd;
    pthread_t       mAEThread;
    // AF Thread
    MUINT32         mLastMagicNumber;
    IEventIrq*      mpAFEventIrq;
    pthread_t       mAFThread;
    MINT32          mbAFThreadLoop;
    AfStateMgr*     mpAfStateMgr;
    List<ReqQ_T>    mReqQ;
};

IThreadRaw*
IThreadRaw::createInstance(Hal3ARaw* pHal3A, MINT32 iSensorDev, MINT32 iSensorIdx)
{
    return ThreadRawImp::createInstance(pHal3A, iSensorDev, iSensorIdx);
}

IThreadRaw*
ThreadRawImp::
createInstance(Hal3ARaw* pHal3A, MINT32 iSensorDev, MINT32 iSensorIdx)
{
    ThreadRawImp* pObj = new ThreadRawImp(pHal3A, iSensorDev, iSensorIdx);
    if(pObj)
        pObj->createAEThread();
    return pObj;
}

MBOOL
ThreadRawImp::
destroyInstance()
{
    destroyAEThread();            // user count protected
    delete this;
    return MTRUE;
}

ThreadRawImp::
ThreadRawImp(Hal3ARaw* pHal3A, MINT32 iSensorDev, MINT32 iSensorIdx)
    : mpHal3A(pHal3A)
    , mu4LogEn(MFALSE)
    , mi4User(0)
    , mLock()
    , mi4SensorDev(iSensorDev)
    , mi4SensorIdx(iSensorIdx)
    , mbEnAESenThd(MFALSE)
    , mbAFThreadLoop(0)
    , mLastMagicNumber(0)
{
    GET_PROP("debug.thread_raw.log", "0", mu4LogEn);
    MY_LOG("[%s] mi4SensorDev(%d) mi4SensorIdx(%d)", __FUNCTION__, mi4SensorDev, mi4SensorIdx);
}

ThreadRawImp::
~ThreadRawImp()
{
    MY_LOG("[%s]", __FUNCTION__);
}

/******************************************************************************
* create AE thread
*******************************************************************************/
MVOID
ThreadRawImp::createAEThread()
{
    Mutex::Autolock lock(mLock);

    if (mi4User > 0)
    {
        MY_LOG_IF(mu4LogEn, "[%s] mi4User(%d)", __FUNCTION__, mi4User);
    }
    else
    {
        MY_LOG("[%s] +", __FUNCTION__);
        mbEnAESenThd = MTRUE;
        sem_init(&semAESen, 0, 0);
        sem_init(&semAESenThdEnd, 0, 1);
        MINT32 result = pthread_create(&mAEThread, NULL, onAEThreadLoop, this);
        if(result != 0)
          MY_ERR("[%s] result(%d)", __FUNCTION__, result);
        MY_LOG("[%s] -", __FUNCTION__);
    }
    android_atomic_inc(&mi4User);
}

/******************************************************************************
* destroy AE thread
*******************************************************************************/
MVOID
ThreadRawImp::destroyAEThread()
{
    Mutex::Autolock lock(mLock);

    if (mi4User > 0)
    {
        // More than one user, so decrease one User.
        android_atomic_dec(&mi4User);

        if (mi4User == 0) // There is no more User after decrease one User
        {
            MY_LOG("[%s] +", __FUNCTION__);

            mbEnAESenThd = MFALSE;
            ::sem_post(&semAESen);
            pthread_join(mAEThread, NULL);
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
* change AE thread setting
*******************************************************************************/
MVOID
ThreadRawImp::changeAEThreadSetting()
{
    // (1) set name
    ::prctl(PR_SET_NAME, "AESenThd", 0, 0, 0);

    // (2) set policy/priority
    {
        int const expect_policy     = SCHED_OTHER;
        int const expect_priority   = NICE_CAMERA_3A_MAIN;
        int policy = 0, priority = 0;
        setThreadPriority(expect_policy, expect_priority);
        getThreadPriority(policy, priority);
        //
        MY_LOG(
            "[Hal3AFlowCtrl::onAEThreadLoop] policy:(expect, result)=(%d, %d), priority:(expect, result)=(%d, %d)"
            , expect_policy, policy, expect_priority, priority
        );
    }

}

/******************************************************************************
* AE thread execution function
*******************************************************************************/
MVOID*
ThreadRawImp::onAEThreadLoop(MVOID *arg)
{
    MY_LOG("[%s] +" ,__FUNCTION__);
    // (1) change thread setting
    ThreadRawImp *_this = reinterpret_cast<ThreadRawImp*>(arg);
    _this->changeAEThreadSetting();

    // (2) thread-in-loop
    while(1)
    {
        ::sem_wait(&_this->semAESen);
        if ( ! _this->mbEnAESenThd) break;

        MY_LOG_IF(_this->mu4LogEn, "[AESensorThreadLoop] updateSensorbyI2C\n");
        CAM_TRACE_BEGIN("AE Sensor I2C");
        IAeMgr::getInstance().updateSensorbyI2C(_this->mi4SensorDev);
        CAM_TRACE_END();

        //sync with 3A thread when PreviewEnd
        {
            int Val;
            MY_LOG_IF(_this->mu4LogEn, "[Hal3A::AESensorThreadLoop] start waiting mAESenMtx lock (3)\n");
            Mutex::Autolock autoLock(_this->mAESenMtx);
            MY_LOG_IF(_this->mu4LogEn, "[Hal3A::AESensorThreadLoop] get mAESenMtx lock (3)\n");

            ::sem_getvalue(&_this->semAESenThdEnd, &Val);
            MY_LOG_IF(_this->mu4LogEn, "[AESensorThreadLoop] semAESenThdEnd before post = %d\n", Val);
            if (Val == 0) ::sem_post(&_this->semAESenThdEnd); //to be 1, 1 means AE set Sensor done, this can tolerate I2C delay too long
        }
    }

    MY_LOG("[%s] -" ,__FUNCTION__);

    return NULL;
}

/******************************************************************************
* trigger AE thread to updateSensorByI2C
*******************************************************************************/
MVOID
ThreadRawImp::postToAESenThread()
{
    Mutex::Autolock autoLock(mAESenMtx);
    int Val;
    ::sem_getvalue(&semAESenThdEnd, &Val);
    if (Val == 1) ::sem_wait(&semAESenThdEnd); //to be 0, it won't block, 0 means AE set Sensor not ready yet, this can tolerate I2C delay too long
    ::sem_post(&semAESen);
    MY_LOG_IF(mu4LogEn, "[postToAESenThread] sem_post semAESen, semAESenThdEnd before wait = %d\n", Val);
}

/******************************************************************************
* change AF thread setting
*******************************************************************************/
MVOID
ThreadRawImp::changeAFThreadSetting()
{
    // (1) set name
    ::prctl(PR_SET_NAME,"AFthread", 0, 0, 0);

    // (2) set policy/priority
    {
        int const expect_policy     = SCHED_OTHER;
        int const expect_priority   = NICE_CAMERA_AF;
        int policy = 0, priority = 0;
        setThreadPriority(expect_policy, expect_priority);
        getThreadPriority(policy, priority);
        //
        MY_LOG(
            "[changeAFThreadSetting] policy:(expect, result)=(%d, %d), priority:(expect, result)=(%d, %d)"
            , expect_policy, policy, expect_priority, priority
        );
    }

}

/******************************************************************************
* AF thread execution function
*******************************************************************************/
MVOID*
ThreadRawImp::onAFThreadLoop(MVOID *arg)
{
#define AF_START_MAGIC_NUMBER 4
    MY_LOG("[%s] +" ,__FUNCTION__);
    MY_LOG("[AFThread] tid: %d \n", gettid());

    ThreadRawImp *_this = reinterpret_cast<ThreadRawImp*>(arg);
    IEventIrq::Duration duration;
    _this->changeAFThreadSetting();

    while (_this->mbAFThreadLoop) {

        MY_LOG_IF(_this->mu4LogEn, "[Hal3A::AFThreadFunc] AfMgr::DoCallback() done\n");
        if ( _this->mpAFEventIrq->wait(duration) >= 0) // success
        {
            ReqQ_T rReq;
            MBOOL bIsAFStart = MFALSE;
            MBOOL bIsAFEnd = MFALSE;
            MBOOL bIsAFTrigger = MFALSE;

            while(_this->getRequest(rReq))
            {
                MY_LOG_IF(_this->mu4LogEn, "[%s] u4MagicNum(%d) \n", __FUNCTION__, rReq.rParam.u4MagicNum);
                switch(rReq.eCmd)
                {
                case ECmd_AFStart:
                    bIsAFStart = MTRUE;
                    break;
                case ECmd_AFEnd:
                    bIsAFEnd = MTRUE;
                    break;
                case ECmd_AFTrigger:
                    bIsAFTrigger = MTRUE;
                    break;
                case ECmd_AFUpdate:
                    _this->mLastMagicNumber = rReq.rParam.u4MagicNum;
                    if(_this->mLastMagicNumber >= AF_START_MAGIC_NUMBER)
                    {
                        IAfMgr::getInstance().SetCurFrmNum(_this->mi4SensorDev, _this->mLastMagicNumber);
                        IAfMgr::getInstance().setAFMode(_this->mi4SensorDev, rReq.rParam.u4AfMode);
                        IAfMgr::getInstance().setAndroidServiceState(_this->mi4SensorDev, rReq.rParam.bIsSupportAndroidService);
                        IAfMgr::getInstance().SetCropRegionInfo(_this->mi4SensorDev,
                                                                rReq.rParam.rScaleCropArea.i4Left,
                                                                rReq.rParam.rScaleCropArea.i4Top,
                                                                rReq.rParam.rScaleCropArea.i4Right - rReq.rParam.rScaleCropArea.i4Left,
                                                                rReq.rParam.rScaleCropArea.i4Bottom- rReq.rParam.rScaleCropArea.i4Top);
                        if (rReq.rParam.bFaceDetectEnable == MFALSE)
                            IAfMgr::getInstance().setAFArea(_this->mi4SensorDev, rReq.rParam.rFocusAreas);

                        // update af state
                        if (rReq.rParam.u4AfMode == MTK_CONTROL_AF_MODE_OFF ||
                            rReq.rParam.u4AfMode == MTK_CONTROL_AF_MODE_CONTINUOUS_PICTURE||
                            rReq.rParam.u4AfMode == MTK_CONTROL_AF_MODE_CONTINUOUS_VIDEO)
                        {
                            EState_T eCurrState = _this->mpAfStateMgr->getStateStatus().eCurrState;
                            if(eCurrState == eState_TAF)
                                _this->mpAfStateMgr->transitState(eCurrState,eState_CAF);
                        } else
                        {
                            EState_T eCurrState = _this->mpAfStateMgr->getStateStatus().eCurrState;
                            if(eCurrState == eState_CAF)
                                _this->mpAfStateMgr->transitState(eCurrState,eState_TAF);
                        }

                        if (rReq.rParam.u4AfMode == MTK_CONTROL_AF_MODE_OFF)
                        {
                            // focus dist
                            IAfMgr::getInstance().setFocusDistance(_this->mi4SensorDev, rReq.rParam.fFocusDistance);
                        }
                    }
                    break;
                }
            }

            if(bIsAFStart && bIsAFEnd)
                MY_ERR("AF start and AF End on AF irq done");

            if(bIsAFStart)
                _this->mpAfStateMgr->sendCmd(ECmd_AFStart);
            else if(bIsAFEnd)
                _this->mpAfStateMgr->sendCmd(ECmd_AFEnd);

            // AF trigger when AE is stable
            if(bIsAFTrigger)
                IAfMgr::getInstance().triggerAF(_this->mi4SensorDev);

            MY_LOG_IF(_this->mu4LogEn, "[Hal3A::AFThreadFunc] AF waitIrq done\n");
            if(_this->mLastMagicNumber >= AF_START_MAGIC_NUMBER)
            {
                _this->mpAfStateMgr->sendCmd(ECmd_AFUpdate);
            }

            MY_LOG_IF(_this->mu4LogEn, "[Hal3A::AFThreadFunc] StateMgr::sendCmd(ECmd_AFUpdate) done\n");
        }
        else
        {
            MY_ERR("[AFThread] AF irq timeout or error\n");
            IAfMgr::getInstance().TimeOutHandle(_this->mi4SensorDev);
        }
    }
    MY_LOG("[AFThread] End \n");
    MY_LOG("[%s] -" ,__FUNCTION__);

    return NULL;
}

/******************************************************************************
* Enable AF thread when previewStart.
*******************************************************************************/
MVOID
ThreadRawImp::enableAFThread(AfStateMgr* pAfStateMgr)
{
    if (mbAFThreadLoop== 0)
    {
        MY_LOG("[%s] +", __FUNCTION__);

        mpAfStateMgr = pAfStateMgr;
        mbAFThreadLoop= 1;

        // set AF irq for Vsync signal
        IEventIrq::ConfigParam IrqConfig(mi4SensorDev, mi4SensorIdx, 5000000, IEventIrq::E_Event_Af);
        mpAFEventIrq = IEventIrq::createInstance(IrqConfig, "AFThread");

        // create AF thread
        MY_LOG("[%s][AFThread] Create", __FUNCTION__);
        pthread_attr_t const attr = {0, NULL, 1024 * 1024, 4096, SCHED_OTHER, NICE_CAMERA_AF};
        MINT32 result = pthread_create(&mAFThread, &attr, onAFThreadLoop, this);
        if(result != 0)
          MY_ERR("[%s] result(%d)", __FUNCTION__, result);

        MY_LOG("[%s] -", __FUNCTION__);
    } else
    {
        MY_LOG("[AFThread] AF is working");
    }
}

/******************************************************************************
* Disable AF thread when previewEnd.
*******************************************************************************/
MVOID
ThreadRawImp::disableAFThread()
{
    if (mbAFThreadLoop == 1)
    {
        MY_LOG("[%s] +", __FUNCTION__);
        mbAFThreadLoop = 0;

        // destroy AF irq
        mpAFEventIrq->destroyInstance("AFIrq");
        mpAFEventIrq = NULL;

        MY_LOG("[AFThread] Wait for pthread_join");
        pthread_join(mAFThread, NULL);

        MY_LOG("[%s] -", __FUNCTION__);
    } else
    {
        MY_LOG("[AFThread] AF isn't working");
    }
}

/******************************************************************************
*
*******************************************************************************/
MBOOL
ThreadRawImp::
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
ThreadRawImp::
sendRequest(ECmd_T const r3ACmd, MUINTPTR const i4Arg)
{
    IThreadRaw::ReqQ_T rReq;
    rReq.eCmd = r3ACmd;
    if (i4Arg != 0)
        rReq.rParam = *reinterpret_cast<AFParam_T*>(i4Arg);
    MY_LOG_IF(mu4LogEn, "[%s] r3ACmd(%d)\n", __FUNCTION__, r3ACmd);

    addReqestQ(rReq);
    return MTRUE;
}

MVOID
ThreadRawImp::
addReqestQ(IThreadRaw::ReqQ_T &rReq)
{
    Mutex::Autolock autoLock(mModuleMtx);

    mReqQ.push_back(rReq);

    MY_LOG_IF(mu4LogEn, "[%s] mReqQ size(%d) + cmd(%d) magic(%d)", __FUNCTION__, mReqQ.size(), rReq.eCmd, rReq.rParam.u4MagicNum);
}

MVOID
ThreadRawImp::
clearReqestQ()
{
    MY_LOG("[%s]+", __FUNCTION__);
    Mutex::Autolock autoLock(mModuleMtx);

    int Qsize = mReqQ.size();

    for (List<ReqQ_T>::iterator it = mReqQ.begin(); it != mReqQ.end();)
    {
        ECmd_T eCmd = it->eCmd;
        MY_LOG("[%s] cmd size(%d), clear(%d)", __FUNCTION__, mReqQ.size(), eCmd);
        it = mReqQ.erase(it);
    }
    MY_LOG("[%s]- Qsize(%d)", __FUNCTION__, Qsize);
}

MBOOL
ThreadRawImp::
getRequest(IThreadRaw::ReqQ_T &rReq)
{
    MY_LOG_IF(mu4LogEn, "[%s] +", __FUNCTION__);
    Mutex::Autolock autoLock(mModuleMtx);
    MY_LOG_IF(mu4LogEn, "[%s] mCmdQ.size()=%d ", __FUNCTION__, mReqQ.size());

    if (mReqQ.size() == 0)
    {
        return MFALSE;
    }

    rReq = *mReqQ.begin();
    MY_LOG_IF(mu4LogEn, "mReqQ-size(%d), eCmd(%d)", mReqQ.size(), rReq.eCmd);

    mReqQ.erase(mReqQ.begin());
    MY_LOG_IF(mu4LogEn, "[%s] -", __FUNCTION__);

    return MTRUE;
}

