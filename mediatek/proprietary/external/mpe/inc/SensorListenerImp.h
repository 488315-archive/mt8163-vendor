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
#ifndef MPE_SENSOR_LISTENER_IMP_H
#define MPE_SENSOR_LISTENER_IMP_H

#ifdef __cplusplus
  extern "C" {
#endif
//-----------------------------------------------------------------------------
#include <sys/types.h>
#include <stdint.h>
#include "SensorListener.h"

bool setThreadPriority(int policy, int priority);
bool getThreadPriority(int& policy, int& priority);

class SensorListenerImpThread : public Thread {
    public:
        SensorListenerImpThread(Looper* looper) : Thread(false) {
            mLooper = sp<Looper>(looper);
        }
        ~SensorListenerImpThread() {
            mLooper.clear();
        }
        status_t readyToRun() {
            int policy = SCHED_OTHER, priority = 0;
            ::prctl(PR_SET_NAME,"MPE@SensorListenerImpThread", 0, 0, 0);
            setThreadPriority(policy,priority);
            getThreadPriority(policy,priority);
            return MPE_SUCCESS;
        }
        virtual bool threadLoop() {
            mLooper->pollOnce(-1);
            return TRUE;
        }
        void wake() {
            mLooper->wake();
        }
    private:
        sp<Looper> mLooper;
};

class SensorListenerImp : public SensorListener {
    public:
        SensorListenerImp();
        ~SensorListenerImp();

        virtual void    destroyInstance(void);
        virtual int   setListener(Listener func);
        virtual int   enableSensor(SensorTypeEnum  sensorType, mtk_uint32 periodInMs);
        virtual int   disableSensor(SensorTypeEnum sensorType);
        virtual int   init(void);
        virtual int   uninit(void);
        virtual int   getEvents(void);
        virtual int   isSupport(void);
    private:
        #define SENSOR_TYPE_AMOUNT  SensorType_max
        mutable Mutex               mLock;
        volatile mtk_int32          mUser;
        Sensor const*               mpSensor[SENSOR_TYPE_AMOUNT];
        volatile mtk_uint32         mSensorEnableCnt;
        sp<Looper>                  mspLooper;
        sp<SensorEventQueue>        mspSensorEventQueue;
        SensorManager*              mpSensorManager;
        Listener                    mpListener;
        sp<SensorListenerImpThread> mspThread;
};

#ifdef __cplusplus
}
#endif

#endif //#ifndef MPE_SENSOR_LISTENER_IMP_H
