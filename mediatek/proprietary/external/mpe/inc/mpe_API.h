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

#ifndef MPE_API_H
#define MPE_API_H

#ifdef __cplusplus
  extern "C" {
#endif

/****************************************************************************** 
* Include
******************************************************************************/
#include "mpe_Attitude.h"

/****************************************************************************** 
* Define
******************************************************************************/  
// Define ASCII character type
typedef  char           CH;      // ASCII character
typedef unsigned char   U1;
typedef  U1  BL;      // boolean logical (TRUE or FALSE only)
typedef unsigned char           mtk_uint8;
typedef signed char             mtk_int8;
typedef unsigned short int      mtk_uint16;
typedef signed short int        mtk_int16;
typedef unsigned int            mtk_uint32;
typedef signed int              mtk_int32;
typedef float                   mtk_r4;
typedef double                  mtk_r8;
typedef void *PVOID;
typedef PVOID HANDLE;

/****************************************************************************** 
* enum
******************************************************************************/
//MPE task ID
typedef enum {
	MPE_MODE_INVALID = -1,
	MPE_MODE_RUN = 0,
	MPE_MODE_PAUSE,
  MPE_MODE_CONTINUE,
  MPE_MODE_READ_RAW_DATA,
  MPE_MODE_READ_CALIBRATE_DATA,
  MPE_MODE_READ_FUSION_DATA,
  MPE_MODE_DR_SET_MODE,
  MPE_MODE_MNL_INPUT_LOC,
  MPE_MODE_FLP_INPUT_LOC,
  MPE_MODE_DEINIT,
  MPE_MODE_END
} MPE_MODE;

typedef enum {
  // To MPE 
  CMD_START_MPE_REQ = 0x00, // no payload , request start of MPE
  CMD_STOP_MPE_REQ,         // no payload, request stop of MPE
  CMD_SET_MPE_MODE,         //  set MPE operational mode 
  CMD_DEINIT_MPE_REQ,             //Request shutdown of MPE
  CMD_GET_SENSOR_RAW_REQ,    //  request for raw sensor data
  CMD_GET_SENSOR_CALIBRATION_REQ,  //  request for calibrated sensor data
  CMD_GET_SENSOR_FUSION_REQ, //  request for sensor fusion data (Euler angle)
  CMD_GET_PDR_STATUS_REQ,     //Request MPE send fused location (per request, by FLP)    
  CMD_GET_ADR_STATUS_REQ,     // Request MPE send AR & heading status (per request, by MNL)
  CMD_SEND_GPS_TIME_RES,       //Send GPS timeto MPE
    CMD_SE_TRIGGER_ADR_CALC_REQ,    //trigger calculation of ADR upon receiving one gyro data
  
  // From MPE 
  CMD_START_MPE_RES = 0x20,  //no payload, response MPE start status
  CMD_STOP_MPE_RES,          //no payload, response MPE stop status
  CMD_SEND_SENSOR_RAW_RES,   // response  MPE sensor raw data
  CMD_SEND_SENSOR_CALIBRATION_RES, // response MPE  calibrated sensor data
  CMD_SEND_SENSOR_FUSION_RES, // response MPE  fused sensor  data
  CMD_SEND_PDR_STATUS_RES,     //PDR response fused loc status upon request to FLP
  CMD_SEND_ADR_STATUS_RES,     //ADR response AR & heading status upon request to MNL
  CMD_SEND_GPS_TIME_REQ,       //Request for GPS time for recording & replay purpose

  //From MNLD
  CMD_SEND_FROM_MNLD = 0x40, //with payload, mtklogger recording setting & path
  CMD_MPE_END = 0x50    
}MPE_CMD;

//location source
typedef enum {
  MPE_LOC_SOURCE_FLP = 1,
  MPE_LOC_SOURCE_GNSS = 2,
  MPE_LOC_SOURCE_OTHER = 4,
  MPE_LOC_SOURCE_END
} MPE_LOC_SOURCE;

/****************************************************************************** 
* structure
******************************************************************************/
typedef struct {
  double lla_in[3];    /* input latitude, longitude, and altitude in radian*/
  float vned_in[3];   /* input velocities in NED frame [vn, ve, vd] in meter/s */
  float lla_in_acc[3];   /* position accuracies in meter*/
  float vned_in_acc[3];  /* velocity accuracies meter/s*/
  mtk_int32 gps_sec;          /* GPS second*/
  mtk_int32 leap_sec;         /*current leap second*/    
}MNL_location_input_t;

typedef struct {
  double lla_out[3];      /* output latitude, longitude, and altitude in radian*/
  float vel_out[3];      /* velocities output in NED frame [vn, ve, vd] in meter/s */
  float heading_out;     /* output heading in radian*/
  float heading_out_acc; /* heading accuracy in radian */
    float height;         /*barometer height in meter*/
  mtk_int32 vout;            /* output calculated velocity/AR status [static, move, uncertain],[0,1,99]*/
  mtk_int32 validation_flag; /*valid flag for mpe result[ valid, invalid], [1,0]*/
}MNL_location_output_t;

typedef struct {
  double lla_in[3];    /* input latitude, longitude, and altitude*/
  float vned_in;   /* input velocitY*/
  float lla_in_acc;   /* position accuracY*/
}FLP_location_input_t;

typedef struct {
  double lla_out[3];      /* output latitude, longitude, and altitude*/
  float lla_acc;         /* output location accuracy*/
  float heading_out;     /* output heading */
  float heading_out_acc; /* heading accuracy*/
  float vout;            /* output calculated velocity*/
}FLP_location_output_t;

typedef struct {
  float accelerometer_raw[3];  /* raw data of accelerometer*/
  float gyro_raw[3];           /* raw data of gyro*/
  float magnetometer_raw[3];   /* raw data of magnetometer */
}MPE_SENSOR_RAW;

typedef struct {
  float accelerometer_k[3];  /* calibrated data of accelerometer*/
  float gyro_k[3];           /* calibrated data of gyro*/
  float magnetometer_k[3];   /* calibrated data of magnetometer */
}MPE_SENSOR_CALIBRATED;

typedef struct {
  float yaw;   /* calculated yaw angle*/
  float pitch; /* calculated pitch angle*/
  float roll;  /* calculated roll angle*/
}MPE_SENSOR_FUSION;

/****************************************************************************** 
* functions Prototype
******************************************************************************/
typedef int (*SExplorerCallBack)(IMU data[], int size);

BL MPE_API_init (void);
BL MPE_API_deinit (void);
BL MPE_API_run (MPE_LOC_SOURCE source, int sampling_rate);
BL MPE_API_pause (MPE_LOC_SOURCE source,int flag);
BL MPE_API_continue (void);
BL MPE_API_MNL_loc_input_data (MNL_location_input_t data );
BL MPE_API_FLP_loc_input_data (FLP_location_input_t data );

#ifdef __cplusplus
}
#endif

#endif //#ifndef MPE_API_H
