/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein is
* confidential and proprietary to MediaTek Inc. and/or its licensors. Without
* the prior written permission of MediaTek inc. and/or its licensors, any
* reproduction, modification, use or disclosure of MediaTek Software, and
* information contained herein, in whole or in part, shall be strictly
* prohibited.
*
* MediaTek Inc. (C) 2010. All rights reserved.
*
* BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
* THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
* RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
* ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
* WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
* WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
* NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
* RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
* INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
* TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
* RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
* OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
* SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
* RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
* STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
* ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
* RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
* MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
* CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
* The following software/firmware and/or related documentation ("MediaTek
* Software") have been modified by MediaTek Inc. All revisions are subject to
* any receiver's applicable license agreements with MediaTek Inc.
*/

/**
* @file HwEventIrq.cpp
* @brief Declarations of Hw IRQ Event waiting interface
*/
#define LOG_TAG "HwIRQ3A"

#include "IEventIrq.h"

#include <string>
#include <aaa_types.h>
#include <aaa_log.h>
#include <isp_tuning.h>
#include <CamIO/INormalPipe.h>

namespace NS3Av3
{
using namespace android;
using namespace NSCam::NSIoPipe;
using namespace NSCam::NSIoPipe::NSCamIOPipe;

/******************************************************************************
 *  Default EventIrq
 ******************************************************************************/
class HwEventIrq : public IEventIrq
{
public:
    /**
     * @brief Create instance of IEventIrq
     * @param [in] rCfg config of sensor, tg, and event for listening.
     * @param [in] strUser username
     */
    static HwEventIrq*          createInstance(const IEventIrq::ConfigParam& rCfg, const char* strUser);

    /**
     * @brief Destroy instance of IEventIrq
     * @param [in] strUser username
     */
    virtual MVOID               destroyInstance(const char* strUser);

    /**
     * @brief Register to listen the event
     * @return
     * - MINT32 0 for success
     */
    virtual MINT32              mark();

    /**
     * @brief Wait for the event
     * @param [out] rDuration
     * @return
     * - 0 for blocking wait case
     * - -1 indicates error
     * - other values indicate the number of event happened
     */
    virtual MINT32              wait(Duration& rDuration);

    /**
     * @brief Query for the event
     * @param [out] rDuration
     * @return
     * - 0 for indicating the event not yet happened
     * - -1 indicates error
     * - other values indicate the number of event happened
     */
    virtual MINT32              query(Duration& rDuration);

protected:
    HwEventIrq();
    virtual ~HwEventIrq(){}

    MVOID                       init(const IEventIrq::ConfigParam& rCfg, const char* strUser);
    MVOID                       uninit(const char* strUser);

    MINT32                      m_i4User;
    mutable Mutex               m_Lock;

    IEventIrq::ConfigParam      m_rCfgParam;
    std::string                 m_strName;

    INormalPipe*                m_pSttPipe;
    MINT32                      m_i4UserKey;
    EPipeSignal                 m_eSignal;
};

template<NSIspTuning::ESensorDev_T eSensorDev, HwEventIrq::E_Event_Type eType>
class HwEventIrqType : public HwEventIrq
{
public:
    static HwEventIrqType* getInstance()
    {
        static HwEventIrqType<eSensorDev, eType> singleton;
        return &singleton;
    }
};

HwEventIrq::
HwEventIrq()
    : m_i4User(0)
    , m_Lock()
{
    MY_LOG("[%s]", __FUNCTION__);
}

HwEventIrq*
HwEventIrq::
createInstance(const IEventIrq::ConfigParam& rCfg, const char* strUser)
{
    HwEventIrq* pHwEventIrq = NULL;
#if 0
    switch (rCfg.eEventType)
    {
    case IEventIrq::E_Event_Vsync:
        switch (rCfg.i4SensorDev)
        {
        case NSIspTuning::ESensorDev_Main:
            pHwEventIrq = HwEventIrqType<NSIspTuning::ESensorDev_Main, IEventIrq::E_Event_Vsync>::getInstance();
            break;
        case NSIspTuning::ESensorDev_Sub:
            pHwEventIrq = HwEventIrqType<NSIspTuning::ESensorDev_Sub, IEventIrq::E_Event_Vsync>::getInstance();
            break;
        case NSIspTuning::ESensorDev_MainSecond:
            pHwEventIrq = HwEventIrqType<NSIspTuning::ESensorDev_MainSecond, IEventIrq::E_Event_Vsync>::getInstance();
            break;
        }
        break;
    case IEventIrq::E_Event_Af:
        switch (rCfg.i4SensorDev)
        {
        case NSIspTuning::ESensorDev_Main:
            pHwEventIrq = HwEventIrqType<NSIspTuning::ESensorDev_Main, IEventIrq::E_Event_Af>::getInstance();
            break;
        case NSIspTuning::ESensorDev_Sub:
            pHwEventIrq = HwEventIrqType<NSIspTuning::ESensorDev_Sub, IEventIrq::E_Event_Af>::getInstance();
            break;
        case NSIspTuning::ESensorDev_MainSecond:
            pHwEventIrq = HwEventIrqType<NSIspTuning::ESensorDev_MainSecond, IEventIrq::E_Event_Af>::getInstance();
            break;
        }
        break;
    case IEventIrq::E_Event_Vsync_Sensor:
        switch (rCfg.i4SensorDev)
        {
        case NSIspTuning::ESensorDev_Main:
            pHwEventIrq = HwEventIrqType<NSIspTuning::ESensorDev_Main, IEventIrq::E_Event_Vsync_Sensor>::getInstance();
            break;
        case NSIspTuning::ESensorDev_Sub:
            pHwEventIrq = HwEventIrqType<NSIspTuning::ESensorDev_Sub, IEventIrq::E_Event_Vsync_Sensor>::getInstance();
            break;
        case NSIspTuning::ESensorDev_MainSecond:
            pHwEventIrq = HwEventIrqType<NSIspTuning::ESensorDev_MainSecond, IEventIrq::E_Event_Vsync_Sensor>::getInstance();
            break;
        }
        break;
    }
#else
    pHwEventIrq = new HwEventIrq;
#endif
    if (pHwEventIrq)
    {
        pHwEventIrq->init(rCfg, strUser);
        return pHwEventIrq;
    }

    MY_ERR("dev(%d)", rCfg.i4SensorDev);
    static HwEventIrq _rHwEventIrq;
    _rHwEventIrq.init(rCfg, strUser);
    return &_rHwEventIrq;
}

MVOID
HwEventIrq::
destroyInstance(const char* strUser)
{
    uninit(strUser);
    delete this;
}

MVOID
HwEventIrq::
init(const IEventIrq::ConfigParam& rCfg, const char* strUser)
{
    Mutex::Autolock lock(m_Lock);

    if (m_i4User > 0)
    {
        MY_LOG_IF(1, "[%s] m_i4User(%d), m_strName(%s), strUser(%s)", __FUNCTION__, m_i4User, m_strName.c_str(), strUser);
        m_i4User ++;
        return;
    }

    m_rCfgParam = rCfg;
    m_strName = strUser;

    m_pSttPipe = INormalPipe::createInstance(rCfg.i4SensorIndex, strUser);
    //IStatisticPipe::createInstance(rCfg.i4SensorIndex, strUser);
    m_i4UserKey = m_pSttPipe->attach(strUser);
    switch (rCfg.eEventType)
    {
    default:
    case IEventIrq::E_Event_Vsync:
        m_eSignal = EPipeSignal_SOF;
        break;
    case IEventIrq::E_Event_Af:
        m_eSignal = EPipeSignal_AFDONE;
        break;
    }
    m_i4User ++;

    MY_LOG("[%s](%s) this(%p) m_pSttPipe(%p), userKey(%d), cfg(%d, %d, %d, %d)", __FUNCTION__, strUser, this, m_pSttPipe, m_i4UserKey,
        m_rCfgParam.i4SensorDev, m_rCfgParam.i4SensorIndex, m_rCfgParam.eEventType, m_rCfgParam.u4TimeoutMs);
}

MVOID
HwEventIrq::
uninit(const char* strUser)
{
    Mutex::Autolock lock(m_Lock);

    if (m_i4User <= 0)
    {
        return;
    }

    m_i4User --;

    if (m_i4User == 0)
    {
        // uninit
        MY_LOG("[%s] m_strName(%s), strUser(%s)", __FUNCTION__, m_strName.c_str(), strUser);
        if (m_pSttPipe)
        {
            m_pSttPipe->destroyInstance(m_strName.c_str());
            m_pSttPipe = MNULL;
        }
    }
    else
    {
        // do nothing
    }
}

MINT32
HwEventIrq::
mark()
{
    #if 0
    if (!m_pIspDrv->markIrq(m_rWaitIrq))
    {
        MY_ERR("Error");
        return -1;
    }
    else
    {
        MY_LOG("[%s] %s", __FUNCTION__, m_strName.c_str());
        return 0;
    }
    #else
        return -1;
    #endif
}

MINT32
HwEventIrq::
query(Duration& rDuration)
{
#if 0
    ISP_WAIT_IRQ_ST rWaitIrq = m_rWaitIrq;

    if (!m_pIspDrv->queryirqtimeinfo(&rWaitIrq))
    {
        MY_ERR("Error");
        return -1;
    }
    else
    {
        rDuration.i4Duration0 = rWaitIrq.TimeInfo.tmark2read_sec*1000000 + rWaitIrq.TimeInfo.tmark2read_usec;
        rDuration.i4Duration1 = rWaitIrq.TimeInfo.tevent2read_sec*1000000 + rWaitIrq.TimeInfo.tevent2read_usec;
        MY_LOG("[%s] %s: T0(%d), T1(%d), EventCnt(%d)", __FUNCTION__, m_strName.c_str(), rDuration.i4Duration0, rDuration.i4Duration1, rWaitIrq.TimeInfo.passedbySigcnt);
        return rWaitIrq.TimeInfo.passedbySigcnt;
    }
#else
    return -1;
#endif
}

MINT32
HwEventIrq::
wait(Duration& rDuration)
{
    if (!m_pSttPipe->wait(m_eSignal, EPipeSignal_ClearWait, m_i4UserKey, m_rCfgParam.u4TimeoutMs))
    {
        MY_ERR("Error");
        return -1;
    }
    else
    {
        #if 0
        rDuration.i4Duration0 = rWaitIrq.TimeInfo.tmark2read_sec*1000000 + rWaitIrq.TimeInfo.tmark2read_usec;
        rDuration.i4Duration1 = rWaitIrq.TimeInfo.tevent2read_sec*1000000 + rWaitIrq.TimeInfo.tevent2read_usec;
        MY_LOG("[%s] %s: T0(%d), T1(%d), EventCnt(%d)", __FUNCTION__, m_strName.c_str(), rDuration.i4Duration0, rDuration.i4Duration1, rWaitIrq.TimeInfo.passedbySigcnt);
        return rWaitIrq.TimeInfo.passedbySigcnt;
        #else
        MY_LOG("[%s] %s", __FUNCTION__, m_strName.c_str());
        return 0;
        #endif
    }
}

#if 1 //(CAM3_3ATESTLVL > CAM3_3AUT)
IEventIrq*
IEventIrq::
createInstance(const ConfigParam& rCfg, const char* strUser)
{
    return HwEventIrq::createInstance(rCfg, strUser);
}
#endif

};
