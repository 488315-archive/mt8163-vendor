#ifndef __PQSERVICECOMMON_H__
#define __PQSERVICECOMMON_H__

typedef enum {
    TDSHP_GAIN_MID = 0,
    TDSHP_GAIN_HIGH,
    TDSHP_COR_GAIN,
    TDSHP_COR_THR,
    TDSHP_COR_ZERO,
    TDSHP_GAIN,
    TDSHP_COR_VALUE,
#ifdef TDSHP_2_0
    TDSHP_HIGH_COR_ZERO,
    TDSHP_HIGH_COR_THR,
    TDSHP_HIGH_COR_GAIN,
    TDSHP_HIGH_COR_VALUE,
    TDSHP_MID_COR_ZERO,
    TDSHP_MID_COR_THR,
    TDSHP_MID_COR_GAIN,
    TDSHP_MID_COR_VALUE,
    EDF_EDGE_GAIN,
    EDF_FLAT_TH,
    EDF_DETAIL_RISE_TH,
    TDSHP_CLIP_RATIO,
    TDSHP_CLIP_THR,
#endif
    TDSHP_REG_INDEX_MAX
} MDP_TDSHP_REG_EX_index_t;

typedef struct {

    int param[TDSHP_REG_INDEX_MAX];

} MDP_TDSHP_REG_EX;

typedef enum {
    PQDS_DS_en = 0,
    PQDS_iUpSlope,
    PQDS_iUpThreshold,
    PQDS_iDownSlope,
    PQDS_iDownThreshold,
    PQDS_iISO_en,
    PQDS_iISO_thr1,
    PQDS_iISO_thr0,
    PQDS_iISO_thr3,
    PQDS_iISO_thr2,
    PQDS_iISO_IIR_alpha,
    PQDS_iCorZero_clip2,
    PQDS_iCorZero_clip1,
    PQDS_iCorZero_clip0,
    PQDS_iCorThr_clip2,
    PQDS_iCorThr_clip1,
    PQDS_iCorThr_clip0,
    PQDS_iCorGain_clip2,
    PQDS_iCorGain_clip1,
    PQDS_iCorGain_clip0,
    PQDS_iGain_clip2,
    PQDS_iGain_clip1,
    PQDS_iGain_clip0,
#ifdef TDSHP_2_0
    //6797 new reg
    PQDS_iHighCorZero_clip2,
    PQDS_iHighCorZero_clip1,
    PQDS_iHighCorZero_clip0,
    PQDS_iHighCorThr_clip2,
    PQDS_iHighCorThr_clip1,
    PQDS_iHighCorThr_clip0,
    PQDS_iHighCorGain_clip2,
    PQDS_iHighCorGain_clip1,
    PQDS_iHighCorGain_clip0,

    PQDS_iMidCorZero_clip2,
    PQDS_iMidCorZero_clip1,
    PQDS_iMidCorZero_clip0,
    PQDS_iMidCorThr_clip2,
    PQDS_iMidCorThr_clip1,
    PQDS_iMidCorThr_clip0,
    PQDS_iMidCorGain_clip2,
    PQDS_iMidCorGain_clip1,
    PQDS_iMidCorGain_clip0,

    PQDS_i_edf_flat_th_clip2,
    PQDS_i_edf_flat_th_clip1,
    PQDS_i_edf_flat_th_clip0,
    PQDS_i_edf_detail_rise_th_clip2,
    PQDS_i_edf_detail_rise_th_clip1,
    PQDS_i_edf_detail_rise_th_clip0,

    //UltraResolution registers
    PQDS_iUltraRes_en,
    PQDS_iUltraRes_ratio_thr2,
    PQDS_iUltraRes_ratio_thr1,
    PQDS_iUltraRes_ratio_thr0,
    PQDS_iUltraRes_gain_mid_clip1,
    PQDS_iUltraRes_gain_mid_clip0,
    PQDS_iUltraRes_gain_high_clip1,
    PQDS_iUltraRes_gain_high_clip0,
    PQDS_iUltraRes_edf_edge_gain_clip1,
    PQDS_iUltraRes_edf_edge_gain_clip0,
    PQDS_iUltraRes_clip_thr_clip1,
    PQDS_iUltraRes_clip_thr_clip0,
    PQDS_iUltraRes_clip_ratio_clip1,
    PQDS_iUltraRes_clip_ratio_clip0,
#endif
    PQDS_INDEX_MAX
} PQ_DS_EX_index_t;


typedef struct {

    int param[PQDS_INDEX_MAX];

} DISP_PQ_DS_PARAM_EX;

typedef enum {
    RSZ_tableMode = 0,
    RSZ_defaultUpTable,
    RSZ_defaultDownTable,
    RSZ_autoTableSelection,
    RSZ_autoAlignment,
    RSZ_autoAlgSelection,
    RSZ_IBSEEnable,
    RSZ_ultraResEnable,
    RSZ_autoCoefTrunc,

    /* algorithm-switch ratios for Rome mode */
    RSZ_switchRatio6Tap6nTap,
    RSZ_switchRatio6nTapAcc,

    /* Ultra Resolution */
    RSZ_dynIBSE_gain_ratio_thr0,
    RSZ_dynIBSE_gain_ratio_thr1,
    RSZ_dynIBSE_gain_ratio_thr2,
    RSZ_dynIBSE_gain_clip1,
    RSZ_dynIBSE_gain_clip2,
    RSZ_dynIBSE_gain_min,
    RSZ_dynIBSE_gain_max,

    RSZ_tapAdaptSlope,
    RSZ_tapAdapt_slope_ratio_thr0,
    RSZ_tapAdapt_slope_ratio_thr1,
    RSZ_tapAdapt_slope_ratio_thr2,
    RSZ_tapAdapt_slope_clip1,
    RSZ_tapAdapt_slope_clip2,
    RSZ_tapAdapt_slope_min,
    RSZ_tapAdapt_slope_max,

    RSZ_IBSE_clip_thr,
    RSZ_IBSE_clip_thr_ratio_thr0,
    RSZ_IBSE_clip_thr_ratio_thr1,
    RSZ_IBSE_clip_thr_ratio_thr2,
    RSZ_IBSE_clip_thr_clip1,
    RSZ_IBSE_clip_thr_clip2,
    RSZ_IBSE_clip_thr_min,
    RSZ_IBSE_clip_thr_max,

    RSZ_IBSE_clip_ratio,
    RSZ_IBSE_clip_ratio_ratio_thr0,
    RSZ_IBSE_clip_ratio_ratio_thr1,
    RSZ_IBSE_clip_ratio_ratio_thr2,
    RSZ_IBSE_clip_ratio_clip1,
    RSZ_IBSE_clip_ratio_clip2,
    RSZ_IBSE_clip_ratio_min,
    RSZ_IBSE_clip_ratio_max,

    RSZ_INDEX_MAX
} PQ_RSZ_index_t;


typedef struct {

    int param[RSZ_INDEX_MAX];

} PQ_RSZ_PARAM;

#define DISP_COLOR_SWREG_START              (0xFFFF0000)
#define DISP_COLOR_SWREG_COLOR_BASE         (DISP_COLOR_SWREG_START)  /* 0xFFFF0000 */
#define DISP_COLOR_SWREG_TDSHP_BASE         (DISP_COLOR_SWREG_COLOR_BASE + 0x1000)  /* 0xFFFF1000 */
#define DISP_COLOR_SWREG_TDSHP_REG_BASE     (DISP_COLOR_SWREG_COLOR_BASE + 0x1001)  /* 0xFFFF1001 */
#define DISP_COLOR_SWREG_PQDC_BASE          (DISP_COLOR_SWREG_TDSHP_BASE + 0x1000)  /* 0xFFFF2000 */
#define DISP_COLOR_SWREG_PQDS_BASE          (DISP_COLOR_SWREG_PQDC_BASE + 0x1000) /* 0xFFFF3000 */
#define DISP_COLOR_SWREG_MDP_COLOR_BASE     (DISP_COLOR_SWREG_PQDS_BASE + 0x1000) /* 0xFFFF4000 */
#define DISP_COLOR_SWREG_END                (DISP_COLOR_SWREG_MDP_COLOR_BASE + 0x1000)  /* 0xFFFF5000 */

#define SWREG_TDSHP_TUNING_MODE             (DISP_COLOR_SWREG_TDSHP_BASE + 0x0000)
#define SWREG_TDSHP_REG_GAIN_MID            (DISP_COLOR_SWREG_TDSHP_REG_BASE + TDSHP_GAIN_MID)
#define SWREG_TDSHP_REG_GAIN_HIGH           (DISP_COLOR_SWREG_TDSHP_REG_BASE + TDSHP_GAIN_HIGH)
#define SWREG_TDSHP_REG_COR_GAIN            (DISP_COLOR_SWREG_TDSHP_REG_BASE + TDSHP_COR_GAIN)
#define SWREG_TDSHP_REG_COR_THR             (DISP_COLOR_SWREG_TDSHP_REG_BASE + TDSHP_COR_THR)
#define SWREG_TDSHP_REG_COR_ZERO            (DISP_COLOR_SWREG_TDSHP_REG_BASE + TDSHP_COR_ZERO)
#define SWREG_TDSHP_REG_GAIN                (DISP_COLOR_SWREG_TDSHP_REG_BASE + TDSHP_GAIN)
#define SWREG_TDSHP_REG_COR_VALUE           (DISP_COLOR_SWREG_TDSHP_REG_BASE + TDSHP_COR_VALUE)
#ifdef TDSHP_2_0
#define SWREG_TDSHP_REG_HIGH_COR_ZERO       (DISP_COLOR_SWREG_TDSHP_REG_BASE + TDSHP_HIGH_COR_ZERO)
#define SWREG_TDSHP_REG_HIGH_COR_THR        (DISP_COLOR_SWREG_TDSHP_REG_BASE + TDSHP_HIGH_COR_THR)
#define SWREG_TDSHP_REG_HIGH_COR_GAIN       (DISP_COLOR_SWREG_TDSHP_REG_BASE + TDSHP_HIGH_COR_GAIN)
#define SWREG_TDSHP_REG_HIGH_COR_VALUE      (DISP_COLOR_SWREG_TDSHP_REG_BASE + TDSHP_HIGH_COR_VALUE)
#define SWREG_TDSHP_REG_MID_COR_ZERO        (DISP_COLOR_SWREG_TDSHP_REG_BASE + TDSHP_MID_COR_ZERO)
#define SWREG_TDSHP_REG_MID_COR_THR         (DISP_COLOR_SWREG_TDSHP_REG_BASE + TDSHP_MID_COR_THR)
#define SWREG_TDSHP_REG_MID_COR_GAIN        (DISP_COLOR_SWREG_TDSHP_REG_BASE + TDSHP_MID_COR_GAIN)
#define SWREG_TDSHP_REG_MID_COR_VALUE       (DISP_COLOR_SWREG_TDSHP_REG_BASE + TDSHP_MID_COR_VALUE)
#define SWREG_TDSHP_REG_EDF_EDGE_GAIN       (DISP_COLOR_SWREG_TDSHP_REG_BASE + EDF_EDGE_GAIN)
#define SWREG_TDSHP_REG_EDF_FLAT_TH         (DISP_COLOR_SWREG_TDSHP_REG_BASE + EDF_FLAT_TH)
#define SWREG_TDSHP_REG_EDF_DETAIL_RISE_TH  (DISP_COLOR_SWREG_TDSHP_REG_BASE + EDF_DETAIL_RISE_TH)
#define SWREG_TDSHP_REG_CLIP_RATIO          (DISP_COLOR_SWREG_TDSHP_REG_BASE + TDSHP_CLIP_RATIO)
#define SWREG_TDSHP_REG_CLIP_THR            (DISP_COLOR_SWREG_TDSHP_REG_BASE + TDSHP_CLIP_THR)
#endif
#define SWREG_TDSHP_REG_END                 (DISP_COLOR_SWREG_TDSHP_REG_BASE + TDSHP_REG_INDEX_MAX)


#define SWREG_PQDS_DS_EN                    (DISP_COLOR_SWREG_PQDS_BASE + PQDS_DS_en)
#define SWREG_PQDS_UP_SLOPE                 (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iUpSlope)
#define SWREG_PQDS_UP_THR                   (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iUpThreshold)
#define SWREG_PQDS_DOWN_SLOPE               (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iDownSlope)
#define SWREG_PQDS_DOWN_THR                 (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iDownThreshold)
#define SWREG_PQDS_ISO_EN                   (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iISO_en)
#define SWREG_PQDS_ISO_THR1                 (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iISO_thr1)
#define SWREG_PQDS_ISO_THR0                 (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iISO_thr0)
#define SWREG_PQDS_ISO_THR3                 (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iISO_thr3)
#define SWREG_PQDS_ISO_THR2                 (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iISO_thr2)
#define SWREG_PQDS_ISO_IIR                  (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iISO_IIR_alpha)
#define SWREG_PQDS_COR_ZERO_2               (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iCorZero_clip2)
#define SWREG_PQDS_COR_ZERO_1               (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iCorZero_clip1)
#define SWREG_PQDS_COR_ZERO_0               (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iCorZero_clip0)
#define SWREG_PQDS_COR_THR_2                (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iCorThr_clip2)
#define SWREG_PQDS_COR_THR_1                (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iCorThr_clip1)
#define SWREG_PQDS_COR_THR_0                (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iCorThr_clip0)
#define SWREG_PQDS_COR_GAIN_2               (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iCorGain_clip2)
#define SWREG_PQDS_COR_GAIN_1               (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iCorGain_clip1)
#define SWREG_PQDS_COR_GAIN_0               (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iCorGain_clip0)
#define SWREG_PQDS_GAIN_2                   (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iGain_clip2)
#define SWREG_PQDS_GAIN_1                   (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iGain_clip1)
#define SWREG_PQDS_GAIN_0                   (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iGain_clip0)
#ifdef TDSHP_2_0
//6797 new reg
#define SWREG_PQDS_HIGH_COR_ZERO_2          (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iHighCorZero_clip2)
#define SWREG_PQDS_HIGH_COR_ZERO_1          (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iHighCorZero_clip1)
#define SWREG_PQDS_HIGH_COR_ZERO_0          (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iHighCorZero_clip0)
#define SWREG_PQDS_HIGH_COR_THR_2           (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iHighCorThr_clip2)
#define SWREG_PQDS_HIGH_COR_THR_1           (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iHighCorThr_clip1)
#define SWREG_PQDS_HIGH_COR_THR_0           (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iHighCorThr_clip0)
#define SWREG_PQDS_HIGH_COR_GAIN_2          (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iHighCorGain_clip2)
#define SWREG_PQDS_HIGH_COR_GAIN_1          (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iHighCorGain_clip1)
#define SWREG_PQDS_HIGH_COR_GAIN_0          (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iHighCorGain_clip0)

#define SWREG_PQDS_MID_COR_ZERO_2           (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iMidCorZero_clip2)
#define SWREG_PQDS_MID_COR_ZERO_1           (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iMidCorZero_clip1)
#define SWREG_PQDS_MID_COR_ZERO_0           (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iMidCorZero_clip0)
#define SWREG_PQDS_MID_COR_THR_2            (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iMidCorThr_clip2)
#define SWREG_PQDS_MID_COR_THR_1            (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iMidCorThr_clip1)
#define SWREG_PQDS_MID_COR_THR_0            (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iMidCorThr_clip0)
#define SWREG_PQDS_MID_COR_GAIN_2           (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iMidCorGain_clip2)
#define SWREG_PQDS_MID_COR_GAIN_1           (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iMidCorGain_clip1)
#define SWREG_PQDS_MID_COR_GAIN_0           (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iMidCorGain_clip0)

#define SWREG_PQDS_EDF_FLAT_TH_2            (DISP_COLOR_SWREG_PQDS_BASE + PQDS_i_edf_flat_th_clip2)
#define SWREG_PQDS_EDF_FLAT_TH_1            (DISP_COLOR_SWREG_PQDS_BASE + PQDS_i_edf_flat_th_clip1)
#define SWREG_PQDS_EDF_FLAT_TH_0            (DISP_COLOR_SWREG_PQDS_BASE + PQDS_i_edf_flat_th_clip0)
#define SWREG_PQDS_EDF_DETAIL_RISE_TH_2     (DISP_COLOR_SWREG_PQDS_BASE + PQDS_i_edf_detail_rise_th_clip2)
#define SWREG_PQDS_EDF_DETAIL_RISE_TH_1     (DISP_COLOR_SWREG_PQDS_BASE + PQDS_i_edf_detail_rise_th_clip1)
#define SWREG_PQDS_EDF_DETAIL_RISE_TH_0     (DISP_COLOR_SWREG_PQDS_BASE + PQDS_i_edf_detail_rise_th_clip0)

//UltraResolution registers
#define SWREG_PQDS_ULTRARES_EN              (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iUltraRes_en)
#define SWREG_PQDS_ULTRARES_RATIO_THR_2     (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iUltraRes_ratio_thr2)
#define SWREG_PQDS_ULTRARES_RATIO_THR_1     (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iUltraRes_ratio_thr1)
#define SWREG_PQDS_ULTRARES_RATIO_THR_0     (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iUltraRes_ratio_thr0)
#define SWREG_PQDS_ULTRARES_GAIN_MID_1      (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iUltraRes_gain_mid_clip1)
#define SWREG_PQDS_ULTRARES_GAIN_MID_0      (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iUltraRes_gain_mid_clip0)
#define SWREG_PQDS_ULTRARES_GAIN_HIGH_1     (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iUltraRes_gain_high_clip1)
#define SWREG_PQDS_ULTRARES_GAIN_HIGH_0     (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iUltraRes_gain_high_clip0)
#define SWREG_PQDS_ULTRARES_EDF_EDGE_GAIN_1 (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iUltraRes_edf_edge_gain_clip1)
#define SWREG_PQDS_ULTRARES_EDF_EDGE_GAIN_0 (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iUltraRes_edf_edge_gain_clip0)
#define SWREG_PQDS_ULTRARES_CLIP_THR_1      (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iUltraRes_clip_thr_clip1)
#define SWREG_PQDS_ULTRARES_CLIP_THR_0      (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iUltraRes_clip_thr_clip0)
#define SWREG_PQDS_ULTRARES_CLIP_RATIO_1    (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iUltraRes_clip_ratio_clip1)
#define SWREG_PQDS_ULTRARES_CLIP_RATIO_0    (DISP_COLOR_SWREG_PQDS_BASE + PQDS_iUltraRes_clip_ratio_clip0)
#endif
#define SWREG_PQDS_END                      (DISP_COLOR_SWREG_PQDS_BASE + PQDS_INDEX_MAX)


#define ASHMEM_TUNING_FLAG                  (0x00)
#endif
