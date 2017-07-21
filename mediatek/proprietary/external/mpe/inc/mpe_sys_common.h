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
#ifndef MPE_COMMON_H
#define MPE_COMMON_H

#include <string.h>
#include <pthread.h>
#include <sys/un.h>
#include <sys/time.h>

#ifdef __cplusplus
#include <climits>   // C++ header
#else
#include <limits.h>  // C header
#endif

/****************************************************************************** 
* Define
******************************************************************************/   
#define C_INVALID_PID  -1   /*invalid process id*/
#define C_INVALID_TID  -1   /*invalid thread id*/
#define C_INVALID_FD   -1   /*invalid file handle*/

/* Return value for most APIs */
#define MPE_SUCCESS                 (0)
#define MPE_ERROR                   (-1)
#define MPE_NO_MSG_RECEIVED         (1)
#define MPE_INVALID                 (-1)
#define MPE_ZERO                    (0)
#define MPE_VALID                   (1)
#define INVALID_HANDLE_VALUE ((HANDLE) -1)
#define C_INVALID_SOCKET -1 /*invalid socket id*/

// define TRUE and FALSE
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

// Ensure NULL is defined
#ifdef NULL
#undef NULL
#endif
#define NULL 0

#define SECS_IN_WEEK (604800)             // Seconds in a GPS Week
#define SECS_IN_DAY  (86400)            // Seconds in a Day
#define DIFF_GPS_C_TIME ((365*10+2+5)*SECS_IN_DAY) // GPS epoch = 06Jan1980,  C epoch   = 01Jan1970,10years+leap years+,5 days Jan * sec in day

// Define real floating point number, single and double precision
typedef  float          R4;      // 4 byte floating point
typedef  double         R8;      // 8 byte floating point

// Define ASCII character type
typedef  char           CH;      // ASCII character
typedef unsigned char   U1;
typedef U1  BL;      // boolean logical (TRUE or FALSE only)
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
typedef size_t  MPE_SIZE_T;
typedef pthread_mutex_t MPE_MUTEX;
typedef pthread_cond_t MPE_EVENT;

/***************************************************************************** 
 * FLP specific types
 *****************************************************************************/
//Define debug buffer size
#define DEBUG_LOG
#define MAX_FMT_SIZE    1024
#define MPE_MSG_RING_SIZE 256
#define MTKMPE_COMM_SOCK_PATH   "/data/mtk_mpe_server"
#define MPE_SOCKET_CONN_NO  2
#define SE_max_data_size  400
/****************************************************************************** 
* enum
******************************************************************************/
typedef enum {
  MPE_THREAD_UNKNOWN             = -1,
  MPE_THREAD_SOCKET              = 0,
  MPE_THREAD_THREAD_NUM          = 1
}MPE_THREAD_ID_ENUM;

typedef enum {
  MPE_INIT_PASS = 0,
  MPE_INIT_MUTEX_FAILED,
  MPE_INIT_EVENT_FAILED,
  MPE_INIT_END
}MPE_INIT_STATUS;//CHANGE NAME:STATUS

typedef enum {
  MPE_EVENT_WAIT = 0,
  MPE_EVENT_END
}MPE_EVENT_ENUM;

// Task synchronization related type
typedef enum {
  MPE_MUTEX_MSG_Q =0,
  MPE_MUTEX_DATA = 1,
  MPE_MUTEX_END
}MPE_mutex_enum;

typedef enum MPE_LOG_PRIORITY_E {
  MPE_LOG_UNKNOWN = 0,
  MPE_LOG_DEFAULT,    /* only for SetMinPriority() */
  MPE_LOG_VERBOSE,
  MPE_LOG_DEBUG,
  MPE_LOG_INFO,
  MPE_LOG_WARN,
  MPE_LOG_ERROR,
  MPE_LOG_FATAL,
  MPE_LOG_SILENT,     /* only for SetMinPriority(); must be last */
}MPE_LOG_PRIORITY_E;

typedef enum {
  MPE_RUNTIME_MODE = 0,   //normal MPE mode
  MPE_REPLAY_RF_MODE,     //replay gps RF + recorded sensor
  MPE_GPS_ONLY_MODE,      //replay gps only mode without recorded sensor data
  MPE_ERROR_MODE = 99,
  MPE_OPERATION_MODE_END
}MPE_OPERATION_MODE;
/****************************************************************************** 
* structure
******************************************************************************/
typedef enum {
  MPE_TASKID_SOCKET = 0x0,
  MPE_TASKID_MAIN,
  MPE_TASKID_END
}MPE_TASKID_E;

// Event definition
typedef struct {
  mtk_uint32    type;           /* message ID */
  mtk_uint32    length;         /* length of 'data' */
}MPE_MSG;

typedef struct {
  MPE_MSG** next_write;     // next position to write to
  MPE_MSG** next_read;      // next position to read from
  MPE_MSG** start_buffer;   // start of buffer
  MPE_MSG** end_buffer;     // end of buffer + 1
}MPE_MSG_RING_BUF;

typedef struct {
  MPE_SIZE_T          size;
  MPE_MUTEX           mutex;
  MPE_EVENT           event;
  MPE_MSG_RING_BUF    msg_body;
  MPE_MSG_RING_BUF*   msg_ring;
  MPE_MSG*          msg_buf[MPE_MSG_RING_SIZE];
  mtk_int32         msg_cnt;
  mtk_int32         sockethandle;
}MPE_TASK_T;

/****************************************************************************** 
* functions Prototype
******************************************************************************/
void MPE_Msg_Ring_Buf_free( MPE_MSG *msg );
void MPE_Msg_Ring_Buf_Init(MPE_TASKID_E task_id);
void MPE_mem_free (void *pMem);
void* MPE_Mem_Alloc (mtk_uint16 size);
BL MPE_threads_create( void );
int MPE_threads_release(void);
BL MPE_task_init (void);
BL MPE_task_uninit(void);
mtk_int16 MPE_initialize_mutex(MPE_MUTEX *mutex);
mtk_int16 MPE_take_mutex( MPE_MUTEX *mutex );
mtk_int16 MPE_give_mutex( MPE_MUTEX *mutex );
mtk_int16 MPE_destroy_mutex( MPE_MUTEX *mutex );
mtk_int16 MPE_event_create( MPE_EVENT *event );
mtk_int16 MPE_event_set( MPE_EVENT *event );
mtk_int16 MPE_event_wait( MPE_EVENT *event, MPE_MUTEX *mutex );
mtk_int16 MPE_event_delete( MPE_EVENT *event );
mtk_int16 MPE_sys_dbg(int prior, const char *tag,  const char *fmt, ...);
mtk_uint32 MPE_sys_socket_send(mtk_int32 sockfd, MPE_MSG *p_msg, struct sockaddr_un sock_addr);
mtk_int32 MPE_sys_socket_read(mtk_int32 sockfd, mtk_uint8*pRecvBuff, mtk_int32 i4RecvLen, struct sockaddr_un sock_addr);
//int MPE_sys_socket_recv(mtk_int32 sockfd, MPE_MSG **p_msg, struct sockaddr_un sock_addr);
int MPE_sys_socket_recv_msg(mtk_int32 sockfd[MPE_SOCKET_CONN_NO], struct sockaddr_un sock_addr[MPE_SOCKET_CONN_NO], char sockbuf[128]);
mtk_int32 MPE_sys_socket_check_fd(mtk_int32 sockfd[2]);
mtk_int32 MPE_sys_scan_socket(int sock_fd_mnl, int sock_fd_flp);
mtk_int32 MPE_sys_msg_recv ( MPE_TASKID_E task_id, MPE_MSG **msg);
mtk_int32 MPE_sys_msg_send ( MPE_TASKID_E task_id, MPE_MSG *msg);
MPE_TASK_T *MPE_find_task(MPE_TASKID_E id);
mtk_int32 mktimeFromGPS ( mtk_uint32 gps_sec);
void mktimeToGPS ( time_t Ctime, mtk_int16 *gpsWeekNo, mtk_uint32 *gpsITOW );// GPS seconds in week range 0-604800
void get_time_stamp(double* timetag, mtk_int32 leap_sec);
extern void mpe_uninit_sensor( void );
extern void mpe_run_algo( double input_time,  mtk_uint16 data_cnt);

#endif //#ifndef MPE_COMMON_H
