/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2015. All rights reserved.
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

#ifndef _MTK_CAMERA_CAMADAPTER_SCENARIO_SHOT_HDRSHOT_H_
#define _MTK_CAMERA_CAMADAPTER_SCENARIO_SHOT_HDRSHOT_H_

#include <BuiltinTypes.h>
#include <Errors.h>

#include <Shot/IShot.h>
#include <ImpShot.h>
#include <LegacyPipeline/processor/ResultProcessor.h>

#include <camshot/_params.h>
#include <camshot/BufferCallbackHandler.h>

#include <common/include/metadata/IMetadata.h>

#include <LegacyPipeline/buffer/StreamBufferProvider.h>

#include <semaphore.h>

#include <vector>

// ---------------------------------------------------------------------------

namespace NSCam {
namespace v1 {
namespace NSLegacyPipeline {
    class ILegacyPipeline;
    class IConsumerPool;
}
}
}

// ---------------------------------------------------------------------------

namespace android {
namespace NSShot {

template<typename T>
class LifeSpanObserver final
{
public:
    LifeSpanObserver(const wp<T>& object);
    virtual ~LifeSpanObserver();

private:
    wp<T> mObject;
};

// ---------------------------------------------------------------------------

class HDRShot final
    : public ImpShot,
      public NSCam::v1::ResultProcessor::IListener,
      public NSCam::v1::IImageCallback
{
public:
    HDRShot(const char *pszShotName,
            uint32_t const u4ShotMode,
            int32_t const i4OpenId);

    virtual ~HDRShot();

    // interface of ImpShot
    void onDestroy() {};

    // interface of ResultProcessor::IListener
    void onResultReceived(
            MUINT32    const requestNo,
            StreamId_T const streamId,
            MBOOL      const errorResult,
            IMetadata  const result);
    void onFrameEnd(MUINT32 const requestNo) {};
    String8 getUserName();

    // interface of IImageCallback
    MERROR onResultReceived(
            MUINT32    const requestNo,
            StreamId_T const streamId,
            MBOOL      const errorBuffer,
            sp<IImageBuffer>& pBuffer);

    // interface of HDRShot
    bool normalCapture();
    bool zsdCapture();

    // statistics for LifeSpanObserver
    static Mutex sInstanceLock;
    static MINT32 sInstanceCount;

private:
    struct SensorSetting
    {
        // sensor size
        MSize size;
        // frame rate per second
        MINT32 fps;
        // pixelMode
        MUINT32 pixelMode;
    };

    // sequence number for debug purpose
    static MINT32 sSequence;
    MINT32 mSequence;

    // shot parameters
    NSCamShot::SensorParam mSensorParam;

    SensorSetting mSensorSetting;

    sp<IImageStreamInfo>   mpImageInfo_fullRaw;
    sp<IImageStreamInfo>   mpImageInfo_resizedRaw;
    sp<IImageStreamInfo>   mpImageInfo_yuvJpeg;
    sp<IImageStreamInfo>   mpImageInfo_yuvPostview;
    sp<IImageStreamInfo>   mpImageInfo_yuvThumbnail;
    sp<IImageStreamInfo>   mpImageInfo_jpeg;

    sp<NSCam::v1::NSLegacyPipeline::ILegacyPipeline> mpPipeline;

    sp<NSCam::v1::BufferCallbackHandler> mpCallbackHandler;

    // bitset of data message types
    MINT32 mDataMsgSet;

    // used by ZSD mode
    sp<NSCam::v1::CallbackBufferPool>   mpP1Pool;
    std::vector< sp<IImageBuffer> >     mpSelectedBuffer;
    sp<NSCam::v1::StreamBufferProvider> mpBufferProvider;
    sp<NSCam::v1::ISelector>            mOriginalSelector;

    // condition for capture done event
    static Mutex sCaptureDoneLock;
    static sem_t sSemCaptureDone;

    // user control functions
    bool sendCommand(
            uint32_t const cmd,
            MUINTPTR const arg1,
            uint32_t const arg2,
            uint32_t const arg3 = 0);

    bool onCmd_reset();
    bool onCmd_capture();
    void onCmd_cancel();

    MUINT32 getRotation() const;

    status_t createStreams(
            const sp<NSCam::v1::StreamBufferProvider>& provider = NULL);
    status_t createPipeline(
            const sp<NSCam::v1::StreamBufferProvider>& provider = NULL);

    status_t getSelectResult(
            const sp<NSCam::v1::ISelector>& selector,
            IMetadata& appSetting,
            IMetadata& halSetting);

    status_t releaseResource();
};

} // namespace NSShot
} // namespace android

#endif // _MTK_CAMERA_CAMADAPTER_SCENARIO_SHOT_HDRSHOT_H_
