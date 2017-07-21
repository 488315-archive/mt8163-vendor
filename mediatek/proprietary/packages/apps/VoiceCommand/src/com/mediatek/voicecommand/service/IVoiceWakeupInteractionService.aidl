package com.mediatek.voicecommand.service;

import com.mediatek.voicecommand.service.IVoiceWakeupInteractionCallback;

interface IVoiceWakeupInteractionService {
    /**
     Register VoiceWakeupInteractionService callback.
    */
    void registerCallback(IVoiceWakeupInteractionCallback callback);

    /**
     Start recognition.
    */
    boolean startRecognition();

    /**
     Stop recognition.
    */
    boolean stopRecognition();
}
