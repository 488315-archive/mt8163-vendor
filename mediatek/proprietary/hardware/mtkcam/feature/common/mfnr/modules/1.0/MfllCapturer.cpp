#include "MfllCapturer.h"
#include "MfllMfb.h"
#include "MfllLog.h"
#include "MfllCore.h"

#define LOG_TAG "MtkCam/MfllCore/MfllCapturer"

using namespace mfll;

IMfllCapturer* IMfllCapturer::createInstance(void)
{
    return (IMfllCapturer*)new MfllCapturer;
}

void IMfllCapturer::destroyInstance(void)
{
    this->decStrong((void*)this);
}

MfllCapturer::MfllCapturer(void)
: m_queueMutex(PTHREAD_MUTEX_INITIALIZER)
, m_queueMutexGmv(PTHREAD_MUTEX_INITIALIZER)
, m_mxVectorRaw(PTHREAD_MUTEX_INITIALIZER)
, m_mxVectorGmv(PTHREAD_MUTEX_INITIALIZER)
{
    m_spEventDispatcher = NULL;
    m_frameNum = 0;
    m_captureMode = (enum MfllMode)0;

    /* lock sync mutex first for waiting an element available */
    pthread_mutex_lock(&m_queueMutex);
    pthread_mutex_lock(&m_queueMutexGmv);
}

MfllCapturer::~MfllCapturer(void)
{
    mfllLogD("Delete MfllCapturer");
}

enum MfllErr MfllCapturer::captureFrames(
    unsigned int captureFrameNum,
    std::vector< sp<IMfllImageBuffer>* > &raws,
    std::vector< sp<IMfllImageBuffer>* > &qyuvs,
    std::vector<MfllMotionVector_t*> &gmvs)
{
    std::vector< sp<IMfllImageBuffer>* > ____NULL;
    return __captureFrames(
            captureFrameNum,
            raws,       // raw
            ____NULL,   // yuv
            qyuvs,      // qyuv
            gmvs);      // gmv
}

enum MfllErr MfllCapturer::captureFrames(
        unsigned int captureFrameNum,
        std::vector< sp<IMfllImageBuffer>* > &raws,
        std::vector< sp<IMfllImageBuffer>* > &yuvs,
        std::vector< sp<IMfllImageBuffer>* > &qyuvs,
        std::vector< MfllMotionVector_t* > &gmvs
)
{
    return __captureFrames(
            captureFrameNum,
            raws,
            yuvs,
            qyuvs,
            gmvs);
}

enum MfllErr MfllCapturer::__captureFrames(
    unsigned int captureFrameNum,
    std::vector< sp<IMfllImageBuffer>* > &raws,
    std::vector< sp<IMfllImageBuffer>* > &yuvs,
    std::vector< sp<IMfllImageBuffer>* > &qyuvs,
    std::vector< MfllMotionVector_t* > &gmvs
)
{
    enum MfllErr err = MfllErr_Ok;

    /* is need full YUV */
    bool bFullYuv = yuvs.size() > 0 ? true : false;

    MfllEventStatus_t eventStatusRaw;
    MfllEventStatus_t eventStatusYuv;
    MfllEventStatus_t eventStatusEis;

    sp<IMfllMfb> pmfb = m_pCore->getMfb();
    if (pmfb.get() == NULL) {
        mfllLogE("%s: IMfllMfb instance is NULL", __FUNCTION__);
    }

    /* handle RAW to YUV frames first (and then, GMVs) */
    for (size_t i = 0; i < captureFrameNum; i++) {
        /* capture RAW */
        sp<IMfllImageBuffer> r;
        dispatchOnEvent(EventType_CaptureRaw, eventStatusRaw, (void*)(long)i, NULL);
        if (eventStatusRaw.ignore == 0) {
            r = dequeFullSizeRaw();
        }
        (*raws[i]) = r;
        dispatchDoneEvent(EventType_CaptureRaw, eventStatusRaw, (void*)(long)i, NULL);

        /* capture YUV */
        dispatchOnEvent(EventType_CaptureYuvQ, eventStatusYuv, (void*)(long)i, NULL);
        if (r.get() == NULL) { // No raw
            eventStatusYuv.err = MfllErr_NullPointer;
            eventStatusYuv.ignore = 1;
            dispatchDoneEvent(EventType_CaptureYuvQ, eventStatusYuv, (void*)(long)i, NULL);
        }
        else {
            err = reinterpret_cast<MfllCore*>(m_pCore)->doAllocQyuvBuffer((void*)(long)i);
            if (err != MfllErr_Ok) {
                mfllLogE("%s: alloc QYUV buffer %d failed", __FUNCTION__, i);
                eventStatusYuv.err = err;
                eventStatusYuv.ignore = 1;
                dispatchDoneEvent(EventType_CaptureYuvQ, eventStatusYuv, (void*)(long)i, NULL);
            }
            else {
                if (bFullYuv) {
                    mfllLogD("%s: RAW to 2 YUVs, %dx%d, %dx%d.", __FUNCTION__,
                            (*yuvs[i])->getWidth(), (*yuvs[i])->getHeight(),
                            (*qyuvs[i])->getWidth(), (*qyuvs[i])->getHeight());

                    err = pmfb->encodeRawToYuv(
                            r.get(),
                            (*yuvs[i]).get(),
                            (*qyuvs[i]).get(),
                            YuvStage_RawToYv16
                            );
                }
                else {
                    err = pmfb->encodeRawToYuv(
                            r.get(),
                            (*qyuvs[i]).get(),
                            YuvStage_RawToYv16);
                }
                eventStatusYuv.err = err;
                dispatchDoneEvent(EventType_CaptureYuvQ, eventStatusYuv, (void*)(long)i, NULL);
            }
        }
    }

    /* GMV */
    for (size_t i = 0; i < captureFrameNum; i++) {
        /* GMV */
        dispatchOnEvent(EventType_CaptureEis, eventStatusEis, (void*)(long)i, NULL);
        MfllMotionVector_t mv;
        if (eventStatusEis.ignore == 0) {
            mv = dequeGmv();
            (*gmvs[i]) = mv;
        }
        dispatchDoneEvent(EventType_CaptureEis, eventStatusEis, (void*)(long)i, NULL);

    }

    mfllLogD("Capture Done");
    return MfllErr_Ok;
}

unsigned int MfllCapturer::getCapturedFrameNum(void)
{
    return m_frameNum;
}

enum MfllErr MfllCapturer::setMfllCore(IMfllCore *c)
{
    m_pCore = c;
    return MfllErr_Ok;
}

enum MfllErr MfllCapturer::setCaptureMode(const enum MfllMode &mode)
{
    m_captureMode = mode;
    return MfllErr_Ok;
}

enum MfllErr MfllCapturer::registerEventDispatcher(const sp<IMfllEvents> &e)
{
    m_spEventDispatcher = e;
    return MfllErr_Ok;
}

enum MfllErr MfllCapturer::queueFullSizeRaw(sp<IMfllImageBuffer> &img)
{
    enum MfllErr err = MfllErr_Ok;
    pthread_mutex_trylock(&m_queueMutex); // broadcast for dequeFullSizeRaw
    {
        pthread_mutex_lock(&m_mxVectorRaw); // make sure vector operation is thread-safe
        {
            m_vRaws.push_back(img); // push_back
        }
        pthread_mutex_unlock(&m_mxVectorRaw);

    }
    pthread_mutex_unlock(&m_queueMutex);
    return err;
}

sp<IMfllImageBuffer> MfllCapturer::dequeFullSizeRaw(void)
{
    sp<IMfllImageBuffer> img;
    pthread_mutex_t *mx = &m_queueMutex;

    pthread_mutex_lock(mx); // wait if necessary
    {
        pthread_mutex_lock(&m_mxVectorRaw);
        {
            img = m_vRaws[0];
            m_vRaws.erase(m_vRaws.begin());
        }
        pthread_mutex_unlock(&m_mxVectorRaw);
    }
    pthread_mutex_unlock(mx);
    return img;
}

enum MfllErr MfllCapturer::queueGmv(const MfllMotionVector_t &mv)
{
    enum MfllErr err = MfllErr_Ok;
    pthread_mutex_trylock(&m_queueMutexGmv);
    {
        pthread_mutex_lock(&m_mxVectorGmv);
        {
            m_vGmvs.push_back(mv);
        }
        pthread_mutex_unlock(&m_mxVectorGmv);
    }
    pthread_mutex_unlock(&m_queueMutexGmv);
    return err;
}

MfllMotionVector_t MfllCapturer::dequeGmv(void)
{
    MfllMotionVector_t mv;
    pthread_mutex_t *mx = &m_queueMutexGmv;

    pthread_mutex_lock(mx);
    {
        pthread_mutex_lock(&m_mxVectorGmv);
        {
            mv = m_vGmvs[0];
            m_vGmvs.erase(m_vGmvs.begin());
        }
        pthread_mutex_unlock(&m_mxVectorGmv);
    }
    pthread_mutex_unlock(mx);
    return mv;
}

void MfllCapturer::dispatchOnEvent(const EventType &t, MfllEventStatus_t &s, void *p1, void *p2)
{
    if (m_spEventDispatcher.get()) {
        m_spEventDispatcher->onEvent(t, s, (void*)m_pCore, p1, p2);
    }
}

void MfllCapturer::dispatchDoneEvent(const EventType &t, MfllEventStatus_t &s, void *p1, void *p2)
{
    if (m_spEventDispatcher.get()) {
        m_spEventDispatcher->doneEvent(t, s, (void*)m_pCore, p1, p2);
    }
}

