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

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.net.Uri;
import android.util.Log;

import com.android.vcard.VCardEntryHandler;
import com.android.vcard.VCardEntry;
import com.mediatek.rcs.common.service.FileStruct;
import com.mediatek.rcs.common.utils.ContextCacher;
import com.mediatek.rcs.common.utils.RcsVcardUtils;
import com.mediatek.rcs.common.utils.RcsVcardParserResult;

import java.io.File;

/**
 * IpVCardMessage is a RCS file transfer message contain a vCard. App can get the count of contacts
 * in the vCard by {@link #getEntryCount()}. If the vCard only has one contact entry, app can get
 * the contact's phone number by {@link #getMobilePhone()}, get the contact's name by
 * {@link #getName()}, get the contact's email by {@link #getEmail()}, and get the contact's
 * portrait by {@link #getPortrait()}.
 *
 */
public class IpVCardMessage extends IpAttachMessage {

    private String mName = "";
    private String mMobilePhone = "";
//    private String mHomePhone;
    private String mAddress;
    private String mEmail = "";
    private Bitmap mPortrait;
    private int mEntryCount = 0;
    private static final int SIZE_ONE = 1;
    private Uri mUri = null;
    //private static final int SIZE_K = 1024;

    private static final String TAG = "IpVCardMessage";

    myVcardEntryHandler mVcardEntryHandler = null;

    /**
     * Constructions.
     * @param fileStruct {@link FileStruct}
     * @param remote remote contact's phone number
     */
    public IpVCardMessage(FileStruct fileStruct, String remote) {
        super(IpMessageConsts.IpMessageType.VCARD);
        Log.d(TAG, "PluginIpVcardMessage(), fileStruct = " + fileStruct + " remote = " + remote);
        int size = (int) fileStruct.mSize;
        if (size == 0) {
            size = SIZE_ONE;
        }
        setSize(size);
        setPath(fileStruct.mFilePath);
        String name = fileStruct.mName.substring(0, (fileStruct.mName).lastIndexOf("."));
        setName(name);
        setRemote(remote);
        setTag(fileStruct.mFileTransferTag);
        //setUri(fileStruct.mFilePath);
        //set entry count
        int entryCount = RcsVcardUtils.getVcardEntryCount(fileStruct.mFilePath);
        setEntryCount(entryCount);
        //set number and photo
        mVcardEntryHandler = new myVcardEntryHandler();
        RcsVcardUtils.parseVcard(fileStruct.mFilePath, mVcardEntryHandler);
    }

    /**
     * Constructions.
     */
    public IpVCardMessage() {
        super(IpMessageConsts.IpMessageType.VCARD);
    }

    private class myVcardEntryHandler implements VCardEntryHandler {
        public void onEntryCreated(final VCardEntry entry) {
            Log.d(TAG, "onEntryCreated,  enter");
            RcsVcardParserResult result = RcsVcardUtils.ParseRcsVcardEntry(
                    entry, ContextCacher.getPluginContext());
            String name = null;
            String number = null;
            String email = null;

            if (result != null) {
                name = result.getName();
                if (result.getNumber() != null && result.getNumber().size() != 0) {
                    number = result.getNumber().get(0).toString();
                } else if (result.getEmail() != null && result.getEmail().size() != 0) {
                    email = result.getEmail().get(0).toString();
                }
            }

            //set email
            if (email != null) {
                Log.d(TAG, "set email, email = " + email);
                setEmail(email);
            }
            //set phone number
            if (number != null) {
                Log.d(TAG, "set number, number = " + number);
                setMobilePhone(number);
            }
            //set name
            if (name != null && getEntryCount() == 1) {
                Log.d(TAG, "set name, name = " + name);
                setName(name);
            }
            //set photo
            Bitmap bitmap = null;
            byte[] pic = result.getPhoto();
            if (pic != null) {
                bitmap = BitmapFactory.decodeByteArray(pic, 0, pic.length);
            }
            setPortrait(bitmap);
        }

        public void onEnd() {

        }

        public void onStart() {

        }
    }
    /**
     * Set file path.
     * @param filePath file full path.
     */
    public void setUri(String filePath) {
        Log.d(TAG, "setUri(), filePath = " + filePath);
        //mUri = Uri.parse("file:///" + filePath);
        mUri = Uri.fromFile(new File(filePath));
    }

    /**
     * Get the file path in {@link Uri} type.
     * @return the file path in {@link Uri} type
     */
    public Uri getUri() {
        return mUri;
    }

    /**
     * Get the contact's name if exist.
     * @return the contact's name
     */
    public String getName() {
        return mName;
    }

    /**
     * Set the contact's name.
     * @param name contact's name
     */
    public void setName(String name) {
        mName = name;
    }

    /**
     * Get the contact's mobile phone number.
     * @return contact's phone number
     */
    public String getMobilePhone() {
        return mMobilePhone;
    }

    /**
     * Set the contact's mobile phone number.
     * @param mobilePhone
     */
    private void setMobilePhone(String mobilePhone) {
        mMobilePhone = mobilePhone;
    }

//    public String getHomePhone() {
//        return mHomePhone;
//    }
//
//    public void setHomePhone(String homePhone) {
//        mHomePhone = homePhone;
//    }

//    public String getAddress() {
//        return mAddress;
//    }
//
//    public void setAddress(String address) {
//        mAddress = address;
//    }

    /**
     * Get the contact's email if the vCard only has one contact.
     * @return it will return null or empty string if the vCard don't has mail set.
     */
    public String getEmail() {
        return mEmail;
    }

    /**
     * Set the contact's email after parse the vCard if the email exist.
     * @param email contact's email
     */
    public void setEmail(String email) {
        mEmail = email;
    }

    /**
     * Set the contact's portrait after parse the vCard to generate portrait.
     * @param bitmap the contact's portrait
     */
    public void setPortrait(Bitmap bitmap) {
        mPortrait = bitmap;
    }

    /**
     * Get the contact's portrait.
     * @return return the contact's portrait if the vCard only has one contact
     * and has portrait. The return value may be null and the return type is {@link Bitmap}
     */
    public Bitmap getPortrait() {
        return mPortrait;
    }

    /**
     * Set entry count.
     * @param entryCount the count of contacts in vCard
     */
    public void setEntryCount(int entryCount) {
        mEntryCount = entryCount;
    }

    /**
     * Get entry count.
     * @return the count of contacts in vCard
     */
    public int getEntryCount() {
        return mEntryCount;
    }
}
