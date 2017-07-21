package com.orangelabs.rcs.service.api;

import java.io.ByteArrayInputStream;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.List;
import java.util.Vector;

import org.gsma.joyn.Build;
import org.gsma.joyn.IJoynServiceRegistrationListener;
import org.gsma.joyn.JoynServiceException;
import org.gsma.joyn.chat.Chat;
import org.gsma.joyn.chat.ChatIntent;
import org.gsma.joyn.chat.ChatLog;
import org.gsma.joyn.chat.ChatMessage;
import org.gsma.joyn.chat.ChatServiceConfiguration;
import org.gsma.joyn.chat.Geoloc;
import org.gsma.joyn.chat.GroupChat;
import org.gsma.joyn.chat.GroupChatIntent;
import org.gsma.joyn.chat.IChat;
import org.gsma.joyn.chat.IExtendChat;
import org.gsma.joyn.chat.IChatListener;
import org.gsma.joyn.chat.IExtendChatListener;
import org.gsma.joyn.chat.IChatService;
import org.gsma.joyn.chat.IGroupChat;
import org.gsma.joyn.chat.IGroupChatListener;
import org.gsma.joyn.chat.IGroupChatSyncingListener;
import org.gsma.joyn.chat.INewChatListener;
import org.gsma.joyn.chat.ConferenceEventData;
import org.gsma.joyn.chat.ExtendMessage;
import org.gsma.joyn.chat.ConferenceEventData.ConferenceUser;
import org.xml.sax.InputSource;

import android.text.TextUtils;
import android.content.Intent;
import android.os.IBinder;
import android.os.RemoteCallbackList;

import com.orangelabs.rcs.core.Core;
import com.orangelabs.rcs.core.ims.ImsModule;
import com.orangelabs.rcs.core.ims.service.ImsServiceSession;
import com.orangelabs.rcs.core.ims.service.SubscribeRequest;
import com.orangelabs.rcs.core.ims.service.SubscriptionManager;
import com.orangelabs.rcs.core.ims.service.im.InstantMessagingService;
import com.orangelabs.rcs.core.ims.service.im.chat.ChatSession;
import com.orangelabs.rcs.core.ims.service.im.chat.ChatUtils;
import com.orangelabs.rcs.core.ims.service.im.chat.ContributionIdGenerator;
import com.orangelabs.rcs.core.ims.service.im.chat.GeolocMessage;
import com.orangelabs.rcs.core.ims.service.im.chat.GroupChatInfo;
import com.orangelabs.rcs.core.ims.service.im.chat.GroupChatSession;
import com.orangelabs.rcs.core.ims.service.im.chat.InstantMessage;
import com.orangelabs.rcs.core.ims.service.im.chat.OneOneChatSession;
import com.orangelabs.rcs.core.ims.service.im.chat.StandaloneChatSession;
import com.orangelabs.rcs.core.ims.service.im.chat.event.ConferenceInfoDocument;
import com.orangelabs.rcs.core.ims.service.im.chat.event.ConferenceInfoParser;
import com.orangelabs.rcs.core.ims.service.im.chat.event.GroupListDocument;
import com.orangelabs.rcs.core.ims.service.im.chat.event.GroupListDocument.BasicGroupInfo;
import com.orangelabs.rcs.core.ims.service.im.chat.event.GroupListParser;
import com.orangelabs.rcs.core.ims.service.im.chat.event.User;
import com.orangelabs.rcs.platform.AndroidFactory;
import com.orangelabs.rcs.provider.messaging.RichMessagingHistory;
import com.orangelabs.rcs.provider.settings.RcsSettings;
import com.orangelabs.rcs.service.LauncherUtils;
import com.orangelabs.rcs.utils.PhoneUtils;
import com.orangelabs.rcs.core.ims.network.registration.RegistrationInfo;
import com.orangelabs.rcs.core.ims.network.registration.RegistrationInfoParser;
import com.orangelabs.rcs.core.ims.network.sip.FeatureTags;
import com.orangelabs.rcs.core.ims.network.sip.SipUtils;
import com.orangelabs.rcs.utils.logger.Logger;

/**
 * Chat service implementation
 * 
 * @author Jean-Marc AUFFRET
 */
public class ChatServiceImpl extends IChatService.Stub {
    /**
     * List of service event listeners
     */
    private RemoteCallbackList<IJoynServiceRegistrationListener> serviceListeners = new RemoteCallbackList<IJoynServiceRegistrationListener>();

    /**
     * List of chat sessions
     */
    private static Hashtable<String, IChat> chatSessions = new Hashtable<String, IChat>();  

    /**
     * List of chat sessions
     */
    private static Hashtable<String, IExtendChat> extendChatSessions = new Hashtable<String, IExtendChat>();

    /**
     * List of chat sessions
     */
    private static Hashtable<String, IChat> storeForwardChatSessions = new Hashtable<String, IChat>();  

    /**
     * List of group chat sessions
     */
    private static Hashtable<String, IGroupChat> groupChatSessions = new Hashtable<String, IGroupChat>();  

    /**
     * List of file chat invitation listeners
     */
    private RemoteCallbackList<INewChatListener> listeners = new RemoteCallbackList<INewChatListener>();

    /**
     * The logger
     */
    private static Logger logger = Logger.getLogger(ChatServiceImpl.class.getName());

    /**
     * Lock used for synchronization
     */
    private Object lock = new Object();

    /**
     * Constructor
     */
    public ChatServiceImpl() {
        if (logger.isActivated()) {
            logger.info("Chat service API is loaded");
        }
    }

    /**
     * Close API
     */
    public void close() {
        // Clear list of sessions
        chatSessions.clear();
        extendChatSessions.clear();
        storeForwardChatSessions.clear();
        groupChatSessions.clear();
        
        if (logger.isActivated()) {
            logger.info("Chat service API is closed");
        }
    }
    
    /**
     * Returns true if the service is registered to the platform, else returns false
     * 
     * @return Returns true if registered else returns false
     */
    public boolean isServiceRegistered() {
        return ServerApiUtils.isImsConnected();
    }

    /**
     * Registers a listener on service registration events
     * 
     * @param listener Service registration listener
     */
    public void addServiceRegistrationListener(IJoynServiceRegistrationListener listener) {
        synchronized(lock) {
            if (logger.isActivated()) {
                logger.info("Add a service listener");
            }

            serviceListeners.register(listener);
        }
    }
    
    /**
     * Unregisters a listener on service registration events
     * 
     * @param listener Service registration listener
     */
    public void removeServiceRegistrationListener(IJoynServiceRegistrationListener listener) {
        synchronized(lock) {
            if (logger.isActivated()) {
                logger.info("Remove a service listener");
            }
            
            serviceListeners.unregister(listener);
        }    
    }    
    
    /**
     * Receive registration event
     * 
     * @param state Registration state
     */
    public void notifyRegistrationEvent(boolean state) {
        // Notify listeners
        synchronized(lock) {
            final int N = serviceListeners.beginBroadcast();
            if (logger.isActivated()) {
                logger.info("notifyRegistrationEvent N: " + N);
            }
            for (int i=0; i < N; i++) {
                try {
                    if (state) {
                        serviceListeners.getBroadcastItem(i).onServiceRegistered();
                    } else {
                        serviceListeners.getBroadcastItem(i).onServiceUnregistered();
                    }
                } catch(Exception e) {
                    if (logger.isActivated()) {
                        logger.error("Can't notify listener", e);
                    }
                }
            }
            serviceListeners.finishBroadcast();
        }                
    }

    /**
     * New pager message received
     * 
     * @param message Message received
     */
    public void receivePagerMessage(InstantMessage message, String conversationId) {
        if (logger.isActivated()) {
            logger.info("LMM Receive pager message from message " + message.getRemote());
        }

        String contact = message.getRemote();
        String number;
        if (message.getMessageType() == ChatLog.Message.Type.PUBLIC) {
            number = PhoneUtils.extractUuidFromUri(contact);
        } else {
            number = PhoneUtils.extractNumberFromUri(contact);
        }

        RichMessagingHistory rmHistory = RichMessagingHistory.getInstance();

        if (RcsSettings.getInstance().isCPMSupported()) {
            String saved = rmHistory.getCoversationID(number, 1);

            if (saved.isEmpty()) {
                // This is the 1st message for the conversation, save conversation ID here
                InstantMessage conversationMsg = ChatUtils.createTextMessage(contact, "system", false);
                rmHistory.addChatSystemMessage(conversationMsg, ChatLog.Message.Direction.INCOMING);
                rmHistory.UpdateCoversationID(number, conversationId, 1);
            } else
            if (!saved.equals(conversationId)) {
                if (conversationId != null) {
                    if (logger.isActivated()) {
                       logger.info("Receive stand alone chat invitation OldId: " + saved + "NewId: " + conversationId);
                    }
                    rmHistory.UpdateCoversationID(number, conversationId, 1);
                } else {
                    if (logger.isActivated()) {
                        logger.info("Receive stand alone chat invitation Conversation Id is null");
                     }
                }
            }
        }

        synchronized(lock) {
            Object[] sessions = extendChatSessions.values().toArray();
            int i;
            for (i = 0; i < sessions.length; i++) {
                List<String> contacts = ((ExtendChatImpl)sessions[i]).getRemoteContacts();
                if (contacts.contains(number)) {
                    if (logger.isActivated()) {
                        logger.info("LMM extendChat is not null");
                    }
                    ((ExtendChatImpl)sessions[i]).handleReceiveMessage(message);
                    break;
                }
            }
            if (i >= sessions.length) {
                if (logger.isActivated()) {
                    logger.info("LMM chat is null");
                }

                rmHistory.addExtendChatMessage(message, ChatLog.Message.Direction.INCOMING);

                ExtendMessage msg = new ExtendMessage(
                                    message.getMessageId(),
                                    number,
                                    message.getTextMessage(),
                                    message.getServerDate(),
                                    message.isImdnDisplayedRequested(),
                                    message.getDisplayName(),
                                    message.getMessageType());

                // Broadcast intent related to the received invitation
                Intent intent = new Intent(ChatIntent.ACTION_NEW_CHAT);
                intent.addFlags(Intent.FLAG_EXCLUDE_STOPPED_PACKAGES);
                intent.putExtra(ChatIntent.EXTRA_CONTACT, msg.getContact());
                intent.putExtra(ChatIntent.EXTRA_DISPLAY_NAME, msg.getDisplayName());
                intent.putExtra(ChatIntent.EXTRA_MESSAGE, msg);

                AndroidFactory.getApplicationContext().sendBroadcast(intent);

                // Notify chat invitation listeners
                synchronized(lock) {
                    final int N = listeners.beginBroadcast();
                    if (logger.isActivated()) {
                        logger.info("receiveStandaloneChatMessage N: " + N);
                    }
                    for (int j=0; j < N; j++) {
                        try {
                            listeners.getBroadcastItem(i).onNewSingleChat(msg.getContact(), msg);
                        } catch(Exception e) {
                            if (logger.isActivated()) {
                                logger.error("Can't notify listener", e);
                            }
                        }
                    }
                    listeners.finishBroadcast();
                }               
            }
        }
    }

    /**
     * Receive a new chat invitation
     * 
     * @param session Chat session
     */
    public void receiveStandaloneChatInvitation(StandaloneChatSession session) {
        if (logger.isActivated()) {
            logger.info("Receive stand alone chat invitation from " + session.getRemoteContact() + 
                    "  Display name: " + session.getRemoteDisplayName());
        }

        if (logger.isActivated()) {
            logger.info("Receive stand alone chat invitation Conversation: " 
                    + session.getConversationID());
        }

        String contact = session.getRemoteContact();
        String number;
        if (session.getMessageType() == ChatLog.Message.Type.PUBLIC)
            number =  PhoneUtils.extractUuidFromUri(contact);
        else
            number =  PhoneUtils.extractNumberFromUri(contact);
        
        RichMessagingHistory rmHistory = RichMessagingHistory.getInstance();

        if (RcsSettings.getInstance().isCPMSupported()) {
            String newConvId = session.getConversationID();
            String oldConvId = rmHistory.getCoversationID(number, 1);

            if (oldConvId.isEmpty()) {
                // This is the 1st message for the conversation, save conversation ID here
                InstantMessage conversationMsg = ChatUtils.createTextMessage(contact, "system", false);
                rmHistory.addChatSystemMessage(conversationMsg, ChatLog.Message.Direction.INCOMING);
                rmHistory.UpdateCoversationID(number, newConvId, 1);
            } else
            if (!oldConvId.equals(newConvId)) {
                if (newConvId != null) {
                    if (logger.isActivated()) {
                       logger.info("Receive stand alone chat invitation OldId: " + oldConvId + "NewId: " + newConvId);
                    }
                    rmHistory.UpdateCoversationID(number, newConvId, 1);
                } else {
                    if (logger.isActivated()) {
                        logger.info("Receive stand alone chat invitation Conversation Id is null");
                     }
                }
            }
        }

        synchronized(lock) {
            Object[] sessions = extendChatSessions.values().toArray();
            int i;
            for (i = 0; i < sessions.length; i++) {
                ExtendChatImpl extendChat = (ExtendChatImpl)sessions[i];
                List<String> contacts = extendChat.getRemoteContacts();
                if (contacts.contains(number)) {
                    if (logger.isActivated()) {
                        logger.info("LMM extendChat is not null");
                    }
                    if (!session.getListeners().contains(extendChat)) {
                        if (logger.isActivated()) {
                            logger.info("Add extendChat to session as callback~");
                        }
                        session.addListener(extendChat);
                    }
                    break;
                }
            }
            if (i >= sessions.length) {
                // Add session in the list
                ExtendChatImpl extendChat = new ExtendChatImpl(number, this);
                ChatServiceImpl.addExtendChatSession(number, extendChat);
                session.addListener(extendChat);

                // Notify chat invitation listeners
                synchronized(lock) {
                    final int N = listeners.beginBroadcast();
                    if (logger.isActivated()) {
                        logger.info("receiveStandaloneChatMessage N: " + N);
                    }
                    for (int j=0; j < N; j++) {
                        try {
                            listeners.getBroadcastItem(i).onNewSingleChat(number, null);
                        } catch(Exception e) {
                            if (logger.isActivated()) {
                                logger.error("Can't notify listener", e);
                            }
                        }
                    }
                    listeners.finishBroadcast();
                }               
            }
        }
    }

    public void receiveStandaloneChatMessage(ChatMessage msgApi) {

        // Broadcast intent related to the received invitation
        Intent intent = new Intent(ChatIntent.ACTION_NEW_CHAT);
        intent.addFlags(Intent.FLAG_EXCLUDE_STOPPED_PACKAGES);
        intent.putExtra(ChatIntent.EXTRA_CONTACT, msgApi.getContact());
        intent.putExtra(ChatIntent.EXTRA_DISPLAY_NAME, msgApi.getDisplayName());
        intent.putExtra(ChatIntent.EXTRA_MESSAGE, msgApi);

        AndroidFactory.getApplicationContext().sendBroadcast(intent);
        
    }

    /**
     * Receive a new chat invitation
     * 
     * @param session Chat session
     */
    public void receiveOneOneChatInvitation(OneOneChatSession session) {
        if (logger.isActivated()) {
            logger.info("Receive chat invitation from " + session.getRemoteContact() + "  Display name: " + session.getRemoteDisplayName());
        }

        // Extract number from contact 
        String number = PhoneUtils.extractNumberFromUri(session.getRemoteContact());

        // Update rich messaging history
        // Nothing done in database
        // Update rich messaging history with o2o chat message
        
        InstantMessage msg = session.getFirstMessage();
        if(msg != null){
        msg.setDisplayName(session.getRemoteDisplayName());
        if (logger.isActivated()) {
            logger.info("Receive chat invitation from first message display: " + msg.getDisplayName());
        }
        
        
        RichMessagingHistory.getInstance().addChatSystemMessage(msg, ChatLog.Message.Direction.INCOMING);
        }
         if(RcsSettings.getInstance().isCPMSupported()) {
                if(!(RichMessagingHistory.getInstance().getCoversationID(session.getRemoteContact(), 1).equals(session.getConversationID()))){
                    if(session.getConversationID() != null) {
                        if (logger.isActivated()) {
                           logger.info("Receive O2O chat invitation OldId: "  + RichMessagingHistory.getInstance().getCoversationID(session.getRemoteContact(),1) + " NewId: " + session.getConversationID());
                        }
                        RichMessagingHistory.getInstance().UpdateCoversationID(session.getRemoteContact(),session.getConversationID(), 1);
                    } else {
                        if (logger.isActivated()) {
                            logger.info("Receive O2O chat invitation Conversation Id is null");
                         }
                    }
                } else {
                    // Create a text message
                    InstantMessage conversationMsg = ChatUtils.createTextMessage(session.getRemoteContact(), "system",
                            Core.getInstance().getImService().getImdnManager().isImdnActivated());
                     RichMessagingHistory.getInstance().addChatSystemMessage(conversationMsg, ChatLog.Message.Direction.INCOMING);
                     RichMessagingHistory.getInstance().UpdateCoversationID(session.getRemoteContact(),session.getConversationID(),1);
                }
            }        


        // Add session in the list
        ChatImpl sessionApi = new ChatImpl(number, session);
        ChatServiceImpl.addChatSession(number, sessionApi);

        // Broadcast intent related to the received invitation
        Intent intent = new Intent(ChatIntent.ACTION_NEW_CHAT);
        intent.addFlags(Intent.FLAG_EXCLUDE_STOPPED_PACKAGES);
        intent.putExtra(ChatIntent.EXTRA_CONTACT, number);
        intent.putExtra(ChatIntent.EXTRA_DISPLAY_NAME, session.getRemoteDisplayName());
        ChatMessage msgApi = null;
        if(msg != null){
        if (msg instanceof GeolocMessage) {
            GeolocMessage geoloc = (GeolocMessage)msg;
            Geoloc geolocApi = new Geoloc(geoloc.getGeoloc().getLabel(),
                    geoloc.getGeoloc().getLatitude(), geoloc.getGeoloc().getLongitude(),
                    geoloc.getGeoloc().getExpiration());
            msgApi = new org.gsma.joyn.chat.GeolocMessage(geoloc.getMessageId(),
                    PhoneUtils.extractNumberFromUri(geoloc.getRemote()),
                    geolocApi, geoloc.getDate(), geoloc.isImdnDisplayedRequested());
            intent.putExtra(ChatIntent.EXTRA_MESSAGE, msgApi);
        } else {
            msgApi = new ChatMessage(msg.getMessageId(),
                    PhoneUtils.extractNumberFromUri(msg.getRemote()),
                    msg.getTextMessage(), msg.getServerDate(),
                    msg.isImdnDisplayedRequested(),msg.getDisplayName());
            intent.putExtra(ChatIntent.EXTRA_MESSAGE, msgApi);    
        }
        }
        AndroidFactory.getApplicationContext().sendBroadcast(intent);
        
        // Notify chat invitation listeners
        synchronized(lock) {
            final int N = listeners.beginBroadcast();
            for (int i=0; i < N; i++) {
                try {
                    listeners.getBroadcastItem(i).onNewSingleChat(number, msgApi);
                } catch(Exception e) {
                    if (logger.isActivated()) {
                        logger.error("Can't notify listener", e);
                    }
                }
            }
            listeners.finishBroadcast();
        }                
    }  
    
    /**
     * Open a single chat with a given contact and returns a Chat instance.
     * The parameter contact supports the following formats: MSISDN in national
     * or international format, SIP address, SIP-URI or Tel-URI.
     * 
     * @param contact Contact
     * @param listener Chat event listener
     * @return Chat
     * @throws ServerApiException
     */
    public IChat openSingleChat(String contact, IChatListener listener) throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("Open a 1-1 chat session with " + contact);
        }

        // Test IMS connection
        ServerApiUtils.testIms();
        
        try {
            // Extract number from contact 
            String number = PhoneUtils.extractNumberFromUri(contact);

            // Check if there is an existing chat or not
            ChatImpl sessionApi = (ChatImpl)ChatServiceImpl.getChatSession(number);
            //if(sessionApi != null){
            //    OneOneChatSession tempSession = sessionApi.getCoreSession();
            //}
            
            if (sessionApi != null && sessionApi.getCoreSession() != null && !sessionApi.getCoreSession().isStoreAndForward()) {
            //if (sessionApi != null ){
                if (logger.isActivated()) {
                    logger.debug("Chat session already exist for " + number);
                }
                
                // Add session listener
                sessionApi.addEventListener(listener);

                // Check core session state
                final OneOneChatSession coreSession = sessionApi.getCoreSession();
                if (coreSession != null) {
                    if (logger.isActivated()) {
                        logger.debug("Core chat session already exist: " + coreSession.getSessionID());
                    }

                    if (coreSession.getDialogPath().isSessionTerminated() ||
                            coreSession.getDialogPath().isSessionCancelled()) {
                        if (logger.isActivated()) {
                            logger.debug("Core chat session is terminated: reset it");
                        }
                        
                        // Session has expired, remove it
                        sessionApi.resetCoreSession();
                    } else
                    if (!coreSession.getDialogPath().isSessionEstablished()) {
                        if (logger.isActivated()) {
                            logger.debug("Core chat session is pending: auto accept it");
                        }
                        
                        // Auto accept the pending session
                        Thread t = new Thread() {
                            public void run() {
                                coreSession.acceptSession();
                            }
                        };
                        t.start();
                    } else {
                        if (logger.isActivated()) {
                            logger.debug("Core chat session is already established");
                        }
                    }
                }
            } else {
                if (logger.isActivated()) {
                    logger.debug("Create a new chat session with " + number);
                }

                // Add session listener
                sessionApi = new ChatImpl(number);
                sessionApi.addEventListener(listener);
    
                // Add session in the list
                ChatServiceImpl.addChatSession(number, sessionApi);
            }
        
            return sessionApi;
        
        } catch(Exception e) {
            if (logger.isActivated()) {
                logger.error("Unexpected error", e);
            }
            throw new ServerApiException(e.getMessage());
        }
    }    
    
    /**
     * Open a single chat with a given contact and returns a Chat instance.
     * The parameter contact supports the following formats: MSISDN in national
     * or international format, SIP address, SIP-URI or Tel-URI.
     * 
     * @param contact Contact
     * @param listener Chat event listener
     * @return Chat
     * @throws ServerApiException
     */
    public IExtendChat openSingleChatEx(String contact, IExtendChatListener listener) throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("Open a 1-1 chat session with " + contact);
        }

        // Test IMS connection
        ServerApiUtils.testIms();

        try {
            /* To compatible with public account */
            contact = PhoneUtils.extractUuidFromUri(contact);

            // Check if there is an existing chat or not
            ExtendChatImpl sessionApi = (ExtendChatImpl)getExtendChatSession(contact);

            if (sessionApi != null) {
                if (logger.isActivated()) {
                    logger.debug("Chat session already exist for " + contact);
                }
                
                // Add session listener
                sessionApi.addEventListener(listener);
            } else {
                if (logger.isActivated()) {
                    logger.debug("Create a new chat session with " + contact);
                }

                // Add session listener
                sessionApi = new ExtendChatImpl(contact, this);
                sessionApi.addEventListener(listener);

                // Add session in the list
                addExtendChatSession(contact, sessionApi);
            }
        
            return sessionApi;
        
        } catch(Exception e) {
            if (logger.isActivated()) {
                logger.error("Unexpected error", e);
            }
            throw new ServerApiException(e.getMessage());
        }
    }

    /**
     * Open a single chat with a given contact and returns a Chat instance.
     * The parameter contact supports the following formats: MSISDN in national
     * or international format, SIP address, SIP-URI or Tel-URI.
     * 
     * @param contact Contact
     * @param listener Chat event listener
     * @return Chat
     * @throws ServerApiException
     */
    public IExtendChat openMultipleChat(List<String> contacts, IExtendChatListener listener) throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("Open a 1-1 chat session with " + contacts);
        }

        // Test IMS connection
        ServerApiUtils.testIms();

        try {
            String number = PhoneUtils.generateContactsText(contacts);

            // Check if there is an existing chat or not
            ExtendChatImpl sessionApi = (ExtendChatImpl)getExtendChatSession(number);

            if (sessionApi != null) {
                if (logger.isActivated()) {
                    logger.debug("Chat session already exist for " + number);
                }

                // Add session listener
                sessionApi.addEventListener(listener);
            } else {
                if (logger.isActivated()) {
                    logger.debug("Create a new chat session with " + number);
                }

                // Add session listener
                sessionApi = new ExtendChatImpl(contacts, this);
                sessionApi.addEventListener(listener);

                // Add session in the list
                addExtendChatSession(number, sessionApi);
            }

            return sessionApi;
        
        } catch(Exception e) {
            if (logger.isActivated()) {
                logger.error("Unexpected error", e);
            }
            throw new ServerApiException(e.getMessage());
        }
    }

    /**
     * Receive message delivery status
     * 
     * @param contact Contact
     * @param msgId Message ID
     * @param status Delivery status
     */
    public void receiveMessageDeliveryStatus(String contact, String msgId, String status) {
        synchronized(lock) {
            if (logger.isActivated()) {
                logger.info("LMM Receive message delivery status for message " + msgId + ", status " + status);
            }

            String number = PhoneUtils.extractNumberFromUri(contact);

            if (logger.isActivated()) {
                logger.info("LMM receiveMessageDeliveryStatus " + contact + ", number " + number);
            }
    
              // Notify message delivery listeners
            ChatImpl chat = (ChatImpl)ChatServiceImpl.getChatSession(number);
            if (chat != null) {
                if (logger.isActivated()) {
                    logger.info("LMM chat is not null");
                }
                chat.handleMessageDeliveryStatus(msgId, status, null);
            } else {
                Object[] sessions = extendChatSessions.values().toArray();
                int i;
                for (i = 0; i < sessions.length; i++) {
                    List<String> contacts = ((ExtendChatImpl)sessions[i]).getRemoteContacts();
                    if (contacts.contains(number)) {
                        if (logger.isActivated()) {
                            logger.info("LMM extendChat is not null");
                        }
                        ((ExtendChatImpl)sessions[i]).handleMessageDeliveryStatus(msgId, status, number);
                        break;
                    }
                }
                if (i >= sessions.length) {
                    if (logger.isActivated()) {
                        logger.info("LMM chat is null");
                    }
                    // Update rich messaging history
                    RichMessagingHistory.getInstance().updateChatMessageDeliveryStatus(msgId, status);
                    
                    Intent intent = new Intent(ChatIntent.ACTION_DELIVERY_STATUS);
                    intent.addFlags(Intent.FLAG_EXCLUDE_STOPPED_PACKAGES);
                    intent.putExtra(ChatIntent.EXTRA_CONTACT, number);
                    intent.putExtra("msgId", msgId);
                    intent.putExtra("status", status);
                    AndroidFactory.getApplicationContext().sendBroadcast(intent);
                }
            }
        }
    }
    
    /**
     * Add a chat session in the list
     * 
     * @param contact Contact
     * @param session Chat session
     */
    public static void addChatSession(String contact, ChatImpl session) {
        if (logger.isActivated()) {
            logger.debug("LMM a chat session in the list (size=" + chatSessions.size() + ") for " + contact);
        }

        if((session.getCoreSession() != null) && session.getCoreSession().isStoreAndForward()){
            if (logger.isActivated()) {
                logger.debug("LMM ChatSession s&f");
            }
            if (logger.isActivated()) {
                logger.debug("AddRemove add storeForwardChatSessions " + session.getCoreSession().getSessionID());
            }
            storeForwardChatSessions.put(contact, session);
        }
        else{
            if (logger.isActivated()) {
                //logger.debug("LMM AddRemove add chatSessions " + session.getCoreSession().getSessionID());
            }
            chatSessions.put(contact, session);
        }
    }

    /**
     * Add a chat session in the list
     * 
     * @param contact Contact
     * @param session Chat session
     */
    public static void addExtendChatSession(String contact, ExtendChatImpl session) {
        if (logger.isActivated()) {
            logger.debug("LMM a chat session in the list (size=" + extendChatSessions.size() + ") for " + contact);
        }

        extendChatSessions.put(contact, session);
    }

    /**
     * Get a chat session from the list for a given contact
     * 
     * @param contact Contact
     * @param GroupChat session
     */
    protected static IChat getStoreChatSession(String contact) {
        if (logger.isActivated()) {
            logger.debug("LMM Get ChatSession s&f " + contact);
        }
        
        return storeForwardChatSessions.get(contact);
    }

    /**
     * Remove a chat session from the list
     * 
     * @param contact Contact
     */
    protected static void removeStoreChatSession(String contact) {
        if (logger.isActivated()) {
            logger.debug("LMM Remove removeStoreChatSession (size=" + storeForwardChatSessions.size() + ") for " + contact);
        }         
        
        if ((storeForwardChatSessions != null) && (contact != null)) {
            storeForwardChatSessions.remove(contact);
        }
    }

    /**
     * Get a chat session from the list for a given contact
     * 
     * @param contact Contact
     * @param GroupChat session
     */
    protected static IChat getChatSession(String contact) {
        if (logger.isActivated()) {
            logger.debug("LMM Get a chat session for " + contact);
        }
        
        return chatSessions.get(contact);
    }

    /**
     * Get a chat session from the list for a given contact
     * 
     * @param contact Contact
     * @param GroupChat session
     */
    protected static IExtendChat getExtendChatSession(String contact) {
        if (logger.isActivated()) {
            logger.debug("LMM Get a chat session for " + contact);
        }
        
        return extendChatSessions.get(contact);
    }

    /**
     * Remove a chat session from the list
     * 
     * @param contact Contact
     */
    protected static void removeChatSession(String contact) {
        if (logger.isActivated()) {
            logger.debug("LMM Remove a chat session from the list (size=" + chatSessions.size() + ") for " + contact);
        }
        
        if ((chatSessions != null) && (contact != null)) {
            chatSessions.remove(contact);
        }
    }
    
    /**
     * Remove a chat session from the list
     * 
     * @param contact Contact
     */
    protected static void removeExtendChatSession(String contact) {
        if (logger.isActivated()) {
            logger.debug("LMM Remove a chat session from the list (size=" + extendChatSessions.size() + ") for " + contact);
        }
        
        if ((extendChatSessions != null) && (contact != null)) {
            extendChatSessions.remove(contact);
        }
    }

    /**
     * Returns the list of single chats in progress
     * 
     * @return List of chats
     * @throws ServerApiException
     */
    public List<IBinder> getChats() throws ServerApiException {
        int size = chatSessions.size() + storeForwardChatSessions.size();
        if (logger.isActivated()) {
            logger.info("Get chat sessions sze: " + size);
        }

        try {
            ArrayList<IBinder> result = new ArrayList<IBinder>(size);
            for (Enumeration<IChat> e = chatSessions.elements() ; e.hasMoreElements() ;) {
                IChat sessionApi = (IChat)e.nextElement() ;
                result.add(sessionApi.asBinder());
            }
            //ArrayList<IBinder> result = new ArrayList<IBinder>(storeForwardChatSessions.size());
            for (Enumeration<IChat> e = storeForwardChatSessions.elements() ; e.hasMoreElements() ;) {
                IChat sessionApi = (IChat)e.nextElement() ;
                result.add(sessionApi.asBinder());
            }
            return result;
        } catch(Exception e) {
            if (logger.isActivated()) {
                logger.error("Unexpected error", e);
            }
            throw new ServerApiException(e.getMessage());
        }
    }
    
    /**
     * Returns the list of single chats in progress
     * 
     * @return List of chats
     * @throws ServerApiException
     */
    public List<IBinder> getExtendChats() throws ServerApiException {
        int size = extendChatSessions.size();
        if (logger.isActivated()) {
            logger.info("Get chat sessions sze: " + size);
        }

        try {
            ArrayList<IBinder> result = new ArrayList<IBinder>(size);
            for (Enumeration<IExtendChat> e = extendChatSessions.elements() ; e.hasMoreElements() ;) {
                IExtendChat sessionApi = (IExtendChat)e.nextElement() ;
                result.add(sessionApi.asBinder());
            }
            return result;
        } catch(Exception e) {
            if (logger.isActivated()) {
                logger.error("Unexpected error", e);
            }
            throw new ServerApiException(e.getMessage());
        }
    }

    /**
     * Returns a chat in progress from its unique ID
     * 
     * @param contact Contact
     * @return Chat or null if not found
     * @throws ServerApiException
     */
    public IChat getChat(String contact) throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("Get chat session with " + contact);
        }

        // Return a session instance
        return chatSessions.get(contact);
    }
    
    /**
     * Returns a chat in progress from its unique ID
     * 
     * @param contact Contact
     * @return Chat or null if not found
     * @throws ServerApiException
     */
    public IExtendChat getExtendChat(String contact) throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("Get chat session with " + contact);
        }

        // Return a session instance
        return extendChatSessions.get(contact);
    }

    /**
     * Receive a new group chat invitation
     * 
     * @param session Chat session
     */
    public void receiveGroupChatInvitation(GroupChatSession session) {
        if (logger.isActivated()) {
            logger.info("Receive group chat invitation from " +
                    session.getRemoteContact() +
                    " Display name: " + session.getGroupRemoteDisplayName() +
                    " new name: " + SipUtils.getDisplayNameFromUri(session.getRemoteContact()));
        }

        RichMessagingHistory rmHistory = RichMessagingHistory.getInstance();

        // Extract number from contact 
        String number = PhoneUtils.extractNumberFromUri(session.getRemoteContact());

        Boolean isChatExist = false;
        GroupChatInfo info = null;

        isChatExist = rmHistory.isGroupChatExists(session.getContributionID());

        // Contribution ID might be different for re-Invite case, try conference ID once more
        if (isChatExist) {
            info = rmHistory.getGroupChatInfo(session.getContributionID());
        } else {
            info = rmHistory.getGroupChatInfoByRejoinId(session.getImSessionIdentity());
            if (info != null) {
                session.setContributionID(info.getContributionId());
                isChatExist = true;
            }
        }

        // Update rich messaging history if not present , to stop same chatId to come , TODO add update function and call in else
        if (!isChatExist) {
            rmHistory.addGroupChat(
                    session.getContributionID(),
                    session.getSubject(),
                    session.getParticipants().getList(),
                    GroupChat.State.INVITED,
                    GroupChat.Direction.INCOMING);
            info = rmHistory.getGroupChatInfo(session.getContributionID());
        }

        int blocked = rmHistory.getGroupBlockedStatus(info.getSessionId());
        if(blocked == 1){
           session.setMessageBlocked(true);
        } else if(blocked == 0){
           session.setMessageBlocked(false);
        }
        //session.setChairman(session.getRemoteContact());//chairman already set in session
        //RichMessagingHistory.getInstance().updateGroupChairman(session.getContributionID(), session.getChairman());
        if (RcsSettings.getInstance().isCPMSupported()) {
            if (logger.isActivated()) {
                logger.info("receiveGroupChatInvitation" +
                        " conversationId: " + session.getConversationID() +
                        " contributionId: " + session.getContributionID());
            }
            rmHistory.UpdateCoversationID(info.getSessionId(), session.getConversationID(), 2);
        }

        // Add session in the list
        GroupChatImpl sessionApi = new GroupChatImpl(session);
        ChatServiceImpl.addGroupChatSession(sessionApi);        

        if (logger.isActivated()) {
            logger.info("receiveGroupChatInvitation ischatexists: " + isChatExist);
        }

        // Broadcast intent related to the received invitation
        Intent intent = new Intent(GroupChatIntent.ACTION_NEW_INVITATION);
        intent.addFlags(Intent.FLAG_EXCLUDE_STOPPED_PACKAGES);
        intent.putExtra(GroupChatIntent.EXTRA_CONTACT, number);
        intent.putExtra(GroupChatIntent.EXTRA_DISPLAY_NAME, session.getGroupRemoteDisplayName());
        intent.putExtra(GroupChatIntent.EXTRA_CHAT_ID, sessionApi.getChatId());
        intent.putExtra(GroupChatIntent.EXTRA_SUBJECT, sessionApi.getSubject());
        intent.putExtra("autoAccept", RcsSettings.getInstance().isGroupChatAutoAccepted());
        intent.putExtra("isGroupChatExist", isChatExist);
        intent.putExtra("isClosedGroupChat", session.isClosedGroup());
        intent.putExtra(GroupChatIntent.EXTRA_SESSION_IDENTITY, session.getImSessionIdentity());
         /**
         * M: managing extra local chat participants that are 
         * not present in the invitation for sending them invite request.@{
         */
        String participants = "";
        List<String> ListParticipant = session.getParticipants().getList();
        for(String currentPartc : ListParticipant){
         participants += currentPartc + ";";    
        }
        /**
         * @}
         */      
        intent.putExtra("participantList", participants);
        
        AndroidFactory.getApplicationContext().sendBroadcast(intent);
        
        // Notify chat invitation listeners
        synchronized(lock) {
            final int N = listeners.beginBroadcast();
            if (logger.isActivated()) {
                logger.info("receiveGroupChatInvitation N: " + N);
            }
            for (int i=0; i < N; i++) {
                try {
                    listeners.getBroadcastItem(i).onNewGroupChat(sessionApi.getChatId());
                } catch(Exception e) {
                    if (logger.isActivated()) {
                        logger.error("Can't notify listener", e);
                    }
                }
            }
            listeners.finishBroadcast();
        }        
    }

    /**
     * Extend a 1-1 chat session
     * 
     * @param groupSession Group chat session
     * @param oneoneSession 1-1 chat session
     */
    public void extendOneOneChatSession(GroupChatSession groupSession, OneOneChatSession oneoneSession) {
        if (logger.isActivated()) {
            logger.info("extendOneOneChatSession ReplaceId: " + groupSession.getSessionID());
        }

        // Add session in the list
        GroupChatImpl sessionApi = new GroupChatImpl(groupSession);
        ChatServiceImpl.addGroupChatSession(sessionApi);
        
        if (logger.isActivated()) {
            logger.info("extendOneOneChatSession ExtraChatId: " + sessionApi.getChatId());
        }
        
        // Broadcast intent related to the received invitation
        Intent intent = new Intent(GroupChatIntent.ACTION_SESSION_REPLACED);
        intent.putExtra("sessionId", groupSession.getSessionID());
        intent.putExtra(GroupChatIntent.EXTRA_CHAT_ID, sessionApi.getChatId());
        AndroidFactory.getApplicationContext().sendBroadcast(intent);
    }
    
    /**
     * Add a group chat session in the list
     * 
     * @param session Chat session
     */
    protected static void addGroupChatSession(GroupChatImpl session) {
        if (logger.isActivated()) {
            logger.debug("Add a group chat session in the list (size=" + groupChatSessions.size() + ")");
        }
        
        groupChatSessions.put(session.getChatId(), session);
    }

    /**
     * Remove a group chat session from the list
     * 
     * @param chatId Chat ID
     */
    protected static void removeGroupChatSession(String chatId) {
        if (logger.isActivated()) {
            logger.debug("Remove a group chat session from the list (size=" + groupChatSessions.size() + ")");
        }
        
        groupChatSessions.remove(chatId);
    }

    /**
     * Returns the state of the group chat message
     * 
     * @return State of the message
     * @see GroupChat.MessageState
     */
    public int getState(String messageId) {
        //int messageStatus = RichMessagingHistory.getInstance().getMessageStatus(messageId);
        int messageStatus = 0;
        switch(messageStatus){
            case ChatLog.Message.Status.Content.SENDING:
                return Chat.MessageState.SENDING;
                
            case ChatLog.Message.Status.Content.SENT:
                return Chat.MessageState.SENT;
                
            case ChatLog.Message.Status.Content.UNREAD_REPORT:
            case ChatLog.Message.Status.Content.UNREAD:
            case ChatLog.Message.Status.Content.READ:
                return Chat.MessageState.DELIVERED;
                
            case ChatLog.Message.Status.Content.FAILED:
                return Chat.MessageState.FAILED;
                
            default:
                return Chat.MessageState.FAILED;
        }
    }

    /**
     * Initiates a group chat with a group of contact and returns a GroupChat
     * instance. The subject is optional and may be null.
     * 
     * @param contact List of contacts
     * @param subject Subject
     * @param listener Chat event listener
     * @throws ServerApiException
     */
    public IGroupChat initiateGroupChat(List<String> contacts, String subject, IGroupChatListener listener) throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("Initiate an ad-hoc group chat session Subject:" + subject);
        }

        if (logger.isActivated()) {
            logger.info("initiateGroupChat contacts:" + contacts);
        }
        
        // Test IMS connection
        ServerApiUtils.testIms();

        try {
            // Initiate the session
            final ChatSession session = Core.getInstance().getImService().initiateAdhocGroupChatSession(contacts, subject);

            // Add session listener
            GroupChatImpl sessionApi = new GroupChatImpl((GroupChatSession)session);
            sessionApi.addEventListener(listener);

            // Update rich messaging history
            RichMessagingHistory.getInstance().addGroupChat(
                    session.getContributionID(),
                    session.getSubject(),
                    session.getParticipants().getList(),
                    GroupChat.State.INITIATED,
                    GroupChat.Direction.OUTGOING);

            if (RcsSettings.getInstance().isCPMSupported()) {
                if (logger.isActivated()) {
                    logger.info("initiateGroupChat conversationId: "
                            + session.getConversationID() 
                            + " contributionId: " 
                            + session.getContributionID());
                }
                RichMessagingHistory.getInstance().UpdateCoversationID(
                        session.getContributionID(),
                        session.getConversationID(),
                        2);
            }
            session.setChairman(ImsModule.IMS_USER_PROFILE.getUsername());
            RichMessagingHistory.getInstance().updateGroupChairman(session.getContributionID(), session.getChairman());

            // Start the session
            Thread t = new Thread() {
                public void run() {
                    session.startSession();
                }
            };
            t.start();
                        
            // Add session in the list
            ChatServiceImpl.addGroupChatSession(sessionApi);
            return sessionApi;
        } catch(Exception e) {
            if (logger.isActivated()) {
                logger.error("Unexpected error", e);
            }
            throw new ServerApiException(e.getMessage());
        }
    }
    
    /**
     * Initiates a group chat with a group of contact and returns a GroupChat
     * instance. The subject is optional and may be null.
     * 
     * @param contact List of contacts
     * @param subject Subject
     * @param listener Chat event listener
     * @throws ServerApiException
     */
    public IGroupChat initiateClosedGroupChat(List<String> contacts, String subject, IGroupChatListener listener) throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("Initiate an closed group chat session Subject:" + subject);
        }
        
        if (logger.isActivated()) {
            logger.info("initiateClosedGroupChat contacts:" + contacts);
        }
        
        // Test IMS connection
        ServerApiUtils.testIms();

        try {
            // Initiate the session
            final ChatSession session = Core.getInstance().getImService().initiateClosedGroupChatSession(contacts, subject);

            // Add session listener
            GroupChatImpl sessionApi = new GroupChatImpl((GroupChatSession)session);
            sessionApi.addEventListener(listener);

            // Update rich messaging history
            RichMessagingHistory.getInstance().addGroupChat(session.getContributionID(),
                    session.getSubject(), session.getParticipants().getList(),
                    GroupChat.State.INITIATED, GroupChat.Direction.OUTGOING);
            session.setChairman(ImsModule.IMS_USER_PROFILE.getUsername());
            RichMessagingHistory.getInstance().updateGroupChairman(session.getContributionID(), session.getChairman());

            // Start the session
            Thread t = new Thread() {
                public void run() {
                    session.startSession();
                }
            };
            t.start();
                        
            // Add session in the list
            ChatServiceImpl.addGroupChatSession(sessionApi);
            return sessionApi;
        } catch(Exception e) {
            if (logger.isActivated()) {
                logger.error("Unexpected error", e);
            }
            throw new ServerApiException(e.getMessage());
        }
    }
    
    /**
     * Rejoins an existing group chat from its unique chat ID
     * 
     * @param chatId Chat ID
     * @return Group chat
     * @throws ServerApiException
     */
    public IGroupChat rejoinGroupChat(String chatId) throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("Rejoin group chat session related to the conversation " + chatId);
        }

        // Test IMS connection
        ServerApiUtils.testIms();

        try {
            // Initiate the session
            final ChatSession session = Core.getInstance().getImService().rejoinGroupChatSession(chatId);
            
            // Start the session
            Thread t = new Thread() {
                public void run() {
                    session.startSession();
                }
            };
            t.start();

            // Add session in the list
            GroupChatImpl sessionApi = new GroupChatImpl((GroupChatSession)session);
            ChatServiceImpl.addGroupChatSession(sessionApi);
            return sessionApi;
        } catch(Exception e) {
            if (logger.isActivated()) {
                logger.error("Unexpected error", e);
            }
            throw new ServerApiException(e.getMessage());
        }
    }
    
    /**
     * Rejoins an existing group chat from its unique chat ID
     * 
     * @param chatId Chat ID
     * @return Group chat
     * @throws ServerApiException
     */
    public IGroupChat rejoinGroupChatId(String chatId, String rejoinId) throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("Rejoin group chat session related to the conversation " + chatId + "; rejoinId: " + rejoinId);
        }

        // Test IMS connection
        ServerApiUtils.testIms();

        try {
            // Initiate the session
            final ChatSession session = Core.getInstance().getImService().rejoinGroupChatSession(chatId,rejoinId);
            
            // Start the session
            Thread t = new Thread() {
                public void run() {
                    session.startSession();
                }
            };
            t.start();

            // Add session in the list
            GroupChatImpl sessionApi = new GroupChatImpl((GroupChatSession)session);
            ChatServiceImpl.addGroupChatSession(sessionApi);
            return sessionApi;
        } catch(Exception e) {
            if (logger.isActivated()) {
                logger.error("Unexpected error", e);
            }
            throw new ServerApiException(e.getMessage());
        }
    }
    
    /**
     * Restarts a previous group chat from its unique chat ID
     * 
     * @param chatId Chat ID
     * @return Group chat
     * @throws ServerApiException
     */
    public IGroupChat restartGroupChat(String chatId) throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("Restart group chat session related to the conversation " + chatId);
        }

        // Test IMS connection
        ServerApiUtils.testIms();

        try {
            // Initiate the session
            final ChatSession session = Core.getInstance().getImService().restartGroupChatSession(chatId);

            // Start the session
            Thread t = new Thread() {
                public void run() {
                    session.startSession();
                }
            };
            t.start();
            
            // Add session in the list
            GroupChatImpl sessionApi = new GroupChatImpl((GroupChatSession)session);
            ChatServiceImpl.addGroupChatSession(sessionApi);
            return sessionApi;
        } catch(Exception e) {
            if (logger.isActivated()) {
                logger.error("Unexpected error", e);
            }
            throw new ServerApiException(e.getMessage());
        }
    }

    public void syncAllGroupChats(IGroupChatSyncingListener listener) throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("Sync all group chats from server");
        }
        ServerApiUtils.testIms();
        new GrouplistSubscriber(listener).start();
    }

    public void syncGroupChat(String chatId, IGroupChatSyncingListener listener)
            throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("Sync one group chat from server");
        }
        ServerApiUtils.testIms();

        InstantMessagingService imService = Core.getInstance().getImsModule().getInstantMessagingService();

        if (chatId == null) {
            if (logger.isActivated()) {
                logger.error("Sync group chat info with null chatId");
            }
            throw new ServerApiException("Invalid Argument");
        }

        /* No need to sync group chat offline when session is active */
        Vector<ChatSession> sessions = imService.getImSessions();
        for (int i=0; i < sessions.size(); i++) {
            ChatSession session = sessions.get(i);
            if (session instanceof GroupChatSession) {
                if (chatId.equals(session.getContributionID()))
                    return;
            }
        }

        new GroupInfoSubscriber(chatId, listener).start();
    }

    /**
     * Returns the list of group chats in progress
     * 
     * @return List of group chat
     * @throws ServerApiException
     */
    public List<IBinder> getGroupChats() throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("Get group chat sessions");
        }

        try {
            ArrayList<IBinder> result = new ArrayList<IBinder>(groupChatSessions.size());
            for (Enumeration<IGroupChat> e = groupChatSessions.elements() ; e.hasMoreElements() ;) {
                IGroupChat sessionApi = (IGroupChat)e.nextElement() ;
                result.add(sessionApi.asBinder());
            }
            return result;
        } catch(Exception e) {
            if (logger.isActivated()) {
                logger.error("Unexpected error", e);
            }
            throw new ServerApiException(e.getMessage());
        }
    }
    
    /**
     * Returns a group chat in progress from its unique ID
     * 
     * @param chatId Chat ID
     * @return Group chat or null if not found
     * @throws ServerApiException
     */
    public IGroupChat getGroupChat(String chatId) throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("Get group chat session " + chatId);
        }

        // Return a session instance
        return groupChatSessions.get(chatId);
    }
    
    /**
     * Block messages in group, stack will not notify application about
     * any received message in this group
     *
     * @param chatId chatId of the group
     * @param flag true means block the message, false means unblock it
     * @throws JoynServiceException
     */
    public void blockGroupMessages(String chatId, boolean flag) {
         if (logger.isActivated()) {
                logger.info("GCM blockGroupMessages flag:" + flag);
        }
        GroupChatImpl groupImpl = (GroupChatImpl)groupChatSessions.get(chatId);
        if(groupImpl != null){
            groupImpl.blockMessages(flag);
        } else{
            // Only Update flag in DB, can't update in session
            if(flag == true) {
                RichMessagingHistory.getInstance().updateGroupBlockedStatus(chatId, 1);
            } else {
                RichMessagingHistory.getInstance().updateGroupBlockedStatus(chatId, 0);
            }
        }
    }
    
    /**
     * Adds a listener on new chat invitation events
     * 
     * @param listener Chat invitation listener
     * @throws ServerApiException
     */
    public void addEventListener(INewChatListener listener) throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("Add a chat invitation listener");
        }
        
        listeners.register(listener);
    }
    
    /**
     * Removes a listener on new chat invitation events
     * 
     * @param listener Chat invitation listener
     * @throws ServerApiException
     */
    public void removeEventListener(INewChatListener listener) throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("Remove a chat invitation listener");
        }
        
        listeners.unregister(listener);
    }
    
    /**
     * Returns the configuration of the chat service
     * 
     * @return Configuration
     */
    public ChatServiceConfiguration getConfiguration() {
        return new ChatServiceConfiguration(
                RcsSettings.getInstance().isStoreForwardWarningActivated(),
                RcsSettings.getInstance().getChatIdleDuration(),
                RcsSettings.getInstance().getIsComposingTimeout(),
                RcsSettings.getInstance().getMaxChatParticipants(),
                RcsSettings.getInstance().getMaxChatMessageLength(),
                RcsSettings.getInstance().getMaxGroupChatMessageLength(),
                RcsSettings.getInstance().getMaxChatSessions(),
                RcsSettings.getInstance().isSmsFallbackServiceActivated(),
                RcsSettings.getInstance().isChatAutoAccepted(),
                RcsSettings.getInstance().isGroupChatAutoAccepted(),
                RcsSettings.getInstance().isImReportsActivated(),
                RcsSettings.getInstance().getMaxGeolocLabelLength(),
                RcsSettings.getInstance().getGeolocExpirationTime(),
                RcsSettings.getInstance().isImAlwaysOn());
    }    

    /**
     * Registers a new chat invitation listener
     * 
     * @param listener New file transfer listener
     * @throws ServerApiException
     */
    public void addNewChatListener(INewChatListener listener) throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("Add a new chat invitation listener");
        }
        
        listeners.register(listener);
    }

    /**
     * Unregisters a chat invitation listener
     * 
     * @param listener New file transfer listener
     * @throws ServerApiException
     */
    public void removeNewChatListener(INewChatListener listener) throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("Remove a chat invitation listener");
        }
        
        listeners.unregister(listener);
    }

    /**
     * Returns IM always on or not
     * 
     * @return boolean value
     * @throws ServerApiException
     */
    public boolean isImCapAlwaysOn(){
        return RcsSettings.getInstance().isImAlwaysOn();
    }

    /**
     * Returns service version
     * 
     * @return Version
     * @see Build.VERSION_CODES
     * @throws ServerApiException
     */
    public int getServiceVersion() throws ServerApiException {
        return Build.API_VERSION;
    }
    
    private static class GrouplistSubscriber extends Thread {
        public GrouplistSubscriber(IGroupChatSyncingListener listener) {
            this.listener = listener;
        }

        public void terminate() {
            beenCanceled = true;
        }

        @Override
        public void run() {
            boolean missing = false;
            synchronized(listLock) {
                subscribeGrouplist();
                try {
                    listLock.wait();
                    if (logger.isActivated()) {
                        logger.info("sync group list done!");
                    }
                    listener.onSyncStart(basicGroupInfos.size());
                    if (basicGroupInfos.size() == 0) {
                        listener.onSyncDone(0);
                    } else {
                        while (basicGroupInfos.size() > 0) {
                            synchronized (infoLock) {
                                String rejoinId = basicGroupInfos.get(0).getUri();
                                String convId = basicGroupInfos.get(0).getConversationid();
                                subscribeGroupInfo(rejoinId, convId);
                                try {
                                    infoLock.wait();
                                    if (eventData == null) {
                                        missing = true;
                                    }
                                    else {
                                        logger.info("callback to group info handler");
                                        String chatId = RichMessagingHistory.getInstance()
                                                .getChatIdbyRejoinId(rejoinId);
                                        listener.onSyncInfo(chatId, eventData);
                                    }
                                } catch (InterruptedException e) {
                                    missing = true;
                                } finally {
                                    basicGroupInfos.remove(0);
                                }
                            }
                        }
                        if (missing)
                            listener.onSyncDone(-1);
                        else
                            listener.onSyncDone(0);
                    }
                } catch (InterruptedException e) {
                    try {
                        listener.onSyncDone(-1);
                    } catch (Exception e2) {

                    }
                } catch (Exception e1) {

                }
            }
        }

        private void subscribeGrouplist() {
            SubscriptionManager manager = SubscriptionManager.getInstance();

            SubscriptionManager.EventCallback eventCallback = new SubscriptionManager.EventCallback() {
                @Override
                protected void handleEventNotify(byte[] content) {
                    if (logger.isActivated()) {
                        logger.info("subscribe group list receive notify");
                    }
                    if (content == null || content.length == 0)
                        return;
                    try {
                        if (logger.isActivated()) {
                            logger.info("group list not null");
                        }
                        InputSource input = new InputSource(new ByteArrayInputStream(content));
                        GroupListParser parser = new GroupListParser(input);
                        GroupListDocument info = parser.getGroupList();

                        basicGroupInfos = info.getGroups();
                        if (logger.isActivated()) {
                            logger.info("group list size: " + basicGroupInfos.size());
                        }
                    } catch (Exception e) {
                        if (logger.isActivated()) {
                            logger.error("Can't parse XML notification", e);
                        }
                    } finally {
                        synchronized (listLock) {
                            listLock.notify();
                        }
                    }
                }

                @Override
                protected void onActive(String identity) {
                }

                @Override
                protected void onPending(String identity) {
                }

                @Override
                protected void onTerminated(String reason, int retryAfter) {
                }
            };

            String contentType = "application/grouplist-ver+xml";
            String content = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" + SipUtils.CRLF +
                             "<grouplist-ver version=0\">" + SipUtils.CRLF +
                             "</grouplist-ver>";

            SubscribeRequest request = new SubscribeRequest.Builder()
                .setRequestUri(RcsSettings.getInstance().getImConferenceUri())
                .setSubscibeEvent("grouplist")
                .setAcceptContent("application/conference-info+xml")
                .setContent(contentType, content.getBytes())
                .build();
            manager.pollStatus(request, eventCallback);
        }

        private void subscribeGroupInfo(final String groupId, final String convId) {
            SubscriptionManager manager = SubscriptionManager.getInstance();

            SubscriptionManager.EventCallback callback = new SubscriptionManager.EventCallback() {
                @Override
                protected void handleEventNotify(byte[] content) {
                    logger.info("handle group info notify");
                    if (content == null || content.length == 0)
                        return;
                    try {
                        InputSource input = new InputSource(new ByteArrayInputStream(content));
                        ConferenceInfoParser parser = new ConferenceInfoParser(input);
                        ConferenceInfoDocument info = parser.getConferenceInfo();

                        if (info == null)
                            return;

                        String chatId = updateGroupChat(groupId, convId, info);
                        ArrayList<ConferenceUser> confUsers =
                                new ArrayList<ConferenceUser>();

                        Vector<User> users = info.getUsers();
                        for (User user:users) {
                            ConferenceUser confUser =
                                    new ConferenceUser(
                                            user.getEntity(),
                                            user.getUserState(),
                                            user.getState(),
                                            user.getDisconnectionMethod(),
                                            user.getRole(),
                                            user.getEtype(),
                                            user.getDisplayName());
                            confUsers.add(confUser);
                        }
                        eventData = new ConferenceEventData(
                                "full",
                                info.getSubject(),
                                info.getChairman(),
                                confUsers);
                    } catch (Exception e) {
                        if (logger.isActivated()) {
                            logger.error("Can't parse XML notification", e);
                        }
                    } finally {
                        synchronized (infoLock) {
                            infoLock.notify();
                        }
                    }
                }

                @Override
                protected void onActive(String identity) {
                }

                @Override
                protected void onPending(String identity) {
                }

                @Override
                protected void onTerminated(String reason, int retryAfter) {
                }
            };

            eventData = null;
            SubscribeRequest request = new SubscribeRequest.Builder()
                .setRequestUri(groupId)
                .setSubscibeEvent("groupinfo")
                .setAcceptContent("application/conference-info+xml")
                .build();
            manager.pollStatus(request, callback);
        }

        private String updateGroupChat(String rejoinId, String convId, ConferenceInfoDocument info) {
            RichMessagingHistory msgHistory = RichMessagingHistory.getInstance();
            GroupChatInfo chatInfo = msgHistory.getGroupChatInfoByRejoinId(rejoinId);
            String chatId = null;

            if (chatInfo == null) {
                String callId = Core.getInstance().getImsModule().getSipManager().getSipStack().generateCallId();
                chatId = ContributionIdGenerator.getContributionId(callId);

                Vector<User> users = info.getUsers();
                ArrayList<String> participants = new ArrayList<String>();

                for (User user:users) {
                    participants.add(user.getEntity());
                }

                msgHistory.addGroupChat(
                            chatId,
                            info.getSubject(),
                            participants,
                            GroupChat.State.TERMINATED,
                            GroupChat.Direction.INCOMING);

                msgHistory.updateGroupChatRejoinId(chatId, rejoinId);
                msgHistory.updateGroupChairman(chatId, info.getChairman());
                msgHistory.UpdateCoversationID(chatId, convId, 2);
            } else {
                chatId = chatInfo.getContributionId();
                msgHistory.updateGroupChatSubject(chatId, info.getSubject());
                msgHistory.updateGroupChairman(chatId, info.getChairman());
            }
            return chatId;
        }

        private IGroupChatSyncingListener listener;
        private List<BasicGroupInfo> basicGroupInfos;
        ConferenceEventData eventData;

        Object listLock = new Object();
        Object infoLock = new Object();
        boolean beenCanceled = false;
    }


    private static class GroupInfoSubscriber extends Thread {
        private String chatId;
        private IGroupChatSyncingListener listener;

        public GroupInfoSubscriber(String chatId, IGroupChatSyncingListener listener) {
            this.chatId = chatId;
            this.listener = listener;
        }

        @Override
        public void run() {
            GroupChatInfo chatInfo = RichMessagingHistory.getInstance().getGroupChatInfo(chatId);
            subscribeGroupInfo(chatInfo.getRejoinId());
        }

        private void subscribeGroupInfo(final String groupId) {
            SubscriptionManager manager = SubscriptionManager.getInstance();

            SubscriptionManager.EventCallback callback = new SubscriptionManager.EventCallback() {
                @Override
                protected void handleEventNotify(byte[] content) {
                    if (content == null || content.length == 0)
                        return;
                    try {
                        InputSource input = new InputSource(new ByteArrayInputStream(content));
                        ConferenceInfoParser parser = new ConferenceInfoParser(input);
                        ConferenceInfoDocument info = parser.getConferenceInfo();

                        if (info == null)
                            return;

                        String chatId = updateGroupChat(groupId, info);
                        ArrayList<ConferenceUser> confUsers =
                                new ArrayList<ConferenceUser>();

                        Vector<User> users = info.getUsers();
                        for (User user:users) {
                            ConferenceUser confUser =
                                    new ConferenceUser(
                                            user.getEntity(),
                                            user.getUserState(),
                                            user.getState(),
                                            user.getDisconnectionMethod(),
                                            user.getRole(),
                                            user.getEtype(),
                                            user.getDisplayName());
                            confUsers.add(confUser);
                        }
                        listener.onSyncInfo(chatId, new ConferenceEventData(
                                "full",
                                info.getSubject(),
                                info.getChairman(),
                                confUsers));
                    } catch (Exception e) {
                        if (logger.isActivated()) {
                            logger.error("Can't parse XML notification", e);
                        }
                    } finally {
                    }
                }

                @Override
                protected void onActive(String identity) {
                }

                @Override
                protected void onPending(String identity) {
                }

                @Override
                protected void onTerminated(String reason, int retryAfter) {
                }
            };

            SubscribeRequest request = new SubscribeRequest.Builder()
                .setRequestUri(groupId)
                .setSubscibeEvent("groupinfo")
                .setAcceptContent("application/conference-info+xml")
                .build();
            manager.pollStatus(request, callback);
        }

        private String updateGroupChat(String rejoinId, ConferenceInfoDocument info) {
            RichMessagingHistory msgHistory = RichMessagingHistory.getInstance();
            GroupChatInfo chatInfo = msgHistory.getGroupChatInfoByRejoinId(rejoinId);
            String chatId = null;

            if (chatInfo == null) {
                String callId = Core.getInstance().getImsModule().getSipManager().getSipStack().generateCallId();
                chatId = ContributionIdGenerator.getContributionId(callId);

                Vector<User> users = info.getUsers();
                ArrayList<String> participants = new ArrayList<String>();

                for (User user:users) {
                    participants.add(user.getEntity());
                }

                msgHistory.addGroupChat(
                            chatId,
                            info.getSubject(),
                            participants,
                            GroupChat.State.TERMINATED,
                            GroupChat.Direction.INCOMING);

                msgHistory.updateGroupChatRejoinId(chatId, rejoinId);
                msgHistory.updateGroupChairman(chatId, info.getChairman());
            } else {
                chatId = chatInfo.getContributionId();
                msgHistory.updateGroupChatSubject(chatId, info.getSubject());
                msgHistory.updateGroupChairman(chatId, info.getChairman());
            }
            return chatId;
        }
    }
}
