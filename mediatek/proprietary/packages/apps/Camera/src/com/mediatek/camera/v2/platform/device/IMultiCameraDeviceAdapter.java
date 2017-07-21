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
package com.mediatek.camera.v2.platform.device;

import java.util.LinkedHashMap;


/**
 * This class used for connect Cross Mount Framework, such as
 * scan/connect/disconnect remote Camera.
 */
public interface IMultiCameraDeviceAdapter {

    // Remote Camera name/id/status key
    public static final String REMOTE_DEVICE_NAME_KEY = "remote_device_name_key";
    public static final String REMOTE_CAMERA_ID_KEY = "remote_camera_id_key";
    public static final String REMOTE_DEVICE_STATUS_KEY = "remote_device_status_key";
    public static final String REMOTE_DEVICE_TYPE_KEY = "remote_device_type_key";
    public static final String REMOTE_DEVICE_SERIAL_KEY = "remote_device_serial_key";

    // Status Value
    public static final int REMOTE_CAMERA_STATUS_OCCUPIED = 0;
    public static final int REMOTE_CAMERA_STATUS_AVALIABLE = 1;
    public static final int REMOTE_CAEMRA_STATUS_CONNECTING = 2;
    public static final int REMOTE_CAEMRA_STATUS_CONNECTED = 3;
    public static final int REMOTE_CAEMRA_STATUS_UNKNOWN = -1;

    //Define Device Type
    public static final int REMOTE_DEVICE_TYPE_PHONE = 0;
    public static final int REMOTE_DEVICE_TYPE_TV = 1;
    public static final int REMOTE_DEVICE_TYPE_TABLET = 2;
    public static final int REMOTE_DEVICE_TYPE_WATCH = 3;
    public static final int REMOTE_DEVICE_TYPE_UNKNOWN = -1;

    //Define serial unknown value.
    public static final int REMOTE_DEVICE_SERIAL_UNKNOWN = -1;

    /**
     * Device status change listener.
     */
    public interface IDevicesStatusListener {
        /**
         * When cross mount scan result update, onAvalibleDevicesUpdate() will be called to
         * update devices info.
         * @param deviceInfo  The result devices info.
         */
        public void onAvalibleDevicesUpdate(LinkedHashMap<String, IRemoteDevice> deviceInfo);
        /**
         * When cross mount connect result update, onConnectedDeviesUpdate() will be called to
         * update devices info.
         * @param deviceInfo The result devices info.
         */
        public void onConnectedDeviesUpdate(LinkedHashMap<String, IRemoteDevice> deviceInfo);
    }

    /**
     * Register Cross mount status update listener.
     * @param listener The device status listener instance for receive device status change.
     */
    public void regisiterStatusUpdateListener(IDevicesStatusListener listener);

    /**
     * Start scan available devices.
     */
    public void startScanDevice();

    /**
     * Stop scan available devices.
     */
    public void stopScanDevice();

    /**
     * Connect selected device.
     * @param deviceKey The selected device key.
     */
    public void connectSelectedDevice(String deviceKey);

    /**
     * Close connected remote camera service.
     */
    public void disconnectAllRemoteCamera();

    /**
     * Release resource.
     */
    public void release();
}
