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

PLATFORM := $(shell echo $(MTK_PLATFORM) | tr A-Z a-z)

ifeq ($(PLATFORM), $(filter $(PLATFORM),mt6797))
FDFT_DIR := FD4.0
else
FDFT_DIR := FD3.6
endif

ifeq ($(PLATFORM), $(filter $(PLATFORM),mt6797))
FDFT_USE_HW := '1'
else
FDFT_USE_HW := '0'
endif

ifeq ($(PLATFORM), $(filter $(PLATFORM),mt6797))
GS_DIR := GS1.0
else
GS_DIR := GS1.0
endif

ifeq ($(PLATFORM), $(filter $(PLATFORM),mt6797))
ASD_DIR := ASD1.0
else
ASD_DIR := ASD1.0
endif

ifeq ($(PLATFORM), $(filter $(PLATFORM),mt6797))
HRD_DIR := HRD1.0
else
HRD_DIR := HRD1.0
endif

#Face detection
FEATURE_FD_INCLUDE := $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/feature/include/common/FaceDetection/$(FDFT_DIR)/
#Gesture detection
FEATURE_GS_INCLUDE := $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/feature/include/common/Gesture/$(GS_DIR)/
#Auto scene detection
FEATURE_ASD_INCLUDE := $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/feature/include/common/ASD/$(ASD_DIR)/
#Heartrate Detection
FEATURE_HRD_INCLUDE := $(TOP)/$(MTK_PATH_SOURCE)/hardware/mtkcam/feature/include/common/HeartRate/$(HRD_DIR)/
