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
import android.view.ViewGroup;

import java.util.List;

/**
 * Define the multi camera view common interface.
 */
public interface IMultiCameraView extends
        IMultiCameraViewManager.IStatusUpdateListener {
    /**
     * Life cycle of the view manager, when enter to the multi camera mode,
     * open() will be called to prepare the views.
     * @param activity  Activity instance.
     * @param parentView Parent view group.
     */
    public void open(Activity activity, ViewGroup parentView);
    /**
     * Life cycle of the view manager, when activity execute onResume(),
     * resume() will be called.
     */
    public void resume();
    /**
     * Life cycle of the view manager, when activity execute onPause(),
     * pause() will be called.
     */
    public void pause();
    /**
     * Life cycle of the view manager, when activity execute onDestory(),
     * close() will be called.
     */
    public void close();

    /**
     * Show the view.
     */
    public void show();

    /**
     * Hide the view.
     */
    public void hide();
    /**
     * May be useless.
     * @param x May be useless.
     * @param y May be useless.
     * @return May be useless.
     */
    public boolean onSingleTapUp(float x, float y);

    /**
     * Notify the new display rotation value.
     * @param displayRotation New rotation value.
     */
    public void onDisplayChanged(int displayRotation);
    /**
     * When the rotate the phone, notify the phone gsensor orientation value to views
     * which will rotate themselves by the orientation.
     * @param gsensorOrientation The new orientation.
     */
    public void onOrientationChanged(int gsensorOrientation);
    /**
     * When enter camera APP, it will lock the activity orientation(portrait or landscape).
     * When the lock orientation changed, should notify the new orientation to the views
     * which will reinflate layout xml.
     * @param isLandscape new lock orientation.
     */
    public void onLayoutOrientationChanged(boolean isLandscape);
    /**
     * Update preview camera list.
     * @param cameraId Current preview camera id list.
     * @param isRemoteOpened  Is there opened remote camera or not.
     */
    public void updatePreviewCamera(List<String> cameraId,
            boolean isRemoteOpened);
    /**
     * Update Capture status.
     * @param isCapturing capture start or not.
     */
    public void updateCapturingStatus(boolean isCapturing);
}