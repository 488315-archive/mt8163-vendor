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
import android.view.Surface;
import android.view.ViewGroup;

import java.util.List;

/**
 * A sink stream controller.
 */
public interface ISinkStreamCtrl {

    /**
     * open the sink stream controller,can be called at the activity create.
     * @param activity
     *            current activity.
     * @param parentView
     *            which view can be add.
     */
    public void open(Activity activity, ViewGroup parentView);

    /**
     * When activity is resumed,can notify sink stream controller to resume.
     */
    public void resume();

    /**
     * When activity is paused,can notify sink stream controller to pause.
     */
    public void pause();

    /**
     * When activity is closed,can notify sink stream controller to close.
     */
    public void close();

    // app resources
    /**
     * Notify the new G-sensor's orientation to stream.
     * @param gsensorOrientation
     *            current new orientation.
     */
    public void onOrientationChanged(int gsensorOrientation);

    /**
     * A callback will be notified that current stream status.
     */
    public interface SinkStreamCallback {

        /**
         * Current stream controller is opened.
         */
        public void onOpened();

        /**
         * Current stream controller is closed.
         */
        public void onClosed();

        /**
         * Current stream controller is paused.
         */
        public void onPaused();

        /**
         * Current stream controller is resumed.
         */
        public void onResumed();

        /**
         * User have touched the top camera.
         */
        public void onTopGraphicTouched();

        /**
         * Current is switch camera preview.
         */
        public void onSwitchPreview();
    }

    /**
     * Notify Stream update the preview camera when preview camera have changed.
     * @param previewCameraId
     *            Current preview camera.
     * @param hasOpenedCamera
     *            Current have opened camera.
     */
    public void updatePreviewCamera(List<String> previewCameraId, List<String> hasOpenedCamera);

    /**
     * set a surface for a snap shot,such as current is in multi-preview,if take
     * picture, just use the preview for take picture.
     * @param snapShotSurface
     *            Which surface will be used for capture.
     */
    public void setSnapShotSurface(Surface snapShotSurface);

    /**
     * Tell stream current need to take a picture.
     */
    public void doSnapShot();
}
