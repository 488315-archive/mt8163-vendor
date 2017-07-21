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
import android.graphics.drawable.GradientDrawable;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;

import com.android.camera.R;

import com.mediatek.camera.v2.platform.device.IRemoteDevice;
import com.mediatek.camera.v2.util.Utils;

import java.util.LinkedHashMap;
import java.util.List;

/**
 * This class used for maintain the view of add/delete/back to multiview.
 */
public class OperateView implements IMultiCameraView {
    private static final float DISABLED_ALPHA = 0.4f;
    private static final float ENABLE_ALPAHA = 1.0f;

    /**
     * Operate view listener.
     */
    public interface IOperateViewListener {
        /**
         * When the disconnect button is clicked, invoke it to notify the event.
         */
        public void onDisconnectButtonClick();
        /**
         * When the scan button is clicked, invoke it to notify the event.
         */
        public void onScanButtonClick();
        /**
         * When the multi preview button is clicked, invoke it to notify the event.
         */
        public void onMultiPreviewButtonClick();
    }

    private static final String TAG = "OperateView";
    private Activity mActivity;
    private ViewGroup mParentView;
    private ViewGroup mOperatorContainer;

    private ImageView mDisconnectIcon;
    private ImageView mScanIcon;
    private ImageView mMultiPreivewIcon;

    private GradientDrawable mBackground;

    private IOperateViewListener mOperateViewListener;

    private List<String> mPreviewingCameraId;
    private boolean mRemoteCameraOpened;
    private ViewGroup mDialogContainer;

    /**
     * Operate view constructor.
     * @param operateViewListener The operate view listener instance.
     */
    public OperateView(IOperateViewListener operateViewListener) {
        mOperateViewListener = operateViewListener;
    }

    @Override
    public void open(Activity activity, ViewGroup parentView) {
        mActivity = activity;
        mParentView = parentView;
        getView();
        hide();
    }

    @Override
    public void show() {
        mOperatorContainer.setVisibility(View.VISIBLE);
    }

    @Override
    public void hide() {
        mOperatorContainer.setVisibility(View.GONE);
    }

    @Override
    public void onOrientationChanged(int gsensorOrientation) {
        Utils.setRotatableOrientation(mOperatorContainer, gsensorOrientation,
                true);
        Utils.setRotatableOrientation(mDialogContainer, gsensorOrientation,
                true);
    }

    @Override
    public void resume() {
        // TODO Auto-generated method stub
    }

    @Override
    public void pause() {
        // TODO Auto-generated method stub
    }

    @Override
    public void close() {
        // TODO Auto-generated method stub
    }

    @Override
    public boolean onSingleTapUp(float x, float y) {
        // TODO Auto-generated method stub
        return false;
    }

    @Override
    public void onDisplayChanged(int displayRotation) {
        // TODO Auto-generated method stub
    }

    @Override
    public void onLayoutOrientationChanged(boolean isLandscape) {
        Log.i(TAG, "onLayoutOrientationChanged  isLandscape" + isLandscape);
        reInflate();
        updatePreviewCamera(mPreviewingCameraId, mRemoteCameraOpened);
    }

    @Override
    public void updatePreviewCamera(List<String> cameraId,
            boolean isRemoteOpened) {
        Log.i(TAG, "[updatePreviewCamera] list size = " + cameraId.size()
                + ",RemoteOpened = " + isRemoteOpened);
        mPreviewingCameraId = cameraId;
        mRemoteCameraOpened = isRemoteOpened;

        if (mPreviewingCameraId == null) {
            return;
        }

        if (isRemoteOpened) {
            mDisconnectIcon.setVisibility(View.VISIBLE);
        } else {
            mDisconnectIcon.setVisibility(View.GONE);
        }

        if (isRemoteOpened && mPreviewingCameraId.size() == 1) {
            mMultiPreivewIcon.setVisibility(View.VISIBLE);
        } else {
            mMultiPreivewIcon.setVisibility(View.GONE);
        }
    }

    @Override
    public void updateCapturingStatus(boolean isCapturing) {
        Log.i(TAG, "[updateCapturingStatus] isCapturing = " + isCapturing);
        if (isCapturing) {
            mDisconnectIcon.setAlpha(DISABLED_ALPHA);
            mDisconnectIcon.setEnabled(false);

            mScanIcon.setAlpha(DISABLED_ALPHA);
            mScanIcon.setEnabled(false);

            mMultiPreivewIcon.setAlpha(DISABLED_ALPHA);
            mMultiPreivewIcon.setEnabled(false);
        } else {
            mDisconnectIcon.setAlpha(ENABLE_ALPAHA);
            mDisconnectIcon.setEnabled(true);

            mScanIcon.setAlpha(ENABLE_ALPAHA);
            mScanIcon.setEnabled(true);

            mMultiPreivewIcon.setAlpha(ENABLE_ALPAHA);
            mMultiPreivewIcon.setEnabled(true);
        }
    }

    @Override
    public void onAvalibleDevicesUpdate(
            LinkedHashMap<String, IRemoteDevice> devices) {
    }

    @Override
    public void onConnectedDeviesUpdate(
            LinkedHashMap<String, IRemoteDevice> devices) {
    }

    private void getView() {
        mActivity.getLayoutInflater().inflate(R.layout.multi_camera_operator,
                mParentView, true);
        mOperatorContainer = (ViewGroup) mParentView
                .findViewById(R.id.operator_container);
        mDisconnectIcon = (ImageView) mOperatorContainer
                .findViewById(R.id.disconnect_bn);

        mDisconnectIcon.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                showDisconnectDialog();
            }
        });
        mScanIcon = (ImageView) mOperatorContainer.findViewById(R.id.scan_bn);
        mScanIcon.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                mOperateViewListener.onScanButtonClick();
            }
        });
        mMultiPreivewIcon = (ImageView) mOperatorContainer
                .findViewById(R.id.multi_preivew_bn);
        mMultiPreivewIcon.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                mOperateViewListener.onMultiPreviewButtonClick();
            }
        });

        mDialogContainer = (ViewGroup) mActivity.getLayoutInflater().inflate(
                R.layout.multi_camera_disconnect_dialog, null, false);
        ((ViewGroup) mParentView.getParent()).addView(mDialogContainer);
        Button negativeButton = (Button) mDialogContainer
                .findViewById(R.id.negative_button);
        negativeButton.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                dismissDisconnectDialog();
            }
        });
        Button positiveButton = (Button) mDialogContainer
                .findViewById(R.id.positive_button);
        positiveButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                mOperateViewListener.onDisconnectButtonClick();
                dismissDisconnectDialog();
            }
        });

        // scan view maybe always be shown.
        mScanIcon.setVisibility(View.VISIBLE);
        // multi preview view and disconnect view will be shown after multi
        // preview start.
    }

    private void removeView() {
        if (mOperatorContainer != null) {
            mOperatorContainer.removeAllViews();
            mParentView.removeView(mOperatorContainer);
            mOperatorContainer = null;
        }
    }

    private void reInflate() {
        Log.i(TAG, "reInflate ");
        boolean isShown = (mOperatorContainer.getVisibility() == View.VISIBLE);
        hide();
        removeView();
        getView();
        // TODO: should init operator views status.
        if (isShown) {
            show();
        }
    }

    private void showDisconnectDialog() {
        mDialogContainer.setVisibility(View.VISIBLE);
    }

    private void dismissDisconnectDialog() {
        mDialogContainer.setVisibility(View.GONE);
    }
}
