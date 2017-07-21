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

#ifndef _MTK_HARDWARE_INCLUDE_MTKCAM_DEVICE_IEXTERNALDEVICE_H_
#define _MTK_HARDWARE_INCLUDE_MTKCAM_DEVICE_IEXTERNALDEVICE_H_
//
#include <utils/Errors.h>
#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <hardware/camera_common.h>
#include <common.h>
#include <v3/stream/IStreamBuffer.h>
#include <camera/CameraMetadata.h>

/******************************************************************************
 *
 ******************************************************************************/

namespace NSCam {

class  IResultCallback;
struct Result;
struct Request;
struct RepeatingRequest;
struct Parcel;
struct Buffer;
struct ImageInfo;

/******************************************************************************
 *
 ******************************************************************************/
class IExternalDevice 
    : public virtual android::RefBase
    , public android::IInterface
{
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Interfaces.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:     ////                    
    DECLARE_META_INTERFACE(ExternalDevice);
    /**
     * DirectRenderer can't destroy before closeDevice() called.
     *
     * @return
     *      0 indicates success; otherwise failure.
     */
    virtual android::status_t  closeDevice ()                                                    = 0;

    /**
     * Camera will call openDevice to notify DirectRenderer that it can start to 
     * send preview buffers.
     *
     * @return
     *      0 indicates success; otherwise failure.
     */
    virtual android::status_t  openDevice ()                                                     = 0;

    /**
     * Set IResultCallback to DirectRenderer.
     *
     * @param[in] cb : DirectRenderer use this to callback buffer to camera.
     *
     * @return
     *      0 indicates success; otherwise failure.
     */
    virtual android::status_t  setCallback ( android::wp< IResultCallback >& cb)                 = 0;

    /**
     * Camera release buffer to DirectRenderer after finish using buffer.
     * 
     * @param[in] buffer : buffer that needs to be returned to DirectRenderer.
     *
     * @return
     *      0 indicates success; otherwise failure.
     */
    virtual android::status_t  releaseBuffer ( Buffer* buffer )                                  = 0;

    /**
     * Camera HAL will call submitRepeatingRequest() when receiving repeating request
     * submitRepeatingRequest() will send a RepeatingRequest structure to DirectRenderer.
     * directRenderer pass to remote AP
     *
     * @param[in] request : request for DirectRenderer.
     *
     * @return
     *      0 indicates success; otherwise failure.
     */
    virtual android::status_t  submitRepeatingRequest ( RepeatingRequest* request )              = 0;

    /**
     * Camera will call submitRequest() when it needs buffer which is not YUV format
     * submitRequest() will send a Request structure to DirectRenderer.
     * directRenderer should check Vector<ImageInfo> info in Request structure for
     * the requesting format.
     *
     * @param[in] request : request for DirectRenderer.
     *
     * @return
     *      0 indicates success; otherwise failure.
     */
    virtual android::status_t  submitRequest ( Request* request )                                = 0;

    /**
     * Camera will send configure stream info to DirectRenderer when AP start configuration.
     * DirectRenderer should send the information to remote device's AP.
     *
     * @param[in] info : configuration information.
     *
     * @return
     *      0 indicates success; otherwise failure.
     */
    virtual android::status_t  configureStreams ( ImageInfo* info )                              = 0;

    /**
     * To Communicate with external devices' AP.
     * DirectRenderer should pass {uint32_t code, const Parcel& data} to external devices
     * and return {Parcel* reply} to camera.
     *
     * @param[in] code : code to notify external device's AP.
     * @param[in] data : data for external device.
     *
     * @param[out] reply : reply from external device.
     *
     * @return
     *      0 indicates success; otherwise failure.
     */
    virtual android::status_t  transact ( uint32_t code,
                                          const Parcel& data, 
                                          Parcel* reply )                                        = 0;
};

/******************************************************************************
 *
 ******************************************************************************/
class BnExternalDevice : public android::BnInterface< IExternalDevice >
{
public:
    virtual android::status_t    onTransact( uint32_t code,
                                             const android::Parcel& data,
                                             android::Parcel* reply,
                                             uint32_t flags = 0);
};

/******************************************************************************
 *
 ******************************************************************************/
class IResultCallback
    : public android::IInterface
{
public:
    DECLARE_META_INTERFACE(ResultCallback);
    /**
     * Return Result structure to camera.
     * MUST NEED data in Result structure including:
     *    (1) timestamp : filled by decoder.
     *    (2) buffers : Buffer structure that DirectRenderer need to fill including
     *                     - status : buffer status OK or ERROR
     *                     - validBitstreamSize : need to be filled if buffer format is JPEG
     *                     - imageInfo : format of image & width & height
     *                     - ionFd : DirectRenderer buffer fd
     *                     - crossMountVA : DirectRenderer buffer va
     *
     * @param[in] result : result from external device.
     *
     */
    virtual android::status_t onResultReceived ( Result* result )                                = 0;
};


/******************************************************************************
 *
 ******************************************************************************/
class BnResultCallback : public android::BnInterface< IResultCallback >
{
public:
    virtual android::status_t    onTransact( uint32_t code,
                                             const android::Parcel& data,
                                             android::Parcel* reply,
                                             uint32_t flags = 0);
};

struct ImageInfo
{
    /**
     * Store image information.
     */
    int width, height;
    int thumbnailWidth, thumbnailHeight;
    int format;
    int planeCount;
    int imgStrideInBytes[3];
    //
    // Parcelable
    android::status_t writeToParcel(android::Parcel& data) {
        data.writeInt32(width);
        data.writeInt32(height);
        data.writeInt32(thumbnailWidth);
        data.writeInt32(thumbnailHeight);
        data.writeInt32(format);
        data.writeInt32(planeCount);
        data.writeInt32(imgStrideInBytes[0]);
        data.writeInt32(imgStrideInBytes[1]);
        data.writeInt32(imgStrideInBytes[2]);
        //
        return OK;
    };
    //
    android::status_t readFromParcel(const android::Parcel* data) {
        data->readInt32(&width);
        data->readInt32(&height);
        data->readInt32(&thumbnailWidth);
        data->readInt32(&thumbnailHeight);
        data->readInt32(&format);
        data->readInt32(&planeCount);
        data->readInt32(&imgStrideInBytes[0]);
        data->readInt32(&imgStrideInBytes[1]);
        data->readInt32(&imgStrideInBytes[2]);
        //
        return OK;
    };
};


struct  Status
{
    /**
     * @param Buffer status.
     */
    enum T
    {
        BUFFER_OK,
        BUFFER_ERROR,
    };
};

struct Buffer
{
    /**
     * Buffer structure for all image format.
     */

    /**
     * @param mark buffer status. Ex. OK, ERROR.
     *        Filled by DirectRenderer.
     */
    Status::T status;

    /**
     * @param store valid bit stream size for JPEG.
     *        Filled by DirectRenderer.
     */
    uint32_t bitstream_size;

    /**
     * @param buffer fd. Filled by DirectRenderer.
     */
    int32_t ionFd;

    /**
     * @param buffer va. Filled by DirectRenderer.
     */
    uintptr_t crossMountVA;

    /**
     * @param buffer information, format, width, height. 
     *        Filled by DirectRenderer.
     */
    ImageInfo imageInfo;
    //
    // Parcelable
    android::status_t writeToParcel(android::Parcel& data) {
        int buf_status = (status&Status::BUFFER_OK) ? 0 : 1;
        data.writeInt32(buf_status);
        data.writeInt32(bitstream_size);
        data.writeInt32(ionFd);
        data.writeInt64(crossMountVA);
        imageInfo.writeToParcel(data);
        //
        return OK;
    };
    //
    android::status_t readFromParcel(const android::Parcel* data) {
        int buf_status;
        data->readInt32(&buf_status);
        status = (buf_status == 0) ? 
                    Status::BUFFER_OK : 
                    Status::BUFFER_ERROR;
        data->readInt32((int*)&bitstream_size);
        data->readInt32(&ionFd);
        data->readInt64((int64_t*)&crossMountVA);
        imageInfo.readFromParcel(data);
        //
        return OK;
    };
};

struct Request
{
    /**
     * Non-repeating request for external device.
     * For capture request : info, setting will be set & isRepeating = FALSE.
     *
     * Capture request: camera_meta_t + repeating + ImageInfo
     * Capture response: camera_meta_t + YUV or JPEG buffer
     *
     * Note that Request is not per-frame sending information.
     */

    /**
     * @param Image information for current request.
     *        Filled by camera.
     *        DirectRenderer should check Vector<ImageInfo> info for the requesting format.
     *
     *        Camera will call submitRequest() and set ImageInfo in format of
     *        JPEG or YUV when capture. DirectRenderer send JPEG buffer by using onResultReceived() in
     *        IResultCallback.
     */
    ImageInfo info;

    /**
     * @param request setting set to external device when
     *            1. first repeating request
     *            2. capture request
     */
    camera_metadata* settings;

    /**
     * @param notify external device if the setting is repeating or not
     */
    bool isRepeating;
    //
    // Parcelable
    android::status_t writeToParcel(android::Parcel& data) {
        data.writeInt32(isRepeating);
        //
        android::CameraMetadata meta = android::CameraMetadata(settings);
        meta.writeToParcel(&data);
        //
        info.writeToParcel(data);
        //
        return OK;
    };
    //
    android::status_t readFromParcel(const android::Parcel* data) {
        data->readInt32((int*)&isRepeating);
        //
        android::CameraMetadata meta;
        meta.readFromParcel(const_cast<android::Parcel*>(data));
        settings = meta.release();
        //
        info.readFromParcel(data);
        //
        return OK;
    };
};

struct RepeatingRequest
{
    /**
     * Repeating request for external device.
     * For preview request : setting will be set & isRepeating = FALSE. No ImageInfo in info vector.
     *
     * Preview request: camera_meta_t + repeating
     * Preview response: camera_meta_t + frame(from playback session)
     *
     * Note that Request is not per-frame sending information.
     */

    /**
     * @param request setting set to external device when
     *            1. first repeating request
     *            2. capture request
     */
    camera_metadata* settings;

    /**
     * @param notify external device if the setting is repeating or not
     */
    bool isRepeating;
    //
    // Parcelable
    android::status_t writeToParcel(android::Parcel& data) {
        data.writeInt32(isRepeating);
        //
        android::CameraMetadata meta = android::CameraMetadata(settings);
        meta.writeToParcel(&data);
        //
        return OK;
    };
    //
    android::status_t readFromParcel(const android::Parcel* data) {
        data->readInt32((int*)&isRepeating);
        //
        android::CameraMetadata meta;
        meta.readFromParcel(const_cast<android::Parcel*>(data));
        settings = meta.release();
        //
        return OK;
    };
};


struct Result
{
    /**
     * Result from external device.
     */

    /**
     * @param time stamp for result buffer. Filled by decoder.
     */
    uintptr_t timestamp;

    /**
     * @param The orientation of the remote device.
     */
    //int deviceOrientation;

    /**
     * @param result data. If there are some information that needs to be return
     *        It can store in settings.
     */
    camera_metadata* result;

    /**
     * @param Buffer for current result.
     */
    Buffer buffers;
    //
    // Parcelable
    android::status_t writeToParcel(android::Parcel& data) {
        data.writeInt64(timestamp);
        //
        android::CameraMetadata meta = android::CameraMetadata(result);
        meta.writeToParcel(&data);
        //
        buffers.writeToParcel(data);
        //
        return OK;
    };
    //
    android::status_t readFromParcel(const android::Parcel* data) {
        data->readInt64((int64_t*)&timestamp);
        //
        android::CameraMetadata meta;
        meta.readFromParcel(const_cast<android::Parcel*>(data));
        result = meta.release();
        //
        buffers.readFromParcel(data);
        //
        return OK;
    };
};

struct Parcel
{
    /**
     * Parcel for transact().
     */
    
    /**
     * @param data that wants to pass to external device.
     *        Filled by camera
     */
    android::Vector<uint8_t> mData;

    /**
     * @param size for mData in byte.
     *        Filled by camera
     */
    //uint64_t mDataSize;
};


struct CameraInfo {
    /**
     * Static information of camera.
     *
     * DirectRenderer send these information when connecting.
     * The information only need to be sent once.
     *    -orientation & cameraCharacteristics are given from remote device. 
     *    -supprotFormat & supprotSize should be filled by DirectRenderer. To notify
     *     camera HAL wifi decode format & size (ex. HAL_PIXEL_FORMAT_YV12 1920x1080) etc.
     */

    /**
     * @param The orientation of the camera image. sensor orientation.
     *        Reference : http://developer.android.com/reference/android/hardware/Camera.CameraInfo.html
     *                    public int	orientation	
     */
    int orientation;

    /**
     * @param Remote device capability. 
     *        e.g. supported format & size etc.
     *        call getCameraCharacteristic in camera service
     *
     *        Reference : alps\system\media\camera\include\system\camera_metadata.h
     */
    camera_metadata* cameraCharacteristics;

    /**
     * @param Remote device supported format. 
     *        (ex.Wifi decode YUV format)
     */
    //Vector<int> supprotFormat;

    /**
     * @param Remote device supported size. 
     *        Wifi decode YUV buffer size --> first two arguments
     *        capture jpeg size --> 3rd & 4th 
     */
    android::Vector<int> supprotSize;
    //
    // Parcelable
    android::status_t writeToParcel(android::Parcel& data) {
        data.writeInt32(orientation);
        //
        android::CameraMetadata meta = android::CameraMetadata(cameraCharacteristics);
        meta.writeToParcel(&data);
        //
        data.writeInt32(supprotSize.size());
        for (size_t i = 0; i < supprotSize.size(); ++i)
            data.writeInt32(supprotSize[i]);
        //
        return OK;
    };
    //
    android::status_t readFromParcel(const android::Parcel* data) {
        data->readInt32(&orientation);
        //
        android::CameraMetadata meta;
        meta.readFromParcel(const_cast<android::Parcel*>(data));
        cameraCharacteristics = meta.release();
        //
        size_t size = data->readInt32();
        for (size_t i = 0; i < size; ++i)
            supprotSize.push_back(data->readInt32());
        //
        return OK;
    };
};


/******************************************************************************
 *
 ******************************************************************************/
class IAppCallback
    : public virtual android::RefBase
{
public:     ////            Operations.
    virtual MVOID   updateFrame(
                        MUINT32 const requestNo,
                        MINTPTR const userId,
                        bool    const lastPart,
                        android::Vector<android::sp<v3::IMetaStreamBuffer> > vOutMeta
                    )                                                                            = 0;

    virtual         ~IAppCallback() {}
};

};  //namespace NSCam
#endif  //_MTK_HARDWARE_INCLUDE_MTKCAM_DEVICE_IEXTERNALDEVICE_H_

