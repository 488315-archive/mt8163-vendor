LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_IS_HOST_MODULE =
LOCAL_MODULE = libwvdrmengine
LOCAL_MODULE_CLASS = SHARED_LIBRARIES
LOCAL_MODULE_PATH = $(PRODUCT_OUT)/system/vendor/lib/mediadrm
LOCAL_MODULE_RELATIVE_PATH =
LOCAL_MODULE_SUFFIX = .so
LOCAL_SHARED_LIBRARIES_32 = libprotobuf-cpp-lite libstagefright_foundation libc++
LOCAL_MULTILIB = 32
LOCAL_SRC_FILES_32 = arm/libwvdrmengine.so
include $(BUILD_PREBUILT)
