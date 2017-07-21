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
import android.hardware.Camera.CameraInfo;
import android.view.Surface;

import java.util.List;

/**
 * this class is service for multiCameraMoudle.java.just is a util class.
 */
@SuppressWarnings("deprecation")
class MultiCameraModuleUtil {
    static boolean isLandScape(Activity activity) {
        int rotation = activity.getWindowManager().getDefaultDisplay().getRotation();
        switch (rotation) {
        case Surface.ROTATION_0:
        case Surface.ROTATION_180:
            return false;
        case Surface.ROTATION_90:
        case Surface.ROTATION_270:
            return true;

        default:
            return false;
        }
    }

    static boolean hasLocalCamera(List<String> cameraId) {
        boolean hasLocalCamera = false;
        for (String id : cameraId) {
            if (String.valueOf(CameraInfo.CAMERA_FACING_BACK).equals(id)
                    || String.valueOf(CameraInfo.CAMERA_FACING_FRONT).equals(id)) {
                hasLocalCamera = true;
                break;
            }
        }
        return hasLocalCamera;
    }

    static boolean hasRemoteCamera(List<String> cameraId) {
        boolean hasRemoteCamera = false;
        if (cameraId != null) {
            for (String id : cameraId) {
                if (!String.valueOf(CameraInfo.CAMERA_FACING_BACK).equals(id)
                        && !String.valueOf(CameraInfo.CAMERA_FACING_FRONT).equals(id)) {
                    hasRemoteCamera = true;
                    break;
                }
            }
        }
        return hasRemoteCamera;
    }

    static boolean checkCameraId(String cameraId, String[] allCameraId) {
        boolean isCorrect = false;
        if (allCameraId != null) {
            for (int index = 0; index < allCameraId.length; index++) {
                if (cameraId.equals(allCameraId[index])) {
                    isCorrect = true;
                    break;
                }
            }
        }
        return isCorrect;
    }

    static boolean canShowVideoButton(List<String> currentPreviewCamera) {
        boolean showVideoButton = true;
        for (String cameraId : currentPreviewCamera) {
            if (!cameraId.equals(String.valueOf(CameraInfo.CAMERA_FACING_BACK))
                    && !cameraId.equals(String.valueOf(CameraInfo.CAMERA_FACING_FRONT))) {
                // means the preview have local camera preview and
                // remote camera so the shutter button just have photo
                // ,will hide the video button
                showVideoButton = false;
                break;
            }
        }
        return showVideoButton;
    }

    static boolean isLocalCamera(String cameraId) {
        boolean localCamera = false;
        if (String.valueOf(CameraInfo.CAMERA_FACING_BACK).equals(cameraId)
                || String.valueOf(CameraInfo.CAMERA_FACING_FRONT).equals(cameraId)) {
            localCamera = true;
        }
        return localCamera;
    }
}
