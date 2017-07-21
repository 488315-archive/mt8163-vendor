package com.mediatek.rcs.common.provider;

import android.net.Uri;

/**
 * Class for definition of database column names and values.
 */
public class GroupMemberData {

    /**
     * The {@code content://} style URI for table groupmember.
     */
    public static final Uri CONTENT_URI = Uri.parse("content://rcs-groupmember");

    /**
     * The authority of {@link CONTENT_URI}.
     */
    public static final String AUTHORITY = "rcs-groupmember";

    /**
     * The unique id of group chat member.
     * <p>Type: INTEGER</p>
     */
    public static final String COLUMN_ID                   = "_id";
    /**
     * The chat id of this member.
     * <p>Type: TEXT</p>
     */
    public static final String COLUMN_CHAT_ID              = "GROUPCHATSERVICE_CHAT_ID";
    /**
     * The phone number of this member.
     * <p>Type: TEXT</p>
     */
    public static final String COLUMN_CONTACT_NUMBER       = "GROUPCHATSERVICE_PHONE_NUMBER";
    /**
     * The member name.
     * <p>Type: TEXT</p>
     */
    public static final String COLUMN_CONTACT_NAME         = "GROUPCHATSERVICE_MEMBER_NAME";
    /**
     * The type of this member.
     * <p>Type: INTEGER</p>
     */
    public static final String COLUMN_TYPE                 = "type";
    /**
     * The portrait of this member.
     * <p>Type: INTEGER</p>
     */
    public static final String COLUMN_PORTRAIT             = "GROUPCHATSERVICE_PORTRAIT";
    /**
     * The state of this member.
     * <p>Type: INTEGER</p>
     */
    public static final String COLUMN_STATE                = "state";
    /**
     * The portrait name of this member.
     * <p>Type: INTEGER</p>
     */
    public static final String COLUMN_PORTRAIT_NAME        = "GROUPCHATSERVICE_PORTRAIT_NAME";

    /**
     * Group member state of {@link COLUMN_STATE}}
     */
    public static final class STATE {
        /**
         * The member has been invited, but not accept the invitation.
         */
        public static final int STATE_PENDING         = 0;

        /**
         * The member is in group.
         */
        public static final int STATE_CONNECTED       = 1;

        /**
         * The member is not in this group.
         */
        public static final int STATE_DISCONNECTED    = 2;
    }
}
