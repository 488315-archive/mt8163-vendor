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

#ifndef _MTK_HAL_CAMCLIENT_FDCLIENT_H_
#define _MTK_HAL_CAMCLIENT_FDCLIENT_H_
//
#include "../inc/CamUtils.h"
using namespace android;
using namespace MtkCamUtils;
#include <v1/IParamsManager.h>
#include "inc/IAsdClient.h"
#include <v1/client/IFDClient.h>
#include "FDBufMgr.h"
//
#include <faces.h>
#include <system/camera.h>
//Hal3A
#include <IHal3A.h>
//
#include <fd_hal_base.h>
#include <gs_hal_base.h>

#include <hr_hal_base.h>

#if (MTKCAM_HR_MONITOR_SUPPORT == '1')
#include <heartrate_device.h>
#endif


static int                bufScaleX[11] = {320, 256, 204, 160, 128, 102, 80, 64, 50, 40, 32};
static int                bufScaleY[11] = {240, 192, 152, 120, 96, 76, 60, 48, 38, 30, 24};

#if (MTKCAM_HR_MONITOR_SUPPORT == '1')
typedef bool   (*HeartrateCallback_t)(HR_detection_result_t const &result, void* user);
#endif


//static int                bufScaleX[11] = {320, 256, 204, 160, 128, 104, 80, 64, 52, 40, 32};
//static int                bufScaleY[11] = {240, 192, 152, 120, 96, 80, 60, 48, 40, 32, 24};

namespace android {
namespace NSCamClient {
namespace NSFDClient {

/******************************************************************************
 *  Preview Client Handler.
 ******************************************************************************/
class FDClient : public IFDClient
               , public Thread
{
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
protected:
    halFDBase*                      mpFDHalObj;
    sp<IAsdClient>                  mpASDClient;
    halGSBase*                      mpGDHalObj;

public:     ////                    Instantiation.
    //
                                    FDClient(sp<IParamsManager> pParamsMgr);
    virtual                         ~FDClient();

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Interfaces.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////

    virtual bool                    init();
    virtual bool                    uninit();

    virtual bool                    setImgBufProviderClient(
                                        sp<IImgBufProviderClient>const& rpClient
                                    );

    //
    //
    virtual void                    setCallbacks(sp<CamMsgCbInfo> const& rpCamMsgCbInfo);

    //
    //
    virtual status_t                sendCommand(int32_t cmd, int32_t arg1, int32_t arg2);
    virtual bool                    stopPreview();
    virtual bool                    takePicture();

    /**
    For Heartrate APP
    */
    virtual bool                    startPreview();
    virtual bool                    startRecording();
    virtual bool                    stopRecording();



    //
    //
    virtual void                    enableMsgType(int32_t msgType);
    virtual void                    disableMsgType(int32_t msgType);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Operations in base class Thread
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////
    // Ask this object's thread to exit. This function is asynchronous, when the
    // function returns the thread might still be running. Of course, this
    // function can be called from a different thread.
    virtual void                    requestExit();

    // Good place to do one-time initializations
    virtual status_t                readyToRun();

private:
    // Derived class must implement threadLoop(). The thread starts its life
    // here. There are two ways of using the Thread object:
    // 1) loop: if threadLoop() returns true, it will be called again if
    //          requestExit() wasn't called.
    // 2) once: if threadLoop() returns false, the thread will exit upon return.
    virtual bool                    threadLoop();

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Command Queue.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
protected:  ////                    Definitions.
                                    struct Command
                                    {
                                        //  Command ID.
                                        enum EID
                                        {
                                            eID_UNKNOWN,
                                            eID_EXIT,
                                            eID_WAKEUP,
                                        };
                                    };

protected:  ////                    Operations.


    virtual void                    postCommand(Command::EID cmd);
    virtual bool                    getCommand(Command::EID &cmd);
    //
    virtual void                    onClientThreadLoop();
    //
    inline  int32_t                 getThreadId() const    { return mi4ThreadId; }

protected:  ////                    Data Members.
    List<Command::EID>              mCmdQue;
    Mutex                           mCmdQueMtx;
    Condition                       mCmdQueCond;    //  Condition to wait: [ ! exitPending() && mCmdQue.empty() ]
    int32_t                         mi4ThreadId;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Operations.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
protected:  ////

    bool                            startFaceDetection();
    bool                            stopFaceDetection();
    bool                            startHeartrateDetection(int enable);
    bool                            stopHeartrateDetection();
    bool                            isEnabledState();
    bool                            isEnabledFD();
    bool                            isEnabledHR();
    bool                            isMsgEnabled();
    bool                            onStateChanged();
    //bool                            doFD(ImgBufQueNode const& rQueNode, bool &rIsDetected_FD, bool &rIsDetected_SD, bool doSD);
    bool                            doFD(ImgBufQueNode const& rQueNode, bool &rIsDetected_FD, bool &rIsDetected_SD, bool doSD, bool &rIsDetected_GD, bool doGD);
    //bool                            performCallback(bool isDetected_FD, bool isDetected_SD);
    bool                            performHRCallback(void *HR_result, int type);
    bool                            performCallback(bool isDetected_FD, bool isDetected_SD, bool isDetected_GD);
    int                                doBufferAnalysis(ImgBufQueNode const& rQueNode);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
protected:  ////                    Common Info.

    mutable Mutex                   mModuleMtx;
    mutable Mutex                   mControlMtx;
    mutable Mutex                   mCallbackMtx;
    sp<CamMsgCbInfo>                mpCamMsgCbInfo;         //  Pointer to Camera Message-Callback Info.
    sp<IParamsManager>              mpParamsMgr;            //  Pointer to Parameters Manager.
    volatile int32_t                mIsFDStarted;           //  FD      Started ?
    volatile int32_t                mIsHRStarted;           //  HR      Started ?
    volatile int32_t                mIsHRPreStarted;        //  HR      Started ?
    #if (MTKCAM_HR_MONITOR_SUPPORT == '1')
    HeartrateCallback_t             mHeartrateCb;
    #endif
    void                            *mpHeartrateUser;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
protected:  ////                    Callback.
    //
    int32_t                         mi4CallbackRefCount;    //  Preview callback reference count.
    int64_t                         mi8CallbackTimeInMs;    //  The timestamp in millisecond of last preview callback.
    //

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Image Buffer
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
protected:
    //
    bool                            initBuffers(sp<IImgBufQueue>const& rpBufQueue);
    void                            uninitBuffers();
    bool                            createDetectedBuffers();
    bool                            createWorkingBuffers(sp<IImgBufQueue>const& rpBufQueue);
    bool                            createDDPWorkBuffers();
    bool                            createFDWorkBuffers();
    void                            destroyDetectedBuffers();
    void                            destroyWorkingBuffers();
    void                            destroyDDPWorkBuffers();
    void                            destroyFDWorkBuffers();
    //
    bool                            waitAndHandleReturnBuffers(sp<IImgBufQueue>const& rpBufQueue, ImgBufQueNode & rQueNode);
    bool                            handleReturnBuffers(sp<IImgBufQueue>const& rpBufQueue, ImgBufQueNode const &rQueNode);
    //
protected:
    //
    sp<IImgBufQueue>                mpImgBufQueue;
    sp<IImgBufProviderClient>       mpImgBufPvdrClient;

    //Detected faces information
    static const int                mDetectedFaceNum = 15;
    static const int                mBufCnt          = 5;
    MtkCameraFaceMetadata*          mpDetectedFaces;
    MtkCameraFaceMetadata*          mpDetectedGestures;
    bool                            mIsDetected_FD;
    bool                            mIsDetected_SD;
    bool                            mIsSDenabled;
    bool                            mIsDetected_GD;
    bool                            mIsGDenabled;

    //HRD
    int                             mHRMode;
    int                             mISHRenabled;
    int                             mHRStarting;
    halHRBase*                      mpHRObj;
    int                             mIsHRSupported;
    // For main face information
    int                             mMainFace_X;
    int                             mMainFace_Y;
    bool                            mIsMainFaceEn;

    //FDBuffer*                       DDPBuffer;
    unsigned char*                  DDPBuffer;
    unsigned char*                  ExtractYBuffer;
    unsigned char*                  FDWorkingBuffer;
    unsigned int                    FDWorkingBufferSize;
    int                             Rotation_Info;
    int clientFDDumpOPT;
    MUINT32                         mLogLevel;
    MUINT32                         mForceDisableFD;

    MUINT32                         mAEStable;
};


}; // namespace NSPreviewClient
}; // namespace NSCamClient
}; // namespace android
#endif  //_MTK_HAL_CAMCLIENT_PREVIEW_PREVIEWCLIENT_H_

