/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2012. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

package com.mediatek.rcs.common;

public final class IpMessageConsts {

    public static final String STATUS = "status";
    public static final char BURN_INDICATOR = 0x07;

    public static final class ServiceNotification {
        public static final String BROADCAST_RCS_NEW_MESSAGE =
                "com.mediatek.rcs.message.BROADCAST_RCS_NEW_MESSAGE";
        public static final String BROADCAST_RCS_GROUP_NEW_MESSAGE =
                "com.mediatek.rcs.message.BROADCAST_RCS_GROUP_NEW_MESSAGE";
        public static final String BROADCAST_RCS_GROUP_BEEN_KICKED_OUT =
                "com.mediatek.rcs.message.BROADCAST_RCS_GROUP_BEEN_KICKED_OUT";
        public static final String BROADCAST_RCS_GROUP_ABORTED =
                "com.mediatek.rcs.message.BROADCAST_RCS_GROUP_ABORTED";
        public static final String BROADCAST_RCS_GROUP_INVITATION =
                "com.mediatek.rcs.message.BROADCAST_RCS_GROUP_NEW_INVITATION";

        public static final String KEY_CHAT_ID = "chatId";
        public static final String KEY_CONTACT = "contact";
        public static final String KEY_MSG_ID = "msgId";
        public static final String KEY_SUBJECT = "subject";
        public static final String KEY_PARTICIPANT = "participant";
    }

    public static final class MessageAction {
        public static final String ACTION_NEW_MESSAGE = "com.mediatek.rcs.message.newMessage";
        public static final String ACTION_SEND_FAIL = "com.mediatek.rcs.message.sendFail";
        public static final String KEY_THREAD_ID = "threadId";
        public static final String KEY_IPMSG_ID = "ipmsgId";
        public static final String KEY_MSG_ID = "msgId";
        public static final String KEY_NUMBER = "number";
    }

    public static final class BurnedMsgStoreSP {
        public static final String PREFS_NAME = "rcs.message.plugin.BurnedMsgStoreSP";
        public static final String PREF_PREFIX_KEY = "msg_id";
    }

    public static final class GroupNotificationType {
        public static final int NOTIFICATION_ENABLE         = 1;
        public static final int NOTIFICATION_DISABLE        = 0;
        public static final int NOTIFICATION_REJECT         = 2;
    }

    public static final class GroupActionList {
        public static final String ACTION_GROUP_NOTIFY = "com.mediatek.rcs.group.notify";
        public static final String KEY_NOTIFY_TYPE          = "groupNotifyType";
        public static final int VALUE_PARTICIPANT_JOIN      = 1;
        public static final int VALUE_PARTICIPANT_LEFT      = 2;
        public static final int VALUE_CHAIRMEN_CHANGED      = 3;
        public static final int VALUE_SUBJECT_MODIFIED      = 4;
        public static final int VALUE_ME_REMOVED            = 5;
        public static final int VALUE_GROUP_ABORTED         = 6;
        public static final int VALUE_NEW_INVITE_RECEIVED   = 7;
        public static final int VALUE_PARTICIPANT_REMOVED   = 8;
        public static final int VALUE_PARTICIPANT_NICKNAME_MODIFIED   = 9;
        public static final int VALUE_SYNC_CONFERENCE_NOTIFY_DONE     = 10;

        public static final String ACTION_GROUP_OPERATION_RESULT =
                "com.mediatek.rcs.group.operation.result";
        public static final String KEY_ACTION_TYPE          = "groupActionType";
        public static final int VALUE_ADD_PARTICIPANTS      = 1;
        public static final int VALUE_REMOVE_PARTICIPANTS   = 2;
        public static final int VALUE_TRANSFER_CHAIRMEN     = 3;
        public static final int VALUE_MODIFY_SUBJECT        = 4;
        public static final int VALUE_MODIFY_NICK_NAME      = 5;
        public static final int VALUE_MODIFY_SELF_NICK_NAME = 6;
        public static final int VALUE_EXIT_GROUP            = 7;
        public static final int VALUE_DESTROY_GROUP         = 8;
        public static final int VALUE_ACCEPT_GROUP_INVITE   = 10;
        public static final int VALUE_REJECT_GROUP_INVITE   = 11;

        public static final String KEY_ACTION_RESULT        = "result";
        public static final int VALUE_SUCCESS               = 0;
        public static final int VALUE_FAIL                  = -1;

        public static final String KEY_CONTACT_NUMBER       = "contact_number";
        public static final String KEY_CONTACT_NAME         = "contact_name";
        public static final String KEY_THREAD_ID            = "threadId";
        public static final String KEY_SUBJECT              = "subject";
        public static final String KEY_NICK_NAME            = "nick_name";
        public static final String KEY_SELF_NICK_NAME       = "self_nick_name";
        public static final String KEY_CHAT_ID              = "chatId";
        public static final String KEY_PARTICIPANT          = "participant";
        public static final String KEY_PARTICIPANT_LIST     = "participant_list";
        public static final String KEY_ADD_SYS_EVENT        = "add_system_event";

        public static final int GROUP_STATUS_INVALID         = -10000;
        public static final int GROUP_STATUS_INVITE_EXPAIRED = -10001;
        public static final int GROUP_STATUS_INVITING        = -10002;
        public static final int GROUP_STATUS_INVITING_AGAIN  = -10003;
    }

    // Add for Joyn
    public static final class DisableServiceStatus {
        public static final String ACTION_DISABLE_SERVICE_STATUS =
                "com.mediatek.mms.ipmessage.disableServiceStatus";
        public static final int ENABLE = 0;
        public static final int DISABLE_TEMPORARY = 1;
        public static final int DISABLE_PERMANENTLY = 2;
    }

    public static final class IpMessageStatus {
        public static final String ACTION_MESSAGE_STATUS =
                "com.mediatek.mms.ipmessage.messageStatus";
        public static final String IP_MESSAGE_ID =
                "com.mediatek.mms.ipmessage.IpMessageRecdId";
    }

    public static final class IpMessageType {
        public static final int TEXT = 0;
        public static final int EMOTICON = 1;
        // file transfer base
        public static final int FT_BASE = 50;
        public static final int PICTURE = FT_BASE + 1;
        public static final int VOICE = FT_BASE + 2;
        public static final int VCARD = FT_BASE+ 3;
        public static final int VIDEO = FT_BASE + 4;
        public static final int CALENDAR = FT_BASE + 5;
        public static final int GEOLOC = FT_BASE + 6;
        public static final int UNKNOWN_FILE = FT_BASE + 7;
    }
}
