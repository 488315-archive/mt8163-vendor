# Copyright Statement:
#
# This software/firmware and related documentation ("MediaTek Software") are
# protected under relevant copyright laws. The information contained herein
# is confidential and proprietary to MediaTek Inc. and/or its licensors.
# Without the prior written permission of MediaTek inc. and/or its licensors,
# any reproduction, modification, use or disclosure of MediaTek Software,
# and information contained herein, in whole or in part, shall be strictly prohibited.
#
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

###############################################################################
# build start
###############################################################################

LOCAL_PATH := $(call my-dir)

########################################
# Support Offload FLP
########################################

########################################
# Copy prebuilt FLP share library
########################################
include $(CLEAR_VARS)
LOCAL_MODULE := libmtkflp
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_SRC_FILES := lib/libmtkflp.so
LOCAL_MODULE_SUFFIX := .so
LOCAL_MULTILIB := 32
include $(BUILD_PREBUILT)

########################################
# Build MTK FLP daemon executable
########################################
include $(CLEAR_VARS)
MY_LOCAL_PATH := $(LOCAL_PATH)

LOCAL_SRC_FILES :=  \
     ./src/mtk_flp_android_main.c  \
     ./src/mtk_flp_sys.c  \
     ./src/slp_api.c  \
     ./src/mtk_flp_dc_cell.c  \
     ./src/mtk_flp_dc_offload.c
     
LOCAL_C_INCLUDES:= \
     $(LOCAL_PATH) \
     $(LOCAL_PATH)/inc/ \ 

LOCAL_MODULE := mtkFlpDaemon
LOCAL_MULTILIB := 32
LOCAL_MODULE_PATH := $(TARGET_OUT_OPTIONAL_EXECUTABLES)
LOCAL_PRELINK_MODULE := false
LOCAL_UNSTRIPPED_PATH := $(TARGET_ROOT_OUT_SBIN_UNSTRIPPED)
#LOCAL_CFLAGS += -DUSE_GCC -DSUPPORT_SHARED_LIBRARY
LOCAL_CFLAGS += -DUSE_GCC -DSUPPORT_SHARED_LIBRARY -DMTK_K2_PLATFORM
LOCAL_LDLIBS:= -llog
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES += libcutils libmtkflp liblog
  
include $(BUILD_EXECUTABLE)


########################################
# Build MTK FLP JNI shared library
########################################
include $(CLEAR_VARS)
#LOCAL_PATH := $(call my-dir)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_SHARED_LIBRARIES := liblog libcutils libhardware

#$(info flag: $(BOARD_GPS_LIBRARIES))

#LOCAL_CFLAGS:= -DFLP_HARDWARE_SUPPORT
LOCAL_CFLAGS:= -DFLP_HARDWARE_SUPPORT -DMTK_K2_PLATFORM
LOCAL_LDLIBS:= -llog
LOCAL_SRC_FILES := src/mtk_flp_hal_jni.c \
        src/mtk_flp_sys.c \
        src/geofence_offload.c
        
LOCAL_C_INCLUDES:= \
     $(LOCAL_PATH) \
     $(LOCAL_PATH)/inc/ \
     
LOCAL_MODULE := flp.default
LOCAL_MULTILIB := 64
LOCAL_MODULE_TAGS := optional 

include $(BUILD_SHARED_LIBRARY)
