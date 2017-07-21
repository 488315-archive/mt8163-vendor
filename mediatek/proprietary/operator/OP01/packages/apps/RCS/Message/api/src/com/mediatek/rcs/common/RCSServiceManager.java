package com.mediatek.rcs.common;

import java.util.HashSet;

import org.gsma.joyn.JoynServiceConfiguration;

import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.SharedPreferences;
import android.content.SharedPreferences.Editor;
import android.database.Cursor;
import android.os.IBinder;
import android.os.RemoteException;
import android.telephony.PhoneNumberUtils;
import android.telephony.SubscriptionManager;
import android.text.TextUtils;
import android.util.Log;

import com.mediatek.rcs.common.IFileSpamReportListener;
import com.mediatek.rcs.common.ISpamReportListener;
import com.mediatek.rcs.common.IpMessageConsts;
import com.mediatek.rcs.common.RCSMessageManager;
import com.mediatek.rcs.common.provider.GroupMemberData;
import com.mediatek.rcs.common.provider.GroupChatCache;
import com.mediatek.rcs.common.provider.GroupChatData;
import com.mediatek.rcs.common.provider.GroupChatUtils;
import com.mediatek.rcs.common.service.IRCSChatService;
import com.mediatek.rcs.common.service.IRCSChatServiceListener;
import com.mediatek.rcs.common.utils.ContextCacher;
import com.mediatek.rcs.common.utils.Logger;
import com.mediatek.rcs.common.utils.RCSUtils;

/**
 * RCSServiceManager is the main interface of Rcs Message APIs. It's a single instance. App should
 * call {@link #createManager(Context)} at first, then to get the instance by
 * {@link #getInstance()}. App can get rcs account number by
 *  {@link #getMyNumber()}; get rcs account's state by {@link #isServiceConfigured()},
 *  {@link #isServiceActivated()}, {@link #isServiceEnabled()}, {@link #serviceIsReady()}. This
 *  manager also provider RCS's configurations by {@link #getFileTransferMaxSize()},
 *  {@link #getGroupChatMaxParticipantsNumber()}.
 */
public class RCSServiceManager {

    private static final String TAG = "RCSServiceManager";

    private final String RCS_CONFIGURATION_CHANGED_ACTION
                                            = "com.orangelabs.rcs.CONFIGURATION_STATUS_TO_APP";
    private final String RCS_SERVICE_STATUS_CHANGED_ON_ACTION
                                            = "com.mediatek.intent.rcs.stack.LaunchService";
    private final String RCS_SERVICE_STATUS_CHANGED_OFF_ACTION
                                            = "com.mediatek.intent.rcs.stack.StopService";
    private final String RCS_SERVICE_NOTIFY_ACTION = "org.gsma.joyn.action.VIEW_SETTINGS";
    private final String RCS_SERVICE_REGISTRERAD_STATUS
                                          = "com.mediatek.rcs.message.REGISTRATION_STATUS_CHANGE";
//    public static final String RCS_SERVICE_MANAGER_READY
//                                                   = "com.mediatek.rcs.message.service.initiated";
    /**
     * The Intent's action that get group chat's list. Caller can start service who has this action
     * intent and package name of 'com.mediatek.rcs.message'.
     */
    public static final String RCS_SYNC_GROUP_CHATS = "com.mediatek.rcs.messageservice.SYNC_CHATS";

    /**
     * The Intent's action that getting group chat's list done.
     * After get group chat's list, service will send broadcast that has this action.
     */
    public static final String RCS_SYNC_GROUP_CHATS_DONE
                                            = "com.mediatek.rcs.message.SYNC_CHATS_DONE";

    /**
     * When rcs service's states changed, will send broadcast that has this action. The states
     * contain configuration state {@link #isServiceActivated()},
     * activated state {@link #isServiceActivated()}, and registered state
     * {@link #serviceIsReady()}.
     */
    public static final String RCS_SERVICE_STATE_ACTION = "com.mediatek.rcs.message.SERVICE_STATE";
    /**
     * The boolean key of Intent to get service's configured state. When receive the broadcast of
     * {@link #RCS_SERVICE_STATE_ACTION}, can get the configured state through the
     * Intent.getBooleanExtra(key) by this key.
     */
    public static final String RCS_SERVICE_STATE_CONFIGURED = "configured";
    /**
     * The boolean key of Intent to get service's activate state. It's like with
     * {@link #RCS_SERVICE_STATE_CONFIGURED}.
     */
    public static final String RCS_SERVICE_STATE_ACTIVATED = "activated";
    /**
     * The boolean key of Intent to get service's registered state. It's like with
     * {@link #RCS_SERVICE_STATE_CONFIGURED}
     */
    public static final String RCS_SERVICE_STATE_REGISTERED = "registered";

    private final static int RCS_CORE_LOADED = 0;
    private final static int RCS_CORE_STARTED = 2;
    private final static int RCS_CORE_STOPPED = 3;
    private final static int RCS_CORE_IMS_CONNECTED = 4;
    private final static int RCS_CORE_IMS_DISCONNECTED = 9;

    private boolean mServiceActivated = false; //whether service is on or off
    private boolean mServiceConfigured = false;//whether configuration completed
    private boolean mServiceRegistered = false; // whether service registered
    private String mNumber = "";

    private static RCSServiceManager sInstance = null;
    private IRCSChatService mChatService = null;
    private Context mContext = null;
    private IRCSChatServiceListener mListener = null;
    private RCSMessageManager mRcsMessageManager;
    private GroupManager mGroupManager;

    /**
     * The value of {@link #getServiceState()} to indicate the RCSServiceNamager is connecting
     * Rcs Message Service.
     */
    public static final int RCS_SERVICE_STATE_CONNECTING = 1;
    /**
     * The value of {@link #getServiceState()} to indicate the RCSServiceNamager has connected to
     * Rcs Message Service.
     */
    public static final int RCS_SERVICE_STATE_CONNECTTED = 2;
    /**
     * The value of {@link #getServiceState()} to indicate the RCSServiceNamager has disconnected
     * from Rcs Message Service.
     */
    public static final int RCS_SERVICE_STATE_DISCONNECTTED = 3;
    private int mState;

    private HashSet<INotifyListener> mNotifyListeners = new HashSet<INotifyListener>();

    private HashSet<IBurnMessageCapabilityListener> mBurnMsgCapListeners =
                                                new HashSet<IBurnMessageCapabilityListener>();

    private HashSet<ISpamReportListener> mSpamReportListeners =
        new HashSet<ISpamReportListener>();
    private HashSet<IFileSpamReportListener> mFileSpamReportListeners =
        new HashSet<IFileSpamReportListener>();

    private HashSet<OnServiceChangedListener> mServiceChangedListeners =
            new HashSet<RCSServiceManager.OnServiceChangedListener>();

    /**
     * A Callback interface for listening of one Contact's capability result after querying.
     *
     */
    public interface IBurnMessageCapabilityListener {
        /**
         * Callback that return the contact's capability result after finish querying.
         * @param contact Contact's number
         * @param result The contact has burned message's capability if the result is true.
         */
        void onRequestBurnMessageCapabilityResult(String contact, boolean result);
    }

    private RCSServiceManager(Context context) {
        Logger.d(TAG, "call constructor");
        mContext = context;
        mGroupManager = GroupManager.createInstance(mContext);
        mRcsMessageManager = RCSMessageManager.createInstance(mContext);
        bindRemoteService();
        mServiceActivated = JoynServiceConfiguration.isServiceActivated(mContext);
        mNumber = getMyAccountNumber();
        mServiceConfigured = getServiceConfigurationState();
        IntentFilter filter = new IntentFilter(RCS_CONFIGURATION_CHANGED_ACTION);
        filter.addAction(RCS_SERVICE_STATUS_CHANGED_ON_ACTION);
        filter.addAction(RCS_SERVICE_STATUS_CHANGED_OFF_ACTION);
        filter.addAction(RCS_SERVICE_NOTIFY_ACTION);
        filter.addAction(RCS_SERVICE_REGISTRERAD_STATUS);
        context.registerReceiver(mRecever, filter);
        broadcastConfigurationChanged();
    }

    private ServiceConnection mConnection = new ServiceConnection() {

        public void onServiceConnected(ComponentName name, IBinder service) {
            Logger.d(TAG, "service connect!!");
            mChatService = IRCSChatService.Stub.asInterface(service);
            mListener = new RCSChatServiceListener(RCSServiceManager.this);
            try {
                mChatService.addRCSChatServiceListener(mListener);
                mRcsMessageManager.deleteLastBurnedMessage();
                updateServiceRegistrationState();
            } catch (RemoteException e) {
                e.printStackTrace();
            }
            mState = RCS_SERVICE_STATE_CONNECTTED;
            notifyServiceStateChanged();
            updateRcsProvisioningData();
        }

        public void onServiceDisconnected(ComponentName name) {
            Logger.d(TAG, "service disconnect!!");
            mChatService = null;
            bindRemoteService();
        }
    };

    private BroadcastReceiver mRecever = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            Log.d(TAG, "mRecever: " + action);
            if (action == null) {
                return;
            }
            if (action.equals(RCS_CONFIGURATION_CHANGED_ACTION)) {
                updateRcsProvisioningData();
                updateServiceConfigurationState();
                asyncGroupChat();
            } else if (action.equals(RCS_SERVICE_STATUS_CHANGED_ON_ACTION)) {
                setServiceActivated(true);
            } else if (action.equals(RCS_SERVICE_STATUS_CHANGED_OFF_ACTION)) {
                setServiceActivated(false);
            } else if (action.equals(RCS_SERVICE_NOTIFY_ACTION)) {
                int state = intent.getIntExtra("label_enum", -1);
                Logger.d(TAG, "state = " + state);
                if (state == RCS_CORE_LOADED || state == RCS_CORE_STARTED ||
                        state == RCS_CORE_STOPPED ||
                        state == RCS_CORE_IMS_CONNECTED ||state == RCS_CORE_IMS_DISCONNECTED) {
                    updateServiceState();
                }
                if (state == RCS_CORE_LOADED) {
                    // after stack loaded, update provisioning data.
                    updateRcsProvisioningData();
                }
            } else if (action.equals(RCS_SERVICE_REGISTRERAD_STATUS)) {
                boolean registered = intent.getBooleanExtra("status", false);
                setServiceRegistrationState(registered);
                asyncUpdateAccountNumber(registered);
                updateRcsProvisioningData();
            }
        }
    };

    /**
     * Get Message Manager, the only one way to get {@link RCSMessageManager} instance.
     * @return {@link RCSMessageManager}
     */
    public RCSMessageManager getRcsMessageManager() {
        return mRcsMessageManager;
    }

    /**
     * Get Group manager, the only one way to get {@link GroupManager} instance.
     * @return {@link GroupManager}
     */
    public GroupManager getGroupManager() {
        return mGroupManager;
    }

    /**
     * A callback to listen one or some result which is composed by one Intent.
     *
     */
    public interface INotifyListener {
        void notificationsReceived(Intent intent);
    }
    /**
     * Get service Listeners.
     * @return IRCSChatServiceListener
     */
    public IRCSChatServiceListener getServiceListener() {
        return mListener;
    }

    /**
     * Regist notify listener.
     * @param listener INotifyListener
     */
    public void registNotifyListener(INotifyListener listener) {
        mNotifyListeners.add(listener);
    }

    /**
     * UnregistNotifyListener.
     * @param listener INotifyListener
     */
    public void unregistNotifyListener(INotifyListener listener) {
        mNotifyListeners.remove(listener);
    }

    /**
     * Register one listener to listen contact's burned message's capability. Caller should call
     * {@link #unregistBurnMsgCapListener(IBurnMessageCapabilityListener)} to stop listening
     * contact's burned message capability.
     * @param listener IBurnMessageCapabilityListener
     */
    public void registBurnMsgCapListener(IBurnMessageCapabilityListener listener) {
        mBurnMsgCapListeners.add(listener);
    }

    /**
     * Unregister burned message capability listener if don't use.
     * @param listener IBurnMessageCapabilityListener
     */
    public void unregistBurnMsgCapListener(IBurnMessageCapabilityListener listener) {
        mBurnMsgCapListeners.remove(listener);
    }

    /**
     * callNotifyListeners.
     * @param intent Intent
     */
    protected void callNotifyListeners(Intent intent) {
        Logger.d(TAG, "callNotifyListeners, action=" + intent.getAction());
        for (INotifyListener listener : mNotifyListeners) {
            listener.notificationsReceived(intent);
        }
    }

    /**
     * callBurnCapListener.
     * @param contact
     * @param result
     */
    protected void callBurnCapListener(String contact, boolean result) {
        for (IBurnMessageCapabilityListener listener : mBurnMsgCapListeners) {
            listener.onRequestBurnMessageCapabilityResult(contact, result);
        }
    }

    /**
     * Service is ready to do RCS action. Caller can listen the registered state by
     * {@link #addOnServiceChangedListener(OnServiceChangedListener)}
     * @return  return true if service is registered or return false.
     */
    public boolean serviceIsReady() {
        Log.d(TAG, "serviceIsReady() entry: " + mServiceRegistered);
        return mServiceRegistered;
    }

    /**
     * Get self account number, if not configured, return "".
     * @return return self account number. it will return "" if not configured. The configured state
     * can get by {@link #isServiceConfigured()} and can listen the configured state by
     * {@link #addOnServiceChangedListener(OnServiceChangedListener)}.
     */
    public String getMyNumber() {
        Logger.d(TAG, "myNumber = " + mNumber);
        return mNumber;
    }

    private String getMyAccountNumber() {
        String publicUri = new JoynServiceConfiguration().getPublicUri(mContext);
        String number = "";
        if (!TextUtils.isEmpty(publicUri)) {
            if (publicUri.indexOf('@') != -1) {
                number = publicUri.substring(publicUri.indexOf(':') + 1, publicUri.indexOf('@'));
            } else {
                number = publicUri.substring(publicUri.indexOf(':') + 1);
            }
        }
        Logger.d(TAG, "getMyAccountNumber = " + number);
        return number;
    }

    private void asyncUpdateAccountNumber(boolean registered) {
        if (registered) {
            new Thread(new Runnable() {
                @Override
                public void run() {
                    mNumber = getMyAccountNumber();
                }
            }).start();
        }
    }

    private void updateServiceActivatedState() {
        boolean activate = JoynServiceConfiguration.isServiceActivated(mContext);;
        if (mServiceActivated != activate) {
            mServiceActivated = activate;
            notifyServiceStateChanged();
        }
    }

    private void updateServiceConfigurationState() {
        boolean configured = getServiceConfigurationState();
        if (mServiceConfigured != configured) {
            mServiceConfigured = configured;
            notifyServiceStateChanged();
        }
    }

    private boolean getServiceConfigurationState() {
        return new JoynServiceConfiguration().getConfigurationState(mContext);
    }

    private void updateServiceRegistrationState() {
        boolean registered = false;
        try {
            if (mChatService != null) {
                registered = mChatService.getRegistrationStatus();
            }
            setServiceRegistrationState(registered);
        } catch (RemoteException e) {
            e.printStackTrace();
            Logger.d(TAG, "updateServiceRegistrationState: e = " + e);
        }
    }

    private void updateServiceState() {
        boolean needNotify = false;
        boolean activated = JoynServiceConfiguration.isServiceActivated(mContext);
        if (mServiceActivated != activated) {
            mServiceActivated = activated;
            needNotify = true;
        }

        boolean configured = getServiceConfigurationState();
        if (mServiceConfigured != configured) {
            mServiceConfigured = configured;
            needNotify = true;
        }

        try {
            if (mChatService != null) {
                boolean registed = mChatService.getRegistrationStatus();
                if (mServiceRegistered != registed) {
                    mServiceRegistered = registed;
                    needNotify = true;
                }
                //TODO: temp process, should remove the code after auto configuration @{
                if (mServiceRegistered == true) {
                    if (mServiceConfigured != true) {
                        mServiceConfigured = true;
                        needNotify = true;
                    }
                }
                // @}
            }
        } catch (RemoteException e) {
            e.printStackTrace();
            Logger.d(TAG, "updateServiceState: e = " + e);
        }
        Log.d(TAG, "updateServiceState: needNotify = " + needNotify);
        if (needNotify) {
            notifyServiceStateChanged();
        }
    }

    /**
     * Whether service is enabled. The enable state equals {@link #isServiceActivated()}
     * && {@link #isServiceConfigured()}.
     * @return return true if service is activated and configured.
     */
    public boolean isServiceEnabled() {
        return (mServiceConfigured && mServiceActivated);
    }

    private void bindRemoteService() {
        Logger.d(TAG, "bindRemoteService");
        Intent intent = new Intent("com.mediatek.rcs.messageservice.chat.RCSMessageChatService");
        intent.setPackage("com.mediatek.rcs.messageservice");
        mContext.startService(intent);
        mContext.bindService(intent, mConnection, Context.BIND_AUTO_CREATE);
        mState = RCS_SERVICE_STATE_CONNECTING;
        notifyServiceStateChanged();
    }

    private void asyncGroupChat() {
        new Thread(new Runnable() {

            @Override
            public void run() {
                syncGroupChat();
            }

        }).start();
    }

    private void syncGroupChat() {
        Logger.d(TAG, "syncGroupChat start");
        GroupChatCache cache = GroupChatCache.getInstance();
        if (cache == null) {
            GroupChatCache.createInstance(ContextCacher.getPluginContext());
            cache = GroupChatCache.getInstance();
        }
        mNumber = getMyAccountNumber();
        ContentResolver resolver = ContextCacher.getPluginContext().getContentResolver();
        int subId = RCSUtils.getRCSSubId();
        String threadSelection =
                "(status<>" + subId + " AND status>0) " +
                " OR " +
                "(status=" + IpMessageConsts.GroupActionList.GROUP_STATUS_INVITING +
                        " AND sub_id<>" + subId + ")";
        Cursor cursor = resolver.query(GroupChatData.CONTENT_URI,
                               GroupChatUtils.GROUP_CHAT_PROJECTION, threadSelection, null, null);
        try {
            while (cursor.moveToNext()) {
                int status = cursor.getInt(cursor.getColumnIndex(GroupChatData.KEY_STATUS));
                String chatId = cursor.getString(cursor.getColumnIndex(GroupChatData.KEY_CHAT_ID));
                int threadSubId = cursor.getInt(cursor.getColumnIndex(GroupChatData.KEY_SUB_ID));
                if (status == IpMessageConsts.GroupActionList.GROUP_STATUS_INVITING
                        && threadSubId != subId) {
                    GroupChatCache.getInstance().updateStatusByChatId(
                            chatId, IpMessageConsts.GroupActionList.GROUP_STATUS_INVALID);
                    continue;
                }
                String selection = GroupMemberData.COLUMN_CHAT_ID + "='" + chatId + "'";
                Cursor memberCursor = resolver.query(GroupMemberData.CONTENT_URI,
                        RCSUtils.PROJECTION_GROUP_MEMBER, selection, null, null);
                try {
                    boolean validGroup = false;
                    while (memberCursor.moveToNext()) {
                        String number = memberCursor.getString(
                                memberCursor.getColumnIndex(GroupMemberData.COLUMN_CONTACT_NUMBER));
                        if (PhoneNumberUtils.compare(number, mNumber)) {
                            GroupChatCache.getInstance().updateStatusByChatId(chatId, subId);
                            validGroup = true;
                            break;
                        }
                    }
                    if (!validGroup) {
                        GroupChatCache.getInstance().updateStatusByChatId(chatId,
                                IpMessageConsts.GroupActionList.GROUP_STATUS_INVALID);
                    }
                } finally {
                    if (memberCursor != null) {
                        memberCursor.close();
                    }
                }
            }
        } finally {
            if (cursor != null) {
                cursor.close();
            }
        }
        Logger.d(TAG, "syncGroupChat end");
    }

    private void setServiceActivated(boolean activated) {
        Logger.d(TAG, "setServiceActivated: " + activated);
        if (mServiceActivated != activated) {
            mServiceActivated = activated;
            notifyServiceStateChanged();
        }
    }

    /**
     * Get RCSServiceManager's state.
     * @return The return value is one of the follows:
     * {@link #RCS_SERVICE_STATE_CONNECTING}, {@link #RCS_SERVICE_STATE_CONNECTTED} and
     * {@link #RCS_SERVICE_STATE_DISCONNECTTED}
     */
    public int getServiceState() {
        return mState;
    }

    /**
     * Whether service is turn on.
     * @return Return true if service is on, return false if service is off. User can turn on/off
     * the RCS service in Settings.
     */
    public boolean isServiceActivated() {
        return mServiceActivated;
    }

    private void setServiceRegistrationState(boolean registered) {
        Logger.d(TAG, "setServiceRegistrationState: " + registered);
        if (mServiceRegistered != registered) {
            mServiceRegistered = registered;
            if (mState == RCS_SERVICE_STATE_CONNECTTED) {
                //only service binded to notify registration state
                notifyServiceStateChanged();
            }
        }
    }

    private void notifyServiceStateChanged() {
        Logger.d(TAG, "notifyServiceStateChanged: mServiceActivated = " + mServiceActivated +
                ", mServiceConfigured = " + mServiceConfigured +
                ", mServiceRegistered = " + mServiceRegistered);
        for (OnServiceChangedListener l : mServiceChangedListeners) {
            l.onServiceStateChanged(mState, mServiceActivated,
                    mServiceConfigured, mServiceRegistered);
        }
        broadcastConfigurationChanged();
    }

    /**
     *  Is Service configured.
     * @return return true is configuration successful or turn false.
     */
    public boolean isServiceConfigured() {
        return mServiceConfigured;
    }
/*
    public boolean isFeatureSupported(int featureId) {
        Log.d(TAG, "isFeatureSupported() featureId is " + featureId);
        switch (featureId) {
            case FeatureId.FILE_TRANSACTION:
            case FeatureId.EXTEND_GROUP_CHAT:
            case FeatureId.GROUP_MESSAGE:
                return true;
            case FeatureId.PARSE_EMO_WITHOUT_ACTIVATE:
            case FeatureId.CHAT_SETTINGS:
            case FeatureId.ACTIVITION:
            case FeatureId.ACTIVITION_WIZARD:
            case FeatureId.MEDIA_DETAIL:
            case FeatureId.TERM:
             case FeatureId.EXPORT_CHAT:
            case FeatureId.APP_SETTINGS:
                  return false;
            default:
                return false;
        }
    }
*/
    /**
     * CreateManager instance. App should call {@link #createManager(Context)} to create instance
     * before {@link #getInstance()} and other functions.
     * @param context Context
     */
    public static void createManager(Context context) {
        Logger.d(TAG, "createManager, entry");
        if (sInstance == null) {
            sInstance = new RCSServiceManager(context);
        }
    }

    /**
     * Get instance. Must call {@link #createManager(Context)} before call this function.
     * @return return {@link #RCSServiceManager}.
     */
    public static RCSServiceManager getInstance() {
        if (sInstance == null) {
            throw new RuntimeException("need call createManager to create instance");
        }
        return sInstance;
    }

    /**
     * Get chat Service.
     * This function should not be called in main thread,
     * or it will block onServiceConnected
     * @return IRCSChatService
     */
    public IRCSChatService getChatService() {
        int i = 0;
        while (mChatService == null && i++ < 3) {
            try {
                Thread.sleep(500);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        return mChatService;
    }

    /**
     * Get the contact's burned message' capability. The querying is a async flow. Caller should
     * call {@link #registBurnMsgCapListener(IBurnMessageCapabilityListener)} to listen the result;
     * Caller should also call {@link #unregistBurnMsgCapListener(IBurnMessageCapabilityListener)}
     * If not want to listen the contact's capability.
     * @param contact The contact number to query
     * @return The return value is not the burned message's capability, if return true, caller can
     * call {@link #registBurnMsgCapListener(IBurnMessageCapabilityListener)} to wait the query
     * result.
     */
    public boolean getBurnMsgCap(String contact) {
        Logger.d(TAG, "getBurnMsgCap for " + contact);
        boolean cap = false;
        try {
            if (mChatService != null) {
                mChatService.getBurnMessageCapability(contact);
            }
        } catch (RemoteException e) {
            e.printStackTrace();
        }
        return cap;
    }

    @Override
    public void finalize() {
        try {
            Logger.d(TAG, "finalize()!!");
            mContext.unbindService(mConnection);
            mContext.unregisterReceiver(mRecever);
            super.finalize();
        } catch (Throwable e) {
            e.printStackTrace();
        }
    }

    /**
     * A callback that Listen RCS service state changed. App can add listener by #
     *
     */
    public interface OnServiceChangedListener {
        /**
         *
         * @param state Rcs Service Manager's state. It's one the the values:
         * {@link #RCS_SERVICE_STATE_CONNECTING}, {@link #RCS_SERVICE_STATE_CONNECTTED} and
         * {@link #RCS_SERVICE_STATE_DISCONNECTTED}.
         * @param activated  whether service is turn on, true is on, false is off
         * @param configured whether service is configured, true is configured
         * @param registered whether service is registered
         */
        public void onServiceStateChanged(int state, boolean activated,
                boolean configured, boolean registered);
    }

    /**
     * Add service changed listener. If not use more, please remove the listener by
     * {@link #removeOnServiceChangedListener(OnServiceChangedListener)}
     * @param l {@link RCSServiceManager.OnServiceChangedListener}
     * @return  return true if add successfully or return false
     */
    public boolean addOnServiceChangedListener(OnServiceChangedListener l) {
        return mServiceChangedListeners.add(l);
    }

    /**
     * Remove service changed listener.
     * @param l the instance of {@link RCSServiceManager.OnServiceChangedListener}
     * @return  return true if remove successfully or return false
     */
    public boolean removeOnServiceChangedListener(OnServiceChangedListener l) {
        return mServiceChangedListeners.remove(l);
    }

    private void broadcastConfigurationChanged() {
        Intent intent = new Intent(RCS_SERVICE_STATE_ACTION);
        intent.putExtra(RCS_SERVICE_STATE_ACTIVATED, mServiceActivated);
        intent.putExtra(RCS_SERVICE_STATE_CONFIGURED, mServiceConfigured);
        intent.putExtra(RCS_SERVICE_STATE_REGISTERED, mServiceRegistered);
        mContext.sendBroadcast(intent);
    }

    /**
     * register Spam Report Listener.
     * @param listener of {@link ISpamReportListener}
     */
    public void registSpamReportListener(ISpamReportListener listener) {
        Log.d(TAG, "[spam-report] registSpamReportListener listener: "+listener  );
        mSpamReportListeners.add(listener);
    }

    /**
     * unregister Spam Report Listener.
     * @param listener of {@link ISpamReportListener}
     */
    public void unregistSpamReportListener(ISpamReportListener listener) {
        Log.d(TAG, "[spam-report] unregistSpamReportListener listener: "+listener  );
        mSpamReportListeners.remove(listener);
    }

    /**
     * handleSpamReportResult.
     * @param contact
     * @param result
     */
    public void handleSpamReportResult(String contact, String msgId, int errorcode) {
        Log.d(TAG, "[spam-report] handleSpamReportResult contact: "+contact+
                " msgId: "+msgId+ " errorcode: "+errorcode  );
        for (ISpamReportListener listener : mSpamReportListeners) {
            listener.onSpamReportResult(contact, msgId, errorcode);
        }
    }

    /**
     * registFileSpamReportListener.
     * @param listener {@link IFileSpamReportListener}
     */
    public void registFileSpamReportListener(IFileSpamReportListener listener) {
        Log.d(TAG, "[spam-report] registFileSpamReportListener listener: "+listener  );
        mFileSpamReportListeners.add(listener);
    }

    /**
     * unregistFileSpamReportListener.
     * @param listener {@link IFileSpamReportListener}
     */
    public void unregistFileSpamReportListener(IFileSpamReportListener listener) {
        Log.d(TAG, "[spam-report] unregistFileSpamReportListener listener: "+listener  );
        mFileSpamReportListeners.remove(listener);
    }

    /**
     * handleFileSpamReportResult.
     * @param contact
     * @param result
     */
    public void handleFileSpamReportResult(String contact, String msgId, int errorcode) {
        Log.d(TAG, " [spam-report] handleFileSpamReportResult: #" + contact +
                " ,#" + msgId + " ,#" + errorcode);
        for (IFileSpamReportListener listener : mFileSpamReportListeners) {
            listener.onFileSpamReportResult(contact, msgId, errorcode);
        }
    }

    /**
     * Get the max participants number of Group chat.
     * @return the max participants number of Group chat, return 0 if error happens.
     */
     public int getGroupChatMaxParticipantsNumber() {
         int number = 0;
         int subId = RCSUtils.getRCSSubId();
         Logger.d(TAG, "[getGroupChatMaxParticipantsNumber] subId = " + subId);
         if (SubscriptionManager.isValidSubscriptionId(subId)) {
             SharedPreferences sp = mContext.getSharedPreferences(SP_CONFIGRATION_DATA,
                     Context.MODE_WORLD_READABLE);
             number = sp.getInt(KEY_GROUPCHAT_MAX_PARTICIPANT_NUMBER + subId, 0);
         }
         Logger.d(TAG, "[getGroupChatMaxParticipantsNumber] = " + number);
         return number;
     }

     /**
      * Get file transfer max size.
      * @return the max size of file transfer.
      */
     public long getFileTransferMaxSize() {
         long maxSize = 0;
         int subId = RCSUtils.getRCSSubId();
         Logger.d(TAG, "[getFileTransferMaxSize] subId = " + subId);
         if (SubscriptionManager.isValidSubscriptionId(subId)) {
             SharedPreferences sp = mContext.getSharedPreferences(SP_CONFIGRATION_DATA,
                     Context.MODE_WORLD_READABLE);
             maxSize = sp.getLong(KEY_FILETRANSFER_MAX_SIZE + subId, 0);
         }
         Logger.d(TAG, "[getFileTransferMaxSize] = " + maxSize);
         return maxSize;
     }

     private static final String SP_CONFIGRATION_DATA = "RCSCONFIG";
     private static final String KEY_GROUPCHAT_MAX_PARTICIPANT_NUMBER =
                                                     "groupChatMaxParcipantsNumber_";
     private static final String KEY_FILETRANSFER_MAX_SIZE = "fileTransferMaxSize_";
     private void updateRcsProvisioningData() {
         int groupChatMaxParcipantsNumber = 0;
         long fileTransferMaxSize = 0;
         if (mChatService == null) {
             Logger.d(TAG, "[updateRcsProvisioningData] chatService is null");
             return;
         }
         int subId = RCSUtils.getRCSSubId();
         if (!SubscriptionManager.isValidSubscriptionId(subId)) {
             Logger.d(TAG, "[updateRcsProvisioningData] subId is invalid: " + subId);
             return;
         }
         try {
             groupChatMaxParcipantsNumber = mChatService.getGroupChatMaxParticipantsNumber();
             fileTransferMaxSize = mChatService.getRcsFileTransferMaxSize();
         } catch (RemoteException e) {
             // TODO: handle exception
             e.printStackTrace();
         }
         Logger.d(TAG, "[updateRcsProvisioningData]:groupChatMaxParcipantsNumber = " +
         groupChatMaxParcipantsNumber + ", fileTransferMaxSize=" + fileTransferMaxSize);
         if (groupChatMaxParcipantsNumber == 0 && fileTransferMaxSize == 0) {
             return;
         }
         SharedPreferences sp = mContext.getSharedPreferences(SP_CONFIGRATION_DATA,
                 Context.MODE_WORLD_READABLE | Context.MODE_WORLD_WRITEABLE);
         Editor editor = sp.edit();
         if (groupChatMaxParcipantsNumber > 0) {
             editor.putInt(KEY_GROUPCHAT_MAX_PARTICIPANT_NUMBER + subId,
                                                 groupChatMaxParcipantsNumber);
         }
         if (fileTransferMaxSize > 0) {
             editor.putLong(KEY_FILETRANSFER_MAX_SIZE + subId, fileTransferMaxSize);
         }
         editor.commit();
     }
}
