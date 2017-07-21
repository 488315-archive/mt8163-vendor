#include "AudioALSAPlaybackHandlerOffload.h"
#include "AudioALSAHardwareResourceManager.h"
#include "AudioVolumeFactory.h"
#include "AudioALSASampleRateController.h"

#include "AudioMTKFilter.h"
#include "AudioVUnlockDL.h"
#include "AudioALSADeviceParser.h"
#include "AudioALSADriverUtility.h"
#if defined(MTK_SPEAKER_MONITOR_SUPPORT)
#include "AudioALSASpeakerMonitor.h"
#endif
#include <tinycompress/tinycompress.h>

#include <pthread.h>
#include <linux/rtpm_prio.h>
#include <sys/prctl.h>


#define LOG_TAG "AudioALSAPlaybackHandlerOffload"

 



namespace android
{
static void *writeThreadOffload(void *arg);
static bool threadExit = true;
static   const char PROPERTY_KEY_EXTDAC[PROPERTY_KEY_MAX]  = "af.resouce.extdac_support";
static char const *const kOffloadDeviceName = "/dev/offloadservice";
static int mFd = -1;

struct offload_stream_property offload_stream;
struct offload_thread_property offload_thread;
struct offload_write_info offload_write;

static void *offload_threadloop(void *arg)
{
    // force to set priority
    int command;
    struct offload_cmd *cmd;
    bool callback, exit, drain;
    stream_callback_event_t event;
    struct listnode *item;
    struct sched_param sched_p;

    pthread_mutex_lock(&offload_thread.offload_mutex);
	AudioALSAPlaybackHandlerOffload *pOffloadHandler = (AudioALSAPlaybackHandlerOffload *)arg;
	ALOGD("%s()+", __FUNCTION__);
    sched_getparam(0, &sched_p);
    sched_p.sched_priority = RTPM_PRIO_AUDIO_PLAYBACK;
    if (0 != sched_setscheduler(0, SCHED_RR, &sched_p))
    {
        ALOGE("[%s] failed, errno: %d", __FUNCTION__, errno);
    }
    else
    {
        sched_p.sched_priority = RTPM_PRIO_AUDIO_CCCI_THREAD;
        sched_getparam(0, &sched_p);
        ALOGD("sched_setscheduler ok, priority: %d", sched_p.sched_priority);
    }
    ALOGD("+%s(), pid: %d, tid: %d", __FUNCTION__, getpid(), gettid());

    offload_stream.offload_state = OFFLOAD_STATE_IDLE;
    offload_stream.remain_write = 0;
    exit = false;
    drain = false;

    for(;;)
    {
        cmd     = NULL;
        command = -1;
        callback = false;
        if (list_empty(&offload_thread.offload_cmd_list))
        {
            ALOGV("%s(),list_empty, state:%x, remain:%x", __FUNCTION__, offload_stream.offload_state, offload_stream.remain_write);
            if (drain && offload_stream.offload_state == OFFLOAD_STATE_PLAYING)
            {
                command = OFFLOAD_CMD_DRAIN;
            }
            else if (offload_stream.remain_write && offload_stream.offload_state == OFFLOAD_STATE_PLAYING)
            {
                command = OFFLOAD_CMD_WRITE;
            }
        }
        else
        {
            ALOGV("%s(),list not empty", __FUNCTION__);
            item = list_head(&offload_thread.offload_cmd_list);
            cmd = node_to_item(item, struct offload_cmd, node);
            command = cmd->cmd;
            list_remove(item);
            free(cmd);
        }

        if (command == -1)
        {
            ALOGV("%s(),waitevent+", __FUNCTION__);
            pthread_cond_wait(&offload_thread.offload_cond, &offload_thread.offload_mutex);
            ALOGV("%s(),waitevent-", __FUNCTION__);
            continue;
        }
        pthread_mutex_unlock(&offload_thread.offload_mutex);
        ALOGD("%s()command:%x", __FUNCTION__, command);

        switch(command)
        {
            case OFFLOAD_CMD_WRITE:
                if (pOffloadHandler->process_write() == OFFLOAD_WRITE_REMAIN)
                {
                    offload_stream.remain_write = 1;
                }
                else
                {
                    offload_stream.remain_write = 0;
                    event = STREAM_CBK_EVENT_WRITE_READY;
                    callback = true;
                    ALOGD("%s() write callback notify", __FUNCTION__);
                }
                ::ioctl(mFd, OFFLOADSERVICE_WRITEBLOCK, 0);
                break;
            case OFFLOAD_CMD_DRAIN:
                if (offload_stream.offload_state == OFFLOAD_STATE_PLAYING && (list_empty(&offload_thread.offload_cmd_list)))
                {
                    ::ioctl(mFd, OFFLOADSERVICE_WRITEBLOCK, 1);
                }
                if (list_empty(&offload_thread.offload_cmd_list))
                {
                    event = STREAM_CBK_EVENT_DRAIN_READY;
                    callback = true;
                    ALOGD("%s() drain callback notify", __FUNCTION__);
                }
                break;
            case OFFLOAD_CMD_PAUSE:
                if (offload_stream.offload_state == OFFLOAD_STATE_PLAYING)
                {
                    offload_stream.offload_state = OFFLOAD_STATE_PAUSED;
                }
                break;
            case OFFLOAD_CMD_RESUME:
                if (offload_stream.offload_state == OFFLOAD_STATE_PAUSED)
                {
                    offload_stream.offload_state = OFFLOAD_STATE_PLAYING;
                }
                break;
            case OFFLOAD_CMD_FLUSH:
                if(offload_stream.offload_state == OFFLOAD_STATE_PLAYING || offload_stream.offload_state == OFFLOAD_STATE_PAUSED)
                {
                    offload_stream.offload_state = OFFLOAD_STATE_IDLE;
                    pOffloadHandler->offload_initialize();
                    offload_stream.remain_write = 0;
                }
                break;
            case OFFLOAD_CMD_CLOSE:
                exit = true;
                break;
            default:
                ALOGE("%s(),Invalid Command", __FUNCTION__);
                break;
        }

        if (callback)
        {
            pOffloadHandler->offload_callback(event);
        }

        pthread_mutex_lock(&offload_thread.offload_mutex);

        if(exit)
        {
            pOffloadHandler->offload_callback(STREAM_CBK_EVENT_WRITE_READY);
            pOffloadHandler->offload_callback(STREAM_CBK_EVENT_DRAIN_READY);
            break;
        }

    }
    pthread_mutex_unlock(&offload_thread.offload_mutex);
    ALOGD("%s()-", __FUNCTION__);
    ::close(mFd);
    threadExit = true;

	return NULL;
}

static int send_offload_cmd(int command)
{
    struct offload_cmd *cmd = (struct offload_cmd *)calloc(1, sizeof(struct offload_cmd));

    ALOGD("%s %d", __FUNCTION__, command);

    cmd->cmd = command;
    list_add_tail(&offload_thread.offload_cmd_list, &cmd->node);
    pthread_mutex_lock(&offload_thread.offload_mutex);
    pthread_cond_signal(&offload_thread.offload_cond);
    pthread_mutex_unlock(&offload_thread.offload_mutex);
    return 0;
}

AudioALSAPlaybackHandlerOffload::AudioALSAPlaybackHandlerOffload(const stream_attribute_t *stream_attribute_source) :
    AudioALSAPlaybackHandlerBase(stream_attribute_source),
    mWriteBsbufSize(0),
    mReady(false)
{
    ALOGD("%s()", __FUNCTION__);
    mPlaybackHandlerType = PLAYBACK_HANDLER_OFFLOAD;
    mFormat = stream_attribute_source->audio_format;
    memset(&mComprConfig, 0, sizeof(mComprConfig));
    //ASSERT(false);
    mMixer = AudioALSADriverUtility::getInstance()->getMixer();   //DOUG TO CHECK

}


AudioALSAPlaybackHandlerOffload::~AudioALSAPlaybackHandlerOffload()
{
    ALOGD("%s()", __FUNCTION__);
}

void AudioALSAPlaybackHandlerOffload::offload_initialize()
{
    mReady = false;
    // mDecPcmbufRemain = 0;
    mWriteBsbufSize  = 0;
}
static int offload_stop()
{
    ALOGD("[Liang]offload_stop!!");

    int retval = ::ioctl(mFd, OFFLOADSERVICE_ACTION, OFFLOAD_ACTION_STOP);
    return 0;
}

static int offload_close()
{
    ALOGD("[Liang]offload_close!!");
    int retval = ::ioctl(mFd, OFFLOADSERVICE_ACTION, OFFLOAD_ACTION_CLOSE);
    return 0;
}

static int offload_pause()
{

    int retval = ::ioctl(mFd, OFFLOADSERVICE_ACTION, OFFLOAD_ACTION_PAUSE);

    return 0;
}


static int offload_resume()
{

    int retval = ::ioctl(mFd, OFFLOADSERVICE_ACTION, OFFLOAD_ACTION_RESUME);

    return 0;
}

status_t AudioALSAPlaybackHandlerOffload::openOffloadDriver()
{
    //open offloadservice device
    int retval = 0;
    mFd = ::open(kOffloadDeviceName, O_RDWR);
    if (mFd < 0)
    {
        ALOGE("%s() fail to open %s", __FUNCTION__, kOffloadDeviceName);
    }

    retval = ::ioctl(mFd, OFFLOADSERVICE_ACTION , OFFLOAD_ACTION_OPEN);
    if (retval < 0)
    {
        ALOGD("%s open offload DRAM initail Failed ", __FUNCTION__);
    }
    else
    {
        ALOGD("%s open offload DRAM initail Success ", __FUNCTION__);
    }

    retval = ::ioctl(mFd, OFFLOADSERVICE_SETPARAM, &mComprConfig);
    ALOGD("%s open offload OFFLOADSERVICE_SETPARAM Success ", __FUNCTION__);
    return NO_ERROR;
}

status_t AudioALSAPlaybackHandlerOffload::closeOffloadDriver()
{
    //offload_stop();
    offload_close();
    return NO_ERROR;
}


status_t AudioALSAPlaybackHandlerOffload::setFilterMng(AudioMTKFilterManager *pFilterMng)
{
    ALOGD("+%s() mAudioFilterManagerHandler [0x%x]", __FUNCTION__, pFilterMng);
    mAudioFilterManagerHandler = pFilterMng;
    ALOGD("-%s()", __FUNCTION__);
    return NO_ERROR;
}

void AudioALSAPlaybackHandlerOffload::offload_callback(stream_callback_event_t event)
{
    if (mCbkCookie != NULL)
    {
        mStreamCbk(event, 0, mCbkCookie);
    }
}


uint32_t AudioALSAPlaybackHandlerOffload::GetLowJitterModeSampleRate()
{
    return 48000;
}


bool AudioALSAPlaybackHandlerOffload::SetLowJitterMode(bool bEnable,uint32_t SampleRate)
{
    ALOGD("%s() bEanble = %d SampleRate = %u", __FUNCTION__, bEnable,SampleRate);

    enum mixer_ctl_type type;
    struct mixer_ctl *ctl;
    int retval = 0;

    // check need open low jitter mode
    if(SampleRate <= GetLowJitterModeSampleRate() && (AudioALSADriverUtility::getInstance()->GetPropertyValue(PROPERTY_KEY_EXTDAC)) == false)
    {
        ALOGD("%s(), bEanble = %d", __FUNCTION__, bEnable);
        return false;
    }

    ctl = mixer_get_ctl_by_name(mMixer, "Audio_I2S0dl1_hd_Switch");

    if (ctl == NULL)
    {
        ALOGE("Audio_I2S0dl1_hd_Switch not support");
        return false;
    }

    if (bEnable == true)
    {
        retval = mixer_ctl_set_enum_by_string(ctl, "On");
        ASSERT(retval == 0);
    }
    else
    {
        retval = mixer_ctl_set_enum_by_string(ctl, "Off");
        ASSERT(retval == 0);
    }
    return true;
}


status_t AudioALSAPlaybackHandlerOffload::open()
{
    ALOGD("+%s(), mDevice = 0x%x", __FUNCTION__, mStreamAttributeSource->output_devices);
    ALOGD("%s(), mStreamAttributeSource: format = %d",__FUNCTION__, mStreamAttributeSource->audio_format);
    AudioAutoTimeoutLock _l(*AudioALSADriverUtility::getInstance()->getStreamSramDramLock());
    // debug pcm dump
    OpenPCMDump(LOG_TAG);
    // acquire pmic clk
    mHardwareResourceManager->EnableAudBufClk(true);
    //doug to check
    //HpImpeDanceDetect();  //doug to check
#if 0
    int pcmindex = AudioALSADeviceParser::getInstance()->GetPcmIndexByString(keypcmDl3Meida);
    int cardindex = AudioALSADeviceParser::getInstance()->GetCardIndexByString(keypcmDl3Meida);

    ALOGD("AudioALSAPlaybackHandlerOffload::open() pcmindex = %d", pcmindex);
    ListPcmDriver(cardindex, pcmindex);

    struct pcm_params *params;
    params = pcm_params_get(cardindex, pcmindex,  PCM_OUT);
    if (params == NULL)
    {
        ALOGD("Device does not exist.\n");
    }
    mStreamAttributeTarget.buffer_size = pcm_params_get_max(params, PCM_PARAM_BUFFER_BYTES);
    ALOGD("%s buffersizemax = %d", __FUNCTION__, mStreamAttributeTarget.buffer_size);
    pcm_params_free(params);
#endif
    mStreamAttributeTarget.buffer_size = 32768;

//#ifdef PLAYBACK_USE_24BITS_ONLY
        //mStreamAttributeTarget.audio_format = AUDIO_FORMAT_PCM_32_BIT;
        mStreamAttributeTarget.audio_format = AUDIO_FORMAT_PCM_8_24_BIT;
        //mStreamAttributeTarget.audio_format = AUDIO_FORMAT_PCM_16_BIT;
//#else
	//	mStreamAttributeTarget.audio_format = (mStreamAttributeSource->audio_format == AUDIO_FORMAT_PCM_32_BIT) ? AUDIO_FORMAT_PCM_8_24_BIT : AUDIO_FORMAT_PCM_16_BIT;
//#endif


    mStreamAttributeTarget.audio_channel_mask = AUDIO_CHANNEL_IN_STEREO;
    mStreamAttributeTarget.num_channels = android_audio_legacy::AudioSystem::popCount(mStreamAttributeTarget.audio_channel_mask);
    mStreamAttributeTarget.sample_rate = ChooseTargetSampleRate(AudioALSASampleRateController::getInstance()->getPrimaryStreamOutSampleRate());
    // HW pcm config
    memset(&mConfig, 0, sizeof(mConfig));
    mConfig.channels = mStreamAttributeTarget.num_channels;
    mConfig.rate = mStreamAttributeTarget.sample_rate;
    mConfig.period_count = 2;
    mConfig.period_size = (mStreamAttributeTarget.buffer_size / (mConfig.channels * mConfig.period_count)) / ((mStreamAttributeTarget.audio_format == AUDIO_FORMAT_PCM_16_BIT) ? 2 : 4);
    mConfig.format = transferAudioFormatToPcmFormat(mStreamAttributeTarget.audio_format);

    mConfig.start_threshold = 0;
    mConfig.stop_threshold = 0;
    mConfig.silence_threshold = 0;
    ALOGD("%s(), mConfig: channels = %d, rate = %d, period_size = %d, period_count = %d, format = %d",
          __FUNCTION__, mConfig.channels, mConfig.rate, mConfig.period_size, mConfig.period_count, mConfig.format);


    mComprConfig.buffer.fragments = 1024;
    mComprConfig.buffer.fragment_size = 8192;
    mComprConfig.codec.sample_rate = mStreamAttributeTarget.sample_rate;
    mComprConfig.codec.reserved[0] = mConfig.period_size;
    mComprConfig.codec.reserved[1] = mComprConfig.buffer.fragments * mComprConfig.buffer.fragment_size;
    if (mConfig.format == PCM_FORMAT_S16_LE)
    {
        mComprConfig.codec.format = SNDRV_PCM_FORMAT_S16_LE;
    }
    else
    {
        mComprConfig.codec.format = SNDRV_PCM_FORMAT_S32_LE;
    }
    mComprConfig.codec.id = SND_AUDIOCODEC_MP3;
    mComprConfig.codec.ch_in = 2;
    mComprConfig.codec.ch_out = 2;

    // disable lowjitter mode   //doug to check
    SetLowJitterMode(true, mStreamAttributeTarget.sample_rate);

    mHardwareResourceManager->startOutputDevice(mStreamAttributeSource->output_devices, mStreamAttributeTarget.sample_rate);

    //if( compress_set_gapless_metadata(mComprStream, &offload_stream.offload_mdata) != 0)

    openOffloadDriver(); //open offload driver

    offload_stream.fragment_size = 8192;

    offload_stream.tmpbsBuffer = (void *)malloc(offload_stream.fragment_size << 1);
    offload_write.tmpBuffer = (void *)malloc(offload_stream.fragment_size);
    offload_write.bytes = offload_stream.fragment_size;

    //mWritebytes = 0;
    offload_stream.num_channels = mStreamAttributeTarget.num_channels;
    offload_stream.sample_rate = mStreamAttributeTarget.sample_rate;
    offload_stream.remain_write = 0;
    ALOGD("%s open offload num_channels = %d, sample_rate = %d , remain_write = %d, offload_write.tmpBuffer = %p", __FUNCTION__, offload_stream.num_channels , offload_stream.sample_rate, offload_stream.remain_write, offload_write.tmpBuffer);

    list_init(&offload_thread.offload_cmd_list);

    int ret = pthread_mutex_init(&offload_thread.offload_mutex, NULL);
    if (ret != 0)
    {
        ALOGE("%s, Failed to initialize Mutex!", __FUNCTION__);
        ASSERT(false);
        return -ENOSYS;
    }
    ret = pthread_cond_init(&offload_thread.offload_cond, NULL);
    if (ret != 0)
    {
        ALOGE("%s, Failed to initialize Cond!", __FUNCTION__);
        ASSERT(false);
        return -ENOSYS;
    }

    threadExit = false;
    ret = pthread_create(&offload_thread.offload_pthread, NULL, &offload_threadloop, this);

    if (ret != 0)
    {
        ALOGE("%s() create thread OffloadWrite fail!!", __FUNCTION__);
        ASSERT(false);
        return -ENOSYS;
    }
    usleep(1 * 1000);
    ALOGD("-%s()", __FUNCTION__);
    return NO_ERROR;
}

status_t AudioALSAPlaybackHandlerOffload::close()
{
    ALOGD("+%s()", __FUNCTION__);
    AudioAutoTimeoutLock _l(*AudioALSADriverUtility::getInstance()->getStreamSramDramLock());

    if (offload_stream.offload_state == OFFLOAD_STATE_PLAYING
        || offload_stream.offload_state == OFFLOAD_STATE_PAUSED)
    {
        offload_stop();
    }

    //close compress device driver
    send_offload_cmd(OFFLOAD_CMD_CLOSE);

    ALOGD("+%s() start wait", __FUNCTION__);
    while(!threadExit)
    {
        usleep(1 * 1000);
    }
    pthread_join(offload_thread.offload_pthread, (void **) NULL);
    pthread_cond_destroy(&offload_thread.offload_cond);
    pthread_mutex_destroy(&offload_thread.offload_mutex);
    // closeComprDriver();
    closeOffloadDriver();
    ALOGD("+%s() close offload driver", __FUNCTION__);

    //============Voice UI&Unlock REFERECE=============
    AudioVUnlockDL *VUnlockhdl = AudioVUnlockDL::getInstance();
    if (VUnlockhdl != NULL)
    {
        VUnlockhdl->SetInputStandBy(true);
    }
    //===========================================


    // close codec driver
    mHardwareResourceManager->stopOutputDevice();

    // close pcm driver
    //closePcmDriver();

    // disable lowjitter mode  //doug to check
    SetLowJitterMode(false, mStreamAttributeTarget.sample_rate);


    // debug pcm dump
    ClosePCMDump();

    //release pmic clk
    mHardwareResourceManager->EnableAudBufClk(false);
    //SetMHLChipEnable(false);   //doug to check

    memset(&mComprConfig,0, sizeof(mComprConfig));

    ALOGD("-%s()", __FUNCTION__);
    return NO_ERROR;
}

status_t AudioALSAPlaybackHandlerOffload::routing(const audio_devices_t output_devices)
{
    mHardwareResourceManager->changeOutputDevice(output_devices);
    if (mAudioFilterManagerHandler) { mAudioFilterManagerHandler->setDevice(output_devices); }
    return NO_ERROR;
}

status_t AudioALSAPlaybackHandlerOffload::pause()
{
    int ret = -1;
    ALOGD("%s() state:%x", __FUNCTION__, offload_stream.offload_state);
    if (offload_pause() != 0)
    {
        ALOGE("%s() error ", __FUNCTION__);
    }
    send_offload_cmd(OFFLOAD_CMD_PAUSE);
    return NO_ERROR;
}

status_t AudioALSAPlaybackHandlerOffload::resume()
{
    int ret = -1;
    ALOGD("%s() state:%x", __FUNCTION__, offload_stream.offload_state);
    if (offload_resume() != 0)
    {
        ALOGE("%s() error", __FUNCTION__);
    }

    send_offload_cmd(OFFLOAD_CMD_RESUME);
    return NO_ERROR;
}

status_t AudioALSAPlaybackHandlerOffload::setVolume(uint32_t vl)
{
    ALOGD("%s() VOL:0x%x ", __FUNCTION__, vl);
    offload_stream.offload_gain[0] = vl;
    offload_stream.offload_gain[1] = 1;
    ::ioctl(mFd, OFFLOADSERVICE_SETGAIN, offload_stream.offload_gain[0]);
    return NO_ERROR;
}

int AudioALSAPlaybackHandlerOffload::drain(audio_drain_type_t type)
{
    ALOGD("%s()", __FUNCTION__);
    int ret = 0;
    send_offload_cmd(OFFLOAD_CMD_DRAIN);

    usleep(1000);

    ::ioctl(mFd, OFFLOADSERVICE_SETDRAIN, type);

    ALOGD("-%s()", __FUNCTION__);

    return 0;
}

status_t AudioALSAPlaybackHandlerOffload::flush()
{
    int ret = -1;
    ALOGD("%s() state:%x", __FUNCTION__, offload_stream.offload_state);
    if (offload_stop() != 0)
    {
        ALOGE("%s() error", __FUNCTION__);
    }

    send_offload_cmd(OFFLOAD_CMD_FLUSH);
    return NO_ERROR;
}


ssize_t AudioALSAPlaybackHandlerOffload::write(const void *buffer, size_t bytes)
{
    // const -> to non const
    void *pBuffer = const_cast<void *>(buffer);
    memcpy(offload_stream.tmpbsBuffer + mWriteBsbufSize, buffer, bytes);
    mWriteBsbufSize += bytes;
    ALOGD("+%s(), offload_write buffer = %p, bytes = %d, mWriteBsbufSize =%d", __FUNCTION__, offload_write.tmpBuffer, bytes, mWriteBsbufSize);
    send_offload_cmd(OFFLOAD_CMD_WRITE);
   return bytes;
}


status_t AudioALSAPlaybackHandlerOffload::get_timeStamp(unsigned long *frames, unsigned int *samplerate)
{
    struct tstamp ktstamp;
    ktstamp.frames = 0;
    ktstamp.samplerate = 0;
    ::ioctl(mFd, OFFLOADSERVICE_GETTIMESTAMP, &ktstamp);
    *frames = ktstamp.frames;
    *samplerate = ktstamp.samplerate;

    ALOGD("%s(), frames  = %lu, samplerate = %u", __FUNCTION__, *frames, *samplerate);
    return NO_ERROR;
}




uint32_t AudioALSAPlaybackHandlerOffload::ChooseTargetSampleRate(uint32_t SampleRate)
{
    uint32_t TargetSampleRate = 44100;
    if(SampleRate <=  192000 && SampleRate > 96000)
    {
        TargetSampleRate = 192000;
    }
    else if(SampleRate <=96000 && SampleRate > 48000)
    {
        TargetSampleRate = 96000;
    }
    else
    {
        TargetSampleRate = SampleRate;
    }
    return TargetSampleRate;
}


AudioMTKDecHandlerBase *AudioALSAPlaybackHandlerOffload::AudioDecHandlerCreate()
{
    AudioMTKDecHandlerBase *pDecHandler = NULL;
    switch(mFormat)
    {
        case AUDIO_FORMAT_MP3:
            pDecHandler = new AudioMTKDecHandlerMP3();
            break;
        default:
            break;
    }

    if (NULL == pDecHandler)
    {
        ALOGD("-%s(), CreateFail", __FUNCTION__);
        return NULL;
    }

    return pDecHandler;
}


int AudioALSAPlaybackHandlerOffload::process_write()
{
    int ret = 0;
    int retval  = 1;
    // check  setwriteblock flag
    ret = ::ioctl(mFd, OFFLOADSERVICE_GETWRITEBLOCK, 0);
    if (!ret)
    {
        if (mWriteBsbufSize >= offload_stream.fragment_size)
        {
            int mRemainedBS = mWriteBsbufSize - offload_stream.fragment_size;
            memcpy(offload_write.tmpBuffer, offload_stream.tmpbsBuffer, offload_stream.fragment_size);
            memmove(offload_stream.tmpbsBuffer, offload_stream.tmpbsBuffer + offload_stream.fragment_size, mRemainedBS);
            mWriteBsbufSize = mRemainedBS;
            ALOGD("%s(), mRemainedBS = %d", __FUNCTION__, mRemainedBS);
            retval = ::ioctl(mFd, OFFLOADSERVICE_WRITE, &offload_write);
        }

        if (retval < 0)
        {
            ALOGE("%s(), write() error, retval = %d", __FUNCTION__, retval);
            ret = retval;
        }
        else
        {
            if (!mReady)
            {
                mReady = true;
                if (offload_stream.offload_state == OFFLOAD_STATE_IDLE)
                {
                    offload_stream.offload_state = OFFLOAD_STATE_PLAYING;
                    ::ioctl(mFd, OFFLOADSERVICE_ACTION, OFFLOAD_ACTION_START);
                    ALOGD("%s(), OFFLOADSERVICE_START", __FUNCTION__);
                }
            }

            return OFFLOAD_WRITE_EMPTY;
        }
    }
    else
    {
        return OFFLOAD_WRITE_REMAIN;
    }

    return ret;
}




} // end of namespace android


