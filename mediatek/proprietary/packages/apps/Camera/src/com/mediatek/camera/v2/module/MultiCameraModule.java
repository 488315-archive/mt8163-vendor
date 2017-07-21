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
package com.mediatek.camera.v2.module;

import android.app.Activity;
import android.graphics.Rect;
import android.graphics.RectF;
import android.hardware.Camera.CameraInfo;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCaptureSession.CaptureCallback;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureRequest.Builder;
import android.hardware.camera2.CaptureResult;
import android.hardware.camera2.TotalCaptureResult;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.view.OrientationEventListener;
import android.view.Surface;

import com.android.camera.R;

import com.mediatek.camera.v2.control.ControlImpl;
import com.mediatek.camera.v2.control.IControl.IAaaController;
import com.mediatek.camera.v2.detection.DetectionManager;
import com.mediatek.camera.v2.module.ModuleListener.RequestType;
import com.mediatek.camera.v2.platform.app.AppController;
import com.mediatek.camera.v2.platform.device.CameraDeviceManager;
import com.mediatek.camera.v2.platform.device.CameraDeviceManager.CameraStateCallback;
import com.mediatek.camera.v2.platform.device.CameraDeviceProxy;
import com.mediatek.camera.v2.platform.device.CameraDeviceProxy.CameraSessionCallback;
import com.mediatek.camera.v2.platform.device.IMultiCameraDeviceAdapter;
import com.mediatek.camera.v2.platform.device.IMultiCameraDeviceAdapter.IDevicesStatusListener;
import com.mediatek.camera.v2.platform.device.IRemoteDevice;
import com.mediatek.camera.v2.setting.SettingCtrl;
import com.mediatek.camera.v2.stream.multicamera.ui.IMultiCameraViewManager.IMultiCameraViewListener;
import com.mediatek.camera.v2.stream.multicamera.ui.IMultiCameraViewManager.IStatusUpdateListener;
import com.mediatek.camera.v2.util.SettingKeys;
import com.mediatek.camera.v2.util.Utils;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

/**
 * This class control open and close camera,and configure session and request.
 */
public class MultiCameraModule extends AbstractCameraModule {

    private static final String TAG = MultiCameraModule.class.getSimpleName();
    private static final boolean ONLY_SUPPORTE_ONE_REMOTE_CAMERA = true;
    private static final int ONLY_ONE_PREVIEW_CAMERA = 1;
    private static final String MULTI_PREVIEW_ON = "on";
    private static final String MULTI_PREVIEW_OFF = "off";


    private int mGSensorOrientation = OrientationEventListener.ORIENTATION_UNKNOWN;
    // The activity is going to switch to the specified camera id. This is
    // needed because texture copy is done in GL thread. -1 means camera is not
    // switching.
    private int mPendingSwitchCameraId = UNKNOWN;

    // when in single remote preview and multi-preview, not show the setting
    // icon and picker manager UI
    private boolean mIsCanShowCommonUi = true;
    private boolean mIsLandScape = false;
    private List<String> mHasOpenedCameraId = new ArrayList<String>();
    private List<String> mPreviewCameraId = new ArrayList<String>();
    private List<String> mOpenedRemoteCameraId = new ArrayList<String>();

    private IStatusUpdateListener mScanResultToViewListener;

    private CameraDeviceManager mCameraManager;
    private CameraDeviceProxy mLocalCameraDeviceProxy;
    private ArrayList<CameraDeviceProxy> mRemoteCameraDeviceProxy =
            new ArrayList<CameraDeviceProxy>();
    // key-value is cameraId-deviceId
    private LinkedHashMap<String, String> mOpenedRemoteDeviceId =
            new LinkedHashMap<String, String>();
    private LinkedHashMap<String, CameraDeviceProxy> mRemoteCameraProxy =
            new LinkedHashMap<String, CameraDeviceProxy>();

    // this handler used for deal with the open status callback
    private HandlerThread mCameraHandlerThread;
    private Handler mCameraHandler;

    // for open camera
    private HandlerThread mOpenCamThread;
    private Handler mOpenCameraHandler;

    private IMultiCameraDeviceAdapter mIMultiCameraDeviceAdapter;

    private String mOpeningRemoteDeviceId;

    private RequestType mCurrentRequestType = RequestType.PREVIEW;
    private RectF mPreviewArea;
    /**
     * Instantiates a camera module for Multi camera.
     * @param app
     *            A controller which is App level.can notify lock orientation
     *            and get the UI.
     */
    public MultiCameraModule(
            AppController app) {
        super(app);
        mDetectionManager = new DetectionManager(app, this, null);
        mAaaControl = new ControlImpl(app, this, true, null);
    }

    @Override
    public void open(Activity activity, boolean isSecureCamera, boolean isCaptureIntent) {
        super.open(activity, isSecureCamera, isCaptureIntent);
        mIsLandScape = MultiCameraModuleUtil.isLandScape(activity);
        getCameraStaticInfo(activity);

        // initialize module UI
        mAbstractModuleUI = new MultiCameraModuleUi(activity, this, mAppUi.getModuleLayoutRoot(),
                mStreamManager.getPreviewCallback(), new MultiCameraViewListenerImpl());

        // set the preview area changed listener
        mAppController.setModuleUiListener(mAbstractModuleUI);
        mAppController.addPreviewAreaSizeChangedListener(this);

        mIMultiCameraDeviceAdapter = mAppController.getMultiCameraDeviceAdapter();
        mIMultiCameraDeviceAdapter.regisiterStatusUpdateListener(new DevicesStatusListener());

        // Notify Module UI current Module is opened
        mAbstractModuleUI.open(activity, isSecureCamera, isCaptureIntent);
    }

    @Override
    public void resume() {
        Log.i(TAG, "[resume]+");

        super.resume();
        mCameraHandlerThread = new HandlerThread("Cross Mount Skin Camera");
        mCameraHandlerThread.start();
        mCameraHandler = new Handler(mCameraHandlerThread.getLooper());

        mOpenCamThread = new HandlerThread("tmpThread_openCam");
        mOpenCamThread.start();
        mOpenCameraHandler = new Handler(mOpenCamThread.getLooper());

        openCamera(mCameraId);
        mAbstractModuleUI.resume();
        Log.i(TAG, "[resume]-");
    }

    @Override
    public void pause() {
        Log.i(TAG, "[pause]+");
        super.pause();
        closeCamera();
        mAbstractModuleUI.pause();
        Log.i(TAG, "[pause]-");
    }

    @Override
    public void close() {
        Log.i(TAG, "[close]+");
        super.close();
        mAppController.removePreviewAreaSizeChangedListener(this);
        mAbstractModuleUI.close();
        updateMultiPreviewSetting(null);
        Log.i(TAG, "[close]-");
    }

    @Override
    public void onPreviewAreaChanged(RectF previewArea) {
        super.onPreviewAreaChanged(previewArea);
        mPreviewArea = previewArea;
        mAbstractModuleUI.onPreviewAreaChanged(previewArea);
    }

    @Override
    public void onSettingChanged(Map<String, String> result) {
        super.onSettingChanged(result);
        boolean cameraIdChanged = result != null && result.containsKey(SettingKeys.KEY_CAMERA_ID);
        if (cameraIdChanged) {
            String cameraId = result.get(SettingKeys.KEY_CAMERA_ID);
            if (cameraId != mCameraId) {
                mCameraId = cameraId;
                requestChangeCaptureRequets(false/* sync */, getRequestType(),
                        CaptureType.REPEATING_REQUEST);
                Log.i(TAG, "onSettingChanged cameraId:" + mCameraId);
            }
        }
    }

    @Override
    public void onCameraPicked(String newCameraId) {
        Log.i(TAG, "onCameraPicked newCameraId: " + newCameraId);
        if (mPaused) {
            Log.i(TAG, "[onCameraPicked]- mPaused = " + mPaused);
            return;
        }
        super.onCameraPicked(newCameraId);
        mPendingSwitchCameraId = Integer.valueOf(newCameraId);
        switchCamera();
        mCurrentMode.switchCamera(newCameraId);
    }

    @Override
    public void onOrientationChanged(int orientation) {
        // We keep the last known orientation. So if the user first orient
        // the camera then point the camera to floor or sky, we still have
        // the correct orientation.
        if (orientation == OrientationEventListener.ORIENTATION_UNKNOWN || mPaused) {
            return;
        }
        int newOrientation = Utils.roundOrientation(orientation, mGSensorOrientation);
        if (mGSensorOrientation != newOrientation) {
            mGSensorOrientation = newOrientation;
        }
        mAbstractModuleUI.onOrientationChanged(mGSensorOrientation);
        mAaaControl.onOrientationChanged(mGSensorOrientation);
        mDetectionManager.onOrientationChanged(mGSensorOrientation);
        mCurrentMode.onOrientationChanged(mGSensorOrientation);
    }

    @Override
    public boolean onSingleTapUp(float x, float y) {
        Log.d(TAG, "[onSingleTapUp] x = " + x + ",y = " + y + ",mPreviewArea = " + mPreviewArea
                + ",mIsLandScape = " + mIsLandScape);
        if (mPaused || mLocalCameraDeviceProxy == null) {
            Log.d(TAG, "[onSingleTapUp]- mPaused : " + mPaused);
            return false;
        }

        if (ONLY_ONE_PREVIEW_CAMERA == mPreviewCameraId.size()
                && MultiCameraModuleUtil.hasRemoteCamera(mPreviewCameraId)) {
            Log.d(TAG, "[onSingleTapUp] current is in remote camera preview,not support AF");
            return true;
        }

        if (!super.onSingleTapUp(x, y)) {
            String clickedCamera = calculateClickedCameraId(x, y);
            previewAreaClicked(clickedCamera);
            return true;
        }

        mAaaControl.onSingleTapUp(x, y);
        mDetectionManager.onSingleTapUp(x, y);
        Log.i(TAG, "[onSingleTapUp]-");
        return false;
    }

    @Override
    public boolean onLongPress(float x, float y) {
        Log.i(TAG, "onLongPress x = " + x + " y = " + y);
        if (super.onLongPress(x, y)) {
            return true;
        }
        mDetectionManager.onLongPressed(x, y);
        // return true means this event is finished,not need transfer to others.
        return false;
    }

    @Override
    public void requestChangeCaptureRequets(boolean sync, RequestType requestType,
            CaptureType captureType) {
        Log.i(TAG, "[requestChangeCaptureRequets] ,mPreviewCameraId: " + mPreviewCameraId);
        super.requestChangeCaptureRequets(sync, requestType, captureType);

        if (MultiCameraModuleUtil.hasLocalCamera(mPreviewCameraId)) {
            requestChangeCaptureRequets(true, sync, requestType, captureType);
        }

        if (MultiCameraModuleUtil.hasRemoteCamera(mPreviewCameraId)) {
            requestChangeCaptureRequets(false, sync, requestType, captureType);
        }
    }

    @Override
    public void requestChangeCaptureRequets(boolean isMainCamera, boolean sync,
            RequestType requestType, CaptureType captureType) {
        Log.i(TAG, "[requestChangeCaptureRequets]requestType = " + requestType + ",isMainCamera = "
                + isMainCamera);
        if (RequestType.RECORDING == requestType || RequestType.PREVIEW == requestType) {
            mCurrentRequestType = requestType;
        }
        if (isMainCamera) {
            if (mLocalCameraDeviceProxy == null) {
                Log.i(TAG, "requestChangeCaptureRequets but local camera is null!");
                return;
            }
            mLocalCameraDeviceProxy.requestChangeCaptureRequets(sync, requestType, captureType);
        } else {

            if (mRemoteCameraDeviceProxy == null) {
                Log.i(TAG, "requestChangeCaptureRequets but remote camera is null!");
                return;
            }

            // TODO this for need configure ....
            for (CameraDeviceProxy device : mRemoteCameraDeviceProxy) {
                device.requestChangeCaptureRequets(sync, requestType, captureType);
            }
        }
    }

    @Override
    public void requestChangeSessionOutputs(boolean sync) {
        Log.i(TAG, "[requestChangeSessionOutputs] sync = " + sync + ",mpreviewCamera = "
                + mPreviewCameraId);
        if (MultiCameraModuleUtil.hasLocalCamera(mPreviewCameraId)) {
            requestChangeSessionOutputs(sync, true);
        }
        if (MultiCameraModuleUtil.hasRemoteCamera(mPreviewCameraId)) {
            requestChangeSessionOutputs(sync, false);
        }
    }

    @Override
    public void requestChangeSessionOutputs(boolean sync, boolean isMainCamera) {
        Log.i(TAG, "requestChangeSessionOutputs,isMainCamera = " + isMainCamera);
        if (isMainCamera) {
            if (mLocalCameraDeviceProxy == null) {
                Log.i(TAG, "requestChangeSessionOutputs but local camera is null!");
                return;
            }
            mLocalCameraDeviceProxy.requestChangeSessionOutputs(sync);
        } else {
            if (mRemoteCameraDeviceProxy == null) {
                Log.i(TAG, "requestChangeSessionOutputs but remote camera is null!");
                return;
            }
            for (CameraDeviceProxy device : mRemoteCameraDeviceProxy) {
                device.requestChangeSessionOutputs(sync);
            }
        }
    }

    @Override
    public IAaaController get3AController(String cameraId) {
        // TODO
        if (SettingCtrl.BACK_CAMERA.equals(cameraId)) {
            return mAaaControl;
        } else {
            return mAaaControl;
        }
    }

    @Override
    public RequestType getRepeatingRequestType() {
        // FIXME how to get repeating request type?
        // multi-camera module may come across some problem without deal with it.
        return RequestType.PREVIEW;
    }

    @Override
    public void requestUpdateRecordingStatus(boolean isRecordingStated) {
        super.requestUpdateRecordingStatus(isRecordingStated);
        mAbstractModuleUI.updateRecordingStatus(isRecordingStated);
    }

    @Override
    public void requestUpdateCaptureStatus(boolean isCapturing) {
        Log.i(TAG, "[requestUpdateCaptureStatus] isCapturing = " + isCapturing);
        super.requestUpdateCaptureStatus(isCapturing);
        mAbstractModuleUI.updateCaptureStatus(isCapturing);
    }

    @Override
    public void onLayoutOrientationChanged(boolean isLandscape) {
        Log.i(TAG, "[onLayoutOrientationChanged] isLandScape = " + isLandscape);
        mIsLandScape = isLandscape;
        mAbstractModuleUI.onLayoutOrientationChanged(isLandscape);
    }

    private void openCamera(final String cameraId) {
        Log.i(TAG, "[openCamera],cameraId = " + cameraId);

        mOpenCameraHandler.post(new Runnable() {
            @Override
            public void run() {
                if (mCameraManager == null) {
                    throw new IllegalStateException("openCamera, but CameraManager is null!");
                }
                CameraDeviceCallback callback = new CameraDeviceCallback();
                mCameraManager.openSync(cameraId, callback, callback, mCameraHandler);

                Log.i(TAG, "[openCamera]------cameraId = " + cameraId);
            } // end of run
        });
    }

    private void closeCamera() {
        Log.i(TAG, "[closeCamera]+");
        doCloseRemoteCameras();
        closeLocalCamera();
        Log.i(TAG, "[closeCamera]-");
    }

    private void closeLocalCamera() {
        Log.i(TAG, "closeLocalCamera +++");
        if (mLocalCameraDeviceProxy != null) {
            mLocalCameraDeviceProxy.close();
            mLocalCameraDeviceProxy = null;
        }
        Log.i(TAG, "closeLocalCamera---");

    }

    private RequestType getRequestType() {
        RequestType requestType = RequestType.PREVIEW;
        if (mCurrentRequestType == RequestType.RECORDING) {
            requestType = RequestType.RECORDING;
        }
        Log.i(TAG, "[getRequestType] requestType = " + requestType);
        return requestType;
    }

    private void getCameraStaticInfo(Activity activity) {
        mCameraManager = mAppController.getCameraManager();
        mCameraId = mSettingController.getCurrentCameraId();
        Log.i(TAG, "[getCameraStaticInfo] mCameraId = " + mCameraId);
    }

    private void updateCameraId(boolean isAdd, String cameraId, String deviceId) {

        // update remote camera preview info
        if (!String.valueOf(CameraInfo.CAMERA_FACING_BACK).equals(cameraId)
                && !String.valueOf(CameraInfo.CAMERA_FACING_FRONT).equals(cameraId)) {
            if (isAdd) {
                if (!mOpenedRemoteCameraId.contains(cameraId)) {
                    mOpenedRemoteCameraId.add(cameraId);
                }
                mOpenedRemoteDeviceId.put(cameraId, deviceId);
            } else {
                mOpenedRemoteCameraId.remove(cameraId);
                mOpenedRemoteDeviceId.remove(cameraId);
            }
        }

        // update current preview and has opened camera info
        if (isAdd) {
            if (!mPreviewCameraId.contains(cameraId)) {
                mPreviewCameraId.add(cameraId);
            }
            if (!mHasOpenedCameraId.contains(cameraId)) {
                mHasOpenedCameraId.add(cameraId);
            }
        } else {
            mPreviewCameraId.remove(cameraId);
            mHasOpenedCameraId.remove(cameraId);
        }

        Log.i(TAG, "[updateCameraId] isAdd : " + isAdd + ",cameraId = " + cameraId
                + ",after change,mOpenedRemoteCameraId: " + mOpenedRemoteCameraId
                + ",mPreviewCameraId = " + mPreviewCameraId + ",mHasOpenedCameraId = "
                + mHasOpenedCameraId);
    }

    private void notifyCameraIdChanged(List<String> previewCameraId, boolean isRemoteCameraOpened) {
        Log.i(TAG, "[notifyCameraIdChanged] previewCameraId = " + previewCameraId
                + ",isRemoteCameraOpened = " + isRemoteCameraOpened + ",mHasOpenedCameraId = "
                + mHasOpenedCameraId);

        List<String> previewIdCopy = new ArrayList<String>();
        previewIdCopy.addAll(previewCameraId);

        List<String> openedIdCopy = new ArrayList<String>();
        openedIdCopy.addAll(mHasOpenedCameraId);
        updateMultiPreviewSetting(previewIdCopy);
        mCurrentMode.updatePreviewCamera(previewIdCopy, openedIdCopy);
        mAbstractModuleUI.updatePreviewCamera(previewIdCopy, isRemoteCameraOpened);

        // hide the settings icon and picker manager icons when remote camera is
        // previewing
        final List<String> previewId = new ArrayList<String>();
        previewId.addAll(previewCameraId);
        updateUiVisibility(previewId);
    }

    private boolean isRemoteCameraOpened() {
        boolean hasOpened = false;
        hasOpened = mOpenedRemoteCameraId.size() > 0;
        Log.i(TAG, "[isRemoteCameraOpened] hasOpened = " + hasOpened);
        return hasOpened;
    }

    private void doCloseRemoteCameras() {
        boolean willUpdatePreviewCamera = false;
        if (!mOpenedRemoteCameraId.isEmpty()) {
            for (int i = 0; i < mRemoteCameraDeviceProxy.size(); i++) {
                CameraDeviceProxy deviceProxy = mRemoteCameraDeviceProxy.get(i);
                String cameraId = deviceProxy.getCameraId();
                Log.i(TAG, "[_doCloseRemoteCameras] cameraId = " + cameraId);
                if (mOpenedRemoteCameraId.contains(cameraId)) {
                    deviceProxy.close();
                    mRemoteCameraDeviceProxy.remove(deviceProxy);
                    String deviceId = mOpenedRemoteDeviceId.get(cameraId);
                    mRemoteCameraProxy.remove(deviceId);
                    updateCameraId(false /* is Add Camera ID */, cameraId, null);
                    willUpdatePreviewCamera = true;
                }
            }
        }

        Log.i(TAG, "[doCloseRemoteCameras], willUpdatePreviewCamera = " + willUpdatePreviewCamera
                + ",mPaused = " + mPaused);

        // notify back to local single preview
        ArrayList<String> localPreviewId = new ArrayList<String>();
        localPreviewId.add(mCameraId);
        // when remote camera has closed,need update the preview camera id
        updateCameraId(true, mCameraId, null);

        if (willUpdatePreviewCamera && !mPaused) {
            notifyCameraIdChanged(localPreviewId, isRemoteCameraOpened());
        }
    }

    private void switchCamera() {
        closeLocalCamera();
        // update the preview camera
        updateCameraId(false, mCameraId, null);
        mCameraId = String.valueOf(mPendingSwitchCameraId);
        openCamera(mCameraId);
    }

    private boolean isNeedNotifyAAAManager(List<String> currentPreivewCamera) {
        boolean isNeed = false;
        if (ONLY_ONE_PREVIEW_CAMERA == currentPreivewCamera.size()
                && MultiCameraModuleUtil.hasLocalCamera(currentPreivewCamera)) {
            isNeed = true;
        }
        return isNeed;
    }

    private void updateUiVisibility(final List<String> previewId) {
        mAppController.getActivity().runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (ONLY_ONE_PREVIEW_CAMERA < previewId.size()
                        || (ONLY_ONE_PREVIEW_CAMERA == previewId.size() && MultiCameraModuleUtil
                                .hasRemoteCamera(previewId))) {
                    Log.i(TAG, "[updatePreviewCamera] hide setting and PickerManager");
                    mAppUi.hideSettingUi();
                    mAppUi.hidePickerManagerUi();
                    mAppUi.hideIndicatorManagerUi();
                } else {
                    Log.i(TAG, "[updateUiVisibility] show setting and PickerManager,"
                            + "mIsCanShowCommonUi = " + mIsCanShowCommonUi);
                    if (mIsCanShowCommonUi) {
                        mAppUi.showSettingUi();
                        mAppUi.showPickerManagerUi();
                        mAppUi.showIndicatorManagerUi();
                    }
                }
            }
        });
    }

    private boolean checkCameraId(String tagregId) {
        String[] allCameraId = mCameraManager.getCameraIdList();
        boolean isIdCorrect = MultiCameraModuleUtil.checkCameraId(tagregId, allCameraId);
        if (!isIdCorrect) {
            // update the camera manager, because current Camera Manager not
            // have updated
            mCameraManager = mAppController.getNewCameraManager();
            allCameraId = mCameraManager.getCameraIdList();
            // re-check the camera id
            isIdCorrect = MultiCameraModuleUtil.checkCameraId(tagregId, allCameraId);
            Log.i(TAG, "[checkCameraId] reget camera manager again, isIdCorrect = " + isIdCorrect);
        }
        return isIdCorrect;
    }

    private String calculateClickedCameraId(float x, float y) {
        String camera = null;
        List<String> remotePreviewCamera = new ArrayList<>();
        int width = (int) mPreviewArea.right;
        int height = (int) mPreviewArea.bottom;
        for (String cameraId : mPreviewCameraId) {
            if (!MultiCameraModuleUtil.isLocalCamera(cameraId)) {
                remotePreviewCamera.add(cameraId);
            }
        }
        // Because the activity orientation is locked,so when rotate the
        // device,the original point not changed
        // TODO when have more than one remote camera ,how to
        // calculate this
        if (mIsLandScape) {
            if (x < width / 2 && remotePreviewCamera.size() > 0) {
                camera = remotePreviewCamera.get(0);
            } else {
                camera = mCameraId;
            }
        } else {
            if (y < height / 2 && remotePreviewCamera.size() > 0) {
                camera = remotePreviewCamera.get(0);
            } else {
                camera = mCameraId;
            }
        }
        return camera;
    }

    private void previewAreaClicked(String cameraID) {
        Log.i(TAG, "[previewAreaClicked] cameraID = " + cameraID);
        List<String> singlePreviewId = new ArrayList<String>();
        singlePreviewId.add(cameraID);
        mPreviewCameraId = singlePreviewId;
        notifyCameraIdChanged(singlePreviewId, isRemoteCameraOpened());
        // when preview camera have changed,need update the shutter UI
        mAppUi.switchShutterButtonLayout(MultiCameraModuleUtil
                .canShowVideoButton(singlePreviewId) ? R.layout.camera_shutter_photo_video_v2
                : R.layout.camera_shutter_photo_v2);
    }

    private void updateMultiPreviewSetting(List<String> previewCameraId) {
        String multiPreviewValue = MULTI_PREVIEW_OFF;
        if (MultiCameraModuleUtil.hasRemoteCamera(previewCameraId)) {
            multiPreviewValue = MULTI_PREVIEW_ON;
        }
        mSettingServant.doSettingChange(SettingKeys.KEY_MULTI_CAMERA_MULTI_PREVIEW,
                multiPreviewValue, false);
    }
    /**
     * An implement class which detected remote camera device status.will be
     * notified when remote camera is connected or disconnected.
     */
    private class DevicesStatusListener implements IDevicesStatusListener {

        @Override
        public void onAvalibleDevicesUpdate(LinkedHashMap<String, IRemoteDevice> deviceInfo) {
            Log.i(TAG, "[onAvalibleDevicesUpdate]");
            mScanResultToViewListener.onAvalibleDevicesUpdate(deviceInfo);
        }

        @Override
        public void onConnectedDeviesUpdate(LinkedHashMap<String, IRemoteDevice> deviceInfo) {
            Log.i(TAG, "[onConnectedDeviesUpdate]");

            mScanResultToViewListener.onConnectedDeviesUpdate(deviceInfo);
            for (Entry<String, CameraDeviceProxy> entry : mRemoteCameraProxy.entrySet()) {
                String key = entry.getKey();
                int status = IMultiCameraDeviceAdapter.REMOTE_CAEMRA_STATUS_UNKNOWN;
                IRemoteDevice device = deviceInfo.get(key);
                if (device != null) {
                    status = device.get(IRemoteDevice.KEY_REMOTE_SERVICE_STATUS);
                }
                if (status != IMultiCameraDeviceAdapter.REMOTE_CAEMRA_STATUS_CONNECTED) {
                    Log.i(TAG, "device disconnected, close opened camera! key = " + key
                            + ",status = " + status);
                    doCloseRemoteCameras();
                }
            }
        }
    }

    /**
     * this implement class will be triggered when UI have be touched,such as
     * user click the scan button or disconnect button.
     */
    private class MultiCameraViewListenerImpl implements IMultiCameraViewListener {

        @Override
        public void regisiterListener(IStatusUpdateListener result) {
            mScanResultToViewListener = result;
        }

        @Override
        public void startScanRemoteDevice() {
            Log.i(TAG, "[startScanRemoteDevice]");
            if (mIMultiCameraDeviceAdapter != null) {
                mIMultiCameraDeviceAdapter.startScanDevice();
            }
        }

        @Override
        public void stopScanRemoteDevice() {
            Log.i(TAG, "[stopScanRemoteDevice]");
            if (mIMultiCameraDeviceAdapter != null) {
                mIMultiCameraDeviceAdapter.stopScanDevice();
            }
        }

        @Override
        public void connectRemoteDevice(String deviceKey) {
            Log.i(TAG, "[connectRemoteDevice] deviceKey " + deviceKey);
            if (mIMultiCameraDeviceAdapter != null) {
                mIMultiCameraDeviceAdapter.connectSelectedDevice(deviceKey);
            }
        }

        @Override
        public void openRemoteDevice(String cameraID, String deviceId) {
            Log.i(TAG, "openRemoteDevice cameraID " + cameraID + " deviceId " + deviceId);

            // If the device camera already has been opened, return directly.
            if (deviceId.equals(mOpenedRemoteDeviceId.get(cameraID))) {
                Log.i(TAG, "[openRemoteCamera] The device camera already has been opened.");
                return;
            }

            mOpeningRemoteDeviceId = deviceId;
            // if just supported one remote camera, when open other remote
            // camera, need close the former one
            if (ONLY_SUPPORTE_ONE_REMOTE_CAMERA) {
                doCloseRemoteCameras();
            }

            if (checkCameraId(cameraID)) {
                openCamera(cameraID);
            } else {
                throw new RuntimeException("open camera id is error");
            }

            Log.i(TAG, "[openRemoteCamera] cameraID = " + cameraID);
        }

        @Override
        public void closeAllRemoteCameras() {
            Log.i(TAG, "[closeAllRemoteCameras] mOpenedRemoteCameraId = " + mOpenedRemoteCameraId);

            doCloseRemoteCameras();
            // Disconnect all linked camera service.so need notify adapter
            if (mIMultiCameraDeviceAdapter != null) {
                mIMultiCameraDeviceAdapter.disconnectAllRemoteCamera();
            }
            mAppUi.switchShutterButtonLayout(R.layout.camera_shutter_photo_video_v2);
        }

        @Override
        public void backToMultiPreview() {
            Log.i(TAG, "[backToMultiPreview]mPreviewCameraId = " + mPreviewCameraId
                    + ", mHasOpenedCameraId = " + mHasOpenedCameraId);

            // Shallow copy the list value
            List<String> hasOpend = new ArrayList<String>();
            hasOpend.addAll(mHasOpenedCameraId);
            mPreviewCameraId = hasOpend;

            // how to notify the stream current preview need same multi-preview
            // before change to single preview
            notifyCameraIdChanged(mPreviewCameraId, isRemoteCameraOpened());
            // because multi-preview not support video,so the button need hide
            mAppUi.switchShutterButtonLayout(R.layout.camera_shutter_photo_v2);
        }

        @Override
        public void updateAllViewVisibility(boolean visible) {
            mIsCanShowCommonUi = visible;
            if (visible) {
                mAppUi.showThumbnailManagerUi();
                mAppUi.showModeOptionsUi();
                final List<String> previewCameraCopy = new ArrayList<String>();
                previewCameraCopy.addAll(mPreviewCameraId);
                mAppController.getActivity().runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (ONLY_ONE_PREVIEW_CAMERA == previewCameraCopy.size()
                                && MultiCameraModuleUtil.hasLocalCamera(previewCameraCopy)) {
                            Log.i(TAG, "[updateAllViewVisibility] show setting and PickerManager");
                            mAppUi.showSettingUi();
                            mAppUi.showPickerManagerUi();
                            mAppUi.showIndicatorManagerUi();
                        }
                    }
                });
            } else {
                mAppUi.setAllCommonViewButShutterVisible(false);
            }

            boolean canShowVideoButton = MultiCameraModuleUtil.canShowVideoButton(mPreviewCameraId);
            mAppUi.switchShutterButtonLayout(canShowVideoButton ?
                    R.layout.camera_shutter_photo_video_v2
                    : R.layout.camera_shutter_photo_v2);
            // Hide App UI and do not show UI by other case.
            mAppUi.stopShowCommonUI(visible ? false : true);

            Log.i(TAG, "[updateAllViewVisibility],visible= " + visible + ",canShowVideoButton = "
                    + canShowVideoButton);
        }
    }

    /**
     * A inner class used for maintain camera state such as opened and error
     * state. also detect the session state,such as configured and configure
     * fail.
     */
    private class CameraDeviceCallback extends CameraStateCallback
            implements CameraSessionCallback {
        private String mCameraId;
        private CameraDeviceProxy mCameraDeviceProxy;

        // CameraStateCallback ---begin
        @Override
        public void onOpened(CameraDeviceProxy camera) {
            mCameraId = camera.getCameraId();
            String tag = MultiCameraModule.class.getSimpleName() + "/"
                    + CameraDeviceCallback.class.getSimpleName() + "(" + mCameraId + ")";
            mCameraDeviceProxy = camera;
            Log.i(tag, "[onOpened] cameraId = " + mCameraId);

            if (MultiCameraModuleUtil.isLocalCamera(mCameraId)) {
                mPendingSwitchCameraId = UNKNOWN;
                mLocalCameraDeviceProxy = camera;
                if (!mPreviewCameraId.contains(mCameraId)) {
                    updateCameraId(true, mCameraId, null);
                }
            } else {
                if (!mPreviewCameraId.contains(mCameraId)) {
                    updateCameraId(true, mCameraId, mOpeningRemoteDeviceId);
                }
                if (!mRemoteCameraDeviceProxy.contains(camera)) {
                    mRemoteCameraDeviceProxy.add(camera);
                    mRemoteCameraProxy.put(mOpeningRemoteDeviceId, camera);
                }
            }
            mCameraDeviceProxy.requestChangeSessionOutputs(false);
        }

        @Override
        public void onError(int error) {

        }

        @Override
        public void onSessionConfigured() {
            mCameraDeviceProxy.requestChangeCaptureRequets(true, getRequestType(),
                    CaptureType.REPEATING_REQUEST);
        }

        // CameraStateCallback ---end

        @Override
        public void onSessionActive() {
        }

        @Override
        public void configuringSessionOutputs(List<Surface> sessionOutputSurfaces) {
            notifyCameraIdChanged(mPreviewCameraId, isRemoteCameraOpened());
            mCurrentMode.configuringSessionOutputs(sessionOutputSurfaces, mCameraId);
        }

        @Override
        public CaptureCallback configuringSessionRequests(Builder requestBuilder,
                RequestType requestType, CaptureType captureType) {
            CaptureCallback captureCallback = null;
            switch (requestType) {
            case RECORDING:
            case PREVIEW:
                captureCallback = mCaptureCallback;
                break;
            case STILL_CAPTURE:
                captureCallback = mCurrentMode.getCaptureCallback();
                break;
            default:
                break;
            }
            Map<RequestType, CaptureRequest.Builder> requestBuilders = new HashMap<RequestType,
                    CaptureRequest.Builder>();

            if (MultiCameraModuleUtil.isLocalCamera(mCameraId)) {
                Rect cropRegion = Utils.cropRegionForZoom(mAppController.getActivity(), mCameraId,
                        1f);
                // apply crop region
                requestBuilder.set(CaptureRequest.SCALER_CROP_REGION, cropRegion);
                requestBuilders.put(requestType, requestBuilder);
                // 1. apply 3A control
                mAaaControl.configuringSessionRequests(requestBuilders, captureType, true);
                // 2. apply addition parameter
                mDetectionManager.configuringSessionRequests(requestBuilders, captureType);
            } else {
                requestBuilders.put(requestType, requestBuilder);
            }
            // Apply mode parameter
            mCurrentMode.configuringSessionRequests(requestBuilders, mCameraId);
            return captureCallback;
        }

        private CaptureCallback mCaptureCallback = new CaptureCallback() {
            @Override
            public void onCaptureStarted(CameraCaptureSession session, CaptureRequest request,
                    long timestamp, long frameNumber) {
                if (MultiCameraModuleUtil.isLocalCamera(mCameraId)) {
                    mDetectionManager.onCaptureStarted(request, timestamp, frameNumber);
                }
                mCurrentMode.onPreviewCaptureStarted(request, timestamp, frameNumber);
            }

            @Override
            public void onCaptureProgressed(CameraCaptureSession session, CaptureRequest request,
                    CaptureResult partialResult) {
            };

            @Override
            public void onCaptureCompleted(CameraCaptureSession session, CaptureRequest request,
                    TotalCaptureResult result) {
                if (MultiCameraModuleUtil.isLocalCamera(mCameraId)) {
                    if (isNeedNotifyAAAManager(mPreviewCameraId)) {
                        mAaaControl.onPreviewCaptureCompleted(request, result);
                    } else {
                        // TODO clear action will be improved
                        mAaaControl.clearFocusUi();
                    }
                    mDetectionManager.onCaptureCompleted(request, result);
                }
                mCurrentMode.onPreviewCaptureCompleted(request, result);
            }
        };
    }
}
