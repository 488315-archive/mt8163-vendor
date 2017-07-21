#include "SpeechDriverOpenDSP.h"

#include <utils/threads.h>
#include <cutils/properties.h>

#include "AudioLock.h"

#include "SpeechDriverLAD.h"
#include "AudioMessengerIPI.h"

#include "audio_speech_msg_id.h"

#define LOG_TAG "SpeechDriverOpenDSP"

namespace android
{

/*==============================================================================
 *                     Singleton Pattern
 *============================================================================*/

SpeechDriverOpenDSP *SpeechDriverOpenDSP::mSpeechDriverMD1 = NULL;
SpeechDriverOpenDSP *SpeechDriverOpenDSP::mSpeechDriverMD2 = NULL;
SpeechDriverOpenDSP *SpeechDriverOpenDSP::mSpeechDriverMDExt = NULL;

SpeechDriverInterface *SpeechDriverOpenDSP::GetInstance(
    modem_index_t modem_index)
{
    static AudioLock mGetInstanceLock;
    AudioAutoTimeoutLock _l(mGetInstanceLock);

    SpeechDriverOpenDSP *pSpeechDriver = NULL;
    ALOGD("%s(), modem_index = %d", __FUNCTION__, modem_index);

    switch (modem_index)
    {
        case MODEM_1:
            if (mSpeechDriverMD1 == NULL)
            {
                mSpeechDriverMD1 = new SpeechDriverOpenDSP(modem_index);
            }
            pSpeechDriver = mSpeechDriverMD1;
            break;
        case MODEM_2:
            if (mSpeechDriverMD2 == NULL)
            {
                mSpeechDriverMD2 = new SpeechDriverOpenDSP(modem_index);
            }
            pSpeechDriver = mSpeechDriverMD2;
            break;
        case MODEM_EXTERNAL:
            if (mSpeechDriverMDExt == NULL)
            {
                mSpeechDriverMDExt = new SpeechDriverOpenDSP(modem_index);
            }
            pSpeechDriver = mSpeechDriverMDExt;
            break;
        default:
            ALOGE("%s: no such modem_index %d", __FUNCTION__, modem_index);
            break;
    }

    ASSERT(pSpeechDriver != NULL);
    return pSpeechDriver;
}


/*==============================================================================
 *                     Constructor / Destructor / Init / Deinit
 *============================================================================*/

SpeechDriverOpenDSP::SpeechDriverOpenDSP(modem_index_t modem_index) :
    pSpeechDriverInternal(NULL),
    pIPI(NULL)
{
    ALOGD("%s(), modem_index = %d", __FUNCTION__, modem_index);

    mModemIndex = modem_index;

    // control internal modem & FD216
    pSpeechDriverInternal = SpeechDriverLAD::GetInstance(modem_index);

    pIPI = AudioMessengerIPI::getInstance();
}

SpeechDriverOpenDSP::~SpeechDriverOpenDSP()
{
    ALOGD("%s()", __FUNCTION__);

    pSpeechDriverInternal = NULL;
}

/*==============================================================================
 *                     Speech Control
 *============================================================================*/
status_t SpeechDriverOpenDSP::SetSpeechMode(const audio_devices_t input_device,
                                            const audio_devices_t output_device)
{
    ALOGD("%s(+)", __FUNCTION__);

#if 0
    // TODO: [OpenDSP] set mode (devices & all related param info)
    FILE *fin = NULL;
    if (output_device == AUDIO_DEVICE_OUT_EARPIECE)
    {
        fopen("/data/handset_mode4.bin", "r");
    }
    else if (output_device == AUDIO_DEVICE_OUT_SPEAKER)
    {
        fopen("/data/handfree_mode2.bin", "r");
    }

    fseek(fin , 0 , SEEK_END);
    unsigned int len = ftell(fin);
    rewind(fin);


    fread(buf, 1, len, fin);
    ALOGD("HAHA, fin = %p, len = %u", fin, len);
    fclose(fin);

    pIPI->sendIpiMsg(TASK_SCENE_PHONE_CALL,
                     AUDIO_IPI_DMA, AUDIO_IPI_MSG_NEED_ACK,
                     IPI_MSG_A2D_SPH_PARAM, len, 0,
                     buf);
    usleep(5000); ////wait to all data is played
#endif

    pSpeechDriverInternal->SetSpeechMode(input_device, output_device);
    ALOGD("%s(-)", __FUNCTION__);

    return NO_ERROR;
}

status_t SpeechDriverOpenDSP::setMDVolumeIndex(int stream, int device,
                                               int index)
{
    return pSpeechDriverInternal->setMDVolumeIndex(stream, device, index);
}




status_t SpeechDriverOpenDSP::SpeechOn()
{
    ALOGD("%s()", __FUNCTION__);

    CheckApSideModemStatusAllOffOrDie();
    SetApSideModemStatus(SPEECH_STATUS_MASK);

    pIPI->setSpmApMdSrcReq(true);
    pIPI->sendIpiMsg(TASK_SCENE_PHONE_CALL,
                     AUDIO_IPI_MSG_ONLY, AUDIO_IPI_MSG_NEED_ACK,
                     IPI_MSG_A2D_SPH_ON, true, mModemIndex,
                     NULL);
    usleep(5000); //TODO wait to all data is played

    pSpeechDriverInternal->SpeechOn();

    return NO_ERROR;
}

status_t SpeechDriverOpenDSP::SpeechOff()
{
    ALOGD("%s()", __FUNCTION__);

    ResetApSideModemStatus(SPEECH_STATUS_MASK);
    CheckApSideModemStatusAllOffOrDie();

    // Clean gain value and mute status
    CleanGainValueAndMuteStatus();

    pSpeechDriverInternal->SpeechOff();

    usleep(10000); //TODO wait DSP off

    pIPI->sendIpiMsg(TASK_SCENE_PHONE_CALL,
                     AUDIO_IPI_MSG_ONLY, AUDIO_IPI_MSG_NEED_ACK,
                     IPI_MSG_A2D_SPH_ON, false, mModemIndex,
                     NULL);
    pIPI->setSpmApMdSrcReq(false);

    return NO_ERROR;
}

status_t SpeechDriverOpenDSP::VideoTelephonyOn()
{
    ALOGD("%s()", __FUNCTION__);
    CheckApSideModemStatusAllOffOrDie();
    SetApSideModemStatus(VT_STATUS_MASK);

    pIPI->setSpmApMdSrcReq(true);
    pIPI->sendIpiMsg(TASK_SCENE_PHONE_CALL,
                     AUDIO_IPI_MSG_ONLY, AUDIO_IPI_MSG_NEED_ACK,
                     IPI_MSG_A2D_SPH_ON, true, mModemIndex,
                     NULL);

    pSpeechDriverInternal->VideoTelephonyOn();
    return NO_ERROR;
}

status_t SpeechDriverOpenDSP::VideoTelephonyOff()
{
    ALOGD("%s()", __FUNCTION__);
    ResetApSideModemStatus(VT_STATUS_MASK);
    CheckApSideModemStatusAllOffOrDie();

    // Clean gain value and mute status
    CleanGainValueAndMuteStatus();

    pSpeechDriverInternal->VideoTelephonyOff();

    pIPI->sendIpiMsg(TASK_SCENE_PHONE_CALL,
                     AUDIO_IPI_MSG_ONLY, AUDIO_IPI_MSG_NEED_ACK,
                     IPI_MSG_A2D_SPH_ON, false, mModemIndex,
                     NULL);
    pIPI->setSpmApMdSrcReq(false);

    return NO_ERROR;
}

status_t SpeechDriverOpenDSP::SpeechRouterOn()
{
    return INVALID_OPERATION;
}

status_t SpeechDriverOpenDSP::SpeechRouterOff()
{
    return INVALID_OPERATION;
}


/*==============================================================================
 *                     Recording Control
 *============================================================================*/

status_t SpeechDriverOpenDSP::RecordOn()
{
    return pSpeechDriverInternal->RecordOn();
}

status_t SpeechDriverOpenDSP::RecordOn(record_type_t type_record)
{
    return pSpeechDriverInternal->RecordOn(type_record);
}

status_t SpeechDriverOpenDSP::RecordOff()
{
    return pSpeechDriverInternal->RecordOff();
}

status_t SpeechDriverOpenDSP::RecordOff(record_type_t type_record)
{
    return pSpeechDriverInternal->RecordOff(type_record);
}

status_t SpeechDriverOpenDSP::SetPcmRecordType(record_type_t type_record)
{
    return pSpeechDriverInternal->SetPcmRecordType(type_record);
}

status_t SpeechDriverOpenDSP::VoiceMemoRecordOn()
{
    ALOGD("%s()", __FUNCTION__);
    SetApSideModemStatus(VM_RECORD_STATUS_MASK);

    pIPI->sendIpiMsg(TASK_SCENE_PHONE_CALL,
                     AUDIO_IPI_MSG_ONLY, AUDIO_IPI_MSG_NEED_ACK,
                     IPI_MSG_A2D_PCM_DUMP_ON, true, 0,
                     NULL);

    return pSpeechDriverInternal->VoiceMemoRecordOn();
}

status_t SpeechDriverOpenDSP::VoiceMemoRecordOff()
{
    ALOGD("%s()", __FUNCTION__);
    ResetApSideModemStatus(VM_RECORD_STATUS_MASK);

    pIPI->sendIpiMsg(TASK_SCENE_PHONE_CALL,
                     AUDIO_IPI_MSG_ONLY, AUDIO_IPI_MSG_NEED_ACK,
                     IPI_MSG_A2D_PCM_DUMP_ON, false, 0,
                     NULL);

    return pSpeechDriverInternal->VoiceMemoRecordOff();
}

uint16_t SpeechDriverOpenDSP::GetRecordSampleRate() const
{
    return pSpeechDriverInternal->GetRecordSampleRate();
}

uint16_t SpeechDriverOpenDSP::GetRecordChannelNumber() const
{
    return pSpeechDriverInternal->GetRecordChannelNumber();
}


/*==============================================================================
 *                     Background Sound
 *============================================================================*/

status_t SpeechDriverOpenDSP::BGSoundOn()
{
    return pSpeechDriverInternal->BGSoundOn();
}

status_t SpeechDriverOpenDSP::BGSoundConfig(uint8_t ul_gain, uint8_t dl_gain)
{
    return pSpeechDriverInternal->BGSoundConfig(ul_gain, dl_gain);
}

status_t SpeechDriverOpenDSP::BGSoundOff()
{
    return pSpeechDriverInternal->BGSoundOff();
}

/*==============================================================================
*                     PCM 2 Way
*============================================================================*/
status_t SpeechDriverOpenDSP::PCM2WayPlayOn()
{
    return pSpeechDriverInternal->PCM2WayPlayOn();
}


status_t SpeechDriverOpenDSP::PCM2WayPlayOff()
{
    return pSpeechDriverInternal->PCM2WayPlayOff();
}


status_t SpeechDriverOpenDSP::PCM2WayRecordOn()
{
    return pSpeechDriverInternal->PCM2WayRecordOn();
}


status_t SpeechDriverOpenDSP::PCM2WayRecordOff()
{
    return pSpeechDriverInternal->PCM2WayRecordOff();
}


status_t SpeechDriverOpenDSP::PCM2WayOn(const bool wideband_on)
{
    return pSpeechDriverInternal->PCM2WayOn(wideband_on);
}


status_t SpeechDriverOpenDSP::PCM2WayOff()
{
    return pSpeechDriverInternal->PCM2WayOff();
}


status_t SpeechDriverOpenDSP::DualMicPCM2WayOn(const bool wideband_on,
                                               const bool record_only)
{
    return pSpeechDriverInternal->DualMicPCM2WayOn(wideband_on, record_only);
}


status_t SpeechDriverOpenDSP::DualMicPCM2WayOff()
{
    return pSpeechDriverInternal->DualMicPCM2WayOff();
}


/*==============================================================================
 *                     TTY-CTM Control
 *============================================================================*/
status_t SpeechDriverOpenDSP::TtyCtmOn(tty_mode_t ttyMode)
{
    pIPI->sendIpiMsg(TASK_SCENE_PHONE_CALL,
                     AUDIO_IPI_MSG_ONLY, AUDIO_IPI_MSG_NEED_ACK,
                     IPI_MSG_A2D_TTY_ON, true, ttyMode,
                     NULL);

    return pSpeechDriverInternal->TtyCtmOn(ttyMode);
}

status_t SpeechDriverOpenDSP::TtyCtmOff()
{
    pIPI->sendIpiMsg(TASK_SCENE_PHONE_CALL,
                     AUDIO_IPI_MSG_ONLY, AUDIO_IPI_MSG_NEED_ACK,
                     IPI_MSG_A2D_TTY_ON, false, AUD_TTY_OFF,
                     NULL);

    return pSpeechDriverInternal->TtyCtmOff();
}

status_t SpeechDriverOpenDSP::TtyCtmDebugOn(bool tty_debug_flag)
{
    return pSpeechDriverInternal->TtyCtmDebugOn(tty_debug_flag);
}

/*==============================================================================
 *                     Acoustic Loopback
 *============================================================================*/

status_t SpeechDriverOpenDSP::SetAcousticLoopback(bool loopback_on)
{
    ALOGD("%s(), loopback_on = %d", __FUNCTION__, loopback_on);

    if (loopback_on == true)
    {
        CheckApSideModemStatusAllOffOrDie();
        SetApSideModemStatus(LOOPBACK_STATUS_MASK);

        pIPI->setSpmApMdSrcReq(true);
        pIPI->sendIpiMsg(TASK_SCENE_PHONE_CALL,
                         AUDIO_IPI_MSG_ONLY, AUDIO_IPI_MSG_NEED_ACK,
                         IPI_MSG_A2D_SPH_ON, true, mModemIndex,
                         NULL);

        pSpeechDriverInternal->SetAcousticLoopback(loopback_on);
    }
    else
    {
        ResetApSideModemStatus(LOOPBACK_STATUS_MASK);
        CheckApSideModemStatusAllOffOrDie();

        // Clean gain value and mute status
        CleanGainValueAndMuteStatus();

        pSpeechDriverInternal->SetAcousticLoopback(loopback_on);

        pIPI->sendIpiMsg(TASK_SCENE_PHONE_CALL,
                         AUDIO_IPI_MSG_ONLY, AUDIO_IPI_MSG_NEED_ACK,
                         IPI_MSG_A2D_SPH_ON, false, mModemIndex,
                         NULL);
        pIPI->setSpmApMdSrcReq(false);
    }

    return NO_ERROR;
}

status_t SpeechDriverOpenDSP::SetAcousticLoopbackBtCodec(bool enable_codec)
{
    return pSpeechDriverInternal->SetAcousticLoopbackBtCodec(enable_codec);
}

status_t SpeechDriverOpenDSP::SetAcousticLoopbackDelayFrames(
    int32_t delay_frames)
{
    return pSpeechDriverInternal->SetAcousticLoopbackDelayFrames(delay_frames);
}

/*==============================================================================
 *                     Volume Control
 *============================================================================*/

status_t SpeechDriverOpenDSP::SetDownlinkGain(int16_t gain)
{
    ALOGD("%s(), gain = 0x%x, old mDownlinkGain = 0x%x", __FUNCTION__, gain, mDownlinkGain);
    if (gain == mDownlinkGain) { return NO_ERROR; }

    mDownlinkGain = gain;
    return pIPI->sendIpiMsg(TASK_SCENE_PHONE_CALL,
                            AUDIO_IPI_MSG_ONLY, AUDIO_IPI_MSG_BYPASS_ACK,
                            IPI_MSG_A2D_DL_GAIN, gain, 0/*TODO*/,
                            NULL);
}

status_t SpeechDriverOpenDSP::SetEnh1DownlinkGain(int16_t gain)
{
    return INVALID_OPERATION; // not support anymore
}

status_t SpeechDriverOpenDSP::SetUplinkGain(int16_t gain)
{
    ALOGD("%s(), gain = 0x%x, old mUplinkGain = 0x%x", __FUNCTION__, gain, mUplinkGain);
    if (gain == mUplinkGain) { return NO_ERROR; }

    mUplinkGain = gain;
    return pIPI->sendIpiMsg(TASK_SCENE_PHONE_CALL,
                            AUDIO_IPI_MSG_ONLY, AUDIO_IPI_MSG_BYPASS_ACK,
                            IPI_MSG_A2D_UL_GAIN, gain, 0/*TODO*/,
                            NULL);
}

status_t SpeechDriverOpenDSP::SetDownlinkMute(bool mute_on)
{
    ALOGD("%s(), mute_on = %d, old mDownlinkMuteOn = %d", __FUNCTION__, mute_on, mDownlinkMuteOn);
    if (mute_on == mDownlinkMuteOn) { return NO_ERROR; }

    mDownlinkMuteOn = mute_on;

    // mute voice dl + bgs
    pIPI->sendIpiMsg(TASK_SCENE_PHONE_CALL,
                     AUDIO_IPI_MSG_ONLY, AUDIO_IPI_MSG_NEED_ACK,
                     IPI_MSG_A2D_DL_MUTE_ON, mute_on, 0,
                     NULL);
    return pSpeechDriverInternal->SetDownlinkMute(mute_on); // for bgs
}

status_t SpeechDriverOpenDSP::SetUplinkMute(bool mute_on)
{
    ALOGD("%s(), mute_on = %d, old mUplinkMuteOn = %d", __FUNCTION__, mute_on, mUplinkMuteOn);
    if (mute_on == mUplinkMuteOn) { return NO_ERROR; }

    mUplinkMuteOn = mute_on;

    // mute voice ul + bgs
    pIPI->sendIpiMsg(TASK_SCENE_PHONE_CALL,
                     AUDIO_IPI_MSG_ONLY, AUDIO_IPI_MSG_NEED_ACK,
                     IPI_MSG_A2D_UL_MUTE_ON, mute_on, 0,
                     NULL);
    return pSpeechDriverInternal->SetUplinkMute(mute_on); // for bgs
}

status_t SpeechDriverOpenDSP::SetUplinkSourceMute(bool mute_on)
{
    ALOGD("%s(), mute_on = %d, old mUplinkSourceMuteOn = %d", __FUNCTION__, mute_on, mUplinkSourceMuteOn);
    if (mute_on == mUplinkSourceMuteOn) { return NO_ERROR; }

    mUplinkSourceMuteOn = mute_on;
    return pIPI->sendIpiMsg(TASK_SCENE_PHONE_CALL,
                            AUDIO_IPI_MSG_ONLY, AUDIO_IPI_MSG_NEED_ACK,
                            IPI_MSG_A2D_UL_MUTE_ON, mute_on, 0,
                            NULL);
}

status_t SpeechDriverOpenDSP::SetSidetoneGain(int16_t gain)
{
    ALOGD("%s(), gain = 0x%x, old mSideToneGain = 0x%x", __FUNCTION__, gain, mSideToneGain);
    if (gain == mSideToneGain) { return NO_ERROR; }

    mSideToneGain = gain;
    return pSpeechDriverInternal->SetSidetoneGain(gain);
}


/*==============================================================================
 *                     Device related Config
 *============================================================================*/

status_t SpeechDriverOpenDSP::SetModemSideSamplingRate(uint16_t sample_rate)
{
    return pSpeechDriverInternal->SetModemSideSamplingRate(sample_rate);
}


/*==============================================================================
 *                     Speech Enhancement Control
 *============================================================================*/
status_t SpeechDriverOpenDSP::SetSpeechEnhancement(bool enhance_on)
{
    pIPI->sendIpiMsg(TASK_SCENE_PHONE_CALL,
                     AUDIO_IPI_MSG_ONLY, AUDIO_IPI_MSG_NEED_ACK,
                     IPI_MSG_A2D_UL_ENHANCE_ON, enhance_on, 0,
                     NULL);
    pIPI->sendIpiMsg(TASK_SCENE_PHONE_CALL,
                     AUDIO_IPI_MSG_ONLY, AUDIO_IPI_MSG_NEED_ACK,
                     IPI_MSG_A2D_DL_ENHANCE_ON, enhance_on, 0,
                     NULL);
    return NO_ERROR;
}

status_t SpeechDriverOpenDSP::SetSpeechEnhancementMask(const
                                                       sph_enh_mask_struct_t &mask)
{
    return INVALID_OPERATION; // not support anymore
}

status_t SpeechDriverOpenDSP::SetBtHeadsetNrecOn(const bool bt_headset_nrec_on)
{
    return pIPI->sendIpiMsg(TASK_SCENE_PHONE_CALL,
                            AUDIO_IPI_MSG_ONLY, AUDIO_IPI_MSG_NEED_ACK,
                            IPI_MSG_A2D_BT_NREC_ON, bt_headset_nrec_on, 0,
                            NULL);
}


/*==============================================================================
 *                     Speech Enhancement Parameters
 *============================================================================*/

status_t SpeechDriverOpenDSP::SetNBSpeechParameters(const
                                                    AUDIO_CUSTOM_PARAM_STRUCT *pSphParamNB)
{
    return INVALID_OPERATION;
}

status_t SpeechDriverOpenDSP::SetDualMicSpeechParameters(
    const AUDIO_CUSTOM_EXTRA_PARAM_STRUCT *pSphParamDualMic)
{
    return INVALID_OPERATION;
}

status_t SpeechDriverOpenDSP::SetMagiConSpeechParameters(
    const AUDIO_CUSTOM_MAGI_CONFERENCE_STRUCT *pSphParamMagiCon)
{
    return INVALID_OPERATION;
}

status_t SpeechDriverOpenDSP::SetHACSpeechParameters(const
                                                     AUDIO_CUSTOM_HAC_PARAM_STRUCT *pSphParamHAC)
{
    return INVALID_OPERATION;
}

status_t SpeechDriverOpenDSP::SetWBSpeechParameters(const
                                                    AUDIO_CUSTOM_WB_PARAM_STRUCT *pSphParamWB)
{
    return INVALID_OPERATION;
}

status_t SpeechDriverOpenDSP::GetVibSpkParam(void *eVibSpkParam)
{
    return pSpeechDriverInternal->GetVibSpkParam(eVibSpkParam);
}

status_t SpeechDriverOpenDSP::SetVibSpkParam(void *eVibSpkParam)
{
    return pSpeechDriverInternal->SetVibSpkParam(eVibSpkParam);
}


/*==============================================================================
 *                     Recover State
 *============================================================================*/

void SpeechDriverOpenDSP::RecoverModemSideStatusToInitState()
{
    // TODO: [OpenDSP] recovery
}


/*==============================================================================
 *                     Check Modem Status
 *============================================================================*/
bool SpeechDriverOpenDSP::CheckModemIsReady()
{
    // TODO: [OpenDSP] scp ready
    return pSpeechDriverInternal->CheckModemIsReady();
}


/*==============================================================================
 *                     Debug Info
 *============================================================================*/
status_t SpeechDriverOpenDSP::ModemDumpSpeechParam()
{
    return INVALID_OPERATION;
}


/*==============================================================================
 *                     Speech Enhancement Parameters
 *============================================================================*/
status_t SpeechDriverOpenDSP::SetDynamicSpeechParameters(const int type,
                                                         const void *param_arg)
{
    return pSpeechDriverInternal->SetDynamicSpeechParameters(type, param_arg);
}


} // end of namespace android

