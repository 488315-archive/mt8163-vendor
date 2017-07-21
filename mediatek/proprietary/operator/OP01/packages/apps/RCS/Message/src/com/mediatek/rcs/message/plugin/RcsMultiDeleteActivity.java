package com.mediatek.rcs.message.plugin;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.AsyncQueryHandler;
import android.content.ContentUris;
import android.content.Context;
import android.content.ContentValues;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.database.Cursor;
import android.preference.PreferenceManager;
import android.provider.Telephony.Mms;
import android.provider.Telephony.Sms;
import android.media.MediaFile;
import android.net.Uri;
import android.os.Environment;
import android.text.TextUtils;
import android.telephony.SubscriptionManager;
import android.util.Log;
import android.view.ActionMode;
import android.view.Menu;
import android.view.MenuItem;
import android.webkit.MimeTypeMap;
import android.widget.Toast;

import com.android.mms.data.Contact;
import com.cmcc.ccs.chat.ChatMessage;
import com.cmcc.ccs.chat.ChatService;

import com.google.android.mms.MmsException;
import com.google.android.mms.pdu.EncodedStringValue;
import com.google.android.mms.pdu.NotificationInd;
import com.google.android.mms.pdu.PduBody;
import com.google.android.mms.pdu.PduParser;
import com.google.android.mms.pdu.GenericPdu;
import com.google.android.mms.pdu.MultimediaMessagePdu;
import com.google.android.mms.pdu.PduComposer;
import com.google.android.mms.pdu.PduPersister;
import com.google.android.mms.pdu.RetrieveConf;
import com.google.android.mms.pdu.SendReq;
import com.google.android.mms.pdu.GenericPdu;
import static com.google.android.mms.pdu.PduHeaders.MESSAGE_TYPE_NOTIFICATION_IND;
import static com.google.android.mms.pdu.PduHeaders.MESSAGE_TYPE_RETRIEVE_CONF;
//
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Collection;
import java.util.Collections;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;
import java.util.Map.Entry;

import com.mediatek.mms.callback.IMultiDeleteActivityCallback;
import com.mediatek.mms.ipmessage.DefaultIpMultiDeleteActivityExt;
import com.mediatek.mms.ipmessage.IIpMessageListAdapterExt;
import com.mediatek.mms.ipmessage.IIpMessageItemExt;
import com.mediatek.rcs.common.RcsLog;

import com.mediatek.rcs.common.RCSServiceManager;
import com.mediatek.rcs.common.IpMessageConsts.IpMessageType;
import com.mediatek.rcs.common.RcsLog.MessageStatus;
import com.mediatek.rcs.common.utils.ContextCacher;
import com.mediatek.rcs.common.utils.RCSUtils;
import com.mediatek.rcs.common.provider.FavoriteMsgData;
import com.mediatek.rcs.common.RCSMessageManager;
import com.mediatek.rcs.common.IpMessage;
import com.mediatek.rcs.common.IpAttachMessage;
import com.mediatek.rcs.common.IpTextMessage;
import com.mediatek.rcs.common.MessageStatusUtils.IFileTransfer.Status;
import com.mediatek.rcs.message.R;
import com.mediatek.rcs.message.data.ForwardSendData;
import com.mediatek.rcs.message.plugin.RcsMessageListAdapter.RCSColumnsMap;
import com.mediatek.rcs.message.provider.FavoriteMsgProvider;
import com.mediatek.rcs.message.ui.FavoriteDataItem.Constants;
import com.mediatek.rcs.message.utils.RcsMessageUtils;

/**
 * Plugin implements. response MultiDeleteActivity.java in MMS host.
 *
 */
public class RcsMultiDeleteActivity extends DefaultIpMultiDeleteActivityExt {
    private static String TAG = "RcsMultiDeleteActivity";

    public static final String IPMSG_IDS = "forward_ipmsg_ids";
    private static final int MENU_FAVORITE = 1001;
    private static final int MENU_FORWARDONEBYONE = 1002;
    private static final int MAX_COUNT_OF_COMBINE_FORWARD = 10;
    /// M: add for ipmessage, record the ipmessage id.
    private Map<Long, Long> mSelectedIpMessageIds = new HashMap<Long, Long>();
    private Activity mContext;
    private Context mPluginContext;
    private long mThreadId;
    private int mForwardMenuId;
    private String mChatId;
    private boolean mIsGroupChat;
    private int mFtCount;

    //favorite
    private String COLUMN_NAME_ID = "_id";
    private String COLUMN_NAME_TYPE = "m_type";
    private String COLUMN_NAME_DATE = "date";
    private String COLUMN_NAME_MESSAGE_BOX = "msg_box";
    private String COLUMN_NAME_READ = "read";
    private String FILE_EXT_PDU = ".pdu";

    private static final String SMS_FORWARD_WITH_SENDER = "pref_key_forward_with_sender";
    boolean favoriteSmsSuccess = false;
    boolean favoriteMmsSuccess = false;
    boolean favoriteRcsSuccess = false;
    private ProgressDialog mProgressDialog;

    private static final String[] SMS_PROJECTION =
    {"body"};

    private static final String[] RCS_PROJECTION =
    {RcsLog.MessageColumn.BODY, RcsLog.MessageColumn.FILE_PATH, RcsLog.MessageColumn.TYPE};

    private IMultiDeleteActivityCallback mCallback;
    private RcsMessageListAdapter mAdapter;
    RCSMessageManager mRcsMessageManager;

    @Override
    public boolean MultiDeleteActivityInit(Activity context,
                                        IMultiDeleteActivityCallback callback) {
        mContext = context;
        mCallback = callback;
        mRcsMessageManager = RCSServiceManager.getInstance().getRcsMessageManager();
        mPluginContext = ContextCacher.getPluginContext();
        mThreadId = mContext.getIntent().getLongExtra("thread_id", 0);
        mChatId = RcsMessageUtils.blockingGetGroupChatIdByThread(context, mThreadId);
        mIsGroupChat = !TextUtils.isEmpty(mChatId);
        return true;
    }

    @Override
    public boolean onIpMultiDeleteClick(AsyncQueryHandler handler, int token, Object cookie,
            int deleteRunningCount, boolean deleteLocked) {
        /// M: delete ipmessage
        if (mSelectedIpMessageIds.size() > 0) {
            Iterator<Entry<Long, Long>> iter = mSelectedIpMessageIds.entrySet().iterator();
            HashSet<Long> mRcsMessageIds = new HashSet<Long>();
            while (iter.hasNext()) {
                 Map.Entry<Long, Long> entry = iter.next();
                 long key = entry.getKey();
                 long msgId = RcsMessageListAdapter.getRcsMsgIdByKey(key);
                 mRcsMessageIds.add(msgId);
            }
            String selection = combineRcsMessageSelection(mRcsMessageIds);
            Log.d(TAG, "[onIpMultiDeleteClick] selection = " + selection);
            handler.startDelete(token, cookie, RcsLog.MessageColumn.CONTENT_URI, selection, null);
            mCallback.setDeleteRunningCount(deleteRunningCount++);
            Log.d(TAG, "delete ipmessage, id:" + mSelectedIpMessageIds.size());
            mSelectedIpMessageIds.clear();
            return true;
        }
        return false;
    }

    private String combineRcsMessageSelection(Set<Long> idSet) {
        if (idSet == null || idSet.size() <= 0) {
            return null;
        }
        StringBuilder sb = new StringBuilder(RcsLog.MessageColumn.ID +" In (");
        boolean firstItem = true;
        for (long id : idSet) {
            if (!firstItem) {
                sb.append(",");
            }
            long rcsId = RcsMessageListAdapter.getRcsMsgIdByKey(id);
            sb.append(rcsId);
            firstItem = false;
        }
        sb.append(")");
        Log.d(TAG, "thiss combineRcsMessageSelection sb.toString() = " + sb.toString());
        return sb.toString();
    }

    @Override
    public boolean onIpDeleteLockedIpMsg(long msgId) {
        if (mSelectedIpMessageIds.size() > 0 && mSelectedIpMessageIds.containsKey(msgId)) {
            mSelectedIpMessageIds.remove(msgId);
            return true;
        }
        return false;
    }

    @Override
    public boolean onIpHandleItemClick(IIpMessageItemExt item, long ipMessageId, boolean isSelected,
            long key) {
        RcsMessageItem rcsItem = (RcsMessageItem) item;
        if (rcsItem.isRcs()) {
            if (isSelected) {
                mSelectedIpMessageIds.put(key, ipMessageId);
            } else {
                mSelectedIpMessageIds.remove(key);
            }
        }
        return true;
    }

    @Override
    public boolean onIpMarkCheckedState(Cursor cursor, boolean checkedState) {
        if (checkedState) {
            RCSColumnsMap columnMap = mAdapter.mRcsColumnsMap;
            if (cursor != null && cursor.moveToFirst()) {
                do {
                    String type = cursor.getString(columnMap.getColumnMsgType());
                    if (type.equals("rcs")) {
                        long msgId = cursor.getLong(columnMap.getColumnMsgId());
                        long ipMessageId = cursor.getLong(columnMap.mColumnRcsMessageIpMsgId);
                        long key = RcsMessageListAdapter.getKey(type, msgId);
                        if (!mSelectedIpMessageIds.containsKey(key)) {
                            mSelectedIpMessageIds.put(key, ipMessageId);
                        }
                    }
                } while (cursor.moveToNext());
            }
        } else {
            mSelectedIpMessageIds.clear();
        }
        return false;
    }

    @Override
    public boolean onAddSelectedIpMessageId(boolean checkedState, long msgId, long ipMessageId) {
        /// M: add for ipmessage
//        if (checkedState && ipMessageId != 0) {
//            mSelectedIpMessageIds.put(msgId, ipMessageId);
//        }
        return true;
    }

    @Override
    public boolean onIpDeleteThread(Collection<Long> threads, int maxSmsId) {
        Log.d(TAG, "onIpDeleteThread, threads = " + threads);
        Iterator<Long> iter = threads.iterator();
        // TODO unfinished function, deleteAll from multiDeleteActivity
//        while (iter.hasNext()) {
//            long threadId = iter.next();
//            mRcsMessageManager.deleteThreadFromMulti(threadId, maxSmsId);
//        }
        mSelectedIpMessageIds.clear();
        return true;
    }


    //favorite
    @Override
    public boolean onCreateIpActionMode(final ActionMode mode, Menu menu) {
      /// M: add for ipmessage menu
        //if (RCSServiceManager.getInstance().serviceIsReady()) {
            MenuItem item = menu
                    .add(0, MENU_FAVORITE, 0, mPluginContext.getString(R.string.menu_favorite))
                    .setTitle(mPluginContext.getString(R.string.menu_favorite));
            item.setVisible(true);
            item = menu
                    .add(0, MENU_FORWARDONEBYONE, 0,
                     mPluginContext.getString(R.string.menu_forward_one_by_one))
                    .setTitle(mPluginContext.getString(R.string.menu_forward_one_by_one));
            item.setVisible(true);
        //}
        return true;
    }

    @Override
    public boolean onPrepareIpActionMode(ActionMode mode, Menu menu, int selectNum, int ForwardMenuId) {
        mForwardMenuId = ForwardMenuId;
        MenuItem favoriteItem = menu.findItem(MENU_FAVORITE);
        if (favoriteItem != null) {
            if (selectNum > 0) {
                favoriteItem.setVisible(true);
            } else {
                favoriteItem.setVisible(false);
            }
        }
        MenuItem forwardOneByOneItem = menu.findItem(MENU_FORWARDONEBYONE);
        if (forwardOneByOneItem != null) {
            if (RCSServiceManager.getInstance().serviceIsReady() &&
                    selectNum > 1) {
                forwardOneByOneItem.setVisible(true);
            } else {
                forwardOneByOneItem.setVisible(false);
            }
        }
        return true;
    }

    @Override
    public boolean onIpActionItemClicked(ActionMode mode, MenuItem item, long[][] selectedIds,
            String[] contacts, Cursor cursor) {
        if (item.getItemId() == MENU_FAVORITE || item.getItemId() == mForwardMenuId ||
                item.getItemId() == MENU_FORWARDONEBYONE) {
            Log.d(TAG, "thiss onIpActionItemClicked");
            HashSet<Long> smsIds = null;
            HashSet<Long> mmsIds = null;
            HashSet<Long> rcsIds = null;
            boolean mHasUnDownloadMsg = false;
            if (selectedIds[0] != null && selectedIds[0].length > 0) {
                smsIds = collectIds(selectedIds[0]);
                if (mSmsItem == null) {
                    mSmsItem = new HashMap<Long, smsBodyandAddress>();
                } else {
                    mSmsItem.clear();
                }
            }
            if (selectedIds[1] != null && selectedIds[1].length > 0) {
                mmsIds = collectIds(selectedIds[1]);
                if (mMmsItem == null) {
                    mMmsItem = new HashMap<Long, mmsSubjectandType>();
                } else {
                    mMmsItem.clear();
                }
            }
            if (selectedIds.length > 2 && selectedIds[2] != null && selectedIds[2].length > 0) {
                rcsIds = collectIds(selectedIds[2]);
                if (mRcsItem == null) {
                    mRcsItem = new HashMap<Long, rcsMsgContent>();
                } else {
                    mRcsItem.clear();
                }
            }

            if (item.getItemId() == MENU_FAVORITE) {
                Log.d(TAG, "thiss onIpActionItemClicked MENU_FAVORITE");
                doFavorite(cursor, contacts, smsIds, mmsIds, rcsIds);
                return true;
            } else if (item.getItemId() == mForwardMenuId) {
                Log.d(TAG, "thiss onIpActionItemClicked mForwardMenuId");
                return doForward(cursor, smsIds, mmsIds, rcsIds);
            } else if (item.getItemId() == MENU_FORWARDONEBYONE) {
                Log.d(TAG, "thiss onIpActionItemClicked MENU_FORWARDONEBYONE");
                doOneByOneForward(cursor, smsIds, mmsIds, rcsIds);
                return true;
            }
        }
        return false;
    }

    private boolean doFavorite(Cursor cursor, String[] contacts, HashSet<Long> smsIds,
           HashSet<Long> mmsIds, HashSet<Long> rcsIds) {
        if (cursor == null || cursor.getCount() <= 0) {
            return false;
        }
        int smsNumber = smsIds == null ? 0 : smsIds.size();
        int mmsNumber = mmsIds == null ? 0 : mmsIds.size();
        int rcsNumber = rcsIds == null ? 0 : rcsIds.size();
        if ((smsNumber + rcsNumber + mmsNumber) > MAX_COUNT_OF_COMBINE_FORWARD) {
            String toastString = mPluginContext.getString(R.string.favorite_item_too_much);
            Toast.makeText(mContext, toastString, Toast.LENGTH_LONG).show();
            return true;
        }
        boolean hasUnDownloadMsg = false;
        try {
            if (cursor.moveToFirst()) {
                do {
                    String type = cursor.getString(0);
                    long msgId = cursor.getLong(1);
                    long key = RcsMessageListAdapter.getKey(type, msgId);
                    if (type.equals("mms") && mmsIds.contains(msgId)) {
                        int boxId = cursor.getInt(cursor.getColumnIndex(Mms.MESSAGE_BOX));
                        int messageType = cursor.getInt(
                                                cursor.getColumnIndexOrThrow(Mms.MESSAGE_TYPE));
                        long date = cursor.getInt(cursor.getColumnIndex(Mms.DATE));
                        if (boxId == Mms.MESSAGE_BOX_INBOX
                                && messageType == MESSAGE_TYPE_NOTIFICATION_IND) {
                            hasUnDownloadMsg  = true;
                            continue;
                        }
                        String sub = cursor.getString(cursor.getColumnIndexOrThrow(Mms.SUBJECT));
                        int sub_cs = 0;
                        if (!TextUtils.isEmpty(sub)) {
                            sub_cs =cursor.getInt(
                                                cursor.getColumnIndexOrThrow(Mms.SUBJECT_CHARSET));
                        }
                        mmsSubjectandType st =
                                    new mmsSubjectandType(boxId, messageType, sub, sub_cs, date);
                        mMmsItem.put(msgId, st);
                    } else if (type.equals("sms") && smsIds.contains(msgId)) {
                        String body    = cursor.getString(cursor.getColumnIndexOrThrow(Sms.BODY));
                        int boxId = cursor.getInt(cursor.getColumnIndexOrThrow(Sms.TYPE));
                        long date = cursor.getInt(cursor.getColumnIndex(Sms.DATE));
                        smsBodyandAddress  ba = new smsBodyandAddress(body, boxId, date);
                        mSmsItem.put(msgId, ba);
                    } else if (type.equals("rcs") && rcsIds.contains(key)) {
                        RCSColumnsMap rcsColumnsMap = mAdapter.mRcsColumnsMap;
                        int messageClass = cursor.getInt(rcsColumnsMap.mColumnRcsMessageClass);
                        int messageType = cursor.getInt(rcsColumnsMap.mColumnRcsMessageType);
                        int direction = cursor.getInt(rcsColumnsMap.mColumnRcsMessageDirection);
//                        int status = cursor.getInt(rcsColumnsMap.mColumnRcsMessageStatus);
                        if (messageClass == RcsLog.Class.BURN) {
                            hasUnDownloadMsg = true;
                            continue;
                        } else if (messageType == RcsLog.MessageType.FT
                                && direction == RcsLog.Direction.INCOMING) {
                            // incoming ft
                            IpAttachMessage ftMessage =
                                    (IpAttachMessage) mRcsMessageManager.getRCSMessageInfo(msgId);
                            if (ftMessage.getRcsStatus() != Status.FINISHED) {
                                hasUnDownloadMsg = true;
                                continue;
                            }
                        }
                        rcsMsgContent rc = new rcsMsgContent(direction, messageType);
                        mRcsItem.put(msgId, rc);
                    }
                } while (cursor.moveToNext());
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        final String address = getStringContact(contacts);
        if (hasUnDownloadMsg) {
            showTipsDialog(address);
        } else {
            startFavorite(address);
        }
        return true;
    }

    private boolean doForward(Cursor cursor, HashSet<Long> smsKeys, HashSet<Long> mmsKeys,
            HashSet<Long> rcsKeys) {
      //forward single ft
        //only one rcsMessage
        int smsNumber = smsKeys == null ? 0 : smsKeys.size();
        int mmsNumber = mmsKeys == null ? 0 : mmsKeys.size();
        int rcsNumber = rcsKeys == null ? 0 : rcsKeys.size();
//        if ((smsNumber + rcsNumber + mmsNumber) > MAX_COUNT_OF_COMBINE_FORWARD) {
//            String toastString = mPluginContext.getString(R.string.forward_item_too_much);
//            Toast.makeText(mContext, toastString, Toast.LENGTH_LONG).show();
//            return true;
//        }
        if (smsNumber == 0 && mmsNumber == 0 && rcsNumber == 1) {
            //only one rcs message
            Long[] rcsKeysArray = rcsKeys.toArray(new Long[1]);
            long rcsId = RcsMessageListAdapter.getRcsMsgIdByKey(rcsKeysArray[0]);
            IpMessage message = mRcsMessageManager.getRCSMessageInfo(rcsId);
            if (message.getBurnedMessage()) {
                // burned message
                Toast.makeText(mContext,
                        mPluginContext.getString(R.string.toast_burned_messsage_forward),
                        Toast.LENGTH_SHORT).show();
                return true;
            }
            if (message instanceof IpAttachMessage) {
                IpAttachMessage attMessage = (IpAttachMessage) message;
                if ((attMessage.getStatus() == MessageStatus.READ
                        || attMessage.getStatus() == MessageStatus.UNREAD)
                    && attMessage.getRcsStatus() != Status.FINISHED) {
                    showFtDialog();
                } else {
                    if (RCSServiceManager.getInstance().serviceIsReady()) {
                        Intent forwardintent =
                            RcsMessageUtils.createForwordIntentFromIpmessage(mContext, attMessage);
                        mContext.startActivity(forwardintent);
                    } else {
                        Toast.makeText(mContext,
                                       mPluginContext.getString(R.string.toast_sms_unable_forward),
                                       Toast.LENGTH_SHORT).show();
                    }
                }
                return true;
            }
        }
        mFtCount = hasFTSelected(rcsKeys);
        Log.d(TAG, "thiss doForward mFtCount = " + mFtCount);
        if (mmsNumber > 0 && mFtCount <= 0) {
            return false;
        }
        if (mFtCount > 0) {
          //showdialog and call back
            showMmsFtDialog();
            return true;
        }
        return false;
    }

    private boolean doOneByOneForward(Cursor cursor, HashSet<Long> smsKeys, HashSet<Long> mmsKeys,
            HashSet<Long> rcsKeys) {

        final int smsNumber = smsKeys == null ? 0 : smsKeys.size();
        final int mmsNumber = mmsKeys == null ? 0 : mmsKeys.size();
        final int rcsNumber = rcsKeys == null ? 0 : rcsKeys.size();
        if ((smsNumber + rcsNumber + mmsNumber) > MAX_COUNT_OF_COMBINE_FORWARD) {
            String toastString = mPluginContext.getString(R.string.forward_item_too_much);
            Toast.makeText(mContext, toastString, Toast.LENGTH_LONG).show();
            return true;
        }
        if (mmsNumber > 0) {
            Toast.makeText(mContext,
                    mPluginContext.getString(R.string.forward_one_by_one_tips_title),
                    Toast.LENGTH_SHORT).show();
        }
        if (smsNumber + rcsNumber > 0) {
            final String rcsSelection = getStringId(rcsKeys);
            final String smsSelection = getStringId(smsKeys);
            new Thread(new Runnable() {
                public void run() {
                    ArrayList<ForwardSendData> dataList = new ArrayList<ForwardSendData>();
                    if (rcsSelection != null) {
                        String rcsWhere = "_id IN " + rcsSelection;
                        Log.d(TAG, "thiss doOneByOneForward rcsWhere = " + rcsWhere);
                        Cursor cursor = mContext.getContentResolver().query(
                             RcsLog.MessageColumn.CONTENT_URI, RCS_PROJECTION,
                             rcsWhere, null, null);
                        try {
                            if (cursor.moveToFirst()) {
                                do {
                                    String body = null;
                                    int messageType = cursor.getInt(2);
                                    if (messageType == RcsLog.MessageType.FT) {
                                        String path = cursor.getString(1);
                                        ForwardSendData data = null;
                                        if (path != null) {
                                            String ct = analysisFileType(path);
                                            if (ct != null) {
                                                if (ct.equals(Constants.CT_TYPE_VEDIO)) {
                                                    data = new ForwardSendData("video/mp4", path);
                                                } else if (ct.equals(Constants.CT_TYPE_AUDIO)) {
                                                    data = new ForwardSendData("video/mp4", path);
                                                } else if (ct.equals(Constants.CT_TYPE_IMAGE)) {
                                                    data = new ForwardSendData("image/jpeg", path);
                                                } else if (ct.equals(Constants.CT_TYPE_VCARD)) {
                                                    data = new ForwardSendData("text/x-vcard",
                                                                   path);
                                                } else if (ct.equals(Constants.
                                                            CT_TYPE_GEOLOCATION)) {
                                                    Log.d(TAG, "GeolocationData()");
                                                    data = new ForwardSendData("geo/*", path);
                                                }

                                                if (data != null) {
                                                    dataList.add(data);
                                                }
                                            }
                                        }
                                    } else if (messageType == RcsLog.MessageType.IM) {
                                        String imBody = cursor.getString(0);
                                        ForwardSendData sendData = new ForwardSendData(
                                                    "text/plain", imBody);
                                        if (sendData != null) {
                                            dataList.add(sendData);
                                        }
                                    }
                                } while (cursor.moveToNext());
                            }
                        } catch (Exception e) {
                            e.printStackTrace();
                        } finally {
                            cursor.close();
                        }
                    }
                    if (smsSelection != null) {
                        String smsWhere = "_id IN " + smsSelection;
                        Cursor c = mContext.getContentResolver().query(Sms.CONTENT_URI,
                                SMS_PROJECTION, smsWhere, null, null);
                        try {
                            if (c.moveToFirst()) {
                                do {
                                    String body = c.getString(0);
                                    ForwardSendData sendData = new ForwardSendData(
                                                 "text/plain", body);
                                    if (sendData != null) {
                                        dataList.add(sendData);
                                    }
                                } while (c.moveToNext());
                            }
                        } catch (Exception e) {
                            e.printStackTrace();
                        } finally {
                            c.close();
                        }
                    }
                    if (dataList.size() > 0) {
                        final ArrayList<ForwardSendData> forwardDataList = dataList;
                        mContext.runOnUiThread(new Runnable() {
                            public void run() {
                                Intent sendIntent = new Intent();
                                sendIntent.setAction(
                                "android.intent.action.ACTION_RCS_MESSAGING_SEND");
                                sendIntent.setType("multisend/forwardonebyone");
                                sendIntent.putParcelableArrayListExtra(
                                "android.intent.RCS_MULTI_SEND", forwardDataList);
                                mContext.startActivity(sendIntent);
                            }
                        });
                    } else {
                        mContext.runOnUiThread(new Runnable() {
                            public void run() {
                                Toast.makeText(mContext,
                                    mPluginContext.getString(R.string.toast_one_by_one_forward),
                                    Toast.LENGTH_SHORT).show();
                            }
                        });
                    }
                }
            }).start();
        } else {
            Toast.makeText(mContext,
                    mPluginContext.getString(R.string.toast_one_by_one_forward),
                    Toast.LENGTH_SHORT).show();
        }
        return true;
    }

    private String analysisFileType(String filePath) {
        if (filePath != null) {
            String mimeType = MediaFile.getMimeTypeForFile(filePath);
            if (mimeType == null) {
                mimeType = MimeTypeMap.getSingleton().getMimeTypeFromExtension(
                        RCSUtils.getFileExtension(filePath));
            }
            if (mimeType != null) {
                if (mimeType.contains(Constants.FILE_TYPE_IMAGE)) {
                    return Constants.CT_TYPE_IMAGE;
                } else if (mimeType.contains(Constants.FILE_TYPE_AUDIO)
                        || mimeType.contains("application/ogg")) {
                    return Constants.CT_TYPE_AUDIO;
                } else if (mimeType.contains(Constants.FILE_TYPE_VIDEO)) {
                    return Constants.CT_TYPE_VEDIO;
                } else if (filePath.toLowerCase().endsWith(".vcf")) {
                    return Constants.CT_TYPE_VCARD;
                } else if (filePath.toLowerCase().endsWith(".xml")) {
                    return Constants.CT_TYPE_GEOLOCATION;
                } else {
                    Log.d(TAG, "analysisFileType() other type add here!");
                }
            }
        } else {
            Log.w(TAG, "analysisFileType(), file name is null!");
        }
        return null;
    }

    private int hasFTSelected(HashSet<Long> rcsKeys) {
        int count = 0;
        String selection = combineRcsMessageSelection(rcsKeys);
        String[] projection = new String[] {RcsLog.MessageColumn.TYPE, RcsLog.MessageColumn.CLASS};
        Cursor cursor = mContext.getContentResolver().query(RcsLog.MessageColumn.CONTENT_URI,
                        projection, selection, null, null);
        try {
            if (cursor != null && cursor.moveToFirst()) {
                do {
                    int type = cursor.getInt(0);
                    if (type == RcsLog.MessageType.FT) {
                        count += 1;
                    }
                } while (cursor.moveToNext());
            }
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }
        Log.d(TAG, "thiss hasFTSelected count = " + count);
        return count;
    }

    public boolean needShowNoTextToast(int mSize) {
        Log.d(TAG, "thiss needShowNoTextToast mFtCount =" +
               mFtCount + " mSize =" + mSize);
        if ((mSize - mFtCount) <= 0) {
            Log.d(TAG, "needShowNoTextToast return true");
            return true;
        }
        return false;
    }

    public boolean needShowReachLimit() {
        if (RCSServiceManager.getInstance().serviceIsReady()) {
            return true;
        } else {
            return false;
        }
    }

    private void showMmsFtDialog() {
        Log.d(TAG, "thiss showMmsFtDialog");
        AlertDialog.Builder builder = new AlertDialog.Builder(mContext);
        builder.setTitle(mPluginContext.getString(R.string.discard_mmsft_title))
               .setIconAttribute(android.R.attr.alertDialogIcon)
               .setCancelable(true)
               .setMessage(mPluginContext.getString(R.string.discard_mmsft_content))
               .setPositiveButton(mPluginContext.getString(R.string.dialog_continue), new DialogInterface.OnClickListener() {
                   public final void onClick(DialogInterface dialog, int which) {
                       dialog.dismiss();
                       mCallback.onForwardActionItemClick();
                   }
               })
               .setNegativeButton(mPluginContext.getString(R.string.Cancel), new DialogInterface.OnClickListener() {
                   public void onClick(DialogInterface dialog, int which) {
                       dialog.dismiss();
                       if (mSmsItem != null) {
                           mSmsItem.clear();
                       }
                       if (mMmsItem != null) {
                           mMmsItem.clear();
                       }
                       if (mRcsItem != null) {
                           mRcsItem.clear();
                       }
                   }
               })//Cancel need to clear hashmap cache
               .show();
    }

    private void showFtDialog() {
        Log.d(TAG, "thiss showFtDialog");
        AlertDialog.Builder builder = new AlertDialog.Builder(mContext);
        builder.setTitle(mPluginContext.getString(R.string.forward_ft_title))
               .setIconAttribute(android.R.attr.alertDialogIcon)
               .setCancelable(true)
               .setMessage(mPluginContext.getString(R.string.forward_ft_body))
               .setPositiveButton(mPluginContext.getString(R.string.dialog_continue), new DialogInterface.OnClickListener() {
                   public final void onClick(DialogInterface dialog, int which) {
                       dialog.dismiss();
                       Toast.makeText(mContext, mPluginContext.getString(R.string.toast_sms_forward),
                               Toast.LENGTH_SHORT).show();
                   }
               })
               .setNegativeButton(mPluginContext.getString(R.string.Cancel), new DialogInterface.OnClickListener() {
                   public void onClick(DialogInterface dialog, int which) {
                       dialog.dismiss();
                       if (mSmsItem != null) {
                           mSmsItem.clear();
                       }
                       if (mMmsItem != null) {
                           mMmsItem.clear();
                       }
                       if (mRcsItem != null) {
                           mRcsItem.clear();
                       }
                   }
               })//Cancel need to clear hashmap cache
               .show();
    }

    private void startFavorite(final String addresses) {
        showProgressIndication();
        favoriteSmsSuccess = false;
        favoriteMmsSuccess = false;
        favoriteRcsSuccess = false;
        new Thread(new Runnable() {
            public void run() {
                favoriteSmsSuccess = setSmsFavorite(addresses);
                favoriteMmsSuccess = setMmsFavorite(addresses);
                favoriteRcsSuccess = setRcsFavorite(addresses);

                mContext.runOnUiThread(new Runnable() {
                    public void run() {
                        dismissProgressIndication();
                        if (favoriteSmsSuccess || favoriteMmsSuccess || favoriteRcsSuccess) {
                            Toast.makeText(mContext, mPluginContext.getString(
                                    R.string.toast_favorite_success), Toast.LENGTH_SHORT).show();
                        } else {
                            Toast.makeText(mContext, mPluginContext.getString(
                                    R.string.toast_sms_favorite), Toast.LENGTH_SHORT).show();
                        }
                    }
                });
            }
        }).start();
    }

    private void showProgressIndication() {
        if (mProgressDialog == null) {
            mProgressDialog = new ProgressDialog(mContext);
            mProgressDialog.setMessage(mPluginContext.getString(R.string.please_wait));
            mProgressDialog.setIndeterminate(true);
            mProgressDialog.setCancelable(false);
        }
        mProgressDialog.show();
    }

    private void dismissProgressIndication() {
        if (mProgressDialog != null && mProgressDialog.isShowing()) {
            mProgressDialog.dismiss();
        }
    }

    private Map<Long, smsBodyandAddress> mSmsItem;
    private Map<Long, mmsSubjectandType> mMmsItem;
    private Map<Long, rcsMsgContent> mRcsItem;
    class smsBodyandAddress {
        int boxId;
        String mBody;
        long mDate;
        public smsBodyandAddress(String mBody, int boxId, long mDate) {
            super();
            this.mBody = mBody;
            this.boxId = boxId;
            this.mDate = mDate;
        }
    }

    class mmsSubjectandType {
        int boxId;
        int mMessageType;
        String mSubject;
        int sub_cs;
        long mDate;
        public mmsSubjectandType(int boxId, int mMessageType, String mSubject, int sub_cs, long mDate) {
            super();
            this.boxId = boxId;
            this.mMessageType = mMessageType;
            this.mSubject = mSubject;
            this.sub_cs = sub_cs;
            this.mDate = mDate;
        }
    }

    class rcsMsgContent {
        int mMessageType;
        int direction;
        public rcsMsgContent(int direction, int mMessageType) {
            super();
            this.direction = direction;
            this.mMessageType = mMessageType;
        }
    }

    private String getStringId(HashSet<Long> selectIds) {
        String idSelect = null;
        if (selectIds != null && selectIds.size() > 0) {
            StringBuffer strBuf = new StringBuffer("(");
            for (long id : selectIds) {
                long newId = RcsMessageListAdapter.getRcsMsgIdByKey(id);
                strBuf.append(newId + ",");
            }
            String str = strBuf.toString();
            idSelect = str.substring(0, str.length() - 1);
            idSelect += ")";
        }
        return idSelect;
    }

    private String getStringContact(String Contacts[]) {
        String ContactsSelect = null;
        if (Contacts != null && Contacts.length > 0) {
            StringBuffer strBuf = new StringBuffer();
            for (String contact : Contacts) {
                strBuf.append(contact + ",");
            }
            String str = strBuf.toString();
            ContactsSelect = str.substring(0, str.length() - 1);
        }
        return ContactsSelect;
    }

    private HashSet<Long> collectIds(long Ids[]) {
        HashSet<Long> idSet = new HashSet<Long>();
        for (long id : Ids) {
            if (id > 0) {
                idSet.add(id);
            }
        }
        return idSet;
    }

//    private HashSet<Long> getSmsCollectIds(long keys[]) {
//        HashSet<Long> idSet = new HashSet<Long>();
//        if (keys != null) {
//            for (long key : keys) {
//                if (!RcsMessageListAdapter.isRcsKey(key)) {
//                    idSet.add(key);
//                }
//            }
//        }
//        return idSet;
//    }
//
//    private HashSet<Long> getRcsCollectIds(long keys[]) {
//        HashSet<Long> idSet = new HashSet<Long>();
//        if (keys != null) {
//            for (long key : keys) {
//                if (RcsMessageListAdapter.isRcsKey(key)) {
//                    idSet.add(RcsMessageListAdapter.getRcsMsgIdByKey(key));
//                }
//            }
//        }
//        return idSet;
//    }

    private void showTipsDialog(final String address) {
        AlertDialog.Builder builder = new AlertDialog.Builder(mContext);
        builder.setTitle(mPluginContext.getString(R.string.favorite_tips_title))
               .setIconAttribute(android.R.attr.alertDialogIcon)
               .setCancelable(true)
               .setPositiveButton(mPluginContext.getString(R.string.dialog_continue), new DialogInterface.OnClickListener() {
                   public final void onClick(DialogInterface dialog, int which) {
                       dialog.dismiss();
                       startFavorite(address);
                   }
               })
               .setNegativeButton(mPluginContext.getString(R.string.Cancel), new DialogInterface.OnClickListener() {
                   public void onClick(DialogInterface dialog, int which) {
                       dialog.dismiss();
                       if (mSmsItem != null) {
                           mSmsItem.clear();
                       }
                       if (mMmsItem != null) {
                           mMmsItem.clear();
                       }
                       if (mRcsItem != null) {
                           mRcsItem.clear();
                       }
                   }
               })//Cancel need to clear hashmap cache
               .setMessage(mPluginContext.getString(R.string.favorite_tips_body))
               .show();
    }

    private boolean setSmsFavorite(final String contact) {
        if (mSmsItem != null && mSmsItem.size() > 0) {
            Iterator<Entry<Long, smsBodyandAddress>> iter = mSmsItem.entrySet().iterator();
            while (iter.hasNext()) {
                 Map.Entry<Long, smsBodyandAddress> entry = iter.next();
                 long id = entry.getKey();
                 long date = entry.getValue().mDate;
                 ContentValues values = new ContentValues();
                 if (entry.getValue().boxId == 1) {
                     values.put(FavoriteMsgData.COLUMN_DA_DIRECTION, ChatMessage.INCOMING);
                 } else {
                     values.put(FavoriteMsgData.COLUMN_DA_DIRECTION, ChatMessage.OUTCOMING);
                 }

                 String mBody = entry.getValue().mBody;

                 //FLAG
                 int flag = 0;
                 if (mIsGroupChat) {
                     flag = ChatMessage.MTM;
                 } else if (contact.indexOf(",") > 0) {
                     flag = ChatMessage.OTM;
                 } else {
                     flag = ChatMessage.OTO;
                 }
                 values.put(FavoriteMsgData.COLUMN_DA_FLAG, flag);

                 //CONTACT_NUMBER
                 if (flag == ChatMessage.MTM) {
                     if (mChatId != null) {
                         values.put(FavoriteMsgData.COLUMN_DA_CONTACT, mChatId);
                     }
                 } else {
                     values.put(FavoriteMsgData.COLUMN_DA_CONTACT, contact);
                 }
                 values.put(FavoriteMsgData.COLUMN_DA_TIMESTAMP, date);
                 RcsMessageUtils.setSmsFavorite(mContext, mBody, id, values);
            }
            mSmsItem.clear();
        } else {
            return false;
        }
        return true;
    }

    private boolean setRcsFavorite(final String contact) {
        if (mRcsItem != null && mRcsItem.size() > 0) {
            Iterator<Entry<Long, rcsMsgContent>> iter = mRcsItem.entrySet().iterator();
            while (iter.hasNext()) {
                 Map.Entry<Long, rcsMsgContent> entry = iter.next();
                 long id = entry.getKey();
                 int mMessageType = entry.getValue().mMessageType;
                 int direction = entry.getValue().direction;
                 ContentValues values = new ContentValues();
                 values.put(FavoriteMsgData.COLUMN_DA_DIRECTION, direction);
                 //FLAG
                 int flag = 0;
                 if (mIsGroupChat) {
                     flag = ChatMessage.MTM;
                 } else if (contact.indexOf(",") > 0) {
                     flag = ChatMessage.OTM;
                 } else {
                     flag = ChatMessage.OTO;
                 }
                 values.put(FavoriteMsgData.COLUMN_DA_FLAG, flag);

                 IpMessage ipMessageForFavorite = mRcsMessageManager.getRCSMessageInfo(id);
                 values.put(FavoriteMsgData.COLUMN_DA_CONTACT, ipMessageForFavorite.getRemote());
                 if (mMessageType == RcsLog.MessageType.FT) {
                     Log.d(TAG, "thiss is FT = true");
                     RcsMessageUtils.setAttachIpmessageFavorite(mContext, ipMessageForFavorite,
                          mChatId, values);
                 } else if (mMessageType == RcsLog.MessageType.IM) {
                     Log.d(TAG, "thiss is IM = true");
                     RcsMessageUtils.setTextIpmessageFavorite(mContext, ipMessageForFavorite,
                          mChatId, values);
                 }
            }
            mRcsItem.clear();
        } else {
            return false;
        }
        return true;
    }

    private boolean setMmsFavorite(final String contacts) {
        if (mMmsItem != null && mMmsItem.size() > 0) {
            Iterator<Entry<Long, mmsSubjectandType>> iter = mMmsItem.entrySet().iterator();
            try {
                while (iter.hasNext()) {
                    Map.Entry<Long, mmsSubjectandType> entry = iter.next();
                    long id = entry.getKey();
                    int mBoxId = entry.getValue().boxId;
                    int type = entry.getValue().mMessageType;
                    String mSubject = entry.getValue().mSubject;
                    long date = entry.getValue().mDate;
                    if (!TextUtils.isEmpty(mSubject)) {
                        int charset = entry.getValue().sub_cs;
                        EncodedStringValue v = new EncodedStringValue(charset,
                                PduPersister.getBytes(mSubject));
                        mSubject = v.getString();
                    }
                    ContentValues values = new ContentValues();
                    int flag = 0;
                    if (contacts.indexOf(",") > 0) {
                         flag = ChatMessage.OTM;
                    } else {
                         flag = ChatMessage.OTO;
                    }
                    values.put(FavoriteMsgData.COLUMN_DA_TIMESTAMP, date);
                    values.put(FavoriteMsgData.COLUMN_DA_CONTACT, contacts);
                    values.put(FavoriteMsgData.COLUMN_DA_FLAG, flag);
                    RcsMessageUtils.setMmsFavorite(mContext, id, mSubject,
                          mBoxId, type, values);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        } else {
            return false;
        }
        return true;
    }

    public boolean ipForwardOneMms(Uri mUri) {
        Intent mIntent = RcsMessageUtils.createForwardIntentFromMms(mContext, mUri);
        if (mIntent != null) {
            mContext.startActivity(mIntent);
            mContext.finish();
            return true;
        }
        return false;
    }

    public boolean forwardIpMessage(String mBody) {
        if (TextUtils.isEmpty(mBody)) {
            mContext.runOnUiThread(new Runnable() {
                public void run() {
                    Toast.makeText(mContext, mPluginContext.getString(R.string.toast_sms_forward),
                            Toast.LENGTH_SHORT).show();
                }
            });
            return true;
        }
        Intent mIntent = RcsMessageUtils.createForwordIntentFromSms(mContext, mBody);
        if (mIntent != null) {
            mContext.startActivity(mIntent);
            mContext.finish();
            return true;
        }
        return false;
    }

    // forwardTextMessage in plugin
    public boolean forwardTextMessage(ArrayList<Long> smsList, int maxLength) {
        if (needShowNoTextToast(smsList.size())) {
            Log.d(TAG, "forwardTextMessage  needShowNoTextToast");
            mCallback.noSmsForward();
            return true;
        }

//        if ((smsList.size() - mFtCount) > MAX_COUNT_OF_COMBINE_FORWARD) {
//            mContext.runOnUiThread(new Runnable() {
//                public void run() {
//                    Toast.makeText(mContext, mPluginContext.getString(
//                            R.string.forward_item_too_much), Toast.LENGTH_LONG).show();
//                }
//            });
//            return true;
//        }

//        Collections.sort(smsList);

        Cursor cursor = mAdapter.getCursor();
        RCSColumnsMap column = mAdapter.mRcsColumnsMap;
        StringBuffer strbuf = new StringBuffer();
        String tempbuf = null;
        boolean reachLimitFlag = false;
        long ipId = 0;
        if (cursor.moveToFirst()) {
            do {
                 String type = cursor.getString(column.getColumnMsgType());
                 long msgId = cursor.getLong(column.getColumnMsgId());
                 long key = msgId;
                 if (type.equals("rcs")) {
                     key = RcsMessageListAdapter.getKey(type, key);
                 }
                 Log.d(TAG, "forwardTextMessage type = " + type + " ,key = " + key);
                 if (!smsList.contains(key)) {
                     continue;
                 }
                 if (type.equals("sms")) {
                     mCallback.formatSmsBody(msgId, strbuf);
                     strbuf.append("\n");
                 } else if (type.equals("rcs")) {
                     String body = null;
                     String address = null;
                     String formatedBody = null;
                     int messageClass = cursor.getInt(column.mColumnRcsMessageClass);
                     int messageType = cursor.getInt(column.mColumnRcsMessageType);
                     if (messageType == RcsLog.MessageType.FT) {
                         continue;
                     }
                     if (messageClass == RcsLog.Class.NORMAL) {
                         body = cursor.getString(column.mColumnRcsMessageBody);
                         address = cursor.getString(column.mColumnRcsMessageAddress);
                     }
                     int direction = cursor.getInt(column.mColumnRcsMessageDirection);
                     boolean isIncoming = direction == RcsLog.Direction.INCOMING;
                     if (!TextUtils.isEmpty(body)) {
                         if (isForwardWithSender() && !TextUtils.isEmpty(address)) {
                             Contact contact = Contact.get(address, false);
                             String number = Contact.formatNameAndNumber(contact.getName(),
                                     contact.getNumber(), "");
                             formatedBody = appendSenderInfoForForwardItem(body, number,
                                                             isIncoming);
                         } else {
                             formatedBody = body;
                         }
                         strbuf.append(formatedBody).append("\n");
                     }
                 }
                 if (strbuf.length() > maxLength && !needShowReachLimit()) {
                     reachLimitFlag = true;
                     /// M: fix bug ALPS02411446, when one message length  > maxLength @{
                     if (tempbuf == null) {
                         tempbuf = strbuf.toString();
                     }
                     /// M: fix bug ALPS00444391, remove the last "\n" when > maxLength @{
                     if (tempbuf != null && tempbuf.endsWith("\n")) {
                         tempbuf = tempbuf.substring(0, tempbuf.length() - 1);
                     }
                     /// @}
                     break;
                 } else {
                     tempbuf = strbuf.toString();
                 }
            } while (cursor.moveToNext());
        }
        if (tempbuf != null && tempbuf.endsWith("\n")) {
            tempbuf = tempbuf.substring(0, tempbuf.length() - 1);
        }


        if (reachLimitFlag) {
            final String contentbuf = tempbuf;
            mContext.runOnUiThread(new Runnable() {
                public void run() {
                    mCallback.showReachLimitDialog(contentbuf);
                }
            });
            return true;
        }

        if (!forwardIpMessage(tempbuf)) {
            mCallback.beginForward(tempbuf);
            return true;
        }
        return true;
    }

    private boolean isForwardWithSender() {
        SharedPreferences prefs = PreferenceManager.getDefaultSharedPreferences(mContext);
        boolean smsForwardWithSender = prefs.getBoolean(SMS_FORWARD_WITH_SENDER, true);
        Log.d(TAG, "isForwardWithSender(): SMS Forward With Sender ?= " + smsForwardWithSender);
        return smsForwardWithSender;
    }

    /**
     * Format a RCS text item for forwarding.
     * @param body text body
     * @param contact contact's number and name formatted string
     * @param isIncoming if the origin item is incoing message, it's true; else it's false
     * @return formated message
     */
    private String appendSenderInfoForForwardItem(String body, String contact, boolean isIncoming) {
        String formattedMessage = null;
        if (isIncoming) {
            formattedMessage = contact + ":\n" + body;
        } else {
            formattedMessage = mPluginContext.getString(R.string.messagelist_sender_self)
                                        + ":\n" + body;
        }
        return formattedMessage;
    }

    @Override
    public boolean startMsgListQuery(AsyncQueryHandler mQueryHandler, int token, Object cookie,
            Uri uri, String[]projection, String selection, String[] selectionArgs, String orderBy) {
        String[] rcsProjections = RcsMessageUtils.combineTwoStringArrays(projection,
                RcsMessageListAdapter.RCS_MESSAGE_PROJECTION_EXTENDS);
        Uri rcsUri = RcsConversation.getConversationUri(mThreadId).buildUpon()
                                         .appendQueryParameter("MultiDelete", "true").build();
        mQueryHandler.startQuery(token, cookie, rcsUri, rcsProjections, selection, selectionArgs,
                orderBy);
        return true;
    }

    public boolean onIpParseDeleteMsg(long key) {
        if (RcsMessageListAdapter.isRcsKey(key)) {
            if (mSelectedIpMessageIds.containsKey(key)) {
                return true;
            }
        }
        return false;
    }

    @Override
    public void initMessageList(IIpMessageListAdapterExt adapter) {
        mAdapter = (RcsMessageListAdapter) adapter;
    }

    @Override
    public long[][] getSelectedMsgIds(Map<Long, Boolean> selectMap, int size) {
        Iterator<Entry<Long, Boolean>> selectIter = selectMap.entrySet().iterator();
        long[][] selectMessageIds = new long[3][size];
        int smsIndex = 0;
        int mmsIndex = 0;
        int rcsIndex = 0;
        while (selectIter.hasNext()) {
            Map.Entry<Long, Boolean> entry = (Entry<Long, Boolean>) selectIter.next();
            if (entry.getValue()) {
                long key = entry.getKey();
                if (key > 0) {
                    if (RcsMessageListAdapter.isRcsKey(key)) {
                        selectMessageIds[2][rcsIndex++] = key;
                    } else {
                        selectMessageIds[0][smsIndex++] = key;
                    }
                } else {
                    selectMessageIds[1][mmsIndex++] = -entry.getKey();
                }
            }
        }
        return selectMessageIds;
    }

    @Override
    public void markAsLocked(final long[][] ids, final boolean lock) {
        // do nothing
        if (ids == null || ids.length < 3) {
            Log.e(TAG, "[markAsLocked] wrong: " + ids);
            return;
        }
        Uri uri = RcsLog.MessageColumn.CONTENT_URI;
        ContentValues values = new ContentValues();
        values.put(RcsLog.MessageColumn.LOCKED, lock ? 1 : 0);
        StringBuffer strBuf = new StringBuffer();
        long[] rcsIds = ids[2];
        for (long key : rcsIds) {
            long id = RcsMessageListAdapter.getRcsMsgIdByKey(key);
            strBuf.append(id + ",");
        }
        String str = strBuf.toString();
        String idSelect = str.substring(0, str.length() - 1);
        String args = RcsLog.MessageColumn.ID + " in (" + idSelect + ")";
        mContext.getContentResolver().update(uri, values, args, null);
    }
}
