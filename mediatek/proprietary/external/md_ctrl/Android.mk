LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := md_ctrl.cpp

LOCAL_C_INCLUDES := system/core/logwrapper/include

LOCAL_MODULE := md_ctrl
LOCAL_SYSTEM_SHARED_LIBRARIES := libc liblogwrap liblog libcutils

include $(BUILD_EXECUTABLE)
