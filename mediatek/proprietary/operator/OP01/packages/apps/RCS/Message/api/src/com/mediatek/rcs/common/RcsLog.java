package com.mediatek.rcs.common;

import android.net.Uri;

/**
 * Class for definition of database column names and values.
 */
public class RcsLog {

    /**
     * Column names of table rcs_message.
     */
    public static class MessageColumn {
        /**
         * The {@code content://} style URI for table rcs_message.
         */
        public static final Uri CONTENT_URI = Uri.parse("content://rcs");

        /**
         * The id of message.
         * <p>Type: INTEGER</p>
         */
        public static final String ID               = "_id";
        /**
         * The date the message was sent.
         * <p>Type: INTEGER</p>
         */
        public static final String DATE_SENT        = "date_sent";
        /**
         * Has the message been seen by the user? The "seen" flag determines
         * whether we need to show a notification.
         * <p>Type: INTEGER(boolean)</p>
         */
        public static final String SEEN             = "seen";
        /**
         * The lock status of the message.
         * <p>Type: INTEGER(boolean)</p>
         */
        public static final String LOCKED           = "locked";
        /**
         * The sub id of the message.
         * <p>Type: INTEGER</p>
         */
        public static final String SUB_ID           = "sub_id";
        /**
         * The ip messags id of the message.
         * <p>Type: INTEGER</p>
         */
        public static final String IPMSG_ID         = "ipmsg_id";
        /**
         * The class type of the message {@link Class}.
         * <p>Type: INTEGER</p>
         */
        public static final String CLASS            = "class";
        /**
         * The file pathof  the message, {@link MessageType} FT only.
         * <p>Type: TEXT</p>
         */
        public static final String FILE_PATH        = "file_path";
        /**
         * The unique String message id of  the message.
         * <p>Type: TEXT</p>
         */
        public static final String MESSAGE_ID       = "CHATMESSAGE_MESSAGE_ID";
        /**
         * The chat id of the message {@link ThreadFlag} MTM only.
         * <p>Type: TEXT</p>
         */
        public static final String CHAT_ID          = "CHATMESSAGE_CHAT_ID";
        /**
         * The contact number of  the message, if multi contact, ";" seperated.
         * <p>Type: TEXT</p>
         */
        public static final String CONTACT_NUMBER   = "CHATMESSAGE_CONTACT_NUMBER";
        /**
         * The content of the message.
         * <p>Type: TEXT</p>
         */
        public static final String BODY             = "CHATMESSAGE_BODY";
        /**
         * The date the message was send/received.
         * <p>Type: INTEGER</p>
         */
        public static final String TIMESTAMP        = "CHATMESSAGE_TIMESTAMP";
        /**
         * The status of the message {@link MessageStatus}.
         * <p>Type: INTEGER</p>
         */
        public static final String MESSAGE_STATUS   = "CHATMESSAGE_MESSAGE_STATUS";
        /**
         * The type of the message {@link MessageType}.
         * <p>Type: INTEGER</p>
         */
        public static final String TYPE             = "CHATMESSAGE_TYPE";
        /**
         * The direction of the message {@link Direction}.
         * <p>Type: INTEGER</p>
         */
        public static final String DIRECTION        = "CHATMESSAGE_DIRECTION";
        /**
         * The flag of the message {@link ThreadFlag}.
         * <p>Type: INTEGER</p>
         */
        public static final String FLAG             = "CHATMESSAGE_FLAG";
        /**
         * The block status of the message.
         * <p>Type: INTEGER</p>
         */
        public static final String ISBLOCKED        = "CHATMESSAGE_ISBLOCKED";
        /**
         * The conversation id of the message.
         * <p>Type: INTEGER</p>
         */
        public static final String CONVERSATION     = "CHATMESSAGE_CONVERSATION";
        /**
         * The mime type of the message, {@link MessageType} FT only.
         * <p>Type: TEXT</p>
         */
        public static final String MIME_TYPE        = "CHATMESSAGE_MIME_TYPE";
    }

    /**
     * Column names of view rcs_threads.
     */
    public static class ThreadsColumn {
        /**
         * The {@code content://} style URI for view rcs_threads.
         */
        public static final Uri CONTENT_URI = Uri.parse("content://mms-sms-rcs/conversations");
        /**
         * The {@code content://} style URI for update group status in rcs_threads.
         * {@hide}
         */
        public static final Uri CONTENT_URI_STATUS =
                Uri.parse("content://mms-sms-rcs/conversations/status");

        /**
         * The id of conversation.
         * <p>Type: INTEGER</p>
         */
        public static final String ID               = "_id";
        /**
         * The snippet of last message in conversation.
         * <p>Type: TEXT</p>
         */
        public static final String SNIPPET          = "snippet";
        /**
         * The snippet charset of last message in conversation.
         * <p>Type: INTEGER</p>
         */
        public static final String SNIPPET_CS       = "snippet_cs";
        /**
         * The type last message in conversation {@link MessageType}.
         * <p>Type: INTEGER</p>
         */
        public static final String TYPE             = "type";
        /**
         * The date of last message in conversation.
         * <p>Type: INTEGER</p>
         */
        public static final String DATE             = "date";
        /**
         * The read message number of conversation.
         * <p>Type: INTEGER</p>
         */
        public static final String READCOUNT        = "readcount";
        /**
         * The total message number of conversation.
         * <p>Type: INTEGER</p>
         */
        public static final String MESSAGE_COUNT    = "message_count";
        /**
         * If the conversation contains send failed message.
         * <p>Type: INTEGER</p>
         */
        public static final String ERROR            = "error";
        /**
         * If the conversation has unread message.
         * <p>Type: INTEGER</p>
         */
        public static final String READ             = "read";
        /**
         * If the conversation has attachment.
         * <p>Type: INTEGER</p>
         */
        public static final String HAS_ATTACHMENT   = "has_attachment";
        /**
         * The status of conversation {@link ThreadFlag} MTM only.
         * <p>Type: INTEGER</p>
         */
        public static final String STATUS           = "status";
        /**
         * A string encoding of the recipient IDs of the recipients of
         * the message, in numerical order and separated by spaces.
         * <p>Type: TEXT</p>
         */
        public static final String RECIPIENT_IDS    = "recipient_ids";
        /**
         * If the conversation is archived.
         * <p>Type: INTEGER(boolean)</p>
         */
        public static final String ARCHIVED         = "archived";
        /**
         * The snippet of last message in conversation.
         * <p>Type: INTEGER</p>
         */
        public static final String CLASS            = "class";
        /**
         * The class of last message in conversation {@link Class}.
         * <p>Type: INTEGER</p>
         */
        public static final String RECIPIENTS       = "CHATMESSAGE_RECIPIENTS";
        /**
         * The flag of conversation {@link ThreadFlag}.
         * <p>Type: INTEGER</p>
         */
        public static final String FLAG             = "CHATMESSAGE_FLAG";
        /**
         * The message type of last message in conversation {@link MessageType}.
         * <p>Type: INTEGER</p>
         */
        public static final String MESSAGE_TYPE     = "CHATMESSAGE_TYPE";
        /**
         * The mime type of last message in conversation {@link MessageType} FT only.
         * <p>Type: TEXT</p>
         */
        public static final String MIME_TYPE        = "CHATMESSAGE_MIME_TYPE";
    }

    /**
     * Class values.
     */
    public static final class Class {
        /**
         * Normal message, include IM and FT.
         */
        public static final int NORMAL     = 0;
        /**
         * Burn message.
         */
        public static final int BURN       = 1;
        /**
         * Emoticon shop message.
         */
        public static final int EMOTICON   = 2;
        /**
         * Cloud message.
         */
        public static final int CLOUD      = 3;
        /**
         * Group chat system message, except invitation.
         */
        public static final int SYSTEM     = 11;
        /**
         * Group chat invitation message.
         */
        public static final int INVITATION = 12;
    }

    /**
     * Message status.
     */
    public static final class MessageStatus {
        /**
         * Unread.
         */
        public static final int UNREAD           = 0;
        /**
         * Read.
         */
        public static final int READ             = 2;
        /**
         * Sending.
         */
        public static final int SENDING          = 3;
        /**
         * Sent.
         */
        public static final int SENT             = 4;
        /**
         * Failed.
         */
        public static final int FAILED           = 5;
        /**
         * To send.
         */
        public static final int TO_SEND          = 6;
        /**
         * Delivered.
         */
        public static final int DELIVERED        = 7;
    }

    /**
     * Message status when {@link Class} is INVITATION.
     */
    public static final class InvitationStatus {
        /**
         * Inviting state.
         */
        public static final int INVITING            = 8;
        /**
         * User accept this invitation.
         */
        public static final int ACCEPTED            = 9;
        /**
         * User reject this invitation.
         */
        public static final int REJECTED            = 10;
        /**
         * User accept another contact's invitation.
         */
        public static final int ACCEPTED_BY_OTHER   = 11;
    }

    /**
     * Message type.
     */
    public static final class MessageType {
        /**
         * SMS or MMS.
         */
        public static final int SMSMMS     = 0;
        /**
         * Text RCS message.
         */
        public static final int IM         = 1;
        /**
         * File transfer RCS message.
         */
        public static final int FT         = 2;
        /**
         * Public account RCS message.
         */
        public static final int XML        = 3;
    }

    /**
     * Thread flag.
     */
    public static final class ThreadFlag {
        /**
         * One to one message/conversation.
         */
        public static final int OTO        = 1;
        /**
         * One to multi message/conversation.
         */
        public static final int OTM        = 2;
        /**
         * Group message/conversation.
         */
        public static final int MTM        = 3;
        /**
         * Public account message/conversation.
         */
        public static final int OFFICIAL   = 4;
    }

    /**
     * Direction.
     */
    public static final class Direction {
        /**
         * Incoming message.
         */
        public static final int INCOMING      = 0;
        /**
         * Outgoing message.
         */
        public static final int OUTGOING      = 1;
    }
}
