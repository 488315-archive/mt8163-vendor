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

#include SRSLIBINC(srs_geq10b_api.h)
#include SRSLIBINC(srs_math_api.h)
#include SRSLIBINC(srs_limiter_api.h)

int16_t gSRS_GEQGainMap[31] = { 0 };
char    GEQScratch[512];
bool    gSRS_GEQGainMapInit = false;

#undef  LOG_TAG
#define LOG_TAG "SRS_Tech_GEQ"

namespace android {

struct SRS_Source_GEQ {
    SRSGeq10bObj    ObjL;
    SRSGeq10bObj    ObjR;
    int             GEQWorkspace[4096];
    int             SampleRate;
    int             ChannelCount;
    char            *pGEQBufL; //[SRS_10BAND_GRAPHICEQ_OBJ_SIZE]; //buffer for creating left PEQ core
    char            *pGEQBufR; //[SRS_10BAND_GRAPHICEQ_OBJ_SIZE]; //buffer for creating right PEQ core
    bool            DidCreate;
    bool            DidConfig;

    SRS_Source_GEQ();
    ~SRS_Source_GEQ();

    void Create(SRS_Source_Out *pOut);
    void Config(SRS_Source_Out *pOut, SRS_Tech_GEQ *pCFG, bool bLimit);

#if defined (_SRSCFG_ARCH_ARM) || defined (_SRSCFG_ARCH_ARM64)
    void Process_256(int32_t *pData);
#endif // _SRSCFG_ARCH_ARM

#ifdef _SRSCFG_ARCH_X86
    void Process_256(SRSFloat32 *pData);
#endif // _SRSCFG_ARCH_X86
};

SRS_Source_GEQ::SRS_Source_GEQ(){
    SRS_LOG("GEQ Created");
    DidCreate = false;

    if (gSRS_GEQGainMapInit == false){
        gSRS_GEQGainMap[0] = SRS_GEQ_MINUS_15DB;
        gSRS_GEQGainMap[1] = SRS_GEQ_MINUS_14DB;
        gSRS_GEQGainMap[2] = SRS_GEQ_MINUS_13DB;
        gSRS_GEQGainMap[3] = SRS_GEQ_MINUS_12DB;
        gSRS_GEQGainMap[4] = SRS_GEQ_MINUS_11DB;
        gSRS_GEQGainMap[5] = SRS_GEQ_MINUS_10DB;
        gSRS_GEQGainMap[6] = SRS_GEQ_MINUS_09DB;
        gSRS_GEQGainMap[7] = SRS_GEQ_MINUS_08DB;
        gSRS_GEQGainMap[8] = SRS_GEQ_MINUS_07DB;
        gSRS_GEQGainMap[9] = SRS_GEQ_MINUS_06DB;
        gSRS_GEQGainMap[10] = SRS_GEQ_MINUS_05DB;
        gSRS_GEQGainMap[11] = SRS_GEQ_MINUS_04DB;
        gSRS_GEQGainMap[12] = SRS_GEQ_MINUS_03DB;
        gSRS_GEQGainMap[13] = SRS_GEQ_MINUS_02DB;
        gSRS_GEQGainMap[14] = SRS_GEQ_MINUS_01DB;
        gSRS_GEQGainMap[15] = SRS_GEQ_0DB;
        gSRS_GEQGainMap[16] = SRS_GEQ_PLUS_01DB;
        gSRS_GEQGainMap[17] = SRS_GEQ_PLUS_02DB;
        gSRS_GEQGainMap[18] = SRS_GEQ_PLUS_03DB;
        gSRS_GEQGainMap[19] = SRS_GEQ_PLUS_04DB;
        gSRS_GEQGainMap[20] = SRS_GEQ_PLUS_05DB;
        gSRS_GEQGainMap[21] = SRS_GEQ_PLUS_06DB;
        gSRS_GEQGainMap[22] = SRS_GEQ_PLUS_07DB;
        gSRS_GEQGainMap[23] = SRS_GEQ_PLUS_08DB;
        gSRS_GEQGainMap[24] = SRS_GEQ_PLUS_09DB;
        gSRS_GEQGainMap[25] = SRS_GEQ_PLUS_10DB;
        gSRS_GEQGainMap[26] = SRS_GEQ_PLUS_11DB;
        gSRS_GEQGainMap[27] = SRS_GEQ_PLUS_12DB;
        gSRS_GEQGainMap[28] = SRS_GEQ_PLUS_13DB;
        gSRS_GEQGainMap[29] = SRS_GEQ_PLUS_14DB;
        gSRS_GEQGainMap[30] = SRS_GEQ_PLUS_15DB;
        gSRS_GEQGainMapInit = true;
    }

#ifdef _SRSCFG_PERFTRACK
    SRS_LOG("[SRS_RAMSIZE] GEQ: %d", sizeof(SRS_Source_GEQ));
#endif  //_SRSCFG_PERFTRACK
}

SRS_Source_GEQ::~SRS_Source_GEQ(){
    if (pGEQBufL != NULL) free(pGEQBufL);
    if (pGEQBufR != NULL) free(pGEQBufR);

    DidCreate = false;

    SRS_LOG("GEQ Destroyed");
}

void SRS_Source_GEQ::Create(SRS_Source_Out *pOut){
    if (pOut->SampleRate <= 0) return;
    if (pOut->ChannelCount != 2) return;

    SampleRate = pOut->SampleRate;
    ChannelCount = pOut->ChannelCount;

    DidCreate = true;

    pGEQBufL = (char *)malloc(SRS_GEQ10B_GetObjSize());
    pGEQBufR = (char *)malloc(SRS_GEQ10B_GetObjSize());

    SRS_GEQ10B_CreateObj(&ObjL, pGEQBufL, NULL, NULL);
    SRS_GEQ10B_CreateObj(&ObjR, pGEQBufR, NULL, NULL);

    if (SampleRate < 9500)      { SRS_GEQ10B_InitObj8k(ObjL);   SRS_GEQ10B_InitObj8k(ObjR); }
    else if (SampleRate < 13500){ SRS_GEQ10B_InitObj11k(ObjL);  SRS_GEQ10B_InitObj11k(ObjR); }
    else if (SampleRate < 19000){ SRS_GEQ10B_InitObj16k(ObjL);  SRS_GEQ10B_InitObj16k(ObjR); }
    else if (SampleRate < 23000){ SRS_GEQ10B_InitObj22k(ObjL);  SRS_GEQ10B_InitObj22k(ObjR); }
    else if (SampleRate < 28000){ SRS_GEQ10B_InitObj24k(ObjL);  SRS_GEQ10B_InitObj24k(ObjR); }
    else if (SampleRate < 38000){ SRS_GEQ10B_InitObj32k(ObjL);  SRS_GEQ10B_InitObj32k(ObjR); }
    else if (SampleRate < 46000){ SRS_GEQ10B_InitObj44k(ObjL);  SRS_GEQ10B_InitObj44k(ObjR); }
    else                        { SRS_GEQ10B_InitObj48k(ObjL);  SRS_GEQ10B_InitObj48k(ObjR); }

    DidConfig = false;
}

void SRS_Source_GEQ::Config(SRS_Source_Out *pOut, SRS_Tech_GEQ *pCFG, bool bLimit){
    (void) (pOut);
    (void) (bLimit);

    if (DidCreate == false) return;

    SRS_GEQ10B_SetEnable(ObjL, true);
    SRS_GEQ10B_SetEnable(ObjR, true);

    //SRS_GEQ10B_SetLimiterEnable(ObjL, bLimit);
    //SRS_GEQ10B_SetLimiterEnable(ObjR, bLimit);

    int i;
    for (i=0; i<10; i++){
        SRS_GEQ10B_SetBandGain(ObjL, i, gSRS_GEQGainMap[pCFG->Users[i] + 15]);
        SRS_GEQ10B_SetBandGain(ObjR, i, gSRS_GEQGainMap[pCFG->Users[i] + 15]);
    }

    DidConfig = true;
}

#if defined (_SRSCFG_ARCH_ARM) || defined (_SRSCFG_ARCH_ARM64)
void SRS_Source_GEQ::Process_256(int32_t *pData){
    SRS_GEQ10B_Process(ObjL, pData, 256, GEQWorkspace);
    SRS_GEQ10B_Process(ObjR, pData + 256, 256, GEQWorkspace);
}
#endif // _SRSCFG_ARCH_ARM

#ifdef _SRSCFG_ARCH_X86
void SRS_Source_GEQ::Process_256(SRSFloat32 *pData){
    SRS_GEQ10B_Process(ObjL, pData, 256, GEQWorkspace);
    SRS_GEQ10B_Process(ObjR, pData + 256, 256, GEQWorkspace);
}
#endif // _SRSCFG_ARCH_X86

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// =-=-=-=-=-=- External Interfacing =-=-=-=-=-=-
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

SRS_Source_GEQ* SRS_Create_GEQ(SRS_Source_Out* pOut){
    SRS_Source_GEQ* pSrc = new SRS_Source_GEQ();
    pSrc->Create(pOut);
    return pSrc;
}

void SRS_Destroy_GEQ(SRS_Source_GEQ* pSrc, SRS_Source_Out* pOut){
    (void) (pOut);

    if (pSrc == NULL) return;
    delete pSrc;
}

void SRS_Config_GEQ(SRS_Source_GEQ* pSrc, SRS_Source_Out* pOut, SRS_Tech_GEQ* pCFG, bool bLimit){
    if (pSrc == NULL) return;
    pSrc->Config(pOut, pCFG, bLimit);
}

#if defined (_SRSCFG_ARCH_ARM) || defined (_SRSCFG_ARCH_ARM64)
void SRS_Process_GEQ_256(SRS_Source_GEQ* pSrc, int32_t* pData){
#endif // _SRSCFG_ARCH_ARM
#ifdef _SRSCFG_ARCH_X86
void SRS_Process_GEQ_256(SRS_Source_GEQ* pSrc, SRSFloat32* pData){
#endif // _SRSCFG_ARCH_X86
    if (pSrc == NULL) return;
    if (pSrc->DidConfig == false) return;

    pSrc->Process_256(pData);
}

enum {
    GEQ_PresetName,
    GEQ_DefaultGains,
    GEQ_UserGains,
};

SRS_Param gGEQ_Params[] = {
    { GEQ_PresetName,       SRS_PTYP_CFG,   SRS_PFMT_STRING,    0.0f,   0.0f,   0.0f,   "geq_presetname", "User-facing name of Preset", "", "", 0},
    { GEQ_DefaultGains,     SRS_PTYP_CFG,   SRS_PFMT_INTARRAY,  0.0f,   -15.0f, 15.0f,  "geq_defgains", "Default gains of Preset", "", "10", 0},
    { GEQ_UserGains,        SRS_PTYP_CFG,   SRS_PFMT_INTARRAY,  0.0f,   -15.0f, 15.0f,  "geq_usergains", "User gains of Preset", "", "10", 0},
};

SRS_Param* SRS_GetBank_GEQ(int& paramCount){
    paramCount = sizeof(gGEQ_Params)/sizeof(SRS_Param);
    return gGEQ_Params;
}

char* SRS_GetVersion_GEQ(char* pWork, size_t maxBytes){
    int v1 = SRS_GEQ_GetTechVersion(SRS_VERSION_MAJOR);
    int v2 = SRS_GEQ_GetTechVersion(SRS_VERSION_MINOR);
    int v3 = SRS_GEQ_GetTechVersion(SRS_VERSION_REVISION);
    int v4 = SRS_GEQ_GetTechVersion(SRS_VERSION_RELEASE);
    int l1 = SRS_GEQ_GetLibVersion(SRS_VERSION_MAJOR);
    int l2 = SRS_GEQ_GetLibVersion(SRS_VERSION_MINOR);
    int l3 = SRS_GEQ_GetLibVersion(SRS_VERSION_REVISION);
    int l4 = SRS_GEQ_GetLibVersion(SRS_VERSION_RELEASE);
    snprintf(pWork, maxBytes, "%d.%d.%d.%d - lib %d.%d.%d.%d", v1, v2, v3, v4, l1, l2, l3, l4);
    return pWork;
}

void SRS_SetParam_GEQ(SRS_Tech_GEQ* pCFG, SRS_Param* pParam, const char* pValue){
    HELP_ParamIn In;

    switch (pParam->EnumID){
        case GEQ_PresetName:{
            if (pCFG->pName != NULL){ delete [] pCFG->pName; pCFG->pName = NULL; }
            int tLen = strlen(pValue);
            if (tLen > 0){
                if (tLen > 63) tLen = 63;
                pCFG->pName = new char[tLen+1];

                if (pCFG->pName != NULL)
                    strlcpy(pCFG->pName, pValue, tLen+1);
            }
        } break;
        case GEQ_UserGains:
        case GEQ_DefaultGains:{
            int tHold[10];
            int* pD = tHold;
            int i;

            int16_t* pS = pCFG->Defs;
            if (pParam->EnumID == GEQ_UserGains) pS = pCFG->Users;

            for (i=0; i<10; i++) tHold[i] = pS[i];      // Fill with current values...
            if (sscanf(pValue, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", pD+0, pD+1, pD+2, pD+3, pD+4, pD+5, pD+6, pD+7, pD+8, pD+9) == 10){
                for (i=0; i<10; i++){
                    if (tHold[i] < -15) pS[i] = -15;
                    else if (tHold[i] > 15) pS[i] = 15;
                    else pS[i] = tHold[i];
                }
            }
        } break;
    }
}

const char* SRS_GetParam_GEQ(SRS_Tech_GEQ* pCFG, SRS_Param* pParam){
    HELP_ParamOut Out;

    switch (pParam->EnumID){
        case GEQ_PresetName:{
            GEQScratch[0] = 0;
            if (pCFG->pName != NULL) strlcpy(GEQScratch, pCFG->pName, sizeof(GEQScratch));
            return GEQScratch;
        } break;
        case GEQ_DefaultGains:{
            int16_t* pD = pCFG->Defs;
            snprintf(GEQScratch, sizeof(GEQScratch), "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", pD[0], pD[1], pD[2], pD[3], pD[4], pD[5], pD[6], pD[7], pD[8], pD[9]);
            return GEQScratch;
        } break;
        case GEQ_UserGains:{
            int16_t* pD = pCFG->Users;
            snprintf(GEQScratch, sizeof(GEQScratch), "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", pD[0], pD[1], pD[2], pD[3], pD[4], pD[5], pD[6], pD[7], pD[8], pD[9]);
            return GEQScratch;
        } break;
    }

    return "";
}

extern void SRS_Default_GEQ(SRS_Tech_GEQ* pCFG){
    pCFG->pName = NULL;
    memset(pCFG->Defs, 0, sizeof(pCFG->Defs));
    memset(pCFG->Users, 0, sizeof(pCFG->Users));
}

};

