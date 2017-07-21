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

#include SRSLIBINC(srs_hardlimiter_api.h)

#undef  LOG_TAG
#define LOG_TAG "SRS_Tech_HLimit"

namespace android {

struct SRS_Source_HLimit {
    SRSHdLmtObj         Obj;
    SRS_Tech_HLimit     Active;
    void                *pObjBuffer;
    void                *pWorkSpace;
    int                 SampleRate;
    int                 ChannelCount;
    bool                ForceActive;
    bool                DidCreate;
    bool                DidConfig;

    SRS_Source_HLimit();
    ~SRS_Source_HLimit();

    void Create(SRS_Source_Out *pOut);
    void Config(SRS_Source_Out *pOut, SRS_Tech_HLimit *pCFG, bool bBypass);
    void Process_256(SRSSamp *pIn, SRSSamp *pOut);
};

SRS_Source_HLimit::SRS_Source_HLimit(){
    SRS_LOG("HLimit Created");

    DidCreate = false;
    DidConfig = false;

    pObjBuffer = malloc(SRS_HdLmt_GetObjSize() * 4);
    pWorkSpace = malloc(SRS_HDLMT_WORKSPACE_SIZE(256) * 4);

#ifdef _SRSCFG_PERFTRACK
    SRS_LOG("[SRS_RAMSIZE] HL: %d", ((SRS_HdLmt_GetObjSize() + SRS_HARDLIMITER_WORKSPACE_SIZE(256)) * 4) + sizeof(SRS_Source_HLimit));
#endif  //_SRSCFG_PERFTRACK
}

SRS_Source_HLimit::~SRS_Source_HLimit(){
    if (pObjBuffer != NULL) free(pObjBuffer);
    pObjBuffer = NULL;
    if (pWorkSpace != NULL) free(pWorkSpace);
    pWorkSpace = NULL;

    DidCreate = false;
    DidConfig = false;

    SRS_LOG("HLimit Destroyed");
}

void SRS_Source_HLimit::Create(SRS_Source_Out *pOut){
    if (pOut->SampleRate <= 0) return;
    if (pOut->ChannelCount != 2) return;

    SampleRate = pOut->SampleRate;
    ChannelCount = pOut->ChannelCount;

    DidCreate = true;
    ForceActive = true;

    SRS_HdLmt_CreateObj(&Obj, pObjBuffer, NULL, NULL); // Ignore Klocwork warning here. This is used as intended.

    SRS_HdLmt_InitObj(Obj);
    SRS_HdLmt_SetControlDefaults(Obj);
}

void SRS_Source_HLimit::Config(SRS_Source_Out *pOut, SRS_Tech_HLimit *pCFG, bool bBypass){
    (void) (pOut);

    if (DidCreate == false) return;

    if (DIFF_FORCED(IGain))     SRS_HdLmt_SetInputGain(Obj, SRS_FXP16(pCFG->IGain, SRS_HDLMT_INOUT_GAIN_IWL));
    if (DIFF_FORCED(OGain))     SRS_HdLmt_SetOutputGain(Obj, SRS_FXP16(pCFG->OGain, SRS_HDLMT_INOUT_GAIN_IWL));
    if (DIFF_FORCED(BGain))     SRS_HdLmt_SetBypassGain(Obj, SRS_FXP16(pCFG->BGain, SRS_HDLMT_BYPASS_GAIN_IWL));
    if (DIFF_FORCED(DelayLen))  SRS_HdLmt_SetDelayLen(Obj, pCFG->DelayLen);
    if (DIFF_FORCED(Boost))     SRS_HdLmt_SetBoostGain(Obj, SRS_FXP32(pCFG->Boost, SRS_HDLMT_BOOST_IWL));
    if (DIFF_FORCED(Limit))     SRS_HdLmt_SetLimitLevel(Obj, SRS_FXP16(pCFG->Limit, SRS_HDLMT_LIMIT_IWL));

    //if (DIFF_FORCED(DecaySmooth)) SRS_SetHardLimiterDecaySmoothEnable(Obj, pCFG->DecaySmooth);

    if (bBypass)    SRS_HdLmt_SetEnable(Obj, false);
    else            SRS_HdLmt_SetEnable(Obj, true);

    DidConfig = true;
    Active = *pCFG;
    ForceActive = false;
}

void SRS_Source_HLimit::Process_256(SRSSamp *pIn, SRSSamp *pOut){
    SRSStereoCh inChans;
    SRSStereoCh outChans;
    inChans.Left = pIn;
    inChans.Right = pIn+256;
    outChans.Left = pOut;
    outChans.Right = pOut+256;

    SRS_HdLmt_Process(Obj, &inChans, &outChans, 256, pWorkSpace);
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// =-=-=-=-=-=- External Interfacing =-=-=-=-=-=-
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

SRS_Source_HLimit* SRS_Create_HLimit(SRS_Source_Out* pOut){
    SRS_Source_HLimit* pSrc = new SRS_Source_HLimit();
    pSrc->Create(pOut);
    return pSrc;
}

void SRS_Destroy_HLimit(SRS_Source_HLimit* pSrc, SRS_Source_Out* pOut){
    (void) (pOut);

    if (pSrc == NULL) return;
    delete pSrc;
}

void SRS_Config_HLimit(SRS_Source_HLimit* pSrc, SRS_Source_Out* pOut, SRS_Tech_HLimit* pCFG, bool bBypass){
    if (pSrc == NULL) return;
    pSrc->Config(pOut, pCFG, bBypass);
}

void SRS_Process_HLimit_256(SRS_Source_HLimit* pSrc, SRSSamp* pIn, SRSSamp* pOut){
    if (pSrc == NULL) return;
    if (pSrc->DidConfig == false) return;

    pSrc->Process_256(pIn, pOut);
}

enum {
    HL_IGain,
    HL_OGain,
    HL_BGain,
    HL_Skip,
    HL_DelayLen,
    HL_DecaySmooth,
    HL_Boost,
    HL_Limit,
};

SRS_Param gHLimit_Params[] = {
    { HL_IGain,         SRS_PTYP_CFG,   SRS_PFMT_FLOAT,     0.6f,   0.0f,   1.0f,   "hlimit_igain", "Input Gain", "gain", "", 0},
    { HL_OGain,         SRS_PTYP_CFG,   SRS_PFMT_FLOAT,     1.0f,   0.0f,   1.0f,   "hlimit_ogain", "Output Gain", "gain", "", 0},
    { HL_BGain,         SRS_PTYP_CFG,   SRS_PFMT_FLOAT,     1.0f,   0.0f,   1.0f,   "hlimit_bgain", "Bypass Gain", "gain", "", 0},
    { HL_DelayLen,      SRS_PTYP_CFG,   SRS_PFMT_INT,       16.0f,  8.0f,   32.0f,  "hlimit_delaylen", "Length of the Delay Line", "length", "", 0},
    { HL_DecaySmooth,   SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   1.0f,   "hlimit_decaysmooth", "Toggles use of smoothing on Decay", "toggle", "", 0},
    { HL_Boost,         SRS_PTYP_CFG,   SRS_PFMT_FLOAT,     1.0f,   0.001f, 256.0f, "hlimit_boost", "Boost", "gain", "", 0},
    { HL_Limit,         SRS_PTYP_CFG,   SRS_PFMT_FLOAT,     1.0f,   0.0f,   1.0f,   "hlimit_limit", "Limit", "control", "", 0},
    { HL_Skip,          SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   1.0f,   "hlimit_skip", "Skips Hard Limiter when true", "toggle", "", 0},
};

SRS_Param* SRS_GetBank_HLimit(int& paramCount){
    paramCount = sizeof(gHLimit_Params)/sizeof(SRS_Param);
    return gHLimit_Params;
}

char* SRS_GetVersion_HLimit(char* pWork, size_t maxBytes){
    int v1 = SRS_HdLmt_GetTechVersion(SRS_VERSION_MAJOR);
    int v2 = SRS_HdLmt_GetTechVersion(SRS_VERSION_MINOR);
    int v3 = SRS_HdLmt_GetTechVersion(SRS_VERSION_REVISION);
    int v4 = SRS_HdLmt_GetTechVersion(SRS_VERSION_RELEASE);
    int l1 = SRS_HdLmt_GetLibVersion(SRS_VERSION_MAJOR);
    int l2 = SRS_HdLmt_GetLibVersion(SRS_VERSION_MINOR);
    int l3 = SRS_HdLmt_GetLibVersion(SRS_VERSION_REVISION);
    int l4 = SRS_HdLmt_GetLibVersion(SRS_VERSION_RELEASE);
    snprintf(pWork, maxBytes, "%d.%d.%d.%d - lib %d.%d.%d.%d", v1, v2, v3, v4, l1, l2, l3, l4);
    return pWork;
}

void SRS_SetParam_HLimit(SRS_Tech_HLimit* pCFG, SRS_Param* pParam, const char* pValue){
    HELP_ParamIn In;

    switch (pParam->EnumID){
        case HL_IGain: pCFG->IGain = In.GetFloat(pValue); break;
        case HL_OGain: pCFG->OGain = In.GetFloat(pValue); break;
        case HL_BGain: pCFG->BGain = In.GetFloat(pValue); break;
        case HL_Skip: pCFG->Skip = In.GetBool(pValue); break;
        case HL_DelayLen: pCFG->DelayLen = In.GetInt(pValue); break;
        case HL_DecaySmooth: pCFG->DecaySmooth = In.GetBool(pValue); break;
        case HL_Boost: pCFG->Boost = In.GetFloat(pValue); break;
        case HL_Limit: pCFG->Limit = In.GetFloat(pValue); break;
    }
}

const char* SRS_GetParam_HLimit(SRS_Tech_HLimit* pCFG, SRS_Param* pParam){
    HELP_ParamOut Out;

    switch (pParam->EnumID){
        case HL_IGain: return Out.FromFloat(pCFG->IGain);
        case HL_OGain: return Out.FromFloat(pCFG->OGain);
        case HL_BGain: return Out.FromFloat(pCFG->BGain);
        case HL_Skip: return Out.FromBool(pCFG->Skip);
        case HL_DelayLen: return Out.FromInt(pCFG->DelayLen);
        case HL_DecaySmooth: return Out.FromBool(pCFG->DecaySmooth);
        case HL_Boost: return Out.FromFloat(pCFG->Boost);
        case HL_Limit: return Out.FromFloat(pCFG->Limit);
    }

    return "";
}

void SRS_Default_HLimit(SRS_Tech_HLimit* pCFG, bool Boosted){

    pCFG->IGain = 1.0f;
    pCFG->OGain = 1.0f;
    pCFG->BGain = 1.0f;
    pCFG->Skip = false;
    pCFG->DelayLen = 22;

#ifdef _SRSCFG_WOWHDX
    pCFG->Boost = 5.5f;
    if (Boosted) pCFG->Boost = 10.0f;
#else   // _SRSCFG_WOWHDX
    pCFG->Boost = 1.0f;
    if (Boosted) pCFG->Boost = 2.5f;
#endif  // _SRSCFG_WOWHDX

    pCFG->Limit = 1.0f;
    pCFG->DecaySmooth = true;
}

};

