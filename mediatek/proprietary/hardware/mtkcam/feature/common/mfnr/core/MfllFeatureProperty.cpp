#include "MfllFeatureProperty.h"
#include "MfllLog.h"
#include "MfllCore.h"

#define LOG_TAG "MtkCam/MfllCore/Prop"

#include <cutils/properties.h> // property_get

#include <stdlib.h> // atoi

#ifndef PROPERTY_VALUE_MAX
#define PROPERTY_VALUE_MAX 32
#endif

using namespace mfll;
using std::vector;

static std::vector<enum EventType> EVENTS_TO_LISTEN_INITIALIZER(void)
{
    std::vector<enum EventType> v;
    #define LISTEN(x) v.push_back(x)
    LISTEN(EventType_Init);
    #undef LISTEN
    return v;
}

static vector<enum EventType> g_eventsToListen = EVENTS_TO_LISTEN_INITIALIZER();


MfllFeatureProperty::MfllFeatureProperty(void)
{
}

MfllFeatureProperty::~MfllFeatureProperty(void)
{
}

vector<enum EventType> MfllFeatureProperty::getListenedEventTypes(void)
{
    return g_eventsToListen;
}

void MfllFeatureProperty::onEvent(enum EventType t, MfllEventStatus_t &status, void *mfllCore, void *param1, void *param2)
{
}

void MfllFeatureProperty::doneEvent(enum EventType t, MfllEventStatus_t &status, void *mfllCore, void *param1, void *param2)
{
    mfllLogD("doneEvent: %s", IMfllCore::getEventTypeName(t));
    switch(t) {
    case EventType_Init:
        for (size_t i = 0; i < (size_t)Property_Size; i++) {
            int v = readProperty((Property_t)i);
            m_propValue[i] = v;

            /* If dump all is set ... */
            if (Property_DumpAll == i && v == MFLL_PROPERTY_ON) {
                for (int j = (i + 1); j<(int)Property_Size; j++) {
                    setProperty((Property_t)j, MFLL_PROPERTY_ON);
                }
                break;
            }
        }
        break;
    }
}

int MfllFeatureProperty::readProperty(const Property_t &t)
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    property_get(PropertyString[(size_t)t], value, "-1");
    int v = atoi(value);
    return v;
}

bool MfllFeatureProperty::isForceMfll(void)
{
    return readProperty(Property_ForceMfll) == MFLL_PROPERTY_ON ? true : false;
}

bool MfllFeatureProperty::isDump(void)
{
    Mutex::Autolock _l(m_mutex);
    bool is_dump = false;
    for (int i = (int)Property_DumpRaw; i < (int)Property_Size; i++) {
        if (m_propValue[i] == MFLL_PROPERTY_ON) {
            is_dump = true;
            break;
        }
    }
    return is_dump;
}

int MfllFeatureProperty::getProperty(const Property_t &t)
{
    Mutex::Autolock _l(m_mutex);
    return m_propValue[(size_t)t];
}

void MfllFeatureProperty::setProperty(const Property_t &t, const int &v)
{
    Mutex::Autolock _l(m_mutex);
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    snprintf(value, PROPERTY_VALUE_MAX, "%d", v);
    property_set(PropertyString[(size_t)t], value);
    m_propValue[(size_t)t] = v;
}

