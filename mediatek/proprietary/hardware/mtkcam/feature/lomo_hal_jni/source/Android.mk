
LOCAL_PATH := $(call my-dir)

################################################################################
#
################################################################################
include $(CLEAR_VARS)

-include $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/mtkcam.mk
#After Jonas' review in KK
#-----------------------------------------------------------
#------------iopipeFrmBtest----------------------------
#-----------------------------------------------------------
LOCAL_SRC_FILES += $(call all-c-cpp-files-under, .)

#LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/legacy/include
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/gralloc_extra/include
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/common/include
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam



#-----------------------------------------------------------
#
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/hal/inc
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/aaa
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/lomo
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/isp_tuning
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/lomo_jni
#Thread Priority
LOCAL_C_INCLUDES += $(TOP)/system/core/include

#feature
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/feature
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/feature/lomo_hal_jni/include
#LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/feature/include/lomo_hal_jni


#for NSIspTuningv3>
# TODO
#LOCAL_C_INCLUDES += $(MTK_PATH_PLATFORM)/hardware/mtkcam/hal/aaa




#LOCAL_C_INCLUDES += $(MTK_PATH_PLATFORM)/hardware/mtkcam/hal/aaa/isp_mgr

#LOCAL_C_INCLUDES += $(MTK_PATH_PLATFORM)/hardware/mtkcam/hal/aaa/ispdrv_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_INCLUDE)/isp_tuning
LOCAL_C_INCLUDES += $(MTKCAM_3A_INCLUDE)/Hal3
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/aaa/source/$(MTKCAM_3A_VERSION)
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/aaa/source/$(MTKCAM_3A_VERSION)/isp_tuning/paramctrl/ggm_mgr
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/aaa/source/$(MTKCAM_3A_VERSION)/isp_tuning
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/aaa/source/$(MTKCAM_3A_VERSION)/isp_mgr
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/aaa/source/$(MTKCAM_3A_VERSION)/ispdrv_mgr
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/aaa/source/$(MTKCAM_3A_VERSION)/buf_mgr
LOCAL_C_INCLUDES += $(MTKCAM_HAL_INCLUDE)

#LOCAL_C_INCLUDES += $(MTK_PATH_PLATFORM)/hardware/mtkcam/hal/aaa/isp_tuning/paramctrl/inc
#LOCAL_C_INCLUDES += $(MTK_PATH_PLATFORM)/hardware/mtkcam/hal/aaa/isp_tuning/paramctrl/pca_mgr
#LOCAL_C_INCLUDES += $(MTK_PATH_PLATFORM)/hardware/mtkcam/hal/aaa/isp_tuning/paramctrl/ccm_mgr
#LOCAL_C_INCLUDES += $(MTK_PATH_PLATFORM)/hardware/mtkcam/hal/aaa/isp_tuning/paramctrl/ggm_mgr
#LOCAL_C_INCLUDES += $(MTK_PATH_PLATFORM)/hardware/mtkcam/hal/aaa/lsc_mgr
#for NSIspTuningv3<
# temp for using iopipe>
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/include/$(PLATFORM)
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/drv/src/isp/$(PLATFORM)/inc
LOCAL_C_INCLUDES += $(MTKCAM_DRV_INCLUDE)
LOCAL_C_INCLUDES += $(MTKCAM_DRV_INCLUDE)/drv
LOCAL_C_INCLUDES += $(MTKCAM_DRV_INCLUDE)/imageio
LOCAL_C_INCLUDES += $(MTKCAM_DRV_INCLUDE)/iopipe/PostProc
LOCAL_C_INCLUDES += $(MTKCAM_C_INCLUDES)/metadata
LOCAL_C_INCLUDES += $(MTKCAM_ALGO_INCLUDE)
# temp for using iopipe<

#for camera_vendor_tags.h:
LOCAL_C_INCLUDES += $(TOP)/system/media/camera/include



LOCAL_SHARED_LIBRARIES := \
    libcutils \
    liblog \
    libcam_utils \
    libcam.iopipe \
    libcam.halsensor \
    libutils
LOCAL_SHARED_LIBRARIES += libstdc++

#-----------------------------------------------------------
# iopipe
LOCAL_SHARED_LIBRARIES += \
	libcam.iopipe

#-----------------------------------------------------------
LOCAL_SHARED_LIBRARIES  += libcamdrv_isp
LOCAL_SHARED_LIBRARIES  += libcamdrv_imem
LOCAL_SHARED_LIBRARIES  += libcamdrv_tuning_mgr

# camUtils
LOCAL_SHARED_LIBRARIES +=\
    libcam_utils \

# for feature stream
LOCAL_SHARED_LIBRARIES += libcameracustom
# remove aft evrest // LOCAL_SHARED_LIBRARIES += libfeatureiodrv
LOCAL_SHARED_LIBRARIES += libdpframework
# for gralloc
LOCAL_SHARED_LIBRARIES += libgralloc_extra
# for v3>
LOCAL_SHARED_LIBRARIES += libcam.hal3a.v3
# for v3<

#LOCAL_STATIC_LIBRARIES := \



#

#-----------------------------------------------------------
LOCAL_MODULE := libcam.jni.lomohaljni

LOCAL_MODULE_TAGS := optional

#-----------------------------------------------------------
LOCAL_CFLAGS += $(MTKCAM_CFLAGS)
#LOCAL_C_INCLUDES += $(MTKCAM_C_INCLUDES)
#LOCAL_C_INCLUDES += $(TOP)/$(MTK_MTKCAM_INCLUDE)
#LOCAL_C_INCLUDES += $(TOP)/$(MTK_MTKCAM_PLATFORM)/include
#LOCAL_C_INCLUDES += $(TOP)/$(MTK_MTKCAM_PLATFORM)/include/
include $(BUILD_SHARED_LIBRARY)


################################################################################
#
################################################################################
#include $(CLEAR_VARS)
include $(call all-makefiles-under,$(LOCAL_PATH))

