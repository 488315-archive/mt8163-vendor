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

#ifneq ($(strip $(MTK_EMULATOR_SUPPORT)),yes)
LOCAL_PATH := $(call my-dir)

################################################################################
#
################################################################################
include $(CLEAR_VARS)
sinclude $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/mtkcam.mk
-include $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/feature/effectHal.mk

# == Only build if not legacy ==
ifeq ($(IS_LEGACY), 0)

#-----------------------------------------------------------
LOCAL_SRC_FILES += $(call all-c-cpp-files-under, .)

# ---------- Include Path -----------------
LOCAL_C_INCLUDES += $(MTKCAM_C_INCLUDES)
LOCAL_C_INCLUDES += $(TOP)/system/media/camera/include
#for IEffectHal related mmsdk header
LOCAL_C_INCLUDES += $(EFFECTHAL_C_INCLUDE)
#for IMetaData
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam
#for All implemented Effect Hal
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/feature/common/
#for Facebeauty effect hal, which needs another header
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/mtkcam/feature/common/fb/cfb_effecthal_pp/include
#for gralloc_extra include
LOCAL_C_INCLUDES += $(MTK_PATH_SOURCE)/hardware/gralloc_extra/include




#-----------------------------------------------------------
#LOCAL_CFLAGS += $(MTKCAM_CFLAGS)
#$(warning "TEST!! MTKCAM_CFLAG = $(MTKCAM_CFLAGS)")

#-----------------------------------------------------------
LOCAL_WHOLE_STATIC_LIBRARIES +=
#
LOCAL_STATIC_LIBRARIES +=
#
LOCAL_SHARED_LIBRARIES += liblog
LOCAL_SHARED_LIBRARIES += libutils
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_SHARED_LIBRARIES += libbinder
LOCAL_SHARED_LIBRARIES += libgui
LOCAL_SHARED_LIBRARIES += libcam.metadata
LOCAL_SHARED_LIBRARIES += libcam.metadataprovider
# remove after faceBeautifyEffectHal change path
#LOCAL_SHARED_LIBRARIES += libcam.camadapter
#LOCAL_SHARED_LIBRARIES += libeffect.facebeautypp
LOCAL_SHARED_LIBRARIES += libeffecthal.jpg
LOCAL_SHARED_LIBRARIES += libeffecthal.cfb
LOCAL_SHARED_LIBRARIES += libeffecthal.base
# for factory

#-----------------------------------------------------------
LOCAL_MODULE := libfeatureio.featurefactory
#-----------------------------------------------------------
include $(BUILD_SHARED_LIBRARY)

endif
# if LEGACY == 0 endif
