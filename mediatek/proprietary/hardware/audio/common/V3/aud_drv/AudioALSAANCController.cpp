#include "AudioALSAANCController.h"

#include <linux/ioctl.h>
#include <cutils/properties.h>

#include "AudioLock.h"

#include "AudioALSAHardwareResourceManager.h"
#include "AudioALSAVolumeController.h"


#define LOG_TAG "AudioALSAANCController"

namespace android
{

/*==============================================================================
 *                     Property keys
 *============================================================================*/


/*==============================================================================
 *                     Const Value
 *============================================================================*/

/*==============================================================================
 *                     Enumerator
 *============================================================================*/

/*==============================================================================
 *                     Singleton Pattern
 *============================================================================*/

AudioALSAANCController *AudioALSAANCController::mAudioALSAANCController = NULL;

AudioALSAANCController *AudioALSAANCController::getInstance()
{
    static AudioLock mGetInstanceLock;
    AudioAutoTimeoutLock _l(mGetInstanceLock);

    if (mAudioALSAANCController == NULL)
    {
        mAudioALSAANCController = new AudioALSAANCController();
    }
    ASSERT(mAudioALSAANCController != NULL);
    return mAudioALSAANCController;
}

/*==============================================================================
 *                     Constructor / Destructor / Init / Deinit
 *============================================================================*/

AudioALSAANCController::AudioALSAANCController() :
    mHardwareResourceManager(AudioALSAHardwareResourceManager::getInstance()),
    //mAudioALSAVolumeController(AudioALSAVolumeController::getInstance()),
    mFmEnable(false),
    mPcm(NULL)
{
    ALOGD("%s()", __FUNCTION__);


    mFd = ::open(kANCDevName, O_RDWR);

    if (mFd < 0)
    {
        ALOGE("%s() fail to open %s", __FUNCTION__, kANCDevName);
    }
    else
    {
        ALOGD("%s() open %s success!", __FUNCTION__, kANCDevName);

        ::ioctl(mFd, SET_ANC_CONTROL, 0);
    }
}

AudioALSAANCController::~AudioALSAANCController()
{
    ALOGD("%s()", __FUNCTION__);
    if (mFd)
    {
        ::close(mFd);
        mFd = 0;
    }
}

/*==============================================================================
 *                     ANC Control
 *============================================================================*/

bool AudioALSAANCController::getFmEnable()
{
    AudioAutoTimeoutLock _l(mLock);
    ALOGV("%s(), mFmEnable = %d", __FUNCTION__, mFmEnable);
    return mFmEnable;
}

int AudioALSAANCController::setCMD(int cmd)
{
    AudioAutoTimeoutLock _l(mLock);
    ALOGD("%s(), CMD = %d", __FUNCTION__, cmd);
    ALOGD("mFd=%d", mFd);
    switch(cmd){
        case 81:
            mHardwareResourceManager->startInputDevice(AUDIO_DEVICE_IN_WIRED_HEADSET);
            ALOGD("%s(), mHardwareResourceManager->startInputDevice(AUDIO_DEVICE_IN_WIRED_HEADSET)", __FUNCTION__);
            cmd=3;  //enable uplink
            break;
        case 82:
            mHardwareResourceManager->stopInputDevice(AUDIO_DEVICE_IN_WIRED_HEADSET);
            ALOGD("%s(), mHardwareResourceManager->stopInputDevice(AUDIO_DEVICE_IN_WIRED_HEADSET)", __FUNCTION__);
            return 0;
            break;
    }

    int result = ::ioctl(mFd, SET_ANC_CONTROL, cmd);
    ALOGD("result %d", result);
    return 0;
}

status_t AudioALSAANCController::setFmEnable(const bool enable, const audio_devices_t output_device)
{
    AudioAutoTimeoutLock _l(mLock);

    ALOGD("+%s(), mFmEnable = %d => enable = %d", __FUNCTION__, mFmEnable, enable);

    ALOGD("-%s()", __FUNCTION__);
    return NO_ERROR;
}

void AudioALSAANCController::download_binary()
{
    AudioAutoTimeoutLock _l(mLock);

    ALOGD("+%s()", __FUNCTION__);

    ALOGD("-%s()", __FUNCTION__);
}

/*==============================================================================
 *                     Audio HW Control
 *============================================================================*/

uint32_t AudioALSAANCController::getFmUplinkSamplingRate() const
{
    return 44100; // TODO(Harvey): query it
}

uint32_t AudioALSAANCController::getFmDownlinkSamplingRate() const
{
    return 44100; // TODO(Harvey): query it
}

} // end of namespace android
