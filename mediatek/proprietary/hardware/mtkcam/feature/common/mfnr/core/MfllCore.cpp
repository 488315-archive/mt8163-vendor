#define MTK_LOG_ENABLE 1
#include "MfllCore.h"
#include "MfllLog.h"

#include <utils/threads.h> // ANDROID_PRIORITY_FOREGROUND
#include <cutils/log.h> // XLOG
#include <sys/time.h>

#include <stdio.h>
#include <vector> // std::vector
#include <map> // std::map
#include <string> // std::string

using android::sp;
using android::Mutex;
using std::vector;
using std::map;
using std::string;
using namespace mfll;

#define LOG_TAG "MtkCam/MfllCore"

/* if MFLL_LOG is 1, give IMfllImageBuffers with name for debug usage */
#if MFLL_LOG
#define BUFFER_NAME(name, i)      (string(name) + to_char(i)).c_str()
#else
#define BUFFER_NAME(name, i)
#endif

inline char to_char(int i)
{
    char c = '0';
    return (c + i);
}

static map<enum EventType, const char*> EVENTTYPE_NAME_INITIALIZER(void)
{
    static std::map<enum EventType, const char*> m;
#define EVENTTYPE_NAME_ADD_ELEMENT(x) m[x] = #x
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_Init);
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_AllocateRawBuffer);
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_AllocateQyuvBuffer);
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_AllocateYuvGolden);
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_AllocateYuvWorking);
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_AllocateYuvMixing);
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_AllocateWeighting);
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_AllocateMemc);
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_AllocatePostview);
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_AllocateThumbnail);
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_AllocateJpeg);
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_Capture);
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_CaptureRaw);
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_CaptureYuvQ);
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_CaptureEis);
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_Bss);
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_EncodeYuvBase);
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_EncodeYuvGolden);
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_MotionEstimation);
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_MotionCompensation);
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_Blending);
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_Mixing);
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_NoiseReduction);
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_Postview);
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_Thumbnail);
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_EncodeJpeg);
    EVENTTYPE_NAME_ADD_ELEMENT(EventType_Destroy);
#undef EVENTTYPE_NAME_ADD_ELEMENT
    return m;
}
static map<enum EventType, const char*> EVENTTYPE_NAME = EVENTTYPE_NAME_INITIALIZER();

const pthread_attr_t PTHREAD_DEFAULT_ATTR = {
    0, NULL, 1024 * 1024, 4096, SCHED_OTHER, ANDROID_PRIORITY_FOREGROUND
};

/* This thread will allocate all buffers sequentially */
static void* thread_allocate_memory(void *arg)
{
    void *ret = 0;
    mfllFunctionIn();

    MfllCore *pCore = (MfllCore*)arg;
    int frameNum = (int)pCore->getBlendFrameNum();

    /* allocate raw buffers frames */
    for(int i = 0; i < frameNum; i++)
        pCore->doAllocRawBuffer((void*)(long)i);

    /* allocate QYUV buffers */
    for(int i = 0; i < frameNum; i++)
        pCore->doAllocQyuvBuffer((void*)(long)i);

    /* ME/MC working buffer */
    for(int i = 0; i < pCore->getMemcInstanceNum(); i++)
        pCore->doAllocMemcWorking((void*)(long)i);

    /* MC working buffer */
    pCore->doAllocYuvMcWorking(NULL);

    /* After BSS done, we can convert RAW to YUV base */
    pCore->doAllocYuvBase(NULL);

    /* YUV working buffer for blending stage */
    pCore->doAllocYuvWorking(NULL);

    /* Allocate weighting table */
    pCore->doAllocWeighting((void*)(long)0);
    pCore->doAllocWeighting((void*)(long)1);

    /* After BSS done, we can convert RAW to YUV golden too (is not urgent) */
    pCore->doAllocYuvGolden(NULL);

    /* Postview / thumbnail / JPEG related buffers */
    pCore->doAllocPostview(NULL);
    pCore->doAllocThumbnail(NULL);
    pCore->doAllocJpeg(NULL);

lbExit:
    pCore->decStrong(pCore);
    mfllFunctionOut();
    return ret;
}

/**
 *  Thread phase1 has responsibility to capture frames, execute BSS, and encode
 *  YUV base/golden frames.
 */
static void* thread_phase1(void *arg)
{
    void *ret = 0;
    mfllFunctionIn();
    MfllCore *c = (MfllCore*)arg;

    c->doCapture(NULL);
    c->doBss(NULL);
    c->doEncodeYuvBase(NULL);
    c->doEncodeYuvGolden(NULL);

    c->decStrong(c);
    mfllFunctionOut();
    return ret;
}

/**
 *  Since our hardware only provides that only one a pass2 driver, all the
 *  operation invokes pass2 driver will be processed in this thread.
 *
 *  Due to pass2 driver is a thread-safe and FIFO mechanism, every operation
 *  can be dispatched as a sub thread to ask pass2 driver for operation using
 *  greedy algorithm.
 */
static void* thread_phase2(void *arg)
{
    void *ret = 0;
    mfllFunctionIn();
    MfllCore *c = (MfllCore*)arg;
    int times = (int)c->getBlendFrameNum() - 1;

    for (int i = 0; i < times; i++)
        c->doBlending((void*)(long)i);

    c->doMixing(NULL);
    c->doNoiseReduction(NULL);
    c->doCreatePostview(NULL);
    c->doEncodeThumbnail(NULL);
    c->doEncodeJpeg(NULL);
    c->decStrong(c);
    mfllFunctionOut();
    return ret;
}

/**
 *  Thread motion estimation will be invoked parallelly. There will be
 *  (m_frameNum - 1) mfll_thread_me
 */
typedef struct _memc_attr {
    MfllCore *pCore;
    vector<int> indexQueue;
} memc_attr_t;

static void* thread_memc(void *arg_memc_attr)
{
    enum MfllErr err = MfllErr_Ok;
    memc_attr_t *pMemcAttr = (memc_attr_t*)arg_memc_attr;

    MfllCore *c = pMemcAttr->pCore;

    mfllFunctionIn();

    for (size_t i = 0; i < pMemcAttr->indexQueue.size(); i++ )
    {
        void *void_index = (void*)(long)pMemcAttr->indexQueue[i];
        err = c->doMotionEstimation(void_index);
        if (err != MfllErr_Ok) {
            mfllLogW("%s: ME(%d) failed, ignore MC and blending(%d)", __FUNCTION__, i, i);
            c->m_bypass.bypassMotionCompensation[i] = 1;
            c->m_bypass.bypassBlending[i] = 1;
        }

        err = pMemcAttr->pCore->doMotionCompensation(void_index);
        if (err != MfllErr_Ok) {
            mfllLogW("%s: MC(%d) failed, ignore blending(%d)", __FUNCTION__, i, i);
            c->m_bypass.bypassBlending[i] = 1; // mark no need blending
        }
    }

lbExit:
    mfllFunctionOut();
    return (void*)(long)err;

}

static void* thread_memc_parallel(void *arg)
{
    void *ret = 0;
    mfllFunctionIn();
    MfllCore *c = (MfllCore*)arg;

    int times = (int)c->getBlendFrameNum() - 1;
    int instanceNum = (int)c->getMemcInstanceNum();
    int threadsNum = (times <= instanceNum ? times : instanceNum);

    mfllLogD("%s: times to blend(%d), MEMC instanceNum(%d), threadsNum(%d)", __FUNCTION__, times, instanceNum, threadsNum);

    memc_attr_t *attrs = new memc_attr_t[threadsNum];
    pthread_t *pThreads = new pthread_t[threadsNum];
    pthread_attr_t pthreadAttr = PTHREAD_DEFAULT_ATTR;

    /* create threadsNum threads for executing ME/MC */
    for (int i = 0; i < times; i++) {
        attrs[i % threadsNum].indexQueue.push_back(i);
        attrs[i % threadsNum].pCore = c;
    }

    for (int i = 0; i < threadsNum; i++) {
        pthread_create((pThreads + i), &pthreadAttr, thread_memc, (void*)&attrs[i]);
    }

    /* sync threads */
    for (int i = 0; i < threadsNum; i++) {
        void *r;
        pthread_join(*(pThreads + i), (void**)&r);
    }

lbExit:
    delete [] pThreads;
    delete [] attrs;

    c->decStrong(c);
    mfllFunctionOut();
    return ret;
}

/* this thread will process ME/MC sequentially */
static void* thread_memc_seq(void *arg)
{
    enum MfllErr err = MfllErr_Ok;

    mfllFunctionIn();

    MfllCore *c = (MfllCore*)arg;
    int times = (int)c->getBlendFrameNum() - 1;

    memc_attr_t memcAttr;
    memcAttr.pCore = c;
    for (int i = 0; i < times; i++) {
        memcAttr.indexQueue.push_back(i);
    }

    err = (enum MfllErr)(long)thread_memc((void*)&memcAttr);

    c->decStrong(c);
    mfllFunctionOut();

    return (void*)(long)err;
}

/******************************************************************************
 * M F L L
 *****************************************************************************/
IMfllCore* IMfllCore::createInstance(void)
{
    return (IMfllCore*)new MfllCore;
}
/*
int             sensor_id;
int             exposure;
int             iso;
int             update_ae;
int             blend_num;
int             capture_num;
enum MfllMode   mfll_mode;
enum RwbMode    rwb_mode;
enum MemcMode   memc_mode;
int             dualphase_mode;
*/
enum MfllErr IMfllCore::getCaptureInfo(const struct MfllConfig *pCfgIn, struct MfllConfig *pCfgOut)
{
    MfllConfig_t cfg;

    cfg.exposure = pCfgIn->exposure;
    cfg.iso = pCfgIn->iso;
    cfg.update_ae = 0;
    cfg.capture_num = MFLL_CAPTURE_FRAME;
    cfg.blend_num = MFLL_BLEND_FRAME;
    cfg.rwb_mode = static_cast<enum RwbMode>(MFLL_RWB_SUPPORT_MODE);
    cfg.memc_mode = static_cast<enum MemcMode>(MFLL_MEMC_SUPPORT_MODE);
    cfg.dualphase_mode = MFLL_DUALPHASE_SUPPORT_MODE;

    /* If using AIS mode, need to update ae */
    if (pCfgIn->mfll_mode == MfllMode_ZsdAis || pCfgIn->mfll_mode == MfllMode_NormalAis)
        cfg.update_ae = 1;

    // TODO: check exposure/iso
    if (0) {
        /* No need to do MFNR */
        cfg.capture_num = 1;
        cfg.blend_num = 1;
    }

    *pCfgOut = cfg;

    return MfllErr_Ok;
}

const char* IMfllCore::getEventTypeName(enum EventType e)
{
    unsigned int index = (unsigned int)e;
    if (e < 0 || e >= (unsigned int)EventType_Size)
        return "Unknown_EventType";
    return EVENTTYPE_NAME[e];
}

bool IMfllCore::isZsdMode(const enum MfllMode &m)
{
    return (m == MfllMode_ZsdMfll || m == MfllMode_ZsdAis) ? true : false;
}

void IMfllCore::destroyInstance(void)
{
    decStrong((void*)this);
}

/**
 *  MfllCore
 */
MfllCore::MfllCore(void)
{
    mfllFunctionIn();

    /* modules */
    m_spCapturer = NULL;
    /* plug-in */
    m_spProperty = NULL;
    m_spDump = NULL;
    /* event dispatcher */
    m_event = IMfllEvents::createInstance();
    /* attributes */
    m_frameNum = MFLL_BLEND_FRAME;
    m_frameNumCaptured = MFLL_CAPTURE_FRAME;
    m_memcInstanceNum = 1;
    m_bShooted = false;
    m_sensorId = -1;
    m_mutexShoot = PTHREAD_MUTEX_INITIALIZER;
    m_privateData = NULL;
    m_privateDataSize = 0;
    m_isFullSizeMc = 0;

    /* set image buffer smart pointers to NULL */
    m_imgYuvBase = NULL;
    m_imgYuvGolden = NULL;
    m_imgYuvBlended = NULL;
    m_imgWeighting[0] = NULL;
    m_imgWeighting[1] = NULL;

    /* pointers of IMfllImageBuffers */
    m_ptrImgYuvBase = NULL;
    m_ptrImgYuvRef = NULL;
    m_ptrImgYuvGolden = NULL;
    m_ptrImgYuvBlended = NULL;
    m_ptrImgYuvMixed = NULL;
    m_ptrImgWeightingIn = NULL;
    m_ptrImgWeightingOut = NULL;
    m_ptrImgWeightingFinal = NULL;


    /* sync objects, lock as default */
    lockSyncObject(&m_syncCapture);
    lockSyncObject(&m_syncEncodeYuvBase);
    lockSyncObject(&m_syncEncodeYuvGolden);
    lockSyncObject(&m_syncBss);
    lockSyncObject(&m_syncMixing);
    lockSyncObject(&m_syncNoiseReduction);
    lockSyncObject(&m_syncPostview);
    lockSyncObject(&m_syncThumbnail);
    lockSyncObject(&m_syncEncJpeg);

    for (unsigned int i = 0; i < MFLL_MAX_FRAMES; i++) {
        /* set image buffer smart pointers = NULL */
        m_imgRaws[i] = NULL;
        m_imgYuvs[i] = NULL;
        m_imgQYuvs[i] = NULL;
        m_imgMemc[i] = NULL;
        /* sync object for operation */
        lockSyncObject(&m_syncCapturedRaw[i]);
        lockSyncObject(&m_syncCapturedYuvQ[i]);
        lockSyncObject(&m_syncMotionEstimation[i]);
        lockSyncObject(&m_syncMotionCompensation[i]);
        lockSyncObject(&m_syncBlending[i]);
    }

    mfllLogD("Create MfllCore version: %s",((std::string)getReversionString()).c_str());
    mfllFunctionOut();
}

MfllCore::~MfllCore(void)
{
    MfllErr err;
    int iResult;
    MfllEventStatus_t status;

    mfllFunctionIn();
    m_event->onEvent(EventType_Destroy, status, (void*)this);

    /* release sync data */
    if (m_privateData) {
        delete m_privateData;
        m_privateDataSize = 0;
    }

    m_event->doneEvent(EventType_Destroy, status, (void*)this);
    mfllFunctionOut();
}

enum MfllErr MfllCore::init(const MfllConfig_t *pCfg /* = 0 */)
{
    MfllErr err = MfllErr_Ok;
    MfllEventStatus_t status;

    mfllFunctionIn();

    pthread_mutex_lock(&m_mutexShoot);
    if (m_bShooted) {
        err = MfllErr_Shooted;
        goto lbExit;
    }

    m_event->onEvent(EventType_Init, status, (void*)this);

    /* assign pointers to real buffer */
    m_ptrImgYuvBase = &(m_imgYuvBase);
    m_ptrImgYuvGolden = &(m_imgYuvGolden);
    m_ptrImgYuvBlended = &(m_imgYuvBlended);
    m_ptrImgWeightingIn = &(m_imgWeighting[0]);
    m_ptrImgWeightingOut = &(m_imgWeighting[1]);

    /* check if using full size MC */
    if (m_width == m_qwidth && m_height == m_qheight) {
        m_isFullSizeMc = 1;
        /* make sure m_qwidth and m_qheight is saving quarter size */
        m_qwidth = m_width/2;
        m_qheight = m_height/2;
    }

    /* check parameter is ok or not */
    if (getBlendFrameNum() <= 0) {
        err = MfllErr_IllegalBlendFrameNum;
        mfllLogE("%s: blending frame number is <= 0", __FUNCTION__);
        goto lbExit;
    }

    if (getCaptureFrameNum() <= 0 || getCaptureFrameNum() < getBlendFrameNum()) {
        err = MfllErr_IllegalCaptureFrameNum;
        mfllLogE("%s: catpure frame num is illegal.", __FUNCTION__);
        goto lbExit;
    }

    /* install property plug-in */
    m_spProperty = new MfllFeatureProperty;
    registerEventListenerNoLock(m_spProperty); // the order is not important for this plug-in

lbExit:
    pthread_mutex_unlock(&m_mutexShoot);

    status.err = err;
    m_event->doneEvent(EventType_Init, status, (void*)this);

    if (err == MfllErr_Shooted)
        mfllLogW("%s: MFLL has shooted, cannot init MFLL anymore", __FUNCTION__);

    mfllFunctionOut();
    return err;
}

enum MfllErr MfllCore::doMfll(MfllConfig_t cfg)
{
    MfllErr err = MfllErr_Ok;
    std::vector<pthread_t> vThreadToJoin;
    pthread_t pThread;
    pthread_t pThreadPhase1;
    void *ret;
    std::vector<pthread_t>::iterator itr;
    int registerMrpFeature = 0;


    /* function pointer to memc */
    void* (*ptr_thread_memc)(void*) = NULL;

    /* only Dual Phase may invoke Mfll async */
    bool bIsAsync = (cfg.dualphase_mode != 0 ? true : false);

    mfllFunctionIn();

    /* init first */
    init();

    /* use mutex to protect operating shoot state */
    pthread_mutex_lock(&m_mutexShoot);
    if (m_bShooted) {
        pthread_mutex_unlock(&m_mutexShoot);
        mfllLogW("MFLL has been shooted, cancel this shoot");
        err = MfllErr_Shooted;
        goto lbExit;
    }
    else {
        m_bShooted = true;
        pthread_mutex_unlock(&m_mutexShoot);
    }

    /**
     *  M F L L    S T A R T
     */
    /* Update catprue frame number and blend frame number */
    m_frameNum = cfg.blend_num;
    m_frameNumCaptured = cfg.capture_num;
    m_sensorId = cfg.sensor_id;
    m_shotMode = static_cast<enum MfllMode>(cfg.mfll_mode);
    m_rwbMode = cfg.rwb_mode;
    m_mrpMode = cfg.mrp_mode;
    m_memcMode = cfg.memc_mode;
    m_dualphaseMode = cfg.dualphase_mode;
    registerMrpFeature = (cfg.mrp_mode == MrpMode_BestPerformance ? 0 : 1);

    /* register Memory Reduce Plan feature */
    //if (cfg.mrp_mode != MrpMode_BestPerformance) {
    if (registerMrpFeature) {
        mfllLogD("Register plug-in Mrp");
        registerEventListenerNoLock(new MfllFeatureMrp, 1); // insert the event listener at the first
    }

    /* if any dump is enable, install feature dump */
    if (m_spProperty->isDump()) {
        mfllLogD("Register plug-in Dump");
        MfllFeatureDump::DumpFlag f;
        f.raw = m_spProperty->isDumpRaw()           ? 1 : 0;
        f.yuv = m_spProperty->isDumpYuv()           ? 1 : 0;
        f.mfb = m_spProperty->isDumpMfb()           ? 1 : 0;
        f.mix = m_spProperty->isDumpMix()           ? 1 : 0;
        f.jpeg = m_spProperty->isDumpJpeg()         ? 1 : 0;
        f.postview = m_spProperty->isDumpPostview() ? 1 : 0;
        f.exif = m_spProperty->isDumpExif()         ? 1 : 0;
        m_spDump = new MfllFeatureDump;
        m_spDump->setDumpFlag(f);
        /* configure dump base on property */
        registerEventListenerNoLock(m_spDump, 1);
    }



    /* assign ME/MC thread according memc_mode */
    switch(cfg.memc_mode){
        case MemcMode_Sequential:
            ptr_thread_memc = &thread_memc_seq;
            break;
        case MemcMode_Parallel:
            /**
             *  Using (number of blending - 1) as MEMC threads num
             *  or the default should be m_memcInstanceNum = MFLL_MEMC_THREADS_NUM;
             */
            m_memcInstanceNum = getBlendFrameNum() - 1;
            ptr_thread_memc = &thread_memc_parallel;
            break;
    }

    /**
     *  If using single shot.
     */
    if (getCaptureFrameNum() <= 1 || getBlendFrameNum() <= 1) {
        doCapture(0);
        doSingleShot(0);
        goto lbExit;
    }

    /* create MEMC instance */
    for (size_t i = 0; i < getMemcInstanceNum(); i++) {
        m_spMemc[i] = IMfllMemc::createInstance();
        m_spMemc[i]->setMotionEstimationResolution(m_qwidth, m_qheight);
        if (m_isFullSizeMc)
            m_spMemc[i]->setMotionCompensationResolution(m_width, m_height);
        else
            m_spMemc[i]->setMotionCompensationResolution(m_qwidth, m_qheight);
        if (m_spMemc[i]->init() != MfllErr_Ok) {
            mfllLogE("%s: init MfllMemc failed with code", __FUNCTION__);
        }
    }

    /* print out configuration */
    {
        mfllLogD("=========  M    F    L    L ============");
        mfllLogD("sensor id         = %d", m_sensorId);
        mfllLogD("capture num       = %d", m_frameNumCaptured);
        mfllLogD("blend-frame       = %d", m_frameNum);
        mfllLogD("full size         = %dx%d", m_width, m_height);
        mfllLogD("quater size       = %dx%d", m_qwidth, m_qheight);
        mfllLogD("full size MC      = %d", m_isFullSizeMc);
        mfllLogD("iso               = %d", m_iso);
        mfllLogD("exposure          = %d", m_exposure);
        mfllLogD("memc mode         = %s", m_memcMode == MemcMode_Sequential ? "sequential" : "parallel");
        mfllLogD("memc threads      = %d", m_memcInstanceNum);
        mfllLogD("rwb mode          = %d", m_rwbMode);
        mfllLogD("dualphase enable  = %d", m_dualphaseMode);
        mfllLogD("feature dump      = %s", m_spProperty->isDump() ? "Yes" : "No");
        if (m_spProperty->isDump()) {
        mfllLogD("feature dump.raw  = %s", m_spProperty->isDumpRaw() ? "Yes" : "No");
        mfllLogD("feature dump.yuv  = %s", m_spProperty->isDumpYuv() ? "Yes" : "No");
        mfllLogD("feature dump.mfb  = %s", m_spProperty->isDumpMfb() ? "Yes" : "No");
        mfllLogD("feature dump.mix  = %s", m_spProperty->isDumpMix() ? "Yes" : "No");
        mfllLogD("feature dump.jpeg = %s", m_spProperty->isDumpJpeg() ? "Yes" : "No");
        mfllLogD("feature dump.pv   = %s", m_spProperty->isDumpPostview() ? "Yes" : "No");
        mfllLogD("feature dump.exif = %s", m_spProperty->isDumpExif() ? "Yes" : "No");
        }
        mfllLogD("feature mrp       = %d", registerMrpFeature);
        mfllLogD("========================================");
    }

    /**
     *  M F L L    T H R E A D S
     */
    /**
     *  thread to allocate buffers sequentially, if using MRP, we don't create
     *  this thread to allocate image buffer.
     */
    if (registerMrpFeature != 0) {
        createThread(&pThread, thread_allocate_memory);
        vThreadToJoin.push_back(pThread);
    }

    /* thread for capture image->bss->raw2yuv */
    createThread(&pThreadPhase1, thread_phase1);

    /* thread to do ME&MC */
    createThread(&pThread, (*ptr_thread_memc));
    vThreadToJoin.push_back(pThread);

    /* thread for blending->mixing->NR->Postview->JPEG */
    createThread(&pThread, thread_phase2);
    vThreadToJoin.push_back(pThread);

    /**
     *  Ok, here we need to sync threads.
     */
    /* Phase1 should be always joined. */
    err = joinThread(&pThreadPhase1);

    /* if not async call, we have to join these threads */
    for( itr = vThreadToJoin.begin(); itr != vThreadToJoin.end(); itr++) {
        pthread_t t = (*itr);

        if (bIsAsync == false) {
            pthread_join(t, &ret);
        }
        else {
            pthread_detach(t);
        }
    }

lbExit:
    mfllFunctionOut();
    return err;
}

int MfllCore::getSensorId(void)
{
    return m_sensorId;
}

unsigned int MfllCore::getReversion(void)
{
    unsigned int v0;
    unsigned int v1;
    unsigned int v2;

    v0 = MFLL_CORE_REVERSION_BUGFIX;
    v1 = MFLL_CORE_REVERSION_MINOR;
    v2 = MFLL_CORE_REVERSION_MAJOR;

    return (v2 << 20) | (v1 << 12) | v0;
}

std::string MfllCore::getReversionString(void)
{
    char memchunk[32];
    sprintf(memchunk, "%d.%d.%d", MFLL_CORE_REVERSION_MAJOR, MFLL_CORE_REVERSION_MINOR, MFLL_CORE_REVERSION_BUGFIX);
    return std::string(memchunk);
}

bool MfllCore::isShooted(void)
{
    bool b = false;
    pthread_mutex_lock(&m_mutexShoot);
    b = m_bShooted;
    pthread_mutex_unlock(&m_mutexShoot);
    return b;
}

bool MfllCore::isFullSizeMc(void)
{
    bool b = false;
    pthread_mutex_lock(&m_mutexShoot);
    b = m_bShooted;
    pthread_mutex_unlock(&m_mutexShoot);

    return b ? (m_isFullSizeMc == 1) : false;
}

enum MfllErr MfllCore::registerEventListener(const sp<IMfllEventListener> &e)
{
    bool b = false;

    pthread_mutex_lock(&m_mutexShoot);
    b = m_bShooted;
    pthread_mutex_unlock(&m_mutexShoot);

    if (b) {
        mfllLogW("%s: Mfll is shooted, ignored.", __FUNCTION__);
        return MfllErr_Shooted;
    }

    return registerEventListenerNoLock(e);
}

enum MfllErr MfllCore::registerEventListenerNoLock(const sp<IMfllEventListener> &e, int useInsert /* = 0 */)
{
    mfllFunctionIn();

    vector<enum EventType> t = e->getListenedEventTypes();
    for (size_t i = 0; i < t.size(); i++) {
        if (useInsert == 0)
            m_event->registerEventListener(t[i], e);
        else
            m_event->registerEventListenerAsFirst(t[i], e);
    }

    mfllFunctionOut();
    return MfllErr_Ok;
}


enum MfllErr MfllCore::removeEventListener(IMfllEventListener *e)
{
    bool b = false;

    pthread_mutex_lock(&m_mutexShoot);
    b = m_bShooted;
    pthread_mutex_unlock(&m_mutexShoot);

    if (b) {
        mfllLogW("%s: Mfll is shooted, ignored.", __FUNCTION__);
        return MfllErr_Shooted;
    }

    vector<enum EventType> t = e->getListenedEventTypes();
    for (size_t i = 0; i < t.size() ; i++) {
        m_event->removeEventListener(t[i], e);
    }
    return MfllErr_Ok;
}

enum MfllErr MfllCore::setBypassOption(const MfllBypassOption_t &b)
{
    bool shooted = false;

    pthread_mutex_lock(&m_mutexShoot);
    shooted = m_bShooted;
    pthread_mutex_unlock(&m_mutexShoot);

    if (shooted) {
        mfllLogW("%s: Mfll is shooted, ignored.", __FUNCTION__);
        return MfllErr_Shooted;
    }

    m_bypass = b;

    return MfllErr_Ok;
}

enum MfllErr MfllCore::setCaptureResolution(unsigned int width, unsigned int height)
{
    enum MfllErr err = MfllErr_Ok;

    pthread_mutex_lock(&m_mutexShoot);
    if (m_bShooted) {
        err = MfllErr_Shooted;
        goto lbExit;
    }

    m_width = width;
    m_height = height;

lbExit:
    pthread_mutex_unlock(&m_mutexShoot);
    if (err == MfllErr_Shooted)
        mfllLogW("MFLL has shooted, cannot set resolution");
    return err;
}

enum MfllErr MfllCore::setMfb(const sp<IMfllMfb> &mfb)
{
    enum MfllErr err = MfllErr_Ok;
    pthread_mutex_lock(&m_mutexShoot);
    if (m_bShooted) {
        err = MfllErr_Shooted;
        goto lbExit;
    }

    m_spMfb = mfb;

lbExit:
    pthread_mutex_unlock(&m_mutexShoot);
    if (err == MfllErr_Shooted)
        mfllLogW("MFLL has shooted, cannot set resolution");
    return err;
}

sp<IMfllMfb> MfllCore::getMfb(void)
{
    return m_spMfb;
}

enum MfllErr MfllCore::setCaptureQResolution(unsigned int qwidth, unsigned int qheight)
{
    enum MfllErr err = MfllErr_Ok;

    pthread_mutex_lock(&m_mutexShoot);
    if (m_bShooted) {
        err = MfllErr_Shooted;
        goto lbExit;
    }

    m_qwidth = qwidth;
    m_qheight = qheight;

lbExit:
    pthread_mutex_unlock(&m_mutexShoot);
    if (err == MfllErr_Shooted)
        mfllLogW("MFLL has shooted, cannot set Q resolution");
    return err;
}

enum MfllErr MfllCore::setCapturer(const sp<IMfllCapturer> &capturer)
{
    enum MfllErr err = MfllErr_Ok;

    pthread_mutex_lock(&m_mutexShoot);
    if (m_bShooted) {
        err = MfllErr_Shooted;
        goto lbExit;
    }

    m_spCapturer = capturer;

lbExit:
    pthread_mutex_unlock(&m_mutexShoot);
    if (err == MfllErr_Shooted)
        mfllLogW("MFLL has shooted, cannot set Q resolution");
    return err;
}

sp<IMfllImageBuffer> MfllCore::retrieveBuffer(const enum MfllBuffer &s, int index /* = 0 */)
{
    enum MfllErr err = MfllErr_Ok;
    sp<IMfllImageBuffer> p = NULL;
    switch (s) {
    case MfllBuffer_Raw:
        if (index < 0 || index >= getCaptureFrameNum()) {
            mfllLogE("%s: get raw buffer but index is wrong (index, max) = (%d, %d)", __FUNCTION__, index, getCaptureFrameNum());
            err = MfllErr_BadArgument;
            goto lbExit;
        }
        p = m_imgRaws[index];
        break;
    case MfllBuffer_QYuv:
        if (index < 0 || index >= getCaptureFrameNum()) {
            mfllLogE("%s: get qyuv buffer but index is wrong (index, max) = (%d, %d)", __FUNCTION__, index, getCaptureFrameNum());
            err = MfllErr_BadArgument;
            goto lbExit;
        }
        p = m_imgQYuvs[index];
        break;
    case MfllBuffer_AlgorithmWorking:
        if (index < 0 || index >= (getCaptureFrameNum()-1)) {
            mfllLogE("%s: get algorithm working buffer but index is wrong (index, max) = (%d, %d)", __FUNCTION__, index, getCaptureFrameNum());
            err = MfllErr_BadArgument;
            goto lbExit;
        }
        p = m_imgMemc[index];
        break;
    case MfllBuffer_FullSizeYuv:
        if (index < 0 || index >= getCaptureFrameNum()) {
            mfllLogE("%s: get full size YUV buffer but index is wrong (index, max) = (%d, %d)", __FUNCTION__, index, getCaptureFrameNum());
            err = MfllErr_BadArgument;
            goto lbExit;
        }
        p = m_imgYuvs[index];
        break;
    case MfllBuffer_BaseYuv:
        p = (*m_ptrImgYuvBase);
        break;
    case MfllBuffer_GoldenYuv:
        p = (*m_ptrImgYuvGolden);
        break;
    case MfllBuffer_ReferenceYuv:
        p = (*m_ptrImgYuvRef);
        break;
    case MfllBuffer_BlendedYuv:
        p = (*m_ptrImgYuvBlended);
        break;
    case MfllBuffer_MixedYuv:
        p = (*m_ptrImgYuvMixed);
        break;
    case MfllBuffer_WeightingIn:
        p = (*m_ptrImgWeightingIn);
        break;
    case MfllBuffer_WeightingOut:
        p = (*m_ptrImgWeightingOut);
        break;
    default:
        mfllLogE("%s: unsupport buffer %d", __FUNCTION__, (int)s);
        err = MfllErr_NotSupported;
        goto lbExit;
    }
lbExit:
    return p;
}

enum MfllErr MfllCore::releaseBuffer(const enum MfllBuffer &s, int index /* = 0 */)
{
    enum MfllErr err = MfllErr_Ok;
    switch (s) {
    case MfllBuffer_Raw:
        if (index < 0 || index >= getCaptureFrameNum()) {
            mfllLogE("%s: raw buffer index is out of range (index, max) = (%d, %d)", __FUNCTION__, index, getCaptureFrameNum());
            err = MfllErr_BadArgument;
            goto lbExit;
        }
        m_imgRaws[index] = 0;
        break;
    case MfllBuffer_QYuv:
        if (index < 0 || index >= getCaptureFrameNum()) {
            mfllLogE("%s: qyuv buffer index is out of range (index, max) = (%d, %d)", __FUNCTION__, index, getCaptureFrameNum());
            err = MfllErr_BadArgument;
            goto lbExit;
        }
        m_imgQYuvs[index] = 0;
        break;
    case MfllBuffer_AlgorithmWorking:
        if (index < 0 || index >= (getCaptureFrameNum()-1)) {
            mfllLogE("%s: algorithm working buffer but index is wrong (index, max) = (%d, %d)", __FUNCTION__, index, getCaptureFrameNum());
            err = MfllErr_BadArgument;
            goto lbExit;
        }
        m_imgMemc[index] = 0;
        break;
    case MfllBuffer_FullSizeYuv:
        if (index < 0 || index >= getCaptureFrameNum()) {
            mfllLogE("%s: qyuv buffer index is out of range (index, max) = (%d, %d)", __FUNCTION__, index, getCaptureFrameNum());
            err = MfllErr_BadArgument;
            goto lbExit;
        }
        m_imgYuvs[index] = 0;
        goto lbExit;
        break;
    case MfllBuffer_BaseYuv:
        (*m_ptrImgYuvBase) = 0;
        break;
    case MfllBuffer_GoldenYuv:
        (*m_ptrImgYuvGolden) = 0;
        break;
    case MfllBuffer_ReferenceYuv:
        (*m_ptrImgYuvRef) = 0;
        break;
    case MfllBuffer_BlendedYuv:
        (*m_ptrImgYuvBlended) = 0;
        break;
    case MfllBuffer_MixedYuv:
        (*m_ptrImgYuvMixed) = 0;
        break;
    case MfllBuffer_WeightingIn:
        (*m_ptrImgWeightingIn) = 0;
        break;
    case MfllBuffer_WeightingOut:
        (*m_ptrImgWeightingOut) = 0;
        break;
    default:
        mfllLogE("%s: unsupport buffer %d", __FUNCTION__, (int)s);
        err = MfllErr_NotSupported;
        goto lbExit;
    }
lbExit:
    return err;
}


/**
 *  ---------------------------------------------------------------------------
 *  MFLL OPERATIONS
 *
 *  These functions are operations that MFLL will invoke. Function name template
 *  is do{$action} where {$action} is the meaniful name for the operation
 *  -----------------------------------------------------------------------------
 */

/**
 *  The first operation of MFLL is to capture frames, but it's necessary to wait
 *  RAW buffers and QYUV buffers for capture are ready.
 *
 *  We have to capture getBlendFrameNum() RAW and QYUV frames by IMfllCapturer
 */
enum MfllErr MfllCore::doCapture(JOB_VOID)
{
    enum MfllErr err = MfllErr_Ok;
    MfllEventStatus_t status;
    mfllFunctionIn();

    /* invokes events */
    m_event->onEvent(EventType_Capture, status, this);

    if (m_bypass.bypassCapture || status.ignore) {
        mfllLogD("%s: Bypass capture operation", __FUNCTION__);
        usleep(500 * 1000); // assume 500 ms
        status.ignore = 1;
    }
    else {
        /**
         *  N O T I C E
         *
         *  Aquire buffers should be invoked by MfllCapturer
         *  1. IMfllCore::doAllocRawBuffer
         *  2. IMfllCore::doAllocQyuvBuffer
         */
        /* check if IMfllCapturer has been assigned */
        if (m_spCapturer.get() == NULL) {
            mfllLogD("%s: create MfllCapturer", __FUNCTION__);
            m_spCapturer = IMfllCapturer::createInstance();
            if (m_spCapturer.get() == NULL) {
                mfllLogE("%s: create MfllCapturer instance", __FUNCTION__);
                err = MfllErr_CreateInstanceFailed;
                goto lbExit;
            }
        }

        vector< sp<IMfllImageBuffer>* > raws;
        vector< sp<IMfllImageBuffer>* > yuvs;
        vector< sp<IMfllImageBuffer>* > qyuvs;
        vector<MfllMotionVector_t*> gmvs;

        /* prepare output buffer */
        for (int i = 0; i < (int)getCaptureFrameNum(); i++) {
            raws.push_back(&m_imgRaws[i]);
            qyuvs.push_back(&m_imgQYuvs[i]);
            gmvs.push_back(&m_globalMv[i]);

            if (m_isFullSizeMc)
                yuvs.push_back(&m_imgYuvs[i]);
        }

        /* register event dispatcher and set MfllCore instance */
        m_spCapturer->setMfllCore((IMfllCore*)this);
        err = m_spCapturer->registerEventDispatcher(m_event);

        if (err != MfllErr_Ok) {
            mfllLogE("%s: MfllCapture::registerEventDispatcher failed with code %d", __FUNCTION__, err);
            goto lbExit;
        }

        /* Catpure frames */
        mfllLogD("Capture frames!");

        if (m_isFullSizeMc)
            err = m_spCapturer->captureFrames(getCaptureFrameNum(), raws, yuvs, qyuvs, gmvs);
        else
            err = m_spCapturer->captureFrames(getCaptureFrameNum(), raws, qyuvs, gmvs);
        if (err != MfllErr_Ok) {
            mfllLogE("%s: MfllCapture::captureFrames failed with code %d", __FUNCTION__, err);
            goto lbExit;
        }
    }

lbExit:
    handleError(err);
    status.err = err;
    m_event->doneEvent(EventType_Capture, status, this);
    syncAnnounceDone(&m_syncCapture);
    mfllFunctionOut();
    return err;
}

/**
 *  Best Shot Selection should takes captured images
 */
enum MfllErr MfllCore::doBss(JOB_VOID)
{
    enum MfllErr err = MfllErr_Ok;
    MfllEventStatus_t status;

    mfllFunctionIn();
    /* conditions */
    syncWaitDone(&m_syncCapture);
    /* trigger events */
    m_event->onEvent(EventType_Bss, status, this);

    if (m_bypass.bypassBss || status.ignore) {
        mfllLogD("%s: Bypass bss", __FUNCTION__);
        usleep(40 * 1000);
        status.ignore = 1;
    }
    else {
        mfllAutoTraceFunc();

        sp<IMfllBss> bss = IMfllBss::createInstance();
        if (bss.get() == NULL) {
            mfllLogE("%s: create IMfllBss instance fail", __FUNCTION__);
            goto lbExit;
        }

        std::vector< sp<IMfllImageBuffer> > imgs;
        std::vector< MfllMotionVector_t >   mvs;
        for (size_t i = 0; i < getCaptureFrameNum(); i++) {
            imgs.push_back(m_imgQYuvs[i]);
            mvs.push_back(m_globalMv[i]);
        }

        std::vector<int> newIndex = bss->bss(imgs, mvs);
        if (newIndex.size() <= 0) {
            mfllLogE("%s: do bss failed", __FUNCTION__);
            err = MfllErr_UnexpectedError;
            goto lbExit;
        }

        // sort items
        std::vector< sp<IMfllImageBuffer> > newRaws;
        std::vector< sp<IMfllImageBuffer> > newQYuvs;
        std::vector< sp<IMfllImageBuffer> > newYuvs;
        for (size_t i = 0; i < getCaptureFrameNum(); i++) {
            int index = newIndex[i]; // new index
            newRaws.push_back(m_imgRaws[index]);
            newQYuvs.push_back(m_imgQYuvs[index]);
            newYuvs.push_back(m_imgYuvs[index]);
            mfllLogD("%s: new index (%d)-->(%d)", __FUNCTION__, i, index);
        }
        for (size_t i = 0; i < getCaptureFrameNum(); i++) {
            m_imgRaws[i] = newRaws[i];
            m_imgQYuvs[i] = newQYuvs[i];
            m_imgYuvs[i] = newYuvs[i];
        }
    }

lbExit:
    handleError(err);
    status.err = err;
    m_event->doneEvent(EventType_Bss, status, this);
    syncAnnounceDone(&m_syncBss);
    mfllFunctionOut();
    return err;
}

/**
 *  To encode base RAW buffer to YUV base buffer. This operation must be invoked
 *  after doBss() has done and YUV base buffer is available to use. Hence the
 *  conditions are:
 *
 *  1. Buffer of YUV base is ready
 *  2. BSS has been done
 */
enum MfllErr MfllCore::doEncodeYuvBase(JOB_VOID)
{
    enum MfllErr err = MfllErr_Ok;
    MfllEventStatus_t status;

    mfllFunctionIn();
    /* conditions */
    syncWaitDone(&m_syncBss);
    /* invokes events */
    m_event->onEvent(EventType_EncodeYuvBase, status, this);

    if (m_bypass.bypassEncodeYuvBase || status.ignore) {
        mfllLogD("%s: Bypass encoding YUV base", __FUNCTION__);
        usleep(50 * 1000);
        status.ignore = 1;
    }
    else {
        err = doAllocYuvBase(NULL);
        if (err != MfllErr_Ok) {
            mfllLogE("%s: allocate YUV base failed", __FUNCTION__);
            err = MfllErr_UnexpectedError;
            goto lbExit;
        }

        if (m_spMfb.get() == NULL) {
            mfllLogE("%s: MfllMfb is NULL, cannot encode", __FUNCTION__);
            status.ignore = 1;
            goto lbExit;
        }

        m_spMfb->init(m_sensorId);
        err = m_spMfb->setSyncPrivateData(m_privateData, m_privateDataSize);
        mfllLogD("%s: encode base raw to yuv", __FUNCTION__);
        err = m_spMfb->encodeRawToYuv(m_imgRaws[0].get(), m_imgYuvBase.get(), YuvStage_BaseYuy2);
        if (err != MfllErr_Ok) {
            mfllLogE("%s: Encode RAW to YUV fail");
            goto lbExit;
        }
    }

lbExit:
    status.err = err;
    m_event->doneEvent(EventType_EncodeYuvBase, status, this);
    syncAnnounceDone(&m_syncEncodeYuvBase);
    mfllFunctionOut();
    return err;
}

/**
 *  To encode base RAW buffer to YUV golden buffer. This operation must be invoked
 *  after doBss() has done and YUV golden buffer is available to use.
 *
 *  1. Buffer of YUV golden is ready
 *  2. BSS has been done
 */
enum MfllErr MfllCore::doEncodeYuvGolden(JOB_VOID)
{
    enum MfllErr err = MfllErr_Ok;
    MfllEventStatus_t status;

    mfllFunctionIn();
    /* conditions */
    syncWaitDone(&m_syncBss);

    /* invokes events */
    m_event->onEvent(EventType_EncodeYuvGolden, status, this); // invokes events

    if (m_bypass.bypassEncodeYuvGolden || status.ignore) {
        mfllLogD("%s: Bypass encoding YUV golden", __FUNCTION__);
        usleep(50 * 1000); // 50 ms
        status.ignore = 1;
    }
    else {
        err = doAllocYuvGolden(NULL);
        if (err != MfllErr_Ok) {
            mfllLogE("%s: allocate YUV golden failed", __FUNCTION__);
            err = MfllErr_UnexpectedError;
            goto lbExit;
        }

        if (m_spMfb.get() == NULL) {
            mfllLogE("%s: MfllMfb is NULL, cannot encode", __FUNCTION__);
            status.ignore = 1;
            goto lbExit;
        }

        m_spMfb->init(m_sensorId);
        err = m_spMfb->setSyncPrivateData(m_privateData, m_privateDataSize);
        err = m_spMfb->encodeRawToYuv(m_imgRaws[0].get(), m_imgYuvGolden.get(), YuvStage_GoldenYuy2);
        if (err != MfllErr_Ok) {
            mfllLogE("%s: Encode RAW to YUV fail");
            goto lbExit;
        }
    }

lbExit:
    status.err = err;
    m_event->doneEvent(EventType_EncodeYuvGolden, status, this);
    syncAnnounceDone(&m_syncEncodeYuvGolden);
    mfllFunctionOut();
    return err;
}

/**
 *  Calculate motion estimation using MfllMemc. This operation must be invoked
 *  while these conditions are matched:
 *
 *  1. BSS has been done
 */
enum MfllErr MfllCore::doMotionEstimation(void *void_index)
{
    unsigned int index = (unsigned int)(long)void_index;
    enum MfllErr err = MfllErr_Ok;
    MfllEventStatus_t status;

    mfllFunctionInInt(index);
    /* conditions */
    syncWaitDone(&m_syncBss);
    /* invokes event */
    m_event->onEvent(EventType_MotionEstimation, status, this, (void*)(long)index);

    if (m_bypass.bypassMotionEstimation[index] || status.ignore) {
        mfllLogD("%s: Bypass motion estimation(%d)", __FUNCTION__, index);
        usleep(100 * 1000); // 100 ms
        status.ignore = 1;
    }
    else {
        mfllAutoLog((std::string("start ME")+to_char(index)).c_str());
        unsigned int memcIndex = index % getMemcInstanceNum();
        sp<IMfllMemc> memc = m_spMemc[memcIndex];
        if (memc.get() == NULL) {
            mfllLogE("%s: MfllMemc is necessary to be created first (index=%d)", __FUNCTION__, index);
            err = MfllErr_NullPointer;
            goto lbExit;
        }

        err = doAllocMemcWorking((void*)(long)memcIndex);
        if (err != MfllErr_Ok) {
            mfllLogE("%s: allocate MEMC working buffer(%d) failed", __FUNCTION__, memcIndex);
            err = MfllErr_UnexpectedError;
            goto lbExit;
        }

        memc->setAlgorithmWorkingBuffer(m_imgMemc[memcIndex]);
        memc->setMeBaseImage(m_imgQYuvs[0]);
        memc->setMeRefImage(m_imgQYuvs[index + 1]);

        err = memc->motionEstimation();
        if (err != MfllErr_Ok) {
            memc->giveupMotionCompensation();
            mfllLogE("%s: IMfllMemc::motionEstimation failed, returns %d", __FUNCTION__, (int)err);
            goto lbExit;
        }
    }

lbExit:
    status.err = err;
    m_event->doneEvent(EventType_MotionEstimation, status, this, (void*)(long)index);
    syncAnnounceDone(&m_syncMotionEstimation[index]);
    mfllFunctionOut();
    return err;
}

/**
 *  Motion compensation is a pure software algorithm, the i-th motion compensation
 *  should be executed if only if:
 *
 *  1. The i-th motion estimation has been done.
 *  2. The (i-1)-th compensation has been done (if i > 1)
 */
enum MfllErr MfllCore::doMotionCompensation(void *void_index)
{
    unsigned int index = (unsigned int)(long)void_index;
    enum MfllErr err = MfllErr_Ok;
    MfllEventStatus_t status;

    mfllFunctionInInt(index);
    /* conditions */
    syncWaitDone(&m_syncMotionEstimation[index]);
    if (index > 0)
        syncWaitDone(&m_syncMotionCompensation[index - 1]);
    /* trigger events */
    m_event->onEvent(EventType_MotionCompensation, status, this, (void*)(long)index);

    if (m_bypass.bypassMotionCompensation[index] || status.ignore) {
        mfllLogD("%s: Bypass motion compensation(%d)", __FUNCTION__, index);
        usleep(50 * 1000); // 50 ms
        status.ignore = 1;
    }
    else {
        unsigned int memcIndex = index % getMemcInstanceNum();
        sp<IMfllMemc> memc = m_spMemc[memcIndex];
        /* using full size YUV if is using full size MC */
        sp<IMfllImageBuffer> *pMcRef = (m_isFullSizeMc == 0)
            ? &m_imgQYuvs[index + 1]
            : &m_imgYuvs[index +1];

        if (memc.get() == NULL) {
            mfllLogE("%s: MfllMemc is necessary to be created first (index=%d)", __FUNCTION__, index);
            err = MfllErr_NullPointer;
            goto lbExit;
        }

        /* check if we need to do MC or not */
        if (memc->isIgnoredMotionCompensation()) {
            mfllLogD("%s: ingored motion compensation & blending", __FUNCTION__);
            m_bypass.bypassBlending[index] = 1;
            goto lbExit;
        }

        /* allocate YUV MC working buffer if necessary */
        err = doAllocYuvMcWorking(NULL);
        if (err != MfllErr_Ok) {
            mfllLogE("%s: allocate YUV MC working buffer failed", __FUNCTION__);
            goto lbExit;
        }

        /* setting up mc */
        memc->setMcRefImage(*pMcRef);
        memc->setMcDstImage(m_imgYuvMcWorking);

        err = memc->motionCompensation();
        if (err != MfllErr_Ok) {
            mfllLogE("%s: IMfllMemc::motionCompensation failed, returns %d", __FUNCTION__, (int)err);
            goto lbExit;
        }

        /* exchange buffer from Dst->Src */
        {
            sp<IMfllImageBuffer> _t = *pMcRef;
            *pMcRef = m_imgYuvMcWorking;
            m_imgYuvMcWorking = _t;
        }

        /* convert image format to Yuy2 without alignment */
        (*pMcRef)->setAligned(2, 2);
        (*pMcRef)->convertImageFormat(ImageFormat_Yuy2);
    }

lbExit:
    status.err = err;
    m_event->doneEvent(EventType_MotionCompensation, status, this, (void*)(long)index);
    syncAnnounceDone(&m_syncMotionCompensation[index]);
    mfllFunctionOut();
    return err;
}

/**
 *  Blending using MTK hardware, furthurmore, operation blending depends on that
 *  the previous blended output. Therefore, the conditions of this operaion are:
 *
 *  1. YUV working buffer for blending is ready.
 *  2. The i-th motion compensation has been done.
 *  3. The (i-1)-th blending has been done.
 */
enum MfllErr MfllCore::doBlending(void *void_index)
{
    unsigned int index = (unsigned int)(long)void_index;
    enum MfllErr err = MfllErr_Ok;
    MfllEventStatus_t status;

    mfllFunctionInInt(index);
    /* conditions */
    syncWaitDone(&m_syncMotionCompensation[index]);
    if (index > 0)
        syncWaitDone(&m_syncBlending[index - 1]);

    /* setting up m_ptrImgXXX pointers */
    if (index > 0) {

        Mutex::Autolock _l(m_lockAccessPtrImgBlended);

        /* ping-pong YUV base <--> YUV blended */
        sp<IMfllImageBuffer> *pbuf = m_ptrImgYuvBase;
        m_ptrImgYuvBase = m_ptrImgYuvBlended;
        m_ptrImgYuvBlended = m_ptrImgYuvBase;

        pbuf = m_ptrImgWeightingIn;
        m_ptrImgWeightingIn = m_ptrImgWeightingOut;
        m_ptrImgWeightingOut = pbuf;
    }
    else {

        Mutex::Autolock _l(m_lockAccessPtrImgBlended);

        m_ptrImgYuvBase = &(m_imgYuvBase);
        m_ptrImgYuvBlended = &(m_imgYuvBlended);
        m_ptrImgWeightingIn = &(m_imgWeighting[0]);
        m_ptrImgWeightingOut = &(m_imgWeighting[1]);
    }


    m_ptrImgYuvRef = m_isFullSizeMc ? &(m_imgYuvs[index + 1]) : &(m_imgQYuvs[index +1]);

    /**
     *  before blending, set final weighting table as input weighting table,
     *  if blending ok, m_ptrImgWeightingFinal will be assigned to
     *  m_ptrImgWeightingOut
     */
    m_ptrImgWeightingFinal = m_ptrImgWeightingIn;


    /* trigger events */
    m_event->onEvent(EventType_Blending, status, this, (void*)(long)index);

    if (m_bypass.bypassBlending[index] || status.ignore) {
        mfllLogD("%s: Bypass blending(%d)", __FUNCTION__, index);
        usleep(50 * 1000);
        status.ignore = 1;

        /* If ignored, ping-pong back to the original */
        if (index > 0) {

            Mutex::Autolock _l(m_lockAccessPtrImgBlended);

            /* ping-pong YUV base <--> YUV blended */
            sp<IMfllImageBuffer> *pbuf = m_ptrImgYuvBase;
            m_ptrImgYuvBase = m_ptrImgYuvBlended;
            m_ptrImgYuvBlended = pbuf;

            pbuf = m_ptrImgWeightingIn;
            m_ptrImgWeightingIn = m_ptrImgWeightingOut;
            m_ptrImgWeightingOut = pbuf;
        }
    }
    else {
        err = doAllocYuvWorking(NULL);
        if (err != MfllErr_Ok) {
            mfllLogE("%s: allocate YUV working buffer failed", __FUNCTION__);
            goto lbExit;
        }

        err = doAllocWeighting((void*)(long)0);
        err = doAllocWeighting((void*)(long)1);
        if (err != MfllErr_Ok) {
            mfllLogE("%s: allocate weighting buffer 0 or 1 failed", __FUNCTION__);
            goto lbExit;
        }

        if (m_spMfb.get() == NULL) {
            mfllLogD("%s: create IMfllMfb instance", __FUNCTION__);
            m_spMfb = IMfllMfb::createInstance();
            if (m_spMfb.get() == NULL) {
                mfllLogE("%s: m_spMfb is NULL", __FUNCTION__);
                err = MfllErr_CreateInstanceFailed;
                goto lbExit;
            }
        }

        err = m_spMfb->init(m_sensorId); // TODO: pass in the correct sensor ID
        err = m_spMfb->setSyncPrivateData(m_privateData, m_privateDataSize);

        if (err != MfllErr_Ok) {
            mfllLogE("%s: m_spMfb init failed with code %d", __FUNCTION__, (int)err);
            goto lbExit;
        }

        m_lockAccessPtrImgBlended.lock();
        /* do blending */
        mfllLogD("%s: do blending now", __FUNCTION__);

        /**
         * while index == 0, which means the first time to blend, the input weighting
         * table should be sent
         */
        err = m_spMfb->blend(
            (*m_ptrImgYuvBase).get(),
            (*m_ptrImgYuvRef).get(),
            (*m_ptrImgYuvBlended).get(),
            (index == 0) ? NULL : (*m_ptrImgWeightingIn).get(),
            (*m_ptrImgWeightingOut).get()
        );

        m_lockAccessPtrImgBlended.unlock();

        if (err != MfllErr_Ok) {
            mfllLogE("%s: Mfb failed with code %d", __FUNCTION__, (int)err);
            goto lbExit;
        }
        else {
            /* save for mixing */
            m_ptrImgWeightingFinal = m_ptrImgWeightingOut;
        }
    }

lbExit:
    status.err = err;
    m_event->doneEvent(EventType_Blending, status, this, (void*)(long)index);


    syncAnnounceDone(&m_syncBlending[index]);
    mfllFunctionOut();
    return err;
}

/**
 *  Mixing a blended frame and a golden frame. Therefore, we must have a blended
 *  frame which means that this operation must wait until blending done. Hence
 *  the conditions are
 *
 *  1. YUV mixing output frame buffer ready
 *  2. All blending has been done.
 */
enum MfllErr MfllCore::doMixing(JOB_VOID)
{
    enum MfllErr err = MfllErr_Ok;
    MfllEventStatus_t status;

    mfllFunctionIn();
    /* conditions */
    for (int i = 0; i < (int)getBlendFrameNum() - 1; i++)
        syncWaitDone(&m_syncBlending[i]);

    /* re-use YUV base frame as output */
    m_ptrImgYuvMixed = m_ptrImgYuvBase;

    /* trigger events */
    m_event->onEvent(EventType_Mixing, status, this);

    /* check if all blending are ignored, if yes, ignore mix stage too */
    do {
        for (size_t i = 0; i < (size_t)getBlendFrameNum() - 1; i++) {
            if (m_bypass.bypassBlending[i] == 0) {
                break;
            }
        }
        mfllLogW("%s: bypass mixing stage due to no blending applied", __FUNCTION__);
        m_bypass.bypassMixing = 1;
    } while(0);

    if (m_bypass.bypassMixing || status.ignore) {
        mfllLogD("%s: Bypass mixing", __FUNCTION__);
        usleep(50 * 1000);
        status.ignore = 1;
    }
    else {
        if (m_spMfb.get() == NULL) {
            m_spMfb = IMfllMfb::createInstance();
            if (m_spMfb.get() == NULL) {
                mfllLogE("%s: create MFB instance failed", __FUNCTION__);
                err = MfllErr_CreateInstanceFailed;
                goto lbExit;
            }
        }

        /* set sensor ID and private data for sync */
        err = m_spMfb->init(m_sensorId);
        err = m_spMfb->setSyncPrivateData(m_privateData, m_privateDataSize);

        if (err != MfllErr_Ok) {
            mfllLogE("%s: init MFB instance failed with code %d", __FUNCTION__, (int)err);
            goto lbExit;
        }

        /* mixing */
        mfllLogD("%s: do mixing", __FUNCTION__);
        err = m_spMfb->mix(
            (*m_ptrImgYuvBlended).get(),
            (*m_ptrImgYuvGolden).get(),
            (*m_ptrImgYuvMixed).get(),
            (*m_ptrImgWeightingFinal).get()
        );
    }

lbExit:
    if (status.ignore || err != MfllErr_Ok) {
        if (err != MfllErr_Ok)
            mfllLogE("%s: mix failed with code %d, using YUV golden as output", __FUNCTION__, err);

        /* using golden YUV as output */
        sp<IMfllImageBuffer> tp = *m_ptrImgYuvMixed;
        *m_ptrImgYuvMixed = *m_ptrImgYuvGolden;
        *m_ptrImgYuvGolden = tp;
    }

    status.err = err;
    m_event->doneEvent(EventType_Mixing, status, this);
    syncAnnounceDone(&m_syncMixing);
    mfllFunctionOut();
    return err;
}

/**
 *  Noise reduction is processed after mixing has been done. So the conditions
 *  are
 *
 *  1. YUV mixing has been done
 */
enum MfllErr MfllCore::doNoiseReduction(JOB_VOID)
{
    enum MfllErr err = MfllErr_Ok;
    MfllEventStatus_t status;

    mfllFunctionIn();
    /* conditions */
    syncWaitDone(&m_syncMixing);
    /* trigger events */
    m_event->onEvent(EventType_NoiseReduction, status, this);

    if (m_bypass.bypassNoiseReduction || status.ignore) {
        mfllLogD(":s Bypass noise reduction", __FUNCTION__);
        usleep(170 * 1000);
        status.ignore = 1;
    }
    else {
        // TODO: do NR
    }

lbExit:
    status.err = err;
    m_event->doneEvent(EventType_NoiseReduction, status, this);
    syncAnnounceDone(&m_syncNoiseReduction);
    mfllFunctionOut();
    return err;
}

/**
 *  After all post-processings are done, creating Postview for application.
 *  We notic that the last post-processing is Noise Reduction and which cannot
 *  be processed parallelly therefore we can assume the conditions for this
 *  funcion are
 *
 *  1. Noise Reduction has been done.
 *  2. Postview related buffers are ready.
 */
enum MfllErr MfllCore::doCreatePostview(JOB_VOID)
{
    enum MfllErr err = MfllErr_Ok;
    MfllEventStatus_t status;

    mfllFunctionIn();
    /* conditions */
    syncWaitDone(&m_syncNoiseReduction);
    /* trigger events */
    m_event->onEvent(EventType_Postview, status, this);

    if (m_bypass.bypassPostview || status.ignore) {
        mfllLogD("%s: Bypass create postview", __FUNCTION__);
        usleep(10 * 1000);
        status.ignore = 1;
    }
    else {
        // TODO: create postview
    }

lbExit:
    handleError(err);
    status.err = err;
    m_event->doneEvent(EventType_Postview, status, this);
    syncAnnounceDone(&m_syncPostview);
    mfllFunctionOut();
    return err;
}

/**
 *  Thumbnail can be created after all the post-processings are done, and buffers
 *  for Thumbnail are ready. Hence conditions are
 *
 *  1. Noise Reduction has been done.
 *  2. Thumbnail related buffers are ready.
 */
enum MfllErr MfllCore::doEncodeThumbnail(JOB_VOID)
{
    enum MfllErr err = MfllErr_Ok;
    MfllEventStatus_t status;

    mfllFunctionIn();
    /* conditions */
    syncWaitDone(&m_syncNoiseReduction);
    /* trigger events */
    m_event->onEvent(EventType_Thumbnail, status, this);

    if (m_bypass.bypassThumbnail || status.ignore) {
        mfllLogD("%s: Bypass create thumbnail", __FUNCTION__);
        usleep(25 * 1000);
        status.ignore = 1;
    }
    else {
        // TODO: create thumbnail.
    }

lbExit:
    status.err = err;
    m_event->doneEvent(EventType_Thumbnail, status, this);
    syncAnnounceDone(&m_syncThumbnail);
    mfllFunctionOut();
    return err;
}

/**
 *  JPEG can be encoded after all the post-processings are done, and buffers
 *  for encoding JPEG are ready. Hence the conditions are
 *
 *  1. Noise Reduction has been done.
 *  2. JPEG encoding related buffers are ready.
 */
enum MfllErr MfllCore::doEncodeJpeg(JOB_VOID)
{
    enum MfllErr err = MfllErr_Ok;
    MfllEventStatus_t status;

    mfllFunctionIn();
    /* conditions */
    syncWaitDone(&m_syncNoiseReduction);
    /* trigger events */
    m_event->onEvent(EventType_EncodeJpeg, status, this);

    if (m_bypass.bypassEncodeJpeg || status.ignore) {
        mfllLogD("%s: Bypass encoding JPEG", __FUNCTION__);
        usleep(70 * 1000);
        status.ignore = 1;
    }
    else {
    }

lbExit:
    status.err = err;
    m_event->doneEvent(EventType_EncodeJpeg, status, this);
    syncAnnounceDone(&m_syncEncJpeg);
    mfllFunctionOut();
    return MfllErr_NotImplemented;
}

enum MfllErr MfllCore::doSingleShot(JOB_VOID)
{
    enum MfllErr err = MfllErr_Ok;
    MfllEventStatus_t status;

    mfllAutoLogFunc();
    /* conditions */
    syncWaitDone(&m_syncCapture);
//    m_event->onEvent(EventType_Mixing, status, this);

    if (m_bypass.bypassMixing || status.ignore) {
    }
    else {
        err = doAllocYuvGolden(NULL);
        if (err != MfllErr_Ok) {
            mfllLogE("%s: allocate YUV golden failed", __FUNCTION__);
            err = MfllErr_UnexpectedError;
            goto lbExit;
        }

        if (m_spMfb.get() == NULL) {
            mfllLogE("%s: MfllMfb is NULL, cannot encode", __FUNCTION__);
            status.ignore = 1;
            goto lbExit;
        }

        m_ptrImgYuvMixed = &(m_imgYuvGolden);

        m_spMfb->init(m_sensorId);
        err = m_spMfb->setSyncPrivateData(m_privateData, m_privateDataSize);
        err = m_spMfb->encodeRawToYuv(m_imgRaws[0].get(), (*m_ptrImgYuvMixed).get(), YuvStage_GoldenYuy2);
        if (err != MfllErr_Ok) {
            mfllLogE("%s: Encode RAW to YUV fail");
            goto lbExit;
        }
    }

lbExit:
    status.err = err;
//    m_event->doneEvent(EventType_EncodeJpeg, status, this);
    syncAnnounceDone(&m_syncEncJpeg); // TODO: add m_syncSingleShot
    return err;
}

enum MfllErr MfllCore::doAllocRawBuffer(void *void_index)
{
    unsigned int index = (unsigned int)(long)void_index;
    enum MfllErr err = MfllErr_Ok;
    MfllEventStatus_t status;

    Mutex::Autolock _l(m_lockAllocRaws[index]);

    mfllFunctionIn();
    /* trigger events */
    m_event->onEvent(EventType_AllocateRawBuffer, status, this, (void*)(long)index);

    if (m_bypass.bypassAllocRawBuffer[index] || status.ignore) {
        mfllLogD("%s: Bypass allocate raw buffers", __FUNCTION__);
        usleep(50 * 1000);
        status.ignore = 1;
    }
    else {
        /* create IMfllImageBuffer instances */
        IMfllImageBuffer *pImg = m_imgRaws[index].get();
        if (pImg == NULL) {
            pImg = IMfllImageBuffer::createInstance(BUFFER_NAME("raw_", index));
            if (pImg == NULL) {
                mfllLogE("%s: create IMfllImageBuffer(%d) failed", __FUNCTION__, index);
                err = MfllErr_CreateInstanceFailed;
                goto lbExit;
            }
            m_imgRaws[index] = pImg;
        }

        /* if not init, init it */
        if (!pImg->isInited()) {
            pImg->setImageFormat(ImageFormat_Raw10);
            pImg->setResolution(m_width, m_height);
            err = pImg->initBuffer();

            if (err != MfllErr_Ok) {
                mfllLogE("%s: init raw buffer(%d) failed", __FUNCTION__, index);
                err = MfllErr_UnexpectedError;
                goto lbExit;
            }
        }
        else {
            status.ignore = 1;
            mfllLogD("%s: raw buffer %d is inited, ignore here", __FUNCTION__, index);
        }
    }

lbExit:
    status.err = err;
    m_event->doneEvent(EventType_AllocateRawBuffer, status, this, (void*)(long)index);
    mfllFunctionOut();
    return err;
}

enum MfllErr MfllCore::doAllocQyuvBuffer(void *void_index)
{
    unsigned int index = (unsigned int)(long)void_index;
    enum MfllErr err = MfllErr_Ok;
    MfllEventStatus_t status;

    Mutex::Autolock _l(m_lockAllocQYuvs[index]);

    mfllFunctionIn();
    /* trigger events */
    m_event->onEvent(EventType_AllocateQyuvBuffer, status, this, (void*)(long)index);

    if (m_bypass.bypassAllocQyuvBuffer[index] || status.ignore) {
        mfllLogD("%s: Bypass allocate QYUV buffers", __FUNCTION__);
        usleep(50 * 1000);
        status.ignore = 1;
    }
    else {
        IMfllImageBuffer *pImg = m_imgQYuvs[index].get();
        IMfllImageBuffer *pFullSizeImg = m_imgYuvs[index].get();

        if (pImg == NULL) {
            pImg = IMfllImageBuffer::createInstance(BUFFER_NAME("qyuv_", index));
            if (pImg == NULL) {
                mfllLogE("%s: create QYUV buffer instance (%d) failed", __FUNCTION__, index);
                err = MfllErr_CreateInstanceFailed;
                goto lbExit;
            }
            m_imgQYuvs[index] = pImg;
        }

        /* full size YUV */
        if (m_isFullSizeMc && pFullSizeImg == NULL) {
            pFullSizeImg = IMfllImageBuffer::createInstance(BUFFER_NAME("fyuv_", index));
            if (pFullSizeImg == NULL) {
                mfllLogE("%s: create full size YUV buffer instance (%d) failed", __FUNCTION__, index);
                err = MfllErr_CreateInstanceFailed;
                goto lbExit;
            }
            m_imgYuvs[index] = pFullSizeImg;
        }

        if (!pImg->isInited()) {
            pImg->setImageFormat(m_isFullSizeMc ? ImageFormat_Y8 : ImageFormat_Yv16);
            pImg->setAligned(16, 16);
            pImg->setResolution(m_qwidth, m_qheight);
            err = pImg->initBuffer();
            if (err != MfllErr_Ok) {
                mfllLogE("%s: init QYUV buffer(%d) failed", __FUNCTION__, index);
                goto lbExit;
            }
        }
        else {
            status.ignore = 1;
            mfllLogD("%s: qyuv buffer %d is inited, ignore here", __FUNCTION__, index);
        }

        /* full size YUV */
        if (m_isFullSizeMc && pFullSizeImg->isInited() == false) {
            pFullSizeImg->setImageFormat(ImageFormat_Yv16);
            pFullSizeImg->setAligned(16, 16);
            pFullSizeImg->setResolution(m_width, m_height);
            err = pFullSizeImg->initBuffer();
            if (err != MfllErr_Ok) {
                mfllLogE("%s: init full size YUV buffer (%d) failed", __FUNCTION__, index);
                goto lbExit;
            }
        }
        else {
            status.ignore = 1;
            mfllLogD("%s: qyuv buffer %d is inited, ignore here", __FUNCTION__, index);
        }
    }

lbExit:
    status.err = err;
    m_event->doneEvent(EventType_AllocateQyuvBuffer, status, this, (void*)(long)index);
    mfllFunctionOut();
    return err;
}

enum MfllErr MfllCore::doAllocYuvBase(JOB_VOID)
{
    enum MfllErr err = MfllErr_Ok;
    MfllEventStatus_t status;

    mfllFunctionIn();

    Mutex::Autolock _l(m_lockAllocYuvBase);

    /* trigger events */
    m_event->onEvent(EventType_AllocateYuvBase, status, this);

    if (m_bypass.bypassAllocYuvBase || status.ignore) {
        mfllLogD("%s: Bypass allocate YUV base buffer", __FUNCTION__);
        usleep(100 * 1000);
        status.ignore = 1;
    }
    else {

        IMfllImageBuffer *pImg = m_imgYuvBase.get();
        if (pImg == NULL) {
            pImg = IMfllImageBuffer::createInstance(BUFFER_NAME("yuv_base", 0));
            if (pImg == NULL) {
                mfllLogE("%s: create YUV base buffer instance failed", __FUNCTION__);
                err = MfllErr_CreateInstanceFailed;
                goto lbExit;
            }
            m_imgYuvBase = pImg;
        }

        if (!pImg->isInited()) {
            pImg->setImageFormat(ImageFormat_Yuy2);
            pImg->setResolution(m_width, m_height);
            err = pImg->initBuffer();

            if (err != MfllErr_Ok) {
                mfllLogE("%s: init YUV base buffer failed", __FUNCTION__);
                err = MfllErr_UnexpectedError;
                goto lbExit;
            }
        }
        else {
            status.ignore = 1;
            mfllLogD("%s: yuv base is inited, ignored here", __FUNCTION__);
        }
    }

lbExit:
    status.err = err;
    m_event->doneEvent(EventType_AllocateYuvBase, status, this);
    mfllFunctionOut();
    return err;
}


enum MfllErr MfllCore::doAllocYuvGolden(JOB_VOID)
{
    enum MfllErr err = MfllErr_Ok;
    MfllEventStatus_t status;

    mfllFunctionIn();

    Mutex::Autolock _l(m_lockAllocYuvGolden);

    /* trigger events */
    m_event->onEvent(EventType_AllocateYuvGolden, status, this);

    if (m_bypass.bypassAllocYuvGolden || status.ignore) {
        mfllLogD("%s: Bypass allocate YUV golen buffer", __FUNCTION__);
        usleep(100 * 1000);
        status.ignore = 1;
    }
    else {
        IMfllImageBuffer *pImg = m_imgYuvGolden.get();
        if (pImg == NULL) {
            pImg = IMfllImageBuffer::createInstance(BUFFER_NAME("yuv_golden", 0));
            if (pImg == NULL) {
                mfllLogE("%s: create YUV golden instance failed", __FUNCTION__);
                err = MfllErr_CreateInstanceFailed;
                goto lbExit;
            }
            m_imgYuvGolden = pImg;
        }

        if (!pImg->isInited()) {
            pImg->setImageFormat(ImageFormat_Yuy2);
            pImg->setResolution(m_width, m_height);
            err = pImg->initBuffer();
            if (err != MfllErr_Ok) {
                mfllLogE("%s: init YUV golden buffer failed", __FUNCTION__);
                err = MfllErr_UnexpectedError;
                goto lbExit;
            }
        }
        else {
            status.ignore = 1;
            mfllLogD("%s: yuv golden is inited, ignored here", __FUNCTION__);
        }
    }

lbExit:
    status.err = err;
    m_event->doneEvent(EventType_AllocateYuvGolden, status, this);
    mfllFunctionOut();
    return err;
}

enum MfllErr MfllCore::doAllocYuvWorking(JOB_VOID)
{
    enum MfllErr err = MfllErr_Ok;
    MfllEventStatus_t status;

    Mutex::Autolock _l(m_lockAllocYuvWorking);

    mfllFunctionIn();
    /* trigger events */
    m_event->onEvent(EventType_AllocateYuvWorking, status, this);

    if (m_bypass.bypassAllocYuvWorking || status.ignore) {
        mfllLogD("%s: Bypass allocate YUV working(mixing) buffer", __FUNCTION__);
        usleep(100 * 1000);
        status.ignore = 1;
    }
    else {
        IMfllImageBuffer *pImg = m_imgYuvBlended.get();
        if (pImg == NULL) {
            pImg = IMfllImageBuffer::createInstance(BUFFER_NAME("yuv_blended",0));
            if (pImg == NULL) {
                mfllLogE("%s: create YUV blended buffer instance failed", __FUNCTION__);
                err = MfllErr_CreateInstanceFailed;
                goto lbExit;
            }
            m_imgYuvBlended = pImg;
        }

        if (!pImg->isInited()) {
            pImg->setImageFormat(ImageFormat_Yuy2);
            pImg->setResolution(m_width, m_height);
            err = pImg->initBuffer();
            if (err != MfllErr_Ok) {
                mfllLogE("%s: init YUV blended buffer failed", __FUNCTION__);
                err = MfllErr_UnexpectedError;
                goto lbExit;
            }
        }
        else {
            status.ignore = 1;
            mfllLogD("%s: yuv blending(working) is inited, ignored here", __FUNCTION__);
        }
    }

lbExit:
    status.err = err;
    m_event->doneEvent(EventType_AllocateYuvWorking, status, this);
    mfllFunctionOut();
    return err;
}

/**
 *  To allocate a YUV buffer (m_qwidth x m_qheight) for Motion Compensation working
 */
enum MfllErr MfllCore::doAllocYuvMcWorking(JOB_VOID)
{
    enum MfllErr err = MfllErr_Ok;
    MfllEventStatus_t status;

    Mutex::Autolock _l(m_lockAllocYuvMcWorking);

    mfllFunctionIn();
    /* trigger events */
    m_event->onEvent(EventType_AllocateYuvMcWorking, status, this);

    if (m_bypass.bypassAllocYuvMcWorking || status.ignore) {
        mfllLogD("%s: Bypass allocate YUV MC working buffer", __FUNCTION__);
        usleep(100 * 1000);
        status.ignore = 1;
    }
    else {
        mfllAutoTraceFunc();

        IMfllImageBuffer *pImg = m_imgYuvMcWorking.get();
        if (pImg == NULL) {
            pImg = IMfllImageBuffer::createInstance("yuv_mc_working");
            if (pImg == NULL) {
                mfllLogE("%s: create YUV MC working buffer instance failed", __FUNCTION__);
                err = MfllErr_CreateInstanceFailed;
                goto lbExit;
            }
            m_imgYuvMcWorking = pImg;
        }

        if (!pImg->isInited()) {
            pImg->setImageFormat(ImageFormat_Yuy2);
            if (m_isFullSizeMc)
                pImg->setResolution(m_width, m_height);
            else
                pImg->setResolution(m_qwidth, m_qheight);
            err = pImg->initBuffer();
            if (err != MfllErr_Ok) {
                mfllLogE("%s: init YUV MC working buffer failed", __FUNCTION__);
                err = MfllErr_UnexpectedError;
                goto lbExit;
            }
        }
        else {
            status.ignore = 1;
            mfllLogD("%s: yuv working(MC) has been inited, ignored here", __FUNCTION__);
        }
    }

lbExit:
    status.err = err;
    m_event->doneEvent(EventType_AllocateYuvMcWorking, status, this);
    mfllFunctionOut();
    return err;
}

enum MfllErr MfllCore::doAllocWeighting(void *void_index)
{
    unsigned int index = (unsigned int)(long)void_index;
    enum MfllErr err = MfllErr_Ok;
    MfllEventStatus_t status;

    Mutex::Autolock _l(m_lockAllocWeighting[index]);

    mfllFunctionIn();
    /* trigger events */
    m_event->onEvent(EventType_AllocateWeighting, status, this, (void*)(long)index);

    if (m_bypass.bypassAllocWeighting[index] || status.ignore) {
        mfllLogD("%s: Bypass allocate weighting table(%d)", __FUNCTION__, index);
        usleep(10 * 1000);
        status.ignore = 1;
    }
    else {
        /* index should be smaller than 2 or it's the critical condition */
        if (index > 2) {
            mfllLogE("%s: index(%d) of weighting table is greater than 2", __FUNCTION__, index);
            abort();
        }

        IMfllImageBuffer *pImg = m_imgWeighting[index].get();
        if (pImg == NULL) {
            pImg = IMfllImageBuffer::createInstance(BUFFER_NAME("wt_", index));
            if (pImg == NULL) {
                mfllLogE("%s: create weighting table(%d) buffer instance failed", __FUNCTION__, index);
                err = MfllErr_CreateInstanceFailed;
                goto lbExit;
            }
            m_imgWeighting[index] = pImg;
        }

        if (!pImg->isInited()) {
            pImg->setImageFormat(ImageFormat_Raw8);
            pImg->setResolution(m_width, m_height);
            err = pImg->initBuffer();
            if (err != MfllErr_Ok) {
                mfllLogE("%s: init weighting table(%d) buffer failed", __FUNCTION__, index);
                err = MfllErr_UnexpectedError;
                goto lbExit;
            }
        }
        else {
            status.ignore = 1;
            mfllLogD("%s: weighting table %d is inited, ignored here", __FUNCTION__, index);
        }
    }

lbExit:
    status.err = err;
    m_event->doneEvent(EventType_AllocateWeighting, status, this, (void*)(long)index);
    mfllFunctionOut();
    return err;
}

enum MfllErr MfllCore::doAllocMemcWorking(void *void_index)
{
    unsigned int index = (unsigned int)(long)void_index;
    enum MfllErr err = MfllErr_Ok;
    MfllEventStatus_t status;

    Mutex::Autolock _l(m_lockAllocMemc[index]);

    mfllFunctionIn();
    /* trigger events */
    m_event->onEvent(EventType_AllocateMemc, status, this, (void*)(long)index);

    if (m_bypass.bypassAllocMemc[index] || status.ignore) {
        mfllLogD("%s: Bypass allocate MEMC working buffer(%d)", __FUNCTION__, index);
        usleep(20 * 1000);
        status.ignore = 1;
    }
    else {
        int bufferIndex = index % getMemcInstanceNum();
        IMfllImageBuffer *pImg = m_imgMemc[bufferIndex].get();
        if (pImg == NULL) {
            pImg = IMfllImageBuffer::createInstance(BUFFER_NAME("memc_", bufferIndex));
            if (pImg == NULL) {
                mfllLogE("%s: create MEMC working buffer(%d) instance failed", __FUNCTION__, bufferIndex);
                err = MfllErr_CreateInstanceFailed;
                goto lbExit;
            }
            m_imgMemc[bufferIndex] = pImg;
        }

        if (!pImg->isInited()) {
            sp<IMfllMemc> memc = m_spMemc[bufferIndex];
            if (memc.get() == NULL) {
                mfllLogE("%s: memc instance(index %d) is NULL", __FUNCTION__, bufferIndex);
                err = MfllErr_NullPointer;
                goto lbExit;
            }
            size_t bufferSize = 0;
            err = memc->getAlgorithmWorkBufferSize(&bufferSize);
            if (err != MfllErr_Ok) {
                mfllLogE("%s: get algorithm working buffer size fail", __FUNCTION__);
                goto lbExit;
            }

            /* check if 2 bytes alignment */
            if (bufferSize % 2 != 0) {
                mfllLogW("%s: algorithm working buffer size is not 2 bytes alignment, make it is", __FUNCTION__);
                bufferSize += 1;
            }

            pImg->setImageFormat(ImageFormat_Raw8);
            pImg->setAligned(2, 2); // always using 2 bytes align
            pImg->setResolution(bufferSize/2, 2);
            err = pImg->initBuffer();
            if (err != MfllErr_Ok) {
                mfllLogE("%s: init MEMC working buffer(%d) failed", __FUNCTION__, bufferIndex);
                err = MfllErr_UnexpectedError;
                goto lbExit;
            }
        }
        else {
            status.ignore = 1;
            mfllLogD("%s: memc buffer %d is inited, ignored here", __FUNCTION__, bufferIndex);
        }
    }

lbExit:
    status.err = err;
    m_event->doneEvent(EventType_AllocateMemc, status, this, (void*)(long)index);
    mfllFunctionOut();
    return err;
}

enum MfllErr MfllCore::doAllocPostview(JOB_VOID)
{
    enum MfllErr err = MfllErr_Ok;
    MfllEventStatus_t status;

    mfllFunctionIn();
    /* trigger events */
    m_event->onEvent(EventType_AllocatePostview, status, this);

    if (m_bypass.bypassAllocPostview) {
        usleep(10 * 1000);
        status.ignore = 1;
    }
    else {
        // Do we need this ?
    }

lbExit:
    status.err = err;
    m_event->doneEvent(EventType_AllocatePostview, status, this);
    mfllFunctionOut();
    return err;
}

enum MfllErr MfllCore::doAllocThumbnail(JOB_VOID)
{
    enum MfllErr err = MfllErr_Ok;
    MfllEventStatus_t status;

    mfllFunctionIn();
    /* trigger events */
    m_event->onEvent(EventType_AllocateThumbnail, status, this);

    if (m_bypass.bypassAllocThumbnail) {
        usleep(10 * 1000);
        status.ignore = 1;
    }
    else {
        // TODO: do we need this anymore?
    }

lbExit:
    status.err = err;
    m_event->doneEvent(EventType_AllocateThumbnail, status, this);
    mfllFunctionOut();
    return err;
}

enum MfllErr MfllCore::doAllocJpeg(JOB_VOID)
{
    enum MfllErr err = MfllErr_Ok;
    MfllEventStatus_t status;

    mfllFunctionIn();
    /* trigger events */
    m_event->onEvent(EventType_AllocateJpeg, status, this);

    if (m_bypass.bypassAllocJpeg) {
        usleep(20 * 1000);
        status.ignore = 1;
    }
    else {
        // TODO: do we need this anymore ?
    }

lbExit:
    status.err = err;
    m_event->doneEvent(EventType_AllocateJpeg, status, this);
    mfllFunctionOut();
    return err;
}

void MfllCore::handleError(enum MfllErr err)
{
    //TODO: handle errors....
    MFLL_UNUSED(err);
}

enum MfllErr MfllCore::createThread(pthread_t *pThread, void *(*routine)(void*), bool bDistach /* = false */)
{
    pthread_attr_t attr = PTHREAD_DEFAULT_ATTR;
    mfllFunctionIn();

    /* increase lifetime, the routine should invoke decStrong */
    incStrong(this);

    pthread_create(pThread, &attr, routine, (void*)this);
    if (bDistach) {
        pthread_detach(*pThread);
    }
    mfllFunctionOut();
    return MfllErr_Ok;
}

enum MfllErr MfllCore::joinThread(pthread_t *pThread)
{
    enum MfllErr err = MfllErr_Ok;
    void *ret;

    mfllFunctionIn();

    int r = pthread_join(*pThread, &ret);
    if (r == EINVAL) {
        mfllLogE("Join pthread 0x%x failed, perhaps it's not a joinable thread");
        err = MfllErr_UnexpectedError;
    }
    mfllFunctionOut();
    return err;
}

void MfllCore::lockSyncObject(MfllSyncObj_t *pSyncObj)
{
    pthread_mutex_lock(&pSyncObj->trigger);
    pthread_mutex_lock(&pSyncObj->done);
}

void MfllCore::unlockSyncObject(MfllSyncObj_t *pSyncObj)
{
    pthread_mutex_unlock(&pSyncObj->trigger);
    pthread_mutex_unlock(&pSyncObj->done);
}

void MfllCore::syncWaitTrigger(MfllSyncObj_t *pSyncObj)
{
    pthread_mutex_lock(&pSyncObj->trigger);
    pthread_mutex_unlock(&pSyncObj->trigger);
}

void MfllCore::syncAnnounceTrigger(MfllSyncObj_t *pSyncObj)
{
    pthread_mutex_unlock(&pSyncObj->trigger);
}

void MfllCore::syncWaitDone(MfllSyncObj_t *pSyncObj)
{
    pthread_mutex_lock(&pSyncObj->done);
    pthread_mutex_unlock(&pSyncObj->done);
}

void MfllCore::syncAnnounceDone(MfllSyncObj_t *pSyncObj)
{
    pthread_mutex_unlock(&pSyncObj->done);
}

MfllConfig_t* MfllCore::createMfllConfig(void)
{
    MfllConfig_t *cfg = new MfllConfig_t;
    // TODO: create MFLL default MfllConfig_t
    return cfg;
}
