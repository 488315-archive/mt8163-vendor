/*======================================================================*
 DTS, Inc.
 5220 Las Virgenes Road
 Calabasas, CA 91302  USA

 CONFIDENTIAL: CONTAINS CONFIDENTIAL PROPRIETARY INFORMATION OWNED BY
 DTS, INC. AND/OR ITS AFFILIATES ("DTS"), INCLUDING BUT NOT LIMITED TO
 TRADE SECRETS, KNOW-HOW, TECHNICAL AND BUSINESS INFORMATION. USE,
 DISCLOSURE OR DISTRIBUTION OF THE SOFTWARE IN ANY FORM IS LIMITED TO
 SPECIFICALLY AUTHORIZED LICENSEES OF DTS.  ANY UNAUTHORIZED
 DISCLOSURE IS A VIOLATION OF STATE, FEDERAL, AND INTERNATIONAL LAWS.
 BOTH CIVIL AND CRIMINAL PENALTIES APPLY.

 DO NOT DUPLICATE. COPYRIGHT 2014, DTS, INC. ALL RIGHTS RESERVED.
 UNAUTHORIZED DUPLICATION IS A VIOLATION OF STATE, FEDERAL AND
 INTERNATIONAL LAWS.

 ALGORITHMS, DATA STRUCTURES AND METHODS CONTAINED IN THIS SOFTWARE
 MAY BE PROTECTED BY ONE OR MORE PATENTS OR PATENT APPLICATIONS.
 UNLESS OTHERWISE PROVIDED UNDER THE TERMS OF A FULLY-EXECUTED WRITTEN
 AGREEMENT BY AND BETWEEN THE RECIPIENT HEREOF AND DTS, THE FOLLOWING
 TERMS SHALL APPLY TO ANY USE OF THE SOFTWARE (THE "PRODUCT") AND, AS
 APPLICABLE, ANY RELATED DOCUMENTATION:  (i) ANY USE OF THE PRODUCT
 AND ANY RELATED DOCUMENTATION IS AT THE RECIPIENT’S SOLE RISK:
 (ii) THE PRODUCT AND ANY RELATED DOCUMENTATION ARE PROVIDED "AS IS"
 AND WITHOUT WARRANTY OF ANY KIND AND DTS EXPRESSLY DISCLAIMS ALL
 WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO ANY
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE, REGARDLESS OF WHETHER DTS KNOWS OR HAS REASON TO KNOW OF THE
 USER'S PARTICULAR NEEDS; (iii) DTS DOES NOT WARRANT THAT THE PRODUCT
 OR ANY RELATED DOCUMENTATION WILL MEET USER'S REQUIREMENTS, OR THAT
 DEFECTS IN THE PRODUCT OR ANY RELATED DOCUMENTATION WILL BE
 CORRECTED; (iv) DTS DOES NOT WARRANT THAT THE OPERATION OF ANY
 HARDWARE OR SOFTWARE ASSOCIATED WITH THIS DOCUMENT WILL BE
 UNINTERRUPTED OR ERROR-FREE; AND (v) UNDER NO CIRCUMSTANCES,
 INCLUDING NEGLIGENCE, SHALL DTS OR THE DIRECTORS, OFFICERS, EMPLOYEES,
 OR AGENTS OF DTS, BE LIABLE TO USER FOR ANY INCIDENTAL, INDIRECT,
 SPECIAL, OR CONSEQUENTIAL DAMAGES (INCLUDING BUT NOT LIMITED TO
 DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION, AND LOSS
 OF BUSINESS INFORMATION) ARISING OUT OF THE USE, MISUSE, OR INABILITY
 TO USE THE PRODUCT OR ANY RELATED DOCUMENTATION.
*======================================================================*/

#include "srs_techs/srs_tech_headers.h"
#include <media/AudioSystem.h>

#include SRSLIBINC(srs_aeq_api.h)
#include SRSLIBINC(srs_misc_api.h)
#include SRSLIBINC(srs_common_ver_api.h)
#include SRSLIBINC(srs_parametriceq_design_api.h)

extern "C" {
    #include "b64/cdecode.h"
}

#undef  LOG_TAG
#define LOG_TAG "SRS_Tech_AEQ"

 char AEQScratch[512];

namespace android {

static SRSAeqCreateData aeqAttr = { {(MAX_NUM_AEQ_BANDS*2), 1024} };

struct SRS_Source_AEQ {

    SRSAeqObj       pObjL;
    SRSAeqObj       pObjR;
    int             AEQWorkspace[9216/4]; //SRS_AEQ_WORKSPACE_SIZE(4096)
    void            *pPBufferL;
    void            *pPBufferR;
    void            *pFftPlanBuf;
    int             SampleRate;
    int             ChannelCount;
    SRS_Tech_AEQ    Active;
    bool            ForceActive;
#if defined (_SRSCFG_ARCH_ARM) || defined (_SRSCFG_ARCH_ARM64)
    SRSCoef32       CoefL[SRS_PEQ_FXP_COEFFICIENT_ARRAY_LEN(MAX_NUM_AEQ_BANDS)];
    SRSCoef32       CoefR[SRS_PEQ_FXP_COEFFICIENT_ARRAY_LEN(MAX_NUM_AEQ_BANDS)];
#endif  // _SRSCFG_ARCH_ARM
#ifdef _SRSCFG_ARCH_X86
    SRSCoef32       CoefL[SRS_PEQ_FLOAT_COEFFICIENT_ARRAY_LEN(MAX_NUM_AEQ_BANDS)];
    SRSCoef32       CoefR[SRS_PEQ_FLOAT_COEFFICIENT_ARRAY_LEN(MAX_NUM_AEQ_BANDS)];
#endif  // _SRSCFG_ARCH_X86
    bool            DidCreate;
    bool            DidConfig;
#if defined (_SRSCFG_ARCH_ARM) || defined (_SRSCFG_ARCH_ARM64)
    unsigned short  FIRCoeff[4096/2];
#endif  // _SRSCFG_ARCH_ARM
#ifdef _SRSCFG_ARCH_X86
    float  FIRCoeff[4096/2];
#endif  // _SRSCFG_ARCH_X86
    bool            FIRCoeffValid;

    SRS_Source_AEQ();
    ~SRS_Source_AEQ();

    void Create(SRS_Source_Out *pOut);
    void Config(SRS_Source_Out *pOut, SRS_Tech_AEQ *pCFG, bool bBypass);
    void Process_256(SRSSamp *pData);
};

/** \brief Inverse headphone impulse response coefficients usable in HPX*/
typedef struct _dtsHpcHpxHpeqData {
    SRSInt8   uuid[16];         /**< UID */
    SRSInt32  nSamplingRate;    /**< Sampling rate */
    SRSInt32  nIhpirLength;     /**< Length of inverse headphone impulse response per channel */
    /* Scale factors and coefficients go here as in file */

} _dtsHpcHpxHpeqData;

/* Headphone 0: Name=Sennheiser HD23x UUID=2bde6750-d4d6-436e-8701-b7d42a24a7eb Bytes=540 */
const static unsigned short headphone_0x2bde6750_48k[270] = {
    0xde2b, 0x5067, 0xd6d4, 0x6e43, 0x0187, 0xd4b7, 0x242a, 0xeba7, 0xbb80, 0x0000, 0x0080, 0x0000, 0x000e, 0x000e, 0x5bb2, 0xca9c,
    0x4546, 0xdc48, 0x1308, 0xfc6f, 0xfdcc, 0x06b8, 0xf90c, 0x02a2, 0xfb24, 0xfdd9, 0xfdad, 0xfb60, 0xfe38, 0xfb35, 0xfdc6, 0xfc7e,
    0xfd6d, 0xfe63, 0xfe06, 0x000c, 0xff59, 0x00d7, 0x0099, 0x00c1, 0x0112, 0x0032, 0x00a9, 0xffa8, 0xffd7, 0xff6c, 0xff33, 0xff78,
    0xff0b, 0xff9b, 0xff54, 0xffb5, 0xffcb, 0xffcb, 0x002b, 0xffee, 0x0051, 0x0020, 0x0046, 0x004e, 0x002b, 0x0060, 0x001d, 0x004e,
    0x0022, 0x0028, 0x002f, 0x0007, 0x0032, 0x0002, 0x002a, 0x0019, 0x0021, 0x003a, 0x0023, 0x0051, 0x0035, 0x0054, 0x004e, 0x004b,
    0x005e, 0x0041, 0x005c, 0x003e, 0x004a, 0x0042, 0x0035, 0x0045, 0x002a, 0x0041, 0x002e, 0x0038, 0x003b, 0x0032, 0x0047, 0x0035,
    0x004b, 0x0041, 0x0047, 0x004e, 0x0044, 0x0056, 0x0045, 0x0054, 0x004c, 0x004d, 0x0054, 0x0048, 0x0057, 0x0049, 0x0054, 0x0050,
    0x004f, 0x0058, 0x004d, 0x005b, 0x0050, 0x0058, 0x0057, 0x0054, 0x005d, 0x0051, 0x005d, 0x0054, 0x0058, 0x0058, 0x0052, 0x005b,
    0x0050, 0x0059, 0x0053, 0x0055, 0x0057, 0x0051, 0x005a, 0x0051, 0x0059, 0x0055, 0x0056, 0x005a, 0x0054, 0x005c, 0x5bb2, 0xca9c,
    0x4546, 0xdc48, 0x1308, 0xfc6f, 0xfdcc, 0x06b8, 0xf90c, 0x02a2, 0xfb24, 0xfdd9, 0xfdad, 0xfb60, 0xfe38, 0xfb35, 0xfdc6, 0xfc7e,
    0xfd6d, 0xfe63, 0xfe06, 0x000c, 0xff59, 0x00d7, 0x0099, 0x00c1, 0x0112, 0x0032, 0x00a9, 0xffa8, 0xffd7, 0xff6c, 0xff33, 0xff78,
    0xff0b, 0xff9b, 0xff54, 0xffb5, 0xffcb, 0xffcb, 0x002b, 0xffee, 0x0051, 0x0020, 0x0046, 0x004e, 0x002b, 0x0060, 0x001d, 0x004e,
    0x0022, 0x0028, 0x002f, 0x0007, 0x0032, 0x0002, 0x002a, 0x0019, 0x0021, 0x003a, 0x0023, 0x0051, 0x0035, 0x0054, 0x004e, 0x004b,
    0x005e, 0x0041, 0x005c, 0x003e, 0x004a, 0x0042, 0x0035, 0x0045, 0x002a, 0x0041, 0x002e, 0x0038, 0x003b, 0x0032, 0x0047, 0x0035,
    0x004b, 0x0041, 0x0047, 0x004e, 0x0044, 0x0056, 0x0045, 0x0054, 0x004c, 0x004d, 0x0054, 0x0048, 0x0057, 0x0049, 0x0054, 0x0050,
    0x004f, 0x0058, 0x004d, 0x005b, 0x0050, 0x0058, 0x0057, 0x0054, 0x005d, 0x0051, 0x005d, 0x0054, 0x0058, 0x0058, 0x0052, 0x005b,
    0x0050, 0x0059, 0x0053, 0x0055, 0x0057, 0x0051, 0x005a, 0x0051, 0x0059, 0x0055, 0x0056, 0x005a, 0x0054, 0x005c,
};

static unsigned short *param_data = NULL;

SRS_Source_AEQ::SRS_Source_AEQ(){
    SRS_LOG("AEQ Created");

    DidCreate = false;
    DidConfig = false;

    int tBufRAM = 0;

    tBufRAM = SRS_AEQ_GetObjSize(&aeqAttr);

    pPBufferL = malloc(tBufRAM);
    pPBufferR = malloc(tBufRAM);

#ifdef _SRSCFG_PERFTRACK
    SRS_LOG("[SRS_RAMSIZE] AEQ: %d", (tBufRAM*2)+sizeof(SRS_Source_AEQ));
#endif  //_SRSCFG_PERFTRACK
}

SRS_Source_AEQ::~SRS_Source_AEQ(){

    if (pPBufferL != NULL) free(pPBufferL);
    pPBufferL = NULL;
    if (pPBufferR != NULL) free(pPBufferR);
    pPBufferR = NULL;

    DidCreate = false;
    DidConfig = false;

    SRS_LOG("AEQ Destroyed");
}

void SRS_Source_AEQ::Create(SRS_Source_Out *pOut){
    FILE *fp = NULL;
    int numRead = 0;

    if (pOut->SampleRate <= 0) return;
    if (pOut->ChannelCount != 2) return;

    SampleRate = pOut->SampleRate;
    ChannelCount = pOut->ChannelCount;

    DidCreate = true;
    ForceActive = true;

    SRS_AEQ_CreateObj(&pObjL, pPBufferL, NULL, NULL, &aeqAttr);
    SRS_AEQ_CreateObj(&pObjR, pPBufferR, NULL, NULL, &aeqAttr);

    memset(FIRCoeff, 0, sizeof(FIRCoeff));
    FIRCoeffValid = false;
#ifdef SRS_AEQ_COEFF_FROM_FILE
    // Read the default coeff file from the file path
    SRS_LOG("Opening coeff file for read %s", SRS_STR(SRS_AEQ_FIR_COEFF_FILE_PATH));
    fp = fopen(SRS_STR(SRS_AEQ_FIR_COEFF_FILE_PATH), "r");
    if(fp != NULL) {
        int i = 0;
        do{
            numRead = fscanf(fp, "%04x", &FIRCoeff[i]);
            SRS_LOG("Read %d, %d, %x", numRead, i, FIRCoeff[i]);
            i++;
        } while (numRead > 0 && i <= 2048);
        fclose(fp);
        FIRCoeffValid = true;
    } else {
        SRS_LOG("Unable to open Coeff file");
    }
#endif

    DidConfig = false;
}

void SRS_Source_AEQ::Config(SRS_Source_Out *pOut, SRS_Tech_AEQ *pCFG, bool bBypass){
    (void) (pOut);

    if (DidCreate == false) return;

    int i;
    int numBandsActive = 0;

    // Set headphone coefficients data
    _dtsHpcHpxHpeqData *hpcHpxHpeqData;
    SRSInt8 *hpeqData;

    if (param_data)
    {
        SRS_LOG("Using FIR coefficients from set param");
        // Use filter coefficients sent via set param
        hpcHpxHpeqData = (_dtsHpcHpxHpeqData*) &param_data[1];
        hpeqData = (((SRSInt8*)(&param_data[1])) + sizeof(_dtsHpcHpxHpeqData));
    } else
#ifdef SRS_AEQ_COEFF_FROM_FILE
    if((pCFG->FIRCoeffFileSelect == true) && (FIRCoeffValid == true))
    {
        SRS_LOG("Using FIR coefficients read from file");
        // Use filter coefficients read from file
        hpcHpxHpeqData = (_dtsHpcHpxHpeqData*) FIRCoeff;
        hpeqData = ((SRSInt8*)FIRCoeff + sizeof(_dtsHpcHpxHpeqData));
    } else
#endif
    {
        SRS_LOG("Using FIR inbuilt coefficients");
        // Use inbuilt filter coefficients
        hpcHpxHpeqData = (_dtsHpcHpxHpeqData*) headphone_0x2bde6750_48k;
        hpeqData = ((SRSInt8*)headphone_0x2bde6750_48k + sizeof(_dtsHpcHpxHpeqData));
    }

    // Check if link LR enabled and copy the left config to right
    if(pCFG->LRLinkEnable == true)
    {
        for (i=0; i<MAX_NUM_AEQ_BANDS; i++){

            pCFG->Params[i+MAX_NUM_AEQ_BANDS][0] = pCFG->Params[i][0];
            pCFG->Params[i+MAX_NUM_AEQ_BANDS][1] = pCFG->Params[i][1];
            pCFG->Params[i+MAX_NUM_AEQ_BANDS][2] = pCFG->Params[i][2];
            pCFG->RBands[i] = pCFG->LBands[i];
            pCFG->BandType[i+MAX_NUM_AEQ_BANDS] = pCFG->BandType[i];
            pCFG->REnable = pCFG->LEnable;
        }
    }

    // Aeq
    bool bCalcL = false;
    for (i=0; i<MAX_NUM_AEQ_BANDS; i++){
        if (DIFF_FORCED(Params[i][0]) || DIFF_FORCED(Params[i][1]) || DIFF_FORCED(Params[i][2]) || DIFF_FORCED(BandType) || DIFF_FORCED(LBands[i])){
            bCalcL = true;
        }
    }

    bool bCalcR = false;
    for (i=0; i<MAX_NUM_AEQ_BANDS; i++){
        if (DIFF_FORCED(Params[i+MAX_NUM_AEQ_BANDS][0]) || DIFF_FORCED(Params[i+MAX_NUM_AEQ_BANDS][1]) || DIFF_FORCED(Params[i+MAX_NUM_AEQ_BANDS][2]) || DIFF_FORCED(BandType) || DIFF_FORCED(RBands[i])){
            bCalcR = true;
        }
    }

    bool bMirrorCoefs = false;
    if (bCalcL || bCalcR){
        bool bSame = true;

        for (i=0; i<MAX_NUM_AEQ_BANDS; i++){
            if (pCFG->Params[i][0] != pCFG->Params[i+MAX_NUM_AEQ_BANDS][0]) bSame = false;
            if (pCFG->Params[i][1] != pCFG->Params[i+MAX_NUM_AEQ_BANDS][1]) bSame = false;
            if (pCFG->Params[i][2] != pCFG->Params[i+MAX_NUM_AEQ_BANDS][2]) bSame = false;
            if (pCFG->BandType[i] != pCFG->BandType[i+MAX_NUM_AEQ_BANDS]) bSame = false;
            if (pCFG->LBands[i] != pCFG->RBands[i]) bSame = false;
        }

        if (bSame == true){
            bMirrorCoefs = true;
            bCalcL = true;
            bCalcR = false;
        }
    }

    if (bCalcL){

        SRSParametriceqBandSpec tBands[MAX_NUM_AEQ_BANDS];
        SRSParametriceqSpec tSpec;

        // Design the filter bands, only if it is enabled and the center frequency is less than the samplerate/2
        numBandsActive = 0;
        for (i=0; i<MAX_NUM_AEQ_BANDS; i++){
            if(pCFG->LBands[i] == true){                            // Band enabled
                if(pCFG->Params[i][0] <= (SampleRate*0.5f)){        // Center frequency is within nyquist rate
                    tBands[numBandsActive].Type = (SRSParametriceqBandType) pCFG->BandType[i];
                    tBands[numBandsActive].CenterFreq = pCFG->Params[i][0];
                    tBands[numBandsActive].Gain = pCFG->Params[i][1];
                    tBands[numBandsActive].QFactor = pCFG->Params[i][2];
                    tBands[numBandsActive].InvertPhase = 0;

                    if(tBands[numBandsActive].CenterFreq < 5.0f) tBands[numBandsActive].CenterFreq = 5.0f;
                    // if(tBands[numBandsActive].CenterFreq > (SampleRate*0.5f)) tBands[i].CenterFreq = SampleRate*0.5f; (This doesnt make sense here )

                    if(tBands[numBandsActive].QFactor < 0.1f) tBands[numBandsActive].QFactor = 0.1f;
                    if(tBands[numBandsActive].QFactor > 10.0f) tBands[numBandsActive].QFactor = 10.0f;

                    // if(pCFG->LBands[i] == false) tBands[i].Gain = 0.0f;    // Disabled means disabled... (This doesnt make sense here )
                    numBandsActive++;
                }
            }
        }

        tSpec.SampleRate = SampleRate;
        tSpec.BandSpecs  = tBands;
        tSpec.NumOfBands = numBandsActive;
#if defined (_SRSCFG_ARCH_ARM) || defined (_SRSCFG_ARCH_ARM64)
        char tWork[SRS_PEQ_DESIGN_WORKSPACE_SIZE(MAX_NUM_AEQ_BANDS)];
        SRS_ParametriceqDesignFxp32(&tSpec, CoefL, tWork); //Design Aeq filters
#endif // _SRSCFG_ARCH_ARM

#ifdef _SRSCFG_ARCH_X86
        SRS_ParametriceqDesignFloat(&tSpec, CoefL); //Design Aeq filters
#endif // _SRSCFG_ARCH_X86
        SRS_LOG("Number of Left AEQ bands %d", numBandsActive);

        SRSAeqFiltersConfig filterCfg;
        filterCfg.FilterOrders.IirOrder = numBandsActive * 2;
        filterCfg.IirCoefs = (SRSInt32 *)CoefL;
#if defined (_SRSCFG_ARCH_ARM) || defined (_SRSCFG_ARCH_ARM64)
        if(pCFG->FIREnable == true) {
            filterCfg.FilterOrders.FirOrder = hpcHpxHpeqData->nIhpirLength - 1;
            filterCfg.FirScaleFactor = *(SRSInt16*)hpeqData;
            filterCfg.FirCoefs = (SRSInt16*)(hpeqData + 2*sizeof(SRSInt16));
        } else {
            filterCfg.FilterOrders.FirOrder = 0;
            filterCfg.FirScaleFactor = 0;
            filterCfg.FirCoefs = NULL;
        }
#endif // _SRSCFG_ARCH_ARM

#ifdef _SRSCFG_ARCH_X86
        if(pCFG->FIREnable == true) {
            filterCfg.FilterOrders.FirOrder = hpcHpxHpeqData->nIhpirLength - 1;
            filterCfg.FirCoefs = (SRSFloat32*)(hpeqData + 2*sizeof(SRSFloat32));
        } else {
            filterCfg.FilterOrders.FirOrder = 0;
            filterCfg.FirCoefs = NULL;
        }
#endif // _SRSCFG_ARCH_X86
        if ((pCFG->FIREnable == true) && (hpcHpxHpeqData->nIhpirLength > 1)) {
            int FftPlanSize = SRS_AEQ_GetFftLength(hpcHpxHpeqData->nIhpirLength - 1);
#if defined (_SRSCFG_ARCH_ARM) || defined (_SRSCFG_ARCH_ARM64)
            if ((!SRS_Fft_IsPlanCreated(FftPlanSize, SRS_RFFT_32C16)) && (pFftPlanBuf == NULL)) {
                pFftPlanBuf = malloc(SRS_Fft_GetPlanBufSize(FftPlanSize));
                SRS_Fft_CreatePlan(pFftPlanBuf, FftPlanSize, SRS_RFFT_32C16);
            }
#endif// _SRSCFG_ARCH_ARM
#ifdef _SRSCFG_ARCH_X86
if ((!SRS_Fft_IsPlanCreated(FftPlanSize, SRS_RFFT)) && (pFftPlanBuf == NULL)) {
                pFftPlanBuf = malloc(SRS_Fft_GetPlanBufSize(FftPlanSize));
                SRS_Fft_CreatePlan(pFftPlanBuf, FftPlanSize, SRS_RFFT);
            }
#endif //_SRSCFG_ARCH_X86
        }
        SRS_AEQ_InitObj(pObjL, &filterCfg);

        ForceActive = true;
    }

    if (DIFF_FORCED(LEnable)){
        SRS_LOG("AEQ left enable value %d", pCFG->LEnable);
        SRS_AEQ_SetIirEnable(pObjL, pCFG->LEnable);
    }

    if (bMirrorCoefs){
        SRS_LOG("Number of Right AEQ bands (same as left) %d", numBandsActive);

        SRSAeqFiltersConfig filterCfg;
        filterCfg.FilterOrders.IirOrder = numBandsActive * 2;
        filterCfg.IirCoefs = (SRSInt32 *)CoefL;
#if defined (_SRSCFG_ARCH_ARM) || defined (_SRSCFG_ARCH_ARM64)
        if(pCFG->FIREnable == true) {
            filterCfg.FilterOrders.FirOrder = hpcHpxHpeqData->nIhpirLength - 1;
            filterCfg.FirScaleFactor = *(SRSInt16*)(hpeqData + sizeof(SRSInt16));
            filterCfg.FirCoefs = (SRSInt16*)(hpeqData + 2*sizeof(SRSInt16) + hpcHpxHpeqData->nIhpirLength*sizeof(SRSInt16));
        } else {
            filterCfg.FilterOrders.FirOrder = 0;
            filterCfg.FirScaleFactor = 0;
            filterCfg.FirCoefs = NULL;
        }
#endif // _SRSCFG_ARCH_ARM

#ifdef _SRSCFG_ARCH_X86
        if(pCFG->FIREnable == true) {
            filterCfg.FilterOrders.FirOrder = hpcHpxHpeqData->nIhpirLength - 1;
            filterCfg.FirCoefs = (SRSFloat32*)(hpeqData + 2*sizeof(SRSFloat32) + hpcHpxHpeqData->nIhpirLength*sizeof(SRSFloat32));
        } else {
            filterCfg.FilterOrders.FirOrder = 0;
            filterCfg.FirCoefs = NULL;
        }
#endif // _SRSCFG_ARCH_X86
        if ((pCFG->FIREnable == true) && (hpcHpxHpeqData->nIhpirLength > 1)) {
            int FftPlanSize = SRS_AEQ_GetFftLength(hpcHpxHpeqData->nIhpirLength - 1);
#if defined (_SRSCFG_ARCH_ARM) || defined (_SRSCFG_ARCH_ARM64)
            if ((!SRS_Fft_IsPlanCreated(FftPlanSize, SRS_RFFT_32C16)) && (pFftPlanBuf == NULL)) {
                pFftPlanBuf = malloc(SRS_Fft_GetPlanBufSize(FftPlanSize));
                SRS_Fft_CreatePlan(pFftPlanBuf, FftPlanSize, SRS_RFFT_32C16);
            }
#endif // _SRSCFG_ARCH_ARM
#ifdef _SRSCFG_ARCH_X86
            if ((!SRS_Fft_IsPlanCreated(FftPlanSize, SRS_RFFT)) && (pFftPlanBuf == NULL)) {
                pFftPlanBuf = malloc(SRS_Fft_GetPlanBufSize(FftPlanSize));
                SRS_Fft_CreatePlan(pFftPlanBuf, FftPlanSize, SRS_RFFT);
            }
#endif // _SRSCFG_ARCH_X86
        }
        SRS_AEQ_InitObj(pObjR, &filterCfg);
    }

    if (bCalcR){

        SRSParametriceqBandSpec tBands[MAX_NUM_AEQ_BANDS];
        SRSParametriceqSpec tSpec;

        // Design the filter bands, only if it is enabled and the center frequency is less than the samplerate/2
        numBandsActive = 0;
        for (i=0; i<MAX_NUM_AEQ_BANDS; i++){
            if(pCFG->RBands[i] == true){                            // Band enabled
                if(pCFG->Params[i+MAX_NUM_AEQ_BANDS][0] <= (SampleRate*0.5f)){      // Center frequency is within nyquist rate
                    tBands[numBandsActive].Type = (SRSParametriceqBandType) pCFG->BandType[i+MAX_NUM_AEQ_BANDS];
                    tBands[numBandsActive].CenterFreq = pCFG->Params[i+MAX_NUM_AEQ_BANDS][0];
                    tBands[numBandsActive].Gain = pCFG->Params[i+MAX_NUM_AEQ_BANDS][1];
                    tBands[numBandsActive].QFactor = pCFG->Params[i+MAX_NUM_AEQ_BANDS][2];
                    tBands[numBandsActive].InvertPhase = 0;

                    if(tBands[numBandsActive].CenterFreq < 5.0f) tBands[numBandsActive].CenterFreq = 5.0f;
                    // if(tBands[numBandsActive].CenterFreq > (SampleRate*0.5f)) tBands[i].CenterFreq = SampleRate*0.5f; (This doesnt make sense here )

                    if(tBands[numBandsActive].QFactor < 0.1f) tBands[numBandsActive].QFactor = 0.1f;
                    if(tBands[numBandsActive].QFactor > 10.0f) tBands[numBandsActive].QFactor = 10.0f;

                    // if(pCFG->RBands[i] == false) tBands[i].Gain = 0.0f;    // Disabled means disabled... (This doesnt make sense here )
                    numBandsActive++;
                }
            }
        }

        tSpec.SampleRate = SampleRate;
        tSpec.BandSpecs = tBands;
        tSpec.NumOfBands = numBandsActive;

#if defined (_SRSCFG_ARCH_ARM) || defined (_SRSCFG_ARCH_ARM64)
        char tWork[SRS_PEQ_DESIGN_WORKSPACE_SIZE(MAX_NUM_AEQ_BANDS)];
        SRS_ParametriceqDesignFxp32(&tSpec, CoefR, tWork); //Design Aeq filters
#endif // _SRSCFG_ARCH_ARM
#ifdef _SRSCFG_ARCH_X86
        SRS_ParametriceqDesignFloat(&tSpec, CoefR); //Design Aeq filters
#endif // _SRSCFG_ARCH_X86

        SRS_LOG("Number of Right AEQ bands %d", numBandsActive);

        SRSAeqFiltersConfig filterCfg;
        filterCfg.FilterOrders.IirOrder = numBandsActive * 2;
        filterCfg.IirCoefs = (SRSInt32 *)CoefR;
#if defined (_SRSCFG_ARCH_ARM) || defined (_SRSCFG_ARCH_ARM64)
        if(pCFG->FIREnable == true) {
            filterCfg.FilterOrders.FirOrder = hpcHpxHpeqData->nIhpirLength - 1;
            filterCfg.FirScaleFactor = *(SRSInt16*)(hpeqData + sizeof(SRSInt16));
            filterCfg.FirCoefs = (SRSInt16*)(hpeqData + 2*sizeof(SRSInt16) + hpcHpxHpeqData->nIhpirLength*sizeof(SRSInt16));
        } else {
            filterCfg.FilterOrders.FirOrder = 0;
            filterCfg.FirScaleFactor = 0;
            filterCfg.FirCoefs = NULL;
        }
#endif // _SRSCFG_ARCH_ARM
#ifdef _SRSCFG_ARCH_X86
        if(pCFG->FIREnable == true) {
            filterCfg.FilterOrders.FirOrder = hpcHpxHpeqData->nIhpirLength - 1;
            filterCfg.FirCoefs = (SRSFloat32*)(hpeqData + 2*sizeof(SRSFloat32) + hpcHpxHpeqData->nIhpirLength*sizeof(SRSFloat32));
        } else {
            filterCfg.FilterOrders.FirOrder = 0;
            filterCfg.FirCoefs = NULL;
        }
#endif // _SRSCFG_ARCH_X86
        if ((pCFG->FIREnable == true) && (hpcHpxHpeqData->nIhpirLength > 1)) {
            int FftPlanSize = SRS_AEQ_GetFftLength(hpcHpxHpeqData->nIhpirLength - 1);
#if defined (_SRSCFG_ARCH_ARM) || defined (_SRSCFG_ARCH_ARM64)
            if ((!SRS_Fft_IsPlanCreated(FftPlanSize, SRS_RFFT_32C16)) && (pFftPlanBuf == NULL)) {
                pFftPlanBuf = malloc(SRS_Fft_GetPlanBufSize(FftPlanSize));
                SRS_Fft_CreatePlan(pFftPlanBuf, FftPlanSize, SRS_RFFT_32C16);
            }
#endif // _SRSCFG_ARCH_ARM
#ifdef _SRSCFG_ARCH_X86
            if ((!SRS_Fft_IsPlanCreated(FftPlanSize, SRS_RFFT)) && (pFftPlanBuf == NULL)) {
                pFftPlanBuf = malloc(SRS_Fft_GetPlanBufSize(FftPlanSize));
                SRS_Fft_CreatePlan(pFftPlanBuf, FftPlanSize, SRS_RFFT);
            }
#endif // _SRSCFG_ARCH_X86
        }
        SRS_AEQ_InitObj(pObjR, &filterCfg);
        ForceActive = true;
    }

    if (DIFF_FORCED(REnable)){
        SRS_LOG("AEQ right enable value %d", pCFG->REnable);
        SRS_AEQ_SetIirEnable(pObjR, pCFG->REnable);
    }

    if (DIFF_FORCED(FIREnable)){
        SRS_LOG("AEQ FIR enable value %d", pCFG->FIREnable);
        SRS_AEQ_SetFirEnable(pObjL, pCFG->FIREnable);
        SRS_AEQ_SetFirEnable(pObjR, pCFG->FIREnable);
    }

    if (DIFF_FORCED(IGain)){
        SRS_AEQ_SetInputGain(pObjL, SRS_FXP16(pCFG->IGain, SRS_AEQ_GAIN_IWL));
        SRS_AEQ_SetInputGain(pObjR, SRS_FXP16(pCFG->IGain, SRS_AEQ_GAIN_IWL));
    }
    if (DIFF_FORCED(OGain)){
        SRS_AEQ_SetOutputGain(pObjL, SRS_FXP16(pCFG->OGain, SRS_AEQ_GAIN_IWL));
        SRS_AEQ_SetOutputGain(pObjR, SRS_FXP16(pCFG->OGain, SRS_AEQ_GAIN_IWL));
    }
    if (DIFF_FORCED(BGain)){
        SRS_AEQ_SetBypassGain(pObjL, SRS_FXP16(pCFG->BGain, SRS_AEQ_GAIN_IWL));
        SRS_AEQ_SetBypassGain(pObjR, SRS_FXP16(pCFG->BGain, SRS_AEQ_GAIN_IWL));
    }

    DidConfig = true;
    Active = *pCFG;

    if (bBypass){
        SRS_AEQ_SetEnable(pObjL, false);
        Active.LEnable = false;

        SRS_AEQ_SetEnable(pObjR, false);
        Active.REnable = false;
    } else {
        SRS_AEQ_SetEnable(pObjL, true);
        Active.LEnable = true;

        SRS_AEQ_SetEnable(pObjR, true);
        Active.REnable = true;
    }

    ForceActive = false;

    free(param_data);
    param_data = NULL;
}

void SRS_Source_AEQ::Process_256(SRSSamp *pData){
    SRSSamp tempOut[256];

    SRS_AEQ_Process(pObjL, pData, tempOut, 256, AEQWorkspace);
#if defined (_SRSCFG_ARCH_ARM) || defined (_SRSCFG_ARCH_ARM64)
    SRS_CopyInt32Vector(tempOut, pData, 256);
#endif// _SRSCFG_ARCH_ARM
#ifdef _SRSCFG_ARCH_X86
    SRS_CopyVector(tempOut, pData, 256);
#endif //_SRSCFG_ARCH_X86
    SRS_AEQ_Process(pObjR, pData + 256, tempOut, 256, AEQWorkspace);
#if defined (_SRSCFG_ARCH_ARM) || defined (_SRSCFG_ARCH_ARM64)
    SRS_CopyInt32Vector(tempOut, pData + 256, 256);
#endif// _SRSCFG_ARCH_ARM
#ifdef _SRSCFG_ARCH_X86
    SRS_CopyVector(tempOut, pData + 256, 256);
#endif //_SRSCFG_ARCH_X86
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// =-=-=-=-=-=- External Interfacing =-=-=-=-=-=-
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

SRS_Source_AEQ* SRS_Create_AEQ(SRS_Source_Out* pOut){
    SRS_Source_AEQ* pSrc = new SRS_Source_AEQ();
    pSrc->Create(pOut);
    return pSrc;
}

void SRS_Destroy_AEQ(SRS_Source_AEQ* pSrc, SRS_Source_Out* pOut){
    (void) (pOut);
    if (pSrc == NULL) return;
    delete pSrc;
}

void SRS_Config_AEQ(SRS_Source_AEQ* pSrc, SRS_Source_Out* pOut, SRS_Tech_AEQ* pCFG, bool bBypass){
    if (pSrc == NULL) return;
    pSrc->Config(pOut, pCFG, bBypass);
}

void SRS_Process_AEQ_256(SRS_Source_AEQ* pSrc, SRSSamp* pData){
    if (pSrc == NULL) return;
    if (pSrc->DidConfig == false) return;

    pSrc->Process_256(pData);
}

enum {
    AEQ_IGain,
    AEQ_OGain,
    AEQ_BGain,
    AEQ_LRLINK_Enable,
    AEQ_L_Enable,
    AEQ_R_Enable,
    AEQ_LB0,
    AEQ_LB1,
    AEQ_LB2,
    AEQ_LB3,
    AEQ_LB4,
    AEQ_LB5,
    AEQ_LB6,
    AEQ_LB7,
    AEQ_LB8,
    AEQ_LB9,
    AEQ_LB10,
    AEQ_LB11,
    AEQ_RB0,
    AEQ_RB1,
    AEQ_RB2,
    AEQ_RB3,
    AEQ_RB4,
    AEQ_RB5,
    AEQ_RB6,
    AEQ_RB7,
    AEQ_RB8,
    AEQ_RB9,
    AEQ_RB10,
    AEQ_RB11,
    AEQ_Def_LBand0,
    AEQ_Def_LBand1,
    AEQ_Def_LBand2,
    AEQ_Def_LBand3,
    AEQ_Def_LBand4,
    AEQ_Def_LBand5,
    AEQ_Def_LBand6,
    AEQ_Def_LBand7,
    AEQ_Def_LBand8,
    AEQ_Def_LBand9,
    AEQ_Def_LBand10,
    AEQ_Def_LBand11,
    AEQ_Def_RBand0,
    AEQ_Def_RBand1,
    AEQ_Def_RBand2,
    AEQ_Def_RBand3,
    AEQ_Def_RBand4,
    AEQ_Def_RBand5,
    AEQ_Def_RBand6,
    AEQ_Def_RBand7,
    AEQ_Def_RBand8,
    AEQ_Def_RBand9,
    AEQ_Def_RBand10,
    AEQ_Def_RBand11,
    AEQ_Def_LBand0_Type,
    AEQ_Def_LBand1_Type,
    AEQ_Def_LBand2_Type,
    AEQ_Def_LBand3_Type,
    AEQ_Def_LBand4_Type,
    AEQ_Def_LBand5_Type,
    AEQ_Def_LBand6_Type,
    AEQ_Def_LBand7_Type,
    AEQ_Def_LBand8_Type,
    AEQ_Def_LBand9_Type,
    AEQ_Def_LBand10_Type,
    AEQ_Def_LBand11_Type,
    AEQ_Def_RBand0_Type,
    AEQ_Def_RBand1_Type,
    AEQ_Def_RBand2_Type,
    AEQ_Def_RBand3_Type,
    AEQ_Def_RBand4_Type,
    AEQ_Def_RBand5_Type,
    AEQ_Def_RBand6_Type,
    AEQ_Def_RBand7_Type,
    AEQ_Def_RBand8_Type,
    AEQ_Def_RBand9_Type,
    AEQ_Def_RBand10_Type,
    AEQ_Def_RBand11_Type,
    AEQ_FIR_Enable,

    AEQ_FIR_CoeffFile_Path,
    AEQ_FIR_CoeffFile_Select,
    AEQ_FIR_Blob,

    AEQ_Skip,
};

SRS_Param gAEQ_Params[] = {
    { AEQ_IGain,        SRS_PTYP_CFG,   SRS_PFMT_FLOAT,     1.0f,   0.0f,   1.0f,   "aeq_igain", "AEQ Input Gain", "gain", "", 0},
    { AEQ_OGain,        SRS_PTYP_CFG,   SRS_PFMT_FLOAT,     1.0f,   0.0f,   4.0f,   "aeq_ogain", "AEQ Output Gain", "gain", "", 0},
    { AEQ_BGain,        SRS_PTYP_CFG,   SRS_PFMT_FLOAT,     1.0f,   0.0f,   4.0f,   "aeq_bgain", "AEQ Bypass Gain", "gain", "", 0},

    { AEQ_LRLINK_Enable,  SRS_PTYP_CFG,   SRS_PFMT_BOOL,    0.0f,   0.0f,   1.0f,   "aeq_lrlink_enable", "AEQ LR Link Enable", "toggle", "", 0},

    { AEQ_L_Enable,     SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_left_enable", "AEQ Left Toggle", "toggle", "", 0},
    { AEQ_LB0,          SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_lband0_enable", "AEQ Left Band 0 Toggle", "toggle", "", 0},
    { AEQ_LB1,          SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_lband1_enable", "AEQ Left Band 1 Toggle", "toggle", "", 0},
    { AEQ_LB2,          SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_lband2_enable", "AEQ Left Band 2 Toggle", "toggle", "", 0},
    { AEQ_LB3,          SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_lband3_enable", "AEQ Left Band 3 Toggle", "toggle", "", 0},
    { AEQ_LB4,          SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_lband4_enable", "AEQ Left Band 4 Toggle", "toggle", "", 0},
    { AEQ_LB5,          SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_lband5_enable", "AEQ Left Band 5 Toggle", "toggle", "", 0},
    { AEQ_LB6,          SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_lband6_enable", "AEQ Left Band 6 Toggle", "toggle", "", 0},
    { AEQ_LB7,          SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_lband7_enable", "AEQ Left Band 7 Toggle", "toggle", "", 0},
    { AEQ_LB8,          SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_lband8_enable", "AEQ Left Band 8 Toggle", "toggle", "", 0},
    { AEQ_LB9,          SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_lband9_enable", "AEQ Left Band 9 Toggle", "toggle", "", 0},
    { AEQ_LB10,         SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_lband10_enable", "AEQ Left Band 10 Toggle", "toggle", "", 0},
    { AEQ_LB11,         SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_lband11_enable", "AEQ Left Band 11 Toggle", "toggle", "", 0},

    { AEQ_R_Enable,     SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_right_enable", "AEQ Right Toggle", "toggle", "", 0},
    { AEQ_RB0,          SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_rband0_enable", "AEQ Right Band 0 Toggle", "toggle", "", 0},
    { AEQ_RB1,          SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_rband1_enable", "AEQ Right Band 1 Toggle", "toggle", "", 0},
    { AEQ_RB2,          SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_rband2_enable", "AEQ Right Band 2 Toggle", "toggle", "", 0},
    { AEQ_RB3,          SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_rband3_enable", "AEQ Right Band 3 Toggle", "toggle", "", 0},
    { AEQ_RB4,          SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_rband4_enable", "AEQ Right Band 4 Toggle", "toggle", "", 0},
    { AEQ_RB5,          SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_rband5_enable", "AEQ Right Band 5 Toggle", "toggle", "", 0},
    { AEQ_RB6,          SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_rband6_enable", "AEQ Right Band 6 Toggle", "toggle", "", 0},
    { AEQ_RB7,          SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_rband7_enable", "AEQ Right Band 7 Toggle", "toggle", "", 0},
    { AEQ_RB8,          SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_rband8_enable", "AEQ Right Band 8 Toggle", "toggle", "", 0},
    { AEQ_RB9,          SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_rband9_enable", "AEQ Right Band 9 Toggle", "toggle", "", 0},
    { AEQ_RB10,         SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_rband10_enable", "AEQ Right Band 10 Toggle", "toggle", "", 0},
    { AEQ_RB11,         SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_rband11_enable", "AEQ Right Band 11 Toggle", "toggle", "", 0},

    { AEQ_Def_LBand0,   SRS_PTYP_CFG,   SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "aeq_lband0", "AEQ Left Band 0 Tuning (CF Gain Q)", "", "3", 0},
    { AEQ_Def_LBand1,   SRS_PTYP_CFG,   SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "aeq_lband1", "AEQ Left Band 1 Tuning", "", "3", 0},
    { AEQ_Def_LBand2,   SRS_PTYP_CFG,   SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "aeq_lband2", "AEQ Left Band 2 Tuning", "", "3", 0},
    { AEQ_Def_LBand3,   SRS_PTYP_CFG,   SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "aeq_lband3", "AEQ Left Band 3 Tuning", "", "3", 0},
    { AEQ_Def_LBand4,   SRS_PTYP_CFG,   SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "aeq_lband4", "AEQ Left Band 4 Tuning", "", "3", 0},
    { AEQ_Def_LBand5,   SRS_PTYP_CFG,   SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "aeq_lband5", "AEQ Left Band 5 Tuning", "", "3", 0},
    { AEQ_Def_LBand6,   SRS_PTYP_CFG,   SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "aeq_lband6", "AEQ Left Band 6 Tuning", "", "3", 0},
    { AEQ_Def_LBand7,   SRS_PTYP_CFG,   SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "aeq_lband7", "AEQ Left Band 7 Tuning", "", "3", 0},
    { AEQ_Def_LBand8,   SRS_PTYP_CFG,   SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "aeq_lband8", "AEQ Left Band 8 Tuning", "", "3", 0},
    { AEQ_Def_LBand9,   SRS_PTYP_CFG,   SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "aeq_lband9", "AEQ Left Band 9 Tuning", "", "3", 0},
    { AEQ_Def_LBand10,  SRS_PTYP_CFG,   SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "aeq_lband10", "AEQ Left Band 10 Tuning", "", "3", 0},
    { AEQ_Def_LBand11,  SRS_PTYP_CFG,   SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "aeq_lband11", "AEQ Left Band 11 Tuning", "", "3", 0},

    { AEQ_Def_RBand0,   SRS_PTYP_CFG,   SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "aeq_rband0", "AEQ Right Band 0 Tuning (CF Gain Q)", "", "3", 0},
    { AEQ_Def_RBand1,   SRS_PTYP_CFG,   SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "aeq_rband1", "AEQ Right Band 1 Tuning", "", "3", 0},
    { AEQ_Def_RBand2,   SRS_PTYP_CFG,   SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "aeq_rband2", "AEQ Right Band 2 Tuning", "", "3", 0},
    { AEQ_Def_RBand3,   SRS_PTYP_CFG,   SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "aeq_rband3", "AEQ Right Band 3 Tuning", "", "3", 0},
    { AEQ_Def_RBand4,   SRS_PTYP_CFG,   SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "aeq_rband4", "AEQ Right Band 4 Tuning", "", "3", 0},
    { AEQ_Def_RBand5,   SRS_PTYP_CFG,   SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "aeq_rband5", "AEQ Right Band 5 Tuning", "", "3", 0},
    { AEQ_Def_RBand6,   SRS_PTYP_CFG,   SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "aeq_rband6", "AEQ Right Band 6 Tuning", "", "3", 0},
    { AEQ_Def_RBand7,   SRS_PTYP_CFG,   SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "aeq_rband7", "AEQ Right Band 7 Tuning", "", "3", 0},
    { AEQ_Def_RBand8,   SRS_PTYP_CFG,   SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "aeq_rband8", "AEQ Right Band 8 Tuning", "", "3", 0},
    { AEQ_Def_RBand9,   SRS_PTYP_CFG,   SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "aeq_rband9", "AEQ Right Band 9 Tuning", "", "3", 0},
    { AEQ_Def_RBand10,  SRS_PTYP_CFG,   SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "aeq_rband10", "AEQ Right Band 10 Tuning", "", "3", 0},
    { AEQ_Def_RBand11,  SRS_PTYP_CFG,   SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "aeq_rband11", "AEQ Right Band 11 Tuning", "", "3", 0},

    { AEQ_Def_LBand0_Type,   SRS_PTYP_CFG,   SRS_PFMT_ENUM,    0.0f,   0.0f,   0.0f,   "aeq_lband0_type", "AEQ Left Band 0 Type", "enum", "Traditional,LowShelf,HighShelf", 0},
    { AEQ_Def_LBand1_Type,   SRS_PTYP_CFG,   SRS_PFMT_ENUM,    0.0f,   0.0f,   0.0f,   "aeq_lband1_type", "AEQ Left Band 1 Type", "enum", "Traditional,LowShelf,HighShelf", 0},
    { AEQ_Def_LBand2_Type,   SRS_PTYP_CFG,   SRS_PFMT_ENUM,    0.0f,   0.0f,   0.0f,   "aeq_lband2_type", "AEQ Left Band 2 Type", "enum", "Traditional,LowShelf,HighShelf", 0},
    { AEQ_Def_LBand3_Type,   SRS_PTYP_CFG,   SRS_PFMT_ENUM,    0.0f,   0.0f,   0.0f,   "aeq_lband3_type", "AEQ Left Band 3 Type", "enum", "Traditional,LowShelf,HighShelf", 0},
    { AEQ_Def_LBand4_Type,   SRS_PTYP_CFG,   SRS_PFMT_ENUM,    0.0f,   0.0f,   0.0f,   "aeq_lband4_type", "AEQ Left Band 4 Type", "enum", "Traditional,LowShelf,HighShelf", 0},
    { AEQ_Def_LBand5_Type,   SRS_PTYP_CFG,   SRS_PFMT_ENUM,    0.0f,   0.0f,   0.0f,   "aeq_lband5_type", "AEQ Left Band 5 Type", "enum", "Traditional,LowShelf,HighShelf", 0},
    { AEQ_Def_LBand6_Type,   SRS_PTYP_CFG,   SRS_PFMT_ENUM,    0.0f,   0.0f,   0.0f,   "aeq_lband6_type", "AEQ Left Band 6 Type", "enum", "Traditional,LowShelf,HighShelf", 0},
    { AEQ_Def_LBand7_Type,   SRS_PTYP_CFG,   SRS_PFMT_ENUM,    0.0f,   0.0f,   0.0f,   "aeq_lband7_type", "AEQ Left Band 7 Type", "enum", "Traditional,LowShelf,HighShelf", 0},
    { AEQ_Def_LBand8_Type,   SRS_PTYP_CFG,   SRS_PFMT_ENUM,    0.0f,   0.0f,   0.0f,   "aeq_lband8_type", "AEQ Left Band 8 Type", "enum", "Traditional,LowShelf,HighShelf", 0},
    { AEQ_Def_LBand9_Type,   SRS_PTYP_CFG,   SRS_PFMT_ENUM,    0.0f,   0.0f,   0.0f,   "aeq_lband9_type", "AEQ Left Band 9 Type", "enum", "Traditional,LowShelf,HighShelf", 0},
    { AEQ_Def_LBand10_Type,  SRS_PTYP_CFG,   SRS_PFMT_ENUM,    0.0f,   0.0f,   0.0f,   "aeq_lband10_type", "AEQ Left Band 10 Type", "enum", "Traditional,LowShelf,HighShelf", 0},
    { AEQ_Def_LBand11_Type,  SRS_PTYP_CFG,   SRS_PFMT_ENUM,    0.0f,   0.0f,   0.0f,   "aeq_lband11_type", "AEQ Left Band 11 Type", "enum", "Traditional,LowShelf,HighShelf", 0},

    { AEQ_Def_RBand0_Type,   SRS_PTYP_CFG,   SRS_PFMT_ENUM,    0.0f,   0.0f,   0.0f,   "aeq_rband0_type", "AEQ Right Band 0 Type", "enum", "Traditional,LowShelf,HighShelf", 0},
    { AEQ_Def_RBand1_Type,   SRS_PTYP_CFG,   SRS_PFMT_ENUM,    0.0f,   0.0f,   0.0f,   "aeq_rband1_type", "AEQ Right Band 1 Type", "enum", "Traditional,LowShelf,HighShelf", 0},
    { AEQ_Def_RBand2_Type,   SRS_PTYP_CFG,   SRS_PFMT_ENUM,    0.0f,   0.0f,   0.0f,   "aeq_rband2_type", "AEQ Right Band 2 Type", "enum", "Traditional,LowShelf,HighShelf", 0},
    { AEQ_Def_RBand3_Type,   SRS_PTYP_CFG,   SRS_PFMT_ENUM,    0.0f,   0.0f,   0.0f,   "aeq_rband3_type", "AEQ Right Band 3 Type", "enum", "Traditional,LowShelf,HighShelf", 0},
    { AEQ_Def_RBand4_Type,   SRS_PTYP_CFG,   SRS_PFMT_ENUM,    0.0f,   0.0f,   0.0f,   "aeq_rband4_type", "AEQ Right Band 4 Type", "enum", "Traditional,LowShelf,HighShelf", 0},
    { AEQ_Def_RBand5_Type,   SRS_PTYP_CFG,   SRS_PFMT_ENUM,    0.0f,   0.0f,   0.0f,   "aeq_rband5_type", "AEQ Right Band 5 Type", "enum", "Traditional,LowShelf,HighShelf", 0},
    { AEQ_Def_RBand6_Type,   SRS_PTYP_CFG,   SRS_PFMT_ENUM,    0.0f,   0.0f,   0.0f,   "aeq_rband6_type", "AEQ Right Band 6 Type", "enum", "Traditional,LowShelf,HighShelf", 0},
    { AEQ_Def_RBand7_Type,   SRS_PTYP_CFG,   SRS_PFMT_ENUM,    0.0f,   0.0f,   0.0f,   "aeq_rband7_type", "AEQ Right Band 7 Type", "enum", "Traditional,LowShelf,HighShelf", 0},
    { AEQ_Def_RBand8_Type,   SRS_PTYP_CFG,   SRS_PFMT_ENUM,    0.0f,   0.0f,   0.0f,   "aeq_rband8_type", "AEQ Right Band 8 Type", "enum", "Traditional,LowShelf,HighShelf", 0},
    { AEQ_Def_RBand9_Type,   SRS_PTYP_CFG,   SRS_PFMT_ENUM,    0.0f,   0.0f,   0.0f,   "aeq_rband9_type", "AEQ Right Band 9 Type", "enum", "Traditional,LowShelf,HighShelf", 0},
    { AEQ_Def_RBand10_Type,  SRS_PTYP_CFG,   SRS_PFMT_ENUM,    0.0f,   0.0f,   0.0f,   "aeq_rband10_type", "AEQ Right Band 10 Type", "enum", "Traditional,LowShelf,HighShelf", 0},
    { AEQ_Def_RBand11_Type,  SRS_PTYP_CFG,   SRS_PFMT_ENUM,    0.0f,   0.0f,   0.0f,   "aeq_rband11_type", "AEQ Right Band 11 Type", "enum", "Traditional,LowShelf,HighShelf", 0},

    { AEQ_FIR_Enable,       SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   0.0f,   "aeq_fir_enable", "AEQ FIR Toggle", "toggle", "", 0},

    { AEQ_FIR_CoeffFile_Select, SRS_PTYP_CFG,   SRS_PFMT_ENUM,      0.0f,   0.0f,   0.0f,   "aeq_fir_coeff_fileselect","AEQ FIR Coef file selection", "toggle", "", 0},
    { AEQ_FIR_Blob,          SRS_PTYP_CFG,   SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "aeq_fir_blob", "AEQ FIR coef block, Base64 encoded, first unsigned 16 bit is size of data", "", "", 0},

    { AEQ_Skip,   SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   1.0f,   "aeq_skip", "Skips AEQ when true", "toggle", "", 0},
};

SRS_Param* SRS_GetBank_AEQ(int& paramCount){
    paramCount = sizeof(gAEQ_Params)/sizeof(SRS_Param);
    return gAEQ_Params;
}

char* SRS_GetVersion_AEQ(char* pWork, size_t maxBytes){
    int v1 = SRS_AEQ_GetTechVersion(SRS_VERSION_MAJOR);
    int v2 = SRS_AEQ_GetTechVersion(SRS_VERSION_MINOR);
    int v3 = SRS_AEQ_GetTechVersion(SRS_VERSION_REVISION);
    int v4 = SRS_AEQ_GetTechVersion(SRS_VERSION_RELEASE);
    int l1 = SRS_AEQ_GetLibVersion(SRS_VERSION_MAJOR);
    int l2 = SRS_AEQ_GetLibVersion(SRS_VERSION_MINOR);
    int l3 = SRS_AEQ_GetLibVersion(SRS_VERSION_REVISION);
    int l4 = SRS_AEQ_GetLibVersion(SRS_VERSION_RELEASE);
    snprintf(pWork, maxBytes, "%d.%d.%d.%d - lib %d.%d.%d.%d", v1, v2, v3, v4, l1, l2, l3, l4);
    return pWork;
}

void SRS_SetParam_AEQ(SRS_Tech_AEQ* pCFG, SRS_Param* pParam, const char* pValue){
    HELP_ParamIn In;

    switch (pParam->EnumID){
        case AEQ_IGain: pCFG->IGain = In.GetFloat(pValue); break;
        case AEQ_OGain: pCFG->OGain = In.GetFloat(pValue); break;
        case AEQ_BGain: pCFG->BGain = In.GetFloat(pValue); break;
        case AEQ_Skip: pCFG->Skip = In.GetBool(pValue); break;

        case AEQ_LRLINK_Enable: pCFG->LRLinkEnable = In.GetInt(pValue); break;
        case AEQ_L_Enable: pCFG->LEnable = In.GetBool(pValue); break;
        case AEQ_R_Enable: pCFG->REnable = In.GetBool(pValue); break;

        case AEQ_LB0:
        case AEQ_LB1:
        case AEQ_LB2:
        case AEQ_LB3:
        case AEQ_LB4:
        case AEQ_LB5:
        case AEQ_LB6:
        case AEQ_LB7:
        case AEQ_LB8:
        case AEQ_LB9:
        case AEQ_LB10:
        case AEQ_LB11: { int tIdx = pParam->EnumID-AEQ_LB0; pCFG->LBands[tIdx] = In.GetBool(pValue); } break;

        case AEQ_RB0:
        case AEQ_RB1:
        case AEQ_RB2:
        case AEQ_RB3:
        case AEQ_RB4:
        case AEQ_RB5:
        case AEQ_RB6:
        case AEQ_RB7:
        case AEQ_RB8:
        case AEQ_RB9:
        case AEQ_RB10:
        case AEQ_RB11: { int tIdx = pParam->EnumID-AEQ_RB0; pCFG->RBands[tIdx] = In.GetBool(pValue); } break;

        case AEQ_Def_LBand0:
        case AEQ_Def_LBand1:
        case AEQ_Def_LBand2:
        case AEQ_Def_LBand3:
        case AEQ_Def_LBand4:
        case AEQ_Def_LBand5:
        case AEQ_Def_LBand6:
        case AEQ_Def_LBand7:
        case AEQ_Def_LBand8:
        case AEQ_Def_LBand9:
        case AEQ_Def_LBand10:
        case AEQ_Def_LBand11:
        case AEQ_Def_RBand0:
        case AEQ_Def_RBand1:
        case AEQ_Def_RBand2:
        case AEQ_Def_RBand3:
        case AEQ_Def_RBand4:
        case AEQ_Def_RBand5:
        case AEQ_Def_RBand6:
        case AEQ_Def_RBand7:
        case AEQ_Def_RBand8:
        case AEQ_Def_RBand9:
        case AEQ_Def_RBand10:
        case AEQ_Def_RBand11:
        {
            int tIdx = pParam->EnumID-AEQ_Def_LBand0;
            float tHold[3];
            float* pD = tHold;
            int i;

            float* pS = pCFG->Params[tIdx];
            for (i=0; i<3; i++) tHold[i] = pS[i];       // Fill with current values
            if (sscanf(pValue, "%f,%f,%f", pD+0, pD+1, pD+2) == 3){
                for (i=0; i<3; i++) pS[i] = tHold[i];
            }
        } break;

        case AEQ_Def_LBand0_Type:
        case AEQ_Def_LBand1_Type:
        case AEQ_Def_LBand2_Type:
        case AEQ_Def_LBand3_Type:
        case AEQ_Def_LBand4_Type:
        case AEQ_Def_LBand5_Type:
        case AEQ_Def_LBand6_Type:
        case AEQ_Def_LBand7_Type:
        case AEQ_Def_LBand8_Type:
        case AEQ_Def_LBand9_Type:
        case AEQ_Def_LBand10_Type:
        case AEQ_Def_LBand11_Type:
        case AEQ_Def_RBand0_Type:
        case AEQ_Def_RBand1_Type:
        case AEQ_Def_RBand2_Type:
        case AEQ_Def_RBand3_Type:
        case AEQ_Def_RBand4_Type:
        case AEQ_Def_RBand5_Type:
        case AEQ_Def_RBand6_Type:
        case AEQ_Def_RBand7_Type:
        case AEQ_Def_RBand8_Type:
        case AEQ_Def_RBand9_Type:
        case AEQ_Def_RBand10_Type:
        case AEQ_Def_RBand11_Type: { int tIdx = pParam->EnumID-AEQ_Def_LBand0_Type; pCFG->BandType[tIdx] = In.GetInt(pValue); } break;

        case AEQ_FIR_Enable: pCFG->FIREnable = In.GetInt(pValue); break;

        case AEQ_FIR_CoeffFile_Select: pCFG->FIRCoeffFileSelect = In.GetBool(pValue); break;

        case AEQ_FIR_Blob: {
            int length = (int)strlen(pValue);
            length = ((length / 4) + ((length & 3) ? 1 : 0)) * 3; // max decode length  is (ceil(x/4))*3
            if (param_data)
                free(param_data);
            param_data = (unsigned short*)malloc(length + 1);
            if (!param_data) {
                SRS_LOG("Memory allocation failed while parsing filter parameter.");
                break;
            }
            base64_decodestate s;
            base64_init_decodestate(&s);
            length = base64_decode_block(pValue, strlen(pValue), (char*)param_data, &s);
            if(length < param_data[0]) {
                SRS_LOG("Error decoding filter data");
                free(param_data);
                param_data = NULL;
            }
            break;
        }
    }
}

const char* SRS_GetParam_AEQ(SRS_Tech_AEQ* pCFG, SRS_Param* pParam){
    HELP_ParamOut Out;

    switch (pParam->EnumID){
        case AEQ_IGain: return Out.FromFloat(pCFG->IGain);
        case AEQ_OGain: return Out.FromFloat(pCFG->OGain);
        case AEQ_BGain: return Out.FromFloat(pCFG->BGain);
        case AEQ_Skip: return Out.FromBool(pCFG->Skip);

        case AEQ_LRLINK_Enable: return Out.FromInt(pCFG->LRLinkEnable);
        case AEQ_L_Enable: return Out.FromBool(pCFG->LEnable);
        case AEQ_R_Enable: return Out.FromBool(pCFG->REnable);

        case AEQ_LB0:
        case AEQ_LB1:
        case AEQ_LB2:
        case AEQ_LB3:
        case AEQ_LB4:
        case AEQ_LB5:
        case AEQ_LB6:
        case AEQ_LB7:
        case AEQ_LB8:
        case AEQ_LB9:
        case AEQ_LB10:
        case AEQ_LB11: { int tIdx = pParam->EnumID-AEQ_LB0; return Out.FromBool(pCFG->LBands[tIdx]); } break;

        case AEQ_RB0:
        case AEQ_RB1:
        case AEQ_RB2:
        case AEQ_RB3:
        case AEQ_RB4:
        case AEQ_RB5:
        case AEQ_RB6:
        case AEQ_RB7:
        case AEQ_RB8:
        case AEQ_RB9:
        case AEQ_RB10:
        case AEQ_RB11: { int tIdx = pParam->EnumID-AEQ_RB0; return Out.FromBool(pCFG->RBands[tIdx]); } break;

        case AEQ_Def_LBand0:
        case AEQ_Def_LBand1:
        case AEQ_Def_LBand2:
        case AEQ_Def_LBand3:
        case AEQ_Def_LBand4:
        case AEQ_Def_LBand5:
        case AEQ_Def_LBand6:
        case AEQ_Def_LBand7:
        case AEQ_Def_LBand8:
        case AEQ_Def_LBand9:
        case AEQ_Def_LBand10:
        case AEQ_Def_LBand11:
        case AEQ_Def_RBand0:
        case AEQ_Def_RBand1:
        case AEQ_Def_RBand2:
        case AEQ_Def_RBand3:
        case AEQ_Def_RBand4:
        case AEQ_Def_RBand5:
        case AEQ_Def_RBand6:
        case AEQ_Def_RBand7:
        case AEQ_Def_RBand8:
        case AEQ_Def_RBand9:
        case AEQ_Def_RBand10:
        case AEQ_Def_RBand11:
        {
            int tIdx = pParam->EnumID-AEQ_Def_LBand0;
            float* pS = pCFG->Params[tIdx];

            snprintf(AEQScratch, sizeof(AEQScratch), "%f,%f,%f", pS[0], pS[1], pS[2]);
            return AEQScratch;
        } break;

        case AEQ_Def_LBand0_Type:
        case AEQ_Def_LBand1_Type:
        case AEQ_Def_LBand2_Type:
        case AEQ_Def_LBand3_Type:
        case AEQ_Def_LBand4_Type:
        case AEQ_Def_LBand5_Type:
        case AEQ_Def_LBand6_Type:
        case AEQ_Def_LBand7_Type:
        case AEQ_Def_LBand8_Type:
        case AEQ_Def_LBand9_Type:
        case AEQ_Def_LBand10_Type:
        case AEQ_Def_LBand11_Type:
        case AEQ_Def_RBand0_Type:
        case AEQ_Def_RBand1_Type:
        case AEQ_Def_RBand2_Type:
        case AEQ_Def_RBand3_Type:
        case AEQ_Def_RBand4_Type:
        case AEQ_Def_RBand5_Type:
        case AEQ_Def_RBand6_Type:
        case AEQ_Def_RBand7_Type:
        case AEQ_Def_RBand8_Type:
        case AEQ_Def_RBand9_Type:
        case AEQ_Def_RBand10_Type:
        case AEQ_Def_RBand11_Type: { int tIdx = pParam->EnumID-AEQ_Def_LBand0_Type; return Out.FromInt(pCFG->BandType[tIdx]); } break;

        case AEQ_FIR_Enable: return Out.FromInt(pCFG->FIREnable);

        case AEQ_FIR_CoeffFile_Select: return Out.FromBool(pCFG->FIRCoeffFileSelect); break;

    }
    return "";
}

void SRS_Default_AEQ(SRS_Tech_AEQ* pCFG){
    pCFG->IGain = 1.0f;
    pCFG->OGain = 1.0f;
    pCFG->BGain = 1.0f;
    pCFG->Skip = false;

    pCFG->LRLinkEnable = false;
    pCFG->LEnable = false;
    pCFG->REnable = false;
    memset(pCFG->LBands, 0, sizeof(pCFG->LBands));
    memset(pCFG->RBands, 0, sizeof(pCFG->RBands));

    memset(pCFG->Params, 0, sizeof(pCFG->Params));
    for (int i=0; i<(2*MAX_NUM_AEQ_BANDS); i++) pCFG->Params[i][2] = 2.0f;

    memset(pCFG->BandType, 0, sizeof(pCFG->BandType));
}

};

