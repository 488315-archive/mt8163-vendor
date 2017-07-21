package com.mediatek.rcs.pam.message;

import org.gsma.joyn.chat.ChatService;
import org.gsma.joyn.chat.ExtendChat;
import org.gsma.joyn.chat.ExtendChatListener;
import org.gsma.joyn.ft.FileTransferListener;
import org.gsma.joyn.ft.FileTransferService;

public interface IPAMMessageHelper {

    ChatService getChatService();

    FileTransferService getFileTransferService();

    ExtendChatListener getChatServiceListener(long msgId);

    FileTransferListener getFileTransferListener(long msgId);

    /**
     * Cache related Interface
     */
    ExtendChat getChatCache(long token, String uuid);

    void updateChatCache(long token, String uuid, ExtendChat chat);

}
