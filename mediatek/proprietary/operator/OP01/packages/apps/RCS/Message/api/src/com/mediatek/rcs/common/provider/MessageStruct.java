package com.mediatek.rcs.common.provider;

import java.util.Set;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.media.MediaFile;
import android.net.Uri;
import android.provider.Telephony.Threads;
import android.text.TextUtils;
import android.util.Log;

import com.mediatek.rcs.common.RcsLog;
import com.mediatek.rcs.common.RcsLog.MessageColumn;
import com.mediatek.rcs.common.RcsLog.Class;
import com.mediatek.rcs.common.RcsLog.ThreadFlag;
import com.mediatek.rcs.common.RcsLog.ThreadsColumn;
import com.mediatek.rcs.common.utils.RCSUtils;

/**
 * Message struct to store message info, and provide function to insert into database.
 * Just a abstract class use {@link ReceiveMessageStruct} and {@link SendMessageStruct} instead.
 */
public abstract class MessageStruct {

    private static final String TAG = "MessageStruct";
    public static final Uri URI_RCS_MESSAGE = RcsLog.MessageColumn.CONTENT_URI;
    public static final Uri URI_RCS_THREADS = RcsLog.ThreadsColumn.CONTENT_URI;
    public Context mContext;

    public int mClass = Class.NORMAL;
    public int mDirection;
    public int mMessageStatus;
    public int mSeen = 1;
    public int mFlag;
    public int mSubId;
    public int mIsBlocked = 0;
    public int mType;
    public int mLocked = 0;
    public long mIpmsgId = Integer.MAX_VALUE;
    public long mTimestamp;
    public long mDateSent;
    public long mConversationId = 0;
    public String mContact = null;
    public String mChatId = null;
    public String mBody = null;
    public String mMessageId = null;
    public String mMimeType = null;
    public Set<String> mContactSet = null;
    public String mFilePath = null;

    public long saveMessage() {
        mConversationId = getOrCreateConversationId();
        if (mConversationId <= 0) {
            return 0;
        }
        ContentValues values = new ContentValues();
        values.put(MessageColumn.CLASS, mClass);
        values.put(MessageColumn.DIRECTION, mDirection);
        values.put(MessageColumn.TIMESTAMP, mTimestamp);
        values.put(MessageColumn.MESSAGE_STATUS, mMessageStatus);
        values.put(MessageColumn.FLAG, mFlag);
        values.put(MessageColumn.SUB_ID, mSubId);
        values.put(MessageColumn.CONVERSATION, mConversationId);
        values.put(MessageColumn.IPMSG_ID, mIpmsgId);
        if (mDateSent > 0) {
            values.put(MessageColumn.DATE_SENT, mDateSent);
        }
        values.put(MessageColumn.ISBLOCKED, mIsBlocked);
        values.put(MessageColumn.LOCKED, mLocked);
        values.put(MessageColumn.TYPE, mType);
        values.put(MessageColumn.SEEN, mSeen);
        if (!TextUtils.isEmpty(mContact)) {
            values.put(MessageColumn.CONTACT_NUMBER, mContact);
        }
        if (!TextUtils.isEmpty(mChatId)) {
            values.put(MessageColumn.CHAT_ID, mChatId);
        }
        if (!TextUtils.isEmpty(mBody)) {
            values.put(MessageColumn.BODY, mBody);
        }
        if (!TextUtils.isEmpty(mMessageId)) {
            values.put(MessageColumn.MESSAGE_ID, mMessageId);
        }
        if (!TextUtils.isEmpty(mMimeType)) {
            values.put(MessageColumn.MIME_TYPE, mMimeType);
        }
        if (!TextUtils.isEmpty(mFilePath)) {
            values.put(MessageColumn.FILE_PATH, mFilePath);
        }
        return insert(values);
    }

    String getMimeType (String filePath) {
        String mimeType = null;
        mimeType = MediaFile.getMimeTypeForFile(extractFileNameFromPath(filePath));
        Log.d(TAG, "getMimeType, mimeType = " + mimeType);
        if (mimeType == null) {
            // for special mimetype
            if (extractFileNameFromPath(filePath).toLowerCase().endsWith(".xml")) {
                Log.d(TAG, "return application/vnd.gsma.rcspushlocation+xml");
                return "application/vnd.gsma.rcspushlocation+xml";
            }
        }
        return mimeType;
    }

    long insert(ContentValues values) {
        Uri uri = mContext.getContentResolver().insert(URI_RCS_MESSAGE, values);
        return Long.valueOf(uri.getLastPathSegment());
    }

    String extractFileNameFromPath(String filePath) {
        if (!TextUtils.isEmpty(filePath)) {
            int lastDashIndex = filePath.lastIndexOf("/");
            if (-1 != lastDashIndex && lastDashIndex < filePath.length() - 1) {
                String fileName = filePath.substring(lastDashIndex + 1);
                return fileName;
            } else {
                return filePath;
            }
        } else {
            return null;
        }
    }

    long getOrCreateConversationId() {
        return RCSDataBaseUtils.getOrCreateConversationId(mContext, mChatId,
                mContactSet, RCSUtils.getRCSSubId());
    }
}