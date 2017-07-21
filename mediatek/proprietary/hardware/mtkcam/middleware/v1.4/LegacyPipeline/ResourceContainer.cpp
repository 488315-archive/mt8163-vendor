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

#define LOG_TAG "MtkCam/ResourceContainer"
//
#include "MyUtils.h"
//
#include <LegacyPipeline/IResourceContainer.h>
#include <metadata/client/mtk_metadata_tag.h>

using namespace NSCam;
using namespace NSCam::v1;
using namespace NSCam::v1::NSLegacyPipeline;
using namespace android;


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

#if 1
#define FUNC_START     MY_LOGD("+")
#define FUNC_END       MY_LOGD("-")
#else
#define FUNC_START
#define FUNC_END
#endif


/******************************************************************************
 *
 ******************************************************************************/
class ResourceContainer : public IResourceContainer
{

public:
                                       ResourceContainer() {}

    virtual                            ~ResourceContainer() {}

    virtual MVOID                      setFeatureFlowControl( sp<IFeatureFlowControl> pControl );

    virtual MVOID                      setConsumer( StreamId_T id, sp<StreamBufferProvider> pProvider );

    virtual sp<StreamBufferProvider>   queryConsumer( StreamId_T id );

    virtual sp<IFeatureFlowControl>    queryFeatureFlowControl();

    virtual MVOID                      setLatestFrameInfo( sp<IFrameInfo> frameInfo );

    virtual sp<IFrameInfo>             queryLatestFrameInfo();

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Implementation.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
protected:  ////                    Definitions.


protected:  ////                    Data Members.
    int                                            miOpenId;
    KeyedVector<MINT32, sp<StreamBufferProvider> > mvStreamBufferProviders;   //(StreamId_T, sp<StreamBufferProvider>)
    wp<IFeatureFlowControl>                        mFeatureFlowControl;
    sp<IFrameInfo>                                 mFrameInfo;
    mutable Mutex                                  mResourceSetLock;

protected:  ////                    Operations.


public:
    virtual status_t                            onCreate(int iOpenId);


};

/******************************************************************************
 *
 ******************************************************************************/
sp<IResourceContainer>
IResourceContainer::
getInstance(int iOpenId)
{
    FUNC_START;
    static KeyedVector<MINT32, wp<ResourceContainer> > gvResMgr;
    sp<ResourceContainer> pContainer = NULL;
    if ( gvResMgr.size() == 0 || gvResMgr.indexOfKey(iOpenId) < 0 ) {
        MY_LOGD("No Container, OpenID(%d) New ResourceContainer", iOpenId);
        pContainer = new ResourceContainer();
        pContainer->onCreate(iOpenId);
        gvResMgr.add(iOpenId,pContainer);
    }
    else
    {
        pContainer = gvResMgr.editValueFor(iOpenId).promote();
        if(pContainer == 0)
        {
            MY_LOGD("Container turn NULL, OpenID(%d) New ResourceContainer", iOpenId);
            pContainer = new ResourceContainer();
            pContainer->onCreate(iOpenId);
            gvResMgr.replaceValueFor(iOpenId,pContainer);
        }
    }
    FUNC_END;
    return pContainer;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
ResourceContainer::
onCreate(int iOpenId)
{
    Mutex::Autolock _l(mResourceSetLock);
    miOpenId = iOpenId;
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
ResourceContainer::
setFeatureFlowControl( android::sp<IFeatureFlowControl> pControl )
{
    FUNC_START;
    if ( pControl == 0 )
    {
        MY_LOGW("OpenID(%d) set NULL pControl", miOpenId);
        FUNC_END;
        return;
    }
    //
    {
        Mutex::Autolock _l(mResourceSetLock);
        mFeatureFlowControl = pControl;
    }
    FUNC_END;
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
ResourceContainer::
setConsumer( StreamId_T id, android::sp<StreamBufferProvider> pProvider )
{
    FUNC_START;
    if ( pProvider == 0 )
    {
        MY_LOGW("OpenID(%d) set NULL pProvider", miOpenId);
        FUNC_END;
        return;
    }
    //
    {
        Mutex::Autolock _l(mResourceSetLock);
        ssize_t index = mvStreamBufferProviders.indexOfKey(id);
        if ( index < 0 ) {
            MY_LOGD("OpenID(%d) set StreamBufferProvider for streamId %#" PRIxPTR , miOpenId, id);
            mvStreamBufferProviders.add( id, pProvider );
        }
        else
        {
            mvStreamBufferProviders.editValueAt(index) = pProvider;
        }
    }
    FUNC_END;
}

/******************************************************************************
 *
 ******************************************************************************/
sp<StreamBufferProvider>
ResourceContainer::
queryConsumer( StreamId_T id )
{
    FUNC_START;
    Mutex::Autolock _l(mResourceSetLock);
    sp<StreamBufferProvider> pProvider = NULL;
    ssize_t index = mvStreamBufferProviders.indexOfKey(id);
    if( index < 0 )
    {
        MY_LOGW("OpenID(%d) queryConsumer streamId(%#" PRIxPTR ") found nothing", miOpenId,id);
    }
    else
    {
        pProvider = mvStreamBufferProviders.editValueFor(id);
    }
    FUNC_END;
    return pProvider;
}

/******************************************************************************
 *
 ******************************************************************************/
sp<IFeatureFlowControl>
ResourceContainer::
queryFeatureFlowControl()
{
    FUNC_START;
    Mutex::Autolock _l(mResourceSetLock);
    sp<IFeatureFlowControl> pControl = mFeatureFlowControl.promote();
    if( pControl == NULL )
    {
        MY_LOGW("queryFeatureFlowControl found nothing");
    }
    FUNC_END;
    return pControl;
}

/******************************************************************************
 *
 ******************************************************************************/
MVOID
ResourceContainer::
setLatestFrameInfo( sp<IFrameInfo> frameInfo)
{
    Mutex::Autolock _l(mResourceSetLock);
    mFrameInfo = frameInfo;
}

/******************************************************************************
 *
 ******************************************************************************/
sp<IFrameInfo>
ResourceContainer::
queryLatestFrameInfo()
{
    Mutex::Autolock _l(mResourceSetLock);
    return mFrameInfo;
}

/******************************************************************************
 *
 ******************************************************************************/
FrameInfo::
FrameInfo()
{

}

/******************************************************************************
 *
 ******************************************************************************/
FrameInfo::
~FrameInfo()
{

}

/******************************************************************************
 *
 ******************************************************************************/
void
FrameInfo::
onResultReceived(
                        MUINT32         const requestNo,
                        StreamId_T      const streamId,
                        MBOOL           const errorResult,
                        IMetadata       const result
                        )
{
    Mutex::Autolock _l(mResultSetLock);
    mResultMeta = result;
}

/******************************************************************************
 *
 ******************************************************************************/
String8
FrameInfo::
getUserName()
{
    return String8(LOG_TAG);
}

/******************************************************************************
 *
 ******************************************************************************/
void
FrameInfo::
getFrameMetadata(IMetadata &rResultMeta)
{
    Mutex::Autolock _l(mResultSetLock);
    rResultMeta = mResultMeta;
}

