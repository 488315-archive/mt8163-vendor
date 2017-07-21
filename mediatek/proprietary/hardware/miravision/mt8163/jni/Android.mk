LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	miravision_jni.cpp
	 
LOCAL_C_INCLUDES := $(JNI_H_INCLUDE)

LOCAL_C_INCLUDES += \
        $(TOP)/bionic \
        $(MTK_PATH_SOURCE)/kernel/include \
        $(MTK_PATH_PLATFORM)/kernel/drivers/dispsys \
        $(MTK_PATH_SOURCE)/hardware/pq/mt8163 \
        $(MTK_PATH_SOURCE)/hardware/pq/mt8163/inc \
        $(MTK_PATH_SOURCE)/hardware/aal/mt8163/inc \

	
LOCAL_SHARED_LIBRARIES := \
	libcutils \
	libutils \
	libpq_cust \
	libdpframework

ifeq ($(strip $(MTK_AAL_SUPPORT)),yes)
    LOCAL_CFLAGS += -DMTK_AAL_SUPPORT

    LOCAL_SHARED_LIBRARIES += \
        libaal
endif

ifeq ($(strip $(MTK_OD_SUPPORT)),yes)
    LOCAL_CFLAGS += -DMTK_OD_SUPPORT
endif

LOCAL_PRELINK_MODULE := false

LOCAL_MODULE := libMiraVision_jni
LOCAL_MULTILIB := both

include $(BUILD_SHARED_LIBRARY)
