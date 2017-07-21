/*
* Copyright (C) 2011-2015 MediaTek Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#define LOG_TAG "Lomo_J_151022_09"  // reduce log msg
#include <string.h>
#include <android/log.h>
#include <jni.h>
#include <android/bitmap.h>
#include <gui/Surface.h>
#include <ui/Region.h>
#include <utils/RefBase.h>
#include <cstdio>
#include <stdio.h>
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"
#include <android_runtime/android_graphics_SurfaceTexture.h>
#include <android_runtime/android_view_Surface.h>
#include <cutils/properties.h>
#include <utils/Vector.h>
#include <utils/Thread.h>
#include <utils/ThreadDefs.h>
#include <utils/Log.h>
#include <linux/ion_drv.h>
#include <ion/ion.h>
#include "libion_mtk/include/ion.h"
#include <sys/ioctl.h>
#include <sys/mman.h>

#ifdef MTK_CAM_LOMO_SUPPORT
#include "lomo_hal_jni.h"
LomoHalJni* mpLomoHalJni=NULL;
static MBOOL bAddrDumpEnable = MFALSE;    
static MBOOL bFpsDumpEnable = MFALSE;   
//#define MTK_LOMO_1_by_1_DEQUE 1
#endif

#include <sys/time.h>

using namespace android;

jclass mLomoEffectClass;
jobject mLomoEffectWeakObject;
jmethodID mPostEventFromNativeMethodId;

int g_previewWidth;
int g_previewHeight;
int g_bufferFormat;
int g_bufferWidth;
int g_bufferHeight;

int g_previewBufferSize;
int g_effectBufferSize;
int g_bufferCurson = 0;
int *g_currentEffectIds;
int g_effectNumOfPage;
int g_numOfBufferBlock;

int g_displayIndex = 0;
bool g_isReayToRun = false;
bool g_isFirstFrameDisplayed = false;

Condition g_condition;
Mutex g_mutex;

struct timeval tv, tv2;
unsigned long long g_display_startTime, g_display_endTime;
int g_displayFrame = 0;
unsigned long long g_input_startTime, g_input_endTime;
int g_inputFrame = 0;

unsigned char* u32lEffectBuffARRAY[12][3];//uint32_t u32lEffectBuffARRAY[12][3];
uint32_t u32lEffectIdx[12];
uint32_t u32lEffectIdxCount;

struct YUVData {
    // image buffer size to process
    int width;
    int height;
    int size;
    int ysize;
    int uvsize;
    YUVData() {
        width  = -1;
        height = -1;
        size   = -1;
        ysize  = -1;
        uvsize = -1;
    }
} gYUVData;

struct EffectBuffer {
    void *m_bffer;
    bool m_dirty;
    int m_effectId;
    int m_id;
};
Vector<EffectBuffer *> g_EffectBuffers;

struct NativeWindow {
    ANativeWindow *m_nativeWindow;
    bool m_isReady;
};
Vector<NativeWindow *> g_NativeWindows;

struct IonInfo {
    int mIonHandle;
    int32_t mIonFD;
    int mSize;
    void *mVirtualAddr;
    ion_user_handle_t mIonAllocHandle;
};
Vector<IonInfo *> g_IonInfos;

/**
 * callback to AP framework after effect processed by algorithm
 */

void postEventToSuper(int msg)
{
    ALOGD("postEventToSuper");
    JNIEnv *env = AndroidRuntime::getJNIEnv();
    env->CallStaticVoidMethod(mLomoEffectClass, mPostEventFromNativeMethodId, mLomoEffectWeakObject, msg);
}

void displayEffectEx(uint8_t *data, int surfaceNumber) {
    //ALOGI("begin displayEffect, surfaceNumber:%d", surfaceNumber);
    ANativeWindowBuffer *nativeWindowBuffer  = NULL;
    sp<GraphicBuffer> graphicbuffer  = NULL;
    void *ptr  = NULL;
    uint8_t *raw_data = data;

    int err = NO_ERROR;

    NativeWindow *nativeWindow = g_NativeWindows.itemAt(surfaceNumber);
    if (nativeWindow->m_isReady == false) {
        return;
    }

    ANativeWindow *aNativeWindow = nativeWindow->m_nativeWindow;
    err = aNativeWindow->dequeueBuffer_DEPRECATED(aNativeWindow, &nativeWindowBuffer);
    if (NO_ERROR != err) {
        ALOGE("dequeue failed (after): %s", strerror(-err));
        return;
    }

    graphicbuffer = new GraphicBuffer(nativeWindowBuffer, false);
    err = graphicbuffer->lock(GRALLOC_USAGE_SW_WRITE_OFTEN, &ptr);
    if (NO_ERROR != err) {
        ALOGE("lock buffer failed (after): %s", strerror(-err));
        graphicbuffer->unlock();
        return;
    }
    YUVData &yuv_data = gYUVData;
    memcpy(ptr, raw_data, yuv_data.size);

    graphicbuffer->unlock();
    aNativeWindow->queueBuffer_DEPRECATED(aNativeWindow, nativeWindowBuffer);
    //ALOGI("end displayEffect, surfaceNumber:%d", surfaceNumber);
    if (!g_isFirstFrameDisplayed) {
        postEventToSuper(100);
        g_isFirstFrameDisplayed = true;
    }

}

void
writeEffectDataToBuffers(uint8_t *data, int size, jint *ids) {
    JNIEnv *env = AndroidRuntime::getJNIEnv();
    if (env == NULL) {
        return;
    }
    const jbyte *jdata = reinterpret_cast<const jbyte*>(data);
    int start = g_bufferCurson * g_effectNumOfPage;
    EffectBuffer *top_buffer = g_EffectBuffers.itemAt(start);
    static int FirstTimeCheck = 1;

    while (top_buffer->m_dirty == false) {
    	ALOGI("block:%d has not been read, sleep 5 ms", g_bufferCurson);
    	usleep(5*1000);
    }
   
    u32lEffectIdxCount=0    ;
    #ifdef MTK_CAM_LOMO_SUPPORT  
    LomoTimer ldequeTmr("lmjava do effects",start,1);    
    #endif
    for (int i = start; i < start + g_effectNumOfPage; i++) {
        EffectBuffer *buffer =  g_EffectBuffers.itemAt(i);
        buffer->m_effectId = ids[i - start];
        if (1) {
            unsigned char* dstBuffer = (unsigned char*)buffer->m_bffer;
            unsigned char* u8lEffectBuff[3];
            int effectId=buffer->m_effectId;
            u8lEffectBuff[0]=(unsigned char*)dstBuffer;
            u8lEffectBuff[1]=(unsigned char*)(dstBuffer+(size*4/6));
            u8lEffectBuff[2]=(unsigned char*)(dstBuffer+(size*5/6));
            #ifdef MTK_CAM_LOMO_SUPPORT
            if(bAddrDumpEnable)
            {
                ALOGD("Enque i-start, effect, addr (g_bufferCurson)=  %d, %d, 0x%p (%d)",i - start, effectId,dstBuffer,g_bufferCurson );
            }
            #endif
            if(effectId!=(-1))
            {
                u32lEffectIdxCount++;
                u32lEffectBuffARRAY[i - start][0]=u8lEffectBuff[0];
                u32lEffectIdx[i - start]=effectId;
                #ifdef MTK_CAM_LOMO_SUPPORT
                #ifdef MTK_CAM_LOMO_NOT_LEGACY
                mpLomoHalJni->AllocLomoDstImage(i, u8lEffectBuff, buffer->m_id);
                #else
                mpLomoHalJni->AllocLomoDstImage(i,u8lEffectBuff);
                #endif
                mpLomoHalJni->LomoImageEnque(u8lEffectBuff,effectId);
                #ifdef MTK_LOMO_1_by_1_DEQUE
                mpLomoHalJni->LomoImageDeque(u8lEffectBuff,effectId);
                #endif
                #endif
            }
            else
            {
                u32lEffectBuffARRAY[i - start][0]=u8lEffectBuff[0];              
                u32lEffectIdx[i - start]=effectId;
            }
        }
        FirstTimeCheck = 0;
    }
    #ifdef MTK_CAM_LOMO_SUPPORT    
    ALOGD("u32lEffectIdxCount final= %d",u32lEffectIdxCount);
    #ifndef MTK_LOMO_1_by_1_DEQUE
    for (int i = start; i < start + g_effectNumOfPage; i++)
    {
        if( u32lEffectIdx[i - start]!=(-1))
        {
            if(bAddrDumpEnable)
            {
                ALOGD("Do deque, i, effectidx, Addr(%d, %d, 0x%p)",i, u32lEffectIdx[i - start],u32lEffectBuffARRAY[i - start][0]);//ALOGD("Do deque, i, effectidx, Addr(%d, %d, 0x%8x)",i, u32lEffectIdx[i - start],u32lEffectBuffARRAY[i - start][0]);//
            }
            mpLomoHalJni->LomoImageDeque(u32lEffectBuffARRAY[i - start],u32lEffectIdx[i - start]);
        }
        else
        {
            if(bAddrDumpEnable)
            {
                ALOGD("No deque, i, effectidx, Addr(%d, %d, 0x%p) No flush addr",i, u32lEffectIdx[i - start],u32lEffectBuffARRAY[i - start][0]);    //ALOGD("No deque, i, effectidx, Addr(%d, %d, 0x%8x) No flush addr",i, u32lEffectIdx[i - start],u32lEffectBuffARRAY[i - start][0]);   
            }
        }
    }
    #elif 0
    for(unsigned int  i = 0; i < u32lEffectIdxCount; i++)
    {
            mpLomoHalJni->DoPostProcessEffectToDstImage(u32lEffectBuffARRAY[i],u32lEffectIdx[i]);
    }
    #else    
    #endif    
    if(bFpsDumpEnable)
    {
        ldequeTmr.printTime();
        ldequeTmr.fps(u32lEffectIdxCount);
    }
    #endif
    top_buffer->m_dirty = false;
    g_bufferCurson = (g_bufferCurson + 1) % g_numOfBufferBlock;

    if (g_inputFrame == 0) {
        gettimeofday(&tv,NULL);
        g_input_startTime = tv.tv_sec * 1000000 + tv.tv_usec;
    }
    g_inputFrame ++ ;
    if (g_inputFrame % 20 == 0) {
        gettimeofday(&tv2,NULL);
        g_input_endTime = tv2.tv_sec * 1000000 + tv2.tv_usec;
        ALOGI("inputFps = %llu\n", 20 * 1000000 / (g_input_endTime - g_input_startTime));
        g_input_startTime = g_input_endTime;
    }
}

class DisplayThread : public Thread {
public:
    DisplayThread(uint32_t id) {
        ALOGI("[%s]", __func__);
        mName = "[LomoEffect] DisplayThread";
        mIsRunning = false;
        mId = id;
    }

    ~DisplayThread() {
         ALOGI("[%s]", __func__);
    }

    virtual void requestExit() {
         mIsRunning = false;
    }

    void start() {
        run(mName.string(), PRIORITY_URGENT_DISPLAY, 0);
    }

private:
    String8 mName;
    bool mIsRunning;
    uint32_t mId;

    virtual void onFirstRef() {
        ALOGI("[%s]", __func__);
        mIsRunning = true;
    }

    virtual status_t readyToRun() {
        return NO_ERROR;
    }

    virtual bool threadLoop() {
        #ifdef MTK_CAM_LOMO_SUPPORT     
        LomoTimer ldispLoopTmr("lmjava dispLoop",g_displayIndex*g_effectNumOfPage,1);            
        #endif    	
        if (false == mIsRunning) {
            ALOGI("[%s] thread is going to stop", __func__);
            Mutex::Autolock l(g_mutex);
            g_condition.broadcast();
            return false;
        }

        JNIEnv *env = AndroidRuntime::getJNIEnv();
        int step = g_effectNumOfPage;
        int startPoint = g_displayIndex * step;
        EffectBuffer *top_buffer = g_EffectBuffers.itemAt(startPoint);


        while (top_buffer->m_dirty == true && mIsRunning == true) {
            ALOGI("block:%d is not update, sleep 3*1000 ms", g_displayIndex);
            usleep(3 * 1000);
        }        
        ALOGI("start display this frame");
        for (int i = startPoint; i < startPoint + step; i++) {
            if (false == mIsRunning) {
                break;
            }
            EffectBuffer *buffer = g_EffectBuffers.itemAt(i);
            if (*(g_currentEffectIds + (i % step)) == buffer->m_effectId
                    && buffer->m_effectId >= 0) {
                displayEffectEx((uint8_t*)buffer->m_bffer, i % step);  /// take buffer display out
            }
        }
        #ifdef MTK_CAM_LOMO_SUPPORT   
        if(bFpsDumpEnable)
        {
            ldispLoopTmr.fps(step);
        }
        ldispLoopTmr.printTime();
        #endif
        ALOGI("end display this frame");
        top_buffer->m_dirty = true;
        g_displayIndex = ( g_displayIndex + 1 ) % g_numOfBufferBlock;

        if (g_displayFrame == 0) {
            gettimeofday(&tv,NULL);
            g_display_startTime = tv.tv_sec * 1000000 + tv.tv_usec;
        }
        g_displayFrame ++;
        if (g_displayFrame % 20 == 0) {
            gettimeofday(&tv2,NULL);
            g_display_endTime = tv2.tv_sec * 1000000 + tv2.tv_usec;
            ALOGI("displayFps = %llu\n", 20 * 1000000 / (g_display_endTime - g_display_startTime));
            g_display_startTime = g_display_endTime;
        }

        return true;
    }
};
sp<DisplayThread> g_displayThread;

void SetupWindow(ANativeWindow *w)
{
    ALOGI("[%s]", __func__);
    YUVData &np = gYUVData;
    native_window_api_connect(w, NATIVE_WINDOW_API_MEDIA);
    ALOGI("nativeWindow size, width:%d, height:%d", np.width, np.height);
    native_window_set_buffers_dimensions(w, np.width, np.height);
    native_window_set_buffers_format(w, HAL_PIXEL_FORMAT_YV12);
    native_window_set_usage(w, GRALLOC_USAGE_SW_WRITE_OFTEN | GRALLOC_USAGE_HW_TEXTURE);
    native_window_set_scaling_mode(w, NATIVE_WINDOW_SCALING_MODE_SCALE_TO_WINDOW);
    native_window_set_buffer_count(w, 5);
}

void setup(JNIEnv *env, jobject thiz, jobject weak_this)
{
    ALOGD("setup()");
    mLomoEffectWeakObject = env->NewGlobalRef(weak_this);
    jclass clazz = env->FindClass("com/mediatek/matrixeffect/MatrixEffect");
    mLomoEffectClass = (jclass)env->NewGlobalRef(clazz);
    mPostEventFromNativeMethodId = env->GetStaticMethodID(clazz, "postEventFromNative", "(Ljava/lang/Object;I)V");
}

void setSurfaceToNative(JNIEnv *env, jobject thiz, jobject jsurface, jint surfaceNumber)
{
    sp<Surface> surface = NULL;
    if (jsurface != NULL) {
        ALOGI("begin convert jaftersurface to native, surfaceNumber:%d", surfaceNumber);
        surface = android_view_Surface_getSurface(env, jsurface);
        if (surface == NULL) {
            ALOGE("surface is null!");
        }
        NativeWindow *nativeWindow = g_NativeWindows.itemAt(surfaceNumber);
        nativeWindow->m_nativeWindow = surface.get();

        YUVData &yuv_data = gYUVData;
        yuv_data.width      = g_bufferWidth;
        yuv_data.height     = g_bufferHeight;
        yuv_data.size       = yuv_data.width * yuv_data.height * 3 / 2;
        yuv_data.ysize      = yuv_data.width * yuv_data.height;
        yuv_data.uvsize     = yuv_data.width * yuv_data.height / 4;
        SetupWindow(nativeWindow->m_nativeWindow);
        nativeWindow->m_isReady = true;
        ALOGI("end convert jaftersurface to native");
    }
}

void displayEffect(JNIEnv *env, jobject thiz, jbyteArray yuvData, jint surfaceNumber)
{
}

void initializeEffect(JNIEnv *env, jclass thiz, jint previewWidth, jint previewHeight,
        jint effectNumOfPage, jint format)
{
    ALOGD("initializeEffect()");
    // output: preview size, preview buffer format
    g_previewWidth = previewWidth;
    g_previewHeight = previewHeight;
    g_bufferFormat = format;
    g_previewBufferSize = (g_previewWidth * g_previewHeight * 3 /2);
    g_effectNumOfPage = effectNumOfPage;
    g_currentEffectIds = (int *)malloc(sizeof(int) * g_effectNumOfPage);
    for (int i = 0; i < g_effectNumOfPage; i++) {
        *(g_currentEffectIds + i) = i;
    }

    for (int i = 0; i < g_effectNumOfPage; i++) {
        NativeWindow *window = (NativeWindow*)malloc(sizeof(NativeWindow));
        window->m_nativeWindow = NULL;
        window->m_isReady = false;
        g_NativeWindows.push(window);
    }
    g_displayIndex = 0;
    g_displayFrame = 0;
    g_inputFrame = 0;
    g_bufferCurson = 0 ;
    u32lEffectIdxCount = 0;
    g_isFirstFrameDisplayed = false;
    g_displayThread = new DisplayThread(0);    
    #ifdef MTK_CAM_LOMO_SUPPORT
    mpLomoHalJni = LomoHalJni::createInstance();
    mpLomoHalJni->init();
    char valueAddrDump[PROPERTY_VALUE_MAX] = {'\0'};    
    property_get("lomoeffect.jni.addrdump", valueAddrDump, "0");        
    bAddrDumpEnable = atoi(valueAddrDump);    
    char valueFpsDump[PROPERTY_VALUE_MAX] = {'\0'};    
    property_get("lomoeffect.jni.fpsdump", valueFpsDump, "0");        
    bFpsDumpEnable = atoi(valueFpsDump); 
    
    #endif
}

IonInfo* allocateIon(int length) {
    int ion_handle = mt_ion_open("matrix_effect");
    ion_user_handle_t ion_allocHandle;
    int32_t ionFD;
    ion_alloc_mm(ion_handle, length, 1, 0, &ion_allocHandle);
    ion_share(ion_handle, ion_allocHandle, &ionFD);
    void *virtualAddr = ion_mmap(ion_handle, 0, length,
            PROT_READ | PROT_WRITE, MAP_SHARED, ionFD, 0);
    IonInfo *info = (IonInfo *)malloc(sizeof(IonInfo));
    info->mIonHandle = ion_handle;
    info->mIonAllocHandle = ion_allocHandle;
    info->mIonFD = ionFD;
    info->mVirtualAddr = virtualAddr;
    info->mSize = length;
    return info;
}

void destroyIon(IonInfo * info) {
    ion_munmap(info->mIonHandle, info->mVirtualAddr, info->mSize);
    ion_share_close(info->mIonHandle, info->mIonFD);
    ion_free(info->mIonHandle, info->mIonAllocHandle);
}

void registerEffectBuffers(JNIEnv *env, jclass thiz, jint bufferWidth, jint bufferHeight, jobjectArray buffers)
{
    ALOGI("[%s] + ", __func__);
    g_bufferWidth = bufferWidth;
    g_bufferHeight = bufferHeight;
    g_effectBufferSize = g_bufferWidth * g_bufferHeight * 3 / 2;
    ALOGD("g_bufferWidth,g_bufferHeight %d,%d",g_bufferWidth,g_bufferHeight);
    int row = env->GetArrayLength(buffers);
    ALOGD("row Row:%d", row);
    for (int i = 0; i < row; i++) {
        jbyteArray bufferArray = (jbyteArray)env->GetObjectArrayElement(buffers, i);
        IonInfo *ionInfo = allocateIon(g_effectBufferSize);
        g_IonInfos.push(ionInfo);
        EffectBuffer *buffer = (EffectBuffer *)malloc(sizeof(EffectBuffer));
        buffer->m_bffer = ionInfo->mVirtualAddr;
        buffer->m_dirty = true;
        buffer->m_effectId = -1;
        buffer->m_id = ionInfo->mIonFD;
        g_EffectBuffers.push(buffer);
    }
    g_numOfBufferBlock = row / g_effectNumOfPage;

    #ifdef MTK_CAM_LOMO_SUPPORT
    #ifdef MTK_CAM_LOMO_NOT_LEGACY
    ALOGD("MTK_CAM_LOMO_NOT_LEGACY");
    #else
    ALOGD("MTK_CAM_LOMO_IS_LEGACY");
    #endif
    mpLomoHalJni->AllocLomoSrcImage(g_previewWidth, g_previewHeight,\
         g_bufferFormat , \
        g_bufferWidth, g_bufferHeight);    
    #endif
    ALOGI("[%s] - ", __func__);
}

void processEffect(JNIEnv *env, jclass thiz, jbyteArray data, jintArray ids)
{
    ALOGI("[%s] + ", __func__);
    jbyte* previewBuffer = (jbyte*)env->GetByteArrayElements(data, NULL);
    jint* effectIds = (jint*)env->GetIntArrayElements(ids, NULL);
    int size = env->GetArrayLength(ids);
    #ifdef MTK_CAM_LOMO_SUPPORT
    ALOGD("previewBuffer 0x%x,0x%p", (unsigned char*)previewBuffer, (void*)previewBuffer);
    mpLomoHalJni->UploadLomoSrcImage((uint8_t*)previewBuffer);
    #endif

    for (int i = 0; i < size; i++) {
        int effectId = effectIds[i];
        *(g_currentEffectIds + i) = effectId;
    }
    
    writeEffectDataToBuffers((uint8_t *)previewBuffer, g_effectBufferSize, effectIds);
    env->ReleaseByteArrayElements(data, previewBuffer, JNI_ABORT);
    env->ReleaseIntArrayElements(ids, effectIds, JNI_ABORT);
    g_displayThread->start();
    ALOGI("[%s] - ", __func__);
}

// release resource
void releaseEffect(JNIEnv *env, jclass thiz)
{
    ALOGI("[%s] + ", __func__);

    g_displayThread->requestExit();
    if (g_displayThread->isRunning()) {
        Mutex::Autolock l(g_mutex);
        g_condition.wait(g_mutex);
    }

    while (!g_EffectBuffers.isEmpty()) {
        EffectBuffer *buffer = g_EffectBuffers.top();
        g_EffectBuffers.pop();
        free(buffer);
    }

    while (!g_NativeWindows.isEmpty()) {
        NativeWindow *window = g_NativeWindows.top();
        if (window->m_nativeWindow) {
            native_window_api_disconnect(window->m_nativeWindow, NATIVE_WINDOW_API_MEDIA);
        }
        g_NativeWindows.pop();
        free(window);
    }

    while (!g_IonInfos.isEmpty()) {
        IonInfo *info = g_IonInfos.top();
        destroyIon(info);
        g_IonInfos.pop();
        free(info);
    }

    free(g_currentEffectIds);
    #ifdef MTK_CAM_LOMO_SUPPORT
    mpLomoHalJni->FreeLomoSrcImage();
    mpLomoHalJni->FreeLomoDstImage();
    mpLomoHalJni->uninit();
    mpLomoHalJni = NULL;
    #endif
    ALOGI("[%s] - ", __func__);
}


const char *classPathName = "com/mediatek/matrixeffect/MatrixEffect";

JNINativeMethod methods[] = {
    {"native_setup", "(Ljava/lang/Object;)V", (void*)setup},
    {"native_setSurfaceToNative", "(Landroid/view/Surface;I)V", (void*)setSurfaceToNative },
    {"native_displayEffect", "([BI)V",(void*)displayEffect},
    {"native_initializeEffect", "(IIII)V", (void*)initializeEffect},
    {"native_registerEffectBuffers", "(II[[B)V",(void*)registerEffectBuffers},
    {"native_processEffect", "([B[I)V", (void*)processEffect},
    {"native_releaseEffect", "()V",(void*)releaseEffect},
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

/*
 * Register native methods for all classes we know about.
 *
 * returns JNI_TRUE on success.
 */
static int registerNatives(JNIEnv* env)
{
  if (!registerNativeMethods(env, classPathName,
                 methods, sizeof(methods) / sizeof(methods[0]))) {
    return JNI_FALSE;
  }

  return JNI_TRUE;
}


// ----------------------------------------------------------------------------

/*
 * This is called by the VM when the shared library is first loaded.
 */
 
typedef union {
    JNIEnv* env;
    void* venv;
} UnionJNIEnvToVoid;

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    UnionJNIEnvToVoid uenv;
    uenv.venv = NULL;
    jint result = -1;
    JNIEnv* env = NULL;
    
    ALOGI("JNI_OnLoad");

    if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK) {
        ALOGE("ERROR: GetEnv failed");
        goto bail;
    }
    env = uenv.env;

    if (registerNatives(env) != JNI_TRUE) {
        ALOGE("ERROR: registerNatives failed");
        goto bail;
    }
    
    result = JNI_VERSION_1_4;
    
bail:
    return result;
}

