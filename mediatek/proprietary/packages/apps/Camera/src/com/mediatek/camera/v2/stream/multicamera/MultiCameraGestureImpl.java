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
package com.mediatek.camera.v2.stream.multicamera;

import android.app.Activity;
import android.hardware.Camera.CameraInfo;
import android.util.Log;
import android.util.Size;

import com.mediatek.camera.v2.stream.multicamera.renderer.AnimationRect;

import junit.framework.Assert;

import java.util.ArrayList;
import java.util.List;

/**
 * Calculate Multi Camera preview area.
 */
public class MultiCameraGestureImpl implements ISinkGestureCtrl {

    private static final String TAG = MultiCameraGestureImpl.class.getSimpleName();

    private int mPreviewWidth = 0;
    private int mPreviewHeight = 0;

    private List<String> mCurrentPreviewCamera = new ArrayList<>();

    // top rectangle attributes records top graphic rectangle's position
    // the location 0 will put the local preview rect.1/2/3...will put the
    // remote camera preview location
    private final AnimationRect mRemotePreviewGraphicRect;
    private final AnimationRect mLocalPreviewGrapichRect;

    private Size mPreviewSize;

    /**
     * instance a multi camera gesture.
     * @param activity current activity
     */
    public MultiCameraGestureImpl(Activity activity) {
        Log.i(TAG, "[MultiCameraGestureImpl]");
        Assert.assertNotNull(activity);
        mRemotePreviewGraphicRect = new AnimationRect();
        mLocalPreviewGrapichRect = new AnimationRect();
    }

    @Override
    public void updatePreviewCamera(List<String> cameraID) {
        Log.i(TAG, "[updatePreviewRect] cameraID = " + cameraID);
        mCurrentPreviewCamera = cameraID;
        updatePreviewRect();
    }

    @Override
    public void setPreviewSize(Size previewSize) {
        Assert.assertNotNull(previewSize);
        mPreviewSize = previewSize;
        mPreviewWidth = mPreviewSize.getWidth();
        mPreviewHeight = mPreviewSize.getHeight();
        Log.i(TAG, "[setPreviewSize]+ : " + mPreviewWidth + " x " + mPreviewHeight);
    }

    @Override
    public AnimationRect getTopGraphicRect(int gsensorOrientation) {
        return mRemotePreviewGraphicRect;
    }

    @Override
    public AnimationRect getBottomGraphicRect(int gsensorOrientation) {
        return mLocalPreviewGrapichRect;
    }

    private void updatePreviewRect() {
        if (mPreviewSize != null) {
            int width = Math.min(mPreviewSize.getWidth(), mPreviewSize.getHeight());
            int height = Math.max(mPreviewSize.getWidth(), mPreviewSize.getHeight());
            mRemotePreviewGraphicRect.setRendererSize(width, height);
            mLocalPreviewGrapichRect.setRendererSize(width, height);

            if (mCurrentPreviewCamera != null) {
                int previewCameraNumber = mCurrentPreviewCamera.size();
                switch (previewCameraNumber) {
                case 1:
                    if (CameraInfo.CAMERA_FACING_BACK == Integer.parseInt(
                            mCurrentPreviewCamera.get(0))
                            || CameraInfo.CAMERA_FACING_FRONT == Integer.parseInt(
                                    mCurrentPreviewCamera.get(0))) {
                        // just show the main preview
                        toSinglePreview(true);
                    } else {
                        toSinglePreview(false);
                    }
                    break;

                case 2:
                    mRemotePreviewGraphicRect.initialize(0, /** left **/
                    0, /** top **/
                    width, /** right **/
                    height / 2 /** bottom **/
                    );

                    mLocalPreviewGrapichRect.initialize(0, /** left **/
                    height / 2, /** top **/
                    width, /** right **/
                    height /** bottom **/
                    );
                    break;

                default:
                    break;
                }
            }
        }
    }

    private void toSinglePreview(boolean isLocal) {
        int width = Math.min(mPreviewSize.getWidth(), mPreviewSize.getHeight());
        int height = Math.max(mPreviewSize.getWidth(), mPreviewSize.getHeight());

        Log.i(TAG, "[toSinglePreview] islocal = " + isLocal + ",width = " + width + ",height = "
                + height);

        if (isLocal) {
            mRemotePreviewGraphicRect.initialize(0, /** left **/
            0, /** top **/
            0, /** right **/
            0 /** bottom **/
            );

            mLocalPreviewGrapichRect.initialize(0, /** left **/
            0, /** top **/
            width, /** right **/
            height /** bottom **/
            );
        } else {
            mRemotePreviewGraphicRect.initialize(0, /** left **/
            0, /** top **/
            width, /** right **/
            height /** bottom **/
            );

            mLocalPreviewGrapichRect.initialize(0, /** left **/
            0, /** top **/
            0, /** right **/
            0 /** bottom **/
            );
        }
    }
}
