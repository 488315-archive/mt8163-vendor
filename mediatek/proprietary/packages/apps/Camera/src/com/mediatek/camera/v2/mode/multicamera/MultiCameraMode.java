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
package com.mediatek.camera.v2.mode.multicamera;

import android.content.ContentValues;
import android.graphics.PixelFormat;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCaptureSession.CaptureCallback;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CaptureFailure;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureRequest.Builder;
import android.media.CamcorderProfile;
import android.media.MediaRecorder;
import android.net.Uri;
import android.util.Log;
import android.util.Size;
import android.view.Surface;
import android.view.ViewGroup;

import com.android.camera.FeatureSwitcher;
import com.android.camera.R;

import com.mediatek.camera.v2.control.IControl.IAaaController;
import com.mediatek.camera.v2.mode.AbstractCameraMode;
import com.mediatek.camera.v2.mode.multicamera.MultiCameraCaptureHelper.ImageCallback;
import com.mediatek.camera.v2.mode.normal.VideoHelper;
import com.mediatek.camera.v2.module.ModuleListener;
import com.mediatek.camera.v2.module.ModuleListener.CaptureType;
import com.mediatek.camera.v2.module.ModuleListener.RequestType;
import com.mediatek.camera.v2.platform.ModeChangeListener;
import com.mediatek.camera.v2.platform.app.AppController;
import com.mediatek.camera.v2.services.FileSaver.OnFileSavedListener;
import com.mediatek.camera.v2.services.ISoundPlayback;
import com.mediatek.camera.v2.setting.ISettingServant;
import com.mediatek.camera.v2.setting.SettingCtrl;
import com.mediatek.camera.v2.setting.SettingItem;
import com.mediatek.camera.v2.stream.ICaptureStream.CaptureStreamCallback;
import com.mediatek.camera.v2.stream.IPreviewStream.PreviewSurfaceCallback;
import com.mediatek.camera.v2.stream.IRecordStream.RecordStreamStatus;
import com.mediatek.camera.v2.stream.ImageInfo;
import com.mediatek.camera.v2.stream.RecordStream;
import com.mediatek.camera.v2.stream.StreamManager;
import com.mediatek.camera.v2.stream.multicamera.ISinkStreamCtrl;
import com.mediatek.camera.v2.util.SettingKeys;
import com.mediatek.camera.v2.util.Utils;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

/**
 * this class used for Multi Camera take picture and video.this part is between
 * the module and stream.
 */
public class MultiCameraMode extends AbstractCameraMode implements ImageCallback {

    private static final String TAG = MultiCameraMode.class.getSimpleName();
    private static final int REMOTE_CAMERA_PICTURE_SIZE_WIDTH = 1920;
    private static final int REMOTE_CAMERA_PICTURE_SIZE_HEIGHT = 1088;
    private static final int ONLY_ONE_CAMERA = 1;
    private static final int FIRST_INDEX = 0;
    private static final int MINUTE_TO_MINI_SECONEDS = 1000;
    private static final int LEFT_SHIFT_NUMBER = 3;

    private boolean mIsCapturing = false;
    private int mJpegOrientation = 0;
    private int mOrientation = 0;
    // current is in previewing camera id
    private List<String> mPreviewCameraId = new ArrayList<String>();
    // current have opened camera id
    private List<String> mOpenedCameraId = new ArrayList<String>();

    private CameraCharacteristics mCameraCharacteristics;
    private CaptureStreamCallback mCaptureStreamCallback;
    private ISinkStreamCtrl mISinkStreamCtrl;

    // capture/recorder/snap shot surface
    private Surface mRecordSurface;
    private Surface mRemoteCaptureSurface;
    private Surface mSnapShotSurface;
    private Map<String, Surface> mPreivewSurface = new HashMap<String, Surface>();
    private Map<String, Surface> mCaptureSurface = new HashMap<String, Surface>();
    private MultiCameraCaptureHelper mSnapShotSurfaceHelper;

    // video recording
    private boolean mIsRecording = false;
    private int mRecordingRotation;
    private int mRequestDurationLimit;
    private long mRequestSizeLimit = 0;
    private String mVideoTempPath;
    private CamcorderProfile mCameraCamcorderProfile;
    private RecordStreamStatus mRecordStreamCallback;
    private VideoHelper mVideoHelper;

    /**
     * Create a MultiCamera mode when into MultiCameraModule.
     * @param app
     *            the App level of controller. so mode will use this object to
     *            get or control the outside service.
     * @param moduleListener
     *            which mode request module do some settings,such as configure
     *            session and request.
     */
    public MultiCameraMode(AppController app, ModuleListener moduleListener) {
        super(app, moduleListener);
        mPreviewCameraId.add(mSettingServant.getCameraId());
        mCameraCharacteristics = Utils.getCameraCharacteristics(app.getActivity(),
                mSettingServant.getCameraId());
        mSnapShotSurfaceHelper = new MultiCameraCaptureHelper();
        mSnapShotSurfaceHelper.setImageCallback(this);
        mVideoHelper = new VideoHelper(app.getServices(), mIntent,
                mIsCaptureIntent, mSettingCtroller);
    }

    // open() before resume()
    @Override
    public void open(StreamManager streamManager, ViewGroup parentView, boolean isCaptureIntent) {
        super.open(streamManager, parentView, isCaptureIntent);
        // initialize the stream
        mISinkStreamCtrl = streamManager.getSinkStreamController();
        mISinkStreamCtrl.open(mAppController.getActivity(), parentView);
    }

    @Override
    public void resume() {
        mISinkStreamCtrl.resume();
        super.resume();
    }

    @Override
    public void pause() {
        super.pause();
        mISinkStreamCtrl.pause();
        mCaptureSurface.clear();
        mRemoteCaptureSurface = null;
        // also need restore the capture state
        restoreCaptureState();
    }

    @Override
    public void close() {
        super.close();
        mSnapShotSurfaceHelper.releaseSnapShotSurface();
        mISinkStreamCtrl.close();
    }

    @Override
    public void configuringSessionOutputs(List<Surface> sessionOutputSurfaces, String cameraId) {
        updatePreviewSurfaces();
        updatePictureSize();
        updateCaptureSurfaces();
        Surface previewSurface = mPreivewSurface.get(cameraId);
        if (previewSurface != null) {
            sessionOutputSurfaces.add(previewSurface);
        }
        Surface captureSurface = mCaptureSurface.get(cameraId);
        if (captureSurface != null) {
            sessionOutputSurfaces.add(captureSurface);
        }
        // configure Recording surface
        if (mIsRecording && mRecordSurface != null) {
            sessionOutputSurfaces.add(mRecordSurface);
        }

        if (sessionOutputSurfaces.size() == FIRST_INDEX) {
            throw new RuntimeException("configure session's surface is null");
        }
        Log.i(TAG, "[configuringSessionOutputs] suface size : " + sessionOutputSurfaces.size());
    }

    @Override
    public void configuringSessionOutputs(List<Surface> surfaces, boolean isLocalCamera) {
        // Do noting
    }

    @Override
    public void configuringSessionRequests(Map<RequestType, Builder> requestBuilders,
            String cameraId) {
        Log.i(TAG, "[configuringSessionRequests] cameraId: " + cameraId);
        Set<RequestType> keySet = requestBuilders.keySet();
        Iterator<RequestType> iterator = keySet.iterator();
        while (iterator.hasNext()) {
            RequestType requestType = iterator.next();
            Log.i(TAG, "requestType = " + requestType + " request number = " + keySet.size());
            switch (requestType) {
            case PREVIEW:
                configurePreviewRequest(requestBuilders.get(requestType), cameraId);
                break;
            case STILL_CAPTURE:
            case VIDEO_SNAP_SHOT:
                configureCaptureRequests(requestBuilders.get(requestType), cameraId);
                break;
            case RECORDING:
                configureRecordeRequests(requestBuilders.get(requestType), cameraId);
                break;

            default:
                break;
            }
        }
        Log.i(TAG, "[configuringSessionRequests] cameraId = " + cameraId + " -----");
    }

    @Override
    public void configuringSessionRequests(Map<RequestType, Builder> requestBuilders,
            boolean localCamera) {
        // do-noting
    }

    @Override
    public void updatePreviewCamera(List<String> previewCameraId, List<String> hasOpenedCamera) {
        super.updatePreviewCamera(previewCameraId, hasOpenedCamera);
        Log.i(TAG, "[updatePreviewCamera] previewCameraId = " + previewCameraId
                + ",hasOpenedCamera = " + hasOpenedCamera);
        mPreviewCameraId = previewCameraId;
        mOpenedCameraId = hasOpenedCamera;
        doUpdatePreviewArea();
    }

    @Override
    public boolean switchCamera(String cameraId) {
        // Switch camera need remove previous camera id from the list
        mPreviewCameraId.remove(SettingCtrl.BACK_CAMERA);
        mPreviewCameraId.remove(SettingCtrl.FRONT_CAMERA);
        mCameraCharacteristics = Utils.getCameraCharacteristics(mAppController.getActivity(),
                mSettingServant.getCameraId());
        return super.switchCamera(cameraId);
    }

    @Override
    public void onShutterPressed(boolean isVideo) {
    }

    @Override
    public void onShutterLongPressed(boolean isVideo) {
    }

    @Override
    public void onShutterReleased(boolean isVideo) {
    }

    @Override
    public void onShutterClicked(boolean isVideo) {
        Log.i(TAG, "[onShutterClicked] isVideo = " + isVideo);
        mJpegOrientation = Utils.getJpegRotation(mOrientation, mCameraCharacteristics);
        if (isVideo) {
            videoShutterButtonClick();
        } else {
            mIsCapturing = true;
            if (mIsRecording) {
                doSnapShot();
            } else {
                photoShutterButtonClick();
            }
        }
    }

    @Override
    public void onOrientationChanged(int orientation) {
        super.onOrientationChanged(orientation);
        mOrientation = orientation;
        mISinkStreamCtrl.onOrientationChanged(orientation);
    }

    // implement the ImageCallback
    @Override
    public void onImageAvaliable(ImageInfo imageInfo) {
        getCaptureStreamCallback().onCaptureCompleted(imageInfo);
    }

    @Override
    public CaptureCallback getCaptureCallback() {
        return new CaptureCallback() {
            @Override
            public void onCaptureFailed(CameraCaptureSession session, CaptureRequest request,
                    CaptureFailure failure) {
                super.onCaptureFailed(session, request, failure);
                Log.i(TAG, "[onCaptureFailed],reson = " + failure.getReason()
                        + ",the request is : " + failure.getRequest());
                restoreCaptureState();
            }
        };
    }

    @Override
    protected int getModeId() {
        return ModeChangeListener.MODE_MULTI_CAMERA_SINK;
    }

    @Override
    protected int getCaptureFormat() {
        int pictureForamt = PixelFormat.JPEG;
        if (mPreviewCameraId.size() > ONLY_ONE_CAMERA) {
            pictureForamt = PixelFormat.RGBA_8888;
        }
        Log.d(TAG, "[getCaptureFormat] current preview camera = " + mPreviewCameraId
                + ",pictureForamt = " + pictureForamt);
        return pictureForamt;
    }

    /**
     * ---if current is in recording, the picture size use the video frame size.
     * ---otherwise use picture size is: if is local single preview, the picture
     * size from setting if is Remote Single Preview or Multi-Preview,the size
     * is 1920 X 1088, this size is depended on WFD resolution // TODO the size
     * need get from the CameraCharacteristics
     */
    @Override
    protected Size getCaptureSize() {
        Size pictureSize = null;
        if (mIsRecording) {
            pictureSize = new Size(mCameraCamcorderProfile.videoFrameWidth,
                    mCameraCamcorderProfile.videoFrameHeight);
        } else {
            // if the mPreviewCameraId have no camera id.// TODO
            // current use a word around method in the construct function.
            if (MultiCameraUtil.isLocalSinglePreview(mPreviewCameraId)) {
                // because current the mPreviewCameraId's size is 1
                pictureSize = getCameraCaptureSize(mPreviewCameraId.get(FIRST_INDEX));
            } else {
                pictureSize = new Size(REMOTE_CAMERA_PICTURE_SIZE_WIDTH,
                        REMOTE_CAMERA_PICTURE_SIZE_HEIGHT);

                // create remote capture surface
                if (ONLY_ONE_CAMERA == mPreviewCameraId.size()) {
                    mRemoteCaptureSurface = mSnapShotSurfaceHelper.createRemoteCaptureSurface(
                            pictureSize, getCaptureFormat());
                }
            }
        }

        Log.d(TAG, "[getCaptureSize] recording = " + mIsRecording + ",Size = " + pictureSize);

        // create the snap shot surface
        mSnapShotSurface = mSnapShotSurfaceHelper.createSnapShotSurface(getPreviewSize(),
                getCaptureFormat());

        return pictureSize;
    }

    @Override
    protected Size getPreviewSize() {
        if (mIsRecording) {
            return new Size(mCameraCamcorderProfile.videoFrameWidth,
                    mCameraCamcorderProfile.videoFrameHeight);
        } else {
            return super.getPreviewSize();
        }
    }

    @Override
    protected CaptureStreamCallback getCaptureStreamCallback() {
        if (mCaptureStreamCallback == null) {
            mCaptureStreamCallback = new CaptureStreamCallback() {

                @Override
                public void onCaptureCompleted(ImageInfo imageInfo) {
                    Log.i(TAG, "[onCaptureCompleted] +++++,mIsRecording = " + mIsRecording);
                    // First: reset the mIsCapturing to false. used for judge
                    // whether is capture or not.because if current is in
                    // recording status.user can to snap shot, so also need
                    // restore the parameters.
                    byte[] data = imageInfo.getData();
                    int width = imageInfo.getWidth();
                    int heigth = imageInfo.getWidth();

                    mIsCapturing = false;
                    mModuleListener.requestUpdateCaptureStatus(false);

                    if (mIsRecording) {
                        mAppController.getActivity().runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mAppUi.setShutterButtonEnabled(true, false);
                            }
                        });
                    } else {
                        mCameraServices.getSoundPlayback().play(ISoundPlayback.SHUTTER_CLICK);
                        mAppController.getActivity().runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mAppUi.setAllCommonViewEnable(true);
                                mAppUi.setSwipeEnabled(true);
                            }
                        });
                    }
                    // Second: save image
                    if (data != null) {
                        int orientation = PixelFormat.JPEG == getCaptureFormat() ? mJpegOrientation
                                : mOrientation;
                        ContentValues values = MultiCameraUtil.getCaptureContentValues(width,
                                heigth, orientation, mLocationManager.getCurrentLocation(),
                                mStorageService.getFileDirectory());
                        mCameraServices.getMediaSaver().addImage(data, values, mMediaSavedListener,
                                mAppController.getActivity().getContentResolver());
                    }
                    Log.i(TAG, "[onCaptureCompleted] ---");
                }
            };
        }
        return mCaptureStreamCallback;
}

    @Override
    protected RecordStreamStatus getRecordStreamCallback() {
        if (mRecordStreamCallback == null) {
            mRecordStreamCallback = new RecordStreamStatus() {

                @Override
                public void onRecordingStoped() {
                    Log.i(TAG, "onRecordingStoped");
                    mSettingCtroller.doSettingChange(SettingKeys.KEY_VIDEO, "off", false);
                    ContentValues contentValues = MultiCameraUtil.getVideoContentValues(
                            mAppController.getActivity(), mCameraCamcorderProfile,
                            mLocationManager.getCurrentLocation(), mRecordingRotation,
                            mVideoTempPath,mStorageService.getFileDirectory());
                    mCameraServices.getMediaSaver().addVideo(mVideoTempPath, contentValues,
                            mMediaSavedListener, mAppController.getActivity().getContentResolver());
                    mVideoTempPath = null;
                    mCameraCamcorderProfile = null;
                }

                @Override
                public void onRecordingStarted() {
                }

                @Override
                public void onInfo(int what, int extra) {
                    switch (what) {
                    case RecordStream.MEDIA_RECORDER_INFO_MAX_FILESIZE_REACHED:
                        if (mIsRecording) {
                            mAppUi.showInfo(mActivity.getResources().getString(
                                    R.string.video_reach_size_limit));
                            videoShutterButtonClick();
                        }
                        break;
                    case RecordStream.MEDIA_RECORDER_INFO_MAX_DURATION_REACHED:
                        if (mIsRecording) {
                            videoShutterButtonClick();
                        }
                        break;
                    default:
                        break;
                    }
                }

                @Override
                public void onError(int what, int extra) {
                    // TODO if error happens,what to do?
                }
            };
        }
        return mRecordStreamCallback;
    }

    /**
     * when file have saved ,need notify Manager.
     */
    protected OnFileSavedListener mMediaSavedListener = new OnFileSavedListener() {

        @Override
        public void onMediaSaved(Uri uri) {
            Log.i(TAG, "onMediaSaved uri = " + uri);
            // only video saving need to dismiss saving dialog.
            mAppUi.dismissSavingProgress();
            mAppController.notifyNewMedia(uri);
        }
    };

    private void updateCaptureSurfaces() {
        Map<String, Surface> surface = mCaptureController.getCaptureInputSurface();
        mCaptureSurface.clear();
        mCaptureSurface.putAll(surface);

        // Add the remote capture surface
        if (ONLY_ONE_CAMERA == mPreviewCameraId.size()
                && !MultiCameraUtil.isLocalSinglePreview(mPreviewCameraId)) {
            String key = mPreviewCameraId.get(FIRST_INDEX);
            mCaptureSurface.put(key, mRemoteCaptureSurface);
        }
        MultiCameraUtil.printSurfaceLog("updateCaptureSurfaces", mCaptureSurface);
    }

    private void updatePreviewSurfaces() {
        Map<String, Surface> surface = mPreviewController.getPreviewInputSurfaces();
        mPreivewSurface.clear();
        mPreivewSurface.putAll(surface);
        MultiCameraUtil.printSurfaceLog("updatePreviewSurfaces", mPreivewSurface);
    }

    private void doUpdatePreviewArea() {
        // because when capture will do configureSesionOutputs,but this case we
        // don't want change the layout
        if (!mIsCapturing) {
            List<String> previewCameraCopy = new ArrayList<String>();
            previewCameraCopy.addAll(mPreviewCameraId);
            List<String> hasOpenedCopy = new ArrayList<String>();
            hasOpenedCopy.addAll(mOpenedCameraId);

            mISinkStreamCtrl.updatePreviewCamera(previewCameraCopy, hasOpenedCopy);
        }
        Log.i(TAG, "[doUpdatePreviewArea],mIsCapturing = " + mIsCapturing);
    }

    private Size getCameraCaptureSize(String cameraId) {
        ISettingServant settingServant = mSettingCtroller.getSettingServant(cameraId);
        SettingItem captureSizeItem = settingServant.getSettingItem(SettingKeys.KEY_PICTURE_SIZE);
        if (captureSizeItem == null) {
            return null;
        } else {
            String size = captureSizeItem.getValue();
            Log.i(TAG, "getCameraCaptureSize camera id:" + cameraId + " captureSizeString:" + size);
            if (size != null) {
                return Utils.getSize(size);
            } else {
                return null;
            }
        }
    }

    private void configurePreviewRequest(CaptureRequest.Builder requestBuilder, String cameraId) {
        Log.i(TAG, "[configurePreviewRequest] cameraId = " + cameraId);
        Surface previewSurface = mPreivewSurface.get(cameraId);
        // If the preview surface is null,get again. if the surface also is
        // null.will throw null pointer exception for track.
        if (previewSurface == null) {
            updatePreviewSurfaces();
            previewSurface = mPreivewSurface.get(cameraId);
        }
        if (previewSurface.isValid()) {
            Log.i(TAG, "[configurePreviewRequest] previewSurface = " + previewSurface);
            requestBuilder.addTarget(previewSurface);
        }
        Log.i(TAG, "[configurePreviewRequest] cameraId = " + cameraId + "------");

    }

    private void configureCaptureRequests(CaptureRequest.Builder requestBuilder, String cameraId) {
        Surface captureSurface = mCaptureSurface.get(cameraId);
        Log.i(TAG, "[configureCaptureRequests] captureSurface = " + captureSurface + ",cameraId = "
                + cameraId);
        requestBuilder.addTarget(captureSurface);
        requestBuilder.set(CaptureRequest.JPEG_QUALITY, JPEG_QUALITY);
    }

    private void configureRecordeRequests(CaptureRequest.Builder requestBuilder, String cameraId) {
        Surface previewSurface = mPreivewSurface.get(cameraId);
        requestBuilder.addTarget(previewSurface);
        if (mRecordSurface != null) {
            requestBuilder.addTarget(mRecordSurface);
        }
        configreEisValue(requestBuilder);
    }

    private void photoShutterButtonClick() {
        Log.i(TAG, "photoShutterButtonClick, mCurrentCameraId = " + mPreviewCameraId);
        mAppUi.setAllCommonViewEnable(false);
        mAppUi.setSwipeEnabled(false);
        mModuleListener.requestUpdateCaptureStatus(true);
        // Why need configure session before take picture ?.
        mModuleListener.requestChangeSessionOutputs(true);
        // size > 1 means current is multi-preview,use snap shot for take picture
        if (mPreviewCameraId.size() > ONLY_ONE_CAMERA) {
            mISinkStreamCtrl.setSnapShotSurface(mSnapShotSurface);
            mISinkStreamCtrl.doSnapShot();
        } else {
            updateCaptureSurfaces();
            if (MultiCameraUtil.isLocalSinglePreview(mPreviewCameraId)) {
                IAaaController aaaController = mModuleListener.get3AController(null);
                aaaController.aePreTriggerAndCapture();
            } else {
                mModuleListener.requestChangeCaptureRequets(false, true, RequestType.STILL_CAPTURE,
                        CaptureType.CAPTURE);
            }
        }
    }

    private void videoShutterButtonClick() {
        Log.i(TAG, "[videoShutterButtonClicked] mIsRecording = " + mIsRecording);
        if (mIsRecording) {
            // Stop VR
            stopRecording();
            mAppUi.stopShowCommonUI(false);
            mAppUi.switchShutterButtonImageResource(R.drawable.btn_video, true);
            mAppUi.setSwipeEnabled(true);
            mAppUi.showModeOptionsUi();
            mAppUi.showSettingUi();
            mAppUi.showIndicatorManagerUi();
            mAppUi.showPickerManagerUi();
            mAppUi.setThumbnailManagerEnable(true);
            mAppUi.setAllCommonViewEnable(true);
            mModuleListener.requestUpdateRecordingStatus(false);
            Size pictureSize = getCaptureSize();
            String pictureFormat = pictureSize.getWidth() + "x" + pictureSize.getHeight()
                    + "-superfine";
            mAppUi.showLeftCounts(Utils.getImageSize(pictureFormat), true);
        } else {
            // Start VR
            mCameraServices.getSoundPlayback().play(ISoundPlayback.START_VIDEO_RECORDING);
            startRecording();
            mAppUi.stopShowCommonUI(true);
            mAppUi.switchShutterButtonImageResource(R.drawable.btn_video_mask, true);
            mAppUi.setSwipeEnabled(false);
            mAppUi.hideModeOptionsUi();
            mAppUi.hideSettingUi();
            mAppUi.hideIndicatorManagerUi();
            mAppUi.hidePickerManagerUi();
            mAppUi.setThumbnailManagerEnable(false);
            mModuleListener.requestUpdateRecordingStatus(true);
            Log.i(TAG, "[videoShutterButtonClicked] mCameraCamcorderProfile = "
                    + mCameraCamcorderProfile);
            long bytePerMs = ((mCameraCamcorderProfile.videoBitRate +
                    mCameraCamcorderProfile.audioBitRate) >> LEFT_SHIFT_NUMBER) /
                    MINUTE_TO_MINI_SECONEDS;
            mAppUi.showLeftTime(bytePerMs);
        }
    }

    private void doSnapShot() {
        Log.i(TAG, "[doSnapShot]");
        mAppUi.setShutterButtonEnabled(false, false);
        mModuleListener.requestChangeCaptureRequets(true, RequestType.VIDEO_SNAP_SHOT,
                CaptureType.CAPTURE);
    }

    private void startRecording() {
        Log.i(TAG, "[startRecording]+");
        mIsRecording = true;
        prepareRecording();
        updatePictureSize();
        updateCaptureSurfaces();
        updatePreviewSize(new PreviewSurfaceCallback() {
            @Override
            public void onPreviewSufaceIsReady(boolean surfaceChanged) {
                Log.i(TAG, "startRecording onPreviewSufaceIsReady");
                mModuleListener.requestChangeSessionOutputs(true);
                mSettingCtroller.doSettingChange(SettingKeys.KEY_VIDEO, "on", false);
                mModuleListener.requestChangeCaptureRequets(true/* sync */, RequestType.RECORDING,
                        CaptureType.REPEATING_REQUEST);
                mRecordController.startRecord();
                mAppController.enableKeepScreenOn(true);
                Log.i(TAG, "[startRecording]-");
            }
        });
    }

    private void stopRecording() {
        Log.i(TAG, "[stopRecording]+");
        mIsRecording = false;
        updatePictureSize();
        updatePreviewSize(new PreviewSurfaceCallback() {
            @Override
            public void onPreviewSufaceIsReady(boolean surfaceChanged) {
                Log.i(TAG, "stopRecording onPreviewSufaceIsReady");
                mRecordSurface = null;
                mModuleListener.requestChangeSessionOutputs(true);
                mModuleListener.requestChangeCaptureRequets(true/* sync */, RequestType.PREVIEW,
                        CaptureType.REPEATING_REQUEST);
                try {
                    mRecordController.stopRecord(true);
                    mAppUi.showSavingProgress(mAppController.getActivity().getResources()
                            .getString(R.string.saving));
                } catch (IllegalStateException e) {
                    e.printStackTrace();
                    Log.e(TAG, "stopRecording with exception:" + e);
                    if (mVideoTempPath != null) {
                        MultiCameraUtil.deleteVideoFile(mVideoTempPath);
                        mVideoTempPath = null;
                    }
                    mCameraCamcorderProfile = null;
                } finally {
                    mCameraServices.getSoundPlayback().play(ISoundPlayback.STOP_VIDEO_RECORDING);
                    mAppController.enableKeepScreenOn(false);
                    Log.i(TAG, "[stopRecording]-");
                }
            }
        });
    }

    private void prepareRecording() {
        Log.i(TAG, "[prepareRecording]  ++");

        int cameraId = Integer.valueOf(mSettingServant.getCameraId());
        int videoQualityValue = mVideoHelper.getRecordingQuality(cameraId);
        mCameraCamcorderProfile = mVideoHelper.fetchProfile(videoQualityValue, cameraId);
        String mirc = mSettingServant.getSettingValue(SettingKeys.KEY_VIDEO_RECORD_AUDIO);
        boolean enableAudio = "on".equals(mirc);
        mRecordingRotation = Utils.getRecordingRotation(
                mOrientation,
                Utils.getCameraCharacteristics(mAppController.getActivity(),
                        mSettingServant.getCameraId()));

        mVideoTempPath = mStorageService.getFileDirectory() + '/' + "videorecorder"
                + mVideoHelper.convertOutputFormatToFileExt(mCameraCamcorderProfile.fileFormat)
                + ".tmp";
        mRecordController.setOutputFile(mVideoTempPath);

        mRecordController.setMediaRecorderParameters(MultiCameraUtil.getMediaRecordingParamters());
        mRecordController.setMaxFileSize(mVideoHelper.getRecorderMaxSize(mRequestSizeLimit));
        mRecordController.setMaxDuration(MINUTE_TO_MINI_SECONEDS * mRequestDurationLimit);
        mRecordController.setRecordingProfile(mCameraCamcorderProfile);
        mRecordController.enalbeAudioRecording(enableAudio);
        mRecordController.setOutputFile(mVideoTempPath);
        mRecordController.setOrientationHint(mRecordingRotation);
        mRecordController.setAudioSource(MediaRecorder.AudioSource.CAMCORDER);
        mRecordController.setVideoSource(MediaRecorder.VideoSource.SURFACE);
        if (FeatureSwitcher.isHdRecordingEnabled()) {
            mRecordController.setHDRecordMode(
                    mSettingServant.getSettingValue(SettingKeys.KEY_VIDEO_HD_AUDIO_RECORDING));
        }
        mRecordController.prepareRecord();

        mRecordSurface = mRecordController.getRecordInputSurface();
        Log.i(TAG, "[prepareRecording]  ----mRecordSurface = " + mRecordSurface);
    }

    private void configreEisValue(Builder requestBuilder) {
        String eisValue = mSettingServant.getSettingValue(SettingKeys.KEY_VIDEO_EIS);
        Log.i(TAG, "configuringRecordingRequests eisValue = " + eisValue);
        if ("on".equals(eisValue)) {
            requestBuilder.set(CaptureRequest.CONTROL_VIDEO_STABILIZATION_MODE,
                    CaptureRequest.CONTROL_VIDEO_STABILIZATION_MODE_ON);
        } else {
            requestBuilder.set(CaptureRequest.CONTROL_VIDEO_STABILIZATION_MODE,
                    CaptureRequest.CONTROL_VIDEO_STABILIZATION_MODE_OFF);
        }
    }

    private void restoreCaptureState() {
        Log.d(TAG, "[restoreCaptureState] mIsCapturing = " + mIsCapturing);
        // Why just is capturing need update the view state ?
        if (mIsCapturing) {
            mAppController.getActivity().runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    mAppUi.setAllCommonViewEnable(true);
                    mAppUi.setSwipeEnabled(true);
                }
            });

            // Need Restore the capture state,such as when in single remote
            // capture, remote camera will in background,so the capture will
            // be fail. but the capture state need restore.
            mIsCapturing = false;
            mModuleListener.requestUpdateCaptureStatus(false);
        }
    }
}
