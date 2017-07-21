LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_IS_HOST_MODULE =
LOCAL_MODULE = libvtmal
LOCAL_MODULE_CLASS = SHARED_LIBRARIES
LOCAL_MODULE_PATH =
LOCAL_MODULE_RELATIVE_PATH =
LOCAL_MODULE_SUFFIX = .so
LOCAL_SHARED_LIBRARIES_64 = libbinder libmedia libui libcamera_client libgui libskia libblisrc libstagefright libc++
LOCAL_MULTILIB = 64
LOCAL_SRC_FILES_64 = libvtmal.so
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_IS_HOST_MODULE =
LOCAL_MODULE = libvtmal
LOCAL_MODULE_CLASS = SHARED_LIBRARIES
LOCAL_MODULE_PATH =
LOCAL_MODULE_RELATIVE_PATH =
LOCAL_MODULE_SUFFIX = .so
LOCAL_SHARED_LIBRARIES_32 = libbinder libmedia libui libcamera_client libmtkcamera_client libgui libskia libamrvt libblisrc libvt_custom libstagefright libamr_wrap libawb_wrap libc++
LOCAL_MULTILIB = 32
LOCAL_SRC_FILES_32 = arm/libvtmal.so
include $(BUILD_PREBUILT)
