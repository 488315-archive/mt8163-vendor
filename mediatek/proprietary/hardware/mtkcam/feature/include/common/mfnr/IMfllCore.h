#ifndef __IMFLLCORE_H__
#define __IMFLLCORE_H__

#include "MfllDefs.h"
#include "MfllTypes.h"
#include "IMfllCapturer.h"
#include "IMfllEventListener.h"
#include "IMfllEvents.h"
#include "IMfllMfb.h"

#include <utils/RefBase.h> // android::RefBase
#include <vector>
#include <string>

using android::sp;
using std::vector;

namespace mfll {

class IMfllCore : public android::RefBase {
public:
    /**
     *  Caller should always create instance via INTERFACE::createInstance method.
     *  And caller also have responsibility to destroy it by invoking destroyInstance.
     *  If caller uses android::sp to manager life cycle of instance who doesn't need to invoke
     *  destroyInstance to destroy instance.
     *
     *  @return             - An IMfllCore instance, caller has responsibility to manager it's lifetime.
     */
    static IMfllCore* createInstance(void);

    /**
     *  To destroy instance, but notice that, because MFLL core related classes inherit android::RefBase,
     *  life time is supposed to be managed by strong pointer. It means that the destructor of these classes
     *  will not be invoked if caller invoked destroyInstance. The destructor will be invoked until
     *  the strong reference counts to zero.
     */
    virtual void destroyInstance(void);

    /**
     *  Get capture information to determine if using MFLL or not. Caller may need to
     *  check if it's necessary to run MFLL of the current lighting condition. Specify
     *  MfllConfig_t::iso, MfllConfig_t::exposure to check if it's necessary to run
     *  MFNR. If not, the MfllConfig_t::blend_num and MfllConfig_t::capture_num will
     *  be 1. Furthermore, to check MfllConfig_t::update_ae is also necessary to be
     *  care about because it may have a case that we don't need to do MFLL but need
     *  to update iso and exposure.
     *
     *  @param *pCfgIn      - pointer of input Mfll configuration
     *  @param *pCfgOut     - output value for containing suggested configuration.
     *  @return             - If ok returns MfllErr_Ok.
     */
    static enum MfllErr getCaptureInfo(const struct MfllConfig *pCfgIn, struct MfllConfig *pCfgOut);

    /**
     *  Get EventType name in const char*
     *
     *  @param e            - EventType to be retrieved.
     *  @return             - Name of EventType.
     */
    static const char* getEventTypeName(enum EventType e);

    /**
     *  Check if the mode is ZSD or note.
     *
     *  @param m            - Mode to check
     *  @return             - If the mode is ZSD mode, returns true
     */
    static bool isZsdMode(const enum MfllMode &m);

public:
    /**
     *  Run MFLL with specified MfllConfig_t structure.
     *
     *  @param cfg          - MFLL configuration to do.
     *  @return             - If ok returns MfllErr_Ok.
     *  @retval MfllErr_Shooted - This instance is out of date and cannot be triggered again.
    */
    virtual enum MfllErr doMfll(MfllConfig_t cfg) = 0;

    /**
     *  Get specified blending frame number.
     *
     *  @return             - Frame number to blend
     */
    virtual unsigned int getBlendFrameNum(void) = 0;

    /**
     *  Get specified capture frame number.
     *
     *  @return             - Frame number to capture
     */
    virtual unsigned int getCaptureFrameNum(void) = 0;

    /**
     *  Get instance num of Memc object.
     *
     *  @return             - Number of Memc instance.
     */
    virtual unsigned int getMemcInstanceNum(void) = 0;

    /**
     *  Get MFLL core reversion
     *
     *  @return             - 8 digitals for representing aa.bbb.ccc
     *                        e.g.: v2.0.1, return value will be
     *                        0x02000001
     */
    virtual unsigned int getReversion(void) = 0;

    /**
     *  Get MFLL core reversion in std::string format
     *
     *  @return             - A std::string object contains reversion info.
     *                        e.g.: 2.0.1
     */
    virtual std::string getReversionString(void) = 0;

    /**
     *  Get sensor ID that MFLL is configured as.
     *
     *  @return             - Sensor ID
     *  @note               - Default it returns -1.
     */
    virtual int getSensorId(void) = 0;

    /**
     *  Retrieve if this MFLL instance shooted or notice
     *
     *  @return             - True or false
     *  @note               - This function is thread-safe.
     */
    virtual bool isShooted(void) = 0;

    /**
     *  Determine if MfllCore runs as full size MC
     *  @return             - True or false.
     *  @note               - This function works only if doMfll has been invoked.
     */
    virtual bool isFullSizeMc(void) = 0;

    /**
     *  MFLL provides that event listener mechanism. You can register event listener
     *  to monitor event and pre-process or post-process operation for the event.
     *
     *  @param e            - A IMfllEventListener object with smart pointer protected
     *                        It means the lifetime of this object will be holded by
     *                        MfllCore.
     *  @return             - If ok returns MfllErr_Ok.
     *  @retval MfllErr_Shooted - The MFLL instance has shooted and cannot execute
     *                            this operation anymore.
     *  @note               - Operations in event may block MFLL flow.
     *                      - This operation is thread-safe.
     *                      - Lifetime will be extended or managed by strong pointer.
     */
    virtual enum MfllErr registerEventListener(const sp<IMfllEventListener> &e) = 0;

    /**
     *  Remove registered event from MFLL.
     *  @param *e           - Identification of IMfllEventListener.
     *  @return             - If ok returns MfllErr_Ok
     *  @retval MfllErr_Shooted - The MFLL instance has shooted and cannot execute
     *                            this operation anymore.
     *  @note               - removing events will travelling all registered events
     *                        and find the specified event and remove it. It means
     *                        this operation is high cost, make sure you really
     *                        want to remove event or just can ignore event.
     *                      - This operation is thread-safe.
     */
    virtual enum MfllErr removeEventListener(IMfllEventListener *e) = 0;

    /**
     *  User can set bypass option before calling doMfll.
     *  @param b            - Bypass option instance to describe the bypass operations
     *  @return             - If applied returns MfllErr_Ok.
     *                        If is doing MFLL, this function doesn't work and returns MfllErr_Shooted.
     */
    virtual enum MfllErr setBypassOption(const MfllBypassOption_t &b) = 0;

    /**
     *  Set capture resolution to MFLL
     *  @param width        - Width of capture image
     *  @param height       - Height of capture image
     *  @return             - If ok returns MfllErr_Ok
     *  @retval MfllErr_Shooted - The MFLL instance has shooted and cannot execute
     *                            this operation anymore
     *  @note               - This operation is thread-safe.
     */
    virtual enum MfllErr setCaptureResolution(unsigned int width, unsigned int height) = 0;

    /**
     *  Set capture resolution of QSize YUV for algorithm
     *  @param qwidth       - Q size width of capture image
     *  @param qheight      - Q size height of capture image
     *  @return             - If ok returns MfllErr_Ok
     *  @retval MfllErr_Shooted - The MFLL instance has shooted and cannot execute
     *                            this operaion anymore
     *  @note               - This operation is thread-safe.
     */
    virtual enum MfllErr setCaptureQResolution(unsigned int qwidth, unsigned int qheight) = 0;

    /**
     *  User may also set capturer to MfllCore. If capturer hasn't been set, MfllCore will
     *  invoke IMfllCapturer::createInstance to create IMfllCapturer instance when need.
     *
     *  @param capturer     - A strong pointer contains reference of IMfllCapturer instance.
     *  @return             - If ok returns MfllErr_Ok
     *  @note               - This operation is thread-safe
     *                        Lifetime of capturer should be handled by android::sp.
     */
    virtual enum MfllErr setCapturer(const sp<IMfllCapturer> &capturer) = 0;

    /**
     *  User may also set a MFB processor to MfllCore. If MFB processor hasn't been set,
     *  MfllCore will invoke IMfllMfb::createInstance to create IMfllMfb instance when need.
     *
     *  @param mfb          - A strong pointer contains reference of IMfllMfb instance.
     *  @return             - If ok returns MfllErr_Ok
     *  @note               - This operation is thread-safe
     */
    virtual enum MfllErr setMfb(const sp<IMfllMfb> &mfb) = 0;

    /**
     *  Returns IMfllMfb instance that IMfllCore is using.
     *
     *  @return             - If there's no IMfllMfb is using, sp<IMfllMfb> contains NULL
     */
    virtual sp<IMfllMfb> getMfb(void) = 0;

    /**
     *  To get IMfllImageBuffer instance in android::sp<IMfllImageBuffer> type.
     *
     *  @param s            - Buffer to release.
     *  @param index        - If the selected buffer is not only just a buffer,
     *                        index will be the index of the serial buffers.
     *  @return             - If this function is fail returns sp<T> which contains NULL pointer.
     *  @sa                 - enum MfllBuffer
     */
    virtual sp<IMfllImageBuffer> retrieveBuffer(const enum MfllBuffer &s, int index = 0) = 0;

    /**
     *  To release buffer using decStrong rather than directly release. When the
     *  reference count of the IMfllImageBuffer instance is counted to 0, it will
     *  be released.
     *
     *  @param s            - Buffer to release.
     *  @param index        - If the selected buffer is not only just a buffer,
     *                        index will be the index of the serial buffers.
     *  @return             - Returns MfllErr_Ok if ok
     *  @sa                 - enum MfllBuffer
     */
    virtual enum MfllErr releaseBuffer(const enum MfllBuffer &s, int index = 0) = 0;

protected:
    virtual ~IMfllCore(void) {};

}; /* class IMfllCore */
}; /* namespace mfll */
#endif

