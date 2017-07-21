LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ifeq ($(MTK_CAM_HDR_SUPPORT), yes)

-include $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/mtkcam.mk

LOCAL_MODULE := libcam.camadapter.scenario.shot.hdrshot

LOCAL_ADDITIONAL_DEPENDENCIES := $(LOCAL_PATH)/Android.mk

MTK_PATH_CAM := $(MTK_PATH_SOURCE)/hardware/mtkcam

LOCAL_SRC_FILES := HDRShot.cpp

LOCAL_C_INCLUDES := \
	system/media/camera/include \
	$(MTKCAM_C_INCLUDES) \
	$(MTK_PATH_CAM) \
	$(MTK_PATH_CAM)/common/include \
	$(MTK_PATH_CAM)/include/$(PLATFORM) \
	$(MTK_PATH_CAM)/middleware/common/include \
	$(MTKCAM_HAL_INCLUDE) \
	$(MTKCAM_HAL_INCLUDE)/../v1/adapter/inc/Scenario \
	$(MTKCAM_HAL_INCLUDE)/../v1/adapter/Scenario/Shot/inc \
	$(MTKCAM_HAL_INCLUDE)/../v1/camshot/inc \
	$(MTKCAM_3A_INCLUDE) \
	$(MTK_PATH_SOURCE)/hardware/mtkcam/ext/include \
	$(MTK_PATH_SOURCE)/hardware/mtkcam/feature/include \
	$(MTK_PATH_SOURCE)/hardware/gralloc_extra/include \
	$(MTK_PATH_SOURCE)/custom/$(TARGET_BOARD_PLATFORM)/hal/inc/isp_tuning

LOCAL_MODULE_TAGS := optional

LOCAL_CFLAGS := -DLOG_TAG=\"hdrshot\"

LOCAL_CFLAGS += -DUSE_SYSTRACE

include $(BUILD_STATIC_LIBRARY)

endif # MTK_CAM_HDR_SUPPORT
