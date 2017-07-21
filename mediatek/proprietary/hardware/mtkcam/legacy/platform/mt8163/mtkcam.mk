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

sinclude $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/mtkcam.mk

# need function: all-c-cpp-files-under
sinclude $(TOP)/device/mediatek/build/build/libs/__gmel

# just print the log for once
PRINT_ONCE_GMEL ?= 1
ifeq ($(PRINT_ONCE_GMEL), 1)
$(warning "will include: __gmel")
PRINT_ONCE_GMEL := 2
endif

#-----------------------------------------------------------
# MTK Hal Standard Include Path
MTKCAM_C_INCLUDES += $(TOP)/$(MTKCAM_C_INCLUDES)/..

MTKCAM_C_INCLUDES += $(TOP)/$(MTK_MTKCAM_PLATFORM)/include
#MTKCAM_C_INCLUDES += $(TOP)/kernel-3.18/arch/arm64/include
#MTKCAM_C_INCLUDES += $(TOP)/kernel-3.18/include
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_MTKCAM_PLATFORM)/include/mtkcam/drv
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_MTKCAM_PLATFORM)/include/mtkcam/common/hw

MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/hal/inc
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/aaa
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/hal/camera_3a
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/camera_feature
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/isp_tuning
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/lomo
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/lomo_jni
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/mtkcam
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/debug_exif
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/debug_exif/aaa
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/debug_exif/cam
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_CUSTOM_PLATFORM)/hal/inc/debug_exif/eis

# temp fix
PLATFORM := $(shell echo $(MTK_PLATFORM) | tr A-Z a-z)
# add begin
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/legacy/include
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/legacy/include/mtkcam
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/legacy/include/mtkcam/hal
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/gralloc_extra/include
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/ext/include

MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/legacy/platform/$(PLATFORM)/v1/adapter/inc
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/legacy/platform/$(PLATFORM)/v1/adapter/inc/Scenario
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/legacy/platform/$(PLATFORM)/v1/adapter/inc/Scenario/Shot
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/legacy/platform/$(PLATFORM)/v1/adapter/Scenario/Shot/inc

#
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/legacy/platform/$(PLATFORM)/core/campipe/inc
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/legacy/platform/$(PLATFORM)/core/camshot/inc
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/legacy/platform/$(PLATFORM)/include/mtkcam/algorithm/libhdr
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/legacy/platform/$(PLATFORM)/include/mtkcam/algorithm/libstereocam
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/legacy/platform/$(PLATFORM)/include/mtkcam/algorithm/libutility
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/legacy/platform/$(PLATFORM)/include/mtkcam/drv
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/legacy/platform/$(PLATFORM)/include/mtkcam/featureio
MTKCAM_C_INCLUDES += $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/legacy/platform/$(PLATFORM)/include
#
# add end

MTKCAM_C_INCLUDES += $(MTK_MTKCAM_PLATFORM)/hal/inc/aaa

# enable or disable dngop 
MTK_TEMP_DNGOP_ENABLE := 1

# SDK Client
# set it here, for every platform can set it, then particial compile legacy/v1/client/SdkClient using mmm won't fail
ifeq ($(strip $(MTK_CAM_MMSDK_SUPPORT)),yes)
MTKSDK_HAVE_GESTURE_CLIENT := '1'
#MTKCAM_HAVE_SDK_CLIENT := '1'
else
# here, we force compile it, because currently, libmmsdk will use it on mt8163, if possible, should modify
MTKSDK_HAVE_GESTURE_CLIENT := '1'
#MTKCAM_HAVE_SDK_CLIENT := '0'
endif

PRINT_ONCE_GESTURE_CLIENT ?= 1
ifeq ($(PRINT_ONCE_GESTURE_CLIENT), 1)
$(info "MTKSDK_HAVE_GESTURE_CLIENT:$(MTKSDK_HAVE_GESTURE_CLIENT)")
PRINT_ONCE_GESTURE_CLIENT := 2
endif

PRINT_ONCE_MTKCAM_C_INCLUDES ?= 1
ifeq ($(PRINT_ONCE_MTKCAM_C_INCLUDES), 1)
$(info "MTKCAM_C_INCLUDES:" $(MTKCAM_C_INCLUDES))
PRINT_ONCE_MTKCAM_C_INCLUDES := 2
endif

#



PRINT_ONCE_MTK_PATH_SOURCE ?= 1
ifeq ($(PRINT_ONCE_MTK_PATH_SOURCE), 1)
$(info "MTK_PATH_SOURCE:" $(MTK_PATH_SOURCE))
PRINT_ONCE_MTK_PATH_SOURCE := 2
endif


PRINT_ONCE_MTK_PATH_CUSTOM_PLATFORM ?= 1
ifeq ($(PRINT_ONCE_MTK_PATH_CUSTOM_PLATFORM), 1)
$(info "MTK_PATH_CUSTOM_PLATFORM:" $(MTK_PATH_CUSTOM_PLATFORM))
PRINT_ONCE_MTK_PATH_CUSTOM_PLATFORM := 2
endif

PRINT_ONCE_MTK_PATH_CUSTOM_PLATFORM ?= 1
ifeq ($(PRINT_ONCE_MTK_PATH_CUSTOM_PLATFORM), 1)
$(info "MTK_PATH_CUSTOM_PLATFORM:" $(MTK_PATH_CUSTOM_PLATFORM))
PRINT_ONCE_MTK_PATH_CUSTOM_PLATFORM := 2
endif

PRINT_ONCE_MTKCAM_COMMON_PATH ?= 1
ifeq ($(PRINT_ONCE_MTKCAM_COMMON_PATH), 1)
$(info "MTKCAM_COMMON_PATH:" $(MTKCAM_COMMON_PATH))
PRINT_ONCE_MTKCAM_COMMON_PATH := 2
endif

PRINT_ONCE_MTK_PATH_CUSTOM ?= 1
ifeq ($(PRINT_ONCE_MTK_PATH_CUSTOM), 1)
$(info "MTK_PATH_CUSTOM:" $(MTK_PATH_CUSTOM))
PRINT_ONCE_MTK_PATH_CUSTOM := 2
endif

