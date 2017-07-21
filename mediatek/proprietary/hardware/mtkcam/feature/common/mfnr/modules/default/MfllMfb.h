#ifndef __MFLLMFB_H__
#define __MFLLMFB_H__

#include "IMfllMfb.h"

namespace mfll {
class MfllMfb : public IMfllMfb {
public:
    MfllMfb(void);
    virtual ~MfllMfb(void);

public:
    virtual enum MfllErr init(int sensorId);
    virtual enum MfllErr blend(IMfllImageBuffer *base, IMfllImageBuffer *ref, IMfllImageBuffer *out, IMfllImageBuffer *wt_in, IMfllImageBuffer *wt_out);
    virtual enum MfllErr mix(IMfllImageBuffer *base, IMfllImageBuffer *ref, IMfllImageBuffer *out, IMfllImageBuffer *wt);
    virtual enum MfllErr setWeightingBuffer(sp<IMfllImageBuffer> input, sp<IMfllImageBuffer> output);
    virtual enum MfllErr setSyncPrivateData(void *data, size_t size);
    virtual enum MfllErr setNoiseReductionType(const enum NoiseReductionType &t);
    virtual sp<IMfllImageBuffer> getWeightingBufferInput(void);
    virtual sp<IMfllImageBuffer> getWeightingBufferOutput(void);
    virtual enum MfllErr encodeRawToYuv(IMfllImageBuffer *input, IMfllImageBuffer *output, const enum YuvStage &s);
    virtual enum MfllErr encodeRawToYuv(
            IMfllImageBuffer *input,
            IMfllImageBuffer *output,
            IMfllImageBuffer *output_q,
            enum YuvStage s = YuvStage_RawToYuy2)
    { return MfllErr_NotImplemented; }

    virtual enum MfllErr setMfllCore(IMfllCore *c)
    { return MfllErr_NotImplemented; }

    virtual enum MfllErr convertImage(
            sp<IMfllImageBuffer> src,
            sp<IMfllImageBuffer> dst,
            sp<IMfllImageBuffer> dst2 = NULL,
            unsigned int transform = 0,
            unsigned int transform2 = 0,
            const MfllRect_t cropRect = MfllRect_t(),
            const MfllRect_t cropRect2 = MfllRect_t()
            )
    { return MfllErr_NotImplemented; }
};
}; /* namespace mfll */

#endif//__MFLLMFB_H__

