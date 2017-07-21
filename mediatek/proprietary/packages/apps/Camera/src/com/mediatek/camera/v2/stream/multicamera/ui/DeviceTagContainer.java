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
import android.hardware.Camera.CameraInfo;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;

import com.android.camera.R;

import com.mediatek.camera.v2.platform.device.IMultiCameraDeviceAdapter;
import com.mediatek.camera.v2.platform.device.IRemoteDevice;
import com.mediatek.camera.v2.util.Utils;

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map.Entry;

/**
 * Device tag container is the parent view of the tag.
 */
public class DeviceTagContainer implements IMultiCameraView,
        DeviceTagView.IDeviceTagViewListener {
    /**
     * Device tag listener.
     */
    public interface IDeviceTagListener {
        /**
         * When a device has been selected in the device tag list, notify the event
         * to the module, open the selected camera device.
         * @param cameraId The selected camera id.
         * @param deviceId The selected device identity key.
         */
        public void onCameraSelected(String cameraId, String deviceId);
    }

    private static final String TAG = "DeviceTagContainer";
    private static final int TAG_NUM = 3;
    private static final int TAG_MARGIN = 12;
    private static final float ROUND_RATIO = 0.5f;

    private Activity mActivity;
    private ViewGroup mParentView;
    private ViewGroup mTagContrainer;
    private ViewGroup mMultiTagContainer;
    private ViewGroup[][] mTags;
    private ViewGroup mLocalTag;

    private ViewGroup mSingleTagContainer;

    private LinkedHashMap<String, String> mConnectedList;
    private LinkedHashMap<String, String> mCandidateList;
    private LinkedHashMap<String, String> mPreviewingList;
    private List<String> mPreviewingCameraId;

    private LinkedHashMap<String, IRemoteDevice> mDevicesList;

    private int mDisplayRotation;
    private int mOrientationCompensation;
    private boolean mRemoteCameraOpened;

    private IDeviceTagListener mDeviceListViewListener;

    /**
     * The DeviceTagContainer constructor.
     * @param deviceTagViewListener The device tag view listener.
     */
    public DeviceTagContainer(IDeviceTagListener deviceTagViewListener) {
        mDeviceListViewListener = deviceTagViewListener;
        mConnectedList = new LinkedHashMap<String, String>();
        mPreviewingList = new LinkedHashMap<String, String>();
        mPreviewingCameraId = new ArrayList<String>();
        mTags = new ViewGroup[TAG_NUM][2];
    }

    @Override
    public void open(Activity activity, ViewGroup parentView) {
        mActivity = activity;
        mParentView = parentView;
        mDisplayRotation = Utils.getDisplayRotation(mActivity);
        getView();
        hide();
    }

    @Override
    public void resume() {
    }

    @Override
    public void pause() {
    }

    @Override
    public void close() {
    }

    @Override
    public void onDeviceCameraSelected(String deviceKey) {
        Log.i(TAG, "onItemClick deviceKey " + deviceKey);
        if (mDeviceListViewListener != null) {
            IRemoteDevice device = mDevicesList.get(deviceKey);
            int cameraID = device.get(IRemoteDevice.KEY_REMOTE_CAMERA_ID);
            mDeviceListViewListener.onCameraSelected(String.valueOf(cameraID), deviceKey);
        }
    }

    @Override
    public void onOrientationChanged(int gsensorOrientation) {
        Log.i(TAG, "onOrientationChanged gsensorOrientation = "
                + gsensorOrientation + " mDisplayRotation " + mDisplayRotation);

        if (mTags != null) {
            DeviceTagView tag = null;
            for (int i = 0; i < mTags.length; i++) {
                tag = (DeviceTagView) mTags[i][1];
                if (tag != null) {
                    tag.onOrientationChanged(gsensorOrientation);
                }
            }
        }
        if (mSingleTagContainer != null) {
            DeviceTagView tag = (DeviceTagView) mSingleTagContainer.getTag();
            tag.onOrientationChanged(gsensorOrientation);
        }
    }
    @Override
    public void show() {
        mTagContrainer.setVisibility(View.VISIBLE);
    }
    @Override
    public void hide() {
        mTagContrainer.setVisibility(View.GONE);
    }

    @Override
    public boolean onSingleTapUp(float x, float y) {
        Log.i(TAG, "onSingleTapUp  [ " + x + ", " + y + "]");
        return false;
    }
    @Override
    public void onDisplayChanged(int displayRotation) {
        Log.i(TAG, "onDisplayChanged = " + displayRotation);
        mDisplayRotation = displayRotation;

        if (mTags != null) {
            DeviceTagView tag = null;
            for (int i = 0; i < mTags.length; i++) {
                tag = (DeviceTagView) mTags[i][1];
                if (tag != null) {
                    tag.onDisplayChanged(displayRotation);
                }
            }
        }
        if (mSingleTagContainer != null) {
            DeviceTagView tag = (DeviceTagView) mSingleTagContainer.getTag();
            tag.onDisplayChanged(displayRotation);
        }
    }
    @Override
    public void onLayoutOrientationChanged(boolean isLandscape) {
        Log.i(TAG, "onLayoutOrientationChanged  isLandscape" + isLandscape);

        removeView();
        getView();
        for (int i = 0; i < mTags.length; i++) {
            ViewGroup tagView = mTags[i][1];
            if (mTags[i][1] != null) {

                mTags[i][0].addView(tagView);
                LinearLayout.LayoutParams params = (LinearLayout.LayoutParams) ((ViewGroup) tagView)
                        .getLayoutParams();
                if (!isLandscape) {
                    params.rightMargin = dp2Px(mActivity, TAG_MARGIN);
                    params.bottomMargin = dp2Px(mActivity, TAG_MARGIN);
                } else {
                    params.rightMargin = dp2Px(mActivity, TAG_MARGIN);
                    params.topMargin = dp2Px(mActivity, TAG_MARGIN);
                }
                tagView.setLayoutParams(params);
            }
        }

        updatePreviewCamera(mPreviewingCameraId, mRemoteCameraOpened);
    }

    @Override
    public void updatePreviewCamera(List<String> cameraId,
            boolean isRemoteOpened) {
        Log.i(TAG, "[updatePreviewCamera] list size = " + cameraId.size()
                + ",RemoteOpened = " + isRemoteOpened);
        if (cameraId.size() == 0) {
            return;
        }
        mPreviewingCameraId = cameraId;
        mRemoteCameraOpened = isRemoteOpened;
        // If remote camera opened, judge which type tag should show.
        if (mRemoteCameraOpened) {
            for (int i = 0; i < mPreviewingCameraId.size(); i++) {
                Log.i(TAG,
                        "updatePreviewingCameras id "
                                + mPreviewingCameraId.get(i));
            }
            // if current preview camera number is equal to 1 and
            // current preview camera is not local one.
            if (mPreviewingCameraId.size() == 1) {
                hideMultiTagView();
                if (!mPreviewingCameraId.get(0).equals(
                        String.valueOf(CameraInfo.CAMERA_FACING_BACK))
                        && !mPreviewingCameraId.get(0).equals(
                                String.valueOf(CameraInfo.CAMERA_FACING_FRONT))) {
                    initSingleTagView(mPreviewingCameraId.get(0));
                    mSingleTagContainer.setVisibility(View.VISIBLE);
                } else {
                    hideSingleTagView();
                }
            } else {
                // If current preview camera number is bigger than
                // 1(include local camera), the state is multi preview.
                // should show multi preview tag.
                hideSingleTagView();
                initPreviewingCameraList();
                initMultiTagView(mPreviewingList);
                if (mPreviewingCameraId.contains(String
                        .valueOf(CameraInfo.CAMERA_FACING_BACK))) {
                    mLocalTag.setTag(String
                            .valueOf(CameraInfo.CAMERA_FACING_BACK));
                } else if (mPreviewingCameraId.contains(String
                        .valueOf(CameraInfo.CAMERA_FACING_FRONT))) {
                    mLocalTag.setTag(String
                            .valueOf(CameraInfo.CAMERA_FACING_FRONT));
                }
                mMultiTagContainer.setVisibility(View.VISIBLE);
            }
            initCandidateCameraList();
        } else {
            hideMultiTagView();
            hideSingleTagView();
        }
    }

    @Override
    public void updateCapturingStatus(boolean isCapturing) {
        // TODO Auto-generated method stub

    }

    @Override
    public void onAvalibleDevicesUpdate(
            LinkedHashMap<String, IRemoteDevice> devices) {
        mDevicesList = devices;

        mActivity.runOnUiThread(new Runnable() {
            public void run() {
                initConnectedCameraList();
                initCandidateCameraList();
            }
        });
    }

    @Override
    public void onConnectedDeviesUpdate(
            LinkedHashMap<String, IRemoteDevice> devices) {
        mDevicesList = devices;
        mActivity.runOnUiThread(new Runnable() {
            public void run() {
                initConnectedCameraList();
                initCandidateCameraList();
            }
        });
    }

    private void hideSingleTagView() {
        mSingleTagContainer.setVisibility(View.INVISIBLE);
        DeviceTagView tag = (DeviceTagView) mSingleTagContainer
                .getTag();
        tag.hideListView();
    }

    private void hideMultiTagView() {
        mMultiTagContainer.setVisibility(View.INVISIBLE);
        if (mTags != null) {
            DeviceTagView tag = null;
            for (int i = 0; i < mTags.length; i++) {
                tag = (DeviceTagView) mTags[i][1];
                if (tag != null) {
                    tag.hideListView();
                }
            }
        }
    }

    private DeviceTagView getTagView(DeviceTagView.DeviceTagType type, String name) {
        DeviceTagView tag = (DeviceTagView) mActivity.getLayoutInflater()
                .inflate(R.layout.multi_camera_device_list, null, false);
        tag.setDeviceName(name);
        tag.setTagType(type);
        tag.setListener(this);
        tag.onOrientationChanged(mOrientationCompensation);
        tag.onDisplayChanged(mDisplayRotation);
        return tag;
    }

    private void removeView() {
        if (mTagContrainer != null) {
            if (mTags != null) {
                for (int i = 0; i < mTags.length; i++) {
                    mTags[i][0].removeAllViews();
                    mTags[i][0] = null;
                }
            }
            mTagContrainer.removeAllViews();
            mParentView.removeView(mTagContrainer);
            mTagContrainer = null;
        }
    }

    private void getView() {
        mActivity.getLayoutInflater().inflate(
                R.layout.multi_camera_device_tag_container, mParentView, true);
        mTagContrainer = (ViewGroup) mParentView
                .findViewById(R.id.tag_container);
        mMultiTagContainer = (ViewGroup) mTagContrainer
                .findViewById(R.id.multi_preview_tag_container);
        mTags[0][0] = (ViewGroup) mMultiTagContainer.findViewById(R.id.tag1);
        mTags[1][0] = (ViewGroup) mMultiTagContainer.findViewById(R.id.tag2);

        mTags[2][0] = (ViewGroup) mMultiTagContainer.findViewById(R.id.tag3);
        mLocalTag = (ViewGroup) mMultiTagContainer.findViewById(R.id.tag4);
        mLocalTag.setVisibility(View.VISIBLE);
        mSingleTagContainer = (ViewGroup) mTagContrainer
                .findViewById(R.id.singe_preview_tag_container);
        DeviceTagView tag = getTagView(DeviceTagView.DeviceTagType.SINGLE,
                null);
        mSingleTagContainer.addView(tag);
        mSingleTagContainer.setTag(tag);
    }

    private void reInflate() {
        removeView();
    }

    private int dp2Px(Context context, float dp) {
        final float scale = context.getResources().getDisplayMetrics().density;
        return (int) (dp * scale + ROUND_RATIO);
    }

    private void initPreviewingCameraList() {
        Log.i(TAG, "initPreviewingCameraList");
        if (mPreviewingList != null && mDevicesList != null) {
            mPreviewingList.clear();
            for (Entry<String, IRemoteDevice> entry : mDevicesList.entrySet()) {
                String key = entry.getKey();
                IRemoteDevice device = entry.getValue();
                int cameraId = device.get(IRemoteDevice.KEY_REMOTE_CAMERA_ID);
                // if camera id in the previewing list.
                if (mPreviewingCameraId.contains(String.valueOf(cameraId))) {
                    String name = device.get(IRemoteDevice.KEY_REMOTE_DEVICE_NAME);
                    Log.i(TAG, "initPreviewingCameraList ID found! name: "
                            + name);
                    mPreviewingList.put(key, name);
                }
            }
        }
    }

    private void initConnectedCameraList() {
        Log.i(TAG, "initConnectedDeviceList");
        if (mDevicesList != null) {
            mConnectedList.clear();
            for (Entry<String, IRemoteDevice> entry : mDevicesList.entrySet()) {
                String key = entry.getKey();
                IRemoteDevice device = entry.getValue();
                int status = device
                        .get(IRemoteDevice.KEY_REMOTE_SERVICE_STATUS);
                if (status == IMultiCameraDeviceAdapter.REMOTE_CAEMRA_STATUS_CONNECTED) {
                    String name = device
                            .get(IRemoteDevice.KEY_REMOTE_DEVICE_NAME);
                    mConnectedList.put(key, name);
                    Log.i(TAG, "find a connected device name = " + name);
                }
            }
        }
    }

    private void initCandidateCameraList() {
        Log.i(TAG, "initCandidateCameraList");
        // 1. Init previewing camera list.
        // 2. Remove previewing cameras in connected cameras.
        mCandidateList = (LinkedHashMap<String, String>) mConnectedList.clone();

        for (Entry<String, String> entry : mPreviewingList.entrySet()) {
            String key = entry.getKey();
            String name = entry.getValue();
            Log.i(TAG, "mPreviewingList key " + key + " name " + name);
            mCandidateList.remove(key);
        }
        // 3. The result is candidate cameras.
        for (Entry<String, String> entry : mCandidateList.entrySet()) {
            String key = entry.getKey();
            String name = entry.getValue();
            Log.i(TAG, "mCandidateList key " + key + " name " + name);
        }
        if (mTags != null) {
            DeviceTagView tag = null;
            for (int i = 0; i < mTags.length; i++) {
                tag = (DeviceTagView) mTags[i][1];
                if (tag != null) {
                    tag.setData(mCandidateList);
                }
            }
        }
    }

    private void initSingleTagView(String cameraID) {
        for (Entry<String, IRemoteDevice> entry : mDevicesList.entrySet()) {
            IRemoteDevice device = entry.getValue();
            int id = device.get(IRemoteDevice.KEY_REMOTE_CAMERA_ID);
            if (cameraID.equals(String.valueOf(id))) {
                String name = device.get(IRemoteDevice.KEY_REMOTE_DEVICE_NAME);
                Log.i(TAG, "ID found! name: " + name);
                DeviceTagView tag = (DeviceTagView) mSingleTagContainer
                        .getTag();
                tag.setDeviceName(name);
            }
        }
    }

    private void initMultiTagView(LinkedHashMap<String, String> previewingList) {
        int num = 0;
        for (Entry<String, String> entry : previewingList.entrySet()) {
            String key = entry.getKey();
            String name = entry.getValue();
            Log.i(TAG, "updatePreviewingCameras runnable name: " + name);
            // 1. if tag container do not has tag view ,create
            // one and add it.
            if (mTags[num][1] == null) {
                mTags[num][1] = (DeviceTagView) getTagView(
                        DeviceTagView.DeviceTagType.MULTI, name);
                mTags[num][0].addView((ViewGroup) mTags[num][1]);
                LinearLayout.LayoutParams params =
                        (LinearLayout.LayoutParams) ((ViewGroup) mTags[num][1])
                        .getLayoutParams();
                if (mDisplayRotation == 0) {
                    params.rightMargin = dp2Px(mActivity, TAG_MARGIN);
                    params.bottomMargin = dp2Px(mActivity, TAG_MARGIN);
                } else {
                    params.rightMargin = dp2Px(mActivity, TAG_MARGIN);
                    params.topMargin = dp2Px(mActivity, TAG_MARGIN);
                }
                ((ViewGroup) mTags[num][1]).setLayoutParams(params);
            } else {
                ((DeviceTagView) mTags[num][1]).setDeviceName(name);
            }
            // record the current camera id.
            IRemoteDevice device = mDevicesList.get(key);
            int id = device.get(IRemoteDevice.KEY_REMOTE_CAMERA_ID);
            mTags[num][0].setTag(String.valueOf(id));
            mTags[num][0].setVisibility(View.VISIBLE);
            num ++;
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

    private int getCameraIdIntValue(Object object) {
        int cameraID = -1;
        if (object != null && object instanceof Integer) {
            cameraID = (Integer) object;
        }
        return cameraID;
    }
}
