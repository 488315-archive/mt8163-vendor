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

#define LOG_TAG "MtkCam/StereoBufferSynchronizer"
//
#include "MyUtils.h"
//
#include <sys/prctl.h>
#include <sys/resource.h>
#include <system/thread_defs.h>
#include <cutils/properties.h>
//
#include <v3/utils/streaminfo/MetaStreamInfo.h>
#include <v3/utils/streaminfo/ImageStreamInfo.h>
#include <v3/utils/streambuf/StreamBuffers.h>
//
#include <metadata/client/mtk_metadata_tag.h>
//
#include <LegacyPipeline/buffer/StreamBufferProvider.h>
#include <LegacyPipeline/stereo/buffer/StereoSelector.h>
#include <LegacyPipeline/stereo/buffer/StereoBufferSynchronizer.h>
// STL
#include <map>
#include <list>

using namespace std;
using namespace android;
using namespace NSCam;
using namespace NSCam::v1;

#define THREAD_NAME       ("Cam@StereoSynchronizer")
#define THREAD_POLICY     (SCHED_OTHER)
#define THREAD_PRIORITY   (0)


/******************************************************************************
 *
 ******************************************************************************/
#define MY_LOGV(fmt, arg...)        CAM_LOGV("[%s][%s] " fmt, getUserName(), __FUNCTION__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_LOGD("[%s][%s] " fmt, getUserName(), __FUNCTION__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_LOGI("[%s][%s] " fmt, getUserName(), __FUNCTION__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_LOGW("[%s][%s] " fmt, getUserName(), __FUNCTION__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_LOGE("[%s][%s] " fmt, getUserName(), __FUNCTION__, ##arg)
#define MY_LOGA(fmt, arg...)        CAM_LOGA("[%s][%s] " fmt, getUserName(), __FUNCTION__, ##arg)
#define MY_LOGF(fmt, arg...)        CAM_LOGF("[%s][%s] " fmt, getUserName(), __FUNCTION__, ##arg)
//
#define MY_LOGV_IF(cond, ...)       do { if ( (cond) ) { MY_LOGV(__VA_ARGS__); } }while(0)
#define MY_LOGD_IF(cond, ...)       do { if ( (cond) ) { MY_LOGD(__VA_ARGS__); } }while(0)
#define MY_LOGI_IF(cond, ...)       do { if ( (cond) ) { MY_LOGI(__VA_ARGS__); } }while(0)
#define MY_LOGW_IF(cond, ...)       do { if ( (cond) ) { MY_LOGW(__VA_ARGS__); } }while(0)
#define MY_LOGE_IF(cond, ...)       do { if ( (cond) ) { MY_LOGE(__VA_ARGS__); } }while(0)
#define MY_LOGA_IF(cond, ...)       do { if ( (cond) ) { MY_LOGA(__VA_ARGS__); } }while(0)
#define MY_LOGF_IF(cond, ...)       do { if ( (cond) ) { MY_LOGF(__VA_ARGS__); } }while(0)

#define MY_LOGW_NO_USERNAME(fmt, arg...)        CAM_LOGW("[%s][%s] " fmt, __FUNCTION__, ##arg)

#define MY_LOGD1(...)               MY_LOGD_IF(1<=mLogLevel, __VA_ARGS__)
#define MY_LOGD2(...)               MY_LOGD_IF(2<=mLogLevel, __VA_ARGS__)

#define FUNC_START     				MY_LOGD1("+")
#define FUNC_END       				MY_LOGD1("-")

#define AVA_QUE_KEEP_COUNT          2
#define PEN_QUE_KEEP_COUNT          2
#define SYNC_THRESHOLD_MS 			1
#define PAIR_THRESHOLD_MS 			32

namespace NSCam {
namespace v1 {
namespace NSLegacyPipeline{

typedef enum _SYNC_RESULT_
{
    SYNC_RESULT_PAIR_OK       = 0,
    SYNC_RESULT_MAIN1_TOO_OLD = 1,
    SYNC_RESULT_MAIN2_TOO_OLD = 2,
    SYNC_RESULT_PAIR_NOT_SYNC = 3
} SYNC_RESULT;

class StereoBufferSynchronizerImp
	: public StereoBufferSynchronizer
    , public Thread
{
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//  StereoBufferSynchronizer Interface.
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
	virtual		MERROR					addStream(
    											StreamId_T      				streamId,
    											sp<StereoSelector>  		    pSelector
    										);

	virtual		MERROR					removeStream(
    											StreamId_T      				streamId
    										);

	virtual		MERROR					enqueBuffer(
											MINT32                          reqNo,
											StreamId_T      				streamId,
											Vector<ISelector::MetaItemSet>  vResultMeta,
                                        	sp<IImageBufferHeap>   			pHeap
										);

	virtual		MERROR					dequeBuffer(
                                            MINT32&                         rRequestNo,
											StreamId_T      				streamId,
											Vector<ISelector::MetaItemSet>& rMeta,
                                        	sp<IImageBufferHeap>&   		rpHeap
										);

    virtual     MERROR                  lock();

    virtual     MERROR                  unlock();

    virtual     MERROR                  start();

    virtual     MERROR                  flush();

    virtual     void                    setDebugMode(MINT32 debug);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//  RefBase Interface.
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
    void                                onLastStrongRef(const void* /*id*/);
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //  Thread Interface.
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    public:    //// thread interface
    // Ask this object's thread to exit. This function is asynchronous, when the
    // function returns the thread might still be running. Of course, this
    // function can be called from a different thread.
    //virtual void        requestExit();

    // Good place to do one-time initializations
    virtual status_t    readyToRun();

private:
    // Derived class must implement threadLoop(). The thread starts its life
    // here. There are two ways of using the Thread object:
    // 1) loop: if threadLoop() returns true, it will be called again if
    //          requestExit() wasn't called.
    // 2) once: if threadLoop() returns false, the thread will exit upon return.
    virtual bool        threadLoop();
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//  Customized Data Types.
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    typedef struct
    {
        StreamId_T                          streamId;
    	MINT32								reqNo;
        sp<IImageBufferHeap>				heap;
        Vector<ISelector::MetaItemSet>   	metadata;
    }BUFFER_SET;
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//  Implementations.
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
                                        StereoBufferSynchronizerImp(char const* szCallerName);

                                        ~StereoBufferSynchronizerImp() {};

    char const*                         getUserName() {return mUserName;};
private:
	MBOOL    							isReadyToSyncCheck();

    MBOOL                               isReadyToLock();

    void                                syncRoutine(
                                            BUFFER_SET* bufInfoResized,
                                            BUFFER_SET* bufInfoResized_main2,
                                            BUFFER_SET* bufInfoFull = NULL,
                                            BUFFER_SET* bufInfoFull_main2 = NULL
                                        );

    SYNC_RESULT                         isTimeSync(
                                            BUFFER_SET* bufInfoMain1,
                                            BUFFER_SET* bufInfoMain2
                                        );

    void                                returnOldestPendingBufferToSelector(StreamId_T streamId);

    void                                returnOldestAvailableBufferToSelector(StreamId_T streamId);

    void                                checkPendingQueueSize(StreamId_T streamId);

    void                                checkAllPendingQueueSize();

    void                                checkAvailableQueueSize(StreamId_T streamId);

    void                                checkAllAvailableQueueSize();

	void								clearQueAndReturnBufferToSelector();

    void                                showPendingSizes();

    void                                showAvailableSizes();

    void                                returnBufferToSelector(BUFFER_SET* bufInfo);

    void                                returnBufferToPendingQue(BUFFER_SET* bufInfo);

    void                                enqueBufferToAvailableQue(BUFFER_SET* bufInfo);
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	//  Data members.
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	char                               	mUserName[64];
	int                                	mLogLevel;
    MINT32                              mDebugMode;
	MBOOL                               mbExitThread;
    MBOOL                               mIsLocked;

	mutable Mutex               		mLock;
	mutable Mutex               		mPendingPoolLock;
	mutable Mutex               		mAvailablePoolLock;
    mutable Condition                   mCondPushPendingPool;
	mutable Condition           		mCondPushAvailablePool;
    mutable Condition                   mCondLockAvailablePool;

	list<BUFFER_SET>       				pendingQueueResizeRaw;
    list<BUFFER_SET>       				pendingQueueFullRaw;
    list<BUFFER_SET>       				pendingQueueResizeRaw_main2;
    list<BUFFER_SET>       				pendingQueueFullRaw_main2;

	list<BUFFER_SET>       				availableQueueResizeRaw;
    list<BUFFER_SET>       				availableQueueFullRaw;
    list<BUFFER_SET>       				availableQueueResizeRaw_main2;
    list<BUFFER_SET>       				availableQueueFullRaw_main2;

    map< StreamId_T, list<BUFFER_SET>* > streamToPendingQue;
    map< StreamId_T, list<BUFFER_SET>* > streamToAvailableQue;
    typedef pair<StreamId_T, list<BUFFER_SET>* > PairStreamToBufferQue;

    map< StreamId_T, sp<StereoSelector> > streamToSelector;
    typedef pair< StreamId_T, sp<StereoSelector> > PairStreamToSelector;

    int                                 tempCounter;
    MINT32                              mTimestamp;
};

/******************************************************************************
 *
 ******************************************************************************/
template <typename T>
inline MBOOL
tryGetMetadata(
    IMetadata* pMetadata,
    MUINT32 const tag,
    T & rVal
)
{
    if( pMetadata == NULL ) {
        MY_LOGW_NO_USERNAME("pMetadata == NULL");
        return MFALSE;
    }

    IMetadata::IEntry entry = pMetadata->entryFor(tag);
    if( !entry.isEmpty() ) {
        rVal = entry.itemAt(0, Type2Type<T>());
        return MTRUE;
    }
    return MFALSE;
}

/******************************************************************************
 *
 ******************************************************************************/
sp<StereoBufferSynchronizer>
StereoBufferSynchronizer::
createInstance(
    char const* szCallerName
)
{
    return new StereoBufferSynchronizerImp( szCallerName );
}
/******************************************************************************
 *
 ******************************************************************************/
StereoBufferSynchronizerImp::
StereoBufferSynchronizerImp(char const* szCallerName)
	: mLogLevel(1)
#warning "log level for synchronizer set to 1"
    , mDebugMode(0)
    , tempCounter(0)
	, mbExitThread(MFALSE)
    , mIsLocked(MFALSE)
    , mTimestamp(0)
{
	strncpy(mUserName, szCallerName, 64);
    MY_LOGD("StereoBufferSynchronizerImp ctor");

	pendingQueueResizeRaw.clear();
	pendingQueueFullRaw.clear();
	pendingQueueResizeRaw_main2.clear();
	pendingQueueFullRaw_main2.clear();

	availableQueueResizeRaw.clear();
    availableQueueFullRaw.clear();
    availableQueueResizeRaw_main2.clear();
    availableQueueFullRaw_main2.clear();

    streamToPendingQue.clear();
    streamToAvailableQue.clear();
    streamToSelector.clear();

    char cLogLevel[PROPERTY_VALUE_MAX];
    ::property_get("debug.camera.log", cLogLevel, "0");
    mLogLevel = atoi(cLogLevel);
    if ( mLogLevel == 0 ) {
        ::property_get("debug.camera.log.stereosync", cLogLevel, "0");
        mLogLevel = atoi(cLogLevel);
    }

    MY_LOGD("PAIR_THRESHOLD_MS=%d  SYNC_THRESHOLD_MS=%d  mLogLevel=%d", PAIR_THRESHOLD_MS, SYNC_THRESHOLD_MS, mLogLevel);
}

/******************************************************************************
 *
 ******************************************************************************/
void
StereoBufferSynchronizerImp::
onLastStrongRef(const void* /*id*/)
{
    flush();
}
/******************************************************************************
 *
 ******************************************************************************/
MERROR
StereoBufferSynchronizerImp::
addStream(StreamId_T  streamId, sp<StereoSelector>	pSelector)
{
	FUNC_START;
	Mutex::Autolock _l(mLock);

	list<BUFFER_SET>* thePendingQue = NULL;
	list<BUFFER_SET>* theAvailableQue = NULL;

	switch(streamId){
        case eSTREAMID_IMAGE_PIPE_RAW_RESIZER:{
        	thePendingQue = &pendingQueueResizeRaw;
        	theAvailableQue = &availableQueueResizeRaw;
        	break;
        }
        case eSTREAMID_IMAGE_PIPE_RAW_OPAQUE:{
        	thePendingQue = &pendingQueueFullRaw;
        	theAvailableQue = &availableQueueFullRaw;
        	break;
        }
        case eSTREAMID_IMAGE_PIPE_RAW_RESIZER_01:{
        	thePendingQue = &pendingQueueResizeRaw_main2;
        	theAvailableQue = &availableQueueResizeRaw_main2;
        	break;
        }
        case eSTREAMID_IMAGE_PIPE_RAW_OPAQUE_01:{
        	thePendingQue = &pendingQueueFullRaw_main2;
        	theAvailableQue = &availableQueueFullRaw_main2;
        	break;
        }
        default:{
        	MY_LOGE("Unknown stream %#" PRIxPTR " . Should not have happended!", streamId);
        	return UNKNOWN_ERROR;
        }
    }

    if(thePendingQue == NULL){
    	MY_LOGE("thePendingQue == NULL for stream %#" PRIxPTR " . Should not have happended!", streamId);
    	return UNKNOWN_ERROR;
    }

    if(theAvailableQue == NULL){
    	MY_LOGE("theAvailableQue == NULL for stream %#" PRIxPTR " . Should not have happended!", streamId);
    	return UNKNOWN_ERROR;
    }

    if(pSelector == NULL){
    	MY_LOGE("pSelector == NULL for stream %#" PRIxPTR " . Should not have happended!", streamId);
    	return UNKNOWN_ERROR;
    }

    streamToPendingQue.insert(PairStreamToBufferQue(streamId, thePendingQue));
    streamToAvailableQue.insert(PairStreamToBufferQue(streamId, theAvailableQue));
    streamToSelector.insert(PairStreamToSelector(streamId, pSelector));

    pSelector->setSynchronizer(this, streamId);

    MY_LOGD("new stream %#" PRIxPTR " added", streamId);

    FUNC_END;
    return OK;
}
/******************************************************************************
 *
 ******************************************************************************/
MERROR
StereoBufferSynchronizerImp::
removeStream(StreamId_T  streamId)
{
	// TODO
	MY_LOGE("Hey, this function is not implemented yet!");
	return UNKNOWN_ERROR;
}
/******************************************************************************
 *
 ******************************************************************************/
MERROR
StereoBufferSynchronizerImp::
dequeBuffer(
                MINT32&                             rRequestNo,
				StreamId_T      					streamId,
				Vector<ISelector::MetaItemSet>&     rMeta,
                android::sp<IImageBufferHeap>&  	rpHeap
			)
{
	list<BUFFER_SET>* theAvailableQue = NULL;

	MY_LOGD1("dequeBuffer %#" PRIxPTR " + ", streamId);

	// find the available queue
	if(streamToAvailableQue.count(streamId)){
        theAvailableQue = streamToAvailableQue[streamId];
    }else{
        MY_LOGE("undefined streamId, should not have happended!");
        return NAME_NOT_FOUND;
    }

    // deque from theAvailableQue
    {
    	Mutex::Autolock _l(mAvailablePoolLock);
        if(theAvailableQue->empty()){
            MY_LOGE("try to deque but the size is 0, should not have happended! Did you call synchronizer->lock() before doing this?");
            return UNKNOWN_ERROR;
        }
		BUFFER_SET dequedSet = theAvailableQue->front();
		theAvailableQue->pop_front();
        rRequestNo = dequedSet.reqNo;
		rMeta = dequedSet.metadata;
		rpHeap = dequedSet.heap;
    }

    MY_LOGD1("dequeBuffer %#" PRIxPTR " - , rpHeap=%p", streamId, rpHeap.get());
	return OK;
}
/******************************************************************************
 *
 ******************************************************************************/
MERROR
StereoBufferSynchronizerImp::
enqueBuffer(
				MINT32								rRequestNo,
				StreamId_T      					streamId,
				Vector<ISelector::MetaItemSet>      vMeta,
            	android::sp<IImageBufferHeap>   	pHeap
			)
{
	list<BUFFER_SET>* thePendingQue = NULL;

    MY_LOGD1("req:%d, heap:%p, meta:%d stream:%#" PRIxPTR " , is enqued to StereoSynchronizer ", rRequestNo, pHeap.get(), vMeta.size());

	if(streamToPendingQue.count(streamId)){
        thePendingQue = streamToPendingQue[streamId];
    }else{
        MY_LOGE("undefined streamId, should not have happended!");
        return NAME_NOT_FOUND;
    }

    {
        // Push new buffer into pendingQueue
        Mutex::Autolock _l(mPendingPoolLock);

        BUFFER_SET newBuffer;
        newBuffer.reqNo = rRequestNo;
        newBuffer.heap = pHeap;
        newBuffer.metadata = vMeta;
        newBuffer.streamId = streamId;
        thePendingQue->push_back(newBuffer);

        mCondPushPendingPool.signal();
    }
	return OK;
}
/******************************************************************************
 *
 ******************************************************************************/
MBOOL
StereoBufferSynchronizerImp::
isReadyToSyncCheck()
{
    showPendingSizes();

    // check if there is anyone empty
    for(map<StreamId_T, list<BUFFER_SET>*>::iterator it=streamToPendingQue.begin(); it!=streamToPendingQue.end(); ++it){
        // if(it->second->empty()){
        //  MY_LOGD1("streamId%#" PRIxPTR " pending Queue is empty, buffer not ready");
        //  return MFALSE;
        // }

        // since mConsumerMap in PStreamBufferProviderImp is not updated yet
        // However we still not sure if consumer is alreay updated the mConsumerMap, use workaround temporaily
        // Workaround: dont do sync check until size >= 2
        if( it->second->size() < 2){
            MY_LOGD2("streamId%#" PRIxPTR " pending Queue size is < 2, buffer not ready", it->first);
            return MFALSE;
        }
    }
	return MTRUE;
}
/******************************************************************************
 *
 ******************************************************************************/
void
StereoBufferSynchronizerImp::
checkPendingQueueSize(StreamId_T streamId)
{
    FUNC_START;

    list<BUFFER_SET>* thePendingQue = NULL;

    // get front buffer from pending queue
    if(streamToPendingQue.count(streamId)){
        thePendingQue = streamToPendingQue[streamId];
    }else{
        FUNC_END;
        return;
    }

    if(thePendingQue->size() > PEN_QUE_KEEP_COUNT){
        MY_LOGW("thePendingQue.size=%d > PEN_QUE_KEEP_COUNT=%d return 1 buffer to selector",
            thePendingQue->size(),
            PEN_QUE_KEEP_COUNT
        );
        returnOldestPendingBufferToSelector(streamId);
    }

    FUNC_END;
}
/******************************************************************************
 *
 ******************************************************************************/
void
StereoBufferSynchronizerImp::
checkAllPendingQueueSize(){
    for(map<StreamId_T, list<BUFFER_SET>*>::iterator it=streamToPendingQue.begin(); it!=streamToPendingQue.end(); ++it){
        checkPendingQueueSize(it->first);
    }
}
/******************************************************************************
 *
 ******************************************************************************/
void
StereoBufferSynchronizerImp::
checkAvailableQueueSize(StreamId_T streamId)
{
    FUNC_START;

    list<BUFFER_SET>* theAvailableQue = NULL;

    // get front buffer from pending queue
    if(streamToAvailableQue.count(streamId)){
        theAvailableQue = streamToAvailableQue[streamId];
    }else{
        FUNC_END;
        return;
    }

    if(theAvailableQue->size() > AVA_QUE_KEEP_COUNT){
        MY_LOGW("theAvailableQue.size=%d > AVA_QUE_KEEP_COUNT=%d return 1 buffer to selector",
            theAvailableQue->size(),
            AVA_QUE_KEEP_COUNT
        );
        returnOldestAvailableBufferToSelector(streamId);
    }

    FUNC_END;
}
/******************************************************************************
 *
 ******************************************************************************/
void
StereoBufferSynchronizerImp::
checkAllAvailableQueueSize(){
    for(map<StreamId_T, list<BUFFER_SET>*>::iterator it=streamToAvailableQue.begin(); it!=streamToAvailableQue.end(); ++it){
        checkAvailableQueueSize(it->first);
    }
}
/******************************************************************************
 *
 ******************************************************************************/
void
StereoBufferSynchronizerImp::
returnOldestPendingBufferToSelector(StreamId_T streamId){
	FUNC_START;

	list<BUFFER_SET>* thePendingQue = NULL;

	// get front buffer from pending queue
	if(streamToPendingQue.count(streamId)){
        thePendingQue = streamToPendingQue[streamId];
    }else{
        return;
    }

    BUFFER_SET dequedSet = thePendingQue->front();
    thePendingQue->pop_front();

	// return to selector
	sp<StereoSelector> theSelector = streamToSelector[streamId];
	theSelector->returnBuffer(dequedSet.heap);

	FUNC_END;
}
/******************************************************************************
 *
 ******************************************************************************/
void
StereoBufferSynchronizerImp::
returnBufferToSelector(BUFFER_SET* bufInfo){
    if(bufInfo == NULL){
        return;
    }

    FUNC_START;

    // return to selector
    sp<StereoSelector> theSelector = streamToSelector[bufInfo->streamId];
    theSelector->returnBuffer(bufInfo->heap);

    FUNC_END;
}
/******************************************************************************
 *
 ******************************************************************************/
void
StereoBufferSynchronizerImp::
returnBufferToPendingQue(BUFFER_SET* bufInfo){
    if(bufInfo == NULL){
        return;
    }

    FUNC_START;

    list<BUFFER_SET>* thePendingQue = NULL;

    // get front buffer from pending queue
    if(streamToPendingQue.count(bufInfo->streamId)){
        thePendingQue = streamToPendingQue[bufInfo->streamId];
    }else{
        return;
    }

    {
        Mutex::Autolock _l(mPendingPoolLock);
        thePendingQue->push_front((*bufInfo));
    }

    FUNC_END;
}
/******************************************************************************
 *
 ******************************************************************************/
void
StereoBufferSynchronizerImp::
returnOldestAvailableBufferToSelector(StreamId_T streamId){
    FUNC_START;

    list<BUFFER_SET>* theAvailableQue = NULL;

    // get front buffer from pending queue
    if(streamToAvailableQue.count(streamId)){
        theAvailableQue = streamToAvailableQue[streamId];
    }else{
        return;
    }

    BUFFER_SET dequedSet = theAvailableQue->front();
    theAvailableQue->pop_front();

    // return to selector
    sp<StereoSelector> theSelector = streamToSelector[streamId];
    theSelector->returnBuffer(dequedSet.heap);

    FUNC_END;
}
/******************************************************************************
 *
 ******************************************************************************/
void
StereoBufferSynchronizerImp::
clearQueAndReturnBufferToSelector(){
	FUNC_START;

	//clear pendingQue
	{
		Mutex::Autolock _l(mPendingPoolLock);
		for(map<StreamId_T, list<BUFFER_SET>*>::iterator it=streamToPendingQue.begin(); it!=streamToPendingQue.end(); ++it){
        	StreamId_T theStreamId = it->first;
        	list<BUFFER_SET>* theQue = it->second;
        	sp<StereoSelector> theSelector = streamToSelector[theStreamId];

    		while(!theQue->empty()){
				BUFFER_SET dequedSet = theQue->front();
			    theQue->pop_front();

				// return to selector
				theSelector->returnBuffer(dequedSet.heap);
			}
    	}
	}

	//clear availableQue
	{
		Mutex::Autolock _l(mAvailablePoolLock);
		for(map<StreamId_T, list<BUFFER_SET>*>::iterator it=streamToAvailableQue.begin(); it!=streamToAvailableQue.end(); ++it){
        	StreamId_T theStreamId = it->first;
        	list<BUFFER_SET>* theQue = it->second;
        	sp<StereoSelector> theSelector = streamToSelector[theStreamId];

    		while(!theQue->empty()){
				BUFFER_SET dequedSet = theQue->front();
			    theQue->pop_front();

				// return to selector
				theSelector->returnBuffer(dequedSet.heap);
			}
    	}
	}

	FUNC_END;
}
/******************************************************************************
 *
 ******************************************************************************/
void
StereoBufferSynchronizerImp::
enqueBufferToAvailableQue(BUFFER_SET* bufInfo){
    if(bufInfo == NULL){
        return;
    }
    FUNC_START;

    list<BUFFER_SET>* theAvailableQue = NULL;
    if(streamToPendingQue.count(bufInfo->streamId)){
        theAvailableQue = streamToAvailableQue[bufInfo->streamId];
        theAvailableQue->push_back((*bufInfo));
    }else{
        MY_LOGE("pendingQue exist but availableQue not exist? should not have happended");
    }

    FUNC_END;
}
/******************************************************************************
 *
 ******************************************************************************/
void
StereoBufferSynchronizerImp::
showPendingSizes()
{
    MY_LOGD2("\n(rez/ful/rez_main2/ful_main2)\npendingQueue:  (%d/%d/%d/%d)",
        (streamToPendingQue.count(eSTREAMID_IMAGE_PIPE_RAW_RESIZER)         > 0 ? pendingQueueResizeRaw.size()              : -1),
        (streamToPendingQue.count(eSTREAMID_IMAGE_PIPE_RAW_OPAQUE)          > 0 ? pendingQueueFullRaw.size()                : -1),
        (streamToPendingQue.count(eSTREAMID_IMAGE_PIPE_RAW_RESIZER_01)      > 0 ? pendingQueueResizeRaw_main2.size()        : -1),
        (streamToPendingQue.count(eSTREAMID_IMAGE_PIPE_RAW_OPAQUE_01)       > 0 ? pendingQueueFullRaw_main2.size()          : -1)
    );
}
/******************************************************************************
 *
 ******************************************************************************/
void
StereoBufferSynchronizerImp::
showAvailableSizes()
{
    MY_LOGD2("\n(rez/ful/rez_main2/ful_main2)\navailableQueue:  (%d/%d/%d/%d)",
        (streamToAvailableQue.count(eSTREAMID_IMAGE_PIPE_RAW_RESIZER)       > 0 ? availableQueueResizeRaw.size()            : -1),
        (streamToAvailableQue.count(eSTREAMID_IMAGE_PIPE_RAW_OPAQUE)        > 0 ? availableQueueFullRaw.size()              : -1),
        (streamToAvailableQue.count(eSTREAMID_IMAGE_PIPE_RAW_RESIZER_01)    > 0 ? availableQueueResizeRaw_main2.size()      : -1),
        (streamToAvailableQue.count(eSTREAMID_IMAGE_PIPE_RAW_OPAQUE_01)     > 0 ? availableQueueFullRaw_main2.size()        : -1)
    );
}
/******************************************************************************
 *
 ******************************************************************************/
void
StereoBufferSynchronizerImp::
syncRoutine(
    BUFFER_SET* bufInfoResized,
    BUFFER_SET* bufInfoResized_main2,
    BUFFER_SET* bufInfoFull,
    BUFFER_SET* bufInfoFull_main2
){
    FUNC_START;

    SYNC_RESULT sync_result = isTimeSync(bufInfoResized, bufInfoResized_main2);

    switch(sync_result){
        case SYNC_RESULT_MAIN1_TOO_OLD:
            // Main1 too old
            // return main1 buffer to selector
            returnBufferToSelector(bufInfoResized);
            returnBufferToSelector(bufInfoFull);
            // return main2 buffer back to pending queue
            returnBufferToPendingQue(bufInfoResized_main2);
            returnBufferToPendingQue(bufInfoFull_main2);
            break;
        case SYNC_RESULT_MAIN2_TOO_OLD:
            // Main2 too old
            // return main2 buffer to selector
            returnBufferToSelector(bufInfoResized_main2);
            returnBufferToSelector(bufInfoFull_main2);
            // return main1 buffer back to pending queue
            returnBufferToPendingQue(bufInfoResized);
            returnBufferToPendingQue(bufInfoFull);
            break;
        case SYNC_RESULT_PAIR_NOT_SYNC:
            // This buffet pair is not sync.
            // return both to selector
            returnBufferToSelector(bufInfoResized);
            returnBufferToSelector(bufInfoFull);
            returnBufferToSelector(bufInfoResized_main2);
            returnBufferToSelector(bufInfoFull_main2);
            break;
        default:
            // SYNC_RESULT_OK
            // This buffet pair is synchronized.
            {
                Mutex::Autolock _l(mAvailablePoolLock);

                while(mIsLocked){
                    mCondLockAvailablePool.wait(mAvailablePoolLock);
                }

                enqueBufferToAvailableQue(bufInfoResized);
                enqueBufferToAvailableQue(bufInfoFull);
                enqueBufferToAvailableQue(bufInfoResized_main2);
                enqueBufferToAvailableQue(bufInfoFull_main2);

                showAvailableSizes();

                checkAllAvailableQueueSize();

                mCondPushAvailablePool.signal();
            }
            break;
    }

    FUNC_END;
}
/*******************************************************************************
*
********************************************************************************/
SYNC_RESULT
StereoBufferSynchronizerImp::
isTimeSync(
    BUFFER_SET* bufInfoMain1,
    BUFFER_SET* bufInfoMain2
){
    SYNC_RESULT ret = SYNC_RESULT_PAIR_OK;

    MINT64 timestamp_main1 = -1;
    MINT64 timestamp_main2 = -1;

    // fetch the metadata with timestamp
    for ( size_t i = 0; i < bufInfoMain1->metadata.size(); ++i) {
        // MY_LOGD1("tryGetMetadata bufInfoMain1->metadata %d", i);
        if ( tryGetMetadata<MINT64>(const_cast<IMetadata*>(&bufInfoMain1->metadata[i].meta), MTK_SENSOR_TIMESTAMP, timestamp_main1) ) {
            // MY_LOGD1("tryGetMetadata bufInfoMain1->metadata %d, suceess!", i);
            break;
        }
    }
    for ( size_t i = 0; i < bufInfoMain2->metadata.size(); ++i) {
        // MY_LOGD1("tryGetMetadata bufInfoMain2->metadata %d", i);
        if ( tryGetMetadata<MINT64>(const_cast<IMetadata*>(&bufInfoMain2->metadata[i].meta), MTK_SENSOR_TIMESTAMP, timestamp_main2) ) {
            // MY_LOGD1("tryGetMetadata bufInfoMain2->metadata %d, suceess!", i);
            break;
        }
    }

if(mDebugMode == DEBUG_MODE::DEBUG_MODE_SKIP_METADATA_CHECK){
    MY_LOGD("Sync check always return OK in DEBUG_MODE_SKIP_METADATA_CHECK");
    return SYNC_RESULT_PAIR_OK;
}

    int timestamp_main1_ms;
    int timestamp_main2_ms;
    int timestamp_diff;

    if(timestamp_main1 != -1 && timestamp_main2 != -1)
    {
        timestamp_main1_ms = timestamp_main1/1000000;
        timestamp_main2_ms = timestamp_main2/1000000;
        timestamp_diff     = timestamp_main1_ms - timestamp_main2_ms;
    }else{
        MY_LOGE("can not get timestamp meta");
        MY_LOGD_IF(timestamp_main1 == -1, "timestamp_main1 is -1");
        MY_LOGD_IF(timestamp_main2 == -1, "timestamp_main2 is -1");
        MY_LOGD1("SYNC_RESULT_PAIR_NOT_SYNC");
        return SYNC_RESULT_PAIR_NOT_SYNC;
    }

    char result_msg[32];
    if(abs(timestamp_diff) < PAIR_THRESHOLD_MS){
        if(abs(timestamp_diff) <= SYNC_THRESHOLD_MS){
            ret = SYNC_RESULT_PAIR_OK;
            snprintf(result_msg,32,"SYNC_RESULT_PAIR_OK");
        }else{
            ret = SYNC_RESULT_PAIR_NOT_SYNC;
            snprintf(result_msg,32,"SYNC_RESULT_PAIR_NOT_SYNC");
        }
    }else{
        if(timestamp_main1 > timestamp_main2){
            ret = SYNC_RESULT_MAIN2_TOO_OLD;
            snprintf(result_msg,32,"SYNC_RESULT_MAIN2_TOO_OLD");
        }else{
            ret = SYNC_RESULT_MAIN1_TOO_OLD;
            snprintf(result_msg,32,"SYNC_RESULT_MAIN1_TOO_OLD");
        }
    }

    MINT32 currentTimestamp = (NSCam::Utils::TimeTool::getReadableTime()) % 100000;
    MY_LOGD1("timestamp_diff:(main1/main2/diff) (%d/%d/%d) (ms), %s, machineTimeDelta:%d(ssmmm)",
        timestamp_main1_ms,
        timestamp_main2_ms,
        timestamp_diff,
        result_msg,
        currentTimestamp - mTimestamp
    );

    mTimestamp = currentTimestamp;

if(mDebugMode == DEBUG_MODE::DEBUG_MODE_SKIP_TIMESTAMP_CHECK){
    MY_LOGD("Sync check always return OK in debugMode=DEBUG_MODE_SKIP_TIMESTAMP_CHECK");
    ret = SYNC_RESULT_PAIR_OK;
}
    return ret;
}
/******************************************************************************
 *
 ******************************************************************************/
MBOOL
StereoBufferSynchronizerImp::
isReadyToLock()
{
    showAvailableSizes();

    for(map<StreamId_T, list<BUFFER_SET>*>::iterator it=streamToAvailableQue.begin(); it!=streamToAvailableQue.end(); ++it){
        if( it->second->empty()){
            return MFALSE;
        }
    }
    MY_LOGD1("availablePools are ready");
    return MTRUE;
}
/******************************************************************************
 *
 ******************************************************************************/
MERROR
StereoBufferSynchronizerImp::
lock(){
    FUNC_START;
    Mutex::Autolock _l(mAvailablePoolLock);

    if(mIsLocked){
        MY_LOGE("try to lock synchronizer when it is already locked!");
        return UNKNOWN_ERROR;
    }

    while(true){
        // break the waiting loop or keep waiting
        if(isReadyToLock()){
            mIsLocked = MTRUE;
            break;
        }else{
            mCondPushAvailablePool.wait(mAvailablePoolLock);
        }
    }

    FUNC_END;
    return OK;
}
/******************************************************************************
 *
 ******************************************************************************/
MERROR
StereoBufferSynchronizerImp::
unlock(){
    FUNC_START;
    Mutex::Autolock _l(mAvailablePoolLock);

    if(!mIsLocked){
        MY_LOGE("try to unlock synchronizer when it is not locked!");
        return UNKNOWN_ERROR;
    }else{
        mIsLocked = MFALSE;
        mCondLockAvailablePool.signal();
    }

    FUNC_END;
    return OK;
}
/******************************************************************************
 *
 ******************************************************************************/
void
StereoBufferSynchronizerImp::
setDebugMode(MINT32 debug)
{
    mDebugMode = debug;
    MY_LOGD("mDebugMode=%d", mDebugMode);
    // mLogLevel = 2;
    // MY_LOGD("automatically increase mLogLevel to:%d in debugMode", mLogLevel);
}
/******************************************************************************
 *
 ******************************************************************************/
MERROR
StereoBufferSynchronizerImp::
flush(){
    FUNC_START;

    MY_LOGD("return all pending/available buffers +");

    clearQueAndReturnBufferToSelector();

    MY_LOGD("return all pending/available buffers -");

    Thread::requestExit();
    mbExitThread = true;

    FUNC_END;
    return OK;
}
/******************************************************************************
 *
 ******************************************************************************/
MERROR
StereoBufferSynchronizerImp::
start(){
    MY_LOGD("synchronizer start to work");
    return run();
}
/******************************************************************************
 *
 ******************************************************************************/
status_t
StereoBufferSynchronizerImp::
readyToRun()
{
    // set name
    ::prctl(PR_SET_NAME, (unsigned long)THREAD_NAME, 0, 0, 0);

    // set normal
    struct sched_param sched_p;
    sched_p.sched_priority = 0;
    ::sched_setscheduler(0, THREAD_POLICY, &sched_p);
    ::setpriority(PRIO_PROCESS, 0, THREAD_PRIORITY);
    //
    ::sched_getparam(0, &sched_p);

    MY_LOGD(
        "tid(%d) policy(%d) priority(%d)"
        , ::gettid(), ::sched_getscheduler(0)
        , sched_p.sched_priority
    );

    //
    return OK;
}
/******************************************************************************
 *
 ******************************************************************************/
bool
StereoBufferSynchronizerImp::
threadLoop()
{
    FUNC_START;

    BUFFER_SET bufInfoResized;
    BUFFER_SET bufInfoResized_main2;
    BUFFER_SET bufInfoFull;
    BUFFER_SET bufInfoFull_main2;
    MBOOL haveFullRaw = false;
    MBOOL haveFullRaw_main2 = false;

    // try to grab the to-be-checked buffer set
    {
        Mutex::Autolock _l(mPendingPoolLock);

        MY_LOGD1("StereoSynchronizer threadLoop wait +");
        while(!isReadyToSyncCheck()){
            mCondPushPendingPool.wait(mPendingPoolLock);
        }
        MY_LOGD1("StereoSynchronizer threadLoop wait -");

        // (Must have)resized raw from each sensor
        bufInfoResized = pendingQueueResizeRaw.front();
        pendingQueueResizeRaw.pop_front();

        bufInfoResized_main2 = pendingQueueResizeRaw_main2.front();
        pendingQueueResizeRaw_main2.pop_front();

        // (Optional)full raw from each sensor
        if(streamToPendingQue.count(eSTREAMID_IMAGE_PIPE_RAW_OPAQUE)){
            bufInfoFull = pendingQueueFullRaw.front();
            pendingQueueFullRaw.pop_front();
            haveFullRaw = true;
        }
        if(streamToPendingQue.count(eSTREAMID_IMAGE_PIPE_RAW_OPAQUE_01)){
            bufInfoFull_main2 = pendingQueueFullRaw_main2.front();
            pendingQueueFullRaw_main2.pop_front();
            haveFullRaw_main2 = true;
        }
    }

    // synchcronization routine
    syncRoutine(
        &bufInfoResized,
        &bufInfoResized_main2,
        (haveFullRaw)       ? &bufInfoFull          : NULL,
        (haveFullRaw_main2) ? &bufInfoFull_main2    : NULL
    );

    // make sure there is no too much buffer pending here
    {
        Mutex::Autolock _l(mPendingPoolLock);
        checkAllPendingQueueSize();
    }

    if(mbExitThread)
    {
        MY_LOGD("exit thread loop");
        FUNC_END;
        return false;
    }

    return  true;
}

/******************************************************************************
 *
 ******************************************************************************/
}; //namespace NSLegacyPipeline
}; //namespace v1
}; //namespace NSCam