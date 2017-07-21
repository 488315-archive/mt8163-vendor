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
import android.view.MotionEvent;
import android.view.ViewGroup;

import com.mediatek.camera.v2.stream.IPreviewStream.PreviewCallback;
import com.mediatek.camera.v2.stream.multicamera.ui.IMultiCameraViewManager;
import com.mediatek.camera.v2.stream.multicamera.ui.IMultiCameraViewManager.IMultiCameraViewListener;
import com.mediatek.camera.v2.stream.multicamera.ui.MultiCameraViewManager;

import java.util.List;

/**
 * A module UI which the level same as module.current all Multi camera ui is
 * module ui.
 */
public class MultiCameraModuleUi extends AbstractModuleUi {

    private ViewGroup mParentView;
    private IMultiCameraViewManager mMultiCameraViewManager;

    private GestureListener mMultiCameraGestureListener = new MultiCameraGestureImpl();
    private AbstractCameraModule mModule;

    /**
     * Create a Multi camera module instance.
     * @param activity
     *            current activity.
     * @param module
     *            multi camera moudel's parent module.
     * @param parentView
     *            the view root will module view will be add.
     * @param callback
     *            when the preview surface is ready or changed and destroyed
     *            will be notified.
     * @param listener
     *            which the interface of Multi View.
     */
    public MultiCameraModuleUi(
            Activity activity, AbstractCameraModule module, ViewGroup parentView,
            PreviewCallback callback, IMultiCameraViewListener listener) {
        super(activity, module, parentView, callback);
        mMultiCameraViewManager = new MultiCameraViewManager(listener);
        mParentView = parentView;
        mModule = module;
    }

    @Override
    public GestureListener getGestureListener() {
        return mMultiCameraGestureListener;
    }

    @Override
    public void onLayoutOrientationChanged(boolean isLandscape) {
        mMultiCameraViewManager.onLayoutOrientationChanged(isLandscape);
    }

    // Should notify the orientation to View
    @Override
    public void onOrientationChanged(int gsensorOrientation) {
        mMultiCameraViewManager.onOrientationChanged(gsensorOrientation);
    }


    @Override
    public void updatePreviewCamera(List<String> cameraId, boolean isRemoteOpened) {
        super.updatePreviewCamera(cameraId, isRemoteOpened);
        mMultiCameraViewManager.updatePreviewCamera(cameraId, isRemoteOpened);
    }

    @Override
    public void updateRecordingStatus(boolean isStarted) {
        super.updateRecordingStatus(isStarted);
        mMultiCameraViewManager.updateRecordingStatus(isStarted);
    }

    @Override
    public void updateCaptureStatus(boolean isCapturing) {
        super.updateCaptureStatus(isCapturing);
        mMultiCameraViewManager.updateCapturingStatus(isCapturing);
    }

    @Override
    public void open(Activity activity, boolean isSecureCamera, boolean isCaptureIntent) {
        super.open(activity, isSecureCamera, isCaptureIntent);
        mMultiCameraViewManager.open(activity, mParentView);
    }

    @Override
    public void resume() {
        super.resume();
        mMultiCameraViewManager.resume();
    }

    @Override
    public void pause() {
        super.pause();
        mMultiCameraViewManager.pause();
    }

    @Override
    public void close() {
        super.close();
        mMultiCameraViewManager.close();
    }

/**
 * A gesture implement such as ondown,onUp.
 */
    private class MultiCameraGestureImpl implements GestureListener {

        @Override
        public boolean onDown(float x, float y) {
            return false;
        }

        @Override
        public boolean onUp() {
            // TODO Auto-generated method stub
            return false;
        }

        @Override
        public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
            // TODO Auto-generated method stub
            return false;
        }

        @Override
        public boolean onScroll(float dx, float dy, float totalX, float totalY) {
            // TODO Auto-generated method stub
            return false;
        }

        @Override
        public boolean onSingleTapUp(float x, float y) {
            mModule.onSingleTapUp(x, y);
            return mMultiCameraViewManager.onSingleTapUp(x, y);
        }

        @Override
        public boolean onSingleTapConfirmed(float x, float y) {
            // TODO Auto-generated method stub
            return false;
        }

        @Override
        public boolean onDoubleTap(float x, float y) {
            // TODO Auto-generated method stub
            return false;
        }

        @Override
        public boolean onScale(float focusX, float focusY, float scale) {
            // TODO Auto-generated method stub
            return false;
        }

        @Override
        public boolean onScaleBegin(float focusX, float focusY) {
            // TODO Auto-generated method stub
            return false;
        }

        @Override
        public boolean onLongPress(float x, float y) {
            // TODO Auto-generated method stub
            return false;
        }

    }
}
