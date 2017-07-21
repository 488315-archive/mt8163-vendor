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

#ifndef _MTK_CAMERA_STREAMING_FEATURE_PIPE_QPARAMS_BASE_H_
#define _MTK_CAMERA_STREAMING_FEATURE_PIPE_QPARAMS_BASE_H_

#include "QParamsBase_t.h"

#include <featurePipe/core/include/PipeLogHeaderBegin.h>
#include "DebugControl.h"
#define PIPE_TRACE TRACE_QPARAMS_BASE
#define PIPE_CLASS_TAG "QParamsBase"
#include <featurePipe/core/include/PipeLog.h>

namespace NSCam {
namespace NSCamFeature {
namespace NSFeaturePipe {

template <typename T>
QParamsBase<T>::QParamsBase()
    : mCount(0)
{
    TRACE_FUNC_ENTER();
    TRACE_FUNC_EXIT();
}

template <typename T>
QParamsBase<T>::~QParamsBase()
{
    TRACE_FUNC_ENTER();
    TRACE_FUNC_EXIT();
}

template <typename T>
MBOOL QParamsBase<T>::onQParamsFailCB(const NSCam::NSIoPipe::NSPostProc::QParams &param, const T &data)
{
    TRACE_FUNC_ENTER();
    MBOOL ret;
    ret = onQParamsCB(param, data);
    TRACE_FUNC_EXIT();
    return ret;
}

template <typename T>
MBOOL QParamsBase<T>::onQParamsBlockCB(const NSCam::NSIoPipe::NSPostProc::QParams &param, const T &data)
{
    TRACE_FUNC_ENTER();
    MBOOL ret;
    ret = onQParamsCB(param, data);
    TRACE_FUNC_EXIT();
    return ret;
}

template <typename T>
void QParamsBase<T>::processCB(NSCam::NSIoPipe::NSPostProc::QParams param, CB_TYPE type)
{
    TRACE_FUNC_ENTER();
    QParamsBase *parent = NULL;
    BACKUP_DATA_TYPE data;

    parent = reinterpret_cast<QParamsBase*>(param.mpCookie);
    if( !parent || !parent->retrieveQParamsData(data) )
    {
        MY_LOGE("Cannot retrieve QParams data from parent=%p", parent);
    }
    else
    {
        data.restore(param);
        switch(type)
        {
        case CB_DONE:
            parent->onQParamsCB(param, data.mData);
            break;
        case CB_FAIL:
            param.mDequeSuccess = MFALSE;
            parent->onQParamsFailCB(param, data.mData);
            break;
        case CB_BLOCK:
            param.mDequeSuccess = MFALSE;
            parent->onQParamsBlockCB(param, data.mData);
            break;
        default:
            MY_LOGE("Unknown CB type = %d", type);
          break;
        }
        parent->signalDone();
    }
    TRACE_FUNC_EXIT();
}

template <typename T>
void QParamsBase<T>::staticQParamsCB(NSCam::NSIoPipe::NSPostProc::QParams &param)
{
    TRACE_FUNC_ENTER();
    processCB(param, CB_DONE);
    TRACE_FUNC_EXIT();
}

template <typename T>
void QParamsBase<T>::staticQParamsFailCB(NSCam::NSIoPipe::NSPostProc::QParams &param)
{
    TRACE_FUNC_ENTER();
    processCB(param, CB_FAIL);
    TRACE_FUNC_EXIT();
}

template <typename T>
void QParamsBase<T>::staticQParamsBlockCB(NSCam::NSIoPipe::NSPostProc::QParams &param)
{
    TRACE_FUNC_ENTER();
    processCB(param, CB_BLOCK);
    TRACE_FUNC_EXIT();
}

template <typename T>
MBOOL QParamsBase<T>::enqueQParams(NSCam::NSIoPipe::NSPostProc::INormalStream *stream, NSCam::NSIoPipe::NSPostProc::QParams param, const T &data)
{
    TRACE_FUNC_ENTER();
    android::Mutex::Autolock lock(mMutex);
    MBOOL ret = MFALSE;
    BACKUP_DATA_TYPE backup(param, data);
    param.mpCookie = (void*)this;
    param.mpfnCallback = QParamsBase<T>::staticQParamsCB;
    param.mpfnEnQFailCallback = QParamsBase<T>::staticQParamsFailCB;
    param.mpfnEnQBlockCallback = QParamsBase<T>::staticQParamsBlockCB;
    if( stream )
    {
        ret = stream->enque(param);
        if( ret )
        {
            mQueue.push(backup);
            ++mCount;
        }
    }
    TRACE_FUNC_EXIT();
    return ret;
}

template <typename T>
MVOID QParamsBase<T>::waitEnqueQParamsDone()
{
    TRACE_FUNC_ENTER();
    android::Mutex::Autolock lock(mMutex);
    while( mCount )
    {
        mCondition.wait(mMutex);
    }
    TRACE_FUNC_EXIT();
}

template <typename T>
MVOID QParamsBase<T>::signalDone()
{
    TRACE_FUNC_ENTER();
    android::Mutex::Autolock lock(mMutex);
    --mCount;
    mCondition.broadcast();
    TRACE_FUNC_EXIT();
}

template <typename T>
MBOOL QParamsBase<T>::retrieveQParamsData(BACKUP_DATA_TYPE &data)
{
    TRACE_FUNC_ENTER();
    android::Mutex::Autolock lock(mMutex);
    MBOOL ret = MFALSE;
    if( !this->mQueue.empty() )
    {
        data = this->mQueue.front();
        this->mQueue.pop();
        ret = MTRUE;
    }
    TRACE_FUNC_EXIT();
    return ret;
}

} // namespace NSFeaturePipe
} // namespace NSCamFeature
} // namespace NSCam

#include <featurePipe/core/include/PipeLogHeaderEnd.h>
#endif // _MTK_CAMERA_STREAMING_FEATURE_PIPE_QPARAMS_BASE_H_
