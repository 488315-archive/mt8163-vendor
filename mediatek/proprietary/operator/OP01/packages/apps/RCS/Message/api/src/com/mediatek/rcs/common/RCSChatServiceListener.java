package com.mediatek.rcs.common;

import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.gsma.joyn.chat.ChatLog;

import android.content.ContentUris;
import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.database.Cursor;
import android.net.Uri;
import android.os.RemoteException;
import android.provider.Telephony.Mms;
import android.provider.Telephony.Threads;

import com.mediatek.rcs.common.GroupManager;
import com.mediatek.rcs.common.IpMessageConsts;
import com.mediatek.rcs.common.IpMessageConsts.GroupActionList;
import com.mediatek.rcs.common.IpMessageConsts.MessageAction;
import com.mediatek.rcs.common.provider.GroupMemberData;
import com.mediatek.rcs.common.provider.RCSDataBaseUtils;
import com.mediatek.rcs.common.provider.GroupChatCache;
import com.mediatek.rcs.common.provider.GroupChatCache.ChatInfo;
import com.mediatek.rcs.common.service.IRCSChatServiceListener;
import com.mediatek.rcs.common.service.Participant;
import com.mediatek.rcs.common.utils.ContextCacher;
import com.mediatek.rcs.common.utils.Logger;
import com.mediatek.rcs.common.utils.RCSUtils;
import com.mediatek.rcs.common.GroupManager.GroupInfo;
import com.mediatek.rcs.common.GroupManager.IInitGroupListener;
import com.mediatek.rcs.common.RcsLog.MessageColumn;
import com.mediatek.rcs.common.RcsLog.MessageStatus;
import com.mediatek.rcs.common.MessageStatusUtils.IFileTransfer.Status;
import com.mediatek.rcs.common.RcsLog.ThreadFlag;
import com.mediatek.rcs.common.RcsLog.ThreadsColumn;

import android.telephony.PhoneNumberUtils;
import android.text.TextUtils;
import android.util.Log;
import com.mediatek.rcs.common.RCSCacheManager;

/**
 * RCSChatServiceListener. Register this listener to service, listen message delivery.
 *
 */
public class RCSChatServiceListener extends IRCSChatServiceListener.Stub {

//    private final String INVITATION_PREFERENCE = "invitation_preference";

    private static final String TAG = "RCSChatServiceListener";
    private RCSServiceManager mRcsServiceManager;

    /**
     * RCSChatServiceListener Constructor.
     */
    public RCSChatServiceListener(RCSServiceManager rcsServiceManager) {
        mRcsServiceManager = rcsServiceManager;
    }

    @Override
    public void onNewMessage(long msgId) throws RemoteException {
        Logger.d(TAG, "onNewMessage, msgId=" + msgId);
        // FIXME: should handle not default app issue
        if (msgId <= 0) {
            Logger.d(TAG, "no inserted sms, maybe not default ap");
            return;
        }
        long threadId = 0;
        long ipmsgId = 0;
        String number = null;
        Cursor cursor = getMessageInfo(msgId);
        try {
            if (cursor != null && cursor.moveToFirst()) {
                threadId = cursor.getLong(cursor.getColumnIndex(MessageColumn.CONVERSATION));
                ipmsgId = cursor.getLong(cursor.getColumnIndex(MessageColumn.IPMSG_ID));
                number = cursor.getString(cursor.getColumnIndex(MessageColumn.CONTACT_NUMBER));
            }
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }
        Logger.d(TAG, "notifyNewMessage, entry: threadId=" + threadId + ", ipmsgId=" + ipmsgId);
        notifyNewMessage(threadId, ipmsgId, number, msgId);
    }

    @Override
    public void onNewGroupMessage(String chatId, long msgId, String number) throws RemoteException {
        Logger.d(TAG, "onNewGroupMessage, chatId=" + chatId);
        // FIXME: should handle not default app issue
        if (msgId <= 0) {
            Logger.d(TAG, "no inserted sms, maybe not default ap");
            return;
        }
        ChatInfo info = GroupChatCache.getInstance().getInfoByChatId(chatId);
        long threadId = RCSDataBaseUtils.getGroupConversationId(
                ContextCacher.getHostContext(), chatId);
        if (info != null) {
            int subId = RCSUtils.getRCSSubId();
            if (info.getSubId() != subId) {
                GroupChatCache.getInstance().updateSubId(chatId, subId);
                GroupChatCache.getInstance().updateStatusByChatId(chatId, subId);
            }
        }
        long ipmsgId = 0;
        Cursor cursor = getMessageInfo(msgId);
        try {
            if (cursor != null && cursor.moveToFirst()) {
                ipmsgId = cursor.getLong(cursor.getColumnIndex(MessageColumn.IPMSG_ID));
            }
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }
        Logger.d(TAG, "notifyNewMessage, entry: threadId=" + threadId + ", ipmsgId=" + ipmsgId);
        notifyNewMessage(threadId, ipmsgId, number, msgId);
    }

    private void notifyNewMessage(long threadId, long ipmsgId, String number, long msgId) {
        Intent intent = new Intent();
        intent.setAction(MessageAction.ACTION_NEW_MESSAGE);
        intent.putExtra(MessageAction.KEY_THREAD_ID, threadId);
        intent.putExtra(MessageAction.KEY_IPMSG_ID, ipmsgId);
        intent.putExtra(MessageAction.KEY_NUMBER, number);
        intent.putExtra(MessageAction.KEY_MSG_ID, msgId);
        notifyAllListeners(intent);
    }

    private Cursor getMessageInfo(long msgId) {
        return RCSDataBaseUtils.getMessage(ContextCacher.getHostContext(), msgId);
    }

    @Override
    public void onSendMessageFailed(long msgId) throws RemoteException {
        Intent intent = new Intent(MessageAction.ACTION_SEND_FAIL);
        intent.putExtra(MessageAction.KEY_MSG_ID, msgId);
        mRcsServiceManager.callNotifyListeners(intent);
    }

    @Override
    public void onParticipantJoined(String chatId, Participant participant) throws RemoteException {
        Logger.d(TAG, "onParticipantJoined, chatId=" + chatId + ", " + participant.getContact());
//        updateGroupAddress(chatId);
        notifyParticipantOperation(chatId, GroupActionList.VALUE_PARTICIPANT_JOIN, participant);
    }

    @Override
    public void onParticipantLeft(String chatId, Participant participant) throws RemoteException {
        Logger.d(TAG, "onParticipantLeft, chatId=" + chatId + ", " + participant.getContact());
//        updateGroupAddress(chatId);
        notifyParticipantOperation(chatId, GroupActionList.VALUE_PARTICIPANT_LEFT, participant);
    }

    @Override
    public void onParticipantRemoved(String chatId, Participant participant)
                                                                throws RemoteException {
        Logger.d(TAG, "onParticipantRemoved, chatId=" + chatId + ", " + participant.getContact());
//        updateGroupAddress(chatId);
        notifyParticipantOperation(chatId, GroupActionList.VALUE_PARTICIPANT_REMOVED, participant);
    }

    @Override
    public void onChairmenChanged(String chatId, Participant participant, boolean isMe)
            throws RemoteException {
        Logger.d(TAG, "onChairmenChanged, chatId=" + chatId + ", " + participant.getContact());
//        ensureGroupThreadId(chatId);
        GroupChatCache.getInstance().updateIsMeChairmenByChatId(chatId, isMe);
        notifyParticipantOperation(chatId, GroupActionList.VALUE_CHAIRMEN_CHANGED, participant);
    }

    @Override
    public void onSubjectModified(String chatId, String subject) throws RemoteException {
        Logger.d(TAG, "onSubjectModified, chatId=" + chatId + ", subject=" + subject);
        GroupChatCache.getInstance().updateSubjectByChatId(chatId, subject);
//        ensureGroupThreadId(chatId);
        notifyStringOperation(chatId, GroupActionList.VALUE_SUBJECT_MODIFIED,
                GroupActionList.KEY_SUBJECT, subject);
    }

    @Override
    public void onParticipantNickNameModified(String chatId, String contact, String nickName)
            throws RemoteException {
        Logger.d(TAG, "onParticipantNickNameChanged,contact=" + contact + ", nickName=" + nickName);
//        ensureGroupThreadId(chatId);
        Intent intent = getNotifyIntent(chatId,
                GroupActionList.ACTION_GROUP_NOTIFY,
                GroupActionList.VALUE_PARTICIPANT_NICKNAME_MODIFIED);
        intent.putExtra(GroupActionList.KEY_CONTACT_NUMBER, contact);
        intent.putExtra(GroupActionList.KEY_NICK_NAME, nickName);
        notifyAllListeners(intent);
    }

    @Override
    public void onMeRemoved(String chatId, String contact) throws RemoteException {
        Logger.d(TAG, "onMeRemoved, contact = " + contact);
//        ensureGroupThreadId(chatId);
        markGroupUnavailable(chatId);
        notifyStringOperation(chatId, GroupActionList.VALUE_ME_REMOVED,
                GroupActionList.KEY_CONTACT_NUMBER, contact);
    }

    @Override
    public void onAbort(String chatId) throws RemoteException {
        Logger.d(TAG, "onAbort, chatId=" + chatId);
//        ensureGroupThreadId(chatId);
        markGroupUnavailable(chatId);

        notifyNullParamOperation(chatId, GroupActionList.VALUE_GROUP_ABORTED);
    }

    @Override
    public void onNewInvite(Participant participant, String subject, String chatId)
            throws RemoteException {
        Context context = ContextCacher.getHostContext();
        Set<String> contacts = new HashSet<String>();
        contacts.add(participant.getContact());
        long threadId = RCSDataBaseUtils.getGroupConversationId(context, chatId);
        GroupChatCache.getInstance().addChatInfo(chatId, subject,
            GroupActionList.GROUP_STATUS_INVITING, false);
        Logger.d(TAG, "onNewInvite, receive new invite, chatId=" + chatId + ", threadId="
                + threadId);

        String name = RCSDataBaseUtils.getContactDisplayName(ContextCacher.getPluginContext(),
                participant.getContact());
        Participant contact = new Participant(participant.getContact(), name);
        Intent intent = getNotifyIntent(chatId,
                GroupActionList.ACTION_GROUP_NOTIFY,
                GroupActionList.VALUE_NEW_INVITE_RECEIVED);
        intent.putExtra(GroupActionList.KEY_PARTICIPANT, contact);
        intent.putExtra(GroupActionList.KEY_SUBJECT, subject);
        intent.putExtra(GroupActionList.KEY_ADD_SYS_EVENT, true);
        notifyAllListeners(intent);
    }

    @Override
    public void onAddParticipantsResult(String chatId, boolean result) throws RemoteException {
        Logger.d(TAG, "onAddParticipantsResult, result=" + result + ", chatId=" + chatId);
        notifyResultOperation(chatId, GroupActionList.VALUE_ADD_PARTICIPANTS,
                result);
    }

    @Override
    public void onRemoveParticipantsResult(String chatId, boolean result) throws RemoteException {
        Logger.d(TAG, "onRemoveParticipantsResult, result=" + result + ", chatId=" + chatId);
        notifyResultOperation(chatId, GroupActionList.VALUE_REMOVE_PARTICIPANTS, result);
    }

    @Override
    public void onTransferChairmenResult(String chatId, boolean result) throws RemoteException {
        Logger.d(TAG, "onTransferChairmenResult, result = " + result + ", chatId=" + chatId);
//        ensureGroupThreadId(chatId);
        if (result) {
            GroupChatCache.getInstance().updateIsMeChairmenByChatId(chatId, false);
        }
        notifyResultOperation(chatId, GroupActionList.VALUE_TRANSFER_CHAIRMEN, result);
    }

    @Override
    public void onMyNickNameModifiedResult(String chatId, boolean result) throws RemoteException {
        Logger.d(TAG, "onMyNickNameModifiedResult, result = " + result + ", chatId=" + chatId);
        String myNickName = getMyNickName(chatId);

        notifyResultOperationWithStringArg(chatId,
                GroupActionList.VALUE_MODIFY_SELF_NICK_NAME, result,
                GroupActionList.KEY_SELF_NICK_NAME, myNickName);
    }

    @Override
    public void onSubjectModifiedResult(String chatId, boolean result) throws RemoteException {
        Logger.d(TAG, "onSubjectModifiedResult, result=" + result + ", chatId=" + chatId);
//        ensureGroupThreadId(chatId);
        String subject = getSubject(chatId);
        GroupChatCache.getInstance().updateSubjectByChatId(chatId, subject);
        notifyResultOperationWithStringArg(chatId,
                GroupActionList.VALUE_MODIFY_SUBJECT, result,
                GroupActionList.KEY_SUBJECT, subject);
    }

    @Override
    public void onQuitConversationResult(String chatId, boolean result) throws RemoteException {
        Logger.d(TAG, "onQuitConversationResult, result=" + result + ", chatId=" + chatId);
        if (result) {
            markGroupUnavailable(chatId);
        }
        notifyResultOperation(chatId, GroupActionList.VALUE_EXIT_GROUP, result);
    }

    @Override
    public void onAbortResult(String chatId, boolean result) throws RemoteException {
        Logger.d(TAG, "onAbortResult, result=" + result + ", chatId=" + chatId);
        if (result) {
            markGroupUnavailable(chatId);
        }
        notifyResultOperation(chatId, GroupActionList.VALUE_DESTROY_GROUP, result);
    }

    @Override
    public void onInitGroupResult(boolean result, String chatId) throws RemoteException {
        Logger.d(TAG, "onInitGroupResult, result=" + result + ", chatId=" + chatId);
        String subject = null;
        List<String> participants = null;
        GroupInfo groupInfo = mRcsServiceManager.getGroupManager().getInvitingGroup(chatId);
        if (groupInfo != null) {
            subject = groupInfo.getSubject();
            participants = groupInfo.getParticipants();
        }
        long threadId = 0;
        if (result) {
            threadId = RCSDataBaseUtils.getGroupConversationId(ContextCacher.getHostContext(),
                    chatId);
            Logger.d(TAG, "onInitGroupResult, threadId=" + threadId + ", subject=" + subject);
            GroupChatCache.getInstance().addChatInfo(chatId, subject,
                    RCSUtils.getRCSSubId(), true);
//            notifyInitGroupResult(chatId, GroupActionList.VALUE_INIT_GROUP,
//                    result, participants);
        }
        List<IInitGroupListener> listeners = mRcsServiceManager.getGroupManager().getAllListeners();
        int success = (result == true) ? GroupActionList.VALUE_SUCCESS
                : GroupActionList.VALUE_FAIL;
        for (IInitGroupListener listener : listeners) {
            listener.onInitGroupResult(success, threadId, chatId);
        }
        if (!TextUtils.isEmpty(chatId)) {
            mRcsServiceManager.getGroupManager().removeInvitingGroup(chatId);
        }
    }

    @Override
    public void onRequestBurnMessageCapabilityResult(String contact, boolean result)
            throws RemoteException {
        Log.d(TAG, "onRequestBurnMessageCapabilityResult contact = " + contact + " result = "
                + result);
        mRcsServiceManager.callBurnCapListener(contact, result);
    }

    @Override
    public void onAcceptInvitationResult(String chatId, boolean result) throws RemoteException {
        if (result) {
            updateGroupStatus(chatId, RCSUtils.getRCSSubId());
        }
        notifyResultOperation(chatId, GroupActionList.VALUE_ACCEPT_GROUP_INVITE,
                result);
        List<IInitGroupListener> listeners = mRcsServiceManager.getGroupManager().getAllListeners();
        long threadId = RCSDataBaseUtils.getGroupConversationId(
                ContextCacher.getHostContext(), chatId);
        int success = (result == true) ? GroupActionList.VALUE_SUCCESS
                : GroupActionList.VALUE_FAIL;
        for (IInitGroupListener listener : listeners) {
            listener.onAcceptGroupInvitationResult(success, threadId, chatId);
        }
    }

    @Override
    public void onRejectInvitationResult(String chatId, boolean result) throws RemoteException {
        Log.d(TAG, "onRejectInvitationResult. chatId = " + chatId + ", result = " + result);
        result = true;
        notifyResultOperation(chatId, GroupActionList.VALUE_REJECT_GROUP_INVITE, result);
        List<IInitGroupListener> listeners = mRcsServiceManager.getGroupManager().getAllListeners();
        long threadId = RCSDataBaseUtils.getGroupConversationId(
                ContextCacher.getHostContext(), chatId);
        int success = (result == true) ? GroupActionList.VALUE_SUCCESS
                : GroupActionList.VALUE_FAIL;
        for (IInitGroupListener listener : listeners) {
            listener.onRejectGroupInvitationResult(success, threadId, chatId);
        }
    }

    @Override
    public void onUpdateFileTransferStatus(long ipMsgId, int rcsStatus, int smsStatus) {
        Log.d(TAG, "onUpdateFileTransferStatus , ipMsgId = " + ipMsgId + " rcsStatus = "
                + rcsStatus + " smsStatus " + smsStatus);
        Status stat = RCSUtils.getRcsStatus(rcsStatus);
        Intent it = new Intent();
        it.setAction(IpMessageConsts.IpMessageStatus.ACTION_MESSAGE_STATUS);
        it.putExtra(IpMessageConsts.STATUS, stat);
        it.putExtra(IpMessageConsts.IpMessageStatus.IP_MESSAGE_ID, ipMsgId);
        mRcsServiceManager.callNotifyListeners(it);
        RCSCacheManager.updateStatus(ipMsgId, stat, smsStatus);
    }

    @Override
    public void setFilePath(long ipMsgId, String filePath) {
        Log.d(TAG, "setFilePath , ipMsgId = " + ipMsgId + " filePath = " + filePath);
        RCSCacheManager.setFilePath(ipMsgId, filePath);
    }

    @Override
    public void onSyncFullConferenceNotifyDone(String chatId) {
        Logger.d(TAG, "onSyncFullConferenceNotifyDone, chatId = " + chatId);
        notifyNullParamOperation(chatId, GroupActionList.VALUE_SYNC_CONFERENCE_NOTIFY_DONE);
    }

    @Override
    public void onSpamReportResult(boolean result) {
        // TODO
    }

    /** temp remove
    private int updateGroupAddress(String chatId) {
        Logger.d(TAG, "updateGroupAddress, entry");
        Set<String> participants = RCSDataBaseUtils.getGroupAvailableParticipants(chatId);
        long threadId = ensureGroupThreadId(chatId);
        Uri.Builder uriBuilder = RCSUtils.MMS_SMS_URI_GROUP_ADDRESS.buildUpon();
        String recipient = null;
        for (String participant : participants) {
            if (Mms.isEmailAddress(participant)) {
                recipient = Mms.extractAddrSpec(recipient);
            }
            Logger.d(TAG, "recipient = " + recipient);
            uriBuilder.appendQueryParameter("recipient", recipient);
        }
        return ContextCacher
                .getHostContext()
                .getContentResolver()
                .update(ContentUris.withAppendedId(uriBuilder.build(), threadId),
                        new ContentValues(1), null, null);
    }

    private void notifyInitGroupResult(String chatId, int actionType, boolean result,
            List<String> participants) {
        Logger.d(TAG, "notifyInitGroupResult, entry: actionType = " + actionType + ", result="
                + result);
        int success = (result == true) ? GroupActionList.VALUE_SUCCESS
                : GroupActionList.VALUE_FAIL;
        Intent intent = getNotifyIntent(chatId,
                GroupActionList.ACTION_GROUP_OPERATION_RESULT, actionType);
        intent.putExtra(GroupActionList.KEY_ACTION_RESULT, success);
        String[] parts = new String[participants.size()];
        int i = 0;
        for (String participant : participants) {
            parts[i++] = RCSDataBaseUtils.getContactDisplayName(ContextCacher.getPluginContext(),
                    participant);
        }
        intent.putExtra(GroupActionList.KEY_PARTICIPANT_LIST, parts);
        notifyAllListeners(intent);
    }
    **/
    private void notifyParticipantOperation(String chatId, int actionType,
            Participant participant) {
        Logger.d(TAG, "notifyParticipantOperation, entry: actionType = " + actionType);
        Intent intent = getNotifyIntent(chatId,
                GroupActionList.ACTION_GROUP_NOTIFY, actionType);
        String name = RCSDataBaseUtils.getContactDisplayName(ContextCacher.getPluginContext(),
                participant.getContact());
        Participant contact = new Participant(participant.getContact(), name);
        intent.putExtra(GroupActionList.KEY_PARTICIPANT, contact);
        notifyAllListeners(intent);
    }

    private Intent getNotifyIntent(String chatId, String action, int actionType) {
        long threadId = RCSDataBaseUtils.getGroupConversationId(
                ContextCacher.getHostContext(), chatId);
        Intent intent = new Intent();
        intent.setAction(action);
        intent.putExtra(GroupActionList.KEY_ACTION_TYPE, actionType);
        intent.putExtra(GroupActionList.KEY_CHAT_ID, chatId);
        intent.putExtra(GroupActionList.KEY_THREAD_ID, threadId);
        return intent;
    }

    private void notifyStringOperation(String chatId, int actionType, String key, String value) {
        Logger.d(TAG, "notifyStringOperation, entry: actionType = " + actionType + ", key=" + key
                + ", value=" + value);
        Intent intent = getNotifyIntent(chatId,
                GroupActionList.ACTION_GROUP_NOTIFY, actionType);
        intent.putExtra(key, value);
        notifyAllListeners(intent);
    }

    private void markGroupUnavailable(String chatId) {
        Logger.d(TAG, "markGroupUnavailable, chatId=" + chatId);
        updateGroupStatus(chatId, GroupActionList.GROUP_STATUS_INVALID);
        Intent intent = new Intent("com.mediatek.rcs.groupchat.STATE_CHANGED");
        intent.putExtra("chat_id", chatId);
        ContextCacher.getPluginContext().sendBroadcast(intent);
    }

    private void updateGroupStatus(String chatId, long status) {
        Logger.d(TAG, "updateGroupStatus, chatId=" + chatId + ", status=" + status);
        ChatInfo info = GroupChatCache.getInstance().getInfoByChatId(chatId);
        if (info != null) {
            GroupChatCache.getInstance().updateStatusByChatId(chatId, status);
        } else {
            Logger.e(TAG, "error: can not find chatId, maybe no invite come to AP");
        }
    }

    private void notifyNullParamOperation(String chatId, int actionType) {
        Logger.d(TAG, "notifyNullParamOperation, entry: actionType = " + actionType);
        Intent intent = getNotifyIntent(chatId,
                GroupActionList.ACTION_GROUP_NOTIFY, actionType);
        notifyAllListeners(intent);
    }

    private void notifyResultOperation(String chatId, int actionType, boolean result) {
        Logger.d(TAG, "notifyResultOperation, entry: actionType = " + actionType + ", result="
                + result);
        int success = (result == true) ? GroupActionList.VALUE_SUCCESS
                : GroupActionList.VALUE_FAIL;
        Intent intent = getNotifyIntent(chatId,
                GroupActionList.ACTION_GROUP_OPERATION_RESULT, actionType);
        intent.putExtra(GroupActionList.KEY_ACTION_RESULT, success);
        notifyAllListeners(intent);
    }

    private void notifyResultOperationWithStringArg(String chatId, int actionType, boolean result,
            String key, String value) {
        Logger.d(TAG, "notifyResultOperationWithStringArg, entry: actionType = " + actionType
                + ", result=" + result);
        int success = (result == true) ? GroupActionList.VALUE_SUCCESS
                : GroupActionList.VALUE_FAIL;
        Intent intent = getNotifyIntent(chatId,
                GroupActionList.ACTION_GROUP_OPERATION_RESULT, actionType);
        intent.putExtra(key, value);
        intent.putExtra(GroupActionList.KEY_ACTION_RESULT, success);
        notifyAllListeners(intent);
    }

    private void notifyAllListeners(Intent intent) {
//        long threadId = intent.getLongExtra(GroupActionList.KEY_THREAD_ID, 0);
//        Logger.d(TAG, "threadId = " + threadId);
//        if (threadId != 0) {
            mRcsServiceManager.callNotifyListeners(intent);
//        }
    }

    private String getSubject(String chatId) {
        Logger.d(TAG, "getSubject, chatId=" + chatId);
        String chatSelection = ChatLog.GroupChat.CHAT_ID + "='" + chatId + "'";
        Cursor chatCursor = null;
        try {
            chatCursor = ContextCacher.getPluginContext().getContentResolver()
                    .query(RCSUtils.RCS_URI_GROUP_CHAT, RCSUtils.PROJECTION_GROUP_INFO,
                            chatSelection, null, null);
            if (chatCursor.moveToFirst()) {
                return chatCursor.getString(chatCursor.getColumnIndex(ChatLog.GroupChat.SUBJECT));
            }
        } finally {
            if (chatCursor != null) {
                chatCursor.close();
            }
        }
        return null;
    }

    private String getMyNickName(String chatId) {
        String memberSelection = GroupMemberData.COLUMN_CHAT_ID + "='" + chatId + "'";
        Cursor cursor = ContextCacher
                .getPluginContext()
                .getContentResolver()
                .query(RCSUtils.RCS_URI_GROUP_MEMBER, RCSUtils.PROJECTION_GROUP_MEMBER,
                        memberSelection, null, null);
        String myNickName = null;
        String myNumber = mRcsServiceManager.getMyNumber();
        try {
            while (cursor.moveToNext()) {
                String number = cursor.getString(cursor
                        .getColumnIndex(GroupMemberData.COLUMN_CONTACT_NUMBER));
                String name = cursor.getString(cursor
                        .getColumnIndex(GroupMemberData.COLUMN_CONTACT_NAME));
                if (PhoneNumberUtils.compare(number, myNumber)) {
                    myNickName = name;
                    break;
                }
            }
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }
        return myNickName;
    }

//    private boolean addInvitationRejectRecord(Context context, String chatId) {
//        SharedPreferences sp = context.getSharedPreferences(INVITATION_PREFERENCE,
//                Context.MODE_WORLD_WRITEABLE);
//        Editor editor = sp.edit();
//        editor.putBoolean(chatId, true);
//        Logger.d(TAG, "[addInvitationRejectRecord], chatId=" + chatId);
//        return editor.commit();
//    }
//
//    private boolean removeInvitationRejectRecord(Context context, String chatId) {
//        SharedPreferences sp = context.getSharedPreferences(INVITATION_PREFERENCE,
//                Context.MODE_WORLD_WRITEABLE);
//        Editor editor = sp.edit();
//        editor.remove(chatId);
//        Logger.d(TAG, "[removeInvitationRejectRecord], chatId=" + chatId);
//        return editor.commit();
//    }
//
//    private boolean getInvitationRejectedState(Context context, String chatId) {
//        SharedPreferences sp = context.getSharedPreferences(INVITATION_PREFERENCE,
//                Context.MODE_WORLD_READABLE);
//        boolean value = sp.getBoolean(chatId, false);
//        return value;
//    }

//    public void handleSpamReportResult(String contact, String msgId, int errorcode)
//        throws RemoteException {
//        Log.d(TAG, " [spam-report] handleSpamReportResult contact = " + contact+
//                " msgId = " + msgId+ " errorCode = "+errorcode);
//        mRcsServiceManager.handleSpamReportResult(contact,msgId,errorcode);
//    }
//
//    public void handleFileSpamReportResult(String contact, String ftId, int errorcode)
//        throws RemoteException {
//        Log.d(TAG, "[spam-report] handleFileSpamReportResult contact = " + contact+
//                " ftId = " + ftId + " errorCode = "+errorcode);
//        mRcsServiceManager.handleFileSpamReportResult(contact,ftId,errorcode);
//    }

//    /**
//     * Ensure group thread id is right. When receive group event exception invitation.
//     * Must ensure the thread id is exit.
//     *
//     * @param chatId group chat id
//     * @return real thread id
//     */
//    private long ensureGroupThreadId(String chatId) {
//        Set<String> participants = RCSDataBaseUtils
//                .getGroupAvailableParticipants(ContextCacher.getHostContext(), chatId);
//        long threadId = RCSDataBaseUtils.getOrCreateConversationId(ContextCacher.getHostContext(),
//                chatId, participants, RCSUtils.getRCSSubId());
//        return threadId;
//    }
}
