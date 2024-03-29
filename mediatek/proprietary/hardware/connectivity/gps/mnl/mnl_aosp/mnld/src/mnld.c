/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*
* MediaTek Inc. (C) 2014. All rights reserved.
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

#define _MNLD_C_
#define MTK_LOG_ENABLE 1
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <cutils/properties.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sched.h>
#include <pthread.h>
#include <cutils/sockets.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <linux/mtk_agps_common.h>
#include <linux/mtcombo.h>
#include <linux/fm.h>
#include <hardware/gps.h>
#include <hardware_legacy/power.h>
#include <private/android_filesystem_config.h>

#include "mnl_common.h"
#include "mnl_linux.h"
#include "mtk_gps_driver_wrapper.h"
#include "mtk_gps.h"
#include "mnld_utile.h"
#include "mnl2agps_interface.h"
#include "mnl_agps_interface.h"
#include "mtk_gps_sys_fp.h"
#include "SUPL_encryption.h"
#include "CFG_GPS_File.h"
#include "mnl2nlps.h"
/*---------------------------------------------------------------------------*/
// for one binary
#define MPE_ENABLE
#define RAW_DATA_SUPPORT 1
#if RAW_DATA_SUPPORT
#define GPS_CONF_FILE_SIZE 100
#define RAW_DATA_CONTROL_FILE_PATH "/data/misc/gps.conf"
static int gps_raw_debug_mode = 0;
static int mtk_msg_raw_meas_flag = 0;
#define IS_SPACE(ch) ((ch == ' ') || (ch == '\t') || (ch == '\n'))
#endif
#if 0
#ifdef GPS_PROPERTY
#undef GPS_PROPERTY
#endif
#define GPS_PROPERTY "/data/misc/GPS_CHIP.cfg"
#endif
ap_nvram_gps_config_struct stGPSReadback;
int gps_nvram_valid = 0;
char chip_id[PROPERTY_VALUE_MAX]={0};
#define COMBO_IOC_GPS_IC_HW_VERSION   7
#define COMBO_IOC_GPS_IC_FW_VERSION   8
#ifdef MPE_ENABLE
#define MPEMNL_UDP_CLI_PATH "/data/mpe_mnl/mnl2mpe"
#define MPEMNL_UDP_SRV_PATH "/data/mpe_mnl/mpe2mnl"
#endif
/*---------------------------------------------------------------------------*/
// for sync supl state
#include <cutils/properties.h>
#ifdef HAVE_LIBC_SYSTEM_PROPERTIES
#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#endif
/*---------------------------------------------------------------------------*/
#if defined(ANDROID)
#define LOG_TAG "MNLD"
#include <cutils/sockets.h>
#include <cutils/log.h>     /*logging in logcat*/
#endif
/******************************************************************************
* Macro & Definition
******************************************************************************/
#define MTK_MNLD2HAL  "/data/gps_mnl/mnld2hal"  // (AGPSD->)MNLD->HAL->JNI->FWK

#define GPS_USER_UNKNOWN 0x0
#define GPS_USER_APP 0x1
#define GPS_USER_AGPS 0x2
#define GPS_USER_META 0x4  // meta or factory mode
#define GPS_USER_RESTART 0x8
static int icon_show;
static int gps_user = GPS_USER_UNKNOWN;

#define M_START 0
#define M_STOP 1
#define M_RESTART 2
static int gps_cnt;
static int assist_req;
static unsigned char wake_lock_acquired = 0;
int epo_setconfig = 0;

int mnld2hal_fd = -1;
int agps2mnld_fd = -1;
int mtklogger2mnld_fd = -1;
int client_fd = -1;
#ifdef MNL_OFFLOAD_SUPPORT
pthread_t emi_download_thread;
int ofl_tst_fd = -1;
int flp_mnld_fd = -1;
int mnld2flp_fd = -1;
int ofl_lpbk_tst_start = 0;
int ofl_lpbk_tst_size = 300;
#define OFL_VER "OFL_VER_2015.01.15-01"
#define MNL_OFL_TST_FIFO "/data/gps_mnl/ofl_test"
#define MNL_FLP_TO_MNLD "/data/gps_mnl/flp2mnld"
#define MNL_MNLD_TO_FLP "/data/gps_mnl/mnld2flp"
#define OFFLOAD_PAYLOAD_LEN (600)
#define IOCTL_MNL_IMAGE_FILE_TO_MEM  1
#define IOCTL_MNL_NVRAM_FILE_TO_MEM  2
#define IOCTL_MNL_NVRAM_MEM_TO_FILE  3
typedef struct {
    UINT32 type;
    UINT32 length;
    UINT8 data[OFFLOAD_PAYLOAD_LEN];
} MTK_FLP_OFFLOAD_MSG_T;
#endif
/*---------------------------------------------------------------------------*/
int g_is_1Hz = 0;
#ifdef MPE_ENABLE
int mnl2mpe_fd = C_INVALID_FD;
int mpe2mnl_fd = C_INVALID_FD;
#endif
UINT32 assist_data_bit_map = FLAG_HOT_START;
static UINT32 delete_aiding_data;
static int in_stop_proc = 0;
static int in_start_proc = 0;
#define EPOLL_NUM 6
#define C_CMD_BUF_SIZE 32
/*---------------------------------------------------------------------------*/
#if defined(ANDROID)
#define MND_MSG(fmt, arg ...) ALOGD("%s: " fmt, __FUNCTION__ ,##arg)
#define MND_ERR(fmt, arg ...) ALOGE("%s: " fmt, __FUNCTION__ ,##arg)
#define MND_TRC(f)            ALOGD("%s\n", __FUNCTION__)
#define MND_VER(...)          do {} while (0)
#else
#define MND_MSG(...) printf(LOG_TAG":" __VA_ARGS__)
#define MND_ERR(...) printf(LOG_TAG":" __VA_ARGS__)
#endif

#ifdef MTK_GPS_CO_CLOCK_DATA_IN_MD
#define GPS_CALI_DATA_PATH "/data/nvram/md/NVRAM/CALIBRAT/ML4A_000"
#endif
/*---------------------------------------------------------------------------*/
enum {
    GPS_PWRCTL_UNSUPPORTED  = 0xFF,
    GPS_PWRCTL_OFF          = 0x00,
    GPS_PWRCTL_ON           = 0x01,
    GPS_PWRCTL_RST          = 0x02,
    GPS_PWRCTL_OFF_FORCE    = 0x03,
    GPS_PWRCTL_RST_FORCE    = 0x04,
    GPS_PWRCTL_MAX          = 0x05,
};
enum {
    GPS_PWR_UNSUPPORTED     = 0xFF,
    GPS_PWR_RESUME          = 0x00,
    GPS_PWR_SUSPEND         = 0x01,
    GPS_PWR_MAX             = 0x02,
};
enum {
    GPS_STATE_UNSUPPORTED   = 0xFF,
    GPS_STATE_PWROFF        = 0x00, /*cleanup/power off, default state*/
    GPS_STATE_INIT          = 0x01, /*init*/
    GPS_STATE_START         = 0x02, /*start navigating*/
    GPS_STATE_STOP          = 0x03, /*stop navigating*/
    GPS_STATE_DEC_FREQ      = 0x04,
    GPS_STATE_SLEEP         = 0x05,
    GPS_STATE_MAX           = 0x06,
};
enum {
    GPS_PWRSAVE_UNSUPPORTED = 0xFF,
    GPS_PWRSAVE_DEC_FREQ    = 0x00,
    GPS_PWRSAVE_SLEEP       = 0x01,
    GPS_PWRSAVE_OFF         = 0x02,
    GPS_PWRSAVE_MAX         = 0x03,
};
enum {
    GPS_MEASUREMENT_UNKNOWN     = 0xFF,
    GPS_MEASUREMENT_INIT        = 0x00,
    GPS_MEASUREMENT_CLOSE       = 0x01,
};
enum {
    GPS_NAVIGATION_UNKNOWN     = 0xFF,
    GPS_NAVIGATION_INIT        = 0x00,
    GPS_NAVIGATION_CLOSE       = 0x01,
};
enum {
    GPS_DEBUGLOG_DISABLE       = 0x00,
    GPS_DEBUGLOG_ENABLE        = 0x01,
};

static unsigned char gps_measurement_state = GPS_MEASUREMENT_UNKNOWN;
static unsigned char gps_navigation_state = GPS_NAVIGATION_UNKNOWN;
static unsigned char gps_debuglog_state = GPS_DEBUGLOG_DISABLE;
static char gps_debuglog_file_name[GPS_DEBUG_LOG_FILE_NAME_MAX_LEN] = "/mnt/sdcard/mtklog/gpsdbglog/gpsdebug.log";
static char storagePath[GPS_DEBUG_LOG_FILE_NAME_MAX_LEN] = {0};

#define PATH_INTERNAL       "internal_sd"
#define PATH_EXTERNAL       "external_sd"
#define PATH_DEFAULT        "/mnt/sdcard/"
#define PATH_EX             "/mnt/sdcard2/"
#define SDCARD_SWITCH_PROP  "persist.mtklog.log2sd.path"
#define PATH_SUFFIX         "mtklog/gpsdbglog/"
/*---------------------------------------------------------------------------*/
#define MNL_ATTR_PWRCTL  "/sys/class/gpsdrv/gps/pwrctl"
#define MNL_ATTR_SUSPEND "/sys/class/gpsdrv/gps/suspend"
#define MNL_ATTR_STATE   "/sys/class/gpsdrv/gps/state"
#define MNL_ATTR_PWRSAVE "/sys/class/gpsdrv/gps/pwrsave"
#define MNL_ATTR_STATUS  "/sys/class/gpsdrv/gps/status"
/*---------------------------------------------------------------------------*/
typedef enum {
    MNL_ALARM_UNSUPPORTED   = 0xFF,
    MNL_ALARM_INIT          = 0x00,
    MNL_ALARM_MONITOR       = 0x01,
    MNL_ALARM_WAKEUP        = 0x02,
    MNL_ALARM_TTFF          = 0x03,
    MNL_ALARM_DEC_FREQ      = 0x04,
    MNL_ALARM_SLEEP         = 0x05,
    MNL_ALARM_PWROFF        = 0x06,
    MNL_ALARM_MAX           = 0x07,
} MNL_ALARM_TYPE;
/*---------------------------------------------------------------------------*/
enum {
    MNL_ALARM_IDX_WATCH     = 0x00,
    MNL_ALARM_IDX_PWRSAVE   = 0x01,
    MNL_ALARM_IDX_MAX       = 0x02
};
/*---------------------------------------------------------------------------*/
enum { /*restart reason*/
    MNL_RESTART_NONE            = 0x00, /*recording the 1st of mnld*/
    MNL_RESTART_TIMEOUT_INIT    = 0x01, /*restart due to timeout*/
    MNL_RESTART_TIMEOUT_MONITOR = 0x02, /*restart due to timeout*/
    MNL_RESTART_TIMEOUT_WAKEUP  = 0x03, /*restart due to timeout*/
    MNL_RESTART_TIMEOUT_TTFF    = 0x04, /*restart due to TTFF timeout*/
    MNL_RESTART_FORCE           = 0x05, /*restart due to external command*/
};
/*---------------------------------------------------------------------------*/
typedef struct
{
    int period;
    int cmd;
    int idx;
} MNL_ALARM_T;
/*---------------------------------------------------------------------------*/
#define MNL_TIMEOUT_INIT    15
#define MNL_TIMEOUT_MONITOR 5
#define MNL_TIMEOUT_WAKEUP  3
#define MNL_TIMEOUT_TTFF    10
#define MNL_TIMEOUT_DEC_REQ 300
#define MNL_TIMEOUT_SLEEP   0
// #define MNL_TIMEOUT_PWROFF  30
#define MNL_TIMEOUT_PWROFF  0
/*---------------------------------------------------------------------------*/
static MNL_CONFIG_T mnld_cfg = {
    .timeout_init  = MNL_TIMEOUT_INIT,
    .timeout_monitor = MNL_TIMEOUT_MONITOR,
    .timeout_sleep = MNL_TIMEOUT_SLEEP,
    .timeout_pwroff = MNL_TIMEOUT_PWROFF,
    .timeout_wakeup = MNL_TIMEOUT_WAKEUP,
    .timeout_ttff = MNL_TIMEOUT_TTFF,
#ifdef MNL_OFFLOAD_SUPPORT
    .OFFLOAD_enabled = 1,
#endif
};
/*---------------------------------------------------------------------------*/
static MNL_ALARM_T mnl_alarm[MNL_ALARM_MAX] = {
    /*the order should be the same as MNL_ALARM_TYPE*/
    {MNL_TIMEOUT_INIT,     MNL_CMD_TIMEOUT_INIT,    MNL_ALARM_IDX_WATCH},
    {MNL_TIMEOUT_MONITOR,  MNL_CMD_TIMEOUT_MONITOR, MNL_ALARM_IDX_WATCH},
    {MNL_TIMEOUT_WAKEUP,   MNL_CMD_TIMEOUT_WAKEUP,  MNL_ALARM_IDX_WATCH},
    {MNL_TIMEOUT_TTFF,     MNL_CMD_TIMEOUT_TTFF,    MNL_ALARM_IDX_WATCH},
    {MNL_TIMEOUT_DEC_REQ,  MNL_CMD_DEC_FREQ,        MNL_ALARM_IDX_PWRSAVE},
    {MNL_TIMEOUT_SLEEP,    MNL_CMD_SLEEP,           MNL_ALARM_IDX_PWRSAVE},
    {MNL_TIMEOUT_PWROFF,   MNL_CMD_PWROFF,          MNL_ALARM_IDX_PWRSAVE},
};
/*---------------------------------------------------------------------------*/
typedef struct
{
    int type;
    struct sigevent evt;
    struct itimerspec expire;
    timer_t id[MNL_ALARM_IDX_MAX];
} MNL_TIMER_T;
/*---------------------------------------------------------------------------*/
static MNL_TIMER_T mnl_timer = {
    .type = MNL_ALARM_UNSUPPORTED,
    .expire = {{0, 0}, {0, 0}},
    .id = {C_INVALID_TIMER, C_INVALID_TIMER},
};
/*---------------------------------------------------------------------------*/
typedef struct {
    int cur_accept_socket;
    int epoll_fd;
    int sig_rcv_fd;  /*pipe for signal handler*/
    int sig_snd_fd;
    int mnl_rcv_fd;  /*pipe for mnl daemon*/
    int mnl_snd_fd;
    int mnl_rcv_agps_fd;  // fd for receiving agps open/close/reset command
    int mnl_rcv_mtklogger_fd;   // fd for receiving mtklogger enable/disable command
    int mpe_rcv_fd;
#ifdef MNL_OFFLOAD_SUPPORT
    int ofl_tst_fd;
    int flp_mnld_fd;
#endif
    unsigned char pwrctl;
    unsigned char suspend;
    unsigned char state;
    unsigned char pwrsave;
}MNLD_DATA_T;
/*---------------------------------------------------------------------------*/
typedef struct {
    int         init;
    pid_t       pid;
    int         count;
    int         terminated;
}MNLD_MONITOR_T;
/*---------------------------------------------------------------------------*/
typedef enum
{
    MNL_THREAD_UNKNOWN      = -1,
    MNL_THREAD_AGPSDISPATCH = 0,
    MNL_THREAD_NUM,
    MNL_THREAD_LAST         = 0x7FFFFFFF
} MNL_THREAD_ID_T;
/*---------------------------------------------------------------------------*/
typedef struct _MNL_THREAD_T
{
    int                 snd_fd;
    MNL_THREAD_ID_T     thread_id;
    pthread_t           thread_handle;
    int (*thread_exit)(struct _MNL_THREAD_T *arg);
    int (*thread_active)(struct _MNL_THREAD_T *arg);
} MNL_THREAD_T;
/*---------------------------------------------------------------------------*/
extern int mtk_gps_sys_init();
extern int mtk_gps_sys_uninit();
int thread_active_notify(MNL_THREAD_T *arg);
/*---------------------------------------------------------------------------*/
int exit_thread_normal(MNL_THREAD_T *arg);
static MNL_THREAD_T mnl_thread[MNL_THREAD_NUM] = {
    {C_INVALID_FD, MNL_THREAD_AGPSDISPATCH,  C_INVALID_TID, exit_thread_normal, thread_active_notify},
};
/*---------------------------------------------------------------------------*/
/*Thread of Agent Dispatch task*/
static volatile int g_ThreadExitAgpsDispatch = 0;
static int condition = 0;
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond=PTHREAD_COND_INITIALIZER;
/*---------------------------------------------------------------------------*/
#ifdef MPE_ENABLE

#define CMD_SEND_FROM_MNLD  0x40  // with payload, mtklogger recording setting & path
#define MPE_LOG_FILE        "/data/misc/gps/"
#define MPEMNLLOG "/data/misc/gps/adr.txt"
typedef struct {
  UINT32    type;           /* message ID */
  UINT32    length;         /* length of 'data' */
} TOMPE_MSG;

int (*gMpeCallBackFunc)()= NULL;
static volatile int g_ThreadExitmnlmpe = 0;
static volatile int pipe_close = 0;
int adr_flag, adr_valid_flag, isFirstRecv = 0;
double adr_time;

int linux_mpe_uninit(void);
int isTriggerMpe = 0;
void *thread_mnlmpe(void * arg);
void check_pipe_handler(int signum);
static MNL_THREAD_T mnlmpe_thread[0] = {
    {C_INVALID_FD, 0,  C_INVALID_TID, NULL, NULL},
};
#endif
static int exit_meta_factory = 0;
/*---------------------------------------------------------------------------*/
MNL_EPO_TIME_T mnl_epo_time = {
    .uSecond_start = 0,
    .uSecond_expire = 0,
};
/*---------------------------------------------------------------------------*/
static MNLD_DATA_T mnld_data = {
    .cur_accept_socket = C_INVALID_SOCKET,
    .epoll_fd   = C_INVALID_FD,
    .sig_rcv_fd = C_INVALID_FD,
    .sig_snd_fd = C_INVALID_FD,
    .mnl_rcv_fd = C_INVALID_FD,
    .mnl_snd_fd = C_INVALID_FD,
    .mnl_rcv_agps_fd = C_INVALID_FD,
    .mpe_rcv_fd = C_INVALID_FD,
    .mnl_rcv_mtklogger_fd = C_INVALID_FD,
    .pwrctl = GPS_PWRCTL_UNSUPPORTED,
    .suspend = GPS_PWR_UNSUPPORTED,
    .state = GPS_STATE_UNSUPPORTED,
    .pwrsave = GPS_PWRSAVE_UNSUPPORTED
};
/*---------------------------------------------------------------------------*/
// for CMCC GPS.LOG
#define  MNL_CONFIG_STATUS      "persist.radio.mnl.prop"
static int ttff = 0;
time_t start_time;
static MNLD_MONITOR_T mnld_monitor = {
    .init   = 0,
    .pid    = C_INVALID_PID,
    .count  = 0,
    .terminated = 1,
};
static int get_prop(int index)
{
    // Read property
    char result[PROPERTY_VALUE_MAX] = {0};
    int ret = 0;
    if (property_get(MNL_CONFIG_STATUS, result, NULL)) {
        ret = result[index] - '0';
        MND_MSG("gps.log: %s, %d\n", &result[index], ret);
    } else {
             if (index == 7)
             {
                 ret = 1;
             }
             else
             {
                 ret = 0;
             }
             MND_MSG("Config is not set yet, use default value");
         }
         return ret;
}
/*---------------------------------------------------------------------------*/
/*Agps dispatcher state mode*/
#define AGENT_LOG_ENABLE      1
typedef enum {
    ST_SI,
    ST_NI,
    ST_IDLE,
    ST_UNKNOWN,
}MTK_AGPS_DISPATCH_STATE;

MTK_AGPS_DISPATCH_STATE state_mode = ST_IDLE;
MTK_AGPS_DISPATCH_STATE last_state_mode = ST_UNKNOWN;
int AGENT_SET_ALARM = 0;
/*---------------------------------------------------------------------------*/
#define GET_VER
#ifdef TCXO
#undef TCXO
#endif
#define TCXO 0
#ifdef CO_CLOCK
#undef CO_CLOCK
#endif
#define CO_CLOCK 1
#ifdef CO_DCXO
#undef CO_DCXO
#endif
#define CO_DCXO 2
#define GPS_CLOCK_TYPE_P    "gps.clock.type"
/*---------------------------------------------------------------------------*/
typedef struct sync_lock
{
    pthread_mutex_t mutx;
    pthread_cond_t con;
    int condtion;
}SYNC_LOCK_T;

static SYNC_LOCK_T lock_for_sync[] = {{PTHREAD_MUTEX_INITIALIZER,PTHREAD_COND_INITIALIZER, 0},
{PTHREAD_MUTEX_INITIALIZER,PTHREAD_COND_INITIALIZER, 0},
{PTHREAD_MUTEX_INITIALIZER,PTHREAD_COND_INITIALIZER, 0},
{PTHREAD_MUTEX_INITIALIZER,PTHREAD_COND_INITIALIZER, 0},
{PTHREAD_MUTEX_INITIALIZER,PTHREAD_COND_INITIALIZER, 0},
{PTHREAD_MUTEX_INITIALIZER,PTHREAD_COND_INITIALIZER, 0}};

static void init_condition(SYNC_LOCK_T *lock)
{
    MND_MSG("before init condition %d\n", lock->condtion);

    pthread_mutex_lock(&(lock->mutx));
    lock->condtion = 0;
    pthread_mutex_unlock(&(lock->mutx));

    MND_MSG("init condition done = %d\n", lock->condtion);
    return;
}
static void get_condition(SYNC_LOCK_T *lock)
{
    int ret = 0;
    MND_MSG("condition before cond_wait is %d\n", lock->condtion);

    pthread_mutex_lock(&(lock->mutx));
    while (!lock->condtion)
        ret = pthread_cond_wait(&(lock->con), &(lock->mutx));
    lock->condtion = 0;
    pthread_mutex_unlock(&(lock->mutx));


    MND_MSG("ret cond wait = %d\n", ret);

    return;
}

static void release_condition(SYNC_LOCK_T *lock)
{
    int ret = 0;
    pthread_mutex_lock(&(lock->mutx));
    lock->condtion = 1;
    pthread_mutex_unlock(&(lock->mutx));

    ret = pthread_cond_signal(&(lock->con));
    MND_MSG("ret cond_signal = %d\n", ret);

    return;
}
/*---------------------------------------------------------------------------*/
MNL_CONFIG_T mnl_config =
{
    .init_speed = 38400,
    .link_speed = 921600,
    .debug_nmea = 1,
    .debug_mnl  = MNL_NEMA_DEBUG_SENTENCE, /*MNL_NMEA_DEBUG_NORMAL,*/
    .pmtk_conn  = PMTK_CONNECTION_SOCKET,
    .socket_port = 7000,
    .dev_dbg = DBG_DEV,
    .dev_dsp = DSP_DEV,
    .dev_gps = GPS_DEV,
    .bee_path = BEE_PATH,
    .epo_file = EPO_FILE,
    .epo_update_file = EPO_UPDATE_HAL,
    .qepo_file = QEPO_FILE,
    .qepo_update_file = QEPO_UPDATE_HAL,
    .delay_reset_dsp = 500,
    .nmea2file = 0,
    .dbg2file = 0,
    .nmea2socket = 1,
    .dbg2socket = 0,
    .timeout_init = 0,
    .timeout_monitor = 0,
    .timeout_wakeup = 0,
    .timeout_sleep = 0,
    .timeout_pwroff = 0,
    .timeout_ttff = 0,
    .EPO_enabled = 1,
    .BEE_enabled = 1,
    .SUPL_enabled = 1,
    .SUPLSI_enabled = 1,
    .EPO_priority = 64,
    .BEE_priority = 32,
    .SUPL_priority = 96,
    .fgGpsAosp_Ver = MTK_GPS_TRUE,
    .AVAILIABLE_AGE = 2,
    .RTC_DRIFT = 30,
    .TIME_INTERVAL = 10,
    .u1AgpsMachine = 0,  // default use spirent "0"
    .ACCURACY_SNR = 1,
    .GNSSOPMode = 2,     // 0: G+G; 1: G+B
    .dbglog_file_max_size = 20*1024*1024,
    .dbglog_folder_max_size = 240*1024*1024
};
/*---------------------------------------------------------------------------*/
#define MNL_SO
void mtk_null(UINT16 a)
{
    return;
}
int SUPL_encrypt_wrapper(unsigned char *plain,
                         unsigned char *cipher, unsigned int length)
{
    return SUPL_encrypt(plain, cipher, length);
}

int SUPL_decrypt_wrapper(unsigned char *plain,
                         unsigned char *cipher, unsigned int length)
{
    return SUPL_decrypt(plain, cipher, length);
}

#ifdef MNL_SO
MTK_GPS_SYS_FUNCTION_PTR_T porting_layer_callback =
{
    .sys_gps_mnl_callback = mtk_gps_sys_gps_mnl_callback,
    .sys_nmea_output_to_app = mtk_gps_sys_nmea_output_to_app,
    .sys_frame_sync_enable_sleep_mode = mtk_gps_sys_frame_sync_enable_sleep_mode,
    .sys_frame_sync_meas_req_by_network = mtk_gps_sys_frame_sync_meas_req_by_network,
    .sys_frame_sync_meas_req = mtk_gps_sys_frame_sync_meas_req,
    .sys_agps_disaptcher_callback = mtk_gps_sys_agps_disaptcher_callback,
    .sys_pmtk_cmd_cb = mtk_null,
    .encrypt = SUPL_encrypt_wrapper,
    .decrypt = SUPL_decrypt_wrapper,
#ifdef MNL_OFFLOAD_SUPPORT
    .ofl_sys_rst_stpgps_req = mtk_gps_ofl_sys_rst_stpgps_req,
    .ofl_sys_submit_flp_data = mtk_gps_ofl_sys_submit_flp_data,
#endif
};
#endif
static int
str2int(const char*  p, const char*  end)
{
    int   result = 0;
    int   len    = end - p;
    int   sign = 1;

    if (*p == '-') {
        sign = -1;
        p++;
        len = end - p;
    }

    for (; len > 0; len--, p++) {
        int  c;

        if (p >= end)
            goto Fail;

        c = *p - '0';
        if ((unsigned)c >= 10)
            goto Fail;

        result = result*10 + c;
    }
    return  sign*result;

Fail:
    return -1;
}

#if RAW_DATA_SUPPORT
int get_val(char *pStr, char** ppKey, char** ppVal)
{
    int len = (int)strlen(pStr);
    char *end = pStr + len;
    char *key = NULL, *val = NULL;
    int stage = 0;

    MND_MSG("pStr = %s,len=%d!!\n", pStr, len);

    if (!len) {
        return -1;    // no data
    } else if (pStr[0] == '#') {   /*ignore comment*/
        *ppKey = *ppVal = NULL;
        return 0;
    } else if (pStr[len-1] != '\n') {
        if (len >= GPS_CONF_FILE_SIZE-1) {
            MND_MSG("buffer is not enough!!\n");
            return -1;
        } else {
            pStr[len] = '\n';
        }
    }
    key = pStr;

    MND_MSG("key = %s!!\n", key);
    while ((*pStr != '=') && (pStr < end)) pStr++;
    if (pStr >= end) {
        MND_MSG("'=' is not found!!\n");
        return -1;    // format error
    }

    *pStr++ = '\0';
    while (IS_SPACE(*pStr) && (pStr < end)) pStr++;    // skip space chars
    val = pStr;
    while (!IS_SPACE(*pStr) && (pStr < end)) pStr++;
    *pStr = '\0';
    *ppKey = key;
    *ppVal = val;

    MND_MSG("val = %s!!\n", val);
    return 0;

}

static int
gps_raw_data_enable(void)
{
    char result[GPS_CONF_FILE_SIZE] = {0};

    FILE *fp = fopen(RAW_DATA_CONTROL_FILE_PATH, "r");
    char *key, *val;
    if (!fp) {
        MND_MSG("%s: open %s fail!\n", __FUNCTION__, RAW_DATA_CONTROL_FILE_PATH);
        return 1;
    }

    while (fgets(result, sizeof(result), fp)) {
        if (get_val(result, &key, &val)) {
            MND_MSG("%s: Get data fails!!\n", __FUNCTION__);
            fclose(fp);
            return 1;
        }
        if (!key || !val) {
            continue;
        }
        if (!strcmp(key, "RAW_DEBUG_MODE")) {
            int len = strlen(val);
            gps_raw_debug_mode = str2int(val, val+len);  // *val-'0';
            if ((gps_raw_debug_mode != 1) && (gps_raw_debug_mode != 0)) {
                gps_raw_debug_mode = 0;
            }
            MND_MSG("gps_raw_debug_mode = %d\n", gps_raw_debug_mode);
        }
    }
    fclose(fp);
    return gps_raw_debug_mode;
}
#endif
// TODO
void agps_reboot() {
    MND_MSG("agps_reboot");
    if ((gps_user&GPS_USER_AGPS) != 0)
        gps_user -= GPS_USER_AGPS;
    return;
}
static void agps_hold_wake_lock(int hold)
{
    int ret = 0;
    if (hold == 1) {
        //  acquire wake lock
        if (!wake_lock_acquired) {
            MND_MSG("acquire mnld wake_lock acquired=%d\n", wake_lock_acquired);
            ret = acquire_wake_lock(PARTIAL_WAKE_LOCK, "mnlddrv");
            MND_MSG("acquire mnld wake_lock ret=%d\n", ret);
            wake_lock_acquired = 1;
        }
    }
    else if (hold == 0) {
        //  release wake lock
        if (wake_lock_acquired) {
            MND_MSG("release mnld wake_lock acquired=%d\n", wake_lock_acquired);
            ret = release_wake_lock("mnlddrv");
            MND_MSG("release mnld wake_lock ret=%d\n", ret);
            wake_lock_acquired = 0;
        }
    }
}
void agps_open_gps_req(int show_gps_icon) {
    MND_MSG("agps_open_gps_req");
    agps_hold_wake_lock(1);
    icon_show = show_gps_icon;
    gps_user |= GPS_USER_AGPS;
#ifdef MNL_OFFLOAD_SUPPORT
    mtk_gps_ofl_set_user(gps_user);
#endif

    MND_MSG("init start condition");
    init_condition(&lock_for_sync[M_START]);
    MND_MSG("user is %x\n", gps_user);
    if (icon_show == 1) {
            char buff[1024] = {0};
            int offset = 0;
            buff_put_int(MNL_CMD_GPS_ICON, buff, &offset);
            buff_put_int(1, buff, &offset);
        if (-1 == mnld_sendto_hal(mnld2hal_fd, MTK_MNLD2HAL, buff, sizeof(buff)))
            MND_ERR("Send to HAL failed, %s\n", strerror(errno));
        else
            MND_MSG("Send to HAL successfully\n");
    }
    if (0 != start_mnl_process())
      MND_ERR("AGPS open GPS fail");
      // To debug, if GPS current is opened, condition can be wait?
    MND_MSG("Wait condition from first callback");
    if (gps_cnt == 1)
        get_condition(&lock_for_sync[M_START]);
    mnl2agps_open_gps_done();
}

void agps_close_gps_req() {
    MND_MSG("agps_close_gps_req");
    if (icon_show == 1) {
        char buff[1024] = {0};
        int offset = 0;
        buff_put_int(MNL_CMD_GPS_ICON, buff, &offset);
        buff_put_int(0, buff, &offset);  // close
        if (-1 == mnld_sendto_hal(mnld2hal_fd, MTK_MNLD2HAL, buff, sizeof(buff)))
            MND_ERR("Send to HAL failed, %s\n", strerror(errno));
        else
            MND_MSG("Send to HAL successfully\n");
    }
    if (0 != stop_mnl_process())
        MND_ERR("AGPS close GPS fail");

    gps_user -= GPS_USER_AGPS;
    mnl2agps_close_gps_done();
    agps_hold_wake_lock(0);
    MND_MSG("user is %x\n", gps_user);
}

void agps_reset_gps_req(int flags) {
    MND_MSG("agps_reset_gps_req flags = 0x%X", flags);

    if (flags == FLAG_AGPS_HOT_START) {
        MND_MSG("AGPS request FLAG_AGPS_HOT_START");
        MTK_GPS_PARAM_RESTART restart = {MTK_GPS_START_HOT};
        if (mnld_monitor.terminated == 1 || in_stop_proc == 1 || in_start_proc == 1) {
            MND_MSG("GPS stopped or in stop/start process");
            assist_data_bit_map = FLAG_HOT_START;
        } else {
            mtk_gps_set_param(MTK_PARAM_CMD_RESTART, &restart);
        }
    }
    else if (flags == FLAG_AGPS_WARM_START) {
        MND_MSG("AGPS request FLAG_AGPS_WARM_START");
        MTK_GPS_PARAM_RESTART restart = {MTK_GPS_START_WARM};
        if (mnld_monitor.terminated == 1 || in_stop_proc == 1 || in_start_proc == 1) {
            MND_MSG("GPS stopped or in stop/start process");
            assist_data_bit_map = FLAG_WARM_START;
        } else {
            mtk_gps_set_param(MTK_PARAM_CMD_RESTART, &restart);
        }
    }
    else if (flags == FLAG_AGPS_COLD_START) {
        MND_MSG("AGPS request FLAG_AGPS_COLD_START");
        MTK_GPS_PARAM_RESTART restart = {MTK_GPS_START_COLD};
        if (mnld_monitor.terminated == 1 || in_stop_proc == 1 || in_start_proc == 1) {
            MND_MSG("GPS stopped or in stop/start process");
            assist_data_bit_map = FLAG_COLD_START;
        } else {
            mtk_gps_set_param(MTK_PARAM_CMD_RESTART, &restart);
        }
    }
    else if (flags == FLAG_AGPS_FULL_START) {
        MND_MSG("AGPS request FLAG_AGPS_FULL_START");
        MTK_GPS_PARAM_RESTART restart = {MTK_GPS_START_FULL};
        if (mnld_monitor.terminated == 1 || in_stop_proc == 1 || in_start_proc == 1) {
            MND_MSG("GPS stopped or in stop/start process");
            assist_data_bit_map = FLAG_FULL_START;
        } else {
            mtk_gps_set_param(MTK_PARAM_CMD_RESTART, &restart);
        }
    }
    else if (flags == FLAG_AGPS_AGPS_START) {
        MND_MSG("AGPS request FLAG_AGPS_AGPS_START");
        MTK_GPS_PARAM_RESTART restart = {MTK_GPS_START_AGPS};
        if (mnld_monitor.terminated == 1 || in_stop_proc == 1 || in_start_proc == 1) {
            MND_MSG("GPS stopped or in stop/start process");
            assist_data_bit_map = FLAG_AGPS_START;
        } else {
            mtk_gps_set_param(MTK_PARAM_CMD_RESTART, &restart);
        }
    }
    mnl2agps_reset_gps_done();
}
void alarm_handler() {
    if (AGENT_SET_ALARM == 1) {
        if ((gps_user& GPS_USER_AGPS) != 0) {
            // SI alarm handler: ignore, as current user is AGPS, it may in NI session
            // NI alarm handler: ignore, as AGPSD send close_gps_req(timer be canceled)
            return;
        }
        if (mnld_monitor.terminated == 1 || in_stop_proc == 1)
        {
                MND_MSG("GPS driver is stopped");
                AGENT_SET_ALARM = 0;
                last_state_mode = state_mode;
                state_mode = ST_IDLE;
                return;
        }
        // SI only session and GPS driver is running
        MND_MSG("Send MTK_AGPS_SUPL_END to MNL");
        int ret = mtk_agps_set_param(MTK_MSG_AGPS_MSG_SUPL_TERMINATE, NULL, MTK_MOD_DISPATCHER, MTK_MOD_AGENT);
        last_state_mode = state_mode;
        state_mode = ST_IDLE;
        MND_MSG("Receive MTK_AGPS_SUPL_END , last_state_mode = %d, state_mode = %d", last_state_mode, state_mode);
    }
    AGENT_SET_ALARM = 0;
}
void agps_session_done() {
    MND_MSG("agps_session_done");
    if (mnld_monitor.terminated == 1 || in_stop_proc == 1) {
        MND_MSG("GPS driver is stopped");
        AGENT_SET_ALARM = 0;
        last_state_mode = state_mode;
        state_mode = ST_IDLE;
        return;
    }
    if (0 == AGENT_SET_ALARM) {
        MNL_MSG("Set up signal & alarm!");
        signal(SIGALRM, alarm_handler);
        alarm(30);
        AGENT_SET_ALARM = 1;
    } else {
        MNL_MSG("AGENT_SET_ALARM == 1");
    }
}

void ni_notify(int session_id, mnl_agps_notify_type type, const char* requestor_id, const char* client_name) {
    MND_MSG("ni_notify to HAL");
    char buff[1024] = {0};
    int offset = 0;
    buff_put_int(MNL_AGPS_TYPE_NI_NOTIFY, buff, &offset);
    buff_put_int(session_id, buff, &offset);
    buff_put_int(type, buff, &offset);
    buff_put_string(requestor_id, buff, &offset);
    buff_put_string(client_name, buff, &offset);
    mnld_sendto_hal(mnld2hal_fd, MTK_MNLD2HAL, buff, sizeof(buff));
}

void data_conn_req(int ipaddr, int is_emergency) {
    MND_MSG("data_conn_req");
    char buff[1024] = {0};
    int offset = 0;
    buff_put_int(MNL_AGPS_TYPE_DATA_CONN_REQ, buff, &offset);
    buff_put_int(ipaddr, buff, &offset);
    buff_put_int(is_emergency, buff, &offset);
    mnld_sendto_hal(mnld2hal_fd, MTK_MNLD2HAL, buff, sizeof(buff));
}

void data_conn_req2(struct sockaddr_storage * addr, int is_emergency) {
    MND_MSG("data_conn_req2");
    char buff[1024] = {0};
    int offset = 0;
    buff_put_int(MNL_AGPS_TYPE_DATA_CONN_REQ2, buff, &offset);
    buff_put_struct(addr, sizeof(struct sockaddr_storage), buff, &offset);
    buff_put_int(is_emergency, buff, &offset);
    mnld_sendto_hal(mnld2hal_fd, MTK_MNLD2HAL, buff, sizeof(buff));
}

void data_conn_release() {
    MND_MSG("data_conn_release");
    char buff[1024] = {0};
    int offset = 0;
    buff_put_int(MNL_AGPS_TYPE_DATA_CONN_RELEASE, buff, &offset);
    mnld_sendto_hal(mnld2hal_fd, MTK_MNLD2HAL, buff, sizeof(buff));
}

void set_id_req(int flags) {
    MND_MSG("set_id_req");
    char buff[1024] = {0};
    int offset = 0;
    buff_put_int(MNL_AGPS_TYPE_SET_ID_REQ, buff, &offset);
    buff_put_int(flags, buff, &offset);
    mnld_sendto_hal(mnld2hal_fd, MTK_MNLD2HAL, buff, sizeof(buff));
}
void ref_loc_req(int flags) {
    MND_MSG("ref_loc_req");
    char buff[1024] = {0};
    int offset = 0;
    buff_put_int(MNL_AGPS_TYPE_REF_LOC_REQ, buff, &offset);
    buff_put_int(flags, buff, &offset);
    mnld_sendto_hal(mnld2hal_fd, MTK_MNLD2HAL, buff, sizeof(buff));
}

void agps_location(mnl_agps_agps_location* location) {
    MND_MSG("agps_location");
    char buff[1024] = {0};
    int offset = 0;
    buff_put_int(MNL_AGPS_TYPE_AGPS_LOC, buff, &offset);
    buff_put_struct(location, sizeof(mnl_agps_agps_location), buff, &offset);
    mnld_sendto_hal(mnld2hal_fd, MTK_MNLD2HAL, buff, sizeof(buff));
}

void rcv_pmtk(const char* pmtk) {
    MND_MSG("rcv_pmtk: %s", pmtk);
    if (mnld_monitor.terminated == 1 || in_stop_proc == 1) {
        MND_MSG("rcv_pmtk: MNL stopped, return");
        return;
    }
    int ret = mtk_agps_set_param(MTK_MSG_AGPS_MSG_SUPL_PMTK, pmtk, MTK_MOD_DISPATCHER, MTK_MOD_AGENT);
    MND_MSG("mtk_agps_set_param: %d", ret);
}
void gpevt(gpevt_type type) {
    MND_MSG("gpevt");
    int gpevt = type;
    // TODO
    // mtk_agps_set_param(MTK_AGPS_STATUS_INFO, &gpevt, MTK_MOD_DISPATCHER, MTK_MOD_AGENT);
}

void ni_notify2(int session_id, mnl_agps_notify_type type, const char* requestor_id, const char* client_name,
        mnl_agps_ni_encoding_type requestor_id_encoding, mnl_agps_ni_encoding_type client_name_encoding) {
    MND_MSG("ni_notify2  session_id=%d type=%d requestor_id=[%s] type=%d client_name=[%s] type=%d",
        session_id, type, requestor_id, requestor_id_encoding, client_name, client_name_encoding);

    char buff[1024] = {0};
    int offset = 0;
    buff_put_int(MNL_AGPS_TYPE_NI_NOTIFY_2, buff, &offset);
    buff_put_int(session_id, buff, &offset);
    buff_put_int(type, buff, &offset);
    buff_put_string(requestor_id, buff, &offset);
    buff_put_string(client_name, buff, &offset);
    buff_put_int(requestor_id_encoding, buff, &offset);
    buff_put_int(client_name_encoding, buff, &offset);
    mnld_sendto_hal(mnld2hal_fd, MTK_MNLD2HAL, buff, sizeof(buff));
}

mnl2agpsInterface mnl_interface = {
    agps_reboot,
    agps_open_gps_req,
    agps_close_gps_req,
    agps_reset_gps_req,
    agps_session_done,
    ni_notify,
    data_conn_req,
    data_conn_release,
    set_id_req,
    ref_loc_req,
    rcv_pmtk,
    gpevt,
    agps_location,
    ni_notify2,
    data_conn_req2,
};
/*---------------------------------------------------------------------------*/
static int send_cmd_ex(int fd, char* cmd, int len, char* caller)
{
    if (fd == C_INVALID_FD) {
        return 0;
    } else {
        int  ret;
        MND_MSG("%s (%d, 0x%x)\n", caller, fd, (int)(*cmd));
        do {
            ret = write(fd, cmd, len);
        }while (ret < 0 && errno == EINTR);

        if (ret == len)
            return 0;
        else {
            MND_ERR("%s fails: %d (%s)\n", caller, errno, strerror(errno));
            return -1;
        }
    }

}
/*---------------------------------------------------------------------------*/
#define mnl_send_cmd(cmd, len) send_cmd_ex(mnld_data.mnl_snd_fd, cmd, len, "mnl_send_cmd")
#define slf_send_cmd(cmd, len) send_cmd_ex(mnld_data.sig_snd_fd, cmd, len, "slf_send_cmd")
/*---------------------------------------------------------------------------*/
MTK_GPS_BOOL enable_dbg_log = MTK_GPS_TRUE;
/*---------------------------------------------------------------------------*/
void linux_gps_load_property()
{
    // enable_dbg_log = 0;
    // nmea_debug_level = 0;

    if (!mnl_utl_load_property(&mnl_config)) {  // use property currently, so it will return -1
        enable_dbg_log = mnl_config.debug_nmea;
        nmea_debug_level = mnl_config.debug_mnl;
    }
}
/*****************************************************************************/
int thread_active_notify(MNL_THREAD_T *arg)
{
    char buf[] = {MNL_CMD_ACTIVE};
    if (!arg) {
        MND_MSG("fatal error: null pointer!!\n");
        return -1;
    }
    return slf_send_cmd(buf, sizeof(buf));
}
/*****************************************************************************/
int send_active_notify() {
    if (!(mnl_config.debug_mnl & MNL_NMEA_DISABLE_NOTIFY)) {
        char buf[] = {MNL_CMD_ACTIVE};
        return slf_send_cmd(buf, sizeof(buf));
    }
    return -1;
}
/*****************************************************************************/
/*Not be called*/
int exit_thread_normal(MNL_THREAD_T *arg)
{   /* exit thread by pthread_kill -> pthread_join*/
    int err;
    if (!arg) {
        return MTK_GPS_ERROR;
    }

    if (arg->thread_id == MNL_THREAD_AGPSDISPATCH) {
        int sock2diapatch = -1;
        struct sockaddr_un local;
        mtk_agps_msg *pDummy_agps_msg = (mtk_agps_msg *)malloc(sizeof(mtk_agps_msg));
        memset(pDummy_agps_msg, 0, sizeof(mtk_agps_msg));
        if (pDummy_agps_msg == NULL) {
            MND_MSG("pDummy_agps_msg is null\n");
            return -1;
        } else {
            MND_MSG("agps dispatch thread return trigger\n");
        }

        g_ThreadExitAgpsDispatch = 1;
        if ((sock2diapatch = socket(AF_LOCAL, SOCK_DGRAM, 0)) == -1) {
            MND_ERR("exit_thread_normal: open sock2supl fails\r\n");
            free(pDummy_agps_msg);
            pDummy_agps_msg = NULL;
            goto EXIT;
        }

        memset(&local, 0, sizeof(local));
        local.sun_family = AF_LOCAL;
        strcpy(local.sun_path, MTK_PROFILE2MNL);

        if (sendto(sock2diapatch, pDummy_agps_msg, sizeof(mtk_agps_msg), 0,
        (struct sockaddr*)&local, sizeof(local)) < 0) {
            MND_ERR("send msg to dispatch fail:%s\r\n", strerror(errno));
        }
        close(sock2diapatch);
        if (pDummy_agps_msg) {
            free(pDummy_agps_msg);
            pDummy_agps_msg = NULL;
        }
    }

EXIT:
    if ((err = pthread_kill(arg->thread_handle, SIGUSR1))) {
        MND_ERR("pthread_kill failed idx:%d, err:%d\n", arg->thread_id, err);
        return err;
    }
    if ((err = pthread_join(arg->thread_handle, NULL))) {
        MND_ERR("pthread_join failed idx:%d, err:%d\n", arg->thread_id, err);
        return err;
    }
    return 0;
}

/*****************************************************************************/
/*the TTFF handler is called from PMTK handler, no use this API presently*/
void mtk_gps_sys_ttff_handler(int type)
{
    char *msg = NULL;
    char buf[] = {MNL_CMD_RCV_TTFF};
    int err;

    if (type == MTK_GPS_START_HOT)
        msg = "HOT ";
    else if (type == MTK_GPS_START_WARM)
        msg = "WARM";
    else if (type == MTK_GPS_START_COLD)
        msg = "COLD";
    else if (type == MTK_GPS_START_FULL)
        msg = "FULL";
    else
        MND_ERR("invalid TTFF type: %d\n", type);

    MND_MSG("receive %s TTFF\n", msg);
    if ((err = slf_send_cmd(buf, sizeof(buf))))
        MND_MSG("send command 0x%X fails\n", (unsigned int)buf[0]);

}
/*****************************************************************************/
INT32
mtk_gps_sys_agps_disaptcher_callback (UINT16 type, UINT16 length, char *data)
{
    INT32 ret = MTK_GPS_SUCCESS;
    MNL_MSG("agps state mode = %d", state_mode);

    if (type == MTK_AGPS_CB_SUPL_PMTK || type == MTK_AGPS_CB_ASSIST_REQ || type == MTK_AGPS_CB_START_REQ)
    {
        if (mnl_config.SUPL_enabled)
        {
            if (type == MTK_AGPS_CB_SUPL_PMTK)
            {
                if (length != 0)
                  mnl2agps_pmtk(data);
                return 0;
            }
            else if (type == MTK_AGPS_CB_ASSIST_REQ)
            {
                if (state_mode == ST_IDLE || state_mode == ST_SI)
                {
                    MNL_MSG("GPS re-aiding\n");
                    mnl2agps_reaiding_req();
                    return 0;
                }
                else
                {
                    MNL_ERR("Dispatcher in %d mode, ignore current request\n", state_mode);
                    return MTK_GPS_ERROR;
                }
            }
            else if (type == MTK_AGPS_CB_START_REQ)
            {
                in_start_proc = 0;
                MNL_MSG("MNL ready and assist req:%d\n", *data);
                     if (*data == 1) {
                         MNL_MSG("Agent assist request");
                         assist_req = 1;
                     } else if (*data == 0) {
                         MNL_MSG("Agent no assist request");
                         assist_req = 0;
                     } else {
                         MNL_MSG("unknown data");
                    assist_req = 0;
                }
                MND_MSG("user is %x\n", gps_user);
                if ((gps_user &GPS_USER_APP) != 0) {
                    MND_MSG("APP open GPS driver");
                    int ret = mnl2agps_gps_open(assist_req);
                }
                if ((gps_user & GPS_USER_AGPS) != 0)
                    release_condition(&lock_for_sync[M_START]);
                if ((gps_user & GPS_USER_RESTART) != 0) {
                    release_condition(&lock_for_sync[M_RESTART]);
                    MND_MSG("release condition for restart");
                }
                return ret;
            }

        }
        else {
            MNL_MSG("mtk_sys_agps_disaptcher_callback: SUPL disable\r\n");
            ret = MTK_GPS_ERROR;
        }
    }
    else if (type == MTK_AGPS_CB_BITMAP_UPDATE)
    {
        MNL_MSG("MNL NTP/NLP request:%d\n", *data);
        if ((*data & 0x01) == 0x01) {
            char buff[1024] = {0};
            int offset = 0;

            buff_put_int(MNL_CMD_GPS_INJECT_TIME_REQ, buff, &offset);
            if (-1 == mnld_sendto_hal(mnld2hal_fd, MTK_MNLD2HAL, buff, sizeof(buff))) {
                MND_ERR("Send to HAL failed, %s\n", strerror(errno));
            } else {
                MND_MSG("MNL inject time req Send to HAL successfully\n");
            }
        }
        if ((*data & 0x02) == 0x02) {
#if 0
            char buff[1024] = {0};
            int offset = 0;

            buff_put_int(MNL_CMD_GPS_INJECT_LOCATION_REQ, buff, &offset);
            if (-1 == mnld_sendto_hal(mnld2hal_fd, MTK_MNLD2HAL, buff, sizeof(buff))) {
                MND_ERR("Send to HAL failed, %s\n", strerror(errno));
            } else {
                MND_MSG("MNL inject location req Send to HAL successfully\n");
            }
#else
            MND_MSG("Call nlp_server request\n");
            mnl2nlp_request_nlp();
#endif
        }
    } else if (type == MTK_AGPS_CB_QEPO_DOWNLOAD_REQ) {
        char buff[1024] = {0};
        int offset = 0;
        UINT16 wn;
        UINT32 tow;
        UINT32 sys_time;
        INT32 ret = 0;

        ret = mtk_agps_agent_qepo_get_rqst_time(&wn, &tow, &sys_time);
        MND_MSG("wn, tow, sys_time = %d, %d, %d\n", wn, tow, sys_time);
        buff_put_int(MNL_CMD_GPS_QUARTER_EPO_REQ, buff, &offset);
        buff_put_int(wn, buff, &offset);
        buff_put_int(tow, buff, &offset);
        buff_put_int(sys_time, buff, &offset);
        if (-1 == mnld_sendto_hal(mnld2hal_fd, MTK_MNLD2HAL, buff, sizeof(buff))) {
            MND_ERR("Send to HAL failed, %s\n", strerror(errno));
        } else {
            MND_MSG("MTK_AGPS_CB_EPO_REQ Send to HAL successfully\n");
        }
    }
    return ret;
}
/*****************************************************************************/
static int epoll_add(int epoll_fd, int fd)
{
    struct epoll_event  ev;
    int ret = 0, flags;

    if (epoll_fd == C_INVALID_FD)
        return -1;

    /* important: make the fd non-blocking */
    flags = fcntl(fd, F_GETFL);
    ret = fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    if (ret < 0) {
        MND_ERR("epoll_add fcntl fail(%s) \n", strerror(errno));
        return -1;
    }

    ev.events  = EPOLLIN;
    ev.data.fd = fd;
    do {
        ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
    } while (ret < 0 && errno == EINTR);
    return ret;
}
/*****************************************************************************/
static int epoll_del(int epoll_fd, int fd)
{
    struct epoll_event  ev;
    int                 ret;

    if (epoll_fd == C_INVALID_FD)
        return -1;

    ev.events  = EPOLLIN;
    ev.data.fd = fd;
    do {
        ret = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &ev);
    } while (ret < 0 && errno == EINTR);
    return ret;
}
/*****************************************************************************/
static int epoll_init(void)
{
    int epoll_fd = epoll_create(EPOLL_NUM);
    MNLD_DATA_T *obj = &mnld_data;

    if (epoll_fd < 0)
        return -1;

    if (obj->cur_accept_socket != C_INVALID_FD) {
        if (epoll_add(epoll_fd, obj->cur_accept_socket))
            return -1;
    }

    if (obj->sig_rcv_fd != C_INVALID_FD) {
        if (epoll_add(epoll_fd, obj->sig_rcv_fd))
            return -1;
    }

    if (obj->mnl_rcv_agps_fd != C_INVALID_FD) {
        if (epoll_add(epoll_fd, obj->mnl_rcv_agps_fd)) {
            MND_ERR("epoll_add mnl_rcv_agps_fd fail");
            // return -1;
        }
    }

    if (obj->mnl_rcv_mtklogger_fd != C_INVALID_FD) {
        if (epoll_add(epoll_fd, obj->mnl_rcv_mtklogger_fd)) {
            MND_ERR("epoll_add mnl_rcv_mtklogger_fd fail");
            return -1;
        } else {
            MND_MSG("Print:epoll_add success");
        }
    }
#ifdef MPE_ENABLE
    if (obj->mpe_rcv_fd != C_INVALID_FD) {
        if (epoll_add(epoll_fd, obj->mpe_rcv_fd)) {
            MND_ERR("epoll_add mpe_rcv_fd fail");
        }
    }
#endif
#ifdef MNL_OFFLOAD_SUPPORT
    if (obj->ofl_tst_fd != C_INVALID_FD) {
    if (epoll_add(epoll_fd, obj->ofl_tst_fd)) {
        MND_ERR("[OFL]epoll_add ofl_tst_fd fail, err=%d, %s",
            errno, strerror(errno));
            // return -1;
        }
    }
    if (obj->flp_mnld_fd != C_INVALID_FD) {
        if (epoll_add(epoll_fd, obj->flp_mnld_fd)) {
            MND_ERR("[FLP2MNLD]epoll_add flp_mnld_fd fail");
            // return -1;
        }
    }
#endif
    obj->epoll_fd = epoll_fd;
    return 0;
}
/*****************************************************************************/
static void epoll_destroy(void)
{
    MNLD_DATA_T *obj = &mnld_data;

    if ((obj) && (obj->epoll_fd != C_INVALID_FD)) {
        if (close(obj->epoll_fd))
            MND_ERR("close(%d) : %d (%s)\n", obj->epoll_fd, errno, strerror(errno));
    }
}
/*****************************************************************************/
void mnl_alarm_handler(sigval_t v)
{
    char buf[] = {(char)v.sival_int};
    MND_MSG("mnl_alarm_handler:%d\n", (int)(*buf));
    slf_send_cmd(buf, sizeof(buf));
}
/*****************************************************************************/
static inline int mnl_alarm_stop(int alarm_idx)
{
    int err = 0;
    MNL_TIMER_T *obj = &mnl_timer;

    if (alarm_idx >= MNL_ALARM_IDX_MAX) {
        err = -1;   /*out-of-range*/
    } else if (obj->id[alarm_idx] != C_INVALID_TIMER) {
        if ((err = timer_delete(obj->id[alarm_idx]))) {
            MND_ERR("timer_delete(%.8X) = %d (%s)\n", (long)obj->id[alarm_idx], errno, strerror(errno));
            return -1;
        }
        obj->id[alarm_idx] = C_INVALID_TIMER;
        obj->type = MNL_ALARM_UNSUPPORTED;
    } else {
        /*the alarm is already stopped*/
    }
    return err;
}
/*****************************************************************************/
static inline int mnl_alarm_stop_watch()
{
    MND_TRC();
    return mnl_alarm_stop(MNL_ALARM_IDX_WATCH);
}
/*****************************************************************************/
static int mnl_alarm_stop_all()
{
    int idx, err;
    MNL_TIMER_T *obj = &mnl_timer;
    for (idx = 0; idx < MNL_ALARM_IDX_MAX; idx++) {
        if (obj->id[idx] != C_INVALID_TIMER) {
            if ((err = timer_delete(obj->id[idx]))) {
                MND_ERR("timer_delete(%ld) = %d (%s)\n", (long)obj->id, errno, strerror(errno));
                return -1;
            }
            obj->id[idx] = C_INVALID_TIMER;
            obj->type = MNL_ALARM_UNSUPPORTED;
        }
    }
    return 0;
}
/*****************************************************************************/
static int mnl_set_alarm(int type)
{
    int err = 0;
    MNL_TIMER_T *obj = &mnl_timer;
    MNL_ALARM_T *ptr;
    if (type >= MNL_ALARM_MAX) {
        MND_ERR("invalid alarm type: %d\n", type);
        return -1;
    }
    ptr = &mnl_alarm[type];
    if (ptr->idx >= MNL_ALARM_IDX_MAX) {
        MND_ERR("invalid alarm index: %d\n", type);
        return -1;
    }
    if (obj->id[ptr->idx] != C_INVALID_TIMER) {
        if (obj->type != type) {
            // MND_MSG("timer_delete(0x%.8X)\n", obj->id);
            if ((err = timer_delete(obj->id[ptr->idx]))) {
                MND_ERR("timer_delete(%ld) = %d (%s)\n", (long)obj->id, errno, strerror(errno));
                return -1;
            }
            obj->id[ptr->idx] = C_INVALID_TIMER;
            obj->type = MNL_ALARM_UNSUPPORTED;
        }
    }
    if (obj->id[ptr->idx] == C_INVALID_TIMER) {
        memset(&obj->evt, 0x00, sizeof(obj->evt));
        obj->evt.sigev_value.sival_ptr = &obj->id[ptr->idx];
        obj->evt.sigev_value.sival_int = ptr->cmd;
        obj->evt.sigev_notify = SIGEV_THREAD;
        obj->evt.sigev_notify_function = mnl_alarm_handler;
        obj->type = type;
        if ((err = timer_create(CLOCK_REALTIME, &obj->evt, &obj->id[ptr->idx]))) {
            MND_ERR("timer_create = %d(%s)\n", errno, strerror(errno));
            return -1;
        }
        // MND_MSG("timer_create(0x%.8X)\n", obj->id);
    }

    /*setup on-shot timer*/
    obj->expire.it_interval.tv_sec = 0;
    obj->expire.it_interval.tv_nsec = 0;
    obj->expire.it_value.tv_sec = ptr->period;
    obj->expire.it_value.tv_nsec = 0;
    if ((err = timer_settime(obj->id[ptr->idx], 0, &obj->expire, NULL))) {
        MND_ERR("timer_settime = %d(%s)\n", errno, strerror(errno));
        return -1;
    }
    MND_MSG("(%d, 0x%.8X, %d)\n", ptr->idx, (long)obj->id[ptr->idx], ptr->period);
    return 0;
}
/*****************************************************************************/
static int mnl_read_attr(const char *name, unsigned char *attr)
{
    int fd = open(name, O_RDWR);
    unsigned char buf;
    int err = 0;

    if (fd == -1) {
        MND_ERR("open %s err = %s\n", name, strerror(errno));
        return err;
    }
    do {
        err = read(fd, &buf, sizeof(buf));
    } while (err < 0 && errno == EINTR);
    if (err != sizeof(buf)) {
        MND_ERR("read fails = %s\n", strerror(errno));
        err = -1;
    } else {
        err = 0;    /*no error*/
    }
    if (close(fd) == -1) {
        MND_ERR("close fails = %s\n", strerror(errno));
        err = (err) ? (err) : (-1);
    }
    if (!err)
        *attr = buf - '0';
    else
        *attr = 0xFF;
    return err;
}
/*****************************************************************************/
static int mnl_write_attr(const char *name, unsigned char attr)
{
    int err, fd = open(name, O_RDWR);
    char buf[] = {attr + '0'};

    if (fd == -1) {
        MND_ERR("open %s err = %s\n", name, strerror(errno));
        return -errno;
    }
    do { err = write(fd, buf, sizeof(buf)); }
    while (err < 0 && errno == EINTR);

    if (err != sizeof(buf)) {
        MND_ERR("write fails = %s\n", strerror(errno));
        err = -errno;
    } else {
        err = 0;    /*no error*/
    }
    if (close(fd) == -1) {
        MND_ERR("close fails = %s\n", strerror(errno));
        err = (err) ? (err) : (-errno);
    }
    MND_MSG("write '%d' to %s okay\n", attr, name);
    return err;
}
/*****************************************************************************/
static int mnl_set_pwrctl(unsigned char pwrctl)
{
    if (pwrctl < GPS_PWRCTL_MAX) {
        return mnl_write_attr(MNL_ATTR_PWRCTL, pwrctl);
    } else {
        MND_ERR("invalid pwrctl = %d\n", pwrctl);
        errno = -EINVAL;
        return -1;
    }
}
/*****************************************************************************/
static int mnl_get_pwrctl(unsigned char *pwrctl)
{
    return mnl_read_attr(MNL_ATTR_PWRCTL, pwrctl);
}
/*****************************************************************************/
static int mnl_set_suspend(unsigned char suspend)
{
    if (suspend < GPS_PWR_MAX) {
        return mnl_write_attr(MNL_ATTR_SUSPEND, suspend);
    } else {
        MND_ERR("invalid suspend = %d\n", suspend);
        errno = -EINVAL;
        return -1;
    }
}
/*****************************************************************************/
static int mnl_get_suspend(unsigned char *suspend)
{
    return mnl_read_attr(MNL_ATTR_SUSPEND, suspend);
}
/*****************************************************************************/
static int mnl_set_state(unsigned char state)
{
    int err;
    if (state < GPS_STATE_MAX) {
        if ((err = mnl_write_attr(MNL_ATTR_STATE, state)))
            return err;
        mnld_data.state = state;
        return 0;
    } else {
        MND_ERR("invalid state = %d\n", state);
        errno = -EINVAL;
        return -1;
    }
}
/*****************************************************************************/
static int mnl_get_state(unsigned char *state)
{
    return mnl_read_attr(MNL_ATTR_STATE, state);
}
/*****************************************************************************/
static int mnl_set_pwrsave(unsigned char pwrsave)
{
    if (pwrsave < GPS_PWRSAVE_MAX) {
        return mnl_write_attr(MNL_ATTR_PWRSAVE, pwrsave);
    } else {
        MND_ERR("invalid pwrsave = %d\n", pwrsave);
        errno = -EINVAL;
        return -1;
    }
}
/*****************************************************************************/
static int mnl_get_pwrsave(unsigned char *pwrsave)
{
    return mnl_read_attr(MNL_ATTR_PWRSAVE, pwrsave);
}
/*****************************************************************************/
static int mnl_set_status(char *buf, int len)
{
    const char *name = MNL_ATTR_STATUS;
    int err, fd = open(name, O_RDWR);

    if (fd == -1) {
        MND_ERR("open %s err = %s\n", name, strerror(errno));
        return -errno;
    }
    do {
        err = write(fd, buf, len);
    } while (err < 0 && errno == EINTR);

    if (err != len) {
        MND_ERR("write fails = %s\n", strerror(errno));
        err = -errno;
    } else {
        err = 0;    /*no error*/
    }
    if (close(fd) == -1) {
        MND_ERR("close fails = %s\n", strerror(errno));
        err = (err) ? (err) : (-errno);
    }
    return err;
}
/*****************************************************************************/
// For GPS.LOG
void get_time_stamp(char time_str1[], char time_str2[]) {
    struct tm *tm_pt;
    time_t time_st;
    struct timeval tv;

    MND_MSG("Get time now");
    time(&time_st);
    gettimeofday(&tv, NULL);
    tm_pt = gmtime(&time_st);
    MND_MSG("time_st = %ld, tv.tv_sec = %ld", time_st, tv.tv_sec);
    tm_pt = localtime(&tv.tv_sec);
    memset(time_str1, 0, sizeof(char)*30);
    memset(time_str2, 0, sizeof(char)*30);
    if (tm_pt) {
        sprintf(time_str1, "%d%02d%02d%02d%02d%02d.%1ld", tm_pt->tm_year+1900, tm_pt->tm_mon+1, tm_pt->tm_mday,
        tm_pt->tm_hour, tm_pt->tm_min, tm_pt->tm_sec, tv.tv_usec/100000);
        sprintf(time_str2, "%d%02d%02d%02d%02d%02d.%03ld", tm_pt->tm_year+1900, tm_pt->tm_mon+1, tm_pt->tm_mday,
        tm_pt->tm_hour, tm_pt->tm_min, tm_pt->tm_sec, tv.tv_usec/1000);
    }
}
int get_ttff_time() {
    time_t current_time;
    time(&current_time);
    ttff = current_time - start_time;
    return ttff;
}
unsigned long get_file_size(const char* filename) {
    struct stat buf;
    if (stat("/sdcard/GPS.LOG", &buf) < 0) {
        MND_ERR("get file size error: %s", strerror(errno));
        return 0;
    }
    return (unsigned long)buf.st_size;
}
int write_gps_log_to_sdcard(char* content) {
    int fd_sd = 0;
    int ret = 0;
    char file_sd[] = "/sdcard/GPS.LOG";
    unsigned long len = 0;
    int retry = 0;

    if (access(file_sd, 0) == -1) {
        MND_MSG("file does not exist!\n");
    } else {
        MND_MSG("GPS.LOG file exist");
    }
    len = get_file_size(file_sd);
    MND_MSG("file size = %lu", len);
    if (len >= (unsigned long)65535) {
        MND_MSG("File size is lager than 64K");
        truncate(file_sd, 0);
    } else {
        MND_MSG("file size = %lu", len);
    }

    fd_sd = open(file_sd, O_RDWR | O_CREAT | O_APPEND | O_NONBLOCK, S_IRUSR | S_IROTH | S_IRGRP);
    if (fd_sd != -1) {
        do {
            ret = write(fd_sd, content, strlen(content));
            MND_ERR("write to /sdcard/ fail, retry: %d, %s", retry, strerror(errno));
            retry++;
        }while ((retry < 2) && (ret < 0));
        close(fd_sd);
    } else {
        MND_ERR("open file_sd fail: %s, %d!", strerror(errno), errno);
        ret = -1;
    }
    return ret;
}
void write_gps_log(int flag) {

    char str[256] = {0};
    char time_str1[30] = {0};
    char time_str2[30] = {0};

    if (get_prop(6)) {
        memset(str, 0x00, sizeof(str));
        memset(time_str1, 0x00, sizeof(time_str1));
        memset(time_str2, 0x00, sizeof(time_str2));
        get_time_stamp(time_str1, time_str2);
        if (flag == 1)
            sprintf(str, "[%s]0x00000000: %s #gps start\r\n", time_str2, time_str2);
        else if (flag == 0)
            sprintf(str, "[%s]0x00000001: %s #gps stop\r\n", time_str2, time_str2);

        if (write_gps_log_to_sdcard(str) < 0)
            MND_ERR("/sdcard/GPS.LOG write fail");

    }
}
void write_gps_location(char* position) {

    char str[256] = {0};
    char time_str1[30] = {0};
    char time_str2[30] = {0};

    if (ttff == 0) {
        ttff = get_ttff_time();
        MND_MSG("TTFF = %d", ttff);
    } else {
        MND_MSG("Already get ttff: %d", ttff);
    }

    if (get_prop(6)) {
        // read position

        MND_MSG("pos = %s", position);
        // write sdcard
        memset(time_str1, 0x00, sizeof(time_str1));
        memset(time_str2, 0x00, sizeof(time_str2));

        get_time_stamp(time_str1, time_str2);
        sprintf(str, "[%s]0x00000002: %s, %s, %d #position(time_stamp, lat, lon, ttff)\r\n",
            time_str2, time_str2, position, ttff);
        MND_MSG("gps postion  = %s", str);
        MND_MSG("/sdcard/GPS.LOG write: %s", str);
        if (write_gps_log_to_sdcard(str) < 0)
            MND_ERR("/sdcard/GPS.LOG write position fail");
        MND_MSG("/sdcard/GPS.LOG write done");
    }
}
// For GPS.LOG end
/*****************************************************************************/
static int mnl_attr_init()
{
    int err;
    char buf[48];
    time_t tm;
    struct tm *p;

    time(&tm);
    p = localtime(&tm);
    if (p == NULL) {
        return -1;
    }
    snprintf(buf, sizeof(buf), "(%d/%d/%d %d:%d:%d) - %d/%d",
        p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec,
        0, MNL_RESTART_NONE);
    if ((err = mnl_set_status(buf, sizeof(buf))))
        return err;
    if ((err = mnl_set_pwrctl(GPS_PWRCTL_OFF)))
        return err;
    if ((err = mnl_set_suspend(GPS_PWR_RESUME)))
        return err;
    if ((err = mnl_set_state(GPS_STATE_PWROFF)))
        return err;
    if ((err = mnl_set_pwrsave(GPS_PWRSAVE_SLEEP)))
        return err;
    return 0;
}
/*****************************************************************************/
static int mnl_set_active()
{
    if (mnld_data.state == GPS_STATE_SLEEP || mnld_data.state == GPS_STATE_PWROFF) {
        MND_MSG("ignore active: state(%d)\n", mnld_data.state);
        return 0;
    } else {
        return mnl_set_alarm(MNL_ALARM_MONITOR);
    }
}
/*****************************************************************************/
static int mnl_init()
{
    int err;
    int s[2];
    MNLD_DATA_T *obj = &mnld_data;

    if ((err = mnl_attr_init()))
        return err;

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, s))
        return -1;

    fcntl(s[0], F_SETFD, FD_CLOEXEC);
    fcntl(s[0], F_SETFL, O_NONBLOCK);
    fcntl(s[1], F_SETFD, FD_CLOEXEC);
    fcntl(s[1], F_SETFL, O_NONBLOCK);

    obj->sig_snd_fd = s[0];
    obj->sig_rcv_fd = s[1];

    /*setup property*/
    if (!mnl_utl_load_property(&mnld_cfg)) {
        mnl_alarm[MNL_ALARM_INIT].period = mnld_cfg.timeout_init;
        mnl_alarm[MNL_ALARM_MONITOR].period = mnld_cfg.timeout_monitor;
        mnl_alarm[MNL_ALARM_SLEEP].period = mnld_cfg.timeout_sleep;
        mnl_alarm[MNL_ALARM_PWROFF].period = mnld_cfg.timeout_pwroff;
        mnl_alarm[MNL_ALARM_WAKEUP].period = mnld_cfg.timeout_wakeup;
        mnl_alarm[MNL_ALARM_TTFF].period = mnld_cfg.timeout_ttff;
    }
    return 0;

}
#ifdef MNL_OFFLOAD_SUPPORT
bool mnl_offload_is_enabled() {
    return !!(mnld_cfg.OFFLOAD_enabled);
}
#endif
#if RAW_DATA_SUPPORT
void get_gps_measurement_clock_data()
{
    MND_MSG("get_gps_measurement_clock_data begin");

    int i;
    MTK_GPS_MEASUREMENT mtk_gps_measurement[32];
    INT8 ch_proc_ord_prn[32]={0};
    mtk_gps_get_measurement(mtk_gps_measurement, ch_proc_ord_prn);
    MND_MSG("sizeof(mtk_gps_get_measurement) = %d,sizeof(mtk_gps_get_measurement[0]) = %d\n",
        sizeof(mtk_gps_measurement), sizeof(mtk_gps_measurement[0]));

    if (gps_raw_debug_mode) {
        for (i = 0; i < 32; i++) {
            if (mtk_gps_measurement[i].size > 0) {
                MND_MSG("i = %d,s ize = %d, flag = %d, PRN = %d, TimeOffsetInNs = %d, state = %d, ReGpsTowInNs = %d\n",
                    i, mtk_gps_measurement[i].size, mtk_gps_measurement[i].flag,
                    mtk_gps_measurement[i].PRN, mtk_gps_measurement[i].TimeOffsetInNs,
                    mtk_gps_measurement[i].state, mtk_gps_measurement[i].ReGpsTowInNs);
                MND_MSG("i = %d, ReGpsTowUnInNs = %d, Cn0InDbHz = %d, PRRateInMeterPreSec = %d, \
                    PRRateUnInMeterPreSec = %d, AcDRState10 = %d, AcDRInMeters = %d\n",
                    i, mtk_gps_measurement[i].ReGpsTowUnInNs, mtk_gps_measurement[i].Cn0InDbHz,
                    mtk_gps_measurement[i].PRRateInMeterPreSec, mtk_gps_measurement[i].PRRateUnInMeterPreSec,
                    mtk_gps_measurement[i].AcDRState10, mtk_gps_measurement[i].AcDRInMeters);
                MND_MSG("i = %d, AcDRUnInMeters = %d, PRInMeters = %d, PRUnInMeters = %d, CPInChips = %d, \
                    CPUnInChips = %d, CFInhZ = %d\n",
                    i, mtk_gps_measurement[i].AcDRUnInMeters, mtk_gps_measurement[i].PRInMeters,
                    mtk_gps_measurement[i].PRUnInMeters, mtk_gps_measurement[i].CPInChips,
                    mtk_gps_measurement[i].CPUnInChips, mtk_gps_measurement[i].CFInhZ);
                MND_MSG("i = %d, CarrierCycle = %d, CarrierPhase = %d, CarrierPhaseUn = %d, \
                    LossOfLock = %d, BitNumber = %d, TimeFromLastBitInMs = %d\n",
                    i, mtk_gps_measurement[i].CarrierCycle, mtk_gps_measurement[i].CarrierPhase,
                    mtk_gps_measurement[i].CarrierPhaseUn, mtk_gps_measurement[i].LossOfLock,
                    mtk_gps_measurement[i].BitNumber, mtk_gps_measurement[i].TimeFromLastBitInMs);
                MND_MSG("i = %d, DopperShiftInHz = %d, DopperShiftUnInHz = %d, MultipathIndicater = %d, \
                    SnrInDb = %d, ElInDeg = %d, ElUnInDeg = %d\n",
                    i, mtk_gps_measurement[i].DopperShiftInHz, mtk_gps_measurement[i].DopperShiftUnInHz,
                    mtk_gps_measurement[i].MultipathIndicater, mtk_gps_measurement[i].SnrInDb,
                    mtk_gps_measurement[i].ElInDeg, mtk_gps_measurement[i].ElUnInDeg);
                MND_MSG("i = %d,AzInDeg = %d, AzUnInDeg = %d,UsedInFix = %d\n",
                    i, mtk_gps_measurement[i].AzInDeg, mtk_gps_measurement[i].AzUnInDeg,
                    mtk_gps_measurement[i].UsedInFix);
            }
        }
    }

    MTK_GPS_CLOCK mtk_gps_clock;
    int ret = 0;
    ret = mtk_gps_get_clock(&mtk_gps_clock);
    if (gps_raw_debug_mode) {
        MND_MSG("sizeof(MTK_GPS_CLOCK) = %d, size = %d, flag = %d, leapsecond = %d, type = %d, \
            TimeInNs = %d, TimeUncertaintyInNs = %d, FullBiasInNs = %d, BiasInNs = %d, \
            BiasUncertaintyInNs = %d, DriftInNsPerSec = %d, DriftUncertaintyInNsPerSec = %d",
            sizeof(MTK_GPS_CLOCK), mtk_gps_clock.size, mtk_gps_clock.flag, mtk_gps_clock.leapsecond,
            mtk_gps_clock.type, mtk_gps_clock.TimeInNs, mtk_gps_clock.TimeUncertaintyInNs,
            mtk_gps_clock.FullBiasInNs, mtk_gps_clock.BiasInNs, mtk_gps_clock.BiasUncertaintyInNs,
            mtk_gps_clock.DriftInNsPerSec, mtk_gps_clock.DriftUncertaintyInNsPerSec);
    }

    if (ret == 1) {
        MND_MSG("mtk_gps_get_clock success,[ret=%d]\n", ret);
        int offset = 0;
        char buff[9000] = {0};
        buff_put_int(MNL_CMD_MEASUREMENT, buff, &offset);
        buff_put_struct(mtk_gps_measurement, 32*sizeof(MTK_GPS_MEASUREMENT), buff, &offset);
        MND_MSG("buff_put_int 'mtk_gps_measurement[32]' success, sizeof(mtk_gps_measurement[32]) = %d\n",
            32*sizeof(MTK_GPS_MEASUREMENT));

        buff_put_struct(&mtk_gps_clock, sizeof(MTK_GPS_CLOCK), buff, &offset);
        MND_MSG("buff_put_struct 'mtk_gps_clock' success, sizeof(mtk_gps_clock) = %d\n", sizeof(MTK_GPS_CLOCK));

        if (-1 == mnld_sendto_hal(mnld2hal_fd, MTK_MNLD2HAL, buff, sizeof(buff)))
            MND_ERR("Send gpsmeasurement data to HAL failed, %s\n", strerror(errno));
        else
            MND_MSG("Send gpsmeasurement data to HAL successfully,buff = %d\n", sizeof(buff));
    } else {
        MND_MSG("mtk_gps_get_clock fail,[ret=%d]\n", ret);
    }
}
void get_gps_navigation_event()
{
    MND_MSG("get_gps_navigation_event begin");

    int svid;
    int i;
    MTK_GPS_NAVIGATION_EVENT *gps_navigation_event = (MTK_GPS_NAVIGATION_EVENT*)
    malloc(sizeof(MTK_GPS_NAVIGATION_EVENT));
    if (gps_navigation_event == NULL) {
        MND_MSG("point gps_navigation_event is null,return!");
        return;
    }
    for (svid = 0; svid < 32; svid++) {
        MND_MSG("ready to mtk_gps_get_navigation_event");
        int ret = mtk_gps_get_navigation_event(gps_navigation_event, svid);
        if (ret == 1) {
            MND_MSG("mtk_gps_get_navigation_event success, SVID = %d, [ret=%d]\n", svid, ret);

            if (gps_raw_debug_mode) {
                MND_MSG("size = %d, %p, prn = %d, %p, type = %d, %p, message_id = %d, %p, \
                    submessage_id = %d, %p, data_length = %d, %p",
                    gps_navigation_event->size, &gps_navigation_event->size, gps_navigation_event->prn,
                    &gps_navigation_event->prn, gps_navigation_event->type, &gps_navigation_event->type,
                    gps_navigation_event->messageID, &gps_navigation_event->messageID,
                    gps_navigation_event->submessageID, &gps_navigation_event->submessageID,
                    gps_navigation_event->length, &gps_navigation_event->length);
                for (i = 0; i < 40; i++) {
                    MND_MSG("MNLD: gps_navigation_event->data[%d] = %x, %p",
                        i, gps_navigation_event->data[i], &gps_navigation_event->data[i]);
                }
            }

            int offset = 0;
            char buff[300] = {0};
            buff_put_int(MNL_CMD_NAVIGATION, buff, &offset);
            buff_put_struct(gps_navigation_event, sizeof(MTK_GPS_NAVIGATION_EVENT), buff, &offset);
            MND_MSG("buff_put_struct 'gps_navigation_event' success, \
                sizeof(MTK_GPS_NAVIGATION_EVENT) = %d \n", sizeof(MTK_GPS_NAVIGATION_EVENT));
            if (-1 == mnld_sendto_hal(mnld2hal_fd, MTK_MNLD2HAL, buff, sizeof(buff)))
                MND_ERR("Send gpsnavigation data to HAL failed, %s\n", strerror(errno));
            else
                MND_MSG("Send gpsnavigation data to HAL successfully\n");
        } else {
            MND_MSG("mtk_gps_get_navigation_event fail,SVID = %d,[ret=%d]\n", svid, ret);
        }
    }
    free(gps_navigation_event);
}
#endif
/*****************************************************************************/
static time_t last_send_time = 0;
static time_t current_time = 0;
static int callback_flags = 0;

INT32
mtk_gps_sys_gps_mnl_callback (MTK_GPS_NOTIFICATION_TYPE msg)
{
    switch (msg) {
    case MTK_GPS_MSG_FIX_READY:
        {
            // For NI open GPS

            double dfRtcD = 0.0, dfAge = 0.0;
            send_active_notify();
            if (mtk_gps_get_rtc_info(&dfRtcD, &dfAge) == MTK_GPS_SUCCESS) {
                MND_MSG("MTK_GPS_MSG_FIX_READY, GET_RTC_OK, %.3lf, %.3lf\n", dfRtcD, dfAge);
                MND_MSG("Age = %d, RTCDiff = %d, Time_interval = %d\n", mnl_config.AVAILIABLE_AGE,
                    mnl_config.RTC_DRIFT, mnl_config.TIME_INTERVAL);
                if ((dfAge <= mnl_config.AVAILIABLE_AGE) && (dfRtcD >= mnl_config.RTC_DRIFT ||
                    dfRtcD <= -mnl_config.RTC_DRIFT) && dfRtcD < 5000) {
                    int fd_fmsta = -1;
                    unsigned char buf[2]= {0};
                    int status = -1;
                    fd_fmsta = open("/proc/fm", O_RDWR);
                    if (fd_fmsta < 0) {
                        MND_MSG("open /proc/fm error\n");
                    } else {
                        MND_MSG("open /proc/fm success!\n");
                        status = read(fd_fmsta, &buf, sizeof(buf));
                        if (status < 0)
                            MND_MSG("read fm status fails = %s\n", strerror(errno));
                        if (close(fd_fmsta) == -1)
                            MND_MSG("close fails = %s\n", strerror(errno));
                    }

                    if (buf[0] == '2') {
                        INT32 time_diff;
                        time(&current_time);
                        time_diff = current_time - last_send_time;
                        if ((0 == last_send_time) || (time_diff > mnl_config.TIME_INTERVAL)) {
                            int fd_fmdev = -1;
                            int ret = 0;
                            struct fm_gps_rtc_info rtcInfo;
                            fd_fmdev = open("dev/fm", O_RDWR);
                            if (fd_fmdev < 0) {
                                MND_MSG("open fm dev error\n");
                            }
                            else {
                                rtcInfo.retryCnt = 2;
                                rtcInfo.ageThd = mnl_config.AVAILIABLE_AGE;
                                rtcInfo.driftThd = mnl_config.RTC_DRIFT;
                                rtcInfo.tvThd.tv_sec = mnl_config.TIME_INTERVAL;
                                rtcInfo.age = dfAge;
                                rtcInfo.drift = dfRtcD;
                                rtcInfo.tv.tv_sec = current_time;
                                ret = ioctl(fd_fmdev, FM_IOCTL_GPS_RTC_DRIFT, &rtcInfo);
                                if (ret) {
                                    MND_MSG("send rtc info failed, [ret=%d]\n", ret);
                                }
                                ret = close(fd_fmdev);
                                if (ret) {
                                    MND_MSG("close fm dev error\n");
                                }
                            }
                        }
                    }
                }
            }
            else {
                MND_MSG("MTK_GPS_MSG_FIX_READY,GET_RTC_FAIL\n");
            }
     #if RAW_DATA_SUPPORT
            if (gps_raw_debug_mode && !mtk_msg_raw_meas_flag) {
                MND_MSG("raw_debug_mode is open, send MTK_MSG_RAW_MEAS to libmnl\n");

                INT32 ret = MTK_GPS_ERROR;
                ret = mtk_gps_set_param(MTK_MSG_RAW_MEAS, NULL);
                MND_MSG("mtk_gps_set_param,ret = %d\n", ret);
                if (ret != MTK_GPS_SUCCESS) {
                    MNL_ERR("send MTK_MSG_RAW_MEASto mnl fail,please reopen gps\n");
                } else {
                    MNL_MSG("send MTK_MSG_RAW_MEAS to mnl OK \n");
                    mtk_msg_raw_meas_flag = 1;  // Don't send MTK_MSG_RAW_MEAS when it was sent to mnl successfully
                }
            }

            unsigned char state = GPS_STATE_UNSUPPORTED;
            int err = mnl_get_state(&state);
            if ((err) || (state >= GPS_STATE_MAX)) {
                MND_ERR("mnl_get_state() = %d, %d\n", err, state);
            }
            MND_MSG("gps_measurement_state = %d ,state = %d", gps_measurement_state, state);
            /*get gps measurement and clock data*/
            if (gps_measurement_state == GPS_MEASUREMENT_INIT && state == GPS_STATE_START) {
                get_gps_measurement_clock_data();
            }

            /*get gps navigation event */
            if (gps_navigation_state == GPS_NAVIGATION_INIT && state == GPS_STATE_START) {
                get_gps_navigation_event();
            }
    #endif
        }
        break;
    case MTK_GPS_MSG_FIX_PROHIBITED:
        {
            send_active_notify();
            MND_MSG("MTK_GPS_MSG_FIX_PROHIBITED\n");
        }
        break;
    default:
        break;
    }
    return  MTK_GPS_SUCCESS;
}
/*****************************************************************************/
int linux_gps_dev_init(void)
{
    return 0;
}
/*****************************************************************************/
int
linux_gps_init (void)
{
    int clock_type;
    int gnss_mode_flag = 0;
    INT32 status = MTK_GPS_SUCCESS;
    static MTK_GPS_INIT_CFG init_cfg;
    static MTK_GPS_DRIVER_CFG driver_cfg;
    UINT8 clk_type = 0xff;  //  for new 43EVK board
    FILE *parm_fp;
    MTK_GPS_BOOT_STATUS mnl_status = 0;
    double latitude, longitude;
    int accuracy  = 100;
    int ret_val = 0;

    memset(&init_cfg, 0, sizeof(MTK_GPS_INIT_CFG));
    memset(&driver_cfg, 0, sizeof(MTK_GPS_DRIVER_CFG));
    MTK_AGPS_USER_PROFILE userprofile;
    memset(&userprofile, 0, sizeof(MTK_AGPS_USER_PROFILE));
    //  ====== default config ======
    init_cfg.if_type = MTK_IF_UART_NO_HW_FLOW_CTRL;
    init_cfg.pps_mode = MTK_PPS_DISABLE;        //  PPS disabled
    init_cfg.pps_duty = 100;                    //  pps_duty (100ms high)
    init_cfg.if_link_spd = 115200;              //  115200bps

    UINT32 hw_ver = 0;
    UINT32 fw_ver = 0;

#ifdef MTK_GPS_CO_CLOCK_DATA_IN_MD
    typedef struct gps_nvram_t {
        unsigned int C0;
        unsigned int C1;
        unsigned int initU;
        unsigned int lastU;
    }GPS_NVRAM_COCLOCK_T;
    GPS_NVRAM_COCLOCK_T gps_clock_calidata;
    int fd = -1;
    int read_size;
    fd = open(GPS_CALI_DATA_PATH, O_RDONLY, 660);
    if (fd == -1) {
        MNL_MSG("open %s error is %s\n", GPS_CALI_DATA_PATH, strerror(errno));
        gps_clock_calidata.C0 = 0x0;
        gps_clock_calidata.C1 = 0x0;
        gps_clock_calidata.initU = 0x0;
        gps_clock_calidata.lastU = 0x0;
    } else {
        read_size = read(fd, &gps_clock_calidata, sizeof(GPS_NVRAM_COCLOCK_T));
        if (read_size != sizeof(GPS_NVRAM_COCLOCK_T)) {
            MNL_MSG("read size is %d, structure size is %d\n", read_size, sizeof(GPS_NVRAM_COCLOCK_T));
        }
        close(fd);
        fd = -1;
    }
    MNL_MSG("=====================");
    MNL_MSG("co = %lx\n", gps_clock_calidata.C0);
    MNL_MSG("c1 = %lx\n", gps_clock_calidata.C1);
    MNL_MSG("initU = %lx\n", gps_clock_calidata.initU);
    MNL_MSG("lastU = %lx\n", gps_clock_calidata.lastU);
    init_cfg.C0 = gps_clock_calidata.C0;
    init_cfg.C1 = gps_clock_calidata.C1;
    init_cfg.initU = gps_clock_calidata.initU;
    init_cfg.lastU = gps_clock_calidata.lastU;
#endif
    if (gps_nvram_valid == 1) {
        init_cfg.hw_Clock_Freq = stGPSReadback.gps_tcxo_hz;            //  26MHz TCXO,
        init_cfg.hw_Clock_Drift = stGPSReadback.gps_tcxo_ppb;                 //  0.5ppm TCXO
        init_cfg.Int_LNA_Config = stGPSReadback.gps_lna_mode;                   //  0 -> Mixer in , 1 -> Internal LNA
        init_cfg.u1ClockType = stGPSReadback.gps_tcxo_type;  // clk_type;
#ifdef MTK_GPS_CO_CLOCK_DATA_IN_MD
#else
        init_cfg.C0 = stGPSReadback.C0;
        init_cfg.C1 = stGPSReadback.C1;
        init_cfg.initU = stGPSReadback.initU;
        init_cfg.lastU = stGPSReadback.lastU;
#endif
    } else {
        init_cfg.hw_Clock_Freq = 26000000;             //  26MHz TCXO
        init_cfg.hw_Clock_Drift = 2000;                 //  0.5ppm TCXO
        init_cfg.Int_LNA_Config = 0;                    //  0 -> Mixer in , 1 -> Internal LNA
        init_cfg.u1ClockType = 0xFF;  // clk_type;
    }

    if (init_cfg.hw_Clock_Drift == 0) {
        MND_MSG("customer didn't set clock drift value, use default value\n");
        init_cfg.hw_Clock_Drift = 2000;
    }

    /*setting 1Hz/5Hz */
    if (g_is_1Hz) {
        init_cfg.fix_interval = 1000;               //  1Hz update rate
    } else {
        init_cfg.fix_interval = 200;               //  5Hz update rate
    }


    init_cfg.datum = MTK_DATUM_WGS84;           //  datum
    init_cfg.dgps_mode = MTK_AGPS_MODE_AUTO;    //  enable SBAS

    dsp_fd = open(mnl_config.dev_dsp, O_RDWR);
    if (dsp_fd == -1) {
        MND_MSG("open_port: Unable to open - %s \n", mnl_config.dev_dsp);
        return MTK_GPS_ERROR;
    } else {
        MND_MSG("open dsp successfully\n");
    }

    if (strcmp(chip_id, "0x6592") == 0 || strcmp(chip_id, "0x6571") == 0
        || strcmp(chip_id, "0x6580") == 0 || strcmp(chip_id, "0x0321") == 0
        || strcmp(chip_id, "0x0335") == 0 || strcmp(chip_id, "0x0337") == 0
        ||strcmp(chip_id, "0x6735") == 0 || strcmp(chip_id, "0x8163") == 0
        || strcmp(chip_id, "0x8127") == 0 || strcmp(chip_id, "0x6755") == 0
        || strcmp(chip_id, "0x6797") == 0) {
        clock_type = ioctl(dsp_fd, 11, NULL);
        clock_type = clock_type & 0x00ff;
        switch (clock_type) {
        case 0x00:
            MND_MSG("TCXO, buffer 2\n");
            init_cfg.u1ClockType = 0xFF;
            if (property_set(GPS_CLOCK_TYPE_P, "20") != 0)
                MND_ERR("set GPS_CLOCK_TYPE_P %s\n", strerror(errno));
            break;
        case 0x10:
            MND_MSG("TCXO, buffer 1\n");
            init_cfg.u1ClockType = 0xFF;
            if (property_set(GPS_CLOCK_TYPE_P, "10") != 0)
                MND_ERR("set GPS_CLOCK_TYPE_P %s\n", strerror(errno));
            break;
        case 0x20:
            MND_MSG("TCXO, buffer 2\n");
            init_cfg.u1ClockType = 0xFF;
            if (property_set(GPS_CLOCK_TYPE_P, "20") != 0)
                MND_ERR("set GPS_CLOCK_TYPE_P %s\n", strerror(errno));
            break;
        case 0x30:
            MND_MSG("TCXO, buffer 3\n");
            init_cfg.u1ClockType = 0xFF;
            if (property_set(GPS_CLOCK_TYPE_P, "30") != 0)
                MND_ERR("set GPS_CLOCK_TYPE_P %s\n", strerror(errno));
            break;
        case 0x40:
            MND_MSG("TCXO, buffer 4\n");
            init_cfg.u1ClockType = 0xFF;
            if (property_set(GPS_CLOCK_TYPE_P, "40") != 0)
                MND_ERR("set GPS_CLOCK_TYPE_P %s\n", strerror(errno));
            break;
        case 0x01:
            MND_MSG("GPS coclock, buffer 2, coTMS\n");
            init_cfg.u1ClockType = 0xFE;
            if (property_set(GPS_CLOCK_TYPE_P, "21") != 0)
                MND_ERR("set GPS_CLOCK_TYPE_P %s\n", strerror(errno));
            break;
        case 0x02:
        case 0x03:
            MND_MSG("TCXO, buffer 2, coVCTCXO\n");
            init_cfg.u1ClockType = 0xFF;
            if (property_set(GPS_CLOCK_TYPE_P, "20") != 0)
                MND_ERR("set GPS_CLOCK_TYPE_P %s\n", strerror(errno));
            break;
        case 0x11:
            MND_MSG("GPS coclock, buffer 1\n");
            init_cfg.u1ClockType = 0xFE;
            if (property_set(GPS_CLOCK_TYPE_P, "11") != 0)
                MND_ERR("set GPS_CLOCK_TYPE_P %s\n", strerror(errno));
            break;
        case 0x21:
            MND_MSG("GPS coclock, buffer 2\n");
            init_cfg.u1ClockType = 0xFE;
            if (property_set(GPS_CLOCK_TYPE_P, "21") != 0)
                MND_ERR("set GPS_CLOCK_TYPE_P %s\n", strerror(errno));
            break;
        case 0x31:
            MND_MSG("GPS coclock, buffer 3\n");
            init_cfg.u1ClockType = 0xFE;
            if (property_set(GPS_CLOCK_TYPE_P, "31") != 0)
                MND_ERR("set GPS_CLOCK_TYPE_P %s\n", strerror(errno));
            break;
        case 0x41:
            MND_MSG("GPS coclock, buffer 4\n");
            init_cfg.u1ClockType = 0xFE;
            if (property_set(GPS_CLOCK_TYPE_P, "41") != 0)
                MND_ERR("set GPS_CLOCK_TYPE_P %s\n", strerror(errno));
            break;
        default:
            MND_ERR("unknown clock type, clocktype = %x\n", clock_type);
        }
    } else {
        if (strcmp(chip_id, "0x6572") == 0 || strcmp(chip_id, "0x6582") == 0
            || strcmp(chip_id, "0x6630") == 0 || strcmp(chip_id, "0x6752") == 0) {
            /*Add clock type to display on YGPS by mtk06325 2013-12-09 begin */
            if (0xFF == init_cfg.u1ClockType) {
                MND_MSG("TCXO\n");
                if (property_set(GPS_CLOCK_TYPE_P, "90") != 0) {
                    MND_ERR("set GPS_CLOCK_TYPE_P %s\n", strerror(errno));
                }
            } else if (0xFE == init_cfg.u1ClockType) {
                MND_MSG("GPS coclock\n");
                if (property_set(GPS_CLOCK_TYPE_P, "91") != 0) {
                    MND_ERR("set GPS_CLOCK_TYPE_P %s\n", strerror(errno));
                }
            } else {
                MND_MSG("GPS unknown clock\n");
            }
        }
        /*Add clock type to display on YGPS by mtk06325 2013-12-09 end */
    }

    if (ioctl(dsp_fd, 10, NULL) == 1) {
        MND_MSG("clear RTC\n");
        delete_aiding_data = GPS_DELETE_TIME;
    } else
        MND_MSG("need do nothing for RTC\n");


    if (strcmp(chip_id, "0x6628") == 0) {
        init_cfg.reservedy = (void *)MTK_GPS_CHIP_KEY_MT6628;
        init_cfg.reservedx = MT6628_E1;
    } else if (strcmp(chip_id, "0x6630") == 0) {
        init_cfg.reservedy = (void *)MTK_GPS_CHIP_KEY_MT6630;
        if (ioctl(dsp_fd, COMBO_IOC_GPS_IC_HW_VERSION, &hw_ver) < 0) {
            MND_MSG("get COMBO_IOC_GPS_IC_HW_VERSION failed\n");
            return MTK_GPS_ERROR;
        }

        if (ioctl(dsp_fd, COMBO_IOC_GPS_IC_FW_VERSION, &fw_ver) < 0) {
            MND_MSG("get COMBO_IOC_GPS_IC_FW_VERSION failed\n");
            return MTK_GPS_ERROR;
        }

        if ((hw_ver == 0x8A00) && (fw_ver == 0x8A00)) {
            MND_MSG("MT6630_E1\n");
            init_cfg.reservedx = MT6630_E1;
        } else if ((hw_ver == 0x8A10) && (fw_ver == 0x8A10)) {
            MND_MSG("MT6630_E2\n");
            init_cfg.reservedx = MT6630_E2;
        } else if ((hw_ver >= 0x8A11) && (fw_ver >= 0x8A11)) {
            MND_MSG("MT6630 chip dection done,hw_ver = %d and fw_ver = %d\n", hw_ver, fw_ver);
            init_cfg.reservedx = MT6630_E2;  /*mnl match E1 or not E1,so we send MT6630_E2 to mnl */
        } else {
            MND_MSG("hw_ver = %d and fw_ver = %d\n", hw_ver, fw_ver);
            init_cfg.reservedx = MT6630_E2; /*default value*/
        }
    } else if (strcmp(chip_id, "0x6572") == 0) {
        init_cfg.reservedy = (void *)MTK_GPS_CHIP_KEY_MT6572;
        init_cfg.reservedx = MT6572_E1;
    } else if (strcmp(chip_id, "0x6571") == 0) {
        init_cfg.reservedy = (void *)MTK_GPS_CHIP_KEY_MT6571;
        init_cfg.reservedx = MT6571_E1;
    } else if (strcmp(chip_id, "0x8127") == 0) {
        init_cfg.reservedy = (void *)MTK_GPS_CHIP_KEY_MT6571;
        init_cfg.reservedx = MT6571_E1;
    } else if (strcmp(chip_id, "0x6582") == 0) {
        init_cfg.reservedy = (void *)MTK_GPS_CHIP_KEY_MT6582;
        init_cfg.reservedx = MT6582_E1;
    } else if (strcmp(chip_id, "0x6592") == 0) {
        init_cfg.reservedy = (void *)MTK_GPS_CHIP_KEY_MT6592;
        init_cfg.reservedx = MT6592_E1;
    } else if (strcmp(chip_id, "0x3332") == 0) {
        init_cfg.reservedy = (void *)MTK_GPS_CHIP_KEY_MT3332;
        init_cfg.reservedx = MT3332_E2;
    } else if (strcmp(chip_id, "0x6752") == 0) {
        init_cfg.reservedy = (void *)MTK_GPS_CHIP_KEY_MT6752;
        init_cfg.reservedx = MT6752_E1;
    } else if (strcmp(chip_id, "0x8163") == 0) {
        mnl_config.GNSSOPMode = 3;  // gps only
        init_cfg.reservedy = (void *)MTK_GPS_CHIP_KEY_MT6735M;
        init_cfg.reservedx = MT6735M_E1;
    } else if (strcmp(chip_id, "0x6580") == 0) {
        init_cfg.reservedy = (void *)MTK_GPS_CHIP_KEY_MT6580;
        init_cfg.reservedx = MT6580_E1;
    } else if (strcmp(chip_id, "0x0321") == 0) {  // Denali1
        init_cfg.reservedy = (void *)MTK_GPS_CHIP_KEY_MT6735;
        init_cfg.reservedx = MT6735_E1;

        gnss_mode_flag = ioctl(dsp_fd, 9, NULL);  //  32'h10206198 value is 01
        MND_MSG("gnss_mode_flag=%d \n", gnss_mode_flag);

        if (((gnss_mode_flag & 0x01000000) != 0) && ((gnss_mode_flag & 0x02000000) == 0)) {
            mnl_config.GNSSOPMode = 3;  //  gps only
        }
    } else if (strcmp(chip_id, "0x0335") == 0) {   // Denali2
        init_cfg.reservedy = (void *)MTK_GPS_CHIP_KEY_MT6735M;
        init_cfg.reservedx = MT6735M_E1;
    } else if (strcmp(chip_id, "0x0337") == 0) {    // Denali3
        init_cfg.reservedy = (void *)MTK_GPS_CHIP_KEY_MT6753;
        init_cfg.reservedx = MT6753_E1;
    } else if (strcmp(chip_id, "0x6755") == 0) {
        init_cfg.reservedy = (void *)MTK_GPS_CHIP_KEY_MT6755;
        init_cfg.reservedx = MT6755_E1;
#ifdef MPE_ENABLE
        isTriggerMpe = 1;
#endif
        gnss_mode_flag = ioctl(dsp_fd, 9, NULL);  // 32'h10206048 value is 01
        MND_MSG("gnss_mode_flag=%d \n", gnss_mode_flag);

        if (((gnss_mode_flag & 0x01000000) != 0) && ((gnss_mode_flag & 0x02000000) == 0)) {
			mnl_config.GNSSOPMode = 3; // gps only
        }
    } else if (strcmp(chip_id, "0x6797") == 0) {
        init_cfg.reservedy = (void *)MTK_GPS_CHIP_KEY_MT6797;
        init_cfg.reservedx = MT6797_E1;
#ifdef MPE_ENABLE
        isTriggerMpe = 1;
#endif
    } else {
        MND_ERR("chip is unknown, chip id is %s\n", chip_id);
    }

    MND_MSG("Get chip version type (%p) \n", init_cfg.reservedy);
    MND_MSG("Get chip version value (%d) \n", init_cfg.reservedx);

    if (mnl_config.ACCURACY_SNR == 1) {
        init_cfg.reservedx |=(UINT32)0x80000000;
    } else if (mnl_config.ACCURACY_SNR == 2) {
        init_cfg.reservedx |=(UINT32)0x40000000;
    } else if (mnl_config.ACCURACY_SNR == 3) {
        init_cfg.reservedx |=(UINT32)0xC0000000;;
    }

    MND_MSG("ACCURACY_SNR = %d\n", mnl_config.ACCURACY_SNR);
    init_cfg.mtk_gps_version_mode = MTK_GPS_AOSP_MODE;
    MND_MSG("mtk_gps_version_mode = %d\n", init_cfg.mtk_gps_version_mode);

    init_cfg.GNSSOPMode = mnl_config.GNSSOPMode;
    MNL_MSG("GNSSOPMode: %d\n", init_cfg.GNSSOPMode);

    strcpy(driver_cfg.nv_file_name, NV_FILE);
    strcpy(driver_cfg.nmeain_port_name, mnl_config.dev_dbg);
    strcpy(driver_cfg.nmea_port_name, mnl_config.dev_gps);
    strcpy(driver_cfg.dsp_port_name, mnl_config.dev_dsp);
    strcpy((char *)driver_cfg.bee_path_name, mnl_config.bee_path);
    driver_cfg.reserved   =   mnl_config.BEE_enabled;
    MND_MSG("zqh: mnl_config.BEE_enabled, %d\n", driver_cfg.reserved);

    driver_cfg.DebugType    =   gps_debuglog_state | mnl_config.dbg2file;
    strcpy(driver_cfg.dbg_file_name, gps_debuglog_file_name);

    driver_cfg.u1AgpsMachine = mnl_config.u1AgpsMachine;
    strcpy((char *)driver_cfg.epo_file_name, mnl_config.epo_file);
    strcpy((char *)driver_cfg.epo_update_file_name, mnl_config.epo_update_file);
    strcpy((char *)driver_cfg.qepo_file_name, mnl_config.qepo_file);
    strcpy((char *)driver_cfg.qepo_update_file_name, mnl_config.qepo_update_file);

    driver_cfg.log_file_max_size = mnl_config.dbglog_file_max_size;
    driver_cfg.log_folder_max_size = mnl_config.dbglog_folder_max_size;

    driver_cfg.u1AgpsMachine = mnl_config.u1AgpsMachine;
    if (driver_cfg.u1AgpsMachine == 1)
        MND_MSG("we use CRTU to test\n");
    else
        MND_MSG("we use Spirent to test\n");

    status = mtk_gps_delete_nv_data(assist_data_bit_map);

    MND_MSG("u4Bitmap, %d\n", status);
    MND_MSG("init_cfg.C0 = %d\n", init_cfg.C0);
    MND_MSG("init_cfg.C1 = %d\n", init_cfg.C1);
    MND_MSG("init_cfg.initU = %d\n", init_cfg.initU);
    MND_MSG("init_cfg.lastU = %d\n", init_cfg.lastU);

#ifdef MNL_SO
#ifdef MNL_OFFLOAD_SUPPORT
#else
    MTK_GPS_SYS_FUNCTION_PTR_T*  mBEE_SYS_FP = &porting_layer_callback;
    if (mtk_gps_sys_function_register(mBEE_SYS_FP) != MTK_GPS_SUCCESS) {
        MND_ERR("register callback for mnl error\n");
        status = MTK_GPS_ERROR;
        return status;
    }
#endif
    driver_cfg.dsp_fd = dsp_fd;
#endif
#ifdef MNL_OFFLOAD_SUPPORT
    if (mnl_offload_is_enabled()) {
        mtk_gps_ofl_set_option(MNL_OFL_OPTION_ENALBE_OFFLOAD);
        MNL_MSG("MNL_OFL_OPTION_ENALBE_OFFLOAD = 1");
    } else {
        mtk_gps_ofl_set_option(0);
        MNL_MSG("MNL_OFL_OPTION_ENALBE_OFFLOAD = 0");
    }
#endif
    mnl_status = mtk_gps_mnl_run((const MTK_GPS_INIT_CFG*)&init_cfg , (const MTK_GPS_DRIVER_CFG*)&driver_cfg);
    MND_MSG("Status (%d) \n", mnl_status);
    if (mnl_status != MNL_INIT_SUCCESS) {
        status = MTK_GPS_ERROR;
        return status;
    }
#ifdef MPE_ENABLE
    MND_MSG("isTriggerMpe =%d", isTriggerMpe);
{
  MND_MSG("mpe enabled");
  int ret;
  g_ThreadExitmnlmpe = 0;
  pipe_close = 0;
  if(isTriggerMpe) {
  gMpeCallBackFunc = mtk_gps_mnl_trigger_mpe;
    ret = mtk_gps_mnl_mpe_callback_reg(gMpeCallBackFunc);
    MND_MSG("register mpe cb %d", ret);
    if (pthread_create(&mnlmpe_thread[0].thread_handle,NULL, thread_mnlmpe, NULL)) {
      MND_ERR("mnlmpe thread init failed");
  } else {
      MND_MSG("mnlmpe_thread create ok!! \n");
  }
  signal(SIGPIPE, check_pipe_handler);
}
}
#endif
    if (access(EPO_UPDATE_HAL, F_OK) == -1) {
        MND_MSG("EPO file does not exist, no EPO yet\n");
    } else {
        MND_MSG("there is a EPOHAL file, please mnl update EPO.DAT from EPOHAL.DAT\n");
        if (mtk_agps_agent_epo_file_update() == MTK_GPS_ERROR) {
            MND_ERR("QEPO file updates fail\n");
        }
    }
    if (access(QEPO_UPDATE_HAL, F_OK) == -1) {
        MND_MSG("QEPO file does not exist, no EPO yet\n");
    } else {
        MND_MSG("there is a QEPOHAL file, please mnl update QEPO.DAT from QEPOHAL.DAT\n");
        if (mtk_agps_agent_qepo_file_update() == MTK_GPS_ERROR) {
            MND_ERR("EPO file updates fail\n");
        }
    }

    MND_MSG("dsp port (%s) \n", driver_cfg.dsp_port_name);
    MND_MSG("nmea port (%s) \n", driver_cfg.nmea_port_name);
    MND_MSG("nmea dbg port (%s) \n", driver_cfg.nmeain_port_name);
    MND_MSG("dbg_file_name (%s) \n", driver_cfg.dbg_file_name);
    MND_MSG("DebugType (%d) \n", driver_cfg.DebugType);
    MND_MSG("nv_file_name (%s) \n", driver_cfg.nv_file_name);

    if (epo_setconfig == 1) {
        userprofile.EPO_enabled = mnl_config.EPO_enabled;
    } else {
        userprofile.EPO_enabled = get_prop(7);
    }
    MND_MSG("EPO_enabled (%d) \n", userprofile.EPO_enabled);
    // userprofile.EPO_enabled = mnl_config.EPO_enabled;
    userprofile.BEE_enabled = mnl_config.BEE_enabled;
    userprofile.SUPL_enabled = mnl_config.SUPL_enabled;
    userprofile.EPO_priority = mnl_config.EPO_priority;
    userprofile.BEE_priority = mnl_config.BEE_priority;
    userprofile.SUPL_priority = mnl_config.SUPL_priority;
    userprofile.fgGpsAosp_Ver = mnl_config.fgGpsAosp_Ver;
#if RAW_DATA_SUPPORT
    gps_raw_data_enable();
#endif
     unsigned int i = 0;
    //  if sending profile msg fail, re-try 2-times, each time sleep 10ms
    for (i = 0; i < 3; i++) {
        INT32 ret = MTK_GPS_ERROR;
        ret = mtk_agps_set_param(MTK_MSG_AGPS_MSG_PROFILE, &userprofile, MTK_MOD_DISPATCHER, MTK_MOD_AGENT);
        if (ret != MTK_GPS_SUCCESS) {
            MNL_MSG("%d st send profile to agent fail. try again \n", i);
            usleep(10000);  //  sleep 10ms for init agent message queue
            ret = mtk_agps_set_param(MTK_MSG_AGPS_MSG_PROFILE, &userprofile, MTK_MOD_DISPATCHER, MTK_MOD_AGENT);
        } else {
            MNL_MSG("%d st send profile to agent OK \n", i);
            break;
        }
    }

    if (mtk_gps_get_position_accuracy(&latitude, &longitude, &accuracy) == MTK_GPS_SUCCESS) {
       MND_MSG("mnl init, mtk_gps_get_position_accuracy success, %lf, %lf, %d", latitude, longitude, accuracy);
       if (accuracy < 100) {
            ret_val = mnl2agps_location_sync(latitude, longitude, accuracy);
            if (0 == ret_val) {
                MND_MSG("mnl2agps_location_sync success");
            }
        }
    }
    return  status;
}
/*****************************************************************************/
int
linux_gps_uninit (void)
{
    int idx ,err;
    for (idx = 0; idx < MNL_THREAD_NUM; idx++) {
        if (mnl_thread[idx].thread_handle == C_INVALID_TID)
            continue;
        if (!mnl_thread[idx].thread_exit)
            continue;
        if ((err = mnl_thread[idx].thread_exit(&mnl_thread[idx]))) {
            MND_ERR("fails to thread_exit thread %d], err = %d\n", idx, err);
            return MTK_GPS_ERROR;
        }
    }
    close(dsp_fd);
    MNL_MSG("close dsp_fd \n");
    return MTK_GPS_SUCCESS;
}
/*****************************************************************************/
int linux_gps_dev_uninit(void)
{
#if CFG_DBG_INFO_GPIO_TOGGLE
    /*init gpio port*/
    if (gpio_fd != C_INVALID_FD)
        close(gpio_fd);
    /*init gpio port*/
#endif
#if CFG_DBG_INFO_UART_OUTPUT
    /*GPS debug info UART */
    if (dbg_info_uart_fd != C_INVALID_FD) {
        close(dbg_info_uart_fd);
        dbg_info_uart_fd = C_INVALID_FD;
    }
#endif
    return 0;
}
/*****************************************************************************/
static int mtk_gps_exit_proc(void)
{
    int err = 0;
    int ret = 0;
    /*finalize library*/
    //  1. DSP in
    //  2. PMTK in
    //  3. BEE
    //  4. MNL
    // 5. MPE
    if (0 == mnld_monitor.terminated) {
        MND_MSG("MNL exiting \n");
        mtk_gps_mnl_stop();
        MND_MSG("mtk_gps_mnl_stop()\n");
#ifdef MPE_ENABLE
        if(isTriggerMpe) {
          if ((ret = linux_mpe_uninit()))
          {
            MND_ERR("linux_mpe_uninit err = %d\n", errno);
            err = (err) ? (err) : (ret);
        }
        }
#endif
        if ((ret = linux_gps_uninit()))
        {
            MND_ERR("linux_gps_uninit err = %d\n", errno);
            err = (err) ? (err) : (ret);
        }

        if ((ret = linux_gps_dev_uninit()))
        {
            MND_ERR("linux_gps_dev_uninit err = %d\n", errno);
            err = (err) ? (err) : (ret);
        }
        if ((ret = mtk_gps_sys_uninit()))
        {
            MND_ERR("mtk_gps_sys_uninit err = %d=\n", errno);
            err = (err) ? (err) : (ret);
        }
        callback_flags = 0;
        MND_MSG("callback_flags = %d --\n", callback_flags);

    }
    else {
        MND_MSG("MNL has exited, jump out\n");
    }
    mnld_monitor.terminated = 1;
    MND_MSG("MNL exiting down,  = %d\n", err);
    return err;
}
/*****************************************************************************/
void get_gps_version()
{
    if (strcmp(chip_id, "0x0321") == 0 || strcmp(chip_id, "0x0335") == 0 ||
        strcmp(chip_id, "0x0337") == 0 ||strcmp(chip_id, "0x6735") == 0) {
        property_set("gps.gps.version", "0x6735");  // Denali1/2/3
    } else {
        property_set("gps.gps.version", chip_id);
    }
    return;
}
/*****************************************************************************/
static int launch_daemon_thread(void)
{
    MNLD_DATA_T *obj = &mnld_data;
    int idx, ret;
    pthread_t thread_cmd;
    clock_t beg,end;
    struct sched_param sched, test_sched;
    int policy = 0xff;
    int err = 0xff;

    mnl_utl_load_property(&mnl_config);

#if 0
    // get chipId
    int fd;
    if ((fd = open(GPS_PROPERTY, O_RDONLY)) == -1)
        MND_ERR("open %s error, %s\n", GPS_PROPERTY, strerror(errno));
    if (read(fd, chip_id, sizeof(chip_id)) == -1)
        MND_ERR("open %s error, %s\n", GPS_PROPERTY, strerror(errno));
    close(fd);
#endif
    MND_MSG("chip_id is %s\n", chip_id);

    g_is_1Hz = 1;

    /* adjust priority when 5 Hz Mode */
    policy = sched_getscheduler(0);
    sched_getparam(0, &test_sched);
    MND_ERR("Before %s policy = %d, priority = %d\n", "main" , policy, test_sched.sched_priority);

    sched.sched_priority = sched_get_priority_max(SCHED_FIFO);
    err = sched_setscheduler(0, SCHED_FIFO, &sched);
    if (err == 0) {
        MND_ERR("pthread_setschedparam SUCCESS \n");
        policy = sched_getscheduler(0);
        sched_getparam(0, &test_sched);
        MND_ERR("After %s policy = %d, priority = %d\n", "main" ,policy , test_sched.sched_priority);
    } else {
        if (err == EINVAL) MND_ERR("policy is not one of SCHED_OTHER, SCHED_RR, SCHED_FIFO\n");
        if (err == EINVAL) MND_ERR("the  priority  value  specified by param is not valid for the specified policy\n");
        if (err == EPERM) MND_ERR("the calling process does not have superuser permissions\n");
        if (err == ESRCH) MND_ERR("the target_thread is invalid or has already terminated\n");
        if (err == EFAULT)  MND_ERR("param points outside the process memory space\n");
        MND_ERR("pthread_setschedparam FAIL \n");
    }


    for (idx = 0; idx < MNL_THREAD_NUM; idx++)
    {
        mnl_thread[idx].thread_id = MNL_THREAD_UNKNOWN;
        mnl_thread[idx].thread_handle = C_INVALID_TID;
        mnl_thread[idx].thread_exit = NULL;
        mnl_thread[idx].thread_active = NULL;
        /*send to mnld itself*/
        mnl_thread[idx].snd_fd = obj->sig_snd_fd;
    }
#ifdef MPE_ENABLE
   mnlmpe_thread[0].thread_id = MNL_THREAD_UNKNOWN;
   mnlmpe_thread[0].thread_handle = C_INVALID_TID;
   mnlmpe_thread[0].thread_exit = NULL;
   mnlmpe_thread[0].thread_active = NULL;
#endif
    mnl_set_alarm(MNL_ALARM_INIT);

    /*initialize system resource (message queue, mutex) used by library*/
    if ((err = mtk_gps_sys_init()))
    {
        MND_MSG("mtk_gps_sys_init err = %d\n",errno);
        // mtk_gps_exit_proc();
        // return err;
    } else {
        MND_MSG("mtk_gps_sys_init() success\n");
    }

    /*initialize UART/GPS device*/
    if ((err = linux_gps_dev_init()))
    {
        MND_MSG("linux_gps_dev_init err = %d\n", errno);
        mtk_gps_exit_proc();
        return err;
    } else {
        MND_MSG("linux_gps_dev_init() success\n");
    }

    /*initialize library thread*/
    if ((err = linux_gps_init()))
    {
        MND_MSG("linux_gps_init err = %d\n", errno);
        mtk_gps_exit_proc();
        return err;
    }
    else
    {
        MND_MSG("linux_gps_init() success\n");
    }

    if ((err = linux_setup_signal_handler()))
    {
        MNL_MSG("linux_setup_signal_handler err = %d\n", errno);
        mtk_gps_exit_proc();
        return err;
    }
    else
    {
        MND_MSG("linux_setup_signal_handler() success\n");
    }

    get_gps_version();
    MND_MSG("MNL running..\n");
    return MTK_GPS_SUCCESS;
}
/*****************************************************************************/
#define restart_mnl_process(X) restart_mnl_process_ex(X, __LINE__)
/*****************************************************************************/
static int restart_mnl_process_ex(unsigned int reborn, unsigned int line)
{
    int err;
    char buf[] = {MNL_CMD_QUIT};
    time_t tm;
    struct tm *p;

    time(&tm);
    p = localtime(&tm);
    if (p == NULL)
    {
        return -1;
    }
    MND_MSG("(%d,%d) (%d/%d/%d %d:%d:%d)", reborn, line,
        p->tm_year+1900, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);

#ifdef MNL_OFFLOAD_SUPPORT
    {
        mnld_monitor.terminated = 1;
        mtk_gps_ofl_rst_stpgps_begin_ntf();
        close(dsp_fd);
            // TODO: To avoid restart caused by watch timer expiration
        MND_MSG("[rst stpgps]dsp_fd(%d) was closed. reopen it(%s)\n", dsp_fd, mnl_config.dev_dsp);
        dsp_fd = open(mnl_config.dev_dsp, O_RDWR);
        if (dsp_fd < 0) {
            MND_MSG("[rst stpgps]open_port: Unable to open - %s \n", mnl_config.dev_dsp);
            // TODO: ERROR HANDLING
        } else {
                   MND_MSG("[rst stpgps]open dsp successfully, fd = %d\n", dsp_fd);
                   mtk_gps_ofl_rst_stpgps_end_ntf(dsp_fd);
                   mnld_monitor.terminated = 0;
            }
        }
        return 0;  // 0 -> no error
#else /* ! MNL_OFFLOAD_SUPPORT */

    if (err = mtk_gps_exit_proc()) {
        return err;
        MND_MSG("mtk_gps_exit_proc for restart: %d", err);
    }
    if ((err = mnl_set_pwrctl(GPS_PWRCTL_RST_FORCE)))
        return err;
    if ((err = launch_daemon_thread()))
        return err;

    MND_MSG("restart success");
    mnld_monitor.terminated = 0;

    mnld_monitor.count++;
    char buf_restart[48];
    snprintf(buf_restart, sizeof(buf_restart), "(%d/%d/%d %d:%d:%d) - %d/%d",
        p->tm_year, 1 + p->tm_mon, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec,
        mnld_monitor.count, reborn);
    return mnl_set_status(buf_restart, strlen(buf_restart));
#endif /* MNL_OFFLOAD_SUPPORT */
}
#ifdef MNL_OFFLOAD_SUPPORT
/*****************************************************************************/
INT32 mtk_gps_ofl_sys_rst_stpgps_req() {
    int ret;

    MND_MSG("");
    restart_mnl_process(MNL_RESTART_FORCE);
    return ret;
}
/*****************************************************************************/
INT32 mtk_gps_ofl_sys_submit_flp_data(UINT8 *buf, UINT32 len) {
    MND_MSG("[OFL]flp submit to host, buf=%p, len=%d", buf, len);
    if (ofl_lpbk_tst_start) {
        static char tmp_buf[1024];
        static int  tmp_buf_w = 0;
        memcpy(&tmp_buf[tmp_buf_w], buf, len);
        tmp_buf_w+=len;
        if (tmp_buf_w >= ofl_lpbk_tst_size) {
            MND_MSG("[OFL] lpbk recv enougth data, actual=%d, except=%d, lpbk restart...",
                tmp_buf_w, ofl_lpbk_tst_size);
            mtk_gps_ofl_send_flp_data(&tmp_buf[0], ofl_lpbk_tst_size);
            tmp_buf_w = 0;
        }
    }
    else if ((buf != NULL) && (len != 0) && (len < 1023) && (mnld2flp_fd != -1)) {
        UINT8 buff[1024] = {0};
        MTK_FLP_OFFLOAD_MSG_T *prMsg = (MTK_FLP_OFFLOAD_MSG_T *)&buff[0];

        prMsg->type = FLP_MNL_CMD_DATA_RECV;
        prMsg->length = len;
        memcpy(&prMsg->data[0], buf, len);
        if (-1 == mnld_sendto_hal(mnld2flp_fd, MNL_MNLD_TO_FLP, buff, sizeof(buff))) {
            MND_ERR("[FLP2MNLD]Send to FLP failed, %s\n", strerror(errno));
            return MTK_GPS_ERROR;
        }
        else {
            MND_MSG("[FLP2MNLD]Send to FLP successfully\n");
        }
    }
    return MTK_GPS_SUCCESS;
}
#endif

/*****************************************************************************/
int start_mnl_process(void)
{
    int err = 0;
    MND_TRC();
    gps_cnt++;
    if (gps_cnt <= 0) {
        MND_MSG("gps_cnt is wrong: %d, correct it", gps_cnt);
        gps_cnt = 1;
    }

    if (gps_cnt > 1) {
        MND_MSG("GPS driver has been opened, gps_cnt = %d\n", gps_cnt);
      if ((gps_user & GPS_USER_AGPS) != 0)
          release_condition(&lock_for_sync[M_START]);
        return 0;
    }

    MND_MSG("GPS driver is stopped, gps_cnt++ = %d\n", gps_cnt);
    if ((err = mnl_alarm_stop_all())) /*if current state is going to pwrsave*/
        return err;
    if (mnld_monitor.terminated == 1) {
        MND_MSG("before lanuch daemon, mnld_monitor.terminated = 1\n");
        if ((err = mnl_set_pwrctl(GPS_PWRCTL_RST))) /*if current state is power off*/
            return err;
        if ((err = launch_daemon_thread()))
            return err;
        mnld_monitor.terminated = 0;
        MND_MSG("after launch daemon, set mnld_monitor.terminated = 0\n");
        return mnl_set_state(GPS_STATE_START);
    } else {
        MND_MSG("daemon is stared, mnld_monitor.terminated = 0\n");
        unsigned char state = GPS_STATE_UNSUPPORTED;
        unsigned char pwrctl = GPS_PWRCTL_UNSUPPORTED;
        err = mnl_get_pwrctl(&pwrctl);
        if ((err) || (pwrctl >= GPS_STATE_MAX)) {
            MND_ERR("mnl_get_pwrctl() = %d, %d\n", err, pwrctl);
            return -1;
        }
        err = mnl_get_state(&state);
        if ((err) || (state >= GPS_STATE_MAX)) {
            MND_ERR("mnl_get_state() = %d, %d\n", err, state);
            return -1;
        }
        MND_MSG("start: pwrctl (%d), state (%d)\n", pwrctl, state);
        if (pwrctl == GPS_PWRCTL_OFF) {
            if ((err = mnl_set_pwrctl(GPS_PWRCTL_ON))) /*if current state is power off*/
                return err;
            return restart_mnl_process(MNL_RESTART_FORCE);
        }
        if (state == GPS_STATE_SLEEP) {
            if ((err = mtk_gps_set_param(MTK_PARAM_CMD_WAKEUP, NULL)))
                MND_ERR("MNL wakeup = %d\n", err);
            if ((err = mnl_set_alarm(MNL_ALARM_WAKEUP)))
                return err;
            return mnl_set_state(GPS_STATE_START);
        }

        if (state == GPS_STATE_STOP) {   // for sync GPS state
            MND_MSG("state from STOP to START again\n");
            mnl_set_state(GPS_STATE_START);
        }

        MND_MSG("mnl_daemon is already started!!\n");
    }
    return 0;
}
int stop_mnl_process(void)
{
    int err;
    unsigned char pwrsave;
    MND_TRC();

    gps_cnt--;
    /*In case of user disable GPS in settings*/
    if (gps_cnt < 0) {
        gps_cnt = 0;
        MND_MSG("It should be user disable gps\n");
        in_stop_proc = 0;
        return 0;
    }

    if (gps_cnt != 0) {
        MND_MSG("No need stop GPS, gps_cnt = %d\n", gps_cnt);

        // when agps opens gps, and user opens gps by app again,some flags will be set up.
        // then user stops gps, we should send msg to hal to clear those flags.
        if ((in_stop_proc == 1) && ((gps_user & GPS_USER_APP) != 0)) {
            if (mnld2hal_fd != -1) {
                char buff[1024] = {0};
                int offset = 0;
                buff_put_int(MNL_CMD_MNL_DIE, buff, &offset);
                if (-1 == mnld_sendto_hal(mnld2hal_fd, MTK_MNLD2HAL, buff, sizeof(buff)))
                    MND_ERR("Send to HAL failed, %s\n", strerror(errno));
                else
                    MND_MSG("Send to HAL successfully\n");
            }
        }

        in_stop_proc = 0;
        return 0;
    }
    MND_MSG("GPS driver needs stop now, gps_cnt: %d\n", gps_cnt);

    mnl_alarm_stop_all();
    if (err = mtk_gps_exit_proc()) {
        MND_MSG("sigrcv_handler: err = %d", err);
        return err;
    } else {
        MND_MSG("mtk_gps_exit_proc success");
    }
    // cancel alarm
    MND_MSG("Cancel alarm");
    alarm(0);

    mnl_set_pwrctl(GPS_PWRCTL_OFF);
    mnl_set_state(GPS_STATE_PWROFF);

    assist_data_bit_map = FLAG_HOT_START;
    // notify HAL stop
    if ((mnld2hal_fd != -1) && ((gps_user & GPS_USER_APP) != 0)) {
        char buff[1024] = {0};
        int offset = 0;
        buff_put_int(MNL_CMD_MNL_DIE, buff, &offset);
        if (-1 == mnld_sendto_hal(mnld2hal_fd, MTK_MNLD2HAL, buff, sizeof(buff)))
            MND_ERR("Send to HAL failed, %s\n", strerror(errno));
        else
            MND_MSG("Send to HAL successfully\n");
    }

    in_stop_proc = 0;
    return 0;
}
/*****************************************************************************/
/*****************************************************************************/
static void sighlr(int signo)
{
    int err = 0;
    pthread_t self = pthread_self();
    // MND_MSG("Signal handler of %.8x -> %s\n", (unsigned int)self, sys_siglist[signo]);
    if (signo == SIGUSR1) {
        char buf[] = {MNL_CMD_ACTIVE};
        err = slf_send_cmd(buf, sizeof(buf));
    } else if (signo == SIGALRM) {
        char buf[] = {MNL_CMD_TIMEOUT};
        err = slf_send_cmd(buf, sizeof(buf));
    } else if (signo == SIGUSR2) {
        char buf[] = {MNL_CMD_TIMEOUT};
        err = slf_send_cmd(buf, sizeof(buf));
    } else if (signo == SIGCHLD) {
        mnld_monitor.terminated = 1;
    }
}
/*****************************************************************************/
static int setup_signal_handler(void)
{
    struct sigaction actions;
    int err;

    /*the signal handler is MUST, otherwise, the thread will not be killed*/
    memset(&actions, 0, sizeof(actions));
    sigemptyset(&actions.sa_mask);
    actions.sa_flags = 0;
    actions.sa_handler = sighlr;
    if ((err = sigaction(SIGUSR1, &actions, NULL))) {
        MND_MSG("register signal hanlder for SIGUSR1: %s\n", strerror(errno));
        return -1;
    } if ((err = sigaction(SIGUSR2, &actions, NULL))) {
        MND_MSG("register signal hanlder for SIGUSR2: %s\n", strerror(errno));
        return -1;
    } if ((err = sigaction(SIGALRM, &actions, NULL))) {
        MND_MSG("register signal handler for SIGALRM: %s\n", strerror(errno));
        return -1;
    } if ((err = sigaction(SIGCHLD, &actions, NULL))) {
        MND_MSG("register signal handler for SIGALRM: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

int get_epo_time() {
    MNLD_DATA_T *obj = &mnld_data;
    int ret = 0;
    time_t time[2];

    /*function call directly*/
    if (MTK_GPS_ERROR == mtk_agps_agent_epo_read_utc_time(&mnl_epo_time.uSecond_start, &mnl_epo_time.uSecond_expire)) {
        MND_ERR("Get EPO start/expire time fail\n ");
        char buff[1024] = {0};
        int offset = 0;
        buff_put_int(MNL_CMD_READ_EPO_TIME_FAIL, buff, &offset);
        if (-1 == mnld_sendto_hal(mnld2hal_fd, MTK_MNLD2HAL, buff, sizeof(buff))) {
            MND_ERR("Send to HAL failed\n");
            return MTK_GPS_ERROR;
        } else {
            MND_MSG("Send to HAL successfully\n");
            return MTK_GPS_SUCCESS;
        }
    } else {
        MND_MSG("Get EPO start/expire time successfully\n");
        MND_MSG("mnl_epo_time.uSecond_start = %ld, mnl_epo_time.uSecond_expire = %ld", mnl_epo_time.uSecond_start,
            mnl_epo_time.uSecond_expire);

        /*read time and sent to HAL*/
        time[0] = mnl_epo_time.uSecond_start;
        time[1] = mnl_epo_time.uSecond_expire;
        MND_MSG("time[0] = %ld, time[1] = %ld\n", time[0], time[1]);

        /*read time and send to HAL*/
        if (ret == sizeof(time)) {
            char buff[1024] = {0};
            int offset = 0;
            // To debug
            buff_put_int(MNL_CMD_READ_EPO_TIME_DONE, buff, &offset);
            buff_put_int(time[0], buff, &offset);
            buff_put_int(time[1], buff, &offset);
            if (-1 == mnld_sendto_hal(mnld2hal_fd, MTK_MNLD2HAL, buff, sizeof(buff))) {
                MND_ERR("Send EPO time to HAL failed\n");
                return MTK_GPS_ERROR;
            } else {
                MND_MSG("Send EPO time to HAL successfully\n");
                return MTK_GPS_SUCCESS;
            }
        } else {
            MND_ERR("Read time failed\n");
            return -1;
        }
    }
    return ret;
}
static int update_quarter_epo_file(void);
static int update_quarter_epo_file(void) {
    int ret = 0;

    if (MTK_GPS_ERROR == mtk_agps_agent_qepo_file_update()) {
        char buff[1024] = {0};
        int offset = 0;

        buff_put_int(MNL_CMD_UPDATE_QEPO_FILE_FAIL, buff, &offset);
        if (-1 == mnld_sendto_hal(mnld2hal_fd, MTK_MNLD2HAL, buff, sizeof(buff))) {
            MND_ERR("Send to HAL failed, %s\n", strerror(errno));
        } else {
            MND_MSG("Send to HAL successfully\n");
        }
        MND_MSG("QEPO file updates fail\n");
        ret = MTK_GPS_ERROR;
    } else {
        char buff[1024] = {0};
        int offset = 0;
        buff_put_int(MNL_CMD_UPDATE_QEPO_FILE_DONE, buff, &offset);
        if (-1 == mnld_sendto_hal(mnld2hal_fd, MTK_MNLD2HAL, buff, sizeof(buff))) {
            MND_ERR("Send to HAL failed, %s\n", strerror(errno));
        } else {
            MND_MSG("Send to HAL successfully\n");
        }
        MND_MSG("QEPO file updates ok\n");
        ret = MTK_GPS_SUCCESS;
    }

    return ret;
}

int update_epo_file() {
    MNLD_DATA_T *obj = &mnld_data;
    int ret = 0;

    if (MTK_GPS_ERROR == mtk_agps_agent_epo_file_update()) {
        char buff[1024] = {0};
        int offset = 0;
        buff_put_int(MNL_CMD_UPDATE_EPO_FILE_FAIL, buff, &offset);
        if (-1 == mnld_sendto_hal(mnld2hal_fd, MTK_MNLD2HAL, buff, sizeof(buff))) {
            MND_ERR("Send to HAL failed, %s\n", strerror(errno));
            ret = -1;
        } else {
            MND_MSG("Send to HAL successfully\n");
        }
        MND_MSG("EPO file updates fail\n");
    } else {
        char buff[1024] = {0};
        int offset = 0;
        buff_put_int(MNL_CMD_UPDATE_EPO_FILE_DONE, buff, &offset);
        if (-1 == mnld_sendto_hal(mnld2hal_fd, MTK_MNLD2HAL, buff, sizeof(buff))) {
            MND_ERR("Send to HAL failed, %s\n", strerror(errno));
            ret = -1;
        } else {
            MND_MSG("Send to HAL successfully\n");
        }
        MND_MSG("EPO file updates ok\n");
    }

    return ret;
}
/*****************************************************************************/
int string_to_array(const char* string, char* array[]) {
    char split[] = ",";
    char *token = NULL;
    int offset = 0;
    token = strtok(string, split);
    while (token != NULL) {
        array[offset] = token;
        offset++;
        token = strtok(NULL, split);
    }
    return 0;
}
/*****************************************************************************/

#ifdef MPE_ENABLE

/*****************************************************************************/
int mpe_socket_mnl() {
    int sockfd;
    struct sockaddr_un soc_addr;
    socklen_t addr_len;

    sockfd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        MNL_ERR("socket failed reason=[%s]\n", strerror(errno));
        return -1;
    }

    strcpy(soc_addr.sun_path, MPEMNL_UDP_SRV_PATH);
    soc_addr.sun_family = AF_LOCAL;
    addr_len = (offsetof(struct sockaddr_un, sun_path) + strlen(soc_addr.sun_path) + 1);

    unlink(soc_addr.sun_path);
    if (bind(sockfd, (struct sockaddr *)&soc_addr, addr_len) < 0) {
        MNL_ERR("bind failed path=[%s] reason=[%s]\n", MPEMNL_UDP_SRV_PATH, strerror(errno));
        close(sockfd);
        return -1;
    }
    if (chmod(MPEMNL_UDP_SRV_PATH, 0660) < 0)
        MNL_ERR("chmod error: %s", strerror(errno));
    if (chown(MPEMNL_UDP_SRV_PATH, -1, AID_INET))
        MNL_ERR("chown error: %s", strerror(errno));
    return sockfd;
}

int mnl_sendto_mpe(int sockfd, void* dest, char* buf, int size) {
    // dest: MTK_MNLD2HAL
    int ret = 0;
    int len = 0;
    struct sockaddr_un soc_addr;
    socklen_t addr_len;
    int retry = 10;

    strcpy(soc_addr.sun_path, dest);
    soc_addr.sun_family = AF_UNIX;
    addr_len = (offsetof(struct sockaddr_un, sun_path) + strlen(soc_addr.sun_path) + 1);

    MNL_MSG("mnld2hal fd: %d\n", sockfd);
    while ((len = sendto(sockfd, buf, size, 0,
        (const struct sockaddr *)&soc_addr, (socklen_t)addr_len)) == -1) {
        if (errno == EINTR) continue;
        if (errno == EAGAIN) {
            if (retry-- > 0) {
                usleep(100 * 1000);
                continue;
            }
        }
        MNL_ERR("[mnld2hal] ERR: sendto dest=[%s] len=%d reason =[%s]\n",
            (char *)dest, size, strerror(errno));
        ret = -1;
        break;
    }
    return ret;
}
static int mpemnl_handler(int sock) {  /*sent from mpe*/
    UINT8 buff[128] = {0};
    int len = 0;
    len = safe_read(sock, buff, sizeof(buff));

    MNL_MSG("len=%d buff:%s\n", len, buff);

    if ((len > 0) && (!mnld_monitor.terminated)) {
       mtk_gps_mnl_get_sensor_info(buff, len);
    }
    MNL_MSG("mpemnl_handler end\n");
    return 0;
}

void mtk_gps_mnl_set_sensor_info(UINT8 *msg, int len) {
    UINT8 buff[128] = {0};

    MNL_MSG("len=%d msg:%s\n", len, msg);
    if ((mnl2mpe_fd != C_INVALID_FD) && (len > 0) && (msg != NULL)) {
        memcpy(&buff[0], msg, len);
        if (-1 == mnl_sendto_mpe(mnl2mpe_fd, MPEMNL_UDP_CLI_PATH, buff, len)) {
            MNL_ERR("[MNL2MPE]Send to MPE failed, %s\n", strerror(errno));
            return MTK_GPS_ERROR;
        } else {
            MNL_MSG("[MNL2MPE]Send to MPE successfully\n");
        }
    }
}

int mtk_gps_mnl_trigger_mpe(void) {
    UINT16 mpe_len;
    UINT8 buff[128] = {0};
    int ret = MTK_GPS_ERROR;

    if (isFirstRecv) {
       mtk_gps_mnl_inject_adr(adr_flag, adr_valid_flag);
    }

    MNL_MSG("mtk_gps_mnl_trigger_mpe\n");
    mpe_len = mtk_gps_set_mpe_info(buff);
    MNL_MSG("mpemsg len=%d\n", mpe_len);
    if ((mnl2mpe_fd != C_INVALID_FD) && (mpe_len > 0)) {
        if (-1 == mnl_sendto_mpe(mnl2mpe_fd, MPEMNL_UDP_CLI_PATH, buff, mpe_len)) {
            MNL_ERR("[MNL2MPE]Send to MPE failed, %s\n", strerror(errno));
        } else {
            ret = MTK_GPS_SUCCESS;
            MNL_MSG("[MNL2MPE]Send to MPE successfully\n");
        }
    }
    return ret;
}

void *thread_mnlmpe(void * arg) {
    int client_to_server = 0;
    char *myfifo = "/data/mnlmpe_server_fifo";

    char buf[64] = {0};
    int read_ret = 0, file_flag = 0;

    pthread_detach(pthread_self());
    isFirstRecv = 0;
    g_ThreadExitmnlmpe = 0;

    if (!g_ThreadExitmnlmpe) {
       /* create the FIFO (named pipe) */
       MNL_MSG("thread 1\n");
       file_flag = mkfifo(myfifo, 0666);

       /* open, read, and display the message from the FIFO */
       MNL_MSG("thread 2, file_flag =%d\n", file_flag);

      if (file_flag == -1) {
          MNL_MSG("file_flag error =%s\n",  strerror(errno));
      }
      client_to_server = open(myfifo, O_RDONLY);
      MNL_MSG("client_to_server =%d\n", client_to_server);

#if 0
   client_to_server = open(myfifo, O_RDONLY|O_NONBLOCK);
   if (client_to_server == -1) {
      MNL_MSG("client_to_server failed, %s \n", strerror(errno));
   }
   file_flag = fcntl(client_to_server, F_GETFL);
   if (file_flag != -1) {
        MNL_MSG("file_flag = %d \n", file_flag);
        file_flag &= ~O_NONBLOCK;
        fcntl(client_to_server, F_SETFL, file_flag);
        MNL_MSG("file_flag new = %d \n", file_flag);
   }
#endif

        MNL_MSG("Server ON.\n");

      while (!g_ThreadExitmnlmpe) {
        if (client_to_server != -1) {
            read_ret = read(client_to_server, buf, 64);
        }

        if ((read_ret != -1)) {
          if ((buf[0] == 0x65) && (buf[1] == 0x78) && (buf[2] == 0x69) && (buf[3] == 0x74)) {
             MNL_MSG("Server OFF.\n");
             g_ThreadExitmnlmpe = 1;
             break;
          } else if (buf[0] != 0) {
             sscanf(buf, "%lf %d %d", &adr_time, &adr_flag, &adr_valid_flag);
             MNL_MSG("Received mpe: %lf, %d, valid=%d\n", adr_time, adr_flag, adr_valid_flag);
             isFirstRecv = 1;
          } else {
            // MNL_MSG("read_ret err 1 \n");
            usleep(1000);
          }
      } else {
          // MNL_MSG("read_ret err 2 \n");
          usleep(1000);
      }

          /* clean buf from any data */
          memset(buf, 0, sizeof(buf));
      }
      MNL_MSG("Exit thread_mnlmpe \n");

      if (client_to_server != -1) {
          MNL_MSG("close client_to_server =%d\n", client_to_server);
          close(client_to_server);
      }

      unlink(myfifo);
      MNL_MSG("close fifo file success \n");
   }
      pthread_exit(NULL);
}

void check_pipe_handler(int signum) {
   if (signum == SIGPIPE) {
        pipe_close = 1;
        MNL_MSG("SIGPIPE recv \n");
   }
}

int linux_mpe_uninit(void) {
    int err;
    int client_to_server;
    char *myfifo = "/data/mnlmpe_server_fifo";
    char str[64];

    mnlmpe_thread[0].thread_id = MNL_THREAD_UNKNOWN;
    mnlmpe_thread[0].thread_handle = C_INVALID_TID;
    mnlmpe_thread[0].thread_exit = NULL;
    mnlmpe_thread[0].thread_active = NULL;
    client_to_server = open(myfifo, O_WRONLY);
    if (client_to_server != -1) {
        if (!pipe_close) {
            sprintf(str, "exit");
            write(client_to_server, str, strlen(str));
            MNL_MSG("send pipe trigger mnlmpe_thread close\n");
        }
    }
    usleep(2000);
    g_ThreadExitmnlmpe = 1;
    isFirstRecv = 0;
    MNL_MSG("close mpemnl thread ok \n");
    if (client_to_server != -1) {
        close(client_to_server);
    }
    return MTK_GPS_SUCCESS;
}
#endif

static int socket_handler(int sock) {  // from HAL
    int ret = 0;

    // for new command
    MNLD_DATA_T *obj = &mnld_data;
    char position[64] = {0};
    int retry = 0;
    // for new command

    char buff[1024] = {0};
    int offset = 0;
    safe_read(sock, buff, sizeof(buff) - 1);
    int info = buff_get_int(buff, &offset);

   MND_MSG("args from hal: %d\n", info);
#ifdef MPE_ENABLE
     if ((info == MNL_CMD_INIT) || (info == MNL_CMD_STOP) || (info == MNL_CMD_START)
        || (info == MNL_CMD_RESTART) || (info == MNL_CMD_RESTART_HOT) || (info == MNL_CMD_RESTART_WARM)
        || (info == MNL_CMD_RESTART_COLD) || (info == MNL_CMD_RESTART_FULL) || (info == MNL_CMD_RESTART_AGPS)) {
             isFirstRecv = 0;
        }
#endif
    switch(info) {
        case MNL_CMD_INIT: {
            if (mnld_monitor.terminated == 0)
            {
                MND_MSG("Before init we should stop GPS first\n");
                ttff = 0;
                write_gps_log(0);
                in_stop_proc = 1;
                mnl_alarm_stop_all();
                ret = stop_mnl_process();
                gps_user = GPS_USER_UNKNOWN;
                mnl2agps_gps_close();
            }
            if ((ret = mnl_set_pwrctl(GPS_PWRCTL_OFF))) /*default power off*/
                return ret;
            ret = mnl_set_state(GPS_STATE_INIT);
            mnl2agps_gps_init();
            break;
        }
        case MNL_CMD_CLEANUP: {
            if (((gps_user & GPS_USER_AGPS) != 0) && (mnld_monitor.terminated == 0)) {
                // May in NI session.
                MND_MSG("Disable GPS when AGPS session running, return");
                mnl2agps_gps_cleanup();
                break;
            }
            in_stop_proc = 1;
            mnl2agps_gps_cleanup();
            in_stop_proc = 0;
            break;
        }
        case MNL_CMD_START: {
            gps_user |= GPS_USER_APP;
            in_start_proc = 1;
#ifdef MNL_OFFLOAD_SUPPORT
            mtk_gps_ofl_set_user(gps_user);
#endif
            ret = start_mnl_process();
            if ((gps_user & GPS_USER_AGPS) != 0) {
                MND_MSG("AGPS has opened GPS driver\n");
                int ret = mnl2agps_gps_open(0);
            }
            time(&start_time);
            write_gps_log(1);
            usleep(1000000);
            break;
        }
        case MNL_CMD_STOP: {
            MND_MSG("Reveive MNL_CMD_STOP from HAL\n");
            ttff = 0;
            write_gps_log(0);
            in_stop_proc = 1;
            mnl_alarm_stop_all();
            ret = stop_mnl_process();
            gps_user -= GPS_USER_APP;
#ifdef MNL_OFFLOAD_SUPPORT
            mtk_gps_ofl_set_user(gps_user);
            if (mnld2hal_fd != -1) {
                char buff[1024] = {0};
                int offset = 0;
                buff_put_int(MNL_CMD_MNL_DIE, buff, &offset);
                if (-1 == mnld_sendto_hal(mnld2hal_fd, MTK_MNLD2HAL, buff, sizeof(buff)))
                    MND_ERR("Send to HAL failed, %s\n", strerror(errno));
                else
                    MND_MSG("Send to HAL successfully\n");
            }
#endif
            mnl2agps_gps_close();
            break;
        }
        case MNL_CMD_RESTART: {
            ret = restart_mnl_process(MNL_RESTART_FORCE);
            break;
        }
        case MNL_CMD_RESTART_HOT:
            MND_MSG("hot start\n");
            if (mnld_monitor.terminated == 1 || in_stop_proc == 1 || in_start_proc == 1) {
                MND_MSG("terminated = %d, in_stop_proc = %d, in_start_proc = %d\n",
                mnld_monitor.terminated, in_stop_proc, in_start_proc);
                MND_MSG("GPS stopped or in stop/start process");
                assist_data_bit_map = FLAG_HOT_START;
                ret = 0;
            } else {
                MTK_GPS_PARAM_RESTART restart = {MTK_GPS_START_HOT};
        gps_user |= GPS_USER_RESTART;
#ifdef MNL_OFFLOAD_SUPPORT
        mtk_gps_ofl_set_user(gps_user);
#endif
                if ((ret = mtk_gps_set_param (MTK_PARAM_CMD_RESTART, &restart)))
                    MND_ERR("GPS restart fail %d\n", ret);
        get_condition(&lock_for_sync[M_RESTART]);
        MND_MSG("Get HOT restart conidtion success");
        gps_user -= GPS_USER_RESTART;
#ifdef MNL_OFFLOAD_SUPPORT
        mtk_gps_ofl_set_user(gps_user);
#endif
            }
            mnl2agps_delete_aiding_data(FLAG_HOT_START);
            break;
        case MNL_CMD_RESTART_WARM:
            MND_MSG("warm start\n");
            if (mnld_monitor.terminated == 1 || in_stop_proc == 1 || in_start_proc == 1) {
                MND_MSG("terminated = %d, in_stop_proc = %d, in_start_proc = %d\n",
                mnld_monitor.terminated, in_stop_proc, in_start_proc);
                MND_MSG("GPS stopped or in stop/start process");
                assist_data_bit_map = FLAG_WARM_START;
                ret = 0;
            } else {
                MTK_GPS_PARAM_RESTART restart = {MTK_GPS_START_WARM};
        gps_user |= GPS_USER_RESTART;
#ifdef MNL_OFFLOAD_SUPPORT
                mtk_gps_ofl_set_user(gps_user);
#endif
                if ((ret = mtk_gps_set_param (MTK_PARAM_CMD_RESTART, &restart)))
                    MND_ERR("GPS restart fail %d\n", ret);
                get_condition(&lock_for_sync[M_RESTART]);
                MND_MSG("Get WARM restart conidtion success");
                gps_user -= GPS_USER_RESTART;
#ifdef MNL_OFFLOAD_SUPPORT
                mtk_gps_ofl_set_user(gps_user);
#endif
            }
            mnl2agps_delete_aiding_data(FLAG_WARM_START);
            break;
        case MNL_CMD_RESTART_COLD:
            MND_MSG("cold start\n");
            if (mnld_monitor.terminated == 1 || in_stop_proc == 1 || in_start_proc == 1) {
                MND_MSG("terminated = %d, in_stop_proc = %d, in_start_proc = %d\n",
                mnld_monitor.terminated, in_stop_proc, in_start_proc);
                MND_MSG("GPS stopped or in stop/start process");
                assist_data_bit_map = FLAG_COLD_START;
                ret = 0;
            } else {
                MTK_GPS_PARAM_RESTART restart = {MTK_GPS_START_COLD};
                gps_user |= GPS_USER_RESTART;
#ifdef MNL_OFFLOAD_SUPPORT
                mtk_gps_ofl_set_user(gps_user);
#endif
                if ((ret = mtk_gps_set_param (MTK_PARAM_CMD_RESTART, &restart)))
                    MND_ERR("GPS restart fail %d\n", ret);
                get_condition(&lock_for_sync[M_RESTART]);
                MND_MSG("Get COLD restart conidtion success");
                gps_user -= GPS_USER_RESTART;
#ifdef MNL_OFFLOAD_SUPPORT
                mtk_gps_ofl_set_user(gps_user);
#endif
            }
            mnl2agps_delete_aiding_data(FLAG_COLD_START);
            break;
        case MNL_CMD_RESTART_FULL:
            MND_MSG("full start\n");
            if (mnld_monitor.terminated == 1 || in_stop_proc == 1 || in_start_proc == 1) {
                MND_MSG("terminated = %d, in_stop_proc = %d, in_start_proc = %d\n",
                mnld_monitor.terminated, in_stop_proc, in_start_proc);
                MND_MSG("GPS stopped or in stop/start process");
                assist_data_bit_map = FLAG_FULL_START;
                ret = 0;
            } else {
                MTK_GPS_PARAM_RESTART restart = {MTK_GPS_START_FULL};
                gps_user |= GPS_USER_RESTART;
#ifdef MNL_OFFLOAD_SUPPORT
                mtk_gps_ofl_set_user(gps_user);
#endif
                if ((ret = mtk_gps_set_param (MTK_PARAM_CMD_RESTART, &restart)))
                    MND_ERR("GPS restart fail %d\n", ret);
                get_condition(&lock_for_sync[M_RESTART]);
                MND_MSG("Get FULL restart conidtion success");
                gps_user -= GPS_USER_RESTART;
#ifdef MNL_OFFLOAD_SUPPORT
                mtk_gps_ofl_set_user(gps_user);
#endif
            }
            mnl2agps_delete_aiding_data(FLAG_FULL_START);
            break;
        case MNL_CMD_RESTART_AGPS:
            MND_MSG("agps start\n");
            if (mnld_monitor.terminated == 1 || in_stop_proc == 1 || in_start_proc == 1) {
                MND_MSG("terminated = %d, in_stop_proc = %d, in_start_proc = %d\n",
                mnld_monitor.terminated, in_stop_proc, in_start_proc);
                MND_MSG("GPS stopped or in stop/start process");
                assist_data_bit_map = FLAG_AGPS_START;
                ret = 0;
            } else {
                MTK_GPS_PARAM_RESTART restart = {MTK_GPS_START_AGPS};
                gps_user |= GPS_USER_RESTART;
#ifdef MNL_OFFLOAD_SUPPORT
                mtk_gps_ofl_set_user(gps_user);
#endif
                if ((ret = mtk_gps_set_param (MTK_PARAM_CMD_RESTART, &restart)))
                    MND_ERR("GPS restart fail %d\n", ret);
                get_condition(&lock_for_sync[M_RESTART]);
                MND_MSG("Get AGPS restart conidtion success");
                gps_user -= GPS_USER_RESTART;
#ifdef MNL_OFFLOAD_SUPPORT
                mtk_gps_ofl_set_user(gps_user);
#endif
            }
            mnl2agps_delete_aiding_data(FLAG_AGPS_START);
            break;
        case MNL_CMD_READ_EPO_TIME: {
            // MNL_CMD_READ_EPO_TIME should not be used in AOSP EPO
            ret = get_epo_time();
            break;
        }
        case MNL_CMD_UPDATE_EPO_FILE:{
            ret = update_epo_file();
            break;
        }
        case MNL_CMD_GPS_LOG_WRITE: {
            buff_get_string(position, buff, &offset);
            write_gps_location(position);
            break;
        }
        case MNL_CMD_GPS_QUARTER_EPO_UPDATE: {
            int qdownload_status = buff_get_int(buff, &offset);
            ret = update_quarter_epo_file();
            if (ret == MTK_GPS_ERROR) {
                qdownload_status = MTK_QEPO_RSP_UPDATE_FAIL;
            } else {
                qdownload_status = MTK_QEPO_RSP_UPDATE_SUCCESS;
            }

            MND_MSG("qdownload_status = %d, 0:success", qdownload_status);
            if ((mtk_agps_set_param (MTK_PARAM_QEPO_DOWNLOAD_RESPONSE,
            &qdownload_status, MTK_MOD_DISPATCHER, MTK_MOD_AGENT))) {
                MND_ERR("GPS QEPO update fail\n");
            }
            break;
        }
        case MNL_CMD_GPS_QUARTER_EPO_STATUS: {
            int qdownload_status = buff_get_int(buff, &offset);
            if (qdownload_status > 0) {
                qdownload_status = MTK_QEPO_RSP_DOWNLOAD_FAIL;
            }
            MND_MSG("qdownload_status = %d, 0:success", qdownload_status);
            if ((ret = mtk_agps_set_param (MTK_PARAM_QEPO_DOWNLOAD_RESPONSE,
            &qdownload_status, MTK_MOD_DISPATCHER, MTK_MOD_AGENT))) {
                MND_ERR("GPS QEPO update status fail\n");
            }
            break;
        }
        case MNL_CMD_GPS_INJECT_TIME: {
            if (mnld_monitor.terminated == 1 || in_stop_proc == 1) {
                MND_MSG("MNL is stopped, MNL_CMD_INJECT_NW_LOCATION return");
                break;
            }
            ntp_context  ntp_inject;

            memset(&ntp_inject, 0, sizeof(ntp_context));
            buff_get_struct(&ntp_inject, sizeof(ntp_context), buff, &offset);
            MND_MSG("MNL_CMD_GPS_INJECT_TIME time= %lld, timeReference = %lld, uncertainty =%d\n",
                ntp_inject.time, ntp_inject.timeReference, ntp_inject.uncertainty);
            mtk_gps_inject_ntp_time(&ntp_inject);
            break;
        }
        case MNL_CMD_GPS_INJECT_LOCATION: {
            if (mnld_monitor.terminated == 1 || in_stop_proc == 1) {
                MND_MSG("MNL is stopped, MNL_CMD_GPS_INJECT_LOCATION return");
                break;
            }
            nlp_context nlp_inject;
            memset(&nlp_inject, 0, sizeof(nlp_context));
            buff_get_struct(&nlp_inject, sizeof(nlp_context), buff, &offset);
            MND_MSG("MNL_CMD_GPS_INJECT_LOCATION lati = %f, longi = %f, accuracy = %f, \
                ts.tv_sec = %lld, ts.tv_nsec = %lld\n",
                nlp_inject.latitude, nlp_inject.longitude, nlp_inject.accuracy,
                nlp_inject.ts.tv_sec, nlp_inject.ts.tv_nsec);
            mtk_gps_inject_nlp_location(&nlp_inject);

            ret = mnl2agps_location_sync(nlp_inject.latitude, nlp_inject.longitude, (int)nlp_inject.accuracy);
            MND_MSG("MNL_CMD_GPS_INJECT_LOCATION, ret = %d", ret);
            if (0 == ret) {
                MND_MSG("gps location inject, mnl2agps_location_sync success");
            }
            break;
        }
        case MNL_CMD_GPS_NLP_LOCATION_REQ: {
            mnl2nlp_request_nlp();
            break;
        }
        case MNL_CMD_INJECT_NW_LOCATION:{
            MND_MSG("MNL_CMD_INJECT_NW_LOCATION");
            if (mnld_monitor.terminated == 1 || in_stop_proc == 1) {
                MND_MSG("MNL is stopped, return");
                break;
            }
            NetworkLocation nw_location;
            MTK_GPS_REF_LOCATION RefLcation;
            MTK_GPS_REF_LOCATION_CELLID cell_id_loc = {0, 0, 0, 0, 0};
            memset(&nw_location, 0, sizeof(NetworkLocation));
            memset(&RefLcation, 0, sizeof(MTK_GPS_REF_LOCATION));
            buff_get_struct(&nw_location, sizeof(NetworkLocation), buff, &offset);
            RefLcation.type = -1;
            RefLcation.u.cellID = cell_id_loc;
            ret = mtk_gps_set_wifi_location_aiding(&RefLcation, nw_location.latitude, nw_location.longitude, nw_location.accuracy);
            break;
        }
        case MNL_CMD_GPS_LOCATION_SYNC_TO_AGPS: {
            char* pos[3] = {NULL};
            int i;
            double latitude, longitude;
            float accuracy;

            buff_get_string(position, buff, &offset);
            i = string_to_array(position, pos);
            if ((NULL == pos[0]) || (NULL == pos[1]) || (NULL == pos[2])) {
                MND_ERR("gps position invalid, ignore\n");
                break;
            }
            MND_MSG("gps position = %s, %s, %s, %d\n", pos[0], pos[1], pos[2], i);

            latitude = atof(pos[0]);
            longitude = atof(pos[1]);
            accuracy = atof(pos[2]);
            ret = mnl2agps_location_sync(latitude, longitude, (int)accuracy);
            if (0 == ret) {
                MND_MSG("gps fix, mnl2agps_location_sync success");
            }
            break;
        }
        case MNL_AGPS_TYPE_DATA_CONN_OPEN:{
            MND_MSG("MNL_AGPS_TYPE_DATA_CONN_OPEN");
            char apn[256] = {0};
            buff_get_string(apn, buff, &offset);
            mnl2agps_data_conn_open(apn);
            break;
        }
        case MNL_AGPS_TYPE_DATA_CONN_CLOSED:{
            MND_MSG("MNL_AGPS_TYPE_DATA_CONN_CLOSED");
            mnl2agps_data_conn_closed();
            break;
        }
        case MNL_AGPS_TYPE_DATA_CONN_FAILED:{
            MND_MSG("MNL_AGPS_TYPE_DATA_CONN_FAILED");
            mnl2agps_data_conn_failed();
            break;
        }
        case MNL_AGPS_TYPE_SET_SERVER:{
            MND_MSG("MNL_AGPS_TYPE_SET_SERVER");
            char hostname[128] = {0};
            AGpsType type;
            int port;
            type = buff_get_int(buff, &offset);
            buff_get_string(hostname, buff, &offset);
            port = buff_get_int(buff, &offset);
            mnl2agps_set_server(type, hostname, port);
            break;
        }
        case MNL_AGPS_TYPE_NI_RESPOND:{
            int id = buff_get_int(buff, &offset);
            int respond = buff_get_int(buff, &offset);
            mnl2agps_ni_respond(id, respond);
            break;
        }
        case MNL_AGPS_TYPE_SET_REF_LOC:{
            AGpsRefLocation ref_location;

            // memset(&cellID, 0, sizeof(AGpsRefLocationCellID));
            int size;
            buff_get_struct(&ref_location, sizeof(AGpsRefLocation), buff, &offset);
            size = buff_get_int(buff, &offset);
            if (ref_location.type == AGPS_REF_LOCATION_TYPE_GSM_CELLID ||
                ref_location.type == AGPS_REF_LOCATION_TYPE_UMTS_CELLID) {
                    MND_MSG("ref_location is cellID");
                    AGpsRefLocationCellID* cellID = &ref_location.u.cellID;
                    MND_MSG("type: %d, mcc: %d, mnc: %d, lac: %d, cid: %d\n", ref_location.type, cellID->mcc, cellID->mnc, cellID->lac, cellID->cid);
                    mnl2agps_set_ref_loc(ref_location.type, cellID->mcc, cellID->mnc, cellID->lac, cellID->cid);
            } else if (ref_location.type == AGPS_REG_LOCATION_TYPE_MAC) {
                MND_MSG("ref_location is MAC");
            }
            break;
        }
        case MNL_AGPS_TYPE_SET_SET_ID:{
            MND_MSG("MNL_AGPS_TYPE_SET_SET_ID");
            int type;
            char setid[128] = {0};
            type = buff_get_int(buff, &offset);
            buff_get_string(setid, buff, &offset);
            mnl2agps_set_set_id(type, setid);
            break;
        }
        case MNL_AGPS_TYPE_NI_MESSAGE:{
            char msg[512] = {0};
            int len;
            len = buff_get_binary(msg, buff, &offset);
            mnl2agps_ni_message(msg, len);
            break;
        }
        case MNL_AGPS_TYPE_UPDATE_NETWORK_STATE:{
            int connected;
            int type;
            int roaming;
            char extra_info[256] = {0};
            connected = buff_get_int(buff, &offset);
            type = buff_get_int(buff, &offset);
            roaming = buff_get_int(buff, &offset);
            buff_get_string(extra_info, buff, &offset);
            mnl2agps_update_network_state(connected, type, roaming, extra_info);
            break;
        }
        case MNL_AGPS_TYPE_UPDATE_NETWORK_AVAILABILITY:{
            int available;
            char apn[128] = {0};
            available = buff_get_int(buff, &offset);
            buff_get_string(apn, buff, &offset);
            mnl2agps_update_network_availability(available, apn);
            break;
         }
        case MNL_AGPS_TYPE_DATA_CONN_OPEN_IP_TYPE: {
            char apn[128] = {0};
            int ip_type;
            buff_get_string(apn, buff, &offset);
            ip_type = buff_get_int(buff, &offset);
            mnl2agps_data_conn_open_ip_type(apn, ip_type);
            break;
        }
        case MNL_AGPS_TYPE_INSTALL_CERTIFICATES: {
            int i;
            int length = buff_get_int(buff, &offset);
            DerEncodedCertificate certificates[length];
            buff_get_struct(certificates, length, buff, &offset);
            for (i = 0; i < length; i++) {
                mnl2agps_install_certificates(i, length, &certificates[i], sizeof(certificates[i]));
            }
            break;
        }
        case MNL_AGPS_TYPE_REVOKE_CERTIFICATES: {
            int length = buff_get_int(buff, &offset);
            Sha1CertificateFingerprint fingerprints;
            buff_get_struct(&fingerprints, length, buff, &offset);
            mnl2agps_revoke_certificates(&fingerprints, length);
            break;
        }
        case MNL_CMD_GPSMEASUREMENT_INIT: {
            gps_measurement_state = GPS_MEASUREMENT_INIT;
            MND_MSG("MNL_CMD_GPSMEASUREMENT_INIT");
            break;
        }
        case MNL_CMD_GPSMEASUREMENT_CLOSE: {
            gps_measurement_state = GPS_MEASUREMENT_CLOSE;
            MND_MSG("MNL_CMD_GPSMEASUREMENT_CLOSE");
            break;
        }
        case MNL_CMD_GPSNAVIGATION_INIT: {
            gps_navigation_state = GPS_NAVIGATION_INIT;
            MND_MSG("MNL_CMD_GPSNAVIGATION_INIT");
            break;
        }
        case MNL_CMD_GPSNAVIGATION_CLOSE: {
            gps_navigation_state = GPS_NAVIGATION_CLOSE;
            MND_MSG("MNL_CMD_GPSNAVIGATION_CLOSE");
            break;
        }
        default: {
            MND_ERR("unknown command: 0x%2X\n", info);
            errno = -EINVAL;
            return errno;
            break;
        }
    }

    return ret;
}
/*****************************************************************************/
void linux_signal_handler(int signo)
{
    int ret = 0;
    MNLD_DATA_T *obj = &mnld_data;

    pthread_t self = pthread_self();
    if (signo == SIGTERM)
    {
        if ((gps_user & GPS_USER_APP) != 0) {
            MND_MSG("Normal mode,sdcard storage send SIGTERM to mnld");

            gps_debuglog_state = GPS_DEBUGLOG_DISABLE;
            if (obj->state == GPS_STATE_START) {
                ret = mtk_gps_set_debug_type(gps_debuglog_state);
                if (MTK_GPS_ERROR== ret) {
                    MND_MSG("sdcard storage send SIGTERM to mnld, stop gpsdebuglog, mtk_gps_set_debug_type fail");
                }
            }
        }
        else {
            MND_MSG("Meta or factory or adb shell mode done");

            // unlink(GPS_PROPERTY);
            stop_mnl_process();
            exit_meta_factory = 1;
        }
    }

    MNL_MSG("Signal handler of %.8x -> %s\n", (unsigned int)self, sys_siglist[signo]);
}
/*****************************************************************************/
int linux_setup_signal_handler(void)
{
    struct sigaction actions;
    int err;
    /*the signal handler is MUST, otherwise, the thread will not be killed*/
    memset(&actions, 0, sizeof(actions));
    sigemptyset(&actions.sa_mask);
    actions.sa_flags = 0;
    actions.sa_handler = linux_signal_handler;
    if ((err = sigaction(SIGTERM, &actions, NULL))) {
        MND_MSG("register signal hanlder for SIGTERM: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}
/*****************************************************************************/
static int sigrcv_handler(int fd)   /*sent from signal handler or internal event*/
{
    int err;
    char cmd = MNL_CMD_UNKNOWN;
    do {
        err = read(fd, &cmd, sizeof(cmd));
    } while (err < 0 && errno == EINTR);
    if (err == 0) {
        MND_ERR("EOF"); /*it should not happen*/
        return 0;
    } else if (err != sizeof(cmd)) {
        MND_ERR("fails: %d %d(%s)\n", err, errno, strerror(errno));
        return -1;
    }

    MND_MSG("args from mnld: %d\n", cmd);

    if (cmd == MNL_CMD_ACTIVE) {
        if ((err = mnl_set_active()))
            return err;
        return 0;
    } else if (cmd == MNL_CMD_TIMEOUT_INIT) {
        mnl2agps_mnl_reboot();
        return restart_mnl_process(MNL_RESTART_TIMEOUT_INIT);
    } else if (cmd == MNL_CMD_TIMEOUT_MONITOR) {
        mnl2agps_mnl_reboot();
        return restart_mnl_process(MNL_RESTART_TIMEOUT_MONITOR);
    } else if (cmd == MNL_CMD_TIMEOUT_WAKEUP) {
        mnl2agps_mnl_reboot();
        return restart_mnl_process(MNL_RESTART_TIMEOUT_WAKEUP);
    } else if (cmd == MNL_CMD_TIMEOUT_TTFF) {
        mnl2agps_mnl_reboot();
        return restart_mnl_process(MNL_RESTART_TIMEOUT_TTFF);
    } else if (cmd == MNL_CMD_SLEEP) {
        return 0;
    } else if (cmd == MNL_CMD_WAKEUP) {
        if ((err = mtk_gps_set_param(MTK_PARAM_CMD_WAKEUP, NULL)))
            MND_ERR("MNL wakeup = %d\n", err);
        if ((err = mnl_set_state(GPS_STATE_START)))
            return err;
        return 0;
    } else if (cmd == MNL_CMD_PWROFF) {
        MND_MSG("MNL_CMD_PWROFF--\n");
        if ((err = mnl_alarm_stop_all()))
            return err;
        if (err = mtk_gps_exit_proc())
            return err;
        mnl_set_pwrctl(GPS_PWRCTL_OFF);
        mnl_set_state(GPS_STATE_PWROFF);
        return 0;
    } else if (cmd == MNL_CMD_QUIT) {
        if (err = mnl_alarm_stop_watch()) {
            MND_MSG("mnl_alarm_stop_watch fail\n");
            return err;
        } else {
            MND_MSG("mnl_alarm_stop_watch success\n");
        }
        if (err = mtk_gps_exit_proc()) {
            MND_MSG("sigrcv_handler: err = %d", err);
            return err;
        }
        MND_MSG("Call mtk_gps_exit_proc() done--");
        mnl_set_pwrctl(GPS_PWRCTL_OFF);
        mnl_set_state(GPS_STATE_PWROFF);
        return 0;
    } else if (cmd == MNL_CMD_SLEPT) {
        /*MNL is slept, stop watch alarm*/
        return mnl_alarm_stop_watch();
    } else if (cmd == MNL_CMD_RCV_TTFF) {
        return mnl_set_alarm(MNL_ALARM_TTFF);
    } else {
        MND_MSG("unknown command: 0x%2X\n", cmd);
        errno = -EINVAL;
        return errno;
    }
}
int notify_client(int fd, char *msg)
{
    int i;
    MND_MSG("write %s to client fd %d ", msg, fd);
    write(fd, msg, strlen(msg));

    return 1;
}

int get_mtklog_path(char *logpath)
{
    char mtklogpath[GPS_DEBUG_LOG_FILE_NAME_MAX_LEN] = {0};
    // char raw_path[GPS_DEBUG_LOG_FILE_NAME_MAX_LEN], temp[GPS_DEBUG_LOG_FILE_NAME_MAX_LEN];
    char temp[GPS_DEBUG_LOG_FILE_NAME_MAX_LEN] = {0};
    int len;

    char* ptr;
    ptr = strchr(logpath, ',');
    if (ptr) {
        strcpy(temp, ptr + 1);
        MND_MSG("logpath for mtklogger socket msg: %s", temp);
    } else {
        MND_MSG("logpath for mtklogger socket msg has not ',': %s", temp);
        strcpy(logpath, "/data/misc/gps/");
        return 0;
    }

    len = strlen(temp);
    if (len != 0  && temp[len-1] != '/') {
        temp[len++] = '/';
        if (len < GPS_DEBUG_LOG_FILE_NAME_MAX_LEN) {
            temp[len] = '\0';
        }
    }
    if (len <= GPS_DEBUG_LOG_FILE_NAME_MAX_LEN - strlen(PATH_SUFFIX)) {
        sprintf(logpath, "%s"PATH_SUFFIX, temp);
        MND_MSG("get_mtklog_path:logpath is %s", logpath);
    }

    if (len <= GPS_DEBUG_LOG_FILE_NAME_MAX_LEN-7) {
        sprintf(mtklogpath, "%smtklog/", temp);
        if (0 != access(mtklogpath, F_OK)) {    // if mtklog dir is not exit, mkdir
             MND_MSG("access dir error(%s), Try to create dir", mtklogpath);
             if (mkdir(mtklogpath, 0775) == -1) {
                 strcpy(logpath, "/data/misc/gps/");  // if mkdir fail, set default path
                 MND_MSG("mkdir %s fail(%s), set default logpath(%s)", mtklogpath, strerror(errno), logpath);
             }
        }
    }
    return 1;
}

int mtklogger2mnl_handler(int fd)
{
    int ret = 0;
    MNLD_DATA_T *obj = &mnld_data;
    char buff[253] = {0};
    char ans[255] = {0};
    int offset = 0;

    #ifdef MPE_ENABLE
    char mpemsg[256] = {0};
    char path_tmp[GPS_DEBUG_LOG_FILE_NAME_MAX_LEN] = {0};
    UINT32 log_rec, rec_loc;
    int mpe_len;
    TOMPE_MSG msg_head;
    #endif
    safe_read(fd, buff, sizeof(buff));
    MND_MSG("recv %s from %d\n", buff, fd);

    sprintf(ans, "%s,1", buff);
    notify_client(fd, ans);  // response msg,1 to mtklogger
    MND_MSG("notify_client,ans = %s\n", ans);

    if (strstr(buff, "set_storage_path")) {
        // buff is "set_storage_path,storagePath"
        strcpy(storagePath, buff);
        get_mtklog_path(storagePath);
    } else if (!strncmp(buff, "deep_start,1", 12)) {
        gps_debuglog_state = GPS_DEBUGLOG_ENABLE;
        MND_MSG("gps_debuglog_state:%d", gps_debuglog_state);

        #ifdef MPE_ENABLE
        strcat(path_tmp, storagePath);
        #endif
        strcat(storagePath, "gpsdebug.log");
        strcpy(gps_debuglog_file_name, storagePath);

        if (obj->state == GPS_STATE_START) {
            ret = mtk_gps_set_debug_type(gps_debuglog_state);
            if (MTK_GPS_ERROR == ret) {
                MND_ERR("deep_start,1,mtk_gps_set_debug_type fail");
            } else {
                MNL_MSG("start gpsdbglog successfully\n");
                property_set("debug.gpsdbglog.enable", "1");
            }
            ret = mtk_gps_set_debug_file(gps_debuglog_file_name);
            if (MTK_GPS_ERROR == ret) {
                MND_ERR("deep_start,1,mtk_gps_set_debug_file fail");
            }
        }
#ifdef MPE_ENABLE

        if ((mnl2mpe_fd != C_INVALID_FD)) {
          msg_head.type = (UINT32)CMD_SEND_FROM_MNLD;
          msg_head.length = GPS_DEBUG_LOG_FILE_NAME_MAX_LEN + 2*sizeof(UINT32);
          log_rec = 1;
          rec_loc = 0;
          mpe_len = msg_head.length + sizeof(TOMPE_MSG);
          memcpy(mpemsg, &msg_head, sizeof(TOMPE_MSG));
          memcpy((mpemsg + sizeof(TOMPE_MSG)), &log_rec, sizeof(UINT32) );
          memcpy((mpemsg + sizeof(TOMPE_MSG)+ sizeof(UINT32)), &rec_loc, sizeof(UINT32) );
          memcpy((mpemsg + sizeof(TOMPE_MSG)+ 2*sizeof(UINT32)) , path_tmp, GPS_DEBUG_LOG_FILE_NAME_MAX_LEN );

          if (-1 == mnl_sendto_mpe(mnl2mpe_fd, MPEMNL_UDP_CLI_PATH, mpemsg, mpe_len)) {
              MNL_ERR("[MNL2MPE]Send to MPE failed, %s\n", strerror(errno));
              return MTK_GPS_ERROR;
          } else {
              MNL_MSG("[MNL2MPE]Send to MPE successfully\n");
          }
        }
#endif
    } else if (!strncmp(buff, "deep_start,2", 12)) {
        gps_debuglog_state = GPS_DEBUGLOG_ENABLE;
        MND_MSG("gps_debuglog_state:%d", gps_debuglog_state);

        strcpy(gps_debuglog_file_name, LOG_FILE);
        if (obj->state == GPS_STATE_START) {
            ret = mtk_gps_set_debug_type(gps_debuglog_state);
            if (MTK_GPS_ERROR == ret) {
                MND_ERR("deep_start,2,mtk_gps_set_debug_type fail");
            } else {
                MNL_MSG("start gpsdbglog successfully\n");
                property_set("debug.gpsdbglog.enable", "1");
            }
            ret = mtk_gps_set_debug_file(gps_debuglog_file_name);
            if (MTK_GPS_ERROR== ret) {
                MND_ERR("deep_start,2,mtk_gps_set_debug_file fail");
            }
        }

#ifdef MPE_ENABLE
        if ((mnl2mpe_fd != C_INVALID_FD)) {
            msg_head.type = (UINT32)CMD_SEND_FROM_MNLD;
            msg_head.length = GPS_DEBUG_LOG_FILE_NAME_MAX_LEN + 2*sizeof(UINT32);
            log_rec = 1;
            rec_loc = 1;
            strcpy(path_tmp, MPE_LOG_FILE);
            mpe_len = msg_head.length + sizeof(TOMPE_MSG);
            memcpy(mpemsg, &msg_head, sizeof(TOMPE_MSG));
            memcpy((mpemsg + sizeof(TOMPE_MSG)), &log_rec, sizeof(UINT32) );
            memcpy((mpemsg + sizeof(TOMPE_MSG)+ sizeof(UINT32)), &rec_loc, sizeof(UINT32) );
            memcpy((mpemsg + sizeof(TOMPE_MSG)+ 2*sizeof(UINT32)) , path_tmp, GPS_DEBUG_LOG_FILE_NAME_MAX_LEN );

            if (-1 == mnl_sendto_mpe(mnl2mpe_fd, MPEMNL_UDP_CLI_PATH, mpemsg, mpe_len)) {
                MNL_ERR("[MNL2MPE]Send to MPE failed, %s\n", strerror(errno));
                return MTK_GPS_ERROR;
            } else {
                MNL_MSG("[MNL2MPE]Send to MPE successfully\n");
            }
        }
#endif

    } else if (!strncmp(buff, "deep_stop", 9)) {
        gps_debuglog_state = GPS_DEBUGLOG_DISABLE;
        MND_MSG("gps_debuglog_state:%d", gps_debuglog_state);
        if (obj->state == GPS_STATE_START) {
            ret = mtk_gps_set_debug_type(gps_debuglog_state);
            if (MTK_GPS_ERROR== ret) {
                MND_ERR("deep_stop,mtk_gps_set_debug_type fail");
            } else {
                MNL_MSG("Stop gpsdbglog successfully\n");
                property_set("debug.gpsdbglog.enable", "0");
            }
        }

#ifdef MPE_ENABLE
        if ((mnl2mpe_fd != C_INVALID_FD)) {
            msg_head.type = (UINT32)CMD_SEND_FROM_MNLD;
            msg_head.length = GPS_DEBUG_LOG_FILE_NAME_MAX_LEN + 2*sizeof(UINT32);
            log_rec = 0;
            rec_loc = 0;
            mpe_len = msg_head.length + sizeof(TOMPE_MSG);
            memcpy(mpemsg, &msg_head, sizeof(TOMPE_MSG));
            memcpy((mpemsg + sizeof(TOMPE_MSG)), &log_rec, sizeof(UINT32));
            memcpy((mpemsg + sizeof(TOMPE_MSG)+ sizeof(UINT32)), &rec_loc, sizeof(UINT32));
            memcpy((mpemsg + sizeof(TOMPE_MSG)+ 2*sizeof(UINT32)) , storagePath, GPS_DEBUG_LOG_FILE_NAME_MAX_LEN);

            if (-1 == mnl_sendto_mpe(mnl2mpe_fd, MPEMNL_UDP_CLI_PATH, mpemsg, mpe_len)) {
                MNL_ERR("[MNL2MPE]Send to MPE failed, %s\n", strerror(errno));
                return MTK_GPS_ERROR;
            } else {
                MNL_MSG("[MNL2MPE]Send to MPE successfully\n");
            }
        }
#endif
    } else {
        MND_ERR("unknown message: %s\n", buff);
    }

    return ret;
}
#ifdef MNL_OFFLOAD_SUPPORT
// RETURN fd
int mnl_ofl_tst_fd_init() {
    int res;
    int fifo_fd = -1;
    res = mkfifo(MNL_OFL_TST_FIFO, 0666);

    if (res != 0 && errno != 17) {
        MND_ERR("[OFL]fifo create failed, name = %s", MNL_OFL_TST_FIFO);
        MND_ERR("[OFL]fifo create failed, res=%d, errno=%d(%s)", res, errno, strerror(errno));
        return -1;
    }

    MND_MSG("[OFL]fifo create success, name = %s", MNL_OFL_TST_FIFO);

    fifo_fd = open(MNL_OFL_TST_FIFO, O_RDWR);
    if (fifo_fd < 0) {
        MND_ERR("[OFL]fifo open failed, res=%d, errno=%d,%s", fifo_fd, errno, strerror(errno));
        unlink(MNL_OFL_TST_FIFO);
        return -1;
    }

    MND_MSG("[OFL]fifo fd = %d", fifo_fd);
    return fifo_fd;
}

int mnl_flp_to_mnld_fd_init() {
    int ofl_fd = -1;
    struct sockaddr_un cmd_local;

    if ((ofl_fd = socket(AF_LOCAL, SOCK_DGRAM, 0)) == -1) {
        MND_ERR("mnl2flp socket create failed\n");
        return ofl_fd;
    }

    unlink(MNL_FLP_TO_MNLD);
    memset(&cmd_local, 0, sizeof(cmd_local));
    cmd_local.sun_family = AF_LOCAL;
    strcpy(cmd_local.sun_path, MNL_FLP_TO_MNLD);

    if (bind(ofl_fd, (struct sockaddr *)&cmd_local, sizeof(cmd_local)) < 0) {
        MND_ERR("mnl2flp socket bind failed\n");
        ofl_fd = -1;
        return ofl_fd;
    }

    int res = chmod(MNL_FLP_TO_MNLD, 0777);  // S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP);
    MND_MSG("chmod res = %d, %s\n", res, strerror(errno));
    return ofl_fd;
}

int mnl_ofl_tst_fd_hdlr(int fd)  {
    int ret;
    char buff[1024] = {0};
    safe_read(fd, buff, sizeof(buff));

    MND_MSG("[OFL]data from ofl tst: %d\n", (int)buff[0]);
    switch (buff[0]) {
    case '1':
        if ((gps_user & GPS_USER_OFL_TEST)) {
            MND_MSG("[OFL]test already on!!!");
        }
        else {
            MND_MSG("[OFL]test on!");
            gps_user |= GPS_USER_OFL_TEST;
            mtk_gps_ofl_set_user(gps_user);
            start_mnl_process();
        }
        break;
    case '2':
        if (!(gps_user & GPS_USER_OFL_TEST)) {
            MND_MSG("[OFL]test already off!!!");
        }
        else {
            MND_MSG("[OFL]test off!");
            gps_user &= ~GPS_USER_OFL_TEST;
            mtk_gps_ofl_set_user(gps_user);
            stop_mnl_process();
        }
        break;

    case '3':
    {
        // Loopback test start
        char tmp_buf[1024];
        int i;

        MND_MSG("[OFL]lpbk start!");

        for (i=0; i < ofl_lpbk_tst_size; i++) {
            tmp_buf[i] = 0x80;
        }
        mtk_gps_ofl_send_flp_data((UINT8 *)&tmp_buf[0], ofl_lpbk_tst_size);
        ofl_lpbk_tst_start = 1;
        break;
    }
    case '4':
        // Loopback test stop
        MND_MSG("[OFL]lpbk stop!");
        ofl_lpbk_tst_start = 0;
        break;

    default:
        MND_MSG("[OFL] invalide opid :%d!", buff[0]);
        break;
    }

    return MTK_GPS_SUCCESS;
}

// Add for dbg usage
#define MAX_DUMP_CHAR 128
void mnl_flp_dump_buf(char *p, int len) {
    int i, r, n;
    char str[MAX_DUMP_CHAR] = {0};
    for (i = 0, n = 0; i < len ; i++) {
        r = snprintf(&str[n], MAX_DUMP_CHAR - n, "%3d,", p[i]);
        if (r + n >= MAX_DUMP_CHAR || r <= 0) {
            MND_MSG("[FLP2MNLD]data from flp: data=%s, error return", str);
            return;
    }

    n += r;
    if ((i % 8 == 7) || i + 1 == len) {
            MND_MSG("[FLP2MNLD]data from flp: data=%s", str);
            n = 0;
            str[0] = '0';
        }
    }
}

int mnl_flp_mnld_fd_hdlr(int fd) {
    int ret;
    int n_read;
    char buff[1024] = {0};
    MTK_FLP_OFFLOAD_MSG_T *prMsg;

    n_read = safe_read(fd, buff, sizeof(buff));
    prMsg = (MTK_FLP_OFFLOAD_MSG_T *)&buff[0];

    MND_MSG("[FLP2MNLD]data from flp: type=%d, len=%d, n_read = %d\n", prMsg->type, prMsg->length, n_read);
    mnl_flp_dump_buf(&buff[0], n_read);
    switch (prMsg -> type) {
    case FLP_MNL_CMD_DEINIT:
        if (!(gps_user & GPS_USER_FLP)) {
            MND_MSG("[FLP2MNLD]already off!!!");
        }
        else {
            MND_MSG("[FLP2MNLD]off!");
            gps_user &= ~GPS_USER_FLP;
            mtk_gps_ofl_set_user(gps_user);
            stop_mnl_process();
        }
        break;

    case FLP_MNL_CMD_INIT:
        if ((gps_user & GPS_USER_FLP)) {
            MND_MSG("[FLP2MNLD]already on!!!");
        }
        else {
            MND_MSG("[FLP2MNLD]on!");
            gps_user |= GPS_USER_FLP;
            mtk_gps_ofl_set_user(gps_user);
            start_mnl_process();
        }
        break;
    case FLP_MNL_CMD_LOOPBACK:
        if (mnld2flp_fd != -1) {
            if (-1 == mnld_sendto_hal(mnld2flp_fd, MNL_MNLD_TO_FLP, buff, sizeof(buff))) {
                MND_ERR("[FLP2MNLD]Send to HAL failed, %s\n", strerror(errno));
                ret = -1;
            }
            else {
                MND_MSG("[FLP2MNLD]Send to HAL successfully\n");
            }
        }
        break;
    case FLP_MNL_CMD_DATA_SEND:
        MND_MSG("[FLP2MNLD]to connsys: len=%d\n", prMsg->length);
        if (prMsg->length > 0 && prMsg->length <= OFFLOAD_PAYLOAD_LEN) {
            mtk_gps_ofl_send_flp_data((UINT8 *)&prMsg->data[0], prMsg->length);
        }
        else {
            MND_MSG("[FLP2MNLD]to connsys: length is invalid\n");
        }
        break;

    default:
        break;
    }

    return MTK_GPS_SUCCESS;
}

static void *thread_emi_download(void* arg) {
    int emi_fd;

    MND_MSG("EMI download thread start");

    emi_fd = open("/dev/gps_emi", O_RDWR);
    if (emi_fd == -1) {
        MND_MSG("open_port: Unable to open /dev/gps_emi\n");
        return NULL;
    } else {
        MND_MSG("open gps_emi successfully\n");
    }

    if (ioctl(emi_fd, IOCTL_MNL_IMAGE_FILE_TO_MEM, NULL) <= 0) {
        MND_MSG("set IOCTL_MNL_IMAGE_FILE_TO_MEM failed\n");
        return NULL;
    }
    usleep(2000000);
    // pthread_exit(NULL);
    MND_MSG("EMI download successful and thread exit done");
    return NULL;
}

#endif

/*****************************************************************************/
#define ERR_REMOTE_HANGUP   0x0F01
#define ERR_MNL_DIED        0x0F02
/*****************************************************************************/
static int process()
{
    struct epoll_event   ev, events[EPOLL_NUM];
    int                  ne, nevents;
    MNLD_DATA_T          *obj = &mnld_data;
    int eof = 0;
    int ret = 0;
    nevents = epoll_wait(obj->epoll_fd, events, EPOLL_NUM, -1);
    if (nevents < 0) {
        if (errno != EINTR)
            MND_ERR("epoll_wait() unexpected error: %s", strerror(errno));
        return -1;
    } else {
        MND_MSG("epoll_wait() received %d events", nevents);
    }

    for (ne = 0; ne < nevents; ne++) {
        if ((events[ne].events & (EPOLLERR|EPOLLHUP)) != 0) {
            MND_ERR("wait: (%d %d %d %d %d), event: 0x%X from %d", obj->cur_accept_socket,
                obj->sig_rcv_fd, obj->mnl_rcv_agps_fd, obj->mnl_rcv_mtklogger_fd,
                client_fd, events[ne].events, events[ne].data.fd);
            if (events[ne].data.fd == obj->cur_accept_socket) {
                /*current socket connection is hang-up, stop current session and
                wait another connection*/
                return ERR_REMOTE_HANGUP;
            }
            else if (events[ne].data.fd == obj->sig_rcv_fd) {
                MND_ERR("wait sig_rcv_fd err");
            }
            else if (events[ne].data.fd == obj->mnl_rcv_agps_fd) {
                MND_ERR("wait mnl_rcv_agps_fd err");
            }
            else if (events[ne].data.fd == obj->mnl_rcv_mtklogger_fd) {
                MND_ERR("wait mnl_rcv_mtklogger_fd EPOLLERR|EPOLLHUP");
                epoll_del(obj->epoll_fd, events[ne].data.fd);
            }
            else if (events[ne].data.fd == client_fd) {
                MND_ERR("wait client_fd EPOLLERR|EPOLLHUP");
                epoll_del(obj->epoll_fd, events[ne].data.fd);
                close(client_fd);
            } else {
                MND_ERR("EPOLLERR or EPOLLHUP after epoll_wait(): %d,epoll_del it\n", events[ne].data.fd);
                epoll_del(obj->epoll_fd, events[ne].data.fd);
            }

            return -1;
        }

        if ((events[ne].events & EPOLLIN) != 0) {
            int  fd = events[ne].data.fd;
            if (fd == obj->cur_accept_socket) {  // from HAL
                ret = socket_handler(fd);
                MND_MSG("socket_handler: ret = %d\n", ret);
                return ret;
            }
            if (fd == obj->sig_rcv_fd) {  // from MNLD
                ret = sigrcv_handler(fd);
                MND_MSG("sigrc_handler: ret = %d\n", ret);
                return ret;
            }
            if (fd == obj->mnl_rcv_agps_fd) {  // from AGPSD
                // return agpsrcv_handler(fd);
                MND_MSG("mnl2agps_handler: ret = %d\n", ret);
                return mnl2agps_handler(fd, &mnl_interface);
            }
            if (fd == obj->mnl_rcv_mtklogger_fd) {  // from mtklogger
                struct sockaddr addr;
                socklen_t alen = sizeof(addr);
                if (client_fd >= 0) {   // mtklogger exeception exit, then reconnect
                    MND_MSG("mtklogger old client_fd = %d, epoll_del it\n", client_fd);
                    epoll_del(obj->epoll_fd, client_fd);
                    close(client_fd);
                }
                client_fd = accept(fd, &addr, &alen);
                if (client_fd < 0) {
                    MND_ERR("accept failed, %s", strerror(errno));
                    return -1;
                }
                MND_MSG("mtklogger new client fd: %d", client_fd);

                if (epoll_add(obj->epoll_fd, client_fd)) {
                    ret = -1;
                    MND_ERR("epoll_add client_fd fail");
                } else {
                    MND_MSG("epoll_add client_fd success");
                }
                return ret;
            }
            if (fd == client_fd) {
                ret = mtklogger2mnl_handler(client_fd);
                MND_MSG("mtklogger2mnl_handler: ret = %d\n", ret);
                return ret;
            }
#ifdef MPE_ENABLE
            if (fd == obj->mpe_rcv_fd) {
                MND_MSG("mpemnl_handler: ret = %d\n", ret);
                return mpemnl_handler(fd);
            }
#endif
#ifdef MNL_OFFLOAD_SUPPORT
            if (fd == obj->ofl_tst_fd) {  // from OFFLOAD TEST
                ret = mnl_ofl_tst_fd_hdlr(fd);
                MND_MSG("[OFL]mnl_ofl_tst_fd_hdlr: ret = %d\n", ret);
                return ret;
            }
            if (fd == obj->flp_mnld_fd) {  // from OFFLOAD TEST
                ret = mnl_flp_mnld_fd_hdlr(fd);
                MND_MSG("[FLP2MNLD]mnl_flp_mnld_fd_hdlr: ret = %d\n", ret);
                return ret;
            }
#endif
        }
    }
    return -1;  /*nothing is hanlded*/
}

/*****************************************************************************/
int setup_socket_mtklogger()
{
    int ret;
    static int socket_fd = 0;

    socket_fd = socket_local_server("gpslogd", ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_STREAM);
    if (socket_fd < 0) {
        MND_ERR("create server fail(%s).", strerror(errno));
        return -1;
    }
    MND_MSG("socket_fd = %d", socket_fd);

    if ((ret = listen(socket_fd, 5)) < 0) {
        MND_ERR("listen socket fail(%s).", strerror(errno));
        close(socket_fd);
        return -1;
    }

    return socket_fd;
}
/*****************************************************************************/
int main (int argc, char** argv)
{
    int err;
    struct sockaddr_un remote;
    socklen_t remotelen;
    remotelen = sizeof(remote);

    int hal2mnld_fd, count;
    int asocket;
    char buf[C_CMD_BUF_SIZE];
    MNLD_DATA_T *obj = &mnld_data;

    MND_ERR("MNLD started\n");
    mnl2agps_mnl_reboot();
#ifdef MNL_OFFLOAD_SUPPORT
#ifdef MNL_SO
    MTK_GPS_SYS_FUNCTION_PTR_T*  mBEE_SYS_FP = &porting_layer_callback;
    if (mtk_gps_sys_function_register(mBEE_SYS_FP) != MTK_GPS_SUCCESS) {
        MND_ERR("register callback for mnl fail\n");
    }
    else {
        MND_MSG("register callback for mnl okey\n");
    }
#endif
    MND_MSG("MNLD can support offload/non-offload, ver = %s.\n", OFL_VER);
#endif
    if (!strncmp(argv[2], "meta", 4) || !strncmp(argv[2], "factory", 4) || !strncmp(argv[0], "test", 4)) {
        MND_MSG("Meta or factory or adb shell mode");
        strcpy(gps_debuglog_file_name,LOG_FILE);
        chip_detector();
#ifdef MNL_OFFLOAD_SUPPORT
        if (argc >= 4) {
            if (!strncmp(argv[3], "od", 2)) {
                MND_MSG("MNL is non-offload");
                mnld_cfg.OFFLOAD_enabled = 0;
            }
            else if (!strncmp(argv[3], "ot", 2)) {
                MND_MSG("MNL is offload, run on test mode");
                gps_user |= GPS_USER_OFL_TEST;
            }
            else {
                MND_MSG("MNL is offload, for meta/factory mode");
                gps_user |= GPS_USER_META;
            }
        }
        mtk_gps_ofl_set_user(gps_user);
#endif
        err = start_mnl_process();
        while (1) {
            usleep(100000);
            if (exit_meta_factory == 1) {
                MND_MSG("Meta or factory mode exit");
                exit_meta_factory = 0;
                exit(1);
            }
            MND_MSG("Meta or factory mode testing...");
        }
    } else {
        MND_MSG("Normal mode");
        // For receiving message from HAL
        hal2mnld_fd = hal_sock_mnld();
        if (hal2mnld_fd == -1) {
            MND_ERR("hal2mnld create socket fail: %d (%s)\n", errno, strerror(errno));
            exit(1);
        }

        // For sending message to HAL
        mnld2hal_fd = socket(PF_LOCAL, SOCK_DGRAM, 0);

        // For receiving message from AGPSD
        agps2mnld_fd = create_agps2mnl_fd();
#ifdef MPE_ENABLE
        mnl2mpe_fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
        mpe2mnl_fd = mpe_socket_mnl();
        // For sending message to AGPSD
        // send2agps();

        if (mpe2mnl_fd == -1) {
            MND_ERR("mpe2mnl_fd create socket fail: %d (%s)\n", errno, strerror(errno));
        } else {
            obj->mpe_rcv_fd = mpe2mnl_fd;
        }
#endif
        if (agps2mnld_fd == -1) {
            MND_ERR("agps2mnld create socket fail: %d (%s)\n", errno, strerror(errno));
        } else {
            obj->mnl_rcv_agps_fd = agps2mnld_fd;
        }

        // Create socket server, wait for MtkLogger to connect,receiving command from mtklogger
        mtklogger2mnld_fd = setup_socket_mtklogger();
        if (mtklogger2mnld_fd == -1) {
            MND_ERR("mtklogger2mnld_fd create socket fail: %d (%s)\n", errno, strerror(errno));
        } else {
            MND_MSG("mtklogger2mnld_fd created success");
            obj->mnl_rcv_mtklogger_fd = mtklogger2mnld_fd;
        }

        if (mnl_init()) {
            MND_ERR("mnl_init: %d (%s)\n", errno, strerror(errno));
            exit(1);
        }
#ifdef MNL_OFFLOAD_SUPPORT
        if (mnl_offload_is_enabled()) {
            int ret = -1;

            MND_MSG("[OFL]is enabled, ver = %s.\n", OFL_VER);
    #if 1
            ofl_tst_fd = mnl_ofl_tst_fd_init();
            if (ofl_tst_fd == -1) {
                obj->ofl_tst_fd = C_INVALID_FD;
            }
            else {
                obj->ofl_tst_fd = ofl_tst_fd;
            }
    #endif
            flp_mnld_fd = mnl_flp_to_mnld_fd_init();
            if (flp_mnld_fd == -1) {
                obj->flp_mnld_fd = -1;
            }
            else {
                obj->flp_mnld_fd = flp_mnld_fd;
            }
            mnld2flp_fd = socket(PF_LOCAL, SOCK_DGRAM, 0);

            // start thread to write data
            ret = pthread_create(&emi_download_thread, NULL, thread_emi_download, NULL);
            if (0 != ret) {
                MND_ERR("emi download thread create fail: %s\n", strerror(errno));
                exit(1);
            }
        }
        else {
            ofl_tst_fd = C_INVALID_FD;
            flp_mnld_fd = C_INVALID_FD;
            mnld2flp_fd = C_INVALID_FD;
            obj->ofl_tst_fd = C_INVALID_FD;
            obj->flp_mnld_fd = C_INVALID_FD;
            MND_MSG("[OFL]is not enabled.\n");
        }
#endif
        MND_MSG("listening..\n");
        chip_detector();
        while (1) {
            MND_MSG("main2\n");
            obj->cur_accept_socket = hal2mnld_fd;
            MND_MSG("main3\n");
            if (epoll_init()) {
              MND_ERR("epoll_init: %d (%s)\n", errno, strerror(errno));
              exit(1);
            }
            MND_MSG("new connection\n");
            for (;;) {
                err = process();
                if (err == ERR_REMOTE_HANGUP) {
                    MND_ERR("remote hangup (cleanup?), wait for new connection\n");
                    break;
                } else if (errno == EINTR) {
                    continue;
                } else if (err) {
                    MND_ERR("process data error: %d (%s)\n", errno, strerror(errno));
                }
            }
            MND_MSG("closing connection\n");
            //  close(s);
            if (hal2mnld_fd != C_INVALID_FD)
                close(hal2mnld_fd);
            if (mnld2hal_fd != C_INVALID_FD)
                close(mnld2hal_fd);
            if (agps2mnld_fd != C_INVALID_FD)
                close(agps2mnld_fd);
            if (mtklogger2mnld_fd != C_INVALID_FD)
                close(mtklogger2mnld_fd);
            if (client_fd != C_INVALID_FD)
                close(client_fd);
#ifdef MPE_ENABLE
            if (mnl2mpe_fd != C_INVALID_FD)
                close(mnl2mpe_fd);
#endif
#ifdef MNL_OFFLOAD_SUPPORT
            if (ofl_tst_fd != C_INVALID_FD) {
                close(ofl_tst_fd);
            }
            if (flp_mnld_fd != C_INVALID_FD) {
                close(flp_mnld_fd);
            }
            if (mnld2flp_fd != C_INVALID_FD) {
                close(mnld2flp_fd);
            }
#endif

            epoll_destroy();
        }
        gps_cnt = 0;
    }
    MND_MSG("exit mnld!\n");
    return 0;
}
