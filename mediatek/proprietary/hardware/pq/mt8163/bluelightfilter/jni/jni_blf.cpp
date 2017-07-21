/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "JNI_BLUELIGHTFILTER"

#include "IBlueLightFilter.h"

#include <utils/Log.h>
#include <utils/threads.h>
#include <cutils/log.h>
#include <cutils/properties.h>
#include <jni.h>
#include <android/log.h>

using namespace android;

#define UNUSED(expr) do { (void)(expr); } while (0)
#define JNI_BLF_CLASS_NAME "com/mediatek/blf/BlueLightFilter"

static jboolean nativeSetBlueLightFilterStrength(JNIEnv* env, jclass clz, jint strength)
{
    UNUSED(env);
    UNUSED(clz);

	jboolean ret;
	ret = android::IBlueLightFilter::getInstance().SetBlueLightFilterStrength(strength);
	return ret;
}


static jint nativeGetBlueLightFilterStrength(JNIEnv* env, jclass clz)
{
    UNUSED(env);
    UNUSED(clz);

	jint ret;
    ret = android::IBlueLightFilter::getInstance().GetBlueLightFilterStrength();
	return ret;
}


static jboolean nativeEnableBlueLightFilter(JNIEnv* env, jclass clz, jboolean enable)
{
    UNUSED(env);
    UNUSED(clz);

	jboolean ret;
    ret = android::IBlueLightFilter::getInstance().EnableBlueLightFilter(enable);
	return ret;
}

static jboolean nativeIsBlueLightFilterEnabled(JNIEnv* env, jclass clz)
{
    UNUSED(env);
    UNUSED(clz);

	jboolean ret;
    ret = android::IBlueLightFilter::getInstance().IsBlueLightFilterEnabled();
	return ret;
}

static jint nativeGetBlueLightFilterStrengthRange(JNIEnv* env, jclass clz)
{
    UNUSED(env);
    UNUSED(clz);

	jint ret;
	ret = android::IBlueLightFilter::getInstance().GetBlueLightFilterStrengthRange();
	return ret;
}

static jboolean nativeBlueLightFilterInit(JNIEnv* env, jclass clz)
{
    UNUSED(env);
    UNUSED(clz);

	jboolean ret;
    ret = android::IBlueLightFilter::getInstance().BlueLightFilterInit();
	return ret;
}



/////////////////////////////////////////////////////////////////////////////////

//JNI register
///////////////////////////////////////////////////////////////
static const char *classPathName = JNI_BLF_CLASS_NAME;

static JNINativeMethod g_methods[] = {
    {"nativeSetBlueLightFilterStrength", "(I)Z", (void*)nativeSetBlueLightFilterStrength},
    {"nativeGetBlueLightFilterStrength", "()I", (void*)nativeGetBlueLightFilterStrength},
    {"nativeEnableBlueLightFilter", "(Z)Z", (void*)nativeEnableBlueLightFilter},
    {"nativeIsBlueLightFilterEnabled", "()Z", (void*)nativeIsBlueLightFilterEnabled},
    {"nativeGetBlueLightFilterStrengthRange", "()I", (void*)nativeGetBlueLightFilterStrengthRange},
    {"nativeBlueLightFilterInit", "()Z", (void*)nativeBlueLightFilterInit},
};

/*
 * Register several native methods for one class.
 */
static int registerNativeMethods(JNIEnv* env, const char* className,
    JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;

    clazz = env->FindClass(className);
    if (clazz == NULL) {
        ALOGE("Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        ALOGE("RegisterNatives failed for '%s'", className);
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

// ----------------------------------------------------------------------------

/*
 * This is called by the VM when the shared library is first loaded.
 */

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;

    UNUSED(reserved);

    ALOGI("JNI_OnLoad");

    if (JNI_OK != vm->GetEnv((void **)&env, JNI_VERSION_1_4)) {
        ALOGE("ERROR: GetEnv failed");
        goto bail;
    }

    if (!registerNativeMethods(env, classPathName, g_methods, sizeof(g_methods) / sizeof(g_methods[0]))) {
        ALOGE("ERROR: registerNatives failed");
        goto bail;
    }

    result = JNI_VERSION_1_4;

bail:
    return result;
}
