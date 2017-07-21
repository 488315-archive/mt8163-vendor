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

#include <utils/Log.h>
#include <cutils/properties.h>
#include <string.h>
#include <iostream>
#include <thread>
#include <telephony/record_stream.h>
#include <cutils/sockets.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <binder/Parcel.h>
#include <string.h>
#include "RilClient.h"
#include "Rfx.h"
#include "ril.h"
#include "RpRilClientController.h"
#include "RfxDefaultDestUtils.h"

/*****************************************************************************
 * Class RfxController
 *****************************************************************************/

#define LOG_TAG "RilClient"
#define ARRAY_LENGTH(array) (sizeof(array)/sizeof(array[0]))

using namespace std;
using namespace android;

RilClient::RilClient(int identity, char* socketName) {
    this -> identity = identity;
    this -> socketName = socketName;
    this -> commandFd = -1;
    this -> listenFd = -1;
    this -> clientState = CLIENT_STATE_UNKNOWN;
    setClientState(CLIENT_INITIALIZING);
    RLOGD("init done");
}

RilClient::~RilClient() {

}

void RilClient::clientStateCallback() {
    RLOGD("Enter callback %s", clientStateToString(clientState));
    switch(clientState) {
        case CLIENT_INITIALIZING:
            handleStateInitializing();
            break;
        case CLIENT_ACTIVE:
            handleStateActive();
            break;
        case CLIENT_DEACTIVE:
            handleStateDeactive();
            break;
        case CLIENT_CLOSED:
            handleStateClosed();
            break;
        default:
            break;
    }
}

void RilClient::handleStateInitializing() {

    int ret;
    char* socketName = this -> socketName;
    struct sockaddr_un my_addr;
    struct sockaddr_un peer_addr;

    if (listenFd < 0) {
        listenFd = android_get_control_socket(socketName);
    }

    /* some trial to manually create socket, will work if permission is added
    if (listenFd < 0) {
        RLOGD("init.rc didnt define, create socket manually");
        //do retry if init.rc didn't define socket
        memset(&my_addr, 0, sizeof(struct sockaddr_un));
        my_addr.sun_family = AF_UNIX;

        char path[256];
        sprintf (path, "/data/%s", socketName);
        RLOGD("socketName is %s", path);
        strncpy(my_addr.sun_path, path,
            sizeof(my_addr.sun_path) - 1);

        listenFd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (listenFd < 0) {
            RLOGD("manually listen fail, closed");
            setClientState(CLIENT_CLOSED);
        }

        int ret = ::bind(listenFd, (struct sockaddr *) &my_addr,
                sizeof(struct sockaddr_un));
        if (ret < 0) {
            RLOGD("bind fail, ret = %d, errno = %d, set state to close", ret, errno);
            listenFd = -1;
            setClientState(CLIENT_CLOSED);
        }
    }
    */

    if (listenFd < 0) {
        RLOGD("Failed to get socket %s", socketName);
        setClientState(CLIENT_CLOSED);
        return;
    }

    RLOGD("start listen on fd: %d", listenFd);
    ret = listen(listenFd, 4);
    if (ret < 0) {
        RLOGD("Failed to listen on control socket '%d': %s",
             listenFd, strerror(errno));
        setClientState(CLIENT_CLOSED);
        return;
    }

    socklen_t socklen = sizeof (peer_addr);
    commandFd = accept (listenFd,  (sockaddr *) &peer_addr, &socklen);
    RLOGD("initialize: commandFd is %d", commandFd);

    if (commandFd < 0 ) {
        RLOGD("Error on accept() errno:%d", errno);
        setClientState(CLIENT_CLOSED);
        return;
    }
    RLOGD("set client state active");
    setClientState(CLIENT_ACTIVE);
}

#define MAX_COMMAND_BYTES (8 * 1024)
void RilClient::handleStateActive() {
    RecordStream *p_rs;
    void *p_record;
    size_t recordlen;
    int ret;

    p_rs = record_stream_new(commandFd, MAX_COMMAND_BYTES);
    RLOGD("command Fd active is %d", commandFd);
    while(clientState == CLIENT_ACTIVE) {
        /* loop until EAGAIN/EINTR, end of stream, or other error */
        ret = record_stream_get_next(p_rs, &p_record, &recordlen);

        if (ret == 0 && p_record == NULL) {
            /* end-of-stream */
            break;
        } else if (ret < 0) {
            break;
        } else if (ret == 0) { /* && p_record != NULL */
            processCommands(p_record, recordlen, identity);
        }
    }

    if (ret == 0 || !(errno == EAGAIN || errno == EINTR)) {
        /* fatal error or end-of-stream */
        if (ret != 0) {
            RLOGD("error on reading command socket errno:%d\n", errno);
        } else {
            RLOGD("EOS.  Closing command socket.");
        }
        record_stream_free(p_rs);
        setClientState(CLIENT_DEACTIVE);
    } else {
        RLOGD("Unhandled: ret: %d, errno: %d", ret, errno);
        record_stream_free(p_rs);
        setClientState(CLIENT_DEACTIVE);
    }
}

void RilClient::handleStateDeactive() {
    if (commandFd != -1) {
        RLOGD("clear Fd Command %d", commandFd);
        close(commandFd);
        commandFd = -1;
    } else {
        RLOGD("commandFd alread -1");
    }
    setClientState(CLIENT_INITIALIZING);
}

void RilClient::handleStateClosed() {
    if (commandFd != -1) {
        RLOGD("closed: clear Fd Command %d", commandFd);
        close(commandFd);
        commandFd = -1;
    } else {
        RLOGD("closed: commandFd alread -1");
    }
}

void RilClient::setClientState(RilClientState state) {
    if (clientState != state) {
        RLOGD("set client state %s with old state %s",
            clientStateToString(state), clientStateToString(clientState));
        clientState = state;
    } else {
        RLOGD("client state is already %s", clientStateToString(state));
    }
    activityThread = new StateActivityThread(this);
    activityThread -> run();
}

char* RilClient::clientStateToString(RilClientState state) {
    char* ret;
    switch(state) {
        case CLIENT_INITIALIZING:
            ret =  "CLIENT_INITIALIZING";
            break;
        case CLIENT_ACTIVE:
            ret =  "CLIENT_ACTIVE";
            break;
        case CLIENT_DEACTIVE:
            ret =  "CLIENT_DEACTIVE";
            break;
        case CLIENT_CLOSED:
            ret = "CLIENT_CLOSED";
            break;
        default:
            ret = "NO_SUCH_STATE";
            break;
    }
    return ret;
}

RilClient::StateActivityThread::StateActivityThread (RilClient* client){
    this -> client = client;
    RLOGD("Consctruct Activity thread");
}

RilClient::StateActivityThread::~StateActivityThread() {
    RLOGD("Desctruct Activity thread");
}

bool RilClient::StateActivityThread::threadLoop() {
    client -> clientStateCallback();
    return false;
}


void RilClient::processCommands(void *buffer, size_t buflen, int clientId) {
    Parcel* p =  new Parcel();
    status_t status;
    int32_t request;
    int32_t token;

    p -> setData((uint8_t *) buffer, buflen);
    status = p -> readInt32(&request);
    status = p -> readInt32 (&token);
    p->setDataPosition(0);

    //TODO: get 3g capacity protocol
    RLOGD("enqueue request id %d with token %d for client %d", request, token, clientId);

    int unsolResponseIndex;
    int ret;
    char prop_value[PROPERTY_VALUE_MAX] = {0};
    property_get(PROPERTY_3G_SIM, prop_value, "1");
    int capabilitySim = atoi(prop_value) - 1;
    RLOGD("capabilitySim = %d", capabilitySim);
    RILD_RadioTechnology_Group dest = RfxDefaultDestUtils::getDefaultDest(request);

    if (dest == RADIO_TECH_GROUP_C2K) {
        int slotId = RpRilClientController::getCdmaSlotId();
        RLOGD("Default destination is cdma, enqueue to slot %d", slotId);
        rfx_enqueue_client_request_message(request, slotId, dest, token, p, clientId);
    } else {
        RLOGD("Default destination is gsm, enqueue to slot %d", capabilitySim);
        rfx_enqueue_client_request_message(request, capabilitySim, dest, token, p, clientId);
    }
}

void RilClient::handleUnsolicited(int slotId, int unsolResponse, const void *data,
                                size_t datalen)
{

    int ret;
    char prop_value[PROPERTY_VALUE_MAX] = {0};
    property_get(PROPERTY_3G_SIM, prop_value, "1");
    int capabilitySim = atoi(prop_value) - 1;
    RLOGD("capabilitySim = %d", capabilitySim);

    if(capabilitySim != slotId) {
        RLOGD("only handle capabilitySim");
        return;
    }


    if (commandFd == -1) {
        RLOGD("command Fd not ready here");
        return;
    }

    Parcel p;

    p.writeInt32 (RESPONSE_UNSOLICITED);
    p.writeInt32 (unsolResponse);
    ret = p.write(data, datalen);

    if (ret != 0) {
        RLOGD("ret = %d, just return", ret);
    }

    RpRilClientController::sendResponse(p, commandFd);
    return;
}
