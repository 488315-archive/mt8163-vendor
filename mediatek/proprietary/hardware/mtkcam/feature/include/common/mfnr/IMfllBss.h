#ifndef __IMFLLBSS_H__
#define __IMFLLBSS_H__

#include "MfllDefs.h"
#include "MfllTypes.h"
#include "IMfllImageBuffer.h"

#include <utils/RefBase.h> // android::RefBase
#include <vector> // std::vector

using android::sp;
using std::vector;

namespace mfll {
class IMfllBss : public android::RefBase {
public:
    static IMfllBss* createInstance(void);
    void destroyInstance(void);

/* interfaces */
public:
    /**
     *  Calcuate bss
     *  @param imgs             IMfllImageBuffer contains image
     *  @param mvs              Motion vectors
     *  @return                 New index
     */
    virtual  vector<int> bss(
            const vector< sp<IMfllImageBuffer> > &imgs,
            const vector<MfllMotionVector_t> &mvs) = 0;

    /**
     *  To setup the percentage of ROI size from the original image
     *  @param p                Percentage in integer
     */
    virtual void setRoiPercentage(const int &p) = 0;

protected:
    virtual ~IMfllBss(){};
};
}; /* namespace mfll */

#endif//__IMFLLBSS_H__
