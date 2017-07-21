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
  * rfx_data_to_parcel_utils.cpp
  *
  *  Created on: 2015/09/02
  *  Author: MTK10602
  *
  */
#include "RfxDataToParcelUtils.h"

static DataToParcelInfo s_data_to_parcel_request[] = {
    #include "data_to_parcel_ril_commands.h"
};
static DataToParcelInfo s_data_to_parcel_mtk_gsm_request[] = {
    #include "data_to_parcel_mtk_ril_commands.h"
};
#define LOG_TAG "RfxDataToParcelUtils"

Parcel* RfxDataToParcelUtils::dataToParcel(int request, int token, void* data, int datalen) {
    // Parcel *parcel;
    DataToParcelInfo *parcelInfo;
    if (request >= 1 && request <= (int32_t)NUM_ELEMS(s_data_to_parcel_request)) {
        // RFX_LOG_D(LOG_TAG, "[RFX] use s_data_to_parcel_request");
        parcelInfo = &(s_data_to_parcel_request[request]);
    } else if (request >= RIL_REQUEST_VENDOR_BASE && (request <= RIL_REQUEST_VENDOR_BASE +
                (int32_t)NUM_ELEMS(s_data_to_parcel_mtk_gsm_request))) {
        // RFX_LOG_D(LOG_TAG, "[RFX] use s_data_to_parcel_mtk_gsm_request");
        parcelInfo = &(s_data_to_parcel_mtk_gsm_request[request - RIL_REQUEST_VENDOR_BASE]);
    } else {
        RFX_LOG_D(LOG_TAG, "[RFX] Should not be here");
        return NULL;
    }
    return parcelInfo->dataToParcelFunction(request, token, data, datalen);
}

Parcel* callForwardToParcel(int request, int token, void* data,
        int datalen) {
    Parcel *parcel = new Parcel();
    char16_t *pString16;
    size_t len16;

    fillHeader(parcel, request, token);
    RIL_CallForwardInfo *cff = (RIL_CallForwardInfo *) data;
    parcel->writeInt32(cff->status);
    parcel->writeInt32(cff->reason);
    parcel->writeInt32(cff->serviceClass);
    parcel->writeInt32(cff->toa);

    pString16 = strdup8to16(cff->number, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);

    parcel->writeInt32(cff->timeSeconds);

    return parcel;
}

Parcel* cdmaBrSmsCnfToParcel(int request, int token, void* data,
        int datalen) {
    Parcel *parcel = new Parcel();

    fillHeader(parcel, request, token);
    int num = datalen/sizeof(RIL_CDMA_BroadcastSmsConfigInfo *);
    parcel->writeInt32(num);

    RIL_CDMA_BroadcastSmsConfigInfo **cdmaBciPtrs = (RIL_CDMA_BroadcastSmsConfigInfo **) data;
    for (int i = 0; i < num; i++) {
        parcel->writeInt32(cdmaBciPtrs[i]->service_category);
        parcel->writeInt32(cdmaBciPtrs[i]->language);
        parcel->writeInt32(cdmaBciPtrs[i]->selected);
    }

    return parcel;
}

Parcel* cdmaSmsToParcel(int request, int token, void* data,
        int datalen) {
    Parcel *parcel = new Parcel();

    fillHeader(parcel, request, token);
    RIL_CDMA_SMS_Message *rcsm = (RIL_CDMA_SMS_Message *) data;
    return cdmaSmsToParcelInternal(parcel, rcsm);
}

Parcel* cdmaSmsAckToParcel(int request, int token, void* data,
        int datalen) {
    Parcel *parcel = new Parcel();

    fillHeader(parcel, request, token);
    RIL_CDMA_SMS_Ack *rcsa = (RIL_CDMA_SMS_Ack *) data;
    parcel->writeInt32(rcsa->uErrorClass);
    parcel->writeInt32(rcsa->uSMSCauseCode);

    return parcel;
}

Parcel* cdmaSubscriptionSourceToParcel(int request, int token,
        void* data, int datalen) {
    Parcel *parcel = new Parcel();
    fillHeader(parcel, request, token);
    return parcel;
}

Parcel* dataCallToParcel(int request, int token, void* data, int datalen) {
    Parcel *parcel = new Parcel();
    char16_t *pString16;
    size_t len16;

    fillHeader(parcel, request, token);
    int countStrings = datalen/sizeof(char *);  // 8; // according to RIL.java
    char **pStrings = (char **) data;
    parcel->writeInt32(countStrings);
    for (int i = 0; i < countStrings; i++) {
        pString16 = strdup8to16(pStrings[i], &len16);
        parcel->writeString16(pString16, len16);
        free(pString16);
    }

    return parcel;
}

Parcel* dataProfileToParcel(int request, int token, void* data, int datalen) {
    // useless
    Parcel *parcel = new Parcel();
    fillHeader(parcel, request, token);
    return parcel;
}

Parcel* dialToParcel(int request, int token, void* data, int datalen) {
    Parcel *parcel = new Parcel();
    char16_t *pString16;
    size_t len16;

    fillHeader(parcel, request, token);
    RIL_Dial *dial = (RIL_Dial *) data;

    pString16 = strdup8to16(dial->address, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);

    parcel->writeInt32(dial->clir);
    if (dial->uusInfo != NULL) {
        parcel->writeInt32(1);
        parcel->writeInt32(dial->uusInfo->uusType);
        parcel->writeInt32(dial->uusInfo->uusDcs);
        if (dial->uusInfo->uusLength == 0) {
            parcel->writeInt32(-1);
        } else {
            parcel->writeInt32(dial->uusInfo->uusLength);
            parcel->writeByteArray(dial->uusInfo->uusLength, (uint8_t *) dial->uusInfo->uusData);
        }
    } else {
        parcel->writeInt32(0);
    }

    return parcel;
}

Parcel* fd_ModeToParcel(int request, int token, void* data, int datalen) {
    Parcel *parcel = new Parcel();

    fillHeader(parcel, request, token);
    RIL_FDModeStructure *args = (RIL_FDModeStructure *) data;
    parcel->writeInt32(args->args_num);
    if (args->args_num >= 1) {
        parcel->writeInt32(args->mode);
    }
    if (args->args_num >= 2) {
        parcel->writeInt32(args->parameter1);
    }
    if (args->args_num >= 3) {
        parcel->writeInt32(args->parameter2);
    }

    return parcel;
}

Parcel* gsmBrSmsCnfToParcel(int request, int token, void* data,
        int datalen) {
    Parcel *parcel = new Parcel();

    fillHeader(parcel, request, token);
    int num = datalen / sizeof(RIL_GSM_BroadcastSmsConfigInfo *);
    parcel->writeInt32(num);
    RIL_GSM_BroadcastSmsConfigInfo **gsmBciPtrs = (RIL_GSM_BroadcastSmsConfigInfo **) data;
    for (int i = 0; i < num; i++) {
        parcel->writeInt32(gsmBciPtrs[i]->fromServiceId);
        parcel->writeInt32(gsmBciPtrs[i]->toServiceId);
        parcel->writeInt32(gsmBciPtrs[i]->fromCodeScheme);
        parcel->writeInt32(gsmBciPtrs[i]->toCodeScheme);
        parcel->writeInt32(gsmBciPtrs[i]->selected);
    }

    return parcel;
}

Parcel* imsCdmaSmsToParcel(Parcel *parcel, RIL_CDMA_SMS_Message *rcsm) {
    return cdmaSmsToParcelInternal(parcel, rcsm);
}

Parcel* imsGsmSmsToParcel(Parcel *parcel, void **gsmMessage, int datalen) {
    int **args = (int **)gsmMessage;
    char16_t *pString16;
    size_t len16;

    int32_t countStrings =  (datalen - sizeof(RIL_RadioTechnologyFamily) - sizeof(uint8_t) -
            sizeof(int32_t))/sizeof(char *);  // 2; // according to RIL.java
    // args = args + 1;
    char **pStrings = (char **)args;
    if (pStrings == NULL) {
        parcel->writeInt32(-1);
    } else {
        parcel->writeInt32(countStrings);
        for (int i = 0; i < countStrings; i++) {
            pString16 = strdup8to16(pStrings[i], &len16);
            parcel->writeString16(pString16, len16);
            free(pString16);
        }
    }
    return parcel;
}

Parcel* imsSmsToParcel(int request, int token, void* data, int datalen) {
    Parcel *parcel = new Parcel();

    fillHeader(parcel, request, token);
    RIL_IMS_SMS_Message *rism = (RIL_IMS_SMS_Message *) data;

    parcel->writeInt32(rism->tech);
    parcel->write(&(rism->retry), sizeof(uint8_t));
    parcel->write(&(rism->messageRef), sizeof(int32_t));
    if (RADIO_TECH_3GPP == rism->tech) {
        return imsCdmaSmsToParcel(parcel, (RIL_CDMA_SMS_Message *) rism->message.cdmaMessage);
    } else if (RADIO_TECH_3GPP2 == rism->tech) {
        return imsGsmSmsToParcel(parcel, (void **)rism->message.gsmMessage, datalen);
    } else {
        RFX_LOG_E(LOG_TAG, "should not be here!");
        return parcel;
    }
}

Parcel* intsToParcel(int request, int token, void* data, int datalen) {
    Parcel *parcel = new Parcel();

    fillHeader(parcel, request, token);
    int *pInts = (int *) data;
    int countInts = datalen/sizeof(int);
    parcel->writeInt32(countInts);
    for (int i = 0; i < countInts; i++) {
        parcel->writeInt32(pInts[i]);
    }

    return parcel;
}

Parcel* modifyDedicateDataCallToParcel(int request, int token,
        void* data, int datalen) {
    Parcel *parcel = new Parcel();

    fillHeader(parcel, request, token);
    RIL_Dedicate_Data_Call_Struct *args = (RIL_Dedicate_Data_Call_Struct *) data;
    parcel->writeInt32(7);  // version, accoridng to RIL.java
    parcel->writeInt32(args->cid);
    parcel->writeInt32(args->hasQos);
    if (args->hasQos) {
        qosToParcelInternal(parcel, &(args->qos));
    } else {
        // RFX_LOG_D(LOG_TAG, "dispatchModifyDedicateDataCall no QOS specified");
    }
    parcel->writeInt32(args->hasTft);
    if (args->hasTft) {
        tftToParcelInternal(parcel, &(args->tft));
    }

    return parcel;
}

Parcel* nvReadItemToParcel(int request, int token, void* data,
        int datalen) {
    Parcel *parcel = new Parcel();

    fillHeader(parcel, request, token);
    RIL_NV_ReadItem *nvri = (RIL_NV_ReadItem *) data;
    parcel->writeInt32(nvri->itemID);

    return parcel;
}

Parcel* nvWriteItemToParcel(int request, int token, void* data,
        int datalen) {
    Parcel *parcel = new Parcel();
    char16_t *pString16;
    size_t len16;

    fillHeader(parcel, request, token);
    RIL_NV_WriteItem *nvwi = (RIL_NV_WriteItem *) data;
    parcel->writeInt32(nvwi->itemID);

    pString16 = strdup8to16(nvwi->value, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);

    return parcel;
}

Parcel* phbEntryToParcel(int request, int token, void* data,
        int datalen) {
    Parcel *parcel = new Parcel();
    char16_t *pString16;
    size_t len16;

    fillHeader(parcel, request, token);
    RIL_PhbEntryStrucutre *args = (RIL_PhbEntryStrucutre *) data;
    parcel->writeInt32(args->type);
    parcel->writeInt32(args->index);

    pString16 = strdup8to16(args->number, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);

    parcel->writeInt32(args->ton);

    pString16 = strdup8to16(args->alphaId, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);

    return parcel;
}

Parcel* radioCapabilityToParcel(int request, int token, void* data, int datalen) {
    Parcel *parcel = new Parcel();
    char16_t *pString16;
    size_t len16;

    fillHeader(parcel, request, token);
    RIL_RadioCapability *rc = (RIL_RadioCapability *) data;
    parcel->writeInt32(rc->version);
    parcel->writeInt32(rc->session);
    parcel->writeInt32(rc->phase);
    parcel->writeInt32(rc->rat);

    pString16 = strdup8to16(rc->logicalModemUuid, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);

    parcel->writeInt32(rc->status);

    return parcel;
}

Parcel* rawToParcel(int request, int token, void* data, int datalen) {
    Parcel *parcel = new Parcel();

    fillHeader(parcel, request, token);
    if (data == NULL) {
        parcel->writeInt32(-1);
    } else {
        parcel->writeInt32(datalen);
        parcel->write(data, datalen);
    }

    return parcel;
}

Parcel* rilCdmaSmsWriteArgsToParcel(int request, int token,
        void* data, int datalen) {
    Parcel *parcel = new Parcel();
    int digitCount;
    uint8_t uct;
    int digitLimit;

    fillHeader(parcel, request, token);
    RIL_CDMA_SMS_WriteArgs *rcsw = (RIL_CDMA_SMS_WriteArgs *) data;
    parcel->writeInt32(rcsw->status);
    parcel->writeInt32(rcsw->message.uTeleserviceID);
    parcel->write(&(rcsw->message.bIsServicePresent), sizeof(uct));
    parcel->writeInt32(rcsw->message.uServicecategory);
    parcel->writeInt32(rcsw->message.sAddress.digit_mode);
    parcel->writeInt32(rcsw->message.sAddress.number_mode);
    parcel->writeInt32(rcsw->message.sAddress.number_type);
    parcel->writeInt32(rcsw->message.sAddress.number_plan);
    parcel->write(&(rcsw->message.sAddress.number_of_digits), sizeof(uct));

    digitLimit= MIN(rcsw->message.sAddress.number_of_digits, RIL_CDMA_SMS_ADDRESS_MAX);
    for (digitCount = 0; digitCount < digitLimit; digitCount++) {
        parcel->write(&(rcsw->message.sAddress.digits[digitCount]), sizeof(uct));
    }
    parcel->writeInt32(rcsw->message.sSubAddress.subaddressType);
    parcel->write(&(rcsw->message.sSubAddress.odd), sizeof(uint8_t));
    parcel->write(&(rcsw->message.sSubAddress.number_of_digits), sizeof(uct));

    digitLimit= MIN(rcsw->message.sSubAddress.number_of_digits, RIL_CDMA_SMS_SUBADDRESS_MAX);
    for (digitCount = 0 ; digitCount < digitLimit; digitCount ++) {
        parcel->write(&(rcsw->message.sSubAddress.digits[digitCount]), sizeof(uct));
    }
    parcel->writeInt32(rcsw->message.uBearerDataLen);

    digitLimit= MIN(rcsw->message.uBearerDataLen, RIL_CDMA_SMS_BEARER_DATA_MAX);
    for (digitCount = 0 ; digitCount < digitLimit; digitCount ++) {
        parcel->write(&(rcsw->message.aBearerData[digitCount]), sizeof(uct));
    }

    return parcel;
}

Parcel* setInitialAttachApnToParcel(int request, int token, void* data,
        int datalen) {
    Parcel *parcel = new Parcel();
    char16_t *pString16;
    size_t len16;

    fillHeader(parcel, request, token);
    RIL_InitialAttachApn *pf = (RIL_InitialAttachApn *) data;
    pString16 = strdup8to16(pf->apn, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);

    pString16 = strdup8to16(pf->protocol, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);
    parcel->writeInt32(pf->authtype);

    pString16 = strdup8to16(pf->username, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);

    pString16 = strdup8to16(pf->password, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);

    pString16 = strdup8to16(pf->operatorNumeric, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);
    parcel->writeInt32(pf->canHandleIms);

    // int countStrings = (datalen - sizeof(pf) + sizeof(pf->dualApnPlmnList)) / sizeof(char *);
    int countStrings = sizeof(pf->dualApnPlmnList) / sizeof(char *);
    if (pf->dualApnPlmnList == NULL) {
        parcel->writeInt32(-1);
    } else {
        parcel->writeInt32(countStrings);
        for (int i = 0; i < countStrings; i++) {
            pString16 = strdup8to16(pf->dualApnPlmnList[i], &len16);
            parcel->writeString16(pString16, len16);
            free(pString16);
        }
    }

    return parcel;
}

Parcel* setupDedicateDataCallToParcel(int request, int token,
        void* data, int datalen) {
    Parcel *parcel = new Parcel();

    fillHeader(parcel, request, token);
    RIL_Dedicate_Data_Call_Struct *args = (RIL_Dedicate_Data_Call_Struct *) data;
    parcel->writeInt32(7);  // according to RIL.java
    parcel->writeInt32(args->ddcId);
    parcel->writeInt32(args->interfaceId);
    parcel->writeInt32(args->signalingFlag);
    parcel->writeInt32(args->hasQos);
    if (args->hasQos) {
        qosToParcelInternal(parcel, &(args->qos));
    }
    parcel->writeInt32(args->hasTft);
    if (args->hasTft) {
        tftToParcelInternal(parcel, &(args->tft));
    }

    return parcel;
}

Parcel* simAuthToParcel(int request, int token, void* data,
        int datalen) {
    Parcel *parcel = new Parcel();
    char16_t *pString16;
    size_t len16;

    fillHeader(parcel, request, token);
    RIL_SimAuthStructure *args = (RIL_SimAuthStructure *) data;
    parcel->writeInt32(args->sessionId);
    parcel->writeInt32(args->mode);

    pString16 = strdup8to16(args->param1, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);

    pString16 = strdup8to16(args->param2, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);
    if (args->mode == 1) {
        parcel->writeInt32(args->tag);
    }

    return parcel;
}

Parcel* simAuthenticationToParcel(int request, int token, void* data,
        int datalen) {
    Parcel *parcel = new Parcel();
    char16_t *pString16;
    size_t len16;

    fillHeader(parcel, request, token);
    RIL_SimAuthentication *pf = (RIL_SimAuthentication *) data;
    parcel->writeInt32(pf->authContext);

    pString16 = strdup8to16(pf->authData, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);

    pString16 = strdup8to16(pf->aid, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);

    return parcel;
}

Parcel* sim_APDUToParcel(int request, int token, void* data,
        int datalen) {
    Parcel *parcel = new Parcel();
    char16_t *pString16;
    size_t len16;

    fillHeader(parcel, request, token);
    RIL_SIM_APDU *apdu = (RIL_SIM_APDU *) data;
    parcel->writeInt32(apdu->sessionid);
    parcel->writeInt32(apdu->cla);
    parcel->writeInt32(apdu->instruction);
    parcel->writeInt32(apdu->p1);
    parcel->writeInt32(apdu->p2);
    parcel->writeInt32(apdu->p3);

    pString16 = strdup8to16(apdu->data, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);

    return parcel;
}

Parcel* sim_IOToParcel(int request, int token, void* data, int datalen) {
    Parcel *parcel = new Parcel();
    char16_t *pString16;
    size_t len16;

    fillHeader(parcel, request, token);
    RIL_SIM_IO_v6 *v6 = (RIL_SIM_IO_v6 *) data;
    parcel->writeInt32(v6->command);
    parcel->writeInt32(v6->fileid);

    pString16 = strdup8to16(v6->path, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);
    parcel->writeInt32(v6->p1);
    parcel->writeInt32(v6->p2);
    parcel->writeInt32(v6->p3);
    pString16 = strdup8to16(v6->data, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);
    pString16 = strdup8to16(v6->pin2, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);
    pString16 = strdup8to16(v6->aidPtr, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);

    return parcel;
}

Parcel* sim_IO_EXToParcel(int request, int token, void* data,
        int datalen) {
    Parcel *parcel = new Parcel();
    char16_t *pString16;
    size_t len16;

    fillHeader(parcel, request, token);
    RIL_SIM_IO_EX_v6 *simIO = (RIL_SIM_IO_EX_v6 *) data;
    parcel->writeInt32(simIO->command);
    parcel->writeInt32(simIO->fileid);

    pString16 = strdup8to16(simIO->path, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);
    parcel->writeInt32(simIO->p1);
    parcel->writeInt32(simIO->p2);
    parcel->writeInt32(simIO->p3);

    pString16 = strdup8to16(simIO->data, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);
    pString16 = strdup8to16(simIO->pin2, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);
    pString16 = strdup8to16(simIO->aidPtr, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);
    parcel->writeInt32(simIO->sessionId);

    return parcel;
}

Parcel* smsParamsToParcel(int request, int token, void* data,
        int datalen) {
    Parcel *parcel = new Parcel();

    fillHeader(parcel, request, token);
    RIL_SmsParams *smsParams = (RIL_SmsParams *) data;
    parcel->writeInt32(4);  // according to RIL.java
    parcel->writeInt32(smsParams->format);
    parcel->writeInt32(smsParams->vp);
    parcel->writeInt32(smsParams->pid);
    parcel->writeInt32(smsParams->dcs);

    return parcel;
}

Parcel* smsWriteToParcel(int request, int token, void* data,
        int datalen) {
    Parcel *parcel = new Parcel();
    char16_t *pString16;
    size_t len16;

    fillHeader(parcel, request, token);
    RIL_SMS_WriteArgs *args = (RIL_SMS_WriteArgs *) data;
    parcel->writeInt32(args->status);
    pString16 = strdup8to16(args->pdu, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);
    pString16 = strdup8to16(args->smsc, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);

    return parcel;
}

Parcel* stringToParcel(int request, int token, void* data, int datalen) {
    Parcel *parcel = new Parcel();
    char16_t *pString16;
    size_t len16;

    fillHeader(parcel, request, token);

    char *pString = (char *) data;
    pString16 = strdup8to16(pString, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);

    return parcel;
}

Parcel* stringsToParcel(int request, int token, void* data, int datalen) {
    Parcel *parcel = new Parcel();
    char16_t *pString16;
    size_t len16;

    fillHeader(parcel, request, token);
    int countStrings = datalen / sizeof(char *);
    parcel->writeInt32(countStrings);
    char **pString = (char **) data;
    for (int i = 0; i < countStrings; i++) {
        pString16 = strdup8to16(pString[i], &len16);
        parcel->writeString16(pString16, len16);
        free(pString16);
    }

    return parcel;
}

Parcel* uiccAuthenticationToParcel(int request, int token, void* data,
        int datalen) {
    Parcel *parcel = new Parcel();
    char16_t *pString16;
    size_t len16;

    fillHeader(parcel, request, token);
    RIL_UICC_Authentication *uiccAuth = (RIL_UICC_Authentication *) data;
    parcel->writeInt32(uiccAuth->session_id);
    pString16 = strdup8to16(uiccAuth->context1, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);
    pString16 = strdup8to16(uiccAuth->context2, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);

    return parcel;
}

Parcel* uiccIoToParcel(int request, int token, void* data, int datalen) {
    Parcel *parcel = new Parcel();
    char16_t *pString16;
    size_t len16;

    fillHeader(parcel, request, token);
    RIL_UICC_IO_v6 *uiccIo = (RIL_UICC_IO_v6 *) data;
    parcel->writeInt32(uiccIo->sessionId);
    parcel->writeInt32(uiccIo->command);
    parcel->writeInt32(uiccIo->fileId);
    pString16 = strdup8to16(uiccIo->path, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);

    parcel->writeInt32(uiccIo->p1);
    parcel->writeInt32(uiccIo->p2);
    parcel->writeInt32(uiccIo->p3);

    pString16 = strdup8to16(uiccIo->data, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);
    pString16 = strdup8to16(uiccIo->pin2, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);

    return parcel;
}

Parcel* uiccSubscriptonToParcel(int request, int token, void* data,
        int datalen) {
    Parcel *parcel = new Parcel();

    fillHeader(parcel, request, token);
    RIL_SelectUiccSub *uicc_sub = (RIL_SelectUiccSub *) data;
    parcel->writeInt32(uicc_sub->slot);
    parcel->writeInt32(uicc_sub->app_index);
    parcel->writeInt32(uicc_sub->sub_type);
    parcel->writeInt32(uicc_sub->act_status);

    return parcel;
}

Parcel* voiceRadioTechToParcel(int request, int token, void* data,
        int datalen) {
    Parcel *parcel = new Parcel();
    fillHeader(parcel, request, token);
    return parcel;
}

Parcel* voidToParcel(int request, int token, void* data, int datalen) {
    Parcel *parcel = new Parcel();
    fillHeader(parcel, request, token);
    return parcel;
}

Parcel* vsimEventToParcel(int request, int token, void* data,
        int datalen) {
    Parcel *parcel = new Parcel();

    fillHeader(parcel, request, token);
    RIL_VsimEvent *args = (RIL_VsimEvent *) data;
    parcel->writeInt32(args->transaction_id);
    parcel->writeInt32(args->eventId);
    parcel->writeInt32(args->sim_type);

    return parcel;
}

Parcel* vsimOperationEventToParcel(int request, int token, void* data,
        int datalen) {
    Parcel *parcel = new Parcel();
    char16_t *pString16;
    size_t len16;

    fillHeader(parcel, request, token);
    RIL_VsimOperationEvent *args = (RIL_VsimOperationEvent *) data;
    parcel->writeInt32(args->transaction_id);
    parcel->writeInt32(args->eventId);
    parcel->writeInt32(args->result);
    parcel->writeInt32(args->data_length);

    pString16 = strdup8to16(args->data, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);

    return parcel;
}

Parcel* writePhbEntryExtToParcel(int request, int token, void* data,
        int datalen) {
    Parcel *parcel = new Parcel();
    char16_t *pString16;
    size_t len16;

    fillHeader(parcel, request, token);
    RIL_PHB_ENTRY *args = (RIL_PHB_ENTRY *) data;
    parcel->writeInt32(args->index);

    pString16 = strdup8to16(args->number, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);
    parcel->writeInt32(args->type);

    pString16 = strdup8to16(args->text, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);
    parcel->writeInt32(args->hidden);

    pString16 = strdup8to16(args->group, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);

    pString16 = strdup8to16(args->adnumber, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);
    parcel->writeInt32(args->adtype);

    pString16 = strdup8to16(args->secondtext, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);
    pString16 = strdup8to16(args->email, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);

    return parcel;
}

Parcel* writePbToRuimToParcel(int request, int token, void* data,
        int datalen) {
    Parcel *parcel = new Parcel();
    char16_t *pString16;
    size_t len16;

    fillHeader(parcel, request, token);
    char **pStrings = (char **) data;
    parcel->writeInt32(*pStrings[0]);

    pString16 = strdup8to16(pStrings[1], &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);
    parcel->writeInt32(*pStrings[2]);

    pString16 = strdup8to16(pStrings[3], &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);
    parcel->writeInt32(*pStrings[4]);
    for (int i = 0; i < 5; i++) {
        pString16 = strdup8to16(pStrings[i], &len16);
        parcel->writeString16(pString16, len16);
        free(pString16);
    }

    return parcel;
}

Parcel* mtkWritePbToRuimToParcel(int request, int token, void* data,
        int datalen) {
    Parcel *parcel = new Parcel();
    char16_t *pString16;
    size_t len16;

    fillHeader(parcel, request, token);
    RIL_MTK_Ruim_Phonebook_Record *args = (RIL_MTK_Ruim_Phonebook_Record *) data;
    parcel->writeInt32(args->type);
    parcel->writeInt32(args->index);

    pString16 = strdup8to16(args->number, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);
    parcel->writeInt32(args->ton);

    pString16 = strdup8to16(args->alphaid, &len16);
    parcel->writeString16(pString16, len16);
    free(pString16);

    return parcel;
}

Parcel* playDTMFToneToParcel(int request, int token,
        void* data, int datalen) {
    Parcel *parcel = new Parcel();

    fillHeader(parcel, request, token);
    RIL_Play_DTMF_Tone *dtmfTone = (RIL_Play_DTMF_Tone *) data;
    parcel->writeInt32(dtmfTone->mode);
    if (dtmfTone->mode == 1) {
        parcel->writeInt32(dtmfTone->dtmfIndex);
        parcel->writeInt32(dtmfTone->volume);
        parcel->writeInt32(dtmfTone->duration);
    }

    return parcel;
}

Parcel* playToneSeqToParcel(int request, int token, void* data,
        int datalen) {
    Parcel *parcel = new Parcel();

    fillHeader(parcel, request, token);
    RIL_Play_Tone_Seq *toneSeq = (RIL_Play_Tone_Seq *) data;
    parcel->writeInt32(toneSeq->num);
    parcel->writeInt32(toneSeq->interation);
    parcel->writeInt32(toneSeq->volume);
    for (int i = 0; i < toneSeq->num; i++) {
        parcel->writeInt32(toneSeq->toneData[i].duration);
        parcel->writeInt32(toneSeq->toneData[i].freq[0]);
        parcel->writeInt32(toneSeq->toneData[i].freq[1]);
    }

    return parcel;
}

Parcel* cdmaSmsToParcelInternal(Parcel *parcel,
        RIL_CDMA_SMS_Message *rcsm) {
    int32_t digitCount;
    int digitLimit;
    parcel->writeInt32(rcsm->uTeleserviceID);
    parcel->write(&(rcsm->bIsServicePresent), sizeof(uint8_t));
    parcel->writeInt32(rcsm->uServicecategory);
    parcel->writeInt32((rcsm->sAddress).digit_mode);
    parcel->writeInt32((rcsm->sAddress).number_mode);
    parcel->writeInt32((rcsm->sAddress).number_type);
    parcel->writeInt32((rcsm->sAddress).number_plan);
    parcel->writeInt32((rcsm->sAddress).number_of_digits);
    digitLimit = MIN((rcsm->sAddress).number_of_digits, RIL_CDMA_SMS_ADDRESS_MAX);
    for (digitCount = 0; digitCount < digitLimit; digitCount++) {
        parcel->write(&((rcsm->sAddress).digits[digitCount]), sizeof(uint8_t));
    }
    parcel->writeInt32((rcsm->sSubAddress).subaddressType);
    parcel->write(&((rcsm->sSubAddress).odd), sizeof(uint8_t));
    parcel->write(&((rcsm->sSubAddress).number_of_digits), sizeof(uint8_t));
    digitLimit = MIN(((rcsm->sSubAddress).number_of_digits), RIL_CDMA_SMS_SUBADDRESS_MAX);
    for (digitCount = 0; digitCount < digitLimit; digitCount++) {
        parcel->write(&((rcsm->sSubAddress).digits[digitCount]), sizeof(uint8_t));
    }
    parcel->writeInt32(rcsm->uBearerDataLen);
    digitLimit = MIN((rcsm->uBearerDataLen), RIL_CDMA_SMS_BEARER_DATA_MAX);
    for (digitCount = 0; digitCount < digitLimit; digitCount++) {
        parcel->write(&(rcsm->aBearerData[digitCount]), sizeof(uint8_t));
    }
    return parcel;
}

Parcel* qosToParcelInternal(Parcel *parcel, Qos *qos) {
    parcel->writeInt32(qos->qci);
    parcel->writeInt32(qos->dlGbr);
    parcel->writeInt32(qos->ulGbr);
    parcel->writeInt32(qos->dlMbr);
    parcel->writeInt32(qos->ulMbr);
    return parcel;
}

Parcel* tftToParcelInternal(Parcel *parcel, Tft *tft) {
    int i, j, k;
    char16_t *pString16;
    size_t len16;

    parcel->writeInt32(tft->operation);
    parcel->writeInt32(tft->pfNumber);
    for (i = 0; i < tft->pfNumber; i++) {
        // packet filter information
        parcel->writeInt32(tft->pfList[i].id);
        parcel->writeInt32(tft->pfList[i].precedence);
        parcel->writeInt32(tft->pfList[i].direction);
        parcel->writeInt32(tft->pfList[i].networkPfIdentifier);
        parcel->writeInt32(tft->pfList[i].bitmap);

        pString16 = strdup8to16(tft->pfList[i].address, &len16);
        parcel->writeString16(pString16, len16);
        free(pString16);

        pString16 = strdup8to16(tft->pfList[i].mask, &len16);
        parcel->writeString16(pString16, len16);
        free(pString16);
        parcel->writeInt32(tft->pfList[i].protocolNextHeader);
        parcel->writeInt32(tft->pfList[i].localPortLow);
        parcel->writeInt32(tft->pfList[i].localPortHigh);
        parcel->writeInt32(tft->pfList[i].remotePortLow);
        parcel->writeInt32(tft->pfList[i].remotePortHigh);
        parcel->writeInt32(tft->pfList[i].spi);
        parcel->writeInt32(tft->pfList[i].tos);
        parcel->writeInt32(tft->pfList[i].tosMask);
        parcel->writeInt32(tft->pfList[i].flowLabel);
    }
    // TftParameter
    parcel->writeInt32(tft->tftParameter.linkedPfNumber);
    for (i = 0; i < tft->tftParameter.linkedPfNumber; i++) {
        parcel->writeInt32(tft->tftParameter.linkedPfList[i]);
    }

    parcel->writeInt32(tft->tftParameter.authtokenFlowIdNumber);
    for (i = 0; i < tft->tftParameter.authtokenFlowIdNumber; i++) {
        parcel->writeInt32(tft->tftParameter.authtokenFlowIdList[i].authTokenNumber);
        for (j = 0; j < tft->tftParameter.authtokenFlowIdList[i].authTokenNumber; j++)
            parcel->writeInt32(tft->tftParameter.authtokenFlowIdList[i].authTokenList[j]);

        parcel->writeInt32(tft->tftParameter.authtokenFlowIdList[i].flowIdNumber);
        for (j = 0; j < tft->tftParameter.authtokenFlowIdList[i].flowIdNumber; j++) {
            for (k = 0; k < 4; k++)
                parcel->writeInt32(tft->tftParameter.authtokenFlowIdList[i].flowIdList[j][k]);
        }
    }
    return parcel;
}

void fillHeader(Parcel *parcel, int request, int token) {
    // RFX_LOG_D(LOG_TAG, "fillHeader(): request = %d, token = %d", request, token);
    parcel->writeInt32(request);
    parcel->writeInt32(token);
}
