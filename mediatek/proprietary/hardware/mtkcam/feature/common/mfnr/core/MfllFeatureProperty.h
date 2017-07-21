#ifndef __MFLLFEATUREPROPERTY_H__
#define __MFLLFEATUREPROPERTY_H__

#include "IMfllCore.h"
#include "IMfllEventListener.h"

#include <utils/Mutex.h> // android::Mutex

#define MFLL_PROPERTY_ON        1
#define MFLL_PROPERTY_OFF       0
#define MFLL_PROPERTY_DEFAULT   -1

using android::Mutex;

namespace mfll {
    typedef enum tagProperty {
        Property_ForceMfll = 0,
        Property_CaptureNum,
        Property_BlendNum,
        Property_Iso,
        Property_Exposure,
        Property_Mrp,
        Property_Rwb,

        Property_DumpAll,
        Property_DumpRaw,
        Property_DumpYuv,
        Property_DumpMfb,
        Property_DumpMix,
        Property_DumpJpeg,
        Property_DumpPostview,
        Property_DumpExif,

        Property_Size
    } Property_t;

    const char* const PropertyString[Property_Size] = {
        "mediatek.mfll.force",
        "mediatek.mfll.catpure_num",
        "mediatek.mfll.blend_num",
        "mediatek.mfll.iso",
        "mediatek.mfll.exp",
        "mediatek.mfll.mrp",
        "mediatek.mfll.rwb",
        "mediatek.mfll.dump.all",
        "mediatek.mfll.dump.raw",
        "mediatek.mfll.dump.yuv",
        "mediatek.mfll.dump.mfb",
        "mediatek.mfll.dump.mix",
        "mediatek.mfll.dump.jpeg",
        "mediatek.mfll.dump.postview",
        "mediatek.mfll.dump.exif"
    };

/**
 *  Property Read/Write/Wait
 *
 *  This plug-in provides a mechanism to read property and with these properties,
 *  some features might be for enabled or not.
 */
class MfllFeatureProperty : public IMfllEventListener {
public:
    MfllFeatureProperty(void);
    virtual ~MfllFeatureProperty(void);

public:

/* implementation */
public:
    virtual void onEvent(enum EventType t, MfllEventStatus_t &status, void *mfllCore, void *param1 = NULL, void *param2 = NULL);
    virtual void doneEvent(enum EventType t, MfllEventStatus_t &status, void *mfllCore, void *param1 = NULL, void *param2 = NULL);
    virtual vector<enum EventType> getListenedEventTypes(void);

/* interface */
public:
    /* To read property from device */
    static int readProperty(const Property_t &t);

    /* To check if force on MFNR */
    static bool isForceMfll(void);

/**
 *  Dump information will be available after MFNR core has been inited
 */
public:
    inline bool isDumpRaw(void)
    { return m_propValue[Property_DumpRaw] == MFLL_PROPERTY_ON ? true : false; }

    inline bool isDumpYuv(void)
    { return m_propValue[Property_DumpYuv] == MFLL_PROPERTY_ON ? true : false; }

    inline bool isDumpMfb(void)
    { return m_propValue[Property_DumpMfb] == MFLL_PROPERTY_ON ? true : false; }

    inline bool isDumpMix(void)
    { return m_propValue[Property_DumpMix] == MFLL_PROPERTY_ON ? true : false; }

    inline bool isDumpJpeg(void)
    { return m_propValue[Property_DumpJpeg] == MFLL_PROPERTY_ON ? true : false; }

    inline bool isDumpPostview(void)
    { return m_propValue[Property_DumpPostview] == MFLL_PROPERTY_ON ? true : false; }

    inline bool isDumpExif(void)
    { return m_propValue[Property_DumpExif] == MFLL_PROPERTY_ON ? true : false; }

public:
    /**
     *  To check if any dump property is specified
     *  @return             - Ture for yes.
     */
    bool isDump(void);

    /**
     *  Get the cached value of the specified property
     *  @param t            - Propery to get
     */
    int getProperty(const Property_t &t);

    /**
     *  Set the value to the specified property
     *  @param t            - Propery to set
     *  @param v            - Value to set
     */
    void setProperty(const Property_t &t , const int &v);

private:
    Mutex m_mutex;
    int m_propValue[Property_Size]; // saves status of this property

}; /* class MfllFeatureProperty */
}; /* namespace mfll */
#endif//__MFLLFEATUREPROPERTY_H__

