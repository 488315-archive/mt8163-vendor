LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	pqdc_tuning_jni.cpp
	 
LOCAL_C_INCLUDES := $(JNI_H_INCLUDE)

LOCAL_C_INCLUDES += \
        $(TOP)/bionic \
        $(TOP)/frameworks/base/include \
        $(MTK_PATH_PLATFORM)/kernel/drivers/dispsys \
        $(MTK_PATH_SOURCE)/hardware/pq/mt8163 \
        $(MTK_PATH_SOURCE)/hardware/pq/mt8163/inc \

	
LOCAL_SHARED_LIBRARIES := \
    libutils \
    libcutils \
    libdpframework

LOCAL_MODULE := libPQDCjni
LOCAL_MULTILIB := both

include $(BUILD_SHARED_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))
