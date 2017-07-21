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
package com.orangelabs.rcs.core.ims.service.im.filetransfer;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.text.ParseException;
import java.util.Vector;
import com.orangelabs.rcs.core.ims.rcsua.RcsUaAdapter;


import com.orangelabs.rcs.core.CoreException;
import javax.sip.header.ContentDispositionHeader;
import javax.sip.header.ContentLengthHeader;
import javax.sip.header.ContentTypeHeader;

import javax2.sip.header.ExtensionHeader;
import javax2.sip.header.Header;

import com.orangelabs.rcs.core.content.MmContent;
import com.orangelabs.rcs.core.ims.ImsModule;
import com.orangelabs.rcs.core.ims.network.sip.FeatureTags;
import com.orangelabs.rcs.core.ims.network.sip.Multipart;
import com.orangelabs.rcs.core.ims.network.sip.SipUtils;
import com.orangelabs.rcs.core.ims.protocol.msrp.MsrpEventListener;
import com.orangelabs.rcs.core.ims.protocol.msrp.MsrpManager;
import com.orangelabs.rcs.core.ims.protocol.msrp.MsrpSession;
import com.orangelabs.rcs.core.ims.protocol.sdp.MediaAttribute;
import com.orangelabs.rcs.core.ims.protocol.sdp.MediaDescription;
import com.orangelabs.rcs.core.ims.protocol.sdp.SdpParser;
import com.orangelabs.rcs.core.ims.protocol.sdp.SdpUtils;
import com.orangelabs.rcs.core.ims.protocol.sip.SipException;
import com.orangelabs.rcs.core.ims.protocol.sip.SipRequest;
import com.orangelabs.rcs.core.ims.protocol.sip.SipResponse;
import com.orangelabs.rcs.core.ims.service.ImsService;
import com.orangelabs.rcs.core.ims.service.ImsServiceError;
import com.orangelabs.rcs.core.ims.service.ImsServiceSession;
import com.orangelabs.rcs.utils.PhoneUtils;
import com.orangelabs.rcs.core.ims.service.im.InstantMessagingService;
import com.orangelabs.rcs.core.ims.service.im.chat.ChatUtils;
import com.orangelabs.rcs.core.ims.service.im.chat.ContributionIdGenerator;
import com.orangelabs.rcs.core.ims.service.im.chat.imdn.ImdnDocument;
import com.orangelabs.rcs.platform.file.FileFactory;
import com.orangelabs.rcs.provider.messaging.RichMessagingHistory;
import com.orangelabs.rcs.provider.settings.RcsSettings;
import com.orangelabs.rcs.service.LauncherUtils;
import com.orangelabs.rcs.service.api.FileTransferImpl;
import com.orangelabs.rcs.service.api.FileTransferServiceImpl;
import com.orangelabs.rcs.service.api.PauseResumeFileObject;
import com.orangelabs.rcs.utils.Base64;
import com.orangelabs.rcs.utils.NetworkRessourceManager;
import com.orangelabs.rcs.utils.logger.Logger;

import com.orangelabs.rcs.core.ims.security.cert.KeyStoreManager;
import com.orangelabs.rcs.core.ims.security.cert.KeyStoreManagerException;
import java.security.KeyStoreException;


/**
 * Originating file transfer session
 * 
 * @author jexa7410
 */
public class OriginatingFileSharingSession extends ImsFileSharingSession implements MsrpEventListener {
    /**
     * Boundary tag
     */
    private final static String BOUNDARY_TAG = "boundary1";
    
    /**
     * MSRP manager
     */
    private MsrpManager msrpMgr = null;
    
    /**
     * The logger
     */
    private Logger logger = Logger.getLogger(this.getClass().getName());

    private boolean isResumed = false;
    
    private long totalFileSize = 0;
    
    long bytesTransferredtoSkip = 0;

    /**
     * Constructor
     * 
     * @param parent IMS service
     * @param content Content to be shared
     * @param contact Remote contact
     * @param thumbnail Thumbnail
     */
    public OriginatingFileSharingSession(ImsService parent, MmContent content, String contact, byte[] thumbnail) {
        super(parent, content, contact, thumbnail);

        // Create dialog path
        createOriginatingDialogPath();

        // Set contribution ID
        String id = ContributionIdGenerator.getContributionId(getDialogPath().getCallId());
        setContributionID(id);    
        
        String newContact = PhoneUtils.extractNumberFromUri(contact);
        if (logger.isActivated()) {
            logger.info("CPMS OriginatingFileSharingSession newContact: " + newContact);
        }
        
        if (RcsSettings.getInstance().isCPMSupported()){
            if (logger.isActivated()) {
                logger.info("FTS OriginatingFileSharingSession old call id: " + getDialogPath().getCallId());
            }
            if (RichMessagingHistory.getInstance().getCoversationID(newContact,1).equals("")) {
                // Set Call-Id
                String callId = getImsService().getImsModule().getSipManager().getSipStack().generateCallId();
                if (logger.isActivated()) {
                    logger.info("FTS OriginatingFileSharingSession callId: " + callId);
                }
                String ConversationId = ContributionIdGenerator.getContributionId(callId);
                if (logger.isActivated()) {
                    logger.info("FTS OriginatingFileSharingSession setConversationID: " + ConversationId);
                }
                setConversationID(ConversationId);
                RichMessagingHistory.getInstance().UpdateCoversationID(newContact,ConversationId,1);
            } else {
                setConversationID(RichMessagingHistory.getInstance().getCoversationID(newContact,1));
            }
        }

        if (content != null && !isReceiveOnly) {
            totalFileSize = content.getSize();
        }
        if (logger.isActivated()) {
            logger.info("FTS OriginatingFileSharingSession content size: "
                    + totalFileSize);
        }
    }

    /**
     * Background processing
     */
    public void run() {
        try {
            if (logger.isActivated()) {
                logger.info("FTS Initiate a file transfer session as originating");
            }
            /**
             * M: Modified to resolve the 403 error issue.@{
             */
            SipRequest invite = createSipInvite();
            /**
             * @}
             */

            boolean isBurnMessage = isBurnMessage();
            /*
             * IF BURN MESSAGE
             */
            if (isBurnMessage) {
                logger.debug("FTS isBurnMessage() : " + isBurnMessage);
                try {
                    ExtensionHeader currHeader = (ExtensionHeader)invite.getHeader(
                                SipUtils.HEADER_ACCEPT_CONTACT);
                    Header header;
                    if (currHeader != null) {
                        String featureTags = currHeader.getValue();
                        logger.debug("old value : " + featureTags);
                        featureTags += ";";
                        featureTags += FeatureTags.FEATURE_CPM_BURNED_MSG;
                        header = SipUtils.HEADER_FACTORY.createHeader(
                                    SipUtils.HEADER_ACCEPT_CONTACT, featureTags);
                    } else {
                        header = SipUtils.HEADER_FACTORY.createHeader(
                                    SipUtils.HEADER_ACCEPT_CONTACT,
                                    FeatureTags.FEATURE_CPM_BURNED_MSG);
                    }
                    invite.getStackMessage().setHeader(header);
                } catch (ParseException e) {
                    // TODO Auto-generated catch block
                    e.printStackTrace();
                }
            }
            /*
             * @:BURN MESSAGE CHANGES END
            */


            // Send INVITE request
            sendInvite(invite);
        } catch (Exception e) {
            if (logger.isActivated()) {
                logger.error("FTS Session initiation has failed", e);
            }

           // Unexpected error
            handleError(new FileSharingError(FileSharingError.UNEXPECTED_EXCEPTION, e.getMessage()));
        }

        if (logger.isActivated()) {
            logger.debug("FTS End of thread");
        }
    }

    /**
     * M: Modified to resolve the 403 error issue.@{
     */
    /**
     * @return A sip invite request
     */

    protected SipRequest createSipInvite(String callId) {
        logger.debug("FTS createSipInvite(), callId = " + callId);
        createOriginatingDialogPath(callId);
        return createSipInvite();
    }

    private SipRequest createSipInvite() {
        logger.debug("FTS createSipInvite()");
            // Set setup mode
            String localSetup = createSetupOffer();
            if (logger.isActivated()){
                logger.debug("FTS Local setup attribute is " + localSetup);
            }

            // Set local port
            int localMsrpPort;
            if ("active".equals(localSetup)) {
                localMsrpPort = 9; // See RFC4145, Page 4
            } else {
                localMsrpPort = NetworkRessourceManager.generateLocalMsrpPort();
            }

            // Create the MSRP manager
            String localIpAddress  = null;
            if(!RcsSettings.getInstance().isSupportOP08()){
                localIpAddress = getImsService().getImsModule().getCurrentNetworkInterface().getNetworkAccess().getIpAddress();
            } else {
                localIpAddress  = RcsUaAdapter.getInstance().getVoLTEStackIPAddress();
            }      
            localIpAddress = ChatUtils.formatIPAddress(localIpAddress);
            msrpMgr = new MsrpManager(localIpAddress, localMsrpPort);
            if (getImsService().getImsModule().isConnectedToWifiAccess()) {
                msrpMgr.setSecured(RcsSettings.getInstance().isSecureMsrpOverWifi());
            }

            // Build SDP part
            String ntpTime = SipUtils.constructNTPtime(System.currentTimeMillis());
            
            String ipAddress = ChatUtils.formatIPAddress(getDialogPath().getSipStack().getLocalIpAddress());
            logger.debug(" getDialogPath().getSipStack().getLocalIpAddress(); after format: " +  ipAddress);
            
            String encoding = getContent().getEncoding();
            String sdp = null;
            String sendMode = "a=sendonly";
            String filetransferID = getSessionID();//getFileTransferId();
            PauseResumeFileObject pauseResumeObject = FileTransferServiceImpl.getPauseInfo(oldFileTransferId);
            if(!isReceiveOnly){
                if (logger.isActivated()){
                    logger.debug("FTS createSipInvite isReceiveOnly: " + isReceiveOnly() + "sendMode: sendOnly");
                }
                sendMode = "a=sendonly";
            } else{
                if (logger.isActivated()){
                    logger.debug("FTS createSipInvite isReceiveOnly: " + isReceiveOnly() + "sendMode: recvOnly" +"oldFileTransferId:" + pauseResumeObject.mOldFileTransferId
                            + "this.isFileTransferPaused() :" + this.isFileTransferPaused());
                }
                sendMode = "a=recvonly";
                if(this.isFileTransferPaused() && pauseResumeObject!= null)
                filetransferID = pauseResumeObject.mOldFileTransferId;
                
            }
            int timeLen = getTimeLen();
        if (logger.isActivated()) {
            logger.debug("FTS createSipInvite"
                    + " timeLen: " + timeLen
                    + " IsSecure: " + isSecureProtocolMessage()
                    + " IsPaused: " + this.isFileTransferPaused());
        }

        sdp =
            "v=0" + SipUtils.CRLF +
            "o=- " + ntpTime + " " + ntpTime + " "
                + SdpUtils.formatAddressType(ipAddress) + SipUtils.CRLF +
            "s=-" + SipUtils.CRLF +
            "c=" + SdpUtils.formatAddressType(ipAddress) + SipUtils.CRLF +
            "t=0 0" + SipUtils.CRLF +
            "m=message " + localMsrpPort + " " + msrpMgr.getLocalSocketProtocol() +
                " *" + SipUtils.CRLF +
            "a=path:" + msrpMgr.getLocalMsrpPath() + SipUtils.CRLF;
        if (isSecureProtocolMessage())
            sdp +=
            "a=fingerprint:" + KeyStoreManager.getFingerPrint() + SipUtils.CRLF;
        sdp +=
            "a=setup:" + localSetup + SipUtils.CRLF +
            "a=accept-types:" + encoding + SipUtils.CRLF;
        if (isGeoLocFile())
            sdp +=
            "a=accept-wrapped-types:" +encoding + SipUtils.CRLF;
        sdp +=
            "a=file-transfer-id:" + filetransferID + SipUtils.CRLF;
        if (timeLen > 0)
            sdp +=
            "a=file-disposition:timelen=" + timeLen + SipUtils.CRLF;
        sdp +=
            sendMode + SipUtils.CRLF;
 
        int maxSize = FileSharingSession.getMaxFileSharingSize();
        if (maxSize > 0) {
            sdp += "a=max-size:" + maxSize + SipUtils.CRLF;
        }
        if(pauseResumeObject == null) {
            pauseResumeObject = FileTransferServiceImpl.getPauseInfo(getSessionID());
            if (logger.isActivated()){
                logger.debug("FTS pauseResumeObject with oldFileTransferId  : " + oldFileTransferId + "pauseResumeObject is:" + pauseResumeObject );
            }
        }
        // Set File-selector attribute
        String selector = getFileSelectorAttribute();

        if (isReceiveOnly && pauseResumeObject!= null) {
            selector = "name:\"" + getContent().getName() + "\"" +
                    " type:" + getContent().getEncoding() +
                    " size:" + (pauseResumeObject.mSize - pauseResumeObject.bytesTransferrred) + " ";
            totalFileSize = pauseResumeObject.mSize;
            if (logger.isActivated()){
                logger.debug("FTS pauseResumeObject msize is  : " + totalFileSize );
            }
            getContent().setOut(pauseResumeObject.pausedStream);
        }
        if (this.isFileTransferPaused() && pauseResumeObject != null) {
            if (logger.isActivated()){
                logger.debug("FTS pauseResumeObject & hashselector : "
                        + pauseResumeObject + "selctor is:"
                        + pauseResumeObject.hashSelector);
            }
            hashselector = pauseResumeObject.hashSelector;
        } else {
            hashselector = getHashSelectorAttribute();
            PauseResumeFileObject pauseResumeObjectTemp = FileTransferServiceImpl
                    .getPauseInfo(getSessionID());
            if (pauseResumeObjectTemp != null) {
                pauseResumeObjectTemp.hashSelector = hashselector;
            }
            if (logger.isActivated()) {
                logger.debug("FTS First INVITE pauseResumeObject & " +
                        "hashselector : " + pauseResumeObjectTemp +
                        "selector is:" + hashselector +
                        "sessionID:" + getSessionID());
            }
        }
        long remainingSize = 0;
        if(RcsSettings.getInstance().supportOP01()){
            remainingSize = getContent().getSize();
        } else {
            remainingSize = getContent().getSize() - pauseResumeObject.bytesTransferrred;
        }
        if (this.isFileTransferPaused() && pauseResumeObject != null && pauseResumeObject.bytesTransferrred > 1 && !isReceiveOnly) {
                selector = "name:\"" + getContent().getName() + "\"" + 
                        " type:" + getContent().getEncoding() +
                        " size:" + remainingSize + " ";
                if (logger.isActivated()){
                    logger.debug("FTS pauseResumeObject pauseResumeObject.bytesTransferrred > 1 is  : " + pauseResumeObject.bytesTransferrred );
                }
        }
        if (selector != null) {
            sdp += "a=file-selector:" + selector + hashselector + SipUtils.CRLF;
        }

        long bytesTransferrred = 0;

        if(this.isFileTransferPaused() && pauseResumeObject!= null) {
            if (logger.isActivated()) {
                logger.info("File Resumed true while INVITE");
            }
            bytesTransferrred = pauseResumeObject.bytesTransferrred;
            bytesTransferredtoSkip = bytesTransferrred;
            if (logger.isActivated()) {
                logger.info("bytes transferred: " + bytesTransferrred + "Old TransferId: " + oldFileTransferId);
            }
            String fileRange = "a=file-range:" + (bytesTransferrred + 1) + "-" + totalFileSize;    
            if(isReceiveOnly && pauseResumeObject.pausedStream == null)
                fileRange = "a=file-range:" + 1 + "-" + totalFileSize; 
            sdp += fileRange + SipUtils.CRLF;
        }
        // Set File-location attribute
        String location = getFileLocationAttribute();
        if (location != null) {
            sdp += "a=file-location:" + location + SipUtils.CRLF;
        }

        if (getThumbnail() != null) {
            // Encode the thumbnail file
            String imageEncoded = Base64.encodeBase64ToString(getThumbnail());
            if (imageEncoded.length() < 10 * 1024 && imageEncoded.length() > 0){ // 10k is the maximum size of thumbnail as mentioned in RCS specs.
                sdp += "a=file-icon:cid:image@joyn.com" + SipUtils.CRLF;

                // Build multipart
                String multipart = 
                        Multipart.BOUNDARY_DELIMITER + BOUNDARY_TAG + SipUtils.CRLF +
                        ContentTypeHeader.NAME + ": application/sdp" + SipUtils.CRLF +
                        ContentLengthHeader.NAME + ": " + sdp.getBytes().length + SipUtils.CRLF +
                        SipUtils.CRLF +
                        sdp + SipUtils.CRLF + 
                        Multipart.BOUNDARY_DELIMITER + BOUNDARY_TAG + SipUtils.CRLF +
                        ContentTypeHeader.NAME + ": " + "image/jpeg" + SipUtils.CRLF +
                        SipUtils.HEADER_CONTENT_TRANSFER_ENCODING + ": base64" + SipUtils.CRLF +
                        SipUtils.HEADER_CONTENT_ID + ": <image@joyn.com>" + SipUtils.CRLF +
                        ContentLengthHeader.NAME + ": "+ imageEncoded.length() + SipUtils.CRLF +
                        ContentDispositionHeader.NAME + ": icon" + SipUtils.CRLF +
                        SipUtils.CRLF +
                        imageEncoded + SipUtils.CRLF;

                if (RcsSettings.getInstance().supportOP01() && isImdnSupport()) {
                    String from = "<" + ImsModule.IMS_USER_PROFILE.getPublicAddress();
                    String to = getRemoteContact();
                    final String msgId = ChatUtils.generateMessageId();
                    final boolean isPausedFile = this.fileTransferPaused;
                    Thread t = new Thread() {
                        public void run() {
                            if (!isPausedFile){
                                RichMessagingHistory.getInstance().updateFileTransferChatId(getSessionID(), null, msgId);
                            }
                        }
                    };
                    t.start();
                    // Send text message                    
                    String imdnData = buildCpimMessageWithImdn(from,to,msgId);
                    if (logger.isActivated()) {
                        logger.info("FTS createSipInvite IMDN data: " + imdnData);
                    }
                    multipart +=  SipUtils.CRLF + 
                            Multipart.BOUNDARY_DELIMITER + BOUNDARY_TAG + SipUtils.CRLF +                 
                            ContentTypeHeader.NAME + ": message/cpim" + SipUtils.CRLF + 
                            SipUtils.CRLF +
                            imdnData +
                            Multipart.BOUNDARY_DELIMITER + BOUNDARY_TAG + Multipart.BOUNDARY_DELIMITER;
                } else {
                    multipart += Multipart.BOUNDARY_DELIMITER + BOUNDARY_TAG + Multipart.BOUNDARY_DELIMITER;
                }

                // Set the local SDP part in the dialog path
                getDialogPath().setLocalContent(multipart);
            } else {
                if (logger.isActivated()) {
                    logger.info("FTS createSipInvite Thumbnail size is greater than 10K");
                }
                if (RcsSettings.getInstance().supportOP01() && isImdnSupport()) {
                    String from = "<" + ImsModule.IMS_USER_PROFILE.getPublicAddress()+">";
                    String to = getRemoteContact();
                    final String msgId = ChatUtils.generateMessageId();
                    final boolean isPausedFile = this.fileTransferPaused;
                    Thread t = new Thread() {
                        public void run() {
                            if(!isPausedFile){
                                RichMessagingHistory.getInstance().updateFileTransferChatId(getSessionID(), null, msgId);
                            }
                        }
                    };
                    t.start();
                    String imdnData = buildCpimMessageWithImdn(from,to,msgId);
                    if (logger.isActivated()) {
                        logger.info("FTS createSipInvite IMDN data: " + imdnData);
                    }
                    String multipart =
                            Multipart.BOUNDARY_DELIMITER + BOUNDARY_TAG + SipUtils.CRLF +
                            ContentTypeHeader.NAME + ": application/sdp" + SipUtils.CRLF +
                            ContentLengthHeader.NAME + ": " + sdp.getBytes().length + SipUtils.CRLF +
                            SipUtils.CRLF +
                            sdp + SipUtils.CRLF +
                            Multipart.BOUNDARY_DELIMITER + BOUNDARY_TAG + SipUtils.CRLF +
                            ContentTypeHeader.NAME + ": message/cpim" + SipUtils.CRLF +
                            SipUtils.CRLF +
                            imdnData +
                            ContentTypeHeader.NAME + ": text/plain" + SipUtils.CRLF +
                            ContentLengthHeader.NAME + ": " + "0" + SipUtils.CRLF +
                            SipUtils.CRLF +
                            Multipart.BOUNDARY_DELIMITER + BOUNDARY_TAG + Multipart.BOUNDARY_DELIMITER;

                    // Set the local SDP part in the dialog path
                    getDialogPath().setLocalContent(multipart);       
                } else {
                    // Set the local SDP part in the dialog path
                    getDialogPath().setLocalContent(sdp);
                }
            }
        } else {
            if(RcsSettings.getInstance().supportOP01() && isImdnSupport()) {
                String from = ImsModule.IMS_USER_PROFILE.getPublicAddress();
                String to = getRemoteContact();
                final String msgId = ChatUtils.generateMessageId();
                final boolean isPausedFile = this.fileTransferPaused;
                Thread t = new Thread() {
                    public void run() {
                        if (!isPausedFile){
                            RichMessagingHistory.getInstance().updateFileTransferChatId(getSessionID(), null, msgId);
                        }
                    }
                };
                t.start();
                String imdnData = buildCpimMessageWithImdn(from,to,msgId);
                if (logger.isActivated()) {
                    logger.info("FTS createSipInvite IMDN data: " + imdnData);
                }
                String multipart =
                        Multipart.BOUNDARY_DELIMITER + BOUNDARY_TAG + SipUtils.CRLF +
                        ContentTypeHeader.NAME + ": application/sdp" + SipUtils.CRLF +
                        ContentLengthHeader.NAME + ": " + sdp.getBytes().length + SipUtils.CRLF +
                        SipUtils.CRLF +
                        sdp + SipUtils.CRLF +
                        Multipart.BOUNDARY_DELIMITER + BOUNDARY_TAG + SipUtils.CRLF +
                        ContentTypeHeader.NAME + ": message/cpim" + SipUtils.CRLF +
                        SipUtils.CRLF +
                        imdnData +
                        ContentTypeHeader.NAME + ": text/plain" + SipUtils.CRLF +
                        ContentLengthHeader.NAME + ": " + "0" + SipUtils.CRLF +
                        SipUtils.CRLF +
                        Multipart.BOUNDARY_DELIMITER + BOUNDARY_TAG + Multipart.BOUNDARY_DELIMITER;

                // Set the local SDP part in the dialog path
                getDialogPath().setLocalContent(multipart);       
            } else {
                // Set the local SDP part in the dialog path
                getDialogPath().setLocalContent(sdp);
            }
        }
        try{
            // Create an INVITE request
            if (logger.isActivated()) {
                logger.info("Send INVITE");
            }
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
        logger.error("Create sip invite failed, return null.");
        return null;
    }

    /**
     * Send delivery report
     * 
     * @param status Report status
     */
    private void sendDeliveryReport(String status) {
        String messageId = RichMessagingHistory.getInstance().getFileMessageId(this.getSessionID());
        if (logger.isActivated()){
            logger.debug("Send delivery report " + messageId);
        }
        if (messageId != null) {
            if (logger.isActivated()){
                logger.debug("Send delivery report " + status);
            }            
            // Send message delivery status via a SIP MESSAGE
            ((InstantMessagingService) getImsService()).getImdnManager().sendMessageDeliveryStatus(getRemoteContact(), messageId, status);
        }
    }

    /**
     * Prepare media session
     * 
     * @throws Exception 
     */
    public void prepareMediaSession() throws Exception {
        if (logger.isActivated()) {
            logger.info("FTS prepareMediaSession isRecv: "
                    + this.isReceiveOnly() + " isPaused: "
                    + this.isFileTransferPaused());
        }
        SdpParser parser = new SdpParser(getDialogPath().getRemoteContent().getBytes());
        Vector<MediaDescription> media = parser.getMediaDescriptions();
        MediaDescription mediaDesc = media.elementAt(0);
        MediaAttribute attr = mediaDesc.getMediaAttribute("path");
        String remoteMsrpPath = attr.getValue();
        String remoteHost = SdpUtils.extractRemoteHost(
                parser.sessionDescription, mediaDesc);
        int remotePort = mediaDesc.port;
        // Extract the "setup" parameter
        String remoteSetup = "passive";
        MediaAttribute attr4 = mediaDesc.getMediaAttribute("setup");
        if (attr4 != null) {
            remoteSetup = attr4.getValue();
        }
        if (logger.isActivated()) {
            logger.debug("FTS Remote setup attribute is " + remoteSetup);
        }
        if(this.isReceiveOnly() && this.isFileTransferPaused()){
            if (logger.isActivated()) {
                logger.error("FTS No need to prepareMediaSession ");
            }
            String localSetup = createSetupAnswer(remoteSetup);
            if (logger.isActivated()) {
                logger.debug("FTS Local setup attribute is " + localSetup);
            }

            // Set local port
            int localMsrpPort;
            if (localSetup.equals("active")) {
                localMsrpPort = 9; // See RFC4145, Page 4
            } else {
                localMsrpPort = NetworkRessourceManager.generateLocalMsrpPort();
            }

            // Create the MSRP manager
            String localIpAddress = getImsService().getImsModule()
                    .getCurrentNetworkInterface().getNetworkAccess()
                    .getIpAddress();
            msrpMgr = new MsrpManager(localIpAddress, localMsrpPort);
            // msrpMgr.setSecured(isSecured);
            // Create the MSRP server session
            if (localSetup.equals("passive")) {
                // Passive mode: client wait a connection
                msrpMgr.createMsrpServerSession(remoteMsrpPath, this);
                // Open the connection
                Thread thread = new Thread() {
                    public void run() {
                        try {
                            // Open the MSRP session
                            msrpMgr.openMsrpSession(ImsFileSharingSession.DEFAULT_SO_TIMEOUT);
                            // Send an empty packet
                            sendEmptyDataChunk();
                        } catch (IOException e) {
                            if (logger.isActivated()) {
                                logger.error("FTS Can't create the MSRP server session", e);
                            }
                        }
                    }
                };
                thread.start();
            }

            // Create the MSRP client session
            if (localSetup.equals("active")) {
                // Active mode: client should connect
                msrpMgr.createMsrpClientSession(remoteHost, remotePort,
                        remoteMsrpPath, this);

                // Open the MSRP session
                msrpMgr.openMsrpSession(ImsFileSharingSession.DEFAULT_SO_TIMEOUT);

                // Send an empty packet
                sendEmptyDataChunk();
            }
             // The session is established
            getDialogPath().sessionEstablished();
            return;
        }
        // Parse the remote SDP part

        // Create the MSRP client session
        MsrpSession session = msrpMgr.createMsrpClientSession(remoteHost, remotePort, remoteMsrpPath, this);
        session.setFailureReportOption(true);
        session.setSuccessReportOption(false);
    }

    /**
     * Send an empty data chunk
     */
    public void sendEmptyDataChunk() {
        try {
            msrpMgr.sendEmptyChunk();
        } catch(Exception e) {
               if (logger.isActivated()) {
                   logger.error("FTS Problem while sending empty data chunk", e);
               }
        }
    }

    /**
     * Start media session
     * 
     * @throws Exception 
     */
    public void startMediaSession() throws Exception {
        if (logger.isActivated()) {
            logger.info("FTS startMediaSession isRecv: " + this.isReceiveOnly() + " isPaused: " + this.isFileTransferPaused());
        }
        if(this.isReceiveOnly() && this.isFileTransferPaused()){
            if (logger.isActivated()) {
                logger.error("No need to startMediaSession ");
            }
            return;
        }
        // Open the MSRP session
        msrpMgr.openMsrpSession();
        if(RcsSettings.getInstance().supportOP01()) {
            sendEmptyDataChunk();
        }

        try {
            // Start sending data chunks
            byte[] data = getContent().getData();
            InputStream stream; 
            if (data == null) {
                // Load data from URL
                stream = FileFactory.getFactory().openFileInputStream(getContent().getUrl());
            } else {
                // Load data from memory
                stream = new ByteArrayInputStream(data);
            }
            if(isFileTransferPaused())
            {                
                //bytesTransferredtoSkip = FileTransferServiceImpl.getPauseInfo(getSessionID()).bytesTransferrred;
                if(bytesToSkip != 0){
                    bytesTransferredtoSkip = bytesToSkip -1;
                }
                if(bytesTransferredtoSkip != 0)
                stream.skip(bytesTransferredtoSkip);
                if (logger.isActivated()) {
                    logger.error("startMediaSession resumed interrupted file byteskipped :" + bytesTransferredtoSkip);
                }
            }            
            msrpMgr.sendChunks(stream, ChatUtils.generateMessageId(), getContent().getEncoding(), getContent().getSize()-bytesTransferredtoSkip);
        } catch(Exception e) {
            // Unexpected error
            if (logger.isActivated()) {
                logger.error("Session initiation has failed", e);
            }
            handleError(new ImsServiceError(ImsServiceError.UNEXPECTED_EXCEPTION,
                    e.getMessage()));
        }
    }

    /**
     * Data has been transfered
     * 
     * @param msgId Message ID
     */
    public void msrpDataTransfered(String msgId) {

        String contact = this.getRemoteContact();
        if (logger.isActivated()) {
            logger.info("FTS Data transfered contact: " + contact + " msgId: " + msgId);
        }

        // File has been transfered
        fileTransfered();

        // Close the media session
        closeMediaSession();

        // Terminate session
        terminateSession(ImsServiceSession.TERMINATION_BY_USER);

        // Remove the current session
        getImsService().removeSession(this);

        // Notify listeners
        for(int j=0; j < getListeners().size(); j++) {
            ((FileSharingSessionListener)getListeners().get(j)).handleFileTransfered(getContent().getUrl());
        }
    }
    
    /**
     * Data transfer has been received
     * 
     * @param msgId Message ID
     * @param data Received data
     * @param mimeType Data mime-type 
     */
    public void msrpDataReceived(String msgId, byte[] data, String mimeType) {

        if (logger.isActivated()) {
            logger.info("FTS msrpDataReceived received");
        }
        
        // File has been transfered
        fileTransfered();
    
        // Send text message
        Thread t = new Thread() {
            public void run() {
                if (logger.isActivated()) {
                    logger.info("FTS Data received1");
                }
                if(RcsSettings.getInstance().supportOP01()) {
                    sendDeliveryReport(ImdnDocument.DELIVERY_STATUS_DELIVERED);
                }
            }
        };
        t.start();
    
        try {
            // Close content with received data
            getContent().writeData2File(data);
            getContent().closeFile();

            // Notify listeners
            for(int j=0; j < getListeners().size(); j++) {
                ((FileSharingSessionListener)getListeners().get(j)).handleFileTransfered(getContent().getUrl());
            }
           } catch(IOException e) {
           if (logger.isActivated()) {
                logger.error("Error delete incomplete file" + e);
            }
               // Delete the temp file
            deleteFile();

               // Notify listeners
            for(int j=0; j < getListeners().size(); j++) {
                ((FileSharingSessionListener)getListeners().get(j)).handleTransferError(new FileSharingError(FileSharingError.MEDIA_SAVING_FAILED));
            }
           } catch(Exception e) {
               // Delete the temp file
            deleteFile();

            // Notify listeners
            for(int j=0; j < getListeners().size(); j++) {
                ((FileSharingSessionListener)getListeners().get(j)).handleTransferError(new FileSharingError(FileSharingError.MEDIA_TRANSFER_FAILED));
            }
            if (logger.isActivated()) {
                   logger.error("FTS Can't save received file", e);
               }
           }
    
    }
    
     /**
     * Delete file
     */
    private void deleteFile() {
        if (logger.isActivated()) {
            logger.error("FTS Delete incomplete received file");
        }
        try {
            getContent().deleteFile();
        } catch (IOException e) {
            if (logger.isActivated()) {
                logger.error("FTS Can't delete received file", e);
            }
        }
    }
    
    /**
     * Data transfer in progress
     * 
     * @param currentSize Current transfered size in bytes
     * @param totalSize Total size in bytes
     */
    public void msrpTransferProgress(long currentSize, long totalSize) {
        // Notify listeners
        for(int j=0; j < getListeners().size(); j++) {
            ((FileSharingSessionListener)getListeners().get(j)).handleTransferProgress(currentSize, totalSize);
        }
        
        PauseResumeFileObject pauseResumeObject = FileTransferServiceImpl.getPauseInfo(getSessionID());
        if(pauseResumeObject != null) {
        pauseResumeObject.bytesTransferrred = currentSize;
        pauseResumeObject.hashSelector = hashselector;
        }
         if (logger.isActivated()) {
                 logger.info("FTS msrpTransferProgress bytes transferred: " + currentSize + "pauseResumeObject:" + pauseResumeObject + "FT TransferId: " + getSessionID()
                         + "hasselector: " + hashselector);
          }
    }    

    /**
     * Data transfer in progress
     *
     * @param currentSize Current transfered size in bytes
     * @param totalSize Total size in bytes
     * @param data received data chunk
     */
    public boolean msrpTransferProgress(long currentSize, long totalSize, byte[] data) {

        try {
            // Update content with received data
            getContent().writeData2File(data);

            PauseResumeFileObject pauseResumeObject = FileTransferServiceImpl.getPauseInfo(getSessionID());
            if(pauseResumeObject != null){
                if (logger.isActivated()) {
                    logger.info("FTS msrpTransferProgress pauseResumeObject : " + pauseResumeObject);
                }
                pauseResumeObject.bytesTransferrred = currentSize; 
                pauseResumeObject.pausedStream = getContent().getOut();
            }
            if (logger.isActivated()) {
                logger.info("FTS msrpTransferProgress bytes transferred: " + currentSize + " TransferId: " + getSessionID()
                        + "hasselector: ");
            }

            // Notify listeners
            for(int j = 0; j < getListeners().size(); j++) {
                ((FileSharingSessionListener) getListeners().get(j)).handleTransferProgress(currentSize, totalSize);
            }
        } catch(Exception e) {
            if (logger.isActivated()) {
                logger.info("FTS msrpTransferProgress exception " + e.getMessage());
            }
        }

        return true;

    }

    /**
     * Data transfer error
     *
     * @param msgId Message ID
     * @param error Error code
     */
    public void msrpTransferError(String msgId, String error) {
        if (isSessionInterrupted()) {
            return;
        }
        
        if (logger.isActivated()) {
            logger.info("Data transfer error " + error);
        }
        String statusCode = null;
        int errorCode = FileSharingError.MEDIA_TRANSFER_FAILED;
        try{
            statusCode = error.substring(12);
        } catch(Exception e){
            logger.error("msrpTransferError ", e);
        }

        try {
            // Terminate session
            if(statusCode != null && statusCode.contains("481")){
                terminateSession(ImsServiceSession.TERMINATION_BY_MSRP_ERROR);
                errorCode = FileSharingError.MEDIA_RESUME_FAILED;
            } else {
                terminateSession(ImsServiceSession.TERMINATION_BY_SYSTEM);
                errorCode = FileSharingError.MEDIA_TRANSFER_FAILED;
            }

            // Close the media session
            closeMediaSession();
        } catch(Exception e) {
            if (logger.isActivated()) {
                logger.error("Can't close correctly the file transfer session", e);
            }
        }

        // Request capabilities
        getImsService().getImsModule().getCapabilityService().requestContactCapabilities(getDialogPath().getRemoteParty());

        // Remove the current session
        getImsService().removeSession(this);

        // Notify listeners
        for(int j=0; j < getListeners().size(); j++) {
            ((FileSharingSessionListener)getListeners().get(j)).handleTransferError(new FileSharingError(errorCode, error));
        }
    }

    /**
     * Data transfer has been aborted
     */
    public void msrpTransferAborted() {
        if (logger.isActivated()) {
            logger.error("FTS Data transfer aborted");
        }
    }

    /**
     * Close media session
     */
    public void closeMediaSession() {
        // Close MSRP session
        if (msrpMgr != null) {
            msrpMgr.closeSession();
        }
        if (logger.isActivated()) {
            logger.debug("FTS MSRP session has been closed");
        }
    }
}
