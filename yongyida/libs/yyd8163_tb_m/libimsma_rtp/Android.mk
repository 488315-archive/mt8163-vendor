LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_IS_HOST_MODULE =
LOCAL_MODULE = libimsma_rtp
LOCAL_MODULE_CLASS = SHARED_LIBRARIES
LOCAL_MODULE_PATH =
LOCAL_MODULE_RELATIVE_PATH =
LOCAL_MODULE_SUFFIX = .so
LOCAL_SHARED_LIBRARIES_64 = libbinder libmedia libui libgui libskia libstagefright libstagefright_foundation libimsma_socketwrapper libc++
LOCAL_MULTILIB = 64
LOCAL_SRC_FILES_64 = libimsma_rtp.so
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_IS_HOST_MODULE =
LOCAL_MODULE = libimsma_rtp
LOCAL_MODULE_CLASS = SHARED_LIBRARIES
LOCAL_MODULE_PATH =
LOCAL_MODULE_RELATIVE_PATH =
LOCAL_MODULE_SUFFIX = .so
LOCAL_SHARED_LIBRARIES_32 = libbinder libmedia libui libgui libskia libstagefright libstagefright_foundation libimsma_socketwrapper libc++
LOCAL_MULTILIB = 32
LOCAL_SRC_FILES_32 = arm/libimsma_rtp.so
include $(BUILD_PREBUILT)
