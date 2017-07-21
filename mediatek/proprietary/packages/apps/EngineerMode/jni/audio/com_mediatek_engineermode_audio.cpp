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
#undef LOG_NDEBUG
#undef NDEBUG

#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "emAudio-JNI"
#endif

#include <cutils/log.h>
extern "C" {
#include "AudioParamParser.h"
}

using namespace android;

static jstring audio_getCategory(JNIEnv *env, jclass clazz, jstring param1, jstring param2) {
    ALOGD("Enter getCategory\n");
    const char *param1Jni = env->GetStringUTFChars(param1, NULL);
    const char *param2Jni = env->GetStringUTFChars(param2, NULL);
    ALOGD("Param is %s, %s\n", param1Jni, param2Jni);
    char* result = utilNativeGetCategory(param1Jni, param2Jni);
    ALOGD("Result is %s\n", result);
    jstring javeResult = env->NewStringUTF(result);
    free(result);
    return javeResult;
}

static jstring audio_getParams(JNIEnv *env, jclass clazz, jstring param1, jstring param2, jstring param3) {
    ALOGD("Enter getParams\n");
    const char *param1Jni = env->GetStringUTFChars(param1, NULL);
    const char *param2Jni = env->GetStringUTFChars(param2, NULL);
    const char *param3Jni = env->GetStringUTFChars(param3, NULL);
    ALOGD("Param is %s, %s, %s\n", param1Jni, param2Jni, param3Jni);
    char* result = utilNativeGetParam(param1Jni, param2Jni, param3Jni);
    ALOGD("Result is %s\n", result);
    jstring javeResult = env->NewStringUTF(result);
    free(result);
    return javeResult;
}

static jstring audio_getChecklist(JNIEnv *env, jclass clazz, jstring param1, jstring param2, jstring param3) {
    ALOGD("Enter getChecklist\n");
    const char *param1Jni = env->GetStringUTFChars(param1, NULL);
    const char *param2Jni = env->GetStringUTFChars(param2, NULL);
    const char *param3Jni = env->GetStringUTFChars(param3, NULL);
    ALOGD("Param is %s, %s, %s\n", param1Jni, param2Jni, param3Jni);
    char* result = utilNativeGetChecklist(param1Jni, param2Jni, param3Jni);
    ALOGD("Result is %s\n", result);
    jstring javeResult = env->NewStringUTF(result);
    return javeResult;
}

static jboolean audio_setParams(JNIEnv *env, jclass clazz,
    jstring param1, jstring param2, jstring param3, jstring param4) {
    ALOGD("Enter setParams\n");
    const char *param1Jni = env->GetStringUTFChars(param1, NULL);
    const char *param2Jni = env->GetStringUTFChars(param2, NULL);
    const char *param3Jni = env->GetStringUTFChars(param3, NULL);
    const char *param4Jni = env->GetStringUTFChars(param4, NULL);
    ALOGD("Param is %s, %s, %s, %s\n", param1Jni, param2Jni, param3Jni, param4Jni);
    APP_STATUS result = utilNativeSetParam(param1Jni, param2Jni, param3Jni, param4Jni);
    if (result == APP_NO_ERROR) {
        ALOGD("set success\n");
        return JNI_TRUE;
    } else {
        ALOGD("set fail\n");
        return JNI_FALSE;
    }
}

static jboolean audio_saveToWork(JNIEnv *env, jclass clazz, jstring param) {
    ALOGD("Enter saveToWork\n");
    const char *paramJni = env->GetStringUTFChars(param, NULL);
    ALOGD("Param is %s\n", paramJni);

    APP_STATUS result = utilNativeSaveXml(paramJni);
    if (result == APP_NO_ERROR) {
        ALOGD("set success\n");
        return JNI_TRUE;
    } else {
        ALOGD("set success\n");
        return JNI_FALSE;
    }
}

static void xmlChangedCallback(AppHandle *appHandle, const char *audioTypeName)
{
    if (appHandleReloadAudioType(appHandle, audioTypeName) == APP_ERROR)
    {
        ALOGD("Reload xml fail! (audioType = %s)\n", audioTypeName);
    }
    else
    {
        ALOGD("Reload XML done. (audioType = %s)\n", audioTypeName);
    }

}

static jboolean audio_registerXmlChangedCallback(JNIEnv *env, jclass clazz) {

        AppHandle * appHandle = appHandleGetInstance();

        appHandleRegXmlChangedCb(appHandle, xmlChangedCallback);

        ALOGD("enter audio_registerXmlChangedCallback\n");

        return true;
}

static JNINativeMethod methods[] = {
    { "getCategory", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;",
    (void *) audio_getCategory },
    { "getParams", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;",
    (void *) audio_getParams },
    { "getChecklist", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;",
    (void *) audio_getChecklist },
    { "setParams", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z",
    (void *) audio_setParams },
    { "saveToWork", "(Ljava/lang/String;)Z", (void *) audio_saveToWork },
    { "registerXmlChangedCallback", "()Z", (void *) audio_registerXmlChangedCallback },
};

// This function only registers the native methods
static int registerNatives(JNIEnv *env) {
    ALOGE("Register: register_com_mediatek_audio()...\n");
    return AndroidRuntime::registerNativeMethods(env,
            "com/mediatek/engineermode/audio/AudioTuningJni", methods, NELEM(methods));
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

