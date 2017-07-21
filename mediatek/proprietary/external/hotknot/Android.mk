LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := hotknot_native_service
ifneq ($(MTK_K64_SUPPORT), yes)
LOCAL_SRC_FILES := hotknot_native/arm/hotknot_native_service
else
LOCAL_SRC_FILES := hotknot_native/arm64/hotknot_native_service
endif
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
include $(BUILD_PREBUILT)
######################################################################
#LOCAL_PATH := $(my-dir)

include $(CLEAR_VARS)

DEFAULT_PLATFORM := common
LOCAL_MODULE := libhotknot_dev
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
ifeq ($(wildcard $(LOCAL_PATH)/hotknot_dev/$(TARGET_BOARD_PLATFORM)),)
    LOCAL_SRC_FILES_arm := hotknot_dev/$(DEFAULT_PLATFORM)/lib/libhotknot_dev.so
    LOCAL_SRC_FILES_arm64 := hotknot_dev/$(DEFAULT_PLATFORM)/lib64/libhotknot_dev.so
else
    LOCAL_SRC_FILES_arm := hotknot_dev/$(TARGET_BOARD_PLATFORM)/lib/libhotknot_dev.so
    LOCAL_SRC_FILES_arm64 := hotknot_dev/$(TARGET_BOARD_PLATFORM)/lib64/libhotknot_dev.so
endif
LOCAL_MODULE_SUFFIX := .so
LOCAL_MULTILIB := both
include $(BUILD_PREBUILT)

