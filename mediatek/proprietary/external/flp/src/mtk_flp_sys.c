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
 *  mtk_flp_osal.c
 *
 * Project:
 * --------
 *
 * Description:
 * ------------
 *
 * Author:
 * -------
 *  Hiki Chen, ext 25281, hiki.chen@mediatek.com, 2012-05-10
 *
 *******************************************************************************/
/*****************************************************************************
 * Include
 *****************************************************************************/
#ifdef WIN32
#include <windows.h>
#include <assert.h>
#endif

#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/time.h>
//#include <sys/timeb.h>
#include <sys/ioctl.h>
#include <sys/un.h>

#include <stdarg.h>

#include <utils/Log.h> // For Debug

#include "mtk_flp_sys_type.h"
#include "mtk_flp_sys.h"
//#include <android/log.h>


/*****************************************************************************
 * Define
 *****************************************************************************/
//#define USE_SIGNAL_EVENT_TO_TIMER_CREATE

#define CLOCKID CLOCK_REALTIME
#define SIG SIGRTMIN

#define DEBUG_LOG

#ifdef DEBUG_LOG

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "FLP_SYS"
#endif


#define FLPE(...)		mtk_flp_sys_dbg(MTK_FLP_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define FLPD(...)		mtk_flp_sys_dbg(MTK_FLP_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define FLPW(...)		mtk_flp_sys_dbg(MTK_FLP_LOG_WARN, LOG_TAG, __VA_ARGS__)


#else
#define FLPD(...)
#define FLPW(...)
#define FLPE(...)
#endif

/*****************************************************************************
 * Data Structure
 *****************************************************************************/
#ifdef WIN32
typedef struct
{
    CRITICAL_SECTION    cs;
    BOOL                is_used;    // 1 = used; 0 = unused
    UINT32              timer_id;   //timer's id returned from SetTimer()
    ppCallBck_t         timer_expiry_callback;
    VOID                *timer_expiry_context;
} flp_timer_table_struct;
#else // for linux
typedef struct
{
    BOOL                is_used;    // 1 = used; 0 = unused
    timer_t             handle;     // system timer handle
    ppCallBck_t         timer_expiry_callback; // timeout callback
    VOID                *timer_expiry_context; // timeout callback context
    BOOL                is_stopped;    // 1 = stopped; 0 = running
} flp_timer_table_struct;
#endif


/*****************************************************************************
 * Extern Area
 *****************************************************************************/
extern int gconn_fd_tmp;
extern int gInterfaceHandle;

#define FLP_MSG_HDR_SIZE        sizeof(MTK_FLP_MSG_T)

/*****************************************************************************
 * GLobal Variable
 *****************************************************************************/
static MTK_FLP_TASK_T	gFlpTasks[MTK_FLP_TASKID_END];

// timer pool
static flp_timer_table_struct flp_timer_table[MTK_FLP_TIMER_MAX_NUM];

//static pthread_mutex_t g_hMutex[MTK_FLP_MUTEX_MAX_NUM];

MTK_FLP_MSG_RING_BUF flp_main_msg_ring_body;
MTK_FLP_MSG_RING_BUF * flp_main_msg_ring = NULL;
MTK_FLP_MSG_T * flp_main_msg_ring_buffer[MTK_FLP_MSG_RING_SIZE]; //pointer array
INT32 flp_main_msg_cnt;

MTK_FLP_MSG_RING_BUF flp_service_msg_ring_body;
MTK_FLP_MSG_RING_BUF * flp_service_msg_ring = NULL;
MTK_FLP_MSG_T * flp_service_msg_ring_buffer[MTK_FLP_MSG_RING_SIZE]; //pointer array
INT32 flp_service_msg_cnt;





/*****************************************************************************
 * Function
 *****************************************************************************/
#if 0
VOID *mtk_flp_sys_i2c_init(INT32 type);
INT32 mtk_flp_sys_i2c_read(UINT8* data, UINT16 len);
INT32 mtk_flp_sys_i2c_write(UINT8* data, UINT16 len);
VOID mtk_flp_sys_i2c_uninit(VOID *);
#endif

INT32 mtk_flp_sys_socket_send(INT32 sockfd, MTK_FLP_MSG_T *p_msg)
{
   INT32 i4SendLen;

   FLPD("Socket fd:%d..\n", sockfd);

   if (p_msg == NULL)
   {
      FLPE("socket send fail: due to msg == null\n");
      return -1;
   }

   if (sockfd < 0)
   {
      FLPW("socket send msg fail: due to invalid sockfd: %d\n", sockfd);
      return -1;
   }

   FLPD("socket send msg: type:0x%02x, length, %d\n", p_msg->type, p_msg->length);

#if 1   //trace location
    if( (p_msg->type == MTK_FLP_MSG_HAL_REPORT_LOC_NTF) || (p_msg->type == MTK_FLP_MSG_KER_REPORT_LOC_NTF) || (p_msg->type == MTK_FLP_MSG_DC_REPORT_LOC_NTF)){
        if( mtk_flp_sys_dbg_check_loc( (UINT8*)p_msg + sizeof(MTK_FLP_MSG_T) ) == FALSE ){
            FLPE("Sending socket with wrong location data");
        }
    }
#endif


   i4SendLen = send(sockfd, (void *)p_msg, sizeof(MTK_FLP_MSG_T) + p_msg->length ,0);
   if (i4SendLen < 0)
   {
      FLPW("socket fd:%d send fail: %d, %s\n", sockfd, errno, strerror(errno));
   }
   else
   {
      FLPD("Socket fd:%d send ok (send len: %d)\n", sockfd, i4SendLen);
   }
   mtk_flp_sys_mem_free(p_msg);

   return i4SendLen;
}

static INT32 mtk_flp_sys_socket_read(INT32 sockfd, UINT8 *pRecvBuff, INT32 i4RecvLen)
{
   INT32 i4ReadLen;
   INT32 i4TotalReadLen;

   //FLPD("mtk_flp_sys_socket_read...(fd:%d, pRecvBuff:0x%x, i4RecvLen:%d)\n", sockfd, (UINT32)pRecvBuff, i4RecvLen);

   if (sockfd < 0)
   {
      FLPE("socket recv msg fail: due to invalid sockfd: %d\n", sockfd);
      return -1;
   }

   i4TotalReadLen = 0;
   while (i4TotalReadLen < i4RecvLen)
   {
      i4ReadLen = read(sockfd, pRecvBuff, i4RecvLen - i4TotalReadLen);
      FLPD("i4ReadLen (%d)\n", i4ReadLen);
      if (i4ReadLen < 0)
      {
         FLPW("socket read fail: %d, %s\n", errno, strerror(errno));
         i4TotalReadLen = i4ReadLen; // keep read fail return value
         break; // exit loop
      }
      else if (i4ReadLen == 0)
      {
         FLPW("socket read fail due to socket be closed\n");
         i4TotalReadLen = i4ReadLen; // keep read fail return value
         break; // exit loop
      }
      else
      {
         i4TotalReadLen += i4ReadLen;
      }

      FLPD("mtk_flp_sys_socket_read ok (read len: %d, target len: %d)\n", i4TotalReadLen, i4RecvLen);
   }

   return i4TotalReadLen;
}

int mtk_flp_sys_socket_recv_msg(INT32 sockfd, MTK_FLP_MSG_T **p_msg)
{
   int i4ReadLen = 0;
   MTK_FLP_MSG_T msg_hdr;
   void *p_msg_body;
   unsigned char *pBuff;

   FLPD("Socket fd[%d] receiving...\n", sockfd);

   // read msg header (blocking read)
   pBuff = (unsigned char *)&msg_hdr;
   i4ReadLen = mtk_flp_sys_socket_read(sockfd, pBuff, FLP_MSG_HDR_SIZE);
   if (i4ReadLen <= 0) // error case
   {
      return MTK_FLP_ERROR;
   }
   else if (FLP_MSG_HDR_SIZE != i4ReadLen)
   {
      FLPD("unexpected length (hdr len: %d, read len: %d)\n", FLP_MSG_HDR_SIZE, i4ReadLen);
      return MTK_FLP_ERROR;
   }
   else
   {
      FLPD("msg hdr (type: 0x%02x, len: %d)\n", msg_hdr.type, msg_hdr.length);

      // malloc msg
      *p_msg = (MTK_FLP_MSG_T *)mtk_flp_sys_mem_alloc(FLP_MSG_HDR_SIZE + msg_hdr.length);
      if (*p_msg == NULL)
      {
         FLPD("malloc fail\n");
         return MTK_FLP_ERROR;
      }

      // fill type & length
      memcpy((unsigned char *)*p_msg, (unsigned char *)&msg_hdr, FLP_MSG_HDR_SIZE);
   }

   // read msg body (blocking read)
   if (msg_hdr.length > 0)
   {
      p_msg_body = (unsigned char *)*p_msg + FLP_MSG_HDR_SIZE;
      pBuff = (unsigned char *)p_msg_body;
      i4ReadLen = mtk_flp_sys_socket_read(sockfd, pBuff, msg_hdr.length);
      if (i4ReadLen <= 0) // error case
      {
         FLPD("read error (%d)\n", i4ReadLen);
         mtk_flp_sys_mem_free(*p_msg);
         *p_msg = NULL;
         return MTK_FLP_ERROR;
      }
      else if (msg_hdr.length != (UINT32)i4ReadLen)
      {
         FLPD("unexpected length (body len: %d, read len %d)\n", msg_hdr.length, i4ReadLen);
         mtk_flp_sys_mem_free(*p_msg);
         *p_msg = NULL;
         return MTK_FLP_ERROR;
      }
   }

   FLPD("Socket fd[%d] recv ok\n", sockfd);

   return MTK_FLP_SUCCESS;
   }


/*****************************************************************************
 * Function
 *  mtk_flp_sys_mem_alloc
 * DESCRIPTION
 *  Allocate a block of memory
 * PARAMETERS
 *  size [IN] the length of the whole memory to be allocated
 * RETURNS
 *  On success, return the pointer to the allocated memory
 * NULL (0) if failed
 *****************************************************************************/
VOID *
mtk_flp_sys_mem_alloc (
    UINT32 u4Size
)
{
    void *pMem = NULL;

    if (u4Size != 0)
    {
        pMem = malloc(u4Size);
        if( pMem == NULL ){
            FLPD("MEMORY ALLOCATION ERROR!!!!!");
        }
    }

    return pMem;

}

/*****************************************************************************
 * Function
 *  mtk_flp_sys_mem_free
 * DESCRIPTION
 *  Release unused memory
 * PARAMETERS
 *  pMem        [IN] the freed memory address
 * RETURNS
 *  NONE
 *****************************************************************************/
VOID
mtk_flp_sys_mem_free (
    VOID *pMem
)
{
   if (pMem != NULL)
   {
       //FLPD("FLP_EXP_FREE: 0x%08X, %d",pMem);
       free(pMem);
       pMem=NULL;
   }

   return;
}

/*****************************************************************************
 * Function
 *  mtk_flp_sys_mutex_create
 * DESCRIPTION
 *  Create a mutex object
 * PARAMETERS
 *  mutex_id    [IN] mutex index used by FLP library
 * RETURNS
 *  MTK_FLP_SUCCESS
 *****************************************************************************/
INT32
mtk_flp_sys_mutex_create (
    MTK_FLP_MUTEX *mutex
)
{
    pthread_mutex_init(mutex, NULL);

    return MTK_FLP_SUCCESS;
}

/*****************************************************************************
 * Function
 *  mtk_flp_sys_mutex_take
 * DESCRIPTION
 *  Request ownership of a mutex and if it's not available now, then block the
 *  thread execution
 * PARAMETERS
 *  mutex_id    [IN] mutex index used by FLP library
 * RETURNS
 *  MTK_FLP_SUCCESS
 *****************************************************************************/
INT32
mtk_flp_sys_mutex_take (
    MTK_FLP_MUTEX *mutex
)
{
    //FLPD("MUTEX_TAKE[%x]", mutex);
    pthread_mutex_lock(mutex);
    //FLPD("MUTEX_TAKE[%x]done", mutex);

    return MTK_FLP_SUCCESS;
}

/*****************************************************************************
 * Function
 *  mtk_flp_sys_mutex_give
 * DESCRIPTION
 *  Release a mutex ownership
 * PARAMETERS
 *  mutex_id    [IN] mutex index used by FLP library
 * RETURNS
 *  MTK_FLP_SUCCESS
 *****************************************************************************/
INT32
mtk_flp_sys_mutex_give (
		MTK_FLP_MUTEX *mutex
)
{
    //FLPD("MUTEX_GIVE[%x]", mutex);
	pthread_mutex_unlock(mutex);
    //FLPD("MUTEX_GIVE[%x]done", mutex);

    return MTK_FLP_SUCCESS;
}

/*****************************************************************************
 * Function
 *  mtk_flp_sys_mutex_destory
 * DESCRIPTION
 *  Destory a mutex object
 * PARAMETERS
 *  mutex_id    [IN] mutex index used by FLP library
 * RETURNS
 *  MTK_FLP_SUCCESS
 *****************************************************************************/
INT32
mtk_flp_sys_mutex_destory (
    MTK_FLP_MUTEX *mutex
)
{
    if (pthread_mutex_destroy(mutex))
    {
        return MTK_FLP_ERROR;
    }

    return MTK_FLP_SUCCESS;
}

/*****************************************************************************
 * Function
 *  mtk_flp_sys_mutex_deinitialize
 * DESCRIPTION
 *  mutex deinitialization
 * PARAMETERS
 *  void
 * RETURNS
 *  MTK_FLP_SUCCESS
 *****************************************************************************/
INT32
mtk_flp_sys_mutex_deinitialize (
    void
)
{
    return MTK_FLP_SUCCESS;
}

/*****************************************************************************
 * Function
 *  mtk_flp_sys_msg_alloc
 * DESCRIPTION
 *  Allocate a block of memory for message
 * PARAMETERS
 *  u2Size      [IN] the length of the whole MTK_FLP_MSG structure
 * RETURNS
 *  Pinter to the created message if successed
 *  NULL (0) if failed
 *****************************************************************************/
MTK_FLP_MSG_T *
mtk_flp_sys_msg_alloc (
    UINT16 u2Size
)
{
    //FLPD("FLP_EXP_ALOC: 0x%08X, %d", mtk_flp_sys_mem_alloc(u2Size));
    return mtk_flp_sys_mem_alloc(u2Size);
}

/*****************************************************************************
 * Function
 *  mtk_flp_sys_msg_initialize
 * DESCRIPTION
 *  Send a message to a task
 * PARAMETERS
 *  task_id     [IN] target task id
 *  msg         [IN] the send message
 * RETURNS
 *  MTK_FLP_SUCCESS
 *****************************************************************************/
INT32
mtk_flp_sys_msg_initialize (
    MTK_FLP_TASKID_E	task_id
)
{
	MTK_FLP_TASK_T *task = mtk_flp_find_task(task_id);

	task->msg_ring = &(task->msg_body);
	task->msg_ring->start_buffer = &task->msg_buf[0];
	task->msg_ring->end_buffer = &task->msg_buf[MTK_FLP_MSG_RING_SIZE-1];
	task->msg_ring->next_write = task->msg_ring->start_buffer;
	task->msg_ring->next_read = task->msg_ring->start_buffer;
	task->msg_cnt = 0;

    return MTK_FLP_SUCCESS;
}


/*****************************************************************************
 * Function
 *  mtk_flp_sys_msg_send
 * DESCRIPTION
 *  Send a message to a task
 * PARAMETERS
 *  task_id     [IN] target task id
 *  msg         [IN] the send message
 * RETURNS
 *  MTK_FLP_SUCCESS
 *****************************************************************************/
INT32
mtk_flp_sys_msg_send (
    MTK_FLP_TASKID_E task_id,
    MTK_FLP_MSG_T *msg
)
{
    MTK_FLP_LOCATION_T *loc;
	MTK_FLP_TASK_T *task = mtk_flp_find_task(task_id);


    //FLPD("Send message target id:%d msg(%x)\n",task_id, (unsigned int)msg);
#if 1   //trace location
    loc = (MTK_FLP_LOCATION_T *)(((INT8*)msg)+sizeof(MTK_FLP_MSG_T));
    if( (msg->type == MTK_FLP_MSG_HAL_REPORT_LOC_NTF) || (msg->type == MTK_FLP_MSG_KER_REPORT_LOC_NTF) || (msg->type == MTK_FLP_MSG_DC_REPORT_LOC_NTF)){
        if( mtk_flp_sys_dbg_check_loc( loc ) == FALSE  ){
            FLPE("Sending wrong location data");
        }
    }
#endif
	if(task==NULL)
	{
        FLPD("Task id error:%d\n",task_id);
        return MTK_FLP_ERROR;
	}

    if (msg == NULL)
    {
        FLPD("Send message full:%d\n",task_id);
        return MTK_FLP_ERROR;
    }


	mtk_flp_sys_mutex_take(&task->mutex);

    if( mtk_flp_is_socket_task(task_id))         //socket thread.
    {
        int ret;
        if( task->sockethandle ){
            ret = mtk_flp_sys_socket_send(task->sockethandle, msg);
        }else{
            FLPD("task:%d not connected(%d)",task_id, task->sockethandle);
            ret = MTK_FLP_ERROR;
        }
        mtk_flp_sys_mutex_give(&task->mutex);
        return ret;
    }


	/* msg queue full check */
	if (task->msg_cnt== MTK_FLP_MSG_RING_SIZE)
	{
		mtk_flp_sys_mutex_give(&task->mutex);
		FLPD("Send message to ID[%d], full\n", task_id);
		return MTK_FLP_ERROR;
	}

	if ( task->msg_ring!= NULL)
	{
		*(task->msg_ring->next_write) = (MTK_FLP_MSG_T*)msg;

		task->msg_ring->next_write++;

		// Wrap check the input circular buffer
		if ( task->msg_ring->next_write > task->msg_ring->end_buffer )
		{
			task->msg_ring->next_write = task->msg_ring->start_buffer;
		}

		task->msg_cnt++;

		FLPD("mtk_flp_sys_event_sent.. to [%d]\n", task_id);
		if (MTK_FLP_SUCCESS != mtk_flp_sys_event_set( &(task->event)))
		{
			FLPD("mtk_flp_sys_event_set to [%d],fail\n", task_id);
			mtk_flp_sys_mutex_give(&task->mutex);
			return MTK_FLP_ERROR;
		}

		mtk_flp_sys_mutex_give(&task->mutex);
		return MTK_FLP_SUCCESS;
	}
	else
	{
		FLPD("message ring not set!!!\n");
		mtk_flp_sys_mutex_give(&task->mutex);
		return MTK_FLP_ERROR;
	}

    return MTK_FLP_ERROR;
}

/*****************************************************************************
 * Function
 *  mtk_flp_sys_msg_recv
 * DESCRIPTION
 *  Recv a message from a task
 * PARAMETERS
 *  task_id     [IN] target task id
 *  msg         [IN] the receive message pointer
 * RETURNS
 *  MTK_FLP_SUCCESS
 *****************************************************************************/
INT32
mtk_flp_sys_msg_recv (
    MTK_FLP_TASKID_E task_id,
    MTK_FLP_MSG_T **msg
)
{
	MTK_FLP_TASK_T *task = mtk_flp_find_task(task_id);


    if( mtk_flp_is_socket_task(task_id) )      //socket thread operations
    {
        if(task->sockethandle > 0){
            FLPD("TaskID:[%d][%d] waiting socket [%d]... \n", task_id, getpid(), task->sockethandle);
            mtk_flp_sys_socket_recv_msg(task->sockethandle, msg);
            return MTK_FLP_SUCCESS;
        }else{
            FLPD("TaskID:[%d] socket not connected... \n", task_id);
            return MTK_FLP_ERROR;
        }
    }

    FLPD("TaskID:[%d] receiving from queue(%p)", task_id, task->msg_ring->next_read);
    if (msg != NULL)
    {
         mtk_flp_sys_mutex_take(&task->mutex);

         /* wait signal if no msg in queue */
         if ( task->msg_cnt<= 0 )
         {
             FLPD("TaskID:[%d][%d] waiting... \n", task_id, getpid());
             if (MTK_FLP_SUCCESS != mtk_flp_sys_event_wait(&task->event, &task->mutex))
             {
                 FLPD("mtk_flp_sys_event_wait, ID:%d,fail\n", task_id);
                 mtk_flp_sys_mutex_give(&task->mutex);
                 return MTK_FLP_ERROR;
             }
             else
             {
                 FLPD("mtk_flp_sys_event_wait,ok, cnt,%d\n", task->msg_cnt);
             }
         }else{
             FLPD("TaskID[%d] has %d events in queue", task_id, task->msg_cnt);
         }

         if (*(task->msg_ring->next_read) == NULL)
         {
             mtk_flp_sys_mutex_give(&task->mutex);
             return MTK_FLP_ERROR;
         }
        (*msg) = *(task->msg_ring->next_read);


        task->msg_ring->next_read++;

        // Wrap check output circular buffer
        if ( task->msg_ring->next_read > task->msg_ring->end_buffer )
        {
            task->msg_ring->next_read = task->msg_ring->start_buffer;
        }

        task->msg_cnt--;

        mtk_flp_sys_mutex_give(&task->mutex);

        return MTK_FLP_SUCCESS;

	}
	else
	{
   		FLPD("mtk_flp_sys_msg_recv: NULL recv message pointer");
		return MTK_FLP_ERROR;
	}
    return MTK_FLP_SUCCESS;
}

/*****************************************************************************
 * Function
 *  mtk_flp_sys_msg_free
 * DESCRIPTION
 *  Free a block of memory for message
 * PARAMETERS
 *  msg         [IN] the freed message
 * RETURNS
 *  NONE
 *****************************************************************************/
VOID
mtk_flp_sys_msg_free (
    MTK_FLP_MSG_T *msg
)
{
    mtk_flp_sys_mem_free(msg);
}


/*****************************************************************************
 * Function
 *  mtk_flp_sys_dbg_string
 * DESCRIPTION
 *  Output a given string
 * PARAMETERS
 *  pString     [IN] pointer to buffer content to be displayed
 * RETURNS
 *  NONE
 *****************************************************************************/
VOID
mtk_flp_sys_dbg_string (
    const CH *pString
)
{
    FLPD("%s", pString);
    return;
}


#define MAX_FMT_SIZE    1024
void format_string(char *fmt,va_list argptr, char *formatted_string);

int mtk_flp_sys_dbg(int prior, const char *tag,  const char *fmt, ...)
{
    char formatted_string[MAX_FMT_SIZE];

    va_list argptr;
    va_start(argptr,fmt);
    __android_log_vprint(ANDROID_LOG_ERROR, tag, fmt, argptr);
    va_end(argptr);
    return 0;

}


/*****************************************************************************
 * Function
 *  mtk_flp_sys_dbg_trace
 * DESCRIPTION
 *  Output the traced raw data
 * PARAMETERS
 *  pString     [IN] data Data block
 *  length      [IN] size buffer size of the data block
 * RETURNS
 *  NONE
 *****************************************************************************/
VOID
mtk_flp_sys_dbg_trace (
    UINT8   pData[],
    UINT32  u4Len
)
{
    UINT32 i;

    for (i = 0; i < u4Len; i++)
    {
        FLPD("%02X,",*(pData+i));
    }

    return;
}




/*
    return TRUE for passing the checks, return FALSE otherwise.
*/
UINT8 mtk_flp_sys_dbg_check_loc(MTK_FLP_LOCATION_T *loc)
{
    if( loc->size < sizeof(MTK_FLP_LOCATION_T) ){
        FLPE("Wrong location size:%d, %d", loc->size, sizeof(MTK_FLP_LOCATION_T));
        return FALSE;
    }
    if(loc->longitude>180 || loc->longitude < -180 || loc->latitude >90 || loc->latitude<-90){
        FLPE("Wrong location value, longitude:%f, latitude:%f", loc->longitude, loc->latitude);
        return FALSE;
    }

    return TRUE;

}
UINT8 mtk_flp_sys_dbg_check_locs(MTK_FLP_LOCATION_T **locs, int num)
{
    int i;
    for(i=0;i<num;i++){
        if( mtk_flp_sys_dbg_check_loc(locs[i]) == FALSE){
            return FALSE;
        }
    }
    return TRUE;
}

VOID mtk_flp_sys_dbg_dump_loc(MTK_FLP_LOCATION_T *loc)
{
    FLPD("Location(%x):LNG:%f LAT:%f ALT:%f ACC:%f SPD:%f BEARING:%f, FLAGS:%04X SOURCES:%08X Timestamp:%lld",
    loc, loc->longitude, loc->latitude, loc->altitude, loc->accuracy,
    loc->speed, loc->bearing, loc->flags, loc->sources_used, loc->timestamp);

    if( mtk_flp_sys_dbg_check_loc(loc) != TRUE ){
        FLPD("ERROR dumping location!!!!");
        mtk_flp_sys_assert(FALSE);
        return;
    }
}


/*****************************************************************************
 * Function
 *  mtk_flp_sys_sleep
 * DESCRIPTION
 *  task sleep funciton
 * PARAMETERS
 *  pString     [IN] data Data block
 *  length      [IN] size buffer size of the data block
 * RETURNS
 *  VOID
 *****************************************************************************/
VOID
mtk_flp_sys_sleep (
    UINT32 u4MilliSeconds
)
{
    usleep(1000*u4MilliSeconds);
}

VOID
mtk_flp_sys_assert (INT32 value)
{
    if( value ){
    }
    else{
        __android_log_assert( "FAILED", "FLP_SYS", "FLP ASSERTED");
    }
}

#ifdef WIN32 // PHY layer
/* ***************************************************************************
Physical Link Function
    gLinkFunc.init  = mtkNfcDal_uart_init;
    gLinkFunc.open = mtkNfcDal_uart_open;
    gLinkFunc.close = mtkNfcDal_uart_close;
    gLinkFunc.read  = mtkNfcDal_uart_read;
    gLinkFunc.write  = mtkNfcDal_uart_write;
    gLinkFunc.flush  = mtkNfcDal_uart_flush;
    gLinkFunc.reset = mtkNfcDal_chip_reset;GPIO control for FLP pins
UART
    void mtkNfcDal_uart_init (void);
    FLPSTATUS mtkNfcDal_uart_open (const char* deviceNode, void ** pLinkHandle)
    int mtkNfcDal_uart_read (uint8_t * pBuffer, int nNbBytesToRead);
    int mtkNfcDal_uart_write (uint8_t * pBuffer, int nNbBytesToWrite);
    void mtkNfcDal_uart_flush (void);
    void mtkNfcDal_uart_close (void);
GPIO
    int mtkNfcDal_chip_reset (int level);
 ************************************************************************** */

extern HANDLE g_hUart;

// UART settings for Windows UART driver
#define FLP_UART_BAUD                   (115200)
#define FLP_UART_BUF_TX                 (1024)
#define FLP_UART_BUF_RX                 (1024)

VOID *
mtk_flp_sys_uart_init (
    const CH* strDevPortName
)
{
    HANDLE hUARTHandle = INVALID_HANDLE_VALUE;

    hUARTHandle = CreateFile(strDevPortName, GENERIC_READ | GENERIC_WRITE,
                  0, NULL, OPEN_EXISTING, 0, NULL);

    if (INVALID_HANDLE_VALUE != hUARTHandle)
    {
        DCB dcb;
        BOOL fSuccess;

        fSuccess = GetCommState(hUARTHandle, &dcb);
        if (fSuccess)
        {
            dcb.BaudRate = FLP_UART_BAUD;
            dcb.ByteSize = 8;
            dcb.Parity = NOPARITY;
            dcb.StopBits = ONESTOPBIT;
            dcb.fOutxDsrFlow = FALSE;
            dcb.fOutxCtsFlow = FALSE;
            dcb.fDtrControl = DTR_CONTROL_DISABLE;
            dcb.fRtsControl = RTS_CONTROL_ENABLE;
            dcb.fInX = FALSE;           // No Xon/Xoff flow control
            dcb.fOutX = FALSE;
            dcb.fBinary = TRUE;
            dcb.fAbortOnError = FALSE;  // Do not abort reads/writes on error
            dcb.fErrorChar = FALSE;     // Disable error replacement
            dcb.fNull = FALSE;          // Disable null stripping

            fSuccess = SetCommState(hUARTHandle, &dcb);

            if (fSuccess)
            {
                COMMTIMEOUTS timeouts;

                // setup device buffer
                SetupComm(hUARTHandle, FLP_UART_BUF_RX, FLP_UART_BUF_TX);

                // setup timeout
                GetCommTimeouts(hUARTHandle, &timeouts);
                timeouts.ReadIntervalTimeout = MAXDWORD;
                timeouts.ReadTotalTimeoutConstant = 0;
                timeouts.ReadTotalTimeoutMultiplier = 0;
                timeouts.WriteTotalTimeoutConstant = 0;
                timeouts.WriteTotalTimeoutMultiplier = 0;
                SetCommTimeouts(hUARTHandle, &timeouts);
            }
        }

        if (!fSuccess)
        {
            CloseHandle(hUARTHandle);
            hUARTHandle = INVALID_HANDLE_VALUE;
        }
    }

    return hUARTHandle;
}

INT32
mtk_flp_sys_uart_read (
//    VOID *pLinkHandle,
    UINT8 *pBuffer,
    UINT32 nNbBytesToRead
)
{
    DWORD dwRead = 0;

    if (INVALID_HANDLE_VALUE != g_hUart)
    {
        if (ReadFile(g_hUart, pBuffer, nNbBytesToRead, (LPDWORD)&dwRead, NULL))
        {
            // read success - one shot read and return
        }
        else
        {
            //assert(0);
            dwRead = -1;
        }
    }
    else
    {
        mtk_flp_sys_dbg_string("UART Handle is invalid\r\n");
        dwRead = -2;
    }

    return dwRead;
}

INT32
mtk_flp_sys_uart_write (
//    VOID *pLinkHandle,
    UINT8 *pBuffer,
    UINT32 nNbBytesToWrite
)
{
    DWORD dwWritten;
    UINT32 u4Offset = 0;

    mtk_flp_sys_dbg_string("            ---> PHY TX: ");
    mtk_flp_sys_dbg_trace(pBuffer, nNbBytesToWrite);
    mtk_flp_sys_dbg_string("\r\n");

    if (INVALID_HANDLE_VALUE != g_hUart)
    {
        while (u4Offset < nNbBytesToWrite)
        {
            if (WriteFile(g_hUart, &pBuffer[u4Offset], nNbBytesToWrite - u4Offset, &dwWritten, NULL))
            {
                // write success - continuely write if the write data is not completed
                u4Offset += dwWritten;
            }
            else
            {
                //assert(0);
                break;
            }
        }
    }
    else
    {
        mtk_flp_sys_dbg_string("UART Handle is invalid\r\n");
    }

    return dwWritten;
}

VOID
mtk_flp_sys_uart_flush (
    VOID *pLinkHandle
)
{
    // purge any information in buffer
    PurgeComm(pLinkHandle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
}

// uninit UART
VOID
mtk_flp_sys_uart_uninit (
    VOID *pLinkHandle
)
{
    if (INVALID_HANDLE_VALUE != pLinkHandle)
    {
        CloseHandle(pLinkHandle);
    }
}
#endif


VOID
mtk_flp_sys_dbg_trx_to_file(
    BOOL    fgIsTx,
    UINT8   pData[],
    UINT32  u4Len
)
{


}


#if 0
/*****************************************************************************
 * FUNCTION
 *  mtk_flp_sys_interface_init
 * DESCRIPTION
 *  Initialize communication interface between DH and FLPC
 * PARAMETERS
 *  strDevPortName      [IN] Device Name
 *  i4Baud              [IN] Baudrate
 * RETURNS
 *  NONE
 *****************************************************************************/
VOID *
mtk_flp_sys_interface_init (
    const CH* strDevPortName,
    const INT32 i4Baud
)
{
#ifdef WIN32
    return mtk_flp_sys_uart_init(strDevPortName, i4Baud);
#else
    return mtk_flp_sys_i2c_init(0);
#endif
}

/*****************************************************************************
 * FUNCTION
 *  mtk_flp_sys_interface_read
 * DESCRIPTION
 *  Read data from FLPC
 * PARAMETERS
 *  pBuffer             [IN] read buffer
 *  nNbBytesToRead      [IN] number of bytes to read
 * RETURNS
 *  number of bytes read
 *****************************************************************************/
INT32
mtk_flp_sys_interface_read (
    UINT8 *pBuffer,
    UINT16 nNbBytesToRead
)
{
#ifdef WIN32
    return mtk_flp_sys_uart_read(pBuffer, nNbBytesToRead);
#else //
   return mtk_flp_sys_i2c_read(pBuffer, nNbBytesToRead);
#endif
}

/*****************************************************************************
 * FUNCTION
 *  mtk_flp_sys_interface_write
 * DESCRIPTION
 *  Write data to FLPC
 * PARAMETERS
 *  pBuffer             [IN] write buffer
 *  nNbBytesToWrite     [IN] number of bytes to write
 * RETURNS
 *  number of bytes written
 *****************************************************************************/
INT32
mtk_flp_sys_interface_write (
    UINT8 *pBuffer,
    UINT16 nNbBytesToWrite
)
{
#ifdef WIN32
    return mtk_flp_sys_uart_write(pBuffer, nNbBytesToWrite);
#else //
   return mtk_flp_sys_i2c_write(pBuffer, nNbBytesToWrite);
#endif
}

/*****************************************************************************
 * FUNCTION
 *  mtk_flp_sys_interface_flush
 * DESCRIPTION
 *  Flush communication interface
 * PARAMETERS
 *  pLinkHandle         [IN] Link Handle
 * RETURNS
 *  NONE
 *****************************************************************************/
VOID
mtk_flp_sys_interface_flush (
    VOID *pLinkHandle
)
{
#ifdef WIN32
    mtk_flp_sys_uart_flush(pLinkHandle);
#endif
}

/*****************************************************************************
 * FUNCTION
 *  mtk_flp_sys_interface_uninit
 * DESCRIPTION
 *  mt6605 gpio config
 * PARAMETERS
 *  pLinkHandle         [IN] Link Handle
 * RETURNS
 *  NONE
 *****************************************************************************/
VOID
mtk_flp_sys_interface_uninit (
    VOID *pLinkHandle
)
{
#ifdef WIN32
    mtk_flp_sys_uart_uninit(pLinkHandle);
#else
    mtk_flp_sys_i2c_uninit(pLinkHandle);
#endif
}
#endif


#if 0
/*****************************************************************************
 * FUNCTION
 *  mtk_flp_sys_gpio_write
 * DESCRIPTION
 *  mt6605 gpio config
 * PARAMETERS
 *  ePin        [IN] GPIO PIN
 *  eHighLow    [IN] High or How
 * RETURNS
 *  NONE
 *****************************************************************************/
VOID
mtk_flp_sys_gpio_write(
    MTK_FLP_GPIO_E ePin,
    MTK_FLP_PULL_E eHighLow
)
{
   UINT32 result;

   result = ioctl(gInterfaceHandle, MTK_FLP_IOCTL_WRITE, ((ePin << 8) | (eHighLow)));

   if(result != MTK_FLP_SUCCESS)
   {
      ;//ERROR
   }

   return;

}

/*****************************************************************************
 * FUNCTION
 *  mtk_flp_sys_gpio_read
 * DESCRIPTION
 *  mt6605 gpio config
 * PARAMETERS
 *  ePin        [IN] GPIO PIN
 * RETURNS
 *  MTK_FLP_PULL_E
 *****************************************************************************/
MTK_FLP_PULL_E
mtk_flp_sys_gpio_read(
    MTK_FLP_GPIO_E ePin
)
{
   MTK_FLP_PULL_E result;

   result = ioctl(gInterfaceHandle, MTK_FLP_IOCTL_READ, (ePin << 8));

   return result;
}
INT32 mtk_flp_sys_i2c_read(UINT8* data, UINT16 len)
{
   INT32 result, i;

   result = read(gInterfaceHandle, data, len);

   if (result > 0)
   {
      CHAR pDeBugBuffer[2048];
      uint32_t DeBugL = 0;

       memset(pDeBugBuffer, 0x00, 2048);
       FLPD("mtk_flp_sys_i2c_read :len,0x%x\r\n", len);
       sprintf(pDeBugBuffer, "[RX],");
       DeBugL = strlen(pDeBugBuffer);
       for( i =0;i < len;i++)
       {
           sprintf((pDeBugBuffer+DeBugL),"%02x,",data[i]);
           DeBugL = strlen(pDeBugBuffer);
       }
       sprintf((pDeBugBuffer+DeBugL),"\n");
       FLPD("%s", pDeBugBuffer);
   }

   return result;
}


INT32 mtk_flp_sys_i2c_write(UINT8* data, UINT16 len)
{
    INT32 result, i;
    CHAR pDeBugBuffer[2048];
    uint32_t DeBugL = 0;

    FLPD("mtk_flp_sys_i2c_write :len,0x%x\r\n", len);

    memset(pDeBugBuffer, 0x00, 2048);

    sprintf(pDeBugBuffer, "[TX],");
    DeBugL = strlen(pDeBugBuffer);

    for( i =0;i < len;i++)
    {
       sprintf((pDeBugBuffer+DeBugL),"%02x,",data[i]);
       DeBugL = strlen(pDeBugBuffer);
    }
    sprintf((pDeBugBuffer+DeBugL),"\n");
    FLPD("%s", pDeBugBuffer);
    result = write(gInterfaceHandle, data, len);

    return result;
}

VOID *mtk_flp_sys_i2c_init(INT32 type)
{
   char *pComPort;

   pComPort = "/dev/mt6605";

   gInterfaceHandle = open(pComPort, O_RDWR | O_NOCTTY);

   return (VOID *)gInterfaceHandle;
}

VOID mtk_flp_sys_i2c_uninit(VOID *pLinkHandle)
{
   if (gInterfaceHandle != -1)
   {
       close(gInterfaceHandle);
       gInterfaceHandle = -1;
   }
}
#endif

#ifdef USE_SIGNAL_EVENT_TO_TIMER_CREATE
VOID flp_timer_expiry_hdlr (int sig, siginfo_t *si, void *uc)
{
    INT32 timer_slot;
    timer_t *tidp;
    ppCallBck_t cb_func;
    VOID *param;

    FLPD("[TIMER]Caugh signal %d\n", sig);


    tidp = si->si_value.sival_ptr;

    /* Look up timer_slot of this timeout, range = 0 ~ (MTK_FLP_TIMER_MAX_NUM-1) */
    for(timer_slot = 0; timer_slot < MTK_FLP_TIMER_MAX_NUM; timer_slot++)
    {
        if ( ( flp_timer_table[timer_slot].is_used == TRUE ) &&
             ( flp_timer_table[timer_slot].handle == *tidp ) )
        {
            break;
        }
    }

    if(timer_slot == MTK_FLP_TIMER_MAX_NUM)    //timer not found in table
    {
        FLPD("[TIMER]timer no found in the table : (handle: 0x%x)\r\n", *tidp);
        return;
    }

    //get the cb and param from gps timer pool
    cb_func = flp_timer_table[timer_slot].timer_expiry_callback;
    param = flp_timer_table[timer_slot].timer_expiry_context;

//    mtk_flp_sys_mutex_give(MTK_FLP_MUTEX_TIMER);

    //stop time (windows timer is periodic timer)
    mtk_flp_sys_timer_stop(timer_slot);

    //execute cb
    (*cb_func)(timer_slot, param);
}
#else
static VOID flp_timer_expiry_hdlr (union sigval sv)
{
    INT32 timer_slot;
    ppCallBck_t cb_func;
    VOID *param;

    FLPD("[TIMER]flp_timer_expiry_hdlr...\n");

#if 0
    mtk_flp_sys_mutex_take(MTK_FLP_MUTEX_TIMER);

    timer_slot = (INT32)(sv.sival_int);

    if(timer_slot >= MTK_FLP_TIMER_MAX_NUM)
    {
        FLPD("[TIMER]invalid timer_slot,%d\r\n", timer_slot);
        mtk_flp_sys_mutex_give(MTK_FLP_MUTEX_TIMER);
        return;
    }

    if (flp_timer_table[timer_slot].is_stopped == 1)
    {
        FLPD("[TIMER] Expired but already stopped timer_slot=%d\n", timer_slot);
        mtk_flp_sys_mutex_give(MTK_FLP_MUTEX_TIMER);
        return;
    }

    //get the cb and param from gps timer pool
    cb_func = flp_timer_table[timer_slot].timer_expiry_callback;
    param = flp_timer_table[timer_slot].timer_expiry_context;

    mtk_flp_sys_mutex_give(MTK_FLP_MUTEX_TIMER);

    //stop time (windows timer is periodic timer)
    mtk_flp_sys_timer_stop(timer_slot);

    //execute cb
    (*cb_func)(timer_slot, param);
#endif
}

#endif


/*****************************************************************************
 * Function
 *  mtk_flp_sys_timer_init
 * DESCRIPTION
 *  Create a new timer
 * PARAMETERS
 *  NONE
 * RETURNS
 *  a valid timer ID or MTK_FLP_TIMER_INVALID_ID if an error occured
 *****************************************************************************/
INT32
mtk_flp_sys_timer_init (
    VOID
)
{
    int ret = MTK_FLP_SUCCESS;
    int timer_slot;

#ifdef USE_SIGNAL_EVENT_TO_TIMER_CREATE
    struct sigaction sa;

    /* Establish handler for timer signal */
    FLPD("Establishing handler for signal %d\n", SIG);
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = flp_timer_expiry_hdlr;
    sigemptyset(&sa.sa_mask);

    ret = sigaction(SIG, &sa, NULL);
    if (ret == -1) {
        FLPD("sigaction fail\r\n");
    }
#endif

    /* Initialize timer pool */
    for (timer_slot = 0; timer_slot < MTK_FLP_TIMER_MAX_NUM; timer_slot++) {
        flp_timer_table[timer_slot].is_used = FALSE;
        flp_timer_table[timer_slot].handle  = 0;
        flp_timer_table[timer_slot].is_used = FALSE;
        flp_timer_table[timer_slot].timer_expiry_callback = NULL;
        flp_timer_table[timer_slot].timer_expiry_context = NULL;
        flp_timer_table[timer_slot].is_stopped = TRUE;
    }

    return ret;
}

/*****************************************************************************
 * Function
 *  mtk_flp_sys_timer_create
 * DESCRIPTION
 *  Create a new timer
 * PARAMETERS
 *  selected_timer  [IN] select the timer slot
 * RETURNS
 *  a valid timer ID or MTK_FLP_TIMER_INVALID_ID if an error occured
 *****************************************************************************/
UINT32
mtk_flp_sys_timer_create (
    MTK_FLP_TIMER_E selected_timer
)
{
    INT32 ret;
    UINT32 timer_slot=0;
    struct sigevent se;
#if 0
    if (selected_timer >= MTK_FLP_TIMER_MAX_NUM) {
        FLPD("[TIMER]Invalid timer request %d\r\n", (UINT32)selected_timer);
        return MTK_FLP_TIMER_INVALID_ID;
    }

    mtk_flp_sys_mutex_take(MTK_FLP_MUTEX_TIMER);

    timer_slot = selected_timer;
    if(flp_timer_table[timer_slot].is_used == TRUE)
    {
        FLPD("[TIMER]timer already created\r\n");
        mtk_flp_sys_mutex_give(MTK_FLP_MUTEX_TIMER);
        return timer_slot;
    }

#ifdef USE_SIGNAL_EVENT_TO_TIMER_CREATE
    /* Create the timer */
    se.sigev_notify = SIGEV_SIGNAL;
    se.sigev_signo = SIG;
    se.sigev_value.sival_ptr = &flp_timer_table[timer_slot].handle;
#else
    se.sigev_notify = SIGEV_THREAD;
    se.sigev_notify_function = flp_timer_expiry_hdlr;
    se.sigev_notify_attributes = NULL;
    se.sigev_value.sival_int = (int) timer_slot;
#endif

    /* Create a POSIX per-process timer */
    if ((ret = timer_create(CLOCKID, &se, &(flp_timer_table[timer_slot].handle))) == -1)
    {
        FLPD("[TIMER]timer_create fail, ret:%d, errno:%d, %s\r\n", ret, errno, strerror(errno));
        mtk_flp_sys_mutex_give(MTK_FLP_MUTEX_TIMER);
        return MTK_FLP_TIMER_INVALID_ID;
    }

    flp_timer_table[timer_slot].is_used = TRUE;
    FLPD("[TIMER]create,time_slot,%d,handle,0x%x\r\n", timer_slot, flp_timer_table[timer_slot].handle);

    mtk_flp_sys_mutex_give(MTK_FLP_MUTEX_TIMER);
#endif
    return timer_slot;
}

/*****************************************************************************
 * Function
 *  mtk_flp_sys_timer_start
 * DESCRIPTION
 *  Start a timer
 * PARAMETERS
 *  timer_slot  [IN] a valid timer slot
 *  period      [IN] expiration time in milliseconds
 *  timer_expiry[IN] callback to be called when timer expires
 *  arg         [IN] callback fucntion parameter
 * RETURNS
 *  NONE
 *****************************************************************************/
VOID
mtk_flp_sys_timer_start (
    UINT32      timer_slot,
    UINT32      period,
    ppCallBck_t timer_expiry,
    VOID        *arg
)
{
    struct itimerspec its;
#if 0
    if (timer_slot >= MTK_FLP_TIMER_MAX_NUM)
    {
        FLPD("[TIMER]timer_slot(%d) exceed max num of timer\r\n", timer_slot);
        return;
    }

    if (timer_expiry == NULL)
    {
        FLPD("[TIMER]timer_expiry_callback == NULL\r\n");
        return;
    }

    mtk_flp_sys_mutex_take(MTK_FLP_MUTEX_TIMER);

    if (flp_timer_table[timer_slot].is_used == FALSE)
    {
        FLPD("[TIMER]timer_slot(%d) didn't be created\r\n", timer_slot);
        mtk_flp_sys_mutex_give(MTK_FLP_MUTEX_TIMER);
        return;
    }

    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;
    its.it_value.tv_sec = period / 1000;
    its.it_value.tv_nsec = 1000000 * (period % 1000);
    if ((its.it_value.tv_sec == 0) && (its.it_value.tv_nsec == 0))
    {
        // this would inadvertently stop the timer (TODO: HIKI)
        its.it_value.tv_nsec = 1;
    }

    flp_timer_table[timer_slot].timer_expiry_callback = timer_expiry;
    flp_timer_table[timer_slot].timer_expiry_context = arg;
    flp_timer_table[timer_slot].is_stopped = FALSE;
    timer_settime(flp_timer_table[timer_slot].handle, 0, &its, NULL);

    FLPD("[TIMER]timer_slot(%d) start, handle(0x%x)\r\n", timer_slot, flp_timer_table[timer_slot].handle);

    mtk_flp_sys_mutex_give(MTK_FLP_MUTEX_TIMER);
#endif
}

/*****************************************************************************
 * Function
 *  mtk_flp_sys_timer_stop
 * DESCRIPTION
 *  Start a timer
 * PARAMETERS
 *  timer_slot    [IN] a valid timer slot
 * RETURNS
 *  NONE
 *****************************************************************************/
VOID
mtk_flp_sys_timer_stop (
    MTK_FLP_TIMER_E timer_slot
)
{
    struct itimerspec its = {{0, 0}, {0, 0}};
#if 0
    if (timer_slot >= MTK_FLP_TIMER_MAX_NUM)
    {
        FLPD("[TIMER]timer_slot(%d) exceed max num of timer\r\n", timer_slot);
        return;
    }

    mtk_flp_sys_mutex_take(MTK_FLP_MUTEX_TIMER);

    if (flp_timer_table[timer_slot].is_used == FALSE)
    {
        FLPD("[TIMER]timer_slot(%d) already be deleted\r\n", timer_slot);
        mtk_flp_sys_mutex_give(MTK_FLP_MUTEX_TIMER);
        return;
    }

    if (flp_timer_table[timer_slot].is_stopped == TRUE)
    {
        FLPD("[TIMER]timer_slot(%d) already be stopped\r\n", timer_slot);
        mtk_flp_sys_mutex_give(MTK_FLP_MUTEX_TIMER);
        return;
    }

    flp_timer_table[timer_slot].is_stopped = TRUE;
    timer_settime(flp_timer_table[timer_slot].handle, 0, &its, NULL);

    FLPD("[TIMER]timer_slot(%d) stop, handle(0x%x)\r\n", timer_slot, flp_timer_table[timer_slot].handle);

    mtk_flp_sys_mutex_give(MTK_FLP_MUTEX_TIMER);
#endif
}

/*****************************************************************************
 * Function
 *  mtk_flp_sys_timer_delete
 * DESCRIPTION
 *  Delete a timer
 * PARAMETERS
 *  timer_slot    [IN] a valid timer slot
 * RETURNS
 *  NONE
 *****************************************************************************/
VOID
mtk_flp_sys_timer_delete (
    MTK_FLP_TIMER_E timer_slot
)
{
#if 0
    if (timer_slot >= MTK_FLP_TIMER_MAX_NUM)
    {
        FLPD("[TIMER]exceed max num of timer,%d\r\n", timer_slot);
        return;
    }

    mtk_flp_sys_mutex_take(MTK_FLP_MUTEX_TIMER);

    if (flp_timer_table[timer_slot].is_used == FALSE)
    {
        FLPD("[TIMER]timer_slot(%d) already be deleted\r\n", timer_slot);
        mtk_flp_sys_mutex_give(MTK_FLP_MUTEX_TIMER);
        return;
    }

    timer_delete(flp_timer_table[timer_slot].handle);
    flp_timer_table[timer_slot].handle = 0;
    flp_timer_table[timer_slot].timer_expiry_callback = NULL;
    flp_timer_table[timer_slot].timer_expiry_context = NULL;
    flp_timer_table[timer_slot].is_used = FALSE; // clear used flag
    FLPD("[TIMER]timer_slot(%d) delete\r\n", timer_slot);

    mtk_flp_sys_mutex_give(MTK_FLP_MUTEX_TIMER);
#endif
}


MTK_FLP_UTC_TIME mtk_flp_get_timestamp(struct timeval *ptv)
{
    struct timeval tv;

    if( ptv ){
        return ( (unsigned long long)ptv->tv_sec*1000ULL+(unsigned long long)ptv->tv_usec/1000ULL);
    }else{
        gettimeofday(&tv, NULL);
        return (  (unsigned long long)tv.tv_sec*1000ULL + (unsigned long long)tv.tv_usec/1000ULL);
    }
}

//static pthread_cond_t g_flp_event_cond[MTK_FLP_EVENT_END];
//static pthread_mutex_t g_flp_event_mtx[MTK_FLP_EVENT_END];

INT32 mtk_flp_sys_event_delete(MTK_FLP_EVENT *event)
{
    INT32 ret = MTK_FLP_SUCCESS;

    if (pthread_cond_destroy(event))
    {
        ret = MTK_FLP_ERROR;
    }

    return ret;
}

INT32 mtk_flp_sys_event_create(MTK_FLP_EVENT *event)
{
    INT32 ret = MTK_FLP_SUCCESS;

    if (pthread_cond_init(event, NULL))
    {
        ret = MTK_FLP_ERROR;
    }

    return ret;
}

INT32 mtk_flp_sys_event_set(MTK_FLP_EVENT *event)
{
    INT32 ret = MTK_FLP_SUCCESS;

    if (pthread_cond_signal(event))
    {
        ret = MTK_FLP_ERROR;
    }

    return ret;
}

INT32 mtk_flp_sys_event_wait(MTK_FLP_EVENT *event, MTK_FLP_MUTEX *mutex)
{
    INT32 ret = MTK_FLP_SUCCESS;

    if (pthread_cond_wait(event, mutex))
    {
        ret = MTK_FLP_ERROR;
    }

    return ret;
}


MTK_FLP_TASK_T *mtk_flp_find_task(MTK_FLP_TASKID_E id)
{
    //FLPD("task[%d] ptr: [%x]\n", id, &(gFlpTasks[id]) );
    return &(gFlpTasks[id]);
}

INT32 mtk_flp_is_socket_task(MTK_FLP_TASKID_E id)
{
    return (id == MTK_FLP_TASKID_SOCKET );
}


void mtk_flp_tasks_dump()
{
    int i=0;
    MTK_FLP_TASK_T *task;
	for(i=0;i<MTK_FLP_TASKID_END;i++){
		task = mtk_flp_find_task(i);
		FLPD("socket:[%d] msg:[%p]\n", task->sockethandle, (task->msg_body));
	}



}

INT32 mtk_flp_tasks_init(VOID)
{
	int i;


	for(i=0;i<MTK_FLP_TASKID_END;i++){
		mtk_flp_sys_msg_initialize((MTK_FLP_TASKID_E)i);
		mtk_flp_sys_mutex_create( (MTK_FLP_MUTEX *)&gFlpTasks[i].mutex );
		mtk_flp_sys_event_create( (MTK_FLP_EVENT *)&gFlpTasks[i].event );
		gFlpTasks[i].sockethandle = C_INVALID_SOCKET;
	}

	for(i=0;i<MTK_FLP_TASKID_END;i++){
		FLPD("Task[%d], sockethandle:%d\n",i,gFlpTasks[i].sockethandle);
	}
	return MTK_FLP_SUCCESS;
}


INT32 mtk_flp_tasks_deinit(VOID)
{

	int i;


	for(i=0;i<MTK_FLP_TASKID_END;i++){
		mtk_flp_sys_mutex_destory( (MTK_FLP_MUTEX *)&gFlpTasks[i].mutex );
		mtk_flp_sys_event_delete( (MTK_FLP_EVENT *)&gFlpTasks[i].event );
		if(gFlpTasks[i].sockethandle >= 0){
		    close(gFlpTasks[i].sockethandle);
		    gFlpTasks[i].sockethandle = C_INVALID_SOCKET;
		}
	}


	return MTK_FLP_SUCCESS;

}






