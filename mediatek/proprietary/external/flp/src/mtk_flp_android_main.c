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
 *  mtk_flp_android_main.c
 *
 * Project:
 * --------
 *
 * Description:
 * ------------
 *
 * Author:
 * -------
 *  LiangChi Huang, ext 25609, liangchi.huang@mediatek.com, 2012-08-14
 *
 *******************************************************************************/
/*****************************************************************************
 * Include
 *****************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <linux/wait.h>

#include <termios.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/inotify.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <errno.h>   /* Error number definitions */


#include <sys/stat.h>

//#include <cutils/log.h> // For Debug
#include <utils/Log.h> // For Debug

#include "mtk_flp_sys_type.h"
//#include "mtk_flp_sys_type_ext.h"
#include "mtk_flp_sys.h"
#include "mtk_flp_dc.h"
#include "mtk_flp.h"
//#include <android/log.h>


/*****************************************************************************
 * Define
 *****************************************************************************/
#ifdef SUPPORT_BLOCKING_READ_MECHANISM
#define THREAD_NUM     (4)    /*MAIN/DATA READ/SERVICE/SOCKET THREAD*/
#else
#define THREAD_NUM     (3)    /*MAIN/DATA READ/SERVICE THREAD*/
#endif

#define DEBUG_LOG

#ifdef DEBUG_LOG

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "FLP_THRD"
#endif

#define FLPE(...)		mtk_flp_sys_dbg(MTK_FLP_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define FLPD(...)		mtk_flp_sys_dbg(MTK_FLP_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define FLPW(...)		mtk_flp_sys_dbg(MTK_FLP_LOG_WARN, LOG_TAG, __VA_ARGS__)

#else
#define FLPD(...)
#define FLPW(...)
#define FLPE(...)
#endif

typedef enum MTK_FLP_THREAD_NUM{
	MTK_FLP_THREAD_HAL = 0,
	MTK_FLP_THREAD_KERNEL,
	MTK_FLP_THREAD_DC,
	MTK_FLP_THREAD_SOCKET,
	MTK_FLP_THREAD_WAKE_MONITOR,
	MTK_FLP_THREAD_END
} 	MTK_FLP_THREAD_NUM_e;

typedef struct MTK_FLP_THREAD
{
    int                     snd_fd;
    MTK_FLP_THREAD_NUM_e    thread_id;
    pthread_t               thread_handle;
    //void (*thread_body)(void *arg);
    int (*thread_exit)(struct MTK_FLP_THREAD *arg);
    int (*thread_active)(struct MTK_FLP_THREAD *arg);
} MTK_FLP_THREAD_T;

typedef void *(*thread_proc)(void *) ;

//AP sleep detection
#define EARLYSUSPEND_WAIT_FOR_FB_SLEEP "/sys/power/wait_for_fb_sleep"
#define EARLYSUSPEND_WAIT_FOR_FB_WAKE "/sys/power/wait_for_fb_wake"
static pthread_mutex_t earlysuspend_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t earlysuspend_cond = PTHREAD_COND_INITIALIZER;
static BOOL wait_for_earlysuspend;
static enum {
    EARLYSUSPEND_ON,
    EARLYSUSPEND_MEM,
};
UINT8 earlysuspend_state = EARLYSUSPEND_ON;


/*****************************************************************************
 * Function Prototype
 *****************************************************************************/
int mtk_flp_sys_init_android(void);
int mtk_flp_sys_deinit_android(void);
INT32 mtk_flp_threads_create(int threadId, thread_proc func);

int mtk_flp_threads_release(void);


VOID *mtk_flp_kernel_thread(VOID * arg);
VOID *mtk_flp_dc_thread(VOID * arg);
VOID *mtk_flp_socket_thread(VOID * arg);
VOID *mtk_flp_wake_monitor_thread(VOID *arg);

int mtk_flp_exit_thread_normal(MTK_FLP_THREAD_T *arg);
int mtk_flp_thread_active_notify(MTK_FLP_THREAD_T *arg);

extern VOID *mtk_flp_sys_interface_init (const CH* strDevPortName, const INT32 i4Baud);
extern VOID  mtk_flp_sys_interface_uninit (VOID *pLinkHandle);
extern INT32 mtk_flp_sys_mutex_deinitialize();


/*****************************************************************************
 * GLobal Variable
 *****************************************************************************/
static MTK_FLP_THREAD_T g_mtk_flp_thread[MTK_FLP_THREAD_END] = {
{C_INVALID_FD, MTK_FLP_THREAD_HAL,      C_INVALID_TID, mtk_flp_exit_thread_normal, mtk_flp_thread_active_notify},
{C_INVALID_FD, MTK_FLP_THREAD_KERNEL,   C_INVALID_TID, mtk_flp_exit_thread_normal, mtk_flp_thread_active_notify},
{C_INVALID_FD, MTK_FLP_THREAD_DC,       C_INVALID_TID, mtk_flp_exit_thread_normal, mtk_flp_thread_active_notify},
{C_INVALID_FD, MTK_FLP_THREAD_SOCKET,   C_INVALID_TID, mtk_flp_exit_thread_normal, mtk_flp_thread_active_notify},
{C_INVALID_FD, MTK_FLP_THREAD_WAKE_MONITOR,   C_INVALID_TID, mtk_flp_exit_thread_normal, mtk_flp_thread_active_notify},
};

// thread exit flag
static volatile int g_ThreadExitHalFunc = FALSE;
static volatile int g_ThreadExitKernelFunc = FALSE;
static volatile int g_ThreadExitDC = FALSE;
static volatile int g_ThreadExitSocket = FALSE;
static volatile int g_ThreadExitWakeMonitor = FALSE;


#define DSP_UART_IN_BUFFER_SIZE (1024)

// physical link handle
int gInterfaceHandle = C_INVALID_FD;

// socket
int gconn_fd_tmp = C_INVALID_SOCKET;
volatile int g_NfcTsetMode = 0;


/*****************************************************************************
 * Function
 *****************************************************************************/
int wait_for_fb_wake(void)
{
    int ino_len;
    int ino_fd;
    int ino_wd;
    char ino_buf[1024];
    char buf[100];
    FILE *fd;
    while(1) {
        ino_fd = inotify_init();
        if (ino_fd < 0) {
                FLPD("inotify_init error");
                return 1;
        }
        ino_wd = inotify_add_watch(ino_fd, "/sys/power/wake_lock", IN_MODIFY);
        ino_len = read(ino_fd, ino_buf, 1024);
        if (ino_len < 0) {
                perror("inotify read error");
                return 1;
        }
        (void) inotify_rm_watch(ino_fd, ino_wd);
        (void) close(ino_fd);

        fd = fopen("/sys/power/wake_lock" , "r");
        if (fd) {
            while (fscanf(fd, "%s", buf) != EOF) {
                if (strstr(buf, "Display") != NULL) {
                    fclose(fd);
                    return 0;
                }
            }
        }
        else {
            FLPD("fopen error\n");
            return 1;
        }
        fclose(fd);
    }
}

static int wait_for_fb_sleep(void)
{
    int ino_len;
    int ino_fd;
    int ino_wd;
    char ino_buf[1024];
    char buf[100];
    FILE *fd;
    while(1) {
        ino_fd = inotify_init();
        if (ino_fd < 0) {
                FLPD("inotify_init error");
                return 1;
        }
        ino_wd = inotify_add_watch(ino_fd, "/sys/power/wake_unlock", IN_MODIFY);
        ino_len = read(ino_fd, ino_buf, 1024);
        if (ino_len < 0) {
                perror("inotify read error");
                return 1;
        }
        (void) inotify_rm_watch(ino_fd, ino_wd);
        (void) close(ino_fd);

        fd = fopen("/sys/power/wake_unlock" , "r");
        if (fd) {
            while (fscanf(fd, "%s", buf) != EOF) {
                if (strstr(buf, "Display") != NULL) {
                    fclose(fd);
                    return 0;
                }
            }
        }
        else {
            FLPD("fopen error\n");
            return 1;
        }
        fclose(fd);
    }
}


int mtk_flp_exit_thread_normal(MTK_FLP_THREAD_T *arg)
{
    int err;
    void *ret;

    FLPD("mtk_flp_exit_thread_normal (taskid: %d)\n", arg->thread_id);

#if 0 // temp remove below code because main/service thread will be blocked on waiting signal
    if (!arg) {
        return MTK_FLP_ERROR;
    }

    if (arg->thread_id == MTK_FLP_THREAD_MAIN) {
        g_ThreadExitMainProcFunc = TRUE;
    } else if (arg->thread_id == MTK_FLP_THREAD_READ) {
        g_ThreadExitReadFunc = TRUE;
    } else if (arg->thread_id == MTK_FLP_THREAD_SERVICE) {
        g_ThreadExitService = TRUE;
    }
#ifdef SUPPORT_BLOCKING_READ_MECHANISM
    else if (arg->thread_id == MTK_FLP_THREAD_SOCKET) {
        g_ThreadExitSocket = TRUE;
    }
#endif

    err = pthread_join(arg->thread_handle, &ret);
    if (err) {
        FLPD("(%d)ThreadLeaveErr: %d\n", (arg->thread_id), err);
        return err;
    } else {
        FLPD("(%d)ThreadLeaveOK\n", (arg->thread_id));
    }
    arg->thread_handle = C_INVALID_TID;
#endif

    if(arg->thread_id == MTK_FLP_THREAD_SOCKET )
    {
        g_ThreadExitSocket = TRUE;

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

int mtk_flp_thread_active_notify(MTK_FLP_THREAD_T *arg)
{
    FLPD("mtk_flp_thread_active_notify,%d\n", arg->thread_id);

    if (!arg)
    {
       // MNL_MSG("fatal error: null pointer!!\n");
       // return -1;
    }
    if (arg->snd_fd != C_INVALID_FD)
    {
        //char buf[] = {MNL_CMD_ACTIVE};
        //return mnl_sig_send_cmd(arg->snd_fd, buf, sizeof(buf));
    }
    return 0;
}

int mtk_flp_sys_init_android(void)
{
    int result = MTK_FLP_SUCCESS;

    FLPD("mtk_flp_sys_init_android...\n");


    // Init task data structures
    mtk_flp_tasks_init();


    // Create Thread, HAL thread will run in main process. not created here
#ifndef K2_PLATFORM
    result = mtk_flp_threads_create(MTK_FLP_THREAD_KERNEL, mtk_flp_kernel_thread);
    if (result != MTK_FLP_SUCCESS)
    {
        FLPD("mtk_flp_threads_create, kernel thread ERR\n");
        return (result);
    }
#endif

    result = mtk_flp_threads_create(MTK_FLP_THREAD_DC, mtk_flp_dc_thread);
    if (result != MTK_FLP_SUCCESS)
    {
        FLPD("mtk_flp_threads_create, data center thread ERR\n");
        return (result);
    }

    result = mtk_flp_threads_create(MTK_FLP_THREAD_SOCKET, mtk_flp_socket_thread);
    if (result != MTK_FLP_SUCCESS)
    {
        FLPD("mtk_flp_threads_create, socket thread ERR\n");
        return (result);
    }

    result = mtk_flp_threads_create(MTK_FLP_THREAD_WAKE_MONITOR, mtk_flp_wake_monitor_thread);
    if (result != MTK_FLP_SUCCESS)
    {
        FLPD("mtk_flp_threads_create, wake monitor thread ERR\n");
        return (result);
    }

    FLPD("mtk_flp_sys_init_android done,%d\n", result);

    return (result);
}

int mtk_flp_sys_deinit_android(void)
{
    int result = MTK_FLP_SUCCESS;
    int idx;

    FLPD("mtk_flp_sys_deinit_android...\n");

    // release thread
    result = mtk_flp_threads_release();
    if (MTK_FLP_SUCCESS != result)
    {
        FLPD("mtk_flp_threads_release fail,%d\n", result);
    }

    // un-initialize physical interface
    //   mtk_flp_sys_interface_uninit(NULL);

    // release timer
    // - TBD

    // release message queue
    // - TBD

    // release mutex
    result = mtk_flp_sys_mutex_deinitialize();
    if (MTK_FLP_SUCCESS != result)
    {
        FLPD("mtk_flp_sys_mutex_deinitialize fail,%d\n", result);
    }

    // delete event resource to avoid blocking call in main & service thread
    mtk_flp_tasks_deinit();

    // un-initialize global variables
    // - TBD

    FLPD("mtk_flp_sys_deinit_android done,%d\n", result);

    return result;
}

INT32 mtk_flp_threads_create(int threadId, thread_proc func)
{
    int result = MTK_FLP_SUCCESS;

    if ( threadId >= MTK_FLP_THREAD_END)
    {
        FLPD("mtk_flp_threads_create fail, invalid threadId, %d\n", threadId);
        result = MTK_FLP_ERROR;
    }

    if (pthread_create(&g_mtk_flp_thread[threadId].thread_handle,
                        NULL, func,
                        (void*)&g_mtk_flp_thread[threadId]))
    {
        g_ThreadExitHalFunc = TRUE;
        g_ThreadExitKernelFunc = TRUE;
        g_ThreadExitDC = TRUE;
        g_ThreadExitSocket = TRUE;
        FLPD("mtk_flp_threads_create fail,%d\n", threadId);
        result = MTK_FLP_ERROR;
    }

    return (result);
}

int mtk_flp_threads_release(void)
{
   int result = MTK_FLP_SUCCESS;
   int idx;

   FLPD("mtk_flp_threads_release...\n");

   for (idx = 0; idx < THREAD_NUM; idx++)
   {
       if (g_mtk_flp_thread[idx].thread_handle == C_INVALID_TID)
       {
           continue;
       }

       if (!g_mtk_flp_thread[idx].thread_exit)
       {
           continue;
       }

       if ((g_mtk_flp_thread[idx].thread_exit(&g_mtk_flp_thread[idx])))
       {
           result = MTK_FLP_ERROR;
           FLPD("mtk_flp_threads_release exit fail,%d\n", idx);
       }
   }

   FLPD("mtk_flp_threads_release done\n");

   return (result);
}


VOID *mtk_flp_socket_thread(VOID * arg)
{
    MTK_FLP_TASK_T* taskp = mtk_flp_find_task(MTK_FLP_TASKID_SOCKET);
    UINT32 ret = MTK_FLP_SUCCESS;
    MTK_FLP_MSG_T *flp_msg;
    MTK_FLP_THREAD_T *ptr = (MTK_FLP_THREAD_T*)arg;

    if (!arg)
    {
        FLPD("flp_socket_thread, Create ERR !arg\n");
        pthread_exit(NULL);
        return NULL;
    }

    FLPD("flp_socket_thread, Create\n");
    //mtk_flp_tasks_dump();

    int server_fd = C_INVALID_SOCKET;
    /*socklen_t*/int size;
    struct sockaddr_un server_addr;
    struct sockaddr_un client_addr;

    memset(&server_addr, '\0', sizeof(struct sockaddr_un));

    unlink (MTKFLP_COMM_SOCK_PATH);

    //----------------------------------------------------------------
    // Create a SOCKET for listening for incoming connection requests.
    //----------------------------------------------------------------
    if ((server_fd = socket(AF_LOCAL, SOCK_STREAM, 0)) == -1)
    {
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

    strcpy (server_addr.sun_path, MTKFLP_COMM_SOCK_PATH);

    FLPD("server_fd,%d\r\n",server_fd);
    FLPD("server_addr.sun_path,%s\r\n",server_addr.sun_path);
    FLPD("server_addr.sun_family,%d\r\n",server_addr.sun_family);

    if((bind (server_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un))) == -1)
    {
        FLPD("bind error\r\n");
        FLPD("fail to get socket from environment: %s\n",strerror(errno));
        close(server_fd);
        g_ThreadExitSocket = TRUE;
        pthread_exit(NULL);
    }

    int res;
    res = chmod(MTKFLP_COMM_SOCK_PATH, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP);
   // res = chmod(MTKFLP_COMM_SOCK_PATH, S_IRUSR | S_IXUSR | S_IWUSR | S_IRGRP |S_IWGRP | S_IXGRP | S_IROTH |S_IWOTH |S_IXOTH);
    FLPD("chmod1,%d..\r\n",res);

    //----------------------------------------------------------------
    // Listen for incoming connection requests on the created socket
    //----------------------------------------------------------------
    FLPD("start listen...(server_fd,%d)\r\n", server_fd);
    if(listen (server_fd, 5) == -1)
    {
        FLPD("listent fail: %s\n",strerror(errno));
        close(server_fd);
        g_ThreadExitSocket = TRUE;
        pthread_exit(NULL);
    }

    //----------------------------------------------------------------
    // Waiting for client to connect
    //----------------------------------------------------------------
    FLPD("Waiting for client to connect...\n");
    taskp->sockethandle = accept(server_fd, (struct sockaddr*)&client_addr, &size);
    FLPD("socket accept,%d\r\n",taskp->sockethandle);

    while (!g_ThreadExitSocket)
    {
        // - recv msg from socket interface
        //ret = mtk_flp_sys_msg_recv(MTK_FLP_TASKID_SOCKET, &flp_msg);
        ret = mtk_flp_sys_socket_recv_msg(taskp->sockethandle, &flp_msg);
        if (ret == MTK_FLP_SUCCESS && (!g_ThreadExitSocket))
        {
            FLPD("flp_socket_thread, read msg ok (type,%d,len:%d)\n", flp_msg->type, flp_msg->length);

            // send msg to HAL thread internal queue
            mtk_flp_sys_msg_send(MTK_FLP_TASKID_HAL, flp_msg);
        }
        else
        {
            FLPD("flp_socket_thread, read msg fail,exit socket thread\n");

            //read msg fail...
            g_ThreadExitSocket = TRUE;
            // [Remind] Only for Blocking-read-mechanism.
            // Since flp_socket_thread is closed , send MTK_FLP_EXIT_REQ message to service.
            // In abnormal case : This mechanism can avoid zombie process.
            flp_msg = mtk_flp_sys_msg_alloc( sizeof(MTK_FLP_MSG_T) );
            flp_msg->type = MTK_FLP_MSG_SYS_SHUTDOWN_NTF;
            flp_msg->length = 0;
            FLPD("flp_socket_thread, send msg to service (MTK_FLP_EXIT_REQ)\n");
            mtk_flp_sys_msg_send(MTK_FLP_TASKID_HAL, flp_msg);
        }
    }

    //----------------------------------------------------------------
    // Close socket
    //----------------------------------------------------------------
    FLPD("closing socket fd[%d],%02x\r\n",taskp->sockethandle);
    close(taskp->sockethandle);
    taskp->sockethandle = C_INVALID_SOCKET;


    unlink (MTKFLP_COMM_SOCK_PATH);

    FLPD("flp_socket_thread, exit\n");

    g_ThreadExitSocket = TRUE;
    pthread_exit(NULL);

    return NULL;
}


VOID * mtk_flp_dc_thread(VOID *arg)
{
    UINT32 ret = MTK_FLP_SUCCESS;
    MTK_FLP_MSG_T *flp_msg;
    MTK_FLP_THREAD_T *ptr = (MTK_FLP_THREAD_T*)arg;

    if (!arg)
    {
        FLPD("mtk_flp_dc_thread, Create ERR !arg\n");
        pthread_exit(NULL);
        return NULL;
    }

    FLPD("mtk_flp_dc_thread, Create\n");
    //mtk_flp_tasks_dump();
    mtk_flp_dc_init();

    while (!g_ThreadExitDC)
    {
        // - recv msg
        FLPD("DC recv message");
        ret = mtk_flp_sys_msg_recv(MTK_FLP_TASKID_DC, &flp_msg);
        if (ret == MTK_FLP_SUCCESS && (!g_ThreadExitDC))
        {
            // - proc msg
            mtk_flp_dc_proc(flp_msg);

            // - free msg
            mtk_flp_sys_msg_free(flp_msg);
        }
        else
        {
            //read msg fail...
            FLPD("mtk_flp_dc_thread, read msg fail\n");

            mtk_flp_sys_sleep(1); // avoid busy loop
        }
    }

    FLPD("mtk_flp_dc_thread, exit\n");

    g_ThreadExitDC = TRUE;
    pthread_exit(NULL);

    return NULL;

}



VOID * mtk_flp_kernel_thread(VOID *arg)
{
    UINT32 ret = MTK_FLP_SUCCESS;
    MTK_FLP_MSG_T *flp_msg;
    MTK_FLP_THREAD_T *ptr = (MTK_FLP_THREAD_T*)arg;

    if (!arg)
    {
        FLPD("mtk_flp_kernel_thread, Create ERR !arg\n");
        pthread_exit(NULL);
        return NULL;
    }

    FLPD("mtk_flp_kernel_thread, Create\n");

    //mtk_flp_tasks_dump();

    while (!g_ThreadExitKernelFunc)
    {
        FLPD("Kernel recv message");
        // - recv msg
        ret = mtk_flp_sys_msg_recv(MTK_FLP_TASKID_KERNEL, &flp_msg);
        if (ret == MTK_FLP_SUCCESS && (!g_ThreadExitKernelFunc))
        {
            // - proc msg
            mtk_flp_kernel_proc(flp_msg);

            // - free msg
            mtk_flp_sys_msg_free(flp_msg);
        }
        else
        {
            //read msg fail...
            FLPD("mtk_flp_kernel_thread, read msg fail\n");

            mtk_flp_sys_sleep(1); // avoid busy loop
        }
    }

    FLPD("mtk_flp_kernel_thread, exit\n");

    g_ThreadExitKernelFunc = TRUE;
    pthread_exit(NULL);

    return NULL;

}



VOID mtk_flp_hal_thread(VOID)
{
    UINT32 ret = MTK_FLP_SUCCESS;
    MTK_FLP_MSG_T *flp_msg;
#if 0
    MTK_FLP_THREAD_T *ptr = (MTK_FLP_THREAD_T*)arg;

    if (!arg)
    {
        FLPD("mtk_flp_hal_thread, Create ERR !arg\n");
        pthread_exit(NULL);
        return NULL;
    }
#endif
    FLPD("mtk_flp_hal_thread, Create\n");

//    mtk_flp_tasks_dump();

    while (!g_ThreadExitHalFunc)
    {
        // - recv msg
        FLPD("HAL recv message");
        ret = mtk_flp_sys_msg_recv(MTK_FLP_TASKID_HAL, &flp_msg);
        if (ret == MTK_FLP_SUCCESS && (!g_ThreadExitHalFunc))
        {
            // - proc msg
            mtk_flp_hal_proc(flp_msg);

            // - free msg
            mtk_flp_sys_msg_free(flp_msg);
        }
        else
        {
            //read msg fail...
            FLPD("mtk_flp_hal_thread, read msg fail\n");

            mtk_flp_sys_sleep(1); // avoid busy loop
        }
    }

    FLPD("mtk_flp_hal_thread, exit\n");

    g_ThreadExitHalFunc = TRUE;
    pthread_exit(NULL);

    return ;

}

VOID * mtk_flp_wake_monitor_thread(VOID *arg)
{
    UINT32 ret = MTK_FLP_SUCCESS;
    MTK_FLP_MSG_T *flp_msg;
    MTK_FLP_THREAD_T *ptr = (MTK_FLP_THREAD_T*)arg;
    static char const * kSleepFileName = "/sys/power/wait_for_fb_sleep";
    char *buf;
    int fd, err = 0;


    if (!arg)
    {
        FLPD("mtk_flp_wake_monitor_thread, Create ERR !arg\n");
        pthread_exit(NULL);
        return NULL;
    }

    FLPD("mtk_flp_wake_monitor_thread, Create\n");

    //mtk_flp_tasks_dump();

    while (!g_ThreadExitWakeMonitor)
    {
        FLPD("Wake Monitor Restart");

        if (wait_for_fb_sleep())
        {
            FLPD("Failed reading wait_for_fb_sleep");
        }
        pthread_mutex_lock(&earlysuspend_mutex);
        earlysuspend_state = EARLYSUSPEND_MEM;
        pthread_cond_signal(&earlysuspend_cond);
        pthread_mutex_unlock(&earlysuspend_mutex);
        FLPD("Screen off");

        //Send NTF to CONNsys
        flp_msg = mtk_flp_sys_msg_alloc( sizeof(MTK_FLP_MSG_T) + sizeof(UINT8) );
        flp_msg->type = MTK_FLP_MSG_CONN_SCREEN_STATUS;
        flp_msg->length = sizeof(UINT8);
        memcpy( ((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T), &earlysuspend_state, sizeof(UINT8));
        mtk_flp_sys_msg_send(MTK_FLP_TASKID_HAL, flp_msg);

        if (wait_for_fb_wake()) {
            FLPD("Failed reading wait_for_fb_wake");
        }
        pthread_mutex_lock(&earlysuspend_mutex);
        earlysuspend_state = EARLYSUSPEND_ON;
        pthread_cond_signal(&earlysuspend_cond);
        pthread_mutex_unlock(&earlysuspend_mutex);
        FLPD("Screen on");

        //Send NTF to CONNsys
        flp_msg = mtk_flp_sys_msg_alloc( sizeof(MTK_FLP_MSG_T)+ sizeof(UINT8) );
        flp_msg->type = MTK_FLP_MSG_CONN_SCREEN_STATUS;
        flp_msg->length = sizeof(UINT8);
        memcpy( ((INT8*)flp_msg)+sizeof(MTK_FLP_MSG_T), &earlysuspend_state, sizeof(UINT8));
        mtk_flp_sys_msg_send(MTK_FLP_TASKID_HAL, flp_msg);
    }

    FLPD("mtk_flp_wake_monitor_thread, exit\n");

    g_ThreadExitWakeMonitor = TRUE;
    pthread_exit(NULL);

    return NULL;

}


int main (int argc, char** argv)
{
   int result = MTK_FLP_ERROR;

   FLPD("FLP Daemon main\n");

   g_NfcTsetMode = 0;
   if(argc == 2)
   {
      if(!strcmp(argv[1],"FLP_TEST_MODE"))
      {
         FLPD("ENTERY_TEST_MODE\r\n");
         g_NfcTsetMode = 1;
      }
   }
   FLPD("ENTERY_TEST_MODE,%d\r\n",g_NfcTsetMode);

   result = mtk_flp_sys_init_android();

   if (MTK_FLP_SUCCESS == result)
   {
      mtk_flp_hal_thread();     //hal thread will run in main process
   }
   else
   {
      FLPD("mtk_flp_sys_init_android fail\n");
   }

   mtk_flp_sys_deinit_android();

   FLPD("mtkFlpDaemon exits\n");

   return (MTK_FLP_SUCCESS);
}

