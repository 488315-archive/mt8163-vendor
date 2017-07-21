package com.mediatek.rcs.common;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArrayList;

import org.gsma.joyn.chat.ChatLog;

import android.content.Context;
import android.database.Cursor;
import android.os.RemoteException;
import android.telephony.PhoneNumberUtils;
import android.text.TextUtils;

import com.mediatek.rcs.common.RCSServiceManager;
import com.mediatek.rcs.common.provider.GroupMemberData;
import com.mediatek.rcs.common.service.IRCSChatService;
import com.mediatek.rcs.common.service.Participant;
import com.mediatek.rcs.common.utils.Logger;
import com.mediatek.rcs.common.utils.RCSUtils;

/**
 * GroupManager is a manager to manage RCS groups. Can be called to get one {@link RCSGroup} with
 * specified chat id by {@link #getRCSGroup(String)}. Also can be called to Accept/Reject group
 * invitation by {@link #acceptGroupInvitation(String)}/ {@link #rejectGroupInvitation(String)}.
 * GroupManager is a single instance. To use this manager, call {@link RCSServiceManager#
 * getGroupManager()} to get the instance.
 */
public class GroupManager {

    private static final String TAG = "GroupManager";
    private static GroupManager sInstance;
    private Context mContext;

    private Map<String, RCSGroup> mGroupList = new ConcurrentHashMap<String, RCSGroup>();
    private List<IInitGroupListener> mListener = new CopyOnWriteArrayList<IInitGroupListener>();
    private Map<String, GroupInfo> mInvitingGroup = new ConcurrentHashMap<String, GroupInfo>();

    /**
     * A callback interface to listen the result of creating group and accepting/rejecting group's
     * invitation. To use this interface, need to
     * {@link GroupManager#addGroupListener(IInitGroupListener)}.If don't use the listener any more,
     * must to call {@link GroupManager#removeGroupListener(IInitGroupListener)} to remove the
     * listener.
     *
     */
    public interface IInitGroupListener {

        void onInitGroupResult(int result, long threadId, String chatId);
        void onAcceptGroupInvitationResult(int result, long threadId, String chatId);
        void onRejectGroupInvitationResult(int result, long threadId, String chatId);
    }

    private GroupManager(Context context) {
        Logger.d(TAG, "GroupManager constructor");
        mContext = context;
    }

    /**
     * Create {@link GroupManager}'s instance.
     * @param context {@link Context}
     * @return {@link GroupManager}
     */
    protected static GroupManager createInstance(Context context) {
        Logger.d(TAG, "createInstance");
        if (sInstance == null) {
            sInstance = new GroupManager(context);
        }
        return sInstance;
    }

    /**
     * add {@link IInitGroupListener} to {@link GroupManager} for listening the result of create
     * group by {@link #initGroupChat(HashSet, String)}, listening the result of accept/reject group
     * invitation by {@link #acceptGroupInvitation(String)}/{@link #rejectGroupInvitation(String)}.
     * If don't use the listener any more, Must to call
     * {@link #removeGroupListener(IInitGroupListener)} to remove it.
     * @param listener please see {@link IInitGroupListener}
     */
    public void addGroupListener(IInitGroupListener listener) {
        mListener.add(listener);
    }

    /**
     * remove {@link IInitGroupListener} from {@link GroupManager}.
     * @param listener please see {@link IInitGroupListener}
     */
    public void removeGroupListener(IInitGroupListener listener) {
        mListener.remove(listener);
    }

    /**
     * Get all {@link IInitGroupListener} from {@link GroupManager}.
     * @return
     */
    public List<IInitGroupListener> getAllListeners() {
        return mListener;
    }

    /**
     *Record inviting Group.
     * @param chatId
     * @param subject
     * @param participants
     */
    private void recordInvitingGroup(String chatId, String subject, List<String> participants) {
        if (TextUtils.isEmpty(chatId)) {
            return;
        }
        mInvitingGroup.put(chatId, new GroupInfo(chatId, subject, participants));
    }

    /**
     * Get inviting group's {@link GroupInfo} by specified group chat id.
     * @param chatId group chat id
     * @return {@link GroupInfo}
     */
    public GroupInfo getInvitingGroup(String chatId) {
        if (!TextUtils.isEmpty(chatId)) {
            return mInvitingGroup.get(chatId);
        }
        return null;
    }

    /**
     * Remove inviting group.
     * @param chatId group chat id
     */
    public void removeInvitingGroup(String chatId) {
        if (!TextUtils.isEmpty(chatId)) {
            mInvitingGroup.remove(chatId);
        }
    }

    /**
     *  Class GroupInfo.
     * @author
     *
     */
    public class GroupInfo {
        private List<String> mParticipants;
        private String mSubject;
        private String mChatId;

        public GroupInfo(String chatId, String subject, List<String> participants) {
            mParticipants = participants;
            mSubject = subject;
            mChatId = chatId;
        }

        public String getSubject() {
            return mSubject;
        }

        public List<String> getParticipants() {
            return mParticipants;
        }
    }

    /**
     * Get one {@link RCSGroup} with specified id. It will query database, so should not call this
     * function in main thread. From this function to get {@link RCSGroup}, should call
     * {@link #releaseRCSGroup(String)} to release the {@link RCSGroup} if don't use it
     * any more.
     * @param chatId group chat id
     * @return {@link RCSGroup}
     */
    public RCSGroup getRCSGroup(String chatId) {
        Logger.d(TAG, "getRCSGroup, chatId=" + chatId);
        RCSGroup groupInfo = mGroupList.get(chatId);
        if (groupInfo != null) {
            groupInfo.addReferenceCount();
            return groupInfo;
        }
        String chatSelection = ChatLog.GroupChat.CHAT_ID + "='" + chatId + "'";
        Cursor chatCursor = null;
        Cursor participantCursor = null;
        try {
            chatCursor = mContext.getContentResolver().query(
                    RCSUtils.RCS_URI_GROUP_CHAT, RCSUtils.PROJECTION_GROUP_INFO, chatSelection,
                        null, null);
            if (chatCursor.moveToFirst()) {
                List<Participant> participants = new ArrayList<Participant>();
                String memberSelection = GroupMemberData.COLUMN_CHAT_ID + "='" + chatId + "'";
//                memberSelection  = memberSelection + " AND " + GroupMemberData.COLUMN_STATE +
//                  "<>" + GroupMemberData.STATE.STATE_PENDING;
                participantCursor = mContext.getContentResolver().query(
                        RCSUtils.RCS_URI_GROUP_MEMBER, RCSUtils.PROJECTION_GROUP_MEMBER,
                        memberSelection, null, null);
                String myNickName = null;
                String myNumber = RCSServiceManager.getInstance().getMyNumber();
                while (participantCursor.moveToNext()) {
                    String number = participantCursor.getString(participantCursor.getColumnIndex(
                            GroupMemberData.COLUMN_CONTACT_NUMBER));
                    String name = participantCursor.getString(participantCursor.getColumnIndex(
                            GroupMemberData.COLUMN_CONTACT_NAME));
                    Participant participant = new Participant(number, name);
                    participant.setState(participantCursor.getInt(participantCursor.getColumnIndex(
                            GroupMemberData.COLUMN_STATE)));
                    participants.add(participant);
                    if (PhoneNumberUtils.compare(number, myNumber)) {
                        myNickName = name;
                    }
                }
                groupInfo = new RCSGroup(mContext
                    , chatCursor.getString(chatCursor.getColumnIndex(ChatLog.GroupChat.CHAT_ID))
                    , chatCursor.getString(chatCursor.getColumnIndex(ChatLog.GroupChat.SUBJECT))
                    , chatCursor.getString(chatCursor.getColumnIndex(ChatLog.GroupChat.CHAIRMAN))
                    , chatCursor.getString(chatCursor.getColumnIndex(ChatLog.GroupChat.NICKNAME))
                    , participants
                    , myNickName
                    );
            }
        } finally {
            if (chatCursor != null) {
                chatCursor.close();
            }
            if (participantCursor != null) {
                participantCursor.close();
            }
        }
        mGroupList.put(chatId, groupInfo);
        return groupInfo;
    }

    /**
     * Release the {@link RCSGroup} to free memory if don't use it any more. Must call this function
     * to free memory if don't use the {@link RCSGroup} that be got by {@link #getRCSGroup(String)}.
     * @param chatId group chat id
     */
    public void releaseRCSGroup(String chatId) {
        Logger.d(TAG, "releaseRCSGroup, chatId=" + chatId);
        RCSGroup groupInfo = mGroupList.get(chatId);
        if (groupInfo != null && groupInfo.releaseGroup()) {
            mGroupList.remove(chatId);
        }
    }

    /**
     * Create group chat with participants's nubmer's set and group chat's subject. Creating group
     * chat is a async flow. To get the result of creating group chat, need to
     * {@link #addGroupListener(IInitGroupListener)} to listen the result by
     * {@link IInitGroupListener#onInitGroupResult(int, long, String)} with the specified chat id
     * which is return by this function.
     * @param participants the set of numbers want to be invited
     * @param subject group name
     * @return return group chat id if starting to create group chat. Will return null if any
     * exception happens, such as RCS is not registered.
     */
    public String initGroupChat(HashSet<String> participants, String subject) {
        Logger.d(TAG, "init GroupChat, subject = " + subject);
        List<String> contacts = new ArrayList<String>(participants);
        IRCSChatService service = RCSServiceManager.getInstance().getChatService();
        String chatId = null;
        try {
            chatId = service.initGroupChat(subject, contacts);
            if (!TextUtils.isEmpty(chatId)) {
                recordInvitingGroup(chatId, subject, contacts);
            }
        } catch (RemoteException e) {
            e.printStackTrace();
        }
        return chatId;
    }

    /**
     * Accept the group chat's invitation with specified chat id. Accept group chat is async flow.
     * Can use {@link IInitGroupListener#onAcceptGroupInvitationResult(int, long, String)} to listen
     * the result after {@link #addGroupListener(IInitGroupListener)}.
     * @param chatId the inviting group's chat id
     * @return Return true if starting process the action.
     */
    public boolean acceptGroupInvitation(String chatId) {
        Logger.d(TAG, "acceptGroupInvitation, chatId=" + chatId);
        IRCSChatService service = RCSServiceManager.getInstance().getChatService();
        try {
            service.acceptGroupChat(chatId);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
        return true;
    }

    /**
     * Reject the group chat's invitation with specified chat id. Reject group chat is async flow.
     * Can use {@link IInitGroupListener#onRejectGroupInvitationResult(int, long, String)} to listen
     * the result after {@link #addGroupListener(IInitGroupListener)}.
     * @param chatId the inviting group's chat id
     * @return Return true if starting process the action.
     */
    public boolean rejectGroupInvitation(String chatId) {
        Logger.d(TAG, "rejectGroupInvitation, chatId=" + chatId);
        IRCSChatService service = RCSServiceManager.getInstance().getChatService();
        try {
            service.rejectGroupChat(chatId);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
        return true;
    }
}
