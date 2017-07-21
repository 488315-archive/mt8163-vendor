LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_SRC_FILES += src/com/mediatek/voicecommand/service/IVoiceWakeupInteractionService.aidl \
                   src/com/mediatek/voicecommand/service/IVoiceWakeupInteractionCallback.aidl

LOCAL_PACKAGE_NAME := VoiceCommand
# Enroll sound model to Native, must need a priv app.
LOCAL_PRIVILEGED_MODULE := true

LOCAL_JNI_SHARED_LIBRARIES := libvoicerecognition_jni
LOCAL_MULTILIB := 32
#LOCAL_CERTIFICATE := media
LOCAL_CERTIFICATE := platform

LOCAL_PROGUARD_FLAG_FILES := proguard.flags

LOCAL_EMMA_COVERAGE_FILTER := +com.mediatek.voicecommand.adapter.*\
                              +com.mediatek.voicecommand.business.*\
                              +com.mediatek.voicecommand.cfg.*\
                              +com.mediatek.voicecommand.data.*\
                              +com.mediatek.voicecommand.mgr.*\
                              +com.mediatek.voicecommand.service.*\
                              +com.mediatek.voicecommand.voicesettings.*

#EMMA_INSTRUMENT := true

include $(BUILD_PACKAGE)

# Use the following include to make our test apk.
include $(call all-makefiles-under,$(LOCAL_PATH))