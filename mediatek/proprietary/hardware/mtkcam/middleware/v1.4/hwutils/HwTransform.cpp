/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
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

/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
#define LOG_TAG "MtkCam/HwTransHelper"
//
#include <Log.h>
#if defined(__func__)
#undef __func__
#endif
#define __func__ __FUNCTION__

#define MY_LOGV(fmt, arg...)        CAM_LOGV("[%s] " fmt, __func__, ##arg)
#define MY_LOGD(fmt, arg...)        CAM_LOGD("[%s] " fmt, __func__, ##arg)
#define MY_LOGI(fmt, arg...)        CAM_LOGI("[%s] " fmt, __func__, ##arg)
#define MY_LOGW(fmt, arg...)        CAM_LOGW("[%s] " fmt, __func__, ##arg)
#define MY_LOGE(fmt, arg...)        CAM_LOGE("[%s] " fmt, __func__, ##arg)
#define MY_LOGA(fmt, arg...)        CAM_LOGA("[%s] " fmt, __func__, ##arg)
#define MY_LOGF(fmt, arg...)        CAM_LOGF("[%s] " fmt, __func__, ##arg)
#define FUNC_START                  MY_LOGD("+")
#define FUNC_END                    MY_LOGD("-")
//

#include <hwutils/HwTransform.h>
#include <IHalSensor.h>

#include <utils/Mutex.h>
#include <utils/KeyedVector.h>

using namespace NSCamHW;
using namespace NSCam;

/*******************************************************************************
*
********************************************************************************/
#define ACTIVEARRAY_MODE            SENSOR_SCENARIO_ID_NORMAL_CAPTURE

MVOID
simplifiedMatrix::
dump(const char* const str) const
{
    MY_LOGD("%s: (%f, %f, %f, %f)",
            str,
            c_00, c_02, c_11, c_12);
}


/*******************************************************************************
*
********************************************************************************/
class SensorInfo
{
public:
                                    SensorInfo(MINT32 const openId)
                                        : mOpenId(openId)
                                    {}
    MBOOL                           getInfo(
                                        MUINT32 const sensorMode,
                                        HwMatrix* mat,
                                        HwMatrix* mat_inv
                                    );

private:
    android::Mutex                  mLock;
    MINT32 const                    mOpenId;

    struct cropItem
    {
        SensorCropWinInfo    sensor_crop_info;
        HwMatrix             trans;
        HwMatrix             inv_trans;
    };
    // key: sensor mode
    // value: cropItem
    android::KeyedVector<
        MUINT32, cropItem
        >                           mvCropInfos;
};


/*******************************************************************************
*
********************************************************************************/
MBOOL
SensorInfo::
getInfo(MUINT32 const sensorMode, HwMatrix* mat, HwMatrix* mat_inv)
{
    struct updater
    {
        IHalSensorList* pSensorList;
        IHalSensor*     pSensorHalObj;
        //
                        updater() : pSensorList(NULL), pSensorHalObj(NULL) {}
                        ~updater() {
                            if( pSensorHalObj ) pSensorHalObj->destroyInstance(LOG_TAG);
                        }
        //
        MBOOL
            operator() (MINT32 const openId, MUINT32 const sensorMode, SensorCropWinInfo& cropInfo)
        {
            pSensorList = IHalSensorList::get();
            if( ! pSensorList ||
                ! (pSensorHalObj = pSensorList->createSensor(LOG_TAG, openId))
              )
            {
                MY_LOGE("fail pSensorList(%p), pSensorHal(%p)", pSensorList, pSensorHalObj);
                return MFALSE;
            }
            //
            MINT32 err = pSensorHalObj->sendCommand(
                    pSensorList->querySensorDevIdx(openId),
                    SENSOR_CMD_GET_SENSOR_CROP_WIN_INFO,
                    (MUINTPTR)&sensorMode,
                    (MUINTPTR)&cropInfo,
                    0);
            //
            if( err != 0 ||
                    cropInfo.full_w == 0 || cropInfo.full_h == 0 ||
                    cropInfo.w0_size == 0 || cropInfo.h0_size == 0)
            {
                MY_LOGW("cannot get proper sensor %d crop win info of mode (%d): use default",
                        openId, sensorMode);
                //
                SensorStaticInfo staticInfo;
                memset(&staticInfo, 0, sizeof(SensorStaticInfo));
                //
                pSensorList->querySensorStaticInfo(
                    pSensorList->querySensorDevIdx(openId),
                    &staticInfo);
                //
                MSize tgsize;
                switch( sensorMode )
                {
                #define sensor_mode_case( _mode_, _key_, _size_ ) \
                    case _mode_:                                  \
                        _size_.w = staticInfo._key_##Width;       \
                        _size_.h = staticInfo._key_##Height;      \
                        break;
                    sensor_mode_case( SENSOR_SCENARIO_ID_NORMAL_PREVIEW, preview, tgsize );
                    sensor_mode_case( SENSOR_SCENARIO_ID_NORMAL_CAPTURE, capture, tgsize );
                    sensor_mode_case( SENSOR_SCENARIO_ID_NORMAL_VIDEO, video, tgsize );
                    sensor_mode_case( SENSOR_SCENARIO_ID_SLIM_VIDEO1, video1, tgsize );
                    sensor_mode_case( SENSOR_SCENARIO_ID_SLIM_VIDEO2, video2, tgsize );
                    sensor_mode_case( SENSOR_SCENARIO_ID_CUSTOM1, SensorCustom1, tgsize);
                    sensor_mode_case( SENSOR_SCENARIO_ID_CUSTOM2, SensorCustom2, tgsize);
                    sensor_mode_case( SENSOR_SCENARIO_ID_CUSTOM3, SensorCustom3, tgsize);
                    sensor_mode_case( SENSOR_SCENARIO_ID_CUSTOM4, SensorCustom4, tgsize);
                    sensor_mode_case( SENSOR_SCENARIO_ID_CUSTOM5, SensorCustom5, tgsize);
                #undef sensor_mode_case
                    default:
                        MY_LOGE("not support sensor scenario(0x%x)", sensorMode);
                        return MFALSE;
                }
                //
                //cropInfo.full_w;
                //cropInfo.full_h;
                cropInfo.x0_offset = 0;
                cropInfo.y0_offset = 0;
                cropInfo.w0_size = tgsize.w;
                cropInfo.h0_size = tgsize.h;
                //
                cropInfo.scale_w = tgsize.w;
                cropInfo.scale_h = tgsize.h;
                //
                cropInfo.x1_offset = 0;
                cropInfo.y1_offset = 0;
                cropInfo.w1_size = tgsize.w;
                cropInfo.h1_size = tgsize.w;
                //
                cropInfo.x2_tg_offset = 0;
                cropInfo.y2_tg_offset = 0;
                cropInfo.w2_tg_size = tgsize.w;
                cropInfo.h2_tg_size = tgsize.h;
            }
            MY_LOGD("senor %d, mode %d: crop infos", openId, sensorMode);
            MY_LOGD("full %dx%d, crop0(%d,%d,%dx%d), resized(%d,%d)",
                    cropInfo.full_w, cropInfo.full_h,
                    cropInfo.x0_offset, cropInfo.y0_offset, cropInfo.w0_size, cropInfo.h0_size,
                    cropInfo.scale_w, cropInfo.scale_h
                    );
            MY_LOGD("crop1(%d,%d,%dx%d), tg(%d,%d,%dx%d)",
                    cropInfo.x1_offset, cropInfo.y1_offset, cropInfo.w1_size, cropInfo.h1_size,
                    cropInfo.x2_tg_offset, cropInfo.y2_tg_offset, cropInfo.w2_tg_size, cropInfo.h2_tg_size
                   );
            return MTRUE;
        }
    };
    //
    android::Mutex::Autolock _l(mLock);
    //
    ssize_t idxTarget = mvCropInfos.indexOfKey(sensorMode);
    if( 0 > idxTarget ) {
        // check active array
        ssize_t idxActive = mvCropInfos.indexOfKey(ACTIVEARRAY_MODE);
        if( 0 > idxActive ) {
            cropItem itemActive;
            if( ! updater()(mOpenId, ACTIVEARRAY_MODE, itemActive.sensor_crop_info) )
                return MFALSE;
            //
            itemActive.trans     = HwMatrix(1.f, 0.f, 1.f, 0.f);
            itemActive.inv_trans = HwMatrix(1.f, 0.f, 1.f, 0.f);
            //
            idxActive = mvCropInfos.add(ACTIVEARRAY_MODE, itemActive);
        }
        SensorCropWinInfo const& crop_active =
            mvCropInfos.valueAt(idxActive).sensor_crop_info;
        //
        cropItem item;
        if( ! updater()(mOpenId, sensorMode, item.sensor_crop_info) )
                return MFALSE;
        //
        SensorCropWinInfo const& crop_target = item.sensor_crop_info;
        //
#if 0
        // check left-top
        if( crop_target.x0_offset <= crop_active.x0_offset ||
            crop_target.y0_offset <= crop_active.y0_offset )
        {
            MY_LOGW("left-top should in active array");
        }
        //
        // check right-bottom
        if(
                (crop_target.x0_offset + crop_target.w0_size <=
                 crop_active.x0_offset + crop_active.w0_size) ||
                (crop_target.y0_offset + crop_target.h0_size <=
                 crop_active.y0_offset + crop_active.h0_size)
          )
        {
            MY_LOGW("right-bottom should in active array");
        }
#endif
        //
        struct forward_matrix
        {
            static
                HwMatrix get(SensorCropWinInfo const& info) {
                    return
                        HwMatrix(
                                1.f,
                                -(info.x1_offset + info.x2_tg_offset),
                                1.f,
                                -(info.y1_offset + info.y2_tg_offset)
                                )
                        * HwMatrix(
                                float(info.scale_w) / float(info.w0_size), 0.f,
                                float(info.scale_h) / float(info.h0_size), 0.f
                                );
                }
        };
        //
        HwMatrix active_forward = forward_matrix::get(crop_active);
        //active_forward.dump("active forward"); //debug
        //
        HwMatrix active_inv;
        if( ! active_forward.getInverse(active_inv) ) {
            MY_LOGE("cannot get proper inverse matrix of active");
            return MFALSE;
        }
        //active_inv.dump("active_inv"); //debug
        //
        HwMatrix target_forward = forward_matrix::get(crop_target);
        //target_forward.dump("target_forward"); //debug
        //
        item.trans =  target_forward * active_inv;
        //item.trans.dump("trans"); //debug
        //
        if( ! item.trans.getInverse(item.inv_trans) ) {
            MY_LOGE("cannot get proper inverse matrix");
        }
        else {
            //item.inv_trans.dump("inv_trans"); //debug
            idxTarget = mvCropInfos.add(sensorMode, item);
        }
    }
    //
    if( mat )    *mat     = mvCropInfos.valueAt(idxTarget).trans;
    if( mat_inv) *mat_inv = mvCropInfos.valueAt(idxTarget).inv_trans;
    //
    return idxTarget >= 0;
}


/*******************************************************************************
*
********************************************************************************/
android::Mutex                                  gLock;
android::KeyedVector< MUINT32, SensorInfo* >    gvSensorInfos;


/*******************************************************************************
*
********************************************************************************/
HwTransHelper::
HwTransHelper(MINT32 const openId)
    : mOpenId(openId)
{
    android::Mutex::Autolock _l(gLock);
    if( gvSensorInfos.indexOfKey(openId) < 0 ) {
        gvSensorInfos.add(openId, new SensorInfo(openId));
    }
}


/*******************************************************************************
*
********************************************************************************/
HwTransHelper::
~HwTransHelper()
{
}


/*******************************************************************************
*
********************************************************************************/
MBOOL
HwTransHelper::
getMatrixFromActive(MUINT32 const sensorMode, HwMatrix& mat)
{
    SensorInfo* pSensorInfo = NULL;
    //
    {
        android::Mutex::Autolock _l(gLock);
        ssize_t idx = gvSensorInfos.indexOfKey(mOpenId);
        if( idx < 0 ) {
            return MFALSE;
        }
        pSensorInfo = gvSensorInfos.valueAt(idx);
    }
    //
    return pSensorInfo->getInfo(sensorMode, &mat, NULL);
}


/*******************************************************************************
*
********************************************************************************/
MBOOL
HwTransHelper::
getMatrixToActive(MUINT32 const sensorMode, HwMatrix& mat)
{
    SensorInfo* pSensorInfo = NULL;
    //
    {
        android::Mutex::Autolock _l(gLock);
        ssize_t idx = gvSensorInfos.indexOfKey(mOpenId);
        if( idx < 0 ) {
            return MFALSE;
        }
        pSensorInfo = gvSensorInfos.valueAt(idx);
    }
    //
    return pSensorInfo->getInfo(sensorMode, NULL, &mat);
}


