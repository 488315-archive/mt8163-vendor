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

#undef LOG_TAG
#define LOG_TAG "SRS_Tech_Tools"

SRS_Perf::SRS_Perf(){
    TimingStart = DeltaStart = DeltaAccum = 0;
    DeltaSamps = DeltaCount = 0;
}

unsigned long long SRS_Perf::GetRelativeTimeNsec(void){
//#ifdef HAVE_POSIX_CLOCKS
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (unsigned long long)now.tv_sec*1000000000LL + now.tv_nsec;
//#else
//  struct timeval now;
//  gettimeofday(&now, NULL);
//  return (unsigned long long)now.tv_sec*1000000000LL + now.tv_usec * 1000LL;
//#endif
}

void SRS_Perf::StartTiming(){
    TimingStart = GetRelativeTimeNsec();
    DeltaAccum = 0;
    DeltaSamps = 0;
    DeltaCount = 0;
}

void SRS_Perf::LogTiming(const char* pTag){
    unsigned long long tCEnd = GetRelativeTimeNsec();

    float tSliceWindow = (tCEnd-TimingStart)/1000000000.0;      // How much real-time were we operating?  1.0 = 1sec
    int32_t tSliceCycles = DeltaAccum;
    float tSliceMIPS = DeltaAccum/(1000.0*tSliceWindow);

    SRS_LOG("%s - %d kcycles / %3.3f sec = %3.3f MIPS (%d samples in %d calls)", pTag, tSliceCycles, tSliceWindow, tSliceMIPS, DeltaSamps, DeltaCount);
}

void SRS_Perf::EndTiming(){
    // Does nothing here - could do file-based logging, etc...
}

void SRS_Perf::StartDelta(){
    DeltaStart = GetRelativeTimeNsec();
}

void SRS_Perf::EndDelta(uint32_t sampleCount){
    unsigned long long tEndTime = GetRelativeTimeNsec();
    unsigned long long tRelTime = tEndTime-DeltaStart;
    double tPcntTime = (double)tRelTime/1000000000.0;   // 1.0 = 1sec
    int32_t tCPUFreq = 1000000; // 1Ghz default

    FILE* pF = fopen("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq", "rb");    // Linux momentary cpu frequency
    if (pF != NULL){
        char tHold[33];
        int tLen = fread(tHold, 1, 32, pF);
        fclose(pF);
        tHold[tLen] = 0;
        tCPUFreq = atoi(tHold);
    }

    DeltaSamps += sampleCount;
    DeltaAccum += tPcntTime*tCPUFreq;   // Cycles used
    DeltaCount++;
}

char ToolScratch[1024];

namespace android {

SRSSamp gFadeHold[512];

bool SRS_Tech_State::PreFade(SRSSamp* pSamples){
    if ((FadeOut == false) && (FadeIn == false)) return false;

    memcpy(gFadeHold, pSamples, sizeof(SRSSamp)*256*2);

    if (FadeOut) return false;  // Fading out - don't update any values!

    // Fading in - maybe notify of update! (won't trigger if fade-out already setup new config in postfade)
    if (WantDirty){
        InPath = WantPath;
        IsActive = WantActive;
        UseIndex = WantIndex;
        WantDirty = false;
        return true;
    }

    return false;
}

SRSFadeScale SRS_Tech_State::CalcFadeScale(float scale){
#if (defined (_SRSCFG_ARCH_ARM) || defined (_SRSCFG_ARCH_ARM64))
    if (scale <= 0.004f) return 65536;  // Floor for 1.0/256.0
    if (scale >= 1.0f) return 256;

    float tFScale = 256.0f/scale;
    return (int)tFScale;
#endif  // _SRSCFG_ARCH_ARM

#ifdef _SRSCFG_ARCH_X86
    return scale;
#endif  // _SRSCFG_ARCH_X86
}

bool SRS_Tech_State::PostFade(SRSSamp* pSamples){
    if ((FadeOut == false) && (FadeIn == false)) return false;

    int i;

    if (FadeOut){       // Do the Fade Out
        SRS_LOG("Fade Out");
        FadeOut = false;

        SRSSamp* pDstL = pSamples;
        SRSSamp* pDstR = pSamples+256;
        SRSSamp* pSrcL = gFadeHold;
        SRSSamp* pSrcR = gFadeHold+256;

        for (i=0; i<256; i++, pDstL++, pDstR++, pSrcL++, pSrcR++){

#if (defined (_SRSCFG_ARCH_ARM) || defined (_SRSCFG_ARCH_ARM64))
            int32_t tL = ((*pDstL>>8)*(255-i))+((*pSrcL/UseScale)*i);
            int32_t tR = ((*pDstR>>8)*(255-i))+((*pSrcR/UseScale)*i);
#endif  // _SRSCFG_ARCH_ARM

#ifdef _SRSCFG_ARCH_X86
            float tI = (float)i/255.0f;
            float tL = (*pDstL*(1.0f-tI))+(*pSrcL*UseScale*tI);
            float tR = (*pDstR*(1.0f-tI))+(*pSrcR*UseScale*tI);
#endif  // _SRSCFG_ARCH_X86

            *pDstL = tL;
            *pDstR = tR;
        }

        InPath = WantPath;
        IsActive = WantActive;
        UseIndex = WantIndex;
        UseScale = WantScale;
        WantDirty = false;
        return true;
    }

    SRS_LOG("Fade In");

    // Fade In!
    FadeIn = false;

    SRSSamp* pDstL = pSamples;
    SRSSamp* pDstR = pSamples+256;
    SRSSamp* pSrcL = gFadeHold;
    SRSSamp* pSrcR = gFadeHold+256;

    for (i=0; i<256; i++, pDstL++, pDstR++, pSrcL++, pSrcR++){

#if (defined (_SRSCFG_ARCH_ARM) || defined (_SRSCFG_ARCH_ARM64))
        int32_t tL = ((*pDstL>>8)*i)+((*pSrcL/UseScale)*(255-i));
        int32_t tR = ((*pDstR>>8)*i)+((*pSrcR/UseScale)*(255-i));
#endif  // _SRSCFG_ARCH_ARM

#ifdef _SRSCFG_ARCH_X86
        float tI = (float)i/255.0f;
        float tL = (*pDstL*tI)+(*pSrcL*UseScale*(1.0f-tI));
        float tR = (*pDstR*tI)+(*pSrcR*UseScale*(1.0f-tI));
#endif  // _SRSCFG_ARCH_X86

        *pDstL = tL;
        *pDstR = tR;
    }

    return false;
}

float Tool_MaxZero(float tIn, float tMax){
    if (tIn < 0.0f) return 0.0f;
    if (tIn > tMax) return tMax;
    return tIn;
}

void Tool_SeekValue(float want, float& active){
    if (active > want){
        active -= 0.01f;
        if (active < want) active = want;
    } else {
        active += 0.01f;
        if (active > want) active = want;
    }
}

void Tool_GenHiPassCoefs(int32_t* pCoefs, int order, float freq, int srate){
    long cutoffFrequency = (long)freq;
    long sampleRate;
    double b0, b1, b2, a0, a1, a2;
    double expTerm;
    double tanTerm;
    int k;

    sampleRate = (long)srate;
    expTerm = pow(exp(0.2302585092994046 * 3.0) - 1.0, 1.0 / (2.0 * order));
    tanTerm = tan(3.141592653589793 * cutoffFrequency / sampleRate);
    if (cutoffFrequency >= sampleRate / 2)
        order = 0;

    if (order & 1){
        b0 =  2.0;
        b1 = -2.0;
        b2 = 0;
        a0 = 2.0 * expTerm * tanTerm + 2.0;
        a1 = 2.0 * expTerm * tanTerm - 2.0;
        a2 = 0;
        b0 /= a0;
        b1 /= a0;
        a1 /= -a0;
        *pCoefs++ = 2;
        *pCoefs++ = SRS_FXP32(b0, 2);
        *pCoefs++ = SRS_FXP32(b1, 2);
        *pCoefs++ = SRS_FXP32(b2, 2);
        *pCoefs++ = SRS_FXP32(a1, 2);
        *pCoefs++ = SRS_FXP32(a2, 2);
    }
    for (k = 1; k <= order / 2; k++){
        double cosTerm = 2.0 * cos((order + 2 * k - 1) * 3.141592653589793 / (2.0 * order));
        b0 = b2 = 4.0;
        b1 = -8.0;
        a0 = 4.0 * (1.0 - expTerm * tanTerm * (cosTerm - expTerm * tanTerm));
        a1 = 8.0 * (expTerm * expTerm * tanTerm * tanTerm - 1.0);
        a2 = 4.0 * (1.0 + expTerm * tanTerm * (cosTerm + expTerm * tanTerm));
        b0 /= a0;
        b1 /= a0;
        b2 /= a0;
        a1 /= -a0;
        a2 /= -a0;
        *pCoefs++ = 2;
        *pCoefs++ = SRS_FXP32(b0, 2);
        *pCoefs++ = SRS_FXP32(b1, 2);
        *pCoefs++ = SRS_FXP32(b2, 2);
        *pCoefs++ = SRS_FXP32(a1, 2);
        *pCoefs++ = SRS_FXP32(a2, 2);
    }

    *pCoefs++ = 1;
    *pCoefs++ = SRS_FXP32(1.0, 1);
}

void Tool_GenHiPassCoefs(void* pCoefs, int order, int freq, int srate, int is32bit){
    long cutoffFrequency = (long)freq;
    long sampleRate;
    double b0, b1, b2, a0, a1, a2;
    double expTerm;
    double tanTerm;
    int k;

#if (defined (_SRSCFG_ARCH_ARM) || defined (_SRSCFG_ARCH_ARM64))
    int32_t* p32Coefs = (int32_t*)pCoefs;
    int16_t* p16Coefs = (int16_t*)pCoefs;
#endif  // _SRSCFG_ARCH_ARM

    sampleRate = (long)srate;
    expTerm = pow(exp(0.2302585092994046 * 3.0) - 1.0, 1.0 / (2.0 * order));
    tanTerm = tan(3.141592653589793 * cutoffFrequency / sampleRate);
    if (cutoffFrequency >= sampleRate / 2)
        order = 0;
    if (order > 8) order = 8;

    int tIWLScale = 4;

    if (order & 1){
        b0 =  2.0;
        b1 = -2.0;
        b2 = 0;
        a0 = 2.0 * expTerm * tanTerm + 2.0;
        a1 = 2.0 * expTerm * tanTerm - 2.0;
        a2 = 0;
        b0 /= a0;
        b1 /= a0;
        a1 /= -a0;

#if (defined (_SRSCFG_ARCH_ARM) || defined (_SRSCFG_ARCH_ARM64))
        if (is32bit){
            *p32Coefs++ = 2;
            *p32Coefs++ = SRS_FXP32(b0, tIWLScale);
            *p32Coefs++ = SRS_FXP32(b1, tIWLScale);
            *p32Coefs++ = SRS_FXP32(b2, tIWLScale);
            *p32Coefs++ = SRS_FXP32(a1, 2);
            *p32Coefs++ = SRS_FXP32(a2, 2);
        } else {
            *p16Coefs++ = 2;
            *p16Coefs++ = SRS_FXP16(b0, tIWLScale);
            *p16Coefs++ = SRS_FXP16(b1, tIWLScale);
            *p16Coefs++ = SRS_FXP16(b2, tIWLScale);
            *p16Coefs++ = SRS_FXP16(a1, 2);
            *p16Coefs++ = SRS_FXP16(a2, 2);
        }
        tIWLScale = 2;
#endif  // _SRSCFG_ARCH_ARM
    }
    for (k = 1; k <= order / 2; k++){
        double cosTerm = 2.0 * cos((order + 2 * k - 1) * 3.141592653589793 / (2.0 * order));
        b0 = b2 = 4.0;
        b1 = -8.0;
        a0 = 4.0 * (1.0 - expTerm * tanTerm * (cosTerm - expTerm * tanTerm));
        a1 = 8.0 * (expTerm * expTerm * tanTerm * tanTerm - 1.0);
        a2 = 4.0 * (1.0 + expTerm * tanTerm * (cosTerm + expTerm * tanTerm));
        b0 /= a0;
        b1 /= a0;
        b2 /= a0;
        a1 /= -a0;
        a2 /= -a0;

#if (defined (_SRSCFG_ARCH_ARM) || defined (_SRSCFG_ARCH_ARM64))
        if (is32bit){
            *p32Coefs++ = 2;
            *p32Coefs++ = SRS_FXP32(b0, tIWLScale);
            *p32Coefs++ = SRS_FXP32(b1, tIWLScale);
            *p32Coefs++ = SRS_FXP32(b2, tIWLScale);
            *p32Coefs++ = SRS_FXP32(a1, 2);
            *p32Coefs++ = SRS_FXP32(a2, 2);
        } else {
            *p16Coefs++ = 2;
            *p16Coefs++ = SRS_FXP16(b0, tIWLScale);
            *p16Coefs++ = SRS_FXP16(b1, tIWLScale);
            *p16Coefs++ = SRS_FXP16(b2, tIWLScale);
            *p16Coefs++ = SRS_FXP16(a1, 2);
            *p16Coefs++ = SRS_FXP16(a2, 2);
        }
        tIWLScale = 2;
#endif // _SRSCFG_ARCH_ARM
    }

#if (defined (_SRSCFG_ARCH_ARM) || defined (_SRSCFG_ARCH_ARM64))
    if (is32bit){
        *p32Coefs++ = 3;
        *p32Coefs++ = SRS_FXP32(4.0, 3);
    } else {
        *p16Coefs++ = 3;
        *p16Coefs++ = SRS_FXP16(4.0, 3);
    }
#endif  // _SRSCFG_ARCH_ARM
}


void Tool_GenLoPassCoefs(void* pCoefs, int order, int freq, int srate, int is32bit){
    long cutoffFrequency = (long)freq;
    long sampleRate;
    double b0, b1, b2, a0, a1, a2;
    double expTerm;
    double tanTerm;
    int k;

    int32_t* p32Coefs = (int32_t*)pCoefs;
    int16_t* p16Coefs = (int16_t*)pCoefs;

    sampleRate = (long)srate;

    if (cutoffFrequency >= sampleRate / 2)
        cutoffFrequency = sampleRate / 2 -1;
    if (order > 8) order = 8;

    expTerm = 2.0 * pow(exp(0.2302585092994046 * 3.0) - 1.0, 1.0 / (2.0 * order));
    tanTerm = 2.0 * tan(3.141592653589793 * cutoffFrequency / sampleRate);

    int tIWLScale = 4;

    if (order & 1){
        b0 = b1 = tanTerm;
        b2 = 0;
        a0 = tanTerm + expTerm;
        a1 = tanTerm - expTerm;
        a2 = 0;
        b0 /= a0;
        b1 /= a0;
        a1 /= -a0;

        b0 = 1.0;
        b1 = 0.0;
        b2 = 0.0;
        a1 = 0.0;
        a2 = 0.0;
        tIWLScale = 2;

        if (is32bit){
            *p32Coefs++ = 2;
            *p32Coefs++ = SRS_FXP32(b0, tIWLScale);
            *p32Coefs++ = SRS_FXP32(b1, tIWLScale);
            *p32Coefs++ = SRS_FXP32(b2, tIWLScale);
            *p32Coefs++ = SRS_FXP32(a1, 2);
            *p32Coefs++ = SRS_FXP32(a2, 2);
        } else {
            *p16Coefs++ = 2;
            *p16Coefs++ = SRS_FXP16(b0, tIWLScale);
            *p16Coefs++ = SRS_FXP16(b1, tIWLScale);
            *p16Coefs++ = SRS_FXP16(b2, tIWLScale);
            *p16Coefs++ = SRS_FXP16(a1, 2);
            *p16Coefs++ = SRS_FXP16(a2, 2);
        }

        tIWLScale = 2;
    }
    for (k = 1; k <= order / 2; k++){
        double cosTerm = cos((order + 2 * k - 1) * 3.141592653589793 / (2.0 * order));
        b0 = b2 = tanTerm * tanTerm;
        b1 = b0 * 2.0;
        a0 = expTerm * expTerm - 2.0 * expTerm * cosTerm * tanTerm + tanTerm * tanTerm;
        a1 = -2.0 * expTerm * expTerm + 2.0 * tanTerm * tanTerm;
        a2 = expTerm * expTerm + 2.0 * expTerm * cosTerm * tanTerm + tanTerm * tanTerm;
        b0 /= a0;
        b1 /= a0;
        b2 /= a0;
        a1 /= -a0;
        a2 /= -a0;

        b0 = 1.0;
        b1 = 0.0;
        b2 = 0.0;
        a1 = 0.0;
        a2 = 0.0;
        tIWLScale = 2;

        if (is32bit){
            *p32Coefs++ = 2;
            *p32Coefs++ = SRS_FXP32(b0, tIWLScale);
            *p32Coefs++ = SRS_FXP32(b1, tIWLScale);
            *p32Coefs++ = SRS_FXP32(b2, tIWLScale);
            *p32Coefs++ = SRS_FXP32(a1, 2);
            *p32Coefs++ = SRS_FXP32(a2, 2);
        } else {
            *p16Coefs++ = 2;
            *p16Coefs++ = SRS_FXP16(b0, tIWLScale);
            *p16Coefs++ = SRS_FXP16(b1, tIWLScale);
            *p16Coefs++ = SRS_FXP16(b2, tIWLScale);
            *p16Coefs++ = SRS_FXP16(a1, 2);
            *p16Coefs++ = SRS_FXP16(a2, 2);
        }

        tIWLScale = 2;
    }

    if (is32bit){
        *p32Coefs++ = 3;
        *p32Coefs++ = SRS_FXP32(1.0, 3);
        // /*p32Coefs++ = SRS_FXP32(4.0, 3);*/
    } else {
        *p16Coefs++ = 3;
        *p16Coefs++ = SRS_FXP16(1.0, 3);
        // /*p16Coefs++ = SRS_FXP16(4.0, 3);*/
    }
}

void SRS_DCRState::Process_256(SRSSamp* pData){
    /* 32/64 bit version (assumes "int" is 32 bits), with full saturation checking */
    /* Warp the cutoff from fs/4 to fNew by setting rho = Sin[Pi*(fNew/fs - 0.25)]/Sin[Pi*(fNew/fs + 0.25)] */

    const int rho = (int)(-0.99869185905046492407684599064794 * (double)0x80000000LL);  // 10 Hz at 48kHz for DC reject, should be okay for all sample rates
    int *state;
    int x, y, z;
    int64_t acc;
    int i, j;

    for (j=0; j<2; j++){
        for (i=0; i<256; i++){
            /* State memory needs two locations per channel (i.e., int mZ[2]) */
            state = Workspace[j];
            x = pData[(j*256)+i];   // Stereo serial (L then R)
            x >>= 1;    /* Multiply by one half */
            z = *state;
            *state++ = x;
            acc = (int64_t)x - (int64_t)*state;
            if (acc > 0x7FFFFFFFLL)
            acc = 0x7FFFFFFFLL;
            else if (acc < -0x80000000LL)
            acc = -0x80000000LL;
            y = (int)acc;
            y = (int)((int64_t)y * (int64_t)rho >> 31);
            acc = (int64_t)y + (int64_t)z;
            if (acc > 0x7FFFFFFFLL)
            acc = 0x7FFFFFFFLL;
            else if (acc < -0x80000000LL)
            acc = -0x80000000LL;
            y = (int)acc;
            *state-- = y;
            acc = (int64_t)x - (int64_t)y;
            if (acc > 0x7FFFFFFFLL)
            acc = 0x7FFFFFFFLL;
            else if (acc < -0x80000000LL)
            acc = -0x80000000LL;
            y = (int)acc;

            pData[(j*256)+i] = y;   // Stereo serial (L then R)
        }
    }
}

};

