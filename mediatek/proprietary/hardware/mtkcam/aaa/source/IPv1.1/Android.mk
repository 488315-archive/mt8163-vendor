# Copyright Statement:
#
# This software/firmware and related documentation ("MediaTek Software") are
# protected under relevant copyright laws. The information contained herein
# is confidential and proprietary to MediaTek Inc. and/or its licensors.
# Without the prior written permission of MediaTek inc. and/or its licensors,
# any reproduction, modification, use or disclosure of MediaTek Software,
# and information contained herein, in whole or in part, shall be strictly prohibited.

# MediaTek Inc. (C) 2010. All rights reserved.
#
# BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
# THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
# RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
# AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
# NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
# SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
# SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
# THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
# THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
# CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
# SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
# STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
# CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
# AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
# OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
# MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
#
# The following software/firmware and/or related documentation ("MediaTek Software")
# have been modified by MediaTek Inc. All revisions are subject to any receiver's
# applicable license agreements with MediaTek Inc.

################################################################################
#
################################################################################

LOCAL_PATH := $(call my-dir)

################################################################################
#
################################################################################
include $(CLEAR_VARS)

#-----------------------------------------------------------
-include $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/mtkcam.mk
MTKCAM_3A_PATH    := $(MTK_PATH_SOURCE)/hardware/mtkcam/aaa/source/$(MTKCAM_3A_VERSION)

#-----------------------------------------------------------
LOCAL_SRC_FILES += Hal3AAdapter3.cpp
LOCAL_SRC_FILES += Hal3AAdapter3Meta.cpp
LOCAL_SRC_FILES += IHal3A.cpp
#LOCAL_SRC_FILES += EventIrq/DefaultEventIrq.cpp
LOCAL_SRC_FILES += EventIrq/HwEventIrq.cpp
LOCAL_SRC_FILES += aaa_hal_if.cpp
#LOCAL_SRC_FILES += aaa_hal_flowCtrl.cpp
#LOCAL_SRC_FILES += aaa_hal_flowCtrl.thread.cpp
#LOCAL_SRC_FILES += aaa_hal_raw.cpp
#LOCAL_SRC_FILES += aaa_hal_raw.thread.cpp
LOCAL_SRC_FILES += Hal3AFlowCtrl.cpp
LOCAL_SRC_FILES += Hal3AFlowCtrl_SMVR.cpp
LOCAL_SRC_FILES += Thread3AImp.cpp
LOCAL_SRC_FILES += ThreadRawImp.cpp
LOCAL_SRC_FILES += wrapper/Dft3AImp.cpp
LOCAL_SRC_FILES += wrapper/Hal3ARawImp.cpp
LOCAL_SRC_FILES += wrapper/Hal3ARawSMVRImp.cpp
LOCAL_SRC_FILES += wrapper/I3AWrapper.cpp
LOCAL_SRC_FILES += aaa_hal_sttCtrl.cpp
LOCAL_SRC_FILES += ResultBufMgr/ResultBufMgr.cpp
LOCAL_SRC_FILES += state_mgr/aaa_state.cpp
LOCAL_SRC_FILES += state_mgr/aaa_state_camera_preview.cpp
LOCAL_SRC_FILES += state_mgr/aaa_state_af.cpp
LOCAL_SRC_FILES += state_mgr/aaa_state_precapture.cpp
#LOCAL_SRC_FILES += state_mgr/aaa_state_capture.cpp
LOCAL_SRC_FILES += state_mgr/aaa_state_mgr.cpp
LOCAL_SRC_FILES += state_mgr_af/af_state_mgr.cpp
LOCAL_SRC_FILES += state_mgr_af/af_state.cpp
LOCAL_SRC_FILES += state_mgr_af/af_state_caf.cpp
LOCAL_SRC_FILES += state_mgr_af/af_state_taf.cpp
LOCAL_SRC_FILES += sensor_mgr/aaa_sensor_mgr.cpp
LOCAL_SRC_FILES += sensor_mgr/aaa_sensor_buf_mgr.cpp
LOCAL_SRC_FILES += nvram_mgr/nvram_drv_mgr.cpp
LOCAL_SRC_FILES += awb_mgr/awb_mgr.cpp
LOCAL_SRC_FILES += awb_mgr/awb_cct_feature.cpp
LOCAL_SRC_FILES += awb_mgr/awb_state.cpp
LOCAL_SRC_FILES += awb_mgr/awb_mgr_if.cpp
LOCAL_SRC_FILES += ae_mgr/ae_mgr.cpp
LOCAL_SRC_FILES += ae_mgr/ae_mgr_ctrl.cpp
LOCAL_SRC_FILES += ae_mgr/ae_mgr_pline.cpp
LOCAL_SRC_FILES += ae_mgr/ae_cct_feature.cpp
LOCAL_SRC_FILES += ae_mgr/ae_mgr_if.cpp
LOCAL_SRC_FILES += af_mgr/af_mgr.cpp
LOCAL_SRC_FILES += af_mgr/af_mgr_if.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_ctl.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_obc.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_dbs.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_bnr.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_lsc.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_rpg.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_pgn.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_udm.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_sl2.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_ccm.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_ggm.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_g2c.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_nbc.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_nbc2.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_pca.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_seee.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_nr3d.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_mfb.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_rmm.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_rmg.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_lce.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_mixer3.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_awb_stat.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_ae_stat.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_af_stat.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_flk.cpp
LOCAL_SRC_FILES += isp_mgr/isp_mgr_helper.cpp
LOCAL_SRC_FILES += isp_mgr/isp_debug.cpp
#LOCAL_SRC_FILES += buf_mgr/aao_buf_mgr.cpp
#LOCAL_SRC_FILES += buf_mgr/afo_buf_mgr.cpp
LOCAL_SRC_FILES += buf_mgr/aaa_buf_mgr.cpp
LOCAL_SRC_FILES += buf_mgr/default_buf_mgr.cpp
LOCAL_SRC_FILES += buf_mgr/StatisticBuf.cpp
LOCAL_SRC_FILES += ispdrv_mgr/ispdrv_mgr.cpp
LOCAL_SRC_FILES += isp_tuning/isp_tuning_mgr.cpp
LOCAL_SRC_FILES += isp_tuning/paramctrl/paramctrl_lifetime.cpp
LOCAL_SRC_FILES += isp_tuning/paramctrl/paramctrl_user.cpp
LOCAL_SRC_FILES += isp_tuning/paramctrl/paramctrl_attributes.cpp
LOCAL_SRC_FILES += isp_tuning/paramctrl/paramctrl_validate.cpp
LOCAL_SRC_FILES += isp_tuning/paramctrl/paramctrl_per_frame.cpp
LOCAL_SRC_FILES += isp_tuning/paramctrl/paramctrl_frameless.cpp
LOCAL_SRC_FILES += isp_tuning/paramctrl/paramctrl_exif.cpp
LOCAL_SRC_FILES += isp_tuning/paramctrl/pca_mgr/pca_mgr.cpp
LOCAL_SRC_FILES += isp_tuning/paramctrl/ccm_mgr/ccm_mgr.cpp
LOCAL_SRC_FILES += isp_tuning/paramctrl/ggm_mgr/ggm_mgr.cpp
LOCAL_SRC_FILES += isp_tuning/paramctrl/gma_mgr/gma_mgr.cpp
LOCAL_SRC_FILES += isp_tuning/capturenr/capturenr.cpp
LOCAL_SRC_FILES += lsc_mgr/LscTbl.cpp
LOCAL_SRC_FILES += lsc_mgr/LscBufImp.cpp
LOCAL_SRC_FILES += lsc_mgr/LscNvramImp.cpp
LOCAL_SRC_FILES += lsc_mgr/LscMgrDefault.cpp
LOCAL_SRC_FILES += lsc_mgr/LscMgrDefault.misc.cpp
LOCAL_SRC_FILES += lsc_mgr/OpenShading.cpp
LOCAL_SRC_FILES += lsc_mgr/TsfDft.cpp
LOCAL_SRC_FILES += lsc_mgr/TsfRto2.cpp
LOCAL_SRC_FILES += lsc_mgr/LscTsf.cpp
LOCAL_SRC_FILES += lsc_mgr/LscMgr.cpp
LOCAL_SRC_FILES += flash_mgr/flash_mgr.cpp
LOCAL_SRC_FILES += flash_mgr/flash_mgr_m.cpp
LOCAL_SRC_FILES += flash_mgr/flash_util.cpp
LOCAL_SRC_FILES += flash_mgr/flash_pline_tool.cpp
LOCAL_SRC_FILES += flash_mgr/flash_cct_quick2.cpp
LOCAL_SRC_FILES += flash_mgr/flash_cct.cpp
LOCAL_SRC_FILES += flash_mgr/flash_hal.cpp
LOCAL_SRC_FILES += flicker/flicker_hal.cpp
LOCAL_SRC_FILES += flicker/flicker_hal_base.cpp
LOCAL_SRC_FILES += flicker/flicker_util.cpp
LOCAL_SRC_FILES += Thread/ThreadSensorGainImp.cpp
LOCAL_SRC_FILES += Thread/ThreadSensorI2CImp.cpp
LOCAL_SRC_FILES += Thread/ThreadStatisticBufImp.cpp
LOCAL_SRC_FILES += Thread/ThreadSensorBufferModeImp.cpp
LOCAL_SRC_FILES += lens/gaflens_drv.cpp
LOCAL_SRC_FILES += lens/laser_drv.cpp
LOCAL_SRC_FILES += lens/lens_drv.cpp
LOCAL_SRC_FILES += lens/mcu_drv.cpp
LOCAL_SRC_FILES += lens/lens_sensor_drv.cpp
LOCAL_SRC_FILES += strobe/flashlight_drv.cpp
LOCAL_SRC_FILES += strobe/strobe_drv.cpp
LOCAL_SRC_FILES += strobe/strobe_global_driver.cpp
LOCAL_SRC_FILES += pd_mgr/pd_mgr.cpp
LOCAL_SRC_FILES += pd_mgr/pd_mgr_if.cpp

#Drv
MTKCAM_DRV_SENSOR_PATH := $(MTK_PATH_SOURCE)/hardware/mtkcam/drv/src/sensor/$(PLATFORM)/
MTKCAM_DRV_ISP_INCLUDE := $(MTK_PATH_SOURCE)/hardware/mtkcam/drv/src/isp/$(PLATFORM)/inc/
#ACDK
MTKCAM_ACDK := $(MTK_PATH_SOURCE)/hardware/mtkcam/drv/include/$(PLATFORM)/acdk/

#------------------ip base new include path-----------------------
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/common/include/
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/common/include/metadata
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/common/
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/utils/include
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/acdk/common/include
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/acdk/common/include/acdk
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/include/$(PLATFORM)
LOCAL_C_INCLUDES += $(MTKCAM_INCLUDE)
LOCAL_C_INCLUDES += $(MTKCAM_DRV_INCLUDE)
LOCAL_C_INCLUDES += $(MTKCAM_DRV_INCLUDE)/drv
LOCAL_C_INCLUDES += $(MTKCAM_DRV_INCLUDE)/mem
LOCAL_C_INCLUDES += $(MTKCAM_DRV_INCLUDE)/imageio
LOCAL_C_INCLUDES += $(MTKCAM_DRV_INCLUDE)/iopipe
LOCAL_C_INCLUDES += $(MTKCAM_ACDK)
LOCAL_C_INCLUDES += $(MTKCAM_ALGO_INCLUDE)
LOCAL_C_INCLUDES += $(MTKCAM_ALGO_INCLUDE)/libflicker
LOCAL_C_INCLUDES += $(MTKCAM_ALGO_INCLUDE)/lib3a
LOCAL_C_INCLUDES += $(MTKCAM_ALGO_INCLUDE)/libfdft
LOCAL_C_INCLUDES += $(MTKCAM_ALGO_INCLUDE)/libsync3a
LOCAL_C_INCLUDES += $(MTKCAM_HAL_INCLUDE)
LOCAL_C_INCLUDES += $(MTKCAM_DRV_ISP_INCLUDE)
LOCAL_C_INCLUDES += $(MTKCAM_3A_INCLUDE)/
LOCAL_C_INCLUDES += $(MTKCAM_3A_INCLUDE)/Hal3
LOCAL_C_INCLUDES += $(MTKCAM_HAL_INCLUDE)/v3/hal

LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/EventIrq
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/state_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/state_mgr_n3d
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/awb_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/af_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/pd_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/ae_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/flash_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/flicker
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/nvram_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/isp_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/buf_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/ispdrv_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/isp_tuning
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/isp_tuning/paramctrl/inc
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/isp_tuning/paramctrl/pca_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/isp_tuning/paramctrl/ccm_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/isp_tuning/paramctrl/ggm_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/isp_tuning/paramctrl/gma_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/lsc_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/sensor_mgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/ResultBufMgr
LOCAL_C_INCLUDES += $(MTKCAM_3A_PATH)/wrapper

#-----------------------------------------------------------
LOCAL_C_INCLUDES += $(MTKCAM_C_INCLUDES)
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/middleware/common/include
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/gralloc_extra/include
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/ext/include

LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/include
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/external/nvram/libnvram/
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/external/nvram/nvramagentclient
LOCAL_C_INCLUDES += $(TOP)/system/media/camera/include/
LOCAL_C_INCLUDES += $(TOP)/bionic
LOCAL_C_INCLUDES += $(TOP)/kernel-3.18/drivers/misc/mediatek/flashlight/inc/
LOCAL_C_INCLUDES += $(TOP)/external/stlport/stlport
LOCAL_C_INCLUDES += $(MTK_PATH_COMMON)/kernel/imgsensor/inc
LOCAL_C_INCLUDES += $(MTK_PATH_COMMON)/hal/inc/camera_feature
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/isp_tuning
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM)/hal/camera
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM_PLATFORM)/hal/inc
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/aaa
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/pd_buf_mgr
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/mtkcam
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/debug_exif/aaa
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/debug_exif/cam
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM_PLATFORM)/hal/camera_3a
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM_PLATFORM)/cgen/cfgfileinc
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_PLATFORM)/hardware/include/mtkcam/algorithm/libflicker
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_PLATFORM)/hardware/include/mtkcam/algorithm/lib3a
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_PLATFORM)/hardware/include/mtkcam/algorithm/libgma
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM)/hal/camera/inc
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM_PLATFORM)/hal/camera/inc
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM)/hal/camera_3a/inc
LOCAL_C_INCLUDES += $(MTK_PATH_CUSTOM_PLATFORM)/hal/camera_3a/inc

LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/include/mtkcam
LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/external/aee/binary/inc

LOCAL_C_INCLUDES += $(MTKCAM_HAL_INCLUDE)/metadata
#-----------------------------------------------------------
LOCAL_CFLAGS += $(MTKCAM_CFLAGS)
#

ifeq ($(HAVE_AEE_FEATURE),yes)
    LOCAL_CFLAGS += -DHAVE_AEE_FEATURE
endif
LOCAL_CFLAGS += -DUSE_AE_THD=1
LOCAL_CFLAGS += -DCAM3_3ATESTLVL=1

LOCAL_CFLAGS += -DCAM3_3AUT=0
LOCAL_CFLAGS += -DCAM3_3ASTTUT=1
LOCAL_CFLAGS += -DCAM3_3AIT=2
LOCAL_CFLAGS += -DCAM3_3AON=3

LOCAL_CFLAGS += -DCAM3_AF_FEATURE_EN=1
LOCAL_CFLAGS += -DCAM3_LSC_FEATURE_EN=1
LOCAL_CFLAGS += -DCAM3_FLASH_FEATURE_EN=1
LOCAL_CFLAGS += -DCAM3_FLICKER_FEATURE_EN=0
LOCAL_CFLAGS += -DCAM3_LSC2_USE_GAIN=0

ifeq ($(strip $(MTK_NATIVE_3D_SUPPORT)),yes)
    LOCAL_CFLAGS += -DMTK_NATIVE_3D_SUPPORT
endif

# for Stereo Feature
ifeq ($(MTK_CAM_STEREO_CAMERA_SUPPORT),yes)
    LOCAL_SRC_FILES += Sync3A.cpp
    LOCAL_SRC_FILES += state_mgr_n3d/aaa_state_n3d.cpp
    LOCAL_SRC_FILES += state_mgr_n3d/aaa_state_camera_preview_n3d.cpp
    LOCAL_SRC_FILES += state_mgr_n3d/aaa_state_af_n3d.cpp
    LOCAL_SRC_FILES += state_mgr_n3d/aaa_state_precapture_n3d.cpp
    LOCAL_SHARED_LIBRARIES += libn3d3a
    LOCAL_CFLAGS += -DCAM3_STEREO_FEATURE_EN=1
endif

#-----------------------------------------------------------
LOCAL_STATIC_LIBRARIES += libfeatureiodrv_mem
#-----------------------------------------------------------
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libcutils
#
#LOCAL_SHARED_LIBRARIES += libstlport
#
LOCAL_SHARED_LIBRARIES += libcam_utils
LOCAL_SHARED_LIBRARIES += libcam_hwutils
LOCAL_SHARED_LIBRARIES += libcam.halsensor
LOCAL_SHARED_LIBRARIES += libcamalgo
LOCAL_SHARED_LIBRARIES += libcamdrv_imem
LOCAL_SHARED_LIBRARIES += libcamdrv_isp
LOCAL_SHARED_LIBRARIES += libcam.metadata
LOCAL_SHARED_LIBRARIES += libcam.metadataprovider
LOCAL_SHARED_LIBRARIES += libcam.utils.sensorlistener
LOCAL_SHARED_LIBRARIES += libcam.iopipe
LOCAL_SHARED_LIBRARIES += libcam.hal3a.v3.nvram
LOCAL_SHARED_LIBRARIES += libcam.hal3a.v3.dng

LOCAL_SHARED_LIBRARIES += libdl
LOCAL_SHARED_LIBRARIES += libcamdrv_tuning_mgr

ifeq ($(HAVE_AEE_FEATURE),yes)
    LOCAL_SHARED_LIBRARIES += libaed
endif
#
ifneq ($(BUILD_MTK_LDVT),true)
    LOCAL_SHARED_LIBRARIES += lib3a
    LOCAL_SHARED_LIBRARIES += lib3a_sample
#    LOCAL_SHARED_LIBRARIES += libcam.exif
    LOCAL_SHARED_LIBRARIES += libcameracustom
endif
#-----------------------------------------------------------
#LOCAL_MODULE_TAGS := eng
LOCAL_MODULE := libcam.hal3a.v3

ifeq ($(BUILD_MTK_LDVT),yes)
    LOCAL_CFLAGS += -DUSING_MTK_LDVT
    LOCAL_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/ldvt/$(MTKCAM_DRV_VERSION)/include
    LOCAL_C_INCLUDES += $(TOP)/$(MTKCAM_INCLUDE)
    LOCAL_C_INCLUDES += $(TOP)/$(MTKCAM_DRV_INCLUDE)
    LOCAL_C_INCLUDES += $(TOP)/$(MTKCAM_DRV_INCLUDE)/iopipe
    LOCAL_C_INCLUDES += $(TOP)/$(MTKCAM_DRV_INCLUDE)/drv
    LOCAL_WHOLE_STATIC_LIBRARIES += libuvvf
endif

#-----------------------------------------------------------
include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_STATIC_LIBRARY)

################################################################################
#
################################################################################
#include $(CLEAR_VARS)
include $(call all-makefiles-under,$(LOCAL_PATH))

