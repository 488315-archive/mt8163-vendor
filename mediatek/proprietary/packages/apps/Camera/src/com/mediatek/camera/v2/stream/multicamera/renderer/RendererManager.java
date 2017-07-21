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
package com.mediatek.camera.v2.stream.multicamera.renderer;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.SurfaceTexture;
import android.opengl.GLES20;
import android.opengl.GLUtils;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.util.Size;
import android.view.Surface;

import com.mediatek.camera.v2.stream.multicamera.renderer.RendererConfig.RendererType;

import junit.framework.Assert;

import java.util.ArrayList;
import java.util.List;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;

/**
 * Managers SurfaceTextures, creates SurfaceTexture and TexureRender Objects,
 * and do SW Sync that ensure two SurfaceTextures are sync.
 */
public class RendererManager {
    private static final String TAG = RendererManager.class.getSimpleName();
    private static final int EGL_CONTEXT_CLIENT_VERSION = 0x3098;
    private static final float MINISECOND_PER_SECOND = 1000.0f;

    private final Activity mActivity;
    private final RendererCallback mRendererCallback;

    private EGLConfig mEglConfig;
    private EGLDisplay mEglDisplay;
    private EGLContext mEglContext;
    private EGLSurface mEglSurface;
    private EGL10 mEgl;

    private final Object mRenderLock = new Object();

    private HandlerThread mEglThread;
    private EglHandler mEglHandler;

    private Surface mCaptureSurface;
    private Surface mSnapshotSurface; // for capture and video snap shot
    private int mCurrentOrientation; // this orientation is for g-sensor

    private boolean mNeedNotifyFirstFrameForSurfaceChanged = true;

    private RecorderRenderer mRecorderRenderer; // for VR

    /**
     * notify first frame arrive when draw first frame.
     */
    public interface RendererCallback {
        /**
         * onFristFrameAvailable.
         *
         * @param timestamp
         *            time stamp
         */
        public void onFristFrameAvailable(long timestamp);
    }

    /**
     * constructor.
     *
     * @param activity
     *            activity
     * @param callback
     *            callback
     */
    public RendererManager(Activity activity, RendererCallback callback) {
        mActivity = activity;
        mRendererCallback = callback;
    }

    /**
     * new a handler thread and create EGL Context in it. We called this thread
     * to "GL Thread".
     * <p>
     * Note: if "GL Thread" exits, skip initialization
     */
    public void init() {
        Log.i(TAG, "[init]+ mEglHandler = " + mEglHandler);
        if (mEglHandler != null) {
            Log.i(TAG, "[init]- ");
            return;
        }

        initializeEglThread();
        initializeEgl();

        mEglHandler.sendEmptyMessage(EglHandler.MSG_INITIALIZE);
        waitDone();
        Log.i(TAG, "[init]- mEglHandler = " + mEglHandler);
    }

    /**
     * release surface textures, related renderer and " GL Thread".
     */
    public void unInit() {
        Log.i(TAG, "[unInit]+ mEglHandler = " + mEglHandler);

        if (mEglHandler == null) {
            return;
        }

        // remove all previous messages and resume mEglThreadBlockVar
        mEglHandler.removeCallbacksAndMessages(null);
        mEglHandler.sendEmptyMessage(EglHandler.MSG_RELEASE);
        waitDone();

        // release thread
        Looper looper = mEglThread.getLooper();
        if (looper != null) {
            looper.quit();
        }
        mEglHandler = null;
        mEglThread = null;

        Log.i(TAG, "[unInit]-");
    }

    /**
     * call this function to add renderer.
     *
     * @param config
     *            renderer configuration
     */
    public void addRenderer(RendererConfig config) {
        Log.d(TAG, "<addRenderer> type:" + config.getType() + ",name:" + config.getName());

        Assert.assertNotNull(mEglHandler);

        mEglHandler.obtainMessage(EglHandler.MSG_ADD_RENDERERS, config).sendToTarget();
        waitDone();
    }

    /**
     * call this function to delete renderer.
     *
     * @param name
     *            renderer name
     */
    public void deleteRender(String name) {
        Log.d(TAG, "<deleteRender> name:" + name);

        Assert.assertNotNull(mEglHandler);

        mEglHandler.obtainMessage(EglHandler.MSG_DELETE_RENDERERS, name).sendToTarget();
        waitDone();
    }

    /**
     * if this renderer is exist in container.
     *
     * @param name
     *            renderer name
     * @return exist->true, not exist -> false
     */
    public boolean isContainsRender(String name) {
        Assert.assertNotNull(mEglHandler);

        return mEglHandler.isContainsRender(name);
    }

    /**
     * set renderer draw region in screen.
     *
     * @param graphicRect
     *            draw rect
     * @param renderName
     *            render name
     */
    public void setRendererRect(AnimationRect graphicRect, String renderName) {
        if (mEglHandler == null) {
            return;
        }
        mEglHandler.setRenderRect(graphicRect, renderName);
    }

    /**
     * if renderer type is image, we should set bitmap to it.
     *
     * @param bitmap
     *            bitmap
     * @param name
     *            name
     */
    public void setRendererImage(Bitmap bitmap, String name) {
        Log.d(TAG, "<setRendererImage> bitmap:" + bitmap + ",name:" + name);

        Assert.assertNotNull(mEglHandler);

        RenderParameter para = new RenderParameter();
        para.mName = name;
        para.mBitmap = bitmap;
        mEglHandler.obtainMessage(EglHandler.MSG_SET_RENDERER_BITMAP, para).sendToTarget();
        waitDone();
    }

    /**
     * when G-sensor's orientation changed, should update it.
     *
     * @param orientation
     *            G-sensor's new orientation
     */
    public void updateGSensorOrientation(int orientation) {
        mCurrentOrientation = orientation;
        if (mEglHandler == null) {
            return;
        }
        mEglHandler.updateOrientation(orientation);
    }

    /**
     * set preview size to renderer.
     *
     * @param previewSize
     *            preview size
     * @param name
     *            renderer name
     */
    public void setPreviewSize(Size previewSize, String name) {
        Log.i(TAG, "[setPreviewSize] +, previewSize:" + previewSize + ",name:" + name);

        Assert.assertNotNull(previewSize);
        Assert.assertNotNull(mEglHandler);

        RenderParameter para = new RenderParameter();
        para.mName = name;
        para.mPreviewSize = previewSize;

        mEglHandler.obtainMessage(EglHandler.MSG_SET_PREVIEW_SIZE, para).sendToTarget();
        waitDone();
        Log.i(TAG, "[setPreviewSize]-");
    }

    /**
     * set preview surface, it will draw content to surface when preview frame
     * update.
     *
     * @param surface
     *            surface
     */
    public void setPreviewSurface(Surface surface) {
        Log.i(TAG, "[setPreviewSurface]+");

        Assert.assertNotNull(surface);
        Assert.assertNotNull(mEglHandler);

        mEglHandler.obtainMessage(EglHandler.MSG_SET_PREVIEW_SURFACE, surface).sendToTarget();
        waitDone();
        Log.i(TAG, "[setPreviewSurface]-");
    }

    /**
     * set capture size.
     *
     * @param captureSize
     *            capture size
     * @param name
     *            render name
     */
    public void setCaptureSize(Size captureSize, String name) {
        Assert.assertNotNull(mEglHandler);

        RenderParameter para = new RenderParameter();
        para.mName = name;
        para.mPictureSize = captureSize;

        mEglHandler.obtainMessage(EglHandler.MSG_SET_CAPTURE_SIZE, para).sendToTarget();
        waitDone();

        Log.i(TAG, "[setCaptureSize]-");
    }

    /**
     * Set picture surface ,this must be set before take picture.
     *
     * @param surface
     *            a surface used to receive picture buffer
     */
    public void setCaptureSurface(Surface surface) {
        Log.i(TAG, "setCaptureSurface surface:" + surface);
        mCaptureSurface = surface;
    }

    /**
     * set snapshot surface, this must be set befor do snapshot.
     *
     * @param surface
     *            surface
     */
    public void setSnapshotSurface(Surface surface) {
        Log.d(TAG, "setSnapshotSurface,surface:" + surface);
        mSnapshotSurface = surface;
    }

    /**
     * do snapshot, draw surface, then save image.
     */
    public void doSnapshot() {
        Log.d(TAG, "doSnapshot");

        Assert.assertNotNull(mEglHandler);

        mEglHandler.removeMessages(EglHandler.MSG_DO_SNAPSHOT);
        mEglHandler.sendEmptyMessage(EglHandler.MSG_DO_SNAPSHOT);
        waitDone();
    }

    /**
     * get preview surface texture.
     *
     * @param name
     *            renderer name
     * @return surface texture
     */
    public SurfaceTexture getPreviewSurfaceTexture(String name) {
        Assert.assertNotNull(mEglHandler);

        return mEglHandler.getPreviewSurfaceTexture(name);
    }

    /**
     * get capture surface texture.
     *
     * @param name
     *            renderer name
     * @return capture surface texture
     */
    public SurfaceTexture getCaptureSurfaceTexture(String name) {
        Assert.assertNotNull(mEglHandler);

        return mEglHandler.getCaptureSurfaceTexture(name);
    }

    /**
     * set recording surface, draw content to surface when recording, must set
     * it before recording.
     *
     * @param surface
     *            surface
     */
    public void setRecordingSurface(Surface surface) {
        Log.i(TAG, "setRecordingSurface surfacee:" + surface);

        Assert.assertNotNull(mEglHandler);
        Assert.assertNotNull(surface);

        mEglHandler.obtainMessage(EglHandler.MSG_SET_RECORDING_SURFACE, surface).sendToTarget();
        waitDone();
    }

    /**
     * prepare recording, call this to create recording renderer.
     */
    public void prepareRecording() {
        Log.i(TAG, "[prepareRecording]+ mRecorderRenderer:" + mRecorderRenderer + " mEglHandler:"
                + mEglHandler);
        Assert.assertNotNull(mEglHandler);

        if (mRecorderRenderer == null) {
            mEglHandler.removeMessages(EglHandler.MSG_CREATE_RECORD_RENDER);
            mEglHandler.sendEmptyMessage(EglHandler.MSG_CREATE_RECORD_RENDER);
            waitDone();

            synchronized (mRenderLock) {
                if (mRecorderRenderer == null) {
                    try {
                        mRenderLock.wait();
                    } catch (InterruptedException e) {
                        Log.w(TAG, "unexpected interruption");
                    }
                }
            }
        }
        Log.i(TAG, "[prepareRecording]-");
    }

    /**
     * start recording.
     */
    public void startRecording() {
        Log.i(TAG, "[startRecording]+ mRecorderRenderer:" + mRecorderRenderer);

        Assert.assertNotNull(mRecorderRenderer);

        mRecorderRenderer.startRecording();
        Log.i(TAG, "[startRecording]-");
    }

    /**
     * stop recording.
     */
    public void stopRecording() {
        Log.i(TAG, "[stopRecording]+ mRecorderRenderer:" + mRecorderRenderer + " mEglHandler:"
                + mEglHandler);

        Assert.assertNotNull(mRecorderRenderer);
        Assert.assertNotNull(mEglHandler);

        mRecorderRenderer.stopRecording();
        mEglHandler.removeMessages(EglHandler.MSG_RELEASE_RECORD_RENDER);
        mEglHandler.sendEmptyMessage(EglHandler.MSG_RELEASE_RECORD_RENDER);
        waitDone();
        Log.i(TAG, "[stopRecording]-");
    }

    private void initializeEglThread() {
        synchronized (mRenderLock) {
            mEglThread = new HandlerThread("PreviewRealtimeRenderer");
            mEglThread.start();
            Looper looper = mEglThread.getLooper();
            if (looper == null) {
                throw new RuntimeException("why looper is null?");
            }
            mEglHandler = new EglHandler(looper);
        }
    }

    private void initializeEgl() {
        mEglHandler.post(new Runnable() {
            @Override
            public void run() {
                mEgl = (EGL10) EGLContext.getEGL();
                mEglDisplay = mEgl.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY);
                if (mEglDisplay == EGL10.EGL_NO_DISPLAY) {
                    throw new RuntimeException("eglGetDisplay failed");
                }
                int[] version = new int[2];
                if (!mEgl.eglInitialize(mEglDisplay, version)) {
                    throw new RuntimeException("eglInitialize failed");
                } else {
                    Log.v(TAG, "<initialize> EGL version: " + version[0] + '.' + version[1]);
                }
                int[] attribList = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE };
                mEglConfig = EGLConfigWrapper.getInstance().getEGLConfigChooser()
                        .chooseConfig(mEgl, mEglDisplay);
                mEglContext = mEgl.eglCreateContext(mEglDisplay, mEglConfig, EGL10.EGL_NO_CONTEXT,
                        attribList);
                if (mEglContext == null || mEglContext == EGL10.EGL_NO_CONTEXT) {
                    throw new RuntimeException("failed to createContext");
                } else {
                    Log.v(TAG, "<initialize> EGL context: create success");
                }
                mEglSurface = mEgl.eglCreatePbufferSurface(mEglDisplay, mEglConfig, null);
                if (mEglSurface == null || mEglSurface == EGL10.EGL_NO_SURFACE) {
                    Log.i(TAG, "createWindowSurface error eglError = " + mEgl.eglGetError());
                    throw new RuntimeException("failed to createWindowSurface mEglSurface = "
                            + mEglSurface + " EGL_NO_SURFACE = " + EGL10.EGL_NO_SURFACE);
                } else {
                    Log.v(TAG, "<initialize> EGL surface: create success");
                }
                if (!mEgl.eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext)) {
                    throw new RuntimeException("failed to eglMakeCurrent");
                } else {
                    Log.v(TAG, "<initialize> EGL make current: success");
                }
            }
        });
    }

    // this method must be called in GL Thread
    private void releaseEgl() {
        Log.i(TAG, "[releaseEgl]+");
        mEgl.eglDestroySurface(mEglDisplay, mEglSurface);
        mEgl.eglDestroyContext(mEglDisplay, mEglContext);
        mEgl.eglMakeCurrent(mEglDisplay, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_SURFACE,
                EGL10.EGL_NO_CONTEXT);
        mEgl.eglTerminate(mEglDisplay);
        mEglSurface = null;
        mEglContext = null;
        mEglDisplay = null;
        Log.i(TAG, "[releaseEgl]-");
    }

    // return false if cancelled.
    private boolean waitDone() {
        final Object waitDoneLock = new Object();
        final Runnable unlockRunnable = new Runnable() {
            @Override
            public void run() {
                synchronized (waitDoneLock) {
                    waitDoneLock.notifyAll();
                }
            }
        };

        synchronized (waitDoneLock) {
            mEglHandler.post(unlockRunnable);
            try {
                waitDoneLock.wait();
            } catch (InterruptedException ex) {
                Log.v(TAG, "waitDone interrupted");
                return false;
            }
        }
        return true;
    }

    /**
     * EglHandler.
     *
     */
    private class EglHandler extends Handler {

        public static final int MSG_INITIALIZE = 1;
        public static final int MSG_CREATE_RECORD_RENDER = 5;
        public static final int MSG_RELEASE_RECORD_RENDER = 6;
        public static final int MSG_SET_RENDERER_BITMAP = 7;
        public static final int MSG_DO_SNAPSHOT = 8;
        public static final int MSG_ADD_RENDERERS = 11;
        public static final int MSG_DELETE_RENDERERS = 19;
        public static final int MSG_SET_PREVIEW_SIZE = 12;
        public static final int MSG_SET_PREVIEW_SURFACE = 13;
        public static final int MSG_SET_RECORDING_SURFACE = 14;
        public static final int MSG_SET_CAPTURE_SIZE = 15;
        public static final int MSG_NEW_PICTURE_FRAME_ARRIVED = 16;
        public static final int MSG_NEW_PREVIEW_FRAME_ARRIVED = 17;
        public static final int MSG_RELEASE = 10;
        private static final int INTERVALS = 300;

        private FrameBuffer mPreviewFrameBuffer;
        private FrameBuffer mPictureFb;

        List<Renderer> mRenderArray = new ArrayList<Renderer>();

        private ScreenRenderer mScreenRenderer; // for PV
        private SnapshotRenderer mSnapshotRenderer; // for CAP/VSS

        private Handler mSurfaceTextureHandler;
        private HandlerThread mStFrameThread = new HandlerThread("StFrameListener");

        private long mTimeStamp = 0L;
        private long mStartTime = 0L;

        private int mRendererTexWidth = -1;
        private int mRendererTexHeight = -1;
        private int mFrameCount = 0;
        private boolean mBlockingForPvSizeChange = false;

        /**
         * constructor.
         *
         * @param looper
         *            thread looper
         */
        public EglHandler(Looper looper) {
            super(looper);
        }

        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
            case MSG_INITIALIZE:
                initialize();
                break;
            case MSG_RELEASE:
                doRelease();
                break;
            case MSG_ADD_RENDERERS: {
                RendererConfig config = (RendererConfig) msg.obj;
                addRenderer(config);
                break;
            }
            case MSG_DELETE_RENDERERS:
                String name = (String) msg.obj;
                deleteRenderer(name);
                break;
            case MSG_SET_PREVIEW_SIZE: {
                RenderParameter para = (RenderParameter) msg.obj;
                setPreviewSize(para.mPreviewSize, para.mName);
                break;
            }
            case MSG_SET_CAPTURE_SIZE: {
                RenderParameter para = (RenderParameter) msg.obj;
                setCaptureSize(para.mPictureSize, para.mName);
                break;
            }
            case MSG_SET_PREVIEW_SURFACE:
                setPreviewSurface((Surface) msg.obj);
                break;
            case MSG_SET_RECORDING_SURFACE:
                setRecordingSurface((Surface) msg.obj);
                break;
            case MSG_NEW_PREVIEW_FRAME_ARRIVED:
                SurfaceTextureWrapper stPreviewWrapper = (SurfaceTextureWrapper) msg.obj;
                stPreviewWrapper.updateTexImage();
                draw();
                break;
            case MSG_NEW_PICTURE_FRAME_ARRIVED:
                SurfaceTextureWrapper stPicWrapper = (SurfaceTextureWrapper) msg.obj;
                stPicWrapper.updateTexImage();
                tryTakePicutre();
                break;
            case MSG_CREATE_RECORD_RENDER:
                createRecordRenderer();
                break;
            case MSG_RELEASE_RECORD_RENDER:
                releaseRecordRenderer();
                break;
            case MSG_SET_RENDERER_BITMAP: {
                setRenderBitmap((RenderParameter) msg.obj);
                break;
            }
            case MSG_DO_SNAPSHOT:
                doSnapshot();
                break;
            default:
                break;
            }
        }

        /**
         * get preview surface texture.
         *
         * @param name
         *            renderer name
         * @return surface texture
         */
        public SurfaceTexture getPreviewSurfaceTexture(String name) {
            Log.d(TAG, "<getPreviewSurfaceTexture> name:" + name + ",size:" + mRenderArray.size());

            synchronized (mRenderLock) {
                for (Renderer render : mRenderArray) {
                    RendererConfig config = render.getConfig();
                    if (name.equals(config.getName())) {
                        if (config.getType() == RendererType.RENDERER_ALPHA_VIDEO
                                || config.getType() == RendererType.RENDERER_PREVIRE) {
                            if (render.getPreviewStWrapper() == null) {
                                try {
                                    mRenderLock.wait();
                                } catch (InterruptedException e) {
                                    Log.w(TAG, "unexpected interruption");
                                }
                            }
                            mBlockingForPvSizeChange = false;
                            return render.getPreviewStWrapper().getSurfaceTexture();
                        } else {
                            Assert.assertTrue(false);
                        }
                    }
                }
            }
            Assert.assertTrue(false);
            return null;
        }

        /**
         * get capture surface texture.
         *
         * @param name
         *            renderer name
         * @return surface texture
         */
        public SurfaceTexture getCaptureSurfaceTexture(String name) {
            Log.d(TAG, "<getCaptureSurfaceTexture> name:" + name);
            synchronized (mRenderLock) {
                for (Renderer render : mRenderArray) {
                    RendererConfig config = render.getConfig();
                    if (name.equals(config.getName())) {
                        if (config.getType() == RendererType.RENDERER_ALPHA_VIDEO
                                || config.getType() == RendererType.RENDERER_PREVIRE) {
                            if (render.getCaptureWrapper() == null) {
                                Assert.assertTrue(false);
                            }
                            return render.getCaptureWrapper().getSurfaceTexture();
                        } else {
                            Assert.assertTrue(false);
                        }
                    }
                }
            }
            Assert.assertTrue(false);
            return null;
        }

        /**
         * set renderer show region.
         *
         * @param graphicRect
         * @param name
         */
        public void setRenderRect(AnimationRect graphicRect, String name) {
            Log.d(TAG, "<setRenderRect> name:" + name + ",rect:" + graphicRect.getRectF());

            synchronized (mRenderLock) {
                for (Renderer render : mRenderArray) {
                    RendererConfig config = render.getConfig();
                    if (name.equals(config.getName())) {
                        render.setRenderRect(graphicRect);
                        return;
                    }
                }
                Assert.assertTrue(false);
            }
        }

        /**
         * judge if exist renderer.
         *
         * @param name
         *            renderer name
         * @return exist->true,not exist -> false
         */
        public boolean isContainsRender(String name) {
            Log.d(TAG, "<isContainsRender> name:" + name);
            synchronized (mRenderLock) {
                for (Renderer render : mRenderArray) {
                    RendererConfig config = render.getConfig();
                    if (name.equals(config.getName())) {
                        Log.d(TAG, "<isContainsRender> find render, name:" + name);
                        return true;
                    }
                }
                Log.d(TAG, "<isContainsRender> can't find render, name:" + name);
                return false;
            }
        }

        /**
         * update orientation when orientation changed.
         *
         * @param orientation
         */
        public void updateOrientation(int orientation) {
            for (Renderer render : mRenderArray) {
                render.updateOrientation(orientation);
            }
        }

        private void addRenderer(RendererConfig renderConfig) {
            Log.i(TAG, "[addRenderer]+ name = " + renderConfig.getName() + ",type:"
                    + renderConfig.getType()
                    + ",mRenderArray_size:" + mRenderArray.size());

            // check exit same name
            for (Renderer render : mRenderArray) {
                RendererConfig config = render.getConfig();
                if (renderConfig.getName().equals(config.getName())) {
                    Log.d(TAG, "exit same name, add fail!!!!");
                    return;
                }
            }

            // create new type
            synchronized (mRenderLock) {
                Renderer render;
                if (renderConfig.getType() == RendererType.RENDERER_PREVIRE) {
                    render = new GraphicRenderer(mActivity, renderConfig);
                } else {
                    Assert.assertTrue(false);
                    render = new GraphicRenderer(mActivity, renderConfig);
                }

                // draw preview that not need rotate before preview that need
                // rotate
                if (render.getConfig().getIsNeedRotate()) {
                    mRenderArray.add(0, render);
                } else {
                    mRenderArray.add(render);
                }
            }
            Log.i(TAG, "<createRenderers> success, size:" + mRenderArray.size());
        }

        private void deleteRenderer(String name) {
            synchronized (mRenderLock) {
                for (Renderer render : mRenderArray) {
                    RendererConfig config = render.getConfig();
                    if (name.equals(config.getName())) {
                        render.release();
                        mRenderArray.remove(render);

                        Log.d(TAG, "<deleteRenderer> delete success, size:" + mRenderArray.size());
                        return;
                    }
                }
                Log.d(TAG, "not exist this render, delete fail!!!");
                Assert.assertTrue(false);
            }
        }

        private void initialize() {
            Log.i(TAG, "[initialize]+ mSurfaceTextureHandler = " + mSurfaceTextureHandler);
            synchronized (mRenderLock) {
                if (mSurfaceTextureHandler == null) {
                    // frame availabe thread
                    if (!mStFrameThread.isAlive()) {
                        mStFrameThread.start();
                    }
                    mSurfaceTextureHandler = new Handler(mStFrameThread.getLooper());
                    // preview fbo
                    mPreviewFrameBuffer = new FrameBuffer();
                    mPreviewFrameBuffer.init();

                    // screen preview renderer
                    mScreenRenderer = new ScreenRenderer(mActivity, new RendererConfig());
                    mScreenRenderer.init();

                    // snapshot renderer
                    mSnapshotRenderer = new SnapshotRenderer(mActivity, new RendererConfig());
                    mSnapshotRenderer.init();
                }
                mRenderLock.notifyAll();
            }
            Log.i(TAG, "[initialize]-");
        }

        private void setPreviewSize(Size renderSize, String name) {
            Log.d(TAG, "<setPreviewSize> renderSize:" + renderSize + ",name:" + name);

            for (Renderer render : mRenderArray) {
                RendererConfig config = render.getConfig();
                if (name.equals(config.getName())) {

                    int width = renderSize.getWidth();
                    int height = renderSize.getHeight();
                    render.setRendererSize(width, height, true);

                    if (config.getType() == RendererType.RENDERER_ALPHA_VIDEO
                            || config.getType() == RendererType.RENDERER_PREVIRE) {
                        SurfaceTextureWrapper stWrapper = render.getPreviewStWrapper();
                        if (stWrapper == null) {
                            stWrapper = new SurfaceTextureWrapper(config.getName());
                            stWrapper.setDefaultBufferSize(width, height);
                            FrameAvailableListener listener = new FrameAvailableListener();
                            listener.init(stWrapper, MSG_NEW_PREVIEW_FRAME_ARRIVED);
                            stWrapper.setOnFrameAvailableListener(listener, mSurfaceTextureHandler);
                            render.setPreviewStWrapper(stWrapper);
                            Log.d(TAG, "<setPreviewSize> set new Preview StWrapper:" + stWrapper);
                        } else if (stWrapper.getWidth() != width ||
                                stWrapper.getHeight() != height) {
                            stWrapper.setDefaultBufferSize(width, height);
                        } else {
                            Log.d(TAG, "same width and height");
                        }
                    }
                }
            }

            Renderer render = mRenderArray.get(0);
            SurfaceTextureWrapper stWrapper = render.getPreviewStWrapper();
            int width = stWrapper.getWidth();
            int height = stWrapper.getHeight();
            if (mRendererTexWidth != width || mRendererTexHeight != height) {
                mRendererTexWidth = width;
                mRendererTexHeight = height;
                if (mPreviewFrameBuffer != null) {
                    mPreviewFrameBuffer.setRendererSize(mRendererTexWidth, mRendererTexHeight);
                }
                if (mScreenRenderer != null) {
                    mScreenRenderer.setRendererSize(mRendererTexWidth, mRendererTexHeight);
                }
            }
            Log.i(TAG, "[setPreviewSize] end");
        }

        private void setCaptureSize(Size captureSize, String name) {
            Log.i(TAG, "setCaptureSize, captureSize:" + captureSize + ",name:" + name);

            for (Renderer render : mRenderArray) {
                RendererConfig config = render.getConfig();
                if (name.equals(config.getName())) {
                    if (config.getType() == RendererType.RENDERER_PREVIRE
                            || config.getType() == RendererType.RENDERER_ALPHA_VIDEO) {
                        int width = captureSize.getWidth();
                        int height = captureSize.getHeight();
                        if (render.getCaptureWrapper() == null) {
                            SurfaceTextureWrapper stWrapper = new SurfaceTextureWrapper(config
                                    .getName());
                            stWrapper.setDefaultBufferSize(width, height);
                            FrameAvailableListener frameAvailableListener =
                                    new FrameAvailableListener();
                            frameAvailableListener.init(stWrapper, MSG_NEW_PICTURE_FRAME_ARRIVED);
                            stWrapper.setOnFrameAvailableListener(frameAvailableListener,
                                    mSurfaceTextureHandler);
                            render.setCaptureStWrapper(stWrapper);
                        } else {
                            render.getCaptureWrapper().setDefaultBufferSize(width, height);
                        }
                        return;
                    } else {
                        Assert.assertTrue(false);
                    }
                }
            }
            Assert.assertTrue(false);
        }

        private void setPreviewSurface(Surface surface) {
            if (mScreenRenderer != null) {
                mScreenRenderer.setSurface(surface);
                mNeedNotifyFirstFrameForSurfaceChanged = true;
            }
        }

        private void releaseRenderers() {
            Log.i(TAG, "[releaseRenderers]+");
            synchronized (mRenderLock) {
                // release frame buffer
                if (mPreviewFrameBuffer != null) {
                    mPreviewFrameBuffer.unInit();
                    mPreviewFrameBuffer = null;
                }
                if (mPictureFb != null) {
                    mPictureFb.unInit();
                    mPictureFb = null;
                }

                for (Renderer render : mRenderArray) {
                    render.release();
                }
                mRenderArray.clear();

                if (mSnapshotRenderer != null) {
                    mSnapshotRenderer.release();
                    mSnapshotRenderer = null;
                }
                if (mRecorderRenderer != null) {
                    mRecorderRenderer.releaseSurface();
                    mRecorderRenderer = null;
                }
                if (mScreenRenderer != null) {
                    mScreenRenderer.release();
                    mScreenRenderer = null;
                }
                mRenderLock.notifyAll();
            }
            Log.i(TAG, "[releaseRenderers]-");
        }

        private void createRecordRenderer() {
            synchronized (mRenderLock) {
                mRecorderRenderer = new RecorderRenderer(mActivity);
                mRecorderRenderer.init();
                mRenderLock.notifyAll();
            }
        }

        private void setRecordingSurface(Surface recordingSurface) {
            synchronized (mRenderLock) {
                if (mRecorderRenderer == null) {
                    throw new IllegalStateException("Before update record surface, "
                            + "please call prepareRecording firstly!");
                }
                mRecorderRenderer.setRecrodingSurface(recordingSurface, true);
                mRenderLock.notifyAll();
            }
        }

        private void releaseRecordRenderer() {
            if (mRecorderRenderer != null) {
                mRecorderRenderer.releaseSurface();
                mRecorderRenderer = null;
            }
        }

        private void doRelease() {
            Log.i(TAG, "[doRelease]+");

            releaseRenderers();
            releaseEgl();

            removeMessages(MSG_NEW_PREVIEW_FRAME_ARRIVED);
            removeMessages(MSG_NEW_PICTURE_FRAME_ARRIVED);

            mSurfaceTextureHandler = null;
            mStFrameThread = null;
            mRendererTexWidth = -1;
            mRendererTexHeight = -1;

            Log.i(TAG, "[doRelease]-");
        }

        // use same frame rate with first preview render
        private boolean doTimestampSync() {
            SurfaceTextureWrapper wrapper = null;
            for (Renderer render : mRenderArray) {
                RendererConfig config = render.getConfig();
                if (config.getType() == RendererType.RENDERER_PREVIRE
                        || config.getType() == RendererType.RENDERER_ALPHA_VIDEO) {
                    wrapper = render.getPreviewStWrapper();
                    if (wrapper.isNewTimeStamp()) {
                        wrapper.updateTimeStamp();
                        mTimeStamp = wrapper.getTimeStamp();
                        return true;
                    } else {
                        return false;
                    }
                }
            }
            return true;
        }

        private void draw() {
            synchronized (mRenderLock) {
                AnimationTime.update();
                if (doTimestampSync() && !mBlockingForPvSizeChange) {
                    mPreviewFrameBuffer.setupFrameBufferGraphics(mRendererTexWidth,
                            mRendererTexHeight);
                    for (Renderer render : mRenderArray) {
                        RendererConfig config = render.getConfig();
                        if (config.getType() == RendererType.RENDERER_PREVIRE
                                || config.getType() == RendererType.RENDERER_ALPHA_VIDEO) {
                            SurfaceTextureWrapper wrapper = render.getPreviewStWrapper();
                            if (wrapper != null) {
                                render.draw(wrapper.getTextureId(),
                                        wrapper.getBufferTransformMatrix(),
                                        GLUtil.createIdentityMtx(), render.getRenderRect(),
                                        mCurrentOrientation,
                                        false);
                            }
                        } else if (config.getType() == RendererType.RENDERER_IMAGE) {
                            render.draw(render.getImageTextureId(), null,
                                    GLUtil.createIdentityMtx(), render.getRenderRect(),
                                    mCurrentOrientation, false);
                        }
                    }

                    mPreviewFrameBuffer.setScreenBufferGraphics();

                    // ensure time stamp will not backup, always choose one
                    // camera's buffer time stamp
                    if (mRecorderRenderer != null) {
                        mRecorderRenderer.draw(mPreviewFrameBuffer.getFboTexId(), mTimeStamp);
                    }

                    if (mNeedNotifyFirstFrameForSurfaceChanged) {
                        mRendererCallback.onFristFrameAvailable(mTimeStamp);
                        mNeedNotifyFirstFrameForSurfaceChanged = false;
                    }
                    // draw to screen
                    mScreenRenderer.draw(mPreviewFrameBuffer.getFboTexId());

                    // frame rate.
                    mFrameCount++;
                    if (mFrameCount % INTERVALS == 0) {
                        long currentTime = System.currentTimeMillis();
                        int intervals = (int) (currentTime - mStartTime);
                        double fps = (mFrameCount * MINISECOND_PER_SECOND) / intervals;
                        Log.i(TAG, "[Preview] Drawing frame, fps = " + fps
                                + " in last " + intervals + " millisecond.");
                        mStartTime = currentTime;
                        mFrameCount = 0;
                    }
                }
            }
        }

        private void doSnapshot() {
            Log.d(TAG, "<doSnapshot> width:" + mRendererTexWidth + ",height:" + mRendererTexHeight);

            mSnapshotRenderer.setRendererSize(mRendererTexWidth, mRendererTexHeight, true);
            mSnapshotRenderer.setCaptureSurface(mSnapshotSurface);
            mPreviewFrameBuffer.setupFrameBufferGraphics(mRendererTexWidth, mRendererTexHeight);

            for (Renderer render : mRenderArray) {
                RendererConfig config = render.getConfig();
                if (config.getType() == RendererType.RENDERER_PREVIRE
                        || config.getType() == RendererType.RENDERER_ALPHA_VIDEO) {
                    SurfaceTextureWrapper wrapper = render.getPreviewStWrapper();
                    if (wrapper != null) {
                        render.draw(wrapper.getTextureId(), wrapper.getBufferTransformMatrix(),
                                GLUtil.createIdentityMtx(), render.getRenderRect(),
                                mCurrentOrientation, false);
                    }
                } else if (config.getType() == RendererType.RENDERER_IMAGE) {
                    render.draw(render.getImageTextureId(), null, GLUtil.createIdentityMtx(),
                            render.getRenderRect(), mCurrentOrientation, false);
                }
            }

            mPreviewFrameBuffer.setScreenBufferGraphics();

            mSnapshotRenderer.draw(mPreviewFrameBuffer.getFboTexId());
        }

        private void tryTakePicutre() {

        }

        /**
         *
         * FrameAvailableListener.
         *
         */
        private class FrameAvailableListener implements SurfaceTexture.OnFrameAvailableListener {
            private SurfaceTextureWrapper mStWrapper = null;
            private int mMsg = MSG_NEW_PREVIEW_FRAME_ARRIVED;

            @Override
            public void onFrameAvailable(SurfaceTexture surfaceTexture) {
                if (mStWrapper != null && mEglHandler != null) {
                    mEglHandler.obtainMessage(mMsg, mStWrapper).sendToTarget();
                }
            }

            public void init(SurfaceTextureWrapper stWrapper, int msg) {
                mStWrapper = stWrapper;
                mMsg = msg;
            }
        }

        private int initBitmapTexture(Bitmap bitmap) {
            int[] textures = new int[1];
            Log.i(TAG, "Renderer initBitmapTexture glGenTextures num = " + 1);
            GLES20.glGenTextures(1, textures, 0);
            int textureId = textures[0];
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId);
            GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER,
                    GLES20.GL_LINEAR);
            GLUtil.checkGlError("initBitmapTexture GL_TEXTURE_MAG_FILTER");
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER,
                    GLES20.GL_LINEAR_MIPMAP_NEAREST);
            GLUtil.checkGlError("initBitmapTexture GL_TEXTURE_MIN_FILTER");
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S,
                    GLES20.GL_CLAMP_TO_EDGE);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T,
                    GLES20.GL_CLAMP_TO_EDGE);

            bitmap.setHasAlpha(false);

            GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0);
            bitmap.recycle();
            bitmap = null;
            GLES20.glGenerateMipmap(GLES20.GL_TEXTURE_2D);
            return textureId;
        }

        private void setRenderBitmap(RenderParameter para) {
            int imageTexId = initBitmapTexture(para.mBitmap);
            for (Renderer render : mRenderArray) {
                RendererConfig config = render.getConfig();
                if (para.mName.equals(config.getName())) {
                    if (config.getType() == RendererType.RENDERER_IMAGE) {
                        render.setImageTextureId(imageTexId);
                        return;
                    } else {
                        Assert.assertTrue(false);
                    }
                }
            }
            Assert.assertTrue(false);
        }
    }

    /**
     * use as message parameter.
     */
    private class RenderParameter {
        public RendererType mType;
        public String mName;
        public Bitmap mBitmap;
        public Size mPreviewSize;
        public Size mPictureSize;
    }
}
