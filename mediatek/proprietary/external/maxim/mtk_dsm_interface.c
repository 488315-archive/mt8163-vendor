#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include <utils/Log.h>
#include <android/log.h>
#include <system/audio.h>
#include <mlds_api.h>
#include <dsm_api.h>
#include <dsm_api_types.h>

#define  LOG_TAG    "maxim_interface"

#define  ALOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  ALOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

// DSM relate information
#define MAXIM_SPEAKER_PROCESSING_SAMPLE (240) // lib define
#define MAXIM_SPEAKER_VOICE_PROCESSING_SAMPLE (80) // lib define
//#define DUMP_IV_DATA

#define DSM_NEW_API_INTERFACE

static int mDsmBuffer[2 * 4 + 1 ]; // for  stereo
static int delayedSamples[4*2];

static unsigned int mDsmMemSize = 0;
static int mDsmSamples = MAXIM_SPEAKER_PROCESSING_SAMPLE;
static void *dsm_handler = NULL;
static mlds_task_config_t gmlds_task_config_t;
static unsigned int mDsmWorkingChannel = 1;

// DL data information and processing size
static uint32_t mRemindBufferSize = 0;
static char *mRemindBuffer = NULL;
static int mDsmProcessingSize = 0;

// IV Buffer and Processing size
static int mDsmIVProcessingSize = 0;
static uint32_t mRemindBIVufferSize = 0;
static char *mRemindIVBuffer = NULL;
static char *mIbuffer = NULL;
static char *mVbuffer = NULL;
static FILE *mIDumpFile = NULL;
static FILE *mVDumpFile = NULL;


// internal working buffer

static char *mSpeakerChannelProcessingBuffer = NULL;
static const int mDsmBufferSize = 1024 * 16;

#ifdef DSM_NEW_API_INTERFACE
static dsm_api_memory_size_ext_t sMemSize;
#endif

static void AssignedDSMParameters(const mlds_task_config_t *p_mlds_task_config)
{
    mDsmWorkingChannel = p_mlds_task_config->stream_downlink.device_extra_info;
    mDsmIVProcessingSize = mDsmSamples * p_mlds_task_config->stream_downlink.num_channels_out * 2 ;
    mDsmProcessingSize = mDsmSamples * p_mlds_task_config->stream_downlink.num_channels_out;

    ALOGD("%s mDsmWorkingChannel = %d mDsmIVProcessingSize = %d mDsmProcessingSize= %d",
        __FUNCTION__,mDsmWorkingChannel,mDsmIVProcessingSize,mDsmProcessingSize);
    if (p_mlds_task_config->stream_downlink.bit_format_in == BIT_FORMAT_S32_LE ||
        p_mlds_task_config->stream_downlink.bit_format_in == BIT_FORMAT_S24_LE)
    {
        mDsmProcessingSize *= 4;
    }
    else
    {
        mDsmProcessingSize *= 2;
    }
    ALOGD("%s mDsmWorkingChannel = %d mDsmIVProcessingSize = %d mDsmProcessingSize= %d",
        __FUNCTION__,mDsmWorkingChannel,mDsmIVProcessingSize,mDsmProcessingSize);
}

static void AllocateBuffer()
{
    // allocate for 240 * stereo and 32bits
    mRemindBufferSize = 0;
    mRemindBIVufferSize = 0;
    if (mRemindBuffer == NULL)
    {
        mRemindBuffer = (char *)malloc(mDsmBufferSize);
        memset((void*)mRemindBuffer,0,mDsmBufferSize);
    }
    if (mRemindIVBuffer == NULL)
    {
        mRemindIVBuffer = (char *)malloc(mDsmBufferSize);
        memset((void*)mRemindIVBuffer,0,mDsmBufferSize);
    }
    if (mIbuffer == NULL)
    {
        mIbuffer = (char *)malloc(mDsmBufferSize);
        memset((void*)mIbuffer,0,mDsmBufferSize);
    }
    if (mVbuffer == NULL)
    {
        mVbuffer = (char *)malloc(mDsmBufferSize);
        memset((void*)mVbuffer,0,mDsmBufferSize);
    }
    if (mSpeakerChannelProcessingBuffer == NULL)
    {
        mSpeakerChannelProcessingBuffer = (char *)malloc(mDsmBufferSize);
        memset((void*)mSpeakerChannelProcessingBuffer,0,mDsmBufferSize);
    }

    #ifdef DUMP_IV_DATA
    mIDumpFile = fopen("/sdcard/mtklog/audio_dump/ivdump.pcm", "wb");
    ALOGD("open Idump");
    #endif
}

static void FreeBuffer()
{
    if (mRemindBuffer)
    {
        free(mRemindBuffer);
        mRemindBuffer = NULL;
    }
    if (mRemindIVBuffer == NULL)
    {
        free(mRemindIVBuffer);
        mRemindIVBuffer = NULL;
    }
    if (mIbuffer)
    {
        free(mIbuffer);
        mIbuffer = NULL;
    }
    if (mVbuffer)
    {
        free(mVbuffer);
        mVbuffer = NULL;
    }
    if (mSpeakerChannelProcessingBuffer)
    {
        free(mSpeakerChannelProcessingBuffer);
        mSpeakerChannelProcessingBuffer = NULL;
    }
    #ifdef DUMP_IV_DATA
    if(mIDumpFile != NULL)
    {
    	fclose(mIDumpFile);
    	mIDumpFile = NULL;
    }
    #endif
}

static void CopyIVbuffer(void *InputBuffert , short *Rbuffer , short *Lbuffer, unsigned int samplecount)
{
    short *pTemp = (short *)InputBuffert;
    //ALOGD("samplecount = %d",samplecount);
    while (samplecount > 0)
    {
        *Rbuffer++ = *pTemp++;
        *Lbuffer++ = *pTemp++;
        samplecount--;
    }
}

static void DoSpeakerProctionPacking(int *pbuffer, int ValidInputBuffersize)
{
    // first copy data to processing buffer with ValidInputBuffersize
    memcpy((void *)mSpeakerChannelProcessingBuffer, (void *)pbuffer, ValidInputBuffersize);
    int *ppostProcessingL = pbuffer;
    int *ppostProcessingR = (int *)((char *)pbuffer + (ValidInputBuffersize / 2));
    int *pTempprocessingBuffer = (int *)mSpeakerChannelProcessingBuffer;


    while (ValidInputBuffersize > 0)
    {
        *ppostProcessingL = *pTempprocessingBuffer;
        ppostProcessingL++;
        pTempprocessingBuffer++;
        *ppostProcessingR = *pTempprocessingBuffer;
        ppostProcessingR++;
        pTempprocessingBuffer++;
        ValidInputBuffersize -= 8;
    }
}

static void  DoSpeakerProctionUnPacking(int *pbuffer, int ValidInputBuffersize)
{
    memcpy((void *)mSpeakerChannelProcessingBuffer, (void *)pbuffer, ValidInputBuffersize);
    int *ppostProcessingL = (int *)mSpeakerChannelProcessingBuffer;
    int *ppostProcessingR = (int *)((char *)mSpeakerChannelProcessingBuffer + (ValidInputBuffersize / 2));
    int *pTempprocessingBuffer = pbuffer;

    /*
    ALOGD("%s ValidInputBuffersize =%d ppostProcessingR = % pppostProcessingL = %p Dsmtempbuffer = %p",
        __FUNCTION__,ValidInputBuffersize,ppostProcessingR,ppostProcessingL,pbuffer);*/

    while (ValidInputBuffersize > 0)
    {
        *pTempprocessingBuffer = *ppostProcessingL;
        pTempprocessingBuffer++;
        ppostProcessingL++;
        *pTempprocessingBuffer = *ppostProcessingR;
        pTempprocessingBuffer++;
        ppostProcessingR++;
        ValidInputBuffersize -= 8;
    }
}

static void  MonoToStereo(int *pbuffer, int ValidInputBuffersize)
{
    if (mDsmWorkingChannel == 2)
    {
        return;
    }

    int *pTempprocessingBuffer = pbuffer + 1;
    while (ValidInputBuffersize > 0)
    {
        *pTempprocessingBuffer = *pbuffer;
        pTempprocessingBuffer += 2;
        pbuffer += 2;
        ValidInputBuffersize -= 8;
    }
}

lib_status_t mlds_query_working_buf_size(
    const mlds_task_config_t *p_mlds_task_config,
    uint32_t *p_working_buf_size)
{
    int ret_code = LIB_OK;
    ALOGD("+%s mDsmMemSize = %u num_channels_out = %d ",__func__,
        mDsmMemSize,p_mlds_task_config->stream_downlink.num_channels_out);

    #ifdef DSM_NEW_API_INTERFACE
    sMemSize.iChannels = p_mlds_task_config->stream_downlink.device_extra_info;
    sMemSize.iSamplingRate = p_mlds_task_config->stream_downlink.sample_rate_in;
    sMemSize.ipCircBufferSizeBytes = mDsmBuffer;
    sMemSize.oMemSizeRequestedBytes = 0;

    ALOGD("%s iChannels = %d iSamplingRate = %d sMemSize.iChannels  = %d",
    	__FUNCTION__,p_mlds_task_config->stream_downlink.device_extra_info,p_mlds_task_config->stream_downlink.sample_rate_in,sMemSize.iChannels);

    ret_code = DSM_API_get_memory_size_ext(&sMemSize, (int)sizeof(dsm_api_memory_size_ext_t));

    if( DSM_API_OK != ret_code)
    {
        ALOGE("DSM_API_get_memory_size_ext ret_code = %d",ret_code);
    }
    /*
    ALOGD("%s iChannels = %d iSamplingRate = %d sMemSize.oMemSizeRequestedBytes  = %d",
    	__FUNCTION__,sMemSize.iChannels, sMemSize.iSamplingRate,sMemSize.oMemSizeRequestedBytes);
    	*/

    *p_working_buf_size = sMemSize.oMemSizeRequestedBytes;
    mDsmMemSize = sMemSize.oMemSizeRequestedBytes;

    #else
    ret_code = DSM_API_get_memory_size(p_mlds_task_config->stream_downlink.device_extra_info, mDsmBuffer, &mDsmMemSize);
    *p_working_buf_size = mDsmMemSize;
    #endif

    ALOGD("-%s mDsmMemSize = %u", __func__,mDsmMemSize);
    return ret_code;
}


/**
 ******************************************************************************
 *  @brief Create handler and initialize it
 *
 *  @param p_mlds_task_config the task configure
 *  @param param_buf_size the size of param_buf
 *  @param p_param_buf the enhancement parameters and lib related configure settings
 *  @param working_buf_size the size of working buffer
 *  @param p_working_buf the allocated buffer and the size is from mlds_create_handler()
 *  @param pp_handler the handler of speech enhancement
 *
 *  @return lib_status_t
 ******************************************************************************
 */
lib_status_t mlds_create_handler(
    const mlds_task_config_t *p_mlds_task_config,
    const uint32_t param_buf_size,
    void *p_param_buf,
    const uint32_t working_buf_size,
    void *p_working_buf,
    void **pp_handler)
{
    int ret_code = LIB_OK;
    dsm_handler = *pp_handler;

    ALOGD("mlds_create_handler mDsmSamples = %d rate = %d num_channels_out = %d",
          mDsmSamples, p_mlds_task_config->stream_downlink.sample_rate_in, p_mlds_task_config->stream_downlink.num_channels_out);

    memcpy((void *)&gmlds_task_config_t, (void *)p_mlds_task_config, sizeof(mlds_task_config_t));
    AssignedDSMParameters(p_mlds_task_config);
    AllocateBuffer();

    #ifdef DSM_NEW_API_INTERFACE
    dsm_api_init_ext_t sInitParam;
    sInitParam.iChannels                         = sMemSize.iChannels;
    sInitParam.ipDelayedSamples         = delayedSamples;
    sInitParam.iSampleBitWidth           = 32;
    sInitParam.iSamplingRate               = sMemSize.iSamplingRate;
   #endif

    if (p_mlds_task_config->stream_downlink.bit_format_in == BIT_FORMAT_S24_LE ||
        p_mlds_task_config->stream_downlink.bit_format_in == BIT_FORMAT_S32_LE)
    {
        #ifdef DSM_NEW_API_INTERFACE
        ret_code = DSM_API_Init_ext((void *)dsm_handler, &sInitParam, sizeof(dsm_api_init_ext_t));

        #else
        ret_code = DSM_API_Init(dsm_handler, p_mlds_task_config->stream_downlink.sample_rate_in
                                , mDsmWorkingChannel | 0x100 , mDsmBuffer, (int *)&mDsmSamples);
        #endif
        ALOGD("dsm_handler = %p DSM_API_Init mDsmWorkingChannel | 0x100 = %d sample_rate_in = %d",
            dsm_handler,mDsmWorkingChannel,p_mlds_task_config->stream_downlink.sample_rate_in);
    }
    else
    {
        #ifdef DSM_NEW_API_INTERFACE
        ret_code = DSM_API_Init_ext((void *)dsm_handler, &sInitParam, sizeof(dsm_api_init_ext_t));
        #else
        ret_code = DSM_API_Init(dsm_handler, p_mlds_task_config->stream_downlink.sample_rate_in
                               , mDsmWorkingChannel, mDsmBuffer, (int *)&mDsmSamples);
        #endif
        ALOGD("dsm_handler = %p DSM_API_Init mDsmWorkingChannel = %d sample_rate_in = %d",
            dsm_handler,mDsmWorkingChannel,p_mlds_task_config->stream_downlink.sample_rate_in);
    }

    if (DSM_API_OK != ret_code)
    {
        ALOGD("DSM_API_Init_ext ret_code = %d", __FUNCTION__,ret_code);
    }
    ALOGD("DSM_API_Init_ext oFB_FrameSizeSamples = %d oFF_FrameSizeSamples = %d",
    	sInitParam.oFB_FrameSizeSamples,sInitParam.oFF_FrameSizeSamples);

    return LIB_OK;
}


/**
 ******************************************************************************
 *  @brief Processing microphone/uplink data
 *
 *  @param p_ul_buf_in the microphone/uplink data to be Processed
 *                   ul_buf_in = ul_buf_mic1(1 frame) + ... + ul_buf_micN (1 frame)
 *  @param p_ul_buf_out the processed microphone/uplink data
 *  @param p_aec_buf_in the AEC reference data
 *  @param delay_ms the delay time(ms) of aec buf
 *  @param p_handler the handler of speech enhancement
 *  @param arg reserved field
 *
 *  @return lib_status_t
 ******************************************************************************
 */
lib_status_t mlds_process_ul_buf(
    void *p_ul_buf_in,
    void *p_ul_buf_out,
    void *p_aec_buf_in,
    const uint32_t delay_ms,
    void *p_handler,
    void *arg)
{
    return LIB_UNSUPPORTED;
}

/**
 ******************************************************************************
 *  @brief Processing playback/downlink data
 *
 *  @param p_dl_buf_in the playback/downlink data to be Processed
 *  @param p_dl_buf_out the processed playback/downlink data
 *  @param p_handler the handler of speech enhancement
 *  @param arg reserved field
 *
 *  @return lib_status_t
 ******************************************************************************
 */
lib_status_t mlds_process_dl_buf(
    void *p_dl_buf_in,
    uint32_t *InSize,
    void *p_dl_buf_out,
    uint32_t *OutSize,
    void *p_ref_buf,
    uint32_t *RefBufSize,
    void *p_handler,
    void *arg)
{

    /*
    memcpy((void*)p_dl_buf_out,(void*)p_dl_buf_in, *InSize); // memcpy test
    ALOGD("%s memcpy",__FUNCTION__);
    *OutSize =  *InSize;
    */

    char *pInputBuf = (char *)p_dl_buf_in;
    uint32_t InputBufferSize =  *InSize;
    char *pOutputBuf = (char *)p_dl_buf_out;
    uint32_t OutputBufferSize = 0;
    char *pRefBuf = (char *)p_ref_buf;
    unsigned int TempBufferIndex = 0;
    int retCode = 0;

    // local variable
    char *pLocalDlPointer = NULL;
    char *pLocalIvPointer = NULL;
    int IV_Input_Sample = mDsmSamples * mDsmWorkingChannel;
    int DSM_Input_Sample = mDsmSamples * mDsmWorkingChannel;
    int DSM_Output_Sample  = mDsmSamples * mDsmWorkingChannel;

    if ((*InSize  < mDsmProcessingSize) || (*OutSize < mDsmProcessingSize))
    {
        *InSize = 0;
        * OutSize = 0;
        *RefBufSize = 0;
        ALOGD("error %s InSize = %d OutSize = %d RefBufSize = %d",__func__,*InSize,*OutSize,*RefBufSize);
        return LIB_OK;
    }

    /*
        I/V data has left
    */
    if (mRemindBIVufferSize)  // collect one buffer process size
    {
        TempBufferIndex = mDsmIVProcessingSize - mRemindBIVufferSize;
        //ALOGD("%s TempBufferIndex = %d mDsmIVProcessingSize = %d mRemindBIVufferSize = %d",__FUNCTION__, TempBufferIndex, mDsmIVProcessingSize, mRemindBIVufferSize);
        memcpy((void *)(mRemindIVBuffer + mRemindBIVufferSize), (void *)pRefBuf, TempBufferIndex);

        //update for IV buffer pointer
        pLocalIvPointer = mRemindIVBuffer;
        pRefBuf += TempBufferIndex;
        *RefBufSize -= TempBufferIndex;
        mRemindBIVufferSize=0;
    }
    else
    {
        //update for IV buffer pointer
        pLocalIvPointer = pRefBuf;
        pRefBuf += mDsmIVProcessingSize;
        *RefBufSize -= mDsmIVProcessingSize;
    }

    // still left buffer from prevous processing
    if (mRemindBufferSize)
    {
        TempBufferIndex = mDsmProcessingSize - mRemindBufferSize;
        //ALOGD("%s TempBufferIndex = %d mDsmProcessingSize = %d mRemindBufferSize = %d",__FUNCTION__, TempBufferIndex, mDsmProcessingSize, mRemindBufferSize);
        memcpy((void *)(mRemindBuffer + mRemindBufferSize), (void *)pInputBuf, TempBufferIndex);

        //update for buffer pointer
        pLocalDlPointer = mRemindBuffer;
        pInputBuf += TempBufferIndex;
        *InSize -= TempBufferIndex;
        *OutSize -= mDsmProcessingSize;
        mRemindBufferSize =0;
    }
    else
    {
        pLocalDlPointer = pInputBuf;
        pInputBuf += mDsmProcessingSize;
        *InSize -= mDsmProcessingSize;
        *OutSize -= mDsmProcessingSize;
    }

    CopyIVbuffer((void *)pLocalIvPointer, (short *)mVbuffer, (short *)mIbuffer, mDsmSamples);

    #ifdef DUMP_IV_DATA
    if(mIDumpFile != NULL)
    {
    	fwrite((void *)mIbuffer, sizeof(short),IV_Input_Sample,mIDumpFile);
    }
    #endif

    retCode = DSM_API_FB_process((void *)dsm_handler, 0, (short *)mIbuffer, (short *)mVbuffer, &IV_Input_Sample);
    if (DSM_API_OK != retCode)
    {
        ALOGD("DSM_API_process_iv retCode = %d", retCode);
    }

    DoSpeakerProctionPacking((int *)pLocalDlPointer, mDsmProcessingSize);
    //ALOGD("%s DSM_Input_Sample = %d DSM_Output_Sample = %d",__FUNCTION__,DSM_Input_Sample,DSM_Output_Sample);
    retCode = DSM_API_FF_process((void *)dsm_handler, 0, (short *)pLocalDlPointer, &DSM_Input_Sample, (short *)pOutputBuf, &DSM_Output_Sample);
    //memcpy((void*)pOutputBuf,(void*)pLocalDlPointer,1920);

    if (DSM_API_OK != retCode)
    {
        ALOGD("DSM_API_FF_process retCode = %d", retCode);
    }
    DoSpeakerProctionUnPacking((int *)pOutputBuf, mDsmProcessingSize);
    MonoToStereo((int *)pOutputBuf, mDsmProcessingSize);

    pOutputBuf += mDsmProcessingSize;
    OutputBufferSize += mDsmProcessingSize;
    //ALOGD("*InSize = %d *OutSize = %d OutputBufferSize = %d RefBufSize = %d", *InSize, *OutSize, OutputBufferSize,*RefBufSize);

    // check if can processing
    while (*InSize >= mDsmProcessingSize && *OutSize  >= mDsmProcessingSize)
    {
        // vairiable assign
        pLocalDlPointer = pInputBuf;
        pLocalIvPointer = pRefBuf;
        IV_Input_Sample = mDsmSamples * mDsmWorkingChannel;
        DSM_Input_Sample = mDsmSamples * mDsmWorkingChannel;
        DSM_Output_Sample  = mDsmSamples * mDsmWorkingChannel;

        if (*RefBufSize >= mDsmIVProcessingSize)
        {
            CopyIVbuffer((void *)pLocalIvPointer, (short *)mVbuffer, (short *)mIbuffer, mDsmSamples);

            #ifdef DUMP_IV_DATA
            if(mIDumpFile != NULL)
            {
            	fwrite((void *)mIbuffer, sizeof(short), IV_Input_Sample,mIDumpFile);
            }
            #endif
            /*
            ALOGD("DSM_API_process_iv DSM_Input_Sample = %d DSM_Output_Sample = %d mDsmProcessingSize = %d",
            	DSM_Input_Sample,DSM_Output_Sample,mDsmProcessingSize);
            	*/
            retCode = DSM_API_FB_process((void *)dsm_handler, 0, (short *)mIbuffer, (short *)mVbuffer, &IV_Input_Sample);
            if (DSM_API_OK != retCode)
            {
                ALOGD("DSM_API_process_iv retCode = %d", retCode);
            }
            pRefBuf += mDsmIVProcessingSize;
            *RefBufSize -= mDsmIVProcessingSize;
        }

        DoSpeakerProctionPacking((int *)pLocalDlPointer, mDsmProcessingSize);
        /*
        ALOGD("DSM_API_FF_process DSM_Input_Sample = %d DSM_Output_Sample = %d mDsmProcessingSize = %d",
        		DSM_Input_Sample,DSM_Output_Sample,mDsmProcessingSize);
        		*/
        retCode = DSM_API_FF_process((void *)dsm_handler, 0, (short *)pLocalDlPointer, &DSM_Input_Sample, (short *)pOutputBuf, &DSM_Output_Sample);
        if (DSM_API_OK != retCode)
        {
            ALOGD("DSM_API_FF_process retCode = %d", retCode);
        }
        DoSpeakerProctionUnPacking((int *)pOutputBuf, mDsmProcessingSize);
        MonoToStereo((int *)pOutputBuf, mDsmProcessingSize);

        //update for buffer pointer
        pOutputBuf += mDsmProcessingSize;
        pInputBuf += mDsmProcessingSize;
        *InSize -= mDsmProcessingSize;
        *OutSize -= mDsmProcessingSize;
        OutputBufferSize += mDsmProcessingSize;
        //ALOGD("*InSize = %d *OutSize = %d OutputBufferSize = %d RefBufSize = %d", *InSize, *OutSize, OutputBufferSize,*RefBufSize);

    }

    if (*InSize > 0 )
    {
        memcpy((void *)(mRemindBuffer), (void *)pInputBuf, *InSize);
        mRemindBufferSize = *InSize;
        *InSize = 0;
        //ALOGD("copy left buffersize mRemindBufferSize = %d",mRemindBufferSize);
    }

    if (*RefBufSize > 0)
    {
        memcpy((void *)(mRemindIVBuffer), (void *)pRefBuf, *RefBufSize);
        mRemindBIVufferSize = *RefBufSize ;
        //ALOGD("mRemindBIVufferSize = %d",mRemindBIVufferSize);
        *RefBufSize  = 0;
    }

    *OutSize  = OutputBufferSize;

    return LIB_OK;
}

/**
 ******************************************************************************
 *  @brief deinitialize handler and destroy it (no need to free the working buffer)
 *
 *  @param p_handler the handler of speech enhancement
 *
 *  @return lib_status_t
 ******************************************************************************
 */
lib_status_t mlds_destroy_handler(void *p_handler)
{
    FreeBuffer();
    dsm_handler = NULL;
    return LIB_OK;
}


/**
 ******************************************************************************
 *  @brief Update task device info
 *
 *  @param p_mlds_task_config the task configure
 *  @param param_buf_size the size of param_buf
 *  @param p_param_buf the enhancement parameters and lib related configure settings
 *  @param p_handler the handler of speech enhancement
 *
 *  @return lib_status_t
 ******************************************************************************
 */
lib_status_t mlds_update_device(
    const mlds_task_config_t *p_mlds_task_config,
    const uint32_t param_buf_size,
    void *p_param_buf,
    void *p_handler)

{
    return LIB_UNSUPPORTED;
}

/**
 ******************************************************************************
 *  @brief Update speech enhancement parameters
 *
 *  @param p_mlds_task_config the task configure
 *  @param param_buf_size the size of param_buf
 *  @param p_param_buf the enhancement parameters and lib related configure settings
 *  @param p_handler the handler of speech enhancement
 *
 *  @return lib_status_t
 ******************************************************************************
 */
lib_status_t mlds_update_param(
    const mlds_task_config_t *p_mlds_task_config,
    const uint32_t param_buf_size,
    void *p_param_buf,
    void *p_handler);


/**
 ******************************************************************************
 *  @brief Query the buffer size to keep speech enhancement parameters(single mode)
 *         Implemented in HAL, but not OpenDSP.
 *
 *  @param p_mlds_task_config the task configure
 *  @param platform_name the platform name by "adb shell getprop ro.product.model"
 *  @param param_file_path the speech enhancement param file (fullset)
 *  @param enhancement_mode the speech enhancement mode by apk
 *  @param p_param_buf_size the pointer to a variable to keep the size of param buffer
 *
 *  @return lib_status_t
 ******************************************************************************
 */
lib_status_t mlds_query_param_buf_size(
    const mlds_task_config_t *p_mlds_task_config,
    const char *platform_name,
    const char *param_file_path,
    const int enhancement_mode,
    uint32_t *p_param_buf_size)
{
    return LIB_OK;
}


/**
 ******************************************************************************
 *  @brief Parsing param file to get param buffer(single mode)
 *         Implemented in HAL, but not OpenDSP.
 *
 *  @param p_mlds_task_config the task configure
 *  @param platform_name the platform name by "adb shell getprop ro.product.model"
 *  @param param_file_path the speech enhancement param file (fullset)
 *  @param enhancement_mode the speech enhancement mode by apk
 *  @param param_buf_size the size of param buffer
 *  @param p_param_buf the param buffer(single mode)
 *
 *  @return lib_status_t
 ******************************************************************************
 */
lib_status_t mlds_parsing_param_file(
    const mlds_task_config_t *p_mlds_task_config,
    const char *platform_name,
    const char *param_file_path,
    const int enhancement_mode,
    const uint32_t param_buf_size,
    void *p_param_buf)
{

    return LIB_OK;
}


/**
 ******************************************************************************
 *  @brief Set value at a specified address
 *
 *  @param addr the specified address
 *  @param value the value to be setted at the specified address
 *  @param p_handler the handler of speech enhancement
 *
 *  @return lib_status_t
 ******************************************************************************
 */
lib_status_t mlds_set_addr_value(
    uint32_t addr,
    uint32_t value,
    void *p_handler)
{
    return LIB_OK;
}


/**
 ******************************************************************************
 *  @brief Get value at a specified address
 *
 *  @param addr the specified address
 *  @param p_value the pointer to a variable to get the value at the specified address
 *  @param p_handler the handler of speech enhancement
 *
 *  @return lib_status_t
 ******************************************************************************
 */
lib_status_t mlds_get_addr_value(
    uint32_t addr,
    uint32_t *p_value,
    void *p_handler)
{
    return LIB_OK;
}

/**
 ******************************************************************************
 *  @brief set uplink digital gain
 *
 *  @param ul_analog_gain_ref_only uplink PGA gain. For reference only.
 *              The unit is 0.25 dB.
 *              value: 0x0000 => 0.0dB, 0xFFFC = -1.0dB, 0x0020 = +8.0dB
 *  @param ul_digital_gain uplink gain.
 *              The unit is 0.25 dB.
 *              So gain value [0, 120] => 0 to 30dB
 *  @param p_handler the handler of speech enhancement
 *
 *  @return lib_status_t
 ******************************************************************************
 */
lib_status_t mlds_set_ul_digital_gain(
    const int16_t ul_analog_gain_ref_only,
    const int16_t ul_digital_gain,
    void *p_handler)
{
    return LIB_UNSUPPORTED;
}

/**
 ******************************************************************************
 *  @brief set downlink digital gain
 *
 *  @param dl_analog_gain_ref_only downlink PGA gain. For reference only.
 *              The unit is 0.25 dB.
 *              value: 0x0000 => 0.0dB, 0xFFFC = -1.0dB, 0x0020 = +8.0dB
 *  @param dl_digital_gain downlink gain.
                The unit is 0.25 dB.
 *              So gain value [-256, 0] => -64 to 0 dB
 *  @param p_handler the handler of speech enhancement
 *
 *  @return lib_status_t
 ******************************************************************************
 */
lib_status_t mlds_set_dl_digital_gain(
    const int16_t dl_analog_gain_ref_only,
    const int16_t dl_digital_gain,
    void *p_handler)
{
    return LIB_OK;
}


/**
 ******************************************************************************
 *  @brief mute/unmute uplink
 *
 *  @param b_mute_on mute uplink or not.
 *             - true: mute
 *             - false: unmute(default state)
 *  @param p_handler the handler of speech enhancement
 *
 *  @return lib_status_t
 ******************************************************************************
 */
lib_status_t mlds_set_ul_mute(uint8_t b_mute_on, void *p_handler)
{
    return LIB_UNSUPPORTED;
}


/**
 ******************************************************************************
 *  @brief mute/unmute downlink
 *
 *  @param b_mute_on mute downlink or not.
 *             - true: mute
 *             - false: unmute(default state)
 *  @param p_handler the handler of speech enhancement
 *
 *  @return lib_status_t
 ******************************************************************************
 */
lib_status_t mlds_set_dl_mute(uint8_t b_mute_on, void *p_handler)
{
    return LIB_OK;
}


/**
 ******************************************************************************
 *  @brief enable/disable uplink enhancement function
 *
 *  @param b_enhance_on enable uplink speech enhancement or not.
 *             true: enable(default state)
 *             false: disable
 *  @param p_handler the handler of speech enhancement
 *
 *  @return lib_status_t
 ******************************************************************************
 */
lib_status_t mlds_set_ul_enhance(uint8_t b_enhance_on, void *p_handler)
{
    return LIB_UNSUPPORTED;
}


/**
 ******************************************************************************
 *  @brief enable/disable downlink enhancement function
 *
 *  @param b_enhance_on enable downlink speech enhancement or not.
 *             - true: enable(default state)
 *             - false: disable
 *  @param p_handler the handler of speech enhancement
 *
 *  @return lib_status_t
 ******************************************************************************
 */
lib_status_t mlds_set_dl_enhance(uint8_t b_enhance_on, void *p_handler)
{
    return LIB_OK;
}



