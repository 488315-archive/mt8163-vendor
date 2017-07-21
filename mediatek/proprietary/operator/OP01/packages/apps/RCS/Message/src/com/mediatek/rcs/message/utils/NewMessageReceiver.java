package com.mediatek.rcs.message.utils;

import android.content.Context;
import android.content.Intent;
import android.database.Cursor;

import com.mediatek.rcs.common.IpMessageConsts.GroupActionList;
import com.mediatek.rcs.common.IpMessageConsts.MessageAction;
import com.mediatek.rcs.common.RcsLog.Class;
import com.mediatek.rcs.common.RcsLog.MessageColumn;
import com.mediatek.rcs.common.RcsLog.MessageStatus;
import com.mediatek.rcs.common.RcsLog.ThreadFlag;
import com.mediatek.rcs.common.RCSServiceManager;
import com.mediatek.rcs.common.RCSServiceManager.INotifyListener;
import com.mediatek.rcs.common.provider.RCSDataBaseUtils;
import com.mediatek.rcs.common.service.Participant;
import com.mediatek.rcs.common.utils.ContextCacher;
import com.mediatek.rcs.common.utils.Logger;
import com.mediatek.rcs.message.SendModeChangedNotifyService;
import com.mediatek.rcs.message.group.PortraitManager;
import com.mediatek.rcs.message.plugin.RcsComposeActivity;
import com.mediatek.rcs.message.plugin.RcsMessagingNotification;
import com.mediatek.rcs.message.plugin.RcsUtilsPlugin;

public class NewMessageReceiver implements INotifyListener {

    private static final String TAG = "NewMessageReceiver";

    private static NewMessageReceiver sInstance;
    private Context mContext;
    private Context mPluginContext;

    private static final int MAX_LENGTH_OF_TRANSFER_TO_SMS_WHEN_SEND_FAILED = 450;

    private NewMessageReceiver(Context context) {
        mContext = context;
        RCSServiceManager.getInstance().registNotifyListener(this);
        mPluginContext = ContextCacher.getPluginContext();
    }

    public static void init(Context context) {
        if (sInstance == null) {
            sInstance = new NewMessageReceiver(context);
        }
    }

    public static NewMessageReceiver getInstance() {
        if (sInstance == null) {
            throw new RuntimeException("NewMessageReceiver not inited");
        }
        return sInstance;
    }

    public void notificationsReceived(Intent intent) {
        String action = intent.getAction();
        if (action.equals(MessageAction.ACTION_NEW_MESSAGE)) {
            long threadId = intent.getLongExtra(MessageAction.KEY_THREAD_ID, 0);
            long msgId = intent.getLongExtra(MessageAction.KEY_MSG_ID, 0);
            RcsUtilsPlugin.unblockingNotifyNewIpMessage(mContext, threadId, msgId);
            String number = intent.getStringExtra(MessageAction.KEY_NUMBER);
            String chatId = RcsMessageUtils.blockingGetGroupChatIdByThread(mContext, threadId);
            if (chatId != null) {
                PortraitManager.getInstance().updateMemberRequeryState(chatId, number, true);
            }
        } else if (action.equals(GroupActionList.ACTION_GROUP_NOTIFY)) {
            int actionType = intent.getIntExtra(GroupActionList.KEY_ACTION_TYPE, 0);
            if (actionType == GroupActionList.VALUE_NEW_INVITE_RECEIVED) {
                long threadId = intent.getLongExtra(GroupActionList.KEY_THREAD_ID, 0);
                String stringArg = intent.getStringExtra(GroupActionList.KEY_SUBJECT);
                Participant participant = intent.getParcelableExtra(
                        GroupActionList.KEY_PARTICIPANT);
                String chatId = intent.getStringExtra(GroupActionList.KEY_CHAT_ID);
                RcsMessagingNotification.updateNewGroupInvitation(participant,
                        stringArg, threadId, chatId);
            }
        } else if (action.equals(MessageAction.ACTION_SEND_FAIL)) {
            processFailedMessage(intent);
        }
    }

    @Override
    public void finalize() {
        Logger.d(TAG, "finalize() called");
        try {
            RCSServiceManager.getInstance().unregistNotifyListener(this);
            super.finalize();
        } catch (Throwable e) {
            e.printStackTrace();
        }
    }

    /**
     * Process failed item
     * @param intent Intent
     */
    private void processFailedMessage(Intent intent) {
        long msgId = intent.getLongExtra(MessageAction.KEY_MSG_ID, 0);
        Cursor cursor = RCSDataBaseUtils.getMessage(mContext, msgId);
        try {
            if (cursor != null && cursor.moveToFirst()) {
                long threadId = cursor.getLong(cursor.getColumnIndex(MessageColumn.CONVERSATION));
                long ipmsgId = cursor.getLong(cursor.getColumnIndex(MessageColumn.IPMSG_ID));
                String body = cursor.getString(cursor.getColumnIndex(MessageColumn.BODY));
                int msgType = cursor.getInt(cursor.getColumnIndex(MessageColumn.CLASS));
                int subId = cursor.getInt(cursor.getColumnIndex(MessageColumn.SUB_ID));
                int flag = cursor.getInt(cursor.getColumnIndex(MessageColumn.FLAG));
                if (body.length() > MAX_LENGTH_OF_TRANSFER_TO_SMS_WHEN_SEND_FAILED ||
                        msgType != Class.NORMAL || flag == ThreadFlag.MTM ||
                        !RcsMessageUtils.isSmsEnabled(mContext)) {
                    RCSDataBaseUtils.updateMessageStatus(mContext, msgId, MessageStatus.FAILED);
                } else if (RcsMessageUtils.isNeedNotifyUserWhenToSms(mPluginContext, subId)) {
                    // notify
                    String address = cursor.getString(
                            cursor.getColumnIndex(MessageColumn.CONTACT_NUMBER));
                    Intent service = SendModeChangedNotifyService.createSmsNotifyIntent(msgId,
                            ipmsgId, threadId, subId, body, address);
                    mContext.startService(service);
                } else {
                    if (RcsMessageUtils.isTransferToSMSWhenSendFailed(mPluginContext, subId)) {
                        String address = cursor.getString(
                                cursor.getColumnIndex(MessageColumn.CONTACT_NUMBER));
                        RcsMessageUtils.transferToSMSFromFailedRcsMessage(mContext, threadId,
                                msgId, ipmsgId, subId, address, body);
                    } else {
                        // send failed.
                        RCSDataBaseUtils.updateMessageStatus(mContext, msgId, MessageStatus.FAILED);
                    }
                }
            }
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }
    }
}
