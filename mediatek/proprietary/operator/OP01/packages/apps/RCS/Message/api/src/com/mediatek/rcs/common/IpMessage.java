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

/**
 * IpMessage is a message that is transfered by RCS.
 *
 */
public class IpMessage {

    private long mId;        // message id in mmssms.db, value of "_id".
//    private long mIpDbId;     // message id in ip message db.
//    private long mSimId;     // sim id that message belongs to.
    private String mMessageId; // messageId or ftId in ipmessage db
    //TODO: should use gsma interface
//    private int mDirection;     // direction of ipmessage
    private long mDate;         // data
    /**
     * Type of Ip message, defined by IpMessageType
     * @see IpMessageConsts
     */
    private int mType;
    private int mStatus;
    private String mRemote;
    private String mFrom;
    private boolean mBurnedMessage;

    /**
     * IpMessage construction.
     * @param type the message type: {@link IpMessageConsts.IpMessageType}
     */
    public IpMessage(int type) {
        mType = type;
    }

    /**
     * IpMessage construction.
     * @param type the message type: {@link IpMessageConsts.IpMessageType}
     * @param rcsId the id in table of 'rcs_message' in TelephonyProvider. Reference the column
     * 'ID' in * {@link RcsLog.MessageColumn}
     * @param messageId the id in RCS protocol.
     * @param direction message direction: received message or sent message. The value should be
     *          one of the values in {@link RcsLog.Direction}
     */
    public IpMessage(int type, long rcsId, String messageId, int direction) {
        mType = type;
        mId = rcsId;
        mMessageId = messageId;
//        mDirection = direction;
    }

    /**
     * Get the id of this message in table of "rcs_message".
     * @return the id of this message.
     */
    public long getId() {
        return mId;
    }

    protected void setId(long id) {
        mId = id;
    }

    /**
     * Whether the message is burned message.
     * @return return true if the message is burned message, else return false.
     */
    public boolean getBurnedMessage() {
        return mBurnedMessage;
    }

    /**
     * Set the Message to be burned message.
     * @param isBurned the value is true if want to set the message to burned message.
     */
    public void setBurnedMessage(boolean isBurned) {
        mBurnedMessage = isBurned;
    }

//    public long getIpDbId() {
//        return mIpDbId;
//    }

//    public void setIpDbId(long ipDbId) {
//        mIpDbId = ipDbId;
//    }

//    public long getSimId() {
//        return mSimId;
//    }

//    public void setSimId(long simId) {
//        mSimId = simId;
//    }

    /**
     * Get the message id.
     * @return The message id.
     */
    public String getMessageId() {
        return mMessageId;
    }

    protected void setMessageId(String messageId) {
        mMessageId = messageId;
    }

//    public int getDirection() {
//        return mDirection;
//    }

//    public void setDirection(int direction) {
//        mDirection = direction;
//    }

    /**
     * Get the message date when received or sent.
     * @return if the message is received message,  return received date; else if the message
     * is sent message, return sent date.
     */
    public long getDate() {
        return mDate;
    }

    /**
     * Get the message date when received or sent.
     * @param date if the message is received message,  the date is received date;
     * else if the message is sent message, the date is sent date.
     */
    protected void setDate(long date) {
        mDate = date;
    }

    /**
     * Get message type.
     * @return The message type. It's one of the values in {@link IpMessageConsts.IpMessageType}
     */
    public int getType() {
        return mType;
    }

    protected void setType(int type) {
        mType = type;
    }

    /**
     * Get the message delivery status.
     * @return the message delivery status. It's one of the values in {@link RcsLog.MessageStatus}
     */
    public int getStatus() {
        return mStatus;
    }

    protected void setStatus(int status) {
        mStatus = status;
    }

    /**
     * Get the message's remote number.
     * @return return the receiver number if the message is sent message; else return the sender
     * number if the message is received message
     */
    public String getRemote() {
        return mRemote;
    }

    /**
     * Set the message's remote phone number. If it's one to multiple message, should set the
     * parameter remote as receivers's phone numbers that separated with a comma.
     * @param remote number of the message. The number can be getted by {@link #getRemote()}
     */
    public void setRemote(String remote) {
        mRemote = remote;
    }
}

