#include "AudioALSAHardwareResourceManager.h"

#include <utils/threads.h>

#include "AudioType.h"
#include "AudioLock.h"
#include "AudioAssert.h"

#include "AudioALSADriverUtility.h"
#include "audio_custom_exp.h"

// TMP
#include "AudioALSACodecDeviceOutEarphonePMIC.h"
#include "AudioALSACodecDeviceOutReceiverPMIC.h"
#include "AudioALSACodecDeviceOutSpeakerNXP.h"
#include "AudioALSACodecDeviceOutSpeakerPMIC.h"
#include "AudioALSACodecDeviceOutSpeakerEarphonePMIC.h"
#include "AudioALSADeviceConfigManager.h"

#ifdef USING_EXTAMP_HP
#include "AudioALSACodecDeviceOutExtSpeakerAmp.h"
#endif

#include "WCNChipController.h"

#define LOG_TAG "AudioALSAHardwareResourceManager"
static   const char PROPERTY_KEY_EXTDAC[PROPERTY_KEY_MAX]  = "af.resouce.extdac_support";

namespace android
{

static const char *SIDEGEN[] =
{
    "I0I1",   "I2",     "I3I4",   "I5I6",
    "I7I8",   "I9",     "I10I11", "I12I13",
    "I14",    "I15I16", "I17I18", "I19I20",
    "I21I22", "O0O1",   "O2",     "O3O4",
    "O5O6",   "O7O8",   "O9O10",  "O11",
    "O12",    "O13O14", "O15O16", "O17O18",
    "O19O20", "O21O22", "O23O24", "OFF"
};

static const char *SIDEGEN_SAMEPLRATE[] = {"8K", "11K", "12K", "16K", "22K", "24K", "32K", "44K", "48K"};


AudioALSAHardwareResourceManager *AudioALSAHardwareResourceManager::mAudioALSAHardwareResourceManager = NULL;
AudioALSAHardwareResourceManager *AudioALSAHardwareResourceManager::getInstance()
{
    static AudioLock mGetInstanceLock;
    AudioAutoTimeoutLock _l(mGetInstanceLock);

    if (mAudioALSAHardwareResourceManager == NULL)
    {
        mAudioALSAHardwareResourceManager = new AudioALSAHardwareResourceManager();
    }
    ASSERT(mAudioALSAHardwareResourceManager != NULL);
    return mAudioALSAHardwareResourceManager;
}

status_t AudioALSAHardwareResourceManager::ResetDevice(void)
{
	if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_Amp_R_Switch"), "Off"))
	{
		ALOGE("Error: Audio_Amp_R_Switch invalid value");
	}

	if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_Amp_L_Switch"), "Off"))
	{
		ALOGE("Error: Audio_Amp_L_Switch invalid value");
	}

	if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Voice_Amp_Switch"), "Off"))
	{
		ALOGE("Error: Voice_Amp_Switch invalid value");
	}

	if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Speaker_Amp_Switch"), "Off"))
	{
		ALOGE("Error: Speaker_Amp_Switch invalid value");
	}

  	if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Ext_Speaker_Amp_Switch"), "Off"))
	{
		ALOGE("Error: Ext_Speaker_Amp_Switch invalid value");
	}

	if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Headset_Speaker_Amp_Switch"), "Off"))
	{
		ALOGE("Error: Headset_Speaker_Amp_Switch invalid value");
	}

	if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_ADC_1_Switch"), "Off"))
	{
		ALOGE("Error: Headset_Speaker_Amp_Switch invalid value");
	}

	if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_ADC_2_Switch"), "Off"))
	{
		ALOGE("Error: Headset_Speaker_Amp_Switch invalid value");
	}

    return NO_ERROR;
}

AudioALSAHardwareResourceManager::AudioALSAHardwareResourceManager() :
    mMixer(AudioALSADriverUtility::getInstance()->getMixer()),
    mPcmDL(NULL),
    mOutputDevices(AUDIO_DEVICE_NONE),
    mInputDevice(AUDIO_DEVICE_NONE),
    mOutputDeviceSampleRate(44100),
    mInputDeviceSampleRate(48000),
    mIsChangingInputDevice(false),
    mStartOutputDevicesCount(0),
    mStartInputDeviceCount(0),
    mMicInverse(false),
    mBuiltInMicSpecificType(BUILTIN_MIC_DEFAULT),
    mNumHSPole(4),
    mHeadchange(false)
{
    ALOGD("%s()", __FUNCTION__);
	ResetDevice();	
    AudioALSACodecDeviceOutEarphonePMIC::getInstance();
    mDeviceConfigManager = AudioALSADeviceConfigManager::getInstance();
    setMicType();
#ifdef NXP_SMARTPA_SUPPORT
    AudioALSACodecDeviceOutSpeakerNXP::getInstance();
#endif
}


AudioALSAHardwareResourceManager::~AudioALSAHardwareResourceManager()
{
    ALOGD("%s()", __FUNCTION__);
}

/**
 * output devices
 */
status_t AudioALSAHardwareResourceManager::setOutputDevice(const audio_devices_t new_devices, const uint32_t sample_rate)
{
    ALOGD("+%s(), new_devices = 0x%x, mStartOutputDevicesCount = %u", __FUNCTION__, new_devices, mStartOutputDevicesCount);

    ASSERT(mStartOutputDevicesCount == 0);

    mOutputDevices = new_devices;
    mOutputDeviceSampleRate = sample_rate;

    return NO_ERROR;
}

status_t AudioALSAHardwareResourceManager::startOutputDevice(const audio_devices_t new_devices, const uint32_t SampleRate)
{
    ALOGD("+%s(), new_devices = 0x%x, mOutputDevices = 0x%x, mStartOutputDevicesCount = %u SampleRate = %d",
          __FUNCTION__, new_devices, mOutputDevices, mStartOutputDevicesCount, SampleRate);

    AudioAutoTimeoutLock _l(mLock);

    if (new_devices == mOutputDevices)
    {
        // don't need to do anything
    }
    else if(AUDIO_DEVICE_NONE != mOutputDevices)
    {
        changeOutputDevice_l(new_devices, SampleRate);
    }
    else
    {
        startOutputDevice_l(new_devices, SampleRate);
    }

    mStartOutputDevicesCount++;

    ALOGD("-%s(), mOutputDevices = 0x%x, mStartOutputDevicesCount = %u", __FUNCTION__, mOutputDevices, mStartOutputDevicesCount);
    return NO_ERROR;
}


status_t AudioALSAHardwareResourceManager::stopOutputDevice()
{
    ALOGD("+%s(), mOutputDevices = 0x%x, mStartOutputDevicesCount = %u", __FUNCTION__, mOutputDevices, mStartOutputDevicesCount);

    AudioAutoTimeoutLock _l(mLock);

    if (mStartOutputDevicesCount > 1)
    {
        // don't need to do anything
    }
    else
    {
        ASSERT(mStartOutputDevicesCount == 1 && mOutputDevices != AUDIO_DEVICE_NONE);
        stopOutputDevice_l();
    }

    mStartOutputDevicesCount--;

    ALOGD("-%s(), mOutputDevices = 0x%x, mStartOutputDevicesCount = %u", __FUNCTION__, mOutputDevices, mStartOutputDevicesCount);
    return NO_ERROR;
}


status_t AudioALSAHardwareResourceManager::changeOutputDevice(const audio_devices_t new_devices)
{
    ALOGD("+%s(), mOutputDevices: 0x%x => 0x%x", __FUNCTION__, mOutputDevices, new_devices);

    AudioAutoTimeoutLock _l(mLock);
    changeOutputDevice_l(new_devices, mOutputDeviceSampleRate);

    ALOGD("-%s(), mOutputDevices: 0x%x", __FUNCTION__, mOutputDevices);
    return NO_ERROR;
}


status_t AudioALSAHardwareResourceManager::startOutputDevice_l(const audio_devices_t new_devices, const uint32_t SampleRate)
{
    if (new_devices == (AUDIO_DEVICE_OUT_SPEAKER | AUDIO_DEVICE_OUT_WIRED_HEADSET) ||
        new_devices == (AUDIO_DEVICE_OUT_SPEAKER | AUDIO_DEVICE_OUT_WIRED_HEADPHONE))
    {
        OpenHeadphoneSpeakerPath(SampleRate);
    }
    else if (new_devices == AUDIO_DEVICE_OUT_EARPIECE)
    {
        OpenReceiverPath(SampleRate);
    }
    else if (new_devices == AUDIO_DEVICE_OUT_SPEAKER)
    {
        OpenSpeakerPath(SampleRate);
    }
    else if (new_devices == AUDIO_DEVICE_OUT_WIRED_HEADSET ||
             new_devices == AUDIO_DEVICE_OUT_WIRED_HEADPHONE)
    {
        OpenHeadphonePath(SampleRate);
    }

    mOutputDevices = new_devices;
    mOutputDeviceSampleRate = SampleRate;

    return NO_ERROR;
}


status_t AudioALSAHardwareResourceManager::stopOutputDevice_l()
{
    if (mOutputDevices == (AUDIO_DEVICE_OUT_SPEAKER | AUDIO_DEVICE_OUT_WIRED_HEADSET) ||
        mOutputDevices == (AUDIO_DEVICE_OUT_SPEAKER | AUDIO_DEVICE_OUT_WIRED_HEADPHONE))
    {
        CloseHeadphoneSpeakerPath();
    }
    else if (mOutputDevices == AUDIO_DEVICE_OUT_EARPIECE)
    {
        CloseReceiverPath();
    }
    else if (mOutputDevices == AUDIO_DEVICE_OUT_SPEAKER)
    {
        CloseSpeakerPath();
    }
    else if (mOutputDevices == AUDIO_DEVICE_OUT_WIRED_HEADSET ||
             mOutputDevices == AUDIO_DEVICE_OUT_WIRED_HEADPHONE)
    {
        CloseHeadphonePath();
    }


    mOutputDevices = AUDIO_DEVICE_NONE;

    return NO_ERROR;
}


status_t AudioALSAHardwareResourceManager::changeOutputDevice_l(const audio_devices_t new_devices, const uint32_t SampleRate)
{
    stopOutputDevice_l();
    startOutputDevice_l(new_devices, SampleRate);

    return NO_ERROR;
}


/**
 * input devices
 */
status_t AudioALSAHardwareResourceManager::setInputDevice(const audio_devices_t new_devices)
{
    ALOGD("+%s(), new_devices = 0x%x, mStartInputDeviceCount = %u", __FUNCTION__, new_devices, mStartInputDeviceCount);

    return NO_ERROR;
}

void AudioALSAHardwareResourceManager::setMIC1Mode(bool isphonemic)
{
    uint32_t micmode;

    if (isphonemic == false)
    {
        micmode = mPhoneMicMode;
    }
    else
    {
        micmode = mHeadsetMicMode;
    }

    ALOGD("%s(), isphonemic %d, micmode %d", __FUNCTION__, isphonemic, micmode);

    if (micmode == AUDIO_MIC_MODE_ACC)
    {
        mDeviceConfigManager->ApplyDeviceSettingByName(AUDIOMIC1_TYPE_ACCMODE);
        /*
        if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MIC1_Mode_Select"), "ACCMODE"))
        {
            ALOGE("%s(), Error: Audio_MIC1_Mode_Select invalid value", __FUNCTION__);
        }
        */
    }
    else if (micmode == AUDIO_MIC_MODE_DCC)
    {
        mDeviceConfigManager->ApplyDeviceSettingByName(AUDIOMIC1_TYPE_DCCMODE);
        /*
        if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MIC1_Mode_Select"), "DCCMODE"))
        {
            ALOGE("%s(), Error: Audio_MIC1_Mode_Select invalid value", __FUNCTION__);
        }
        */
    }
    else if ((micmode == AUDIO_MIC_MODE_DMIC) || (micmode == AUDIO_MIC_MODE_DMIC_LP))
    {
        if (IsAudioSupportFeature(AUDIO_SUPPORT_DMIC))
        {
            mDeviceConfigManager->ApplyDeviceSettingByName(AUDIOMIC1_TYPE_DMICMODE);
            /*
            if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MIC1_Mode_Select"), "DMIC"))
            {
                ALOGE("%s(), Error: Audio_MIC1_Mode_Select invalid value", __FUNCTION__);
            }
            */
        }
        else
        {
            mDeviceConfigManager->ApplyDeviceSettingByName(AUDIOMIC1_TYPE_ACCMODE);
            /*
            if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MIC1_Mode_Select"), "ACCMODE"))
            {
                ALOGE("%s(), Error: Audio_MIC1_Mode_Select invalid value", __FUNCTION__);
            }*/
        }
    }
    else if (micmode == AUDIO_MIC_MODE_DCCECMDIFF)
    {
        mDeviceConfigManager->ApplyDeviceSettingByName(AUDIOMIC1_TYPE_DCCECMDIFFMODE);
        /*
        if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MIC1_Mode_Select"), "DCCECMDIFFMODE"))
        {
            ALOGE("%s(), Error: Audio_MIC1_Mode_Select invalid value", __FUNCTION__);
        }
        */
    }
    else if (micmode == AUDIO_MIC_MODE_DCCECMSINGLE)
    {
        mDeviceConfigManager->ApplyDeviceSettingByName(AUDIOMIC1_TYPE_DCCECMSINGLEMODE);
        /*
        if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MIC1_Mode_Select"), "DCCECMSINGLEMODE"))
        {
            ALOGE("%s(), Error: Audio_MIC1_Mode_Select invalid value", __FUNCTION__);
        }
        */
    }
}

void AudioALSAHardwareResourceManager::setMIC2Mode(bool isphonemic)
{
    uint32_t micmode;

    if (isphonemic == false)
    {
        micmode = mPhoneMicMode;
    }
    else
    {
        micmode = mHeadsetMicMode;
    }

    ALOGD("%s(), isphonemic %d, micmode %d", __FUNCTION__, isphonemic, micmode);

    if (micmode == AUDIO_MIC_MODE_ACC)
    {
        mDeviceConfigManager->ApplyDeviceSettingByName(AUDIOMIC2_TYPE_ACCMODE);
        /*
        if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MIC2_Mode_Select"), "ACCMODE"))
        {
            ALOGE("%s(), Error: Audio_MIC2_Mode_Select invalid value", __FUNCTION__);
        }
        */
    }
    else if (micmode == AUDIO_MIC_MODE_DCC)
    {
        mDeviceConfigManager->ApplyDeviceSettingByName(AUDIOMIC2_TYPE_DCCMODE);
        /*
        if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MIC2_Mode_Select"), "DCCMODE"))
        {
            ALOGE("%s(), Error: Audio_MIC2_Mode_Select invalid value", __FUNCTION__);
        }
        */
    }
    else if ((micmode == AUDIO_MIC_MODE_DMIC) || (micmode == AUDIO_MIC_MODE_DMIC_LP))
    {
        if (IsAudioSupportFeature(AUDIO_SUPPORT_DMIC))
        {
            mDeviceConfigManager->ApplyDeviceSettingByName(AUDIOMIC2_TYPE_DMICMODE);
            /*
            if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MIC2_Mode_Select"), "DMIC"))
            {
                ALOGE("%s(), Error: Audio_MIC2_Mode_Select invalid value", __FUNCTION__);
            }
            */
        }
        else
        {
            mDeviceConfigManager->ApplyDeviceSettingByName(AUDIOMIC2_TYPE_ACCMODE);
            /*
            if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MIC2_Mode_Select"), "ACCMODE"))
            {
                ALOGE("%s(), Error: Audio_MIC2_Mode_Select invalid value", __FUNCTION__);
            }
            */
        }
    }
    else if (micmode == AUDIO_MIC_MODE_DCCECMDIFF)
    {
        mDeviceConfigManager->ApplyDeviceSettingByName(AUDIOMIC2_TYPE_DCCECMDIFFMODE);
        /*
        if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MIC2_Mode_Select"), "DCCECMDIFFMODE"))
        {
            ALOGE("%s(), Error: Audio_MIC2_Mode_Select invalid value", __FUNCTION__);
        }
        */
    }
    else if (micmode == AUDIO_MIC_MODE_DCCECMSINGLE)
    {
        mDeviceConfigManager->ApplyDeviceSettingByName(AUDIOMIC2_TYPE_DCCECMSINGLEMODE);
        /*
        if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MIC2_Mode_Select"), "DCCECMSINGLEMODE"))
        {
            ALOGE("%s(), Error: Audio_MIC2_Mode_Select invalid value", __FUNCTION__);
        }
        */
    }
}

status_t AudioALSAHardwareResourceManager::startInputDevice(const audio_devices_t new_device)
{
    ALOGD("+%s(), new_device = 0x%x, mInputDevice = 0x%x, mStartInputDeviceCount = %d", __FUNCTION__, new_device, mInputDevice,mStartInputDeviceCount);

    if (((mInputDevice & new_device) & ~AUDIO_DEVICE_BIT_IN) != 0)
    {
        ALOGW("%s(), input device already opened", __FUNCTION__);
        if (new_device != AUDIO_DEVICE_IN_SPK_FEED)
        {
            mStartInputDeviceCount++;
        }
        ALOGD("-%s(), mInputDevice = 0x%x, mStartInputDeviceCount = %d", __FUNCTION__, mInputDevice, mStartInputDeviceCount);
        return NO_ERROR;
    }

    int retval = 0;
    if (new_device == AUDIO_DEVICE_IN_BUILTIN_MIC)
    {
            setMIC1Mode(false);
            setMIC2Mode(false);
            /*
            retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MicSource1_Setting"), "ADC1");
            ASSERT(retval == 0);
            retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_ADC_1_Switch"), "On");
            ASSERT(retval == 0);

            retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_ADC_2_Switch"), "On");
            ASSERT(retval == 0);
            */

        if (mBuiltInMicSpecificType == BUILTIN_MIC_MIC1_ONLY)
        {
            mDeviceConfigManager->ApplyDeviceTurnonSequenceByName(AUDIO_DEVICE_BUILTIN_MIC_MIC1);
            /*
            retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_Preamp1_Switch"), "IN_ADC1");
            ASSERT(retval == 0);
            retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_Preamp2_Switch"), "IN_ADC3");
            ASSERT(retval == 0);
            */
        }
        else if (mBuiltInMicSpecificType == BUILTIN_MIC_DEFAULT)
        {
#ifdef MTK_DUAL_MIC_SUPPORT
            mDeviceConfigManager->ApplyDeviceTurnonSequenceByName(AUDIO_DEVICE_BUILTIN_DUAL_MIC);

            if (mMicInverse == true)
            {
                ALOGD("%s(), need MicInverse ", __FUNCTION__);
                mDeviceConfigManager->ApplyDeviceSettingByName(AUDIO_MIC_INVERSE);
                /*
                retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_Preamp1_Switch"), "IN_ADC3");
                ASSERT(retval == 0);
                retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_Preamp2_Switch"), "IN_ADC3");
                ASSERT(retval == 0);
                */
            }
            else
            {
                mDeviceConfigManager->ApplyDeviceSettingByName(AUDIO_MIC_NOINVERSE);
                /*
                retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_Preamp1_Switch"), "IN_ADC1");
                ASSERT(retval == 0);
                retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_Preamp2_Switch"), "IN_ADC1");
                ASSERT(retval == 0);
                */
            }
#else
            mDeviceConfigManager->ApplyDeviceTurnonSequenceByName(AUDIO_DEVICE_BUILTIN_SINGLE_MIC);
            /*
            retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_Preamp1_Switch"), "IN_ADC1");
            ASSERT(retval == 0);
            retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_Preamp2_Switch"), "IN_ADC3");
            ASSERT(retval == 0);
            */
#endif
        }
    }
#ifdef MTK_DUAL_MIC_SUPPORT
    else if (new_device == AUDIO_DEVICE_IN_BACK_MIC)
    {
        setMIC1Mode(false);
        setMIC2Mode(false);
        mDeviceConfigManager->ApplyDeviceTurnonSequenceByName(AUDIO_DEVICE_BUILTIN_BACK_MIC);
        /*
        retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MicSource1_Setting"), "ADC1");
        ASSERT(retval == 0);
        retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_ADC_1_Switch"), "On");
        ASSERT(retval == 0);
        retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_ADC_2_Switch"), "On");
        ASSERT(retval == 0);
        retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_Preamp1_Switch"), "IN_ADC3");
        ASSERT(retval == 0);
        retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_Preamp2_Switch"), "IN_ADC1");
        ASSERT(retval == 0);
        */
    }
#endif
    else if (new_device == AUDIO_DEVICE_IN_WIRED_HEADSET)
    {
        setMIC1Mode(true);
        setMIC2Mode(true);
        mDeviceConfigManager->ApplyDeviceTurnonSequenceByName(AUDIO_DEVICE_HEADSET_MIC);
        /*
        retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MicSource1_Setting"), "ADC2");
        ASSERT(retval == 0);
        retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_ADC_1_Switch"), "On");
        ASSERT(retval == 0);
        retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_ADC_2_Switch"), "On");
        ASSERT(retval == 0);
        retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_Preamp1_Switch"), "IN_ADC2");
        ASSERT(retval == 0);
        retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_Preamp2_Switch"), "IN_ADC2");
        ASSERT(retval == 0);
        */
    }
#if defined(MTK_SPEAKER_MONITOR_SUPPORT)
    else if (new_device == AUDIO_DEVICE_IN_SPK_FEED)
    {
        retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_ADC_3_Switch"), "On");
        ASSERT(retval == 0);
        retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_ADC_4_Switch"), "On");
        ASSERT(retval == 0);
        retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MicSource3_Setting"), "ADC3");
        ASSERT(retval == 0);
        retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MicSource4_Setting"), "ADC4");
        ASSERT(retval == 0);
    }
#endif

    mInputDevice |= new_device;
    if (new_device != AUDIO_DEVICE_IN_SPK_FEED)
    {
        mStartInputDeviceCount++;
    }

    ALOGD("-%s(), mInputDevice = 0x%x, mStartInputDeviceCount = %d", __FUNCTION__, mInputDevice, mStartInputDeviceCount);
    return NO_ERROR;
}


status_t AudioALSAHardwareResourceManager::stopInputDevice(const audio_devices_t stop_device)
{
    ALOGD("+%s(), mInputDevice = 0x%x, stop_device = 0x%x, mStartInputDeviceCount = %d", __FUNCTION__, mInputDevice, stop_device,mStartInputDeviceCount);

    if (((mInputDevice & stop_device) & ~AUDIO_DEVICE_BIT_IN) == 0)
    {
        ALOGW("%s(), input device not opened yet, do nothing", __FUNCTION__);
        return NO_ERROR;
    }

    if (stop_device != AUDIO_DEVICE_IN_SPK_FEED)
    {
        mStartInputDeviceCount--;
        if (mStartInputDeviceCount > 0)
        {
            ALOGD("-%s(), mInputDevice = 0x%x, mStartInputDeviceCount = %d", __FUNCTION__, mInputDevice, mStartInputDeviceCount);
            return NO_ERROR;
        }
    }

    int retval = 0;

    if (stop_device == AUDIO_DEVICE_IN_BUILTIN_MIC)
    {
        retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_Preamp1_Switch"), "OPEN");
        ASSERT(retval == 0);

        if (mBuiltInMicSpecificType == BUILTIN_MIC_MIC3_ONLY)
        {
            mDeviceConfigManager->ApplyDeviceTurnoffSequenceByName(AUDIO_DEVICE_BUILTIN_MIC_MIC3);
            //retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_ADC_3_Switch"), "Off");
            ASSERT(retval == 0);
        }
        else
        {
            setMIC1Mode(false);
            setMIC2Mode(false);
            /*
            retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_ADC_1_Switch"), "Off");
            ASSERT(retval == 0);
            */
#ifdef MTK_DUAL_MIC_SUPPORT
            mDeviceConfigManager->ApplyDeviceTurnoffSequenceByName(AUDIO_DEVICE_BUILTIN_DUAL_MIC);
            /*
            retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_ADC_2_Switch"), "Off");
            ASSERT(retval == 0);
            */
#else
            mDeviceConfigManager->ApplyDeviceTurnoffSequenceByName(AUDIO_DEVICE_BUILTIN_SINGLE_MIC);
#endif
        }
    }
#ifdef MTK_DUAL_MIC_SUPPORT
    else if (stop_device == AUDIO_DEVICE_IN_BACK_MIC)
    {
        setMIC1Mode(false);
        setMIC2Mode(false);
        mDeviceConfigManager->ApplyDeviceTurnoffSequenceByName(AUDIO_DEVICE_BUILTIN_BACK_MIC);
        /*
        retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_Preamp1_Switch"), "OPEN");
        ASSERT(retval == 0);

        retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_ADC_1_Switch"), "Off");
        ASSERT(retval == 0);

        retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_ADC_2_Switch"), "Off");
        ASSERT(retval == 0);
        */
    }
#endif
    else if (stop_device == AUDIO_DEVICE_IN_WIRED_HEADSET)
    {
        setMIC1Mode(true);
        mDeviceConfigManager->ApplyDeviceTurnoffSequenceByName(AUDIO_DEVICE_HEADSET_MIC);
        /*
        retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_Preamp1_Switch"), "OPEN");
        ASSERT(retval == 0);

        retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_ADC_1_Switch"), "Off");
        ASSERT(retval == 0);
        */
    }
#if defined(MTK_SPEAKER_MONITOR_SUPPORT)
    else if (stop_device == AUDIO_DEVICE_IN_SPK_FEED)
    {
        retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_ADC_3_Switch"), "Off");
        ASSERT(retval == 0);

        retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_ADC_4_Switch"), "Off");
        ASSERT(retval == 0);
    }
#endif

    mInputDevice &= ((~stop_device) | AUDIO_DEVICE_BIT_IN);
    if (mInputDevice == AUDIO_DEVICE_BIT_IN) { mInputDevice = AUDIO_DEVICE_NONE; }

    ALOGD("-%s(), mInputDevice = 0x%x, mStartInputDeviceCount = %d", __FUNCTION__, mInputDevice, mStartInputDeviceCount);
    return NO_ERROR;
}


status_t AudioALSAHardwareResourceManager::changeInputDevice(const audio_devices_t new_device)
{
    ALOGD("+%s(), mInputDevice: 0x%x => 0x%x", __FUNCTION__, mInputDevice, new_device);

    stopInputDevice(getInputDevice());
    startInputDevice(new_device);

    ALOGD("-%s(), mInputDevice: 0x%x", __FUNCTION__, mInputDevice);
    return NO_ERROR;
}



status_t AudioALSAHardwareResourceManager::setHWGain2DigitalGain(const uint32_t gain)
{
    ALOGD("%s(), gain = 0x%x", __FUNCTION__, gain);

    const uint32_t kMaxAudioHWGainValue = 0x80000;

    if (gain > kMaxAudioHWGainValue)
    {
        ALOGE("%s(), gain(0x%x) > kMaxAudioHWGainValue(0x%x)!! return!!", __FUNCTION__, gain, kMaxAudioHWGainValue);
        return BAD_VALUE;
    }

    int retval = 0;
    if (WCNChipController::GetInstance()->IsFMMergeInterfaceSupported() == true)
    {
        retval = mixer_ctl_set_value(mixer_get_ctl_by_name(mMixer, "Audio Mrgrx Volume"), 0, gain);
    }
    else
    {
        retval = mixer_ctl_set_value(mixer_get_ctl_by_name(mMixer, "Audio FM I2S Volume"), 0, gain);
    }
    ASSERT(retval == 0);

    return (retval == 0) ? NO_ERROR : UNKNOWN_ERROR;
}


status_t AudioALSAHardwareResourceManager::setInterruptRate(const uint32_t rate)
{
    if (rate <= 0 || rate >= 65535)
    {
        ALOGE("%s, rate is not in range", __FUNCTION__);
        return UNKNOWN_ERROR;
    }

#ifdef DOWNLINK_LOW_LATENCY
    int retval = mixer_ctl_set_value(mixer_get_ctl_by_name(mMixer, "Audio IRQ1 CNT"), 0, rate);
#else
    int retval = 0;
#endif

    ASSERT(retval == 0);

    return (retval == 0) ? NO_ERROR : UNKNOWN_ERROR;
}

status_t AudioALSAHardwareResourceManager::setInterruptRate2(const uint32_t rate)
{
    if (rate <= 0 || rate >= 65535)
    {
        ALOGE("%s, rate is not in range", __FUNCTION__);
        return UNKNOWN_ERROR;
    }

    //int retval = mixer_ctl_set_value(mixer_get_ctl_by_name(mMixer, "Audio IRQ2 CNT"), 0, rate);
    //ASSERT(retval == 0);

    //return (retval == 0) ? NO_ERROR : UNKNOWN_ERROR;
    return NO_ERROR;
}

status_t AudioALSAHardwareResourceManager::openAddaOutput(const uint32_t sample_rate)
{
    ALOGD("+%s(), sample_rate = 0x%x", __FUNCTION__, sample_rate);
    AudioAutoTimeoutLock _l(*AudioALSADriverUtility::getInstance()->getStreamSramDramLock());
    struct pcm_config mConfig;

    // DL  setting
    memset(&mConfig, 0, sizeof(mConfig));
    mConfig.channels = 2;
    mConfig.rate = sample_rate;
    mConfig.period_size = 1024;
    mConfig.period_count = 2;
    mConfig.format = PCM_FORMAT_S16_LE;
    mConfig.start_threshold = 0;
    mConfig.stop_threshold = 0;
    mConfig.silence_threshold = 0;

    ASSERT(mPcmDL == NULL);
    mPcmDL = pcm_open(0, 8, PCM_OUT, &mConfig);
    ASSERT(mPcmDL != NULL);
    ALOGV("%s(), mPcmDL = %p", __FUNCTION__, mPcmDL);

    pcm_start(mPcmDL);
    return NO_ERROR;
}


status_t AudioALSAHardwareResourceManager::closeAddaOutput()
{
    ALOGD("+%s()", __FUNCTION__);
    AudioAutoTimeoutLock _l(*AudioALSADriverUtility::getInstance()->getStreamSramDramLock());
    pcm_stop(mPcmDL);
    pcm_close(mPcmDL);
    mPcmDL = NULL;

    ALOGD("-%s()", __FUNCTION__);
    return NO_ERROR;
}


status_t AudioALSAHardwareResourceManager::setSgenMode(const sgen_mode_t sgen_mode)
{
    ALOGD("setSgenMode = %s", SIDEGEN[sgen_mode]);
    if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_SideGen_Switch"), SIDEGEN[sgen_mode]))
    {
        ALOGE("Error: Audio_SideGen_Switch invalid value");
    }

    return NO_ERROR;
}

status_t AudioALSAHardwareResourceManager::setSgenSampleRate(const sgen_mode_samplerate_t sample_rate)
{
    ALOGD("setSgenSampleRate = %d", sample_rate);
    if (sample_rate < 0 || sample_rate >= 9)
    {
        ALOGE("%s, sgen_rate is not in range", __FUNCTION__);
        return UNKNOWN_ERROR;
    }

    if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_SideGen_SampleRate"), SIDEGEN_SAMEPLRATE[sample_rate]))
    {
        ALOGE("Error: Audio_SideGen_SampleRate invalid value");
    }

    return NO_ERROR;
}

status_t AudioALSAHardwareResourceManager::EnableSideToneFilter(const bool enable)
{
    ALOGD("EnableSideToneFilter enable = %d", enable);
    if(enable)
    {
        mDeviceConfigManager->ApplyDeviceTurnoffSequenceByName(AUDIO_DEVICE_SIDETONE);
    }
    else
    {
        mDeviceConfigManager->ApplyDeviceTurnoffSequenceByName(AUDIO_DEVICE_SIDETONE);
    }
    /*
    if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_Sidetone_Switch"), enable ? "On" : "Off"))
    {
        ALOGE("Error: Audio_SideGen_Switch invalid value");
    }
    */

    return NO_ERROR;
}

bool AudioALSAHardwareResourceManager::GetExtDacPropertyEnable()
{
    return AudioALSADriverUtility::getInstance()->GetPropertyValue(PROPERTY_KEY_EXTDAC);
}

status_t AudioALSAHardwareResourceManager::SetExtDacGpioEnable(bool bEnable)
{
    int retval =0;
    // check for property first
    if(GetExtDacPropertyEnable() == true)
    {
        ALOGD("%s GetExtDacPropertyEnable bEnable = true",__FUNCTION__,bEnable);
        if(bEnable == true)
        {
            retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_I2S1_Setting"), "On");
            ASSERT(retval == 0);
        }
        else
        {
            retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_I2S1_Setting"), "Off");
            ASSERT(retval == 0);
        }
        return NO_ERROR;
    }

    // if supprot for extdac
    #ifdef EXT_DAC_SUPPORT
    if(bEnable == true)
    {
        retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_I2S1_Setting"), "On");
        ASSERT(retval == 0);
    }
    else
    {
        retval = mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_I2S1_Setting"), "Off");
        ASSERT(retval == 0);
    }
    #endif
    return NO_ERROR;
}


status_t AudioALSAHardwareResourceManager::setMicType(void)
{
    char value[PROPERTY_VALUE_MAX];
    int ret, bflag;

    mPhoneMicMode = GetMicDeviceMode(0);
    mHeadsetMicMode = GetMicDeviceMode(1);

    if (IsAudioSupportFeature(AUDIO_SUPPORT_DMIC))
    {
        mDeviceConfigManager->ApplyDeviceSettingByName(AUDIOMIC1_TYPE_DMICMODE);
        mDeviceConfigManager->ApplyDeviceSettingByName(AUDIOMIC2_TYPE_DMICMODE);
        mDeviceConfigManager->ApplyDeviceSettingByName(AUDIOMIC3_TYPE_DMICMODE);
        mDeviceConfigManager->ApplyDeviceSettingByName(AUDIOMIC4_TYPE_DMICMODE);
        /*
        if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MIC1_Mode_Select"), "DMIC"))
        {
            ALOGE("Error: Audio_MIC1_Mode_Select invalid value");
        }
        if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MIC2_Mode_Select"), "DMIC"))
        {
            ALOGE("Error: Audio_MIC2_Mode_Select invalid value");
        }
        if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MIC3_Mode_Select"), "DMIC"))
        {
            ALOGE("Error: Audio_MIC3_Mode_Select invalid value");
        }
        if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MIC4_Mode_Select"), "DMIC"))
        {
            ALOGE("Error: Audio_MIC4_Mode_Select invalid value");
        }
        */
    }
    else
    {
        mDeviceConfigManager->ApplyDeviceSettingByName(AUDIOMIC1_TYPE_ACCMODE);
        mDeviceConfigManager->ApplyDeviceSettingByName(AUDIOMIC2_TYPE_ACCMODE);
        /*
        if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MIC1_Mode_Select"), "ACCMODE"))
        {
            ALOGE("Error: Audio_MIC1_Mode_Select invalid value");
        }
        if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MIC2_Mode_Select"), "ACCMODE"))
        {
            ALOGE("Error: Audio_MIC2_Mode_Select invalid value");
        }
        */
#if defined(MTK_SPEAKER_MONITOR_SUPPORT)
        mDeviceConfigManager->ApplyDeviceSettingByName(AUDIOMIC3_TYPE_DCCMODE);
        mDeviceConfigManager->ApplyDeviceSettingByName(AUDIOMIC4_TYPE_DCCMODE);
        /*
        if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MIC3_Mode_Select"), "DCCMODE"))
        {
            ALOGE("Error: Audio_MIC3_Mode_Select invalid value");
        }
        if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MIC4_Mode_Select"), "DCCMODE"))
        {
            ALOGE("Error: Audio_MIC4_Mode_Select invalid value");
        }
        */
#else
        mDeviceConfigManager->ApplyDeviceSettingByName(AUDIOMIC3_TYPE_ACCMODE);
        mDeviceConfigManager->ApplyDeviceSettingByName(AUDIOMIC4_TYPE_ACCMODE);
        /*
        if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MIC3_Mode_Select"), "ACCMODE"))
        {
            ALOGE("Error: Audio_MIC3_Mode_Select invalid value");
        }
        if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_MIC4_Mode_Select"), "ACCMODE"))
        {
            ALOGE("Error: Audio_MIC4_Mode_Select invalid value");
        }
        */
#endif
    }
    return NO_ERROR;
}


status_t AudioALSAHardwareResourceManager::setSPKCurrentSensor(bool bSwitch)
{
    struct mixer_ctl *ctl;
    struct mixer_ctl *ctl2;

    enum mixer_ctl_type type;
    unsigned int num_values;
    static AUDIO_SPEAKER_MODE eSpeakerMode;
    ALOGD("%s(), bSwitch = %d", __FUNCTION__, bSwitch);
    ctl = mixer_get_ctl_by_name(mMixer, "Audio_Speaker_CurrentSensing");

    if (ctl == NULL)
    {
        ALOGE("Kernel unsupport Audio_Speaker_CurrentSensing");
        return UNKNOWN_ERROR;
    }

    if (mixer_ctl_set_enum_by_string(ctl, bSwitch ? "On" : "Off"))
    {
        ALOGE("Error: Audio_Speaker_CurrentSensing invalid value : On");
    }


    ctl2 = mixer_get_ctl_by_name(mMixer, "Audio_Speaker_class_Switch");

    if (true == bSwitch)
    {
        //Save Speaker Mode
        ALOGD("Enable+ value [%d] [%s]", mixer_ctl_get_value(ctl2, 0), mixer_ctl_get_enum_string(ctl2, mixer_ctl_get_value(ctl2, 0)));

        if (strcmp(mixer_ctl_get_enum_string(ctl2, mixer_ctl_get_value(ctl2, 0)), "CLASSAB"))
        {
            eSpeakerMode = AUDIO_SPEAKER_MODE_D;
        }
        else
        {
            eSpeakerMode = AUDIO_SPEAKER_MODE_AB;
        }

        ALOGD("Current Mode [%d]", eSpeakerMode);

        if (mixer_ctl_set_enum_by_string(ctl2, "CLASSAB"))
        {
            ALOGE("Error: Audio_Speaker_CurrentPeakDetector invalid value");
        }

        ALOGD("Enable- [%s]", mixer_ctl_get_enum_string(ctl2, mixer_ctl_get_value(ctl2, 0)));

    }
    else
    {
        //restore Speaker Mode

        if (mixer_ctl_set_enum_by_string(ctl2, eSpeakerMode ? "CLASSAB" : "CALSSD"))
        {
            ALOGE("Error: Audio_Speaker_CurrentPeakDetector invalid value");
        }

        ALOGD("RollBack to [%s]", mixer_ctl_get_enum_string(ctl2, 0));
    }


    ALOGD("Audio_Speaker_CurrentSensing Get value [%d] [%s]", mixer_ctl_get_value(ctl, 0), mixer_ctl_get_enum_string(ctl, mixer_ctl_get_value(ctl, 0)));

    return NO_ERROR;
}

status_t AudioALSAHardwareResourceManager::setSPKCurrentSensorPeakDetectorReset(bool bSwitch)
{
    struct mixer_ctl *ctl;
    enum mixer_ctl_type type;
    unsigned int num_values;
    ALOGD("%s(), bSwitch = %d", __FUNCTION__, bSwitch);
    ctl = mixer_get_ctl_by_name(mMixer, "Audio_Speaker_CurrentPeakDetector");

    if (ctl == NULL)
    {
        ALOGE("Kernel unsupport Audio_Speaker_CurrentPeakDetector");
        return UNKNOWN_ERROR;
    }

    type = mixer_ctl_get_type(ctl);
    num_values = mixer_ctl_get_num_values(ctl);
    if (mixer_ctl_set_enum_by_string(ctl, bSwitch ? "On" : "Off"))
    {
        ALOGE("Error: Audio_Speaker_CurrentPeakDetector invalid value");
    }

    return NO_ERROR;
}


audio_devices_t AudioALSAHardwareResourceManager::getOutputDevice()
{
    return mOutputDevices;
}

audio_devices_t AudioALSAHardwareResourceManager::getInputDevice()
{
    return mInputDevice & ((~AUDIO_DEVICE_IN_SPK_FEED) | AUDIO_DEVICE_BIT_IN);
}


status_t AudioALSAHardwareResourceManager::setMicInverse(bool bMicInverse)
{
    ALOGD("%s(), bMicInverse = %d", __FUNCTION__, bMicInverse);
#ifdef MTK_DUAL_MIC_SUPPORT
    mMicInverse = bMicInverse;
#else
    ALOGD("%s(), not support", __FUNCTION__);
#endif
    return NO_ERROR;
}

void AudioALSAHardwareResourceManager::EnableAudBufClk(bool bEanble)
{
    ALOGD("%s(), bEanble = %d", __FUNCTION__, bEanble);
    enum mixer_ctl_type type;
    struct mixer_ctl *ctl;
    int retval = 0;
    ctl = mixer_get_ctl_by_name(mMixer, "AUD_CLK_BUF_Switch");

    if (ctl == NULL)
    {
        ALOGE("EnableAudBufClk not support");
        return ;
    }

    if (bEanble == true)
    {
        retval = mixer_ctl_set_enum_by_string(ctl, "On");
        ASSERT(retval == 0);
    }
    else
    {
        retval = mixer_ctl_set_enum_by_string(ctl, "Off");
        ASSERT(retval == 0);
    }
}


bool AudioALSAHardwareResourceManager::getMicInverse(void)
{
    ALOGD("%s(), mMicInverse = %d", __FUNCTION__, mMicInverse);
    return mMicInverse;
}


void AudioALSAHardwareResourceManager::setAudioDebug(const bool enable)
{
    if (mixer_ctl_set_enum_by_string(mixer_get_ctl_by_name(mMixer, "Audio_Debug_Setting"), enable ? "On" : "Off"));
    {
        ALOGE("Error: Audio_Debug_Setting invalid value");
    }
}

status_t AudioALSAHardwareResourceManager::OpenReceiverPath(const uint32_t SampleRate)
{
    //AudioALSACodecDeviceOutReceiverPMIC::getInstance()->open();
    mDeviceConfigManager->ApplyDeviceTurnonSequenceByName(AUDIO_DEVICE_RECEIVER);
    return NO_ERROR;
}

status_t AudioALSAHardwareResourceManager::CloseReceiverPath()
{
    ALOGD("%s(), AUDIO_DEVICE_OUT_EARPIECE", __FUNCTION__);
    //AudioALSACodecDeviceOutReceiverPMIC::getInstance()->close();
    mDeviceConfigManager->ApplyDeviceTurnoffSequenceByName(AUDIO_DEVICE_RECEIVER);
    return NO_ERROR;
}

status_t AudioALSAHardwareResourceManager::OpenHeadphonePath(const uint32_t SampleRate)
{
    SetExtDacGpioEnable(true);
    //AudioALSACodecDeviceOutEarphonePMIC::getInstance()->open();
    mDeviceConfigManager->ApplyDeviceTurnonSequenceByName(AUDIO_DEVICE_HEADPHONE);
    return NO_ERROR;
}

status_t AudioALSAHardwareResourceManager::CloseHeadphonePath()
{
    ALOGD("%s(), AUDIO_DEVICE_OUT_WIRED_HEADSET | AUDIO_DEVICE_OUT_WIRED_HEADPHONE", __FUNCTION__);
    //AudioALSACodecDeviceOutEarphonePMIC::getInstance()->close();
    mDeviceConfigManager->ApplyDeviceTurnoffSequenceByName(AUDIO_DEVICE_HEADPHONE);
    return NO_ERROR;
}


status_t AudioALSAHardwareResourceManager::OpenSpeakerPath(const uint32_t SampleRate)
{
    SetExtDacGpioEnable(true);
#ifdef NXP_SMARTPA_SUPPORT
    AudioALSACodecDeviceOutSpeakerNXP::getInstance()->open(SampleRate);
#else
#ifndef USING_EXTAMP_HP
    mDeviceConfigManager->ApplyDeviceTurnonSequenceByName(AUDIO_DEVICE_SPEAKER);
    //AudioALSACodecDeviceOutSpeakerPMIC::getInstance()->open();
#else
    mDeviceConfigManager->ApplyDeviceTurnonSequenceByName(AUDIO_DEVICE_HEADPHONE);
    mDeviceConfigManager->ApplyDeviceTurnonSequenceByName(AUDIO_DEVICE_EXT_SPEAKER);
    //AudioALSACodecDeviceOutEarphonePMIC::getInstance()->open();
    //AudioALSACodecDeviceOutExtSpeakerAmp::getInstance()->open();
#endif
#endif
    return NO_ERROR;
}

status_t  AudioALSAHardwareResourceManager::CloseSpeakerPath()
{
    ALOGD("%s(), AUDIO_DEVICE_OUT_SPEAKER", __FUNCTION__);
#ifdef NXP_SMARTPA_SUPPORT
    AudioALSACodecDeviceOutSpeakerNXP::getInstance()->close();
#else
#ifndef USING_EXTAMP_HP
    mDeviceConfigManager->ApplyDeviceTurnoffSequenceByName(AUDIO_DEVICE_SPEAKER);
    //AudioALSACodecDeviceOutSpeakerPMIC::getInstance()->close();
#else
    mDeviceConfigManager->ApplyDeviceTurnoffSequenceByName(AUDIO_DEVICE_EXT_SPEAKER);
    mDeviceConfigManager->ApplyDeviceTurnoffSequenceByName(AUDIO_DEVICE_HEADPHONE);
    //AudioALSACodecDeviceOutExtSpeakerAmp::getInstance()->close();
    //AudioALSACodecDeviceOutEarphonePMIC::getInstance()->close();
#endif
#endif
    SetExtDacGpioEnable(false);
    return NO_ERROR;
}

status_t  AudioALSAHardwareResourceManager::OpenHeadphoneSpeakerPath(const uint32_t SampleRate)
{
    SetExtDacGpioEnable(true);
#ifdef NXP_SMARTPA_SUPPORT
    AudioALSACodecDeviceOutSpeakerNXP::getInstance()->open(SampleRate);
    mDeviceConfigManager->ApplyDeviceTurnonSequenceByName(AUDIO_DEVICE_HEADPHONE);
    //AudioALSACodecDeviceOutEarphonePMIC::getInstance()->open();
#else
#ifndef USING_EXTAMP_HP //defined in audio_custom_exp.h
    //AudioALSACodecDeviceOutSpeakerEarphonePMIC::getInstance()->open();
    mDeviceConfigManager->ApplyDeviceTurnonSequenceByName(AUDIO_DEVICE_EARPHONE_SPEAKER);
#else
    mDeviceConfigManager->ApplyDeviceTurnonSequenceByName(AUDIO_DEVICE_HEADPHONE);
    mDeviceConfigManager->ApplyDeviceTurnonSequenceByName(AUDIO_DEVICE_EXT_SPEAKER);
    //AudioALSACodecDeviceOutEarphonePMIC::getInstance()->open();
    //AudioALSACodecDeviceOutExtSpeakerAmp::getInstance()->open();
#endif
#endif

    return NO_ERROR;
}

status_t  AudioALSAHardwareResourceManager::CloseHeadphoneSpeakerPath()
{
#ifdef NXP_SMARTPA_SUPPORT
    AudioALSACodecDeviceOutSpeakerNXP::getInstance()->close();
    //AudioALSACodecDeviceOutEarphonePMIC::getInstance()->close();
    mDeviceConfigManager->ApplyDeviceTurnoffSequenceByName(AUDIO_DEVICE_HEADPHONE);
#else
#ifndef USING_EXTAMP_HP //defined in audio_custom_exp.h
    //AudioALSACodecDeviceOutSpeakerEarphonePMIC::getInstance()->close();
    mDeviceConfigManager->ApplyDeviceTurnoffSequenceByName(AUDIO_DEVICE_EARPHONE_SPEAKER);
#else
    mDeviceConfigManager->ApplyDeviceTurnoffSequenceByName(AUDIO_DEVICE_EXT_SPEAKER);
    mDeviceConfigManager->ApplyDeviceTurnoffSequenceByName(AUDIO_DEVICE_HEADPHONE);
    //AudioALSACodecDeviceOutExtSpeakerAmp::getInstance()->close();
    //AudioALSACodecDeviceOutEarphonePMIC::getInstance()->close();
#endif
#endif
    SetExtDacGpioEnable(false);
    return NO_ERROR;
}

status_t  AudioALSAHardwareResourceManager::OpenBuiltInMicPath()
{
    return NO_ERROR;
}

status_t  AudioALSAHardwareResourceManager::CloseBuiltInMicPath()
{
    return NO_ERROR;
}

status_t  AudioALSAHardwareResourceManager::OpenBackMicPath()
{
    return NO_ERROR;
}

status_t  AudioALSAHardwareResourceManager::CloseBackMicPath()
{
    return NO_ERROR;
}

status_t  AudioALSAHardwareResourceManager::OpenWiredHeadsetMicPath()
{
    return NO_ERROR;
}

status_t  AudioALSAHardwareResourceManager::CloseWiredHeadsetMicPath()
{
    return NO_ERROR;
}




} // end of namespace android
