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

#include "RpRilClientController.h"
#include "RilClientConstants.h"
#include "RfxSocketState.h"
#include <utils/Log.h>
#include <cutils/properties.h>
#include <string.h>
#include <ril.h>
/*****************************************************************************
 * Class RfxController
 *****************************************************************************/

#define LOG_TAG "RpRilClientController"
#define ARRAY_LENGTH(array) (sizeof(array)/sizeof(array[0]))

static ClientInformation client[] = {
    {1001, "rilproxy-mal"}
};

RFX_IMPLEMENT_CLASS("RpRilClientController", RpRilClientController, RfxController);

RpRilClientController::RpRilClientController() {
}

RpRilClientController::~RpRilClientController() {
}

RilClientQueue* RpRilClientController::clientHead = new RilClientQueue();

void RpRilClientController::onInit() {
    RLOGD("RpRilClientController init()");
    RfxController::onInit();
    initRilClient();
}

void RpRilClientController::initRilClient() {
    RLOGD("init client number: %d", ARRAY_LENGTH(client));
    for (int i = 0; i < ARRAY_LENGTH(client); i++) {
        ClientInformation information = client[i];
        RilClient* client = new RilClient(information.identity, information.socketName);
        registerRilClient(client);
    }
}


void RpRilClientController::registerRilClient(RilClient* client) {
    RilClientQueue* queue = clientHead;
    RilClientQueue* clientBeingRegistered = new RilClientQueue(client);
    while(true) {
        if (queue -> nextClient == NULL) {
            queue -> nextClient = clientBeingRegistered;
            clientBeingRegistered -> prevClient = queue;
            break;
        } else {
            queue = queue -> nextClient;
        }
    }
}

int RpRilClientController::queryFileDescriptor(int clientId) {
    RilClientQueue* queue = clientHead -> nextClient;
    while(true) {
        if (queue == NULL) {
            return -1;
        }
        RilClient* client = queue -> client;
        if (client == NULL) {
            LOG_ALWAYS_FATAL("should not have client null here");
        }if (client -> identity == clientId) {
            RLOGD("Find Client FD: %d", client -> commandFd);
            return client -> commandFd;
        } else {
            queue = queue -> nextClient;
        }
    }
}

bool RpRilClientController::onClientRequestComplete(int slotId, int token, RIL_Errno e, void *response, size_t responselen,
                                                    int clientId) {
    RLOGD("Enter onClientRequestComplete");
    int ret;
    int fd = queryFileDescriptor(clientId);
    RLOGD("onClientRequestComplete: fd is %d", fd);
    size_t errorOffset;

    Parcel p;

    p.writeInt32 (RESPONSE_SOLICITED);
    p.writeInt32 (token);
    errorOffset = p.dataPosition();
    p.writeInt32 (e);

    if (response != NULL) {
        ret = p.write(response, responselen);

        if (ret != 0) {
            RLOGD ("responseFunction error, ret %d", ret);
            p.setDataPosition(errorOffset);
            p.writeInt32 (ret);
        }
    }

    if (e != RIL_E_SUCCESS) {
        RLOGD("fails by %d", e);
    }

    if (fd < 0) {
        RLOGD ("RIL onRequestComplete: Command channel closed");
        return false;
    }

    sendResponse(p, fd);

    return true;
}

int RpRilClientController::sendResponse (Parcel& p, int fd) {
    uint32_t header;
    int ret;
    header = htonl(p.dataSize());
    ret = blockingWrite(fd, (void *)&header, sizeof(header));

    if (ret < 0) {
        RLOGD("sendResponse: ret %d", ret);
        return ret;
    }

    ret = blockingWrite(fd, p.data(), p.dataSize());

    if (ret < 0) {
        RLOGD("sendResponse: ret %d", ret);
        return ret;
    }

    return ret;
}

bool RpRilClientController::onClientUnsolicitedResponse(int slotId, int urcId, void *response,
                                                        size_t responseLen) {
    RilClientQueue* queue = clientHead -> nextClient;
    while(true) {
        if (queue == NULL) {
            break;
        }
        RilClient* client = queue -> client;
        if (client == NULL) {
            LOG_ALWAYS_FATAL("should not have client null here");
        } else {
            RLOGD("client %d handle unsolictied", client -> identity);
            client -> handleUnsolicited(slotId, urcId, response, responseLen);
        }
        queue = queue -> nextClient;
    }
    return true;
}

int RpRilClientController::blockingWrite(int fd, const void *buffer, size_t len) {
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
        } else {
            RLOGD ("RIL Response: unexpected error on write errno:%d", errno);
            close(fd);
            return -1;
        }
    }
    RLOGD("leave blocking write");
    return 0;
}

int RpRilClientController::getCdmaSlotId() {
    char tempstr[PROPERTY_VALUE_MAX];
    memset(tempstr, 0, sizeof(tempstr));
    property_get("persist.radio.cdma_slot", tempstr, "1");
    int cdmaSlotId = atoi(tempstr) - 1;
    RLOGD("CDMA slot Id is %d", cdmaSlotId);
    return cdmaSlotId;
}