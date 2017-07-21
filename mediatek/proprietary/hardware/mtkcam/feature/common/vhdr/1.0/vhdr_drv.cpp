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
* @file vhdr_drv.cpp
*
* VHDR Driver Source File
*
*/

#include <utils/Errors.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <utils/threads.h>
#include <cutils/atomic.h>
#include <cutils/properties.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <semaphore.h>
#include <pthread.h>
#include <queue>

using namespace std;
using namespace android;

//#include <imageio/ispio_stddef.h>   // for  register struct  // now no need, Use INormalPipe.h


//#include <utils/include/imagebuf/IDummyImageBufferHeap.h>


//using namespace NSImageio;
//using namespace NSIspio;

#include <drv/isp_reg.h> // for register name
#include <drv/isp_drv.h> // for isp driver object

#include <v1/config/PriorityDefs.h>
#include <iopipe/CamIO/INormalPipe.h>    // for pass1 register setting API
#include <IHalSensor.h>

#include <imageio/Cam_Notify_datatype.h> // for LCS_REG_CFG definition

using namespace NSCam;
using namespace NSIoPipe;
using namespace NSCamIOPipe;

//#include "mtkcam/drv/imem_drv.h"
//#include "camera_custom_vhdr.h"    // TODO-not suer and file is not exist

#include "isp_mgr/isp_mgr.h"

//using namespace NSIspTuning;
using namespace NSIspTuningv3;

#include <utils/include/common.h>
#include <hwutils/HwMisc.h>
using namespace NSCam::Utils;

#include "vhdr_drv_imp.h"
#include "common/vhdr/1.0/vhdr_debug.h"

/*******************************************************************************
*
********************************************************************************/
#define VHDR_DRV_DEBUG

#ifdef VHDR_DRV_DEBUG

#undef __func__
#define __func__ __FUNCTION__

#undef LOG_TAG
#define LOG_TAG "VHDRDrv"
#define VHDR_LOG(fmt, arg...)    CAM_LOGD("[%s]" fmt, __func__, ##arg)
#define VHDR_INF(fmt, arg...)    CAM_LOGI("[%s]" fmt, __func__, ##arg)
#define VHDR_WRN(fmt, arg...)    CAM_LOGW("[%s] WRN(%5d):" fmt, __func__, __LINE__, ##arg)
#define VHDR_ERR(fmt, arg...)    CAM_LOGE("[%s] %s ERROR(%5d):" fmt, __func__,__FILE__, __LINE__, ##arg)

#else
#define VHDR_LOG(a,...)
#define VHDR_INF(a,...)
#define VHDR_WRN(a,...)
#define VHDR_ERR(a,...)
#endif

#define MY_LOGD_IF(cond, ...)       do { if ( (cond) >= (2) ) { VHDR_LOG(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) >= (1) ) { VHDR_INF(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) >= (1) ) { VHDR_WRN(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) >= (0) ) { VHDR_ERR(__VA_ARGS__); } }while(0)


#define VHDR_DRV_NAME "VHDRDrv"
#define ROUND_TO_2X(x) ((x) & (~0x1))
#define ALIGN_SIZE(in,align) (in & ~(align-1))

#define NAME "VHDRDrv"
/*******************************************************************************
*
********************************************************************************/
static MINT32 g_debugDump = 0;
static MINT32 g_disable = 0;
static MINT32 gLcsoDebugDump = 0;

/*******************************************************************************
*
********************************************************************************/
VHdrDrv *VHdrDrv::CreateInstance(const MUINT32 &aSensorIdx)
{
    return VHdrDrvImp::GetInstance(aSensorIdx);
}

/*******************************************************************************
*
********************************************************************************/
VHdrDrv *VHdrDrvImp::GetInstance(const MUINT32 &aSensorIdx)
{
    VHDR_LOG("aSensorIdx(%u)",aSensorIdx);
    return new VHdrDrvImp(aSensorIdx);
}

/*******************************************************************************
*
********************************************************************************/
MVOID VHdrDrvImp::DestroyInstance()
{
    VHDR_LOG("+");
	delete this;
}

/*******************************************************************************
*
********************************************************************************/
VHdrDrvImp::VHdrDrvImp(const MUINT32 &aSensorIdx) : VHdrDrv()
{
    mUsers = 0;

    m_pNormalPipe = NULL;

#if VHDR_RUN_LEGACY
    m_pISPDrvObj = NULL;
    m_pISPVirtDrv = NULL;  // for command queue
    m_pIMemDrv = NULL;
#endif

    mLcsEn = MFALSE;
    mLcsD_En = MFALSE;
    mLcs_outW = 0;
    mLcs_outH = 0;
    mLcsD_outW = 0;
    mLcsD_outH = 0;
    mSensorIdx = aSensorIdx;
    mFlareGain = 0x100;
    mFlareOffset = 0;
    mIsVhdrConfig = 0;

    mLcso_memInfo.memID = -5;
    mLcso_memInfo.useNoncache = 1;
    mLcso_memInfo.virtAddr = mLcso_memInfo.phyAddr = mLcso_memInfo.size = 0;
    mLcsoD_memInfo.memID = -5;
    mLcsoD_memInfo.useNoncache = 1;
    mLcsoD_memInfo.virtAddr = mLcsoD_memInfo.phyAddr = mLcsoD_memInfo.size = 0;
    mLcso_addrIdx = 0;
    mLcso_virAddr = 0;
    mLcso_phyAddr = 0;
    mLcsoD_addrIdx = 0;
    mLcsoD_phyAddr = 0;
    mLcsoD_virAddr = 0;

    mSensorDev = 0;
    mSensorTg  = CAM_TG_NONE;

    mState = VHDR_STATE_NONE;

    mVrWithEis = MFALSE;
    mLcsAws    = 0;
    mLcsD_Aws  = 0;
    mVideoPrvW = 0;
    mVideoPrvH = 0;
    mPass1OutW = 0;
    mPass1OutH = 0;
    mZoomRatio = 100;
    mpVHdrP1Cb = NULL;

    while(!mLcsoData.empty())
    {
        mLcsoData.pop();
    }

    while(!mLcsoData4P2.empty())
    {
        mLcsoData4P2.pop();
    }

    while(!mLcsoD_Data.empty())
    {
        mLcsoD_Data.pop();
    }

    while(!mLcsoD_Data4P2.empty())
    {
        mLcsoD_Data4P2.pop();
    }
}

#if 1
/*******************************************************************************
*
********************************************************************************/
#if VHDR_RUN_LEGACY
MVOID *VHdrDrvImp::LcsoThreadLoop(MVOID *arg)
{
    VHdrDrvImp *_this = reinterpret_cast<VHdrDrvImp *>(arg);
    MINT32 err = VHDR_RETURN_NO_ERROR;

    //====== Change Thread Setting ======

    _this->ChangeThreadSetting("LcsoThreadLoop");

    //====== Main Loop ======

    VHDR_LOG("wait mLcsoSem");
    ::sem_wait(&_this->mLcsoSem); // wait here until someone use sem_post() to wake this semaphore up
    VHDR_LOG("got mLcsoSem");

    VHDR_STATE_ENUM eState = _this->GetVHdrState();

    while(eState != VHDR_STATE_UNINIT)
    {
        eState = _this->GetVHdrState();

        switch(eState)
        {
            case VHDR_STATE_ALIVE:
                    // 1. wait Vsync
                    err = _this->UpdateLcso();
                    if(VHDR_RETURN_NO_ERROR != err)
                    {
                        MY_LOGE_IF(g_debugDump, "UpdateLcso fail(%d)",err);
                    }

                    // 2. wait pass1 done
                    err = _this->UpdateLcsoIdx();
                    if(VHDR_RETURN_NO_ERROR != err)
                    {
                        MY_LOGE_IF(g_debugDump, "UpdateLcsoIdx fail(%d)",err);
                    }
                break;
            case VHDR_STATE_UNINIT:
                VHDR_LOG("UNINIT");
                break;
            default:
                MY_LOGE_IF(g_debugDump, "State Error(%d)",eState);
        }

        eState = _this->GetVHdrState();
    }

    ::sem_post(&_this->mLcsoSemEnd);
    VHDR_LOG("post mLcsoSemEnd");

    ::sem_post(&_this->mUninitSem);
    VHDR_LOG("post mUninitSem");

    return NULL;
}

#endif

/*******************************************************************************
*
********************************************************************************/
MINT32 VHdrDrvImp::Init(MUINT32 lcsoW, MUINT32 lcsoH)
{
    Mutex::Autolock lock(mLock);

    //====== Reference Count ======

    if(mUsers > 0)
    {
        android_atomic_inc(&mUsers);
        MY_LOGD_IF(g_debugDump, "mSensorIdx(%u) has %d users",mSensorIdx,mUsers);
        return VHDR_RETURN_NO_ERROR;
    }

    MINT32 err = VHDR_RETURN_NO_ERROR;

    //====== Dynamic Debug ======

    g_debugDump = ::property_get_int32(VHDR_DEBUG_PROP, 0);

    g_disable = ::property_get_int32(VHDR_DISABLE_PROP, 0);

    //====== Set State ======

    SetVHdrState(VHDR_STATE_ALIVE);

    MY_LOGD_IF(g_debugDump, "mSensorIdx(%u) init",mSensorIdx);
#if VHDR_RUN_LEGACY
    //====== Create ISP Driver Object  ======

    m_pISPDrvObj = IspDrv::createInstance();
    if(m_pISPDrvObj == NULL)
    {
        MY_LOGE_IF(g_debugDump, "m_pISPDrvObj create instance fail");
        err = VHDR_RETURN_NULL_OBJ;
        return err;
    }

    if(MFALSE == m_pISPDrvObj->init(VHDR_DRV_NAME))
    {
        MY_LOGE_IF(g_debugDump, "m_pISPDrvObj->init() fail");
        err = VHDR_RETURN_API_FAIL;
        return err;
    }

    //====== Create IMem Object ======

    m_pIMemDrv = IMemDrv::createInstance();
    if(m_pIMemDrv == NULL)
    {
        MY_LOGE_IF(g_debugDump, "Null IMemDrv Obj");
        err = VHDR_RETURN_NULL_OBJ;
        return err;
    }

    if(MFALSE == m_pIMemDrv->init(NAME))
    {
        MY_LOGE_IF(g_debugDump, "m_pIMemDrv->init fail");
        err = VHDR_RETURN_API_FAIL;
        return err;
    }
#endif
    #if 0   // opend it when needed
    // Command Queue
    m_pISPVirtDrv = m_pISPDrvObj->getCQInstance(ISP_DRV_CQ0);
    if(m_pISPVirtDrv == NULL)
    {
        MY_LOGE_IF(g_debugDump, "m_pISPVirtDrv create instance fail");
        err = EIS_RETURN_NULL_OBJ;
        return err;
    }
    #endif

    //====== Create INormalPipe Object ======

    m_pNormalPipe = INormalPipe::createInstance(mSensorIdx, VHDR_DRV_NAME);
    if(m_pNormalPipe == NULL)
    {
        MY_LOGE_IF(g_debugDump, "create INormalPipe fail");
        err = VHDR_RETURN_NULL_OBJ;
        return err;
    }


    //====== Create Pass1 Callback Class ======

    mpVHdrP1Cb = new VHdrP1Cb(this);

    //====== Create Thread ======
#if VHDR_RUN_LEGACY
    //> init semphore

    ::sem_init(&mLcsoSem, 0, 0);
    ::sem_init(&mLcsoSemEnd, 0, 0);
    ::sem_init(&mTsSem, 0, 0);
    ::sem_init(&mUninitSem, 0, 0);

    //> create thread

    pthread_create(&mLcsoThread, NULL, LcsoThreadLoop, this);
#endif
    android_atomic_inc(&mUsers);    // increase reference count


    // ===== Initial LCSO w,h (Jou-Feng Add) =========
   mLcs_outW = lcsoW;   // 64
   mLcs_outH = lcsoH;   // 48
   mLcsD_outW = lcsoW;   // 64
   mLcsD_outH = lcsoH;   // 48

#if 0
    //mLcs_outW = 4;   // test
    //mLcs_outH = 4;
    //mLcsD_outW = 4;
    //mLcsD_outH = 4;

    if(mSensorTg == CAM_TG_1)
    {
        CreateLcsoBuffers(mLcs_outW, mLcs_outH, LCSO_BUFFER_NUM, m_vSliceBufs);
    }
    else if(mSensorTg == CAM_TG_2)
    {
        CreateLcsoBuffers(mLcsD_outW, mLcsD_outH, LCSO_BUFFER_NUM, m_vSliceBufs);
    }else
    {
        MY_LOGE_IF(g_debugDump, "sensorTg = %d, use default w,h to allocate memory", mSensorTg);
        CreateLcsoBuffers(mLcs_outW, mLcs_outH, LCSO_BUFFER_NUM, m_vSliceBufs);
    }
#endif
    MY_LOGD_IF(g_debugDump, "-");
    return err;
}

/*******************************************************************************
*
********************************************************************************/
MINT32 VHdrDrvImp::Uninit()
{
    Mutex::Autolock lock(mLock);

    //====== Reference Count ======

    if(mUsers <= 0) // No more users
    {
        MY_LOGD_IF(g_debugDump, "mSensorIdx(%u) has 0 user",mSensorIdx);
        return VHDR_RETURN_NO_ERROR;
    }

    // >= one user
    android_atomic_dec(&mUsers);

    MINT32 err = VHDR_RETURN_NO_ERROR;

    if(mUsers == 0)
    {
        //====== Set State ======

        SetVHdrState(VHDR_STATE_UNINIT);

        MY_LOGD_IF(g_debugDump, "mSensorIdx(%u) uninit",mSensorIdx);
#if VHDR_RUN_LEGACY
        //====== Destory ISP Driver Object ======

        if(m_pISPVirtDrv != NULL)
        {
            m_pISPVirtDrv = NULL;
        }

        if(m_pISPDrvObj != NULL)
        {
            if(MFALSE == m_pISPDrvObj->uninit(VHDR_DRV_NAME))
            {
                MY_LOGE_IF(g_debugDump, "m_pISPDrvObj->uninit fail");
                err = VHDR_RETURN_API_FAIL;
            }

            m_pISPDrvObj->destroyInstance();
            m_pISPDrvObj = NULL;
        }

        //====== Destory IMem ======

        if(m_pIMemDrv != NULL)
        {
            DestroyMemBuf(1,&mLcso_memInfo);
            DestroyMemBuf(1,&mLcsoD_memInfo);

            mLcso_memInfo.memID = -5;
            mLcso_memInfo.useNoncache = 1;
            mLcso_memInfo.virtAddr = mLcso_memInfo.phyAddr = mLcso_memInfo.size = 0;
            mLcsoD_memInfo.memID = -5;
            mLcsoD_memInfo.useNoncache = 1;
            mLcsoD_memInfo.virtAddr = mLcsoD_memInfo.phyAddr = mLcsoD_memInfo.size = 0;

            mLcso_addrIdx = 0;
            mLcso_virAddr = 0;
            mLcso_phyAddr = 0;
            mLcsoD_addrIdx = 0;
            mLcsoD_virAddr = 0;
            mLcsoD_phyAddr = 0;

            if(MFALSE == m_pIMemDrv->uninit(NAME))
            {
                MY_LOGE_IF(g_debugDump, "m_pIMemDrv->uninit fail");
                err = VHDR_RETURN_API_FAIL;
            }

            m_pIMemDrv->destroyInstance();
            m_pIMemDrv = NULL;
        }
#else
        //DestroyLcsoBuffers(m_vSliceBufs);
#endif
        //====== Destory INormalPipe ======

        if(m_pNormalPipe != NULL)
        {
            m_pNormalPipe->destroyInstance(VHDR_DRV_NAME);
            m_pNormalPipe = NULL;
        }


        //====== Rest Member Variable ======

        mUsers = 0;
        mLcsEn = MFALSE;
        mLcsD_En = MFALSE;
        mLcs_outW = 0;
        mLcs_outH = 0;
        mLcsD_outW = 0;
        mLcsD_outH = 0;
        mSensorIdx = 0;
        mFlareGain = 0x100;
        mFlareOffset = 0;
        mIsVhdrConfig = 0;

        mSensorDev = 0;
        mSensorTg  = CAM_TG_NONE;

        mVrWithEis = MFALSE;
        mLcsAws    = 0;
        mLcsD_Aws  = 0;
        mVideoPrvW = 0;
        mVideoPrvH = 0;
        mPass1OutW = 0;
        mPass1OutH = 0;

        // delete pass1 callback class

        delete mpVHdrP1Cb;
        mpVHdrP1Cb = NULL;
#if VHDR_RUN_LEGACY
        //====== Destroy Thread ======

        //> wait sem
        MY_LOGD_IF(g_debugDump, "wait mLcsoSemEnd");
        ::sem_wait(&mLcsoSemEnd);
        MY_LOGD_IF(g_debugDump, "got mLcsoSemEnd");

        //> delete thread
        pthread_join(mLcsoThread, NULL);
#endif
        //====== Clean Queue ======

        while(!mLcsoData.empty())
        {
            mLcsoData.pop();
        }

        while(!mLcsoData4P2.empty())
        {
            mLcsoData4P2.pop();
        }

        while(!mLcsoD_Data.empty())
        {
            mLcsoD_Data.pop();
        }

        while(!mLcsoD_Data4P2.empty())
        {
            mLcsoD_Data4P2.pop();
        }

        //====== Set State ======

        SetVHdrState(VHDR_STATE_NONE);
    }
    else
    {
         MY_LOGD_IF(g_debugDump, "mSensorIdx(%u) has %d users",mSensorIdx,mUsers);
    }

    MY_LOGD_IF(g_debugDump, "X");
    return err;
}

/*******************************************************************************
*
********************************************************************************/
MVOID VHdrDrvImp::SetFlareInfo(const MINT16 &aFlareGain,const MINT16 &aFlareOffset)
{
    mFlareGain = aFlareGain >> 1;   // AE is 512 based, LCS is 256 based, confirm with CC
    mFlareOffset = aFlareOffset >> 2;
    VHDR_LOG( "(gain,ofst)=I(%u,%u),O(%u,%u)",aFlareGain,aFlareOffset,mFlareGain,mFlareOffset);
}

/*******************************************************************************
*
********************************************************************************/
MVOID VHdrDrvImp::SetSensorInfo(const MUINT32 &aSensorDev,const MUINT32 &aSensorTg)
{
    mSensorDev = aSensorDev;
    mSensorTg  = aSensorTg;
    VHDR_LOG( "(dev,tg)=(%u,%u)",mSensorDev,mSensorTg);
}

/*******************************************************************************
*
********************************************************************************/
MVOID VHdrDrvImp::SetLcsoThreadState(VHDR_STATE_ENUM aState)
{
    MY_LOGD_IF(g_debugDump, "aState(%d)",aState);


    if(aState == VHDR_STATE_ALIVE)
    {
        //====== ConfigVHdr Done ======

        mIsVhdrConfig = 1;

#if VHDR_RUN_LEGACY
        //====== Clear Pass1 Done Signal First ======

        ISP_DRV_WAIT_IRQ_STRUCT waitIrq;

        waitIrq.Clear      = ISP_DRV_IRQ_CLEAR_STATUS;
        waitIrq.Type       = (mSensorTg == CAM_TG_1) ? ISP_DRV_IRQ_TYPE_INT_P1_ST : ISP_DRV_IRQ_TYPE_INT_P1_ST_D;
        waitIrq.Status     = (mSensorTg == CAM_TG_1) ? CAM_CTL_INT_P1_STATUS_PASS1_DON_ST : CAM_CTL_INT_P1_STATUS_D_PASS1_DON_ST;
        waitIrq.Timeout    = 411;
        waitIrq.UserNumber = ISP_DRV_IRQ_USER_VHDR;
        waitIrq.UserName   = const_cast<char *>("VHDR");

        if(MTRUE != m_pISPDrvObj->waitIrq(&waitIrq))
        {
            MY_LOGE_IF(g_debugDump, "ISP_DRV_IRQ_CLEAR_STATUS fail");
            MY_LOGE_IF(g_debugDump, "TG(%d),Clear(%d),Type(%d),Status(%d),Timeout(%u),UserNumber(%d)",mSensorTg,
                                                                                       waitIrq.Clear,
                                                                                       waitIrq.Type,
                                                                                       waitIrq.Status,
                                                                                       waitIrq.Timeout,
                                                                                       waitIrq.UserNumber);
        }

        //====== Post Sem ======

        ::sem_post(&mLcsoSem);
        MY_LOGD_IF(g_debugDump, "post mLcsoSem");
#endif

    }
    else if(aState == VHDR_STATE_UNINIT)
    {
        SetVHdrState(VHDR_STATE_UNINIT);
#if VHDR_RUN_LEGACY
        ISP_DRV_WAIT_IRQ_STRUCT waitIrq;

		    waitIrq.Type	   = (mSensorTg == CAM_TG_1) ? ISP_DRV_IRQ_TYPE_INT_P1_ST : ISP_DRV_IRQ_TYPE_INT_P1_ST_D;
		    waitIrq.Status	   = (mSensorTg == CAM_TG_1) ? CAM_CTL_INT_P1_STATUS_VS1_INT_ST : CAM_CTL_INT_P1_STATUS_D_VS1_INT_ST;
		    waitIrq.UserNumber = ISP_DRV_IRQ_USER_VHDR;

		    if(MTRUE != m_pISPDrvObj->flushIrq(waitIrq))
        {
            MY_LOGE_IF(g_debugDump, "Flush irq fail");
        }

		    waitIrq.Type	   = (mSensorTg == CAM_TG_1) ? ISP_DRV_IRQ_TYPE_INT_P1_ST : ISP_DRV_IRQ_TYPE_INT_P1_ST_D;
		    waitIrq.Status	   = (mSensorTg == CAM_TG_1) ? CAM_CTL_INT_P1_STATUS_PASS1_DON_ST : CAM_CTL_INT_P1_STATUS_D_PASS1_DON_ST;
		    waitIrq.UserNumber = ISP_DRV_IRQ_USER_VHDR;

		    if(MTRUE != m_pISPDrvObj->flushIrq(waitIrq))
        {
            MY_LOGE_IF(g_debugDump, "Flush irq fail");
        }

        MY_LOGD_IF(g_debugDump, "wait mUninitSem");
        ::sem_wait(&mUninitSem);    // block MW thread to prevent no VD or pass1 done signal
#endif
    }

}

/*******************************************************************************
*
********************************************************************************/
MVOID VHdrDrvImp::SetVideoSizeInfo(const VHDR_CMD_ENUM &aCmd,MINT32 arg1,MINT32 arg2,MINT32 arg3)
{
    switch(aCmd)
    {
        case VHDR_CMD_SET_VR_EIS_ON_OFF:
            mVrWithEis = arg1;
            if(g_debugDump > 0)
            {
                VHDR_LOG("mVrWithEis(%d)",mVrWithEis);
            }
            break;
        case VHDR_CMD_SET_VIDEO_SIZE:
            mVideoPrvW = arg1;
            mVideoPrvH = arg2;
            if(g_debugDump > 0)
            {
                VHDR_LOG( "VideoPrv(W,H)=(%d,%d)",mVideoPrvW,mVideoPrvH);
            }
            break;
        case VHDR_CMD_SET_ZOOM_RATIO:
            mZoomRatio = (MUINT32)arg1;
            if(g_debugDump > 0)
            {
                VHDR_LOG( "zoomRatio(%u)",mZoomRatio);
            }
            break;
        case VHDR_CMD_SET_PASS1_OUT_SIZE:
            mPass1OutW = arg1;
            mPass1OutH = arg2;
            if(g_debugDump > 0)
            {
                VHDR_LOG( "Pass1Out(W,H)=(%d,%d)",mPass1OutW,mPass1OutH);
            }
            break;
        default:
            MY_LOGE_IF(g_debugDump, "wrong cmd(%d)",aCmd);
    }
}

#if 0
/*******************************************************************************
*
********************************************************************************/
MVOID VHdrDrvImp::DoVHdr(LCEI_DMA_INFO *apLceiDmaInfo,const android::sp<IImageBuffer>& lcsoBuf)
{
    if(mSensorTg == CAM_TG_1){
        apLceiDmaInfo->xSize       = mLcs_outW - 1;
        apLceiDmaInfo->ySize       = mLcs_outH - 1;
        apLceiDmaInfo->stride      = mLcs_outW;
        apLceiDmaInfo->memID       = lcsoBuf->getFD();
        apLceiDmaInfo->va          = lcsoBuf->getBufVA(0);
        apLceiDmaInfo->pa          = lcsoBuf->getBufPA(0);
        apLceiDmaInfo->size        = mLcs_outW * mLcs_outH;
        apLceiDmaInfo->bufSecu     = mLcso_memInfo.bufSecu;  // default value
        apLceiDmaInfo->bufCohe     = mLcso_memInfo.bufCohe;  // default value
        apLceiDmaInfo->useNoncache = mLcso_memInfo.useNoncache; // default value
    }
    else if(mSensorTg == CAM_TG_2)
    {
        apLceiDmaInfo->xSize       = mLcsD_outW - 1;
        apLceiDmaInfo->ySize       = mLcsD_outH - 1;
        apLceiDmaInfo->stride      = mLcsD_outW;
        apLceiDmaInfo->memID       = lcsoBuf->getFD();
        apLceiDmaInfo->va          = lcsoBuf->getBufVA(0);
        apLceiDmaInfo->pa          = lcsoBuf->getBufPA(0);
        apLceiDmaInfo->size        = mLcsD_outW * mLcsD_outH;
        apLceiDmaInfo->bufSecu     = mLcsoD_memInfo.bufSecu;  // default value
        apLceiDmaInfo->bufCohe     = mLcsoD_memInfo.bufCohe;  // default value
        apLceiDmaInfo->useNoncache = mLcsoD_memInfo.useNoncache; // default value
    }
    else
    {
        MY_LOGE_IF(g_debugDump, "wrong TG(%u) in DoVHdr",mSensorTg);
    }
}
#endif

/*******************************************************************************
*
********************************************************************************/
MVOID VHdrDrvImp::GetLceiDmaInfo(LCEI_DMA_INFO *apLceiDmaInfo,const MINT64 &aTimeStamp)
{
    //====== Start Update LCEI ======

    Mutex::Autolock lock(mQueLock);

    MY_LOGD_IF(g_debugDump, "tg(%u),TS(%lld)",mSensorTg,aTimeStamp);

    MUINT32 hasMatch = 0;

    if(mSensorTg == CAM_TG_1)
    {
        if(mLcsoData4P2.empty())
        {
            MY_LOGE_IF(g_debugDump, "mLcsoData4P2 empty)");
            return;
        }

        while(!mLcsoData4P2.empty())
        {
            if(mLcsoData4P2.front().timeStamp < aTimeStamp)
            {
                MY_LOGD_IF(g_debugDump, "drop,PA(0x%08x),TS(%lld)",mLcsoData4P2.front().pa,mLcsoData4P2.front().timeStamp);
                mLcsoData4P2.pop();
            }
            else if(mLcsoData4P2.front().timeStamp == aTimeStamp)
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
            apLceiDmaInfo->xSize       = mLcs_outW - 1;
            apLceiDmaInfo->ySize       = mLcs_outH - 1;
            apLceiDmaInfo->stride      = mLcs_outW;
            apLceiDmaInfo->memID       = mLcso_memInfo.memID;
            apLceiDmaInfo->va          = mLcsoData4P2.front().va;
            apLceiDmaInfo->pa          = mLcsoData4P2.front().pa;
            apLceiDmaInfo->size        = mLcs_outW * mLcs_outH;
            apLceiDmaInfo->bufSecu     = mLcso_memInfo.bufSecu;
            apLceiDmaInfo->bufCohe     = mLcso_memInfo.bufCohe;
            apLceiDmaInfo->useNoncache = mLcso_memInfo.useNoncache;

            if(g_debugDump > 0)
            {
                MY_LOGD_IF(g_debugDump, "match,PA(0x%08x),TS(%lld)",mLcsoData4P2.front().pa,mLcsoData4P2.front().timeStamp);
            }
            mLcsoData4P2.pop();
        }
        else
        {
            apLceiDmaInfo->memID = VHDR_LCSO_SYNC_FAIL;
            MY_LOGE_IF(g_debugDump, "no match");
        }
    }
    else if(mSensorTg == CAM_TG_2)
    {
        if(mLcsoD_Data4P2.empty())
        {
            MY_LOGE_IF(g_debugDump, "mLcsoD_Data4P2 empty)");
            return;
        }

        while(!mLcsoD_Data4P2.empty())
        {
            if(mLcsoD_Data4P2.front().timeStamp < aTimeStamp)
            {
                MY_LOGD_IF(g_debugDump, "dropD,PA(0x%08x),TS(%lld)",mLcsoD_Data4P2.front().pa,mLcsoD_Data4P2.front().timeStamp);
                mLcsoD_Data4P2.pop();
            }
            else if(mLcsoD_Data4P2.front().timeStamp == aTimeStamp)
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
            apLceiDmaInfo->xSize       = mLcsD_outW - 1;
            apLceiDmaInfo->ySize       = mLcsD_outH - 1;
            apLceiDmaInfo->stride      = mLcsD_outW;
            apLceiDmaInfo->memID       = mLcsoD_memInfo.memID;
            apLceiDmaInfo->va          = mLcsoD_Data4P2.front().va;
            apLceiDmaInfo->pa          = mLcsoD_Data4P2.front().pa;
            apLceiDmaInfo->size        = mLcsD_outW * mLcsD_outH;
            apLceiDmaInfo->bufSecu     = mLcsoD_memInfo.bufSecu;
            apLceiDmaInfo->bufCohe     = mLcsoD_memInfo.bufCohe;
            apLceiDmaInfo->useNoncache = mLcsoD_memInfo.useNoncache;

            if(g_debugDump > 0)
            {
                MY_LOGD_IF(g_debugDump, "matchD,PA(0x%08x),TS(%lld)",mLcsoD_Data4P2.front().pa,mLcsoD_Data4P2.front().timeStamp);
            }
            mLcsoD_Data4P2.pop();
        }
        else
        {
            apLceiDmaInfo->memID = VHDR_LCSO_SYNC_FAIL;
            MY_LOGE_IF(g_debugDump, "no match");
        }
    }
    else
    {
        MY_LOGE_IF(g_debugDump, "wrong TG(%u)",mSensorTg);
    }
}

/*******************************************************************************
*
********************************************************************************/
MVOID VHdrDrvImp::GetLCSInputDimen(MUINT32& w, MUINT32& h)
{

    MUINT32 tgW = 0;
    MUINT32 tgH = 0;
    MUINT32 binW = 0;
    MUINT32 binH = 0;

    m_pNormalPipe->sendCommand(ENPipeCmd_GET_TG_OUT_SIZE, (MINTPTR)(&tgW),(MINTPTR)(&tgH), 0);
    m_pNormalPipe->sendCommand(ENPipeCmd_GET_HBIN_INFO, (MINTPTR)(&binW), (MINTPTR)(&binH), 0);

    if( binW < tgW)
        w = binW;
    else
        w = tgW;

    if( binH < tgH)
        h = binH;
    else
        h = tgH;
}

/*******************************************************************************
*
********************************************************************************/
MINT32 VHdrDrvImp::ConfigLcs()
{
    MY_LOGD_IF(g_debugDump, "(dev,tg)=(%u,%u)",mSensorDev,mSensorTg);

    ESensorDev_T tuningSensorDevType = ConvertSensorDevType(mSensorDev);
    ESensorTG_T tuningSensorTgType   = ConvertSensorTgType(mSensorTg);

    MINTPTR handle = -1;

    if(mSensorTg == CAM_TG_1)
    {
        //====== Prepare Register Setting ======

        // CAM_LCS_CON

        MUINT32 lcsCon = 0;

        // === move to Init() (Jou-Feng)
        //mLcs_outW = 0x40;   // 64
        //mLcs_outH = 0x30;   // 48

        lcsCon = lcsCon | (mLcs_outH << 24) | (mLcs_outW << 16) | 0x01;
        MY_LOGD_IF(g_debugDump, "lcsCon(0x%08x)",lcsCon);

        //CAM_LCS_AWS

        MUINT32 lcsInWidth = 0;
        MUINT32 lcsInHeight = 0;
        MUINT32 lcsAws = 0;

        GetLCSInputDimen(lcsInWidth, lcsInHeight);
#if 0
        MUINT32 hbinW = 0;
        MUINT32 hbinH = 0;
        //NormalPipe_HBIN_Info hbinInfo; // no use now

        //> query 2-pixel mode
        // (1) not 2-pixel mode : TG width, TG height
        // (2) 2-pixel mode : TG width/2, TG height

        MY_LOGD_IF(g_debugDump, "mSensorIdx(%u)",mSensorIdx);

        m_pNormalPipe->sendCommand(ENPipeCmd_GET_TG_OUT_SIZE, (MINTPTR)(&lcsInWidth),(MINTPTR)(&lcsInHeight), 0);
        m_pNormalPipe->sendCommand(ENPipeCmd_GET_HBIN_INFO, (MINTPTR)(&hbinW), (MINTPTR)(&hbinH), 0);

        if(hbinW == (lcsInWidth/2))
        {
            MY_LOGD_IF(g_debugDump, "2-pixel mode in w");
            lcsInWidth /= 2;
        }

        if(hbinH == (lcsInHeight/2))
        {
            MY_LOGD_IF(g_debugDump, "2-pixel mode in h");
            lcsInHeight /= 2;  // Jou-Feng Add
        }
#endif

        lcsAws  = lcsAws | (lcsInHeight << 16) | lcsInWidth;
        mLcsAws = lcsAws;

        VHDR_LOG( "lcsInWidth(0x%08x),lcsInHeight(0x%08x)",lcsInWidth,lcsInHeight);
        MY_LOGD_IF(g_debugDump, "lcsAws(0x%08x)",lcsAws);

        //CAM_LCS_FLR

        MUINT32 lcsFlrGain = mFlareGain;        // default value = 0x100 (not use)
        MUINT32 lcsFlrOffset = mFlareOffset;    // default value = 0 (not use)
        MUINT32 lcsFlr = 0;

        lcsFlr = lcsFlr | (lcsFlrGain << 7) | lcsFlrOffset;
        MY_LOGD_IF(g_debugDump, "lcsFlr(0x%08x)",lcsFlr);

        // CAM_LCS_LRZR_1 & CAM_LCS_LRZR_2

        MUINT32 lcsLrzrX = ((2 * mLcs_outW - 2) << 20) / (lcsInWidth - 1);
        MUINT32 lcsLrzrY = ((mLcs_outH - 1) << 20) / (lcsInHeight - 1);

        MY_LOGD_IF(g_debugDump, "lcsLrzrX(0x%08x),lcsLrzrY(0x%08x)",lcsLrzrX,lcsLrzrY);

        //====== Set to LCE ======
#if VHDR_ISP_MGR_READY
        // No no need ?
        //ISP_MGR_LCE_T::getInstance(tuningSensorDevType,tuningSensorTgType).setLcsoSize(mLcs_outW,mLcs_outH);
#endif

        //====== Get Handle ======

#if 0   // use callback to config

        if(MFALSE == m_pNormalPipe->sendCommand(EPIPECmd_GET_MODULE_HANDLE, EModule_LCS,(MINT32)(&handle),(MINT32)(&(VHDR_DRV_NAME))))
        {
            MY_LOGE_IF(g_debugDump, "get LCS handle fail");
            return VHDR_RETURN_API_FAIL;
        }

        //====== Write to NormalPipe ======

        IOPIPE_SET_MODUL_REG(handle,CAM_LCS_CON,lcsCon);
        IOPIPE_SET_MODUL_REG(handle,CAM_LCS_ST,0);
        IOPIPE_SET_MODUL_REG(handle,CAM_LCS_AWS,lcsAws);
        IOPIPE_SET_MODUL_REG(handle,CAM_LCS_FLR,lcsFlr);
        IOPIPE_SET_MODUL_REG(handle,CAM_LCS_LRZR_1,lcsLrzrX);
        IOPIPE_SET_MODUL_REG(handle,CAM_LCS_LRZR_2,lcsLrzrY);

        //====== Configure Done ======

        if(MFALSE == m_pNormalPipe->sendCommand(EPIPECmd_SET_MODULE_CFG_DONE, handle,-1,-1))
        {
            MY_LOGE_IF(g_debugDump, "LCS cofig done fail");
            m_pNormalPipe->sendCommand(EPIPECmd_RELEASE_MODULE_HANDLE,handle,(MINT32)(&(VHDR_DRV_NAME)),-1);
            return VHDR_RETURN_API_FAIL;
        }

        //====== Release Handle ======

        if(MFALSE == m_pNormalPipe->sendCommand(EPIPECmd_RELEASE_MODULE_HANDLE,handle,(MINT32)(&(VHDR_DRV_NAME)),-1))
        {
            MY_LOGE_IF(g_debugDump, "release LCS handle fail");
            return VHDR_RETURN_API_FAIL;
        }
#endif
    }
    else if(mSensorTg == CAM_TG_2)   //RAW_D
    {
        //====== Prepare Register Setting ======

        // CAM_LCS_D_CON

        MUINT32 lcsD_Con = 0;

        // === move to Init() (Jou-Feng)
        //mLcsD_outW = 0x40;   // 64
        //mLcsD_outH = 0x30;   // 48

        lcsD_Con = lcsD_Con | (mLcsD_outH << 24) | (mLcsD_outW << 16) | 0x01;
        MY_LOGD_IF(g_debugDump, "lcsD_Con(0x%08x)",lcsD_Con);

        //CAM_LCS_AWS

        MUINT32 lcsD_InWidth = 0;
        MUINT32 lcsD_InHeight = 0;
        MUINT32 lcsD_Aws = 0;

        GetLCSInputDimen(lcsD_InWidth, lcsD_InHeight);

#if 0
        MUINT32 hbinDW = 0;
        MUINT32 hbinDH = 0;
        //NormalPipe_HBIN_Info hbinInfo; // no use now

        //> query 2-pixel mode
        // (1) not 2-pixel mode : TG width, TG height
        // (2) 2-pixel mode : TG width/2, TG height

        MY_LOGD_IF(g_debugDump, "mSensorIdx(%u)",mSensorIdx);
        m_pNormalPipe->sendCommand(ENPipeCmd_GET_TG_OUT_SIZE, (MINTPTR)(&lcsD_InWidth),(MINTPTR)(&lcsD_InHeight) , 0);
        m_pNormalPipe->sendCommand(ENPipeCmd_GET_HBIN_INFO, (MINTPTR)(&hbinDW), (MINTPTR)(&hbinDH), 0);

        if(hbinDW == (lcsD_InWidth/2))
        {
            MY_LOGD_IF(g_debugDump, "2-pixel mode in w");
            lcsD_InWidth /= 2;
        }

        if(hbinDH == (lcsD_InHeight/2))
        {
            MY_LOGD_IF(g_debugDump, "2-pixel mode in h");
            lcsD_InHeight /= 2;
        }
#endif

        lcsD_Aws  = lcsD_Aws | (lcsD_InHeight << 16) | lcsD_InWidth;
        mLcsD_Aws = lcsD_Aws;

        VHDR_LOG( "lcsD_InWidth(0x%08x),lcsD_InHeight(0x%08x)",lcsD_InWidth,lcsD_InHeight);
        MY_LOGD_IF(g_debugDump, "lcsD_Aws(0x%08x)",lcsD_Aws);

        //CAM_LCS_D_FLR

        MUINT32 lcsD_FlrGain = mFlareGain;      // default value = 0x100 (not use)
        MUINT32 lcsD_FlrOffset = mFlareOffset;  // default value = 0 (not use)
        MUINT32 lcsD_Flr = 0;

        lcsD_Flr = lcsD_Flr | (lcsD_FlrGain << 7) | lcsD_FlrOffset;
        MY_LOGD_IF(g_debugDump, "lcsD_Flr(0x%08x)",lcsD_Flr);

        // CAM_LCS_D_LRZR_1 & CAM_LCS_D_LRZR_2

        MUINT32 lcsD_LrzrX = ((2 * mLcsD_outW - 2) << 20) / (lcsD_InWidth - 1);
        MUINT32 lcsD_LrzrY = ((mLcsD_outH - 1) << 20) / (lcsD_InHeight - 1);

        MY_LOGD_IF(g_debugDump, "lcsD_LrzrX(0x%08x),lcsD_LrzrY(0x%08x)",lcsD_LrzrX,lcsD_LrzrY);

        //====== Set to LCE ======
#if VHDR_ISP_MGR_READY
        // now no need??
        //ISP_MGR_LCE_T::getInstance(tuningSensorDevType,tuningSensorTgType).setLcsoSize(mLcsD_outW,mLcsD_outH);
#endif

        //====== Get Handle ======

#if 0   // use callback to config

        if(MFALSE == m_pNormalPipe->sendCommand(EPIPECmd_GET_MODULE_HANDLE, EModule_LCS_D,(MINT32)(&handle),(MINT32)(&(VHDR_DRV_NAME))))
        {
            MY_LOGE_IF(g_debugDump, "get LCS_D handle fail");
            return VHDR_RETURN_API_FAIL;
        }

        //====== Write to NormalPipe ======

        IOPIPE_SET_MODUL_REG(handle,CAM_LCS_D_CON,lcsD_Con);
        IOPIPE_SET_MODUL_REG(handle,CAM_LCS_D_ST,0);
        IOPIPE_SET_MODUL_REG(handle,CAM_LCS_D_AWS,lcsD_Aws);
        IOPIPE_SET_MODUL_REG(handle,CAM_LCS_D_FLR,lcsD_Flr);
        IOPIPE_SET_MODUL_REG(handle,CAM_LCS_D_LRZR_1,lcsD_LrzrX);
        IOPIPE_SET_MODUL_REG(handle,CAM_LCS_D_LRZR_2,lcsD_LrzrY);

        //====== Configure Done ======

        if(MFALSE == m_pNormalPipe->sendCommand(EPIPECmd_SET_MODULE_CFG_DONE, handle,-1,-1))
        {
            MY_LOGE_IF(g_debugDump, "LCS_D cofig done fail");
            m_pNormalPipe->sendCommand(EPIPECmd_RELEASE_MODULE_HANDLE,handle,(MINT32)(&(VHDR_DRV_NAME)),-1);
            return VHDR_RETURN_API_FAIL;
        }

        //====== Release Handle ======

        if(MFALSE == m_pNormalPipe->sendCommand(EPIPECmd_RELEASE_MODULE_HANDLE,handle,(MINT32)(&(VHDR_DRV_NAME)),-1))
        {
            MY_LOGE_IF(g_debugDump, "release LCS_D handle fail");
            return VHDR_RETURN_API_FAIL;
        }
#endif
    }
    else
    {
        MY_LOGE_IF(g_debugDump, "wrong TG(%u)",mSensorTg);
    }

    // ======== register P1 Tuning Notify callback ==========
    // because this command need to be after MW configPipe, and don't want to add on more API
    // so just put it here
    if( ! (g_disable & VHDR_DISABLE_LCS_LCE) ){
        m_pNormalPipe->sendCommand(ENPipeCmd_SET_LCS_CBFP,(MINTPTR)mpVHdrP1Cb,-1,-1);
    }

    MY_LOGD_IF(g_debugDump, "-");
    return VHDR_RETURN_NO_ERROR;
}


/*******************************************************************************
*
********************************************************************************/
#if 0
MINT32 VHdrDrvImp::GetBufLCSO(android::sp<IImageBuffer>& spBuf)
{
    static MUINT32 currentSliceIndex = 0;

MY_LOGD_IF(g_debugDump, "GetBufLCSO, currentIndex =  %d, buf slice list size = %d", currentSliceIndex, m_vSliceBufs.size());
    if(currentSliceIndex < m_vSliceBufs.size())
    {
        spBuf = m_vSliceBufs[currentSliceIndex];
        currentSliceIndex++;
        if(currentSliceIndex >= m_vSliceBufs.size())
            currentSliceIndex = 0;

        MY_LOGD_IF(g_debugDump, "GetBufLCSO, successfully. Next get index = %d", currentSliceIndex);
        return VHDR_RETURN_NO_ERROR;
    }
    else
    {
        MY_LOGE_IF(g_debugDump, "GetBufLCSO failed. currentIndex = %d", currentSliceIndex);
        return VHDR_RETURN_UNKNOWN_ERROR;
    }

}
#endif

#if 0
/*******************************************************************************
*
********************************************************************************/
MINT32 VHdrDrvImp::CreateMultiMemBuf(MUINT32 memSize, MUINT32 num, android::sp<IImageBuffer>& spMainImageBuf,
                VecSpImgBuf& vecSliceBufs)
{
    MINT32 err = VHDR_RETURN_NO_ERROR;
    MUINT32 totalSize = memSize*num;

    vecSliceBufs.clear();

    IImageBufferAllocator::ImgParam imgParam(totalSize, 0);

    sp<IIonImageBufferHeap> pHeap = IIonImageBufferHeap::create("VHDR_DRV", imgParam);
    if (pHeap == NULL)
    {
        MY_LOGE_IF(g_debugDump, "image buffer heap create fail");
        return VHDR_RETURN_MEMORY_ERROR;
    }

    MUINT const usage = (GRALLOC_USAGE_SW_READ_OFTEN |
                        GRALLOC_USAGE_HW_CAMERA_READ |
                        GRALLOC_USAGE_HW_CAMERA_WRITE);
    spMainImageBuf = pHeap->createImageBuffer();
    if (spMainImageBuf == NULL)
    {
        MY_LOGE_IF(g_debugDump, "mainImage buffer create fail");
        return VHDR_RETURN_MEMORY_ERROR;
    }
    if (!(spMainImageBuf->lockBuf("VHDR_DRV", usage)))
    {
        MY_LOGE_IF(g_debugDump, " image buffer lock fail");
        return VHDR_RETURN_MEMORY_ERROR;
    }
    MUINTPTR const iVAddr = pHeap->getBufVA(0);
    MUINTPTR const iPAddr = pHeap->getBufPA(0);
    MINT32 const iHeapId = pHeap->getHeapID();

    MY_LOGD_IF(g_debugDump, "IIonImageBufferHeap iVAddr:%p, iPAddr:%p, iHeapId:%d\n", iVAddr, iPAddr, iHeapId);
    MY_LOGD_IF(g_debugDump, "spMainImageBuf iVAddr:%p, iPAddr:%p\n",spMainImageBuf->getBufVA(0),spMainImageBuf->getBufPA(0));

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
        sp<IImageBufferHeap> imgBufHeap = IDummyImageBufferHeap::create("VHDR_DRV", imgParam_t, portBufInfo, false);
        if (imgBufHeap == NULL)
        {
            MY_LOGE_IF(g_debugDump, "acquire VHDR_DRV - image buffer heap create fail");
            return VHDR_RETURN_MEMORY_ERROR;
        }

        sp<IImageBuffer> imgBuf = imgBufHeap->createImageBuffer();
        if (imgBuf == NULL)
        {
            MY_LOGE_IF(g_debugDump, "acquire VHDR_DRV - image buffer create fail");
            return VHDR_RETURN_MEMORY_ERROR;
        }

        if (!(imgBuf->lockBuf("VHDR_DRV", usage)))
        {
            MY_LOGE_IF(g_debugDump, "acquire VHDR_DRV - image buffer lock fail");
            return VHDR_RETURN_MEMORY_ERROR;
        }

        vecSliceBufs.push_back(imgBuf);
    }
    MY_LOGD_IF(g_debugDump, "X");
    return err;
}
/*******************************************************************************
*
********************************************************************************/
MINT32 VHdrDrvImp::DestroMultiMemBuf(android::sp<IImageBuffer>& spMainImageBuf, VecSpImgBuf& vecSliceBufs)
{
    MUINT32 index;
    for (index = 0; index < vecSliceBufs.size(); index++)
    {
        vecSliceBufs[index]->unlockBuf("VHDR_DRV");
        vecSliceBufs[index] = NULL;
    }
    vecSliceBufs.clear();

    spMainImageBuf->unlockBuf("VHDR_DRV");
    spMainImageBuf = NULL;

    MY_LOGD_IF(g_debugDump, "X");

    return VHDR_RETURN_NO_ERROR;
}

#endif
/*******************************************************************************
*
********************************************************************************/
MINT32 VHdrDrvImp::ConfigLcso()
{
    MY_LOGD_IF(g_debugDump, "(dev,tg)=(%u,%u)",mSensorDev,mSensorTg);

    ESensorDev_T tuningSensorDevType = ConvertSensorDevType(mSensorDev);
    ESensorTG_T tuningSensorTgType   = ConvertSensorTgType(mSensorTg);
    MINTPTR handle = -1;

    if(mSensorTg == CAM_TG_1)   //RAW
    {
        //====== Create Memory for LCSO and LCEI ======
#if VHDR_RUN_LEGACY
        MUINT32 lcs_memSize = mLcs_outW * mLcs_outH * LCSO_BUFFER_NUM;

        MY_LOGD_IF(g_debugDump, "mLcs_outW(%u),mLcs_outH(%u),lcs_memSize(%u)",mLcs_outW,mLcs_outH,lcs_memSize);

        CreateMemBuf(lcs_memSize,1,&mLcso_memInfo);
        if(mLcso_memInfo.virtAddr == 0 && mLcso_memInfo.phyAddr == 0)
        {
            MY_LOGE_IF(g_debugDump, "lcs create IMem fail");
            return VHDR_RETURN_API_FAIL;
        }

        mLcso_addrIdx = 0;
        mLcso_virAddr = mLcso_memInfo.virtAddr;
        mLcso_phyAddr = mLcso_memInfo.phyAddr;

        MY_LOGD_IF(g_debugDump, "LcsoMem:ID(%d),SZ(%u),VA(0x%08x),PA(0x%08x),cache(%d)",mLcso_memInfo.memID,
                                                                         mLcso_memInfo.size,
                                                                         mLcso_memInfo.virtAddr,
                                                                         mLcso_memInfo.phyAddr,
                                                                         mLcso_memInfo.useNoncache);

        //====== Set to LCE ======

        ISP_MGR_LCE_T::getInstance(tuningSensorDevType,tuningSensorTgType).setLceiAddr(mLcso_phyAddr);  // only for initial value

        //====== Prepare Register Setting ======

        MUINT32 lcso_xSize = mLcs_outW - 1;
        MUINT32 lcso_ySize = mLcs_outH - 1;
        MUINT32 lcso_con  = 0x80202020;  //default value = 0x80202020
        MUINT32 lcso_con2 = 0x202000;    //default value = 0x202000

        MY_LOGD_IF(g_debugDump, "lcso_xSize(0x%08x),lcso_ySize(0x%08x)",lcso_xSize,lcso_ySize);

        //====== Get Handle ======

        if(MFALSE == m_pNormalPipe->sendCommand(EPIPECmd_GET_MODULE_HANDLE, EModule_LCSO,(MINTPTR)(&handle),(MINTPTR)(&(VHDR_DRV_NAME))))
        {
            MY_LOGE_IF(g_debugDump, "get LCSO handle fail");
            return VHDR_RETURN_API_FAIL;
        }

        //====== Write to NormalPipe =======

        IOPIPE_SET_MODUL_REG(handle,CAM_LCSO_BASE_ADDR,mLcso_phyAddr);
        IOPIPE_SET_MODUL_REG(handle,CAM_LCSO_OFST_ADDR,0);
        IOPIPE_SET_MODUL_REG(handle,CAM_LCSO_XSIZE,lcso_xSize);
        IOPIPE_SET_MODUL_REG(handle,CAM_LCSO_YSIZE,lcso_ySize);
        IOPIPE_SET_MODUL_REG(handle,CAM_LCSO_STRIDE,mLcs_outW);
        IOPIPE_SET_MODUL_REG(handle,CAM_LCSO_CON,lcso_con);
        IOPIPE_SET_MODUL_REG(handle,CAM_LCSO_CON2,lcso_con2);

        //====== Configure Done ======

        if(MFALSE == m_pNormalPipe->sendCommand(EPIPECmd_SET_MODULE_CFG_DONE, handle,-1,-1))
        {
            MY_LOGE_IF(g_debugDump, "LCSO cofig done fail");
            m_pNormalPipe->sendCommand(EPIPECmd_RELEASE_MODULE_HANDLE,handle,(MINTPTR)(&(VHDR_DRV_NAME)),-1);
            return VHDR_RETURN_API_FAIL;
        }

        //====== Release Handle ======

        if(MFALSE == m_pNormalPipe->sendCommand(EPIPECmd_RELEASE_MODULE_HANDLE,handle,(MINTPTR)(&(VHDR_DRV_NAME)),-1))
        {
            MY_LOGE_IF(g_debugDump, "release LCSO handle fail");
            return VHDR_RETURN_API_FAIL;
        }
#else
        // ============== Create IImageBuffer for LCSO ============
        //CreateLcsoBuffers(mLcs_outW, mLcs_outH, LCSO_BUFFER_NUM, m_vSliceBufs);  // move to Init
        // Config operation will be done in P1Node hardwareOps_start
#endif
    }
    else if(mSensorTg == CAM_TG_2)
    {
        //====== Create Memory for LCSO and LCEI ======
#if VHDR_RUN_LEGACY
        MUINT32 lcsD_memSize = mLcsD_outW * mLcsD_outH * LCSO_BUFFER_NUM;

        MY_LOGD_IF(g_debugDump, "mLcsD_outW(%u),mLcsD_outH(%u),lcsD_memSize(%u)",mLcsD_outW,mLcsD_outH,lcsD_memSize);

        CreateMemBuf(lcsD_memSize,1,&mLcsoD_memInfo);
        if(mLcsoD_memInfo.virtAddr == 0 && mLcsoD_memInfo.phyAddr == 0)
        {
            MY_LOGE_IF(g_debugDump, "lcsD create IMem fail");
            return VHDR_RETURN_API_FAIL;
        }

        mLcsoD_addrIdx = 0;
        mLcsoD_virAddr = mLcsoD_memInfo.virtAddr;
        mLcsoD_phyAddr = mLcsoD_memInfo.phyAddr;

        MY_LOGD_IF(g_debugDump, "LcsoD_Mem:ID(%d),SZ(%u),VA(0x%08x),PA(0x%08x),cache(%d)",mLcsoD_memInfo.memID,
                                                                           mLcsoD_memInfo.size,
                                                                           mLcsoD_memInfo.virtAddr,
                                                                           mLcsoD_memInfo.phyAddr,
                                                                           mLcsoD_memInfo.useNoncache);

        //====== Set to LCE ======

        ISP_MGR_LCE_T::getInstance(tuningSensorDevType,tuningSensorTgType).setLceiAddr(mLcsoD_phyAddr); // only for initial value


        //====== Prepare Register Setting ======

        MUINT32 lcsoD_xSize = mLcsD_outW - 1;
        MUINT32 lcsoD_ySize = mLcsD_outH - 1;
        MUINT32 lcsoD_con  = 0x80202020;  //default value = 0x80202020
        MUINT32 lcsoD_con2 = 0x202000;    //default value = 0x202000

        MY_LOGD_IF(g_debugDump, "lcsoD_xSize(0x%08x),lcsoD_ySize(0x%08x)",lcsoD_xSize,lcsoD_ySize);

        //====== Get Handle ======

        if(MFALSE == m_pNormalPipe->sendCommand(EPIPECmd_GET_MODULE_HANDLE, EModule_LCSO_D,(MINTPTR)(&handle),(MINTPTR)(&(VHDR_DRV_NAME))))
        {
            MY_LOGE_IF(g_debugDump, "get LCSO_D handle fail");
            return VHDR_RETURN_API_FAIL;
        }

        //====== Write to NormalPipe =======

        IOPIPE_SET_MODUL_REG(handle,CAM_LCSO_D_BASE_ADDR,mLcsoD_phyAddr);
        IOPIPE_SET_MODUL_REG(handle,CAM_LCSO_D_OFST_ADDR,0);
        IOPIPE_SET_MODUL_REG(handle,CAM_LCSO_D_XSIZE,lcsoD_xSize);
        IOPIPE_SET_MODUL_REG(handle,CAM_LCSO_D_YSIZE,lcsoD_ySize);
        IOPIPE_SET_MODUL_REG(handle,CAM_LCSO_D_STRIDE,mLcsD_outW);
        IOPIPE_SET_MODUL_REG(handle,CAM_LCSO_D_CON,lcsoD_con);
        IOPIPE_SET_MODUL_REG(handle,CAM_LCSO_D_CON2,lcsoD_con2);

        //====== Configure Done ======

        if(MFALSE == m_pNormalPipe->sendCommand(EPIPECmd_SET_MODULE_CFG_DONE, handle,-1,-1))
        {
            MY_LOGE_IF(g_debugDump, "LCSO_D cofig done fail");
            m_pNormalPipe->sendCommand(EPIPECmd_RELEASE_MODULE_HANDLE,handle,(MINTPTR)(&(VHDR_DRV_NAME)),-1);
            return VHDR_RETURN_API_FAIL;
        }

        //====== Release Handle ======

        if(MFALSE == m_pNormalPipe->sendCommand(EPIPECmd_RELEASE_MODULE_HANDLE,handle,(MINTPTR)(&(VHDR_DRV_NAME)),-1))
        {
            MY_LOGE_IF(g_debugDump, "release LCSO_D handle fail");
            return VHDR_RETURN_API_FAIL;
        }
#else
        // ============== Create IImageBuffer for LCSO ============
        //CreateLcsoBuffers(mLcsD_outW, mLcsD_outH, LCSO_BUFFER_NUM, m_vSliceBufs); //Move to Init (Jou-Feng modify)
        // Config operation will be done in P1Node hardwareOps_start
#endif
    }
    else
    {
        MY_LOGE_IF(g_debugDump, "wrong TG(%u)",mSensorTg);
    }

    MY_LOGD_IF(g_debugDump, "-");

    return VHDR_RETURN_NO_ERROR;
}

/*******************************************************************************
*
********************************************************************************/
MVOID VHdrDrvImp::ConfigRmg_ivHDR(const MUINT32 &aLeFirst)
{
    // 0 : first two rows are SE,  1 : first two rows are LE
    VHDR_INF( "(leFirst,dev,tg)=(%u,%u,%u)",aLeFirst,mSensorDev,mSensorTg);

    ESensorDev_T tuningSensorDevType = ConvertSensorDevType(mSensorDev);
    ESensorTG_T tuningSensorTgType   = ConvertSensorTgType(mSensorTg);

//#if VHDR_ISP_MGR_READY
    ISP_MGR_RMG_T::getInstance(tuningSensorDevType).setIHdrEnable(MTRUE);
    ISP_MGR_RMG_T::getInstance(tuningSensorDevType).setLeFirst(aLeFirst);
//#endif

}

/*******************************************************************************
*
********************************************************************************/
MVOID VHdrDrvImp::ConfigRmg_zvHDR(const MUINT32 &zPattern)
{
    // 0 : first two rows are SE,  1 : first two rows are LE
    VHDR_INF( "(zPattern,dev,tg)=(%u,%u,%u)",zPattern,mSensorDev,mSensorTg);

    ESensorDev_T tuningSensorDevType = ConvertSensorDevType(mSensorDev);
    ESensorTG_T tuningSensorTgType   = ConvertSensorTgType(mSensorTg);

#if VHDR_ISP_MGR_READY
    ISP_MGR_RMG_T::getInstance(tuningSensorDevType).setZHdrEnable(MTRUE);
    ISP_MGR_RMG_T::getInstance(tuningSensorDevType).setZPattern(zPattern);
#endif

}

/*******************************************************************************
*
********************************************************************************/
MINT32 VHdrDrvImp::UpdateLcs()
{
// use callback to config
#if 0

    MY_LOGD_IF(g_debugDump, "tg(%u)",mSensorTg);

    MINT32 handle = -1;

    if(mSensorTg == CAM_TG_1)
    {
        //====== Update Flare ======

        // CAM_LCS_FLR

        MUINT32 lcsFlrGain = mFlareGain;
        MUINT32 lcsFlrOffset = mFlareOffset;
        MUINT32 lcsFlr = 0;

        lcsFlr = lcsFlr | (lcsFlrGain << 7) | lcsFlrOffset;
        MY_LOGD_IF(g_debugDump, "lcsFlr(0x%08x)",lcsFlr);

        //====== Get Handle ======

        if(MFALSE == m_pNormalPipe->sendCommand(EPIPECmd_GET_MODULE_HANDLE, EModule_LCS,(MINT32)(&handle),(MINT32)(&(VHDR_DRV_NAME))))
        {
            MY_LOGE_IF(g_debugDump, "get LCS handle fail");
            return VHDR_RETURN_API_FAIL;
        }

        //====== Write to NormalPipe ======

        IOPIPE_SET_MODUL_REG(handle,CAM_LCS_FLR,lcsFlr);

        //====== Configure Done ======

        if(MFALSE == m_pNormalPipe->sendCommand(EPIPECmd_SET_MODULE_CFG_DONE, handle,-1,-1))
        {
            MY_LOGE_IF(g_debugDump, "LCS flare cofig done fail");
            m_pNormalPipe->sendCommand(EPIPECmd_RELEASE_MODULE_HANDLE,handle,(MINT32)(&(VHDR_DRV_NAME)),-1);
            return VHDR_RETURN_API_FAIL;
        }

        //====== Release Handle ======

        if(MFALSE == m_pNormalPipe->sendCommand(EPIPECmd_RELEASE_MODULE_HANDLE,handle,(MINT32)(&(VHDR_DRV_NAME)),-1))
        {
            MY_LOGE_IF(g_debugDump, "release LCS handle fail");
            return VHDR_RETURN_API_FAIL;
        }
    }
    else if(mSensorTg == CAM_TG_2)
    {
        //====== Update Flare ======

        // CAM_LCS_D_FLR

        MUINT32 lcsD_FlrGain = mFlareGain;      // default value = 0x100 (not use)
        MUINT32 lcsD_FlrOffset = mFlareOffset;  // default value = 0 (not use)
        MUINT32 lcsD_Flr = 0;

        lcsD_Flr = lcsD_Flr | (lcsD_FlrGain << 7) | lcsD_FlrOffset;
        MY_LOGD_IF(g_debugDump, "lcsD_Flr(0x%08x)",lcsD_Flr);

        //====== Get Handle ======

        if(MFALSE == m_pNormalPipe->sendCommand(EPIPECmd_GET_MODULE_HANDLE, EModule_LCS_D,(MINT32)(&handle),(MINT32)(&(VHDR_DRV_NAME))))
        {
            MY_LOGE_IF(g_debugDump, "get LCS_D handle fail");
            return VHDR_RETURN_API_FAIL;
        }

        //====== Write to NormalPipe ======

        IOPIPE_SET_MODUL_REG(handle,CAM_LCS_D_FLR,lcsD_Flr);

        //====== Configure Done ======

        if(MFALSE == m_pNormalPipe->sendCommand(EPIPECmd_SET_MODULE_CFG_DONE, handle,-1,-1))
        {
            MY_LOGE_IF(g_debugDump, "LCS_D flare cofig done fail");
            m_pNormalPipe->sendCommand(EPIPECmd_RELEASE_MODULE_HANDLE,handle,(MINT32)(&(VHDR_DRV_NAME)),-1);
            return VHDR_RETURN_API_FAIL;
        }

        //====== Release Handle ======

        if(MFALSE == m_pNormalPipe->sendCommand(EPIPECmd_RELEASE_MODULE_HANDLE,handle,(MINT32)(&(VHDR_DRV_NAME)),-1))
        {
            MY_LOGE_IF(g_debugDump, "release LCS_D handle fail");
            return VHDR_RETURN_API_FAIL;
        }
    }
    else
    {
        MY_LOGE_IF(g_debugDump, "wrong TG(%u)",mSensorTg);
    }
#endif
    return VHDR_RETURN_NO_ERROR;
}

/*******************************************************************************
*
********************************************************************************/
MINT32 VHdrDrvImp::UpdateLcso()
{
#if VHDR_RUN_LEGACY
    if(VHDR_STATE_UNINIT == GetVHdrState())
    {
        ::sem_post(&mUninitSem);
        MY_LOGD_IF(g_debugDump, "post mUninitSem");
        return VHDR_RETURN_NO_ERROR;
    }

    MINTPTR handle = -1;

    //====== Wait Vsync ======

    ISP_DRV_WAIT_IRQ_STRUCT waitIrq;

    waitIrq.Clear      = ISP_DRV_IRQ_CLEAR_NONE;
    waitIrq.Type       = (mSensorTg == CAM_TG_1) ? ISP_DRV_IRQ_TYPE_INT_P1_ST : ISP_DRV_IRQ_TYPE_INT_P1_ST_D;
    waitIrq.Status     = (mSensorTg == CAM_TG_1) ? CAM_CTL_INT_P1_STATUS_VS1_INT_ST : CAM_CTL_INT_P1_STATUS_D_VS1_INT_ST;
    waitIrq.Timeout    = 513;
    waitIrq.UserNumber = ISP_DRV_IRQ_USER_VHDR;
    waitIrq.UserName   = const_cast<char *>("VHDR");

    if(g_debugDump > 0)
    {
        MY_LOGD_IF(g_debugDump, "wait Vsync");
    }

    if(MTRUE != m_pISPDrvObj->waitIrq(&waitIrq))
    {
        if(VHDR_STATE_UNINIT != GetVHdrState())
        {
            MY_LOGE_IF(g_debugDump, "wait Vsync fail,state(%d)",mState);
            MY_LOGE_IF(g_debugDump, "TG(%d),Clear(%d),Type(%d),Status(%d),Timeout(%u),UserNumber(%d)",mSensorTg,
                                                                                       waitIrq.Clear,
                                                                                       waitIrq.Type,
                                                                                       waitIrq.Status,
                                                                                       waitIrq.Timeout,
                                                                                       waitIrq.UserNumber);

            return VHDR_RETURN_API_FAIL;
        }
        else
        {
            MY_LOGD_IF(g_debugDump, "state(%d)",GetVHdrState());
            return VHDR_RETURN_NO_ERROR;
        }
    }

    Mutex::Autolock lock(mIdxLock);

    if(mSensorTg == CAM_TG_1)
    {
        MY_LOGD_IF(g_debugDump, "Lcso:idx(%u),PA(0x%08x)",mLcso_addrIdx,mLcso_phyAddr);

        //====== Get Handle ======

        if(MFALSE == m_pNormalPipe->sendCommand(EPIPECmd_GET_MODULE_HANDLE, EModule_LCSO,(MINTPTR)(&handle),(MINTPTR)(&(VHDR_DRV_NAME))))
        {
            MY_LOGE_IF(g_debugDump, "get LCSO handle fail");
            return VHDR_RETURN_API_FAIL;
        }

        //====== Write to NormalPipe =======

        IOPIPE_SET_MODUL_REG(handle,CAM_LCSO_BASE_ADDR,mLcso_phyAddr);

        //====== Configure Done ======

        if(MFALSE == m_pNormalPipe->sendCommand(EPIPECmd_SET_MODULE_CFG_DONE, handle,-1,-1))
        {
            MY_LOGE_IF(g_debugDump, "LCSO cofig done fail");
            m_pNormalPipe->sendCommand(EPIPECmd_RELEASE_MODULE_HANDLE,handle,(MINTPTR)(&(VHDR_DRV_NAME)),-1);
            return VHDR_RETURN_API_FAIL;
        }

        //====== Release Handle ======

        if(MFALSE == m_pNormalPipe->sendCommand(EPIPECmd_RELEASE_MODULE_HANDLE,handle,(MINTPTR)(&(VHDR_DRV_NAME)),-1))
        {
            MY_LOGE_IF(g_debugDump, "release LCSO handle fail");
            return VHDR_RETURN_API_FAIL;
        }
    }
    else if(mSensorTg == CAM_TG_2)
    {
        MY_LOGD_IF(g_debugDump, "LcsoD:idx(%u),PA(0x%08x)",mLcsoD_addrIdx,mLcsoD_phyAddr);

        //====== Get Handle ======

        if(MFALSE == m_pNormalPipe->sendCommand(EPIPECmd_GET_MODULE_HANDLE, EModule_LCSO_D,(MINTPTR)(&handle),(MINTPTR)(&(VHDR_DRV_NAME))))
        {
            MY_LOGE_IF(g_debugDump, "get LCSO_D handle fail");
            return VHDR_RETURN_API_FAIL;
        }

        //====== Write to NormalPipe =======

        IOPIPE_SET_MODUL_REG(handle,CAM_LCSO_D_BASE_ADDR,mLcsoD_phyAddr);

        //====== Configure Done ======

        if(MFALSE == m_pNormalPipe->sendCommand(EPIPECmd_SET_MODULE_CFG_DONE, handle,-1,-1))
        {
            MY_LOGE_IF(g_debugDump, "LCSO_D cofig done fail");
            m_pNormalPipe->sendCommand(EPIPECmd_RELEASE_MODULE_HANDLE,handle,(MINTPTR)(&(VHDR_DRV_NAME)),-1);
            return VHDR_RETURN_API_FAIL;
        }

        //====== Release Handle ======

        if(MFALSE == m_pNormalPipe->sendCommand(EPIPECmd_RELEASE_MODULE_HANDLE,handle,(MINTPTR)(&(VHDR_DRV_NAME)),-1))
        {
            MY_LOGE_IF(g_debugDump, "release LCSO_D handle fail");
            return VHDR_RETURN_API_FAIL;
        }
    }
    else
    {
        MY_LOGE_IF(g_debugDump, "wrong TG(%d)",mSensorTg);
        return VHDR_RETURN_INVALID_PARA;
    }

    if(VHDR_STATE_UNINIT == GetVHdrState())
    {
        ::sem_post(&mUninitSem);
        MY_LOGD_IF(g_debugDump, "post mUninitSem");
    }

#endif
    return VHDR_RETURN_NO_ERROR;
}

/*******************************************************************************
*
********************************************************************************/
MINT32 VHdrDrvImp::UpdateLcsoIdx()
{
#if VHDR_RUN_LEGACY
    static MINT32 isFirst = 1;

    if(VHDR_STATE_UNINIT == GetVHdrState())
    {
        ::sem_post(&mUninitSem);
        MY_LOGD_IF(g_debugDump, "post mUninitSem");
        return VHDR_RETURN_NO_ERROR;
    }

    //====== Wait Pass1 Done ======

    ISP_DRV_WAIT_IRQ_STRUCT waitIrq;

    waitIrq.Clear      = ISP_DRV_IRQ_CLEAR_NONE;
    waitIrq.Type       = (mSensorTg == CAM_TG_1) ? ISP_DRV_IRQ_TYPE_INT_P1_ST : ISP_DRV_IRQ_TYPE_INT_P1_ST_D;
    waitIrq.Status     = (mSensorTg == CAM_TG_1) ? CAM_CTL_INT_P1_STATUS_PASS1_DON_ST : CAM_CTL_INT_P1_STATUS_D_PASS1_DON_ST;
    waitIrq.Timeout    = 511;
    waitIrq.UserNumber = ISP_DRV_IRQ_USER_VHDR;
    waitIrq.UserName   = const_cast<char *>("VHDR");

    if(g_debugDump > 0)
    {
        MY_LOGD_IF(g_debugDump, "wait P1 done");
    }

    if(MTRUE != m_pISPDrvObj->waitIrq(&waitIrq))
    {
        if(VHDR_STATE_UNINIT != GetVHdrState())
        {
            MY_LOGE_IF(g_debugDump, "wait P1 done fail,state(%d)",GetVHdrState());
            MY_LOGE_IF(g_debugDump, "TG(%d),Clear(%d),Type(%d),Status(%d),Timeout(%u),UserNumber(%d)",mSensorTg,
                                                                                       waitIrq.Clear,
                                                                                       waitIrq.Type,
                                                                                       waitIrq.Status,
                                                                                       waitIrq.Timeout,
                                                                                       waitIrq.UserNumber);

            MY_LOGD_IF(g_debugDump, "[miss]post mTsSem");
        }
        else
        {
            MY_LOGD_IF(g_debugDump, "[miss]post mTsSem,state(%d)",GetVHdrState());
        }


        if(mSensorTg == CAM_TG_1)
        {
            LCSO_DATA lcsoData;

            {
                Mutex::Autolock lock(mQueLock);

                if(isFirst == 1)
                {
                    lcsoData.va = mLcso_virAddr;
                    lcsoData.pa = mLcso_phyAddr;
                }
                else
                {
                    MUINT32 tempLcsoIdx = (mLcso_addrIdx == 0) ? LCSO_BUFFER_NUM-1 : mLcso_addrIdx-1;

                    lcsoData.va = mLcso_memInfo.virtAddr + (mLcs_outW * mLcs_outH * tempLcsoIdx);
                    lcsoData.pa = mLcso_memInfo.phyAddr  + (mLcs_outW * mLcs_outH * tempLcsoIdx);
                }

                lcsoData.timeStamp = -1;

                mLcsoData.push(lcsoData);
            }
        }
        else if(mSensorTg == CAM_TG_2)
        {
            LCSO_DATA lcsoD_Data;

            {
                Mutex::Autolock lock(mQueLock);

                if(isFirst == 1)
                {
                    lcsoD_Data.va = mLcsoD_virAddr;
                    lcsoD_Data.pa = mLcsoD_phyAddr;
                }
                else
                {
                    MUINT32 tempLcsoDIdx = (mLcsoD_addrIdx == 0) ? LCSO_BUFFER_NUM-1 : mLcsoD_addrIdx-1;

                    lcsoD_Data.va = mLcsoD_memInfo.virtAddr + (mLcsD_outW * mLcsD_outH * tempLcsoDIdx);
                    lcsoD_Data.pa = mLcsoD_memInfo.phyAddr  + (mLcsD_outW * mLcsD_outH * tempLcsoDIdx);
                }

                lcsoD_Data.timeStamp = -1;

                mLcsoD_Data.push(lcsoD_Data);
            }
        }

        ::sem_post(&mTsSem);
        return VHDR_RETURN_NO_ERROR;
    }

    Mutex::Autolock lock(mIdxLock);

    if(mSensorTg == CAM_TG_1)
    {
        //====== DUMP ======

        if(gLcsoDebugDump == 1 || gLcsoDebugDump == 3)
        {
            DumpLcso("Lcso",mLcso_virAddr);
        }

        LCSO_DATA lcsoData;

        //====== Save Ready LCSO PA ======

        {
            Mutex::Autolock lock(mQueLock);

            if(isFirst == 1)
            {
                lcsoData.va = mLcso_virAddr;
                lcsoData.pa = mLcso_phyAddr;
                isFirst = 0;
            }
            else
            {
                MUINT32 tempLcsoIdx = (mLcso_addrIdx == 0) ? LCSO_BUFFER_NUM-1 : mLcso_addrIdx-1;

                lcsoData.va = mLcso_memInfo.virtAddr + (mLcs_outW * mLcs_outH * tempLcsoIdx);
                lcsoData.pa = mLcso_memInfo.phyAddr  + (mLcs_outW * mLcs_outH * tempLcsoIdx);
            }

            lcsoData.timeStamp = -1;

            mLcsoData.push(lcsoData);

            //====== Update LCSO Phy Addr ======

            mLcso_addrIdx++;

            if(mLcso_addrIdx == LCSO_BUFFER_NUM)
            {
                mLcso_addrIdx = 0;
            }

            mLcso_virAddr = mLcso_memInfo.virtAddr + (mLcs_outW * mLcs_outH * mLcso_addrIdx);
            mLcso_phyAddr = mLcso_memInfo.phyAddr  + (mLcs_outW * mLcs_outH * mLcso_addrIdx);

            MY_LOGD_IF(g_debugDump, "Lcso:idx(%u->%u),savePA(0x%08x),NEXT(va,pa)=(0x%08x,0x%08x)",(mLcso_addrIdx == 0)?LCSO_BUFFER_NUM-1:mLcso_addrIdx-1,
                                                                                    mLcso_addrIdx,
                                                                                    mLcsoData.back().pa,
                                                                                    mLcso_virAddr,
                                                                                    mLcso_phyAddr);
        }

    }
    else if(mSensorTg == CAM_TG_2)
    {
        //====== DUMP ======

        if(gLcsoDebugDump == 1 || gLcsoDebugDump == 3)
        {
            DumpLcso("LcsoD",mLcsoD_virAddr);
        }

        LCSO_DATA lcsoD_Data;

        //====== Save Ready LCSOD PA ======

        {
            Mutex::Autolock lock(mQueLock);

            if(isFirst == 1)
            {
                lcsoD_Data.va = mLcsoD_virAddr;
                lcsoD_Data.pa = mLcsoD_phyAddr;
                isFirst = 0;
            }
            else
            {
                MUINT32 tempLcsoDIdx = (mLcsoD_addrIdx == 0) ? LCSO_BUFFER_NUM-1 : mLcsoD_addrIdx-1;

                lcsoD_Data.va = mLcsoD_memInfo.virtAddr + (mLcsD_outW * mLcsD_outH * tempLcsoDIdx);
                lcsoD_Data.pa = mLcsoD_memInfo.phyAddr  + (mLcsD_outW * mLcsD_outH * tempLcsoDIdx);
    }

            lcsoD_Data.timeStamp = -1;

            mLcsoD_Data.push(lcsoD_Data);

            //====== Update LCSOD Phy Addr ======

            mLcsoD_addrIdx++;

            if(mLcsoD_addrIdx == LCSO_BUFFER_NUM)
            {
                mLcsoD_addrIdx = 0;
            }

            mLcsoD_virAddr = mLcsoD_memInfo.virtAddr + (mLcsD_outW * mLcsD_outH * mLcsoD_addrIdx);
            mLcsoD_phyAddr = mLcsoD_memInfo.phyAddr  + (mLcsD_outW * mLcsD_outH * mLcsoD_addrIdx);

            MY_LOGD_IF(g_debugDump, "LcsoD:idx(%u->%u),savePA(0x%08x),NEXT(va,pa)=(0x%08x,0x%08x)",(mLcsoD_addrIdx == 0)?LCSO_BUFFER_NUM-1:mLcsoD_addrIdx-1,
                                                                                    mLcsoD_addrIdx,
                                                                                    mLcsoD_Data.back().pa,
                                                                                    mLcsoD_virAddr,
                                                                                    mLcsoD_phyAddr);
        }
    }
    else
    {
        MY_LOGE_IF(g_debugDump, "wrong TG(%d)",mSensorTg);
        return VHDR_RETURN_INVALID_PARA;
    }

    MY_LOGD_IF(g_debugDump, "post mTsSem");
    ::sem_post(&mTsSem);

    if(VHDR_STATE_UNINIT == GetVHdrState())
    {
        ::sem_post(&mUninitSem);
        MY_LOGD_IF(g_debugDump, "post mUninitSem");
    }
#endif
    return VHDR_RETURN_NO_ERROR;
}

/*******************************************************************************
*
********************************************************************************/
MVOID VHdrDrvImp::UpdateLcsoTimeStamp(const MINT64 &aTimeStamp)
{
    //====== Wait Semaphore ======
#if VHDR_RUN_LEGACY
    MY_LOGD_IF(g_debugDump, "wait mTsSem");
    ::sem_wait(&mTsSem);
    if(g_debugDump > 0)
    {
        MY_LOGD_IF(g_debugDump, "got mTsSem");
    }

    //====== Update Time Stamp ======

    Mutex::Autolock lock(mQueLock);

    MY_LOGD_IF(g_debugDump, "TG(%u),TS(%lld)",mSensorTg,aTimeStamp);

    if(mSensorTg == CAM_TG_1)
    {
        if(mLcsoData.empty())
    {
            MY_LOGE_IF(g_debugDump, "mLcsoData is empty");
        }

        if(aTimeStamp == 0)
        {
            if(g_debugDump > 0)
            {
                MY_LOGD_IF(g_debugDump, "dropPA(0x%08x)",mLcsoData.front().pa);
            }
            mLcsoData.pop();
        }
        else
        {
            LCSO_DATA lcsoData4P2;

            lcsoData4P2.va = mLcsoData.front().va;
            lcsoData4P2.pa = mLcsoData.front().pa;
            lcsoData4P2.timeStamp = aTimeStamp;

            mLcsoData4P2.push(lcsoData4P2);
            mLcsoData.pop();

            MY_LOGD_IF(g_debugDump, "enqueP2,PA(0x%08x),TS(%lld)",mLcsoData4P2.back().pa,mLcsoData4P2.back().timeStamp);
        }
    }
    else if(mSensorTg == CAM_TG_2)
    {
        if(mLcsoD_Data.empty())
        {
            MY_LOGE_IF(g_debugDump, "mLcsoD_Data is empty");
        }

        if(aTimeStamp == 0)
        {
            if(g_debugDump > 0)
            {
                MY_LOGD_IF(g_debugDump, "dropD_PA(0x%08x)",mLcsoD_Data.front().pa);
            }
            mLcsoD_Data.pop();
            }
        else
        {
            LCSO_DATA lcsoD_Data4P2;

            lcsoD_Data4P2.va = mLcsoD_Data.front().va;
            lcsoD_Data4P2.pa = mLcsoD_Data.front().pa;
            lcsoD_Data4P2.timeStamp = aTimeStamp;

            mLcsoD_Data4P2.push(lcsoD_Data4P2);
            mLcsoD_Data.pop();

            MY_LOGD_IF(g_debugDump, "enqueD_P2,PA(0x%08x),TS(%lld)",mLcsoD_Data4P2.back().pa,mLcsoD_Data4P2.back().timeStamp);
        }
    }
    else
        {
        MY_LOGE_IF(g_debugDump, "wrong TG(%d)",mSensorTg);
    }
#endif
}

/*******************************************************************************
*
********************************************************************************/
MINT32 VHdrDrvImp::EnableLcs(const MBOOL &aEn)
{
    MY_LOGD_IF(g_debugDump, "(en,TG)=(%d,%u)",aEn,mSensorTg);

    if(mSensorTg == CAM_TG_1)
    {
        // LCS_EN
    #if 0   // use callback to enable
        if(MFALSE == m_pNormalPipe->sendCommand(ENPipeCmd_SET_MODULE_EN, EModule_LCS,aEn, -1))
            {
            MY_LOGE_IF(g_debugDump, "LCS_EN fail");
            return VHDR_RETURN_API_FAIL;
            }
    #else
        mLcsEn = aEn;
    #endif
#if VHDR_RUN_LEGACY
        // LCSO_EN  (Now no need to enable LCSO ?? )
        if(MFALSE == m_pNormalPipe->sendCommand(ENPipeCmd_SET_MODULE_EN, EModule_LCSO,aEn, -1))
        {
            MY_LOGE_IF(g_debugDump, "LCSO_EN fail");
            return VHDR_RETURN_API_FAIL;
        }


        // LCS_SEL_EN
        if(MFALSE == m_pNormalPipe->sendCommand(ENPipeCmd_SET_MODULE_SEL, EPipe_P1Sel_LCS_EN,aEn,-1))
        {
            MY_LOGE_IF(g_debugDump, "LCS_SEL_EN fail");
            return VHDR_RETURN_API_FAIL;
        }

        // LCS_SEL
        if(MFALSE == m_pNormalPipe->sendCommand(ENPipeCmd_SET_MODULE_SEL, EPipe_P1Sel_LCS,aEn,-1))
        {
            MY_LOGE_IF(g_debugDump, "LCS_SEL fail");
            return VHDR_RETURN_API_FAIL;
        }
#endif
    }
    else if(mSensorTg == CAM_TG_2)
    {
        // LCS_EN_D
    #if 0   // use callback to enable
        if(MFALSE == m_pNormalPipe->sendCommand(ENPipeCmd_SET_MODULE_EN, EModule_LCS_D,aEn, -1))
        {
            MY_LOGE_IF(g_debugDump, "LCS_EN_D fail");
            return VHDR_RETURN_API_FAIL;
        }
    #else
        mLcsD_En = aEn;
    #endif
#if VHDR_RUN_LEGACY
        // LCSO_EN_D (Now no need to enable LCSO??)
        if(MFALSE == m_pNormalPipe->sendCommand(ENPipeCmd_SET_MODULE_EN, EModule_LCSO_D,aEn, -1))
        {
            MY_LOGE_IF(g_debugDump, "LCSO_EN_D fail");
            return VHDR_RETURN_API_FAIL;
        }

        // LCS_SEL_EN_D
        if(MFALSE == m_pNormalPipe->sendCommand(ENPipeCmd_SET_MODULE_SEL, EPipe_P1Sel_LCS_EN_D,aEn,-1))
        {
            MY_LOGE_IF(g_debugDump, "LCS_SEL_EN fail");
            return VHDR_RETURN_API_FAIL;
        }

        // LCS_SEL_D
        if(MFALSE == m_pNormalPipe->sendCommand(ENPipeCmd_SET_MODULE_SEL, EPipe_P1Sel_LCS_D,aEn,-1))
        {
            MY_LOGE_IF(g_debugDump, "LCS_SEL fail");
            return VHDR_RETURN_API_FAIL;
        }
#endif
    }
    else
    {
        MY_LOGE_IF(g_debugDump, "wrong TG(%u)",mSensorTg);
        return VHDR_RETURN_INVALID_PARA;
    }

    return VHDR_RETURN_NO_ERROR;
}


/*******************************************************************************
*
********************************************************************************/
MVOID VHdrDrvImp::EnableLce(const MBOOL &aEn)
{
    VHDR_LOG("(en,dev,tg)=(%d,%u,%u)",aEn,mSensorDev,mSensorTg);

    ESensorDev_T tuningSensorDevType = ConvertSensorDevType(mSensorDev);
    ESensorTG_T tuningSensorTgType   = ConvertSensorTgType(mSensorTg);
#if VHDR_ISP_MGR_READY
    ISP_MGR_LCE_T::getInstance(tuningSensorDevType).setEnable(aEn);
#endif
}

/*******************************************************************************
*
********************************************************************************/
MVOID VHdrDrvImp::EnableRmg(const MBOOL &aEn)
{
    //MY_LOGD_IF(g_debugDump, "(en,dev,tg)=(%d,%u,%u)",aEn,mSensorDev,mSensorTg);

    ESensorDev_T tuningSensorDevType = ConvertSensorDevType(mSensorDev);
    ESensorTG_T tuningSensorTgType   = ConvertSensorTgType(mSensorTg);
    VHDR_LOG( "(en,dev,tg)=(%d,%d,%d)",aEn,tuningSensorDevType,tuningSensorTgType);

#if VHDR_ISP_MGR_READY
    ISP_MGR_RMG_T::getInstance(tuningSensorDevType).setEnable(aEn);
    MY_LOGD_IF(g_debugDump, "EnableRmg instance=%p", &ISP_MGR_RMG_T::getInstance(tuningSensorDevType));

    // enable debug log, we don't want to add API, so we add here.
    if(aEn){
        ISP_MGR_RMG_T::getInstance(tuningSensorDevType).setDebugDump(g_debugDump);
    }else{
        ISP_MGR_RMG_T::getInstance(tuningSensorDevType).setDebugDump(0);
    }
#endif
}

/*******************************************************************************
*
********************************************************************************/
MVOID VHdrDrvImp::EnableRmm(const MBOOL &aEn)
{
    //MY_LOGD_IF(g_debugDump, "(en,dev,tg)=(%d,%u,%u)",aEn,mSensorDev,mSensorTg);

    ESensorDev_T tuningSensorDevType = ConvertSensorDevType(mSensorDev);
    ESensorTG_T tuningSensorTgType   = ConvertSensorTgType(mSensorTg);
    VHDR_LOG( "(en,dev,tg)=(%d,%d,%d)",aEn,tuningSensorDevType,tuningSensorTgType);

#if VHDR_ISP_MGR_READY
    ISP_MGR_RMM_T::getInstance(tuningSensorDevType).setEnable(aEn);
    MY_LOGD_IF(g_debugDump, "EnableRmm instance=%p", &ISP_MGR_RMG_T::getInstance(tuningSensorDevType));

    // enable debug log, we don't want to add API, so we add here.
    if(aEn){
        ISP_MGR_RMM_T::getInstance(tuningSensorDevType).setDebugDump(g_debugDump);
    }else{
        ISP_MGR_RMM_T::getInstance(tuningSensorDevType).setDebugDump(0);
    }
#endif
}

/*******************************************************************************
*
********************************************************************************/
MINT32 VHdrDrvImp::CreateMemBuf(MUINT32 &memSize,const MUINT32 &bufCnt, IMEM_BUF_INFO *bufInfo)
{
    MINT32 err = VHDR_RETURN_NO_ERROR;
#if VHDR_RUN_LEGACY

    MUINT32 alingSize = (memSize + L1_CACHE_BYTES - 1) & ~(L1_CACHE_BYTES - 1);

    MY_LOGD_IF(g_debugDump, "Cnt(%u),Size(%u),alingSize(%u)",bufCnt, memSize, alingSize);

    memSize = alingSize;

    if(bufCnt > 1)  // more than one
    {
        for(MUINT32 i = 0; i < bufCnt; ++i)
        {
            bufInfo[i].size = alingSize;

            if(m_pIMemDrv->allocVirtBuf(NAME, &bufInfo[i]) < 0)
            {
                MY_LOGE_IF(g_debugDump, "m_pIMemDrv->allocVirtBuf() error, i(%d)",i);
                err = VHDR_RETURN_API_FAIL;
            }

            if(m_pIMemDrv->mapPhyAddr(&bufInfo[i]) < 0)
            {
                MY_LOGE_IF(g_debugDump, "m_pIMemDrv->mapPhyAddr() error, i(%d)",i);
                err = VHDR_RETURN_API_FAIL;
            }
        }
    }
    else
    {
        bufInfo->size = alingSize;

        if(m_pIMemDrv->allocVirtBuf(NAME, bufInfo) < 0)
        {
            MY_LOGE_IF(g_debugDump, "m_pIMemDrv->allocVirtBuf() error");
            err = VHDR_RETURN_API_FAIL;
        }

        if(m_pIMemDrv->mapPhyAddr(bufInfo) < 0)
        {
            MY_LOGE_IF(g_debugDump, "m_pIMemDrv->mapPhyAddr() error");
            err = VHDR_RETURN_API_FAIL;
        }
    }
#endif
    return err;
}



/******************************************************************************
*
*******************************************************************************/
MINT32 VHdrDrvImp::DestroyMemBuf(const MUINT32 &bufCnt, IMEM_BUF_INFO *bufInfo)
{

    MINT32 err = VHDR_RETURN_NO_ERROR;
#if VHDR_RUN_LEGACY
    MY_LOGD_IF(g_debugDump, "Cnt(%u)", bufCnt);


    if(bufCnt > 1)  // more than one
    {
        for(MUINT32 i = 0; i < bufCnt; ++i)
        {
            if(0 == bufInfo[i].virtAddr)
            {
                MY_LOGD_IF(g_debugDump, "Buffer doesn't exist, i(%d)",i);
                continue;
            }

            if(m_pIMemDrv->unmapPhyAddr(&bufInfo[i]) < 0)
            {
                MY_LOGE_IF(g_debugDump, "m_pIMemDrv->unmapPhyAddr() error, i(%d)",i);
                err = VHDR_RETURN_API_FAIL;
            }

            if (m_pIMemDrv->freeVirtBuf(NAME, &bufInfo[i]) < 0)
            {
                MY_LOGE_IF(g_debugDump, "m_pIMemDrv->freeVirtBuf() error, i(%d)",i);
                err = VHDR_RETURN_API_FAIL;
            }
        }
    }
    else
    {
        if(0 == bufInfo->virtAddr)
        {
            MY_LOGD_IF(g_debugDump, "Buffer doesn't exist");
        }

        if(m_pIMemDrv->unmapPhyAddr(bufInfo) < 0)
        {
            MY_LOGE_IF(g_debugDump, "m_pIMemDrv->unmapPhyAddr() error");
            err = VHDR_RETURN_API_FAIL;
        }

        if (m_pIMemDrv->freeVirtBuf(NAME, bufInfo) < 0)
        {
            MY_LOGE_IF(g_debugDump, "m_pIMemDrv->freeVirtBuf() error");
            err = VHDR_RETURN_API_FAIL;
        }
    }
#endif
    return err;
}

/******************************************************************************
*
*******************************************************************************/
ESensorDev_T VHdrDrvImp::ConvertSensorDevType(const MUINT32 &aSensorDev)
{
    switch(aSensorDev)
    {
        case SENSOR_DEV_MAIN: return ESensorDev_Main;
        case SENSOR_DEV_SUB: return ESensorDev_Sub;
        case SENSOR_DEV_MAIN_2: return ESensorDev_MainSecond;
        case SENSOR_DEV_MAIN_3D: return ESensorDev_Main3D;
        default : MY_LOGE_IF(g_debugDump, "wrong sensorDev(%d), return ESensorDev_Main",aSensorDev);
                  return ESensorDev_Main;
    }
}

/******************************************************************************
*
*******************************************************************************/
ESensorTG_T VHdrDrvImp::ConvertSensorTgType(const MUINT32 &aSensorTg)
{
    switch(aSensorTg)
    {
        case CAM_TG_1: return ESensorTG_1;
        case CAM_TG_2: return ESensorTG_2;
        default : MY_LOGE_IF(g_debugDump, "wrong sensorTg(%d), return ESensorTG_1",aSensorTg);
                  return ESensorTG_1;
    }
}

/******************************************************************************
*
*******************************************************************************/
MVOID VHdrDrvImp::DumpLcso(const char *usr,const MUINT32 &addr)
{
#if VHDR_RUN_LEGACY
#define LCSO_DUMP_PREFIX "/sdcard/lcsodump/"

    static MUINT32 cnt = 0;

    if(cnt == 4294967295)
    {
        cnt = 1;
    }
    else
    {
        ++cnt;
    }

    char filename[256];
    sprintf(filename, "%s%s_%dx%d_%d.raw",LCSO_DUMP_PREFIX,usr,mLcs_outW,mLcs_outH,cnt);                                              \

    if(!makePath(LCSO_DUMP_PREFIX,0660))
    {
        MY_LOGE_IF(g_debugDump, "makePath [%s] fail",LCSO_DUMP_PREFIX);
        return;
    }

    if(MFALSE == SaveToFile(filename,addr))
    {
        MY_LOGE_IF(g_debugDump, "dump LCEO fail");
    }
#endif
}

/******************************************************************************
*
*******************************************************************************/
MBOOL VHdrDrvImp::SaveToFile(char const *filepath,const MUINTPTR &addr)
{
    MBOOL ret = MFALSE;
    MINT32 fd = -1;
    MUINT8 *pBuf = (MUINT8 *)addr;
    MUINT32 size = mLcs_outW * mLcs_outH;
    MUINT32 written = 0;
    MINT32 nw = 0, cnt = 0;

    MY_LOGD_IF(g_debugDump, "save to %s",filepath);

    //====== open file descriptor =======

    fd = ::open(filepath, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
    if( fd < 0 )
    {
        MY_LOGE_IF(g_debugDump, "fail to open %s: %s", filepath, ::strerror(errno));
        goto saveToFileExit;
    }

    //====== Save File ======

    while(written < size )
    {
        nw = ::write(fd, pBuf+written, size-written);
        if(nw < 0)
        {
            MY_LOGE_IF(g_debugDump, "fail to write %s, write-count:%d, written-bytes:%d : %s",filepath,cnt,written,::strerror(errno));
            goto saveToFileExit;
        }
        written += nw;
        cnt ++;
    }
    MY_LOGD_IF(g_debugDump, "[%d-th plane] write %d bytes to %s",size,filepath);

    ret = MTRUE;
saveToFileExit:

    if  ( fd >= 0 )
    {
        ::close(fd);
    }

    return  ret;
}

/*******************************************************************************
*
********************************************************************************/
MVOID VHdrDrvImp::SetVHdrState(const VHDR_STATE_ENUM &aState)
{
    mState = aState;
    MY_LOGD_IF(g_debugDump, "aState(%d),mState(%d)",aState,mState);
}


/*******************************************************************************
*
********************************************************************************/
VHDR_STATE_ENUM VHdrDrvImp::GetVHdrState()
{
    return mState;
}

/*******************************************************************************
*
********************************************************************************/
MVOID VHdrDrvImp::ChangeThreadSetting(char const *userName)
{
#if VHDR_RUN_LEGACY
    //> set name

    ::prctl(PR_SET_NAME,userName, 0, 0, 0);

    //> set policy/priority
    {
        const MINT32 expect_policy   = SCHED_OTHER;
        const MINT32 expect_priority = NICE_CAMERA_3A_MAIN;
        MINT32 policy = 0, priority = 0;
        setThreadPriority(expect_policy, expect_priority);
        getThreadPriority(policy, priority);

        MY_LOGD_IF(g_debugDump, "policy:(expect,result)=(%d,%d),priority:(expect,result)=(%d, %d)",expect_policy,policy,expect_priority,priority);
    }
#endif
}

/*******************************************************************************
*
********************************************************************************/
VHdrP1Cb::VHdrP1Cb(MVOID *arg)
{
    m_pClassObj = arg;
}

/*******************************************************************************
*
********************************************************************************/
VHdrP1Cb::~VHdrP1Cb()
{
    m_pClassObj = NULL;
}

/*******************************************************************************
*
********************************************************************************/
void VHdrP1Cb::p1TuningNotify(MVOID *pInput,MVOID *pOutput)
{

    MY_LOGD_IF(g_debugDump, "[ + ]");
    VHdrDrvImp *_this = reinterpret_cast<VHdrDrvImp *>(m_pClassObj);

    LCS_REG_CFG *apLcsCfgData   = (LCS_REG_CFG *)pOutput;

    if(_this->mIsVhdrConfig == 0)
    {
        MY_LOGD_IF(g_debugDump, "not config done");
        apLcsCfgData->bLCS_Bypass = MTRUE;
        return;
    }

    LCS_INPUT_INFO *apLcsInInfo = (LCS_INPUT_INFO *)pInput;  // Crop Use
    LCS_REG_CFG     lcs_cfg; // output

    MSize           lcs_out(_this->mLcs_outW, _this->mLcs_outH);

    // ==== For no Crop situation, Find LCS input info ==========
#if 1
    MUINT32 lcsInWidth;
    MUINT32 lcsInHeight;
    MUINT32 lcsOutWidth;
    MUINT32 lcsOutHeight;

    if(_this->mSensorTg == CAM_TG_1)
    {
        lcsInWidth  = _this->mLcsAws & 0x1FFF;
        lcsInHeight = (_this->mLcsAws & 0x1FFF0000) >> 16;
        lcsOutWidth = _this->mLcs_outW;
        lcsOutHeight = _this->mLcs_outH;
    }
    else if(_this->mSensorTg == CAM_TG_2)
    {
        lcsInWidth  = _this->mLcsD_Aws & 0x1FFF;
        lcsInHeight = (_this->mLcsD_Aws & 0x1FFF0000) >> 16;
        lcsOutWidth = _this->mLcsD_outW;
        lcsOutHeight = _this->mLcsD_outH;
    }
    else
    {
        MY_LOGE_IF(g_debugDump, "Not supported CAM_TG number!, mSensorTg = %d", _this->mSensorTg);
        return;
    }



   // MSize           lcs_in(_this->mVideoPrvW, _this->mVideoPrvH);  // TODO use prev size set by P1Node, before configVhdr
    //MSize           lcs_in(lcsInWidth, lcsInHeight);  // TODO use prev size set by P1Node, before configVhdr
    NSCamHW::Rect lcsCropRect(0, 0, lcsInWidth, lcsInHeight);
     // TODO need to check
#endif

    // =====  For RRZ Has Crop, find LCS input info  ===========
#if 0
    NSCamHW::Rect lcsCropRect(apLcsInInfo->RRZ_IN_CROP.start_x , apLcsInInfo->RRZ_IN_CROP.start_y
                        , apLcsInInfo->RRZ_IN_CROP.crop_size_w, apLcsInInfo->RRZ_IN_CROP.crop_size_h);
#endif

    // ============================


    MUINT32         lrzr_x, lrzr_y;

    lrzr_x = ((2*lcs_out.w - 2)*1048576)/(lcsCropRect.w - 1);
    lrzr_y = ((lcs_out.h-1)*1048576)/(lcsCropRect.h-1);

    //MY_LOGD_IF(g_debugDump, "LCS info: in_%d_%d out_%d_%d lrzr_%d_%d", lcs_in.w, lcs_in.h, lcs_out.w, lcs_out.h,
    //        lrzr_x, lrzr_y);

    MY_LOGD_IF(g_debugDump, "TG(%d),bIsHbin(%d), binInSize(%u,%u)",_this->mSensorTg,apLcsInInfo->bIsHbin, apLcsInInfo->sHBINOut.w, apLcsInInfo->sHBINOut.h);
    VHDR_LOG( "bIsHbin(%d), binInSize(%u,%u), RRZ_crop(%u,%u,%u,%u) RRZ_In(%u,%u) RRZ_Out(%u,%u)",
                apLcsInInfo->bIsHbin, apLcsInInfo->sHBINOut.w, apLcsInInfo->sHBINOut.h,
                apLcsInInfo->RRZ_IN_CROP.start_x, apLcsInInfo->RRZ_IN_CROP.start_y,
                apLcsInInfo->RRZ_IN_CROP.crop_size_w, apLcsInInfo->RRZ_IN_CROP.crop_size_h,
                apLcsInInfo->RRZ_IN_CROP.in_size_w, apLcsInInfo->RRZ_IN_CROP.in_size_h,
                apLcsInInfo->sRRZOut.w, apLcsInInfo->sRRZOut.h);
    MY_LOGD_IF(g_debugDump, "LcsOut(%u,%u)", lcs_out.w, lcs_out.h);

    lcs_cfg.bLCS_EN = MTRUE;
    lcs_cfg.bLCS_Bypass = MFALSE;
    lcs_cfg._LCS_REG.LCS_CON = (((lcs_out.h&0x7F)<<24) | ((lcs_out.w&0x7F)<<16));
    lcs_cfg._LCS_REG.LCS_ST  = 0 | lcsCropRect.x | (lcsCropRect.y << 16);;
    lcs_cfg._LCS_REG.LCS_AWS = (((lcsCropRect.h&0x1FFF)<<16) | (lcsCropRect.w&0x1FFF));
    lcs_cfg._LCS_REG.LCS_FLR = 0 | (_this->mFlareGain << 7) | _this->mFlareOffset;
    lcs_cfg._LCS_REG.LCS_LRZR_1 = (lrzr_x & 0xFFFFF);
    lcs_cfg._LCS_REG.LCS_LRZR_2 = (lrzr_y & 0xFFFFF);

    *(LCS_REG_CFG*)pOutput = lcs_cfg;
// ======

#if 0
    LCS_INPUT_INFO *apLcsInInfo = (LCS_INPUT_INFO *)pInput;  // Crop Use


    apLcsCfgData->bLCS_Bypass = MFALSE;
    apLcsCfgData->bLCS_EN = (_this->mSensorTg == CAM_TG_1) ? _this->mLcsEn : _this->mLcsD_En;

    if(_this->mVrWithEis == MFALSE)
    {
        //> LCS_ST

        apLcsCfgData->_LCS_REG.LCS_ST = 0;

        if(_this->mSensorTg == CAM_TG_1)
        {
            //> LCS_CON

            MUINT32 lcsCon = 0 | (_this->mLcs_outH << 24) | (_this->mLcs_outW << 16) | 0x01;

            apLcsCfgData->_LCS_REG.LCS_CON = lcsCon;

            //> LCS_AWS

            apLcsCfgData->_LCS_REG.LCS_AWS = _this->mLcsAws;

            //> LCS_FLR

            MUINT32 lcsFlr = 0 | (_this->mFlareGain << 7) | _this->mFlareOffset;

            apLcsCfgData->_LCS_REG.LCS_FLR = lcsFlr;

            //> LCS_LRZR_1 and LCS_LRZR_2

            MUINT32 lcsInWidth  = _this->mLcsAws & 0x1FFF;
            MUINT32 lcsInHeight = (_this->mLcsAws & 0x1FFF0000) >> 16;

            MUINT32 lcsLrzrX = ((2 * _this->mLcs_outW - 2) << 20) / (lcsInWidth - 1);
            MUINT32 lcsLrzrY = ((_this->mLcs_outH - 1) << 20) / (lcsInHeight - 1);

            apLcsCfgData->_LCS_REG.LCS_LRZR_1 = lcsLrzrX;
            apLcsCfgData->_LCS_REG.LCS_LRZR_2 = lcsLrzrY;

            if(g_debugDump > 0)
            {
                MY_LOGD_IF(g_debugDump, "lcsIn(%u,%u),mLcs_out(%u,%u)",lcsInWidth,lcsInHeight,_this->mLcs_outW,_this->mLcs_outH);
            }
        }
        else
        {
            //> LCS_CON

            MUINT32 lcsCon = 0 | (_this->mLcsD_outH << 24) | (_this->mLcsD_outW << 16) | 0x01;

            apLcsCfgData->_LCS_REG.LCS_CON = lcsCon;

            //> LCS_AWS

            apLcsCfgData->_LCS_REG.LCS_AWS = _this->mLcsD_Aws;

            //> LCS_FLR

            MUINT32 lcsFlr = 0 | (_this->mFlareGain << 7) | _this->mFlareOffset;

            apLcsCfgData->_LCS_REG.LCS_FLR = lcsFlr;

            //> LCS_LRZR_1 and LCS_LRZR_2

            MUINT32 lcsInWidth  = _this->mLcsD_Aws & 0x1FFF;
            MUINT32 lcsInHeight = (_this->mLcsD_Aws & 0x1FFF0000) >> 16;

            MUINT32 lcsLrzrX = ((2 * _this->mLcsD_outW - 2) << 20) / (lcsInWidth - 1);
            MUINT32 lcsLrzrY = ((_this->mLcsD_outH - 1) << 20) / (lcsInHeight - 1);

            apLcsCfgData->_LCS_REG.LCS_LRZR_1 = lcsLrzrX;
            apLcsCfgData->_LCS_REG.LCS_LRZR_2 = lcsLrzrY;

            if(g_debugDump > 0)
            {
                MY_LOGD_IF(g_debugDump, "lcsIn(%u,%u),mLcsD_out(%u,%u)",lcsInWidth,lcsInHeight,_this->mLcsD_outW,_this->mLcsD_outH);
            }
        }

        if(g_debugDump > 0)
        {
            MY_LOGD_IF(g_debugDump, "CON(0x%08x),ST(0x%08x),AWS(0x%08x),FLR(0x%08x),LRZR_1(0x%08x),LRZR_2(0x%08x)",apLcsCfgData->_LCS_REG.LCS_CON,
                                                                                                    apLcsCfgData->_LCS_REG.LCS_ST,
                                                                                                    apLcsCfgData->_LCS_REG.LCS_AWS,
                                                                                                    apLcsCfgData->_LCS_REG.LCS_FLR,
                                                                                                    apLcsCfgData->_LCS_REG.LCS_LRZR_1,
                                                                                                    apLcsCfgData->_LCS_REG.LCS_LRZR_2);
        }
    }
    else
    {
        //====== Calculate Crop ======

        NSCamHW::Rect srcRect(0,0,apLcsInInfo->sRRZOut.w,apLcsInInfo->sRRZOut.h);
        NSCamHW::Rect dstRect(0,0,_this->mVideoPrvW,_this->mVideoPrvH);

        NSCamHW::Rect lcsCropRect = android::MtkCamUtils::calCrop(srcRect,dstRect,_this->mZoomRatio);
        //NSCamHW::Rect lcsCropRect = android::MtkCamUtils::calCrop(srcRect,dstRect,100);

        //====== Calculate EIS for FE Crop ======

        MUINT32 feBlock;

        if(lcsCropRect.w <= 792 && lcsCropRect.h <= 528)
        {
            feBlock = 8;
        }
        else if(lcsCropRect.w <= 2304 && lcsCropRect.h <= 1296)
        {
            feBlock = 16;
        }
        else
        {
            feBlock = 32;
            MY_LOGW_IF(g_debugDump, ("FE should not use 32");
        }

        lcsCropRect.w = ALIGN_SIZE(lcsCropRect.w,feBlock);
        lcsCropRect.h = ALIGN_SIZE(lcsCropRect.h,feBlock);

        //====== Apply to LCS Input Image ======

        if(!apLcsInInfo->bIsHbin)
        {
            lcsCropRect.x *= 2;
            lcsCropRect.w *= 2;
        }

        lcsCropRect.y *= 2;
        lcsCropRect.h *= 2;

        MY_LOGD_IF(g_debugDump, "lcsCropRect(%u,%u,%u,%u)",lcsCropRect.x,lcsCropRect.y,lcsCropRect.w,lcsCropRect.h);

        if(g_debugDump > 0)
        {
            MY_LOGD_IF(g_debugDump, "TG(%d),bIsHbin(%d)",_this->mSensorTg,apLcsInInfo->bIsHbin);
            MY_LOGD_IF(g_debugDump, "RRZ(%u,%u),HBIN(%u,%u)",apLcsInInfo->sRRZOut.w,apLcsInInfo->sRRZOut.h,apLcsInInfo->sHBINOut.w,apLcsInInfo->sHBINOut.h);
            MY_LOGD_IF(g_debugDump, "pass1Out(%d,%d),VideoPrv(%u,%u)",_this->mPass1OutW,_this->mPass1OutH,_this->mVideoPrvW,_this->mVideoPrvH);
            MY_LOGD_IF(g_debugDump, "mZoomRatio(%u),feBlock(%u)",_this->mZoomRatio,feBlock);
            MY_LOGD_IF(g_debugDump, "LcsOut(%u,%u)",(_this->mSensorTg == CAM_TG_1) ? _this->mLcs_outW : _this->mLcsD_outW,
                                     (_this->mSensorTg == CAM_TG_1) ? _this->mLcs_outH : _this->mLcsD_outH);
        }

        //====== Prepare Reg ======

        MUINT32 lcsCon = 0;

        if(_this->mSensorTg == CAM_TG_1)
        {
            lcsCon = lcsCon | (_this->mLcs_outH << 24) | (_this->mLcs_outW << 16) | 0x01;
        }
        else
        {
            lcsCon = lcsCon | (_this->mLcsD_outH << 24) | (_this->mLcsD_outW << 16) | 0x01;
        }

        MUINT32 lcsSt  = 0 | lcsCropRect.x | (lcsCropRect.y << 16);
        MUINT32 lcsAws = 0 | lcsCropRect.w | (lcsCropRect.h << 16);
        MUINT32 lcsFlr = 0 | (_this->mFlareGain << 7) | _this->mFlareOffset;
        MUINT32 lcsLrzrX = (_this->mSensorTg == CAM_TG_1) ? ((2*_this->mLcs_outW-2)<<20)/(lcsCropRect.w-1) : ((2*_this->mLcsD_outW-2)<<20) / (lcsCropRect.w-1);
        MUINT32 lcsLrzrY = (_this->mSensorTg == CAM_TG_1) ? ((_this->mLcs_outH-1)<<20)/(lcsCropRect.h-1) : ((_this->mLcsD_outH-1)<<20)/(lcsCropRect.h-1);

        apLcsCfgData->_LCS_REG.LCS_CON = lcsCon;
        apLcsCfgData->_LCS_REG.LCS_ST  = lcsSt;
        apLcsCfgData->_LCS_REG.LCS_AWS = lcsAws;
        apLcsCfgData->_LCS_REG.LCS_FLR = lcsFlr;
        apLcsCfgData->_LCS_REG.LCS_LRZR_1 = lcsLrzrX;
        apLcsCfgData->_LCS_REG.LCS_LRZR_2 = lcsLrzrY;

        if(g_debugDump > 0)
        {
            MY_LOGD_IF(g_debugDump, "(con,st,aws,flr,lrzr1,lrzr2)=(0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x)",apLcsCfgData->_LCS_REG.LCS_CON,
                                                                                                apLcsCfgData->_LCS_REG.LCS_ST,
                                                                                                apLcsCfgData->_LCS_REG.LCS_AWS,
                                                                                                apLcsCfgData->_LCS_REG.LCS_FLR,
                                                                                                apLcsCfgData->_LCS_REG.LCS_LRZR_1,
                                                                                                apLcsCfgData->_LCS_REG.LCS_LRZR_2);
        }
    }
#endif


    MY_LOGD_IF(g_debugDump, "[ - ]");
}

#endif

