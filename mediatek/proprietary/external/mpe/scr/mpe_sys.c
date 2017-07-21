/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2014
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
***************************************************************/
#ifndef MPE_SYS_C
#define MPE_SYS_C

#ifdef __cplusplus
  extern "C" {
#endif

/*****************************************************************************
 * Include
 *****************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h> 
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <errno.h>
#include <utils/Log.h> // For Debug
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "mpe_sys_common.h"

/***************************************************************************** 
 * Define
 *****************************************************************************/
#ifdef DEBUG_LOG

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "MPE_SYS"
#define LOG_EN

#ifdef LOG_EN 
#define FLPE(...)		MPE_sys_dbg(MPE_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define FLPD(...)		MPE_sys_dbg(MPE_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define FLPW(...)		MPE_sys_dbg(MPE_LOG_WARN, LOG_TAG, __VA_ARGS__)
#else
#define FLPD(...)
#define FLPW(...)
#define FLPE(...)
#endif

#endif

#define MPE_MSG_HDR_SIZE        sizeof(MPE_MSG)
#define MPE_UDP_LEN    4096

typedef struct _MPE_THREAD_T {
  int                     snd_fd;
  MPE_THREAD_ID_ENUM  thread_id;
  pthread_t               thread_handle;
  int (*thread_exit)(struct MPE_THREAD_T *arg);
  int (*thread_active)(struct MPE_THREAD_T *arg);
} MPE_THREAD_T;

/***************************************************************************** 
 * Global Variables
 *****************************************************************************/
int MPE_exit_thread_normal(MPE_THREAD_T *arg);

static MPE_THREAD_T MPE_thread[MPE_THREAD_SOCKET] = {
{C_INVALID_FD, MPE_THREAD_SOCKET, C_INVALID_TID, MPE_exit_thread_normal},
};
MPE_TASK_T   gMPETasks[MPE_TASKID_END];
static char mpesockbuf[MPE_UDP_LEN];

/***************************************************************************** 
 * Function Prototype
 *****************************************************************************/
extern void * thread_socket_MPE(void * arg);
/*****************************************************************************
* FUNCTION
*  MPE_threads_create
* DESCRIPTION
*  Create MPE main thread
* PARAMETERS
* RETURNS
*  TRUE/FALSE (MPE thread create failed/passed)
*****************************************************************************/
BL MPE_threads_create( void ) {
  BL ret = TRUE;

  if(pthread_create(&MPE_thread[MPE_THREAD_SOCKET].thread_handle,
                 NULL, thread_socket_MPE,NULL)) {
     ret = FALSE;
     FLPD("MPE socket thread init failed");
  }
  return ret;
}
/*****************************************************************************
* FUNCTION
*  MPE_threads_release
* DESCRIPTION
*  Release MPE main thread
* PARAMETERS
* RETURNS
*  TRUE/FALSE (MPE thread release failed/passed)
*****************************************************************************/
int MPE_threads_release(void) {
  int result = MPE_SUCCESS;
  int idx;
  
  FLPD("MPE_threads_release...\n");
    
  for (idx = 0; idx < (MPE_THREAD_THREAD_NUM-1); idx++) {
  	if (MPE_thread[idx].thread_handle == C_INVALID_TID) {
  		continue;
    }

    if (!MPE_thread[idx].thread_exit) {
    	continue;
    }

    if ((MPE_thread[idx].thread_exit(&MPE_thread[idx]))) {
    	result = MPE_ERROR;
      FLPD("MPE_threads_release exit fail,%d\n", idx);
    }
  }
  FLPD("MPE_threads_release done\n");
  return (result);
}

int MPE_exit_thread_normal(MPE_THREAD_T *arg) {
  int err;
  void *ret;

  FLPD("mtk_flp_exit_thread_normal (taskid: %d)\n", arg->thread_id);
  if(arg->thread_id == MPE_THREAD_SOCKET ) {
  	err = pthread_join(arg->thread_handle, &ret);
  	if (err) {
  		FLPD("(%d)ThreadLeaveErr: %d\n", (arg->thread_id), err);
      return err;
    } else {
      FLPD("(%d)ThreadLeaveOK\n", (arg->thread_id));
    }
    arg->thread_handle = C_INVALID_TID;
  }
  return 0;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_find_task
 * DESCRIPTION
 *  Get MPE task address
 * PARAMETERS
 *   IN: task id
 * RETURNS
 *  task address
 *****************************************************************************/
MPE_TASK_T *MPE_find_task(MPE_TASKID_E id) {
	return &(gMPETasks[id]);
}
/*****************************************************************************
 * FUNCTION
 *  MPE_initialize_mutex
 * DESCRIPTION
 *  Initialize MPE Mutex
 * PARAMETERS
 * RETURNS
 *  TRUE/FALSE (mutex init failed/passed)
 *****************************************************************************/
mtk_int16 MPE_initialize_mutex(MPE_MUTEX *mutex) {
  pthread_mutex_init(mutex, NULL);
  return TRUE;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_take_mutex
 * DESCRIPTION
 *  Take MPE Mutex
 * PARAMETERS: mutex id
 * RETURNS
 *  TRUE/FALSE (mutex take failed/passed)
 *****************************************************************************/
mtk_int16 MPE_take_mutex( MPE_MUTEX *mutex ) {
  pthread_mutex_lock(mutex);
  return MPE_SUCCESS;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_give_mutex
 * DESCRIPTION
 *  Release MPE Mutex
 * PARAMETERS: mutex id
 * RETURNS
 *  TRUE/FALSE (MPE mutex release failed/passed)
 *****************************************************************************/
mtk_int16 MPE_give_mutex( MPE_MUTEX *mutex ) {
  pthread_mutex_unlock(mutex);
  return MPE_SUCCESS;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_destroy_mutex
 * DESCRIPTION
 *  Destroy MPE Mutex
 * PARAMETERS: mutex id
 * RETURNS
 *  TRUE/FALSE (MPE mutex destroy failed/passed)
 *****************************************************************************/
mtk_int16 MPE_destroy_mutex( MPE_MUTEX *mutex ) {
  FLPD("mpe destroy mutex \n");
  if (pthread_mutex_destroy(mutex)) {
  	FLPD("destroy mutex failed");
    return MPE_ERROR;
  }
  FLPD("destroy mutex success");
  return MPE_SUCCESS;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_event_create
 * DESCRIPTION
 *  Create MPE Event
 * PARAMETERS: event id
 * RETURNS
 *  TRUE/FALSE (MPE event create failed/passed)
 *****************************************************************************/
mtk_int16 MPE_event_create( MPE_EVENT *event ) {
  mtk_int16 ret = MPE_SUCCESS;
  if (pthread_cond_init(event, NULL)) {
  	ret = MPE_ERROR;
  }
  return ret;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_event_set
 * DESCRIPTION
 *  Set MPE Event
 * PARAMETERS: event id
 * RETURNS
 *  TRUE/FALSE (MPE event set failed/passed)
 *****************************************************************************/
mtk_int16 MPE_event_set( MPE_EVENT *event )
{
  mtk_int16 ret = MPE_SUCCESS;
  if (pthread_cond_signal(event)) {
      ret = MPE_ERROR;
  }
  return ret;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_event_wait
 * DESCRIPTION
 *  Set MPE Event
 * PARAMETERS: event id
 * RETURNS
 *  TRUE/FALSE (MPE event wait failed/passed)
 *****************************************************************************/
mtk_int16 MPE_event_wait( MPE_EVENT *event, MPE_MUTEX *mutex ) {
  mtk_int16 ret = MPE_SUCCESS;
  if (pthread_cond_wait(event, mutex)) {
  	ret = MPE_ERROR;
  }
  return ret;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_event_delete
 * DESCRIPTION
 *  Delete MPE Event
 * PARAMETERS: event id
 * RETURNS
 *  TRUE/FALSE (MPE event delete failed/passed)
 *****************************************************************************/
mtk_int16 MPE_event_delete( MPE_EVENT *event ) {
  mtk_int32 ret = MPE_SUCCESS;
  if (pthread_cond_destroy(event)) {
  	ret = MPE_ERROR;
    FLPD("delete event failed 1\n");
  }
  FLPD("delete event success\n");
  return ret;  
}
/*****************************************************************************
 * FUNCTION
 *  MPE_sys_socket_send
 * DESCRIPTION
 *  MPE send by socket
 * PARAMETERS: socket id, sent message
 * RETURNS
 *  TRUE/FALSE (MPE sent failed/passed)
 *****************************************************************************/
mtk_uint32 MPE_sys_socket_send(mtk_int32 sockfd, MPE_MSG *p_msg, struct sockaddr_un sock_addr) {
  mtk_int32 i4SendLen;
  char buff[128];

  if (p_msg == NULL) {
     FLPE("socket send fail: due to msg == null\n");
     return -1;
  }

  if (sockfd < 0) {
     FLPW("socket send msg fail: due to invalid sockfd: %d\n", sockfd);
     return -1;
  }

  memcpy(&buff, (char *)p_msg, sizeof(MPE_MSG)+p_msg->length);
  i4SendLen = sendto(sockfd, buff, sizeof(MPE_MSG) + p_msg->length ,0,(struct sockaddr*)&sock_addr, sizeof(sock_addr) );

  if (i4SendLen < 0) {
     FLPW("socket fd:%d send fail: %d, %s\n", sockfd, errno, strerror(errno));
  }
  MPE_mem_free(p_msg);
  return i4SendLen;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_sys_socket_read
 * DESCRIPTION
 *  MPE read from socket
 * PARAMETERS: socket id, received message & length
 * RETURNS
 *  TRUE/FALSE (MPE read failed/passed)
 *****************************************************************************/
mtk_int32 MPE_sys_socket_read(mtk_int32 sockfd, mtk_uint8*pRecvBuff, mtk_int32 i4RecvLen, struct sockaddr_un sock_addr) {
  mtk_int32 i4ReadLen;
  mtk_int32 i4TotalReadLen;
  socklen_t addrlen = sizeof(sock_addr);
  int i;

  if (sockfd < 0) {
     FLPE("socket recv msg fail: due to invalid sockfd: %d\n", sockfd);
     return -1;
  } 
  i4TotalReadLen = 0;
  i4ReadLen = recvfrom(sockfd, pRecvBuff, i4RecvLen, 0, (struct sockaddr*)&sock_addr,&addrlen );
  if (i4ReadLen < 0) {
  	FLPW("socket read fail: %d, %s\n", errno, strerror(errno));
    i4TotalReadLen = i4ReadLen; // keep read fail return value
  } else if (i4ReadLen == 0) {
  	FLPW("socket read fail due to socket be closed\n");
    i4TotalReadLen = i4ReadLen; // keep read fail return value
  } else {
    i4TotalReadLen += i4ReadLen;
  }    
  FLPD("MPE_sys_socket_read ok (read len: %d, target len: %d)\n", i4TotalReadLen, i4RecvLen);
  return i4TotalReadLen;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_sys_socket_recv
 * DESCRIPTION
 *  MPE received from socket
 * PARAMETERS: socket id, message
 * RETURNS
 *  TRUE/FALSE (MPE receive failed/passed)
 *****************************************************************************/
 #if 0
int MPE_sys_socket_recv(mtk_int32 sockfd, MPE_MSG **p_msg, struct sockaddr_un sock_addr) {
  int i4ReadLen = 0, i;
  MPE_MSG msg_hdr;
  void *p_msg_body;
  unsigned char *pBuff;

  // read msg header (blocking read)
  //pBuff = (unsigned char *)&msg_hdr;
  i4ReadLen = MPE_sys_socket_read(sockfd, pBuff, 128, sock_addr);

  if ((i4ReadLen <= 0)) {// error case
  	FLPD("i4ReadLen error");
    return MPE_ERROR;
  } else {
  	memcpy(&msg_hdr, (signed char *)pBuff, MPE_MSG_HDR_SIZE); 
    FLPD("msg hdr (type: 0x%02x, len: %d)\n", msg_hdr.type, msg_hdr.length);
    // fill type & length
    memcpy((unsigned char *)*p_msg,((signed char *)pBuff),(MPE_MSG_HDR_SIZE+msg_hdr.length));
  }
  return MPE_SUCCESS;
}
#endif

int MPE_sys_socket_recv_msg(mtk_int32 sockfd[MPE_SOCKET_CONN_NO], struct sockaddr_un sock_addr[MPE_SOCKET_CONN_NO], char sockbuf[128]) {
    mtk_int32 i4TotalReadLen;
    int recv_status = 0;
    fd_set readfds;
    int max_sd = 0, ret=0, i, recvlen;
    socklen_t addrlen = sizeof(sock_addr[0]);

    recvlen = recvfrom(sockfd[0], &sockbuf[0], 128, 0, (struct sockaddr*)&(sock_addr[0]), &addrlen);
    if( recvlen >= (int)sizeof(MPE_MSG) ){//normal message
    	return recvlen;
    } else {
    	FLPD("Error recveiving mpe UDP data: %s", strerror(errno));
      return -1;
    }
}
/*****************************************************************************
 * FUNCTION
 *  MPE_sys_scan_socket
 * DESCRIPTION
 *  scan across valid socket, return socket fd which is ready to recv msg
 *  modify size of sock_fd if more than 2 clients are connected to MPE in the future
 * PARAMETERS: socket fd in
 * RETURNS : socket fd out 
 *****************************************************************************/
mtk_int32 MPE_sys_scan_socket(int sock_fd_mnl, int sock_fd_flp) {
  fd_set readfds;
  int recvlen = 0;
  int max_sd = 0, ret=0;
  mtk_int32 fd_used = -1;

  max_sd = (sock_fd_mnl>sock_fd_flp?sock_fd_mnl:sock_fd_flp);
  FD_ZERO(&readfds);
  FD_SET(sock_fd_mnl, &readfds);
  FD_SET(sock_fd_flp, &readfds);

  ret = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
  fd_used = *(mtk_int32 *)(&readfds);
  FLPD("scan socket =%d", fd_used);

  if(ret<0) {
  	FLPD("mpe select err: %s", strerror(errno) );
  }
  return fd_used;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_Msg_Ring_Buf_Init
 * DESCRIPTION
 *  Init MPE ring buf message
 * PARAMETERS: event id
 * RETURNS : no  
 *****************************************************************************/
void MPE_Msg_Ring_Buf_Init(MPE_TASKID_E task_id) {    
  MPE_TASK_T *task = MPE_find_task(task_id);
  task->msg_ring = &(task->msg_body);
  task->msg_ring->start_buffer = &task->msg_buf[0];
  task->msg_ring->end_buffer = &task->msg_buf[MPE_MSG_RING_SIZE-1];
  task->msg_ring->next_write = task->msg_ring->start_buffer;
  task->msg_ring->next_read = task->msg_ring->start_buffer;
  task->msg_cnt = 0;
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
 *  MPE_SUCCESS
 *****************************************************************************/ 
mtk_int32 MPE_sys_msg_send ( MPE_TASKID_E task_id, MPE_MSG *msg) {
  MPE_TASK_T *task = MPE_find_task(task_id);

  if(task==NULL) {
  	FLPD("Task id error:%d\n",task_id);
    return MPE_ERROR;   
  }  
  if (msg == NULL) {
  	FLPD("Send message full:%d\n",task_id);
    return MPE_ERROR;
  }
  MPE_take_mutex(&task->mutex);
  /* msg queue full check */
  if (task->msg_cnt== MPE_MSG_RING_SIZE) {
  	MPE_give_mutex(&task->mutex);
    FLPD("Send message to ID[%d], full, reset\n", task_id);
    MPE_Msg_Ring_Buf_Init((MPE_TASKID_E)MPE_TASKID_MAIN);
    return MPE_ERROR;
  }
  if ( task->msg_ring!= NULL) {
  	*(task->msg_ring->next_write) = (MPE_MSG*)msg;
    task->msg_ring->next_write++;
    // Wrap check the input circular buffer
    if ( task->msg_ring->next_write > task->msg_ring->end_buffer ) {
    	task->msg_ring->next_write = task->msg_ring->start_buffer;
    }
    task->msg_cnt++;

    if (MPE_SUCCESS != MPE_event_set( &(task->event))) {
    	FLPD("MPE_event_set to [%d],fail\n", task_id);
      MPE_give_mutex(&task->mutex);
      return MPE_ERROR;
    }
    MPE_give_mutex(&task->mutex);
    return MPE_SUCCESS;
  } else {
  	FLPD("message ring not set!!!\n");
    MPE_give_mutex(&task->mutex);
    return MPE_ERROR;
  }
  return MPE_ERROR;
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
mtk_int32 MPE_sys_msg_recv ( MPE_TASKID_E task_id, MPE_MSG **msg) {
  MPE_TASK_T *task = MPE_find_task(task_id);

  if (msg != NULL) {
  	MPE_take_mutex(&task->mutex);
    /* wait signal if no msg in queue */
    if ( task->msg_cnt<= 0 ) {
    	FLPD("TaskID:[%d][%d] waiting... \n", task_id, getpid());
      if (MPE_SUCCESS != MPE_event_wait(&task->event, &task->mutex)) {
      	FLPD("MPE_event_wait, ID:%d,fail\n", task_id);
        MPE_give_mutex(&task->mutex);
        return MPE_ERROR;
      }
    } else {
    	FLPD("TaskID[%d] has %d events in queue", task_id, task->msg_cnt);
    }

    if (*(task->msg_ring->next_read) == NULL) {
    	MPE_give_mutex(&task->mutex);
      return MPE_ERROR;
    }
    (*msg) = *(task->msg_ring->next_read);
    task->msg_ring->next_read++;

    // Wrap check output circular buffer
    if ( task->msg_ring->next_read > task->msg_ring->end_buffer ) {
    	task->msg_ring->next_read = task->msg_ring->start_buffer;
    }

   task->msg_cnt--;
   MPE_give_mutex(&task->mutex);
   return MPE_SUCCESS;
  } else {
  	FLPD("MPE_sys_msg_recv: NULL recv message pointer");
    return MPE_ERROR;
  }
  return MPE_SUCCESS;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_Msg_Ring_Buf_free
 * DESCRIPTION
 *  Free MPE ring buf message
 * PARAMETERS: message
 * RETURNS
 *  TRUE/FALSE (MPE message free failed/passed)
 *****************************************************************************/
void MPE_Msg_Ring_Buf_free( MPE_MSG *msg ) {
  if(msg != NULL)
  	free(msg);
   return;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_Mem_Alloc
 * DESCRIPTION
 *  memory allocation of MPE variables
 * PARAMETERS: size of variable
 * RETURNS
 *   (MPE malloc size)
 *****************************************************************************/
void* MPE_Mem_Alloc (mtk_uint16 size) {
  void *ptr;
  ptr = malloc(size);
  return (void *)ptr;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_mem_free
 * DESCRIPTION
 *  memory free of MPE variables
 * PARAMETERS: size of variable
 * RETURNS
 *   NULL
 *****************************************************************************/
void MPE_mem_free (void *pMem) {
  if (pMem != NULL) {
  	free(pMem);
  	pMem=NULL;
  }
  return;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_task_init
 * DESCRIPTION
 *  initialize MPE  
 * PARAMETERS
 * RETURNS
 *  TRUE/FALSE (init failed/passed)
 *****************************************************************************/
BL MPE_task_init (void) {
  int i;

  for(i=0;i<MPE_TASKID_END;i++) {
  	MPE_Msg_Ring_Buf_Init((MPE_TASKID_E)i);
    MPE_initialize_mutex( (MPE_MUTEX *)&gMPETasks[i].mutex );
    MPE_event_create( (MPE_EVENT *)&gMPETasks[i].event );
    gMPETasks[i].sockethandle = C_INVALID_SOCKET;
    FLPD("Task[%d], sockethandle:%d\n",i,gMPETasks[i].sockethandle);
  }
  return MPE_SUCCESS;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_task_uninit
 * DESCRIPTION
 *  Deinitialize MPE   
 * PARAMETERS
 * RETURNS
 *  TRUE/FALSE (MPE deinit failed/passed)
 *****************************************************************************/
BL MPE_task_uninit(void) {
	int i;
  for(i=0;i<MPE_TASKID_END;i++) {
  	MPE_destroy_mutex( (MPE_MUTEX *)&gMPETasks[i].mutex );
    MPE_event_delete( (MPE_EVENT *)&gMPETasks[i].event );
    if(gMPETasks[i].sockethandle >= 0) {
        close(gMPETasks[i].sockethandle);
        gMPETasks[i].sockethandle = C_INVALID_SOCKET;
    }
  }
  return MPE_SUCCESS;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_sys_dbg
 * DESCRIPTION
 *  Print MPE  debug message 
 * PARAMETERS: priority, tag and format
 * RETURNS
 *  TRUE/FALSE 
 *****************************************************************************/
mtk_int16 MPE_sys_dbg(int prior, const char *tag,  const char *fmt, ...) {
  char formatted_string[MAX_FMT_SIZE];

  va_list argptr;
  va_start(argptr,fmt);
  __android_log_vprint(ANDROID_LOG_ERROR, tag, fmt, argptr);
  va_end(argptr);
  return 0;
}

//*****************************************************************************
//  mktimeFromGPS : Convert GPS time to C time_t using
//                  C standard library routines See K&R pages 255-257
mtk_int32 mktimeFromGPS ( mtk_uint32 gps_sec ) {
   return (mtk_int32) ( gps_sec + DIFF_GPS_C_TIME );// difference between GPS and
}

//*****************************************************************************
//  mktimeToGPS :   Convert C time_t to GPS time using
//                  C standard library routines See K&R pages 255-257
void mktimeToGPS (time_t Ctime, mtk_int16 *gpsWeekNo, mtk_uint32 *gpsITOW ) {// GPS seconds in week range 0-604800
  div_t gpsWeek_ITOW;        // quot and rem equal gpsWeek and ITOW
  gpsWeek_ITOW = div(Ctime-DIFF_GPS_C_TIME, SECS_IN_WEEK);
  *gpsWeekNo = (mtk_int16)gpsWeek_ITOW.quot;
  *gpsITOW = gpsWeek_ITOW.rem;
  return;
}
//*****************************************************************************
void get_time_stamp(double* timetag, mtk_int32 leap_sec) {
  struct tm *tm_pt;
  struct timeval tv;
  char time_str[30];
  //get second and usec
  gettimeofday(&tv, NULL);
  //convert to local time
  tm_pt = localtime(&tv.tv_sec);
  (*timetag) = mktime(tm_pt);
  (*timetag)= (*timetag) + leap_sec +((float)tv.tv_usec)/1000000;
}

#ifdef __cplusplus
  extern "C" }
#endif

#endif //#ifndef MPE_FUSION_SYS_C
