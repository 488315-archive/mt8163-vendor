/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/
/**
 * @file af_mgr.h
 * @brief AF manager, do focusing for raw sensor.
 */
#ifndef _AF_MGR_H_
#define _AF_MGR_H_
#include <isp_tuning.h>
#include <af_feature.h>
#include <iopipe/CamIO/INormalPipe.h>
#include <lib3a/af_algo_if.h>
#include <aaa_hal_if.h>
#include <pd_buf_common.h>
#include <laser_drv.h>
#include <vector>
#include <mcu_drv.h>
#include <drv/isp_drv.h>
#include <debug_exif/aaa/dbg_aaa_param.h>
#include <utils/Vector.h>


class NvramDrvBase;
using namespace android;
using namespace NSIoPipe;
using namespace NSCamIOPipe;
using namespace std;

namespace NS3Av3
{

//define command interface is called by host or af_mgr internally.
#define AF_MGR_CALLER 0
#define AF_CMD_CALLER 1

//--- pre-tuned scenechange parameters start ---
#define SENSOR_ACCE_SCALE 100
#define SENSOR_GYRO_SCALE 100

/* new part ; format chgT|chgN|stbT|stbN */
// thres unit: scale 100
const static MINT32 SCENE_GYRO_LEVEL[3]     = { 40051012,  40031010,  20031007};
const static MINT32 SCENE_GYRO_VID_LEVEL[3] = { 40071015,  40051013,  20051010};// video mode cnt > normal mode cnt for stable
const static MINT32 SCENE_ACCE_LEVEL[3]     = { 80155012,  80125010,  60125007};
const static MINT32 SCENE_ACCE_VID_LEVEL[3] = { 80155015,  80125013,  60125010};// video mode cnt > normal mode cnt for stable
// thres unit: % ; chgT and chgN from org part
const static MINT32 SCENE_AEBLOCK_CHG_THR_DIFF = 10; // to add based on GS chgT
const static MINT32 SCENE_AEBLOCK_CHG_CNT_DIFF = 5;  // to add based on GS chgN
const static MINT32 SCENE_GS_LEVEL[3]          = {      512,       510,       507};
const static MINT32 SCENE_GS_VID_LEVEL[3]      = {      515,       513,       510};// video mode cnt > normal mode cnt for stable
const static MINT32 SCENE_AEBLOCK_LEVEL[3]     = {     1012,      1010,      1007};
const static MINT32 SCENE_AEBLOCK_VID_LEVEL[3] = {     1015,      1013,      1010};// video mode cnt > normal mode cnt for stable

/* org part */
const static MINT32 SCENE_FV_CHG_THR_LEVEL_DIFF[3] = { 0,  0, -10};  // chgT lower to sensitive
const static MINT32 SCENE_FV_CHG_CNT_LEVEL_DIFF[3] = { 3,  0,   0};  // chgN bigger to stable
const static MINT32 SCENE_FV_STB_THR_LEVEL_DIFF[3] = { 0,  0,   0};  // stbT should fix
const static MINT32 SCENE_FV_STB_CNT_LEVEL_DIFF[3] = { 5,  0,  -5};  // stbN bigger to stable
const static MINT32 SCENE_GS_CHG_THR_LEVEL_DIFF[3] = { 0,  0,  -5};  // chgT lower to sensitive
const static MINT32 SCENE_GS_CHG_CNT_LEVEL_DIFF[3] = { 3,  0,   0};  // chgN bigger to stable
//--- pre-tuned scenechange parameters end ---


/* frame information */
typedef struct AF_FRM_INFO_t
{
    MINT32 i4FrmNum;  // magic number
    MINT32 i4RawFmt;  // raw format
    MINT32 i4LensPos; // lens position

    AF_FRM_INFO_t()
    {
        i4FrmNum  = 0;
        i4RawFmt  = 0;
        i4LensPos = 0;
    }
    AF_FRM_INFO_t( MINT32 FrmNum, MINT32 RawFmt, MINT32 LensPos)
    {
        i4FrmNum  = FrmNum;
        i4RawFmt  = RawFmt;
        i4LensPos = LensPos;
    }
} AF_FRM_INFO_T;

/* non-valatlize information. */
typedef struct AF_NON_VOLATILIZE_INFO_t
{
    MINT32 i4AFMode;
    LENS_INFO_T sLensPos;
    LIB3A_AF_MODE_T eLib3AAFMode;

    AF_NON_VOLATILIZE_INFO_t()
    {
        memset( &sLensPos, 0, sizeof(LENS_INFO_T));
        i4AFMode = MTK_CONTROL_AF_MODE_CONTINUOUS_PICTURE;
        eLib3AAFMode = LIB3A_AF_MODE_AFC;
    }
} AF_NON_VOLATILIZE_INFO_T;

/* This struct is uesed to convert DMA buffer */
typedef struct
{
    MUINT32 byte_00_03;
    MUINT32 byte_04_07;
    MUINT32 byte_08_11;
    MUINT32 byte_12_15;
} AF_HW_STAT_T;


/* Focus distance information. */
typedef struct AF_FOCUS_DIS_t
{
    MINT32 i4LensPos;
    MFLOAT i4Dist;

    AF_FOCUS_DIS_t()
    {
        i4LensPos = 0;
        i4Dist    = 0.33;
    }
} AF_FOCUS_DIS_T;


typedef enum
{
    EVENT_CMD_CHANGE_MODE = 0,
    EVENT_CMD_AUTOFOCUS,
    EVENT_CMD_AUTOFOCUS_CANCEL,
    EVENT_SEARCHING_START,
    EVENT_SEARCHING_END,

} AF_EVENT_T;



/**
 * @brief AF manager class
 */
class AfMgr
{
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                                 Ctor/Dtor.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
private:
    /*  Copy constructor is disallowed. */
    AfMgr( AfMgr const&);

    /*  Copy-assignment operator is disallowed. */
    AfMgr& operator=( AfMgr const&);

public:
    AfMgr( ESensorDev_T eSensorDev);
    virtual ~AfMgr();

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                                  Operations.
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
    /**
     * @Brief :
     *          Get AF manager instance.
     * @Param :
     *          [ in] i4SensorDev - sensor devices.
     * @Return:
     *          [out] return instance.
     */
    static AfMgr& getInstance( MINT32 const i4SensorDev);

    /**
     * @Brief :
     *          Initial AF manager functions.
     * @Param :
     *          [ in] i4SensorIdx - sensor index.
     *          [ in] isInitMCU - initial MCU or not.
     * @Return:
     *          [out] status
     */
    MRESULT init( MINT32 i4SensorIdx, MINT32 isInitMCU);

    /**
     * @Brief :
     *          After initial af_mgr, start af_mgr is required.
     * @Param :
     *          [ in] NA
     * @Return:
     *          [out] NA
     */
    MBOOL Start();

    /**
     * @Brief :
     *          Do power on related task.
     * @Param :
     *          [ in] NA
     * @Return:
     *          [out] NA
     */
    MBOOL CamPwrOnState();

    /**
     * @Brief :
     *          Do power off related task.
     * @Param :
     *          [ in] NA
     * @Return:
     *          [out] NA
     */
    MBOOL CamPwrOffState();

    /**
     * @Brief :
     *          Before uninitial af_mgr, stop af_mgr is required.
     * @Param :
     *          [ in] NA
     * @Return:
     *          [out] status
     */
    MBOOL Stop();

    /**
     * @Brief :
     *          Uninitial AF manager functions.
     * @Param :
     *          [ in] isInitMCU - uninitial MCU or not.
     * @Return:
     *          [out] status
     */
    MRESULT uninit( MINT32 isInitMCU);

    /**
     * @Brief :
     *          Command from 3a hal. set AF focusing area.
     * @Param :
     *          [ in] a_sAFArea - input focusing area.
     * @Return:
     *          [out] status
     */
    MRESULT setAFArea( CameraFocusArea_T &sInAFArea);

    /**
     * @Brief :
     *          Command from 3a hal.
     *          set crop region coordinate accroding to TG coordinate.
     *          This command is also used in af_mgr.
     * @Param :
     *          [  in] u4XOffset - X axis start address.
     *          [  in] u4YOffset - Y axis start address.
     *          [  in] u4Width   - width of crop region.
     *          [  in] u4Height  - height of crop region.
     *          [  in] u4Caller  - This flag is used to caller is internal af_mgr or host.
     * @Return:
     *          [out] status
     */
    MRESULT SetCropRegionInfo( MUINT32 u4XOffset, MUINT32 u4YOffset, MUINT32 u4Width, MUINT32 u4Height, MUINT32 u4Caller);

    /**
     * @Brief :
     *          command from 3a hal.
     *          set AF triggerAF.
     *          This command is also used in af_mgr.
     * @Param :
     *          [ in] u4Caller - This flag is used to caller is internal af_mgr or host.
     * @Return:
     *          [out] status
     */
    MRESULT triggerAF( MUINT32 u4Caller);

    /**
     * @Brief :
     *          command from 3a hal.
     *          Auto focus command.
     *          At auto mode, this command doesn't trigger AF. 3A HAL flow will call triggerAF once AE is stable.
     *          At continuous mode, flow is followed standard.
     * @Param :
     *          [ in] NA
     * @Return:
     *          [out] NA
     */
    MVOID autoFocus();

    /**
     * @Brief :
     *          command from 3a hal.
     *          Cancel auto focus command which is implemented and followed standard.
     * @Param :
     *          [ in] NA
     * @Return:
     *          [out] NA
     */
    MVOID cancelAutoFocus();

    /**
     * @Brief :
     *          command from 3a hal.
     *          set AF mode.
     *          This command is also used in af_mgr.
     * @Param :
     *          [ in] a_eAFMode - AF mode..
     *          [ in] u4Caller - This flag is used to caller is internal af_mgr or host.
     * @Return:
     *          [out] status
     */
    MRESULT setAFMode( MINT32 a_eAFMode, MUINT32 u4Caller);

    /**
     * @Brief :
     *          command from 3a hal.
     *          set manual focus position. When AF mode is MF, use it to set lens position.
     *          This command is also used in af_mgr.
     * @Param :
     *          [ in] a_i4Pos - Lens position. Usually value in 0~1023.
     *          [ in] u4Caller - This flag is used to caller is internal af_mgr or host.
     * @Return:
     *          [out] status
     */
    MRESULT setMFPos( MINT32 a_i4Pos, MUINT32 u4Caller);

    /**
     * @Brief :
     *          command from 3a hal.
     *          set current magic number.
     * @Param :
     *          [ in] i4FrmNum - magic number.
     * @Return:
     *          [out] NA
     */
    MVOID SetCurFrmNum( MUINT32 &i4FrmNum);

    /**
     * @Brief :
     *          command from 3a hal.
     *          Get debug information. For internal debug information.
     * @Param :
     *          [ in] rAFDebugInfo - debug information data pointer.;Please refer AF_DEBUG_INFO_T in dbg_af_param.h.
     * @Return:
     *          [out] status
     */
    MRESULT getDebugInfo( AF_DEBUG_INFO_T &rAFDebugInfo);

    /**
     * @Brief :
     *          command from 3a hal.
     *          AF HAL core flow.
     * @Param :
     *          [ in] ptrInAFData - Input statistic data buffer struct. [Ref] StatisticBufInfo in StatisticBuf.h
     * @Return:
     *          [out] status
     */
    MRESULT doAF( MVOID *ptrInAFData);


    /**
     * @Brief :
     *          command from 3a hal.
     *          Process PD virtual channel data.
     * @Param :
     *          [ in] ptrInVCData -  Input virtual channel data buffer struct. [Ref] StatisticBufInfo in StatisticBuf.h
     * @Return:
     *          [out] status
     */
    MRESULT doPDVC( MVOID *ptrInVCData);

    /**
     * @Brief :
     *          command from 3a hal.
     *          Process PDO data.
     * @param :
     *          [ in] ptrInPDOData -  Input PDO data buffer struct. [Ref] StatisticBufInfo in StatisticBuf.h
     * @Return:
     *          [out] status
     */
    MRESULT doPDO( MVOID *ptrInPDOData);

    /**
     * @Brief :
     *          command from 3a hal.
     *          Get AF state result. [Ref] Android define.
     * @Param :
     *          [ in] NA
     * @Return:
     *          [out] result.
     */
    NS3A::EAfState_T getAFState();

    /**
     * @Brief :
     *          command from 3a hal.
     *          Get focus area result for AP display.
     * @Param :
     *          [out] vecOutPos - center coordinate of focusing area.
     *          [out] vecOutRes - The focusing area result.
     *          [out] i4OutSzW - The focusing area width.
     *          [out] i4OutSzH - The focusing area height.
     * @Return:
     *          [out] status.
     */
    MRESULT getFocusAreaResult( android::Vector<MINT32> &vecOutPos, android::Vector<MUINT8> &vecOutRes, MINT32 &i4OutSzW, MINT32 &i4OutSzH);

    /**
     * @Brief :
     *          command from 3a hal.
     *          Set face detecion result information.
     * @Param :
     *          [ in] a_sFaces - Face detection result. [Ref] MtkCameraFaceMetadata in faces.h.
     * @Return:
     *          [out] status.
     */
    MRESULT setFDInfo( MVOID *a_sFaces);

    /**
     * @Brief :
     *          command from 3a hal.
     *          Set object tracking result information.
     * @Param :
     *          [ in] a_sObtinfo - OT information. [Ref] MtkCameraFaceMetadata in faces.h.
     * @Return:
     *          [out] status.
     */
    MRESULT setOTInfo( MVOID *a_sObtinfo);

    /**
     * @Brief :
     *          command from 3a hal.
     *          Check if AF is finished.
     * @Param :
     *          [ in] NA.
     * @Return:
     *          [out] 1 - Searching is finished.
     *                0 - Searching.
     */
    MINT32 isFocusFinish();

    /**
     * @Brief :
     *          command from 3a hal.
     *          Check if AF is focused success. This check can not be done before AF finished.
     * @Param :
     *          [ in] NA.
     * @Return:
     *          [out] 1 - scene is focused success.
     *                0 - scene cann't be focused.
     */
    MINT32 isFocused();

    /**
     * @Brief :
     *          command from 3a hal.
     *          get AF maximum area number.
     * @Param :
     *          [ in] NA.
     * @Return:
     *          [out] AF maximum area number.
     */
    MINT32 getAFMaxAreaNum();

    /**
     * @Brief :
     *          command from 3a hal.
     *          Timeout handle, when AF statistic is timeout or no response, this timeout function will be called and do something.
     * @Param :
     *          [ in] NA.
     * @Return:
     *          [out] NA.
     */
    MVOID TimeOutHandle();

    /**
     * @Brief :
     *          command from 3a hal.
     *          set sensor mode for PDAF virtual channel control.
     * @Param :
     *          [ in] i4NewSensorMode - Senriao.
     *          [ in] i4BINInfo_SzW - Binning size width, for configuring ISP3.0's statistic HW.
     *          [ in] i4BINInfo_SzH - Binning size height, for configuring ISP3.0's statistic HW.
     * @Return:
     *          [out] status.
     */
    MBOOL setSensorMode( MINT32 &i4NewSensorMode, MUINT32 &i4BINInfo_SzW, MUINT32 &i4BINInfo_SzH);

    /**
     * @Brief :
     *          command from 3a hal.
     *          Get AF ROI for getting AE related information.
     * @Param :
     *          [out] rWinSize - AF ROI.
     * @Return:
     *          [out] NA.
     */
    MVOID getAFRefWin( CameraArea_T &rWinSize);

    /**
     * @Brief :
     *          command from 3a hal.
     *          set AE information to Hybrid AF.
     * @Param :
     *          [ in] rAEInfo - AE related information.
     * @Return:
     *          [out] NA.
     */
    MVOID setAE2AFInfo( AE2AFInfo_T &rAEInfo);

    /**
     * @Brief :
     *          command from 3a hal.
     *          Get lens status to check lens is moving or not.
     * @Param :
     *          [ in] NA.
     * @Return:
     *          [out] status.
     */
    MINT32 getLensMoving();

    /**
     * @Brief :
     *          command from 3a hal.
     *          Set multi zone function as enable or disable.
     * @Param :
     *          [ in] bEn - MTRUE : enable, MFALSE : disable.
     * @Return:
     *          [out] status.
     */
    MINT32 setMultiZoneEnable( MBOOL &bEn);

    /**
     * @Brief :
     *          command from 3a hal.
     *          Get maximum lens position value, it gets AF MCU internal position value.
     *          return AF maximum lens Position value, the macro boundary position value in MCU.
     * @Param :
     *          [ in] NA.
     * @Return:
     *          [out] AF maximum lens Position value.
     */
    MINT32 getMaxLensPos();

    /**
     * @Brief :
     *          command from 3a hal.
     *          Get minimum. lens position value, it gets AF MCU internal position value.
     *          return AF minimum lens position value, the infinite boundary position value in MCU.
     * @Param :
     *          [ in] NA.
     * @Return:
     *          [out] AF minimum lens position value.
     */
    MINT32  getMinLensPos();

    /**
     * @Brief :
     *          command from 3a hal.
     *          Set SGG Pre-gain. For AE control gain used.
     * @Param :
     *          [ in] i4SGG_PGAIN - SGG Pre-gain..
     * @Return:
     *          [out] NA.
     */
    MVOID setSGGPGN( MINT32 i4SGG_PGAIN);

    /**
     * @Brief :
     *          command from 3a hal.
     *          get the information if android is runing.
     *          If android not run, the service depends on it should be disable.
     * @Param :
     *          [ in] a_state - android service state.
     * @Return:
     *          [out] NA.
     */
    MVOID setAndroidServiceState( MBOOL a_state);

    /**
     * @Brief :
     *          command from 3a hal.
     *          Depth AF API.
     *          This function is used for Depth HAL.
     *          Return the buffer pointer to Depth HAL for updating Depth information.
     * @Param :
     *          [out] ptbl - data buffer pointer.
     * @Return:
     *          [out] state.
     */
    MINT32 getDAFtbl( MVOID **ptbl);

    /**
     * @Brief :
     *          command from 3a hal.
     *          Set full scan step..
     *          When full scan, this value sets the step interval between each scanning.
     *          i.e.  Current position 100, next 103, thus step is 3.
     * @Param :
     *          [ in] a_i4Step - Steps for scan interval.
     * @Return:
     *          [out] state.
     */
    MRESULT setFullScanstep( MINT32 a_i4Step);

    /**
     * @Brief :
     *          command from 3a hal.
     *          Get AF best position value.
     *          return Latest time AF searching result position value.
     * @Param :
     *          [ in] NA.
     * @Return:
     *          [out] AF searching result position value. Range : 0-1023
     */
    MINT32 getAFBestPos();

    /**
     * @Brief :
     *          command from 3a hal.
     *          Get AF current position value.
     *          return AF current position value
     * @Param :
     *          [ in] NA.
     * @Return:
     *          [out] AF current position value. Range : 0-1023
     */
    MINT32 getAFPos();

    /**
     * @Brief :
     *          command from 3a hal.
     *          Check if AF is stable..
     *          return AF stable value; if 1, AF is stable.; if 0, AF is not stable.
     * @Param :
     *          [ in] NA.
     * @Return:
     *          [out] AF stable value
     */
    MINT32 getAFStable();

    /**
     * @Brief :
     *          command from 3a hal.
     *          Get AF table Offset.
     *          return AF table offset value;
     *          It usually means the infinite position value.
     *          [Ref] i4Offset in lens_para_<MCU_Name>.cpp
     * @Param :
     *          [ in] NA.
     * @Return:
     *          [out] AF table offset value
     */
    MINT32 getAFTableOffset();

    /**
     * @Brief :
     *          command from 3a hal.
     *          get AF table macro index.
     *          return AF table macro index.
     *          An index for AF table, it denotes the macro position of AF table.
     *          [Ref] i4MacroNum in lens_para_<MCU_Name>.cpp
     * @Param :
     *          [ in] NA.
     * @Return:
     *          [out] AF table macro index.
     */
    MINT32 getAFTableMacroIdx();

    /**
     * @Brief :
     *          command from 3a hal.
     *          get AF table index number.
     *          return AF table index number.
     *          It means that usable AF table length, and it's define to be 30.
     *          [Ref] AF_TABLE_NUM in camera_custom_nvram.h
     * @Param :
     *          [ in] NA.
     * @Return:
     *          [out] AF table index number.
     */
    MINT32 getAFTableIdxNum();

    /**
     * @Brief :
     *          command from 3a hal.
     *          get AF Table.
     *          return AF table pointer.
     *          AF table is contains 30 entries.
     *          [Ref]lens_para_<MCU_Name>.cpp
     * @Param :
     *          [ in] NA.
     * @Return:
     *          [out] AF Table.
     */
    MVOID* getAFTable();

    /**
     * @Brief :
     *          command from 3a hal.
     *          Get AF full statistic result.
     *          This result is generated for auto-flicker used, and it does not work while AF.
     *          return AF full statistic result.
     *          [Ref] AF_FULL_STAT_T in af_param.h
     * @Param :
     *          [ in] NA.
     * @Return:
     *          [out] AF full statistic result.
     */
    AF_FULL_STAT_T getFLKStat();



    //===================================================================================================
    //===================================================================================================
    //===================================================================================================
    //===================================================================================================
    //===================================================================================================

    MRESULT Hal3AInitState( MINT32 i4SensorIdx,MINT32 isInitMCU);

    MRESULT Hal3AUninitState( MINT32 isInitMCU);


    /**
     * @brief Do AF task when af statistic result done.
     * @param [in] pAFStatBuf Buffer of statistic result.
     */
    MVOID doPDBuffer( MVOID * buffer, MINT32 w,MINT32 h, MINT32 stride, MUINT32 i4FrmNum=0);
    MINT32 PDPureRawInterval();
    MRESULT doPDTask();

    /**
     * @brief Set camera mode
     * @param [in] a_eCamMode Camera modes; please refer EAppMode in CamDefs.h
     */
    MRESULT setCamMode( MINT32 a_eCamMode);

    /**
     * @brief Set best shot configure. When continous shot, camera get AF statistic result for each picture. This function configures the floating window for it.
     */
    MRESULT setBestShotConfig();

    /**
     * @brief Calculate best shot value. When continous shot, camera must do this function to calculate AF statistic result.
     * @param [in] pAFStatBuf Buffer of statistic result.
     */
    MRESULT calBestShotValue( MVOID *pAFStatBuf);

    /**
     * @brief Get best shot value. After calculating best shot AF statistic result, use it to get value.
     * @return Best shot AF statistic result.
     */
    MINT64 getBestShotValue();

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                             Camera 3.0
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    MVOID   setAperture( MFLOAT lens_aperture);
    MFLOAT  getAperture();
    MVOID   setFilterDensity( MFLOAT lens_filterDensity);
    MFLOAT  getFilterDensity();
    MVOID   setFocalLength( MFLOAT lens_focalLength);
    MFLOAT  getFocalLength();
    MVOID   setFocusDistance( MFLOAT lens_focusDistance);
    MFLOAT  getFocusDistance();
    MVOID   setOpticalStabilizationMode (MINT32 ois_OnOff);
    MINT32  getOpticalStabilizationMode();
    MVOID   getFocusRange( MFLOAT *vnear, MFLOAT *vfar);
    MINT32  getLensState();

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                             CCT feature
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public:
    MRESULT CCTMCUNameinit( MINT32 i4SensorIdx);
    MRESULT CCTMCUNameuninit();
    MINT32  CCTOPAFOpeartion();
    MINT32  CCTOPMFOpeartion( MINT32 a_i4MFpos);
    MINT32  CCTOPAFGetAFInfo( MVOID *a_pAFInfo, MUINT32 *a_pOutLen);
    MINT32  CCTOPAFGetBestPos( MINT32 *a_pAFBestPos, MUINT32 *a_pOutLen);
    MINT32  CCTOPAFCaliOperation( MVOID *a_pAFCaliData, MUINT32 *a_pOutLen);
    MINT32  CCTOPAFSetFocusRange( MVOID *a_pFocusRange);
    MINT32  CCTOPAFGetFocusRange( MVOID *a_pFocusRange, MUINT32 *a_pOutLen);
    MINT32  CCTOPAFGetNVRAMParam( MVOID *a_pAFNVRAM, MUINT32 *a_pOutLen);
    MINT32  CCTOPAFApplyNVRAMParam( MVOID *a_pAFNVRAM);
    MINT32  CCTOPAFSaveNVRAMParam();
    MINT32  CCTOPAFGetFV( MVOID *a_pAFPosIn, MVOID *a_pAFValueOut, MUINT32 *a_pOutLen);
    MINT32  CCTOPAFEnable();
    MINT32  CCTOPAFDisable();
    MINT32  CCTOPAFGetEnableInfo( MVOID *a_pEnableAF, MUINT32 *a_pOutLen);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                           Private functions
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
private:
    /**
     * @Brief :
     *          send command to normal pipe which is used in af mgr internal.
     * @Param :
     *          [ in] cmd - define in normal pipe.
     *          [ in] arg1 - define in normal pipe.
     *          [ in] arg2 - define in normal pipe.
     *          [ in] arg3 - define in normal pipe.
     * @Return:
     *          [out] send command status
     */
    MBOOL sendAFNormalPipe( MINT32 cmd, MINTPTR arg1, MINTPTR arg2, MINTPTR arg3);

    /**
     * @Brief :
     *          This function is used for AF factory calibration. It executes:  1.Read sensor One Time Programmable(OTP) memory. 2. Calculate and apply the factory data to AF table.
     * @Param :
     *          [ in] NA.
     * @Return:
     *          [out] status
     */
    MRESULT readOTP();

    /**
     * @Brief :
     *          Convert AF statistic data format from DMA.
     * @Param :
     *          [ in] i4CurPos      - Current lens position.
     *          [ in] ptrInStatBuf - Statistic buffer.
     *          [out] sOutSata      - AF statistic profile for hybrid AF
     * @Return:
     *          [out] NA
     */
    MVOID ConvertDMABufToStat( MINT32 &i4CurPos, MVOID *ptrInStatBuf, AF_STAT_PROFILE_T &sOutSata);

    /**
     * @Brief :
     *          Sum all af static data.
     * @Param :
     *          [ in] sInSataProfile - input AF statistic profile.
     * @Return:
     *          [out] result.
     */
    MINT64 TransStatProfileToAlgo( AF_STAT_PROFILE_T &sInSataProfile);

    /**
     * @Brief :
     *          Do Hybrid AF Pre-processing
     * @Param :
     *          [ in] NA.
     * @Return:
     *          [out] NA.
     */
    MVOID HybridAFPreprocessing();

    /**
     * @Brief :
     *          config Hybrid AF Algo mode.
     * @Param :
     *          [ in] NA.
     * @Return :
     *          [out] NA.
     */
    MVOID ConfigHybridAFMode();

    /**
     * @print HW configure struct.
     * @param :
     *        [ in] sAFHWCfg - af configure struct.
     * @Return :
     *        [out] NA..
     */
    MVOID PrintHWRegSetting( AF_CONFIG_T &sAFHWCfg);

    /**
     * @Brief :
     *          configure hw register to isp_mgr_af_stat.
     * @Param :
     *          [ in] sInHWCfg  - af configure which is outputed from alogorithm.
     *          [out] sOutHWROI - output ROI coordinate which is applied HW min constraint.
     *          [out] i4OutHWBlkNumX - output HW block X number which is applied HW min constraint.
     *          [out] i4OutHWBlkNumY - output HW block Y number which is applied HW min constraint.
     * @Return :
     *          [out] MFALSE-No new HW configure request.
     *                MTRUE -New HW configure should be applied.
     */
    MBOOL ConfigHWReg( AF_CONFIG_T &sInHWCfg, AREA_T &sOutHWROI, MINT32 &i4OutHWBlkNumX, MINT32 &i4OutHWBlkNumY);

    /**
     * @Brief :
     *          Apply zoom effect to ROI.
     * @Param :
     *          [ in] sOutAFRegion - output AF region coordinate which is applied zoom effect.
     * @Return:
     *          [out] NA.
     */
    MVOID ApplyZoomEffect( AREA_T &sOutAFRegion);

    /**
     * @Brief :
     *          set OT or FD information to hybrid AF algorithm.
     * @Param :
     *          [ in] sInROIs - input ROIs.
     *          [ in] i4Type - [0]FD [other]OT.
     * @Return:
     *          [out] setstatus..
     */
    MRESULT setOTFDInfo( MVOID* sInROIs, MINT32 i4Type);

    /**
     * @Brief :
     *          set OT or FD information to hybrid AF algorithm.
     * @Param :
     *          [ in] sInAFInfo - input hybrid AF output struct.
     *          [out] sOutFocusingROI - output selected ROI.
     * @Return:
     *          [out] return selected ROI
     */
    AREA_T& SelROIToFocusing( AF_OUTPUT_T &sInAFInfo);

    /**
     * @Brief :
     *          Move les to target position and return time stamp.
     * @Param :
     *          [ in] i4TargetPos - input target position.
     * @Return:
     *          [out] return time stamp
     */
    MINT64 MoveLensTo( MINT32 &i4TargetPos);

    /**
     * @Brief :
     *          This function is used to get lens information which is return by MCU driver.
     * @Param :
     *          [ in] a_rLensInfo - input target position.
     * @Return:
     *          [out] status
     */
    MRESULT getLensInfo( LENS_INFO_T &a_rLensInfo);

    /**
     * @Brief :
     *          get time stamp.
     * @Param :
     *          [ in] NA
     * @Return:
     *          [out] time stamp in ms
     */
    MINT64 getTimeStamp();

    /**
     * @Brief :
     *          update the analyzed window which is for PD algorithm.
     * @Param :
     *          [ in] sInArea - The focusing ROI which is output from Hybrid AF.(For print debug message only.)
     *          [ in] sInHWArea - ROI coordinate which is applied HW min constraint.
     * @Return:
     *          [out] NA
     */
    MVOID UpdatePDParam( AREA_T &sInArea, AREA_T &sInHWArea);

    /**
     * @Brief :
     *          write af_mgr debug information.
     * @Param :
     *          [ in] rAFDebugInfo - debug information data pointer.;Please refer AF_DEBUG_INFO_T in dbg_af_param.h.
     *          [ in] i4InIdx - write to index.
     *          [ in] i4InTag - tag of debug information.
     *          [ in] i4InVal - value of tag.
     *          [ in] i4InLineKeep - line feed control.
     * @Return:
     *          [out] status
     */
    MRESULT WDBGInfo( AF_DEBUG_INFO_T &sOutDbgInfo, MUINT32 i4InIdx, MUINT32 i4InTag, MUINT32 i4InVal, MUINT32 i4InLineKeep);

    /**
     * @Brief :
     *          Get af_mgr debug information. For internal debug information.
     * @Param :
     *          [ in] rAFDebugInfo - debug information data pointer.;Please refer AF_DEBUG_INFO_T in dbg_af_param.h.
     * @Return:
     *          [out] status
     */
    MRESULT GetMgrDbgInfo( AF_DEBUG_INFO_T &sOutDbgInfo);


    /**
     * @Brief :
     *          update af state at MF mode by event.
     * @Param :
     *          [ in] eInCurSate - current af state.
     *          [ in] sInEvent - event.
     * @Return:
     *          [out] new af state.
     */
    NS3A::EAfState_T UpdateStateMFMode( NS3A::EAfState_T &eInCurSate, AF_EVENT_T &sInEvent);

    /**
     * @Brief :
     *          update af state at OFF mode by event.
     * @Param :
     *          [ in] eInCurSate - current af state.
     *          [ in] sInEvent - event.
     * @Return:
     *          [out] new af state.
     */
    NS3A::EAfState_T UpdateStateOFFMode( NS3A::EAfState_T &eInCurSate, AF_EVENT_T &sInEvent);

    /**
     * @Brief :
     *          update af state at CONTINUOUS-VIDEO mode by event.
     * @param :
     *          [ in] eInCurSate - current af state.
     *          [ in] sInEvent - event.
     * @Return:
     *          [out] new af state.
     */
    NS3A::EAfState_T UpdateStateContinuousVideoMode( NS3A::EAfState_T &eInCurSate, AF_EVENT_T &sInEvent);

    /**
     * @Brief :
     *          update af state at CONTINUOUS-PICTURE mode by event.
     * @param :
     *          [ in] eInCurSate - current af state.
     *          [ in] sInEvent - event.
     * @Return:
     *          [out] new af state.
     */
    NS3A::EAfState_T UpdateStateContinuousPicMode( NS3A::EAfState_T &eInCurSate, AF_EVENT_T &sInEvent);

    /**
     * @Brief :
     *          update af state at AUTO mode by event.
     * @Param :
     *          [ in] eInCurSate - current af state.
     *          [ in] sInEvent - event.
     * @Return:
     *          [out] new af state.
     */
    NS3A::EAfState_T UpdateStateAutoMode( NS3A::EAfState_T &eInCurSate, AF_EVENT_T &sInEvent);

    /**
     * @Brief :
     *          update af state at Full Scan mode by event.
     * @Param :
     *          [ in] eInCurSate - current af state.
     *          [ in] sInEvent - event.
     * @Return:
     *          [out] new af state.
     */
    NS3A::EAfState_T UpdateStateFullScanMode( NS3A::EAfState_T &eInCurSate, AF_EVENT_T &sInEvent);

    /**
     * @Brief :
     *          update AF state by event.
     * @Param :
     *          [ in] sInEvent - event.
     * @Return:
     *          [out] af state.
     */
    NS3A::EAfState_T UpdateState( AF_EVENT_T sInEvent);

    /**
     * @Brief :
     *          update center ROI.
     *          The ROI size is depended on processing as MZAF, PDAF or Contrast AF.
     * @Param :
     *          [out] sOutAreaCenter - center ROI.
     * @Return:
     *          [out] NA.
     */
    MVOID UpdateCenterROI( AREA_T &sOutAreaCenter);

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                                 member
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
public :
    /**
     *  Instance.
     */
    static AfMgr  *s_pAfMgr;

private :
    MUINT8 m_aeBlockV[25];
    MINT32 m_prePosition;

    // PDAF
    MVOID *m_pdaf_rawbuf;
    MINT32 m_pdaf_w;
    MINT32 m_pdaf_h;
    MINT32 m_pdaf_stride;
    MINT32 m_pdaf_raw_frmnum;
    //pre frame information
    vector< AF_FRM_INFO_T> m_vFrmInfo;

    //===================================================================================================
    //===================================================================================================
    //===================================================================================================
    //===================================================================================================
    //===================================================================================================

    /**
     *  Instance.
     */
    NS3A::IAfAlgo *m_pIAfAlgo;

    /**
     *  Flow control
     */
    mutable Mutex    m_Lock;
    volatile MINT32  m_i4Users;
    volatile MINT32  m_CCTUsers;
    LIB3A_AF_MODE_T  m_eLIB3A_AFMode;
    NS3A::EAfState_T m_eAFState;
    NS3A::EAfState_T m_eAFStatePre;
    IMetadata        m_sMetaData;
    NVRAM_LENS_PARA_STRUCT    m_sNVRam;      /* nv rame data which is copied from nvram driver */
    NVRAM_LENS_PARA_STRUCT   *m_ptrNVRam;    /* AF related NV ram pointer from nvram driver */
    AF_NON_VOLATILIZE_INFO_T  m_NonValInfo;  /* auto initial once instance is created. */    
    MBOOL   m_bGetMetaData; /* MTRUE : module's static meata data is read from sensor. */
    MBOOL   m_bPWrInit;     /* MTRUE : search, MFALSE : monitor*/
    MBOOL   m_bIsROICmd;    /* Control timing of triggering searching from host. */
    MBOOL   m_bLatchROI;    /* Control timing of latching ROI from host. */
    MBOOL   m_bRunPDEn;     /* PD algorithm can be calculating result after af algorithm is ready. */
    MBOOL   m_bLock;
    MBOOL   m_bMZEn; /* Multi zone. */
    MBOOL   m_bMZHostEn; /* Multi zone. */
    MBOOL   m_bIsFullScan;
    MUINT32 m_i4CurFrmNum;
    MINT32  m_i4IsMonitorFV_CurState; /* Record current status of "i4IsMonitorFV" which is output from algorithm*/
    MINT32  m_i4IsMonitorFV_PreState; /* Record previours status of "i4IsMonitorFV" which is output from algorithm*/
    MINT32  m_i4IsFocused;
    MINT32  m_i4CurLensPos;
    MINT32  m_i4PreLensPos;
    MINT32  m_i4EnableAF;
    MINT32  m_i4MFPos;
    MINT32  m_i4SensorIdx;
    MINT32  m_Pix_Id;
    MINT32  m_eCurrAFMode;
    MINT32  m_i4FullScanStep;   
    MINT64  m_i8MvLensTS; /* ms, time stamp as moving lens*/
    


    /**
     * Debug
     */
    MINT32 m_i4DgbLogLv;
    MINT32 m_i4DbgMotorDisable;
    MINT32 m_i4DbgMotorMPos;
    MINT32 m_i4DbgMotorMPosPre;

    /**
     * HybridAF IO
     */
    AF_INPUT_T  m_sAFInput;
    AF_OUTPUT_T m_sAFOutput;
    AF_PARAM_T  m_sAFParam;
    MBOOL       m_bAcceEn; /* g/gyro sensor listener handler and data*/
    MBOOL       m_bGyroEn;

    /**
     * PDAF
     */
    EPDBuf_Type_t m_PDBuf_Type;
    MBOOL         m_bPDVCTest;
    MINT32        m_i4PDResNum;
    MINT32        m_i4PDAreaNum;
    AREA_T        m_sArea_PD[AF_PSUBWIN_NUM];

    /**
     * Depth AF
     */
    DAF_TBL_STRUCT m_sDAF_TBL;
    MBOOL          m_bDAFEn;
    MUINT32        m_daf_distance;
    MUINT32        m_next_query_FrmNum;

    /**
     * Digital zoom controlling..
     */
    AREA_T  m_sCropRegionInfo;
    MUINT32 m_i4DZFactor; /* scale is 100, 1x=100, 2x=200 */
    MUINT32 m_i4DzWinCfg;
    MUINT32 m_i4RawSel;

    /**
     * Configure HW flow controlling.
     */
    AF_CONFIG_T m_sHWCfg;
    MINT32      m_i4HWBlkNumX;
    MINT32      m_i4HWBlkNumY;


    /**
     * Sensor information
     */
    MINT32 m_i4CurrSensorDev;
    MINT32 m_i4CurrSensorId;
    MINT32 m_i4SensorMode;
    MINT32 m_i4BINSzW;
    MINT32 m_i4BINSzH;
    MINT32 m_i4TGSzW;
    MINT32 m_i4TGSzH;
    MINT32 m_i4PDSensorType;

    /**
     * ROI Control :
     * All stored ROIs' coordinate is depended on TG size and applied Zoom effect.
     */
    // CameraFocusArea_T m_CameraFocusArea; /*Receive ROIs from Host command*/
    MINT32 m_i4ROISel_PreState;      /*Record previours status of "i4ROISel" which is output from algorithm*/
    AREA_T m_sArea_Focusing; /* Focusing area*/
    AREA_T m_sArea_Center;   /* Center area.*/
    AREA_T m_sArea_APCmd;    /* AP command.*/
    AREA_T m_sArea_OTFD;     /* OT/FD command.*/
    AREA_T m_sArea_HW;       /* Record the ROI coordinate which is applied HW min constraint.*/

    /**
     * Driver
     */
    MINT32       m_i4CurrLensId;
    MINT32       m_i4MvLensTo;
    LENS_INFO_T  m_sCurLensInfo;
    MCUDrv      *m_pMcuDrv; /* VCM  driver*/
    LaserDrv    *m_pLaserDrv; /* Laser driver*/
    INormalPipe *m_pPipe; /* Iopipe 2.0*/

    /**
     * AF_mgr information for host.
     */
    AF_FOCUS_DIS_T m_sFocusDis;
};

};  //  namespace NS3Av3
#endif // _AF_MGR_H_
