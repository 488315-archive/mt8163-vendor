#
# libacdk
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

sinclude $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/mtkcam.mk
sinclude $(TOP)/$(MTK_MTKCAM_PLATFORM)/mtkcam.mk

#
LOCAL_SRC_FILES := \
    acdk/AcdkBase.cpp \
    acdk/AcdkIF.cpp \
    acdk/AcdkMain.cpp \
    acdk/AcdkMhalBase.cpp \
    acdk/AcdkMhalEng.cpp \
    acdk/AcdkMhalPure.cpp \
    acdk/AcdkUtility.cpp \
    surfaceview/AcdkSurfaceView.cpp \
    surfaceview/surfaceView.cpp \
    main.cpp \
    test_preview.cpp
#
LOCAL_C_INCLUDES += \
    $(TOP)/$(MTK_MTKCAM_PLATFORM)/hardware/mtkcam \
    $(TOP)/$(MTK_MTKCAM_PLATFORM)/hardware/mtkcam/hal/sensor/test/inc/acdk \
    $(TOP)/$(MTK_MTKCAM_PLATFORM)/hardware/mtkcam/core/imageio/inc \
    $(TOP)/$(MTK_MTKCAM_PLATFORM)/hardware/mtkcam/core/imageio_common/inc \
    $(TOP)/$(MTK_MTKCAM_PLATFORM)/hardware/mtkcam/v1/hal/adapter/inc \
    $(TOP)/$(MTK_MTKCAM_PLATFORM)/hardware/mtkcam/core/featureio/pipe/aaa/isp_tuning \
    $(TOP)/$(MTK_MTKCAM_PLATFORM)/hardware/mtkcam/core/featureio/pipe/aaa/ae_mgr \
    $(TOP)/$(MTK_MTKCAM_PLATFORM)/hardware/mtkcam/core/featureio/pipe/aaa \
    $(TOP)/$(MTK_PATH_SOURCE)/hardware/m4u/$(PLATFORM) \
    $(TOP)/$(MTK_MTKCAM_PLATFORM)/kernel/core/include/mach \
    $(TOP)/$(MTK_MTKCAM_PLATFORM)/external/ldvt/include \
    #    $(TOP)/$(MTK_PATH_COMMON)/kernel/imgsensor/inc
LOCAL_C_INCLUDES += \
    $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/kernel/imgsensor/inc \
    $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/isp_tuning \
    $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/aaa \
    $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/debug_exif/cam \
    $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/custom/hal/inc \
    $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/debug_exif/aaa \
    $(TOP)/bionic/libc/kernel/common/linux/mt6582 \
    $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/cgen/inc \
    $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/cgen/cfgfileinc \
    $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/hal/inc \
    $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/hal/imgsensor \
    $(TOP)/$(MTK_MTKCAM_PLATFORM)/include/mtkcam \
LOCAL_C_INCLUDES += $(TOP)/system/media/camera/include
# Add a define value that can be used in the code to indicate that it's using LDVT now.
# For print message function when using LDVT.
# Note: It will be cleared by "CLEAR_VARS", so if it is needed in other module, it
# has to be set in other module again.
ifeq ($(BUILD_MTK_LDVT),true)
    LOCAL_CFLAGS += -DUSING_MTK_LDVT
endif

#
LOCAL_WHOLE_STATIC_LIBRARIES := \
    libcct \

ifeq ($(BUILD_MTK_LDVT),true)
   LOCAL_WHOLE_STATIC_LIBRARIES += libuvvf
endif
#

#
LOCAL_SHARED_LIBRARIES := \
    libcutils \
    libimageio \
    libcamdrv \
    libm4u \
    libcam.exif \
    libcamalgo \
    libcam.halsensor


ifneq ($(BUILD_MTK_LDVT),true)
LOCAL_SHARED_LIBRARIES += libfeatureio
LOCAL_SHARED_LIBRARIES += libcameracustom
LOCAL_SHARED_LIBRARIES += libcam.camshot
endif
LOCAL_SHARED_LIBRARIES += liblog
#
LOCAL_PRELINK_MODULE := false

#
LOCAL_MODULE := test_preview

#

#
# Start of common part ------------------------------------

#-----------------------------------------------------------
LOCAL_CFLAGS += $(MTKCAM_CFLAGS)

#-----------------------------------------------------------
LOCAL_C_INCLUDES += $(MTKCAM_C_INCLUDES)
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/gralloc_extra/include
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/ext/include


#-----------------------------------------------------------
LOCAL_C_INCLUDES +=$(TOP)/$(MTK_MTKCAM_PLATFORM)/include
LOCAL_C_INCLUDES +=$(TOP)/$(MTKCAM_C_INCLUDES)
LOCAL_C_INCLUDES +=$(TOP)/$(MTKCAM_C_INCLUDES)/..

LOCAL_C_INCLUDES += $(TOP)/$(MTK_MTKCAM_PLATFORM)/include

# End of common part ---------------------------------------
#
#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_EXECUTABLE)

#
include $(call all-makefiles-under, $(LOCAL_PATH))
