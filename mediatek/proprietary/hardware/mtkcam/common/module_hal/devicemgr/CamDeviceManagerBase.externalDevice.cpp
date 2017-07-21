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

#define LOG_TAG "MtkCam/devicemgr"

// get_boot_mode
#include <sys/stat.h>
#include <fcntl.h>
//
#include <binder/IServiceManager.h> 
//#include <mmsdk/IMMSdkService.h>
//
#include <cutils/properties.h>
#include "MyUtils.h"
//
#include <device/ExternalModuleBase.h>
//
#include <metadata/IMetadata.h>
#include <metadata/IMetadataConverter.h>
#include <metadata/IMetadataProvider.h>
#include <metadata/mtk_metadata_types.h>
#include <metadata/client/TagMap.h>
//
//using namespace NSMMSdk; 

//
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


/******************************************************************************
 *
 ******************************************************************************/
CamDeviceManagerBase::
ExternalDeviceInfo::
~ExternalDeviceInfo()
{
}


/******************************************************************************
 *
 ******************************************************************************/
CamDeviceManagerBase::
ExternalDeviceInfo::
ExternalDeviceInfo()
    : RefBase()
    , pExternalDevice(NULL)
    , pLocalDevice(NULL)
    , i4OpenId(0)
    , uDeviceVersion(0)
    , i8OpenTimestamp(0)
    , uDeviceID(0)
    , isOpen(false)
    , isDisconnected(false)
{
}


/******************************************************************************
 *
 ******************************************************************************/
status_t
CamDeviceManagerBase::
getExternalDeviceListener(
    android::sp<IBinder>& listener
)
{
    if ( mListener == 0){
        mListener = new ExternalDeviceListenerImp(this);
        MY_LOGI("IExternalDeviceListener has been created. %p", mListener.get());
    }
    listener = IInterface::asBinder(mListener);
    //
    return OK;
}
/******************************************************************************
 *
 ******************************************************************************/
status_t
CamDeviceManagerBase::
updateCameraCharacteristics (
    CameraInfo*& info,
    camera_metadata*& cameraCharacteristics,
    IMetadata& camera
)
{
    MY_LOGD("+");
    android::sp<IMetadataConverter> mpMetadataConverter =
        IMetadataConverter::createInstance(IDefaultMetadataTagSet::singleton()->getTagSet());
    //IMetadata camera;
    mpMetadataConverter->convert(info->cameraCharacteristics, camera);
    //
    MY_LOGD("filter static metadata yuv(%d,%d) jpeg(%d,%d)", info->supprotSize[0], info->supprotSize[1], info->supprotSize[2], info->supprotSize[3]);
    //
    // update android.scaler.availableStreamConfigurations
    // update android.scaler.availableMinFrameDurations
    // update android.scaler.availableStallDurations
    IMetadata::IEntry availableStreamConfigurations(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS);
    IMetadata::IEntry availableMinFrameDurations(MTK_SCALER_AVAILABLE_MIN_FRAME_DURATIONS);
    IMetadata::IEntry availableStallDurations(MTK_SCALER_AVAILABLE_STALL_DURATIONS);
    //
    IMetadata::IEntry const& entryScaler =
      camera.entryFor(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS);
    IMetadata::IEntry const& entryScalerMin =
      camera.entryFor(MTK_SCALER_AVAILABLE_MIN_FRAME_DURATIONS);
    IMetadata::IEntry const& entryScalerStall =
      camera.entryFor(MTK_SCALER_AVAILABLE_STALL_DURATIONS);

    if ( entryScaler.isEmpty() ) {
        MY_LOGE("no static MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS");
        return -EINVAL;
    }

    // int32 x n x 4
    // (format, width, height, input?)
    for ( size_t i = 0; i < entryScaler.count(); i += 4 )
    {
        if (  (entryScaler.itemAt(i + 0, Type2Type< MINT32 >()) == HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED
            || entryScaler.itemAt(i + 0, Type2Type< MINT32 >()) == HAL_PIXEL_FORMAT_YCbCr_420_888)
            &&(entryScaler.itemAt(i + 1, Type2Type< MINT32 >()) <= info->supprotSize[0]
            && entryScaler.itemAt(i + 2, Type2Type< MINT32 >()) <= info->supprotSize[1])
            &&(entryScaler.itemAt(i + 3, Type2Type< MINT32 >()) == MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT))
        {
            MY_LOGD("find yuv(%d,%d)", entryScaler.itemAt(i + 1, Type2Type< MINT32 >()), entryScaler.itemAt(i + 2, Type2Type< MINT32 >()));
            //
            availableStreamConfigurations.push_back(entryScaler.itemAt(i + 0, Type2Type< MINT32 >()), Type2Type< MINT32 >());
            availableStreamConfigurations.push_back(entryScaler.itemAt(i + 1, Type2Type< MINT32 >()), Type2Type< MINT32 >());
            availableStreamConfigurations.push_back(entryScaler.itemAt(i + 2, Type2Type< MINT32 >()), Type2Type< MINT32 >());
            availableStreamConfigurations.push_back(entryScaler.itemAt(i + 3, Type2Type< MINT32 >()), Type2Type< MINT32 >());
            //
            availableMinFrameDurations.push_back(entryScalerMin.itemAt(i + 0, Type2Type< MINT64 >()), Type2Type< MINT64 >());
            availableMinFrameDurations.push_back(entryScalerMin.itemAt(i + 1, Type2Type< MINT64 >()), Type2Type< MINT64 >());
            availableMinFrameDurations.push_back(entryScalerMin.itemAt(i + 2, Type2Type< MINT64 >()), Type2Type< MINT64 >());
            availableMinFrameDurations.push_back(entryScalerMin.itemAt(i + 3, Type2Type< MINT64 >()), Type2Type< MINT64 >());
            //
            availableStallDurations.push_back(entryScalerStall.itemAt(i + 0, Type2Type< MINT64 >()), Type2Type< MINT64 >());
            availableStallDurations.push_back(entryScalerStall.itemAt(i + 1, Type2Type< MINT64 >()), Type2Type< MINT64 >());
            availableStallDurations.push_back(entryScalerStall.itemAt(i + 2, Type2Type< MINT64 >()), Type2Type< MINT64 >());
            availableStallDurations.push_back(entryScalerStall.itemAt(i + 3, Type2Type< MINT64 >()), Type2Type< MINT64 >());
        } else 
        if (   entryScaler.itemAt(i + 0, Type2Type< MINT32 >()) == HAL_PIXEL_FORMAT_BLOB
            &&(entryScaler.itemAt(i + 1, Type2Type< MINT32 >()) == info->supprotSize[2]
            && entryScaler.itemAt(i + 2, Type2Type< MINT32 >()) == info->supprotSize[3])
            &&(entryScaler.itemAt(i + 3, Type2Type< MINT32 >()) == MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS_OUTPUT))
        {
            MY_LOGD("find jpeg(%d,%d)", entryScaler.itemAt(i + 1, Type2Type< MINT32 >()), entryScaler.itemAt(i + 2, Type2Type< MINT32 >()));
            //
            availableStreamConfigurations.push_back(entryScaler.itemAt(i + 0, Type2Type< MINT32 >()), Type2Type< MINT32 >());
            availableStreamConfigurations.push_back(entryScaler.itemAt(i + 1, Type2Type< MINT32 >()), Type2Type< MINT32 >());
            availableStreamConfigurations.push_back(entryScaler.itemAt(i + 2, Type2Type< MINT32 >()), Type2Type< MINT32 >());
            availableStreamConfigurations.push_back(entryScaler.itemAt(i + 3, Type2Type< MINT32 >()), Type2Type< MINT32 >());
            //
            availableMinFrameDurations.push_back(entryScalerMin.itemAt(i + 0, Type2Type< MINT64 >()), Type2Type< MINT64 >());
            availableMinFrameDurations.push_back(entryScalerMin.itemAt(i + 1, Type2Type< MINT64 >()), Type2Type< MINT64 >());
            availableMinFrameDurations.push_back(entryScalerMin.itemAt(i + 2, Type2Type< MINT64 >()), Type2Type< MINT64 >());
            availableMinFrameDurations.push_back(entryScalerMin.itemAt(i + 3, Type2Type< MINT64 >()), Type2Type< MINT64 >());
            //
            availableStallDurations.push_back(entryScalerStall.itemAt(i + 0, Type2Type< MINT64 >()), Type2Type< MINT64 >());
            availableStallDurations.push_back(entryScalerStall.itemAt(i + 1, Type2Type< MINT64 >()), Type2Type< MINT64 >());
            availableStallDurations.push_back(entryScalerStall.itemAt(i + 2, Type2Type< MINT64 >()), Type2Type< MINT64 >());
            availableStallDurations.push_back(entryScalerStall.itemAt(i + 3, Type2Type< MINT64 >()), Type2Type< MINT64 >());
        }
    }
    camera.update(MTK_SCALER_AVAILABLE_STREAM_CONFIGURATIONS, availableStreamConfigurations);
    camera.update(MTK_SCALER_AVAILABLE_MIN_FRAME_DURATIONS, availableMinFrameDurations);
    camera.update(MTK_SCALER_AVAILABLE_STALL_DURATIONS, availableStallDurations);
    //
    // update android.sync.maxLatency
    IMetadata::IEntry syncMaxLatency(MTK_SYNC_MAX_LATENCY);
    syncMaxLatency.push_back(MTK_SYNC_MAX_LATENCY_UNKNOWN, Type2Type< MINT32 >());
    camera.update(MTK_SYNC_MAX_LATENCY, syncMaxLatency);
    //
    // update android.request.availableCapabilities
    IMetadata::IEntry availableCapabilities(MTK_REQUEST_AVAILABLE_CAPABILITIES);
    availableCapabilities.push_back(MTK_REQUEST_AVAILABLE_CAPABILITIES_BACKWARD_COMPATIBLE, Type2Type< MUINT8 >());
    availableCapabilities.push_back(MTK_REQUEST_AVAILABLE_CAPABILITIES_MANUAL_SENSOR, Type2Type< MUINT8 >());
    availableCapabilities.push_back(MTK_REQUEST_AVAILABLE_CAPABILITIES_READ_SENSOR_SETTINGS, Type2Type< MUINT8 >());
    camera.update(MTK_REQUEST_AVAILABLE_CAPABILITIES, availableCapabilities);
    //
    //mpMetadataConverter->dumpAll(camera);
    //
    mpMetadataConverter->convert(camera, cameraCharacteristics);
    //
    mpMetadataConverter = NULL;
    //
    MY_LOGD("-");
    //
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
CamDeviceManagerBase::
addExternalDeviceLocked (
    uintptr_t aDeviceID,
    android::sp<IExternalDevice> aDevice,
    CameraInfo* info, 
    int32_t& remoteID
)
{
    if ( mExternalDeviceInfoMap.size() >= 8 ) {
        MY_LOGW("Too many external devices:%d", mExternalDeviceInfoMap.size());
        return -EINVAL;
    }
    remoteID = (remoteID == -1) 
        ? mEnumMap.size() + mExternalDeviceInfoMap.size()-1
        : remoteID;
    sp<ExternalDeviceInfo> pExternalDeviceInfo = new ExternalDeviceInfo;
    pExternalDeviceInfo->pInfo = info;
    pExternalDeviceInfo->pExternalDevice = aDevice;
    pExternalDeviceInfo->i4OpenId = remoteID;
    pExternalDeviceInfo->uDeviceVersion = CAMERA_DEVICE_API_VERSION_3_2;
    pExternalDeviceInfo->i8OpenTimestamp = 0;
    pExternalDeviceInfo->uDeviceID = aDeviceID;
    pExternalDeviceInfo->isOpen = false;
    //
    IMetadata mtk_meta;
    updateCameraCharacteristics(info, pExternalDeviceInfo->pMetadata, mtk_meta);
    // TODO add orientation
    //
    mExternalDeviceInfoMap.add(remoteID, pExternalDeviceInfo);
    mExternalOrderList.add(remoteID);
    //
    sp<IMetadataProvider> pMetadataProvider = IMetadataProvider::create(remoteID, mtk_meta, pExternalDeviceInfo->pMetadata);
    NSMetadataProviderManager::add(remoteID, pMetadataProvider.get());
    //
    MY_LOGD("onConnected remoteID:%d deviceID:%d Device:%p mExternalDeviceInfoMap size:%zu", remoteID, aDeviceID, pExternalDeviceInfo->pExternalDevice.get(), mExternalDeviceInfoMap.size());
    //
    mpModuleCallbacks->camera_device_status_change(mpModuleCallbacks, remoteID, CAMERA_DEVICE_STATUS_PRESENT);
    //
    return OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
CamDeviceManagerBase::
removeExternalDeviceLocked (
    uintptr_t aDeviceID,
    int32_t& remoteID
)
{
    MY_LOGD("remove aDeviceID:%zu +", aDeviceID);
    int index = -1;
    for(size_t i = 0; i < mExternalDeviceInfoMap.size(); ++i) {
        if(mExternalDeviceInfoMap.valueAt(i)->uDeviceID == aDeviceID) {
            index = i;
            break;
        }
    }
    //
    MY_LOGD("remove aDeviceID:%zu index:%d", aDeviceID, index);
    if (index == -1) {
        MY_LOGW("fail to remove device:%d mExternalDeviceInfoMap size:%zu", aDeviceID, mExternalDeviceInfoMap.size());
        return -EINVAL;
    } else {
        remoteID = mExternalDeviceInfoMap.keyAt(index);
        sp<ExternalDeviceInfo> pExternalDeviceInfo = mExternalDeviceInfoMap.valueAt(index);
        MY_LOGD("remove aDeviceID:%zu index:%d remoteId:%zu mExternalDeviceInfoMap size:%zu localDevice:%d",
                aDeviceID, index, remoteID, mExternalDeviceInfoMap.size(), pExternalDeviceInfo->pLocalDevice == 0);
        if ( pExternalDeviceInfo->pLocalDevice == 0 ) {
            MY_LOGD("remove directly");
            NSMetadataProviderManager::remove(remoteID);
            mExternalDeviceInfoMap.removeItemsAt(index);
            mExternalOrderList.remove(remoteID);
            pExternalDeviceInfo = NULL;
            mpModuleCallbacks->camera_device_status_change(mpModuleCallbacks, remoteID, CAMERA_DEVICE_STATUS_PRESENT);
        } else {
            MY_LOGD("set flag");
            pExternalDeviceInfo->isDisconnected = true;
        }
        //
        MY_LOGD("onDisconnected remoteID:%d deviceID:%d mExternalDeviceInfoMap size:%zu", remoteID, aDeviceID, mExternalDeviceInfoMap.size());
        //
        return OK;
    }
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
CamDeviceManagerBase::
getExternalDeviceInfo(int const aDeviceIndex, camera_info& rInfo)
{
    size_t deviceIndex = aDeviceIndex - mEnumMap.size() + 1;
    if  ( deviceIndex >=  mExternalDeviceInfoMap.size() ) {
        MY_LOGE("Bad External index:%d", deviceIndex);
        return -EINVAL;
    }
    //sp<ExternalDeviceInfo> pExternalDeviceInfo = mExternalDeviceInfoMap.valueFor(deviceIndex);
    sp<ExternalDeviceInfo> pExternalDeviceInfo = mExternalDeviceInfoMap.valueFor(mExternalOrderList[deviceIndex]);
    //
    if  ( pExternalDeviceInfo == 0 ) {
        MY_LOGE("Bad External deviceId:%d", mExternalOrderList[deviceIndex]);
        return -EINVAL;
    }
    //
    if  ( pExternalDeviceInfo->isDisconnected ) {
        MY_LOGE("External deviceId:%d disconnect", mExternalOrderList[deviceIndex]);
        return -EINVAL;
    }
    //
    rInfo.device_version= pExternalDeviceInfo->uDeviceVersion;
    rInfo.facing        = CAMERA_FACING_BACK;
    rInfo.orientation   = 90;//pExternalDeviceInfo->pInfo->orientation;
    rInfo.static_camera_characteristics = pExternalDeviceInfo->pMetadata;
    //
    MY_LOGD("External index:%d deviceId:%d device_version:0x%x facing:%d orientation:%d", deviceIndex, mExternalOrderList[deviceIndex], rInfo.device_version, rInfo.facing, rInfo.orientation);
    return  OK;
}


/******************************************************************************
 *
 ******************************************************************************/
status_t
CamDeviceManagerBase::
validateExternalDeviceLocked(int32_t i4OpenIndex, uint32_t device_version) const
{
    size_t deviceIndex = i4OpenIndex - mEnumMap.size() + 1;
    if  ( deviceIndex >=  mExternalOrderList.size() ) {
        MY_LOGE("Bad External index:%d", i4OpenIndex);
        return -EINVAL;
    }
    sp<ExternalDeviceInfo> pExternalDeviceInfo = mExternalDeviceInfoMap.valueFor(mExternalOrderList[deviceIndex]);
    if  ( pExternalDeviceInfo == 0 ) {
        MY_LOGE(
            "Bad External Device index:%d OpenId:%d - version:0x%x mExternalDeviceInfoMap.size:%zu",
            i4OpenIndex, mExternalOrderList[deviceIndex], device_version, mExternalDeviceInfoMap.size()
        );
        /*
         * -EINVAL:     The input arguments are invalid, i.e. the id is invalid,
         *              and/or the module is invalid.
         */
        return -EINVAL;
    }
    //
    if  ( pExternalDeviceInfo != 0 ) {
        if(    !pExternalDeviceInfo->isOpen 
            && !pExternalDeviceInfo->isDisconnected
          ) {
            return OK;
        }
        //
        MY_LOGE(
            "Busy External index:%d deviceId:%d; device:%p has already been opened with version:0x%x OpenTimestamp:%lld isDisconnected:%d isOpen:%d",
            i4OpenIndex, mExternalOrderList[deviceIndex], pExternalDeviceInfo.get(), pExternalDeviceInfo->uDeviceVersion, pExternalDeviceInfo->i8OpenTimestamp, pExternalDeviceInfo->isDisconnected, pExternalDeviceInfo->isOpen
        );
        /*
         * -EBUSY:      The camera device was already opened for this camera id
         *              (by using this method or common.methods->open method),
         *              regardless of the device HAL version it was opened as.
         */
        return -EBUSY;
    }
    //
    return  OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
CamDeviceManagerBase::
openExternalDevice (
    hw_device_t** device,
    hw_module_t const* module,
    int32_t const i4OpenIndex,
    uint32_t device_version
)
{
    size_t i4OpenId = mExternalOrderList[i4OpenIndex - mEnumMap.size() + 1];
    status_t status = OK;
    sp<ICamDevice> pDevice = NULL;
    //
    String8 const s8ClientAppMode("ExternalDevice");
    //
    MY_LOGD(
        "+ index:%d, OpenId:%d with version 0x%x - mExternalDeviceInfoMap.size:%zu",
        i4OpenIndex, i4OpenId, device_version, mExternalDeviceInfoMap.size()
    );
    //
    //  [1] check to see whether it's ready to open.
    if  ( OK != (status = validateExternalDeviceLocked(i4OpenIndex, device_version)) )
    {
        return  status;
    }
    //
    //  [2] get platform
    IPlatform*const pPlatform = getPlatform();
    if  ( ! pPlatform )
    {
        MY_LOGE("No Platform");
        return  -ENODEV;
    }
    //
    //  [3] create device based on device version.
    if  ( device_version == CAMERA_DEVICE_API_VERSION_1_0 )
    {
        MY_LOGE("External Device unsupported version:0x%x", device_version);
        return  -ENODEV;
    }
    else
    if  ( device_version >= CAMERA_DEVICE_API_VERSION_3_0 )
    {
        //int32_t i4OpenId = mExternalOrderList[i4OpenIndex];
        MY_LOGD("openExternalDevice index:%d i4OpenId:%d externalDevice:%p localDevice:%p mExternalDeviceInfoMap size:%zu",
                i4OpenIndex, i4OpenId, mExternalDeviceInfoMap.valueFor(i4OpenId)->pExternalDevice.get(), mExternalDeviceInfoMap.valueFor(i4OpenId)->pLocalDevice.get(),mExternalDeviceInfoMap.size());
        //
        pDevice = pPlatform->createCam3ExternalDevice(
                                s8ClientAppMode.string(), 
                                i4OpenId,
                                mExternalDeviceInfoMap.valueFor(i4OpenId)->pExternalDevice.get(), 
                                mExternalDeviceInfoMap.valueFor(i4OpenId)->pInfo
                            );
    }
    else
    {
        MY_LOGE("Unsupported version:0x%x", device_version);
        return  -EOPNOTSUPP;
    }
    //
    if  ( pDevice == 0 )
    {
        MY_LOGE("device creation failure");
        return  -ENODEV;
    }
    //
    //  [4] open device successfully.
    {
        *device = const_cast<hw_device_t*>(pDevice->get_hw_device());
        //
        pDevice->set_hw_module(module);
        pDevice->set_module_callbacks(mpModuleCallbacks);
        pDevice->setDeviceManager(this);
        //
        //attachDeviceLocked(pDevice, device_version);
        //sp<ExternalDeviceInfo> pExternalDeviceInfo = mExternalDeviceInfoMap.valueFor(i4OpenIndex);
        sp<ExternalDeviceInfo> pExternalDeviceInfo = mExternalDeviceInfoMap.valueFor(i4OpenId);
        pExternalDeviceInfo->pLocalDevice = pDevice;
        pExternalDeviceInfo->isOpen = true;
        pExternalDeviceInfo->i8OpenTimestamp = ::systemTime();
    }
    //
    return  OK;
}

/******************************************************************************
 *
 ******************************************************************************/
status_t
CamDeviceManagerBase::
closeExternalDevice (
    android::sp<ICamDevice> pDevice
)
{
    //
    int32_t const openId = pDevice->getOpenId();
    //
    sp<ExternalDeviceInfo> pExternalInfo = mExternalDeviceInfoMap.valueFor(openId);
    if  ( pExternalInfo == 0 ||  pExternalInfo->pLocalDevice != pDevice ) {
        MY_LOGE("device %d: not found!!! mExternalDeviceInfoMap.size:%d pExternalInfo:%p", openId, mExternalDeviceInfoMap.size(), pExternalInfo.get());
        MY_LOGE_IF(pExternalInfo != 0, "device %p %p", pExternalInfo->pExternalDevice.get(), pDevice.get());
        return  NAME_NOT_FOUND;
    }
    //
    if (pDevice != 0 &&  pExternalInfo->uDeviceVersion == CAMERA_DEVICE_API_VERSION_1_0) {
         MY_LOGE("Unsupported device version.");
         return  NAME_NOT_FOUND;
    }
    //
    if ( pExternalInfo->isDisconnected ) {
        MY_LOGD("External deviceId:%d device_version:0x%x", openId, pExternalInfo->uDeviceVersion);
        NSMetadataProviderManager::remove(openId);
        mExternalDeviceInfoMap.removeItem(openId);
        mExternalOrderList.removeAt(openId);
        mpModuleCallbacks->camera_device_status_change(mpModuleCallbacks, openId, CAMERA_DEVICE_STATUS_PRESENT);
    }
    pExternalInfo->pLocalDevice = NULL;
    pExternalInfo->isOpen = false;
    MY_LOGD("device: %s %d isOpen:%d", pDevice->getDevName(), pDevice->getOpenId(), pExternalInfo->isOpen);
    //
    return  OK;
}
