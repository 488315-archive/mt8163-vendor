package com.mediatek.rcs.messageservice.chat;

import android.content.ContentValues;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.database.DatabaseUtils;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.RemoteException;
import android.provider.Telephony.Threads;

import com.mediatek.rcs.common.IpMessageConsts;
import com.mediatek.rcs.common.IpMessageConsts.GroupActionList;
import com.mediatek.rcs.common.RcsLog.MessageColumn;
import com.mediatek.rcs.common.RcsLog.Class;
import com.mediatek.rcs.common.RcsLog.InvitationStatus;
import com.mediatek.rcs.common.RcsLog.MessageStatus;
import com.mediatek.rcs.common.RcsLog.ThreadFlag;
import com.mediatek.rcs.common.RcsLog.ThreadsColumn;

import com.mediatek.rcs.common.provider.RCSDataBaseUtils;
import com.mediatek.rcs.common.service.IRCSChatServiceListener;
import com.mediatek.rcs.common.service.Participant;
import com.mediatek.rcs.common.utils.RCSUtils;

import com.mediatek.rcs.messageservice.utils.Logger;
import com.mediatek.rcs.messageservice.R;

public class RCSChatServiceListenerWrapper extends IRCSChatServiceListener.Stub implements
        IBinder.DeathRecipient {
    public static final String TAG = "RCSChatServiceListenerWrapper";

    IRCSChatServiceListener mListenerImpl = null;
    Handler mHandler = null;
    Context mContext = null;

    RCSChatServiceListenerWrapper(Context context) {
        mContext = context;

        HandlerThread thread = new HandlerThread(TAG);
        thread.start();
        mHandler = new Handler(thread.getLooper());
    }

    void addListener(IRCSChatServiceListener listener) {
        Logger.d(TAG, "addListener:" + listener);
        mListenerImpl = listener;
    }

    void removeListener(IRCSChatServiceListener listener) {
        Logger.d(TAG, "removeListener:" + listener);
        if (mListenerImpl == listener) {
            mListenerImpl = null;
        }
    }

    IRCSChatServiceListener getListener() {
        return mListenerImpl;
    }

    @Override
    public void binderDied() {
        Logger.d(TAG, "RCS Plug in App binderDied");
        mListenerImpl = null;
    }

    @Override
    public void onNewMessage(final long msgId) {
        Logger.d(TAG, "onNewMessage:" + msgId);
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    if (mListenerImpl != null) {
                        mListenerImpl.onNewMessage(msgId);
                    } else {
                        Intent intent = new Intent(
                                IpMessageConsts.ServiceNotification.BROADCAST_RCS_NEW_MESSAGE);
                        intent.setClassName("com.android.mms", "com.mediatek.rcs.EmptyReceiver");
                        intent.putExtra(IpMessageConsts.ServiceNotification.KEY_MSG_ID, msgId);
                        mContext.sendBroadcast(intent);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });

    }

    @Override
    public void onNewGroupMessage(final String chatId, final long msgId, final String number) {
        Logger.d(TAG, "onNewGroupMessage chatId/msgId: " + chatId + "/" + msgId);
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    if (mListenerImpl != null) {
                        mListenerImpl.onNewGroupMessage(chatId, msgId, number);
                    } else {
                        Intent intent = new Intent(
                                IpMessageConsts.ServiceNotification.
                                BROADCAST_RCS_GROUP_NEW_MESSAGE);
                        intent.setClassName("com.android.mms", "com.mediatek.rcs.EmptyReceiver");
                        intent.putExtra(IpMessageConsts.ServiceNotification.KEY_CHAT_ID, chatId);
                        intent.putExtra(IpMessageConsts.ServiceNotification.KEY_MSG_ID, msgId);
                        intent.putExtra(IpMessageConsts.ServiceNotification.KEY_CONTACT, number);
                        mContext.sendBroadcast(intent);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    public void onSendMessageFailed(final long msgId) {
        Logger.d(TAG, "onSendO2OMessageFailed:" + msgId);
        if (mListenerImpl == null)
            return;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    if (mListenerImpl != null) {
                        mListenerImpl.onSendMessageFailed(msgId);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    public void onRequestBurnMessageCapabilityResult(final String contact, final boolean result) {
        Logger.d(TAG, "onRequestBurnMessageCapabilityResult:" + contact);
        if (mListenerImpl == null)
            return;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    if (mListenerImpl != null) {
                        mListenerImpl.onRequestBurnMessageCapabilityResult(contact, result);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    public void onParticipantJoined(final String chatId, final Participant participant) {
        Logger.d(TAG, "onParticipantJoined: #" + chatId + " ,#" + participant);
        if (mListenerImpl == null)
            return;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    String contactName = RCSDataBaseUtils.getContactDisplayName(
                            mContext, participant.getContact());
                    String body = mContext.getString(R.string.group_participant_join, contactName);
                    addGroupSysMessage(chatId, null, body, false, false);
                    if (mListenerImpl != null) {
                        mListenerImpl.onParticipantJoined(chatId, participant);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    public void onParticipantLeft(final String chatId, final Participant participant) {
        Logger.d(TAG, "onParticipantLeft: #" + chatId + " ,#" + participant);
        if (mListenerImpl == null)
            return;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    String contactName = RCSDataBaseUtils.getContactDisplayName(
                            mContext, participant.getContact());
                    String body = mContext.getString(R.string.group_participant_left, contactName);
                    addGroupSysMessage(chatId, null, body, false, false);
                    if (mListenerImpl != null) {
                        mListenerImpl.onParticipantLeft(chatId, participant);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    public void onParticipantRemoved(final String chatId, final Participant participant) {
        Logger.d(TAG, "onParticipantRemoved: #" + chatId + " ,#" + participant);
        if (mListenerImpl == null)
            return;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    String contactName = RCSDataBaseUtils.getContactDisplayName(
                            mContext, participant.getContact());
                    String body = mContext.getString(R.string.group_participant_removed,
                            contactName);
                    addGroupSysMessage(chatId, null, body, false, false);
                    if (mListenerImpl != null) {
                        mListenerImpl.onParticipantRemoved(chatId, participant);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    public void onChairmenChanged(final String chatId, final Participant participant,
            final boolean isMe) {
        Logger.d(TAG, "onChairmenChanged: #" + chatId + " ,#" + participant);
        if (mListenerImpl == null)
            return;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    String body = null;
                    if (isMe) {
                        body = mContext.getString(R.string.group_i_become_chairmen);
                    } else {
                        String contactName = RCSDataBaseUtils.getContactDisplayName(
                                mContext, participant.getContact());
                        body = mContext.getString(R.string.group_chairmen_transferred, contactName);
                    }
                    addGroupSysMessage(chatId, null, body, false, false);
                    if (mListenerImpl != null) {
                        mListenerImpl.onChairmenChanged(chatId, participant, isMe);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    public void onSubjectModified(final String chatId, final String subject) {
        Logger.d(TAG, "onSubjectModified: #" + chatId + " ,#" + subject);
        if (mListenerImpl == null)
            return;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    String body = mContext.getString(R.string.group_subject_modified, subject);
                    addGroupSysMessage(chatId, null, body, false, false);
                    if (mListenerImpl != null) {
                        mListenerImpl.onSubjectModified(chatId, subject);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    public void onParticipantNickNameModified(final String chatId, final String contact,
            final String nickName) {
        Logger.d(TAG, "onParticipantNickNameModified: #" + chatId + " ,#" + contact);
        if (mListenerImpl == null)
            return;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    if (mListenerImpl != null) {
                        mListenerImpl.onParticipantNickNameModified(chatId, contact, nickName);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    public void onMeRemoved(final String chatId, final String contact) {
        Logger.d(TAG, "onMeRemoved: #" + chatId + " ,#" + contact);
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    String contactName = RCSDataBaseUtils.getContactDisplayName(
                            mContext, contact);
                    String body = mContext.getString(R.string.group_me_removed, contactName);
                    addGroupSysMessage(chatId, null, body, false, false);
                    updateGroupStatus(chatId, GroupActionList.GROUP_STATUS_INVALID);
                    if (mListenerImpl != null) {
                        mListenerImpl.onMeRemoved(chatId, contact);
                    } else {
                        Intent intent = new Intent(
                                IpMessageConsts.ServiceNotification.
                                BROADCAST_RCS_GROUP_BEEN_KICKED_OUT);
                        intent.setClassName("com.android.mms", "com.mediatek.rcs.EmptyReceiver");
                        intent.putExtra(IpMessageConsts.ServiceNotification.KEY_CHAT_ID, chatId);
                        intent.putExtra(IpMessageConsts.ServiceNotification.KEY_CONTACT, contact);
                        mContext.sendBroadcast(intent);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    public void onAbort(final String chatId) {
        Logger.d(TAG, "onAbort: #" + chatId);
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    String body = mContext.getString(R.string.group_aborted);
                    addGroupSysMessage(chatId, null, body, false, false);
                    updateGroupStatus(chatId, GroupActionList.GROUP_STATUS_INVALID);
                    if (mListenerImpl != null) {
                        mListenerImpl.onAbort(chatId);
                    } else {
                        Intent intent = new Intent(
                                IpMessageConsts.ServiceNotification.BROADCAST_RCS_GROUP_ABORTED);
                        intent.setClassName("com.android.mms", "com.mediatek.rcs.EmptyReceiver");
                        intent.putExtra(IpMessageConsts.ServiceNotification.KEY_CHAT_ID, chatId);
                        mContext.sendBroadcast(intent);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    public void onNewInvite(
            final Participant participant, final String subject, final String chatId) {
        Logger.d(TAG, "onNewInvite:" + chatId + ", from: " + participant.getContact());
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    String escapedNumber = DatabaseUtils.sqlEscapeString(participant.getContact());
                    String selection =
                        MessageColumn.CHAT_ID + " IS NOT NULL " +
                        " AND " + MessageColumn.CHAT_ID + "='" + chatId + "' " +
                        " AND (" + MessageColumn.CONTACT_NUMBER + "=" + escapedNumber +
                            " OR PHONE_NUMBERS_EQUAL(" + MessageColumn.CONTACT_NUMBER +
                            ", " + escapedNumber + ")) " +
                        " AND " + MessageColumn.CLASS + "=" + Class.INVITATION +
                        " AND " + MessageColumn.MESSAGE_STATUS + "=" + InvitationStatus.INVITING;
                    String[] projection = {MessageColumn.ID};
                    Cursor cursor = mContext.getContentResolver().query(
                            MessageColumn.CONTENT_URI, projection, selection, null, null);
                    try {
                        if (cursor == null) {
                            Logger.d(TAG, " cursor is null !!");
                        }
                        if (cursor == null || !cursor.moveToFirst()) {
                            String name = RCSDataBaseUtils.getContactDisplayName(mContext,
                                    participant.getContact());
                            String body = mContext.getString(
                                    R.string.group_invitation_received, name);
                            addGroupSysMessage(chatId, participant.getContact(), body, true, true);
                        } else {
                            return;
                        }
                    } finally {
                        if (cursor != null) {
                            cursor.close();
                        }
                    }
                    if (mListenerImpl != null) {
                        mListenerImpl.onNewInvite(participant, subject, chatId);
                    } else {
                        Intent intent = new Intent(
                                IpMessageConsts.ServiceNotification.BROADCAST_RCS_GROUP_INVITATION);
                        intent.setClassName("com.android.mms", "com.mediatek.rcs.EmptyReceiver");
                        intent.putExtra(IpMessageConsts.ServiceNotification.KEY_CHAT_ID, chatId);
                        intent.putExtra(IpMessageConsts.ServiceNotification.KEY_SUBJECT, subject);
                        intent.putExtra(
                                IpMessageConsts.ServiceNotification.KEY_PARTICIPANT, participant);
                        mContext.sendBroadcast(intent);
                    }

                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    public void onAddParticipantsResult(final String chatId, final boolean result) {
        Logger.d(TAG, "onAddParticipantsResult: #" + chatId + " ,#" + result);
        if (mListenerImpl == null)
            return;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    if (mListenerImpl != null) {
                        mListenerImpl.onAddParticipantsResult(chatId, result);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    public void onRemoveParticipantsResult(final String chatId, final boolean result) {
        Logger.d(TAG, "onRemoveParticipantsResult: #" + chatId + " ,#" + result);
        if (mListenerImpl == null)
            return;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    if (mListenerImpl != null) {
                        mListenerImpl.onRemoveParticipantsResult(chatId, result);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    public void onTransferChairmenResult(final String chatId, final boolean result) {
        Logger.d(TAG, "onTransferChairmenResult: #" + chatId + " ,#" + result);
        if (mListenerImpl == null)
            return;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    if (mListenerImpl != null) {
                        mListenerImpl.onTransferChairmenResult(chatId, result);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    public void onMyNickNameModifiedResult(final String chatId, final boolean result) {
        Logger.d(TAG, "onMyNickNameModifiedResult: #" + chatId + " ,#" + result);
        if (mListenerImpl == null)
            return;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    if (mListenerImpl != null) {
                        mListenerImpl.onMyNickNameModifiedResult(chatId, result);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    public void onSubjectModifiedResult(final String chatId, final boolean result) {
        Logger.d(TAG, "onSubjectModifiedResult: #" + chatId + " ,#" + result);
        if (mListenerImpl == null)
            return;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    if (mListenerImpl != null) {
                        mListenerImpl.onSubjectModifiedResult(chatId, result);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    public void onQuitConversationResult(final String chatId, final boolean result) {
        Logger.d(TAG, "onQuitConversationResult: #" + chatId + " ,#" + result);
        if (mListenerImpl == null)
            return;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    if (result) {
                        String body = mContext.getString(R.string.group_i_quit);
                        addGroupSysMessage(chatId, null, body, false, false);
                        updateGroupStatus(chatId, GroupActionList.GROUP_STATUS_INVALID);
                    }
                    if (mListenerImpl != null) {
                        mListenerImpl.onQuitConversationResult(chatId, result);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    public void onAbortResult(final String chatId, final boolean result) {
        Logger.d(TAG, "onAbortResult: #" + chatId + " ,#" + result);
        if (mListenerImpl == null)
            return;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    if (result) {
                        String body = mContext.getString(R.string.group_aborted);
                        addGroupSysMessage(chatId, null, body, false, false);
                        updateGroupStatus(chatId, GroupActionList.GROUP_STATUS_INVALID);
                    }
                    if (mListenerImpl != null) {
                        mListenerImpl.onAbortResult(chatId, result);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    public void onInitGroupResult(final boolean result, final String chatId) {
        Logger.d(TAG, "onInitGroupResult: #" + chatId + " ,#" + result);
        if (mListenerImpl == null)
            return;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    if (mListenerImpl != null) {
                        mListenerImpl.onInitGroupResult(result, chatId);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    public void onAcceptInvitationResult(final String chatId, final boolean result) {
        Logger.d(TAG, "onAcceptInvitationResult: #" + chatId + " ,#" + result);
        if (mListenerImpl == null)
            return;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    if (result) {
                        updateGroupStatus(chatId, RCSUtils.getRCSSubId());
                    }
                    if (mListenerImpl != null) {
                        mListenerImpl.onAcceptInvitationResult(chatId, result);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    public void onRejectInvitationResult(final String chatId, final boolean result) {
        Logger.d(TAG, "onRejectInvitationResult: #" + chatId + " ,#" + result);
        if (mListenerImpl == null)
            return;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    if (mListenerImpl != null) {
                        mListenerImpl.onRejectInvitationResult(chatId, result);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    public void onUpdateFileTransferStatus(final long ipMsgId, final int stat, final int status) {
        Logger.d(TAG, "onUpdateFileTransferStatus: #" + ipMsgId + " ,#" + stat);
        if (mListenerImpl == null)
            return;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    if (mListenerImpl != null) {
                        mListenerImpl.onUpdateFileTransferStatus(ipMsgId, stat, status);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    public void setFilePath(final long ipMsgId, final String filePath) {
        Logger.d(TAG, "setFilePath: #" + ipMsgId + " ,#" + filePath);
        if (mListenerImpl == null)
            return;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    if (mListenerImpl != null) {
                        mListenerImpl.setFilePath(ipMsgId, filePath);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    public void onSyncFullConferenceNotifyDone(final String chatId) {
        Logger.d(TAG, "onSyncFullConferenceNotifyDone: #" + chatId);
        if (mListenerImpl == null)
            return;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    if (mListenerImpl != null) {
                        mListenerImpl.onSyncFullConferenceNotifyDone(chatId);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    @Override
    public void onSpamReportResult(final boolean arg0) {
        Logger.d(TAG, "onSpamReportResult: #" + arg0);
        if (mListenerImpl == null)
            return;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    if (mListenerImpl != null) {
                        mListenerImpl.onSpamReportResult(arg0);
                    }
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });
    }

    private void addGroupSysMessage(String chatId, String contact, String body,
            boolean invite, boolean createThread) {
        RCSDataBaseUtils.addGroupSysMessage(mContext, chatId, contact, body, invite, createThread);
    }

    private void updateGroupStatus(String chatId, long status) {
        ContentValues values = new ContentValues();
        values.put(Threads.STATUS, status);
        String where = ThreadsColumn.RECIPIENTS + "='" + chatId + "' AND " +
                    ThreadsColumn.FLAG + "=" + ThreadFlag.MTM;
        mContext.getContentResolver().update(ThreadsColumn.CONTENT_URI_STATUS, values, where, null);
    }
}
