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

#ifndef _MTK_HARDWARE_MTKCAM_V1_STEREO_BUFFER_SYNCHRONIZER_H_
#define _MTK_HARDWARE_MTKCAM_V1_STEREO_BUFFER_SYNCHRONIZER_H_

namespace NSCam {
namespace v1 {
namespace NSLegacyPipeline{

class StereoSelector;

class StereoBufferSynchronizer
	: public virtual android::RefBase
{
public:
	static android::sp<StereoBufferSynchronizer>
                                   		createInstance( char const* szCallerName );

	// virtual                             ~StereoBufferSynchronizer() {};

	/**
 	*    add streams to syncronizer which enable the sync process of this stream buffer
 	*    MUST add streams before enque/deque
 	*/
	virtual		MERROR					addStream(
											StreamId_T      				streamId,
											sp<StereoSelector>  			rpSelector
										) = 0;

	virtual		MERROR					removeStream(
											StreamId_T      				streamId
										) = 0;

	/**
 	*    enqueBuffer from selector to pending pool.
 	*    It will trigger sync routine.
 	*/
	virtual		MERROR 					enqueBuffer(
											MINT32							rRequestNo,
											StreamId_T      				streamId,
											Vector<ISelector::MetaItemSet>  vResultMeta,
                                        	android::sp<IImageBufferHeap>   rpHeap
										) = 0;
	/**
 	*    dequeBuffer from available pool to selector.
 	*    The buffer dequed is syncronized.
 	*/
	virtual		MERROR					dequeBuffer(
											MINT32&							rRequestNo,
											StreamId_T      				streamId,
											Vector<ISelector::MetaItemSet>& rMeta,
                                        	android::sp<IImageBufferHeap>&  rpHeap
										) = 0;
	/**
 	*    lock/unlock synchronizer
 	*    MUST lock before trying to getResult from synchronizer
 	*/
	virtual		MERROR					lock() = 0;

	virtual		MERROR					unlock() = 0;

	/**
 	*    start synchronizer
 	*    MUST called AFTER all pass1 streams are added
 	*/
	virtual		MERROR					start() = 0;

	/**
 	*    flush synchronizer
 	*    all buffer will be returned to selectors
 	*/
	virtual		MERROR					flush() = 0;

    struct DEBUG_MODE{
	    enum {
	    	DEBUG_MODE_NONE = 0,
	        DEBUG_MODE_SKIP_TIMESTAMP_CHECK,
			DEBUG_MODE_SKIP_METADATA_CHECK
	    };
	};

	virtual     void                    setDebugMode(MINT32 debug) = 0;
};
/******************************************************************************
 *
 ******************************************************************************/
};  //namespace NSLegacyPipeline
};  //namespace v1
};  //namespace NSCam

#endif  //_MTK_HARDWARE_MTKCAM_V1_STEREO_BUFFER_SYNCHRONIZER_H_

