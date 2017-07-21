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

#ifndef ANDROID_SRS_PARAMS
#define ANDROID_SRS_PARAMS

namespace android {

struct SRS_ParamBlock;
struct SRS_ParamSource;
struct SRS_ParamInfo;
struct SRS_ParamIndexInfo;

typedef void (*Param_Set)(SRS_ParamBlock* pPB, SRS_ParamSource* pSrc, SRS_ParamInfo& info, const char* pValue);                 // Set bank.param to pValue
typedef const char* (*Param_Get)(SRS_ParamBlock* pPB, SRS_ParamSource* pSrc, SRS_ParamInfo& info);                              // Get value of bank.param
typedef void (*Param_GetIndexList)(SRS_ParamBlock* pPB, SRS_ParamSource* pSrc, SRS_ParamInfo& info, SRS_ParamIndexInfo& idxinfo);       // Get list of indexes legal for bank.param[x]
typedef void (*Bank_GetIndexList)(SRS_ParamBlock* pPB, SRS_ParamSource* pSrc, SRS_ParamInfo& info, SRS_ParamIndexInfo& idxinfo);        // Get list of indexes legal for bank[X]

struct SRS_ParamSource {    // Implement for IO Operations
    void*                   pSourceToken;   // Misc additional data

    Param_Set               SetParam;
    Param_Get               GetParam;
    Param_GetIndexList      ListParam;
    Bank_GetIndexList       ListBank;
};

struct SRS_ParamInfo {
    int bank;               // Bank Index
    int param;              // Param Index

    const char* pbidx;      // Bank[Index] string (if found)
    int blen;               // Length of chars in String...

    const char* ppidx;      // Param[Index] string (if found)
    int plen;               // Length of chars in String...

    SRS_ParamInfo(){
        bank = -1;
        param = -1;
        pbidx = NULL;
        blen = 0;
        ppidx = NULL;
        plen = 0;
    }
};

struct SRS_ParamIndexInfo {
    int Count;
    const char** pList;
    const char* pNotice;    // For CFG writing...

    SRS_ParamIndexInfo(){
        Count = 0;
        pList = NULL;
        pNotice = NULL;
    }
};

struct SRS_ParamInfoCache {
    const char* pKey;
    int KeyLen;
    unsigned int KeyCRC;

    const char* pKeyIdx;
    int KeyIdxLen;

    void Clear(){
        pKey = NULL;
        KeyLen = 0;
        KeyCRC = 0;
        pKeyIdx = NULL;
        KeyIdxLen = 0;
    }

    SRS_ParamInfoCache(){ Clear(); }
};

enum SRS_ParamTypes {   // Any writable type must follow _CFG - readonly before _CFG
    SRS_PTYP_INFO = 0,  // Readonly - optional serialized
    SRS_PTYP_DEBUG,     // Readonly - debug serialized
    SRS_PTYP_CFG,       // Read/write - config serialized
    SRS_PTYP_PREF,      // Read/write - user-pref serialized
    SRS_PTYP_RUNTIME,   // Read/write - never serialized (often used to store things like setup paths, etc)
    SRS_PTYP_ALIAS      // Read/write - input serialized only (follows permission of type aliased)
};

enum SRS_ParamTypeFlags {
    SRS_PTFLAG_INFO = (1<<SRS_PTYP_INFO),
    SRS_PTFLAG_DEBUG = (1<<SRS_PTYP_DEBUG),
    SRS_PTFLAG_CFG = (1<<SRS_PTYP_CFG),
    SRS_PTFLAG_PREF = (1<<SRS_PTYP_PREF),
    SRS_PTFLAG_RUNTIME = (1<<SRS_PTYP_RUNTIME),
    SRS_PTFLAG_ALIAS = (1<<SRS_PTYP_ALIAS)
};

enum SRS_ParamFormats {
    SRS_PFMT_STATIC = -1,   // Info, etc
    SRS_PFMT_BOOL = 0,
    SRS_PFMT_INT,
    SRS_PFMT_FLOAT,
    SRS_PFMT_ENUM,
    SRS_PFMT_STRING,
    SRS_PFMT_INTARRAY,
    SRS_PFMT_HEX8ARRAY,
    SRS_PFMT_FLOATARRAY
};

struct SRS_Param {
    int EnumID;
    int Type;           // Propery, Info, etc
    int Format;         // Int, Float, Enum, etc
    float DefValue;
    float Min;
    float Max;
    const char* pKey;   // Param name for code
    const char* pName;  // Param name for humans
    const char* pInfo;  // Info for humans
    const char* pEnums; // a,b,c,d style for enum format - single int for 'array' formats
    unsigned int KeyCRC;    // Zero
};

struct SRS_ParamBank {
    int EnumID;
    int Index;  // Speeds using multiple IP Banks
    const char* pType;  // WOWHD, MAXV, Info, Routing, etc
    const char* pPrefix;
    const char* pInfo;
    SRS_Param* pParams;
    int ParamCount;
    unsigned int PrefixCRC; // Zero
    int PrefixLen;  // Zero
};

enum SRS_ParamBlockFlags {
    SRS_PBFS_FILLED = 0x0001,           // Initial dynamic pass on params data performed (queries for lib versions, defaults overrides, etc)
    SRS_PBFS_PARAMSCACHED = 0x0002      // String of all Params for passing to Java apps was created.
};

struct SRS_ParamBlock {
    int ConfigFlags;    // Always starts 0 - allows one-time config of a Param Set, even if a thread or usages causes a restart more than once per boot.
    SRS_ParamBank* pBanks;
    int BankCount;
    const char* pParamCache;

    void FillPreCalcs();
    SRS_Param* FindParam(const char* pKey, SRS_ParamInfo& info, SRS_ParamInfoCache* pCache);
    const char* GetParamCache();

    // IO Support
    bool ConfigRead(const char* pPath, SRS_ParamSource* pSource, uint32_t typeMask=0xFFFFFFFF);
    void ConfigWrite(const char* pPath, SRS_ParamSource* pSource, uint32_t typeMask=0xFFFFFFFF, const char* pFormat=NULL);
    bool ConfigParse(char* data, SRS_ParamSource* pSource, uint32_t typeMask=0xFFFFFFFF);

    // CRC Support
    static unsigned int CRC_Bank[256];
    static unsigned int CalcCRC(unsigned char* pData, unsigned int dataLen);
};

struct HELP_ParamTools {
    static int GetIntIndex(const char* pV, int len, int def);
};

struct HELP_ParamIn : public HELP_ParamTools {  // Limit the amount of duplicated string in/out processing we do...
    static bool GetBool(const char* pV);
    static int GetInt(const char* pV);
    static float GetFloat(const char* pV);
};

struct HELP_ParamOut : public HELP_ParamTools { // Limit the amount of duplicated string in/out processing we do...
    static char Workspace[512];
    static const char* FromBool(bool tV);
    static const char* FromInt(int tV);
    static const char* FromFloat(float tV);
};

};

#endif  // ANDROID_SRS_PARAMS

