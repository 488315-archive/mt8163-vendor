#ifndef __MFLLMEMC_H__
#define __MFLLMEMC_H__

#include "IMfllMemc.h"

#include <libmfbll/MTKMfbll.h>

using android::sp;

namespace mfll {
class MfllMemc : public IMfllMemc {
public:
    MfllMemc();

public:
    enum MfllErr init(void);
    enum MfllErr getAlgorithmWorkBufferSize(size_t *size);
    enum MfllErr motionEstimation();
    enum MfllErr motionCompensation();
    void setMotionEstimationResolution(const int &w, const int &h)
    {
        m_widthMe = w;
        m_heightMe = h;
        updateIsUsingFullMemc();
    }
    void setMotionCompensationResolution(const int &w, const int &h)
    {
        m_widthMc = w;
        m_heightMc = h;
        updateIsUsingFullMemc();
    }
    void setAlgorithmWorkingBuffer(sp<IMfllImageBuffer> &buffer)
    {
        m_imgWorkingBuffer = buffer;
    }
    void setMeBaseImage(sp<IMfllImageBuffer> &img)
    {
        m_imgSrcMe = img;
    }
    void setMeRefImage(sp<IMfllImageBuffer> &img)
    {
        m_imgRefMe = img;
    }
    void setMcRefImage(sp<IMfllImageBuffer> &img)
    {
        m_imgRefMc = img;
    }
    void setMcDstImage(sp<IMfllImageBuffer> &img)
    {
        m_imgDstMc = img;
    }
    void setMcThreadNum(const int &thread_num)
    {
        m_mcThreadNum = thread_num;
    }
    int isIgnoredMotionCompensation()
    {
        return m_isIgnoredMc;
    }
    void giveupMotionCompensation()
    {
        m_isIgnoredMc = 1;
    }

private:
    inline void updateIsUsingFullMemc()
    {
        /* if resolution is different, try it as using full MC */
        m_isUsingFullMc = (m_widthMe == m_widthMc && m_heightMe == m_heightMc)
            ? 0 : 1;
    }

private:
    MTKMfbll *m_mfbll;

    int m_widthMe;
    int m_heightMe;
    int m_widthMc;
    int m_heightMc;
    int m_mcThreadNum;

    volatile int m_isUsingFullMc;
    volatile int m_isIgnoredMc;

    /* image buffers for algorithm working, decStrong after motionCompensation */
    sp<IMfllImageBuffer> m_imgWorkingBuffer;

    /* image buffers for ME, decStrong after motionEstimation() */
    sp<IMfllImageBuffer> m_imgSrcMe;
    sp<IMfllImageBuffer> m_imgRefMe;

    /* image buffers for MC, decStrong after motionCompensation */
    sp<IMfllImageBuffer> m_imgRefMc;
    sp<IMfllImageBuffer> m_imgDstMc;

protected:
    virtual ~MfllMemc(void);
};
}; /* namespace mfll */

#endif//__MFLLMEMC_H__

