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
#include <time.h>
#include <utils/Log.h>
#include <utils/String8.h>

#include "srs_types.h"
#include "srs_workspace.h"
#include "srs_params.h"
#include "srs_tech.h"

#undef LOG_TAG
#define LOG_TAG "SRS_ProcWS"

namespace android {

SRS_Base_Source::SRS_Base_Source(){
    DidAPIInit = false;
    pSource = NULL;
    Route = -1;
    SampleRate = -1;
    ChannelCount = -1;
    pOwner = NULL;
    CFGSig = -1;
}

SRS_Base_Source::~SRS_Base_Source(){
    pSource = NULL;
}

SRS_Base_Workspace::SRS_Base_Workspace(){
    CFGSig = 0;
    Handle = -2;

#ifdef SRS_PROTECT_AUTHID
    SRS_License_SetInfo(SRS_PROTECT_AUTHID, (char *)SRS_STR(SRS_PROTECT_PATH));
#endif  // SRS_PROTECT_AUTHID

    pOutSpool = SRS_Spool_Create(SRS_WORKSOURCES);
    pInSpool = SRS_Spool_Create(SRS_WORKSOURCES);
    LicenseState = SRS_License_State();
    ActiveOut = 0;
    ActiveIn = 0;
    DidInit = 0;
    is_trumediahd_enabled = SRS_License_Is_Trumediahd_Enabled();
}

SRS_Base_Workspace::~SRS_Base_Workspace(){
    SRS_Spool_Destroy(pOutSpool);
    SRS_Spool_Destroy(pInSpool);

    pOutSpool = NULL;
    pInSpool = NULL;
}

void SRS_Workspace::SourceOutAdd(void* pSource){
    void* pEntry = SRS_Spool_SourceFind(pOutSpool, pSource);
    if (pEntry != NULL) { SRS_LOG("SRS_Processing - SourceOutAdd - Dupe Source %p", pSource); return; }
    if (SRS_Spool_SourceAvail(pOutSpool) == false){ SRS_LOG("SRS_Processing - SourceOutAdd - No Available Slot for %p", pSource); return; }

    SRS_Source_Out* pSrc = new SRS_Source_Out();
    pSrc->pSource = pSource;
    pSrc->pOwner = (SRS_Workspace*)this;

    SRS_Spool_SourceAdd(pOutSpool, pSrc, pSource);
    ActiveOut++;

    CFGSig++;
    DoInit();
}

void SRS_Workspace::SourceOutDel(void* pSource){
    SRS_Source_Out* pSrc = SourceOutFind(pSource);
    if (pSrc == NULL){
        SRS_LOG("SRS_Processing - SourceOutDel - Source Not Located %p", pSource);
    }

    delete pSrc;

    if (SRS_Spool_SourceDel(pOutSpool, pSource) == false){
        SRS_LOG("SRS_Processing - SourceOutDel - Source Not Deleted %p", pSource);
        return;
    }

    ActiveOut--;
    CFGSig++;
}

SRS_Source_Out* SRS_Workspace::SourceOutFind(void* pSource){
    return (SRS_Source_Out*) SRS_Spool_SourceFind(pOutSpool, pSource);
}

void SRS_Workspace::SourceInAdd(void* pSource){
    void* pEntry = SRS_Spool_SourceFind(pInSpool, pSource);
    if (pEntry != NULL) { SRS_LOG("SRS_Processing - SourceInAdd - Dupe Source %p", pSource); return; }
    if (SRS_Spool_SourceAvail(pInSpool) == false){ SRS_LOG("SRS_Processing - SourceInAdd - No Available Slot for %p", pSource); return; }

    SRS_Source_In* pSrc = new SRS_Source_In();
    pSrc->pSource = pSource;
    pSrc->pOwner = (SRS_Workspace*)this;

    SRS_Spool_SourceAdd(pInSpool, pSrc, pSource);
    ActiveIn++;

    CFGSig++;
    DoInit();
}

void SRS_Workspace::SourceInDel(void* pSource){
    SRS_Source_In* pSrc = SourceInFind(pSource);
    if (pSrc == NULL){
        SRS_LOG("SRS_Processing - SourceInDel - Source Not Located %p", pSource);
    }

    delete pSrc;

    if (SRS_Spool_SourceDel(pInSpool, pSource) == false){
        SRS_LOG("SRS_Processing - SourceInDel - Source Not Deleted %p", pSource);
        return;
    }

    ActiveIn--;
    CFGSig++;
}

SRS_Source_In* SRS_Workspace::SourceInFind(void* pSource){
    return (SRS_Source_In*) SRS_Spool_SourceFind(pInSpool, pSource);
}

void SRS_Workspace::DoInit(){
    if (DidInit != 0) return;

    DidInit = 1;
    LoadBaseConfigs();
}

SRS_Workspace gAuto;

SRS_Workspace** SRS_Base_Workspace::pSW_Stack = NULL;
int SRS_Base_Workspace::pSW_StackSize = 0;

int SRS_Base_Workspace::CreateWS(){
    int handle = -1;
    int i;

    for (i=0; i<pSW_StackSize; i++){
        if (pSW_Stack[i] == NULL){
            handle = i;
            break;
        }
    }

    if (handle < 0){
        SRS_Workspace** pHold = new SRS_Workspace*[pSW_StackSize+1];
        if (pSW_StackSize > 0) memcpy(pHold, pSW_Stack, pSW_StackSize*sizeof(SRS_Workspace*));
        if (pSW_Stack != NULL){
            delete [] pSW_Stack;
            pSW_Stack = NULL;
        }
        pSW_Stack = pHold;
        handle = pSW_StackSize;
        pSW_StackSize++;
    }

    pSW_Stack[handle] = new SRS_Workspace;
    pSW_Stack[handle]->Handle = handle;

    return handle;
}

void SRS_Base_Workspace::DeleteWS(int handle){
    if (handle < 0) return;
    if (handle >= pSW_StackSize) return;

    SRS_Workspace* pWS = pSW_Stack[handle];
    if (pWS != NULL) delete pWS;
    pSW_Stack[handle] = NULL;
}

SRS_Workspace* SRS_Base_Workspace::GetWS(int handle, int autoId){
    if (handle == autoId)
        return &gAuto;

    if (handle < 0) return NULL;
    if (handle >= pSW_StackSize) return NULL;

    return pSW_Stack[handle];
}

};

