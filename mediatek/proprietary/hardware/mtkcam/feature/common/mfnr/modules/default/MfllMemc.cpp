#include "MfllMemc.h"

using namespace mfll;

IMfllMemc* IMfllMemc::createInstance()
{
    return reinterpret_cast<IMfllMemc*>(new MfllMemc);
}

void IMfllMemc::destroyInstance()
{
    decStrong(reinterpret_cast<void*>(this));
}


