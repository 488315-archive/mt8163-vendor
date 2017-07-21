/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2014. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

package com.mediatek.voicecommand.service;

import android.content.Intent;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;
import android.service.voice.AlwaysOnHotwordDetector;
import android.service.voice.AlwaysOnHotwordDetector.Callback;
import android.service.voice.AlwaysOnHotwordDetector.EventPayload;
import android.service.voice.VoiceInteractionService;
import android.widget.Toast;

import java.util.Arrays;
import java.util.Locale;

import com.mediatek.common.voicecommand.VoiceCommandListener;
import com.mediatek.voicecommand.business.VoiceTrainingEnrollmentUtil;
import com.mediatek.voicecommand.business.VoiceWakeupBusiness;
import com.mediatek.voicecommand.util.Log;

/**
 * Service implementation for VoiceWakeup.
 */
public class VoiceWakeupInteractionService extends VoiceInteractionService {
    private static final String TAG = "VoiceWakeupInteractionService";
    private static final boolean DBG_TOAST = false;
    private AlwaysOnHotwordDetector mHotwordDetector = null;
    private VoiceWakeupBusiness mVoiceWakeupBusiness;
    private IVoiceWakeupInteractionCallback mVoiceInteractionCallback;
    private int mHotwordAvailability = 0;

    @Override
    public IBinder onBind(Intent intent) {
        if ((VoiceWakeupBusiness.VOICE_WAKEUP_SERVICE_ACTION).equals(intent.getAction())) {
            return mBinder;
        }
        return super.onBind(intent);
    }

    private final Callback mHotwordCallback = new Callback() {
        @Override
        public void onAvailabilityChanged(int status) {
            Log.d(TAG, "[onAvailabilityChanged](" + status + ")");
            hotwordAvailabilityChangeHelper(status);
        }

        @Override
        public void onDetected(EventPayload eventPayload) {
            Log.d(TAG, "[onDetected]");
            hotwordDetectedHelper();
        }

        @Override
        public void onError() {
            Log.d(TAG, "[onError]");
        }

        @Override
        public void onRecognitionPaused() {
            Log.d(TAG, "[onRecognitionPaused]");
        }

        @Override
        public void onRecognitionResumed() {
            Log.d(TAG, "[onRecognitionResumed]");
        }
    };

    @Override
    public void onReady() {
        super.onReady();
        Log.d(TAG, "Creating " + this);
        Log.d(TAG, "Keyphrase enrollment error? " + getKeyphraseEnrollmentInfo().getParseError());
        Log.d(TAG,
                "Keyphrase enrollment meta-data: "
                        + Arrays.toString(getKeyphraseEnrollmentInfo().listKeyphraseMetadata()));
        if (mHotwordDetector == null) {
        /*We need to ensure that the HotWordDetector restart is avoided for IPO boot case*/
            mHotwordDetector = createAlwaysOnHotwordDetector(VoiceTrainingEnrollmentUtil.TEXT,
                Locale.forLanguageTag(VoiceTrainingEnrollmentUtil.BCP47_LOCALE), mHotwordCallback);
        } else {
            Log.d(TAG, "[onReady] mHotwordDetector is already NOT NULL");
        }
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Bundle args = new Bundle();
//        args.putParcelable("intent", new Intent(this, TestInteractionActivity.class));
//        startSession(args);
//        stopSelf(startId);
        Log.d(TAG, "onStartCommand ");
        return START_NOT_STICKY;
    }

    /**
     * Handle hotword availability change message.
     *
     * @param availability
     *            state of keyphrase availability
     */
    private void hotwordAvailabilityChangeHelper(int availability) {
        mHotwordAvailability = availability;
        switch (availability) {
        case AlwaysOnHotwordDetector.STATE_HARDWARE_UNAVAILABLE:
            break;

        case AlwaysOnHotwordDetector.STATE_KEYPHRASE_UNSUPPORTED:
            break;

        case AlwaysOnHotwordDetector.STATE_KEYPHRASE_UNENROLLED:
            Intent enroll = mHotwordDetector.createEnrollIntent();
            Log.d(TAG, "[hotwordAvailabilityChangeHelper]Need to enroll with " + enroll);
            break;

        case AlwaysOnHotwordDetector.STATE_KEYPHRASE_ENROLLED:
            if (VoiceWakeupBusiness.getWakeupCmdStatus(getApplicationContext())
                   == VoiceCommandListener.VOICE_WAKEUP_STATUS_COMMAND_CHECKED) {
                Log.d(TAG, "[hotwordAvailabilityChangeHelper] starting recognition");
                startRecognitionInternal();
            }
            break;

         default:
            break;
        }
    }

    /**
     * Start recognition.
     *
     * @return boolean success
     */
    public boolean startRecognitionInternal() {
        if (mHotwordDetector == null) {
            Log.e(TAG, "[startRecognition] mHotwordDetector is null ");
            return false;
        }
        boolean success = false;
        try {
            success = mHotwordDetector
                    .startRecognition(AlwaysOnHotwordDetector.RECOGNITION_FLAG_NONE);
        } catch (UnsupportedOperationException e) {
            Log.e(TAG, "[startRecognition] exception: " + e.getMessage());
        } catch (IllegalStateException e) {
            Log.e(TAG, "[startRecognition] exception: " + e.getMessage());
        }
        Log.d(TAG, "[startRecognition]: " + success);
        if (!success) {
            if (DBG_TOAST) {
                Toast.makeText(getApplicationContext(), "Fail to startRecognition",
                    Toast.LENGTH_SHORT).show();
            }
            Log.d(TAG, "Fail to startRecognition");
        }
        return success;
    }

    /**
     * Stop recognition.
     *
     * @return boolean success
     */
    public boolean stopRecognitionInternal() {
        if (mHotwordDetector == null) {
            Log.e(TAG, "[stopRecognition] mHotwordDetector is null ");
            return false;
        }
        boolean success = false;
        try {
            success = mHotwordDetector.stopRecognition();
        } catch (UnsupportedOperationException e) {
            Log.e(TAG, "[stopRecognition] exception: " + e.getMessage());
        } catch (IllegalStateException e) {
            Log.e(TAG, "[stopRecognition] exception: " + e.getMessage());
        }
        Log.d(TAG, "[stopRecognition]: " + success);
        if (!success) {
            if (DBG_TOAST) {
                Toast.makeText(getApplicationContext(), "Fail to stopRecognition",
                    Toast.LENGTH_SHORT).show();
            }
            Log.d(TAG, "Fail to stopRecognition");
        }
        return success;
    }

    /**
     * It is used to implement the VoiceWakeupInteractionService public methods.
     */
    private IVoiceWakeupInteractionService.Stub mBinder =
            new IVoiceWakeupInteractionService.Stub() {
        @Override
        public void registerCallback(IVoiceWakeupInteractionCallback callback) {
            mVoiceInteractionCallback = callback;
        }

        @Override
        public boolean startRecognition() {
            if (mHotwordAvailability == AlwaysOnHotwordDetector.STATE_KEYPHRASE_ENROLLED) {
                return startRecognitionInternal();
            }
            return false;
        }

        @Override
        public boolean stopRecognition() {
            if (mHotwordAvailability == AlwaysOnHotwordDetector.STATE_KEYPHRASE_ENROLLED) {
                return stopRecognitionInternal();
            }
            return false;
        }
    };

    /**
     * Handle hotword detected message.
     */
    private void hotwordDetectedHelper() {
        // Only commandId = 0 support.
        int commandId = 0;
        try {
            if (mVoiceInteractionCallback != null) {
                mVoiceInteractionCallback.onVoiceWakeupDetected(commandId);
            }
        } catch (RemoteException e) {
            Log.e(TAG, "[hotwordDetectedHelper]exception: " + e.getMessage());
        }
        // In Google design once voice detected, Soundtrigger will be set up
        // mStart=false and Native will be set up IDLE.
        // But in mtk design, we need detect continuity, so workaround as:
        // when detected, startRecognition once again.
        startRecognitionInternal();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        mVoiceInteractionCallback = null;
        mHotwordAvailability = 0;
    }
}
