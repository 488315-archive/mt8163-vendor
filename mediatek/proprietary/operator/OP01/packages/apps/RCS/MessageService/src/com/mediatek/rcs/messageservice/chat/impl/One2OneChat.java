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

import java.util.HashSet;
import java.util.Set;

import org.gsma.joyn.JoynServiceException;
import org.gsma.joyn.capability.Capabilities;
import org.gsma.joyn.chat.ChatLog;
import org.gsma.joyn.chat.ChatService;
import org.gsma.joyn.chat.ExtendChatListener;
import org.gsma.joyn.chat.ExtendMessage;

import android.content.ContentResolver;
import android.database.Cursor;
import android.util.Log;

import com.mediatek.rcs.common.RcsLog.Direction;
import com.mediatek.rcs.common.RcsLog.Class;
import com.mediatek.rcs.common.provider.MessageStruct;
import com.mediatek.rcs.common.provider.RCSDataBaseUtils;
import com.mediatek.rcs.common.provider.ReceiveMessageStruct;
import com.mediatek.rcs.common.provider.SpamMsgData;
import com.mediatek.rcs.common.provider.SpamMsgUtils;
import com.mediatek.rcs.common.utils.RCSUtils;
import com.mediatek.rcs.common.service.Participant;
import com.mediatek.rcs.messageservice.chat.GsmaManager;
import com.mediatek.rcs.messageservice.chat.RCSChatServiceBinder;
import com.mediatek.rcs.messageservice.utils.Logger;

/**
 * This class is the implementation of a 1-2-1 chat model
 */
public class One2OneChat extends BaseChatImpl {
    public final String TAG = this.toString().substring(this.toString().lastIndexOf('.') + 1);

    private Participant mParticipant = null;
    private Set<String> spamMessageList = null;

    public One2OneChat(RCSChatServiceBinder service, String tag, String contact) {
        super(service, tag);

        mParticipant = new Participant(contact, null);
        mListener = new One2OneChatListener();
        updateTAPIChat();

        if (contact.equalsIgnoreCase(GsmaManager.SPAM_CONTACT)) {
            spamMessageList = new HashSet<String>();
        }
        Logger.d(TAG, "Constructor contact: " + contact);
    }

    /**
     * Get participant of this chat.
     *
     * @return participant of this chat.
     */
    public Participant getParticipant() {
        return mParticipant;
    }

    @Override
    protected void updateTAPIChat() {
        try {
            if (mChatImpl == null || !mChatImpl.isAlive()) {
                ChatService chatService = mGsmaManager.getChatApi();
                mChatImpl = chatService.openSingleChat(mParticipant.getContact(), mListener);
                Logger.d(TAG, "updateTAPIChat() mChatImpl: " + mChatImpl);
            }
        } catch (JoynServiceException e1) {
            // TODO Auto-generated catch block
            e1.printStackTrace();
        }
    }

    /**
     * Judge whether participants is duplicated.
     *
     * @param participants
     *            The participants to be compared.
     * @return True, if participants is duplicated, else false.
     */
    public boolean isDuplicated(Participant participant) {
        return mParticipant.equals(participant);
    }

//    @Override
//    public void sendBurnDeliveryReport(final String msgId) {
//        Runnable worker = new Runnable() {
//            @Override
//            public void run() {
//                sendBurnDeliveryReportToStack(msgId);
//            }
//        };
//        Logger.w(TAG, "sendBurnDeliveryReport() post to worker thread");
//        mWorkHandler.post(worker);
//    }


    @Override
    public void sendSpamMessage(final String msgId) {
        Runnable worker = new Runnable() {
            @Override
            public void run() {
                sendSpamMessageToStack(msgId);
            }
        };
        Logger.w(TAG, "sendSpamMessage() post to worker thread");
        mWorkHandler.post(worker);
    }

    @Override
    public void onReceiveChatMessage(ExtendMessage message) {
        Logger.v(TAG, "onReceiveChatMessage : " + message.getId());
        if (RCSUtils.isIpSpamMessage(mService.getContext(), message.getContact())) {
            Logger.d(TAG, "onReceiveChatMessage, spam msg, contact=" + message.getContact());
            receiveSpamMessage(message);
            return;
        }
        int messageType = Class.NORMAL;
        if (message.getMessageType() == ChatLog.Message.Type.BURN) {
            messageType = Class.BURN;
        } else if (message.getMessageType() == ChatLog.Message.Type.EMOTICON) {
            messageType = Class.EMOTICON;
        }
        long ipmsgId = RCSDataBaseUtils.getStackMessageId(
                mContext, message.getId(), ChatLog.Message.Direction.INCOMING);
        MessageStruct struct = new ReceiveMessageStruct(mContext, message.getContact(),
                message.getMessage(), message.getId(), ipmsgId, messageType);
        long msgId = struct.saveMessage();
        // report UI, just for Notification
        mService.getListener().onNewMessage(msgId);

        // check if need display report. CMCC not need.
    }

    @Override
    public void onRegistrationStatusChanged(boolean status) {
        super.onRegistrationStatusChanged(status);
    }

    @Override
    public void onCapabilityChanged(Capabilities capability) {
        final boolean burnMessageEnable = capability.isBurnAfterRead();
        Logger.d(TAG, "onCapabilityChanged #burnMessageEnable: " + burnMessageEnable);
        mService.getListener().onRequestBurnMessageCapabilityResult(mParticipant.getContact(),
                burnMessageEnable);
    }

    @Override
    protected String sendMessageToStack(final int type, final String content) {
        String messageId = null;
        try {
            updateTAPIChat();
            Logger.d(TAG, "sendMessage() MessageType: " + (type == Class.NORMAL ? "Normal" :
                    (type == Class.EMOTICON ? "Emoticon" : "Burn")));
            if (type == Class.NORMAL) {
                messageId = mChatImpl.sendMessage(content);
            } else if (type == Class.EMOTICON) {
                messageId = mChatImpl.sendMessage(content, ChatLog.Message.Type.EMOTICON);
            } else if (type == Class.BURN) {// burning message;
                messageId = mChatImpl.sendMessage(content, ChatLog.Message.Type.BURN);
            }
            Logger.d(TAG, "sendMessage() to stack. messageId: " + messageId);
        } catch (JoynServiceException e1) {
            e1.printStackTrace();
        }
        return messageId;
    }

//    private void sendBurnDeliveryReportToStack(String msgId) {
//        if (TextUtils.isEmpty(msgId)) {
//            Logger.d(TAG, "sendBurnDeliveryReport, null msgId");
//            return;
//        }
//        try {
//            updateTAPIChat();
//            Logger.d(TAG, "sendBurnDeliveryReport() to stack. remote: " +
//                    mParticipant.getContact());
//            Logger.d(TAG, "sendBurnDeliveryReport() messageId: " + msgId);
//            mChatImpl.sendBurnedDeliveryReport(msgId);
//        } catch (JoynServiceException e1) {
//            e1.printStackTrace();
//        }
//    }

    private void sendSpamMessageToStack(String msgId) {
        try {
            updateTAPIChat();
            Logger.d(TAG, "sendSpamMessage() report messageId: " + msgId);
            String spamMsgId = mChatImpl.prosecuteMessage(msgId);
            if (spamMsgId != null) {
                Logger.d(TAG, "sendSpamMessage() spam messageId: " + spamMsgId);
                spamMessageList.add(spamMsgId);
            }
            else {
                mService.getListener().onSpamReportResult(false);
            }
        } catch (Exception e) {
            // ???
        }
    }

    private void receiveSpamMessage(ExtendMessage message) {
        long ipmsgId = RCSDataBaseUtils.getStackMessageId(
                mContext, message.getId(), ChatLog.Message.Direction.INCOMING);
        int type = SpamMsgData.Type.TYPE_IP_TEXT_MSG;
        if (message.getMessageType() == ChatLog.Message.Type.BURN) {
            type = SpamMsgData.Type.TYPE_IP_BURN_TEXT_MSG;
        } else if (message.getMessageType() == ChatLog.Message.Type.CLOUD) {
            type = SpamMsgData.Type.TYPE_CLOUD_MSG;
        } else if (message.getMessageType() == ChatLog.Message.Type.EMOTICON) {
            type = SpamMsgData.Type.TYPE_EMOTICON_MSG;
        }
        SpamMsgUtils.getInstance(mService.getContext()).insertSpamMessage(
                message.getMessage(),
                message.getContact(),
                RCSUtils.getRCSSubId(),
                type,
                ipmsgId,
                message.getId());
        return;
    }

    private void deleteBurnedMsg(final String msgId) {
        Log.d(TAG, " drawDeleteBARMsgIndicator deleteBurnedMsg()  ");
        ContentResolver resolver = mService.getContext().getContentResolver();
        String[] projection = { ChatLog.Message.MESSAGE_TYPE, ChatLog.Message.ID };
        Cursor cursor = resolver.query(RCSUtils.RCS_URI_MESSAGE, projection, "msg_id='" + msgId
                + "'", null, null);
        try {
            if (cursor == null) {
                Log.d(TAG, " drawDeleteBARMsgIndicator cursor == null");
            }
            if (cursor != null && cursor.moveToFirst()) {
                boolean isBurned = cursor.getInt(cursor.getColumnIndex(
                        ChatLog.Message.MESSAGE_TYPE)) == ChatLog.Message.Type.BURN ? true : false;
                Log.d(TAG, " drawDeleteBARMsgIndicator deleteBurnedMsg, isBurned=" + isBurned);
                if (isBurned) {
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            try {
                                Log.d(TAG, " drawDeleteBARMsgIndicator deleteBurnedMsg, run");
                                Thread.sleep(5100);
                                RCSDataBaseUtils.deleteBurnMessage(
                                    mContext,
                                    msgId,
                                    Direction.OUTGOING);
                            } catch (Exception e) {
                                e.printStackTrace();
                            }
                        }
                    }).start();
                }
            }
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }
        return;
    }

    private class One2OneChatListener extends ExtendChatListener {

        private final String TAG = One2OneChat.this.TAG + "#ChatListener";

        public One2OneChatListener() {
        }

        @Override
        public void onNewMessage(final ExtendMessage message) {
            Logger.d(TAG, "onNewMessage()   message id:" + message.getId() + " ,message text:"
                    + message.getMessage());
            // just listen the broadcast.
            // onReceiveChatMessage(message);
        }

        @Override
        public void onReportMessageDelivered(final String arg0, final String arg1) {
            Runnable Notification = new Runnable() {
                @Override
                public void run() {
                    Logger.d(TAG, "onReportMessageDelivered()  msgId:" + arg0);
                    if (spamMessageList != null) {
                        return;
                    }
                    onReceiveMessageDeliveryStatus(arg0, "delivered");
                }
            };
            mWorkHandler.post(Notification);
        }

        @Override
        public void onReportMessageDisplayed(final String arg0, final String arg1) {
            Logger.d(TAG, "onReportMessageDISPLAYED()  msgId:" + arg0);
        }

        @Override
        public void onReportMessageFailed(final String arg0, final int arg1, final String arg2) {
            Runnable Notification = new Runnable() {
                @Override
                public void run() {
                    Logger.d(TAG, "onReportMessageFailed()  msgId:" + arg0);
                    if (spamMessageList != null && !spamMessageList.isEmpty()) {
                        spamMessageList.remove(arg0);
                        mService.getListener().onSpamReportResult(false);
                        return;
                    }
                    onReceiveMessageDeliveryStatus(arg0, "failed");
                }
            };
            mWorkHandler.post(Notification);
        }

        @Override
        public void onReportMessageSent(final String arg0) {
            Runnable Notification = new Runnable() {
                @Override
                public void run() {
                    Logger.d(TAG, "onReportSentMessage()  msgId:" + arg0);
                    if (spamMessageList != null && !spamMessageList.isEmpty()) {
                        spamMessageList.remove(arg0);
                        mService.getListener().onSpamReportResult(true);
                        return;
                    }
                    onReceiveMessageDeliveryStatus(arg0, "sent");
                    deleteBurnedMsg(arg0);
                }
            };
            mWorkHandler.post(Notification);
        }
    }
}
