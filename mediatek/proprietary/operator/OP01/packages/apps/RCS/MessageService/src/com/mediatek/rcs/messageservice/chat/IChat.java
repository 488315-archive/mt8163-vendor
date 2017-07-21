package com.mediatek.rcs.messageservice.chat;

import org.gsma.joyn.capability.Capabilities;
import org.gsma.joyn.chat.ExtendMessage;

public interface IChat {
    public void sendChatMessage(final long msgIdInSMS, final String content, final int type);

    public void resendChatMessage(final String msgId);

//    public void sendBurnDeliveryReport(final String msgId);

    public void sendSpamMessage(final String msgId);

    public void onReceiveChatMessage(ExtendMessage message);

    public void onReceiveMessageDeliveryStatus(final String msgId, final String status);

    public void onSendChatMessageResult(final long smsId, final boolean result);

    public void onRegistrationStatusChanged(boolean status);

    public void onCapabilityChanged(Capabilities capability);
}
