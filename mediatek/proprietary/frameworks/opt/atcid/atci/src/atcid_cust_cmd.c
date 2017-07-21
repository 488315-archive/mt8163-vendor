/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

#include "atcid_serial.h"
#include "atcid_util.h"
#include "atcid_cust_cmd.h"
#include "at_tok.h"
#include "cutils/properties.h"
#include <private/android_filesystem_config.h>
#include <stdlib.h>
#include "libwifitest.h"
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h> // for using tolower()

#define ATCI_SIM "persist.service.atci.sim"
#define ATCI_PS "persist.service.atci.ps"
#define PERSIST_SYS_USB_CONFIG "persist.sys.usb.config"
#define ENABLE_URC_PROP "persist.service.atci_urc.enable"

int wifiEnableState = -1;
int wifiBand = -1;
uint32_t wifiFreq = 0;
uint32_t wifiRate = 0;
uint32_t wifiGain = 0;

int gRateCodeWiFi = 0;

extern int s_fdService_command;
/*misc global vars */
extern Serial serial;
extern  bool isCdmaLteDcSupport();

ATRESPONSE_t pas_cclk_handler(char* cmdline, ATOP_t opType, char* response) {
    time_t timep;
    struct tm *p;
    struct tm setclk;
    char   tm_buffer[40];
    pid_t child_pid;
    int status;
    struct timeval tv;
    struct timezone tz;

    LOGATCI(LOG_DEBUG, "cmdline %s", cmdline);

    switch(opType) {
        case AT_READ_OP:
            time(&timep);
            //p=localtime(&timep);
            p = gmtime(&timep);
            gettimeofday(&tv, &tz);
            LOGATCI(LOG_DEBUG, "timezone: %d", tz.tz_minuteswest);
            int GMT = (0 - tz.tz_minuteswest)/60;

            if (GMT >= 0) {
                sprintf(response, "+CCLK: \"%02d/%02d/%02d,%02d:%02d:%02d+%02d\"\n",
                        (1900+p->tm_year)%100, (1+p->tm_mon), p->tm_mday,
                        p->tm_hour, p->tm_min, p->tm_sec, GMT*4);
            } else {
                sprintf(response, "+CCLK: \"%02d/%02d/%02d,%02d:%02d:%02d%03d\"\n",
                        (1900+p->tm_year)%100, (1+p->tm_mon), p->tm_mday,
                        p->tm_hour, p->tm_min, p->tm_sec, GMT*4);
            }
            return AT_OK;
            break;
        case AT_SET_OP:
            if(strlen(cmdline) != strlen("yy/MM/dd,hh:mm:ss")) {
                sprintf(response, "CME ERROR: Invalid parameters");
                return AT_ERROR;
            }

            sscanf(cmdline, "%2d/%2d/%2d,%2d:%2d:%2d", &setclk.tm_year, &setclk.tm_mon,
                    &setclk.tm_mday, &setclk.tm_hour, &setclk.tm_min, &setclk.tm_sec);
            LOGATCI(LOG_DEBUG, "%02d/%02d/%02d,%02d:%02d:%02d", setclk.tm_year, setclk.tm_mon,
                    setclk.tm_mday, setclk.tm_hour, setclk.tm_min, setclk.tm_sec);
            memset(tm_buffer, 0, sizeof(tm_buffer));
            sprintf(tm_buffer, "%02d%02d%02d%02d%02d.%02d", setclk.tm_mon, setclk.tm_mday,
                    setclk.tm_hour, setclk.tm_min, setclk.tm_year, setclk.tm_sec);
            LOGATCI(LOG_DEBUG, "Set Date/Time [MMDDhhmmYY.ss] is [%s]", tm_buffer);

            child_pid = fork();
            if (child_pid < 0) {
                LOGATCI(LOG_ERR, "Can't fork for execute command");
                return AT_ERROR;
            }

            if (child_pid != 0) {
                waitpid(child_pid,&status,0);
                LOGATCI(LOG_DEBUG, "child status is [%d]", status);
            } else {
                if(execl("/system/bin/date", "date", tm_buffer, NULL) == -1) {
                    exit(1);
                }
            }

            return AT_OK;
            break;
        case AT_TEST_OP:
            strcpy(response, "+CCLK: yy/MM/dd,hh:mm:ss");
            return AT_OK;
            break;
        default:

            break;
    }

    return AT_ERROR;
}

ATRESPONSE_t pas_echo_handler(char* cmdline, ATOP_t opType, char* response) {
    int len = 0, i = 0, flag = -1;

    LOGATCI(LOG_DEBUG, "cmdline %s", cmdline);

    len = strlen(cmdline);
    flag = serial.echo[serial.currDevice];;

    switch(opType) {
        case AT_BASIC_OP:
        case AT_SET_OP:
            for(i = 0; i < len; i ++) {
                if( cmdline[i] == 'E' || cmdline[i] == 'e') {
                    if(i < len-1) {
                        flag = cmdline[i+1] - '0';
                        break;
                    }
                } else {
                    i++;
                }
            }

            if(flag == 0 || flag == 1) {
                setEchoOption(flag);
                return AT_OK;
            }

            break;
        case AT_READ_OP:
            sprintf(response, "%d\r\n", flag);
            return AT_OK;
            break;
        case AT_TEST_OP:
            sprintf(response, "0:disable echo, 1:enable echo\r\n", flag);
            return AT_OK;
            break;
        default:
            break;
    }

    return AT_ERROR;
}

ATRESPONSE_t pas_modem_handler(char* cmdline, ATOP_t opType, char* response) {

    return AT_ERROR;
}

ATRESPONSE_t pas_eurc_handler(char* cmdline, ATOP_t opType, char* response) {
    int err = 0, cmdID = 0;
    char enable_urc_prop[PROPERTY_VALUE_MAX];
    int urc_enabled = 0;
    LOGATCI(LOG_DEBUG, "pas_eurc_handler enter with opType: %d", opType);

    switch(opType) {
        case AT_SET_OP:
            err = at_tok_nextint(&cmdline, &cmdID);

            if (err < 0) return -1;

            if(1 == cmdID)
                property_set(ENABLE_URC_PROP,"1");
            else if(0 == cmdID)
                property_set(ENABLE_URC_PROP,"0");
            return AT_OK;
            break;
        case AT_TEST_OP:
            writeDataToSerial("+EURC:(0,1)",strlen("+EURC:(0,1)"));
            return AT_OK;
            break;
        case AT_READ_OP:
            property_get(ENABLE_URC_PROP, enable_urc_prop, "0");
            urc_enabled = atoi(enable_urc_prop);
            if(1 == urc_enabled) {
                strcpy(response, "+EURC:1\r\n");
            } else if (0 == urc_enabled) {
                strcpy(response, "+EURC:0\r\n");
            } else {
                strcpy(response, "+EURC:invalid value.\r\n");
            }
            return AT_OK;
            break;
        default:
            break;
    }
    return AT_ERROR;
}

ATRESPONSE_t pas_atci_handler(char* cmdline, ATOP_t opType, char* response) {
    int err = 0, cmdID = 0;
    char atci_usermode[PROPERTY_VALUE_MAX];
    int usermode = 0;
    LOGATCI(LOG_DEBUG, "pas_atci_handler enter with opType: %d", opType);

    switch(opType) {
        case AT_SET_OP:
            err = at_tok_nextint(&cmdline, &cmdID);

            if (err < 0) return -1;

            if(1 == cmdID)
                property_set(ATCI_IN_USERMODE_PROP,"1");
            else if(0 == cmdID)
                property_set(ATCI_IN_USERMODE_PROP,"0");
            return AT_OK;
            break;
        case AT_TEST_OP:
            writeDataToSerial("+ATCI:(0,1)",strlen("+ATCI:(0,1)"));
            return AT_OK;
            break;
        case AT_READ_OP:
            property_get(ATCI_IN_USERMODE_PROP, atci_usermode, "0");
            usermode = atoi(atci_usermode);
            if(1 == usermode) {
                strcpy(response, "+ATCI:1\r\n");
            } else if (0 == usermode) {
                strcpy(response, "+ATCI:0\r\n");
            } else {
                strcpy(response, "+ATCI:invalid value.\r\n");
            }
            return AT_OK;
            break;
        default:
            break;
    }
    return AT_ERROR;
}

ATRESPONSE_t pas_pct_handler(char* cmdline, ATOP_t opType, char* response) {
    int err = 0, cmdID = 0;
    char pct_mode[PROPERTY_VALUE_MAX];
    int pctmode = 0;
    LOGATCI(LOG_DEBUG, "pas_pct_handler enter with opType: %d", opType);

    switch(opType) {
        case AT_SET_OP:
            err = at_tok_nextint(&cmdline, &cmdID);

            if (err < 0) return -1;

            if(1 == cmdID)
                property_set(ATCI_IN_PCT_PROP,"1");
            else if(0 == cmdID)
                property_set(ATCI_IN_PCT_PROP,"0");
            return AT_OK;
            break;
        case AT_TEST_OP:
            writeDataToSerial("+PCT:(0,1)",strlen("+PCT:(0,1)"));
            return AT_OK;
            break;
        case AT_READ_OP:
            property_get(ATCI_IN_PCT_PROP, pct_mode, "0");
            pctmode = atoi(pct_mode);
            if(1 == pctmode) {
                strcpy(response, "+PCT:1\r\n");
            } else if (0 == pctmode) {
                strcpy(response, "+PCT:0\r\n");
            } else {
                strcpy(response, "+PCT:invalid value.\r\n");
            }
            return AT_OK;
            break;
        default:
            break;
    }
    return AT_ERROR;
}

ATRESPONSE_t pas_esuo_handler(char* cmdline, ATOP_t opType, char* response) {
    int err = 0, cmdID = 0;
    char simIDProperty[PROPERTY_VALUE_MAX];
    int simID = 0;
    LOGATCI(LOG_DEBUG, "pas_esuo_handler enter with opType: %d", opType);

    switch(opType) {
        case AT_SET_OP:
            err = at_tok_nextint(&cmdline, &cmdID);

            if (err < 0) return -1;

            //if the mode is dual talk, then we need to make sure which socket need to be connect by the property "ATCI_SIM"
            property_set(ATCI_PS, "");
            if (isDTSupport()) {

                LOGATCI(LOG_DEBUG, "current mode is dual talk and cmdID = d%. " + cmdID);
                if(cmdID == 4) { //Configure the socket connect to md1, 
                    setSocketConnect(SOCKET_MD1);
                } else if(cmdID == 5) { //Configure the socket connect to md2
                    setSocketConnect(SOCKET_MD2);
                }
                //return AT_OK;
            }
            //end dual talk handle

            //start for c2k

            if (isCdmaLteDcSupport()) {
                    LOGATCI(LOG_DEBUG, "C2K mode");
                if (cmdID == 4) {
                    LOGATCI(LOG_DEBUG, "Set default to SIM1, setSocket to MD1");
                    property_set(ATCI_SIM, "0");
                    setSocketConnect(SOCKET_MD1);
                } else if (cmdID == 9) {
                    LOGATCI(LOG_DEBUG, "C2K, setSocket to MD3");
                    property_set(ATCI_SIM, "9");
                    //setSocketConnect(SOCKET_MD3);
                } else if (cmdID == 5) {
                    LOGATCI(LOG_DEBUG, "Set default to SIM2, setSocket to MD1");
                    property_set(ATCI_SIM, "1");
                    setSocketConnect(SOCKET_MD1);
                } else {
                    return AT_ERROR;
                }
                return AT_OK;
            } else {
                if (cmdID == 9) {
                    return AT_ERROR;
                }
            }
            //end for c2k
            
#ifdef MTK_GEMINI
            if(cmdID == 4) { //Configure the default SIM to SIM1
                property_set(ATCI_SIM, "0");
                LOGATCI(LOG_DEBUG, "Set default to SIM1");
                return AT_OK;
            } else if(cmdID == 5) {
                //Configure the default SIM to SIM2
                property_set(ATCI_SIM, "1");
                LOGATCI(LOG_DEBUG, "Set default to SIM2");
                return AT_OK;
            }
#ifdef MTK_GEMINI_3SIM_SUPPORT
            else if(cmdID == 6) {
                //Configure the default SIM to SIM3
                property_set(ATCI_SIM, "2");
                LOGATCI(LOG_DEBUG, "Set default to SIM3");
                return AT_OK;
            }
#endif
#ifdef MTK_GEMINI_4SIM_SUPPORT
            else if(cmdID == 7) {
                //Configure the default SIM to SIM4
                property_set(ATCI_SIM, "3");
                LOGATCI(LOG_DEBUG, "Set default to SIM4");
                return AT_OK;
            }
#endif
#endif /* MTK_GEMINI */
            break;
        case AT_TEST_OP:
#ifdef MTK_GEMINI_3SIM_SUPPORT
            writeDataToSerial("+ESUO: (4-6)", strlen("+ESUO: (4-6)"));
#elif defined MTK_GEMINI_4SIM_SUPPORT
            writeDataToSerial("+ESUO: (4-7)", strlen("+ESUO: (4-7)"));
#else
            writeDataToSerial("+ESUO: (4-5)", strlen("+ESUO: (4-5)"));
#endif
            return AT_OK;
            break;
        case AT_READ_OP:
            property_get(ATCI_SIM, simIDProperty, "0");
            simID = atoi(simIDProperty);
#ifdef MTK_GEMINI
            if(simID == 1) {
                strcpy(response, "+ESUO:5\r\n");
            }
#ifdef MTK_GEMINI_3SIM_SUPPORT
            else if(simID == 2) {
                strcpy(response, "+ESUO:6\r\n");
            }
#endif
#ifdef MTK_GEMINI_4SIM_SUPPORT
            else if(simID == 3) {
                strcpy(response, "+ESUO:7\r\n");
            }
#endif
            else if (simID == 9) {
                strcpy(response, "+ESUO:9\r\n");
            } else {
                strcpy(response, "+ESUO:4\r\n");
            }
#endif
            if (isCdmaLteDcSupport()) {
                if(getSocketConnect() == SOCKET_MD3) {
                    strcpy(response, "+ESUO:9\r\n");
                }
            }

            return AT_OK;
            break;
        default:
            break;

    }

    return AT_ERROR;
}

ATRESPONSE_t pas_reboot_handler(char* cmdline, ATOP_t opType, char* response) {

    LOGATCI(LOG_DEBUG, "handle cmdline:%s", cmdline);

    if(opType != AT_ACTION_OP) {
        return AT_ERROR;
    }

    switch(opType) {
        case AT_ACTION_OP:
            system("shutdown -r -t 3");
            return AT_OK;
            break;
        default:
            break;
    }

    return AT_ERROR;
}

int sendATCommandToServiceWithResult(char* line) {
    if (s_fdService_command < 0) {
        connectTarget(GENERIC_TYPE);
    }
    sendDataToGenericService(line);
    return serviceReaderLoopWithResult(line);
}

ATRESPONSE_t pas_wienable_handler(char* cmdline, ATOP_t opType, char* response) {
    int err = 0, cmdID = 0;
    bool b = false;
    int r = -1;

    switch(opType) {
        case AT_SET_OP:
            err = at_tok_nextint(&cmdline, &cmdID);

            if (err < 0) return -1;
            switch(cmdID) {
                case 0:
                    if(wifiEnableState == 1) {
                        r = sendATCommandToServiceWithResult("AT+WITOF=1");
                        if(r != -1)
                            wifiEnableState = 0;
                        else
                            return AT_ERROR;
                    } else {
#ifdef MTK_WLAN_FEATURE
                        b = WIFI_TEST_CloseDUT();
#endif
                        if(b)
                            wifiEnableState = 0;
                        else
                            return AT_ERROR;
                    }
                    return AT_OK;
                case 1:
                    r = sendATCommandToServiceWithResult("AT+WITOF=2");
                    if(r != -1)
                        wifiEnableState = 1;
                    else
                        return AT_ERROR;
                    return AT_OK;
                case 2:
#ifdef MTK_WLAN_FEATURE
                    b = WIFI_TEST_OpenDUT();
#endif
                    if(b)
                        wifiEnableState = 2;
                    else
                        return AT_ERROR;
                    return AT_OK;
            }
            break;
        case AT_READ_OP:
            sprintf(response, "%d", wifiEnableState);
            return AT_OK;
        default:
            break;

    }

    return AT_ERROR;
}

ATRESPONSE_t pas_wimode_handler(char* cmdline, ATOP_t opType, char* response) {
    int err = 0, cmdID = 0;
    bool b = false;
    uint32_t mode = 0;

    switch(opType) {
        case AT_SET_OP:
            err = at_tok_nextint(&cmdline, &cmdID);

            if (err < 0) return -1;
            switch(cmdID) {
                case 0:
#ifdef MTK_WLAN_FEATURE
                    b = WIFI_TEST_SetMode(WIFI_TEST_MODE_BY_API_CONTROL);
#endif
                    break;
                case 1:
#ifdef MTK_WLAN_FEATURE
                    b = WIFI_TEST_SetMode(WIFI_TEST_MODE_CW_ONLY);
#endif
                    break;
                case 2:
#ifdef MTK_WLAN_FEATURE
                    b = WIFI_TEST_SetMode(WIFI_TEST_MODE_80211A_ONLY);
#endif
                    break;
                case 3:
#ifdef MTK_WLAN_FEATURE
                    b = WIFI_TEST_SetMode(WIFI_TEST_MODE_80211B_ONLY);
#endif
                    break;
                case 4:
#ifdef MTK_WLAN_FEATURE
                    b = WIFI_TEST_SetMode(WIFI_TEST_MODE_80211G_ONLY);
#endif
                    break;
            }
            if(b)
                return AT_OK;
            else
                return AT_ERROR;
        case AT_TEST_OP:
#ifdef MTK_WLAN_FEATURE
            b = WIFI_TEST_GetSupportedMode(mode);
#endif
            sprintf(response, "%d", mode);
            break;
        default:
            break;
    }
    if(b)
        return AT_OK;
    else
        return AT_ERROR;
}

ATRESPONSE_t pas_wiband_handler(char* cmdline, ATOP_t opType, char* response) {
    int err = 0, cmdID = 0;
    bool b = false;
    uint32_t mode = 0;
    switch(opType) {
        case AT_SET_OP:
            err = at_tok_nextint(&cmdline, &cmdID);
            if (err < 0) return -1;
            switch(cmdID) {
                case 0:
#ifdef MTK_WLAN_FEATURE
                    b = WIFI_TEST_SetBandwidth(WIFI_TEST_BW_20MHZ);
#endif
                    if(b) {
                        sprintf(response, "20MHZ");
                        wifiBand = 0;
                    }
                    break;
                case 1:
#ifdef MTK_WLAN_FEATURE
                    b = WIFI_TEST_SetBandwidth(WIFI_TEST_BW_40MHZ);
#endif
                    if(b) {
                        sprintf(response, "40MHZ");
                        wifiBand = 1;
                    }
                    break;
            }
            if(b)
                return AT_OK;
            else
                return AT_ERROR;
        case AT_READ_OP:
            if(wifiBand == 0) {
                sprintf(response, "20MHZ");
                return AT_OK;
            } else if(wifiBand == 1) {
                sprintf(response, "40MHZ");
                return AT_OK;
            } else {
                return AT_ERROR;
            }
            break;
        default:
            break;
    }
    return AT_ERROR;
}

ATRESPONSE_t pas_wifreq_handler(char* cmdline, ATOP_t opType, char* response) {
    int err = 0;
    bool b = false;
    uint32_t freq = 0, offset = 0;

    switch(opType) {
        case AT_SET_OP:
            err = at_tok_nextint(&cmdline, &freq);
            if (err < 0) return -1;
#ifdef MTK_WLAN_FEATURE
            b = WIFI_TEST_SetFrequency(freq, offset);
#endif
            if(b) {
                wifiFreq = freq;
                return AT_OK;
            } else {
                wifiFreq = -1;
                return AT_ERROR;
            }
        case AT_READ_OP:
            sprintf(response, "%d", wifiFreq);
            return AT_OK;
        default:
            break;
    }
    return AT_ERROR;
}

ATRESPONSE_t pas_widatarate_handler(char* cmdline, ATOP_t opType, char* response) {
    int err = 0;
    bool b = false;
    uint32_t rate = 0;

    switch(opType) {
        case AT_SET_OP:
            err = at_tok_nextint(&cmdline, &rate);
            if (err < 0) return -1;
#ifdef MTK_WLAN_FEATURE
            b = WIFI_TEST_SetRate(rate);
#endif
            if(b) {
                wifiRate = rate;
                LOGATCI(LOG_DEBUG, "pas_widatarate_handler [rate=%d] \n", wifiRate);
                gRateCodeWiFi = wifiRate;
                return AT_OK;
            } else {
                wifiRate = -1;
                return AT_ERROR;
            }
        case AT_READ_OP:
            sprintf(response, "%d", wifiRate);
            return AT_OK;
        default:
            break;
    }
    return AT_ERROR;
}

ATRESPONSE_t pas_wipow_handler(char* cmdline, ATOP_t opType, char* response) {
    int err = 0;
    bool b = false;
    uint32_t gain = 0;

    switch(opType) {
        case AT_SET_OP:
            err = at_tok_nextint(&cmdline, &gain);
            if (err < 0) return -1;
#ifdef MTK_WLAN_FEATURE
            b = WIFI_TEST_SetTXPower(gain);
#endif
            if(b) {
                wifiGain = gain;
                return AT_OK;
            } else {
                wifiGain = -1;
                return AT_ERROR;
            }
        case AT_READ_OP:
            sprintf(response, "%d", wifiGain);
            return AT_OK;
        default:
            break;
    }
    return AT_ERROR;
}

ATRESPONSE_t pas_witx_handler(char* cmdline, ATOP_t opType, char* response) {
    int err = 0, cmd = -1;
    bool b = false;

    switch(opType) {
        case AT_SET_OP:
            err = at_tok_nextint(&cmdline, &cmd);
            if (err < 0) return -1;
#ifdef MTK_WLAN_FEATURE
            b = WIFI_TEST_SetTX(cmd == 0 ? false : true);
#endif
            if(b)
                return AT_OK;
            else
                return AT_ERROR;
        default:
            break;
    }
    return AT_ERROR;
}

ATRESPONSE_t pas_wirx_handler(char* cmdline, ATOP_t opType, char* response) {
    int err = 0, cmd = -1;
    bool b = false;
    bool enable = false;
    char* srcAddr = "", dstAddr = "";

    switch(opType) {
        case AT_SET_OP:
            err = at_tok_nextint(&cmdline, &cmd);
            if (err < 0) return -1;
            switch(cmd) {
                case 0:
#ifdef MTK_WLAN_FEATURE
                    b = WIFI_TEST_SetRX(false, srcAddr, dstAddr);
#endif
                    break;
                case 1:
                    err = at_tok_nextint(&cmdline, &srcAddr);
                    if (err < 0) return -1;
                    err = at_tok_nextint(&cmdline, &dstAddr);
                    if (err < 0) return -1;
#ifdef MTK_WLAN_FEATURE
                    b = WIFI_TEST_SetRX(true, srcAddr, dstAddr);
#endif
                    break;
            }
        default:
            break;
    }
    if(b)
        return AT_OK;
    else
        return AT_ERROR;
}

ATRESPONSE_t pas_wirpckg_handler(char* cmdline, ATOP_t opType, char* response) {
    int err = 0;
    bool b = false;
    uint32_t pu4GoodFrameCount = 0;
    uint32_t pu4BadFrameCount = 0;

    switch(opType) {
        case AT_SET_OP:
#ifdef MTK_WLAN_FEATURE
            b = WIFI_TEST_ClearResult();
#endif
            if(b) {
                return AT_OK;
            } else {
                return AT_ERROR;
            }
        case AT_READ_OP:
#ifdef MTK_WLAN_FEATURE
            b = WIFI_TEST_GetResult(&pu4GoodFrameCount, &pu4BadFrameCount);
#endif
            sprintf(response, "%d,%d OK", pu4GoodFrameCount, pu4BadFrameCount);
            if(b)
                return AT_OK;
            else
                return AT_ERROR;
        default:
            break;
    }
    return AT_ERROR;
}

#ifndef MTK_TC1_FEATURE
ATRESPONSE_t pas_witestmode_handler(char* cmdline, ATOP_t opType, char* response) {
    return AT_ERROR;
}

ATRESPONSE_t pas_witx2_handler(char* cmdline, ATOP_t opType, char* response) {
    return AT_ERROR;
}

ATRESPONSE_t pas_wirx2_handler(char* cmdline, ATOP_t opType, char* response) {
    return AT_ERROR;
}

ATRESPONSE_t pas_wimac_handler(char* cmdline, ATOP_t opType, char* response) {
    return AT_ERROR;
}

ATRESPONSE_t pas_wimacck_handler(char* cmdline, ATOP_t opType, char* response) {
    return AT_ERROR;
}
#endif

/*
ATRESPONSE_t pas_enabledata_handler(char* cmdline, ATOP_t opType, char* response) {
    int err = 0;
    int cmdID = 0;
    LOGATCI(LOG_DEBUG, "handle cmdline:%s", cmdline);
    if (opType != AT_SET_OP) {
        return AT_ERROR;
    }
    switch(opType) {
       case AT_SET_OP:
            err = at_tok_nextint(&cmdline, &cmdID);
            if (err < 0) {
                return -1;
            }
            if (1 == cmdID) {   
                system("iptables -t mangle -D OUTPUT -o cc+ -j DROP;iptables -t mangle -D OUTPUT -p icmp  -j ACCEPT");
                return AT_OK;
            } else if (0 == cmdID) {
                system("iptables -t mangle -A OUTPUT -o cc+ -j DROP;iptables -t mangle -I OUTPUT -p icmp  -j ACCEPT");
                return AT_OK;
            }       
        default:
            break;
    }

    return AT_ERROR;
}
*/


ATRESPONSE_t pas_usbTethering_handler(char* cmdline, ATOP_t opType, char* response) {
    int err = 0;
    int cmdID = 0;
    char usbTethering[PROPERTY_VALUE_MAX] = {0};
    int isSet = 0;
    
    LOGATCI(LOG_DEBUG, "handle cmdline:%s", cmdline);
    if (opType != (AT_SET_OP) && (opType != AT_READ_OP)) {
        return AT_ERROR;
    }
    switch(opType) {
       case AT_SET_OP:
            err = at_tok_nextint(&cmdline, &cmdID);
            if (err < 0) {
                return -1;
            }
            if (1 == cmdID) {   
                property_set(USB_TETHERING_PROP, "1");
                LOGATCI(LOG_DEBUG, "set usbTethering to 1");
                return AT_OK;
            } else if (0 == cmdID) {
                property_set(USB_TETHERING_PROP, "0");
                return AT_OK;
            }
        case AT_READ_OP:
            property_get(USB_TETHERING_PROP, usbTethering, "0");
            isSet = atoi(usbTethering);
            if (isSet == 1) {
                strcpy(response, "+USBTETHERING:1\r\n");
            } else {
                strcpy(response, "+USBTETHERING:0\r\n");
            }
            return AT_OK;
        default:
            break;
    }
    return AT_ERROR;
}

ATRESPONSE_t pas_usbconfig_handler(char* cmdline, ATOP_t opType, char* response) {
    char config[PROPERTY_VALUE_MAX] = {0};
    int retval = -1;

    LOGATCI(LOG_DEBUG, "handle cmdline:%s", cmdline);

    if (opType != (AT_SET_OP) && (opType != AT_READ_OP)) {
        return AT_ERROR;
    }

    switch(opType) {
        /*
         * Runtime switch USB HIF through MTK-defined ATCMD
         *
         * AT@USBCONFIG=<config>
         * AT@USBCONFIG=rndis,acm              --> RNDIS + AT COM
         * AT@USBCONFIG=rndis,adb,acm          --> RNDIS + ADB + ELT COM
         * AT@USBCONFIG=rndis,gs3,dual_acm     --> RNDIS + AT COM + ELT COM
         * AT@USBCONFIG=rndis,adb,gs3,dual_acm --> RNDIS + ADB + AT COM + ELT COM
         */
        case AT_SET_OP: {
            LOGATCI(LOG_DEBUG, "opType = AT_SET_OP");

            // Eliminates any [ \'\"] in cmdline and changes to lower case
            unsigned int i = 0;
            char *line;
            line = config;
            for (i = 0; i < strlen(cmdline); i++) {
                if (i == PROPERTY_VALUE_MAX - 1) {
                    break;
                }

                if (cmdline[i] != ' ' && cmdline[i] != '\"' &&
                        cmdline[i] != '\''&& cmdline[i] != '\n') {
                    *(line++) = tolower(cmdline[i]);
                }
            }
            *line = '\0';
            LOGATCI(LOG_DEBUG, "reads cmdline as config: %s", config);

            if (strlen(config) <= 0) {
                LOGATCI(LOG_DEBUG, "skip setting %s property due to length=%d is abnormal",
                        PERSIST_SYS_USB_CONFIG, (int) strlen(config));
                sprintf(response, "\r\n%d\r\nERROR\r\n\r\n", retval);
                return AT_ERROR;
            }

            retval = property_set(PERSIST_SYS_USB_CONFIG, config);

            if (retval == 0) {
                LOGATCI(LOG_DEBUG, "set %s successful", PERSIST_SYS_USB_CONFIG);
                sprintf(response, "\r\n@USBCONFIG=\"%s\"\r\n\r\nOK\r\n", config);
                return AT_OK;
            } else {
                LOGATCI(LOG_DEBUG, "set %s property failed, length=%d is abnormal",
                        PERSIST_SYS_USB_CONFIG, retval);
                sprintf(response, "\r\n@USBCONFIG=\"%s\"\r\n\r\nERROR\r\n", config);
                return AT_ERROR;
            }

            break;
        }
        /*
         * Get USB HIF through MTK-defined ATCMD
         *
         * AT@USBCONFIG?
         */
        case AT_READ_OP: {
            LOGATCI(LOG_DEBUG, "opType = AT_READ_OP");
            retval = property_get(PERSIST_SYS_USB_CONFIG, config, NULL);

            if (retval > 0) {
                LOGATCI(LOG_DEBUG, "get %s property successful, config=%s, length=%d are decent",
                        PERSIST_SYS_USB_CONFIG, config, retval);
                sprintf(response, "\r\n@USBCONFIG: \"%s\"\r\n\r\nOK\r\n", config);
                return AT_OK;
            } else {
                LOGATCI(LOG_DEBUG, "get %s property fail, config=%s, length=%d is abnormal",
                        PERSIST_SYS_USB_CONFIG, config, retval);
                sprintf(response, "\r\n@USBCONFIG: \"%s\"\r\n\r\nERROR\r\n", config);
                return AT_ERROR;
            }
        }
        default:
            LOGATCI(LOG_DEBUG, "unknown opType :%d", opType);
            break;
    }
    return AT_ERROR;
}
