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

/*****************************************************************************
 * Include
 *****************************************************************************/
#include "RpAtciController.h"
#include "RfxRootController.h"
#include "RfxStatusDefs.h"
#include "socket_channel.h"
#include <cutils/properties.h>
#include <cutils/jstring.h>
#include <cutils/sockets.h>
#include <errno.h>
#include <string.h>
#include "ril.h"

#define RFX_LOG_TAG "RpAtciController"

extern void *sendDataToAtci(void *arg);

/*****************************************************************************
 * Class RfxController
 *****************************************************************************/

RFX_IMPLEMENT_CLASS("RpAtciController", RpAtciController, RfxController);

RpAtciController::RpAtciController() {
    logD(RFX_LOG_TAG,"RpAtciController E!");
}

RpAtciController::~RpAtciController() {
    logD(RFX_LOG_TAG,"RpAtciController X!");
    pthread_cond_signal(&writeCond);
    pthread_mutex_unlock(&writeMutex);
    pthread_mutex_destroy(&writeMutex);
    pthread_cond_destroy(&writeCond);
    pthread_cond_signal(&bufferCond);
    pthread_mutex_unlock(&bufferMutex);
    pthread_mutex_destroy(&bufferMutex);
    pthread_cond_destroy(&bufferCond);
}

void RpAtciController::onInit() {
    RfxController::onInit(); // Required: invoke super class implementation

    const int request_id_list[] = {
        RIL_LOCAL_REQUEST_OEM_HOOK_ATCI_INTERNAL,
        RIL_REQUEST_OEM_HOOK_RAW_WITH_PROXY
    };
    const int urc_id_list[] = {
        RIL_UNSOL_ATCI_RESPONSE
    };
    logD(RFX_LOG_TAG, "onInit enter.");

    pthread_attr_t attr;
    PthreadPtr pptr = sendDataToAtci;
    int result;
    pthread_t socketThreadId;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    writeMutex = PTHREAD_MUTEX_INITIALIZER;
    writeCond = PTHREAD_COND_INITIALIZER;
    bufferMutex = PTHREAD_MUTEX_INITIALIZER;
    bufferCond = PTHREAD_COND_INITIALIZER;
    bufferReady = 0;
    atciResponseData = new AtciResponseData(-1, NULL, 0, &bufferReady,
            &writeMutex, &writeCond, &bufferMutex, &bufferCond);
    result = pthread_create(&socketThreadId, &attr,
            pptr, atciResponseData);
    if(result < 0) {
        RLOGE("pthread_create failed with result:%d",result);
    }
    pthread_attr_destroy(&attr);
    // register request & URC id list
    // NOTE. one id can only be registered by one controller
    registerToHandleRequest(request_id_list,
            sizeof(request_id_list)/sizeof(const int), NORMAL);
    registerToHandleUrc(urc_id_list, sizeof(urc_id_list)/sizeof(const int));
}

bool RpAtciController::onHandleAtciRequest(const sp<RfxMessage>& message) {
    int msg_id = message->getId();
    char *responseErrorStr = "ERROR\n";
    int group = RADIO_TECH_GROUP_GSM;
    int cardType = 0;
    int activeCdmaLteSlot = 0;
    RfxStatusManager* statusManager;

    logD(RFX_LOG_TAG, "onHandleAtciRequest msg_id: %d, slot id: %d.",
            msg_id, m_slot_id);

    switch (msg_id) {
    case RIL_LOCAL_REQUEST_OEM_HOOK_ATCI_INTERNAL: {
            sp<RfxMessage> request;
            group = checkTechGroupByEsuo();
            if (-1 == group) {
                break;
            }
            request = RfxMessage::obtainRequest((RILD_RadioTechnology_Group)group, msg_id, message, true);
            Parcel* p = request->getParcel();
            // Debug
            p->setDataPosition(0);

            int req, token, len;
            char *data;
            char *atBuffer;

            p->readInt32(&req);
            p->readInt32(&token);
            p->readInt32(&len);
            logD(RFX_LOG_TAG, "req %d, len %d", req, len);
            if (len > 0) {
                data = (char*)p->readInplace(len);
                p->setDataPosition(0);
                p->writeInt32(RIL_REQUEST_OEM_HOOK_RAW);
                logD(RFX_LOG_TAG, "atBuffer %s", data);
                if ((strcmp(data, "AT+CFUN=0") == 0) || (strcmp(data, "at+cfun=0") == 0) ||
                       (strcmp(data, "AT+CFUN=1") == 0) || (strcmp(data, "at+cfun=1") == 0) ||
                       (strcmp(data, "AT+CFUN=4") == 0) || (strcmp(data, "at+cfun=4") == 0)) {
                    logD(RFX_LOG_TAG, "[RpAtciController] RIL_REQUEST_OEM_HOOK_RAW_REDIRECT");
                    p->setDataPosition(0);
                    p->writeInt32(RIL_REQUEST_OEM_HOOK_RAW_REDIRECT);
                }
            } else {
                RLOGE("len <= 0 return");
                break;
            }
            // Shift the position to 0 and pass to RILD.
            p->setDataPosition(0);
            //WARNING: NE here. Do not use this obtainRequest API, it is used by FWK.
            //request = RfxMessage::obtainRequest(m_slot_id,
            //        group, msg_id, message->getPToken(), message->getParcel());
            // send request to corresponding RILD
            requestToRild(request);
        }
        break;
    default:
        break;
    }
    return true;
}

bool RpAtciController::onHandleAtciResponse(const sp<RfxMessage>& message) {
    int msg_id = message->getId();
    int32_t type;
    int32_t token;
    int32_t error;
    int32_t respLen;
    char *responseStr = NULL;
    bool returnError = false;

    logD(RFX_LOG_TAG, "onHandleAtciResponse msg_id: %d.", msg_id);
    switch (msg_id) {
    case RIL_LOCAL_REQUEST_OEM_HOOK_ATCI_INTERNAL: {
        Parcel* p = message->getParcel();

        p->setDataPosition(0);
        p->readInt32(&type);
        p->readInt32(&token);
        p->readInt32(&error);

        logD(RFX_LOG_TAG, "type: %d, token %d, error %d"
                , type, token, error);
        if (error == -1) {
            returnError = true;
        } else {
            p->readInt32(&respLen);
            logD(RFX_LOG_TAG, "respLen: %d", respLen);
            if (respLen > 0) {
                responseStr = (char*)p->readInplace(respLen);
                if (responseStr == NULL) {
                    logD(RFX_LOG_TAG, "responseStr == NULL");
                    returnError = true;
                }
            } else {
                returnError = true;
            }
        }
        if (returnError) {
            responseStr = "ERROR\n";
            respLen = strlen(responseStr);
        }
        responseToAtci(responseStr, respLen);
    }
        break;
    default:
        break;
    }
    return true;
}
bool RpAtciController::onHandleRequest(const sp<RfxMessage>& message) {
    int msg_id = message->getId();
    int group = RADIO_TECH_GROUP_GSM;

    logD(RFX_LOG_TAG, "onHandleRequest msg_id: %d.", msg_id);
    switch (msg_id) {
    case RIL_REQUEST_OEM_HOOK_RAW_WITH_PROXY: {
        sp<RfxMessage> request;
        group = checkTechGroupByEsuo();
        if (-1 == group) {
            break;
        }
        request = RfxMessage::obtainRequest((RILD_RadioTechnology_Group)group, msg_id, message, true);
        requestToRild(request);
    }
        break;
    default:
        break;
    }
    return true;
}

bool RpAtciController::onHandleResponse(const sp<RfxMessage>& message) {
    int msg_id = message->getId();

    logD(RFX_LOG_TAG, "onHandleResponse msg_id: %d.", msg_id);
    switch (msg_id) {
    case RIL_REQUEST_OEM_HOOK_RAW_WITH_PROXY: {
        responseToRilj(message);
    }
        break;
    default:
        break;
    }
    return true;
}
bool RpAtciController::onHandleUrc(const sp<RfxMessage>& message) {
    int msg_id = message->getId();
    int32_t urcLen = 0;
    char *urcStr = NULL;
    logD(RFX_LOG_TAG, "onHandleUrc msg_id %d", msg_id);

    if (msg_id == RIL_UNSOL_ATCI_RESPONSE) {
        Parcel* p = message->getParcel();
        urcStr = strdupReadString(p);
        if (urcStr == NULL) {
            logD(RFX_LOG_TAG, "urcStr == NULL");
            return true;
        }
        urcLen = strlen(urcStr);
    }
    logD(RFX_LOG_TAG, "%d > %s", urcLen, urcStr);
    responseToAtci(urcStr, urcLen);
    pthread_mutex_lock(&bufferMutex);
    while(1 == bufferReady) {
        pthread_cond_wait(&bufferCond, &bufferMutex);
    }
    if (NULL != urcStr) {
        free(urcStr);
    }
    pthread_mutex_unlock(&bufferMutex);
    logD(RFX_LOG_TAG, "End onHandleUrc");
    return true;
}

char* RpAtciController::strdupReadString(Parcel* p) {
    size_t stringlen;
    const char16_t *s16;

    s16 = p->readString16Inplace(&stringlen);

    return strndup16to8(s16, stringlen);
}

void RpAtciController::responseToAtci(char *responseStr, int strLen) {
    RilpAtciSocket* rilpAtciSocket = RilpAtciSocket::getInstance();
    int rilpAtciSocketFd = rilpAtciSocket->getAtciRilSocket();
    logD(RFX_LOG_TAG, "Start responseToAtci: %s.", responseStr);

    pthread_mutex_lock(&writeMutex);
    bufferReady = 1;
    atciResponseData->m_atci_socket_fd = rilpAtciSocketFd;
    atciResponseData->setData(responseStr);
    atciResponseData->setDataLength(strLen);
    pthread_cond_signal(&writeCond);
    pthread_mutex_unlock(&writeMutex);

    logD(RFX_LOG_TAG, "End responseToAtci");
}

bool RpAtciController::isGsmCard(int cardType) {
    if ((cardType & RFX_CARD_TYPE_CSIM) == 0 &&
        (cardType & RFX_CARD_TYPE_RUIM) == 0 &&
        ((cardType & RFX_CARD_TYPE_SIM) > 0 ||
        (cardType & RFX_CARD_TYPE_USIM) > 0)) {
        return true;
    }
    return false;
}

bool RpAtciController::isCdmaCard(int cardType) {
    if ((cardType & RFX_CARD_TYPE_SIM) == 0 &&
        (cardType & RFX_CARD_TYPE_USIM) == 0 &&
        ((cardType & RFX_CARD_TYPE_CSIM) > 0 ||
        (cardType & RFX_CARD_TYPE_RUIM) > 0)) {
        return true;
    }
    return false;
}

bool RpAtciController::isGsmCdmaCard(int cardType) {
    if (containsGsm(cardType) && containsCdma(cardType)) {
        return true;
    }
    return false;
}

bool RpAtciController::containsCdma(int cardType) {
     if ((cardType & RFX_CARD_TYPE_RUIM) > 0 ||
         (cardType & RFX_CARD_TYPE_CSIM) > 0) {
         return true;
     }
     return false;
 }

bool RpAtciController::containsGsm(int cardType) {
     if ((cardType & RFX_CARD_TYPE_SIM) > 0 ||
         (cardType & RFX_CARD_TYPE_USIM) > 0) {
         return true;
     }
     return false;
 }

int RpAtciController::checkTechGroupByEsuo() {
    char simNo[PROPERTY_VALUE_MAX] = {0};

    property_get("persist.service.atci.sim", simNo, "0");
    logD(RFX_LOG_TAG, "simNo: %d.", simNo[0]);

    if (simNo[0] == '9') {
        return (int)RADIO_TECH_GROUP_C2K;
    } else if (simNo[0] == '0' ||
        simNo[0] == '1') {
        return (int)RADIO_TECH_GROUP_GSM;
    } else {
        logD(RFX_LOG_TAG, "Not support slot id: %d.", simNo[0]);
        return -1;
    }
}
int blockingWrite(int fd, const void *buffer, size_t len) {
    size_t writeOffset = 0;
    const uint8_t *toWrite;

    toWrite = (const uint8_t *)buffer;

    while (writeOffset < len) {
        ssize_t written;
        do {
            written = write (fd, toWrite + writeOffset,
                    len - writeOffset);
        } while (written < 0 && ((errno == EINTR) || (errno == EAGAIN)));
        if (written >= 0) {
            writeOffset += written;
        } else {// written < 0
            RLOGI("RpAtciController:unexpected error on write:%d", errno);
            //close(fd);
            return -1;
        }
    }
    return 0;
}

extern "C"
void *sendDataToAtci(void *arg) {
    int retLen = 0;
    AtciResponseData *atciResponseData = (AtciResponseData *)arg;
    char *data = NULL;

    RLOGI("RpAtciController:sendDataToAtci:respLen %d.",
            atciResponseData->m_response_data_length);
    while (1) {
        pthread_mutex_lock(atciResponseData->m_write_mutex);
        RLOGI("RpAtciController:sendDataToAtci:ready %d.", *(atciResponseData->m_buffer_ready));
        while (0 == *(atciResponseData->m_buffer_ready)) {
            pthread_cond_wait(atciResponseData->m_write_cond, atciResponseData->m_write_mutex);
        }
        if (NULL == atciResponseData->m_response_data) {
            *(atciResponseData->m_buffer_ready) = 0;
            pthread_cond_signal(atciResponseData->m_buffer_cond);
            continue;
        }
        data = strdup(atciResponseData->m_response_data);
        //Signal to free buffer for URC.
        *(atciResponseData->m_buffer_ready) = 0;
        pthread_cond_signal(atciResponseData->m_buffer_cond);
        retLen = blockingWrite(atciResponseData->m_atci_socket_fd,
                data,
                atciResponseData->m_response_data_length);
        if (retLen < 0) {
            RLOGI("RpAtciController:Error: retLen < 0.");
        }
        if (NULL != data) {
            free(data);
        }
        pthread_mutex_unlock(atciResponseData->m_write_mutex);
    }
    pthread_detach(pthread_self());
    RLOGI("RpAtciController:sendDataToAtci:End.");
    return NULL;
}


