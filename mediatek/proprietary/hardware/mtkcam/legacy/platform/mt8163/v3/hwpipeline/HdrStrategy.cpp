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

#define LOG_TAG "MtkCam/Hwpipeline"
//
#include "MyUtils.h"
#include <mtkcam/v3/hwpipeline/HdrStrategy.h>
//
//#include <mtkcam/v3/hwnode/HDRNode.h>
//
using namespace android;
using namespace NSCam;
using namespace NSCam::v3;
using namespace NSCam::v3::Pipeline::HdrStrategy;


/******************************************************************************
 *
 ******************************************************************************/
#define MY_LOGV(fmt, arg...)        CAM_LOGV("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_LOGD("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_LOGI("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_LOGW("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_LOGE("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGA(fmt, arg...)        CAM_LOGA("[%s] " fmt, __FUNCTION__, ##arg)
#define MY_LOGF(fmt, arg...)        CAM_LOGF("[%s] " fmt, __FUNCTION__, ##arg)
//
#define MY_LOGV_IF(cond, ...)       do { if ( (cond) ) { MY_LOGV(__VA_ARGS__); } }while(0)
#define MY_LOGD_IF(cond, ...)       do { if ( (cond) ) { MY_LOGD(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) ) { MY_LOGI(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) ) { MY_LOGW(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) ) { MY_LOGE(__VA_ARGS__); } }while(0)
#define MY_LOGA_IF(cond, ...)       do { if ( (cond) ) { MY_LOGA(__VA_ARGS__); } }while(0)
#define MY_LOGF_IF(cond, ...)       do { if ( (cond) ) { MY_LOGF(__VA_ARGS__); } }while(0)

#define _MULTIFRAME
/******************************************************************************
 *
 ******************************************************************************/
struct  evaluate_request::Imp
{
    typedef IPipelineBufferSetFrameControl                          PipelineFrameT;

    template <class _IStreamBufferT_>
    struct Trait
    {
        typedef _IStreamBufferT_                                    IStreamBufferT;
        typedef typename IStreamBufferT::IStreamInfoT               IStreamInfoT;
        typedef KeyedVector<StreamId_T, sp<IStreamInfoT> >          IStreamInfoMapT;
        typedef KeyedVector<StreamId_T, sp<IStreamBufferT> >        IStreamBufferMapT;
    };

    template <typename StreamId_T, class BufT, class InfoT>
    static
    void
    convert_BufMap_to_InfoMap(
        KeyedVector<StreamId_T, sp<BufT> > const& rBufMap,
        KeyedVector<StreamId_T, sp<InfoT> >&      rInfoMap
    )
    {
        rInfoMap.setCapacity(rBufMap.size());
        for (size_t i = 0; i < rBufMap.size(); i++) {
            StreamId_T const streamId = rBufMap.keyAt(i);
            InfoT* pStreamInfo = const_cast<InfoT*>(rBufMap.valueAt(i)->getStreamInfo());
            rInfoMap.add(streamId, pStreamInfo);
        }
    }


    MVOID
    dump(evaluate_request_pipeline::Params& rParams)
    {
        sp<IStreamInfoSetControl> pSInfoSet = rParams.pStreamInfoSet;
        struct
        {
            KeyedVector<StreamId_T, sp<IImageStreamInfo> > vAppImage;
            KeyedVector<StreamId_T, sp<IImageStreamInfo> > vHalImage;
            KeyedVector<StreamId_T, sp<IMetaStreamInfo > > vAppMeta;
            KeyedVector<StreamId_T, sp<IMetaStreamInfo > > vHalMeta;
        }
        aReqInfoSet =
        {
            vAppImage   : rParams.pStreamInfoSet->getAppImage(),
            vHalImage   : rParams.pStreamInfoSet->getHalImage(),
            vAppMeta    : rParams.pStreamInfoSet->getAppMeta(),
            vHalMeta    : rParams.pStreamInfoSet->getHalMeta(),
        };
        MY_LOGD("Lanjuice dump App/Img(%d), Hal/Img(%d), App/Meta(%d), Hal/Meta(%d)",
                aReqInfoSet.vAppImage.size(), aReqInfoSet.vHalImage.size(),
                aReqInfoSet.vAppMeta.size(),  aReqInfoSet.vHalMeta.size() );
    }


    MERROR
    operator()(Params& rParams, IEvaluator const* pEvaluator)
    {
        MY_LOGD("Lanjuice - 0");
        ////////////////////////////////////////////////////////////////////////
        //  Variable Declaration/Preparation.
        ////////////////////////////////////////////////////////////////////////
        struct
        {
        android::KeyedVector<StreamId_T, android::sp<IImageStreamBuffer> >& vIImageBuffers;
        android::KeyedVector<StreamId_T, android::sp<IImageStreamBuffer> >& vOImageBuffers;
        android::KeyedVector<StreamId_T, android::sp< IMetaStreamBuffer> >& vIMetaBuffers;
        android::KeyedVector<StreamId_T, android::sp< IMetaStreamBuffer> >& vOMetaBuffers;
        }
        rRequest =
        {
            *rParams.vIImageBuffers,
            *rParams.vOImageBuffers,
            *rParams.vIMetaBuffers,
            *rParams.vOMetaBuffers,
        };
        //
        MY_LOGD("Lanjuice - 1");
        //  [Pipeline Frame] create an instance; not assign content yet.
#ifdef _MULTIFRAME
        android::Vector< android::sp<PipelineFrameT> > vFrame;
        for ( int i=0; i<rParams.subFrameCnt; i++ ) {
             android::sp<PipelineFrameT> pFrame = PipelineFrameT::create(
                rParams.requestNo, i, rParams.pAppCallback, pEvaluator->getStreamBufferProvider()
            );
            pFrame->startConfiguration();
            vFrame.add(pFrame);
        }
#else
        sp<PipelineFrameT> pFrame = PipelineFrameT::create(
            rParams.requestNo, i, rParams.pAppCallback, pEvaluator->getStreamBufferProvider()
        );
        pFrame->startConfiguration();
#endif
        // FIX ME: Need to get HdrNode instance from nodemap and then get capture param
        //android::Vector< HDRNode::CaptureParam_T >* pvCapParamSet = HDRNode::getCaptureParamSet();
        MY_LOGD("Lanjuice - 2");
        //
        //  [App Meta Request/Control]
        IMetadata aAppMetaRequest;
        {
            IMetadata* data = rRequest.vIMetaBuffers[0]->tryReadLock(__FUNCTION__);
            aAppMetaRequest = *data;
            rRequest.vIMetaBuffers[0]->unlock(__FUNCTION__, data);
        }
        MY_LOGD("Lanjuice - 3");
        //
        ////////////////////////////////////////////////////////////////////////
        //  Evaluate REQUEST pipeline & App/Hal stream info set.
        //
        //  Input:
        //      App Image Streams:IN, OUT
        //      App Meta Streams :IN
        //
        //  Output:
        //      REQUEST pipeline & App/Hal stream info set.
        ////////////////////////////////////////////////////////////////////////
        RequestAppStreamsInfo aRequestAppStreamsParams;
        evaluate_request_pipeline::Params aEvaluateRequestPipelineParams;
        {
            convert_BufMap_to_InfoMap(rRequest.vIImageBuffers, aRequestAppStreamsParams.vIAppImageStreams);
            convert_BufMap_to_InfoMap(rRequest.vOImageBuffers, aRequestAppStreamsParams.vOAppImageStreams);
            convert_BufMap_to_InfoMap(rRequest.vIMetaBuffers,  aRequestAppStreamsParams.vIAppMetaStreams);
            //aRequestAppStreamsParams.vOAppMetaStreams
            //
            aEvaluateRequestPipelineParams.pNodeMap     = IPipelineFrameNodeMapControl::create();;
            aEvaluateRequestPipelineParams.pIAppMetaData= &aAppMetaRequest;
            aEvaluateRequestPipelineParams.pAppStreams  = &aRequestAppStreamsParams;
            //
            MERROR err = evaluate_request_pipeline()(aEvaluateRequestPipelineParams, pEvaluator);
            if  ( OK != err ) {
                MY_LOGE("evaluate_request_pipeline - err:%d(%s)", err, ::strerror(-err));
                return err;
            }
            dump(aEvaluateRequestPipelineParams);
        }

        MY_LOGD("Lanjuice - 4");
        //
        ////////////////////////////////////////////////////////////////////////
        //  Prepare stream buffers' users
        ////////////////////////////////////////////////////////////////////////
#ifndef _MULTIFRAME
        android::Vector< refine_meta_stream_buffer_params > vaRefineMetaStreamBufferParams;
#else
        refine_meta_stream_buffer_params aRefineMetaStreamBufferParams;
#endif
        //
        //  Hal Image Buffers   => lazy allocation
        //  App Image Buffers
        {
            // if request exists app image buffer, only one subframe owns app image buffer => add extra hal image to other subframes
            // H
            typedef HalImageStreamBuffer                        StreamBufferT;
            typedef Trait<StreamBufferT>                        TraitTHal;
            typedef typename TraitTHal::IStreamInfoT            IStreamInfoT;    // FIX ME
            typedef typename TraitTHal::IStreamInfoMapT         IStreamInfoMapHT;
            // A
            typedef IImageStreamBuffer                          IStreamBufferT;
            typedef Trait<IStreamBufferT>                       TraitTApp;
            typedef typename TraitTApp::IStreamInfoMapT         IStreamInfoMapAT; // FIX ME
            typedef typename TraitTApp::IStreamBufferMapT       IStreamBufferMapT;
            // H
            IStreamInfoMapHT const& rStreamMapHal = aEvaluateRequestPipelineParams.pStreamInfoSet->getHalImage();
            aRefineMetaStreamBufferParams.pHalImageStreams = &rStreamMapHal;
            // A
            IStreamInfoMapAT const& rStreamMapApp = aEvaluateRequestPipelineParams.pStreamInfoSet->getAppImage();
            aRefineMetaStreamBufferParams.pAppImageStreams = &rStreamMapApp;
            //
#ifdef _MULTIFRAME
//            for ( int i=0; i<1; i++ ) {
            for ( int i=0; i<rParams.subFrameCnt; i++ ) {
                android::sp<PipelineFrameT> pFrame = vFrame.editItemAt(i);
                //
                sp<PipelineFrameT::IMap<StreamBufferT> > pBufMapHal = pFrame->editMap_HalImage();
                sp<PipelineFrameT::IMap<IImageStreamBuffer> > pBufMapApp = pFrame->editMap_AppImage();
                //
                if (i==0) { // default: first subframe owns app image buffer
                    // H: ok
                    pBufMapHal->setCapacity(rStreamMapHal.size());
                    for (size_t i = 0; i < rStreamMapHal.size(); i++) {
                        sp<IStreamInfoT>const& pStreamInfo = rStreamMapHal[i];
                        pBufMapHal->add(pStreamInfo, NULL);
                    }
                    // A: ok
                    pBufMapApp->setCapacity(rStreamMapApp.size());
                    //rRequest.vInputImageBuffers   => allocated yet
                    {
                        IStreamBufferMapT const& rBufMapApp = rRequest.vIImageBuffers;
                        for (size_t i = 0; i < rBufMapApp.size(); i++) {
                            pBufMapApp->add(rBufMapApp[i]);
                        }
                    }
                    //rRequest.vOutputImageBuffers  => allocated yet
                    {
                        IStreamBufferMapT const& rBufMapApp = rRequest.vOImageBuffers;
                        for (size_t i = 0; i < rBufMapApp.size(); i++) {
                            pBufMapApp->add(rBufMapApp[i]);
                        }
                    }
                    //
                    MY_LOGD("Lanjuice subframe-%d: HalImg(%d) / AppImg(%d)",
                            i, pBufMapHal->size(), pBufMapApp->size());
                    //
                }else { // other subframes takes no app image buffer (preview&jpeg), append to hal buffer?
                    // H: ok
                    // FIX ME: this capacity might be recalculated
                    pBufMapHal->setCapacity(rStreamMapHal.size());
                    for (size_t i = 0; i < rStreamMapHal.size(); i++) {
                        sp<IStreamInfoT>const& pStreamInfo = rStreamMapHal[i];
                        MINT imgFormat = pStreamInfo->getImgFormat();
                        if ( imgFormat == eImgFmt_YUY2 )
                            continue;
                        pBufMapHal->add(pStreamInfo, NULL);
                    }
                    // A: ok
                    pBufMapApp->setCapacity(0);
                    //rRequest.vInputImageBuffers    => none
                    {
                        IStreamBufferMapT const& rBufMapApp = rRequest.vIImageBuffers;
                        MY_LOGE("Request might not include <IN> App/Img: %d", rBufMapApp.size());
                    }
                    //rRequest.vOutputImageBuffers  => none to duplicate <HDR(P2) node for display, Jpeg node>
                    {
                        IStreamBufferMapT const& rBufMapApp = rRequest.vOImageBuffers;
                        MY_LOGD("Request might include <Out> App/Img: %d, but only main-frame does", rBufMapApp.size());
                    }
                    //
                    MY_LOGD("Lanjuice subframe-%d: HalImg(%d) / AppImg(%d)",
                            i, pBufMapHal->size(), pBufMapApp->size());
                    //
                }
            }
#else
            //android::sp<PipelineFrameT> pFrame = vFrame.editItemAt(0);
            // H
            sp<PipelineFrameT::IMap<StreamBufferT> > pBufMapHal = pFrame->editMap_HalImage();
            pBufMapHal->setCapacity(rStreamMapHal.size());
            for (size_t i = 0; i < rStreamMapHal.size(); i++) {
                sp<IStreamInfoT>const& pStreamInfo = rStreamMapHal[i];
                pBufMapHal->add(pStreamInfo, NULL);
            }
            // A
            sp<PipelineFrameT::IMap<IImageStreamBuffer> > pBufMapApp = pFrame->editMap_AppImage();
            pBufMapApp->setCapacity(rStreamMapApp.size());
            //
            //rRequest.vInputImageBuffers   => allocated yet
            {
                IStreamBufferMapT const& rBufMapApp = rRequest.vIImageBuffers;
                for (size_t i = 0; i < rBufMapApp.size(); i++) {
                    pBufMapApp->add(rBufMapApp[i]);
                }
            }
            //
            //rRequest.vOutputImageBuffers  => allocated yet
            {
                IStreamBufferMapT const& rBufMapApp = rRequest.vOImageBuffers;
                for (size_t i = 0; i < rBufMapApp.size(); i++) {
                    pBufMapApp->add(rBufMapApp[i]);
                }
            }
            //
            MY_LOGD("Lanjuice frame: HalImg(%d) / AppImg(%d)",
                    pBufMapHal->size(), pBufMapApp->size());
            //
#endif
        }
        MY_LOGD("Lanjuice - 5");
        //
        MY_LOGD("Lanjuice - 6");
        //
        // if request exists app meta buffer, only one subframe owns app meta buffer => add extra hal meta to other subframes
        //  Hal Meta Buffers    => need allocating here
        //  App Meta Buffers
        {
            // H
            typedef HalMetaStreamBuffer                         StreamBufferT;
            typedef Trait<StreamBufferT>                        TraitTHal;
            typedef typename TraitTHal::IStreamInfoT            IStreamInfoHT;
            typedef typename TraitTHal::IStreamInfoMapT         IStreamInfoMapHT;
            // A
            typedef IMetaStreamBuffer                           IStreamBufferT;
            typedef Trait<IStreamBufferT>                       TraitTApp;
            typedef typename TraitTApp::IStreamInfoT            IStreamInfoAT;
            typedef typename TraitTApp::IStreamInfoMapT         IStreamInfoMapAT;
            typedef typename TraitTApp::IStreamBufferMapT       IStreamBufferMapT;
            // H
            IStreamInfoMapHT const& rStreamMapHal = aEvaluateRequestPipelineParams.pStreamInfoSet->getHalMeta();
            aRefineMetaStreamBufferParams.pHalMetaStreams = &rStreamMapHal;
            // A
            IStreamInfoMapAT const& rStreamMapApp = aEvaluateRequestPipelineParams.pStreamInfoSet->getAppMeta();
            aRefineMetaStreamBufferParams.pAppMetaStreams = &rStreamMapApp;
            //
#ifdef _MULTIFRAME
//            for ( int i=0; i<1; i++ ) {
            for ( int i=0; i<rParams.subFrameCnt; i++ ) {
                android::sp<PipelineFrameT> pFrame = vFrame.editItemAt(i);
                //
                sp<PipelineFrameT::IMap<StreamBufferT> >     pBufMapHal = pFrame->editMap_HalMeta();
                sp<PipelineFrameT::IMap<IMetaStreamBuffer> > pBufMapApp = pFrame->editMap_AppMeta();
                //
                if (i==0) { // default: first subframe owns app meta buffer
                    // H
                    pBufMapHal->setCapacity(rStreamMapHal.size());
                    aRefineMetaStreamBufferParams.vHalMetaBuffer.setCapacity(rStreamMapHal.size());
                    //
                    for (size_t i = 0; i < rStreamMapHal.size(); i++) {
                        sp<IStreamInfoHT>const& pStreamInfo = rStreamMapHal[i];
                        sp<StreamBufferT> pStreamBuffer     = HalMetaStreamBufferAllocatorT(pStreamInfo.get())();
                        MY_LOGE_IF(pStreamBuffer==0, "Fail to allocate Hal Meta Buffer %s", pStreamInfo->getStreamName());
                        //
                        pBufMapHal->add(pStreamBuffer);
                        //
                        aRefineMetaStreamBufferParams.vHalMetaBuffer.add(pStreamBuffer->getStreamInfo()->getStreamId(), pStreamBuffer);
                    }
                    // A
                    pBufMapApp->setCapacity(rStreamMapApp.size());
                    aRefineMetaStreamBufferParams.vAppMetaBuffer.setCapacity(rStreamMapApp.size());
                    //rRequest.vInputMetaBuffers    => allocated yet
                    {
                        IStreamBufferMapT const& rBufMapApp = rRequest.vIMetaBuffers;
                        for (size_t i = 0; i < rBufMapApp.size(); i++) {
                            pBufMapApp->add(rBufMapApp[i]);
                            //
                            aRefineMetaStreamBufferParams.vAppMetaBuffer.add(rBufMapApp[i]->getStreamInfo()->getStreamId(), rBufMapApp[i]);
                        }
                    }
                    //rRequest.vOutputMetaBuffers   => need allocating here
                    {
                        IStreamInfoMapAT const& rStreamMapOutApp = aEvaluateRequestPipelineParams.pAppStreams->vOAppMetaStreams;
                        //
                        rRequest.vOMetaBuffers.clear();
                        rRequest.vOMetaBuffers.setCapacity(rStreamMapOutApp.size());
                        //
                        for (size_t i = 0; i < rStreamMapOutApp.size(); i++) {
                            sp<IStreamInfoAT>  pStreamInfo   = rStreamMapOutApp[i];
                            sp<IStreamBufferT> pStreamBuffer = HalMetaStreamBufferAllocatorT(pStreamInfo.get())();
                            MY_LOGE_IF(pStreamBuffer==0, "Fail to allocate App Output Meta Buffer %s", pStreamInfo->getStreamName());
                            //
                            rRequest.vOMetaBuffers.add(pStreamInfo->getStreamId(), pStreamBuffer);
                            pBufMapApp->add(pStreamBuffer);
                            //
                            aRefineMetaStreamBufferParams.vAppMetaBuffer.add(pStreamBuffer->getStreamInfo()->getStreamId(), pStreamBuffer);
                        }
                    }
                }else { // other subframes takes no app image buffer (preview?), append to hal buffer?
                    // H
                    pBufMapHal->setCapacity(rStreamMapHal.size());
                    //aRefineMetaStreamBufferParams.vHalMetaBuffer.setCapacity(rStreamMapHal.size());
                    //
                    for (size_t i = 0; i < rStreamMapHal.size(); i++) {
                        sp<IStreamInfoHT>const& pStreamInfo = rStreamMapHal[i];
                        sp<StreamBufferT> pStreamBuffer     = HalMetaStreamBufferAllocatorT(pStreamInfo.get())();
                        MY_LOGE_IF(pStreamBuffer==0, "Fail to allocate Hal Meta Buffer %s", pStreamInfo->getStreamName());
                        //
                        pBufMapHal->add(pStreamBuffer);
                        //
                        //aRefineMetaStreamBufferParams.vHalMetaBuffer.add(pStreamBuffer->getStreamInfo()->getStreamId(), pStreamBuffer);
                    }
                    // A
                    pBufMapApp->setCapacity(0);
                    //aRefineMetaStreamBufferParams.vAppMetaBuffer.setCapacity(0);
                    //rRequest.vInputMetaBuffers    => create three HalMetaControl for P1
                    //rRequest.vOutputMetaBuffers   => need allocating here & callback to pipeline when work done
                }
                //
                MY_LOGD("Lanjuice subframe-%d: HalMeta(%d) / AppMeta(%d)",
                         i, pBufMapHal->size(), pBufMapApp->size());
                //
            }
#else
            //android::sp<PipelineFrameT> pFrame = vFrame.editItemAt(0);
            // H
            sp<PipelineFrameT::IMap<StreamBufferT> > pBufMapHal = pFrame->editMap_HalMeta();
            pBufMapHal->setCapacity(rStreamMapHal.size());
            aRefineMetaStreamBufferParams.vHalMetaBuffer.setCapacity(rStreamMapHal.size());
            //
            for (size_t i = 0; i < rStreamMapHal.size(); i++) {
                sp<IStreamInfoHT>const& pStreamInfo = rStreamMapHal[i];
                sp<StreamBufferT> pStreamBuffer     = HalMetaStreamBufferAllocatorT(pStreamInfo.get())();
                MY_LOGE_IF(pStreamBuffer==0, "Fail to allocate Hal Meta Buffer %s", pStreamInfo->getStreamName());
                //
                pBufMapHal->add(pStreamBuffer);
                //
                aRefineMetaStreamBufferParams.vHalMetaBuffer.add(pStreamBuffer->getStreamInfo()->getStreamId(), pStreamBuffer);
            }
            // A
            sp<PipelineFrameT::IMap<IMetaStreamBuffer> > pBufMapApp = pFrame->editMap_AppMeta();
            pBufMapApp->setCapacity(rStreamMapApp.size());
            aRefineMetaStreamBufferParams.vAppMetaBuffer.setCapacity(rStreamMapApp.size());
            //rRequest.vInputMetaBuffers    => allocated yet
            {
                IStreamBufferMapT const& rBufMapApp = rRequest.vIMetaBuffers;
                for (size_t i = 0; i < rBufMapApp.size(); i++) {
                    pBufMapApp->add(rBufMapApp[i]);
                    //
                    aRefineMetaStreamBufferParams.vAppMetaBuffer.add(rBufMapApp[i]->getStreamInfo()->getStreamId(), rBufMapApp[i]);
                }
            }
            //rRequest.vOutputMetaBuffers   => need allocating here
            {
                IStreamInfoMapAT const& rStreamMapOutApp = aEvaluateRequestPipelineParams.pAppStreams->vOAppMetaStreams;
                //
                rRequest.vOMetaBuffers.clear();
                rRequest.vOMetaBuffers.setCapacity(rStreamMapOutApp.size());
                //
                for (size_t i = 0; i < rStreamMapOutApp.size(); i++) {
                    sp<IStreamInfoAT>  pStreamInfo   = rStreamMapOutApp[i];
                    sp<IStreamBufferT> pStreamBuffer = HalMetaStreamBufferAllocatorT(pStreamInfo.get())();
                    MY_LOGE_IF(pStreamBuffer==0, "Fail to allocate App Output Meta Buffer %s", pStreamInfo->getStreamName());
                    //
                    rRequest.vOMetaBuffers.add(pStreamInfo->getStreamId(), pStreamBuffer);
                    pBufMapApp->add(pStreamBuffer);
                    //
                    aRefineMetaStreamBufferParams.vAppMetaBuffer.add(pStreamBuffer->getStreamInfo()->getStreamId(), pStreamBuffer);
                }
            }
            //
            MY_LOGD("Lanjuice frame: HalMeta(%d) / AppMeta(%d)",
                    pBufMapHal->size(), pBufMapApp->size());
            //
#endif
        }
        //
        MY_LOGD("Lanjuice - 7");
        //
        {
            MERROR err = pEvaluator->refineRequestMetaStreamBuffers(aRefineMetaStreamBufferParams);
            MY_LOGE_IF(
                OK !=err ,
                "[refineRequestMetaStreamBuffers] err:%d(%s)",
                err, ::strerror(-err)
            );
        }
        //
        MY_LOGD("Lanjuice - 8");
        //
        {
            MERROR err = pEvaluator->createHdrCaptureParams();
        }
        //
        ////////////////////////////////////////////////////////////////////////
        //  Evaluate Stream buffers' users
        ////////////////////////////////////////////////////////////////////////
        {
#ifdef _MULTIFRAME
//                        for ( int i=0; i<1; i++ ) {
            for ( int i=0; i<rParams.subFrameCnt; i++ ) {
                android::sp<PipelineFrameT> pFrame = vFrame.editItemAt(i);
                evaluate_request_buffer_users::Params params;
                params.pEvaluator       = pEvaluator;
                params.pPipelineDAG     = aEvaluateRequestPipelineParams.pPipelineDAG.get();
                params.pNodeMap         = aEvaluateRequestPipelineParams.pNodeMap.get();
                params.pBufferSet       = pFrame.get();
                MERROR err = evaluate_request_buffer_users()(params);
                if  ( OK != err ) {
                    MY_LOGE("evaluate_request_buffer_users: %d(%s)", err, ::strerror(-err));
                    return err;
                }
            }
#else
            evaluate_request_buffer_users::Params params;
            params.pEvaluator       = pEvaluator;
            params.pPipelineDAG     = aEvaluateRequestPipelineParams.pPipelineDAG.get();
            params.pNodeMap         = aEvaluateRequestPipelineParams.pNodeMap.get();
            params.pBufferSet       = pFrame.get();
            MERROR err = evaluate_request_buffer_users()(params);
            if  ( OK != err ) {
                MY_LOGE("evaluate_request_buffer_users: %d(%s)", err, ::strerror(-err));
                return err;
            }
#endif
        }
        //
        MY_LOGD("Lanjuice - 9");
        ////////////////////////////////////////////////////////////////////////
        //  Pipeline Frame: content assignment
        ////////////////////////////////////////////////////////////////////////
#ifdef _MULTIFRAME
                    for ( int i=0; i<1; i++ ) {
        //            for ( int i=0; i<rParams.subFrameCnt; i++ ) {

            android::sp<PipelineFrameT> pFrame = vFrame.editItemAt(i);
            pFrame->setPipelineNodeMap(aEvaluateRequestPipelineParams.pPipelineNodeMap);
            pFrame->setNodeMap      (aEvaluateRequestPipelineParams.pNodeMap);
            pFrame->setPipelineDAG  (aEvaluateRequestPipelineParams.pPipelineDAG);
            pFrame->setStreamInfoSet(aEvaluateRequestPipelineParams.pStreamInfoSet);
            pFrame->finishConfiguration();
            //
            rParams.vFrame->add(pFrame);
            //
        }
#else
        pFrame->setPipelineNodeMap(aEvaluateRequestPipelineParams.pPipelineNodeMap);
        pFrame->setNodeMap      (aEvaluateRequestPipelineParams.pNodeMap);
        pFrame->setPipelineDAG  (aEvaluateRequestPipelineParams.pPipelineDAG);
        pFrame->setStreamInfoSet(aEvaluateRequestPipelineParams.pStreamInfoSet);
        pFrame->finishConfiguration();
        //
        rParams.vFrame->add(pFrame);
        *rParams.ppFrame = pFrame;
        //
#endif
        return  OK;
    }
};


/******************************************************************************
 *
 ******************************************************************************/
MERROR
evaluate_request::
operator()(Params& rParams, IEvaluator const* pEvaluator)
{
    return Imp()(rParams, pEvaluator);
}


/******************************************************************************
 *
 ******************************************************************************/
struct  evaluate_request_pipeline::Imp
{
    MERROR
    operator()(Params& rParams, IEvaluator const* pEvaluator) const
    {
        ////////////////////////////////////////////////////////////////////////////
        //  Refine the given Request App (stream) params
        ////////////////////////////////////////////////////////////////////////////
// FIX ME: reference PipelineModel.DefaultStrategy.cpp
        pEvaluator->refineRequestAppParams(rParams);
//        MTK_STATISTICS_FACE_DETECT_MODE
//        MTK_CONTROL_SCENE_MODE

        ////////////////////////////////////////////////////////////////////////////
        //  Variables Initialization.
        ////////////////////////////////////////////////////////////////////////////

        sp<IPipelineNodeMapControl> const   pConfigNodeMap  = pEvaluator->getConfigPipelineNodeMap();
        sp<IPipelineDAG> const              pConfigDAG      = pEvaluator->getConfigPipelineDAG();
        typedef Vector<IPipelineDAG::NodeObj_T> ToposortT;
        ToposortT const&                    rToposort       = pConfigDAG->getToposort();

        rParams.pNodeMap->setCapacity(pConfigNodeMap->size());;

        //Request Stream Set
        rParams.pStreamInfoSet = IStreamInfoSetControl::create();
        struct
        {
            KeyedVector<StreamId_T, sp<IImageStreamInfo> >& vAppImage;
            KeyedVector<StreamId_T, sp<IImageStreamInfo> >& vHalImage;
            KeyedVector<StreamId_T, sp<IMetaStreamInfo > >& vAppMeta;
            KeyedVector<StreamId_T, sp<IMetaStreamInfo > >& vHalMeta;
        }
        aReq =
        {
            vAppImage   : rParams.pStreamInfoSet->editAppImage(),
            vHalImage   : rParams.pStreamInfoSet->editHalImage(),
            vAppMeta    : rParams.pStreamInfoSet->editAppMeta(),
            vHalMeta    : rParams.pStreamInfoSet->editHalMeta(),
        };

        //Request Node Set
        IPipelineDAG::NodeIdSet_T aReqNodeSet;
        aReqNodeSet.setCapacity(pConfigNodeMap->size());

        ////////////////////////////////////////////////////////////////////////////
        //  Move initial input value into request streams
        ////////////////////////////////////////////////////////////////////////////
        {
            //  aReq.vAppImage <- rParams.vIAppImageStreams
            //  aReq.vAppImage <- rParams.vOAppImageStreams
            aReq.vAppImage.setCapacity(
                rParams.pAppStreams->vIAppImageStreams.size() +
                rParams.pAppStreams->vOAppImageStreams.size()
            );
            for (size_t i = 0; i < rParams.pAppStreams->vIAppImageStreams.size(); i++) {
                aReq.vAppImage.add(
                    rParams.pAppStreams->vIAppImageStreams.keyAt(i),
                    rParams.pAppStreams->vIAppImageStreams.valueAt(i)
                );
            }
            for (size_t i = 0; i < rParams.pAppStreams->vOAppImageStreams.size(); i++) {
                aReq.vAppImage.add(
                    rParams.pAppStreams->vOAppImageStreams.keyAt(i),
                    rParams.pAppStreams->vOAppImageStreams.valueAt(i)
                );
            }
            //
            //  aReq.vAppMeta <- rParams.vIAppMetaStreams
            //  aReq.vAppMeta <- rParams.vOAppMetaStreams
            aReq.vAppMeta.setCapacity(
                aReqNodeSet.size() +
                rParams.pAppStreams->vIAppMetaStreams.size() +
                rParams.pAppStreams->vOAppMetaStreams.size()
            );
            for (size_t i = 0; i < rParams.pAppStreams->vIAppMetaStreams.size(); i++) {
                aReq.vAppMeta.add(
                    rParams.pAppStreams->vIAppMetaStreams.keyAt(i),
                    rParams.pAppStreams->vIAppMetaStreams.valueAt(i)
                );
            };
            for (size_t i = 0; i < rParams.pAppStreams->vOAppMetaStreams.size(); i++) {
                aReq.vAppMeta.add(
                    rParams.pAppStreams->vOAppMetaStreams.keyAt(i),
                    rParams.pAppStreams->vOAppMetaStreams.valueAt(i)
                );
            };
        }
        //
        ////////////////////////////////////////////////////////////////////////////
        //  Iterate each node according to the inverse order of toposort
        ////////////////////////////////////////////////////////////////////////////
        ToposortT::const_iterator it = rToposort.end();
        do
        {
            it--;
            //
            //Get streams of current node
            sp<IPipelineNodeMapControl::INode> node = pConfigNodeMap->getNodeAt(it->val);
            NodeId_T const nodeId = node->getNode()->getNodeId();
            struct
            {
                IStreamInfoSetControl::Map<IImageStreamInfo>const& vOAppImage;
                IStreamInfoSetControl::Map<IImageStreamInfo>const& vOHalImage;
                IStreamInfoSetControl::Map< IMetaStreamInfo>const& vOAppMeta;
                IStreamInfoSetControl::Map< IMetaStreamInfo>const& vOHalMeta;
                IStreamInfoSetControl::Map<IImageStreamInfo>const& vIAppImage;
                IStreamInfoSetControl::Map<IImageStreamInfo>const& vIHalImage;
                IStreamInfoSetControl::Map< IMetaStreamInfo>const& vIAppMeta;
                IStreamInfoSetControl::Map< IMetaStreamInfo>const& vIHalMeta;
            }
            nodeCfg =
            {
                vOAppImage  : node->getOutStreams()->getAppImage(),
                vOHalImage  : node->getOutStreams()->getHalImage(),
                vOAppMeta   : node->getOutStreams()->getAppMeta(),
                vOHalMeta   : node->getOutStreams()->getHalMeta(),
                vIAppImage  : node->getInStreams()->getAppImage(),
                vIHalImage  : node->getInStreams()->getHalImage(),
                vIAppMeta   : node->getInStreams()->getAppMeta(),
                vIHalMeta   : node->getInStreams()->getHalMeta(),
            };
            MY_LOGD("Lanjuice node config(%d): <In> AppMeta(%d)/HalMeta(%d)/AppImage(%d)/HalImage(%d)",
                    nodeId, nodeCfg.vIAppMeta.size(), nodeCfg.vIHalMeta.size(),
                            nodeCfg.vIAppImage.size(), nodeCfg.vIHalImage.size());
            MY_LOGD("Lanjuice node config(%d): <Out> AppMeta(%d)/HalMeta(%d)/AppImage(%d)/HalImage(%d)",
                    nodeId, nodeCfg.vOAppMeta.size(), nodeCfg.vOHalMeta.size(),
                            nodeCfg.vOAppImage.size(), nodeCfg.vOHalImage.size());

            //Node Request params (use to mark this node's streams)
            evaluate_node_io_streams_params nodeReq;

            //STEP (1)
            //Mark App:Image output streams if they belong to REQUEST App:Image output stream set.
            add_streams1_to_streams2_for_each_streams3_within_streams1(
                rParams.pAppStreams->vOAppImageStreams, nodeReq.vOAppImage, nodeCfg.vOAppImage
            );

            //STEP (2)
            //Mark App:Meta output streams if they belong to REQUEST stream set.
            //For example: face detection metadata
            add_streams1_to_streams2_for_each_streams3_within_streams1(
                aReq.vAppMeta, nodeReq.vOAppMeta, nodeCfg.vOAppMeta
            );

            //STEP (2)
            //Mark Hal:Image output streams if they belong to REQUEST stream set.
            add_streams1_to_streams2_for_each_streams3_within_streams1(
                aReq.vHalImage, nodeReq.vOHalImage, nodeCfg.vOHalImage
            );

            //STEP (2)
            //Mark Hal:Meta output streams if they belong to REQUEST stream set.
            add_streams1_to_streams2_for_each_streams3_within_streams1(
                aReq.vHalMeta, nodeReq.vOHalMeta, nodeCfg.vOHalMeta
            );

            //STEP (3)
            //Skip this node if there's no stream marked before.
            if  (
                    nodeReq.vOAppImage.isEmpty()
                &&  nodeReq.vOHalImage.isEmpty()
                &&  nodeReq.vOAppMeta.isEmpty()
                &&  nodeReq.vOHalMeta.isEmpty()
                )
            {
                CAM_LOGD_IF(0, "[evaluate_request_pipeline] Skip this node:%#"PRIxPTR, nodeId);
                continue;
            }

            //Here this node is necessary.
            FrameNodeMapT::INode* pNode = rParams.pNodeMap->getNodeAt( rParams.pNodeMap->addNode(it->id) );

            //add this node to REQUEST node set
            aReqNodeSet.add(it->id);


            //STEP (4)
            //Mark left App:Meta output streams and add them to REQUEST stream set.
#if 0       //covered via evaluateNodeIOStreams()
            for (size_t i = 0; i < nodeCfg.vOAppMeta.size(); i++)
            {
                StreamId_T const streamId = nodeCfg.vOAppMeta.keyAt(i);
                if  ( 0 > nodeReq.vOAppMeta.indexOfKey(streamId) )
                {
                    //Mark it
                    nodeReq.vOAppMeta.add(streamId, nodeCfg.vOAppMeta.valueAt(i));

                    //Add it into request streams
                    aReq.vAppMeta.add(streamId, nodeCfg.vOAppMeta.valueAt(i));
                    rParams.vOAppMetaStreams.add(streamId, nodeCfg.vOAppMeta.valueAt(i));
                }
            }
#endif

            //STEP (5)
            //Mark App:Image input streams if they belong to REQUEST stream set.
            //For example: P1 -> App:RAW_OPAQUE -> P2
            add_streams1_to_streams2_for_each_streams3_within_streams1(
                aReq.vAppImage, nodeReq.vIAppImage, nodeCfg.vIAppImage
            );

            //STEP (5)
            //Mark App:Meta input streams if they belong to REQUEST stream set.
#if 0       //covered via evaluateNodeIOStreams()
            add_streams1_to_streams2_for_each_streams3_within_streams1(
                aReq.vAppMeta, nodeReq.vIAppMeta, nodeCfg.vIAppMeta
            );
#endif

            //STEP (6)
            //By quering, mark the other (input) streams, and add them to REQUEST stream set.
            nodeReq.pIAppMetaData = rParams.pIAppMetaData;
            nodeReq.pIOMapSet = &pNode->editInfoIOMapSet();
            pEvaluator->evaluateNodeIOStreams(nodeId, nodeReq);
#if 0
            {
                InfoIOMapSet const& rIOMapSet = pNode->getInfoIOMapSet();
                CAM_LOGD("[evaluate_request_pipeline] IO Map~ nodeId:%#"PRIxPTR" Image Set #%zu Meta Set #%zu ", nodeId, rIOMapSet.mImageInfoIOMapSet.size(), rIOMapSet.mMetaInfoIOMapSet.size());
                Vector<String8> logs;
                pEvaluator->dump(logs, rIOMapSet);
                for (size_t j = 0; j < logs.size(); j++) {
                    CAM_LOGD(" ~ %s", logs[j].string());
                }
            }
#endif
            //
            add_streams1_to_streams2(nodeReq.vOAppMeta,  rParams.pAppStreams->vOAppMetaStreams);
            add_streams1_to_streams2(nodeReq.vOAppMeta,  aReq.vAppMeta);
            add_streams1_to_streams2(nodeReq.vIAppMeta,  aReq.vAppMeta);
            add_streams1_to_streams2(nodeReq.vOHalImage, aReq.vHalImage);
            add_streams1_to_streams2(nodeReq.vIHalImage, aReq.vHalImage);
            add_streams1_to_streams2(nodeReq.vOHalMeta,  aReq.vHalMeta);
            add_streams1_to_streams2(nodeReq.vIHalMeta,  aReq.vHalMeta);
#if 0
            add_streams1_to_streams2(nodeReq.vOAppImage, aReq.vAppImage);
            add_streams1_to_streams2(nodeReq.vIAppImage, aReq.vAppImage);
#endif

            //  Node I/O Streams.
            typedef NSCam::v3::Utils::SimpleStreamInfoSetControl StreamInfoSetT;
            {// I Streams.
                sp<StreamInfoSetT> pStreams = new StreamInfoSetT;
                pNode->setIStreams(pStreams);
                //
                StreamInfoSetT::Map<IMetaStreamInfo>& rMeta = pStreams->editMeta();
                add_streams1_to_streams2(nodeReq.vIAppMeta,  rMeta);
                add_streams1_to_streams2(nodeReq.vIHalMeta,  rMeta);
                //
                StreamInfoSetT::Map<IImageStreamInfo>& rImage = pStreams->editImage();
                add_streams1_to_streams2(nodeReq.vIAppImage, rImage);
                add_streams1_to_streams2(nodeReq.vIHalImage, rImage);
            }
            {// O Streams.
                sp<StreamInfoSetT> pStreams = new StreamInfoSetT;
                pNode->setOStreams(pStreams);
                //
                StreamInfoSetT::Map<IMetaStreamInfo>& rMeta = pStreams->editMeta();
                add_streams1_to_streams2(nodeReq.vOAppMeta,  rMeta);
                add_streams1_to_streams2(nodeReq.vOHalMeta,  rMeta);

                StreamInfoSetT::Map<IImageStreamInfo>& rImage = pStreams->editImage();
                add_streams1_to_streams2(nodeReq.vOAppImage, rImage);
                add_streams1_to_streams2(nodeReq.vOHalImage, rImage);
            }
            //
            MY_LOGD("Lanjuice node request(%d): <In> AppMeta(%d)/HalMeta(%d)/AppImage(%d)/HalImage(%d)",
                    nodeId, nodeReq.vIAppMeta.size(), nodeReq.vIHalMeta.size(),
                            nodeReq.vIAppImage.size(), nodeReq.vIHalImage.size());
            MY_LOGD("Lanjuice node request(%d): <Out> AppMeta(%d)/HalMeta(%d)/AppImage(%d)/HalImage(%d)",
                    nodeId, nodeReq.vOAppMeta.size(), nodeReq.vOHalMeta.size(),
                            nodeReq.vOAppImage.size(), nodeReq.vOHalImage.size());
        }   while (it != rToposort.begin());

        //After finishing all nodes...
        //Check to see whether root node exist or not
        IPipelineDAG::NodeObj_T const aConfigRootNode = pConfigDAG->getRootNode();
        if  ( 0 > aReqNodeSet.indexOf(aConfigRootNode.id) ) {
            MY_LOGW("REQUEST nodes contain no CONFIG root node:%#"PRIxPTR, aConfigRootNode.id);
            MY_LOGW("REQUEST nodes #%zu, in terms of:", aReqNodeSet.size());
            for (size_t i = 0; i < aReqNodeSet.size(); i++)
            {
                NodeId_T const nodeId = aReqNodeSet[i];
                FrameNodeMapT::INode* pNode = rParams.pNodeMap->getNodeFor(nodeId);
                CAM_LOGW(" nodeId:%#"PRIxPTR" IO Map:", nodeId);
                InfoIOMapSet const& rIOMapSet = pNode->getInfoIOMapSet();
                Vector<String8> logs;
                pEvaluator->dump(logs, rIOMapSet);
                for (size_t j = 0; j < logs.size(); j++) {
                    CAM_LOGW(" ~ %s", logs[j].string());
                }
            }
            //
            MY_LOGW("Force to add root node:%#"PRIxPTR, aConfigRootNode.id);
            aReqNodeSet.add(aConfigRootNode.id);
        }

        {
            sp<IPipelineDAG> pRequestDAG = pConfigDAG->clone(aReqNodeSet);
            if  ( pRequestDAG == 0 ) {
                MY_LOGE("fail to clone Request DAG");
                return UNKNOWN_ERROR;
            }

            //Check whether the new DAG is reachable from root
            {
                IPipelineDAG::NodeIdSet_T orphanNodes;
                IPipelineDAG::NodeIdSet_T checkList;
                pRequestDAG->getOrphanNodes(orphanNodes, checkList);
                if ( ! orphanNodes.isEmpty() )
                {
                    MY_LOGW("Request DAG is not connected");
                    pConfigDAG->getNodesAndPathsForNewDAG(orphanNodes, checkList, pRequestDAG);
                }
            }

            //Prepare to return...
            rParams.pPipelineDAG    = pRequestDAG;
            rParams.pPipelineNodeMap= pConfigNodeMap;
            ToposortT const& rRequestToposort = rParams.pPipelineDAG->getToposort();
            if  ( rRequestToposort.isEmpty() ) {
                MY_LOGE(
                    "It seemes that the connection of nodes cannot form a DAG for request..."
                    "TopoSort#:%zu",
                    rRequestToposort.size()
                );
            }
        }
        //
        return OK;
    }


    template <class Streams1T, class Streams2T, class Streams3T>
    static
    MERROR
    add_streams1_to_streams2_for_each_streams3_within_streams1(
        Streams1T const& streams1,
        Streams2T& streams2,
        Streams3T const& streams3
    )
    {
        for (size_t i = 0; i < streams3.size(); i++)
        {
            StreamId_T const streamId = streams3.keyAt(i);
            ssize_t const index1 = streams1.indexOfKey(streamId);
            if  ( 0 <= index1 )
            {
                if  ( 0 > streams2.add(streamId, streams1.valueAt(index1)) ) {
                    MY_LOGE("Not enough memory");
                    return NO_MEMORY;
                }
            }
        }
        //
        return OK;
    }


    template <class Streams1T, class Streams2T>
    static
    MERROR
    add_streams1_to_streams2(
        Streams1T const& streams1,
        Streams2T& streams2
    )
    {
        for (size_t i = 0; i < streams1.size(); i++)
        {
            StreamId_T const streamId = streams1.keyAt(i);
            if  ( 0 > streams2.indexOfKey(streamId) )
            {
                if  ( 0 > streams2.add(streamId, streams1.valueAt(i)) ) {
                    MY_LOGE("Not enough memory");
                    return NO_MEMORY;
                }
            }
        }
        //
        return OK;
    }
};


/******************************************************************************
 *
 ******************************************************************************/
MERROR
evaluate_request_pipeline::
operator()(
    Params& rParams,
    IEvaluator const* pEvaluator
)
{
    return Imp()(rParams, pEvaluator);
}


/******************************************************************************
 *
 ******************************************************************************/
struct  evaluate_request_buffer_users::Imp
{
    typedef IPipelineFrameNodeMapControl    FrameNodeMapT;
    typedef Vector<IPipelineDAG::NodeObj_T> ToposortT;
    IPipelineDAG const*                     mpPipelineDAG;
    Vector<IPipelineDAG::Edge>              mvEdges;

    MERROR
    operator()(Params& rParams)
    {
        mpPipelineDAG   = rParams.pPipelineDAG;
        mvEdges.clear();
        rParams.pPipelineDAG->getEdges(mvEdges);
        //
        evaluateAppImage(rParams);
        evaluateHalImage(rParams);
        evaluateAppMeta (rParams);
        evaluateHalMeta (rParams);
        //
        return  OK;
    }

#define _IMP_EVALUATE_(_NAME_, _TYPE_) \
    MERROR \
    evaluate##_NAME_##_TYPE_(Params& rParams) \
    { \
        MakeUser_##_NAME_##_TYPE_ makeUser(rParams.pEvaluator, rParams.pNodeMap); \
        doIt(makeUser, rParams.pBufferSet->editMap_##_NAME_##_TYPE_()); \
        return OK; \
    }

    _IMP_EVALUATE_(App, Image);
    _IMP_EVALUATE_(App, Meta);
    _IMP_EVALUATE_(Hal, Image);
    _IMP_EVALUATE_(Hal, Meta);

#undef  _IMP_EVALUATE_

    template <class MakeUserT, class MapT>
    MVOID
    doIt(
        MakeUserT const& makeUser,
        MapT pBufMap
    )
    {
        ToposortT const& rToposort = mpPipelineDAG->getToposort();
        for (size_t i = 0; i < pBufMap->size(); i++)
        {
            sp<IUsersManager>const& pUsersManager = pBufMap->usersManagerAt(i);

            //User graph of current buffer
            sp<IUsersManager::IUserGraph> userGraph = pUsersManager->createGraph();

            //Add users
            ToposortT::const_iterator user = rToposort.begin();
            do
            {
                userGraph->addUser(makeUser(pBufMap->streamInfoAt(i), user->id));
                //
                user++;
            }  while ( user != rToposort.end() );

            //Add edges
            for (size_t j = 0; j < mvEdges.size(); j++) {
                userGraph->addEdge(mvEdges.itemAt(j).src, mvEdges.itemAt(j).dst);
            }

            //Enqueue graph
            pUsersManager->enqueUserGraph(userGraph);
            pUsersManager->finishUserSetup();
        }
    }

    struct  MakeUserBase
    {
        IEvaluator const*                   mpEvaluator;
        FrameNodeMapT const*                mpNodeMap;

        IUsersManager::User
        makeImageUser(
            IImageStreamInfo const* pStreamInfo,
            NodeId_T const nodeId
        )   const
        {
            StreamId_T const streamId = pStreamInfo->getStreamId();
            //
            IUsersManager::User user;
            user.mUserId = nodeId;
            //
            FrameNodeMapT::INode* pNode = mpNodeMap->getNodeFor(nodeId);
            refineUser(
                user, streamId,
                pNode->getOStreams()->getImageInfoMap(),
                pNode->getIStreams()->getImageInfoMap()
            );
            if  ( IUsersManager::Category::NONE != user.mCategory ) {
                user.mUsage = mpEvaluator->queryMinimalUsage(nodeId, pStreamInfo);
            }
            //
            return user;
        }

        IUsersManager::User
        makeMetaUser(
            IMetaStreamInfo const* pStreamInfo,
            NodeId_T const nodeId
        )   const
        {
            StreamId_T const streamId = pStreamInfo->getStreamId();
            //
            IUsersManager::User user;
            user.mUserId = nodeId;
            //
            FrameNodeMapT::INode* pNode = mpNodeMap->getNodeFor(nodeId);
            refineUser(
                user, streamId,
                pNode->getOStreams()->getMetaInfoMap(),
                pNode->getIStreams()->getMetaInfoMap()
            );
            //
            return user;
        }

        template <class StreamsT>
        MVOID
        refineUser(
            IUsersManager::User& rUser,
            StreamId_T const streamId,
            StreamsT const& pOStreams,
            StreamsT const& pIStreams
        )   const
        {
            if  ( pOStreams != 0 && pIStreams != 0 )
            {
                if  ( 0 <= pOStreams->indexOfKey(streamId) )
                {
                    rUser.mCategory = IUsersManager::Category::PRODUCER;
                    return;
                }
                //
                if  ( 0 <= pIStreams->indexOfKey(streamId) )
                {
                    rUser.mCategory = IUsersManager::Category::CONSUMER;
                    return;
                }
                //
                MY_LOGD_IF(
                    0,
                    "streamId:%#"PRIxPTR" nodeId:%#"PRIxPTR": not found in IO streams",
                    streamId, rUser.mUserId
                );
            }
            else
            {
                MY_LOGW_IF(
                    1,
                    "streamId:%#"PRIxPTR" nodeId:%#"PRIxPTR": no IO streams(%p,%p)",
                    streamId, rUser.mUserId, pIStreams.get(), pOStreams.get()
                );
            }
            //
            rUser.mCategory = IUsersManager::Category::NONE;
            rUser.mReleaseFence = rUser.mAcquireFence;
        }
    };

#define _DEFINE_MAKEUSER_(_NAME_, _TYPE_)                                   \
    struct  MakeUser_##_NAME_##_TYPE_                                       \
        : public MakeUserBase                                               \
    {                                                                       \
        MakeUser_##_NAME_##_TYPE_(                                          \
            IEvaluator const* pEvaluator,                                   \
            FrameNodeMapT const* pNodeMap                                   \
        )                                                                   \
        {                                                                   \
            mpEvaluator = pEvaluator;                                       \
            mpNodeMap   = pNodeMap;                                         \
        }                                                                   \
                                                                            \
        IUsersManager::User                                                 \
        operator()(                                                         \
            I##_TYPE_##StreamInfo const* pStreamInfo,                       \
            NodeId_T const nodeId                                           \
        )   const                                                           \
        {                                                                   \
            return make##_TYPE_##User(                                      \
                pStreamInfo,                                                \
                nodeId                                                      \
            );                                                              \
        }                                                                   \
    };

    _DEFINE_MAKEUSER_(App, Image);
    _DEFINE_MAKEUSER_(App, Meta);
    _DEFINE_MAKEUSER_(Hal, Image);
    _DEFINE_MAKEUSER_(Hal, Meta);

};


/******************************************************************************
 *
 ******************************************************************************/
MERROR
evaluate_request_buffer_users::
operator()(Params& rParams)
{
    return Imp()(rParams);
}

