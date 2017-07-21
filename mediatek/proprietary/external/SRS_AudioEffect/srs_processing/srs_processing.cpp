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

#include <stdint.h>
#include <sys/types.h>
#include <limits.h>
#include <utils/Log.h>
#include <utils/String8.h>
#include "cutils/properties.h"

#include <media/AudioSystem.h>
#include <media/AudioParameter.h>

#include "srs_types.h"
#include "srs_processing.h"
#include "srs_params.h"
#include "srs_workspace.h"
#include "srs_tech.h"

#undef LOG_TAG
#define LOG_TAG "SRS_Proc"

namespace android {

// New build.prop testing routines

#ifdef POSTPRO_PROPGATE

static int gSRS_AllowOutput = -1;
static int gSRS_AllowInput = -1;

void SRS_PullAllows(){
    char buf[PROPERTY_VALUE_MAX];
    const char* pInKey = "audio.srs.input";
    const char* pOutKey = "audio.srs.output";

    int len = property_get(pInKey, buf, "");
    if (len <= 0) gSRS_AllowInput = 1;  // No prop?  Allow.
    else if (buf[0] == '1') gSRS_AllowInput = 1;    // Prop?  1?  Allow.
    else gSRS_AllowInput = 0;   // Prop?  Anything but 1?  Disallow.

    len = property_get(pOutKey, buf, "");
    if (len <= 0) gSRS_AllowOutput = 1; // No prop?  Allow.
    else if (buf[0] == '1') gSRS_AllowOutput = 1;   // Prop?  1?  Allow.
    else gSRS_AllowOutput = 0;  // Prop?  Anything but 1?  Disallow.
}

bool SRS_DoOutput(){
    if (gSRS_AllowOutput < 0) SRS_PullAllows();
    return (gSRS_AllowOutput > 0)?true:false;
}

bool SRS_DoInput(){
    if (gSRS_AllowInput < 0) SRS_PullAllows();
    return (gSRS_AllowInput > 0)?true:false;
}

bool SRS_DoAny(){
    if ((SRS_DoOutput() == true) || (SRS_DoInput() == true)) return true;
    return false;
}

#endif

const int SRS_Processing::AUTO = -1;

int SRS_Processing::CreateHandle(){
    return SRS_Base_Workspace::CreateWS();
}

void SRS_Processing::DestroyHandle(int handle){
    SRS_Base_Workspace::DeleteWS(handle);
}

void SRS_Processing::ProcessOutNotify(int handle, void* pSource, bool init){
    SRS_Workspace* pWS = SRS_Base_Workspace::GetWS(handle, SRS_Processing::AUTO);
    if (pWS == NULL) return;

    if (init) pWS->SourceOutAdd(pSource);
    else pWS->SourceOutDel(pSource);

    SRS_ParamBlock* pPB = pWS->GetParamBlock();
    pPB->GetParamCache();
}

void SRS_Processing::ProcessOutRoute(int handle, void* pSource, int device){
    SRS_Workspace* pWS = SRS_Base_Workspace::GetWS(handle, SRS_Processing::AUTO);
    ALOGD("ProcessOutRoute: handle = %d, device = %d, pWS = %p", handle, device, pWS);
    if (pWS == NULL) return;

    SRS_Source_Out* pSrc = pWS->SourceOutFind(pSource);
    if (pSrc != NULL) pSrc->SetRoute(device);
}

#ifdef SRS_AUDIOLOG
#ifdef SRS_AUDIOLOG_MARKERS
unsigned long long gTrigTime = 0;
#endif  // SRS_AUDIOLOG_MARKERS
#endif  // SRS_AUDIOLOG
#ifdef MTK_AUDIO
void SRS_Processing::ProcessOut(int handle, void* pSource, void* pSamples, int sampleBytes, int sampleRate, int countChans, int fmt){
    SRS_Workspace* pWS = SRS_Base_Workspace::GetWS(handle, SRS_Processing::AUTO);
    if (pWS == NULL) return;

    SRS_Source_Out* pSrc = pWS->SourceOutFind(pSource);
    if (pSrc == NULL) return;

#ifdef SRS_AUDIOLOG
#ifdef SRS_AUDIOLOG_MARKERS
    unsigned long long nowTime = SRS_Perf::GetRelativeTimeNsec();
    unsigned long long difTime = nowTime-gTrigTime;
    gTrigTime = nowTime;

    if (difTime > 2000000000){  // 2 seconds of sleep!
        short sessMarker[] = { 0, 4096, 0, 2048, 0, 1024, 0, 0, 0, -1024, 0, -2048, 0, -4096 };
        pWS->AL_PreProc(sessMarker, sizeof(sessMarker));
        pWS->AL_PostProc(sessMarker, sizeof(sessMarker));
    }
#endif  // SRS_AUDIOLOG_MARKERS
    pWS->AL_PreProc(pSamples, sampleBytes);
#endif  // SRS_AUDIOLOG
#ifdef MTK_AUDIO
    pSrc->processFormat = fmt;
#endif
#ifdef MTK_AUDIO
if(fmt == AUDIO_FORMAT_PCM_32_BIT)
{
    pSrc->Process<int32_t>(pSamples, sampleBytes, sampleRate, countChans);
}else{
    pSrc->Process<int16_t>(pSamples, sampleBytes, sampleRate, countChans);
}
#else
    pSrc->Process(pSamples, sampleBytes, sampleRate, countChans);
#endif

#ifdef SRS_AUDIOLOG
    pWS->AL_PostProc(pSamples, sampleBytes);
#endif  // SRS_AUDIOLOG

#ifdef SRS_FORCE_SILENCE
    memset(pSamples, 0, sampleBytes);
#endif  // SRS_FORCE_SILENCE

}
#endif
void SRS_Processing::ProcessOut(int handle, void* pSource, void* pSamples, int sampleBytes, int sampleRate, int countChans){
    SRS_Workspace* pWS = SRS_Base_Workspace::GetWS(handle, SRS_Processing::AUTO);
    if (pWS == NULL) return;

    SRS_Source_Out* pSrc = pWS->SourceOutFind(pSource);
    if (pSrc == NULL) return;

#ifdef SRS_AUDIOLOG
#ifdef SRS_AUDIOLOG_MARKERS
    unsigned long long nowTime = SRS_Perf::GetRelativeTimeNsec();
    unsigned long long difTime = nowTime-gTrigTime;
    gTrigTime = nowTime;

    if (difTime > 2000000000){  // 2 seconds of sleep!
        short sessMarker[] = { 0, 4096, 0, 2048, 0, 1024, 0, 0, 0, -1024, 0, -2048, 0, -4096 };
        pWS->AL_PreProc(sessMarker, sizeof(sessMarker));
        pWS->AL_PostProc(sessMarker, sizeof(sessMarker));
    }
#endif  // SRS_AUDIOLOG_MARKERS
    pWS->AL_PreProc(pSamples, sampleBytes);
#endif  // SRS_AUDIOLOG

#ifdef MTK_AUDIO
        pSrc->Process<int16_t>(pSamples, sampleBytes, sampleRate, countChans);
#else
        pSrc->Process(pSamples, sampleBytes, sampleRate, countChans);
#endif

#ifdef SRS_AUDIOLOG
    pWS->AL_PostProc(pSamples, sampleBytes);
#endif  // SRS_AUDIOLOG

#ifdef SRS_FORCE_SILENCE
    memset(pSamples, 0, sampleBytes);
#endif  // SRS_FORCE_SILENCE

}

void SRS_Processing::ProcessInNotify(int handle, void* pSource, bool init){
    SRS_Workspace* pWS = SRS_Base_Workspace::GetWS(handle, SRS_Processing::AUTO);
    if (pWS == NULL) return;

    if (init) pWS->SourceInAdd(pSource);
    else pWS->SourceInDel(pSource);
}

void SRS_Processing::ProcessInRoute(int handle, void* pSource, int device){
    SRS_Workspace* pWS = SRS_Base_Workspace::GetWS(handle, SRS_Processing::AUTO);
    if (pWS == NULL) return;

    SRS_Source_In* pSrc = pWS->SourceInFind(pSource);
    if (pSrc != NULL) pSrc->SetRoute(device);
}

void SRS_Processing::ProcessIn(int handle, void* pSource, void* pSamples, int sampleBytes, int sampleRate, int countChans){
    SRS_Workspace* pWS = SRS_Base_Workspace::GetWS(handle, SRS_Processing::AUTO);
    if (pWS == NULL) return;

    SRS_Source_In* pSrc = pWS->SourceInFind(pSource);
    if (pSrc == NULL) return;

    pSrc->Process(pSamples, sampleBytes, sampleRate, countChans);
}

#ifdef _SRSCFG_DSPOFFLOAD_PATH
    #define DSPOFFLOAD_RAWDATASET_HEAD
    #include SRS_STR(_SRSCFG_DSPOFFLOAD_PATH)
    #undef DSPOFFLOAD_RAWDATASET_HEAD
#endif // _SRSCFG_DSPOFFLOAD_PATH

void SRS_Processing::RawDataSet(int* pHandle, char* pKey, void* pData, int dataLen){
    (void)(pKey);
    (void)(pData);
    (void)(dataLen);

    SRS_Workspace* pWS = NULL;

    if (pHandle != NULL)
        pWS = SRS_Base_Workspace::GetWS(*pHandle, SRS_Processing::AUTO);

#ifdef _SRSCFG_DSPOFFLOAD_PATH
    #define DSPOFFLOAD_RAWDATASET_FUNC
    #include SRS_STR(_SRSCFG_DSPOFFLOAD_PATH)
    #undef DSPOFFLOAD_RAWDATASET_FUNC
#endif // _SRSCFG_DSPOFFLOAD_PATH
}

void SRS_Processing::ParamsSet(int handle, const String8& keyValues){
    ParamsSet_Notify(handle, keyValues);
}

bool SRS_Processing::ParamsSet_Notify(int handle, const String8& keyValues){
    SRS_Workspace* pWS = SRS_Base_Workspace::GetWS(handle, SRS_Processing::AUTO);
    if (pWS == NULL) return false;

    SRS_ParamBlock* pPB = pWS->GetParamBlock();
    if (pPB == NULL) return false;

    SRS_LOG("ParamSet string: %s", keyValues.string());

    AudioParameter param = AudioParameter(keyValues);
    String8 value;
    String8 key;

    int tHoldSIG = pWS->CFGSig;

    bool bPostSave = true;
    bool bDidSet = false;

    key = "bluetooth_enabled";
    if (param.get(key, value) == NO_ERROR){
        pWS->RouteMap.AllowBT = 0;
        const char* pVal = value.string();
        if (pVal != NULL){
            if (pVal[0] == '1') pWS->RouteMap.AllowBT = 1;
            if (pVal[0] == 't') pWS->RouteMap.AllowBT = 1;
        }
        pWS->CFGSig++;
        return true;    // Do not process other keys!
    }

    key = "srs_processing_defersave";
    if (param.get(key, value) == NO_ERROR) bPostSave = false;   // Don't save as a result of change (useful for sliders, etc)

    key = "srs_processing_forcesave";
    if (param.get(key, value) == NO_ERROR){ bDidSet = true; bPostSave = true; }     // Save even without changes occuring (often used after a chain of defers)

    key = "srs_processing_loadcfg";                     // Load from path in value, or stored path if empty
    if (param.get(key, value) == NO_ERROR){
        const char* pPath = value.string();
        pWS->ReadUserConfig(pPath);
    }

    key = "srs_processing_savecfg";                     // save to path in value, or stored path if empty
    if (param.get(key, value) == NO_ERROR){
        const char* pPath = value.string();
        pWS->WriteUserConfig(pPath);
    }

    key = "srs_processing_formatcfg";
    if (param.get(key, value) == NO_ERROR){
        const char* pPath = value.string();
        pWS->FormatUserConfig(pPath);
    }

    key = "srs_processing_userdefaults";
    if (param.get(key, value) == NO_ERROR){
        pWS->ApplyUserDefaults();
        bDidSet = true;
    }

#ifdef SRS_AUDIOLOG
    key = "srs_audiolog_start";
    if (param.get(key, value) == NO_ERROR) pWS->AL_Start();

    key = "srs_audiolog_stop";
    if (param.get(key, value) == NO_ERROR) pWS->AL_Stop();
#endif  // SRS_AUDIOLOG

    SRS_ParamInfo info;

    size_t i;
    size_t tLen = param.size();
    for (i=0; i<tLen; i++){
        param.getAt(i, key, value);

        SRS_Param* pP = pPB->FindParam(key.string(), info, NULL);

        if ((pP != NULL) && (pP->Type >= SRS_PTYP_CFG)){

#ifdef SRS_PARAMWRITE_CFG_BLOCK
            if (pP->Type == SRS_PTYP_CFG) continue;
#endif  // SRS_PARAMWRITE_CFG_BLOCK

#ifdef SRS_PARAMWRITE_PREF_BLOCK
            if (pP->Type == SRS_PTYP_PREF) continue;
#endif  // SRS_PARAMWRITE_PREF_BLOCK

#ifdef _SRS_PROC_ANDROID_SYNCHRONIZE
            {   // acquire lock
                AutoMutex _l(pWS->mLock);       // Only the structures are protected
#endif //_SRS_PROC_ANDROID_SYNCHRONIZE
                pWS->SetParamValue(pPB, info, value.string());
#ifdef _SRS_PROC_ANDROID_SYNCHRONIZE
            }   // release lock
#endif //_SRS_PROC_ANDROID_SYNCHRONIZE
            bDidSet = true;     // Flag to indicate something may have changed...
        }
    }

    if (bPostSave && bDidSet) pWS->WriteUserConfig(NULL);

    if (pWS->CFGSig != tHoldSIG) return true;
    return false;
}

const char* gBlockTypes = ""
#ifdef SRS_PARAMWRITE_CFG_BLOCK
    "write_cfg,"
#endif
#ifdef SRS_PARAMWRITE_PREF_BLOCK
    "write_pref,"
#endif
#ifdef SRS_PARAMREAD_INFO_BLOCK
    "read_info,"
#endif
#ifdef SRS_PARAMREAD_DEBUG_BLOCK
    "read_debug,"
#endif
#ifdef SRS_PARAMREAD_CFG_BLOCK
    "read_cfg,"
#endif
#ifdef SRS_PARAMREAD_PREF_BLOCK
    "read_pref,"
#endif
#ifdef SRS_USERCFG_ALLOW
    "allow_user,"
#endif
#ifdef SRS_USERCFG_UNLOCKED
    "unlock_user,"
#endif
#ifdef SRS_AUDIOLOG
    "audio_log,"
#endif  // SRS_AUDIOLOG
;   // End of Blocking String

char gBuildTags[] = { "INV - " SRS_STR(SRS_TAGS) };

String8 SRS_Processing::ParamsGet(int handle, const String8& keys){
    SRS_Workspace* pWS = SRS_Base_Workspace::GetWS(handle, SRS_Processing::AUTO);
    if (pWS == NULL) return String8("");

    SRS_ParamBlock* pPB = pWS->GetParamBlock();

    AudioParameter param = AudioParameter(keys);
    String8 value;
    String8 key;

    key = "srs_processing_params";
    if (param.get(key, value) == NO_ERROR && pPB){     // If we get this, we _only_ do this.
        const char *pStr = pPB->GetParamCache();
        if (pStr)
            return String8(pStr);
        return String8();
    }

    AudioParameter outParams = AudioParameter();
    SRS_ParamInfo info;

    size_t i;

    key = "srs_processing_basecfg";
    if (param.get(key, value) == NO_ERROR){
        value = SRS_STR(SRS_BASECFG_READPATH);
        outParams.add(key, value);
    }

    key = "srs_processing_usercfg";
    if (param.get(key, value) == NO_ERROR){
        value = SRS_STR(SRS_USERCFG_PATH);
        outParams.add(key, value);
    }

    key = "srs_processing_tags";
    if (param.get(key, value) == NO_ERROR){
        size_t tLen = strlen(gBuildTags);
        for (i=0; i<tLen; i++)
            if (gBuildTags[i] == '^') gBuildTags[i] = ',';  // Weird sep due to cmd-line define syntax...

        if (pWS->LicenseState == 1){ gBuildTags[0] = 'D'; gBuildTags[1] = 'M'; gBuildTags[2] = 'O'; }
        else if (pWS->LicenseState > 1){ gBuildTags[0] = 'R'; gBuildTags[1] = 'E'; gBuildTags[2] = 'T'; }

        value = (const char *)gBuildTags;
        outParams.add(key, value);
    }

    key = "srs_processing_blocked";
    if (param.get(key, value) == NO_ERROR){
        value = gBlockTypes;
        outParams.add(key, value);
    }

    size_t tLen = param.size();
    for (i=0; i<tLen; i++){
        param.getAt(i, key, value);

        SRS_Param* pP = pPB->FindParam(key.string(), info, NULL);
        if (pP != NULL){

#ifdef SRS_PARAMREAD_INFO_BLOCK
            if (pP->Type == SRS_PTYP_INFO) continue;
#endif

#ifdef SRS_PARAMREAD_DEBUG_BLOCK
            if (pP->Type == SRS_PTYP_DEBUG) continue;
#endif

#ifdef SRS_PARAMREAD_CFG_BLOCK
            if (pP->Type == SRS_PTYP_CFG) continue;
#endif

#ifdef SRS_PARAMREAD_PREF_BLOCK
            if (pP->Type == SRS_PTYP_PREF) continue;
#endif

            const char* pV = NULL;
#ifdef _SRS_PROC_ANDROID_SYNCHRONIZE
            {   // acquire lock
                AutoMutex _l(pWS->mLock);       // Only the structures are protected
#endif //_SRS_PROC_ANDROID_SYNCHRONIZE
                pV = pWS->GetParamValue(pPB, info);
#ifdef _SRS_PROC_ANDROID_SYNCHRONIZE
            }   // release lock
#endif //_SRS_PROC_ANDROID_SYNCHRONIZE
            if (pV != NULL){
                value = pV;
                outParams.add(key, value);
            }
        }
    }

    return outParams.toString();
}

};  // namespace android

