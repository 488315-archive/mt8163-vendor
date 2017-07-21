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

#define MTK_LOG_ENABLE 1
#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"
#include <cutils/properties.h>
#undef LOG_NDEBUG
#undef NDEBUG

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "emMbim-JNI"
#endif

#include <cutils/log.h>

using namespace android;



static const char DAEMON_NAME[]  = "mbimd";
static const int NAP_TIME = 50;

/*
 * Wait for a system property to be assigned a specified value.
 * If desired_value is NULL, then just wait for the property to
 * be created with any value. maxwait is the maximum amount of
 * time in seconds to wait before giving up.
 */
static int wait_for_property(const char *name, const char *desired_value, int maxwait)
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    int maxnaps = (maxwait * 1000) / NAP_TIME;

    if (maxnaps < 1) {
        maxnaps = 1;
    }

    while (maxnaps-- > 0) {
        if (property_get(name, value, NULL)) {
            if (desired_value == NULL)
            {
                if (value != NULL && strcmp(value, "obtaining"))
                    return 0;
            }
            else if (strcmp(value, desired_value) == 0)
            {
                return 0;
            }
        }
        usleep(NAP_TIME * 1000);
    }
    return -1; /* failure */
}

/**
 * Start atci service in user load.
 * atci service can auto-start in eng load
 */

static int atci_start(void)
{
    const char *ctrl_prop = "ctl.start";
    const char *desired_status = "running";
    const char atci_service[]  = "atci_service";
    const char atci_daemon[]  = "atcid-daemon-u";
    const char *daemon_prop_name = "init.svc.atci_service";
    const char *daemon_prop_name2 = "init.svc.atcid-daemon-u";
    property_set("persist.service.atci.usermode", "1");
    ALOGI("atci service start");
    property_set(ctrl_prop, atci_service);
    if (wait_for_property(daemon_prop_name, desired_status, 10) < 0) {
        ALOGI("Timed out waiting for atci_service to start");
        return -2;
    }

    ALOGI(" atcid-daemon-u start");
    property_set(ctrl_prop, atci_daemon);
    if (wait_for_property(daemon_prop_name2, desired_status, 10) < 0) {
        ALOGI("Timed out waiting for atci_daemon  to start");
        return -3;
    }
    return 0;
}

/**
 * Stop atci service in user load.
 */

static int atci_stop(void)
{
    const char *ctrl_prop = "ctl.stop";
    const char *desired_status = "stopped";
    const char atci_service[]  = "atci_service";
    const char atci_daemon[]  = "atcid-daemon-u";
    const char *daemon_prop_name = "init.svc.atci_service";
    const char *daemon_prop_name2 = "init.svc.atcid-daemon-u";
    property_set("persist.service.atci.usermode", "0");
    ALOGI("atci service stop");
    property_set(ctrl_prop, atci_service);
    if (wait_for_property(daemon_prop_name, desired_status, 10) < 0) {
        ALOGI("Timed out waiting for atci_service to stop");
        return -2;
    }

    ALOGI(" atcid-daemon-u stop");
    property_set(ctrl_prop, atci_daemon);
    if (wait_for_property(daemon_prop_name2, desired_status, 10) < 0) {
        ALOGI("Timed out waiting for atci_daemon  to stop");
        return -3;
    }
    return 0;
}

int mbimd_start(void)
{
    const char *ctrl_prop = "ctl.start";
    const char *desired_status = "running";
    const char *daemon_prop_name = "init.svc.mbimd";


    ALOGI("mbimd start");
    property_set(ctrl_prop, DAEMON_NAME);
    if (wait_for_property(daemon_prop_name, desired_status, 10) < 0) {
        ALOGI("Timed out waiting for mbimd to start");
        return -1;
    }
  #ifndef ENG_VERSION
    //add acti start on no-eng load
    ALOGI("user/userdebug version,start atcid");
    return atci_start();
  #else
    return 0 ;
  #endif
}


/**
 * Stop the mbimd daemon.
 */
int mbimd_stop(void)
{

   const char *ctrl_prop = "ctl.stop";
   const char *desired_status = "stopped";
   const char *daemon_prop_name = "init.svc.mbimd";


    ALOGI("mbimd_stop.");

    property_set(ctrl_prop, DAEMON_NAME);
    if (wait_for_property(daemon_prop_name, desired_status, 5) < 0) {
        ALOGI("Timed out waiting for mbimd to stop");
        return -1;
    }
    #ifndef ENG_VERSION
        //add acti stop on no-eng load
      ALOGI("user/userdebug version, stop atcid");
     return atci_stop();
    #else
     return 0 ;
    #endif
}



static jint mbim_startService(JNIEnv *env, jclass clazz) {
    ALOGD("Enter mbim_startService\n");
    int ret = mbimd_start();
    ALOGD("Return %d\n", ret);
    return ret;
}

static jint mbim_stopService(JNIEnv *env, jclass clazz) {
    ALOGD("Enter mbim_stopService\n");
    int ret = mbimd_stop();
    ALOGD("Return %d\n", ret);
    return ret;
}



static JNINativeMethod methods[] = {
    { "startService", "()I", (void *) mbim_startService },
    { "stopService", "()I", (void *) mbim_stopService },
};

// This function only registers the native methods
static int registerNatives(JNIEnv *env) {
    ALOGE("Register: register_com_mediatek_mbim()...\n");
    return AndroidRuntime::registerNativeMethods(env,
            "com/mediatek/engineermode/mbim/MBIMJni", methods, NELEM(methods));
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env = NULL;
    jint result = -1;

    ALOGD("Enter JNI_OnLoad()...\n");
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        ALOGE("ERROR: GetEnv failed\n");
        goto bail;
    }
    assert(env != NULL);

    if (registerNatives(env) < 0) {
        ALOGE("ERROR: Native registration failed\n");
        goto bail;
    }

    /* success -- return valid version number */
    result = JNI_VERSION_1_4;

    ALOGD("Leave JNI_OnLoad()...\n");
    bail: return result;
}

