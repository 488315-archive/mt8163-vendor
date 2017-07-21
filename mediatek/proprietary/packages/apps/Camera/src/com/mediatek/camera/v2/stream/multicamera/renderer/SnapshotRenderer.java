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
import android.opengl.EGL14;
import android.opengl.EGLContext;
import android.opengl.EGLDisplay;
import android.opengl.EGLSurface;
import android.opengl.GLES20;
import android.opengl.Matrix;
import android.util.Log;
import android.view.Surface;

import java.nio.FloatBuffer;

/**
 * SnapshotRenderer used to snapshot .
 *
 */
public class SnapshotRenderer extends Renderer {
    private static final String TAG = SnapshotRenderer.class.getSimpleName();

    private static final int TEMP_SIZE_NUM2 = 2;
    private static final int TEMP_SIZE_NUM3 = 3;
    private static final int TEMP_SIZE_NUM4 = 4;
    private static final int TEMP_SIZE_NUM6 = 6;
    private FloatBuffer mVtxBuf;
    private FloatBuffer mTexCoordBuf;
    // transform matrix
    private float[] mPosMtx = GLUtil.createIdentityMtx();
    private float[] mPMtx = GLUtil.createIdentityMtx();
    private float[] mVMtx = GLUtil.createIdentityMtx();
    private float[] mMMtx = GLUtil.createIdentityMtx();

    private EglCore mEglCore;
    private Surface mCaptureSurface;
    private WindowSurface mCaptureEGLSurface;

    private int mProgram = -1;
    private int mPositionHandle = -1;
    private int mTexCoordHandle = -1;
    private int mPosMtxHandle = -1;
    private int mSamplerHandle = -1;

    private EGLDisplay mSavedEglDisplay = null;
    private EGLSurface mSavedEglDrawSurface = null;
    private EGLSurface mSavedEglReadSurface = null;
    private EGLContext mSavedEglContext = null;

    /**
     * constructor .
     *
     * @param activity
     *            activity
     * @param config
     *            configuration
     */
    public SnapshotRenderer(Activity activity, RendererConfig config) {
        super(activity, config);
    }

    /**
     * initialize .
     */
    public void init() {
        Log.i(TAG, "initScreenSurface");
        mEglCore = new EglCore(EGL14.eglGetCurrentContext(), EglCore.FLAG_TRY_GLES3);
        initGL();
        mTexCoordBuf = createFloatBuffer(mTexCoordBuf, GLUtil.createTexCoord());
    }

    @Override
    public void setRendererSize(int width, int height, boolean needPortrait) {
        Log.i(TAG, "setRendererSize width = " + width + " height = " + height);
        int selectedWidth = Math.min(width, height);
        int selectedHeight = Math.max(width, height);
        if (needPortrait) { // portrait
            selectedWidth = Math.min(width, height);
            selectedHeight = Math.max(width, height);
        } else { // landscape
            selectedWidth = Math.max(width, height);
            selectedHeight = Math.min(width, height);
        }

        if (selectedWidth == getRendererWidth() && selectedHeight == getRendererHeight()) {
            return;
        }
        resetMatrix();
        super.setRendererSize(selectedWidth, selectedHeight);
        Matrix.orthoM(mPMtx, 0, 0, selectedWidth, 0, selectedHeight, -1, 1);
        // Matrix.translateM(mMMtx, 0, 0, height, 0);
        // Matrix.scaleM(mMMtx, 0, mMMtx, 0, 1, -1, 1);
        Matrix.multiplyMM(mPosMtx, 0, mMMtx, 0, mVMtx, 0);
        Matrix.multiplyMM(mPosMtx, 0, mPMtx, 0, mPosMtx, 0);
        initVertexData(selectedWidth, selectedHeight);
    }

    /**
     * set capture surface, draw content to this surface .
     *
     * @param surface
     *            surface
     */
    public void setCaptureSurface(Surface surface) {
        if (surface == null) {
            throw new RuntimeException("setCaptureSurface capture surface is null!!!!!");
        }
        mCaptureSurface = surface;
    }

    /**
     * draw texId to capture surface .
     *
     * @param texId
     *            texture id
     */
    public void draw(int texId) {
        saveRenderState();
        Log.i(TAG, "draw texId = " + texId + " mCaptureSurface = " + mCaptureSurface);
        if (mCaptureSurface != null) {
            mCaptureEGLSurface = new WindowSurface(mEglCore, mCaptureSurface);
            mCaptureEGLSurface.makeCurrent();
            Log.i(TAG, "Surface width = " + mCaptureEGLSurface.getWidth() + " height = "
                    + mCaptureEGLSurface.getHeight());
        }
        if (getRendererWidth() <= 0 || getRendererHeight() <= 0 || mCaptureEGLSurface == null) {
            return;
        }
        GLUtil.checkGlError("CaptureDraw_Start");
        GLES20.glViewport(0, 0, getRendererWidth(), getRendererHeight());
        GLES20.glClearColor(0f, 0f, 0f, 1f);
        GLES20.glClear(GLES20.GL_DEPTH_BUFFER_BIT | GLES20.GL_COLOR_BUFFER_BIT);
        // use program
        GLES20.glUseProgram(mProgram);
        // vertex
        mVtxBuf.position(0);
        GLES20.glVertexAttribPointer(mPositionHandle, TEMP_SIZE_NUM3, GLES20.GL_FLOAT, false,
                TEMP_SIZE_NUM4 * TEMP_SIZE_NUM3, mVtxBuf);
        mTexCoordBuf.position(0);
        GLES20.glVertexAttribPointer(mTexCoordHandle, TEMP_SIZE_NUM2, GLES20.GL_FLOAT, false,
                TEMP_SIZE_NUM4 * TEMP_SIZE_NUM2,
                mTexCoordBuf);
        GLES20.glEnableVertexAttribArray(mPositionHandle);
        GLES20.glEnableVertexAttribArray(mTexCoordHandle);
        // matrix
        GLES20.glUniformMatrix4fv(mPosMtxHandle, 1, false, mPosMtx, 0);
        // sampler
        GLES20.glUniform1i(mSamplerHandle, 0);
        // texture
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texId);
        // draw
        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, TEMP_SIZE_NUM6);
        mCaptureEGLSurface.swapBuffers();
        GLUtil.checkGlError("CaptureDraw_End");
        mCaptureEGLSurface.makeNothingCurrent();
        mCaptureEGLSurface.releaseEglSurface();
        mCaptureEGLSurface = null;
        restoreRenderState();
        Log.i(TAG, "draw end");
    }

    @Override
    public void release() {
        if (mCaptureEGLSurface != null) {
            mCaptureEGLSurface.makeNothingCurrent();
            mCaptureEGLSurface.release();
            mCaptureEGLSurface = null;
        }
        if (mEglCore != null) {
            mEglCore.release();
            mEglCore = null;
        }
    }

    private void initVertexData(float width, float height) {
        mVtxBuf = createFloatBuffer(mVtxBuf, GLUtil.createFullSquareVtx(width, height));
    }

    private void initGL() {
        Log.i(TAG, "initGL");
        GLUtil.checkGlError("initGL_Start");
        final String vertexShader =
                "attribute vec4 aPosition;\n" +
                        "attribute vec4 aTexCoord;\n" +
                        "uniform   mat4 uPosMtx;\n" +
                        "varying   vec2 vTexCoord;\n" +
                        "void main() {\n" +
                        "  gl_Position = uPosMtx * aPosition;\n" +
                        "  vTexCoord   = aTexCoord.xy;\n" +
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
    }

    private void saveRenderState() {
        mSavedEglDisplay = EGL14.eglGetCurrentDisplay();
        mSavedEglDrawSurface = EGL14.eglGetCurrentSurface(EGL14.EGL_DRAW);
        mSavedEglReadSurface = EGL14.eglGetCurrentSurface(EGL14.EGL_READ);
        mSavedEglContext = EGL14.eglGetCurrentContext();
    }

    private void restoreRenderState() {
        if (!EGL14.eglMakeCurrent(mSavedEglDisplay, mSavedEglDrawSurface, mSavedEglReadSurface,
                mSavedEglContext)) {
            throw new RuntimeException("eglMakeCurrent failed");
        }
    }
}
