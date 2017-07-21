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

char UPEQScratch[512];

#undef  LOG_TAG
#define LOG_TAG "SRS_Tech_UPEQ"

namespace android {

// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// =-=-=-=-=-=- External Interfacing =-=-=-=-=-=-
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

enum {
    UPEQ_Enabled,       // CFG
    UPEQ_PresetInt,
    UPEQ_IGain,
    UPEQ_OGain,
    UPEQ_BGain,
    UPEQ_Skip,
    UPEQ_PresetName,    // Preset
    UPEQ_UBand0,        // UserParams
    UPEQ_UBand1,
    UPEQ_UBand2,
    UPEQ_UBand3,
    UPEQ_DBand0,        // DefParams
    UPEQ_DBand1,
    UPEQ_DBand2,
    UPEQ_DBand3,
};

SRS_Param gUPEQ_Params_CFG[] = {
    { UPEQ_Enabled,         SRS_PTYP_PREF,  SRS_PFMT_BOOL,      0.0f,   0.0f,   1.0f,   "upeq_enable", "UserPEQ Toggle", "toggle", "", 0},
    { UPEQ_PresetInt,       SRS_PTYP_PREF,  SRS_PFMT_INT,       0.0f,   0.0f,   0.0f,   "upeq_int_preset", "UserPEQ Preset - Internal", "The active UPEQ preset", "", 0},
    { UPEQ_IGain,           SRS_PTYP_CFG,   SRS_PFMT_FLOAT,     1.0f,   0.0f,   1.0f,   "upeq_igain", "UserPEQ Input Gain", "gain", "", 0},
    { UPEQ_OGain,           SRS_PTYP_CFG,   SRS_PFMT_FLOAT,     1.0f,   0.0f,   4.0f,   "upeq_ogain", "UserPEQ Output Gain", "gain", "", 0},
    { UPEQ_BGain,           SRS_PTYP_CFG,   SRS_PFMT_FLOAT,     1.0f,   0.0f,   4.0f,   "upeq_bgain", "UserPEQ Bypass Gain", "gain", "", 0},
    { UPEQ_Skip,            SRS_PTYP_CFG,   SRS_PFMT_BOOL,      0.0f,   0.0f,   1.0f,   "upeq_skip", "Skips UserPEQ when true", "toggle", "", 0},
};

SRS_Param gUPEQ_Params_Preset[] = {
    { UPEQ_PresetName,      SRS_PTYP_CFG,   SRS_PFMT_STRING,    0.0f,   0.0f,   0.0f,   "upeq_presetname", "User-facing name of Preset", "", "", 0},
    { UPEQ_UBand0,          SRS_PTYP_PREF,  SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "upeq_uband0", "UserPEQ Band 0 User Tuning (CF Gain Q)", "", "3", 0},
    { UPEQ_UBand1,          SRS_PTYP_PREF,  SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "upeq_uband1", "UserPEQ Band 1", "", "3", 0},
    { UPEQ_UBand2,          SRS_PTYP_PREF,  SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "upeq_uband2", "UserPEQ Band 2", "", "3", 0},
    { UPEQ_UBand3,          SRS_PTYP_PREF,  SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "upeq_uband3", "UserPEQ Band 3", "", "3", 0},
    { UPEQ_DBand0,          SRS_PTYP_PREF,  SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "upeq_dband0", "UserPEQ Band 0 Default Tuning (CF Gain Q)", "", "3", 0},
    { UPEQ_DBand1,          SRS_PTYP_PREF,  SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "upeq_dband1", "UserPEQ Band 1", "", "3", 0},
    { UPEQ_DBand2,          SRS_PTYP_PREF,  SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "upeq_dband2", "UserPEQ Band 2", "", "3", 0},
    { UPEQ_DBand3,          SRS_PTYP_PREF,  SRS_PFMT_FLOATARRAY,    0.0f,   0.0f,   0.0f,   "upeq_dband3", "UserPEQ Band 3", "", "3", 0},

};

SRS_Param* SRS_GetBank_UserPEQ_CFG(int &paramCount){
    paramCount = sizeof(gUPEQ_Params_CFG)/sizeof(SRS_Param);
    return gUPEQ_Params_CFG;
}

SRS_Param* SRS_GetBank_UserPEQ_Preset(int &paramCount){
    paramCount = sizeof(gUPEQ_Params_Preset)/sizeof(SRS_Param);
    return gUPEQ_Params_Preset;
}

void SRS_SetParam_UserPEQ_CFG(SRS_Tech_UserPEQ_CFG *pCFG, SRS_Param *pParam, const char *pValue){
    HELP_ParamIn In;

    switch (pParam->EnumID){
        case UPEQ_Enabled: pCFG->Enabled = In.GetBool(pValue); break;
        case UPEQ_PresetInt: pCFG->Preset_Int = In.GetInt(pValue); break;
        case UPEQ_IGain: pCFG->IGain = In.GetFloat(pValue); break;
        case UPEQ_OGain: pCFG->OGain = In.GetFloat(pValue); break;
        case UPEQ_BGain: pCFG->BGain = In.GetFloat(pValue); break;
        case UPEQ_Skip: pCFG->Skip = In.GetBool(pValue); break;
    }
}

void SRS_SetParam_UserPEQ_Preset(SRS_Tech_UserPEQ_Preset *pCFG, SRS_Param *pParam, const char *pValue){
    HELP_ParamIn In;

    switch (pParam->EnumID){
        case UPEQ_PresetName:{
            if (pCFG->pName != NULL){ delete [] pCFG->pName; pCFG->pName = NULL; }
            int tLen = strlen(pValue);
            if (tLen > 0){
                pCFG->pName = new char[tLen+1];
                strcpy(pCFG->pName, pValue);
            }
        } break;

        case UPEQ_UBand0:
        case UPEQ_UBand1:
        case UPEQ_UBand2:
        case UPEQ_UBand3:
        case UPEQ_DBand0:
        case UPEQ_DBand1:
        case UPEQ_DBand2:
        case UPEQ_DBand3:{
            float* pP = NULL;
            if ((pParam->EnumID >= UPEQ_UBand0) && (pParam->EnumID <= UPEQ_UBand3)) pP = pCFG->UserParams[pParam->EnumID-UPEQ_UBand0];
            if ((pParam->EnumID >= UPEQ_DBand0) && (pParam->EnumID <= UPEQ_DBand3)) pP = pCFG->DefParams[pParam->EnumID-UPEQ_DBand0];

            if (pP != NULL){
                float tHold[3];
                float* pD = tHold;
                int i;

                for (i=0; i<3; i++) tHold[i] = pP[i];       // Fill with current values...
                if (sscanf(pValue, "%f,%f,%f", pD+0, pD+1, pD+2) == 3){
                    for (i=0; i<3; i++) pP[i] = tHold[i];
                }
            }
        } break;
    }
}

const char* SRS_GetParam_UserPEQ_CFG(SRS_Tech_UserPEQ_CFG *pCFG, SRS_Param *pParam){
    HELP_ParamOut Out;

    switch (pParam->EnumID){
        case UPEQ_Enabled: return Out.FromBool(pCFG->Enabled);
        case UPEQ_PresetInt: return Out.FromBool(pCFG->Preset_Int);
        case UPEQ_IGain: return Out.FromFloat(pCFG->IGain);
        case UPEQ_OGain: return Out.FromFloat(pCFG->OGain);
        case UPEQ_BGain: return Out.FromFloat(pCFG->BGain);
        case UPEQ_Skip: return Out.FromBool(pCFG->Skip);
    }

    return "";
}

const char* SRS_GetParam_UserPEQ_Preset(SRS_Tech_UserPEQ_Preset *pCFG, SRS_Param *pParam){
    HELP_ParamOut Out;

    switch (pParam->EnumID){
        case UPEQ_PresetName:{
            UPEQScratch[0] = 0;
            if (pCFG->pName != NULL) strncpy(UPEQScratch, pCFG->pName, 64);
            return UPEQScratch;
        } break;

        case UPEQ_UBand0:
        case UPEQ_UBand1:
        case UPEQ_UBand2:
        case UPEQ_UBand3:
        case UPEQ_DBand0:
        case UPEQ_DBand1:
        case UPEQ_DBand2:
        case UPEQ_DBand3:{
            float* pP = NULL;
            if ((pParam->EnumID >= UPEQ_UBand0) && (pParam->EnumID <= UPEQ_UBand3)) pP = pCFG->UserParams[pParam->EnumID-UPEQ_UBand0];
            if ((pParam->EnumID >= UPEQ_DBand0) && (pParam->EnumID <= UPEQ_DBand3)) pP = pCFG->DefParams[pParam->EnumID-UPEQ_DBand0];

            if (pP != NULL){
                sprintf(UPEQScratch, "%f,%f,%f", pP[0], pP[1], pP[2]);
                return UPEQScratch;
            }
        } break;
    }

    return "";
}

extern void SRS_Default_UserPEQ_CFG(SRS_Tech_UserPEQ_CFG *pCFG){
    pCFG->IGain = 1.0f;
    pCFG->OGain = 1.0f;
    pCFG->BGain = 1.0f;

    pCFG->Enabled = false;
    pCFG->Preset_Int = -1;

    pCFG->Skip = 0;
}

extern void SRS_Default_UserPEQ_Preset(SRS_Tech_UserPEQ_Preset *pCFG){
    pCFG->pName = NULL;
    memset(pCFG->UserParams, 0, sizeof(pCFG->UserParams));
    memset(pCFG->DefParams, 0, sizeof(pCFG->DefParams));
}

void SRS_Apply_UserPEQ_CFG(SRS_Tech_TruEQ *pTruEQ, SRS_Tech_UserPEQ_CFG *pCFG){
    pTruEQ->IGain = pCFG->IGain;
    pTruEQ->OGain = pCFG->OGain;
    pTruEQ->BGain = pCFG->BGain;
    pTruEQ->Skip = pCFG->Skip;

    if (pCFG->Enabled == false){
        pTruEQ->LEnable = false;
        pTruEQ->REnable = false;
    }
}

void SRS_Apply_UserPEQ_Preset(SRS_Tech_TruEQ *pTruEQ, SRS_Tech_UserPEQ_Preset *pCFG){
    bool tActive = false;

    int i;
    for (i=0; i<4; i++){
        bool tBActive = false;
        if (pCFG->UserParams[i][1] != 0.0f) tBActive = true;
        if (tBActive == true) tActive = true;

        pTruEQ->LBands[i] = tBActive;
        pTruEQ->RBands[i] = tBActive;

        pTruEQ->Params[i][0] = pCFG->UserParams[i][0];
        pTruEQ->Params[i][1] = pCFG->UserParams[i][1];
        pTruEQ->Params[i][2] = pCFG->UserParams[i][2];
        pTruEQ->Params[i+4][0] = pCFG->UserParams[i][0];
        pTruEQ->Params[i+4][1] = pCFG->UserParams[i][1];
        pTruEQ->Params[i+4][2] = pCFG->UserParams[i][2];
    }

    pTruEQ->LEnable = tActive;
    pTruEQ->REnable = tActive;
}

};

