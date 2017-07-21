#ifndef __IMFLLMFB_H__
#define __IMFLLMFB_H__

#include "MfllDefs.h"
#include "MfllTypes.h"
#include "IMfllEvents.h"
#include "IMfllImageBuffer.h"

#include <utils/RefBase.h> // android::RefBase

namespace mfll {

/**
 *  IMfllMfb is a class which is resonsibility to do some operations of Pass 2.
 *      1. blending
 *      2. mixing
 *      3. color space converting
 *      4. image cropping/scaling/rotating(90 degree)
 *  For these operations, caller has to set sensor ID first (using IMfllMfb::init),
 *  and perhaps it's necessary to set the sync private data.
 *
 *  @note   Pass 2 driver is NOT a thread-safe mechanism which means if you process
 *          the operation in different thread may cause race condition and occur
 *          unexpected error. Therefore, IMfllMfb has done the job that make all
 *          IMfllMfb instances won't process the operation at the same time to avoid
 *          race condition of Pass 2 driver.
 */
class IMfllMfb : public android::RefBase {
public:
    static IMfllMfb* createInstance(void);
    virtual void destroyInstance(void);

public:
    /**
     *  Init IMfllMfb module
     *
     *  @note               - You don't need to to unint this module it will be unit in destructor.
     */
    virtual enum MfllErr init(int sensorId) = 0;

    /**
     *  Give reference of IMfllCore
     *  @param c                - IMfllCore address
     */
    virtual enum MfllErr setMfllCore(IMfllCore *c) = 0;

    /**
     *  Do image blending.
     *  Before invokes this method, you could prepare weighting memory for IMfllMfb, or
     *  IMfllMfb will invoke IMfllImageBuffer::createInstance to create. And IMfllImageBuffer::initBuffer
     *  is also invoked before use it.
     *
     *  @param base         - Base frame as input image.
     *  @param ref          - Reference frame as input image.
     *  @param out          - Blended image.
     *  @param wt_in        - Weighting table input, the first time to blend, set this argument as NULL
     *  @param wt_out       - Weighting table output.
     *  @note               - This function must be thread-safe.
     */
    virtual enum MfllErr blend(IMfllImageBuffer *base, IMfllImageBuffer *ref, IMfllImageBuffer *out, IMfllImageBuffer *wt_in, IMfllImageBuffer *wt_out) = 0;

    /**
     *  Do image mixing.
     *  Before invokes this method, you could prepare weighting memory for IMfllMfb, or
     *  IMfllMfb will invoke IMfllImageBuffer::createInstance to create. And IMfllImageBuffer::initBuffer
     *  is also invoked before use it.
     *
     *  @param base         - Base frame as input image.
     *  @param ref          - Reference frame as input image.
     *  @param out          - Mixed frame image.
     *  @param wt           - Weighting table of the last blended output
     *  @note               - This function must be thread-safe.
     */
    virtual enum MfllErr mix(IMfllImageBuffer *base, IMfllImageBuffer *ref, IMfllImageBuffer *out, IMfllImageBuffer *wt) = 0;

    /**
     *  Encode RAW10 image to YUV serial image using pass 2 driver, like YUV2, YV16
     *  If the the resolutions are different between input and output buffers,
     *  a cropping will be applied and the ratio will be keeped automatically
     *
     *  @param input        - Input buffer
     *  @param output       - Output buffer
     *  @param s            - Different stage will relate to different ISP profile.
     *  @return             - Returns MfllErr_Ok if ok.
     *  @note               - The different ISP profile may be applied if the
     *                        NoiseReductionType is different.
     *  @sa                 setNoiseReductionType
     */
    virtual enum MfllErr encodeRawToYuv(IMfllImageBuffer *input, IMfllImageBuffer *output, const enum YuvStage &s) = 0;

    /**
     *  Encode RAW10 image to 2 YUV buffer (full size + quarter size).
     *  If the the resolutions are different between input and output buffers,
     *  a cropping will be applied and the ratio will be keeped automatically
     *  @param input        Input buffer, full size
     *  @param output       Output buffer, full size
     *  @param output_q     Output buffer, quarter size
     *  @param s            YuvStage should be YuvStage_RawToYuy2 or YuvStage_RawToYv16
     */
    virtual enum MfllErr encodeRawToYuv(
            IMfllImageBuffer *input,
            IMfllImageBuffer *output,
            IMfllImageBuffer *output_q,
            enum YuvStage s = YuvStage_RawToYuy2) = 0;

    /**
     *  Set sync data for pass 1 and pass 2. This data should be get by IspSyncControl
     *
     *  @param *data        - Private data
     *  @param size         - Data size
     *  @return             - Returns MfllErr_Ok if ok.
     */
    virtual enum MfllErr setSyncPrivateData(void *data, size_t size) = 0;

    /**
     *  To tell IMfllMfb the type of Noise Reduction to be used. Hence MFNR may have
     *  chance to apply SWNR or HWNR, the different ISP profiles may be applied if
     *  ISP profile is different. Please remember to specify the NoiseReductionType.
     *
     *  @param nrType       - The type of NR
     *  @return             - Returns MfllErr_Ok if ok.
     *  @sa                 NoiseReductionType
     */
    virtual enum MfllErr setNoiseReductionType(const enum NoiseReductionType &t) = 0;

    /**
     *  A quick way to crop, rotate (90 degree based), scale.
     *
     *  @param src          - Source image
     *  @param dst          - Destination image
     *  @param dst2         - Destination image 2, can be NULL
     *  @param transform    - Rotating transform in degree, 0, 90, 180, 270
     *  @param transform2   - Rotating transform in degree for dst2
     *  @param cropRect     - Cropping window of source image, if do not apply
     *                        cropping, don't set this argument
     *  @param cropRect2    - Cropping window of source image, for dst2
     */
    virtual enum MfllErr convertImage(
            sp<IMfllImageBuffer> src,
            sp<IMfllImageBuffer> dst,
            sp<IMfllImageBuffer> dst2 = NULL,
            unsigned int transform = 0,
            unsigned int transform2 = 0,
            const MfllRect_t cropRect = MfllRect_t(),
            const MfllRect_t cropRect2 = MfllRect_t()
            ) = 0;

protected:
    virtual ~IMfllMfb(void){};
};
}; /* namespace mfll */

#endif//__IMFLLMFB_H__

