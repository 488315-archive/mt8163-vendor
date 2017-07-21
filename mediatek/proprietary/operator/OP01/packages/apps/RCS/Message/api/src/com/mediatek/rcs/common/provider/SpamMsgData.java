package com.mediatek.rcs.common.provider;

import android.net.Uri;

/**
 * Class for definition of database column names and values.
 */
public class SpamMsgData {

    /**
     * The {@code content://} style URI for table spam.
     */
    public static final Uri CONTENT_URI = Uri.parse("content://rcs-spam");

    /**
     * The authority of {@link CONTENT_URI}.
     */
    public static final String AUTHORITY = "rcs-spam";

    /**
     * The unique id of spam message.
     * <p>Type: INTEGER</p>
     */
    public static final String COLUMN_ID            = "_id";
    /**
     * Message content.
     * <p>Type: TEXT</p>
     */
    public static final String COLUMN_BODY          = "body";
    /**
     * Timestamp when message received.
     * <p>Type: INTEGER</p>
     */
    public static final String COLUMN_DATE          = "date";
    /**
     * Remote number of message.
     * <p>Type: TEXT</p>
     */
    public static final String COLUMN_ADDRESS       = "address";
    /**
     * Message type {@link Type}.
     * <p>Type: INTEGER</p>
     */
    public static final String COLUMN_TYPE          = "type";
    /**
     * Message sub id.
     * <p>Type: INTEGER</p>
     */
    public static final String COLUMN_SUB_ID        = "sub_id";
    /**
     * Message index id in RcsStack database.
     * <p>Type: INTEGER</p>
     */
    public static final String COLUMN_IPMSG_ID      = "ipmsg_id";
    /**
     * Message unique String id.
     * <p>Type: TEXT</p>
     */
    public static final String COLUMN_MESSAGE_ID    = "msg_id";

    /**
     * Message type of message. {@link COLUMN_TYPE}
     */
    public static class Type {
        /**
         * SMS.
         */
        public static final int TYPE_SMS            = 0;

        /**
         * MMS Push.
         */
        public static final int TYPE_MMS_PUSH       = 1;

        /**
         * RCS Text message.
         */
        public static final int TYPE_IP_TEXT_MSG    = 2;

        /**
         * RCS File transfer message.
         */
        public static final int TYPE_IP_FT_MSG      = 3;

        /**
         * RCS burn after read text message.
         */
        public static final int TYPE_IP_BURN_TEXT_MSG = 4;

        /**
         * RCS burn after read file transfer message.
         */
        public static final int TYPE_IP_BURN_FT_MSG   = 5;

        /**
         * RCS emoticon message.
         */
        public static final int TYPE_EMOTICON_MSG     = 6;

        /**
         * RCS cloud message.
         */
        public static final int TYPE_CLOUD_MSG        = 7;
    }
}
