#include "MfllBss.h"
#include "MfllLog.h"

using std::vector;
using namespace mfll;

IMfllBss* IMfllBss::createInstance()
{
    return reinterpret_cast<IMfllBss*>(new MfllBss);
}

void IMfllBss::destroyInstance()
{
    decStrong((void*)this);
}
