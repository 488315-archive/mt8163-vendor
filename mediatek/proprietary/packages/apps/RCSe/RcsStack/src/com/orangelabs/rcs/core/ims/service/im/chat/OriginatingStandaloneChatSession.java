/*******************************************************************************
 * Software Name : RCS IMS Stack
 *
 * Copyright (C) 2010 France Telecom S.A.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/

package com.orangelabs.rcs.core.ims.service.im.chat;

import com.orangelabs.rcs.core.CoreException;
import com.orangelabs.rcs.core.ims.ImsModule;
import com.orangelabs.rcs.core.ims.network.sip.Multipart;
import com.orangelabs.rcs.core.ims.network.sip.SipMessageFactory;
import com.orangelabs.rcs.core.ims.network.sip.SipUtils;
import com.orangelabs.rcs.core.ims.protocol.sdp.SdpUtils;
import com.orangelabs.rcs.core.ims.protocol.sip.SipRequest;
import com.orangelabs.rcs.core.ims.service.ImsService;
import com.orangelabs.rcs.core.ims.service.ImsServiceSession;
import com.orangelabs.rcs.core.ims.service.im.chat.cpim.CpimMessage;
import com.orangelabs.rcs.core.ims.service.im.chat.geoloc.GeolocInfoDocument;
import com.orangelabs.rcs.core.ims.service.im.chat.imdn.ImdnDocument;
import com.orangelabs.rcs.core.ims.service.im.filetransfer.http.FileTransferHttpInfoDocument;
import com.orangelabs.rcs.service.LauncherUtils;
import com.orangelabs.rcs.utils.PhoneUtils;
import com.orangelabs.rcs.utils.StringUtils;
import com.orangelabs.rcs.provider.settings.RcsSettings;
import com.orangelabs.rcs.provider.messaging.RichMessagingHistory;
import com.orangelabs.rcs.core.ims.protocol.sip.SipException;
import com.orangelabs.rcs.utils.logger.Logger;

import javax2.sip.header.ContentDispositionHeader;
import javax2.sip.header.ContentLengthHeader;
import javax2.sip.header.ContentTypeHeader;

import com.orangelabs.rcs.core.ims.security.cert.KeyStoreManager;

import java.util.List;

import org.gsma.joyn.chat.ChatLog;


/**
 * Originating one-to-one chat session
 *
 * @author jexa7410
 */
public class OriginatingStandaloneChatSession extends StandaloneChatSession {
    
    /**
     * The logger
     */
    private Logger logger = Logger.getLogger(this.getClass().getName());

    /**
     * Constructor
     *
     * @param parent IMS service
     * @param contact Remote contact
     * @param msg First message of the session
     */
    public OriginatingStandaloneChatSession(
            ImsService parent,
            String target,
            List<String> contacts,
            InstantMessage msg,
            List<String> featureTags,
            String contentType,
            String preferService) {
        super(parent, target, contacts, featureTags);

        // Set first message
        setFirstMesssage(msg);
        this.contentType = contentType;
        this.preferService = preferService;

        // Create dialog path
        createOriginatingDialogPath();

        // Set contribution ID
        String id = ContributionIdGenerator.getContributionId(getDialogPath().getCallId());
        setContributionID(id);

        String contact = PhoneUtils.generateContactsText(contacts);
        String newContact = PhoneUtils.extractNumberFromUri(contact);
        if (logger.isActivated()) {
            logger.info("CPMS OriginatingStandaloneChatSession newContact: " + newContact);
        }

        RichMessagingHistory rmHistory = RichMessagingHistory.getInstance();
        if (RcsSettings.getInstance().isCPMSupported()) {
            String convsId = rmHistory.getCoversationID(newContact,1); 
            if (!convsId.isEmpty()) {
                setConversationID(convsId);
            } else {
                // Create a text message
                InstantMessage sysMessage = ChatUtils.createTextMessage(contact, "system", false);
                rmHistory.addChatSystemMessage(sysMessage, ChatLog.Message.Direction.OUTGOING);
                String callId = getImsService().getImsModule().getSipManager().getSipStack()
                        .generateCallId();

                // Set conversation ID
                String ConversationId = ContributionIdGenerator.getContributionId(callId);
                if (logger.isActivated()) {
                    logger.info("CPMS OriginatingStandaloneChatSession ConversationId: " +
                            ConversationId);
                }
                rmHistory.UpdateCoversationID(newContact, ConversationId, 1);
                setConversationID(ConversationId);
            }
        }
    }

    /**
     * Background processing
     */
    public void run() {
        try {
            if (logger.isActivated()) {
                logger.info("Initiate a new 1-1 chat session as originating");
            }
            /**
             * M: Modified to resolve the 403 error issue @{
             */
            SipRequest invite = createSipInvite();
            /**
             * @}
             */
            // Send INVITE request
            sendInvite(invite);
        } catch(Exception e) {
            if (logger.isActivated()) {
                logger.error("Session initiation has failed", e);
            }

            // Unexpected error
            handleError(new ChatError(ChatError.UNEXPECTED_EXCEPTION, e.getMessage()));
        }
    }

    /**
     * M: Modified to resolve the 403 error issue @{
     */
    /**
     * @return A sip invite request
     */
    protected SipRequest createSipInvite(String callId) {
        logger.debug("createSipInvite(), callId = " + callId);
        createOriginatingDialogPath(callId);
        return createSipInvite();
    }

    private SipRequest createSipInvite() {
        logger.debug("createSipInvite()");

        // If there is a first message then builds a multipart content else builds a SDP content
        InstantMessage msg = getFirstMessage();

        
        String content = composeRequestContent(msg, contentType);

        getDialogPath().setLocalContent(content);

        try {
            SipRequest invite = createInvite();

            // Set the Authorization header
            getAuthenticationAgent().setAuthorizationHeader(invite);

            // Set initial request in the dialog path
            getDialogPath().setInvite(invite);

            return invite;
        } catch (SipException e) {
            e.printStackTrace();
        } catch (CoreException e) {
            e.printStackTrace();
        }

        logger.error("Create sip invite failed, retrn null.");
        return null;
    }

    /**
     * Data has been transfered
     *
     * @param msgId Message ID
     */
    public void msrpDataTransfered(String msgId) {

        String contact = this.getRemoteContact();
        if (logger.isActivated()) {
            logger.info("Data transfered contact: " + contact);
        }

        if (msgId != null) {
            // Notify listeners
            for(int i=0; i < getListeners().size(); i++) {
                ((ChatSessionListener)getListeners().get(i)).handleMessageDeliveryStatus(
                        msgId, ImdnDocument.DELIVERY_STATUS_SENT, contact);
            }
        }

        // Close the media session
        closeMediaSession();

        // Terminate session
        terminateSession(ImsServiceSession.TERMINATION_BY_USER);

        // Remove the current session
        getImsService().removeSession(this);

        // Notify listeners
        for(int j=0; j < getListeners().size(); j++) {
            ((ChatSessionListener)getListeners().get(j)).handleSessionAborted(0);
        }
    }
    
    private String composeRequestContent(InstantMessage message, String contentType) {
        String content = "";
        String contentPart = null;

        if (message != null) {
            if (contentType.equals(CpimMessage.MIME_TYPE)) {
                contentPart = composeCpimContent(message);
            } else {
                // Raw content
                contentPart = message.getTextMessage();
            }
        }

        String sdp = composeSdpContent();
        List<String> contacts = getParticipants().getList();

        if (contacts.size() > 1 || contentPart != null || extraContent != null) {
            if (extraContent != null) {
                content +=  Multipart.BOUNDARY_DELIMITER + BOUNDARY_TAG + SipUtils.CRLF +
                        ContentTypeHeader.NAME + ": " + "text-plain;charset=UTF8" + SipUtils.CRLF +
                        ContentLengthHeader.NAME + ": " + extraContent.getBytes().length + SipUtils.CRLF +
                        SipUtils.CRLF +
                        extraContent + SipUtils.CRLF +
                        SipUtils.CRLF;
            }
            if (contacts.size() > 1) {
                String resourceList = ChatUtils.generateMultiChatResourceList(contacts);
                content += Multipart.BOUNDARY_DELIMITER + BOUNDARY_TAG + SipUtils.CRLF +
                            ContentTypeHeader.NAME + ": application/resource-lists+xml" + SipUtils.CRLF +
                            ContentDispositionHeader.NAME + ": recipient-list " + SipUtils.CRLF +
                            SipUtils.CRLF +
                            resourceList + SipUtils.CRLF +
                            SipUtils.CRLF;
            }
            content += Multipart.BOUNDARY_DELIMITER + BOUNDARY_TAG + SipUtils.CRLF +
                    ContentTypeHeader.NAME + ": application/sdp" + SipUtils.CRLF +
                    ContentLengthHeader.NAME + ": " + sdp.getBytes().length + SipUtils.CRLF +
                    SipUtils.CRLF +
                    sdp +
                    SipUtils.CRLF;

            if (contentPart != null) {
                content += Multipart.BOUNDARY_DELIMITER + BOUNDARY_TAG + SipUtils.CRLF +
                    ContentTypeHeader.NAME + ": "+ contentType + SipUtils.CRLF +
                    ContentLengthHeader.NAME + ": " + contentPart.getBytes().length + SipUtils.CRLF +
                    SipUtils.CRLF +
                    contentPart +
                    SipUtils.CRLF;
            }
            content += Multipart.BOUNDARY_DELIMITER + BOUNDARY_TAG + Multipart.BOUNDARY_DELIMITER;
        } else {
            content = sdp;
        }

        return content;
    }
    
    private String composeSdpContent() {
        String sdp;

        // Set setup mode
        String localSetup = createSetupOffer();
        if (logger.isActivated()){
            logger.debug("Local setup attribute is " + localSetup);
        }

        // Set local port
        int localMsrpPort;
        if ("active".equals(localSetup)) {
            localMsrpPort = 9; // See RFC4145, Page 4
        } else {
            localMsrpPort = getMsrpMgr().getLocalMsrpPort();
        }

        // Build SDP part
        String ntpTime = SipUtils.constructNTPtime(System.currentTimeMillis());
        String ipAddress = SdpUtils.formatAddressType(
                getDialogPath().getSipStack().getLocalIpAddress());
        logger.debug("isSecureProtocolMessage()");
        
        sdp =   "v=0" + SipUtils.CRLF +
                "o=- " + ntpTime + " " + ntpTime + " " + ipAddress + SipUtils.CRLF +
                "s=-" + SipUtils.CRLF +
                "c=" + ipAddress + SipUtils.CRLF +
                "t=0 0" + SipUtils.CRLF +
                "m=message " + localMsrpPort + " " + getMsrpMgr().getLocalSocketProtocol() +
                    " *" + SipUtils.CRLF +
                "a=path:" + getMsrpMgr().getLocalMsrpPath() + SipUtils.CRLF;

        if (isSecureProtocolMessage())
            sdp += 
                "a=fingerprint:" + KeyStoreManager.getFingerPrint() + SipUtils.CRLF; 
        
        sdp +=
                "a=setup:" + localSetup + SipUtils.CRLF +
                "a=accept-types:" + getAcceptTypes() + SipUtils.CRLF +
                "a=accept-wrapped-types:" + getWrappedTypes() + SipUtils.CRLF +
                "a=sendrecv" + SipUtils.CRLF;
        
        return sdp;
    }

    private String composeCpimContent(InstantMessage message) {
        String cpim;
        
        String from = ImsModule.IMS_USER_PROFILE.getPublicUri();
        String to = ChatUtils.ANOMYNOUS_URI;
        String textMsg;
        String mimeType;

        if (message instanceof GeolocMessage) {
            GeolocMessage geolocMsg = (GeolocMessage)message;
            textMsg = ChatUtils.buildGeolocDocument(
                        geolocMsg.getGeoloc(),
                        ImsModule.IMS_USER_PROFILE.getPublicUri(),
                        message.getMessageId());
            mimeType = GeolocInfoDocument.MIME_TYPE;
        } else
        if (message instanceof FileTransferMessage) {
            FileTransferMessage fileMsg = (FileTransferMessage)message;
            textMsg = fileMsg.getFileInfo();
            mimeType = FileTransferHttpInfoDocument.MIME_TYPE;
        } else {
            textMsg = message.getTextMessage();
            mimeType = InstantMessage.MIME_TYPE;
        }
        
        if (RcsSettings.getInstance().supportOP01()) {
            // Content will be sent by MSRP session
            cpim = ChatUtils.buildCpimMessageWithDeliveredImdn(
                    from,
                    to,
                    message.getMessageId(),
                    null,
                    mimeType);
        } else
        if (getImdnManager().isImdnActivated()) {
            cpim = ChatUtils.buildCpimMessageWithImdn(
                    from,
                    to,
                    message.getMessageId(),
                    StringUtils.encodeUTF8(textMsg),
                    mimeType);
        } else {
            cpim = ChatUtils.buildCpimMessage(
                    from,
                    to,
                    StringUtils.encodeUTF8(textMsg),
                    mimeType);
        }

        return cpim;
    }
}

 /**
 * @}
 */
