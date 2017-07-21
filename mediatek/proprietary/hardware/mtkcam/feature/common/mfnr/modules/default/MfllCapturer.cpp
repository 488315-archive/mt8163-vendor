#include "MfllCapturer.h"

using namespace mfll;

IMfllCapturer* IMfllCapturer::createInstance(void)
{
    return (IMfllCapturer*)new MfllCapturer;
}

void IMfllCapturer::destroyInstance(void)
{
    decStrong((void*)this);
}
