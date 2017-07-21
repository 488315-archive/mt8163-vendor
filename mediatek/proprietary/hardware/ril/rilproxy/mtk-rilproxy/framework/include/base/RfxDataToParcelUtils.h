/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */
 /*
  * rfx_data_to_parcel_utils.h
  *
  *  Created on: 2015/09/02
  *  Author: MTK10602
  *
  */
#include "Parcel.h"
#include "ril.h"
#include "RfxLog.h"
#include <cutils/jstring.h>

using ::android::Parcel;

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define NUM_ELEMS(x) (sizeof(x)/sizeof(x[0]))

extern const char *getVersion();
typedef struct DataToParcelInfo {
    int request;
    Parcel* (*dataToParcelFunction) (int request, int token, void* data, int datalen);
} DataToParcelInfo;

static Parcel* callForwardToParcel(int request, int token, void* data, int datalen);
static Parcel* cdmaBrSmsCnfToParcel(int request, int token, void* data, int datalen);
static Parcel* cdmaSmsToParcel(int request, int token, void* data, int datalen);
static Parcel* cdmaSmsAckToParcel(int request, int token, void* data, int datalen);
static Parcel* cdmaSubscriptionSourceToParcel(int request, int token, void* data,
        int datalen);
static Parcel* dataCallToParcel(int request, int token, void* data, int datalen);
static Parcel* dataProfileToParcel(int request, int token, void* data, int datalen);
static Parcel* dialToParcel(int request, int token, void* data, int datalen);
static Parcel* fd_ModeToParcel(int request, int token, void* data, int datalen);
static Parcel* gsmBrSmsCnfToParcel(int request, int token, void* data, int datalen);
static Parcel* imsCdmaSmsToParcel(Parcel *parcel, RIL_CDMA_SMS_Message *rcsm);
static Parcel* imsGsmSmsToParcel(Parcel *parcel, void **gsmMessage, int datalen);
static Parcel* imsSmsToParcel(int request, int token, void* data, int datalen);
static Parcel* intsToParcel(int request, int token, void* data, int datalen);
static Parcel* modifyDedicateDataCallToParcel(int request, int token, void* data, int datalen);
static Parcel* nvReadItemToParcel(int request, int token, void* data, int datalen);
static Parcel* nvWriteItemToParcel(int request, int token, void* data, int datalen);
static Parcel* phbEntryToParcel(int request, int token, void* data, int datalen);
static Parcel* radioCapabilityToParcel(int request, int token, void* data, int datalen);
static Parcel* rawToParcel(int request, int token, void* data, int datalen);
static Parcel* rilCdmaSmsWriteArgsToParcel(int request, int token, void* data, int datalen);
static Parcel* setInitialAttachApnToParcel(int request, int token, void* data, int datalen);
static Parcel* setupDedicateDataCallToParcel(int request, int token, void* data, int datalen);
static Parcel* simAuthToParcel(int request, int token, void* data, int datalen);
static Parcel* simAuthenticationToParcel(int request, int token, void* data, int datalen);
static Parcel* sim_APDUToParcel(int request, int token, void* data, int datalen);
static Parcel* sim_IOToParcel(int request, int token, void* data, int datalen);
static Parcel* sim_IO_EXToParcel(int request, int token, void* data, int datalen);
static Parcel* smsParamsToParcel(int request, int token, void* data, int datalen);
static Parcel* smsWriteToParcel(int request, int token, void* data, int datalen);
static Parcel* stringToParcel(int request, int token, void* data, int datalen);
static Parcel* stringsToParcel(int request, int token, void* data, int datalen);
static Parcel* uiccAuthenticationToParcel(int request, int token, void* data, int datalen);
static Parcel* uiccIoToParcel(int request, int token, void* data, int datalen);
static Parcel* uiccSubscriptonToParcel(int request, int token, void* data, int datalen);
static Parcel* voiceRadioTechToParcel(int request, int token, void* data, int datalen);
static Parcel* voidToParcel(int request, int token, void* data, int datalen);
static Parcel* vsimEventToParcel(int request, int token, void* data, int datalen);
static Parcel* vsimOperationEventToParcel(int request, int token, void* data, int datalen);
static Parcel* writePhbEntryExtToParcel(int request, int token, void* data, int datalen);
static Parcel* writePbToRuimToParcel(int request, int token, void* data, int datalen);
static Parcel* mtkWritePbToRuimToParcel(int request, int token, void* data, int datalen);
static Parcel* playDTMFToneToParcel(int request, int token, void* data, int datalen);
static Parcel* playToneSeqToParcel(int request, int token, void* data, int datalen);
static Parcel* cdmaSmsToParcelInternal(Parcel *parcel, RIL_CDMA_SMS_Message *rcsm);
static Parcel* qosToParcelInternal(Parcel *parcel, Qos *qos);
static Parcel* tftToParcelInternal(Parcel *parcel, Tft *tft);
static void fillHeader(Parcel *parcel, int request, int token);


class RfxDataToParcelUtils {
private:
    RfxDataToParcelUtils();

    virtual ~RfxDataToParcelUtils();

public:
    static Parcel* dataToParcel(int request, int token, void* data, int datalen);
};
