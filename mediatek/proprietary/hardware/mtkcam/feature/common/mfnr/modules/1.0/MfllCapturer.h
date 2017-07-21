#ifndef __MFLLCAPTURER_H__
#define __MFLLCAPTURER_H__

#include "IMfllCore.h"
#include "IMfllCapturer.h"
#include "IMfllImageBuffer.h"

#include <pthread.h>
#include <vector>

using std::vector;
using android::sp;

namespace mfll {
class MfllCapturer : public IMfllCapturer {
public:
    MfllCapturer(void);
    virtual ~MfllCapturer(void);

/* Implementations */
public:
    enum MfllErr captureFrames(
        unsigned int captureFrameNum,
        std::vector< sp<IMfllImageBuffer>* > &raws,
        std::vector< sp<IMfllImageBuffer>* > &qyuvs,
        std::vector< MfllMotionVector_t* > &gmvs
    );
    enum MfllErr captureFrames(
        unsigned int captureFrameNum,
        std::vector< sp<IMfllImageBuffer>* > &raws,
        std::vector< sp<IMfllImageBuffer>* > &yuvs,
        std::vector< sp<IMfllImageBuffer>* > &qyuvs,
        std::vector< MfllMotionVector_t* > &gmvs
    );
    unsigned int getCapturedFrameNum(void);
    enum MfllErr setMfllCore(IMfllCore *c);
    enum MfllErr setCaptureMode(const enum MfllMode &mode);
    enum MfllErr registerEventDispatcher(const sp<IMfllEvents> &e);
    enum MfllErr queueFullSizeRaw(sp<IMfllImageBuffer> &img);
    enum MfllErr queueGmv(const MfllMotionVector_t &mv);

private:
    /* real captureFrames Implementations */
    enum MfllErr __captureFrames(
        unsigned int captureFrameNum,
        std::vector< sp<IMfllImageBuffer>* > &raws,
        std::vector< sp<IMfllImageBuffer>* > &yuvs,
        std::vector< sp<IMfllImageBuffer>* > &qyuvs,
        std::vector< MfllMotionVector_t* > &gmvs
    );

    sp<IMfllImageBuffer>    dequeFullSizeRaw(void);
    MfllMotionVector_t      dequeGmv(void);

    unsigned int m_frameNum;
    enum MfllMode m_captureMode; // indicates to capture mode.
    sp<IMfllEvents> m_spEventDispatcher;
    IMfllCore *m_pCore;

    /* mutex/cond for RAW */
    mutable pthread_mutex_t m_queueMutex;
    mutable pthread_mutex_t m_mxVectorRaw; // mutex for operating m_vRaws
    vector< sp<IMfllImageBuffer> >  m_vRaws;

    /* for GMV info */
    mutable pthread_mutex_t m_queueMutexGmv;
    mutable pthread_mutex_t m_mxVectorGmv; // mutex for operating m_vRaws
    vector<MfllMotionVector_t>      m_vGmvs;

private:
    void dispatchOnEvent(const EventType &t, MfllEventStatus_t &s, void *p1, void *p2);
    void dispatchDoneEvent(const EventType &t, MfllEventStatus_t &s, void *p1, void *p2);

}; /* class mfll */
}; /* namespace mfll */
#endif /* __MFLLCAPTURER_H__ */
