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

package com.orangelabs.rcs.core.ims.service.im;

import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;
import java.util.Vector;

import org.gsma.joyn.chat.ChatLog;
import org.gsma.joyn.ft.FileTransfer;
import org.gsma.joyn.ft.FileTransferLog;

import javax2.sip.header.ContactHeader;


import com.orangelabs.rcs.core.Core;
import com.orangelabs.rcs.core.CoreException;
import com.orangelabs.rcs.core.content.MmContent;
import com.orangelabs.rcs.core.ims.ImsModule;
import com.orangelabs.rcs.core.ims.network.sip.FeatureTags;
import com.orangelabs.rcs.core.ims.network.sip.SipMessageFactory;
import com.orangelabs.rcs.core.ims.network.sip.SipUtils;
import com.orangelabs.rcs.core.ims.protocol.sdp.MediaAttribute;
import com.orangelabs.rcs.core.ims.protocol.sdp.MediaDescription;
import com.orangelabs.rcs.core.ims.protocol.sdp.SdpParser;
import com.orangelabs.rcs.core.ims.protocol.sip.SipRequest;
import com.orangelabs.rcs.core.ims.protocol.sip.SipResponse;
import com.orangelabs.rcs.core.ims.service.ImsService;
import com.orangelabs.rcs.core.ims.service.ImsServiceSession;
import com.orangelabs.rcs.core.ims.service.capability.Capabilities;
import com.orangelabs.rcs.core.ims.service.im.chat.ChatSession;
import com.orangelabs.rcs.core.ims.service.im.chat.ChatUtils;
import com.orangelabs.rcs.core.ims.service.im.chat.GroupChatInfo;
import com.orangelabs.rcs.core.ims.service.im.chat.GroupChatSession;
import com.orangelabs.rcs.core.ims.service.im.chat.InstantMessage;
import com.orangelabs.rcs.core.ims.service.im.chat.ListOfParticipant;
import com.orangelabs.rcs.core.ims.service.im.chat.OneOneChatSession;
import com.orangelabs.rcs.core.ims.service.im.chat.OriginatingAdhocGroupChatSession;
import com.orangelabs.rcs.core.ims.service.im.chat.OriginatingClosedGroupChatSession;
import com.orangelabs.rcs.core.ims.service.im.chat.OriginatingOne2OneChatSession;
import com.orangelabs.rcs.core.ims.service.im.chat.OriginatingStandaloneChatSession;
import com.orangelabs.rcs.core.ims.service.im.chat.RejoinGroupChatSession;
import com.orangelabs.rcs.core.ims.service.im.chat.RestartGroupChatSession;
import com.orangelabs.rcs.core.ims.service.im.chat.TerminatingAdhocGroupChatSession;
import com.orangelabs.rcs.core.ims.service.im.chat.TerminatingOne2OneChatSession;
import com.orangelabs.rcs.core.ims.service.im.chat.TerminatingStandaloneChatSession;
import com.orangelabs.rcs.core.ims.service.im.chat.cpim.CpimMessage;
import com.orangelabs.rcs.core.ims.service.im.chat.cpim.CpimParser;
import com.orangelabs.rcs.core.ims.service.im.chat.imdn.ImdnDocument;
import com.orangelabs.rcs.core.ims.service.im.chat.imdn.ImdnManager;
import com.orangelabs.rcs.core.ims.service.im.chat.standfw.StoreAndForwardManager;
import com.orangelabs.rcs.core.ims.service.im.chat.standfw.TerminatingStoreAndForwardMsgSession;
import com.orangelabs.rcs.core.ims.service.im.filetransfer.FileSharingError;
import com.orangelabs.rcs.core.ims.service.im.filetransfer.FileSharingSession;
import com.orangelabs.rcs.core.ims.service.im.filetransfer.OriginatingExtendedFileSharingSession;
import com.orangelabs.rcs.core.ims.service.im.filetransfer.OriginatingFileSharingSession;
import com.orangelabs.rcs.core.ims.service.im.filetransfer.OriginatingGroupFileSharingSession; 
import com.orangelabs.rcs.core.ims.service.im.filetransfer.TerminatingGroupFileSharingSession;
import com.orangelabs.rcs.core.ims.service.im.filetransfer.TerminatingFileSharingSession;
import com.orangelabs.rcs.core.ims.service.im.filetransfer.http.FileTransferHttpInfoDocument;
import com.orangelabs.rcs.core.ims.service.im.filetransfer.http.OriginatingHttpFileSharingSession;
import com.orangelabs.rcs.core.ims.service.im.filetransfer.http.OriginatingHttpGroupFileSharingSession;
import com.orangelabs.rcs.core.ims.service.im.filetransfer.http.TerminatingHttpFileSharingSession;
import com.orangelabs.rcs.provider.eab.ContactsManager;
import com.orangelabs.rcs.provider.messaging.RichMessagingHistory;
import com.orangelabs.rcs.provider.settings.RcsSettings;
import com.orangelabs.rcs.provider.settings.RcsSettingsData;
import com.orangelabs.rcs.service.LauncherUtils;
import com.orangelabs.rcs.service.api.FileTransferServiceImpl;
import com.orangelabs.rcs.service.api.PauseResumeFileObject;
import com.orangelabs.rcs.utils.Base64;
import com.orangelabs.rcs.utils.IdGenerator;
import com.orangelabs.rcs.utils.PhoneUtils;
import com.orangelabs.rcs.utils.StringUtils;
import com.orangelabs.rcs.utils.logger.Logger;

/**
* Instant messaging services (1-1 chat, group chat and file transfer)
* 
 * @author Jean-Marc AUFFRET
*/
public class InstantMessagingService extends ImsService {
    /**
     * Chat features tags
     */
    public final static String[] CHAT_FEATURE_TAGS = { FeatureTags.FEATURE_OMA_IM };

    /**
     * Chat features tags
     */
    public final static String[] CPM_CHAT_FEATURE_TAGS = { FeatureTags.FEATURE_RCSE_CPM_SESSION };

    /**
     * Chat features tags
     */
    public final static String[] CPM_LARGE_MESSAGE_FEATURE_TAGS = { FeatureTags.FEATURE_RCSE_LARGE_MSG };

    /**
     * File transfer features tags
     */
    public final static String[] FT_FEATURE_TAGS = { FeatureTags.FEATURE_OMA_IM };

    /**
     * File transfer features tags
     */
    public final static String[] CPM_FT_FEATURE_TAGS = { FeatureTags.FEATURE_RCSE_CPM_FT };

    /**
     * Max chat sessions
     */
    private int maxChatSessions;

    /**
     * Max file transfer sessions
     */
    private int maxFtSessions;
                
    /**
     * Max file transfer size
     */
    private int maxFtSize;

    /**
     * IMDN manager
     */
    private ImdnManager imdnMgr = null;   
                
    /**
     * Store & Forward manager
     */
    private StoreAndForwardManager storeAndFwdMgr = new StoreAndForwardManager(this);

    /**
     * The logger
     */
    private Logger logger = Logger.getLogger(this.getClass().getName());

    /**
     * Constructor
     * 
     * @param parent IMS module
     * @throws CoreException
     */
    public InstantMessagingService(ImsModule parent) throws CoreException {
        super(parent, true);

        this.maxChatSessions = RcsSettings.getInstance().getMaxChatSessions();
        this.maxFtSessions = RcsSettings.getInstance().getMaxFileTransferSessions();
        this.maxFtSize = FileSharingSession.getMaxFileSharingSize();
    }

    /**
     * Start the IMS service
     */
    public synchronized void start() {
        if (isServiceStarted()) {
            // Already started
            return;
        }
        setServiceStarted(true);
                                
        // Start IMDN manager
        imdnMgr = new ImdnManager(this);
        imdnMgr.start();
    }

    /**
     * Stop the IMS service
     */
    public synchronized void stop() {
        if (!isServiceStarted()) {
            // Already stopped
            return;
        }
        setServiceStarted(false);
                                
        // Stop IMDN manager
        imdnMgr.terminate();
        imdnMgr.interrupt();
    }

    /**
     * Check the IMS service
     */
    public void check() {
    }

    /**
     * Returns the IMDN manager
     * 
     * @return IMDN manager
     */
    public ImdnManager getImdnManager() {
        return imdnMgr;
    }              

    /**
     * Get Store & Forward manager
     */
    public StoreAndForwardManager getStoreAndForwardManager() {
        return storeAndFwdMgr;
    }

    /**
     * Returns IM sessions
     * 
     * @return List of sessions
     */
    public Vector<ChatSession> getImSessions() {
        // Search all IM sessions
        Vector<ChatSession> result = new Vector<ChatSession>();
        Enumeration<ImsServiceSession> list = getSessions();
        while(list.hasMoreElements()) {
            ImsServiceSession session = list.nextElement();
            if (session instanceof ChatSession) {
                result.add((ChatSession)session);
            }
        }

        return result;
    }

    /**
     * Returns IM sessions with a given contact
     * 
     * @param contact Contact
     * @return List of sessions
     */
    public Vector<ChatSession> getImSessionsWith(String contact) {
        // Search all IM sessions
        Vector<ChatSession> result = new Vector<ChatSession>();
        Enumeration<ImsServiceSession> list = getSessions();
        while(list.hasMoreElements()) {
            ImsServiceSession session = list.nextElement();
            if ((session instanceof OneOneChatSession) && PhoneUtils.compareNumbers(session.getRemoteContact(), contact)) {
                result.add((ChatSession)session);
            }
        }

        return result;
    }

    /**
     * Returns IM sessions with a list of contact
     * 
     * @param contact Contact
     * @return List of sessions
     */
    public Vector<ChatSession> getImSessionsWith(List<String> contacts) {
        // Search all IM sessions
        Vector<ChatSession> result = new Vector<ChatSession>();
        Enumeration<ImsServiceSession> list = getSessions();
        loopThroughSession:while(list.hasMoreElements()) {
            ImsServiceSession session = list.nextElement();
            if (session instanceof GroupChatSession) {
                List<String> connectedParticipants = ((GroupChatSession) session).getConnectedParticipants().getList();

                if(contacts.size() != connectedParticipants.size())
                {
                    continue;
                }

                for(String contact : contacts)
                {
                    if(!connectedParticipants.contains(contact))
                    {
                        continue loopThroughSession;
                    }
                }
                result.add((ChatSession)session);
            }
        }

        return result;
    }

    /**
     * Returns file transfer sessions with a given contact
     * 
     * @param contact Contact
     * @return List of sessions
     */
    public Vector<FileSharingSession> getFileTransferSessionsWith(String contact) {
        Vector<FileSharingSession> result = new Vector<FileSharingSession>();
        Enumeration<ImsServiceSession> list = getSessions();
        while(list.hasMoreElements()) {
            ImsServiceSession session = list.nextElement();
            if ((session instanceof FileSharingSession) && PhoneUtils.compareNumbers(session.getRemoteContact(), contact)) {
                result.add((FileSharingSession)session);
            }
        }

        return result;
    }

    /**
     * Returns file transfer sessions with a given fileTransferId
     * 
     * @param contact Contact
     * @return List of sessions
     */
    public FileSharingSession getFileTransferSessionsWithId(String ftSessionId) {
        FileSharingSession result = null;
        Enumeration<ImsServiceSession> list = getSessions();
        while(list.hasMoreElements()) {
            ImsServiceSession session = list.nextElement();
            if ((session instanceof FileSharingSession) && session.getSessionID().equals(ftSessionId)) {
                return result;
            }
        }

        return result;
    }

    /**
     * Returns active file transfer sessions
     * 
     * @return List of sessions
     */
    public Vector<FileSharingSession> getFileTransferSessions() {
        Vector<FileSharingSession> result = new Vector<FileSharingSession>();
        Enumeration<ImsServiceSession> list = getSessions();
        while(list.hasMoreElements()) {
            ImsServiceSession session = list.nextElement();
            if (session instanceof FileSharingSession) {
                result.add((FileSharingSession)session);
            }
        }

        return result;
    }

    /**
     * Initiate a file transfer session
     * 
     * @param contact Remote contact
     * @param content Content to be sent
     * @param thumbnail Thumbnail filename
     * @param chatSessionId Chat session ID
     * @param chatContributionId Chat contribution Id
     * @return File transfer session
     * @throws CoreException
     */
    public FileSharingSession initiateFileTransferSession(String contact, MmContent content, String thumbnail, String chatSessionId, String chatContributionId) throws CoreException {
        if (logger.isActivated()) {
            logger.info("FTS Initiate a file transfer session with contact " + contact + ", file " + content.toString());
        }
        if (logger.isActivated()) {
            logger.info("FTS initiateFileTransferSession chatSessionId: " + chatSessionId + ", chatContributionId " + chatContributionId);
        }

        // Test number of sessions
        if ((maxFtSessions != 0) && (getFileTransferSessions().size() >= maxFtSessions)) {
            if (logger.isActivated()) {
                logger.debug("FTS The max number of file transfer sessions is achieved: cancel the initiation");
            }
            throw new CoreException("FTS Max file transfer sessions achieved");
        }

        // Test max size
        if (maxFtSize > 0 && content.getSize() > maxFtSize) {
            if (logger.isActivated()) {
                logger.debug("FTS File exceeds max size: cancel the initiation");
            }
            throw new CoreException("FTS File exceeds max size");
        }

        // Check contact capabilities
        boolean isFToHttpSupportedByRemote = false;
        Capabilities capability = ContactsManager.getInstance().getContactCapabilities(contact);
        Capabilities myCapability = RcsSettings.getInstance().getMyCapabilities();

        if(capability == null || myCapability == null) {
            if (logger.isActivated()) {
                logger.debug("initiateFileTransferSession capability is null");
            }
        }

        if (logger.isActivated() && myCapability != null && capability != null) {
            logger.debug("FTS initiateFileTransferSession remote capability isFTThumbnail: " + capability.isFileTransferThumbnailSupported() + "isFTHTTP: " + capability.isFileTransferHttpSupported());
            logger.debug("FTS initiateFileTransferSession self capability isFTThumbnail: " + myCapability.isFileTransferThumbnailSupported()+ "isFTHTTP: " + myCapability.isFileTransferHttpSupported());
        }
                                
        if (capability != null) {
            isFToHttpSupportedByRemote = capability.isFileTransferHttpSupported();
        }

        // Get thumbnail data
        byte[] thumbnailData = null;
        if (thumbnail != null && capability != null && capability.isFileTransferThumbnailSupported()) {
            if (logger.isActivated()) {
                logger.debug("FTS initiateFileTransferSession get thumbnail");
            }
            // Create the thumbnail
            thumbnailData = ChatUtils.getFileThumbnail(thumbnail);
            if (logger.isActivated()) {
                logger.debug("FTS Created thumbnail0 is: " + thumbnailData);
            }
        }
        else{
            if(RcsSettings.getInstance().supportOP01() ||
               (RcsSettings.getInstance().isFileTransferThumbnailSupported() && capability != null && capability.isFileTransferThumbnailSupported())){
                                                                
                thumbnailData = ChatUtils.createFileThumbnail(content.getUrl());
                // Create the thumbnail
                if (logger.isActivated()) {
                    logger.debug("FTS initiateFileTransferSession create thumbnail" + thumbnailData);
                }
                if (thumbnailData == null || thumbnailData.length == 0)
                    thumbnailData = ChatUtils.createVideoThumbnail(content.getUrl());
                if (logger.isActivated()) {
                    logger.debug("FTS Created thumbnail is: " + thumbnailData);
                }
            }
        }

        //thumbnailData = ChatUtils.createFileThumbnail(content.getUrl());

        if(thumbnailData != null){
            if (logger.isActivated()) {
                logger.debug("FTS length of thumbanil: " + thumbnailData.length );
            }
        }

        // Select default protocol
        boolean isHttpProtocol = false;
        if (isFToHttpSupportedByRemote && myCapability.isFileTransferHttpSupported() && !myCapability.isFileTransferSupported()) {
            if (RcsSettings.getInstance().getFtProtocol().equals(RcsSettingsData.FT_PROTOCOL_HTTP)) {
                isHttpProtocol = true;
            }
        }
        if (logger.isActivated()) {
            logger.info("FTS isHttpProtocol: " + isHttpProtocol);
        }

        // Initiate session
        FileSharingSession session;
        //isHttpProtocol = true;
        isHttpProtocol = false;
                    if (logger.isActivated()) {
                        logger.info("FTS isHttpProtocol1: " + isHttpProtocol);
                    }
        if (isHttpProtocol) {
            // Create a new session
            session = new OriginatingHttpFileSharingSession(
                    this,
                    content,
                    PhoneUtils.formatNumberToSipUri(contact),
                    thumbnailData,
                    chatSessionId,
                    chatContributionId);
        } else {
            // Create a new session
            session = new OriginatingFileSharingSession(
                    this,
                    content,
                    PhoneUtils.formatNumberToSipUri(contact),
                    thumbnailData);
        }

        return session;
    }

    /**
     * Initiate a file transfer session
     * 
     * @param contact Remote contact
     * @param content Content to be sent
     * @param thumbnail Thumbnail filename
     * @return File transfer session
     * @throws CoreException
     */
    public FileSharingSession initiateExtendedTransferSession(
            String chatId,
            List<String> contacts,
            MmContent content,
            String thumbnail,
            int duration,
            int type) throws CoreException {
        if (logger.isActivated()) {
            logger.info("FTS Initiate file transfer session with contacts " + contacts);
        }

        // Test number of sessions
        if ((maxFtSessions != 0) && (getFileTransferSessions().size() >= maxFtSessions)) {
            if (logger.isActivated()) {
                logger.debug("FTS The max number of file transfer sessions is achieved: cancel the initiation");
            }
            throw new CoreException("FTS Max file transfer sessions achieved");
        }

        // Test max size
        if (maxFtSize > 0 && content.getSize() > maxFtSize) {
            if (logger.isActivated()) {
                logger.debug("FTS File exceeds max size: cancel the initiation");
            }
            throw new CoreException("FTS File exceeds max size");
        }

        boolean httpSupported = false;
        boolean thumbnailSupported = false;

        if (RcsSettings.getInstance().supportOP01()) {
            httpSupported = false;
            thumbnailSupported = true;
        } else {
            // HTTP file transfer not support multiple recipients case yet,
            // so HTTP file transfer checking only valid for single file transfer
            if (chatId != null || contacts.size() > 1) {
                httpSupported = false;
                thumbnailSupported = RcsSettings.getInstance().isFileTransferThumbnailSupported();
            } else {
                String contact = contacts.get(0);
                // Check contact capabilities
                Capabilities capability = ContactsManager.getInstance().getContactCapabilities(contact);
                Capabilities myCapability = RcsSettings.getInstance().getMyCapabilities();
        
                if (capability == null || myCapability == null) {
                    httpSupported = false;
                    thumbnailSupported = true;
                } else {
                    httpSupported = capability.isFileTransferHttpSupported();
                    thumbnailSupported = capability.isFileTransferThumbnailSupported() &&
                            RcsSettings.getInstance().isFileTransferThumbnailSupported();
                }
            }
        }

        byte[] thumbnailData = null;
        if (thumbnailSupported) { 
            if (thumbnail != null) {
                thumbnailData = ChatUtils.getFileThumbnail(thumbnail);
            } else {
                if (content.getEncoding().contains("image")) {
                    thumbnailData = ChatUtils.createFileThumbnail(content.getUrl());
                } else
                if (content.getEncoding().contains("video")) {
                    thumbnailData = ChatUtils.createVideoThumbnail(content.getUrl());
                }
            }
        }

        if (thumbnailData != null){
            if (logger.isActivated()) {
                logger.debug("FTS length of thumbanil: " + thumbnailData.length);
            }
        }

        if (logger.isActivated()) {
            logger.info("FTS isHttpProtocol: " + httpSupported);
        }

        GroupChatSession groupSession = null;
        
        if (chatId != null) {
            Enumeration<ImsServiceSession> e = Core.getInstance().getImService().getSessions();
            while (e.hasMoreElements()) {
                ImsServiceSession session = (ImsServiceSession) e.nextElement();
                
                if (session instanceof GroupChatSession) {
                    if (((GroupChatSession) session).getContributionID().equals(chatId)) {
                        groupSession = (GroupChatSession)session;
                        break;
                    }
                }
            }
            if (groupSession == null)
                throw new CoreException("No active session for file transfer");
        }

        // Initiate session
        FileSharingSession session;

        if (httpSupported) {
            // HTTP not support 1-N file transfer yet
            if (chatId != null) {
                session = new OriginatingHttpGroupFileSharingSession(
                            this,
                            content,
                            ImsModule.IMS_USER_PROFILE.getImConferenceUri(),
                            new ListOfParticipant(contacts),
                            thumbnailData,
                            groupSession.getSessionID(),
                            chatId);
            } else {
                session = new OriginatingHttpFileSharingSession(
                            this,
                            content,
                            PhoneUtils.formatNumberToSipUri(contacts.get(0)),
                            thumbnailData,
                            null,
                            null);
            }
        } else {
            String contact;
            if (chatId != null) {
                contact = groupSession.getImSessionIdentity();
            } else
            if (contacts.size() > 1) {
                contact = RcsSettings.getInstance().getImMultiConferenceUri();
            } else
            if (type == FileTransferLog.Type.PUBLIC) {
                contact = contacts.get(0);
            } else {
                contact = PhoneUtils.formatNumberToSipUri(contacts.get(0));
            }

            session = new OriginatingExtendedFileSharingSession(
                        this,
                        content,
                        contact,
                        contacts,
                        chatId,
                        thumbnailData);
            if (duration > 0)
                session.setTimeLen(duration);
        }

        return session;
    }
                
    /**
     * Initiate a group file transfer session
     * 
     * @param contacts List of remote contacts
     * @param content Content to be sent
     * @param thumbnail Thumbnail filename
     * @param chatSessionId Chat session ID
     * @param chatContributionId Chat contribution ID
     * @return File transfer session
     * @throws CoreException
     */
    public FileSharingSession initiateGroupFileTransferSession(List<String> contactList, MmContent content, String thumbnail, String chatSessionId, String chatContributionId) throws CoreException {
        if (logger.isActivated()) {
            logger.info("FTS group Send file " + content.toString() + " to " + contactList.size() + " contacts");
        }  

        Capabilities myCapability = RcsSettings.getInstance().getMyCapabilities();

        if (logger.isActivated()) {
            logger.debug("FTS initiateGroupFileTransferSession self capability isFTThumbnail: " + myCapability.isFileTransferThumbnailSupported());
        }

        // Select default protocol
        if (!myCapability.isFileTransferHttpSupported()) {
            if (logger.isActivated()) {
                logger.debug("FTS Group HTTP not supported");
            }
            //throw new CoreException("Group file transfer not supported");
        }
        
        // Test number of sessions
        if ((maxFtSessions != 0) && (getFileTransferSessions().size() >= maxFtSessions)) {
            if (logger.isActivated()) {
                logger.debug("FTS Group The max number of file transfer sessions is achieved: cancel the initiation");
            }
            throw new CoreException("Max file transfer sessions achieved");
        }

        // Test max size
        if (maxFtSize > 0 && content.getSize() > maxFtSize) {
            if (logger.isActivated()) {
                logger.debug("FTS Group File exceeds max size: cancel the initiation");
            }
            throw new CoreException("File exceeds max size");
        }

        // Get thumbnail data
        byte[] thumbnailData = null;
        if (thumbnail != null) {
            // Create the thumbnail
            if (logger.isActivated()) {
                logger.debug("FTS initiateGroupFileTransferSession get thumbnail");
            }
            thumbnailData = ChatUtils.getFileThumbnail(thumbnail);
        }
        else if(myCapability.isFileTransferThumbnailSupported()){
            thumbnailData = ChatUtils.createFileThumbnail(content.getUrl());
            if (logger.isActivated()) {
                logger.debug("FTS initiateGroupFileTransferSession create thumbnail");
            }
            if (thumbnailData == null || thumbnailData.length == 0)
                thumbnailData = ChatUtils.createVideoThumbnail(content.getUrl());
        }

        //thumbnailData = ChatUtils.createFileThumbnail(content.getUrl());

        if(thumbnailData != null){
            if (logger.isActivated()) {
                logger.debug("FTS group length of thumbanil: " + thumbnailData.length );
            }
        }

        // Select default protocol
        boolean isHttpProtocol = false;
        if (RcsSettings.getInstance().isFileTransferHttpSupported()) {
            isHttpProtocol = true;
        }
        if (logger.isActivated()) {
                logger.info("FTS group Http issupportl: " + isHttpProtocol);
        }

        ChatSession tmpSession = (ChatSession) Core.getInstance().getImService().getSession(chatSessionId);

        String conferenceFocus = tmpSession.getImSessionIdentity();
        boolean msrpFtSupport =  false;
        msrpFtSupport = ((GroupChatSession)tmpSession).getMsrpFtSupport();
        if (logger.isActivated()) {
            logger.info("FTS group msrpFtSupport supportl: " + msrpFtSupport + ": Focus: " + conferenceFocus);
        }

        if(msrpFtSupport){
            isHttpProtocol = false;
        }
        if (logger.isActivated()) {
            logger.info("FTS group msrp issupportl: " + msrpFtSupport);
        }
    msrpFtSupport = true;
    isHttpProtocol = false;

    if(RcsSettings.getInstance().supportOP01() ||
       RcsSettings.getInstance().isSupportOP08()) {
            msrpFtSupport = true;
        } else {
            isHttpProtocol = true;
        }


        FileSharingSession session = null;
        if (isHttpProtocol){
            // Create a new HTTP session 
            session = new OriginatingHttpGroupFileSharingSession(
                    this,
                    content,
                    ImsModule.IMS_USER_PROFILE.getImConferenceUri(),
                    new ListOfParticipant(contactList),
                    thumbnailData,
                    chatSessionId,
                    tmpSession.getContributionID());
        }
        else if (msrpFtSupport){
            //Create a new MSRP Session
            session = new OriginatingGroupFileSharingSession(
                    this,
                    content,
                    conferenceFocus,
                    new ListOfParticipant(contactList),
                    thumbnailData,
                    chatSessionId,
                    tmpSession.getContributionID());
        }
        else{
            if (logger.isActivated()) {
                logger.info("FTS WRONF FILE TRANSFER");
            }
        }

        // Start the session
        return session;
    }

    /**
     * Receive a file transfer invitation
     * 
     * @param invite Initial invite
     */
    public void receiveFileTransferInvitation(SipRequest invite) {
        if (logger.isActivated()) {
            logger.info("FTS Receive a file transfer session invitation");
        }

        // Test if the contact is blocked
        String remote = SipUtils.getAssertedIdentity(invite);
        if (ContactsManager.getInstance().isFtBlockedForContact(remote)) {
            if (logger.isActivated()) {
                logger.debug("FTS Contact " + remote + " is blocked: automatically reject the file transfer invitation");
            }

            // Send a 603 Decline response
            sendErrorResponse(invite, 603);
            return;
        }

        // Test number of sessions
        if ((maxFtSessions != 0) && (getFileTransferSessions().size() >= maxFtSessions)) {
            if (logger.isActivated()) {
                logger.debug("FTS The max number of file transfer sessions is achieved: reject the invitation");
            }

            // Send a 603 Decline response
            sendErrorResponse(invite, 603);
            return;
        }
        String fileTransferId = "";
        boolean isPaused = false;
        int fileLen = 0;
        MediaDescription mediaDesc = null;

        if(true){
            try {
                // Extract the "setup" parameter
                String timeLen = null;
                String remoteSdp = invite.getSdpContent();
                SdpParser parser = new SdpParser(remoteSdp.getBytes());
                Vector<MediaDescription> media = parser.getMediaDescriptions();
                mediaDesc = media.elementAt(0);

                MediaAttribute attr5 = mediaDesc.getMediaAttribute("file-disposition");
                if (attr5 != null) {
                    timeLen = attr5.getValue();
                }
                if (logger.isActivated()){
                    logger.debug("FTS timeLen attribute is " + timeLen);
                }
                if (timeLen != null) {
                    String[] timeStringArray = timeLen.split("=");
                    if (logger.isActivated()){
                        logger.debug("FTS timeStringArray attribute is " + timeStringArray[0] + " new" + timeStringArray[1]);
                    }
                    String timeString = timeStringArray[1];
                    fileLen = Integer.parseInt(timeString);
                    if (logger.isActivated()){
                        logger.debug("FTS timeString attribute is " + timeString + "filelen is " + fileLen);
                    }
                }           
            } catch(Exception e){
                e.printStackTrace();
            }
        }

        if (isTagPresent(invite.getSdpContent(), "file-range")) {
            MediaAttribute attr1 = mediaDesc.getMediaAttribute("file-selector");
            String oldHashselector = attr1.getValue().substring(attr1.getValue().indexOf("hash:sha-1:")+1);
            PauseResumeFileObject pauseResumeObject = FileTransferServiceImpl.getHashPauseInfo(oldHashselector);
            if(pauseResumeObject == null){
                if (logger.isActivated()) {
                    logger.debug("Can not be resumed. Hash selector does not match");
                }
                // Send a 603 Decline response
                sendErrorResponse(invite, 603);
                return;
            }
            isPaused = true;
            fileTransferId = pauseResumeObject.mFileTransferId;
        }

        // Create a new session
        FileSharingSession session = new TerminatingFileSharingSession(this, invite);
        if (isPaused){
            session.fileTransferPaused();
            session.setOldFileTransferId(fileTransferId);
            if (isTagPresent(invite.getSdpContent(), "recvonly")) {
                session.setSendOnly(true);
            }
        }
        session.setTimeLen(fileLen);
        session.setOldFileTransferId(session.getSessionID());
        if (logger.isActivated()) {
            logger.debug("Old transfer Id. " + session.getSessionID());
        }

        int sessionType = FileTransferLog.Type.CHAT;
        String publicChatService =  SipUtils.getPreferredService(invite);
        if(publicChatService != null &&
           publicChatService.equals(FeatureTags.FEATURE_CMCC_URN_PUBLIC_ACCOUNT)){
            session.setPublicChatFile(true);
            sessionType = FileTransferLog.Type.PUBLIC;
        }

        FileTransferServiceImpl.addFileTransferPauseResumeData(
                session.getSessionID(),
                session.getContent().getUrl(),
                session.getContent().getSize(),
                session.getRemoteContact(),
                session.getHashselector(),
                session.getContent().getEncoding(),
                sessionType);

        // Start the session
        session.startSession();

        if (isTagPresent(invite.getSdpContent(), "file-range")) {
            // Notify listener
            getImsModule().getCore().getListener().handleResumeFileTransferInvitation(session, false,fileTransferId);
        } else {
            // Notify listener
            getImsModule().getCore().getListener().handleFileTransferInvitation(session, false);
        }
    }

    /**
     * Test a tag is present or not in SIP message
     * 
     * @param message Message or message part
     * @param tag Tag to be searched
     * @return Boolean
     */
    private boolean isTagPresent(String message, String tag) {
        if ((message != null) && (tag != null) && (message.toLowerCase().indexOf(tag) != -1)) {
            return true;
        } else {
            return false;
        }
    }

    /**
     * Receive a file transfer invitation
     * 
     * @param invite Initial invite
     */
    public void receiveGroupFileTransferInvitation(SipRequest invite) {
        if (logger.isActivated()) {
            logger.info("FTS Receive a file transfer session invitation Contact Uri: " + invite.getContactURI());
        }

        // Test if the contact is blocked
        String remote = SipUtils.getAssertedIdentity(invite);
        if (ContactsManager.getInstance().isFtBlockedForContact(remote)) {
            if (logger.isActivated()) {
                logger.debug("FTS Contact " + remote + " is blocked: automatically reject the file transfer invitation");
            }

            // Send a 603 Decline response
            sendErrorResponse(invite, 603);
            return;
        }

        // Test number of sessions
        if ((maxFtSessions != 0) && (getFileTransferSessions().size() >= maxFtSessions)) {
            if (logger.isActivated()) {
                logger.debug("FTS The max number of file transfer sessions is achieved: reject the invitation");
            }

            // Send a 603 Decline response
            sendErrorResponse(invite, 603);
            return;
        }

        Vector<ChatSession> sessions = getImSessions();
        GroupChatSession groupSession = null;
        GroupChatSession groupChatSession = null;
        for (int i=0; i < sessions.size(); i++) {
            ChatSession session = (ChatSession)sessions.get(i);
            if (session instanceof GroupChatSession) {
                String contactUri = null;
                String targetUri = null;
                groupChatSession = (GroupChatSession)session;
                try {
                    contactUri =  invite.getFromUri();
                    if (logger.isActivated()) {
                        logger.info("FTS Receive a file transfer session invitation fromUri  " + contactUri);
                    }
                } catch(Exception e){
                    e.printStackTrace();
                }
                try {
                    targetUri = ((ImsServiceSession)groupChatSession).getDialogPath().getTarget();
                    if (logger.isActivated()) {
                        logger.info("FTS Receive a file transfer session invitation targetUri  " + targetUri);
                    }
                } catch (Exception e){
                    e.printStackTrace();
                }
                if ((targetUri != null && contactUri != null && targetUri.equals(contactUri)) || RcsSettings.getInstance().isSupportOP08()) {
                    if (logger.isActivated()) {
                        logger.info("FTS Receive a file transfer session contact found " + contactUri);
                    }
                    groupSession = groupChatSession;
                    break;
                }
            }
        }

        if (groupSession == null){
            groupSession = groupChatSession;
            if (logger.isActivated()) {
                logger.info("FTS Receive a file transfer, group chat session not found");
            }
            if (!RcsSettings.getInstance().supportOP01()) {
                // Send a 603 Decline response
                sendErrorResponse(invite, 603);
                return;
            }
        }

        if (groupSession != null){
            if (groupSession.isMessageBlocked() == true){
                if (logger.isActivated()) {
                    logger.debug("receiveGroupFileTransferInvitation is blocked: automatically reject the file transfer invitation");
                }
                return;
            }
        }

        // Create a new session
        FileSharingSession session = new TerminatingGroupFileSharingSession(
                this, groupSession, invite);
        FileTransferServiceImpl.addFileTransferPauseResumeData(
                session.getSessionID(),
                session.getContent().getUrl(),
                session.getContent().getSize(),
                session.getRemoteContact(),
                session.getHashselector(),
                session.getContent().getEncoding(),
                FileTransferLog.Type.CHAT);
        // Start the session
        session.startSession();

        // Notify listener
        getImsModule().getCore().getListener().handleFileTransferInvitation(session, true);
    }

    /**
     * Initiate a one-to-one chat session
     * 
     * @param contact Remote contact
     * @param firstMsg First message
     * @return IM session
     * @throws CoreException
     */
    public ChatSession initiateOne2OneChatSession(String contact, InstantMessage firstMsg) throws CoreException {
        if (logger.isActivated()) {
            logger.info("Initiate 1-1 chat session with " + contact);
        }

        // Test number of sessions
        if ((maxChatSessions != 0) && (getImSessions().size() >= maxChatSessions)) {
            if (logger.isActivated()) {
                logger.debug("The max number of chat sessions is achieved: cancel the initiation");
            }
            throw new CoreException("Max chat sessions achieved");
        }

        // Create a new session
        OriginatingOne2OneChatSession session = new OriginatingOne2OneChatSession(
                this,
                PhoneUtils.formatNumberToSipUri(contact),
                firstMsg);

        return session;
    }          

    /**
     * Initiate a stand-alone chat session
     * 
     * @param contact Remote contact
     * @param firstMsg First message
     * @return IM session
     * @throws CoreException
     */
    public ChatSession initiateStandaloneChatSession(
            List<String> contacts, InstantMessage msg) throws CoreException {

        if ((maxChatSessions != 0) && (getImSessions().size() >= maxChatSessions)) {
            if (logger.isActivated()) {
                logger.info("The max number of chat sessions is achieved: cancel the initiation");
            }
            throw new CoreException("Max chat sessions achieved");
        }

        String target;
        if (contacts.size() > 1)
            target = RcsSettings.getInstance().getImMultiConferenceUri();
        else if (msg.getMessageType() == ChatLog.Message.Type.PUBLIC)
            target = PhoneUtils.formatUuidToSipUri(contacts.get(0));
        else
            target = PhoneUtils.formatNumberToSipUri(contacts.get(0));

        ArrayList<String> featureTags = new ArrayList<String>();
        String contentType = CpimMessage.MIME_TYPE;
        String preferService = null;

        featureTags.add(FeatureTags.FEATURE_RCSE_LARGE_MSG);

        switch (msg.getMessageType()) {
        case ChatLog.Message.Type.BURN:
            featureTags.add(FeatureTags.FEATURE_CPM_BURNED_MSG);
            break;

        case ChatLog.Message.Type.PUBLIC:
            featureTags.add(FeatureTags.FEATURE_CMCC_IARI_PUBLIC_ACCOUNT);
            preferService = FeatureTags.FEATURE_CMCC_URN_PUBLIC_ACCOUNT;
            break;

        case ChatLog.Message.Type.CLOUD:
            featureTags.set(0, FeatureTags.FEATURE_CMCC_IARI_CLOUD_FILE);
            contentType = CpimMessage.MIME_TYPE_CLOUDMESSAGE;
            break;

        case ChatLog.Message.Type.EMOTICON:
            featureTags.set(0, FeatureTags.FEATURE_CMCC_IARI_EMOTICON);
            contentType = CpimMessage.MIME_TYPE_PAYEMOTICON;
            break;
        }

        OriginatingStandaloneChatSession session = new OriginatingStandaloneChatSession(
                this, target, contacts, msg, featureTags, contentType, preferService);

        return session;
    }
    
    /**
     * Receive a one-to-one chat session invitation
     * 
     * @param invite Initial invite
     */
    public void receiveStandaloneChatSession(SipRequest invite) {
        if (logger.isActivated()){
            logger.info("Receive a 1-1 stand alone chat session invitation");
        }

        // Test if the contact is blocked
        String remote = ChatUtils.getReferredIdentity(invite);
        if (ContactsManager.getInstance().isImBlockedForContact(remote)) {
            if (logger.isActivated()) {
                logger.debug("Contact " + remote + " is blocked: automatically reject the chat invitation");
            }          
            
            // Send a 486 Busy response
            sendErrorResponse(invite, 486);
            return;
        }

        // Test number of sessions
        if ((maxChatSessions != 0) && (getImSessions().size() >= maxChatSessions)) {
            if (logger.isActivated()) {
                logger.info("The max number of chat sessions is achieved: reject the invitation");
            }
            
            // Send a 486 Busy response
            sendErrorResponse(invite, 486);
            return;
        }

        // Create a new session
        TerminatingStandaloneChatSession session = new TerminatingStandaloneChatSession(this, invite);

        // Start the session
        session.startSession();

        // Notify listener
        getImsModule().getCore().getListener().handleStandaloneChatSessionInvitation(session);
    }

    /**
     * Receive a one-to-one chat session invitation
     * 
     * @param invite Initial invite
     */
    public void receiveOne2OneChatSession(SipRequest invite) {
        if (logger.isActivated()){
            logger.info("Receive a 1-1 chat session invitation");
        }

        // Test if the contact is blocked
        String remote = ChatUtils.getReferredIdentity(invite);
        if (ContactsManager.getInstance().isImBlockedForContact(remote)) {
            if (logger.isActivated()) {
                logger.debug("Contact " + remote + " is blocked: automatically reject the chat invitation");
            }

            // Save the message in the spam folder
            InstantMessage firstMsg = ChatUtils.getFirstMessage(invite);
            if (firstMsg != null) {
                RichMessagingHistory.getInstance().addSpamMessage(firstMsg);
            }

            // Send message delivery report if requested
            if (ChatUtils.isImdnDeliveredRequested(invite)) {
                // Check notification disposition
                String msgId = ChatUtils.getMessageId(invite);
                if (msgId != null) {
                    String remoteInstanceId = null;
                    ContactHeader inviteContactHeader = (ContactHeader)invite.getHeader(ContactHeader.NAME);
                    if (inviteContactHeader != null) {
                        remoteInstanceId = inviteContactHeader.getParameter(SipUtils.SIP_INSTANCE_PARAM);
                    }
                    // Send message delivery status via a SIP MESSAGE
                    getImdnManager().sendMessageDeliveryStatusImmediately(SipUtils.getAssertedIdentity(invite),
                            msgId, ImdnDocument.DELIVERY_STATUS_DELIVERED, remoteInstanceId);
                }
            }

            // Send a 486 Busy response
            sendErrorResponse(invite, 486);
            return;
        }

        // Save the message
        InstantMessage firstMsg = ChatUtils.getFirstMessage(invite);
        if (firstMsg != null) {
            RichMessagingHistory.getInstance().addChatMessage(firstMsg, ChatLog.Message.Direction.INCOMING);
        }

        // Test number of sessions
        if ((maxChatSessions != 0) && (getImSessions().size() >= maxChatSessions)) {
            if (logger.isActivated()) {
                logger.info("The max number of chat sessions is achieved: reject the invitation");
            }

            // Send a 486 Busy response
            sendErrorResponse(invite, 486);
            return;
        }

        // Create a new session
        TerminatingOne2OneChatSession session = new TerminatingOne2OneChatSession(this, invite);

        // Start the session
        session.startSession();

        // Notify listener
        getImsModule().getCore().getListener().handleOneOneChatSessionInvitation(session);
    }

    /**
     * Initiate an ad-hoc group chat session
     * 
     * @param contacts List of contacts
     * @param subject Subject
     * @return IM session
     * @throws CoreException
     */
    public ChatSession initiateAdhocGroupChatSession(List<String> contacts, String subject) throws CoreException {
        if (logger.isActivated()) {
            logger.info("Initiate an ad-hoc group chat session");
        }

        // Test number of sessions
        if ((maxChatSessions != 0) && (getImSessions().size() >= maxChatSessions)) {
            if (logger.isActivated()) {
                logger.info("The max number of chat sessions is achieved: cancel the initiation");
            }
            throw new CoreException("Max chat sessions achieved");
        }

        // Create a new session
        OriginatingAdhocGroupChatSession session = new OriginatingAdhocGroupChatSession(
                this,
                ImsModule.IMS_USER_PROFILE.getImConferenceUri(),
                subject,
                new ListOfParticipant(contacts));

        return session;
    }

/**
     * Initiate an ad-hoc group chat session
     * 
     * @param contacts List of contacts
     * @param subject Subject
     * @return IM session
     * @throws CoreException
     */
    public ChatSession initiateClosedGroupChatSession(List<String> contacts, String subject) throws CoreException {
        if (logger.isActivated()) {
            logger.info("Initiate an closed group chat session");
        }

        // Test number of sessions
        if ((maxChatSessions != 0) && (getImSessions().size() >= maxChatSessions)) {
            if (logger.isActivated()) {
                logger.info("The max number of chat sessions is achieved: cancel the initiation");
            }
            throw new CoreException("Max chat sessions achieved");
        }

        // Create a new session
        OriginatingClosedGroupChatSession session = new OriginatingClosedGroupChatSession(
                this,
                ImsModule.IMS_USER_PROFILE.getImConferenceUri(),
                subject,
                new ListOfParticipant(contacts));

        return session;
    }

    /**
     * Receive ad-hoc group chat session invitation
     * 
     * @param invite Initial invite
     */
    public void receiveAdhocGroupChatSession(SipRequest invite) {
        if (logger.isActivated()) {
            logger.info("Receive an ad-hoc group chat session invitation");
        }

        // Test if the contact is blocked
        String remote = ChatUtils.getReferredIdentity(invite);
        if (ContactsManager.getInstance().isImBlockedForContact(remote)) {
            if (logger.isActivated()) {
                logger.info("Contact " + remote + " is blocked: automatically reject the chat invitation");
            }

            // Send a 486 Busy response
            sendErrorResponse(invite, 486);
            return;
        }

        // Test number of sessions
        if ((maxChatSessions != 0) && (getImSessions().size() >= maxChatSessions)) {
            if (logger.isActivated()) {
                logger.info("The max number of chat sessions is achieved: reject the invitation");
            }

            // Send a 486 Busy response
            sendErrorResponse(invite, 486);
            return;
        }

        String isClosedGroup = null;
        try {
            String remoteSdp = invite.getSdpContent();
            SdpParser parser = new SdpParser(remoteSdp.getBytes());
            Vector<MediaDescription> media = parser.getMediaDescriptions();
            MediaDescription mediaDesc = media.elementAt(0);
            MediaAttribute attr2 = mediaDesc.getMediaAttribute("isClosedGroup");        
            if (attr2 != null) {
                isClosedGroup = attr2.getValue();
            }
            if (logger.isActivated()){
                logger.info("Is closed group " + isClosedGroup);
            }
        }catch(Exception e){
            
            e.printStackTrace();
        }

        // Create a new session
        TerminatingAdhocGroupChatSession session = new TerminatingAdhocGroupChatSession(this, invite);

        // Start the session
        session.startSession();
        if(isClosedGroup != null){
            session.setClosedGroup(true);
        }

        // Notify listener
        getImsModule().getCore().getListener().handleAdhocGroupChatSessionInvitation(session);
    }

    public void receivePagerModeMessage(SipRequest request) {
        if (logger.isActivated()) {
            logger.debug("received a pager mode message we will send 200 OK");
        }
        
        InstantMessage message = null;

        String contentType = request.getContentType();
        String text = request.getContent();
        boolean imdnRequested = ChatUtils.isImdnDeliveredRequested(request); 

        if (contentType != null && contentType.equalsIgnoreCase(CpimMessage.MIME_TYPE)) {
            // now update others about this
            CpimParser cpimParser = null;
            try {
                cpimParser = new CpimParser(request.getContent());
            } catch (Exception e1) {
                if (logger.isActivated()) {
                    logger.debug("failed to parse the msg" + request.getContent());
                }
            }
            CpimMessage cpimMsg = cpimParser.getCpimMessage();
            text = cpimMsg.getMessageContent();

            //check encoding type
            String encoding = cpimMsg.getContentHeader(SipUtils.HEADER_CONTENT_TRANSFER_ENCODING);
            if (encoding.contains("base64")) {
                if (logger.isActivated()) {
                    logger.debug("received a pager message in base 64");
                }
                text = new String(Base64.decodeBase64(cpimMsg.getMessageContent().getBytes()));
            }
        }
        message = ChatUtils.createTextMessage(request.getFrom(), text, imdnRequested);

        if (SipUtils.isFeatureTagPresent(request, FeatureTags.FEATURE_CPM_BURNED_MSG)) {
            message.setMessageType(ChatLog.Message.Type.BURN);
        } else
        if (SipUtils.isFeatureTagPresent(request, FeatureTags.FEATURE_CMCC_PUBLIC_ACCOUNT)) {
            message.setMessageType(ChatLog.Message.Type.PUBLIC);
        } else
        if (SipUtils.isFeatureTagPresent(request, FeatureTags.FEATURE_CMCC_CLOUD_FILE)) {
            message.setMessageType(ChatLog.Message.Type.CLOUD);
        } else
        if (SipUtils.isFeatureTagPresent(request, FeatureTags.FEATURE_CMCC_EMOTICON)) {
            message.setMessageType(ChatLog.Message.Type.EMOTICON);
        }

        // Notify listener
        String conversation = ChatUtils.getCoversationId(request);
        getImsModule().getCore().getListener().handleReceivePagerMessage(message, conversation);

        // Send a 200 OK response
        try {
            SipResponse response = SipMessageFactory.createResponse(request, IdGenerator.getIdentifier(), 200);
            getImsModule().getSipManager().sendSipResponse(response);
        } catch (Exception e) {
            if (logger.isActivated()) {
                logger.error("Can't send 200 OK response for the received pager msg", e);
            }

        }

        // Send message delivery report if requested
        if (ChatUtils.isImdnDeliveredRequested(request)) {
            // Check notification disposition
            String msgId = ChatUtils.getMessageId(request);
            if (msgId != null) {
                String remoteInstanceId = null;
                ContactHeader inviteContactHeader = (ContactHeader)request.getHeader(ContactHeader.NAME);
                if (inviteContactHeader != null) {
                    remoteInstanceId = inviteContactHeader.getParameter(SipUtils.SIP_INSTANCE_PARAM);
                }
                // Send message delivery status via a SIP MESSAGE
                try{
                    getImdnManager().sendMessageDeliveryStatusImmediately(
                        SipUtils.getAssertedIdentity(request),
                        msgId,
                        ImdnDocument.DELIVERY_STATUS_DELIVERED,
                        remoteInstanceId);
                }catch(Exception e){
                    e.printStackTrace();
                }
            }
        }
    }
    
    
    /**
     * Rejoin a group chat session
     * 
     * @param chatId Chat ID
     * @return IM session
     * @throws CoreException
     */
    public ChatSession rejoinGroupChatSession(String chatId) throws CoreException {
        if (logger.isActivated()) {
            logger.info("Rejoin group chat session");
        }

        // Test number of sessions
        if ((maxChatSessions != 0) && (getImSessions().size() >= maxChatSessions)) {
            if (logger.isActivated()) {
                logger.warn("The max number of chat sessions is achieved: cancel the initiation");
            }
            throw new CoreException("Max chat sessions achieved");
        }

        // Get the group chat info from database
        GroupChatInfo groupChat = RichMessagingHistory.getInstance().getGroupChatInfo(chatId); 
        if (groupChat == null) {
            if (logger.isActivated()) {
                logger.warn("Group chat " + chatId + " can't be rejoined: conversation not found");
            }
            throw new CoreException("Group chat conversation not found in database");
        }
        if (groupChat.getRejoinId() == null) {
            if (logger.isActivated()) {
                logger.warn("Group chat " + chatId + " can't be rejoined: rejoin ID not found");
            }
            throw new CoreException("Rejoin ID not found in database");
        }

        if(!(RcsSettings.getInstance().supportOP01())){
            //for terminating side in CMCC, invite doesn't contain any participants list, so need to use member DB
            List<String> participants = groupChat.getParticipants(); // Added by Deutsche Telekom AG
            if (participants.size() == 0) {
                if (logger.isActivated()) {
                    logger.warn("Group chat " + chatId + " can't be rejoined: participants not found");
                }
                throw new CoreException("Group chat participants not found in database");
            }
        }

        // Create a new session
        if (logger.isActivated()) {
            logger.debug("Rejoin group chat: " + groupChat.toString());
        }
        RejoinGroupChatSession session = new RejoinGroupChatSession(
                this,
                groupChat.getRejoinId(),
                groupChat.getContributionId(),
                groupChat.getSubject(),
                groupChat.getParticipants());

        session.setChairman(RichMessagingHistory.getInstance().getGroupChairman(chatId));
        int blocked = RichMessagingHistory.getInstance().getGroupBlockedStatus(chatId);
        if(blocked == 1) {
            session.setMessageBlocked(true);
        } else if(blocked == 0){
            session.setMessageBlocked(false);
        }

        return session;
    }
    
    /**
     * Rejoin a group chat session
     * 
     * @param chatId Chat ID
     * @return IM session
     * @throws CoreException
     */
    public ChatSession rejoinGroupChatSession(String chatId, String rejoiId) throws CoreException {
        if (logger.isActivated()) {
            logger.info("Rejoin group chat session");
        }

        // Test number of sessions
        if ((maxChatSessions != 0) && (getImSessions().size() >= maxChatSessions)) {
            if (logger.isActivated()) {
                logger.warn("The max number of chat sessions is achieved: cancel the initiation");
            }
            throw new CoreException("Max chat sessions achieved");
        }

        if (RcsSettings.getInstance().supportOP01()) {
            if (logger.isActivated()) {
                logger.warn("Group chat " + chatId + " update rejoinId in DB: " + rejoiId);
            }
            RichMessagingHistory.getInstance().updateGroupChatRejoinId(chatId, rejoiId);
        }

        // Get the group chat info from database
        GroupChatInfo groupChat = RichMessagingHistory.getInstance().getGroupChatInfo(chatId); 
        if (groupChat == null) {
            if (logger.isActivated()) {
                logger.warn("Group chat " + chatId + " can't be rejoined: conversation not found");
            }
            throw new CoreException("Group chat conversation not found in database");
        }
        if (groupChat.getRejoinId() == null) {
            if (logger.isActivated()) {
                logger.warn("Group chat " + chatId + " can't be rejoined: rejoin ID not found");
            }
            throw new CoreException("Rejoin ID not found in database");
        }

        if(!(RcsSettings.getInstance().supportOP01())){
            //for terminating side in CMCC, invite doesn't contain any participants list, so need to use member DB
            List<String> participants = groupChat.getParticipants(); // Added by Deutsche Telekom AG
            if (participants.size() == 0) {
                if (logger.isActivated()) {
                    logger.warn("Group chat " + chatId + " can't be rejoined: participants not found");
                }
                throw new CoreException("Group chat participants not found in database");
            }
        }

        // Create a new session
        if (logger.isActivated()) {
            logger.debug("Rejoin group chat: " + groupChat.toString());
        }
        RejoinGroupChatSession session = new RejoinGroupChatSession(
                this,
                groupChat.getRejoinId(),
                groupChat.getContributionId(),
                groupChat.getSubject(),
                groupChat.getParticipants());

        session.setChairman(RichMessagingHistory.getInstance().getGroupChairman(chatId));
        int blocked = RichMessagingHistory.getInstance().getGroupBlockedStatus(chatId);
        if(blocked == 1){
            session.setMessageBlocked(true);
        } else if(blocked == 0){
            session.setMessageBlocked(false);
        }

        return session;
    }

    /**
     * Restart a group chat session
     * 
     * @param chatId Chat ID
     * @return IM session
     * @throws CoreException
     */
    public ChatSession restartGroupChatSession(String chatId) throws CoreException {
        if (logger.isActivated()) {
            logger.info("Restart group chat session");
        }

        // Test number of sessions
        if ((maxChatSessions != 0) && (getImSessions().size() >= maxChatSessions)) {
            if (logger.isActivated()) {
                logger.warn("The max number of chat sessions is achieved: cancel the initiation");
            }
            throw new CoreException("Max chat sessions achieved");
        }
                                
        // Get the group chat info from database
        GroupChatInfo groupChat = RichMessagingHistory.getInstance().getGroupChatInfo(chatId);
        if (groupChat == null) {
            if (logger.isActivated()) {
                logger.warn("Group chat " + chatId + " can't be restarted: conversation not found");
            }
            throw new CoreException("Group chat conversation not found in database");
        }

        // Get the connected participants from database
        List<String> participants = RichMessagingHistory.getInstance().getGroupChatConnectedParticipants(chatId);

        if (participants.size() == 0) {
            if (logger.isActivated()) {
                logger.warn("Group chat " + chatId + " can't be restarted: participants not found");
            }
            throw new CoreException("Group chat participants not found in database");
        }

        // Create a new session
        if (logger.isActivated()) {
            logger.debug("Restart group chat: " + groupChat.toString());
        }
        RestartGroupChatSession session = new RestartGroupChatSession(
                this,
                ImsModule.IMS_USER_PROFILE.getImConferenceUri(),
                groupChat.getSubject(),
                new ListOfParticipant(participants),
                chatId);

        session.setChairman(RichMessagingHistory.getInstance().getGroupChairman(chatId));
        int blocked = RichMessagingHistory.getInstance().getGroupBlockedStatus(chatId);
        if (blocked == 1){
            session.setMessageBlocked(true);
        } else if(blocked == 0){
            session.setMessageBlocked(false);
        }

        return session;
    }

    /**
     * Receive a conference notification
     * 
     * @param notify Received notify
     */
    public void receiveConferenceNotification(SipRequest notify) {
        // Dispatch the notification to the corresponding session
        Vector<ChatSession> sessions = getImSessions();
        for (int i=0; i < sessions.size(); i++) {
            ChatSession session = (ChatSession)sessions.get(i);
            if (session instanceof GroupChatSession) {
                GroupChatSession groupChatSession = (GroupChatSession)session;
                if (groupChatSession.getConferenceEventSubscriber().isNotifyForThisSubscriber(notify)) {
                    groupChatSession.getConferenceEventSubscriber().receiveNotification(notify);
                }
            }
        }
    }

    
    /**
     * Receive a group list notification
     * 
     * @param notify Received notify
     */
    public void receiveGroupListNotification(SipRequest notify) {
        if (logger.isActivated()) {
            logger.info("receiveGroupListNotification");
        }
    }

    /**
     * Receive a message delivery status
     * 
     * @param message Received message
     */
    public void receiveMessageDeliveryStatus(SipRequest message) {
        // Send a 200 OK response
        try {
            if (logger.isActivated()) {
                logger.info("Send 200 OK");
            }
            SipResponse response = SipMessageFactory.createResponse(message,
                    IdGenerator.getIdentifier(), 200);
            getImsModule().getSipManager().sendSipResponse(response);
        } catch(Exception e) {
            if (logger.isActivated()) {
                logger.error("Can't send 200 OK response", e);
            }
            return;
        }

        // Parse received message
        ImdnDocument imdn = ChatUtils.parseCpimDeliveryReport(message.getContent());
        if ((imdn != null) && (imdn.getMsgId() != null) && (imdn.getStatus() != null)) {
            String contact = SipUtils.getAssertedIdentity(message);
            String status = imdn.getStatus();
            String msgId = imdn.getMsgId();

            // Check if message delivery of a file transfer
            String ftSessionId = RichMessagingHistory.getInstance().getFileTransferId(msgId);
            if (logger.isActivated()) {
                logger.info("receiveMessageDeliveryStatus: msgId: " + msgId + ", ftId: " + ftSessionId);
            }
            if (!StringUtils.isEmpty(ftSessionId)) {
                // Notify the file delivery outside of the chat session
                receiveFileDeliveryStatus(ftSessionId, status,contact);
            } else {
                // Get session associated to the contact
                Vector<ChatSession> sessions = Core.getInstance().getImService().getImSessionsWith(contact);
                if (sessions.size() > 0) {
                    // Notify the message delivery from the chat session
                    for(int i=0; i < sessions.size(); i++) {
                        ChatSession session = sessions.elementAt(i);
                        session.handleMessageDeliveryStatus(msgId, status, contact);
                    }
                } else {
                    // Notify the message delivery outside of the chat session
                    getImsModule().getCore().getListener().handleMessageDeliveryStatus(contact, msgId, status);
                }
            }
        }
    }

    public void receiveFileDeliveryStatus(String ftSessionId, String status, String contact) {
        // Notify the file delivery outside of the chat session
        getImsModule().getCore().getListener().handleFileDeliveryStatus(ftSessionId, status,contact);
    }

    /**
     * Receive S&F push messages
     * 
     * @param invite Received invite
     */
    public void receiveStoredAndForwardPushMessages(SipRequest invite) {
        if (logger.isActivated()) {
            logger.info("ABC Receive S&F push messages invitation");
        }

        // Test if the contact is blocked
        String remote = ChatUtils.getReferredIdentity(invite);
        if (ContactsManager.getInstance().isImBlockedForContact(remote)) {
            if (logger.isActivated()) {
                logger.debug("Contact " + remote + " is blocked: automatically reject the S&F invitation");
            }

            // Send a 486 Busy response
            sendErrorResponse(invite, 486);
            return;
        }

        // Save the message
        InstantMessage firstMsg = ChatUtils.getFirstMessage(invite);
        if (firstMsg != null) {
            if (!RichMessagingHistory.getInstance().isOne2OneMessageExists(firstMsg.getMessageId())){
                if (logger.isActivated()) {
                    logger.info("ABC Message does not exis in DB ID: " + firstMsg.getMessageId());
                }
                RichMessagingHistory.getInstance().addChatMessage(firstMsg, ChatLog.Message.Direction.INCOMING);
            }
            else{
                if (logger.isActivated()) {
                    logger.info("ABC Message already exist in DB ID: " + firstMsg.getMessageId());
                }
            }
        }

        // Create a new session
        getStoreAndForwardManager().receiveStoredMessages(invite);
    }

    /**
     * Receive S&F push notifications
     * 
     * @param invite Received invite
     */
    public void receiveStoredAndForwardPushNotifications(SipRequest invite) {
        if (logger.isActivated()) {
            logger.info("ABC Receive S&F push notifications invitation");
        }
                
        // Test if the contact is blocked
        String remote = ChatUtils.getReferredIdentity(invite);
        if (ContactsManager.getInstance().isImBlockedForContact(remote)) {
            if (logger.isActivated()) {
                logger.debug("Contact " + remote + " is blocked: automatically reject the S&F invitation");
            }

            // Send a 486 Busy response
            sendErrorResponse(invite, 486);
            return;
        }

        // Create a new session
        getStoreAndForwardManager().receiveStoredNotifications(invite);
    }

    /**
     * Receive HTTP file transfer invitation
     *
     * @param invite Received invite
     * @param ftinfo File transfer info document
     */
    public void receiveHttpFileTranferInvitation(SipRequest invite, FileTransferHttpInfoDocument ftinfo) {
        if (logger.isActivated()){
            logger.info("Receive a single HTTP file transfer invitation");
        }

        // Test if the contact is blocked
        String remote = ChatUtils.getReferredIdentity(invite);
        if (ContactsManager.getInstance().isFtBlockedForContact(remote)) {
            if (logger.isActivated()) {
                logger.debug("Contact " + remote + " is blocked, automatically reject the HTTP File transfer");
            }

            // Send a 603 Decline response
            sendErrorResponse(invite, 603);
            return;
        }

        // Test number of sessions
        if ((maxFtSessions != 0) && (getFileTransferSessions().size() >= maxFtSessions)) {
            if (logger.isActivated()) {
                logger.debug("The max number of FT sessions is achieved, reject the HTTP File transfer");
            }

            // Send a 603 Decline response
            sendErrorResponse(invite, 603);
            return;
        }

        // Reject if file is too big or size exceeds device storage capacity. This control should be done
        // on UI. It is done after end user accepts invitation to enable prior handling by the application.
        FileSharingError error = FileSharingSession.isFileCapacityAcceptable(ftinfo.getFileSize());
        if (error != null) {
            // Send a 603 Decline response
            sendErrorResponse(invite, 603);
            return;
        }

        // Create and start a chat session
        TerminatingOne2OneChatSession one2oneChatSession = new TerminatingOne2OneChatSession(this, invite);
        one2oneChatSession.startSession();
        
        // Create and start a new HTTP file transfer session
        TerminatingHttpFileSharingSession httpFiletransferSession = new TerminatingHttpFileSharingSession(this,
                one2oneChatSession, ftinfo, ChatUtils.getMessageId(invite),one2oneChatSession.getRemoteDisplayName(),one2oneChatSession.getRemoteContact());
        httpFiletransferSession.startSession();
        
        // Notify listener
        getImsModule().getCore().getListener().handle1to1FileTransferInvitation(httpFiletransferSession, one2oneChatSession);
    }

    /**
     * Receive S&F HTTP file transfer invitation
     *
     * @param invite Received invite
     * @param ftinfo File transfer info document
     */
    public void receiveStoredAndForwardHttpFileTranferInvitation(SipRequest invite, FileTransferHttpInfoDocument ftinfo) {
        if (logger.isActivated()) {
            logger.info("Receive a single S&F HTTP file transfer invitation");
        }

        // Create and start a chat session
        TerminatingStoreAndForwardMsgSession one2oneChatSession = new TerminatingStoreAndForwardMsgSession(this, invite);
        one2oneChatSession.startSession();
        
        // Auto reject if file too big
        if (isFileSizeExceeded(ftinfo.getFileSize())) {
            if (logger.isActivated()) {
                logger.debug("File is too big, reject file transfer invitation");
            }

            // Send a 403 Decline response
            //TODO add warning header "xxx Size exceeded"
            one2oneChatSession.sendErrorResponse(invite, one2oneChatSession.getDialogPath().getLocalTag(), 403);

            // Close session
            one2oneChatSession.handleError(new FileSharingError(FileSharingError.MEDIA_SIZE_TOO_BIG));
            return;
        }
        
        // Create and start a new HTTP file transfer session
        TerminatingHttpFileSharingSession httpFiletransferSession = new TerminatingHttpFileSharingSession(this,
                one2oneChatSession, ftinfo, ChatUtils.getMessageId(invite),one2oneChatSession.getRemoteDisplayName(),one2oneChatSession.getRemoteContact());
        httpFiletransferSession.startSession();
        
        // Notify listener
        getImsModule().getCore().getListener().handle1to1FileTransferInvitation(httpFiletransferSession, one2oneChatSession);
    }
                
    /**
     * Check whether file size exceeds the limit
     * 
     * @param size of file
     * @return {@code true} if file size limit is exceeded, otherwise {@code false}
     */
    private boolean isFileSizeExceeded(long size) {
        // Auto reject if file too big
        int maxSize = FileSharingSession.getMaxFileSharingSize();
        if (maxSize > 0 && size > maxSize) {
            return true;
        }

        return false;
    }
}
