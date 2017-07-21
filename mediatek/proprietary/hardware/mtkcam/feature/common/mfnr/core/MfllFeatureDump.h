#ifndef __MFLLFEATUREDUMP_H__
#define __MFLLFEATUREDUMP_H__

#include "IMfllCore.h"
#include "IMfllEventListener.h"

#include <string>

#define MFLL_DUMP_DEFAUL_PATH "/sdcard/DCIM/Camera/"

namespace mfll {

/**
 *  Dump Mechanism
 *
 *  This plug-in provides a mechanism to dump frame data for every stage.
 */
class MfllFeatureDump : public IMfllEventListener {
public:
    typedef struct _DumpFlag {
        int raw;
        int yuv;
        int mfb;
        int mix;
        int jpeg;
        int postview;
        int exif;
    } DumpFlag;

    typedef enum tagDumpStage {
        DumpStage_Capture = 0,
        DumpStage_Bss,
        DumpStage_Base,
        DumpStage_Golden,
        DumpStage_Memc,
        DumpStage_Mfb,
        DumpStage_Mix,
        DumpStage_Rwb,
        DumpStage_Jpeg,
        DumpStage_Postview,
        /* size */
        DumpStage_Size
    } DumpStage;

public:
    MfllFeatureDump(void);

protected:
    virtual ~MfllFeatureDump(void);

/* implementation */
public:
    virtual void onEvent(enum EventType t, MfllEventStatus_t &status, void *mfllCore, void *param1 = NULL, void *param2 = NULL);
    virtual void doneEvent(enum EventType t, MfllEventStatus_t &status, void *mfllCore, void *param1 = NULL, void *param2 = NULL);
    virtual vector<enum EventType> getListenedEventTypes(void);

/* interface */
public:
    inline void setFilePath(const std::string &s)
    { m_filepath = s; }
    inline void setDumpFlag(const DumpFlag &flags)
    { m_dumpFlag = flags; }

private:
    DumpFlag m_dumpFlag;
    std::string m_filepath;

    void dump_image(IMfllImageBuffer *pImg, const DumpStage &stage, const char *name, int number = 0);
    void dumpCapture(IMfllCore *pCore);
    void dumpBaseStage(IMfllCore *pCore);
    void dumpMfbStage(IMfllCore *pCore, int index);
    void dumpMixStage(IMfllCore *pCore);

}; /* class MfllFeatureDump */
}; /* namespace mfll */
#endif//__MFLLFEATUREDUMP_H__

