/***********************************************************************
*   This software/firmware and related documentation ("MediaTek Software")
*   are protected under relevant copyright laws. The information contained
*   herein is confidential and proprietary to MediaTek Inc. and/or its licensors.
*
*   Without the prior written permission of MediaTek Inc. and/or its licensors,
*   any reproduction, modification, use or disclosure of MediaTek Software, and
*   information contained herein, in whole or in part, shall be strictly prohibited.
*
*   MediaTek Inc. (C) [2008]. All rights reserved.
*
*************************************************************************/
/*****************************************************************************
 *
 * Filename:
 * ---------
 *   mtk_gps_sys_fp.h
 *
 * Description:
 * ------------
 *   Marco porting layer APT to Function pointer using by MTK navigation library
 *
 ****************************************************************************/

#ifndef MTK_GPS_SYS_FP_H
#define MTK_GPS_SYS_FP_H

#include "mtk_gps_type.h"

#ifdef __cplusplus
  extern "C" {
#endif

extern INT32 (*gfpmtk_gps_sys_gps_mnl_callback )(MTK_GPS_NOTIFICATION_TYPE);
extern INT32 (*gfpmtk_gps_sys_nmea_output_to_app)(const char *,UINT32);
extern INT32 (*gfpmtk_gps_sys_frame_sync_enable_sleep_mode)(unsigned char);
extern INT32 (*gfpmtk_gps_sys_frame_sync_meas_req_by_network)(void);
extern INT32 (*gfpmtk_gps_sys_frame_sync_meas_req) (MTK_GPS_FS_WORK_MODE);
extern INT32 (*gfpmtk_gps_sys_agps_disaptcher_callback) (UINT16, UINT16, char *);
extern void  (*gfpmtk_gps_sys_pmtk_cmd_cb)(UINT16);
extern int   (*gfpSUPL_encrypt)(unsigned char *, unsigned char *, unsigned int );
extern int   (*gfpSUPL_decrypt)(unsigned char *, unsigned char *, unsigned int );

#if 1
extern INT32 (*gfmtk_gps_ofl_sys_rst_stpgps_req)(void);
extern INT32 (*gfmtk_gps_ofl_sys_submit_flp_data)(UINT8 *buf, UINT32 len);
#endif

typedef struct
{
    INT32  (*sys_gps_mnl_callback)(MTK_GPS_NOTIFICATION_TYPE );
    INT32  (*sys_nmea_output_to_app)(const char *,UINT32);
    INT32  (*sys_frame_sync_enable_sleep_mode)(unsigned char );
    INT32  (*sys_frame_sync_meas_req_by_network)(void );
    INT32  (*sys_frame_sync_meas_req)(MTK_GPS_FS_WORK_MODE );
    INT32  (*sys_agps_disaptcher_callback)(UINT16, UINT16, char * );
    void   (*sys_pmtk_cmd_cb)(UINT16 );
    int    (*encrypt)(unsigned char *, unsigned char *, unsigned int );
    int    (*decrypt)(unsigned char *, unsigned char *, unsigned int );
    #if 1
    INT32  (*ofl_sys_rst_stpgps_req)(void);
    INT32  (*ofl_sys_submit_flp_data)(UINT8 *buf, UINT32 len);
    #endif
} MTK_GPS_SYS_FUNCTION_PTR_T;

/*****************************************************************************
 * FUNCTION
 *  mtk_gps_sys_function_registery
 * DESCRIPTION
 *  register the body of function pointer
 * PARAMETERS
 *  fp_t     [IN]  function pointer API
 * RETURNS
 *  success(MTK_GPS_SUCCESS); failure (MTK_GPS_ERROR)
 *****************************************************************************/
INT32
mtk_gps_sys_function_register (MTK_GPS_SYS_FUNCTION_PTR_T *fp_t);

#ifdef __cplusplus
   }
#endif

#endif /* MTK_GPS_SYS_FP_H */
