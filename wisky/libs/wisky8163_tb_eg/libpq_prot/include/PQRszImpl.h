#ifndef __PQRSZIMPL_H__
#define __PQRSZIMPL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef RSZ_VERSION
    #define RSZ_VERSION RSZ_VER_EVEREST
    #define RSZ_VER_EVEREST 0
    #define RSZ_VER_ROCKY   1
    #define RSZ_VER_WHITNEY 2
    #define RSZ_VER_ELBRUS  3
#endif

#ifndef RESZIER_6TAP_STEPCOUNTER_BIT
    #define RESZIER_6TAP_STEPCOUNTER_BIT 15
#endif

#define RSZ_ANDROID_PLATFORM

typedef unsigned int    uint32_t;
typedef unsigned short  uint16_t;
typedef unsigned char   uint8_t;
typedef signed int      int32_t;

// Initial register values to Resizer HW
struct RszInitReg {
    uint32_t reserved;  // To pass compilation
};

// Initial parameter to Resizer HW
struct RszInitParam {
    uint32_t reserved;  // To pass compilation
};

// Fields collected from Resizer HW
struct RszInput {
    uint16_t srcWidth;
    uint16_t srcHeight;
    uint16_t dstWidth;
    uint16_t dstHeight;
    uint16_t cropOffsetX;
    uint32_t cropSubpixX;
    uint16_t cropOffsetY;
    uint32_t cropSubpixY;
    uint16_t cropWidth;
    uint16_t cropHeight;

    uint8_t enable;         // = 1;
    uint8_t yuv422Tyuv444;  // = 0;
    uint8_t demoEnable;     // = 0;

    // Demo Window
    uint16_t demoOutHStart;
    uint16_t demoOutHEnd;
    uint16_t demoOutVStart;
    uint16_t demoOutVEnd;

#if (RSZ_VERSION == RSZ_VER_WHITNEY)
    // 8b or 10b mode
    uint8_t powerSavingMode;
#endif
};

// Fields which will be set to HW registers
struct RszOutput {
    uint8_t  rszEnable;
    uint8_t  horEnable;
    uint8_t  verEnable;
    uint8_t  verticalFirst;
    uint8_t  horAlgo;
    uint8_t  verAlgo;
    uint8_t  horTable;
    uint8_t  verTable;
    uint32_t precX;
    uint32_t precY;
    uint32_t coeffStepX;
    uint32_t coeffStepY;
    uint8_t  IBSEEnable;//should not be used
    uint8_t  tapAdaptEnable;

    // demo window
    uint16_t demoInHStart;
    uint16_t demoInHEnd;
    uint16_t demoInVStart;
    uint16_t demoInVEnd;

    // offset
    int32_t horLumaIntOffset;
    int32_t horLumaSubOffset;
    int32_t horChromaIntOffset;
    int32_t horChromaSubOffset;
    int32_t verLumaIntOffset;
    int32_t verLumaSubOffset;
    int32_t verChromaIntOffset;
    int32_t verChromaSubOffset;

    // truncation of coefficients
    uint8_t horTruncBit;
    uint8_t verTruncBit;

#if ((RSZ_VERSION==RSZ_VER_ELBRUS) | (RSZ_VERSION==RSZ_VER_WHITNEY))
    // truncation of coefficients
    uint8_t verCubicTruncEn;
    uint8_t verLumaCubicTruncBit;
    uint8_t verChromaCubicTruncBit;
    uint8_t horCubicTruncEn;
    uint8_t horLumaCubicTruncBit;
    uint8_t horChromaCubicTruncBit;
#endif

    //Ultra Resolution
    uint8_t IBSEGainMid;
    uint8_t IBSE_clip_thr;
    uint8_t IBSE_clip_ratio;
    uint8_t TapAdaptSlope;

#if ((RSZ_VERSION==RSZ_VER_ELBRUS) | (RSZ_VERSION==RSZ_VER_WHITNEY))
    //C42, C24 control
    uint8_t c24CropEn;
    uint8_t c42odd2evenMode;
    uint8_t c42LCropEn;
    uint8_t c42InterpEn;
#endif
};

// Resizer FW registers
struct RszReg {
    uint8_t  tableMode;  // rollback mode or not (Rome or Everest)
    uint8_t  defaultUpTable; // defaultTable for up scaling
    uint8_t  defaultDownTable; // defaultTable for down scaling
    uint8_t  autoTableSelection; // enable auto table selection
    uint8_t  autoAlignment; // enable auto alignment
    uint8_t  autoAlgSelection; // enable auto algorithm selelction
    uint8_t  IBSEEnable; // enable IBSE
    uint8_t  ultraResEnable; // enable Ultra Resolution
    uint8_t  autoCoefTrunc; // bit truncation for (1/2, 1]x scaling ratio

    // algorithm-switch ratios for Rome mode
    uint8_t  switchRatio6Tap6nTap;
    uint8_t  switchRatio6nTapAcc;

    // Ultra Resolution
    int32_t dynIBSE_gain_ratio_thr0;
    int32_t dynIBSE_gain_ratio_thr1;
    int32_t dynIBSE_gain_ratio_thr2;
    int32_t dynIBSE_gain_clip1;
    int32_t dynIBSE_gain_clip2;
    int32_t dynIBSE_gain_min;
    int32_t dynIBSE_gain_max;

    int32_t tapAdaptSlope;
    int32_t tapAdapt_slope_ratio_thr0;
    int32_t tapAdapt_slope_ratio_thr1;
    int32_t tapAdapt_slope_ratio_thr2;
    int32_t tapAdapt_slope_clip1;
    int32_t tapAdapt_slope_clip2;
    int32_t tapAdapt_slope_min;
    int32_t tapAdapt_slope_max;

    int32_t IBSE_clip_thr;
    int32_t IBSE_clip_thr_ratio_thr0;
    int32_t IBSE_clip_thr_ratio_thr1;
    int32_t IBSE_clip_thr_ratio_thr2;
    int32_t IBSE_clip_thr_clip1;
    int32_t IBSE_clip_thr_clip2;
    int32_t IBSE_clip_thr_min;
    int32_t IBSE_clip_thr_max;

    int32_t IBSE_clip_ratio;
    int32_t IBSE_clip_ratio_ratio_thr0;
    int32_t IBSE_clip_ratio_ratio_thr1;
    int32_t IBSE_clip_ratio_ratio_thr2;
    int32_t IBSE_clip_ratio_clip1;
    int32_t IBSE_clip_ratio_clip2;
    int32_t IBSE_clip_ratio_min;
    int32_t IBSE_clip_ratio_max;
};


class CPQRszFW
{
public:

#ifdef RSZ_ANDROID_PLATFORM
    void onCalculate(const RszInput *input, RszOutput *output);
    void onInitPlatform(const RszInitParam &initParam, RszInitReg *initReg);
#else
    void vDrvRszProc_int(const RszInput *input, RszOutput *output);
#endif

    void initialize(void);

    CPQRszFW()
    {
        memset(this, 0, sizeof(*this));
        #ifdef RSZ_ANDROID_PLATFORM
        initialize();
        #endif
    };

    ~CPQRszFW()
    {
    };

    RszReg m_rszReg;

private:
    /****** Resizer processes, variables ******/
    void rsz_alg_det(const RszInput *reg_in, RszOutput *reg_out);
    void rsz_config(const RszInput *reg_in, RszOutput *reg_out, int Alg, int isHor);
    int rsz_tbl_select(int Alg, int stepSize);
    void rsz_config_demoWindow(const RszInput *input, RszOutput *output);
    void rsz_config_ctrlRegs(const RszInput *reg_in, RszOutput *reg_out);
    void rsz_auto_align(const RszInput *reg_in, RszOutput *reg_out, int Alg, int isHor);
    void rsz_ultraRes(const RszInput *reg_in, RszOutput *reg_out);
    int32_t UltraResGetRegWOInit(int32_t in_ratio, int32_t reg_ratio_thr0, int32_t reg_ratio_thr1, int32_t reg_ratio_thr2, int32_t reg_clip1, int32_t reg_clip2, int32_t reg_min, int32_t reg_max);
    int32_t UltraResGetRegWInit(int32_t in_value, int32_t in_ratio, int32_t reg_ratio_thr0, int32_t reg_ratio_thr1, int32_t reg_ratio_thr2, int32_t reg_clip1, int32_t reg_clip2, int32_t reg_min, int32_t reg_max);
    int32_t AlphaBlending(int32_t data1, int32_t data2, int32_t alpha, int32_t bits);
    void rsz_auto_coefTrunc(const RszInput *reg_in, RszOutput *reg_out);
    void rsz_offset_check(const RszInput *reg_in, RszOutput *reg_out);
    //void rsz_coef_trunc_6nTap(void);

#if 0
    RszInput oldInput;
    RszOutput oldOutput;
#endif
};


#endif  // __PQRSZIMPL_H__
