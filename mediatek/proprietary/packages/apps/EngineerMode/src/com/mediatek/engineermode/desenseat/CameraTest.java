/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */
/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.mediatek.engineermode.desenseat;

import android.app.Activity;
import android.hardware.Camera;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.Size;
import android.media.CameraProfile;
import android.os.Bundle;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import com.mediatek.engineermode.Elog;

import java.io.IOException;
import java.util.Collections;
import java.util.List;

/**
 * The Camera activity which can preview.
 */
public class CameraTest extends Activity implements SurfaceHolder.Callback {
    static final String TAG = "EM/DesenseAT";
    private static final int PREVIEW_STOPPED = 0;
    private static final int IDLE = 1;
    private SurfaceHolder mSurfaceHolder = null;
    private Camera mCameraDevice;
    private int mCameraId;
    private int mCameraState = PREVIEW_STOPPED;
    private boolean mPausing;

    @Override
    public void onCreate(Bundle icicle) {
        super.onCreate(icicle);
        Elog.i(TAG, "[onCreate]");
        mCameraId = getIntent().getIntExtra("facing", 0);
        SurfaceView surfaceView = new SurfaceView(this);
        setContentView(surfaceView);
        SurfaceHolder holder = surfaceView.getHolder();
        holder.addCallback(this);
        holder.setType(SurfaceHolder.SURFACE_TYPE_PUSH_BUFFERS);
        Elog.i(TAG, "[onCreate] Finished");
    }

    @Override
    protected void onResume() {
        super.onResume();
        Elog.i(TAG, "onResume()");
        mPausing = false;
        mCameraDevice = openCamera(mCameraId);
        if (mCameraDevice != null) {
            startPreview();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        Elog.i(TAG, "onPause()");
        mPausing = true;
        stopPreview();
        closeCamera();
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Elog.v(TAG, "surfaceCreated.");
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        if (holder == null || holder.getSurface() == null || mCameraDevice == null
                || mPausing || isFinishing()) {
            Elog.d(TAG, "holder.getSurface() == null");
            return;
        }
        mSurfaceHolder = holder;
        if (mCameraState == PREVIEW_STOPPED) {
            startPreview();
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Elog.v(TAG, "surfaceDestroyed()");
        stopPreview();
        mSurfaceHolder = null;
    }

    Camera openCamera(int cameraId) {
        for (int i = 0; i < 10; i++) {
            Camera camera = Camera.open(cameraId);
            if (camera == null) {
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            } else {
                return camera;
            }
        }
        return null;
    }

    private void closeCamera() {
        if (mCameraDevice != null) {
            mCameraDevice.release();
            mCameraDevice = null;
            setCameraState(PREVIEW_STOPPED);
        }
    }

    private void startPreview() {
        Elog.d(TAG, "set Preview");
        if (mSurfaceHolder == null || mPausing || isFinishing()) {
            Elog.d(TAG, "startPreview() return");
            return;
        }
        if (mCameraState != PREVIEW_STOPPED) {
            stopPreview();
        }
        setPreviewDisplay(mSurfaceHolder);
        setDisplayOrientation();
        setCameraParameters();
        Elog.v(TAG, "==== startPreview ====");
        mCameraDevice.startPreview();
        setCameraState(IDLE);
    }

    private void stopPreview() {
        if (mCameraDevice != null && mCameraState != PREVIEW_STOPPED) {
            Elog.v(TAG, "stopPreview");
            mCameraDevice.stopPreview();
        }
        setCameraState(PREVIEW_STOPPED);
    }

    private void setPreviewDisplay(SurfaceHolder holder) {
        try {
            mCameraDevice.setPreviewDisplay(holder);
        } catch (IOException ex) {
            closeCamera();
        }
    }

    private void setDisplayOrientation() {
        int displayRotation = getDisplayRotation();
        int displayOrientation = getDisplayOrientation(displayRotation, mCameraId);
        mCameraDevice.setDisplayOrientation(displayOrientation);
    }

    private int getDisplayRotation() {
        int rotation = getWindowManager().getDefaultDisplay().getRotation();
        switch (rotation) {
            case Surface.ROTATION_0:
                return 0;
            case Surface.ROTATION_90:
                return 90;
            case Surface.ROTATION_180:
                return 180;
            case Surface.ROTATION_270:
                return 270;
            default:
                break;
        }
        return 0;
    }

    private int getDisplayOrientation(int degrees, int cameraId) {
        // See android.hardware.Camera.setDisplayOrientation for
        // documentation.
        Camera.CameraInfo info = new Camera.CameraInfo();
        Camera.getCameraInfo(cameraId, info);
        int result;
        if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
            result = (info.orientation + degrees) % 360;
            result = (360 - result) % 360; // compensate the mirror
        } else { // back-facing
            result = (info.orientation - degrees + 360) % 360;
        }
        return result;
    }

    private void setCameraState(int state) {
        Elog.d(TAG, "setCameraState() state " + state);
        mCameraState = state;
    }

    private void setCameraParameters() {
        Parameters parameters = mCameraDevice.getParameters();

        List<Integer> frameRates = parameters.getSupportedPreviewFrameRates();
        if (frameRates != null) {
            Integer max = Collections.max(frameRates);
//            if (max > 30) max = 30;
            Elog.v(TAG, "Max frame rate is " + max);
            parameters.setPreviewFrameRate(max);
        }

        List<Size> pictureSizes = parameters.getSupportedPictureSizes();
        Size maxSize = pictureSizes.get(0);
        for (Size size : pictureSizes) {
            if ((size.height > maxSize.height) || (size.width > maxSize.width)) {
                maxSize = size;
            }
        }
        Elog.v(TAG, "Max picture size is " + maxSize.width + "x" + maxSize.height);
        parameters.setPictureSize(maxSize.width, maxSize.height);

        List<Size> previewSizes = parameters.getSupportedPreviewSizes();
        maxSize = previewSizes.get(0);
        for (Size size : previewSizes) {
            if ((size.height > maxSize.height) || (size.width > maxSize.width)) {
                maxSize = size;
            }
        }
        Elog.v(TAG, "Max preview size is " + maxSize.width + "x" + maxSize.height);
        parameters.setPreviewSize(maxSize.width, maxSize.height);

        int jpegQuality = CameraProfile.getJpegEncodingQualityParameter(mCameraId,
                CameraProfile.QUALITY_HIGH);
        parameters.setJpegQuality(jpegQuality);
        mCameraDevice.setParameters(parameters);
    }
}
