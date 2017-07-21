LOCAL_PATH:= $(call my-dir)


#
# libmmsdkservice
#

include $(CLEAR_VARS)

#-----------------------------------------------------------
-include $(TOP)/$(MTK_PATH_SOURCE)/frameworks/av/services/mmsdk/libmmsdkservice/mmsdk.mk
-include $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/mtkcam.mk

LOCAL_SRC_FILES += $(call all-c-cpp-files-under, .)

LOCAL_SRC_FILES := ./MMSdkService.cpp \
                   ./IMMSdkService.cpp \
                   ./IFeatureManager.cpp \
                   ./FeatureManager.cpp \


LOCAL_SHARED_LIBRARIES += \
    libui \
    liblog \
    libutils \
    libbinder \
    libcutils \
    libhardware \
    libsync \

ifneq ($(strip $(MTK_EMULATOR_SUPPORT)),yes)
ifeq "'1'" "$(strip $(MTKCAM_HAVE_SDK_CLIENT))"
LOCAL_SHARED_LIBRARIES += libcam3_utils
endif
endif


#ifneq ($(strip $(MTK_EMULATOR_SUPPORT)),yes)
#LOCAL_SHARED_LIBRARIES += libmmsdkservice.feature
#endif


#LOCAL_SHARED_LIBRARIES += libcam.camadapter	#remove later

LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/ext/include
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/frameworks/av/services/mmsdk/include/
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/frameworks/av/services/mmsdk/include/mmsdk
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/frameworks/av/include/
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/include/
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mmsdk/include/
LOCAL_C_INCLUDES += $(TOP)/$(MTKCAM_C_INCLUDES)
LOCAL_C_INCLUDES += $(TOP)/system/media/camera/include
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/gralloc_extra/include/
#
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/frameworks-ext/av/include
LOCAL_C_INCLUDES += $(MTK_PATH_PLATFORM)/hardware/include # remove later

ifeq ($(strip $(TARGET_BOARD_PLATFORM)), mt6797)
#swo
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/common/include
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/common/include/device
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/middleware/common/include
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/utils/include
else
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/legacy/include/mtkcam/utils
#For v3 FeatureMgr.h
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/legacy/include/

endif



LOCAL_CFLAGS += -Wall -Wextra


LOCAL_WHOLE_STATIC_LIBRARIES += libmmsdk.client.imageTransform
LOCAL_WHOLE_STATIC_LIBRARIES += libmmsdk.client.effect
LOCAL_WHOLE_STATIC_LIBRARIES += libmmsdk.client.gesture
LOCAL_WHOLE_STATIC_LIBRARIES += libmmsdk.client.heartrate


################################################################################
LOCAL_MODULE:= libmmsdkservice


include $(BUILD_SHARED_LIBRARY)


################################################################################
#
################################################################################
include $(CLEAR_VARS)
include $(call all-makefiles-under,$(LOCAL_PATH))

