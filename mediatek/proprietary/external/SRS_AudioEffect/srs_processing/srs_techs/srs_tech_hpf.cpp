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
#include SRSLIBINC(srs_iir_design_api.h)
#include SRSLIBINC(srs_iir_api.h)
#include SRSLIBINC(srs_common_ver_api.h)

char HPFScratch[1024];
char HPFScratch2[1024];

//#define SRS_XOVERMODE

#undef  LOG_TAG
#define LOG_TAG "SRS_Tech_HiPass"

namespace android {

struct SRS_Source_HiPass {
    SRSIirObj       ObjL;
    SRSIirObj       ObjR;
    void            *pObjLBuf;
    void            *pObjRBuf;
    SRSCoef32       IIR_Coefs_32bits[32];
    SRS_Tech_HiPass Active;
    int             SampleRate;
    int             ChannelCount;
    bool            ForceActive;
    bool            DidCreate;
    bool            DidConfig;

    SRS_Source_HiPass();
    ~SRS_Source_HiPass();

    void Create(SRS_Source_Out *pOut);
    void Config(SRS_Source_Out *pOut, SRS_Tech_HiPass *pCFG, bool bBypass);
    void Process_256(SRSSamp *pData);
};

SRS_Source_HiPass::SRS_Source_HiPass(){
    SRS_LOG("HiPass Created");

    DidCreate = false;
    DidConfig = false;

    pObjLBuf = malloc(SRS_IIR_GetObjSize(8));   // 8th order max...
    pObjRBuf = malloc(SRS_IIR_GetObjSize(8));   // 8th order max...

#ifdef _SRSCFG_PERFTRACK
    SRS_LOG("[SRS_RAMSIZE] HPF: %d", SRS_IIR_OBJ_SIZE(8)+sizeof(SRS_Source_HiPass));
#endif  //_SRSCFG_PERFTRACK
    ForceActive = true;
    Active.Skip = false;
    Active.Order = 0;
    Active.Frequency = 0;
    Active.Is32Bit = false;
}

SRS_Source_HiPass::~SRS_Source_HiPass(){
    if (pObjLBuf != NULL) free(pObjLBuf);
    pObjLBuf = NULL;
    if (pObjRBuf != NULL) free(pObjRBuf);
    pObjRBuf = NULL;

    DidCreate = false;
    DidConfig = false;

    SRS_LOG("HiPass Destroyed");
}

void SRS_Source_HiPass::Create(SRS_Source_Out* pOut){
    if (pOut->SampleRate <= 0) return;
    if (pOut->ChannelCount != 2) return;

    SampleRate = pOut->SampleRate;
    ChannelCount = pOut->ChannelCount;

    DidCreate = true;

    // We don't build objects here - forced or changed filters will cause it...

    DidConfig = false;
}

void SRS_Source_HiPass::Config(SRS_Source_Out *pOut, SRS_Tech_HiPass *pCFG, bool bBypass){
    (void) (pOut);

    if (DidCreate == false) return;

#if defined (_SRSCFG_ARCH_ARM) || defined (_SRSCFG_ARCH_ARM64)
    if (DIFF_FORCED(Is32Bit)){  // Must rebuild!
        ForceActive = true;
    }
#endif  // _SRSCFG_ARCH_ARM

    if (DIFF_FORCED(Order) || DIFF_FORCED(Frequency))
    {
        int tXOverDelta = 0;

        SRS_IIR_CreateObj(&ObjL, pObjLBuf, NULL, NULL, 8);
        SRS_IIR_CreateObj(&ObjR, pObjRBuf, NULL, NULL, 8);

#ifdef _SRSCFG_ARCH_X86
        SRS_IirFilterDesignFloat((float) (pCFG->Frequency + tXOverDelta), pCFG->Order, SampleRate, SRS_IIR_DESIGN_HP, IIR_Coefs_32bits);
        SRS_IIR_InitObj(ObjL, pCFG->Order, IIR_Coefs_32bits);
        SRS_IIR_InitObj(ObjR, pCFG->Order, IIR_Coefs_32bits);
#endif

#if defined (_SRSCFG_ARCH_ARM) || defined (_SRSCFG_ARCH_ARM64)
        Tool_GenHiPassCoefs(IIR_Coefs_32bits, pCFG->Order, pCFG->Frequency+tXOverDelta, SampleRate, pCFG->Is32Bit);
        SRS_IIR_InitObj(ObjL, pCFG->Order, IIR_Coefs_32bits, pCFG->Is32Bit ? SRSFilter32 : SRSFilter16);
        SRS_IIR_InitObj(ObjR, pCFG->Order, IIR_Coefs_32bits, pCFG->Is32Bit ? SRSFilter32 : SRSFilter16);
#endif // _SRSCFG_ARCH_X86
    }

    if (bBypass){
        SRS_IIR_SetEnable(ObjL, false);
        SRS_IIR_SetEnable(ObjR, false);
    }
    else {
        SRS_IIR_SetEnable(ObjL, true);
        SRS_IIR_SetEnable(ObjR, true);
    }

    DidConfig = true;
    Active = *pCFG;
    ForceActive = false;
}

void SRS_Source_HiPass::Process_256(SRSSamp* pData){
    SRS_IIR_Process(ObjL, pData, 256);
    SRS_IIR_Process(ObjR, pData + 256, 256);
}

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// =-=-=-=-=-=- External Interfacing =-=-=-=-=-=-
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

SRS_Source_HiPass* SRS_Create_HiPass(SRS_Source_Out* pOut){
    SRS_Source_HiPass* pSrc = new SRS_Source_HiPass();
    pSrc->Create(pOut);
    return pSrc;
}

void SRS_Destroy_HiPass(SRS_Source_HiPass* pSrc, SRS_Source_Out* pOut){
    (void) (pOut);

    if (pSrc == NULL) return;
    delete pSrc;
}

void SRS_Config_HiPass(SRS_Source_HiPass* pSrc, SRS_Source_Out* pOut, SRS_Tech_HiPass* pCFG, bool bBypass){
    if (pSrc == NULL) return;
    pSrc->Config(pOut, pCFG, bBypass);
}

void SRS_Process_HiPass_256(SRS_Source_HiPass* pSrc, SRSSamp* pData){
    if (pSrc == NULL) return;
    if (pSrc->DidConfig == false) return;

    pSrc->Process_256(pData);
}

enum {
    HPF_Skip,

    HPF_Is32Bit,

    HPF_Order,
    HPF_Freq,
};

SRS_Param gHiPass_Params[] = {
    { HPF_Order,            SRS_PTYP_CFG,   SRS_PFMT_INT,           6.0f,   1.0f,   8.0f,   "hipass_order", "Filter Order", "", "", 0},
    { HPF_Freq,             SRS_PTYP_CFG,   SRS_PFMT_INT,           6.0f,   1.0f,   8.0f,   "hipass_frequency", "Filter Frequency", "", "", 0},

#if (defined (_SRSCFG_ARCH_ARM) || defined (_SRSCFG_ARCH_ARM64))
    { HPF_Is32Bit,          SRS_PTYP_CFG,   SRS_PFMT_BOOL,          0.0f,   0.0f,   1.0f,   "hipass_is32bit", "Filter Precision (16bit when 0)", "", "", 0},
#endif  // _SRSCFG_ARCH_ARM

    { HPF_Skip,             SRS_PTYP_CFG,   SRS_PFMT_BOOL,          0.0f,   0.0f,   1.0f,   "hipass_skip", "Skips High Pass Filter when true", "toggle", "", 0},
};

SRS_Param* SRS_GetBank_HiPass(int& paramCount){
    paramCount = sizeof(gHiPass_Params)/sizeof(SRS_Param);
    return gHiPass_Params;
}

char* SRS_GetVersion_HiPass(char* pWork, size_t maxBytes){
    int v1 = SRS_Common_GetTechVersion(SRS_VERSION_MAJOR);
    int v2 = SRS_Common_GetTechVersion(SRS_VERSION_MINOR);
    int v3 = SRS_Common_GetTechVersion(SRS_VERSION_REVISION);
    int v4 = SRS_Common_GetTechVersion(SRS_VERSION_RELEASE);
    int l1 = SRS_Common_GetLibVersion(SRS_VERSION_MAJOR);
    int l2 = SRS_Common_GetLibVersion(SRS_VERSION_MINOR);
    int l3 = SRS_Common_GetLibVersion(SRS_VERSION_REVISION);
    int l4 = SRS_Common_GetLibVersion(SRS_VERSION_RELEASE);
    snprintf(pWork, maxBytes, "%d.%d.%d.%d - lib %d.%d.%d.%d", v1, v2, v3, v4, l1, l2, l3, l4);
    return pWork;
}

void SRS_SetParam_HiPass(SRS_Tech_HiPass* pCFG, SRS_Param* pParam, const char* pValue){
    HELP_ParamIn In;

    switch (pParam->EnumID){
        case HPF_Skip: pCFG->Skip = In.GetBool(pValue); break;

        case HPF_Order: pCFG->Order = In.GetInt(pValue); break;
        case HPF_Freq: pCFG->Frequency = In.GetInt(pValue); break;
        case HPF_Is32Bit: pCFG->Is32Bit = In.GetBool(pValue); break;
    }
}

const char* SRS_GetParam_HiPass(SRS_Tech_HiPass* pCFG, SRS_Param* pParam){
    HELP_ParamOut Out;

    switch (pParam->EnumID){
        case HPF_Skip: return Out.FromBool(pCFG->Skip);

        case HPF_Order: return Out.FromInt(pCFG->Order);
        case HPF_Freq: return Out.FromInt(pCFG->Frequency);
        case HPF_Is32Bit: return Out.FromBool(pCFG->Is32Bit);
    }

    return "";
}

void SRS_Default_HiPass(SRS_Tech_HiPass* pCFG){
    pCFG->Skip = false;

    pCFG->Order = 4;
    pCFG->Frequency = 200;
    pCFG->Is32Bit = false;
}

};

