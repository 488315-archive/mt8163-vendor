#ifndef ANDROID_ALSA_AUDIO_ANC_CONTROLLER_H
#define ANDROID_ALSA_AUDIO_ANC_CONTROLLER_H

#include "AudioType.h"
#include "AudioLock.h"

#include <tinyalsa/asoundlib.h>

#define AUD_DRV_ANC_IOC_MAGIC 'C'
//ANC Controlaa
#define SET_ANC_CONTROL          _IOW(AUD_DRV_ANC_IOC_MAGIC, 0x1, int)
#define SET_ANC_PARAMETER        _IOW(AUD_DRV_ANC_IOC_MAGIC, 0x2, int)
#define GET_ANC_PARAMETER        _IOW(AUD_DRV_ANC_IOC_MAGIC, 0x3, int)

namespace android
{

static char const *const kANCDevName = "/dev/ancservice";

class AudioALSAHardwareResourceManager;

class AudioALSAANCController
{
    public:
        virtual ~AudioALSAANCController();

        static AudioALSAANCController *getInstance();

        virtual void download_binary();

        virtual bool     getFmEnable();
        virtual status_t setFmEnable(const bool enable, const audio_devices_t output_device);

        virtual uint32_t getFmUplinkSamplingRate() const;
        virtual uint32_t getFmDownlinkSamplingRate() const;

        int setCMD(int cmd);

        protected:
        AudioALSAANCController();

        AudioALSAHardwareResourceManager *mHardwareResourceManager;
        //AudioALSAVolumeController        *mAudioALSAVolumeController;


        AudioLock mLock; // TODO(Harvey): could remove it later...

        bool mFmEnable;
        bool mIsFmDirectConnectionMode;

        //Mutex   mMutex;
        int     mFd;

        float mFmVolume;

        struct pcm_config mConfig;
        struct pcm *mPcm;


    private:
        static AudioALSAANCController *mAudioALSAANCController; // singleton

};

} // end namespace android

#endif // end of ANDROID_ALSA_AUDIO_ANC_CONTROLLER_H
