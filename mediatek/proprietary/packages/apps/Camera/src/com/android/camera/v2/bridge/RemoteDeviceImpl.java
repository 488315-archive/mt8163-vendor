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
 * MediaTek Inc. (C) 2015. All rights reserved.
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
package com.android.camera.v2.bridge.device;


import android.util.Log;

import com.mediatek.camera.v2.platform.device.IRemoteDevice;
import com.mediatek.camera.v2.platform.device.IRemoteDevice.Key;

import com.mediatek.crossmount.adapter.Device;
import com.mediatek.crossmount.adapter.Device.Type;
import com.mediatek.crossmount.adapter.Service;
import com.mediatek.crossmount.adapter.Service.ServiceState;

/**
 * The implement of the IRemoteDevice.java, the class represent a remote device.
 * Supply APIs to get device's properties, mount/unmount camera of remote device
 * and dump properties information.
 */
public class RemoteDeviceImpl implements IRemoteDevice {
    private static final String TAG = "RemoteDeviceImpl";

    private Device mDevice;
    private Service mCameraService;
    private Integer mCameraMappingID;

    /**
     * Constructor of the RemoteDeviceImpl class.
     * @param device The device instance that get from cross mount framework.
     * @param service The camera service.
     * @param cameraMappingID The camera mapping id of remote device.
     */
    public RemoteDeviceImpl(Device device, Service service, Integer cameraMappingID) {
        mDevice = device;
        mCameraService = service;
        mCameraMappingID = cameraMappingID;
    }

    //TDO:use a list to maintain the data.
    @Override
    public <T> T get(Key<T> key) {
        if (KEY_REMOTE_DEVICE_NAME.getName().equals(key.getName())) {
            return (T) getDeviceName(mDevice);
        } else if (KEY_REMOTE_CAMERA_ID.getName().equals(key.getName())) {
            return (T) getCameraId(mCameraService);
        } else if (KEY_REMOTE_SERVICE_STATUS.getName().equals(key.getName())) {
            return (T) getCameraServiceState(mCameraService);
        } else if (KEY_REMOTE_DEVICE_TYPE.getName().equals(key.getName())) {
            return (T) getDeviceType(mDevice);
        } else if (KEY_REMOTE_DEVICE_SERIAL.getName().equals(key.getName())) {
            return (T) getSerialNumber(mCameraService);
        } else {
            return null;
        }
    }

    @Override
    public void mountCamera() {
        mCameraService.mountFrom();
    }

    @Override
    public void unmountCamera() {
        mCameraService.stopMountFrom();
    }

    @Override
    public void dumpInfo() {
        StringBuilder stringBuilder = new StringBuilder();

        String deviceId = mDevice.getId();
        String name = mDevice.getName();
        Type type = mDevice.getType();
        int status = getCameraServiceState(mCameraService);
        int cameraID = mCameraService.getCameraId();
        int serialNumber = mCameraService.getSerialNumber();
        int mappingValue = mCameraMappingID;

        stringBuilder.append("key = ").append(deviceId).append(" name =")
                .append(name).append(" Type = ").append(type)
                .append(" status = ").append(status).append(" cameraId = ")
                .append(cameraID).append(" serialNumber = ")
                .append(serialNumber).append(" mappingvalue = ")
                .append(mappingValue);
        Log.i(TAG, "dumpInfo deviceId " + stringBuilder.toString());
    }


    private String getDeviceName(Device device) {
        return device.getName();
    }

    private Integer getCameraId(Service service) {
        return service.getCameraId();
    }


    private Integer getCameraServiceState(Service service) {
        ServiceState state = service.getState(null);
        switch (state) {
        case OCCUPIED:
            return REMOTE_CAMERA_STATUS_OCCUPIED;
        case AVAILABLE:
            return REMOTE_CAMERA_STATUS_AVALIABLE;
        case CONNECTING:
            return REMOTE_CAEMRA_STATUS_CONNECTING;
        case CONNECTED:
            return REMOTE_CAEMRA_STATUS_CONNECTED;
         default:
            return REMOTE_CAEMRA_STATUS_UNKNOWN;
        }
    }

    private Integer getDeviceType(Device device) {
        Type type = device.getType();
        Log.i(TAG, "getDeviceType type " + type);
        switch (type) {
        case PHONE:
            return REMOTE_DEVICE_TYPE_PHONE;
        case TV:
            return REMOTE_DEVICE_TYPE_TV;
         default:
            return REMOTE_DEVICE_TYPE_UNKNOWN;
        }
    }

    private Integer getSerialNumber(Service service) {
        return service.getSerialNumber();
    }
}