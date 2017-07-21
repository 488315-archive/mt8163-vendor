/*
 * Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are protected under
 * relevant copyright laws. The information contained herein is confidential and proprietary to
 * MediaTek Inc. and/or its licensors. Without the prior written permission of MediaTek inc. and/or
 * its licensors, any reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2012. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES THAT THE
 * SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED FROM MEDIATEK AND/OR ITS
 * REPRESENTATIVES ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS
 * ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK
 * PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED
 * BY, INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO
 * SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT
 * IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN
 * MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE
 * TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM. RECEIVER'S SOLE
 * AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK
 * SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK
 * SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software") have been
 * modified by MediaTek Inc. All revisions are subject to any receiver's applicable license
 * agreements with MediaTek Inc.
 */

package com.mediatek.rcs.messageservice.chat.impl;

import java.util.List;

import org.gsma.joyn.JoynServiceException;
import org.gsma.joyn.chat.ChatService;
import org.gsma.joyn.chat.ChatLog;
import org.gsma.joyn.chat.ExtendChatListener;
import org.gsma.joyn.chat.ExtendMessage;

import com.mediatek.rcs.common.RcsLog.Class;
import com.mediatek.rcs.messageservice.utils.Logger;
import com.mediatek.rcs.messageservice.chat.IO2MChat;
import com.mediatek.rcs.messageservice.chat.RCSChatServiceBinder;

/**
 * This class is the implementation of a 1-2-1 chat model
 */

public class One2MultiChat extends BaseChatImpl implements IO2MChat {
    public final String TAG = this.toString().substring(this.toString().lastIndexOf('.') + 1);;

    private List<String> mContacts;

    public One2MultiChat(RCSChatServiceBinder service, String tag, List<String> contacts) {
        super(service, tag);
        mContacts = contacts;
        mListener = new One2MultiChatListener();
        updateTAPIChat();
    }

    public void setContacts(List<String> contacts) {
        mContacts = contacts;
    }

    @Override
    protected void updateTAPIChat() {
        try {
            if (mChatImpl == null || !mChatImpl.isAlive()) {
                ChatService chatService = mGsmaManager.getChatApi();
                mChatImpl = chatService.openMultipleChat(mContacts, mListener);
                Logger.d(TAG, "updateTAPIChat() mChatImpl: " + mChatImpl);
            }
        } catch (JoynServiceException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    @Override
    public void onReceiveChatMessage(ExtendMessage message) {
        // no message receive from one2multi.
        Logger.d(TAG, "onReceiveChatMessage()");
    }

    @Override
    public void onRegistrationStatusChanged(boolean status) {
        super.onRegistrationStatusChanged(status);
    }

    @Override
    protected String sendMessageToStack(final int type, String content) {
        String messageId = null;
        try {
            Logger.d(TAG, "sendMessage() to stack. type: " + type);
            Logger.d(TAG, "sendMessage() to stack. remotes: " + mContacts);
            updateTAPIChat();
            if (type == Class.EMOTICON) {
                messageId = mChatImpl.sendMessage(content, ChatLog.Message.Type.EMOTICON);
            } else {
                messageId = mChatImpl.sendMessage(content);
            }
            Logger.d(TAG, "sendMessage() messageId: " + messageId);
        } catch (JoynServiceException e1) {
            e1.printStackTrace();
        }
        return messageId;
    }

    private class One2MultiChatListener extends ExtendChatListener {
        private final String TAG = this.toString()
                .substring(this.toString().indexOf("One2MultiChatListener"),
                        this.toString().length());

        public One2MultiChatListener() {
        }

        @Override
        public void onNewMessage(ExtendMessage arg0) {
            Logger.d(TAG, "onNewMessage msgId:" + arg0.getId());
        }

        @Override
        public void onReportMessageDelivered(final String arg0, final String arg1) {
            Runnable worker = new Runnable() {
                @Override
                public void run() {
                    Logger.d(TAG, "onReportDeliveredMessage()  msgId:" + arg0);
                    onReceiveMessageDeliveryStatus(arg0, "delivered");
                }
            };
            mWorkHandler.post(worker);
        }

        @Override
        public void onReportMessageDisplayed(final String arg0, final String arg1) {
            Logger.d(TAG, "onReportMessageDisplayed()  msgId:" + arg0);
        }

        @Override
        public void onReportMessageFailed(final String arg0, final int arg1, final String arg2) {
            Runnable worker = new Runnable() {
                @Override
                public void run() {
                    Logger.d(TAG, "onReportMessageFailed()  msgId:" + arg0);
                    onReceiveMessageDeliveryStatus(arg0, "failed");
                }
            };
            mWorkHandler.post(worker);
        }

        @Override
        public void onReportMessageSent(final String arg0) {
            Runnable worker = new Runnable() {
                @Override
                public void run() {
                    Logger.d(TAG, "onReportSentMessage()  msgId:" + arg0);
                    onReceiveMessageDeliveryStatus(arg0, "sent");
                }
            };
            mWorkHandler.post(worker);
        }
    }
}
