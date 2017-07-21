/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
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

#include <cutils/sockets.h>
#include <utils/Log.h>
#include <assert.h>
#include <pthread.h>
#include <cutils/properties.h>
#include <telephony/record_stream.h>
#include <stdio.h>
#include <stdlib.h>
#include <cutils/sockets.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/select.h>

#include "ril.h"
#include "Rfx.h"
#include "socket_channel.h"
#include "socket_util.h"
#include "RfxMainThread.h"

extern "C"
void setRadioState(RIL_RadioState newState, RIL_SOCKET_ID rid);

extern "C"
void decodeAndSendSapMessage(void *data, size_t datalen,
        RIL_SOCKET_ID socketId);

#define MAX_COMMAND_BYTES (20 * 1024)
#define MAX_DATA_SIZE 2048

extern void *ril_socket_reader_looper(void *arg);
extern void *ril_atci_socket_reader_looper(void *arg);
extern void *sap_ril_proxy_rild_socket_reader_looper(void *arg);

RFX_IMPLEMENT_CLASS("RilpAtciSocket", RilpAtciSocket, RfxObject);
RFX_OBJ_IMPLEMENT_SINGLETON_CLASS(RilpAtciSocket);

void RilpSocket::connectSocket(void)
{
    RLOGI ("connectSocket to %d %d socket_name %s\n", group, id, name);
    while (1) {
        socketFd = socket_local_client(name,
                ANDROID_SOCKET_NAMESPACE_RESERVED,
                SOCK_STREAM);
        if(socketFd > 0) {
            RLOGI ("connectSocket to %d\n", socketFd);
            rfx_set_socket(group, id, socketFd);
            return;
        } else {
            RLOGD ("connectSocket fail, try again %d\n", socketFd);
            sleep(1);
        }
    }
}

const char *RilpSocket::rildGroupIdtoString(RILD_RadioTechnology_Group group) {
    switch (group) {
        case RADIO_TECH_GROUP_GSM:
            return "GSM";
        case RADIO_TECH_GROUP_C2K:
            return "C2K";
        default:
            RLOGI ("Wrong group id: %d\n", group);
            break;
    }
    return NULL;
}

void RilpSocket::readerLooper(void) {
    RecordStream *p_rs = record_stream_new(socketFd, MAX_COMMAND_BYTES);
    void *p_record;
    size_t recordlen;
    int ret;

    RLOGI ("readerLooper Group: %s, SlotId: %d, socketFd:%d\n", rildGroupIdtoString(group), id, socketFd);

    while (1) {
        for (;;) {
            /* loop until EAGAIN/EINTR, end of stream, or other error */
            ret = record_stream_get_next(p_rs, &p_record, &recordlen);

            if (ret == 0 && p_record == NULL) {
                /* end-of-stream */
                break;
            } else if (ret < 0) {
                break;
            } else if (ret == 0) { /* && p_record != NULL */
                rfx_process_raw_data(group, id, p_record, recordlen);
            }
        }
        if (ret == 0 || !(errno == EAGAIN || errno == EINTR)) {
            /* fatal error or end-of-stream */
            if (ret != 0) {
                RLOGE("error on reading command socket errno:%d\n", errno);
            } else {
                RLOGW("EOS.  Closing command socket.");
            }

            close(socketFd);
            record_stream_free(p_rs);
            socketFd = -1;
            return;
        }
    }
}

void RilpSocket::initSocket(const char *socketName, RIL_SOCKET_ID socketid, RILD_RadioTechnology_Group groupId) {
    pthread_attr_t attr;
    PthreadPtr pptr = ril_socket_reader_looper;
    int result;

    RilpSocket* socket = new RilpSocket(socketName, socketid, groupId);
    socket->connectSocket();

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    //Start socket data receiving loop thread
    result = pthread_create(&socket->socketThreadId, &attr, pptr, socket);
    if(result < 0) {
        RLOGE("pthread_create failed with result:%d",result);
    }

    RLOGI("socket connected and socket data loop thread started");
}
//RilpAtciSocket* RilpAtciSocket::s_instance = new RilpAtciSocket();

void RilpAtciSocket::initSocket(const char *socketName) {
    pthread_attr_t attr;
    PthreadPtr pptr = ril_atci_socket_reader_looper;
    int result;

    RilpAtciSocket *rilpAtciSocket = RilpAtciSocket::getInstance();
    rilpAtciSocket->name = socketName;
    rilpAtciSocket->connectSocket();

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    //Start socket data receiving loop thread
    result = pthread_create(&rilpAtciSocket->socketThreadId, &attr,
            pptr, rilpAtciSocket);
    if(result < 0) {
        RLOGE("pthread_create failed with result:%d",result);
    }
    RLOGI("socket connected and socket data loop thread started");
}

void RilpAtciSocket::connectSocket(void) {
    RLOGI ("connectSocket to %d socket_name %s\n", id, name);
    int ret;

    socketFd = android_get_control_socket(name);
    if (socketFd < 0) {
        RLOGE("Failed to get socket : %s errno:%d", name, errno);
        return;
    }
    ret = listen(socketFd, 4);
    RLOGI ("connectSocket to %d\n", socketFd);

    if (ret < 0) {
        RLOGE("Failed to listen on ril ATCI socket '%d': %s",
                socketFd, strerror(errno));
        return;
    }
}

void RilpAtciSocket::readerLooper(void) {
    int ret;
    int request = RIL_REQUEST_OEM_HOOK_RAW;//RIL_LOCAL_REQUEST_OEM_HOOK_ATCI_INTERNAL;
    int token = 0xFFFFFFFF;
    int recvLen = -1;
    char buffer[MAX_DATA_SIZE] = {0};
    fd_set rfds;

    RLOGI ("[ATCI] readerLooper SlotId: %d, clientSocketFd: %d\n", id, clientSocketFd);

    while(1) {
        FD_ZERO(&rfds);
        FD_SET(clientSocketFd, &rfds);
        memset(buffer, 0, sizeof(char) * MAX_DATA_SIZE);
        RLOGI ("[ATCI] select fd: %d\n", clientSocketFd);
        ret = select(clientSocketFd + 1, &rfds, NULL, NULL, NULL);
        if (ret == -1) {
            if(errno == EINTR || errno == EAGAIN) {
                continue;
            }
            RLOGE("[ATCI] Fail to select in readline. error: %d", errno);
            close(clientSocketFd);
            return;
        } else if(ret == 0) {
            RLOGE("[ATCI] ERROR: No data from atci socket.");
            continue;
        }
        if (clientSocketFd > 0 && FD_ISSET(clientSocketFd, &rfds)) {
            do {
                recvLen = recv(clientSocketFd, buffer, MAX_DATA_SIZE, 0);
                RLOGD("[ATCI] buffer %s, length %d", buffer, recvLen);
                if (recvLen == -1) {
                    RLOGE("[ATCI] fail to recv from ril-atci socket. errno = %d", errno);
                    if(errno != EAGAIN && errno != EINTR) {
                        return;
                    } else {
                        RLOGE("[ATCI] Try again. errno = %d", errno);
                        continue;
                    }
                } else if (recvLen == 0) {
                    continue;
                }
            } while(recvLen <= 0);
            Parcel* p =  new Parcel();
            char *atBuffer;

            p->writeInt32(request);
            p->writeInt32(token);

            atBuffer = (char*)calloc(1, (recvLen * sizeof(char)) + 1);
            memset(atBuffer, 0, (recvLen * sizeof(char)) + 1);
            memcpy(atBuffer, buffer, recvLen);

            p->writeInt32(recvLen + 1);
            p->write((void*) atBuffer, (size_t) recvLen + 1);
            p->setDataPosition(0);

            process_raw_data(p);
            if (NULL != atBuffer) {
                free(atBuffer);
            }
        }
    }
}
/*RilpAtciSocket* RilpAtciSocket::getInstance(void) {
    if (s_instance == NULL) {
        s_instance = new RilpAtciSocket();
    }
    return s_instance;
}*/

int RilpAtciSocket::getAtciRilSocket(void) {
    return clientSocketFd;
}

void RilpAtciSocket::process_raw_data(void *pRecord) {
    char simNo[PROPERTY_VALUE_MAX] = {0};
    int slotId = 0;

    property_get("persist.service.atci.sim", simNo, "0");
    RLOGD("process_raw_data simNo: %d.", simNo[0]);

    if (simNo[0] == '0' || simNo[0] == '9') {//SIM 1
        slotId = RIL_SOCKET_ID(0);
    } else if (simNo[0] == '1') {//SIM 2
        slotId = RIL_SOCKET_ID(1);
    } else if (simNo[0] == '2') {//SIM 3
        slotId = RIL_SOCKET_ID(2);
    } else if (simNo[0] == '3') {//SIM 4
        slotId = RIL_SOCKET_ID(3);
    } else {
        RLOGD("process_raw_data unsupported slot number.");
        return;
    }

     rfx_enqueue_request_message_old(RIL_LOCAL_REQUEST_OEM_HOOK_ATCI_INTERNAL, slotId,
            RADIO_TECH_GROUP_GSM, 0xffffffff, pRecord);
}

void SapRilProxyRildSocket::initSocket(const char *socketName, RIL_SOCKET_ID socketid,
        RILD_RadioTechnology_Group groupId) {
    pthread_attr_t attr;
    PthreadPtr pptr = sap_ril_proxy_rild_socket_reader_looper;
    int result;

    SapRilProxyRildSocket* socket = new SapRilProxyRildSocket(socketName, socketid, groupId);
    socket->connectSocket();

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    // Start socket data receiving loop thread
    result = pthread_create(&socket->socketThreadId, &attr, pptr, socket);
    if (result < 0) {
        RLOGE("pthread_create failed with result:%d", result);
    }

    RLOGI("socket connected and socket data loop thread started");
}

void SapRilProxyRildSocket::connectSocket(void) {
    RLOGI("connectSocket to %d socket_name %s\n", id, name);
    int ret;

    while (1) {
        socketFd = socket_local_client(name,
                ANDROID_SOCKET_NAMESPACE_RESERVED,
                SOCK_STREAM);
        if(socketFd > 0) {
            RLOGI ("connectSocket to %d\n", socketFd);
            rfx_set_sap_socket(id , group, socketFd);
            return;
        } else {
            RLOGD ("connectSocket fail, try again %d\n", socketFd);
            sleep(1);
        }
    }
}

void SapRilProxyRildSocket::readerLooper(void) {
    RecordStream *p_rs = record_stream_new(socketFd, MAX_COMMAND_BYTES);
    void *p_record;
    size_t recordlen;
    int ret;

    RLOGI ("readerLooper Group: %s, SlotId: %d, socketFd:%d\n", rildGroupIdtoString(group), id,
            socketFd);

    while (1) {
        for (;;) {
            /* loop until EAGAIN/EINTR, end of stream, or other error */
            ret = record_stream_get_next(p_rs, &p_record, &recordlen);

            if (ret == 0 && p_record == NULL) {
                /* end-of-stream */
                break;
            } else if (ret < 0) {
                break;
            } else if (ret == 0) { /* && p_record != NULL */
                decodeAndSendSapMessage(p_record, recordlen, id);
            }
        }
        if (ret == 0 || !(errno == EAGAIN || errno == EINTR)) {
            /* fatal error or end-of-stream */
            if (ret != 0) {
                RLOGE("error on reading command socket errno:%d\n", errno);
            } else {
                RLOGW("EOS.  Closing command socket.");
            }

            close(socketFd);
            record_stream_free(p_rs);
            socketFd = -1;
            return;
        }
    }
}

const char* SapRilProxyRildSocket::rildGroupIdtoString(RILD_RadioTechnology_Group group) {
    switch (group) {
        case RADIO_TECH_GROUP_SAP_GSM:
            return "GSM SAP";
        case RADIO_TECH_GROUP_SAP_C2K:
            return "C2K SAP";
        default:
            RLOGI ("Wrong group id: %d\n", group);
            break;
    }
    return NULL;
}

bool isCdmaLteDcSupport()
{
    int isSvlteSupport = 0;
    int isSrlteSupport = 0;

    char property_value[PROPERTY_VALUE_MAX] = { 0 };
    property_get("ro.mtk_svlte_support", property_value, "0");
    isSvlteSupport = atoi(property_value);
    RLOGI("isSvlteSupport: %d", isSvlteSupport);

    property_get("ro.mtk_srlte_support", property_value, "0");
    isSrlteSupport = atoi(property_value);
    RLOGI("isSrlteSupport: %d", isSrlteSupport);

    return (isSvlteSupport || isSrlteSupport) ? true : false;
}

extern "C"
void ril_socket_init() {
    int i=0;
    char *socketName;
    for(i=0; i<RIL_SOCKET_NUM; i++) {
        socketName = RilpSocketUtil::getSocketName(RADIO_TECH_GROUP_GSM, RIL_SOCKET_ID(i));
        RilpSocket::initSocket(socketName, RIL_SOCKET_ID(i), RADIO_TECH_GROUP_GSM);
    }

    socketName = RilpSocketUtil::getSocketName(RADIO_TECH_GROUP_C2K, RIL_SOCKET_1);
    if(isCdmaLteDcSupport() == true) {
        RilpSocket::initSocket(socketName, RIL_SOCKET_1, RADIO_TECH_GROUP_C2K);
    }

    for (i=0; i<RIL_SOCKET_NUM; i++) {
        setRadioState(RADIO_STATE_OFF, RIL_SOCKET_ID(i));
    }
    socketName = RilpSocketUtil::getSocketName(RADIO_TECH_GROUP_ATCI, RIL_SOCKET_1);
    RilpAtciSocket::initSocket(socketName);

    // ril-proxy to rild socket
    for (i = 0; i < RIL_SOCKET_NUM; i++) {
        socketName = RilpSocketUtil::getSocketName(RADIO_TECH_GROUP_SAP_GSM, RIL_SOCKET_ID(i));
        SapRilProxyRildSocket::initSocket(socketName, RIL_SOCKET_ID(i), RADIO_TECH_GROUP_SAP_GSM);
    }
    socketName = RilpSocketUtil::getSocketName(RADIO_TECH_GROUP_SAP_C2K, RIL_SOCKET_ID(0));
    SapRilProxyRildSocket::initSocket(socketName, RIL_SOCKET_ID(0), RADIO_TECH_GROUP_SAP_C2K);
}

extern "C"
void *ril_socket_reader_looper(void *arg) {
    while (1) {
        RilpSocket *socket = (RilpSocket *)arg;
        socket->readerLooper();
        rfx_close_socket(socket->group, socket->id);

        for(int i=0; i<RIL_SOCKET_NUM; i++) {
            // always set to unavailable when socket is disconnected
            setRadioState(RADIO_STATE_UNAVAILABLE, RIL_SOCKET_ID(i));
        }

        RLOGI("reconnect socket");
        socket->connectSocket();
    }
    RLOGI("end of ril_socket_reader_looper");
    return NULL;
}

extern "C"
void *ril_atci_socket_reader_looper(void *arg) {
    RilpAtciSocket *socket = (RilpAtciSocket *)arg;
    int ret = 0;
    struct sockaddr_un peeraddr;
    socklen_t socklen = sizeof (peeraddr);

    RLOGD("[ATCI] start ril_atci_socket_reader_looper, socketFd %d.", socket->socketFd);

    while (1) {
        socket->clientSocketFd = accept(socket->socketFd, (sockaddr *) &peeraddr, &socklen);

        if (socket->clientSocketFd  < 0 ) {
            RLOGE("[ATCI] Error on accept() errno:%d", errno);
            sleep(200);
            continue;
        }
        RLOGD("[ATCI] accept");
        int flags = fcntl(socket->clientSocketFd, F_GETFL);
        ret = fcntl(socket->clientSocketFd , F_SETFL, flags | O_NONBLOCK);

        if (ret < 0) {
            RLOGE ("[ATCI] Error setting O_NONBLOCK errno:%d", errno);
        }

        socket->readerLooper();
        close(socket->clientSocketFd);
        socket->connectSocket();
    }
    RLOGI("end of ril_atci_socket_reader_looper");
    return NULL;
}

extern "C"
void *sap_ril_proxy_rild_socket_reader_looper(void *arg) {
    while (1) {
        SapRilProxyRildSocket *socket = (SapRilProxyRildSocket *)arg;
        socket->readerLooper();

        rfx_set_sap_socket(socket->id , socket->group, -1);
        RLOGI("reconnect socket");
        socket->connectSocket();
    }
    RLOGI("end of sap_ril_proxy_rild_socket_reader_looper");
    return NULL;
}
