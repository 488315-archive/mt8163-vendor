LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ifeq ($(MTK_CAM_HDR_SUPPORT), yes)

ifeq ($(TARGET_BOARD_PLATFORM), $(filter $(TARGET_BOARD_PLATFORM), mt6797))

-include $(MTK_PATH_SOURCE)/hardware/mtkcam/mtkcam.mk

LOCAL_MODULE := libhdrproc

LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/Android.mk

MTK_PATH_CAM := $(MTK_PATH_SOURCE)/hardware/mtkcam
MTK_PATH_HDR_PLATFORM := ../$(TARGET_BOARD_PLATFORM)

LOCAL_SRC_FILES := \
	HDRProc.cpp \
	HDRAlgo.cpp \
	HDRUtils.cpp \
	hal/hdr_hal_base.cpp \
	hal/hdr_hal.cpp \
	$(MTK_PATH_HDR_PLATFORM)/Platform.cpp \
	utils/ImageBufferUtils.cpp \
	utils/Timer.cpp

LOCAL_C_INCLUDES := \
	system/media/camera/include \
	$(MTK_PATH_CAM) \
	$(MTK_PATH_CAM)/common/include \
	$(MTK_PATH_CAM)/utils/include \
	$(MTK_PATH_CAM)/middleware/common/include \
	$(MTK_PATH_CAM)/feature/include \
	$(MTK_PATH_SOURCE)/hardware/gralloc_extra/include \
	$(MTKCAM_HAL_INCLUDE)/.. \
	$(MTKCAM_3A_INCLUDE) \
	$(MTKCAM_3A_INCLUDE)/Hal3 \
	$(MTK_PATH_CUSTOM_PLATFORM)/hal/inc \
	$(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/isp_tuning

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libcamalgo \
	libcam_utils \
	libcam.iopipe \
	libcameracustom \
	libcam.hal3a.v3 \
	libcam.halsensor

ifeq ($(TARGET_BOARD_PLATFORM), $(filter $(TARGET_BOARD_PLATFORM), mt6797))
LOCAL_C_INCLUDES += \
	$(MTK_PATH_CAM)/drv/include/$(TARGET_BOARD_PLATFORM) \
	$(MTK_PATH_CAM)/drv/include/$(TARGET_BOARD_PLATFORM)/iopipe \
	$(MTK_PATH_CAM)/include/algorithm/$(TARGET_BOARD_PLATFORM) \
	$(LOCAL_PATH)/../$(TARGET_BOARD_PLATFORM)
endif

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS := -DLOG_TAG=\"hdrproc\"

LOCAL_CFLAGS += -DUSE_SYSTRACE

# workaroud to judge whether libperfservicenative exists nor not
# remove this condition after MTK_PERFSERVICE_SUPPORT works as expected
ifneq ($(MTK_BASIC_PACKAGE), yes)
LOCAL_CFLAGS += -DUSE_PERFSERVICE
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/perfservice
LOCAL_SHARED_LIBRARIES += libperfservicenative
endif

include $(BUILD_SHARED_LIBRARY)

endif # TARGET_BOARD_PLATFORM

endif # MTK_CAM_HDR_SUPPORT
