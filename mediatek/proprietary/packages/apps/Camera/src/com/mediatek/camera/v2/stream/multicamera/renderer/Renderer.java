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

import android.app.Activity;
import android.opengl.GLES20;
import android.util.Log;

import junit.framework.Assert;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

/**
 * Almost all Renderer's methods should be called in GL Thread.
 */
public class Renderer {
    private static final String TAG = "Renderer";

    private static final int INTERVALS = 300;
    private static final int ROTATE_DEGREE_90 = 90;
    private static final int ROTATE_DEGREE_180 = 180;
    private static final int ROTATE_DEGREE_270 = 270;
    private static final int ROTATE_DEGREE_360 = 360;
    private static final int TEMP_SIZE_NUM4 = 4;
    private static final float MINISECOND_PER_SECOND = 1000.0f;
    protected final RendererConfig mConfig;
    private int mRendererWidth = 0;
    private int mRendererHeight = 0;
    private final Activity mActivity;

    private SurfaceTextureWrapper mPreviewStWrapper = null;
    private SurfaceTextureWrapper mCaptureStWrapper = null;

    private AnimationRect mRenderRect = null;
    private AnimationRect mOriRenderRect = null;
    private int mTextureId = -1;
    private int mOrientation = 0;

    // debug info for draw preview.
    private int mDrawFrameCount = 0;
    private long mDrawStartTime = 0;

    private int mCurOrientation = 0;
    private int mFromOrientation = 0;
    private int mToOrientation = 0;

    private final int mAnimationDuration = 300;

    private static final long NO_ANIMATION = -1;

    private float mCurScale = 1;
    private float mFromScale = 0;
    private float mToScale = 0;
    private long mAnimationStartTime = NO_ANIMATION;

    /**
     * Constructor.
     *
     * @param activity
     *            activity
     * @param config
     *            config
     */
    public Renderer(Activity activity, RendererConfig config) {
        Assert.assertNotNull(config);
        mActivity = activity;
        mConfig = config;
    }

    /**
     * setPreviewStWrapper,can get preview surface texture from this.
     *
     * @param stWrapper
     *            surface texture
     */
    public void setPreviewStWrapper(SurfaceTextureWrapper stWrapper) {
        mPreviewStWrapper = stWrapper;
    }

    /**
     * getConfig, get renderer configuration.
     *
     * @return config
     */
    public RendererConfig getConfig() {
        return mConfig;
    }

    /**
     * getPreviewStWrapper, can get preivew surface texture from this.
     *
     * @return surface texture
     */
    public SurfaceTextureWrapper getPreviewStWrapper() {
        return mPreviewStWrapper;
    }

    /**
     * getCaptureWrapper, can get capture surface texture from this.
     *
     * @return surface texture
     */
    public SurfaceTextureWrapper getCaptureWrapper() {
        return mCaptureStWrapper;
    }

    /**
     * setCaptureStWrapper.
     *
     * @param stWrapper
     *            surface texture
     */
    public void setCaptureStWrapper(SurfaceTextureWrapper stWrapper) {
        mCaptureStWrapper = stWrapper;
    }

    /**
     * setImageTextureId,set image texture id.
     *
     * @param id
     *            image texture id
     */
    public void setImageTextureId(int id) {
        mTextureId = id;
    }

    /**
     * getImageTextureId,get image texture id.
     *
     * @return texture id
     */
    public int getImageTextureId() {
        return mTextureId;
    }

    /**
     * set renderer region, if orientation is not 0, will rotation and scale
     * base on orientation.
     *
     * @param rect
     *            animation rect
     */
    public void setRenderRect(AnimationRect rect) {
        mOriRenderRect = rect;
        if (mConfig.getIsNeedRotate()) {
            if (mOrientation != 0) {
                double width = mOriRenderRect.getRectF().width();
                double height = mOriRenderRect.getRectF().height();
                float scale = (float) (Math.min(width, height) / Math.max(width, height));

                mRenderRect = mOriRenderRect.copy();
                if (mOrientation % ROTATE_DEGREE_180 != 0) {
                    mRenderRect.scale(scale, false);
                }
                mRenderRect.rotate(mOrientation);
            } else {
                mRenderRect = null;
            }
        }

    }

    /**
     * get renderer region, if animation is doing, will get new region which
     * will rotate and scale base on new orientation.
     *
     * @return animation rect
     */
    public AnimationRect getRenderRect() {
        advanceAnimation();
        return mRenderRect == null ? mOriRenderRect : mRenderRect;
    }

    /**
     * getActivity.
     *
     * @return activity
     */
    public Activity getActivity() {
        return mActivity;
    }

    /**
     * getRendererWidth.
     *
     * @return width
     */
    public int getRendererWidth() {
        return mRendererWidth;
    }

    /**
     * getRendererHeight.
     *
     * @return height
     */
    public int getRendererHeight() {
        return mRendererHeight;
    }

    /**
     * createProgram.
     *
     * @param vertexSource
     *            vertexSource
     * @param fragmentSource
     *            fragmentSource
     * @return result
     */
    public int createProgram(String vertexSource, String fragmentSource) {
        int vertexShader = loadShader(GLES20.GL_VERTEX_SHADER, vertexSource);
        int fragmentShader = loadShader(GLES20.GL_FRAGMENT_SHADER, fragmentSource);
        int program = GLES20.glCreateProgram();
        GLES20.glAttachShader(program, vertexShader);
        GLES20.glAttachShader(program, fragmentShader);
        GLES20.glLinkProgram(program);

        int[] linkStatus = new int[1];
        GLES20.glGetProgramiv(program, GLES20.GL_LINK_STATUS, linkStatus, 0);
        if (linkStatus[0] != GLES20.GL_TRUE) {
            Log.e(TAG, "Could not link program:");
            Log.e(TAG, GLES20.glGetProgramInfoLog(program));
            GLES20.glDeleteProgram(program);
            program = 0;
        }
        return program;
    }

    /**
     * loadShader.
     *
     * @param shaderType
     *            shader type
     * @param source
     *            source
     * @return result
     */
    public int loadShader(int shaderType, String source) {
        int shader = GLES20.glCreateShader(shaderType);
        GLES20.glShaderSource(shader, source);
        GLES20.glCompileShader(shader);

        int[] compiled = new int[1];
        GLES20.glGetShaderiv(shader, GLES20.GL_COMPILE_STATUS, compiled, 0);
        if (compiled[0] == 0) {
            Log.e(TAG, "Could not compile shader(TYPE=" + shaderType + "):");
            Log.e(TAG, GLES20.glGetShaderInfoLog(shader));
            GLES20.glDeleteShader(shader);
            shader = 0;
        }

        return shader;
    }

    /**
     * debugFrameRate, get current draw frame rate.
     *
     * @param tag
     *            tag
     */
    public void debugFrameRate(String tag) {
        mDrawFrameCount++;
        if (mDrawFrameCount % INTERVALS == 0) {
            long currentTime = System.currentTimeMillis();
            int intervals = (int) (currentTime - mDrawStartTime);
            Log.i(tag, "[Wrapping-->" + tag + "]" + "[Preview] Drawing frame, fps = "
                    + (mDrawFrameCount * MINISECOND_PER_SECOND) / intervals + " in last "
                    + intervals + " millisecond.");
            mDrawStartTime = currentTime;
            mDrawFrameCount = 0;
        }
    }

    /**
     * release.
     */
    public void release() {
        if (mPreviewStWrapper != null) {
            mPreviewStWrapper.release();
            mPreviewStWrapper = null;
        }
        if (mCaptureStWrapper != null) {
            mCaptureStWrapper.release();
            mCaptureStWrapper = null;
        }
    }

    /**
     * draw.
     *
     * @param preTex
     *            preTex
     * @param preTexMtx
     *            preTexMtx
     * @param texReverseRotateMtx
     *            texReverseRotateMtx
     * @param topRect
     *            topRect
     * @param rotation
     *            rotation
     * @param needFlip
     *            needFlip
     */
    public void draw(int preTex, final float[] preTexMtx, final float[] texReverseRotateMtx,
            final AnimationRect topRect, int rotation, boolean needFlip) {

    }

    /**
     * update orientation, and do animation.
     *
     * @param orientation
     *            new orientation
     */
    public void updateOrientation(int orientation) {
        int toOrientation = (ROTATE_DEGREE_360 - orientation) % ROTATE_DEGREE_360;
        if (mOrientation != orientation && mConfig.getIsNeedRotate() == true) {
            Log.d(TAG, "updateOrientation, orientation:" + orientation + ",mOrientation:"
                    + mOrientation);
            double width = mOriRenderRect.getRectF().width();
            double height = mOriRenderRect.getRectF().height();
            float scale = (float) (Math.min(width, height) / Math.max(width, height));

            if ((toOrientation - mOrientation + ROTATE_DEGREE_360) % ROTATE_DEGREE_180 != 0) {
                int from = ((mOrientation == ROTATE_DEGREE_270 && toOrientation !=
                        ROTATE_DEGREE_180) ? -ROTATE_DEGREE_90 : mOrientation);
                int to = ((mOrientation != ROTATE_DEGREE_180 && toOrientation == ROTATE_DEGREE_270)
                        ? -ROTATE_DEGREE_90 : toOrientation);

                if (toOrientation % ROTATE_DEGREE_180 != 0) {
                    doAnimation(from, to, 1.0f, scale);
                } else {
                    doAnimation(from, to, scale, 1.0f);
                }
            } else {
                mRenderRect = mOriRenderRect.copy();
                mRenderRect.scale(scale, false);
                mRenderRect.rotate(toOrientation);
            }
        }
        mOrientation = toOrientation;
    }

    protected FloatBuffer createFloatBuffer(FloatBuffer floatBuffer, float[] texCoor) {
        if (floatBuffer == null) {
            Log.v(TAG, "ByteBuffer.allocateDirect");
            floatBuffer = ByteBuffer.allocateDirect(texCoor.length * TEMP_SIZE_NUM4)
                    .order(ByteOrder.nativeOrder()).asFloatBuffer();
        }
        floatBuffer.clear();
        floatBuffer.put(texCoor);
        floatBuffer.position(0);
        return floatBuffer;
    }

    // when do take picture, should reverse coordinate system align y
    protected void setRendererSize(int width, int height, boolean needReverse) {
        mRendererWidth = width;
        mRendererHeight = height;
    }

    // default is no need to reverse coordinate system
    protected void setRendererSize(int width, int height) {
        mRendererWidth = width;
        mRendererHeight = height;
    }

    protected void releaseBitmapTexture(int textureId) {
        int[] textures = new int[] { textureId };
        GLES20.glDeleteTextures(1, textures, 0);
        Log.i(TAG, "Renderer releaseBitmapTexture glDeleteTextures num = " + 1);
    }

    private void doAnimation(int fromOrientation, int toOrientation, float fromScale,
            float toScale) {
        mFromOrientation = fromOrientation;
        mToOrientation = toOrientation;
        mFromScale = fromScale;
        mToScale = toScale;
        mAnimationStartTime = AnimationTime.startTime();
        advanceAnimation();
    }

    private void advanceAnimation() {
        if (mAnimationStartTime == NO_ANIMATION) {
            return;
        }

        float progress;
        if (mAnimationDuration == 0) {
            progress = 1;
        } else {
            long now = AnimationTime.get();
            progress =
                    (float) (now - mAnimationStartTime) / mAnimationDuration;
        }

        if (progress >= 1) {
            progress = 1;
        }

        boolean done = interpolate(progress);

        mRenderRect = mOriRenderRect.copy();
        Log.d("zhongchao", "advanceAnimation,mCurOrientation:" + mCurOrientation + ",mCurScale:"
                + mCurScale + ",progress:" + progress);
        mRenderRect.scale(mCurScale, false);
        mRenderRect.rotate(mCurOrientation);

        if (done) {
            mAnimationStartTime = NO_ANIMATION;
        }
    }

    private boolean interpolate(float progress) {
        if (progress >= 1) {
            mCurScale = mToScale;
            mCurOrientation = mToOrientation;
            return true;
        } else {
            mCurScale = mFromScale + progress * (mToScale - mFromScale);
            mCurOrientation = (int) (mFromOrientation + progress
                    * (mToOrientation - mFromOrientation));

            return (mCurScale == mToScale && mCurOrientation == mToOrientation);
        }
    }

}
