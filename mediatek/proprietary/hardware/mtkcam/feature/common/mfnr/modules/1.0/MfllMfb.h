#ifndef __MfllMfb_H__
#define __MfllMfb_H__

#include "IMfllMfb.h"
#include "MfllImageBuffer.h"

/* mtkcam related headers */
#include <IImageBuffer.h>
#include <iopipe/PostProc/INormalStream.h>
#include <iopipe/PostProc/IPortEnum.h>
#include <Hal3/IHal3A.h>

#include <utils/Mutex.h> // android::Mutex

using android::sp;
using android::Mutex;
using NSCam::IImageBuffer;
using NSCam::MRect;
using NSCam::MSize;
using NSCam::MPoint;
using NS3Av3::MetaSet_T;
using NSCam::NSIoPipe::NSPostProc::INormalStream;
using NS3Av3::IHal3A;

namespace mfll {
class MfllMfb : public IMfllMfb {
public:
    MfllMfb(void);
    virtual ~MfllMfb(void);

/* interface */
public:
    enum MfllErr setMetadata(const MetaSet_T& metaset);


/* implementations */
public:
    virtual enum MfllErr init(int sensorId);
    virtual enum MfllErr setMfllCore(IMfllCore *c)
    { m_pCore = c; return MfllErr_Ok; }
    virtual enum MfllErr blend(IMfllImageBuffer *base, IMfllImageBuffer *ref, IMfllImageBuffer *out, IMfllImageBuffer *wt_in, IMfllImageBuffer *wt_out);
    virtual enum MfllErr mix(IMfllImageBuffer *base, IMfllImageBuffer *ref, IMfllImageBuffer *out, IMfllImageBuffer *wt);
    virtual enum MfllErr setSyncPrivateData(void *data, size_t size);
    virtual enum MfllErr setNoiseReductionType(const enum NoiseReductionType &t);

    /* Raw to Yuv */
    virtual enum MfllErr encodeRawToYuv(IMfllImageBuffer *input, IMfllImageBuffer *output, const enum YuvStage &s);
    virtual enum MfllErr encodeRawToYuv(
            IMfllImageBuffer *input,
            IMfllImageBuffer *output,
            IMfllImageBuffer *output_q,
            enum YuvStage s = YuvStage_RawToYuy2);
    
    virtual enum MfllErr convertImage(
            sp<IMfllImageBuffer> src,
            sp<IMfllImageBuffer> dst,
            sp<IMfllImageBuffer> dst2 = NULL,
            unsigned int transform = 0,
            unsigned int transform2 = 0,
            const MfllRect_t cropRect = MfllRect_t(),
            const MfllRect_t cropRect2 = MfllRect_t()
            );

/* private interface */
protected:
    enum MfllErr encodeRawToYuv(
            IImageBuffer *src,
            IImageBuffer *dst,
            IImageBuffer *dst2,
            const enum YuvStage &s);

    enum MfllErr blend(IImageBuffer *base, IImageBuffer *ref, IImageBuffer *out, IImageBuffer *wt_in, IImageBuffer *wt_out);

    /**
     *  A function to scale/crop/rotate image
     */
    static enum MfllErr convertImage(
        IImageBuffer *img_src,
        IImageBuffer *img_dst,
        IImageBuffer *img_dst2,
        unsigned int transform,
        unsigned int transform2,
        int usingCrop = 0,
        int usingCrop2 = 0,
        MRect cropRect = {MPoint(0, 0), MSize(0,0)},
        MRect cropRect2 = {MPoint(0, 0), MSize(0,0)}
    );

/* attributes */
private:
    /* thread-safe protector */
    mutable Mutex m_mutex;

private:
    int m_sensorId;
    enum MfllMode m_shotMode;
    enum NoiseReductionType m_nrType;
    void *m_syncPrivateData;
    size_t m_syncPrivateDataSize;
    MetaSet_T m_metaSet;
    std::vector<MetaSet_T> m_vMetaSet;
    IMfllCore *m_pCore;

    IHal3A          *m_p3A;
    INormalStream   *m_pNormalStream;

    volatile int m_encodeYuvCount;
    volatile int m_blendCount;
};
};/* namespace mfll */
#endif//__MfllMfb_H__

