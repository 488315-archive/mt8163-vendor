#include "AudioALSAPlaybackHandlerHDMI.h"
#include <sys/ioctl.h>
#include <linux/hdmitx.h>
#include <stdio.h>
#include <fcntl.h>

#include "AudioALSADriverUtility.h"
#include "audio_custom_exp.h"

#ifdef MTK_BASIC_PACKAGE
#include "AudioTypeExt.h"
#endif

#define LOG_TAG "AudioALSAPlaybackHandlerHDMI"
//#define __2CH_TO_8CH
//#define _TDM_DEBUG
//#define DEBUG_LATENCY

#define calc_time_diff(x,y) ((x.tv_sec - y.tv_sec )+ (double)( x.tv_nsec - y.tv_nsec ) / (double)1000000000)
//#define HDMI_DEV_DRV "/dev/hdmitx"


namespace android
{

#ifdef _TDM_DEBUG
static FILE *pOutFile = NULL;
static FILE *pOutFileorg = NULL;
#endif

AudioALSAPlaybackHandlerHDMI::AudioALSAPlaybackHandlerHDMI(const stream_attribute_t *stream_attribute_source) :
    AudioALSAPlaybackHandlerBase(stream_attribute_source)
{
    ALOGD("%s()", __FUNCTION__);
    mPlaybackHandlerType = PLAYBACK_HANDLER_HDMI;
}

AudioALSAPlaybackHandlerHDMI::~AudioALSAPlaybackHandlerHDMI()
{
    ALOGD("%s()", __FUNCTION__);
}

status_t AudioALSAPlaybackHandlerHDMI::SetMHLChipParameter(int channels, int bits, int samplerate)
{
    ALOGD("%s MTK_HDMI_AUDIO_SETTING\n", __FUNCTION__);
    int fd = ::open(HDMI_DEV_DRV, O_RDWR);
    if (fd == -1)
    {
        ALOGE("%s open %s fail errno = %d\n", __FUNCTION__, HDMI_DEV_DRV, fd);
        return -1;
    }
    else
    {
        ALOGD("%s open %s success, fd = %d\n", __FUNCTION__, HDMI_DEV_DRV, fd);
    }

    HDMITX_AUDIO_PARA HdmiAudioParam;
    HdmiAudioParam.e_hdmi_aud_in = SV_I2S;
    HdmiAudioParam.e_aud_code = AVD_LPCM;
    HdmiAudioParam.e_I2sFmt = HDMI_I2S_24BIT;
    HdmiAudioParam.u1HdmiI2sMclk = MCLK_128FS;

    switch (samplerate)
    {
        case 192000:
            HdmiAudioParam.e_iec_frame = IEC_192K;
            HdmiAudioParam.e_hdmi_fs = HDMI_FS_192K;
            HdmiAudioParam.bhdmi_LCh_status[3] = 0xE;
            break;
        case 176400:
            HdmiAudioParam.e_iec_frame = IEC_176K;
            HdmiAudioParam.e_hdmi_fs = HDMI_FS_176K;
            HdmiAudioParam.bhdmi_LCh_status[3] = 0xC;
            break;
        case 96000:
            HdmiAudioParam.e_iec_frame = IEC_96K;
            HdmiAudioParam.e_hdmi_fs = HDMI_FS_96K;
            HdmiAudioParam.bhdmi_LCh_status[3] = 0xA;
            break;
        case 88200:
            HdmiAudioParam.e_iec_frame = IEC_88K;
            HdmiAudioParam.e_hdmi_fs = HDMI_FS_88K;
            HdmiAudioParam.bhdmi_LCh_status[3] = 0x8;
            break;
        case 48000:
            HdmiAudioParam.e_iec_frame = IEC_48K;
            HdmiAudioParam.e_hdmi_fs = HDMI_FS_48K;
            HdmiAudioParam.bhdmi_LCh_status[3] = 0x2;
            break;
        case 44100:
            HdmiAudioParam.e_iec_frame = IEC_44K;
            HdmiAudioParam.e_hdmi_fs = HDMI_FS_44K;
            HdmiAudioParam.bhdmi_LCh_status[3] = 0x0;
            break;
        case 32000:
            HdmiAudioParam.e_iec_frame = IEC_32K;
            HdmiAudioParam.e_hdmi_fs = HDMI_FS_32K;
            HdmiAudioParam.bhdmi_LCh_status[3] = 0x3;
            break;
        default:
            ALOGW("%s unsupported sample rate %d\n", __FUNCTION__, samplerate);
            break;
    }
   
    switch (channels)
    {
        case 1:
            HdmiAudioParam.u1Aud_Input_Chan_Cnt = AUD_INPUT_1_0;
            break;
        case 2:
            HdmiAudioParam.u1Aud_Input_Chan_Cnt = AUD_INPUT_2_0;
            break;
        case 4:
            HdmiAudioParam.u1Aud_Input_Chan_Cnt = AUD_INPUT_4_0;
            break;
        case 6:
            HdmiAudioParam.u1Aud_Input_Chan_Cnt = AUD_INPUT_5_1;
            break;
        case 8:
            HdmiAudioParam.u1Aud_Input_Chan_Cnt = AUD_INPUT_7_1;
            break;
        default:
            ALOGW("%s unsupported channel num %d\n", __FUNCTION__, channels);
            break;
    }

    HdmiAudioParam.bhdmi_LCh_status[0] = CHANNEL_STATUS_COPY_BIT << 2;
    HdmiAudioParam.bhdmi_LCh_status[1] = CHANNEL_STATUS_CATEGORY_CODE;
    HdmiAudioParam.bhdmi_LCh_status[2] = 0;
    HdmiAudioParam.bhdmi_LCh_status[4] = 0x2;
    memcpy(HdmiAudioParam.bhdmi_RCh_status, HdmiAudioParam.bhdmi_LCh_status, sizeof(HdmiAudioParam.bhdmi_LCh_status));
    
    int ret = ::ioctl(fd, MTK_HDMI_AUDIO_SETTING, &HdmiAudioParam);
    if (ret != 0)
    {
        ALOGE("%s ioctl MTK_HDMI_AUDIO_SETTING fail errno = %d\n", __FUNCTION__, errno);
    }

    ::close(fd);
    return ret;
}

status_t AudioALSAPlaybackHandlerHDMI::SetMHLChipEnable(int enable)
{
    ALOGD("+%s()", __FUNCTION__);

#if defined(MTK_HDMI_SUPPORT)
    // File descriptor
    int fd_audio = ::open(HDMI_DEV_DRV, O_RDWR);
    ALOGD("%s(), open(%s), fd_audio = %d", __FUNCTION__, HDMI_DEV_DRV, fd_audio);

    if (fd_audio >= 0)
    {
        ::ioctl(fd_audio, MTK_HDMI_AUDIO_ENABLE, enable);

        ALOGD("%s(), ioctl:MTK_HDMI_AUDIO_ENABLE =0x%x \n", __FUNCTION__, enable);

        ::close(fd_audio);
    }
    ALOGD("-%s(), fd_audio=%d", __FUNCTION__, fd_audio);
#endif

    return NO_ERROR;
}



status_t AudioALSAPlaybackHandlerHDMI::open()
{
    ALOGD("+%s(), mDevice = 0x%x", __FUNCTION__, mStreamAttributeSource->output_devices);
    AudioAutoTimeoutLock _l(*AudioALSADriverUtility::getInstance()->getStreamSramDramLock());

#ifdef _TDM_DEBUG

#ifdef __2CH_TO_8CH
    pOutFile = fopen("/sdcard/mtklog/hdmi8ch.pcm", "wb");
    if (pOutFile == NULL)
    {
        ALOGW("%s(), open file fail ", __FUNCTION__);
    }
#endif

    pOutFileorg = fopen("/sdcard/mtklog/hdmi.pcm", "wb");
    if (pOutFileorg == NULL)
    {
        ALOGW("%s(), open file fail ", __FUNCTION__);
    }
#else
    OpenPCMDump(LOG_TAG);
#endif

	ALOGD("%s(), mStreamAttributeSource: format = %d",__FUNCTION__, mStreamAttributeSource->audio_format);


//    mStreamAttributeTarget.audio_format = AUDIO_FORMAT_PCM_16_BIT;
    mStreamAttributeTarget.audio_format = (mStreamAttributeSource->audio_format == AUDIO_FORMAT_PCM_32_BIT || mStreamAttributeSource->audio_format == AUDIO_FORMAT_PCM_8_24_BIT) ? AUDIO_FORMAT_PCM_8_24_BIT : AUDIO_FORMAT_PCM_16_BIT;

    mStreamAttributeTarget.audio_channel_mask = mStreamAttributeSource->audio_channel_mask;
    mStreamAttributeTarget.num_channels = android_audio_legacy::AudioSystem::popCount(mStreamAttributeTarget.audio_channel_mask);
    mStreamAttributeTarget.sample_rate = mStreamAttributeSource->sample_rate; // same as source stream

    ALOGD("%s(), mStreamAttributeTarget: channels = %d, rate = %d, format = %d",
          __FUNCTION__, mStreamAttributeTarget.num_channels, mStreamAttributeTarget.sample_rate, mStreamAttributeTarget.audio_format);

    // HW pcm config
    mConfig.channels = mStreamAttributeTarget.num_channels;
    mConfig.rate = mStreamAttributeTarget.sample_rate;


#ifdef __2CH_TO_8CH
    mConfig.period_size = 1536*4;
    mConfig.period_count = 8;
#else
    if (mStreamAttributeTarget.num_channels == 2)
    {
        //mConfig.period_size = 3072*2;
        //mConfig.period_count = 2;
        mConfig.period_size = 1024;
        mConfig.period_count = 8;
        mConfig.channels = 2;
    }
    else if (mStreamAttributeTarget.num_channels == 4) 
    {
        mConfig.period_size = 3072*2;
        mConfig.period_count = 2;
        mConfig.channels = 4;
    }
    else
    {
		if (mStreamAttributeSource->audio_format == AUDIO_FORMAT_PCM_16_BIT)
		{
            mConfig.period_size = 3072*2;
			mConfig.period_count = 2;
			mConfig.channels = 8;		
		}
		else
		{
            mConfig.period_size = 3072*2*2;
			mConfig.period_count = 2;
			mConfig.channels = 8;
		}
    }
#endif

    mConfig.format = transferAudioFormatToPcmFormat(mStreamAttributeTarget.audio_format);

    SetMHLChipParameter(mConfig.channels, mStreamAttributeTarget.audio_format, mConfig.rate);
    mConfig.start_threshold = 0;
    mConfig.stop_threshold = 0;
    mConfig.silence_threshold = 0;
    ALOGD("%s(), mConfig: channels = %d, rate = %d, period_size = %d, period_count = %d, format = %d",
          __FUNCTION__, mConfig.channels, mConfig.rate, mConfig.period_size, mConfig.period_count, mConfig.format);

    if (mStreamAttributeSource->output_devices != 0)
    {
        if ((AUDIO_DEVICE_OUT_AUX_DIGITAL & mStreamAttributeSource->output_devices) == 0)
        {
            SetMHLChipEnable(false);
        }
        else
        {
            SetMHLChipEnable(true);
        }
    }


    // SRC
    initBliSrc();


    // bit conversion
    initBitConverter();


    // open pcm driver
    openPcmDriver(3);

#if 0
    ASSERT(mPcm == NULL);
    mPcm = pcm_open(0, 3, PCM_OUT, &mConfig);

    if (mPcm == NULL)
    {
        ALOGE("%s(), mPcm == NULL!!", __FUNCTION__);
    }
    else if (pcm_is_ready(mPcm) == false)
    {
        ALOGE("%s(), pcm_is_ready(%p) == false due to %s, close pcm.", __FUNCTION__, mPcm, pcm_get_error(mPcm));
        pcm_close(mPcm);
        mPcm = NULL;
    }
    else
    {
        pcm_start(mPcm);
    }

    ALOGD("-%s(), mPcm = %p", __FUNCTION__, mPcm);
    ASSERT(mPcm != NULL);

#endif

    ALOGD("-%s()", __FUNCTION__);
    return NO_ERROR;
}


status_t AudioALSAPlaybackHandlerHDMI::close()
{
    ALOGD("+%s()", __FUNCTION__);
    AudioAutoTimeoutLock _l(*AudioALSADriverUtility::getInstance()->getStreamSramDramLock());

    SetMHLChipEnable(false);

    // close pcm driver
    closePcmDriver();


    // bit conversion
    deinitBitConverter();


    // SRC
    deinitBliSrc();


#ifdef _TDM_DEBUG

#ifdef __2CH_TO_8CH
    if (pOutFile != NULL)
    {
        fclose(pOutFile);
    }
#endif
    if (pOutFileorg != NULL)
    {
        fclose(pOutFileorg);
    }
#else
    ClosePCMDump();
#endif


    ALOGD("-%s()", __FUNCTION__);
    return NO_ERROR;
}


status_t AudioALSAPlaybackHandlerHDMI::routing(const audio_devices_t output_devices)
{
    return INVALID_OPERATION;
}

status_t AudioALSAPlaybackHandlerHDMI::pause()
{
    return INVALID_OPERATION;
}

status_t AudioALSAPlaybackHandlerHDMI::resume()
{
    return INVALID_OPERATION;
}

status_t AudioALSAPlaybackHandlerHDMI::flush()
{
    return INVALID_OPERATION;
}

status_t AudioALSAPlaybackHandlerHDMI::setVolume(uint32_t vol)
{
    return INVALID_OPERATION;
}


int AudioALSAPlaybackHandlerHDMI::drain(audio_drain_type_t type)
{
    return 0;
}    


ssize_t AudioALSAPlaybackHandlerHDMI::write(const void *buffer, size_t bytes)
{
    ALOGV("%s(), buffer = %p, bytes = %d", __FUNCTION__, buffer, bytes);

    if (mPcm == NULL)
    {
        ALOGE("%s(), mPcm == NULL, return", __FUNCTION__);
        return bytes;
    }

    // work around for incall mode HDMI is playing
    if ((mStreamAttributeSource->audio_mode ==  AUDIO_MODE_IN_CALL)
        || (mStreamAttributeSource->audio_mode ==  AUDIO_MODE_IN_CALL_2)
        || (mStreamAttributeSource->audio_mode ==  AUDIO_MODE_IN_CALL_EXTERNAL))
    {
        usleep(30 * 1000);
        return bytes;
    }
    // const -> to non const
    void *pBuffer = const_cast<void *>(buffer);
    ASSERT(pBuffer != NULL);

#ifdef DEBUG_LATENCY
    clock_gettime(CLOCK_REALTIME, &mNewtime);
    latencyTime[0] = calc_time_diff(mNewtime, mOldtime);
    mOldtime = mNewtime;
#endif

    // SRC
    void *pBufferAfterBliSrc = NULL;
    uint32_t bytesAfterBliSrc = 0;
    doBliSrc(pBuffer, bytes, &pBufferAfterBliSrc, &bytesAfterBliSrc);


    // bit conversion
    void *pBufferAfterBitConvertion = NULL;
    uint32_t bytesAfterBitConvertion = 0;
    doBitConversion(pBufferAfterBliSrc, bytesAfterBliSrc, &pBufferAfterBitConvertion, &bytesAfterBitConvertion);

    void *newbuffer[96 * 1024] = {0};
    unsigned char *aaa;
    unsigned char *bbb;
    int i = 0;
    int j = 0;
	int retval = 0;

    aaa = (unsigned char *)newbuffer;
    bbb = (unsigned char *)pBufferAfterBitConvertion;
	

#ifdef __2CH_TO_8CH


    for (i = 0 ; j < bytesAfterBitConvertion; i += 16)
    {
        memcpy(aaa + i,      bbb + j, 4);
        memcpy(aaa + i + 4,  bbb + j, 4);
        memcpy(aaa + i + 8,  bbb + j, 4);
        memcpy(aaa + i + 12, bbb + j, 4);
        j += 4;
    }

    // write data to pcm driver
    retval = pcm_write(mPcm, newbuffer, bytesAfterBitConvertion << 2);
#else
    ALOGV("%s(), channels = %d, format = %d ,bytes = %d", __FUNCTION__, mStreamAttributeTarget.num_channels, mStreamAttributeTarget.audio_format, bytes);

	if (mStreamAttributeTarget.num_channels == 6) 
	{
		if (mStreamAttributeTarget.audio_format == AUDIO_FORMAT_PCM_16_BIT)
		{
			aaa = (unsigned char *)newbuffer;
			bbb = (unsigned char *)pBuffer;
			
			for (i = 0 ; j < bytesAfterBitConvertion; i += 16)
			{
				memcpy(aaa + i, 	 bbb + j, 12);
//				memcpy(aaa + i, 	 bbb + j + 4, 2); //test copy center to left
//				memcpy(aaa + i + 4, 	 bbb + j + 2, 2); //test copy right to center                             
				memset(aaa + i + 12, 0 , 4);
				j += 12;
			}
		    retval = pcm_write(mPcm, newbuffer, ((bytesAfterBitConvertion*4)/3));
		}
		else
		{
			for (i = 0 ; j < bytesAfterBitConvertion; i += 32)
			{
				memcpy(aaa + i, 	 bbb + j, 24);
				memset(aaa + i + 24, 0 , 8);
				j += 24;
			}
		    retval = pcm_write(mPcm, newbuffer, ((bytesAfterBitConvertion*4)/3));

		}		
	}
	else
	{		
	    // write data to pcm driver
#ifdef DEBUG_LATENCY
	    clock_gettime(CLOCK_REALTIME, &mNewtime);
        latencyTime[1] = calc_time_diff(mNewtime, mOldtime);
        mOldtime = mNewtime;
#endif
        
	    retval = pcm_write(mPcm, pBufferAfterBitConvertion, bytesAfterBitConvertion);

#ifdef DEBUG_LATENCY
        clock_gettime(CLOCK_REALTIME, &mNewtime);
        latencyTime[2] = calc_time_diff(mNewtime, mOldtime);
        mOldtime = mNewtime;
#endif
	}
    // write data to pcm driver
#endif


    if (retval != 0)
    {
        ALOGE("%s(), pcm_write() error, retval = %d", __FUNCTION__, retval);
    }


#ifdef _TDM_DEBUG

#ifdef __2CH_TO_8CH

    if (pOutFile != NULL)
    {
        ALOGD("%s(), newbuffer = %p", __FUNCTION__, newbuffer);
        fwrite(newbuffer, sizeof(char), bytesAfterBitConvertion * 4, pOutFile);
    }
#endif
    if (pOutFileorg != NULL)
    {
        ALOGD("%s(), pBufferAfterBitConvertion = %p", __FUNCTION__, pBufferAfterBitConvertion);
        fwrite(pBufferAfterBitConvertion, sizeof(char), bytesAfterBitConvertion, pOutFileorg);
    }
#else
	if (mStreamAttributeTarget.num_channels == 6) 
	{
        ALOGD("%s(), dumpbytes = %d", __FUNCTION__, ((bytesAfterBitConvertion*16)/12));
	
	    WritePcmDumpData(newbuffer, ((bytesAfterBitConvertion*16)/12));
	}
	else
	{		
	    WritePcmDumpData(pBufferAfterBitConvertion, bytesAfterBitConvertion);
	}
#endif

#ifdef DEBUG_LATENCY
    ALOGD("AudioALSAPlaybackHandlerHDMI::write (-) latency_in_us,%1.6lf,%1.6lf,%1.6lf", latencyTime[0], latencyTime[1], latencyTime[2]);
#endif

    return bytes;
}


} // end of namespace android
