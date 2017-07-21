package com.mediatek.rcs.common.provider;

import java.util.HashSet;
import java.util.Set;

import org.gsma.joyn.chat.ChatLog;

import android.content.Context;
import android.text.TextUtils;

import com.mediatek.rcs.common.RcsLog.Direction;
import com.mediatek.rcs.common.RcsLog.InvitationStatus;
import com.mediatek.rcs.common.RcsLog.MessageStatus;
import com.mediatek.rcs.common.RcsLog.MessageType;
import com.mediatek.rcs.common.RcsLog.Class;
import com.mediatek.rcs.common.RcsLog.ThreadFlag;
import com.mediatek.rcs.common.utils.RCSUtils;

/**
 * Group system message struct for insert into database.
 */
public class SystemMessageStruct extends MessageStruct {

    private SystemMessageStruct(Context context) {
        mContext = context;
        mDirection = Direction.INCOMING;
        mTimestamp = System.currentTimeMillis();
        mSeen = 1;
        mMessageStatus = MessageStatus.READ;
        mSubId = RCSUtils.getRCSSubId();
        mIsBlocked = 0;
        mIpmsgId = Integer.MAX_VALUE;
        mLocked = 0;
        mClass = Class.SYSTEM;
        mType = MessageType.IM;
        mFlag = ThreadFlag.MTM;
        mDateSent = 0;
        mConversationId = 0;
        mBody = null;
        mContact = null;
        mChatId = null;
        mMessageId = null;
        mMimeType = null;
        mFilePath = null;
        mContactSet = new HashSet<String>();
    }

    /**
     * constructor for group System message
     * @param context
     * @param contact
     * @param content
     */
    public SystemMessageStruct(Context context, String chatId, String content) {
        this(context);
        if (TextUtils.isEmpty(chatId)) {
            throw new IllegalArgumentException("Empty chatId!!");
        }
        mBody = content;
        mChatId = chatId;
    }

    /**
     * contructor for group System message
     * @param context
     * @param chatId
     * @param content
     */
    public SystemMessageStruct(Context context, String chatId, String contact, String content,
            int msgType) {
        this(context, chatId, content);
        mContact = contact;
        mClass = msgType;
        if (!TextUtils.isEmpty(contact)) {
            mContactSet.add(contact);
        }
        if (msgType == Class.INVITATION) {
            mMessageStatus = InvitationStatus.INVITING;
        }
    }

    @Override
    long getOrCreateConversationId() {
        return RCSDataBaseUtils.getOrCreateConversationId(mContext, mChatId,
                new HashSet<String>(), RCSUtils.getRCSSubId());
    }
}