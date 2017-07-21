/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2012. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

package com.mediatek.rcs.common;

import android.util.Log;
import com.mediatek.rcs.common.IpMessageConsts;
import com.mediatek.rcs.common.service.FileStruct;

/**
 * IpVoiceMessage is a RCS file transfer message with a audio. App can get the audio's duration by
 * {@link #getDuration()}.
 *
 */
public class IpVoiceMessage extends IpAttachMessage {

    private int mDurationTime;
//    private String mCaption;
//    private String mFileName;

    private static final String TAG = "IpVoiceMessage";

    /**
     * Construction.
     * @param fileStruct {@link FileStruct}
     * @param remote remote contact's number
     */
    public IpVoiceMessage(FileStruct fileStruct, String remote) {
          super(IpMessageConsts.IpMessageType.VOICE);
          Log.d(TAG, "PluginIpImageMessage(), fileStruct = " + fileStruct + " remote = " + remote);
          setSize((int) fileStruct.mSize);
          setPath(fileStruct.mFilePath);
//          mFileName = fileStruct.mName;
          Log.d(TAG, " [BurnedMsg]: -------- audio Burn = " + fileStruct.mSessionType);
          setBurnedMessage(fileStruct.mSessionType);
          setRemote(remote);
          setTag(fileStruct.mFileTransferTag);
          analysisAttribute();
          setDuration(fileStruct.mDuration);
    }

    /**
     * Construction.
     */
    public IpVoiceMessage() {
        super(IpMessageConsts.IpMessageType.VOICE);
    }

    private void analysisAttribute() {
        Log.d(TAG, "analysisAttribute()");
 //       String whereClause = MediaStore.Audio.AudioColumns.DISPLAY_NAME + "='" + mFileName + "'";
//        ContentResolver cr = AndroidFactory.getApplicationContext().getContentResolver();
//        Cursor cursor = null;
//        int duration = 0;
//        try {
//            cursor = cr.query(MediaStore.Audio.Media.EXTERNAL_CONTENT_URI, new String[] {
//                    MediaStore.Audio.AudioColumns.DURATION}, whereClause, null, null);
//            if (cursor != null && cursor.getCount() != 0) {
//                cursor.moveToFirst();
//                duration = cursor.getInt(cursor.getColumnIndex(
//                      MediaStore.Audio.AudioColumns.DURATION));
//            } else {
//                cursor = cr.query(MediaStore.Audio.Media.INTERNAL_CONTENT_URI, new String[] {
//                    MediaStore.Audio.AudioColumns.DURATION
//                }, whereClause, null, null);
//                if (cursor != null && cursor.getCount() != 0) {
//                    cursor.moveToFirst();
//                    duration = cursor.getInt(cursor
//                            .getColumnIndex(MediaStore.Audio.Media.DURATION));
//                } else {
//                    Log.w(TAG, "analysisAttribute(), cursor is null!");
//                }
//            }
//            Log.d(TAG, "analysisAttribute(), duration = " + duration);
//            setDuration(duration / SIZE_SECOND);
//        } finally {
//            if (cursor != null) {
//                cursor.close();
//                cursor = null;
//            }
//        }
    }

    /**
     * Get the duration of the audio in seconds.
     * @return the duration of he audio
     */
    public int getDuration() {
        return mDurationTime;
    }

    /**
     * Set the duration of the audio in seconds.
     * @param durationTime duration of the audio in seconds
     */
    public void setDuration(int durationTime) {
        mDurationTime = durationTime;
    }

//    public String getCaption() {
//        return mCaption;
//    }
//
//    public void setCaption(String caption) {
//        mCaption = caption;
//    }
//    public void setName(String name) {
//        Log.d(TAG, "setName()");
//        mFileName = name;
//    }
//
//    public String getName() {
//        Log.d(TAG, "getName()");
//        return mFileName;
//    }
}
