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
import android.opengl.GLES11Ext;
import android.opengl.GLES20;
import android.opengl.Matrix;
import android.util.Log;
import android.view.Surface;

/**
 * GLUtil.
 *
 */
public class GLUtil {
    private static final String TAG = "GLUtil";
    private static final int ROTATE_DEGREE_90 = 90;
    private static final int ROTATE_DEGREE_180 = 180;
    private static final int ROTATE_DEGREE_270 = 270;
    private static final int MATRIX_LEN = 16;

    /**
     * createIdentityMtx.
     *
     * @return matrix
     */
    public static float[] createIdentityMtx() {
        float[] matrix = new float[MATRIX_LEN];
        Matrix.setIdentityM(matrix, 0);
        return matrix;
    }

    /**
     * createFullSquareVtx.
     *
     * @param width
     *            width
     * @param height
     *            height
     * @return result
     */
    public static float[] createFullSquareVtx(float width, float height) {
        float vertices[] = new float[] {
                0, 0, 0,
                0, height, 0,
                width, height, 0,

                width, height, 0,
                width, 0, 0,
                0, 0, 0,
        };
        return vertices;
    }

    /**
     * createTopRightRect.
     *
     * @param rect AnimationRect
     * @return result
     */
    public static float[] createTopRightRect(AnimationRect rect) {
        float vertices[] = new float[] {
                rect.getLeftTop()[0], rect.getLeftTop()[1], 0,
                rect.getLeftBottom()[0], rect.getLeftBottom()[1], 0,
                rect.getRightBottom()[0], rect.getRightBottom()[1], 0,

                rect.getRightBottom()[0], rect.getRightBottom()[1], 0,
                rect.getRightTop()[0], rect.getRightTop()[1], 0,
                rect.getLeftTop()[0], rect.getLeftTop()[1], 0,
        };
        return vertices;
    }

    /**
     * createTexCoord.
     *
     * @return result
     */
    public static float[] createTexCoord() {
        float texCoor[] = new float[] {
                0, 0,
                0, 1f,
                1f, 1f,

                1f, 1f,
                1f, 0,
                0, 0 };
        return texCoor;
    }

    /**
     * create 180 degree texture coordinate.
     *
     * @param lowWidth
     *            lowWidth
     * @param highWidth
     *            highWidth
     * @param lowHeight
     *            lowHeight
     * @param highHeight
     *            highHeight
     * @return result
     */
    public static float[] createReverseStandTexCoord(float lowWidth, float highWidth,
            float lowHeight, float highHeight) {
        float texCoor[] = new float[] {
                lowWidth, highHeight,
                lowWidth, lowHeight,
                highWidth, lowHeight,

                highWidth, lowHeight,
                highWidth, highHeight,
                lowWidth, highHeight };
        return texCoor;
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
    public static int createProgram(String vertexSource, String fragmentSource) {
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
     *            shaderType
     * @param source
     *            source
     * @return result
     */
    public static int loadShader(int shaderType, String source) {
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
     * checkGlError.
     *
     * @param op
     *            parameter
     */
    public static void checkGlError(String op) {
        int error;
        while ((error = GLES20.glGetError()) != GLES20.GL_NO_ERROR) {
            Log.i(TAG, op + ":glGetError:0x" + Integer.toHexString(error));
            throw new RuntimeException("glGetError encountered (see log)");
        }
    }

    /**
     * checkEglError.
     *
     * @param op
     *            parameter
     */
    public static void checkEglError(String op) {
        int error;
        while ((error = EGL14.eglGetError()) != EGL14.EGL_SUCCESS) {
            Log.e(TAG, op + ":eglGetError:0x" + Integer.toHexString(error));
            throw new RuntimeException("eglGetError encountered (see log)");
        }
    }

    /**
     * generateTextureIds.
     *
     * @param num
     *            number
     * @return result
     */
    public static int[] generateTextureIds(int num) {
        Log.i(TAG, "GLUtil glGenTextures num = " + num);
        int[] textures = new int[num];
        GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
        GLES20.glGenTextures(num, textures, 0);
        int[] sizes = new int[2];
        GLES20.glGetIntegerv(GLES20.GL_MAX_TEXTURE_SIZE, sizes, 0);
        Log.i(TAG, "GL_MAX_TEXTURE_SIZE sizes[0] = " + sizes[0] + " size[1] = " + sizes[1]);
        return textures;
    }

    /**
     * deleteTextures.
     *
     * @param textureIds
     *            texture id
     */
    public static void deleteTextures(int[] textureIds) {
        Log.i(TAG, "GLUtil glDeleteTextures num = " + textureIds.length);
        GLES20.glDeleteTextures(textureIds.length, textureIds, 0);
    }

    /**
     * bindPreviewTexure.
     *
     * @param texId
     *            texture id
     */
    public static void bindPreviewTexure(int texId) {
        GLES20.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, texId);
        GLES20.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MIN_FILTER,
                GLES20.GL_LINEAR);
        GLES20.glTexParameterf(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_MAG_FILTER,
                GLES20.GL_LINEAR);

        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_S,
                GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameteri(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, GLES20.GL_TEXTURE_WRAP_T,
                GLES20.GL_CLAMP_TO_EDGE);
    }

    private static void bindTexture(int texId) {
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texId);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER,
                GLES20.GL_NEAREST);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER,
                GLES20.GL_LINEAR);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S,
                GLES20.GL_CLAMP_TO_EDGE);
        GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T,
                GLES20.GL_CLAMP_TO_EDGE);
    }

    private static int getDisplayRotation(Activity activity) {
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

    private static float[] convertAnimationRectToTextureArray(AnimationRect rect) {
        float lowWidth = rect.getLeftTop()[0];
        float highWidth = rect.getRightBottom()[0];
        float lowHeight = rect.getLeftTop()[1];
        float highHeight = rect.getRightBottom()[1];
        float vertices[] = new float[] {
                lowWidth, lowHeight, 0,
                lowWidth, highHeight, 0,
                highWidth, highHeight, 0,

                highWidth, highHeight, 0,
                highWidth, lowHeight, 0,
                lowWidth, lowHeight, 0,
        };
        return vertices;
    }

    private static float[] createSquareVtxByCenterEdge(float centerX, float centerY, float edge) {
        float vertices[] = new float[] { (centerX - edge / 2),
                (centerY - edge / 2), 0, (centerX - edge / 2),
                (centerY + edge / 2), 0, (centerX + edge / 2),
                (centerY + edge / 2), 0,

                (centerX + edge / 2), (centerY + edge / 2), 0,
                (centerX + edge / 2), (centerY - edge / 2), 0,
                (centerX - edge / 2), (centerY - edge / 2), 0, };
        return vertices;
    }

    // create 90 degree texture coordinate
    private static float[] createRightTexCoord(float lowWidth, float highWidth, float lowHeight,
            float highHeight) {
        float texCoor[] = new float[] {
                lowWidth, lowHeight,
                highWidth, lowHeight,
                highWidth, highHeight,

                highWidth, highHeight,
                lowWidth, highHeight,
                lowWidth, lowHeight };
        return texCoor;
    }

    // create 270 degree texture coordinate
    private static float[] createLeftTexCoord(float lowWidth, float highWidth, float lowHeight,
            float highHeight) {
        float texCoor[] = new float[] {
                highWidth, highHeight,
                lowWidth, highHeight,
                lowWidth, lowHeight,

                lowWidth, lowHeight,
                highWidth, lowHeight,
                highWidth, highHeight };
        return texCoor;
    }

    private static float[] createTexCoord(float lowWidth, float highWidth, float lowHeight,
            float highHeight, boolean needHorizontalFlip) {
        if (needHorizontalFlip) {
            return createHorizontalFlipTexCoord(lowWidth, highWidth, lowHeight, highHeight);
        }
        return createTexCoord(lowWidth, highWidth, lowHeight, highHeight);
    }

    private static float[] createTexCoord(float lowWidth, float highWidth, float lowHeight,
            float highHeight) {
        float texCoor[] = new float[] {
                lowWidth, lowHeight,
                lowWidth, highHeight,
                highWidth, highHeight,

                highWidth, highHeight,
                highWidth, lowHeight,
                lowWidth, lowHeight };
        return texCoor;
    }

    // create 0 degree texture coordinate
    private static float[] createStandTexCoord(float lowWidth, float highWidth, float lowHeight,
            float highHeight) {
        float texCoor[] = new float[] {
                highWidth, lowHeight,
                highWidth, highHeight,
                lowWidth, highHeight,

                lowWidth, highHeight,
                lowWidth, lowHeight,
                highWidth, lowHeight };
        return texCoor;
    }

    private static float[] createHorizontalFlipTexCoord(float lowWidth, float highWidth,
            float lowHeight, float highHeight) {
        float texCoor[] = new float[] {
                highWidth, lowHeight,
                highWidth, highHeight,
                lowWidth, highHeight,

                lowWidth, highHeight,
                lowWidth, lowHeight,
                highWidth, lowHeight };
        return texCoor;
    }
}
