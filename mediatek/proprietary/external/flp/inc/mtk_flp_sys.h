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
/*******************************************************************************
 * Filename:
 * ---------
 *  mtk_flp_sys.h
 *
 * Project:
 * --------
 *  FLP
 *
 * Description:
 * ------------
 *  Operation System Abstration Layer Implementation
 *
 * Author:
 * -------
 *  Hiki Chen, ext 25281, hiki.chen@mediatek.com, 2012-05-10
 *  Demarco Chou, demarco.chou@mediatek.com,      2014-03-22
 *
 *******************************************************************************/

#ifndef MTK_FLP_SYS_H
#define MTK_FLP_SYS_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Include
 *****************************************************************************/
#include "mtk_flp_sys_type.h"
#include <pthread.h>
#include <sys/time.h>

#define DEBUG_LOG

#define SUPPORT_BLOCKING_READ_MECHANISM

/*****************************************************************************
 * Define
 *****************************************************************************/
#define FLP_DLL_EXEC_SOCKET_CONN

#define MTK_FLP_TIMER_INVALID_ID    0xFFFFFFFF

#define MTK_FLP_MAX_ATR_LENGTH  (48)

/*****************************************************************************
 * FLP Return Value for APIs
 *****************************************************************************/
#define MTK_FLP_SUCCESS     (0)
#define MTK_FLP_ERROR       (-1)
#define MTK_FLP_TIMEOUT     (-2)

#define C_INVALID_TID  (-1)   /*invalid thread id*/
#define C_INVALID_FD   (-1)   /*invalid file handle*/
#define C_INVALID_SOCKET (-1)


/*****************************************************************************
 * FLP specific types
 *****************************************************************************/
typedef UINT32  FLPSTATUS;  /* FLP return values */
typedef unsigned int MTK_FLP_HANDLER;

typedef void (*ppCallBck_t)(UINT32 TimerId, void *pContext);

#define MTKFLP_COMM_SOCK_PATH   "/data/mtk_flp_server"


/*****************************************************************************
 * Data Structure
 *****************************************************************************/
typedef pthread_mutex_t MTK_FLP_MUTEX;
typedef pthread_cond_t MTK_FLP_EVENT;

typedef enum
{
    MTK_FLP_MUTEX_MAIN = 0x0,
    MTK_MUTEX_MSG_Q,
    MTK_MUTEX_MSG_CNT,
    MTK_MUTEX_SERVICE_MSG_Q,
    MTK_MUTEX_SOCKET_MSG_Q,
    MTK_FLP_MUTEX_TIMER,
    MTK_FLP_MUTEX_RESERVE_1,
    MTK_FLP_MUTEX_RESERVE_2,
    MTK_FLP_MUTEX_MAX_NUM
} MTK_FLP_MUTEX_E;


typedef enum
{
	MTK_FLP_EVENT_2HAL = 0,
	MTK_FLP_EVENT_2KER,
	MTK_FLP_EVENT_2DC,
	MTK_FLP_EVENT_END
} MTK_FLP_EVENT_E;

typedef enum
{
    MTK_FLP_TIMER_BOOT = 0x0,
    MTK_FLP_TIMER_MAX_NUM
} MTK_FLP_TIMER_E;


typedef enum
{
    MTK_FLP_TASKID_HAL = 0x0,
    MTK_FLP_TASKID_KERNEL,
    MTK_FLP_TASKID_DC,
    MTK_FLP_TASKID_SOCKET,
    MTK_FLP_TASKID_SENSOR,
    MTK_FLP_TASKID_CELL,
    MTK_FLP_TASKID_SCP,
    MTK_FLP_TASKID_WIFI,
    MTK_FLP_TASKID_END
} MTK_FLP_TASKID_E;



typedef struct
{
    UINT32 type;
    UINT32 length;
} MTK_FLP_MSG_T;

typedef struct
{
  UINT16    year;           /* years since 1900 */
  UINT8     month;          /* 0-11 */
  UINT8     mday;           /* 1-31 */
  UINT8     hour;           /* 0-23 */
  UINT8     min;            /* 0-59 */
  UINT8     sec;            /* 0-59 */
  UINT8     pad1;
  UINT16    msec;           /* 0-999 */
  UINT16    pad2;
} MTK_TIME_T;

typedef struct      // Ring buffer
{
    MTK_FLP_MSG_T** next_write;     // next position to write to
    MTK_FLP_MSG_T** next_read;      // next position to read from
    MTK_FLP_MSG_T** start_buffer;   // start of buffer
    MTK_FLP_MSG_T** end_buffer;     // end of buffer + 1
} MTK_FLP_MSG_RING_BUF;


#define MTK_FLP_MSG_RING_SIZE 16
typedef struct
{
	MTK_FLP_SIZE_T          size;
	MTK_FLP_MUTEX			mutex;
	MTK_FLP_EVENT			event;
	MTK_FLP_MSG_RING_BUF	msg_body;
	MTK_FLP_MSG_RING_BUF*  	msg_ring;
	MTK_FLP_MSG_T*			msg_buf[MTK_FLP_MSG_RING_SIZE];
	INT32					msg_cnt;
	INT32                   sockethandle;
}MTK_FLP_TASK_T;


typedef enum
{
    MTK_FLP_POWER_STATE_INIT = 0x0,
    MTK_FLP_POWER_STATE_ON,
    MTK_FLP_POWER_STATE_OFF,
    MTK_FLP_POWER_STATE_RESET,
    MTK_FLP_POWER_STATE_SUSPEND,
    MTK_FLP_POWER_STATE_STANDBY,
    MTK_FLP_POWER_STATE_MAX_NUM
} MTK_FLP_POWER_STATE_E;

typedef enum
{
    MTK_FLP_GPIO_EN_B = 0x0,
    MTK_FLP_GPIO_SYSRST_B,
    MTK_FLP_GPIO_EINT,
    MTK_FLP_GPIO_IRQ,
    MTK_FLP_GPIO_MAX_NUM
} MTK_FLP_GPIO_E;

typedef enum
{
    MTK_FLP_PULL_LOW  = 0x0,
    MTK_FLP_PULL_HIGH,
    MTK_FLP_PULL_INVALID,
} MTK_FLP_PULL_E;

typedef enum
{
    MTK_FLP_IOCTL_READ = 0x0,
    MTK_FLP_IOCTL_WRITE,
    MTK_FLP_IOCTL_MAX_NUM
} MTK_FLP_IOCTL_E;

typedef struct
{
    UINT32 pin;
    UINT32 highlow;
} MTK_FLP_IOCTL_ARG_T;

typedef enum
{
    MTK_FLP_FS_READ = 0,     // open file for reading (r)
    MTK_FLP_FS_WRITE,        // create file for writing, discard previous contents if any (w)
    MTK_FLP_FS_APPEND,       // open or create file for writing at end of file (a)
    MTK_FLP_FS_RW,           // open file for reading and writing (r+)
    MTK_FLP_FS_RW_DISCARD,   // create file for reading and writing, discard previous contents if any (w+)
    MTK_FLP_FS_RW_APPEND     // open or create file for reading and writing at end of file (a+)
}   MTK_FLP_FMODE;

typedef enum
{
    MTK_FLP_FS_SEEK_SET = 0, // seek from beginning of file
    MTK_FLP_FS_SEEK_CUR,     // seek from current position
    MTK_FLP_FS_SEEK_END      // seek from end of file
} MTK_FLP_FSEEK;




/*****************************************************************************
 * Extern Area
 *****************************************************************************/

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/
// - Memory
VOID *mtk_flp_sys_mem_alloc (UINT32 u4Size);
VOID mtk_flp_sys_mem_free (VOID *pMem);

// - Synchronization (Semaphore or Mutex)
INT32 mtk_flp_sys_mutex_create (MTK_FLP_MUTEX *mutex_id);
INT32 mtk_flp_sys_mutex_take (MTK_FLP_MUTEX *mutex_id);
INT32 mtk_flp_sys_mutex_give (MTK_FLP_MUTEX *mutex_id);
INT32 mtk_flp_sys_mutex_destory (MTK_FLP_MUTEX *mutex_id);

// - Task Communication
INT32 mtk_flp_sys_msg_initialize ();
MTK_FLP_MSG_T *mtk_flp_sys_msg_alloc (UINT16 u2Size);
INT32 mtk_flp_sys_msg_send (MTK_FLP_TASKID_E task_id, MTK_FLP_MSG_T *msg);
INT32 mtk_flp_sys_msg_recv (MTK_FLP_TASKID_E task_id, MTK_FLP_MSG_T **msg);
VOID mtk_flp_sys_msg_free (MTK_FLP_MSG_T *msg);
int mtk_flp_sys_socket_recv_msg(INT32 sockfd, MTK_FLP_MSG_T **p_msg);
INT32 mtk_flp_sys_socket_send(INT32 sockfd, MTK_FLP_MSG_T *p_msg);

// - Debug
VOID mtk_flp_sys_dbg_string (const CH *pString);
int mtk_flp_sys_dbg(int prior, const char *tag,  const char *fmt, ...);
VOID mtk_flp_sys_dbg_trace (UINT8 pData[], UINT32 u4Len);
UINT8 mtk_flp_sys_dbg_check_loc(MTK_FLP_LOCATION_T *loc);
UINT8 mtk_flp_sys_dbg_check_locs(MTK_FLP_LOCATION_T **locs, int num);
VOID mtk_flp_sys_dbg_dump_loc(MTK_FLP_LOCATION_T *loc);
VOID mtk_flp_sys_dbg_trx_to_file(BOOL fgIsTx, UINT8 pData[], UINT32  u4Len);
VOID mtk_flp_sys_dbg_to_file(const CH *data, ...);

// - Timer
INT32 mtk_flp_sys_timer_init ();
UINT32 mtk_flp_sys_timer_create (MTK_FLP_TIMER_E selected_timer);
VOID mtk_flp_sys_timer_start (UINT32 timer_slot, UINT32 period, ppCallBck_t timer_expiry, VOID *arg);
VOID mtk_flp_sys_timer_stop (UINT32 timer_slot);
VOID mtk_flp_sys_timer_delete (UINT32 timer_slot);
MTK_FLP_UTC_TIME mtk_flp_get_timestamp(struct timeval *ptv);
// - Sleep Function
VOID mtk_flp_sys_sleep (UINT32 u4MilliSeconds);

// - Assert Function
VOID mtk_flp_sys_assert ( INT32 value );

// - Time Utitiles
INT32 mtk_flp_sys_time_read (MTK_TIME_T* utctime);

// - Event
INT32 mtk_flp_sys_event_delete(MTK_FLP_EVENT *event);
INT32 mtk_flp_sys_event_create(MTK_FLP_EVENT *event);
INT32 mtk_flp_sys_event_set(MTK_FLP_EVENT *event);
INT32 mtk_flp_sys_event_wait(MTK_FLP_EVENT *event, MTK_FLP_MUTEX *mutex_idx);


// - Tasks
MTK_FLP_TASK_T *mtk_flp_find_task(MTK_FLP_TASKID_E id);
INT32 mtk_flp_is_socket_task(MTK_FLP_TASKID_E id);
INT32 mtk_flp_tasks_init(VOID);
INT32 mtk_flp_tasks_deinit(VOID);


#ifdef __cplusplus
   }  /* extern "C" */
#endif

#endif
