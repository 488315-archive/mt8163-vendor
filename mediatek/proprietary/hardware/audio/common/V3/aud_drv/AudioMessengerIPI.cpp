#include "AudioMessengerIPI.h"

#include "AudioLock.h"


#define LOG_TAG "AudioMessengerIPI"


/*#define ENABLE_DUMP_IPI_MSG*/
#define MAX_IPI_MSG_QUEUE_SIZE (8)
#define IPI_MSG_DUMMY_PAYLOAD_ADDR (0xFFFFFFFF)


#define AUDIO_IPI_DEVICE_PATH "/dev/audio_ipi"
#define AUDIO_IPI_IOC_MAGIC 'i'

#define AUDIO_IPI_IOCTL_SEND_MSG_ONLY _IOW(AUDIO_IPI_IOC_MAGIC, 0, unsigned int)
#define AUDIO_IPI_IOCTL_SEND_PAYLOAD  _IOW(AUDIO_IPI_IOC_MAGIC, 1, unsigned int)
#define AUDIO_IPI_IOCTL_SEND_DRAM     _IOW(AUDIO_IPI_IOC_MAGIC, 2, unsigned int)

#define AUDIO_IPI_IOCTL_SPM_MDSRC_ON  _IOW(AUDIO_IPI_IOC_MAGIC, 99, unsigned int)

namespace android
{

AudioMessengerIPI *AudioMessengerIPI::mAudioMessengerIPI = NULL;
AudioMessengerIPI *AudioMessengerIPI::getInstance()
{
    AudioLock mGetInstanceLock;
    AudioAutoTimeoutLock _l(mGetInstanceLock);

    if (mAudioMessengerIPI == NULL)
    {
        mAudioMessengerIPI = new AudioMessengerIPI();
    }
    ASSERT(mAudioMessengerIPI != NULL);
    return mAudioMessengerIPI;
}


AudioMessengerIPI::AudioMessengerIPI() :
    mDeviceDriver(-1)
{
    ALOGV("%s()", __FUNCTION__);
    openDriver();
}

AudioMessengerIPI::~AudioMessengerIPI()
{
    ALOGV("%s()", __FUNCTION__);
    closeDriver();
}

status_t AudioMessengerIPI::openDriver()
{
    ALOGD("%s()", __FUNCTION__);
    AudioAutoTimeoutLock _l(mLock);

    mDeviceDriver = open(AUDIO_IPI_DEVICE_PATH, O_RDWR);
    if (mDeviceDriver < 0)
    {
        ALOGE("%s() fail to open %s, errno: %d", __FUNCTION__, AUDIO_IPI_DEVICE_PATH, errno);
        return UNKNOWN_ERROR;
    }

    return NO_ERROR;
}

status_t AudioMessengerIPI::closeDriver()
{
    ALOGD("%s()", __FUNCTION__);
    AudioAutoTimeoutLock _l(mLock);

    if (mDeviceDriver >= 0)
    {
        close(mDeviceDriver);
        mDeviceDriver = -1;
    }

    return NO_ERROR;
}


uint16_t AudioMessengerIPI::getMessageBufSize(const ipi_msg_t *ipi_msg)
{
    if (ipi_msg->data_type == AUDIO_IPI_MSG_ONLY)
    {
        return IPI_MSG_HEADER_SIZE;
    }
    else if (ipi_msg->data_type == AUDIO_IPI_PAYLOAD)
    {
        return (IPI_MSG_HEADER_SIZE + ipi_msg->param1);
    }
    else if (ipi_msg->data_type == AUDIO_IPI_DMA)
    {
        return (IPI_MSG_HEADER_SIZE + sizeof(uint32_t)); /* sizeof(char *) */
    }
    else
    {
        return 0;
    }
}


void AudioMessengerIPI::dumpMsg(const ipi_msg_t *ipi_msg)
{
#ifdef ENABLE_DUMP_IPI_MSG
    int i = 0;
    int payload_size = 0;

    ALOGD("%s(), sizeof(ipi_msg_t) = %d", __FUNCTION__, sizeof(ipi_msg_t));

    ALOGD("%s(), magic = 0x%x", __FUNCTION__, ipi_msg->magic);
    ALOGD("%s(), task_scene = 0x%x", __FUNCTION__, ipi_msg->task_scene);
    ALOGD("%s(), msg_layer = 0x%x", __FUNCTION__, ipi_msg->msg_layer);
    ALOGD("%s(), data_type = 0x%x", __FUNCTION__, ipi_msg->data_type);
    ALOGD("%s(), ack_type = 0x%x", __FUNCTION__, ipi_msg->ack_type);
    ALOGD("%s(), msg_id = 0x%x", __FUNCTION__, ipi_msg->msg_id);
    ALOGD("%s(), param1 = 0x%x", __FUNCTION__, ipi_msg->param1);
    ALOGD("%s(), param2 = 0x%x", __FUNCTION__, ipi_msg->param2);

    if (ipi_msg->data_type == AUDIO_IPI_PAYLOAD)
    {
        payload_size = ipi_msg->param1;
        for (i = 0; i < payload_size; i++)
        {
            ALOGD("%s(), payload[%d] = 0x%x", __FUNCTION__, i, ipi_msg->payload[i]);
        }
    }
    else if (ipi_msg->data_type == AUDIO_IPI_DMA)
    {
        ALOGD("%s(), dma_addr = %p\n", __FUNCTION__, ipi_msg->dma_addr);
    }
#endif
}


void AudioMessengerIPI::checkMsgFormat(const ipi_msg_t *ipi_msg, unsigned int len)
{
    dumpMsg(ipi_msg);

    ASSERT(ipi_msg->magic == IPI_MSG_MAGIC_NUMBER);
    ASSERT(getMessageBufSize(ipi_msg) == len);
}


void AudioMessengerIPI::setSpmApMdSrcReq(const bool enable)
{
    int retval = ioctl(mDeviceDriver, AUDIO_IPI_IOCTL_SPM_MDSRC_ON, enable);
    if (retval != 0)
    {
        ALOGE("%s() ioctl fail! retval = %d, errno: %d", __FUNCTION__, retval, errno);
    }
}


status_t AudioMessengerIPI::sendIpiMsg(
    task_scene_t task_scene,
    audio_ipi_msg_data_t data_type,
    audio_ipi_msg_ack_t ack_type,
    uint16_t msg_id,
    uint32_t param1,
    uint32_t param2,
    char    *payload)
{
    ALOGV("%s()", __FUNCTION__);
    AudioAutoTimeoutLock _l(mLock);

    if (mDeviceDriver < 0)
    {
        ALOGW("%s(), mDeviceDriver = %d, return", __FUNCTION__, mDeviceDriver);
        return UNKNOWN_ERROR;
    }


    ipi_msg_t ipi_msg;
    uint32_t ipi_msg_len = 0;

    memset(&ipi_msg, 0, MAX_IPI_MSG_BUF_SIZE);

    ipi_msg.magic      = IPI_MSG_MAGIC_NUMBER;
    ipi_msg.task_scene = task_scene;
    ipi_msg.msg_layer  = AUDIO_IPI_LAYER_KERNEL;
    ipi_msg.data_type  = data_type;
    ipi_msg.ack_type   = ack_type;
    ipi_msg.msg_id     = msg_id;
    ipi_msg.param1     = param1;
    ipi_msg.param2     = param2;

    if (ipi_msg.data_type == AUDIO_IPI_PAYLOAD)
    {
        ASSERT(payload != NULL);

        if (ipi_msg.param1 > MAX_IPI_MSG_PAYLOAD_SIZE)
        {
            ALOGE("%s(), payload size(%d) > MAX_IPI_MSG_PAYLOAD_SIZE(%d)",
                  __FUNCTION__, ipi_msg.param1, MAX_IPI_MSG_PAYLOAD_SIZE);
            ipi_msg.param1 = MAX_IPI_MSG_PAYLOAD_SIZE;
        }
        ipi_msg.param2 = IPI_MSG_DUMMY_PAYLOAD_ADDR;
        memcpy(ipi_msg.payload, payload, ipi_msg.param1);
    }
    else if (ipi_msg.data_type == AUDIO_IPI_DMA)
    {
        ASSERT(payload != NULL);
        ipi_msg.param2 = IPI_MSG_DUMMY_PAYLOAD_ADDR;
        ipi_msg.dma_addr = payload;
    }


    ipi_msg_len = getMessageBufSize(&ipi_msg);
    checkMsgFormat(&ipi_msg, ipi_msg_len);


    int retval = 0;
    if (ipi_msg.data_type == AUDIO_IPI_MSG_ONLY)
    {
        retval = ioctl(mDeviceDriver, AUDIO_IPI_IOCTL_SEND_MSG_ONLY, &ipi_msg);
    }
    else if (ipi_msg.data_type == AUDIO_IPI_PAYLOAD)
    {
        retval = ioctl(mDeviceDriver, AUDIO_IPI_IOCTL_SEND_PAYLOAD, &ipi_msg);
    }
    else if (ipi_msg.data_type == AUDIO_IPI_DMA)
    {
        retval = ioctl(mDeviceDriver, AUDIO_IPI_IOCTL_SEND_DRAM, &ipi_msg);
    }
    if (retval != 0)
    {
        ALOGE("%s() ioctl fail! retval = %d, errno: %d", __FUNCTION__, retval, errno);
    }

    return NO_ERROR;
}




} // end of namespace android
