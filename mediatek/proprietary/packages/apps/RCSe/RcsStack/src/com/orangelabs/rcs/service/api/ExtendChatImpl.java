package com.orangelabs.rcs.service.api;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;

import javax2.sip.SipException;
import javax2.sip.header.ContentDispositionHeader;
import javax2.sip.header.ContentLengthHeader;
import javax2.sip.header.ContentTypeHeader;

import org.gsma.joyn.chat.ChatIntent;
import org.gsma.joyn.chat.ChatLog;
import org.gsma.joyn.chat.ExtendChat;
import org.gsma.joyn.chat.ExtendMessage;
import org.gsma.joyn.chat.Geoloc;
import org.gsma.joyn.chat.IExtendChat;
import org.gsma.joyn.chat.IExtendChatListener;
import org.gsma.joyn.chat.ConferenceEventData.ConferenceUser;

import com.orangelabs.rcs.core.Core;
import com.orangelabs.rcs.core.CoreException;
import com.orangelabs.rcs.core.ims.ImsModule;
import com.orangelabs.rcs.core.ims.network.sip.FeatureTags;
import com.orangelabs.rcs.core.ims.network.sip.Multipart;
import com.orangelabs.rcs.core.ims.network.sip.SipMessageFactory;
import com.orangelabs.rcs.core.ims.network.sip.SipUtils;
import com.orangelabs.rcs.core.ims.protocol.sip.SipDialogPath;
import com.orangelabs.rcs.core.ims.protocol.sip.SipInterface;
import com.orangelabs.rcs.core.ims.protocol.sip.SipRequest;
import com.orangelabs.rcs.core.ims.protocol.sip.SipTransactionContext;
import com.orangelabs.rcs.core.ims.service.im.InstantMessagingService;
import com.orangelabs.rcs.core.ims.service.im.chat.ChatError;
import com.orangelabs.rcs.core.ims.service.im.chat.ChatSession;
import com.orangelabs.rcs.core.ims.service.im.chat.ChatSessionListener;
import com.orangelabs.rcs.core.ims.service.im.chat.ChatUtils;
import com.orangelabs.rcs.core.ims.service.im.chat.ContributionIdGenerator;
import com.orangelabs.rcs.core.ims.service.im.chat.GeolocMessage;
import com.orangelabs.rcs.core.ims.service.im.chat.GeolocPush;
import com.orangelabs.rcs.core.ims.service.im.chat.InstantMessage;
import com.orangelabs.rcs.core.ims.service.im.chat.StandaloneChatSession;
import com.orangelabs.rcs.core.ims.service.im.chat.cpim.CpimMessage;
import com.orangelabs.rcs.core.ims.service.im.chat.imdn.ImdnDocument;
import com.orangelabs.rcs.platform.AndroidFactory;
import com.orangelabs.rcs.provider.messaging.RichMessagingHistory;
import com.orangelabs.rcs.provider.settings.RcsSettings;
import com.orangelabs.rcs.service.LauncherUtils;
import com.orangelabs.rcs.utils.DateUtils;
import com.orangelabs.rcs.utils.PhoneUtils;
import com.orangelabs.rcs.utils.StringUtils;
import com.orangelabs.rcs.utils.logger.Logger;

import android.content.Intent;
import android.os.AsyncTask;
import android.os.RemoteCallbackList;

/**
* Chat implementation
*
 * @author Jean-Marc AUFFRET
*/
public class ExtendChatImpl extends IExtendChat.Stub implements ChatSessionListener {
    
    private final static String BOUNDARY_TAG = "jXfWUFcrCxZEXdN";

    /**
     * Remote contact
     */
    private List<String> contacts;
    
    private String contact;

    private InstantMessagingService imService;
    
    private SipInterface sipStack;
    
    private ChatServiceImpl chatService;

    /**
     * List of listeners
     */
    private RemoteCallbackList<IExtendChatListener> listeners = 
            new RemoteCallbackList<IExtendChatListener>();
    
    private CopyOnWriteArrayList<InstantMessage> pendingMsgList =
            new CopyOnWriteArrayList<InstantMessage>();
    
    private StandaloneChatSession session;

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
    public ExtendChatImpl(String contact, ChatServiceImpl chatService) {
        this.contacts = new ArrayList<String>();
        this.contacts.add(contact);
        this.contact = contact;
        this.imService = Core.getInstance().getImService();
        this.sipStack = Core.getInstance().getImsModule().getSipManager().getSipStack();

        this.chatService = chatService;
    }

    /**
     * Constructor
     *
     * @param contact Remote contact
     */
    public ExtendChatImpl(List<String> contacts, ChatServiceImpl chatService) {
        this.contacts = contacts;
        this.contact = PhoneUtils.generateContactsText(contacts);
        this.imService = Core.getInstance().getImService();
        this.sipStack = Core.getInstance().getImsModule().getSipManager().getSipStack();

        this.chatService = chatService;
    }

    public String getChatId(){
        return PhoneUtils.generateContactsText(contacts);
    }

    /**
     * Returns the remote contact
     *
     * @return Contact
     */
    public List<String> getRemoteContacts() {
        return contacts;
    }

    /**
     * Sends a message
     *
     * @param message message content
     * @param msgType specified message type, OP01 extension
     * @return Unique message ID or null in case of error
     */
    public String sendMessage(String message, int msgType) {
        if (logger.isActivated()) {
            logger.debug("Send message:" + message + " with msgType " + msgType);
        }

        int size = ChatUtils.getEncodedMessageSize(message);

        InstantMessage instantMessage = ChatUtils.createTextMessage(
                contact, message, imService.getImdnManager().isImdnActivated());
        instantMessage.setMessageType(msgType);

        String msgId = instantMessage.getMessageId();

        RichMessagingHistory.getInstance().addExtendChatMessage(
                instantMessage, ChatLog.Message.Direction.OUTGOING);

        int result;
        if (size < RcsSettings.getInstance().getMaxPagerContentSize())
            result = sendPagerMessage(instantMessage);
        else
            result = sendLargeMessage(instantMessage);

        if (result < 0) {
            if (logger.isActivated()) {
                logger.debug("send message fail");
            }
            RichMessagingHistory.getInstance().updateChatMessageDeliveryStatus(
                    msgId, ImdnDocument.DELIVERY_STATUS_FAILED);

            return null;
        }

        return msgId;
    }

    /**
     * Re-send message
     *
     * @param msgId message identifier
     * @return Unique message ID or null in case of error
     */
    public int resendMessage(String msgId) {
        if (logger.isActivated()) {
             logger.debug("resendMessage msgId:" + msgId);
        }
        
        RichMessagingHistory rmHistory = RichMessagingHistory.getInstance(); 
        String msgText = rmHistory.getMessageText(msgId);
        int msgType = rmHistory.getMessageType(msgId);

        if (msgText == null) {
            if (logger.isActivated()) {
                logger.debug("resend message fail->no message found");
            }
            return 0;
        }

        InstantMessage message = ChatUtils.createTextMessage(
                contact, msgText, imService.getImdnManager().isImdnActivated());
        message.setMessageType(msgType);
        message.setMessageId(msgId);

        int result;
        int size = ChatUtils.getEncodedMessageSize(msgText);
        if (size < RcsSettings.getInstance().getMaxPagerContentSize())
            result = sendPagerMessage(message);
        else
            result = sendLargeMessage(message);

        if (result < 0) {
            if (logger.isActivated()) {
                logger.debug("resend message fail");
            }
            rmHistory.updateChatMessageDeliveryStatus(
                    msgId, ImdnDocument.DELIVERY_STATUS_FAILED);
        }

        return 0;
    }
    
    public String prosecuteMessage(String msgId) {
        if (logger.isActivated()) {
            logger.debug("resendMessage msgId:" + msgId);
        }

        RichMessagingHistory rmHistory = RichMessagingHistory.getInstance(); 
        String msgText = rmHistory.getMessageText(msgId);
        
        if (msgText.isEmpty())
            return null;

        InstantMessage message = ChatUtils.createTextMessage(
                contact, msgText, imService.getImdnManager().isImdnActivated());

        List<String> contacts = rmHistory.getMessageParticipants(msgId);
        long timeStamp = rmHistory.getMessageTimeStamp(msgId); 
        
        String extraContent = 
                "Spam-From: " + ChatUtils.formatCpimSipUri(contacts.get(0)) + SipUtils.CRLF +
                "Spam-To: " + ImsModule.IMS_USER_PROFILE.getPublicUri() + SipUtils.CRLF +
                "DateTime: " + DateUtils.encodeDate(timeStamp);
        message.setExtraContent(extraContent);

        int result = 0;
        int size = ChatUtils.getEncodedMessageSize(msgText);
        if (size < RcsSettings.getInstance().getMaxPagerContentSize())
            result = sendPagerMessage(message);
        else
            result = sendLargeMessage(message);
        
        if (result < 0)
            return null;

        return message.getMessageId();
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

        GeolocPush geolocPush = new GeolocPush(geoloc.getLabel(),
                geoloc.getLatitude(), geoloc.getLongitude(),
                geoloc.getExpiration(), geoloc.getAccuracy());

        GeolocMessage msg = ChatUtils.createGeolocMessage(
                contact, geolocPush, imService.getImdnManager().isImdnActivated());

        if (sendLargeMessage(msg) >= 0)
            return msg.getMessageId();
        
        return null;
    }

    /**
     * Sends a Burn delivery report for a given message ID
     *
     * @param msgId Message ID
     */
    public void sendBurnedDeliveryReport(String msgId) {
        try {
            if (logger.isActivated()) {
                logger.info("Set displayed delivery report for " + msgId);
            }

            RichMessagingHistory rmHistory = RichMessagingHistory.getInstance();
            
            List<String> contacts = rmHistory.getMessageParticipants(msgId);
            int msgType = rmHistory.getMessageType(msgId);

            if (contacts != null && msgType == ChatLog.Message.Type.BURN) {
                String contact = contacts.get(0);
                if (logger.isActivated()) {
                    logger.info("Set burned delivery report for contact: " + contact);
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
                    logger.info("Set burned delivery report for newContact: " + newContact);
                }

                imService.getImdnManager().sendMessageDeliveryStatus(
                        newContact, msgId, ImdnDocument.BURN_STATUS_BURNED);
            }
        } catch(Exception e) {
            if (logger.isActivated()) {
                logger.error("Could not send delivery status",e);
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
                logger.info("LMM Set displayed delivery report for " + msgId);
            }

            RichMessagingHistory rmHistory = RichMessagingHistory.getInstance();
            
            List<String> contacts = rmHistory.getMessageParticipants(msgId);
            if (contacts != null) {
                String contact = contacts.get(0);
                if (logger.isActivated()) {
                    logger.info("LMM Set displayed delivery report for contact: " + contact);
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
                    logger.info("LMM Set displayed delivery report for newContact: " + newContact);
                }
                // Send via SIP MESSAGE
                imService.getImdnManager().sendMessageDeliveryStatus(
                        newContact, msgId, ImdnDocument.DELIVERY_STATUS_DISPLAYED);
            }
        } catch(Exception e) {
            if (logger.isActivated()) {
                logger.error("LMM Could not send MSRP delivery status",e);
            }
        }
    }

    /**
     * Adds a listener on chat events
     *
     * @param listener Chat event listener
     */
    public void addEventListener(IExtendChatListener listener) {
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
    public void removeEventListener(IExtendChatListener listener) {
        if (logger.isActivated()) {
            logger.info("LMM Remove an event listener");
        }

        synchronized(lock) {
            listeners.unregister(listener);
        }
    }

    private int sendPagerMessage(InstantMessage message) {
        int result = 0;

        String contentType = CpimMessage.MIME_TYPE;

        switch (message.getMessageType()) {
        case ChatLog.Message.Type.BURN:
            break;

        case ChatLog.Message.Type.PUBLIC:
            break;

        case ChatLog.Message.Type.CLOUD:
            contentType = CpimMessage.MIME_TYPE_CLOUDMESSAGE;
            break;

        case ChatLog.Message.Type.EMOTICON:
            contentType = CpimMessage.MIME_TYPE_PAYEMOTICON;
            break;
        }
        
        String content = composeRequestContent(message, contentType);
        
        if (contacts.size() > 1 || message.getExtraContent() != null)
            contentType = "multipart/mixed;" + "boundary=" + BOUNDARY_TAG;

        final SipRequest request = createSipMessageRequest(
                message.getMessageType(), contentType, content);

        if (request == null)
            return -1;
        
        final String msgId = message.getMessageId();
        new Thread() {
            public void run() {
                sendSipRequest(request, msgId);
            }
            
        }.start();
        
        return result;
    }

    private int sendLargeMessage(InstantMessage message) {

        synchronized (lock) {
            if (pendingMsgList.size() == 0) {
                sendLargeMessageImmdiately(message);
            }
            pendingMsgList.add(message);
        }

        return 0;
    }
    
    private int sendLargeMessageImmdiately(InstantMessage message) {
        ChatSession session = null;
        try {
            session = imService.initiateStandaloneChatSession(contacts, message);
        } catch (CoreException e) {
            e.printStackTrace();
            return -1;
        }
        this.session = (StandaloneChatSession)session;
        this.session.setExtraContent(message.getExtraContent());
        session.addListener(this);

        session.startSession();
        
        return 0;
    }

    private String composeRequestContent(InstantMessage message, String contentType) {
        String content;
        String contentPart;

        if (contentType.equals(CpimMessage.MIME_TYPE)) {
            contentPart = composeCpimContent(message);
        } else {
            // Raw content
            contentPart = message.getTextMessage();
        }

        String extraContent = message.getExtraContent();
        if (contacts.size() > 1 ||
            extraContent != null) {
            content = Multipart.BOUNDARY_DELIMITER + BOUNDARY_TAG + SipUtils.CRLF;
            
            if (extraContent != null) {
                content += ContentTypeHeader.NAME + ": text-plain;charset=UTF8" + SipUtils.CRLF +
                           ContentLengthHeader.NAME + ": " + extraContent.getBytes().length + SipUtils.CRLF +
                           SipUtils.CRLF +
                           extraContent + SipUtils.CRLF +
                           SipUtils.CRLF;
            }
            if (contacts.size() > 1) {
                String resourceList = ChatUtils.generateMultiChatResourceList(contacts);

                content += ContentTypeHeader.NAME + ": application/resource-lists+xml" + SipUtils.CRLF +
                           ContentDispositionHeader.NAME + ": recipient-list" + SipUtils.CRLF +
                           SipUtils.CRLF +
                           resourceList + SipUtils.CRLF +
                           SipUtils.CRLF;
            }
            content += Multipart.BOUNDARY_DELIMITER + BOUNDARY_TAG + SipUtils.CRLF +
                    ContentTypeHeader.NAME + ": " + contentType + SipUtils.CRLF +
                    SipUtils.CRLF +
                    contentPart +
                    SipUtils.CRLF +
                    Multipart.BOUNDARY_DELIMITER + BOUNDARY_TAG + Multipart.BOUNDARY_DELIMITER;
        } else {
            content = contentPart;
        }
        
        return content;
    }
    
    private String composeCpimContent(InstantMessage message) {
        String cpim;
        
        String from = ImsModule.IMS_USER_PROFILE.getPublicUri();
        String to = ChatUtils.ANOMYNOUS_URI;
        if (RcsSettings.getInstance().supportOP01()) {
            cpim = ChatUtils.buildCpimMessageWithDeliveredImdn(
                    from,
                    to,
                    message.getMessageId(),
                    StringUtils.encodeUTF8(message.getTextMessage()),
                    InstantMessage.MIME_TYPE);
        }
        else if (imService.getImdnManager().isImdnActivated()) {
            cpim = ChatUtils.buildCpimMessageWithImdn(
                    from,
                    to,
                    message.getMessageId(),
                    StringUtils.encodeUTF8(message.getTextMessage()),
                    InstantMessage.MIME_TYPE);
        }
        else {
            cpim = ChatUtils.buildCpimMessage(
                    from,
                    to,
                    StringUtils.encodeUTF8(message.getTextMessage()),
                    InstantMessage.MIME_TYPE);
        }

        return cpim;
    }

    private SipRequest createSipMessageRequest(
            int msgType, String contentType, String content) {
        ArrayList<String> featureTagList = new ArrayList<String>();
        String preferService = null;

        featureTagList.add(FeatureTags.FEATURE_RCSE_PAGER_MSG);

        switch (msgType) {
        case ChatLog.Message.Type.BURN:
            featureTagList.add(FeatureTags.FEATURE_CPM_BURNED_MSG);
            break;

        case ChatLog.Message.Type.PUBLIC:
            featureTagList.add(FeatureTags.FEATURE_CMCC_IARI_PUBLIC_ACCOUNT);
            preferService = FeatureTags.FEATURE_CMCC_URN_PUBLIC_ACCOUNT;
            break;

        case ChatLog.Message.Type.CLOUD:
            featureTagList.set(0, FeatureTags.FEATURE_CMCC_IARI_CLOUD_FILE);
            break;

        case ChatLog.Message.Type.EMOTICON:
            featureTagList.set(0, FeatureTags.FEATURE_CMCC_IARI_EMOTICON);
            break;
        }

        String remoteContact = contact;
        if (contacts.size() > 1)
            remoteContact = ImsModule.IMS_USER_PROFILE.getMultiImConferenceUri();
        else if (msgType == ChatLog.Message.Type.PUBLIC)
            remoteContact = PhoneUtils.formatUuidToSipUri(contact);
        else
            remoteContact = PhoneUtils.formatNumberToSipUri(contact);

        SipDialogPath dialogPath = new SipDialogPath(
                sipStack,
                sipStack.generateCallId(),
                1,
                remoteContact,
                ImsModule.IMS_USER_PROFILE.getPublicUri(),
                remoteContact,
                sipStack.getServiceRoutePath());

        try {
            String[] featureTags = new String[featureTagList.size()];
            for (int i = 0; i < featureTags.length; i++) {
                featureTags[i] = featureTagList.get(i);
            }
            SipRequest request = SipMessageFactory.createMessageCPM(
                        dialogPath, featureTags, preferService, contentType, content);

            SipUtils.buildAllowHeader(request.getStackMessage());

            String contribId = ContributionIdGenerator.getContributionId(dialogPath.getCallId());
            request.addHeader(ChatUtils.HEADER_CONTRIBUTION_ID, contribId);

            String convsId = RichMessagingHistory.getInstance().getCoversationID(contact, 1);
            if (convsId.isEmpty()) {
                String callId = sipStack.generateCallId();
                convsId = ContributionIdGenerator.getContributionId(callId);

                InstantMessage conversationMsg = ChatUtils.createTextMessage(remoteContact, "system", false);
                RichMessagingHistory.getInstance().addChatSystemMessage(
                        conversationMsg, ChatLog.Message.Direction.OUTGOING);
                RichMessagingHistory.getInstance().UpdateCoversationID(contact, convsId, 1);
            }
            request.addHeader(ChatUtils.HEADER_CONVERSATION_ID, convsId);
            
            return request;
        } catch (SipException e) {
            e.printStackTrace();
            return null;
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }
    
    private void sendSipRequest(SipRequest request, String msgId) {

        SipTransactionContext ctx = null;
        try {
            ctx = imService.getImsModule().getSipManager().sendSipMessageAndWait(request);
            // Analyze received message
            if ((ctx.getStatusCode() == 200) || (ctx.getStatusCode() == 202)) {
                // 200 OK received
                if (logger.isActivated()) {
                    logger.info("20x OK response received");
                }
                handleMessageDeliveryStatus(msgId, ImdnDocument.DELIVERY_STATUS_SENT, null);
            }  else {
                // Error responses
                if (logger.isActivated()) {
                    logger.info("failed: " + ctx.getStatusCode() + " response received");
                }
                handleMessageDeliveryStatus(msgId, ImdnDocument.DELIVERY_STATUS_ERROR, null);
            }
        } catch (Exception e) {
            if (logger.isActivated()) {
                logger.info("exception while sending pager message");
            }
            handleMessageDeliveryStatus(msgId, ImdnDocument.DELIVERY_STATUS_ERROR, null);
        }
    }

    /*------------------------------- SESSION EVENTS ----------------------------------*/

    /**
     * Session is started
     */
    public void handleSessionStarted() {

        if (logger.isActivated()) {
            logger.info("LMM handleSessionStarted");
        }

        synchronized(lock) {
            if (pendingMsgList.size() > 0) {
                final InstantMessage msg = pendingMsgList.get(0);

                new Thread() {

                    public void run() {
                        session.sendMessage(
                                msg.getMessageId(), msg.getTextMessage(), msg.getMessageType());
                    }

                }.start();
            }
        }
    }

    /**
     * Session has been aborted
     *
     * @param reason Termination reason
     */
    public void handleSessionAborted(int reason) {

        String chatId = session.getChatId();
        if (logger.isActivated()) {
            logger.info("LMM handleSessionAborted: " + chatId);
        }

        synchronized(lock) {
            pendingMsgList.remove(0);
            if (pendingMsgList.size() == 0)
                return;
            
            AsyncTask.execute(new Runnable() {
                @Override
                public void run() {
                    InstantMessage imMessage = pendingMsgList.get(0);
                    if (logger.isActivated()) {
                        logger.info("LMM handleSessionAborted " +
                                "text: " + imMessage.getTextMessage() +
                                "MsgId: " + imMessage.getMessageId());
                    }
                    sendLargeMessageImmdiately(imMessage);
                }
            });
        }
    }

    /**
     * Session has been terminated by remote
     */
    public void handleSessionTerminatedByRemote() {
        String chatId = session.getChatId();
        if (logger.isActivated()) {
            logger.info("LMM handleSessionAborted: " + chatId);
        }
        String msgId = session.getFirstMessage().getMessageId();
        RichMessagingHistory rmHistory = RichMessagingHistory.getInstance(); 
        rmHistory.updateChatMessageDeliveryStatus(msgId, ImdnDocument.DELIVERY_STATUS_FAILED);

        synchronized (lock) {
            final int N = listeners.beginBroadcast();
            for (int i=0; i < N; i++) {
                try {
                    listeners.getBroadcastItem(i).onReportMessageFailed(
                            msgId, ExtendChat.ErrorCodes.UNKNOWN, "");
                } catch(Exception e) {
                    if (logger.isActivated()) {
                        logger.error("Can't notify listener", e);
                    }
                }
            }
            listeners.finishBroadcast();
        }

        synchronized(lock) {
            pendingMsgList.remove(0);
            if (pendingMsgList.size() > 0) {
                AsyncTask.execute(new Runnable() {
                    @Override
                    public void run() {
                        InstantMessage imMessage = pendingMsgList.get(0);
                        if (logger.isActivated()) {
                            logger.info("LMM handleSessionAborted " +
                                    "text: " + imMessage.getTextMessage() +
                                    "MsgId: " + imMessage.getMessageId());
                        }
                        sendLargeMessageImmdiately(imMessage);
                    }
                });
            }
        }
    }

    /**
     * New text message received
     *
     * @param text Text message
     */
    public void handleReceiveMessage(InstantMessage message) {
        if (logger.isActivated()) {
            logger.info("LMM handleReceiveMessage New IM received: "
                    + message.getTextMessage());
        }

        RichMessagingHistory rmHistory = RichMessagingHistory.getInstance();
        
        rmHistory.addExtendChatMessage(message, ChatLog.Message.Direction.INCOMING);

        String number;
        if (message.getMessageType() == ChatLog.Message.Type.PUBLIC)
            number = PhoneUtils.extractUuidFromUri(message.getRemote());
        else
            number = PhoneUtils.extractNumberFromUri(message.getRemote());

        ExtendMessage msgApi = new ExtendMessage(
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
        intent.putExtra(ChatIntent.EXTRA_CONTACT, msgApi.getContact());
        intent.putExtra(ChatIntent.EXTRA_DISPLAY_NAME, msgApi.getDisplayName());
        intent.putExtra(ChatIntent.EXTRA_MESSAGE, msgApi);

        AndroidFactory.getApplicationContext().sendBroadcast(intent);

        synchronized(lock) {
            final int N = listeners.beginBroadcast();
            if (logger.isActivated()) {
                logger.info("LMM handleRecieveMessage N:" + N);
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

            chatService.receiveStandaloneChatMessage(msgApi);
        }
    }

    /**
     * IM session error
     *
     * @param error Error
     */
    public void handleImError(ChatError error) {
        if (logger.isActivated()) {
            logger.info("LMM IM error " + error.getErrorCode() );
        }

        RichMessagingHistory rmHistory = RichMessagingHistory.getInstance();
        String msgId = session.getFirstMessage().getMessageId();

        switch(error.getErrorCode()){
        case ChatError.SESSION_INITIATION_FAILED:
        case ChatError.SESSION_INITIATION_CANCELLED:
            rmHistory.updateChatMessageStatus(msgId, ChatLog.Message.Status.Content.FAILED);

            synchronized (lock) {
                final int N = listeners.beginBroadcast();
                if (logger.isActivated()) {
                    logger.info("LMM handleImError N:" + N);
                }
                for (int i=0; i < N; i++) {
                    try {
                        listeners.getBroadcastItem(i).onReportMessageFailed(
                                msgId, ExtendChat.ErrorCodes.INTERNAL, ""); 
                    } catch(Exception e) {
                        if (logger.isActivated()) {
                            logger.error("Can't notify listener", e);
                        }
                    }
                }
                listeners.finishBroadcast();
            }
            break;
        default:
            break;
        }

        synchronized(lock) {
            pendingMsgList.remove(0);
            if (pendingMsgList.size() > 0) {
                AsyncTask.execute(new Runnable() {
                    @Override
                    public void run() {
                        InstantMessage imMessage = pendingMsgList.get(0);
                        if (logger.isActivated()) {
                            logger.info("LMM handleIMError " +
                                "text: " + imMessage.getTextMessage() +
                                "MsgId: " + imMessage.getMessageId());
                        }
                        sendLargeMessageImmdiately(imMessage);
                    }
                });
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
                            listeners.getBroadcastItem(i).onReportMessageInviteError(session.getFirstMessage().getMessageId(),null,false);
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
                            listeners.getBroadcastItem(i).onReportMessageInviteError(session.getFirstMessage().getMessageId(),error.getMessage(),true);
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
        }
        listeners.finishBroadcast();
    }

    /**
     * Is composing event
     *
     * @param contact Contact
     * @param status Status
     */
    public void handleIsComposingEvent(String contact, boolean status) {
    }

     /**
     * New message delivery status
     *
     * @param msgId Message ID
     * @param status Delivery status
     */
    public void handleMessageDeliveryStatus(String msgId, String status, String contact) {
        if (logger.isActivated()) {
            logger.info("LMM New message delivery status for message " + msgId + ", status " + status);
        }

        // Update rich messaging history
        RichMessagingHistory.getInstance().updateChatMessageDeliveryStatus(msgId, status);

        // Notify event listeners
        synchronized (lock) {
            final int N = listeners.beginBroadcast();
            if (logger.isActivated()) {
                logger.info("LMM handleMessageDeliveryStatus1 N:" + N);
            }
            for (int i=0; i < N; i++) {
                try {
                    if (status.equals(ImdnDocument.DELIVERY_STATUS_DELIVERED)) {
                        listeners.getBroadcastItem(i).onReportMessageDelivered(msgId, contact);
                    } else
                    if (status.equals(ImdnDocument.DELIVERY_STATUS_DISPLAYED)) {
                        listeners.getBroadcastItem(i).onReportMessageDisplayed(msgId, contact);
                    } else
                    if (status.equals(ImdnDocument.DELIVERY_STATUS_ERROR) ||
                        status.equals(ImdnDocument.DELIVERY_STATUS_FAILED)) {
                        listeners.getBroadcastItem(i).onReportMessageFailed(
                                msgId, ExtendChat.ErrorCodes.UNKNOWN, "");
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
        if (logger.isActivated()) {
            logger.info("LMM New message delivery status for message " + msgId + ", status " + status);
        }
        int codeError = 0;

        // Update rich messaging history
        RichMessagingHistory.getInstance().updateChatMessageDeliveryStatus(msgId, status);
        switch(errorCode) {
        case ImdnDocument.TIMEOUT:
            codeError = ExtendChat.ErrorCodes.TIMEOUT;
            break;
        case ImdnDocument.INTERNAL_ERROR:
            codeError = ExtendChat.ErrorCodes.INTERNAL;
            break;
        case ImdnDocument.OUT_OF_SIZE:
            codeError = ExtendChat.ErrorCodes.OUTOFSIZE;
            break;
        case ImdnDocument.UNKNOWN:
            codeError = ExtendChat.ErrorCodes.UNKNOWN;
            break;
        default:
            codeError = ExtendChat.ErrorCodes.TIMEOUT;
            break;
        }

        // Notify event listeners
        synchronized (lock) {
            final int N = listeners.beginBroadcast();
            if (logger.isActivated()) {
                logger.info("LMM handleMessageDeliveryStatus0 N:" + N);
            }
            for (int i=0; i < N; i++) {
                try {
                    if (status.equals(ImdnDocument.DELIVERY_STATUS_ERROR) ||
                        status.equals(ImdnDocument.DELIVERY_STATUS_FAILED)) {
                        listeners.getBroadcastItem(i).onReportMessageFailed(
                                msgId, codeError, statusCode);
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
