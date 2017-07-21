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

import com.mediatek.rcs.common.MessageStatusUtils.IFileTransfer.Status;

/**
 * IpAttachMessage is a RCS message that has attachment. It contains image, audio, video, vCard and
 * geolocation message.
 *
 */
public class IpAttachMessage extends IpMessage {
    private String mPath;
//    private String mUrl;    /* for inbox msg, it may contain the downloaded url */
    private int mSize;

    private long mProgress = 0;
    //private int mStatus;
    private Status mRcsStatus;
    private String mTransferTag;

    private static final String TAG = "IpAttachMessage";

    /**
     * IpAttachMessage construction.
     * @param type the message type: {@link IpMessageConsts.IpMessageType}
     */
    public IpAttachMessage(int type) {
        super(type);
    }

//    public boolean isInboxMsgDownloalable() {
//        return !TextUtils.isEmpty(mUrl);
//    }

    /**
     * Get file path.
     * @return the full file path include dir and file name. If the message is received message and
     * has not been download, it maybe return null.
     */
    public String getPath() {
        return mPath;
    }

    /**
     * Set file path.
     * @param path the full file path include dir and file name
     */
    public void setPath(String path) {
        mPath = path;
    }

//    public String getUrl() {
//        return mUrl;
//    }
//
//    public void setUrl(String url) {
//        mUrl = url;
//    }

    /**
     * Get the message's size.
     * @return the message's size in bytes
     */
    public int getSize() {
        return mSize;
    }

    /**
     * Set the message's size.
     * @param size the message's size in bytes
     */
    public void setSize(int size) {
        mSize = size;
    }

//    public Drawable getFileTypeIcon() {
//        return null;
//    }

//    public void setProgress(long progress) {
//        Log.d(TAG, "setProgress(), progress = " + progress);
//        mProgress = progress;
//    }

//    public int getProgress() {
//        Log.d(TAG, "getProgress()");
//        return (int) mProgress;
//    }

/*
    public void setStatus(int status) {
        Log.d(TAG, "SetStatus()");
        mStatus = status;
    }

    public int getStatus() {
        Log.d(TAG, "getStatus()");
        return mStatus;
    }
    */

    protected void setRcsStatus(Status status) {
        Log.d(TAG, "setRcsStatus()");
        mRcsStatus = status;
    }

    /**
     * Get the message's status.
     * @return reference the message's status {@link MessageStatusUtils.IFileTransfer.Status}
     */
    public Status getRcsStatus() {
        Log.d(TAG, "getRcsStatus()");
        return mRcsStatus;
    }

    void setTag(String tag) {
        Log.d(TAG, "setTag()");
        mTransferTag = tag;
    }

    /**
     * Get file transfer's tag.
     * @return the the file transfer 's download id. App can use this id to download file transfer
     * {@see RCSMessageManager#downloadAttach(String, String)} or redownload file transfer by
     * {@link RCSMessageManager#reDownloadAttach(String, String)}
     */
    public String getFtId() {
        Log.d(TAG, "getTag()");
        return mTransferTag;
    }
}
