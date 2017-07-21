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
import android.content.Context;
import android.hardware.display.DisplayManager;
import android.opengl.EGL14;
import android.opengl.EGLContext;
import android.opengl.GLES20;
import android.opengl.Matrix;
import android.os.ConditionVariable;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.Surface;

import java.nio.FloatBuffer;

/**
 * Render preview buffer to preview surface.
 */
public class ScreenRenderer extends Renderer implements Runnable {
    private static final String TAG = "ScreenRenderer";
    private static final int BLOCK_SHORT_TIME = 100;
    private static final int BLOCK_LONG_TIME = 2000;
    private static final int ROTATE_DEGREE_90 = 90;
    private static final int ROTATE_DEGREE_180 = 180;
    private static final int ROTATE_DEGREE_270 = 270;
    private static final float TRANSLATE_LEN = .5f;
    private FloatBuffer mVtxBuf; // vertex coordinates
    private FloatBuffer mTexCoordBuf; // texture coordinates
    // private FloatBuffer mTopGraphicPositionBuf = null;
    private int mRenderTexWidth;
    private int mRenderTexHeight;
    private int mTextureRotation = 0;
    // transform matrix
    private float[] mPosMtx = GLUtil.createIdentityMtx();
    private float[] mPMtx = GLUtil.createIdentityMtx(); // project
    private float[] mVMtx = GLUtil.createIdentityMtx(); // view
    private float[] mMMtx = GLUtil.createIdentityMtx(); // mode
    private float[] mTexRotateMtx = GLUtil.createIdentityMtx(); // rotate
                                                                // texture
    // resource renderer

    // A surface is used to receive preview buffer, will create EGLSurface by it
    private Surface mPreviewSurface = null;
    private WindowSurface mPreviewEGLSurface;
    private EglCore mEglCore;
    // private int mEditTexSize = 0;
    private int mProgram = -1; // program id
    private int mPositionHandle = -1; // vertex position handle
    private int mTexRotateMtxHandle = -1;
    private int mTexCoordHandle = -1; // texture position handle
    private int mPosMtxHandle = -1;
    private int mSamplerHandle = -1; // sampler handle

    private ScreenHandler mScreenHandler;
    private final Object mReadyFence = new Object(); // guards ready/running
    private boolean mReady;
    private boolean mRunning;
    private EGLContext mSharedEGLContext = null;
    private boolean mIsEGLSurfaceReady = false;
    private final ConditionVariable mUpdateEGLSurfaceSync = new ConditionVariable();
    private final ConditionVariable mReleaseScreenSurfaceSync = new ConditionVariable();
    private final ConditionVariable mDrawLockableConditionVariable = new ConditionVariable();

    /**
     * This can be called from non-gl thread.
     *
     * @param activity
     *            activity
     * @param config
     *            render config
     */
    public ScreenRenderer(Activity activity, RendererConfig config) {
        super(activity, config);
        mTexCoordBuf = createFloatBuffer(mTexCoordBuf, GLUtil.createTexCoord());
        // new a thread to share EGLContext with pip wrapping GL Thread
        new Thread(this, "ScreenRenderer").start();
        ((DisplayManager) activity
                .getSystemService(Context.DISPLAY_SERVICE))
                .registerDisplayListener(mDisplayListener, null);
    }

    /**
     * set surface, draw content to this surface.
     *
     * @param surface
     *            surface
     */
    public void setSurface(Surface surface) {
        Log.i(TAG, "setSurface surface = " + surface);
        mIsEGLSurfaceReady = false;
        if (surface == null) {
            throw new RuntimeException("ScreenRenderer setSurface to null!!!!!");
        }
        mPreviewSurface = surface;
        waitRendererThreadActive();
        updateEGLSurface();
        checkDisplayRotation();
        mIsEGLSurfaceReady = true;
    }

    /**
     * Initialize screen renderer program and shader, get shared egl context and
     * init sub render
     * <p>
     * Note: this should be called in GL Thread.
     *
     */
    public void init() {
        Log.i(TAG, "init: " + this);
        // initialize program and shader
        initGL();
        // get shared egl context
        mSharedEGLContext = EGL14.eglGetCurrentContext();
    }

    @Override
    public void release() {
        Log.i(TAG, "release: " + this);
        synchronized (mReadyFence) {
            if (mScreenHandler != null) {
                mScreenHandler.removeCallbacksAndMessages(null);
                mReleaseScreenSurfaceSync.close();
                mScreenHandler.obtainMessage(ScreenHandler.MSG_RELEASE_SURFACE).sendToTarget();
                mReleaseScreenSurfaceSync.block(BLOCK_LONG_TIME);
            }
        }
        super.setRendererSize(-1, -1);
        ((DisplayManager) getActivity()
                .getSystemService(Context.DISPLAY_SERVICE))
                .unregisterDisplayListener(mDisplayListener);
    }

    /**
     * draw texId to surface.
     *
     * @param texId
     *            texture id
     */
    public void draw(int texId) {
        synchronized (mReadyFence) {
            if (mScreenHandler != null && mIsEGLSurfaceReady) {
                mDrawLockableConditionVariable.close();
                mScreenHandler.removeMessages(ScreenHandler.MSG_FRAME_AVAILABLE);
                mScreenHandler.obtainMessage(ScreenHandler.MSG_FRAME_AVAILABLE, texId, 0)
                        .sendToTarget();
                mDrawLockableConditionVariable.block(BLOCK_SHORT_TIME);
            }
        }
    }

    @Override
    public void run() {
        Looper.prepare();
        synchronized (mReadyFence) {
            Log.i(TAG, "Screen renderer thread started!");
            mScreenHandler = new ScreenHandler();
            mReady = true;
            mReadyFence.notify();
        }
        Looper.loop();
        Log.i(TAG, "Screen renderer thread exiting!");
        synchronized (mReadyFence) {
            mReady = mRunning = false;
            mScreenHandler = null;
        }
    }

    private void checkDisplayRotation() {
        mRenderTexWidth = mPreviewEGLSurface.getWidth();
        mRenderTexHeight = mPreviewEGLSurface.getHeight();
        mTextureRotation = getDisplayRotation(getActivity());
        updateRendererSize(mRenderTexWidth, mRenderTexHeight);
    }

    private final DisplayManager.DisplayListener mDisplayListener =
            new DisplayManager.DisplayListener() {
                @Override
                public void onDisplayAdded(int arg0) {
                    // Do nothing.
                }

                @Override
                public void onDisplayChanged(int displayId) {
                    Log.i(TAG, "onDisplayChanged displayId = " + displayId);
                    checkDisplayRotation();
                }

                @Override
                public void onDisplayRemoved(int arg0) {

                }
            };

    private void waitRendererThreadActive() {
        synchronized (mReadyFence) {
            if (mRunning) {
                Log.i(TAG, "screen renderer already running!");
                return;
            }
            mRunning = true;
            while (!mReady) {
                try {
                    Log.i(TAG, "wait for screen renderer thread ready, current mReady = " + mReady);
                    mReadyFence.wait();
                } catch (InterruptedException ie) {
                    // ignore
                }
            }
            // when screen renderer thread started, be sure not block first
            // frame
            mDrawLockableConditionVariable.open();
        }
    }

    private void updateRendererSize(int width, int height) {
        Log.i(TAG, "updateRendererSize width = " + width + " height = " + height);
        resetMatrix();
        Matrix.orthoM(mPMtx, 0, 0, width, 0, height, -1, 1);
        initVertexData(width, height);
    }

    private int getDisplayRotation(Activity activity) {
        int rotation = activity.getWindowManager().getDefaultDisplay().getRotation();
        switch (rotation) {
        case Surface.ROTATION_0:
            return 0;
        case Surface.ROTATION_90:
            return ROTATE_DEGREE_90;
        case Surface.ROTATION_180:
            return ROTATE_DEGREE_180;
        case Surface.ROTATION_270:
            return ROTATE_DEGREE_270;
         default:
             return 0;
        }
    }

    /**
     * ScreenHandler.
     *
     */
    private class ScreenHandler extends Handler {
        public static final int MSG_SETUP_SURFACE = 0;
        public static final int MSG_RELEASE_SURFACE = 1;
        public static final int MSG_UPDATE_EGL_SURFACE = 2;
        public static final int MSG_FRAME_AVAILABLE = 3;
        private static final int TEMP_SIZE_NUM6 = 6;
        private static final int TEMP_SIZE_NUM2 = 2;
        private static final int TEMP_SIZE_NUM4 = 4;
        private static final int TEMP_SIZE_NUM3 = 3;

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case MSG_SETUP_SURFACE:
                doSetupEGLSurface();
                break;
            case MSG_RELEASE_SURFACE:
                doReleaseSurface();
                mReleaseScreenSurfaceSync.open();
                break;
            case MSG_UPDATE_EGL_SURFACE:
                doUpdateEGLSurface();
                mUpdateEGLSurfaceSync.open();
                break;
            case MSG_FRAME_AVAILABLE:
                doDraw(msg.arg1);
                break;
            default:
                break;
            }
        }

        private void doSetupEGLSurface() {
            Log.i(TAG, "handleSetupSurface  mEglCore = " + mEglCore +
                    " mPreviewEGLSurface = " + mPreviewEGLSurface + " mPreviewSurface = "
                    + mPreviewSurface);
            if (mEglCore == null) {
                mEglCore = new EglCore(mSharedEGLContext, EglCore.FLAG_TRY_GLES3);
            }
            if (mPreviewEGLSurface == null) {
                mPreviewEGLSurface = new WindowSurface(mEglCore, mPreviewSurface);
                mPreviewEGLSurface.makeCurrent();
            }
        }

        private void doUpdateEGLSurface() {
            Log.i(TAG, "updateEGLSurface mPreviewEGLSurface = " + mPreviewEGLSurface);
            if (mPreviewEGLSurface != null) {
                // release old egl surface
                mPreviewEGLSurface.makeNothingCurrent();
                mPreviewEGLSurface.releaseEglSurface();
                // create new egl surface
                mPreviewEGLSurface = new WindowSurface(mEglCore, mPreviewSurface);
                mPreviewEGLSurface.makeCurrent();
            } else {
                doSetupEGLSurface();
            }
        }

        private void doReleaseSurface() {
            Log.i(TAG, "doReleaseSurface");
            if (mPreviewEGLSurface != null) {
                mPreviewEGLSurface.makeNothingCurrent();
                mPreviewEGLSurface.releaseEglSurface();
                mPreviewEGLSurface = null;
            }
            if (mEglCore != null) {
                mEglCore.release();
                mEglCore = null;
            }
            mIsEGLSurfaceReady = false;
            mPreviewSurface = null;
            mProgram = -1;
            Looper looper = Looper.myLooper();
            if (looper != null) {
                looper.quit();
            }
        }

        private void doDraw(int texId) {
            // long time = System.currentTimeMillis();
            if (getRendererWidth() <= 0 || getRendererHeight() <= 0 || mPreviewEGLSurface == null) {
                return;
            }
            GLUtil.checkGlError("ScreenDraw_Start");
            GLES20.glViewport(0, 0, mRenderTexWidth, mRenderTexHeight);
            GLES20.glClearColor(0f, 0f, 0f, 1f);
            GLES20.glClear(GLES20.GL_DEPTH_BUFFER_BIT | GLES20.GL_COLOR_BUFFER_BIT);
            // use program
            GLES20.glUseProgram(mProgram);
            // vertex
            mVtxBuf.position(0);
            GLES20.glVertexAttribPointer(mPositionHandle, TEMP_SIZE_NUM3, GLES20.GL_FLOAT, false,
                    TEMP_SIZE_NUM4 * TEMP_SIZE_NUM3,
                    mVtxBuf);
            mTexCoordBuf.position(0);
            GLES20.glVertexAttribPointer(mTexCoordHandle, TEMP_SIZE_NUM2, GLES20.GL_FLOAT, false,
                    TEMP_SIZE_NUM4 * TEMP_SIZE_NUM2,
                    mTexCoordBuf);
            GLES20.glEnableVertexAttribArray(mPositionHandle);
            GLES20.glEnableVertexAttribArray(mTexCoordHandle);
            // matrix
            GLES20.glUniformMatrix4fv(mPosMtxHandle, 1, false, mPosMtx, 0);
            GLES20.glUniformMatrix4fv(mTexRotateMtxHandle, 1, false, mTexRotateMtx, 0);
            // sampler
            GLES20.glUniform1i(mSamplerHandle, 0);
            // texture
            GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texId);
            // draw
            GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, TEMP_SIZE_NUM6);
            // swap buffer
            mPreviewEGLSurface.swapBuffers();
            mDrawLockableConditionVariable.open();
            debugFrameRate(TAG);
            // mPreviewEGLSurface.saveFrame(new
            // File("/storage/sdcard0/piptest.jpg"));
            GLUtil.checkGlError("ScreenDraw_End");
        }
    }

    private void initGL() {
        if (mProgram != -1) {
            return;
        }
        GLUtil.checkGlError("initGL_Start");
        final String vertexShader =
                "attribute vec4 aPosition;\n" +
                        "attribute vec4 aTexCoord;\n" +
                        "uniform   mat4 uPosMtx;\n" +
                        "uniform   mat4 uTexRotateMtx;\n" +
                        "varying   vec2 vTexCoord;\n" +
                        "void main() {\n" +
                        "  gl_Position = uPosMtx * aPosition;\n" +
                        "  vTexCoord   = (uTexRotateMtx * aTexCoord).xy;\n" +
                        "}\n";
        final String fragmentShader =
                "precision mediump float;\n" +
                        "uniform sampler2D uSampler;\n" +
                        "varying vec2      vTexCoord;\n" +
                        "void main() {\n" +
                        "  gl_FragColor = texture2D(uSampler, vTexCoord);\n" +
                        "}\n";
        mProgram = GLUtil.createProgram(vertexShader, fragmentShader);
        mPositionHandle = GLES20.glGetAttribLocation(mProgram, "aPosition");
        mTexCoordHandle = GLES20.glGetAttribLocation(mProgram, "aTexCoord");
        // matrix
        mPosMtxHandle = GLES20.glGetUniformLocation(mProgram, "uPosMtx");
        mTexRotateMtxHandle = GLES20.glGetUniformLocation(mProgram, "uTexRotateMtx");
        // sampler
        mSamplerHandle = GLES20.glGetUniformLocation(mProgram, "uSampler");
        GLES20.glDisable(GLES20.GL_DEPTH_TEST);
        GLES20.glDisable(GLES20.GL_CULL_FACE);
        GLES20.glDisable(GLES20.GL_BLEND);
        GLUtil.checkGlError("initGL_E");
    }

    private void resetMatrix() {
        mPosMtx = GLUtil.createIdentityMtx();
        mPMtx = GLUtil.createIdentityMtx();
        mVMtx = GLUtil.createIdentityMtx();
        mMMtx = GLUtil.createIdentityMtx();
        mTexRotateMtx = GLUtil.createIdentityMtx();
    }

    private void updateEGLSurface() {
        synchronized (mReadyFence) {
            mUpdateEGLSurfaceSync.close();
            if (mScreenHandler != null) {
                mScreenHandler.removeMessages(ScreenHandler.MSG_FRAME_AVAILABLE);
                mScreenHandler.obtainMessage(ScreenHandler.MSG_UPDATE_EGL_SURFACE).sendToTarget();
            }
            mUpdateEGLSurfaceSync.block();
        }
    }

    private void initVertexData(float width, float height) {
        android.opengl.Matrix.translateM(mTexRotateMtx, 0, mTexRotateMtx, 0, TRANSLATE_LEN,
                TRANSLATE_LEN, 0);
        android.opengl.Matrix.rotateM(mTexRotateMtx, 0, -mTextureRotation, 0, 0, 1);
        android.opengl.Matrix.translateM(mTexRotateMtx, 0, -TRANSLATE_LEN, -TRANSLATE_LEN, 0);
        mVtxBuf = createFloatBuffer(mVtxBuf, GLUtil.createFullSquareVtx(width, height));
        Matrix.multiplyMM(mPosMtx, 0, mMMtx, 0, mVMtx, 0);
        Matrix.multiplyMM(mPosMtx, 0, mPMtx, 0, mPosMtx, 0);
    }
}