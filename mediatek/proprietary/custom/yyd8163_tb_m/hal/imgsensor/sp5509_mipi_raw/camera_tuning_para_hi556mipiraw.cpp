#include <utils/Log.h>
#include <fcntl.h>
#include <math.h>
#include <cstring>
#include "camera_custom_nvram.h"
#include "camera_custom_sensor.h"
#include "image_sensor.h"
#include "kd_imgsensor_define.h"
#include "camera_AE_PLineTable_hi556mipiraw.h"
#include "camera_info_hi556mipiraw.h"
#include "camera_custom_AEPlinetable.h"
#include "camera_custom_tsf_tbl.h"
const NVRAM_CAMERA_ISP_PARAM_STRUCT CAMERA_ISP_DEFAULT_VALUE =
{{
    //Version
    Version: NVRAM_CAMERA_PARA_FILE_VERSION,
    //SensorId
    SensorId: SENSOR_ID,
    ISPComm:{
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        }
    },
    ISPPca: {
#include INCLUDE_FILENAME_ISP_PCA_PARAM
    },
    ISPRegs:{
#include INCLUDE_FILENAME_ISP_REGS_PARAM
    },
    ISPMfbMixer:{{
    {
      0x00000000, 0x00000000
    },
    {
      0x00000000, 0x00000000
    },
    {
      0x00000000, 0x00000000
    },
    {
      0x00000000, 0x00000000
    },
    {
      0x00000000, 0x00000000
    },
    {
      0x00000000, 0x00000000
    },
    {
      0x00000000, 0x00000000
    }
    }},
    ISPMulitCCM:{
      Poly22:{
        84450,      // i4R_AVG
        19409,      // i4R_STD
        100150,      // i4B_AVG
        24154,      // i4B_STD
        0,      // i4R_MAX
        0,      // i4R_MIN
        0,      // i4G_MAX
        0,      // i4G_MIN
        0,      // i4B_MAX
        0,      // i4B_MIN
        {  // i4P00[9]
            4555000, -1730000, -265000, -817500, 3662500, -285000, -10000, -2417500, 4990000
        },
        {  // i4P10[9]
            954799, -876987, -77812, -77985, 71852, 6133, 31220, 1763353, -1802378
        },
        {  // i4P01[9]
            1020852, -1007407, -13445, -215782, -114312, 330093, 16538, 612596, -633269
        },
        {  // i4P20[9]
            0, 0, 0, 0, 0, 0, 0, 0, 0
        },
        {  // i4P11[9]
            0, 0, 0, 0, 0, 0, 0, 0, 0
        },
        {  // i4P02[9]
            0, 0, 0, 0, 0, 0, 0, 0, 0
        }
      },

      AWBGain:{
        // Strobe
        {
          512,    // i4R
          512,    // i4G
          512    // i4B
        },
        // A
        {
          608,    // i4R
          512,    // i4G
          1214    // i4B
        },
        // TL84
        {
          793,    // i4R
          512,    // i4G
          1068    // i4B
        },
        // CWF
        {
          908,    // i4R
          512,    // i4G
          1070    // i4B
        },
        // D65
        {
          1069,    // i4R
          512,    // i4G
          654    // i4B
        },
        // Reserved 1
        {
          512,    // i4R
          512,    // i4G
          512    // i4B
        },
        // Reserved 2
        {
          512,    // i4R
          512,    // i4G
          512    // i4B
        },
        // Reserved 3
        {
          512,    // i4R
          512,    // i4G
          512    // i4B
        }
      },
      Weight:{
        1, // Strobe
        1, // A
        1, // TL84
        1, // CWF
        1, // D65
        1, // Reserved 1
        1, // Reserved 2
        1  // Reserved 3
      }
    },

    //bInvokeSmoothCCM
    bInvokeSmoothCCM: MTRUE,

    DngMetadata:{
      0, // i4RefereceIlluminant1
      0, // i4RefereceIlluminant2
      rNoiseProfile:{
        {
          S:{
            0.000000,      // a
            0.000000       // b
          },
          O:{
            0.000000,      // a
            0.000000       // b
          }
        },
        {
          S:{
            0.000000,      // a
            0.000000       // b
          },
          O:{
            0.000000,      // a
            0.000000       // b
          }
        },
        {
          S:{
            0.000000,      // a
            0.000000       // b
          },
          O:{
            0.000000,      // a
            0.000000       // b
          }
        },
        {
          S:{
            0.000000,      // a
            0.000000       // b
          },
          O:{
            0.000000,      // a
            0.000000       // b
          }
        }
      }
  }
}};

const NVRAM_CAMERA_3A_STRUCT CAMERA_3A_NVRAM_DEFAULT_VALUE =
{
    NVRAM_CAMERA_3A_FILE_VERSION, // u4Version
    SENSOR_ID, // SensorId

    // AE NVRAM
    {
        // rDevicesInfo
        {
            1280,    // u4MinGain, 1024 base = 1x
            15872,    // u4MaxGain, 16x
            100,    // u4MiniISOGain, ISOxx  
            64,    // u4GainStepUnit, 1x/8 
            16001,    // u4PreExpUnit 
            30,    // u4PreMaxFrameRate
            16001,    // u4VideoExpUnit  
            30,    // u4VideoMaxFrameRate 
            1024,    // u4Video2PreRatio, 1024 base = 1x 
            16001,    // u4CapExpUnit 
            30,    // u4CapMaxFrameRate
            1024,    // u4Cap2PreRatio, 1024 base = 1x
            16001,    // u4Video1ExpUnit
            120,    // u4Video1MaxFrameRate
            1024,    // u4Video12PreRatio, 1024 base = 1x
            16001,    // u4Video2ExpUnit
            30,    // u4Video2MaxFrameRate
            1024,    // u4Video22PreRatio, 1024 base = 1x
            26091,    // u4Custom1ExpUnit
            12,    // u4Custom1MaxFrameRate
            1024,    // u4Custom12PreRatio, 1024 base = 1x
            26091,    // u4Custom2ExpUnit
            12,    // u4Custom2MaxFrameRate
            1024,    // u4Custom22PreRatio, 1024 base = 1x
            26091,    // u4Custom3ExpUnit
            12,    // u4Custom3MaxFrameRate
            1024,    // u4Custom32PreRatio, 1024 base = 1x
            26091,    // u4Custom4ExpUnit
            12,    // u4Custom4MaxFrameRate
            1024,    // u4Custom42PreRatio, 1024 base = 1x
            26091,    // u4Custom5ExpUnit
            12,    // u4Custom5MaxFrameRate
            1024,    // u4Custom52PreRatio, 1024 base = 1x
            28,    // u4LensFno, Fno = 2.8
            350    // u4FocusLength_100x
        },
        // rHistConfig
        {
            4,    // u4HistHighThres
            40,    // u4HistLowThres
            2,    // u4MostBrightRatio
            1,    // u4MostDarkRatio
            160,    // u4CentralHighBound
            20,    // u4CentralLowBound
            {240, 230, 220, 210, 200},    // u4OverExpThres[AE_CCT_STRENGTH_NUM] 
            {62, 70, 82, 108, 141},    // u4HistStretchThres[AE_CCT_STRENGTH_NUM] 
            {18, 22, 26, 30, 34}    // u4BlackLightThres[AE_CCT_STRENGTH_NUM] 
        },
        // rCCTConfig
        {
            TRUE,    // bEnableBlackLight
            TRUE,    // bEnableHistStretch
            TRUE,    // bEnableAntiOverExposure
            TRUE,    // bEnableTimeLPF
            TRUE,    // bEnableCaptureThres
            TRUE,    // bEnableVideoThres
            TRUE,    // bEnableVideo1Thres
            TRUE,    // bEnableVideo2Thres
            TRUE,    // bEnableCustom1Thres
            TRUE,    // bEnableCustom2Thres
            TRUE,    // bEnableCustom3Thres
            TRUE,    // bEnableCustom4Thres
            TRUE,    // bEnableCustom5Thres
            TRUE,    // bEnableStrobeThres
            47,    // u4AETarget
            47,    // u4StrobeAETarget
            50,    // u4InitIndex
            4,    // u4BackLightWeight
            32,    // u4HistStretchWeight
            4,    // u4AntiOverExpWeight
            2,    // u4BlackLightStrengthIndex
            2,    // u4HistStretchStrengthIndex
            2,    // u4AntiOverExpStrengthIndex
            2,    // u4TimeLPFStrengthIndex
            {1, 3, 5, 7, 8},    // u4LPFConvergeTable[AE_CCT_STRENGTH_NUM] 
            90,    // u4InDoorEV = 9.0, 10 base 
            -6,    // i4BVOffset delta BV = value/10 
            64,    // u4PreviewFlareOffset
            64,    // u4CaptureFlareOffset
            3,    // u4CaptureFlareThres
            64,    // u4VideoFlareOffset
            3,    // u4VideoFlareThres
            64,    // u4CustomFlareOffset
            3,    // u4CustomFlareThres
            64,    // u4StrobeFlareOffset
            3,    // u4StrobeFlareThres
            160,    // u4PrvMaxFlareThres
            0,    // u4PrvMinFlareThres
            160,    // u4VideoMaxFlareThres
            0,    // u4VideoMinFlareThres
            18,    // u4FlatnessThres    // 10 base for flatness condition.
            75,    // u4FlatnessStrength
            //rMeteringSpec
            {
                //rHS_Spec
                {
                    TRUE,//bEnableHistStretch           // enable histogram stretch
                    1024,//u4HistStretchWeight          // Histogram weighting value
                    40,//u4Pcent                      // 
                    160,//u4Thd                        // 0~255
                    75, //74,//u4FlatThd                    // 0~255
                    120,//u4FlatBrightPcent
                    120,//u4FlatDarkPcent
                    //sFlatRatio
                    {
                        1000,  //i4X1
                        1024,  //i4Y1
                        2400,  //i4X2
                        0  //i4Y2
                    },
                    TRUE, //bEnableGreyTextEnhance
                    1800, //u4GreyTextFlatStart, > sFlatRatio.i4X1, < sFlatRatio.i4X2
                    {
                        10,  //i4X1
                        1024,  //i4Y1
                        80,  //i4X2
                        0  //i4Y2
                    }
                },
                //rAOE_Spec
                {
                    TRUE, //bEnableAntiOverExposure
                    1024, //u4AntiOverExpWeight
                    10, //u4Pcent
                    200, //u4Thd
                    TRUE, //bEnableCOEP
                    1, //u4COEPcent
                    106, //u4COEThd
                    0, //u4BVCompRatio
                    //sCOEYRatio;     // the outer y ratio
                    {
                        23,  //i4X1
                        1024,  //i4Y1
                        47,  //i4X2
                        0  //i4Y2
                    },
                    //sCOEDiffRatio;  // inner/outer y difference ratio
                    {
                        1500,  //i4X1
                        0,  //i4Y1
                        2100,  //i4X2
                        1024  //i4Y2
                    }
                },
                //rABL_Spec
                {
                    TRUE,//bEnableBlackLight
                    1024,//u4BackLightWeight
                    400,//u4Pcent
                    28,//u4Thd
                    255, // center luminance
                    256, // final target limitation, 256/128 = 2x
                    //sFgBgEVRatio
                    {
                        2200,  //i4X1
                        0,  //i4Y1
                        4000,  //i4X2
                        1024  //i4Y2
                    },
                    //sBVRatio
                    {
                        3800,  //i4X1
                        0,  //i4Y1
                        5000,  //i4X2
                        1024  //i4Y2
                    }
                },
                //rNS_Spec
                {
                    TRUE, // bEnableNightScene
                    5,//u4Pcent
                    160,//u4Thd
                    72,//u4FlatThd
                    200,//u4BrightTonePcent
                    86,//u4BrightToneThd
                    500,//u4LowBndPcent
                    5,//u4LowBndThd
                    23,//u4LowBndThdLimit
                    50,//u4FlatBrightPcent
                    300,//u4FlatDarkPcent
                    //sFlatRatio
                    {
                        1200,  //i4X1
                        1024,  //i4Y1
                        2400,  //i4X2
                        0  //i4Y2
                    },
                    //sBVRatio
                    {
                        -500,  //i4X1
                        1024,  //i4Y1
                        3000,  //i4X2
                        0  //i4Y2
                    },
                    TRUE, // bEnableNightSkySuppresion
                    //sSkyBVRatio
                    {
                        -4000,  //i4X1
                        1024,  //i4Y1
                        -2000,  //i4X2
                        0  //i4Y2
                    }
                },
                // rTOUCHFD_Spec
                {
                    40, //uMeteringYLowBound;
                    50, //uMeteringYHighBound;
                    40, //uFaceYLowBound;
                    50, //uFaceYHighBound;
                    3, //uFaceCentralWeight;
                    120, //u4MeteringStableMax;
                    80, //u4MeteringStableMin;
                }
            }, //End rMeteringSpec
            // rFlareSpec
            {
                {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, //uPrvFlareWeightArr[16];
                {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, //uVideoFlareWeightArr[16];
                96,                                               //u4FlareStdThrHigh;
                48,                                               //u4FlareStdThrLow;
                0,                                               //u4PrvCapFlareDiff;
                4,                                               //u4FlareMaxStepGap_Fast;
                0,                                               //u4FlareMaxStepGap_Slow;
                1800,                                               //u4FlarMaxStepGapLimitBV;
                0,                                               //u4FlareAEStableCount;
            },
            //rAEMoveRatio =
            {
                100, //u4SpeedUpRatio
                100, //u4GlobalRatio
                190, //u4Bright2TargetEnd
                20, //u4Dark2TargetStart
                90, //u4B2TEnd
                70, //u4B2TStart
                60, //u4D2TEnd
                90, //u4D2TStart
            },
            //rAEVideoMoveRatio =
            {
                100, //u4SpeedUpRatio
                100, //u4GlobalRatio
                150, //u4Bright2TargetEnd
                20, //u4Dark2TargetStart
                90, //u4B2TEnd
                10, //u4B2TStart
                10, //u4D2TEnd
                90, //u4D2TStart
            },
            //rAEVideo1MoveRatio =
            {
                100, //u4SpeedUpRatio
                100, //u4GlobalRatio
                150, //u4Bright2TargetEnd
                20, //u4Dark2TargetStart
                90, //u4B2TEnd
                10, //u4B2TStart
                10, //u4D2TEnd
                90, //u4D2TStart
            },
            //rAEVideo2MoveRatio =
            {
                100, //u4SpeedUpRatio
                100, //u4GlobalRatio
                150, //u4Bright2TargetEnd
                20, //u4Dark2TargetStart
                90, //u4B2TEnd
                10, //u4B2TStart
                10, //u4D2TEnd
                90, //u4D2TStart
            },
            //rAECustom1MoveRatio =
            {
                100, //u4SpeedUpRatio
                100, //u4GlobalRatio
                150, //u4Bright2TargetEnd
                20, //u4Dark2TargetStart
                90, //u4B2TEnd
                10, //u4B2TStart
                10, //u4D2TEnd
                90, //u4D2TStart
            },
            //rAECustom2MoveRatio =
            {
                100, //u4SpeedUpRatio
                100, //u4GlobalRatio
                150, //u4Bright2TargetEnd
                20, //u4Dark2TargetStart
                90, //u4B2TEnd
                10, //u4B2TStart
                10, //u4D2TEnd
                90, //u4D2TStart
            },
            //rAECustom3MoveRatio =
            {
                100, //u4SpeedUpRatio
                100, //u4GlobalRatio
                150, //u4Bright2TargetEnd
                20, //u4Dark2TargetStart
                90, //u4B2TEnd
                10, //u4B2TStart
                10, //u4D2TEnd
                90, //u4D2TStart
            },
            //rAECustom4MoveRatio =
            {
                100, //u4SpeedUpRatio
                100, //u4GlobalRatio
                150, //u4Bright2TargetEnd
                20, //u4Dark2TargetStart
                90, //u4B2TEnd
                10, //u4B2TStart
                10, //u4D2TEnd
                90, //u4D2TStart
            },
            //rAECustom5MoveRatio =
            {
                100, //u4SpeedUpRatio
                100, //u4GlobalRatio
                150, //u4Bright2TargetEnd
                20, //u4Dark2TargetStart
                90, //u4B2TEnd
                10, //u4B2TStart
                10, //u4D2TEnd
                90, //u4D2TStart
            },
            //rAEFaceMoveRatio =
            {
                100, //u4SpeedUpRatio
                100, //u4GlobalRatio
                190, //u4Bright2TargetEnd
                10, //u4Dark2TargetStart
                80, //u4B2TEnd
                30, //u4B2TStart
                20, //u4D2TEnd
                60, //u4D2TStart
            },
            //rAETrackingMoveRatio =
            {
                100, //u4SpeedUpRatio
                100, //u4GlobalRatio
                190, //u4Bright2TargetEnd
                10, //u4Dark2TargetStart
                80, //u4B2TEnd
                30, //u4B2TStart
                20, //u4D2TEnd
                60, //u4D2TStart
            },
            //rAEAOENVRAMParam =
            {
                1,      // i4AOEStrengthIdx: 0 / 1 / 2
                130,      // u4BVCompRatio
                {//rAEAOEAlgParam
                    {//rAEAOEAlgParam[0]
                        47,  //u4Y_Target
                        25,  //u4AOE_OE_percent
                        210,  //u4AOE_OEBound
                        10,  //u4AOE_DarkBound
                        950,  //u4AOE_LowlightPrecent
                        1,  //u4AOE_LowlightBound
                        145,  //u4AOESceneLV_L
                        180,  //u4AOESceneLV_H
                        40,  //u4AOE_SWHdrLE_Bound
                    },
                    {//rAEAOEAlgParam[1]
                        47,  //u4Y_Target
                        25,  //u4AOE_OE_percent
                        210,  //u4AOE_OEBound
                        15,  //u4AOE_DarkBound
                        950,  //u4AOE_LowlightPrecent
                        3,  //u4AOE_LowlightBound
                        145,  //u4AOESceneLV_L
                        180,  //u4AOESceneLV_H
                        40,  //u4AOE_SWHdrLE_Bound
                    },
                    {//rAEAOEAlgParam[2]
                        47,  //u4Y_Target
                        25,  //u4AOE_OE_percent
                        210,  //u4AOE_OEBound
                        25,  //u4AOE_DarkBound
                        950,  //u4AOE_LowlightPrecent
                        8,  //u4AOE_LowlightBound
                        145,  //u4AOESceneLV_L
                        180,  //u4AOESceneLV_H
                        40,  //u4AOE_SWHdrLE_Bound
                    }
                }
            }
        }
    },
    // AWB NVRAM
    {
        {
          {
            // AWB calibration data
            {
                // rUnitGain (unit gain: 1.0 = 512)
                {
                    0,    // i4R
                    0,    // i4G
                    0    // i4B
                },
                // rGoldenGain (golden sample gain: 1.0 = 512)
                {
                    0,    // i4R
                    0,    // i4G
                    0    // i4B
                },
                // rTuningUnitGain (Tuning sample unit gain: 1.0 = 512)
                {
                    0,    // i4R
                    0,    // i4G
                    0    // i4B
                },
                // rD65Gain (D65 WB gain: 1.0 = 512)
                {
                    1118,    // D65Gain_R
                    512,    // D65Gain_G
                    633    // D65Gain_B
                }
            },
            // Original XY coordinate of AWB light source
            {
                // Strobe
                {
                    0,    // i4X
                    0    // i4Y
                },
                // Horizon
                {
                    -357,    // OriX_Hor
                    -376    // OriY_Hor
                },
                // A
                {
                    -230,    // OriX_A
                    -390    // OriY_A
                },
                // TL84
                {
                    -81,    // OriX_TL84
                    -452    // OriY_TL84
                },
                // CWF
                {
                    -23,    // OriX_CWF
                    -504    // OriY_CWF
                },
                // DNP
                {
                    84,    // OriX_DNP
                    -389    // OriY_DNP
                },
                // D65
                {
                    210,    // OriX_D65
                    -367    // OriY_D65
                },
                // DF
                {
                    0,    // OriX_DF
                    0    // OriY_DF
                }
            },
            // Rotated XY coordinate of AWB light source
            {
                // Strobe
                {
                    0,    // i4X
                    0    // i4Y
                },
                // Horizon
                {
                    -382,    // RotX_Hor
                    -349    // RotY_Hor
                },
                // A
                {
                    -257,    // RotX_A
                    -372    // RotY_A
                },
                // TL84
                {
                    -112,    // RotX_TL84
                    -445    // RotY_TL84
                },
                // CWF
                {
                    -58,    // RotX_CWF
                    -500    // RotY_CWF
                },
                // DNP
                {
                    56,    // RotX_DNP
                    -393    // RotY_DNP
                },
                // D65
                {
                    183,    // RotX_D65
                    -380    // RotY_D65
                },
                // DF
                {
                    172,    // RotX_DF
                    -486    // RotY_DF
                }
            },
            // AWB gain of AWB light source
            {
                // Strobe 
                {
                    512,    // i4R
                    512,    // i4G
                    512    // i4B
                },
                // Horizon 
                {
                    526,    // AWBGAIN_HOR_R
                    512,    // AWBGAIN_HOR_G
                    1382    // AWBGAIN_HOR_B
                },
                // A 
                {
                    636,    // AWBGAIN_A_R
                    512,    // AWBGAIN_A_G
                    1185    // AWBGAIN_A_B
                },
                // TL84 
                {
                    846,    // AWBGAIN_TL84_R
                    512,    // AWBGAIN_TL84_G
                    1053    // AWBGAIN_TL84_B
                },
                // CWF 
                {
                    982,    // AWBGAIN_CWF_R
                    512,    // AWBGAIN_CWF_G
                    1045    // AWBGAIN_CWF_B
                },
                // DNP 
                {
                    971,    // AWBGAIN_DNP_R
                    512,    // AWBGAIN_DNP_G
                    774    // AWBGAIN_DNP_B
                },
                // D65 
                {
                    1118,    // AWBGAIN_D65_R
                    512,    // AWBGAIN_D65_G
                    633    // AWBGAIN_D65_B
                },
                // DF 
                {
                    512,    // AWBGAIN_DF_R
                    512,    // AWBGAIN_DF_G
                    512    // AWBGAIN_DF_B
                }
            },
            // Rotation matrix parameter
            {
                4,    // RotationAngle
                255,    // Cos
                18    // Sin
            },
            // Daylight locus parameter
            {
                -144,    // i4SlopeNumerator
                128    // i4SlopeDenominator
            },
            // Predictor gain
            {
                100, // i4PrefRatio100
                // DaylightLocus_L
                {
                    1088,    // i4R
                    530,    // i4G
                    653    // i4B
                },
                // DaylightLocus_H
                {
                    872,    // i4R
                    512,    // i4G
                    841    // i4B
                },
                // Temporal General
                {
                    1118,    // i4R
                    512,    // i4G
                    633    // i4B
                },
                // AWB LSC Gain
                {
                    931,        // i4R
                    512,        // i4G
                    781        // i4B
                }
            },
            // AWB light area
            {
                // Strobe:FIXME
                {
                    0,    // i4RightBound
                    0,    // i4LeftBound
                    0,    // i4UpperBound
                    0    // i4LowerBound
                },
                // Tungsten
                {
                    -170,    // TungRightBound
                    -782,    // TungLeftBound
                    -294,    // TungUpperBound
                    -409    // TungLowerBound
                },
                // Warm fluorescent
                {
                    -170,    // WFluoRightBound
                    -782,    // WFluoLeftBound
                    -409,    // WFluoUpperBound
                    -590    // WFluoLowerBound
                },
                // Fluorescent
                {
                    1,    // FluoRightBound
                    -170,    // FluoLeftBound
                    -320,    // FluoUpperBound
                    -450    // FluoLowerBound
                },
                // CWF
                {
                43,    // CWFRightBound
                -170,    // CWFLeftBound
                -450,    // CWFUpperBound
                -555    // CWFLowerBound
                },
                // Daylight
                {
                    213,    // DayRightBound
                    1,    // DayLeftBound
                    -320,    // DayUpperBound
                    -450    // DayLowerBound
                },
                // Shade
                {
                    543,    // ShadeRightBound
                    213,    // ShadeLeftBound
                    -320,    // ShadeUpperBound
                    -428    // ShadeLowerBound
                },
                // Daylight Fluorescent
                {
                    213,    // DFRightBound
                    43,    // DFLeftBound
                    -450,    // DFUpperBound
                    -555    // DFLowerBound
                }
            },
            // PWB light area
            {
                // Reference area
                {
                    543,    // PRefRightBound
                    -782,    // PRefLeftBound
                    0,    // PRefUpperBound
                    -590    // PRefLowerBound
                },
                // Daylight
                {
                    238,    // PDayRightBound
                    1,    // PDayLeftBound
                    -320,    // PDayUpperBound
                    -450    // PDayLowerBound
                },
                // Cloudy daylight
                {
                    338,    // PCloudyRightBound
                    163,    // PCloudyLeftBound
                    -320,    // PCloudyUpperBound
                    -450    // PCloudyLowerBound
                },
                // Shade
                {
                    438,    // PShadeRightBound
                    163,    // PShadeLeftBound
                    -320,    // PShadeUpperBound
                    -450    // PShadeLowerBound
                },
                // Twilight
                {
                    1,    // PTwiRightBound
                    -159,    // PTwiLeftBound
                    -320,    // PTwiUpperBound
                    -450    // PTwiLowerBound
                },
                // Fluorescent
                {
                    233,    // PFluoRightBound
                    -212,    // PFluoLeftBound
                    -330,    // PFluoUpperBound
                    -550    // PFluoLowerBound
                },
                // Warm fluorescent
                {
                    -157,    // PWFluoRightBound
                    -357,    // PWFluoLeftBound
                    -330,    // PWFluoUpperBound
                    -550    // PWFluoLowerBound
                },
                // Incandescent
                {
                    -157,    // PIncaRightBound
                    -357,    // PIncaLeftBound
                    -320,    // PIncaUpperBound
                    -450    // PIncaLowerBound
                },
                // Gray World
                {
                    5000,    // PGWRightBound
                    -5000,    // PGWLeftBound
                    5000,    // PGWUpperBound
                    -5000    // PGWLowerBound
                }
            },
            // PWB default gain	
            {
                // Daylight
                {
                    1039,    // PWB_Day_R
                    512,    // PWB_Day_G
                    699    // PWB_Day_B
                },
                // Cloudy daylight
                {
                    1225,    // PWB_Cloudy_R
                    512,    // PWB_Cloudy_G
                    578    // PWB_Cloudy_B
                },
                // Shade
                {
                    1304,    // PWB_Shade_R
                    512,    // PWB_Shade_G
                    538    // PWB_Shade_B
                },
                // Twilight
                {
                    810,    // PWB_Twi_R
                    512,    // PWB_Twi_G
                    931    // PWB_Twi_B
                },
                // Fluorescent
                {
                    981,    // PWB_Fluo_R
                    512,    // PWB_Fluo_G
                    877    // PWB_Fluo_B
                },
                // Warm fluorescent
                {
                    701,    // PWB_WFluo_R
                    512,    // PWB_WFluo_G
                    1291    // PWB_WFluo_B
                },
                // Incandescent
                {
                    647,    // PWB_Inca_R
                    512,    // PWB_Inca_G
                    1205    // PWB_Inca_B
                },
                // Gray World
                {
                    512,    // PWB_GW_R
                    512,    // PWB_GW_G
                    512    // PWB_GW_B
                }
            },
            // AWB preference color	
            {
                // Tungsten
                {
                    0,    // TUNG_SLIDER
                    6799    // TUNG_OFFS
                },
                // Warm fluorescent	
                {
                    0,    // WFluo_SLIDER
                    5820    // WFluo_OFFS
                },
                // Shade
                {
                    25,    // Shade_SLIDER
                    909    // Shade_OFFS
                },
                // Sunset Area
                {
                    98,   // i4Sunset_BoundXr_Thr
                    -393    // i4Sunset_BoundYr_Thr
                },
                // Sunset Vertex
                {
                    98,   // i4Sunset_BoundXr_Thr
                    -393    // i4Sunset_BoundYr_Thr
                },
                // Shade F Area
                {
                    -170,   // i4BoundXrThr
                    -430    // i4BoundYrThr
                },
                // Shade F Vertex
                {
                    -170,   // i4BoundXrThr
                    -461    // i4BoundYrThr
                },
                // Shade CWF Area
                {
                    -170,   // i4BoundXrThr
                    -504    // i4BoundYrThr
                },
                // Shade CWF Vertex
                {
                    -170,   // i4BoundXrThr
                    -530    // i4BoundYrThr
                },
                // Low CCT Area
                {
                    -402,   // i4BoundXrThr
                    316    // i4BoundYrThr
                },
                // Low CCT Vertex
                {
                    -402,   // i4BoundXrThr
                    316    // i4BoundYrThr
                }
            },
            // CCT estimation
            {
                // CCT
                {
                    2300,    // i4CCT[0]
                    2850,    // i4CCT[1]
                    3750,    // i4CCT[2]
                    5100,    // i4CCT[3]
                    6500     // i4CCT[4]
                },
                // Rotated X coordinate
                {
                -565,    // i4RotatedXCoordinate[0]
                -440,    // i4RotatedXCoordinate[1]
                -295,    // i4RotatedXCoordinate[2]
                -127,    // i4RotatedXCoordinate[3]
                0    // i4RotatedXCoordinate[4]
                }
                }
            },
            // Algorithm Tuning Paramter
            {
                // AWB Backup Enable
                0,

            // Daylight locus offset LUTs for tungsten
            {
                21, // i4Size: LUT dimension
                {0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500, 10000}, // i4LUTIn
                {0, 350, 800, 1222, 1444, 1667, 1889, 2111, 2333, 2556, 2778, 3000, 3222, 3444, 3667, 3889, 4111, 4333, 4556, 4778, 5000} // i4LUTOut
            },
            // Daylight locus offset LUTs for WF
            {
                21, // i4Size: LUT dimension
                {0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500, 10000}, // i4LUTIn
                {0, 350, 700, 1000, 1444, 1667, 1889, 2111, 2333, 2556, 2778, 3000, 3222, 3444, 3667, 3889, 4111, 4333, 4556, 4778, 5000} // i4LUTOut
            },
            // Daylight locus offset LUTs for Shade
            {
                21, // i4Size: LUT dimension
                {0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500, 10000}, // i4LUTIn
                {0, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500, 10000} // i4LUTOut
            },
            // Preference gain for each light source
            {
                {
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, 
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                }, // STROBE
                {
                    {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, 
                    {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, {505, 512, 515}
                }, // TUNGSTEN
                {
                    {512, 512, 520}, {512, 512, 520}, {512, 512, 520}, {512, 512, 520}, {512, 512, 520}, {512, 512, 520}, {512, 512, 520}, {512, 512, 520}, {512, 512, 520}, {512, 512, 520}, 
                    {512, 512, 520}, {512, 512, 520}, {512, 512, 520}, {512, 512, 520}, {512, 512, 520}, {512, 512, 520}, {512, 512, 520}, {512, 512, 520}, {512, 512, 520}
                }, // WARM F
                {
                    {498, 512, 525}, {498, 512, 525}, {498, 512, 525}, {498, 512, 525}, {498, 512, 525}, {498, 512, 525}, {498, 512, 525}, {498, 512, 525}, {498, 512, 525}, {498, 512, 525}, 
                    {498, 512, 525}, {498, 512, 525}, {498, 512, 525}, {498, 512, 525}, {498, 512, 525}, {498, 512, 525}, {498, 512, 525}, {498, 512, 525}, {498, 512, 525}
                }, // F
                {
                    {500, 510, 545}, {500, 510, 545}, {500, 510, 545}, {500, 510, 545}, {500, 510, 545}, {500, 510, 545}, {500, 510, 545}, {500, 510, 545}, {500, 510, 545}, {500, 510, 545}, 
                    {500, 510, 545}, {500, 510, 545}, {500, 510, 545}, {500, 510, 545}, {500, 510, 545}, {500, 510, 545}, {500, 510, 545}, {500, 510, 545}, {500, 510, 545}
                }, // CWF
                {
                    {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, 
                    {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, {505, 512, 515}, {505, 512, 515}
                }, // DAYLIGHT
                {
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, 
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                }, // SHADE
                {
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, 
                    {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}, {512, 512, 512}
                } // DAYLIGHT F
            },
                // Parent block weight parameter
                {
                    1,      // bEnable
                    6           // i4ScalingFactor: [6] 1~12, [7] 1~6, [8] 1~3, [9] 1~2, [>=10]: 1
                },
                // AWB LV threshold for predictor
                {
                    115,    // i4InitLVThr_L
                    155,    // i4InitLVThr_H
                    100      // i4EnqueueLVThr
                },
                // AWB number threshold for temporal predictor
                {
                    65,     // i4Neutral_ParentBlk_Thr
                    //LV0  1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18
                    { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  50,  25,   2,   2,   2,   2,   2,   2,   2}  // (%) i4CWFDF_LUTThr
                },
                // AWB light neutral noise reduction for outdoor
                {
                    //LV0  1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18
                    // Non neutral
                    {   3,   3,   3,   3,   3,   3,   3,   3,   3,   3,   5,  10,  10,  10,  10,  10,  10,  10,  10},  // (%)
                    // Flurescent
                    {   0,   0,   0,   0,   0,   3,   5,   5,   5,   5,   5,  10,  10,  10,  10,  10,  10,  10,  10},  // (%)
                    // CWF
                    {   0,   0,   0,   0,   0,   3,   5,   5,   5,   5,   5,  10,  10,  10,  10,  10,  10,  10,  10},  // (%)
                    // Daylight
                    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   2,   2,   2,   2,   2,   2,   2,   2},  // (%)
                    // DF
                    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   5,  10,  10,  10,  10,  10,  10,  10,  10},  // (%)
                },
                // AWB feature detection
                {
                // Sunset Prop
                    {
                        1,          // i4Enable
                        120,        // i4LVThr_L
                        130,        // i4LVThr_H
                        10,         // i4SunsetCountThr
                        0,          // i4SunsetCountRatio_L
                        171         // i4SunsetCountRatio_H
                    },
                    // Shade F Detection
                    {
                        1,          // i4Enable
                        50,        // i4LVThr_L
                        70,        // i4LVThr_H
                        128         // i4DaylightProb
                    },
                    // Shade CWF Detection
                    {
                        1,          // i4Enable
                        50,        // i4LVThr_L
                        70,        // i4LVThr_H
                        192         // i4DaylightProb
                    },
                    // Low CCT
                    {
                        0,          // i4Enable
                        256        // i4SpeedRatio
                    }
                },
                // AWB non-neutral probability for spatial and temporal weighting look-up table (Max: 100; Min: 0)
                {
                    //LV0   1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16   17   18
                    {   0,  33,  66, 100, 100, 100, 100, 100, 100, 100, 100,  70,  30,  20,  10,   0,   0,   0,   0}
                },
                // AWB daylight locus probability look-up table (Max: 100; Min: 0)
                {   //LV0    1     2     3      4     5     6     7     8      9      10     11    12   13     14    15   16    17    18
                    { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  50,  25,   0,   0,   0,   0}, // Strobe
                    { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  75,  50,  25,   0,   0,   0}, // Tungsten
                    { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  75,  50,  25,  25,  25,   0,   0,   0}, // Warm fluorescent
                    { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  95,  75,  50,  25,  25,  25,   0,   0,   0}, // Fluorescent
                    {  90,  90,  90,  90,  90,  90,  90,  90,  90,  90,  80,  55,  30,  30,  30,  30,   0,   0,   0}, // CWF
                    { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  75,  50,  40,  30,  20}, // Daylight
                    { 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,  75,  50,  25,   0,   0,   0,   0}, // Shade
                    {  90,  90,  90,  90,  90,  90,  90,  90,  90,  90,  80,  55,  30,  30,  30,  30,   0,   0,   0} // Daylight fluorescent
                }
            }
        },
        {
          {
            // AWB calibration data
            {
                // rUnitGain (unit gain: 1.0 = 512)
                {
                    0,    // i4R
                    0,    // i4G
                    0    // i4B
                },
                // rGoldenGain (golden sample gain: 1.0 = 512)
                {
                    0,    // i4R
                    0,    // i4G
                    0    // i4B
                },
                // rTuningUnitGain (Tuning sample unit gain: 1.0 = 512)
                {
                    0,    // i4R
                    0,    // i4G
                    0    // i4B
                },
                // rD65Gain (D65 WB gain: 1.0 = 512)
                {
                    0,    // D65Gain_R
                    0,    // D65Gain_G
                    0    // D65Gain_B
                }
            },
            // Original XY coordinate of AWB light source
            {
                // Strobe
                {
                    0,    // i4X
                    0    // i4Y
                },
                // Horizon
                {
                    0,    // OriX_Hor
                    0    // OriY_Hor
                },
                // A
                {
                    0,    // OriX_A
                    0    // OriY_A
                },
                // TL84
                {
                    0,    // OriX_TL84
                    0    // OriY_TL84
                },
                // CWF
                {
                    0,    // OriX_CWF
                    0    // OriY_CWF
                },
                // DNP
                {
                    0,    // OriX_DNP
                    0    // OriY_DNP
                },
                // D65
                {
                    0,    // OriX_D65
                    0    // OriY_D65
                },
                // DF
                {
                    0,    // OriX_DF
                    0    // OriY_DF
                }
            },
            // Rotated XY coordinate of AWB light source
            {
                // Strobe
                {
                    0,    // i4X
                    0    // i4Y
                },
                // Horizon
                {
                    0,    // RotX_Hor
                    0    // RotY_Hor
                },
                // A
                {
                    0,    // RotX_A
                    0    // RotY_A
                },
                // TL84
                {
                    0,    // RotX_TL84
                    0    // RotY_TL84
                },
                // CWF
                {
                    0,    // RotX_CWF
                    0    // RotY_CWF
                },
                // DNP
                {
                    0,    // RotX_DNP
                    0    // RotY_DNP
                },
                // D65
                {
                    0,    // RotX_D65
                    0    // RotY_D65
                },
                // DF
                {
                    0,    // RotX_DF
                    0    // RotY_DF
                }
            },
            // AWB gain of AWB light source
            {
                // Strobe 
                {
                    0,    // i4R
                    0,    // i4G
                    0    // i4B
                },
                // Horizon 
                {
                    0,    // AWBGAIN_HOR_R
                    0,    // AWBGAIN_HOR_G
                    0    // AWBGAIN_HOR_B
                },
                // A 
                {
                    0,    // AWBGAIN_A_R
                    0,    // AWBGAIN_A_G
                    0    // AWBGAIN_A_B
                },
                // TL84 
                {
                    0,    // AWBGAIN_TL84_R
                    0,    // AWBGAIN_TL84_G
                    0    // AWBGAIN_TL84_B
                },
                // CWF 
                {
                    0,    // AWBGAIN_CWF_R
                    0,    // AWBGAIN_CWF_G
                    0    // AWBGAIN_CWF_B
                },
                // DNP 
                {
                    0,    // AWBGAIN_DNP_R
                    0,    // AWBGAIN_DNP_G
                    0    // AWBGAIN_DNP_B
                },
                // D65 
                {
                    0,    // AWBGAIN_D65_R
                    0,    // AWBGAIN_D65_G
                    0    // AWBGAIN_D65_B
                },
                // DF 
                {
                    0,    // AWBGAIN_DF_R
                    0,    // AWBGAIN_DF_G
                    0    // AWBGAIN_DF_B
                }
            },
            // Rotation matrix parameter
            {
                0,    // RotationAngle
                0,    // Cos
                0    // Sin
            },
            // Daylight locus parameter
            {
                0,    // i4SlopeNumerator
                0    // i4SlopeDenominator
            },
            // Predictor gain
            {
                0, // i4PrefRatio100
                // DaylightLocus_L
                {
                    0,    // i4R
                    0,    // i4G
                    0    // i4B
                },
                // DaylightLocus_H
                {
                    0,    // i4R
                    0,    // i4G
                    0    // i4B
                },
                // Temporal General
                {
                    0,    // i4R
                    0,    // i4G
                    0    // i4B
                },
                // AWB LSC Gain
                {
                    0,        // i4R
                    0,        // i4G
                    0        // i4B
                }
            },
            // AWB light area
            {
                // Strobe:FIXME
                {
                    0,    // i4RightBound
                    0,    // i4LeftBound
                    0,    // i4UpperBound
                    0    // i4LowerBound
                },
                // Tungsten
                {
                    0,    // TungRightBound
                    0,    // TungLeftBound
                    0,    // TungUpperBound
                    0    // TungLowerBound
                },
                // Warm fluorescent
                {
                    0,    // WFluoRightBound
                    0,    // WFluoLeftBound
                    0,    // WFluoUpperBound
                    0    // WFluoLowerBound
                },
                // Fluorescent
                {
                    0,    // FluoRightBound
                    0,    // FluoLeftBound
                    0,    // FluoUpperBound
                    0    // FluoLowerBound
                },
                // CWF
                {
                0,    // CWFRightBound
                0,    // CWFLeftBound
                0,    // CWFUpperBound
                0    // CWFLowerBound
                },
                // Daylight
                {
                    0,    // DayRightBound
                    0,    // DayLeftBound
                    0,    // DayUpperBound
                    0    // DayLowerBound
                },
                // Shade
                {
                    0,    // ShadeRightBound
                    0,    // ShadeLeftBound
                    0,    // ShadeUpperBound
                    0    // ShadeLowerBound
                },
                // Daylight Fluorescent
                {
                    0,    // DFRightBound
                    0,    // DFLeftBound
                    0,    // DFUpperBound
                    0    // DFLowerBound
                }
            },
            // PWB light area
            {
                // Reference area
                {
                    0,    // PRefRightBound
                    0,    // PRefLeftBound
                    0,    // PRefUpperBound
                    0    // PRefLowerBound
                },
                // Daylight
                {
                    0,    // PDayRightBound
                    0,    // PDayLeftBound
                    0,    // PDayUpperBound
                    0    // PDayLowerBound
                },
                // Cloudy daylight
                {
                    0,    // PCloudyRightBound
                    0,    // PCloudyLeftBound
                    0,    // PCloudyUpperBound
                    0    // PCloudyLowerBound
                },
                // Shade
                {
                    0,    // PShadeRightBound
                    0,    // PShadeLeftBound
                    0,    // PShadeUpperBound
                    0    // PShadeLowerBound
                },
                // Twilight
                {
                    0,    // PTwiRightBound
                    0,    // PTwiLeftBound
                    0,    // PTwiUpperBound
                    0    // PTwiLowerBound
                },
                // Fluorescent
                {
                    0,    // PFluoRightBound
                    0,    // PFluoLeftBound
                    0,    // PFluoUpperBound
                    0    // PFluoLowerBound
                },
                // Warm fluorescent
                {
                    0,    // PWFluoRightBound
                    0,    // PWFluoLeftBound
                    0,    // PWFluoUpperBound
                    0    // PWFluoLowerBound
                },
                // Incandescent
                {
                    0,    // PIncaRightBound
                    0,    // PIncaLeftBound
                    0,    // PIncaUpperBound
                    0    // PIncaLowerBound
                },
                // Gray World
                {
                    0,    // PGWRightBound
                    0,    // PGWLeftBound
                    0,    // PGWUpperBound
                    0    // PGWLowerBound
                }
            },
            // PWB default gain	
            {
                // Daylight
                {
                    0,    // PWB_Day_R
                    0,    // PWB_Day_G
                    0    // PWB_Day_B
                },
                // Cloudy daylight
                {
                    0,    // PWB_Cloudy_R
                    0,    // PWB_Cloudy_G
                    0    // PWB_Cloudy_B
                },
                // Shade
                {
                    0,    // PWB_Shade_R
                    0,    // PWB_Shade_G
                    0    // PWB_Shade_B
                },
                // Twilight
                {
                    0,    // PWB_Twi_R
                    0,    // PWB_Twi_G
                    0    // PWB_Twi_B
                },
                // Fluorescent
                {
                    0,    // PWB_Fluo_R
                    0,    // PWB_Fluo_G
                    0    // PWB_Fluo_B
                },
                // Warm fluorescent
                {
                    0,    // PWB_WFluo_R
                    0,    // PWB_WFluo_G
                    0    // PWB_WFluo_B
                },
                // Incandescent
                {
                    0,    // PWB_Inca_R
                    0,    // PWB_Inca_G
                    0    // PWB_Inca_B
                },
                // Gray World
                {
                    0,    // PWB_GW_R
                    0,    // PWB_GW_G
                    0    // PWB_GW_B
                }
            },
            // AWB preference color	
            {
                // Tungsten
                {
                    0,    // TUNG_SLIDER
                    0    // TUNG_OFFS
                },
                // Warm fluorescent	
                {
                    0,    // WFluo_SLIDER
                    0    // WFluo_OFFS
                },
                // Shade
                {
                    0,    // Shade_SLIDER
                    0    // Shade_OFFS
                },
                // Sunset Area
                {
                    0,   // i4Sunset_BoundXr_Thr
                    0    // i4Sunset_BoundYr_Thr
                },
                // Sunset Vertex
                {
                    0,   // i4Sunset_BoundXr_Thr
                    0    // i4Sunset_BoundYr_Thr
                },
                // Shade F Area
                {
                    0,   // i4BoundXrThr
                    0    // i4BoundYrThr
                },
                // Shade F Vertex
                {
                    0,   // i4BoundXrThr
                    0    // i4BoundYrThr
                },
                // Shade CWF Area
                {
                    0,   // i4BoundXrThr
                    0    // i4BoundYrThr
                },
                // Shade CWF Vertex
                {
                    0,   // i4BoundXrThr
                    0    // i4BoundYrThr
                },
                // Low CCT Area
                {
                    0,   // i4BoundXrThr
                    0    // i4BoundYrThr
                },
                // Low CCT Vertex
                {
                    0,   // i4BoundXrThr
                    0    // i4BoundYrThr
                }
            },
            // CCT estimation
            {
                // CCT
                {
                    0,    // i4CCT[0]
                    0,    // i4CCT[1]
                    0,    // i4CCT[2]
                    0,    // i4CCT[3]
                    0     // i4CCT[4]
                },
                // Rotated X coordinate
                {
                0,    // i4RotatedXCoordinate[0]
                0,    // i4RotatedXCoordinate[1]
                0,    // i4RotatedXCoordinate[2]
                0,    // i4RotatedXCoordinate[3]
                0    // i4RotatedXCoordinate[4]
                }
                }
            },
            // Algorithm Tuning Paramter
            {
                // AWB Backup Enable
                0,

            // Daylight locus offset LUTs for tungsten
            {
                0, // i4Size: LUT dimension
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // i4LUTIn
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} // i4LUTOut
            },
            // Daylight locus offset LUTs for WF
            {
                0, // i4Size: LUT dimension
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // i4LUTIn
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} // i4LUTOut
            },
            // Daylight locus offset LUTs for Shade
            {
                0, // i4Size: LUT dimension
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // i4LUTIn
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0} // i4LUTOut
            },
            // Preference gain for each light source
            {
                {
                    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 
                    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}
                }, // STROBE
                {
                    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 
                    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}
                }, // TUNGSTEN
                {
                    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 
                    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}
                }, // WARM F
                {
                    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 
                    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}
                }, // F
                {
                    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 
                    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}
                }, // CWF
                {
                    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 
                    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}
                }, // DAYLIGHT
                {
                    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 
                    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}
                }, // SHADE
                {
                    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, 
                    {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}
                } // DAYLIGHT F
            },
                // Parent block weight parameter
                {
                    0,      // bEnable
                    0           // i4ScalingFactor: [6] 1~12, [7] 1~6, [8] 1~3, [9] 1~2, [>=10]: 1
                },
                // AWB LV threshold for predictor
                {
                    0,    // i4InitLVThr_L
                    0,    // i4InitLVThr_H
                    0      // i4EnqueueLVThr
                },
                // AWB number threshold for temporal predictor
                {
                    0,     // i4Neutral_ParentBlk_Thr
                    //LV0  1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18
                    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}  // (%) i4CWFDF_LUTThr
                },
                // AWB light neutral noise reduction for outdoor
                {
                    //LV0  1    2    3    4    5    6    7    8    9    10   11   12   13   14   15   16   17   18
                    // Non neutral
                    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},  // (%)
                    // Flurescent
                    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},  // (%)
                    // CWF
                    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},  // (%)
                    // Daylight
                    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},  // (%)
                    // DF
                    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0},  // (%)
                },
                // AWB feature detection
                {
                // Sunset Prop
                    {
                        0,          // i4Enable
                        0,        // i4LVThr_L
                        0,        // i4LVThr_H
                        0,         // i4SunsetCountThr
                        0,          // i4SunsetCountRatio_L
                        0         // i4SunsetCountRatio_H
                    },
                    // Shade F Detection
                    {
                        0,          // i4Enable
                        0,        // i4LVThr_L
                        0,        // i4LVThr_H
                        0         // i4DaylightProb
                    },
                    // Shade CWF Detection
                    {
                        0,          // i4Enable
                        0,        // i4LVThr_L
                        0,        // i4LVThr_H
                        0         // i4DaylightProb
                    },
                    // Low CCT
                    {
                        0,          // i4Enable
                        0        // i4SpeedRatio
                    }
                },
                // AWB non-neutral probability for spatial and temporal weighting look-up table (Max: 100; Min: 0)
                {
                    //LV0   1    2    3    4    5    6    7    8    9   10   11   12   13   14   15   16   17   18
                    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}
                },
                // AWB daylight locus probability look-up table (Max: 100; Min: 0)
                {   //LV0    1     2     3      4     5     6     7     8      9      10     11    12   13     14    15   16    17    18
                    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}, // Strobe
                    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}, // Tungsten
                    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}, // Warm fluorescent
                    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}, // Fluorescent
                    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}, // CWF
                    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}, // Daylight
                    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0}, // Shade
                    {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0} // Daylight fluorescent
                }
            }
        }
    },
    // Flash AWB NVRAM
    {
#include INCLUDE_FILENAME_FLASH_AWB_PARA
    },
    {0}
};

#include INCLUDE_FILENAME_ISP_LSC_PARAM
//};  //  namespace

const CAMERA_TSF_TBL_STRUCT CAMERA_TSF_DEFAULT_VALUE =
{
    {
        1, // isTsfEn
        2, // tsfCtIdx
        {20, 2000, -110, -110, 512, 512, 512, 0}    // rAWBInput[8]
    },
#include INCLUDE_FILENAME_TSF_PARA
#include INCLUDE_FILENAME_TSF_DATA
};


typedef NSFeature::RAWSensorInfo<SENSOR_ID> SensorInfoSingleton_T;


namespace NSFeature {
template <>
UINT32
SensorInfoSingleton_T::
impGetDefaultData(CAMERA_DATA_TYPE_ENUM const CameraDataType, VOID*const pDataBuf, UINT32 const size) const
{
    UINT32 dataSize[CAMERA_DATA_TYPE_NUM] = {sizeof(NVRAM_CAMERA_ISP_PARAM_STRUCT),
                                             sizeof(NVRAM_CAMERA_3A_STRUCT),
                                             sizeof(NVRAM_CAMERA_SHADING_STRUCT),
                                             sizeof(NVRAM_LENS_PARA_STRUCT),
                                             sizeof(AE_PLINETABLE_T),
                                             0,
                                             sizeof(CAMERA_TSF_TBL_STRUCT)};
    if (CameraDataType > CAMERA_DATA_TSF_TABLE || NULL == pDataBuf || (size < dataSize[CameraDataType]))
    {
        return 1;
    }

    switch(CameraDataType)
    {
        case CAMERA_NVRAM_DATA_ISP:
            memcpy(pDataBuf,&CAMERA_ISP_DEFAULT_VALUE,sizeof(NVRAM_CAMERA_ISP_PARAM_STRUCT));
            break;
        case CAMERA_NVRAM_DATA_3A:
            memcpy(pDataBuf,&CAMERA_3A_NVRAM_DEFAULT_VALUE,sizeof(NVRAM_CAMERA_3A_STRUCT));
            break;
        case CAMERA_NVRAM_DATA_SHADING:
            memcpy(pDataBuf,&CAMERA_SHADING_DEFAULT_VALUE,sizeof(NVRAM_CAMERA_SHADING_STRUCT));
            break;
        case CAMERA_DATA_AE_PLINETABLE:
            memcpy(pDataBuf,&g_PlineTableMapping,sizeof(AE_PLINETABLE_T));
            break;
        case CAMERA_DATA_TSF_TABLE:
            memcpy(pDataBuf,&CAMERA_TSF_DEFAULT_VALUE,sizeof(CAMERA_TSF_TBL_STRUCT));
            break;
        default:
            return 1;
    }
    return 0;
}}; // NSFeature


