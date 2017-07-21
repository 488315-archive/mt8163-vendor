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
 * rfx_ril_adapter.cpp
 *
 *  Created on: 2015/7/22
 *      Author: MTK08471
 *
 *  RIL Adapter prototype
 */

#include <sys/types.h>
#include <cutils/sockets.h>
#include <cutils/properties.h>
#include <utils/Log.h>
#include <stdlib.h>
#include "RfxRilAdapter.h"
#include <string.h>
#include "RfxRootController.h"
#include "telephony/mal/RpRilClientController.h"
#include "modecontroller/RpCdmaLteModeController.h"

/*************************************************************
 * RfxRilAdapter Class Implementation
 *************************************************************/
RFX_IMPLEMENT_CLASS("RfxRilAdapter", RfxRilAdapter, RfxObject);
RFX_OBJ_IMPLEMENT_SINGLETON_CLASS(RfxRilAdapter);

int RfxRilAdapter::sim_count = 0;
#define LOG_TAG "RfxRilAdapter"

RfxRilAdapter::RfxRilAdapter() {
    // TODO Auto-generated constructor stub

#if (SIM_COUNT>=2)
    sim_count = 2;
#else
    sim_count = 1;
#endif

    for (int i=0; i<RADIO_TECH_GROUP_NUM; i++) {
        socket_1_fds[i] = -1;
        socket_2_fds[i] = -1;
        pthread_mutex_init(&socket_1_mutex[i], NULL);
        pthread_mutex_init(&socket_2_mutex[i], NULL);
    }

    for (int i = 0; i < SIM_COUNT; i++) {
        socket_sap_gsm_fds[i] = -1;
        socket_sap_c2k_fds[i] = -1;
        socket_sap_bt_fds[i] = -1;
        pthread_mutex_init(&sap_socket_gsm_mutex[i], NULL);
        pthread_mutex_init(&sap_socket_c2k_mutex[i], NULL);
        pthread_mutex_init(&sap_socket_bt_mutex[i], NULL);
    }

    for (int i = 0; i < SIM_COUNT; i++) {
        pthread_mutex_init(&request_queue_gsm_mutex[i], NULL);
    }
}

RfxRilAdapter::~RfxRilAdapter() {
    unregisterSocketStateListener();
}

bool RfxRilAdapter::setSocket(int slotId, RILD_RadioTechnology_Group group, int fd) {

    if ( (slotId >= sim_count) ||
            (group >= RADIO_TECH_GROUP_NUM) ) {
        RFX_LOG_D(LOG_TAG, "setSocket param error (slotId=%d, group=%d, fd=%d)",
                slotId, group, fd);
        return false;
    }

    switch (slotId) {
    case 0:
        pthread_mutex_lock(&socket_1_mutex[group]);
        socket_1_fds[group] = fd;
        pthread_mutex_unlock(&socket_1_mutex[group]);
        RFX_LOG_D(LOG_TAG, "setSocket (slotId=%d, group=%d, fd=%d)", slotId, group, fd);
        break;
    case 1:
        pthread_mutex_lock(&socket_2_mutex[group]);
        socket_2_fds[group] = fd;
        pthread_mutex_unlock(&socket_2_mutex[group]);
        RFX_LOG_D(LOG_TAG, "setSocket (slotId=%d, group=%d, fd=%d)", slotId, group, fd);
        break;
    }
    return true;
}

bool RfxRilAdapter::switchC2kSocket(int targetSlotId) {
    bool ret = false;
    if (targetSlotId >= sim_count) {
        RFX_LOG_D(LOG_TAG, "switchC2kSocket param error (targetSlotId=%d)", targetSlotId);
        return false;
    }

    RFX_LOG_D(LOG_TAG, "switchC2kSocket - start all fds mutex");
    pthread_mutex_lock(&socket_1_mutex[RADIO_TECH_GROUP_C2K]);
    pthread_mutex_lock(&socket_2_mutex[RADIO_TECH_GROUP_C2K]);

    int c2kFd = -1;
    int c2kSlotId = -1;
    if (socket_1_fds[RADIO_TECH_GROUP_C2K] != -1) {
        c2kFd = socket_1_fds[RADIO_TECH_GROUP_C2K];
        c2kSlotId = 0;
    } else if (socket_2_fds[RADIO_TECH_GROUP_C2K] != -1) {
        c2kFd = socket_2_fds[RADIO_TECH_GROUP_C2K];
        c2kSlotId = 1;
    }
    RFX_LOG_D(LOG_TAG,
            "switchC2kSocket check current c2k fd (c2kSlotId=%d, c2kFd=%d, targetSlotId=%d)",
            c2kSlotId, c2kFd, targetSlotId);

    // check current condition
    int doSwitch = 0;
    if (c2kSlotId == -1) {
        RFX_LOG_D(LOG_TAG, "switchC2kSocket status error (c2k fd not exist)");
        doSwitch = 0;
        ret = false;
    } else if (c2kSlotId == targetSlotId) {
        RFX_LOG_D(LOG_TAG, "switchC2kSocket c2k already at target slot (c2kSlotId=%d, c2kFd=%d)",
                c2kSlotId, c2kFd);
        doSwitch = 0;
        ret = true;
    } else {
        doSwitch = 1;
        ret = true;
    }

    if (doSwitch == 1) {
        switch (targetSlotId) {
        case 0:
            socket_1_fds[RADIO_TECH_GROUP_C2K] = c2kFd;
            socket_2_fds[RADIO_TECH_GROUP_C2K] = -1;
            break;

        case 1:
            socket_1_fds[RADIO_TECH_GROUP_C2K] = -1;
            socket_2_fds[RADIO_TECH_GROUP_C2K] = c2kFd;
            break;

        }
    }

    pthread_mutex_unlock(&socket_2_mutex[RADIO_TECH_GROUP_C2K]);
    pthread_mutex_unlock(&socket_1_mutex[RADIO_TECH_GROUP_C2K]);
    RFX_LOG_D(LOG_TAG, "switchC2kSocket - release all fds mutex (doSwitch=%d)", doSwitch);

    if (doSwitch) {
        switch (targetSlotId) {
        case 0:
            clearRequestQueue(1, RADIO_TECH_GROUP_C2K);
            break;
        case 1:
            clearRequestQueue(0, RADIO_TECH_GROUP_C2K);
            break;
        }
    }

    RFX_LOG_D(LOG_TAG, "switchC2kSocket done fds: %d, %d", socket_1_fds[RADIO_TECH_GROUP_C2K],
            socket_2_fds[RADIO_TECH_GROUP_C2K]);
    return ret;
}

bool RfxRilAdapter::closeSocket(int slotId, RILD_RadioTechnology_Group group) {

    RFX_LOG_D(LOG_TAG, "closeSocket (slotId=%d, group=%d)", slotId, group);

    if ( (slotId >= sim_count) ||
            (group >= RADIO_TECH_GROUP_NUM) ) {
        return false;
    }

    switch (slotId) {
    case 0:
        pthread_mutex_lock(&socket_1_mutex[group]);
        socket_1_fds[group] = -1;
        pthread_mutex_unlock(&socket_1_mutex[group]);

        break;
    case 1:
        pthread_mutex_lock(&socket_2_mutex[group]);
        socket_2_fds[group] = -1;
        pthread_mutex_unlock(&socket_2_mutex[group]);
        break;
    }
    return false;
}

#define MAX_COMMAND_BYTES (20 * 1024)

bool RfxRilAdapter::requestToRild(const sp<RfxMessage>& message) {
    // arg check
    if (message == NULL || message.get() == NULL) {
        RFX_LOG_D(LOG_TAG, "(w)requestToRild arg check fail");
        return false;
    }

    // type check
    if (message->getType() != RFX_MESSAGE_TYPE::REQUEST) {
        RFX_LOG_D(LOG_TAG, "(w)requestToRild type check fail (%d)", message->getType());
        return false;
    }

    // fd check
    int slotId = message->getSlotId();
    int dest = message->getDest();
    int targetFd = -1;
    if (slotId == 0) {
        targetFd = socket_1_fds[dest];
    } else if (slotId == 1) {
        targetFd = socket_2_fds[dest];
    }

    if (targetFd == -1) {
        int currC2kSlotId = RpCdmaLteModeController::getCdmaSocketSlotId();
        if (dest == RADIO_TECH_GROUP_C2K && slotId != currC2kSlotId) {
            RFX_LOG_D(LOG_TAG,
                    "(w)requestToRild incorrect c2k socket (slotId=%d, dest=%d, targetFd=%d) (c2kslotId=%d)",
                    slotId, dest, targetFd, currC2kSlotId);
            return false;
        }

        RFX_LOG_D(LOG_TAG, "(w)requestToRild socket not ready (slotId=%d, dest=%d, targetFd=%d)",
                slotId, dest, targetFd);
        addRequestQueue(message->getSlotId(), message->getDest(), message);
        return true;
    }

    if (!isRequestQueueEmpty(message->getSlotId(), message->getDest())) {
        RFX_LOG_D(LOG_TAG,
                "(w)requestToRild request queue not empty, dispatch first. (slotId=%d, dest=%d, targetFd=%d)",
                slotId, dest, targetFd);
        dispatchRequestQueue(message->getSlotId(), message->getDest());
    }

    return requestToRildX(message);
}

bool RfxRilAdapter::requestToRildX(const sp<RfxMessage>& message) {
    bool sendResult = false;

    uint8_t dataLength[4];

    // arg check
    if (message == NULL || message.get() == NULL) {
        RFX_LOG_D(LOG_TAG, "requestToRild arg check fail");
        return false;
    }

    // type check
    if (message->getType() != RFX_MESSAGE_TYPE::REQUEST) {
        RFX_LOG_D(LOG_TAG, "requestToRild type check fail (%d)", message->getType());
        return false;
    }

    // fd check
    int slotId = message->getSlotId();
    int dest = message->getDest();
    int targetFd = -1;
    if (slotId == 0) {
        targetFd = socket_1_fds[dest];
    } else if (slotId == 1) {
        targetFd = socket_2_fds[dest];
    }
    if (targetFd == -1) {
        RFX_LOG_D(LOG_TAG, "requestToRild socket not ready (slotId=%d, dest=%d, targetFd=%d)",
                slotId, dest, targetFd);
        return false;
    }

    int reqId = message->getId();
    // check data obj
    // Parcel* parcel = message->getParcel();
    Parcel *parcel = NULL;
    if (message->getDataObj() != NULL) {
        // RFX_LOG_D(LOG_TAG, "requestToRild: use data obj");
        message->getDataObj()->rfxData2Parcel(parcel);
    } else {
        // RFX_LOG_D(LOG_TAG, "requestToRild: use parcel");
        parcel = message->getParcel();
    }

    // payload check
    if (parcel == NULL) {
        RFX_LOG_D(LOG_TAG, "requestToRild parcel is NULL");
        return false;
    }
    int dataSize = parcel->dataSize();
    const uint8_t* data = parcel->data();
    if (dataSize > MAX_COMMAND_BYTES) {
        RFX_LOG_D(LOG_TAG, "requestToRild parcel dataSize check fail %d (>%d)",
                dataSize, MAX_COMMAND_BYTES);
        return false;
    }

    // parcel length in big endian
    dataLength[0] = dataLength[1] = 0;
    dataLength[2] = ((dataSize >> 8) & 0xff);
    dataLength[3] = ((dataSize) & 0xff);

    if (slotId == 0) {
        pthread_mutex_lock(&socket_1_mutex[dest]);
    } else if (slotId == 1) {
        pthread_mutex_lock(&socket_2_mutex[dest]);
    }

    sendResult = true;

    // Send Data
    ssize_t sent = 0;
    RfxDispatchThread::addMessageToPendingQueue(message);
    sent = send(targetFd , dataLength , 4, 0);
    if( sent < 0 ) {
        RFX_LOG_D(LOG_TAG,
                "requestToRild send datalen fail (sent=%d, err=%d)(%d, %d, %d)(%d, %d, %d, %d)",
                sent, errno, slotId, dest, targetFd, dataLength[0], dataLength[1], dataLength[2],
                dataLength[3]);
        sendResult = false;
        pendingQueue.checkAndDequeue(message->getToken());
    }
    if (sendResult) {
        sent += send(targetFd , data , dataSize, 0);
        if( sent < 0 ) {
            RFX_LOG_D(LOG_TAG,
                    "requestToRild send data fail (sent=%d, err=%d)(%d, %d, %d, dataSize=%d)",
                    sent, errno, slotId, dest, targetFd, dataSize);
            sendResult = false;
            pendingQueue.checkAndDequeue(message->getToken());
        }
    }

    if (slotId == 0) {
        pthread_mutex_unlock(&socket_1_mutex[dest]);
    } else if (slotId == 1) {
        pthread_mutex_unlock(&socket_2_mutex[dest]);
    }
    // RfxDispatchThread::addMessageToPendingQueue(message);

    RFX_LOG_D(LOG_TAG,
            "requestToRild send request done (slotId=%d, dest=%d, reqId=%d, %s, dataSize=%d, %d)",
            slotId, dest, reqId, (sendResult == true ? "true" : "false"), dataSize, sent);
    return sendResult;
}

extern "C" bool RIL_onRequestCompleteRfx(int slotId, int token, RIL_Errno e, Parcel* parcel);
extern "C" bool RIL_onUnsolicitedResponseRfx(int slotId, int unsolResponse, Parcel* parcel);

bool RfxRilAdapter::responseToRilj(const sp<RfxMessage>& message) {

    bool ret = false;

    void* data = NULL;
    int datalen = 0;

    // arg check
    if (message == NULL || message.get() == NULL) {
        RFX_LOG_D(LOG_TAG, "requestToRilj arg check fail");
        return false;
    }

    int slotId = message->getSlotId();
    RFX_MESSAGE_TYPE type = message->getType();

    // type check
    if (type != RFX_MESSAGE_TYPE::RESPONSE && type != RFX_MESSAGE_TYPE::URC) {
        RFX_LOG_D(LOG_TAG, "requestToRilj type check fail (%d)", type);
        return false;
    }

    // payload check
    Parcel* parcel = message->getParcel();
    if (parcel == NULL) {
        RFX_LOG_D(LOG_TAG, "requestToRilj parcel is NULL");
        return false;
    }

    int dataSize = parcel->dataSize();
    if (type == RFX_MESSAGE_TYPE::RESPONSE) {
        if (message -> getClientId() != -1) {
            void* clientData = NULL;
            size_t clientDatalen = 0;
            parcel->setDataPosition(sizeof(uint32_t)*3);
            clientDatalen = (size_t)parcel->dataAvail();
            clientData = (void*)(parcel->readInplace(datalen));
            RFX_LOG_D(LOG_TAG, "client: response to client");
            ret = RpRilClientController::
                onClientRequestComplete(slotId, message->getPToken(), message->getError(),
                                        clientData, clientDatalen, message -> getClientId());
        } else {
            ret = RIL_onRequestCompleteRfx(slotId, message->getPToken(), message->getError(), parcel);
        }
        RFX_LOG_D(LOG_TAG,
                "responseToRilj send Response done (slotId=%d, pToken=%d, pId=%d, dataSize=%d)",
                slotId, message->getPToken(), message->getPId(), dataSize);
    } else if (type == RFX_MESSAGE_TYPE::URC) {
        // for Ril Client
        void* clientData = NULL;
        size_t clientDatalen = 0;
        parcel->setDataPosition(sizeof(uint32_t)*2);
        clientDatalen = (size_t)parcel->dataAvail();
        clientData = (void*)(parcel->readInplace(datalen));
        RFX_LOG_D(LOG_TAG, "client: unsol response to client");
        ret = RpRilClientController::
            onClientUnsolicitedResponse(slotId, message->getPId(), clientData, clientDatalen);

        ret = RIL_onUnsolicitedResponseRfx(slotId, message->getPId(), parcel);
        RFX_LOG_D(LOG_TAG, "responseToRilj send Urc done (slotId=%d, PId=%d, dataSize=%d)",
                slotId, message->getPId(), dataSize);
    }

    return ret;
}

bool RfxRilAdapter::setSapSocket(RIL_SOCKET_ID socketId, RILD_RadioTechnology_Group group,
        int fd) {
    if (socketId > SIM_COUNT) {
        RFX_LOG_D(LOG_TAG, "setSapSocket parameter error: %d", socketId);
        return false;
    }

    switch (group) {
        case RADIO_TECH_GROUP_SAP_GSM:
            pthread_mutex_lock(&sap_socket_gsm_mutex[socketId]);
            socket_sap_gsm_fds[socketId] = fd;
            pthread_mutex_unlock(&sap_socket_gsm_mutex[socketId]);
            RFX_LOG_D(LOG_TAG, "setSapSocket (socketId=%d, group=%d, fd=%d)", socketId, group, fd);
            break;
        case RADIO_TECH_GROUP_SAP_C2K:
            pthread_mutex_lock(&sap_socket_c2k_mutex[socketId]);
            socket_sap_c2k_fds[socketId] = fd;
            pthread_mutex_unlock(&sap_socket_c2k_mutex[socketId]);
            RFX_LOG_D(LOG_TAG, "setSapSocket (socketId=%d, group=%d, fd=%d)", socketId, group, fd);
            break;
        case RADIO_TECH_GROUP_SAP_BT:
            pthread_mutex_lock(&sap_socket_bt_mutex[socketId]);
            socket_sap_bt_fds[socketId] = fd;
            pthread_mutex_unlock(&sap_socket_bt_mutex[socketId]);
            RFX_LOG_D(LOG_TAG, "setSapSocket (socketId=%d, group=%d, fd=%d)", socketId, group, fd);
            break;
    }
    return true;
}

bool RfxRilAdapter::sendSapMessageToRild(void *data, int datalen, RIL_SOCKET_ID socketId) {
    bool ret = true;
    int targetFd = -1;

    if (data == NULL) {
        RFX_LOG_D(LOG_TAG, "sendSapMessageToRild arg check fail");
        return false;
    }

    RILD_RadioTechnology_Group dest = RfxRilAdapterUtils::choiceDestViaCurrCardType(socketId);
    if (RADIO_TECH_GROUP_GSM == dest) {
        // send to gsm rild
        targetFd = socket_sap_gsm_fds[socketId];
        RFX_LOG_D(LOG_TAG, "sendSapMessageToRild send to gsm, fd = %d", targetFd);
    } else if (RADIO_TECH_GROUP_C2K == dest) {
        // send to c2k rild
        targetFd = socket_sap_c2k_fds[socketId];
        RFX_LOG_D(LOG_TAG, "sendSapMessageToRild send to c2k, fd = %d", targetFd);
    }
    if (targetFd == -1) {
        RFX_LOG_D(LOG_TAG, "sendSapMessageToRild socket not ready (dest=%d, targetFd=%d)",
                dest, targetFd);
        return false;
    }

    if (RADIO_TECH_GROUP_GSM == dest) {
        pthread_mutex_lock(&sap_socket_gsm_mutex[socketId]);
    } else {
        pthread_mutex_lock(&sap_socket_c2k_mutex[socketId]);
    }
    ssize_t sent = 0;
    sent = send(targetFd , data , datalen, 0);
    if (sent < 0) {
        RFX_LOG_D(LOG_TAG,
                "sendSapMessageToRild send data fail (sent=%d, err=%d)(%d, %d, %d, dataSize=%d)",
                sent, errno, socketId, dest, targetFd, datalen);
        ret = false;
    }
    if (RADIO_TECH_GROUP_GSM == dest) {
        pthread_mutex_unlock(&sap_socket_gsm_mutex[socketId]);
    } else {
        pthread_mutex_unlock(&sap_socket_c2k_mutex[socketId]);
    }

    return ret;
}

static char PROPERTY_RIL_FULL_UICC_TYPE[4][25] = {
        "gsm.ril.fulluicctype",
        "gsm.ril.fulluicctype.2",
        "gsm.ril.fulluicctype.3",
        "gsm.ril.fulluicctype.4",
};

Parcel* RfxRilAdapterUtils::extractResponse(uint8_t* buf, int buflen) {
    Parcel* parcel = NULL;

    if (buf != NULL) {
        parcel = new Parcel();
        parcel->setData(buf, buflen); // p.setData((uint8_t *) buffer, buflen);
        parcel->setDataPosition(0);
    }

    return parcel;
}

RILD_RadioTechnology_Group RfxRilAdapterUtils::choiceDestViaCurrCardType(int slotId) {
    RILD_RadioTechnology_Group dest = RADIO_TECH_GROUP_GSM;
    const char *fullUiccType = NULL;
    char tmp[PROPERTY_VALUE_MAX] = {0};

    do {
        if (slotId >= 0 && slotId <= 4) {
            fullUiccType = PROPERTY_RIL_FULL_UICC_TYPE[slotId];
            property_get(fullUiccType, tmp, "");
        } else {
            RFX_LOG_D(LOG_TAG, "choiceDestViaCurrCardType, slotId %d is wrong!", slotId);
            break;
        }

        if ((strncmp(tmp, "SIM", 3) == 0) || (strstr(tmp, "USIM") != NULL)) {
            // Send GSM RILD the request if there is SIM/USIM
            break;
        }

        if ((strstr(tmp, "CSIM") != NULL) || (strstr(tmp, "RUIM") != NULL)) {
            // Send C2K RILD the request if this is pure CDMA card
            dest = RADIO_TECH_GROUP_C2K;
            break;
        }

        RFX_LOG_D(LOG_TAG, "choiceDestViaCurrCardType, No 3GPP and 3GPP2?!(slot %d)",
                slotId);
    } while (0);

    RFX_LOG_D(LOG_TAG, "choiceDestViaCurrCardType, dest %d! (slot %d)", dest, slotId);
    return dest;
}

bool RfxRilAdapter::addRequestQueue(int slotId, RILD_RadioTechnology_Group group,
        const sp<RfxMessage>& message) {
    bool ret = false;
    Vector<sp<RfxMessage>>* rQueue = NULL;

    if (group == RADIO_TECH_GROUP_GSM) {
        pthread_mutex_lock(&request_queue_gsm_mutex[slotId]);
        if (slotId == 0) {
            rQueue = &request_queue_gsm_1;
        } else if (slotId == 1) {
            rQueue = &request_queue_gsm_2;
        }
    } else if (group == RADIO_TECH_GROUP_C2K) {
        pthread_mutex_lock(&request_queue_c2k_mutex);
        rQueue = &request_queue_c2k;
    }

    if (rQueue !=NULL) {
        rQueue->add(message);
        ret = true;
    }

    if (group == RADIO_TECH_GROUP_GSM) {
        pthread_mutex_unlock(&request_queue_gsm_mutex[slotId]);
    } else if (group == RADIO_TECH_GROUP_C2K) {
        pthread_mutex_unlock(&request_queue_c2k_mutex);
    }

    RFX_LOG_D(LOG_TAG, "addRequestQueue slotId=%d, group=%d, id=%d, type=%d, ret=%s",
            slotId, group, message->getId(), message->getType(), (ret ? "t" : "f"));
    return ret;
}

bool RfxRilAdapter::isRequestQueueEmpty(int slotId, RILD_RadioTechnology_Group group) {
    bool ret = true;

    Vector<sp<RfxMessage>>* rQueue = NULL;

    if (group == RADIO_TECH_GROUP_GSM) {
        pthread_mutex_lock(&request_queue_gsm_mutex[slotId]);
        if (slotId == 0) {
            rQueue = &request_queue_gsm_1;
        } else if (slotId == 1) {
            rQueue = &request_queue_gsm_2;
        }
    } else if (group == RADIO_TECH_GROUP_C2K) {
        pthread_mutex_lock(&request_queue_c2k_mutex);
        rQueue = &request_queue_c2k;
    }

    if (rQueue !=NULL) {
        ret = rQueue->isEmpty();
    }

    if (group == RADIO_TECH_GROUP_GSM) {
        pthread_mutex_unlock(&request_queue_gsm_mutex[slotId]);
    } else if (group == RADIO_TECH_GROUP_C2K) {
        pthread_mutex_unlock(&request_queue_c2k_mutex);
    }

    RFX_LOG_D(LOG_TAG, "isRequestQueueEmpty slotId=%d, group=%d, ret=%s", slotId, group,
            (ret ? "t" : "f"));
    return ret;
}

void RfxRilAdapter::onSocketStateChanged(RfxStatusKeyEnum key, RfxVariant old_value,
        RfxVariant value) {
    RFX_UNUSED(key);
    RfxSocketState old_socket_state = old_value.asSocketState();
    RfxSocketState socket_state = value.asSocketState();
    int slotId = socket_state.getSlotId();
    bool stateValue = false, stateValueOld = false;
    // check gsm
    stateValue = socket_state.getSocketState(RfxSocketState::SOCKET_GSM);
    stateValueOld = old_socket_state.getSocketState(RfxSocketState::SOCKET_GSM);
    if (stateValue != stateValueOld) {
        RFX_LOG_D(LOG_TAG, "onSocketStateChanged, slotId=%d, gsm, stateValue: %s -> %s", slotId,
                (stateValueOld ? "t" : "f"), (stateValue ? "t" : "f"));
        if (!stateValueOld && stateValue) {
            // disconnect -> connect, check queue & dispatch
            bool reqQueueEmpty = isRequestQueueEmpty(slotId, RADIO_TECH_GROUP_GSM);
            if (!reqQueueEmpty) {
                dispatchRequestQueue(slotId, RADIO_TECH_GROUP_GSM);
            }
        } else if (stateValueOld && !stateValue) {
            clearRequestQueue(slotId, RADIO_TECH_GROUP_GSM);
        }
    }
    // check c2k
    bool isCdmaSlot = socket_state.getIsCdmaSlot();
    if (isCdmaSlot) {
        stateValue = socket_state.getSocketState(RfxSocketState::SOCKET_C2K);
        stateValueOld = old_socket_state.getSocketState(RfxSocketState::SOCKET_C2K);
        if (stateValue != stateValueOld) {
            RFX_LOG_D(LOG_TAG, "onSocketStateChanged, slotId=%d, c2k, stateValue: %s -> %s", slotId,
                    (stateValueOld ? "t" : "f"), (stateValue ? "t" : "f"));
            if (!stateValueOld && stateValue) {
                // disconnect -> connect, check queue & dispatch
                bool reqQueueEmpty = isRequestQueueEmpty(slotId, RADIO_TECH_GROUP_C2K);
                if (!reqQueueEmpty) {
                    dispatchRequestQueue(slotId, RADIO_TECH_GROUP_C2K);
                }
            } else if (stateValueOld && !stateValue) {
                clearRequestQueue(slotId, RADIO_TECH_GROUP_C2K);
            }
        }
    }
    RFX_LOG_D(LOG_TAG, "onSocketStateChanged, done");
}

void RfxRilAdapter::registerSocketStateListener() {
    RFX_LOG_D(LOG_TAG, "registerSocketStateListener");

    for (int i=0; i < SIM_COUNT; i++) {
        RfxStatusManager *slotStatusMgr = RFX_OBJ_GET_INSTANCE(RfxRootController)->getStatusManager(i);
        if (slotStatusMgr != NULL) {
            slotStatusMgr->registerStatusChanged(RFX_STATUS_KEY_SOCKET_STATE,
                    RfxStatusChangeCallback(this, &RfxRilAdapter::onSocketStateChanged));
        } else {
            RFX_LOG_E(LOG_TAG, "registerSocketStateListener slotStatusManager(%d) NULL", i);
        }
    }
}

void RfxRilAdapter::unregisterSocketStateListener() {
    RFX_LOG_D(LOG_TAG, "unregisterSocketStateListener");

    for (int i=0; i < SIM_COUNT; i++) {
        RfxStatusManager *slotStatusMgr = RFX_OBJ_GET_INSTANCE(RfxRootController)->getStatusManager(i);
        if (slotStatusMgr != NULL) {
            slotStatusMgr->unRegisterStatusChanged(RFX_STATUS_KEY_SOCKET_STATE,
                    RfxStatusChangeCallback(this, &RfxRilAdapter::onSocketStateChanged));
        } else {
            RFX_LOG_E(LOG_TAG, "unregisterSocketStateListener slotStatusManager(%d) NULL", i);
        }
    }
}

bool RfxRilAdapter::dispatchRequestQueue(int slotId, RILD_RadioTechnology_Group group) {
    bool ret = false;

    int dispatchCount = 0;
    Vector<sp<RfxMessage>>* rQueue = NULL;
    if (group == RADIO_TECH_GROUP_GSM) {
        pthread_mutex_lock(&request_queue_gsm_mutex[slotId]);
        if (slotId == 0) {
            rQueue = &request_queue_gsm_1;
        } else if (slotId == 1) {
            rQueue = &request_queue_gsm_2;
        }
    } else if (group == RADIO_TECH_GROUP_C2K) {
        pthread_mutex_lock(&request_queue_c2k_mutex);
        rQueue = &request_queue_c2k;
    }

    if (rQueue !=NULL) {
        if (!rQueue->isEmpty()) {
            for (size_t i = 0; i < rQueue->size(); i++) {
                sp<RfxMessage> msg = rQueue->itemAt(i);
                requestToRildX(msg);
                dispatchCount++;
            }
            rQueue->clear();
        }
        ret = true;
    }

    if (group == RADIO_TECH_GROUP_GSM) {
        pthread_mutex_unlock(&request_queue_gsm_mutex[slotId]);
    } else if (group == RADIO_TECH_GROUP_C2K) {
        pthread_mutex_unlock(&request_queue_c2k_mutex);
    }

    RFX_LOG_D(LOG_TAG, "dispatchRequestQueue slotId=%d, group=%d, ret=%s, dispatchCount=%d", slotId,
            group, (ret ? "t" : "f"), dispatchCount);
    return ret;
}

void RfxRilAdapter::clearRequestQueue(int slotId, RILD_RadioTechnology_Group group) {
    int clearCount = 0;
    Vector<sp<RfxMessage>>* rQueue = NULL;
    if (group == RADIO_TECH_GROUP_GSM) {
        pthread_mutex_lock(&request_queue_gsm_mutex[slotId]);
        if (slotId == 0) {
            rQueue = &request_queue_gsm_1;
        } else if (slotId == 1) {
            rQueue = &request_queue_gsm_2;
        }
    } else if (group == RADIO_TECH_GROUP_C2K) {
        pthread_mutex_lock(&request_queue_c2k_mutex);
        rQueue = &request_queue_c2k;
    }

    if (rQueue !=NULL) {
        for (size_t i = 0; i < rQueue->size(); i++) {
            sp<RfxMessage> msg = rQueue->itemAt(i);
            RFX_LOG_D(LOG_TAG, "clearRequestQueue slotId=%d, group=%d, queue[%d]: id=%d", slotId,
                    group, i, msg->getId());
            clearCount++;
        }
        rQueue->clear();
    }

    if (group == RADIO_TECH_GROUP_GSM) {
        pthread_mutex_unlock(&request_queue_gsm_mutex[slotId]);
    } else if (group == RADIO_TECH_GROUP_C2K) {
        pthread_mutex_unlock(&request_queue_c2k_mutex);
    }

    RFX_LOG_D(LOG_TAG, "clearRequestQueue slotId=%d, group=%d, clearCount=%d", slotId, group,
            clearCount);
}

void RfxRilAdapter::initSocketNotifyByFwk() {
    registerSocketStateListener();
}

void RfxRilAdapter::clearAllRequestQueue() {
    RFX_LOG_D(LOG_TAG, "clearAllRequestQueue");
    clearRequestQueue(0, RADIO_TECH_GROUP_GSM);
    clearRequestQueue(0, RADIO_TECH_GROUP_C2K);
    clearRequestQueue(1, RADIO_TECH_GROUP_GSM);
    clearRequestQueue(1, RADIO_TECH_GROUP_C2K);
}
