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
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.HorizontalScrollView;
import android.widget.LinearLayout;
import android.widget.ProgressBar;

import com.android.camera.R;

import com.mediatek.camera.v2.platform.device.IMultiCameraDeviceAdapter;
import com.mediatek.camera.v2.platform.device.IRemoteDevice;
import com.mediatek.camera.v2.util.Utils;

import java.util.Comparator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

/**
 * Scan view is a view for device scan and connect.
 */
public class ScanView implements IMultiCameraView {
    /**
     * Scan view listener.
     */
    public interface IScanViewListener {
        /**
         * Start the cross mount scan.
         */
        public void onDiscoverStated();
        /**
         * Stop the cross mount scan.
         */
        public void onDiscoverStoped();
        /**
         * When a device is selected, invoke it to notify the event.
         * @param deviceKey The selected camera key value.
         */
        public void onDeviceCameraSelected(String deviceKey);
        /**
         * When the selected camera is connected, invoke it to notify the event.
         * @param cameraId The camera id value of the connected device.
         * @param deviceId The device id value of the connected device.
         */
        public void onDeviceCameraConnected(String cameraId, String deviceId);
        /**
         * When the scan view is hided, invoke it to notify the event.
         */
        public void onScanViewDissmiss();
        /**
         * When the scan view is shown, should hide other UI.
         * When hide scan view, show other UI.
         * @param show Is show common UI no not.
         */
        public void updateCommonUiVisibility(boolean show);
    }

    private static final String TAG = "ScanView";
    private Activity mActivity;
    private ViewGroup mParentView;
    private ViewGroup mScanRootLayout;
    private ViewGroup mScanContainer;
    private ViewGroup mAvailableDevicesView;
    private HorizontalScrollView mScrollContainer;
    private View mDismissView;
    private ProgressBar mScanProgress;
    private LayoutInflater mInflater;

    private SelectorItem mConnectingItem;

    private IScanViewListener mScanViewListener;

    private  String mRecentConnectedKey;
    private  int mBiggestSerialNumber;

    private boolean mIsShowConnectedDevice = false;
    private boolean mIsOpenRecentConnectedDevice = false;

    private LinkedHashMap<String, IRemoteDevice> mDevicesList;

    /**
     * Scan view contructor.
     * @param scanViewListener Scan view lisntener instance.
     */
    public ScanView(IScanViewListener scanViewListener) {
        Log.i(TAG, "ScanView");
        mScanViewListener = scanViewListener;
    }

    @Override
    public void open(Activity activity, ViewGroup parentView) {
        mActivity = activity;
        mParentView = parentView;
        mActivity.getLayoutInflater().inflate(R.layout.multi_camera_scan,
                mParentView, true);
        mScanRootLayout = (ViewGroup) mParentView
                .findViewById(R.id.scan_root_layout);
        mScanContainer = (ViewGroup) mParentView
                .findViewById(R.id.scan_container);
        mDismissView = mScanContainer.findViewById(R.id.dismiss);
        mDismissView.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                mScanViewListener.onScanViewDissmiss();
                hide();
            }
        });
        mScanProgress = (ProgressBar) mScanContainer
                .findViewById(R.id.scan_progress);
        mScrollContainer = (HorizontalScrollView) mScanContainer
                .findViewById(R.id.scroll_container);
        mAvailableDevicesView = (ViewGroup) mScanContainer
                .findViewById(R.id.availableDevices);
        mInflater = (LayoutInflater) mActivity
                .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
        mIsShowConnectedDevice = true;
        mIsOpenRecentConnectedDevice = true;
    }

    @Override
    public void resume() {
        Log.i(TAG, "resume");
        if (mScanRootLayout.getVisibility() == View.VISIBLE) {
            mScanViewListener.updateCommonUiVisibility(false);
        } else {
            mScanViewListener.updateCommonUiVisibility(true);
        }
        mScanViewListener.onDiscoverStated();
    }

    @Override
    public void pause() {
        Log.i(TAG, "pause");
        mScanViewListener.updateCommonUiVisibility(true);
        mScanViewListener.onDiscoverStoped();
    }

    @Override
    public void close() {
        Log.i(TAG, "close");
    }

    @Override
    public void onOrientationChanged(int gsensorOrientation) {
        Utils.setRotatableOrientation(mScanContainer, gsensorOrientation, true);
    }

    @Override
    public void show() {
        mScanRootLayout.setVisibility(View.VISIBLE);
        mScanViewListener.onDiscoverStated();
        // when scan view is showing ,will hide all common UI
        mScanViewListener.updateCommonUiVisibility(false);
    }

    @Override
    public void hide() {
        mScanRootLayout.setVisibility(View.GONE);
        // when scan view is hide ,will show all common UI
        mScanViewListener.updateCommonUiVisibility(true);
    }

    @Override
    public void onLayoutOrientationChanged(boolean isLandscape) {
        // TODO Auto-generated method stub
    }

    @Override
    public void updatePreviewCamera(List<String> cameraId,
            boolean isRemoteOpened) {
        Log.i(TAG, "[updatePreviewCamera] list size = " + cameraId.size()
                + ",RemoteOpened = " + isRemoteOpened);
        if (isRemoteOpened) {
            mIsShowConnectedDevice = false;
        } else {
            mIsShowConnectedDevice = true;
        }
    }

    @Override
    public void updateCapturingStatus(boolean isCapturing) {
        // TODO Auto-generated method stub
    }

    @Override
    public boolean onSingleTapUp(float x, float y) {
        // TODO Auto-generated method stub
        return false;
    }

    @Override
    public void onDisplayChanged(int displayRotation) {
        // TODO Auto-generated method stub
    }

    @Override
    public void onAvalibleDevicesUpdate(
            LinkedHashMap<String, IRemoteDevice> devices) {
        Log.i(TAG, "onAvalibleDevicesUpdate");
        mDevicesList = devices;
        mActivity.runOnUiThread(new Runnable() {
            public void run() {
                initializeAvailableDevices();
            }
        });
    }

    @Override
    public void onConnectedDeviesUpdate(
            LinkedHashMap<String, IRemoteDevice> devices) {
        Log.i(TAG, "onConnectedDeviesUpdate");
        mDevicesList = devices;
        mActivity.runOnUiThread(new Runnable() {
            public void run() {
                int count = mAvailableDevicesView.getChildCount();
                for (int i = 0; i < count; i++) {
                    SelectorItem item = (SelectorItem) mAvailableDevicesView
                            .getChildAt(i);
                    String key = item.getKey();
                    IRemoteDevice device = mDevicesList.get(key);
                    int status = device.get(IRemoteDevice.KEY_REMOTE_SERVICE_STATUS);
                    item.setConnected(isDeviceCameraConnected(status));
                }
                if (mConnectingItem != null) {
                    // check the connecting device status.
                    IRemoteDevice device = mDevicesList.get(mConnectingItem.getKey());

                    int status = device.get(IRemoteDevice.KEY_REMOTE_SERVICE_STATUS);
                    int cameraID = device.get(IRemoteDevice.KEY_REMOTE_CAMERA_ID);

                    Log.i(TAG, "onConnectedDeviesUpdate status " + status
                            + " cameraID " + cameraID);
                    if (isDeviceCameraConnected(status)) {
                        // 1. dismiss the progress.
                        mConnectingItem.showConnecting(false);
                        // 2. show connected background.
                        mConnectingItem.setConnected(true);
                        // notify camera id to module.
                        mScanViewListener.onDeviceCameraConnected(
                                String.valueOf(cameraID),
                                mConnectingItem.getKey());
                        // 3. clear flag.
                        mConnectingItem = null;
                    } else if (isDeviceCameraConnecting(status)) {
                        mConnectingItem.showConnecting(true);
                    } else {
                        mConnectingItem.showConnecting(false);
                        mConnectingItem = null;
                    }
                }
            }
        });
    }

    private void initializeAvailableDevices() {
        Log.i(TAG, "initializeAvailableDevices");
        if (mAvailableDevicesView.getChildCount() != 0) {
            mAvailableDevicesView.removeAllViews();
            mScrollContainer.scrollTo(0, 0); // move to the left of the list.
        }

        if (mDevicesList.isEmpty()) {
            // if device list is empty, show the scan progress.
            mScanProgress.setVisibility(View.VISIBLE);
            mScanRootLayout.setVisibility(View.VISIBLE);
        } else {
            updateAvailableDeviceView();
            if (mIsOpenRecentConnectedDevice) {
                if (mBiggestSerialNumber > IMultiCameraDeviceAdapter.REMOTE_DEVICE_SERIAL_UNKNOWN) {
                    Log.d(TAG, "Find the recent connected device  key "
                            + mRecentConnectedKey + " mBiggestSerialNumber "
                            + mBiggestSerialNumber);
                    IRemoteDevice device = mDevicesList.get(mRecentConnectedKey);
                    int cameraID = device.get(IRemoteDevice.KEY_REMOTE_CAMERA_ID);
                    // notify camera id to module.
                    mScanViewListener.onDeviceCameraConnected(
                            String.valueOf(cameraID), mRecentConnectedKey);
                    mScanViewListener.onScanViewDissmiss();
                } else {
                    mScanRootLayout.setVisibility(View.VISIBLE);
                    mScanViewListener.updateCommonUiVisibility(false);
                    Log.d(TAG, "Do not find recent connected device!");
                }
                mIsOpenRecentConnectedDevice = false;
            }
        }
    }

    private void updateAvailableDeviceView() {
        LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.WRAP_CONTENT,
                LinearLayout.LayoutParams.WRAP_CONTENT);

        mRecentConnectedKey = null;
        mBiggestSerialNumber = IMultiCameraDeviceAdapter.REMOTE_DEVICE_SERIAL_UNKNOWN;

        for (Entry<String, IRemoteDevice> entry : mDevicesList.entrySet()) {
            String key = entry.getKey();
            IRemoteDevice device = entry.getValue();
            int status = device.get(IRemoteDevice.KEY_REMOTE_SERVICE_STATUS);

            if (!mIsShowConnectedDevice && isDeviceCameraConnected(status)) {
                // if need not show the connected device and current device
                // status is connected,
                // do not add the device to the list.
                continue;
            }
            String name = device.get(IRemoteDevice.KEY_REMOTE_DEVICE_NAME);
            int type = device.get(IRemoteDevice.KEY_REMOTE_DEVICE_TYPE);

            final SelectorItem deviceIcon = (SelectorItem) mInflater.inflate(
                    R.layout.multi_camera_device_selector, null, false);
            deviceIcon.setName(name);
            deviceIcon.setKey(key);
            deviceIcon.setImageResource(getDeviceTypeIconResId(type));
            deviceIcon.setConnected(isDeviceCameraConnected(status));

            if (mConnectingItem != null && mConnectingItem.getKey().equals(key)
                    && isDeviceCameraConnecting(status)) {
                deviceIcon.showConnecting(true);
            }

            deviceIcon.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {
                    onItemSelected(deviceIcon);
                }
            });
            mAvailableDevicesView.addView(deviceIcon, params);
            int serialNumber = device
                    .get(IRemoteDevice.KEY_REMOTE_DEVICE_SERIAL);
            if (serialNumber > mBiggestSerialNumber) {
                mRecentConnectedKey = key;
                mBiggestSerialNumber = serialNumber;
            }
            Log.d(TAG, "initializeAvailableDevices [" + key + ", " + name
                    + ", " + type + ", " + status + "]");
        }
        // If has no available devices need to be shown, show scan progress.
        if (mAvailableDevicesView.getChildCount() != 0) {
            mScanProgress.setVisibility(View.GONE);
        } else {
            mScanProgress.setVisibility(View.VISIBLE);
        }
    }

    private void onItemSelected(SelectorItem item) {
        Log.i(TAG, "onItemSelected");
        // check the connecting device status.
        if (mConnectingItem != null) {
            IRemoteDevice device = mDevicesList.get((mConnectingItem.getKey()));
            int status = device.get(IRemoteDevice.KEY_REMOTE_SERVICE_STATUS);
            if (isDeviceCameraConnecting(status)) {
                Log.i(TAG, "onItemSelected already has a connecting device!");
                return;
            }
        }

        IRemoteDevice device = mDevicesList.get((item.getKey()));
        int status = device.get(IRemoteDevice.KEY_REMOTE_SERVICE_STATUS);
        // check the connecting device status.
        if (isDeviceCameraConnected(status)) {
            int cameraID = device.get(IRemoteDevice.KEY_REMOTE_CAMERA_ID);
            // notify camera id to module.
            mScanViewListener.onDeviceCameraConnected(String.valueOf(cameraID),
                    item.getKey());
        } else {
            mConnectingItem = item;
            device.mountCamera();
            if (isDeviceCameraConnecting(status)) {
                mConnectingItem.showConnecting(true);
            }
        }
    }

    private int[] getDeviceTypeIconResId(int type) {
        Log.i(TAG, "getDeviceTypeIconResId type " + type);
        switch (type) {
        case IMultiCameraDeviceAdapter.REMOTE_DEVICE_TYPE_PHONE:
            return new int[] { R.drawable.ic_crossmount_phone,
                    R.drawable.ic_crossmount_phone_on };
        case IMultiCameraDeviceAdapter.REMOTE_DEVICE_TYPE_TV:
            return new int[] { R.drawable.ic_crossmount_tv,
                    R.drawable.ic_crossmount_tv_on };
        case IMultiCameraDeviceAdapter.REMOTE_DEVICE_TYPE_TABLET:
            return new int[] { R.drawable.ic_crossmount_tablet,
                    R.drawable.ic_crossmount_tablet_on };
        case IMultiCameraDeviceAdapter.REMOTE_DEVICE_TYPE_WATCH:
            return new int[] { R.drawable.ic_crossmount_watch,
                    R.drawable.ic_crossmount_watch_on };
         default:
             return new int[] { R.drawable.ic_crossmount_phone,
                     R.drawable.ic_crossmount_phone_on };
        }
    }

    private boolean isDeviceCameraConnected(int status) {
        Log.i(TAG, "isDeviceCameraConnected status " + status);
        if (status == IMultiCameraDeviceAdapter.REMOTE_CAEMRA_STATUS_CONNECTED) {
            return true;
        } else {
            return false;
        }
    }

    private boolean isDeviceCameraConnecting(int status) {
        Log.i(TAG, "isDeviceCameraConnecting status " + status);
        if (status == IMultiCameraDeviceAdapter.REMOTE_CAEMRA_STATUS_CONNECTING) {
            return true;
        } else {
            return false;
        }
    }

    private boolean isDeviceCameraAvailable(int status) {
        if (status == IMultiCameraDeviceAdapter.REMOTE_CAMERA_STATUS_AVALIABLE) {
            return true;
        } else {
            return false;
        }
    }

    private int getStatusIntValue(Object object) {
        int status = IMultiCameraDeviceAdapter.REMOTE_CAEMRA_STATUS_UNKNOWN;
        if (object != null && object instanceof Integer) {
            status = (Integer) object;
        }
        return status;
    }

    private int getTypeIntValue(Object object) {
        int type = IMultiCameraDeviceAdapter.REMOTE_DEVICE_TYPE_UNKNOWN;
        if (object != null && object instanceof Integer) {
            type = (Integer) object;
        }
        return type;
    }

    private int getSerialIntValue(Object object) {
        int serial = IMultiCameraDeviceAdapter.REMOTE_DEVICE_SERIAL_UNKNOWN;
        if (object != null && object instanceof Integer) {
            serial = (Integer) object;
        }
        return serial;
    }

    private int getCameraIdIntValue(Object object) {
        int cameraID = -1;
        if (object != null && object instanceof Integer) {
            cameraID = (Integer) object;
        }
        return cameraID;
    }

    /**
     *  A comparator for sort device list.
     */
    private class KeyValueComparator implements
            Comparator<Map.Entry<String, Object>> {
        @Override
        public int compare(Entry<String, Object> lhs, Entry<String, Object> rhs) {
            return ((String) lhs.getKey()).compareToIgnoreCase((String) rhs
                    .getKey());
        }
    }
}
