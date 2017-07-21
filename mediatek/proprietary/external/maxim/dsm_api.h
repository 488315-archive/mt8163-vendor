/*=============================================================================
 #
 # The content of this file or document is CONFIDENTIAL and PROPRIETARY
 # to Maxim Integrated Products.  It is subject to the terms of a
 # License Agreement between Licensee and Maxim Integrated Products.
 # restricting among other things, the use, reproduction, distribution
 # and transfer.  Each of the embodiments, including this information and
 # any derivative work shall retain this copyright notice.
 #
 #============================================================================*/
/*
 *       DSM module interfaces
 *
 *   In this file are defined all messages, constants and
 * functions which are used to interface to DSM module.
 *
 * ********************************************************/
#ifndef _DSM_API_H_
#define _DSM_API_H_

#include "dsm_api_types.h"


#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
 *
 *       DSM module interfaces
 *
 *     In this file defines all messages, constants and
 * functions which are used to interface with DSM module.
 *
 *     The command consists of 32 bits whose format is
 * described as below:
 *
 *     +----------------------------+------------------+
 *     |  bit31  bit30  ...  bit24  | bit23  ...   bit0|
 *     +----------------------------+------------------+
 *     |    speaker channel mask    |    commandId     |
 *     +----------------------------+------------------+
 *         Table_1:  format of DSM API command
 *
 *     +-----------+-----------+-----------+-----------+
 *     |   bit31   |   bit30   |   ......  |   bit24   |
 *     +-----------+-----------+-----------+-----------+
 *     | channel_7 | channel_6 |           | channel_0 |
 *     +-----------+-----------+---- ------+-----------+
 *        Table_2:  format of channel mask
 *
 *    The bits between bit31 and bit24 are called channel mask bits.
 * The bit with the value of one indicates that the command is applied
 * for the corresponding channel.
 *
 *    For instance, in a stereo speaker system, 0x1 represents left
 * channel, 0x3 represents left and right channels.
 *    A special case: when the channel mask equals to zero, that means
 * the command will apply for all channels.
 *
 *    The lower 24 bits contain the command Id.
 *
 * ********************************************************/

/*********************************************************************
 *       macro DSM_CMD
 *
 * Description:
 *     The macro DSM_CMD is used to merge channel Id with command Id.
 * Direct use of command Id implies that the command will be sent to
 * the left channel.
 *
 *********************************************************************/

#define SAMPLING_RATE_SUPPORTED			((unsigned int)48000)
#define DSM_MAX_STRING_PARAM_SIZE		32	// in bytes
#define	DSM_CMD(chanId, cmdId)			((chanId)|(cmdId))

#if (DSM_CHANNELS==2 )
#define DSM_SET_PARAM(cmdId)            DSM_SET_STEREO_PARAM(cmdId)
#else
#define DSM_SET_PARAM(cmdId)            DSM_SET_MONO_PARAM(cmdId)
#endif

#ifndef TO_FIX
#define TO_FIX(a, q)     		        ((int)((a) * ((unsigned int)1<<(q))))
#endif

#ifndef TO_DBL
#define TO_DBL(a, q)                    (double)((double)((double)(a)/(double)((unsigned int)1<<(q))))
#endif


#define DSM_TIME_MAX_SIZE               16 //in bytes
#define DSM_DATE_MAX_SIZE               32 //in bytes
#define DSM_VERSION_MAX_SIZE            64
#define DSM_CHIPSET_MAX_SIZE            16 //in bytes

typedef struct DSM_API_command_map{
    char    *paramName;
    int     paramID;
    int     paramQvalue;
} DSM_API_command_map_t;

/*******************************************************
 *
 *      Data type DSM_API_SPKER
 *
 * Description:
 *     This data type DSM_API_SPKER defines the speaker
 * channel Id.
 *
 * *****************************************************/

enum DSM_API_SMART_PA_MODE
{
    DSM_API_SMART_PA_DISABLE_MODE       = 0,
    DSM_API_SMART_PA_ENABLE_MODE        = 1,
    DSM_API_SMART_PA_BYPASS_ADD_MODE    = 2
};

enum DSM_API_DSM_MODULE_ID
{
    DSM_API_FEED_FORWARD_MODULE         = 1<<0,
    DSM_API_FEED_BACKWORD_MODULE        = 1<<1,
};

typedef enum {
    DSM_API_MONO_SPKER                  = 0x00000000,//the mono speaker
    DSM_API_STEREO_SPKER                = 0x03000000,//the stereo speakers

    DSM_API_L_CHAN                      = 0x01000000,//the left channel speaker Id
    DSM_API_R_CHAN                      = 0x02000000,//the left channel speaker Id

    DSM_API_CHANNEL_1                   = 0x01000000,
    DSM_API_CHANNEL_2                   = 0x02000000,
    DSM_API_CHANNEL_3                   = 0x04000000,
    DSM_API_CHANNEL_4                   = 0x08000000,
    DSM_API_CHANNEL_5                   = 0x10000000,
    DSM_API_CHANNEL_6                   = 0x20000000,
    DSM_API_CHANNEL_7                   = 0x40000000,
//    DSM_API_CHANNEL_8                   = 0x80000000,

    DSM_MAX_SUPPORTED_CHANNELS          = 8
} DSM_API_CHANNEL_ID;

typedef enum{
    DSM_API_FF_PATH                     = 0,
    DSM_API_FB_PATH                     = 1
} DSM_API_PATH_ID;


typedef enum{
    DSM_API_INPUT_CIRC_BUFFER_ID        = 0,
    DSM_API_OUTPUT_CIRC_BUFFER_ID,
    DSM_API_I_CIRC_BUFFER_ID,
    DSM_API_V_CIRC_BUFFER_ID,
} DSM_API_CIRC_BUFFER_ID;


#define DSM_SET_MONO_PARAM(cmdId)       ((cmdId&0x00FFFFFF)|DSM_API_MONO_SPKER)
#define DSM_SET_STEREO_PARAM(cmdId)     ((cmdId&0x00FFFFFF)|DSM_API_STEREO_SPKER)
#define DSM_SET_LEFT_PARAM(cmdId)       ((cmdId&0x00FFFFFF)|DSM_API_L_CHAN)
#define DSM_SET_RIGHT_PARAM(cmdId)      ((cmdId&0x00FFFFFF)|DSM_API_R_CHAN)

typedef enum {
    LANDSCAPE_1_RCV_RIGHT = 0,
    LANDSCAPE_2_RCV_LEFT,
    PORTRAIT_ORIENT
} stereo_spk_orient_t;

/*******************************************************
 *
 *      Data type DSM_API_EQ_BAND
 *
 * Description:
 *     This data type DSM_API_EQ_BAND defines the Equalizer
 * band Id.
 *
 * *****************************************************/

typedef enum {
    DSM_API_EQ_BAND_1                   = 1<<0,
    DSM_API_EQ_BAND_2                   = 1<<1,
    DSM_API_EQ_BAND_3                   = 1<<2,
    DSM_API_EQ_BAND_4                   = 1<<3
} DSM_API_EQ_BAND;

/**************************************************
 *      Data type DSM_API_CMD
 *
 * Description:
 *     The data type of DSM_API_CMD is defined
 * as a 16-bit integer. The highest 4-bits contain
 * the channel Id and lower 12-bits the command Id.
 *
 * ************************************************/
typedef enum
{
    //The following messages are read/write
    DSM_API_GET_MAXIMUM_CMD_ID                  = 0,    //32-bits data, Q0
    DSM_API_SETGET_ENABLE                       = 1,    //32-bits data, Q0
    DSM_API_SETGET_COILTEMP_THRESHOLD           = 2,    //32-bits data, Q19, in C degrees
    DSM_API_SETGET_XCL_THRESHOLD                = 3,    //32-bits data, Q27, in millimeters
    DSM_API_SETGET_LIMITERS_RELTIME             = 4,    //32-bits data, Q30, in seconds
    DSM_API_SETGET_MAKEUP_GAIN                  = 5,    //32-bits data, Q29, in %
    DSM_API_SETGET_RDC_AT_ROOMTEMP              = 6,    //32-bits data, Q27
    DSM_API_SETGET_COPPER_CONSTANT              = 7,    //32-bits data, Q30
    DSM_API_SETGET_COLDTEMP                     = 8,    //32-bits data, Q19
    DSM_API_SETGET_PITONE_GAIN                  = 9,    //32-bits data, Q31, in %
    DSM_API_SETGET_LEAD_RESISTANCE              = 10,   //32-bits data, Q27
    DSM_API_SETGET_HPCUTOFF_FREQ                = 11,   //32-bits data, Q9, in Hz
    DSM_API_SETGET_LFX_GAIN                     = 12,   //32-bits data, Q30

    //for impedence model updating
    DSM_API_SETGET_REF_FC                       = 13,   //32-bits data, Q9, in Hz
    DSM_API_SETGET_REF_Q                        = 14,   //32-bits data, Q29
    DSM_API_INIT_F_Q_FILTERS                    = 15,   //32-bits data, Q0

    //The following messages are read only
    DSM_API_GET_ADAPTIVE_FC                     = 16,   //32-bits data, Q9, in Hz
    DSM_API_GET_ADAPTIVE_Q                      = 17,   //32-bits data, Q29
    DSM_API_GET_ADAPTIVE_DC_RES                 = 18,   //32-bits data, Q27
    DSM_API_GET_ADAPTIVE_COILTEMP               = 19,   //32-bits data, Q19
    DSM_API_GET_EXCURSION                       = 20,   //32-bits data, Q27

    //The fllowing 4 commands are used the buffers used for the following commands must be allocated
    //by the caller function. The maxim buffer size is 4K bytes.
    DSM_API_GET_PCM_INPUT_DATA                  = 21,   //the buffer will be filled with 16-bit input PCM data to DSM module.
    DSM_API_GET_IV_DATA                         = 22,   //the buffer will be filled with I and V data.
    DSM_API_GET_PCM_AND_IV_DATA                 = 23,   //the buffer will be filled with PCM, I and V data. All data are in 16-bit integer.
    DSM_API_GET_PCM_OUTPUT_DATA                 = 24,   //the buffer will be filled with 16-bit output PCM data of DSM module.

    DSM_API_SETGET_VLIMIT_RELEASE_TIME          = 25,   //This command is obsolete.
    DSM_API_SETGET_VLIMIT                       = 26,   //32-bits data, Q27, in percentage
    DSM_API_SETGET_ROOM_TEMP                    = 27,   //This command is obsolete.

    DSM_API_SETGET_PILOT_ENABLE                 = 28,   //32-bits data, Q0
    DSM_API_SETGET_CLIP_ENABLE                  = 29,   //32-bits data, Q0
    DSM_API_SETGET_EXC_ENABLE                   = 30,   //32-bits data, Q0
    DSM_API_SETGET_THERMAL_ENABLE               = 31,   //32-bits data, Q0

    DSM_API_SETGET_EQ_BAND_FC                   = 32,   //32-bits data, Q9, in Hz
    DSM_API_SETGET_EQ_BAND_Q                    = 33,   //32-bits data, Q29
    DSM_API_SETGET_EQ_BAND_ATTENUATION_DB       = 34,   //32-bits data, Q20, in db
    DSM_API_SET_EQ_BAND_COEFF_UPDATE            = 35,   //32-bits data, Q0
    DSM_API_SETGET_EQ_BAND_ENABLE               = 36,   //32-bits data, Q0. Bit field definitions: bit0:band1,bit1:band2,bit2:band3,bit3:band4
    DSM_API_SETGET_EQ_BAND_DISABLE              = 37,   //This command is obsolete.

    // ZIMP - imdepdence filter coefficients, for power from voltage
    DSM_API_GET_ZIMP_A1                         = 38,   //32-bits data, Q28
    DSM_API_GET_ZIMP_A2                         = 39,   //32-bits data, Q28
    DSM_API_GET_ZIMP_B0                         = 40,   //32-bits data, Q28
    DSM_API_GET_ZIMP_B1                         = 41,   //32-bits data, Q28
    DSM_API_GET_ZIMP_B2                         = 42,   //32-bits data, Q28

    // EQ Biquad coefficients
    DSM_API_SETGET_EQ1_A1                       = 43,   //32-bits data, Q28
    DSM_API_SETGET_EQ1_A2                       = 44,   //32-bits data, Q28
    DSM_API_SETGET_EQ1_B0                       = 45,   //32-bits data, Q28
    DSM_API_SETGET_EQ1_B1                       = 46,   //32-bits data, Q28
    DSM_API_SETGET_EQ1_B2                       = 47,   //32-bits data, Q28

    DSM_API_SETGET_EQ2_A1                       = 48,   //32-bits data, Q28
    DSM_API_SETGET_EQ2_A2                       = 49,   //32-bits data, Q28
    DSM_API_SETGET_EQ2_B0                       = 50,   //32-bits data, Q28
    DSM_API_SETGET_EQ2_B1                       = 51,   //32-bits data, Q28
    DSM_API_SETGET_EQ2_B2                       = 52,   //32-bits data, Q28

    DSM_API_SETGET_EQ3_A1                       = 53,   //32-bits data, Q28
    DSM_API_SETGET_EQ3_A2                       = 54,   //32-bits data, Q28
    DSM_API_SETGET_EQ3_B0                       = 55,   //32-bits data, Q28
    DSM_API_SETGET_EQ3_B1                       = 56,   //32-bits data, Q28
    DSM_API_SETGET_EQ3_B2                       = 57,   //32-bits data, Q28

    DSM_API_SETGET_EQ4_A1                       = 58,   //32-bits data, Q28
    DSM_API_SETGET_EQ4_A2                       = 59,   //32-bits data, Q28
    DSM_API_SETGET_EQ4_B0                       = 60,   //32-bits data, Q28
    DSM_API_SETGET_EQ4_B1                       = 61,   //32-bits data, Q28
    DSM_API_SETGET_EQ4_B2                       = 62,   //32-bits data, Q28

    DSM_API_SETGET_TRAJECTORY                   = 63,   //32-bits data, Q0
    DSM_API_GET_FFTTRACK_FC                     = 64,   //32-bits data, Q0

    DSM_API_SET_UPDATE_DELAY                    = 65,   //32-bits data, Q0
    DSM_API_SETGET_DELAY                        = 66,   //32-bits data, Q0
    DSM_API_GET_MAXIMUM_DELAY                   = 67,   //32-bits data, Q0

    DSM_API_SETGET_ENABLE_FAST_FC               = 68,   //32-bits data, Q0

   DSM_API_SETGET_ORM_QUALIFY_THRESH            = 69,   //32-bits data, Q0,  reuse obsolete MSG_ID "DSM_API_SETGET_MEASURE_DELAY" to faciliate ORM debugging

    DSM_API_SETGET_ENABLE_LOGGING               = 70,   //32-bits data, Q0

    DSM_API_SETGET_SPEAKER_PARAM_LFX_A1         = 71,   //32-bits data, Q28
    DSM_API_SETGET_SPEAKER_PARAM_LFX_A2         = 72,   //32-bits data, Q28
    DSM_API_SETGET_SPEAKER_PARAM_LFX_B0         = 73,   //32-bits data, Q28
    DSM_API_SETGET_SPEAKER_PARAM_LFX_B1         = 74,   //32-bits data, Q28
    DSM_API_SETGET_SPEAKER_PARAM_LFX_B2         = 75,   //32-bits data, Q28
    DSM_API_SETGET_SPEAKER_PARAM_TCTH1          = 76,   //32-bits data, Q20
    DSM_API_SETGET_SPEAKER_PARAM_TCTH2          = 77,   //32-bits data, Q20
    DSM_API_SETGET_SPEAKER_PARAM_RTH1           = 78,   //32-bits data, Q24
    DSM_API_SETGET_SPEAKER_PARAM_RTH2           = 79,   //32-bits data, Q23

    DSM_API_SETGET_SPEAKER_PARAM_ADMIT_A1       = 80,   //32-bits data, Q28
    DSM_API_SETGET_SPEAKER_PARAM_ADMIT_A2       = 81,   //32-bits data, Q28
    DSM_API_SETGET_SPEAKER_PARAM_ADMIT_B0       = 82,   //32-bits data, Q28
    DSM_API_SETGET_SPEAKER_PARAM_ADMIT_B1       = 83,   //32-bits data, Q28
    DSM_API_SETGET_SPEAKER_PARAM_ADMIT_B2       = 84,   //32-bits data, Q28

    DSM_API_SETGET_SPEAKER_PARAM_UPDATE         = 85,   //unsigned 32-bit, Q0

    DSM_API_SETGET_FC_RESET_COUNT               = 86,   //unsigned 32-bit, Q0

    DSM_API_SETGET_GENERATE_SINE_WAVE           = 87,   //32-bit, Q0

    // special APIs
    DSM_API_GET_FIRMWARE_BUILD_TIME             = 88,   //C string
    DSM_API_GET_FIRMWARE_BUILD_DATE             = 89,   //C string
    DSM_API_GET_FIRMWARE_VERSION                = 90,   //C string
    DSM_API_GET_CHIPSET_MODEL                   = 91,   //C string
    DSM_API_GET_ENDIAN                          = 92,   //32-bits data, Q0, = 0xdeadbeef

    DSM_API_SETGET_SINE_WAVE_GAIN               = 93,   //32-bits data, Q15

    DSM_API_SETGET_VIRTUAL_V_ENABLE             = 94,   //32-bits data, Q0
    DSM_API_SETGET_FORCED_VIRTUAL_V             = 95,   //32-bits data, Q0
    DSM_API_SETGET_I_SHIFT_BITS                 = 96,   //32-bits data, Q0

    DSM_API_SETGET_EXC_FUNC_GAIN_ADJUSTED       = 97,   //32-bits data, Q0

    DSM_API_SETGET_FADE_IN_TIME_MS              = 98,   //32-bits data, Q0
    DSM_API_SETGET_FADE_OUT_TIME_MS             = 99,   //32-bits data, Q0

    //This command takes effect only after
    //DSM_API_FADE_OUT_TIME_MS
    DSM_API_SETGET_FADE_OUT_MUTE_TIME_MS        = 100,  //32-bits data, Q0

    DSM_API_SETGET_FADE_IN_ENABLE               = 101,  //32-bits data, Q0
    DSM_API_SETGET_FADE_OUT_ENABLE              = 102,  //32-bits data, Q0

    DSM_API_SETGET_ENABLE_MULTICHAN_LINKING     = 103,  //32-bits data, Q0

    DSM_API_SETGET_ENABLE_SMART_PT              = 104,  //32-bits data, Q0
    DSM_API_SETGET_PILOTTONE_SILENCE_THRESHOLD  = 105,  //32-bits data, Q0
    DSM_API_SETGET_SILENCE_PILOTTONE_GAIN_Q31   = 106,  //32-bits data, Q31
    DSM_API_SETGET_SILENCE_FRAMES               = 107,  //32-bits data, Q0
    DSM_API_SETGET_PILOTTONE_TRANSITION_FRAMES  = 108,  //32-bits data, Q0

    DSM_API_SETGET_LARGE_PILOTTONE_THRESHOLD    = 109,  //32-bits data, Q0
    DSM_API_SETGET_LARGE_PILOTTONE_GAIN_Q31     = 110,  //32-bits data, Q31

    DSM_API_SETGET_ENABLE_LINKWITZ_EQ           = 111,  //32-bits data, Q0

    DSM_API_SETGET_CHANNEL_MASK                 = 112,  //32-bits data, Q0
    DSM_API_SETGET_ENABLE_FF_FB_MODULES         = 113,  //32-bits data, Q0: bit0=FF module, bit1=FB module

    DSM_API_SETGET_ENABLE_CROSSOVER             = 114,  //32-bits data, Q0

    DSM_API_SETGET_AUX_EQ_BAND_FC               = 115,  //32-bits data, Q9, in Hz
    DSM_API_SETGET_AUX_EQ_BAND_Q                = 116,  //32-bits data, Q29
    DSM_API_SETGET_AUX_EQ_BAND_ATTENUATION_DB   = 117,  //32-bits data, Q20, in db
    DSM_API_SET_EQ_AUX_BAND_COEFF_UPDATE        = 118,  //32-bits data, Q0
    DSM_API_SETGET_AUX_EQ_BAND_ENABLE           = 119,  //32-bits data, Q0. Bit field definitions: bit0:band1,bit1:band2,bit2:band3,bit3:band4

    DSM_API_SETGET_ENABLE_AUX_CROSSOVER         = 120,  //32-bits data, Q0

    DSM_API_SETGET_ENABLE_AUX1_DELAYED_SAMPLES  = 121,  //32-bits data, Q0
    DSM_API_SETGET_ENABLE_AUX2_DELAYED_SAMPLES  = 122,  //32-bits data, Q0
    DSM_API_SETGET_ENABLE_AUX3_DELAYED_SAMPLES  = 123,  //32-bits data, Q0
    DSM_API_SETGET_ENABLE_AUX4_DELAYED_SAMPLES  = 124,  //32-bits data, Q0

    DSM_API_SETGET_SPEECH_GUARD_BINS            = 125,  //32-bits data, Q0
    DSM_API_SETGET_MEAN_SPEECH_THRESHOLD        = 126,  //32-bits data, Q30

    DSM_API_SETGET_HPCUTOFF_FREQ_AUX            = 127,  //32-bits data, Q9
    DSM_API_SETGET_AUX_HP_FILTER_ENABLE         = 128,  //32-bits data, Q0

    DSM_API_SETGET_STEREO_CROSSOVER_MODE        = 129,  //32-bit data, Q0

    DSM_API_SETGET_ENABLE_UPDATE_FC_Q           = 130,  //32-bit data, Q0

    DSM_API_SETGET_RECEIVER_PHYSICAL_LAYOUT     = 131,  //32-bit data, Q0

    DSM_API_SETGET_MAX_V_SAMPLE                 = 132,  //32-bit data, Q0
    DSM_API_SETGET_MAX_I_SAMPLE                 = 133,  //32-bit data, Q0
    DSM_API_SETGET_MAX_INPUT_SAMPLE             = 134,  //32-bit data, Q0
    DSM_API_SETGET_MAX_OUTPUT_SAMPLE            = 135,  //32-bit data, Q0

    DSM_API_SETGET_PARAMS_COUNT

} DSM_API_CMD;

/*******************************************
 *
 *  Data type DSM_API_ERROR
 *
 * Description:
 *     The enum data type DSM_API_ERROR is used
 * as the data type of DSM APIs return.
 *
 * *****************************************/
typedef enum {
    DSM_API_OK = 0,
    DSM_API_ERR_NULL_MODULE_HANDLER         = 1<<1,
    DSM_API_ERR_NULL_PARAM_POINTER          = 1<<2,
    DSM_API_ERR_NULL_INPUT_BUFFER_POINTER   = 1<<3,
    DSM_API_ERR_NULL_OUTPUT_BUFFER_POINTER  = 1<<4,
    DSM_API_ERR_INVALIDE_CMD_ID             = 1<<5,
    DSM_API_ERR_INVALIDE_PARAM              = 1<<6,
    DSM_API_ERR_INVALIDE_PARAMS_NUM         = 1<<7,
    DSM_API_ERR_INVALIDE_SAMPLING_RATE      = 1<<8,

    DSM_API_ERR_NOT_IMPLEMENTED             = 1<<9,

    DSM_API_ERR_INVALID_MEMORY              = 1<<10,
    DSM_API_ERR_ZERO_I                      = 1<<11,
    DSM_API_ERR_ZERO_V                      = 1<<12,
    DSM_API_ERR_MIN_RDC_BEYOND_THRESHOLD    = 1<<13,
    DSM_API_ERR_MAX_RDC_BEYOND_THRESHOLD    = 1<<14,

    DSM_API_MISMATCHED_SETGET_CMD           = 1<<15,

    DSM_API_ERR_IV_DATA_WARNING             = 1<<16,
    DSM_API_ERR_COIL_TEMPERATURE_WARNING    = 1<<17,
    DSM_API_ERR_EXCURSION_WARNING           = 1<<18,

    DSM_API_ERR_WRONG_COMMAND_TYPE          = 1<<19,
    DSM_API_ERR_COMMAND_OBSOLETE            = 1<<20,

    DSM_API_ERR_INSUFFICIENT_INPUT_DATA     = 1<<21,

    DSM_API_ERR_INVALID
} DSM_API_ERROR;

/****************************************************
 *
 * Data type DSM_API_Data_Type
 *
 * Description:
 *     The enum data type DSM_API_Data_Type is used
 * to indicate the data type of DSM parameters.
 *
 * *************************************************/
typedef enum {
    DSM_Int8 = 0,                           //8-bit integer
    DSM_Int16,                              //16-bit integer
    DSM_Int32,                              //32-bit integer
    DSM_Int64,                              //64-bit integer
    DSM_Float,                              //32-bit IEEE float
    DSM_Double,                             //64-bit IEEE double float
    DSM_C_String,                           //C string with the termination '\0'
    DSM_Buffer,                             //a pointer pointed at a data buffer allocated by caller function

    DSM_Invalide //invalide
} DSM_API_Data_Type;

enum DSM_FEATURES {
    DSM_FEATURE_PILOT_ENABLE                = (1<<0x00),
    DSM_FEATURE_EXC_ENABLE                  = (1<<0x01),
    DSM_FEATURE_THERMAL_ENABLE              = (1<<0x02),
    DSM_FEATURE_OUTPUT_GAIN_CONTROL_ENABLE  = (1<<0x03),
    DSM_FEATURE_NUM
};

/*      Interface functions       */

/*******************************************************************************
 *    DSM_API_get_memory_size()
 *
 * Description:
 *     This function returns the size of data memory which is required by DSM module
 * and must be called before any other DSM API functions.
 *     The DSP framework should be responsible for allocating memory for
 * DSM module.
 *
 * Input:
 *     iChannels:  the number of channels which are requested to be supported.
 *     ipCircBufferSize:   a 32-bit integer array which contains the sizes of input, output,
 *                I data and V data buffers. These buffers are used for debugging or
 *                performance tuning.
 *                   The format of this argument is as below:
 *                +--------------------+\
 *                |  input buffer size | \
 *                +--------------------+  \
 *                | output buffer size |   \
 *                +--------------------+   -> channel 1
 *                | I data buffer size |   /
 *                +--------------------+  /
 *                | V data buffer size | /
 *                +--------------------+/
 *                       .
 *                       .
 *                       .
 *                +--------------------+\
 *                |  input buffer size | \
 *                +--------------------+  \
 *                | output buffer size |   \
 *                +--------------------+   -> channel N
 *                | I data buffer size |   /
 *                +--------------------+  /
 *                | V data buffer size | /
 *                +--------------------+/
 *
 * Output:
 *     opMemSizeRequired:
 *          the address of a 32-bit variable which contains the size of memory.
 *
 * Returns:
 *     DSM_API_OK: successful;
 *     Otherwise:  error code.
 *
 *******************************************************************************/
extern DSM_API_ERROR DSM_API_get_memory_size(
        short    iChannels,
        int     *ipCircBufferSize,
        int     *opMemSizeRequired
);

/*******************************************************************************
 *    DSM_API_get_memory_size_ext()
 *
 * Description:
 *     This function returns the size of data memory which is required by DSM module
 * and must be called before any other DSM API functions.
 *     The DSP framework should be responsible for allocating memory for
 * DSM module.
 *
 * Input:
 *     iParamSize: the size of the data structure dsm_api_memory_size_ext_t.
 *
 * Output:
 *     N/A
 *
 * I/O:
 *     iopMemParam: the address of data structure containing input and output arguments.
 *
 * Returns:
 *     DSM_API_OK: successful;
 *     Otherwise:  error code.
 *
 *******************************************************************************/
DSM_API_ERROR DSM_API_get_memory_size_ext(
        dsm_api_memory_size_ext_t   *iopMmemParam,
        int                         iParamSize);

/*******************************************************************************
 *    DSM_API_Init()
 *
 * Description:
 *     This function is used to initialize DSM module and must be called after
 * DSM_API_get_memory_size() and before all other DSM API functions.
 *
 * Inputs:
 *     ipModuleHandler:
 *                 the address of DSM module which is allocated by framework caller.
 *     iSamplerate: the sampling rate of input PCM data to DSM module.
 *     iChannels:   the number of channels which are requested to be supported.
 *                     The returned memory size and frame sizes depend on the input sampling rate.
 *                 independent of the sampling rate.
 *     ipCircBufferSize: a 32-bit integer array which contains the sizes of input, output,
 *                 I data and V data buffers. These buffers are used for debugging or
 *                 performance tuning. This parameter should be the same as the one used
 *                 in DSM_API_get_memory_size().
 *
 * Outputs:
 *     opFrame_size:the address of a 32-bit integer which contains the size of frame in samples.
 *
 * Returns:
 *     DSM_API_OK: successful;
 *     Otherwise:  error codes.
 *
 *******************************************************************************/
extern DSM_API_ERROR DSM_API_Init(
        void            *ipModuleHandler,
        unsigned int     iSamplerate,
        short            iChannels,
        int             *ipCircBufferSize,
        int             *opFrame_size
);

/*******************************************************************************
 *    DSM_API_Init_ext()
 *
 * Description:
 *     This function is used to initialize DSM module and must be called after
 * DSM_API_get_memory_size() and before all other DSM API functions.
 *
 * Inputs:
 *     ipModuleHandler:
 *                  the handler of DSM module which is allocated by framework caller.
 *     iParamSize:  the size of the data structure dsm_api_init_ext_t.
 *
 * Outputs:
 *     N/A
 *
 * I/O:
 *     iopParamStruct: the address of a data structure containing input and output arguments.
 *
 * Returns:
 *     DSM_API_OK: successful;
 *     Otherwise:  error codes.
 *
 *******************************************************************************/
extern DSM_API_ERROR DSM_API_Init_ext(
        void                *ipModuleHandler,
        dsm_api_init_ext_t  *iopParamStruct,
        int                  iParamSize
);

/*******************************************************************************
 *    DSM_API_FF_process()
 *
 * Description:
 *     This function is used to process the input audio PCM DSM data.
 *
 * Inputs:
 *     ipModuleHandler:
 *                    the handler of DSM module which is allocated by framework caller.
 *     iChannelMask: the low 8-bits indicate which channels should be executed.
 *                  0: default channels setting, mono or stereo
 *                  1: L channel
 *                  2: R channel
 *                  3: L and R channels
 *                 -1: place input L channel onto output R channel
 *                 -2: place input R channel onto output L channel
 *                 -3: switch L and R channel
 *     ipInputBuffer:    the input buffer which contains 16-bit audio PCM input data.
 *                 The multi-channel input PCM data are ordered in the format below:
 *                 +-----------------------+    ...    +-------------------------+
 *                 |    one frame samples  |    ...    |   one frame samples     |
 *                 +-----------------------+    ...    +-------------------------+
 *                      1st channel             ...           N-th channel
 *
 * Outputs:
 *     opOutputBuffer:   the output buffer which contains the 16-bit PCM data processed by DSM.
 *                The multi-channel output PCM data are ordered in the format below:
 *                 +-----------------------+    ...    +-------------------------+
 *                 |    one frame samples  |    ...    |   one frame samples     |
 *                 +-----------------------+    ...    +-------------------------+
 *                      1st channel             ...           N-th channel
 *     opNrSamples:   the address of a variable which contains the number of samples in output buffer.
 *
 * I/O:
 *     iopInputSamples: the number of audio PCM samples to be processed, in 32-bit
 *                   long integer.
 *                       The returned value indicates how many samples of input data are
 *                   not used in the input buffer. In this case, DSP framework should
 *                   send back the remaining data in next process.
 *
 * Returns:
 *     DSM_API_OK: successful;
 *     Otherwise:  error codes.
 *
 *******************************************************************************/
extern DSM_API_ERROR DSM_API_FF_process(
        void            *ipModuleHandler,
        int              iChannelMask,
        short           *ipInputBuffer,
        int             *iopInputSamples,
        short           *opOutputBuffer,
        int             *opOutputSamples
);

/*******************************************************************************
 *    DSM_API_FB_process()
 *
 * Description:
 *     This function is used to process current(I) and voltage(V) feedback data.
 *
 * Inputs:
 *     ipModuleHandler:
 *                    the handler of DSM module which is allocated by framework caller.
 *     channelMask:   the low 8-bits indicate which channels should be executed.
 *                  0: default channels setting, mono or stereo
 *                  1: L channel
 *                  2: R channel
 *                  3: L and R channels
 *                 -1: place input L channel onto output R channel
 *                 -2: place input R channel onto output L channel
 *                 -3: switch L and R channel
 *     iCurrBuffer:   the input buffer which contains I data.
 *                 The multi-channel I data are ordered in the format below:
 *                 +-----------------------+    ...    +-------------------------+
 *                 |    one frame samples  |    ...    |   one frame samples     |
 *                 +-----------------------+    ...    +-------------------------+
 *                      1st channel             ...           N-th channel
 *     iVoltBuffer:   the input buffer which contains V data.
 *                 The multi-channel V data are ordered in the format below:
 *                 +-----------------------+    ...    +-------------------------+
 *                 |    one frame samples  |    ...    |   one frame samples     |
 *                 +-----------------------+    ...    +-------------------------+
 *                      1st channel             ...           N-th channel
 *
 * I/O:
 *     iopNrSamples:  the address of a variable which contains number of I/V data to
 *                    be processed.
 *                        The returned value indicates how many samples of I/V data are
 *                    not used in the I/V buffer. In this case, DSP framework should send
 *                    back the remaining I and V data in next process.
 *
 * Outputs:
 *     N/A
 *
 * Returns:
 *     DSM_API_OK: successful;
 *     Otherwise:  error codes.
 *
 *******************************************************************************/
extern DSM_API_ERROR DSM_API_FB_process(
        void            *ipModuleHandler,
        int              iChannelMask,
        short           *iCurrBuffer,
        short           *iVoltBuffer,
        int             *iopNrSamples
);
#define DSM_API_process_iv      DSM_API_FB_process  //keep backward compatible

/*******************************************************************************
 *    DSM_API_generate_auxiliary_channels()
 *
 * Description:
 *     This function is used to generate data for auxiliary channels which are
 * usually connected to speakers that don't need DSM processing.
 *
 * Inputs:
 *     ipModuleHandler:
 *                  the handler of DSM module which is allocated by framework caller.
 *     iChannelMask:  the low 8-bits indicate which channels should be executed.
 *                  0: default channels setting, mono or stereo
 *                  1: L channel
 *                  2: R channel
 *                  3: L and R channels
 *                 -1: place input L channel onto output R channel
 *                 -2: place input R channel onto output L channel
 *                 -3: switch L and R channel
 *     iChannelBuffer:   the input buffer which contains input PCM data.
 *                 The multi-channel I data are ordered in the format below:
 *                 +-----------------------+    ...    +-------------------------+
 *                 |    one frame samples  |    ...    |   one frame samples     |
 *                 +-----------------------+    ...    +-------------------------+
 *                      1st channel             ...           N-th channel
 *
 * Outputs:
 *     oChannelBuffer:   the output buffer which contains the PCM data generated based on input data.
 *                The multi-channel output PCM data are ordered in the format below:
 *                 +-----------------------+    ...    +-------------------------+
 *                 |    one frame samples  |    ...    |   one frame samples     |
 *                 +-----------------------+    ...    +-------------------------+
 *                      1st channel             ...           N-th channel
 *
 * I/O:
 *     iopNrSamples:  the address of a variable which contains number of I/V data to
 *                    be processed.
 *                        The returned value indicates how many samples of I/V data are
 *                    not used in the I/V buffer. In this case, DSP framework should send
 *                    back the remaining I and V data in next process.
 *
 * Outputs:
 *     N/A
 *
 * Returns:
 *     DSM_API_OK: successful;
 *     Otherwise:  error codes.
 *
 *******************************************************************************/
extern DSM_API_ERROR DSM_API_generate_auxiliary_channels(
        void            *ipModuleHandler,
        int             iChannelMask,
        void            *iChannelBuffer,
        void            *oChannelBuffer,
        int             *iopNrSamples
);

/*******************************************************************************
 *    DSM_API_param_size_inquery()
 *
 * Description:
 *     This function is used to inquiry the memory size of a parameter.
 *
 * Inputs:
 *     cmd:      a 32-bit command ID.
 * Outputs:
 *     opBytes:  the memory size of the inquired parameter, in bytes.
 *     oDataType:the data type of the inquired parameter
 * Returns:
 *     DSM_API_OK: successful;
 *     Otherwise:  error codes.
 *
 *******************************************************************************/
extern DSM_API_ERROR DSM_API_param_size_inquery(
        int                 cmd,
        int                 *opBytes,
        DSM_API_Data_Type   *oDataType
);

/*******************************************************************************
 *    DSM_API_get_excursion_log_data()
 *    DSM_API_get_temperature_log_data()
 *
 * Description:
 *     These two functions are used to get log data which are recorded
 * when excursion or coil temperature hit their thresholds respectively.
 *
 * Inputs:
 *     ipModuleHandler:
 *              the handler of DSM module which is allocated by framework caller.
 *
 * Outputs:
 *     oLogDataBuf: the address of the log data in the format of structure dsm_param_log_t.
 *
 * Returns:
 *     DSM_API_OK: successful;
 *     Otherwise:  error codes.
 *
 *******************************************************************************/
extern DSM_API_ERROR DSM_API_get_excursion_log_data(
        void            *ipModuleHandler,
        dsm_param_log_t *oLogDataBuf
);

extern DSM_API_ERROR DSM_API_get_temperature_log_data(
        void            *ipModuleHandler,
        dsm_param_log_t *oLogDataBuf
);

/*******************************************************************************
 *    DSM_API_Set_Params()
 *
 * Description:
 *     This function is used to set a serial of DSM parameters in one call.
 *
 * Inputs:
 *     ipModuleHandler:
 *               the handler of DSM module which is allocated by framework caller.
 *     cmdNum:   the number of commands. The total memory size of this
 *               input argument
 *                    = (cmdNum * 2) * sizeof(int) bytes.
 *     ipParams: the buffer of input parameters which are stored in the format below.
 *               The parameters should be set separately for each channel.
 *               ipValue memory map:
 *               ---+-----------+-------------------
 *                  |   cmd_1   |  32-bits integer command
 *                  +-----------+
 *                  |  param_1  |  32-bits data
 *               ---+-----------+-------------------
 *                  |    ...    |
 *                  |    ...    |
 *                  |    ...    |
 *               ---+-----------+-------------------
 *                  |   cmd_N   |  32-bits integer command
 *                  +-----------+
 *                  |  param_N  |  N-th 32-bit data
 *               ---+-----------+-------------------
 *            Total: N parameters
 *
 * Outputs:
 *     N/A.
 *
 * Returns:
 *     DSM_API_OK: successful;
 *     Otherwise:  error codes.
 *
 *******************************************************************************/
extern DSM_API_ERROR DSM_API_Set_Params(
        void    *ipModuleHandler,
        int      iCommandNumber,
        void    *ipParamsBuffer
);

/*******************************************************************************
 *    DSM_API_Get_Params()
 *
 * Description:
 *     This function is used to get a serial of DSM parameters in one call.
 *
 * Inputs:
 *     ipModuleHandler:
 *              the handler of DSM module which is allocated by framework caller.
 *     cmdNum:   the number of parameters. The total memory size of this
 *               input argument
 *                    = cmdNum * (1 + channel_number) ) * sizeof(int) bytes
 *
 * Outputs:
 *     opParams: the buffer of the output parameters which are stored in the format below.
 *               opParams memory map:
 *             -----+-------------+-----------------------------
 *                  |    cmd_1    |  32-bits integer command Id
 *                  +-------------+
 *                  |  param_1_1  |  32-bits data of channel 1\
 *                  +-------------+                            \
 *                  +    ......   +                             -> parameters of N channels
 *                  +-------------+                            /
 *                  |  param_1_C  |  32-bits data of channel N/
 *             -----+-------------+-----------------------------
 *                  +    ......   +
 *                  +    ......   +
 *                  +    ......   +
 *             -----+-------------+-----------------------------
 *                  +    cmd_M    +  32-bits integer command Id
 *                  +-------------+
 *                  |  param_M_1  |  32-bits data of channel 1\
 *                  +-------------+                            \
 *                  +    ......   +                             -> parameters of N channels
 *                  +-------------+                            /
 *                  |  param_M_C  |  32-bits data of channel N/
 *             -----+-------------+------------------------------
 *            Total: M parameters of N channels
 *
 * 			Exception: the parameter sizes of the commands
 * 	                       DSM_API_GET_FIRMWARE_BUILD_TIME      = 88, //C string
 *                         DSM_API_GET_FIRMWARE_BUILD_DATE      = 89, //C string
 *                         DSM_API_GET_FIRMWARE_VERSION         = 90, //C string
 *                         DSM_API_GET_CHIPSET_MODEL            = 91, //C string
 *               are DSM_MAX_STRING_PARAM_SIZE (=32) bytes rather than 4 bytes of
 *               32-bits data because these parameters are in the format C string.
 *
 * Returns:
 *     DSM_API_OK: successful;
 *     Otherwise:  error codes.
 *
 *******************************************************************************/
extern DSM_API_ERROR DSM_API_Get_Params(
        void    *ipModuleHandler,
        int      iCommandNum,
        void    *opParams
);


/*******************************************************************************
 *    DSM_API_Assign_Mutex()
 *
 * Description:
 *     This function is used to set a mutex to each channel in a multithreading system.
 *
 * Inputs:
 *     chanId:  a 32-bit channel ID starting from 0 through the maximum number of channels.
 *     iMutex:  the address of a mutex which is created by DSP framework.
 *
 * Outputs:
 *     N/A.
 *
 * Returns:
 *     DSM_API_OK: successful;
 *     Otherwise:  error codes.
 *
 *******************************************************************************/
extern DSM_API_ERROR DSM_API_Assign_Mutex(
        int     chanId,
        void    *iMutex
);

/*******************************************************************************
 *    iMutexLockFunc()
 *
 * Description:
 *     This function is used to assign a mutex lock function in a multi-threading
 * system.
 *
 * Inputs:
 *     iMutexLockFunc:  the address of the function which is used to lock a mutex.
 *                  This function should have only one input argument which passes
 *                  the address of a mutex.
 *
 * Outputs:
 *     N/A.
 *
 * Returns:
 *     DSM_API_OK: successful;
 *     Otherwise:  error codes.
 *
 *******************************************************************************/
extern DSM_API_ERROR DSM_API_Assign_Mutex_Lock_function(
        void (*iMutexLockFunc)(void*)
);

/*******************************************************************************
 *    iMutexUnlockFunc()
 *
 * Description:
 *     This function is used to assign a mutex unlock function in a multi-threading
 * system.
 *
 * Inputs:
 *     iMutexUnlockFunc:  the address of the function which is used to unlock a mutex.
 *                  This function should have only one input argument which passes
 *                  the address of a mutex.
 *
 * Outputs:
 *     N/A.
 *
 * Returns:
 *     DSM_API_OK: successful;
 *     Otherwise:  error codes.
 *
 *******************************************************************************/
DSM_API_ERROR DSM_API_Assign_Mutex_Unlock_function(
        void (*iMutexUnlockFunc)(void*)
);


/***********************************************************************
 *   The Example Use of DSM APIs
 *
 *     There are several steps to use the DSM APIs:
 * -- get the memory size which is required by DSM module
 * -- allocate memory for DSM module
 * -- set configuration parameters, if necessary
 * -- Execute DSM forward and feedback processing
 * -- retrieve some DSM parameters for debugging and monitoring
 *
 * ------------------------------
 *
 * Step_1: allocate data memory for DSM memory
 *
 *  #define DSM_CHANNELS        2
 *
 * 	//request the size of memory which is allocated to DSM module
 *  void            *dsm_handler;
 *  int             memSize;
 *  DSM_API_ERROR   retCode;
 *  int             bufferSize[DSM_CHANNELS*4];
 *
 *  for(i=0; i<DSM_CHANNELS; i++)
 *  {
 *      bufferSize[4*i+0]     = 0;//input circular buffer size
 *      bufferSize[4*i+1]     = 0;//output circular buffer size
 *      bufferSize[4*i+2]     = 0;//I circular buffer size
 *      bufferSize[4*i+3]     = 0;//V circular buffer size
 *  }
 *
 * 	retCode = DSM_API_get_memory_size(DSM_CHANNELS, bufferSize, &memSize);
 *  if( DSM_API_OK == retCode ){
 * 		dsm_handler = (int*)malloc(memSize);
 * 		if( 0==dsm_handler ){
 * 			//error handling
 * 		}
 *  }
 *  else{
 * 		//error handling
 *  }
 *
 * 	  The dsm_handler will be used by all of DSM API functions to specify the instance
 * of a DSM module.
 *
 * ------------------------------
 *
 * Step_2: Initialize DSM module
 *
 * 	//Initialize DSM module
 *  DSM_API_ERROR	retCode;
 *  int             frame_size;
 *  int             bufferSize[DSM_CHANNELS*4];
 *  int             delayedSamples[DSM_CHANNELS*4];
 *
 *
 *  // !!! the buffer sizes MUST be the same as used for calling DSM_API_get_memory_size()
 *
 *  for(i=0; i<DSM_CHANNELS; i++)
 *  {
 *      bufferSize[4*i+0]     = 0;//input circular buffer size
 *      bufferSize[4*i+1]     = 0;//output circular buffer size
 *      bufferSize[4*i+2]     = 0;//I circular buffer size
 *      bufferSize[4*i+3]     = 0;//V circular buffer size
 *
 *      delayedSamples[4*i+0] = 0;//input buffer delayed samples
 *      delayedSamples[4*i+1] = 0;//output buffer delayed samples
 *      delayedSamples[4*i+2] = 0;//I buffer delayed samples
 *      delayedSamples[4*i+3] = 0;//V buffer delayed samples
 *  }
 *
 *	retCode = DSM_API_Init(dsm_handler,
 *	                       SAMPLING_RATE_SUPPORTED,
 *	                       DSM_CHANNELS,
 *	                       bufferSize,
 *	                       (unsigned int *)&frame_size,
 *	                       delayedSamples
 *	                       );
 *  if( DSM_API_OK != retCode ){
 * 		//error handling
 *  }
 *  if( 0 == frame_size )
 * {
 *      //error handling
 * }
 *
 * ------------------------------
 *
 * Step_3: Set a group of DSM parameters in run-time
 *
 *  #define DSM_CHANNELS            2//1=mono; 2=stereo
 *  #include "dsm_api.h"
 *
 *  DSM_API_ERROR	retCode;
 *  int             userParam[2*3];	//assuming to set 3 parameters
 *
 *
 *
 *  //assume to set 3 parameters
 *  userParam[2*0  ] = DSM_SET_PARAM(2);    //set coil temperature limit
 *  userParam[2*0+1] = TO_FIX(80.0,19);     //set to 80C degree
 *
 *  userParam[2*1]   = DSM_SET_PARAM(3);    //set excursion limit
 *  userParam[2*1+1] = TO_FIX(0.45,27);     //set to 0.45mm
 *
 *  userParam[2*2]   = DSM_SET_PARAM(6);    //set default Rdc
 *  userParam[2*2+1] = TO_FIX(0.206, 27);   //set to 2.06
 *
 *  retCode = DSM_API_Set_Params(dsm_handler, 3, userParam,);
 *  if( DSM_API_OK != retCode ){
 *      printf("DSM_API_Set_Params() failed: error code = %i\n", (int)retCode);
 * 		//error handling
 *  }
 * ------------------------------
 *
 * Step_4: Process audio PCM data and feedback I-V data
 *
 *  DSM_API_ERROR	retCode;
 *  int             remaining_samples, outputSamples;
 *  short           *pcmIn, *pcmOut;
 *  short           *iData, *vData;
 *
 *  remaining_samples  = input_samples;//the samples to be processed
 *  pcmIn              = (short*)???;//assign the 16-bit PCM input buffer
 *  pcmOut             = (short*)???;//assign the 16-bit PCM output buffer
 *
 *  iData              = (short*)???;//assign the 16-bit I buffer
 *  vData              = (short*)???;//assign the 16-bit V buffer
 *
 *  //process DSM feedback path
 *  retCode = DSM_API_FB_process((void *)dsm_handler, iData, vData, &remaining_samples);
 *  if( DSM_API_OK != retCode ){
 *      //error handling
 *  }
 *  if( 0 != remaining_samples )
 *  {
 *      //store the remaining (input_samples-remaining_samples) samples
 *      //input data for processing in next call
 *  }
 *
 * 	//process DSM forward path
 *  remaining_samples  = input_samples;//the samples to be processed
 *	retCode = DSM_API_FF_process((void *)dsm_handler, pcmIn, &remaining_samples, pcmOut, &outputSamples);
 *  if( DSM_API_OK != retCode ){
 * 		//error handling
 *  }
 *  if( 0 != remaining_samples ){
 *      //store the remaining (input_samples-remaining_samples) samples
 *      //input data for processing in next call
 *  }
 *
 *
 * ------------------------------
 *
 * Step_5 (optional): Inquiry DSM parameters in run-time
 *
 *  //parameter size is recommended to be inquired in run-time.
 *  long  			paramBytes, *paramBuf;
 *  DSM_Data_Type	dataType;
 *  DSM_API_ERROR	retCode;
 *
 *  //all parameters are in 32-bit containers except for
 *  //commands 88, 89, 90 and 91 which are in C string format.
 *  retCode = DSM_API_param_size_inquery(cmdId, &paramBytes, &DSM_Data_Type);
 *  if( DSM_API_OK == retCode ){
 *  	if( paramBytes > 0 ){
 *    		paramBuf = malloc(paramBytes);
 *          if( o == paramBuf ){
 * 				//error handling
 *          }
 *  	}
 *  	else{
 * 			//error handling
 *  	}
 *
 * 		//get/set DSM parameters
 * 		...
 *
 * 		if( paramBuf )
 * 			free(paramBuf);
 *  }
 *  else{
 * 		//error handling
 *  }
 *
 * ------------------------------
 *
 * Step_6: Get a group of DSM parameters in run-time
 *
 *  DSM_API_ERROR	retCode;
 *  long    		dsmParam[(1+DSM_CHANNELS)*5];	//assuming to get 5 parameters of 32-bit integer
 *
 *   //get 5 adaptive DSM parameters
 *   *lPtr++ = DSM_API_GET_ADAPTIVE_FC;
 *    lPtr+=DSM_CHANNELS; //floating-point value stored here
 *    *lPtr++ = DSM_API_GET_ADAPTIVE_Q;
 *    lPtr+=DSM_CHANNELS; //floating-point value stored here
 *    *lPtr++ = DSM_API_GET_ADAPTIVE_DC_RES;
 *    lPtr+=DSM_CHANNELS; //floating-point value stored here
 *    *lPtr++ = DSM_API_GET_ADAPTIVE_COILTEMP;
 *    lPtr+=DSM_CHANNELS; //floating-point value stored here
 *    *lPtr++ = DSM_API_GET_EXCURSION;
 *    lPtr+=DSM_CHANNELS; //floating-point value stored here
 *
 *   //issue one command to get 5 run-time DSM parameters
 *    retCode = DSM_API_Get_Params(dsm_handler, 5, (void *)dsmParam);
 *    if (DSM_API_OK != retCode) {
 *        printf("DSM_API_Get_Params() failed: error code = %i\n", (int)retCode);
 *        return retCode;
 *    }
 *
 *
 *  (END of example codes)
 *
 ***********************************************************************/


#ifdef __cplusplus
}
#endif

#endif
