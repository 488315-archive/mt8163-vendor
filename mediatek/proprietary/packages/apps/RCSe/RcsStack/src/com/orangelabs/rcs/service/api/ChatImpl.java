package com.orangelabs.rcs.service.api;

import java.nio.charset.Charset;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.TimeZone;
import java.util.concurrent.CopyOnWriteArrayList;

import javax2.sip.header.ExtensionHeader;
import javax2.sip.header.Header;

import org.gsma.joyn.JoynServiceException;
import org.gsma.joyn.chat.Chat;
import org.gsma.joyn.chat.ChatIntent;
import org.gsma.joyn.chat.ChatLog;
import org.gsma.joyn.chat.ChatMessage;
import org.gsma.joyn.chat.ConferenceEventData.ConferenceUser;
import org.gsma.joyn.chat.Geoloc;
import org.gsma.joyn.chat.IChat;
import org.gsma.joyn.chat.IChatListener;

import android.content.ContentResolver;
import android.content.Intent;
import android.database.Cursor;
import android.os.AsyncTask;
import android.os.RemoteCallbackList;
import android.text.TextUtils;
import android.util.Base64;

import com.orangelabs.rcs.core.Core;
import com.orangelabs.rcs.core.ims.ImsModule;
import com.orangelabs.rcs.core.ims.service.ImsService;
import com.orangelabs.rcs.core.ims.service.SessionAuthenticationAgent;
import com.orangelabs.rcs.core.ims.service.im.chat.ChatError;
import com.orangelabs.rcs.core.ims.service.im.chat.ChatSessionListener;
import com.orangelabs.rcs.core.ims.service.im.chat.ChatUtils;
import com.orangelabs.rcs.core.ims.service.im.chat.ContributionIdGenerator;
import com.orangelabs.rcs.core.ims.service.im.chat.GeolocMessage;
import com.orangelabs.rcs.core.ims.service.im.chat.GeolocPush;
import com.orangelabs.rcs.core.ims.service.im.chat.InstantMessage;
import com.orangelabs.rcs.core.ims.service.im.chat.OneOneChatSession;
import com.orangelabs.rcs.core.ims.service.im.chat.cpim.CpimMessage;
import com.orangelabs.rcs.core.ims.service.im.chat.imdn.ImdnDocument;
import com.orangelabs.rcs.core.ims.service.im.chat.imdn.ImdnUtils;
import com.orangelabs.rcs.platform.AndroidFactory;
import com.orangelabs.rcs.provider.messaging.RichMessagingHistory;
import com.orangelabs.rcs.provider.settings.RcsSettings;
import com.orangelabs.rcs.utils.DateUtils;
import com.orangelabs.rcs.utils.PhoneUtils;
import com.orangelabs.rcs.utils.StringUtils;
import com.orangelabs.rcs.utils.logger.Logger;
import com.orangelabs.rcs.core.ims.network.sip.FeatureTags;
import com.orangelabs.rcs.core.ims.network.sip.Multipart;
import com.orangelabs.rcs.core.ims.network.sip.SipMessageFactory;
import com.orangelabs.rcs.core.ims.network.sip.SipUtils;
import com.orangelabs.rcs.core.ims.protocol.sip.SipDialogPath;
import com.orangelabs.rcs.core.ims.protocol.sip.SipException;
import com.orangelabs.rcs.core.ims.protocol.sip.SipRequest;
import com.orangelabs.rcs.core.ims.protocol.sip.SipTransactionContext;

import gov2.nist.javax2.sip.header.SIPHeader;

/**
* Chat implementation
*
 * @author Jean-Marc AUFFRET
*/
public class ChatImpl extends IChat.Stub implements ChatSessionListener {
    /**
     * Remote contact
     */
    private String contact;


    private ImsService imsService;
    /**
     * Core session
     */
    private OneOneChatSession session;

    /**
     * List of listeners
     */
    private RemoteCallbackList<IChatListener> listeners = new RemoteCallbackList<IChatListener>();

    /**
     * Lock used for synchronisation
     */
    private Object lock = new Object();

    /**
     * The logger
     */
    private Logger logger = Logger.getLogger(this.getClass().getName());

    /**
     * Constructor
     *
     * @param contact Remote contact
     */
    public ChatImpl(String contact) {
        this.contact = contact;
        this.session = null;
        imsService = Core.getInstance().getImService();
    }

    public String getChatId(){
        return session.getRemoteContact();
    }

    /**
     * Constructor
     *
     * @param contact Remote contact
     * @param session Session
     */
    public ChatImpl(String contact, OneOneChatSession session) {
        this.contact = contact;
        this.session = session;

        session.addListener(this);
    }

    /**
     * Set core session
     *
     * @param session Core session
     */
    public void setCoreSession(OneOneChatSession session) {
        this.session = session;

        session.addListener(this);
    }

    /**
     * Reset core session
     */
    public void resetCoreSession() {
        this.session = null;
    }

    /**
     * Get core session
     *
     * @return Core session
     */
    public OneOneChatSession getCoreSession() {
        return session;
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
     * Returns the remote contact
     *
     * @return Contact
     */
    public String getRemoteContact() {
        return PhoneUtils.extractNumberFromUri(contact);
    }

    /**
     * Sends a plain text message
     *
     * @param message Text message
     * @return Unique message ID or null in case of error
     */
    public String sendMessage(String message) {
        if (logger.isActivated()) {
            logger.debug("Send text message:" + message);
        }

        // Create a text message
        InstantMessage msg = ChatUtils.createTextMessage(contact, message,
                Core.getInstance().getImService().getImdnManager().isImdnActivated());

        // Send message
        return sendChatMessage(msg);
    }


   public int resendMessage(String msgId) {
       if (logger.isActivated()) {
            logger.debug("resendMessage MsgId:" + msgId);
        }
        String message = RichMessagingHistory.getInstance().getMessageText(msgId);
        if (logger.isActivated()) {
            logger.debug("resendMessage message:" + message);
        }

        // Create a text message
        InstantMessage msg = ChatUtils.createTextMessage(this.contact, message,
                Core.getInstance().getImService().getImdnManager().isImdnActivated());
        msg.setMessageId(msgId);

        return getState(msgId);
   }

    /**
     * Sends a geoloc message
     *
     * @param geoloc Geoloc
     * @return Unique message ID or null in case of error
     */
    public String sendGeoloc(Geoloc geoloc) {
        if (logger.isActivated()) {
            logger.debug("Send geoloc message");
        }

        // Create a geoloc message
        GeolocPush geolocPush = new GeolocPush(geoloc.getLabel(),
                geoloc.getLatitude(), geoloc.getLongitude(),
                geoloc.getExpiration(), geoloc.getAccuracy());

        // Create a geoloc message
        GeolocMessage msg = ChatUtils.createGeolocMessage(contact, geolocPush,
                Core.getInstance().getImService().getImdnManager().isImdnActivated());

        // Send message
        return sendChatMessage(msg);
    }

    /**
     * Sends a chat message
     *
     * @param msg Message
     * @return Unique message ID or null in case of error
     */
    private String sendChatMessage(final InstantMessage msg) {
        synchronized(lock) {
            if (logger.isActivated()) {
                logger.debug("LMM Send chat message");
            }

            // Check if a session should be initiated or not
            if ((session == null) ||
                    session.getDialogPath().isSessionTerminated() ||
                    !session.getDialogPath().isSessionEstablished()) {
                try {
                    if (logger.isActivated()) {
                        logger.debug("ABC Core session is not yet established:"
                                + " initiate a new session to send the message");
                    }

                    // Initiate a new session
                    session = (OneOneChatSession)Core.getInstance().getImService()
                            .initiateOne2OneChatSession(contact, msg);

                    // Update with new session
                    setCoreSession(session);

                    // Update rich messaging history
                    if(! RichMessagingHistory.getInstance().isOne2OneMessageExists(
                            msg.getMessageId())){

                        RichMessagingHistory.getInstance().addChatMessage(
                                msg, ChatLog.Message.Direction.OUTGOING);
                    }
                    if(RcsSettings.getInstance().isSupportOP08()){
                   // mPendingMessage.add(msg);
                   // pendingMessage = msg.getTextMessage();
                   // pendingMessageId = msg.getMessageId();
                  }
                    // Start the session
                    Thread t = new Thread() {
                        public void run() {
                            session.startSession();
                        }
                    };
                    t.start();
                    return session.getFirstMessage().getMessageId();
                } catch(Exception e) {
                    if (logger.isActivated()) {
                        logger.error("ABC Can't send a new chat message", e);
                    }
                    return null;
                }
            } else {
                if (logger.isActivated()) {
                    logger.debug("ABC Core session is established:"
                            + " use existing one to send the message");
                }

                // Generate a message Id
                final String msgId = ChatUtils.generateMessageId();

                // Send message
                Thread t = new Thread() {
                    public void run() {
                        if (msg instanceof GeolocMessage) {
                            session.sendGeolocMessage(msgId, ((GeolocMessage)msg).getGeoloc());
                        } else {
                            session.sendTextMessage(msgId, msg.getTextMessage());
                        }
                    }
                };
                t.start();
                return msgId;
            }
        }
    }

    /**
     * Sends a displayed delivery report for a given message ID
     *
     * @param msgId Message ID
     */
    public void sendDisplayedDeliveryReport(final String msgId) {
        try {
            if (logger.isActivated()) {
                logger.info("LMM Set displayed delivery report for "
                        + msgId + "contact: " + session.getRemoteContact());
            }

            // Send delivery status
            if ((session != null) &&
                    (session.getDialogPath() != null) &&
                    (session.getDialogPath().isSessionEstablished()) &&
                    session.getMsrpMgr() != null &&
                    session.getMsrpMgr().getMsrpSession() != null &&
                    !session.isLargeMessageMode()) {
                // Send via MSRP
                Thread t = new Thread() {
                    public void run() {
                        session.sendMsrpMessageDeliveryStatus(
                                session.getRemoteContact(),
                                msgId,
                                ImdnDocument.DELIVERY_STATUS_DISPLAYED);
                    }
                };
                t.start();
            } else {
                String contact = SipUtils.extractUriFromAddress(session.getRemoteContact());
                if (logger.isActivated()) {
                    logger.info("LMM Set displayed delivery report for1 " + "contact: " + contact);
                }
                String subStr =  contact.substring(0,3);
                String newContact = null;
                if(!(subStr.equals("sip") || subStr.equals("tel"))){
                    newContact = "tel:" + contact ;
                }
                else{
                    newContact = contact;
                }
                if (logger.isActivated()) {
                    logger.info("LMM Set displayed delivery report for1 "
                            + "newContact: " + newContact + "subStr: " + subStr);
                }
                // Send via SIP MESSAGE
                Core.getInstance().getImService().getImdnManager().sendMessageDeliveryStatus(
                        newContact, msgId, ImdnDocument.DELIVERY_STATUS_DISPLAYED);
            }
        } catch(Exception e) {
            if (logger.isActivated()) {
                logger.error("LMM Could not send MSRP delivery status",e);
            }
        }
    }

    /**
     * Sends an is-composing event. The status is set to true when
     * typing a message, else it is set to false.
     *
     * @param status Is-composing status
     */
    public void sendIsComposingEvent(final boolean status) {
        if (logger.isActivated()) {
            logger.info("LMM sendIsComposingEvent " + session.isLargeMessageMode());
        }
        if (session != null && !session.isLargeMessageMode()) {
            Thread t = new Thread() {
                public void run() {
                    session.sendIsComposingStatus(status);
                }
            };
            t.start();
        }
    }

    /**
     * Adds a listener on chat events
     *
     * @param listener Chat event listener
     */
    public void addEventListener(IChatListener listener) {
        if (logger.isActivated()) {
            logger.info("LMM Add an event listener");
        }

        synchronized(lock) {
            listeners.register(listener);
        }
    }

    /**
     * Removes a listener on chat events
     *
     * @param listener Chat event listener
     */
    public void removeEventListener(IChatListener listener) {
        if (logger.isActivated()) {
            logger.info("LMM Remove an event listener");
        }

        synchronized(lock) {
            listeners.unregister(listener);
        }
    }

    /*------------------------------- SESSION EVENTS ----------------------------------*/

    /**
     * Session is started
     */
    public void handleSessionStarted() {
        synchronized(lock) {
            if (logger.isActivated()) {
                logger.info("LMM Session started");
            }

            // Update rich messaging history
            // Nothing done in database
        }
    }

    /**
     * Session has been aborted
     *
     * @param reason Termination reason
     */
    public void handleSessionAborted(int reason) {
        synchronized(lock) {
            String remoteContact = session.getRemoteContact();
            String number = PhoneUtils.extractNumberFromUri(remoteContact);
            if (logger.isActivated()) {
                logger.error("LMM handleSessionAborted: " + remoteContact + "Number: " + number);
            }

            // Update rich messaging history
            // Nothing done in database

            // Remove session from the list
            if(session.isStoreAndForward()){
                if (logger.isActivated()) {
                    logger.debug("LMM AddRemove remove storeChatSessions "
                            + session.getSessionID());
                }
                ChatServiceImpl.removeStoreChatSession(number);
            }
            else{
                if (logger.isActivated()) {
                    logger.debug("LMM AddRemove remove chatSessions " + session.getSessionID());
                }
                ChatServiceImpl.removeChatSession(number);
            }
        }
    }

    /**
     * Session has been terminated by remote
     */
    public void handleSessionTerminatedByRemote() {
        synchronized(lock) {

            String remoteContact = session.getRemoteContact();
            String number = PhoneUtils.extractNumberFromUri(remoteContact);
            if (logger.isActivated()) {
                logger.error("LMM handleSessionTerminatedByRemote: " + remoteContact + "Number: " + number);
            }

            // Update rich messaging history
            // Nothing done in database

            // Remove session from the list
            if(session.isStoreAndForward()){
                if (logger.isActivated()) {
                    logger.debug("LMM AddRemove remove storeChatSessions "
                            + session.getSessionID());
                }
                ChatServiceImpl.removeStoreChatSession(number);
            }
            else{
                if (logger.isActivated()) {
                    logger.debug("LMM AddRemove remove chatSessions " + session.getSessionID());
                }
                ChatServiceImpl.removeChatSession(number);
            }
        }
    }

    /**
     * New text message received
     *
     * @param text Text message
     */
    public void handleReceiveMessage(InstantMessage message) {
        synchronized(lock) {
            if (logger.isActivated()) {
                logger.info("LMM handleReceiveMessage New IM received: "
                        + message.getTextMessage());
            }

            // Update rich messaging history
            RichMessagingHistory.getInstance().addChatMessage(
                    message, ChatLog.Message.Direction.INCOMING);

            // Create a chat message
            ChatMessage msgApi = null;
            msgApi = new ChatMessage(
                        message.getMessageId(),
                        PhoneUtils.extractNumberFromUri(message.getRemote()),
                        message.getTextMessage(),
                        message.getServerDate(),
                        message.isImdnDisplayedRequested(),
                        message.getDisplayName());

            // Broadcast intent related to the received invitation
            Intent intent = new Intent(ChatIntent.ACTION_NEW_CHAT);
            intent.addFlags(Intent.FLAG_EXCLUDE_STOPPED_PACKAGES);
            intent.putExtra(ChatIntent.EXTRA_CONTACT, msgApi.getContact());
            intent.putExtra(ChatIntent.EXTRA_DISPLAY_NAME, session.getRemoteDisplayName());
            intent.putExtra(ChatIntent.EXTRA_MESSAGE, msgApi);
            AndroidFactory.getApplicationContext().sendBroadcast(intent);

            // Notify event listeners
            final int N = listeners.beginBroadcast();
            if (logger.isActivated()) {
                logger.info("LMM handleReceiveMessage N:" + N);
            }
            for (int i=0; i < N; i++) {
                try {
                    listeners.getBroadcastItem(i).onNewMessage(msgApi);
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
     * New geoloc message received
     *
     * @param geoloc Geoloc message
     */
    public void handleReceiveGeoloc(GeolocMessage geoloc) {
        synchronized(lock) {
            if (logger.isActivated()) {
                logger.info("New geoloc received");
            }

            // Update rich messaging history
            RichMessagingHistory.getInstance().addChatMessage(
                        geoloc, ChatLog.Message.Direction.INCOMING);

            // Create a geoloc message
            Geoloc geolocApi = new Geoloc(geoloc.getGeoloc().getLabel(),
                    geoloc.getGeoloc().getLatitude(), geoloc.getGeoloc().getLongitude(),
                    geoloc.getGeoloc().getExpiration());
            org.gsma.joyn.chat.GeolocMessage msgApi = new org.gsma.joyn.chat.GeolocMessage(
                    geoloc.getMessageId(),
                    PhoneUtils.extractNumberFromUri(geoloc.getRemote()),
                    geolocApi, geoloc.getDate(), geoloc.isImdnDisplayedRequested());

            // Broadcast intent related to the received invitation
            Intent intent = new Intent(ChatIntent.ACTION_NEW_CHAT);
            intent.addFlags(Intent.FLAG_EXCLUDE_STOPPED_PACKAGES);
            intent.putExtra(ChatIntent.EXTRA_CONTACT, msgApi.getContact());
            intent.putExtra(ChatIntent.EXTRA_DISPLAY_NAME, session.getRemoteDisplayName());
            intent.putExtra(ChatIntent.EXTRA_MESSAGE, msgApi);
            AndroidFactory.getApplicationContext().sendBroadcast(intent);

            // Notify event listeners
            final int N = listeners.beginBroadcast();
            for (int i=0; i < N; i++) {
                try {
                    listeners.getBroadcastItem(i).onNewGeoloc(msgApi);
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
     * IM session error
     *
     * @param error Error
     */
    public void handleImError(ChatError error) {
        synchronized(lock) {
            if (logger.isActivated()) {
                logger.error("LMM IM error " + error.getErrorCode() + "; status: " + error.getMessage());
            }
            String remoteContact = session.getRemoteContact();
            String number = PhoneUtils.extractNumberFromUri(remoteContact);
            String status = error.getMessage();

            if (logger.isActivated()) {
                logger.info("LMM handleImError: " + remoteContact + "Number: " + number);
            }

            // Update rich messaging history
            switch(error.getErrorCode()){
                case ChatError.SESSION_INITIATION_FAILED:
                case ChatError.SESSION_INITIATION_CANCELLED:
                    RichMessagingHistory.getInstance().updateChatMessageStatus(
                            session.getFirstMessage().getMessageId(),
                            ChatLog.Message.Status.Content.FAILED);
                    // TODO: notify listener
                    // Notify event listeners
                    final int N = listeners.beginBroadcast();
                    if (logger.isActivated()) {
                        logger.info("LMM handleImError N:" + N);
                    }
                    for (int i=0; i < N; i++) {
                        try {
                            if(status.contains("timeout") && RcsSettings.getInstance().isFallbackToPagerModeSupported()){
                                listeners.getBroadcastItem(i).onReportFailedMessage(session.getFirstMessage().getMessageId(),error.getErrorCode(),error.getMessage());
                            } else {
                                    listeners.getBroadcastItem(i).onReportMessageFailed(session.getFirstMessage().getMessageId());
                            }
                        } catch(Exception e) {
                            if (logger.isActivated()) {
                                logger.error("Can't notify listener", e);
                            }
                        }
                    }
                    listeners.finishBroadcast();
                    break;
                default:
                    break;
            }

            // Remove session from the list
            if(session.isStoreAndForward()){
                if (logger.isActivated()) {
                    logger.debug("LMM AddRemove remove storeChatSessions "
                            + session.getSessionID());
                }
                ChatServiceImpl.removeStoreChatSession(number);
            }
            else{
                if (logger.isActivated()) {
                    logger.debug("LMM AddRemove remove chatSessions " + session.getSessionID());
                }
                ChatServiceImpl.removeChatSession(number);
            }
        }
    }

/**
     * IM session error
     * 
     * @param error Error
     */
    public void handleInviteError(ChatError error) {
        synchronized(lock) {
            if (logger.isActivated()) {
                logger.info("LMM Invite error " + error.getErrorCode() );
            }
            String remoteContact = session.getRemoteContact();
            String number = PhoneUtils.extractNumberFromUri(remoteContact);

            if (logger.isActivated()) {
                logger.info("LMM handleInviteError: " + remoteContact + "Number: " + number);
            }
            
            final int N = listeners.beginBroadcast();
            
            // Update rich messaging history
            switch(error.getErrorCode()){
                case ChatError.SESSION_INITIATION_ERROR:
                    RichMessagingHistory.getInstance().updateChatMessageStatus(session.getFirstMessage().getMessageId(),
                            ChatLog.Message.Status.Content.FAILED);
                    // TODO: notify listener
                    // Notify event listeners
                    
                    if (logger.isActivated()) {
                        logger.info("LMM SESSION_INITIATION_ERROR N:" + N);
                    }
                    for (int i=0; i < N; i++) {
                        try {
                            listeners.getBroadcastItem(i).onReportMessageInviteFailed(session.getFirstMessage().getMessageId());
                        } catch(Exception e) {
                            if (logger.isActivated()) {
                                logger.error("Can't notify listener", e);
                            }
                        }
                    }
                    break;
                case ChatError.SESSION_FORBIDDEN_ERROR:
                    RichMessagingHistory.getInstance().updateChatMessageStatus(session.getFirstMessage().getMessageId(),
                            ChatLog.Message.Status.Content.FAILED);
                    // TODO: notify listener
                    // Notify event listeners
                    if (logger.isActivated()) {
                        logger.info("LMM SESSION_FORBIDDEN_ERROR N:" + N);
                    }
                    for (int i=0; i < N; i++) {
                        try {
                            listeners.getBroadcastItem(i).onReportMessageInviteForbidden(session.getFirstMessage().getMessageId(),error.getMessage());
                        } catch(Exception e) {
                            if (logger.isActivated()) {
                                logger.error("Can't notify listener", e);
                            }
                        }
                    }
                    break;
                default:
                    break;
            }
            
            // Remove session from the list
            if(session.isStoreAndForward()){
                if (logger.isActivated()) {
                    logger.debug("LMM AddRemove remove storeChatSessions " + session.getSessionID());
                }
                ChatServiceImpl.removeStoreChatSession(number);
            }
            else {
                if (logger.isActivated()) {
                    logger.debug("LMM AddRemove remove chatSessions " + session.getSessionID());
                }
                ChatServiceImpl.removeChatSession(number);
            }
            listeners.finishBroadcast();
        }
    }

    /**
     * Is composing event
     *
     * @param contact Contact
     * @param status Status
     */
    public void handleIsComposingEvent(String contact, boolean status) {
        synchronized(lock) {
            if (logger.isActivated()) {
                logger.info(contact + " is composing status set to " + status);
            }

            // Notify event listeners
            final int N = listeners.beginBroadcast();
            if (logger.isActivated()) {
                logger.info("LMM handleIsComposingEvent N:" + N);
            }
            for (int i=0; i < N; i++) {
                try {
                    listeners.getBroadcastItem(i).onComposingEvent(status);
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
     * New message delivery status
     *
     * @param msgId Message ID
     * @param status Delivery status
     */
    public void handleMessageDeliveryStatus(String msgId, String status, String contact) {
        synchronized(lock) {
            if (logger.isActivated()) {
                logger.info("LMM New message delivery status for message "
                        + msgId + ", status " + status);
            }

            // Update rich messaging history
            RichMessagingHistory.getInstance().updateChatMessageDeliveryStatus(msgId, status);

            // Notify event listeners
            final int N = listeners.beginBroadcast();
            if (logger.isActivated()) {
                logger.info("LMM handleMessageDeliveryStatus1 N:" + N);
            }
            for (int i=0; i < N; i++) {
                try {
                    if (status.equals(ImdnDocument.DELIVERY_STATUS_DELIVERED)) {
                        listeners.getBroadcastItem(i).onReportMessageDelivered(msgId);
                    } else
                    if (status.equals(ImdnDocument.DELIVERY_STATUS_DISPLAYED)) {
                        listeners.getBroadcastItem(i).onReportMessageDisplayed(msgId);
                    } else
                    if (status.equals(ImdnDocument.DELIVERY_STATUS_ERROR)) {
                        listeners.getBroadcastItem(i).onReportMessageFailed(msgId);
                    } else
                    if (status.equals(ImdnDocument.DELIVERY_STATUS_SENT)) {
                        listeners.getBroadcastItem(i).onReportMessageSent(msgId);
                    }
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
     * New message delivery status
     *
     * @param msgId Message ID
     * @param status Delivery status
     */
    public void handleMessageDeliveryStatus(
            String msgId, String status, String contact, int errorCode ,String statusCode) {
        synchronized(lock) {
            if (logger.isActivated()) {
                logger.error("New message delivery status for message " + msgId + ", status " + status + ", Statuscode:" + statusCode + ", errorcode:" + errorCode + " ,Contact:" + contact);
            }
            int codeError = 0;

            // Update rich messaging history
            RichMessagingHistory.getInstance().updateChatMessageDeliveryStatus(msgId, status);

            // Remove session from the list
            if(statusCode != null && statusCode.contains("403")){
                if(session.isStoreAndForward()){
                    if (logger.isActivated()) {
                        logger.debug("handleMessageDeliveryStatus Remove  storeChatSessions " + session.getSessionID());
                    }
                    ChatServiceImpl.removeStoreChatSession(contact);
                } else {
                    if (logger.isActivated()) {
                        logger.debug("handleMessageDeliveryStatus Remove  chatSessions " + session.getSessionID());
                    }
                    ChatServiceImpl.removeChatSession(contact);
                }
            }
            // Notify event listeners
            final int N = listeners.beginBroadcast();
            if (logger.isActivated()) {
                logger.info("LMM handleMessageDeliveryStatus0 N:" + N);
            }
            for (int i=0; i < N; i++) {
                try {
                    if (status.equals(ImdnDocument.DELIVERY_STATUS_ERROR) || status.equals(ImdnDocument.DELIVERY_STATUS_FAILED)) {
                        if (logger.isActivated()) {
                            logger.info("LMM handleMessageDeliveryStatus reportfailedmessage:");
                        }
                        listeners.getBroadcastItem(i).onReportFailedMessage(msgId,codeError,statusCode);
                    }
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
     * Conference event
     *
     * @param contact Contact
     * @param contactDisplayname Contact display name
     * @param state State associated to the contact
     */
    public void handleConferenceEvent(
            String contact, String contactDisplayname,
            String state, String method,
            String userStateParameter, String conferenceState) {
        // Not used here
    }

    /**
     * Request to add participant is successful
    */
    public void handleAddParticipantSuccessful() {
        // Not used in single chat
    }

    /**
     * Request to add participant has failed
     *
     * @param reason Error reason
     */
    public void handleAddParticipantFailed(String reason) {
        // Not used in single chat
    }

    @Override
    public void handleModifySubjectSuccessful(String subject) {
        // TODO Auto-generated method stub
    }

    @Override
    public void handleModifySubjectFailed(int statusCode) {
        // TODO Auto-generated method stub
    }

    @Override
    public void handleTransferChairmanSuccessful(String newChairman) {
        // TODO Auto-generated method stub
    }

    @Override
    public void handleTransferChairmanFailed(int statusCode) {
        // TODO Auto-generated method stub
    }

    @Override
    public void handleRemoveParticipantSuccessful(String removedParticipant){
        // TODO Auto-generated method stub
    }

    @Override
    public void handleRemoveParticipantFailed(int statusCode){
        // TODO Auto-generated method stub
    }

    @Override
    public void handleAbortConversationResult(int reason, int code) {
        // TODO Auto-generated method stub
    }

    @Override
    public void handleSessionTerminatedByGroupRemote(String cause, String text) {
        // TODO Auto-generated method stub
    }

    @Override
    public void handleQuitConversationResult(int code) {
        // TODO Auto-generated method stub
    }

    @Override
    public void handleModifySubjectByRemote(String subject) {
        // TODO Auto-generated method stub
    }

    @Override
    public void handleModifyNicknameSuccessful(String contact,
            String newNickName) {
        // TODO Auto-generated method stub
    }

    @Override
    public void handleModifyNicknameFailed(String contact, int statusCode) {
        // TODO Auto-generated method stub
    }

    @Override
    public void handleModifyNicknameByRemote(String contact, String newNickname) {
        // TODO Auto-generated method stub
    }

    @Override
    public void handleTransferChairmanByRemote(String newChairman) {
        // TODO Auto-generated method stub
    }

    @Override
    public void handleConferenceNotify(String confState, List<ConferenceUser> users) {
        // TODO Auto-generated method stub
    }
}
