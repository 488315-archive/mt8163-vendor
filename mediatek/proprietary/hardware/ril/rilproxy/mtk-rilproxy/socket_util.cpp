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

#include "ril.h"
#include "socket_util.h"

char* RilpSocketUtil::getSocketName(RILD_RadioTechnology_Group group, RIL_SOCKET_ID socket_id) {
    char *socketName = "null";

    if(group == RADIO_TECH_GROUP_GSM) {
        switch(socket_id) {
            case RIL_SOCKET_1:
                socketName = "mrild";
                break;
        #if (SIM_COUNT >= 2)
            case RIL_SOCKET_2:
                socketName = "mrild2";
                break;
        #endif
        #if (SIM_COUNT >= 3)
            case RIL_SOCKET_3:
                socketName = "mrild3";
                break;
        #endif
        #if (SIM_COUNT >= 4)
            case RIL_SOCKET_4:
                socketName = "mrild4";
                break;
        #endif
            default:
                RLOGE("Socket id is wrong!!");
                break;
        }
    } else if (group == RADIO_TECH_GROUP_C2K) {
        socketName = "rild-via";
    } else if (group == RADIO_TECH_GROUP_ATCI) {
        socketName = "rilproxy-atci";
    } else if (group == RADIO_TECH_GROUP_SAP_GSM) {
        switch (socket_id) {
            case RIL_SOCKET_1:
                socketName = "msap_uim_socket1";
                break;
        #if (SIM_COUNT >= 2)
            case RIL_SOCKET_2:
                socketName = "msap_uim_socket2";
                break;
        #endif
        #if (SIM_COUNT >= 3)
            case RIL_SOCKET_3:
                socketName = "msap_uim_socket3";
                break;
        #endif
        #if (SIM_COUNT >= 4)
            case RIL_SOCKET_4:
                socketName = "msap_uim_socket4";
                break;
        #endif
            default:
                RLOGE("Socket id is wrong!!");
                break;
        }
    } else if (group == RADIO_TECH_GROUP_SAP_C2K) {
        switch (socket_id) {
            case RIL_SOCKET_1:
                socketName = "msap_c2k_socket1";
                break;
        #if (SIM_COUNT >= 2)
            case RIL_SOCKET_2:
                socketName = "msap_c2k_socket2";
                break;
        #endif
        #if (SIM_COUNT >= 3)
            case RIL_SOCKET_3:
                socketName = "msap_c2k_socket3";
                break;
        #endif
        #if (SIM_COUNT >= 4)
            case RIL_SOCKET_4:
                socketName = "msap_c2k_socket4";
                break;
        #endif
            default:
                RLOGE("Socket id is wrong!!");
                break;
        }
    } else if (group == RADIO_TECH_GROUP_SAP_BT) {
        switch (socket_id) {
            case RIL_SOCKET_1:
                socketName = "sap_uim_socket1";
                break;
        #if (SIM_COUNT >= 2)
            case RIL_SOCKET_2:
                socketName = "sap_uim_socket2";
                break;
        #endif
        #if (SIM_COUNT >= 3)
            case RIL_SOCKET_3:
                socketName = "sap_uim_socket3";
                break;
        #endif
        #if (SIM_COUNT >= 4)
            case RIL_SOCKET_4:
                socketName = "sap_uim_socket4";
                break;
        #endif
            default:
                RLOGE("Socket id is wrong!!");
                break;
        }
    } else {
        RLOGE("Socket group is wrong!!");
    }
    RLOGI ("getSocketName %s\n", socketName);
    return socketName;
}
