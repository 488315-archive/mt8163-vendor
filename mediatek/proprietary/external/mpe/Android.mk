# Copyright 2005 The Android Open Source Project
#LOCAL_PATH := $(my-dir)
LOCAL_PATH := $(call my-dir)

########################################
# Copy prebuilt SensorListener share library
########################################
include $(CLEAR_VARS)
LOCAL_MODULE := libmpe.sensorlistener
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SRC_FILES := lib/libmpe.sensorlistener.so
LOCAL_MODULE_SUFFIX := .so
LOCAL_MULTILIB := 32
include $(BUILD_PREBUILT)

########################################
# Copy prebuilt SensorDriver share library
########################################
include $(CLEAR_VARS)
LOCAL_MODULE := libmpe.driver
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SRC_FILES := lib/libmpe.driver.so
LOCAL_MODULE_SUFFIX := .so
LOCAL_MULTILIB := 32
include $(BUILD_PREBUILT)

########################################
# Copy prebuilt SensorFusion share library
########################################
include $(CLEAR_VARS)
LOCAL_MODULE := libfusion
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SRC_FILES := lib/libfusion.so
LOCAL_MODULE_SUFFIX := .so
LOCAL_MULTILIB := 32
include $(BUILD_PREBUILT)

########################################
# Copy prebuilt SensorDR share library
########################################
include $(CLEAR_VARS)
LOCAL_MODULE := libDR
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SRC_FILES := lib/libDR.so
LOCAL_MODULE_SUFFIX := .so
LOCAL_MULTILIB := 32
include $(BUILD_PREBUILT)

########################################
# Build MPE daemon executable
########################################
include $(CLEAR_VARS)
MY_LOCAL_PATH := $(LOCAL_PATH)

# compile sensor_explorer
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
   scr/mpe_sys.c \
   scr/mpe_main.c 

LOCAL_C_INCLUDES:= \
   $(LOCAL_PATH)/inc 
LOCAL_MODULE := MPED
LOCAL_MULTILIB := 32
LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_PRELINK_MODULE := false
LOCAL_UNSTRIPPED_PATH := $(TARGET_ROOT_OUT_SBIN_UNSTRIPPED)
#LOCAL_CFLAGS += -DUSE_GCC -DSUPPORT_SHARED_LIBRARY
LOCAL_LDLIBS:= -llog
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES += liblog libmpe.driver libmpe.sensorlistener libfusion libDR
LOCAL_ALLOW_UNDEFINED_SYMBOLS:=true

include $(BUILD_EXECUTABLE)

