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

package com.mediatek.camera.v2.stream.multicamera.renderer;

import android.graphics.SurfaceTexture;
import android.graphics.SurfaceTexture.OnFrameAvailableListener;
import android.os.Handler;
import android.util.Log;

import junit.framework.Assert;

/**
 * A wrapper for surface texture.
 */
public class SurfaceTextureWrapper {
    private static final String TAG = SurfaceTextureWrapper.class.getSimpleName();
    private static final int INTERVALS = 300;
    private static final int MATRIX_LEN = 16;
    private static final float MINISECOND_PER_SECOND = 1000.0f;
    private SurfaceTexture mSurfaceTexture = null;
    private String mName;
    private float[] mSTTransformMatrix = new float[MATRIX_LEN];
    private long mTimeStamp = 0L;
    private long mNewTimeStamp = 0L;
    private long mStartTime = 0L;

    private int mTextureId = -1;
    private int mFrameCount = 0;

    private int mWidth = -1;
    private int mHeight = -1;

    /**
     * constructor .
     *
     * @param name
     *            surface texture name
     */
    public SurfaceTextureWrapper(String name) {
        mName = name;
    }

    /**
     * setOnFrameAvailableListener, set listener .
     *
     * @param frameListener
     *            frame listener
     * @param handler
     *            handler
     */
    public void setOnFrameAvailableListener(OnFrameAvailableListener frameListener,
            Handler handler) {
        if (mSurfaceTexture == null) {
            Assert.assertTrue(false);
        }
        mSurfaceTexture.setOnFrameAvailableListener(frameListener, handler);
    }

    /**
     * set surface texture buffer size .
     *
     * @param width
     *            width
     * @param height
     *            height
     */
    public void setDefaultBufferSize(int width, int height) {
        Assert.assertTrue(width > 0);
        Assert.assertTrue(height > 0);
        if (mWidth == width && mHeight == height && mSurfaceTexture != null) {
            Log.i(TAG, "skip setDefaultBufferSize w = " + width + " h = " + height);
            return;
        }
        mWidth = width;
        mHeight = height;
        if (mSurfaceTexture == null) {
            if (mTextureId < 0) {
                mTextureId = GLUtil.generateTextureIds(1)[0];
                GLUtil.bindPreviewTexure(mTextureId);
            }
            mSurfaceTexture = new SurfaceTexture(mTextureId);
        }
        mSurfaceTexture.setDefaultBufferSize(width, height);
    }

    /**
     * get width .
     *
     * @return width
     */
    public int getWidth() {
        return mWidth;
    }

    /**
     * get Height .
     *
     * @return height
     */
    public int getHeight() {
        return mHeight;
    }

    /**
     * get surface texture id .
     *
     * @return texture id
     */
    public int getTextureId() {
        return mTextureId;
    }

    /**
     * get surface texture .
     *
     * @return surface texture
     */
    public SurfaceTexture getSurfaceTexture() {
        return mSurfaceTexture;
    }

    /**
     * getBufferTransformMatrix .
     *
     * @return matrix
     */
    public float[] getBufferTransformMatrix() {
        return mSTTransformMatrix;
    }

    /**
     * get tiem stamp .
     *
     * @return time stamp
     */
    public long getTimeStamp() {
        return mTimeStamp;
    }

    /**
     * judge whether update time stamp .
     *
     * @return whether is new time stamp
     */
    public boolean isNewTimeStamp() {
        return mTimeStamp != mNewTimeStamp;
    }

    /**
     * update time stamp .
     */
    public void updateTimeStamp() {
        mTimeStamp = mNewTimeStamp;
    }

    /**
     * this method must be called in GL Thread .
     */
    public void updateTexImage() {
        if (mSurfaceTexture != null) {
            mSurfaceTexture.updateTexImage();
            mNewTimeStamp = mSurfaceTexture.getTimestamp();
            mSurfaceTexture.getTransformMatrix(mSTTransformMatrix);
        }
        // frame rate.
        mFrameCount++;
        if (mFrameCount % INTERVALS == 0) {
            long currentTime = System.currentTimeMillis();
            int intervals = (int) (currentTime - mStartTime);
            double fps = (mFrameCount * MINISECOND_PER_SECOND) / intervals;
            Log.i(TAG, "[updateTexImage] name:" + mName + ", fps = " + fps);
            mStartTime = currentTime;
            mFrameCount = 0;
        }
    }

    /**
     * release .
     */
    public void release() {
        resetSTStatus();
        mWidth = 0;
        mHeight = 0;
        if (mTextureId >= 0) {
            GLUtil.deleteTextures(new int[] { mTextureId });
            mTextureId = -1;
        }
        if (mSurfaceTexture != null) {
            mSurfaceTexture.setOnFrameAvailableListener(null);
            mSurfaceTexture.release();
            mSurfaceTexture = null;
        }
    }

    private void resetSTStatus() {
        mTimeStamp = 0L;
        mNewTimeStamp = 0L;
        mSTTransformMatrix = new float[MATRIX_LEN];
    }
}