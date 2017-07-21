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

#include <telephony/mtk_ril.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <alloca.h>
#include "atchannels.h"
#include "at_tok.h"
#include "misc.h"
#include <getopt.h>
#include <sys/socket.h>
#include <cutils/sockets.h>
#include <termios.h>

#include <ril_callbacks.h>

#ifdef MTK_RIL_MD1
#define LOG_TAG "RIL"
#else
#define LOG_TAG "RILMD2"
#endif

#include <utils/Log.h>

#include <cutils/properties.h>

#include <hardware/ril/librilutils/proto/sap-api.pb.h>

#define PROPERTY_RIL_SIM_READY  "ril.ready.sim"

static void onRequest (int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID socket_id);
static RIL_RadioState currentState(RIL_SOCKET_ID rid);
static int onSupports (int requestCode);
static void onCancel (RIL_Token t);
static const char *getVersion();

#if defined(ANDROID_MULTI_SIM)
static void onSapRequest(int request, void *data, size_t datalen, RIL_Token t,
        RIL_SOCKET_ID socket_id);
#else
static void onSapRequest(int request, void *data, size_t datalen, RIL_Token t);
#endif

extern const char * requestToString(int request);
extern void initRILChannels(void);
/*** Static Variables ***/
static const RIL_RadioFunctionsSocket s_callbacks = {
    RIL_VERSION,
    onRequest,
    currentState,
    onSupports,
    onCancel,
    getVersion
};

#ifdef RIL_SHLIB
const struct RIL_EnvSocket *s_rilenv;
#endif  /* RIL_SHLIB */

const struct RIL_EnvSocket *s_rilsapenv = NULL;
static const RIL_RadioFunctionsSocket s_sapcallbacks = {
    RIL_VERSION,
    onSapRequest,
    currentState,
    onSupports,
    onCancel,
    getVersion
};

static RIL_RadioState sState = RADIO_STATE_UNAVAILABLE;
static RIL_RadioState sState2 = RADIO_STATE_UNAVAILABLE;
static RIL_RadioState sState3 = RADIO_STATE_UNAVAILABLE;
static RIL_RadioState sState4 = RADIO_STATE_UNAVAILABLE;

static pthread_mutex_t s_state_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t s_state_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t s_switch_mutex = PTHREAD_MUTEX_INITIALIZER;

static int s_port = -1;
static const char * s_device_path = NULL;
static int          s_device_socket = 0;

//[Emu]TODO
int          is_gemini_emulator = 0;

static int s_device_range_begin = -1;
static int s_device_range_end = -1;

/* trigger change to this with s_state_cond */
static int s_closed = 0;

#ifdef MTK_RIL
static const RIL_SOCKET_ID s_pollSimId = RIL_SOCKET_1;
static const RIL_SOCKET_ID s_pollSimId2 = RIL_SOCKET_2;
static const RIL_SOCKET_ID s_pollSimId3 = RIL_SOCKET_3;
static const RIL_SOCKET_ID s_pollSimId4 = RIL_SOCKET_4;
#endif /* MTK_RIL */

static const struct timeval TIMEVAL_0 = {2,0};

/* Commands pending in AT channel */
static pthread_mutex_t s_pendinglist_mutex = PTHREAD_MUTEX_INITIALIZER;
static int pendinglist[RIL_SUPPORT_CHANNELS] = {0};

/* To check if new SS service class feature is supported or not */
int bNewSSServiceClassFeature = 0;

/* logic modem id
 **/
char s_logicalModemId[SIM_COUNT][MAX_UUID_LENGTH];

/* capability switch verion
 * value : 1 for legacy sim switch
 *         2 for no MD reset sim switch
 **/
int s_simSwitchVersion = 1;

/* check if under sim switching
 * value : 0 for idle state
 *         1 for under sim switching
 **/
int s_isSimSwitching = 0;

/* modem capability
 * value(bit mask) : RAF_GSM, RAF_UMTS, RAF_LTE...etc
 **/
int s_modemCapability[RIL_SOCKET_NUM] = {0};

/*
 * If found the request in pendinglist, return list index else retrun RIL_SUPPORT_CHANNELS
*/
static int findPendingRequest(int request)
{
    int i;

    for (i = 0; i < getSupportChannels(); i++)
    {
        if (*(pendinglist+i) == request)
        {
            return i;
        }
    }

    return i;
}

static void setRequest(int request)
{
    pthread_mutex_lock(&s_pendinglist_mutex);
    /* find an empty slot */
    pendinglist[findPendingRequest(0)] = request;
    assert(i < getSupportChannels());
    pthread_mutex_unlock(&s_pendinglist_mutex);
}

static void resetRequest(int request)
{
    pthread_mutex_lock(&s_pendinglist_mutex);
    pendinglist[findPendingRequest(request)] = 0;
    assert(i < getSupportChannels());
    pthread_mutex_unlock(&s_pendinglist_mutex);
}

extern int RILcheckPendingRequest(int request)
{
    return (getSupportChannels()== findPendingRequest(request)) ? 0 : 1;
}

/** do post-AT+CFUN=1 initialization */
static void onRadioPowerOn(RIL_SOCKET_ID rid)
{
    const RIL_SOCKET_ID * p_rilId = &s_pollSimId;

    if (RIL_SOCKET_4 == rid) {
        p_rilId = &s_pollSimId4;
    } else if (RIL_SOCKET_3 == rid) {
        p_rilId = &s_pollSimId3;
    } else if (RIL_SOCKET_2 == rid) {
        p_rilId = &s_pollSimId2;
    }

    pollSIMState((void *)p_rilId);
}

/** do post- SIM ready initialization */
static void onSIMReady(RIL_SOCKET_ID rid)
{
}

/*** Callback methods from the RIL library to us ***/

/**
 * Call from RIL to us to make a RIL_REQUEST
 *
 * Must be completed with a call to RIL_onRequestComplete()
 *
 * RIL_onRequestComplete() may be called from any thread, before or after
 * this function returns.
 *
 * Will always be called from the same thread, so returning here implies
 * that the radio is ready to process another command (whether or not
 * the previous command has completed).
 */
static void onRequest(int request, void *data, size_t datalen, RIL_Token t, RIL_SOCKET_ID socket_id)
{
    RIL_RadioState radioState = sState;

    RLOGD("onRequest: %s, datalen = %d", requestToString(request), datalen);

    if (RIL_CHANNEL_SET4_OFFSET <= RIL_queryMyChannelId(t)) {
        radioState = sState4;
    } else if (RIL_CHANNEL_SET3_OFFSET <= RIL_queryMyChannelId(t)) {
        radioState = sState3;
    } else if (RIL_CHANNEL_OFFSET <= RIL_queryMyChannelId(t)) {
        radioState = sState2;
    }

    /* Ignore all requests except RIL_REQUEST_GET_SIM_STATUS
     * when RADIO_STATE_UNAVAILABLE.
     */
    if (radioState == RADIO_STATE_UNAVAILABLE &&
            request != RIL_REQUEST_GET_SIM_STATUS
    ) {
        RIL_onRequestComplete(t, RIL_E_RADIO_NOT_AVAILABLE, NULL, 0);
        return;
    }

    /* Ignore all non-power requests when RADIO_STATE_OFF
     * (except RIL_REQUEST_GET_SIM_STATUS)
     */
    if (radioState == RADIO_STATE_OFF &&
            !(request == RIL_REQUEST_RADIO_POWER ||
              request == RIL_REQUEST_MODEM_POWERON ||
              request == RIL_REQUEST_DUAL_SIM_MODE_SWITCH ||
              request == RIL_REQUEST_SET_GPRS_CONNECT_TYPE ||
              request == RIL_REQUEST_SET_GPRS_TRANSFER_TYPE ||
              request == RIL_REQUEST_GET_SIM_STATUS ||
              request == RIL_REQUEST_GET_IMEI ||
              request == RIL_REQUEST_GET_IMEISV ||
              request == RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE ||
              request == RIL_REQUEST_BASEBAND_VERSION ||
              request == RIL_REQUEST_QUERY_AVAILABLE_BAND_MODE ||
              request == RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE ||
              request == RIL_REQUEST_SET_LOCATION_UPDATES ||
              request == RIL_REQUEST_WRITE_SMS_TO_SIM ||
              request == RIL_REQUEST_DELETE_SMS_ON_SIM ||
              request == RIL_REQUEST_GSM_GET_BROADCAST_SMS_CONFIG ||
              request == RIL_REQUEST_GSM_SET_BROADCAST_SMS_CONFIG ||
              request == RIL_REQUEST_GSM_SMS_BROADCAST_ACTIVATION ||
              request == RIL_REQUEST_GET_SMSC_ADDRESS ||
              request == RIL_REQUEST_SET_SMSC_ADDRESS ||
              request == RIL_REQUEST_REPORT_SMS_MEMORY_STATUS ||
              request == RIL_REQUEST_SCREEN_STATE ||
              request == RIL_REQUEST_RESET_RADIO ||
              request == RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE ||
              request == RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND ||
              request == RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM ||
              request == RIL_REQUEST_SET_TTY_MODE ||
              request == RIL_REQUEST_MOBILEREVISION_AND_IMEI ||
              request == RIL_REQUEST_GET_SMS_SIM_MEM_STATUS ||
              request == RIL_REQUEST_QUERY_PHB_STORAGE_INFO ||
              request == RIL_REQUEST_WRITE_PHB_ENTRY ||
              request == RIL_REQUEST_READ_PHB_ENTRY
    )) {
        RIL_onRequestComplete(t, RIL_E_RADIO_NOT_AVAILABLE, NULL, 0);
        return;
    }

    /* set pending RIL request */
    setRequest(request);

    if (!(rilSimMain(request, data, datalen, t) ||
        rilNwMain(request, data, datalen, t) ||
        rilCcMain(request, data, datalen, t) ||
        rilSsMain(request, data, datalen, t) ||
        rilSmsMain(request, data, datalen, t) ||
        rilStkMain(request, data, datalen, t) ||
        rilOemMain(request, data, datalen, t) ||
        rilDataMain(request, data, datalen, t))) {
        RIL_onRequestComplete(t, RIL_E_REQUEST_NOT_SUPPORTED, NULL, 0);
    }

    /* Reset pending RIL request */
    resetRequest(request);
}

#if defined(ANDROID_MULTI_SIM)
static void onSapRequest(int request, void *data, size_t datalen, RIL_Token t,
        RIL_SOCKET_ID socket_id) {
#else
static void onSapRequest(int request, void *data, size_t datalen, RIL_Token t) {
#endif
    RLOGD("onSapRequest: %d", request);

    RIL_SOCKET_ID socketId = RIL_SOCKET_1;
    #if defined(ANDROID_MULTI_SIM)
    socketId = socket_id;
    #endif

    if (request < MsgId_RIL_SIM_SAP_CONNECT /* MsgId_UNKNOWN_REQ */ ||
            request > MsgId_RIL_SIM_SAP_SET_TRANSFER_PROTOCOL) {
        RLOGD("invalid request");
        RIL_SIM_SAP_ERROR_RSP rsp;
        rsp.dummy_field = 1;
        sendSapResponseComplete(t, Error_RIL_E_REQUEST_NOT_SUPPORTED, MsgId_RIL_SIM_SAP_ERROR_RESP
                , &rsp);
        return;
    }

    if (s_md_off) {
        RLOGD("MD off and reply failure to Sap message");
        RIL_SIM_SAP_CONNECT_RSP rsp;
        rsp.response = RIL_SIM_SAP_CONNECT_RSP_Response_RIL_E_SAP_CONNECT_FAILURE;
        rsp.has_max_message_size = false;
        rsp.max_message_size = 0;
        sendSapResponseComplete(t, Error_RIL_E_RADIO_NOT_AVAILABLE, MsgId_RIL_SIM_SAP_CONNECT,
                &rsp);
        return;
    }

    /* set pending RIL request */
    setRequest(request);

    if (!(rilBtSapMain(request, data, datalen, t, socketId) ||
            rilStkBtSapMain(request, data, datalen, t, socketId))) {
        RIL_SIM_SAP_ERROR_RSP rsp;
        rsp.dummy_field = 1;
        sendSapResponseComplete(t, Error_RIL_E_REQUEST_NOT_SUPPORTED, MsgId_RIL_SIM_SAP_ERROR_RESP
                , &rsp);
    }

    /* Reset pending RIL request */
    resetRequest(request);
}

/**
 * Synchronous call from the RIL to us to return current radio state.
 * RADIO_STATE_UNAVAILABLE should be the initial state.
 */
static RIL_RadioState currentState(RIL_SOCKET_ID rid)
{
    return getRadioState(rid);
}

/**
 * Call from RIL to us to find out whether a specific request code
 * is supported by this implementation.
 *
 * Return 1 for "supported" and 0 for "unsupported"
 */
static int onSupports(int requestCode)
{
    //@@@ todo
    return 1;
}

static void onCancel (RIL_Token t)
{
    //@@@todo
}

static const char * getVersion(void)
{
    switch (SIM_COUNT) {
        case 2:
        return "mtk gemini ril 1.0";
        case 3:
            return "mtk gemini+ 3 SIM ril 1.0";
        case 4:
            return "mtk gemini+ 4 SIM ril 1.0";
        case 1:
        default:
        return "mtk ril w10.20";
    }
}


/**
 * Initialize everything that can be configured while we're still in
 * AT+CFUN=0
 */
#ifdef MTK_RIL
static void initializeCallback(void *param)
{
    RLOGE ("[Emu]get in initializeCallback");
    ATResponse *p_response = NULL;
    int err;
    RIL_SOCKET_ID rid = *((RIL_SOCKET_ID *)param);

    char property_value[5] = { 0 };
    int current_share_modem = 0;

    if (SIM_COUNT >= 2) {
        current_share_modem = 2;
    } else {
        current_share_modem = 1;
    }

    setRadioState (RADIO_STATE_OFF,rid);

    err = at_handshake(getDefaultChannelCtx(rid));

    RLOGI("AT handshake: %d",err);

    /* note: we don't check errors here. Everything important will
     * be handled in onATTimeout and onATReaderClosed */

    /*  atchannel is tolerant of echo but it must */
    /*  have verbose result codes */
    at_send_command("ATE0Q0V1", NULL, getDefaultChannelCtx(rid));

    /*  No auto-answer */
    at_send_command("ATS0=0", NULL,getDefaultChannelCtx(rid));

    /*  Extended errors */
    at_send_command("AT+CMEE=1", NULL, getDefaultChannelCtx(rid));

    /*  Network registration events */
    err = at_send_command("AT+CREG=2", &p_response, getDefaultChannelCtx(rid));

    /* some handsets -- in tethered mode -- don't support CREG=2 */
    if (err < 0 || p_response->success == 0) {
        at_send_command("AT+CREG=1", NULL, getDefaultChannelCtx(rid));
    }

    at_response_free(p_response);

    /*  GPRS registration events */
    at_send_command("AT+CGREG=1", NULL, getDefaultChannelCtx(rid));

    /*  Call Waiting notifications */
    at_send_command("AT+CCWA=1", NULL, getDefaultChannelCtx(rid));
//[Emu]TODO diff
    /*  mtk00924: enable Call Progress notifications */
    at_send_command("AT+ECPI=4294967295", NULL, getDefaultChannelCtx(rid));

    /*  Alternating voice/data off */
    /*
     * at_send_command("AT+CMOD=0", NULL, getDefaultChannelCtx(rid));
    */

    /*  Not muted */
    /*
     * at_send_command("AT+CMUT=0", NULL, getDefaultChannelCtx(rid));
    */

//[Emu]TODO diff
    /*  +CSSU unsolicited supp service notifications */
    at_send_command("AT+CSSN=1,1", NULL, getDefaultChannelCtx(rid));
//[Emu]TODO diff
    /*  connected line identification on */
    at_send_command("AT+COLP=1", NULL, getDefaultChannelCtx(rid));
//[Emu]TODO diff
    /*  HEX character set */
    at_send_command("AT+CSCS=\"UCS2\"", NULL, getDefaultChannelCtx(rid));

    /*  USSD unsolicited */
    at_send_command("AT+CUSD=1", NULL, getDefaultChannelCtx(rid));

    /*  Enable +CGEV GPRS event notifications, but don't buffer */
    at_send_command("AT+CGEREP=1,0", NULL, getDefaultChannelCtx(rid));

    /*  SMS PDU mode */
    at_send_command("AT+CMGF=0", NULL, getDefaultChannelCtx(rid));

    /* Initial CID table */
    initialCidTable();

//[Emu]TODO diff
    /* Enable getting NITZ, include TZ and Operator Name*/
    /* To Receive +CIEV: 9 and +CIEV: 10*/
//[Emu]TODO_1_ changed
  //  at_send_command("AT+CTZR=1", NULL, getDefaultChannelCtx(rid));
//[Emu]TODO diff
    /*  Enable getting CFU info +ECFU and speech info +ESPEECH*/
  //  at_send_command("AT+EINFO=114", NULL, getDefaultChannelCtx(rid));
//[Emu]TODO diff
    /*  Enable get ECSQ URC */
  //  at_send_command("AT+ECSQ=2", NULL, getDefaultChannelCtx(rid));
//[Emu]TODO diff
    /*  Enable get +CIEV:7 URC to receive SMS SIM Storage Status*/
  //  at_send_command("AT+CMER=1,0,0,2,0", NULL, getDefaultChannelCtx(rid));
//[Emu]TODO diff

/// M: For 3G VT only @{
#ifdef MTK_VT3G324M_SUPPORT
    at_send_command("AT+CRC=1", NULL, getDefaultChannelCtx(rid));
#endif
/// @}

    if(SIM_COUNT >= 2) {
        requestSimReset(rid);

        RLOGD("start rild bootup flow [%d, %d, %d, %d]", isDualTalkMode(), rid, RIL_is3GSwitched(), current_share_modem);
        if (isDualTalkMode()) {
            if (rid == RIL_SOCKET_1) {
                flightModeBoot();
                bootupGetIccid(rid); //query ICCID after AT+ESIMS
                bootupGetImei(rid);
                bootupGetImeisv(rid);
                bootupGetBasebandVersion(rid);
                bootupGetCalData(rid);
                RLOGD("get SIM inserted status (DT) [%d]", sim_inserted_status);
            }
        } else {
            if (current_share_modem == 1) {
                if (rid == RIL_SOCKET_1) {
                    flightModeBoot();
                    bootupGetIccid(rid); //query ICCID after AT+ESIMS
                    bootupGetImei(rid);
                    bootupGetImeisv(rid);
                    bootupGetBasebandVersion(rid);
                    bootupGetCalData(rid);
                    RLOGD("get SIM inserted status (Single) [%d]", sim_inserted_status);
                }
            } else if (rid == RIL_SOCKET_2) {
                flightModeBoot();
                bootupGetIccid(RIL_SOCKET_1);
                bootupGetIccid(RIL_SOCKET_2);
                bootupGetImei(RIL_SOCKET_1);
                bootupGetImei(RIL_SOCKET_2);
                bootupGetImeisv(RIL_SOCKET_1);
                bootupGetImeisv(RIL_SOCKET_2);
                bootupGetBasebandVersion(RIL_SOCKET_1);
                bootupGetBasebandVersion(RIL_SOCKET_2);
                bootupGetCalData(RIL_SOCKET_1);
                RLOGD("get SIM inserted status [%d]", sim_inserted_status);
            }
        }
    } else {
        flightModeBoot();
        bootupGetIccid(rid);
        bootupGetImei(rid);
        bootupGetImeisv(rid);
        bootupGetBasebandVersion(rid);
        bootupGetCalData(rid);
    }

//[Emu]TODO diff
    /* assume radio is off on error */
    if (isRadioOn(rid) > 0) {
        setRadioState (RADIO_STATE_ON, rid);
    }
}
#else   /* MTK_RIL */
static void initializeCallback(void *param)
{
    ATResponse *p_response = NULL;
    int err;

    setRadioState (RADIO_STATE_OFF);

    at_handshake();

    /* note: we don't check errors here. Everything important will
     * be handled in onATTimeout and onATReaderClosed */

    /*  atchannel is tolerant of echo but it must */
    /*  have verbose result codes */
    at_send_command("ATE0Q0V1", NULL);

    /*  No auto-answer */
    at_send_command("ATS0=0", NULL);

    /*  Extended errors */
    at_send_command("AT+CMEE=1", NULL);

    /*  Network registration events */
    err = at_send_command("AT+CREG=2", &p_response);

    /* some handsets -- in tethered mode -- don't support CREG=2 */
    if (err < 0 || p_response->success == 0)
        at_send_command("AT+CREG=1", NULL);

    at_response_free(p_response);

    /*  GPRS registration events */
    at_send_command("AT+CGREG=1", NULL);

    /*  Call Waiting notifications */
    at_send_command("AT+CCWA=1", NULL);

    /*  Alternating voice/data off */
    at_send_command("AT+CMOD=0", NULL);

    /*  Not muted */
    at_send_command("AT+CMUT=0", NULL);

    /*  +CSSU unsolicited supp service notifications */
    at_send_command("AT+CSSN=0,1", NULL);

    /*  no connected line identification */
    at_send_command("AT+COLP=0", NULL);

    /*  HEX character set */
    at_send_command("AT+CSCS=\"HEX\"", NULL);

    /*  USSD unsolicited */
    at_send_command("AT+CUSD=1", NULL);

    /*  Enable +CGEV GPRS event notifications, but don't buffer */
    at_send_command("AT+CGEREP=1,0", NULL);

    /*  SMS PDU mode */
    at_send_command("AT+CMGF=0", NULL);

#ifdef USE_TI_COMMANDS
    at_send_command("AT%CPI=3", NULL);

    /*  TI specific -- notifications when SMS is ready (currently ignored) */
    at_send_command("AT%CSTAT=1", NULL);
#endif /* USE_TI_COMMANDS */

    /* assume radio is off on error */
    if (isRadioOn() > 0) {
        setRadioState (RADIO_STATE_ON);
    }
}
#endif  /* MTK_RIL */

static void waitForClose()
{
RLOGI("[Emu]waitForClose in");
    pthread_mutex_lock(&s_state_mutex);

    while (s_closed == 0) {
        RLOGI("[Emu]waitForClose while in");
        pthread_cond_wait(&s_state_cond, &s_state_mutex);
        RLOGI("[Emu]waitForClose while out");
    }

    pthread_mutex_unlock(&s_state_mutex);
    RLOGI("[Emu]waitForClose out");
}


/**
 * Called by atchannel when an unsolicited line appears
 * This is called on atchannel's reader thread. AT commands may
 * not be issued here
 */
static void onUnsolicited (const char *s, const char *sms_pdu, void * pChannel)
{
    RLOGI("[Emu]get in onUnsolicited");
    RLOGI("[Emu]command %s",s);
    char *line = NULL;
    int err;
    RIL_RadioState radioState = sState;
    RILChannelCtx * p_channel = (RILChannelCtx *) pChannel;

    if (RIL_SOCKET_2 == getRILIdByChannelCtx(p_channel)) {
        radioState = sState2;
        RLOGI("[Emu]state2");
    }

    /* Ignore unsolicited responses until we're initialized.
     * This is OK because the RIL library will poll for initial state
     */
    if (radioState == RADIO_STATE_UNAVAILABLE) {
        return;
    }

    if (!(rilNwUnsolicited(s,sms_pdu,p_channel) ||
          rilCcUnsolicited(s,sms_pdu, p_channel) ||
          rilSsUnsolicited(s,sms_pdu, p_channel) ||
          rilSmsUnsolicited(s,sms_pdu, p_channel) ||
          rilStkUnsolicited(s,sms_pdu, p_channel) ||
          rilOemUnsolicited(s,sms_pdu, p_channel) ||
          rilDataUnsolicited(s,sms_pdu, p_channel)||
          rilSimUnsolicited(s,sms_pdu, p_channel)))
    {
        RLOGE("Unhandled unsolicited result code: %s\n", s);

    }
}

#ifdef MTK_RIL
/* Called on command or reader thread */
static void onATReaderClosed(RILChannelCtx *p_channel)
{
    RLOGI("AT channel closed\n");
    at_close(p_channel);
    assert(0);
    s_closed = 1;

    setRadioState (RADIO_STATE_UNAVAILABLE, getRILIdByChannelCtx(p_channel));
}

/* Called on command thread */
static void onATTimeout(RILChannelCtx *p_channel)
{
    RLOGI("AT channel timeout; closing\n");
    at_close(p_channel);
    assert(0);
    s_closed = 1;

    /* FIXME cause a radio reset here */

    setRadioState (RADIO_STATE_UNAVAILABLE, getRILIdByChannelCtx(p_channel));
}
#else   /* MTK_RIL */
/* Called on command or reader thread */
static void onATReaderClosed()
{
    RLOGI("AT channel closed\n");
    at_close();
    s_closed = 1;

    setRadioState (RADIO_STATE_UNAVAILABLE);
}

/* Called on command thread */
static void onATTimeout()
{
    RLOGI("AT channel timeout; closing\n");
    at_close();

    s_closed = 1;

    /* FIXME cause a radio reset here */

    setRadioState (RADIO_STATE_UNAVAILABLE);
}
#endif /* MTK_RIL */

static void usage(char *s)
{
#ifdef RIL_SHLIB
    fprintf(stderr, "reference-ril requires: -p <tcp port> or -d /dev/tty_device\n");
#else   /* RIL_SHLIB */
    fprintf(stderr, "usage: %s [-p <tcp port>] [-d /dev/tty_device]\n", s);
    exit(-1);
#endif  /* RIL_SHLIB */
}


#ifdef MTK_RIL
/* These nodes are created by gsm0710muxd */
char * s_mux_path[RIL_SUPPORT_CHANNELS] =
{
    "/dev/pttynoti",
    "/dev/pttycmd1",
    "/dev/pttycmd2",
    "/dev/pttycmd3"

    ,"/dev/ptty2noti"
    ,"/dev/ptty2cmd1"
    ,"/dev/ptty2cmd2"
    ,"/dev/ptty2cmd3"
};
static void emulator_gemini_opensocket()
{
    int fd;
            int ret;
        for (;;) {
                fd = -1;
            RLOGI("[Emu]emulator_gemini_opensocket_in\n");

            RLOGI("[Emu]s_device_socket %d\n",s_device_socket);
            RLOGI("[Emu]s_port %d\n",s_port);
             while  (fd < 0) {
                        if (s_port > 0) {
                            fd = socket_loopback_client(s_port, SOCK_STREAM);
                            RLOGI("[Emu]fd1 %d\n",fd);
                        } else if (s_device_socket) {
                                if (!strcmp(s_device_path, "/dev/socket/qemud")) {
                                    /* Qemu-specific control socket */
                                        fd = socket_local_client( "qemud",
                                                  ANDROID_SOCKET_NAMESPACE_RESERVED,
                                                  SOCK_STREAM );
                                RLOGI("[Emu]fd2 %d\n",fd);
                                    if (fd >= 0 ) {
                                             char  answer[2];

                                        if ( write(fd, "gsm", 3) != 3 ||
                                             read(fd, answer, 2) != 2 ||
                                             memcmp(answer, "OK", 2) != 0)
                                        {
                                            close(fd);
                                            fd = -1;
                                        }
                                     }
                                }
                               else {
                          fd = socket_local_client( s_device_path,
                                            ANDROID_SOCKET_NAMESPACE_FILESYSTEM,
                                            SOCK_STREAM );
                        }
                               RLOGI("[Emu]fd3 %d\n",fd);

                            } else if (s_device_path != NULL) {
                                fd = open (s_device_path, O_RDWR);
                                if ( fd >= 0 && !memcmp( s_device_path, "/dev/ttyS", 9 ) ) {
                                     /* disable echo on serial ports */
                                    struct termios  ios;
                                    tcgetattr( fd, &ios );
                                    ios.c_lflag = 0;  /* disable ECHO, ICANON, etc... */
                              ios.c_iflag = 0;
                                    tcsetattr( fd, TCSANOW, &ios );
                                }
                             RLOGI("[Emu]fd4 %d\n",fd);
                        }

                        if (fd < 0) {
                             RLOGI("[Emu]fd<0");
                            perror ("opening AT interface. retrying...");
                            sleep(10);
                            /* never returns */
                        }
                }

                s_closed = 0;
                ret = at_open_emulator(fd, onUnsolicited,is_gemini_emulator);

                if (ret < 0) {
                    RLOGE ("AT error %d on at_open\n", ret);
                    return 0;
                }
            RLOGI("[Emu]RIL_requestTimedCallback");
            RIL_requestTimedCallback(initializeCallback, &s_pollSimId, &TIMEVAL_0);
            if (SIM_COUNT >= 2) {
                RIL_requestTimedCallback(initializeCallback, &s_pollSimId2, &TIMEVAL_0);
            }
            RLOGI("[Emu]RIL_requestTimedCallback out");
            // Give initializeCallback a chance to dispatched, since
            // we don't presently have a cancellation mechanism
            sleep(1);

            waitForClose();
            RLOGI("Re-opening after close");

        }
}

void sendSetPhoneRatUrc(int sim3G)
{
    int urcData[2] = {0};
    urcData[0] = RAF_GSM;

    RLOGI("sim switch done, send URC, sim3G %d", sim3G);

    switch(sim3G) {
        case CAPABILITY_3G_SIM2:
            RIL_onUnsolicitedResponseSocket(RIL_UNSOL_RADIO_CAPABILITY, urcData, sizeof(int)*2, RIL_SOCKET_1);
            if (SIM_COUNT >= 3) {
                RIL_onUnsolicitedResponseSocket(RIL_UNSOL_RADIO_CAPABILITY, urcData, sizeof(int)*2, RIL_SOCKET_3);
            }
            if (SIM_COUNT >= 4) {
                RIL_onUnsolicitedResponseSocket(RIL_UNSOL_RADIO_CAPABILITY, urcData, sizeof(int)*2, RIL_SOCKET_4);
            }
            if(isLteSupport()) {
                urcData[0] = RAF_GSM | RAF_UMTS | RAF_LTE;
            } else {
                urcData[0] = RAF_GSM | RAF_UMTS;
            }
            RIL_onUnsolicitedResponseSocket(RIL_UNSOL_RADIO_CAPABILITY, urcData, sizeof(int)*2, RIL_SOCKET_2);
        break;

        case CAPABILITY_3G_SIM3:
            RIL_onUnsolicitedResponseSocket(RIL_UNSOL_RADIO_CAPABILITY, urcData, sizeof(int)*2, RIL_SOCKET_1);

            RIL_onUnsolicitedResponseSocket(RIL_UNSOL_RADIO_CAPABILITY, urcData, sizeof(int)*2, RIL_SOCKET_2);
            if (SIM_COUNT >= 4) {
                RIL_onUnsolicitedResponseSocket(RIL_UNSOL_RADIO_CAPABILITY, urcData, sizeof(int)*2, RIL_SOCKET_4);
            }
            if(isLteSupport()) {
                urcData[0] = RAF_GSM | RAF_UMTS | RAF_LTE;
            } else {
                urcData[0] = RAF_GSM | RAF_UMTS;
            }
            RIL_onUnsolicitedResponseSocket(RIL_UNSOL_RADIO_CAPABILITY, urcData, sizeof(int)*2, RIL_SOCKET_3);
        break;

        case CAPABILITY_3G_SIM4:
            RIL_onUnsolicitedResponseSocket(RIL_UNSOL_RADIO_CAPABILITY, urcData, sizeof(int)*2, RIL_SOCKET_1);
            RIL_onUnsolicitedResponseSocket(RIL_UNSOL_RADIO_CAPABILITY, urcData, sizeof(int)*2, RIL_SOCKET_2);
            RIL_onUnsolicitedResponseSocket(RIL_UNSOL_RADIO_CAPABILITY, urcData, sizeof(int)*2, RIL_SOCKET_3);
            if(isLteSupport()) {
                urcData[0] = RAF_GSM | RAF_UMTS | RAF_LTE;
            } else {
                urcData[0] = RAF_GSM | RAF_UMTS;
            }
            RIL_onUnsolicitedResponseSocket(RIL_UNSOL_RADIO_CAPABILITY, urcData, sizeof(int)*2, RIL_SOCKET_4);
        break;

        default:
        case CAPABILITY_3G_SIM1:
            if (SIM_COUNT >= 2) {
                RIL_onUnsolicitedResponseSocket(RIL_UNSOL_RADIO_CAPABILITY, urcData, sizeof(int)*2, RIL_SOCKET_2);
            }
            if (SIM_COUNT >= 3) {
                RIL_onUnsolicitedResponseSocket(RIL_UNSOL_RADIO_CAPABILITY, urcData, sizeof(int)*2, RIL_SOCKET_3);
            }
            if (SIM_COUNT >= 4) {
                RIL_onUnsolicitedResponseSocket(RIL_UNSOL_RADIO_CAPABILITY, urcData, sizeof(int)*2, RIL_SOCKET_4);
            }

            if(isLteSupport()) {
                urcData[0] = RAF_GSM | RAF_UMTS | RAF_LTE;
            } else {
                urcData[0] = RAF_GSM | RAF_UMTS;
            }
            RIL_onUnsolicitedResponseSocket(RIL_UNSOL_RADIO_CAPABILITY, urcData, sizeof(int)*2, RIL_SOCKET_1);
        break;
    }
}

static void *
mainLoop(void *param)
{
    AT_DUMP("== ", "entering mainLoop()", -1 );
    at_set_on_reader_closed(onATReaderClosed);
    at_set_on_timeout(onATTimeout);
    initRILChannels();
    RLOGI("[Emu]mainloop_in");
    RLOGI("[Emu]mainloop_in %d\n",s_device_socket);
    if(s_device_socket)
    {
        emulator_gemini_opensocket();
        return NULL;
    }
    else
        {
        int ret;
           int i;
            RILChannelCtx * p_channel;

            for (;;) {

                for (i=0; i < getSupportChannels(); i ++)
                {
                    p_channel = getChannelCtxbyId(i);

                    while (p_channel->fd < 0)
                    {
                        do {
                            p_channel->fd = open(s_mux_path[i], O_RDWR);
                        } while (p_channel->fd < 0 && errno == EINTR);

                        if (p_channel->fd < 0)
                        {
                            perror ("opening AT interface. retrying...");
                            RLOGE("could not connect to %s: %s",  s_mux_path[i], strerror(errno));
                            sleep(10);
                            /* never returns */
                        }
                        else
                        {
                            struct termios  ios;
                            tcgetattr(p_channel->fd, &ios );
                            ios.c_lflag = 0;  /* disable ECHO, ICANON, etc... */
                                ios.c_iflag = 0;
                            tcsetattr(p_channel->fd, TCSANOW, &ios );
                        }
                    }


                    s_closed = 0;
                    ret = at_open(p_channel->fd,onUnsolicited, p_channel);


                    if (ret < 0) {
                        RLOGE ("AT error %d on at_open\n", ret);
                        return 0;
                    }
                    }

                RIL_requestTimedCallback(initializeCallback, &s_pollSimId, &TIMEVAL_0);
                if (SIM_COUNT >= 2) {
                    RIL_requestTimedCallback(initializeCallback, &s_pollSimId2, &TIMEVAL_0);
                }

                // Give initializeCallback a chance to dispatched, since
                // we don't presently have a cancellation mechanism
                sleep(1);

                waitForClose();
                RLOGI("Re-opening after close");
        }
        }

}
#else   /* MTK_RIL */
static void *mainLoop(void *param)
{
    int fd;
    int ret;
    char path[50];
    int ttys_index;

    AT_DUMP("== ", "entering mainLoop()", -1 );
    at_set_on_reader_closed(onATReaderClosed);
    at_set_on_timeout(onATTimeout);

    for (;;) {
        fd = -1;
        while  (fd < 0) {
            if (s_port > 0) {
                fd = socket_loopback_client(s_port, SOCK_STREAM);
            } else if (s_device_socket) {
                if (!strcmp(s_device_path, "/dev/socket/qemud")) {
                    /* Qemu-specific control socket */
                    fd = socket_local_client("qemud",
                                              ANDROID_SOCKET_NAMESPACE_RESERVED,
                                              SOCK_STREAM );
                    if (fd >= 0 ) {
                        char  answer[2];

                        if (write(fd, "gsm", 3) != 3 ||
                            read(fd, answer, 2) != 2 ||
                            memcmp(answer, "OK", 2) != 0) {
                            close(fd);
                            fd = -1;
                        }
                   }
                } else {
                    fd = socket_local_client( s_device_path,
                                            ANDROID_SOCKET_NAMESPACE_FILESYSTEM,
                                            SOCK_STREAM );
                }
            } else if (s_device_path != NULL) {
                fd = open (s_device_path, O_RDWR);
                if ( fd >= 0 && !memcmp( s_device_path, "/dev/ttyS", 9 ) ) {
                    /* disable echo on serial ports */
                    struct termios  ios;
                    tcgetattr( fd, &ios );
                    ios.c_lflag = 0;  /* disable ECHO, ICANON, etc... */
                    ios.c_iflag = 0;
                    tcsetattr( fd, TCSANOW, &ios );
                }
            }


            if (fd < 0) {
                perror ("opening AT interface. retrying...");
                sleep(10);
                /* never returns */
            }
        }

        RLOGD("FD: %d", fd);

        s_closed = 0;
        ret = at_open(fd, onUnsolicited);

        if (ret < 0) {
            RLOGE ("AT error %d on at_open\n", ret);
            return 0;
        }

        RIL_requestTimedCallback(initializeCallback, NULL, &TIMEVAL_0);

        // Give initializeCallback a chance to dispatched, since
        // we don't presently have a cancellation mechanism
        sleep(1);

        waitForClose();
        RLOGI("Re-opening after close");
    }
}
#endif /* MTK_RIL */

#ifdef MTK_RIL
RIL_RadioState getRadioState(RIL_SOCKET_ID rid)
{
    RIL_RadioState radioState = sState;

    if (RIL_SOCKET_2 == rid) {
        radioState = sState2;
    }
    return radioState;
}
#else   /* MTK_RIL */
RIL_RadioState getRadioState(void)
{
    return sState;
}
#endif /* MTK_RIL */


#ifdef MTK_RIL
void setRadioState(RIL_RadioState newState, RIL_SOCKET_ID rid)
#else
void setRadioState(RIL_RadioState newState)
#endif /* MTK_RIL */
{
    RIL_RadioState oldState;
    RIL_RadioState *pState = NULL;

    pthread_mutex_lock(&s_state_mutex);

    oldState = sState;
    pState = &sState;
    if (RIL_SOCKET_2 == rid) {
        oldState = sState2;
        pState = &sState2;
    }

    if (s_closed > 0) {
        // If we're closed, the only reasonable state is
        // RADIO_STATE_UNAVAILABLE
        // This is here because things on the main thread
        // may attempt to change the radio state after the closed
        // event happened in another thread
        assert(0);
        newState = RADIO_STATE_UNAVAILABLE;
    }

    if (*pState != newState || s_closed > 0) {
        *pState = newState;
        assert(0);
        pthread_cond_broadcast (&s_state_cond);
    }

    pthread_mutex_unlock(&s_state_mutex);


    /* do these outside of the mutex */
    if (*pState != oldState) {
        RIL_onUnsolicitedResponseSocket(RIL_UNSOL_RESPONSE_RADIO_STATE_CHANGED,
                                    NULL, 0, rid);

        /* FIXME onSimReady() and onRadioPowerOn() cannot be called
         * from the AT reader thread
         * Currently, this doesn't happen, but if that changes then these
         * will need to be dispatched on the request thread
         */
        if (*pState == RADIO_STATE_SIM_READY) {
            onSIMReady(rid);
        } else if (*pState == RADIO_STATE_SIM_NOT_READY) {
            onRadioPowerOn(rid);
        }

        /* the following property is used for auto test*/
        if (*pState == RADIO_STATE_SIM_READY)
        {
            property_set((const char *)PROPERTY_RIL_SIM_READY, (const char *)"true");
        }
        else
        {
            property_set((const char *)PROPERTY_RIL_SIM_READY, (const char *)"false");
        }
    }
}


#ifdef RIL_SHLIB
pthread_t s_tid_mainloop;

const RIL_RadioFunctionsSocket *RIL_InitSocket(const struct RIL_EnvSocket *env, int argc, char **argv)
{
    int ret;
    int fd = -1;
    int opt;
    int index;
    char *tmp;
    pthread_attr_t attr;
    char path[50];
    int ttys_index, i;

    s_rilenv = env;

    while (-1 != (opt = getopt(argc, argv, "p:d:s:c:m:"))) {
        switch (opt) {
            case 'p':
                s_port = atoi(optarg);
                RLOGI("s_port %s\n", s_port);
                if (s_port == 0) {
                    usage(argv[0]);
                    return NULL;
                }
                RLOGI("Opening loopback port %d\n", s_port);
            break;

            case 'd':
                s_device_path = optarg;
                RLOGI("Opening tty device %s\n", s_device_path);
            break;

            case 's':
                s_device_path   = optarg;
                s_device_socket = 1;
                RLOGI("Opening socket %s\n", s_device_path);
                RLOGD("Got device!");
                //[Emu]TODO create the middleLayer
                is_gemini_emulator = 1;
            break;
            case 'c':
                RLOGD("skip the argument");
                break;

            #ifdef MTK_RIL
            case 'm':
                RLOGD("Input range: %s %s %s",optarg, argv[1], argv[2]);
                tmp = strtok(optarg, " ");
                s_device_range_begin = atoi(tmp) ;
                tmp = strtok(NULL, " ");
                s_device_range_end = atoi(tmp);

                if ((s_device_range_end - s_device_range_begin + 1) != getSupportChannels()) {
                    RLOGE("We can't accept the input configuration for muliple channel since we need %d COM ports", getSupportChannels());
                    return NULL;
                }

                RLOGD("Open the ttyS%d to ttyS%d", s_device_range_begin, s_device_range_end);

                RLOGD("Link ttyS....");
                ttys_index = s_device_range_begin;
                i = 0;
                while(ttys_index <= s_device_range_end)
                {
                    sprintf(path, "/dev/ttyS%d", ttys_index);
                    RLOGD("Unlock %s on Link %s", path, s_mux_path[i]);
                    /*if(chmod(path, 0666) < 0 )
                     * {
                     *  RLOGD("chomod: system-error: '%s' (code: %d)", strerror(errno), errno);
                     *  return NULL;
                     * }*/
                    if (symlink(path, s_mux_path[i]) < 0) {
                        RLOGD("symlink: system-error: '%s' (code: %d)", strerror(errno), errno);
                        return NULL;
                    }
                    ttys_index++;
                    i++;
                }
                break;
#endif  /* MTK_RIL */

            default:
                usage(argv[0]);
                return NULL;
        }
    }
    RLOGI("while out %s\n", s_device_path);
    if (s_port < 0 && s_device_path == NULL &&
        (s_device_range_begin < 0 || s_device_range_end < 0) ) {
        usage(argv[0]);
        return NULL;
    }

    pthread_attr_init (&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    //if (is_gemini_emulator){
    //    ret = pthread_create(&s_tid_mainloop, &attr, mainLoop_emulator, NULL);
    //}else{
        ret = pthread_create(&s_tid_mainloop, &attr, mainLoop, NULL);
    //}


    return &s_callbacks;
}

const RIL_RadioFunctionsSocket *RIL_SAP_Init(const struct RIL_EnvSocket *env, int argc,
        char **argv) {
    s_rilsapenv = env;
    return &s_sapcallbacks;
}
#else /* RIL_SHLIB */
int main (int argc, char **argv)
{
    int ret;
    int fd = -1;
    int opt;

    while ( -1 != (opt = getopt(argc, argv, "p:d:"))) {
        switch (opt) {
            case 'p':
                s_port = atoi(optarg);
                if (s_port == 0)
                    usage(argv[0]);
                RLOGI("Opening loopback port %d\n", s_port);
            break;

            case 'd':
                s_device_path = optarg;
                RLOGI("Opening tty device %s\n", s_device_path);
            break;

            case 's':
                s_device_path   = optarg;
                s_device_socket = 1;
                RLOGI("Opening socket %s\n", s_device_path);
            break;

            default:
                usage(argv[0]);
        }
    }

    if (s_port < 0 && s_device_path == NULL)
        usage(argv[0]);

    RIL_register(&s_callbacks);

    mainLoop(NULL);

    return 0;
}
#endif /* RIL_SHLIB */

int RIL_is3GSwitched()
{
    return 0;
}

int RIL_get3GSIM()
{
    char *simId = malloc(sizeof(char) * PROPERTY_VALUE_MAX);
    memset(simId, 0, sizeof(char) * PROPERTY_VALUE_MAX);

    property_get(PROPERTY_3G_SIM, simId, "1");

    return atoi(simId);
}

int isEmulatorRunning() {
    return 1;
}
int getTelephonyMode() {
    return 0;
}
int getFirstModem() {
    return 0;
}
int isCCCIPowerOffModem() {
    return 0;
}
int isDualTalkMode() {
    return 0;
}

int findPid(char* name){
    return -1;
}

int getExternalModemSlot() {
    return -1;
}

int isInternationalRoamingEnabled() {
    return 0;
}

int isSupportCommonSlot() {
    return 0;
}

int isSupportSimHotSwapC2k() {
    return 0;
}

int isSvlteSupport()
{
    return 0;
}

int isCdmaSupport()
{
    return 0;
}

int isCdmaIratSupport()
{
    return 0;
}

int isSvlteLcgSupport()
{
    return 0;
}

int isBootupWith3GCapability() {
	return 0;
}

int isEvdoOnDualtalkMode() {
    return 0;
}

/// M: workaround for world phone lib test @{ 
int isWorldPhoneSupport() {
    return 0;
}
/// @}

/// @}
int isLteSupport() {
    return 0;
}
int getMtkShareModemCurrent() {
    return 2;
}

int isImsSupport() {
    return 0;
}

int isCdmaLteDcSupport() {
    return 0;
}

int isSrlteSupport() {
    return 0;
}

int isEnableModulationReport() {
    return 0;
}

int getSvlteProjectType() {
    return 0;
}

int getSvlteTelephonyMode() {
    return TELEPHONY_MODE_UNKNOWN;
}

int isUserLoad() {
    return 0;
}
void setSimSwitchProp(int SimId) {}

void setMSimProperty(int phoneId, char *pPropertyName, char *pUpdateValue) {}	

void getMSimProperty(int phoneId, char *pPropertyName,char *pPropertyValue) {}

int isNeedCombineAttach()
{
    return 1;
}

int handleAee(const char *modem_warning, const char *modem_version) {
    return 1;
}

int getSimCount() {
    return SIM_COUNT;
}

/// M: SAP start
void sendSapResponseComplete(RIL_Token t, RIL_Errno ret, MsgId msgId, void *data) {
    const pb_field_t *fields = NULL;
    size_t encoded_size = 0;
    uint32_t written_size = 0;
    size_t buffer_size = 0;
    pb_ostream_t ostream;
    bool success = false;
    ssize_t written_bytes;
    int i = 0;

    RLOGD("sendSapResponseComplete, start (%d)", msgId);

    switch (msgId) {
        case MsgId_RIL_SIM_SAP_CONNECT:
            fields = RIL_SIM_SAP_CONNECT_RSP_fields;
            break;
        case MsgId_RIL_SIM_SAP_DISCONNECT:
            fields = RIL_SIM_SAP_DISCONNECT_RSP_fields;
            break;
        case MsgId_RIL_SIM_SAP_APDU:
            fields = RIL_SIM_SAP_APDU_RSP_fields;
            break;
        case MsgId_RIL_SIM_SAP_TRANSFER_ATR:
            fields = RIL_SIM_SAP_TRANSFER_ATR_RSP_fields;
            break;
        case MsgId_RIL_SIM_SAP_POWER:
            fields = RIL_SIM_SAP_POWER_RSP_fields;
            break;
        case MsgId_RIL_SIM_SAP_RESET_SIM:
            fields = RIL_SIM_SAP_RESET_SIM_RSP_fields;
            break;
        case MsgId_RIL_SIM_SAP_SET_TRANSFER_PROTOCOL:
            fields = RIL_SIM_SAP_SET_TRANSFER_PROTOCOL_RSP_fields;
            break;
        case MsgId_RIL_SIM_SAP_ERROR_RESP:
            fields = RIL_SIM_SAP_ERROR_RSP_fields;
            break;
        default:
            RLOGE("sendSapResponseComplete, MsgId is mistake!");
            return;
    }

    if ((success = pb_get_encoded_size(&encoded_size, fields, data)) &&
            encoded_size <= INT32_MAX) {
        //buffer_size = encoded_size + sizeof(uint32_t);
        buffer_size = encoded_size;
        uint8_t buffer[buffer_size];
        //written_size = htonl((uint32_t) encoded_size);
        ostream = pb_ostream_from_buffer(buffer, buffer_size);
        //pb_write(&ostream, (uint8_t *)&written_size, sizeof(written_size));
        success = pb_encode(&ostream, fields, data);

        if(success) {
            RLOGD("sendSapResponseComplete, Size: %d (0x%x) Size as written: 0x%x",
                encoded_size, encoded_size, written_size);
            // Send response
            RIL_SAP_onRequestComplete(t, ret, buffer, buffer_size);
        } else {
            RLOGE("sendSapResponseComplete, Encode failed!");
        }
    } else {
        RLOGE("Not sending response type %d: encoded_size: %u. encoded size result: %d",
        msgId, encoded_size, success);
    }
}
/// SAP end

void switchMuxPath() {}
void sendRadioCapabilityDoneIfNeeded () {}
void openChannelFds() {}
void queryActiveMode(RIL_SOCKET_ID rid, RILChannelCtx *p_channel) {}
void queryBearer(RIL_SOCKET_ID rid) {}
void imsInit(RIL_SOCKET_ID rid) {}

