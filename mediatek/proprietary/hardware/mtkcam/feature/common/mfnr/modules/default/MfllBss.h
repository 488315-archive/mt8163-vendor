#ifndef __MFLLBSS_H__
#define __MFLLBSS_H__

#include "IMfllBss.h"

using std::vector;

namespace mfll {
class MfllBss : public IMfllBss {
public:
    vector<int> bss(
            const vector< sp<IMfllImageBuffer> > &imgs,
            const vector<MfllMotionVector_t> &mvs)
    { return vector<int>(); }

    void setRoiPercentage(const int &p) { }

protected:
    ~MfllBss() {};
}; // class MfllBss
}; // namespace mfll

#endif//__MFLLBSS_H__
