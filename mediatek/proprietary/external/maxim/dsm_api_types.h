#ifndef DSM_API_TYPES
#define DSM_API_TYPES

#define A_COEFF_LEN                             3
#define B_COEFF_LEN                             3

#define AVG_200MS_SIZE                          40
#define LOG_2S_SIZE	                            10

//! The structure that keeps biquad filter coefficients and state variables
//! biquad structure
typedef struct biqcoeff_
{
    //holds den coeff
    int a_coeff_f[A_COEFF_LEN]; // Q4.28
    //holds num coeff
    int b_coeff_f[B_COEFF_LEN]; // Q4.28
    // coeff scale value
    int scale_f;
} biqcoeff_f;

typedef struct dsm_speaker_params_
{
    //holds den coeff
    biqcoeff_f spk_xc_extra_coeffQ28;
    float RTH1_f; //(51.5)
    float TCTH1_f; //(2)
    float RTH2_f; //(119)
    float TCTH2_f; //(138)
    float FIXED_ADMIT_A1_f; //TO_FIX(-1.86828671, FIX_ADMIT_QFORMAT)
    float FIXED_ADMIT_A2_f; //TO_FIX(0.87732655,  FIX_ADMIT_QFORMAT)
    float FIXED_ADMIT_B0_f; //TO_FIX(0.146768839, FIX_ADMIT_QFORMAT)
    float FIXED_ADMIT_B1_f; //TO_FIX(-0.287487241,FIX_ADMIT_QFORMAT)
    float FIXED_ADMIT_B2_f; //TO_FIX(0.142050629, FIX_ADMIT_QFORMAT)
} dsm_speaker_params_f;

typedef struct dsm_param_log
{
    unsigned char coilTempProbPersistentQ0[LOG_2S_SIZE];
    unsigned short excurProbPersistentQ7[LOG_2S_SIZE];
    unsigned short rdcProbPersistentQ11[LOG_2S_SIZE];
    unsigned short freqProbPersistentQ4[LOG_2S_SIZE];
} dsm_param_log_t;

typedef struct dsm_user_log
{
    dsm_param_log_t afterExcurProbPersistent;
    //	unsigned int 	freqAfterExcurProbPersistent[LOG_2S_SIZE];
    //	unsigned char 	coilTempAfterExcurProbPersistent[LOG_2S_SIZE];
    //	unsigned char 	excurAfterExcurProbPersistent[LOG_2S_SIZE];
    //	unsigned short 	rdcAfterExcurProbPersistent[LOG_2S_SIZE];

    dsm_param_log_t afterTemperatureProbPersistent;
    //	unsigned int 	freqAfterTempProbPersistent[LOG_2S_SIZE];
    //	unsigned char 	coilTempAfterTempProbPersistent[LOG_2S_SIZE];
    //	unsigned char 	excurAfterTempProbPersistent[LOG_2S_SIZE];
    //	unsigned short 	rdcAfterTempProbPersistent[LOG_2S_SIZE];

    unsigned int tempLogStatus;
    unsigned int excurLogStatus;
    unsigned int RdcLogStatus;
    unsigned int FcLogStatus;

    unsigned int minTemp;
    unsigned int minExcur;
    unsigned int minRdc;
    unsigned int minFc;

    unsigned int maxTemp;
    unsigned int maxExcur;
    unsigned int maxRdc;
    unsigned int maxFc;

} dsm_user_log_t;

typedef struct dsm_api_memory_size_ext
{
    int     iSamplingRate;                  //the sampling rate of input PCM samples, in Hz
                                            //supported sampling rates={48KHZ, 44.1KHz, 16KHZ}
    int     iChannels;                      //the number of maximum channels supported
    int     *ipCircBufferSizeBytes;         //the sizes of, if used, circular buffers, in samples
    int     oMemSizeRequestedBytes;         //the memory size required by DSM module, in bytes
}dsm_api_memory_size_ext_t;

typedef struct dsm_api_init_ext{
    int     iSamplingRate;                  //the sampling rate of input PCM samples, in Hz
                                            //supported sampling rates={48KHZ, 44.1KHz, 16KHZ}
    int     iChannels;                      //the number of maximum channels supported
    int     oFF_FrameSizeSamples;           //the frame size of DSM feed-forward processing, in samples
    int     oFB_FrameSizeSamples;           //the frame size of DSM backward processing, in samples
    int     *ipCircBufferSizeBytes;         //the sizes of, if used, circular buffers, in samples
    int     *ipDelayedSamples;              //the delayed samples if circular buffers are used
    int     iSampleBitWidth;                //the number of bits of one input PCM sample, ={32, 16}
} dsm_api_init_ext_t;

#endif	//DSM_API_TYPES
