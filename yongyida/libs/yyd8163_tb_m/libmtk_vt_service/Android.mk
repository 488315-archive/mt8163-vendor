LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_IS_HOST_MODULE =
LOCAL_MODULE = libmtk_vt_service
LOCAL_MODULE_CLASS = SHARED_LIBRARIES
LOCAL_MODULE_PATH =
LOCAL_MODULE_RELATIVE_PATH =
LOCAL_MODULE_SUFFIX = .so
LOCAL_SHARED_LIBRARIES_64 = libandroid_runtime libbinder libnativehelper libui libgui libmedia libimsma libvtmal libsource libimsma_rtp libmtk_vt_swip libvt_avsync libvt_socketbind libmtk_vt_utils libstagefright_foundation libc++
LOCAL_MULTILIB = 64
LOCAL_SRC_FILES_64 = libmtk_vt_service.so
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_IS_HOST_MODULE =
LOCAL_MODULE = libmtk_vt_service
LOCAL_MODULE_CLASS = SHARED_LIBRARIES
LOCAL_MODULE_PATH =
LOCAL_MODULE_RELATIVE_PATH =
LOCAL_MODULE_SUFFIX = .so
LOCAL_SHARED_LIBRARIES_32 = libandroid_runtime libbinder libnativehelper libui libgui libmedia libimsma libvtmal libsource libimsma_rtp libmtk_vt_swip libvt_avsync libvt_socketbind libmtk_vt_utils libstagefright_foundation libc++
LOCAL_MULTILIB = 32
LOCAL_SRC_FILES_32 = arm/libmtk_vt_service.so
include $(BUILD_PREBUILT)
