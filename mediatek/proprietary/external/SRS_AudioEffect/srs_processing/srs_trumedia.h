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

#ifndef ANDROID_SRS_TRUMEDIA
#define ANDROID_SRS_TRUMEDIA

#include "srs_types.h"
#include "srs_techs/srs_tech_tools.h"
#include "srs_subs/srs_routing.h"

#ifdef _SRSCFG_WOWHDX
#include "srs_techs/srs_tech_wowhdx.h"
#else   // _SRSCFG_WOWHDX
#include "srs_techs/srs_tech_wowhd.h"
#endif  // _SRSCFG_WOWHDX

#include "srs_techs/srs_tech_hpf.h"
#include "srs_techs/srs_tech_cshp.h"

#include "srs_techs/srs_tech_aeq.h"
#include "srs_techs/srs_tech_hlimit.h"

#ifdef _SRSCFG_USERPEQ
    #include "srs_techs/srs_tech_userpeq.h"
    #define UPEQ_PRESET_COUNT   10
#endif  // _SRSCFG_USERPEQ

#ifdef _SRSCFG_MOBILE_EQ
    #include "srs_techs/srs_tech_geq.h"
    #define GEQ_PRESET_COUNT    10
#endif  // _SRSCFG_MOBILE_EQ

namespace android {

#ifdef _SRSCFG_WOWHDX
    #define BID_Int_Mus_AEQ   0
    #define BID_Int_Mov_AEQ   1
    #define BID_Int_Pod_AEQ   2
    #define BID_Int_All_AEQ   0
    #define BID_Ext_All_AEQ   3

    #define BID_Int_Mus_HLimL   0
    #define BID_Int_Mus_HLimB   1
    #define BID_Int_Mov_HLimL   2
    #define BID_Int_Mov_HLimB   3
    #define BID_Int_Pod_HLimL   4
    #define BID_Int_Pod_HLimB   5
    #define BID_Int_All_HLimL   0
    #define BID_Int_All_HLimB   1
    #define BID_Int_Byp_HLimL   6
    #define BID_Int_Byp_HLimB   7
    #define BID_Ext_All_HLimL   8
    #define BID_Ext_Byp_HLimL   9
#else   // _SRSCFG_WOWHDX
    #define BID_Int_Mus_AEQ   0
    #define BID_Int_Mov_AEQ   0
    #define BID_Int_Pod_AEQ   0
    #define BID_Int_All_AEQ   0
    #define BID_Ext_All_AEQ   1

    #define BID_Int_Mus_HLimL   0
    #define BID_Int_Mus_HLimB   1
    #define BID_Int_Mov_HLimL   0
    #define BID_Int_Mov_HLimB   1
    #define BID_Int_Pod_HLimL   0
    #define BID_Int_Pod_HLimB   1
    #define BID_Int_All_HLimL   0
    #define BID_Int_All_HLimB   1
    #define BID_Int_Byp_HLimL   2
    #define BID_Int_Byp_HLimB   3
    #define BID_Ext_All_HLimL   4
    #define BID_Ext_Byp_HLimL   5
#endif  // _SRSCFG_WOWHDX

#define BID_AEQ_Max (BID_Ext_All_AEQ+1)
#define BID_HLim_Max (BID_Ext_Byp_HLimL+1)

#ifdef _SRSCFG_MOBILE_EQ_BASIC
    #define BID_Int_Mus_GEQ 0
    #define BID_Int_Mov_GEQ 0
    #define BID_Int_Pod_GEQ 0
    #define BID_Int_All_GEQ 0
    #define BID_Ext_Mus_GEQ 1
    #define BID_Ext_Mov_GEQ 1
    #define BID_Ext_Pod_GEQ 1
    #define BID_Ext_All_GEQ 1
    #define BID_GEQ_Max 2
#endif // _SRSCFG_MOBILE_EQ_BASIC

#ifdef _SRSCFG_MOBILE_EQ_EXTENDED
    #define BID_Int_Mus_GEQ 0
    #define BID_Int_Mov_GEQ 1
    #define BID_Int_Pod_GEQ 2
    #define BID_Int_All_GEQ 0
    #define BID_Ext_Mus_GEQ 3
    #define BID_Ext_Mov_GEQ 4
    #define BID_Ext_Pod_GEQ 5
    #define BID_Ext_All_GEQ 3
    #define BID_GEQ_Max 6
#endif // _SRSCFG_MOBILE_EQ_EXTENDED
#ifdef MTK_AUDIO
// do not define sample_t
#else
#ifdef _SRS_PROC_32BIT
typedef int32_t sample_t;
#else
typedef int16_t sample_t;
#endif //_SRS_PROC_32BIT
#endif
enum {
    PROCST_OFF = 0,
    PROCST_IN,
    PROCST_ON,
    PROCST_OUT
};

#define PROC_FADELEN (1024)
#define PROC_DELAYIN (256)

struct SRS_Source_Out : public SRS_Base_Source {
    int RouteCode;          // The result of the route passed through the Routing Table...
    int LogRouteCode;
    int LogRoute;

    int ProcState;          // None, Fade In, Active, Fade Out
    int ProcTrack;          // Num of samples for Fade, etc...

    int Framesize;

    bool UseCache;
    void* pCacheSpool;
    SRSSamp* pCacheData;    // From
    SRSSamp* pCacheTarget;  // To
    int CachePos;

    SRSSamp ActiveGain;     // Pre-procesing scale

    SRS_DCRState DCState;

    int HPFIndex;
    bool HPFPath;
    bool HPFActive;
    SRS_Source_HiPass* pHPF;

    int WOWHDIndex;
    SRS_Tech_State WHDState;
    SRS_Source_WOWHD* pWOWHD;
    void WHDConfigState(void);

    int CSHPIndex;
    SRS_Tech_State CSHPState;
    SRS_Source_CSHP* pCSHP;
    void CSHPConfigState(void);

#ifdef _SRSCFG_USERPEQ
    SRS_Tech_State UPEQState;
    SRS_Source_AEQ* pUPEQ;
    void UPEQConfigState(void);
#endif  // _SRSCFG_USERPEQ

#ifdef _SRSCFG_MOBILE_EQ
    int GEQIndex;
    SRS_Tech_State GEQState;
    SRS_Source_GEQ* pGEQ;
    void GEQConfigState(void);
#endif  // _SRSCFG_MOBILE_EQ

    int AEQIndex;
    bool AEQPath;
    bool AEQActive;
    SRS_Source_AEQ* pAEQ;

    int HLimitIndex;
    bool HLimitPath;
    bool HLimitActive;
    SRS_Source_HLimit* pHLimit;

    SRS_Source_Out();
    ~SRS_Source_Out();

    void SetRoute(int route);
#ifdef MTK_AUDIO
    template < typename sample_t> void Process(void* pSamples, int sampleBytes, int sampleRate, int countChans);
#else
    void Process(void* pSamples, int sampleBytes, int sampleRate, int countChans);
#endif
    void InitCaching();
    void SwapCaching();

    bool WillProcess();
    #ifdef MTK_AUDIO
    template < typename sample_t>
    void CoreProcess(sample_t* pSamples, int frames);
    #else
    void CoreProcess(sample_t* pSamples, int frames);
    #endif
    void SubProcess();

    void APIInit(int sampleRate, int countChans);
    void CFGSync();
    #ifdef MTK_AUDIO
    int processFormat;
    #endif
};

struct SRS_Source_In : public SRS_Base_Source {
    SRS_Source_In();
    ~SRS_Source_In();

    void SetRoute(int route);
    void Process(void* pSamples, int sampleBytes, int sampleRate, int countChans);
};

struct SRS_Workspace : public SRS_Base_Workspace {
    bool Skip;

    bool TMEnable;
    int TMPreset;
    bool VolIntEnable;  // Vol Int or Ext can be either MaxVF or TruVol

    float TMIGains[2];  // Int/Ext (add more for extra devices)

    int TMSkip;

    SRS_RouteMap RouteMap;

#ifdef _SRSCFG_MOBILE_EQ
    bool GEQEnables[BID_GEQ_Max];
    bool GEQLimits[BID_GEQ_Max];
    int GEQPresets[BID_GEQ_Max];
#endif

#ifdef _SRSCFG_PERFTRACK
    SRS_Perf PerfTrack;
    int PerfType;
#endif

    SRS_Tech_HiPass HPFTunings[2];
    SRS_Tech_WOWHD WHDTunings[5];
    SRS_Tech_CSHP CSHPTuning;
    SRS_Tech_AEQ AEQTunings[BID_AEQ_Max];
    SRS_Tech_HLimit HLTunings[BID_HLim_Max];

#ifdef _SRSCFG_USERPEQ
    SRS_Tech_UserPEQ_CFG UPEQCFG;
    SRS_Tech_UserPEQ_Preset UPEQInts[UPEQ_PRESET_COUNT];
#endif

#ifdef _SRSCFG_MOBILE_EQ
    SRS_Tech_GEQ GEQInts[GEQ_PRESET_COUNT];
    SRS_Tech_GEQ GEQExts[GEQ_PRESET_COUNT];
#endif

    SRS_Workspace();
    ~SRS_Workspace();

    void SourceOutAdd(void* pSource);
    void SourceOutDel(void* pSource);
    SRS_Source_Out* SourceOutFind(void* pSource);

    void SourceInAdd(void* pSource);
    void SourceInDel(void* pSource);
    SRS_Source_In* SourceInFind(void* pSource);

    SRS_ParamBlock* GetParamBlock();
    void FillParamBlock(SRS_ParamBlock* pPB);
    void SetParamValue(SRS_ParamBlock* pPB, SRS_ParamInfo& info, const char* pValue);
    const char* GetParamValue(SRS_ParamBlock* pPB, SRS_ParamInfo& info);
    void ListParamIndexes(SRS_ParamBlock* pPB, SRS_ParamInfo& info, SRS_ParamIndexInfo& idxinfo);
    void ListBankIndexes(SRS_ParamBlock* pPB, SRS_ParamInfo& info, SRS_ParamIndexInfo& idxinfo);


    void DoInit();
    void LoadBaseConfigs();
    void LoadConfigs();

    void ApplyUserDefaults();

    bool ReadUserConfig(const char* pPath);
    void WriteUserConfig(const char* pPath);
    void FormatUserConfig(const char* pFormat);

    bool ConfigRead(const char* pPath, SRS_ParamBlock* pBlock, uint32_t typeMask=0xFFFFFFFF);
    void ConfigWrite(const char* pPath, SRS_ParamBlock* pBlock, uint32_t typeMask=0xFFFFFFFF, const char* pFormat=NULL);

#ifdef _SRSCFG_DSPOFFLOAD_PATH
    #define DSPOFFLOAD_WORKSPACE
    #include SRS_STR(_SRSCFG_DSPOFFLOAD_PATH)
    #undef DSPOFFLOAD_WORKSPACE
#endif // _SRSCFG_DSPOFFLOAD_PATH

    void DSPOffload_Init();
    void DSPOffload_Uninit();
    void DSPOffload_Send(SRS_Source_Out* pOut);
    void DSPOffload_Clear(SRS_Source_Out* pOut);

#ifdef SRS_AUDIOLOG
    int AL_Active;
    int AL_Status;
    FILE* pAL_OutPre;
    FILE* pAL_OutPost;

    void AL_Init();
    void AL_Exit();

    void AL_Start();
    void AL_Stop();
    void AL_PreProc(void* pSamples, int sampleBytes);
    void AL_PostProc(void* pSamples, int sampleBytes);
#endif
};

};

#endif  // ANDROID_SRS_TRUMEDIA

