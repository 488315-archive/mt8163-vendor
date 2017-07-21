LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_IS_HOST_MODULE =
LOCAL_MODULE = libstagefright_hdcprx
LOCAL_MODULE_CLASS = SHARED_LIBRARIES
LOCAL_MODULE_PATH =
LOCAL_MODULE_RELATIVE_PATH =
LOCAL_MODULE_SUFFIX = .so
LOCAL_SHARED_LIBRARIES_64 = libstagefright libstagefright_foundation libtz_uree liburee_meta_drmkeyinstall_v2 libgui libhardware libc++
LOCAL_MULTILIB = 64
LOCAL_SRC_FILES_64 = libstagefright_hdcprx.so
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_IS_HOST_MODULE =
LOCAL_MODULE = libstagefright_hdcprx
LOCAL_MODULE_CLASS = SHARED_LIBRARIES
LOCAL_MODULE_PATH =
LOCAL_MODULE_RELATIVE_PATH =
LOCAL_MODULE_SUFFIX = .so
LOCAL_SHARED_LIBRARIES_32 = libstagefright libstagefright_foundation libtz_uree liburee_meta_drmkeyinstall_v2 libgui libhardware libc++
LOCAL_MULTILIB = 32
LOCAL_SRC_FILES_32 = arm/libstagefright_hdcprx.so
include $(BUILD_PREBUILT)
