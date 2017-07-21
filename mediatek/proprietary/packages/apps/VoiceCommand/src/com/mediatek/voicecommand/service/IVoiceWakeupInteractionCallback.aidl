package com.mediatek.voicecommand.service;

interface IVoiceWakeupInteractionCallback {
    /**
     Callback from VoiceWakeupInteractionService for recognition result
    */
    void onVoiceWakeupDetected(int commandId);
}
