/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2012
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
#ifndef MTK_FLP_SYS_TYPE_H
#define MTK_FLP_SYS_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

//#ifdef WIN32 // clear warning for WIN32
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
//#endif

/*****************************************************************************
 * Basic Type Definitions
 *****************************************************************************/
typedef signed char         INT8;
typedef signed short        INT16;
#ifndef INT32
typedef signed int          INT32;
#endif
typedef unsigned char       UINT8;
typedef unsigned short      UINT16;
typedef unsigned int        UINT32;
typedef int64_t             INT64;
typedef uint64_t            UINT64;

#ifndef CHAR
typedef char                CHAR;
#endif
typedef char                CH;

#ifndef BOOL
typedef INT32               BOOL;//, BL;
#endif

#ifndef VOID
typedef void                VOID;
#endif

#ifndef TRUE
#define TRUE                (1)
#endif

#ifndef FALSE
#define FALSE               (0)
#endif

#ifndef NULL
#define NULL                ((void *)0)
#endif

#ifndef FLP_FILE
typedef UINT32              FLP_FILE;
#endif


/*************************************************
  Message Table for MTK FLP
    XXXX_CMD --> for command/response pair.
    XXXX_RSP  --> for command/response pair.
    XXXX_NTF  --> for notification.
*************************************************/
typedef enum{
    MTK_FLP_MSG_SYS_SHUTDOWN_NTF = 0x00,
    MTK_FLP_MSG_SYS_,

    //HAL messages
    MTK_FLP_MSG_HAL_INIT_CMD = 0x10,
    MTK_FLP_MSG_HAL_INIT_RSP,
    MTK_FLP_MSG_HAL_DEINIT_CMD,
    MTK_FLP_MSG_HAL_DEINIT_RSP,
    MTK_FLP_MSG_HAL_START_CMD,
    MTK_FLP_MSG_HAL_START_RSP,
    MTK_FLP_MSG_HAL_STOP_CMD,
    MTK_FLP_MSG_HAL_STOP_RSP,
    MTK_FLP_MSG_HAL_SET_OPTION_CMD,
    MTK_FLP_MSG_HAL_SET_OPTION_RSP,
    MTK_FLP_MSG_HAL_INJECT_LOC_CMD,
    MTK_FLP_MSG_HAL_INJECT_LOC_RSP,
    MTK_FLP_MSG_HAL_REPORT_LOC_NTF,
    MTK_FLP_MSG_HAL_DIAG_INJECT_DATA_NTF,
    MTK_FLP_MSG_HAL_DIAG_REPORT_DATA_NTF,
    MTK_FLP_MSG_HAL_COMMON_INDEX,           //common message for HAL/KER/DC, messages less than is index can be converted.
    MTK_FLP_MSG_HAL_ACQUIRE_WAKE_LOCK_NTF,
    MTK_FLP_MSG_HAL_RELEASE_WAKE_LOCK_NTF,
    MTK_FLP_MSG_HAL_GEOFENCE_CALLBACK_NTF,
    MTK_FLP_MSG_HAL_GEOFENCE_CMD,
    MTK_FLP_MSG_HAL_GEOFENCE_RSP,
    MTK_FLP_MSG_HAL_REQUEST_LOC_NTF,
    MTK_FLP_MSG_HAL_FLUSH_LOC_NTF,
    MTK_FLP_MSG_OFL_GEOFENCE_CALLBACK_NTF,  //for Offload geofence use
    MTK_FLP_MSG_OFL_GEOFENCE_CMD,
    MTK_FLP_MSG_OFL_GEOFENCE_RSP,


    //KERNEL Messages
    MTK_FLP_MSG_KER_INIT_CMD = 0x30,
    MTK_FLP_MSG_KER_INIT_RSP,
    MTK_FLP_MSG_KER_DEINIT_CMD,
    MTK_FLP_MSG_KER_DEINIT_RSP,
    MTK_FLP_MSG_KER_START_CMD,
    MTK_FLP_MSG_KER_START_RSP,
    MTK_FLP_MSG_KER_STOP_CMD,
    MTK_FLP_MSG_KER_STOP_RSP,
    MTK_FLP_MSG_KER_SET_OPTION_CMD,
    MTK_FLP_MSG_KER_SET_OPTION_RSP,
    MTK_FLP_MSG_KER_INJECT_LOC_CMD,
    MTK_FLP_MSG_KER_INJECT_LOC_RSP,
    MTK_FLP_MSG_KER_REPORT_LOC_NTF,
    MTK_FLP_MSG_KER_DIAG_INJECT_DATA_NTF,
    MTK_FLP_MSG_KER_DIAG_REPORT_DATA_NTF,
    MTK_FLP_MSG_KER_COMMON_INDEX,           //common message for HAL/KER/DC, messages less than is index can be converted.

    //Data center messages
    MTK_FLP_MSG_DC_INIT_CMD = 0x50,
    MTK_FLP_MSG_DC_INIT_RSP,
    MTK_FLP_MSG_DC_DEINIT_CMD,
    MTK_FLP_MSG_DC_DEINIT_RSP,
    MTK_FLP_MSG_DC_START_CMD,
    MTK_FLP_MSG_DC_START_RSP,
    MTK_FLP_MSG_DC_STOP_CMD,
    MTK_FLP_MSG_DC_STOP_RSP,
    MTK_FLP_MSG_DC_SET_OPTION_CMD,
    MTK_FLP_MSG_DC_SET_OPTION_RSP,
    MTK_FLP_MSG_DC_INJECT_LOC_CMD,
    MTK_FLP_MSG_DC_INJECT_LOC_RSP,
    MTK_FLP_MSG_DC_REPORT_LOC_NTF,
    MTK_FLP_MSG_DC_DIAG_INJECT_DATA_NTF,
    MTK_FLP_MSG_DC_DIAG_REPORT_DATA_NTF,
    MTK_FLP_MSG_DC_COMMON_INDEX=   0x60,        //common message for HAL/KER/DC, messages less than is index can be converted.
    MTK_FLP_MSG_DC_GEO_PASSTHRU_SEND_NTF,
    MTK_FLP_MSG_DC_GEO_PASSTHRU_RECV_NTF,
    MTK_FLP_MSG_DC_BT_SETUP_STATUS_NTF = 0x63,     //fix value for bt interface.
    MTK_FLP_MSG_DC_WIFI_MANUAL_GET_LOC_NTF,
    MTK_FLP_MSG_DC_RAW_SAP_NTF=     0x70,       //raw sensor measurement
    MTK_FLP_MSG_DC_RAW_HPC_NTF,                 //for hpc execution
    MTK_FLP_MSG_DC_RAW_WIFI_NTF,
    MTK_FLP_MSG_DC_RAW_BT_NTF,
    MTK_FLP_MSG_DC_RAW_CELL_NTF,
    MTK_FLP_DC_ENA_DEBUG_CMD,
    MTK_FLP_MSG_CONN_SCREEN_STATUS,     //for AP to connsys screen on/off status exchange
    MTK_FLP_MSG_DC_RAW_SENSORHUB_CMD,  //init sensorhub
    MTK_FLP_MSG_DC_RAW_SENSORHUB_NTF,   //ntf from sensorhub
    MTK_FLP_MDS_DC_CELL_DATA_TO_AP,    //relay data to AP for cell ID checking
    MTK_FLP_MDS_DC_CELL_DATA_FROM_AP,  //receive cell lla data from AP

    //mnld special command
    MTK_FLP_MSG_MNLD_NETWORK_STATUS_NTF= 0x90,   //mnld update network status to flp
    MTK_FLP_MSG_END,

}MTK_FLP_MSG_TYPE;

#define MSG_HAL2KER(msg) (msg + (MTK_FLP_MSG_KER_INIT_CMD-MTK_FLP_MSG_HAL_INIT_CMD ))
#define MSG_KER2HAL(msg) (msg - (MTK_FLP_MSG_KER_INIT_CMD-MTK_FLP_MSG_HAL_INIT_CMD ))
#define MSG_KER2DC(msg) (msg + (MTK_FLP_MSG_DC_INIT_CMD-MTK_FLP_MSG_KER_INIT_CMD ))
#define MSG_DC2KER(msg) (msg - (MTK_FLP_MSG_DC_INIT_CMD-MTK_FLP_MSG_KER_INIT_CMD ))
#define MSG_HAL2DC(msg) (msg + (MTK_FLP_MSG_DC_INIT_CMD-MTK_FLP_MSG_HAL_INIT_CMD ))
#define MSG_DC2HAL(msg) (msg - (MTK_FLP_MSG_DC_INIT_CMD-MTK_FLP_MSG_HAL_INIT_CMD ))


typedef enum MTK_FLP_LOG_PRIORITY_E{
    MTK_FLP_LOG_UNKNOWN = 0,
    MTK_FLP_LOG_DEFAULT,    /* only for SetMinPriority() */
    MTK_FLP_LOG_VERBOSE,
    MTK_FLP_LOG_DEBUG,
    MTK_FLP_LOG_INFO,
    MTK_FLP_LOG_WARN,
    MTK_FLP_LOG_ERROR,
    MTK_FLP_LOG_FATAL,
    MTK_FLP_LOG_SILENT,     /* only for SetMinPriority(); must be last */
} MTK_FLP_LOG_PRIORITY_E;


typedef struct {
    UINT16      param;
    UINT16      len;    //data len
} MTK_FLP_CMD_T;

typedef struct {
    UINT16      status;
    UINT16      len;    //data len
} MTK_FLP_RSP_T;


typedef struct {
    double max_power_allocation_mW;
    UINT32 sources_to_use;
    INT32 flags;
    INT64 period_ns;
} MTK_FLP_BATCH_OPTION_T;

/** Milliseconds since January 1, 1970 */
typedef int64_t MTK_FLP_UTC_TIME;
typedef size_t  MTK_FLP_SIZE_T;

/** Flags to indicate which values are valid in a FlpLocation. */
typedef uint16_t FlpLocationFlags;

/**
 * Constants to indicate the various subsystems
 * that will be used.
 */
#define FLP_TECH_MASK_GNSS      (1U<<0)
#define FLP_TECH_MASK_WIFI      (1U<<1)
#define FLP_TECH_MASK_SENSORS   (1U<<2)
#define FLP_TECH_MASK_CELL      (1U<<3)
#define FLP_TECH_MASK_BLUETOOTH (1U<<4)
#define FLP_TECH_MASK_PSEUDO    (1U<<5)


/** FlpLocation has valid latitude and longitude. */
#define FLP_LOCATION_HAS_LAT_LONG   (1U<<0)
/** FlpLocation has valid altitude. */
#define FLP_LOCATION_HAS_ALTITUDE   (1U<<1)
/** FlpLocation has valid speed. */
#define FLP_LOCATION_HAS_SPEED      (1U<<2)
/** FlpLocation has valid bearing. */
#define FLP_LOCATION_HAS_BEARING    (1U<<4)
/** FlpLocation has valid accuracy. */
#define FLP_LOCATION_HAS_ACCURACY   (1U<<8)

typedef struct {
    /** set to sizeof(FlpLocation) */
    MTK_FLP_SIZE_T  size;
    /** Flags associated with the location object. */
    UINT16          flags;
    /** Represents l atitude in degrees. */
    double          latitude;
    /** Represents longitude in degrees. */
    double          longitude;
    /**
     * Represents altitude in meters above the WGS 84 reference
     * ellipsoid. */
    double          altitude;
    /** Represents speed in meters per second. */
    float           speed;
    /** Represents heading in degrees. */
    float           bearing;
    /** Represents expected accuracy in meters. */
    float           accuracy;

    /** Timestamp for the location fix.  Milliseconds since January 1, 1970 */
    MTK_FLP_UTC_TIME      timestamp;

    /** Sources used, will be Bitwise OR of the FLP_TECH_MASK bits. */
    UINT32         sources_used;
} MTK_FLP_LOCATION_T;




#ifdef __cplusplus
}
#endif

#endif /* MTK_FLP_SYS_TYPE_H */
