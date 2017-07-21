package com.mediatek.rcs.common.provider;

import android.net.Uri;
import com.cmcc.ccs.chat.ChatMessage;

/**
 * Contains favorite columu and favorite uri.
 */
public class FavoriteMsgData {

    /**
     * favorite uri.
     */
    public static final Uri CONTENT_URI = Uri.parse("content://com.cmcc.ccs.favor_message");

    /**
     * favorite AUTHORITY.
     */
    public static final String AUTHORITY = "com.cmcc.ccs.favor_message";

    /**
     * a column for unique identifier.
     * <P>Type: INTEGER</P>
     */
    public static final String COLUMN_ID            = "_id";

    /**
     * a column for favorite date.
     * <P>Type: INTEGER</P>
     */
    public static final String COLUMN_DATE          = "date";

    /**
     * a column for group chat unique id.
     * <P>Type: TEXT</P>
     */
    public static final String COLUMN_CHATID        = "CHATMESSAGE_CHAT_ID";

    /**
     * a column for favorite original message id.
     * <P>Type: TEXT</P>
     */
    public static final String COLUMN_DA_ID         = ChatMessage.MESSAGE_ID;

    /**
     * a column for contact number.
     * <P>Type: TEXT</P>
     */
    public static final String COLUMN_DA_CONTACT    = ChatMessage.CONTACT_NUMBER;

    /**
     * a column for send or receive message content.
     * <P>Type: TEXT</P>
     */
    public static final String COLUMN_DA_BODY       = ChatMessage.BODY;

    /**
     * a column for send or receive message time.
     * <P>Type: LONG</P>
     */
    public static final String COLUMN_DA_TIMESTAMP  = ChatMessage.TIMESTAMP;

    /**
     * a column for file MIME TYPE.
     * <P>Type: TEXT</P>
     */
    public static final String COLUMN_DA_MIME_TYPE  = ChatMessage.MIME_TYPE;

    /**
     * a column for send or receive message status.
     * <P>Type: INTEGER</P>
     */
    public static final String COLUMN_DA_MESSAGE_STATUS  = ChatMessage.MESSAGE_STATUS;

    /**
     * a column for incoming or outgoing message direction.
     * <P>Type: INTEGER</P>
     */
    public static final String COLUMN_DA_DIRECTION  = ChatMessage.DIRECTION;

    /**
     * a column for message type.
     * <P>Type: INTEGER</P>
     */
    public static final String COLUMN_DA_TYPE       = ChatMessage.TYPE;

    /**
     * a column for message flag, the flag contains OTO, OTM, MTM.
     * <P>Type: INTEGER</P>
     */
    public static final String COLUMN_DA_FLAG       = ChatMessage.FLAG;

    /**
     * a column for file path.
     * <P>Type: TEXT</P>
     */
    public static final String COLUMN_DA_FILENAME   = ChatMessage.FILENAME;

    /**
     * a column for file thumb path.
     * <P>Type: TEXT</P>
     */
    public static final String COLUMN_DA_FILEICON   = ChatMessage.FILEICON;

    /**
     * a column for file size.
     * <P>Type: LONG</P>
     */
    public static final String COLUMN_DA_FILESIZE   = ChatMessage.FILESIZE;

}
