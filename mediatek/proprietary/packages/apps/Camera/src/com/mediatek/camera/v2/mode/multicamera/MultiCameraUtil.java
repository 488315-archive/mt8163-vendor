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

import android.app.Activity;
import android.content.ContentValues;
import android.graphics.Bitmap;
import android.graphics.ImageFormat;
import android.graphics.PixelFormat;
import android.hardware.Camera.CameraInfo;
import android.location.Location;
import android.media.CamcorderProfile;
import android.media.Image;
import android.media.Image.Plane;
import android.media.MediaRecorder;
import android.provider.MediaStore.Images.ImageColumns;
import android.provider.MediaStore.MediaColumns;
import android.provider.MediaStore.Video;
import android.util.Log;
import android.view.Surface;

import com.android.camera.R;

import com.mediatek.camera.v2.stream.RecordStream;
import com.mediatek.camera.v2.stream.multicamera.renderer.EGLConfigWrapper;
import com.mediatek.camera.v2.util.Utils;

import junit.framework.Assert;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

/**
 * this class service for Multi Camera Mode.
 */
public class MultiCameraUtil {
    private static final String TAG = MultiCameraUtil.class.getSimpleName();
    private static final int ONLY_ONE_CAMERA = 1;
    private static final int FIRST_INDEX = 0;
    private static final int MINUTE_TO_MINI_SECONEDS = 1000;
    private static final int JPEG_QUALITY = 90;
    private static final int BYTE_TO_BIT = 8;


    protected static ContentValues getCaptureContentValues(int width, int height, int orientation,
           Location location, String filePath) {
        ContentValues contentValues = new ContentValues();
        long dateTaken = System.currentTimeMillis();
        String title = Utils.createJpegName(dateTaken);
        String filename = title + ".jpg";
        String mime = "image/jpeg";
        String path = filePath + '/' + filename;

        contentValues.put(ImageColumns.DATE_TAKEN, dateTaken);
        contentValues.put(ImageColumns.TITLE, title);
        contentValues.put(ImageColumns.DISPLAY_NAME, filename);
        contentValues.put(ImageColumns.DATA, path);
        contentValues.put(ImageColumns.MIME_TYPE, mime);

        contentValues.put(ImageColumns.WIDTH, width);
        contentValues.put(ImageColumns.HEIGHT, height);
        contentValues.put(ImageColumns.ORIENTATION, orientation);

        if (location != null) {
            contentValues.put(ImageColumns.LATITUDE, location.getLatitude());
            contentValues.put(ImageColumns.LONGITUDE, location.getLongitude());
        }
        Log.i(TAG, "[getCaptureContentValues] width = " + width + ",height = " + height
                + ",path = " + path + ", orientation = " + orientation);
        return contentValues;
    }

    protected static ContentValues getVideoContentValues(Activity activity,
            CamcorderProfile profile, Location location, int orientation,
                  String tempPath, String filePath) {
        ContentValues mVideoContentValues = new ContentValues();
        long dateTaken = System.currentTimeMillis();
        String title = createName(activity, dateTaken);
        String filename = title
                + MultiCameraUtil.convertOutputFormatToFileExt(profile.fileFormat);
        String mime = convertOutputFormatToMimeType(profile.fileFormat);
        String path = filePath + '/' + filename;

        mVideoContentValues = new ContentValues();
        mVideoContentValues.put(Video.Media.TITLE, title);
        mVideoContentValues.put(Video.Media.DISPLAY_NAME, filename);
        mVideoContentValues.put(Video.Media.DATE_TAKEN, dateTaken);
        mVideoContentValues.put(MediaColumns.DATE_MODIFIED, dateTaken / MINUTE_TO_MINI_SECONEDS);
        mVideoContentValues.put(Video.Media.MIME_TYPE, mime);
        mVideoContentValues.put(Video.Media.DATA, path);
        mVideoContentValues.put(Video.Media.WIDTH, profile.videoFrameWidth);
        mVideoContentValues.put(Video.Media.HEIGHT, profile.videoFrameHeight);
        mVideoContentValues.put(Video.Media.ORIENTATION, orientation);
        mVideoContentValues.put(
                Video.Media.RESOLUTION,
                Integer.toString(profile.videoFrameWidth) + "x"
                        + Integer.toString(profile.videoFrameHeight));
        mVideoContentValues.put(Video.Media.SIZE, new File(tempPath).length());

        if (location != null) {
            mVideoContentValues.put(ImageColumns.LATITUDE, location.getLatitude());
            mVideoContentValues.put(ImageColumns.LONGITUDE, location.getLongitude());
        }
        return mVideoContentValues;
    }

    protected static List<String> getMediaRecordingParamters() {
        List<String> recordParamters = new ArrayList<String>();
        recordParamters.add(RecordStream.RECORDER_INFO_SUFFIX
                + RecordStream.MEDIA_RECORDER_INFO_BITRATE_ADJUSTED);
        recordParamters.add(RecordStream.RECORDER_INFO_SUFFIX
                + RecordStream.MEDIA_RECORDER_INFO_FPS_ADJUSTED);
        recordParamters.add(RecordStream.RECORDER_INFO_SUFFIX
                + RecordStream.MEDIA_RECORDER_INFO_START_TIMER);
        recordParamters.add(RecordStream.RECORDER_INFO_SUFFIX
                + RecordStream.MEDIA_RECORDER_INFO_WRITE_SLOW);
        recordParamters.add(RecordStream.RECORDER_INFO_SUFFIX
                + RecordStream.MEDIA_RECORDER_INFO_CAMERA_RELEASE);
        return recordParamters;
    }

    protected static void printSurfaceLog(String fucntionName, Map<String, Surface> surface) {
        if (surface == null) {
            Log.i(TAG, "[" + fucntionName + "] surface is null");
        }
        Iterator entries = surface.entrySet().iterator();
        while (entries.hasNext()) {
            Map.Entry entry = (Map.Entry) entries.next();
            Object key = entry.getKey();
            Object value = entry.getValue();
            Log.i(TAG, "[" + fucntionName + "],key : " + key + ",value = " + value);
        }
    }

    protected static boolean isLocalSinglePreview(List<String> cameraId) {
        boolean onlyLocalSinglePreview = false;
        if (cameraId != null && cameraId.size() == ONLY_ONE_CAMERA) {
            String id = cameraId.get(FIRST_INDEX);
            onlyLocalSinglePreview = Integer.toString(CameraInfo.CAMERA_FACING_BACK).equals(id)
                    || Integer.toString(CameraInfo.CAMERA_FACING_FRONT).equals(id);
        }

        Log.i(TAG, "[isLocalSinglePreview] onlyLocalSinglePreview = " + onlyLocalSinglePreview);
        return onlyLocalSinglePreview;
    }

    private static String createName(Activity activity, long dateTaken) {
        Date date = new Date(dateTaken);
        SimpleDateFormat dateFormat = new SimpleDateFormat(
                activity.getString(R.string.video_file_name_format));

        return dateFormat.format(date);
    }

    protected static String convertOutputFormatToFileExt(int outputFileFormat) {
        if (outputFileFormat == MediaRecorder.OutputFormat.MPEG_4) {
            Log.i(TAG, "[convertOutputFormatToFileExt] return .mp4");
            return ".mp4";
        }
        Log.i(TAG, "[convertOutputFormatToFileExt] return .3gp");
        return ".3gp";
    }

    protected static String convertOutputFormatToMimeType(int outputFileFormat) {
        if (outputFileFormat == MediaRecorder.OutputFormat.MPEG_4) {
            return "video/mp4";
        }
        return "video/3gpp";
    }

    protected static void deleteVideoFile(String fileName) {
        File f = new File(fileName);
        if (!f.delete()) {
            Log.i(TAG, "[deleteVideoFile] Could not delete " + fileName);
        }
    }

    protected static byte[] compressRGBA888ToJpeg(Image image) {
        byte[] jpegData = null;
        try {
            Bitmap bitmap = Bitmap.createBitmap(image.getWidth(), image.getHeight(),
                    EGLConfigWrapper.getInstance().getBitmapConfig());
            ByteBuffer imageBuffer = null;
            // get byte buffer from image
            if ((image.getPlanes()[0].getPixelStride() * image.getWidth()) != image.getPlanes()[0]
                    .getRowStride()) {
                Log.i(TAG, "getPixelStride = " + image.getPlanes()[0].getPixelStride()
                        + " getRowStride = " + image.getPlanes()[0].getRowStride());
                // buffer is not placed continuously, should remove buffer
                // position again
                byte[] bytes = getContinuousRGBADataFromImage(image);
                imageBuffer = ByteBuffer.allocateDirect(bytes.length);
                imageBuffer.put(bytes);
                imageBuffer.rewind();
                bytes = null;
            } else {
                // continuous buffer, read directly
                imageBuffer = image.getPlanes()[0].getBuffer();
            }
            System.gc();
            // copy buffer to bitmap
            bitmap.copyPixelsFromBuffer(imageBuffer);
            imageBuffer.clear();
            imageBuffer = null;
            // compress and save it
            ByteArrayOutputStream bos = null;
            try {
                bos = new ByteArrayOutputStream();
                bitmap.compress(Bitmap.CompressFormat.JPEG, JPEG_QUALITY, bos);
                bitmap.recycle();
                bitmap = null;
            } finally {
                jpegData = bos.toByteArray();
                if (bos != null) {
                    try {
                        bos.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }
            System.gc();
            image.close();
            image = null;
        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        }
        return jpegData;
    }

    protected static byte[] getContinuousRGBADataFromImage(Image image) {
        Log.i(TAG, "getContinuousRGBADataFromImage begin");
        if (image.getFormat() != EGLConfigWrapper.getInstance().getPixelFormat()) {
            Log.i(TAG, "error format = " + image.getFormat());
            return null;
        }
        int format = image.getFormat();
        int width = image.getWidth();
        int height = image.getHeight();
        int rowStride;
        int pixelStride;
        byte[] data = null;
        Plane[] planes = image.getPlanes();
        if (format == EGLConfigWrapper.getInstance().getPixelFormat()) {
            PixelFormat pixelInfo = new PixelFormat();
            PixelFormat.getPixelFormatInfo(format, pixelInfo);
            ByteBuffer buffer = planes[0].getBuffer();
            rowStride = planes[0].getRowStride();
            pixelStride = planes[0].getPixelStride();
            data = new byte[width * height * pixelInfo.bitsPerPixel / BYTE_TO_BIT];
            int offset = 0;
            int rowPadding = rowStride - pixelStride * width;
            // this format, pixelStride == bytesPerPixel, so read of the entire
            // row
            for (int y = 0; y < height; y++) {
                int length = width * pixelStride;
                buffer.get(data, offset, length);
                // Advance buffer the remainder of the row stride
                buffer.position(buffer.position() + rowPadding);
                offset += length;
            }
        }

        Log.i(TAG, "getContinuousRGBADataFromImage end");
        return data;
    }

    protected static byte[] acquireJpegBytesAndClose(Image image) {
        Assert.assertNotNull(image);
        ByteBuffer buffer;
        if (image.getFormat() == ImageFormat.JPEG) {
            Image.Plane plane0 = image.getPlanes()[0];
            buffer = plane0.getBuffer();
        } else {
            throw new RuntimeException("Unsupported image format.");
        }
        byte[] imageBytes = new byte[buffer.remaining()];
        buffer.get(imageBytes);
        buffer.rewind();
        image.close();
        return imageBytes;
    }
}