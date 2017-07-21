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

#ifndef _MTK_HARDWARE_MTKCAM_ADAPTER_STEREO_STEREOFLOWCONTROL_H_
#define _MTK_HARDWARE_MTKCAM_ADAPTER_STEREO_STEREOFLOWCONTROL_H_
//
#include <utils/RefBase.h>
#include <utils/StrongPointer.h>
#include <utils/String8.h>
#include <utils/Vector.h>
//
#include <metadata/IMetadata.h>
#include <v3/utils/streambuf/StreamBuffers.h>
#include <v3/utils/streambuf/StreamBufferProvider.h>
#include <LegacyPipeline/ILegacyPipeline.h>
#include <Scenario/IFlowControl.h>
#include <LegacyPipeline/request/IRequestController.h>
#include <LegacyPipeline/buffer/StreamBufferProvider.h>
//
#include <LegacyPipeline/stereo/buffer/StereoBufferPool.h>
#include <LegacyPipeline/stereo/buffer/StereoSelector.h>
#include <LegacyPipeline/stereo/buffer/StereoBufferSynchronizer.h>
#include <LegacyPipeline/NodeId.h>
#include <LegacyPipeline/StreamId.h>

using namespace android;
#include <v1/IParamsManagerV3.h>
#include <LegacyPipeline/LegacyPipelineUtils.h>
#include <LegacyPipeline/LegacyPipelineBuilder.h>
#include <LegacyPipeline/stereo/StereoLegacyPipelineBuilder.h>
#include <hwutils/HwInfoHelper.h>

#include <LegacyPipeline/stereo/StereoPipelineData.h>
#include <LegacyPipeline/stereo/ContextBuilder/ImageStreamManager.h>
#include <LegacyPipeline/stereo/ContextBuilder/MetaStreamManager.h>

#include <bandwidth_control.h>

#include <LegacyPipeline/IResourceContainer.h>
#include "../inc/FlowControlBase.h"
/******************************************************************************
 *
 ******************************************************************************/
//
namespace NSCam {
namespace v1 {
namespace NSLegacyPipeline {


class StereoFlowControl
    : public FlowControlBase
{

    struct CommmandType_T{
        enum {
            CONTROL_STOP_P2_PRV,
            CONTROL_START_P2_PRV
        };
    };

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Interface.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
                                                StereoFlowControl(
                                                    char const*                 pcszName,
                                                    MINT32 const                i4OpenId,
                                                    sp<IParamsManager>          pParamsManager,
                                                    sp<ImgBufProvidersManager>  pImgBufProvidersManager,
                                                    sp<INotifyCallback>         pCamMsgCbInfo
                                                );

    virtual                                     ~StereoFlowControl() {};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  IFlowControl Interface.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    virtual char const*                         getName()   const;

    virtual int32_t                             getOpenId() const;

public:  //// Adapter

    virtual status_t                            startPreview();

    virtual status_t                            stopPreview();

    virtual status_t                            suspendPreview();

    virtual status_t                            resumePreview();

    virtual status_t                            startRecording();

    virtual status_t                            stopRecording();

    virtual status_t                            autoFocus();

    virtual status_t                            cancelAutoFocus();

    virtual status_t                            precapture(int& flashRequired);

    virtual status_t                            takePicture();

    virtual status_t                            setParameters();

    virtual status_t                            sendCommand(
                                                    int32_t cmd,
                                                    int32_t arg1,
                                                    int32_t arg2
                                                );

public:

    virtual status_t                            dump(
                                                    int fd,
                                                    Vector<String8>const& args
                                                );

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  IRequestUpdater Interface.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:

    virtual MERROR                              updateRequestSetting(
                                                    IMetadata* appSetting,
                                                    IMetadata* halSetting
                                                );

    virtual MERROR                              updateParameters(
                                                    IMetadata* setting
                                                );

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  IFeatureFlowControl Interface.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:

    virtual MERROR                  submitRequest(
                                        Vector< SettingSet >          vSettings,
                                        Vector< StreamId >            vDstStreams,
                                        Vector< MINT32 >&             vRequestNo
                                    );

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  RefBase Interface.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
    virtual MVOID                               onLastStrongRef( const void* /*id*/);
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Implementations.(nested calsses)
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
    class StereoRequestUpdater
        : public virtual android::RefBase
        , public IRequestUpdater
    {
        public:
            //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //  IRequestUpdater Interface.
            //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            virtual MERROR                          updateRequestSetting(
                                                        IMetadata* appSetting,
                                                        IMetadata* halSetting
                                                    );

            virtual MERROR                          updateParameters(
                                                        IMetadata* setting
                                                    );
            //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //  StereoRequestUpdater Interface.
            //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            struct StreamType{
                enum {
                    RESIZED,
                    FULL,
                    RESIZED_MAIN2,
                    FULL_MAIN2
                };
            };
                                                    StereoRequestUpdater(
                                                        sp<StereoFlowControl>  pStereoFlowControl,
                                                        sp<IParamsManager> pParamsManager,
                                                        sp<StereoBufferSynchronizer> pStereoBufferSynchronizer
                                                    );

            virtual MVOID                           onLastStrongRef( const void* /*id*/);

            virtual MERROR                          addSelector(sp<StereoSelector> pSelector, MINT32 streamType);

            virtual MERROR                          addPool(sp<StereoBufferPool> pPool, MINT32 streamType);

            virtual MERROR                          setSensorParams(StereoPipelineSensorParam main1, StereoPipelineSensorParam main2);

            //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            //  StereoRequestUpdater data members
            //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        protected:
            sp<StereoSelector>                                 mSelector_resized;
            sp<StereoSelector>                                 mSelector_full;
            sp<StereoSelector>                                 mSelector_resized_main2;
            sp<StereoSelector>                                 mSelector_full_main2;

            sp<StereoBufferPool>                               mPool_resized;
            sp<StereoBufferPool>                               mPool_full;
            sp<StereoBufferPool>                               mPool_resized_main2;
            sp<StereoBufferPool>                               mPool_full_main2;

            StereoPipelineSensorParam                          mSensorParam;
            StereoPipelineSensorParam                          mSensorParam_main2;

            sp<StereoFlowControl>                              mpStereoFlowControl;
            sp<IParamsManager>                                 mpParamsManager;
            sp<StereoBufferSynchronizer>                       mpStereoBufferSynchronizer;

            int                                                mReqNo;
            int                                                mLogLevel;
    };

    class StereoMain2RequestUpdater
        :public StereoRequestUpdater
    {
        public:
                                                    StereoMain2RequestUpdater(
                                                        sp<StereoFlowControl>  pStereoFlowControl,
                                                        sp<IParamsManager> pParamsManager,
                                                        sp<StereoBufferSynchronizer> pStereoBufferSynchronizer
                                                    );

            virtual MERROR                          updateRequestSetting(
                                                        IMetadata* appSetting,
                                                        IMetadata* halSetting
                                                    );

    };
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Implementations.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
    MERROR                                      setCamClient(
                                                    char* const name,
                                                    StreamId streamId,
                                                    Vector<PipelineImageParam>& vImageParam,
                                                    Vector<MUINT32> clientMode
                                                );

    MERROR                                      checkNotifyCallback(
                                                    IMetadata* meta
                                                );

    int32_t                                     getOpenId_Main2() const;

    int32_t                                     getOpenId_P2Prv() const;

    MRect                                       getActiveArrayCrop() const {return mActiveArrayCrop;};

    MRect                                       getActiveArrayCrop_Main2() const {return mActiveArrayCrop_Main2;};

    MRect                                       getSensorDomainCrop() const {return mSensorDomainCrop;};

    MRect                                       getSensorDomainCrop_Main2() const {return mSensorDomainCrop_Main2;};

    MBOOL                                       waitP2PrvReady();
private:
    sp<ILegacyPipeline>                         constructP1Pipeline_Main1(
                                                    const MetaStreamManager::metadata_info_setting* metaTable,
                                                    const MINT32* nodeConfigData,
                                                    ImageStreamManager::image_stream_info_pool_setting* imageTable
                                                );

    sp<ILegacyPipeline>                         constructP1Pipeline_Main2(
                                                    const MetaStreamManager::metadata_info_setting* metaTable,
                                                    const MINT32* nodeConfigData,
                                                    ImageStreamManager::image_stream_info_pool_setting* imageTable
                                                );

    sp<ILegacyPipeline>                         constructP2Pipeline_Prv(
                                                    const MetaStreamManager::metadata_info_setting* metaTable,
                                                    const MINT32* nodeConfigData,
                                                    ImageStreamManager::image_stream_info_pool_setting* imageTable
                                                );

    sp<ILegacyPipeline>                         constructP2Pipeline_Cap(
                                                    const MetaStreamManager::metadata_info_setting* metaTable,
                                                    const MINT32* nodeConfigData,
                                                    ImageStreamManager::image_stream_info_pool_setting* imageTable
                                                );
    MINT                                        getImageStreamIndex(
                                                    ImageStreamManager::image_stream_info_pool_setting* pTable,
                                                    NSCam::v3::StreamId_T id
                                                );
    MERROR                                      updateRrzoInfo(
                                                    MBOOL bMain1,
                                                    ImageStreamManager::image_stream_info_pool_setting* pImgTable,
                                                    MUINT32 mode
                                                );
    MERROR                                      updateImgoInfo(
                                                    MBOOL bMain1,
                                                    ImageStreamManager::image_stream_info_pool_setting* pImgTable,
                                                    MUINT32 mode
                                                );
    MUINT32                                     prepareSensor(MUINT sensorId, StereoPipelineSensorParam& sensorParam);
    virtual MINT32                              getScenario() const;
    virtual android::status_t                   changeToPreviewStatus();

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Data Members.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
protected:
    enum {
        PipelineMode_PREVIEW,
        PipelineMode_RECORDING,
        PipelineMode_ZSD
    };

    char*                                       mName;
    int                                         mLogLevel;
    MINT32                                      mOpenId;
    MINT32                                      mOpenId_main2;
    MINT32                                      mOpenId_P2Prv;
    MINT32                                      mPipelineMode;

    // Pipelines
    // This pipeline consists of DepthNode and BokehNode
    sp< ILegacyPipeline >                       mpPipeline;
    // This pipeline consists of P1Node
    sp< ILegacyPipeline >                       mpPipeline_P1;
    // This pipeline consists of P1Node_Main2
    sp< ILegacyPipeline >                       mpPipeline_P1_Main2;

    // RequestControllers. There are three requestController for each pipeline
    sp<IRequestController>                      mpRequestController;
    sp<IRequestController>                      mpRequestController_P1;
    sp<IRequestController>                      mpRequestController_P1_Main2;
    // This updater is used by mpPipeline, it will block request thread and try to get meta and buffer from consumers
    sp<StereoRequestUpdater>                    mpStereoRequestUpdater;
    sp<StereoMain2RequestUpdater>               mpStereoRequestUpdater_Main2;

    sp<IParamsManager>                          mpParamsManager;
    sp<IParamsManagerV3>                        mpParamsManagerV3;
    sp<ImgBufProvidersManager>                  mpImgBufProvidersMgr;
    StereoPipelineSensorParam                   mSensorParam;
    StereoPipelineSensorParam                   mSensorParam_main2;
    sp<INotifyCallback>                         mpCamMsgCbInfo;

    sp<IResourceContainer>                      mpResourceContainier;
    sp<IResourceContainer>                      mpResourceContainierMain2;
    sp<IResourceContainer>                      mpResourceContainierP2Prv;

    sp<StereoSelector>                          mpStereoSelector_RESIZER;
    sp<StereoSelector>                          mpStereoSelector_RESIZER_MAIN2;
    sp<StereoSelector>                          mpStereoSelector_OPAQUE;
    sp<StereoSelector>                          mpStereoSelector_OPAQUE_MAIN2;

    sp<StereoBufferSynchronizer>                mpStereoBufferSynchronizer;

    sp<StereoBufferPool>                        mpStereoBufferPool_RESIZER;
    sp<StereoBufferPool>                        mpStereoBufferPool_RESIZER_MAIN2;
    sp<StereoBufferPool>                        mpStereoBufferPool_OPAQUE;
    sp<StereoBufferPool>                        mpStereoBufferPool_OPAQUE_MAIN2;

    MBOOL                                       mbDVFSLevel;
    MBOOL                                       mbEnableP2Prv;

    BWC_PROFILE_TYPE                            mBWCtype;

    MRect                                       mActiveArrayCrop;
    MRect                                       mActiveArrayCrop_Main2;

    MRect                                       mSensorDomainCrop;
    MRect                                       mSensorDomainCrop_Main2;

    mutable Mutex                               mP2PrvLock;
    mutable Condition                           mCondP2PrvLock;
};

/******************************************************************************
*
******************************************************************************/
};  //namespace NSPipelineContext
};  //namespace v1
};  //namespace NSCam
#endif  //_MTK_HARDWARE_MTKCAM_ADAPTER_STEREO_STEREOFLOWCONTROL_H_

