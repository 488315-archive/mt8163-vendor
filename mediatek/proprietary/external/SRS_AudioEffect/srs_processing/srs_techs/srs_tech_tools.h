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

#ifndef ANDROID_SRS_TECH_TOOLS
#define ANDROID_SRS_TECH_TOOLS

#include <time.h>

struct SRS_Perf {
    SRS_Perf();

    static unsigned long long   GetRelativeTimeNsec(void);

    unsigned long long  TimingStart;    // Time at start of perf
    unsigned long long  DeltaStart;     // Time at start of delta

    unsigned long long  DeltaAccum;     // Accumulation of all deltas
    uint32_t            DeltaSamps;     // Accumulation of all samples
    uint32_t            DeltaCount;     // Number of times a delta is added

    void    StartTiming();
    void    LogTiming(const char* pTag);
    void    EndTiming();

    void    StartDelta();
    void    EndDelta(uint32_t sampleCount);
};

#define DIFF_FORCED(a) ((ForceActive) || (Active.a != pCFG->a))

namespace android {

struct SRS_Tech_State {
    bool InPath;        // Now
    bool IsActive;
    int UseIndex;
    SRSFadeScale UseScale;

    bool FadeOut;
    bool FadeIn;

    bool WantPath;      // Post Fade Out, Pre Fade In
    bool WantActive;
    int WantIndex;
    SRSFadeScale WantScale;

    bool WantDirty;

    SRS_Tech_State(){
        WantPath = InPath = false;
        WantActive = IsActive = false;
        WantIndex = UseIndex = -1;
        WantScale = UseScale = SRSFadeScale_Default;
        FadeIn = FadeOut = false;
        WantDirty = false;
    }

    bool SetWants(bool path, bool active, int index, SRSFadeScale scale = SRSFadeScale_Default){
        WantPath = path;
        WantActive = active;
        WantIndex = index;
        WantScale = scale;

        if (WantPath == false){ WantActive = false; WantIndex = -1; }
        else if (WantIndex < 0){ WantIndex = -1; WantPath = false; WantActive = false; }

        FadeIn = FadeOut = false;
        WantDirty = false;

        if (WantPath != InPath) WantDirty = true;
        if (WantActive != IsActive) WantDirty = true;
        if (WantIndex != UseIndex) WantDirty = true;

        if (WantDirty == false) return false;

        if (InPath) FadeOut = true; // Fade out because we're active now, either turing off, or changing...
        if (WantPath) FadeIn = true;    // Fade in because we're starting, or changing.

        return true;    // Needs to fade
    }

    bool PreFade(SRSSamp* pSamples);    // If we need to fade, prep here - return true to cause Reconfig of tech...
    bool PostFade(SRSSamp* pSamples);   // If we need to fade, act here - return true to cause Reconfig of tech...
    static SRSFadeScale CalcFadeScale(float scale);
};

struct SRS_DCRState {
    int Workspace[2][4];    // Stereo

    SRS_DCRState(){
        memset(Workspace, 0, sizeof(Workspace));
    }

    void Process_256(SRSSamp* pData);
};

extern double Tool_DoComplexWithReals(double freq, double gain, unsigned sampleRate, double a1, double a2);
extern void Tool_GenHiPassCoefs(void* pCoefs, int order, int freq, int srate, int is32bit);
extern void Tool_GenLoPassCoefs(void* pCoefs, int order, int freq, int srate, int is32bit);

extern float Tool_MaxZero(float tIn, float tMax);
extern void Tool_SeekValue(float want, float& active);

};

#endif  // ANDROID_SRS_TECH_TOOLS
