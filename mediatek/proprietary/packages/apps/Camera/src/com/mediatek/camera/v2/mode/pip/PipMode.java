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
 * MediaTek Inc. (C) 2014. All rights reserved.
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

package com.mediatek.camera.v2.mode.pip;

import android.content.ContentValues;
import android.graphics.Bitmap;
import android.graphics.PixelFormat;
import android.graphics.RectF;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureRequest.Builder;
import android.media.CamcorderProfile;
import android.media.Image;
import android.media.MediaRecorder;
import android.net.Uri;
import android.os.Handler;
import android.provider.MediaStore.Images.ImageColumns;
import android.provider.MediaStore.MediaColumns;
import android.provider.MediaStore.Video;
import android.util.Size;
import android.view.Surface;
import android.view.ViewGroup;

import com.android.camera.R;
import com.mediatek.camera.debug.LogHelper;
import com.mediatek.camera.debug.LogHelper.Tag;
import com.mediatek.camera.v2.control.IControl.IAaaController;
import com.mediatek.camera.v2.mode.AbstractCameraMode;
import com.mediatek.camera.v2.mode.pip.combination.FdAffectedRule;
import com.mediatek.camera.v2.mode.pip.combination.PictureSizeRule;
import com.mediatek.camera.v2.mode.pip.combination.PreviewSizeRule;
import com.mediatek.camera.v2.mode.pip.combination.VideoQualityRule;
import com.mediatek.camera.v2.module.ModuleListener;
import com.mediatek.camera.v2.module.ModuleListener.CaptureType;
import com.mediatek.camera.v2.module.ModuleListener.RequestType;
import com.mediatek.camera.v2.platform.ModeChangeListener;
import com.mediatek.camera.v2.platform.app.AppController;
import com.mediatek.camera.v2.platform.app.AppUi;
import com.mediatek.camera.v2.services.FileSaver.OnFileSavedListener;
import com.mediatek.camera.v2.services.ISoundPlayback;
import com.mediatek.camera.v2.setting.ISettingRule;
import com.mediatek.camera.v2.setting.ISettingServant;
import com.mediatek.camera.v2.setting.ISettingServant.ISettingChangedListener;
import com.mediatek.camera.v2.setting.SettingCtrl;
import com.mediatek.camera.v2.setting.SettingItem;
import com.mediatek.camera.v2.stream.ICaptureStream.CaptureStreamCallback;
import com.mediatek.camera.v2.stream.IPreviewStream.PreviewSurfaceCallback;
import com.mediatek.camera.v2.stream.IRecordStream;
import com.mediatek.camera.v2.stream.ImageInfo;
import com.mediatek.camera.v2.stream.IRecordStream.RecordStreamStatus;
import com.mediatek.camera.v2.stream.StreamManager;
import com.mediatek.camera.v2.stream.pip.IPipStream;
import com.mediatek.camera.v2.stream.pip.IPipStream.PipStreamCallback;
import com.mediatek.camera.v2.stream.pip.IPipView;
import com.mediatek.camera.v2.stream.pip.IPipView.PipViewCallback;
import com.mediatek.camera.v2.stream.pip.PipStreamView;
import com.mediatek.camera.v2.stream.pip.pipwrapping.PipEGLConfigWrapper;

import com.mediatek.camera.v2.util.SettingKeys;
import com.mediatek.camera.v2.util.Utils;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class PipMode extends AbstractCameraMode {
    private final Tag mTag;
    private final String mOriginalCameraIdStr;
    private IPipStream mPipStreamController;
    private ModePipStreamCallback mPipCallback;
    private PictureSizeRule mPictureSizeRule;
    private PreviewSizeRule mPreviewSizeRule;
    private VideoQualityRule mVideoQualityRule;
    private ISettingRule mFaceDetectionRule;
    private ISettingRule mGestureShotRule;
    private ISettingRule mSmileShotRule;

    private IPipView mPipViewController;

    // preview
    private Surface mBottomPreviewSurface;
    private Surface mTopPreviewSurface;

    // capture / video snap shot
    private Surface mBottomCaptureSurface;
    private Surface mTopCaptureSurface;
    private CaptureStreamCallback mCaptureStreamCallback;
    private boolean mDuringCapture;

    // video recording
    private boolean mRecording = false;
    private static final long SHUTTER_BUTTON_TIMEOUT = 500L; // 500ms
    private Handler mHandler = new Handler();
    private boolean mIsPaused;
    private RecordStreamStatus mRecordStreamCallback;
    private int mRecordingRotation;
    private CamcorderProfile mCameraCamcorderProfile;
    private String mVideoTempPath;

    // app resources
    private int mCurrentOrientation;

    private ContentValues mCapContentValues;
    private ContentValues mVideoContentValues;

    public PipMode(AppController app, ModuleListener moduleListener) {
        super(app, moduleListener);
        mAppUi.switchShutterButtonLayout(R.layout.camera_shutter_photo_video_v2);
        mTag = new Tag(PipMode.class.getSimpleName() + "(" + mFeatureTag + ")");
        mVideoQualityRule = new VideoQualityRule(mSettingCtroller);
        mPreviewSizeRule = new PreviewSizeRule(mSettingCtroller);
        mPictureSizeRule = new PictureSizeRule(mSettingCtroller);
        mFaceDetectionRule = new FdAffectedRule(
                mSettingCtroller, SettingKeys.KEY_CAMERA_FACE_DETECT);
        mGestureShotRule = new FdAffectedRule(
                mSettingCtroller, SettingKeys.KEY_GESTURE_SHOT);
        mSmileShotRule = new FdAffectedRule(
                mSettingCtroller, SettingKeys.KEY_SMILE_SHOT);

        mSettingServant  = mSettingCtroller.getSettingServant(
                mSettingCtroller.getCurrentCameraId());

        mSettingCtroller.addRule(SettingKeys.KEY_PHOTO_PIP,
                SettingKeys.KEY_PICTURE_SIZE, mPictureSizeRule);
        mSettingCtroller.addRule(SettingKeys.KEY_PHOTO_PIP,
                SettingKeys.KEY_PICTURE_RATIO, mPreviewSizeRule);
        mSettingCtroller.addRule(SettingKeys.KEY_PHOTO_PIP,
                SettingKeys.KEY_VIDEO_QUALITY, mVideoQualityRule);
        mSettingCtroller.addRule(SettingKeys.KEY_PHOTO_PIP,
                SettingKeys.KEY_CAMERA_FACE_DETECT, mFaceDetectionRule);
        mSettingCtroller.addRule(SettingKeys.KEY_PHOTO_PIP,
                SettingKeys.KEY_GESTURE_SHOT, mGestureShotRule);
        mSettingCtroller.addRule(SettingKeys.KEY_PHOTO_PIP,
                SettingKeys.KEY_SMILE_SHOT, mSmileShotRule);

        mOriginalCameraIdStr = mSettingServant.getCameraId();

        mPipCallback = new ModePipStreamCallback(new Handler(app.getActivity().getMainLooper()));
    }

    @Override
    protected void updateCaredSettingChangedKeys() {
        super.updateCaredSettingChangedKeys();
        addCaredSettingChangedKeys(SettingKeys.KEY_CAMERA_ID);
    }

    @Override
    public void onSettingChanged(Map<String, String> result) {
        super.onSettingChanged(result);
        // when camera id changed, update relative setting servant
        if (result.containsKey(SettingKeys.KEY_CAMERA_ID)) {
            String cameraId = result.get(SettingKeys.KEY_CAMERA_ID);
            if (cameraId != null) {
                LogHelper.i(mTag, "onSettingChanged cameraId new :" + cameraId);
                mSettingServant.unRegisterSettingChangedListener(this);
                mSettingServant = mSettingCtroller.getSettingServant(cameraId);
                mSettingServant.registerSettingChangedListener(this,
                        mCaredSettingChangedKeys, ISettingChangedListener.MIDDLE_PRIORITY);
            }
        }
    }

    @Override
    protected int getModeId() {
        return ModeChangeListener.MODE_PIP;
    }

    @Override
    public void open(StreamManager streamManager, ViewGroup parentView,
            boolean isCaptureIntent) {
        LogHelper.i(mTag, "[open]+");

        super.open(streamManager, parentView, isCaptureIntent);
        mPipStreamController = streamManager.getPipStreamController();
        mPipStreamController.registerPipStreamCallback(mPipCallback);
        mPipViewController = new PipStreamView(
                mAppController,
                mPipStreamController,
                new PipViewCallbackImpl());

        mPipViewController.open();
        mPipStreamController.open(mAppController.getActivity());
        if (!SettingCtrl.BACK_CAMERA.equals(mSettingCtroller.getCurrentCameraId())) {
            mPipStreamController.switchingPip();
        }
        LogHelper.i(mTag, "[open]-");
    }

    @Override
    public void close() {
        LogHelper.i(mTag, "[close]+");

        mPipStreamController.close();
        mPipStreamController.unregisterPipStreamCallback(mPipCallback);
        mPipViewController.close();
        super.close();
        if (!mOriginalCameraIdStr.equals(mSettingServant.getCameraId())) {
            mAppUi.performCameraPickerBtnClick();
        }
        LogHelper.i(mTag, "[close]-");
    }

    @Override
    public void resume() {
        LogHelper.i(mTag, "[resume]+");
        mIsPaused = false;
        mDuringCapture = false;
        mPipStreamController.resume();
        super.resume();

        LogHelper.i(mTag, "[resume]-");
    }

    @Override
    public void pause() {
        LogHelper.i(mTag, "[pause]+");
        mIsPaused = true;
        if (mRecording) {
            videoShutterClicked();
        }
        mPipStreamController.pause();
        super.pause();
        mBottomCaptureSurface = null;
        mTopCaptureSurface = null;
        LogHelper.i(mTag, "[pause]-");
    }

    @Override
    public void onOrientationChanged(int orientation) {
        super.onOrientationChanged(orientation);
        mCurrentOrientation = orientation;
        mPipStreamController.onOrientationChanged(orientation);
        mPipViewController.onOrientationChanged(orientation);
    }

    @Override
    public void onPreviewVisibilityChanged(int visibility) {
        super.onPreviewVisibilityChanged(visibility);
        LogHelper.i(mTag, "onPreviewVisibilityChanged visibility:" + visibility);
        mPipViewController.onPreviewVisibleChanged(
                visibility == AppUi.PREVIEW_VISIBILITY_UNCOVERED);
    }

    @Override
    public void onPreviewAreaChanged(RectF previewArea) {
        LogHelper.i(mTag, "onPreviewAreaChanged previewArea:" + previewArea);
        super.onPreviewAreaChanged(previewArea);
        mPipStreamController.onPreviewAreaChanged(previewArea);
    }

    @Override
    public ModeGestureListener getModeGestureListener() {
        return mModeGestureListener;
    }

    @Override
    public void onShutterPressed(boolean isVideo) {

    }

    @Override
    public void onShutterClicked(boolean isVideo) {
        if (isVideo) {
            if (!mDuringCapture) {
                videoShutterClicked();
            }
            return;
        }
        if (mRecording) {
            takeVss();
            return;
        }
        takePicture();
        return;
    }

    @Override
    public void onShutterLongPressed(boolean isVideo) {

    }

    @Override
    public void onShutterReleased(boolean isVideo) {

    }

    @Override
    public boolean onBackPressed() {
        if (mRecording) {
            onShutterClicked(true/**video shutter**/);
            return true;
        }
        if (mPipViewController != null) {
            return mPipViewController.onBackPressed();
        }
        return super.onBackPressed();
    }

    @Override
    public boolean switchCamera(String cameraId) {
        LogHelper.i(mTag, "switchCamera cameraId:" + cameraId);
        mPipStreamController.switchingPip();
        return true;
    }

    @Override
    protected CaptureStreamCallback getCaptureStreamCallback() {
        if (mCaptureStreamCallback == null) {
            mCaptureStreamCallback = new CaptureStreamCallback() {
                @Override
                public void onCaptureCompleted(ImageInfo imageInfo) {
                    byte[] data = imageInfo.getData();
                    int width = imageInfo.getWidth();
                    int heigth = imageInfo.getHeigth();
                    LogHelper.i(mTag, "onCaptureCompleted width = " + width
                            + ",heigth = " + heigth);
                    if (mRecording) {
                        mAppController.getActivity().runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mAppUi.setShutterButtonEnabled(true, false/*photo shutter*/);
                            }
                        });
                        mDuringCapture = false;
                    } else {
                        mCameraServices.getSoundPlayback().play(ISoundPlayback.SHUTTER_CLICK);
                        mBottomCaptureSurface = null;
                        mModuleListener.requestChangeSessionOutputs(true/*sync*/, true);
                        mAppController.getActivity().runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mDuringCapture = false;
                                mAppUi.setAllCommonViewEnable(true);
                                mAppUi.setSwipeEnabled(true);
                            }
                        });
                    }
                    int orientation = Utils.getJpegRotation(mCurrentOrientation,
                            Utils.getCameraCharacteristics(
                            mAppController.getActivity(), mSettingServant.getCameraId()));
                    if (data != null) {
                        updateCaptureContentValues(width, heigth, orientation);
                        mCameraServices.getMediaSaver().addImage(data, mCapContentValues,
                            mMediaSavedListener, mAppController.getActivity().getContentResolver());
                    }
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
                public void onRecordingStarted() {

                }
                @Override
                public void onRecordingStoped() {
                    LogHelper.i(mTag, "onRecordingStoped");
                    mSettingCtroller.doSettingChange(SettingKeys.KEY_VIDEO_PIP, "off", false);
                    updateVideoContentValues();
                    mCameraServices.getMediaSaver().addVideo(mVideoTempPath, mVideoContentValues,
                            mMediaSavedListener, mAppController.getActivity().getContentResolver());
                    mVideoTempPath = null;
                    mCameraCamcorderProfile = null;
                }
                @Override
                public void onInfo(int what, int extra) {

                }
                @Override
                public void onError(int what, int extra) {

                }
            };
        }
        return mRecordStreamCallback;
    }

    @Override
    protected Size getPreviewSize() {
        LogHelper.i(mTag, "[getPreviewSize]+ mRecording: " + mRecording +
                " mCameraCamcorderProfile: " + mCameraCamcorderProfile);
        if (mRecording && mCameraCamcorderProfile != null) {
            return new Size(mCameraCamcorderProfile.videoFrameWidth,
                    mCameraCamcorderProfile.videoFrameHeight);
        }
        Size bottomSize = getPreviewSize(mSettingServant);
        Size topSize = getPreviewSize(mSettingCtroller.getSettingServant(getAnotherCameraId()));
        LogHelper.i(mTag, "getPreviewSize bottomSize:" + Utils.buildSize(bottomSize) +
                " topSize:" + Utils.buildSize(topSize));
        // pip need the same preview size,
        if (Utils.compareSize(bottomSize, topSize)) {
            return topSize;
        }
        return bottomSize;
    }

    @Override
    protected int getCaptureFormat() {
        return PixelFormat.RGBA_8888;
    }

    @Override
    protected Size getCaptureSize() {
        if (mRecording && mCameraCamcorderProfile != null) {
            return new Size(mCameraCamcorderProfile.videoFrameWidth,
                    mCameraCamcorderProfile.videoFrameHeight);
        }
        Size mainCamCaptureSize = getCameraCaptureSize(SettingCtrl.BACK_CAMERA);
        Size subCamCaptureSize = getCameraCaptureSize(SettingCtrl.FRONT_CAMERA);
        LogHelper.i(mTag, "getCaptureSize bottomSize:" + Utils.buildSize(mainCamCaptureSize) +
                " subCamCaptureSize:" + Utils.buildSize(subCamCaptureSize));
        if (mainCamCaptureSize == null || subCamCaptureSize == null) {
            return null;
        }
        if (Utils.compareSize(mainCamCaptureSize, subCamCaptureSize)) {
            return mainCamCaptureSize;
        }
        return subCamCaptureSize;
    }

    @Override
    protected boolean changingModePictureSize() {
        super.changingModePictureSize();
        if (mRecording) {
            mPipStreamController.setCaptureSize(getPreviewSize(), getPreviewSize());
            return false;
        } else {
            Size mainCamCaptureSize = getCameraCaptureSize(SettingCtrl.BACK_CAMERA);
            Size subCamCaptureSize = getCameraCaptureSize(SettingCtrl.FRONT_CAMERA);
            mPipStreamController.setCaptureSize(mainCamCaptureSize, subCamCaptureSize);
            return true;
        }
    }

    @Override
    public void configuringSessionOutputs(List<Surface> sessionOutputSurfaces, boolean mainCamera) {
        LogHelper.i(mTag, "[configuringSessionOutputs]+ mainCamera = " + mainCamera);
        waitPreviewSurfaceReady();
        // pip video recording get buffer from preview,
        // no need to configure recording stream from camera device.
        if (mainCamera) {
            if (mRecording) {
                updatePictureSize();
                updateCaptureSurfaces(true/**update main**/);
            }
            sessionOutputSurfaces.add(mBottomPreviewSurface);
            if (mBottomCaptureSurface != null) {
                sessionOutputSurfaces.add(mBottomCaptureSurface);
            }
        } else {
            updatePictureSize();
            updateCaptureSurfaces(false/**update sub**/);
            sessionOutputSurfaces.add(mTopPreviewSurface);
            if (mTopCaptureSurface != null) {
                sessionOutputSurfaces.add(mTopCaptureSurface);
            }
        }
        LogHelper.i(mTag, "[configuringSessionOutputs]- "
                + " output surface size = " + sessionOutputSurfaces.size());
    }

    @Override
    public void configuringSessionRequests(
            Map<RequestType, Builder> requestBuilders, boolean isMainCamera) {
        LogHelper.i(mTag, "[configuringSessionRequests]+ isMainCamera = " + isMainCamera);
        Set<RequestType> keySet = requestBuilders.keySet();
        Iterator<RequestType> iterator = keySet.iterator();
        while (iterator.hasNext()) {
            RequestType requestType = iterator.next();
            configuringPreviewRequests(requestBuilders.get(requestType), isMainCamera);
            switch (requestType) {
            case STILL_CAPTURE:
                configuringCaptureRequests(requestBuilders.get(requestType), isMainCamera);
                break;
            case VIDEO_SNAP_SHOT:
                configuringCaptureRequests(requestBuilders.get(requestType), isMainCamera);
                configreEisValue(requestBuilders.get(requestType));
                break;
            case RECORDING:
                configreEisValue(requestBuilders.get(requestType));
                break;
            default:
                break;
            }
        }
        LogHelper.i(mTag, "[configuringSessionRequests]- ");
    }

    private Size getCameraCaptureSize(String cameraId) {
        ISettingServant settingServant = mSettingCtroller.getSettingServant(cameraId);
        SettingItem captureSizeItem = settingServant.getSettingItem(SettingKeys.KEY_PICTURE_SIZE);
        if (captureSizeItem == null) {
            return null;
        } else {
            String captureSizeString = captureSizeItem.getValue();
            LogHelper.i(mTag, "getCameraCaptureSize camera id:" + cameraId +
                    " captureSizeString:" + captureSizeString);
            if (captureSizeString != null) {
                return Utils.getSize(captureSizeString);
            } else {
                return null;
            }
        }
    }

    private String getAnotherCameraId() {
        String currentCameraId = mSettingServant.getCameraId();
        if (SettingCtrl.BACK_CAMERA.equalsIgnoreCase(currentCameraId)) {
            return SettingCtrl.FRONT_CAMERA;
        }
        return SettingCtrl.BACK_CAMERA;
    }

    private void waitPreviewSurfaceReady() {
        Map<String, Surface> pipPreviewSurface = mPreviewController.getPreviewInputSurfaces();
        mBottomPreviewSurface = pipPreviewSurface.get(IPipStream.BOTTOM_SURFACE_KEY);
        mTopPreviewSurface = pipPreviewSurface.get(IPipStream.TOP_SURFACE_KEY);
    }

    private void configreEisValue(Builder requestBuilder) {
        String eisValue = mSettingServant.getSettingValue(SettingKeys.KEY_VIDEO_EIS);
        LogHelper.i(mTag, "configuringRecordingRequests eisValue = " + eisValue);
        if ("on".equals(eisValue)) {
            requestBuilder.set(CaptureRequest.CONTROL_VIDEO_STABILIZATION_MODE,
                    CaptureRequest.CONTROL_VIDEO_STABILIZATION_MODE_ON);
        } else {
            requestBuilder.set(CaptureRequest.CONTROL_VIDEO_STABILIZATION_MODE,
                    CaptureRequest.CONTROL_VIDEO_STABILIZATION_MODE_OFF);
        }
    }

    private void configuringPreviewRequests(CaptureRequest.Builder requestBuilder,
            boolean bottomCamera) {
        if (bottomCamera) {
            if (mBottomPreviewSurface != null && mBottomPreviewSurface.isValid()) {
                requestBuilder.addTarget(mBottomPreviewSurface);
            }
        } else {
            if (mTopPreviewSurface != null && mTopPreviewSurface.isValid()) {
                requestBuilder.addTarget(mTopPreviewSurface);
            }
        }
    }

    private void configuringCaptureRequests(CaptureRequest.Builder requestBuilder,
            boolean bottomCamera) {
        if (bottomCamera) {
            if (mBottomCaptureSurface != null) {
                requestBuilder.addTarget(mBottomCaptureSurface);
            }
        } else {
            if (mTopCaptureSurface != null) {
                requestBuilder.addTarget(mTopCaptureSurface);
            }
        }
        requestBuilder.set(CaptureRequest.JPEG_QUALITY, JPEG_QUALITY);
    }

    private void takePicture() {
        LogHelper.i(mTag, "photoShutterButtonClicked");
        if (mStorageService.getCaptureStorageSpace() <= 0) {
            LogHelper.w(mTag, "Not enough space or storage not available, " +
                    "remaining:" + mStorageService.getCaptureStorageSpace());
            return;
        }
        mDuringCapture = true;
        mAppUi.setAllCommonViewEnable(false);
        mAppUi.setSwipeEnabled(false);

        updatePictureSize();
        IAaaController frontAaaController =
            mModuleListener.get3AController(SettingCtrl.FRONT_CAMERA);
        frontAaaController.aePreTriggerAndCapture();

        updateCaptureSurfaces(true);
        mModuleListener.requestChangeSessionOutputs(/*sync = */true, /*main cam = */true);
        IAaaController backAaaController =
            mModuleListener.get3AController(SettingCtrl.BACK_CAMERA);
        backAaaController.aePreTriggerAndCapture();
    }

    private void takeVss() {
        mDuringCapture = true;
        mAppUi.setShutterButtonEnabled(false, false/*photo shutter*/);
        mModuleListener.requestChangeCaptureRequets(true,
                false, RequestType.VIDEO_SNAP_SHOT, CaptureType.CAPTURE);
        mModuleListener.requestChangeCaptureRequets(false,
                false, RequestType.VIDEO_SNAP_SHOT, CaptureType.CAPTURE);
    }

    private void videoShutterClicked() {
        LogHelper.i(mTag, "videoShutterButtonClicked");
        if (!mRecording && mStorageService.getRecordStorageSpace() <= 0) {
            LogHelper.w(mTag, "Not enough space or storage not available, " +
                        "remaining:" + mStorageService.getRecordStorageSpace());
            mAppUi.showPickerManagerUi();
            return;
        }
        mAppUi.setShutterButtonEnabled(false, false);
        mAppUi.setShutterButtonEnabled(false, true);
        if (mRecording) {
            stopRecording();
            mAppUi.stopShowCommonUI(false);
            mAppUi.switchShutterButtonImageResource(R.drawable.btn_video,
                    true/**video shutter**/);
            mAppUi.setSwipeEnabled(true);
            mAppUi.showModeOptionsUi();
            mAppUi.showSettingUi();
            mAppUi.showIndicatorManagerUi();
            mAppUi.showPickerManagerUi();
            mAppUi.setThumbnailManagerEnable(true);
            mAppUi.setAllCommonViewEnable(true);
            Size pictureSize = getCaptureSize();
            String pictureFormat = pictureSize.getWidth() + "x" +
                    pictureSize.getHeight() + "-superfine";
            mAppUi.showLeftCounts(Utils.getImageSize(pictureFormat), true);
        } else {
            mCameraServices.getSoundPlayback().play(ISoundPlayback.START_VIDEO_RECORDING);
            startRecording();
            mAppUi.stopShowCommonUI(true);
            mAppUi.switchShutterButtonImageResource(R.drawable.btn_video_mask,
                    true/**video shutter**/);
            mAppUi.setSwipeEnabled(false);
            mAppUi.hideModeOptionsUi();
            mAppUi.hideSettingUi();
            mAppUi.hideIndicatorManagerUi();
            mAppUi.hidePickerManagerUi();
            mAppUi.setThumbnailManagerEnable(false);
            long bytePerMs = ((mCameraCamcorderProfile.videoBitRate +
                    mCameraCamcorderProfile.audioBitRate) >> 3) / 1000;
            mAppUi.showLeftTime(bytePerMs);
        }
    }

    private void startRecording() {
        LogHelper.i(mTag, "[startRecording]+");
        mRecording = true;
        prepareRecording();
        updatePictureSize();
        updateCaptureSurfaces(true/**update main**/);
        updatePreviewSize(new PreviewSurfaceCallback() {
            @Override
            public void onPreviewSufaceIsReady(boolean surfaceChanged) {
                LogHelper.i(mTag, "startRecording onPreviewSufaceIsReady");
                mModuleListener.requestChangeSessionOutputs(true);
                mSettingCtroller.doSettingChange(SettingKeys.KEY_VIDEO_PIP, "on", false);
                mModuleListener.requestChangeCaptureRequets(true/*sync*/,
                        RequestType.RECORDING, CaptureType.REPEATING_REQUEST);
                mRecordController.startRecord();
                mAppController.enableKeepScreenOn(true);
                mHandler.postDelayed(mEnableShutterButtonRunnable,
                        SHUTTER_BUTTON_TIMEOUT);
            }
        });
    }

    private void stopRecording() {
        LogHelper.i(mTag, "[stopRecording]+");
        mRecording = false;
        if (mIsPaused) {
            doStopRecording();
            return;
        }
        updatePictureSize();
        updatePreviewSize(new PreviewSurfaceCallback() {
            @Override
            public void onPreviewSufaceIsReady(boolean surfaceChanged) {
                doStopRecording();
            }
        });
    }

    private Runnable mEnableShutterButtonRunnable = new Runnable() {
        @Override
        public void run() {
            mAppUi.setShutterButtonEnabled(true, false);
            mAppUi.setShutterButtonEnabled(true, true);
        }
    };

    private void doStopRecording() {
        LogHelper.i(mTag, "[doStopRecording]+");
        mBottomCaptureSurface = null;
        mTopCaptureSurface = null;
        try {
            mRecordController.stopRecord(true);
            mAppUi.showSavingProgress(
                    mAppController.getActivity().getResources().getString(R.string.saving));
        } catch (RuntimeException e) {
            e.printStackTrace();
            LogHelper.e(mTag, "stopRecording with exception:" + e);
            mVideoTempPath = null;
            mCameraCamcorderProfile = null;
        } finally {
            mModuleListener.requestChangeSessionOutputs(true);
            mModuleListener.requestChangeCaptureRequets(true/*sync*/,
                    RequestType.PREVIEW, CaptureType.REPEATING_REQUEST);
            mCameraServices.getSoundPlayback().play(ISoundPlayback.STOP_VIDEO_RECORDING);
            mAppController.enableKeepScreenOn(false);
            mAppUi.setShutterButtonEnabled(true, false);
            mAppUi.setShutterButtonEnabled(true, true);
            LogHelper.i(mTag, "[doStopRecording]-");
        }
    }

    private void prepareRecording() {
        int cameraId = Integer.valueOf(mSettingServant.getCameraId());
        int videoQualityValue = mVideoHelper.getRecordingQuality(cameraId);
        mCameraCamcorderProfile = mVideoHelper.fetchProfile(videoQualityValue,
                cameraId);
        String mirc = mSettingServant.getSettingValue(SettingKeys.KEY_VIDEO_RECORD_AUDIO);
        boolean enableAudio = "on".equals(mirc);
        mRecordingRotation = Utils.getRecordingRotation(mCurrentOrientation,
                Utils.getCameraCharacteristics(
                        mAppController.getActivity(), mSettingServant.getCameraId()));
        mVideoTempPath = mVideoHelper.generateVideoFileName(
                mCameraCamcorderProfile.fileFormat, null);
        LogHelper.i(mTag, "prepareRecording enableAudio = " + enableAudio);
        prepareMediaRecordingParamters();
        mRecordController.setRecordingProfile(mCameraCamcorderProfile);
        mRecordController.enalbeAudioRecording(enableAudio);
        mRecordController.setOutputFile(mVideoTempPath);
        mRecordController.setOrientationHint(mRecordingRotation);
        mRecordController.setAudioSource(MediaRecorder.AudioSource.CAMCORDER);
        mRecordController.setVideoSource(MediaRecorder.VideoSource.SURFACE);
        mRecordController.prepareRecord();

        mRecordController.getRecordInputSurface();
    }

    private void updateCaptureSurfaces(boolean updateMain) {
        Map<String, Surface> pipCaptureSurface = mCaptureController.getCaptureInputSurface();
        if (updateMain) {
            mBottomCaptureSurface = pipCaptureSurface.get(IPipStream.BOTTOM_SURFACE_KEY);
        } else {
            mTopCaptureSurface = pipCaptureSurface.get(IPipStream.TOP_SURFACE_KEY);
        }
    }

    /**
     * when file have saved ,need notify Manager
     */
    private OnFileSavedListener mMediaSavedListener = new OnFileSavedListener() {
        @Override
        public void onMediaSaved(Uri uri) {
            LogHelper.i(mTag, "onMediaSaved uri = " + uri);
            // only video saving need to dismiss saving diaLogHelper.
            mAppUi.dismissSavingProgress();
            mAppController.notifyNewMedia(uri);
        }
    };

    private void updateCaptureContentValues(int width, int height, int orientation) {
        mCapContentValues = new ContentValues();
        long dateTaken = System.currentTimeMillis();
        String title = Utils.createJpegName(dateTaken);

        String filename = title + ".jpg";
        String mime = "image/jpeg";
        String path = mStorageService.getFileDirectory() + '/' + filename;
        String tmpPath = path + ".tmp";

        mCapContentValues.put(ImageColumns.DATE_TAKEN, dateTaken);
        mCapContentValues.put(ImageColumns.TITLE, title);
        mCapContentValues.put(ImageColumns.DISPLAY_NAME, filename);
        mCapContentValues.put(ImageColumns.DATA, path);
        mCapContentValues.put(ImageColumns.MIME_TYPE, mime);

        mCapContentValues.put(ImageColumns.WIDTH, width);
        mCapContentValues.put(ImageColumns.HEIGHT, height);
        mCapContentValues.put(ImageColumns.ORIENTATION, orientation);

        mLocation = mLocationManager.getCurrentLocation();
        if (mLocation != null) {
            mCapContentValues.put(ImageColumns.LATITUDE, mLocation.getLatitude());
            mCapContentValues.put(ImageColumns.LONGITUDE, mLocation.getLongitude());
        }
        LogHelper.i(mTag, "updateCaptureContentValues orientation: " + orientation);
    }

    private void updateVideoContentValues() {
        long dateTaken = System.currentTimeMillis();
        String title = mVideoHelper.createFileTitle(dateTaken, mAppController);
        String mime = mVideoHelper
                .convertOutputFormatToMimeType(mCameraCamcorderProfile.fileFormat);
        String filename = title
                + mVideoHelper
                        .convertOutputFormatToFileExt(mCameraCamcorderProfile.fileFormat);
        String path = mStorageService.getFileDirectory() + '/' + filename;
        long duration = mVideoHelper.getDuration(mVideoTempPath);

        mVideoContentValues = new ContentValues();
        mVideoContentValues.put(Video.Media.DURATION, duration);
        mVideoContentValues.put(Video.Media.TITLE, title);
        mVideoContentValues.put(Video.Media.DISPLAY_NAME, filename);
        mVideoContentValues.put(Video.Media.DATE_TAKEN, dateTaken);
        mVideoContentValues.put(MediaColumns.DATE_MODIFIED, dateTaken / 1000);
        mVideoContentValues.put(Video.Media.MIME_TYPE, mime);
        mVideoContentValues.put(Video.Media.DATA, path);
        mVideoContentValues.put(Video.Media.WIDTH, mCameraCamcorderProfile.videoFrameWidth);
        mVideoContentValues.put(Video.Media.HEIGHT, mCameraCamcorderProfile.videoFrameHeight);
        mVideoContentValues.put(Video.Media.ORIENTATION, mRecordingRotation);
        mVideoContentValues.put(Video.Media.RESOLUTION,
                Integer.toString(mCameraCamcorderProfile.videoFrameWidth) + "x"
                + Integer.toString(mCameraCamcorderProfile.videoFrameHeight));
        mVideoContentValues.put(Video.Media.SIZE, new File(mVideoTempPath).length());

        mLocation = mLocationManager.getCurrentLocation();
        if (mLocation != null) {
            mVideoContentValues.put(ImageColumns.LATITUDE,
                    mLocation.getLatitude());
            mVideoContentValues.put(ImageColumns.LONGITUDE,
                    mLocation.getLongitude());
        }
    }

    private ModeGestureListener mModeGestureListener = new ModeGestureListener() {
        @Override
        public boolean onUp() {
            if (mDuringCapture) {
                return true;
            }
            if (mPipStreamController != null) {
                return mPipStreamController.onUp();
            }
            return false;
        }

        @Override
        public boolean onSingleTapUp(float x, float y) {
            if (mDuringCapture) {
                return true;
            }
            if (mPipStreamController != null && mPipViewController != null
                    && !mPipViewController.onTouchEvent()) {
                return mPipStreamController.onSingleTapUp(x, y);
            }
            return false;
        }

        @Override
        public boolean onScroll(float dx, float dy, float totalX, float totalY) {
            if (mDuringCapture) {
                return true;
            }
            if (mPipStreamController != null) {
                return mPipStreamController.onScroll(dx, dy, totalX, totalY);
            }
            return false;
        }

        @Override
        public boolean onLongPress(float x, float y) {
            if (mDuringCapture) {
                return true;
            }
            if (mPipStreamController != null) {
                return mPipStreamController.onLongPress(x, y);
            }
            return false;
        }

        @Override
        public boolean onDown(float x, float y) {
            if (mDuringCapture) {
                return true;
            }
            if (mPipStreamController != null) {
                return mPipStreamController.onDown(x, y);
            }
            return false;
        }
    };

    private class PipViewCallbackImpl implements PipViewCallback {
        @Override
        public void onTemplateChanged(int rearResId, int frontResId,
                int highlightResId, int editBtnResId) {
            mPipStreamController.onTemplateChanged(rearResId,
                    frontResId, highlightResId, editBtnResId);
        }
    }
    private class ModePipStreamCallback implements PipStreamCallback {
        private final Handler mHandler;
        public ModePipStreamCallback(Handler handler) {
            mHandler = handler;
        }

        @Override
        public void onOpened() {

        }

        @Override
        public void onClosed() {

        }

        @Override
        public void onPaused() {

        }

        @Override
        public void onResumed() {

        }

        @Override
        public void onTopGraphicTouched() {

        }

        @Override
        public void onSwitchPipEventReceived() {
            mHandler.removeCallbacks(mCamPickerBtnClieckRunnale);
            mHandler.post(mCamPickerBtnClieckRunnale);
        }

        private Runnable mCamPickerBtnClieckRunnale = new Runnable() {

            @Override
            public void run() {
                mAppUi.performCameraPickerBtnClick();
            }

        };
    }

    // pip max preview size is 1920x1088
    private Size getPreviewSize(ISettingServant settingServant) {
        List<Size> supportedSizes = settingServant.getSupportedPreviewSizes();
        String pictureRatio = settingServant.getSettingValue(SettingKeys.KEY_PICTURE_RATIO);
        List<Size> filteredSupportedSize =
                Utils.filterSizesByBound(supportedSizes, new Size(1280, 720));
        return Utils.getOptimalPreviewSize(mActivity,
                filteredSupportedSize, Double.parseDouble(pictureRatio));
    }

    private void prepareMediaRecordingParamters() {
        List<String> recordParamters = new ArrayList<String>();
        recordParamters.add(IRecordStream.RECORDER_INFO_SUFFIX
                + IRecordStream.MEDIA_RECORDER_INFO_BITRATE_ADJUSTED);
        recordParamters.add(IRecordStream.RECORDER_INFO_SUFFIX
                + IRecordStream.MEDIA_RECORDER_INFO_FPS_ADJUSTED);
        recordParamters.add(IRecordStream.RECORDER_INFO_SUFFIX
                + IRecordStream.MEDIA_RECORDER_INFO_START_TIMER);
        recordParamters.add(IRecordStream.RECORDER_INFO_SUFFIX
                + IRecordStream.MEDIA_RECORDER_INFO_WRITE_SLOW);
        recordParamters.add(IRecordStream.RECORDER_INFO_SUFFIX
                + IRecordStream.MEDIA_RECORDER_INFO_CAMERA_RELEASE);
        if (mIsCaptureIntent) {
            recordParamters.add(IRecordStream.RECORDER_INFO_SUFFIX
                    + IRecordStream.MEDIA_RECORDER_INFO_RECORDING_SIZE);
        }
        mRecordController.setMediaRecorderParameters(recordParamters);
    }
}
