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
package com.mediatek.camera.v2.mode.multicamera;

import android.graphics.ImageFormat;
import android.graphics.PixelFormat;
import android.media.Image;
import android.media.ImageReader;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.util.Size;
import android.view.Surface;

import com.mediatek.camera.v2.stream.ImageInfo;


import junit.framework.Assert;

/**
 * this class used for create Cross Mount Multi Preview Snap shot and remote
 * camera single capture.
 */
public class MultiCameraCaptureHelper {

    private static final String TAG = MultiCameraCaptureHelper.class.getSimpleName();
    private static final int MAX_CAPTURE_IMAGES = 2;

    private int mSnapShotSurfaceWidth;
    private int mSnapShotSurfaceHeight;

    private ImageCallback mImageCallback;
    // Handler thread for camera-related operations.
    private HandlerThread mCaptureHandlerThread;
    private Handler mCaptureHandler;
    // Receives the normal captured images.
    private ImageReader mImageReader;
    private Surface mSnapShotSurface;

    // for Remote Camera create surface
    private Surface mRemoteSurface;
    private int mRemoteSurfaceWidth;
    private int mRemoteSurfaceHeight;
    private ImageReader mRemoteImageReader;

    /**
     * this interface used for notify current have image available.
     */
    public interface ImageCallback {
        /**
         * when image available will trigger this function.
         * @param imageInfo the info such as data,width,height and format.
         */
        public void onImageAvaliable(ImageInfo imageInfo);
    }

    /**
     * MultiCamera capture helper construct function.
     * current do noting.
     */
    public MultiCameraCaptureHelper() {
        // do -noting
    }

    /**
     * Will create a remote capture surface with the target size and format.
     * @param size
     *            the surface's size.
     * @param pictureFormat
     *            which picture format will be create for the surface.
     * @return a surface with the required size and format.
     */
    public Surface createRemoteCaptureSurface(Size size, int pictureFormat) {
        Log.i(TAG, "[createRemoteCaptureSurface]+ size:" + size.getWidth() + "x" + size.getHeight()
                + ",format = " + pictureFormat);
        checkCreateSufaceCondition(size, pictureFormat);

        // check size, if same size skip
        int width = size.getWidth();
        int height = size.getHeight();
        if (needCreateImageReader(mRemoteImageReader, width, height, pictureFormat)) {
            mRemoteSurfaceWidth = width;
            mRemoteSurfaceHeight = height;

            mRemoteImageReader = ImageReader.newInstance(mRemoteSurfaceWidth, mRemoteSurfaceHeight,
                    pictureFormat, MAX_CAPTURE_IMAGES);
            mRemoteImageReader.setOnImageAvailableListener(mCaptureListener, mCaptureHandler);
            mRemoteSurface = mRemoteImageReader.getSurface();
        }

        Log.i(TAG, "[createRemoteCaptureSurface]- mRemoteSurface:" + mRemoteSurface);
        return mRemoteSurface;
    }

    /**
     * Will create a snap shot surface with the target size and format.
     * @param size
     *            the surface's size.
     * @param pictureFormat
     *            which picture format will be create for the surface.
     * @return a surface with the required size and format.
     */
    public Surface createSnapShotSurface(Size size, int pictureFormat) {
        Log.i(TAG, "[createSnapShotSurface]+ size:" + size.getWidth() + "x" + size.getHeight()
                + ",pictureFormat = " + pictureFormat);
        checkCreateSufaceCondition(size, pictureFormat);

        // check size, if same size skip //TODO will fix this
        int width = size.getHeight() /* size.getWidth() */;
        int heigth = size.getWidth() /* size.getHeight() */;
        if (needCreateImageReader(mImageReader, width, heigth, pictureFormat)) {
            mSnapShotSurfaceWidth = width;
            mSnapShotSurfaceHeight = heigth;

            mImageReader = ImageReader.newInstance(mSnapShotSurfaceWidth, mSnapShotSurfaceHeight,
                    pictureFormat, MAX_CAPTURE_IMAGES);
            mImageReader.setOnImageAvailableListener(mCaptureListener, mCaptureHandler);
            mSnapShotSurface = mImageReader.getSurface();
        }
        Log.i(TAG, "[createSnapShotSurface]- mSnapShotSurface:" + mSnapShotSurface);
        return mSnapShotSurface;
    }

    /**
     * register a image callback will want to notify when a image available.
     * @param callback
     *            which callback will be notify.
     */
    public void setImageCallback(ImageCallback callback) {
        Log.i(TAG, "[setImageCallback] callback = " + callback);
        mImageCallback = callback;
    }

    /**
     * release the snap shot surface when don't need this surface ,such as
     * activity pause.
     */
    public void releaseSnapShotSurface() {
        if (mImageReader != null) {
            mImageReader.close();
            mImageReader = null;
        }
        if (mRemoteImageReader != null) {
            mRemoteImageReader.close();
            mRemoteImageReader = null;
        }
    }

    private void checkSupportedFormat(int format) {
        boolean supported = false;
        switch (format) {
        case PixelFormat.RGBA_8888:
        case ImageFormat.JPEG:
            supported = true;
            break;
        default:
            break;
        }
        if (!supported) {
            throw new IllegalArgumentException("ImageReaderStream unsupported format : " + format);
        }
    }

    private void checkCreateSufaceCondition(Size size, int pictureFormat) {
        Assert.assertNotNull(size);
        checkSupportedFormat(pictureFormat);
        // if image reader thread not started, start it
        if (mCaptureHandler == null) {
            mCaptureHandlerThread = new HandlerThread("ImageReaderStream.SnapShotCaptureThread");
            mCaptureHandlerThread.start();
            mCaptureHandler = new Handler(mCaptureHandlerThread.getLooper());
        }
    }

    private boolean needCreateImageReader(ImageReader reader, int width, int heigth, int format) {
        return !(reader != null && reader.getWidth() == width && reader.getHeight() == heigth &&
                reader.getImageFormat() == format);
    }

    private final ImageReader.OnImageAvailableListener mCaptureListener =
            new ImageReader.OnImageAvailableListener() {

        @Override
        public void onImageAvailable(ImageReader reader) {
            Log.i(TAG, "[onImageAvailable],mImageCallback = " + mImageCallback);
            if (mImageCallback != null) {
                byte[] data = null;
                int width = 0;
                int height = 0;
                int imageFormat = -1;
                synchronized (reader) {
                    Image image = reader.acquireLatestImage();
                    width = image.getWidth();
                    height = image.getHeight();
                    imageFormat = image.getFormat();
                    if (PixelFormat.RGBA_8888 == imageFormat) {
                        data = MultiCameraUtil.compressRGBA888ToJpeg(image);
                    } else if (PixelFormat.JPEG == imageFormat) {
                        data = MultiCameraUtil.acquireJpegBytesAndClose(image);
                    }
                }
                mImageCallback.onImageAvaliable(new ImageInfo(data, width, height, imageFormat));
            }
        }
    };
}