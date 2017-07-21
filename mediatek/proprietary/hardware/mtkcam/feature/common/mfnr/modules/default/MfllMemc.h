#ifndef __MFLLMEMC_H__
#define __MFLLMEMC_H__

#include "IMfllMemc.h"

namespace mfll {
class MfllMemc : public IMfllMemc {
public:
    MfllMemc(void) {}
public:
    enum MfllErr init(void) { return MfllErr_NotImplemented; }
    enum MfllErr getAlgorithmWorkBufferSize(size_t *size) { return MfllErr_NotImplemented; }
    void setMotionEstimationResolution(const int &w, const int &h){}
    void setMotionCompensationResolution(const int &w, const int &h){}
    void setAlgorithmWorkingBuffer(sp<IMfllImageBuffer> &buffer){}
    void setMeBaseImage(sp<IMfllImageBuffer> &img){}
    void setMeRefImage(sp<IMfllImageBuffer> &img){}
    void setMcRefImage(sp<IMfllImageBuffer> &img){}
    void setMcDstImage(sp<IMfllImageBuffer> &img){}
    void setMcThreadNum(const int &thread_num){}
    enum MfllErr motionEstimation(){ return MfllErr_NotImplemented; }
    enum MfllErr motionCompensation(){ return MfllErr_NotImplemented; }
    int isIgnoredMotionCompensation(){ return 0; }
    void giveupMotionCompensation(){}

protected:
    virtual ~MfllMemc(void) {}
};
}; /* namespace mfll */

#endif//__MFLLMEMC_H__

