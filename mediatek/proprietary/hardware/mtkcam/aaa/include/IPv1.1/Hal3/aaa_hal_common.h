/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein is
* confidential and proprietary to MediaTek Inc. and/or its licensors. Without
* the prior written permission of MediaTek inc. and/or its licensors, any
* reproduction, modification, use or disclosure of MediaTek Software, and
* information contained herein, in whole or in part, shall be strictly
* prohibited.
*
* MediaTek Inc. (C) 2010. All rights reserved.
*
* BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
* THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
* RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
* ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
* WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
* WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
* NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
* RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
* INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
* TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
* RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
* OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
* SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
* RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
* STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
* ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
* RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
* MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
* CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
* The following software/firmware and/or related documentation ("MediaTek
* Software") have been modified by MediaTek Inc. All revisions are subject to
* any receiver's applicable license agreements with MediaTek Inc.
*/

/**
* @file aaa_hal_common.h
* @brief Declarations of Abstraction of 3A Hal Class and Top Data Structures
*/

#ifndef __AAA_HAL_COMMON_V3_H__
#define __AAA_HAL_COMMON_V3_H__

#include <metadata/client/mtk_metadata_tag.h>
#include <utils/Vector.h>
#include <isp_tuning.h>
#include <string.h>
#include <vector>

#define HAL3A_SETBIT(a, mask, value) (a) = (((a) & ~(mask)) | ((value) & (mask)))

#define HAL_FLG_DUMMY (1<<0)
#define HAL_FLG_DUMMY2 (1<<2)

using namespace NSIspTuning;

namespace NS3Av3
{
struct TuningParam
{
    void* pRegBuf;
    void* pLsc2Buf;
};

typedef enum
{
    ESensorDevId_Main         = 0x01,
    ESensorDevId_Sub          = 0x02,
    ESensorDevId_MainSecond   = 0x04,
    ESensorDevId_Main3D       = 0x05,
    ESensorDevId_Atv          = 0x08,
}   ESensorDevId_T;

enum ESensorType_T {
    ESensorType_RAW = 0,
    ESensorType_YUV = 1
};
/**
 * @brief 3A commands
 */
enum ECmd_T {
     ECmd_CameraPreviewStart,
     ECmd_CameraPreviewEnd,
     ECmd_PrecaptureStart,
     ECmd_PrecaptureEnd,
     ECmd_CaptureStart,
     ECmd_CaptureEnd,
     ECmd_RecordingStart,
     ECmd_RecordingEnd,
     ECmd_Update,
     // internal command
     ECmd_Init,
     ECmd_Uninit,
     ECmd_AFUpdate, // sync with AF done
     ECmd_AFStart,
     ECmd_AFEnd,
     ECmd_AFTrigger

};

/*******************************************************************************
* Sensor Mode
*******************************************************************************/
enum EQueryType_T{
    EQueryType_Init,
    EQueryType_Effect,
    EQueryType_AWB,
    EQueryType_AF
};

enum E3ACaptureMode_T
{
    ECapMode_P2_Cal     = (1<<0),
    ECapMode_P2_Set     = (1<<1)
};

enum E3APreviewMode_T
{
    EPv_Normal    = 0,
    EPv_Video
};

enum ECaptureType_T
{
    ECapType_SingleCapture = 0,
    ECapType_MultiCapture
};

typedef struct
{
    MINT64 iYvalue;
    MINT32 i4ISO;
    MINT32 i4IsAEStable;
    MINT32 i4SceneLV;
    MINT32 ishutterValue;
    MUINT8 aeBlockV[25];
    MINT32 i4IsFlashFrm;
    MINT32 i4AEBlockAreaYCnt;
    MUINT8 *pAEBlockAreaYvalue;
    MINT32 i4IsAELocked;
} AE2AFInfo_T;

struct FrameOutputParam_T
{
    MUINT32 u4AEIndex;
    MUINT32 u4FRameRate_x10;     // 10 base frame rate
    MUINT32 u4PreviewShutterSpeed_us;    // micro second
    MUINT32 u4PreviewSensorGain_x1024;  // 1024 base
    MUINT32 u4PreviewISPGain_x1024;       // 1024 base
    MUINT32 u4RealISOValue;
    MUINT32 u4CapShutterSpeed_us;    // micro second
    MUINT32 u4CapSensorGain_x1024;  // 1024 base
    MUINT32 u4CapISPGain_x1024;       // 1024 base
    MINT32 i4BrightValue_x10;       // 10 base brightness value
    MINT32 i4ExposureValue_x10;       // 10 base exposure value
    MINT32 i4LightValue_x10;       // 10 base lumince value
    MINT16 i2FlareOffset;              // 12 bit domain
    MINT16 i2FlareGain;                // 9 base gain
};

struct Hal3A_HDROutputParam_T
{
    MUINT32 u4OutputFrameNum;     // Output frame number (2 or 3)
    MUINT32 u4FinalGainDiff[2];   // 1x=1024; [0]: Between short exposure and 0EV; [1]: Between 0EV and long exposure
    MUINT32 u4TargetTone; //Decide the curve to decide target tone
};
/**
 * @brief 3A parameters for capture
 */
struct CaptureParam_T
{
    MUINT32 u4ExposureMode;  //0: exp. time, 1: exp. line
    MUINT32 u4Eposuretime;   //!<: Exposure time in us
    MUINT32 u4AfeGain;       //!<: sensor gain
    MUINT32 u4IspGain;       //!<: raw gain
    MUINT32 u4RealISO;       //!<: Real ISO speed
    MUINT32 u4FlareOffset;
    MUINT32 u4FlareGain;     // 512 is 1x
    MINT32  i4LightValue_x10;// 10 base LV value
    MINT32  i4YuvEvIdx;
    CaptureParam_T()
        : i4YuvEvIdx(0)
    {}
};

struct RequestSet_T
 {
     std::vector<MINT32> vNumberSet;
 };

struct FeatureParam_T {
    MBOOL   bExposureLockSupported;
    MBOOL   bAutoWhiteBalanceLockSupported;
    MUINT32 u4MaxFocusAreaNum;
    MUINT32 u4MaxMeterAreaNum;
    MBOOL   bEnableDynamicFrameRate;
    MINT32 i4MaxLensPos;
    MINT32 i4MinLensPos;
    MINT32 i4AFBestPos;
    MUINT32 u4FocusLength_100x;
    MINT32 u4PureRawInterval;
};

struct CameraArea_T {
    MINT32 i4Left;
    MINT32 i4Top;
    MINT32 i4Right;
    MINT32 i4Bottom;
    MINT32 i4Weight;
};

struct ScaleCropedArea_T {
    MINT32 i4Xoffset;
    MINT32 i4Yoffset;
    MINT32 i4Xwidth;
    MINT32 i4Yheight;
};


struct ExpSettingParam_T {
    MINT32 u4AOEMode;
    MUINT32 u4MaxSensorAnalogGain; // 1x=1024
    MUINT32 u4MaxAEExpTimeInUS;    // unit: us
    MUINT32 u4MinAEExpTimeInUS;    // unit: us
    MUINT32 u4ShutterLineTime;     // unit: 1/1000 us
    MUINT32 u4MaxAESensorGain;     // 1x=1024
    MUINT32 u4MinAESensorGain;     // 1x=1024
    MUINT32 u4ExpTimeInUS0EV;      // unit: us
    MUINT32 u4SensorGain0EV;       // 1x=1024
    MUINT32 u4ISOValue;       // real ISO value
    MUINT8  u1FlareOffset0EV;
    MINT32     i4GainBase0EV;            // AOE application for LE calculation
    MINT32     i4LE_LowAvg;            // AOE application for LE calculation, def: 0 ~ 39 avg
    MINT32     i4SEDeltaEVx100;        // AOE application for SE calculation
    MBOOL bDetectFace;               // detect face or not
    MUINT32 u4Histogram[128];
    MUINT32 u4FlareHistogram[128];
    MVOID* pPLineAETable;
    MINT32 i4aeTableCurrentIndex;
};

#define MAX_FOCUS_AREAS  9

struct CameraFocusArea_T {
    CameraArea_T rAreas[MAX_FOCUS_AREAS];
    MUINT32 u4Count;
};

#define MAX_METERING_AREAS 9

struct CameraMeteringArea_T {
    CameraArea_T rAreas[MAX_METERING_AREAS];
    MUINT32 u4Count;
};

// 3A ASD info
struct ASDInfo_T {
    MINT32 i4AELv_x10;          // AE Lv
    MBOOL  bAEBacklit;          // AE backlit condition
    MBOOL  bAEStable;           // AE stable
    MINT16 i2AEFaceDiffIndex;   // Face AE difference index with central weighting
    MINT32 i4AWBRgain_X128;     // AWB Rgain
    MINT32 i4AWBBgain_X128;     // AWB Bgain
    MINT32 i4AWBRgain_D65_X128; // AWB Rgain (D65; golden sample)
    MINT32 i4AWBBgain_D65_X128; // AWB Bgain (D65; golden sample)
    MINT32 i4AWBRgain_CWF_X128; // AWB Rgain (CWF; golden sample)
    MINT32 i4AWBBgain_CWF_X128; // AWB Bgain (CWF; golden sample)
    MBOOL  bAWBStable;          // AWB stable
    MINT32 i4AFPos;             // AF position
    MVOID* pAFTable;            // Pointer to AF table
    MINT32 i4AFTableOffset;     // AF table offset
    MINT32 i4AFTableMacroIdx;   // AF table macro index
    MINT32 i4AFTableIdxNum;     // AF table total index number
    MBOOL  bAFStable;           // AF stable
};

// LCE Info
struct LCEInfo_T {
    MINT32 i4NormalAEidx;    // gain >= 4x AE Pline table index at 30fps
    MINT32 i4LowlightAEidx;  // gain max AE Pline table index at 30fps
    MINT32 i4AEidxCur;          // AE current frame Pline table index
    MINT32 i4AEidxNext;        // AE next frame Pline table index
};

typedef enum
{
    E_AE_PRECAPTURE_IDLE,
    E_AE_PRECAPTURE_START
} EAePreCapture_T;

///HQC

enum E_CAPTURE_TYPE
{
    E_CAPTURE_NORMAL = 0,
    E_CAPTURE_HIGH_QUALITY_CAPTURE
} ;

struct SMVR_GAIN_DELAY_T {
    MUINT32 u4SensorGain_Delay;
    MUINT32 u4SutterGain_Delay;
    MUINT32 u4ISPGain_Delay;
};

struct CapParam_T
{
    MUINT32 u4CapType;
    CapParam_T()
        :u4CapType(E_CAPTURE_NORMAL){}
};

/**
 * @brief 3A parameters
 */
// 3A parameters
struct Param_T {
    // DEFAULT DEFINITION CATEGORY ( ordered by SDK )
    MUINT32 u4MagicNum;
    MUINT32 u4MagicNumCur;
    MUINT32 u4HalFlag;
    MUINT8  u1ControlMode;
    MUINT8  u1PrecapTrig;
    MINT32  i4MinFps;
    MINT32  i4MaxFps;
    MUINT32 u4AeMode;
    MUINT32 u4AwbMode;
    MUINT32 u4EffectMode;
    MUINT32 u4AntiBandingMode;
    MUINT32 u4SceneMode;
    MUINT32 u4StrobeMode;
    MINT32  i4ExpIndex;
    MFLOAT  fExpCompStep;

    // NEWLY-ADDED CATEGORY
    MUINT32 u4CamMode;   //Factory, ENG, normal
    MUINT32 u4ShotMode;
    MUINT32 u4CapType;  //refer to ECaptureType_T

    // MTK DEFINITION CATEGORY
    MUINT32 u4IsoSpeedMode;
    MINT32 i4BrightnessMode;
    MINT32 i4HueMode;
    MINT32 i4SaturationMode;
    MINT32 i4ContrastMode;
    MINT32 i4EdgeMode;
    MUINT32 u4AeMeterMode;
    MINT32  i4RotateDegree;

    //EAePreCapture_T eAePreCapture;
    //MBOOL   bHistogramMode;
    MBOOL   bIsAELock;
    MBOOL   bIsAWBLock;

    // flash for engineer mode
    MINT32  i4PreFlashDuty;
    MINT32  i4PreFlashStep;
    MINT32  i4MainFlashDuty;
    MINT32  i4MainFlashStep;

    // shading
    MUINT8  u1ShadingMode;
    MUINT8  u1ShadingMapMode;
    MUINT8  u1ShadingMapXGrid;
    MUINT8  u1ShadingMapYGrid;

    // manual sensor
    MINT64  i8FrameDuration;  // naro sec
    MINT64  i8ExposureTime;   // naro sec
    MINT32  i4Sensitivity;    // ISO value
    MUINT8  u1BlackLvlLock;
    MUINT8  u1RollingShutterSkew;

    // color correction
    MUINT8  u1ColorCorrectMode;
    MFLOAT  fColorCorrectGain[4];
    MFLOAT  fColorCorrectMat[9];

    // edge & NR
    MUINT8  u1EdgeMode;
    MUINT8  u1NRMode;

    // tonemap
    MUINT8  u1TonemapMode;
    android::Vector<MFLOAT> vecTonemapCurveBlue;
    android::Vector<MFLOAT> vecTonemapCurveGreen;
    android::Vector<MFLOAT> vecTonemapCurveRed;

    MINT32  i4RawType;
    MUINT8  u1CaptureIntent;
    MUINT8  u1IsGetExif;
    MBOOL   u1HQCapEnable;

    CameraMeteringArea_T    rMeteringAreas;
    ScaleCropedArea_T       rScaleCropRect;

    Param_T()
        : u4MagicNum(0)
        , u4MagicNumCur(0)
        , u4HalFlag(0)
        , u1ControlMode(MTK_CONTROL_MODE_AUTO)
        , u1PrecapTrig(MTK_CONTROL_AE_PRECAPTURE_TRIGGER_IDLE)
        , i4MinFps(5000)
        , i4MaxFps(30000)
        , u4AeMode(1)
        , u4AwbMode(1)
        , u4EffectMode(0)
        , u4AntiBandingMode(3)
        , u4SceneMode(0)
        , u4StrobeMode(0)
        , i4ExpIndex(0)
        , fExpCompStep(5)
        , u4CapType(ECapType_SingleCapture)
        , u4IsoSpeedMode(0)
        , i4BrightnessMode(1)
        , i4HueMode(1)
        , i4SaturationMode(1)
        , i4ContrastMode(1)
        , i4EdgeMode(1)
        , u4AeMeterMode (0)
        , i4RotateDegree(0)
        , bIsAELock(MFALSE)
        , bIsAWBLock(MFALSE)
        , i4PreFlashDuty(-1)
        , i4PreFlashStep(-1)
        , i4MainFlashDuty(-1)
        , i4MainFlashStep(-1)
        , u1ShadingMode(MTK_SHADING_MODE_FAST)
        , u1ShadingMapMode(MTK_STATISTICS_LENS_SHADING_MAP_MODE_OFF)
        , u1ShadingMapXGrid(1)
        , u1ShadingMapYGrid(1)
        , u1BlackLvlLock(MTK_BLACK_LEVEL_LOCK_OFF)
        , u1ColorCorrectMode(MTK_COLOR_CORRECTION_MODE_FAST)
        , u1EdgeMode(MTK_EDGE_MODE_FAST)
        , u1NRMode(MTK_NOISE_REDUCTION_MODE_FAST)
        , u1TonemapMode(MTK_TONEMAP_MODE_FAST)
        , i4RawType(NSIspTuning::ERawType_Proc)
        , u1CaptureIntent(MTK_CONTROL_CAPTURE_INTENT_PREVIEW)
        , u1IsGetExif(0)
        , u1HQCapEnable(MFALSE)
        , rMeteringAreas()
    {}
};

/**
 * @brief AF parameters
 */
// AF parameters
struct AF_Param_T {
    MUINT32 u4MagicNum;
    MUINT32 u4AfMode;
    MFLOAT  fFocusDistance;
    MBOOL   bIsSupportAndroidService;

    CameraFocusArea_T       rFocusAreas;
    CameraArea_T            rScaleCropArea;

    AF_Param_T()
        : u4MagicNum(0)
        , u4AfMode(4)
        , rFocusAreas()
        , rScaleCropArea()
        , bIsSupportAndroidService(MTRUE)
    {}
};


struct ParamIspProfile_T
{
    EIspProfile_T eIspProfile;
    MINT32        i4MagicNum;
    MINT32        i4MagicNumCur;
    MINT32        iEnableRPG;
    MINT32        iValidateOpt;
    MBOOL         bMainFlash;
    RequestSet_T  rRequestSet;
    enum
    {
        EParamValidate_None     = 0,
        EParamValidate_All      = 1,
        EParamValidate_P2Only   = 2
    };

    ParamIspProfile_T()
        : eIspProfile(EIspProfile_Preview)
        , i4MagicNum(0)
        , i4MagicNumCur(0)
        , iEnableRPG(0)
        , iValidateOpt(1)
        , bMainFlash(MFALSE)
        , rRequestSet()
    {}

    ParamIspProfile_T(
        EIspProfile_T eIspProfile_,
        MINT32        i4MagicNum_,
        MINT32        i4MagicNumCur_,
        MINT32        iEnableRPG_,
        MINT32        iValidateOpt_,
        RequestSet_T  rRequestSet_)
        : eIspProfile(eIspProfile_)
        , i4MagicNum(i4MagicNum_)
        , i4MagicNumCur(i4MagicNumCur_)
        , iEnableRPG(iEnableRPG_)
        , iValidateOpt(iValidateOpt_)
        , bMainFlash(MFALSE)
        , rRequestSet(rRequestSet_)
    {}
};

// max frames to queue DAF information
#define DAF_TBL_QLEN 32
#define DIST_TBL_QLEN 16
typedef struct
{
    MUINT32 frm_mun;
    MUINT8  is_learning;
    MUINT8  is_querying;
    MUINT8  af_valid;
    MUINT16 af_dac_index;
    MUINT16 af_confidence;
    MUINT16 af_win_start_x;
    MUINT16 af_win_start_y;
    MUINT16 af_win_end_x;
    MUINT16 af_win_end_y;
    MUINT16 daf_dac_index;
    MUINT16 daf_confidence;
    MUINT16 daf_distance;
} DAF_VEC_STRUCT;

typedef struct
{
    MUINT8   is_daf_run;
    MUINT32  is_query_happen;
    MUINT32  curr_p1_frm_num;
    MUINT32  curr_p2_frm_num;
    MUINT16  af_dac_min;
    MUINT16  af_dac_max;
    MUINT16  af_dac_start;
    MUINT32 dac[DIST_TBL_QLEN];
    MUINT32 dist[DIST_TBL_QLEN];

    DAF_VEC_STRUCT daf_vec[DAF_TBL_QLEN];
}DAF_TBL_STRUCT;

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
enum E3ACtrl_T
{
    E3ACtrl_Begin = 0,
    //shading
    E3ACtrl_SetShadingSdblkCfg      = 0x0001,
    E3ACtrl_SetShadingEngMode,
    E3ACtrl_SetShadingByp123,
    E3ACtrl_SetShadingOnOff,
    E3ACtrl_SetShadingTSFOnOff,
    E3ACtrl_SetShadingDynamic,
    E3ACtrl_SetShadingColorTemp,
//    E3ACtrl_SetShadingStrength,

    //AWB
    E3ACtrl_SetAwbBypCalibration    = 0x0100,

    //AE set
//    E3ACtrl_SetExposureParam        = 0x0200,
    E3ACtrl_SetHalHdr               = 0x0200,
    E3ACtrl_SetAETargetMode,
    E3ACtrl_SetAEIsoSpeedMode,
    E3ACtrl_SetAELimiterMode,
    E3ACtrl_SetAECamMode,
    E3ACtrl_EnableDisableAE,

    //AE get
    E3ACtrl_GetCurrentEV            = 0x0280,
    E3ACtrl_GetNVRAMParam,
    E3ACtrl_GetAEPLineTable,
    E3ACtrl_GetExposureInfo,
    E3ACtrl_GetExposureParam,
    E3ACtrl_GetInitExposureTime,
    E3ACtrl_GetAECapPLineTable,
    E3ACtrl_GetIsAEStable,
    E3ACtrl_GetRTParamsInfo,
    E3ACtrl_GetEvCapture,

    //ISP
//    E3ACtrl_ConvertToIspGamma
    E3ACtrl_GetIspGamma             = 0x0300,

    //Flash
    E3ACtrl_GetQuickCalibration     = 0x0400,
    E3ACtrl_EnableFlashQuickCalibration,

    //3A misc set
    //3A misc get
    E3ACtrl_GetAsdInfo              = 0x0480,
    E3ACtrl_GetExifInfo,
    E3ACtrl_GetSupportedInfo,

    //AF set
    E3ACtrl_SetAFMode               = 0x0500,
    E3ACtrl_SetAFMFPos,
    E3ACtrl_SetAFFullScanStep,
    E3ACtrl_SetAFCamMode,

    //AF get
    E3ACtrl_GetAFDAFTable           = 0x0600,

    //Flow control set
    E3ACtrl_Enable3ASetParams       = 0x1000,
    E3ACtrl_SetOperMode             = 0x1001,
    //Flow control get
    E3ACtrl_GetOperMode             = 0x2001,
    E3ACtrl_Num
};

struct E3ACtrl_ConvertToIspGamma_ARG_T
{
    MUINT32 u4NumPts;
    const MFLOAT* pPtPairs;
    MINT32* pOutGMA;

    E3ACtrl_ConvertToIspGamma_ARG_T()
        : pPtPairs(NULL), u4NumPts(0), pOutGMA(NULL) {}
    E3ACtrl_ConvertToIspGamma_ARG_T(const MFLOAT* _pPtPairs, MUINT32 _u4NumPts, MINT32* _pOutGMA)
        : pPtPairs(_pPtPairs), u4NumPts(_u4NumPts), pOutGMA(_pOutGMA) {}
};
}

#endif //__AAA_HAL_COMMON_V3_H__
