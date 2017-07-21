#include "AudioALSADeviceParser.h"
#include <stdint.h>
#include <dirent.h>
#include <sys/types.h>

#define LOG_TAG "AudioALSADeviceParser"

#define ALSASOUND_CARD_LOCATION "proc/asound/cards"
#define ALSASOUND_DEVICE_LOCATION "/proc/asound/devices/"
#define ALSASOUND_PCM_LOCATION "/proc/asound/pcm"
#define PROC_READ_BUFFER_SIZE (256)

namespace android
{

static String8 keypcmPlayback = String8("playback");
static String8 keypcmCapture = String8("capture");

AudioALSADeviceParser *AudioALSADeviceParser::UniqueAlsaDeviceInstance = NULL;

AudioALSADeviceParser *AudioALSADeviceParser::getInstance()
{
    if (UniqueAlsaDeviceInstance == 0)
    {
        ALOGD("+UniqueVolumeInstance\n");
        UniqueAlsaDeviceInstance = new AudioALSADeviceParser();
        ALOGD("-UniqueVolumeInstance\n");
    }
    return UniqueAlsaDeviceInstance;
}

AudioALSADeviceParser::AudioALSADeviceParser()
{
    ALOGD("%s()", __FUNCTION__);
    ParseCardIndex();
    GetAllPcmAttribute();
    QueryPcmDriverCapability();
    dump();
}

void AudioALSADeviceParser::ParseCardIndex()
{
    /*
     * $adb shell cat proc/asound/cards
     *  0 [mtsndcard      ]: mt-snd-card - mt-snd-card
     *                       mt-snd-card
     * mCardIndex = 0;
     */
    ALOGD("%s()", __FUNCTION__);
    FILE *mCardFile = NULL;
    char tempbuffer[PROC_READ_BUFFER_SIZE];
    mCardFile = fopen(ALSASOUND_CARD_LOCATION, "r");
    bool isCardIndexFound = false;
    if (mCardFile)
    {
        ALOGD("card open success");
        while (!feof(mCardFile))
        {
            fgets(tempbuffer, PROC_READ_BUFFER_SIZE, mCardFile);

            if (strchr(tempbuffer, '['))    // this line contain '[' character
            {
                char *Rch = strtok(tempbuffer, "[");
                mCardIndex = atoi(Rch);
                ALOGD("\tcurrent CardIndex = %d, Rch = %s", mCardIndex, Rch);
                Rch = strtok(NULL, " ]");
                ALOGD("\tcurrent sound card name = %s", Rch);
                if (strcmp(Rch, keyCardName.string()) == 0)
                {
                    ALOGD("\tmCardIndex found = %d", mCardIndex);
                    isCardIndexFound = true;
                    break;
                }
            }

            memset((void *)tempbuffer, 0, PROC_READ_BUFFER_SIZE);
        }
        ALOGD("reach EOF");
    }
    else
    {
        ALOGD("Pcm open fail");
    }

    if(mCardFile)
        fclose(mCardFile);

    ASSERT(isCardIndexFound);
}

void AudioALSADeviceParser::dump()
{
    size_t i = 0;
    ALOGD("dump size = %d", mAudioDeviceVector.size());
    for (i = 0 ; i < mAudioDeviceVector.size(); i++)
    {
        AudioDeviceDescriptor *temp = mAudioDeviceVector.itemAt(i);
        ALOGD("name = %s ", temp->mStreamName.string());
        ALOGD("card index = %d pcm index = %d", temp->mCardindex, temp->mPcmindex);
        ALOGD("playback  = %d capture = %d", temp->mplayback, temp->mRecord);
    }
    ALOGD("dump done");
}

unsigned int AudioALSADeviceParser::GetPcmBufferSize(unsigned int  pcmindex, unsigned int direction)
{
    size_t i = 0;
    unsigned int buffersize =0;
    for (i = 0 ; i < mAudioDeviceVector.size(); i++)
    {
        AudioDeviceDescriptor *temp = mAudioDeviceVector.itemAt(i);
        if (temp->mPcmindex ==  pcmindex)
        {
            if(direction == PCM_OUT)
            {
                buffersize =  temp->mPlayparam.mBufferBytes;
            }
            else if (direction == PCM_IN)
            {
                buffersize =  temp->mRecordparam.mBufferBytes;
            }
            break;
        }
    }
    ALOGD("%s buffersize = %d",__FUNCTION__,buffersize);
    return buffersize;
}


unsigned int  AudioALSADeviceParser::GetPcmIndexByString(String8 stringpair)
{
    ALOGD("%s() stringpair = %s ", __FUNCTION__, stringpair.string());
    int pcmindex = -1;
    size_t i = 0;
    for (i = 0 ; i < mAudioDeviceVector.size(); i++)
    {
        AudioDeviceDescriptor *temp = mAudioDeviceVector.itemAt(i);
        if (temp->mStreamName.compare(stringpair) == 0)
        {
            pcmindex = temp->mPcmindex;
            ALOGD("compare success = %d", pcmindex);
            break;
        }
    }
    return pcmindex;
}

unsigned int  AudioALSADeviceParser::GetCardIndexByString(String8 stringpair)
{
    ALOGD("%s() stringpair = %s ", __FUNCTION__, stringpair.string());
    int Cardindex = -1;
    size_t i = 0;
    for (i = 0 ; i < mAudioDeviceVector.size(); i++)
    {
        AudioDeviceDescriptor *temp = mAudioDeviceVector.itemAt(i);
        if (temp->mStreamName.compare(stringpair) == 0)
        {
            Cardindex = temp->mCardindex;
            ALOGD(" compare success Cardindex = %d", Cardindex);
            break;
        }
    }
    return Cardindex;
}


void AudioALSADeviceParser::SetPcmCapability(AudioDeviceDescriptor *Descriptor , char  *Buffer)
{
    if (Buffer == NULL)
    {
        return;
    }
    String8 CompareString = String8(Buffer);
    //ALOGD("SetPcmCapability CompareString = %s",CompareString.string ());
    if ((CompareString.compare(keypcmPlayback)) == 0 )
    {
        //ALOGD("SetPcmCapability playback support");
        Descriptor->mplayback = 1;
    }
    if ((CompareString.compare(keypcmCapture)) == 0)
    {
        //ALOGD("SetPcmCapability capture support");
        Descriptor->mRecord = 1;
    }
}

void AudioALSADeviceParser::AddPcmString(char *InputBuffer)
{
    ALOGD("AddPcmString InputBuffer = %s", InputBuffer);
    char *Rch;
    AudioDeviceDescriptor *mAudiioDeviceDescriptor = NULL;
    Rch = strtok(InputBuffer, "-");
    // parse for stream name
    if (Rch  != NULL)
    {
        mAudiioDeviceDescriptor = new AudioDeviceDescriptor();
        mAudiioDeviceDescriptor->mCardindex = atoi(Rch);
        Rch = strtok(NULL, ":");
        mAudiioDeviceDescriptor->mPcmindex = atoi(Rch);
        Rch = strtok(NULL, ": ");
        mAudiioDeviceDescriptor->mStreamName = String8(Rch);
        Rch = strtok(NULL, ": ");
        mAudioDeviceVector.push(mAudiioDeviceDescriptor);
    }
    // parse for playback or record support
    while (Rch  != NULL)
    {
        Rch = strtok(NULL, ": ");
        if(mAudiioDeviceDescriptor != NULL)
        {
            SetPcmCapability(mAudiioDeviceDescriptor, Rch);
        }
    }
}

void AudioALSADeviceParser::GetAllPcmAttribute(void)
{
    ALOGD("%s()", __FUNCTION__);
    FILE *mPcmFile = NULL;
    char tempbuffer[PROC_READ_BUFFER_SIZE];
    int result;
    mPcmFile = fopen(ALSASOUND_PCM_LOCATION, "r");
    if (mPcmFile)
    {
        ALOGD("Pcm open success");
        while (!feof(mPcmFile))
        {
            fgets(tempbuffer, PROC_READ_BUFFER_SIZE, mPcmFile);
            AddPcmString(tempbuffer);
            memset((void *)tempbuffer, 0, PROC_READ_BUFFER_SIZE);
        }
        ALOGD("reach EOF");
    }
    else
    {
        ALOGD("Pcm open fail");
    }

    if(mPcmFile)
        fclose(mPcmFile);
}

status_t AudioALSADeviceParser::GetPcmDriverparameters(AudioPcmDeviceparam *PcmDeviceparam,struct pcm_params *params)
{

    PcmDeviceparam->mRateMin = pcm_params_get_min(params, PCM_PARAM_RATE);
    PcmDeviceparam->mRateMax = pcm_params_get_max(params, PCM_PARAM_RATE);
    ALOGD("Rate:\tmin=%uHz\tmax=%uHz\n",PcmDeviceparam->mRateMin,PcmDeviceparam->mRateMax);

    PcmDeviceparam->mChannelMin = pcm_params_get_min(params, PCM_PARAM_CHANNELS);
    PcmDeviceparam->mChannelMax = pcm_params_get_max(params, PCM_PARAM_CHANNELS);
    ALOGD("Channels:\tmin=%u\t\tmax=%u\n",PcmDeviceparam->mChannelMin, PcmDeviceparam->mChannelMax );

    PcmDeviceparam->mSampleBitMin = pcm_params_get_min(params, PCM_PARAM_SAMPLE_BITS);
    PcmDeviceparam->mSampleBitMax = pcm_params_get_max(params, PCM_PARAM_SAMPLE_BITS);
    ALOGD("Sample bits:\tmin=%u\t\tmax=%u\n",PcmDeviceparam->mSampleBitMin , PcmDeviceparam->mSampleBitMax);

    PcmDeviceparam->mPreriodSizeMin= pcm_params_get_min(params, PCM_PARAM_PERIOD_SIZE);
    PcmDeviceparam->mPreriodSizeMax = pcm_params_get_max(params, PCM_PARAM_PERIOD_SIZE);
    ALOGD("Period size:\tmin=%u\t\tmax=%u\n",PcmDeviceparam->mPreriodSizeMin,PcmDeviceparam->mPreriodSizeMax );

    PcmDeviceparam->mPreriodCountMin = pcm_params_get_min(params, PCM_PARAM_PERIODS);
    PcmDeviceparam->mPreriodCountMax = pcm_params_get_max(params, PCM_PARAM_PERIODS);
    ALOGD("Period count:\tmin=%u\t\tmax=%u\n",PcmDeviceparam->mPreriodCountMin,    PcmDeviceparam->mPreriodCountMax);

    PcmDeviceparam->mBufferBytes = pcm_params_get_max(params, PCM_PARAM_BUFFER_BYTES);
    ALOGD("PCM_PARAM_BUFFER_BYTES :\t max=%u\t\n",PcmDeviceparam->mBufferBytes);

    return NO_ERROR;
}


status_t AudioALSADeviceParser::QueryPcmDriverCapability()
{
    struct pcm_params *params = NULL;
    AudioDeviceDescriptor *temp = NULL;;
    int Direction = 0;
    for(int i=0; i < mAudioDeviceVector.size();i++)
    {
        temp = mAudioDeviceVector.itemAt(i);
        ALOGD("pcm %d %s %s",temp->mPcmindex,temp->mStreamName.string (),temp->mCodecName.string ());
        ALOGD("pcm mCardindex %d mPcmindex %d ",temp->mCardindex,temp->mPcmindex);
        if(temp->mplayback == 1)
        {
            params = pcm_params_get(temp->mCardindex,temp->mPcmindex,PCM_OUT);
            if (params == NULL)
            {
                ALOGD("Device %d does not exist playback",i);
            }
            else
            {
                if(temp->mplayback == 1)
                {
                    GetPcmDriverparameters(&temp->mPlayparam,params);
                }
            }
            if(params != NULL)
            {
                pcm_params_free(params);
                params = NULL;
            }
        }

        if (temp->mRecord == 1)
        {
            params = pcm_params_get(temp->mCardindex,temp->mPcmindex,PCM_IN);
            if (params == NULL)
            {
                ALOGD("Device %d does not exist capture",i);
            }
            else
            {
                if(temp->mRecord == 1)
                {
                    GetPcmDriverparameters(&temp->mRecordparam,params);
                }
            }
            if(params != NULL)
            {
                pcm_params_free(params);
                params = NULL;
            }
        }
    }
    return NO_ERROR;
}


}
