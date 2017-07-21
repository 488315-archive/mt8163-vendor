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
package com.orangelabs.rcs.service.api;

import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.List;
import java.util.Vector;


import org.gsma.joyn.Build;
import org.gsma.joyn.IJoynServiceRegistrationListener;
import org.gsma.joyn.chat.ChatIntent;
import org.gsma.joyn.ft.FileTransfer;
import org.gsma.joyn.ft.FileTransferIntent;
import org.gsma.joyn.ft.FileTransferLog;
import org.gsma.joyn.ft.FileTransferServiceConfiguration;
import org.gsma.joyn.ft.IFileTransfer;
import org.gsma.joyn.ft.IFileTransferListener;
import org.gsma.joyn.ft.IFileTransferService;
import org.gsma.joyn.ft.INewFileTransferListener;

import android.content.Intent;
import android.os.IBinder;
import android.os.RemoteCallbackList;

import com.orangelabs.rcs.core.Core;
import com.orangelabs.rcs.core.content.ContentManager;
import com.orangelabs.rcs.core.content.MmContent;
import com.orangelabs.rcs.core.ims.ImsModule;
import com.orangelabs.rcs.core.ims.network.sip.FeatureTags;
import com.orangelabs.rcs.core.ims.network.sip.SipUtils;
import com.orangelabs.rcs.core.ims.protocol.sdp.MediaAttribute;
import com.orangelabs.rcs.core.ims.protocol.sdp.MediaDescription;
import com.orangelabs.rcs.core.ims.protocol.sdp.SdpParser;
import com.orangelabs.rcs.core.ims.service.im.InstantMessagingService;
import com.orangelabs.rcs.core.ims.service.im.chat.ChatSession;
import com.orangelabs.rcs.core.ims.service.im.chat.ChatUtils;
import com.orangelabs.rcs.core.ims.service.im.chat.GroupChatSession;
import com.orangelabs.rcs.core.ims.service.im.chat.imdn.ImdnDocument;
import com.orangelabs.rcs.core.ims.service.im.filetransfer.FileSharingSession;
import com.orangelabs.rcs.core.ims.service.im.filetransfer.OriginatingExtendedFileSharingSession;
import com.orangelabs.rcs.core.ims.service.im.filetransfer.http.HttpFileTransferSession;
import com.orangelabs.rcs.platform.AndroidFactory;
import com.orangelabs.rcs.platform.file.FileDescription;
import com.orangelabs.rcs.platform.file.FileFactory;
import com.orangelabs.rcs.provider.messaging.FileTransferData;
import com.orangelabs.rcs.provider.messaging.RichMessagingHistory;
import com.orangelabs.rcs.provider.settings.RcsSettings;
import com.orangelabs.rcs.utils.DateUtils;
import com.orangelabs.rcs.utils.PhoneUtils;
import com.orangelabs.rcs.utils.logger.Logger;
import com.orangelabs.rcs.service.LauncherUtils;
import com.orangelabs.rcs.core.ims.service.im.filetransfer.TerminatingGroupFileSharingSession;

/**
 * File transfer service implementation
 * 
 * @author Jean-Marc AUFFRET
 */
public class FileTransferServiceImpl extends IFileTransferService.Stub {
    /**
     * List of service event listeners
     */
    private RemoteCallbackList<IJoynServiceRegistrationListener> serviceListeners =
            new RemoteCallbackList<IJoynServiceRegistrationListener>();

    /**
     * List of file transfer sessions
     */
    private static Hashtable<String, IFileTransfer> ftSessions =
            new Hashtable<String, IFileTransfer>();

    /**
     * List of file transfer invitation listeners
     */
    private RemoteCallbackList<INewFileTransferListener> listeners =
            new RemoteCallbackList<INewFileTransferListener>();

    /**
     * The logger
     */
    private static Logger logger = Logger.getLogger(FileTransferServiceImpl.class.getName());

    /**
     * Lock used for synchronization
     */
    private Object lock = new Object();

    public static ArrayList<PauseResumeFileObject> mFileTransferPauseResumeData =
            new ArrayList<PauseResumeFileObject>();

    /**
     * Constructor
     */
    public FileTransferServiceImpl() {
        if (logger.isActivated()) {
            logger.info("File transfer service API is loaded");
        }
    }

    /**
     * Close API
     */
    public void close() {
        // Clear list of sessions
        ftSessions.clear();

        if (logger.isActivated()) {
            logger.info("File transfer service API is closed");
        }
    }

    /**
     * Add a file transfer session in the list
     *
     * @param session File transfer session
     */
    protected static void addFileTransferSession(FileTransferImpl session) {
        if (logger.isActivated()) {
            logger.debug("FTS Add a file transfer session in the list (size="
                    + ftSessions.size() + ")" + " Id: " + session.getTransferId());
        }

        ftSessions.put(session.getTransferId(), session);
    }

    /**
     * Remove a file transfer session from the list
     *
     * @param sessionId Session ID
     */
    protected static void removeFileTransferSession(String sessionId) {
        if (logger.isActivated()) {
            logger.debug("FTS Remove a file transfer session from the list (size="
                    + ftSessions.size() + ")" + " Id: " + sessionId);
        }

        ftSessions.remove(sessionId);
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
        synchronized (lock) {
            if (logger.isActivated()) {
                logger.info("FTS Add a service listener");
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
        synchronized (lock) {
            if (logger.isActivated()) {
                logger.info("FTS Remove a service listener");
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
        if (logger.isActivated()) {
            logger.info("FTS notifyRegistrationEvent State: " + state);
        }
        // Notify listeners
        synchronized (lock) {
            final int N = serviceListeners.beginBroadcast();
            for (int i = 0; i < N; i++) {
                try {
                    if (state) {
                        serviceListeners.getBroadcastItem(i).onServiceRegistered();
                    } else {
                        serviceListeners.getBroadcastItem(i).onServiceUnregistered();
                    }
                } catch (Exception e) {
                    if (logger.isActivated()) {
                        logger.error("FTS Can't notify listener", e);
                    }
                }
            }
            serviceListeners.finishBroadcast();
        }
    }

    /**
     * Receive a new file transfer invitation
     *
     * @param session File transfer session
     * @param isGroup is group file transfer
     */
    public void receiveHttpFileTransferInvitation(String remote, FileSharingSession session,
            boolean isGroup) {
        if (logger.isActivated()) {
            logger.info("FTS Receive httpfile transfer invitation from " + remote + " isGroup "
                    + isGroup);
        }
        String chatId = null;
        int timeLen = session.getTimeLen();
        if (session instanceof HttpFileTransferSession) {
            chatId = ((HttpFileTransferSession) session).getContributionID();
            if (logger.isActivated()) {
                logger.info("FTS Receive HTTP file transfer invitation chatid: " + chatId);
            }
        } else if (isGroup) {
            chatId = ((TerminatingGroupFileSharingSession) session).getGroupChatSession()
                    .getContributionID();
            if (logger.isActivated()) {
                logger.info("FTS Receive group MSRP file transfer invitation chatid: " + chatId);
            }
        }

        // Extract number from contact
        String number = remote;

        if (isGroup) {
            RichMessagingHistory.getInstance().addIncomingGroupFileTransfer(chatId, number,
                    session.getSessionID(), session.getContent());
        } else {
            // Update rich messaging history
            RichMessagingHistory.getInstance().addFileTransfer(
                    number, session.getSessionID(),
                    FileTransfer.Direction.INCOMING, session.getContent());
        }

        RichMessagingHistory.getInstance().updateFileIcon(
                session.getSessionID(), session.getThumbUrl());

        if (RcsSettings.getInstance().isCPMSupported()) {
            if (!(RichMessagingHistory.getInstance().getCoversationID(
                    session.getRemoteContact(), 1).equals(session.getConversationID()))) {
                if (session.getConversationID() != null) {
                    if (logger.isActivated()) {
                        logger.info("FTS receiveHttpFileTransferInvitation  OldId: "
                                + RichMessagingHistory.getInstance().getCoversationID(
                                        session.getRemoteContact(), 1) + " NewId: "
                                + session.getConversationID());
                    }
                    RichMessagingHistory.getInstance().UpdateCoversationID(
                            session.getRemoteContact(), session.getConversationID(), 1);
                } else {
                    if (logger.isActivated()) {
                        logger.info("FTS receiveHttpFileTransferInvitation"
                                + " conversation id is null");
                    }
                }
            }
        }

        // Add session in the list
        FileTransferImpl sessionApi = new FileTransferImpl(session);
        FileTransferServiceImpl.addFileTransferSession(sessionApi);

        // Broadcast intent related to the received invitation
        Intent intent = new Intent(FileTransferIntent.ACTION_NEW_INVITATION);
        intent.addFlags(Intent.FLAG_EXCLUDE_STOPPED_PACKAGES);
        intent.putExtra(FileTransferIntent.EXTRA_CONTACT, number);
        intent.putExtra(FileTransferIntent.EXTRA_DISPLAY_NAME, session.getRemoteDisplayName());
        intent.putExtra(FileTransferIntent.EXTRA_TRANSFER_ID, session.getSessionID());
        intent.putExtra(FileTransferIntent.EXTRA_FILENAME, session.getContent().getName());
        intent.putExtra(FileTransferIntent.EXTRA_FILESIZE, session.getContent().getSize());
        intent.putExtra(FileTransferIntent.EXTRA_FILETYPE, session.getContent().getEncoding());
        intent.putExtra(FileTransferIntent.TIME_LEN, session.getTimeLen());
        intent.putExtra(FileTransferIntent.GEOLOC_FILE, session.isGeoLocFile());
        /** M: ftAutAccept @{ */
        intent.putExtra("autoAccept", session.shouldAutoAccept());
        /** @} */
        String chatSessionId = null;
        if (session instanceof HttpFileTransferSession) {
            intent.putExtra("chatSessionId", ((HttpFileTransferSession)session).getChatSessionID());
            if (isGroup) {
                intent.putExtra("chatId", chatId);
            }
            intent.putExtra("isGroupTransfer", isGroup);
        } else if (isGroup) {

            chatSessionId = ((TerminatingGroupFileSharingSession) session).getGroupChatSession()
                    .getSessionID();
            if (logger.isActivated()) {
                logger.info("FTS Receive file transfer invitation: " + "Chatsessionid: "
                        + chatSessionId);
            }
            intent.putExtra("chatSessionId", chatSessionId);
            intent.putExtra("chatId", chatId);
            intent.putExtra("isGroupTransfer", isGroup);
        }
        AndroidFactory.getApplicationContext().sendBroadcast(intent);

        // Notify file transfer invitation listeners
        synchronized (lock) {
            final int N = listeners.beginBroadcast();
            if (logger.isActivated()) {
                logger.info("FTS Receive HTTP file transfer invitation N: " + N);
            }
            for (int i = 0; i < N; i++) {
                try {
                    listeners.getBroadcastItem(i).onNewFileTransfer(session.getSessionID());
                    listeners.getBroadcastItem(i).onNewFileTransferReceived(
                            session.getSessionID(),
                            session.shouldAutoAccept(),
                            isGroup, chatSessionId, chatId, timeLen);
                } catch (Exception e) {
                    if (logger.isActivated()) {
                        logger.error("FTS Can't notify listener", e);
                    }
                }
            }
            listeners.finishBroadcast();
        }
    }

    /**
     * Receive a new file transfer invitation
     *
     * @param session File transfer session
     * @param isGroup is group file transfer
     */
    public void receiveFileTransferInvitation(FileSharingSession session, boolean isGroup) {
        if (logger.isActivated()) {
            logger.info("Receive file transfer invitation from " + session.getRemoteContact()
                    + " Timelen " + session.getTimeLen());
        }
        String chatId = null;

        boolean isBurn = false;
        isBurn = session.isBurnMessage();

        boolean isPublicAccountFile = session.isPublicChatFile();

        int timeLen = session.getTimeLen();
        if (session instanceof HttpFileTransferSession) {
            chatId = ((HttpFileTransferSession) session).getContributionID();
            if (logger.isActivated()) {
                logger.info("Receive HTTP file transfer invitation chatid: " + chatId);
            }
        } else if (isGroup) {
            GroupChatSession parentSession = 
                ((TerminatingGroupFileSharingSession) session).getGroupChatSession();
            if (parentSession != null) {
                chatId = parentSession.getContributionID();
            } else {
                String rejoinId = session.getDialogPath().getTarget();
                chatId = RichMessagingHistory.getInstance().getChatIdbyRejoinId(rejoinId);
            }
            if (logger.isActivated()) {
                logger.info("Receive group MSRP file transfer invitation chatid: " + chatId);
            }
		} else {
			chatId = session.getRemoteContact();
			if (logger.isActivated()) {
				logger.info("Receive O2O MSRP file transfer invitation chatid: " + chatId);
			}
        }

        String number;
        int sessionType = FileTransferLog.Type.CHAT;
        if (session.isPublicChatFile()) {
            number = session.getRemoteContact();
            sessionType = FileTransferLog.Type.PUBLIC;
        } else {
            if (isBurn)
                sessionType = FileTransferLog.Type.BURN;
            number = PhoneUtils.extractNumberFromUri(session.getRemoteContact());
        }
        
        RichMessagingHistory.getInstance().addExtendedFileTransfer(
                chatId,
                number,
                session.getSessionID(),
                FileTransfer.Direction.INCOMING,
                sessionType,
                session.getContent());
        RichMessagingHistory.getInstance().updateFileTransferChatId(
                session.getSessionID(), chatId, session.getMessageId());

        if (session.getTimeLen() > 0) {
            RichMessagingHistory.getInstance().updateFileTransferDuration(
                    session.getSessionID(), session.getTimeLen());
        }
        RichMessagingHistory.getInstance().updateFileIcon(
                session.getSessionID(), session.getThumbUrl());

        if (RcsSettings.getInstance().supportOP01()) {
            String remoteSdp = session.getDialogPath().getInvite().getSdpContent();
            SdpParser parser = new SdpParser(remoteSdp.getBytes());
            Vector<MediaDescription> media = parser.getMediaDescriptions();
            MediaDescription mediaDescription = media.elementAt(0);
            MediaAttribute attrftid = mediaDescription.getMediaAttribute("file-transfer-id");
            if (attrftid != null) {
                RichMessagingHistory.getInstance().updateFileId(session.getSessionID(),
                        attrftid.getValue());
            }
            if (logger.isActivated()) {
                logger.debug("file-transfer-id: " + attrftid.getValue());
            }
        }

     try { 
        if (RcsSettings.getInstance().isCPMSupported() && isGroup) {
            if (!(RichMessagingHistory.getInstance()
                    .getCoversationID(session.getRemoteContact(), 1).equals(session
                    .getConversationID()))) {
                if (session.getConversationID() != null) {
                    if (logger.isActivated()) {
                        logger.info("FTS receiveFileTransferInvitation  OldId: "
                                + RichMessagingHistory.getInstance().getCoversationID(
                                        session.getRemoteContact(), 1) + " NewId: "
                                + session.getConversationID());
                    }
                    RichMessagingHistory.getInstance().UpdateCoversationID(
                            session.getRemoteContact(), session.getConversationID(), 1);
                } else {
                    if (logger.isActivated()) {
                        logger.info("FTS receiveFileTransferInvitation  Conversation Id is null");
                    }
                }
            }
        }
       } catch(Exception e){
      e.printStackTrace();
       }

        // Add session in the list
        FileTransferImpl sessionApi = new FileTransferImpl(session);
        FileTransferServiceImpl.addFileTransferSession(sessionApi);

        // addFileTransferPauseResumeData(
        // session.getSessionID(),session.getContent().getUrl(),session.getContent().getSize(),0,
        // session.getRemoteContact(),session.getHashselector());

        // Broadcast intent related to the received invitation
        Intent intent = new Intent(FileTransferIntent.ACTION_NEW_INVITATION);
        intent.addFlags(Intent.FLAG_EXCLUDE_STOPPED_PACKAGES);
        intent.putExtra(FileTransferIntent.EXTRA_CONTACT, number);
        intent.putExtra(FileTransferIntent.EXTRA_DISPLAY_NAME, session.getRemoteDisplayName());
        intent.putExtra(FileTransferIntent.EXTRA_TRANSFER_ID, session.getSessionID());
        intent.putExtra(FileTransferIntent.EXTRA_FILENAME, session.getContent().getName());
        intent.putExtra(FileTransferIntent.EXTRA_FILESIZE, session.getContent().getSize());
        intent.putExtra(FileTransferIntent.EXTRA_FILETYPE, session.getContent().getEncoding());
        intent.putExtra(FileTransferIntent.TIME_LEN, session.getTimeLen());
        intent.putExtra(FileTransferIntent.GEOLOC_FILE, session.isGeoLocFile());
        if (isBurn)
            intent.putExtra(FileTransferIntent.EXTRA_TRANSFERTYPE, FileTransfer.Type.BURNED);
        else
        if (isPublicAccountFile)
            intent.putExtra(FileTransferIntent.EXTRA_TRANSFERTYPE, FileTransfer.Type.PUBACCOUNT);
        else
            intent.putExtra(FileTransferIntent.EXTRA_TRANSFERTYPE, FileTransfer.Type.NORMAL);

        String chatSessionId = null;
        /** M: ftAutAccept @{ */
        intent.putExtra("autoAccept", session.shouldAutoAccept());
        /** @} */
        if (session instanceof HttpFileTransferSession) {
            chatSessionId = ((HttpFileTransferSession) session).getChatSessionID();
            intent.putExtra("chatSessionId", chatSessionId);
            if (isGroup) {
                intent.putExtra("chatId", chatId);
            }
            intent.putExtra("isGroupTransfer", isGroup);
        } else if (isGroup) {
            //not happened now.
            GroupChatSession parentSession = 
                ((TerminatingGroupFileSharingSession) session).getGroupChatSession();
            if (parentSession != null) {
                chatSessionId = parentSession.getContributionID();
            } else {
                String rejoinId = session.getDialogPath().getTarget();
                chatSessionId = RichMessagingHistory.getInstance().getChatIdbyRejoinId(rejoinId);
            }
            if (logger.isActivated()) {
                logger.info("FTS Receive file transfer invitation: " + "Chatsessionid: "
                        + chatSessionId);
            }
            intent.putExtra("chatSessionId", chatSessionId);
            intent.putExtra("chatId", chatId);
            intent.putExtra("isGroupTransfer", isGroup);
        } else {
            if (logger.isActivated()) {
                logger.info("FTS Receive file transfer invitation: It is not group");
            }
        }
        AndroidFactory.getApplicationContext().sendBroadcast(intent);

        // Notify file transfer invitation listeners
        synchronized (lock) {
            final int N = listeners.beginBroadcast();
            if (logger.isActivated()) {
                logger.info("FTS receiveFileTransferInvitation N: " + N);
            }
            for (int i = 0; i < N; i++) {
                try {
                    if (isPublicAccountFile) {
                        listeners.getBroadcastItem(i).onNewPublicAccountChatFile(
                                session.getSessionID(), session.shouldAutoAccept(), isGroup,
                                chatSessionId, chatId);
                    } else {
                        if (isBurn) {

                            listeners.getBroadcastItem(i).onNewBurnFileTransfer(
                                    session.getSessionID(), isGroup, chatSessionId, chatId);
                        } else {
                            listeners.getBroadcastItem(i).onNewFileTransfer(session.getSessionID());
                            listeners.getBroadcastItem(i).onNewFileTransferReceived(
                                    session.getSessionID(),
                                    session.shouldAutoAccept(),
                                    isGroup, chatSessionId, chatId, timeLen);
                        }
                    }

                } catch (Exception e) {
                    if (logger.isActivated()) {
                        logger.error("FTS Can't notify listener", e);
                    }
                }
            }
            listeners.finishBroadcast();
        }
    }

    /**
     * Receive a new file transfer invitation
     *
     * @param session File transfer session
     * @param isGroup is group file transfer
     */
    public void receiveResumeFileTransferInvitation(
            FileSharingSession session, boolean isGroup, String fileTransferId) {
        if (logger.isActivated()) {
            logger.info("Resume file transfer invitation from " + session.getRemoteContact());
        }
        String chatId = null;
        if (session instanceof HttpFileTransferSession) {
            chatId = ((HttpFileTransferSession) session).getContributionID();
            if (logger.isActivated()) {
                logger.info("Receive Resume HTTP file transfer invitation chatid: " + chatId);
            }
        } else if (isGroup) {
            chatId = ((TerminatingGroupFileSharingSession) session).getGroupChatSession()
                    .getContributionID();
            if (logger.isActivated()) {
                logger.info("Receive Resume group MSRP file transfer invitation chatid: "
                        + chatId);
            }
        }

        // Extract number from contact
        String number = PhoneUtils.extractNumberFromUri(session.getRemoteContact());

        // Update rich messaging history -- Deepak
        // RichMessagingHistory.getInstance().addFileTransfer(number,
        // session.getSessionID(), FileTransfer.Direction.INCOMING, session.getContent());

        // Add session in the list
        FileTransferImpl sessionApi = new FileTransferImpl(session);
        FileTransferServiceImpl.addFileTransferSession(sessionApi);

        // Broadcast intent related to the received invitation
        Intent intent = new Intent(FileTransferIntent.ACTION_RESUME_FILE);
        intent.addFlags(Intent.FLAG_EXCLUDE_STOPPED_PACKAGES);
        intent.putExtra(FileTransferIntent.EXTRA_CONTACT, number);
        intent.putExtra(FileTransferIntent.EXTRA_DISPLAY_NAME, session.getRemoteDisplayName());
        intent.putExtra(FileTransferIntent.EXTRA_TRANSFER_ID, session.getSessionID());
        intent.putExtra(FileTransferIntent.RESUMED_TRANSFER_ID, fileTransferId);
        intent.putExtra(FileTransferIntent.EXTRA_FILENAME, session.getContent().getName());
        intent.putExtra(FileTransferIntent.EXTRA_FILESIZE, session.getContent().getSize());
        intent.putExtra(FileTransferIntent.EXTRA_FILETYPE, session.getContent().getEncoding());

        /** @} */
        if (session instanceof HttpFileTransferSession) {
            intent.putExtra(
                    "chatSessionId", ((HttpFileTransferSession) session).getChatSessionID());
            if (isGroup) {
                intent.putExtra("chatId", chatId);
            }
            intent.putExtra("isGroupTransfer", isGroup);
        } else if (isGroup) {

            String chatSessionId = ((TerminatingGroupFileSharingSession) session)
                    .getGroupChatSession().getSessionID();
            if (logger.isActivated()) {
                logger.info("Resume file transfer invitation: " + "Chatsessionid: "
                        + chatSessionId);
            }
            intent.putExtra("chatSessionId", chatSessionId);
            intent.putExtra("chatId", chatId);
            intent.putExtra("isGroupTransfer", isGroup);
        }
        AndroidFactory.getApplicationContext().sendBroadcast(intent);

        // Notify file transfer invitation listeners about resume File
        /*
         * synchronized(lock) { final int N = listeners.beginBroadcast(); for (int i=0; i
         * < N; i++) { try {
         * listeners.getBroadcastItem(i).onNewFileTransfer(session.getSessionID()); }
         * catch(Exception e) { if (logger.isActivated()) {
         * logger.error("Can't notify listener", e); } } } listeners.finishBroadcast(); }
         */
    }

    /**
     * Receive a new HTTP file transfer invitation outside of an existing chat session
     *
     * @param session File transfer session
     */
    public void receiveFileTransferInvitation(
            FileSharingSession session, ChatSession chatSession) {

        if (logger.isActivated()) {
            logger.info("FTS Receive file transfer invitation from1 "
                    + session.getRemoteContact());
        }
        // Update rich messaging history
        if (chatSession.isGroupChat()) {
            // RichMessagingHistory.getInstance().updateFileTransferChatId(
            // chatSession.getFirstMessage().getMessageId(),
            // chatSession.getContributionID());
        }
        // Add session in the list
        FileTransferImpl sessionApi = new FileTransferImpl(session);
        addFileTransferSession(sessionApi);

        // Display invitation
        receiveFileTransferInvitation(session, chatSession.isGroupChat());
        // Display invitation
        /*
         * TODO receiveFileTransferInvitation(session, chatSession.isGroupChat());
         *
         * // Update rich messaging history
         * RichMessaging.getInstance().addIncomingChatSessionByFtHttp(chatSession);
         *
         * // Add session in the list ImSession sessionApi = new ImSession(chatSession);
         * MessagingApiService.addChatSession(sessionApi);
         */
    }

    public static PauseResumeFileObject getPauseInfo(String transferId) {
        PauseResumeFileObject object = null;
        for (int j = 0; j < mFileTransferPauseResumeData.size(); j++) {
            object = FileTransferServiceImpl.mFileTransferPauseResumeData.get(j);
            if (object.mFileTransferId.equals(transferId)) {
                if (logger.isActivated()) {
                    logger.info("getPauseInfo, FileTransfer Id & currentBytesTransferred"
                            + object.mFileTransferId);
                }
                return object;
            }
        }
        if (logger.isActivated()) {
            logger.info("getPauseInfo , Check databse");
        }
        object = RichMessagingHistory.getInstance().getPauseInfo(transferId);
        if (object != null && object.mFileTransferId != null) {
            mFileTransferPauseResumeData.add(object);
        }
        if (logger.isActivated()) {
            logger.info("getPauseInfo , After check databse" + object);
        }
        return object;

    }

    public static PauseResumeFileObject getHashPauseInfo(String hashSelector) {
        try {
            if (logger.isActivated()) {
                logger.info("getHashPauseInfo, entry hashSelector " + hashSelector);
            }
            for (int j = 0; j < mFileTransferPauseResumeData.size(); j++) {
                PauseResumeFileObject object = FileTransferServiceImpl.mFileTransferPauseResumeData
                        .get(j);
                if (object.hashSelector.equals(hashSelector)) {
                    if (logger.isActivated()) {
                        logger.info("getPauseInfo, hashSelector Id & currentBytesTransferred"
                                + object.hashSelector);
                    }
                    return object;
                }
            }
            if (logger.isActivated()) {
                logger.info("getPauseInfo , Return null");
            }
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return null;
    }

    /**
     * Returns the configuration of the file transfer service
     * 
     * @return Configuration
     */
    public FileTransferServiceConfiguration getConfiguration() {
        return new FileTransferServiceConfiguration(
                RcsSettings.getInstance().getWarningMaxFileTransferSize(),
                RcsSettings.getInstance().getMaxFileTransferSize(),
                RcsSettings.getInstance().isFileTransferAutoAccepted(),
                RcsSettings.getInstance().isFileTransferThumbnailSupported(),
                RcsSettings.getInstance().getMaxFileIconSize(),
                RcsSettings.getInstance().getMaxFileTransferSessions());
    }

    public static void addFileTransferPauseResumeData(
            String fileTransferId,
            String path,
            long fileSize,
            String contact,
            String hash,
            String mimeType,
            int transferType) {
        if (logger.isActivated()) {
            logger.info("addFileTransferPauseResumeData fileTransferId " + fileTransferId
                    + " path : " + path + "fileSize : " + fileSize
                    + "contact :" + contact + "hash:" + hash);
        }

        PauseResumeFileObject filePauseDdata = new PauseResumeFileObject();
        filePauseDdata.bytesTransferrred = 0;
        filePauseDdata.mFileTransferId = fileTransferId;
        filePauseDdata.mPath = path;
        filePauseDdata.mSize = fileSize;
        filePauseDdata.mContact = contact;
        filePauseDdata.hashSelector = hash;
        filePauseDdata.mMimeType = mimeType;
        filePauseDdata.mTransferType = transferType;
        mFileTransferPauseResumeData.add(filePauseDdata);

        try {
            RichMessagingHistory.getInstance().updateFileTransferProgressCode(
                    fileTransferId, 0, fileSize, path, contact, hash);
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    public IFileTransfer resumeGroupFileTransfer(String chatId, String fileTranferId,
            IFileTransferListener listener) throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("resumeGroupFile file " + fileTranferId);
        }

        PauseResumeFileObject pauseResumeObject = getPauseInfo(fileTranferId);
        if (pauseResumeObject == null) {
            if (logger.isActivated()) {
                logger.info("resumeGroupFile return null ");
            }
            return null;
        }

        String filename = pauseResumeObject.mPath;

        if (logger.isActivated()) {
            logger.info("resumeGroupFile file contact:" + "filename:" + filename);
        }

        // Test IMS connection
        ServerApiUtils.testIms();
        // Query Db whether FT is incoming/outgoing //Deepak

        try {
            // Initiate the session
            FileDescription desc = FileFactory.getFactory().getFileDescription(filename);
            MmContent content = ContentManager.createMmContentFromUrl(filename, desc.getSize());
            FileSharingSession session = null;
            List fileParticipants = RichMessagingHistory.getInstance().getFileTransferContacts(
                    fileTranferId);
            if (logger.isActivated() && fileParticipants != null) {
                logger.info("resumeGroupFile file participants:" + fileParticipants + " size:"
                        + fileParticipants.size());
            }
            session = Core.getInstance().getImService().initiateGroupFileTransferSession(
                    fileParticipants, content, null, chatId, null);
            final FileSharingSession resumedSession = session;
            resumedSession.fileTransferPaused();
            resumedSession.setOldFileTransferId(fileTranferId);
            resumedSession.setSessionId(fileTranferId);

            String direction = RichMessagingHistory.getInstance().getFtDirection(fileTranferId);
            if (logger.isActivated()) {
                logger.info("resumeGroupFile file direction:" + direction);
            }
            if (direction.equals("1")) {
                resumedSession.setReceiveOnly(false);
            } else {
                resumedSession.setReceiveOnly(true);
            }
            // Add session listener
            FileTransferImpl sessionApi = new FileTransferImpl(resumedSession);
            sessionApi.addEventListener(listener);

            if (pauseResumeObject != null && direction.equals("1")) {
                if (logger.isActivated()) {
                    logger.info("resumeGroupFile file pauseResumeObject:"
                            + pauseResumeObject + " & new Id is"
                            + sessionApi.getTransferId());
                }
                pauseResumeObject.mFileTransferId = sessionApi.getTransferId();

            }
            // Update Filetransfer Id rich messaging history
            if (direction.equals("1"))
                RichMessagingHistory.getInstance().updateFileTransferId(fileTranferId,
                        sessionApi.getTransferId());
            // Start the session
            Thread t = new Thread() {
                public void run() {
                    resumedSession.startSession();
                }
            };
            t.start();

            // Add session in the list
            addFileTransferSession(sessionApi);
            return sessionApi;
        } catch (Exception e) {
            if (logger.isActivated()) {
                logger.error("Unexpected error", e);
            }
            throw new ServerApiException(e.getMessage());
        }
    }

    /**
     * Transfers a file to a contact. The parameter file contains the complete filename
     * including the path to be transferred. The parameter contact supports the following
     * formats: MSISDN in national or international format, SIP address, SIP-URI or
     * Tel-URI. If the format of the contact is not supported an exception is thrown.
     *
     * @param contact Contact
     * @param filename Filename to transfer
     * @param fileicon Filename of the file icon associated to the file to be transfered
     * @param listenet File transfer event listener
     * @return File transfer
     * @throws ServerApiException
     */
    public IFileTransfer resumeFileTransfer(
            String fileTranferId, IFileTransferListener listener) throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("resumeFile file " + fileTranferId);
        }

        RichMessagingHistory rmHistory = RichMessagingHistory.getInstance();
        InstantMessagingService imService = Core.getInstance().getImService();

        PauseResumeFileObject pauseResumeObject = getPauseInfo(fileTranferId);
        if (pauseResumeObject == null) {
            if (logger.isActivated()) {
                logger.info("resumeFile return null ");
            }
            return null;
        }

        String contact = pauseResumeObject.mContact;
        String filename = pauseResumeObject.mPath;
        String chatId = rmHistory.getFileTransferChatId(fileTranferId);

        if (logger.isActivated()) {
            logger.info("resumeFile file contact:" + contact + " filename:" + filename);
        }

        // Test IMS connection
        ServerApiUtils.testIms();
        // Query Db whether FT is incoming/outgoing //Deepak

        try {
            // Initiate the session
            final FileSharingSession resumedSession;
            FileDescription desc = FileFactory.getFactory().getFileDescription(filename);
            MmContent content = ContentManager.createMmContentFromUrl(filename, desc.getSize());

            List<String> contacts = PhoneUtils.generateContactsList(pauseResumeObject.mContact);
            if (pauseResumeObject.mContact.equals(chatId)) {
                // not file transfer in group chat
                resumedSession = imService.initiateExtendedTransferSession(
                        null, contacts, content, null, 0, pauseResumeObject.mTransferType);
            } else {
                resumedSession = imService.initiateExtendedTransferSession(
                        chatId, contacts, content, null, 0, pauseResumeObject.mTransferType);
            }
            resumedSession.fileTransferPaused();
            resumedSession.setOldFileTransferId(fileTranferId);
            resumedSession.setSessionId(fileTranferId);

            String direction = rmHistory.getFtDirection(fileTranferId);
            if (logger.isActivated()) {
                logger.info("resumeFile file direction:" + direction + "contact:" + contact);
            }
            if (direction.equals("1")) {
                resumedSession.setReceiveOnly(false);
            } else {
                resumedSession.setReceiveOnly(true);
            }
            ((OriginatingExtendedFileSharingSession)resumedSession).setPauseInfo(pauseResumeObject);

            // Add session listener
            FileTransferImpl sessionApi = new FileTransferImpl(resumedSession);
            sessionApi.addEventListener(listener);

            // Start the session
            Thread t = new Thread() {
                public void run() {
                    resumedSession.startSession();
                }
            };
            t.start();

            // Add session in the list
            addFileTransferSession(sessionApi);
            return sessionApi;
        } catch (Exception e) {
            if (logger.isActivated()) {
                logger.error("Unexpected error", e);
            }
            throw new ServerApiException(e.getMessage());
        }
    }

    /**
     * Transfers a file to a contact. The parameter file contains the complete filename
     * including the path to be transferred. The parameter contact supports the following
     * formats: MSISDN in national or international format, SIP address, SIP-URI or
     * Tel-URI. If the format of the contact is not supported an exception is thrown.
     *
     * @param contact Contact
     * @param filename Filename to transfer
     * @param fileicon Filename of the file icon associated to the file to be transfered
     * @param listenet File transfer event listener
     * @return File transfer
     * @throws ServerApiException
     */
    public IFileTransfer transferFile(
            String contact,
            String filename,
            String fileicon,
            IFileTransferListener listener) throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("FTS Transfer file " + filename + " to " + contact);
        }

        // Test IMS connection
        ServerApiUtils.testIms();

        try {
            // Initiate the session
            FileDescription desc = FileFactory.getFactory().getFileDescription(filename);
            if (logger.isActivated()) {
                logger.info("FTS Transfer file " + filename + " size " + desc.getSize());
            }
            MmContent content = ContentManager.createMmContentFromUrl(filename, desc.getSize());
            final FileSharingSession session = Core.getInstance().getImService()
                    .initiateFileTransferSession(contact, content, fileicon, null, null); // TODO

            if (content.getEncoding().contains("vnd.gsma.rcspushlocation"))
                session.setGeoLocFile();

            // Add session listener
            FileTransferImpl sessionApi = new FileTransferImpl(session);
            sessionApi.addEventListener(listener);

            // Update rich messaging history
            RichMessagingHistory.getInstance().addFileTransfer(contact, session.getSessionID(),
                    FileTransfer.Direction.OUTGOING, session.getContent());
            if (session.getTimeLen() > 0) {
                RichMessagingHistory.getInstance().updateFileTransferDuration(
                        session.getSessionID(), session.getTimeLen());
            }
            addFileTransferPauseResumeData(
                    session.getSessionID(),
                    filename,
                    session.getContent().getSize(),
                    contact,
                    "",
                    content.getEncoding(),
                    FileTransferLog.Type.CHAT);
            // Start the session
            Thread t = new Thread() {
                public void run() {
                    session.startSession();
                }
            };
            t.start();

            // Add session in the list
            addFileTransferSession(sessionApi);
            return sessionApi;
        } catch (Exception e) {
            if (logger.isActivated()) {
                logger.error("Unexpected error", e);
            }
            throw new ServerApiException(e.getMessage());
        }
    }

    /**
     * Transfer a file to a group of contacts outside of an existing group chat
     *
     * @param contacts List of contact
     * @param file File to be transfered
     * @param thumbnail Thumbnail option
     * @return File transfer session
     * @throws ServerApiException
     */
    public IFileTransfer transferFileToGroup(
                String chatId, List<String> contacts, String filename,
                String fileicon, int timeLen, IFileTransferListener listener)
                        throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("FTS Transfer file " + filename + " to " + contacts);
            logger.info("FTS Transfer file CHATID: " + chatId);
        }

        // Test IMS connection
        ServerApiUtils.testIms();

        try {
            // Initiate the session
            FileDescription desc = FileFactory.getFactory().getFileDescription(filename);
            MmContent content = ContentManager.createMmContentFromUrl(filename, desc.getSize());
            final FileSharingSession session = Core.getInstance().getImService()
                    .initiateGroupFileTransferSession(contacts, content, fileicon, chatId, null);
            if (timeLen > 0) {
                session.setTimeLen(timeLen);
            }
            if (content.getEncoding().contains("vnd.gsma.rcspushlocation"))
                session.setGeoLocFile();

            // Add session in the list
            FileTransferImpl sessionApi = new FileTransferImpl(session);
            sessionApi.addEventListener(listener);

            logger.info("ABC Transfer file ContribitionID: " + session.getContributionID());

            // Update rich messaging history
            RichMessagingHistory.getInstance().addOutgoingGroupFileTransfer(
                    chatId,
                    session.getSessionID(),
                    session.getContent(),
                    null,
                    session.getRemoteContact());
            RichMessagingHistory.getInstance().updateFileTransferDuration(
                    session.getSessionID(), timeLen);
            addFileTransferPauseResumeData(
                    session.getSessionID(),
                    filename,
                    session.getContent().getSize(),
                    "",
                    "",
                    content.getEncoding(),
                    FileTransferLog.Type.CHAT);
            // Start the session
            Thread t = new Thread() {
                public void run() {
                    session.startSession();
                }
            };
            t.start();
            addFileTransferSession(sessionApi);
            return sessionApi;
        } catch (Exception e) {
            if (logger.isActivated()) {
                logger.error("FTS Unexpected error", e);
            }
            throw new ServerApiException(e.getMessage());
        }
    }

    /**
     * Transfers a file to a contact. The parameter file contains the complete filename
     * including the path to be transferred. The parameter contact supports the following
     * formats: MSISDN in national or international format, SIP address, SIP-URI or
     * Tel-URI. If the format of the contact is not supported an exception is thrown.
     *
     * @param contact Contact
     * @param filename Filename to transfer
     * @param fileicon Filename of the file icon associated to the file to be transfered
     * @param duration Video media file duration
     * @param type Specified transfer file
     * @param listener File transfer event listener
     * @return File transfer
     * @throws ServerApiException
     */
    public IFileTransfer transferFileEx(
            String contact,
            String filename,
            String fileicon,
            int duration,
            String type, 
            IFileTransferListener listener) throws ServerApiException {
        
        List<String> contacts = PhoneUtils.generateContactsList(contact);
        return transferFileEx(null, contacts, filename, fileicon, duration, type, listener);
    }

    /**
     * Transfers a file to multiple contacts. The parameter file contains the complete filename
     * including the path to be transferred. The parameter contact supports the following
     * formats: MSISDN in national or international format, SIP address, SIP-URI or
     * Tel-URI. If the format of the contact is not supported an exception is thrown.
     *
     * @param contacts Multiple contacts list
     * @param filename Filename to transfer
     * @param fileicon Filename of the file icon associated to the file to be transfered
     * @param duration Video media file duration
     * @param type Specified transfer file
     * @param listener File transfer event listener
     * @return File transfer
     * @throws ServerApiException
     */
    public IFileTransfer transferFileToMultiple(
            List<String> contacts,
            String filename,
            String fileicon,
            int duration,
            String type, 
            IFileTransferListener listener) throws ServerApiException {

        return transferFileEx(null, contacts, filename, fileicon, duration, type, listener);
    }

    /**
     * Transfers a file to group chat. The parameter file contains the complete filename
     * including the path to be transferred. The parameter contact supports the following
     * formats: MSISDN in national or international format, SIP address, SIP-URI or
     * Tel-URI. If the format of the contact is not supported an exception is thrown.
     *
     * @param chatId Identifier of group chat
     * @param contacts Contact
     * @param filename Filename to transfer
     * @param fileicon Filename of the file icon associated to the file to be transfered
     * @param duration Video media file duration
     * @param type Specified transfer file
     * @param listener File transfer event listener
     * @return File transfer
     * @throws ServerApiException
     */
    public IFileTransfer transferFileToGroupEx(
                String chatId,
                String filename,
                String fileicon,
                int duration,
                String type,
                IFileTransferListener listener) throws ServerApiException {
        return transferFileEx(chatId, null, filename, fileicon, duration, type, listener);
    }

    /**
     * Prosecute file to specific service number.
     *
     * @param contacts Contact
     * @param transferId file identifier
     * @param listener File transfer event listener
     * @return File transfer
     * @throws ServerApiException
     */
    public IFileTransfer prosecuteFile(
            String contact,
            String transferId,
            IFileTransferListener listener) throws ServerApiException {
        
        RichMessagingHistory rmHistory = RichMessagingHistory.getInstance();
        PauseResumeFileObject info = rmHistory.getPauseInfo(transferId);

        String extraContent = 
                "Spam-From: " + ChatUtils.formatCpimSipUri(info.mContact) + SipUtils.CRLF +
                "Spam-To: " + ImsModule.IMS_USER_PROFILE.getPublicUri() + SipUtils.CRLF +
                "DateTime: " + DateUtils.encodeDate(rmHistory.getFileTimeStamp(transferId));

        List<String> contacts = PhoneUtils.generateContactsList(contact);
        return transferFileEx(
                null, contacts, info.mPath, extraContent, 0, FileTransfer.Type.PROSECUTE, listener);
    }

    private IFileTransfer transferFileEx(
            String chatId,
            List<String> contacts,
            String filename,
            String fileicon,
            int duration,
            String type,
            IFileTransferListener listener) throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("FTS Transfer file " + filename + " to " + contacts + "with Timelen "
                    + duration);
        }

        // Test IMS connection
        ServerApiUtils.testIms();

        InstantMessagingService imService = Core.getInstance().getImService();
        RichMessagingHistory rmHistory = RichMessagingHistory.getInstance();

        int sessionType = FileTransferLog.Type.CHAT;
        if (type.equals(FileTransfer.Type.PUBACCOUNT)) {
            sessionType = FileTransferLog.Type.PUBLIC;
        } else
        if (type.equals(FileTransfer.Type.BURNED)) {
            sessionType = FileTransferLog.Type.BURN;
        }
        String extraContent = fileicon;
        if (type.equals(FileTransfer.Type.PROSECUTE)) {
            fileicon = null;
        }

        try {
            // Initiate the session
            FileDescription desc = FileFactory.getFactory().getFileDescription(filename);
            MmContent content = ContentManager.createMmContentFromUrl(filename, desc.getSize());
            final FileSharingSession session = imService.initiateExtendedTransferSession(
                    chatId, contacts, content, null, duration, sessionType);

            String transferId = session.getSessionID();

            // Add session listener
            FileTransferImpl sessionApi = new FileTransferImpl(session);
            sessionApi.addEventListener(listener);

            if (content.getEncoding().contains("vnd.gsma.rcspushlocation"))
                session.setGeoLocFile();

            String contact = PhoneUtils.generateContactsText(contacts);

            if (type.equals(FileTransfer.Type.NORMAL)) {
                
            } else
            if (type.equals(FileTransfer.Type.BURNED)) {
                session.setBurnMessage(true);
                OriginatingExtendedFileSharingSession exSession = 
                        (OriginatingExtendedFileSharingSession)session;
                List<String> extraTags = new ArrayList<String>();
                extraTags.add(FeatureTags.FEATURE_CPM_BURNED_MSG);
                exSession.setExtraTags(extraTags);
            } else
            if (type.equals(FileTransfer.Type.PUBACCOUNT)) {
                session.setPublicChatFile(true);
                OriginatingExtendedFileSharingSession exSession = 
                        (OriginatingExtendedFileSharingSession)session;
                List<String> extraTags = new ArrayList<String>();
                extraTags.add(FeatureTags.FEATURE_CMCC_IARI_PUBLIC_ACCOUNT);
                exSession.setExtraTags(extraTags);
                exSession.setPreferService(FeatureTags.FEATURE_CMCC_URN_PUBLIC_ACCOUNT);
            } else
            if (type.equals(FileTransfer.Type.PROSECUTE)) {
                OriginatingExtendedFileSharingSession exSession = 
                        (OriginatingExtendedFileSharingSession)session;
                exSession.setExtraContent(extraContent, "text-plain;charset=UTF8");
            }

            rmHistory.addExtendedFileTransfer(
                    chatId,
                    contact,
                    transferId,
                    FileTransfer.Direction.OUTGOING,
                    sessionType,
                    content);

            // Update rich messaging history
            if (duration > 0) {
                rmHistory.updateFileTransferDuration(transferId, duration);
            }
            
            addFileTransferPauseResumeData(
                    transferId,
                    filename,
                    content.getSize(),
                    contact,
                    "",
                    content.getEncoding(),
                    sessionType);

            // Start the session
            Thread t = new Thread() {
                public void run() {
                    session.startSession();
                }
            };
            t.start();

            // Add session in the list
            addFileTransferSession(sessionApi);
            return sessionApi;
        } catch (Exception e) {
            if (logger.isActivated()) {
                logger.error("Unexpected error", e);
            }
            throw new ServerApiException(e.getMessage());
        }
    }

    /**
     * Returns the list of file transfers in progress
     *
     * @return List of file transfer
     * @throws ServerApiException
     */
    public List<IBinder> getFileTransfers() throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("FTS Get file transfer sessions");
        }

        try {
            ArrayList<IBinder> result = new ArrayList<IBinder>(ftSessions.size());
            for (Enumeration<IFileTransfer> e = ftSessions.elements(); e.hasMoreElements();) {
                IFileTransfer sessionApi = e.nextElement();
                result.add(sessionApi.asBinder());
            }
            return result;
        } catch (Exception e) {
            if (logger.isActivated()) {
                logger.error("FTS Unexpected error", e);
            }
            throw new ServerApiException(e.getMessage());
        }
    }

    /**
     * Returns a current file transfer from its unique ID
     *
     * @return File transfer
     * @throws ServerApiException
     */
    public IFileTransfer getFileTransfer(String transferId) throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("FTS Get file transfer session " + transferId);
        }

        return ftSessions.get(transferId);
    }

    /**
     * Registers a file transfer invitation listener
     *
     * @param listener New file transfer listener
     * @throws ServerApiException
     */
    public void addNewFileTransferListener(INewFileTransferListener listener)
            throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("FTS Add a file transfer invitation listener");
        }

        listeners.register(listener);
    }

    /**
     * Unregisters a file transfer invitation listener
     *
     * @param listener New file transfer listener
     * @throws ServerApiException
     */
    public void removeNewFileTransferListener(INewFileTransferListener listener)
            throws ServerApiException {
        if (logger.isActivated()) {
            logger.info("FTS Remove a file transfer invitation listener");
        }

        listeners.unregister(listener);
    }

    /**
     * File Transfer delivery status. In FToHTTP, Delivered status is done just after
     * download information are received by the terminating, and Displayed status is done
     * when the file is downloaded. In FToMSRP, the two status are directly done just
     * after MSRP transfer complete.
     *
     * @param ftSessionId File transfer session Id
     * @param status status of File transfer
     */
    public void handleFileDeliveryStatus(String ftSessionId, String status, String contact) {
        if (logger.isActivated()) {
            logger.info("FTS handleFileDeliveryStatus contact: " + contact + " FtId: "
                    + ftSessionId + " Status: " + status);
        }
        if (status.equalsIgnoreCase(ImdnDocument.DELIVERY_STATUS_DELIVERED)) {
            // Update rich messaging history
            RichMessagingHistory.getInstance().updateFileTransferStatus(ftSessionId,
                    FileTransfer.State.DELIVERED);

            // Notify File transfer delivery listeners
            final int N = listeners.beginBroadcast();
            if (logger.isActivated()) {
                logger.info("FTS handleFileDeliveryStatus N is: " + N);
            }
            for (int i = 0; i < N; i++) {
                try {
                    listeners.getBroadcastItem(i).onFileDeliveredReport(ftSessionId, contact);
                } catch (Exception e) {
                    if (logger.isActivated()) {
                        logger.error("FTS Can't notify listener", e);
                    }
                }
            }
            listeners.finishBroadcast();
            if (RcsSettings.getInstance().supportOP01()) {
                if (logger.isActivated()) {
                    logger.info("LMM FT is null");
                }
                Intent intent = new Intent(FileTransferIntent.ACTION_DELIVERY_STATUS);
                intent.addFlags(Intent.FLAG_EXCLUDE_STOPPED_PACKAGES);
                intent.putExtra(ChatIntent.EXTRA_CONTACT, contact);
                intent.putExtra("msgId", ftSessionId);
                intent.putExtra("status", status);
                AndroidFactory.getApplicationContext().sendBroadcast(intent);
            }
        } else if (status.equalsIgnoreCase(ImdnDocument.DELIVERY_STATUS_DISPLAYED)) {
            // Update rich messaging history
            RichMessagingHistory.getInstance().updateFileTransferStatus(ftSessionId,
                    FileTransfer.State.DISPLAYED);

            // Notify File transfer delivery listeners
            final int N = listeners.beginBroadcast();
            if (logger.isActivated()) {
                logger.info("FTS handleFileDeliveryStatus N: " + N);
            }
            for (int i = 0; i < N; i++) {
                try {
                    listeners.getBroadcastItem(i).onFileDisplayedReport(ftSessionId, contact);
                } catch (Exception e) {
                    if (logger.isActivated()) {
                        logger.error("FTS Can't notify listener", e);
                    }
                }
            }
            listeners.finishBroadcast();
        }
    }

    /**
     * Returns the maximum number of file transfer session simultaneously
     *
     * @return numenr of sessions
     */

    public int getMaxFileTransfers() {
        return RcsSettings.getInstance().getMaxFileTransferSessions();
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
}
