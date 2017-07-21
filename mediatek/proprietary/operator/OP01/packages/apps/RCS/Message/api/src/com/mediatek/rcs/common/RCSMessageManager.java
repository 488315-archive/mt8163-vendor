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

import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Set;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Random;
import java.util.concurrent.ConcurrentHashMap;

import org.gsma.joyn.chat.ChatLog;
import org.gsma.joyn.chat.ChatLog.Message.Status.Content;
import org.gsma.joyn.ft.FileTransferLog;

import android.content.ContentResolver;
import android.content.ContentUris;
import android.content.Context;
import android.content.SharedPreferences;
import android.database.Cursor;
import android.net.Uri;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Process;
import android.os.RemoteException;
import android.provider.Telephony.Sms;
import android.provider.Telephony.Threads;
import android.util.Log;

import com.google.android.mms.util.PduCache;
import com.mediatek.rcs.common.IpMessageConsts.IpMessageType;
import com.mediatek.rcs.common.RcsLog.MessageColumn;
import com.mediatek.rcs.common.RcsLog.MessageType;
import com.mediatek.rcs.common.RcsLog.Class;
import com.mediatek.rcs.common.RcsLog.ThreadFlag;
import com.mediatek.rcs.common.RcsLog.ThreadsColumn;
import com.mediatek.rcs.common.RCSServiceManager;
import com.mediatek.rcs.common.provider.RCSDataBaseUtils;
import com.mediatek.rcs.common.provider.GroupChatCache;
import com.mediatek.rcs.common.provider.GroupChatCache.ChatInfo;
import com.mediatek.rcs.common.service.FileStruct;
import com.mediatek.rcs.common.utils.ContextCacher;
import com.mediatek.rcs.common.utils.Logger;
import com.mediatek.rcs.common.utils.RCSUtils;
import com.mediatek.rcs.common.service.IRCSChatService;
import android.media.MediaFile;
import android.text.TextUtils;
import java.io.File;

/**
* RCSMessageManager is a manager to manage messages, such as send message by
* {@link #sendRCSMessage(String, IpMessage)}, delete message by {@link #deleteRCSMessage(long)},
* get rcs message as a {@link IpMessage} by {@link #getRCSMessageInfo(long)} and so on. Use
* {@link RCSServiceManager#getRcsMessageManager()} to get {@link RCSMessageManager} instance.
*/
public class RCSMessageManager{
    private static final String TAG = "RCSMessageManager";

    public static final int SAVE_SUCCESS = 1;
    public static final int ERROR_CODE_UNSUPPORT_TYPE = 100;
    public static final int ERROR_CODE_INVALID_PATH = 101;
    public static final int ERROR_CODE_EXCEED_MAXSIZE = 102;
    public static final int ERROR_CODE_UNKNOWN = 103;
    private static RCSMessageManager sInstance;
    private Context mContext;

    /**
     * The Constant COMMA.
     */
    private static final String COMMA = ",";

    private RCSMessageManager(Context context) {
        mContext = context;
    }

    /**
     * Create {@link RCSMessageManager} instance. Call this api to create and get the instance.
     * @param context {@link Context}
     */
    protected static RCSMessageManager createInstance(Context context) {
        if (sInstance == null) {
            sInstance = new RCSMessageManager(context);
        }
        return sInstance;
    }

    /**
     * Get RCS Message information by id.
     * @param messageId the id of rcs message
     * @return one {@link IpMessage} with messageId
     */
    public IpMessage getRCSMessageInfo(long messageId) {
        Cursor cursor = RCSDataBaseUtils.getMessage(mContext, messageId);
        if (cursor == null) {
            return null;
        }
        IpMessage ipMessage = null;
        try {
            if (cursor.moveToFirst()) {
                int type = cursor.getInt(cursor.getColumnIndex(MessageColumn.TYPE));
                if (type == MessageType.FT) {
                    ipMessage = getFTMessageInfo(cursor);
                } else if (type == MessageType.IM) {
                    ipMessage = getIMMessageInfo(cursor);
                }
            }
        } finally {
            if (!cursor.isClosed()) {
                cursor.close();
            }
        }
        return ipMessage;
    }

    private IpMessage getIMMessageInfo(Cursor cursor) {
        if (cursor == null) {
            return null;
        }
        IpTextMessage ipMessage = null;
        try {
            int msgType = cursor.getInt(cursor.getColumnIndex(MessageColumn.CLASS));
            switch (msgType) {
                case Class.NORMAL:
                case Class.BURN:
                    ipMessage = new IpTextMessage();
                    break;
                case Class.EMOTICON:
                    ipMessage = new IpEmoticonMessage();
                    break;
                case Class.CLOUD:
                    // TODO
                default:
                    Logger.d(TAG,
                        "getIMMessageInfo, do not support this type currently, type=" + msgType);
                    return null;
            }
            ipMessage.setBurnedMessage(msgType == Class.BURN ? true : false);
            ipMessage.setDate(cursor.getLong(cursor.getColumnIndex(MessageColumn.TIMESTAMP)));
//            int direction = cursor.getInt(cursor.getColumnIndex(MessageColumn.DIRECTION));
//            ipMessage.setDirection(direction);
            ipMessage.setRemote(cursor.getString(cursor.getColumnIndex(
                    MessageColumn.CONTACT_NUMBER)));
            ipMessage.setId(cursor.getLong(cursor.getColumnIndex(MessageColumn.ID)));
//            ipMessage.setIpDbId(cursor.getLong(cursor.getColumnIndex(MessageColumn.IPMSG_ID)));
            ipMessage.setStatus(cursor.getInt(cursor.getColumnIndex(MessageColumn.MESSAGE_STATUS)));
            ipMessage.setMessageId(cursor.getString(cursor.getColumnIndex(
                    MessageColumn.MESSAGE_ID)));
            ipMessage.setBody(cursor.getString(cursor.getColumnIndex(MessageColumn.BODY)));
//            ipMessage.setSimId(cursor.getInt(cursor.getColumnIndex(MessageColumn.SUB_ID)));
        } finally {
            if (!cursor.isClosed()) {
                cursor.close();
            }
        }
        return ipMessage;
    }

    private IpMessage getFTMessageInfo(Cursor cursor) {
        if (cursor == null) {
            return null;
        }
        long ipmsgId = 0;
        IpMessage ipMessage = null;
        try {
            if (cursor.moveToFirst()) {
                ipmsgId = cursor.getLong(cursor.getColumnIndex(MessageColumn.IPMSG_ID));
                if (RCSCacheManager.getIpMessage(ipmsgId) != null) {
                    Log.d(TAG, "getIpFTMsgInfo, get ipMessage from cache! ");
                    String filepath =
                            ((IpAttachMessage) (RCSCacheManager.getIpMessage(ipmsgId))).getPath();
                    Log.d(TAG, "Test filepath 1  = " + filepath);
                    return RCSCacheManager.getIpMessage(ipmsgId) ;
                }
                if (isDummyId(ipmsgId)) {
                    String filePath = cursor.getString(cursor.getColumnIndex(
                            MessageColumn.FILE_PATH));
                    String remote = cursor.getString(cursor.getColumnIndex(
                            MessageColumn.CONTACT_NUMBER));
                    int status = cursor.getInt(cursor.getColumnIndex(MessageColumn.MESSAGE_STATUS));
                    int msgType = cursor.getInt(cursor.getColumnIndex(MessageColumn.CLASS));
                    boolean sessionType = msgType == Class.BURN ? true : false;
                    String fileName = getFileName(filePath);
                    String fileTransferTag = (Long.valueOf(ipmsgId)).toString();
                    String thumbNail = null;
                    int duration = RCSUtils.getDuration(mContext, filePath);
                    long size = RCSUtils.getFileSize(filePath);
                    Date date =  new Date();

                    FileStruct filestruct = new FileStruct(filePath,
                                                           fileName,
                                                           size,
                                                           fileTransferTag,
                                                           date,
                                                           remote,
                                                           thumbNail,
                                                           sessionType,
                                                           duration);
                    ipMessage = RCSUtils.analysisFileType(remote,filestruct);
//                    ipMessage.setIpDbId(ipmsgId);
                    ipMessage.setMessageId(fileTransferTag);
                    ipMessage.setStatus(status);
                } else {
                    ipMessage = getIpFTMsgInfo(ipmsgId);
                }
            }
        } finally {
            if (!cursor.isClosed()) {
                cursor.close();
            }
        }
        return ipMessage;
    }

    private IpMessage getIpFTMsgInfo(long ipmsgId) {
        Log.d(TAG, "getIpFTMsgInfo() ipmsgId = " + ipmsgId);
        IpMessage ipMessage = null;
        ContentResolver resolver = mContext.getContentResolver();
        Cursor cursor;

        if (RCSCacheManager.getIpMessage(ipmsgId) != null) {
            Log.d(TAG, "getIpFTMsgInfo, get ipMessage from cache! ");
            String filepath = ((IpAttachMessage)(RCSCacheManager.getIpMessage(ipmsgId))).getPath();
            Log.d(TAG, "Test filepath 1  = " + filepath);
            return RCSCacheManager.getIpMessage(ipmsgId) ;
        }

        //one2one or group
        cursor = resolver.query(RCSUtils.RCS_URI_FT, RCSUtils.PROJECTION_FILE_TRANSFER,
                "_id=" + ipmsgId, null, null);

        try {
            if (cursor != null && cursor.moveToFirst()) {
                String filePath = cursor.getString(cursor.getColumnIndex(FileTransferLog.FILENAME));
                int index = filePath.lastIndexOf("/");
                String fileName = filePath.substring(index + 1);
                long size = cursor.getLong(cursor.getColumnIndex(FileTransferLog.FILESIZE));
                String fileTransferTag = cursor.getString(cursor.getColumnIndex(
                        FileTransferLog.FT_ID));
                String remote = cursor.getString(cursor.getColumnIndex(
                        FileTransferLog.CONTACT_NUMBER));
                String thumbNail = cursor.getString(cursor.getColumnIndex(
                        FileTransferLog.FILEICON));
                Date date =  new Date();
                boolean sessionType = false;
                sessionType = (cursor.getInt(
                        cursor.getColumnIndex(FileTransferLog.SESSION_TYPE)) == 1) ? true:false;

                int duration;
                if (sessionType) {
                    duration = RCSUtils.getDuration(mContext, filePath);
                    Log.d(TAG, "BURN, get duration from path, duration = " + duration);
                } else {
                    duration = cursor.getInt(cursor.getColumnIndex(FileTransferLog.DURATION));
                    Log.d(TAG, "NOT BURN, get duration from stack, duration = " + duration);
                }
                int state = cursor.getInt(cursor.getColumnIndex(FileTransferLog.STATE));
                        Log.d(TAG, "Test filepath 2  = " + filePath);
                FileStruct filestruct = new FileStruct(filePath,
                                                       fileName,
                                                       size,
                                                       fileTransferTag,
                                                       date,
                                                       remote,
                                                       thumbNail,
                                                       sessionType,
                                                       duration);
                Log.d(TAG, "yangfeng test stack db state  = " + state);
                Log.d(TAG, "yangfeng test new FileStruct: filePath = " + filePath +
                        "fileName = " + fileName +" thumbNail = " + thumbNail);
                ipMessage = RCSUtils.analysisFileType(remote,filestruct);
                if (ipMessage != null) {
//                    ipMessage.setIpDbId(ipmsgId);
                    ipMessage.setMessageId(fileTransferTag);
                    ((IpAttachMessage) ipMessage).setRcsStatus(
                            RCSUtils.getRcsStatusformStack(state));
                    ipMessage.setStatus(RCSDataBaseUtils.getFTMessageStatus(mContext, ipmsgId));
                }
            }
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }

        RCSCacheManager.setIpMessage(ipMessage,ipmsgId);

        Log.d(TAG, "yangfeng test getIpFTMsgInfo() ipMessage = " + ipMessage);
        return ipMessage;
    }

    /**
     * Delete the RCS message specified by a msgId.
     * @param msgId the id of rcs message in rcs message table.
     * @return The number of rows deleted
     */
    public int deleteRCSMessage(long msgId) {
        Uri uri = ContentUris.withAppendedId(RCSDataBaseUtils.URI_RCS_MESSAGE, msgId);
        return mContext.getContentResolver().delete(uri, null, null);
    }

    private String getFileName(String filePath) {
        int index = filePath.lastIndexOf("/");
        String fileName = filePath.substring(index + 1);
        return fileName;
    }

    private boolean isDummyId(long ipmsgId) {
        if (ipmsgId > Integer.MAX_VALUE - 1001 && ipmsgId < Integer.MAX_VALUE ) {
            return true;
        } else {
            return false;
        }
    }

    /**
     * Send RCS message to specified by chatId. if send group message, should set the
     * parameter chatId as the group's chat id, else can set the parameter chatId as "".
     * To send RCS message, should create one {@link IpMessage}, set receiver(s)'s number(s) by
     * {@link IpMessage#setRemote(String)}. If the parameter msg is text message,
     * need set the content {@link IpTextMessage#setBody(String)}, if the parameter msg is
     * {@link IpAttachMessage}, need set the content by {@link IpAttachMessage#setPath(String)} and
     * {@link IpVideoMessage#setDuration(int)}, {@link IpVideoMessage#setSize(int)} etc. If send
     * burned message, need set the burned flag by {@link IpMessage#setBurnedMessage(boolean)}.
     * @param chatId group chat id, the parameter is ignored if the message is not group message
     * @param msg the {@link IpMessage} to send
     * @return one of the values follow: {@link #SAVE_SUCCESS}, {@link #ERROR_CODE_EXCEED_MAXSIZE},
     * {@link #ERROR_CODE_INVALID_PATH}, {@link #ERROR_CODE_UNSUPPORT_TYPE} and
     * {@link #ERROR_CODE_UNKNOWN}
     */
    public int sendRCSMessage(String chatId, IpMessage msg) {
        int result = SAVE_SUCCESS;
        if (msg instanceof IpTextMessage) {
            IpTextMessage message = (IpTextMessage) msg;
            sendTextRCSMessage(message, chatId);
        } else {
            IpAttachMessage message = (IpAttachMessage) msg;
            result = sendFileTransfer(message, chatId);
        }
        return result;
    }

    private int sendFileTransfer(IpAttachMessage msg, String chatId) {
        Log.d(TAG, "sendFileTransfer enter chatId = " + chatId);
        if (msg == null) {
            return ERROR_CODE_UNKNOWN ;
        }

        /* check if the ipmessage is sendable */
        if (msg.getSize() > RCSUtils.getFileTransferMaxSize()*1024) {
            Log.d(TAG, "msg.getSize() = " + msg.getSize() +
                    " maxSize = " + RCSUtils.getFileTransferMaxSize()*1024);
            return ERROR_CODE_EXCEED_MAXSIZE;
        }

        String filePath = msg.getPath();
        int index = filePath.lastIndexOf("/");
        String fileName = filePath.substring(index + 1);
        String mimeType = MediaFile.getMimeTypeForFile(fileName);

        if(mimeType == null && !(fileName.toLowerCase().endsWith(".vcf"))
            && !(fileName.toLowerCase().endsWith(".xml"))) {
            Log.d(TAG, "saveFileTransferMsg() mimeType null");
            return ERROR_CODE_UNSUPPORT_TYPE;
        }

        if (TextUtils.isEmpty(filePath)) {
            Log.e(TAG, "saveFileTransferMsg() invalid filePath: " + filePath);
            return ERROR_CODE_INVALID_PATH;
        }

        File file = new File(filePath);
        if (!file.exists()) {
            Log.e(TAG, "saveFileTransferMsg() file does not exist: " + filePath);
            return ERROR_CODE_INVALID_PATH;
        }
        IRCSChatService service = RCSServiceManager.getInstance().getChatService();
        try {
            String contact = msg.getRemote();
            if (!TextUtils.isEmpty(chatId)) {
                Log.d(TAG, "sendGroupFilrTransfer, content=" + msg.getPath());
                service.sendGroupFileTransfer(chatId, msg.getPath());
            } else if (contact != null && contact.contains(COMMA)){
                Log.d(TAG, "sendOne2MultiFT, filePath= " + msg.getPath() + ", contact=" + contact);
                List<String> recipients = collectMultiContact(contact);
                service.sendOne2MultiFileTransfer(recipients,msg.getPath());
            } else if (contact != null && !contact.contains(COMMA)) {
                if (msg.getBurnedMessage()) {
                    Log.d(TAG, "sendBurnFT, filePath= " + msg.getPath());
                    service.sendOne2OneBurnFileTransfer(contact, msg.getPath());
                } else {
                    Logger.d(TAG, "sendOne2OneFT, filePath= " + msg.getPath());
                    service.sendOne2OneFileTransfer(contact, msg.getPath());
                }
            }
        } catch (RemoteException e) {
            e.printStackTrace();
        }
        return SAVE_SUCCESS;
    }

    private void sendTextRCSMessage(IpTextMessage msg, String chatId) {
        Logger.d(TAG, "sendTextRCSMessage enter, chatId=" + chatId);
        if (msg == null) {
            return;
        }
        IRCSChatService service = RCSServiceManager.getInstance().getChatService();
        try {
            int type = Class.NORMAL;
            if (msg.getBurnedMessage()) {
                type = Class.BURN;
            } else if (msg.getType() == IpMessageConsts.IpMessageType.EMOTICON) {
                type = Class.EMOTICON;
            }
            String contact = msg.getRemote();
            if (!TextUtils.isEmpty(chatId)) {
                Logger.d(TAG, "sendGroupMessage, content=" + msg.getBody());
                service.sendGroupMessage(chatId, msg.getBody(), type);
            } else if (contact != null && contact.contains(COMMA)){
                Logger.d(TAG, "sendOne2MultiMessage, content=" + msg.getBody() +
                        ", contact=" + contact);
                List<String> recipients = collectMultiContact(contact);
                service.sendOne2MultiMessage(recipients, msg.getBody(), type);
            } else if (contact != null && !contact.contains(COMMA)) {
                Logger.d(TAG, "sendOne2OneMessage, content=" + msg.getBody());
                service.sendOne2OneMessage(contact, msg.getBody(), type);
            }
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    /**
     * Resend failed RCS message with specified msgId.
     * @param msgId msgId the id of rcs message in rcs message table.
     */
    public void resendRCSMessage(long msgId) {
        IRCSChatService service = RCSServiceManager.getInstance().getChatService();
        try {
            service.resendRCSMessage(msgId);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    /**
     * Delete RCS messages in threads with specified threadId list.
     * @param threadIds the thread id list to be deleted
     * @param maxSmsId this value is not used now
     * @param deleteLock whether delete locked messages, if it's true, will delete locked messages.
     */
    public void deleteRCSThreads(Collection<Long> threadIds, long maxSmsId, boolean deleteLock) {
        long now = System.currentTimeMillis();
        Uri uri = Threads.CONTENT_URI;
        String selection = deleteLock ? null : "locked=0";
        PduCache.getInstance().purge(uri);
        uri = uri.buildUpon().appendQueryParameter("multidelete", "true").build();
        uri = uri.buildUpon().appendQueryParameter("date", Long.toString(now))
                .appendQueryParameter("deleteThread", "true")
                .appendQueryParameter("rcs", "true").build();
        String[] selectionArgs = new String[threadIds.size()];
        Iterator<Long> iter = threadIds.iterator();
        int i = 0;
        while (iter.hasNext()) {
            selectionArgs[i++] = String.valueOf(iter.next());
        }
        mContext.getContentResolver().delete(uri, null, selectionArgs);
    }

    public void deleteLastBurnedMessage() {
        // delete ip burned message
        SharedPreferences sp = ContextCacher.getPluginContext().getSharedPreferences(
                IpMessageConsts.BurnedMsgStoreSP.PREFS_NAME, Context.MODE_WORLD_READABLE);
        Set<String> burnedMsgList = sp.getStringSet(
                IpMessageConsts.BurnedMsgStoreSP.PREF_PREFIX_KEY, null);
        if (burnedMsgList == null) {
            Log.d(TAG, "[BurnedMsg]: onIpMmsCreate() burnedMsgList is null");
            return;
        }
        Log.d(TAG, "[BurnedMsg]: onIpMmsCreate() burnedMsgList = "+burnedMsgList);
        for (String id : burnedMsgList) {
            deleteRCSMessage(Long.valueOf(id));
        }

        SharedPreferences spDelete = ContextCacher.getPluginContext().getSharedPreferences(
                IpMessageConsts.BurnedMsgStoreSP.PREFS_NAME, Context.MODE_WORLD_READABLE);
        SharedPreferences.Editor prefsDelete = spDelete.edit();
        prefsDelete.clear();
        prefsDelete.apply();
    }

    /**
     * Download file transfer(Image, Video). It support continuous transmission download.
     * @param chatId if the message is in group, set to group's chat id, else set to ""
     * @param fid file transfer id. app can get the file transfer id by
     * {@link IpAttachMessage#getFtId()}
     * @return one the values follow: {@link #SAVE_SUCCESS}, {@link #ERROR_CODE_UNKNOWN}
     */
    public int downloadAttach(String chatId, String fid) {
        Log.d(TAG, "downloadAttach enter, chatId = " + chatId + " fid = " + fid);
        IRCSChatService service = RCSServiceManager.getInstance().getChatService();

        try {
                if (!TextUtils.isEmpty(chatId)) {
                    // for group chat
                    service.downloadGroupFile(chatId, fid);
                } else {
                    // for one2one
                    service.downloadFile(fid);
                }
            } catch (RemoteException e) {
                e.printStackTrace();
        }
        return SAVE_SUCCESS;
    }


    /**
     * Pause download file transfer. Can called after {@link #downloadAttach(String, String)}.
     * @param ftId the same value in {@link #downloadAttach(String, String)}
     * @return @return one the values follow: {@link #SAVE_SUCCESS}, {@link #ERROR_CODE_UNKNOWN}
     */
    public int pauseDownloadAttach(String ftId) {
        Log.d(TAG, "pauseDownloadAttach enter, ftId = " + ftId);

        IRCSChatService service = RCSServiceManager.getInstance().getChatService();
        try {
            service.pauseFileTransfer(ftId);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
        return SAVE_SUCCESS;
    }

    private List<String> collectMultiContact(String contact) {
        String[] contacts = contact.split(COMMA);
        List<String> contactSet = new ArrayList<String>();
        for (String singleContact : contacts) {
            // TODO: format number
            contactSet.add(singleContact);
        }
        return contactSet;
    }

//    public void sendBurnDeliveryReport(String contact,String msgId) {
//        Logger.d(TAG,
//        "sendBurnDeliveryReport drawDeleteBARMsgIndicator contact= "+contact+" msgId = "+ msgId );
//        IRCSChatService service = RCSServiceManager.getInstance().getChatService();
//        try {
//            service.sendBurnDeliveryReport(contact, msgId);
//        } catch (RemoteException e) {
//            e.printStackTrace();
//        }
//    }

    /**
     * Init spam report. This function is not used now.
     * @param contact
     * @param msgId
     * @param msgType
     * @param isPagerMode
     */
    public void initSpamReport(String contact, String msgId, int msgType,boolean isPagerMode) {
        Logger.d(TAG, " [spam-report] initSpamReport contact = " + contact +
                " msgId = " + msgId+" msgType = " + msgType);
//        IRCSChatService service = RCSServiceManager.getInstance()
//                .getChatService();
//        try {
//            if (msgType == IpMessageType.TEXT) {
//                service.initiateSpamReport(contact, msgId,isPagerMode);
//            } else {
//                service.initiateFileSpamReport(contact, msgId);
//            }
//        } catch (RemoteException e) {
//            e.printStackTrace();
//        }
    }
}