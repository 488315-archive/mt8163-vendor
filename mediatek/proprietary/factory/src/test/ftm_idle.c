/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
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

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#if defined(MTK_FTM_C2K_SUPPORT)
#include <c2kutils.h>
#endif
#include <sys/mount.h>
#include <sys/statfs.h>

#include <linux/fb.h>

#include <cutils/properties.h>

#include "common.h"
#include "miniui.h"
#include "utils.h"
#include "ftm.h"

#include "hardware/ccci_intf.h"

#ifdef FEATURE_FTM_IDLE

#define TAG "[IDLE] "

#define mod_to_idle(p) (struct ftm_idle *)((char *)(p) + sizeof(struct ftm_module))

struct ftm_idle
{
    struct ftm_module *mod;
    struct textview tv;
    text_t title;
};

#ifdef MTK_ENABLE_MD1
static int fd_atcmd;
static char ccci_md1_dev[32];
#endif
#ifdef MTK_ENABLE_MD2
static int fd_atcmd2;
static char ccci_md2_dev[32];
#endif
#if defined(MTK_EXTERNAL_MODEM_SLOT) && !defined(EVDO_FTM_DT_VIA_SUPPORT) && !defined(MTK_FTM_C2K_SUPPORT)
static int fd_atcmd_dt;
#endif

#if defined(MTK_FTM_C2K_SUPPORT) && !defined(EVDO_FTM_DT_VIA_SUPPORT)
static int fd3;
#endif

#ifdef MTK_MD_SHUT_DOWN_NT
static int fd_ioctl;
static int fd_ioctlmd2;
static int fd_ioctlextmd;
static char ccci_md1_power_ioctl_dev[32];
static char ccci_md2_power_ioctl_dev[32];
#endif

static const char *atcmd_ret;
static char *atDevPath3 = NULL ;

extern int usb_com_port;
extern int usb_plug_in;
extern int idle_current_done;

static int idle_key_handler(int key, void *priv)
{
    return 0;
}

#if defined(MTK_FTM_C2K_SUPPORT) && !defined(EVDO_FTM_DT_VIA_SUPPORT)
extern void C2KEnterFlightMode();
extern void C2KLeaveFlightMode();
static int wait_SDIO_ready()
{
    int ret = 0;
    char value[100] = {0};
    while(1) {
       usleep(5000);
       ret = property_get("net.cdma.mdmstat",value,NULL);
       LOGD(TAG "wait_SDIO_ready ret = %d",ret);
       LOGD(TAG "wait_SDIO_ready value = %s",value);
       if (ret > 0) {
           if (!strncmp(value,"ready",5)) {
               LOGD(TAG "Get SDIO ready");
               break;
           }
       }
    }
    return 1;
}

static int PowerOffModem3(int fd) {
    LOGD(TAG "[C2K]AT polling first:\n");
    send_at(fd, "ATE0Q0V1\r\n");
    do {
        send_at(fd, "AT\r\n");
    } while (wait4_ack(fd, NULL, 3000));


    LOGD(TAG "[C2K]Send AT+EPOF to reboot modem \n");
    if (send_at(fd, "AT+EPOF\r\n")) goto err;
    wait4_ack(fd, NULL, 5000);
    LOGD(TAG "[C2K]Wait for +VPON:0, C2K modem turn off:\n");
    wait4_ack(fd, "+VPON:0", 10000);
    closeDevice(fd);
    // close fd before calling flightmode API.
    LOGD(TAG "[C2K]close fd3\n");
    C2KEnterFlightMode();
err:
    return -1;
}
#endif

static int power_off_md()
{
    int i = 0;

    /* Make MD into flight mode */
    #if defined(MTK_FTM_C2K_SUPPORT) && !defined(EVDO_FTM_DT_VIA_SUPPORT)
    LOGD(TAG "[C2K]Go to open C2K modem fd3!");
    wait_SDIO_ready();
    // wait modem3 ready
    atDevPath3 = viatelAdjustDevicePathFromProperty(VIATEL_CHANNEL_AT);
    LOGD(TAG "[C2K]Go to open modem3 fd3 atDevPath3 = %s", atDevPath3);
    fd3 = openDeviceWithDeviceName(atDevPath3);
    if (fd3 < 0) {
        LOGD(TAG "[C2K]Fail to open ttySDIO4\n");
        return -1;
    }
    LOGD(TAG "[C2K]OK to open atDevPath3\n");
    for (i = 0; i < 30; i++) usleep(50000);
    // sleep 1.5s wait for modem bootup
    PowerOffModem3(fd3);
    #endif

    #ifdef MTK_ENABLE_MD1
        snprintf(ccci_md1_dev, 32, "%s", ccci_get_node_name(USR_FACTORY_IDLE, MD_SYS1));
        LOGD(TAG "Open CCCI MD1 dev node: %s\n", ccci_md1_dev);
        fd_atcmd = openDeviceWithDeviceName(ccci_md1_dev);
        if (-1 == fd_atcmd) {
            LOGD(TAG "Fail to open CCCI interface\n");
            return -1;
        }
        for (i = 0; i < 30; i++) usleep(50000); // sleep 1s wait for modem bootup

        #ifdef MTK_MD_SHUT_DOWN_NT
            snprintf(ccci_md1_power_ioctl_dev, 32, "%s", ccci_get_node_name(USR_FACTORY_IDLE_IOCTL, MD_SYS1));
            LOGD(TAG "Open CCCI MD1 Power IOCTL dev node: %s\n", ccci_md1_power_ioctl_dev);
            fd_ioctl = openDeviceWithDeviceName(ccci_md1_power_ioctl_dev);
            if (-1 == fd_ioctl) {
                LOGD(TAG "Fail to open CCCI IOCTL interface\n");
                return -1;
            }

            ExitFlightMode_PowerOffModem(fd_atcmd,fd_ioctl,FALSE);
        #else
            ExitFlightMode_idle(fd_atcmd, FALSE);
        #endif
    #endif
    #ifdef MTK_ENABLE_MD2
        snprintf(ccci_md2_dev, 32, "%s", ccci_get_node_name(USR_FACTORY_IDLE, MD_SYS2));
        LOGD(TAG "Open CCCI MD2 dev node: %s\n", ccci_md2_dev);
        fd_atcmd2 = openDeviceWithDeviceName(ccci_md2_dev);
        if (-1 == fd_atcmd2) {
            LOGD(TAG "Fail to open MD2 CCCI interface\n");
            return -1;
        }
        for (i = 0; i < 30; i++) usleep(50000); // sleep 1s wait for modem bootup

        #ifdef MTK_MD_SHUT_DOWN_NT
            snprintf(ccci_md2_power_ioctl_dev, 32, "%s", ccci_get_node_name(USR_FACTORY_IDLE_IOCTL, MD_SYS2));
            LOGD(TAG "Open CCCI MD2 Power IOCTL dev node: %s\n", ccci_md2_power_ioctl_dev);
            fd_ioctlmd2 = openDeviceWithDeviceName(ccci_md2_power_ioctl_dev);
            if (-1 == fd_ioctlmd2) {
                LOGD(TAG "Fail to open CCCI MD2 IOCTL interface\n");
                return -1;
            }
            ExitFlightMode_PowerOffModem(fd_atcmd2,fd_ioctlmd2,FALSE);
        #else
            ExitFlightMode_idle(fd_atcmd2, FALSE);
        #endif
    #endif
    #if defined(MTK_DT_SUPPORT) && defined(MTK_EXTERNAL_MODEM_SLOT) && !defined(EVDO_FTM_DT_VIA_SUPPORT) && !defined(MTK_FTM_C2K_SUPPORT)
        #if defined(PURE_AP_USE_EXTERNAL_MODEM)
            fd_atcmd_dt = openDeviceWithDeviceName(CCCI_MODEM_MT8135);
        #else
            fd_atcmd_dt= openDeviceWithDeviceName(CCCI_MODEM_MT6252);
        #endif

        if (-1 == fd_atcmd_dt) {
            LOGD(TAG "Fail to open CCCI interface\n");
            return -1;
        }

        #if defined(PURE_AP_USE_EXTERNAL_MODEM)
            initTermIO(fd_atcmd_dt, 5);
        #endif

        for (i = 0; i < 30; i++) usleep(50000); // sleep 1s wait for modem bootup

        ExitFlightMode_idle(fd_atcmd_dt, FALSE);
    #endif

    return 0;
}

static int power_on_md()
{
    int i = 0;

    /* Make MD exit flight mode */
    #ifdef MTK_ENABLE_MD1
        #ifdef MTK_MD_SHUT_DOWN_NT
            ExitFlightMode_PowerOffModem(fd_atcmd,fd_ioctl,TRUE);
            closeDevice(fd_ioctl);
        #endif
        closeDevice(fd_atcmd);
    #endif
    #ifdef MTK_ENABLE_MD2
        #ifdef MTK_MD_SHUT_DOWN_NT
            ExitFlightMode_PowerOffModem(fd_atcmd2,fd_ioctlmd2,TRUE);
            closeDevice(fd_ioctlmd2);
        #endif
        closeDevice(fd_atcmd2);
    #endif
    #if defined(MTK_EXTERNAL_MODEM_SLOT) && !defined(EVDO_FTM_DT_VIA_SUPPORT) && !defined(MTK_FTM_C2K_SUPPORT)
        closeDevice(fd_atcmd_dt);
    #endif
#if defined(MTK_FTM_C2K_SUPPORT) && !defined(EVDO_FTM_DT_VIA_SUPPORT)
    C2KLeaveFlightMode();
#endif
    for (i = 0; i < 30; i++) usleep(100000); // sleep 1s wait for modem bootup

    return 0;
}

int idle_entry(struct ftm_param *param, void *priv)
{
    struct ftm_idle *idle = (struct ftm_idle *)priv;
    struct textview *tv = &idle->tv;

    int fd_suspend = -1, fd_backlight = -1, fd_mdm = -1, fd_fb = -1;
    int ret = 0, key = 0, i = 0, err = 0;

    char *s_state_mem = "mem";
    char *s_state_on = "on";
    char *s_backlight_on = "102";
    char *s_backlight_off = "0";
    char *s_mdm_txpwr_disable = "0";
    char *close_com_port = "close com port!";
    char filename[32] = {0};

    LOGD(TAG "%s: idle_entry\n", __FUNCTION__);

    init_text(&idle->title, param->name, COLOR_YELLOW);

    ui_init_textview(tv, idle_key_handler, (void*)idle);
    tv->set_title(tv, &idle->title);

    /* Turn off backlight */
    property_set("debug.factory.idle_state", "backlight_off_0");
    fd_backlight = open("/sys/class/leds/lcd-backlight/brightness", O_RDWR, 0);
    if (fd_backlight < 0) {
        idle->mod->test_result = FTM_TEST_FAIL;
        LOGD(TAG "%s: cannot open /sys/class/leds/lcd-backlight/brightness\n", __FUNCTION__);
        return -1;
    }
    ret = write(fd_backlight, s_backlight_off, strlen(s_backlight_off));
    close(fd_backlight);
    property_set("debug.factory.idle_state", "backlight_off_1");

    /* make fb blank */
    property_set("debug.factory.idle_state", "display_blank_0");
    snprintf(filename, sizeof(filename), "/dev/graphics/fb0");
    fd_fb = open(filename, O_RDWR);
    if (fd_fb < 0) {
        LOGD("Failed to open fb0 device: %s", strerror(errno));
    }
    err = ioctl(fd_fb, FBIOBLANK, FB_BLANK_POWERDOWN);
    if (err < 0) {
        LOGD("Failed to blank fb0 device: %s", strerror(errno));
    }
    close(fd_fb);
    property_set("debug.factory.idle_state", "display_blank_1");

    /* power off md */
    property_set("debug.factory.idle_state", "md_power_off_0");
    LOGD("power_off_md (start)");
    if (power_off_md() < 0) {
        LOGD(TAG "%s: fail to power off md\n", __FUNCTION__);
    }
    LOGD("power_off_md (end)");
    property_set("debug.factory.idle_state", "md_power_off_1");

    /* Turn off thermal query MD TXPWR function */
    property_set("debug.factory.idle_state", "md_txpwr_off_0");
    fd_mdm = open("/proc/mtk_mdm_txpwr/txpwr_sw", O_RDWR, 0);
    if (fd_mdm < 0) {
        idle->mod->test_result = FTM_TEST_FAIL;
        LOGD(TAG "%s: cannot open /proc/mtk_mdm_txpwr/txpwr_sw, not support\n", __FUNCTION__);
    }
    ret = write(fd_mdm, s_mdm_txpwr_disable, strlen(s_mdm_txpwr_disable));
    close(fd_mdm);
    property_set("debug.factory.idle_state", "md_txpwr_off_1");

    /* Make AP enter sleep mode */
    property_set("debug.factory.idle_state", "ap_enter_suspend_0");
    fd_suspend = open("/sys/power/state", O_RDWR, 0);
    if (fd_suspend < 0) {
        idle->mod->test_result = FTM_TEST_FAIL;
        LOGD(TAG "%s: cannot open /sys/power/state\n", __FUNCTION__);
        return -1;
    }
    ret = write(fd_suspend, s_state_mem, strlen(s_state_mem));
    close(fd_suspend);
    property_set("debug.factory.idle_state", "ap_enter_suspend_1");

    if (param->test_type == FTM_MANUAL_ITEM) {
        while (1) {
            key = ui_wait_phisical_key();
            LOGD(TAG "%s: %d\n", __FUNCTION__, key);

            /* Make AP enter resume mode */
            fd_suspend = open("/sys/power/state", O_RDWR, 0);
            if (fd_suspend < 0) {
                idle->mod->test_result = FTM_TEST_FAIL;
                LOGD(TAG "%s: cannot open /sys/power/state\n", __FUNCTION__);
                return -1;
            }
            ret = write(fd_suspend, s_state_on, strlen(s_state_on));
            close(fd_suspend);
            LOGD(TAG "%s: exit from suspend\n", __FUNCTION__);

            break;
        }
    } else {
        int write_len = write(usb_com_port, close_com_port, strlen(close_com_port));
        LOGD(TAG "after write data to pc\n");
        if (write_len != strlen(close_com_port)) {
            LOGD(TAG "write data to pc fail\n");
        }

        close_usb();

        while(is_usb_state_plugin()) {
            sleep(1);
        }

        sleep(2);

        while(!is_usb_state_plugin()) {
            sleep(1);
        }

        /* Make AP enter resume mode */
        fd_suspend = open("/sys/power/state", O_RDWR, 0);
        if (fd_suspend < 0) {
            idle->mod->test_result = FTM_TEST_FAIL;
            LOGD(TAG "%s: cannot open /sys/power/state\n", __FUNCTION__);
            return -1;
        }
        ret = write(fd_suspend, s_state_on, strlen(s_state_on));
        close(fd_suspend);
        LOGD(TAG "%s: exit from suspend\n", __FUNCTION__);

        usb_plug_in = 1;
        idle_current_done = 1;
    }

    /* power on md */
    property_set("debug.factory.idle_state", "md_power_on_0");
    LOGD("power_on_md (start)");
    if (power_on_md() < 0) {
        LOGD(TAG "%s: fail to power on md\n", __FUNCTION__);
    }
    LOGD("power_on_md (end)");
    property_set("debug.factory.idle_state", "md_power_on_1");

    /* make fb unblank */
    property_set("debug.factory.idle_state", "display_unblank_0");
    snprintf(filename, sizeof(filename), "/dev/graphics/fb0");
    fd_fb = open(filename, O_RDWR);
    if (fd_fb < 0) {
        LOGD("Failed to open fb0 device: %s", strerror(errno));
    }
    err = ioctl(fd_fb, FBIOBLANK, FB_BLANK_UNBLANK);
    if (err < 0) {
        LOGD("Failed to unblank fb0 device: %s", strerror(errno));
    }
    close(fd_fb);
    property_set("debug.factory.idle_state", "display_unblank_1");

    /* Turn on backlight */
    property_set("debug.factory.idle_state", "backlight_on_0");
    fd_backlight = open("/sys/class/leds/lcd-backlight/brightness", O_RDWR, 0);
    if (fd_backlight < 0) {
        idle->mod->test_result = FTM_TEST_FAIL;
        LOGD(TAG "%s: cannot open /sys/class/leds/lcd-backlight/brightness\n", __FUNCTION__);
        return -1;
    }
    ret = write(fd_backlight, s_backlight_on, strlen(s_backlight_on));
    close(fd_backlight);
    property_set("debug.factory.idle_state", "backlight_on_1");

    idle->mod->test_result = FTM_TEST_PASS;
    return 0;
}

int idle_init(void)
{
    int ret = 0;
    struct ftm_module *mod;
    struct ftm_idle *idle;

    LOGD(TAG "idle_init\n");

    mod = ftm_alloc(ITEM_IDLE, sizeof(struct ftm_idle));
    if (!mod)
        return -ENOMEM;

    idle = mod_to_idle(mod);
    idle->mod = mod;

    ret = ftm_register(mod, idle_entry, (void*) idle);
    if (ret) {
        LOGD(TAG "register IDLE failed (%d)\n", ret);
        return ret;
    }

    return 0;
}

#endif
