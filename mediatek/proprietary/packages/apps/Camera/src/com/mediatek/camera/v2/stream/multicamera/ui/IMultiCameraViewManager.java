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
package com.mediatek.camera.v2.stream.multicamera.ui;

import android.app.Activity;
import android.view.ViewGroup;

import com.mediatek.camera.v2.platform.device.IRemoteDevice;

import java.util.LinkedHashMap;
import java.util.List;



/**
 * The class is define to the multi camera view manager.
 * It provider manager view interface to multi camera mode.
 */
public interface IMultiCameraViewManager {

    /**
     * It define a serial interface for view manager notify view
     * response events to the module.
     */
    public interface IMultiCameraViewListener {

        /**
         * Register device status change listener.
         * @param listener Listener instance.
         */
        public void regisiterListener(IStatusUpdateListener listener);

        /**
         * Start to scan remote device.
         */
        public void startScanRemoteDevice();

        /**
         * Stop to scan remote device.
         */
        public void stopScanRemoteDevice();

        /**
         * Connect a remote device camera service.
         * @param deviceKey The device identity key.
         */
        public void connectRemoteDevice(String deviceKey);

        /**
         * Open a remote device camera.
         * @param cameraID The remote camera ID.
         * @param deviceId The remote device identity key.
         */
        public void openRemoteDevice(String cameraID, String deviceId);

        /**
         * Close all opened remote cameras.
         */
        public void closeAllRemoteCameras();

        /**
         * Restore to multi preview state.
         */
        public void backToMultiPreview();

        /**
         * Update camera common UI visibility state.
         * @param visible common UI visibility state.
         */
        public void updateAllViewVisibility(boolean visible);
    }

    /**
     * Device status change listener.
     */
    public interface IStatusUpdateListener {
        /**
         * When cross mount scan result update, onAvalibleDevicesUpdate() will be called to
         * update devices info.
         * @param devices  The result devices info.
         */
        public void onAvalibleDevicesUpdate(
                LinkedHashMap<String, IRemoteDevice> devices);

        /**
         * When cross mount connect result update, onConnectedDeviesUpdate() will be called to
         * update devices info.
         * @param devices The result devices info.
         */
        public void onConnectedDeviesUpdate(
                LinkedHashMap<String, IRemoteDevice> devices);
    }

    // life cycle
    /**
     * Life cycle of the view manager, when enter to the multi camera mode,
     * open() will be called to prepare the views.
     * @param activity  Activity instance.
     * @param parentView Parent view group.
     */
    public void open(Activity activity, ViewGroup parentView);

    /**
     * Life cycle of the view manager, when activity execute onResume(),
     * resume() will be called.
     */
    public void resume();

    /**
     * Life cycle of the view manager, when activity execute onPause(),
     * pause() will be called.
     */
    public void pause();

    /**
     * Life cycle of the view manager, when activity execute onDestory(),
     * close() will be called.
     */
    public void close();

    /**
     * When enter camera APP, it will lock the activity orientation(portrait or landscape).
     * When the lock orientation changed, should notify the new orientation to the views
     * which will reinflate layout xml.
     * @param isLandscape new lock orientation.
     */
    public void onLayoutOrientationChanged(boolean isLandscape);

    /**
     * When the rotate the phone, notify the phone gsensor orientation value to views
     * which will rotate themselves by the orientation.
     * @param newOrientation The new orientation.
     */
    public void onOrientationChanged(int newOrientation);

    /**
     * May be useless.
     * @param x May be useless.
     * @param y May be useless.
     * @return May be useless.
     */
    public boolean onSingleTapUp(float x, float y);

    /**
     * Update preview camera list.
     * @param cameraId Current preview camera id list.
     * @param isRemoteOpened  Is there opened remote camera or not.
     */
    public void updatePreviewCamera(List<String> cameraId, boolean isRemoteOpened);

    /**
     * Update recording status.
     * @param isStarted Recording start or not.
     */
    public void updateRecordingStatus(boolean isStarted);

    /**
     * Update Capture status.
     * @param isCapturing capture start or not.
     */
    public void updateCapturingStatus(boolean isCapturing);
}
