LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
	main_pq.cpp 

LOCAL_C_INCLUDES += \
        $(TOP)/bionic \
        $(TOP)/frameworks/base/include \
        $(MTK_PATH_PLATFORM)/kernel/drivers/dispsys \
        $(MTK_PATH_SOURCE)/hardware/pq/mt8163 \
        $(MTK_PATH_SOURCE)/hardware/pq/mt8163/inc \


LOCAL_SHARED_LIBRARIES := \
    libutils \
    libcutils \
    libpq_cust \
    libdpframework

ifeq ($(MTK_BLUE_LIGHT_FILTER_SUPPORT), yes)
	LOCAL_CFLAGS += -DBLUE_LIGHT_FILTER
LOCAL_SHARED_LIBRARIES += \
	libblf
endif

LOCAL_MODULE:= pq

LOCAL_MODULE_CLASS := EXECUTABLES

include $(BUILD_EXECUTABLE)

include $(call all-makefiles-under,$(LOCAL_PATH))
