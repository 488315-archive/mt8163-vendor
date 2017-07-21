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
import android.content.Context;
import android.hardware.display.DisplayManager;
import android.util.Log;
import android.view.ViewGroup;

import com.mediatek.camera.v2.platform.device.IRemoteDevice;
import com.mediatek.camera.v2.stream.multicamera.ui.DeviceTagContainer.IDeviceTagListener;
import com.mediatek.camera.v2.stream.multicamera.ui.OperateView.IOperateViewListener;
import com.mediatek.camera.v2.stream.multicamera.ui.ScanView.IScanViewListener;
import com.mediatek.camera.v2.util.Utils;

import java.util.LinkedHashMap;
import java.util.List;

/**
 * A manager to multi camera views.
 */
public class MultiCameraViewManager implements IMultiCameraViewManager {

    private static final String TAG = "MultiCameraViewManager";
    private static final int ORIENTATION_360 = 360;
    private Activity mActivity;
    private ViewGroup mParentView;

    private IMultiCameraView mIScanView;
    private IMultiCameraView mOperateView;
    private IMultiCameraView mDeviceTagView;

    private int mOrientationCompensation = -1;
    private int mDisplayRotation;
    // when user click the add/scan/multi ->single preview icon, need
    // notify this listener to do something.
    // this listener will be notify module -> platform - > Cross Mount Framework
    private IMultiCameraViewListener mViewListener;

    /**
     * View manager constructor.
     * @param viewListener View manager call back for moudle.
     */
    public MultiCameraViewManager(IMultiCameraViewListener viewListener) {
        Log.i(TAG, "[MultiCameraViewManager]");
        mIScanView = new ScanView(new ScanViewListener());
        mOperateView = new OperateView(new OperateViewListener());
        mDeviceTagView = new DeviceTagContainer(new DeviceTagViewListener());
        mViewListener = viewListener;
        mViewListener.regisiterListener(new StatusUpdateListenerImpl());
    }

    @Override
    public void open(Activity activity, ViewGroup parentView) {
        Log.i(TAG, "[open]");
        mActivity = activity;
        mParentView = parentView;

        mDeviceTagView.open(mActivity, mParentView);
        mOperateView.open(mActivity, mParentView);
        mIScanView.open(mActivity, mParentView);

        ((DisplayManager) mActivity
                .getSystemService(Context.DISPLAY_SERVICE))
                .registerDisplayListener(mDisplayListener, null);
    }

    @Override
    public void resume() {
        Log.i(TAG, "[resume]");
        mIScanView.resume();
        mOperateView.resume();
        mDeviceTagView.resume();
    }

    @Override
    public void pause() {
        mIScanView.pause();
        mOperateView.pause();
        mDeviceTagView.pause();
    }

    @Override
    public void close() {
        mIScanView.close();
        mOperateView.close();
        mDeviceTagView.close();
        ((DisplayManager) mActivity
                .getSystemService(Context.DISPLAY_SERVICE))
                .unregisterDisplayListener(mDisplayListener);
    }

    @Override
    public void onOrientationChanged(int gsensorOrientation) {
        int orientationCompensation = (gsensorOrientation +
                Utils.getDisplayRotation(mActivity)) % ORIENTATION_360;

        if (mOrientationCompensation != orientationCompensation) {
            Log.i(TAG, "[onOrientationChanged] orientationCompensation "
                    + orientationCompensation);
            mIScanView.onOrientationChanged(orientationCompensation);
            mOperateView.onOrientationChanged(orientationCompensation);
            mDeviceTagView.onOrientationChanged(orientationCompensation);
            mOrientationCompensation = orientationCompensation;
        }
    }

    @Override
    public boolean onSingleTapUp(float x, float y) {
        Log.i(TAG, "[onSingleTapUp]");
        mDeviceTagView.onSingleTapUp(x, y);
        return false;
    }

    @Override
    public void updatePreviewCamera(List<String> cameraId, boolean isRemoteOpened) {
        Log.i(TAG, "[updatePreviewCamera] cameraId = " + cameraId
                + ",isRemoteOpened = " + isRemoteOpened);

        final List<String> list = cameraId;
        final boolean opened = isRemoteOpened;

        mActivity.runOnUiThread(new Runnable() {

            @Override
            public void run() {
                mIScanView.updatePreviewCamera(list, opened);
                mDeviceTagView.updatePreviewCamera(list, opened);
                mOperateView.updatePreviewCamera(list, opened);
            }
        });
    }


    @Override
    public void updateRecordingStatus(boolean isStarted) {
        Log.i(TAG, "[updateRecordingStatus] isStarted = " + isStarted);
        if (isStarted) {
            mOperateView.hide();
        } else {
            mOperateView.show();
        }
    }

    @Override
    public void updateCapturingStatus(final boolean isCapturing) {
        Log.i(TAG, "[updateCapturingStatus] isCapturing = " + isCapturing);
        // current just operate view need this info for disable the operate
        // view, such as add/disconnect/back to multi-preview icon and set alpha
        // for the image icon
        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mOperateView.updateCapturingStatus(isCapturing);
            }
        });
    }

    @Override
    public void onLayoutOrientationChanged(boolean isLandscape) {
        mDeviceTagView.onLayoutOrientationChanged(isLandscape);
        mOperateView.onLayoutOrientationChanged(isLandscape);
    }

    private DisplayManager.DisplayListener mDisplayListener = new DisplayManager.DisplayListener() {
        @Override
        public void onDisplayAdded(int arg0) {
            // Do nothing.
        }
        @Override
        public void onDisplayChanged(int displayId) {
            mDisplayRotation = Utils.getDisplayRotation(mActivity);
            Log.i(TAG, "[onDisplayChanged]- mDisplayRotation:" + mDisplayRotation);
            mDeviceTagView.onDisplayChanged(mDisplayRotation);
        }
        @Override
        public void onDisplayRemoved(int arg0) {
            // Do nothing.
        }
    };

    /**
     * Device status change listener.
     */
    private class StatusUpdateListenerImpl implements IStatusUpdateListener {
        @Override
        public void onAvalibleDevicesUpdate(
                LinkedHashMap<String, IRemoteDevice> devices) {
            mIScanView.onAvalibleDevicesUpdate(devices);
            mOperateView.onAvalibleDevicesUpdate(devices);
            mDeviceTagView.onAvalibleDevicesUpdate(devices);
        }

        @Override
        public void onConnectedDeviesUpdate(
                LinkedHashMap<String, IRemoteDevice> devices) {
            mIScanView.onConnectedDeviesUpdate(devices);
            mOperateView.onConnectedDeviesUpdate(devices);
            mDeviceTagView.onConnectedDeviesUpdate(devices);
        }
    }

    /**
     * Scan view listener.
     */
    private class ScanViewListener implements IScanViewListener {
        @Override
        public void onDiscoverStated() {
            mViewListener.startScanRemoteDevice();
        }
        @Override
        public void onDiscoverStoped() {
            mViewListener.stopScanRemoteDevice();
        }
        @Override
        public void onDeviceCameraSelected(String deviceKey) {
            mViewListener.connectRemoteDevice(deviceKey);
        }
        @Override
        public void onDeviceCameraConnected(String cameraId, String deviceId) {
            mViewListener.openRemoteDevice(cameraId, deviceId);
        }
        @Override
        public void onScanViewDissmiss() {
            mOperateView.show();
            mDeviceTagView.show();
        }
        @Override
        public void updateCommonUiVisibility(boolean show) {
            mViewListener.updateAllViewVisibility(show);
        }
    }

    /**
     * Operate view listener.
     */
    private class OperateViewListener implements IOperateViewListener {
        @Override
        public void onDisconnectButtonClick() {
            mViewListener.closeAllRemoteCameras();
        }
        @Override
        public void onScanButtonClick() {
            mIScanView.show();
            mOperateView.hide();
            mDeviceTagView.hide();
        }
        @Override
        public void onMultiPreviewButtonClick() {
            Log.i(TAG, "[onMultiPreviewButtonClick]");
            mViewListener.backToMultiPreview();
        }
    }

    /**
     * Device tag view listener.
     */
    private class DeviceTagViewListener implements IDeviceTagListener {
        @Override
        public void onCameraSelected(String cameraId, String deviceId) {
            mViewListener.openRemoteDevice(cameraId, deviceId);
        }
    }
}
