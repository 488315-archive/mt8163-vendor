/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
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
#ifndef MPE_MAIN_C
#define MPE_MAIN_C

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
#include <utils/Log.h> // For Debug
#include <cutils/sockets.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include <cutils/properties.h>
#include <private/android_filesystem_config.h>
#include "mpe_sys_common.h"
#include "mpe_API.h"
#include "mpe_PDR.h"
#include "SensorListener_API.h"

/*****************************************************************************
 * Define
 *****************************************************************************/
#ifdef DEBUG_LOG

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "MPE_MA"
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

#define BATCH_LOCAL_SIZE 100
#define sensor_axis 3
#define GYRO_RATE 10
#define ACC_RATE 10
#define MAG_RATE 10
#define EPOLL_NUM 1
#define ERR_REMOTE_HANGUP   0x0F01
#define ERR_MNL_DIED        0x0F02
#define SDCARD_SWITCH_PROP  "persist.mtklog.log2sd.path"
#define PATH_INTERNAL       "internal_sd"
#define PATH_EXTERNAL       "external_sd"
#define PATH_DEFAULT        "/mnt/sdcard/"
#define PATH_EX             "/mnt/sdcard2/"
#define PATH_SUFFIX         "mtklog/gpsdbglog"
#define LOG_FILE            "/data/misc/"
#define MPEMNL_COMM_SOCK_PATH   "/data/mtk_flp_server"

//socket connecting mnl & mpe
#define MPEMNL_UDP_CLI_PATH    "/data/mpe_mnl/mnl2mpe"
#define MPEMNL_UDP_SRV_PATH    "/data/mpe_mnl/mpe2mnl"

//socket connecting flp & mpe
#define MPEFLP_UDP_CLI_PATH    "/data/mpe_flp/flp2mpe"
#define MPEFLP_UDP_SRV_PATH    "/data/mpe_flp/mpe2flp"
/*****************************************************************************
 * GLobal Variable
 *****************************************************************************/
extern MPE_TASK_T   gMPETasks[MPE_TASKID_END];
static struct sockaddr_un sock_mpemnl_cli_addr;
static struct sockaddr_un sock_mpeflp_cli_addr;
static struct sockaddr_un sock_mpemnl_srv_addr;
static struct sockaddr_un sock_mpeflp_srv_addr;
static volatile int g_ThreadExitMPE = FALSE;
static volatile int g_ThreadExitSocket = FALSE;
static volatile BL SE_listener_mode = MPE_ERROR_MODE;
static BL current_run_mode = -1;
volatile mtk_int32 dr_status = 0;
mtk_int32 dr_flag = 0;
static int mpe_mnl_socket_fd = -1;
static int mpe_mnl_socket_server_fd = -1;
static int mpe_flp_socket_fd = -1;
static int mpe_flp_socket_server_fd = -1;
int (*gSExplorerCallBack)(IMU* data, int size)= NULL;
mtk_uint16 write_cnt_sensorRaw, write_cnt_sensorK, write_cnt_sensorFused; //write count of raw, k, fused data in global
mtk_uint16 read_cnt_sensorRaw, read_cnt_sensorK, read_cnt_sensorFused; //Read count of raw, k, fused data in global
BL isSensorRaw_overwrite, isSensorK_overwrite, isSensorFused_overwrite; //sensor data overwrite flag
BL isRunning = FALSE;
BL isInit = FALSE;
BL isUninit_SE = FALSE;
int isRunning_source = 0;

//Mtklogger function
static char propbuf[512];
static int mtklogger2mpe_fd = -1;
static int mpe_rcv_mtklogger_fd = -1;   //fd for receiving mtklogger enable/disable command
static unsigned char gps_debuglog_state = GPS_DEBUGLOG_DISABLE;
char mpe_debuglog_file_name[MPE_DEBUG_LOG_FILE_NAME_MAX_LEN];
MPE_SENSOR_RAW _global_sensor_raw[BATCH_LOCAL_SIZE];
MPE_SENSOR_CALIBRATED _global_sensor_k[BATCH_LOCAL_SIZE];
MPE_SENSOR_FUSION _global_sensor_fusion[BATCH_LOCAL_SIZE];
int epoll_fd;
int client_fd = -1;
int client_to_server;
char *myfifo = "/data/mnlmpe_server_fifo";
int server_to_client;
char *myfifo2 = "/data/mnlmpe_client_fifo";
static volatile int pipe_close = 0;
float abs_height = 0, rel_height = 0;
/*****************************************************************************
 * Functions
 *****************************************************************************/
extern int update_activity_monitor(IMU* pImu);
int MPE_API_fusion_update_attitude(IMU* pImu, float* angles, int dt);
int MPE_sensor_cb(IMU* data, mtk_uint16 len);
BL MPE_read_sensor_raw( void );
BL MPE_read_sensor_k( void );
BL MPE_read_sensor_fusion( void );
void mpe_mnl_out_set(MNL_location_input_t *mnl_in,MNL_location_output_t *mnl_out );
void check_pipe_handler( int signum );
/*****************************************************************************
* FUNCTION
*  thread_main_MPE
* DESCRIPTION
*   Main thread of MPE, process REQ & RES to/from socket
* PARAMETERS:
* RETURNS
*****************************************************************************/
void thread_main_MPE(void) {
  BL ret, SE_ret;
  mtk_int8 init_status;
  mtk_int32 ret1 = MPE_SUCCESS;
  mtk_uint16 SE_data_cnt;
  mtk_uint32 res = MPE_SUCCESS;
  int tmp_data_len, tt;
  int log_rec, rec_loc;
  MPE_MSG *msg, *msg1;
  mtk_int32 run_source, SE_run_rate, gps_sec, leap_sec;
  int SE_pause_mode, MPE_op_mode, SE_data_copy_ret;
  MNL_location_input_t mnl_in;
  FLP_location_input_t flp_in;
  MNL_location_output_t mnl_out;
  FLP_location_output_t flp_out;
  IMU* SE_data = NULL;

   //FLPD("Start of MPE main thread");

  //variables initialization
  memset(&_global_sensor_raw[0],0,BATCH_LOCAL_SIZE*sizeof(MPE_SENSOR_RAW));
  memset(&_global_sensor_k[0],0,BATCH_LOCAL_SIZE*sizeof(MPE_SENSOR_CALIBRATED));
  memset(&_global_sensor_fusion[0],0,BATCH_LOCAL_SIZE*sizeof(MPE_SENSOR_FUSION));
  memset(&mnl_out, 0, sizeof(MNL_location_output_t));
  memset(&flp_out, 0, sizeof(FLP_location_output_t));

  write_cnt_sensorRaw = 0;
  write_cnt_sensorK = 0;
  write_cnt_sensorFused = 0;
  read_cnt_sensorRaw = 0;
  read_cnt_sensorK = 0;
  read_cnt_sensorFused = 0;
  isSensorRaw_overwrite = FALSE;
  isSensorK_overwrite = FALSE;
  isSensorFused_overwrite = FALSE;

  signal(SIGPIPE, check_pipe_handler);
  client_to_server = open(myfifo, O_WRONLY);

  while( !g_ThreadExitMPE )
  {
    //recv msg
    ret1 = MPE_sys_msg_recv(MPE_TASKID_MAIN, &msg);
    msg1 = NULL; //reset msg1 content for each run

    if (MPE_NO_MSG_RECEIVED == ret1) {
    	FLPD("no msg received");
    } else if (MPE_SUCCESS == ret1) {
    	if(msg == NULL) {
    		FLPD("msg null \n");
      } else {
      	//Debug code, disable SE
        if( msg->type == CMD_START_MPE_REQ ) {
        	memcpy( &run_source,((mtk_int8 *)msg) + sizeof(MPE_MSG), sizeof(mtk_int32) );
          memcpy( &SE_run_rate,((mtk_int8 *)msg) + sizeof(MPE_MSG) + sizeof(mtk_int32), sizeof(mtk_int32) );
          memcpy( &gps_sec,((mtk_int8 *)msg) + sizeof(MPE_MSG)+ 2*sizeof(mtk_int32), sizeof(mtk_int32) );
          memcpy( &leap_sec,((mtk_int8 *)msg) + sizeof(MPE_MSG) + 3*sizeof(mtk_int32), sizeof(mtk_int32) );
          FLPD("run rate =%ld, source =%ld, gps_sec =%ld, leap_sec=%ld\n", SE_run_rate,run_source, gps_sec, leap_sec);

          //send response to client for successful startup
          msg1 = MPE_Mem_Alloc( (mtk_uint16)(sizeof(MPE_MSG)) );
          msg1->length = 0;
          msg1->type = CMD_START_MPE_RES;
          if(run_source == MPE_LOC_SOURCE_FLP) {
          	MPE_sys_socket_send( mpe_flp_socket_server_fd, msg1,sock_mpeflp_srv_addr );
          } else if(run_source == MPE_LOC_SOURCE_GNSS) {
          	MPE_sys_socket_send( mpe_mnl_socket_server_fd, msg1,sock_mpemnl_srv_addr );
          }
          //reset pipe
          pipe_close = 0;
          if( client_to_server == -1) {
          	client_to_server = open(myfifo, O_WRONLY);
          	FLPD("reopen client_to_server 1, %d \n", client_to_server);
          }
          if(!isInit) {
          	//run setting
            isRunning = TRUE;
            SE_listener_mode = SE_sensor_run(gps_sec, leap_sec);
            isRunning_source |= run_source;
            FLPD("SE run = %d, rate =%d", SE_listener_mode, SE_run_rate);

            if(SE_listener_mode == MPE_RUNTIME_MODE) {//isInit is false for other mode since selistener will not be deinit automatically
            	//Init SensorExplorer+ ksensor lib
              isInit = TRUE;
            }
            //send response to client for successful startup
            msg1 = MPE_Mem_Alloc( (mtk_uint16)(sizeof(MPE_MSG)) );
            msg1->length = 0;
            msg1->type = CMD_START_MPE_RES;
            if(run_source == MPE_LOC_SOURCE_FLP) {
            	MPE_sys_socket_send( mpe_flp_socket_server_fd, msg1,sock_mpeflp_srv_addr );
            } else if(run_source == MPE_LOC_SOURCE_GNSS) {
            	MPE_sys_socket_send( mpe_mnl_socket_server_fd, msg1,sock_mpemnl_srv_addr );
            }
          } else {
          	if( !(isRunning_source & run_source)) {//record initialization from multiple sources
          		isRunning_source |= run_source;
            }
            FLPD("SE continue run= %d",run_source);

            //send response to client for successful startup
            msg1 = MPE_Mem_Alloc( (mtk_uint16)(sizeof(MPE_MSG)) );
            msg1->length = 0;
            msg1->type = CMD_START_MPE_RES;
            if(run_source == MPE_LOC_SOURCE_FLP) {
                MPE_sys_socket_send( mpe_flp_socket_server_fd, msg1,sock_mpeflp_srv_addr );
            } else if(run_source == MPE_LOC_SOURCE_GNSS) {
                MPE_sys_socket_send( mpe_mnl_socket_server_fd, msg1,sock_mpemnl_srv_addr );
            }
          }
        } else if( msg->type == CMD_STOP_MPE_REQ ) {
        	SE_pause_mode = 1; //set full stop for all case
          memcpy( &run_source,((mtk_int8 *)msg) +sizeof(MPE_MSG), sizeof(mtk_int32) );
          FLPD("trigger uninit se, %ld %d", run_source, isRunning_source);

          isRunning_source ^= MPE_LOC_SOURCE_GNSS; // remove current client
          dr_flag = -1;

          if(client_to_server != -1) {
          	FLPD("close client_to_server %d \n",client_to_server);
            close(client_to_server);
            client_to_server = -1;
          }
          //Pause SensorExplorer
          isRunning = FALSE;
          isInit = FALSE;
          ret = SE_sensor_stop(); //full stop ==1, partial stop ==0
          MPE_Msg_Ring_Buf_Init((MPE_TASKID_E)MPE_TASKID_MAIN);
          FLPD("SE stop = %d", ret);
          SE_listener_mode = MPE_ERROR_MODE; //reset listener mode to enable first ADR flag can be sent to mnld upon next startup
          //send response to client for successful stop
          if(!isUninit_SE) { //return RES if uninit not triggered by SE
          	msg1 = MPE_Mem_Alloc( (mtk_uint16)(sizeof(MPE_MSG)) );
            msg1->length = 0;
            msg1->type = CMD_STOP_MPE_RES;
            FLPD("uninit se type, source %d\n",run_source);
            MPE_sys_socket_send( mpe_mnl_socket_server_fd, msg1,sock_mpemnl_srv_addr );
          } else {
          	isUninit_SE = FALSE;
            FLPD("uninit se type automatically \n");
          }
        } else if( msg->type == CMD_SET_MPE_MODE ) {
        	memcpy( &MPE_op_mode,((mtk_int8 *)msg) +sizeof(MPE_MSG), sizeof(int) );
          FLPD("MPE operation mode =%d", MPE_op_mode);
        } else if( msg->type == CMD_GET_ADR_STATUS_REQ ) {
        	memcpy( &mnl_in,((mtk_int8*)msg)+sizeof(MPE_MSG), msg->length );
          //Call SE lib to check buffer size & get SE data
          gps_sec = mnl_in.gps_sec;
          leap_sec = mnl_in.leap_sec;

          if(isInit){
          	SE_data_cnt = SE_Acquire_data_cnt(gps_sec, leap_sec, &SE_listener_mode);
            FLPD("SE data size  =%d, gps_sec=%ld, leap_sec=%ld, mode =%d \n",SE_data_cnt,gps_sec, leap_sec, SE_listener_mode);


                 #if 1
                 //Return DR result
                 mpe_mnl_out_set(&mnl_in, &mnl_out);
                 msg1 = MPE_Mem_Alloc( (mtk_uint16)(sizeof(MPE_MSG)+ sizeof(MNL_location_output_t)) );
                 msg1->length = sizeof(MNL_location_output_t);
                 msg1->type = CMD_SEND_ADR_STATUS_RES;
                 memcpy( ((mtk_int8*)msg1)+sizeof(MPE_MSG), &mnl_out, sizeof(MNL_location_output_t));
                 FLPD("return ADR success, vout=%d, baro %f", dr_status, abs_height);
                 MPE_sys_socket_send( mpe_mnl_socket_server_fd, msg1,sock_mpemnl_srv_addr );
                 #endif
          }
        } else if( msg->type == CMD_GET_PDR_STATUS_REQ ) {
        	memcpy( &flp_in,((mtk_int8*)msg)+sizeof(MPE_MSG), msg->length );
        	//Return DR result
          msg1 = MPE_Mem_Alloc( (mtk_uint16)(sizeof(MPE_MSG)+ sizeof(FLP_location_output_t)) );
          msg1->length = sizeof(FLP_location_output_t);
          msg1->type = CMD_SEND_PDR_STATUS_RES;
          flp_out.vout = (float)dr_status;
          memcpy( ((mtk_int8*)msg1)+sizeof(MPE_MSG), &flp_out, sizeof(FLP_location_output_t));
          MPE_sys_socket_send( mpe_flp_socket_server_fd, msg1,sock_mpeflp_srv_addr );
          FLPD("return PDR success");
        } else if( msg->type == CMD_SEND_FROM_MNLD ) {
        	memcpy(&log_rec,((mtk_int8*)msg)+sizeof(MPE_MSG),sizeof(int));
          memcpy(&rec_loc,((mtk_int8*)msg)+sizeof(MPE_MSG)+sizeof(int),sizeof(int));
          memcpy(mpe_debuglog_file_name,((mtk_int8*)msg)+sizeof(MPE_MSG)+2*sizeof(int),MPE_DEBUG_LOG_FILE_NAME_MAX_LEN);
          FLPD("log_rec =%d, rec_loc=%d, mpelog_path:%s", log_rec, rec_loc,mpe_debuglog_file_name );
          SE_mtklogger_check((mtk_int16)log_rec, &mpe_debuglog_file_name, (mtk_int8)rec_loc);
        } else if( msg->type == CMD_DEINIT_MPE_REQ ) {
        	g_ThreadExitMPE = TRUE;
          ret = SE_sensor_stop();
          FLPD("SE deinit = %d",ret);
          pthread_exit(NULL);
          return 0;
        } else {
        	FLPD("invalid msg type = %d",msg->type);
        }
        MPE_mem_free(msg);
      }
    }
  }
  return 0;
}
/*****************************************************************************
* FUNCTION
*  MPE_multiple_socket_connect
* DESCRIPTION
*   build socket connection with other client, curently FLP & MNL
* PARAMETERS:
* RETURNS
*    success/fail
*****************************************************************************/
static int MPE_multiple_socket_connect( void ) {
  char cmd[16], rsp[1024];
  int rsplen=0, ret = MPE_ERROR;
  //Connect to MNL FD
  if((mpe_mnl_socket_fd = socket(AF_LOCAL, SOCK_DGRAM, 0)) == -1) {
  	FLPD("mpe mnl open udp sock failed\r\n");
  } else {
  	FLPD("mpe_mnl_socket_fd =%d",mpe_mnl_socket_fd);
    unlink(MPEMNL_UDP_CLI_PATH);
    memset(&sock_mpemnl_cli_addr, 0, sizeof(sock_mpemnl_cli_addr));
    sock_mpemnl_cli_addr.sun_family = AF_LOCAL;
    strcpy(sock_mpemnl_cli_addr.sun_path, MPEMNL_UDP_CLI_PATH);
    FLPD("server_fd,%d\r\n",mpe_mnl_socket_fd);
    FLPD("server_addr.sun_path,%s\r\n",sock_mpemnl_cli_addr.sun_path);
    FLPD("server_addr.sun_family,%d\r\n",sock_mpemnl_cli_addr.sun_family);

    if(bind(mpe_mnl_socket_fd, (const struct sockaddr *) &sock_mpemnl_cli_addr, sizeof(sock_mpemnl_cli_addr)) < 0) {
    	close(mpe_mnl_socket_fd);
      FLPD("mpe mnl server: bind");
      mpe_mnl_socket_fd = -1;
    } else {
    	int res = chmod(MPEMNL_UDP_CLI_PATH, 0660);// chmod(MPEMNL_UDP_CLI_PATH, 0777);
    	chown(MPEMNL_UDP_CLI_PATH, -1, AID_INET);
      if( res < 0 ) {
          FLPD("mpe mnl chmod error:%d", res);
      } else {
      	if((mpe_mnl_socket_server_fd = socket(AF_LOCAL, SOCK_DGRAM, 0)) < 0) {
      		FLPD("open server sock failed\r\n");
          return -1;
        }
        memset(&sock_mpemnl_srv_addr, 0, sizeof(sock_mpemnl_srv_addr));
        sock_mpemnl_srv_addr.sun_family = AF_LOCAL;
        strcpy(sock_mpemnl_srv_addr.sun_path, MPEMNL_UDP_SRV_PATH);
        //Debug print
        FLPD("mpe_mnl_socket_server_fd,%d\r\n",mpe_mnl_socket_server_fd);
        FLPD("sock_mpemnl_srv_addr.sun_path,%s\r\n",sock_mpemnl_srv_addr.sun_path);
        FLPD("sock_mpemnl_srv_addr.sun_family,%d\r\n",sock_mpemnl_srv_addr.sun_family);
      }
    }
  }
  //Connect to FLP FD
#if 0
  if((mpe_flp_socket_fd = socket(AF_LOCAL, SOCK_DGRAM, 0)) == -1) {
  	FLPD("mpe flp open udp sock failed\r\n");
  } else {
  	FLPD("mpe_flp_socket_fd =%d",mpe_flp_socket_fd);
    unlink(MPEFLP_UDP_CLI_PATH);
    memset(&sock_mpeflp_cli_addr, 0, sizeof(sock_mpeflp_cli_addr));
    sock_mpeflp_cli_addr.sun_family = AF_LOCAL;
    strcpy(sock_mpeflp_cli_addr.sun_path, MPEFLP_UDP_CLI_PATH);
    FLPD("server_fd,%d\r\n",mpe_flp_socket_fd);
    FLPD("server_addr.sun_path,%s\r\n",sock_mpeflp_cli_addr.sun_path);
    FLPD("server_addr.sun_family,%d\r\n",sock_mpeflp_cli_addr.sun_family);

    if(bind(mpe_flp_socket_fd, (const struct sockaddr *) &sock_mpeflp_cli_addr, sizeof(sock_mpeflp_cli_addr)) < 0) {
    	close(mpe_flp_socket_fd);
      FLPD("mpe flp server: bind");
      mpe_flp_socket_fd = -1;
    } else {
    	int res = chmod(MPEFLP_UDP_CLI_PATH, 0777);
      if( res < 0 ) {
      	FLPD("mpe flp chmod error:%d", res);
      } else {
      	if((mpe_flp_socket_server_fd = socket(AF_LOCAL, SOCK_DGRAM, 0)) < 0) {
      		FLPD("open server sock failed\r\n");
          return -1;
        }
      memset(&sock_mpeflp_srv_addr, 0, sizeof(sock_mpeflp_srv_addr));
      sock_mpeflp_srv_addr.sun_family = AF_LOCAL;
      strcpy(sock_mpeflp_srv_addr.sun_path, MPEFLP_UDP_SRV_PATH);
      }
    }
  }
#endif
  return ret;
}
/*****************************************************************************
* FUNCTION
*  thread_socket_MPE
* DESCRIPTION
*   Main Socket thread of MPE, send/recieve message to/from MNL/FLP or other client
* PARAMETERS:
* RETURNS
*****************************************************************************/
void *thread_socket_MPE(void * arg) {
  MPE_TASK_T* taskp = MPE_find_task(MPE_TASKID_SOCKET);
  mtk_int32 ret = MPE_SUCCESS;
  MPE_MSG *mpe_msg;
  mtk_int32 socket_ready = C_INVALID_FD;
  mtk_int32 sock_list[2];
  char sockbuff[128];
  struct sockaddr_un sock_addr[2];
  //test socket in L
  char test_buff[128];
  MPE_MSG test_msg;
  mtk_int32 test_i4SendLen;
  int test_ret, status, on = 1;
  int server_fd = C_INVALID_SOCKET;
  int size;/*socklen_t*/
  struct sockaddr_un server_addr;
  struct sockaddr_un client_addr;

  //FLPD("flp_socket_thread, Create\n");
  memset(&server_addr, '\0', sizeof(struct sockaddr_un));
  test_ret = unlink (MTKMPE_COMM_SOCK_PATH);
  if( test_ret == -1)
  	FLPD("test_ret = %d, %s", test_ret, strerror(errno));

  //----------------------------------------------------------------
  // Create a SOCKET for listening for incoming connection requests.
  //----------------------------------------------------------------
  if ((server_fd = socket(AF_LOCAL, SOCK_STREAM, 0)) == -1){
  	FLPD("socket error\r\n");
    g_ThreadExitSocket = TRUE;
    pthread_exit(NULL);
  }
  //----------------------------------------------------------------
  // The sockaddr_in structure specifies the address family,
  // IP address, and port for the socket that is being bound.
  //----------------------------------------------------------------
  //Bind and listen
  server_addr.sun_family = AF_LOCAL;
  strcpy (server_addr.sun_path, MTKMPE_COMM_SOCK_PATH);
  FLPD("server_fd,%d\r\n",server_fd);
  FLPD("server_addr.sun_path,%s\r\n",server_addr.sun_path);
  FLPD("server_addr.sun_family,%d\r\n",server_addr.sun_family);
  status = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on));

  if (status == -1)
  	FLPD("setsockopt() error %s", strerror(errno));

  if((bind (server_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un))) == -1) {
  	FLPD("bind error\r\n");
    FLPD("fail to get socket from environment: %s\n",strerror(errno));
    close(server_fd);
    g_ThreadExitSocket = TRUE;
    pthread_exit(NULL);
  }

  int res;
  res = chmod(MTKMPE_COMM_SOCK_PATH, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP);
  FLPD("chmod1,%d..\r\n",res);

  //----------------------------------------------------------------
  // Listen for incoming connection requests on the created socket
  //----------------------------------------------------------------
  FLPD("start listen...(server_fd,%d)\r\n", server_fd);
  if(listen (server_fd, 5) == -1) {
  	FLPD("listent fail: %s\n",strerror(errno));
    close(server_fd);
    g_ThreadExitSocket = TRUE;
    pthread_exit(NULL);
  }
  //Init the socket connection between MPE & FLP + MNL
  MPE_multiple_socket_connect();
  sock_list[0] = mpe_mnl_socket_fd;
  sock_list[1] = mpe_flp_socket_fd;//mpe_mnl_socket_server_fd;
  sock_addr[0] = sock_mpemnl_cli_addr;
  sock_addr[1] = sock_mpeflp_cli_addr;

  //FLPD("socket fds = %d, %d", mpe_mnl_socket_fd, mpe_flp_socket_fd);

  while (!g_ThreadExitSocket) {
  	ret = MPE_sys_socket_recv_msg(sock_list, sock_addr, sockbuff);
    if ((ret>0 ) && (!g_ThreadExitSocket)) {
    	mpe_msg = MPE_Mem_Alloc((mtk_uint16)ret);
      memcpy(mpe_msg, &sockbuff[0], ret );
      FLPD("flp_socket_thread, read msg ok (type,%d,len:%d)\n", mpe_msg->type, mpe_msg->length);
      //Process received message from socket
      MPE_sys_msg_send(MPE_TASKID_MAIN,mpe_msg);
    } else {
    	FLPD("flp_socket_thread, read msg fail,exit socket thread\n");
      //read msg fail...
      g_ThreadExitSocket = TRUE;
      // [Remind] Only for Blocking-read-mechanism.
      // Since flp_socket_thread is closed , send MTK_FLP_EXIT_REQ message to service.
      // In abnormal case : This mechanism can avoid zombie process.
      mpe_msg = MPE_Mem_Alloc( sizeof(MPE_MSG) );
      mpe_msg->type = CMD_DEINIT_MPE_REQ;
      mpe_msg->length = 0;
      FLPD("MPE_socket_thread, send msg to service exit req\n");
      MPE_sys_msg_send(MPE_TASKID_MAIN, mpe_msg);
    }
  }
  //----------------------------------------------------------------
  // Close socket
  //----------------------------------------------------------------
  FLPD("closing socket fd[%d],%02x\r\n",taskp->sockethandle);
  close(taskp->sockethandle);
  taskp->sockethandle = C_INVALID_SOCKET;
  unlink (MTKMPE_COMM_SOCK_PATH);

  FLPD("MPE_socket_thread, exit\n");
  g_ThreadExitSocket = TRUE;
  pthread_exit(NULL);
  return NULL;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_read_sensor_raw
 * DESCRIPTION
 *  Output sensor raw data & len info
 * PARAMETERS
 * RETURNS
 *  data: succeded/failed
 *****************************************************************************/
BL MPE_read_sensor_raw( void ) {
  BL ret = TRUE;
  int total_len, tmp_size;
  MPE_SENSOR_RAW sensor_raw[BATCH_LOCAL_SIZE];
  MPE_MSG *msg1;

  MPE_take_mutex(&gMPETasks[MPE_TASKID_SOCKET].mutex);

  if( read_cnt_sensorRaw < write_cnt_sensorRaw ) {//new data updated to global
  	total_len = (write_cnt_sensorRaw - read_cnt_sensorRaw);
    memcpy(&sensor_raw[0],&_global_sensor_raw[read_cnt_sensorRaw],total_len*sizeof(MPE_SENSOR_RAW));
    read_cnt_sensorRaw = write_cnt_sensorRaw;
    //send msg to socket
    msg1 = MPE_Mem_Alloc( (mtk_uint16)(sizeof(MPE_MSG)+ total_len*sizeof(MPE_SENSOR_RAW)) );
    msg1->length = total_len*sizeof(MPE_SENSOR_RAW);
    msg1->type = CMD_SEND_SENSOR_RAW_RES;
    memcpy( ((mtk_int8*)msg1)+sizeof(MPE_MSG), &sensor_raw, total_len*sizeof(MPE_SENSOR_RAW));
    MPE_sys_socket_send( mpe_flp_socket_server_fd, msg1,sock_mpeflp_srv_addr );
  } else if ( read_cnt_sensorRaw > write_cnt_sensorRaw ) {//data roll over
  	if( isSensorRaw_overwrite ) {
  		tmp_size  = BATCH_LOCAL_SIZE - read_cnt_sensorRaw;
      memcpy(&sensor_raw[0],&_global_sensor_raw[read_cnt_sensorRaw],tmp_size*sizeof(MPE_SENSOR_RAW)); //read from read_cnt to max + read from 0 to write_cnt
      memcpy(&sensor_raw[tmp_size],&_global_sensor_raw[0],write_cnt_sensorRaw*sizeof(MPE_SENSOR_RAW));
      total_len = tmp_size + write_cnt_sensorRaw;
      read_cnt_sensorRaw = write_cnt_sensorRaw;
      isSensorRaw_overwrite = FALSE;
      //send msg to socket
      msg1 = MPE_Mem_Alloc( (mtk_uint16)(sizeof(MPE_MSG)+ total_len*sizeof(MPE_SENSOR_RAW)) );
      msg1->length = total_len*sizeof(MPE_SENSOR_RAW);
      msg1->type = CMD_SEND_SENSOR_RAW_RES;
      memcpy( ((mtk_int8*)msg1)+sizeof(MPE_MSG), &sensor_raw, total_len*sizeof(MPE_SENSOR_RAW));
      MPE_sys_socket_send( mpe_flp_socket_server_fd, msg1,sock_mpeflp_srv_addr );
    } else {
    	ret = FALSE;
      FLPD("error reading sensor raw, overwrite wrong");
    }
  } else if( read_cnt_sensorRaw == write_cnt_sensorRaw) {//no new data updated to global
  	ret = FALSE;
    FLPD("error reading sensor raw, no update");
  }
  MPE_give_mutex(&gMPETasks[MPE_TASKID_SOCKET].mutex);
  return ret;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_read_sensor_k
 * DESCRIPTION
 *  Output calibrated sensor data & len info
 * PARAMETERS
 * RETURNS
 *  data: succeded/failed
 *****************************************************************************/
BL MPE_read_sensor_k( void ) {
  BL ret = TRUE;
  int total_len, tmp_size;
  MPE_MSG *msg1;
  MPE_SENSOR_CALIBRATED sensor_k_data[BATCH_LOCAL_SIZE];
  MPE_take_mutex(&gMPETasks[MPE_TASKID_SOCKET].mutex);

  if( read_cnt_sensorK < write_cnt_sensorK) {//new data updated to global
  	total_len = (write_cnt_sensorK - read_cnt_sensorK);
    memcpy(&sensor_k_data[0],&_global_sensor_k[read_cnt_sensorK],total_len*sizeof(MPE_SENSOR_CALIBRATED));
    read_cnt_sensorK = write_cnt_sensorK;
    //send msg to socket
    msg1 = MPE_Mem_Alloc( (mtk_uint16)(sizeof(MPE_MSG)+ total_len*sizeof(MPE_SENSOR_CALIBRATED)) );
    msg1->length = total_len*sizeof(MPE_SENSOR_CALIBRATED);
    msg1->type = CMD_SEND_SENSOR_CALIBRATION_RES;
    memcpy( ((mtk_int8*)msg1)+sizeof(MPE_MSG), &sensor_k_data, total_len*sizeof(MPE_SENSOR_CALIBRATED));
    MPE_sys_socket_send( mpe_flp_socket_server_fd, msg1,sock_mpeflp_srv_addr );
  } else if ( read_cnt_sensorK > write_cnt_sensorK ) {//data roll over
  	if( isSensorK_overwrite ) {
  		tmp_size  = BATCH_LOCAL_SIZE - read_cnt_sensorK;
      memcpy(&sensor_k_data[0],&_global_sensor_k[read_cnt_sensorK],tmp_size*sizeof(MPE_SENSOR_CALIBRATED)); //read from read_cnt to max + read from 0 to write_cnt
      memcpy(&sensor_k_data[tmp_size],&_global_sensor_k[0],write_cnt_sensorK*sizeof(MPE_SENSOR_CALIBRATED));
      total_len = tmp_size + write_cnt_sensorK;
      read_cnt_sensorK = write_cnt_sensorK;
      isSensorK_overwrite = FALSE;
      //send msg to socket
      msg1 = MPE_Mem_Alloc( (mtk_uint16)(sizeof(MPE_MSG)+ total_len*sizeof(MPE_SENSOR_CALIBRATED)) );
      msg1->length = total_len*sizeof(MPE_SENSOR_CALIBRATED);
      msg1->type = CMD_SEND_SENSOR_CALIBRATION_RES;
      memcpy( ((mtk_int8*)msg1)+sizeof(MPE_MSG), &sensor_k_data, total_len*sizeof(MPE_SENSOR_CALIBRATED));
      MPE_sys_socket_send( mpe_flp_socket_server_fd, msg1,sock_mpeflp_srv_addr );
      } else {
      	ret = FALSE;
        FLPD("error reading sensor k, overwrite wrong");
      }
  } else if( read_cnt_sensorK == write_cnt_sensorK) {//no new data updated to global
  	ret = FALSE;
    FLPD("error reading sensor k, no update");
  }
  MPE_give_mutex(&gMPETasks[MPE_TASKID_SOCKET].mutex);
  return ret;
}
/*****************************************************************************
 * FUNCTION
 *  MPE_read_sensor_fusion
 * DESCRIPTION
 *  Output sensor fusion data & len info
 * PARAMETERS
 * RETURNS
 *  data: succeded/failed
 *****************************************************************************/
BL MPE_read_sensor_fusion( void ) {
  BL ret = TRUE;
  int total_len, tmp_size;
  MPE_MSG *msg1;
  MPE_SENSOR_FUSION sensor_fusion[BATCH_LOCAL_SIZE];

  MPE_take_mutex(&gMPETasks[MPE_TASKID_SOCKET].mutex);

  if( read_cnt_sensorFused < write_cnt_sensorFused ) {//new data updated to global
  	total_len = (write_cnt_sensorFused- read_cnt_sensorFused );
    memcpy(&sensor_fusion[0],&_global_sensor_fusion[read_cnt_sensorFused],total_len*sizeof(MPE_SENSOR_FUSION));
    read_cnt_sensorFused = write_cnt_sensorFused;
    //send msg to socket
    msg1 = MPE_Mem_Alloc( (mtk_uint16)(sizeof(MPE_MSG)+ total_len*sizeof(MPE_SENSOR_FUSION)) );
    msg1->length = total_len*sizeof(MPE_SENSOR_FUSION);
    msg1->type = CMD_SEND_SENSOR_FUSION_RES;
    memcpy( ((mtk_int8*)msg1)+sizeof(MPE_MSG), &sensor_fusion, total_len*sizeof(MPE_SENSOR_FUSION));
    MPE_sys_socket_send( mpe_flp_socket_server_fd, msg1,sock_mpeflp_srv_addr );
  } else if ( read_cnt_sensorFused > write_cnt_sensorFused ) {//data roll over
  	if( isSensorFused_overwrite ) {
  		tmp_size  = BATCH_LOCAL_SIZE - read_cnt_sensorFused ;
      memcpy(&sensor_fusion[0],&_global_sensor_fusion[read_cnt_sensorFused],tmp_size*sizeof(MPE_SENSOR_FUSION)); //read from read_cnt to max + read from 0 to write_cnt
      memcpy(&sensor_fusion[tmp_size],&_global_sensor_fusion[0],write_cnt_sensorFused*sizeof(MPE_SENSOR_FUSION));
      total_len = tmp_size + write_cnt_sensorFused;
      read_cnt_sensorFused = write_cnt_sensorFused;
      isSensorFused_overwrite = FALSE;
      //send msg to socket
      msg1 = MPE_Mem_Alloc( (mtk_uint16)(sizeof(MPE_MSG)+ total_len*sizeof(MPE_SENSOR_FUSION)) );
      msg1->length = total_len*sizeof(MPE_SENSOR_FUSION);
      msg1->type = CMD_SEND_SENSOR_FUSION_RES;
      memcpy( ((mtk_int8*)msg1)+sizeof(MPE_MSG), &sensor_fusion, total_len*sizeof(MPE_SENSOR_FUSION));
      MPE_sys_socket_send( mpe_flp_socket_server_fd, msg1,sock_mpeflp_srv_addr );
    } else {
    	ret = FALSE;
      FLPD("error reading sensor fusion, overwrite wrong");
    }
  } else if( read_cnt_sensorFused == write_cnt_sensorFused ) {//no new data updated to global
  	ret = FALSE;
    FLPD("error reading sensor fusion, no update");
  }
  MPE_give_mutex(&gMPETasks[MPE_TASKID_SOCKET].mutex);
  return ret;
}

void MPE_data_converter(IMU* sensor_data, MPE_SENSOR_RAW* sensor_raw,MPE_SENSOR_CALIBRATED* sensor_k, mtk_uint16 len ) {
  mtk_uint16 i;

  for(i=0; i<len; i++) {
  	//sensor raw
    sensor_raw[i].accelerometer_raw[0] = sensor_data[i].acceleration_raw[0];
    sensor_raw[i].accelerometer_raw[1] = sensor_data[i].acceleration_raw[1];
    sensor_raw[i].accelerometer_raw[2] = sensor_data[i].acceleration_raw[2];
    sensor_raw[i].gyro_raw[0] = sensor_data[i].angularVelocity_raw[0];
    sensor_raw[i].gyro_raw[1] = sensor_data[i].angularVelocity_raw[1];
    sensor_raw[i].gyro_raw[2] = sensor_data[i].angularVelocity_raw[2];
    sensor_raw[i].magnetometer_raw[0] = sensor_data[i].magnetic_raw[0];
    sensor_raw[i].magnetometer_raw[1] = sensor_data[i].magnetic_raw[1];
    sensor_raw[i].magnetometer_raw[2] = sensor_data[i].magnetic_raw[2];
    //sensor k
    sensor_k[i].accelerometer_k[0] = sensor_data[i].acceleration[0];
    sensor_k[i].accelerometer_k[1] = sensor_data[i].acceleration[1];
    sensor_k[i].accelerometer_k[2] = sensor_data[i].acceleration[2];
    sensor_k[i].gyro_k[0] = sensor_data[i].angularVelocity[0];
    sensor_k[i].gyro_k[1] = sensor_data[i].angularVelocity[1];
    sensor_k[i].gyro_k[2] = sensor_data[i].angularVelocity[2];
    sensor_k[i].magnetometer_k[0] = sensor_data[i].magnetic[0];
    sensor_k[i].magnetometer_k[1] = sensor_data[i].magnetic[1];
    sensor_k[i].magnetometer_k[2] = sensor_data[i].magnetic[2];
  }
  return;
}
#if 0
/*****************************************************************************
 * FUNCTION
 *  MPE_data_batch
 * DESCRIPTION
 *  Batch sensor raw, calibration, fusion data to global variables
 * PARAMETERS
 *  1). sensor data
 *  2). sensor fusion data (Euler Angles)
 * RETURNS
 *  data: none
 *****************************************************************************/
void MPE_data_batch(IMU* sensor_data, MPE_SENSOR_FUSION* attitude, mtk_uint16 len) {
  mtk_uint16 i;
  MPE_SENSOR_RAW* sensor_raw;
  MPE_SENSOR_CALIBRATED* sensor_k;

  if( (sensor_data == NULL) || (attitude == NULL)) {
  	FLPD("MPE_data_batch error");
    return;
  }

  sensor_raw = MPE_Mem_Alloc(len*sizeof(MPE_SENSOR_RAW));
  sensor_k = MPE_Mem_Alloc(len*sizeof(MPE_SENSOR_CALIBRATED));

  if((sensor_raw == NULL) || (sensor_k == NULL )) {
  	FLPD("data batch alloc failed\n");
    return;
  }
  MPE_data_converter(sensor_data,sensor_raw,sensor_k,len);
  MPE_take_mutex(&gMPETasks[MPE_TASKID_SOCKET].mutex);

  if( (write_cnt_sensorRaw + len ) > BATCH_LOCAL_SIZE) {
  	memcpy(&_global_sensor_raw[write_cnt_sensorRaw], sensor_raw, (BATCH_LOCAL_SIZE-write_cnt_sensorRaw)*sizeof(MPE_SENSOR_RAW));
    memcpy(&_global_sensor_raw[0], sensor_raw+(BATCH_LOCAL_SIZE-write_cnt_sensorRaw), (len - BATCH_LOCAL_SIZE+write_cnt_sensorRaw)*sizeof(MPE_SENSOR_RAW));
    memcpy(&_global_sensor_k[write_cnt_sensorK], sensor_k, (BATCH_LOCAL_SIZE-write_cnt_sensorK)*sizeof(MPE_SENSOR_CALIBRATED));
    memcpy(&_global_sensor_k[0], sensor_k+(BATCH_LOCAL_SIZE-write_cnt_sensorK), (len - BATCH_LOCAL_SIZE+write_cnt_sensorK)*sizeof(MPE_SENSOR_CALIBRATED));
    memcpy(&_global_sensor_fusion[write_cnt_sensorFused], attitude, (BATCH_LOCAL_SIZE-write_cnt_sensorFused)*sizeof(MPE_SENSOR_FUSION));
    memcpy(&_global_sensor_fusion[0], attitude+(BATCH_LOCAL_SIZE-write_cnt_sensorFused), (len - BATCH_LOCAL_SIZE+write_cnt_sensorFused)*sizeof(MPE_SENSOR_FUSION));
  } else {
  	memcpy(&_global_sensor_raw[write_cnt_sensorRaw], sensor_raw, len*sizeof(MPE_SENSOR_RAW));
    memcpy(&_global_sensor_k[write_cnt_sensorK], sensor_k, len*sizeof(MPE_SENSOR_CALIBRATED));
    memcpy(&_global_sensor_fusion[write_cnt_sensorFused], attitude, len*sizeof(MPE_SENSOR_FUSION));
  }

  write_cnt_sensorRaw += len;
  write_cnt_sensorK += len;
  write_cnt_sensorFused +=len;

  if( write_cnt_sensorRaw >= BATCH_LOCAL_SIZE ) {
  	write_cnt_sensorRaw -= BATCH_LOCAL_SIZE ;
    write_cnt_sensorK -= BATCH_LOCAL_SIZE;
    write_cnt_sensorFused -= BATCH_LOCAL_SIZE;
    isSensorRaw_overwrite  = TRUE;
    isSensorK_overwrite  = TRUE;
    isSensorFused_overwrite  = TRUE;
  }
  MPE_give_mutex(&gMPETasks[MPE_TASKID_SOCKET].mutex);
  MPE_mem_free(sensor_raw);
  MPE_mem_free(sensor_k);
}
#endif

/*****************************************************************************
 * FUNCTION
 *  MPE_sensor_cb
 * DESCRIPTION
 *  Callback function register in sensor driver, enabled upon sensor raw data recipient, calculate fusion data & batching to global
 * PARAMETERS
 *  1). sensor data
 *  2). sensor len
 * RETURNS
 *  data: successed or failed
 *****************************************************************************/
int MPE_sensor_cb(IMU* data, mtk_uint16 len) {
  int i, ret;
  float attitude[3];
  int64_t dt = 0;

//IMU* g_local_ksensor;
  MPE_SENSOR_FUSION* local_attitude;

//mutex
  if(len <0) {
  	FLPD("MPE_sensor_len error = %d\n",len);
    return MPE_ERROR;
  }

  if(data == NULL) {
  	FLPD("MPE_sensor data NULL\n");
    return MPE_ERROR;
  }
  local_attitude = MPE_Mem_Alloc(len*sizeof(MPE_SENSOR_FUSION));
  memset(local_attitude,0,len*sizeof(MPE_SENSOR_FUSION));

  if( (data == NULL) || (local_attitude == NULL)) {
  	FLPD("allocate sensor cb error\n");
    return MPE_ERROR;
  }

  for( i=0; i<len; i++ ) {
  	if(data+i != NULL) {
  		if(i==0) {
  			dt = GYRO_RATE*1000000;
      } else {
      	dt = data[i].input_time_acc - data[i-1].input_time_acc;
      }
      ret = MPE_API_fusion_update_attitude(data+i,&attitude,(int)(dt/1000.f));

      #ifdef __ANDROID_AXIS__
          local_attitude[i].yaw = attitude[0];
          local_attitude[i].roll = attitude[1];
          local_attitude[i].pitch = attitude[2];
      #else
          local_attitude[i].yaw = attitude[0];
          local_attitude[i].roll = attitude[2];
          local_attitude[i].pitch = attitude[1];
      #endif
      dr_status = update_activity_monitor(data+i);
      mpe_update_altitude(data+i, &abs_height, &rel_height );
    } else {
          FLPD("accept null data\n");
    }
  }
  MPE_mem_free(local_attitude);
  return MPE_SUCCESS;
}

void mpe_mnl_out_set(MNL_location_input_t *mnl_in,MNL_location_output_t *mnl_out ) {
  mnl_out->lla_out[0] = mnl_in->lla_in[0];
  mnl_out->lla_out[1] = mnl_in->lla_in[1];
  mnl_out->lla_out[2] = mnl_in->lla_in[2];
  mnl_out->heading_out = 0;
  mnl_out->vout = dr_status;
  mnl_out->height = abs_height;

  if((SE_listener_mode == MPE_RUNTIME_MODE) || (SE_listener_mode == MPE_REPLAY_RF_MODE)) {
  	mnl_out->validation_flag = 1;
  } else {
  	mnl_out->validation_flag = 0;
  }
}

void mpe_uninit_sensor( void ) {
  mtk_int32 run_source;
  MPE_MSG *mpe_msg;

  mpe_msg = MPE_Mem_Alloc(sizeof(MPE_MSG) + sizeof(mtk_int32));
  mpe_msg->type = CMD_STOP_MPE_REQ;
  mpe_msg->length = sizeof(mtk_int32)+sizeof(MPE_MSG);
  run_source = MPE_LOC_SOURCE_GNSS;
  memcpy( ((mtk_int8 *)mpe_msg) + sizeof(MPE_MSG),&run_source, sizeof(mtk_int32) );
  isUninit_SE = TRUE;
  FLPD("send uninit request from listener \n");
  MPE_sys_msg_send(MPE_TASKID_MAIN,mpe_msg);
}

void mpe_run_algo( double input_time,  mtk_uint16 data_cnt) {
  BL ret;
  int data_ret, valid_flag;
  char name[128], str[128];
  IMU *SE_data;
  FILE *adr_file;

  SE_data = MPE_Mem_Alloc(data_cnt*sizeof(IMU));
  if( SE_data != NULL ) {
  	ret = SE_Acquire_Data(SE_data,data_cnt);
    data_ret = MPE_sensor_cb(SE_data, data_cnt);

    if((SE_listener_mode == MPE_RUNTIME_MODE) || (SE_listener_mode == MPE_REPLAY_RF_MODE)) {
    	valid_flag = 1;
    } else {
    	valid_flag = 0;
    }

    if(data_ret == MPE_SUCCESS) {
    	if((dr_flag != dr_status) || (current_run_mode != SE_listener_mode) ) {
    		FLPD("mpe_algo,%lf,%d,%d\n",input_time, dr_status, valid_flag);
        sprintf(str, "%lf %d %d", input_time, dr_status, valid_flag);

        if( client_to_server == -1) {
        	client_to_server = open(myfifo, O_WRONLY);
          FLPD("reopen client_to_server");
        }

        if( (client_to_server != -1)) {
        	if(!pipe_close) {
        		write(client_to_server, str, strlen(str));
          } else {
          	FLPD("pipe broken \n");
          }
        }
        dr_flag = dr_status;
        current_run_mode = SE_listener_mode;
          }
    }
    MPE_mem_free(SE_data);
  }
}

void check_pipe_handler( int signum ) {
  if(signum == SIGPIPE ) {
  	pipe_close = 1;
    FLPD("SIGPIPE recv \n");
  }
}
/*****************************************************************************
 * FUNCTION
 *  main
 * DESCRIPTION
 *  main function of MPE Daemon, create upon startup
 * PARAMETERS
 * RETURNS
 *  data: successed or failed
 *****************************************************************************/
int main (int argc ) {
  BL ret = FALSE;
  int res;

  FLPD("MPE Daemon main\n");

  //Init tasks & threads
  MPE_task_init();
  ret = MPE_threads_create();
  usleep(1000);
  FLPD("MPE thread create=%d\n",ret);

  if (TRUE == ret) {
  	thread_main_MPE();     //hal thread will run in main process
  } else {
  	FLPD("thread_main_MPE fail\n");
    g_ThreadExitSocket = TRUE;
    g_ThreadExitMPE =TRUE;
  }

  if(client_to_server != -1) {
  	close(client_to_server);
  }
  ret = MPE_task_uninit();
  res = MPE_threads_release();
  FLPD("MPE task & thread release =%d, %d", ret, res );
  return (MPE_SUCCESS);
}

#ifdef __cplusplus
  extern "C" }
#endif

#endif //#ifndef MPE_FUSION_MAIN_C
