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
package com.mediatek.sourcecamera;

import com.mediatek.sourcecamera.R;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.BroadcastReceiver;
import android.content.ContentResolver;
import android.content.ContentValues;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CameraMetadata;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureResult;
import android.hardware.camera2.TotalCaptureResult;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.Image;
import android.media.ImageReader;
import android.net.Uri;
import android.graphics.ImageFormat;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;
import android.os.UserHandle;
import android.provider.MediaStore.Images;
import android.provider.MediaStore.Images.ImageColumns;
import android.util.Log;
import android.util.Size;
import android.view.Display;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

import com.mediatek.crossmount.adapter.CrossMountCameraAdapter;
import com.mediatek.crossmount.adapter.CrossMountCameraAdapter.CallBack;

import java.io.File;
import java.io.IOException;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.nio.ByteBuffer;
import java.sql.Date;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.Locale;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.Semaphore;

/* A class that handles everything about source camera
 *  when it is linked with sink camera.
 *
 * The test cases:
 * (1) The source camera can preview. Link source camera by Wifi from sink camera.
 * (2) The source camera can take picture. Take a picture from sink camera in
 *     single preview, a picture can be taken with source camera preview.
 * (3) The source camera can exit actively. Touch back key when source camera is
 *     previewing, a dialog which needs user to confirm will show.
 *
 */

public class SourceCamera extends Activity implements SurfaceHolder.Callback {
    private static final String TAG = "SourceCamera";
    private static final int MSG_SINK_PREVIEW_CONTROL = 0;
    private static final int MSG_SINK_CAPTURE_STILL_PICTURE = 1;
    private static final String WFD_SURFACE_KEY = "surface";
    private static final String SUBFFIX_TAG = ".jpg.temp";
    private static final int CAPTURE_DONE = 2;

    // Receive broadcast for connected status with sink part
    private static final String CONNECT_STATUS_ACTION =
            "com.mediatek.crossmount.streaming.STATE_CHANGE";
    private static final int CROSSMOUNT_STREAMING_STATE_DISCONNECT = 0;
    private static final int CROSSMOUNT_STREAMING_STATE_CONNECT = 1;
    private static final String EXTRA_CONNECT_INFO = "connect_state";
    private static final String EXTRA_UNIQUE_ID = "unique_id";
    private SinkConnectStatusReceiver mSinkConnectStatusReceiver =
            new SinkConnectStatusReceiver();

    // Send broadcast for source app exit
    private static final String CROSSMOUNT_EXIT_ACTION =
            "com.mediatek.crossmount.ACTIVITY_STATE_EXIT";
    private static final String EXTRA_CAMERA_ACTIVITY_STATE = "camera_activity_status";
    private static final String EXTRA_CONNECT_TARGET = "connect_target";
    public static final int SAVE_FILE = 0;

    private CameraCaptureSession mCaptureSession;
    private CameraDevice mCameraDevice;
    private Size mPreviewSize;

    private CameraManager mCameraManager;
    private CaptureRequest.Builder mPreviewRequestBuilder;
    private CaptureRequest.Builder mCaptureBuilder;
    private CaptureRequest mPreviewRequest;
    private List<Size> mSupportedPreviewSize;
    private ImageReader mImageReader;
    private CaptureResult mCaptureResult;

    private View mCoverView;
    private SurfaceView mSurfaceView;
    private Surface mSurface;
    private Surface mWfdSurface;
    private Surface mImageSurface;

    private Semaphore mCameraOpenCloseLock = new Semaphore(1);
    private HandlerThread mBackgroundThread;
    private Handler mBackgroundHandler;
    private CameraHandler mCameraHandler;

    private Uri mUri;
    private String mSinkTarget;
    private int mSinkUniqueId = -1;
    private CrossMountCameraAdapter mCameraAdapter;
    private Long mCaptureTime;
    private String mCameraId;
    private static int mCurrentNum = 0;

    // debug info for preview result.
    private static final int INTERVALS = 300;
    private int mFrameCount = 0;
    private long mStartTime = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "onCreate");
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        setContentView(R.layout.source_preview);

        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

        mCoverView = (View) findViewById(R.id.camera_cover);
        mCoverView.bringToFront();
        mCameraManager = (CameraManager) getSystemService(Context.CAMERA_SERVICE);
        findOptionPreviewSize();
        mSurfaceView = (SurfaceView) findViewById(R.id.surface_view);
        SurfaceHolder mSurfaceHolder = mSurfaceView.getHolder();
        mSurfaceHolder.addCallback(this);
        mSurfaceHolder.setFixedSize(mPreviewSize.getWidth(), mPreviewSize.getHeight());
        mSinkTarget = getIntent().getStringExtra(EXTRA_CONNECT_TARGET);
        mSinkUniqueId = getIntent().getIntExtra(EXTRA_UNIQUE_ID, 0);
        mWfdSurface = (Surface) getIntent().getParcelableExtra(WFD_SURFACE_KEY);
        mCameraAdapter = new CrossMountCameraAdapter(this, mAdapterCallBack);
        registerReceiver(mSinkConnectStatusReceiver, new IntentFilter(
                CONNECT_STATUS_ACTION));
    }

    @Override
    protected void onResume() {
        Log.i(TAG, "onResume");
        super.onResume();
        startBackgroundThread();
        Log.i(TAG, "[onResume ]mSurfaceView = " + mSurfaceView);
        if (null == mSurfaceView) {
          mSurfaceView = (SurfaceView) findViewById(R.id.surface_view);
          SurfaceHolder mSurfaceHolder = mSurfaceView.getHolder();
          mSurfaceHolder.setFixedSize(mPreviewSize.getWidth(), mPreviewSize.getHeight());
        } else if (mSurfaceView.getVisibility() != View.VISIBLE) {
            Log.i(TAG, "[onResume ] SurfaceViewVisibility = " + mSurfaceView.getVisibility());
          mSurfaceView.setVisibility(View.VISIBLE);
        }
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    @Override
    protected void onPause() {
        Log.i(TAG, "onPause");
        super.onPause();
        showCoverView();
        closeCamera();
        stopBackgroundThread();
        if (null != mSurfaceView && mSurfaceView.getVisibility() == View.VISIBLE) {
            Log.i(TAG, "[onPause ] SurfaceViewVisibility = " + mSurfaceView.getVisibility());
          mSurfaceView.setVisibility(View.INVISIBLE);
        }
        if (null != getWindow()) {
           getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        }
    }

    @Override
    protected void onDestroy() {
        Log.i(TAG, "onDestroy");
        super.onDestroy();
        unregisterReceiver(mSinkConnectStatusReceiver);
        mCameraAdapter.releaseCallBack(mAdapterCallBack);
        if (null != mSurfaceView && mSurfaceView.getVisibility() == View.VISIBLE) {
            mSurfaceView.setVisibility(View.GONE);
        }
    }

    @Override
    public void onBackPressed() {
        Log.i(TAG, "onBackPressed");
        showExitDlg(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.i(TAG, "surfaceCreated");
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width,
            int height) {
        Log.i(TAG, "surfaceChanged");
        mSurface = holder.getSurface();
        openCamera();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.i(TAG, "surfaceDestroyed");
    }

    private class CameraHandler extends Handler {
        public CameraHandler(Looper looper) {
            super(looper);
        }
        @Override
        public void handleMessage(Message msg) {
            Log.i(TAG, "handleMessage msg.what = " + msg.what);
            switch (msg.what) {
            case MSG_SINK_PREVIEW_CONTROL:
                sinkControlPreview((CaptureRequest.Builder)msg.obj);
                break;

            case MSG_SINK_CAPTURE_STILL_PICTURE:
                captureStillPicture((CaptureRequest.Builder)msg.obj);
                break;

            default:
                break;
            }
        }
    }

    private void openCamera() {
        Log.i(TAG, "openCamera");
        setUpCameraOutputs();
        try {
            mCameraManager.openCamera(mCameraId, mStateCallback,
                    mCameraHandler);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }

    private void setUpCameraOutputs() {
        Log.i(TAG, "setUpCameraOutputs");
        try {
            CameraCharacteristics characteristics = mCameraManager
                    .getCameraCharacteristics(mCameraId);
            StreamConfigurationMap map = characteristics
                    .get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
            // TODO hardcode for image size for native issue now.
            mImageReader = ImageReader.newInstance(1920, 1088,
                    ImageFormat.JPEG, /* maxImages */2);
            mImageReader.setOnImageAvailableListener(mOnImageAvailableListener,
                    mCameraHandler);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        } catch (NullPointerException e) {
            e.printStackTrace();
        }
    }

    private final CameraDevice.StateCallback mStateCallback = new CameraDevice.StateCallback() {
        @Override
        public void onOpened(CameraDevice cameraDevice) {
            // This method is called when the camera is opened. We start camera
            // preview here.
            Log.i(TAG, "onOpened");
            mCameraOpenCloseLock.release();
            mCameraDevice = cameraDevice;
            createCameraCaptureSession();
        }

        @Override
        public void onDisconnected(CameraDevice cameraDevice) {
            Log.i(TAG, "onDisconnected");
            mCameraOpenCloseLock.release();
            cameraDevice.close();
            mCameraDevice = null;
        }

        @Override
        public void onError(CameraDevice cameraDevice, int error) {
            Log.i(TAG, "onError");
            mCameraOpenCloseLock.release();
            cameraDevice.close();
            mCameraDevice = null;
        }
    };

    /**
     * Creates a new CameraCaptureSession for camera preview.
     */
    private void createCameraCaptureSession() {
        Log.i(TAG, "createCameraCaptureSession");
        try {
            List<Surface> outputSurfaces = new ArrayList<Surface>();
            configuringSessionOutputs(outputSurfaces);
            mCameraDevice.createCaptureSession(outputSurfaces,
                    mSessionCallback, mCameraHandler);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }

    private void configuringSessionOutputs(List<Surface> outputSurfaces) {
        Log.i(TAG, "configuringSessionOutputs");
        mImageSurface = mImageReader.getSurface();
        outputSurfaces.add(mSurface);
        if (mImageSurface != null && mImageSurface.isValid()) {
            outputSurfaces.add(mImageReader.getSurface());
        }
        if (mWfdSurface != null && mWfdSurface.isValid()) {
            outputSurfaces.add(mWfdSurface);
        }
    }

    private CameraCaptureSession.StateCallback mSessionCallback
                   = new CameraCaptureSession.StateCallback() {
        @Override
        public void onConfigured(CameraCaptureSession cameraCaptureSession) {
            Log.i(TAG, "onConfigured mCameraDevice = " + mCameraDevice);
            // The camera is already closed
            if (null == mCameraDevice) {
                return;
            }
            mCaptureSession = cameraCaptureSession;
            try {
                // Finally, we start displaying the camera preview.
                Log.i(TAG, "configuringSessionRequest mPreviewRequestBuilder= "
                + mPreviewRequestBuilder);
                if (null == mPreviewRequestBuilder) {
                    try {
                        mPreviewRequestBuilder = mCameraDevice
                                .createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
                    } catch (CameraAccessException e) {
                        e.printStackTrace();
                    }
                }
                mPreviewRequestBuilder.addTarget(mSurface);
                if (mWfdSurface != null && mWfdSurface.isValid()) {
                    mPreviewRequestBuilder.addTarget(mWfdSurface);
                }
                // Auto focus should be continuous for camera preview.
                mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AF_MODE,
                        CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE);
                // Flash is automatically enabled when necessary.
                mPreviewRequestBuilder.set(CaptureRequest.CONTROL_AE_MODE,
                        CaptureRequest.CONTROL_AE_MODE_ON_AUTO_FLASH);
                mPreviewRequest = mPreviewRequestBuilder.build();
                mCaptureSession.setRepeatingRequest(mPreviewRequest,
                        mPreviewCallback, mCameraHandler);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        hideCoverView();
                    }
                });
            } catch (CameraAccessException e) {
                e.printStackTrace();
            }
        }

        @Override
        public void onConfigureFailed(CameraCaptureSession cameraCaptureSession) {
            Log.i(TAG, "onConfigureFailed");
        }
    };

    /**
     * A CameraCaptureSession.CaptureCallback that handles events related to
     * JPEG capture.
     */
    private CameraCaptureSession.CaptureCallback mPreviewCallback
                    = new CameraCaptureSession.CaptureCallback() {
        @Override
        public void onCaptureProgressed(CameraCaptureSession session,
                CaptureRequest request, CaptureResult partialResult) {
        }

        @Override
        public void onCaptureCompleted(CameraCaptureSession session,
                CaptureRequest request, TotalCaptureResult result) {
            mFrameCount++;
            if (mFrameCount % INTERVALS == 0) {
                long currentTime = System.currentTimeMillis();
                int intervals = (int) (currentTime - mStartTime);
                Log.i(TAG,
                        "[PreviewCallback] fps = "
                                + (mFrameCount * 1000.0f) / intervals
                                + " in last " + intervals + " millisecond.");
                mStartTime = currentTime;
                mFrameCount = 0;
            }
            mCameraAdapter.sendRepeatResult(result);
        }
    };

    private CrossMountCameraAdapter.CallBack mAdapterCallBack =
            new CrossMountCameraAdapter.CallBack() {
        @Override
        public void onRepeatRequest(CaptureRequest.Builder builder) {
            Log.i(TAG, "onRepeatRequest builder = " + builder.isEmpty());
            if (!builder.isEmpty()) {
                mCameraHandler.obtainMessage(MSG_SINK_PREVIEW_CONTROL, builder).sendToTarget();
            }
        }

        @Override
        public void onNonRepeatRequest(CaptureRequest.Builder builder,
                int[] imageInfo) {
            Log.i(TAG, "onNonRepeatRequest builder = " + builder.isEmpty());
            for (int i = 0; null != imageInfo && i < imageInfo.length; i++) {
                Log.i(TAG, "onNonRepeatRequest imageInfo[" + i + "] = " + imageInfo[i]);
            }
            mCameraHandler.obtainMessage(MSG_SINK_CAPTURE_STILL_PICTURE, builder).sendToTarget();
        }
    };

    private void sinkControlPreview(CaptureRequest.Builder builder) {
        Log.i(TAG, "sinkControlPreview");
        try {
            if (null == mCameraDevice) {
                return;
            }
            mPreviewRequestBuilder = builder;
            mPreviewRequestBuilder.addTarget(mSurface);
            if (null != mWfdSurface && mWfdSurface.isValid()) {
                mPreviewRequestBuilder.addTarget(mWfdSurface);
            }
            mPreviewRequest = mPreviewRequestBuilder.build();
            if (null != mCaptureSession) {
                mCaptureSession.setRepeatingRequest(mPreviewRequest,
                        mPreviewCallback, mCameraHandler);
            }
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }

    private void captureStillPicture(CaptureRequest.Builder builder) {
        Log.i(TAG, "captureStillPicture");
        try {
            if (null == mCameraDevice) {
                return;
            }
            mCaptureTime = System.currentTimeMillis();
            mCaptureBuilder = builder;
            mCaptureBuilder.addTarget(mImageSurface);
            mCaptureSession.capture(mCaptureBuilder.build(), mCaptureCallback,
                    mCameraHandler);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }

    /**
     * This a callback object for the ImageReader. "onImageAvailable"
     * will be called when a still image is ready to be saved.
     */
    private final ImageReader.OnImageAvailableListener mOnImageAvailableListener =
            new ImageReader.OnImageAvailableListener() {
        @Override
        public void onImageAvailable(ImageReader reader) {
            Log.i(TAG, "onImageAvailable");
            String fileName = generateName(mCaptureTime);
            String filePath = Util.generateFilepath(fileName);
            File file = new File(filePath);
            SaveFile(reader.acquireNextImage(), file);
            mUri = Uri.fromFile(file);
            notifyCaptureDataDone();
        }
    };

    private CameraCaptureSession.CaptureCallback mCaptureCallback =
            new CameraCaptureSession.CaptureCallback() {
        @Override
        public void onCaptureCompleted(CameraCaptureSession session,
                CaptureRequest request, TotalCaptureResult result) {
            Log.i(TAG, "CaptureCallback onCaptureCompleted");
            mCaptureResult = result;
            notifyCaptureDataDone();
        }
    };

    private void notifyCaptureDataDone() {
        Log.i(TAG, "notifyCaptureDataDone");
        mCurrentNum++;
        if (mCurrentNum == CAPTURE_DONE && mUri != null && mCaptureResult != null) {
            mCameraAdapter.sendNonRepeatResult(mUri, mCaptureResult);
            mCurrentNum = 0;
            mUri = null;
            mCaptureResult = null;
        }
    }

    private void closeCamera() {
        Log.i(TAG, "closeCamera");
        if (null != mCaptureSession) {
            mCaptureSession.close();
            mCaptureSession = null;
        }
        if (null != mCameraDevice) {
            mCameraDevice.close();
            mCameraDevice = null;
        }
        if (null != mImageReader) {
            mImageReader.close();
            mImageReader = null;
        }
    }

    private List<Size> getSupportedPreviewSize(
            CameraCharacteristics cameraCharacteristics) {
        StreamConfigurationMap s = cameraCharacteristics
                .get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
        Size[] sizes = s.getOutputSizes(SurfaceHolder.class);
        mSupportedPreviewSize = new ArrayList<Size>(sizes.length);
        for (Size size : sizes) {
            mSupportedPreviewSize.add(size);
        }
        return mSupportedPreviewSize;
    }

    // Connect status Receiver
    private class SinkConnectStatusReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            Log.i(TAG, "SinkConnectStatusReceiver");
            int connectInfo = intent.getIntExtra(EXTRA_CONNECT_INFO, 0);
            if (connectInfo == CROSSMOUNT_STREAMING_STATE_DISCONNECT) {
                if (!isFinishing()) {
                    finish();
                }
            }
        }
    }

    //TODO Test Save image performance.
    /**
     * Saves a JPEG into the specified File.
     */
    private class ImageSaverHandler extends Handler {
        public ImageSaverHandler(Looper looper) {
            super(looper);
        }

        @Override
        public void handleMessage(final Message msg) {
            Log.i(TAG, "handleMessage");
            switch (msg.what) {
            case SAVE_FILE:
//                SaveFile();
                break;

            default:
                break;
            }
        }
    }

    /**
     * Starts a background thread and its {@link Handler}.
     */
    private void startBackgroundThread() {
        Log.i(TAG, "startBackgroundThread");
        mBackgroundThread = new HandlerThread("CameraBackground");
        mBackgroundThread.start();
        mCameraHandler = new CameraHandler(mBackgroundThread.getLooper());
    }

    private void SaveFile(Image image, File file) {
        ByteBuffer buffer = image.getPlanes()[0].getBuffer();
        byte[] bytes = new byte[buffer.remaining()];
        buffer.get(bytes);
        FileOutputStream output = null;
        try {
            output = new FileOutputStream(file);
            output.write(bytes);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            Log.i(TAG, "ImageSaver done");
            image.close();
            if (null != output) {
                try {
                    output.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
    }

    /**
     * Stops the background thread and its Handler.
     */
    private void stopBackgroundThread() {
        Log.i(TAG, "stopBackgroundThread");
        mBackgroundThread.quitSafely();
        try {
            mBackgroundThread.join();
            mBackgroundThread = null;
            mCameraHandler = null;
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }

    private String generateName(long dateTaken) {
        Date date = new Date(dateTaken);
        String result = new SimpleDateFormat(
                getString(R.string.image_file_name_format), Locale.ENGLISH)
                .format(date);
        result += SUBFFIX_TAG;
        Log.i(TAG, "generateName result = " + result);
        return result;
    }

    private void showExitDlg(final Activity activity) {
        AlertDialog.Builder mAlertDlg = new AlertDialog.Builder(activity);
        DialogInterface.OnClickListener okListener
        = new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                Intent intent = new Intent(CROSSMOUNT_EXIT_ACTION);
                intent.putExtra(EXTRA_CAMERA_ACTIVITY_STATE,
                        CROSSMOUNT_STREAMING_STATE_DISCONNECT);
                intent.putExtra(EXTRA_UNIQUE_ID, mSinkUniqueId);
                sendBroadcastAsUser(intent, UserHandle.ALL);
                if (!activity.isFinishing()) {
                    activity.finish();
                }
            }
        };

        mAlertDlg
                .setMessage(activity
                        .getString(R.string.multicamera_disconnect_hint, mSinkTarget))
                .setTitle(R.string.multicamera_disconnect_dialog_title)
                .setPositiveButton(
                        R.string.multicamera_disconnect_dialog_positive,
                        okListener);
        DialogInterface.OnClickListener cancelListener
             = new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
            }
        };
        mAlertDlg
                .setNegativeButton(
                        R.string.multicamera_disconnect_dialog_negative,
                        cancelListener).show();
    }

    private void hideCoverView() {
        if (null != mCoverView && mCoverView.getVisibility() == View.VISIBLE) {
            mCoverView.setVisibility(View.GONE);
        }
    }

    private void showCoverView() {
        if (null != mCoverView && mCoverView.getVisibility() != View.VISIBLE) {
            mCoverView.setVisibility(View.VISIBLE);
        }
    }

    private void findOptionPreviewSize() {
        Log.i(TAG, "findOptionPreviewSize");
        try {
            for (String cameraId : mCameraManager.getCameraIdList()) {
                CameraCharacteristics characteristics = mCameraManager
                        .getCameraCharacteristics(cameraId);
                if (characteristics.get(CameraCharacteristics.LENS_FACING)
                        == CameraCharacteristics.LENS_FACING_FRONT) {
                    continue;
                }
                double targetRatio = Util.findFullscreenRatio(this);
                List<Size> supportedSizes = getSupportedPreviewSize(characteristics);
                mPreviewSize = Util.getOptimalPreviewSize(this, supportedSizes,
                        targetRatio);
                mCameraId = cameraId;
            }
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }
}