#ifndef __MFLLCAPTURER_H__
#define __MFLLCAPTURER_H__

#include "IMfllCapturer.h"


using android::sp;

namespace mfll {
class MfllCapturer : public IMfllCapturer {
public:
    MfllCapturer(void){};

/* Implementations */
public:
    enum MfllErr captureFrames(
            unsigned int captureFrameNum,
            std::vector< sp<IMfllImageBuffer>* > &raws,
            std::vector< sp<IMfllImageBuffer>* > &qyuvs,
            std::vector<MfllMotionVector_t*> &gmvs)
    {
        return MfllErr_NotImplemented;
    }
    enum MfllErr captureFrames(
            unsigned int captureFrameNum,
            std::vector< sp<IMfllImageBuffer>* > &raws,
            std::vector< sp<IMfllImageBuffer>* > &yuvs,
            std::vector< sp<IMfllImageBuffer>* > &qyuvs,
            std::vector< MfllMotionVector_t* > &gmvs)
    {
        return MfllErr_NotImplemented;
    }
    unsigned int getCapturedFrameNum(void)
    {
        return -1;
    }
    enum MfllErr setCaptureMode(const enum MfllMode &mode)
    {
        return MfllErr_NotImplemented;
    }
    enum MfllErr registerEventDispatcher(const sp<IMfllEvents> &e)
    {
        return MfllErr_NotImplemented;
    }
    enum MfllErr setMfllCore(IMfllCore *c)
    {
        return MfllErr_NotImplemented;
    }
    enum MfllErr queueFullSizeRaw(sp<IMfllImageBuffer> &img)
    {
        return MfllErr_NotImplemented;
    }
    enum MfllErr queueGmv(const MfllMotionVector_t &mv)
    {
        return MfllErr_NotImplemented;
    }

protected:
    virtual ~MfllCapturer(void){}
}; /* class mfll */
}; /* namespace mfll */
#endif /* __MFLLCAPTURER_H__ */

