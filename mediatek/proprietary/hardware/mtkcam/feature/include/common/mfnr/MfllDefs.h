#ifndef __MFLLDEFS_H__
#define __MFLLDEFS_H__

/**
 *  The definition that maximum frames that MFLL supports
 */
#ifndef MFLL_MAX_FRAMES
#define MFLL_MAX_FRAMES                                 10
#endif

/**
 *  The defualt frame number that MFLL will blend
 */
#ifndef MFLL_BLEND_FRAME
#define MFLL_BLEND_FRAME                                6
#endif

/**
 *  The default frame number that MFLL will capture
 *  If not specify, generally it should be the same as frame-blend number
 */
#ifndef MFLL_CAPTURE_FRAME
#define MFLL_CAPTURE_FRAME                              MFLL_BLEND_FRAME
#endif

/**
 *  RWB sensor support mode, if the project is RWB sesnor, the default value of
 *  this option should be 1 (GPU CCM)
 *  Value:
 *      0: Not use CCM
 *      1: GPU CCM
 *      2: MDP CCM
 */
#ifndef MFLL_RWB_SUPPORT_MODE
#define MFLL_RWB_SUPPORT_MODE                           0
#endif

/**
 *  Memory Reduce Plan (MRP) mode.
 *  0: disabled
 *  1: enable MRP
 */
#ifndef MFLL_MRP_SUPPORT_MODE
#define MFLL_MRP_SUPPORT_MODE                           0
#endif

/**
 *  Parallel ME/MC
 *  0: sequential MEMC
 *  1: parallel MEMC
 */
#ifndef MFLL_MEMC_SUPPORT_MODE
#define MFLL_MEMC_SUPPORT_MODE                          1
#endif

/**
 *  If using parallel ME/MC, define this value to adjust how many threads that
 *  you want run for executing Motion Estimation and Compensation.
 */
#ifndef MFLL_MEMC_THREADS_NUM
#define MFLL_MEMC_THREADS_NUM                           3
#endif

/**
 *  The MC algorithm also supports (if does) multi-threads computing. This
 *  define is describing how many threads that MC algorithm can create.
 */
#ifndef MFLL_MC_THREAD_NUM
#define MFLL_MC_THREAD_NUM                              4
#endif

/**
 *  Dual Phase only supports with ZSD+MFLL
 *  0: disabled
 *  1: enabled
 */
#ifndef MFLL_DUALPHASE_SUPPORT_MODE
#define MFLL_DUALPHASE_SUPPORT_MODE                     0
#endif

/**
 *  If invoke logs
 */
#ifndef MFLL_LOG
#define MFLL_LOG                                        1
#endif

/**
 *  Define the percentage of ROI for BSS
 */
#ifndef MFLL_BSS_ROI_PERCENTAGE
#define MFLL_BSS_ROI_PERCENTAGE                         95
#endif

/**
 *  Bypass option
 *
 *  Programmer may use these option to bypass some operations, and register
 *  event listener to do the specified behaviors if necessary.
 */
#ifndef MFLL_DEBUG_BYPASS_ALLOCATE_RAW_BUFFER
#define MFLL_DEBUG_BYPASS_ALLOCATE_RAW_BUFFER           0
#endif

#ifndef MFLL_DEBUG_BYPASS_ALLOCATE_QYUV_BUFFER
#define MFLL_DEBUG_BYPASS_ALLOCATE_QYUV_BUFFER          0
#endif

#ifndef MFLL_DEBUG_BYPASS_ALLOCATE_YUV_BASE
#define MFLL_DEBUG_BYPASS_ALLOCATE_YUV_BASE             0
#endif

#ifndef MFLL_DEBUG_BYPASS_ALLOCATE_YUV_GOLDEN
#define MFLL_DEBUG_BYPASS_ALLOCATE_YUV_GOLDEN           0
#endif

#ifndef MFLL_DEBUG_BYPASS_ALLOCATE_YUV_WORKING
#define MFLL_DEBUG_BYPASS_ALLOCATE_YUV_WORKING          0
#endif

#ifndef MFLL_DEBUG_BYPASS_ALLOCATE_YUV_MC_WORKING
#define MFLL_DEBUG_BYPASS_ALLOCATE_YUV_MC_WORKING       0
#endif

#ifndef MFLL_DEBUG_BYPASS_ALLOCATE_YUV_MIXING
#define MFLL_DEBUG_BYPASS_ALLOCATE_YUV_MIXING           0
#endif

#ifndef MFLL_DEBUG_BYPASS_ALLOCATE_WEIGHTING
#define MFLL_DEBUG_BYPASS_ALLOCATE_WEIGHTING            0
#endif

#ifndef MFLL_DEBUG_BYPASS_ALLOCATE_MEMC
#define MFLL_DEBUG_BYPASS_ALLOCATE_MEMC                 0
#endif

#ifndef MFLL_DEBUG_BYPASS_ALLOCATE_POSTVIEW
#define MFLL_DEBUG_BYPASS_ALLOCATE_POSTVIEW             0
#endif

#ifndef MFLL_DEBUG_BYPASS_ALLOCATE_THUMBNAIL
#define MFLL_DEBUG_BYPASS_ALLOCATE_THUMBNAIL            0
#endif

#ifndef MFLL_DEBUG_BYPASS_ALLOCATE_JPEG
#define MFLL_DEBUG_BYPASS_ALLOCATE_JPEG                 0
#endif

#ifndef MFLL_DEBUG_BYPASS_CAPTURE
#define MFLL_DEBUG_BYPASS_CAPTURE                       0
#endif

#ifndef MFLL_DEBUG_BYPASS_BSS
#define MFLL_DEBUG_BYPASS_BSS                           0
#endif

#ifndef MFLL_DEBUG_BYPASS_ENCODE_YUV_BASE
#define MFLL_DEBUG_BYPASS_ENCODE_YUV_BASE               0
#endif

#ifndef MFLL_DEBUG_BYPASS_ENCODE_YUV_GOLDEN
#define MFLL_DEBUG_BYPASS_ENCODE_YUV_GOLDEN             0
#endif

#ifndef MFLL_DEBUG_BYPASS_MOTION_ESTIMATION
#define MFLL_DEBUG_BYPASS_MOTION_ESTIMATION             0
#endif

#ifndef MFLL_DEBUG_BYPASS_MOTION_COMPENSATION
#define MFLL_DEBUG_BYPASS_MOTION_COMPENSATION           0
#endif

#ifndef MFLL_DEBUG_BYPASS_BLENDING
#define MFLL_DEBUG_BYPASS_BLENDING                      0
#endif

#ifndef MFLL_DEBUG_BYPASS_MIXING
#define MFLL_DEBUG_BYPASS_MIXING                        0
#endif

#ifndef MFLL_DEBUG_BYPASS_NOISE_REDUCTION
#define MFLL_DEBUG_BYPASS_NOISE_REDUCTION               0
#endif

#ifndef MFLL_DEBUG_BYPASS_POSTVIEW
#define MFLL_DEBUG_BYPASS_POSTVIEW                      0
#endif

#ifndef MFLL_DEBUG_BYPASS_THUMBNAIL
#define MFLL_DEBUG_BYPASS_THUMBNAIL                     0
#endif

#ifndef MFLL_DEBUG_BYPASS_ENCODE_JPEG
#define MFLL_DEBUG_BYPASS_ENCODE_JPEG                   0
#endif

/* Avoid unused parameter warning */
#define MFLL_UNUSED(x) (void)(x)

#endif /* __MFLLDEFS_H__ */

