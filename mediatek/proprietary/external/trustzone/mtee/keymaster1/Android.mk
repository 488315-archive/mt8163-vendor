LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := keystore.$(TARGET_BOARD_PLATFORM)
LOCAL_CFLAGS += -fvisibility=hidden -DLOG_TAG=\"KM_UREE\"
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_SRC_FILES := \
       android_keymaster.cpp  \
       android_keymaster_messages.cpp \
	   android_keymaster_utils.cpp \
	   authorization_set.cpp \
	   keymaster_mtee_api.cpp \
	   serializable.cpp \
	   keymaster_hal.cpp

LOCAL_C_INCLUDES += \
    hardware/libhardware/include \
	vendor/mediatek/proprietary/external/trustzone/mtee/include \
	$(LOCAL_PATH)/include \
	$(LOCAL_PATH)/include/keymaster

LOCAL_SHARED_LIBRARIES += libcrypto liblog libutils libtz_uree

include $(BUILD_SHARED_LIBRARY)

ifeq ($(MTK_KEYMASTER_TEST_SUPPORT),yes)
# Unit tests for libkeymaster
include $(CLEAR_VARS)
LOCAL_MODULE := mtee_keymaster_tests
LOCAL_CFLAGS += -DLOG_TAG=\"KM_TEST\"
LOCAL_SRC_FILES := \
	test/keymaster_test.cpp

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include \
	$(LOCAL_PATH)/test \
	hardware/libhardware/include \
	system/security/softkeymaster/include

LOCAL_CLANG_CFLAGS += -Wno-error=unused-const-variable -Wno-error=unused-private-field
LOCAL_MODULE_TAGS := tests
LOCAL_SHARED_LIBRARIES := \
	libcrypto \
	libsoftkeymaster \
	libhardware \
	libutils \
	libcutils \
	libtz_uree

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/keymaster/ \
									  $(LOCAL_PATH)/include/

LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/Android.mk
include $(BUILD_NATIVE_TEST)
endif
