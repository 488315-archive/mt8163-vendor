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

/**
 * It's a implementation of IChat, it indicates a specify chat model.
 */

package com.mediatek.rcs.messageservice.chat.impl;

import java.util.Vector;

import org.gsma.joyn.JoynServiceException;
import org.gsma.joyn.capability.Capabilities;
import org.gsma.joyn.chat.ExtendChat;
import org.gsma.joyn.chat.ExtendChatListener;
import org.gsma.joyn.chat.ExtendMessage;

import android.content.Context;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;

import com.mediatek.rcs.common.RcsLog.MessageStatus;
import com.mediatek.rcs.common.RcsLog.MessageType;
import com.mediatek.rcs.common.provider.RCSDataBaseUtils;
import com.mediatek.rcs.common.utils.RCSUtils;
import com.mediatek.rcs.messageservice.chat.GsmaManager;
import com.mediatek.rcs.messageservice.chat.IChat;
import com.mediatek.rcs.messageservice.chat.RCSChatServiceBinder;
import com.mediatek.rcs.messageservice.utils.Logger;

public abstract class BaseChatImpl implements IChat {
    private static final String TAG = "BaseChatImpl";

    public static final int MAX_PAGER_MODE_MSG_LENGTH = 900;
    // 60s
    public static final long MAX_MSG_PENDING_TIME = 45 * 1000;

    public static final int BASECHAT_OP_BASE = 0;
    public static final int BASE_OP_SEND_MESSAGE = BASECHAT_OP_BASE + 1;
    public static final int BASE_OP_RESEND_MESSAGE = BASECHAT_OP_BASE + 2;
    public static final int BASE_OP_SEND_MESSAGE_TIMEOUT = BASECHAT_OP_BASE + 3;

    public static final int BASECHAT_NOTIFICATION_BASE = 10000;
    public static final int NOTIFICATION_REGISTRATION_STATUS_FALSE = BASECHAT_NOTIFICATION_BASE + 1;
    public static final int NOTIFICATION_REGISTRATION_STATUS_TRUE = BASECHAT_NOTIFICATION_BASE + 2;
    public static final int NOTIFICATION_CORESERVICE_DOWN = BASECHAT_NOTIFICATION_BASE + 3;
    // Chat message list, not used.
    // private final List<ChatMessage> mMessageList = new LinkedList<ChatMessage>();

    protected String mTag = null;
    protected ExtendChat mChatImpl = null;
    protected ExtendChatListener mListener = null;

    protected Vector<SendingMessage> mSendingMessageList = new Vector<SendingMessage>();

    protected GsmaManager mGsmaManager = GsmaManager.getInstance();
    protected RCSChatServiceBinder mService = null;
    protected Context mContext = null;

    static protected Handler mWorkHandler = null;

    static {
        HandlerThread thread = new HandlerThread(TAG);
        thread.start();
        mWorkHandler = new WorkHandler(thread.getLooper());
    }

    protected BaseChatImpl(RCSChatServiceBinder service, String tag) {
        mService = service;
        mTag = tag;
        mContext = mService.getContext();
    }

    protected String getTag() {
        return mTag;
    }

    protected void setTag(String tag) {
        mTag = tag;
    }

    protected void updateTAPIChat() {
        Logger.d(TAG, "updateTAPIChat()");
    }

    @Override
    public void onSendChatMessageResult(final long msgId, final boolean result) {
        Logger.d(TAG, "onSendChatMessageResult()");
        if (findSendingMessageFromList(msgId) == null) {
            return;
        }
        removeMsgFromSendingList(msgId);
        if (!result) {
            mService.getListener().onSendMessageFailed(msgId);
        }
    }

    @Override
    public void sendChatMessage(final long smsId, final String content, final int type) {
        Runnable worker = new Runnable() {
            @Override
            public void run() {
                sendMessage(smsId, content, type);
            }
        };
        Logger.w(TAG, "sendChatMessage() post to worker thread");
        mWorkHandler.post(worker);
    }

    private void sendMessage(final long msgIdInSMS, final String content, final int type) {
        if (RCSUtils.in2GCall(GsmaManager.getContext())) {
            onSendChatMessageResult(msgIdInSMS, false);
            return;
        }
        SendingMessage sendingMsg = findSendingMessageFromList(msgIdInSMS);
        if (sendingMsg == null) {
            sendingMsg = addMsgToSendingList(msgIdInSMS, content, type);
            // first time, send a message for the result.
            Message timerMsg = mWorkHandler.obtainMessage(BASE_OP_SEND_MESSAGE_TIMEOUT, sendingMsg);
            mWorkHandler.sendMessageDelayed(timerMsg, MAX_MSG_PENDING_TIME);
        }

        if (!GsmaManager.isServiceAvailable()) {
            return;
        }

        String rcsMsgId = sendMessageToStack(type, content);
        if (rcsMsgId == null) {
            return;
        } else {
            updateStackMsgId(msgIdInSMS, rcsMsgId);
            sendingMsg.state = SendingMessage.SNEDING;
            RCSDataBaseUtils.combineMsgId(mContext, msgIdInSMS, rcsMsgId, MessageType.IM);
        }
    }

    protected String sendMessageToStack(final int type, final String content) {
        Logger.d(TAG, "sendMessageToStack()");
        return null;
    }

    private void resendMessage(final String msgId) {
        if (RCSUtils.in2GCall(GsmaManager.getContext())) {
            onReceiveMessageDeliveryStatus(msgId, "failed");
            return;
        }

        SendingMessage sendingMsg = findSendingMessageFromList(msgId);
        if (sendingMsg != null && (sendingMsg.state == SendingMessage.SNEDING ||
                sendingMsg.state == SendingMessage.RESENDING)) {
            Logger.d(TAG, "resendChatMessage() already resending: " + msgId);
            return;
        }

        if (sendingMsg == null) {
            Logger.d(TAG, "resendMessage() setup timer msg");
            sendingMsg = addMsgToSendingList(msgId);
            Message timerMsg = mWorkHandler.obtainMessage(BASE_OP_SEND_MESSAGE_TIMEOUT, sendingMsg);
            mWorkHandler.sendMessageDelayed(timerMsg, MAX_MSG_PENDING_TIME);
        }

        if (!GsmaManager.isServiceAvailable()) {
            return;
        }

        try {
            Logger.d(TAG, "resendChatMessage() to stack. remote: " + mTag);
            updateTAPIChat();
            Logger.d(TAG, "resendChatMessage() messageId: " + msgId);
            mChatImpl.resendMessage(msgId);
            sendingMsg.state = SendingMessage.RESENDING;
        } catch (JoynServiceException e1) {
            e1.printStackTrace();
        }
    }

    @Override
    public void resendChatMessage(final String msgId) {
        Runnable worker = new Runnable() {
            @Override
            public void run() {
                resendMessage(msgId);
            }
        };
        Logger.w(TAG, "resendChatMessage() post to worker thread");
        mWorkHandler.post(worker);
    }

//    @Override
//    public void sendBurnDeliveryReport(final String msgId) {
//        Logger.w(TAG, "sendBurnDeliveryReport() Default");
//    }

    @Override
    public void sendSpamMessage(final String msgId) {
        Logger.w(TAG, "sendSpamMessage() Default");
    }

    @Override
    public abstract void onReceiveChatMessage(ExtendMessage message);

    @Override
    public void onReceiveMessageDeliveryStatus(final String msgId, final String status) {
        // status: ImdnDocument.java "sent" "delivered" "displayed" "failed" "error"
        // "display_burned"
        Logger.d(TAG, "onReceiveMessageDeliveryStatus() msgId: " + msgId + ", status: " + status);

        if (status.equalsIgnoreCase("sent")) {
            removeMsgFromSendingList(msgId);
            RCSDataBaseUtils.updateMessageStatus(mContext, msgId, MessageStatus.SENT);
        } else if (status.equalsIgnoreCase("failed")) {
            Logger.d(TAG, "onReceiveMessageDeliveryStatus() Fail, do nothing~ Wait for retry!");
            if (GsmaManager.isServiceAvailable()) {
                SendingMessage sendingmsg = findSendingMessageFromList(msgId);
                if (sendingmsg != null) {
                    Logger.d(TAG, "onReportMessageFailed() resend~");
                    sendingmsg.state = SendingMessage.STARTING;
                    mWorkHandler.obtainMessage(BASE_OP_RESEND_MESSAGE, sendingmsg).sendToTarget();
                }
            }
            // removeMsgFromSendingList(msgId);
            // RCSDataBaseUtils.updateMessageStatus(mContext, msgId, MessageStatus.FAILED);
            // long smsdbId = RCSDataBaseUtils.getRcsMessageId(mContext, msgId, Direction.OUTGOING);
            // onSendChatMessageResult(smsdbId, false);
        } else if (status.equalsIgnoreCase("delivered")) {
            RCSDataBaseUtils.updateMessageStatus(mContext, msgId, MessageStatus.DELIVERED);
        }
    }

    @Override
    public void onRegistrationStatusChanged(boolean status) {
        Logger.w(TAG, "onRegistrationStatusChanged() Default");
        if (status == true) {
            for (SendingMessage sendingMsg : mSendingMessageList) {
                if (sendingMsg.rcsMsgId != null) {
                    resendChatMessage(sendingMsg.rcsMsgId);
                } else {
                    sendChatMessage(sendingMsg.smsId, sendingMsg.content, sendingMsg.type);
                }
            }
        } else {
            for (SendingMessage sendingMsg : mSendingMessageList) {
                sendingMsg.state = SendingMessage.STARTING;
            }
        }
    }

    @Override
    public void onCapabilityChanged(Capabilities capability) {
        Logger.w(TAG, "onRegistrationStatusChanged() Default");
    }

    public static class SendingMessage {
        public static final int STARTING = 0;
        public static final int SNEDING = 1;
        public static final int RESENDING = 2;
        public static final int ENDING = 3;

        public IChat chat = null;

        public int state = -1;

        public long smsId = -1;
        public String content = null;
        public int type = -1;

        public String rcsMsgId = null;
    }

    protected SendingMessage addMsgToSendingList(long smsId, String content, int type) {
        SendingMessage sendingMsg = findSendingMessageFromList(smsId);
        if (sendingMsg != null)
            return sendingMsg;

        Logger.w(TAG, "addMsgToSendingList: " + smsId);
        sendingMsg = new SendingMessage();
        sendingMsg.state = SendingMessage.STARTING;
        sendingMsg.chat = this;
        sendingMsg.smsId = smsId;
        sendingMsg.content = content;
        sendingMsg.type = type;
        mSendingMessageList.add(sendingMsg);

        return sendingMsg;
    }

    protected SendingMessage addMsgToSendingList(String rcsMsgId) {
        SendingMessage sendingMsg = findSendingMessageFromList(rcsMsgId);
        if (sendingMsg != null)
            return sendingMsg;

        Logger.w(TAG, "addMsgToSendingList: " + rcsMsgId);
        sendingMsg = new SendingMessage();
        sendingMsg.state = SendingMessage.STARTING;
        sendingMsg.chat = this;
        sendingMsg.rcsMsgId = rcsMsgId;
        mSendingMessageList.add(sendingMsg);

        return sendingMsg;
    }

    protected SendingMessage findSendingMessageFromList(String rcsMsgId) {
        for (SendingMessage sendingMsg : mSendingMessageList) {
            if (rcsMsgId.equals(sendingMsg.rcsMsgId)) {
                Logger.w(TAG, "findSendingMessageFromList found");
                return sendingMsg;
            }
        }
        return null;
    }

    protected SendingMessage findSendingMessageFromList(long smsId) {
        for (SendingMessage sendingMsg : mSendingMessageList) {
            if (smsId == sendingMsg.smsId) {
                Logger.w(TAG, "findSendingMessageFromList found");
                return sendingMsg;
            }
        }
        return null;
    }

    protected void updateStackMsgId(long smsId, String rcsMsgId) {
        for (SendingMessage sendingMsg : mSendingMessageList) {
            if (sendingMsg.smsId == smsId) {
                Logger.w(TAG, "updateStackMsgId: smsId: " + smsId + " rcsmsgId: " + rcsMsgId);
                sendingMsg.rcsMsgId = rcsMsgId;
                sendingMsg.state = SendingMessage.SNEDING;
            }
        }
    }

    protected void removeMsgFromSendingList(String msgId) {
        Logger.w(TAG, "removeMsgFromSendingList: " + msgId);
        for (SendingMessage sendingMsg : mSendingMessageList) {
            if (sendingMsg.rcsMsgId.equals(msgId)) {
                mSendingMessageList.remove(sendingMsg);
                break;
            }
        }
    }

    protected void removeMsgFromSendingList(long msgId) {
        Logger.w(TAG, "removeMsgFromSendingList: " + msgId);
        for (SendingMessage sendingMsg : mSendingMessageList) {
            if (sendingMsg.smsId == msgId) {
                mSendingMessageList.remove(sendingMsg);
                break;
            }
        }
    }


    private static class WorkHandler extends Handler {
        static final String TAG = "WorkHandler";

        public WorkHandler(Looper looper) {
            super(looper);
        }

        @Override
        public void handleMessage(Message msg) {
            Logger.d(TAG, "handleMessage():" + msgToString(msg.what));
            switch (msg.what) {
            case BaseChatImpl.BASE_OP_RESEND_MESSAGE:
                SendingMessage sendingmsg = (SendingMessage) msg.obj;
                sendingmsg.chat.resendChatMessage(sendingmsg.rcsMsgId);
                break;
            case BaseChatImpl.BASE_OP_SEND_MESSAGE_TIMEOUT:
                sendingmsg = (SendingMessage) msg.obj;
                sendingmsg.chat.onSendChatMessageResult(sendingmsg.smsId, false);
                break;
            default:
                break;
            }
        }

        private String msgToString(int msg) {
            String result = "";
            switch (msg) {
            case BASE_OP_RESEND_MESSAGE:
                result = "BASE_OP_RESEND_MESSAGE";
                break;
            case BASE_OP_SEND_MESSAGE_TIMEOUT:
                result = "BASE_OP_SEND_MESSAGE_TIMEOUT";
            default:
                result = "NOT_HANDLED";
                break;
            }
            return result;
        }
    }
}
