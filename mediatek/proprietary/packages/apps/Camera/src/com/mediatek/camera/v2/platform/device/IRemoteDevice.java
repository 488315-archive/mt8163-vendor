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


/**
 * This class is a abstract of remote device for cross mount.
 */
public interface IRemoteDevice {

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


    public static final Key<String> KEY_REMOTE_DEVICE_ID =
            new Key<String>("key_remote_device_id", String.class);

    public static final Key<String> KEY_REMOTE_DEVICE_NAME =
            new Key<String>("key_remote_device_name", String.class);

    public static final Key<Integer> KEY_REMOTE_CAMERA_ID =
            new Key<Integer>("key_remote_camera_id", int.class);

    public static final Key<Integer> KEY_REMOTE_SERVICE_STATUS =
            new Key<Integer>("key_remote_service_status", int.class);

    public static final Key<Integer> KEY_REMOTE_DEVICE_TYPE =
            new Key<Integer>("key_remote_device_type", int.class);

    public static final Key<Integer> KEY_REMOTE_DEVICE_SERIAL =
            new Key<Integer>("key_remote_device_serial", int.class);

    /**
     * The class used to define a property of remote device.
     * @param <T>
     */
    public static final class Key<T> {

        private final String mName;
        private final Class<T> mType;

        /**
         * The constructor of the Key class.
         * @param name The name of the property.
         * @param type The value's type of the property.
         */
        public Key(String name, Class<T> type) {
            mName = name;
            mType = type;
        }

        public String getName() {
            return mName;
        }
    }


    /**
     * Get the property value of remote device.
     * @param key The key of the property.
     * @param <T> The type of the property value.
     * @return The value of the property.
     */
    public <T> T get(Key<T> key);

    /**
     * Mount the device's camera. When the connection is established,
     * the host device can receive the camera preview data of remote device.
     */
    public void mountCamera();

    /**
     * Unmount the device's camera.
     */
    public void unmountCamera();

    /**
     * Print the device's properties.
     */
    public void dumpInfo();
}