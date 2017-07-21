#ifndef ANDROID_AUDIO_MTK_DEVICE_PARSER_H
#define ANDROID_AUDIO_MTK_DEVICE_PARSER_H

#include <stdint.h>
#include <sys/types.h>

#include <utils/Mutex.h>
#include <utils/String8.h>
#include <media/AudioSystem.h>
#include <utils/KeyedVector.h>
#include <tinyalsa/asoundlib.h>
#include "AudioALSADeviceString.h"
#include "AudioType.h"

namespace android
{

class AudioPcmDeviceparam
{
    public:
        unsigned int mBufferBytes;
        unsigned int mRateMax;
        unsigned int mRateMin;
        unsigned int mChannelMax;
        unsigned int mChannelMin;
        unsigned int mSampleBitMax;
        unsigned int mSampleBitMin;
        unsigned int mPreriodSizeMax;
        unsigned int mPreriodSizeMin;
        unsigned int mPreriodCountMax;
        unsigned int mPreriodCountMin;
};

class AudioDeviceDescriptor
{
    public:
        AudioDeviceDescriptor()
        {
            mCardindex = 0;
            mPcmindex = 0;
            mplayback = 0;
            mRecord = 0;
        };
        String8 mStreamName;
        String8 mCodecName;
        unsigned int mCardindex;
        unsigned int mPcmindex;
        unsigned int mplayback;
        unsigned int mRecord;
        AudioPcmDeviceparam mPlayparam;
        AudioPcmDeviceparam mRecordparam;
};

class AudioALSADeviceParser
{
    public:
        unsigned int GetPcmIndexByString(String8 stringpair);
        unsigned int GetCardIndexByString(String8 stringpair);
        unsigned int GetPcmBufferSize(unsigned int  pcmindex, unsigned int direction);
        static AudioALSADeviceParser *getInstance();
        void dump();

        unsigned int GetCardIndex(){return mCardIndex;}
    private:
        static AudioALSADeviceParser *UniqueAlsaDeviceInstance;
        AudioALSADeviceParser();
        void GetAllPcmAttribute(void);
        void AddPcmString(char *InputBuffer);
        status_t QueryPcmDriverCapability();
        status_t GetPcmDriverparameters(AudioPcmDeviceparam *PcmDeviceparam,struct pcm_params *params);
        void SetPcmCapability(AudioDeviceDescriptor *Descriptor , char  *Buffer);
        void ParseCardIndex();
        /**
         * Audio Pcm vector
         */
        Vector <AudioDeviceDescriptor *> mAudioDeviceVector;

        unsigned int mCardIndex;
};

}

#endif
