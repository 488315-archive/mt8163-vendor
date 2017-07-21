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
package com.android.camera.v2.bridge;

import android.app.Activity;
import android.content.Context;
import android.hardware.Camera.CameraInfo;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraManager;
import android.util.Log;

import com.android.camera.v2.bridge.device.RemoteDeviceImpl;

import com.mediatek.camera.v2.platform.device.IMultiCameraDeviceAdapter;
import com.mediatek.camera.v2.platform.device.IRemoteDevice;

import com.mediatek.crossmount.adapter.CrossMountAdapter;
import com.mediatek.crossmount.adapter.CrossMountAdapter.BuildInServiceType;
import com.mediatek.crossmount.adapter.CrossMountAdapter.FeatureState;
import com.mediatek.crossmount.adapter.Device;
import com.mediatek.crossmount.adapter.Service;
import com.mediatek.crossmount.adapter.Service.ServiceState;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map.Entry;

/**
 * The class is defined to encapsulates the function of cross mount framework.
 * Adapter will provider the interface to use cross mount framework API, and it also notify
 * device status change to other module.
 */
public class MultiCameraDeviceAdapterImpl implements IMultiCameraDeviceAdapter,
        CrossMountAdapter.CallBack {
    private static final String TAG = "MultiCameraDeviceAdapter";

    private Activity mActivity;
    private CrossMountAdapter mCrossMountAdapter;

    //This listener is used for notify current the device status
    private IDevicesStatusListener mStatusUpdateListener;
    private DeviceInfoManager mDeviceInfoManager;

    /**
     * Adapter constructor.
     * @param activity The activity the adapter is running in.
     */
    public MultiCameraDeviceAdapterImpl(Activity activity) {
        Log.i(TAG, "MultiCameraDeviceAdapter");
        mActivity = activity;
        mDeviceInfoManager = new DeviceInfoManager(activity);
    }

    @Override
    public void regisiterStatusUpdateListener(IDevicesStatusListener listener) {
        mStatusUpdateListener = listener;
    }

    @Override
    public void startScanDevice() {
        startScanDevices();
    }

    @Override
    public void stopScanDevice() {
        stopScanDevices();
    }

    @Override
    public void connectSelectedDevice(String deviceKey) {
        Log.i(TAG, "connectSelectedDevice deviceKey " + deviceKey);
        throw new RuntimeException("Function is not ready.");
    }

    @Override
    public void disconnectAllRemoteCamera() {
        Log.i(TAG, "onDisconnectButtonClicked");
        //Disconnect all linked camera service.
        List<IRemoteDevice> list = mDeviceInfoManager.getConnectedDeviceList();
        IRemoteDevice device = null;
        for (int i = 0; i < list.size(); i++) {
            device = list.get(i);
            device.unmountCamera();
        }
    }
    @Override
    public void release() {
        Log.i(TAG, "release");
    }

    @Override
    public void onFeatureStateUpdate(CrossMountAdapter.FeatureState featureState) {
        Log.i(TAG, "onFeatureStateUpdate featureState = " + featureState);
        if (mCrossMountAdapter == null) {
            return;
        }
        //check if the feature is enabled or not
        boolean enabled = FeatureState.ENABLED == featureState;
        if (enabled) {
            //here call startDiscover()
            mCrossMountAdapter.startDiscover(null);
            // after startDiscover, call the onDiscoverResultUpdate() and find
            // the result immediately.
            onDiscoverResultUpdate(0);
        }
    }

    @Override
    public void onDiscoverResultUpdate(int reason) {
        Log.i(TAG, "onDiscoverResultUpdate reason = " + reason);
        if (mCrossMountAdapter == null) {
            return;
        }
        mStatusUpdateListener.onAvalibleDevicesUpdate(
                mDeviceInfoManager.createDeviceInfo(mCrossMountAdapter
                .getAvailableDevices()));
    }

    @Override
    public void onConnectionResultUpdate() {
        Log.i(TAG, "onConnectionResultUpdate");
        if (mCrossMountAdapter == null) {
            return;
        }
        mStatusUpdateListener.onConnectedDeviesUpdate(
                mDeviceInfoManager.createDeviceInfo(mCrossMountAdapter
                .getAvailableDevices()));
    }

    @Override
    public void onTrustDeviceUpdated() {
        Log.i(TAG, "onTrustDeviceUpdated");
    }

    @Override
    public void onMyProviderServicesUpdate() {
        Log.i(TAG, "onMyProviderServicesUpdate");
    }

    @Override
    public void onPlugInServiceIconReady() {
        Log.i(TAG, "onPlugInServiceIconReady");
    }

    private boolean isCrossMountEnabled() {
        boolean enabled;
        FeatureState state = mCrossMountAdapter.getFeatureState();
        if (FeatureState.ENABLED == state) {
            enabled = true;
        } else {
            enabled = false;
        }
        Log.i(TAG, "isCrossMountEnabled enabled = " + enabled);
        return enabled;
    }

    /**
     * Start scan remote devices.
     */
    private void startScanDevices() {
        Log.i(TAG, "startScanDevices");

        //1. Get cross mount adapter and register listener.
        if (mCrossMountAdapter == null) {
            mCrossMountAdapter = CrossMountAdapter.getDefaultAdapter(mActivity, this);
        }
        //2 . judge if cross mount feature enabled
        if (isCrossMountEnabled()) {
            //if crossmount already enabled, scan remote devices directly.
             //startDiscover is not available now, show check it with cross mount framework.
            mCrossMountAdapter.startDiscover(null);
            // after startDiscover, call the onDiscoverResultUpdate() and find
            // the result immediately.
            onDiscoverResultUpdate(0);
        } else {
            //if crossmount feature not enabled yet. turn on the feature firstly.
            mCrossMountAdapter.enable();
            // after enabled, wait the onFeatureStateUpdate ASYNC call back,
            // check the state again and start discover
            //if feature enabled.
        }
    }

    private void stopScanDevices() {
        Log.i(TAG, "stopScanDevices");
        if (mCrossMountAdapter != null) {
            mCrossMountAdapter.stopDiscover();
            // Release cross mount adapter listener.
            mCrossMountAdapter.releaseCallBack(this);
            mCrossMountAdapter = null;
        }
    }

    /**
     * DeviceInfo is a abstract model for a remote phone device.
     * It present a remote phone device attribute.
     */
    private class DeviceInfo {
        private final String mKey;
        private final String mName;
        private final int mStatus;
        private final int mCameraID;
        private final int mSerialNumber;
        private final Service mCameraService;

        public DeviceInfo(String key, String name, int status, int cameraID,
                int serialNumber, Service service) {
            mKey = key;
            mName = name;
            mStatus = status;
            mCameraID = cameraID;
            mSerialNumber = serialNumber;
            mCameraService = service;
        }

        public String getName() {
            return mName;
        }

        public int getStatus() {
            return mStatus;
        }

        public int getCameraID() {
            return mCameraID;
        }

        public int getSerialNumber() {
            return mSerialNumber;
        }

        public Service getCameraService() {
            return mCameraService;
        }
    }

    /**
     * It is a manager for device info, when devices status change,
     * it will filter the interested info of the devices and use DeviceInfo class to store
     * the info if need.
     */
    private class DeviceInfoManager {

        private ArrayList<Integer> mCameraIDMappingList;
        private final ArrayList<Integer> mLocalCameraIDList;

        private LinkedHashMap<String, IRemoteDevice> mDevicesList;

        public DeviceInfoManager(Activity activity) {
            mCameraIDMappingList = new ArrayList<Integer>();
            mDevicesList = new LinkedHashMap<String, IRemoteDevice>();
            mLocalCameraIDList = getLocalCameraIDList(activity);
        }

        public LinkedHashMap<String, IRemoteDevice> createDeviceInfo(
                Device[] devices) {

            mDevicesList.clear();

            Service service = null;
            Log.i(TAG, "createDeviceInfo devices.length "
                    + devices.length);
            initCameraIDMappingTable(devices);
            // get supported services of each device.
            for (int i = 0; i < devices.length; i++) {
                if ((service = getCameraService(devices[i])) != null) {
                    String deviceID = devices[i].getId();
                    int cameraID = service.getCameraId();
                    int mappingValue = getCameraIDMappingvalue(cameraID);

                    IRemoteDevice device =
                            new RemoteDeviceImpl(devices[i],
                                                 getCameraService(devices[i]),
                                                 getCameraIDMappingvalue(cameraID));
                    device.dumpInfo();
                    mDevicesList.put(deviceID, device);
                }
            }
            return mDevicesList;
        }

        public List<IRemoteDevice> getConnectedDeviceList() {
            List<IRemoteDevice> list = new ArrayList<IRemoteDevice>();
            IRemoteDevice device = null;
            for (Entry<String, IRemoteDevice> entry : mDevicesList.entrySet()) {
                device = entry.getValue();
                int status = device.get(IRemoteDevice.KEY_REMOTE_SERVICE_STATUS);
                if (status == IRemoteDevice.REMOTE_CAEMRA_STATUS_CONNECTED) {
                    list.add(device);
                }
            }
            return list;
        }

        private ArrayList<Integer> getLocalCameraIDList(Activity activity) {
            CameraManager cameraManager = (CameraManager) activity
                    .getSystemService(Context.CAMERA_SERVICE);
            ArrayList<Integer> localCameraList = new ArrayList<Integer>();
            String[] idList = null;
            try {
                idList = cameraManager.getCameraIdList();
            } catch (CameraAccessException e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }

            List list = Arrays.asList(idList);
            if (list.contains(String.valueOf(CameraInfo.CAMERA_FACING_FRONT))) {
                localCameraList.add(CameraInfo.CAMERA_FACING_FRONT);
            }
            if (list.contains(String.valueOf(CameraInfo.CAMERA_FACING_BACK))) {
                localCameraList.add(CameraInfo.CAMERA_FACING_BACK);
            }
            Log.i(TAG, "getLocalCameraIDList localCameraList "
                    + localCameraList);
            return localCameraList;
        }

        private void initCameraIDMappingTable(Device[] devices) {
            //clear list
            mCameraIDMappingList.clear();
            //add local camera id
            mCameraIDMappingList.addAll(mLocalCameraIDList);

            Service service = null;
            for (int i = 0; i < devices.length; i++) {
                if ((service = getCameraService(devices[i])) != null) {
                    if (getCameraServiceState(service) == REMOTE_CAEMRA_STATUS_CONNECTED) {
                        int id = service.getCameraId();
                        Log.i(TAG, "[initCameraIDMappingTable] get a connected camera id = " + id);
                        mCameraIDMappingList.add(id);
                    }
                }
            }
            Collections.sort(mCameraIDMappingList);
            Log.i(TAG, "[initCameraIDMappingTable] mCameraIDMappingList = " + mCameraIDMappingList);
        }

        private int getCameraIDMappingvalue(int cameraID) {
            Log.i(TAG, "[getCameraIDMappingvalue] cameraID = " + cameraID);
            return mCameraIDMappingList.indexOf(cameraID);
        }

        private Service getCameraService(Device device) {
            Log.i(TAG, "getCameraService device = " + device);
            Service[] services = device.getProviderServices();
            String cameraServiceName = mCrossMountAdapter
                    .getBuildInServiceName(BuildInServiceType.CAMERA);
            // find if the device has the camera service or not.
            for (int i = 0; i < services.length; i++) {
                String serviceName = services[i].getName();
                Log.i(TAG, "getCameraService serviceName = " + serviceName);
                if (cameraServiceName.equals(serviceName)) {
                    return services[i];
                }
            }
            return null;
        }

        private int getCameraServiceState(Service service) {
            ServiceState state = service.getState(null);
            Log.i(TAG, "getServiceStateValue state " + state);
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
    }
}
